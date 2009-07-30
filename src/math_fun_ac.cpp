/***************************************************************************
                          math_fun_ac.cpp  -  math GDL library function (AC)
                             -------------------
    begin                : 20 April 2007
    copyright            : (C) 2007 by Alain Coulais
    email                : alaingdl@users.sourceforge.net

****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*

  using the Besel functions provided by GSL
 
  http://www.physics.ohio-state.edu/~ntg/780/gsl_examples/J0_test.cpp
  http://www.gnu.org/software/gsl/manual/html_node/Bessel-Functions.html

  ----------------------- Warning -------------

  (obsolete) As is on 20/April/2007: Warning : GSL allows only INTEGER type for order 
  Thibaut contributes to extend to REAL in July 2009

  Important information: since bad formating issue in IDL
  (only first field in BESEL*([1], [1,2,3.]) is OK, doc said IDL must return a [1elem] array
  but return a [3elem] (see converse case BESELI( [1,2,3.], [1])))
  WE decided to follow IDL but to compute all the cases ...

  Note on 28/July/2009: when called with only X elements in IDL, Besel*
  functions are computed with implicit N==0, we don't follow this undocumented feature ...

  ----------------------- Warning -------------

  Since functions are clones derivated from the first one,
  please propagates bugs' correction and improvments.
  Don't know how to symplified :-(

  ----------------------- Warning -------------

  some codes here (macro) are common with "math_fun_gm.cpp" and "math_fun_ng.cpp"

*/

#define GM_EPS   1.0e-6
#define GM_ITER  50
#define GM_TINY  1.0e-18

#define GM_MIN(a, b) ((a) < (b) ? (a) : (b))

#define GM_5P0(a)							\
  e->NParam(a);								\
  									\
  DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>(0);				\
  SizeT nElp0 = p0->N_Elements();					\
  if (nElp0 == 0)							\
    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(0));	\
  									\
  DType t0 = e->GetParDefined(0)->Type();				\
  //if (t0 == COMPLEX || t0 == COMPLEXDBL)				\
  //  e->Throw("Complex not implemented (GSL limitation). ");

#define AC_2P1()							\
  SizeT nElp1;								\
  DIntGDL* p1;								\
  DFloatGDL* p1_float;							\
  DType t1;								\
									\
  if  (e->NParam() == 1)						\
    {									\
      p1 = new DIntGDL(1, BaseGDL::NOZERO);				\
      (*p1)[0]=0;							\
      nElp1=1;								\
      t1 = INT;								\
      p1_float = new DFloatGDL(1, BaseGDL::NOZERO);			\
      (*p1_float)[0]=0.000;						\
    }									\
  else									\
    {									\
      p1 = e->GetParAs<DIntGDL>(1);					\
      nElp1 = p1->N_Elements();						\
      t1 = e->GetParDefined(1)->Type();					\
      p1_float = e->GetParAs<DFloatGDL>(1);				\
    }									\
									\
  const double dzero = 0.0000000000000000000 ;				\
									\
  //    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(1)); \
  									\
//  DType t1 = e->GetParDefined(1)->Type();				\
  //  if (t1 == COMPLEX || t1 == COMPLEXDBL)				\
  // e->Throw("Complex not implemented (GSL limitation). ");

#define GM_DF2()							\
  									\
  DDoubleGDL* res;							\
  if (nElp0 == 1 && nElp1 == 1)						\
    res = new DDoubleGDL(1, BaseGDL::NOZERO);				\
  else if (nElp0 > 1 && nElp1 == 1)					\
    res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);			\
  else if (nElp0 == 1 && nElp1 > 1)					\
    res = new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);			\
  else if (nElp0 <= nElp1)						\
    res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);			\
  else									\
    res = new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);			\
  									\
  SizeT nElp = res->N_Elements();					\

#define GM_CV1()					\
  static DInt doubleKWIx = e->KeywordIx("DOUBLE");	\
							\
  if (t0 != DOUBLE && !e->KeywordSet(doubleKWIx))	\
    return res->Convert2(FLOAT, BaseGDL::CONVERT);	\
  else							\
    return res;						\
							\
  return new DByteGDL(0);

#define GM_CC1()						\
  static DInt coefKWIx = e->KeywordIx("ITER");			\
  if(e->KeywordPresent(coefKWIx))				\
    {								\
      cout << "ITER keyword not used, always return -1)" << endl;	\
      e->SetKW( coefKWIx, new DLongGDL( -1));			\
    }


#include "includefirst.hpp"

#include "math_fun_ac.hpp"
#include <gsl/gsl_sf_bessel.h>

using namespace std;

namespace lib {

