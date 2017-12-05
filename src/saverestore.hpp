/***************************************************************************
                          saverestore.hpp  -  GDL header
                             -------------------
    begin                : Dec 10 2017
    copyright            : (C) 2017 by Gilles Duvert

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
 #include <config.h>
#endif


#ifndef SAVERESTORE_HPP_
#define SAVERESTORE_HPP_

#include "datatypes.hpp"
#include "envt.hpp"
//enum gdlSaveEnumOfCodes {START_MARKER=0, COMMON_VARIABLE, VARIABLE, SYSTEM_VARIABLE, UNKOWN1, UNKNOWN2 , END_MARKER,
//     UNKNOWN3 , UNKNOWN4 ,UNKNOWN5 , TIMESTAMP, UNKNOWN6 , COMPILED,  IDENTIFICATION, THEVERSION,
//      HEAP_HEADER, HEAP_DATA, PROMOTE64, UNKNOWN7, NOTICE, DESCRIPTION};

namespace lib {
  void gdl_restore(EnvT* e);
}
#endif
