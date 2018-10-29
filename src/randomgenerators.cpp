/***************************************************************************
                          randomgenerators.cpp  -  GDL library function
                             -------------------
    begin                : Oct 26 2018
    copyright            : (C) 2004 by Joel Gales
                         : (C) 2018 G. Duvert 
    email                : see https://github.com/gnudatalanguage/gdl
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************************************************/
  
#include "includefirst.hpp"

#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "dinterpreter.hpp"
#include "nullgdl.hpp"

#include "gsl_fun.hpp"



#ifdef _MSC_VER
#define isnan _isnan
#define isfinite _finite
#endif

namespace lib {

  using namespace std;

#ifndef _MSC_VER
  using std::isnan;
#endif

  /* following are some modified codes taken from the GNU Scientific Library.
   * 
   * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2006, 2007 James Theiler, Brian Gough
   * Copyright (C) 2006 Charles Karney
   * 
   * This program is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 3 of the License, or (at
   * your option) any later version.
   * 
   * This program is distributed in the hope that it will be useful, but
   * WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * General Public License for more details.
   * 
   * You should have received a copy of the GNU General Public License
   * along with this program; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
   */


  // following is the default version that uses the dSFMT algo, with further parallelisation using their jump function
#include "dSFMT/dSFMT.h"
#include "dSFMT/dSFMT-params.h"
#include "dSFMT/dSFMT-common.h"
#define GSL_M_E  2.7182818284590452354 /* e */

  void modified_dsfmt_ran_gaussian_d(dsfmt_t *dsfmt_mem, double *ret, const SizeT nEl, const double sigma)
  {
    //As in modified_gsl_ran_gaussian_d above, but using the fast mersenne twister dSFMT 
    // create an aligned array of random values 1.5 time larger than the necessary. (excess is $\simeq 1-\frac{\pi]{4}$)
#define safety_factor 1.3
    SizeT n = safety_factor*nEl; //something sufficiently large... 
    n=(n % 2) ? (n + 1) : n;
    double* xx = (double*) gdlAlignedMalloc(n * sizeof (double));
    dsfmt_fill_array_open_close(dsfmt_mem, xx, n);
    double x, y, r2;
    SizeT i = 0;
    SizeT index = 0;
    do {
      do {
        x = -1 + 2 * xx[i++];
        y = -1 + 2 * xx[i++];
        /* see if it is in the unit circle */
        r2 = x * x + y * y;
      } while (r2 > 1.0 || r2 == 0);
      /* Box-Muller transform */
      double fct = sqrt(-2.0 * log(r2) / r2);
      double current = sigma * y * fct;
      ret[index++] = current;
      if (index < nEl) {
        double other = sigma * x * fct;
        ret[index++] = other;
      }
    } while (index < nEl);
    gdlAlignedFree((void*) xx);
  }

  double modified_dsfmt_ran_gaussian_d_singlevalue(dsfmt_t *dsfmt_mem, const double sigma)
  {
    double x, y, r2;
    do {
      x = -1 + 2 * dsfmt_genrand_close_open(dsfmt_mem);
      y = -1 + 2 * dsfmt_genrand_close_open(dsfmt_mem);
      /* see if it is in the unit circle */
      r2 = x * x + y * y;
    } while (r2 > 1.0 || r2 == 0);
    /* Box-Muller transform */
    double fct = sqrt(-2.0 * log(r2) / r2);
    double current = sigma * y * fct;
    return current;
  }

  template< typename T1, typename T2>
  int random_uniform(T1* res, dsfmt_t *dsfmt_mem, dimension dim)
  {
    SizeT nEl = res->N_Elements();

    if (nEl >= dsfmt_get_min_array_size() + 1) {
      if (sizeof (T2) == sizeof (double)) {
        SizeT n = (nEl % 2) ? nEl - 1 : nEl;
        dsfmt_fill_array_close_open(dsfmt_mem, (double*) &(*res)[0], n);
        if (!(n == nEl)) (*res)[n] = dsfmt_genrand_open_close(dsfmt_mem);
      } else if (sizeof (T2) == sizeof (float)) {
        SizeT n = (nEl % 2) ? nEl + 1 : nEl;
        double* temp = (double*) gdlAlignedMalloc(n * sizeof (double));
        dsfmt_fill_array_close_open(dsfmt_mem, temp, n);
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = temp[i];
        gdlAlignedFree((void*) temp);
      }
      return 0;
    } else {
      for (SizeT i = 0; i < nEl; ++i) (*res)[i] = (T2) dsfmt_genrand_close_open(dsfmt_mem);
      return 0;
    }
    return 1;
  }

