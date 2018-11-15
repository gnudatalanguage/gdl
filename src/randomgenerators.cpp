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
#include "gtdhelper.hpp" //for gettimeofday()
#else
#include <sys/time.h>
#endif


namespace lib {

  using namespace std;
#ifdef USE_EIGEN
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
//for jumps and parallelism
#include "dSFMT/dSFMT-jump.h"
#include "dSFMT/dSFMT-poly.h"

#define GSL_M_E  2.7182818284590452354 /* e */

  //our own struct to keep up things related to parallel seeds
  //it will contain all 128-bit internal state arrays, one per thread.
  //as the number of threads is not known, it will be initialized at start.
  struct DSFMT_STATE {
    dsfmt_t **r; 
 };
 typedef struct DSFMT_STATE dsfmt_state;
#define DEFINE_NCHUNK_FOR_dSFMT  int nchunk = (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) ? CpuTPOOL_NTHREADS : 1;
 

// This function could prove to be way faster than the function below, provided the parallelization insures
// an alignment on _align16 , i.e., 2 doubles = 128 bits = address%16==0
//
//  int random_uniform(double* res, dsfmt_state state, SizeT nEl)
//  {
//    if (nEl >= dsfmt_get_min_array_size() + 1) {
//      SizeT n = (nEl % 2) ? nEl - 1 : nEl;
//      dsfmt_fill_array_close_open(state.r[0], res, n);
//      if (!(n == nEl)) res[nEl - 1] = dsfmt_genrand_close_open(state.r[0]);
//    } else {
//      for (SizeT i = 0; i < nEl; ++i) res[i] = dsfmt_genrand_close_open(state.r[0]);
//    }
//    return 0;
//  }
  
  int random_uniform(double* res, dsfmt_state state, SizeT nEl)
  {
    //no difficulty as we do not use aligned functions here.
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;

#pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk-1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i=start_index; i<stop_index; ++i) res[i] = dsfmt_genrand_close_open(state.r[thread_id]);
    }
    return 0;
  }

  int random_uniform(float* res, dsfmt_state state, SizeT nEl)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;

#pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk-1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i=start_index; i<stop_index; ++i) res[i] = (float) dsfmt_genrand_close_open(state.r[thread_id]);
    }
    return 0;
  }

  double dsfmt_gauss(dsfmt_t *r, const double sigma)
  {
    double x, y, r2;
    do {
      x = -3 + 2 * dsfmt_genrand_close1_open2(r);//belongs to [1,2): faster
      y = -3 + 2 * dsfmt_genrand_close1_open2(r);
      /* see if it is in the unit circle */
      r2 = x * x + y * y;
    } while (r2 > 1.0 || r2 == 0);
    /* Box-Muller transform */
    double fct = sqrt(-2.0 * log(r2) / r2);
    double current = sigma * y * fct;
    return current;
  }
  
//  //unused, but could prove useful, see comment sbelow.
//  void dsfmt_gauss_array(dsfmt_t *r, double *ret, const SizeT n, const double sigma)
//  {
//    //populate array with random doubles (fastest mode)
//    //use these randoms to make them gaussian until no more available.
//    //complete then with singular entries.
//    //nEl must be even and greater than 382. Only this permits the following code. 
//    //assert(n%2==0); //There is an assert already in dSFMT!
//    
//    dsfmt_fill_array_close1_open2(r, ret, n); //belongs to [1,2): faster
//    for (SizeT i=0; i< n; ++i) ret[i]=-3.+2*ret[i]; //hopefully optimized by compiler!
//    double x, y, r2;
//    SizeT i = 0;
//    SizeT MARGIN=(n/5 > 512)? 512:n/5; //huge safety margin, at least 76. Probability is thus always much less than (1-PI/4)^76=210^-51
//    SizeT index = 0;
//    /* choose x,y in uniform square (-1,-1) to (+1,+1) */
//    do {
//      do {
//        x = ret[i++];
//        y = ret[i++];
//        /* see if it is in the unit circle */
//        r2 = x * x + y * y;
//      } while (r2 > 1.0 || r2 == 0);
//      /* Box-Muller transform */
//      double fct = sqrt(-2.0 * log(r2) / r2);
//      double current = sigma * y * fct;
//      ret[index++] = current;
//      if (index < n-1) {
//        double other = sigma * x * fct;
//        ret[index++] = other;
//      }
//    } while (i < n-MARGIN); //i always > index
//    //finish the few last values
//    for (SizeT k = index; k < n; ++k) ret[k] = dsfmt_gauss(r, sigma);
//  }
  
