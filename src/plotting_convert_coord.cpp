/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
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
#include "plotting.hpp"

#define TONORMCOORDX( in, out, log) out = (log) ? sx[0] + sx[1] * log10(in) : sx[0] + sx[1] * in;
#define TODATACOORDX( in, out, log) out = (log) ? pow(10.0, (in -sx[0])/sx[1]) : (in -sx[0])/sx[1];
#define TONORMCOORDY( in, out, log) out = (log) ? sy[0] + sy[1] * log10(in) : sy[0] + sy[1] * in;
#define TODATACOORDY( in, out, log) out = (log) ? pow(10.0, (in -sy[0])/sy[1]) : (in -sy[0])/sy[1];
#define TONORMCOORDZ( in, out, log) out = (log) ? sz[0] + sz[1] * log10(in) : sz[0] + sz[1] * in;
#define TODATACOORDZ( in, out, log) out = (log) ? pow(10.0, (in -sz[0])/sz[1]) : (in -sz[0])/sz[1];

namespace lib {

  using namespace std;

  //THE FOLLOWING ARE POSSIBLY THE WORST WAY TO DO THE JOB. At least they are to be used *only*
  //for [4,4] generalized 3D matrices

  void SelfPrint3d(DDoubleGDL* me) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim1 > 0) {
      for (auto j = 0, k = 0; j < dim1; ++j) {
        for (auto i = 0; i < dim0; ++i) std::cerr << (*me)[k++] << " ";
        std::cerr << std::endl;
      }
    } else for (auto i = 0; i < dim0; ++i) std::cerr << (*me)[i] << " ";
    std::cerr << std::endl;
  }

  void SelfTranspose3d(DDoubleGDL* me) {
    //crude quick hack to have the same behaviour as the other functions.
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = (new DDoubleGDL(dimension(dim1, dim0), BaseGDL::NOZERO));
    for (int j = 0; j < dim0; ++j) for (int i = 0; i < dim1; ++i)(*mat)[i * dim1 + j] = (*me)[j * dim0 + i];
    memcpy(me->DataAddr(), mat->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(mat);
  }

  void SelfReset3d(DDoubleGDL* me) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* Identity = (new DDoubleGDL(dimension(dim0, dim1)));
    for (SizeT i = 0; i < dim1; ++i) {
      (*Identity)[i * dim1 + i] = (double) 1.0;
    }
    memcpy(me->DataAddr(), Identity->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(Identity);
  }

  void SelfTranslate3d(DDoubleGDL* me, DDouble* trans) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = (new DDoubleGDL(dimension(dim0, dim1)));
    SelfReset3d(mat); //identity Matrix
    for (SizeT i = 0; i < 3; ++i) {
      (*mat)[3 * dim1 + i] = trans[i];
    }
    DDoubleGDL* intermediary = mat->MatrixOp(me, false, false);
    memcpy(me->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(intermediary);
    GDLDelete(mat);
  }

  void SelfScale3d(DDoubleGDL* me, DDouble *scale) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = (new DDoubleGDL(dimension(dim0, dim1)));
    SelfReset3d(mat); //identity Matrix
    (*mat)[0] = scale[0];
    (*mat)[5] = scale[1];
    (*mat)[10] = scale[2];
    //    SelfPrint3d(mat);
    DDoubleGDL* intermediary = mat->MatrixOp(me, false, false);
    memcpy(me->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(intermediary);
    GDLDelete(mat);
  }
#define DPI (double)(4*atan(1.0))
#define DEGTORAD (DPI/180.0)
#define RADTODEG (180.0/DPI)

  void SelfRotate3d(DDoubleGDL* me, DDouble *rot) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = (new DDoubleGDL(dimension(4, 4)));
    SelfReset3d(mat);
    DDoubleGDL* maty = (new DDoubleGDL(dimension(4, 4)));
    SelfReset3d(maty);
    DDoubleGDL* matz = (new DDoubleGDL(dimension(4, 4)));
    SelfReset3d(matz);
    SizeT ncols = 4;
    double c, s;
    for (SizeT j = 0; j < 3; ++j) {
      c = cos(rot[j] * DEGTORAD);
      s = sin(rot[j] * DEGTORAD);
      switch (j) {
      case 0:
      {
        (*mat)[1 * ncols + 1] = c;
        (*mat)[1 * ncols + 2] = s;
        (*mat)[2 * ncols + 1] = -s;
        (*mat)[2 * ncols + 2] = c;
        break;
      }
      case 1:
      {
        (*maty)[0 * ncols + 0] = c;
        (*maty)[0 * ncols + 2] = -s;
        (*maty)[2 * ncols + 0] = s;
        (*maty)[2 * ncols + 2] = c;
        DDoubleGDL* intermediary = maty->MatrixOp(mat, false, false);
        memcpy(mat->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
        GDLDelete(intermediary);
        break;
      }
      case 2:
      {
        (*matz)[0 * ncols + 0] = c;
        (*matz)[0 * ncols + 1] = s;
        (*matz)[1 * ncols + 0] = -s;
        (*matz)[1 * ncols + 1] = c;
        DDoubleGDL* intermediary = matz->MatrixOp(mat, false, false);
        memcpy(mat->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
        GDLDelete(intermediary);
      }
      }
    }

    DDoubleGDL* intermediary = mat->MatrixOp(me, false, false);
    memcpy(me->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(intermediary);
    GDLDelete(matz);
    GDLDelete(maty);
    GDLDelete(mat);
  }

  void SelfPerspective3d(DDoubleGDL* me, DDouble zdist) {
    if (!isfinite(zdist)) return; //Nan
    if (zdist == 0.0) return;
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = (new DDoubleGDL(dimension(dim0, dim1)));
    SelfReset3d(mat); //identity Matrix
    (*mat)[2 * dim1 + 3] = -1.0 / zdist;
    DDoubleGDL* intermediary = mat->MatrixOp(me, false, false);
    memcpy(me->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(intermediary);
    GDLDelete(mat);
  }

  void SelfOblique3d(DDoubleGDL* me, DDouble dist, DDouble angle) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = (new DDoubleGDL(dimension(dim0, dim1)));
    SelfReset3d(mat); //identity Matrix
    (*mat)[2 * dim1 + 2] = 0.0;
    (*mat)[2 * dim1 + 0] = dist * cos(angle * DEGTORAD);
    (*mat)[2 * dim1 + 1] = dist * sin(angle * DEGTORAD);
    DDoubleGDL* intermediary = mat->MatrixOp(me, false, false);
    memcpy(me->DataAddr(), intermediary->DataAddr(), dim0 * dim1 * sizeof (double));
    GDLDelete(intermediary);
    GDLDelete(mat);
  }

  void SelfExch3d(DDoubleGDL* me, T3DEXCHANGECODE axisExchangeCode) {
    SizeT dim0 = me->Dim(0);
    SizeT dim1 = me->Dim(1);
    if (dim0 != 4 && dim1 != 4) return;
    DDoubleGDL* mat = me->Dup();
    switch (axisExchangeCode) {
    case XY: //exchange 0 and 1
      for (SizeT i = 0; i < dim0; ++i) {
        (*me)[0 * dim1 + i] = (*mat)[1 * dim1 + i];
        (*me)[1 * dim1 + i] = (*mat)[0 * dim1 + i];
      }
      break;
    case XZ: //exchange 0 and 2
      for (SizeT i = 0; i < dim0; ++i) {
        (*me)[0 * dim1 + i] = (*mat)[2 * dim1 + i];
        (*me)[2 * dim1 + i] = (*mat)[0 * dim1 + i];
      }
      break;
    case YZ: //exchange 1 and 2
      for (SizeT i = 0; i < dim0; ++i) {
        (*me)[1 * dim1 + i] = (*mat)[2 * dim1 + i];
        (*me)[2 * dim1 + i] = (*mat)[1 * dim1 + i];
      }
      break;
    default: //is an error
      assert(false);
    }
    GDLDelete(mat);
  }

  DDoubleGDL* convert_coord_double(EnvT* e, DDoubleGDL* xVal, DDoubleGDL* yVal, DDoubleGDL* zVal, COORDSYS icoordinateSystem, COORDSYS ocoordinateSystem) {

    DLong dims[2] = {3, 0};

    DDoubleGDL* res;
    SizeT nrows;

    nrows = xVal->Dim(0);
    dims[1] = nrows;
    dimension dim((DLong *) dims, 2);
    res = new DDoubleGDL(dim, BaseGDL::NOZERO);

    //eliminate simplest case here:
    if (ocoordinateSystem == icoordinateSystem) {
      if ((GDL_NTHREADS=parallelize( nrows, TP_MEMORY_ACCESS))==1) {
        for (OMPInt i = 0; i < nrows; ++i) {
          (*res)[i * 3 + 0] = (*xVal)[i];
          (*res)[i * 3 + 1] = (*yVal)[i];
          (*res)[i * 3 + 2] = (*zVal)[i];
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nrows; ++i) {
          (*res)[i * 3 + 0] = (*xVal)[i];
          (*res)[i * 3 + 1] = (*yVal)[i];
          (*res)[i * 3 + 2] = (*zVal)[i];
        }
      }
      return res;
    }

    DDouble *sx, *sy, *sz;
    GetSFromPlotStructs(&sx, &sy, &sz);

    bool xLog, yLog, zLog;
    gdlGetAxisType(XAXIS, xLog);
    gdlGetAxisType(YAXIS, yLog);
    gdlGetAxisType(ZAXIS, zLog);

    int xSize, ySize;
    //give default values
    DStructGDL* dStruct = SysVar::D();
    unsigned xsizeTag = dStruct->Desc()->TagIndex("X_SIZE");
    unsigned ysizeTag = dStruct->Desc()->TagIndex("Y_SIZE");
    xSize = (*static_cast<DLongGDL*> (dStruct->GetTag(xsizeTag, 0)))[0];
    ySize = (*static_cast<DLongGDL*> (dStruct->GetTag(ysizeTag, 0)))[0];
    // Use Size in lieu of VSize
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    DLong wIx = actDevice->ActWin();
    if (wIx != -1) bool success = actDevice->WSize(wIx, &xSize, &ySize); //on failure, sizes are ot changed by WSize.
    else Warning("CONVERT_COORD: Window is closed and unavailable.");
    
    //projection?
    bool mapSet = false;
#ifdef USE_LIBPROJ
    static LPTYPE idata;
    static XYTYPE odata;
    get_mapset(mapSet);
    if (mapSet) {
      ref = map_init();
      if (ref == NULL) e->Throw("Projection initialization failed.");
    }
#endif

    //convert input (we can overwrite X Y and Z) to normalized
    switch (icoordinateSystem) {
    case DATA:
      // to u,v
#ifdef USE_LIBPROJ
      if (mapSet) {
          for (SizeT i = 0; i < nrows; i++) {
#if LIBPROJ_MAJOR_VERSION >= 5
            idata.lam = (*xVal)[i] * DEG_TO_RAD;
            idata.phi = (*yVal)[i] * DEG_TO_RAD;
            odata = protect_proj_fwd_lp(idata, ref);
            (*xVal)[i] = odata.x;
            (*yVal)[i] = odata.y;
#else
            idata.u = (*xVal)[i] * DEG_TO_RAD;
            idata.v = (*yVal)[i] * DEG_TO_RAD;
            odata = PJ_FWD(idata, ref);
            (*xVal)[i] = odata.u;
            (*yVal)[i] = odata.v;
#endif
          }
      }
#endif
      // to norm:
      if ((GDL_NTHREADS=parallelize( nrows, TP_MEMORY_ACCESS))==1) {
        for (OMPInt i = 0; i < nrows; ++i) {
          TONORMCOORDX((*xVal)[i], (*xVal)[i], xLog);
          TONORMCOORDY((*yVal)[i], (*yVal)[i], yLog);
          TONORMCOORDZ((*zVal)[i], (*zVal)[i], zLog);
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nrows; ++i) {
          TONORMCOORDX((*xVal)[i], (*xVal)[i], xLog);
          TONORMCOORDY((*yVal)[i], (*yVal)[i], yLog);
          TONORMCOORDZ((*zVal)[i], (*zVal)[i], zLog);
        }
      }
      break;
    case DEVICE:
      if ((GDL_NTHREADS=parallelize( nrows, TP_MEMORY_ACCESS))==1) {
        for (OMPInt i = 0; i < nrows; ++i) {
          (*xVal)[i] /= xSize;
          (*yVal)[i] /= ySize;
          // (zSize is 1)
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nrows; ++i) {
          (*xVal)[i] /= xSize;
          (*yVal)[i] /= ySize;
          // (zSize is 1)
        }
      }
    default:
      break;
    }

    //convert to output from normalized
    switch (ocoordinateSystem) {
    case DATA:
      // from norm:
      if ((GDL_NTHREADS=parallelize( nrows, TP_MEMORY_ACCESS))==1) {
        for (OMPInt i = 0; i < nrows; ++i) {
          TODATACOORDX((*xVal)[i], (*xVal)[i], xLog);
          TODATACOORDY((*yVal)[i], (*yVal)[i], yLog);
          TODATACOORDZ((*zVal)[i], (*zVal)[i], zLog);
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nrows; ++i) {
          TODATACOORDX((*xVal)[i], (*xVal)[i], xLog);
          TODATACOORDY((*yVal)[i], (*yVal)[i], yLog);
          TODATACOORDZ((*zVal)[i], (*zVal)[i], zLog);
        }
      }

      // from u,v
#ifdef USE_LIBPROJ
      if (mapSet) {
          for (SizeT i = 0; i < nrows; i++) {
#if LIBPROJ_MAJOR_VERSION >= 5
            odata.x = (*xVal)[i];
            odata.y = (*yVal)[i];
            idata = protect_proj_inv_xy(odata, ref);
            (*xVal)[i] = idata.lam * RAD_TO_DEG;
            (*yVal)[i] = idata.phi * RAD_TO_DEG;
#else
            odata.u = (*xVal)[i];
            odata.v = (*yVal)[i];
            idata = PJ_INV(odata, ref);
            (*xVal)[i] = idata.u * RAD_TO_DEG;
            (*yVal)[i] = idata.v * RAD_TO_DEG;
#endif
          }
        }
#endif

      break;
    case DEVICE:
      if ((GDL_NTHREADS=parallelize( nrows, TP_MEMORY_ACCESS))==1) {
        for (OMPInt i = 0; i < nrows; ++i) {
          (*xVal)[i] *= xSize;
          (*yVal)[i] *= ySize;
          //(zSize is 1)
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nrows; ++i) {
          (*xVal)[i] *= xSize;
          (*yVal)[i] *= ySize;
          //(zSize is 1)
        }
      }
    default:
      break;
    }

    if ((GDL_NTHREADS=parallelize( nrows, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < nrows; ++i) {
        (*res)[i * 3 + 0] = (*xVal)[i];
        (*res)[i * 3 + 1] = (*yVal)[i];
        (*res)[i * 3 + 2] = (*zVal)[i];
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nrows; ++i) {
        (*res)[i * 3 + 0] = (*xVal)[i];
        (*res)[i * 3 + 1] = (*yVal)[i];
        (*res)[i * 3 + 2] = (*zVal)[i];
      }
    }
    return res;
  }

  BaseGDL* convert_coord(EnvT* e) {
    DDoubleGDL* xVal, *yVal, *zVal;
    Guard<DDoubleGDL> xval_guard, yval_guard, zval_guard;
    SizeT xEl, yEl, zEl, minEl, xDim, yDim, zDim;

    //behaviour: 1 argument: needs to be [2,*] or [3,*] else 2 args: X,vector, Y vector 1 (z=vector zero). else 3 args, 3 vectors.
    //in case of vectors, note usual behaviour:
    //minimum set of dimensions of arrays. singletons expanded to dimension,

    SizeT nParam = e->NParam(1);

    BaseGDL* p0;
    BaseGDL* p1;
    BaseGDL* p2;

    DType type = GDL_FLOAT;
    static int doubleIx = e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx)) type = GDL_DOUBLE;

    p0 = e->GetParDefined(0);
    if (p0->Type() == GDL_DOUBLE) type = GDL_DOUBLE;

    if (nParam == 1) {
      if (p0->Dim(0) == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(0));
      if (p0->Dim(0) != 2 && p0->Dim(0) != 3) e->Throw("When only 1 param, dims must be (2,n) or (3,n)"); //with n=0 also!!!
      SizeT dim0 = p0->Dim(0);
      minEl = p0->Dim(1);
      if (minEl == 0) minEl = 1; // aka in convert_coord([44,22])-->n_dim=1,dim=2
      xVal = new DDoubleGDL(dimension(minEl), BaseGDL::NOZERO);
      xval_guard.Reset(xVal); // delete upon exit
      DDoubleGDL* tmpVal = e->GetParAs< DDoubleGDL>(0);
      for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpVal)[i * dim0 + 0];
      yVal = new DDoubleGDL(dimension(minEl), BaseGDL::NOZERO);
      yval_guard.Reset(yVal); // delete upon exit
      for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpVal)[i * dim0 + 1];
      zVal = new DDoubleGDL(dimension(minEl), BaseGDL::ZERO);
      zval_guard.Reset(zVal); // delete upon exit
      if (dim0 == 3) for (SizeT i = 0; i < minEl; ++i) (*zVal)[i] = (*tmpVal)[i * dim0 + 2];
    }

    if (nParam >= 2) {
      p1 = e->GetParDefined(1);
      if (p1->Type() == GDL_DOUBLE) type = GDL_DOUBLE;
      p0 = e->GetParDefined(0);
      xEl = p0->N_Elements();
      xDim = p0->Dim(0);
      yEl = p1->N_Elements();
      yDim = p1->Dim(0);
      //minEl:
      minEl = -1;
      minEl = (minEl > xEl && xDim) ? xEl : minEl;
      minEl = (minEl > yEl && yDim) ? yEl : minEl;

      if (minEl == -1) {
        minEl = 1;
      } else {
        minEl = (minEl > xEl && xDim) ? xEl : minEl;
        minEl = (minEl > yEl && yDim) ? yEl : minEl;
      }
      DDoubleGDL* tmpxVal = e->GetParAs< DDoubleGDL>(0);
      xVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
      xval_guard.Reset(xVal); // delete upon exit
      if (xDim) for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpxVal)[i];
      else for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpxVal)[0];
      DDoubleGDL* tmpyVal = e->GetParAs< DDoubleGDL>(1);
      yVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
      yval_guard.Reset(yVal); // delete upon exit
      if (yDim) for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpyVal)[i];
      else for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpyVal)[0];
      zVal = new DDoubleGDL(minEl, BaseGDL::ZERO);
      zval_guard.Reset(zVal); // delete upon exit
    }

    if (nParam == 3) {
      p2 = e->GetParDefined(2);
      if (p2->Type() == GDL_DOUBLE) type = GDL_DOUBLE;
      zEl = p2->N_Elements();
      zDim = p2->Dim(0);
      p0 = e->GetParDefined(0);
      xEl = p0->N_Elements();
      xDim = p0->Dim(0);
      p1 = e->GetParDefined(1);
      yEl = p1->N_Elements();
      yDim = p1->Dim(0);
      //minEl:
      minEl = -1;
      minEl = (minEl > xEl && xDim) ? xEl : minEl;
      minEl = (minEl > yEl && yDim) ? yEl : minEl;
      minEl = (minEl > zEl && zDim) ? zEl : minEl;

      if (minEl == -1) {
        minEl = 1;
      } else {
        minEl = (minEl > xEl && xDim) ? xEl : minEl;
        minEl = (minEl > yEl && yDim) ? yEl : minEl;
        minEl = (minEl > zEl && zDim) ? zEl : minEl;
      }
      DDoubleGDL* tmpxVal = e->GetParAs< DDoubleGDL>(0);
      xVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
      xval_guard.Reset(xVal); // delete upon exit
      if (xDim) for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpxVal)[i];
      else for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpxVal)[0];
      DDoubleGDL* tmpyVal = e->GetParAs< DDoubleGDL>(1);
      yVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
      yval_guard.Reset(yVal); // delete upon exit
      if (yDim) for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpyVal)[i];
      else for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpyVal)[0];
      DDoubleGDL* tmpzVal = e->GetParAs< DDoubleGDL>(2);
      zVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
      zval_guard.Reset(zVal); // delete upon exit
      if (zDim) for (SizeT i = 0; i < minEl; ++i) (*zVal)[i] = (*tmpzVal)[i];
      else for (SizeT i = 0; i < minEl; ++i) (*zVal)[i] = (*tmpzVal)[0];
    }
    COORDSYS icoordinateSystem = DATA, ocoordinateSystem = DATA;
    //check presence of DATA,DEVICE and NORMAL options
    static int DATAIx = e->KeywordIx("DATA");
    static int DEVICEIx = e->KeywordIx("DEVICE");
    static int NORMALIx = e->KeywordIx("NORMAL");
    static int TO_DATAIx = e->KeywordIx("TO_DATA");
    static int TO_DEVICEIx = e->KeywordIx("TO_DEVICE");
    static int TO_NORMALIx = e->KeywordIx("TO_NORMAL");

    if (e->KeywordSet(DATAIx)) icoordinateSystem = DATA;
    if (e->KeywordSet(DEVICEIx)) icoordinateSystem = DEVICE;
    if (e->KeywordSet(NORMALIx)) icoordinateSystem = NORMAL;
    if (e->KeywordSet(TO_DATAIx)) ocoordinateSystem = DATA;
    if (e->KeywordSet(TO_DEVICEIx)) ocoordinateSystem = DEVICE;
    if (e->KeywordSet(TO_NORMALIx)) ocoordinateSystem = NORMAL;
    
    //special treatment of T3D: conversion HERE BELOW
    static int t3dIx = e->KeywordIx("T3D");
    bool doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());
    doT3d = doT3d && e->KeywordSet(TO_DEVICEIx); //T3D ignored if TO is not device;
    doT3d = (doT3d && !e->KeywordSet(NORMALIx)); //and if entries are no DATA-like
    doT3d = (doT3d && !e->KeywordSet(DEVICEIx));
    
    if (doT3d) //convert X,Y,Z in X',Y' as per T3D perspective.
    {  //input is DATA 

      SizeT nEl = xVal->N_Elements();

      //projection?
      bool mapSet = false;
#ifdef USE_LIBPROJ
      static LPTYPE idata;
      static XYTYPE odata;
      get_mapset(mapSet);
      if (mapSet) {
        ref = map_init();
        if (ref == NULL) e->Throw("Projection initialization failed.");
      }
#endif
      // to u,v
#ifdef USE_LIBPROJ
      if (mapSet) {
        for (SizeT i = 0; i < nEl; i++) {
#if LIBPROJ_MAJOR_VERSION >= 5
          idata.lam = (*xVal)[i] * DEG_TO_RAD;
          idata.phi = (*yVal)[i] * DEG_TO_RAD;
          odata = protect_proj_fwd_lp(idata, ref);
          (*xVal)[i] = odata.x;
          (*yVal)[i] = odata.y;
#else
          idata.u = (*xVal)[i] * DEG_TO_RAD;
          idata.v = (*yVal)[i] * DEG_TO_RAD;
          odata = PJ_FWD(idata, ref);
          (*xVal)[i] = odata.u;
          (*yVal)[i] = odata.v;
#endif
        }
      }
#endif
      
      DDoubleGDL* t3dMatrix = gdlGetT3DMatrix(); //the original one
      Guard<BaseGDL> t3dMatrix_guard(t3dMatrix);
      DDouble *sx, *sy, *sz;
      GetSFromPlotStructs(&sx, &sy, &sz);

        //populate a 4D matrix with reduced coordinates through sx,sy,sz:
      DDoubleGDL* xyzw = new DDoubleGDL(dimension(nEl, 4));
      memcpy(&((*xyzw)[0]), xVal->DataAddr(), nEl * sizeof (double));
      memcpy(&((*xyzw)[nEl]), yVal->DataAddr(), nEl * sizeof (double));
      memcpy(&((*xyzw)[2 * nEl]), zVal->DataAddr(), nEl * sizeof (double));
      for (int index = 0; index < nEl; ++index) {
        (*xyzw)[3 * nEl + index] = 1.0;
      }
      for (auto i=0; i< nEl; ++i) (*xyzw)[i] = (*xyzw)[i]*sx[1]+sx[0];
      for (auto i=nEl; i< 2*nEl; ++i) (*xyzw)[i] = (*xyzw)[i]*sy[1]+sy[0];
      for (auto i=2*nEl; i< 3*nEl; ++i) (*xyzw)[i] = (*xyzw)[i]*sz[1]+sz[0];
      DDoubleGDL* trans = t3dMatrix->MatrixOp(xyzw, false, true); //transpose xyzw for Operation
      SelfPrint3d(trans);
      for (auto i=0; i< nEl; ++i) (*xVal)[i] = (*trans)[i*4];
      for (auto i=0; i< nEl; ++i) (*yVal)[i] = (*trans)[i*4+1];
      for (auto i=0; i< nEl; ++i) (*zVal)[i] = (*trans)[i*4+2];
      GDLDelete(trans);
      GDLDelete(xyzw);
      //values are now in NORMED:
      icoordinateSystem=NORMAL;
    }

    DDoubleGDL* res = convert_coord_double(e, xVal, yVal, zVal, icoordinateSystem, ocoordinateSystem);

    if (type == GDL_DOUBLE) {
      return res;
    }
    //else return something...
    DFloatGDL* res1 = static_cast<DFloatGDL*> (res->Convert2(GDL_FLOAT, BaseGDL::COPY));
    delete res;
    return res1;
  }

  DDoubleGDL* gdlDoAsSurfr(DDouble az, DDouble alt, DDouble *scalex, DDouble *scaley, DDouble *scalez ) {
//    std::cerr<<"gdlDoAsSurfr()\n";
    DDoubleGDL* plplot3d = (new DDoubleGDL(dimension(4, 4), BaseGDL::NOZERO));
    SelfReset3d(plplot3d);
    static DDouble mytrans[3] = {-0.5, -0.5, -0.5};
    SelfTranslate3d(plplot3d, mytrans);
    DDouble rot1[3] = {-90.0, az, 0.0};
    DDouble rot2[3] = {alt, 0.0, 0.0};
    SelfRotate3d(plplot3d, rot1);
    SelfRotate3d(plplot3d, rot2);
  // create all 8 homogenous coordinates of the cubes points, 
  // compute their projected coord,
  // find min and max on both axes.
    static DDoubleGDL* cube_coord = (new DDoubleGDL(dimension(8,4), BaseGDL::NOZERO));
    static DDouble vals[32]={0,1,0,1,0,1,0,1,0,0,1,1,0,0,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1};
    for (auto i=0; i< 32; ++i) (*cube_coord)[i]=vals[i];
    //    w=v#!p.t
    DDoubleGDL* tmp = plplot3d->MatrixOp(cube_coord, false, true);
    DDoubleGDL* res = static_cast<DDoubleGDL*>(tmp->Transpose(NULL));
    GDLDelete(tmp);
//    SelfPrint3d(res);
    //    z=w[*,3]
    // normalize? not useful as this is not a perspective or oblique matrix.
//w[*,0]/=z
//w[*,1]/=z
//w[*,2]/=z
//min=min(w,dim=1)//max=max(w,dim=1)
    DDouble valmin[4], valmax[4];
    DLong   posmin[4], posmax[4];
    res->MinMax(&posmin[0], &posmax[0],NULL,NULL, false, 0, 8);
    res->MinMax(&posmin[1], &posmax[1],NULL,NULL, false, 8, 16);
    res->MinMax(&posmin[2], &posmax[2],NULL,NULL, false, 16, 24);
    res->MinMax(&posmin[3], &posmax[3],NULL,NULL, false, 24, 32);
    for (auto i=0; i<4; ++i) {
      valmin[i]=(*res)[posmin[i]];
      valmax[i]=(*res)[posmax[i]];
    }

    if (valmax[0] == valmin[0]) valmax[0]=valmin[0]+1;
      if (valmax[1] == valmin[1]) valmax[1]=valmin[1]+1;
      if (valmax[2] == valmin[2]) valmax[2]=valmin[2]+1;
//translate=[ -min[0], -min[1], -min[2]]
      DDouble translate[3] = {-valmin[0],-valmin[1],-valmin[2]}; 
//scale=1./(max[0:2]-min[0:2])
      *scalex=1./(valmax[0]-valmin[0]);
      *scaley=1./(valmax[1]-valmin[1]);
      *scalez=1./(valmax[2]-valmin[2]);
      DDouble scale[3]={*scalex,*scaley,*scalez};
//t3d,tr = translate, sc=scale
      SelfTranslate3d(plplot3d, translate);
      SelfScale3d(plplot3d, scale);
  return plplot3d; 
  }
  
