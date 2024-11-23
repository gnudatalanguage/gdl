/***************************************************************************
						  |FILENAME|  -  description
							 -------------------
	begin                : |DATE|
	copyright            : (C) |YEAR| by |AUTHOR|
	email                : |EMAIL|
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <string>       // std::string

#else

#include "envt.hpp"
#include "gdl2gdl.hpp"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <thread>
#include <fcntl.h>      /* O_flags */
#include <sys/mman.h>   /* shared memory and mmap() */
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "dinterpreter.hpp"

static bool atexit_already_done = false;
#define THEBUFLEN 4096
static char theBuf[THEBUFLEN];

#include "dinterpreter.hpp"
//for client gdl2gdl (see IDL_IDLBridge)
#if defined(__APPLE__)
#include <mach/mach.h>
#include <servers/bootstrap.h>
#else
#include <mqueue.h>
#endif

 messageBoxHandle gdl2gdlMessageBoxHandle; //client server queue descriptor

static int ClientSessionNumber=0;
static std::string g2gClientMailbox;
GDLEventQueue gdl2gdlCallbackQueue;
std::map<pid_t, gdl2gdlparams> g2gMap;

std::map<int, int> g2gListOfSharedMem;

static const std::string base{"_GMEM$"};

#endif

#if defined(_WIN32) && !defined(__CYGWIN__)

#else
void gdl_ipc_read_client_nowait(pid_t pid);
void gdl_ipc_read_client_wait(pid_t pid);

int gdl_ipc_sendCtrlCToClient(int pid) {
  return kill(pid, SIGINT);
}

static void g2gPerformCallbackProcedure(pid_t pid) {
  // push callback event if necessary
  DObjGDL* o = g2gMap.at(pid).obj;
  DStructGDL* self = BaseGDL::interpreter->GetObjHeap((*o)[0]);
  DStringGDL* callbackname=static_cast<DStringGDL*>(self->GetTag(3));
  StrUpCaseInplace((*callbackname)[0]);
  if(callbackname->NBytes() >0) {
	int proIx = GDLInterpreter::GetProIx((*callbackname)[0]);
	if (proIx != -1) {
	  // perform callback and reset status to IDLE at end of callback
	  DIntGDL* status = new DIntGDL(g2gMap.at(pid).status);
	  DStringGDL* error = new DStringGDL(g2gMap.at(pid).description); 
	  EnvUDT* newEnv = new EnvUDT(NULL, proList[ proIx], NULL);
	  newEnv->SetNextPar(status);
	  newEnv->SetNextPar(error);
	  newEnv->SetNextPar(o);

	  DPtrGDL* ptrgdl = static_cast<DPtrGDL*> (self->GetTag(5));
	  DPtr p = (*ptrgdl)[0];
	  if (BaseGDL::interpreter->PtrValid(p)) {
		BaseGDL* data = BaseGDL::interpreter->GetHeap(p);
		newEnv->SetNextPar(data);
	  }
	  BaseGDL::interpreter->CallStack().push_back(newEnv);
	  // make the call
	  BaseGDL::interpreter->call_pro(proList[ proIx]->GetTree());
	  BaseGDL::interpreter->CallStack().pop_back();

	  g2gMap.at(pid).status = 0;
	  g2gMap.at(pid).description.clear();
	  g2gMap.at(pid).nowait = false;
	}
  }
}

void g2gAsynchronousReturnTrap() {
  int status = -1;
  do {
    // wait for all sub-processes in the same process group to finish -> avoid zombies
    // Note: by default, wait also waits for sub-processes in another thread, so it seems fine to run this code in another thread
    if (waitpid(0, &status, WUNTRACED | WCONTINUED) == -1) {
      // we're out of luck, wait returns an error, so break to loop and exit
      std::cerr << "g2gAsynchronousReturnTrap exiting" << std::endl;
      break;
    }
    for (const auto& g2gMapIter : g2gMap) {
      //for (g2gMapIter g2gMapIter = g2gMap.begin(); g2gMapIter != g2gMap.end(); ++g2gMapIter) {
      if (g2gMapIter.second.status == 1 && g2gMapIter.second.nowait) gdl_ipc_read_client_nowait(g2gMapIter.first);
    }
    usleep (10000);
  } while (!WIFEXITED(status) && !WIFSIGNALED(status));
}

int gdl_ipc_write_to_client(EnvT* e, DLong* id, const std::string & command, bool nowait = true) {
  pid_t pid=id[2];
  // should start with no error
  g2gMap.at(pid).description.clear();
  auto l = command.length();
  g2gMap.at(pid).status = 1;
  g2gMap.at(pid).nowait=nowait;

  int status = write(id[1], command.c_str(), l);
  if (status != l) {e->Throw("Error in write command to subprocess:" + std::string(strerror(errno)));
	return 0;
  }

  if (!nowait) gdl_ipc_read_client_wait(id[2]);
  return 0;
}

//tidy the use of shared memory and message channel
void gmem_clean() {
  for (g2gSharedMemListIter it = g2gListOfSharedMem.begin(); it != g2gListOfSharedMem.end(); ++it) {
	std::string sharedId = base + i2s(getpid()) + "$" + i2s((*it).second);
	shm_unlink(sharedId.c_str());
  }
}

void AddGmemCleanToAtexit() {
  if (!atexit_already_done) {
	atexit_already_done = true;
	atexit(gmem_clean);
  }
}
// called from main eventloop. Reads g2gObjetcs events (could easily be extended to other event-needing objects)

