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

#define TRACEMATRIX_C(var__)						\
  {int dim0__=(var__)->Dim(0), dim1__=(var__)->Dim(1);			\
    fprintf(stderr,"c matrix[%d,%d]\n",dim0__,dim1__);			\
    for (int row=0; row < dim0__ ; row++)				\
      {									\
	for (int col=0; col < dim1__-1; col++)				\
          {								\
            fprintf(stderr,"%g, ",(*var__)[row*dim1__ + col]);		\
          }								\
	fprintf(stderr,"%g\n",(*var__)[row*dim1__ + dim1__ -1]);	\
      }									\
    fprintf(stderr,"\n");						\
  }
//The following abbrevs should output the C matrix as IDL would do (ie,transposed):
#define TRACEMATRIX_IDL(var__)						\
  {int dim0__=(var__)->Dim(0), dim1__=(var__)->Dim(1);			\
    fprintf(stderr,"idl matrix[%d,%d]\n[",dim0__,dim1__);		\
    for (int col=0; col < dim1__; col++)				\
      {									\
	fprintf(stderr,"[");						\
	for (int row=0; row < dim0__; row++)				\
          {								\
            fprintf(stderr,"%g",(*var__)[row*dim1__ + col]);		\
            if (row<dim0__-1) fprintf(stderr," ,");			\
            else if (col<dim1__-1) fprintf(stderr," ],$\n"); else fprintf(stderr," ]]\n") ; \
          }								\
      }									\
  }

#include "envt.hpp"
#include "graphicsdevice.hpp"
#include "initsysvar.hpp"
#include <limits>

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
    NORMAL3D=0,
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
  struct GDL_TICKDATA
  {
    GDLGStream *a;
    bool isLog;
    DDouble axisrange; //to circumvent plplot passing a non-zero value instead of strict 0.0
    double nchars; //length of string *returned* after formatting. Can be non-integer.
  };

  struct GDL_TICKNAMEDATA
  {
    GDLGStream *a;
    SizeT counter;
    SizeT nTickName;
    DStringGDL* TickName;
    bool isLog;
    DDouble axisrange; //to circumvent plplot passing a non-zero value instead of strict 0.0
    double nchars; //length of string *returned* after formatting. Can be non-integer.
  };

  struct GDL_MULTIAXISTICKDATA
  {
    EnvT *e;
    GDLGStream *a;
    SizeT counter;
    int what;
    SizeT nTickFormat;
    DDouble axismin;
    DDouble axismax;
    DStringGDL* TickFormat;
    SizeT nTickUnits;
    DStringGDL* TickUnits;
    bool isLog;
    DDouble axisrange; //to circumvent plplot passing a non-zero value instead of strict 0.0
    double nchars; //length of string *returned* after formatting. Can be non-integer.
  };
  
  typedef struct GDL_SAVEBOX {
   bool initialized;
    PLFLT wx1; //world coord of x min
    PLFLT wx2;
    PLFLT wy1;
    PLFLT wy2;
    PLFLT nx1; 
    PLFLT nx2;
    PLFLT ny1;
    PLFLT ny2;
  } gdlSavebox ;
  
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
  BaseGDL* format_axis_values(EnvT *e);
  void scale3_pro(EnvT* e);
  void t3d_pro( EnvT* e);
  
  // Map stuff
  void get_mapset(bool &mapset);
  void set_mapset(bool mapset);
  BaseGDL* map_proj_forward_fun( EnvT* e);
  BaseGDL* map_proj_inverse_fun( EnvT* e);
  BaseGDL* convert_coord( EnvT* e);
  //dummy functions for compatibility support of GCTP projections 
  void map_proj_gctp_forinit (EnvT* e);
  void map_proj_gctp_revinit (EnvT* e);
  
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
  DDoubleGDL* gdlProjForward(PROJTYPE ref, DStructGDL* map,
			     DDoubleGDL* lon, DDoubleGDL *lat, DLongGDL* connectivity,
			     bool doConn, DLongGDL* &gons, bool doGons,
			     DLongGDL* &lines, bool doLines, bool doFill);

  void GDLgrProjectedPolygonPlot(GDLGStream * a, PROJTYPE ref, DStructGDL* map, 
				 DDoubleGDL *lons, DDoubleGDL *lats, bool isRadians,
				 bool const doFill, DLongGDL *conn=NULL);
   
  DStructGDL *GetMapAsMapStructureKeyword(EnvT *e, bool &externalMap); //not static since KW is at same place for all uses.

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
  DDoubleGDL* gdlConvertT3DMatrixToPlplotRotationMatrix(DDouble zValue, DDouble &az, DDouble &alt, 
							DDouble &ay, DDouble &scale, ORIENTATION3D &code);
  DDoubleGDL* gdlGetScaledNormalizedT3DMatrix(DDoubleGDL* Matrix=NULL);
  DDoubleGDL* gdlGetT3DMatrix();
  void gdlNormed3dToWorld3d(DDoubleGDL *xVal, DDoubleGDL *yVal, DDoubleGDL* zVal,
                            DDoubleGDL* xValou, DDoubleGDL *yValou, DDoubleGDL *zValou);
  void gdl3dto2dProjectDDouble(DDoubleGDL* t3dMatrix, DDoubleGDL *xVal, DDoubleGDL *yVal, 
                               DDoubleGDL* zVal, DDoubleGDL *xValou, DDoubleGDL *yValou, int* code);
  bool T3Denabled();
  
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
  public: void call(EnvT* e, SizeT n_params_required)
    {
      // when !d.name == Null  we do nothing !
      DString name = (*static_cast<DStringGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("NAME"), 0)))[0];
      if (name == "NULL") return;

      _nParam = e->NParam(n_params_required);

      overplot = handle_args(e);

      GDLGStream* actStream = GraphicsDevice::GetDevice()->GetStream();
      if (actStream == NULL) e->Throw("Unable to create window.");
      
      //ALL THE DoubleBuffering and Flush() code below introduces terrible slowness in remote X displays, as well as a lot of time lost
      //for displays on the same server. They are completely removed now.      
      //      //double buffering kills the logic and operation of XOR modes. Use HasSafeDoubleBuffering() that tests this feature.)
      //      isDB = actStream->HasSafeDoubleBuffering();
      //      if (isDB) actStream->SetDoubleBuffering();

      if (name == "X" || name == "MAC" || name == "WIN" )  actStream->updatePageInfo(); //since window size can change

      old_body(e, actStream); // TODO: to be removed!
      call_plplot(e, actStream);

      post_call(e, actStream);
      // IDEM: SLOW
      //      if (isDB) actStream->eop(); else actStream->flush();
      //      if (isDB) actStream->UnSetDoubleBuffering();
      
      //this is absolutely necessary for widgets as for windows. However the virtual Update function
      //i.e., calling  plstream::cmd(PLESC_EXPOSE, NULL) is very slow.
      // See how to overload it by a faster function such as in GDLXStream::Update() . 
      actStream->Update();
    }
  };
  void gdlDoRangeExtrema(DDoubleGDL *xVal, DDoubleGDL *yVal, DDouble &min, DDouble &max, DDouble xmin, DDouble xmax, bool doMinMax=FALSE, DDouble minVal=0, DDouble maxVal=0);
  void draw_polyline(GDLGStream *a, DDoubleGDL *xVal, DDoubleGDL *yVal, 
		     DDouble minVal, DDouble maxVal, bool doMinMax,
		     bool xLog, bool yLog, //end non-default values 
         DLong psym=0, bool useProjectionInfo=false, bool append=FALSE, DLongGDL *color=NULL);
  //protect from (inverted, strange) axis log values
  void gdlHandleUnwantedAxisValue(DDouble &min, DDouble &max, bool log);
  void gdlSetGraphicsPenColorToBackground(GDLGStream *a);
  void gdlLineStyle(GDLGStream *a, DLong style);
  void gdlStoreAxisCRANGE(string axis, DDouble Start, DDouble End, bool log);
  void gdlStoreAxisSandWINDOW(GDLGStream* actStream, string axis, DDouble Start, DDouble End, bool log=false);
  void gdlGetAxisType(string axis, bool &log);
  void gdlGetCurrentAxisRange(string axis, DDouble &Start, DDouble &End, bool checkMapset=FALSE);
  void gdlGetCurrentAxisWindow(string axis, DDouble &wStart, DDouble &wEnd);
  void gdlStoreAxisType(string axis, bool type);
  void gdlGetCharSizes(GDLGStream *a, PLFLT &nsx, PLFLT &nsy, DDouble &wsx, DDouble &wsy, 
		       DDouble &dsx, DDouble &dsy, DDouble &lsx, DDouble &lsy); 
  void GetSFromPlotStructs(DDouble **sx, DDouble **sy, DDouble **sz=NULL);
  void GetWFromPlotStructs(DFloat **wx, DFloat **wy);
  void setPlplotScale(GDLGStream* a);
  void DataCoordLimits(DDouble *sx, DDouble *sy, DFloat *wx, DFloat *wy, 
		       DDouble *xStart, DDouble *xEnd, DDouble *yStart, DDouble *yEnd, bool);
  void stopClipping(GDLGStream *a);
  void gdlStoreCLIP(DLongGDL* clipBox);
  void GetCurrentUserLimits(GDLGStream *a, 
			    DDouble &xStart, DDouble &xEnd, DDouble &yStart, DDouble &yEnd);
  PLFLT gdlAdjustAxisRange(EnvT* e, string axis, DDouble &val_min, DDouble &val_max, bool log = false, int calendarcode = 0);
  PLFLT AutoTick(DDouble x);
  void setIsoPort(GDLGStream* actStream,PLFLT x1,PLFLT x2,PLFLT y1,PLFLT y2,PLFLT aspect);
  void GetMinMaxVal( DDoubleGDL* val, double* minVal, double* maxVal);
  void CheckMargin( GDLGStream* actStream,
                    DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT,
                    PLFLT& xMR, PLFLT& xML, PLFLT& yMB, PLFLT& yMT);
  void UpdateSWPlotStructs(GDLGStream* actStream, DDouble xStart, DDouble xEnd, DDouble yStart,
			   DDouble yEnd, bool xLog, bool yLog);
  gdlSavebox* getSaveBox();
  gdlSavebox* getTempBox();
  void gdlSimpleAxisTickFunc( PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data);
  void gdlSingleAxisTickNamedFunc( PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data);
  void gdlMultiAxisTickFunc(PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data);
  void doOurOwnFormat(PLINT axisNotUsed, PLFLT value, char *label, PLINT length, PLPointer data);
