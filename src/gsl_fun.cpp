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
			  "INVERT: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DComplexGDL* res = new DComplexGDL( p0C->Dim(), BaseGDL::NOZERO);

	float f32_2[2];
	double f64_2[2];

	gsl_matrix_complex *mat = gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(1));
	gsl_matrix_complex *inverse = gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
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

	gsl_matrix_complex *mat = gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(1));
	gsl_matrix_complex *inverse = gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
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


  template< typename T>
  int radix2_template( BaseGDL* p0, DComplexDblGDL* res)
  {
    T* p0c = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();
    double f64;

    for( SizeT i=0; i<nEl; ++i) {
      if (p0->Type() == DOUBLE) f64 = (*p0c)[i]; else f64 = (double) (*p0c)[i];
      memcpy(&(*res)[i], &f64, szdbl);

      f64 = 0;
      memcpy(((double*) &(*res)[i])+1, &f64, szdbl);
    }
    return 0;
  }

  template< typename T>
  int radix2_float_template( BaseGDL* p0, DComplexGDL* res)
  {
    T* p0c = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();
    float f32;

    for( SizeT i=0; i<nEl; ++i) {
      if (p0->Type() == FLOAT) f32 = (*p0c)[i]; else f32 = (float) (*p0c)[i];
      memcpy(&(*res)[i], &f32, szflt);

      f32 = 0;
      memcpy(((double*) &(*res)[i])+1, &f32, szflt);
    }
    return 0;
  }


  template< typename T>
  int mxradix_template( BaseGDL* p0, DComplexDblGDL* res)
  {
    T* p0c = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();
    double f64;

    if (p0->Type() == DOUBLE) {
      memcpy(&(*res)[0], &(*p0c)[0], nEl*szdbl);
    } else {
      for( SizeT i=0; i<nEl; ++i) {
	f64 = (double) (*p0c)[i];
	memcpy(((double*) &(*res)[i/2])+(i%2), &f64, szdbl);
      }
    }
    return 0;
  }

  template< typename T>
  int mxradix_float_template( BaseGDL* p0, DComplexGDL* res)
  {
    T* p0c = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();
    float f32;

    if (p0->Type() == FLOAT) {
      memcpy(&(*res)[0], &(*p0c)[0], nEl*szflt);
    } else {
      for( SizeT i=0; i<nEl; ++i) {
	f32 = (float) (*p0c)[i];
	memcpy(((float*) &(*res)[i/2])+(i%2), &f32, szflt);
      }
    }
    return 0;
  }


  BaseGDL* fft_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    int s, dbl=0;
    int overwrite=0;
    float f32[2];
    double f64[2];
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
			    "FFT: Expression must be a scalar or 1 element array in this context: "
			    +e->GetParString(1));

      DDoubleGDL* direction = 
	static_cast<DDoubleGDL*>(p1->Convert2( DOUBLE, BaseGDL::COPY));
      direct = GSL_SIGN((*direction)[0]);
    }
    if( e->KeywordSet(0)) dbl = 1;
    if( e->KeywordSet(1)) direct = +1.0;
    if( e->KeywordSet(2)) overwrite = 1;

    SizeT radix2 = 0;
    for( SizeT i=0; i<32; ++i) {
      if (nEl == (2 << i)) {
	radix2 = 1;
	break;
      }
    }

    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);

	if (radix2 && overwrite == 0) {

	  memcpy(&(*res)[0], &(*p0C)[0], szflt*2*nEl);

	  if (direct == -1) {
	    gsl_fft_complex_float_radix2_forward ((float*) &(*res)[0], 1, nEl); 
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    gsl_fft_complex_float_radix2_backward ((float*) &(*res)[0], 1, nEl);
	  }

	}
	else if (!radix2 && overwrite == 0) {

	  memcpy(&(*res)[0], &(*p0C)[0], szflt*2*nEl);

	  gsl_fft_complex_wavetable_float * wavetable;
	  gsl_fft_complex_workspace_float * workspace;

	  wavetable = gsl_fft_complex_wavetable_float_alloc (nEl);
	  workspace = gsl_fft_complex_workspace_float_alloc (nEl);

	  if (direct == -1) {
	    gsl_fft_complex_float_forward ((float*) &(*res)[0], 1, nEl, 
					   wavetable, workspace);
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    gsl_fft_complex_float_backward ((float*) &(*res)[0], 1, nEl, 
					    wavetable, workspace);
	  }
	  gsl_fft_complex_wavetable_float_free (wavetable);
	  gsl_fft_complex_workspace_float_free (workspace);
	}
	return res;
      }

    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);

	if (radix2 && overwrite == 0) {

	  memcpy(&(*res)[0], &(*p0C)[0], szdbl*2*nEl);

	  if (direct == -1) {
	    gsl_fft_complex_radix2_forward ((double*) &(*res)[0], 1, nEl); 
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    gsl_fft_complex_radix2_backward ((double*) &(*res)[0], 1, nEl);
	  }
	}
	else if (!radix2 && overwrite == 0) {

	  memcpy(&(*res)[0], &(*p0C)[0], szdbl*2*nEl);

	  gsl_fft_complex_wavetable * wavetable;
	  gsl_fft_complex_workspace * workspace;

	  wavetable = gsl_fft_complex_wavetable_alloc (nEl);
	  workspace = gsl_fft_complex_workspace_alloc (nEl);

	  if (direct == -1) {
	    gsl_fft_complex_forward ((double*) &(*res)[0], 1, nEl, 
					   wavetable, workspace);
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    gsl_fft_complex_backward ((double*) &(*res)[0], 1, nEl, 
				      wavetable, workspace);
	  }
	  gsl_fft_complex_wavetable_free (wavetable);
	  gsl_fft_complex_workspace_free (workspace);
	}
	return res;
      }
    else if( p0->Type() == DOUBLE || dbl) 
      {
	DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);

	if (radix2) {

	  switch ( p0->Type()) {
	  case DOUBLE: 
	    radix2_template< DDoubleGDL>( p0, res);
	    break;
	  case FLOAT: 
	    radix2_template< DFloatGDL>( p0, res);
	    break;
	  case LONG: 
	    radix2_template< DLongGDL>( p0, res);
	    break;
	  case ULONG: 
	    radix2_template< DULongGDL>( p0, res);
	    break;
	  case INT: 
	    radix2_template< DIntGDL>( p0, res);
	    break;
	  case UINT: 
	    radix2_template< DUIntGDL>( p0, res);
	    break;
	  case BYTE: 
	    radix2_template< DByteGDL>( p0, res);
	    break;
	  }

	  if (direct == -1) {
	    gsl_fft_complex_radix2_forward ((double*) &(*res)[0], 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    gsl_fft_complex_radix2_inverse ((double*) &(*res)[0], 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] *= nEl;
	  }
	}
	else if (!radix2) {

	  switch ( p0->Type()) {
	  case DOUBLE: 
	    mxradix_template< DFloatGDL>( p0, res);
	    break;
	  case FLOAT: 
	    mxradix_template< DFloatGDL>( p0, res);
	    break;
	  case LONG:
	    mxradix_template< DLongGDL>( p0, res);
	    break;
	  case ULONG: 
	    mxradix_template< DULongGDL>( p0, res);
	    break;
	  case INT: 
	    mxradix_template< DIntGDL>( p0, res);
	    break;
	  case UINT: 
	    mxradix_template< DUIntGDL>( p0, res);
	    break;
	  case BYTE: 
	    mxradix_template< DByteGDL>( p0, res);
	    break;
	  }

	  gsl_fft_real_wavetable *real;
	  gsl_fft_real_workspace *work;

	  work = gsl_fft_real_workspace_alloc(nEl);
	  real = gsl_fft_real_wavetable_alloc(nEl);

	  gsl_fft_real_transform((double*) &(*res)[0], 1, nEl, real, work);

	  if (direct == -1) {
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    for( SizeT i=1; i<nEl/2; ++i) {
	      memcpy(&f64[0], &(*res)[i], szdbl);
	      f64[0] = -f64[0];
	      memcpy(&(*res)[i], &f64[0], szdbl);
	    }

	    if ((nEl % 2) != 0) {
	      memcpy(&f64[0], &(*res)[(nEl-1)/2], szdbl);
	      f64[0] = -f64[0];
	      memcpy(&(*res)[(nEl-1)/2], &f64[0], szdbl);
	    }
	  }

	  unpack_real_mxradix((double*) &(*res)[0], nEl);

	  gsl_fft_real_wavetable_free(real);
	  gsl_fft_real_workspace_free(work);
	}

	return res;
      }
    else if( p0->Type() == FLOAT ||
	     p0->Type() == LONG ||
	     p0->Type() == ULONG ||
	     p0->Type() == INT ||
	     p0->Type() == UINT ||
	     p0->Type() == BYTE)
      {

	DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);

	if (radix2) {

	  switch ( p0->Type()) {
	  case FLOAT: 
	    radix2_float_template< DFloatGDL>( p0, res);
	    break;
	  case LONG: 
	    radix2_float_template< DLongGDL>( p0, res);
	    break;
	  case ULONG: 
	    radix2_float_template< DULongGDL>( p0, res);
	    break;
	  case INT: 
	    radix2_float_template< DIntGDL>( p0, res);
	    break;
	  case UINT: 
	    radix2_float_template< DUIntGDL>( p0, res);
	    break;
	  case BYTE: 
	    radix2_float_template< DByteGDL>( p0, res);
	    break;
	  }

	  if (direct == -1) {
	    gsl_fft_complex_float_radix2_forward ((float*) &(*res)[0], 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    gsl_fft_complex_float_radix2_inverse ((float*) &(*res)[0], 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] *= nEl;
	  }
	}
	else if (!radix2) {

	  switch ( p0->Type()) {
	  case FLOAT: 
	    mxradix_float_template< DFloatGDL>( p0, res);
	    break;
	  case LONG:
	    mxradix_float_template< DLongGDL>( p0, res);
	    break;
	  case ULONG: 
	    mxradix_float_template< DULongGDL>( p0, res);
	    break;
	  case INT: 
	    mxradix_float_template< DIntGDL>( p0, res);
	    break;
	  case UINT: 
	    mxradix_float_template< DUIntGDL>( p0, res);
	    break;
	  case BYTE: 
	    mxradix_float_template< DByteGDL>( p0, res);
	    break;
	  }

	  gsl_fft_real_wavetable_float *real;
	  gsl_fft_real_workspace_float *work;

	  work = gsl_fft_real_workspace_float_alloc(nEl);
	  real = gsl_fft_real_wavetable_float_alloc(nEl);

	  gsl_fft_real_float_transform((float*) &(*res)[0], 1, nEl, real, work);

	  if (direct == -1) {
	    for( SizeT i=0; i<nEl; ++i) (*res)[i] /= nEl;
	  } else if (direct == +1) {
	    for( SizeT i=1; i<nEl/2; ++i) {
	      memcpy(&f32[0], &(*res)[i], szflt);
	      f32[0] = -f32[0];
	      memcpy(&(*res)[i], &f32[0], szflt);
	    }

	    if ((nEl % 2) != 0) {
	      memcpy(&f32[0], &(*res)[(nEl-1)/2], szflt);
	      f32[0] = -f32[0];
	      memcpy(&(*res)[(nEl-1)/2], &f32[0], szflt);
	    }
	  }

	  unpack_real_mxradix_float((float*) &(*res)[0], nEl);

	  gsl_fft_real_wavetable_float_free(real);
	  gsl_fft_real_workspace_float_free(work);
	}

	return res;
      }
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));

	return res;
      }
  }

  BaseGDL* randomu_fun( EnvT* e)
  {

    DULongGDL* seed;
    BaseGDL** p0L = &e->GetPar( 0);
    if( *p0L != NULL) {
      seed = static_cast<DULongGDL*>((*p0L)->Convert2( ULONG, BaseGDL::COPY));
      delete (*p0L); 
    } else {
      time_t t1;
      (void) time(&t1);
      seed = new DULongGDL( (DULong) t1);
    }

    dimension dim;
    arr( e, dim, 1);

    DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
    SizeT nEl = res->N_Elements();

    gsl_rng *r = gsl_rng_alloc (gsl_rng_mt19937);
    gsl_rng_set (r, (*seed)[0]);


    for( SizeT i=0; i<nEl; ++i) {
      (*res)[ i] = (float) gsl_rng_uniform (r);
    }
    gsl_rng_free (r);

    *p0L = new DULongGDL( (DULong) (4294967296.0 * (*res)[0]) );

    return(res);
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


  int unpack_real_radix2(double *dptr, SizeT nEl) {

    double f64[2];

    for( SizeT i=0; i<nEl/4+1; ++i) {
      memcpy(&f64[0], &dptr[2*i], 2*szdbl);
      if (i == 0)
	memcpy(&dptr[2*(nEl-i-1)], &f64[1], szdbl);
      else if (i == nEl/4)
	memcpy(&dptr[nEl], &f64[0], szdbl);
      else {
	memcpy(&dptr[2*(nEl-2*i)],   &f64[0], szdbl);
	memcpy(&dptr[2*(nEl-2*i-1)], &f64[1], szdbl);
      }
    }

    for( SizeT i=nEl/4; i<nEl/2; ++i) {
      memcpy(&f64[0], &dptr[2*i], 2*szdbl);
      f64[0] = -f64[0];
      f64[1] = -f64[1];
      if (i == nEl/4)
	memcpy(&dptr[nEl+3], &f64[1], szdbl);
      else {
	memcpy(&dptr[4*i+1], &f64[0], szdbl);
	memcpy(&dptr[4*i+3], &f64[1], szdbl);
      }
    }
    
    for( SizeT i=nEl/2+1; i<nEl; ++i) {
      memcpy(&f64[0], &dptr[2*i], 2*szdbl);
      memcpy(&dptr[2*(nEl-i)], &f64[0], szdbl);
      
      f64[1] = -f64[1];
      memcpy(&dptr[2*(nEl-i)+1], &f64[1], szdbl);
    }
    
    f64[0] = 0;
    memcpy(&dptr[1], &f64[0], szdbl);
  }


  int unpack_real_mxradix(double *dptr, SizeT nEl) {
  
    double f64[2];

    for( SizeT i=1; i<nEl/2+(nEl%2); ++i) {
      memcpy(&f64[0], &dptr[2*i-1], szdbl);
      memcpy(&f64[1], &dptr[2*i], szdbl);

      f64[1] = -f64[1];
      memcpy(&dptr[2*(nEl-i)], &f64[0], szdbl);
      memcpy(&dptr[2*(nEl-i)+1], &f64[1], szdbl);
    }

    memcpy(&dptr[nEl], &dptr[nEl-1], szdbl);

    for( SizeT i=1; i<nEl/2+(nEl%2); ++i) {
      memcpy(&f64[0], &dptr[2*(nEl-i)], szdbl);
      memcpy(&f64[1], &dptr[2*(nEl-i)+1], szdbl);

      f64[1] = -f64[1];

      memcpy(&dptr[2*i],   &f64[0], szdbl);
      memcpy(&dptr[2*i+1], &f64[1], szdbl);
    }
    f64[1] = 0;
    memcpy(&dptr[1], &f64[1], szdbl);
  }


  int unpack_real_mxradix_float(float *dptr, SizeT nEl) {
  
    float f32[2];

    for( SizeT i=1; i<nEl/2+(nEl%2); ++i) {
      memcpy(&f32[0], &dptr[2*i-1], szflt);
      memcpy(&f32[1], &dptr[2*i], szflt);

      f32[1] = -f32[1];
      memcpy(&dptr[2*(nEl-i)], &f32[0], szflt);
      memcpy(&dptr[2*(nEl-i)+1], &f32[1], szflt);
    }

    memcpy(&dptr[nEl], &dptr[nEl-1], szflt);

    for( SizeT i=1; i<nEl/2+(nEl%2); ++i) {
      memcpy(&f32[0], &dptr[2*(nEl-i)], szflt);
      memcpy(&f32[1], &dptr[2*(nEl-i)+1], szflt);

      f32[1] = -f32[1];

      memcpy(&dptr[2*i],   &f32[0], szflt);
      memcpy(&dptr[2*i+1], &f32[1], szflt);
    }
    f32[1] = 0;
    memcpy(&dptr[1], &f32[1], szflt);
  }


} // namespace