  template< typename T1, typename T2>
  int random_normal(T1* res, dsfmt_t *dsfmt_mem, dimension dim)
  {
    SizeT nEl = res->N_Elements();
    if (nEl >= dsfmt_get_min_array_size() + 1) {
      if (sizeof (T2) == sizeof (double)) {
        modified_dsfmt_ran_gaussian_d(dsfmt_mem, (double*) (&(*res)[0]), nEl, 1.0);
      } else {
        SizeT n = (nEl % 2) ? nEl + 1 : nEl;
        double* temp = (double*) gdlAlignedMalloc(n * sizeof (double));
        modified_dsfmt_ran_gaussian_d(dsfmt_mem, temp, n, 1.0);
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = temp[i];
        gdlAlignedFree((void*) temp);
      }
      return 0;
    } else {
      for (SizeT i = 0; i < nEl; ++i) (*res)[i] = (T2) modified_dsfmt_ran_gaussian_d_singlevalue(dsfmt_mem, 1.0);
    }
    return 1;
  }

  //gamma, poisson and binomial distributions code taken from GSL and updated to use dSFMT generator.

  static double
  dsfmt_gamma_large(dsfmt_t * r, const double a)
  {
    /* Works only if a > 1, and is most efficient if a is large

       This algorithm, reported in Knuth, is attributed to Ahrens.  A
       faster one, we are told, can be found in: J. H. Ahrens and
       U. Dieter, Computing 12 (1974) 223-246.  */

    double sqa, x, y, v;
    sqa = sqrt(2 * a - 1);
    do {
      do {
        y = tan(M_PI * dsfmt_genrand_close_open(r));
        x = sqa * y + a - 1;
      }      while (x <= 0);
      v = dsfmt_genrand_close_open(r);
    } while (v > (1 + y * y) * exp((a - 1) * log(x / (a - 1)) - sqa * y));

    return x;
  }

  static double
  dsfmt_gamma_frac(dsfmt_t * r, const double a)
  {
    /* This is exercise 16 from Knuth; see page 135, and the solution is
       on page 551.  */

    double p, q, x, u, v;

    if (a == 0) {
      return 0;
    }

    p = GSL_M_E / (a + GSL_M_E);
    do {
      u = dsfmt_genrand_close_open(r);
      v = dsfmt_genrand_open_open(r);

      if (u < p) {
        x = exp((1 / a) * log(v));
        q = exp(-x);
      } else {
        x = 1 - log(v);
        q = exp((a - 1) * log(x));
      }
    } while (dsfmt_genrand_close_open(r) >= q);

    return x;
  }

  static double
  dsfmt_ran_gamma_int(dsfmt_t * r, const unsigned int a)
  {
    if (a < 12) {
      unsigned int i;
      double prod = 1;

      for (i = 0; i < a; i++) {
        prod *= dsfmt_genrand_open_open(r);
      }

      /* Note: for 12 iterations we are safe against underflow, since
         the smallest positive random number is O(2^-32). This means
         the smallest possible product is 2^(-12*32) = 10^-116 which
         is within the range of double precision. */

      return -log(prod);
    } else {
      return dsfmt_gamma_large(r, (double) a);
    }
  }

  static double
  dsfmt_ran_gamma_knuth(dsfmt_t * r, const double a, const double b)
  {
    /* assume a > 0 */
    unsigned int na = floor(a);

    if (a >= UINT_MAX) {
      return b * (dsfmt_gamma_large(r, floor(a)) + dsfmt_gamma_frac(r, a - floor(a)));
    } else if (a == na) {
      return b * dsfmt_ran_gamma_int(r, na);
    } else if (na == 0) {
      return b * dsfmt_gamma_frac(r, a);
    } else {
      return b * (dsfmt_ran_gamma_int(r, na) + dsfmt_gamma_frac(r, a - na));
    }
  }