//
//--------------FOLLOWING ARE STATIC FUNCTIONS-----------------------------------------------
//This because static pointers to options indexes are needed to speed up process, but these indexes vary between
//the definition of the caller functions (e.g. "CHARSIZE" is 1 for CONTOUR but 7 for XYOUTS). So they need to be kept
//static (for speed) but private for each graphic command.
  static void gdlSetGraphicsBackgroundColorFromKw(EnvT *e, GDLGStream *a, bool kw=true)
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DLong background=
    (*static_cast<DLongGDL*>
     (pStruct->GetTag(pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
    if ( kw ) {
      static int BACKGROUNDIx=e->KeywordIx("BACKGROUND");
      e->AssureLongScalarKWIfPresent(BACKGROUNDIx, background);
    }
    DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
    a->Background(background,decomposed);
  }
  static void gdlSetGraphicsForegroundColorFromKw(EnvT *e, GDLGStream *a, string OtherColorKw="")
  {
    // Get COLOR from PLOT system variable
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DLong color=
    (*static_cast<DLongGDL*>
     (pStruct->GetTag(pStruct->Desc()->TagIndex("COLOR"), 0)))[0];

    DLongGDL *colorVect;
    static int colorIx=e->KeywordIx ( "COLOR" );
    int realcolorIx=colorIx;
    //eventually do not get color from standard "COLOR" keyword but from another...
    if (OtherColorKw != "") realcolorIx=e->KeywordIx (OtherColorKw);
    if ( e->GetKW ( realcolorIx )!=NULL )
    {
      colorVect=e->GetKWAs<DLongGDL>( realcolorIx ); //color can be vectorial, but...
      color=(*colorVect)[0]; //this function only sets color to 1st arg in list!
    }
    // Get decomposed value for colors
    DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
    a->Color(color, decomposed);
  }
  
  static void gdlGetPsym(EnvT *e, DLong &psym)
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    psym=(*static_cast<DLongGDL*>
          (pStruct->GetTag(pStruct->Desc()->TagIndex("PSYM"), 0)))[0];
    static int PSYMIx=e->KeywordIx("PSYM");
    e->AssureLongScalarKWIfPresent(PSYMIx, psym);
    if ( psym>10||psym < -8||psym==9 )
      e->Throw(
               "PSYM (plotting symbol) out of range.");
  }
   static void gdlSetSymsize(EnvT *e, GDLGStream *a)
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DFloat symsize=(*static_cast<DFloatGDL*>
                    (pStruct->GetTag(pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
                    //NOTE THAT AS OF IDL 8.2 !P.SYMSIZE, HOWEVER EXISTING, IS NOT TAKEN INTO ACCOUNT. We however do not want
                    //to reproduce this feature.
    static int SYMSIZEIx=e->KeywordIx("SYMSIZE");
    e->AssureFloatScalarKWIfPresent(SYMSIZEIx, symsize);
    if ( symsize<=0.0 ) symsize=1.0;
    a->setSymbolSize(symsize);
  }
//  static void GetUserSymSize(EnvT *e, GDLGStream *a, DDouble& UsymConvX, DDouble& UsymConvY)
//  {
//    //get symsize
//    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
//    DFloat symsize=(*static_cast<DFloatGDL*>
//                    (pStruct->GetTag(pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
//    static int SYMSIZEIx = e->KeywordIx("SYMSIZE");
//    e->AssureFloatScalarKWIfPresent(SYMSIZEIx, symsize);
//    if ( symsize<=0.0 ) symsize=1.0;
//    
//    UsymConvX=(0.5*symsize*(a->wCharLength()/a->charScale())); //be dependent only on symsize!
//    UsymConvY=(0.5*symsize*(a->wCharHeight()/a->charScale()));
//    PLFLT wun, wdeux, wtrois, wquatre; //take care of axes world orientation!
//    a->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
//    if ((wdeux-wun)<0) UsymConvX*=-1.0;
//    if ((wquatre-wtrois)<0) UsymConvY*=-1.0;
//    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GetUserSymSize(%f,%f), charlen=%f, charheight=%f, charscale=%f\n",
//				    UsymConvX, UsymConvY,a->wCharLength(),a->wCharHeight(),a->charScale());
//  }
  static void gdlSetPlotCharsize(EnvT *e, GDLGStream *a, bool accept_sizeKw=false)
  {
    PLFLT charsize;
    DDouble pmultiscale=1.0;
    // get !P preference
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    charsize=(*static_cast<DFloatGDL*>
              (pStruct->GetTag
               (pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    //overload with command preference. Charsize may be a vector now in some gdl commands, take care of it:
    if (accept_sizeKw) //XYOUTS specials!
    {
      static int SIZEIx=e->KeywordIx("SIZE"); //define here only (else trig an assert() )
      DFloat fcharsize;
      fcharsize=charsize;
      e->AssureFloatScalarKWIfPresent(SIZEIx, fcharsize); 
      charsize=fcharsize;
    }
    static int charsizeIx=e->KeywordIx ( "CHARSIZE" );
    if ( e->GetKW ( charsizeIx )!=NULL )
    {
      DFloatGDL* charsizeVect=e->GetKWAs<DFloatGDL>( charsizeIx );
      charsize=(*charsizeVect)[0];
    }
    if ( charsize<=0.0 ) charsize=1.0;
    // adjust if MULTI:
    DLongGDL* pMulti=SysVar::GetPMulti();
    if ( (*pMulti)[1]>2||(*pMulti)[2]>2 ) pmultiscale=0.5;
    a->sizeChar(charsize*pmultiscale);
  }
  
   static void gdlSetPlotCharthick(EnvT *e, GDLGStream *a)
  {
     // get !P preference
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DFloat charthick=(*static_cast<DFloatGDL*>
              (pStruct->GetTag
               (pStruct->Desc()->TagIndex("CHARTHICK"), 0)))[0];
    static int charthickIx=e->KeywordIx ( "CHARTHICK" ); //Charthick values may be vector in GDL, not in IDL!
    if ( e->GetKW ( charthickIx )!=NULL )
    {
      DFloatGDL* charthickVect=e->GetKWAs<DFloatGDL>( charthickIx );
      charthick=(*charthickVect)[0];
    }
    if ( charthick <= 0.0 ) charthick=1.0;
    a->Thick(charthick);
  }
   
  static PLFLT gdlComputeTickInterval(EnvT *e, string axis, DDouble &min, DDouble &max, bool log)
  {
    DLong nticks=0;

    static int XTICKSIx = e->KeywordIx("XTICKS");
    static int YTICKSIx = e->KeywordIx("YTICKS");
    static int ZTICKSIx = e->KeywordIx("ZTICKS");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKSIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKSIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKSIx; }

    if ( Struct!=NULL )
    {
      unsigned tickTag=Struct->Desc()->TagIndex("TICKS");
      nticks=(*static_cast<DLongGDL*>(Struct->GetTag(tickTag, 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, nticks);

    PLFLT intv;
    if (nticks == 0)
    {
      intv = (log)? AutoTick(log10(max-min)): AutoTick(max-min);
    } else {
      intv = (log)? log10(max-min)/nticks: (max-min)/nticks;
    }
    return intv;
  }
  
  static void gdlGetDesiredAxisCharsize(EnvT* e, string axis, DFloat &charsize)
  {
    //default:
    charsize=1.0;
    // get !P preference. Even if [xyz]charsize is absent, presence of charsize or !P.charsize must be taken into account.
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    charsize=(*static_cast<DFloatGDL*>
              (pStruct->GetTag
              (pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    static int CharsizeIx= e->KeywordIx( "CHARSIZE");
    //cerr<<" CHARSIZE: "<< CharsizeIx<<" ("<< &CharsizeIx<<")"<<endl;
    e->AssureFloatScalarKWIfPresent(CharsizeIx, charsize); // option charsize overloads P.CHARSIZE
    if (charsize==0) charsize=1.0;
    // Axis Preference. Is a Multiplier!
    static int XCharsizeIx = e->KeywordIx("XCHARSIZE");
    static int YCharsizeIx = e->KeywordIx("YCHARSIZE");
    static int ZCharsizeIx = e->KeywordIx("ZCHARSIZE");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XCharsizeIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YCharsizeIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZCharsizeIx; }

    if ( Struct!=NULL )
    {
      unsigned charsizeTag=Struct->Desc()->TagIndex("CHARSIZE"); //[XYZ].CHARSIZE
      DFloat axisCharsizeMultiplier=(*static_cast<DFloatGDL*>(Struct->GetTag(charsizeTag, 0)))[0];
      e->AssureFloatScalarKWIfPresent(choosenIx, axisCharsizeMultiplier); //option [XYZ]CHARSIZE overloads ![XYZ].CHARSIZE
      if (axisCharsizeMultiplier>0.0) charsize*=axisCharsizeMultiplier; //IDL Behaviour...
    }
  }
  static  void gdlSetAxisCharsize(EnvT *e, GDLGStream *a, string axis)
  {

    DFloat charsize=0.0;
    DDouble pmultiscale=1.0;
    gdlGetDesiredAxisCharsize(e, axis, charsize);
    // adjust if MULTI:
    DLongGDL* pMulti=SysVar::GetPMulti();
    if ( (*pMulti)[1]>2||(*pMulti)[2]>2 ) pmultiscale=0.5; //IDL behaviour
    // scale default value (which depends on number of subpages)
    // a->schr(0.0, charsize*pmultiscale);
    a->sizeChar(charsize*pmultiscale);
  }

  static void gdlGetDesiredAxisGridStyle(EnvT* e, string axis, DLong &axisGridstyle)
  {
    axisGridstyle=0;
    DStructGDL* Struct=NULL;
    static int XGRIDSTYLEIx = e->KeywordIx("XGRIDSTYLE");
    static int YGRIDSTYLEIx = e->KeywordIx("YGRIDSTYLE");
    static int ZGRIDSTYLEIx = e->KeywordIx("ZGRIDSTYLE");
    int choosenIx;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XGRIDSTYLEIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YGRIDSTYLEIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZGRIDSTYLEIx; }

    if ( Struct!=NULL )
    {
      unsigned gridstyleTag=Struct->Desc()->TagIndex("GRIDSTYLE");
      axisGridstyle=(*static_cast<DLongGDL*>(Struct->GetTag(gridstyleTag, 0)))[0];
      e->AssureLongScalarKWIfPresent(choosenIx, axisGridstyle);
    }
  }
  static void gdlGetDesiredAxisMargin(EnvT *e, string axis, DFloat &start, DFloat &end)
  {
    static int XMARGINIx = e->KeywordIx("XMARGIN");
    static int YMARGINIx = e->KeywordIx("YMARGIN");
    static int ZMARGINIx = e->KeywordIx("ZMARGIN");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XMARGINIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YMARGINIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZMARGINIx; }

    if ( Struct!=NULL )
    {
      unsigned marginTag=Struct->Desc()->TagIndex("MARGIN");
      start= (*static_cast<DFloatGDL*>(Struct->GetTag(marginTag, 0)))[0];
      end  = (*static_cast<DFloatGDL*>(Struct->GetTag(marginTag, 0)))[1];
    }

    BaseGDL* Margin=e->GetKW(choosenIx);
    if ( Margin!=NULL )
    {
      if ( Margin->N_Elements()>2 )
        e->Throw("Keyword array parameter "+axis+"MARGIN must have from 1 to 2 elements.");
      Guard<DFloatGDL> guard;
      DFloatGDL* MarginF=static_cast<DFloatGDL*>
      (Margin->Convert2(GDL_FLOAT, BaseGDL::COPY));
      guard.Reset(MarginF);
      start=(*MarginF)[0];
      if ( MarginF->N_Elements()>1 )
        end=(*MarginF)[1];
    }
  }
  static void gdlGetDesiredAxisMinor(EnvT* e, string axis, DLong &axisMinor)
  {
    axisMinor=0;
    static int XMINORIx = e->KeywordIx("XMINOR");
    static int YMINORIx = e->KeywordIx("YMINOR");
    static int ZMINORIx = e->KeywordIx("ZMINOR");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XMINORIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YMINORIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZMINORIx; }
   if ( Struct!=NULL )
    {
      unsigned AxisMinorTag=Struct->Desc()->TagIndex("MINOR");
      axisMinor=(*static_cast<DLongGDL*>(Struct->GetTag(AxisMinorTag,0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, axisMinor);
  }
  static bool gdlGetDesiredAxisRange(EnvT *e, string axis, DDouble &start, DDouble &end)
  {
    bool set=FALSE;
    static int XRANGEIx = e->KeywordIx("XRANGE");
    static int YRANGEIx = e->KeywordIx("YRANGE");
    static int ZRANGEIx = e->KeywordIx("ZRANGE");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XRANGEIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YRANGEIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZRANGEIx; }
    if ( Struct!=NULL )
    {
      DDouble test1, test2;
      unsigned rangeTag=Struct->Desc()->TagIndex("RANGE");
      test1=(*static_cast<DDoubleGDL*>(Struct->GetTag(rangeTag, 0)))[0];
      test2=(*static_cast<DDoubleGDL*>(Struct->GetTag(rangeTag, 0)))[1];
      if ( !((test1-test2)==0.0) )
      {
        start=test1;
        end=test2;
        set=true;
      }
    }
    BaseGDL* Range=e->GetKW(choosenIx);
    if ( Range!=NULL )
    {
      if ( Range->N_Elements()!=2 )
        e->Throw("Keyword array parameter "+axis+"RANGE must have 2 elements.");
      Guard<DDoubleGDL> guard;
      DDoubleGDL* RangeF=static_cast<DDoubleGDL*>(Range->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      guard.Reset(RangeF);
      if (!(((*RangeF)[0]-(*RangeF)[1])==0.0))
      {
        start=(*RangeF)[0];
        end=(*RangeF)[1];
        set=true;
      }
    }
    return set;
  }
  static  void gdlGetDesiredAxisStyle(EnvT *e, string axis, DLong &style)
  {
    static int XSTYLEIx = e->KeywordIx("XSTYLE");
    static int YSTYLEIx = e->KeywordIx("YSTYLE");
    static int ZSTYLEIx = e->KeywordIx("ZSTYLE");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XSTYLEIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YSTYLEIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZSTYLEIx; }

    if ( Struct!=NULL )
    {
      int styleTag=Struct->Desc()->TagIndex("STYLE");
      style= (*static_cast<DLongGDL*>(Struct->GetTag(styleTag, 0)))[0];
    }

    e->AssureLongScalarKWIfPresent( choosenIx, style);
  }
    static void gdlGetDesiredAxisThick(EnvT *e,  string axis, DFloat &thick)
  {
    thick=1.0;
    static int XTHICKIx = e->KeywordIx("XTHICK");
    static int YTHICKIx = e->KeywordIx("YTHICK");
    static int ZTHICKIx = e->KeywordIx("ZTHICK");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTHICKIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTHICKIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTHICKIx; }

    if ( Struct!=NULL )
    {
      //not static!
      int thickTag=Struct->Desc()->TagIndex("THICK");
      thick = (*static_cast<DFloatGDL*>(Struct->GetTag(thickTag, 0)))[0];
    }
    e->AssureFloatScalarKWIfPresent(choosenIx, thick);
    if ( thick <= 0.0 ) thick=1.0;
  }
   static void gdlGetDesiredAxisTickget(EnvT *e,  string axis, DDoubleGDL *Axistickget)
  {
    //TODO!
  }

  static void gdlGetDesiredAxisTickFormat(EnvT* e, string axis, DStringGDL* &axisTickformatVect)
  {
    static int XTICKFORMATIx = e->KeywordIx("XTICKFORMAT");
    static int YTICKFORMATIx = e->KeywordIx("YTICKFORMAT");
    static int ZTICKFORMATIx = e->KeywordIx("ZTICKFORMAT");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKFORMATIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKFORMATIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKFORMATIx; }

   if ( Struct!=NULL )
    {
      unsigned AxisTickformatTag=Struct->Desc()->TagIndex("TICKFORMAT");
      axisTickformatVect = static_cast<DStringGDL*>(Struct->GetTag(AxisTickformatTag,0));
    }
    if ( e->GetKW ( choosenIx )!=NULL )
    {
      axisTickformatVect=e->GetKWAs<DStringGDL>( choosenIx );
    }
  }

  static void gdlGetDesiredAxisTickInterval(EnvT* e, string axis, DDouble &axisTickinterval)
  {
    axisTickinterval=0;
    static int XTICKINTERVALIx = e->KeywordIx("XTICKINTERVAL");
    static int YTICKINTERVALIx = e->KeywordIx("YTICKINTERVAL");
    static int ZTICKINTERVALIx = e->KeywordIx("ZTICKINTERVAL");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKINTERVALIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKINTERVALIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKINTERVALIx; }

    if ( Struct!=NULL )
    {
      axisTickinterval=(*static_cast<DDoubleGDL*>
                (Struct->GetTag
                (Struct->Desc()->TagIndex("TICKINTERVAL"), 0)))[0];
    }
    e->AssureDoubleScalarKWIfPresent(choosenIx, axisTickinterval);
  }

  static void gdlGetDesiredAxisTickLayout(EnvT* e, string axis, DLong &axisTicklayout)
  {
    axisTicklayout=0;
    static int XTICKLAYOUTIx = e->KeywordIx("XTICKLAYOUT");
    static int YTICKLAYOUTIx = e->KeywordIx("YTICKLAYOUT");
    static int ZTICKLAYOUTIx = e->KeywordIx("ZTICKLAYOUT");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKLAYOUTIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKLAYOUTIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKLAYOUTIx; }
    if ( Struct!=NULL )
    {
      axisTicklayout=(*static_cast<DLongGDL*>
                (Struct->GetTag
                (Struct->Desc()->TagIndex("TICKLAYOUT"), 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, axisTicklayout);
  }

  static void gdlGetDesiredAxisTickLen(EnvT* e, string axis, DFloat &ticklen)
  {
    // order: !P.TICKLEN, TICKLEN, !X.TICKLEN, /XTICKLEN
    // get !P preference
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    ticklen=(*static_cast<DFloatGDL*>
            (pStruct->GetTag
            (pStruct->Desc()->TagIndex("TICKLEN"), 0)))[0]; //!P.TICKLEN, always exist, may be 0
    static int TICKLENIx = e->KeywordIx("TICKLEN");
    e->AssureFloatScalarKWIfPresent(TICKLENIx, ticklen); //overwritten by TICKLEN option

    static int XTICKLENIx = e->KeywordIx("XTICKLEN");
    static int YTICKLENIx = e->KeywordIx("YTICKLEN");
    static int ZTICKLENIx = e->KeywordIx("ZTICKLEN");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKLENIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKLENIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKLENIx; }
    if ( Struct!=NULL )
    {
      unsigned ticklenTag=Struct->Desc()->TagIndex("TICKLEN");
      DFloat axisTicklen=(*static_cast<DFloatGDL*>(Struct->GetTag(ticklenTag, 0)))[0]; //![XYZ].TICKLEN (exist)
      e->AssureFloatScalarKWIfPresent(choosenIx, axisTicklen); //overriden by kw
      if (axisTicklen!=0.0) ticklen=axisTicklen;
    }
  }

 static void gdlGetDesiredAxisTickName(EnvT* e, GDLGStream* a, string axis, DStringGDL* &axisTicknameVect)
  {

    static int XTICKNAMEIx = e->KeywordIx("XTICKNAME");
    static int YTICKNAMEIx = e->KeywordIx("YTICKNAME");
    static int ZTICKNAMEIx = e->KeywordIx("ZTICKNAME");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKNAMEIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKNAMEIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKNAMEIx; }
    if ( Struct!=NULL )
    {
      unsigned AxisTicknameTag=Struct->Desc()->TagIndex("TICKNAME");
      axisTicknameVect=static_cast<DStringGDL*>(Struct->GetTag(AxisTicknameTag,0));
    }
    if ( e->GetKW ( choosenIx )!=NULL )
    {
      axisTicknameVect=e->GetKWAs<DStringGDL>( choosenIx );
      //translate format codes here:
//      for (SizeT iname=0; iname < axisTicknameVect->N_Elements(); ++iname) {
//        std::string out = std::string("");
//        a->TranslateFormatCodes(((*axisTicknameVect)[iname]).c_str(),out);
////TBD: not finished, see cases not treated in TransmateFormatCodes (gdlgstream.cpp)
//        (*axisTicknameVect)[iname]=out;
//      }
    }

  }

  static void gdlGetDesiredAxisTicks(EnvT* e, string axis, DLong &axisTicks)
  {
    axisTicks=0;

    static int XTICKSIx = e->KeywordIx("XTICKS");
    static int YTICKSIx = e->KeywordIx("YTICKS");
    static int ZTICKSIx = e->KeywordIx("ZTICKS");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKSIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKSIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKSIx; }

    if ( Struct!=NULL )
    {
      axisTicks=(*static_cast<DLongGDL*>
                (Struct->GetTag
                (Struct->Desc()->TagIndex("TICKS"), 0)))[0];
    }
    e->AssureLongScalarKWIfPresent(choosenIx, axisTicks);
    if (axisTicks > 59) e->Throw("Value of number of ticks is out of allowed range.");
  }
  
  
  //if axis tick units is specified, first tickunit determines how the automatic limits are computed.
  // for example, if tickunits=['year','day'] the limits will be on a round nuber of years.
  // This is conveyed by the code
  static int gdlGetCalendarCode(EnvT* e, string axis)
  {
    static int XTICKUNITSIx = e->KeywordIx("XTICKUNITS");
    static int YTICKUNITSIx = e->KeywordIx("YTICKUNITS");
    static int ZTICKUNITSIx = e->KeywordIx("ZTICKUNITS");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKUNITSIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKUNITSIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKUNITSIx; }
    DStringGDL* axisTickunitsVect=NULL;
    if ( Struct!=NULL )
    {
      unsigned AxisTickunitsTag=Struct->Desc()->TagIndex("TICKUNITS");
      axisTickunitsVect=static_cast<DStringGDL*>(Struct->GetTag(AxisTickunitsTag,0));
    }
    if ( e->GetKW ( choosenIx )!=NULL )
    {
      axisTickunitsVect=e->GetKWAs<DStringGDL>( choosenIx );
    }
    int code=0;
    DString what=StrUpCase((*axisTickunitsVect)[0]);
    if (what.substr(0,4)=="YEAR") code=1;
    else if (what.substr(0,5)=="MONTH") code=2;
    else if (what.substr(0,3)=="DAY") code=3;
    else if (what.substr(0,7)=="NUMERIC") code=3;
    else if (what.substr(0,4)=="HOUR") code=4;
    else if (what.substr(0,6)=="MINUTE") code=5;
    else if (what.substr(0,6)=="SECOND") code=6;
    else if (what.substr(0,4)=="TIME") code=7;
    return code;
  }
 
 static void gdlGetDesiredAxisTickUnits(EnvT* e, string axis, DStringGDL* &axisTickunitsVect)
  {
    static int XTICKUNITSIx = e->KeywordIx("XTICKUNITS");
    static int YTICKUNITSIx = e->KeywordIx("YTICKUNITS");
    static int ZTICKUNITSIx = e->KeywordIx("ZTICKUNITS");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKUNITSIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKUNITSIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKUNITSIx; }
   if ( Struct!=NULL )
    {
      unsigned AxisTickunitsTag=Struct->Desc()->TagIndex("TICKUNITS");
      axisTickunitsVect=static_cast<DStringGDL*>(Struct->GetTag(AxisTickunitsTag,0));
    }
    if ( e->GetKW ( choosenIx )!=NULL )
    {
      axisTickunitsVect=e->GetKWAs<DStringGDL>( choosenIx );
    }
      }

  static void gdlGetDesiredAxisTickv(EnvT* e, string axis, DDoubleGDL* axisTickvVect)
  {
    static int XTICKVIx = e->KeywordIx("XTICKV");
    static int YTICKVIx = e->KeywordIx("YTICKV");
    static int ZTICKVIx = e->KeywordIx("ZTICKV");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTICKVIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTICKVIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTICKVIx; }
    if ( Struct!=NULL )
    {
      unsigned AxisTickvTag=Struct->Desc()->TagIndex("TICKV");
      axisTickvVect=static_cast<DDoubleGDL*>(Struct->GetTag(AxisTickvTag,0));

    }
    if ( e->GetKW ( choosenIx )!=NULL )
    {
      axisTickvVect=e->GetKWAs<DDoubleGDL>( choosenIx );
    }
  }

  static void gdlGetDesiredAxisTitle(EnvT *e, string axis, DString &title)
  {
    static int XTITLEIx = e->KeywordIx("XTITLE");
    static int YTITLEIx = e->KeywordIx("YTITLE");
    static int ZTITLEIx = e->KeywordIx("ZTITLE");
    int choosenIx;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) { Struct=SysVar::X(); choosenIx=XTITLEIx; }
    if ( axis=="Y" ) { Struct=SysVar::Y(); choosenIx=YTITLEIx; }
    if ( axis=="Z" ) { Struct=SysVar::Z(); choosenIx=ZTITLEIx; }

    if ( Struct!=NULL )
    {
      unsigned titleTag=Struct->Desc()->TagIndex("TITLE");
      title=
      (*static_cast<DStringGDL*>(Struct->GetTag(titleTag, 0)))[0];
    }

    e->AssureStringScalarKWIfPresent(choosenIx, title);
  }

    static void gdlSetLineStyle(EnvT *e, GDLGStream *a)
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DLong linestyle=
    (*static_cast<DLongGDL*>
     (pStruct->GetTag(pStruct->Desc()->TagIndex("LINESTYLE"), 0)))[0];

    // if the LINESTYLE keyword is present, the value will be change
    DLong linestyleNew=-1111;
    static int linestyleIx = e->KeywordIx("LINESTYLE");

    if (e->KeywordSet(linestyleIx)) e->AssureLongScalarKWIfPresent(linestyleIx, linestyleNew);

    bool debug=false;
    if ( debug )
    {
      cout<<"temp_linestyle "<<linestyleNew<<endl;
      cout<<"     linestyle "<<linestyle<<endl;
    }
    if ( linestyleNew!= -1111 )
    {
      linestyle=linestyleNew;
    }//+1;
    if ( linestyle<0 )
    {
      linestyle=0;
    }
    if ( linestyle>5 )
    {
      linestyle=5;
    }
    gdlLineStyle(a, linestyle);
  }
    

  
  static DFloat gdlGetPenThickness(EnvT *e, GDLGStream *a)
  {
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    DFloat thick=(*static_cast<DFloatGDL*>
                  (pStruct->GetTag(pStruct->Desc()->TagIndex("THICK"), 0)))[0];

    static int THICKIx = e->KeywordIx("THICK");
    e->AssureFloatScalarKWIfPresent(THICKIx, thick);
    if ( thick <= 0.0 ) thick=1.0;
    return thick;
  }
  
  static void gdlSetPenThickness(EnvT *e, GDLGStream *a)
  {
    a->Thick(gdlGetPenThickness(e, a));
  }
  
  static void gdlWriteTitleAndSubtitle(EnvT* e, GDLGStream *a)
  {
    unsigned titleTag=SysVar::P()->Desc()->TagIndex("TITLE");
    unsigned subTitleTag=SysVar::P()->Desc()->TagIndex("SUBTITLE");
    DString title=(*static_cast<DStringGDL*>(SysVar::P()->GetTag(titleTag, 0)))[0];
    DString subTitle=(*static_cast<DStringGDL*>(SysVar::P()->GetTag(subTitleTag, 0)))[0];

    static int TITLEIx = e->KeywordIx("TITLE");
    static int SUBTITLEIx = e->KeywordIx("SUBTITLE");
    e->AssureStringScalarKWIfPresent(TITLEIx, title);
    e->AssureStringScalarKWIfPresent(SUBTITLEIx, subTitle);
    if (title.empty() && subTitle.empty()) return;

    gdlSetPlotCharsize(e, a);
    if (!title.empty())
    {
      e->AssureStringScalarKWIfPresent(TITLEIx, title);
      gdlSetPlotCharthick(e, a);
      a->sizeChar(1.25*a->charScale());
      a->mtex("t", 1.5, 0.5, 0.5, title.c_str()); //position is in units of current char height. baseline at half-height
      a->sizeChar(a->charScale()/1.25);
    }
    if (!subTitle.empty()) 
    {
      e->AssureStringScalarKWIfPresent(SUBTITLEIx, subTitle);
      DFloat step=a->mmLineSpacing()/a->mmCharHeight();
      a->mtex("b", 5*step, 0.5, 0.5, subTitle.c_str());
    }
 }
  //call this function if Y data is strictly >0.
  //set yStart to 0 only if gdlYaxisNoZero is false.
    static bool gdlYaxisNoZero(EnvT* e)
  {
    //no explict range given?
    DDouble test1, test2;
    unsigned rangeTag=SysVar::Y()->Desc()->TagIndex("RANGE");
    test1=(*static_cast<DDoubleGDL*>(SysVar::Y()->GetTag(rangeTag, 0)))[0];
    test2=(*static_cast<DDoubleGDL*>(SysVar::Y()->GetTag(rangeTag, 0)))[1];
    if(!(test1==0.0 && test2==0.0)) return TRUE;
    static int YRANGEIx=e->KeywordIx( "YRANGE"); 

    if ( e->KeywordSet( YRANGEIx)) return TRUE;
    //Style contains 1?
    DLong ystyle;
    gdlGetDesiredAxisStyle(e, "Y", ystyle);
    if (ystyle&1) return TRUE;

    DLong nozero=0;
    if (ystyle&16) nozero=1;
    static int YNOZEROIx=e->KeywordIx( "YNOZERO");
    if ( e->KeywordSet(YNOZEROIx)) nozero = 1;
    return (nozero==1);
  }


  //advance to next plot unless the noerase flag is set
  // function declared static (local to each function using it) to avoid messing the NOERASEIx index which is not the same.
  static void gdlNextPlotHandlingNoEraseOption(EnvT *e, GDLGStream *a, bool noe=0)
  {
    bool noErase=FALSE;
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 

    if ( !noe )
    {
      DLong LnoErase=(*static_cast<DLongGDL*>
                      (pStruct->
                       GetTag(pStruct->Desc()->TagIndex("NOERASE"), 0)))[0];
      noErase=(LnoErase==1);
      static int NOERASEIx = e->KeywordIx("NOERASE");

      if ( e->KeywordSet(NOERASEIx) )
      {
        noErase=TRUE;
      }
    }
    else
    {
      noErase=TRUE;
    }

    a->NextPlot(!noErase);
      // all but the first element of !P.MULTI are ignored if POSITION kw or !P.POSITION or !P.REGION is specified
    // TODO: !P.REGION!

    DFloatGDL* pos=NULL;

    // system variable !P.REGION first ?? TODO 
    pos=static_cast<DFloatGDL*>(pStruct-> GetTag(pStruct->Desc()->TagIndex("POSITION"), 0));
    if ( (*pos)[0]==(*pos)[2] ) pos=NULL; //ignored

    // keyword
    if ( pos==NULL )
    {
      static int positionIx=e->KeywordIx("POSITION");
      if ( e->GetKW ( positionIx )!=NULL )
      {
       pos=e->GetKWAs<DFloatGDL>(positionIx);
      }
    }
    if ( pos!=NULL ) a->NoSub();
  }
  static bool gdlSet3DViewPortAndWorldCoordinates(EnvT* e,
                                           GDLGStream* actStream,
                                           DDoubleGDL* Matrix,
                                           bool xLog, bool yLog,
                                           DDouble xStart,
                                           DDouble xEnd,
                                           DDouble yStart,
                                           DDouble yEnd, DDouble zStart=0.0, DDouble zEnd=1.0, bool zLog=false)
  {

   // set ![XY].CRANGE Before doing anything relative to 3D.
    gdlStoreAxisCRANGE("X", xStart, xEnd, xLog);
    gdlStoreAxisCRANGE("Y", yStart, yEnd, yLog);
    gdlStoreAxisCRANGE("Z", zStart, zEnd, zLog);
    //set ![XY].type
    gdlStoreAxisType("X",xLog);
    gdlStoreAxisType("Y",yLog);
    gdlStoreAxisType("Z",zLog);
    //set ![XY].WINDOW and ![XY].S
    gdlStoreAxisSandWINDOW(actStream, "X", xStart, xEnd, xLog);
    gdlStoreAxisSandWINDOW(actStream, "Y", yStart, yEnd, yLog);
    gdlStoreAxisSandWINDOW(actStream, "Z", zStart, zEnd, zLog);

    //3D work
    enum{ DATA=0,
          NORMAL,
          DEVICE
        } coordinateSystem=DATA;
    //To center plot, compute projected corners of 1 unit box
    static DDouble zz[8]={0,0,0,0,1,1,1,1};
    static DDouble yy[8]={0,0,1,1,0,0,1,1};
    static DDouble xx[8]={0,1,0,1,0,1,0,1};
    static DDouble ww[8]={1,1,1,1,1,1,1,1};

    DDoubleGDL* V=(new DDoubleGDL(dimension(8,4)));
    memcpy(&((*V)[0]),xx,8*sizeof(double));
    memcpy(&((*V)[8]),yy,8*sizeof(double));
    memcpy(&((*V)[16]),zz,8*sizeof(double));
    memcpy(&((*V)[24]),ww,8*sizeof(double));

    DDoubleGDL* pV=(Matrix->MatrixOp(V,false,true));

    DDouble xmin,xmax,ymin,ymax;
    DLong iMin,iMax;
    pV->MinMax(&iMin,&iMax,NULL,NULL,false,0,0,4);
    xmin=(*pV)[iMin];
    xmax=(*pV)[iMax];
    pV->MinMax(&iMin,&iMax,NULL,NULL,false,1,0,4);
    ymin=(*pV)[iMin];
    ymax=(*pV)[iMax];

    PLFLT xMR, xML, yMB, yMT;
    DFloat xMarginL, xMarginR, yMarginB, yMarginT;
    gdlGetDesiredAxisMargin(e, "X", xMarginL, xMarginR);
    gdlGetDesiredAxisMargin(e, "Y", yMarginB, yMarginT);
    PLFLT sclx=actStream->nCharLength(); //current char width
    xML=xMarginL*sclx; //margin as percentage of subpage
    xMR=xMarginR*sclx;
    PLFLT scly=actStream->nLineSpacing();
    yMB=(yMarginB)*scly;
    yMT=(yMarginT)*scly;

    if ( xML+xMR>=1.0 )
    {
      PLFLT xMMult=xML+xMR;
      xML/=xMMult*1.5;
      xMR/=xMMult*1.5;
    }
    if ( yMB+yMT>=1.0 )
    {
      PLFLT yMMult=yMB+yMT;
      yMB/=yMMult*1.5;
      yMT/=yMMult*1.5;
    }

    static PLFLT positionP[4]={0, 0, 0, 0};
    static PLFLT regionP[4]={0, 0, 0, 0};
    static PLFLT position[4]={0,0,1,1};
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    // Get !P.position values. !P.REGION is superseded by !P.POSITION
    if ( pStruct!=NULL )
    {
      
      unsigned regionTag=pStruct->Desc()->TagIndex("REGION");
      for ( SizeT i=0; i<4; ++i ) regionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(pStruct->GetTag(regionTag, 0)))[i];
      unsigned positionTag=pStruct->Desc()->TagIndex("POSITION");
      for ( SizeT i=0; i<4; ++i ) positionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(pStruct->GetTag(positionTag, 0)))[i];
    }
    if (regionP[0]!=regionP[2] && positionP[0]==positionP[2]) //if not ignored, and will be used, as 
                //a surrogate of !P.Position:
    {
        //compute position removing margins
        positionP[0]=regionP[0]+xMarginL*actStream->nCharLength();
        positionP[1]=regionP[1]+yMarginB*actStream->nLineSpacing();//nCharHeight();
        positionP[2]=regionP[2]-xMarginR*actStream->nCharLength();
        positionP[3]=regionP[3]-yMarginT*actStream->nLineSpacing();//nCharHeight();
    }
    //compatibility: Position NEVER outside [0,1]:
    positionP[0]=max(0.0,positionP[0]);
    positionP[1]=max(0.0,positionP[1]);
    positionP[2]=min(1.0,positionP[2]);
    positionP[3]=min(1.0,positionP[3]);

    //check presence of DATA,DEVICE and NORMAL options
    static int DATAIx=e->KeywordIx("DATA");
    static int DEVICEIx=e->KeywordIx("DEVICE");
    static int NORMALIx=e->KeywordIx("NORMAL");

    if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
    if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
    if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
