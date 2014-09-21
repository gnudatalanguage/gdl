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

#include <memory>

#include <string>
#include <fstream>
#include "envt.hpp"
#include "dinterpreter.hpp"
// PLplot is used for direct graphics
#include <plplot/plstream.h>

#include "initsysvar.hpp"
#include "graphicsdevice.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace lib
{

  using namespace std;
//  using std::isinf;
#ifdef _MSC_VER
#define finite _finite
#else
  using std::isnan;
#endif
  
//static values
  static DDouble savedPointX=0.0;
  static DDouble savedPointY=0.0;
  static gdlbox saveBox;
  static DDouble epsDouble=0.0;
  static DDouble absoluteMinVal=0.0;
  static DDouble absoluteMaxVal=0.0;
  static DFloat sym1x[5]={1, -1, 0, 0, 0}; // +
  static DFloat sym1y[5]={0, 0, 0, -1, 1}; // +
  static DFloat sym2x[11]= {1, -1, 0, 0, 0, 0,1,-1,0,1,-1}; //*
  static DFloat sym2y[11]= {0, 0, 0, -1, 1,0,1,-1,0,-1,1}; // *
  static DFloat sym3x[2]={0,0}; // .
  static DFloat sym3y[2]={0,0}; // .
  static DFloat sym4x[5]={ 0, 1, 0, -1, 0 }; //diamond.
  static DFloat sym4y[5]={ 1, 0, -1, 0, 1 }; //diamond.
  static DFloat sym5x[4]={ -1, 0, 1, -1 }; // triangle up.
  static DFloat sym5y[4]={ -1, 1, -1, -1 }; // triangle up.
  static DFloat sym6x[5]={ -1, 1, 1, -1, -1 }; //square
  static DFloat sym6y[5]={ 1, 1, -1, -1, 1 }; //square
  static DFloat sym7x[5]= {1,-1,0,1,-1}; //x
  static DFloat sym7y[5]= {1,-1,0,-1,1}; //x
  DLong syml[7]={5,11,2,5,4,5,5};

  struct GDL_TICKDATA
  {
    bool isLog;
  };

  struct GDL_TICKNAMEDATA
  {
    SizeT counter;
    SizeT nTickName;
    DStringGDL* TickName;
    bool isLog;
  };

  struct GDL_MULTIAXISTICKDATA
  {
    EnvT *e;
    SizeT counter;
    int what;
    SizeT nTickFormat;
    DDouble axismin;
    DDouble axismax;
    DStringGDL* TickFormat;
    SizeT nTickUnits;
    DStringGDL* TickUnits;
    bool isLog;
  };

  // local helper function
  DDouble gdlEpsDouble()
  {
    static bool done=FALSE;
    if (!done)
    {
      long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
      double epsD, epsnegD, xminD, xmaxD;
      machar_d(&ibeta, &it, &irnd, &ngrd, &machep,
                &negep, &iexp, &minexp, &maxexp,
                &epsDouble, &epsnegD, &xminD, &xmaxD );
      done=TRUE;
    }
    return epsDouble;
  }
  DDouble gdlAbsoluteMinValueDouble()
  {
    static bool done=FALSE;
    if (!done)
    {
      long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
      double epsD, epsnegD, xminD, xmaxD;
       machar_d(&ibeta, &it, &irnd, &ngrd, &machep,
       &negep, &iexp, &minexp, &maxexp,
       &epsD, &epsnegD, &absoluteMinVal, &xmaxD );
      done=TRUE;
    }
    return absoluteMinVal;
  }
  DDouble gdlAbsoluteMaxValueDouble()
  {
    static bool done=FALSE;
    if (!done)
    {
      long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
      double epsD, epsnegD, xminD, xmaxD;
       machar_d(&ibeta, &it, &irnd, &ngrd, &machep,
       &negep, &iexp, &minexp, &maxexp,
       &epsD, &epsnegD, &xminD, &absoluteMaxVal );
      done=TRUE;
    }
    return absoluteMaxVal;
  }

  template <typename T> void gdlDoRangeExtrema(T* xVal, T* yVal, DDouble &min, DDouble &max, DDouble xmin, DDouble xmax, bool doMinMax, DDouble minVal, DDouble maxVal)
  {
    DDouble valx, valy;
    SizeT i,k;
    DLong n=xVal->N_Elements();
    if(n!=yVal->N_Elements()) return;
    for (i=0,k=0 ; i<n ; ++i)
    {
       //look only in range x=[xmin,xmax]
       valx=(*xVal)[i];
       if (isnan(valx)) break;
       if(valx<xmin || valx>xmax) break;
       //min and max of y if not NaN and in range [minVal, maxVal] if doMinMax=yes (min_value, max_value keywords)
       valy=(*yVal)[i];
       if (isnan(valy)) break;
       if (doMinMax &&(valy<minVal || valy>maxVal)) break;
       if(k==0) {min=valy; max=valy;} else {min=gdlPlot_Min(min,valy); max=gdlPlot_Max(max,valy);}
       k++;
    }
  }
  template void gdlDoRangeExtrema(Data_<SpDDouble>*, Data_<SpDDouble>*, DDouble &, DDouble &, DDouble, DDouble, bool, DDouble, DDouble);

  void GetMinMaxVal(DDoubleGDL* val, double* minVal, double* maxVal)
  {
    DLong minE, maxE;
    const bool omitNaN=true;
    val->MinMax(&minE, &maxE, NULL, NULL, omitNaN);
    if ( minVal!=NULL ) *minVal=(*val)[ minE];
    if ( maxVal!=NULL ) *maxVal=(*val)[ maxE];
  }

  //call this function if Y data is strictly >0.
  //set yStart to 0 only if gdlYaxisNoZero is false.
  bool gdlYaxisNoZero(EnvT* e)
  {
    //no explict range given?
    DDouble test1, test2;
    static unsigned rangeTag=SysVar::Y()->Desc()->TagIndex("RANGE");
    test1=(*static_cast<DDoubleGDL*>(SysVar::Y()->GetTag(rangeTag, 0)))[0];
    test2=(*static_cast<DDoubleGDL*>(SysVar::Y()->GetTag(rangeTag, 0)))[1];
    if(!(test1==0.0 && test2==0.0)) return TRUE;
    if ( e->KeywordSet( "YRANGE")) return TRUE;
    //Style contains 1?
    DLong ystyle;
    gdlGetDesiredAxisStyle(e, "Y", ystyle);
    if (ystyle&1) return TRUE;

    DLong nozero=0;
    if (ystyle&16) nozero=1;
    if ( e->KeywordSet( "YNOZERO")) nozero = 1;
    return (nozero==1);
  }
  PLFLT AutoTick(DDouble x)
  {
    if ( x==0.0 ) return 1.0;

    DLong n=static_cast<DLong>(floor(log10(x/3.5)));
    DDouble y=(x/(3.5*pow(10., static_cast<double>(n))));
    DLong m;
    if ( y>=1&&y<2 )
      m=1;
    else if ( y>=2&&y<5 )
      m=2;
    else if ( y>=5 )
      m=5;

    PLFLT intv=(PLFLT)(m*pow(10., static_cast<double>(n)));
    return intv;
  }

  PLFLT AutoIntv(DDouble x)
  {
    if ( x==0.0 )
    {
      //      cout << "zero"<<endl;
      return 1.0;
    }

    DLong n=static_cast<DLong>(floor(log10(x/2.82)));
    DDouble y=(x/(2.82*pow(10., static_cast<double>(n))));
    DLong m;
    if ( y>=1&&y<2 )
      m=1;
    else if ( y>=2&&y<4.47 )
      m=2;
    else if ( y>=4.47 )
      m=5;

    //    cout << "AutoIntv" << x << " " << y << endl;

    PLFLT intv=(PLFLT)(m*pow(10., static_cast<double>(n)));
    return intv;
  }

  PLFLT gdlComputeTickInterval(EnvT *e, string axis, DDouble &min, DDouble &max, bool log)
  {
    DLong nticks=0;
    DStructGDL* Struct;

    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();

    if ( Struct!=NULL )
    {
      static unsigned tickTag=Struct->Desc()->TagIndex("TICKS");
      nticks=
      (*static_cast<DLongGDL*>(Struct->GetTag(tickTag, 0)))[0];
    }

    string TitleName=axis+"TICKS";
    e->AssureLongScalarKWIfPresent(TitleName, nticks);

    PLFLT intv;
    if (nticks == 0)
    {
      intv = (log)? AutoTick(log10(max-min)): AutoTick(max-min);
    } else {
      intv = (log)? log10(max-min)/nticks: (max-min)/nticks;
    }
    return intv;
  }

 #define EXTENDED_DEFAULT_LOGRANGE 12
  //protect from (inverted, strange) axis log values
  void gdlHandleUnwantedAxisValue(DDouble &min, DDouble &max, bool log)
  {
    bool invert=FALSE;
    DDouble val_min, val_max;
    if (!log) return;

    if(max-min >= 0)
    {
      val_min=min;
      val_max=max;
      invert=FALSE;
    } else {
      val_min=max;
      val_max=min;
      invert=TRUE;
    }

    if ( val_min<=0. )
    {
      if ( val_max<=0. )
      {
        val_min=-EXTENDED_DEFAULT_LOGRANGE;
        val_max=0.0;
      }
      else
      {
        val_min=log10(val_max)-EXTENDED_DEFAULT_LOGRANGE;
        val_max=log10(val_max);
      }
    }
    else
    {
      val_min=log10(val_min);
      val_max=log10(val_max);
    }
    if (invert)
    {
      min=pow(10.,val_max);
      max=pow(10.,val_min);
    } else {
      min=pow(10.,val_min);
      max=pow(10.,val_max);
    }

  }
#undef EXTENDED_DEFAULT_LOGRANGE


  //improved version of "AutoIntv" for:
  // 1/ better managing ranges when all the data have same value
  // 2/ mimic IDL behavior when data are all positive
  // please notice that (val_min, val_max) will be changed
  // and "epsilon" is a coefficient if "extended range" is expected
  // input: linear min and max, output: linear min and max.

  PLFLT AutoIntvAC(DDouble &start, DDouble &end, bool log)
  {
    gdlHandleUnwantedAxisValue(start, end, log);

    DDouble min, max;
    bool invert=FALSE;

    if(end-start >= 0)
    {
      min=start;
      max=end;
      invert=FALSE;
    } else {
      min=end;
      max=start;
      invert=TRUE;
    }

    PLFLT intv=1.;
    int cas=0;
    DDouble x;
    bool debug=false;
    if ( debug )
    {
      cout<<"init: "<<min<<" "<<max<<endl;
    }
    //retrieve eps Double
    // case "all below ABS((MACHAR()).xmin)
    if ( !log && (abs(min)<=gdlEpsDouble()) && (abs(max)<gdlEpsDouble()) )
    {
      min=DDouble(0.);
      max=DDouble(1.);
      intv=(PLFLT)(2.);
      cas=1;
    }

    if(log)
    {
      min=log10(min);
      max=log10(max);
    }

    // case "all values are equal"
    if ( cas==0 )
    {
      x=max-min;
      if ( abs(x)<1e-30 )
      {
        DDouble val_ref;
        val_ref=max;
        if ( 0.98*min<val_ref )
        { // positive case
          max=1.02*val_ref;
          min=0.98*val_ref;
        }
        else
        { // negative case
          max=0.98*val_ref;
          min=1.02*val_ref;
        }
        if ( debug )
        {
          cout<<"Rescale : "<<min<<" "<<max<<endl;
        }
      }
    }

    // general case (only negative OR negative and positive)
    if ( cas==0 ) //rounding is not aka idl due to use of ceil and floor. TBD.
    {
      x=max-min;
      intv=AutoIntv(x);
      if ( log ) {
        max=ceil((max/intv)*intv);
        min=floor((min/intv)*intv);        
      } else {
        max=ceil(max/intv)*intv;
        min=floor(min/intv)*intv;
      }
    }

    if ( debug )
    {
      cout<<"cas: "<<cas<<" new range: "<<min<<" "<<max<<endl;
    }
    //give back non-log values
    if ( log )
    {
      min=pow(10, min);
      max=pow(10, max);
    }
    if (invert)
    {
      start=max;
      end=min;
    } else {
      start=min;
      end=max;
    }
    return intv;
  }

  void GetUserSymSize(EnvT *e, GDLGStream *a, DDouble& UsymConvX, DDouble& UsymConvY)
  {
    //get symsize
    static DStructGDL* pStruct=SysVar::P();
    DFloat symsize=(*static_cast<DFloatGDL*>
                    (pStruct->GetTag(pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
    e->AssureFloatScalarKWIfPresent("SYMSIZE", symsize);
    if ( symsize<=0.0 ) symsize=1.0;
    
    UsymConvX=(0.5*symsize*(a->wCharLength()/a->charScale())); //be dependent only on symsize!
    UsymConvY=(0.5*symsize*(a->wCharHeight()/a->charScale()));
    PLFLT wun, wdeux, wtrois, wquatre; //take care of axes world orientation!
    a->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
    if ((wdeux-wun)<0) UsymConvX*=-1.0;
    if ((wquatre-wtrois)<0) UsymConvY*=-1.0;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GetUserSymSize(%f,%f)\n",a->wCharLength(),a->wCharHeight());
  }

  void AdjustAxisOpts(string& xOpt, string& yOpt,
                      DLong xStyle, DLong yStyle, DLong xTicks, DLong yTicks,
                      string& xTickformat, string& yTickformat, DLong xLog, DLong yLog
                      )
  {
    if ( (xStyle&8)==8 ) xOpt="b";
    if ( (yStyle&8)==8 ) yOpt="b";

    if ( xTicks==1 ) xOpt+="t";
    else xOpt+="st";
    if ( yTicks==1 ) yOpt+="tv";
    else yOpt+="stv";

    if ( xTickformat!="(A1)" ) xOpt+="n";
    if ( yTickformat!="(A1)" ) yOpt+="n";

    if ( xLog ) xOpt+="l";
    if ( yLog ) yOpt+="l";

    if ( (xStyle&4)==4 ) xOpt="";
    if ( (yStyle&4)==4 ) yOpt="";
  }


  void CheckMargin3d(EnvT* e, GDLGStream* actStream,
                   PLFLT& xMR,
                   PLFLT& xML,
                   PLFLT& yMB,
                   PLFLT& yMT)
  {
    // [XY]MARGIN
    DFloat xMarginL, xMarginR, yMarginB, yMarginT;
    gdlGetDesiredAxisMargin(e, "X", xMarginL, xMarginR);
    gdlGetDesiredAxisMargin(e, "Y", yMarginB, yMarginT);
//    PLFLT scl=actStream->dCharLength()/actStream->xSubPageSize(); //current char length/subpage size
    PLFLT scl=actStream->nCharLength(); //current char length
    xML=xMarginL*scl; //margin as percentage of subpage
    xMR=xMarginR*scl;
    cout<<scl<<","<<xML<<endl;
//    scl=actStream->dCharHeight()/actStream->ySubPageSize(); //current char length/subpage size
    scl=actStream->nCharHeight(); //current char height
    cout<<scl<<endl;
    yMB=(yMarginB)*scl;
    yMT=(yMarginT)*scl;

    if ( xML+xMR>=1.0 )
    {
      Message(e->GetProName()+": XMARGIN to large (adjusted).");
      PLFLT xMMult=xML+xMR;
      xML/=xMMult*1.5;
      xMR/=xMMult*1.5;
    }
    if ( yMB+yMT>=1.0 )
    {
      Message(e->GetProName()+": YMARGIN to large (adjusted).");
      PLFLT yMMult=yMB+yMT;
      yMB/=yMMult*1.5;
      yMT/=yMMult*1.5;
    }
  }


  void CheckMargin(EnvT* e, GDLGStream* actStream,
                   DFloat xMarginL,
                   DFloat xMarginR,
                   DFloat yMarginB,
                   DFloat yMarginT,
                   PLFLT& xMR,
                   PLFLT& xML,
                   PLFLT& yMB,
                   PLFLT& yMT)
  {
    PLFLT scl=actStream->dCharLength()/actStream->xSubPageSize(); //current char length/subpage size
    xML=xMarginL*scl; //margin as percentage of subpage
    xMR=xMarginR*scl;
    scl=actStream->dCharHeight()/actStream->ySubPageSize(); //current char length/subpage size
    yMB=(yMarginB+1.85)*scl;
    yMT=(yMarginT+1.85)*scl; //to allow subscripts and superscripts (as in IDL)

    if ( xML+xMR>=1.0 )
    {
      Message(e->GetProName()+": XMARGIN to large (adjusted).");
      PLFLT xMMult=xML+xMR;
      xML/=xMMult*1.5;
      xMR/=xMMult*1.5;
    }
    if ( yMB+yMT>=1.0 )
    {
      Message(e->GetProName()+": YMARGIN to large (adjusted).");
      PLFLT yMMult=yMB+yMT;
      yMB/=yMMult*1.5;
      yMT/=yMMult*1.5;
    }
  }

  void setIsoPort(GDLGStream* actStream,
                  PLFLT x1,
                  PLFLT x2,
                  PLFLT y1,
                  PLFLT y2,
                  PLFLT aspect)
  {
    PLFLT X1, X2, Y1, Y2, X1s, X2s, Y1s, Y2s, displacx, displacy, scalex, scaley, offsetx, offsety;
    if ( aspect<=0.0 )
    {
      actStream->vpor(x1, x2, y1, y2);
      return;
    }
    // here we need too compensate for the change of aspect due to eventual !P.MULTI plots
    actStream->vpor(x1, x2, y1, y2); //ask for non-iso window
    actStream->gvpd(X1, X2, Y1, Y2); //get viewport values
    //compute relation desiredViewport-page viewport x=scalex*X+offsetx:
    scalex=(x2-x1)/(X2-X1);
    offsetx=(x1*X2-x2*X1)/(X2-X1);
    scaley=(y2-y1)/(Y2-Y1);
    offsety=(y1*Y2-y2*Y1)/(Y2-Y1);
    //ask for wiewport scaled to isotropic by plplot
    actStream->vpas(x1, x2, y1, y2, aspect);
    //retrieve values
    actStream->gvpd(X1s, X2s, Y1s, Y2s);
    //measure displacement
    displacx=X1s-X1;
    displacy=Y1s-Y1;
    //set wiewport scaled by plplot, displaced, as vpor using above linear transformation
    x1=(X1s-displacx)*scalex+offsetx;
    x2=(X2s-displacx)*scalex+offsetx;
    y1=(Y1s-displacy)*scaley+offsety;
    y2=(Y2s-displacy)*scaley+offsety;
    actStream->vpor(x1, x2, y1, y2);
  }

  bool gdlSet3DViewPortAndWorldCoordinates(EnvT* e,
                                           GDLGStream* actStream,
                                           DDoubleGDL* Matrix,
                                           bool xLog, bool yLog,
                                           DDouble xStart,
                                           DDouble xEnd,
                                           DDouble yStart,
                                           DDouble yEnd,
                                           DDouble zStart,
                                           DDouble zEnd,
                                           bool zLog)
  {
    // First, do as gdlSetViewPortAndWorldCoordinates, save some values:
    // set ![XY].CRANGE
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
    PLFLT scl=actStream->nCharLength(); //current char length
    xML=xMarginL*scl; //margin as percentage of subpage
    xMR=xMarginR*scl;
    scl=actStream->nCharHeight(); //current char height
    yMB=(yMarginB)*scl;
    yMT=(yMarginT)*scl;

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

    static bool kwP=FALSE;
    PLFLT positionP[4]={0, 0, 0, 0};
//default box for 3d: evrything minus P.Title /P.subtitle place
    PLFLT position[4]={0,0,1,1};
    // Get !P.position default values
    static unsigned positionTag=SysVar::P()->Desc()->TagIndex("POSITION");
    for ( SizeT i=0; i<4; ++i ) positionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(SysVar::P()->GetTag(positionTag, 0)))[i];
    //check presence of DATA,DEVICE and NORMAL options
    if ( e->KeywordSet( "DATA")) coordinateSystem=DATA;
    if ( e->KeywordSet( "DEVICE")) coordinateSystem=DEVICE;
    if ( e->KeywordSet( "NORMAL")) coordinateSystem=NORMAL;
    if (coordinateSystem==DATA && !actStream->validWorldBox()) e->Throw("PLOT: Data coordinate system not established.");
    // read boxPosition if needed
    int positionIx = e->KeywordIx( "POSITION");
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

    // New plot without POSITION=[] as argument
    if ( boxPosition==(DFloatGDL*)0xF )
    {
      kwP=false;
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
      kwP=true;
      actStream->vpor(position[0], position[2], position[1], position[3]);
    }
    //adjust 'world' values to give room to axis labels. Could be better if we take
    //into account projection angles
    // fix word values without labels:
    actStream->wind(xmin, xmax, ymin, ymax);
    //compute world Charsize
    PLFLT xb, xe, yb, ye;
    xb=xmin-xMarginL*actStream->wCharLength();
    xe=xmax+xMarginR*actStream->wCharLength();
    yb=ymin-yMarginB*actStream->wCharHeight();
    ye=ymax-yMarginT*actStream->wCharHeight();
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
  
  bool gdlSetViewPortAndWorldCoordinates(EnvT* e,
                                         GDLGStream* actStream,
                                         DFloatGDL* boxPosition,
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

    CheckMargin(e, actStream,
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
    DStructGDL* pStruct=SysVar::P();
    // Get !P.position values. !P.REGION is superseded by !P.POSITION
    if ( pStruct!=NULL )
    {
      
      static unsigned regionTag=pStruct->Desc()->TagIndex("REGION");
      for ( SizeT i=0; i<4; ++i ) regionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(pStruct->GetTag(regionTag, 0)))[i];
      static unsigned positionTag=pStruct->Desc()->TagIndex("POSITION");
      for ( SizeT i=0; i<4; ++i ) positionP[i]=(PLFLT)(*static_cast<DFloatGDL*>(pStruct->GetTag(positionTag, 0)))[i];
    }
    if (regionP[0]!=regionP[2] && positionP[0]==positionP[2]) //if not ignored, and will be used, as 
                //a surrogate of !P.Position:
    {
        //compute position removing margins
        positionP[0]=regionP[0]+xMarginL*actStream->nCharLength();
        positionP[1]=regionP[1]+yMarginB*actStream->nCharHeight();
        positionP[2]=regionP[2]-xMarginR*actStream->nCharLength();
        positionP[3]=regionP[3]-yMarginT*actStream->nCharHeight();
    }
    //compatibility: Position NEVER outside [0,1]:
    positionP[0]=max(0.0,positionP[0]);
    positionP[1]=max(0.0,positionP[1]);
    positionP[2]=min(1.0,positionP[2]);
    positionP[3]=min(1.0,positionP[3]);
    
    //check presence of DATA,DEVICE and NORMAL options
    if ( e->KeywordSet( "DATA")) coordinateSystem=DATA;
    if ( e->KeywordSet( "DEVICE")) coordinateSystem=DEVICE;
    if ( e->KeywordSet( "NORMAL")) coordinateSystem=NORMAL;
    if (coordinateSystem==DATA && !actStream->validWorldBox()) e->Throw("PLOT: Data coordinate system not established.");
    // read boxPosition if needed
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


  void GetSFromPlotStructs(DDouble **sx, DDouble **sy, DDouble **sz)
  {
    static DStructGDL* xStruct=SysVar::X();
    static DStructGDL* yStruct=SysVar::Y();
    static DStructGDL* zStruct=SysVar::Z();
    unsigned sxTag=xStruct->Desc()->TagIndex("S");
    unsigned syTag=yStruct->Desc()->TagIndex("S");
    unsigned szTag=zStruct->Desc()->TagIndex("S");
    if (sx != NULL) *sx= &(*static_cast<DDoubleGDL*>(xStruct->GetTag(sxTag, 0)))[0];
    if (sy != NULL) *sy= &(*static_cast<DDoubleGDL*>(yStruct->GetTag(syTag, 0)))[0];
    if (sz != NULL) *sz= &(*static_cast<DDoubleGDL*>(zStruct->GetTag(szTag, 0)))[0];
  }

  void GetWFromPlotStructs(DFloat **wx, DFloat **wy)
  {
    static DStructGDL* xStruct=SysVar::X();
    static DStructGDL* yStruct=SysVar::Y();
    unsigned xwindowTag=xStruct->Desc()->TagIndex("WINDOW");
    unsigned ywindowTag=yStruct->Desc()->TagIndex("WINDOW");
    *wx= &(*static_cast<DFloatGDL*>(xStruct->GetTag(xwindowTag, 0)))[0];
    *wy= &(*static_cast<DFloatGDL*>(yStruct->GetTag(ywindowTag, 0)))[0];
  }

  void GetUsym(DLong **n, DInt **do_fill, DFloat **x, DFloat **y)
  {
    static DStructGDL* usymStruct=SysVar::USYM();
    unsigned nTag=usymStruct->Desc()->TagIndex("DIM");
    unsigned fillTag=usymStruct->Desc()->TagIndex("FILL");
    unsigned xTag=usymStruct->Desc()->TagIndex("X");
    unsigned yTag=usymStruct->Desc()->TagIndex("Y");

    *n= &(*static_cast<DLongGDL*>(usymStruct->GetTag(nTag, 0)))[0];
    *do_fill= &(*static_cast<DIntGDL*>(usymStruct->GetTag(fillTag, 0)))[0];
    *x= &(*static_cast<DFloatGDL*>(usymStruct->GetTag(xTag, 0)))[0];
    *y= &(*static_cast<DFloatGDL*>(usymStruct->GetTag(yTag, 0)))[0];
  }

  void SetUsym(DLong n, DInt do_fill, DFloat *x, DFloat *y)
  {
    static DStructGDL* usymStruct=SysVar::USYM();
    unsigned xTag=usymStruct->Desc()->TagIndex("X");
    unsigned yTag=usymStruct->Desc()->TagIndex("Y");
    unsigned nTag=usymStruct->Desc()->TagIndex("DIM");
    unsigned fillTag=usymStruct->Desc()->TagIndex("FILL");

    (*static_cast<DLongGDL*>(usymStruct->GetTag(nTag, 0)))[0]=n;
    (*static_cast<DIntGDL*>(usymStruct->GetTag(fillTag, 0)))[0]=do_fill;

    for ( int i=0; i<n; i++ )
    {
      (*static_cast<DFloatGDL*>(usymStruct->GetTag(xTag, 0)))[i]=x[i];
      (*static_cast<DFloatGDL*>(usymStruct->GetTag(yTag, 0)))[i]=y[i];
    }
  }

  void DataCoordLimits(DDouble *sx, DDouble *sy, DFloat *wx, DFloat *wy,
                       DDouble *xStart, DDouble *xEnd, DDouble *yStart, DDouble *yEnd, bool clip_by_default)
  {
    *xStart=(wx[0]-sx[0])/sx[1];
    *xEnd=(wx[1]-sx[0])/sx[1];
    *yStart=(wy[0]-sy[0])/sy[1];
    *yEnd=(wy[1]-sy[0])/sy[1];
    //   cout << *xStart <<" "<< *xEnd << " "<< *yStart <<" "<< *yEnd << ""<< endl;

    // patch from Joanna (tracker item no. 3029409, see test_clip.pro)
    if ( !clip_by_default )
    {
      //      cout << "joanna" << endl;
      DFloat wxlen=wx[1]-wx[0];
      DFloat wylen=wy[1]-wy[0];
      DFloat xlen= *xEnd- *xStart;
      DFloat ylen= *yEnd- *yStart;
      *xStart= *xStart-xlen/wxlen*wx[0];
      *xEnd= *xEnd+xlen/wxlen*(1-wx[1]);
      *yStart= *yStart-ylen/wylen*wy[0];
      *yEnd= *yEnd+ylen/wylen*(1-wy[1]);
    }
    //    cout << *xStart <<" "<< *xEnd << " "<< *yStart <<" "<< *yEnd << ""<< endl;
  }

  void ac_histo(GDLGStream *a, int i_buff, PLFLT *x_buff, PLFLT *y_buff, bool xLog)
  {
    PLFLT x, x1, y, y1, val;
    for ( int jj=1; jj<i_buff; ++jj )
    {
      x1=x_buff[jj-1];
      x=x_buff[jj];
      y1=y_buff[jj-1];
      y=y_buff[jj];
      // cf patch 3567803
      if ( xLog )
      {
        //  val=log10((pow(10.0,x1)+pow(10.0,x))/2.0);
        val=x1+log10(0.5+0.5*(pow(10.0, x-x1)));
      }
      else
      {
        val=(x1+x)/2.0;
      }
      a->join(x1, y1, val, y1);
      a->join(val, y1, val, y);
      a->join(val, y, x, y);
    }
  }
  bool startClipping(EnvT *e, GDLGStream *a, bool UsePClip)
  {
    if (GDL_DEBUG_PLSTREAM)  fprintf(stderr,"startClipping\n");
    //function to be called when clipping must be actived, i.e., if the combination of CLIP= and NOCLIP= necessitate it
    //the function retrievs the pertinent information in keywords
    enum
    {
      DATA=0,
      NORMAL,
      DEVICE
    } coordinateSystem=DATA;
    bool xinverted=FALSE;
    bool yinverted=FALSE; //for inverted DATA coordinates
    if ( e->KeywordSet("DATA") ) coordinateSystem=DATA;
    if ( e->KeywordSet("DEVICE") ) coordinateSystem=DEVICE;
    if ( e->KeywordSet("NORMAL") ) coordinateSystem=NORMAL;
    //special treatment for PLOTS, XYOUTS...

    if (UsePClip)
    {
      coordinateSystem=DEVICE;
      if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"USEPCLIP=TRUE!\n");
    }
    int clippingix=e->KeywordIx("CLIP");
    DFloatGDL* clipBox=NULL;
    clipBox=e->IfDefGetKWAs<DFloatGDL>(clippingix);
    if (clipBox!=NULL)
    {
      if (clipBox->N_Elements()<4) return false;
      if ( (*clipBox)[0]==(*clipBox)[3] ||(*clipBox)[1]==(*clipBox)[2] ) return false;
    } else if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NULL CLIPBOX passed\n");
    //Save current box
    a->gvpd(saveBox.nx1, saveBox.nx2, saveBox.ny1, saveBox.ny2); //save norm of current box
    a->gvpw(saveBox.wx1, saveBox.wx2, saveBox.wy1, saveBox.wy2); //save world of current box
    //test axis inversion
    xinverted=(saveBox.wx1>saveBox.wx2);
    yinverted=(saveBox.wy1>saveBox.wy2);
    //GET CLIPPING
    PLFLT dClipBox[4]={0, 0, 0, 0};
    PLFLT tempbox[4]={0, 0, 0, 0};
    DDouble un, deux, trois, quatre;
    bool willNotClip=e->KeywordSet("NOCLIP");

    if (willNotClip)
    {
        dClipBox[2]=a->xPageSize();
        dClipBox[3]=a->yPageSize();
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "using  NOCLIP, i.e. [%f,%f,%f,%f]\n", dClipBox[0], dClipBox[1], dClipBox[2], dClipBox[3]);
    }
    else
    {
      if ( clipBox==NULL || UsePClip ) //get !P.CLIP
      {
        DStructGDL* pStruct=SysVar::P();
        unsigned clipTag=pStruct->Desc()->TagIndex("CLIP"); //is in device coordinates
        for ( int i=0; i<4; ++i ) tempbox[i]=dClipBox[i]=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i];
        if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "using !P.CLIP=[%f,%f,%f,%f]\n", dClipBox[0], dClipBox[1], dClipBox[2], dClipBox[3]);
      }
      else //get units, convert to world coords for plplot, take care of axis direction
      {
        for ( int i=0; i<4&&i<clipBox->N_Elements(); ++i ) tempbox[i]=dClipBox[i]=(*clipBox)[i];
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
    }
    //if new box is in error, return it:
    if (dClipBox[0]>=dClipBox[2]||dClipBox[1]>=dClipBox[3]) return FALSE;
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

  void stopClipping(GDLGStream *a)
  {
    a->vpor(saveBox.nx1, saveBox.nx2, saveBox.ny1, saveBox.ny2); //restore norm of current box
    a->wind(saveBox.wx1, saveBox.wx2, saveBox.wy1, saveBox.wy2); //give back world of current box
  }

  void saveLastPoint(GDLGStream *a, DDouble wx, DDouble wy)
  {
    a->WorldToNormedDevice(wx, wy, savedPointX, savedPointY);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"saveLastPoint as %lf %lf\n", savedPointX, savedPointY);
  }

  void getLastPoint(GDLGStream *a, DDouble& wx, DDouble& wy)
  {
    a->NormedDeviceToWorld(savedPointX, savedPointY, wx, wy);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"getLastPoint: Got dev: %lf %lf giving %lf %lf world\n", savedPointX, savedPointY, wx, wy);
  }


  //CORE PLOT FUNCTION -> Draws a line along xVal, yVal

  template <typename T> bool draw_polyline(EnvT *e, GDLGStream *a,
                                           T * xVal, T* yVal,
                                           DDouble minVal, DDouble maxVal, bool doMinMax,
                                           bool xLog, bool yLog,
                                           DLong psym, bool append, DLongGDL *color)
  {
    bool docolor=(color != NULL);
 
    // Get decomposed value for colors
    DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
    //if docolor, do we really have more than one color?
    if (docolor) if (color->N_Elements() == 1) { //do the job once and forget it after.
      a->Color ( ( *color )[0], decomposed);
      docolor=false;
      
    }    
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"draw_polyline()\n");
    SizeT plotIndex=0;
    bool line=false;
    bool valid=true;
    DLong psym_=0;

    if ( psym<0 )
    {
      line=true;
      psym_= -psym;
    }
    else if ( psym==0 )
    {
      line=true;
      psym_=psym;
    }
    else
    {
      psym_=psym;
    }

    //usersym and other syms as well!
    DFloat *userSymX, *userSymY;
    DLong *userSymArrayDim;
    DInt *do_fill;
    static DInt nofill=0;
    DDouble UsymConvX, UsymConvY;
    GetUserSymSize(e, a, UsymConvX, UsymConvY);
    if ( psym_==8 )
    {
      GetUsym(&userSymArrayDim, &do_fill, &userSymX, &userSymY);
      if ( *userSymArrayDim==0 )
      {
        e->Throw("No user symbol defined.");
      }
    }
    else if ( (psym_>0&&psym_<8))
    {
      do_fill=&nofill;
      userSymArrayDim=&(syml[psym_-1]);
      switch(psym_)
      {
        case 1:
          userSymX=sym1x;
          userSymY=sym1y;
          break;
        case 2:
          userSymX=sym2x;
          userSymY=sym2y;
          break;
        case 3:
          userSymX=sym3x;
          userSymY=sym3y;
          break;
        case 4:
          userSymX=sym4x;
          userSymY=sym4y;
          break;
        case 5:
          userSymX=sym5x;
          userSymY=sym5y;
          break;
        case 6:
          userSymX=sym6x;
          userSymY=sym6y;
          break;
        case 7:
          userSymX=sym7x;
          userSymY=sym7y;
          break;
     }
    }

    DLong minEl=(xVal->N_Elements()<yVal->N_Elements())?
    xVal->N_Elements():yVal->N_Elements();
    // if scalar x
    if ( xVal->N_Elements()==1&&xVal->Rank()==0 )
      minEl=yVal->N_Elements();
    // if scalar y
    if ( yVal->N_Elements()==1&&yVal->Rank()==0 )
      minEl=xVal->N_Elements();
//    bool mapSet=false;
//#ifdef USE_LIBPROJ4
//    // Map Stuff (xtype = 3)
//    LPTYPE idata;
//    XYTYPE odata;
//
//    get_mapset(mapSet);
//
//    DDouble xStart, xEnd;
//    gdlGetCurrentAxisRange("X", xStart, xEnd);
//
//    if ( mapSet )
//    {
//      ref=map_init();
//      if ( ref==NULL )
//      {
//        e->Throw("Projection initialization failed.");
//      }
//    }
//#endif

    // is one of the 2 "arrays" a singleton or not ?

    PLFLT y, yMapBefore, y_ref;
    int flag_y_const=0;
    y_ref=static_cast<PLFLT>((*yVal)[0]);
    if ( yVal->N_Elements()==1&&yVal->Rank()==0 ) flag_y_const=1;

    PLFLT x, x1, xMapBefore, x_ref;
    int flag_x_const=0;
    x_ref=static_cast<PLFLT>((*xVal)[0]);
    if ( xVal->N_Elements()==1&&xVal->Rank()==0 ) flag_x_const=1;

    // AC 070601 we use a buffer to use the fast ->line method
    // instead of the slow ->join one.
    // 2 tricks:
    // trick 1/ size of buffer is limited to 1e4 (compromize syze/speed) in order to be able to manage very
    //    large amount of data whitout duplicating all the arrays
    // trick 2/ when we have a NaN or and Inf, we realize the plot, then reset.

    int GDL_POLYLINE_BUFFSIZE=500000; // idl default seems to be more than 2e6 !!

    if ( minEl<GDL_POLYLINE_BUFFSIZE ) GDL_POLYLINE_BUFFSIZE=append?minEl+1:minEl;
    int i_buff=0;
    PLFLT *x_buff=new PLFLT[GDL_POLYLINE_BUFFSIZE];
    PLFLT *y_buff=new PLFLT[GDL_POLYLINE_BUFFSIZE];

    // flag to reset Buffer when a NaN or a Infinity are founded
    int reset=0;

    bool isBad=FALSE;

    for ( SizeT i=0; i<minEl; ++i ) {
      isBad=FALSE;
      if ( append ) //start with the old point
      {
        getLastPoint(a, x, y);
        i--; //to get good counter afterwards
        append=FALSE; //and stop appending after!
        if ( xLog ) x=pow(10, x);
        if ( yLog ) y=pow(10, y);
      }
      else
      {
        if ( !flag_x_const ) x=static_cast<PLFLT>((*xVal)[i]);
        else x=x_ref;
        if ( !flag_y_const ) y=static_cast<PLFLT>((*yVal)[i]);
        else y=y_ref;
      }
//#ifdef USE_LIBPROJ4
//      if ( mapSet&& !e->KeywordSet("NORMAL") )
//      {
//	    idata.u=x * DEG_TO_RAD;
//        idata.v=y * DEG_TO_RAD;
//        if ( i>0 )
//        {
//          xMapBefore=odata.u;
//          yMapBefore=odata.v;
//        }
//        odata=PJ_FWD(idata, ref);
//        x=odata.u;
//        y=odata.v;
//      }
//#endif
      //note: here y is in minVal maxVal
      if ( doMinMax ) isBad=((y<minVal)||(y>maxVal));
      if ( xLog ) x=log10(x);
      if ( yLog ) y=log10(y);
      isBad=(isBad||!isfinite(x)|| !isfinite(y)||isnan(x)||isnan(y));
      if ( isBad )
      {
        reset=1;
        if ( i_buff>0 )
        {
          if ( line )
          {
            if (docolor) for (SizeT jj=0; jj< i_buff-1 ; ++jj)
            {
              a->Color ( ( *color )[plotIndex%color->N_Elements ( )], decomposed);
              a->line(2, &(x_buff[jj]), &(y_buff[jj]));
              plotIndex++;
            }
            else a->line(i_buff, x_buff, y_buff);
          }
          if (psym_>0&&psym_<9)
          {
            PLFLT *xx=new PLFLT[*userSymArrayDim];
            PLFLT *yy=new PLFLT[*userSymArrayDim];
            for ( int j=0; j<i_buff; ++j )
            {
              for ( int kk=0; kk < *userSymArrayDim; kk++ )
              {
                xx[kk]=x_buff[j]+userSymX[kk]*UsymConvX;
                yy[kk]=y_buff[j]+userSymY[kk]*UsymConvY;
              }
              if (docolor)
              {
                a->Color ( ( *color )[plotIndex%color->N_Elements ( )], decomposed);
                plotIndex++;
              }
              if ( *do_fill==1 )
              {
                a->fill(*userSymArrayDim, xx, yy);
              }
              else
              {
                a->line(*userSymArrayDim, xx, yy);
              }
            }
          }
          if ( psym_==10 )
          {
            ac_histo(a, i_buff, x_buff, y_buff, xLog);
          }
          i_buff=0;
        }
        continue;
      }

//#ifdef USE_LIBPROJ4
//      if ( mapSet&& !e->KeywordSet("NORMAL") ) //IS BROKEN FOR X/YLOG !!!!!!
//      {
//        if ( i>0 ) //;&& (i_buff >0))
//        {
//          x1=xMapBefore;
//          if ( !isfinite(xMapBefore)|| !isfinite(yMapBefore) ) continue;
//
//          // Break "jumps" across maps (kludge!)
//          if ( fabs(x-x1)>0.5*(xEnd-xStart) )
//          {
//            reset=1;
//            if ( (i_buff>0)&&(line) )
//            {
//              a->line(i_buff, x_buff, y_buff);
//              //		  x_buff[0]=x_buff[i_buff-1];
//              //y_buff[0]=y_buff[i_buff-1];
//              i_buff=0;
//            }
//            continue;
//          }
//        }
//      }
//#endif
      x_buff[i_buff]=x;
      y_buff[i_buff]=y;
      i_buff=i_buff+1;

      //	cout << "nbuf: " << i << " " << i_buff << " "<< n_buff_max-1 << " " << minEl-1 << endl;

      if ( (i_buff==GDL_POLYLINE_BUFFSIZE)||((i==minEl-1)&& !append)||((i==minEl)&&append) )
      {
        if ( line )
        {
          if (docolor) for (SizeT jj=0; jj< i_buff-1 ; ++jj)
            {
              a->Color ( ( *color )[plotIndex%color->N_Elements ( )], decomposed);
              a->line(2, &(x_buff[jj]), &(y_buff[jj]));
              plotIndex++;
            }
            else a->line(i_buff, x_buff, y_buff);
        }
        if ( psym_>0&&psym_<9 )
        {
          PLFLT *xx=new PLFLT[*userSymArrayDim];
          PLFLT *yy=new PLFLT[*userSymArrayDim];
          for ( int j=0; j<i_buff; ++j )
          {
            for ( int kk=0; kk < *userSymArrayDim; kk++ )
            {
              xx[kk]=x_buff[j]+userSymX[kk]*UsymConvX;
              yy[kk]=y_buff[j]+userSymY[kk]*UsymConvY;
            }
            if (docolor)
            {
              a->Color ( ( *color )[plotIndex%color->N_Elements ( )], decomposed);
              plotIndex++;
            }
            if ( *do_fill==1 )
            {
              a->fill(*userSymArrayDim, xx, yy);
            }
            else
            {
              a->line(*userSymArrayDim, xx, yy);
            }
          }
        }
        if ( psym_==10 )
        {
          ac_histo(a, i_buff, x_buff, y_buff, xLog);
        }

        // we must recopy the last point since the line must continue (tested via small buffer ...)
        x_buff[0]=x_buff[i_buff-1];
        y_buff[0]=y_buff[i_buff-1];
        i_buff=1;
      }
    }

    delete[] x_buff;
    delete[] y_buff;
    //save last point
    saveLastPoint(a, x, y);
    return (valid);
  }
  // explicit instantiation for SpDDouble
  template bool draw_polyline(EnvT*, GDLGStream*, Data_<SpDDouble>*, Data_<SpDDouble>*, DDouble, DDouble, bool, bool, bool, DLong, bool, DLongGDL*);

 
  //BACKGROUND COLOR

  void gdlSetGraphicsBackgroundColorFromKw(EnvT *e, GDLGStream *a, bool kw)
  {
    static DStructGDL* pStruct=SysVar::P();
    DLong background=
    (*static_cast<DLongGDL*>
     (pStruct->GetTag(pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
    if ( kw )
      e->AssureLongScalarKWIfPresent("BACKGROUND", background);
    DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
    a->Background(background,decomposed);
  }

  //Very special usage only in plotting surface
  void gdlSetGraphicsPenColorToBackground(GDLGStream *a)
  {
    a->plstream::col0( 0);
  }

  //COLOR
  void gdlSetGraphicsForegroundColorFromKw(EnvT *e, GDLGStream *a, string OtherColorKw)
  {
    // Get COLOR from PLOT system variable
    static DStructGDL* pStruct=SysVar::P();
    DLong color=
    (*static_cast<DLongGDL*>
     (pStruct->GetTag(pStruct->Desc()->TagIndex("COLOR"), 0)))[0];

    DLongGDL *colorVect;
    int colorIx;
    //eventually do not get color from standard "COLOR" keyword but from another...
    if (OtherColorKw == "") colorIx=e->KeywordIx ( "COLOR" ); //color may be vector in GDL!
    else colorIx=e->KeywordIx (OtherColorKw);
    if ( e->GetKW ( colorIx )!=NULL )
    {
      colorVect=e->GetKWAs<DLongGDL>( colorIx );
      color=(*colorVect)[0]; //this function only sets color to 1st arg in list!
    }
    // Get decomposed value for colors
    DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
    a->Color(color, decomposed);
  }

  // helper for NOERASE (but also used in XYOUTS)

  void handle_pmulti_position(EnvT *e, GDLGStream *a)
  {
    // all but the first element of !P.MULTI are ignored if POSITION kw or !P.POSITION or !P.REGION is specified
    // TODO: !P.REGION!

    DFloatGDL* pos=NULL;

    // system variable !P.REGION first
    static DStructGDL* pStruct=SysVar::P();
    pos=static_cast<DFloatGDL*>(pStruct-> GetTag(pStruct->Desc()->TagIndex("POSITION"), 0));
    if ( (*pos)[0]==(*pos)[2] ) pos=NULL; //ignored

    // keyword
    if ( pos==NULL )
    {
      DSub* pro=e->GetPro();
      int positionIx=pro->FindKey("POSITION");
      if ( positionIx!= -1 ) pos=e->IfDefGetKWAs<DFloatGDL>(positionIx);
    }

    if ( pos!=NULL ) a->NoSub();
  }

  //NOERASE

  void gdlNextPlotHandlingNoEraseOption(EnvT *e, GDLGStream *a, bool noe)
  {
    bool noErase=FALSE;
    static DStructGDL* pStruct=SysVar::P();

    if ( !noe )
    {
      DLong LnoErase=(*static_cast<DLongGDL*>
                      (pStruct->
                       GetTag(pStruct->Desc()->TagIndex("NOERASE"), 0)))[0];
      noErase=(LnoErase==1);
      if ( e->KeywordSet("NOERASE") )
      {
        noErase=TRUE;
      }
    }
    else
    {
      noErase=TRUE;
    }

    a->NextPlot(!noErase);
    handle_pmulti_position(e, a);
  }

  //PSYM

  void gdlGetPsym(EnvT *e, DLong &psym)
  {
    static DStructGDL* pStruct=SysVar::P();
    psym=(*static_cast<DLongGDL*>
          (pStruct->GetTag(pStruct->Desc()->TagIndex("PSYM"), 0)))[0];

    e->AssureLongScalarKWIfPresent("PSYM", psym);
    if ( psym>10||psym < -8||psym==9 )
      e->Throw(
               "PSYM (plotting symbol) out of range.");
  }

  //SYMSIZE

  void gdlSetSymsize(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct=SysVar::P();
    DFloat symsize=(*static_cast<DFloatGDL*>
                    (pStruct->GetTag(pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
    e->AssureFloatScalarKWIfPresent("SYMSIZE", symsize);
    if ( symsize<=0.0 ) symsize=1.0;
    a->ssym(0.0, symsize);
  }

  //CHARSIZE

  void gdlSetPlotCharsize(EnvT *e, GDLGStream *a, bool accept_sizeKw)
  {
    PLFLT charsize;
    DDouble pmultiscale=1.0;
    // get !P preference
    static DStructGDL* pStruct=SysVar::P();
    charsize=(*static_cast<DFloatGDL*>
              (pStruct->GetTag
               (pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    //overload with command preference. Charsize may be a vector now in some gdl commands, take care of it:
    if (accept_sizeKw) //XYOUTS specials!
    {
      DFloat fcharsize;
      fcharsize=charsize;
      e->AssureFloatScalarKWIfPresent("SIZE", fcharsize);   //conversions are boring...
      charsize=fcharsize;
    }
    int charsizeIx=e->KeywordIx ( "CHARSIZE" );
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

  void gdlSetPlotCharthick(EnvT *e, GDLGStream *a)
  {
    DFloat charthick=1;
     // get !P preference
    static DStructGDL* pStruct=SysVar::P();
    charthick=(*static_cast<DFloatGDL*>
              (pStruct->GetTag
               (pStruct->Desc()->TagIndex("CHARTHICK"), 0)))[0];
    int charthickIx=e->KeywordIx ( "CHARTHICK" ); //Charthick values may be vector in GDL, not in IDL!
    if ( e->GetKW ( charthickIx )!=NULL )
    {
      DFloatGDL* charthickVect=e->GetKWAs<DFloatGDL>( charthickIx );
      charthick=(*charthickVect)[0];
    }
    if ( charthick <= 0.0 ) charthick=1.0;
    a->Thick(charthick);
  }

  void gdlSetAxisCharsize(EnvT *e, GDLGStream *a, string axis)
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


  //THICK

  void gdlSetPenThickness(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct=SysVar::P();
    DFloat thick=(*static_cast<DFloatGDL*>
                  (pStruct->GetTag(pStruct->Desc()->TagIndex("THICK"), 0)))[0];

    e->AssureFloatScalarKWIfPresent("THICK", thick);
    if ( thick <= 0.0 ) thick=1.0;
    a->Thick(thick);
  }
  
  DFloat gdlGetPenThickness(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct=SysVar::P();
    DFloat thick=(*static_cast<DFloatGDL*>
                  (pStruct->GetTag(pStruct->Desc()->TagIndex("THICK"), 0)))[0];

    e->AssureFloatScalarKWIfPresent("THICK", thick);
    if ( thick <= 0.0 ) thick=1.0;
    return thick;
  }
  //LINESTYLE
  void gdlLineStyle(GDLGStream *a, DLong style)
  {
      static PLINT mark1[]={75};
      static PLINT space1[]={1500};
      static PLINT mark2[]={1500};
      static PLINT space2[]={1500};
      static PLINT mark3[]={1500, 100};
      static PLINT space3[]={1000, 1000};
      static PLINT mark4[]={1500, 100, 100, 100};
      static PLINT space4[]={1000, 1000, 1000, 1000};
      static PLINT mark5[]={3000};
      static PLINT space5[]={1500};          // see plplot-5.5.3/examples/c++/x09.cc
      switch(style)
      {
        case 0:
          a->styl(0, mark1, space1);
          return;
         case 1:
          a->styl(1, mark1, space1);
          return;
        case 2:
          a->styl(1, mark2, space2);
          return;
        case 3:
          a->styl(2, mark3, space3);
          return;
        case 4:
          a->styl(4, mark4, space4);
          return;
        case 5:
          a->styl(1, mark5, space5);
          return;
        default:
          a->styl(0, NULL, NULL);
          return;
      }
  }

  void gdlSetLineStyle(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct=SysVar::P();
    DLong linestyle=
    (*static_cast<DLongGDL*>
     (pStruct->GetTag(pStruct->Desc()->TagIndex("LINESTYLE"), 0)))[0];

    // if the LINESTYLE keyword is present, the value will be change
    DLong temp_linestyle=-1111;
    if (e->KeywordSet("LINESTYLE")) e->AssureLongScalarKWIfPresent("LINESTYLE", temp_linestyle);

    bool debug=false;
    if ( debug )
    {
      cout<<"temp_linestyle "<<temp_linestyle<<endl;
      cout<<"     linestyle "<<linestyle<<endl;
    }
    if ( temp_linestyle!= -1111 )
    {
      linestyle=temp_linestyle;
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

  //TITLE

  void gdlWriteTitleAndSubtitle(EnvT* e, GDLGStream *a)
  {
    static unsigned titleTag=SysVar::P()->Desc()->TagIndex("TITLE");
    static unsigned subTitleTag=SysVar::P()->Desc()->TagIndex("SUBTITLE");
    DString title=(*static_cast<DStringGDL*>(SysVar::P()->GetTag(titleTag, 0)))[0];
    DString subTitle=(*static_cast<DStringGDL*>(SysVar::P()->GetTag(subTitleTag, 0)))[0];
    e->AssureStringScalarKWIfPresent("TITLE", title);
    e->AssureStringScalarKWIfPresent("SUBTITLE", subTitle);
    if (title.empty() && subTitle.empty()) return;

    gdlSetPlotCharsize(e, a);
    if (!title.empty())
    {
      e->AssureStringScalarKWIfPresent("TITLE", title);
      gdlSetPlotCharthick(e, a);
      a->sizeChar(1.25*a->charScale());
      a->mtex("t", 1.5, 0.5, 0.5, title.c_str()); //position is in units of current char height. baseline at half-height
      a->sizeChar(a->charScale()/1.25);
    }
    if (!subTitle.empty()) 
    {
      e->AssureStringScalarKWIfPresent("SUBTITLE", subTitle);
      a->mtex("b", 5.4, 0.5, 0.5, subTitle.c_str());
    }
 }
  
  //crange to struct

  void gdlStoreAxisCRANGE(string axis, DDouble Start, DDouble End, bool log)
  {
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      int debug=0;
      if ( debug ) cout<<"Set     :"<<Start<<" "<<End<<endl;

      unsigned crangeTag=Struct->Desc()->TagIndex("CRANGE");
      if ( log )
      {
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[0]=log10(Start);
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[1]=log10(End);
        if ( debug ) cout<<"set log"<<Start<<" "<<End<<endl;
      }
      else
      {
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[0]=Start;
        (*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[1]=End;
      }
    }
  }

  //CRANGE from struct

  void gdlGetCurrentAxisRange(string axis, DDouble &Start, DDouble &End)
  {
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      int debug=0;
      if ( debug ) cout<<"Get     :"<<Start<<" "<<End<<endl;
      bool isProj;
      get_mapset(isProj);
      if (isProj && axis!="Z") {
        static DStructGDL* mapStruct=SysVar::Map();
        static unsigned uvboxTag=mapStruct->Desc()->TagIndex("UV_BOX");
        static DDoubleGDL *uvbox;
        uvbox=static_cast<DDoubleGDL*>(mapStruct->GetTag(uvboxTag, 0));
        if (axis=="X") {
          Start=(*uvbox)[0];
          End=(*uvbox)[2];
        } else {
          Start=(*uvbox)[1];
          End=(*uvbox)[3];
        }
      } else {
      static unsigned crangeTag=Struct->Desc()->TagIndex("CRANGE");
      Start=(*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[0];
      End=(*static_cast<DDoubleGDL*>(Struct->GetTag(crangeTag, 0)))[1];

      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");
      if ( (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]==1 )
      {
        Start=pow(10., Start);
        End=pow(10., End);
        if ( debug ) cout<<"Get log :"<<Start<<" "<<End<<endl;
      }
    }
  }
  }

  //Stores [XYZ].WINDOW, .REGION and .S
  void gdlStoreAxisSandWINDOW(GDLGStream* actStream, string axis, DDouble Start, DDouble End, bool log)
  {
    PLFLT p_xmin, p_xmax, p_ymin, p_ymax, norm_min, norm_max, charDim;
    actStream->gvpd(p_xmin, p_xmax, p_ymin, p_ymax); //viewport normalized coords
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) {Struct=SysVar::X(); norm_min=p_xmin; norm_max=p_xmax; charDim=actStream->nCharLength();}
    if ( axis=="Y" ) {Struct=SysVar::Y(); norm_min=p_ymin; norm_max=p_ymax; charDim=actStream->nCharHeight();}
    if ( axis=="Z" ) {Struct=SysVar::Z(); norm_min=0; norm_max=1; charDim=actStream->nCharLength();}
    if ( Struct!=NULL )
    {
      unsigned marginTag=Struct->Desc()->TagIndex("MARGIN");
      DFloat m1=(*static_cast<DFloatGDL*>(Struct->GetTag(marginTag, 0)))[0];
      DFloat m2=(*static_cast<DFloatGDL*>(Struct->GetTag(marginTag, 0)))[1];
      static unsigned regionTag=Struct->Desc()->TagIndex("REGION");
      (*static_cast<DFloatGDL*>(Struct->GetTag(regionTag, 0)))[0]=max(0.0,norm_min-m1*charDim);
      (*static_cast<DFloatGDL*>(Struct->GetTag(regionTag, 0)))[1]=min(1.0,norm_max+m2*charDim);

      if ( log ) {Start=log10(Start); End=log10(End);}
      static unsigned windowTag=Struct->Desc()->TagIndex("WINDOW");
      (*static_cast<DFloatGDL*>(Struct->GetTag(windowTag, 0)))[0]=norm_min;
      (*static_cast<DFloatGDL*>(Struct->GetTag(windowTag, 0)))[1]=norm_max;

      static unsigned sTag=Struct->Desc()->TagIndex("S");
      (*static_cast<DDoubleGDL*>(Struct->GetTag(sTag, 0)))[0]=
      (norm_min*End-norm_max*Start)/(End-Start);
      (*static_cast<DDoubleGDL*>(Struct->GetTag(sTag, 0)))[1]=
      (norm_max-norm_min)/(End-Start);
    }
  }

  void gdlStoreCLIP(DLongGDL* clipBox)
  {
    static DStructGDL* pStruct=SysVar::P();
    int i;
    static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP");
    for ( i=0; i<clipBox->N_Elements(); ++i ) (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i]=(*clipBox)[i];
  }

  void gdlGetAxisType(string axis, bool &log)
  {
    DStructGDL* Struct;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");
      if ( (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]==1 )
        log=true;
      else
        log=false;
    }
  }

  void get_mapset(bool &mapset)
  {
    DStructGDL* Struct=SysVar::X();
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");

      if ( (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]==3 )
        mapset=true;
      else
        mapset=false;
    }
  }

  void set_mapset(bool mapset)
  {
    DStructGDL* Struct=SysVar::X();
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");
      (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]=(mapset)?3:0;
    }
  }


  //axis type (log..)

  void gdlStoreAxisType(string axis, bool Type)
  {
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned typeTag=Struct->Desc()->TagIndex("TYPE");
      (*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0]=Type;
    } 
  }

  void gdlGetDesiredAxisCharsize(EnvT* e, string axis, DFloat &charsize)
  {
    //default:
    charsize=1.0;
    // get !P preference. Even if xcharsize is absent, presence of charsize or !P.charsize must be taken into account.
    static DStructGDL* pStruct=SysVar::P();
    charsize=(*static_cast<DFloatGDL*>
              (pStruct->GetTag
              (pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    string Charsize_s="CHARSIZE";
    e->AssureFloatScalarKWIfPresent(Charsize_s, charsize); // option charsize overloads P.CHARSIZE
    if (charsize==0) charsize=1.0;
    // Axis Preference
    static DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();

    if ( Struct!=NULL )
    {
      DFloat axisCharsize=0.0;
      static unsigned charsizeTag=Struct->Desc()->TagIndex("CHARSIZE"); //X.CHARSIZE
      axisCharsize=(*static_cast<DFloatGDL*>(Struct->GetTag(charsizeTag, 0)))[0];
      Charsize_s=axis+"CHARSIZE"; //XCHARSIZE
      e->AssureFloatScalarKWIfPresent(Charsize_s, axisCharsize); //option [XYZ]CHARSIZE overloads ![XYZ].CHARSIZE
      if (axisCharsize>0.0) charsize*=axisCharsize; //IDL Behaviour...
    }
  }
  
  void gdlGetDesiredAxisGridStyle(EnvT* e, string axis, DLong &axisGridstyle)
  {
    axisGridstyle=0;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      string gridstyle_s=axis+"GRIDSTYLE";
      e->AssureLongScalarKWIfPresent(gridstyle_s, axisGridstyle);
    }
  }

  //[XYZ]MARGIN kw decoding
  void gdlGetDesiredAxisMargin(EnvT *e, string axis, DFloat &start, DFloat &end)
  {
    DStructGDL* Struct;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();

    if ( Struct!=NULL )
    {
      unsigned marginTag=Struct->Desc()->TagIndex("MARGIN");
      start=
      (*static_cast<DFloatGDL*>(Struct->GetTag(marginTag, 0)))[0];
      end=
      (*static_cast<DFloatGDL*>(Struct->GetTag(marginTag, 0)))[1];
    }

    string MarginName=axis+"MARGIN";
    BaseGDL* Margin=e->GetKW(e->KeywordIx(MarginName));
    if ( Margin!=NULL )
    {
      if ( Margin->N_Elements()>2 )
        e->Throw("Keyword array parameter "+MarginName+
                 " must have from 1 to 2 elements.");
      Guard<DFloatGDL> guard;
      DFloatGDL* MarginF=static_cast<DFloatGDL*>
      (Margin->Convert2(GDL_FLOAT, BaseGDL::COPY));
      guard.Reset(MarginF);
      start=(*MarginF)[0];
      if ( MarginF->N_Elements()>1 )
        end=(*MarginF)[1];
    }
  }

  void gdlGetDesiredAxisMinor(EnvT* e, string axis, DLong &axisMinor)
  {
    axisMinor=0;
    string what_s="MINOR";
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned AxisMinorTag=Struct->Desc()->TagIndex(what_s);
      axisMinor=(*static_cast<DLongGDL*>(Struct->GetTag(AxisMinorTag,0)))[0];
    }
    what_s=axis+"MINOR";
    e->AssureLongScalarKWIfPresent(what_s, axisMinor);
  }

  //GET RANGE. Bool is set if Range has been set, either with [XYZ].RANGE or with [XYZ]RANGE=value

  bool gdlGetDesiredAxisRange(EnvT *e, string axis, DDouble &start, DDouble &end)
  {
    DStructGDL* Struct;
    bool set=FALSE;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      DDouble test1, test2;
      static unsigned rangeTag=Struct->Desc()->TagIndex("RANGE");
      test1=(*static_cast<DDoubleGDL*>(Struct->GetTag(rangeTag, 0)))[0];
      test2=(*static_cast<DDoubleGDL*>(Struct->GetTag(rangeTag, 0)))[1];
      if ( !(test1==0.0&&test2==0.0) )
      {
        start=test1;
        end=test2;
        set=TRUE;
      }
    }
    string RangeName=axis+"RANGE";
    BaseGDL* Range=e->GetKW(e->KeywordIx(RangeName));
    if ( Range!=NULL )
    {
      if ( Range->N_Elements()!=2 )
        e->Throw("Keyword array parameter "+RangeName+
                 " must have 2 elements.");
      Guard<DDoubleGDL> guard;
      DDoubleGDL* RangeF=static_cast<DDoubleGDL*>(Range->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      guard.Reset(RangeF);
      start=(*RangeF)[0];
      end=(*RangeF)[1];
      set=TRUE;
    }
    return set;
  }

  //STYLE

  void gdlGetDesiredAxisStyle(EnvT *e, string axis, DLong &style)
  {
    DStructGDL* Struct;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      int styleTag=Struct->Desc()->TagIndex("STYLE");
      style=
      (*static_cast<DLongGDL*>(Struct->GetTag(styleTag, 0)))[0];
    }

    string style_s=axis+"STYLE";
    e->AssureLongScalarKWIfPresent( style_s, style);
  }

   //XTHICK
  void gdlGetDesiredAxisThick(EnvT *e,  string axis, DFloat &thick)
  {
    thick=1.0;
    DStructGDL* Struct;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();

    if ( Struct!=NULL )
    {
      //not static!
      int thickTag=Struct->Desc()->TagIndex("THICK");
      thick=
      (*static_cast<DFloatGDL*>(Struct->GetTag(thickTag, 0)))[0];
    }
    string thick_s=axis+"THICK";
    e->AssureFloatScalarKWIfPresent(thick_s, thick);
    if ( thick <= 0.0 ) thick=1.0;
  }

  void gdlGetDesiredAxisTickget(EnvT *e,  string axis, DDoubleGDL *Axistickget)
  {
    //TODO!
  }

  void gdlGetDesiredAxisTickFormat(EnvT* e, string axis, DStringGDL* &axisTickformatVect)
  {
    static DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned AxisTickformatTag=Struct->Desc()->TagIndex("TICKFORMAT");
      axisTickformatVect=static_cast<DStringGDL*>(Struct->GetTag(AxisTickformatTag,0));
    }
    string what_s=axis+"TICKFORMAT";
    int axistickformatIx=e->KeywordIx (what_s);
    if (axistickformatIx==-1)
    {
      Warning("[XYZ]TICKFORMAT Keyword unknown (FIXME)");
      return;
    }
    if ( e->GetKW ( axistickformatIx )!=NULL )
    {
      axisTickformatVect=e->GetKWAs<DStringGDL>( axistickformatIx );
    }
  }

   void gdlGetDesiredAxisTickInterval(EnvT* e, string axis, DDouble &axisTickinterval)
  {
    axisTickinterval=0;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      axisTickinterval=(*static_cast<DDoubleGDL*>
                (Struct->GetTag
                (Struct->Desc()->TagIndex("TICKINTERVAL"), 0)))[0];
    }
    string what_s=axis+"TICKINTERVAL";
    e->AssureDoubleScalarKWIfPresent(what_s, axisTickinterval);
  }

  void gdlGetDesiredAxisTickLayout(EnvT* e, string axis, DLong &axisTicklayout)
  {
    axisTicklayout=0;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      axisTicklayout=(*static_cast<DLongGDL*>
                (Struct->GetTag
                (Struct->Desc()->TagIndex("TICKLAYOUT"), 0)))[0];
    }
    string what_s=axis+"TICKLAYOUT";
    e->AssureLongScalarKWIfPresent(what_s, axisTicklayout);
  }

  void gdlGetDesiredAxisTickLen(EnvT* e, string axis, DFloat &ticklen)
  {
    // order: !P.TICKLEN, TICKLEN, !X.TICKLEN, /XTICKLEN
    // get !P preference
    static DStructGDL* pStruct=SysVar::P();
    ticklen=(*static_cast<DFloatGDL*>
            (pStruct->GetTag
            (pStruct->Desc()->TagIndex("TICKLEN"), 0)))[0]; //!P.TICKLEN, always exist, may be 0
    string ticklen_s="TICKLEN";
    e->AssureFloatScalarKWIfPresent(ticklen_s, ticklen); //overwritten by TICKLEN option

    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned ticklenTag=Struct->Desc()->TagIndex("TICKLEN");
      DFloat axisTicklen=0.0;
      axisTicklen=(*static_cast<DFloatGDL*>(Struct->GetTag(ticklenTag, 0)))[0]; //![XYZ].TICKLEN (exist)
      ticklen_s=axis+"TICKLEN";
      e->AssureFloatScalarKWIfPresent(ticklen_s, axisTicklen); //overriden by kw
      if (axisTicklen!=0.0) ticklen=axisTicklen;
    }
  }


  void gdlGetDesiredAxisTickName(EnvT* e, string axis, DStringGDL* &axisTicknameVect)
  {
    static DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned AxisTicknameTag=Struct->Desc()->TagIndex("TICKNAME");
      axisTicknameVect=static_cast<DStringGDL*>(Struct->GetTag(AxisTicknameTag,0));
    }
    string what_s=axis+"TICKNAME";
    int axisticknameIx=e->KeywordIx (what_s);
    if (axisticknameIx==-1)
    {
      Warning("[XYZ]TICKNAME Keyword unknown (FIXME)");
      return;
    }
    if ( e->GetKW ( axisticknameIx )!=NULL )
    {
      axisTicknameVect=e->GetKWAs<DStringGDL>( axisticknameIx );
    }

  }

  void gdlGetDesiredAxisTicks(EnvT* e, string axis, DLong &axisTicks)
  {
    axisTicks=0;
    DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      axisTicks=(*static_cast<DLongGDL*>
                (Struct->GetTag
                (Struct->Desc()->TagIndex("TICKS"), 0)))[0];
    }
    string what_s=axis+"TICKS";
    e->AssureLongScalarKWIfPresent(what_s, axisTicks);
  }

 void gdlGetDesiredAxisTickUnits(EnvT* e, string axis, DStringGDL* &axisTickunitsVect)
  {
    static DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned AxisTickunitsTag=Struct->Desc()->TagIndex("TICKUNITS");
      axisTickunitsVect=static_cast<DStringGDL*>(Struct->GetTag(AxisTickunitsTag,0));
    }
    string what_s=axis+"TICKUNITS";
    int axistickunitsIx=e->KeywordIx (what_s);
    if (axistickunitsIx==-1) 
    {
      Warning("[XYZ]TICKUNITS Keyword unknown (FIXME)");
      return;
    }
    if ( e->GetKW ( axistickunitsIx )!=NULL )
    {
      axisTickunitsVect=e->GetKWAs<DStringGDL>( axistickunitsIx );
    }
  }

  void gdlGetDesiredAxisTickv(EnvT* e, string axis, DDoubleGDL* axisTickvVect)
  {
    static DStructGDL* Struct=NULL;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();
    if ( Struct!=NULL )
    {
      static unsigned AxisTickvTag=Struct->Desc()->TagIndex("TICKV");
      axisTickvVect=static_cast<DDoubleGDL*>(Struct->GetTag(AxisTickvTag,0));

    }
    string what_s=axis+"TICKV";
    int axistickvIx=e->KeywordIx (what_s);
    if (axistickvIx==-1)
    {
      Warning("[XYZ]TICKV Keyword unknown (FIXME)");
      return;
    }
    if ( e->GetKW ( axistickvIx )!=NULL )
    {
      axisTickvVect=e->GetKWAs<DDoubleGDL>( axistickvIx );
    }
  }

  void gdlGetDesiredAxisTitle(EnvT *e, string axis, DString &title)
  {
    DStructGDL* Struct;
    if ( axis=="X" ) Struct=SysVar::X();
    if ( axis=="Y" ) Struct=SysVar::Y();
    if ( axis=="Z" ) Struct=SysVar::Z();

    if ( Struct!=NULL )
    {
      static unsigned titleTag=Struct->Desc()->TagIndex("TITLE");
      title=
      (*static_cast<DStringGDL*>(Struct->GetTag(titleTag, 0)))[0];
    }

    string TitleName=axis+"TITLE";
    e->AssureStringScalarKWIfPresent(TitleName, title);
  }

    void tickformat_date(PLFLT juliandate, string &Month , PLINT &Day , PLINT &Year , PLINT &Hour , PLINT &Minute, PLFLT &Second)
    {
    static string theMonth[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    PLFLT JD,Z,F,a;
    PLINT A,B,C,D,E,month;
    JD = juliandate + 0.5;
    Z = floor(JD);
    F = JD - Z;

    if (Z < 2299161) A = (PLINT)Z;
    else {
      a = (int) ((Z - 1867216.25) / 36524.25);
      A = (PLINT) (Z + 1 + a - (int)(a / 4));
    }

    B = A + 1524;
    C = (PLINT) ((B - 122.1) / 365.25);
    D = (PLINT) (365.25 * C);
    E = (PLINT) ((B - D) / 30.6001);

    // month
    month = E < 14 ? E - 1 : E - 13;
    Month=theMonth[month-1];
    // day
    Day=B - D - (int)(30.6001 * E);
    // year
    Year = month > 2 ? C - 4716 : C - 4715;
    // hours
    Hour = (PLINT) (F * 24);
    F -= (double)Hour / 24;
    // minutes
    Minute = (int) (F * 1440);
    F -= (double)Minute / 1440;
    // seconds
    Second = F * 86400;
  }

  void doOurOwnFormat(PLINT axisNotUsed, PLFLT value, char *label, PLINT length, PLPointer data)
  {
    struct GDL_TICKDATA *ptr = (GDL_TICKDATA* )data;
    static string normalfmt[7]={"%1.0fx10#u%d#d","%2.1fx10#u%d#d","%3.2fx10#u%d#d","%4.2fx10#u%d#d","%5.4fx10#u%d#d","%6.5fx10#u%d#d","%7.6fx10#u%d#d"};
    static string specialfmt="10#u%d#d";
    static string specialfmtlog="10#u%s#d";
    PLFLT z;
    int ns;
    char *i;
    int sgn=(value<0)?-1:1;
    if (sgn*value<gdlEpsDouble()) 
    {
      snprintf(label, length, ((ptr->isLog)?"1":"0")); 
      return;
    }
    int e=floor(log10(value*sgn));
    char *test=(char*)calloc(2*length, sizeof(char)); //be safe
    if (!isfinite(e)||(e<4 && e>-4)) 
    {
      snprintf(test, length, "%f",value);
      ns=strlen(test);
      i=strrchr (test,'0');
      while (i==(test+ns-1)) //remove trailing zeros...
      {
          *i='\0';
        i=strrchr(test,'0');
        ns--;
      }
      i=strrchr(test,'.'); //remove trailing '.'
      if (i==(test+ns-1)) {*i='\0'; ns--;}
      if (ptr->isLog) snprintf( label, length, specialfmtlog.c_str(),test);
      else
      strncpy(label, test, length);
    }
    else
    {
      z=value*sgn/pow(10.,e);
      snprintf(test,20,"%7.6f",z);
      ns=strlen(test);
      i=strrchr(test,'0');
      while (i==(test+ns-1))
      {
          *i='\0';
        i=strrchr(test,'0');
        ns--;
      }
      ns-=2;ns=(ns>6)?6:ns;
      if (floor(sgn*z)==1 && ns==0) snprintf( label, length, specialfmt.c_str(),e); else snprintf( label, length, normalfmt[ns].c_str(),sgn*z,e);
    }
    free(test);
  }

  void gdlMultiAxisTickFunc(PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data)
  {
    static GDL_TICKDATA tdata;
    static SizeT internalIndex=0;
    static DLong lastUnits=0;
    string Month;
    PLINT Day , Year , Hour , Minute;
    PLFLT Second;
    struct GDL_MULTIAXISTICKDATA *ptr = (GDL_MULTIAXISTICKDATA* )data;
    tdata.isLog=ptr->isLog;
    if (ptr->counter != lastUnits)
    {
      lastUnits=ptr->counter;
      internalIndex=0;
    }
    if (ptr->what==GDL_TICKFORMAT || (ptr->what==GDL_TICKFORMAT_AND_UNITS && ptr->counter < ptr->nTickFormat) )
    {
      if (ptr->counter > ptr->nTickFormat-1)
      {
        doOurOwnFormat(axis, value, label, length, &tdata);
//        snprintf( label, length, "%f", value );
      }
      else
      {
        if (((*ptr->TickFormat)[ptr->counter]).substr(0,1) == "(")
        { //internal format, call internal func "STRING"
          EnvT *e=ptr->e;
          static int stringIx = LibFunIx("STRING");
          assert( stringIx >= 0);
          EnvT* newEnv= new EnvT(e, libFunList[stringIx], NULL);
          Guard<EnvT> guard( newEnv);
          // add parameters
          newEnv->SetNextPar( new DDoubleGDL(value));
          newEnv->SetNextPar( new DStringGDL(((*ptr->TickFormat)[ptr->counter]).c_str()));
          // make the call
          BaseGDL* res = static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
          strcpy(label,(*static_cast<DStringGDL*>(res))[0].c_str()); 
        }
        else if (((*ptr->TickFormat)[ptr->counter]).substr(0,10) == "LABEL_DATE")
        { //special internal format, TBD
          Warning("unsupported LABEL_DATE for TICKFORMAT (FIXME)");
        }
        else // external function: if tickunits not specified, pass Axis (int), Index(int),Value(Double)
          //    else pass also Level(int)
          // Thanks to Marc for code snippet!
        {
          EnvT *e=ptr->e;
          DString callF=(*ptr->TickFormat)[ptr->counter];
          // this is a function name -> convert to UPPERCASE
          callF = StrUpCase( callF);
          	//  Search in user proc and function
          SizeT funIx = GDLInterpreter::GetFunIx( callF);

          EnvUDT* newEnv = new EnvUDT( e->CallingNode(), funList[ funIx], (DObjGDL**)NULL);
          Guard< EnvUDT> guard( newEnv);
          // add parameters
          newEnv->SetNextPar( new DLongGDL(axis));
          newEnv->SetNextPar( new DLongGDL(internalIndex));
          newEnv->SetNextPar( new DDoubleGDL(value));
          if (ptr->what==GDL_TICKFORMAT_AND_UNITS) newEnv->SetNextPar( new DLongGDL(ptr->counter));
          // guard *before* pushing new env
          StackGuard<EnvStackT> guard1 ( e->Interpreter()->CallStack());
          e->Interpreter()->CallStack().push_back(newEnv);
          guard.release();

          BaseGDL* retValGDL = e->Interpreter()->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree()); 
          // we are the owner of the returned value
          Guard<BaseGDL> retGuard( retValGDL);
          strcpy(label,(*static_cast<DStringGDL*>(retValGDL))[0].c_str()); 
        }
      }
    }
    else if (ptr->what==GDL_TICKUNITS)
    {
      if (ptr->counter > ptr->nTickUnits-1)
      {
        doOurOwnFormat(axis, value, label, length, &tdata);
//        snprintf( label, length, "%f", value );
      }
      else
      {
        DString what=StrUpCase((*ptr->TickUnits)[ptr->counter]);
        DDouble range=abs(ptr->axismax-ptr->axismin);
        tickformat_date(value, Month , Day , Year , Hour , Minute, Second);
        if (what.substr(0,4)=="YEAR")
          snprintf( label, length, "%d", Year);
        else if (what.substr(0,5)=="MONTH")
          snprintf( label, length, "%s", Month.c_str());
        else if (what.substr(0,3)=="DAY")
          snprintf( label, length, "%d", Day);
        else if (what.substr(0,4)=="HOUR")
          snprintf( label, length, "%d", Hour);
        else if (what.substr(0,6)=="MINUTE")
          snprintf( label, length, "%d", Minute);
        else if (what.substr(0,6)=="SECOND")
          snprintf( label, length, "%f", Second);
        else if (what.substr(0,4)=="TIME")
        {
          if(range>=366) snprintf( label, length, "%d", Year);
          else if(range>=32) snprintf( label, length, "%s", Month.c_str());
          else if(range>=1.1) snprintf( label, length, "%d", Day);
          else if(range*24>=1.1) snprintf( label, length, "%d", Hour);
          else if(range*24*60>=1.1) snprintf( label, length, "%d", Minute);
          else snprintf( label, length, "%04.1f",Second);
        }
        else snprintf( label, length, "%g", value );
      }
    }
    internalIndex++;
  }

  void gdlSingleAxisTickFunc( PLINT axis, PLFLT value, char *label, PLINT length, PLPointer data)
  {
    static GDL_TICKDATA tdata;
    struct GDL_TICKNAMEDATA *ptr = (GDL_TICKNAMEDATA* )data;
    tdata.isLog=ptr->isLog;
    if (ptr->counter > ptr->nTickName-1)
    {
      doOurOwnFormat(axis, value, label, length, &tdata);
    }
    else
    {
      snprintf( label, length, "%s", ((*ptr->TickName)[ptr->counter]).c_str() );
    }
    ptr->counter++;
  }

  bool gdlAxis(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log, DLong modifierCode, DDouble NormedLength)
  {
    static GDL_TICKNAMEDATA data;
    static GDL_MULTIAXISTICKDATA muaxdata;

    static GDL_TICKDATA tdata;
    tdata.isLog=Log;

    data.nTickName=0;
    muaxdata.e=e;
    muaxdata.what=GDL_NONE;
    muaxdata.nTickFormat=0;
    muaxdata.nTickUnits=0;
    muaxdata.axismin=Start;
    muaxdata.axismax=End;

    //special values
    PLFLT OtherAxisSizeInMm;
    if (axis=="X") OtherAxisSizeInMm=a->mmyPageSize()*(a->boxnYSize());
    if (axis=="Y") OtherAxisSizeInMm=a->mmxPageSize()*(a->boxnXSize());
    //special for AXIS who change the requested box size!
    if (axis=="axisX") {axis="X"; OtherAxisSizeInMm=a->mmyPageSize()*(NormedLength);}
    if (axis=="axisY") {axis="Y"; OtherAxisSizeInMm=a->mmxPageSize()*(NormedLength);}
    
    DFloat Charsize;
    gdlGetDesiredAxisCharsize(e, axis, Charsize);
    DLong GridStyle;
    gdlGetDesiredAxisGridStyle(e, axis, GridStyle);
    DFloat MarginL, MarginR;
    gdlGetDesiredAxisMargin(e, axis, MarginL, MarginR);
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
    gdlGetDesiredAxisTickName(e, axis, TickName);
    DLong Ticks;
    gdlGetDesiredAxisTicks(e, axis, Ticks);
    DStringGDL* TickUnits;
    gdlGetDesiredAxisTickUnits(e, axis, TickUnits);
    DDoubleGDL Tickv;
    gdlGetDesiredAxisTickv(e, axis, &Tickv);
    DString Title;
    gdlGetDesiredAxisTitle(e, axis, Title);

    if ( (Style&4)!=4 ) //if we write the axis...
    {
      if (TickInterval==0)
      {
        if (Ticks<=0) TickInterval=gdlComputeTickInterval(e, axis, Start, End, Log);
        else if (Ticks>1) TickInterval=(End-Start)/Ticks;
        else TickInterval=(End-Start);
      }
      string Opt;
      //first write labels only:
      gdlSetAxisCharsize(e, a, axis);
      gdlSetPlotCharthick(e, a);
      // axis legend if box style, else do not draw:
      if (modifierCode==0 ||modifierCode==1)
      {
        if (axis=="X") a->mtex("b", 3.5, 0.5, 0.5, Title.c_str());
        else if (axis=="Y") a->mtex("l",5.0,0.5,0.5,Title.c_str());
      }
      else if (modifierCode==2)
      {
        if (axis=="X") a->mtex("t", 3.5, 0.5, 0.5, Title.c_str());
        else if (axis=="Y") a->mtex("r",5.0,0.5,0.5,Title.c_str());
      }
      //axis, 1st time: labels
      Opt="tvx";// the x option is in plplot 5.9.8 but not before. It permits
                // to avoid writing tick marks here (they will be written after)
                // I hope old plplots were clever enough to ignore 'x'
                // if they did not understand 'x'
      if ( Log ) Opt+="l";
      if (TickName->NBytes()>0) // /TICKNAME=[array]
      {
        data.counter=0;
        data.TickName=TickName;
        data.nTickName=TickName->N_Elements();
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlSingleAxisTickFunc, &data );
        Opt+="o";
#endif
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      else if (TickUnits->NBytes()>0) // /TICKUNITS=[several types of axes written below each other]
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
        muaxdata.nTickUnits=TickUnits->N_Elements();
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlMultiAxisTickFunc, &muaxdata );
        Opt+="o";
#endif
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        for (SizeT i=0; i< muaxdata.nTickUnits; ++i) //loop on TICKUNITS axis
        {
          PLFLT un,deux,trois,quatre,xun,xdeux,xtrois,xquatre;
          a->plstream::gvpd(un,deux,trois,quatre);
          a->plstream::gvpw(xun,xdeux,xtrois,xquatre);
          if (axis=="X") 
          {
            a->smaj(a->mmCharHeight(), 1.0 );
            a->plstream::vpor(un,deux,(PLFLT)(trois-i*3*a->nCharHeight()),quatre);
            a->plstream::wind(xun,xdeux,xtrois,xquatre);
            a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
          }
          else if (axis=="Y") 
          {
            a->smaj(a->mmCharLength(), 1.0 );
            a->plstream::vpor(un-i*3*a->nCharLength(),deux,trois,quatre);
            a->plstream::wind(xun,xdeux,xtrois,xquatre);
            a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
          }
          a->plstream::vpor(un,deux,trois,quatre);
          a->plstream::wind(xun,xdeux,xtrois,xquatre);
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
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
        
#if (HAVE_PLPLOT_SLABELFUNC)        
        a->slabelfunc( NULL, NULL );
#endif
      }
      else
      {
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( doOurOwnFormat, &tdata );
        Opt+="o";
#endif
        if (modifierCode==2) Opt+="m"; else Opt+="n";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0 ,0.0, Opt.c_str(), TickInterval, Minor);
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      
      if (TickLayout==0)
      {
        a->smaj((PLFLT)OtherAxisSizeInMm, 1.0); //set base ticks to default 0.02 viewport converted to mm.
        a->smin((PLFLT)OtherAxisSizeInMm/2.0,1.0); //idem min (plplt defaults)
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
        bool bloatsmall=(TickLen<0.3);
        //gridstyle applies here:
        gdlLineStyle(a,GridStyle);
        a->smaj (0.0, (PLFLT)TickLen); //relative value
        if (bloatsmall) a->smin (0.0, (PLFLT)TickLen); else a->smin( 1.5, 1.0 );
        if ( Log ) Opt+="l";
        if (axis=="X") a->box(Opt.c_str(), TickInterval, Minor, "", 0.0, 0);
        else if (axis=="Y") a->box("", 0.0, 0, Opt.c_str(), TickInterval, Minor);
        //reset ticks to default plplot value...
        a->smaj( 3.0, 1.0 );//back to default values
        a->smin( 1.5, 1.0 );
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

  bool gdlBox(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, bool xLog, bool yLog)
  {
    gdlAxis(e, a, "X", xStart, xEnd, xLog);
    gdlAxis(e, a, "Y", yStart, yEnd, yLog);
    // title and sub title
    gdlWriteTitleAndSubtitle(e, a);
    return true;
  }

 bool gdlAxis3(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log, DLong zAxisCode, DDouble NormedLength)
  {
    //exit if nothing to do...
    string addCode="b"; //for X and Y, and some Z
    if(zAxisCode==1 || zAxisCode==4) addCode="cm";
    if(zAxisCode==-1) return true;

    //
    static GDL_TICKNAMEDATA data;
    static GDL_MULTIAXISTICKDATA muaxdata;
    data.nTickName=0;
    muaxdata.e=e;
    muaxdata.what=GDL_NONE;
    muaxdata.nTickFormat=0;
    muaxdata.nTickUnits=0;
    muaxdata.axismin=Start;
    muaxdata.axismax=End;
    
    //special values
    PLFLT OtherAxisSizeInMm;
    if (axis=="X") OtherAxisSizeInMm=a->mmyPageSize()*(a->boxnYSize());
    if (axis=="Y") OtherAxisSizeInMm=a->mmxPageSize()*(a->boxnXSize());
    if (axis=="Z") OtherAxisSizeInMm=a->mmxPageSize()*(a->boxnXSize()); //not always correct
    //special for AXIS who change the requested box size!
    if (axis=="axisX") {axis="X"; OtherAxisSizeInMm=a->mmyPageSize()*(NormedLength);}
    if (axis=="axisY") {axis="Y"; OtherAxisSizeInMm=a->mmxPageSize()*(NormedLength);}
    if (axis=="axisZ") {axis="Y"; OtherAxisSizeInMm=a->mmxPageSize()*(NormedLength);} //not always correct
    
    DFloat Charsize;
    gdlGetDesiredAxisCharsize(e, axis, Charsize);
    DLong GridStyle;
    gdlGetDesiredAxisGridStyle(e, axis, GridStyle);
    DFloat MarginL, MarginR;
    gdlGetDesiredAxisMargin(e, axis, MarginL, MarginR);
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
    gdlGetDesiredAxisTickName(e, axis, TickName);
    DLong Ticks;
    gdlGetDesiredAxisTicks(e, axis, Ticks);
    DStringGDL* TickUnits;
    gdlGetDesiredAxisTickUnits(e, axis, TickUnits);
    DDoubleGDL* Tickv;
    gdlGetDesiredAxisTickv(e, axis, Tickv);
    DString Title;
    gdlGetDesiredAxisTitle(e, axis, Title);

    if ( (Style&4)!=4 ) //if we write the axis...
    {
      if (TickInterval==0)
      {
        if (Ticks<=0) TickInterval=gdlComputeTickInterval(e, axis, Start, End, Log);
        else if (Ticks>1) TickInterval=(End-Start)/Ticks;
        else TickInterval=(End-Start);
      }
      string Opt;
      //first write labels only:
      gdlSetAxisCharsize(e, a, axis);
      gdlSetPlotCharthick(e, a);
      // axis legend if box style, else do not draw:
      Opt="u";

      if      (axis=="X") a->box3(Opt.c_str(), Title.c_str() , 0.0, 0, "", "", 0.0, 0, "", "", 0.0, 0);
      else if (axis=="Y") a->box3("", "", 0.0 ,0, Opt.c_str(), Title.c_str(), 0.0, 0, "", "", 0.0, 0);
      else if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), Title.c_str(), 0.0, 0);


      //axis, 1st time: labels
      Opt=addCode+"nst"; //will write labels beside the left hand axis (u) at major ticks (n)
      if ( Log ) Opt+="l";
      if (TickName->NBytes()>0) // /TICKNAME=[array]
      {
        data.counter=0;
        data.TickName=TickName;
        data.nTickName=TickName->N_Elements();
#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( gdlSingleAxisTickFunc, &data );
        Opt+="o";
#endif
        if      (axis=="X") a->box3(Opt.c_str(), "" , TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
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
        else if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);

#if (HAVE_PLPLOT_SLABELFUNC)
        a->slabelfunc( NULL, NULL );
#endif
      }
      else
      {
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
      }

      if (TickLayout==0)
      {
        a->smaj((PLFLT)OtherAxisSizeInMm, 1.0); //set base ticks to default 0.02 viewport converted to mm.
        a->smin((PLFLT)OtherAxisSizeInMm/2.0,1.0); //idem min (plplt defaults)
        //thick for box and ticks.
        a->Thick(Thick);
        //ticks or grid eventually with style and length:
        if (abs(TickLen)<1e-6) Opt=""; else Opt="st"; //remove ticks if ticklen=0
        if (TickLen<0) {Opt+="i"; TickLen=-TickLen;}
        bool bloatsmall=(TickLen<0.3);
        //gridstyle applies here:
        gdlLineStyle(a,GridStyle);
        a->smaj (0.0, (PLFLT)TickLen); //relative value
        if (bloatsmall) a->smin (0.0, (PLFLT)TickLen); else a->smin( 1.5, 1.0 );
        if ( Log ) Opt+="l";
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "", "", 0.0, 0, "", "", 0.0, 0);
        else if (axis=="Y") a->box3("", "", 0.0 ,0.0, Opt.c_str(),"", TickInterval, Minor, "", "", 0.0, 0);
        else if (axis=="Z") a->box3("", "", 0.0, 0, "", "", 0.0, 0, Opt.c_str(), "", TickInterval, Minor);
        //reset ticks to default plplot value...
        a->smaj( 3.0, 1.0 );
        a->smin( 1.5, 1.0 );
        //reset gridstyle
        gdlLineStyle(a,0);
        // pass over with outer box, with thick. No style applied, only ticks
        Opt="b";
        if      (axis=="X") a->box3(Opt.c_str(), "", TickInterval, Minor, "","",0,0,"","",0,0);
        else if (axis=="Y") a->box3("","",0,0, Opt.c_str(), "", TickInterval, Minor, "","",0,0);
        else if (axis=="Z") a->box3("","",0,0,"","",0,0, Opt.c_str(), "", TickInterval, Minor);
      }
      //reset charsize & thick
      a->Thick(1.0);
      a->sizeChar(1.0);
    }
	return 0;
  }

  bool gdlBox3(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart,
      DDouble yEnd, DDouble zStart, DDouble zEnd, bool xLog, bool yLog, bool zLog, bool doSpecialZAxisPlacement)
  {
    DLong zAxisCode=0;
    if (doSpecialZAxisPlacement) e->AssureLongScalarKWIfPresent("ZAXIS", zAxisCode);
    gdlAxis3(e, a, "X", xStart, xEnd, xLog, 0);
    gdlAxis3(e, a, "Y", yStart, yEnd, yLog, 0);
    gdlAxis3(e, a, "Z", zStart, zEnd, zLog, zAxisCode);
    // title and sub title
    gdlWriteTitleAndSubtitle(e, a);
    return true;
  }

  bool T3Denabled(EnvT *e)
  {
    static DStructGDL* pStruct=SysVar::P();
    DLong ok4t3d=(*static_cast<DLongGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("T3D"), 0)))[0];
    if (ok4t3d==0) return false; else return true;
  }
  
  void usersym(EnvT *e)
  {
    DFloatGDL *xyVal, *xVal, *yVal;
    Guard<BaseGDL> p0_guard;
    DLong n;
    DInt do_fill;
    DFloat *x, *y;
    SizeT nParam=e->NParam();

    if ( nParam==1 )
    {
      BaseGDL* p0=e->GetNumericArrayParDefined(0)->Transpose(NULL); //hence [1024,2]

      xyVal=static_cast<DFloatGDL*>
      (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      p0_guard.Reset(p0); // delete upon exit

      if ( xyVal->Rank()!=2||xyVal->Dim(1)!=2 )
        e->Throw(e->GetParString(0)+" must be a 2-dim array of type [2,N] in this context.");

      if ( xyVal->Dim(0)>1024 )
      {
        e->Throw("Max array size for USERSYM is 1024");
      }
      n=xyVal->Dim(0);
      // array is in the good order for direct C assignement
      x=&(*xyVal)[0];
      y=&(*xyVal)[n];
    }
    else
    {
      xVal=e->GetParAs< DFloatGDL>(0);
      if ( xVal->Rank()!=1 )
        e->Throw(e->GetParString(0)+" must be a 1D array in this context: ");

      yVal=e->GetParAs< DFloatGDL>(1);
      if ( yVal->Rank()!=1 )
        e->Throw("Expression must be a 1D array in this context: "+e->GetParString(1));

      if ( xVal->Dim(0)!=yVal->Dim(0) )
      {
        e->Throw("Arrays must have same size ");
      }

      if ( xVal->Dim(0)>1024 )
      {
        e->Throw("Max array size for USERSYM is 1024");
      }
      n=xVal->Dim(0);
      x=&(*xVal)[0];
      y=&(*yVal)[0];
    }
    do_fill=0;
    if ( e->KeywordSet("FILL") )
    {
      do_fill=1;
    }
    SetUsym(n, do_fill, x, y);
  }
  
  BaseGDL* trigrid_fun( EnvT* e)
  {
    //   Compute plane parameters A,B,C given 3 points on plane.
    //
    //   z = A + Bx + Cy
    //
    //       (^x21^z10 - ^x10^z21)
    //   C = --------------------- 
    //       (^x21^y10 - ^x10^y21)
    //
    //       (^z10 - C*^y10)
    //   B = ---------------
    //            ^x10
    //
    //   A = z - Bx -Cy
    //
    //   where ^x21 = x2 - x1, etc.

    SizeT nParam=e->NParam();
    if( nParam < 4)
      e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);
    BaseGDL* p1 = e->GetParDefined( 1);
    BaseGDL* p2 = e->GetParDefined( 2);
    BaseGDL* p3 = e->GetParDefined( 3);

    if (p0->N_Elements() != p1->N_Elements() ||
	p0->N_Elements() != p2->N_Elements() ||
	p1->N_Elements() != p2->N_Elements())
      e->Throw( "X, Y, or Z array dimensions are incompatible.");

    if( p3->Rank() == 0)
      e->Throw( "Expression must be an array "
		"in this context: "+ e->GetParString(0));
    if (p3->N_Elements() % 3 != 0)
      e->Throw( "Array of triangles incorrectly dimensioned.");
    DLong n_tri = p3->N_Elements() / 3;

    if( p0->Rank() == 0)
      e->Throw( "Expression must be an array "
		"in this context: "+ e->GetParString(0));
    if( p0->N_Elements() < 3)
      e->Throw( "Value of Bounds is out of allowed range.");

    if( p1->Rank() == 0)
      e->Throw( "Expression must be an array "
		"in this context: "+ e->GetParString(1));

    if( p2->Rank() == 0)
      e->Throw( "Expression must be an array "
		"in this context: "+ e->GetParString(2));

    if (p2->Rank() < 1 || p2->Rank() > 2)
      e->Throw( "Array must have 1 or 2 dimensions: "
		+e->GetParString(0));

    DDoubleGDL* GS=NULL;
    DDoubleGDL* limits=NULL;
    if( nParam > 4) {
      BaseGDL* p4 = e->GetParDefined( 4);
      if( p4->Rank() == 0)
	e->Throw( "Expression must be an array "
		  "in this context: "+ e->GetParString(4));
      if (p4->N_Elements() != 2)
	e->Throw( "Array must have 2 elements: "
		  +e->GetParString(4));
      GS = static_cast<DDoubleGDL*>
	(p4->Convert2( GDL_DOUBLE, BaseGDL::COPY));

      if( nParam == 6) {
	BaseGDL* p5 = e->GetParDefined( 5);
	if( p5->Rank() == 0)
	  e->Throw( "Expression must be an array "
		    "in this context: "+ e->GetParString(4));
	if (p5->N_Elements() != 4)
	  e->Throw( "Array must have 4 elements: "
		    +e->GetParString(5));
	limits = static_cast<DDoubleGDL*>
	  (p5->Convert2( GDL_DOUBLE, BaseGDL::COPY));
      }
    }

    DLong n_segx = 50;
    DLong n_segy = 50;

    DDouble diff[3][2];
    DDouble edge[3][3];

    DDouble diff_pl[3][3];
    DDouble edge_pl[3][3];
    DDouble uv_tri[3][3];
    DDouble uv_gridpt[3];

    DDouble delx10;
    DDouble delx21;
    DDouble dely10;
    DDouble dely21;
    DDouble delz10;
    DDouble delz21;

    DDouble A;
    DDouble B;
    DDouble C;

    bool vertx=false;
    bool verty=false;

    DDouble lon;
    DDouble lat;

    DLong minxEl;
    DLong maxxEl;
    DLong minyEl;
    DLong maxyEl;
    
    // Get NX, NY values if present
    int nxix = e->KeywordIx( "NX");
    if(e->KeywordSet(nxix)) {
      e->AssureLongScalarKW( nxix, n_segx);
    }
    int nyix = e->KeywordIx( "NY");
    if(e->KeywordSet(nyix)) {
      e->AssureLongScalarKW( nyix, n_segy);
    }

    DDoubleGDL* x_tri = static_cast<DDoubleGDL*>
      (p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* y_tri = static_cast<DDoubleGDL*>
      (p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    DDoubleGDL* z = static_cast<DDoubleGDL*>
      (p2->Convert2( GDL_DOUBLE, BaseGDL::COPY));
    DLongGDL* triangles = static_cast<DLongGDL*>
      (p3->Convert2( GDL_LONG, BaseGDL::COPY));


    //    bool sphere=false;
    //int sphereix = e->KeywordIx( "SPHERE");
    //if(e->KeywordSet(sphereix)) sphere = true;

    DDouble xvsx[2];
    DDouble yvsy[2];
    bool map=false;


#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)

    // Stuff needed for MAP keyword processing
    int mapix = e->KeywordIx( "MAP");
    BaseGDL* Map = e->GetKW( mapix);
    if( Map != NULL) {
      //      e->Throw("MAP keyword not yet supported.");
      map = true;
      if(Map != NULL) 
      {
	if(Map->N_Elements() != 4)
	  e->Throw("Keyword array parameter MAP"
		   "must have 4 elements.");
	Guard<DDoubleGDL> guard;
	DDoubleGDL* mapD = static_cast<DDoubleGDL*>
	  ( Map->Convert2( GDL_DOUBLE, BaseGDL::COPY));
	guard.Reset( mapD);
	xvsx[0] = (*mapD)[0];
	xvsx[1] = (*mapD)[1];
	yvsy[0] = (*mapD)[2];
	yvsy[1] = (*mapD)[3];
      }
    }

    LPTYPE idata;
    XYTYPE odata;

    if ( map) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }

      // Convert lon/lat to x/y device coord
      for( SizeT i=0; i<x_tri->N_Elements(); ++i) {
        idata.u = (*x_tri)[i] * DEG_TO_RAD;
        idata.v = (*y_tri)[i] * DEG_TO_RAD;
        odata = PJ_FWD(idata, ref);
        (*x_tri)[i] = odata.u *  xvsx[1] + xvsx[0];
        (*y_tri)[i] = odata.v *  yvsy[1] + yvsy[0];
      }
    }
#endif

    // Determine grid range
    x_tri->MinMax( &minxEl, &maxxEl, NULL, NULL, true);
    y_tri->MinMax( &minyEl, &maxyEl, NULL, NULL, true);

    DDouble x0;
    DDouble y0;
    DDouble x_span = (*x_tri)[maxxEl] - (*x_tri)[minxEl];
    DDouble y_span = (*y_tri)[maxyEl] - (*y_tri)[minyEl];
    if (limits != NULL) {
      x_span = (*limits)[2] - (*limits)[0];
      y_span = (*limits)[3] - (*limits)[1];
      x0 = (*limits)[0];
      y0 = (*limits)[1];
    } else {
      x0 = (*x_tri)[minxEl];
      y0 = (*y_tri)[minyEl];
    }

    // Determine grid spacing
    DDouble x_spacing = x_span / n_segx;
    DDouble y_spacing = y_span / n_segy;
    if (GS != NULL && !e->KeywordSet(nxix)) {
      x_spacing = (*GS)[0];
      n_segx = (DLong) (x_span / x_spacing);
    }
    if (GS != NULL && !e->KeywordSet(nyix)) {
      y_spacing = (*GS)[1];
      n_segy = (DLong) (y_span / y_spacing);
    }
    if (e->KeywordSet(nxix) && n_segx == 1) n_segx = 0;
    if (e->KeywordSet(nyix) && n_segy == 1) n_segy = 0;

    // Setup return array
    DLong dims[2];
    dims[0] = n_segx + 1;
    dims[1] = n_segy + 1;
    dimension dim((DLong *) dims, 2);
    DDoubleGDL* res = new DDoubleGDL( dim, BaseGDL::ZERO);

    bool *found = new bool [(n_segx+1)*(n_segy+1)];
    for( SizeT i=0; i<(n_segx+1)*(n_segy+1); ++i) found[i] = false;

    // *** LOOP THROUGH TRIANGLES *** //

    // Loop through all triangles
    for( SizeT i=0; i<n_tri; ++i) {

      DLong tri0 = (*triangles)[3*i+0];
      DLong tri1 = (*triangles)[3*i+1];
      DLong tri2 = (*triangles)[3*i+2];

      /*
      // Convert lon/lat to x/y device coord
      if ( map) {
	idata.lam = (*x_tri)[tri0] * DEG_TO_RAD;
	idata.phi = (*y_tri)[tri0] * DEG_TO_RAD;
	odata = pj_fwd(idata, ref);
	(*x_tri)[tri0] = odata.x *  xvsx[1] + xvsx[0];
	(*y_tri)[tri0] = odata.y *  yvsy[1] + yvsy[0];

	idata.lam = (*x_tri)[tri1] * DEG_TO_RAD;
	idata.phi = (*y_tri)[tri1] * DEG_TO_RAD;
	odata = pj_fwd(idata, ref);
	(*x_tri)[tri1] = odata.x *  xvsx[1] + xvsx[0];
	(*y_tri)[tri1] = odata.y *  yvsy[1] + yvsy[0];

	idata.lam = (*x_tri)[tri2] * DEG_TO_RAD;
	idata.phi = (*y_tri)[tri2] * DEG_TO_RAD;
	odata = pj_fwd(idata, ref);
	(*x_tri)[tri2] = odata.x *  xvsx[1] + xvsx[0];
	(*y_tri)[tri2] = odata.y *  yvsy[1] + yvsy[0];
      }
      */
      // *** PLANE INTERPOLATION *** //

      delx10 = (*x_tri)[tri1] - (*x_tri)[tri0];
      delx21 = (*x_tri)[tri2] - (*x_tri)[tri1];

      dely10 = (*y_tri)[tri1] - (*y_tri)[tri0];
      dely21 = (*y_tri)[tri2] - (*y_tri)[tri1];

      delz10 = (*z)[tri1] - (*z)[tri0];
      delz21 = (*z)[tri2] - (*z)[tri1];
	
      // Compute grid array
      for( SizeT j=0; j<3; ++j) {
	DLong itri = (*triangles)[3*i+j];
	DLong ktri = (*triangles)[3*i+((j+1) % 3)];
	edge[j][0] = (*x_tri)[ktri] - (*x_tri)[itri];
	edge[j][1] = (*y_tri)[ktri] - (*y_tri)[itri];
      }

      C = (delx21*delz10 - delx10*delz21) /
	(delx21*dely10 - delx10*dely21);
      B = (delz10 - C*dely10) / delx10;
      A = (*z)[tri0] - B*(*x_tri)[tri0] - C*(*y_tri)[tri0];

      
      // *** LOOP THROUGH GRID POINTS *** //

      // Loop through all y-grid values
      for( SizeT iy=0; iy<n_segy+1; ++iy) {
	DDouble devy = y0 + iy * y_spacing;

	// Loop through all x-grid values
	for( SizeT ix=0; ix<n_segx+1; ++ix) {

	  if (found[iy*(n_segx+1)+ix]) continue;

	  bool inside = true;
	  DDouble devx = x0 + ix * x_spacing;

	  // *** PLANE INTERPOLATION *** //

	  // Compute diff array for xy-values
	  for( SizeT j=0; j<3; ++j) {
	    DLong itri = (*triangles)[3*i+j];
	    diff[j][0] = (*x_tri)[itri] - devx;
	    diff[j][1] = (*y_tri)[itri] - devy;
	  }


	  // Determine if inside triangle
	  for( SizeT ivert=0; ivert<3; ++ivert) {
	    DLong kvert = (ivert+1) % 3;

	    DDouble crs1;
	    DDouble crs2;
	    crs1 = diff[ivert][0]*edge[ivert][1]-diff[ivert][1]*edge[ivert][0];
	    crs2 = edge[kvert][0]*edge[ivert][1]-edge[kvert][1]*edge[ivert][0];
	    if (crs1*crs2 > 0) {
	      inside = false;
	      break;
	    }
	  }

	  if (inside == true) {
	    found[iy*(n_segx+1)+ix] = true;
	    (*res)[iy*(n_segx+1)+ix] = A + B*devx + C*devy;
	    if ( map) {
	      //	      cout << setiosflags(ios::fixed);
	      //cout << setw(2);
	      //cout << setprecision(2);
	      //cout << left << "lon: "   << setw(10) << right << lon;
	      //cout << left << "  lat: " << setw(10) << right << lat; 
	      //cout << " in triangle: " << i << endl;
	    }
	  }

	} // ix loop
      } // iy loop
    } // i (triangle) loop

    for( SizeT i=0; i<(n_segx+1)*(n_segy+1); ++i) {
      if (found[i] == false && map) {
	//	cout << i/(n_segy+1) << " ";
	//cout << i - (i/(n_segy+1))*(n_segy+1);
	//cout << " NOT in triangles." << endl;
	//(*res)[i] = 0;
      }
    }

    delete[] found;
    return res;
}

