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
#include "gsl_fun.hpp"

#include "smooth.hpp"

using namespace std;

void LibInit_ac()
{
  const char KLISTEND[] = "";

#if defined(HAVE_LIBGSL)

  // Besel functions family
  const string beseliKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFunRetNew(lib::beseli_fun,string("BESELI"),2,beseliKey);
  const string beseljKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFunRetNew(lib::beselj_fun,string("BESELJ"),2,beseljKey);
  const string beselkKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFunRetNew(lib::beselk_fun,string("BESELK"),2,beselkKey);
  const string beselyKey[]={"DOUBLE","ITER","HELP",KLISTEND};
  new DLibFunRetNew(lib::besely_fun,string("BESELY"),2,beselyKey);

  // Matrix functions family
  const string ludcKey[]={"COLUMN","DOUBLE","INTERCHANGES",KLISTEND};
  new DLibPro(lib::ludc_pro,string("LUDC"),2,ludcKey);
  const string lusolKey[]={"COLUMN","DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::lusol_fun,string("LUSOL"),3,lusolKey);
  const string determKey[]={"DOUBLE","CHECK","ZERO",KLISTEND};
  new DLibFunRetNew(lib::determ_fun,string("DETERM"),1,determKey);

  const string trisolKey[]={"DOUBLE",KLISTEND};
  new DLibFunRetNew(lib::trisol_fun,string("TRISOL"),4,trisolKey);

  // QSIMP/QROMB (we share code for QSIMP and QROMB)
  const string qsimpKey[]={"DOUBLE","EPS","JMAX",KLISTEND};
  const string qsimpWarnKey[]={"K",KLISTEND}; //RSIMP has no 'K'
  new DLibFunRetNew(lib::qromb_fun,string("QSIMP"),3,qsimpKey,qsimpWarnKey);
  const string qrombKey[]={"DOUBLE","EPS","JMAX",KLISTEND};
  const string qrombWarnKey[]={"K",KLISTEND}; //QROMB code does not make use of 'K' key
  new DLibFunRetNew(lib::qromb_fun,string("QROMB"),3,qrombKey,qrombWarnKey);
  //QROMO
  const string qromoKey[]={"DOUBLE","EPS","JMAX","K","MIDEXP",
			   "MIDINF","MIDPNT","MIDSQL","MIDSQU",KLISTEND};
  new DLibFunRetNew(lib::qromo_fun,string("QROMO"),3,qromoKey);
  const string fz_rootsKey[]={"DOUBLE","EPS","NO_POLISH",KLISTEND};
  new DLibFunRetNew(lib::fz_roots_fun,string("FZ_ROOTS"),3,fz_rootsKey);
  const string fx_rootKey[]={"DOUBLE","ITMAX","STOP","TOL",KLISTEND};
  new DLibFunRetNew(lib::fx_root_fun,string("FX_ROOT"),2,fx_rootKey);

#endif

  const string spl1Key[]={"YP0","YPN_1","DOUBLE","HELP",KLISTEND};
  new DLibFunRetNew(lib::spl_init_fun,string("SPL_INIT"),2,spl1Key);
  const string spl2Key[]={"DOUBLE","HELP",KLISTEND};
  new DLibFunRetNew(lib::spl_interp_fun,string("SPL_INTERP"),4,spl2Key);

  const string helpKey[]={"HELP",KLISTEND};
  new DLibFunRetNew(lib::roberts_fun,string("ROBERTS"),1,helpKey);
  new DLibFunRetNew(lib::sobel_fun,string("SOBEL"),1,helpKey);
  new DLibFunRetNew(lib::prewitt_fun,string("PREWITT"),1,helpKey);

  const string erodeKey[]={"HELP","GRAY","PRESERVE_TYPE","UINT","ULONG","VALUES",KLISTEND};
  new DLibFunRetNew(lib::erode_fun,string("ERODE"),5,erodeKey);

  const string dilateKey[]={"HELP","GRAY","PRESERVE_TYPE","UINT","ULONG","VALUES",
			    "CONSTRAINED","BACKGROUND",KLISTEND};
  new DLibFunRetNew(lib::dilate_fun,string("DILATE"),5,dilateKey);

  const string matrix_multiplyKey[]={"ATRANSPOSE","BTRANSPOSE",KLISTEND};
  new DLibFunRetNew(lib::matrix_multiply,string("MATRIX_MULTIPLY"),2,matrix_multiplyKey);

// Levan Loria and Alain Coulais, September 2014 : to be extensively tested
// please replace smooth with one of these functions only when thoroughly tested.
//  const string smoothKey[]={"NAN",KLISTEND};
//  new DLibFunRetNew(lib::smooth2_fun,string("SMOOTH2"),2,smoothKey);
//  new DLibFunRetNew(lib::smooth3_fun,string("SMOOTH3"),2,smoothKey);
  
}

