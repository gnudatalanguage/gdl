/***************************************************************************
                          basic_fun_jmg.hpp  -  basic GDL library functions
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASIC_FUN_JMG_HPP_
#define BASIC_FUN_JMG_HPP_

namespace lib {

  BaseGDL* size( EnvT* e);
  BaseGDL* fstat( EnvT* e); 
  BaseGDL* make_array( EnvT* e);
  BaseGDL* reform( EnvT* e);
  BaseGDL* routine_names_value( EnvT* e);
  BaseGDL** routine_names_reference( EnvT* e);

} // namespace

#endif
