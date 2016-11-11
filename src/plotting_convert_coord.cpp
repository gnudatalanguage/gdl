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
#define TONORMCOORDZ( in, out, log, doT3d) out = (doT3d)? (log) ? sz[0] + sz[1] * log10(in) : sz[0] + sz[1] * in   : (log) ? log10(in)    : in;
#define TODATACOORDZ( in, out, log, doT3d) out = (doT3d)? (log) ? pow(10.0, (in -sz[0])/sz[1]) : (in -sz[0])/sz[1] : (log) ? pow(10.0,in) : in;
namespace lib {

  using namespace std;

  static DDouble cubeCorners[32]=
      {
   0,1,0,1,0,1,0,1,0,0,1,1,0,0,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1
  };

  DDoubleGDL* convert_coord_double(EnvT* e, DDoubleGDL* xVal, DDoubleGDL* yVal, DDoubleGDL* zVal) {

    typedef enum
    {
      DATA=0,
      NORMAL,
      DEVICE
    } COORDSYS; 
    
    COORDSYS icoordinateSystem=DATA, ocoordinateSystem=DATA ;
    //check presence of DATA,DEVICE and NORMAL options
    static int DATAIx=e->KeywordIx("DATA");
    static int DEVICEIx=e->KeywordIx("DEVICE");
    static int NORMALIx=e->KeywordIx("NORMAL");
    static int TO_DATAIx=e->KeywordIx("TO_DATA");
    static int TO_DEVICEIx=e->KeywordIx("TO_DEVICE");
    static int TO_NORMALIx=e->KeywordIx("TO_NORMAL");
    
    if ( e->KeywordSet(DATAIx) ) icoordinateSystem=DATA;
    if ( e->KeywordSet(DEVICEIx) ) icoordinateSystem=DEVICE;
    if ( e->KeywordSet(NORMALIx) ) icoordinateSystem=NORMAL;
    if ( e->KeywordSet(TO_DATAIx) ) ocoordinateSystem=DATA;
    if ( e->KeywordSet(TO_DEVICEIx) ) ocoordinateSystem=DEVICE;
    if ( e->KeywordSet(TO_NORMALIx) ) ocoordinateSystem=NORMAL;
    static int t3dIx = e->KeywordIx( "T3D");
    bool doT3d=(e->KeywordSet(t3dIx) || T3Denabled());   
    DLong dims[2] = {3, 0};

    DDoubleGDL* res;
    SizeT nrows;

    nrows = xVal->Dim(0);
    dims[1] = nrows;
    dimension dim((DLong *) dims, 2);
    res = new DDoubleGDL(dim, BaseGDL::NOZERO);

    //eliminate simplest case here:
    if (ocoordinateSystem==icoordinateSystem)
    {
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
      {
#pragma omp for
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
    gdlGetAxisType("X", xLog);
    gdlGetAxisType("Y", yLog);
    gdlGetAxisType("Z", zLog);
        
    int xSize, ySize;
    //give default values
    DStructGDL* dStruct = SysVar::D();
    static unsigned xsizeTag = dStruct->Desc()->TagIndex( "X_SIZE");
    static unsigned ysizeTag = dStruct->Desc()->TagIndex( "Y_SIZE");
    xSize = (*static_cast<DLongGDL*>( dStruct->GetTag( xsizeTag, 0)))[0];
    ySize = (*static_cast<DLongGDL*>( dStruct->GetTag( ysizeTag, 0)))[0];
    // Use Size in lieu of VSize
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    DLong wIx = actDevice->ActWin();
    if( wIx != -1) bool success = actDevice->WSize(wIx, &xSize, &ySize); //on failure, sizes are ot changed by WSize.
    
    //projection?
      bool mapSet=false;
#ifdef USE_LIBPROJ4
      static LPTYPE idata;
      static XYTYPE odata;
      get_mapset ( mapSet );
      if ( mapSet )
      {
        ref=map_init ();
        if ( ref==NULL ) e->Throw ( "Projection initialization failed." );
      }
#endif
      
    //convert input (we can overwrite X Y and Z) to normalized 
    switch(icoordinateSystem)
    {
      case DATA:
        // to u,v
#ifdef USE_LIBPROJ4
      if ( mapSet )
      {
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
        {
#pragma omp for private(idata,odata)
            for (SizeT i = 0; i < nrows; i++) {
              idata.u = (*xVal)[i] * DEG_TO_RAD;
              idata.v = (*yVal)[i] * DEG_TO_RAD;
              odata = PJ_FWD(idata, ref);
              (*xVal)[i] = odata.u;
              (*yVal)[i] = odata.v;
            }
          }
        }
#endif
        // to norm:
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
      {
#pragma omp for
        for (OMPInt i = 0; i < nrows; ++i) {
          TONORMCOORDX((*xVal)[i], (*xVal)[i], xLog);
          TONORMCOORDY((*yVal)[i], (*yVal)[i], yLog);
          TONORMCOORDZ((*zVal)[i], (*zVal)[i], zLog, doT3d);
        }
      }
        break;
      case DEVICE:
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
      {
#pragma omp for
        for (OMPInt i = 0; i < nrows; ++i) {
          (*xVal)[i] /= xSize;
          (*yVal)[i] /= ySize;
          // (zSize is 1)
        }
      }
    }

    //convert to output from normalized 
    switch(ocoordinateSystem)
    {
      case DATA:
        // from norm:
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
      {
#pragma omp for
        for (OMPInt i = 0; i < nrows; ++i) {
          TODATACOORDX((*xVal)[i], (*xVal)[i], xLog);
          TODATACOORDY((*yVal)[i], (*yVal)[i], yLog);
          TODATACOORDZ((*zVal)[i], (*zVal)[i], zLog, doT3d);
        }
      }
      
      // from u,v
#ifdef USE_LIBPROJ4
      if ( mapSet )
      {
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
        {
#pragma omp for private(idata,odata)
            for (SizeT i = 0; i < nrows; i++) {
              odata.u = (*xVal)[i];
              odata.v = (*yVal)[i];
              idata = PJ_INV(odata, ref);
              (*xVal)[i] = idata.u * RAD_TO_DEG;
              (*yVal)[i] = idata.v * RAD_TO_DEG;
            }
          }
        }
#endif

        break;
      case DEVICE:
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
      {
#pragma omp for
        for (OMPInt i = 0; i < nrows; ++i) {
          (*xVal)[i] *= xSize;
          (*yVal)[i] *= ySize;
          //(zSize is 1)
        }
      }
    }
    
#pragma omp parallel if (nrows >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nrows))
      {
#pragma omp for
        for (OMPInt i = 0; i < nrows; ++i) {
          (*res)[i * 3 + 0] = (*xVal)[i];
          (*res)[i * 3 + 1] = (*yVal)[i];
          (*res)[i * 3 + 2] = (*zVal)[i];
        }
      }
    return res;
  }

  BaseGDL* convert_coord( EnvT* e)
  {
    DDoubleGDL* xVal, *yVal, *zVal;
    Guard<DDoubleGDL> xval_guard, yval_guard, zval_guard;
    SizeT xEl, yEl, zEl, minEl, xDim, yDim, zDim;

    //behaviour: 1 argument: needs to be [2,*] or [3,*] else 2 args: X,vector, Y vector 1 (z=vector zero). else 3 args, 3 vectors.
    //in case of vectors, note usual behaviour:
    //minimum set of dimensions of arrays. singletons expanded to dimension,

    //T3D
    static int t3dIx = e->KeywordIx( "T3D");
    bool doT3d=(e->KeywordSet(t3dIx) || T3Denabled());
    DDoubleGDL *xValou;
    DDoubleGDL *yValou;
    Guard<BaseGDL> xvalou_guard, yvalou_guard;
    
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0;
    BaseGDL* p1;
    BaseGDL* p2;
    
    DType type=GDL_FLOAT;
    static int doubleIx = e->KeywordIx( "DOUBLE");
    if (e->KeywordSet(doubleIx)) type=GDL_DOUBLE;

    p0 = e->GetParDefined( 0);
    if (p0->Type() == GDL_DOUBLE) type=GDL_DOUBLE;

    if( e->NParam() == 1) {
      if (p0->Dim(0) == 0) e->Throw( "Expression must be an array in this context: "+e->GetParString(0));
      if (p0->Dim(0) != 2 && p0->Dim(0) != 3) e->Throw( "When only 1 param, dims must be (2,n) or (3,n)"); //with n=0 also!!!
      SizeT dim0=p0->Dim(0);
      minEl=p0->Dim(1);
      if (minEl==0) minEl=1; // aka in convert_coord([44,22])-->n_dim=1,dim=2
      xVal=new DDoubleGDL(dimension(minEl), BaseGDL::NOZERO);
      xval_guard.Reset(xVal); // delete upon exit
      DDoubleGDL* tmpVal=e->GetParAs< DDoubleGDL>(0);
      for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpVal)[i*dim0+0];
      yVal=new DDoubleGDL(dimension(minEl), BaseGDL::NOZERO);
      yval_guard.Reset(yVal); // delete upon exit
      for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpVal)[i*dim0+1];
      zVal=new DDoubleGDL(dimension(minEl), BaseGDL::ZERO);
      zval_guard.Reset(zVal); // delete upon exit
      if (dim0==3) for (SizeT i=0; i< minEl ; ++i) (*zVal)[i]=(*tmpVal)[i*dim0+2];
    }

    if (nParam >= 2) {
      p1 = e->GetParDefined( 1);
      if (p1->Type() == GDL_DOUBLE) type=GDL_DOUBLE;
      xVal=e->GetParAs< DDoubleGDL>(0);
      xEl=xVal->N_Elements();
      xDim=xVal->Dim(0);
      yVal=e->GetParAs< DDoubleGDL>(1);
      yEl=yVal->N_Elements();
      yDim=yVal->Dim(0);
      //minEl:
      minEl=-1;
      minEl=(minEl>xEl&&xDim)?xEl:minEl;
      minEl=(minEl>yEl&&yDim)?yEl:minEl;
      
      if (minEl==-1){
        minEl=1;
      } else {
        minEl=(minEl>xEl&&xDim)?xEl:minEl;
        minEl=(minEl>yEl&&yDim)?yEl:minEl;
      }
      DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
      xVal=new DDoubleGDL(minEl, BaseGDL::NOZERO); 
      xval_guard.Reset(xVal); // delete upon exit
      if (xDim) for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[i]; else for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
      DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
      yVal=new DDoubleGDL(minEl, BaseGDL::NOZERO); 
      yval_guard.Reset(yVal); // delete upon exit
      if (yDim) for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[i]; else for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
      zVal=new DDoubleGDL(minEl, BaseGDL::ZERO); 
      zval_guard.Reset(zVal); // delete upon exit
    }

    if (nParam == 3) {
      p2 = e->GetParDefined( 2);
      if (p2->Type() == GDL_DOUBLE) type=GDL_DOUBLE;
      xVal=e->GetParAs< DDoubleGDL>(0);
      xEl=xVal->N_Elements();
      xDim=xVal->Dim(0);
      yVal=e->GetParAs< DDoubleGDL>(1);
      yEl=yVal->N_Elements();
      yDim=yVal->Dim(0);
      zVal=e->GetParAs<DDoubleGDL>(2);
      zEl=zVal->N_Elements();
      zDim=zVal->Dim(0);
      //minEl:
      minEl=-1;
      minEl=(minEl>xEl&&xDim)?xEl:minEl;
      minEl=(minEl>yEl&&yDim)?yEl:minEl;
      minEl=(minEl>zEl&&zDim)?zEl:minEl;
      
      if (minEl==-1){
        minEl=1;
      } else {
        minEl=(minEl>xEl&&xDim)?xEl:minEl;
        minEl=(minEl>yEl&&yDim)?yEl:minEl;
        minEl=(minEl>zEl&&zDim)?zEl:minEl;
      }
      DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
      xVal=new DDoubleGDL(minEl, BaseGDL::NOZERO); 
      xval_guard.Reset(xVal); // delete upon exit
      if (xDim) for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[i]; else for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
      DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
      yVal=new DDoubleGDL(minEl, BaseGDL::NOZERO); 
      yval_guard.Reset(yVal); // delete upon exit
      if (yDim) for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[i]; else for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
      DDoubleGDL* tmpzVal=e->GetParAs< DDoubleGDL>(2);
      zVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
      zval_guard.Reset(zVal); // delete upon exit
      if (zDim) for (SizeT i=0; i< minEl ; ++i) (*zVal)[i]=(*tmpzVal)[i]; else for (SizeT i=0; i< minEl ; ++i) (*zVal)[i]=(*tmpzVal)[0];       
    }

    if ( doT3d ) //convert X,Y,Z in X',Y' as per T3D perspective.
    {
      DDoubleGDL* t3dMatrix;
      Guard<BaseGDL> t3dMatrix_guard;
      t3dMatrix=gdlGetT3DMatrix(); //the original one
      t3dMatrix_guard.Reset(t3dMatrix);
      DDouble *sx, *sy, *sz;
      GetSFromPlotStructs(&sx, &sy, &sz);
      xValou=new DDoubleGDL(dimension(minEl));
      yValou=new DDoubleGDL(dimension(minEl));
      Guard<BaseGDL> xval_guard, yval_guard;
      xval_guard.reset(xValou);
      yval_guard.reset(yValou);
      gdlProject3dCoordinatesIn2d(t3dMatrix, xVal, sx, yVal, sy, zVal, sz, xValou, yValou);
      xVal=xValou;
      yVal=yValou;
    }
    
    DDoubleGDL* res=convert_coord_double( e, xVal, yVal, zVal);

    if (type == GDL_DOUBLE) {
      return res;
    } else if (type == GDL_FLOAT) {
      DFloatGDL* res1 = static_cast<DFloatGDL*>
              (res->Convert2(GDL_FLOAT, BaseGDL::COPY));
      delete res;
      return res1;
    }
  }