//    if (coordinateSystem==DATA && !actStream->validWorldBox()) e->Throw("PLOT: Data coordinate system not established.");
    // read boxPosition if needed
    static int positionIx = e->KeywordIx( "POSITION");
    DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    if (boxPosition == NULL) boxPosition = (DFloatGDL*) 0xF;
    if ( boxPosition!=(DFloatGDL*)0xF)
    {
      for ( SizeT i=0; i<4&&i<boxPosition->N_Elements(); ++i ) position[i]=(*boxPosition)[i];
    }
    // modify positionP and/or boxPosition to NORMAL if DEVICE is present
    if (coordinateSystem==DEVICE)
    {
      PLFLT normx;
      PLFLT normy;
      actStream->DeviceToNormedDevice(positionP[0], positionP[1], normx, normy);
      positionP[0]=normx;
      positionP[1]=normy;
      actStream->DeviceToNormedDevice(positionP[2], positionP[3], normx, normy);
      positionP[2]=normx;
      positionP[3]=normy;
      if ( boxPosition!=(DFloatGDL*)0xF)
      {
        actStream->DeviceToNormedDevice(position[0], position[1], normx, normy);
        position[0]=normx;
        position[1]=normy;
        actStream->DeviceToNormedDevice(position[2], position[3], normx, normy);
        position[2]=normx;
        position[3]=normy;
      }
    }
    if ( boxPosition!=(DFloatGDL*)0xF)
    {    //compatibility again: Position NEVER outside [0,1]:
      position[0]=max(0.0,position[0]);
      position[1]=max(0.0,position[1]);
      position[2]=min(1.0,position[2]);
      position[3]=min(1.0,position[3]);
    }

    // New plot without POSITION=[] as argument
    if ( boxPosition==(DFloatGDL*)0xF )
    {
      // If !P.position not set use default values. coordinatesSystem not used even if present!
      if ( positionP[0]==0&&positionP[1]==0&&
           positionP[2]==0&&positionP[3]==0 )
      {
        // Set to (smart?) default values
        position[0]=0;
        position[1]=0+2*(yMB/yMarginB); //subtitle
        position[2]=1.0;
        position[3]=1.0-2*(yMT/yMarginT); //title
        actStream->vpor(position[0], position[2], position[1], position[3]);
      }
      else
      {
        // Use !P.position values.
        actStream->vpor(positionP[0], positionP[2], positionP[1], positionP[3]);
     }
    }
    else // Position keyword set
    {
      actStream->vpor(position[0], position[2], position[1], position[3]);
    }
    //adjust 'world' values to give room to axis labels. Could be better if we take
    //into account projection angles
    // fix word values without labels:
    actStream->wind(xmin, xmax, ymin, ymax);
    //compute world Charsize
    PLFLT xb, xe, yb, ye;
    xb=xmin-xMarginL*actStream->wCharLength();
    xe=xmax+xMarginR*actStream->wLineSpacing();
    yb=ymin-yMarginB*actStream->wCharHeight();
    ye=ymax-yMarginT*actStream->wLineSpacing();
    actStream->wind(xb, xe, yb, ye);


    //Clipping is false in 3D... 

    //set P.CLIP (done by PLOT, CONTOUR, SHADE_SURF, and SURFACE)
    Guard<BaseGDL> clipbox_guard;
    DLongGDL* clipBox= new DLongGDL(4, BaseGDL::ZERO); clipbox_guard.Reset(clipBox);
    PLFLT x,y;
    actStream->gvpd(xmin, xmax, ymin, ymax);

    actStream->NormedDeviceToDevice(xmin, ymin, x,y);
    (*clipBox)[0]=x;
    (*clipBox)[1]=y;
    actStream->NormedDeviceToDevice(xmax, ymax,x,y);
    (*clipBox)[2]=x;
    (*clipBox)[3]=y;
    gdlStoreCLIP(clipBox);
    return true;
  }
    //TODO: put margin discovery in gdlSetViewPortAndWorldCoordinates (simplify call list)
  //also, solve the proble of passing back xStart etc if they are changed by unwantedaxisvalue())
  
  static bool gdlSetViewPortAndWorldCoordinates(EnvT* e,
                                         GDLGStream* actStream,
                                         bool xLog, bool yLog,
                                         DFloat xMarginL,
                                         DFloat xMarginR,
                                         DFloat yMarginB,
                                         DFloat yMarginT,
                                         DDouble xStart,
                                         DDouble xEnd,
                                         DDouble yStart,
                                         DDouble yEnd,
                                         DLong iso)
  {

    PLFLT xMR;
    PLFLT xML;
    PLFLT yMB;
    PLFLT yMT;
    enum{ DATA=0,
          NORMAL,
          DEVICE
        } coordinateSystem=DATA;

    CheckMargin(actStream,
                xMarginL,
                xMarginR,
                yMarginB,
                yMarginT,
                xMR, xML, yMB, yMT);

    // viewport - POSITION overrides
    static bool kwP=FALSE;
    static bool do_iso=FALSE;
    static PLFLT aspect=0.0;

    static PLFLT positionP[4]={0, 0, 0, 0};
    static PLFLT regionP[4]={0, 0, 0, 0};
    static PLFLT position[4]={0,0,1,1};
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    // Get !P.position values. !P.REGION is superseded by !P.POSITION
    if ( pStruct!=NULL )
    {
      
      unsigned regionTag=pStruct->Desc()->TagIndex("REGION");
      for ( SizeT i=0; i<4; ++i ) regionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(pStruct->GetTag(regionTag, 0)))[i];
      unsigned positionTag=pStruct->Desc()->TagIndex("POSITION");
      for ( SizeT i=0; i<4; ++i ) positionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(pStruct->GetTag(positionTag, 0)))[i];
    }
    if (regionP[0]!=regionP[2] && positionP[0]==positionP[2]) //if not ignored, and will be used, as 
                //a surrogate of !P.Position:
    {
        //compute position removing margins
        positionP[0]=regionP[0]+xMarginL*actStream->nCharLength();
        positionP[1]=regionP[1]+yMarginB*actStream->nLineSpacing();
        positionP[2]=regionP[2]-xMarginR*actStream->nCharLength();
        positionP[3]=regionP[3]-yMarginT*actStream->nLineSpacing();
    }
    //compatibility: Position NEVER outside [0,1]:
    positionP[0]=max(0.0,positionP[0]);
    positionP[1]=max(0.0,positionP[1]);
    positionP[2]=min(1.0,positionP[2]);
    positionP[3]=min(1.0,positionP[3]);
    
    //check presence of DATA,DEVICE and NORMAL options
    static int DATAIx=e->KeywordIx("DATA");
    static int DEVICEIx=e->KeywordIx("DEVICE");
    static int NORMALIx=e->KeywordIx("NORMAL");

    if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
    if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
    if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
