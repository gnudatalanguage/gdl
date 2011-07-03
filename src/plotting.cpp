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

// PLplot is used for direct graphics
#include <plplot/plstream.h>

#include "initsysvar.hpp"
#include "envt.hpp"
#include "graphics.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

namespace lib {

  using namespace std;

  // local helper function
  void GetMinMaxVal( DDoubleGDL* val, double* minVal, double* maxVal)
  {
    DLong minE, maxE;
    const bool omitNaN = true;
    val->MinMax( &minE, &maxE, NULL, NULL, omitNaN);
    if( minVal != NULL) *minVal = (*val)[ minE];
    if( maxVal != NULL) *maxVal = (*val)[ maxE];
  }


  PLFLT AutoTick(DDouble x)
  {
    if( x == 0.0) return 1.0;

    DLong n = static_cast<DLong>( floor(log10(x/3.5)));
    DDouble y = (x / (3.5 * pow(10.,static_cast<double>(n))));
    DLong m;
    if (y >= 1 && y < 2)
      m = 1;
    else if (y >= 2 && y < 5)
      m = 2;
    else if (y >= 5)
      m = 5;

    PLFLT intv = (PLFLT) (m * pow(10.,static_cast<double>(n)));
    return intv;
  }


  PLFLT AutoIntv(DDouble x)
  {
    if( x == 0.0) {
      //      cout << "zero"<<endl;
      return 1.0;
    }

    DLong n = static_cast<DLong>( floor(log10(x/2.82)));
    DDouble y = (x / (2.82 * pow(10.,static_cast<double>(n))));
    DLong m;
    if (y >= 1 && y < 2)
      m = 1;
    else if (y >= 2 && y < 4.47)
      m = 2;
    else if (y >= 4.47)
      m = 5;
    
    //    cout << "AutoIntv" << x << " " << y << endl;

    PLFLT intv = (PLFLT) (m * pow(10.,static_cast<double>(n)));
    return intv;
  }

  //improved version of "AutoIntv" for:
  // 1/ better managing ranges when all the data have same value
  // 2/ mimic IDL behavior when data are all posivite
  // please notice that (val_min, val_max) will be changed
  // and "epsilon" is a coefficient if "extended range" is expected
  PLFLT AutoIntvAC(DDouble &val_min, DDouble &val_max, DLong NoZero, bool log)
  {
    PLFLT intv = 1.;
    int cas = 0 ;
    DDouble x; 
    bool debug = false ;
    if (debug) {cout << "init: " <<  val_min << " " << val_max << endl;}
    
    if (log)
    {
      if (val_min == 0 || val_max == 0) return intv;
      val_min = log10(val_min);
      val_max = log10(val_max);
    }

    // case "all below ABS((MACHAR()).xmin)
    if ((abs(val_min) < 1e-38) && (abs(val_max) < 1e-38)) 
      {
	val_min=DDouble(-1.);
	val_max=DDouble( 1.);
	intv = (PLFLT) (2.);
	cas = 1 ;
      }
    
    // case "all values are equal"
    if (cas == 0)
      {
	x=val_max-val_min;
	if (abs(x) < 1e-30) {
	  DDouble val_ref;
          val_ref=val_max;
          if (0.98*val_min < val_ref) { // positive case
            val_max=1.02*val_ref;
            val_min=0.98*val_ref;
          } else {     // negative case
            val_max=0.98*val_ref;
            val_min=1.02*val_ref;
          }
          if (debug) {cout << "Rescale : " << val_min << " " << val_max << endl;}
	}
      }
    
    // case "all data positive, must start at Zero" (mimic IDL behavior)
    if ((cas == 0) && (val_min >= 0.0) && (NoZero == 0))
      {
	cas = 2 ;
	DDouble resu, val_norm ;
	// we used redundant scale (1.,1.2 and 10., 12. to avoid roundoff problem in log10)
	DDouble levels[12]={1.,1.2,1.5,2.,2.5,3.,4.,5.,6.,8.,10.,12.};
	int nb_levels= 12;
	
	DLong n = static_cast<DLong>( floor(log10(val_max)));
	DDouble scale= pow(10.,static_cast<double>(n));
	
	val_norm=val_max/scale;
     
	resu=levels[0];
	for (int c = 0; c < nb_levels; c++) {
	  if ((val_norm > levels[c]) && (val_norm <= levels[c+1])) resu=levels[c+1] ;
	}
	val_min=0.0;
	val_max=resu*scale;
	intv = (PLFLT)(val_max);
      }
  
    // general case (only negative OR negative and positive)
    if (cas == 0)
      {  
	x=val_max-val_min;	
	intv = AutoIntv( x);
	val_max = ceil(val_max/intv) * intv;
	val_min = floor(val_min/intv) * intv;
      }
  
    if (debug) {cout << "cas: "<< cas << " new range: "<<  val_min << " " << val_max << endl;}

    if (log)
    {
      val_min = pow(10, val_min);
      val_max = pow(10, val_max);
    }
    
    return intv;
  }
  
