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
	  memcpy(f32_2, &(*p0C)[i], sizeof(double));
	  f64 = (double) f32_2[0];
	  memcpy(&mat->data[2*i], &f64, sizeof(double));

	  f64 = (double) f32_2[1];
	  memcpy(&mat->data[2*i+1], &f64, sizeof(double));
	}
 
	gsl_linalg_complex_LU_decomp (mat, perm, &s);
       	det = gsl_linalg_complex_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_complex_LU_invert (mat, perm, inverse);
	  if (det * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	for( SizeT i=0; i<nEl; ++i) {
	  memcpy(&f64_2[0], &inverse->data[2*i], sizeof(double)*2);
	  f32_2[0] = (float) f64_2[0];
	  f32_2[1] = (float) f64_2[1];

	  memcpy(&(*res)[i], &f32_2[0], sizeof(float)*2);
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

	memcpy(mat->data, &(*p0C)[0], nEl*sizeof(double)*2);

	gsl_linalg_complex_LU_decomp (mat, perm, &s);
	det = gsl_linalg_complex_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_complex_LU_invert (mat, perm, inverse);
	  if (det * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	memcpy(&(*res)[0], inverse->data, nEl*sizeof(double)*2);

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

	memcpy(mat->data, &(*p0D)[0], nEl*sizeof(double));

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

	memcpy(&(*res)[0], inverse->data, nEl*sizeof(double));

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
	  memcpy(&mat->data[i], &f64, sizeof(double));
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


  BaseGDL* fft_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    int s;
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
    if( e->KeywordSet(1)) direct = +1.0;

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

	if (radix2 && direct == -1) {

	  float *data = new float[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(float)*2*nEl);

	  gsl_fft_complex_float_radix2_forward (data, 1, nEl); 

	  for( SizeT i=0; i<2*nEl; ++i) data[i] /= nEl;

	  memcpy(&(*res)[0], &data[0], sizeof(float)*2*nEl);
	  delete(data);
	}
	else if (radix2 && direct == +1) {

	  float *data = new float[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(float)*2*nEl);

	  gsl_fft_complex_float_radix2_backward (data, 1, nEl); 

	  memcpy(&(*res)[0], &data[0], sizeof(float)*2*nEl);
	  delete(data);
	}

	else if (!radix2 && direct == -1) {

	  float *data = new float[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(float)*2*nEl);

	  gsl_fft_complex_wavetable_float * wavetable;
	  gsl_fft_complex_workspace_float * workspace;

	  wavetable = gsl_fft_complex_wavetable_float_alloc (nEl);
	  workspace = gsl_fft_complex_workspace_float_alloc (nEl);

	  gsl_fft_complex_float_forward (data, 1, nEl, 
					 wavetable, workspace);

	  gsl_fft_complex_wavetable_float_free (wavetable);
	  gsl_fft_complex_workspace_float_free (workspace);

	  for( SizeT i=0; i<2*nEl; ++i) data[i] /= nEl;

	  memcpy(&(*res)[0], &data[0], sizeof(float)*2*nEl);
	  delete(data);
	}
	else if (!radix2 && direct == +1) {

	  float *data = new float[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(float)*2*nEl);

	  gsl_fft_complex_wavetable_float * wavetable;
	  gsl_fft_complex_workspace_float * workspace;

	  wavetable = gsl_fft_complex_wavetable_float_alloc (nEl);
	  workspace = gsl_fft_complex_workspace_float_alloc (nEl);

	  gsl_fft_complex_float_backward (data, 1, nEl, 
					  wavetable, workspace);

	  gsl_fft_complex_wavetable_float_free (wavetable);
	  gsl_fft_complex_workspace_float_free (workspace);

	  memcpy(&(*res)[0], &data[0], sizeof(float)*2*nEl);
	  delete(data);
	}
	return res;
      }

    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);

	if (radix2 && direct == -1) {

	  double *data = new double[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(double)*2*nEl);

	  gsl_fft_complex_radix2_forward (data, 1, nEl); 

	  for( SizeT i=0; i<2*nEl; ++i) data[i] /= nEl;

	  memcpy(&(*res)[0], &data[0], sizeof(double)*2*nEl);
	  delete(data);
	}
	else if (radix2 && direct == +1) {

	  double *data = new double[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(double)*2*nEl);

	  gsl_fft_complex_radix2_backward (data, 1, nEl); 

	  memcpy(&(*res)[0], &data[0], sizeof(double)*2*nEl);
	  delete(data);
	}

	else if (!radix2 && direct == -1) {

	  double *data = new double[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(double)*2*nEl);

	  gsl_fft_complex_wavetable * wavetable;
	  gsl_fft_complex_workspace * workspace;

	  wavetable = gsl_fft_complex_wavetable_alloc (nEl);
	  workspace = gsl_fft_complex_workspace_alloc (nEl);

	  gsl_fft_complex_forward (data, 1, nEl, 
				   wavetable, workspace);

	  gsl_fft_complex_wavetable_free (wavetable);
	  gsl_fft_complex_workspace_free (workspace);

	  for( SizeT i=0; i<2*nEl; ++i) data[i] /= nEl;

	  memcpy(&(*res)[0], &data[0], sizeof(double)*2*nEl);
	  delete(data);
	}
	else if (!radix2 && direct == +1) {

	  double *data = new double[2*nEl];

	  memcpy(&data[0], &(*p0C)[0], sizeof(double)*2*nEl);

	  gsl_fft_complex_wavetable * wavetable;
	  gsl_fft_complex_workspace * workspace;

	  wavetable = gsl_fft_complex_wavetable_alloc (nEl);
	  workspace = gsl_fft_complex_workspace_alloc (nEl);

	  gsl_fft_complex_backward (data, 1, nEl, 
					  wavetable, workspace);

	  gsl_fft_complex_wavetable_free (wavetable);
	  gsl_fft_complex_workspace_free (workspace);

	  memcpy(&(*res)[0], &data[0], sizeof(double)*2*nEl);
	  delete(data);
	}
	return res;
      }
    else if( p0->Type() == DOUBLE)
      {
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);

	if (radix2 && direct == -1) {

	  double *data = new double[nEl];
	  memcpy(data, &(*p0D)[0], nEl*sizeof(double));

	  gsl_fft_real_radix2_transform (data, 1, nEl);
	  for( SizeT i=0; i<nEl; ++i) data[i] /= nEl;

	  memcpy(&(*res)[0], &data[0], sizeof(double));
	  memcpy(&(*res)[nEl/2], &data[nEl/2], sizeof(double));
	  for( SizeT i=1; i<nEl/2; ++i) {
	    memcpy(&f64[0], &data[i], sizeof(double));
	    memcpy(&f64[1], &data[nEl-i], sizeof(double));
	    memcpy(&(*res)[i], f64, 2*sizeof(double));

	    memcpy(&f64[0], &data[i], sizeof(double));
	    memcpy(&f64[1], &data[nEl-i], sizeof(double));
	    f64[1] = -f64[1];
	    memcpy(&(*res)[nEl-i], f64, 2*sizeof(double));
	  }
	  delete(data);
	}
	else if (radix2 && direct == +1) {

	  double *data = new double[2*nEl];
	  for( SizeT i=0; i<nEl; ++i) data[2*i] = (*p0D)[i];
	  for( SizeT i=0; i<nEl; ++i) data[2*i+1] = 0;

	  gsl_fft_complex_radix2_inverse (data, 1, nEl);
	  for( SizeT i=0; i<2*nEl; ++i) data[i] *= nEl;

	  memcpy(&(*res)[0], &data[0], 2*nEl*sizeof(double));

	  delete(data);
	}

	else if (!radix2 && direct == -1) {

	  gsl_fft_real_wavetable *real;
	  gsl_fft_real_workspace *work;

	  double *data = new double[nEl];
	  memcpy(data, &(*p0D)[0], nEl*sizeof(double));

	  work = gsl_fft_real_workspace_alloc(nEl);
	  real = gsl_fft_real_wavetable_alloc(nEl);

	  gsl_fft_real_transform(data, 1, nEl, real, work);

	  for( SizeT i=0; i<nEl; ++i) data[i] /= nEl;

	  gsl_fft_halfcomplex_unpack(data, (double*) &(*res)[0], 1, nEl);

	  gsl_fft_real_wavetable_free(real);
	  gsl_fft_real_workspace_free(work);

	  delete(data);
	}
	else if (!radix2 && direct == +1) {

	  gsl_fft_real_wavetable *real;
	  gsl_fft_real_workspace *work;

	  double *data = new double[nEl];
	  memcpy(data, &(*p0D)[0], nEl*sizeof(double));

	  work = gsl_fft_real_workspace_alloc(nEl);
	  real = gsl_fft_real_wavetable_alloc(nEl);

	  gsl_fft_real_transform(data, 1, nEl, real, work);

	  if ((nEl % 2) == 0)
	    for( SizeT i=1; i<nEl/2; ++i) data[2*i] *= -1;

	  if ((nEl % 2) != 0) {
	    for( SizeT i=1; i<nEl/2; ++i) data[2*i] *= -1;
	    data[nEl-1] *= -1;
	  }

	  gsl_fft_halfcomplex_unpack(data, (double*) &(*res)[0], 1, nEl);

	  gsl_fft_real_wavetable_free(real);
	  gsl_fft_real_workspace_free(work);

	  delete (data);
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
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DLongGDL* p0L = static_cast<DLongGDL*>( p0);
	DULongGDL* p0UL = static_cast<DULongGDL*>( p0);
	DIntGDL* p0I = static_cast<DIntGDL*>( p0);
	DUIntGDL* p0UI = static_cast<DUIntGDL*>( p0);
	DByteGDL* p0B = static_cast<DByteGDL*>( p0);

	if (radix2 && direct == -1) {

	  double *data = new double[nEl];

	  for( SizeT i=0; i<nEl; ++i) {
	    switch ( p0->Type()) {
	    case FLOAT: f64[0] = (double) (*p0F)[i]; break;
	    case LONG:  f64[0] = (double) (*p0L)[i]; break;
	    case ULONG: f64[0] = (double) (*p0UL)[i]; break;
	    case INT:   f64[0] = (double) (*p0I)[i]; break;
	    case UINT:  f64[0] = (double) (*p0UI)[i]; break;
	    case BYTE:  f64[0] = (double) (*p0B)[i]; break;
	    }
	    memcpy(&data[i], &f64[0], sizeof(double));
	  }

	  if( e->KeywordSet(0)) {

	    DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);

	    gsl_fft_real_radix2_transform (data, 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) data[i] /= nEl;

	    memcpy(&(*res)[0], &data[0], sizeof(double));
	    memcpy(&(*res)[nEl/2], &data[nEl/2], sizeof(double));
	    for( SizeT i=1; i<nEl/2; ++i) {
	      memcpy(&f64[0], &data[i], sizeof(double));
	      memcpy(&f64[1], &data[nEl-i], sizeof(double));
	      memcpy(&(*res)[i], f64, 2*sizeof(double));
	      
	      memcpy(&f64[0], &data[i], sizeof(double));
	      memcpy(&f64[1], &data[nEl-i], sizeof(double));
	      f64[1] = -f64[1];
	      memcpy(&(*res)[nEl-i], f64, 2*sizeof(double));
	    }

	    return res;

	  } else {

	    DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);

	    gsl_fft_real_radix2_transform (data, 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) data[i] /= nEl;

	    f32[0] = (float) data[0];
	    memcpy(&(*res)[0], &f32[0], sizeof(float));
	    f32[0] = (float) data[nEl/2];
	    memcpy(&(*res)[nEl/2], &f32[0], sizeof(float));
	    for( SizeT i=1; i<nEl/2; ++i) {
	      f32[0] = data[i];
	      f32[1] = data[nEl-i];
	      memcpy(&(*res)[i], f32, 2*sizeof(float));
	      
	      f32[0] = data[i];
	      f32[1] = -data[nEl-i];
	      memcpy(&(*res)[nEl-i], f32, 2*sizeof(float));
	    }
	    return res;
	  }
	  delete(data);
	}
	else if (radix2 && direct == +1) {

	  double *data = new double[2*nEl];
	  for( SizeT i=0; i<nEl; ++i) {
	    switch ( p0->Type()) {
	    case FLOAT: f64[0] = (double) (*p0F)[i]; break;
	    case LONG:  f64[0] = (double) (*p0L)[i]; break;
	    case ULONG: f64[0] = (double) (*p0UL)[i]; break;
	    case INT:   f64[0] = (double) (*p0I)[i]; break;
	    case UINT:  f64[0] = (double) (*p0UI)[i]; break;
	    case BYTE:  f64[0] = (double) (*p0B)[i]; break;
	    }
	    memcpy(&data[2*i], &f64[0], sizeof(double));
	    memset(&data[2*i+1], 0, sizeof(double));
	  }

	  if( e->KeywordSet(0)) {

	    DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);

	    gsl_fft_complex_radix2_inverse (data, 1, nEl);
	    for( SizeT i=0; i<2*nEl; ++i) data[i] *= nEl;

	    memcpy(&(*res)[0], &data[0], 2*nEl*sizeof(double));
	    return res;
	  } else {

	    DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);

	    gsl_fft_complex_radix2_inverse (data, 1, nEl);
	    for( SizeT i=0; i<nEl; ++i) {
	      f32[0] = (float) (data[2*i] * nEl);
	      f32[1] = (float) (data[2*i+1] * nEl);
	      memcpy(&(*res)[i], f32, 2*sizeof(float));
	    }
	    return res;
	  }
	  delete(data);

	}
	else if (!radix2 && direct == -1) {

	  if( e->KeywordSet(0)) {

	    DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);
	    double *data = new double[nEl];

	    for( SizeT i=0; i<nEl; ++i) {
	      switch ( p0->Type()) {
	      case FLOAT: f64[0] = (double) (*p0F)[i]; break;
	      case LONG:  f64[0] = (double) (*p0L)[i]; break;
	      case ULONG: f64[0] = (double) (*p0UL)[i]; break;
	      case INT:   f64[0] = (double) (*p0I)[i]; break;
	      case UINT:  f64[0] = (double) (*p0UI)[i]; break;
	      case BYTE:  f64[0] = (double) (*p0B)[i]; break;
	      }
	      memcpy(&data[i], &f64[0], sizeof(double));
	    }

	    gsl_fft_real_wavetable *real;
	    gsl_fft_real_workspace *work;

	    work = gsl_fft_real_workspace_alloc(nEl);
	    real = gsl_fft_real_wavetable_alloc(nEl);

	    gsl_fft_real_transform(data, 1, nEl, real, work);

	    for( SizeT i=0; i<nEl; ++i) data[i] /= nEl;

	    gsl_fft_halfcomplex_unpack(data, (double*) &(*res)[0], 1, nEl);

	    gsl_fft_real_wavetable_free(real);
	    gsl_fft_real_workspace_free(work);

	    delete(data);

	    return res;

	  } else {

	    DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);
	    float *data = new float[nEl];

	    for( SizeT i=0; i<nEl; ++i) {
	      switch ( p0->Type()) {
	      case FLOAT: f32[0] = (float) (*p0F)[i]; break;
	      case LONG:  f32[0] = (float) (*p0L)[i]; break;
	      case ULONG: f32[0] = (float) (*p0UL)[i]; break;
	      case INT:   f32[0] = (float) (*p0I)[i]; break;
	      case UINT:  f32[0] = (float) (*p0UI)[i]; break;
	      case BYTE:  f32[0] = (float) (*p0B)[i]; break;
	      }
	      memcpy(&data[i], &f32[0], sizeof(float));
	    }

	    gsl_fft_real_wavetable_float *real;
	    gsl_fft_real_workspace_float *work;

	    work = gsl_fft_real_workspace_float_alloc(nEl);
	    real = gsl_fft_real_wavetable_float_alloc(nEl);

	    gsl_fft_real_float_transform(data, 1, nEl, real, work);

	    for( SizeT i=0; i<nEl; ++i) data[i] /= nEl;

	    gsl_fft_halfcomplex_float_unpack(data, (float*) &(*res)[0], 1, nEl);

	    gsl_fft_real_wavetable_float_free(real);
	    gsl_fft_real_workspace_float_free(work);

	    delete(data);

	    return res;
	  }
	}
	else if (!radix2 && direct == +1) {

	  if( e->KeywordSet(0)) {

	    DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);
	    double *data = new double[nEl];

	    for( SizeT i=0; i<nEl; ++i) {
	      switch ( p0->Type()) {
	      case FLOAT: f64[0] = (double) (*p0F)[i]; break;
	      case LONG:  f64[0] = (double) (*p0L)[i]; break;
	      case ULONG: f64[0] = (double) (*p0UL)[i]; break;
	      case INT:   f64[0] = (double) (*p0I)[i]; break;
	      case UINT:  f64[0] = (double) (*p0UI)[i]; break;
	      case BYTE:  f64[0] = (double) (*p0B)[i]; break;
	      }
	      memcpy(&data[i], &f64[0], sizeof(double));
	    }

	    gsl_fft_real_wavetable *real;
	    gsl_fft_real_workspace *work;

	    work = gsl_fft_real_workspace_alloc(nEl);
	    real = gsl_fft_real_wavetable_alloc(nEl);

	    gsl_fft_real_transform(data, 1, nEl, real, work);

	    if ((nEl % 2) == 0)
	      for( SizeT i=1; i<nEl/2; ++i) data[2*i] *= -1;

	    if ((nEl % 2) != 0) {
	      for( SizeT i=1; i<nEl/2; ++i) data[2*i] *= -1;
	      data[nEl-1] *= -1;
	    }

	    gsl_fft_halfcomplex_unpack(data, (double*) &(*res)[0], 1, nEl);
	    
	    gsl_fft_real_wavetable_free(real);
	    gsl_fft_real_workspace_free(work);

	    delete(data);

	    return res;

	  } else {

	    DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);
	    float *data = new float[nEl];

	    for( SizeT i=0; i<nEl; ++i) {
	      switch ( p0->Type()) {
	      case FLOAT: f32[0] = (float) (*p0F)[i]; break;
	      case LONG:  f32[0] = (float) (*p0L)[i]; break;
	      case ULONG: f32[0] = (float) (*p0UL)[i]; break;
	      case INT:   f32[0] = (float) (*p0I)[i]; break;
	      case UINT:  f32[0] = (float) (*p0UI)[i]; break;
	      case BYTE:  f32[0] = (float) (*p0B)[i]; break;
	      }
	      memcpy(&data[i], &f32[0], sizeof(float));
	    }

	    gsl_fft_real_wavetable_float *real;
	    gsl_fft_real_workspace_float *work;

	    work = gsl_fft_real_workspace_float_alloc(nEl);
	    real = gsl_fft_real_wavetable_float_alloc(nEl);

	    gsl_fft_real_float_transform(data, 1, nEl, real, work);

	    if ((nEl % 2) == 0)
	      for( SizeT i=1; i<nEl/2; ++i) data[2*i] *= -1;

	    if ((nEl % 2) != 0) {
	      for( SizeT i=1; i<nEl/2; ++i) data[2*i] *= -1;
	      data[nEl-1] *= -1;
	    }

	    gsl_fft_halfcomplex_float_unpack(data, (float*) &(*res)[0], 1, nEl);
	    
	    gsl_fft_real_wavetable_float_free(real);
	    gsl_fft_real_workspace_float_free(work);

	    delete(data);

	    return res;
	  }
	}
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

} // namespace
