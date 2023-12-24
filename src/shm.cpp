/***************************************************************************
                          shm.cpp  -  Shared memory mapping
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

#include "shm.hpp"

namespace lib {

  void shmmap_pro(EnvT* e){};
  void shmunmap_pro(EnvT* e){};
  BaseGDL* shmvar_fun(EnvT* e){return new DIntGDL(0);}
  BaseGDL* shmdebug_fun(EnvT* e){return new DIntGDL(0);}

} // namespace
