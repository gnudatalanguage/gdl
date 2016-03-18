/* *************************************************************************
                          plotting.hpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOTTING_HPP_
#define PLOTTING_HPP_
#define gdlPlot_Min(a, b) ((a) < (b) ? (a) : (b))
#define gdlPlot_Max(a, b) ((a) > (b) ? (a) : (b))


//To debug Affine 3D homogenous projections matrices.
//IDL define a matrix as  M[ncol,mrow] and print as such. However col_major and
//row_major refer to the math notation M[row,col] where row=dim(0) and col=dim(1).
//Matrices are stored COL Major in IDL/Fortran and ROW Major in C,C++ etc.
//so element at (i,j) is computed as  (j*dim0 + i) for ColMajor/IDL
//and (i*dim1 + j) for RowMajor/C

#define TRACEMATRIX_C(var__)\
      {int dim0__=(var__)->Dim(0), dim1__=(var__)->Dim(1);\
        fprintf(stderr,"c matrix[%d,%d]\n",dim0__,dim1__);\
        for (int row=0; row < dim0__ ; row++)\
        {\
          for (int col=0; col < dim1__-1; col++)\
          {\
            fprintf(stderr,"%g, ",(*var__)[row*dim1__ + col]);\
          }\
          fprintf(stderr,"%g\n",(*var__)[row*dim1__ + dim1__ -1]);\
        }\
        fprintf(stderr,"\n");\
      }
//The following abbrevs should output the C matrix as IDL would do (ie,transposed):
#define TRACEMATRIX_IDL(var__)\
      {int dim0__=(var__)->Dim(0), dim1__=(var__)->Dim(1);\
        fprintf(stderr,"idl matrix[%d,%d]\n[",dim0__,dim1__);\
        for (int col=0; col < dim1__; col++)\
        {\
          fprintf(stderr,"[");\
          for (int row=0; row < dim0__; row++)\
          {\
            fprintf(stderr,"%g",(*var__)[row*dim1__ + col]);\
            if (row<dim0__-1) fprintf(stderr," ,"); \
            else if (col<dim1__-1) fprintf(stderr," ],$\n"); else fprintf(stderr," ]]\n") ;\
          }\
        }\
      }

#include "envt.hpp"
#include "graphicsdevice.hpp"
#include "initsysvar.hpp"

  struct GDL_3DTRANSFORMDATA
  {
    DDoubleGDL* Matrix;
    DDouble zValue;
    int* code;
    DDouble x0;
    DDouble xs;
    DDouble y0;
    DDouble ys;
    DDouble z0;
    DDouble zs;
    bool xlog;
    bool ylog;
    bool zlog;
  };

  static GDL_3DTRANSFORMDATA Data3d;

  static int code012[3] = {0, 1, 2};
  static int code102[3] = {1, 0, 2};
  static int code120[3] = {1, 2, 0};
  static int code210[3] = {2, 1, 0};
  static int code201[3] = {2, 0, 1};
  static int code021[3] = {0, 2, 1};

  enum ORIENTATION3D
  {
    NORMAL=0,
    XY,
    XZ,
    YZ,
    XZYZ,
    XZXY
  };
#define GDL_NONE -1
#define GDL_TICKFORMAT 0
#define GDL_TICKUNITS 1
#define GDL_TICKFORMAT_AND_UNITS 2

namespace lib {

  using namespace std;

  // main plotting routine (all defined using the plotting_routine_call class)
  void plot( EnvT* e);
  void plot_io( EnvT* e);
  void plot_oo( EnvT* e);
  void plot_oi( EnvT* e);
  void oplot( EnvT* e);
  void plots( EnvT* e);
  void surface( EnvT* e);
  void shade_surf( EnvT* e);
  void contour( EnvT* e);
  void xyouts( EnvT* e);
  void axis( EnvT* e);
  void polyfill( EnvT* e);
  void tv_image( EnvT* e);
  void usersym( EnvT* e);
  void set_shading( EnvT* e);

  // other plotting routines
  void erase( EnvT* e);
  void tvlct( EnvT* e);
  void wshow( EnvT* e);
  void wdelete( EnvT* e);
  void wset( EnvT* e);
  void window( EnvT* e);
  void set_plot( EnvT* e);
  BaseGDL* get_screen_size( EnvT* e);
  void device( EnvT* e);
  void cursor( EnvT* e);
  void tvcrs( EnvT* e);
  void empty(EnvT* e);

  // Map stuff
  void get_mapset(bool &mapset);
  void set_mapset(bool mapset);
  BaseGDL* map_proj_forward_fun( EnvT* e);
  BaseGDL* map_proj_inverse_fun( EnvT* e);
  BaseGDL* convert_coord( EnvT* e);
  BaseGDL* trigrid_fun( EnvT* e);

#if defined(USE_LIBPROJ4) || defined(USE_LIBPROJ4_NEW) 
#define GDL_COMPLEX COMPLEX2

#ifdef USE_LIBPROJ4_NEW
extern "C" {
//#include "projects.h"
#include "proj_api.h"
}
#define LPTYPE projLP
#define XYTYPE projXY

#define PROJTYPE projPJ
#define PROJDATA projUV
#define PJ_INIT pj_init

PROJDATA protect_proj_fwd (PROJDATA idata, PROJTYPE proj);
PROJDATA protect_proj_inv (PROJDATA idata, PROJTYPE proj);

#define PJ_FWD protect_proj_fwd
#define LIB_PJ_FWD pj_fwd
#define PJ_INV protect_proj_inv 
#define LIB_PJ_INV pj_inv
PROJTYPE map_init(DStructGDL *map=SysVar::Map());
static volatile PROJTYPE ref;
static volatile PROJTYPE prev_ref;
static PROJDATA badProj={sqrt(-1),sqrt(-1)};
#else
extern "C" {
//adding this removes the problem with lam,phi vs. x,y and make no diffs between old an new lib proj.4    
#define PROJ_UV_TYPE 1
#include "lib_proj.h"
}
#define LPTYPE PROJ_LP
#define XYTYPE PROJ_XY

#define PROJTYPE PROJ*
#define PROJDATA PROJ_UV
#define PJ_INIT proj_init

PROJDATA protect_proj_fwd (PROJDATA idata, PROJTYPE proj);
PROJDATA protect_proj_inv (PROJDATA idata, PROJTYPE proj);

#define PJ_FWD protect_proj_fwd
#define LIB_PJ_FWD proj_fwd
#define PJ_INV protect_proj_inv
#define LIB_PJ_INV proj_inv
PROJTYPE map_init(DStructGDL *map=SysVar::Map());
static PROJTYPE ref;
static PROJTYPE prev_ref;
static PROJDATA badProj={sqrt(-1),sqrt(-1)};
//general reprojecting function
#endif

//general reprojecting function
DDoubleGDL* gdlProjForward(PROJTYPE ref, DStructGDL* map, DDoubleGDL* lon, DDoubleGDL *lat, DLongGDL* connectivity,
bool doConn,DLongGDL* &gons, bool doGons,DLongGDL* &lines, bool doLines, bool doFill);

void GDLgrProjectedPolygonPlot(EnvT* e, GDLGStream * a, PROJTYPE ref, DStructGDL* map, 
DDoubleGDL *lons, DDoubleGDL *lats, bool isRadians, bool const doFill, DLongGDL *conn=NULL);
DStructGDL *GetMapAsMapStructureKeyword(EnvT *e, bool &externalMap);
//dummy functions for compatibility support of GCTP projections 
  void map_proj_gctp_forinit (EnvT* e);
  void map_proj_gctp_revinit (EnvT* e);


#define COMPLEX2 GDL_COMPLEX
#else //NOT USE_LIBPROJ4 : define some more or less dummy values:
#define PROJTYPE void*
#define DEG_TO_RAD 0.017453292
#endif //USE_LIBPROJ4






  //3D conversions
  void SelfTranspose3d(DDoubleGDL* me);
  void SelfReset3d(DDoubleGDL* me);
  void SelfTranslate3d(DDoubleGDL* me, DDouble *trans);
  void SelfScale3d(DDoubleGDL* me, DDouble *scale);
  void SelfRotate3d(DDoubleGDL* me, DDouble *rot);
  void SelfPerspective3d(DDoubleGDL* me, DDouble zdist);
  void SelfOblique3d(DDoubleGDL* me, DDouble dist, DDouble angle);
  void SelfExch3d(DDoubleGDL* me, DLong code);
  void gdl3dTo2dTransformContour(PLFLT x, PLFLT y, PLFLT *xt, PLFLT *yt, PLPointer data);
  void gdl3dTo2dTransform(PLFLT x, PLFLT y, PLFLT *xt, PLFLT *yt, PLPointer data);
  void gdlProject3dCoordinatesIn2d( DDoubleGDL* Matrix, DDoubleGDL *xVal, DDouble *sx,
                                    DDoubleGDL *yVal, DDouble *sy, DDoubleGDL* zVal,
                                    DDouble *sz, DDoubleGDL *xValou, DDoubleGDL *yValou);
  DDoubleGDL* gdlComputePlplotRotationMatrix(DDouble az, DDouble alt, DDouble zValue, DDouble scale=1.0);
  DDoubleGDL* gdlConvertT3DMatrixToPlplotRotationMatrix(DDouble zValue, DDouble &az, DDouble &alt, DDouble &ay, DDouble &scale, ORIENTATION3D &code);
  DDoubleGDL* gdlGetScaledNormalizedT3DMatrix(DDoubleGDL* Matrix=NULL);
  DDoubleGDL* gdlGetT3DMatrix();
  void gdlNormed3dToWorld3d(DDoubleGDL *xVal, DDoubleGDL *yVal, DDoubleGDL* zVal,
                            DDoubleGDL* xValou, DDoubleGDL *yValou, DDoubleGDL *zValou);
  void gdl3dto2dProjectDDouble(DDoubleGDL* t3dMatrix, DDoubleGDL *xVal, DDoubleGDL *yVal, 
                               DDoubleGDL* zVal, DDoubleGDL *xValou, DDoubleGDL *yValou, int* code);
  bool gdlSet3DViewPortAndWorldCoordinates( EnvT* e, GDLGStream* actStream, DDoubleGDL* Matrix,
  bool xLog, bool yLog, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, DDouble zStart=0.0, DDouble zEnd=1.0, bool zLog=false);
  void scale3_pro(EnvT* e);
  void t3d_pro( EnvT* e);
  bool gdlAxis3(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log, DLong zAxisCode=0, DDouble length=0 );
  bool gdlBox3(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd,
        DDouble zStart, DDouble zEnd, bool xLog, bool yLog, bool zLog,bool doSpecialAxisPlacement=0);
  bool T3Denabled(EnvT *e);
  
  class plotting_routine_call
  {
    // ensure execution of child-class destructors
    public: virtual ~plotting_routine_call() {};
    
    // private fields
    private: SizeT _nParam;
    private: bool overplot;
    private: bool isDB; //see below why commented.

    // common helper methods
    protected: inline SizeT nParam() { return _nParam; }

    // prototypes for methods defining various steps
    private: virtual bool handle_args(EnvT*) = 0; // return value = overplot
    private: virtual void old_body(EnvT*, GDLGStream*) = 0;
    private: virtual void call_plplot(EnvT*, GDLGStream*) = 0;
    private: virtual void post_call(EnvT*, GDLGStream*) = 0;

    // all steps combined (virtual methods cannot be called from ctor)
    public: void call(EnvT* e, SizeT n_params_required) // {{{
    {
      // when !d.name == Null  we do nothing !
      DString name = (*static_cast<DStringGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("NAME"), 0)))[0];
      if (name == "NULL") return;

      _nParam = e->NParam(n_params_required);

      overplot = handle_args(e);

      GDLGStream* actStream = GraphicsDevice::GetDevice()->GetStream();
      if (actStream == NULL) e->Throw("Unable to create window.");
      //double buffering kills the logic and operation of XOR modes. Use HasSafeDoubleBuffering() that tests this feature.)
      isDB = actStream->HasSafeDoubleBuffering();
      if (isDB) actStream->SetDoubleBuffering();

      if (name == "X" || name == "MAC" || name == "WIN" )  actStream->updatePageInfo(); //since window size can change

      old_body(e, actStream); // TODO: to be removed!
      call_plplot(e, actStream);

      post_call(e, actStream);
//      if (isDB) actStream->eop(); else actStream->flush();
      if (isDB) actStream->UnSetDoubleBuffering();
      actStream->Update();
    } // }}}
  };
  void gdlDoRangeExtrema(DDoubleGDL *xVal, DDoubleGDL *yVal, DDouble &min, DDouble &max, DDouble xmin, DDouble xmax, bool doMinMax=FALSE, DDouble minVal=0, DDouble maxVal=0);
  bool draw_polyline(EnvT *e,  GDLGStream *a, DDoubleGDL *xVal, DDoubleGDL *yVal, 
             DDouble minVal, DDouble maxVal, bool doMinMax,
		     bool xLog, bool yLog, 
		     DLong psym=0, bool append=FALSE, DLongGDL *color=NULL);
  DDouble gdlEpsDouble();
  DDouble gdlAbsoluteMinValueDouble();
  DDouble gdlAbsoluteMaxValueDouble();
  //protect from (inverted, strange) axis log values
  void gdlHandleUnwantedAxisValue(DDouble &min, DDouble &max, bool log);
  //set the background color
  void gdlSetGraphicsBackgroundColorFromKw(EnvT * e, GDLGStream * a,bool kw=true);
  //set The background color as foreground ()
  void gdlSetGraphicsPenColorToBackground(GDLGStream *a);
  //set the foreground color
  void gdlSetGraphicsForegroundColorFromKw(EnvT * e, GDLGStream * a, string otherColorKw="");
  //advance to next plot unless the noerase flag is set
  void gdlNextPlotHandlingNoEraseOption(EnvT * e, GDLGStream * a,bool noe=0);
  //set the symbol shape
  void gdlGetPsym(EnvT *e, DLong &psym);
  //set the symbol size
  void gdlSetSymsize(EnvT * e, GDLGStream * a);
  //set the PLOT character size (including MULTI subscaling)
  void gdlSetPlotCharsize(EnvT *e, GDLGStream *a, bool accept_sizeKw=false);
  //set the PLOT Char Thickness
  void gdlSetPlotCharthick(EnvT *e, GDLGStream *a);
  //set the line thickness
  void gdlSetPenThickness(EnvT * e, GDLGStream * a);
  //get the line thickness
  DFloat gdlGetPenThickness(EnvT * e, GDLGStream * a);
  //set the linestyle
  void gdlLineStyle(GDLGStream *a, DLong style);
  void gdlSetLineStyle(EnvT * e, GDLGStream * a);
  //set axis linewidth
  void gdlSetAxisThickness(EnvT *e, GDLGStream *a, string axis);
  //title
  void gdlWriteTitleAndSubtitle(EnvT* e, GDLGStream *a);
  //set the !axis.crange vector
  void gdlStoreAxisCRANGE(string axis, DDouble Start, DDouble End, bool log);
  //set the !axis.s vector
  void gdlStoreAxisSandWINDOW(GDLGStream* actStream, string axis, DDouble Start, DDouble End, bool log=false);

  //axis_type
  void gdlGetAxisType(string axis, bool &log);
  //get the !axis.crange vector
  void gdlGetCurrentAxisRange(string axis, DDouble &Start, DDouble &End, bool checkMapset=TRUE);
  void gdlGetCurrentAxisWindow(string axis, DDouble &wStart, DDouble &wEnd);
  void gdlGetDesiredAxisMargin(EnvT *e, string axis,DFloat &start, DFloat &end);
  void gdlGetDesiredAxisCharsize(EnvT* e, string axis, DFloat &charsize);
  void gdlGetDesiredAxisStyle(EnvT *e, string axis,DLong &style);
  void gdlGetDesiredAxisTitle(EnvT *e, string axis,DString &title);
  bool gdlGetDesiredAxisRange(EnvT *e, string axis,
		      DDouble &start, DDouble &end);
  //set the axis 'axis' charsize (including MULTI subscaling)
  void gdlSetAxisCharsize(EnvT *e, GDLGStream *a, string axis);
  void gdlStoreAxisType(string axis, bool type);

  //length and height of a char in normalized coords, using trick
  void gdlGetCharSizes(GDLGStream *a, PLFLT &nsx, PLFLT &nsy, DDouble &wsx, DDouble &wsy, DDouble &dsx, DDouble &dsy, DDouble &lsx, DDouble &lsy);
  void GetSFromPlotStructs(DDouble **sx, DDouble **sy, DDouble **sz=NULL);
  void GetWFromPlotStructs(DFloat **wx, DFloat **wy);
  bool startClipping(EnvT *e, GDLGStream *a, bool UsePClip);
  void stopClipping(GDLGStream *a);
  void gdlStoreCLIP(DLongGDL* clipBox);
  void DataCoordLimits(DDouble *sx, DDouble *sy, DFloat *wx, DFloat *wy, 
    DDouble *xStart, DDouble *xEnd, DDouble *yStart, DDouble *yEnd, bool);
  void GetCurrentUserLimits(EnvT* e, GDLGStream *a, DDouble &xStart, DDouble &xEnd, DDouble &yStart, DDouble &yEnd);
  PLFLT gdlAdjustAxisRange(DDouble &val_min, DDouble &val_max, bool log = false);
  PLFLT AutoTick(DDouble x);
  PLFLT gdlComputeTickInterval(EnvT *e, string axis, DDouble &min, DDouble &max, bool log);
  bool gdlYaxisNoZero(EnvT* e);
  bool gdlSetViewPortAndWorldCoordinates( EnvT* e, GDLGStream* actStream, DFloatGDL* boxPosition, bool xLog, bool yLog,
                 DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT,
                 DDouble xStart, DDouble xEnd, DDouble minVal, DDouble maxVal, DLong iso);
  void GetMinMaxVal( DDoubleGDL* val, double* minVal, double* maxVal);

    void CheckMargin( EnvT* e, GDLGStream* actStream,
                    DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT,
                    PLFLT& xMR, PLFLT& xML, PLFLT& yMB, PLFLT& yMT);
    void handle_pmulti_position(EnvT *e, GDLGStream *a);
    void UpdateSWPlotStructs(GDLGStream* actStream, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, bool xLog, bool yLog);
    bool gdlAxis(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log, DLong modifierCode=0, DDouble length=0);
    bool gdlBox(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, bool xLog, bool yLog);
} // namespace

#endif
