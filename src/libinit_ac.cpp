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
#include "gsl_matrix.hpp"

using namespace std;

void LibInit_ac()
{
  const char KLISTEND[] = "";

#if defined(HAVE_LIBGSL)

  // Besel functions family
  const string beseliKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFun(lib::beseli_fun,string("BESELI"),2,beseliKey);
  const string beseljKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFun(lib::beselj_fun,string("BESELJ"),2,beseljKey);
  const string beselkKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFun(lib::beselk_fun,string("BESELK"),2,beselkKey);
  const string beselyKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFun(lib::besely_fun,string("BESELY"),2,beselyKey);

  // Matrix functions family
  const string ludcKey[]={"COLUMN","DOUBLE","INTERCHANGES",KLISTEND};
  new DLibPro(lib::ludc_pro,string("LUDC"),2,ludcKey);
  const string lusolKey[]={"COLUMN","DOUBLE",KLISTEND};
  new DLibFun(lib::lusol_fun,string("LUSOL"),3,lusolKey);


#endif
  
  const string spl1Key[]={"YP0","YPN_1","DOUBLE","HELP",KLISTEND};
  new DLibFun(lib::spl_init_fun,string("SPL_INIT"),2,spl1Key);
  const string spl2Key[]={"DOUBLE","HELP",KLISTEND};
  new DLibFun(lib::spl_interp_fun,string("SPL_INTERP"),4,spl2Key);

  const string robertsKey[]={"HELP",KLISTEND};
  new DLibFun(lib::roberts_fun,string("ROBERTS"),1,robertsKey);
  const string sobelKey[]={"HELP",KLISTEND};
  new DLibFun(lib::sobel_fun,string("SOBEL"),1,sobelKey);
  const string prewittKey[]={"HELP",KLISTEND};
  new DLibFun(lib::prewitt_fun,string("PREWITT"),1,prewittKey);

}

