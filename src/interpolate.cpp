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

#define CALL_INTERPOLATE_1D(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  if (cubic)  interpolate_1d_cubic<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else if (nnbor) interpolate_1d_nearest<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous);\
  else  interpolate_1d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_1D_SINGLE(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  if (cubic)  interpolate_1d_cubic_single<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()), use_missing, missing);\
  else if (nnbor) interpolate_1d_nearest_single<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()));\
  else  interpolate_1d_linear_single<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()), use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_1D_COMPLEX(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  ncontiguous *= 2;\
  if (cubic)  interpolate_1d_cubic<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else if (nnbor) interpolate_1d_nearest<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous);\
  else interpolate_1d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}

#define CALL_INTERPOLATE_2D(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G2* p2D=e->GetParAs<G2>(2);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  if (cubic) interpolate_2d_cubic<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else interpolate_2d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_2D_COMPLEX(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G2* p2D=e->GetParAs<G2>(2);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  ncontiguous *= 2;\
  if (cubic) interpolate_2d_cubic<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else interpolate_2d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_2D_GRID(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G2* p2D=e->GetParAs<G2>(2);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  if (cubic) interpolate_2d_cubic_grid<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else if (nnbor) interpolate_2d_nearest_grid<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous);\
  else interpolate_2d_linear_grid<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_2D_GRID_COMPLEX(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G2* p2D=e->GetParAs<G2>(2);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  ncontiguous *= 2;\
  if (cubic) interpolate_2d_cubic_grid<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else if (nnbor) interpolate_2d_nearest_grid<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous);\
  else interpolate_2d_linear_grid<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}

#define CALL_INTERPOLATE_3D(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G2* p2D=e->GetParAs<G2>(2);\
  G2* p3D=e->GetParAs<G2>(3);\
  G1* res = new G1(outdim, BaseGDL::NOZERO);\
  interpolate_3d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, un3, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),static_cast<T2*>(p3D->DataAddr()),static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}

static double gdl_cubic_gamma = -1.0;

void gdl_update_cubic_interpolation_coeff(double gammaValue) {
  gdl_cubic_gamma = -1;
  if (gammaValue <= 0 && gammaValue >= -1.0) gdl_cubic_gamma = gammaValue;
}

template <typename T1, typename T2> //NEAREST_NEIGHBOUR is NOT STANDARD and is USED only by CONGRID. There is no "missing" possibility.
void interpolate_1d_nearest(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, SizeT ncontiguous) {

  T1 *v0, *vres;
  T2 x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t n1 = un1;
#pragma omp parallel private(ix,x,v0,vres) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
  {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) {
      vres = &(res[ncontiguous * j]);
      x = xx[j];
      if (x < 0) {
        v0 = &(array[0]);
      } else if (x < n1 - 1) {
        ix = floor(x); //floor  ix is [0 .. n1[
        v0 = &(array[ncontiguous * ix]);
      } else {
        v0 = &(array[ncontiguous * (n1 - 1)]);
      }
      for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = v0[i];
    }
  }
}

template <typename T1, typename T2> //NEAREST_NEIGHBOUR is NOT STANDARD and is USED only by CONGRID. There is no "missing" possibility.
void interpolate_1d_nearest_single(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res) {
  T2 x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t n1 = un1;
#pragma omp parallel private(ix,x) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
  {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) {
      x = xx[j];
      if (x < 0) {
        res[j] = array[0];
      } else if (x < n1 - 1) {
        ix = floor(x); //floor  ix is [0 .. n1[
        res[j] = array[ix];
      } else {
        res[j] = array[n1 - 1];
      }
    }
  }
}

template <typename T1, typename T2>
void interpolate_1d_linear(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {

  T1 *v0, *v1, *vres;
  double dx; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[2];
  ssize_t n1 = un1;
  if (use_missing) {
#pragma omp parallel private(xi,ix,dx,x,v0,v1,vres) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      vres = &(res[ncontiguous * j]);
      x = xx[j];
      if (x < 0) {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      } else if (x < n1) {
        ix = floor(x);
        xi[0]=ix; xi[1]=ix+1;
      //make in range
        if (xi[0]<0) xi[0]=0; else if (xi[0]>n1-1) xi[0]=n1-1;
        if (xi[1]<0) xi[1]=0; else if (xi[1]>n1-1) xi[1]=n1-1; 
        dx = (x - xi[0]);
        v0 = &(array[ncontiguous * xi[0]]);
        v1 = &(array[ncontiguous * xi[1]]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = (1. - dx) * v0[i] + dx * v1[i];
        }
      } else {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      }
    }
    }
  } else {
#pragma omp parallel private(xi,ix,dx,x,v0,v1,vres) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
      {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) {
      vres = &(res[ncontiguous * j]);
      x = xx[j];
      if (x < 0) {
        v0 = &(array[0]);
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = v0[i];
      } else if (x < n1 - 1) {
        ix = floor(x);
        xi[0]=ix; xi[1]=ix+1;
      //make in range
        if (xi[0]<0) xi[0]=0; else if (xi[0]>n1-1) xi[0]=n1-1;
        if (xi[1]<0) xi[1]=0; else if (xi[1]>n1-1) xi[1]=n1-1; 
        dx = (x - xi[0]);
        v0 = &(array[ncontiguous * xi[0]]);
        v1 = &(array[ncontiguous * xi[1]]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = (1. - dx) * v0[i] + dx * v1[i];
        }
      } else {
        v0 = &(array[ncontiguous * (n1 - 1)]);
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = v0[i];
      }
    }
      }
  }
}

template <typename T1, typename T2>
void interpolate_1d_linear_single(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, bool use_missing, DDouble missing) {

  double dx; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[2];
  ssize_t n1 = un1;
  if (use_missing) {
#pragma omp parallel private(xi,ix,dx,x) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      x = xx[j];
      if (x < 0) {
        res[j] = missing;
      }
      else if (x < n1) {
        ix = floor(x);
        xi[0]=ix; xi[1]=ix+1;
      //make in range
        if (xi[0]<0) xi[0]=0; else if (xi[0]>n1-1) xi[0]=n1-1;
        if (xi[1]<0) xi[1]=0; else if (xi[1]>n1-1) xi[1]=n1-1; 
        dx = (x - xi[0]);
        res[j] = (1. - dx) * array[xi[0]] + dx * array[xi[1]];
      } else {
        res[j] = missing;
      }
    }
    }
  } else {
#pragma omp parallel private(xi,ix,dx,x) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) {
      x = xx[j];
      if (x < 0) {
        res[j] = array[0];
      } else if (x < n1) {
        ix = floor(x);
        xi[0]=ix; xi[1]=ix+1;
      //make in range
        if (xi[0]<0) xi[0]=0; else if (xi[0]>n1-1) xi[0]=n1-1;
        if (xi[1]<0) xi[1]=0; else if (xi[1]>n1-1) xi[1]=n1-1; 
        dx = (x - xi[0]);
        res[j] = (1. - dx) * array[xi[0]] + dx * array[xi[1]];
      } else {
        res[j] = array[n1-1];
      }
    }
    }
  }
}


template <typename T1, typename T2>
void interpolate_1d_cubic(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *v0, *v1, *v2, *v3, *vres;
  double dx;
  double g = gdl_cubic_gamma;
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[4];
  ssize_t n1 = un1;
  if (use_missing) {
#pragma omp parallel private(xi,ix,dx,x,v0,v1,v2,v3,vres) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      vres = &(res[ncontiguous * j]);
      x = xx[j];
     if (x < 0) {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      } else if (x  < n1-1 ) { 
        ix = floor(x); //floor  ix is [0 .. n1[
        xi[0]=ix-1; xi[1]=ix; xi[2]=ix+1; xi[3]=ix+2;
      //make in range
        if (xi[0]<0) xi[0]=0; else if (xi[0]>n1-1) xi[0]=n1-1;
        if (xi[1]<0) xi[1]=0; else if (xi[1]>n1-1) xi[1]=n1-1; 
        if (xi[2]<0) xi[2]=0; else if (xi[2]>n1-1) xi[2]=n1-1; 
        if (xi[3]<0) xi[3]=0; else if (xi[3]>n1-1) xi[3]=n1-1; 
        v0 = &(array[ncontiguous * xi[0]]);
        v1 = &(array[ncontiguous * xi[1]]);
        v2 = &(array[ncontiguous * xi[2]]);
        v3 = &(array[ncontiguous * xi[3]]);
        dx = (x - xi[1]);
        double d2 = dx*dx;
        double d3 = d2*dx;
        double omd = 1 - dx;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + dx;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - dx;
        double dmd2 = dmd*dmd;
        double dmd3 = dmd2*dmd;
        double c1 = ((g + 2) * d3 - (g + 3) * d2 + 1);
        double c2 = ((g + 2) * omd3 - (g + 3) * omd2 + 1);
        double c0 = (g * opd3 - 5 * g * opd2 + 8 * g * opd - 4 * g);
        double c3 = (g * dmd3 - 5 * g * dmd2 + 8 * g * dmd - 4 * g);

        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = c1 * v1[i] + c2 * v2[i] + c0 * v0[i] + c3 * v3[i];
        }
      } else if (x < n1) {
        v0 = &(array[ncontiguous * (n1-1)]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = v0[i];
        }
      } else {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      }
    }
    }
  } else {
#pragma omp parallel private(xi,ix,dx,x,v0,v1,v2,v3,vres) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      vres = &(res[ncontiguous * j]);
      x = xx[j];
     if (x < 0) {
        v0 = &(array[0]);
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = v0[i];
      } else if (x  < n1-1 ) { 
        ix = floor(x); //floor  ix is [0 .. n1[
        xi[0]=ix-1; xi[1]=ix; xi[2]=ix+1; xi[3]=ix+2;
      //make in range
        if (xi[0]<0) xi[0]=0; else if (xi[0]>n1-1) xi[0]=n1-1;
        if (xi[1]<0) xi[1]=0; else if (xi[1]>n1-1) xi[1]=n1-1; 
        if (xi[2]<0) xi[2]=0; else if (xi[2]>n1-1) xi[2]=n1-1; 
        if (xi[3]<0) xi[3]=0; else if (xi[3]>n1-1) xi[3]=n1-1; 
        v0 = &(array[ncontiguous * xi[0]]);
        v1 = &(array[ncontiguous * xi[1]]);
        v2 = &(array[ncontiguous * xi[2]]);
        v3 = &(array[ncontiguous * xi[3]]);
        dx = (x - xi[1]);
        double d2 = dx*dx;
        double d3 = d2*dx;
        double omd = 1 - dx;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + dx;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - dx;
        double dmd2 = dmd*dmd;
        double dmd3 = dmd2*dmd;
        double c1 = ((g + 2) * d3 - (g + 3) * d2 + 1);
        double c2 = ((g + 2) * omd3 - (g + 3) * omd2 + 1);
        double c0 = (g * opd3 - 5 * g * opd2 + 8 * g * opd - 4 * g);
        double c3 = (g * dmd3 - 5 * g * dmd2 + 8 * g * dmd - 4 * g);

        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = c1 * v1[i] + c2 * v2[i] + c0 * v0[i] + c3 * v3[i];
        }
      } else {
        v0 = &(array[ncontiguous * (n1-1)]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = v0[i];
        }
      }
    }
    }
  }
}

