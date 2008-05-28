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

  static int szdbl=sizeof(double);
  static int szflt=sizeof(float);

  template < typename T>
  T* fftw_template(BaseGDL* p0,
		   SizeT nEl, SizeT dbl, SizeT overwrite, double direct)
  {
    int dim[8];

    T* res;

    if (overwrite == 0)
      res = new T( p0->Dim(), BaseGDL::ZERO);
    else
      res = (T*) p0;


    for( SizeT i=0; i<p0->Rank(); ++i) {
      dim[i] = (int) p0->Dim(p0->Rank()-i-1);
    }

    DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
    DComplexGDL* p0CF = static_cast<DComplexGDL*>( p0);

    if( p0->Type() == COMPLEXDBL) {
      double *dptr;
      dptr = (double*) &(*res)[0];

      fftw_plan p;
      fftw_complex *in, *out;
      in = (fftw_complex *) &(*p0C)[0];
      out = (fftw_complex *) &dptr[0];

      p = fftw_plan_dft((int) p0->Rank(), dim, in, out, (int) direct, 
			FFTW_ESTIMATE);

      fftw_execute(p);

      if (direct == -1) {
	for( SizeT i=0; i<nEl; ++i) {
	  out[i][0] /= nEl;
	  out[i][1] /= nEl;
	}
      }
    }
    else if( p0->Type() == COMPLEX) {
      float *dptrf;
      dptrf = (float*) &(*res)[0];
      
      fftwf_plan p_f;
      fftwf_complex *in_f, *out_f;
      in_f = (fftwf_complex *) &(*p0CF)[0];
      out_f = (fftwf_complex *) &dptrf[0];

      p_f = fftwf_plan_dft((int) p0->Rank(), dim, in_f, out_f, (int) direct, 
			   FFTW_ESTIMATE);

      fftwf_execute(p_f);

      if (direct == -1) {
	for( SizeT i=0; i<nEl; ++i) {
	  out_f[i][0] /= nEl;
	  out_f[i][1] /= nEl;
	}
      }
    }
    
    return res;
  }


  BaseGDL* fftw_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    SizeT overwrite=0, dbl=0;
    SizeT stride;
    SizeT offset;

    // If DIMENSION keyword set then use GSL fft
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
	static_cast<DDoubleGDL*>(p1->Convert2( DOUBLE, BaseGDL::COPY));
      direct = GSL_SIGN((*direction)[0]);
    }

    if( e->KeywordSet(0)) dbl = 1;
    if( e->KeywordSet(1)) direct = +1.0;
    if( e->KeywordSet(2)) overwrite = 1;

    if( p0->Type() == COMPLEXDBL || p0->Type() == DOUBLE || dbl) { 

      DComplexDblGDL *p0C;

      if( p0->Type() != COMPLEXDBL) {
	p0C = static_cast<DComplexDblGDL*>
	  (p0->Convert2( COMPLEXDBL, BaseGDL::COPY));
	auto_ptr<BaseGDL> guard_p0C( p0C); 
      } else p0C = (DComplexDblGDL *) p0;

      return fftw_template< DComplexDblGDL> (p0C, nEl, dbl, overwrite, direct);

    }
    else if( p0->Type() == COMPLEX) {

      DComplexGDL* res;

      return fftw_template< DComplexGDL> (p0, nEl, dbl, overwrite, direct);

    }
    else if (p0->Type() == FLOAT ||
	     p0->Type() == LONG ||
	     p0->Type() == ULONG ||
	     p0->Type() == INT ||
	     p0->Type() == UINT ||
	     p0->Type() == BYTE) {

      overwrite = 0;

      DComplexGDL* p0C = static_cast<DComplexGDL*>
	(p0->Convert2( COMPLEX, BaseGDL::COPY));
      auto_ptr<BaseGDL> guard_p0C( p0C); 
      return fftw_template< DComplexGDL> (p0C, nEl, dbl, overwrite, direct);

    } else {
      DFloatGDL* res = static_cast<DFloatGDL*>
	(p0->Convert2( FLOAT, BaseGDL::COPY));

      return res;

    }
  }

} // namespace

#endif // USE_FFTW
