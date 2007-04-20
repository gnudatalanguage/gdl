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

As is on 20/April/2007: Warning : GSL allows only INTEGER type for order 

----------------------- Warning -------------

Since functions are clones derivated from the first one,
please propagates bugs' correction and improvments.
Don't know how to symplified :-(

----------------------- Warning -------------

*/

#include "includefirst.hpp"

#include "math_fun_ac.hpp"
#include <gsl/gsl_sf_bessel.h>

using namespace std;

namespace lib {

  BaseGDL* beseli_fun(EnvT* e)
  {

    // Status 20/04/2007 : X array, N array. Type of N can be only Integer (GSL limit)

    // It is the "matrix"  of following clone below:  beselj_fun, beselk_fun and besely_fun
    // if a bug founded, please propage corrections

    SizeT nParam = e->NParam(2);
  
    // "X" input value or array 
    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == COMPLEX || e->GetParDefined(0)->Type() == COMPLEXDBL)
      e->Throw("Complex BeselI not implemented: ");
    SizeT nEx = xvals->N_Elements();
  
    // "N order" array this field can be an array ...
    // only integer available in GSL for N ...
    //DDoubleGDL* nvals = e->GetParAs<DDoubleGDL>(1);
    DIntGDL* nvals = e->GetParAs<DIntGDL>(1);
    SizeT nEn = nvals->N_Elements();
    
    SizeT count;

    // we need to check if N values (array) are Integer or not
    DFloatGDL* nvals_float = e->GetParAs<DFloatGDL>(1);
    for (count = 0;count<nEn;++count) {
      if (abs((*nvals_float)[count]-(float)(*nvals)[count]) > 0.000001) // don't know if a "machar" value exists
	e->Throw("Only Integer value of N allowed (GSL limitation).");
    }
   
    // what is the maximal size of the output ?
    // when N is an array, we have to return shortest of (x,N)
    SizeT nEmax;
    nEmax = (nEx < nEn)? nEx: nEn ;
    // when N is a single number, we have to return array of length (x)
    if (nEn ==1 ) nEmax=nEx;
    DDoubleGDL* res = new DDoubleGDL(nEmax,BaseGDL::NOZERO);

    if (nEn > 1) {
      for (count = 0;count<nEmax;++count)
	(*res)[count] = gsl_sf_bessel_In((*nvals)[count],(*xvals)[count]);
    } else {
      for (count = 0;count<nEx;++count)
	(*res)[count] = gsl_sf_bessel_In((*nvals)[0],(*xvals)[count]);
    }
    
    static DInt coefKWIx = e->KeywordIx("ITER");
    //if ITER present, not used (return -1)
    if(e->KeywordPresent(coefKWIx)) {
      cout << "ITER keyword not used, always return -1)";
      coefKWIx = -1 ;
    }

    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    //if need, convert things back
    if(e->GetParDefined(0)->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(FLOAT,BaseGDL::CONVERT);
    else
      return res;
  
    return new DByteGDL(0);
  }

  BaseGDL* beselj_fun(EnvT* e)
  {

    // Status 20/04/2007 : X array, N array. Type of N can be only Integer (GSL limit)

    // It is a clone of  beseli_fun, if a bug founded, please propage corrections

    SizeT nParam = e->NParam(2);
    
    // "X" input value or array 
    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == COMPLEX || e->GetParDefined(0)->Type() == COMPLEXDBL)
      e->Throw("Complex BeselJ not implemented: ");
    SizeT nEx = xvals->N_Elements();
  
    // "N order" array this field can be an array ...
    // only integer available in GSL for N ...
    //DDoubleGDL* nvals = e->GetParAs<DDoubleGDL>(1);
    DIntGDL* nvals = e->GetParAs<DIntGDL>(1);
    SizeT nEn = nvals->N_Elements();
    
    SizeT count;

