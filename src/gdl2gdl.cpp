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

static const DString commandRetall = "retall\n";
static bool atexit_already_done = false;
#define THEBUFLEN 4096
static char theBuf[THEBUFLEN];

#include "dinterpreter.hpp"

void HandleObjectsCallbacks() {
  for (g2gSubprocessListIter it = g2gListOfSubprocesses.begin(); it != g2gListOfSubprocesses.end(); ++it) {
	if ((*it).second.first == 2) {
	  (*it).second.first = 0; //make completed
	  (*it).second.second = "";
	  //perform generic callback
	  DObjGDL* o = g2gListOfObjects.at((*it).first); //pid
	  DStructGDL* self = BaseGDL::interpreter->GetObjHeap((*o)[0]);
	  DPro* cbk = self->Desc()->GetPro("ONCALLBACK");
	  EnvUDT* newEnv = new EnvUDT(NULL, cbk, NULL);
	  newEnv->SetNextPar(self->Dup()); // pass 'self'
	  newEnv->SetNextPar(new DLongGDL((*it).second.first));
	  newEnv->SetNextPar(new DStringGDL((*it).second.second));
	  BaseGDL::interpreter->CallStack().push_back(newEnv);
	  // make the call
	  BaseGDL::interpreter->call_pro(cbk->GetTree());
	  BaseGDL::interpreter->CallStack().pop_back();
	}
  }
}

static void ChildSignalHandler(int sig, siginfo_t *siginfo, void *context) {
  // get pid of sender,
  pid_t pid = siginfo->si_pid;
  //  std::cout << SysVar::MsgPrefix() << "Signal from child " << pid << std::endl;
  g2gListOfSubprocesses.at(pid).first = 2; //child just completed command
}

int WriteToChild(EnvT* e, DLong* id, const std::string & command, bool nowait = true) {
  sigset_t sigmask;
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGUSR2);
  //	sigaddset(&sigmask, SIGCHLD);
  //will trigger signalhandler
  struct sigaction siga;
  memset(&siga, 0, sizeof (struct sigaction)); //no complaints by Valgrind
  siga.sa_sigaction = *ChildSignalHandler;
  siga.sa_flags = SA_SIGINFO; // get detail info
  pid_t pid=id[2];
  // should start with no error
  g2gListOfSubprocesses.at(pid).second = "";
  if (sigaction(SIGUSR2, &siga, NULL) != 0) {
	g2gListOfSubprocesses.at(pid).first = 3;
	g2gListOfSubprocesses.at(pid).second = "Error in  WriteToChild(), problem with sigaction:" + std::string(strerror(errno));
	return 0;
  }
  auto l = command.length();
  kill(id[2], SIGUSR1); //ask for a SIGUSR2 when returned
  g2gListOfSubprocesses.at(pid).first = 1;
  int status = write(id[1], command.c_str(), l);
  if (status != l) {
	g2gListOfSubprocesses.at(pid).first = 3;
	g2gListOfSubprocesses.at(pid).second = "Error in write command to suprocess:" + std::string(strerror(errno));
	//   e->Throw("error sending command subprocess.");
	return 0;
  }

  struct timespec tv = {.tv_sec = 10, .tv_nsec = 0};
  if (!nowait && g2gListOfSubprocesses.at(id[2]).first == 1) {
	//	std::cout << SysVar::MsgPrefix() << "caught " << g2gListOfSubprocesses.at(id[2]) << std::endl;
	while (g2gListOfSubprocesses.at(id[2]).first == 1) pause();
  }
  //  std::cout << SysVar::MsgPrefix() << "caught " << g2gListOfSubprocesses.at(id[2]) << std::endl;
  return 0;
}

std::map<int, int> g2gListOfSharedMem;
std::map<pid_t, std::pair<int, std::string> > g2gListOfSubprocesses;
std::map<pid_t, DObjGDL*> g2gListOfObjects;