static DDouble bad=sqrt(-1);

#define PROJEPSILON 1E-6
  //plane-vector intersection 
  inline void OnSphereVectorPlaneIntersection(DDouble x1,DDouble y1,DDouble z1,DDouble x2,DDouble y2,
  DDouble z2,DDouble u,DDouble v,DDouble w,DDouble h,
  DDouble &x, DDouble &y, DDouble &z, DDouble incr=0.0){
    DDouble a=x2-x1;
    DDouble b=y2-y1;
    DDouble c=z2-z1;
    DDouble t=u*a+v*b+w*c;
    if (abs(t)<PROJEPSILON) {x=bad; y=bad; z=bad; return;}
    t=(-h-u*x1-v*y1-w*z1)/t;
    x=a*(t+incr)+x1;
    y=b*(t+incr)+y1;
    z=c*(t+incr)+z1;
    DDouble norm=sqrt(x*x+y*y+z*z);
    x/=norm;y/=norm;z/=norm;
    return;
  }
  
DStructGDL *GetMapAsMapStructureKeyword(EnvT *e, bool &externalmap)
{
    // MATRIX keyword (read). Never declare mapIx as static!
    int mapIx = e->KeywordIx( "MAP_STRUCTURE" );
    externalmap = e->KeywordPresent( mapIx );

    DStructGDL* map = NULL;

    if ( externalmap ) {
      map = e->GetKWAs<DStructGDL>(mapIx);
      if ( map->Desc( ) != SysVar::Map( )->Desc( ) ) e->Throw( "Conflicting data structures: MAP_STRUCTURE,!MAP." );
      unsigned projectionTag = map->Desc( )->TagIndex( "PROJECTION" );
      DLong map_projection = (*static_cast<DLongGDL*> (map->GetTag( projectionTag, 0 )))[0];
      if ( map_projection < 1 ) e->Throw( "Map transform not established." );
    } else {
      map = SysVar::Map( );
      unsigned projectionTag = map->Desc( )->TagIndex( "PROJECTION" );
      DLong map_projection = (*static_cast<DLongGDL*> (map->GetTag( projectionTag, 0 )))[0];
      if ( map_projection < 1 ) e->Throw( "Map transform not established." );
    }
    return map;
}