template <typename T1, typename T2>
void interpolate_1d_cubic_single(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, bool use_missing, DDouble missing) {
  double dx;
  double g = gdl_cubic_gamma;
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[4];
  ssize_t n1 = un1;
  if (use_missing) {
#pragma omp parallel private(xi,ix,dx,x) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      x = xx[j];
      if (x < 0) {
        res[j] = missing;
      } else if (x < n1 - 1) {
        ix = floor(x); //floor  ix is [0 .. n1[
        xi[0] = ix - 1;
        xi[1] = ix;
        xi[2] = ix + 1;
        xi[3] = ix + 2;
        //make in range
        if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 - 1) xi[0] = n1 - 1;
        if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
        if (xi[2] < 0) xi[2] = 0; else if (xi[2] > n1 - 1) xi[2] = n1 - 1;
        if (xi[3] < 0) xi[3] = 0; else if (xi[3] > n1 - 1) xi[3] = n1 - 1;
        dx = (x - xi[1]);
        double d2 = dx*dx;
        double d3 = d2*dx;
        double omd = 1 - dx;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + dx;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - dx;
        double dmd2 = dmd*dmd;
        double dmd3 = dmd2*dmd;
        double c1 = ((g + 2) * d3 - (g + 3) * d2 + 1);
        double c2 = ((g + 2) * omd3 - (g + 3) * omd2 + 1);
        double c0 = (g * opd3 - 5 * g * opd2 + 8 * g * opd - 4 * g);
        double c3 = (g * dmd3 - 5 * g * dmd2 + 8 * g * dmd - 4 * g);
        res[j] = c1 * array[xi[1]] + c2 * array[xi[2]] + c0 * array[xi[0]] + c3 * array[xi[3]];
      } else if (x < n1) {
        res[j] = array[n1 - 1];
      } else {
        res[j] = missing;
      }
    }
    }
  } else {
#pragma omp parallel private(xi,ix,dx,x) if (CpuTPOOL_NTHREADS> 1 && nx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx))
    {
#pragma omp for 
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      x = xx[j];
      if (x < 0) {
        res[j] = array[0];
      } else if (x < n1 - 1) {
        ix = floor(x);  if (x<0)x=0; if(x>n1-1)x=n1-1; //floor  ix is [0 .. n1[
        xi[0] = ix - 1;
        xi[1] = ix;
        xi[2] = ix + 1;
        xi[3] = ix + 2;
        //make in range
        if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 - 1) xi[0] = n1 - 1;
        if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
        if (xi[2] < 0) xi[2] = 0; else if (xi[2] > n1 - 1) xi[2] = n1 - 1;
        if (xi[3] < 0) xi[3] = 0; else if (xi[3] > n1 - 1) xi[3] = n1 - 1;
        dx = (x - xi[1]);
        double d2 = dx*dx;
        double d3 = d2*dx;
        double omd = 1 - dx;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + dx;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - dx;
        double dmd2 = dmd*dmd;
        double dmd3 = dmd2*dmd;
        double c1 = ((g + 2) * d3 - (g + 3) * d2 + 1);
        double c2 = ((g + 2) * omd3 - (g + 3) * omd2 + 1);
        double c0 = (g * opd3 - 5 * g * opd2 + 8 * g * opd - 4 * g);
        double c3 = (g * dmd3 - 5 * g * dmd2 + 8 * g * dmd - 4 * g);
        res[j] = c1 * array[xi[1]] + c2 * array[xi[2]] + c0 * array[xi[0]] + c3 * array[xi[3]];
      } else {
        res[j] = array[n1-1];
      }
    }
    }
  }
}

template <typename T1, typename T2>
void interpolate_2d_nearest_grid(T1* array, SizeT un1, SizeT un2, T2* xx, SizeT nx, T2* yy, SizeT ny, T1* res, SizeT ncontiguous) {
  T1 *vx0, *vres;
  double x, y;
  ssize_t ix = 0;
  ssize_t iy = 0; //operations on unsigned are not what you think, signed are ok
  ssize_t xi, yi; //operations on unsigned are not what you think, signed are ok
  ssize_t n1 = un1;
  ssize_t n2 = un2;
#pragma omp parallel private(xi,yi,ix,iy,x,y,vx0,vres) if (CpuTPOOL_NTHREADS> 1 && nx*ny >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx*ny))
  {
#pragma omp for collapse(2)
  for (SizeT k = 0; k < ny; ++k) {
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      vres = &(res[ncontiguous * (k * nx + j) ]);
      x = xx[j];
      if (x < 0) {
        xi = 0;
      } else if (x >= n1-1 ) {
        xi = n1-1;
      } else {
        ix = floor(x);
        xi = ix;
      }
      y = yy[k];
      if (y < 0) {
        yi = 0; 
      } else if (y >= n2-1 ) {
        yi = n2-1; 
      } else {
        iy = floor(y);
        yi = iy;
      }
      vx0 = &(array[ncontiguous * (yi * n1 + xi)]);
      for (SizeT i = 0; i < ncontiguous; ++i) {
        vres[i] = vx0[i];
      }
    }
  }
  }
}

template <typename T1, typename T2>
void interpolate_2d_linear(T1* array, SizeT un1,  SizeT un2, T2* xx, SizeT n, T2* yy, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *vx0, *vx1, *vy0, *vy1, *vres;
  double dx, dy; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x, y;
  ssize_t ix = 0;
  ssize_t iy = 0; //operations on unsigned are not what you think, signed are ok
  ssize_t xi[2], yi[2]; //operations on unsigned are not what you think, signed are ok
  ssize_t n1 = un1;
  ssize_t n2 = un2;
  if (use_missing) { //following behaviour validated.
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0,vx1,vy0,vy1,vres) if (CpuTPOOL_NTHREADS> 1 && n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
    {
#pragma omp for 
      for (SizeT j = 0; j < n; ++j) { //nb output points
        vres = &(res[ncontiguous * j ]);
        x = xx[j];
        if (x < 0) {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        } else if (x <= n1 - 1) {
          y = yy[j];
          if (y < 0) {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          } else if (y <= n2 - 1) {
            ix = floor(x);
            xi[0] = ix;
            xi[1] = ix + 1;
            //make in range
            if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
            dx = (x - xi[0]);
            iy = floor(y);
            yi[0] = iy;
            yi[1] = iy + 1;
            //make in range
            if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
            dy = (y - yi[0]);
            vx0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
            vx1 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
            vy0 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
            vy1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
            for (SizeT i = 0; i < ncontiguous; ++i) {
              double dxdy = dx*dy;
              double c0 = (1 - dy - dx + dxdy);
              double c1 = (dy - dxdy);
              double c2 = (dx - dxdy);
              vres[i] = vx0[i] * c0 + vy0[i] * c1 + vx1[i] * c2 + vy1[i] * dxdy;
            }
          } else {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          }
        } else {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
      }
    }
  } else { //following behaviour validated.
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0,vx1,vy0,vy1,vres) if (CpuTPOOL_NTHREADS> 1 && n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
    {
#pragma omp for 
      for (SizeT j = 0; j < n; ++j) { //nb output points
        vres = &(res[ncontiguous * j ]);
        x = xx[j];
        if (x < 0) {
          xi[0] = 0;
          xi[1] = 0;
        } else if (x >= n1 - 1) {
          xi[0] = n1 - 1;
          xi[1] = n1 - 1;
        } else {
          ix = floor(x);
          xi[0] = ix;
          xi[1] = ix + 1;
        }
        y = yy[j];
        if (y < 0) {
          yi[0] = 0;
          yi[1] = 0;
        } else if (y >= n2 - 1) {
          yi[0] = n2 - 1;
          yi[1] = n2 - 1;
        } else {
          iy = floor(y);
          yi[0] = iy;
          yi[1] = iy + 1;
        }
        dx = (x - xi[0]);
        dy = (y - yi[0]);
        vx0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
        vx1 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
        vy0 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
        vy1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          double dxdy = dx*dy;
          double c0 = (1 - dy - dx + dxdy);
          double c1 = (dy - dxdy);
          double c2 = (dx - dxdy);
          vres[i] = vx0[i] * c0 + vy0[i] * c1 + vx1[i] * c2 + vy1[i] * dxdy;
        }
      }
    }
  }
}

