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

/*

  Using the Erfs functions and Gammas function provides by GSL

  ----------------------- Warning -------------

  Since Erfs and Gammas functions are clones derivated from the first one,
  please propagates bugs' correction and improvements.
  Do not know how to simplified :-(

  ----------------------- Note -------------

  See http://www.netlib.org/specfun/erf for further details on how processing Erfcx.

  ----------------------- Note ----------------

  Variables are double because of the GSL.

  ----------------------- Note ----------------

  ExpInt code is partially adapted from Numerical Recipes in C, section 6.3.
  See http://www.nrbook.com/b/bookcpdf/c6-3.pdf

*/

// Constants -----------------------------------------------------------------
#define GM_EPS   1.0e-6
#define GM_ITER  50
#define GM_TINY  1.0e-18

// Macros --------------------------------------------------------------------
#define GM_MIN(a, b) ((a) < (b) ? (a) : (b))

/*
  Table 3-115: IDL Type Codes and Names (IDL Help)
  ================================================

  Type Code | Type Name | Data Type
  -------------------------------------------------
  0     | UNDEFINED | Undefined
  1     | GDL_BYTE      | Byte
  2     | GDL_INT       | Integer
  3     | GDL_LONG      | Longword integer
  4     | GDL_FLOAT     | Floating point
  5     | GDL_DOUBLE    | Double-precision floating
  6     | GDL_COMPLEX   | Complex floating
  7     | GDL_STRING    | String
  8     | GDL_STRUCT    | Structure
  9     | DCOMPLEX  | Double-precision complex
  10     | POINTER   | Pointer
  11     | OBJREF    | Object reference
  12     | GDL_UINT      | Unsigned Integer
  13     | GDL_ULONG     | Unsigned Longword Integer
  14     | GDL_LONG64    | 64-bit Integer
  15     | GDL_ULONG64   | Unsigned 64-bit Integer
*/

// Macros GM_xPy(a): - x must be replaced by the desired type code in the table above (set it to 2 if you want to get an int, etc.);
//                   - P means parameter, don't replace it;
//                   - y must be replaced by the number of the parameter you want to get (it starts at 0);
//                   - a, when you define the 0th parameter, must be replaced by the total number of parameters.
//
//                   Note that macros suite is incomplete.
//                   See below for examples.

#define GM_2P0(a)							\
  e->NParam(a);								\
  									\
  DIntGDL* p0 = e->GetParAs<DIntGDL>(0);				\
  SizeT nElp0 = p0->N_Elements();					\
  									\
  if (nElp0 == 0)							\
    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(0));	\

#define GM_5P0(a)							\
  e->NParam(a);								\
  									\
  DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>(0);				\
  SizeT nElp0 = p0->N_Elements();					\
  									\
  if (nElp0 == 0)							\
    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(0)); \

#define GM_5P1()							\
  DDoubleGDL* p1 = e->GetParAs<DDoubleGDL>(1);				\
  SizeT nElp1 = p1->N_Elements();					\
  									\
  if (nElp1 == 0)							\
    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(1));	\

#define GM_CheckComplex_P0(flag)						\
  DType t0 = e->GetParDefined(0)->Type();				\
  if (flag == 1 && (t0 == GDL_COMPLEX || t0 == GDL_COMPLEXDBL))		\
    e->Throw("Complex not implemented (GSL limitation). ");		\

#define GM_CheckComplex_P1(flag)						\
  DType t1 = e->GetParDefined(1)->Type();				\
  if (flag == 1 && (t1 == GDL_COMPLEX || t1 == GDL_COMPLEXDBL))		\
    e->Throw("Complex not implemented (GSL limitation). ");		\

// Use this macro to define Inf and NaN, number of elements and result in a function with one parameter.
// Note no STATIC for SysVar::, due to .reset 
#define GM_NaN_Inf()							\
  DStructGDL *Values =  SysVar::Values();				\
  DDouble d_infinity=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0]; \
  DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0]; \

#define GM_DF1()							\
  DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);