DDoubleGDL* gdlProjForward(PROJTYPE ref, DStructGDL* map, DDoubleGDL *lonsIn, DDoubleGDL *latsIn, DLongGDL *connIn,
bool doConn, DLongGDL *&gonsOut, bool doGons, DLongGDL *&linesOut, bool doLines, bool const doFill ) {

//rule: we do not change input values, we copy them.
  
  //DATA MUST BE IN RADIANS
#ifdef USE_LIBPROJ4
  LPTYPE idata;
  XYTYPE odata;
#endif
  
  unsigned pTag = map->Desc( )->TagIndex( "PIPELINE" );
  DDoubleGDL* pipeline = (static_cast<DDoubleGDL*> (map->GetTag( pTag, 0 )));
  DLong dims[2];

  enum {
    EXIT = 0,
    SPLIT,
    CLIP_PLANE,
    TRANSFORM,
    CLIP_UV
  } action;
  
  dims[0] = pipeline->Dim( 0 );
  dims[1] = pipeline->Dim( 1 );
  int line = 0;
  int icode = (*pipeline)[dims[0] * line + 0];
  DDouble a = (*pipeline)[dims[0] * line + 1]; //plane a,b,c,d
  DDouble b = (*pipeline)[dims[0] * line + 2];
  DDouble c = (*pipeline)[dims[0] * line + 3];
  DDouble d = (*pipeline)[dims[0] * line + 4];
  DDouble px = (*pipeline)[dims[0] * line + 5]; //pole x,y,z
  DDouble py = (*pipeline)[dims[0] * line + 6];
  DDouble pz = (*pipeline)[dims[0] * line + 7];
  DDouble x, y, z, before, after, xs, ys, zs, xe, ye, ze, x1, y1, z1, x2, y2, z2;
  SizeT out,nOut, nEl, iconn, nConn, lastConn;
  OMPInt in;
  DDoubleGDL *lons;
  DDoubleGDL *lats;
  DLongGDL *currentConn,*nextConn,*tmpConn;
  DDoubleGDL *oldLons;
  DDoubleGDL *oldLats;
  //interpolations for GONS on cuts is every 2.5 degrees.
  //Gons takes precedence on Lines

  nEl=lonsIn->N_Elements();
  //if connectivity does not exist, fake a simple one
  if (!doConn) {
    currentConn=new DLongGDL( dimension( nEl+1 ), BaseGDL::INDGEN);
    currentConn->Dec(); 
    (*currentConn)[0]=nEl; //[nEl,0,1...nEl]  very important!
  } else { //just copy
    currentConn=connIn->Dup();
  }

  //copy Input
  lons=lonsIn->Dup();
  lats=latsIn->Dup();
  int pass=0;
  SizeT index;
  SizeT size;
  SizeT start;
  SizeT k,chunksize,ichunk;
  bool isVisible;
  while ( icode > 0 ) {
    switch ( icode ) {
      case SPLIT:
      case CLIP_PLANE:
        pass++;
        //explore conn; for each segment, work inside the segment and determine the needed output sizes.
        index=0;
        nOut=0;
        nConn=0;
        //fprintf( stderr, "\npass%d lon=%d conn=%d, ",pass,lons->N_Elements( ), currentConn->N_Elements());

        while(index >= 0 && index < currentConn->N_Elements() ) {
          size=(*currentConn)[index];
          if (size<0) break;
          start=index+1; //start new chunk...
          nConn++; nConn++;
          k=(*currentConn)[start+0];
          x = cos( (*lons)[k] ) * cos( (*lats)[k] );
          y = sin( (*lons)[k] ) * cos( (*lats)[k] );
          z = sin( (*lats)[k] );
          before = a * x + b * y + c * z + d;
          isVisible = ( before > 0.0  || icode == SPLIT ); //use isVisible only for CLIP_PLANE
          nOut++;
          for ( in = 1; in < size; in++ ) {
            k=(*currentConn)[start+in]; //conn is a list of indexes...
            x = cos( (*lons)[k] ) * cos( (*lats)[k] );
            y = sin( (*lons)[k] ) * cos( (*lats)[k] );
            z = sin( (*lats)[k] );
            after = a * x + b * y + c * z + d;
            if ( before * after < 0.0 ) {
              nOut += 2; //two more points
              if (doGons) nConn += 3; else nConn += 3;  //on more for size---> need to close polygons TODO!!!
            }
            nOut++; 
            nConn++;
            before = after;
          }
          index+=(size+1); 
        }
        //fprintf( stderr, "end: found lon=%d conn=%d...",nOut,nConn);
        //2nd pass, cut if necessary.
        if (true) {//nOut > nEl ) { //switch lons,lats,lines,gons with new ones
          //fprintf( stderr, "(replace)\n",nOut,nConn);
          oldLons = lons;
          oldLats = lats;
          lons = new DDoubleGDL( nOut, BaseGDL::NOZERO );
          lats = new DDoubleGDL( nOut, BaseGDL::NOZERO );
          nextConn = new DLongGDL( nConn, BaseGDL::NOZERO );
          in = 0;
          out = 0;
          iconn = -1;
          index=0;
          ichunk=0;
          while(index >= 0 && index < currentConn->N_Elements() ) {
            size=(*currentConn)[index];
            if (size<0) break;
            start=index+1;
            ichunk++;
            k=(*currentConn)[start+0];

            iconn++;  (*nextConn)[iconn] = -1; lastConn=iconn;  chunksize=0;
            
            //fprintf( stderr, "\nchunk %d size %d starts at %d with %d at %d, running index in lon: %d\n", ichunk, size, start,(*nextConn)[lastConn],lastConn,k);

            xs = cos( (*oldLons)[k] ) * cos( (*oldLats)[k] );
            ys = sin( (*oldLons)[k] ) * cos( (*oldLats)[k] );
            zs = sin( (*oldLats)[k] );
            before = a * xs + b * ys + c * zs + d;

            isVisible = ( before > 0.0  || icode == SPLIT ); //use isVisible only for CLIP_PLANE
            
            if (isVisible) {(*lons)[out] = (*oldLons)[k]; (*lats)[out] = (*oldLats)[k];} 
            else {(*lons)[out]=bad;(*lats)[out] =bad;}
            
            iconn++; (*nextConn)[iconn] = out; out++; chunksize++;
            
            //fprintf( stderr, "[%d", (*nextConn)[iconn]);

            for ( in = 1; in < size; in++) {
              k=(*currentConn)[start+in]; //conn is a list of indexes...
              
              xe = cos( (*oldLons)[k] ) * cos( (*oldLats)[k] );
              ye = sin( (*oldLons)[k] ) * cos( (*oldLats)[k] );
              ze = sin( (*oldLats)[k] );
              after = a * xe + b * ye + c * ze + d;

              if ( before * after < 0.0 ) { //insert two or more points
                //find intersection epsilon before  
                OnSphereVectorPlaneIntersection( xs, ys, zs, xe, ye, ze, a, b, c, d , x, y, z, -0.001 );
                if (isVisible) (*lons)[out] = atan2( y, x ); else (*lons)[out] = bad;
                if (isVisible) (*lats)[out] = asin( z ); else (*lats)[out] = bad;
                iconn++;  (*nextConn)[iconn] = out;   out++; chunksize++;
                
                if (icode == CLIP_PLANE ) isVisible = !isVisible;
                
                //fprintf( stderr, ",%d", (*nextConn)[iconn]);
                //fprintf( stderr, "]; %d->", (*nextConn)[lastConn]);
                (*nextConn)[lastConn] = chunksize; //give back total of points
                //fprintf( stderr, "%d\n", (*nextConn)[lastConn]);
              
                chunksize=0; //reset chunksize
                iconn++; lastConn=iconn;(*nextConn)[iconn]=-1;
                
                //fprintf( stderr, "next at index %d says %d", lastConn, (*nextConn)[lastConn]);
              
                OnSphereVectorPlaneIntersection( xs, ys, zs, xe, ye, ze, a, b, c, d, x, y, z, +0.001 );
                if (isVisible) (*lons)[out] = atan2( y, x ); else (*lons)[out] = bad;
                if (isVisible) (*lats)[out] = asin( z ); else (*lats)[out] = bad;
                iconn++; (*nextConn)[iconn] = out; out++; chunksize++;
              
                //fprintf( stderr, "[%d", (*nextConn)[iconn]);
              }
              
              if (isVisible) {(*lons)[out] = (*oldLons)[k]; (*lats)[out] = (*oldLats)[k];} 
              else {(*lons)[out] = bad; (*lats)[out] = bad;}
              iconn++;  (*nextConn)[iconn] = out; out++; chunksize++;

              //fprintf( stderr, ",%d", (*nextConn)[iconn]);
            
              before = after;
              xs=xe;ys=ye;zs=ze;
            }
            //fprintf( stderr, "]; end chunk; %d->", (*nextConn)[lastConn]);
            (*nextConn)[lastConn] = chunksize; //give back total of points
            //fprintf( stderr, "%d\n -last written conn index: %d of %d", (*nextConn)[lastConn],iconn,nextConn->N_Elements());
            index+=(size+1); 
          }
          //switch connexions...
          tmpConn=currentConn;
          currentConn=nextConn;
          GDLDelete(tmpConn);
        }
        break;
      case TRANSFORM:
#ifdef USE_LIBPROJ4
        nEl = lons->N_Elements( );
#ifdef PROJ_IS_THREADSAFE
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
#endif
        for ( in = 0; in < nEl; in++ ) {
          idata.u = (*lons)[in];
          idata.v = (*lats)[in];
          odata = PJ_FWD( idata, ref );
          (*lons)[in] = odata.u;
          (*lats)[in] = odata.v;
        }
#ifdef PROJ_IS_THREADSAFE
    }
#endif
#endif   //USE_LIBPROJ4 
        break;
      case CLIP_UV:
        nEl = lons->N_Elements( );
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
  #pragma omp for
            for ( in = 0; in < nEl; in++ ) { //use abcd, here  Umin, Vmin, Umax, Vmax
            if (finite((*lons)[in]*(*lats)[in])) if ( (*lons)[in] < a || (*lons)[in] > c || (*lats)[in] < b || (*lats)[in] > d ) {
              (*lons)[in] = sqrt( -1.0 );
              (*lats)[in] = sqrt( -1.0 );
            }
          }
      }
        break;
      default:
        continue;
//        fprintf( stderr, "BUG in map_pipeline, please report!\n" );
//        for ( SizeT i = 1; i < dims[0]; ++i ) fprintf( stderr, "%f, ", (*pipeline)[dims[0] * line + i] );
//        fprintf( stderr, "\n" );
    }
    line++;
    icode = (*pipeline)[dims[0] * line + 0];
    a = (*pipeline)[dims[0] * line + 1]; //plane a,b,c,d
    b = (*pipeline)[dims[0] * line + 2];
    c = (*pipeline)[dims[0] * line + 3];
    d = (*pipeline)[dims[0] * line + 4];
    px = (*pipeline)[dims[0] * line + 5]; //pole x,y,z
    py = (*pipeline)[dims[0] * line + 6];
    pz = (*pipeline)[dims[0] * line + 7];
  }
  
  nEl = lons->N_Elements( ); 
  DLong odims[2];
  odims[0] = 2;
  odims[1] = nEl;
  dimension dim(odims, 2);
  DDoubleGDL *res = new DDoubleGDL(dim, BaseGDL::NOZERO);
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for (OMPInt i = 0; i < nEl ; ++i) {
        (*res)[2 * i] = (*lons)[i];
        (*res)[2 * i + 1] = (*lats)[i];
      }
    }
  //cleanup
  GDLDelete(lons);
  GDLDelete(lats);
  if (doGons || doLines ) {
    if (doGons) gonsOut=currentConn; else linesOut=currentConn;
  } else  GDLDelete(currentConn);
  return res;
}

