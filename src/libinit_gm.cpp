/***************************************************************************
                          math_fun_gm.cpp  -  math GDL library function (GM)
                             -------------------
    begin                : 03 May 2007
    copyright            : (C) 2007 by Gregory Marchal
    email                : gregory.marchal_at_obspm.fr
    website              : http://format.obspm.fr/~m1/gmarchal/

****************************************************************************/

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
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_gamma.h>
#endif

#include "math_fun_gm.hpp"

using namespace std;

void LibInit_gm()
{
  const char KLISTEND[] = "";

#if defined(HAVE_LIBGSL)

  const string erfKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::erf_fun,string("ERF"),1,erfKey);

  const string errorfKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::errorf_fun,string("ERRORF"),1,errorfKey);

  const string erfcKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::erfc_fun,string("ERFC"),1,erfcKey);

  const string gammaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::gamma_fun,string("GAMMA"),1,gammaKey);

  const string lngammaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::lngamma_fun,string("LNGAMMA"),1,lngammaKey);

  const string igammaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::igamma_fun,string("IGAMMA"),2,igammaKey);

  const string betaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::beta_fun,string("BETA"),2,betaKey);

  const string expintKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::expint_fun,string("EXPINT"),2,expintKey);

  const string gaussintKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::gaussint_fun,string("GAUSSINT"),1,gaussintKey);

#endif

}