//    if (coordinateSystem==DATA && !actStream->validWorldBox()) e->Throw("PLOT: Data coordinate system not established.");
    // read boxPosition if needed
    static int positionIx = e->KeywordIx( "POSITION");
    DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    if (boxPosition == NULL) boxPosition = (DFloatGDL*) 0xF;
    if ( boxPosition!=NULL && boxPosition!=(DFloatGDL*)0xF )
    {
      for ( SizeT i=0; i<4&&i<boxPosition->N_Elements(); ++i ) position[i]=(*boxPosition)[i];
    }
    // modify positionP and/or boxPosition to NORMAL if DEVICE is present
    if (coordinateSystem==DEVICE)
    {
      PLFLT normx;
      PLFLT normy;
      actStream->DeviceToNormedDevice(positionP[0], positionP[1], normx, normy);
      positionP[0]=normx;
      positionP[1]=normy;
      actStream->DeviceToNormedDevice(positionP[2], positionP[3], normx, normy);
      positionP[2]=normx;
      positionP[3]=normy;
      if ( boxPosition!=NULL && boxPosition!=(DFloatGDL*)0xF )
      {
        actStream->DeviceToNormedDevice(position[0], position[1], normx, normy);
        position[0]=normx;
        position[1]=normy;
        actStream->DeviceToNormedDevice(position[2], position[3], normx, normy);
        position[2]=normx;
        position[3]=normy;
     }
    }
    if ( boxPosition!=NULL && boxPosition!=(DFloatGDL*)0xF )
    {
       //compatibility again: Position NEVER outside [0,1]:
      position[0]=max(0.0,position[0]);
      position[1]=max(0.0,position[1]);
      position[2]=min(1.0,position[2]);
      position[3]=min(1.0,position[3]);
    }
    // Adjust Start and End for Log (convert to log)
    if ( boxPosition!=NULL ) //new box
    {
      if ( xLog )
      {
        gdlHandleUnwantedAxisValue(xStart, xEnd, xLog);
        xStart=log10(xStart);
        xEnd=log10(xEnd);
      }
      if ( yLog )
      {
        gdlHandleUnwantedAxisValue(yStart, yEnd, yLog);
        yStart=log10(yStart);
        yEnd=log10(yEnd);
      }
    }
    // If pos == NULL (oplot, /OVERPLOT etc: Reuse previous values)
    if ( boxPosition==NULL )
    {
      // If position keyword previously set
      if ( kwP )
      {
        // Creates a viewport with the specified normalized subpage coordinates.
        if ( do_iso ) setIsoPort(actStream, position[0], position[2], position[1], position[3], aspect);
        else actStream->vpor(position[0], position[2], position[1], position[3]);
      }
      else
      {
        // If !P.position not set
        if ( positionP[0]==0&&positionP[1]==0&&
             positionP[2]==0&&positionP[3]==0 )
        {
          if ( do_iso ) setIsoPort(actStream, position[0], position[2], position[1], position[3], aspect);
          else actStream->vpor(position[0], position[2], position[1], position[3]);
        }
        else
        {
          // !P.position set
          if ( do_iso ) setIsoPort(actStream, positionP[0], positionP[2], positionP[1], positionP[3], aspect);
          else actStream->vpor(positionP[0], positionP[2], positionP[1], positionP[3]);
        }
      }
    }
    else //New Plot
    {
      if ( iso==1 ) // Check ISOTROPIC first
      {
        do_iso=TRUE;
        aspect=abs((yEnd-yStart)/(xEnd-xStart)); //log-log or lin-log
      }
      else
      {
        do_iso=FALSE;
        aspect=0.0; // vpas with aspect=0.0 equals vpor.
      }

      // New plot without POSITION=[] as argument
      if ( boxPosition==(DFloatGDL*)0xF )
      {
        kwP=false;
        // If !P.position not set use default values. coordinatesSystem not used even if present!
        if ( positionP[0]==0&&positionP[1]==0&&
             positionP[2]==0&&positionP[3]==0 )
        {

          // Set to default values
          position[0]=xML;
          position[1]=yMB;
          position[2]=1.0-xMR;
          position[3]=1.0-yMT;
          if ( do_iso ) setIsoPort(actStream, position[0], position[2], position[1], position[3], aspect);
          else actStream->vpor(position[0], position[2], position[1], position[3]);
        }
        else
        {
          // Use !P.position values.
          if ( do_iso ) setIsoPort(actStream, positionP[0], positionP[2], positionP[1], positionP[3], aspect);
          else actStream->vpor(positionP[0], positionP[2], positionP[1], positionP[3]);
        }
      }
      else // Position keyword set
      {
        kwP=true;
        if ( do_iso ) setIsoPort(actStream, position[0], position[2], position[1], position[3], aspect);
        else actStream->vpor(position[0], position[2], position[1], position[3]);
      }
    }

    // for OPLOT start and end values are already log
    // SA: changing only local variables!

    //cout << "VP wind: "<<xStart<<" "<<xEnd<<" "<<yStart<<" "<<yEnd<<endl;
    //printf("data lim (setv): %f %f %f %f\n", xStart, xEnd, yStart, yEnd);
    // set world coordinates
    //protection against silly coordinates
    if (xStart==xEnd)
    {
      Message(e->GetProName()+"Coordinate system in error, please report to authors.");
      xStart=0.0;
      xEnd=1.0;
    }
    if (yStart==yEnd)
    {
      Message(e->GetProName()+"Coordinate system in error, please report to authors.");
      yStart=0.0;
      yEnd=1.0;
    }
    actStream->wind(xStart, xEnd, yStart, yEnd);
    //       cout << "xStart " << xStart << "  xEnd "<<xEnd<<endl;
    //        cout << "yStart " << yStart << "  yEnd "<<yEnd<<endl;
    
    // set ![XYZ].CRANGE (Z is not defined but must be [0,1])
    gdlStoreAxisCRANGE("X", xStart, xEnd, FALSE); //already in log here if relevant!
    gdlStoreAxisCRANGE("Y", yStart, yEnd, FALSE);

    //set ![XY].type
    gdlStoreAxisType("X",xLog); 
    gdlStoreAxisType("Y",yLog);

    //set ![XY].WINDOW and ![XY].S
    gdlStoreAxisSandWINDOW(actStream, "X", xStart, xEnd, FALSE);//already in log here if relevant!
    gdlStoreAxisSandWINDOW(actStream, "Y", yStart, yEnd, FALSE);
    //set P.CLIP (done by PLOT, CONTOUR, SHADE_SURF, and SURFACE)
    Guard<BaseGDL> clipbox_guard;
    DLongGDL* clipBox= new DLongGDL(4, BaseGDL::ZERO); clipbox_guard.Reset(clipBox);
    PLFLT xmin, xmax, ymin, ymax, x,y;
    actStream->gvpd(xmin, xmax, ymin, ymax);

    actStream->NormedDeviceToDevice(xmin, ymin, x,y);
    (*clipBox)[0]=x;
    (*clipBox)[1]=y;
    actStream->NormedDeviceToDevice(xmax, ymax,x,y);
    (*clipBox)[2]=x;
    (*clipBox)[3]=y;
    gdlStoreCLIP(clipBox);
    return true;
  }
  
  static bool startClipping(EnvT *e, GDLGStream *a, bool canUsePClip=false)
  {
    if (GDL_DEBUG_PLSTREAM)  fprintf(stderr,"startClipping\n");
    //function to be called when clipping must be actived, i.e., if the combination of CLIP= and NOCLIP= necessitate it
    //the function retrieves the pertinent information in keywords
    enum
    {
      DATA=0,
      NORMAL,
      DEVICE
    } coordinateSystem=DATA;
    bool xinverted=FALSE;
    bool yinverted=FALSE; //for inverted DATA coordinates
    

    
    static int clippingix=e->KeywordIx("CLIP");
    DFloatGDL* clipBox=NULL;
    clipBox=e->IfDefGetKWAs<DFloatGDL>(clippingix);

    //Get saveBox
    gdlSavebox* saveBox=getSaveBox();
    //Save current box
    a->gvpd(saveBox->nx1, saveBox->nx2, saveBox->ny1, saveBox->ny2); //save norm of current box
    a->gvpw(saveBox->wx1, saveBox->wx2, saveBox->wy1, saveBox->wy2); //save world of current box
    saveBox->initialized=true; //mark as initialized (debug complicated clipping algo)
    //test axis inversion
    xinverted=(saveBox->wx1>saveBox->wx2);
    yinverted=(saveBox->wy1>saveBox->wy2);
    //GET CLIPPING
    PLFLT dClipBox[4]={0, 0, 0, 0};
    PLFLT tempbox[4]={0, 0, 0, 0};
    DDouble un, deux, trois, quatre;
    static int NOCLIPIx=e->KeywordIx("NOCLIP");
    static string proname=e->GetProName();
    static bool invertedMeaning=(proname=="PLOTS"||proname=="POLYFILL"||proname=="XYOUTS");
    int noclipvalue=1;
    e->AssureLongScalarKWIfPresent( NOCLIPIx, noclipvalue);
    bool willNotClip;
    //eliminate simple cases
    if (invertedMeaning) willNotClip=(noclipvalue==1); else willNotClip=(e->KeywordSet(NOCLIPIx));
    if (willNotClip) return false;
    if ( !canUsePClip  && clipBox==NULL ) return false;
    if ( !canUsePClip  && clipBox->N_Elements()<4) return false;
    //now we can start checking more deeply
    if (proname != "OPLOT") {     //OPLOT has no /DEVICE /NORM and is already /DATA
     static int DATAIx=e->KeywordIx("DATA");
     static int DEVICEIx=e->KeywordIx("DEVICE");
     static int NORMALIx=e->KeywordIx("NORMAL");

     if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
     if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
     if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
    }

    if ( clipBox==NULL && canUsePClip ) //get !P.CLIP. Coordinates are always DEVICE
      {
        DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
        unsigned clipTag=pStruct->Desc()->TagIndex("CLIP"); //is in device coordinates
        for ( int i=0; i<4; ++i ) tempbox[i]=dClipBox[i]=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i];
        coordinateSystem = DEVICE; //is in device coordinates
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "using !P.CLIP=[%f,%f,%f,%f]\n", dClipBox[0], dClipBox[1], dClipBox[2], dClipBox[3]);
      }
      else //get units, convert to world coords for plplot, take care of axis direction
      {
        if ( (*clipBox)[0]>=(*clipBox)[2] ||(*clipBox)[1]>=(*clipBox)[3] ) {
         coordinateSystem=NORMAL;
         tempbox[0]=0.0;
         tempbox[1]=0.0;
         tempbox[2]=0.00001; //ridiculous but works.
         tempbox[3]=0.00001;
        } else for ( int i=0; i<4&&i<clipBox->N_Elements(); ++i ) tempbox[i]=dClipBox[i]=(*clipBox)[i];
        
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "using given CLIP=[%f,%f,%f,%f]\n", dClipBox[0], dClipBox[1], dClipBox[2], dClipBox[3]);
        if ( coordinateSystem==DATA )
        {
          int *tx,*ty;
          int txn[2]={0,2};
          int txr[2]={2,0};
          int tyn[2]={1,3};
          int tyr[2]={3,1};
          if(tempbox[0]<tempbox[2]) { if (xinverted) tx=txr; else tx=txn;} else { if (xinverted) tx=txn; else tx=txr;}
          if(tempbox[1]<tempbox[3]) { if (yinverted) ty=tyr; else ty=tyn;} else { if (yinverted) ty=tyn; else ty=tyr;}
          un=tempbox[tx[0]];
          deux=tempbox[ty[0]];
          a->WorldToDevice(un, deux, trois, quatre);
          dClipBox[0]=trois;
          dClipBox[1]=quatre;
          un=tempbox[tx[1]];
          deux=tempbox[ty[1]];
          a->WorldToDevice(un, deux, trois, quatre);
          dClipBox[2]=trois;
          dClipBox[3]=quatre;
        }
        else if ( coordinateSystem==NORMAL )
        {
          a->NormToDevice(tempbox[0], tempbox[1], dClipBox[0], dClipBox[1]);
          a->NormToDevice(tempbox[2], tempbox[3], dClipBox[2], dClipBox[3]);
        }
      }
    // we are now in DEVICE Coords