  // !P
  void GetPData( DLong& p_background,
		 DLong& p_noErase, DLong& p_color, DLong& p_psym,
		 DLong& p_linestyle,
		 DFloat& p_symsize, DFloat& p_charsize, DFloat& p_thick,
		 DString& p_title, DString& p_subTitle, DFloat& p_ticklen)
  {
    static DStructGDL* pStruct = SysVar::P();
    static unsigned backgroundTag = pStruct->Desc()->TagIndex( "BACKGROUND");
    static unsigned noEraseTag = pStruct->Desc()->TagIndex( "NOERASE");
    static unsigned colorTag = pStruct->Desc()->TagIndex( "COLOR");
    static unsigned psymTag = pStruct->Desc()->TagIndex( "PSYM");
    static unsigned linestyleTag = pStruct->Desc()->TagIndex( "LINESTYLE");
    static unsigned symsizeTag = pStruct->Desc()->TagIndex( "SYMSIZE");
    static unsigned charsizeTag = pStruct->Desc()->TagIndex( "CHARSIZE");
    static unsigned thickTag = pStruct->Desc()->TagIndex( "THICK");
    static unsigned ticklenTag = pStruct->Desc()->TagIndex( "TICKLEN");
    static unsigned titleTag = pStruct->Desc()->TagIndex( "TITLE");
    static unsigned subTitleTag = pStruct->Desc()->TagIndex( "SUBTITLE");
    p_background = 
      (*static_cast<DLongGDL*>( pStruct->GetTag( backgroundTag, 0)))[0];
    p_noErase = 
      (*static_cast<DLongGDL*>( pStruct->GetTag( noEraseTag, 0)))[0];
    p_color = 
      (*static_cast<DLongGDL*>( pStruct->GetTag( colorTag, 0)))[0];
    p_psym = 
      (*static_cast<DLongGDL*>( pStruct->GetTag( psymTag, 0)))[0];
    p_linestyle = 
      (*static_cast<DLongGDL*>( pStruct->GetTag( linestyleTag, 0)))[0];
    p_symsize = 
      (*static_cast<DFloatGDL*>( pStruct->GetTag( symsizeTag, 0)))[0];
    p_charsize = 
      (*static_cast<DFloatGDL*>( pStruct->GetTag( charsizeTag, 0)))[0];
    p_thick = 
      (*static_cast<DFloatGDL*>( pStruct->GetTag( thickTag, 0)))[0];
    p_title = 
      (*static_cast<DStringGDL*>( pStruct->GetTag( titleTag, 0)))[0];
    p_subTitle = 
      (*static_cast<DStringGDL*>( pStruct->GetTag( subTitleTag, 0)))[0];
    p_ticklen = 
      (*static_cast<DFloatGDL*>( pStruct->GetTag( ticklenTag, 0)))[0];
  }

  // !X, !Y, !Z
  void GetAxisData( DStructGDL* xStruct,
		    DLong& style, DString& title, DFloat& charSize,
		    DFloat& margin0, DFloat& margin1, DFloat& ticklen)
  {		    
    static unsigned styleTag = xStruct->Desc()->TagIndex( "STYLE");
    static unsigned marginTag = xStruct->Desc()->TagIndex( "MARGIN");
    static unsigned axisTitleTag = xStruct->Desc()->TagIndex( "TITLE");
    static unsigned axischarsizeTag = xStruct->Desc()->TagIndex( "CHARSIZE");
    static unsigned ticklenTag = xStruct->Desc()->TagIndex( "TICKLEN");
    style = 
      (*static_cast<DLongGDL*>( xStruct->GetTag( styleTag, 0)))[0];
    title = 
      (*static_cast<DStringGDL*>( xStruct->GetTag( axisTitleTag, 0)))[0];
    charSize = 
      (*static_cast<DFloatGDL*>( xStruct->GetTag( axischarsizeTag, 0)))[0];
    margin0 = 
      (*static_cast<DFloatGDL*>( xStruct->GetTag( marginTag, 0)))[0];
    margin1 = 
      (*static_cast<DFloatGDL*>( xStruct->GetTag( marginTag, 0)))[1];
    ticklen = 
      (*static_cast<DFloatGDL*>( xStruct->GetTag( ticklenTag, 0)))[0];
  }

  void AdjustAxisOpts(string& xOpt, string& yOpt,
    DLong xStyle, DLong yStyle, DLong xTicks, DLong yTicks,
    string& xTickformat, string& yTickformat, DLong xLog, DLong yLog
  ) // {{{
  {
    if ((xStyle & 8) == 8) xOpt = "b";
    if ((yStyle & 8) == 8) yOpt = "b";

    if (xTicks == 1) xOpt += "t"; else xOpt += "st";
    if (yTicks == 1) yOpt += "tv"; else yOpt += "stv";

    if (xTickformat != "(A1)") xOpt += "n";
    if (yTickformat != "(A1)") yOpt += "n";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";

    if ((xStyle & 4) == 4) xOpt = "";
    if ((yStyle & 4) == 4) yOpt = "";
  } // }}}

  void CheckMargin( EnvT* e, GDLGStream* actStream,
		    DFloat xMarginL, 
		    DFloat xMarginR, 
		    DFloat yMarginB, 
		    DFloat yMarginT,
		    PLFLT& xMR,
		    PLFLT& xML,
		    PLFLT& yMB,
		    PLFLT& yMT)
  {
    // get subpage in mm
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT); 
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;
      
    // get char size in mm (default, actual)
    PLFLT defH, actH;
    actStream->gchr( defH, actH);
      
    xML = xMarginL * actH / scrX;
    xMR = xMarginR * actH / scrX;
    
    // factor 1.111 by ACoulais on 16/12/2010. Consequences on CONVERT_COORD
    const float yCharExtension = 1.5*1.11111;
    yMB = yMarginB * actH / scrY * yCharExtension;
    yMT = yMarginT * actH / scrY * yCharExtension;
    