// Use this macro to define Inf and NaN, number of elements and result in a function with two parameters.
#define GM_DF2()							\
  DDoubleGDL* res;							\
									\
  if (p0->Rank() == 0)							\
    res = new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);			\
  else if (p1->Rank() == 0)						\
    res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);			\
  else if (p0->N_Elements() > p1->N_Elements())				\
    res = new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);			\
  else									\
    res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);			\
  SizeT nElp = res->N_Elements();					\
									\
  //cout << "nElp0 : " << nElp0 << std::endl;				\
  //cout << "nElp1 : " << nElp1 << std::endl;				\
  //cout << "nElp  : " << nElp  << std::endl;

  // Use this macro to convert result in a function with one parameter.
#define GM_CV1() \
  static DInt doubleKWIx = e->KeywordIx("DOUBLE");			\
  									\
  if (t0 != GDL_DOUBLE && !e->KeywordSet(doubleKWIx))			\
    return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);			\
  else									\
    return res;								\
  									\
  return new DByteGDL(0);

// Use this macro to convert result in a function with two parameters.
#define GM_CV2() \
  static DInt doubleKWIx = e->KeywordIx("DOUBLE");			\
  									\
  if (t0 != GDL_DOUBLE && t1 != GDL_DOUBLE && !e->KeywordSet(doubleKWIx))	\
    return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);			\
  else									\
    return res;								\
  									\
  return new DByteGDL(0);

#include "includefirst.hpp"
#include "initsysvar.hpp"  // Used to define Double Infinity and Double NaN

#include "math_fun_gm.hpp"

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_sf_expint.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_psi.h>

#ifdef _MSC_VER
#define isnan _isnan
#define isfinite _finite
#endif

namespace lib
{
using namespace std;
#ifndef _MSC_VER
using std::isnan;
#endif

  double gm_expint(int n, double x);
  double gm_lentz(double a[], double b[], double tiny, int n, double eps);

  BaseGDL* erf_fun(EnvT* e)
  {
    GM_CheckComplex_P0(1);
    GM_5P0(1);
    GM_DF1();
    GM_NaN_Inf();

    for (SizeT c = 0; c < nElp0; ++c)
    {
        if ((*p0)[c] == d_infinity)
            (*res)[c] = 1.0;
        else if ((*p0)[c] == -d_infinity)
            (*res)[c] = -1.0;
        else if (isnan((*p0)[c]) == 1)
            (*res)[c] = d_nan;
        else
            (*res)[c] = gsl_sf_erf((*p0)[c]);
    }

    GM_CV1();
  } // erf_fun

    // ERRORF is the IDL old name of the ERF function. It is implemented to keep old syntax active.
  BaseGDL* errorf_fun(EnvT* e)
  {
    return erf_fun(e);
  } // errorf_fun

  // this a separable function : GAUSSINT(a,b)=GAUSSINT(a)*GAUSSINT(b)
  BaseGDL* gaussint_fun(EnvT* e)
  {
    GM_NaN_Inf();

    SizeT nParam = e->NParam();
    //    cout << nParam << endl;

    // real part of Complex inputs are used
    GM_CheckComplex_P0(0);
    GM_5P0();

    DDoubleGDL* tmp0;
    tmp0=new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);

    DDoubleGDL* tmp1;
    DDoubleGDL* p1 = e->GetParAs<DDoubleGDL>(1);			\
    SizeT nElp1 = p1->N_Elements();					\