  BaseGDL* beseli_fun(EnvT* e)
  {
    GM_5P0(1);
    AC_2P1();
    GM_DF2();

    SizeT count;

    // GSL Limitation for X : must be lower than ~708
    for (count = 0;count<nElp0;++count)
	  if ((*p0)[count] > 708.)
	    e->Throw("Value of X is out of allowed range.");

    // we need to check if N values (array) are Integer or not
    int test=0;
    
    for (count = 0;count<nElp1;++count)
      if (abs((*p1_float)[count]-(float)(*p1)[count]) > 0.000001) // don't know if a "machar" value exists
	test=1;
	
    if (test==0)
      {
	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_In((*p1)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_In((*p1)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_In((*p1)[count],(*p0)[count]);
	  }
      }
    else
      {
	// we need to check if X values (array) are positives
	for (count = 0;count<nElp0;++count)
	  if ((*p0)[count] < dzero) // don't know if a "machar" value exists
	    e->Throw("Value of X is out of allowed range (Only positive values when N is non integer).");

	// we need to check if N values (array) are positives
	for (count = 0;count<nElp1;++count)
	  if ((*p1)[count] < dzero) // don't know if a "machar" value exists
	    e->Throw("Value of N is out of allowed range (Only positive values when N is non integer).");

	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Inu((*p1_float)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Inu((*p1_float)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Inu((*p1_float)[count],(*p0)[count]);
	  }
      }
    GM_CC1();
    GM_CV1();
  }


  BaseGDL* beselj_fun(EnvT* e)
  {
    GM_5P0(1);
    AC_2P1();
    GM_DF2();

    SizeT count;

    // we need to check if N values (array) are Integer or not
    int test=0;
    
    for (count = 0;count<nElp1;++count)
      if (abs((*p1_float)[count]-(float)(*p1)[count]) > 0.000001) // don't know if a "machar" value exists
	test=1;

    if (test==0)
      {
	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Jn((*p1)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Jn((*p1)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Jn((*p1)[count],(*p0)[count]);
	  }
      }
    else
      {
	// we need to check if X values (array) are positives
	for (count = 0;count<nElp0;++count)
	  if ((*p0)[count] < dzero) // don't know if a "machar" value exists
	    e->Throw("Value of X is out of allowed range (Only positive values when N is non integer).");

	// we need to check if N values (array) are positives
	for (count = 0;count<nElp1;++count)
	  if ((*p1)[count] < dzero) // don't know if a "machar" value exists
	    e->Throw("Value of N is out of allowed range (Only positive values when N is non integer).");

	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Jnu((*p1_float)[count],(*p0)[0]);
	  }

	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Jnu((*p1_float)[0],(*p0)[count]);
	  }

	else
	  {
	    for (count = 0;count<nElp;++count)
	      (*res)[count] = gsl_sf_bessel_Jnu((*p1_float)[count],(*p0)[count]);
	  }
      }
    GM_CC1();
    GM_CV1();
  }

  // very preliminary version:
  // should not work when N LT 0
  // should return Inf at x==0
  // should not work for x LT 0

  BaseGDL* beselk_fun(EnvT* e)
  {
    GM_5P0(1);
    AC_2P1();
    GM_DF2();

    // we need to check if N values (array) are Integer or not
    SizeT count;

    // do we have negative numbers ?
    for (count = 0;count<nElp0;++count)
      if ((*p0)[count] < dzero) // don't know if a "machar" value exists
        e->Throw("Value of X is out of allowed range (Only positive values).");
    
    int test=0;
    
    for (count = 0;count<nElp1;++count)
      if (abs((*p1_float)[count]-(float)(*p1)[count]) > 0.000001) // don't know if a "machar" value exists
	test=1;

    if (test==0)
      {
	// when X value is below ~1e-20 --> return -Inf (we use log(0.) which gives -Inf)
	const double smallVal = 1e-38 ;

	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[0]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Kn((*p1)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Kn((*p1)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Kn((*p1)[count],(*p0)[count]);
	  }
      }
    else
      {
	// we need to check if N values (array) are positives
	for (count = 0;count<nElp1;++count)
	  if ((*p1)[count] < dzero) // don't know if a "machar" value exists
	    e->Throw("Value of N is out of allowed range (Only positive values when N is non integer).");

	// when X value is below ~1e-20 --> return -Inf (we use log(0.) which gives -Inf)
	const double smallVal = 1e-38 ;

	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[0]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Knu((*p1_float)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Knu((*p1_float)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Knu((*p1_float)[count],(*p0)[count]);
	  }
      }
    GM_CC1();
    GM_CV1();
  }
  
  
  // very preliminary version:
  // should not work when N LT 0
  // should return Inf at x==0
  // should not work for x LT 0
    
  BaseGDL* besely_fun(EnvT* e)
  { 
    GM_5P0(1);
    AC_2P1();
    GM_DF2();

    // we need to check if N values (array) are Integer or not
    SizeT count;
    // do we have negative numbers ?
    for (count = 0;count<nElp0;++count)
      if ((*p0)[count] < dzero) // don't know if a "machar" value exists
        e->Throw("Value of X is out of allowed range (Only positive values).");

    int test=0;
    
    for (count = 0;count<nElp1;++count)
      if (abs((*p1_float)[count]-(float)(*p1)[count]) > 0.000001) // don't know if a "machar" value exists
	test=1;

    if (test==0)
      {
	// when X value is below ~1e-20 --> return -Inf (we use log(0.) which gives -Inf)
	const double smallVal = 1e-38 ;

	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[0]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Yn((*p1)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Yn((*p1)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Yn((*p1)[count],(*p0)[count]);
	  }
      }
    else
      {
	// we need to check if N values (array) are positives
	for (count = 0;count<nElp1;++count)
	  if ((*p1)[count] < dzero) // don't know if a "machar" value exists
	    e->Throw("Value of N is out of allowed range (Only positive values when N is non integer).");

	// when X value is below ~1e-20 --> return -Inf (we use log(0.) which gives -Inf)
	const double smallVal = 1e-38 ;

	if(nElp0==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[0]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Ynu((*p1_float)[count],(*p0)[0]);
	  }
	else if(nElp1==1)
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Ynu((*p1_float)[0],(*p0)[count]);
	  }
	else
	  {
	    for (count = 0;count<nElp;++count)
	      if (abs((*p0)[count]) < smallVal)
		(*res)[count] =log(dzero) ;
	      else
		(*res)[count] = gsl_sf_bessel_Ynu((*p1_float)[count],(*p0)[count]);
	  }
      }
    GM_CC1();
    GM_CV1();
  }

} // namespace

