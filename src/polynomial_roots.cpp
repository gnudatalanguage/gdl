/*
 * Laguerre method with deflation for complex polynomial roots
 * Copyright (C) 2025
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */

#include "polynomial_roots.hpp"

#include <complex>
#include <cmath>
#include <vector>
#include <iostream>

#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_vector.h>
// zeropoly
#include <gsl/gsl_poly.h>

using std::complex;
using std::abs;
using std::sqrt;
using namespace std;



  void gsl_err_2_gdl_warn(const char *reason, const char *file, int line, int gsl_errno) {
    static string prefix;
    if (line == -1 && gsl_errno == -1 && file == NULL) prefix = string(reason) + ": ";
    else Warning(prefix + "GSL: " + reason);
  }

  class gsl_err_2_gdl_warn_guard {
    gsl_error_handler_t *old_handler;
  public:
    gsl_err_2_gdl_warn_guard(gsl_error_handler_t *old_handler_) { old_handler = old_handler_; }

    ~gsl_err_2_gdl_warn_guard() { gsl_set_error_handler(old_handler); }
  };


/* -------------------------------------------------- */
/* Polynomial evaluation (Horner)                     */
/* -------------------------------------------------- */
static void poly_eval(
    const std::vector<complex<double>>& a,
    complex<double> x,
    complex<double>& p,
    complex<double>& dp,
    complex<double>& ddp)
{
    int n = (int)a.size() - 1;

    p   = a[n];
    dp  = complex<double>(0.0, 0.0);
    ddp = complex<double>(0.0, 0.0);

    for (int i = n - 1; i >= 0; --i) {
        ddp = ddp * x + 2.0 * dp;
        dp  = dp  * x + p;
        p   = p   * x + a[i];
    }
}

/* -------------------------------------------------- */
/* Laguerre iteration for ONE root                    */
/* -------------------------------------------------- */
static bool laguerre(
    const std::vector<complex<double>>& a,
    complex<double>& x)
{
    const int    max_iter = 100;
    const double eps = 1e-14;
    int n = (int)a.size() - 1;

    for (int iter = 0; iter < max_iter; ++iter) {
        complex<double> p, dp, ddp;
        poly_eval(a, x, p, dp, ddp);

        if (abs(p) < eps)
            return true;

        complex<double> G = dp / p;
        complex<double> H = G * G - ddp / p;
        complex<double> sq = sqrt(
            complex<double>(n - 1) *
            (complex<double>(n) * H - G * G)
        );

        complex<double> d1 = G + sq;
        complex<double> d2 = G - sq;
        complex<double> denom = (abs(d1) > abs(d2)) ? d1 : d2;

	//        if (abs(denom) < eps)
        //    return false;
	// ---- SAFEGUARD ----
        if (abs(denom) < eps) {
	  // small random-looking perturbation
	  x += complex<double>(0.1, 0.1);
	  continue;
        }

        x -= complex<double>(n) / denom;
    }
    return false;
}

/* -------------------------------------------------- */
/* Deflation: divide by (x - root)                    */
/* -------------------------------------------------- */
static void deflate(
    std::vector<complex<double>>& a,
    complex<double> root)
{
    int n = (int)a.size() - 1;
    std::vector<complex<double>> b(n);

    b[n - 1] = a[n];
    for (int i = n - 2; i >= 0; --i)
        b[i] = a[i + 1] + root * b[i + 1];

    a.resize(n);
    for (int i = 0; i < n; ++i)
        a[i] = b[i];
}

/* -------------------------------------------------- */
/* Find ALL roots                                     */
/* -------------------------------------------------- */
std::vector<complex<double>> find_roots(
    const std::vector<complex<double>>& coeffs)
{
    std::vector<complex<double>> a = coeffs;
    int n = (int)a.size() - 1;

    std::vector<complex<double>> roots;
    roots.reserve(n);

    for (int i = n; i > 0; --i) {
        complex<double> x(0.1, 0.0);   // initial guess

        if (!laguerre(a, x)) {
            std::cerr << "Laguerre failed\n";
            break;
        }

        roots.push_back(x);
        deflate(a, x);
    }
    return roots;
}

using namespace std;

namespace lib {
  
  typedef std::complex<double> cplx;
  
  BaseGDL* fz_roots2_fun( EnvT* e)
  {
    
    BaseGDL *p0 = e->GetNumericArrayParDefined(0);
    SizeT n= p0->N_Elements();
    DComplexDblGDL* coeffs = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
    
    bool debug=false;
    
    std::vector<cplx> a(n);
    std::vector<cplx> roots(n-1);
    for(SizeT i=0;i<n;i++) {
      a[i] = cplx((*coeffs)[i].real(), (*coeffs)[i].imag());
      if (debug) cout << a[i] << endl;
    }
    
    roots = find_roots(a);

    if (debug) {
      for (size_t i = 0; i < roots.size(); ++i)
	std::cout << "root " << i << " = " << roots[i] << "\n";
    }
    
    static int DoubleIx = e->KeywordIx("DOUBLE");
    bool doDouble = e->KeywordSet(DoubleIx);
    if (doDouble || p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL *result = new DComplexDblGDL(dimension(n-1), BaseGDL::NOZERO);
      for(SizeT i=0;i<n-1;i++) {
	(*result)[ i] = DComplex(roots[i].real(), roots[i].imag());
      }
      return result;
    } else {
      DComplexGDL *result = new DComplexGDL(dimension(n-1), BaseGDL::NOZERO);
      for(SizeT i=0;i<n-1;i++) {
	(*result)[ i] = roots[i];
      }
      return result;
    } 
  }


// FZ_ROOT:compute polynomial roots
// Ascending coefficient array

