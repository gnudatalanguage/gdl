/***************************************************************************
                          fftw.cpp  -  GDL FFTW library function
                             -------------------
    begin                : Sep 21 2005
    copyright            : (C) 2005 by Joel Gales
    email                : jomoga@users.sourceforge.net
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

#ifdef USE_FFTW

#include <complex>
#include <cmath>

#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "fftw3.h"
#include <gsl/gsl_math.h>
#include "gsl_fun.hpp"

#undef GDL_DEBUG

namespace lib {

  using namespace std;

//   static int szdbl=sizeof(double);
//   static int szflt=sizeof(float);

  template < typename T>
  T* fftw_template(EnvT* e, BaseGDL* p0,
		   SizeT nEl, SizeT dbl, SizeT overwrite, double direct, bool recenter) {
    int dim[MAXRANK];

    T* res;
    BaseGDL* data;
    Guard<BaseGDL> guard_data;

    // if recenter and inverse (direct > 0) we will work on a "de-centered" p0 variant.
    // and of course not center the result.

    if (recenter && direct == 1)
    {
      DLong centerIx[ MAXRANK];
      for (int i = 0; i < p0->Rank(); ++i) centerIx[i] = (p0->Dim(i)%2==1)?((p0->Dim(i))/2)+1:((p0->Dim(i))/2);
      data = p0->CShift(centerIx);
      recenter = false;
      guard_data.Reset(data);
    } else data = p0;

    if (overwrite == 0)
      res = new T(data->Dim(), BaseGDL::ZERO);
    else
    {
      res = (T*) p0; //we overwrite the real p0.
      if (e->GlobalPar(0)) e->SetPtrToReturnValue(&e->GetPar(0));
    }

    for (SizeT i = 0; i < data->Rank(); ++i)
    {
      dim[i] = (int) data->Dim(data->Rank() - i - 1);
    }

    DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (data);
    DComplexGDL* p0CF = static_cast<DComplexGDL*> (data);

    if (data->Type() == GDL_COMPLEXDBL)
    {
      double *dptr;
      dptr = (double*) &(*res)[0];

      fftw_plan p;
      fftw_complex *in, *out;
      in = (fftw_complex *) &(*p0C)[0];
      out = (fftw_complex *) & dptr[0];

      p = fftw_plan_dft((int) data->Rank(), dim, in, out, (int) direct, FFTW_ESTIMATE);

      fftw_execute(p);

      if (direct == -1)
      {
        //        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
        {
#pragma omp for
          for (OMPInt i = 0; i < nEl; ++i)
          {
            out[i][0] /= nEl;
            out[i][1] /= nEl;
          }
        }
      }

      // 02 06 2010
      //cout << "fftw dest" << endl ;
      fftw_destroy_plan(p); // 1 

    } else if (data->Type() == GDL_COMPLEX)
    {
      float *dptrf;
      dptrf = (float*) &(*res)[0];

      fftwf_plan p_f;
      fftwf_complex *in_f, *out_f;
      in_f = (fftwf_complex *) &(*p0CF)[0];
      out_f = (fftwf_complex *) & dptrf[0];

      p_f = fftwf_plan_dft((int) data->Rank(), dim, in_f, out_f, (int) direct, FFTW_ESTIMATE);

      fftwf_execute(p_f);

      if (direct == -1)
      {
        //        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
        {
#pragma omp for
          for (OMPInt i = 0; i < nEl; ++i)
          {
            out_f[i][0] /= nEl;
            out_f[i][1] /= nEl;
          }
        }
      }

      // 02 06 2010
      //cout << "fftwF dest" << endl ;
      fftwf_destroy_plan(p_f); // 2

    }
    if (recenter)
    {
      Guard<BaseGDL> guard_res(res);
      DLong centerIx[ MAXRANK];
      for (int i = 0; i < data->Rank(); ++i) centerIx[i] = (p0->Dim(i))/2;
      return (T*) res->CShift(centerIx);
    } else return res;
  }


  BaseGDL* fftw_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    SizeT overwrite=0, dbl=0;
    bool recenter=false;
    SizeT stride;
    SizeT offset;

    // If DIMENSION keyword set then use GSL fft
    
    
    // which is bad practice. We should do the same as in min_fun or max_fun, use the stride facility.
    if(e->KeywordSet(3)) {
      return fft_fun(e);
    }

    double direct=-1.0;

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "FFT: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "FFT: Variable is undefined: "+e->GetParString(0));

    if( nParam == 2) {
      BaseGDL* p1 = e->GetPar( 1);
      if (p1->N_Elements() > 1)
	throw GDLException( e->CallingNode(), 
			    "FFT: Expression must be a scalar or 1 element array: "
			    +e->GetParString(1));

      DDoubleGDL* direction = 
	static_cast<DDoubleGDL*>(p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
      direct = GSL_SIGN((*direction)[0]);
    }

    if( e->KeywordSet(0)) dbl = 1;
    if( e->KeywordSet(1)) direct = +1.0;
    if( e->KeywordSet(2)) overwrite = 1;
    if( e->KeywordSet(4)) recenter = true;

    // If not global parameter no overwrite
    // ok as we steal it then //if( !e->GlobalPar( 0)) overwrite = 0;

    // If double keyword no overwrite
    if( dbl) overwrite = 0;

    if( p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_DOUBLE || dbl) { 

      DComplexDblGDL *p0C;

      Guard<BaseGDL> guard_p0C;

      if( p0->Type() != GDL_COMPLEXDBL) {
	p0C = static_cast<DComplexDblGDL*>(p0->Convert2( GDL_COMPLEXDBL, BaseGDL::COPY));
        guard_p0C.Init(p0C); 
      } else
      {
	  if( overwrite)
	  {
		e->StealLocalPar(0);
//		e->StealLocalParUndefGlobal(0);
		}
       p0C = (DComplexDblGDL *) p0;
	 }

      return fftw_template< DComplexDblGDL> (e, p0C, nEl, dbl, overwrite, direct, recenter);

    }
    else if( p0->Type() == GDL_COMPLEX) {

      //      DComplexGDL* res;
	  if( overwrite)
	  	e->StealLocalPar(0);
// 		e->StealLocalParUndefGlobal(0);

      return fftw_template< DComplexGDL> (e, p0, nEl, dbl, overwrite, direct, recenter);

    }
    else {
      overwrite = 0;

      DComplexGDL* p0C = static_cast<DComplexGDL*>
	(p0->Convert2( GDL_COMPLEX, BaseGDL::COPY));
      Guard<BaseGDL> guard_p0C( p0C); 
      return fftw_template< DComplexGDL> (e, p0C, nEl, dbl, overwrite, direct, recenter);

    }
  }


} // namespace

#endif // USE_FFTW