    if( xML+xMR >= 1.0)
      {
	Message( e->GetProName() + ": XMARGIN to large (adjusted).");
	PLFLT xMMult = xML+xMR;
	xML /= xMMult * 1.5;
	xMR /= xMMult * 1.5;
      }
    if( yMB+yMT >= 1.0)
      {
	Message( e->GetProName() + ": YMARGIN to large (adjusted).");
	PLFLT yMMult = yMB+yMT;
	yMB /= yMMult * 1.5;
	yMT /= yMMult * 1.5;
      }
  }

  void Clipping( DDoubleGDL* clippingD, 
		 DDouble& xStart,
		 DDouble& xEnd,
		 DDouble& minVal,
		 DDouble& maxVal)
  {
    SizeT cEl=clippingD->N_Elements();
    
    // world coordinates
    DDouble wcxs, wcxe,wcys, wcye; 
    
    if(cEl >= 1) wcxs=(*clippingD)[0]; else wcxs=0;
    if(cEl >= 2) wcys=(*clippingD)[1]; else wcys=0;
    if(cEl >= 3) wcxe=(*clippingD)[2]; else wcxe=wcxs;
    if(cEl >= 4) wcye=(*clippingD)[3]; else wcye=wcys;
    
    if(wcxe < wcxs ) wcxe=wcxs; 
    if(wcye < wcys ) wcye=wcys; 
    
    //     // viewport (0..1)
    //     DDouble cxs, cxe,cys, cye;
    //     cxs=(-xStart+wcxs)*(1-0)/(xEnd-xStart);
    //     cxe=(-xStart+wcxe)*(1-0)/(xEnd-xStart);
    //     cys=(-yStart+wcys)*(1-0)/(yEnd-yStart);
    //     cye=(-yStart+wcye)*(1-0)/(yEnd-yStart);
    //     actStream->vpor(cxs, cxe, cys, cye);
    
    xStart=wcxs; xEnd=wcxe; minVal=wcys; maxVal=wcye;
  }


  bool SetVP_WC( EnvT* e, 
		 GDLGStream* actStream,
		 DFloatGDL* pos,
		 DDoubleGDL* clippingD,
		 bool xLog, bool yLog,
		 DFloat xMarginL, 
		 DFloat xMarginR, 
		 DFloat yMarginB, 
		 DFloat yMarginT,
		 // input/output
		 DDouble xStart,
		 DDouble xEnd,
		 DDouble minVal,
		 DDouble maxVal)
  {
    //    cout << "xStart " << xStart << "  xEnd "<<xEnd<<endl;
    //    cout << "yStart " << minVal << "  yEnd "<<maxVal<<endl;


    PLFLT xMR;
    PLFLT xML; 
    PLFLT yMB; 
    PLFLT yMT;

    CheckMargin( e, actStream,
		 xMarginL, 
		 xMarginR, 
		 yMarginB, 
		 yMarginT,
		 xMR, xML, yMB, yMT);

    // viewport - POSITION overrides
    static bool kwP;
    static PLFLT positionP[ 4]={0,0,0,0};
    static PLFLT position[ 4];
    DStructGDL* pStruct = SysVar::P();

    // Get !P.position values
    if(pStruct != NULL) {
      static unsigned positionTag = pStruct->Desc()->TagIndex( "POSITION");
      for( SizeT i=0; i<4; ++i)
	positionP[i] = (PLFLT)
	  (*static_cast<DFloatGDL*>(pStruct->GetTag( positionTag, 0)))[i];
    }

    // If pos == NULL (oplot)

    if ( pos == NULL) {

      // If position keyword previously set
      if( kwP) {
	actStream->vpor(position[0],position[2],position[1],position[3]);
      } else {
	// If !P.position not set
	if (positionP[0] == 0 && positionP[1] == 0 &&
	    positionP[2] == 0 && positionP[3] == 0)
	  actStream->vpor(position[0],position[2],position[1],position[3]);
	else {
	  // !P.position set
	  actStream->vpor(positionP[0],positionP[2],positionP[1],positionP[3]);
	}
      }
      // New plot
    } else if ( pos == (DFloatGDL*) 0xF) {
      kwP = false;

      // If !P.position not set use default values
      if (positionP[0] == 0 && positionP[1] == 0 &&
	  positionP[2] == 0 && positionP[3] == 0) {

	// Set to default values
	position[0] = xML;
	position[1] = yMB;
	position[2] = 1.0 - xMR;
	position[3] = 1.0 - yMT;
	actStream->vpor(position[0],position[2],position[1],position[3]);
      } else {
	// !P.position values
	actStream->vpor(positionP[0],positionP[2],positionP[1],positionP[3]);
      }
      // Position keyword set
    } else {
      kwP = true;
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];
      actStream->vpor(position[0],position[2],position[1],position[3]);
    }


    // CLIPPING
    if( clippingD != NULL)
	Clipping( clippingD, xStart, xEnd, minVal, maxVal);

    // for OPLOT start and end values are already log
    // SA: changing only local variables!
    if( pos != NULL)
      {
	if( xLog)
	  {	  
	    if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
	    if( xEnd   <= 0.0) return false; else xEnd = log10( xEnd);
	  }
	if( yLog)
	  {
	    if( minVal <= 0.0) minVal = 0.0; else minVal = log10( minVal);
	    if( maxVal <= 0.0) return false; else maxVal = log10( maxVal);
	  }
      }
	  
    // set world coordinates
    // cout << "VP wind: "<<xStart<<" "<<xEnd<<" "<<minVal<<" "<<maxVal<<endl;
    //    printf("data lim (setv): %f %f %f %f\n", xStart, xEnd, minVal, maxVal);
    actStream->wind( xStart, xEnd, minVal, maxVal);
    //    cout << "xStart " << xStart << "  xEnd "<<xEnd<<endl;
    //    cout << "yStart " << minVal << "  yEnd "<<maxVal<<endl;

    return true;
  }
  
  void UpdateSWPlotStructs(GDLGStream* actStream, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd)
  {
    // Get viewpoint parameters and store in WINDOW & S
    PLFLT p_xmin, p_xmax, p_ymin, p_ymax;
    actStream->gvpd (p_xmin, p_xmax, p_ymin, p_ymax);

    DStructGDL* Struct = NULL;

    Struct = SysVar::X();
    static unsigned windowTag = Struct->Desc()->TagIndex( "WINDOW");
    static unsigned sTag = Struct->Desc()->TagIndex( "S");
    if (Struct != NULL) 
    {   
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[0] = p_xmin;
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[1] = p_xmax;

      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] = 
        (p_xmin*xEnd - p_xmax*xStart) / (xEnd - xStart);
      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] = 
        (p_xmax - p_xmin) / (xEnd - xStart);
    }   

    Struct = SysVar::Y();
    if(Struct != NULL) 
    {   
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[0] = p_ymin;
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[1] = p_ymax;

      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] = 
        (p_ymin*yEnd - p_ymax*yStart) / (yEnd - yStart);
      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] = 
        (p_ymax - p_ymin) / (yEnd - yStart);
    }
  }

  void GetSFromPlotStructs(DDouble **sx, DDouble **sy)
  {
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    unsigned syTag = yStruct->Desc()->TagIndex( "S");
    *sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    *sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];
  }
    
  void GetWFromPlotStructs(DFloat **wx, DFloat **wy)
  {
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    unsigned xwindowTag = xStruct->Desc()->TagIndex( "WINDOW");
    unsigned ywindowTag = yStruct->Desc()->TagIndex( "WINDOW");
    *wx = &(*static_cast<DFloatGDL*>( xStruct->GetTag( xwindowTag, 0)))[0];
    *wy = &(*static_cast<DFloatGDL*>( yStruct->GetTag( ywindowTag, 0)))[0];
  }
    
  void DataCoordLimits(DDouble *sx, DDouble *sy, DFloat *wx, DFloat *wy, 
    DDouble *xStart, DDouble *xEnd, DDouble *yStart, DDouble *yEnd, bool clip_by_default)
  {
    *xStart = (wx[0] - sx[0]) / sx[1];
    *xEnd   = (wx[1] - sx[0]) / sx[1];
    *yStart = (wy[0] - sy[0]) / sy[1];
    *yEnd   = (wy[1] - sy[0]) / sy[1];
    //    cout << *xStart <<" "<< *xEnd << " "<< *yStart <<" "<< *yEnd << ""<< endl;

    // patch from Joanna (tracker item no. 3029409, see test_clip.pro)
    if (!clip_by_default) {
      //      cout << "joanna" << endl;
      DFloat wxlen = wx[1] - wx[0];
      DFloat wylen = wy[1] - wy[0];
      DFloat xlen = *xEnd - *xStart;
      DFloat ylen = *yEnd - *yStart;
      *xStart = *xStart - xlen/wxlen * wx[0];
      *xEnd = *xEnd + xlen/wxlen * (1 - wx[1]);
      *yStart = *yStart - ylen/wylen * wy[0];
      *yEnd = *yEnd + ylen/wylen * (1 - wy[1]);
    }  
    //    cout << *xStart <<" "<< *xEnd << " "<< *yStart <<" "<< *yEnd << ""<< endl;
  }


  void ac_histo(GDLGStream *a, int i_buff, PLFLT *x_buff, PLFLT *y_buff )
  {
    PLFLT x,x1,y,y1;
    for ( int jj=1; jj<i_buff; ++jj){
      x1=x_buff[jj-1];
      x=x_buff[jj];
      y1=y_buff[jj-1];
      y=y_buff[jj];
      a->join(x1,y1,(x1+x)/2.0,y1);
      a->join((x1+x)/2.0,y1,(x1+x)/2.0,y);
      a->join((x1+x)/2.0,y,x,y);
    }
  }

  //CORE PLOT FUNCTION -> Draws a line along xVal, yVal
  template <typename T> bool draw_polyline(EnvT *e,  GDLGStream *a,
					   T * xVal, T* yVal, 
					   bool xLog, bool yLog, 
					   DDouble yStart, DDouble yEnd, 
					   DLong psym)
  {
    bool line=false;
    bool valid=true;
    DLong psym_=0;

    if(psym <0 ) {line=true; psym_=-psym;}
    else if(psym == 0 ) {line=true;psym_=psym;}
    else {psym_=psym;}
    DLong minEl = (xVal->N_Elements() < yVal->N_Elements())? 
      xVal->N_Elements() : yVal->N_Elements();
    // if scalar x
    if (xVal->N_Elements() == 1 && xVal->Rank() == 0) 
      minEl = yVal->N_Elements();
    // if scalar y
    if (yVal->N_Elements() == 1 && yVal->Rank() == 0) 
      minEl = xVal->N_Elements();

    DDouble *sx, *sy;
    GetSFromPlotStructs(&sx, &sy);

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)
    LPTYPE idata;
    XYTYPE odata;

    get_mapset(mapSet);

    DDouble xStart, xEnd;
    get_axis_crange("X", xStart, xEnd);

    if ( mapSet) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }
    }