  double
  dsfmt_ran_gamma(dsfmt_t * r, const double a, const double b)
  {
    /* assume a > 0 */

    if (a < 1) {
      double u = dsfmt_genrand_open_open(r);
      return dsfmt_ran_gamma(r, 1.0 + a, b) * pow(u, 1.0 / a);
    }

    {
      double x, v, u;
      double d = a - 1.0 / 3.0;
      double c = (1.0 / 3.0) / sqrt(d);

      while (1) {
        do {
          x = modified_dsfmt_ran_gaussian_d_singlevalue(r, 1.0); //GSL's method uses gaussian_ziggurat but intent is the same!
          v = 1.0 + c * x;
        }        while (v <= 0);

        v = v * v * v;
        u = dsfmt_genrand_open_open(r);

        if (u < 1 - 0.0331 * x * x * x * x)
          break;

        if (log(u) < 0.5 * x * x + d * (1 - v + log(v)))
          break;
      }

      return b * d * v;
    }
  }

  double
  dsfmt_ran_beta(dsfmt_t * r, const double a, const double b)
  {
    if ((a <= 1.0) && (b <= 1.0)) {
      double U, V, X, Y;
      while (1) {
        U = dsfmt_genrand_open_open(r);
        V = dsfmt_genrand_open_open(r);
        X = pow(U, 1.0 / a);
        Y = pow(V, 1.0 / b);
        if ((X + Y) <= 1.0) {
          if (X + Y > 0) {
            return X / (X + Y);
          } else {
            double logX = log(U) / a;
            double logY = log(V) / b;
            double logM = logX > logY ? logX : logY;
            logX -= logM;
            logY -= logM;
            return exp(logX - log(exp(logX) + exp(logY)));
          }
        }
      }
    } else {
      double x1 = dsfmt_ran_gamma(r, a, 1.0);
      double x2 = dsfmt_ran_gamma(r, b, 1.0);
      return x1 / (x1 + x2);
    }
  }

  static unsigned int
  dsfmt_ran_binomial_knuth(dsfmt_t * r, double p, unsigned int n)
  {
    unsigned int i, a, b, k = 0;

    while (n > 10) /* This parameter is tunable */ {
      double X;
      a = 1 + (n / 2);
      b = 1 + n - a;

      X = dsfmt_ran_beta(r, (double) a, (double) b);

      if (X >= p) {
        n = a - 1;
        p /= X;
      } else {
        k += a;
        n = b - 1;
        p = (p - X) / (1 - X);
      }
    }

    for (i = 0; i < n; i++) {
      double u = dsfmt_genrand_close_open(r);
      if (u < p)
        k++;
    }

    return k;
  }

  unsigned int
  dsfmt_ran_poisson(dsfmt_t * r, double mu)
  {
    double emu;
    double prod = 1.0;
    unsigned int k = 0;

    while (mu > 10) {
      unsigned int m = mu * (7.0 / 8.0);

      double X = dsfmt_ran_gamma_int(r, m);

      if (X >= mu) {
        return k + dsfmt_ran_binomial_knuth(r, mu / X, m - 1);
      } else {
        k += m;
        mu -= X;
      }
    }

    /* This following method works well when mu is small */

    emu = exp(-mu);

    do {
      prod *= dsfmt_genrand_close_open(r);
      k++;
    } while (prod > emu);

    return k - 1;

  }

  template< typename T1, typename T2>
  int random_gamma(T1* res, dsfmt_t *dsfmt_mem, dimension dim, DLong n)
  {
    SizeT nEl = res->N_Elements();
    for (SizeT i = 0; i < nEl; ++i) (*res)[ i] =
        (T2) dsfmt_ran_gamma_knuth(dsfmt_mem, 1.0 * n, 1.0);
    return 0;
  }

