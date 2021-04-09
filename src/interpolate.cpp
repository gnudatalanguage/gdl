/***************************************************************************
         interpolate.cpp  -  all things related to interpolate command
                             -------------------
    begin                : Mar 30 2021
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
 ***************************************************************************/

#include "includefirst.hpp"

#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"



//interpolate
#include <gsl/gsl_errno.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_nan.h>

#include "interp_multid.h"

//#include "gsl_errorhandler.hpp"

#ifdef _MSC_VER
#define isnan _isnan
#define isfinite _finite
#endif


DDoubleGDL* interpolate_1dim(EnvT* e, const gdl_interp1d_type* interp_type, 
			       DDoubleGDL* array, DDoubleGDL* x, bool use_missing,
			       DDouble missing, DDouble gamma)
  {

    SizeT nx = x->N_Elements();

    // Determine number and value of input points along x-axis and y-axis
    if (array->Rank() < 1) e->Throw("Number of parameters must agree with dimensions of argument.");
    SizeT rankLeft = array->Rank()-1;

    //initialize output array with correct dimensions
    DLong dims[MAXRANK]; 
    SizeT i = 0;
    for (; i < rankLeft; ++i) dims[i] = array->Dim(i);
    for (; i < MAXRANK; ++i) dims[i] = 0; 

    SizeT resRank = rankLeft;
    SizeT chunksize;

    for (SizeT i = 0; i < x->Rank(); ++i)
      {
	dims[resRank++] = x->Dim(i); 
	if (resRank>MAXRANK) 
	  e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
      }
    chunksize=nx;

    dimension dim((DLong *)dims, resRank);
    DDoubleGDL *res;
    res = new DDoubleGDL(dim, BaseGDL::NOZERO);

    // Determine number of interpolations for remaining dimensions
    SizeT ninterp = 1;
    for (SizeT i = 0; i < rankLeft; ++i) ninterp *= array->Dim(i);

    //need to PAD the intermediate work array to satisfy the IDL requirement that
    //INTERPOLATE considers location points with values between zero and n,
    //where n is the number of values in the input array, to be valid.
    // Seems however to be the case only for 1D interpolation.
    SizeT nxa = array->Dim(rankLeft)+1;
    double *xa = new double[nxa];
    ArrayGuard<double> xaGuard(xa);
    for (SizeT i = 0; i < nxa; ++i) xa[i] = (double)i;

    // Setup interpolation arrays
    gsl_interp_accel *accx = gsl_interp_accel_alloc();
    GDLGuard<gsl_interp_accel> g1( accx, gsl_interp_accel_free);
    gdl_interp1d* interpolant = gdl_interp1d_alloc(interp_type, nxa);
    GDLGuard<gdl_interp1d> g2( interpolant, gdl_interp1d_free);
    
    // output locations tables:
    double *xval = new double[chunksize];
    ArrayGuard<double> xvalGuard(xval);
    for (SizeT count = 0; count < chunksize; ++count)
      {
        xval[count] = (*x)[count]; 
      }
    //construct 1d intermediate array, subset of array with stride ninterp
    double *temp = new double[nxa];
    ArrayGuard<double> tempGuard(temp);

    // Interpolate iteratively ninterp times:
    // loop could be multihreaded easily
    for (SizeT iterate = 0; iterate < ninterp; ++iterate)
      {
	//here we use a padded temp array (1D only):
	for (SizeT k = 0; k < nxa-1; ++k) temp[k]=(*array)[k*ninterp+iterate]; temp[nxa-1]=temp[nxa-2]; //pad!
	gdl_interp1d_init(interpolant, xa, temp, nxa, use_missing?missing_GIVEN:missing_NEAREST, missing, gamma);
#ifndef __PATHCC__
#pragma omp parallel if (chunksize >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= chunksize))
#pragma omp for
#endif
	for (OMPInt i = 0; i < chunksize; ++i)
	  {
	    double x = xval[i];
	    (*res)[i*ninterp+iterate] = gdl_interp1d_eval(interpolant, xa, temp, x, accx);
	  }
      }

    //     gsl_interp_accel_free(accx);
    //     gdl_interp1d_free(interpolant);
    return res;
  }


  DDoubleGDL* interpolate_2dim(EnvT* e, const gdl_interp2d_type* interp_type, DDoubleGDL* array, DDoubleGDL* x, DDoubleGDL* y, bool grid, bool use_missing, DDouble missing, DDouble gamma)
  {

    SizeT nx = x->N_Elements();
    SizeT ny = y->N_Elements();

    if (nx == 1 && ny == 1)  grid = false;

    // Determine number and value of input points along x-axis and y-axis
    if (array->Rank() < 2) e->Throw("Number of parameters must agree with dimensions of argument.");
    SizeT rankLeft = array->Rank()-2;

    // If not GRID then check that rank and dims match
    if  (!grid)
      {
	if (x->Rank() != y->Rank())
	  e->Throw("Coordinate arrays must have same rank if Grid not set.");
	else
	  {
	    for (SizeT i = 0; i < x->Rank(); ++i)
	      {
		if (x->Dim(i) != y->Dim(i))
		  e->Throw("Coordinate arrays must have same shape if Grid not set.");
	      }
	  }
      }

    //initialize output array with correct dimensions
    DLong dims[MAXRANK]; 
    SizeT i = 0;
    for (; i < rankLeft; ++i) dims[i] = array->Dim(i);
    for (; i < MAXRANK; ++i) dims[i] = 0; 

    SizeT resRank = rankLeft;
    SizeT chunksize;
    if (grid)
      {
	dims[resRank++] = nx;
	if (resRank > MAXRANK - 1)
	  e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
	dims[resRank++] = ny;
	chunksize = nx*ny;
      } else
      {
	for (SizeT i = 0; i < x->Rank(); ++i)
	  {
	    dims[resRank++] = x->Dim(i);
	    if (resRank > MAXRANK)
	      e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
	  }
	chunksize = nx;
      }
    dimension dim((DLong *)dims, resRank);
    DDoubleGDL *res;
    res = new DDoubleGDL(dim, BaseGDL::NOZERO);

    // Determine number of interpolations for remaining dimensions
    SizeT ninterp = 1;
    for (SizeT i = 0; i < rankLeft; ++i) ninterp *= array->Dim(i);

    SizeT nxa = array->Dim(rankLeft);
    double *xa = new double[nxa];
    ArrayGuard<double> xaGuard( xa);
    for (SizeT i = 0; i < nxa; ++i) xa[i] = (double)i;
    SizeT nya = array->Dim(rankLeft+1);
    double *ya = new double[nya];
    ArrayGuard<double> yaGuard( ya);
    for (SizeT i = 0; i < nya; ++i) ya[i] = (double)i;

    // Setup interpolation arrays
    gsl_interp_accel *accx = gsl_interp_accel_alloc();
    GDLGuard<gsl_interp_accel> g1( accx, gsl_interp_accel_free);
    gsl_interp_accel *accy = gsl_interp_accel_alloc();
    GDLGuard<gsl_interp_accel> g2( accy, gsl_interp_accel_free);
    gdl_interp2d* interpolant = gdl_interp2d_alloc(interp_type, nxa, nya);
    GDLGuard<gdl_interp2d> g3( interpolant, gdl_interp2d_free);

    // output locations tables:
    double *xval = new double[chunksize];
    ArrayGuard<double> xvalGuard( xval);
    double *yval = new double[chunksize];
    ArrayGuard<double> yvalGuard( yval);
    if (grid)
      {
	for (SizeT j = 0, count=0; j < ny; j++)
	  {
	    for (SizeT i = 0, count = 0; i < nx; i++)
	      {
		count = INDEX_2D(i, j, nx, ny);
		xval[count] = (*x)[i];
		yval[count] = (*y)[j];
	      }
	  }
      } else
      {
	for (SizeT count = 0; count < chunksize; ++count)
	  {
	    xval[count] = (*x)[count];
	    yval[count] = (*y)[count];
	  }
      }
    //construct 2d intermediate array, subset of array with stride ninterp
    double *temp = new double[nxa*nya];
    ArrayGuard<double> tempGuard( temp);
    // Interpolate iteratively ninterp times:
    // loop could be multihreaded easily
    for (SizeT iterate = 0; iterate < ninterp; ++iterate)
      {

	for (SizeT k = 0; k < nxa * nya; ++k) temp[k] = (*array)[k * ninterp + iterate];
	gdl_interp2d_init(interpolant, xa, ya, temp, nxa, nya, use_missing ? missing_GIVEN : missing_NEAREST, missing, gamma);
#ifndef __PATHCC__
#pragma omp parallel if (chunksize >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= chunksize))
#pragma omp for
#endif
	for (OMPInt i = 0; i < chunksize; ++i)
	  {
	    double x = xval[i];
	    double y = yval[i];
	    (*res)[i * ninterp + iterate] = gdl_interp2d_eval(interpolant, xa, ya, temp, x, y, accx, accy);
	  }
      }

    //     gsl_interp_accel_free(accx);
    //     gsl_interp_accel_free(accy);
    //     gdl_interp2d_free(interpolant);
    return res;
  }


  DDoubleGDL* interpolate_3dim(EnvT* e, const gdl_interp3d_type* interp_type, DDoubleGDL* array, DDoubleGDL* x, DDoubleGDL* y, DDoubleGDL* z, bool grid, bool use_missing, DDouble missing)
  {
    SizeT nx = x->N_Elements();
    SizeT ny = y->N_Elements();
    SizeT nz = z->N_Elements();

    if (nx == 1 && ny == 1 && nz == 1)  grid = false;

    // Determine number and value of input points along x-axis and y-axis
    if (array->Rank() < 3) e->Throw("Number of parameters must agree with dimensions of argument.");
    SizeT rankLeft = array->Rank()-3;

    // If not GRID then check that rank and dims match
    if  (!grid)
      {
	if (x->Rank() != y->Rank() || x->Rank() != z->Rank() )
	  e->Throw("Coordinate arrays must have same rank if Grid not set.");
	else
	  {
	    for (SizeT i = 0; i < x->Rank(); ++i)
	      {
		if (x->Dim(i) != y->Dim(i) || x->Dim(i) != z->Dim(i))
		  e->Throw("Coordinate arrays must have same shape if Grid not set.");
	      }
	  }
      }

    //initialize output array with correct dimensions
    DLong dims[MAXRANK];
    SizeT i = 0;
    for (; i < rankLeft; ++i) dims[i] = array->Dim(i);
    for (; i < MAXRANK; ++i) dims[i] = 0;
    SizeT resRank= rankLeft;
    SizeT chunksize;
    if (grid)
      {
	dims[resRank++] = nx;
	if (resRank>MAXRANK-2) e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
	dims[resRank++] = ny;
	dims[resRank++] = nz;
	chunksize=nx*ny*nz;
      }
    else
      {
	for (SizeT i = 0; i < x->Rank(); ++i)
	  {
	    dims[resRank++] = x->Dim(i); if (resRank>MAXRANK) e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
	  }
	chunksize=nx;
      }
    dimension dim((DLong *)dims, resRank);
    DDoubleGDL *res;
    res = new DDoubleGDL(dim, BaseGDL::NOZERO);

    // Determine number of interpolations for remaining dimensions
    SizeT ninterp = 1;
    for (SizeT i = 0; i < rankLeft; ++i) ninterp *= array->Dim(i);

    SizeT nxa = array->Dim(rankLeft);
    double *xa = new double[nxa];
    ArrayGuard<double> xaGuard( xa);
    for (SizeT i = 0; i < nxa; ++i) xa[i] = (double)i;

    SizeT nya = array->Dim(rankLeft+1);
    double *ya = new double[nya];
    ArrayGuard<double> yaGuard( ya);
    for (SizeT i = 0; i < nya; ++i) ya[i] = (double)i;

    SizeT nza = array->Dim(rankLeft+2);
    double *za = new double[nza];
    ArrayGuard<double> zaGuard( za);
    for (SizeT i = 0; i < nza; ++i) za[i] = (double)i;

    // test if interp_type kernel trace is statisfied by nxa,nya,nza:
    if (nxa<gdl_interp3d_type_min_size(interp_type)||nya<gdl_interp3d_type_min_size(interp_type)||nza<gdl_interp3d_type_min_size(interp_type)) 
      e->Throw("Array(s) dimensions too small for this interpolation type.");
    // Setup interpolation arrays
    gsl_interp_accel *accx = gsl_interp_accel_alloc();
    GDLGuard<gsl_interp_accel> g1( accx, gsl_interp_accel_free);
    gsl_interp_accel *accy = gsl_interp_accel_alloc();
    GDLGuard<gsl_interp_accel> g2( accy, gsl_interp_accel_free);
    gsl_interp_accel *accz = gsl_interp_accel_alloc();
    GDLGuard<gsl_interp_accel> g3( accz, gsl_interp_accel_free);
    gdl_interp3d* interpolant = gdl_interp3d_alloc(interp_type, nxa, nya, nza);
    GDLGuard<gdl_interp3d> g4( interpolant, gdl_interp3d_free);

    // output locations tables:
    double *xval = new double[chunksize];
    ArrayGuard<double> xvalGuard( xval);
    double *yval = new double[chunksize];
    ArrayGuard<double> yvalGuard( yval);
    double *zval = new double[chunksize];
    ArrayGuard<double> zvalGuard( zval);
    if (grid)
      {
	for (SizeT k = 0, count = 0; k < nz; ++k)
	  {
	    for (SizeT j = 0; j < ny; ++j)
	      {
		for (SizeT i = 0; i < nx; ++i)
		  {
		    count = INDEX_3D(i, j, k, nx, ny, nz);
		    xval[count] = (*x)[i];
		    yval[count] = (*y)[j];
		    zval[count] = (*z)[k];
		  }
	      }
	  }
      } else
      {
	for (SizeT count = 0; count < chunksize; ++count)
	  {
	    xval[count] = (*x)[count];
	    yval[count] = (*y)[count];
	    zval[count] = (*z)[count];
	  }
      }
    //construct 3d intermediate array, subset of array with stride ninterp
    double *temp = new double[nxa*nya*nza];
    ArrayGuard<double> tempGuard( temp);

    // Interpolate iteratively ninterp times:
    // this outer loop could be multihreaded easily
    for (SizeT iterate = 0; iterate < ninterp; ++iterate)
      {
	for (SizeT k = 0; k < nxa*nya*nza; ++k) temp[k]=(*array)[k*ninterp+iterate];
	gdl_interp3d_init(interpolant, xa, ya, za, temp, nxa, nya, nza, use_missing?missing_GIVEN:missing_NEAREST, missing);
#ifndef __PATHCC__
#pragma omp parallel if (chunksize >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= chunksize))
#pragma omp for
#endif
	for (OMPInt i = 0; i < chunksize; ++i)
	  {
	    double x = xval[i];
	    double y = yval[i];
	    double z = zval[i];
	    (*res)[i*ninterp+iterate] = gdl_interp3d_eval(interpolant, xa, ya, za, temp, x, y, z, accx, accy, accz);
	  }
      }

    //     gsl_interp_accel_free(accx);
    //     gsl_interp_accel_free(accy);
    //     gsl_interp_accel_free(accz);
    //     gdl_interp3d_free(interpolant);
    return res;
  }

