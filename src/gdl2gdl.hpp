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
extern void gdl_ipc_ClientGetsMailboxAddress(std::string & s);
#else

//for client gdl2gdl (see IDL_IDLBridge)
#if defined(__APPLE__)
#include <mach/mach.h>
#include <servers/bootstrap.h>
typedef mach_port_t messageBoxHandle;
extern messageBoxHandle port; //client server queue descriptor
#define G2G_SERVER_QUEUE_BASENAME   "org.gdl."

#elif defined(_WIN32) && !defined(__CYGWIN__)
typedef int messageBoxHandle;
#else
#include <mqueue.h>
typedef mqd_t messageBoxHandle;
extern messageBoxHandle gdl2gdlMessageBoxHandle; //client server queue descriptor
#define G2G_SERVER_QUEUE_BASENAME   "/gdl-server-"
#endif

#define QUEUE_PERMISSIONS 0700
#define MAX_MESSAGES 1 
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

extern GDLEventQueue gdl2gdlCallbackQueue;

struct GDL2GDLINFOS {
  DObjGDL* obj;
  int status;
  bool nowait;
  std::string description;
  messageBoxHandle MessageChannelHandle;
};
typedef GDL2GDLINFOS gdl2gdlparams;
extern std::map<pid_t, gdl2gdlparams> g2gMap;
typedef std::map<pid_t, gdl2gdlparams>::iterator g2gMapIter;

extern std::map<int, int> g2gListOfSharedMem;
typedef std::map<int,int>::iterator g2gSharedMemListIter;

using namespace std;
extern void g2gEventDispatcher();

extern void StartMasterMessageChannel();
extern void StartIndividualClientMessageChannel();
extern void gdl_ipc_ClientGetsMailboxAddress(std::string & s);

void gmem_clean(); //internal

namespace lib {
  BaseGDL* gmem_subprocess_receivevar(EnvT* e);
  void gmem_subprocess_givevar(EnvT* e);
  void gmem_setvar(EnvT* e);
  BaseGDL* gmem_getvar(EnvT* e);
  void gmem_abort(EnvT*e);
  BaseGDL* gmem_fork(EnvT* e);
  void gmem_send(EnvT* e);
  BaseGDL* gmem_status(EnvT* e);
  void gmem_exit(EnvT* e);
  void gdl2gdl_callback(EnvUDT* e);
} // namespace

#endif