  template< typename T1, typename T2>
  int random_binomial(T1* res, dsfmt_t *dsfmt_mem, dimension dim, DDoubleGDL* binomialKey)
  {
    SizeT nEl = res->N_Elements();
    //Note: Binomial values are not same IDL.    
    DULong n = (DULong) (*binomialKey)[0];
    DDouble p = (DDouble) (*binomialKey)[1];
    for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = (T2) dsfmt_ran_binomial_knuth(dsfmt_mem, p, n);
    return 0;
  }

  template< typename T1, typename T2>
  int random_poisson(T1* res, dsfmt_t *dsfmt_mem, dimension dim, DDoubleGDL* poissonKey)
  {
    SizeT nEl = res->N_Elements();
    //Removed old code that would return non-integer values for high mu values.
    DDouble mu = (DDouble) (*poissonKey)[0];
    for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = (T2) dsfmt_ran_poisson(dsfmt_mem, mu);
    return 0;
  }

  void set_random_state(dsfmt_t *dsfmt_mem, const unsigned long int* seed, const int pos, const int n)
  {
    uint32_t *psfmt32 = &dsfmt_mem->status[0].u32[0];
    for (int k = 0; k < n; ++k) psfmt32[k] = seed[k];
    dsfmt_mem->idx = pos;
  }

  void get_random_state(EnvT* e, dsfmt_t *dsfmt_mem, const DULong seed)
  {
    if (e->GlobalPar(0)) {
      DULongGDL* ret = new DULongGDL(dimension(DSFMT_N32 + 2), BaseGDL::ZERO); //ZERO as not all elements are initialized here
      DULong* newstate = (DULong*) (ret->DataAddr());
      newstate[0] = seed;
      newstate[1] = dsfmt_mem->idx;
      uint32_t *psfmt32 = &dsfmt_mem->status[0].u32[0];
      for (int k = 0; k < DSFMT_N32; ++k) newstate[k + 2] = psfmt32[k];
      e->SetPar(0, ret);
    }
  }

    // GDL uses now by default the hardware accelerated mersenne twister (dSFMT) two to four times faster than IDL's.
    // This depends on the presence of switches, environment variable and if Eigen:: is used
    // (because Eigen:: aligns correctly wrt. the requirements of dSFMT)
    // However the produced random numbers are not the same.
    // To get values comparable with IDL, but slowly, use the /RAN1 switch (1).  
    // Moreover, the seed arrays are different. Switching from one to another is possible. 
    // If one keeps two different seed arrays, say, "seed_mersenne" and "seed_dsfmt", it is possible to use them both,
    // one with the /RAN1 option, the other without. Neat and simple.
    // (1) Why /RAN1? Because this option is present in IDL, and, instead of throwing an error on it,
    // we use it also as a compatibility switch. But in our case the compatibility is with IDL8+
    // results, not with IDL6.
  