//    }
    //if new box is in error, return it:
    if (dClipBox[0]>=dClipBox[2]||dClipBox[1]>=dClipBox[3]) return false;
    //compute and set corresponding world coords before using whole page:
    a->DeviceToWorld(dClipBox[0], dClipBox[1],tempbox[0], tempbox[1]);
    a->DeviceToWorld(dClipBox[2], dClipBox[3],tempbox[2], tempbox[3]);

    a->NoSub();
    // set full page viewport for the clip box boundaries:
    PLFLT xmin,xmax,ymin,ymax;
    a->DeviceToNormedDevice(dClipBox[0], dClipBox[1],xmin, ymin);
    a->DeviceToNormedDevice(dClipBox[2], dClipBox[3],xmax, ymax);
    a->vpor(xmin, xmax,ymin, ymax);
    a->wind(tempbox[0], tempbox[2], tempbox[1], tempbox[3]);
//    a->box( "bc", 0, 0, "bc", 0.0, 0);
    return TRUE;
  }
    static bool gdlAxis(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log,
    DLong modifierCode=0, DDouble NormedLength=0)
  {
    static GDL_TICKNAMEDATA data;
    static GDL_MULTIAXISTICKDATA muaxdata;

    static GDL_TICKDATA tdata;
    tdata.a=a;
    tdata.isLog=Log;
    tdata.axisrange=abs(End-Start);

    data.a=a;
    data.nTickName=0;
    data.axisrange=abs(End-Start);
    muaxdata.e=e;
    muaxdata.a=a;
    muaxdata.what=GDL_NONE;
    muaxdata.nTickFormat=0;
    muaxdata.nTickUnits=0;
    muaxdata.axismin=Start;
    muaxdata.axismax=End;
    muaxdata.axisrange=abs(End-Start);

    //special values
    PLFLT OtherAxisSizeInMm;
    if (axis=="X") OtherAxisSizeInMm=a->mmyPageSize()*(a->boxnYSize());
    if (axis=="Y") OtherAxisSizeInMm=a->mmxPageSize()*(a->boxnXSize());
    //special for AXIS who change the requested box size!
    if (axis=="axisX") {axis="X"; OtherAxisSizeInMm=a->mmyPageSize()*(NormedLength);}
    if (axis=="axisY") {axis="Y"; OtherAxisSizeInMm=a->mmxPageSize()*(NormedLength);}
    
    DLong GridStyle;
    gdlGetDesiredAxisGridStyle(e, axis, GridStyle);
    DLong Minor;
    gdlGetDesiredAxisMinor(e, axis, Minor);
    DLong Style;
    gdlGetDesiredAxisStyle(e, axis, Style);
    DFloat Thick;
    gdlGetDesiredAxisThick(e, axis, Thick);
    DStringGDL* TickFormat;
    gdlGetDesiredAxisTickFormat(e, axis, TickFormat);
    DDouble TickInterval;
    gdlGetDesiredAxisTickInterval(e, axis, TickInterval);
    DLong TickLayout;
    gdlGetDesiredAxisTickLayout(e, axis, TickLayout);
    DFloat TickLen;
    gdlGetDesiredAxisTickLen(e, axis, TickLen);
    DStringGDL* TickName;
    gdlGetDesiredAxisTickName(e, a, axis, TickName);
    DLong Ticks;
    gdlGetDesiredAxisTicks(e, axis, Ticks);
    DStringGDL* TickUnits;
    gdlGetDesiredAxisTickUnits(e, axis, TickUnits);
//    DDoubleGDL *Tickv;
//    gdlGetDesiredAxisTickv(e, axis, Tickv);
    DString Title;
    gdlGetDesiredAxisTitle(e, axis, Title);
    
    bool hasTickUnitDefined = (TickUnits->NBytes()>0);
    int tickUnitArraySize=(hasTickUnitDefined)?TickUnits->N_Elements():0;
    
    //For labels we need ticklen in current character size, for ticks we need it in mm
    DFloat ticklen_in_mm= TickLen;
    if (TickLen<0) ticklen_in_mm*=-1;
    //ticklen in a percentage of box x or y size, to be expressed in mm 
    if (axis=="X") ticklen_in_mm=a->mmyPageSize()*(a->boxnYSize())*ticklen_in_mm;
    if (axis=="Y") ticklen_in_mm=a->mmxPageSize()*(a->boxnXSize())*ticklen_in_mm;
    DFloat ticklen_as_norm=(axis=="X")?a->mm2ndy(ticklen_in_mm):a->mm2ndx(ticklen_in_mm); //in normed coord
    //eventually, each succesive X or Y axis is separated from previous by interligne + ticklen in adequate units. 
    DFloat interligne_as_char;
    DFloat interligne_as_norm;
    DFloat typical_char_size_mm= (axis=="X")?a->mmCharHeight():a->mmCharLength();
    interligne_as_char=(axis=="X")?a->mmLineSpacing()/typical_char_size_mm:a->mmCharLength()/typical_char_size_mm; //in normed coord
    interligne_as_norm=(axis=="X")?a->nLineSpacing():a->nCharLength(); //in normed coord
    DFloat displacement_of_new_axis_as_norm=2*interligne_as_norm+ticklen_as_norm;
    DFloat current_displacement=0;
    DFloat title_position=0;
    if ( (Style&4)!=4 ) //if we write the axis...
    {
      double nchars; //max number of chars written in label of axis. 
      string Opt;
      string otherOpt;
      if (TickInterval==0)
      {
        if (Ticks<=0) TickInterval=gdlComputeTickInterval(e, axis, Start, End, Log);
        else if (Ticks>1) TickInterval=(End-Start)/Ticks;
        else TickInterval=(End-Start);
      } else { //check that tickinterval does not make more than 59 ticks:
       if (abs((End-Start)/TickInterval) > 59) TickInterval=(End-Start)/59;
      }
      //first write labels only:
      gdlSetAxisCharsize(e, a, axis);
      gdlSetPlotCharthick(e, a);

      //axis, 1st time: labels
      Opt="tvx";otherOpt="tv"; //draw major ticks "t" + v:values perp to Y axis + x:
      // the x option is in plplot 5.9.8 but not before. It permits
                // to avoid writing tick marks here (they will be written after)
                // I hope old plplots were clever enough to ignore 'x'
                // if they did not understand 'x'
      if ( Log ) Opt+="l"; //"l" for log; otherOpt is never in log I believe
      if (TickName->NBytes()>0) // /TICKNAME=[array]
      {
        data.counter=0;
        data.nchars=0;
        data.TickName=TickName;
        data.nTickName=TickName->N_Elements();
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlSingleAxisTickNamedFunc, &data );
        Opt+="o";
#endif
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
        nchars=data.nchars;
        if (axis=="Y") title_position=nchars+2.5; else title_position=3.5;
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      //care Tickunits size is 10 if not defined because it is the size of !X.TICKUNITS.
      else if (hasTickUnitDefined) // /TICKUNITS=[several types of axes written below each other]
      {
        muaxdata.counter=0;
        muaxdata.what=GDL_TICKUNITS;
        if (TickFormat->NBytes()>0)  // with also TICKFORMAT option..
        {
          muaxdata.what=GDL_TICKFORMAT_AND_UNITS;
          muaxdata.TickFormat=TickFormat;
          muaxdata.nTickFormat=TickFormat->N_Elements();
        }
        muaxdata.TickUnits=TickUnits;
        muaxdata.nTickUnits=tickUnitArraySize;
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlMultiAxisTickFunc, &muaxdata );
        Opt+="o";otherOpt+="o"; //use external func custom labeling
#endif
        if (modifierCode==2) {Opt+="m"; otherOpt+="m";} else {Opt+="n"; otherOpt+="n";} //m: write numerical/right above, n: below/left (normal)
        PLFLT un,deux,trois,quatre,xun,xdeux,xtrois,xquatre;
        a->getCurrentNormBox(un,deux,trois,quatre);
        a->getCurrentWorldBox(xun,xdeux,xtrois,xquatre);
        a->smaj(ticklen_in_mm, 1.0 );
        for (SizeT i=0; i< muaxdata.nTickUnits; ++i) //loop on TICKUNITS axis
        {
          muaxdata.nchars=0; //set nchars to 0, at the end nchars will be the maximum size.
          if (i>0) Opt=otherOpt+"b"; //supplementary axes are to be wwritten with ticks, no smallticks;
          if (axis=="X") 
          {
            a->vpor(un,deux,trois-current_displacement,quatre);
            a->wind(xun,xdeux,xtrois,xquatre);
            a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0); //to avoid plplot crashes: do not use tickinterval. or recompute it correctly (no too small!)
            title_position=current_displacement/a->nCharHeight()+3.5;
            current_displacement+=displacement_of_new_axis_as_norm; //and the spacing plus the ticklengths
          }
          else if (axis=="Y") 
          {
            a->vpor(un-current_displacement,deux,trois,quatre);
            a->wind(xun,xdeux,xtrois,xquatre);
            a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor); //to avoid plplot crashes: do not use tickinterval. or recompute it correctly (no too small!)
            nchars=muaxdata.nchars;
            title_position=current_displacement/a->nCharLength()+nchars+2.5;
            current_displacement+=(nchars-1)*a->nCharLength(); //we'll skip what was written
            current_displacement+=displacement_of_new_axis_as_norm; //and the spacing plus the ticklengths
          }
          muaxdata.counter++;
        }
        a->vpor(un,deux,trois,quatre);
        a->wind(xun,xdeux,xtrois,xquatre);
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      else if (TickFormat->NBytes()>0) //no /TICKUNITS=> only 1 value taken into account
      {
        muaxdata.counter=0;
        muaxdata.nchars=0;
        muaxdata.what=GDL_TICKFORMAT;
        muaxdata.TickFormat=TickFormat;
        muaxdata.nTickFormat=1;
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlMultiAxisTickFunc, &muaxdata );
        Opt+="o";
