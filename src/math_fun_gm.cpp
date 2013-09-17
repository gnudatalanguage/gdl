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
									\
  DType t0 = e->GetParDefined(0)->Type();				\
                     if (t0 == GDL_COMPLEX || t0 == GDL_COMPLEXDBL)		\
		       e->Throw("Complex not implemented (GSL limitation). ");

#define GM_5P0(a)							\
  e->NParam(a);								\
									\
  DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>(0);				\
  SizeT nElp0 = p0->N_Elements();					\
									\
  if (nElp0 == 0)							\
    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(0));	\
									\
  DType t0 = e->GetParDefined(0)->Type();				\
  if (t0 == GDL_COMPLEX || t0 == GDL_COMPLEXDBL)				\
    e->Throw("Complex not implemented (GSL limitation). ");

#define GM_5P1()							\
  DDoubleGDL* p1 = e->GetParAs<DDoubleGDL>(1);				\
  SizeT nElp1 = p1->N_Elements();					\
  									\
  if (nElp1 == 0)							\
    throw GDLException(e->CallingNode(), "Variable is undefined: "+e->GetParString(1));	\
  									\
  DType t1 = e->GetParDefined(1)->Type();				\
  if (t1 == GDL_COMPLEX || t1 == GDL_COMPLEXDBL)				\
    e->Throw("Complex not implemented (GSL limitation). ");


// Use this macro to define Inf and NaN, number of elements and result in a function with one parameter.
#define GM_DF1()							\
  static DStructGDL *Values =  SysVar::Values();			\
  DDouble d_infinity=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0]; \
  DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0]; \
  									\
  DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);

// Use this macro to define Inf and NaN, number of elements and result in a function with two parameters.
#define GM_DF2()							\
  static DStructGDL *Values =  SysVar::Values();			\
  DDouble d_infinity=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0]; \
  DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0]; \
  									\
  DDoubleGDL* res;							\
  if (nElp0 == 1 && nElp1 == 1)						\
  {                                                                     \
    if (p0->Rank() == 0 && p1->Rank() == 0)                             \
      res = new DDoubleGDL(BaseGDL::NOZERO);				\
    else                                                                \
      res = new DDoubleGDL(1, BaseGDL::NOZERO);				\
  }                                                                     \
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
using std::isnan;

  double gm_expint(int n, double x);
  double gm_lentz(double a[], double b[], double tiny, int n, double eps);

  BaseGDL* erf_fun(EnvT* e)
  {
    GM_5P0(1);
    GM_DF1();

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

  BaseGDL* errorf_fun(EnvT* e)
  {

    // ERRORF is the IDL old name of the ERF function. It is implemented to keep old syntax active.

    return erf_fun(e);
  } // errorf_fun

  BaseGDL* gaussint_fun(EnvT* e)
  {
    GM_5P0(1);
    GM_DF1();

    for (SizeT c = 0; c < nElp0; ++c)
    {
        if ((*p0)[c] == d_infinity)
            (*res)[c] = 1.0;
        else if ((*p0)[c] == -d_infinity)
            (*res)[c] = 0.0;
        else if (isnan((*p0)[c]) == 1)
            (*res)[c] = d_nan;
        else
            (*res)[c] = 0.5*(1.+gsl_sf_erf((*p0)[c]/sqrt(2.)));
    }

    GM_CV1();
  } // gaussint_fun

  BaseGDL* erfc_fun(EnvT* e)
  {
    GM_5P0(1);
    GM_DF1();

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
    GM_5P0(1);
    GM_DF1();

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
    GM_5P0(1);
    GM_DF1();

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
    GM_5P0(2);
    GM_5P1();
    GM_DF2();

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

    GM_CV2();
  } // igamma_fun

  BaseGDL* beta_fun(EnvT* e)
  {
    GM_5P0(2);
    GM_5P1();
    GM_DF2();

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
    GM_2P0(2);
    GM_5P1();
    GM_DF2();

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