// see comment about random_uniform (double) above for optimzation possibilities.
//  int random_normal( double* res, dsfmt_state state, SizeT nEl)
//  {
//      if (nEl >= dsfmt_get_min_array_size() + 1) {
//          SizeT n = (nEl % 2) ? nEl - 1 : nEl;
//          dsfmt_gauss_array(state.r[0], res, n, 1.0);
//          if (!(n == nEl)) res[nEl-1] = dsfmt_gauss(state.r[0], 1.0);
//      } else {
//        for (SizeT i = 0; i < nEl; ++i) res[i] = dsfmt_gauss(state.r[0], 1.0);
//      }
//    return 0;
//  }

  int random_normal(double* res, dsfmt_state state, SizeT nEl)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk-1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i=start_index; i<stop_index; ++i) res[i] = dsfmt_gauss(state.r[thread_id],1.0);
    }
    return 0;
  }
  
  int random_normal( float* res, dsfmt_state state, SizeT nEl)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk-1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i=start_index; i<stop_index; ++i) res[i] = (float) dsfmt_gauss(state.r[thread_id],1.0);
    }
    return 0;
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
          x = dsfmt_gauss(r, 1.0); //GSL's method uses gaussian_ziggurat but intent is the same!
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

  int random_gamma(double* res, dsfmt_state state, SizeT nEl, DLong n)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
#pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = dsfmt_ran_gamma_knuth(state.r[thread_id], 1.0 * n, 1.0);
    }
    return 0;
  }

  int random_gamma(float* res, dsfmt_state state, SizeT nEl, DLong n)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
#pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = (float) dsfmt_ran_gamma_knuth(state.r[thread_id], 1.0 * n, 1.0);
    }
    return 0;
  }
  
  int random_binomial(double* res, dsfmt_state state, SizeT nEl, DDoubleGDL* binomialKey)
  {
    //Note: Binomial values are not same IDL.    
    DULong n = (DULong) (*binomialKey)[0];
    DDouble p = (DDouble) (*binomialKey)[1];
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = dsfmt_ran_binomial_knuth(state.r[thread_id], p, n);
    }
    return 0;    
  }
  
  int random_binomial(float* res, dsfmt_state state, SizeT nEl, DDoubleGDL* binomialKey)
  {
    //Note: Binomial values are not same IDL.    
    DULong n = (DULong) (*binomialKey)[0];
    DDouble p = (DDouble) (*binomialKey)[1];
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = (float) dsfmt_ran_binomial_knuth(state.r[thread_id], p, n);
    }
    return 0;    
  }
  
  int random_poisson(double* res, dsfmt_state state, SizeT nEl, DDoubleGDL* poissonKey)
  {
    DDouble mu = (DDouble) (*poissonKey)[0];
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = dsfmt_ran_poisson(state.r[thread_id], mu);
    }
    return 0;
  }

  int random_poisson(float* res, dsfmt_state state, SizeT nEl, DDoubleGDL* poissonKey)
  {
    DDouble mu = (DDouble) (*poissonKey)[0];
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk-1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i=start_index; i<stop_index; ++i) res[i] = (float) dsfmt_ran_poisson(state.r[thread_id], mu);
    }
    return 0;
  }

  int random_dlong(DLong* res, dsfmt_state state, SizeT nEl)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = dsfmt_genrand_int31(state.r[thread_id]); //int31 as in [0..2^31-1] 
    }
    return 0;
  }

  int random_dulong(DULong* res, dsfmt_state state, SizeT nEl)
  {
    DEFINE_NCHUNK_FOR_dSFMT
    SizeT chunksize = nEl / nchunk;
    #pragma omp parallel num_threads(nchunk) if (nchunk > 1)
    {
      int thread_id = currentThreadNumber();
      SizeT start_index, stop_index;
      start_index = thread_id * chunksize;
      if (thread_id != nchunk - 1) //robust wrt. use of threads or not.
      {
        stop_index = start_index + chunksize;
      } else {
        stop_index = nEl;
      }
      SizeT i;
      for (i = start_index; i < stop_index; ++i) res[i] = dsfmt_genrand_uint32(state.r[thread_id]); //int31 as in [0..2^31-1] 
    }
    return 0;
  }  
  
  void set_random_state(dsfmt_t *dsfmt_mem, const DULong64* seedState, const int pos)
  {
    uint64_t *psfmt64 = (uint64_t*) &(dsfmt_mem->status[0].u[0]);
    for (int k = 0; k < DSFMT_N64; ++k) psfmt64[k] = seedState[k];
    dsfmt_mem->idx = pos;
  }

  void get_random_state(EnvT* e, dsfmt_state state, const DULong seed)
  {
    if (e->GlobalPar(0)) {
      DULong64GDL* ret = new DULong64GDL(dimension(1+(DSFMT_N64+1)*maxNumberOfThreads()), BaseGDL::NOZERO);
      DULong64* newstate = (DULong64*) (ret->DataAddr());
      long k=0;
      newstate[k++] = seed;
      for (int ithread=0; ithread < maxNumberOfThreads() ; ++ithread) {
        newstate[k++] = state.r[ithread]->idx;
        uint64_t *psfmt64 = &(state.r[ithread]->status[0].u[0]);
        for (int j = 0; j < DSFMT_N64; ++j) newstate[k++] = psfmt64[j];
      }
      e->SetPar(0, ret);
    }
  }

  // GDL uses now by default the hardware accelerated mersenne twister (dSFMT) written
  // by  Mutsuo Saito (Hiroshima University) and Makoto Matsumoto (The University of Tokyo).
  // This is already two to four times faster than IDL.
  // Use of dSFMT depends on the presence of switches (--no-dSFMT), environment variable (GDL_USE_DSFMT)
  // and if Eigen:: is used (because Eigen:: aligns correctly wrt. the requirements of dSFMT)

  // We moreover definitely speed up random number generation for a very large number of
  // values by parallelizing the code. This is possible within dSFMT, provided one use the dsfmt-jump() function 
  // written by the authors above. It permits to "jump" the seed to a new state as if 2^{128} 
  // random numbers had been generated in the meantime. (This in a random series with a period of 2^19937 !).
  // Note: 2^128 is already way larger than the number of particles in the Universe.
  // The implementation creates maxNumberOfThreads() seed states, separated by a 2^{128} state jump,
  // and run TPOOL_NTHREADS in parallell, each continuing with its own seed.
  
  // The price to pay is that **the produced random numbers are not the same as IDL**.
  // To get values comparable with IDL, but slowly, use the /RAN1 switch (1) (or do not enable dSFMT).  
  // Moreover, the seed arrays are different. Switching from one to another is *NOT* possible as the
  // types and seed lengths are different. Besides, our dSFMT seed is, because of the use of parallel threads
  // to speed up the random generator, approx NTHREADS larger than the IDL one (not a big deal!).
  
  // (1) Why /RAN1? Because this option is present in IDL, and, instead of throwing an error on it,
  // we use it also as a compatibility switch. But in our case the compatibility is with IDL8+
  // results, not with IDL6.
  