  BaseGDL *fz_roots_fun(EnvT *e) {

    BaseGDL *p0 = e->GetNumericArrayParDefined(0);

    if (ComplexType(p0->Type())) {
      return lib::fz_roots2_fun(e);
      //      e->Throw("Polynomials with complex coefficients not supported yet (FIXME!)");
    }

    DDoubleGDL *coef = e->GetParAs<DDoubleGDL>(0);

    if (coef->N_Elements() < 2) {
      e->Throw("Degree of the polynomial must be strictly greather than zero");
    }

    for (int i = 0; i < coef->N_Elements(); i++) {
      if (!isfinite((*coef)[i])) e->Throw("Not a number and infinity are not supported");
    }

    // GSL function

    gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(coef->N_Elements());
    GDLGuard<gsl_poly_complex_workspace> g1(w, gsl_poly_complex_workspace_free);

    SizeT resultSize = coef->N_Elements() - 1;
    vector<double> tmp(2 * resultSize);

    gsl_poly_complex_solve(&(*coef)[0], coef->N_Elements(), w, &(tmp[0]));

    //     gsl_poly_complex_workspace_free (w);

    int debug = 0;
    if (debug) {
      for (int i = 0; i < resultSize; i++) {
	printf("z%d = %+.18f %+.18f\n", i, tmp[2 * i], tmp[2 * i + 1]);
      }
    }
    DComplexDblGDL *result = new DComplexDblGDL(dimension(resultSize), BaseGDL::NOZERO);
    for (SizeT i = 0; i < resultSize; ++i) {
      (*result)[i] = complex<double>(tmp[2 * i], tmp[2 * i + 1]);
    }

    static int doubleIx = e->KeywordIx("DOUBLE");

    return result->Convert2(
			    e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE
			    ? GDL_COMPLEXDBL
			    : GDL_COMPLEX,
			    BaseGDL::CONVERT);
  }



  // SA: helper class for zeropoly
  // an auto_ptr-like class for guarding the poly_complex_workspace
  //   class gsl_poly_complex_workspace_guard
  //   {
  //     gsl_poly_complex_workspace* workspace;
  //     public:
  //     gsl_poly_complex_workspace_guard(gsl_poly_complex_workspace* workspace_) { workspace = workspace_; }
  //     ~gsl_poly_complex_workspace_guard() { gsl_poly_complex_workspace_free(workspace); }
  //   };
  BaseGDL *zeropoly(EnvT *e) {
    
    static int doubleIx = e->KeywordIx("DOUBLE");
    static int jenkisTraubIx = e->KeywordIx("JENKINS_TRAUB");

    //        SizeT nParam = e->NParam(1);
    if (e->KeywordSet(jenkisTraubIx))
      e->Throw("Jenkins-Traub method not supported yet (FIXME!)");

    BaseGDL *p0 = e->GetNumericArrayParDefined(0);
    if (ComplexType(p0->Type()))
      e->Throw("Polynomials with complex coefficients not supported yet (FIXME!)");
    if (p0->Rank() != 1)
      e->Throw("The first argument must be a column vector: " + e->GetParString(0));
    DDoubleGDL *coef = e->GetParAs<DDoubleGDL>(0);

    // GSL error handling
    gsl_error_handler_t *old_handler = gsl_set_error_handler(&gsl_err_2_gdl_warn);
    gsl_err_2_gdl_warn_guard old_handler_guard(old_handler);
    gsl_err_2_gdl_warn(e->GetProName().c_str(), NULL, -1, -1);

    // initializing complex polynomial workspace
    gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(coef->N_Elements());
    GDLGuard<gsl_poly_complex_workspace> g1(w, gsl_poly_complex_workspace_free);
    //     gsl_poly_complex_workspace_guard w_guard(w);

    SizeT resultSize = coef->N_Elements() - 1;
    vector<double> tmp(2 * resultSize);

    if (GSL_SUCCESS != gsl_poly_complex_solve(
					      &(*coef)[0], coef->N_Elements(), w, &(tmp[0]))
	)
      e->Throw("Failed to compute the roots of the polynomial");

    DComplexDblGDL *result = new DComplexDblGDL(dimension(resultSize), BaseGDL::NOZERO);
    for (SizeT i = 0; i < resultSize; ++i)
      (*result)[i] = complex<double>(tmp[2 * i], tmp[2 * i + 1]);

    return result->Convert2(
			    e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE
			    ? GDL_COMPLEXDBL
			    : GDL_COMPLEX,
			    BaseGDL::CONVERT
			    );
  }

} //namespace lib 