template <typename T1, typename T2>
void interpolate_2d_linear_grid(T1* array, SizeT un1, SizeT un2, T2* xx, SizeT nx, T2* yy, SizeT ny, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *vx0, *vx1, *vy0, *vy1, *vres;
  double dx, dy; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x, y;
  ssize_t ix = 0;
  ssize_t iy = 0; //operations on unsigned are not what you think, signed are ok
  ssize_t xi[2], yi[2]; //operations on unsigned are not what you think, signed are ok
  ssize_t n1 = un1;
  ssize_t n2 = un2;
  if (use_missing) {  //following behaviour validated.
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0,vx1,vy0,vy1,vres) if (CpuTPOOL_NTHREADS> 1 && nx*ny >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx*ny))
    {
#pragma omp for collapse(2)
      for (SizeT k = 0; k < ny; ++k) {
      for (SizeT j = 0; j < nx; ++j) { //nb output points
        vres = &(res[ncontiguous * (k * nx + j) ]);
        x = xx[j];
        if (x < 0) {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        } else if (x <= n1 - 1) {
          y = yy[k];
          if (y < 0) {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          } else if (y <= n2 - 1) {
            ix = floor(x);
            xi[0] = ix;
            xi[1] = ix + 1;
            //make in range
            if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
            dx = (x - xi[0]);
            iy = floor(y);
            yi[0] = iy;
            yi[1] = iy + 1;
            //make in range
            if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
            dy = (y - yi[0]);
            vx0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
            vx1 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
            vy0 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
            vy1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
            for (SizeT i = 0; i < ncontiguous; ++i) {
              double dxdy=dx*dy;
              double c0=(1-dy-dx+dxdy);
              double c1=(dy-dxdy);
              double c2=(dx-dxdy);
              vres[i] = vx0[i] * c0 + vy0[i] * c1 + vx1[i] * c2 + vy1[i]*dxdy;
            }
          } else {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          }
        } else {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
      }
    }
    }
  } else { //following behaviour validated.
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0,vx1,vy0,vy1,vres) if (CpuTPOOL_NTHREADS> 1 && nx*ny >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx*ny))
    {
#pragma omp for collapse(2)
    for (SizeT k = 0; k < ny; ++k) {
      for (SizeT j = 0; j < nx; ++j) { //nb output points
        vres = &(res[ncontiguous * (k * nx + j) ]);
        x = xx[j];
        if (x < 0) {
          xi[0] = 0; xi[1] = 0;
        } else if (x >= n1-1 ) {
          xi[0] = n1-1; xi[1] = n1-1;
        } else {
          ix = floor(x);
          xi[0] = ix;
          xi[1] = ix + 1;
        }
        y = yy[k];
        if (y < 0) {
          yi[0] = 0; yi[1] = 0;
        } else if (y >= n2-1 ) {
          yi[0] = n2-1; yi[1] = n2-1;
        } else {
          iy = floor(y);
          yi[0] = iy;
          yi[1] = iy + 1;
        }
        dx = (x - xi[0]);
        dy = (y - yi[0]);
        vx0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
        vx1 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
        vy0 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
        vy1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          double dxdy=dx*dy;
          double c0=(1-dy-dx+dxdy);
          double c1=(dy-dxdy);
          double c2=(dx-dxdy);
          vres[i] = vx0[i] * c0 + vy0[i] * c1 + vx1[i] * c2 + vy1[i]*dxdy;
        }
      }
    }
    }
  }
}