#endif
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
        nchars=muaxdata.nchars;
        if (axis=="Y") title_position=nchars+2; else title_position=3.5;
#if (HAVE_PLPLOT_SLABELFUNC)        
        a->slabelfunc( NULL, NULL );
#endif
      }
      else
      {
#if (HAVE_PLPLOT_SLABELFUNC)
        tdata.nchars=0;
        a->slabelfunc( gdlSimpleAxisTickFunc, &tdata );
        Opt+="o";
#endif
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
        nchars=tdata.nchars;
        if (axis=="Y") title_position=nchars+2; else title_position=3.5;
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }

      if (modifierCode==0 ||modifierCode==1)
      {
        if (axis=="X") a->mtex("b",title_position, 0.5, 0.5, Title.c_str());
        else if (axis=="Y") a->mtex("l",title_position,0.5,0.5,Title.c_str());
      }
      else if (modifierCode==2)
      {
        if (axis=="X") a->mtex("t", title_position, 0.5, 0.5, Title.c_str());
        else if (axis=="Y") a->mtex("r",title_position,0.5,0.5,Title.c_str());
      }      
      
      if (TickLayout==0)
      {
        a->smaj(ticklen_in_mm, 1.0); //set base ticks to default 0.02 viewport converted to mm.
        a->smin(ticklen_in_mm/2.0,1.0); //idem min (plplt defaults)
        //thick for box and ticks.
        a->Thick(Thick);

        //ticks or grid eventually with style and length:
        if (abs(TickLen)<1e-6) Opt=""; else Opt="st"; //remove ticks if ticklen=0
        if (TickLen<0) {Opt+="i"; TickLen=-TickLen;}
        switch(modifierCode)
        {
          case 2:
            Opt+="c";
            break;
          case 1:
            Opt+="b";
            break;
          case 0:
            if ( (Style&8)==8 ) Opt+="b"; else Opt+="bc";
        }
        //gridstyle applies here:
        gdlLineStyle(a,GridStyle);
        if ( Log ) Opt+="l";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0, 0, Opt.c_str(), TickInterval, Minor);
        //reset gridstyle
        gdlLineStyle(a,0);
        // pass over with outer box, with thick. No style applied, only ticks
        Opt=" ";
        switch(modifierCode)
        {
          case 2:
            Opt+="c";
            break;
          case 1:
            Opt+="b";
            break;
          case 0:
            if ( (Style&8)==8 ) Opt+="b"; else Opt+="bc";
        }
        if (axis=="X") a->box(Opt.c_str(), 0.0, 0, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0, 0 , Opt.c_str(), 0.0, 0);
      }
      //reset charsize & thick
      a->Thick(1.0);
      a->sizeChar(1.0);
    }
	return 0;
  }


  static bool gdlBox(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, bool xLog, bool yLog)
  {
    gdlWriteTitleAndSubtitle(e, a);
    gdlAxis(e, a, "X", xStart, xEnd, xLog);
    gdlAxis(e, a, "Y", yStart, yEnd, yLog);
    // title and sub title
    return true;
  }
  static bool gdlAxis3(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log, DLong zAxisCode=0, DDouble NormedLength=0)
  {
    string addCode="b"; //for X and Y, and some Z
    if(zAxisCode==1 || zAxisCode==4) addCode="cm";
    bool doZ=(zAxisCode>=0);

    //
    static GDL_TICKNAMEDATA data;
    static GDL_MULTIAXISTICKDATA muaxdata;

    static GDL_TICKDATA tdata;
    tdata.a=a;
    tdata.isLog=Log;
    tdata.axisrange=abs(End-Start);

    data.a=a;
    data.nTickName=0;
    data.axisrange=abs(End-Start);
    muaxdata.e=e;
    muaxdata.a=a;
    muaxdata.what=GDL_NONE;
    muaxdata.nTickFormat=0;
    muaxdata.nTickUnits=0;
    muaxdata.axismin=Start;
    muaxdata.axismax=End;
    muaxdata.axisrange=abs(End-Start);
    
    //special values
    PLFLT OtherAxisSizeInMm;
    if (axis=="X") OtherAxisSizeInMm=a->mmyPageSize()*(a->boxnYSize());
    if (axis=="Y") OtherAxisSizeInMm=a->mmxPageSize()*(a->boxnXSize());
    if (axis=="Z") OtherAxisSizeInMm=a->mmxPageSize()*(a->boxnXSize()); //not always correct
    //special for AXIS who change the requested box size!
    if (axis=="axisX") {axis="X"; OtherAxisSizeInMm=a->mmyPageSize()*(NormedLength);}
    if (axis=="axisY") {axis="Y"; OtherAxisSizeInMm=a->mmxPageSize()*(NormedLength);}
    if (axis=="axisZ") {axis="Y"; OtherAxisSizeInMm=a->mmxPageSize()*(NormedLength);} //not always correct
    
//    DFloat Charsize;//done in gdlSetAxisCharsize() below
//    gdlGetDesiredAxisCharsize(e, axis, Charsize);
    DLong GridStyle;
    gdlGetDesiredAxisGridStyle(e, axis, GridStyle);
//    DFloat MarginL, MarginR; //unused yet (fixme)
//    gdlGetDesiredAxisMargin(e, axis, MarginL, MarginR);
    DLong Minor;
    gdlGetDesiredAxisMinor(e, axis, Minor);
    DLong Style;
    gdlGetDesiredAxisStyle(e, axis, Style);
    DFloat Thick;
    gdlGetDesiredAxisThick(e, axis, Thick);
    DStringGDL* TickFormat;
    gdlGetDesiredAxisTickFormat(e, axis, TickFormat);
    DDouble TickInterval;
    gdlGetDesiredAxisTickInterval(e, axis, TickInterval);
    DLong TickLayout;
    gdlGetDesiredAxisTickLayout(e, axis, TickLayout);
    DFloat TickLen;
    gdlGetDesiredAxisTickLen(e, axis, TickLen);
    DStringGDL* TickName;
    gdlGetDesiredAxisTickName(e, a, axis, TickName);
    DLong Ticks;
    gdlGetDesiredAxisTicks(e, axis, Ticks);
    DStringGDL* TickUnits;
    gdlGetDesiredAxisTickUnits(e, axis, TickUnits);
//    DDoubleGDL* Tickv;
//    gdlGetDesiredAxisTickv(e, axis, Tickv);
    DString Title;
    gdlGetDesiredAxisTitle(e, axis, Title);

    bool hasTickUnitDefined = (TickUnits->NBytes()>0);
    int tickUnitArraySize=(hasTickUnitDefined)?TickUnits->N_Elements():0;
    //For labels we need ticklen in current character size, for ticks we need it in mm
    DFloat ticklen_in_mm= TickLen;
    if (TickLen<0) ticklen_in_mm*=-1;
    //ticklen in a percentage of box x or y size, to be expressed in mm 
    if (axis=="X") ticklen_in_mm=a->mmyPageSize()*(a->boxnYSize())*ticklen_in_mm;
    if (axis=="Y") ticklen_in_mm=a->mmyPageSize()*(a->boxnXSize())*ticklen_in_mm;
    //eventually, each succesive X or Y axis is separated from previous by interligne + ticklen in adequate units. 
    DFloat interligne;
    if (axis=="X") interligne=a->mmLineSpacing()/a->mmCharHeight(); //in units of character size      
    if (axis=="Y") interligne=a->mmLineSpacing()/a->mmCharLength(); //in units of character size 

    if ( (Style&4)!=4 ) //if we write the axis...
    {
      if (TickInterval==0)
      {
        if (Ticks<=0) TickInterval=gdlComputeTickInterval(e, axis, Start, End, Log);
        else if (Ticks>1) TickInterval=(End-Start)/Ticks;
        else TickInterval=(End-Start);
      }
      //Following hopefully corrects a bug in plplot when TickInterval is very very tiny. The only solution
      // is to avoid plotting the axis!
      if (TickInterval < 10*std::numeric_limits<double>::epsilon()) {
       return 0;
      }
      string Opt;
      //first write labels only:
      gdlSetAxisCharsize(e, a, axis);
      gdlSetPlotCharthick(e, a);
      // axis legend if box style, else do not draw. Take care writing BELOW/ABOVE all axis if tickunits present:actStream->wCharHeight()
      DDouble displacement=(tickUnitArraySize>1)?2.5*tickUnitArraySize:0;

      //no option to care of placement of Z axis???
      if (axis=="X") a->mtex3("xp",3.5+displacement, 0.5, 0.5, Title.c_str());
      else if (axis=="Y") a->mtex3("yp",5.0+displacement,0.5,0.5,Title.c_str());
      else if (doZ) a->mtex3("zp",5.0+displacement,0.5,0.5,Title.c_str());
      
      //axis, 1st time: labels
      Opt=addCode+"nst"; //will write labels beside the left hand axis (u) at major ticks (n)
      if ( Log ) Opt+="l";
      if (TickName->NBytes()>0) // /TICKNAME=[array]
      {
        data.counter=0;
        data.TickName=TickName;
        data.nTickName=TickName->N_Elements();
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlSingleAxisTickNamedFunc, &data );
        Opt+="o";
#endif
        if      (axis=="X") a->box3(Opt.c_str(), "" , TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (doZ) if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      //care Tickunits size is 10 if not defined because it is the size of !X.TICKUNITS.
      else if (hasTickUnitDefined) // /TICKUNITS=[several types of axes written below each other]
      {
        muaxdata.counter=0;
        muaxdata.what=GDL_TICKUNITS;
        if (TickFormat->NBytes()>0)  // with also TICKFORMAT option..
        {
          muaxdata.what=GDL_TICKFORMAT_AND_UNITS;
          muaxdata.TickFormat=TickFormat;
          muaxdata.nTickFormat=TickFormat->N_Elements();
        }
        muaxdata.TickUnits=TickUnits;
        muaxdata.nTickUnits=tickUnitArraySize;
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlMultiAxisTickFunc, &muaxdata );
        Opt+="o";
#endif
        for (SizeT i=0; i< muaxdata.nTickUnits; ++i) //loop on TICKUNITS axis
        {
// no equivalent in 3d yet...
//          PLFLT un,deux,trois,quatre,xun,xdeux,xtrois,xquatre;
//          a->plstream::gvpd(un,deux,trois,quatre);
//          a->plstream::gvpw(xun,xdeux,xtrois,xquatre);
            if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
            else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
            else if (doZ) if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
//          a->plstream::vpor(un,deux,trois,quatre);
//          a->plstream::wind(xun,xdeux,xtrois,xquatre);
            muaxdata.counter++;
        }
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      else if (TickFormat->NBytes()>0) //no /TICKUNITS=> only 1 value taken into account
      {
        muaxdata.counter=0;
        muaxdata.what=GDL_TICKFORMAT;
        muaxdata.TickFormat=TickFormat;
        muaxdata.nTickFormat=1;
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlMultiAxisTickFunc, &muaxdata );
        Opt+="o";
#endif
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (doZ) if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
        
#if (HAVE_PLPLOT_SLABELFUNC)        
        a->slabelfunc( NULL, NULL );
#endif
      }
      else
      {
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlSimpleAxisTickFunc, &tdata );
        Opt+="o";
#endif
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (doZ) if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }

      if (TickLayout==0)
      {
        a->smaj(ticklen_in_mm, 1.0); //set base ticks to default 0.02 viewport converted to mm.
        a->smin(ticklen_in_mm/2.0,1.0); //idem min (plplt defaults)
        //thick for box and ticks.
        a->Thick(Thick);
        
        //ticks or grid eventually with style and length:
        if (abs(TickLen)<1e-6) Opt=""; else Opt="st"; //remove ticks if ticklen=0
        if (TickLen<0) {Opt+="i"; TickLen=-TickLen;}
        
        //gridstyle applies here:
        gdlLineStyle(a,GridStyle);
        if ( Log ) Opt+="l";
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (doZ) if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
        //reset ticks to default plplot value...
        //reset gridstyle
        gdlLineStyle(a,0);
        // pass over with outer box, with thick. No style applied, only ticks
        Opt="b";
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "","",0,0,"","",0,0);
        else if (axis=="Y") a->box3("","",0,0, Opt.c_str(), "", TickInterval, Minor, "","",0,0);
        else if (doZ) if (axis=="Z") a->box3("","",0,0,"","",0,0, Opt.c_str(), "", TickInterval, Minor);
      }
      //reset charsize & thick
      a->Thick(1.0);
      a->sizeChar(1.0);
    }
	return 0;
  }

  static bool gdlBox3(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart,
      DDouble yEnd, DDouble zStart, DDouble zEnd, bool xLog, bool yLog, bool zLog, bool doSpecialZAxisPlacement=0)
  {
    DLong zAxisCode=0;
    static int ZAXISIx=e->KeywordIx("ZAXIS");
    if (doSpecialZAxisPlacement) e->AssureLongScalarKWIfPresent(ZAXISIx, zAxisCode);
    gdlAxis3(e, a, "X", xStart, xEnd, xLog, 0);
    gdlAxis3(e, a, "Y", yStart, yEnd, yLog, 0);
    gdlAxis3(e, a, "Z", zStart, zEnd, zLog, zAxisCode);
    // title and sub title
    gdlWriteTitleAndSubtitle(e, a);
    return true;
  }

} // namespace

#endif
