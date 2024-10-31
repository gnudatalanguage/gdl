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

#include "envt.hpp"
#include "fftw3.h"
//#include <gsl/gsl_math.h>
//#include "gsl_fun.hpp"

//#undef GDL_DEBUG

namespace lib {

  using namespace std;

  //   static int szdbl=sizeof(double);
  //   static int szflt=sizeof(float);

  //overdim is 1,2...
  template < typename T>
  T* fftw_template(EnvT* e, BaseGDL* p0, SizeT nEl, DLong overdim, SizeT overwrite, int sign, bool recenter) {

	T* res;
	BaseGDL* data;
	Guard<BaseGDL> guard_data;

	// if recenter and inverse (direct > 0) we will work on a "de-centered" p0 variant.
	// and of course not center the result.

	if (recenter && sign == FFTW_BACKWARD) {
	  DLong centerIx[ MAXRANK];
	  for (int i = 0; i < p0->Rank(); ++i) centerIx[i] = (p0->Dim(i) % 2 == 1) ? ((p0->Dim(i)) / 2) + 1 : ((p0->Dim(i)) / 2);
	  data = p0->CShift(centerIx);
	  recenter = false;
	  guard_data.Reset(data);
	} else data = p0;

	if (overwrite == 0)
	  res = new T(data->Dim(), BaseGDL::ZERO);
	else {
	  res = (T*) p0; //we overwrite the real p0.
	  if (e->GlobalPar(0)) e->SetPtrToReturnValue(&e->GetPar(0));
	}

	// WARNING FFTW uses C ROW-MAJOR (C) FORMAT --- Just pass the dimension in reverse order.
	// old standard part - keep it , fftw_plan_dft may be faster than fftw_plan_many_dft.
	// shoudl probably be specialized for 1d and 2d
    if (overdim < 1) { //old standard case
	  int dim[ data->Rank()];
	  for (SizeT i = 0; i < data->Rank(); ++i) {
		dim[i] = (int) data->Dim(data->Rank() - i - 1);
	  }

	  DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (data);
	  DComplexGDL* p0CF = static_cast<DComplexGDL*> (data);
	  if (data->Type() == GDL_COMPLEXDBL) {
		double *dptr;
		dptr = (double*) &(*res)[0];

		fftw_plan p;
		fftw_complex *in, *out;
		in = (fftw_complex *) &(*p0C)[0];
		out = (fftw_complex *) & dptr[0];

		p = fftw_plan_dft((int) data->Rank(), dim, in, out, sign, FFTW_ESTIMATE);

		fftw_execute(p);

		if (sign == FFTW_FORWARD) {
		  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
			for (OMPInt i = 0; i < nEl; ++i) {
			  out[i][0] /= nEl;
			  out[i][1] /= nEl;
			}
		  } else {
			TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
			  for (OMPInt i = 0; i < nEl; ++i) {
			  out[i][0] /= nEl;
			  out[i][1] /= nEl;
			}
		  }
		}

		// 02 06 2010
		//cout << "fftw dest" << endl ;
		fftw_destroy_plan(p); // 1 

	  } else if (data->Type() == GDL_COMPLEX) {
		float *dptrf;
		dptrf = (float*) &(*res)[0];

		fftwf_plan p_f;
		fftwf_complex *in_f, *out_f;
		in_f = (fftwf_complex *) &(*p0CF)[0];
		out_f = (fftwf_complex *) & dptrf[0];

		p_f = fftwf_plan_dft((int) data->Rank(), dim, in_f, out_f, sign, FFTW_ESTIMATE);

		fftwf_execute(p_f);

		if (sign == FFTW_FORWARD) {
		  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
			for (OMPInt i = 0; i < nEl; ++i) {
			  out_f[i][0] /= nEl;
			  out_f[i][1] /= nEl;
			}
		  } else {
			TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
			  for (OMPInt i = 0; i < nEl; ++i) {
			  out_f[i][0] /= nEl;
			  out_f[i][1] /= nEl;
			}
		  }
		}

		// 02 06 2010
		//cout << "fftwF dest" << endl ;
		fftwf_destroy_plan(p_f); // 2

	  }
	  if (recenter) {
		Guard<BaseGDL> guard_res(res);
		DLong centerIx[ MAXRANK];
		for (int i = 0; i < data->Rank(); ++i) centerIx[i] = (p0->Dim(i)) / 2;
		return (T*) res->CShift(centerIx);
	  } else return res;
    }
	
	
    int rank=data->Rank();
	int idist, odist, istride, ostride, howmanyPerCall, howmanyCalls, callOffset;
	
	// Due to the COL-Majorness of the FFTW and the lack of documentation, it seems that the 'Advanced interface' of FFTW cannot be called
	// in the general case just by pretending our array is transposed, as in the basic case. It will 'work' only on the last dimension
	// of the array. So we take advantage a [n,m,p,q...] array is just a bunch of m*p*q*.. [n] arrays for dim 1, or [p*q*..] [n,m] arrays for dim 2 etc.

	howmanyPerCall = 1;
	howmanyCalls=1;
	istride=ostride=1;
	idist = odist = 1; /* our sub-arrays are contiguous in memory with this stride */
	
	for (auto i = 0; i < overdim-1 ; ++i) howmanyPerCall *= data->Dim(i); //number of sub-fft in the [n,m,..overdim] subarrays in the passed data
	istride = ostride = data->Dim().Stride(overdim-1);
	callOffset=data->Dim().Stride(overdim); //in floats or doubles.
	int sublength=data->Dim(overdim-1);
	int n[] = {sublength};
	howmanyCalls=nEl/(howmanyPerCall*sublength);