namespace lib {
  
  BaseGDL* interpolate_fun(EnvT* e){

    SizeT nParam = e->NParam();
    if (nParam < 2) e->Throw("Incorrect number of arguments.");

    // options
    static int cubicIx = e->KeywordIx("CUBIC");
    bool cubic = e->KeywordSet(cubicIx);
    DDouble gamma=-1.0;
    e->AssureDoubleScalarKWIfPresent(cubicIx, gamma);

    static int nnborIx = e->KeywordIx("NEAREST_NEIGHBOUR"); //usage restricted to GDL, undocumented, normally for CONGRID.
    bool nnbor = e->KeywordSet(nnborIx);
    if (nnbor && cubic) nnbor=false;  //undocumented nearest neighbour give way wrt. other options.

    static int gridIx = e->KeywordIx("GRID");
    bool grid = e->KeywordSet(gridIx);

    // AC 2018-feb-01 : NB !! Double Keyword is related to Grid Keyword
    //
    // due to our "bug" report to IDL, /Double was add in IDL since 8.2.3
    // but we don't managed it (/double) in GDL because we always
    // compute Grid related stuffs in Double ....
    //
    //    static int dblIx = e->KeywordIx("DOUBLE");
    // bool dbl = e->KeywordSet(dblIx);

    static int missingIx = e->KeywordIx("MISSING");
    bool use_missing = e->KeywordPresent(missingIx);
    DDouble missing=0;
    if (use_missing) e->AssureDoubleScalarKWIfPresent(missingIx, missing);

    DDoubleGDL* p0D[2];
    DDoubleGDL* p1D;
    DDoubleGDL* p2D;
    DDoubleGDL* p3D;
    Guard<BaseGDL> guard00;
    Guard<BaseGDL> guard01;
    Guard<BaseGDL> guard1;
    Guard<BaseGDL> guard2;
    Guard<BaseGDL> guard3;
    int complexity=1;


    // convert to internal double arrays. Special case for complex values, we separate R and I
    BaseGDL* p0 = e->GetParDefined(0);
    if (p0->Rank() < nParam - 1)
      e->Throw("Number of parameters must agree with dimensions of argument.");
    
    //Ok, Ranks are compatible, but check if last parameter(s) have dimension 1.
    //interpol is unable to interpolate on a zero-size dimension. We will downgrade the number
    //of dimensions to be interpolated, and duplicate the result to the size of the last parameter(s).
    SizeT lastDim=0;
    bool needsLastDims=false;
    dimension dimIni=p0->Dim();
    dimension dimEnd=p0->Dim();
    dimEnd.Purge();
    SizeT RankDiff=dimIni.Rank()-dimEnd.Rank(); //number of 1-sized dimensions consecutive at end
    SizeT resDimInit[ MAXRANK];
    SizeT iAddRank=0;
    for (iAddRank=0;iAddRank<dimEnd.Rank();++iAddRank) resDimInit[iAddRank]=dimEnd[iAddRank];
    //All of the above not useful if grid is not set!
    if (grid && RankDiff>0) {
      for (SizeT i=RankDiff; i>=1; --i){
        lastDim=(e->GetParDefined(nParam-i))->Dim(0);
        if (lastDim > 1) needsLastDims=true; //correct behaviour: last dim=1 is trimmed anyway.
        resDimInit[iAddRank]=(lastDim==0)?1:lastDim;
        iAddRank++;
      }
      nParam-=RankDiff;
    }

    
    if (p0->Type() == GDL_COMPLEX) {
      complexity=2;
      DComplexGDL* c0 = static_cast<DComplexGDL*> (p0);
      p0D[0] = new DDoubleGDL(c0->Dim(), BaseGDL::NOZERO); guard00.Init(p0D[0]);
      p0D[1] = new DDoubleGDL(c0->Dim(), BaseGDL::NOZERO); guard01.Init(p0D[1]);
#pragma omp parallel if ( p0->N_Elements() >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= p0->N_Elements()))
#pragma omp for
      for (OMPInt i = 0; i < c0->N_Elements(); ++i) {
	(*p0D[0])[i] = (*c0)[i].real();
	(*p0D[1])[i] = (*c0)[i].imag();
      }
    }
    else if( p0->Type() == GDL_COMPLEXDBL) {
      complexity=2;
      DComplexDblGDL* c0 = static_cast<DComplexDblGDL*> (p0);
      p0D[0] = new DDoubleGDL(c0->Dim(), BaseGDL::NOZERO); guard00.Init(p0D[0]);
      p0D[1] = new DDoubleGDL(c0->Dim(), BaseGDL::NOZERO); guard01.Init(p0D[1]);
#pragma omp parallel if ( p0->N_Elements() >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= p0->N_Elements()))
#pragma omp for
      for (OMPInt i = 0; i < c0->N_Elements(); ++i) {
	(*p0D[0])[i] = (*c0)[i].real();
	(*p0D[1])[i] = (*c0)[i].imag();
      }
    }
    else if (p0->Type() == GDL_DOUBLE) p0D[0] = static_cast<DDoubleGDL*>(p0);
    else
      {
	p0D[0] = static_cast<DDoubleGDL*>(p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
	guard00.Init(p0D[0]);
      }
    
    BaseGDL* p1 = e->GetParDefined(1);
    if (p1->Type() == GDL_DOUBLE) p1D = static_cast<DDoubleGDL*>(p1);
    else
      {
	p1D = static_cast<DDoubleGDL*>(p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
	guard1.Init(p1D);
      }

    BaseGDL* p2 = NULL;
    if (nParam >= 3) {
      p2 = e->GetParDefined(2);
      if (p2->Type() == GDL_DOUBLE) p2D = static_cast<DDoubleGDL*>(p2);
      else
	{
	  p2D = static_cast<DDoubleGDL*>(p2->Convert2( GDL_DOUBLE, BaseGDL::COPY));
	  guard2.Init(p2D);
	}
    }

    BaseGDL* p3 = NULL;
    if (nParam >= 4) {
      p3 = e->GetParDefined(3);
      if (p3->Type() == GDL_DOUBLE) p3D = static_cast<DDoubleGDL*>(p3);
      else
	{
	  p3D = static_cast<DDoubleGDL*>(p3->Convert2( GDL_DOUBLE, BaseGDL::COPY));
	  guard3.Init(p3D);
	}
    }

    DDoubleGDL* res[2];
    for (int iloop=0; iloop<complexity; ++iloop)
      {
        // 0D Interpolation (special case with needLastDims=true)
        if (nParam < 2) res[iloop]=p0D[iloop]->Dup();
        // 1D Interpolation
        if (nParam == 2) {
          //   res[iloop]=interpolate_1dim(e,p0D[iloop],p1D,cubic,use_missing,missing);
          if (nnbor)   res[iloop]=interpolate_1dim(e,gdl_interp1d_nearest,p0D[iloop],p1D,use_missing,missing,0.0);
          else if (cubic)   res[iloop]=interpolate_1dim(e,gdl_interp1d_cubic,p0D[iloop],p1D,use_missing,missing,gamma);
          else         res[iloop]=interpolate_1dim(e,gdl_interp1d_linear,p0D[iloop],p1D,use_missing,missing,0.0);
        }
        // 2D Interpolation
        if (nParam == 3) {
          if (nnbor)        res[iloop]=interpolate_2dim(e,gdl_interp2d_binearest,p0D[iloop],p1D,p2D,grid,use_missing,missing,0.0);
          else if (cubic)   res[iloop]=interpolate_2dim(e,gdl_interp2d_bicubic,p0D[iloop],p1D,p2D,grid,use_missing,missing,gamma);
          else              res[iloop]=interpolate_2dim(e,gdl_interp2d_bilinear,p0D[iloop],p1D,p2D,grid,use_missing,missing,0.0);
        }
        // 3D Interpolation
        if (nParam == 4) {
          res[iloop]=interpolate_3dim(e,gdl_interp3d_trilinear,p0D[iloop],p1D,p2D,p3D,grid,use_missing,missing);
        }
	//special case where last dimension of input was 1
	if (needsLastDims){
	  dimension dim=(res[iloop])->Dim();
	  for (SizeT i=0; i<(res[iloop])->Rank(); ++i){
	    resDimInit[i]=dim[i]; //put back first dimensions of interpolated result
	  }
	  dimension resDim( resDimInit, iAddRank); //change as Dimension...
	  res[iloop]= static_cast<DDoubleGDL*>(res[iloop]->Rebin(resDim, true)); //Rebin to the extra number of dims.
	}
      }

    // AC 2018-feb-01 : don't put here a conversion with /Double : not need !!
    // in IDL since 8.2.3, /Double is related to grid keyword

    if (p0->Type() == GDL_DOUBLE)
      {
	return res[0];
      }
    else if (p0->Type() == GDL_FLOAT)
      {
	DFloatGDL* res1 = static_cast<DFloatGDL*>
	  (res[0]->Convert2(GDL_FLOAT, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_INT)
      {
	DIntGDL* res1 = static_cast<DIntGDL*>
	  (res[0]->Convert2(GDL_INT, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_UINT)
      {
	DUIntGDL* res1 = static_cast<DUIntGDL*>
	  (res[0]->Convert2(GDL_UINT, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_LONG)
      {
	DLongGDL* res1 = static_cast<DLongGDL*>
	  (res[0]->Convert2(GDL_LONG, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_ULONG)
      {
	DULongGDL* res1 = static_cast<DULongGDL*>
	  (res[0]->Convert2(GDL_ULONG, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_LONG64)
      {
	DLong64GDL* res1 = static_cast<DLong64GDL*>
	  (res[0]->Convert2(GDL_LONG64, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_ULONG64)
      {
	DULong64GDL* res1 = static_cast<DULong64GDL*>
	  (res[0]->Convert2(GDL_ULONG64, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_BYTE)
      {
	DByteGDL* res1 = static_cast<DByteGDL*>
	  (res[0]->Convert2(GDL_BYTE, BaseGDL::COPY));
	delete res[0];
	return res1;
      }
    else if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* res1 = new DComplexGDL(res[0]->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if ( p0->N_Elements() >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= p0->N_Elements()))
#pragma omp for
      for (OMPInt i = 0; i < res1->N_Elements(); ++i) {
	(*res1)[ i] = DComplex((*res[0])[i],(*res[1])[i]);
	//             (*res1)[i].real() = (*res[0])[i];
	//             (*res1)[i].imag() = (*res[1])[i];
      }
      delete res[0]; delete res[1];
      return res1;       
    }
    else if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* res1 = new DComplexDblGDL(res[0]->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if ( p0->N_Elements() >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= p0->N_Elements()))
#pragma omp for
      for (OMPInt i = 0; i < res1->N_Elements(); ++i) {
	(*res1)[ i] = DComplexDbl((*res[0])[i],(*res[1])[i]);
	// 	    (*res1)[i].real() = (*res[0])[i];
	//             (*res1)[i].imag() = (*res[1])[i];
      }
      delete res[0]; delete res[1];
      return res1;       
    }
    else //?
      {
	return res[0];
      }

  }

}
