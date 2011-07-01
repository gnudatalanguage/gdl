/***************************************************************************
                          semshm.hpp  -  Semaphores / Shared memory 
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

#ifndef SEMSHM_HPP_
#define SEMSHM_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* sem_create(EnvT*);
  void sem_delete(EnvT*);
  BaseGDL* sem_lock(EnvT*);
  void sem_release(EnvT*);

  void sem_onexit();

} // namespace

#endif