///
/// Performs all projections and clips defined in a !map.pipeline structure on a vector of lons and lats.
/// eventually with connectivity 'conn'. Depending on doFill, fill or plot the vectors.
/// @param general environnement pointer 
/// @param graphic stream 
/// @param ref pointer on proj.4 opaque projection
/// @param map pointer to a DStructGDL of type !MAP (can be null, the internal !MAP is used)
/// @param lons pointer on DDouble longitude-type values
/// @param lats pointer on DDouble latitude-type values
/// @param isRadians bool false if lon/lat are not in radians (performs conversion to radians)
/// @param doFill const bool if output u,v path has to be filled.
/// @param conn pointer to a DLongGDL connectivity list (can be null)
///
  void GDLgrProjectedPolygonPlot( EnvT* e, GDLGStream * a, PROJTYPE ref, DStructGDL* map,
  DDoubleGDL *lons, DDoubleGDL *lats, bool isRadians, bool const doFill, DLongGDL *conn ) {
    DStructGDL* localMap = map;
    if (localMap==NULL) localMap=SysVar::Map( );
    bool mapSet; 
    get_mapset(mapSet); //if mapSet, output will be converted to normalized coordinates as this seems to be the way to do it.
    bool doConn = (conn != NULL);
    DLongGDL *gons, *lines;
    if (!isRadians) {
    SizeT nin = lons->N_Elements( );
#pragma omp parallel if (nin >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nin))
      {
#pragma omp for
        for ( OMPInt in = 0; in < nin; in++ ) { //pass in radians for gdlProjForward
          (*lons)[in] *= DEG_TO_RAD;
          (*lats)[in] *= DEG_TO_RAD;
        }
      }
    }
    DDoubleGDL *res = gdlProjForward( ref, localMap, lons, lats, conn, doConn, gons, doFill, lines, !doFill, false );
    res = static_cast<DDoubleGDL*> (static_cast<BaseGDL*> (res)->Transpose( NULL ));
    SizeT nout = res->N_Elements( ) / 2;
    int minpoly;
    if ( doFill ) {
      conn = gons;
      minpoly = 3;
    } else {
      conn = lines;
      minpoly = 2;
    }
    SizeT index = 0;
    SizeT size;
    SizeT start;
    while ( index >= 0 && index < conn->N_Elements( ) ) {
      size = (*conn)[index];
      if ( size < 0 ) break;
      start = (*conn)[index + 1];
      if ( size >= minpoly )
        if ( doFill ) {
          a->fill( size, (PLFLT*) &((*res)[start]), (PLFLT*) &((*res)[start + nout]) );
        } else {
          a->line( size, (PLFLT*) &((*res)[start]), (PLFLT*) &((*res)[start + nout]) );
        }
      index += (size + 1);
    }
    GDLDelete( res );
    if ( doFill ) GDLDelete( gons );
    else GDLDelete( lines );
  }
  

} // namespace
