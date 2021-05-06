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
  G1* res = new G1(dim, BaseGDL::NOZERO);\
  if (cubic)  interpolate_1d_cubic<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else if (nnbor) interpolate_1d_nearest<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous);\
  else  interpolate_1d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_1D_SINGLE(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G1* res = new G1(dim, BaseGDL::NOZERO);\
  if (cubic)  interpolate_1d_cubic_single<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()), use_missing, missing);\
  else if (nnbor) interpolate_1d_nearest_single<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()));\
  else  interpolate_1d_linear_single<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()), use_missing, missing);\
  return res;\
  break;\
}
#define CALL_INTERPOLATE_1D_COMPLEX(G1,T1,G2,T2)\
{\
  G2* p1D=e->GetParAs<G2>(1);\
  G1* res = new G1(dim, BaseGDL::NOZERO);\
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
  G1* res = new G1(dim, BaseGDL::NOZERO);\
  /* if (cubic)  interpolate_1d_cubic<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
  else if (nnbor) interpolate_1d_nearest<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T1*>(res->DataAddr()),ncontiguous);\
  else  */\
  interpolate_2d_linear<T1,T2>(static_cast<T1*>(p0->DataAddr()), un1, un2, static_cast<T2*>(p1D->DataAddr()),nx,static_cast<T2*>(p2D->DataAddr()),ny,static_cast<T1*>(res->DataAddr()),ncontiguous, use_missing, missing);\
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
  double d;
  T2 x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t n1 = un1;
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

template <typename T1, typename T2> //NEAREST_NEIGHBOUR is NOT STANDARD and is USED only by CONGRID. There is no "missing" possibility.
void interpolate_1d_nearest_single(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res) {
  double d;
  T2 x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t n1 = un1;
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

template <typename T1, typename T2>
void interpolate_1d_linear(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {

  T1 *v0, *v1, *vres;
  double d; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[2];
  ssize_t n1 = un1;
  if (use_missing) {
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
        d = (x - xi[0]);
        v0 = &(array[ncontiguous * xi[0]]);
        v1 = &(array[ncontiguous * xi[1]]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = (1. - d) * v0[i] + d * v1[i];
        }
      } else {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      }
    }
  } else {
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
        d = (x - xi[0]);
        v0 = &(array[ncontiguous * xi[0]]);
        v1 = &(array[ncontiguous * xi[1]]);
        for (SizeT i = 0; i < ncontiguous; ++i) {
          vres[i] = (1. - d) * v0[i] + d * v1[i];
        }
      } else {
        v0 = &(array[ncontiguous * (n1 - 1)]);
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = v0[i];
      }
    }
  }
}

template <typename T1, typename T2>
void interpolate_1d_linear_single(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, bool use_missing, DDouble missing) {

  double d; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[2];
  ssize_t n1 = un1;
  if (use_missing) {
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
        d = (x - xi[0]);
        res[j] = (1. - d) * array[xi[0]] + d * array[xi[1]];
      } else {
        res[j] = missing;
      }
    }
  } else {
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
        d = (x - xi[0]);
        res[j] = (1. - d) * array[xi[0]] + d * array[xi[1]];
      } else {
        res[j] = array[n1-1];
      }
    }
  }
}