    if (nParam == 2) {
      GM_CheckComplex_P1(0);
      p1 = e->GetParAs<DDoubleGDL>(1);
      SizeT nElp1 = p1->N_Elements();
      tmp1=new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);
    } else {
      nElp1=1;
      tmp1=new DDoubleGDL(1.);
      
    }

    // computation for X input
    for (SizeT c = 0; c < nElp0; ++c)
    {
        if ((*p0)[c] == d_infinity)
            (*tmp0)[c] = 1.0;
        else if ((*p0)[c] == -d_infinity)
            (*tmp0)[c] = 0.0;
        else if (isnan((*p0)[c]) == 1)
            (*tmp0)[c] = d_nan;
        else
            (*tmp0)[c] = 0.5*(1.+gsl_sf_erf((*p0)[c]/sqrt(2.)));
    }
    if (nParam == 2) {

    // computation for Y input 
    for (SizeT c = 0; c < nElp1; ++c)
    {
        if ((*p1)[c] == d_infinity)
            (*tmp1)[c] = 1.0;
        else if ((*p1)[c] == -d_infinity)
            (*tmp1)[c] = 0.0;
        else if (isnan((*p1)[c]) == 1)
            (*tmp1)[c] = d_nan;
        else
            (*tmp1)[c] = 0.5*(1.+gsl_sf_erf((*p1)[c]/sqrt(2.)));
    }
    }    
    DDoubleGDL* res;

    if (p0->Rank() == 0) {
      res =new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);
      for (SizeT c = 0; c < nElp1; ++c) 
	(*res)[c] =(*tmp1)[c]*(*tmp0)[0];
    } else {
      if (p1->Rank() == 0) {
	res =new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
	for (SizeT c = 0; c < nElp0; ++c) 
	  (*res)[c] =(*tmp0)[c]*(*tmp1)[0];
      } else {
	SizeT nbp;
	if (nElp0 > nElp1) {
	  nbp=nElp1;
	  res =new DDoubleGDL(p1->Dim(), BaseGDL::NOZERO);
	} else {
	  nbp=nElp0;
	  res =new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
	}
	for (SizeT c = 0; c < nbp; ++c) 
	  (*res)[c] =(*tmp0)[c]*(*tmp1)[c];
      }
    }

    GM_CV1();
  } // gaussint_fun

  BaseGDL* erfc_fun(EnvT* e)
  {
    GM_CheckComplex_P0(1);
    GM_5P0(1);
    GM_DF1();
    GM_NaN_Inf();

    for (SizeT c = 0; c < nElp0; ++c)
    {
        if ((*p0)[c] == d_infinity)
            (*res)[c] = 0.0;
        else if ((*p0)[c] == -d_infinity)
            (*res)[c] = 2.0;
        else if (isnan((*p0)[c]) == 1)
            (*res)[c] = d_nan;
        else
            (*res)[c] = gsl_sf_erfc((*p0)[c]);
    }

    GM_CV1();
  } // erfc_fun

  BaseGDL* gamma_fun(EnvT* e)
  {
    GM_CheckComplex_P0(1);
    GM_5P0(1);
    GM_DF1();
    GM_NaN_Inf();

    for (SizeT c = 0; c < nElp0; ++c)
        if (((*p0)[c] == 0.0) || (((*p0)[c] < 0.0) && ((int)(*p0)[c] == (*p0)[c])) || (*p0)[c] >= GSL_SF_GAMMA_XMAX)
            (*res)[c] = d_infinity;
        else if (isnan((*p0)[c]) == 1 || (*p0)[c] == -d_infinity)
            (*res)[c] = d_nan;
        else
            (*res)[c] = gsl_sf_gamma((*p0)[c]);

    GM_CV1();
  } // gamma_fun

  BaseGDL* lngamma_fun(EnvT* e)
  {
    GM_CheckComplex_P0(1);
    GM_5P0(1);
    GM_DF1();
    GM_NaN_Inf();

    for (SizeT c = 0; c < nElp0; ++c)
        if ((*p0)[c] == 0.0 || (*p0)[c] < 0.0)
            (*res)[c] = d_infinity;
        else if (isnan((*p0)[c]))
            (*res)[c] = d_nan;
        else
            (*res)[c] = gsl_sf_lngamma((*p0)[c]);

    GM_CV1();
  } // lngamma_fun

  BaseGDL* igamma_fun(EnvT* e)
  {
    GM_CheckComplex_P0(1);
    GM_CheckComplex_P1(1);
    GM_5P0(2);
    GM_5P1();
    GM_DF2();
    GM_NaN_Inf();

    if (nElp0 == 1)
    {
        if ((*p0)[0] <= 0.0)
            e->Throw("First argument must be greater than zero (GSL limitation)! ");
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p1)[c] < 0.0)
                e->Throw("Second argument must be greater than or equal to zero! ");
	}
    }
    else if (nElp1 == 1)
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p0)[c] <= 0.0)
                e->Throw("First argument must be greater than zero (GSL limitation)! ");
	}
        if ((*p1)[0] < 0.0)
            e->Throw("Second argument must be greater than or equal to zero! ");
    }
    else
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p0)[c] <= 0.0)
                e->Throw("First argument must be greater than zero (GSL limitation)! ");
            else if ((*p1)[c] < 0.0)
                e->Throw("Second argument must be greater than or equal to zero! ");
	}
    }

    if (nElp0 == 1)
        for (SizeT c = 0; c < nElp; ++c)
            if ((*p0)[0] == d_infinity && (*p1)[c] == d_infinity)
                (*res)[c] = 1.0;
            else if (isnan((*p0)[0]) == 1 || isnan((*p1)[c]))
                (*res)[c] = d_nan;
            else
                (*res)[c] = gsl_sf_gamma_inc_P((*p0)[0], (*p1)[c]);
    else if (nElp1 == 1)
        for (SizeT c = 0; c < nElp; ++c)
            if ((*p0)[c] == d_infinity && (*p1)[0] == d_infinity)
                (*res)[c] = 1.0;
            else if (isnan((*p0)[c]) == 1 || isnan((*p1)[0]))
                (*res)[c] = d_nan;
            else
                (*res)[c] = gsl_sf_gamma_inc_P((*p0)[c], (*p1)[0]);
    else
        for (SizeT c = 0; c < nElp; ++c)
            if ((*p0)[c] == d_infinity && (*p1)[c] == d_infinity)
                (*res)[c] = 1.0;
            else if (isnan((*p0)[c]) == 1 || isnan((*p1)[c]))
                (*res)[c] = d_nan;
            else
                (*res)[c] = gsl_sf_gamma_inc_P((*p0)[c], (*p1)[c]);

    static int methodIx = e->KeywordIx("METHOD");
    if (e->KeywordPresent(methodIx)) {
      DIntGDL* IxMethod = new DIntGDL(nElp, BaseGDL::NOZERO);
      for( SizeT i=0; i<nElp; ++i) (*IxMethod)[ i] = 0;
      e->SetKW( methodIx, IxMethod);
    }

    GM_CV2();
  } // igamma_fun

  BaseGDL* beta_fun(EnvT* e)
  {
    GM_CheckComplex_P0(1);
    GM_CheckComplex_P1(1);
    GM_5P0(2);
    GM_5P1();
    GM_DF2();
    GM_NaN_Inf();

    if (nElp0 == 1)
    {
        bool flag0 = false;
        bool flag1 = false;

        if (isfinite((*p0)[0]) == 0)
            flag0 = true;
        if ((int)(*p0)[0] == (*p0)[0] && (*p0)[0] <= 0.0)
            flag1 = true;

        for (SizeT c = 0; c < nElp; ++c)
        {
            if (flag0 == true || isfinite((*p1)[c]) == 0)
                (*res)[c] = d_nan;
            else if (flag1 == true || ((int)(*p1)[c] == (*p1)[c] && (*p1)[c] <= 0.0))
                (*res)[c] = d_infinity;
            else
                (*res)[c] = gsl_sf_beta((*p0)[0], (*p1)[c]);
        }
    }
    else if (nElp1 == 1)
    {
        bool flag0 = false;
        bool flag1 = false;

        if (isfinite((*p1)[0]) == 0)
            flag0 = true;
        if ((int)(*p1)[0] == (*p1)[0] && (*p1)[0] <= 0.0)
            flag1 = true;

        for (SizeT c = 0; c < nElp; ++c)
        {
            if (isfinite((*p0)[c]) == 0 || flag0 == true)
                (*res)[c] = d_nan;
            else if (((int)(*p0)[c] == (*p0)[c] && (*p0)[c] <= 0.0) || flag1 == true)
                (*res)[c] = d_infinity;
            else
                (*res)[c] = gsl_sf_beta((*p0)[c], (*p1)[0]);
        }
    }
    else
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if (isfinite((*p0)[c]) == 0 || isfinite((*p1)[c]) == 0)
                (*res)[c] = d_nan;
            else if (((int)(*p0)[c] == (*p0)[c] && (*p0)[c] <= 0.0) || ((int)(*p1)[c] == (*p1)[c] && (*p1)[c] <= 0.0))
                (*res)[c] = d_infinity;
            else
                (*res)[c] = gsl_sf_beta((*p0)[c], (*p1)[c]);
        }
    }

    GM_CV2();
  } // beta_fun

  BaseGDL* expint_fun(EnvT* e)
  {
    // real part of Complex inputs are used
    GM_CheckComplex_P0(0);
    GM_CheckComplex_P1(0);

    GM_2P0(2);
    GM_5P1();
    GM_DF2();
    GM_NaN_Inf();

    //Tests -----------------
    if (nElp0 == 1)
    {
        if ((*p0)[0] < 0)
            e->Throw("Arguments must be greater than or equal to zero. ");
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p1)[c] < 0.0)
                e->Throw("Arguments must be greater than or equal to zero. ");
        }
    }
    else if (nElp1 == 1)
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p0)[c] < 0)
                e->Throw("Arguments must be greater than or equal to zero. ");
        }
        if ((*p1)[0] < 0.0)
            e->Throw("Arguments must be greater than or equal to zero. ");
    }
    else
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p0)[c] < 0 || (*p1)[c] < 0.0)
                e->Throw("Arguments must be greater than or equal to zero. ");
        }
    }

    // Calculus --------------
    if (nElp0 == 1)
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p1)[c] == d_infinity)
                (*res)[c] = 0.0;