    // we need to check if N values (array) are Integer or not
    DFloatGDL* nvals_float = e->GetParAs<DFloatGDL>(1);
    for (count = 0;count<nEn;++count) {
      if (abs((*nvals_float)[count]-(float)(*nvals)[count]) > 0.000001) // don't know if a "machar" value exists
	e->Throw("Only Integer value of N allowed (GSL limitation).");
    }
   
    // what is the maximal size of the output ?
    // when N is an array, we have to return shortest of (x,N)
    SizeT nEmax;
    nEmax = (nEx < nEn)? nEx: nEn ;
    // when N is a single number, we have to return array of length (x)
    if (nEn ==1 ) nEmax=nEx;
    DDoubleGDL* res = new DDoubleGDL(nEmax,BaseGDL::NOZERO);

    if (nEn > 1) {
      for (count = 0;count<nEmax;++count)
	(*res)[count] = gsl_sf_bessel_Jn((*nvals)[count],(*xvals)[count]);
    } else {
      for (count = 0;count<nEx;++count)
	(*res)[count] = gsl_sf_bessel_Jn((*nvals)[0],(*xvals)[count]);
    }
    
    static DInt coefKWIx = e->KeywordIx("ITER");
    //if ITER present, not used (return -1)
    if(e->KeywordPresent(coefKWIx)) {
      cout << "ITER keyword not used, always return -1)";
      coefKWIx = -1 ;
    }

    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    //if need, convert things back
    if(e->GetParDefined(0)->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(FLOAT,BaseGDL::CONVERT);
    else
      return res;
  
    return new DByteGDL(0);
  }

  // very preliminary version:
  // should not work when N LT 0
  // should return Inf at x==0
  // should not work for x LT 0

  BaseGDL* beselk_fun(EnvT* e)
  {
    SizeT nParam = e->NParam(2);
    SizeT count;
    const double dzero = 0.0000000000000000000 ;
  
    // "X" input value or array 
    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == COMPLEX || e->GetParDefined(0)->Type() == COMPLEXDBL)
      e->Throw("Complex BeselK not implemented: ");
    SizeT nEx = xvals->N_Elements();

    // we need to check if X values (array) are positives
    for (count = 0;count<nEx;++count) {
      if ((*xvals)[count] < dzero) // don't know if a "machar" value exists
	e->Throw("Value of X is out of allowed range (Only positive values).");
    }
  
    // "N order" array this field can be an array ...
    // only integer available in GSL for N ...
    //DDoubleGDL* nvals = e->GetParAs<DDoubleGDL>(1);
    DIntGDL* nvals = e->GetParAs<DIntGDL>(1);
    SizeT nEn = nvals->N_Elements();
    
    // we need to check if N values (array) are Integer or not
    DFloatGDL* nvals_float = e->GetParAs<DFloatGDL>(1);
    for (count = 0;count<nEn;++count) {
      if (abs((*nvals_float)[count]-(float)(*nvals)[count]) > 0.000001) // don't know if a "machar" value exists
	e->Throw("Only Integer value of N allowed (GSL limitation).");
    }
   
    // what is the maximal size of the output ?
    // when N is an array, we have to return shortest of (x,N)
    SizeT nEmax;
    nEmax = (nEx < nEn)? nEx: nEn ;
    // when N is a single number, we have to return array of length (x)
    if (nEn ==1 ) nEmax=nEx;
    DDoubleGDL* res = new DDoubleGDL(nEmax,BaseGDL::NOZERO);

    // when X value is below ~1e-20 --> return -Inf (we use log(0.) which gives -Inf)
    const double smallVal = 1e-38 ;