  //***IMPORTANT*** It is possible, even desirable, to further speed up random number generation for a very large number of
  // values by parallelizing the code. This is possible with dSFMT, provided one use the dsfmt-jump() function written
  // by  Mutsuo Saito (Hiroshima University) and Makoto Matsumoto (The University of Tokyo).
  // It permits to "jump" the seed 2^xxxx times (in a random series with a period of 2^19937 !). The implementation would "just"
  // create MAX_NUM_THREADS seed states, separated by a 2^XXX state, and run NUM_THREADS in parallell, each continuing with its own seed.
  // The problem is, how to store these MAX_NUM_THREADS seed states? Especially if some non-threaded calls are made to the dsfmt library
  // in-beteween, because of, say, a single "a=randomu(seed)" statement somewhere: seeds would not be in sync. This may not be a problem,
  // comments welcome on Github.
 
  
  BaseGDL* random_fun_dsfmt(EnvT* e)
  {

    //used in RANDOMU and RANDOMN, which share the SAME KEYLIST. It is safe to speed up by using static ints KeywordIx.
    //Note: LONG or ULONG are obeyed irrespectively of the presence of GAMMA etc which are ignored.
    static int LONGIx = e->KeywordIx("LONG");
    static int ULONGIx = e->KeywordIx("ULONG");
    static int GAMMAIx = e->KeywordIx("GAMMA");
    static int BINOMIALIx = e->KeywordIx("BINOMIAL");
    static int NORMALIx = e->KeywordIx("NORMAL");
    static int POISSONIx = e->KeywordIx("POISSON");
    static int UNIFORMIx = e->KeywordIx("UNIFORM");
    // testing Exclusive Keywords ...
    int exclusiveKW = e->KeywordPresent(GAMMAIx);
    exclusiveKW = exclusiveKW + e->KeywordPresent(BINOMIALIx);
    exclusiveKW = exclusiveKW + e->KeywordPresent(NORMALIx);
    exclusiveKW = exclusiveKW + e->KeywordPresent(POISSONIx);
    exclusiveKW = exclusiveKW + e->KeywordPresent(UNIFORMIx);

    if (exclusiveKW > 1) e->Throw("Conflicting keywords.");
    //idem for LONG and ULONG at the same time!
    exclusiveKW = e->KeywordPresent(LONGIx);
    exclusiveKW = exclusiveKW + e->KeywordPresent(ULONGIx);
    if (exclusiveKW > 1) e->Throw("Conflicting keywords.");

    // the generator structure
    static DULong *seed0 = NULL;
    static dsfmt_t dsfmt_mem;

    //initialise pool. As gsl is still used, we initiate seed0 for both. 
    if (seed0 == NULL) { //initialize pool with systime
      struct timeval tval;
      struct timezone tzone;
      gettimeofday(&tval, &tzone);
      long long int tt = tval.tv_sec * 1e6 + tval.tv_usec; // time in UTC microseconds
      seed0 = new DULong(tt);
      dsfmt_init_gen_rand(&dsfmt_mem, (*seed0));

    }

    SizeT nParam = e->NParam(1);

    dimension dim;
    if (nParam > 1) arr(e, dim, 1);

    DULong seed = *seed0;

    bool isAnull = NullGDL::IsNULLorNullGDL(e->GetPar(0));
    if (!isAnull) {
      DULongGDL* p0L = e->IfDefGetParAs< DULongGDL>(0);
      if (p0L != NULL) // some non-null value passed -> can be a seed state, 628 integers, or use first value:
      {
        // IDL does not check that the seed sequence has been changed: as long as it is a 628 element Ulong, it takes it
        // and use it as the current sequence (try with "all zeroes").
        if (p0L->N_Elements() == DSFMT_N32 + 2 && p0L->Type() == GDL_ULONG) { //a (valid?) seed sequence
          seed = (*p0L)[0];
          int pos = (*p0L)[1];
          int n = DSFMT_N32;
          unsigned long int sequence[n];
          for (int i = 0; i < n; ++i) sequence[i] = (unsigned long int) (*p0L)[i + 2];
          set_random_state(&dsfmt_mem, sequence, pos, n); //the seed 
        } else { // not a seed sequence: take first (IDL does more than this...)
          if (p0L->N_Elements() > 0) {
            seed = (*p0L)[0];
            dsfmt_init_gen_rand(&dsfmt_mem, seed);
          }
        }
      }
    }

    if (e->KeywordSet(LONGIx)) {
      DLongGDL* res = new DLongGDL(dim, BaseGDL::NOZERO);
      SizeT nEl = res->N_Elements();
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = dsfmt_genrand_int31(&dsfmt_mem); //int31 as in [0..2^31-1]
      get_random_state(e, &dsfmt_mem, seed);
      return res;
    }

    if (e->KeywordSet(ULONGIx)) {
      DULongGDL* res = new DULongGDL(dim, BaseGDL::NOZERO);
      SizeT nEl = res->N_Elements();
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = dsfmt_genrand_uint32(&dsfmt_mem);
      get_random_state(e, &dsfmt_mem, seed);
      return res;
    }


    if (e->KeywordPresent(GAMMAIx)) {
      DLong n = -1; //please initialize everything!
      e->AssureLongScalarKW(GAMMAIx, n);
      if (n == 0) {
        DDouble test_n;
        e->AssureDoubleScalarKW(GAMMAIx, test_n);
        if (test_n > 0.0) n = 1;
      }
      if (n <= 0) e->Throw("Value of (Int/Long) GAMMA is out of allowed range: Gamma = 1, 2, 3, ...");
      if (!e->KeywordSet(0)) { //hence:float
        if (n >= 10000000) e->Throw("Value of GAMMA is out of allowed range: Try /DOUBLE.");
      }
      if (e->KeywordSet(0)) { // GDL_DOUBLE
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_gamma< DDoubleGDL, double>(res, &dsfmt_mem, dim, n);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_gamma< DFloatGDL, float>(res, &dsfmt_mem, dim, n);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      }
    }

    DDoubleGDL* binomialKey = e->IfDefGetKWAs<DDoubleGDL>(BINOMIALIx);
    if (binomialKey != NULL) {
      SizeT nBinomialKey = binomialKey->N_Elements();
      if (nBinomialKey != 2)
        e->Throw("Keyword array parameter BINOMIAL must have 2 elements.");

      if ((*binomialKey)[0] < 1.0)
        e->Throw(" Value of BINOMIAL[0] is out of allowed range: n = 1, 2, 3, ...");

      if (((*binomialKey)[1] < 0.0) || ((*binomialKey)[1] > 1.0))
        e->Throw(" Value of BINOMIAL[1] is out of allowed range: 0.0 <= p <= 1.0");

      if (e->KeywordSet(0)) { // GDL_DOUBLE
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_binomial< DDoubleGDL, double>(res, &dsfmt_mem, dim, binomialKey);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_binomial< DFloatGDL, float>(res, &dsfmt_mem, dim, binomialKey);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      }
    }

    DDoubleGDL* poissonKey = e->IfDefGetKWAs<DDoubleGDL>(POISSONIx);
    if (poissonKey != NULL) {
      SizeT nPoissonKey = poissonKey->N_Elements();
      if (nPoissonKey != 1)
        e->Throw("Expression must be a scalar or 1 element array in this context: " + e->GetString(POISSONIx));

      if (e->KeywordSet(0)) { // GDL_DOUBLE
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_poisson< DDoubleGDL, double>(res, &dsfmt_mem, dim, poissonKey);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_poisson< DFloatGDL, float>(res, &dsfmt_mem, dim, poissonKey);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      }
    }

    if (e->KeywordSet(UNIFORMIx) || ((e->GetProName() == "RANDOMU") && !e->KeywordSet(NORMALIx))) {
      if (e->KeywordSet(0)) { // GDL_DOUBLE
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_uniform< DDoubleGDL, double>(res, &dsfmt_mem, dim);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_uniform< DFloatGDL, float>(res, &dsfmt_mem, dim);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      }
    }

    if (e->KeywordSet(NORMALIx) || ((e->GetProName() == "RANDOMN") && !e->KeywordSet(UNIFORMIx))) {
      if (e->KeywordSet(0)) { // GDL_DOUBLE
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_normal< DDoubleGDL, double>(res, &dsfmt_mem, dim);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_normal< DFloatGDL, float>(res, &dsfmt_mem, dim);
        get_random_state(e, &dsfmt_mem, seed);
        return res;
      }
    }
  }

  BaseGDL* random_fun(EnvT* e)
  {
    //switches between gsl-based and parallelized version depending on enviromnent and RAN1 switch.
    //Probably the gsl version could be dropped at some point as the speed gain is more important that everything.

    static int RAN1Ix = e->KeywordIx("RAN1");
    static bool warning_about_ran1 = false;
    if (useDSFMTAcceleration && e->KeywordSet(RAN1Ix) && !warning_about_ran1) {
      Message("When using the RAN1 mode, be sure to keep the RAN1 and dSFMT seed arrays in separate variables.");
      warning_about_ran1 = true;
    }
    //we may have set -no-dSFMT, or GDL_NO_DSFMT, or simply /RAN1 only.
    bool use_dsfmt = (!e->KeywordSet(RAN1Ix) && useDSFMTAcceleration == true);

    if (use_dsfmt) return random_fun_dsfmt(e);
    else return random_fun_gsl(e);
  }
} //namespace lib
