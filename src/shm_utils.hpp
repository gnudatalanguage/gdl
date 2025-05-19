/***************************************************************************
                          shm_utils.hpp  -  Shared memory mapping, external utilities defs
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
#ifndef SHM_UTILS_HPP_
#define SHM_UTILS_HPP_
#ifndef _WIN32
namespace lib {
  void help_par_shared(BaseGDL* var, std::ostream& ostr);
  void help_shared(EnvT* e, std::ostream& ostr);
  void shm_unreference(BaseGDL* var);
} // namespace
#else
namespace lib {
  void help_par_shared(BaseGDL* ret, std::ostream& ostr){};
  void help_shared(EnvT* e, std::ostream& ostr){};
  void shm_unreference(BaseGDL* ret){};
} // namespace
#endif
#endif