DDoubleGDL* gdlDoAsScale3(DDouble az, DDouble alt, DDouble *scalex, DDouble *scaley, DDouble *scalez ) {
//    std::cerr<<"gdlDoAsScale3()\n";
    DDoubleGDL* plplot3d = (new DDoubleGDL(dimension(4, 4), BaseGDL::NOZERO));
    SelfReset3d(plplot3d);
    static DDouble mytrans1[3] = {-0.5, -0.5, -0.5};
    static DDouble mytrans2[3] = {0.5, 0.5, 0.5};
    SelfTranslate3d(plplot3d, mytrans1);
    *scalex=1./sqrt(3); *scaley=*scalex; *scalez=*scalex;
    DDouble scale[3]={*scalex,*scaley,*scalez};
    SelfScale3d(plplot3d, scale);
    DDouble rot1[3] = {-90.0, az, 0.0};
    DDouble rot2[3] = {alt, 0.0, 0.0};
    SelfRotate3d(plplot3d, rot1);
    SelfRotate3d(plplot3d, rot2);
    SelfTranslate3d(plplot3d, mytrans2);
  return plplot3d; 
  }
  
   DDoubleGDL* gdlDefinePlplotRotationMatrix(DDouble az, DDouble alt, DDouble *scale, bool save) {
//    std::cerr<<"gdlDefinePlplotRotationMatrix()\n";
//    DDoubleGDL* t3dMatrix = gdlDoAsScale3(az,alt, &scale[0], &scale[1], &scale[2]); 
    DDoubleGDL* t3dMatrix = gdlDoAsSurfr(az,alt, &scale[0], &scale[1], &scale[2]); 
    SelfTranspose3d(t3dMatrix);
    if (save) {
      DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
      static unsigned tTag=pStruct->Desc()->TagIndex("T");
      for (int i=0; i<t3dMatrix->N_Elements(); ++i )(*static_cast<DDoubleGDL*>(pStruct->GetTag(tTag, 0)))[i]=(*t3dMatrix)[i];
    }
    return t3dMatrix; 
  }
  
 //converts 3D values according to COORDSYS towards NORMAL coordinates and , logically, unset xLog,yLo,zLog and define code as NORMAL.
  void SelfConvertToNormXYZ(DDoubleGDL *x, bool &xLog, DDoubleGDL *y, bool &yLog, DDoubleGDL *z, bool &zLog, COORDSYS &code) {
//    std::cerr<<"SelfConvertToNormXYZ(DDoubleGDL)\n";
    SizeT n=x->N_Elements();
    if (code == DATA) {
      DDouble *sx, *sy, *sz;
      GetSFromPlotStructs(&sx, &sy, &sz);
      for (auto i = 0; i < x->N_Elements(); ++i) TONORMCOORDX( (*x)[i], (*x)[i], xLog);
      for (auto i = 0; i < y->N_Elements(); ++i) TONORMCOORDY( (*y)[i], (*y)[i], yLog);
      for (auto i = 0; i < z->N_Elements(); ++i) TONORMCOORDZ( (*z)[i], (*z)[i], zLog);
      code = NORMAL;
      xLog = false;
      yLog = false;
      zLog = false;
    }
  }
  //same for a DDouble, leaves code and log unchanged.

  void SelfConvertToNormXYZ(DDouble &x, bool const xLog, DDouble &y, bool const yLog, DDouble &z, bool const zLog, COORDSYS const code) {
//    std::cerr << "SelfConvertToNormXYZ(DDouble)\n";
    if (code == DATA) {
      DDouble *sx, *sy, *sz;
      GetSFromPlotStructs(&sx, &sy, &sz);
      TONORMCOORDX(x, x, xLog);
      TONORMCOORDY(y, y, yLog);
      TONORMCOORDZ(z, z, zLog);
    } else if (code == DEVICE) { //Z=0 in this case
      int xSize, ySize;
      //give default values
      DStructGDL* dStruct = SysVar::D();
      unsigned xsizeTag = dStruct->Desc()->TagIndex("X_SIZE");
      unsigned ysizeTag = dStruct->Desc()->TagIndex("Y_SIZE");
      xSize = (*static_cast<DLongGDL*> (dStruct->GetTag(xsizeTag, 0)))[0];
      ySize = (*static_cast<DLongGDL*> (dStruct->GetTag(ysizeTag, 0)))[0];
      x /= xSize;
      y /= ySize;
      z = 0;
    }
  }
  
  //converts x and y but leaves code and log unchanged.
  void SelfConvertToNormXY(SizeT n, DDouble *x, bool const xLog, DDouble *y, bool const yLog, COORDSYS const code) {
//  std::cerr<<"SelfConvertToNormXY(DDouble)"<<std::endl;
  if (code == DATA) {
      DDouble *sx, *sy;
      GetSFromPlotStructs(&sx, &sy);
      for (auto i = 0; i < n; ++i) TONORMCOORDX( x[i], x[i], xLog);
      for (auto i = 0; i < n; ++i) TONORMCOORDY( y[i], y[i], yLog);
    } else if (code == DEVICE) {
      int xSize, ySize;
      //give default values
      DStructGDL* dStruct = SysVar::D();
      unsigned xsizeTag = dStruct->Desc()->TagIndex("X_SIZE");
      unsigned ysizeTag = dStruct->Desc()->TagIndex("Y_SIZE");
      xSize = (*static_cast<DLongGDL*> (dStruct->GetTag(xsizeTag, 0)))[0];
      ySize = (*static_cast<DLongGDL*> (dStruct->GetTag(ysizeTag, 0)))[0];
      for (auto i = 0; i < n; ++i) x[i] /= xSize;
      for (auto i = 0; i < n; ++i) y[i] /= ySize;
    }
  } 
  
  //converts x and y and updates code and log, for futher use in the pipeline.
   void SelfConvertToNormXY(DDoubleGDL *x, bool &xLog, DDoubleGDL *y, bool &yLog, COORDSYS &code) {
//  std::cerr<<"SelfConvertToNormXY(DDoubleGD)"<<std::endl;
    SizeT nx=x->N_Elements();
    SizeT ny=y->N_Elements();
  if (code == DATA) {
      DDouble *sx, *sy;
      GetSFromPlotStructs(&sx, &sy);
      for (auto i = 0; i < nx; ++i) TONORMCOORDX( (*x)[i], (*x)[i], xLog);
      for (auto i = 0; i < ny; ++i) TONORMCOORDY( (*y)[i], (*y)[i], yLog);
    } else if (code == DEVICE) {
      int xSize, ySize;
      //give default values
      DStructGDL* dStruct = SysVar::D();
      unsigned xsizeTag = dStruct->Desc()->TagIndex("X_SIZE");
      unsigned ysizeTag = dStruct->Desc()->TagIndex("Y_SIZE");
      xSize = (*static_cast<DLongGDL*> (dStruct->GetTag(xsizeTag, 0)))[0];
      ySize = (*static_cast<DLongGDL*> (dStruct->GetTag(ysizeTag, 0)))[0];
      for (auto i = 0; i < nx; ++i) (*x)[i] /= xSize;
      for (auto i = 0; i < ny; ++i) (*y)[i] /= ySize;
    }
    code=NORMAL;
    xLog=false;
    yLog=false;
  }
   
  //Implied Useage: x,y,z in NORMAL coordinates
  void SelfPDotTTransformXYZ(SizeT n, DDouble *x, DDouble *y, DDouble *z){
//  std::cerr<<"SelfPDotTTransformXYZ()"<<std::endl;
    //retrieve !P.T 
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    DDouble* t= static_cast<DDouble*>(pStruct->GetTag(tTag, 0)->DataAddr());
    DDouble a,b,c,w;
    for (SizeT i=0; i< n; ++i) {
      a = x[i] * t[0] + y[i] * t[1] + z[i] * t[2] + t[3]; 
      b = x[i] * t[4] + y[i] * t[5] + z[i] * t[6] + t[7]; 
      c = x[i] * t[8] + y[i] * t[9] + z[i] * t[10] + t[11]; 
      w = x[i] * t[12] + y[i] * t[13] + z[i] * t[14] + t[15];
    
      x[i] = a / w; 
      y[i] = b / w; 
      z[i] = c / w;
    }
  }
  void SelfPDotTTransformXYZ(DDoubleGDL *x, DDoubleGDL *y, DDoubleGDL *z){
//  std::cerr<<"SelfPDotTTransformXYZ()"<<std::endl;
    assert (x->N_Elements()==y->N_Elements());
    SizeT n=x->N_Elements();
    //retrieve !P.T 
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    DDouble* t= static_cast<DDouble*>(pStruct->GetTag(tTag, 0)->DataAddr());
    DDouble a,b,c,w;
    for (SizeT i=0; i< n; ++i) {
      a = (*x)[i] * t[0]  + (*y)[i] * t[1]  + (*z)[i] * t[2]  + t[3]; 
      b = (*x)[i] * t[4]  + (*y)[i] * t[5]  + (*z)[i] * t[6]  + t[7]; 
      c = (*x)[i] * t[8]  + (*y)[i] * t[9]  + (*z)[i] * t[10] + t[11]; 
      w = (*x)[i] * t[12] + (*y)[i] * t[13] + (*z)[i] * t[14] + t[15];
    
      (*x)[i] = a / w; 
      (*y)[i] = b / w; 
      (*z)[i] = c / w;
    }
  }  

   void PDotTTransformXYZval(DDouble x, DDouble y, DDouble *xt, DDouble *yt, PLPointer data){
//    std::cerr<<"PDotTTransformXYZval()"<<std::endl;
    DDouble *pz=(DDouble*) data;
    DDouble z=*pz;
    //retrieve !P.T 
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    DDouble* t= static_cast<DDouble*>(pStruct->GetTag(tTag, 0)->DataAddr());
    *xt = x * t[0] + y * t[1] + z * t[2] + t[3]; 
    *yt = x * t[4] + y * t[5] + z * t[6] + t[7]; 
  }

   DDoubleGDL* gdlGetT3DMatrix() {
//    std::cerr<<"gdlGetT3DMatrix()\n";
    DDoubleGDL* t3dMatrix = (new DDoubleGDL(dimension(4, 4), BaseGDL::NOZERO));
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    for (int i = 0; i < t3dMatrix->N_Elements(); ++i)(*t3dMatrix)[i] = (*static_cast<DDoubleGDL*> (pStruct->GetTag(tTag, 0)))[i];
    SelfTranspose3d(t3dMatrix);
    return t3dMatrix;
  }

