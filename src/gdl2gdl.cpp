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
#else
#include "envt.hpp"
#include "gdl2gdl.hpp"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <fcntl.h>      /* O_flags */
#include <sys/mman.h>   /* shared memory and mmap() */
#include <sys/shm.h>
#include <sys/stat.h>
#include "dinterpreter.hpp"

static bool atexit_already_done = false;
#define THEBUFLEN 4096
static char theBuf[THEBUFLEN];

#include "dinterpreter.hpp"
//for client gdl2gdl (see IDL_IDLBridge)
#if defined(__APPLE__)
mach_port_t gdl2gdlMasterMessageBox; //client server queue descriptor

//#elif defined(_WIN32) && !defined(__CYGWIN__)

#else
#include <mqueue.h>
mqd_t gdl2gdlMasterMessageBox; //client server queue descriptor
#endif

pid_t g2gParentPid;
GDLEventQueue gdl2gdlCallbackQueue;
std::map<pid_t, gdl2gdlparams> g2gMap;

std::map<int, int> g2gListOfSharedMem;

static const std::string base{"_GMEM$"};
#endif

// stub for windows -- one other time
#if defined(_WIN32) && !defined(__CYGWIN__)
void StartMasterMessageChannel(){}
void AttachToMasterMessageChannel() {}
//client side
void gdl_ipc_acknowledge_suprocess_started(long long pid) {}
//master side
int gdl_ipc_wait_for_subprocess_started(int pid){return 0;}
int gdl_ipc_sendsignalToParent()    {return 0;}
#elif defined (__APPLE__)
void StartMasterMessageChannel(){
     // Create a new port.
    kern_return_t kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &gdl2gdlMasterMessageBox);
    if (kr != KERN_SUCCESS) {
        printf("mach_port_allocate() failed with code 0x%x\n", kr);
        return;
    }
    printf("mach_port_allocate() created port right name %d\n", gdl2gdlMasterMessageBox);


    // Give us a send right to this port, in addition to the receive right.
    kr = mach_port_insert_right(mach_task_self(), gdl2gdlMasterMessageBox, gdl2gdlMasterMessageBox, MACH_MSG_TYPE_MAKE_SEND);
    if (kr != KERN_SUCCESS) {
        printf("mach_port_insert_right() failed with code 0x%x\n", kr);
        return;
    }
    printf("mach_port_insert_right() inserted a send right\n");

    std::string g2gServerName=G2G_SERVER_QUEUE_BASENAME+i2s(g2gParentPid);
	int l=g2gServerName.size();
	name_t name;
	strncpy(name,g2gServerName.c_str(),l);
    // Send the send right to the bootstrap server, so that it can be looked up by other processes.
    kr = bootstrap_register(bootstrap_port, name, gdl2gdlMasterMessageBox);
    if (kr != KERN_SUCCESS) {
        printf("bootstrap_register() failed with code 0x%x\n", kr);
        return;
    }
    printf("bootstrap_register()'ed our port\n");
 
}
void AttachToMasterMessageChannel() {
    // Lookup the receiver port using the bootstrap server.
    std::string g2gServerName=G2G_SERVER_QUEUE_BASENAME+i2s(g2gParentPid);
	int l=g2gServerName.size();
	name_t name;
	strncpy(name,g2gServerName.c_str(),l);
    kern_return_t kr = bootstrap_look_up(bootstrap_port, name, &gdl2gdlMasterMessageBox);
    if (kr != KERN_SUCCESS) {
        printf("bootstrap_look_up() failed with code 0x%x\n", kr);
        return;
    }
    printf("bootstrap_look_up() returned port right name %d\n", gdl2gdlMasterMessageBox);
}
//client side
void gdl_ipc_acknowledge_suprocess_started(pid_t pid) {
     // Construct our message.
    struct {
        mach_msg_header_t header;
        char some_text[10];
        int pid;
    } message;

    message.header.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);
    message.header.msgh_remote_port = gdl2gdlMasterMessageBox;
    message.header.msgh_local_port = MACH_PORT_NULL;

    strncpy(message.some_text, "Hello", sizeof(message.some_text));
    message.pid = pid;

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
        printf("mach_msg() failed with code 0x%x\n", kr);
        return;
    }
    printf("Sent a message\n");
}
//master side
pid_t gdl_ipc_wait_for_subprocess_started(){
    // Wait for a message.
    struct {
        mach_msg_header_t header;
        char some_text[10];
        int pid;
        mach_msg_trailer_t trailer;
    } message;

    kern_return_t kr = mach_msg(
        &message.header,  // Same as (mach_msg_header_t *) &message.
        MACH_RCV_MSG,     // Options. We're receiving a message.
        0,                // Size of the message being sent, if sending.
        sizeof(message),  // Size of the buffer for receiving.
        gdl2gdlMasterMessageBox,             // The port to receive a message on.
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL    // Port for the kernel to send notifications about this message to.
    );
    if (kr != KERN_SUCCESS) {
        printf("mach_msg() failed with code 0x%x\n", kr);
        return 1;
    }
        printf ("Server: message received: %d\n",message.pid);
		return message.pid;
}
#else
void StartMasterMessageChannel(){

  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_MESSAGES;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;
     
  std::string g2gServerName=G2G_SERVER_QUEUE_BASENAME+i2s(g2gParentPid);

  if ((gdl2gdlMasterMessageBox = mq_open(g2gServerName.c_str(), O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
	perror("Server: mq_open (server)");
	exit(1);
  }
  //qd client not (yet) used.
 
}
void AttachToMasterMessageChannel() {
  //open master 
  std::string g2gServerName = G2G_SERVER_QUEUE_BASENAME + i2s(g2gParentPid);

  if ((gdl2gdlMasterMessageBox = mq_open(g2gServerName.c_str(), O_WRONLY)) == -1) {
	perror("Client: mq_open (server)");
	exit(1);
  }
  //here is a good point to start to be absolutely silent
  std::cout.rdbuf(NULL);
  std::cerr.rdbuf(NULL);
}
//client side
void gdl_ipc_acknowledge_suprocess_started(pid_t pid) {
     char out_buffer [MSG_BUFFER_SIZE];
	 static int l=sizeof(pid_t);
	 memcpy(out_buffer,&pid,l);
	 if (mq_send (gdl2gdlMasterMessageBox, out_buffer, l + 1, 0) == -1) {
		perror ("Client: Not able to send message to server");
	}
}
//master side
pid_t gdl_ipc_wait_for_subprocess_started(){
     char in_buffer [MSG_BUFFER_SIZE];
        if (mq_receive (gdl2gdlMasterMessageBox, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }
	 pid_t pid;
	 static int l=sizeof(pid_t);
	 memcpy(&pid,in_buffer,l);
        printf ("Server: message received: %d\n",pid);
		return pid;
		//in_buffer can be used to open a communication to child, qd_client
}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)

void DefineG2GParentPid(int pid) {}

#else
void DefineG2GParentPid(pid_t pid) {
  if (pid == 0) g2gParentPid = getpid(); else g2gParentPid = pid;
}

int gdl_ipc_sendsignalToParent() {
  return kill(getppid(), GDL_SIGUSR2);
}

int gdl_ipc_sendCtrlCToChild(int pid) {
  return kill(pid, SIGINT);
}

int gdl_ipc_sendsignalToChild(int pid) {
  return kill(pid, GDL_SIGUSR1);
}

int gdl_ipc_SetReceiverForChildSignal(void (*handler) (int sig, siginfo_t *siginfo, void *context)) {
  struct sigaction siga;
  memset(&siga, 0, sizeof (struct sigaction)); //no complaints by Valgrind
  siga.sa_sigaction = *handler; //*ChildSignalHandler;
  siga.sa_flags = SA_SIGINFO; // get detail info
  return sigaction(GDL_SIGUSR2, &siga, NULL);
}

void HandleObjectsCallbacks() {
  DStructGDL* ev;
  while ((ev = gdl2gdlCallbackQueue.Pop()) != NULL) { // get event
	DStringGDL* callbackname = static_cast<DStringGDL*> (ev->GetTag(0));
//	std::cerr << (*callbackname)[0] << std::endl;
	DIntGDL* status = static_cast<DIntGDL*> (ev->GetTag(1));
	DStringGDL* error = static_cast<DStringGDL*> (ev->GetTag(2));
	DObjGDL* o = static_cast<DObjGDL*> (ev->GetTag(3));
	DPtrGDL* ptrgdl = static_cast<DPtrGDL*> (ev->GetTag(4));
	BaseGDL* data = BaseGDL::interpreter->GetHeap((*ptrgdl)[0]);
	int proIx = GDLInterpreter::GetProIx((*callbackname)[0]);
	if (proIx != -1) {
	  EnvUDT* newEnv = new EnvUDT(NULL, proList[ proIx], NULL);
	  newEnv->SetNextPar(status);
	  newEnv->SetNextPar(error);
	  newEnv->SetNextPar(o);
	  newEnv->SetNextPar(data);
	  BaseGDL::interpreter->CallStack().push_back(newEnv);
	  // make the call
	  BaseGDL::interpreter->call_pro(proList[ proIx]->GetTree());
	  BaseGDL::interpreter->CallStack().pop_back();
	}
  }
}

static void ChildSignalHandlerWithCallBack(int sig, siginfo_t *siginfo, void *context) {
  // get pid of sender,
  pid_t pid = siginfo->si_pid;
//    std::cout << SysVar::MsgPrefix() << "Signal from child " << pid << std::endl;
  g2gMap.at(pid).status = 2; //child just completed command
  // push callback event if necessary
  DObjGDL* o = g2gMap.at(pid).obj;
  DStructGDL* self = BaseGDL::interpreter->GetObjHeap((*o)[0]);
  DStringGDL* callbackproc=static_cast<DStringGDL*>(self->GetTag(3));
  StrUpCaseInplace((*callbackproc)[0]);
  if(callbackproc->NBytes() >0) {
	  DStructGDL* ev = new DStructGDL( "GDL2GDL_CBK_EVENT");
      ev->InitTag("CALLBACKPROC", DStringGDL((*callbackproc)[0]));
      ev->InitTag("CALLBACKSTATUS", DIntGDL(2));
      ev->InitTag("CALLBACKERROR", DStringGDL("")); 
      ev->InitTag("CALLBACKOBJECT", *o); 
      ev->InitTag("CALLBACKUSERDATA", *(self->GetTag(5))); 
      gdl2gdlCallbackQueue.PushFront(ev);
  }
}
static void ReportUsingCallBack(pid_t pid) {
  // push callback event if necessary
  DObjGDL* o = g2gMap.at(pid).obj;
  DStructGDL* self = BaseGDL::interpreter->GetObjHeap((*o)[0]);
  DStringGDL* callbackproc=static_cast<DStringGDL*>(self->GetTag(3));
  StrUpCaseInplace((*callbackproc)[0]);
  if(callbackproc->NBytes() >0) {
	  DStructGDL* ev = new DStructGDL( "GDL2GDL_CBK_EVENT");
      ev->InitTag("CALLBACKPROC", DStringGDL((*callbackproc)[0]));
      ev->InitTag("CALLBACKSTATUS", DIntGDL(g2gMap.at(pid).status));
      ev->InitTag("CALLBACKERROR", DStringGDL(g2gMap.at(pid).description));
      ev->InitTag("CALLBACKOBJECT", *o); 
      ev->InitTag("CALLBACKUSERDATA", *(self->GetTag(5))); 
      gdl2gdlCallbackQueue.PushFront(ev);
  }
}


static void WaitForChildExecuteCompleted(int sig, siginfo_t *siginfo, void *context) {
  // get pid of sender,
  pid_t pid = siginfo->si_pid;
//    std::cout << SysVar::MsgPrefix() << "Signal from child " << pid << std::endl;
  g2gMap.at(pid).status = 2; //child just completed command
}
int WriteToChild(EnvT* e, DLong* id, const std::string & command, bool nowait = true) {
  //will trigger signalhandler
//  struct sigaction siga;
//  memset(&siga, 0, sizeof (struct sigaction)); //no complaints by Valgrind
//  siga.sa_sigaction = *ChildSignalHandler;
//  siga.sa_flags = SA_SIGINFO; // get detail info
  pid_t pid=id[2];
  // should start with no error
  g2gMap.at(pid).description.clear();
  //Warning: ChildSignalHandlerWithCallBack must be used ONLY when a NOWAIT is asked for.
  // Normal return handling is with WaitForChildExecuteCompleted
  void (* callback)(int, siginfo_t *, void *);
  if (nowait) callback=ChildSignalHandlerWithCallBack; else callback=WaitForChildExecuteCompleted;
  if (gdl_ipc_SetReceiverForChildSignal(*callback) != 0) {
	g2gMap.at(pid).status = 3;
	g2gMap.at(pid).description = "Error in  WriteToChild(), problem with sigaction:" + std::string(strerror(errno));
	ReportUsingCallBack(pid);
	return 0;
  }
  auto l = command.length();
  gdl_ipc_sendsignalToChild(id[2]); //ask for a GDL_SIGUSR2 when returned
  g2gMap.at(pid).status = 1;
  int status = write(id[1], command.c_str(), l);
  if (status != l) {
	g2gMap.at(pid).status = 3;
	g2gMap.at(pid).description = "Error in write command to suprocess:" + std::string(strerror(errno));
	ReportUsingCallBack(pid);
	return 0;
  }

  if (!nowait && g2gMap.at(id[2]).status == 1) {
//		std::cout << SysVar::MsgPrefix() << "WriteToChild caught " << g2gMap.at(id[2]) .status<< std::endl;
	while (g2gMap.at(id[2]).status == 1) pause();
  }
//    std::cout << SysVar::MsgPrefix() << "WriteToChild caught " << g2gMap.at(id[2]).status << std::endl;
  return 0;
}


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
	int status = WriteToChild(e, &((*triplet)[0]), command, false);

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
	int status = WriteToChild(e, &((*triplet)[0]), command, false);
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
	return new DIntGDL(g2gMap.at(pid).status);
	static int ERROR = e->KeywordIx("ERROR");
	if (e->KeywordPresent(ERROR)) e->SetKW(ERROR, new DStringGDL(g2gMap.at(pid).description));
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

	int status = WriteToChild(e, &((*triplet)[0]), command, e->KeywordSet(NOWAIT));
  }

  void gmem_abort(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid=(*triplet)[2];
	gdl_ipc_sendCtrlCToChild((*triplet)[2]); //make ^C
	g2gMap.at(pid).status = 4; //aborted
	g2gMap.at(pid).description = "Command aborted."; //aborted
	ReportUsingCallBack(pid);
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
	  std::string myParentPid=i2s(g2gParentPid);
	  if (-1 == execl(me.c_str(), me.c_str(), subp.c_str(), myParentPid.c_str(), (char  *)NULL)) {
		perror("child process execve failed [%m]");
		return new DLongGDL(0);
	  }
	  printf("closing fd %d\n", write_pipe[0]);
	  printf("subprocess %d exited\n", getpid());
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
	  g2gMap.insert(std::pair<pid_t,gdl2gdlparams>(subprocess_pid,params));
	  // insure communication with child is OK waiting for a status change
      gdl_ipc_wait_for_subprocess_started();
//  if (gdl_ipc_SetReceiverForChildSignal(*WaitForChildExecuteCompleted) != 0) {
//		g2gMap.at(subprocess_pid).status = 3;
//		g2gMap.at(subprocess_pid).description = "Error in  WriteToChild(), problem with sigaction:" + std::string(strerror(errno));
//		e->Throw("problem starting child process.");
//		return triplet;
//	  }
//	  if (g2gMap.at(subprocess_pid).status == 0) {
//			std::cout << SysVar::MsgPrefix() << "gmem_fork caught " << g2gMap.at(subprocess_pid).status << std::endl;
//		while (g2gMap.at(subprocess_pid).status == 0) pause();
//	  }
//    std::cout << SysVar::MsgPrefix() << "gmem_fork caught " << g2gMap.at(subprocess_pid).status << std::endl;
//	// reset immediately to 'idle'
//    g2gMap.at(subprocess_pid).status == 0;
//    g2gMap.at(subprocess_pid).description.clear();
      return triplet;
	}

  }

  // gmem_exit,id : cleans the shared mem & objectlist, called by IDL_IDLBridge::Cleanup

  void gmem_exit(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid = (*triplet)[2];


	if (g2gMap.at(pid).status == 1) { //interrupt the process
	  gdl_ipc_sendsignalToParent();
	  g2gMap.at(pid).status = 2; //aborted
	  HandleObjectsCallbacks(); //callback must be called
	}
	//remove shared mem and pid, object
	g2gSharedMemListIter it = g2gListOfSharedMem.find(pid);
	if (it != g2gListOfSharedMem.end()) {
	  std::string sharedId = base + i2s(getpid()) + "$" + i2s((*it).second);
	  shm_unlink(sharedId.c_str());
	  g2gListOfSharedMem.erase(pid);
	}

    g2gMap.erase(pid);
	DString command = "EXIT\n";
	int status = write((*triplet)[1], command.c_str(), 5);
  }
}
#endif