template <typename T1, typename T2>
void interpolate_2d_cubic(T1* array, SizeT un1, SizeT un2, T2* xx, SizeT n, T2* yy, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *vx0y0, *vx1y0, *vx2y0, *vx3y0;
  T1 *vx0y1, *vx1y1, *vx2y1, *vx3y1;
  T1 *vx0y2, *vx1y2, *vx2y2, *vx3y2;
  T1 *vx0y3, *vx1y3, *vx2y3, *vx3y3;
  T1 *vres;
  double g = gdl_cubic_gamma;
  double dx, dy; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x, y;
  ssize_t ix = 0;
  ssize_t iy = 0; //operations on unsigned are not what you think, signed are ok
  ssize_t xi[4], yi[4]; //operations on unsigned are not what you think, signed are ok
  ssize_t n1 = un1;
  ssize_t n2 = un2;
  if (use_missing) { 
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0y0,vx1y0,vx2y0,vx3y0,vx0y1,vx1y1,vx2y1,vx3y1,vx0y2,vx1y2,vx2y2,vx3y2,vx0y3,vx1y3,vx2y3,vx3y3,vres) if (CpuTPOOL_NTHREADS> 1 && n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
    {
#pragma omp for 
      for (SizeT j = 0; j < n; ++j) { //nb output points
        vres = &(res[ncontiguous * j]);
        x = xx[j];
        if (x < 0) {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        } else if (x <= n1 - 1) {
          y = yy[j];
          if (y < 0) {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          } else if (y <= n2 - 1) {
            ix = floor(x); //floor  ix is [0 .. n1[
            xi[0] = ix - 1;
            xi[1] = ix;
            xi[2] = ix + 1;
            xi[3] = ix + 2;
            //make in range
            if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 - 1) xi[0] = n1 - 1;
            if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
            if (xi[2] < 0) xi[2] = 0; else if (xi[2] > n1 - 1) xi[2] = n1 - 1;
            if (xi[3] < 0) xi[3] = 0; else if (xi[3] > n1 - 1) xi[3] = n1 - 1;
            dx = (x - xi[1]);
            double dx2 = dx*dx;
            double dx3 = dx2*dx;
            double omdx = 1 - dx;
            double omdx2 = omdx*omdx;
            double omdx3 = omdx2*omdx;
            double opdx = 1 + dx;
            double opdx2 = opdx*opdx;
            double opdx3 = opdx2*opdx;
            double dmdx = 2 - dx;
            double dmdx2 = dmdx*dmdx;
            double dmdx3 = dmdx2*dmdx;
            double cx1 = ((g + 2) * dx3 - (g + 3) * dx2 + 1);
            double cx2 = ((g + 2) * omdx3 - (g + 3) * omdx2 + 1);
            double cx0 = (g * opdx3 - 5 * g * opdx2 + 8 * g * opdx - 4 * g);
            double cx3 = (g * dmdx3 - 5 * g * dmdx2 + 8 * g * dmdx - 4 * g);

            iy = floor(y);
            yi[0] = iy - 1;
            yi[1] = iy;
            yi[2] = iy + 1;
            yi[3] = iy + 2;
            //make in range
            if (yi[0] < 0) yi[0] = 0; else if (yi[0] > n2 - 1) yi[0] = n2 - 1;
            if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
            if (yi[2] < 0) yi[2] = 0; else if (yi[2] > n2 - 1) yi[2] = n2 - 1;
            if (yi[3] < 0) yi[3] = 0; else if (yi[3] > n2 - 1) yi[3] = n2 - 1;
            dy = (y - yi[1]);
            double dy2 = dy*dy;
            double dy3 = dy2*dy;
            double omdy = 1 - dy;
            double omdy2 = omdy*omdy;
            double omdy3 = omdy2*omdy;
            double opdy = 1 + dy;
            double opdy2 = opdy*opdy;
            double opdy3 = opdy2*opdy;
            double dmdy = 2 - dy;
            double dmdy2 = dmdy*dmdy;
            double dmdy3 = dmdy2*dmdy;
            double cy1 = ((g + 2) * dy3 - (g + 3) * dy2 + 1);
            double cy2 = ((g + 2) * omdy3 - (g + 3) * omdy2 + 1);
            double cy0 = (g * opdy3 - 5 * g * opdy2 + 8 * g * opdy - 4 * g);
            double cy3 = (g * dmdy3 - 5 * g * dmdy2 + 8 * g * dmdy - 4 * g);
            vx0y0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
            vx1y0 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
            vx2y0 = &(array[ncontiguous * (yi[0] * n1 + xi[2])]);
            vx3y0 = &(array[ncontiguous * (yi[0] * n1 + xi[3])]);

            vx0y1 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
            vx1y1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
            vx2y1 = &(array[ncontiguous * (yi[1] * n1 + xi[2])]);
            vx3y1 = &(array[ncontiguous * (yi[1] * n1 + xi[3])]);

            vx0y2 = &(array[ncontiguous * (yi[2] * n1 + xi[0])]);
            vx1y2 = &(array[ncontiguous * (yi[2] * n1 + xi[1])]);
            vx2y2 = &(array[ncontiguous * (yi[2] * n1 + xi[2])]);
            vx3y2 = &(array[ncontiguous * (yi[2] * n1 + xi[3])]);

            vx0y3 = &(array[ncontiguous * (yi[3] * n1 + xi[0])]);
            vx1y3 = &(array[ncontiguous * (yi[3] * n1 + xi[1])]);
            vx2y3 = &(array[ncontiguous * (yi[3] * n1 + xi[2])]);
            vx3y3 = &(array[ncontiguous * (yi[3] * n1 + xi[3])]);

            for (SizeT i = 0; i < ncontiguous; ++i) {
              double r0=cx1*vx1y0[i]+cx2*vx2y0[i]+cx0*vx0y0[i]+cx3*vx3y0[i];
              double r1=cx1*vx1y1[i]+cx2*vx2y1[i]+cx0*vx0y1[i]+cx3*vx3y1[i];
              double r2=cx1*vx1y2[i]+cx2*vx2y2[i]+cx0*vx0y2[i]+cx3*vx3y2[i];
              double r3=cx1*vx1y3[i]+cx2*vx2y3[i]+cx0*vx0y3[i]+cx3*vx3y3[i];
              vres[i] = cy1*r1+cy2*r2+cy0*r0+cy3*r3;
            }
          } else {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          }
        } else {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
      }
    }
  } else {
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0y0,vx1y0,vx2y0,vx3y0,vx0y1,vx1y1,vx2y1,vx3y1,vx0y2,vx1y2,vx2y2,vx3y2,vx0y3,vx1y3,vx2y3,vx3y3,vres) if (CpuTPOOL_NTHREADS> 1 && n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
  {
#pragma omp for 
    for (SizeT j = 0; j < n; ++j) { //nb output points
        vres = &(res[ncontiguous * j ]);
        x = xx[j]; if (x<0)x=0; if(x>n1-1)x=n1-1;
        ix = floor(x); //floor  ix is [0 .. n1[
        xi[0] = ix - 1;
        xi[1] = ix;
        xi[2] = ix + 1;
        xi[3] = ix + 2;
        //make in range
        if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 -1 ) xi[0] = n1 - 1;
        if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 -1 ) xi[1] = n1 - 1;
        if (xi[2] < 0) xi[2] = 0; else if (xi[2] > n1 -1 ) xi[2] = n1 - 1;
        if (xi[3] < 0) xi[3] = 0; else if (xi[3] > n1 -1 ) xi[3] = n1 - 1;
        dx = (x - xi[1]);
        double dx2 = dx*dx;
        double dx3 = dx2*dx;
        double omdx = 1 - dx;
        double omdx2 = omdx*omdx;
        double omdx3 = omdx2*omdx;
        double opdx = 1 + dx;
        double opdx2 = opdx*opdx;
        double opdx3 = opdx2*opdx;
        double dmdx = 2 - dx;
        double dmdx2 = dmdx*dmdx;
        double dmdx3 = dmdx2*dmdx;
        double cx1 = ((g + 2) * dx3 - (g + 3) * dx2 + 1);
        double cx2 = ((g + 2) * omdx3 - (g + 3) * omdx2 + 1);
        double cx0 = (g * opdx3 - 5 * g * opdx2 + 8 * g * opdx - 4 * g);
        double cx3 = (g * dmdx3 - 5 * g * dmdx2 + 8 * g * dmdx - 4 * g);

        y = yy[j]; if (y<0)y=0; if(y>n2-1)y=n2-1;
        iy = floor(y);
        yi[0] = iy - 1;
        yi[1] = iy;
        yi[2] = iy + 1;
        yi[3] = iy + 2;
        //make in range
        if (yi[0] < 0) yi[0] = 0; else if (yi[0] > n2-1 ) yi[0] = n2 - 1;
        if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2-1 ) yi[1] = n2 - 1;
        if (yi[2] < 0) yi[2] = 0; else if (yi[2] > n2-1 ) yi[2] = n2 - 1;
        if (yi[3] < 0) yi[3] = 0; else if (yi[3] > n2-1 ) yi[3] = n2 - 1;
        dy = (y - yi[1]);
        double dy2 = dy*dy;
        double dy3 = dy2*dy;
        double omdy = 1 - dy;
        double omdy2 = omdy*omdy;
        double omdy3 = omdy2*omdy;
        double opdy = 1 + dy;
        double opdy2 = opdy*opdy;
        double opdy3 = opdy2*opdy;
        double dmdy = 2 - dy;
        double dmdy2 = dmdy*dmdy;
        double dmdy3 = dmdy2*dmdy;
        double cy1 = ((g + 2) * dy3 - (g + 3) * dy2 + 1);
        double cy2 = ((g + 2) * omdy3 - (g + 3) * omdy2 + 1);
        double cy0 = (g * opdy3 - 5 * g * opdy2 + 8 * g * opdy - 4 * g);
        double cy3 = (g * dmdy3 - 5 * g * dmdy2 + 8 * g * dmdy - 4 * g);

        vx0y0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
        vx1y0 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
        vx2y0 = &(array[ncontiguous * (yi[0] * n1 + xi[2])]);
        vx3y0 = &(array[ncontiguous * (yi[0] * n1 + xi[3])]);

        vx0y1 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
        vx1y1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
        vx2y1 = &(array[ncontiguous * (yi[1] * n1 + xi[2])]);
        vx3y1 = &(array[ncontiguous * (yi[1] * n1 + xi[3])]);

        vx0y2 = &(array[ncontiguous * (yi[2] * n1 + xi[0])]);
        vx1y2 = &(array[ncontiguous * (yi[2] * n1 + xi[1])]);
        vx2y2 = &(array[ncontiguous * (yi[2] * n1 + xi[2])]);
        vx3y2 = &(array[ncontiguous * (yi[2] * n1 + xi[3])]);

        vx0y3 = &(array[ncontiguous * (yi[3] * n1 + xi[0])]);
        vx1y3 = &(array[ncontiguous * (yi[3] * n1 + xi[1])]);
        vx2y3 = &(array[ncontiguous * (yi[3] * n1 + xi[2])]);
        vx3y3 = &(array[ncontiguous * (yi[3] * n1 + xi[3])]);

        for (SizeT i = 0; i < ncontiguous; ++i) {
          double r0 = cx1 * vx1y0[i] + cx2 * vx2y0[i] + cx0 * vx0y0[i] + cx3 * vx3y0[i];
          double r1 = cx1 * vx1y1[i] + cx2 * vx2y1[i] + cx0 * vx0y1[i] + cx3 * vx3y1[i];
          double r2 = cx1 * vx1y2[i] + cx2 * vx2y2[i] + cx0 * vx0y2[i] + cx3 * vx3y2[i];
          double r3 = cx1 * vx1y3[i] + cx2 * vx2y3[i] + cx0 * vx0y3[i] + cx3 * vx3y3[i];
          vres[i] = cy1 * r1 + cy2 * r2 + cy0 * r0 + cy3*r3;
        }
      }
  }
  }
}