    if (nEn > 1) {
      for (count = 0;count<nEmax;++count)
	if (abs((*xvals)[count]) < smallVal) {
	  (*res)[count] =log(dzero) ;
	} else {
	  (*res)[count] = gsl_sf_bessel_Kn((*nvals)[count],(*xvals)[count]);
	}
    } else {
      for (count = 0; count<nEmax;++count)
      	if (abs((*xvals)[count]) < smallVal) {
	  (*res)[count] =log(dzero) ;
	} else {
	  (*res)[count] = gsl_sf_bessel_Kn((*nvals)[0],(*xvals)[count]);
	}
    }
    
    static DInt coefKWIx = e->KeywordIx("ITER");
    //if ITER present, not used (return -1)
    if(e->KeywordPresent(coefKWIx)) {
      cout << "ITER keyword not used, always return -1)";
      coefKWIx = -1 ;
    }

    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    //if need, convert things back
    if(e->GetParDefined(0)->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(FLOAT,BaseGDL::CONVERT);
    else
      return res;
  
    return new DByteGDL(0);
  }


  // very preliminary version:
  // should not work when N LT 0
  // should return Inf at x==0
  // should not work for x LT 0
    
  BaseGDL* besely_fun(EnvT* e)
  {  
    SizeT nParam = e->NParam(2);
    SizeT count;
    const double dzero = 0.0000000000000000000 ;
  
    // "X" input value or array 
    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == COMPLEX || e->GetParDefined(0)->Type() == COMPLEXDBL)
      e->Throw("Complex BeselY not implemented: ");
    SizeT nEx = xvals->N_Elements();

    // we need to check if X values (array) are positives
    for (count = 0;count<nEx;++count) {
      if ((*xvals)[count] < dzero) // don't know if a "machar" value exists
	e->Throw("Value of X is out of allowed range (Only positive values).");
    }
  
    // "N order" array this field can be an array ...
    // only integer available in GSL for N ...
    //DDoubleGDL* nvals = e->GetParAs<DDoubleGDL>(1);
    DIntGDL* nvals = e->GetParAs<DIntGDL>(1);
    SizeT nEn = nvals->N_Elements();
    
    // we need to check if N values (array) are Integer or not
    DFloatGDL* nvals_float = e->GetParAs<DFloatGDL>(1);
    for (count = 0;count<nEn;++count) {
      if (abs((*nvals_float)[count]-(float)(*nvals)[count]) > 0.000001) // don't know if a "machar" value exists
	e->Throw("Only Integer value of N allowed (GSL limitation).");
    }
   
    // what is the maximal size of the output ?
    // when N is an array, we have to return shortest of (x,N)
    SizeT nEmax;
    nEmax = (nEx < nEn)? nEx: nEn ;
    // when N is a single number, we have to return array of length (x)
    if (nEn ==1 ) nEmax=nEx;
    DDoubleGDL* res = new DDoubleGDL(nEmax,BaseGDL::NOZERO);

    // when X value is below ~1e-20 --> return -Inf (we use log(0.) which gives -Inf)
    const double smallVal = 1e-38 ;

    if (nEn > 1) {
      for (count = 0;count<nEmax;++count)
	if (abs((*xvals)[count]) < smallVal) {
	  (*res)[count] =log(dzero) ;
	} else {
	  (*res)[count] = gsl_sf_bessel_Yn((*nvals)[count],(*xvals)[count]);
	}
    } else {
      for (count = 0; count<nEmax;++count)
      	if (abs((*xvals)[count]) < smallVal) {
	  (*res)[count] =log(dzero) ;
	} else {
	  (*res)[count] = gsl_sf_bessel_Yn((*nvals)[0],(*xvals)[count]);
	}
    }
    
    static DInt coefKWIx = e->KeywordIx("ITER");
    //if ITER present, not used (return -1)
    if(e->KeywordPresent(coefKWIx)) {
      cout << "ITER keyword not used, always return -1)";
      coefKWIx = -1 ;
    }

    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    //if need, convert things back
    if(e->GetParDefined(0)->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(FLOAT,BaseGDL::CONVERT);
    else
      return res;
  
    return new DByteGDL(0);
}


} // namespace

