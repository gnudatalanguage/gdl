/***************************************************************************
                          shm.hpp  -  Shared memory mapping
                             -------------------
    begin                : Dec 24 2023
    copyright            : (C) 2023 by Gilles Duvert
    email                : surname dot name at free dot fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SHM_HPP_
#define SHM_HPP_

#include "envt.hpp"
#include <iostream>     // std::cout, std::ostream, std::ios
#include "shm_utils.hpp"
#ifndef _WIN32

typedef struct {
  void* mapped_address;
  std::string osHandle;
  size_t length;
  off_t offset;
  int   refcount;
  dimension dim;
  int type;
  int flags;
} SHMAP_STRUCT;

enum Shmap_flags {
  DELETE_PENDING=1,
  MAPPEDFILE=2,
  DESTROY_SEGMENT_ON_UNMAP=4,
  USE_SYSV=8,
  IS_PRIVATE=16
};
extern std::map<DString, SHMAP_STRUCT > shmList;

typedef std::map<DString, SHMAP_STRUCT > ::iterator shmListIter;
namespace lib {

  void shmmap_pro(EnvT* e);
  void shmunmap_pro(EnvT* e);
  BaseGDL* shmvar_fun(EnvT* e);
  BaseGDL* shmdebug_fun(EnvT* e);
  void TidySharedAtGDLExit();
} // namespace

#endif
#endif