  //THE FOLLOWING ARE POSSIBLY THE WORST WAY TO DO THE JOB. At least they are to be used *only*
  //for [4,4] generalized 3D matrices
  void SelfTranspose3d(DDoubleGDL* me)
  {
    //crude quick hack to have the same behaviour as the other functions.
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim1,dim0),BaseGDL::NOZERO));
    for (int j=0; j < dim0; ++j) for (int i=0; i < dim1; ++i)(*mat)[i*dim1+j]=(*me)[j*dim0 + i];
    memcpy(me->DataAddr(),mat->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(mat);
  }
  void SelfReset3d(DDoubleGDL* me)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* Identity=(new DDoubleGDL(dimension(dim0,dim1)));
    for(SizeT i=0; i<dim1; ++i) {(*Identity)[i*dim1+i]=(double)1.0;}
    memcpy(me->DataAddr(),Identity->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(Identity);
  }
  DDoubleGDL* Translate3d(DDoubleGDL* me, DDouble* trans)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    Guard<BaseGDL> mat_guard;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim0,dim1)));
    mat_guard.Reset(mat);
    SelfReset3d(mat); //identity Matrix
    for(SizeT i=0; i<3; ++i) {(*mat)[3*dim1+i]=trans[i];}
    return mat->MatrixOp(me,false,false);
  }
  void SelfTranslate3d(DDoubleGDL* me, DDouble* trans)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim0,dim1)));
    SelfReset3d(mat); //identity Matrix
    for(SizeT i=0; i<3; ++i) {(*mat)[3*dim1+i]=trans[i];}
    DDoubleGDL* intermediary=mat->MatrixOp(me,false,false);
    memcpy(me->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(intermediary);
    GDLDelete(mat);
  }
  DDoubleGDL* Scale3d(DDoubleGDL* me, DDouble *scale)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    Guard<BaseGDL> mat_guard;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim0,dim1)));
    mat_guard.Reset(mat);
    SelfReset3d(mat); //identity Matrix
    for(SizeT i=0; i<3; ++i) {(*mat)[i*dim1+i]=scale[i];}
    return mat->MatrixOp(me,false,false);
  }
  void SelfScale3d(DDoubleGDL* me, DDouble *scale)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim0,dim1)));
    SelfReset3d(mat); //identity Matrix
    for(SizeT i=0; i<3; ++i) {(*mat)[i*dim1+i]=scale[i];}
    DDoubleGDL* intermediary=mat->MatrixOp(me,false,false);
    memcpy(me->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(intermediary);
    GDLDelete(mat);
}
#define DPI (double)(4*atan(1.0))
#define DEGTORAD (DPI/180.0)
#define RADTODEG (180.0/DPI)
  void SelfRotate3d(DDoubleGDL* me, DDouble *rot)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(4,4)));
    SelfReset3d(mat);
    DDoubleGDL* maty=(new DDoubleGDL(dimension(4,4)));
    SelfReset3d(maty);
    DDoubleGDL* matz=(new DDoubleGDL(dimension(4,4)));
    SelfReset3d(matz);
    SizeT ncols=4;
    double c,s;
    for(SizeT j=0; j<3; ++j)
    {
        c=cos(rot[j]*DEGTORAD);
        s=sin(rot[j]*DEGTORAD);
        switch(j)
        {
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
            DDoubleGDL* intermediary=maty->MatrixOp(mat,false,false);
            memcpy(mat->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
            GDLDelete(intermediary);
            break;
          }
          case 2:
          {
            (*matz)[0 * ncols + 0] = c;
            (*matz)[0 * ncols + 1] = s;
            (*matz)[1 * ncols + 0] = -s;
            (*matz)[1 * ncols + 1] = c;
            DDoubleGDL* intermediary=matz->MatrixOp(mat,false,false);
            memcpy(mat->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
            GDLDelete(intermediary);
          }
        }
      }

    DDoubleGDL* intermediary=mat->MatrixOp(me,false,false);
    memcpy(me->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(intermediary);
    GDLDelete(matz);
    GDLDelete(maty);
    GDLDelete(mat);
  }
  void SelfPerspective3d(DDoubleGDL* me, DDouble zdist)
  {
    if (!isfinite(zdist)) return; //Nan
    if (zdist==0.0) return;
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim0,dim1)));
    SelfReset3d(mat); //identity Matrix
    (*mat)[2*dim1+3]=-1.0/zdist;
    DDoubleGDL* intermediary=mat->MatrixOp(me,false,false);
    memcpy(me->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(intermediary);
    GDLDelete(mat);
 }
  void SelfOblique3d(DDoubleGDL* me, DDouble dist, DDouble angle)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=(new DDoubleGDL(dimension(dim0,dim1)));
    SelfReset3d(mat); //identity Matrix
    (*mat)[2*dim1+2]=0.0;
    (*mat)[2*dim1+0]=dist*cos(angle*DEGTORAD);
    (*mat)[2*dim1+1]=dist*sin(angle*DEGTORAD);
    DDoubleGDL* intermediary=mat->MatrixOp(me,false,false);
    memcpy(me->DataAddr(),intermediary->DataAddr(),dim0*dim1*sizeof(double));
    GDLDelete(intermediary);
    GDLDelete(mat);
  }
  void SelfExch3d(DDoubleGDL* me, DLong code)
  {
    SizeT dim0=me->Dim(0);
    SizeT dim1=me->Dim(1);
    if (dim0 !=4 && dim1 !=4) return;
    DDoubleGDL* mat=me->Dup();
    switch(code)
    {
      case 1: //exchange 0 and 1
        for(SizeT i=0; i<dim0; ++i)
        {
          (*me)[0*dim1+i]=(*mat)[1*dim1+i];
          (*me)[1*dim1+i]=(*mat)[0*dim1+i];
        }
        break;
      case 2: //exchange 0 and 2
        for(SizeT i=0; i<dim0; ++i)
        {
          (*me)[0*dim1+i]=(*mat)[2*dim1+i];
          (*me)[2*dim1+i]=(*mat)[0*dim1+i];
        }
        break;
      case 12: //exchange 1 and 2
        for(SizeT i=0; i<dim0; ++i)
        {
          (*me)[1*dim1+i]=(*mat)[2*dim1+i];
          (*me)[2*dim1+i]=(*mat)[1*dim1+i];
        }
    }
    GDLDelete(mat);
  }
 DDoubleGDL* gdlComputePlplotRotationMatrix(DDouble az, DDouble alt, DDouble zValue, DDouble scale)
  {
    //scale should not be used and must be 1 until scale is introduced everywhre.
    DDoubleGDL* plplot3d=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
    SelfReset3d(plplot3d);
    static DDouble mytrans[3]={-0.5, -0.5, -zValue};
    SelfTranslate3d(plplot3d,mytrans);
    static DDouble myscale[3]={scale, scale, scale};
    SelfScale3d(plplot3d,myscale);
    DDouble rot1[3]={-90.0, az, 0.0};
    DDouble rot2[3]={alt, 0.0, 0.0};
    SelfRotate3d(plplot3d,rot1);
    SelfRotate3d(plplot3d,rot2);
//    static DDouble mytrans2[3]={0.5, 0.5, zValue};
//    SelfTranslate3d(plplot3d,mytrans2);
    return plplot3d; //this matrix converts 3D xyz in plplot's 2d
 }

 void gdl3dTo2dTransform(PLFLT x, PLFLT y, PLFLT *xt, PLFLT *yt, PLPointer data)
 {
   struct GDL_3DTRANSFORMDATA *ptr = (GDL_3DTRANSFORMDATA* )data;
   DDoubleGDL* xyzw=new DDoubleGDL(dimension(4));
   (*xyzw)[3]=1.0;
   (*xyzw)[ptr->code[0]]=(x+ptr->x0)*ptr->xs;
   (*xyzw)[ptr->code[1]]=(y+ptr->y0)*ptr->ys;;
   (*xyzw)[ptr->code[2]]=ptr->zValue;
   DDoubleGDL* trans=xyzw->MatrixOp(ptr->Matrix,false,true);
   *xt=(*trans)[0];
   *yt=(*trans)[1];
   GDLDelete(trans);
   GDLDelete(xyzw);
 }