// initially was:
//            else if (isnan((*p0)[0]) == 1 || isnan((*p1)[c]) == 1 || (*p1)[c] < 0.0)
// but (*p0) is an Int (see GM_2P0) , and insnan() does not work on Ints. Error on some compilers.
            else if (isnan((*p1)[c]) == 1 || (*p1)[c] < 0.0)
                (*res)[c] = d_nan;
            else
                (*res)[c] = gm_expint((*p0)[0], (*p1)[c]);
        }
    }
    else if (nElp1 == 1)
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p1)[0] == d_infinity)
                (*res)[c] = 0.0;
                //same comment as above
            else if (isnan((*p1)[0]) == 1 || (*p1)[0] < 0.0)
                (*res)[c] = d_nan;
            else
                (*res)[c] = gm_expint((*p0)[c], (*p1)[0]);
        }
    }
    else
    {
        for (SizeT c = 0; c < nElp; ++c)
        {
            if ((*p1)[c] == d_infinity)
                (*res)[c] = 0.0;
                //same comment as above
            else if (isnan((*p1)[c]) == 1 || (*p1)[c] < 0.0)
                (*res)[c] = d_nan;
            else
                (*res)[c] = gm_expint((*p0)[c], (*p1)[c]);
        }
    }

    GM_CV2();
  } // expint_fun

  /*
  Modified Lentz's algorithm, as described in Numerical Recipes in C, section 5.2.
  See http://www.nrbook.com/b/bookcpdf/c5-2.pdf
  I strictly used the notation of the book.
  This algorithm is used to evaluate continued fractions.
  */
  double gm_lentz(double a[], double b[], double tiny, int n, double eps)
  {
    double f, C, D, Delta;

    f = b[0];
    if (b[0] == 0.0)
    {
      f = tiny;
    }

    C = f;

    D = 0.0;

    for (int j = 1; j <= n; j++)
    {
      D = b[j]+a[j]*D;
      if (D == 0.0)
      {
        D = tiny;
      }
      C = b[j]+a[j]/C;
      if (C == 0.0)
      {
        C = tiny;
      }
      D = 1.0/D;
      Delta = C*D;
      f = f*Delta;
      if (fabs(Delta-1.0) < eps)
      {
        return f;
      }
    }
    assert(false);
    return 0.0;
  } // gm_lentz

  double gm_expint(int n, double x)
  {
      int m;
      double res;

      m = n-1;

      if (n == 0)
      {
          res = exp(-x)/x;
      }
      else if (x == 0.0)
      {
          res = 1.0/m;
      }
      else if (x <= 1.0) // Series, used for 0 < x <= 1
      {
          double c, Delta;

          res = m != 0 ? 1.0/m : -log(x)-M_EULER;
          c = 1.0;
          for (int i = 1; i <= GM_ITER; i++)
          {
              c *= -x/i;
              if (i != m)
              {
                  Delta = -c/(i-m);
              }
              else
              {
                  Delta = c*(-log(x)+gsl_sf_psi_int(n));
              }
              res += Delta;
              if (fabs(Delta) < fabs(res)*GM_EPS)
              {
                  return res;
              }
          }
      }
      else               // Modified Lentz's algorithm, used for x > 1
      {
          double a[GM_ITER+1], b[GM_ITER+1];
          b[0] = 0.0;
          a[1] = 1.0;
          b[1] = x+n;

          for (int i = 2; i <= GM_ITER; i++)
          {
              a[i] = (1-i)*(n+i-2);
              b[i] = b[i-1]+2.0;
          }
          res = exp(-x)*gm_lentz(a, b, GM_TINY, GM_ITER, GM_EPS);
          return res;
      }
  return res;
  } // gm_expint
} // namespace