template <typename T1, typename T2>
void interpolate_2d_cubic_grid(T1* array, SizeT un1, SizeT un2, T2* xx, const SizeT nx, T2* yy, const SizeT ny, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *vx0y0, *vx1y0, *vx2y0, *vx3y0;
  T1 *vx0y1, *vx1y1, *vx2y1, *vx3y1;
  T1 *vx0y2, *vx1y2, *vx2y2, *vx3y2;
  T1 *vx0y3, *vx1y3, *vx2y3, *vx3y3;
  T1 *vres;
  const double g = gdl_cubic_gamma;
  double dx, dy; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x, y;
  ssize_t ix = 0;
  ssize_t iy = 0; //operations on unsigned are not what you think, signed are ok
  ssize_t xi[4], yi[4]; //operations on unsigned are not what you think, signed are ok
  const ssize_t n1 = un1;
  const ssize_t n2 = un2;
  if (use_missing) {
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0y0,vx1y0,vx2y0,vx3y0,vx0y1,vx1y1,vx2y1,vx3y1,vx0y2,vx1y2,vx2y2,vx3y2,vx0y3,vx1y3,vx2y3,vx3y3,vres) if (CpuTPOOL_NTHREADS> 1 && nx*ny >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx*ny))
    {
#pragma omp for collapse(2)
    for (SizeT k = 0; k < ny; ++k) {
      for (SizeT j = 0; j < nx; ++j) { //nb output points
        vres = &(res[ncontiguous * (k * nx + j) ]);
        x = xx[j];
        if (x < 0) {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        } else if (x <= n1 - 1) {
          y = yy[k];
          if (y < 0) {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          } else if (y <= n2 - 1) {
            ix = floor(x); //floor  ix is [0 .. n1[
            xi[0] = ix - 1;
            xi[1] = ix;
            xi[2] = ix + 1;
            xi[3] = ix + 2;
            //make in range
            if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 - 1) xi[0] = n1 - 1;
            if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
            if (xi[2] < 0) xi[2] = 0; else if (xi[2] > n1 - 1) xi[2] = n1 - 1;
            if (xi[3] < 0) xi[3] = 0; else if (xi[3] > n1 - 1) xi[3] = n1 - 1;
            dx = (x - xi[1]);
            double dx2 = dx*dx;
            double dx3 = dx2*dx;
            double omdx = 1 - dx;
            double omdx2 = omdx*omdx;
            double omdx3 = omdx2*omdx;
            double opdx = 1 + dx;
            double opdx2 = opdx*opdx;
            double opdx3 = opdx2*opdx;
            double dmdx = 2 - dx;
            double dmdx2 = dmdx*dmdx;
            double dmdx3 = dmdx2*dmdx;
            double cx1 = ((g + 2) * dx3 - (g + 3) * dx2 + 1);
            double cx2 = ((g + 2) * omdx3 - (g + 3) * omdx2 + 1);
            double cx0 = (g * opdx3 - 5 * g * opdx2 + 8 * g * opdx - 4 * g);
            double cx3 = (g * dmdx3 - 5 * g * dmdx2 + 8 * g * dmdx - 4 * g);

            iy = floor(y);
            yi[0] = iy - 1;
            yi[1] = iy;
            yi[2] = iy + 1;
            yi[3] = iy + 2;
            //make in range
            if (yi[0] < 0) yi[0] = 0; else if (yi[0] > n2 - 1) yi[0] = n2 - 1;
            if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
            if (yi[2] < 0) yi[2] = 0; else if (yi[2] > n2 - 1) yi[2] = n2 - 1;
            if (yi[3] < 0) yi[3] = 0; else if (yi[3] > n2 - 1) yi[3] = n2 - 1;
            dy = (y - yi[1]);
            double dy2 = dy*dy;
            double dy3 = dy2*dy;
            double omdy = 1 - dy;
            double omdy2 = omdy*omdy;
            double omdy3 = omdy2*omdy;
            double opdy = 1 + dy;
            double opdy2 = opdy*opdy;
            double opdy3 = opdy2*opdy;
            double dmdy = 2 - dy;
            double dmdy2 = dmdy*dmdy;
            double dmdy3 = dmdy2*dmdy;
            double cy1 = ((g + 2) * dy3 - (g + 3) * dy2 + 1);
            double cy2 = ((g + 2) * omdy3 - (g + 3) * omdy2 + 1);
            double cy0 = (g * opdy3 - 5 * g * opdy2 + 8 * g * opdy - 4 * g);
            double cy3 = (g * dmdy3 - 5 * g * dmdy2 + 8 * g * dmdy - 4 * g);
            vx0y0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
            vx1y0 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
            vx2y0 = &(array[ncontiguous * (yi[0] * n1 + xi[2])]);
            vx3y0 = &(array[ncontiguous * (yi[0] * n1 + xi[3])]);

            vx0y1 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
            vx1y1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
            vx2y1 = &(array[ncontiguous * (yi[1] * n1 + xi[2])]);
            vx3y1 = &(array[ncontiguous * (yi[1] * n1 + xi[3])]);

            vx0y2 = &(array[ncontiguous * (yi[2] * n1 + xi[0])]);
            vx1y2 = &(array[ncontiguous * (yi[2] * n1 + xi[1])]);
            vx2y2 = &(array[ncontiguous * (yi[2] * n1 + xi[2])]);
            vx3y2 = &(array[ncontiguous * (yi[2] * n1 + xi[3])]);

            vx0y3 = &(array[ncontiguous * (yi[3] * n1 + xi[0])]);
            vx1y3 = &(array[ncontiguous * (yi[3] * n1 + xi[1])]);
            vx2y3 = &(array[ncontiguous * (yi[3] * n1 + xi[2])]);
            vx3y3 = &(array[ncontiguous * (yi[3] * n1 + xi[3])]);
            for (SizeT i = 0; i < ncontiguous; ++i) {
              double r0=cx1*vx1y0[i]+cx2*vx2y0[i]+cx0*vx0y0[i]+cx3*vx3y0[i];
              double r1=cx1*vx1y1[i]+cx2*vx2y1[i]+cx0*vx0y1[i]+cx3*vx3y1[i];
              double r2=cx1*vx1y2[i]+cx2*vx2y2[i]+cx0*vx0y2[i]+cx3*vx3y2[i];
              double r3=cx1*vx1y3[i]+cx2*vx2y3[i]+cx0*vx0y3[i]+cx3*vx3y3[i];
              vres[i] = cy1*r1+cy2*r2+cy0*r0+cy3*r3;
            }
          } else {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          }
        } else {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
      }
    }
    }
  } else { 
#pragma omp parallel private(xi,yi,ix,iy,dx,dy,x,y,vx0y0,vx1y0,vx2y0,vx3y0,vx0y1,vx1y1,vx2y1,vx3y1,vx0y2,vx1y2,vx2y2,vx3y2,vx0y3,vx1y3,vx2y3,vx3y3,vres) if (CpuTPOOL_NTHREADS> 1 && nx*ny >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nx*ny))
    {
#pragma omp for collapse(2)
    for (SizeT k = 0; k < ny; ++k) {
      for (SizeT j = 0; j < nx; ++j) { //nb output points
        vres = &(res[ncontiguous * (k * nx + j) ]);
        x = xx[j]; if (x<0)x=0; if(x>n1-1)x=n1-1;
        ix = floor(x); //floor  ix is [0 .. n1[
        xi[0] = ix - 1;
        xi[1] = ix;
        xi[2] = ix + 1;
        xi[3] = ix + 2;
        //make in range
        if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 -1 ) xi[0] = n1 - 1;
        if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 -1 ) xi[1] = n1 - 1;
        if (xi[2] < 0) xi[2] = 0; else if (xi[2] > n1 -1 ) xi[2] = n1 - 1;
        if (xi[3] < 0) xi[3] = 0; else if (xi[3] > n1 -1 ) xi[3] = n1 - 1;
        dx = (x - xi[1]);
        double dx2 = dx*dx;
        double dx3 = dx2*dx;
        double omdx = 1 - dx;
        double omdx2 = omdx*omdx;
        double omdx3 = omdx2*omdx;
        double opdx = 1 + dx;
        double opdx2 = opdx*opdx;
        double opdx3 = opdx2*opdx;
        double dmdx = 2 - dx;
        double dmdx2 = dmdx*dmdx;
        double dmdx3 = dmdx2*dmdx;
        double cx1 = ((g + 2) * dx3 - (g + 3) * dx2 + 1);
        double cx2 = ((g + 2) * omdx3 - (g + 3) * omdx2 + 1);
        double cx0 = (g * opdx3 - 5 * g * opdx2 + 8 * g * opdx - 4 * g);
        double cx3 = (g * dmdx3 - 5 * g * dmdx2 + 8 * g * dmdx - 4 * g);

        y = yy[k]; if (y<0)y=0; if(y>n2-1)y=n2-1;
        iy = floor(y);
        yi[0] = iy - 1;
        yi[1] = iy;
        yi[2] = iy + 1;
        yi[3] = iy + 2;
        //make in range
        if (yi[0] < 0) yi[0] = 0; else if (yi[0] > n2-1 ) yi[0] = n2 - 1;
        if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2-1 ) yi[1] = n2 - 1;
        if (yi[2] < 0) yi[2] = 0; else if (yi[2] > n2-1 ) yi[2] = n2 - 1;
        if (yi[3] < 0) yi[3] = 0; else if (yi[3] > n2-1 ) yi[3] = n2 - 1;
        dy = (y - yi[1]);
        double dy2 = dy*dy;
        double dy3 = dy2*dy;
        double omdy = 1 - dy;
        double omdy2 = omdy*omdy;
        double omdy3 = omdy2*omdy;
        double opdy = 1 + dy;
        double opdy2 = opdy*opdy;
        double opdy3 = opdy2*opdy;
        double dmdy = 2 - dy;
        double dmdy2 = dmdy*dmdy;
        double dmdy3 = dmdy2*dmdy;
        double cy1 = ((g + 2) * dy3 - (g + 3) * dy2 + 1);
        double cy2 = ((g + 2) * omdy3 - (g + 3) * omdy2 + 1);
        double cy0 = (g * opdy3 - 5 * g * opdy2 + 8 * g * opdy - 4 * g);
        double cy3 = (g * dmdy3 - 5 * g * dmdy2 + 8 * g * dmdy - 4 * g);

        vx0y0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
        vx1y0 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
        vx2y0 = &(array[ncontiguous * (yi[0] * n1 + xi[2])]);
        vx3y0 = &(array[ncontiguous * (yi[0] * n1 + xi[3])]);

        vx0y1 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
        vx1y1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
        vx2y1 = &(array[ncontiguous * (yi[1] * n1 + xi[2])]);
        vx3y1 = &(array[ncontiguous * (yi[1] * n1 + xi[3])]);

        vx0y2 = &(array[ncontiguous * (yi[2] * n1 + xi[0])]);
        vx1y2 = &(array[ncontiguous * (yi[2] * n1 + xi[1])]);
        vx2y2 = &(array[ncontiguous * (yi[2] * n1 + xi[2])]);
        vx3y2 = &(array[ncontiguous * (yi[2] * n1 + xi[3])]);

        vx0y3 = &(array[ncontiguous * (yi[3] * n1 + xi[0])]);
        vx1y3 = &(array[ncontiguous * (yi[3] * n1 + xi[1])]);
        vx2y3 = &(array[ncontiguous * (yi[3] * n1 + xi[2])]);
        vx3y3 = &(array[ncontiguous * (yi[3] * n1 + xi[3])]);

        for (SizeT i = 0; i < ncontiguous; ++i) {
          double r0 = cx1 * vx1y0[i] + cx2 * vx2y0[i] + cx0 * vx0y0[i] + cx3 * vx3y0[i];
          double r1 = cx1 * vx1y1[i] + cx2 * vx2y1[i] + cx0 * vx0y1[i] + cx3 * vx3y1[i];
          double r2 = cx1 * vx1y2[i] + cx2 * vx2y2[i] + cx0 * vx0y2[i] + cx3 * vx3y2[i];
          double r3 = cx1 * vx1y3[i] + cx2 * vx2y3[i] + cx0 * vx0y3[i] + cx3 * vx3y3[i];
          vres[i] = cy1 * r1 + cy2 * r2 + cy0 * r0 + cy3*r3;
        }
      }
    }
  }
  }
}

