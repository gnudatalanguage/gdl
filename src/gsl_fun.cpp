/***************************************************************************
                          gsl_fun.cpp  -  GDL GSL library function
                             -------------------
    begin                : Jan 20 2004
    copyright            : (C) 2004 by Joel Gales
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

#include <complex>
#include <cmath>

#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "gsl_fun.hpp"

#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_real_float.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_halfcomplex_float.h>
#include <gsl/gsl_fft_complex_float.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

#define LOG10E 0.434294

//#define GDL_DEBUG
#undef GDL_DEBUG


namespace lib {

  using namespace std;

  int szdbl=sizeof(double);
  int szflt=sizeof(float);

  BaseGDL* invert_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    int s;
    float f32;
    double f64;
    double det;
    long singular=0;

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "INVERT: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "INVERT: Variable is undefined: "+
			  e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DComplexGDL* res = new DComplexGDL( p0C->Dim(), BaseGDL::NOZERO);

	float f32_2[2];
	double f64_2[2];

	gsl_matrix_complex *mat = 
	  gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(1));
	gsl_matrix_complex *inverse = 
	  gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));

	for( SizeT i=0; i<nEl; ++i) {
	  memcpy(f32_2, &(*p0C)[i], szdbl);
	  f64 = (double) f32_2[0];
	  memcpy(&mat->data[2*i], &f64, szdbl);

	  f64 = (double) f32_2[1];
	  memcpy(&mat->data[2*i+1], &f64, szdbl);
	}
 
	gsl_linalg_complex_LU_decomp (mat, perm, &s);
       	det = gsl_linalg_complex_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_complex_LU_invert (mat, perm, inverse);
	  if (det * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	for( SizeT i=0; i<nEl; ++i) {
	  memcpy(&f64_2[0], &inverse->data[2*i], szdbl*2);
	  f32_2[0] = (float) f64_2[0];
	  f32_2[1] = (float) f64_2[1];

	  memcpy(&(*res)[i], &f32_2[0], szflt*2);
	}

	gsl_permutation_free(perm);
	gsl_matrix_complex_free(mat);
	gsl_matrix_complex_free(inverse);

	return res;
      }
    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DComplexDblGDL* res = new DComplexDblGDL( p0C->Dim(), BaseGDL::NOZERO);

	gsl_matrix_complex *mat = 
	  gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(1));
	gsl_matrix_complex *inverse = 
	  gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));

	memcpy(mat->data, &(*p0C)[0], nEl*szdbl*2);

	gsl_linalg_complex_LU_decomp (mat, perm, &s);
	det = gsl_linalg_complex_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_complex_LU_invert (mat, perm, inverse);
	  if (det * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	memcpy(&(*res)[0], inverse->data, nEl*szdbl*2);

	gsl_permutation_free(perm);
	gsl_matrix_complex_free(mat);
	gsl_matrix_complex_free(inverse);
	return res;
      }
    else if( p0->Type() == DOUBLE)
      {
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);

	gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(1));
	gsl_matrix *inverse = gsl_matrix_calloc(p0->Dim(0), p0->Dim(1));
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));

	memcpy(mat->data, &(*p0D)[0], nEl*szdbl);

	gsl_linalg_LU_decomp (mat, perm, &s);
	det = gsl_linalg_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_LU_invert (mat, perm, inverse);
	  if (det * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	if (nParam == 2) {
	    BaseGDL** p1L = &e->GetPar( 1);
            delete (*p1L); 
	    *p1L = new DLongGDL( singular);
	}

	memcpy(&(*res)[0], inverse->data, nEl*szdbl);

	gsl_permutation_free(perm);
	gsl_matrix_free(mat);
	gsl_matrix_free(inverse);

	return res;
      }
    else if( p0->Type() == FLOAT ||
	     p0->Type() == LONG ||
	     p0->Type() == ULONG ||
	     p0->Type() == INT ||
	     p0->Type() == UINT ||
	     p0->Type() == BYTE)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DLongGDL* p0L = static_cast<DLongGDL*>( p0);
	DULongGDL* p0UL = static_cast<DULongGDL*>( p0);
	DIntGDL* p0I = static_cast<DIntGDL*>( p0);
	DUIntGDL* p0UI = static_cast<DUIntGDL*>( p0);
	DByteGDL* p0B = static_cast<DByteGDL*>( p0);

	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);

	gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(1));
	gsl_matrix *inverse = gsl_matrix_calloc(p0->Dim(0), p0->Dim(1));
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));

	for( SizeT i=0; i<nEl; ++i) {
	  switch ( p0->Type()) {
	  case FLOAT: f64 = (double) (*p0F)[i]; break;
	  case LONG:  f64 = (double) (*p0L)[i]; break;
	  case ULONG: f64 = (double) (*p0UL)[i]; break;
	  case INT:   f64 = (double) (*p0I)[i]; break;
	  case UINT:  f64 = (double) (*p0UI)[i]; break;
	  case BYTE:  f64 = (double) (*p0B)[i]; break;
	  }
	  memcpy(&mat->data[i], &f64, szdbl);
	}

	gsl_linalg_LU_decomp (mat, perm, &s);
	det = gsl_linalg_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_LU_invert (mat, perm, inverse);
	  if (det * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	if (nParam == 2) {
	    BaseGDL** p1L = &e->GetPar( 1);
            delete (*p1L); 
	    *p1L = new DLongGDL( singular);
	}

	for( SizeT i=0; i<nEl; ++i) {
	  f32 = (float) inverse->data[i];
	  memcpy(&(*res)[i], &f32, 4);
	}

	gsl_permutation_free(perm);
	gsl_matrix_free(mat);
	gsl_matrix_free(inverse);

	return res;
      }
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));

	return res;
      }
  }


  template< typename T1, typename T2>
  int cp2data2_template( BaseGDL* p0, T2* data, SizeT nEl, 
			 SizeT offset, SizeT stride)
  {
    T1* p0c = static_cast<T1*>( p0);
    for( SizeT i=0; i<nEl; ++i) 
      data[2*(i*stride+offset)] = (T2) (*p0c)[i*stride+offset]; 

    return 0;
  }


  template< typename T>
  int cp2data_template( BaseGDL* p0, T* data, SizeT nEl, 
			 SizeT offset, SizeT stride)
  {
    switch ( p0->Type()) {
    case DOUBLE: 
      cp2data2_template< DDoubleGDL, T>( p0, data, nEl, offset, stride);
      break;
    case FLOAT: 
      cp2data2_template< DFloatGDL, T>( p0, data, nEl, offset, stride);
      break;
    case LONG:
      cp2data2_template< DLongGDL, T>( p0, data, nEl, offset, stride);
      break;
    case ULONG: 
      cp2data2_template< DULongGDL, T>( p0, data, nEl, offset, stride);
      break;
    case INT: 
      cp2data2_template< DIntGDL, T>( p0, data, nEl, offset, stride);
      break;
    case UINT: 
      cp2data2_template< DUIntGDL, T>( p0, data, nEl, offset, stride);
      break;
    case BYTE: 
      cp2data2_template< DByteGDL, T>( p0, data, nEl, offset, stride);
      break;
    }
  }


  template< typename T>
  int unpack_real_mxradix_template(T *dptr, SizeT nEl, double direct, 
				   SizeT offset, SizeT stride) {
  
    if (direct == -1) {
      for( SizeT i=0; i<nEl; ++i) dptr[2*(i*stride+offset)] /= nEl;
    }

    for( SizeT i=1; i<nEl/2+(nEl%2); ++i) {
      dptr[2*stride*i+1+2*offset]       = +dptr[2*stride*2*i+2*offset];
      dptr[2*stride*(nEl-i)+1+2*offset] = -dptr[2*stride*2*i+2*offset];
    }

    for( SizeT i=2; i<nEl/2+(nEl%2); ++i) {
      dptr[2*stride*i+2*offset]       = +dptr[2*stride*(2*i-1)+2*offset];
    }

    for( SizeT i=2; i<nEl/2+(nEl%2); ++i) {
      dptr[2*stride*(nEl-i)+2*offset] = +dptr[2*stride*i+2*offset];
    }

    dptr[1] = 0;
    if ((nEl%2) == 0) 
      dptr[stride*nEl+2*offset] = dptr[2*stride*(nEl-1)+2*offset];
    dptr[2*stride*(nEl-1)+2*offset] = dptr[2*stride+2*offset];


    if (direct == + 1) {
      for( SizeT i=1; i<nEl; ++i) 
	dptr[2*(i*stride+offset)+1] = -dptr[2*(i*stride+offset)+1];
    }
  }

  template< typename T, typename T1, typename T2>
  int real_fft_transform_template(BaseGDL* p0, T *dptr, SizeT nEl, 
				  double direct, 
				  SizeT offset, SizeT stride, SizeT radix2,
				  int (*complex_radix2_forward) 
				  (T[], const size_t, size_t),
				  int (*complex_radix2_backward) 
				  (T[], const size_t, size_t),
				  int (*real_transform)
				  (T[], const size_t, size_t,
				   const T1*, T2*),
				  T1 *(*wavetable_alloc)(size_t),
				  T2 *(*workspace_alloc)(size_t),
				  void (*wavetable_free)(T1*),
				  void (*workspace_free)(T2*))
  {

    cp2data_template< T>( p0, dptr, nEl, offset, stride);

    if (radix2) {

      if (direct == -1) {
	(*complex_radix2_forward) (&dptr[2*offset], stride, nEl);
	for( SizeT i=0; i<nEl; ++i) 
	  ((std::complex<T> &) dptr[2*(i*stride+offset)]) /= nEl;
      } else if (direct == +1) {
	(*complex_radix2_backward) (&dptr[2*offset], stride, nEl);
      }
    }
    else if (!radix2) {

      T1 *wave;
      T2 *work;
      
      work = (*workspace_alloc) (nEl);
      wave = (*wavetable_alloc) (nEl);

      (*real_transform) (&dptr[2*offset], 2*stride, nEl, wave, work);

      unpack_real_mxradix_template< T>( dptr, nEl, direct, offset, stride);

      (*workspace_free) (work);
      (*wavetable_free) (wave);
    }
  }


  template< typename T, typename T1, typename T2>
  int complex_fft_transform_template(BaseGDL* p0, T *dptr, SizeT nEl, 
				     double direct, 
				     SizeT offset, SizeT stride, SizeT radix2,
				     int (*complex_radix2_forward) 
				     (T[], const size_t, size_t),
				     int (*complex_radix2_backward) 
				     (T[], const size_t, size_t),
				     int (*complex_forward_transform)
				     (T[], const size_t, size_t,
				      const T1*, T2*),
				     int (*complex_backward_transform)
				     (T[], const size_t, size_t,
				      const T1*, T2*),
				     T1 *(*wavetable_alloc)(size_t),
				     T2 *(*workspace_alloc)(size_t),
				     void (*wavetable_free)(T1*),
				     void (*workspace_free)(T2*))
  {
    if (radix2) {

      if (direct == -1) {
	(*complex_radix2_forward) (&dptr[2*offset], stride, nEl);
	for( SizeT i=0; i<nEl; ++i) 
	  ((std::complex<T> &) dptr[2*(i*stride+offset)]) /= nEl;
      } else if (direct == +1) {
	(*complex_radix2_backward) (&dptr[2*offset], stride, nEl);
      }
    }
    else if (!radix2) {

      T1 *wave;
      T2 *work;
      
      work = (*workspace_alloc) (nEl);
      wave = (*wavetable_alloc) (nEl);

      if (direct == -1) {
	(*complex_forward_transform) (&dptr[2*offset], stride, nEl, 
				      wave, work);
	for( SizeT i=0; i<nEl; ++i) 
	  ((std::complex<T> &) dptr[2*stride*i+2*offset]) /= nEl;

      } else if (direct == +1) {
	(*complex_backward_transform) (&dptr[2*offset], stride, nEl, 
				       wave, work);
      }

      (*workspace_free) (work);
      (*wavetable_free) (wave);
    }
  }


  template < typename T>
  T* fft_template(BaseGDL* p0,
		  SizeT nEl, SizeT dbl, SizeT overwrite, double direct)
  {
    SizeT offset;
    SizeT stride;

    T* res;

    if (overwrite == 0)
      res = new T( p0->Dim(), BaseGDL::ZERO);
    else
      res = (T*) p0;


    if( p0->Rank() == 1) {
      offset=0;
      stride=1;
	fft_1d( p0, &(*res)[0], nEl, offset, stride, 
		direct, dbl);
    }

    if( p0->Rank() == 2) {
      stride=p0->Dim(0);
      for( SizeT i=0; i<p0->Dim(0); ++i) {
	fft_1d( p0, &(*res)[0], p0->Dim(1), i, stride, 
		direct, dbl);
      }
      for( SizeT i=0; i<p0->Dim(1); ++i) {
	fft_1d( res, &(*res)[0], 
		p0->Dim(0), i*p0->Dim(0), 1, 
		direct, dbl);
      }
    }

    if( p0->Rank() >= 3) {
      unsigned char *used = new unsigned char [nEl];

      stride = nEl;
      for( SizeT i=p0->Rank(); i<nEl; ++i) used[i] = 0;

      for (SizeT k=p0->Rank(); k>0; --k) {
	for( SizeT i=0; i<nEl; ++i) used[i] = 0;
	stride /= p0->Dim(k-1);

	SizeT cnt=1;
	offset = 0;
	while(cnt <= nEl/p0->Dim(k-1)) {
	  if (used[offset] != 1) {
	    cnt++;
	    for( SizeT i=0; i<p0->Dim(k-1); ++i) 
	      used[offset+i*stride] = 1;
	    if (k == p0->Rank())
	      fft_1d( p0, &(*res)[0], p0->Dim(k-1), offset, stride, 
		      direct, dbl);
	    else
	      fft_1d( res, &(*res)[0], p0->Dim(k-1), offset, stride, 
		      direct, dbl);
	  }
	  offset++;
	}
      }
      delete used;
    }

    return res;
  }


  BaseGDL* fft_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    SizeT overwrite=0, dbl=0;
    SizeT stride;
    SizeT offset;

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

      return fft_template< DComplexDblGDL> (p0, nEl, dbl, overwrite, direct);

    }
    else if( p0->Type() == COMPLEX) {

      DComplexGDL* res;

      return fft_template< DComplexGDL> (p0, nEl, dbl, overwrite, direct);

    }
    else if (p0->Type() == FLOAT ||
	     p0->Type() == LONG ||
	     p0->Type() == ULONG ||
	     p0->Type() == INT ||
	     p0->Type() == UINT ||
	     p0->Type() == BYTE) {

      overwrite = 0;
      return fft_template< DComplexGDL> (p0, nEl, dbl, overwrite, direct);

    } else {
      DFloatGDL* res = static_cast<DFloatGDL*>
	(p0->Convert2( FLOAT, BaseGDL::COPY));

      return res;

    }
  }


  int fft_1d( BaseGDL* p0, void* data, SizeT nEl, SizeT offset, SizeT stride, 
	      double direct, SizeT dbl)
  {
    float f32[2];
    double f64[2];

    // Determine if radix2
    SizeT radix2 = 0;
    for( SizeT i=0; i<32; ++i) {
      if (nEl == (2 << i)) {
	radix2 = 1;
	break;
      }
    }

    if( p0->Type() == COMPLEX && dbl == 0)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	float *dptr;
	dptr = (float*) data;

	if (stride == 1 && offset == 0) {
	  if ((void*) dptr != (void*) &(*p0C)[0]) 
	    memcpy(dptr, &(*p0C)[0], szflt*2*nEl);
	} else {
	  for( SizeT i=0; i<nEl; ++i) {
	    memcpy(&dptr[2*(i*stride+offset)], &(*p0C)[i*stride+offset], szflt*2);
	  }
	}

	complex_fft_transform_template<float, 
	  gsl_fft_complex_wavetable_float,
	  gsl_fft_complex_workspace_float> 
	  (p0, dptr, nEl, direct, offset, stride, radix2,
	   gsl_fft_complex_float_radix2_forward,
	   gsl_fft_complex_float_radix2_backward,
	   gsl_fft_complex_float_forward,
	   gsl_fft_complex_float_backward,
	   gsl_fft_complex_wavetable_float_alloc,
	   gsl_fft_complex_workspace_float_alloc,
	   gsl_fft_complex_wavetable_float_free,
	   gsl_fft_complex_workspace_float_free);

	return 0;
      }
    else if( p0->Type() == COMPLEXDBL || 
	     (p0->Type() == COMPLEX && dbl))
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DComplexGDL* p0CF = static_cast<DComplexGDL*>( p0);

	double *dptr;
	dptr = (double*) data;

	if( p0->Type() == COMPLEXDBL) {
	  for( SizeT i=0; i<nEl; ++i) {
	    memcpy(&dptr[2*(i*stride+offset)], 
		   &(*p0C)[i*stride+offset], szdbl*2);
	  }
	}
	else if( p0->Type() == COMPLEX) {
	  DComplexDbl c128;
	  for( SizeT i=0; i<nEl; ++i) {
	    c128 = (*p0CF)[i*stride+offset];
	    memcpy(&dptr[2*(i*stride+offset)], &c128, 2*szdbl);
	  }
	}

	complex_fft_transform_template<double, 
	  gsl_fft_complex_wavetable,
	  gsl_fft_complex_workspace> 
	  (p0, dptr, nEl, direct, offset, stride, radix2,
	   gsl_fft_complex_radix2_forward,
	   gsl_fft_complex_radix2_backward,
	   gsl_fft_complex_forward,
	   gsl_fft_complex_backward,
	   gsl_fft_complex_wavetable_alloc,
	   gsl_fft_complex_workspace_alloc,
	   gsl_fft_complex_wavetable_free,
	   gsl_fft_complex_workspace_free);
	
	return 0;
      }
    else if( p0->Type() == DOUBLE || dbl) 
      {
	double *dptr;
	dptr = (double*) data;

	real_fft_transform_template<double, 
	  gsl_fft_real_wavetable,
	  gsl_fft_real_workspace> 
	  (p0, dptr, nEl, direct, offset, stride, radix2,
	   gsl_fft_complex_radix2_forward,
	   gsl_fft_complex_radix2_backward,
	   gsl_fft_real_transform,
	   gsl_fft_real_wavetable_alloc,
	   gsl_fft_real_workspace_alloc,
	   gsl_fft_real_wavetable_free,
	   gsl_fft_real_workspace_free);

	//	printf("real_fft_transform_template\n");

	return 0;
      }
    else if( p0->Type() == FLOAT ||
	     p0->Type() == LONG ||
	     p0->Type() == ULONG ||
	     p0->Type() == INT ||
	     p0->Type() == UINT ||
	     p0->Type() == BYTE)
      {
	float *dptr;
	dptr   = (float*) data;

	real_fft_transform_template<float, 
	  gsl_fft_real_wavetable_float,
	  gsl_fft_real_workspace_float> 
	  (p0, dptr, nEl, direct, offset, stride, radix2,
	   gsl_fft_complex_float_radix2_forward,
	   gsl_fft_complex_float_radix2_backward,
	   gsl_fft_real_float_transform,
	   gsl_fft_real_wavetable_float_alloc,
	   gsl_fft_real_workspace_float_alloc,
	   gsl_fft_real_wavetable_float_free,
	   gsl_fft_real_workspace_float_free);

	//	printf("real_fft_transform_template (float)\n");

	return 0;
      }
  }


  template< typename T1, typename T2>
  int random_template( EnvT* e, T1* res, gsl_rng *r, 
		       dimension dim, 
		       DDoubleGDL* binomialKey, DDoubleGDL* poissonKey)
  {
    SizeT nEl = res->N_Elements();

    if( e->KeywordSet(1)) {// GAMMA
      DLong n;
      e->AssureLongScalarKWIfPresent( "GAMMA", n);
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = 
				    (T2) gsl_ran_gamma_int (r,n);
    } else if( e->KeywordSet(3)) { // NORMAL
      SizeT nEl = res->N_Elements();
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] =
				    (T2) gsl_ran_ugaussian (r);
    } else if( e->KeywordSet(4)) { // BINOMIAL
      if (binomialKey != NULL) {
	DULong  n = (DULong)  (*binomialKey)[0];
	DDouble p = (DDouble) (*binomialKey)[1];
	SizeT nEl = res->N_Elements();
	for( SizeT i=0; i<nEl; ++i) (*res)[ i] =
				      (T2) gsl_ran_binomial (r, p, n);
      }
    } else if( e->KeywordSet(5)) { // POISSON
      if (poissonKey != NULL) {
	DDouble mu = (DDouble) (*poissonKey)[0];
	SizeT nEl = res->N_Elements();
	for( SizeT i=0; i<nEl; ++i) (*res)[ i] =
				      (T2) gsl_ran_poisson (r, mu);
      }
    } else if( e->KeywordSet(6)) { // UNIFORM
      SizeT nEl = res->N_Elements();
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] =
				    (T2) gsl_rng_uniform (r);
    } else if ( e->GetProName() == "RANDOMU") {
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = 
				    (T2) gsl_rng_uniform (r);
    } else if ( e->GetProName() == "RANDOMN") {
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = 
				    (T2) gsl_ran_ugaussian (r);
    }

    return 0;
  }


  BaseGDL* random_fun( EnvT* e)
  {
    const unsigned long seedMul = 65535;

    SizeT nParam = e->NParam( 1);

    dimension dim;
    if( nParam > 1)
      arr( e, dim, 1);

    DLongGDL* seed;
    static DLong seed0 = 0;

    gsl_rng *r;

    if( e->GlobalPar( 0))
      {
	DLongGDL* p0L = e->IfDefGetParAs< DLongGDL>( 0);
	if( p0L != NULL) // defined global -> use and update
	  {
	    seed0 = (*p0L)[ 0];	    

	    r = gsl_rng_alloc (gsl_rng_mt19937);
	    gsl_rng_set (r, seed0);

	    seed0 += dim.N_Elements() * seedMul; // avoid repetition in next call
	    // if called with undefined global

	    seed = new DLongGDL( seed0);
	    e->SetPar( 0, seed);
	  }
	else // undefined global -> init
	  {
	    if( seed0 == 0) // first time
	      {
		time_t t1;
		time(&t1);
		seed0 = static_cast<DLong>( t1);
	      }

	    r = gsl_rng_alloc (gsl_rng_mt19937);
	    gsl_rng_set (r, seed0);

	    seed0 += dim.N_Elements() * seedMul; // avoid repetition in next call
	    // which would be defined global if used in a loop
	    
	    seed = new DLongGDL( seed0);
	    e->SetPar( 0, seed);
	  }
      } 
    else // local (always defined) -> just use it
      {
	seed = e->GetParAs< DLongGDL>( 0);
	seed0 = (*seed)[0];

	r = gsl_rng_alloc (gsl_rng_mt19937);
	gsl_rng_set (r, seed0);

	seed0 += dim.N_Elements() * seedMul; // avoid repetition in next call
	// if called with undefined global
      }
    
    if( e->KeywordSet(2)) { // LONG

      DLongGDL* res = new DLongGDL(dim, BaseGDL::NOZERO);
      SizeT nEl = res->N_Elements();
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] =
				    (DLong) (gsl_rng_uniform (r) * 2147483646);
      gsl_rng_free (r);
      //      *p0L = new DULongGDL( (DULong) (4294967296.0 * (*res)[0]) );
      return res;
    }

    DSub* pro=dynamic_cast<DSub*>(e->GetPro());
    DDoubleGDL* binomialKey = e->IfDefGetKWAs<DDoubleGDL>( 4);
    DDoubleGDL* poissonKey = e->IfDefGetKWAs<DDoubleGDL>( 5);

    if( e->KeywordSet(0)) { // DOUBLE
      DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);

      random_template< DDoubleGDL, double>( e, res, r, dim, 
					    binomialKey, poissonKey);

      gsl_rng_free (r);
      //      *p0L = new DULongGDL( (DULong) (4294967296.0 * (*res)[0]) );
      return res;
    } else {
      DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);

      random_template< DFloatGDL, float>( e, res, r, dim, 
					  binomialKey, poissonKey);

      gsl_rng_free (r);
      //      *p0L = new DULongGDL( (DULong) (4294967296.0 * (*res)[0]) );
      return res;
    }
  }


  BaseGDL* histogram_fun( EnvT* e)
  {
    double a;
    double b;
    double bsize=1;
    DULong nh, nri;

    SizeT nParam=e->NParam();

    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();

    if( p0->Type() == COMPLEX || p0->Type() == COMPLEXDBL)
      throw GDLException( e->CallingNode(), 
			  "HISTOGRAM: Complex expression not allowed in this context: "
			  +e->GetParString(0));

    DDoubleGDL* binsize = e->IfDefGetKWAs<DDoubleGDL>( 0);
    DLongGDL* input = e->IfDefGetKWAs<DLongGDL>( 1);
    DDoubleGDL* max = e->IfDefGetKWAs<DDoubleGDL>( 2);
    DDoubleGDL* min = e->IfDefGetKWAs<DDoubleGDL>( 3);
    DLongGDL* nbins = e->IfDefGetKWAs<DLongGDL>( 4);

    if (binsize != NULL && nbins != NULL && max != NULL)
      throw GDLException( e->CallingNode(), 
			  "HISTOGRAM: Conflicting keywords.");

    DDoubleGDL *p0D = static_cast<DDoubleGDL*>
      (p0->Convert2( DOUBLE, BaseGDL::COPY));

    if (min == NULL) {
      if( p0->Type() == BYTE)
	a = 0.0;
      else {
	a = (*p0D)[0];
	for( SizeT i=1; i<nEl; ++i) {
	  if ((*p0D)[i] < a) a = (*p0D)[i];
	}
      }
    } else a = (*min)[0];

    if (max == NULL) {
      if (binsize != NULL && nbins != NULL) {
	b = a + (*binsize)[0] * (*nbins)[0];
      } else {
	b = (*p0D)[0];
	for( SizeT i=1; i<nEl; ++i) {
	  if ((*p0D)[i] > b) b = (*p0D)[i];
	}
      }
    } else b = (*max)[0];

    a -= 1e-15;
    b += 1e-15;

    nh = (DULong) ((b - a) / bsize + 1);
    if (binsize != NULL) nh = (DULong) ((b - a) / (*binsize)[0] + 1);
    if (nbins != NULL) nh = (*nbins)[0];

    gsl_histogram * h = gsl_histogram_alloc (nh);
    gsl_histogram_set_ranges_uniform (h, a, b);

    for( SizeT i=0; i<nEl; ++i) {
      gsl_histogram_increment (h, (*p0D)[i]);
    }

    //    dimension dim(&nh, (SizeT) 1);
    dimension dim( nh); // ambiguous on OS X:  (&n, (SizeT) 1);
    DLongGDL* res = new DLongGDL(dim, BaseGDL::NOZERO);

    for( SizeT i=0; i<nh; ++i) {
      (*res)[i] = (DLong) gsl_histogram_get (h, i);
    }


    // OMAX
    if( e->KeywordPresent( 5)) {
      BaseGDL** omaxKW = &e->GetKW( 5);
      delete (*omaxKW); 
      *omaxKW = new DDoubleGDL( gsl_histogram_max(h));
    }


    // OMIN
    if( e->KeywordPresent( 6)) {
      BaseGDL** ominKW = &e->GetKW( 6);
      delete (*ominKW); 
      *ominKW = new DDoubleGDL( gsl_histogram_min(h));
    }


    // REVERSE_INDICES
    if( e->KeywordPresent( 7)) {
      BaseGDL** revindKW = &e->GetKW( 7);
      delete (*revindKW);
      nri = nh + nEl + 1;
      dimension dim( nri);
      //      dimension dim(&nri, (SizeT) 1); 
      *revindKW = new DLongGDL( dim, BaseGDL::NOZERO);

      DULong bin;
      DULong k=0;
      for( SizeT i=0; i<nh; ++i) {
	for( SizeT j=0; j<nEl; ++j) {
	  gsl_histogram_find (h, (*p0D)[j], (size_t*) &bin);
	  if (bin == i) {
	    (*(DLongGDL*) *revindKW)[nh+1+k] = j;
	    k++;
	  }
	}
      }
      (*(DLongGDL*) *revindKW)[0] = nh + 1;
      k = 0;
      for( SizeT i=1; i<=nh; ++i) {
	k += (*res)[i-1];
	(*(DLongGDL*) *revindKW)[i] = k+ nh + 1;
      }
    }
    gsl_histogram_free (h);

    return(res);
  }


  void interpolate_linear(SizeT nxa, SizeT nx, const double ya[], 
			  double x[], double y[])
  {
    gsl_interp_accel *acc 
      = gsl_interp_accel_alloc ();

    gsl_interp *interp = gsl_interp_alloc (gsl_interp_linear, nxa);

    double *xa = new double[nxa];
    for( SizeT i=0; i<nxa; ++i) xa[i] = (double) i;
    
    gsl_interp_init (interp, xa, ya, nxa);

    for( SizeT i=0; i<nx; ++i) {
	  y[i] = gsl_interp_eval (interp, xa, ya, x[i], acc);
    }

    gsl_interp_free (interp);
    gsl_interp_accel_free (acc);
    delete xa;
  }


  void interpolate_cubic(SizeT nxa, SizeT nx, const double ya[], 
			 double x[], double y[])
  {
    gsl_interp_accel *acc 
      = gsl_interp_accel_alloc ();

    gsl_spline *spline = gsl_spline_alloc (gsl_interp_cspline, nxa);

    double *xa = new double[nxa];
    for( SizeT i=0; i<nxa; ++i) xa[i] = (double) i;
    
    gsl_spline_init (spline, xa, ya, nxa);

    for( SizeT i=0; i<nx; ++i) {
	  y[i] = gsl_spline_eval (spline, x[i], acc);
    }

    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
    delete xa;
  }



  BaseGDL* interpolate_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam < 2)
      throw GDLException( e->CallingNode(), 
			  "INTERPOLATE: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);
    BaseGDL* p1 = e->GetParDefined( 1);
    BaseGDL* p2;
    BaseGDL* p3;
    if ( nParam == 3) p2 = e->GetParDefined( 2);
    if ( nParam == 4) p3 = e->GetParDefined( 3);

    DDoubleGDL* p0D;
    DDoubleGDL* p1D;
    DDoubleGDL* p2D;
    DDoubleGDL* p3D;

    if( p0->Rank() < nParam-1)
      throw GDLException( e->CallingNode(), 
			  "INTERPOLATE: Number of parameters must agree with dimensions of argument.");

    bool cubic = false;
    if ( e->KeywordSet(0)) cubic = true;

    bool grid = false;
    if ( e->KeywordSet(1)) grid = true;


    // If not GRID then check that rank and dims match
    if ( nParam == 3 && !grid) {
      if (p1->Rank() != p2->Rank())
	   throw GDLException( e->CallingNode(), 
			       "INTERPOLATE: Coordinate arrays must have same length if Grid not set.");
      else {
	for( SizeT i=0; i<p1->Rank(); ++i) {
	  if (p1->Dim(i) != p2->Dim(i))
	    throw GDLException( e->CallingNode(), 
				"INTERPOLATE: Coordinate arrays must have same length if Grid not set.");
	}
      }
    }

    if (p0->Type() == DOUBLE)
	p0D = static_cast<DDoubleGDL*> ( p0);
    else
	p0D = static_cast<DDoubleGDL*>
	  (p0->Convert2( DOUBLE, BaseGDL::COPY));


    // Determine dimensions of output
    DDoubleGDL* res;
    DLong dims[8]={0,0,0,0,0,0,0,0};
    SizeT resRank;
    // Linear Interpolation or No GRID
    if ( nParam == 2 || !grid) {
      for( SizeT i=0; i<p0->Rank()-(nParam-1); ++i) 
	dims[i] = p0->Dim(i);
      for( SizeT i=0; i<p1->Rank(); ++i) 
	dims[i+p0->Rank()-(nParam-1)] = p1->Dim(i);
      resRank = p0->Rank()-(nParam-1)+p1->Rank();
    } else {
      // GRID
      for( SizeT i=0; i<p0->Rank()-(nParam-1); ++i) 
	dims[i] = p0->Dim(i);

      dims[p0->Rank()-(nParam-1)] = p1->Dim(0);
      for( SizeT i=1; i<p1->Rank(); ++i) 
	dims[p0->Rank()-(nParam-1)] *= p1->Dim(i);

      dims[p0->Rank()-(nParam-1)+1] = p2->Dim(0);
      for( SizeT i=1; i<p2->Rank(); ++i) 
	dims[p0->Rank()-(nParam-1)+1] *= p2->Dim(i);

      resRank = p0->Rank()-(nParam-2)+1;
    }
    dimension dim((SizeT *) dims, resRank);
    res = new DDoubleGDL(dim, BaseGDL::NOZERO);

    // Determine number of interpolations
    SizeT ninterp = 1;
    for( SizeT i=0; i<p0->Rank()-(nParam-1); ++i) ninterp *= p0->Dim(i);


    // 1D Interpolation
    if( nParam == 2) {

      if ( p1->Type() == DOUBLE) 
	p1D = static_cast<DDoubleGDL*> ( p1);
      else
	p1D = static_cast<DDoubleGDL*>
	  (p1->Convert2( DOUBLE, BaseGDL::COPY));

      SizeT nxa = p0->Dim(p0->Rank()-1);

      // Single Interpolation
      if (ninterp == 1) {
	if( cubic)
	  // cubic interpolation
	  interpolate_cubic(nxa, p1D->N_Elements(), 
	  		    &(*p0D)[0], &(*p1D)[0], &(*res)[0]);
	else
	  // linear interpolation
	  interpolate_linear(nxa, p1D->N_Elements(), 
			     &(*p0D)[0], &(*p1D)[0], &(*res)[0]);
      } else {
	// Multiple Interpolation
	for( SizeT i=0; i<ninterp; ++i) {
	  double *ya = new double[nxa];
	  for( SizeT j=0; j<nxa; ++j) ya[j] = (*p0D)[j*ninterp+i];
	  double *y = new double[p1D->N_Elements()];
	  if( cubic)
	    // cubic interpolation
	    interpolate_cubic(nxa, p1D->N_Elements(), 
			      ya, &(*p1D)[0], y);
	  else
	    // linear interpolation
	    interpolate_linear(nxa, p1D->N_Elements(), 
			       ya, &(*p1D)[0], y);
	  for( SizeT j=0; j<p1D->N_Elements(); ++j) 
	    (*res)[j*ninterp+i] = y[j];

	  delete (ya);
	  delete (y);
	}
      }

    }


    // 2D Interpolation
    if( nParam == 3) {

      if( cubic)
	throw GDLException( e->CallingNode(), 
			    "INTERPOLATE: Bicubic interpolation not yet supported.");

      if ( p1->Type() == DOUBLE) 
	p1D = static_cast<DDoubleGDL*> ( p1);
      else
	p1D = static_cast<DDoubleGDL*>
	  (p1->Convert2( DOUBLE, BaseGDL::COPY));

      if ( p2->Type() == DOUBLE) 
	p2D = static_cast<DDoubleGDL*> ( p2);
      else
	p2D = static_cast<DDoubleGDL*>
	  (p2->Convert2( DOUBLE, BaseGDL::COPY));

      SizeT nxa = p0->Dim(p0->Rank()-2);
      SizeT nya = p0->Dim(p0->Rank()-1);
      

      double **ya = new double*[nya];
      for( SizeT k=0; k<nya; ++k) ya[k] = new double[nxa];


      SizeT nx = 1;
      if (grid) nx = res->Dim(resRank-2);
      
      SizeT ny = res->Dim(resRank-1);

      double **work = new double*[ny];
      for( SizeT k=0; k<ny; ++k) work[k] = new double[nx];


      for( SizeT i=0; i<ninterp; ++i) {

	for( SizeT k=0; k<nya; ++k) {
	  for( SizeT j=0; j<nxa; ++j) {
	    ya[k][j] = (*p0D)[i+(ninterp*j)+(ninterp*nxa)*k];
	    //  cout << k << "  " << j << "  " << ya[k][j] << endl;
	  }
	}

	bool first = true;
	DLong lastrow;
	double *dptr;
	for( SizeT k=0; k<ny; ++k) {
	  //	    printf("k: %d\n", k);
	  // Interpolate along rows
	  DLong row = (DLong) floor((*p2D)[k]);
	  if (grid) dptr = &(*p1D)[0]; else dptr = &(*p1D)[k]; 

	  if (first || (row != lastrow) || !grid) {
	    if (row < 0) row = 0;
	    if (row >= nya) row = nya - 1;
	    interpolate_linear(nxa, nx, ya[row], dptr, &work[0][0]);
	    if (row < -1) row = -1;
	    if (row >= nya-1) row = nya - 2;
	    interpolate_linear(nxa, nx, ya[(row+1)], dptr, &work[1][0]);
	  }
	  first = false;
	  lastrow = row;

	  if (grid) {
	    // Interpolate between rows
	    for( SizeT j=0; j<nx; ++j) {
	      (*res)[i+ninterp*j+(ninterp*nx)*k] = 
		work[0][j] + (work[1][j]-work[0][j]) * ((*p2D)[k] - row); 
	    } // column (j) loop
	  } else {
	    (*res)[i+ninterp*k] = 
	      work[0][0] + (work[1][0]-work[0][0]) * ((*p2D)[k] - row); 
	  }

	} // row (k) loop
      } // interp loop 


      // Free dynamic arrays
      for( SizeT k=0; k<ny; ++k) delete(work[k]);
      delete(work);

      for( SizeT k=0; k<nya; ++k) delete(ya[k]);
      delete(ya);

    } // if( nParam == 3) {

    if (p0->Type() == DOUBLE) {
      return res;	
    } else if (p0->Type() == FLOAT) {
      DFloatGDL* res1 = static_cast<DFloatGDL*>
	(res->Convert2( FLOAT, BaseGDL::COPY));
      return res1;
    } else if (p0->Type() == INT) {
      DIntGDL* res1 = static_cast<DIntGDL*>
	(res->Convert2( INT, BaseGDL::COPY));
      return res1;
    } else if (p0->Type() == UINT) {
      DUIntGDL* res1 = static_cast<DUIntGDL*>
	(res->Convert2( UINT, BaseGDL::COPY));
      return res1;
    } else if (p0->Type() == LONG) {
      DLongGDL* res1 = static_cast<DLongGDL*>
	(res->Convert2( LONG, BaseGDL::COPY));
      return res1;
    } else if (p0->Type() == ULONG) {
      DULongGDL* res1 = static_cast<DULongGDL*>
	(res->Convert2( ULONG, BaseGDL::COPY));
      return res1;
    } else if (p0->Type() == LONG64) {
      DLong64GDL* res1 = static_cast<DLong64GDL*>
	(res->Convert2( LONG64, BaseGDL::COPY));
      return res1;
    } else if (p0->Type() == ULONG64) {
      DULong64GDL* res1 = static_cast<DULong64GDL*>
	(res->Convert2( ULONG64, BaseGDL::COPY));
      return res1;
    }  else if (p0->Type() == BYTE) {
      DByteGDL* res1 = static_cast<DByteGDL*>
	(res->Convert2( BYTE, BaseGDL::COPY));
      return res1;
    } else {
      return res;
    }
    
  }

} // namespace


