/***************************************************************************
                          basic_fun.hpp  -  basic GDL library functions
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
                         : (C) 2010 by Christoph Fuchs (CALL_EXTERNAL)
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

#ifndef BASIC_PRO_JMG_HPP_
#define BASIC_PRO_JMG_HPP_

#include <dlfcn.h>
#include <time.h>

namespace lib {

  void point_lun( EnvT* e);
  void linkimage( EnvT* e);
  void wait( EnvT* e);

  void kwtest( EnvT* e);


  // CALL_EXTERNAL by Christoph Fuchs
  typedef struct {
    int   slen;
    short stype;
    char  *s;
  } EXTERN_STRING;

  BaseGDL* call_external( EnvT* e);
  EXTERN_STRING* ce_StringGDLtoIDL( EnvT* e, const BaseGDL* par);
  void ce_StringIDLtoGDL( EXTERN_STRING* extstring, BaseGDL* par, int freeMemory);
  void* ce_StructGDLtoIDL( EnvT* e, const BaseGDL* par, SizeT* length, SizeT myAlign);
  void ce_StructIDLtoGDL( EnvT* e,  void* IDLStruct, BaseGDL* par, int freeMemory, SizeT myAlign);
  SizeT ce_LengthOfIDLStruct( EnvT* e, const BaseGDL* par, SizeT myAlign);

} // namespace

#endif