// Check if passed 4x4 matrix is valid :
// the projection of the Z axis must be on the Y axis, otherwise the matrix is not good.
// return NULL if not or retrieves exchange code.
bool isValidRotationMatrix(DDoubleGDL* Matrix, T3DEXCHANGECODE &axisExchangeCode){
    axisExchangeCode=NORMAL3D;
    return true;
//    bool a=(abs((*Matrix)[0]) < 1E-4);
//    bool b=(abs((*Matrix)[4]) < 1E-4);
//    bool c=(abs((*Matrix)[8]) < 1E-4);
//    if (c) {
//      static DDoubleGDL* x = new DDoubleGDL(dimension(4));
//      static DDoubleGDL* y = new DDoubleGDL(dimension(4));
//      static DDoubleGDL* z = new DDoubleGDL(dimension(4));
//      (*x)[0] = 1;
//      (*y)[1] = 1;
//      (*z)[2] = 1;
//
//      DDoubleGDL* res;
//      res = Matrix->MatrixOp(y, false, false);
//      SelfPrint3d(res);
//      if (abs((*res)[0] < 1E-4)) {
//        //XY?
//        std::cerr << "NORMAL3D\n";
//        axisExchangeCode = NORMAL3D;
//        return true;
//      } else {
//        res = Matrix->MatrixOp(x, false, false);
//        SelfPrint3d(res);
//        if (abs((*res)[0] < 1E-4)) {
//          std::cerr << "XY\n";
//          axisExchangeCode = XY;
//          return true;
//        } else {
//          return false;
//        }
//      }
//    } else {
//      if (b) {
//        std::cerr << "YZ\n";
//        axisExchangeCode = YZ;
//        return true;
//      } else if (a) {
//        std::cerr << "XZ\n";
//        axisExchangeCode = XZ;
//        return true;
//      } else return false;
//    }
    return false;
}  
bool isAxonometricRotation(DDoubleGDL* Matrix, DDouble &alt, DDouble &az, DDouble &ay, DDouble *scale, T3DEXCHANGECODE &axisExchangeCode, bool &below) {
//  std::cerr<<"isAxonometricRotation()\n";
  // Comments To Be Revised equations below are not exact.
  //with *DL notations, the 3x3 'rotation+scale' subset of the 4x4 matrix is of the form:
  //        [             cos(ay) cos(az)                               -cos(ay) sin(az)                     sin(ay)     ]
  //        [                                                                                                            ]
  // rot := [sin(ax) sin(ay) cos(az) + cos(ax) sin(az)     -sin(ax) sin(ay) sin(az) + cos(ax) cos(az)    -sin(ax) cos(ay)]
  //        [                                                                                                            ]
  //        [-cos(ax) sin(ay) cos(az) + sin(ax) sin(az)    cos(ax) sin(ay) sin(az) + sin(ax) cos(az)     cos(ax) cos(ay) ]
  //For the 3x3 matrix to be compatible with plplot ax (elevation) and az (azimuth) ay must be 0 after a +90 initial rotation around X:
 //      0                1                  2
 // 0 [cos(az)    sin(az) sin(ax)     sin(az) cos(ax) ]
 //   [                                               ]
 // 1 [sin(az)    -cos(az) sin(ax)    -cos(az) cos(ax)]
 //   [                                               ]
 // 2 [   0           cos(ax)             -sin(ax)    ]

  // work on duplicate to avoid problems.
    DDoubleGDL* t3dMatrix = Matrix->Dup();
    Guard<DDoubleGDL> guard(t3dMatrix);
 
    axisExchangeCode=NORMAL3D;
    
    // to be improved --- not useful yet
//    if (!isValidRotationMatrix(t3dMatrix, axisExchangeCode)) return false;
//    if (axisExchangeCode!=NORMAL3D) SelfExch3d(t3dMatrix,axisExchangeCode);

    //scaling
    DDouble sx=1; DDouble sy=1; DDouble sz=1;
    sx=sqrt( (*t3dMatrix)[0 * 4 + 0] * (*t3dMatrix)[0 * 4 + 0]+ (*t3dMatrix)[1 * 4 + 0]*(*t3dMatrix)[1 * 4 + 0]+ (*t3dMatrix)[2 * 4 + 0]*(*t3dMatrix)[2 * 4 + 0]);
    sy=sqrt( (*t3dMatrix)[0 * 4 + 1] * (*t3dMatrix)[0 * 4 + 1]+ (*t3dMatrix)[1 * 4 + 1]*(*t3dMatrix)[1 * 4 + 1]+ (*t3dMatrix)[2 * 4 + 1]*(*t3dMatrix)[2 * 4 + 1]);
    sz=sqrt( (*t3dMatrix)[0 * 4 + 2] * (*t3dMatrix)[0 * 4 + 2]+ (*t3dMatrix)[1 * 4 + 2]*(*t3dMatrix)[1 * 4 + 2]+ (*t3dMatrix)[2 * 4 + 2]*(*t3dMatrix)[2 * 4 + 2]);
    //unscale not necessary as we use atan2 below.
//    for (auto i=0; i<3; ++i) (*t3dMatrix)[i * 4 + 0]/=sx;
//    for (auto i=0; i<3; ++i) (*t3dMatrix)[i * 4 + 1]/=sy;
//    for (auto i=0; i<3; ++i) (*t3dMatrix)[i * 4 + 2]/=sz;
    scale[0]=sx;
    scale[1]=sy;
    scale[2]=sz;
    ay = -atan2((*t3dMatrix)[2 * 4 + 0], sqrt(pow((*t3dMatrix)[2 * 4 + 1], 2.0) + pow((*t3dMatrix)[2 * 4 + 2], 2.0))) * RADTODEG;
    az = atan2(-(*t3dMatrix)[1 * 4 + 0], (*t3dMatrix)[0 * 4 + 0]) * RADTODEG;
    alt =  atan2((*t3dMatrix)[2 * 4 + 2], (*t3dMatrix)[2 * 4 + 1]) * RADTODEG;
    below = false;
    alt = fmod((alt + 360), 360.0);
    if (alt > 90 && alt <= 270) {
      az += 180.;
      if (alt > 180) {
        below = true;
        alt -= 180;
        alt *= -1;
      } else alt = 180 - alt;
    } else if (alt > 270) {
      below = true;
      alt = -(360. - alt);
    }
//    std::cerr<<"ax="<<ax<<", az="<<az<<", ay="<<ay<<", scale x="<<sx<<", scale y="<<sy<<", scale z="<<sz<<std::endl;
    if (abs(ay) > 1E-4) return false; //not a true rotation
    return true;
  }

  //examine general !P.T like matrix and determine if it is valid for an non-tilted axes axonometric perspective, enabling
  // futher plotting of axes and surfaces by plplot.
  // Note that if drawing of axes and hidden surfaces (by plplot) are not needed (e.g., PLOTS), any matrix can be used.
  // Returns a 'plplot-compatible' matrix that will be used in calls to plplot.
  // Retunrs NULL if conversion is impossible.
  bool gdlInterpretT3DMatrixAsPlplotRotationMatrix(DDouble &az, DDouble &alt, DDouble &ay, DDouble *scale, T3DEXCHANGECODE &axisExchangeCode, bool &below) {
//    std::cerr<<"gdlInterpretT3DMatrixAsPlplotRotationMatrix(()\n";
    //returns NULL if error!
    DDoubleGDL* t3dMatrix = (new DDoubleGDL(dimension(4, 4)));
    Guard<DDoubleGDL> guard(t3dMatrix);
    //retrieve !P.T and find az, alt, inversions, and (possibly) scale and roty
    DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
    static unsigned tTag = pStruct->Desc()->TagIndex("T");
    for (int i = 0; i < t3dMatrix->N_Elements(); ++i)(*t3dMatrix)[i] = (*static_cast<DDoubleGDL*> (pStruct->GetTag(tTag, 0)))[i];
    SelfTranspose3d(t3dMatrix);
    //check if valid, get rotation etc.
    if (!isAxonometricRotation(t3dMatrix, alt, az, ay, scale, axisExchangeCode, below)) return false;
    return true;
  }
  
  void scale3_pro(EnvT* e) {
    static unsigned tTag = SysVar::P()->Desc()->TagIndex("T");
    const double invsqrt3 = 1.0 / sqrt(3.0);
    //AX
    DDouble ax = 30.0;
    static int AX = e->KeywordIx("AX");
    e->AssureDoubleScalarKWIfPresent(AX, ax);
    //AZ
    DDouble az = 30.0;
    static int AZ = e->KeywordIx("AZ");
    e->AssureDoubleScalarKWIfPresent(AZ, az);
    DDoubleGDL* mat = (new DDoubleGDL(dimension(4, 4), BaseGDL::NOZERO));
    SelfReset3d(mat);
    static DDouble mytrans[3] = {-0.5, -0.5, -0.5};
    SelfTranslate3d(mat, mytrans);
    static DDouble myscale[3] = {invsqrt3, invsqrt3, invsqrt3};
    SelfScale3d(mat, myscale);
    DDouble rot1[3] = {-90.0, az, 0.0};
    DDouble rot2[3] = {ax, 0.0, 0.0};
    SelfRotate3d(mat, rot1);
    SelfRotate3d(mat, rot2);
    static DDouble mytrans2[3] = {0.5, 0.5, 0.5};
    SelfTranslate3d(mat, mytrans2);
    SelfTranspose3d(mat);
    for (int i = 0; i < mat->N_Elements(); ++i)(*static_cast<DDoubleGDL*> (SysVar::P()->GetTag(tTag, 0)))[i] = (*mat)[i];

    DDouble size;
    //XRANGE
    static int xrangeIx = e->KeywordIx("XRANGE");
    DDoubleGDL* xrange = e->IfDefGetKWAs<DDoubleGDL>(xrangeIx);
    if (xrange != NULL) {
      if (xrange->N_Elements() < 2) e->Throw("XRANGE needs at least a 2-elements vector");
      static unsigned sTag = SysVar::X()->Desc()->TagIndex("S");
      size = ((*xrange)[1]-(*xrange)[0]);
      (*static_cast<DDoubleGDL*> (SysVar::X()->GetTag(sTag, 0)))[0] = -(*xrange)[0] / size;
      (*static_cast<DDoubleGDL*> (SysVar::X()->GetTag(sTag, 0)))[1] = 1.0 / size;
    }
    //YRANGE
    static int yrangeIx = e->KeywordIx("YRANGE");
    DDoubleGDL* yrange = e->IfDefGetKWAs<DDoubleGDL>(yrangeIx);
    if (yrange != NULL) {
      if (yrange->N_Elements() < 2) e->Throw("YRANGE needs at least a 2-elements vector");
      static unsigned sTag = SysVar::Y()->Desc()->TagIndex("S");
      size = ((*yrange)[1]-(*yrange)[0]);
      (*static_cast<DDoubleGDL*> (SysVar::Y()->GetTag(sTag, 0)))[0] = -(*yrange)[0] / size;
      (*static_cast<DDoubleGDL*> (SysVar::Y()->GetTag(sTag, 0)))[1] = 1.0 / size;
    }
    //ZRANGE
    static int zrangeIx = e->KeywordIx("ZRANGE");
    DDoubleGDL* zrange = e->IfDefGetKWAs<DDoubleGDL>(zrangeIx);
    if (zrange != NULL) {
      if (zrange->N_Elements() < 2) e->Throw("ZRANGE needs at least a 2-elements vector");
      static unsigned sTag = SysVar::Z()->Desc()->TagIndex("S");
      size = ((*zrange)[1]-(*zrange)[0]);
      (*static_cast<DDoubleGDL*> (SysVar::Z()->GetTag(sTag, 0)))[0] = -(*zrange)[0] / size;
      (*static_cast<DDoubleGDL*> (SysVar::Z()->GetTag(sTag, 0)))[1] = 1.0 / size;
    }
  }

  void t3d_pro(EnvT* e) {
    static unsigned tTag = SysVar::P()->Desc()->TagIndex("T");
    DDoubleGDL *mat = NULL;
    DDoubleGDL *matin = NULL;
    // MATRIX keyword (read, write)
    static int matrixIx = e->KeywordIx("MATRIX");
    bool externalarray = e->KeywordPresent(matrixIx);

    static int resetIx = e->KeywordIx("RESET");
    bool reset = e->KeywordSet(resetIx);
    if (e->NParam() > 1) {
      e->Throw("Accepts only one (optional) 4x4 array");
    } else if (e->NParam() == 1 && !reset) {
      matin = e->GetParAs< DDoubleGDL > (0);
      if (matin->Rank() != 2) e->Throw(e->GetParString(0) + "must be a 2d array.");
      if (matin->Dim(0) != 4 || matin->Dim(1) != 4) e->Throw(e->GetParString(0) + "must be a [4,4] array.");
      mat = matin->Dup();
    } else {
      mat = (new DDoubleGDL(dimension(4, 4)));
      for (int i = 0; i < mat->N_Elements(); ++i)(*mat)[i] = (*static_cast<DDoubleGDL*> (SysVar::P()->GetTag(tTag, 0)))[i];
    }
    SelfTranspose3d(mat); //for c matrix handling
    if (reset) SelfReset3d(mat);
    //TRANSLATE
    static int translateIx = e->KeywordIx("TRANSLATE");
    DDoubleGDL* translate = e->IfDefGetKWAs<DDoubleGDL>(translateIx);
    if (translate != NULL) {
      if (translate->N_Elements() != 3) e->Throw("TRANSLATE parameter must be a [3] array.");
      SelfTranslate3d(mat, (DDouble*) translate->DataAddr());
    }
    //SCALE
    static int scaleIx = e->KeywordIx("SCALE");
    DDoubleGDL* scale = e->IfDefGetKWAs<DDoubleGDL>(scaleIx);
    if (scale != NULL) {
      if (scale->N_Elements() != 3) e->Throw("SCALE parameter must be a [3] array.");
      SelfScale3d(mat, (DDouble*) scale->DataAddr());
    }
    //ROTATE
    static int rotateIx = e->KeywordIx("ROTATE");
    DDoubleGDL* rotate = e->IfDefGetKWAs<DDoubleGDL>(rotateIx);
    if (rotate != NULL) {
      if (rotate->N_Elements() != 3) e->Throw("ROTATE parameter must be a [3] array.");
      SelfRotate3d(mat, (DDouble*) rotate->DataAddr());
    }
    //PERSPECTIVE
    static int perspIx = e->KeywordIx("PERSPECTIVE");
    BaseGDL* perspective = e->GetKW(perspIx);
    if (perspective != NULL) {
      DDoubleGDL* persp = static_cast<DDoubleGDL*> (perspective->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      SelfPerspective3d(mat, (*persp)[0]);
    }
    //OBLIQUE
    static int obliqueIx = e->KeywordIx("OBLIQUE");
    DDoubleGDL* oblique = e->IfDefGetKWAs<DDoubleGDL>(obliqueIx);
    if (oblique != NULL) {
      if (oblique->N_Elements() != 2) e->Throw("OBLIQUE parameter must be a [2] array.");
      SelfOblique3d(mat, (*oblique)[0], (*oblique)[1]);
    }
    T3DEXCHANGECODE code=INVALID;
    //XYEXCH
    static int exchxyIx = e->KeywordIx("XYEXCH");
    bool exchxy = e->KeywordSet(exchxyIx);
    if (exchxy) code = XY;
    //XZEXCH
    static int exchxzIx = e->KeywordIx("XZEXCH");
    bool exchxz = e->KeywordSet(exchxzIx);
    if (exchxz) code = XZ;
    //YYEXCH
    static int exchyzIx = e->KeywordIx("YZEXCH");
    bool exchyz = e->KeywordSet(exchyzIx);
    if (exchyz) code = YZ;

    if (exchxy || exchxz || exchyz) SelfExch3d(mat, code);

    SelfTranspose3d(mat); //prior to give back.
    if (externalarray) {
      e->SetKW(matrixIx, mat);
    } else {
      for (int i = 0; i < mat->N_Elements(); ++i)(*static_cast<DDoubleGDL*> (SysVar::P()->GetTag(tTag, 0)))[i] = (*mat)[i];
      GDLDelete(mat);
    }
  }
} // namespace