template <typename T1, typename T2>
void interpolate_3d_linear(T1* array, SizeT un1,  SizeT un2, SizeT un3, T2* xx, SizeT n, T2* yy, T2* zz, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *vx0y0z0,*vx1y0z0, *vx0y1z0,*vx1y1z0, *vx0y0z1,*vx1y0z1, *vx0y1z1,*vx1y1z1, *vres;
  double dx, dy, dz; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double umdx, umdy, umdz;
  double x, y, z;
  ssize_t ix = 0;
  ssize_t iy = 0; //operations on unsigned are not what you think, signed are ok
  ssize_t iz = 0; 
  ssize_t xi[2], yi[2], zi[2]; //operations on unsigned are not what you think, signed are ok
  ssize_t n1 = un1;
  ssize_t n2 = un2;
  ssize_t n3 = un3;
  ssize_t n1n2=n1*n2;
  if (use_missing) { 
#pragma omp parallel private(xi,yi,zi,ix,iy,iz,dx,dy,dz,x,y,z,vx0y0z0,vx1y0z0,vx0y1z0,vx1y1z0,vx0y0z1,vx1y0z1,vx0y1z1,vx1y1z1,vres) if (CpuTPOOL_NTHREADS> 1 && n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
    {
#pragma omp for
      for (SizeT j = 0; j < n; ++j) { //nb output points
        vres = &(res[ncontiguous * j ]);
        x = xx[j];
        if (x < 0) {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        } else if (x <= n1 - 1) {
          y = yy[j];
          if (y < 0) {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          } else if (y <= n2 - 1) {
            z = zz[j];
            if (z < 0) {
              for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
            } else if (z <= n3 - 1) {
              ix = floor(x);
              xi[0] = ix;
              xi[1] = ix + 1;
              //make in range
              if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
              dx = (x - xi[0]); umdx=1-dx;
              
              iy = floor(y);
              yi[0] = iy;
              yi[1] = iy + 1;
              //make in range
              if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
              dy = (y - yi[0]); umdy=1-dy;
                
              iz = floor(z);
              zi[0] = iz;
              zi[1] = iz + 1;
              //make in range
              if (zi[1] < 0) zi[1] = 0; else if (zi[1] > n3 - 1) zi[1] = n3 - 1;
              dz = (z - zi[0]); umdz=1-dz;

              vx0y0z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[0] * n1 + xi[0])] );
              vx1y0z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[0] * n1 + xi[1])] );
              vx0y1z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[1] * n1 + xi[0])] );
              vx1y1z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[1] * n1 + xi[1])] );
              vx0y0z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[0] * n1 + xi[0])] );
              vx1y0z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[0] * n1 + xi[1])] );
              vx0y1z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[1] * n1 + xi[0])] );
              vx1y1z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[1] * n1 + xi[1])] );
              for (SizeT i = 0; i < ncontiguous; ++i) {
                double y0z0=umdx*vx0y0z0[i]+dx*vx1y0z0[i];
                double y1z0=umdx*vx0y1z0[i]+dx*vx1y1z0[i];
                double y0z1=umdx*vx0y0z1[i]+dx*vx1y0z1[i];
                double y1z1=umdx*vx0y1z1[i]+dx*vx1y1z1[i];
                double   z0=umdy*y0z0+dy*y1z0;
                double   z1=umdy*y0z1+dy*y1z1;
                vres[i] = umdz*z0+dz*z1; 
              }
            } else {
              for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
            }
          } else {
            for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
          }
        } else {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
      }
    }
  } else {
#pragma omp parallel private(xi,yi,zi,ix,iy,iz,dx,dy,dz,x,y,z,vx0y0z0,vx1y0z0,vx0y1z0,vx1y1z0,vx0y0z1,vx1y0z1,vx0y1z1,vx1y1z1,vres) if (CpuTPOOL_NTHREADS> 1 && n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
    {
#pragma omp for
      for (SizeT j = 0; j < n; ++j) { //nb output points
        vres = &(res[ncontiguous * j ]);
        x = xx[j]; if (x<0)x=0; if(x>n1-1)x=n1-1;
        y = yy[j]; if (y<0)y=0; if(y>n2-1)y=n2-1;
        z = zz[j]; if (z<0)z=0; if(z>n3-1)z=n3-1;

        ix = floor(x); 
        xi[0] = ix;
        xi[1] = ix + 1;
        //make in range
        if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
        dx = (x - xi[0]); umdx=1-dx;

        iy = floor(y);
        yi[0] = iy;
        yi[1] = iy + 1;
        //make in range
        if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
        dy = (y - yi[0]); umdy=1-dy;

        iz = floor(z);
        zi[0] = iz;
        zi[1] = iz + 1;
        //make in range
        if (zi[1] < 0) zi[1] = 0; else if (zi[1] > n3 - 1) zi[1] = n3 - 1;
        dz = (z - zi[0]); umdz=1-dz;

        vx0y0z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[0] * n1 + xi[0])] );
        vx1y0z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[0] * n1 + xi[1])] );
        vx0y1z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[1] * n1 + xi[0])] );
        vx1y1z0 = &( array[ncontiguous * ( zi[0] * n1n2 + yi[1] * n1 + xi[1])] );
        vx0y0z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[0] * n1 + xi[0])] );
        vx1y0z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[0] * n1 + xi[1])] );
        vx0y1z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[1] * n1 + xi[0])] );
        vx1y1z1 = &( array[ncontiguous * ( zi[1] * n1n2 + yi[1] * n1 + xi[1])] );
        for (SizeT i = 0; i < ncontiguous; ++i) {
          double y0z0=umdx*vx0y0z0[i]+dx*vx1y0z0[i];
          double y1z0=umdx*vx0y1z0[i]+dx*vx1y1z0[i];
          double y0z1=umdx*vx0y0z1[i]+dx*vx1y0z1[i];
          double y1z1=umdx*vx0y1z1[i]+dx*vx1y1z1[i];
          double   z0=umdy*y0z0+dy*y1z0;
          double   z1=umdy*y0z1+dy*y1z1;
          vres[i] = umdz*z0+dz*z1; 
        }
      }
    }
  }
}

namespace lib {