void g2gEventDispatcher() {
  DStructGDL* ev;
  while ((ev = gdl2gdlCallbackQueue.Pop()) != NULL) { // get event
	DStringGDL* callbackname = static_cast<DStringGDL*> (ev->GetTag(0));
	//	std::cerr << "callback: " << (*callbackname)[0] << std::endl;
	int proIx = GDLInterpreter::GetProIx((*callbackname)[0]);
	if (proIx != -1) {
	  // perform callback and reset status to IDLE at end of callback
	  DIntGDL* status = static_cast<DIntGDL*> (ev->GetTag(1));
	  DStringGDL* error = static_cast<DStringGDL*> (ev->GetTag(2));
	  DObjGDL* o = static_cast<DObjGDL*> (ev->GetTag(3));
	  DStructGDL* self = BaseGDL::interpreter->GetObjHeap((*o)[0]);
	  DLongGDL* triplet = static_cast<DLongGDL*> (self->GetTag(6));
	  pid_t pid = (*triplet)[2];
	  EnvUDT* newEnv = new EnvUDT(NULL, proList[ proIx], NULL);
	  newEnv->SetNextPar(status);
	  newEnv->SetNextPar(error);
	  newEnv->SetNextPar(o);

	  DPtrGDL* ptrgdl = static_cast<DPtrGDL*> (ev->GetTag(4));
	  DPtr p = (*ptrgdl)[0];
	  if (BaseGDL::interpreter->PtrValid(p)) {
		BaseGDL* data = BaseGDL::interpreter->GetHeap(p);
		newEnv->SetNextPar(data);
	  }
	  BaseGDL::interpreter->CallStack().push_back(newEnv);
	  // make the call
	  BaseGDL::interpreter->call_pro(proList[ proIx]->GetTree());
	  BaseGDL::interpreter->CallStack().pop_back();
	  g2gMap.at(pid).status = 0;
	  g2gMap.at(pid).description.clear();
	  g2gMap.at(pid).nowait = false;
	}
  }
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
void gdl_ipc_ClientGetsMailboxAddress(std::string & s) {}
void gdl_ipc_ClientSignalsOperationsOK() {}
void gdl_ipc_ClientSendReturn(unsigned char status, std::string s){};
void gdl_ipc_ClientClosesMailBox(){};
#elif defined (__APPLE__)
messageBoxHandle StartIndividualClientMessageChannel(std::string & name){
     // Create a new port.
  messageBoxHandle port;
    kern_return_t kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    if (kr != KERN_SUCCESS) {
		fprintf(stderr,"mach_port_allocate() failed with code 0x%x\n", kr);
	    throw GDLException("gdl Server error");
        return 0;
    }
//    printf("mach_port_allocate() created port right name %d\n", port);


    // Give us a send right to this port, in addition to the receive right.
    kr = mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND);
    if (kr != KERN_SUCCESS) {
        printf("mach_port_insert_right() failed with code 0x%x\n", kr);
        throw GDLException("gdl Server error");
        return 0;
    }
//    printf("mach_port_insert_right() inserted a send right\n");
    //service registration
	name=G2G_SERVER_QUEUE_BASENAME+i2s(getpid())+"_"+i2s(++ClientSessionNumber);
	int l=name.size();
	name_t namet;
	strncpy(namet,name.c_str(),l);
    // Send the send right to the bootstrap server, so that it can be looked up by other processes.
    messageBoxHandle h;
    kr = bootstrap_register(bootstrap_port, namet, port  );
    if (kr != KERN_SUCCESS) {
        printf("bootstrap_register() failed with code 0x%x\n", kr);
	    throw GDLException("gdl Server error");
        return 0;
    }
//    printf("MASTER: bootstrap_register()'ed our Master Message Channel port\n");
    return port;
}
void gdl_ipc_ClientClosesMailBox(){
//  mach_port_destroy(mach_task_self(), MBOXName.c_str());
}
void gdl_ipc_ClientGetsMailboxAddress(std::string & s) {
    g2gClientMailbox=s;
    // Lookup the receiver port using the bootstrap server.
	int l=g2gClientMailbox.size();
	name_t name;
	strncpy(name,g2gClientMailbox.c_str(),l);
    kern_return_t kr = bootstrap_look_up(bootstrap_port, name, &gdl2gdlMessageBoxHandle);
    if (kr != KERN_SUCCESS) {
        printf("bootstrap_look_up() failed with code 0x%x\n", kr);
	    throw GDLException("gdl Server error");
        return;
    }
//    printf("CLIENT: bootstrap_look_up() returned MASTER port right name %d\n", gdl2gdlMessageBoxHandle);
  //here is a good point to start to be absolutely silent
      std::cout.rdbuf(NULL);
    std::cerr.rdbuf(NULL);
}
//client side
void gdl_ipc_ClientSignalsOperationsOK() {
     // Construct our message.
    struct {
        mach_msg_header_t header;
        int pid;
        mach_port_name_t handle;
    } message;

    message.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
    message.header.msgh_remote_port = gdl2gdlMessageBoxHandle;
    message.header.msgh_local_port = MACH_PORT_NULL;

    message.handle = gdl2gdlMessageBoxHandle; //gdl2gdlIndividualClientMessageBox;
	message.pid = getpid();

    // Send the message.
    kern_return_t kr = mach_msg(
        &message.header,  // Same as (mach_msg_header_t *) &message.
        MACH_SEND_MSG,    // Options. We're sending a message.
        sizeof(message),  // Size of the message being sent.
        0,                // Size of the buffer for receiving.
        MACH_PORT_NULL,   // A port to receive a message on, if receiving.
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL    // Port for the kernel to send notifications about this message to.
    );
    if (kr != KERN_SUCCESS) {
        printf("gdl_ipc_acknowledge_subprocess_started() mach_msg() failed with code 0x%x\n", kr);
  	    throw GDLException("gdl Server error");
        return;
    }
//  printf("client Sent first message\n");
}
//client side
void gdl_ipc_ClientSendReturn(unsigned char status, std::string s) {
	 int l=s.size(); l=(l > MSG_BUFFER_SIZE)?MSG_BUFFER_SIZE-1:l;
    // Construct our message.
    struct {
        mach_msg_header_t header;
        char some_text[MSG_BUFFER_SIZE];
        int status;
    } message;

    message.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
    message.header.msgh_remote_port = gdl2gdlMessageBoxHandle; //gdl2gdlIndividualClientMessageBox;
//	printf("Client %d will write to port %d\n", getpid(), gdl2gdlMasterMessageBox); //gdl2gdlIndividualClientMessageBox);
    message.header.msgh_local_port = MACH_PORT_NULL;

    strncpy(message.some_text, s.c_str(), l);
    message.status = status;

    // Send the message.
    kern_return_t kr = mach_msg(
        &message.header,  // Same as (mach_msg_header_t *) &message.
        MACH_SEND_MSG,    // Options. We're sending a message.
        sizeof(message),  // Size of the message being sent.
        0,                // Size of the buffer for receiving.
        MACH_PORT_NULL,   // A port to receive a message on, if receiving.
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL    // Port for the kernel to send notifications about this message to.
    );
    if (kr != KERN_SUCCESS) {
        printf("gdl_ipc_client_write() mach_msg() failed with code 0x%x\n", kr);
        return;
    }
//    printf("Sent a message\n");
	 //perform client "retall" if code is > 2 (error or cancel)
	 if (status < 3) return;
	static int proIx = LibProIx("RETALL");
    EnvT* env = new EnvT(NULL, libProList[proIx]);
    Guard<EnvT> env_guard(env);
    static_cast<DLibPro*>(env->GetPro())->Pro()(static_cast<EnvT*>(env));
}
//master side
void gdl_ipc_MasterWaitsForClientOK(pid_t pid){
    // Wait for a message.
    struct {
        mach_msg_header_t header;
		int pid;
        mach_port_name_t handle;
        mach_msg_trailer_t trailer;
    } message;

    kern_return_t kr = mach_msg(
        &message.header,  // Same as (mach_msg_header_t *) &message.
        MACH_RCV_MSG,     // Options. We're receiving a message.
        0,                // Size of the message being sent, if sending.
        sizeof(message),  // Size of the buffer for receiving.
        g2gMap.at(pid).MessageChannelHandle,             // The port to receive a message on.
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL    // Port for the kernel to send notifications about this message to.
    );
    if (kr != KERN_SUCCESS) {
        printf("gdl_ipc_wait_for_subprocess_started() mach_msg() failed with code 0x%x\n", kr);
	    throw GDLException("gdl Server error");
        return ;
    }
}
void gdl_ipc_read_client_wait(pid_t pid) {
  messageBoxHandle clientMessageBoxHandle=g2gMap.at(pid).MessageChannelHandle;
  struct {
	mach_msg_header_t header;
	char some_text[MSG_BUFFER_SIZE];
	int status;
	mach_msg_trailer_t trailer;
  } message;

  kern_return_t  kr = mach_msg(
	&message.header, // Same as (mach_msg_header_t *) &message.
	MACH_RCV_MSG, // Options. We're receiving a message.
	0, // Size of the message being sent, if sending.
	sizeof (message), // Size of the buffer for receiving.
	g2gMap.at(pid).MessageChannelHandle,
	MACH_MSG_TIMEOUT_NONE,
	MACH_PORT_NULL // Port for the kernel to send notifications about this message to.
	);
  if (kr != KERN_SUCCESS) {
	printf("gdl_ipc_read_client_wait() mach_msg() failed on port %d with code 0x%x\n",clientMessageBoxHandle, kr);
	throw GDLException("gdl Server error");
    return;
  }
  //        printf ("Server: message received: %d\n",message.pid);
  int status = message.status; //2,3, or 4
  //  std::cerr<<"\ngot something:"<<status<<std::endl;
  if (status < 2 || status > 4) throw GDLException("Wrong return from client program");
  g2gMap.at(pid).status=status;
  std::string r(message.some_text);
  g2gMap.at(pid).description=r;
  g2gPerformCallbackProcedure(pid);
}
void gdl_ipc_read_client_nowait(pid_t pid){
  mach_port_name_t clientMessageBoxHandle=g2gMap.at(pid).MessageChannelHandle;
  struct {
	mach_msg_header_t header;
	char some_text[MSG_BUFFER_SIZE];
	int status;
	mach_msg_trailer_t trailer;
  } message;

  kern_return_t kr = mach_msg(
	&message.header, // Same as (mach_msg_header_t *) &message.
	MACH_RCV_MSG, // Options. We're receiving a message.
	0, // Size of the message being sent, if sending.
	sizeof (message), // Size of the buffer for receiving.
	g2gMap.at(pid).MessageChannelHandle,
	1, //1 millisec
	MACH_PORT_NULL // Port for the kernel to send notifications about this message to.
	);
  if (kr == KERN_SUCCESS) {
  int status = message.status; //2,3, or 4
//  std::cerr<<"\ngot something:"<<status<<std::endl;
  if (status < 2 || status > 4) throw GDLException("Wrong return from client program");
  g2gMap.at(pid).status=status;
  std::string r(message.some_text);
  g2gMap.at(pid).description=r;
  g2gPerformCallbackProcedure(pid);
  } else if (kr == MACH_RCV_TIMED_OUT)  return;
  else throw GDLException("Server: error reading client message.");
}
void gdl_ipc_CloseMasterMasterMessageChannel(pid_t pid){
  mach_port_deallocate(mach_task_self(), gdl2gdlMessageBoxHandle);
}