#include "dSFMT/dSFMT-jump.c"

 //this initializes parallel states up to min of max_allowed_threads and omp_max_threads.
 //independently of the fact that only a subset of theses thtreads will be used in loops.
 void init_seeds(dsfmt_state state, DULong seed) {
   //populate with seed template state 'temp'
   dsfmt_t temp;   
   dsfmt_init_gen_rand(&temp, seed);
   //sucessively push by 2^128 and copy to next place
   //Note: we use the maximum number of threads allowed as this seed can be replayed
   //after changing the number of threads to be used.
   memcpy((void*)(state.r[0]),(void*)&temp,sizeof(temp));
   for (int i=1; i<maxNumberOfThreads(); ++i) {
     dSFMT_jump(&temp, poly_128);
     memcpy((void*)(state.r[i]),(void*)&temp,sizeof(temp));
   }
 }
  
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
    
    static dsfmt_state dsfmt_mem;
    //initialize only once!
    if (dsfmt_mem.r==NULL) {
      dsfmt_mem.r=(dsfmt_t**)malloc(maxNumberOfThreads()*sizeof(dsfmt_t*));
      {for (int i=0; i< maxNumberOfThreads() ; ++i) dsfmt_mem.r[i]=(dsfmt_t*)malloc(sizeof(dsfmt_t));}
    }

    SizeT nParam = e->NParam(1);

    dimension dim;
    if (nParam > 1) arr(e, dim, 1);

    DULong seed;
    bool initialized=false;

    BaseGDL* p0 = e->GetPar(0);
    bool isAnull = NullGDL::IsNULLorNullGDL(p0);
    if (!isAnull) { //something is passed
      // IDL does not check that the seed sequence has been changed: as long as it is a 628 element Ulong, it takes it
      // and use it as the current sequence (try with "all zeroes").
      // for us, a valid seed sequence is the content of dsfmt_mem.r, i.e, (DSFMT_N64+1)*maxNumberOfThreads(), 
      // plus the memory of the initial seed value.
      if (p0->Type() == GDL_ULONG64) { //good chances we have here a genuine dSFMT seed!
        DULong64GDL* p0L = e->IfDefGetParAs< DULong64GDL>(0);
        if (p0L->N_Elements() == 1 + (DSFMT_N64 + 1) * maxNumberOfThreads()) {
          long k = 0;
          seed = (*p0L)[k++]; //hopefully it is always compatible with an unisgned int32 as reslut of a saved previous seed.
          for (int ithread = 0; ithread < maxNumberOfThreads(); ++ithread) {
            int pos = (*p0L)[k++];
            DULong64 sequence[DSFMT_N64];
            for (int i = 0; i < DSFMT_N64; ++i) sequence[i] = (*p0L)[k++];
            set_random_state(dsfmt_mem.r[ithread], sequence, pos); //initialize each thread seed 
          }
          initialized=true;
        } else { // not a seed sequence: take first value as 32 bit UNsigned integer (for dSFMT compatibility).
          DULongGDL* p02L = e->IfDefGetParAs< DULongGDL>(0);
          if (p02L->N_Elements() > 0) {
            seed = (*p02L)[0];
            //this initialize all the maxNumberOfThreads() parallel states, as a new seed has been given.
            init_seeds(dsfmt_mem, seed);
            initialized=true;
          }
        }
      } else { // not a seed sequence: take first value as 32 bit UNsigned integer (for dSFMT compatibility).
        DULongGDL* p0L = e->IfDefGetParAs< DULongGDL>(0);
        if (p0L->N_Elements() > 0) {
          seed = (*p0L)[0];
          //this initialize all the maxNumberOfThreads() parallel states, as a new seed has been given.
          init_seeds(dsfmt_mem, seed);
          initialized=true;
        }
      }
    }
    if (!initialized) { //initialze with something (/dev/urandom? no: idl uses systime:
      struct timeval tval;
      struct timezone tzone;
      gettimeofday(&tval, &tzone);
      long long int tt = tval.tv_sec * 1e6 + tval.tv_usec; // time in UTC microseconds
      seed = (tt);
      init_seeds(dsfmt_mem, seed);
      initialized=true;
    }

    if (e->KeywordSet(LONGIx)) {
      DLongGDL* res = new DLongGDL(dim, BaseGDL::NOZERO);
      random_dlong((DLong*)res->DataAddr(), dsfmt_mem,res->N_Elements());
      get_random_state(e, dsfmt_mem, seed);
      return res;
    }
    if (e->KeywordSet(ULONGIx)) {
      DULongGDL* res = new DULongGDL(dim, BaseGDL::NOZERO);
      random_dulong((DULong*)res->DataAddr(), dsfmt_mem,res->N_Elements());
      get_random_state(e, dsfmt_mem, seed);
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
        random_gamma((double*)res->DataAddr(), dsfmt_mem,res->N_Elements(), n);
        get_random_state(e, dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_gamma((float*)res->DataAddr(), dsfmt_mem,res->N_Elements(), n);
        get_random_state(e, dsfmt_mem, seed);
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
        random_binomial((double*)res->DataAddr(), dsfmt_mem, res->N_Elements(), binomialKey);
        get_random_state(e, dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_binomial((float*)res->DataAddr(), dsfmt_mem, res->N_Elements(), binomialKey);
        get_random_state(e, dsfmt_mem, seed);
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
        random_poisson((double*)res->DataAddr(), dsfmt_mem, res->N_Elements(), poissonKey);
        get_random_state(e, dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_poisson((float*)res->DataAddr(), dsfmt_mem, res->N_Elements(), poissonKey);
        get_random_state(e, dsfmt_mem, seed);
        return res;
      }
    }

    if (e->KeywordSet(UNIFORMIx) || ((e->GetProName() == "RANDOMU") && !e->KeywordSet(NORMALIx))) {
      if (e->KeywordSet(0)) { // GDL_DOUBLE
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_uniform((double*)res->DataAddr(), dsfmt_mem, res->N_Elements());
        get_random_state(e, dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_uniform((float*)res->DataAddr(), dsfmt_mem, res->N_Elements());
        get_random_state(e, dsfmt_mem, seed);
        return res;
      }      
    }

    if (e->KeywordSet(NORMALIx) || ((e->GetProName() == "RANDOMN") && !e->KeywordSet(UNIFORMIx))) {
      if (e->KeywordSet(0)) { // GDL_DOUBLE
       DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        random_normal((double*)res->DataAddr(), dsfmt_mem, res->N_Elements());
        get_random_state(e, dsfmt_mem, seed);
        return res;
      } else {
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        random_normal((float*)res->DataAddr(), dsfmt_mem, res->N_Elements());
        get_random_state(e, dsfmt_mem, seed);
        return res;
      }
    }
    assert(false);
    return NULL;
  }
#endif
  BaseGDL* random_fun(EnvT* e)
  {
    //switches between gsl-based and parallelized version depending on enviromnent and RAN1 switch.
    //Probably the gsl version could be dropped at some point as the speed gain is more important that everything.
    //USE_EIGEN as long as we have not our own alignment malloc procedure and rely on Eigen:: only.
    
#ifdef USE_EIGEN
    static int RAN1Ix = e->KeywordIx("RAN1");
    static bool warning_about_ran1 = false;
    if (useDSFMTAcceleration && e->KeywordSet(RAN1Ix) && !warning_about_ran1) {
      Message("When using the RAN1 mode, be sure to keep the RAN1 and dSFMT seed arrays in separate variables.");
      warning_about_ran1 = true;
    }
    //we may have set -no-dSFMT, or GDL_NO_DSFMT, or simply /RAN1 only.
    bool use_dsfmt = (!e->KeywordSet(RAN1Ix) && useDSFMTAcceleration == true);

    if (use_dsfmt) return random_fun_dsfmt(e);
    else
#endif      
      return random_fun_gsl(e);
  }
} //namespace lib
