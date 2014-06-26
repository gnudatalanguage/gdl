/***************************************************************************
                          semshm.cpp  -  Semaphores / Shared memory 
                          -------------------
    begin                : Jun 29 2011
    copyright            : (C) 2011 by Mateusz Turcza 
    email                : mturcza@mimuw.edu.pl 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "includefirst.hpp"

#if defined(_WIN32) && !defined(__CYGWIN__)
#define sem_trywait(sem) (WaitForSingleObject(sem, 0) == WAIT_OBJECT_0 ? 0 : -1) 
#define sem_post(sem) (ReleaseSemaphore(sem, 1, NULL) ? 0 : -1) 
#else
#include <semaphore.h>
#endif
#include <fcntl.h>
#include <map>
#include <cerrno>

#include "envt.hpp"


namespace lib {

  // map: semaphore_name -> semaphore_data
  typedef struct {
#if defined(_WIN32) && !defined(__CYGWIN__)
    HANDLE sem;
#else
    sem_t *sem;
#endif
    DByte deletable;
    DByte owner;
    DByte locked;
  } sem_data_t;

  typedef std::map<DString, sem_data_t> sem_map_t;

  static inline bool sem_is_deletable(const sem_data_t &data)
  {
    return data.deletable;
  }

  static inline bool sem_is_locked(const sem_data_t &data)
  {
    return data.locked;
  }

  static inline bool sem_is_owner(const sem_data_t &data)
  {
    return data.owner;
  }

  static inline void sem_set_deletable(sem_data_t &data, bool deletable)
  {
    data.deletable = deletable;
  }

  static inline void sem_set_locked(sem_data_t &data, bool locked)
  {
    data.locked = locked;
  }

  static inline void sem_set_owner(sem_data_t &data, bool owner)
  {
    data.owner = owner;
  }

  static sem_map_t &sem_map()
  {
    static sem_map_t map;
    return map;
  }

  static inline void sem_remove(const DString &name)
  {
    sem_map().erase(name);
  }

  static inline void sem_add(const DString &name, const sem_data_t &data)
  {
    sem_map_t &map = sem_map();
    sem_map_t::iterator it = map.find(name);

    if (it == map.end())
    {
      assert(data.sem);
      map.insert(std::pair<DString, sem_data_t>(name, data));
    } 
  }

  static inline sem_data_t &sem_get_data(const DString &name, EnvT *e)
  {
    sem_map_t &map = sem_map();
    sem_map_t::iterator it = map.find(name);

    if (it == map.end())
    {
      e->Throw("Unknown semaphore name provided: " + name + ".");
    }

    return it->second;
  }

  // executed in gdlexit()
  void sem_onexit()
  {
    // remove semaphores created by this gdl process
    sem_map_t &map = sem_map();
    for (sem_map_t::iterator it = map.begin(); it != map.end(); ++it)
    {
      if (sem_is_deletable(it->second))
      {
#if defined(_WIN32) && !defined(__CYGWIN__)
        CloseHandle(it->second.sem);
#else
        sem_unlink(it->first.c_str());
#endif
      }
    }
    // don't bother with invalid semaphores because we're exiting
  }

  // GDL routines implementation

  BaseGDL* sem_create(EnvT *e)
  {
    SizeT nParam = e->NParam(1); // 1 is the minimal number of parameter required

    DString name;
    e->AssureStringScalarPar(0, name); // IDL accepts null-string name

    int destroyIx = e->KeywordIx("DESTROY_SEMAPHORE");
    bool destroyKWPresent = e->KeywordPresent(destroyIx);
    DLong destroy = 0;
    if (destroyKWPresent) 
    {
      destroy = (*e->GetKWAs<DLongGDL>(0))[0];
    }

    bool owner = true;
#if defined(_WIN32) && !defined(__CYGWIN__)
    // TODO: Needs error handling with name length > 256
    const char* cname = name.c_str();
    TCHAR tname[256] = {0,};

    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cname, strlen(cname), tname, 256);
    HANDLE sem = CreateSemaphore(NULL,1,1,tname);
    if (sem == NULL) {
	owner = false;
	return new DIntGDL(0);
    }
#else
    sem_t *sem = sem_open(name.c_str(), O_CREAT | O_EXCL, 0666, 1);
    if (sem == SEM_FAILED)
    {
      owner = false;
      if (errno == EEXIST)
      {
        sem = sem_open(name.c_str(), 0);
      }
      if (sem == SEM_FAILED)
      {
        return new DIntGDL(0);
      }
    }
#endif

    // Behavior for different values of DESTROY_SEMAPHORE:
    // DESTROY_SEMAPHORE | owner    | other (== !owner)
    // ------------------+----------+-----------
    // not set           | delete   | ignore
    // != 0              | delete   | delete
    // 0                 | ignore   | ignore
    sem_data_t data;
    data.sem = sem;

    sem_set_owner(data, owner);
    sem_set_deletable(data, (!destroyKWPresent && owner) || (destroy != 0));
    sem_set_locked(data, false);

    sem_add(name, data);

    return new DIntGDL(1);
  }

  void sem_delete(EnvT *e)
  {
    SizeT nParam = e->NParam(1);

    DString name;
    e->AssureStringScalarPar(0, name);

    const sem_data_t &data = sem_get_data(name, e);
#if defined(_WIN32) && !defined(__CYGWIN__)
    CloseHandle(data.sem);
#else
    sem_close(data.sem);

    if (sem_is_deletable(data))
    {
      sem_unlink(name.c_str());
    }
#endif

    sem_remove(name);
  }

  BaseGDL* sem_lock(EnvT *e)
  {
    SizeT nParam = e->NParam(1);

    DString name;
    e->AssureStringScalarPar(0, name);

    sem_data_t &data = sem_get_data(name, e);
    if (sem_is_locked(data)) 
    {
      // lock already held
      return new DIntGDL(1);
    }
    if (sem_trywait(data.sem) == 0)
    {
      sem_set_locked(data, true);
      return new DIntGDL(1);
    }

    return new DIntGDL(0);
  }

  void sem_release(EnvT *e)
  {
    SizeT nParam = e->NParam(1);

    DString name;
    e->AssureStringScalarPar(0, name);

    sem_data_t data = sem_get_data(name, e);
    if (sem_is_locked(data))
    {
      sem_set_locked(data, false);
      sem_post(data.sem);
    }
  }

}
