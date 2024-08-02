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
extern std::map<int,int> g2gListOfSharedMem;
extern std::map<pid_t,int> g2gListOfSubprocesses;
extern std::map<pid_t,DObjGDL*> g2gListOfObjects;

typedef std::map<int,int>::iterator g2gSharedMemListIter;
typedef std::map<pid_t,int>::iterator g2gSubprocessListIter;
typedef std::map<pid_t,DObjGDL*>::iterator g2gObjectListIter;

using namespace std;
extern void HandleObjectsCallbacks();

namespace lib {
  BaseGDL* gmem_read(EnvT* e);
  void gmem_write(EnvT* e);
  void gmem_setvar(EnvT* e);
  BaseGDL* gmem_getvar(EnvT* e);
  void gmem_abort(EnvT*e);
  BaseGDL* gmem_fork(EnvT* e);
  void gmem_send(EnvT* e);
  BaseGDL* gmem_status(EnvT* e);
  BaseGDL* gmem_receive(EnvT* e);
  void gmem_clean();
  void gdl2gdl_callback(EnvUDT* e);
} // namespace