//	std::cout<<"overdim= "<<overdim<<", callOffset= "<<callOffset<<" DIM=["; for (auto i = 0; i < overdim; ++i) std::cout<<data->Dim(i)<<","; std::cout<<"]\n";
//	std::cout << "howmanyCalls: "<< howmanyCalls <<", istride: "<< istride <<", howmanyPerCall = "<<howmanyPerCall<< " n= [";
//	for (auto i = 0; i < 1; ++i) std::cout<<n[i]<<","; std::cout<<"]\n";

	if (data->Type() == GDL_COMPLEXDBL) {
	  DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (data);
	  fftw_plan p;
	  for (auto j = 0; j < howmanyCalls; ++j) {
		fftw_complex *in, *out;
		in = (fftw_complex *) &(*p0C)[j * callOffset];
		out = (fftw_complex *) &(*res)[j * callOffset];

		p = fftw_plan_many_dft(1, n,
		  howmanyPerCall,
		  in, NULL, istride, idist,
		  out, NULL, ostride, odist,
		  sign, FFTW_ESTIMATE);
		fftw_execute(p);
	  }
	  fftw_destroy_plan(p);
	  fftw_complex *in, *out;
	  in = (fftw_complex *) &(*p0C)[0];
	  out = (fftw_complex *) &(*res)[0];
	  if (sign == FFTW_FORWARD) {
		if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		  for (OMPInt i = 0; i < nEl; ++i) {
			out[i][0] /= sublength;
			out[i][1] /= sublength;
		  }
		} else {
		  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
			for (OMPInt i = 0; i < nEl; ++i) {
			out[i][0] /= sublength;
			out[i][1] /= sublength;
		  }
		}
	  }
	} else if (data->Type() == GDL_COMPLEX) {
	  DComplexGDL* p0CF = static_cast<DComplexGDL*> (data);
	  fftwf_plan p_f;
	  for (auto j = 0; j < howmanyCalls; ++j) {
		fftwf_complex *in_f, *out_f;
		in_f = (fftwf_complex *) &(*p0CF)[j * callOffset];
		out_f = (fftwf_complex *) &(*res)[j * callOffset];

		p_f = fftwf_plan_many_dft(1, n,
		  howmanyPerCall,
		  in_f, NULL, istride, idist,
		  out_f, NULL, ostride, odist,
		  sign, FFTW_ESTIMATE);
		fftwf_execute(p_f);
	  }
	  fftwf_destroy_plan(p_f);
	  fftwf_complex *in_f, *out_f;
	  in_f = (fftwf_complex *) &(*p0CF)[0];
	  out_f = (fftwf_complex *) &(*res)[0];
	  if (sign == FFTW_FORWARD) {
		if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		  for (OMPInt i = 0; i < nEl; ++i) {
			out_f[i][0] /= sublength;
			out_f[i][1] /= sublength;
		  }
		} else {
		  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
			for (OMPInt i = 0; i < nEl; ++i) {
			out_f[i][0] /= sublength;
			out_f[i][1] /= sublength;
		  }
		}
	  }
	}
	if (recenter) {
	  Guard<BaseGDL> guard_res(res);
	  DLong centerIx[ MAXRANK];
	  for (int i = 0; i < data->Rank(); ++i) centerIx[i] = (p0->Dim(i)) / 2;
	  return (T*) res->CShift(centerIx);
	} else return res;
  }

  BaseGDL* fftw_fun(EnvT* e) {
	SizeT nParam = e->NParam(1);
	SizeT overwrite = 0, dbl = 0;
	bool recenter = false;

	int sign = FFTW_FORWARD;

	BaseGDL* p0 = e->GetParDefined(0);

	SizeT nEl = p0->N_Elements();
	if (nEl == 0)
	  throw GDLException(e->CallingNode(),
	  "FFT: Variable is undefined: " + e->GetParString(0));
	
	DLong overdim = -1; //all array
    static int DimEnvIx = e->KeywordIx("DIMENSION");
    if (e->KeywordSet(DimEnvIx)) {
      BaseGDL *DimOfDim = e->GetKW(DimEnvIx);
      if (DimOfDim->N_Elements() > 1) e->Throw("Expression must be a scalar or 1 element array in this context:");
      e->AssureLongScalarKW(DimEnvIx, overdim);
      if ((overdim < 0) || (overdim > p0->Rank())) {e->Throw("Illegal keyword value for DIMENSION.");
      }
    }
	
	if (nParam == 2) {
	  BaseGDL* p1 = e->GetPar(1);
	  if (p1->N_Elements() > 1)
		throw GDLException(e->CallingNode(),
		"FFT: Expression must be a scalar or 1 element array: "
		+ e->GetParString(1));

	  DDoubleGDL* direction =
		static_cast<DDoubleGDL*> (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
	  sign = ((*direction)[0]>= 0.0) ? FFTW_BACKWARD : FFTW_FORWARD;
	  GDLDelete(direction);
	}

	static int doubleIx = e->KeywordIx("DOUBLE");
	static int inverseIx = e->KeywordIx("INVERSE");
	static int overwriteIx = e->KeywordIx("OVERWRITE");
	static int centerIx = e->KeywordIx("CENTER");
	if (e->KeywordSet(doubleIx)) dbl = 1;
	if (e->KeywordSet(inverseIx)) sign = FFTW_BACKWARD;
	if (e->KeywordSet(overwriteIx)) overwrite = 1;
	if (e->KeywordSet(centerIx)) recenter = true;

	// If not global parameter no overwrite
	// ok as we steal it then //if( !e->GlobalPar( 0)) overwrite = 0;

	// If double keyword no overwrite
	if (dbl) overwrite = 0;

	if (p0->Type() == GDL_COMPLEXDBL || p0->Type() == GDL_DOUBLE || dbl) {

	  DComplexDblGDL *p0C;

	  Guard<BaseGDL> guard_p0C;

	  if (p0->Type() != GDL_COMPLEXDBL) {
		p0C = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
		guard_p0C.Init(p0C);
	  } else {
		if (overwrite) {
		  e->StealLocalPar(0);
		  //		e->StealLocalParUndefGlobal(0);
		}
		p0C = (DComplexDblGDL *) p0;
	  }

	  return fftw_template< DComplexDblGDL> (e, p0C, nEl, overdim, overwrite, sign, recenter);

	} else if (p0->Type() == GDL_COMPLEX) {

	  //      DComplexGDL* res;
	  if (overwrite)
		e->StealLocalPar(0);
	  // 		e->StealLocalParUndefGlobal(0);


	  return fftw_template< DComplexGDL> (e, p0, nEl, overdim, overwrite, sign, recenter);

	} else {
	  overwrite = 0;

	  DComplexGDL* p0C = static_cast<DComplexGDL*>
		(p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
	  Guard<BaseGDL> guard_p0C(p0C);
	  return fftw_template< DComplexGDL> (e, p0C, nEl, overdim, overwrite, sign, recenter);

	}
  }


} // namespace

#endif // USE_FFTW