#endif

    // is one of the 2 "arrays" a singleton or not ?

    PLFLT y, y1, yMapBefore, y_ref;
    int flag_y_const =0;
    y_ref = static_cast<PLFLT>( (*yVal)[0]);
    if (yVal->N_Elements() == 1 && yVal->Rank() == 0) flag_y_const=1 ;

    PLFLT x, x1, xMapBefore, x_ref;
    int flag_x_const =0;
    x_ref = static_cast<PLFLT>( (*xVal)[0]);
    if (xVal->N_Elements() == 1 && xVal->Rank() == 0) flag_x_const=1 ;

    // AC 070601 we use a buffer to use the fast ->line method
    // instead of the slow ->join one.
    // 2 tricks: 
    // trick 1/ size of buffer is limited to 1e4 (compromize syze/speed) in order to be able to manage very
    //    large among of data whitout duplicating all the arrays
    // trick 2/ when we have a NaN or and Inf, we realize the plot, then reset.
    
    int n_buff_max=500000; // idl default seems to be more than 2e6 !!
    if (minEl < n_buff_max)  n_buff_max=minEl;
    int i_buff=0;
    PLFLT *x_buff = new PLFLT[n_buff_max];
    PLFLT *y_buff = new PLFLT[n_buff_max];
   
    // flag to reset Buffer when a NaN or a Infinity are founded
    int reset=0;

    // translation plplot symbols - GDL symbols
    // for now usersym is a circle
    const PLINT codeArr[]={ 0,2,3,1,11,7,6,5,4};

    for( int i=0; i<minEl; ++i)
      {
	if (!flag_x_const) x = static_cast<PLFLT>( (*xVal)[i]); else x=x_ref;
	if (!flag_y_const) y = static_cast<PLFLT>( (*yVal)[i]); else y=y_ref;

#ifdef USE_LIBPROJ4
	if (mapSet && !e->KeywordSet("NORMAL")) {
	  idata.lam = x * DEG_TO_RAD;
	  idata.phi = y * DEG_TO_RAD;
	  if (i >0) {
	    xMapBefore=odata.x;
	    yMapBefore=odata.y;
	  }
	  odata = PJ_FWD(idata, ref);
	  x = odata.x;
	  y = odata.y;	  
	}
#endif	
	if (!isfinite(x) || !isfinite(y) || isnan(x) || isnan(y)) {
	  reset=1;
	  if (i_buff > 0) {
	    if (line) { a->line (i_buff, x_buff, y_buff);}
	    if (psym_ > 0 && psym_ < 10) { a->poin(i_buff, x_buff, y_buff, codeArr[psym_]);}
	    if (psym_ == 10) {  ac_histo( a, i_buff, x_buff, y_buff ); }
	    i_buff=0;
	  }
	  continue;
	}

#ifdef USE_LIBPROJ4
	if (mapSet && !e->KeywordSet("NORMAL")) {
	  if (i >0 ) //;&& (i_buff >0))
	    {
	      x1=xMapBefore;
	      if (!isfinite(xMapBefore) || !isfinite(yMapBefore)) continue;

	      // Break "jumps" across maps (kludge!)
	      if (fabs(x-x1) > 0.5*(xEnd-xStart)) {
		reset=1;
		if ((i_buff > 0) && (line)) {
		  a->line (i_buff, x_buff, y_buff);
		  //		  x_buff[0]=x_buff[i_buff-1];
		  //y_buff[0]=y_buff[i_buff-1];
		  i_buff=0;
		}
		continue;
	      }
	    }
	}
#endif

	if( xLog) if( x <= 0.0) continue; else x = log10( x);
	if( yLog) if( y <= 0.0) continue; else y = log10( y);

	x_buff[i_buff]=x;
	y_buff[i_buff]=y;
	i_buff=i_buff+1;
	
	if ((i_buff == n_buff_max-1) || (i == minEl-1 )) {
	  if (line) { a->line(i_buff, x_buff, y_buff); };
	  if (psym_ > 0 && psym_ < 10) { a->poin(i_buff, x_buff, y_buff, codeArr[psym_]);}
	  if (psym_ == 10) {  ac_histo( a, i_buff, x_buff, y_buff ); }
	    
	  // we must recopy the last point since the line must continue (tested via small buffer ...)
	  x_buff[0]=x_buff[i_buff-1];
	  y_buff[0]=y_buff[i_buff-1];
	  i_buff=1;
	}
      }
    
    delete[] x_buff;
    delete[] y_buff;
    
    return (valid);
  }
  // explicit instantiation for SpDDouble
  template bool draw_polyline(EnvT*, GDLGStream*, Data_<SpDDouble>*, Data_<SpDDouble>*, bool, bool, DDouble, DDouble, DLong);