#else
messageBoxHandle StartIndividualClientMessageChannel(std::string & name){

  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
     
  name=G2G_SERVER_QUEUE_BASENAME+i2s(getpid())+"_"+i2s(++ClientSessionNumber);
  messageBoxHandle h;
  if ((h = mq_open(name.c_str(), O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
   throw GDLException("Server: mq_open (client) :"+string(strerror(errno)));
  }
  return h;
}
void gdl_ipc_ClientClosesMailBox(){
  	  mq_close(gdl2gdlMessageBoxHandle);
	  mq_unlink(g2gClientMailbox.c_str());
}
void gdl_ipc_ClientGetsMailboxAddress(std::string & s) {
  g2gClientMailbox=s;
  if ((gdl2gdlMessageBoxHandle = mq_open(g2gClientMailbox.c_str(), O_WRONLY)) == -1) {
   throw GDLException("Server: mq_open (server) :"+string(strerror(errno)));
  }
  //here is a good point to start to be absolutely silent
  std::cout.rdbuf(NULL);
  std::cerr.rdbuf(NULL);
}
//client side
void gdl_ipc_ClientSignalsOperationsOK() {
     pid_t pid=getpid();
     char out_buffer [MSG_BUFFER_SIZE];
	 static int l=sizeof(pid_t);
	 memcpy(out_buffer,&pid,l);
//	std::cerr << "using handle " <<gdl2gdlMessageBoxHandle<<" for acknowledge"<<std::endl;
	 if (mq_send (gdl2gdlMessageBoxHandle, out_buffer, l + 1, 0) == -1) {
		throw GDLException("Client"+i2s(pid)+" is not able to send message to server, reason: "+string(strerror(errno)));
	}
}
//client side
void gdl_ipc_ClientSendReturn(unsigned char status, std::string s) {
     char out_buffer [MSG_BUFFER_SIZE];
	 int l=s.size()+1; l=(l > MSG_BUFFER_SIZE)?MSG_BUFFER_SIZE-1:l;
	 out_buffer[0]=status;
	 strncpy(&out_buffer[1],s.c_str(),l);
	std::cerr << "using handle " <<gdl2gdlMessageBoxHandle<<" for response"<<status<<std::endl;
	 if (mq_send (gdl2gdlMessageBoxHandle, out_buffer, l + 2, 0) == -1) {
		throw GDLException("Client "+i2s(getpid())+" is not able to send message to server, reason: "+string(strerror(errno)));
	}
	 //perform client "retall" if code is > 2 (error or cancel)
	 if (status < 3) return;
	static int proIx = LibProIx("RETALL");
    EnvT* env = new EnvT(NULL, libProList[proIx]);
    Guard<EnvT> env_guard(env);
    static_cast<DLibPro*>(env->GetPro())->Pro()(static_cast<EnvT*>(env));
 }
//master side
void gdl_ipc_MasterWaitsForClientOK(pid_t pid){
//  std::cerr << "wait for client "<<pid<<" started at handle " <<g2gMap.at(pid).MessageChannelHandle<<std::endl;
     char in_buffer [MSG_BUFFER_SIZE];
        if (mq_receive (g2gMap.at(pid).MessageChannelHandle, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
          throw GDLException("Server: mq_receive :"+string(strerror(errno)));
        }
}


// used for non-waiting executes, run in parallel (in g2gAsynchronousReturnTrap()) 
// with current EnvT of gdl, so MUST NOT interfere with it: just
// push an event in a queue (thread safe). Event will be processed in the MAIN MASTER LOOP
static void ReportUsingCallBack(pid_t pid) {
  // push callback event if necessary
  DObjGDL* o = g2gMap.at(pid).obj;
  DStructGDL* self = BaseGDL::interpreter->GetObjHeap((*o)[0]);
  DStringGDL* callbackproc = static_cast<DStringGDL*> (self->GetTag(3));
  if (callbackproc->NBytes() > 0) {
	StrUpCaseInplace((*callbackproc)[0]);
	DStructGDL* ev = new DStructGDL("GDL2GDL_CBK_EVENT");
	ev->InitTag("CALLBACKPROC", DStringGDL((*callbackproc)[0]));
	ev->InitTag("CALLBACKSTATUS", DIntGDL(g2gMap.at(pid).status));
	ev->InitTag("CALLBACKERROR", DStringGDL(g2gMap.at(pid).description));
	ev->InitTag("CALLBACKOBJECT", *o);
	ev->InitTag("CALLBACKUSERDATA", *(self->GetTag(5)));
	gdl2gdlCallbackQueue.PushBack(ev);
  }
}

void gdl_ipc_read_client_wait(pid_t pid){
     char* in_buffer=(char*) calloc(MSG_BUFFER_SIZE,1);
	 messageBoxHandle clientMessageBoxHandle=g2gMap.at(pid).MessageChannelHandle;
        if (mq_receive (clientMessageBoxHandle, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
          throw GDLException("Server: mq_receive :"+string(strerror(errno)));
        }
  int status = in_buffer[0]; //2,3, or 4
//  std::cerr<<"\nwait got something:"<<status<<std::endl;
  if (status < 2 || status > 4) throw GDLException("Wrong return from client program");
  g2gMap.at(pid).status=status;
  std::string r(&in_buffer[1]);
  g2gMap.at(pid).description=r;
  g2gPerformCallbackProcedure(pid);
}

void gdl_ipc_read_client_nowait(pid_t pid) {
  char* in_buffer = (char*) calloc(MSG_BUFFER_SIZE, 1);
  timespec timeout;
  timeout.tv_nsec = 0;
  timeout.tv_sec = 0;
  messageBoxHandle clientMessageBoxHandle = g2gMap.at(pid).MessageChannelHandle;
  if (mq_timedreceive(clientMessageBoxHandle, in_buffer, MSG_BUFFER_SIZE, NULL, &timeout) == -1) {
	if (errno != ETIMEDOUT) {
          throw GDLException("Server: mq_receive :"+string(strerror(errno)));
	} else {
	  return;
	}
  }
  //it has already returned something
  int status = in_buffer[0]; //2,3, or 4
//  std::cerr<<"\nnowait got something:"<<status<<std::endl;
  if (status < 2 || status > 4) throw GDLException("Wrong return from client program");
  g2gMap.at(pid).status=status;
  std::string r(&in_buffer[1]);
  g2gMap.at(pid).description=r;
  ReportUsingCallBack(pid); //just report (can be done in a thread), will be 'treated' by master loop in GDL.
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#else
namespace lib {
  // create a shared memory location containing a copy of variable v
  // if a shared memory file of sufficient size exist, use it
  // else remove it and create a larger one with tag number increased.
  // name based on master process pid
  // master : gm_setvar,id,name,value

  void gmem_setvar(EnvT* e) {
	SizeT nParam = e->NParam(3);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	//insure versioning of shared memory
	// key is subprocess id (triplet[2])
	int subpid = (*triplet)[2];
	if (g2gMap.at(subpid).status == 1) e->Throw("Error: The object's associated IDL process is currently busy.");

	BaseGDL* p1 = e->GetParDefined(1);
	if (p1->Type() != GDL_STRING) e->Throw(e->GetParString(1) + " must be a string");
	DStringGDL* s = e->GetParAs<DStringGDL>(1);
	BaseGDL* p2 = e->GetParDefined(2);
	if (!ConvertableType(p2->Type())) e->Throw("type of " + e->GetParString(2) + " is not permitted.");

	SizeT nbytes = p2->NBytes();
	int type = p2->Type();
	int dims[8];
	int rank = p2->Rank();
	for (int i = 0; i < 8; ++i) dims[i] = 1;
	for (int i = 0; i < rank; ++i) dims[i] = p2->Dim(i);
	int n = sizeof (int);
	int nbig = sizeof (SizeT);
	int offset = 10 * n + nbig;
	int shm_fd; /* file descriptor */
	int exist_perms = S_IRUSR | S_IWUSR;
	int create_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

	std::string sharedId;
	for (g2gSharedMemListIter it = g2gListOfSharedMem.begin(); it != g2gListOfSharedMem.end(); ++it) {
	  if ((*it).first == subpid) {
		int version = (*it).second;
		sharedId = base + i2s(getpid()) + "$" + i2s(version);
		//		std::cout << "found:" << sharedId << std::endl;
		break;
	  }
	}
	if (sharedId.empty()) { //not found, create one, one more than max of list
	  //max of list
	  int version = 0;
	  for (g2gSharedMemListIter it = g2gListOfSharedMem.begin(); it != g2gListOfSharedMem.end(); ++it) if ((*it).second > version) version = (*it).second;
	  version++;
	  sharedId = base + i2s(getpid()) + "$" + i2s(version);
	  g2gListOfSharedMem.insert(std::pair<int, int>(subpid, version));
	  //	  std::cout << "creating:" << sharedId << std::endl;
	  // create the mapped region and put var in it
	  shm_fd = shm_open(sharedId.c_str(), O_RDWR, exist_perms); //test existence
	  if (shm_fd != -1) { //file exist. this is not an error
		// will just truncate
	  } else { //:create
		int oflags = O_RDWR | O_CREAT | O_EXCL; /* open flags receives -c, -x, -t */
		//try to open unexisting:
		shm_fd = shm_open(sharedId.c_str(), oflags, create_perms);
		if (shm_fd == -1) e->Throw("Mapping segment " + sharedId + " failed."); //this is an error
	  }
	  int status = ftruncate(shm_fd, nbytes + offset);
	  if (status != 0) e->Throw("Shared Memory Segment " + sharedId + " creation failed (size), reason: " + std::string(strerror(errno)) + ".");
	} else { //found, but is size sufficient?
	  shm_fd = shm_open(sharedId.c_str(), O_RDWR, exist_perms); //test existence
	  if (shm_fd == -1) { // unexistent - well, create again
		int oflags = O_RDWR | O_CREAT | O_EXCL; /* open flags receives -c, -x, -t */
		//try to open unexisting:
		shm_fd = shm_open(sharedId.c_str(), oflags, create_perms);
		if (shm_fd == -1) e->Throw("Mapping segment " + sharedId + " failed."); //this is an error
	  }
	  int status = ftruncate(shm_fd, nbytes + offset);
	  if (status != 0) e->Throw("Shared Memory Segment " + sharedId + " creation failed (size), reason: " + std::string(strerror(errno)) + ".");
	}
	//get address
	void* mapAddress = mmap(NULL, nbytes + offset, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	close(shm_fd); //"After a call to mmap(2) the file descriptor may be closed without affecting the memory mapping."
	if (mapAddress == MAP_FAILED) e->Throw("shmmap failed, please report.");
	//copy variable
	memcpy(mapAddress, &type, n);
	memcpy((char*) (mapAddress) + n, &rank, n);
	memcpy((char*) (mapAddress) + 2 * n, dims, 8 * n);
	memcpy((char*) (mapAddress) + 10 * n, &nbytes, nbig);
	memcpy((char*) (mapAddress) + offset, p2->DataAddr(), nbytes);
	msync(mapAddress, nbytes, MS_SYNC);

	munmap(mapAddress, nbytes + offset); //unmap

	DString command = (*s)[0] + "=gmem_subprocess_receivevar('" + sharedId + "')\n";
	int status = gdl_ipc_write_to_client(e, &((*triplet)[0]), command, false);

	AddGmemCleanToAtexit(); //will suppress shared sections at exit
  }

  // slave : gm_write,handle,variable

  void gmem_subprocess_givevar(EnvT* e) {
	SizeT nParam = e->NParam(2);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->Type() != GDL_STRING) e->Throw(e->GetParString(0) + " must be a string");
	DStringGDL* handleGDL = e->GetParAs<DStringGDL>(0);
	DString handle = (*handleGDL)[0];
	BaseGDL* p1 = e->GetParDefined(1);
	if (!ConvertableType(p1->Type())) e->Throw("type of " + e->GetParString(1) + " is not permitted.");

	SizeT nbytes = p1->NBytes();
	int type = p1->Type();
	int dims[8];
	int rank = p1->Rank();
	for (int i = 0; i < 8; ++i) dims[i] = 1;
	for (int i = 0; i < rank; ++i) dims[i] = p1->Dim(i);
	int n = sizeof (int);
	int nbig = sizeof (SizeT);
	int offset = 10 * n + nbig;
	int shm_fd; /* file descriptor */
	int exist_perms = S_IRUSR | S_IWUSR;

	shm_fd = shm_open(handle.c_str(), O_RDWR, exist_perms); //test existence
	if (shm_fd != -1) { //file exist. this is not an error
	  // will just truncate
	} else {
	  e->Throw("Mapping segment " + handle + " failed."); //this is an error
	}
	int status = ftruncate(shm_fd, nbytes + offset);
	//get address
	void* mapAddress = mmap(NULL, nbytes + offset, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	close(shm_fd); //"After a call to mmap(2) the file descriptor may be closed without affecting the memory mapping."
	if (mapAddress == MAP_FAILED) e->Throw("shmmap failed, please report.");
	//copy variable
	memcpy(mapAddress, &type, n);
	memcpy((char*) (mapAddress) + n, &rank, n);
	memcpy((char*) (mapAddress) + 2 * n, dims, 8 * n);
	memcpy((char*) (mapAddress) + 10 * n, &nbytes, nbig);
	memcpy((char*) (mapAddress) + offset, p1->DataAddr(), nbytes);
	msync(mapAddress, nbytes, MS_SYNC);
	munmap(mapAddress, nbytes + offset); //unmap
  }

  //master: var=gmem_getvar(id,name)

  BaseGDL* gmem_getvar(EnvT* e) {
	SizeT nParam = e->NParam(2);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	//insure versioning of shared memory
	// key is subprocess id (triplet[2])
	int subpid = (*triplet)[2];
	if (g2gMap.at(subpid).status == 1) e->Throw("Error: The object's associated IDL process is currently busy.");
	BaseGDL* p1 = e->GetParDefined(1);
	if (p1->Type() != GDL_STRING) e->Throw(e->GetParString(1) + " must be a string");
	DStringGDL* child_varname = e->GetKWAs<DStringGDL>(1);
	DString child_var = (*child_varname)[0];


	// get adequate shared memory handle
	std::string sharedId;
	int shm_fd; /* file descriptor */
	for (g2gSharedMemListIter it = g2gListOfSharedMem.begin(); it != g2gListOfSharedMem.end(); ++it) {
	  if ((*it).first == subpid) {
		int version = (*it).second;
		sharedId = base + i2s(getpid()) + "$" + i2s(version);
		//		std::cout << "found:" << sharedId << std::endl;
		break;
	  }
	}
	if (sharedId.empty()) { //not found, create one, one more than max of list
	  //max of list
	  int version = 0;
	  for (g2gSharedMemListIter it = g2gListOfSharedMem.begin(); it != g2gListOfSharedMem.end(); ++it) if ((*it).second > version) version = (*it).second;
	  version++;
	  sharedId = base + i2s(getpid()) + "$" + i2s(version);
	  g2gListOfSharedMem.insert(std::pair<int, int>(subpid, version));
	  //	  std::cout << "creating:" << sharedId << std::endl;
	  // create the mapped region
	  int exist_perms = S_IRUSR | S_IWUSR;
	  shm_fd = shm_open(sharedId.c_str(), O_RDWR, exist_perms); //test existence
	  if (shm_fd != -1) { //file exist. this is not an error
		// will just truncate
	  } else { //:create
		int oflags = O_RDWR | O_CREAT | O_EXCL; /* open flags receives -c, -x, -t */
		//try to open unexisting:
		int create_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
		shm_fd = shm_open(sharedId.c_str(), oflags, create_perms);
		if (shm_fd == -1) e->Throw("Mapping segment " + sharedId + " failed."); //this is an error
	  }
	  int status = ftruncate(shm_fd, 1024); //set to 1024 . Why not?
	  if (status != 0) e->Throw("Shared Memory Segment " + sharedId + " creation failed (size), reason: " + std::string(strerror(errno)) + ".");
	  close(shm_fd); //will be reopened
	  AddGmemCleanToAtexit(); //will suppress shared sections at exit
	}

	DString command = "gmem_subprocess_givevar,'" + sharedId + "'," + child_var + "\n";
	int status = gdl_ipc_write_to_client(e, &((*triplet)[0]), command, false);
	//read exported var

	int exist_perms = S_IRUSR | S_IWUSR;
	shm_fd = shm_open(sharedId.c_str(), O_RDWR, exist_perms);
	struct stat statbuf;
	fstat(shm_fd, &statbuf);
	SizeT length = statbuf.st_size;
	void* mapAddress = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	close(shm_fd); //"After a call to mmap(2) the file descriptor may be closed without affecting the memory mapping."
	if (mapAddress == MAP_FAILED) e->Throw("shmmap failed, please report.");
	//copy variable
	int type;
	int dims[8];
	int rank;
	int n = sizeof (int);
	int nbig = sizeof (SizeT);
	int offset = 10 * n + nbig;
	SizeT nbytes;
	memcpy(&type, mapAddress, n);
	memcpy(&rank, (char*) (mapAddress) + n, n);
	memcpy(dims, (char*) (mapAddress) + 2 * n, 8 * n);
	memcpy(&nbytes, (char*) (mapAddress) + 10 * n, nbig);

	SizeT k = dims[0];
	dimension theDim;
	for (int i = 0; i < 8; ++i) {
	  k = dims[i];
	  theDim << k;
	}

	BaseGDL *var;
	switch (type) {
	case GDL_BYTE: //	Byte
	  var = new DByteGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_INT: //	16-bit Integer 
	  var = new DIntGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_LONG: //	32-bit Long Integer 
	  var = new DLongGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_FLOAT: //	32-bit Floating Point Number 
	  var = new DFloatGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_DOUBLE: //	64-bit Floating Point Number 
	  var = new DDoubleGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_COMPLEX: //	Complex Floating Point Number (32-bits each) 
	  var = new DComplexGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_STRING: //	String
	  var = new DStringGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_COMPLEXDBL: //	Complex Floating Point Number (64-bits each) 
	  var = new DComplexDblGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_UINT: //	16-bit Unsigned Integer 
	  var = new DUIntGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_ULONG: //	32-bit Unsigned Integer 
	  var = new DULongGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_LONG64: //	64-bit Integer 
	  var = new DLong64GDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_ULONG64: //	64-bit Unsigned Integer 
	  var = new DULong64GDL(theDim, BaseGDL::NOZERO);
	  break;
	default: //	0 ? Undefined (not allowed) 
	  e->Throw("Should not happen: struct");
	  break;
	}
	memcpy(var->DataAddr(), (char*) (mapAddress) + offset, nbytes);
	munmap(mapAddress, length); //unmap
	return var;
  }

  //slave: var=gmem_read(handle)

  BaseGDL* gmem_subprocess_receivevar(EnvT* e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->Type() != GDL_STRING) e->Throw(e->GetParString(0) + " must be a string");
	DStringGDL* s = e->GetKWAs<DStringGDL>(0);
	DString handle = (*s)[0];
	int shm_fd; /* file descriptor */
	int exist_perms = S_IRUSR | S_IWUSR;
	shm_fd = shm_open(handle.c_str(), O_RDWR, exist_perms);
	struct stat statbuf;
	fstat(shm_fd, &statbuf);
	SizeT length = statbuf.st_size;
	void* mapAddress = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	close(shm_fd); //"After a call to mmap(2) the file descriptor may be closed without affecting the memory mapping."
	if (mapAddress == MAP_FAILED) e->Throw("shmmap failed, please report.");
	//copy variable
	int type;
	int dims[8];
	int rank;
	int n = sizeof (int);
	int nbig = sizeof (SizeT);
	int offset = 10 * n + nbig;
	SizeT nbytes;
	memcpy(&type, mapAddress, n);
	memcpy(&rank, (char*) (mapAddress) + n, n);
	memcpy(dims, (char*) (mapAddress) + 2 * n, 8 * n);
	memcpy(&nbytes, (char*) (mapAddress) + 10 * n, nbig);

	SizeT k = dims[0];
	dimension theDim;
	for (int i = 0; i < 8; ++i) {
	  k = dims[i];
	  theDim << k;
	}

	BaseGDL *var;
	switch (type) {
	case GDL_BYTE: //	Byte
	  var = new DByteGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_INT: //	16-bit Integer 
	  var = new DIntGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_LONG: //	32-bit Long Integer 
	  var = new DLongGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_FLOAT: //	32-bit Floating Point Number 
	  var = new DFloatGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_DOUBLE: //	64-bit Floating Point Number 
	  var = new DDoubleGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_COMPLEX: //	Complex Floating Point Number (32-bits each) 
	  var = new DComplexGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_STRING: //	String
	  var = new DStringGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_COMPLEXDBL: //	Complex Floating Point Number (64-bits each) 
	  var = new DComplexDblGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_UINT: //	16-bit Unsigned Integer 
	  var = new DUIntGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_ULONG: //	32-bit Unsigned Integer 
	  var = new DULongGDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_LONG64: //	64-bit Integer 
	  var = new DLong64GDL(theDim, BaseGDL::NOZERO);
	  break;
	case GDL_ULONG64: //	64-bit Unsigned Integer 
	  var = new DULong64GDL(theDim, BaseGDL::NOZERO);
	  break;
	default: //	0 ? Undefined (not allowed) 
	  e->Throw("Should not happen: struct");
	  break;
	}
	memcpy(var->DataAddr(), (char*) (mapAddress) + offset, nbytes);
	munmap(mapAddress, length); //unmap
	return var;
  }

  BaseGDL* gmem_status(EnvT* e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid = (*triplet)[2];
	static int ERROR = e->KeywordIx("ERROR");
	std::string s;
	if (e->KeywordPresent(ERROR)) e->SetKW(ERROR, new DStringGDL(g2gMap.at(pid).description));
	int status=g2gMap.at(pid).status;
	if(status>1) {
	//status() resets status to 0 when called.
	  g2gMap.at(pid).status=0;
	  g2gMap.at(pid).description.clear();
	}
	return new DIntGDL(status);
  }

  //gmem_send,pipefd,"command",/NOWAIT
  //gmem_send must always trigger the callback, wait or nowait.

  void gmem_send(EnvT* e) {
	SizeT nParam = e->NParam(2);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	static int NOWAIT = e->KeywordIx("NOWAIT");

	pid_t pid = (*triplet)[2];
	if (g2gMap.at(pid).status == 1) e->Throw("Error: The object's associated IDL process is currently busy.");

	BaseGDL* p1 = e->GetParDefined(1);
	if (p1->Type() != GDL_STRING) e->Throw(e->GetParString(1) + " must be a string");

	DStringGDL* s = e->GetParAs<DStringGDL>(1);
	DString command = (*s)[0] + "\n";

	int status = gdl_ipc_write_to_client(e, &((*triplet)[0]), command, e->KeywordSet(NOWAIT));
  }

  void gmem_abort(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid=(*triplet)[2];
    g2gMap.at(pid).status = 1;
    g2gMap.at(pid).nowait=false;
	gdl_ipc_sendCtrlCToClient(pid); //make ^C
	gdl_ipc_read_client_wait(pid); //synchronously wait for subprocess halted. (not sure IDL does that)
  }

  BaseGDL* gmem_fork(EnvT* e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->Type() != GDL_OBJ) e->Throw("I need an object.");
	DObjGDL* o = e->GetParAs<DObjGDL>(0);

	int write_pipe[2];
	int read_pipe[2];
	pid_t subprocess_pid;

	if (pipe(write_pipe) == -1) {
	  perror("pipe");
	  exit(EXIT_FAILURE);
	}
	if (pipe(read_pipe) == -1) {
	  perror("pipe");
	  exit(EXIT_FAILURE);
	}
// create message box & get name
	std::string passed_name;
    messageBoxHandle id=StartIndividualClientMessageChannel(passed_name);
//	std::cerr << "creating handle " <<id<<" for "<<passed_name <<std::endl;
	//now 2 processes
	subprocess_pid = fork();
	if (subprocess_pid == -1) {
	  perror("fork");
	  exit(EXIT_FAILURE);
	}
	
	if (subprocess_pid == 0) { //child
	  close(write_pipe[1]); /* Close unused write end */
	  close(read_pipe[0]); /* Close unused read end */
	  close(0);
	  dup2(write_pipe[0], 0);
	  close(1);
	  close(2);
	  dup2(read_pipe[1], 1);
	  dup2(read_pipe[1], 2);
	  std::string me=whereami_gdl+"/gdl";
	  std::string subp="--subprocess";
	  if (-1 == execl(me.c_str(), me.c_str(), subp.c_str(), passed_name.c_str(), (char  *)NULL)) {
		perror("child process execve failed [%m]");
		return new DLongGDL(0);
	  }
//	  printf("closing fd %d\n", write_pipe[0]);
//	  printf("subprocess %d exited\n", getpid());
	  close(write_pipe[0]);
	  close(read_pipe[1]);
	  _exit(EXIT_SUCCESS);
	  return NULL; //DUMMY
	} else {
	  close(write_pipe[0]); /* Close unused read end */
	  close(read_pipe[1]); /* Close unused write end */
	  DLongGDL* triplet = new DLongGDL(dimension(3));
	  (*triplet)[0] = read_pipe[0];
	  (*triplet)[1] = write_pipe[1];
	  (*triplet)[2] = subprocess_pid;
	  gdl2gdlparams params;
	  params.description.clear();
	  params.status=0;
	  params.obj=o;
	  bool startspy = (g2gMap.empty()) ; //will start a spy detached thread for NOWAIT operations, that will end itself when g2GMap is empty
	  g2gMap.insert(std::pair<pid_t,gdl2gdlparams>(subprocess_pid,params));
	  g2gMap.at(subprocess_pid).MessageChannelHandle=id;
	  // insure communication with child is OK waiting for a status change
      gdl_ipc_MasterWaitsForClientOK(subprocess_pid);
	  //  start eventually spy process
	  if (startspy)  std::thread(g2gAsynchronousReturnTrap).detach();
      return triplet;
	}

  }

  // gmem_exit,id : cleans the shared mem & objectlist, called by IDL_IDLBridge::Cleanup
 // MASTER only
  void gmem_exit(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid = (*triplet)[2];


	if (g2gMap.at(pid).status == 1) { //interrupt the process
	  gdl_ipc_sendCtrlCToClient(pid);
	  g2gMap.at(pid).status = 4; //aborted
	  g2gPerformCallbackProcedure(pid);
	}
	//remove shared mem and pid, object, close messagebox
	g2gSharedMemListIter it = g2gListOfSharedMem.find(pid);
	if (it != g2gListOfSharedMem.end()) {
	  std::string sharedId = base + i2s(getpid()) + "$" + i2s((*it).second);
	  shm_unlink(sharedId.c_str());
	  g2gListOfSharedMem.erase(pid);
	}

	DString command = "EXIT\n";
	size_t l=command.size();
	int status = write((*triplet)[1], command.c_str(), l);
    g2gMap.erase(pid);
  }
}
#endif