template <typename T1, typename T2>
void interpolate_1d_cubic(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {
  T1 *v0, *v1, *v2, *v3, *vres;
  double d;
  double g = gdl_cubic_gamma;
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[4];
  ssize_t n1 = un1;
  if (use_missing) {
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
        d = (x - xi[1]);
        double d2 = d*d;
        double d3 = d2*d;
        double omd = 1 - d;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + d;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - d;
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
  } else {
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
        d = (x - xi[1]);
        double d2 = d*d;
        double d3 = d2*d;
        double omd = 1 - d;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + d;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - d;
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

template <typename T1, typename T2>
void interpolate_1d_cubic_single(T1* array, SizeT un1, T2* xx, SizeT nx, T1* res, bool use_missing, DDouble missing) {
  double d;
  double g = gdl_cubic_gamma;
  double x;
  //operations on unsigned are not what you think, signed are ok
  ssize_t ix = 0;
  ssize_t xi[4];
  ssize_t n1 = un1;
  if (use_missing) {
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
        d = (x - xi[1]);
        double d2 = d*d;
        double d3 = d2*d;
        double omd = 1 - d;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + d;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - d;
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
  } else {
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      x = xx[j];
      if (x < 0) {
        res[j] = array[0];
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
        d = (x - xi[1]);
        double d2 = d*d;
        double d3 = d2*d;
        double omd = 1 - d;
        double omd2 = omd*omd;
        double omd3 = omd2*omd;
        double opd = 1 + d;
        double opd2 = opd*opd;
        double opd3 = opd2*opd;
        double dmd = 2 - d;
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

template <typename T1, typename T2>
void interpolate_2d_linear(T1* array, SizeT un1,  SizeT un2, T2* xx, SizeT nx, T2* yy, SizeT ny, T1* res, SizeT ncontiguous, bool use_missing, DDouble missing) {

  T1 *vx0, *vx1, *vy0, *vy1, *vres;
  double dx, dy; //"In either case, the actual interpolation is always done using double-precision arithmetic."
  double x, y;
  ssize_t ix=0;
  ssize_t iy=0;  //operations on unsigned are not what you think, signed are ok
  ssize_t xi[2], yi[2];  //operations on unsigned are not what you think, signed are ok
  ssize_t n1 = un1;
  ssize_t n2 = un2;
  if (use_missing) {
    for (SizeT j = 0; j < nx; ++j) { //nb output points
      vres = &(res[ncontiguous * j]);
      x = xx[j];
      if (x < 0) {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      } else if (x < n1) {
        y = yy[j];
        if (y < 0) {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
        else if (y < n2 ) {
          ix = floor(x);
          xi[0] = ix;
          xi[1] = ix + 1;
          //make in range
          if (xi[0] < 0) xi[0] = 0; else if (xi[0] > n1 - 1) xi[0] = n1 - 1;
          if (xi[1] < 0) xi[1] = 0; else if (xi[1] > n1 - 1) xi[1] = n1 - 1;
          dx = (x - xi[0]);
          iy = floor(y);
          yi[0] = iy;
          yi[1] = iy + 1;
          //make in range
          if (yi[0] < 0) yi[0] = 0; else if (yi[0] > n2 - 1) yi[0] = n2 - 1;
          if (yi[1] < 0) yi[1] = 0; else if (yi[1] > n2 - 1) yi[1] = n2 - 1;
          dy = (y - yi[0]);
          vx0 = &(array[ncontiguous * (yi[0] * n1 + xi[0])]);
          vx1 = &(array[ncontiguous * (yi[0] * n1 + xi[1])]);
          vy0 = &(array[ncontiguous * (yi[1] * n1 + xi[0])]);
          vy1 = &(array[ncontiguous * (yi[1] * n1 + xi[1])]);
          for (SizeT i = 0; i < ncontiguous; ++i) {
            T1 x12 = (1. - dx) * vx0[i] + dx * vx1[i];
            T1 x34 = (1. - dx) * vy0[i] + dx * vy1[i];
            vres[i] = (1. - dy) * x12 + dy * x34;
          }
        } else {
          for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
        }
      } else {
        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = missing;
      }
    }
  } else {
//    for (SizeT j = 0; j < nx; ++j) {
//      vres = &(res[ncontiguous * j]);
//      x = xx[j];
//      if (x < 0) {
//        vx0 = &(array[0]);
//        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = vx0[i];
//      } else if (x < n1 - 1) {
//        ix = floor(x); //floor  ix is [0 .. n1[
//        d = (x - ix);
//        vx0 = &(array[ncontiguous * ix]);
//        vx1 = &(array[ncontiguous * (ix + 1)]);
//        for (SizeT i = 0; i < ncontiguous; ++i) {
//          vres[i] = (1. - d) * vx0[i] + d * vx1[i];
//        }
//      } else {
//        vx0 = &(array[ncontiguous * (n1 - 1)]);
//        for (SizeT i = 0; i < ncontiguous; ++i) vres[i] = vx0[i];
//      }
//    }
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

    //Ok, Ranks are compatible, but check if last parameter(s) have dimension 1.
    //interpol is unable to interpolate on a zero-size dimension. We will downgrade the number
    //of dimensions to be interpolated, and duplicate the result to the size of the last parameter(s).
    SizeT lastDim = 0;
    bool needsLastDims = false;
    dimension dimIni = p0->Dim();
    dimension dimEnd = p0->Dim();
    dimEnd.Purge();
    SizeT RankDiff = dimIni.Rank() - dimEnd.Rank(); //number of 1-sized dimensions consecutive at end
    SizeT resDimInit[ MAXRANK];
    SizeT iAddRank = 0;
    for (iAddRank = 0; iAddRank < dimEnd.Rank(); ++iAddRank) resDimInit[iAddRank] = dimEnd[iAddRank];
    //All of the above not useful if grid is not set!
    if (grid && RankDiff > 0) {
      for (SizeT i = RankDiff; i >= 1; --i) {
        lastDim = (e->GetParDefined(nParam - i))->Dim(0);
        if (lastDim > 1) needsLastDims = true; //correct behaviour: last dim=1 is trimmed anyway.
        resDimInit[iAddRank] = (lastDim == 0) ? 1 : lastDim;
        iAddRank++;
      }
      nParam -= RankDiff;
    }


    // 1D Interpolation
    if (nParam == 2) {
      //// the interpolant type used depends on the number of bytes of p0.    
      BaseGDL* p1 = e->GetParDefined(1);

      SizeT nx = p1->N_Elements();

      // Determine number and value of input points along x-axis and y-axis
      if (p0->Rank() < 1) e->Throw("Number of parameters must agree with dimensions of argument.");
      SizeT rankLeft = p0->Rank() - 1;

      //initialize output array with correct dimensions
      SizeT outRank = rankLeft;
      DLong dims[MAXRANK];
      SizeT i = 0;
      for (; i < outRank; ++i) dims[i] = p0->Dim(i);
      for (; i < MAXRANK; ++i) dims[i] = 0;
      for (SizeT i = 0; i < p1->Rank(); ++i) {
        dims[outRank++] = p1->Dim(i);
        if (outRank > MAXRANK)
          e->Throw("Rank of resulting array is currently limited to " + i2s(MAXRANK) + ".");
      }
      dimension dim((DLong *) dims, outRank);

      // Determine number of (contiguous) points in remaining dimensions
      SizeT ncontiguous = 1;
      for (SizeT i = 0; i < rankLeft; ++i) ncontiguous *= p0->Dim(i);
      
      SizeT un1=p0->Dim(p0->Rank()-1);
      if (un1<3 && cubic) cubic=false;
      if (un1<2) nnbor=true;
      
      //  std::cerr<<"rankLeft="<<rankLeft<<",ninterp="<<ninterp<<", nx="<<nx<<std::endl;

      //res dimension is [<----rankleft--->,nx]
      //sizes:           [   niterp values    ,nx]
      //linear: for each x, get ncontiguous values v0[,,,] at &array[,,,ix=floor(x)] , ncont.. values v1[,,,] at &array[,,,ix+1], with u=(x-ix), result is linConv(u,v0,v1);
      if (ncontiguous == 1) { //optimisation for this particular case: twice the speed.
        switch (p0->Type()) {
        case GDL_FLOAT:
          CALL_INTERPOLATE_1D_SINGLE(DFloatGDL, DFloat, DFloatGDL, DFloat)
        case GDL_DOUBLE:
          CALL_INTERPOLATE_1D_SINGLE(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
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
          CALL_INTERPOLATE_1D(DFloatGDL, DFloat, DFloatGDL, DFloat)
        case GDL_DOUBLE:
          CALL_INTERPOLATE_1D(DDoubleGDL, DDouble, DDoubleGDL, DDouble)
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
    throw;
  }
}