static const std::string base{"_GMEM$"};

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
	if (g2gListOfSubprocesses.at(subpid).first == 1) e->Throw("Error: The object's associated IDL process is currently busy.");

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

	DString command = (*s)[0] + "=gmem_read('" + sharedId + "')\n";
	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR2);
	sigaddset(&sigmask, SIGCHLD);
	int status = WriteToChild(e, &((*triplet)[0]), command, false);
	//	if ( g2gListOfSubprocesses.at(subpid) == 1) status = sigsuspend(&sigmask); //wait for SIGUSR2

	AddGmemCleanToAtexit(); //will suppress shared sections at exit
  }

  // slave : gm_write,handle,variable

  void gmem_write(EnvT* e) {
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
	if (g2gListOfSubprocesses.at(subpid).first == 1) e->Throw("Error: The object's associated IDL process is currently busy.");
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

	DString command = "gmem_write,'" + sharedId + "'," + child_var + "\n";
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

  BaseGDL* gmem_read(EnvT* e) {
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
	return new DIntGDL(g2gListOfSubprocesses.at(pid).first);
	static int ERROR = e->KeywordIx("ERROR");
	if (e->KeywordPresent(ERROR)) e->SetKW(ERROR, new DStringGDL(g2gListOfSubprocesses.at(pid).second));
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
	if (g2gListOfSubprocesses.at(pid).first == 1) e->Throw("Error: The object's associated IDL process is currently busy.");

	BaseGDL* p1 = e->GetParDefined(1);
	if (p1->Type() != GDL_STRING) e->Throw(e->GetParString(1) + " must be a string");

	DStringGDL* s = e->GetParAs<DStringGDL>(1);
	DString command = (*s)[0] + "\n";

	int status = WriteToChild(e, &((*triplet)[0]), command, e->KeywordSet(NOWAIT));
  }

  BaseGDL* gmem_receive(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	fd_set fds;
	struct timeval tv = {.tv_sec = 0, .tv_usec = 0};
	FD_ZERO(&fds);
	FD_SET((*triplet)[0], &fds); // Watch stdin (fd 0)
	if (select((*triplet)[0] + 1, &fds, NULL, NULL, &tv)) {
	  int nread = read((*triplet)[0], theBuf, THEBUFLEN);
	  if (nread == -1) {
		perror("gmem_receive");
		return new DStringGDL("");
	  }
	  DString statement(theBuf, nread);
	  return new DStringGDL(statement);
	} else return new DStringGDL("");
  }

  bool executeCommand(EnvT*e, DString statement) {
	std::istringstream iss(statement, std::ios_base::out);
	try {
	  e->Interpreter()->ExecuteLine(&iss);
	} catch (GDLException& e) {
	  return false;
	}
	return true;
  }

  void gmem_abort(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid=(*triplet)[2];
	g2gListOfSubprocesses.at(pid).first = 4; //aborted
	g2gListOfSubprocesses.at(pid).second = "Command aborted."; //aborted
	kill((*triplet)[2], SIGINT); //make ^C
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

	if (subprocess_pid == 0) {
	  close(write_pipe[1]); /* Close unused write end */
	  close(read_pipe[0]); /* Close unused read end */
	  close(0);
	  close(1);
	  close(2);
	  dup2(write_pipe[0], 0);
	  dup2(read_pipe[1], 1);
	  dup2(read_pipe[1], 2);
	  if (-1 == execv(master_argv[0], (char **) master_argv)) { //, NULL)) {
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
	  // insure communication with child is OK
	  // send startup commands
	  // return
	  DLongGDL* ret = new DLongGDL(dimension(3));
	  (*ret)[0] = read_pipe[0];
	  (*ret)[1] = write_pipe[1];
	  (*ret)[2] = subprocess_pid;
	  g2gListOfSubprocesses.insert( std::pair<pid_t, std::pair<int, std::string> > (subprocess_pid, std::pair<int, std::string>(0,""))); //idle
	  g2gListOfObjects.insert(std::pair<pid_t, DObjGDL*>(subprocess_pid, o));
	  return ret;
	}

  }

  // gmem_exit,id : cleans the shared mem & objectlist, called by IDL_IDLBridge::Cleanup

  void gmem_exit(EnvT*e) {
	SizeT nParam = e->NParam(1);
	BaseGDL* p0 = e->GetParDefined(0);
	if (p0->N_Elements() != 3) e->Throw("I need a triplet.");
	DLongGDL *triplet = e->GetParAs<DLongGDL>(0);
	pid_t pid = (*triplet)[2];


	if (g2gListOfSubprocesses.at(pid).first == 1) { //interrupt the process
	  kill((*triplet)[2], SIGUSR2);
	  g2gListOfSubprocesses.at(pid).first = 2; //aborted
	  HandleObjectsCallbacks(); //callback must be called
	}
	//remove shared mem and pid, object
	g2gSharedMemListIter it = g2gListOfSharedMem.find(pid);
	if (it != g2gListOfSharedMem.end()) {
	  std::string sharedId = base + i2s(getpid()) + "$" + i2s((*it).second);
	  shm_unlink(sharedId.c_str());
	  g2gListOfSharedMem.erase(pid);
	}

	g2gListOfSubprocesses.erase(pid);
	g2gListOfObjects.erase(pid);

	DString command = "EXIT\n";
	int status = write((*triplet)[1], command.c_str(), 5);
  }
}
