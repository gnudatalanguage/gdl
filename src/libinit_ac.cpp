/***************************************************************************
                 libinit_ac.cpp  -  initialization of GDL library routines
                             -------------------
    begin                : 20 April 2007
    copyright            : (C) 2007 by Alain Coulais
    email                : alaingdl@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "includefirst.hpp"

#if defined(HAVE_LIBGSL)
#include <gsl/gsl_sf_bessel.h>
#endif

#include "math_fun_ac.hpp"

using namespace std;

void LibInit_ac()
{
  const char KLISTEND[] = "";

#if defined(HAVE_LIBGSL)
  const string beseliKey[]={"DOUBLE","ITER",KLISTEND};
  new DLibFun(lib::beseli_fun,string("BESELI"),2,beseliKey);
  const string beseljKey[]={"DOUBLE","ITER",KLISTEND};
  new DLibFun(lib::beselj_fun,string("BESELJ"),2,beseliKey);
  const string beselkKey[]={"DOUBLE","ITER",KLISTEND};
  new DLibFun(lib::beselk_fun,string("BESELK"),2,beseliKey);
  const string beselyey[]={"DOUBLE","ITER",KLISTEND};
  new DLibFun(lib::besely_fun,string("BESELY"),2,beseliKey);
#endif
 
}