/********* SA: this is not used anywhere!
  
  //CORE PLOT FUNCTION -> Draws a line along xVal, yVal
  template <typename T> bool draw_polyline_ref(EnvT *e,  GDLGStream *a,
					   T *xVal, T *yVal, 
					   bool xLog, bool yLog, 
					   DDouble yStart, DDouble yEnd, 
					   DLong psym)
  {
    bool line=false;
    bool valid=true;
    DLong psym_=0;

    if(psym <0 ) {line=true; psym_=-psym;}
    else if(psym == 0 ) {line=true;psym_=psym;}
    else {psym_=psym;}
    DLong minEl = (xVal->N_Elements() < yVal->N_Elements())? 
      xVal->N_Elements() : yVal->N_Elements();
    // if scalar x
    if (xVal->N_Elements() == 1 && xVal->Rank() == 0) 
      minEl = yVal->N_Elements();
    // if scalar y
    if (yVal->N_Elements() == 1 && yVal->Rank() == 0) 
      minEl = xVal->N_Elements();

    DDouble *sx, *sy;
    GetSFromPlotStructs(&sx, &sy);

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)
    LPTYPE idata;
    XYTYPE odata;

    get_mapset(mapSet);

    DDouble xStart, xEnd;
    get_axis_crange("X", xStart, xEnd);

    if ( mapSet) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }
    }
#endif

    for( int i=0; i<minEl; ++i)
      {
	PLFLT y;
	if (yVal->N_Elements() == 1 && yVal->Rank() == 0) 
	  y = static_cast<PLFLT>( (*yVal)[0]);
	else
	  y = static_cast<PLFLT>( (*yVal)[i]);

	if( yLog) if( y <= 0.0) continue; else y = log10( y);
	
	PLFLT x;
	if (xVal->N_Elements() == 1 && xVal->Rank() == 0) 
	  x = static_cast<PLFLT>( (*xVal)[0]);
	else
	  x = static_cast<PLFLT>( (*xVal)[i]);

	if( xLog) 
	  if( x <= 0.0) 
	    continue; 
	  else 
	    x = log10( x);

#ifdef USE_LIBPROJ4
	if (mapSet && !e->KeywordSet("NORMAL")) {
	  idata.lam = x * DEG_TO_RAD;
	  idata.phi = y * DEG_TO_RAD;
	  odata = PJ_FWD(idata, ref);
	  x = odata.x;
	  y = odata.y;
	  if (!isfinite(x) || !isfinite(y)) continue;
	}
#endif

	if( i>0)
	  {
	    if( line)
	      {
		PLFLT y1;
		if (yVal->N_Elements() == 1 && yVal->Rank() == 0) 
		  y1 = static_cast<PLFLT>( (*yVal)[0]);
		else
		  y1 = static_cast<PLFLT>( (*yVal)[i-1]);

		if( !yLog || y1 > 0.0)
		  {
		    if( yLog) y1 = log10( y1);

		    PLFLT x1;
		    if (xVal->N_Elements() == 1 && xVal->Rank() == 0) 
		      x1 = static_cast<PLFLT>( (*xVal)[0]);
		    else
		      x1 = static_cast<PLFLT>( (*xVal)[i-1]);
		    
		    if( !xLog || x1 > 0.0)
		      {
			if( xLog) x1 = log10( x1);

#ifdef USE_LIBPROJ4
			// Convert from lon/lat in degrees to radians
			// Convert from lon/lat in radians to data coord
			if (mapSet && !e->KeywordSet("NORMAL")) {
			  idata.lam = x1 * DEG_TO_RAD;
			  idata.phi = y1 * DEG_TO_RAD;
			  odata = PJ_FWD(idata, ref);
			  x1 = odata.x;
			  y1 = odata.y;
			  if (!isfinite(x1) || !isfinite(y1)) continue;

			  // Break "jumps" across maps (kludge!)
			  if (fabs(x-x1) > 0.5*(xEnd-xStart)) continue;
			}
#endif

			a->join(x1,y1,x,y);

			// cout << "join( "<<x1<<", "<<y1<<", "<<
			// x<<", "<<y<<")"<<endl;
		      }
		  }
	      }
	    else if( psym_ == 10)
	      {	// histogram
		PLFLT y1 = static_cast<PLFLT>( (*yVal)[i-1]);
		
		if( !yLog || y1 > 0.0)
		  {
		    if( yLog) y1 = log10( y1);
		    if( y1 >= yStart && y1 <= yEnd)
		      {
			PLFLT x1 = static_cast<PLFLT>( (*xVal)[i-1]);
			
			if( !xLog || x1 > 0.0)
			  {
			    if( xLog) x1 = log10( x1);
	
#ifdef USE_LIBPROJ4
			    if (mapSet && !e->KeywordSet("NORMAL")) {
			      idata.lam = x1 * DEG_TO_RAD;
			      idata.phi = y1 * DEG_TO_RAD;
			      odata = PJ_FWD(idata, ref);
			      x1 = odata.x;
			      y1 = odata.y;
			    }
#endif		    
			    a->join(x1,y1,(x1+x)/2.0,y1);
			    a->join((x1+x)/2.0,y1,(x1+x)/2.0,y);
			    a->join((x1+x)/2.0,y,x,y);
			  }
		      }
		  }
	      }
	  }
	if( psym_ == 0 || psym_ == 10) continue;
	
	// translation plplot symbols - GDL symbols
	// for now usersym is a circle
	const PLINT codeArr[]={ 0,2,3,1,11,7,6,5,4};

	if (isfinite(x) && isfinite(y)) {
	  a->poin(1,&x,&y,codeArr[psym_]);
	}
      }
    return (valid);
  }
*********/

  //MARGIN
  void gkw_axis_margin(EnvT *e, string axis,DFloat &start, DFloat &end)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();

    if(Struct != NULL)
      {
	static unsigned marginTag = Struct->Desc()->TagIndex( "MARGIN");
	start = 
	  (*static_cast<DFloatGDL*>( Struct->GetTag( marginTag, 0)))[0];
	end = 
	  (*static_cast<DFloatGDL*>( Struct->GetTag( marginTag, 0)))[1];
      }

    string MarginName=axis+"MARGIN";
    BaseGDL* Margin=e->GetKW(e->KeywordIx(MarginName));
    if(Margin !=NULL)
      {
	if(Margin->N_Elements() > 2)
	  e->Throw("Keyword array parameter "+MarginName+
		   " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* MarginF = static_cast<DFloatGDL*>
	  ( Margin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( MarginF);
	start = (*MarginF)[0];
	if( MarginF->N_Elements() > 1)
	  end = (*MarginF)[1];
      }
  }

  //BACKGROUND COLOR
  void gkw_background(EnvT *e, GDLGStream *a, bool kw)
  {
    static DStructGDL* pStruct = SysVar::P();
    DLong background = 
      (*static_cast<DLongGDL*>
       (pStruct->GetTag( pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
    if(kw)
      e->AssureLongScalarKWIfPresent( "BACKGROUND", background);

    // Get decomposed value
    Graphics* actDevice = Graphics::GetDevice();
    DLong decomposed = actDevice->GetDecomposed();
    if (decomposed != 0 && decomposed != 1) {decomposed=0;}

    a->Background( background, decomposed);  
  }

  //COLOR
  void gkw_color(EnvT *e, GDLGStream *a)
  {
    // Get COLOR from PLOT system variable
    static DStructGDL* pStruct = SysVar::P();
    DLong color = 
      (*static_cast<DLongGDL*>
       (pStruct->GetTag( pStruct->Desc()->TagIndex("COLOR"), 0)))[0];

    // Get # of colors from DEVICE system variable
    DVar *var=FindInVarList(sysVarList,"D");
    DStructGDL* s = static_cast<DStructGDL*>( var->Data());
    DLong ncolor = (*static_cast<DLongGDL*>
                    (s->GetTag(s->Desc()->TagIndex("N_COLORS"), 0)))[0];

    if (ncolor > 256 && color == 255) color = ncolor - 1;

    e->AssureLongScalarKWIfPresent( "COLOR", color);

    // Get decomposed value
    Graphics* actDevice = Graphics::GetDevice();
    DLong decomposed = actDevice->GetDecomposed();
    if (decomposed != 0 && decomposed != 1) {decomposed=0;}
    a->Color( color, decomposed, 2);  
  }

  // helper for NOERASE (but also used in XYOUTS)
  void handle_pmulti_position(EnvT *e, GDLGStream *a)
  {
    // !P.MULTI is ignored if POSITION kw or !P.POSITION or !P.REGION is specified
    // TODO: !P.REGION!

    DFloatGDL* pos = NULL;

    // system variable
    static DStructGDL* pStruct = SysVar::P();
    pos = static_cast<DFloatGDL*>(pStruct-> GetTag( pStruct->Desc()->TagIndex("POSITION"), 0));
    if ((*pos)[0] == (*pos)[2]) pos = NULL;

    // keyword
    if (pos == NULL) 
    {
      DSub* pro = e->GetPro();
      int positionIx = pro->FindKey( "POSITION");
      if (positionIx != -1) pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    }

    if (pos != NULL) a->NoSub();
  }

  //NOERASE
  void gkw_noerase(EnvT *e,GDLGStream *a, bool noe)
  {
    DLong noErase=0;
    DLongGDL* pMulti = SysVar::GetPMulti();
    static DStructGDL* pStruct = SysVar::P();

    if(!noe)
      {
	noErase = (*static_cast<DLongGDL*>
		   ( pStruct->
		     GetTag( pStruct->Desc()->TagIndex("NOERASE"), 0)))[0];
	if(e->KeywordSet("NOERASE")) {
	  noErase=1;
	}
      }
    else
      {
	noErase=1;
      }

    a->NextPlot( !noErase);
    handle_pmulti_position(e, a);
  }

  //PSYM
  void gkw_psym(EnvT *e, GDLGStream *a, bool &line, DLong &psym)
  {
    static DStructGDL* pStruct = SysVar::P();
    psym= (*static_cast<DLongGDL*>
	   (pStruct->GetTag(pStruct->Desc()->TagIndex("PSYM"), 0)))[0];

    line = false;
    e->AssureLongScalarKWIfPresent( "PSYM", psym);
    if( psym > 10 || psym < -8 || psym == 9)
      e->Throw( 
			  "PSYM (plotting symbol) out of range.");
  }

    //SYMSIZE
  void gkw_symsize(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DFloat symsize = (*static_cast<DFloatGDL*>
		      (pStruct->GetTag( pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
    e->AssureFloatScalarKWIfPresent( "SYMSIZE", symsize);
    if( symsize <= 0.0) symsize = 1.0;
    a->ssym(0.0, symsize);  
  }

  //CHARSIZE
  void gkw_charsize(EnvT *e, GDLGStream *a, DFloat &charsize, bool kw)
  {
    static DStructGDL* pStruct = SysVar::P();
    charsize = (*static_cast<DFloatGDL*>
			(pStruct->GetTag
			 ( pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    if(kw)
      e->AssureFloatScalarKWIfPresent( "CHARSIZE", charsize);

    if( charsize <= 0.0) charsize = 1.0;
    a->schr(0.0, charsize);  
  }
  //THICK
  void gkw_thick(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DFloat thick = (*static_cast<DFloatGDL*>
		    (pStruct->GetTag( pStruct->Desc()->TagIndex("THICK"), 0)))[0];

    e->AssureFloatScalarKWIfPresent( "THICK", thick);
    if( thick <= 0.0) thick = 1.0;
    a->wid( static_cast<PLINT>(floor( thick-0.5)));
  }

  // AC 18 juin 2007 
  // NOT READY NOW
  //LINESTYLE for contour
  void gkw_linestyle_c(EnvT *e, GDLGStream *a, bool OnlyPline)
  {
    if (OnlyPline == false) {
      // if the LINESTYLE keyword is present, the value will be change
      DLong temp_linestyle=-1111;
      e->AssureLongScalarKWIfPresent( "LINESTYLE",temp_linestyle);
    }
    static DStructGDL* pStruct = SysVar::P();
    DLong linestyle= 
      (*static_cast<DLongGDL*>
       (pStruct->GetTag( pStruct->Desc()->TagIndex("LINESTYLE"), 0)))[0];

    if (linestyle < 0 ) {linestyle=0;}
    if (linestyle > 5 ) {linestyle=5;}

    //    if (linestyle == 1) { // dots
      static PLINT nbp=1;
      static PLINT mark[] = {75};
      static PLINT space[] = {1500};
      a->styl(nbp, mark, space);
    
    
  }
  //LINESTYLE
  void gkw_linestyle(EnvT *e, GDLGStream *a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DLong linestyle= 
      (*static_cast<DLongGDL*>
       (pStruct->GetTag( pStruct->Desc()->TagIndex("LINESTYLE"), 0)))[0];

    // if the LINESTYLE keyword is present, the value will be change
    DLong temp_linestyle=-1111;
    e->AssureLongScalarKWIfPresent( "LINESTYLE",temp_linestyle);
    
    bool debug=false;
    if (debug) {
      cout << "temp_linestyle " <<  temp_linestyle << endl;
      cout << "     linestyle " <<  linestyle << endl;
    }
    if (temp_linestyle != -1111) {linestyle=temp_linestyle;}//+1;
    if (linestyle < 0 ) {linestyle=0;}
    if (linestyle > 5 ) {linestyle=5;}

      // see
      // file:///home/coulais/SoftsExternes/plplot-5.5.3/examples/c++/x09.cc
      // file:///home/coulais/SoftsExternes/plplot-5.5.3/doc/docbook/src/plstyl.html

    if (linestyle == 0) { // solid (continuous line)
      static PLINT nbp=0;
      a->styl(nbp, NULL, NULL);
    }
    if (linestyle == 1) { // dots
      static PLINT nbp=1;
      static PLINT mark[] = {75};
      static PLINT space[] = {1500};
      a->styl(nbp, mark, space);
    }
    if (linestyle == 2) { // dashed
      static PLINT nbp=1;
      static PLINT mark[] = {1500};
      static PLINT space[] = {1500};
      a->styl(nbp, mark, space);
  }
    if (linestyle == 3) { // dash dot
      static PLINT nbp=2;
      static PLINT mark[] = {1500,100};
      static PLINT space[] = {1000,1000};
      a->styl(nbp, mark, space);
    }
    if (linestyle == 4) { // dash dot dot
      static PLINT nbp=4;
      static PLINT mark[] = {1500,100,100,100};
      static PLINT space[] = {1000,1000,1000,1000};
      a->styl(nbp, mark, space);
    }
    if (linestyle == 5) { // long dash
      static PLINT nbp=1;
      static PLINT mark[] = {3000};
      static PLINT space[] = {1500}; 
      a->styl(nbp, mark, space);
    }
  }

  //TITLE
  void gkw_title(EnvT* e, GDLGStream *a, PLFLT ad)
  {
    DLong thick=0;
    e->AssureLongScalarKWIfPresent("CHARTHICK",thick);
    a->wid(thick);

    static DStructGDL* pStruct = SysVar::P();
    static unsigned titleTag = pStruct->Desc()->TagIndex( "TITLE");
    static unsigned subTitleTag = pStruct->Desc()->TagIndex( "SUBTITLE");
    DString title =   
      (*static_cast<DStringGDL*>( pStruct->GetTag( titleTag, 0)))[0];
    DString subTitle =  
      (*static_cast<DStringGDL*>( pStruct->GetTag( subTitleTag, 0)))[0];
    e->AssureStringScalarKWIfPresent( "TITLE", title);
    e->AssureStringScalarKWIfPresent( "SUBTITLE", subTitle);

    a->schr( 0.0, 1.25*ad);
    a->mtex("t",1.25,0.5,0.5,title.c_str());
    a->schr( 0.0, ad); // charsize is reset here
    a->mtex("b",5.4,0.5,0.5,subTitle.c_str());
    a->wid(0);
  }

  //crange to struct
  void set_axis_crange(string axis, DDouble Start, DDouble End)
  {
    DStructGDL* Struct=NULL;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(axis=="Z") Struct = SysVar::Z();
    if(Struct!=NULL)
      {
	static unsigned crangeTag = Struct->Desc()->TagIndex( "CRANGE");
	(*static_cast<DDoubleGDL*>( Struct->GetTag( crangeTag, 0)))[0] = Start;
	(*static_cast<DDoubleGDL*>( Struct->GetTag( crangeTag, 0)))[1] = End;
      }
  }

  //CRANGE from struct
  void get_axis_crange(string axis, DDouble &Start, DDouble &End)
  {
    DStructGDL* Struct=NULL;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(axis=="Z") Struct = SysVar::Z();
    if(Struct!=NULL)
      {
	static unsigned crangeTag = Struct->Desc()->TagIndex( "CRANGE");
	Start = (*static_cast<DDoubleGDL*>( Struct->GetTag( crangeTag, 0)))[0]; 
	End = (*static_cast<DDoubleGDL*>( Struct->GetTag( crangeTag, 0)))[1];
      }
  }

   void get_axis_type(string axis,bool &log)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(axis=="Z") Struct = SysVar::Z();
    if(Struct != NULL) {
      static unsigned typeTag = Struct->Desc()->TagIndex( "TYPE");
      if ((*static_cast<DLongGDL*>(Struct->GetTag( typeTag, 0)))[0] == 1)
	log = 1;
      else
	log=0;
    }
  }

  void get_mapset(bool &mapset)
  {
    DStructGDL* Struct = SysVar::X();
    if(Struct != NULL) {
      static unsigned typeTag = Struct->Desc()->TagIndex( "TYPE");

      if ((*static_cast<DLongGDL*>(Struct->GetTag( typeTag, 0)))[0] == 3)
	mapset = 1;
      else
	mapset = 0;
    }
  }

  void set_mapset(bool mapset)
  {
    DStructGDL* Struct = SysVar::X();
    if(Struct!=NULL)
      {
	static unsigned typeTag = Struct->Desc()->TagIndex( "TYPE");
	(*static_cast<DLongGDL*>( Struct->GetTag( typeTag, 0)))[0] = mapset;
      }
  }
  

  //axis type (log..)
  void set_axis_type(string axis, bool Type)
  {
    DStructGDL* Struct=NULL;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(axis=="Z") Struct = SysVar::Z();
    if(Struct!=NULL)
      {
	static unsigned typeTag = Struct->Desc()->TagIndex("TYPE");   
	(*static_cast<DLongGDL*>(Struct->GetTag(typeTag, 0)))[0] = Type; 
      }
  }

  void gkw_axis_charsize(EnvT* e, string axis, DFloat &charsize)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();

    if(Struct != NULL)
      {
	static unsigned charsizeTag = Struct->Desc()->TagIndex("CHARSIZE");
	charsize = 
	  (*static_cast<DFloatGDL*>( Struct->GetTag( charsizeTag, 0)))[0];
      }

    string Charsize_s=axis+"CHARSIZE";
    e->AssureFloatScalarKWIfPresent( Charsize_s, charsize);
    if(charsize <=0.0) charsize=1.0;
  }


  //STYLE
  void gkw_axis_style(EnvT *e, string axis, DLong &style)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(Struct != NULL)
      {
	static unsigned styleTag = Struct->Desc()->TagIndex( "STYLE");
	style = 
	  (*static_cast<DLongGDL*>( Struct->GetTag( styleTag, 0)))[0];
      }

    string StyleName=axis+"STYLE";

  }

  void gkw_axis_title(EnvT *e, string axis, DString &title)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    
    if(Struct != NULL)
      {
	static unsigned titleTag = Struct->Desc()->TagIndex("TITLE");
	title = 
	  (*static_cast<DStringGDL*>( Struct->GetTag( titleTag, 0)))[0];
      }

    string TitleName=axis+"TITLE";
    e->AssureStringScalarKWIfPresent( TitleName, title);

  }

  //GET RANGE
  void gkw_axis_range(EnvT *e, string axis, DDouble &start, DDouble &end, 
		      DLong &ynozero)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if (Struct != NULL) 
      {
	DDouble test1, test2;
	static unsigned rangeTag = Struct->Desc()->TagIndex( "RANGE");
	test1 = (*static_cast<DDoubleGDL*>( Struct->GetTag( rangeTag, 0)))[0]; 
	test2 = (*static_cast<DDoubleGDL*>( Struct->GetTag( rangeTag, 0)))[1];
	if (!(test1 == 0.0 && test2 == 0.0)) 
	{ 
		start = test1; 
		end = test2; 
	}
      }
    string RangeName=axis+"RANGE";
    BaseGDL* Range=e->GetKW(e->KeywordIx(RangeName));
    if(Range !=NULL)
      {
	if(Range->N_Elements() != 2)
	  e->Throw("Keyword array parameter "+RangeName+
		   " must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* RangeF = static_cast<DFloatGDL*>
	  ( Range->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( RangeF);
	start = (*RangeF)[0];
	end = (*RangeF)[1];
	if(axis=="Y") ynozero=1;
      }
  }

  void get_axis_margin(string axis, DFloat &low, DFloat &high)
  {
    DStructGDL* Struct=NULL;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(Struct!=NULL)
      {
	static unsigned marginTag = Struct->Desc()->TagIndex( "MARGIN");
	low = (*static_cast<DFloatGDL*>( Struct->GetTag( marginTag, 0)))[0]; 
	high = (*static_cast<DFloatGDL*>( Struct->GetTag( marginTag, 0)))[1];
      }
  }

} // namespace