//Special for Contour (not special for the moment in fact):
 void gdl3dTo2dTransformContour(PLFLT x, PLFLT y, PLFLT *xt, PLFLT *yt, PLPointer data)
 {
   struct GDL_3DTRANSFORMDATA *ptr = (GDL_3DTRANSFORMDATA* )data;
   DDoubleGDL* xyzw=new DDoubleGDL(dimension(4));
   (*xyzw)[3]=1.0;
   (*xyzw)[ptr->code[0]]=(x+ptr->x0)*ptr->xs;
   (*xyzw)[ptr->code[1]]=(y+ptr->y0)*ptr->ys;;
   (*xyzw)[ptr->code[2]]=ptr->zValue;
   DDoubleGDL* trans=xyzw->MatrixOp(ptr->Matrix,false,true);
   *xt=(*trans)[0];
   *yt=(*trans)[1];
   GDLDelete(trans);
   GDLDelete(xyzw);
 }

 //retrieve !P.T,
 DDoubleGDL* gdlGetT3DMatrix()
 {
    DDoubleGDL* t3dMatrix=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    static unsigned tTag=pStruct->Desc()->TagIndex("T");
    for (int i=0; i<t3dMatrix->N_Elements(); ++i )(*t3dMatrix)[i]=(*static_cast<DDoubleGDL*>(pStruct->GetTag(tTag, 0)))[i];
    SelfTranspose3d(t3dMatrix);
    return t3dMatrix;
 }
 // retrieve !P.T, (or use passed matrix)
 // scale to current X.S Y.S and Z.S, returns a matrix that can be applied directly to
 // XYZ data to get projected X' Y' *normalized* coordinates values
 DDoubleGDL* gdlGetScaledNormalizedT3DMatrix(DDoubleGDL* passedMatrix)
 {
    DDoubleGDL* t3dMatrix;
    if (passedMatrix==NULL) t3dMatrix=gdlGetT3DMatrix(); else t3dMatrix=passedMatrix;
    DDouble *sx, *sy, *sz;
    GetSFromPlotStructs(&sx, &sy, &sz);
    DDoubleGDL* toScaled=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
    SelfReset3d(toScaled);
    DDouble depla[3]={sx[0],sy[0],sz[0]};
    DDouble scale[3]={sx[1],sy[1],sz[1]};
    SelfScale3d(toScaled, scale);  //pay attention to order for matrices!
    SelfTranslate3d(toScaled,depla);
    DDoubleGDL* returnMatrix=t3dMatrix->MatrixOp(toScaled,false,false);
    GDLDelete(toScaled);
    if (passedMatrix==NULL) GDLDelete(t3dMatrix);
    return returnMatrix;
 }

 void gdlNormed3dToWorld3d(DDoubleGDL *xValin, DDoubleGDL *yValin, DDoubleGDL* zValin,
                           DDoubleGDL *xValou, DDoubleGDL *yValou, DDoubleGDL* zValou)
 {
    DDouble *sx, *sy, *sz;
    GetSFromPlotStructs(&sx, &sy, &sz);
    DDoubleGDL* toWorld=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
    SelfReset3d(toWorld);
    DDouble depla[3]={-sx[0],-sy[0],-sz[0]};
    DDouble scale[3]={1/sx[1],1/sy[1],1/sz[1]};
    SelfTranslate3d(toWorld,depla);  //pay attention to order for matrices!
    SelfScale3d(toWorld, scale);
    //populate a 4D matrix with reduced coordinates through sx,sy,sz:
    SizeT nEl=xValin->N_Elements();
    DDoubleGDL* xyzw=new DDoubleGDL(dimension(nEl,4));
    memcpy(&((*xyzw)[0]),xValin->DataAddr(),nEl*sizeof(double));
    memcpy(&((*xyzw)[nEl]),yValin->DataAddr(),nEl*sizeof(double));
    if (zValin != NULL) memcpy(&((*xyzw)[2*nEl]),zValin->DataAddr(),nEl*sizeof(double));
    else for (int index=0; index< nEl; ++index){ (*xyzw)[2*nEl+index]=1.0;}
    for (int index=0; index< nEl; ++index){ (*xyzw)[3*nEl+index]=1.0;}
    DDoubleGDL* trans=xyzw->MatrixOp(toWorld,false,true);
    memcpy(xValou->DataAddr(), trans->DataAddr(),nEl*sizeof(double));
    memcpy(yValou->DataAddr(), &(*trans)[nEl],nEl*sizeof(double));
    if (zValou != NULL) memcpy(zValou->DataAddr(), &(*trans)[2*nEl],nEl*sizeof(double));
    GDLDelete(trans);
    GDLDelete(xyzw);
    GDLDelete(toWorld);
 }
 void gdl3dto2dProjectDDouble(DDoubleGDL* t3dMatrix, DDoubleGDL *xVal, DDoubleGDL *yVal, DDoubleGDL* zVal,
                              DDoubleGDL *xValou, DDoubleGDL *yValou, int* code)
 {
      DDoubleGDL *decodedAxis[3]={xVal,yVal,zVal};
      int *localCode=code;
      if (localCode == NULL) localCode=code012;
      //populate a 4D matrix with reduced coordinates through sx,sy,sz:
      SizeT nEl=xVal->N_Elements();
      DDoubleGDL* xyzw=new DDoubleGDL(dimension(nEl,4));
      memcpy(&((*xyzw)[0]),decodedAxis[localCode[0]]->DataAddr(),nEl*sizeof(double));
      memcpy(&((*xyzw)[nEl]),decodedAxis[localCode[1]]->DataAddr(),nEl*sizeof(double));
      memcpy(&((*xyzw)[2*nEl]),decodedAxis[localCode[2]]->DataAddr(),nEl*sizeof(double));
      for (int index=0; index< nEl; ++index){ (*xyzw)[3*nEl+index]=1.0;}
      DDoubleGDL* trans=xyzw->MatrixOp(t3dMatrix,false,true);
      memcpy(xValou->DataAddr(), trans->DataAddr(),nEl*sizeof(double));
      memcpy(yValou->DataAddr(), &(*trans)[nEl],nEl*sizeof(double));
      GDLDelete(trans);
      GDLDelete(xyzw);
 }

  void gdlProject3dCoordinatesIn2d(DDoubleGDL* Matrix, DDoubleGDL *xVal, DDouble* sx,
      DDoubleGDL *yVal, DDouble *sy, DDoubleGDL* zVal, DDouble *sz , DDoubleGDL *xValou, DDoubleGDL *yValou)
 {
      DDoubleGDL* toScaled=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
      SelfReset3d(toScaled);
      DDouble depla[3]={sx[0],sy[0],sz[0]};
      DDouble scale[3]={sx[1],sy[1],sz[1]};
      SelfScale3d(toScaled, scale);
      SelfTranslate3d(toScaled,depla);
      //populate a 4D matrix with reduced coordinates through sx,sy,sz:
      SizeT nEl=xVal->N_Elements();
      DDoubleGDL* xyzw=new DDoubleGDL(dimension(nEl,4));
      memcpy(&((*xyzw)[0]),xVal->DataAddr(),nEl*sizeof(double));
      memcpy(&((*xyzw)[nEl]),yVal->DataAddr(),nEl*sizeof(double));
      memcpy(&((*xyzw)[2*nEl]),zVal->DataAddr(),nEl*sizeof(double));
      for (int index=0; index< nEl; ++index){ (*xyzw)[3*nEl+index]=1.0;}
      DDoubleGDL* temp=Matrix->MatrixOp(toScaled,false,false);
      DDoubleGDL* trans=xyzw->MatrixOp(temp,false,true);
      memcpy(xValou->DataAddr(), trans->DataAddr(),nEl*sizeof(double));
      memcpy(yValou->DataAddr(), &(*trans)[nEl],nEl*sizeof(double));
      GDLDelete(trans);
      GDLDelete(xyzw);
      GDLDelete(temp);
 }

  bool isMatrixRotation(DDoubleGDL* Matrix,DDouble &rx, DDouble &ry, DDouble &rz, DDouble &scale)
  {
    DDoubleGDL* t3dMatrix=Matrix->Dup();
    // !P.T=rt#cs#9r#Ry#Rx(#Rz?)#tr !Ry contains az!
    // r9#sc#tr# rt#cs#9r#Ry#Rx(#Rz?)#tr #rt =  r9#sc#tr#!P.T#rt = Ry#Rx(#Rz?)
    //
    // a= r9#sc#tr#!P.T#rt
    //construct derotator of Matrix=!P.T . We can find sc if not stretch.
    //substract translation rt
    static DDouble rt[3]={-0.5, -0.5, -0.5};
    SelfTranslate3d(t3dMatrix,rt); //!P.T#rt
    //on the other end compute the good invert translation-rotation t3dMatrix
    DDoubleGDL* test=(new DDoubleGDL(dimension(4,4)));
    SelfReset3d(test);
    static DDouble r9[3]={90.0, 0.0, 0.0};
    SelfRotate3d(test,r9);
    static DDouble tr[3]={0.5, 0.5, 0.5};
    SelfTranslate3d(test,tr);
    // product of the two should be a pure scaled rotx,roty(rotz)(scale) matrix, hence:
    DDoubleGDL* xz=(t3dMatrix->MatrixOp(test,false,false));
    rx=atan2((*xz)[1*4+2],(*xz)[1*4+1])*RADTODEG;
    ry=atan2((*xz)[2*4+0],sqrt(pow((*xz)[2*4+1],2.0)+pow((*xz)[2*4+2],2.0)))*RADTODEG;
    rz=atan2((*xz)[1*4+0],(*xz)[0*4+0])*RADTODEG;
    //test by rotation inverse
    static DDouble Rot[3];
    memset(Rot,'\0',3*sizeof(DDouble)); Rot[2]=-rz; SelfRotate3d(xz,Rot); //#zR
    memset(Rot,'\0',3*sizeof(DDouble)); Rot[0]=-rx; SelfRotate3d(xz,Rot); //#xR
    memset(Rot,'\0',3*sizeof(DDouble)); Rot[1]=-ry; SelfRotate3d(xz,Rot); //#yR
    scale=(*xz)[0];
    DDouble sum=(*xz)[0]+(*xz)[5]+(*xz)[10];
    sum/=scale; //sum of scaled Rotation matrix diagonal
    if (abs(sum-3.0)<1E-4) return true; else return false;
  }
  DDoubleGDL* gdlConvertT3DMatrixToPlplotRotationMatrix( DDouble zValue, DDouble &az,
      DDouble &alt,  DDouble &ay, DDouble &scale, ORIENTATION3D &code)
  {
    //returns NULL if error!
    DDoubleGDL* t3dMatrix=(new DDoubleGDL(dimension(4,4)));
    //retrieve !P.T and find az, alt, inversions, and (possibly) scale and roty
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    static unsigned tTag=pStruct->Desc()->TagIndex("T");
    for (int i=0; i<t3dMatrix->N_Elements(); ++i )(*t3dMatrix)[i]=(*static_cast<DDoubleGDL*>(pStruct->GetTag(tTag, 0)))[i];
    SelfTranspose3d(t3dMatrix);
    //check repeatedly rotations & translations
    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
    {
      code=NORMAL3D;   goto done; // 0
    }
    SelfExch3d(t3dMatrix,01); //XY, 1
    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
    {
      code=XY;   goto done;
    }
    SelfExch3d(t3dMatrix,01); //-XY
    SelfExch3d(t3dMatrix,02); //+XZ 2
    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
    {
      code=XZ;   goto done;
    }
    SelfExch3d(t3dMatrix,02); //-XZ
    SelfExch3d(t3dMatrix,12); //+YZ 3
    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
    {
      code=YZ;   goto done;
    }
    SelfExch3d(t3dMatrix,12); //-YZ

    SelfExch3d(t3dMatrix,01); //XY first

    SelfExch3d(t3dMatrix,02); //+XZ 5
    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
    {
      code=XZXY;   goto done;
    }
    SelfExch3d(t3dMatrix,02); //-XZ
    SelfExch3d(t3dMatrix,12); //+YZ 4
    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
    {
      code=XZYZ;   goto done;
    }
    SelfExch3d(t3dMatrix,12); //-YZ
    SelfExch3d(t3dMatrix,01); //-XY
//redundant
//    SelfExch3d(t3dMatrix,12); // YZ first
//
//    SelfExch3d(t3dMatrix,01); //XY 5
//    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
//    {
//      code=YZXY;   goto done;
//    }
//    SelfExch3d(t3dMatrix,01); //-XY
//    SelfExch3d(t3dMatrix,02); //+XZ 4
//    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
//    {
//      code=YZXZ;   goto done;
//    }
//    SelfExch3d(t3dMatrix,02); //-XZ
//    SelfExch3d(t3dMatrix,12); //-YZ
//
//    SelfExch3d(t3dMatrix,02); // XZ first
//
//    SelfExch3d(t3dMatrix,01); //XY 4
//    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
//    {
//      code=XZXY;   goto done;
//    }
//    SelfExch3d(t3dMatrix,01); //-XY
//    SelfExch3d(t3dMatrix,12); //+YZ 4
//    if (isMatrixRotation(t3dMatrix,alt,az,ay,scale))
//    {
//      code=XZYZ;   goto done;
//    }
    return (DDoubleGDL*)(NULL); //ERROR!
done:
    if (alt > 90.0 || alt <-1.E-3) return (DDoubleGDL*)(NULL);
    if (alt<0.0) alt=0.0; //prevents plplot complain for epsilon not being strictly positive.

    //recompute transformation matrix with plplot conventions:
    DDoubleGDL* plplot3d=gdlComputePlplotRotationMatrix(az, alt, zValue, scale);
    return plplot3d;
  }
  void scale3_pro(EnvT* e)
  {
    static unsigned tTag=SysVar::P()->Desc()->TagIndex("T");
    const double invsqrt3=1.0/sqrt(3.0);
    //AX
    DDouble ax=30.0;
    static int AX=e->KeywordIx("AX");
    e->AssureDoubleScalarKWIfPresent(AX, ax);
    //AZ
    DDouble az=30.0;
    static int AZ=e->KeywordIx("AZ");
    e->AssureDoubleScalarKWIfPresent(AZ, az);
    DDoubleGDL* mat=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
    SelfReset3d(mat);
    static DDouble mytrans[3]={-0.5, -0.5, -0.5};
    SelfTranslate3d(mat,mytrans);
    static DDouble myscale[3]={invsqrt3, invsqrt3, invsqrt3};
    SelfScale3d(mat,myscale);
    DDouble rot1[3]={-90.0, az, 0.0};
    DDouble rot2[3]={ax, 0.0, 0.0};
    SelfRotate3d(mat,rot1);
    SelfRotate3d(mat,rot2);
    static DDouble mytrans2[3]={0.5, 0.5, 0.5};
    SelfTranslate3d(mat,mytrans2);
    SelfTranspose3d(mat); 
    for (int i=0; i<mat->N_Elements(); ++i )(*static_cast<DDoubleGDL*>(SysVar::P()->GetTag(tTag, 0)))[i]=(*mat)[i];

    DDouble size;
    //XRANGE
    static int xrangeIx = e->KeywordIx( "XRANGE");
    DDoubleGDL* xrange = e->IfDefGetKWAs<DDoubleGDL>( xrangeIx);
    if (xrange != NULL){
        if (xrange->N_Elements()<2) e->Throw("XRANGE needs at least a 2-elements vector");
        static unsigned sTag=SysVar::X()->Desc()->TagIndex("S");
        size=((*xrange)[1]-(*xrange)[0]);
        (*static_cast<DDoubleGDL*>(SysVar::X()->GetTag(sTag, 0)))[0]=-(*xrange)[0]/size;
        (*static_cast<DDoubleGDL*>(SysVar::X()->GetTag(sTag, 0)))[1]=1.0/size;
      }
    //YRANGE
    static int yrangeIx = e->KeywordIx( "YRANGE");
    DDoubleGDL* yrange = e->IfDefGetKWAs<DDoubleGDL>( yrangeIx);
    if (yrange != NULL){
        if (yrange->N_Elements()<2) e->Throw("YRANGE needs at least a 2-elements vector");
        static unsigned sTag=SysVar::Y()->Desc()->TagIndex("S");
        size=((*yrange)[1]-(*yrange)[0]);
        (*static_cast<DDoubleGDL*>(SysVar::Y()->GetTag(sTag, 0)))[0]=-(*yrange)[0]/size;
        (*static_cast<DDoubleGDL*>(SysVar::Y()->GetTag(sTag, 0)))[1]=1.0/size;
      }
    //ZRANGE
    static int zrangeIx = e->KeywordIx( "ZRANGE");
    DDoubleGDL* zrange = e->IfDefGetKWAs<DDoubleGDL>( zrangeIx);
    if (zrange != NULL){
      if (zrange->N_Elements()<2) e->Throw("ZRANGE needs at least a 2-elements vector");
        static unsigned sTag=SysVar::Z()->Desc()->TagIndex("S");
        size=((*zrange)[1]-(*zrange)[0]);
        (*static_cast<DDoubleGDL*>(SysVar::Z()->GetTag(sTag, 0)))[0]=-(*zrange)[0]/size;
        (*static_cast<DDoubleGDL*>(SysVar::Z()->GetTag(sTag, 0)))[1]=1.0/size;
      }
  }
  void t3d_pro( EnvT* e)
  {
    static unsigned tTag=SysVar::P()->Desc()->TagIndex("T");
    DDoubleGDL *mat=NULL;
    DDoubleGDL *matin=NULL;
    // MATRIX keyword (read, write)
    static int matrixIx=e->KeywordIx("MATRIX");
    bool externalarray=e->KeywordPresent(matrixIx);

    static int resetIx = e->KeywordIx( "RESET");
    bool reset=e->KeywordSet(resetIx);
    if (e->NParam() > 1)
    {
        e->Throw("Accepts only one (optional) 4x4 array");
    }
    else  if (e->NParam() == 1 && !reset)
    {
      matin=e->GetParAs< DDoubleGDL > (0);
      if (matin->Rank() != 2)  e->Throw(e->GetParString(0)+"must be a 2d array.");
      if (matin->Dim(0) != 4 || matin->Dim(1) != 4) e->Throw(e->GetParString(0)+"must be a [4,4] array.");
      mat=matin->Dup();
    }
    else
    {
      mat=(new DDoubleGDL(dimension(4,4)));
      for (int i=0; i<mat->N_Elements(); ++i )(*mat)[i]=(*static_cast<DDoubleGDL*>(SysVar::P()->GetTag(tTag, 0)))[i];
    }
    SelfTranspose3d(mat); //for c matrix handling
    if (reset) SelfReset3d(mat);
    //TRANSLATE
    static int translateIx = e->KeywordIx( "TRANSLATE");
    DDoubleGDL* translate = e->IfDefGetKWAs<DDoubleGDL>( translateIx);
    if (translate != NULL)
    {
      if (translate->N_Elements() != 3) e->Throw("TRANSLATE parameter must be a [3] array.");
      SelfTranslate3d(mat, (DDouble*)translate->DataAddr());
    }
    //SCALE
    static int scaleIx = e->KeywordIx( "SCALE");
    DDoubleGDL* scale = e->IfDefGetKWAs<DDoubleGDL>( scaleIx);
    if (scale != NULL)
    {
      if (scale->N_Elements() != 3) e->Throw("SCALE parameter must be a [3] array.");
      SelfScale3d(mat, (DDouble*)scale->DataAddr());
    }
    //ROTATE
    static int rotateIx = e->KeywordIx( "ROTATE");
    DDoubleGDL* rotate = e->IfDefGetKWAs<DDoubleGDL>( rotateIx);
    if (rotate != NULL)
    {
      if (rotate->N_Elements() != 3) e->Throw("ROTATE parameter must be a [3] array.");
      SelfRotate3d(mat, (DDouble*)rotate->DataAddr());
    }
    //PERSPECTIVE
    static int perspIx = e->KeywordIx("PERSPECTIVE");
    BaseGDL* perspective=e->GetKW(perspIx);
    if (perspective != NULL) 
    { DDoubleGDL* persp= static_cast<DDoubleGDL*>(perspective->Convert2( GDL_DOUBLE, BaseGDL::COPY));
      SelfPerspective3d(mat, (*persp)[0]);
    }
    //OBLIQUE
    static int obliqueIx = e->KeywordIx( "OBLIQUE");
    DDoubleGDL* oblique = e->IfDefGetKWAs<DDoubleGDL>( obliqueIx);
    if (oblique != NULL)
    {
      if (oblique->N_Elements() != 2) e->Throw("OBLIQUE parameter must be a [2] array.");
      SelfOblique3d(mat, (*oblique)[0],(*oblique)[1]);
    }
    DLong code;
    //XYEXCH
    static int exchxyIx = e->KeywordIx( "XYEXCH");
    bool exchxy=e->KeywordSet(exchxyIx);
    if (exchxy) code=01;
    //XZEXCH
    static int exchxzIx = e->KeywordIx( "XZEXCH");
    bool exchxz=e->KeywordSet(exchxzIx);
    if (exchxz) code=02;
    //YYEXCH
    static int exchyzIx = e->KeywordIx( "YZEXCH");
    bool exchyz=e->KeywordSet(exchyzIx);
    if (exchyz) code=12;

    if (exchxy||exchxz||exchyz) SelfExch3d(mat, code );

    SelfTranspose3d(mat); //prior to give back.
    if ( externalarray )
    {
       e->SetKW(matrixIx, mat);
    }
    else
    {
      for (int i=0; i<mat->N_Elements(); ++i )(*static_cast<DDoubleGDL*>(SysVar::P()->GetTag(tTag, 0)))[i]=(*mat)[i];
      GDLDelete(mat);
    }
  }
} // namespace
