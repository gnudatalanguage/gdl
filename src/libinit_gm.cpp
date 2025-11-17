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
#include <gsl/gsl_sf_gamma.h>
#endif

#include "math_fun_gm.hpp"

using namespace std;

void LibInit_gm()
{
  const char KLISTEND[] = "";

#if defined(HAVE_LIBGSL)

  const string gammaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNewTP(lib::gamma_fun,string("GAMMA"),1,gammaKey);  //UsesThreadPOOL 
  // undocumented function ...
  new DLibFunRetNew(lib::gamma_fun,string("NR_GAMMA"),1,gammaKey);

  const string lngammaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNewTP(lib::lngamma_fun,string("LNGAMMA"),1,lngammaKey);  //UsesThreadPOOL 

  //  const string igammaKey[]={"DOUBLE","EPS","ITER","ITMAX","METHOD",KLISTEND};
  const string igammaKey[]={"DOUBLE","METHOD",KLISTEND};
  const string igammaWarnKey[]={"EPS","ITER","ITMAX",KLISTEND};
  new DLibFunRetNew(lib::igamma_fun,string("IGAMMA"),2,igammaKey,igammaWarnKey);
  // undocumented function ...
  new DLibFunRetNewTP(lib::igamma_fun,string("IDL_IGAMMA"),2,igammaKey);  //UsesThreadPOOL 

  const string betaKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNewTP(lib::beta_fun,string("BETA"),2,betaKey);  //UsesThreadPOOL 

  const string ibetaKey[]={"DOUBLE",KLISTEND};
  const string ibetaWarnKey[]={"EPS","ITER","ITMAX",KLISTEND};
  new DLibFunRetNewTP(lib::ibeta_fun,string("IBETA"),3,ibetaKey, ibetaWarnKey);  //UsesThreadPOOL 

  const string expintKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNewTP(lib::expint_fun,string("EXPINT"),2,expintKey);  //UsesThreadPOOL 

  const string gaussintKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNewTP(lib::gaussint_fun,string("GAUSSINT"),2,gaussintKey);  //UsesThreadPOOL 

#endif

}