  BaseGDL* interpolate_fun(EnvT* e) {

    SizeT nParam = e->NParam();
    if (nParam < 2) e->Throw("Incorrect number of arguments.");

    // options
    static int cubicIx = e->KeywordIx("CUBIC");
    bool cubic = e->KeywordSet(cubicIx);
    DDouble gamma = -1.0;
    e->AssureDoubleScalarKWIfPresent(cubicIx, gamma);
    gdl_update_cubic_interpolation_coeff(gamma);

    static int nnborIx = e->KeywordIx("NEAREST_NEIGHBOUR"); //usage restricted to GDL, undocumented, normally for CONGRID.
    bool nnbor = e->KeywordSet(nnborIx);
    if (nnbor && cubic) nnbor = false; //undocumented nearest neighbour give way wrt. other options.

    static int gridIx = e->KeywordIx("GRID");
    bool grid = e->KeywordSet(gridIx);

    static int dblIx = e->KeywordIx("DOUBLE"); //only with GRID.
    bool dbl = e->KeywordSet(dblIx);

    static int missingIx = e->KeywordIx("MISSING");
    bool use_missing = e->KeywordPresent(missingIx);
    DDouble missing = 0;
    if (use_missing) e->AssureDoubleScalarKWIfPresent(missingIx, missing);

    // convert to internal double arrays. Special case for complex values, we separate R and I
    BaseGDL* p0 = e->GetParDefined(0);
    if (p0->Rank() < nParam - 1)
      e->Throw("Number of parameters must agree with dimensions of argument.");


    // 1D Interpolation
    if (nParam == 2) {
      BaseGDL* p1 = e->GetParDefined(1);

      SizeT nx = p1->N_Elements();

      if (p0->Rank() < 1) e->Throw("Number of parameters must agree with dimensions of argument.");
      dimension d0=p0->Dim();
      d0.Purge(); //remove last dims equal to 1 if any
      dimension d1=p1->Dim();
      d1.Purge(); //remove last dims equal to 1 if any
      SizeT rankLeft = d0.Rank() - 1;

      //initialize output array with correct dimensions, remove last dimensions 1 if any.
      SizeT outRank = rankLeft;
      DLong outdims[MAXRANK];
      SizeT i = 0;
      for (; i < outRank; ++i) outdims[i] = d0[i];
      for (; i < MAXRANK; ++i) outdims[i] = 0;
      for (SizeT i = 0; i < d1.Rank(); ++i) {
        outdims[outRank++] = d1[i];
        if (outRank > MAXRANK)
          e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
      }
      
      dimension outdim((DLong *) outdims, outRank);

      // Determine number of (contiguous) points in remaining dimensions
      SizeT ncontiguous = 1;
      for (SizeT i = 0; i < rankLeft; ++i) ncontiguous *= d0[i];
      
      SizeT un1=p0->Dim(d0.Rank()-1);
      if (un1<3 && cubic) cubic=false;
      if (un1<2) nnbor=true;
      
      //  std::cerr<<"rankLeft="<<rankLeft<<",ninterp="<<ninterp<<", nx="<<nx<<std::endl;

      //res dimension is [<----rankleft--->,nx]
      //sizes:           [   niterp values    ,nx]
      //linear: for each x, get ncontiguous values v0[,,,] at &array[,,,ix=floor(x)] , ncont.. values v1[,,,] at &array[,,,ix+1], with u=(x-ix), result is linConv(u,v0,v1);
      if (ncontiguous == 1) { //optimisation for this particular case: twice the speed.
        switch (p0->Type()) {
        case GDL_FLOAT:
          if (dbl) CALL_INTERPOLATE_1D_SINGLE(DFloatGDL, DFloat, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_1D_SINGLE(DFloatGDL, DFloat, DFloatGDL, DFloat)
        case GDL_DOUBLE:
          if (dbl) CALL_INTERPOLATE_1D_SINGLE(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_1D_SINGLE(DDoubleGDL, DDouble, DFloatGDL, DFloat)
        case GDL_LONG:
          CALL_INTERPOLATE_1D_SINGLE(DLongGDL, DLong, DDoubleGDL, DDouble)
        case GDL_BYTE:
          CALL_INTERPOLATE_1D_SINGLE(DByteGDL, DByte, DFloatGDL, DFloat)
        case GDL_INT:
          CALL_INTERPOLATE_1D_SINGLE(DIntGDL, DInt, DFloatGDL, DFloat)
        case GDL_COMPLEX:
          // A complex is just a double array with 1 dimension more and first dim is 2. IT IS NOT 'SIMPLE'
          CALL_INTERPOLATE_1D_COMPLEX(DComplexGDL, DFloat, DFloatGDL, DFloat) //Complex as a series of Floats
        case GDL_COMPLEXDBL:
          // A complex is just a double array with 1 dimension more and first dim is 2. IT IS NOT 'SIMPLE'
          CALL_INTERPOLATE_1D_COMPLEX(DComplexDblGDL, DDouble, DDoubleGDL, DDouble) //ComplexDbl as a serie of Doubles
        case GDL_UINT:
          CALL_INTERPOLATE_1D_SINGLE(DUIntGDL, DUInt, DFloatGDL, DFloat)
        case GDL_ULONG:
          CALL_INTERPOLATE_1D_SINGLE(DULongGDL, DULong, DDoubleGDL, DDouble)
        case GDL_LONG64:
          CALL_INTERPOLATE_1D_SINGLE(DLong64GDL, DLong64, DDoubleGDL, DDouble)
        case GDL_ULONG64:
          CALL_INTERPOLATE_1D_SINGLE(DULong64GDL, DULong64, DDoubleGDL, DDouble)
        default:
          //  case GDL_STRING:
          //  case GDL_PTR:
          //  case GDL_OBJ:
          //  case GDL_STRUCT:
          //  case GDL_UNDEF:
          throw GDLException(p0->TypeStr() + " expression not allowed in this context: " + e->GetParString(0));
        }
      } else {
        switch (p0->Type()) {
        case GDL_FLOAT:
          if (dbl) CALL_INTERPOLATE_1D(DFloatGDL, DFloat, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_1D(DFloatGDL, DFloat, DFloatGDL, DFloat)
        case GDL_DOUBLE:
          if (dbl) CALL_INTERPOLATE_1D(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_1D(DDoubleGDL, DDouble, DFloatGDL, DFloat)
        case GDL_LONG:
          CALL_INTERPOLATE_1D(DLongGDL, DLong, DDoubleGDL, DDouble)
        case GDL_BYTE:
          CALL_INTERPOLATE_1D(DByteGDL, DByte, DFloatGDL, DFloat)
        case GDL_INT:
          CALL_INTERPOLATE_1D(DIntGDL, DInt, DFloatGDL, DFloat)
        case GDL_COMPLEX:
          // A complex is just a double array with 1 dimension more and first dim is 2.
          CALL_INTERPOLATE_1D_COMPLEX(DComplexGDL, DFloat, DFloatGDL, DFloat) //Complex as a series of Floats
        case GDL_COMPLEXDBL:
          // A complex is just a double array with 1 dimension more and first dim is 2.
          CALL_INTERPOLATE_1D_COMPLEX(DComplexDblGDL, DDouble, DDoubleGDL, DDouble) //ComplexDbl as a serie of Doubles
        case GDL_UINT:
          CALL_INTERPOLATE_1D(DUIntGDL, DUInt, DFloatGDL, DFloat)
        case GDL_ULONG:
          CALL_INTERPOLATE_1D(DULongGDL, DULong, DDoubleGDL, DDouble)
        case GDL_LONG64:
          CALL_INTERPOLATE_1D(DLong64GDL, DLong64, DDoubleGDL, DDouble)
        case GDL_ULONG64:
          CALL_INTERPOLATE_1D(DULong64GDL, DULong64, DDoubleGDL, DDouble)
        default:
          //  case GDL_STRING:
          //  case GDL_PTR:
          //  case GDL_OBJ:
          //  case GDL_STRUCT:
          //  case GDL_UNDEF:
          throw GDLException(p0->TypeStr() + " expression not allowed in this context: " + e->GetParString(0));
        }
      }
    } 
     else if (nParam == 3) {
      //// the interpolant type used depends on the number of bytes of p0.    
      BaseGDL* p1 = e->GetParDefined(1);
      BaseGDL* p2 = e->GetParDefined(2);

      SizeT nx = p1->N_Elements();
      SizeT ny = p2->N_Elements();

      if (nx == 1 && ny == 1) grid = false;

      dimension d0=p0->Dim();
      d0.Purge(); //remove last dims equal to 1 if any
      if (d0.Rank() < 2) e->Throw("Number of parameters must agree with dimensions of argument.");
      dimension d1=p1->Dim();
      d1.Purge(); //remove last dims equal to 1 if any
      dimension d2=p2->Dim();
      d2.Purge(); //remove last dims equal to 1 if any
      SizeT rankLeft = d0.Rank() - 2;

      // If not GRID then check that length match, the rank will be the rank of d1.
      if (!grid) {
        if (d1.NDimElements() != d2.NDimElements())
          e->Throw("Coordinate arrays must have same length if Grid not set.");
      }

      //initialize output array with correct dimensions
      SizeT outRank = rankLeft;

      DLong dims[MAXRANK];
      SizeT i = 0;
      for (; i < outRank; ++i) dims[i] = d0[i];
      for (; i < MAXRANK; ++i) dims[i] = 0;

      if (grid) {
        dims[outRank++] = nx;
        if (outRank > MAXRANK - 1)
          e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
        dims[outRank++] = ny;
      } else {
        for (SizeT i = 0; i < d1.Rank(); ++i) {
          dims[outRank++] = d1[i];
          if (outRank > MAXRANK)
            e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
        }
      }
      
      dimension outdim((DLong *) dims, outRank);

      // Determine number of interpolations for remaining dimensions
      SizeT ncontiguous = 1;
      for (SizeT i = 0; i < rankLeft; ++i) ncontiguous *= d0[i];

      SizeT un1 = d0[d0.Rank() - 2];
      if (un1 < 3 && cubic) cubic = false;
      if (un1 < 2) nnbor = true;
      SizeT un2 = d0[d0.Rank() - 1];

      if (grid) {
        switch (p0->Type()) {
        case GDL_FLOAT:
          if (dbl) CALL_INTERPOLATE_2D_GRID(DFloatGDL, DFloat, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_2D_GRID(DFloatGDL, DFloat, DFloatGDL, DFloat)
        case GDL_DOUBLE:
          if (dbl) CALL_INTERPOLATE_2D_GRID(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_2D_GRID(DDoubleGDL, DDouble, DFloatGDL, DFloat)
        case GDL_LONG:
          CALL_INTERPOLATE_2D_GRID(DLongGDL, DLong, DDoubleGDL, DDouble)
        case GDL_BYTE:
          CALL_INTERPOLATE_2D_GRID(DByteGDL, DByte, DFloatGDL, DFloat)
        case GDL_INT:
          CALL_INTERPOLATE_2D_GRID(DIntGDL, DInt, DFloatGDL, DFloat)
        case GDL_COMPLEX:
          // A complex is just a double array with 1 dimension more and first dim is 2.
          CALL_INTERPOLATE_2D_GRID_COMPLEX(DComplexGDL, DFloat, DFloatGDL, DFloat) //Complex as a series of Floats
        case GDL_COMPLEXDBL:
          // A complex is just a double array with 1 dimension more and first dim is 2.
          CALL_INTERPOLATE_2D_GRID_COMPLEX(DComplexDblGDL, DDouble, DDoubleGDL, DDouble) //ComplexDbl as a serie of Doubles
        case GDL_UINT:
          CALL_INTERPOLATE_2D_GRID(DUIntGDL, DUInt, DFloatGDL, DFloat)
        case GDL_ULONG:
          CALL_INTERPOLATE_2D_GRID(DULongGDL, DULong, DDoubleGDL, DDouble)
        case GDL_LONG64:
          CALL_INTERPOLATE_2D_GRID(DLong64GDL, DLong64, DDoubleGDL, DDouble)
        case GDL_ULONG64:
          CALL_INTERPOLATE_2D_GRID(DULong64GDL, DULong64, DDoubleGDL, DDouble)
        default:
          //  case GDL_STRING:
          //  case GDL_PTR:
          //  case GDL_OBJ:
          //  case GDL_STRUCT:
          //  case GDL_UNDEF:
          throw GDLException(p0->TypeStr() + " expression not allowed in this context: " + e->GetParString(0));
        }
      } else {
        switch (p0->Type()) {
        case GDL_FLOAT:
          if (dbl) CALL_INTERPOLATE_2D(DFloatGDL, DFloat, DDoubleGDL, DDouble)
            else CALL_INTERPOLATE_2D(DFloatGDL, DFloat, DFloatGDL, DFloat)
        case GDL_DOUBLE:
          if (dbl) CALL_INTERPOLATE_2D(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
            else CALL_INTERPOLATE_2D(DDoubleGDL, DDouble, DFloatGDL, DFloat) 
        case GDL_LONG:
          CALL_INTERPOLATE_2D(DLongGDL, DLong, DDoubleGDL, DDouble)
        case GDL_BYTE:
          CALL_INTERPOLATE_2D(DByteGDL, DByte, DFloatGDL, DFloat)
        case GDL_INT:
          CALL_INTERPOLATE_2D(DIntGDL, DInt, DFloatGDL, DFloat)
        case GDL_COMPLEX:
          // A complex is just a double array with 1 dimension more and first dim is 2.
          CALL_INTERPOLATE_2D_COMPLEX(DComplexGDL, DFloat, DFloatGDL, DFloat) //Complex as a series of Floats
        case GDL_COMPLEXDBL:
          // A complex is just a double array with 1 dimension more and first dim is 2.
          CALL_INTERPOLATE_2D_COMPLEX(DComplexDblGDL, DDouble, DDoubleGDL, DDouble) //ComplexDbl as a serie of Doubles
        case GDL_UINT:
          CALL_INTERPOLATE_2D(DUIntGDL, DUInt, DFloatGDL, DFloat)
        case GDL_ULONG:
          CALL_INTERPOLATE_2D(DULongGDL, DULong, DDoubleGDL, DDouble)
        case GDL_LONG64:
          CALL_INTERPOLATE_2D(DLong64GDL, DLong64, DDoubleGDL, DDouble)
        case GDL_ULONG64:
          CALL_INTERPOLATE_2D(DULong64GDL, DULong64, DDoubleGDL, DDouble)
        default:
          //  case GDL_STRING:
          //  case GDL_PTR:
          //  case GDL_OBJ:
          //  case GDL_STRUCT:
          //  case GDL_UNDEF:
          throw GDLException(p0->TypeStr() + " expression not allowed in this context: " + e->GetParString(0));
        }
      }
    } else if (nParam == 4) {
      //// the interpolant type used depends on the number of bytes of p0.    
      BaseGDL* p1 = e->GetParDefined(1);
      BaseGDL* p2 = e->GetParDefined(2);
      BaseGDL* p3 = e->GetParDefined(3);

      SizeT nx = p1->N_Elements();
      SizeT ny = p2->N_Elements();
      SizeT nz = p3->N_Elements();

      if (nx == 1 && ny == 1 && nz == 1) grid = false;

      dimension d0 = p0->Dim();
      d0.Purge(); //remove last dims equal to 1 if any
      if (d0.Rank() < 3) e->Throw("Number of parameters must agree with dimensions of argument.");
      dimension d1 = p1->Dim();
      d1.Purge(); //remove last dims equal to 1 if any
      dimension d2 = p2->Dim();
      d2.Purge(); //remove last dims equal to 1 if any
      dimension d3 = p3->Dim();
      d3.Purge(); //remove last dims equal to 1 if any
      SizeT rankLeft = d0.Rank() - 3;

      // If not GRID then check that length match, the rank will be the rank of d1.
      if (!grid) {
        if (d1.NDimElements() != d2.NDimElements() || d1.NDimElements() != d3.NDimElements() || d2.NDimElements() != d3.NDimElements() )
          e->Throw("Coordinate arrays must have same length if Grid not set.");
      }

      //initialize output array with correct dimensions
      SizeT outRank = rankLeft;

      DLong dims[MAXRANK];
      SizeT i = 0;
      for (; i < outRank; ++i) dims[i] = d0[i];
      for (; i < MAXRANK; ++i) dims[i] = 0;

      if (grid) {
        dims[outRank++] = nx;
        if (outRank > MAXRANK - 1)
          e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
        dims[outRank++] = ny;
        if (outRank > MAXRANK - 1)
          e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
        dims[outRank++] = nz;
        if (outRank > MAXRANK - 1)
          e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
      } else {
        for (SizeT i = 0; i < d1.Rank(); ++i) {
          dims[outRank++] = d1[i];
          if (outRank > MAXRANK)
            e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
        }
      }

      dimension outdim((DLong *) dims, outRank);

      // Determine number of interpolations for remaining dimensions
      SizeT ncontiguous = 1;
      for (SizeT i = 0; i < rankLeft; ++i) ncontiguous *= d0[i];

      SizeT un1 = d0[d0.Rank() - 3];
      if (un1 < 3 && cubic) cubic = false;
      if (un1 < 2) nnbor = true;
      SizeT un2 = d0[d0.Rank() - 2];
      SizeT un3 = d0[d0.Rank() - 1];

//      if (grid) {
//        switch (p0->Type()) {
//        case GDL_FLOAT:
//          if (dbl) CALL_INTERPOLATE_2D_GRID(DFloatGDL, DFloat, DDoubleGDL, DDouble)
//          else CALL_INTERPOLATE_2D_GRID(DFloatGDL, DFloat, DFloatGDL, DFloat)
//          case GDL_DOUBLE:
//            if (dbl) CALL_INTERPOLATE_2D_GRID(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
//          else CALL_INTERPOLATE_2D_GRID(DDoubleGDL, DDouble, DFloatGDL, DFloat)
//          case GDL_LONG:
//            CALL_INTERPOLATE_2D_GRID(DLongGDL, DLong, DDoubleGDL, DDouble)
//          case GDL_BYTE:
//            CALL_INTERPOLATE_2D_GRID(DByteGDL, DByte, DFloatGDL, DFloat)
//          case GDL_INT:
//            CALL_INTERPOLATE_2D_GRID(DIntGDL, DInt, DFloatGDL, DFloat)
//          case GDL_COMPLEX:
//            // A complex is just a double array with 1 dimension more and first dim is 2.
//            CALL_INTERPOLATE_2D_GRID_COMPLEX(DComplexGDL, DFloat, DFloatGDL, DFloat) //Complex as a series of Floats
//          case GDL_COMPLEXDBL:
//            // A complex is just a double array with 1 dimension more and first dim is 2.
//            CALL_INTERPOLATE_2D_GRID_COMPLEX(DComplexDblGDL, DDouble, DDoubleGDL, DDouble) //ComplexDbl as a serie of Doubles
//          case GDL_UINT:
//            CALL_INTERPOLATE_2D_GRID(DUIntGDL, DUInt, DFloatGDL, DFloat)
//          case GDL_ULONG:
//            CALL_INTERPOLATE_2D_GRID(DULongGDL, DULong, DDoubleGDL, DDouble)
//          case GDL_LONG64:
//            CALL_INTERPOLATE_2D_GRID(DLong64GDL, DLong64, DDoubleGDL, DDouble)
//          case GDL_ULONG64:
//            CALL_INTERPOLATE_2D_GRID(DULong64GDL, DULong64, DDoubleGDL, DDouble)
//          default:
//            //  case GDL_STRING:
//            //  case GDL_PTR:
//            //  case GDL_OBJ:
//            //  case GDL_STRUCT:
//            //  case GDL_UNDEF:
//            throw GDLException(p0->TypeStr() + " expression not allowed in this context: " + e->GetParString(0));
//        }
//      } else {
        switch (p0->Type()) {
        case GDL_FLOAT:
          if (dbl) CALL_INTERPOLATE_3D(DFloatGDL, DFloat, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_3D(DFloatGDL, DFloat, DFloatGDL, DFloat)
          case GDL_DOUBLE:
            if (dbl) CALL_INTERPOLATE_3D(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
          else CALL_INTERPOLATE_3D(DDoubleGDL, DDouble, DFloatGDL, DFloat)
          case GDL_LONG:
            CALL_INTERPOLATE_3D(DLongGDL, DLong, DDoubleGDL, DDouble)
          case GDL_BYTE:
            CALL_INTERPOLATE_3D(DByteGDL, DByte, DFloatGDL, DFloat)
          case GDL_INT:
            CALL_INTERPOLATE_3D(DIntGDL, DInt, DFloatGDL, DFloat)
//          case GDL_COMPLEX:
//            // A complex is just a double array with 1 dimension more and first dim is 2.
//            CALL_INTERPOLATE_3D_COMPLEX(DComplexGDL, DFloat, DFloatGDL, DFloat) //Complex as a series of Floats
//          case GDL_COMPLEXDBL:
//            // A complex is just a double array with 1 dimension more and first dim is 2.
//            CALL_INTERPOLATE_3D_COMPLEX(DComplexDblGDL, DDouble, DDoubleGDL, DDouble) //ComplexDbl as a serie of Doubles
          case GDL_UINT:
            CALL_INTERPOLATE_3D(DUIntGDL, DUInt, DFloatGDL, DFloat)
          case GDL_ULONG:
            CALL_INTERPOLATE_3D(DULongGDL, DULong, DDoubleGDL, DDouble)
          case GDL_LONG64:
            CALL_INTERPOLATE_3D(DLong64GDL, DLong64, DDoubleGDL, DDouble)
          case GDL_ULONG64:
            CALL_INTERPOLATE_3D(DULong64GDL, DULong64, DDoubleGDL, DDouble)
          default:
            //  case GDL_STRING:
            //  case GDL_PTR:
            //  case GDL_OBJ:
            //  case GDL_STRUCT:
            //  case GDL_UNDEF:
            throw GDLException(p0->TypeStr() + " expression not allowed in this context: " + e->GetParString(0));
        }
//      }
    }
    throw;
  }
}
