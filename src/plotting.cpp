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

  void set_plot( EnvT* e) // modifies !D system variable
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");
    DString device;
    e->AssureScalarPar<DStringGDL>( 0, device);

    // this is the device name
    device = StrUpCase( device);
    
    bool success = Graphics::SetDevice( device);
    if( !success)
      e->Throw( "Device not supported/unknown: "+device);

    if (device == "PS" || device == "SVG") {
      static DStructGDL* pStruct = SysVar::P();
      // SA: this does not comply with IDL behaviour, see testsuite/test_pmulti.pro
      //static unsigned noEraseTag = pStruct->Desc()->TagIndex( "NOERASE");
      //(*static_cast<DLongGDL*>( pStruct->GetTag( noEraseTag, 0)))[0] = 1;
      if (device == "PS") {
	static unsigned colorTag = pStruct->Desc()->TagIndex( "COLOR");
	(*static_cast<DLongGDL*>( pStruct->GetTag( colorTag, 0)))[0] = 0;
      }
    } else {
      static DStructGDL* pStruct = SysVar::P();
      static unsigned colorTag = pStruct->Desc()->TagIndex( "COLOR");
      (*static_cast<DLongGDL*>( pStruct->GetTag( colorTag, 0)))[0] = 255;
    }
  }

  void tvlct( EnvT* e)
  {
    int nParam = e->NParam( 1); //, "TVLCT");

    static int getKW = e->KeywordIx( "GET");
    bool get = e->KeywordSet( getKW);
    static int hlsKW = e->KeywordIx( "HLS");
    bool hls = e->KeywordSet( hlsKW);
    static int hsvKW = e->KeywordIx( "HSV");
    bool hsv = e->KeywordSet( hsvKW);

    DLong start = 0;
    GDLCT* actCT = Graphics::GetCT();
    if( nParam <= 2) // TVLCT,I[,Start]
      {
	if( nParam == 2)
	  e->AssureLongScalarPar( 1, start);
	if( start < 0) start = 0;

 	if( get) // always RGB
	  {
	    BaseGDL*& p0 = e->GetParGlobal( 0);

	    SizeT nCol = ctSize - start;

	    DByteGDL* rgb = new DByteGDL( dimension( nCol, 3), BaseGDL::NOZERO);

	    for( SizeT i=start,ii=0; i<ctSize; ++i,++ii)
	      actCT->Get( i, (*rgb)[ ii], (*rgb)[ ii+nCol], (*rgb)[ ii+2*nCol]);

	    delete p0;
	    p0 = rgb;
	  }
	else
	  {
	    if( hls || hsv)
	      {
		DFloatGDL* rgb = e->GetParAs< DFloatGDL>( 0);
		if( rgb->Dim( 1) != 3)
		  e->Throw( "Array must have dimensions of (1, 3): "+
			    e->GetParString( 0));

		SizeT nCol = rgb->Dim( 0);
		SizeT setCol = nCol + start;
		if( setCol > ctSize) setCol = ctSize;

		if( hls)
		  for( SizeT i=start, ii=0; i<setCol; ++i,++ii)
		    actCT->SetHLS( i, 
				   (*rgb)[ ii], 
				   (*rgb)[ ii+nCol],
				   (*rgb)[ ii+2*nCol]);
		else
		  for( SizeT i=start, ii=0; i<setCol; ++i,++ii)
		    actCT->SetHSV( i, 
				   (*rgb)[ ii], 
				   (*rgb)[ ii+nCol],
				   (*rgb)[ ii+2*nCol]);
	      }
	    else
	      {
		DByteGDL* rgb = e->GetParAs< DByteGDL>( 0);
		if( rgb->Dim( 1) != 3)
		  e->Throw( "Array must have dimensions of (1, 3): "+
			    e->GetParString( 0));

		SizeT nCol = rgb->Dim( 0);
		SizeT setCol = nCol + start;
		if( setCol > ctSize) setCol = ctSize;

		for( SizeT i=start, ii=0; i<setCol; ++i,++ii)
		  actCT->Set( i, 
			      (*rgb)[ ii], 
			      (*rgb)[ ii+nCol],
			      (*rgb)[ ii+2*nCol]);
	      }
	  }
      }
    else // TVLCT,I1,I2,I3[,Start]
      {
	if( nParam == 4)
	  e->AssureLongScalarPar( 3, start);
	if( start < 0) start = 0;

 	if( get) // always RGB
	  {
	    BaseGDL*& p0 = e->GetParGlobal( 0);
	    BaseGDL*& p1 = e->GetParGlobal( 1);
	    BaseGDL*& p2 = e->GetParGlobal( 2);

	    SizeT nCol = ctSize - start;

	    DByteGDL* r = new DByteGDL( dimension( nCol), BaseGDL::NOZERO);
	    DByteGDL* g = new DByteGDL( dimension( nCol), BaseGDL::NOZERO);
	    DByteGDL* b = new DByteGDL( dimension( nCol), BaseGDL::NOZERO);

	    for( SizeT i=start,ii=0; i<ctSize; ++i,++ii)
	      actCT->Get( i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);

	    delete p0; p0 = r;
	    delete p1; p1 = g;
	    delete p2; p2 = b;
	  }
	else
	  {
	    if( hls || hsv)
	      {
		DFloatGDL* r = e->GetParAs< DFloatGDL>( 0);
		DFloatGDL* g = e->GetParAs< DFloatGDL>( 1);
		DFloatGDL* b = e->GetParAs< DFloatGDL>( 2);
		SizeT rCol = r->N_Elements();
		SizeT gCol = g->N_Elements();
		SizeT bCol = b->N_Elements();
		SizeT nCol = rCol;
		if( gCol < nCol) nCol = gCol;
		if( bCol < nCol) nCol = bCol;
	
		SizeT setCol = nCol + start;
		if( setCol > ctSize) setCol = ctSize;

		if( hls)
		  for( SizeT i=start, ii=0; i<setCol; ++i,++ii)
		    actCT->SetHLS( i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);
		else
		  for( SizeT i=start, ii=0; i<setCol; ++i,++ii)
		    actCT->SetHSV( i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);
	      }
	    else
	      {
		DByteGDL* r = e->GetParAs< DByteGDL>( 0);
		DByteGDL* g = e->GetParAs< DByteGDL>( 1);
		DByteGDL* b = e->GetParAs< DByteGDL>( 2);
		SizeT rCol = r->N_Elements();
		SizeT gCol = g->N_Elements();
		SizeT bCol = b->N_Elements();
		SizeT nCol = rCol;
		if( gCol < nCol) nCol = gCol;
		if( bCol < nCol) nCol = bCol;
	
		SizeT setCol = nCol + start;
		if( setCol > ctSize) setCol = ctSize;

		for( SizeT i=start, ii=0; i<setCol; ++i,++ii)
		  actCT->Set( i, (*r)[ ii], (*g)[ ii], (*b)[ ii]);
	      }
	  }
      }
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
  PLFLT AutoIntvAC(DDouble &val_min, DDouble &val_max, DLong NoZero)
  {
    PLFLT intv = 1.;
    int cas = 0 ;
    DDouble x; 
    bool debug = false ;
    if (debug) {cout << "init: " <<  val_min << " " << val_max << endl;}
    
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

  GDLGStream* GetPlotStream( EnvT* e)
  {
    Graphics* actDevice=Graphics::GetDevice();
    GDLGStream* actStream = actDevice->GetStream();
    if( actStream == NULL)
      e->Throw( "Unable to create window.");
    return actStream;
  }
  
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
		 DDouble& xStart,
		 DDouble& xEnd,
		 DDouble& minVal,
		 DDouble& maxVal)
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


  void erase( EnvT* e)
  {
    SizeT nParam=e->NParam();
    GDLGStream* actStream = GetPlotStream( e);

    if( nParam > 1)
      e->Throw( "Incorrect number of arguments.");

    if( nParam == 0)
      actStream->Clear();
    else {
      DLong bColor;
      e->AssureLongScalarPar( 0, bColor);
      if (bColor > 255) bColor = 255;
      if (bColor < 0)   bColor = 0;
      actStream->Clear( bColor);
    }
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

  //NOERASE
  void gkw_noerase(EnvT *e,GDLGStream *a, bool noe)
  {
    DLong noErase=0;
    DLongGDL* pMulti = SysVar::GetPMulti();

    if(!noe)
      {
	static DStructGDL* pStruct = SysVar::P();
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

    DSub* pro = e->GetPro();
    int positionIx = pro->FindKey( "POSITION");
    DFloatGDL* pos = NULL;
    if( positionIx != -1)
      pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);

    a->NextPlot( !noErase);

    if (pos != NULL) a->NoSub();
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


#ifdef USE_LIBPROJ4

  BaseGDL* map_proj_forward_fun( EnvT* e)
  {
    // lonlat -> xy

    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    LPTYPE idata;
    XYTYPE odata;

    ref = map_init();
    if ( ref == NULL) {
      e->Throw( "Projection initialization failed.");
    }

    BaseGDL* p0;
    BaseGDL* p1;

    DDoubleGDL* lon;
    DDoubleGDL* lat;
    DDoubleGDL* ll;
    DDoubleGDL* res;
    DLong dims[2];

    if ( nParam == 1) {
      p0 = e->GetParDefined( 0);
      DDoubleGDL* ll = static_cast<DDoubleGDL*>
	(p0->Convert2( DOUBLE, BaseGDL::COPY));

      dims[0] = 2;
      if (p0->Rank() == 1) {
	dimension dim((DLong *) dims, 1);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      } else {
	dims[1] = p0->Dim(1);
	dimension dim((DLong *) dims, 2);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      }

      SizeT nEl = p0->N_Elements();
      for( SizeT i=0; i<nEl/2; ++i) {
	idata.lam = (*ll)[2*i]   * DEG_TO_RAD;
	idata.phi = (*ll)[2*i+1] * DEG_TO_RAD;
	odata = PJ_FWD(idata, ref);
	(*res)[2*i]   = odata.x;
	(*res)[2*i+1] = odata.y;
      }
      return res;

    } else if ( nParam == 2) {
      p0 = e->GetParDefined( 0);
      p1 = e->GetParDefined( 1);
      DDoubleGDL* lon = static_cast<DDoubleGDL*>
	(p0->Convert2( DOUBLE, BaseGDL::COPY));
      DDoubleGDL* lat = static_cast<DDoubleGDL*>
	(p1->Convert2( DOUBLE, BaseGDL::COPY));

      dims[0] = 2;
      if (p0->Rank() == 0 || p0->Rank() == 1) {
	dimension dim((DLong *) dims, 1);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      } else {
	dims[1] = p0->Dim(0);
	dimension dim((DLong *) dims, 2);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      }

      SizeT nEl = p0->N_Elements();
      for( SizeT i=0; i<nEl; ++i) {
	idata.lam = (*lon)[i] * DEG_TO_RAD;
	idata.phi = (*lat)[i] * DEG_TO_RAD;
	odata = PJ_FWD(idata, ref);
	(*res)[2*i]   = odata.x;
	(*res)[2*i+1] = odata.y;
      }
      return res;
    }
  }


  BaseGDL* map_proj_inverse_fun( EnvT* e)
  {
    // xy -> lonlat
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    XYTYPE idata;
    LPTYPE odata;

    ref = map_init();
    if ( ref == NULL) {
      e->Throw( "Projection initialization failed.");
    }

    BaseGDL* p0;
    BaseGDL* p1;

    DDoubleGDL* x;
    DDoubleGDL* y;
    DDoubleGDL* xy;
    DDoubleGDL* res;
    DLong dims[2];

    if ( nParam == 1) {
      p0 = e->GetParDefined( 0);
      DDoubleGDL* xy = static_cast<DDoubleGDL*>
	(p0->Convert2( DOUBLE, BaseGDL::COPY));

      dims[0] = 2;
      if (p0->Rank() == 1) {
	dimension dim((DLong *) dims, 1);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      } else {
	dims[1] = p0->Dim(1);
	dimension dim((DLong *) dims, 2);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      }

      SizeT nEl = p0->N_Elements();
      for( SizeT i=0; i<nEl/2; ++i) {
	idata.x = (*xy)[2*i];
	idata.y = (*xy)[2*i+1];
	odata = PJ_INV(idata, ref);
	(*res)[2*i]   = odata.lam * RAD_TO_DEG;
	(*res)[2*i+1] = odata.phi * RAD_TO_DEG;
      }
      return res;

    } else if ( nParam == 2) {
      p0 = e->GetParDefined( 0);
      p1 = e->GetParDefined( 1);
      DDoubleGDL* x = static_cast<DDoubleGDL*>
	(p0->Convert2( DOUBLE, BaseGDL::COPY));
      DDoubleGDL* y = static_cast<DDoubleGDL*>
	(p1->Convert2( DOUBLE, BaseGDL::COPY));

      dims[0] = 2;
      if (p0->Rank() == 0 || p0->Rank() == 1) {
	dimension dim((DLong *) dims, 1);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      } else {
	dims[1] = p0->Dim(0);
	dimension dim((DLong *) dims, 2);
	res = new DDoubleGDL( dim, BaseGDL::NOZERO);
      }

      SizeT nEl = p0->N_Elements();
      for( SizeT i=0; i<nEl; ++i) {
	idata.x = (*x)[i];
	idata.y = (*y)[i];
	odata = PJ_INV(idata, ref);
	(*res)[2*i]   = odata.lam * RAD_TO_DEG;
	(*res)[2*i+1] = odata.phi * RAD_TO_DEG;
      }
      return res;
    }
  }
#endif

  template< typename T1, typename T2>
  BaseGDL* convert_coord_template( EnvT* e, 
				   BaseGDL* p0, BaseGDL* p1, BaseGDL* p2,
				   DDouble *sx, DDouble *sy, DDouble *sz,
				   DLong xv, DLong yv, DLong xt, DLong yt)
  {
    DLong dims[2]={3,0};
    if( e->NParam() == 1) {
      if (p0->Dim(0) != 2 && p0->Dim(0) != 3)
	e->Throw( "When only 1 param, dims must be (2,n) or (3,n)");
    }

    DType aTy;
    if (p0->Type() == DOUBLE || e->KeywordSet("DOUBLE"))
      aTy = DOUBLE;
    else
      aTy = FLOAT;

    T1* res;
    SizeT nrows;

    if (p0->Rank() == 0) {
      nrows = 1;
      dimension dim((DLong *) dims, 1);
      res = new T1( dim, BaseGDL::ZERO);
    } else if (p0->Rank() == 1) {
      if (e->NParam() == 1) {
	nrows = 1;
	dimension dim((DLong *) dims, 1);
	res = new T1( dim, BaseGDL::ZERO);
      } else {
	nrows = p0->Dim(0);
	dims[1] = nrows;
	dimension dim((DLong *) dims, 2);
	res = new T1( dim, BaseGDL::ZERO);
      }
    } else {
      // rank == 2
      nrows = 1;
      for( SizeT i = 0; i<2; ++i) {	
	nrows  *= p0->Dim(i);
      }
      dims[1] = nrows;
      dimension dim((DLong *) dims, 2);
      res = new T1( dim, BaseGDL::ZERO);
    }

    T1 *in, *in1, *in2, *in3;
    T2 *ptr1, *ptr2, *ptr3;
    DLong deln=1, ires=0;
    bool third = false;
    if( e->NParam() == 1) { 
      in = static_cast<T1*>(p0->Convert2( aTy, BaseGDL::COPY));
      ptr1 = &(*in)[0];
      ptr2 = &(*in)[1];
      if (p0->Dim(0) == 3) {
	ptr3 = &(*in)[2];
	third = true;
      }
      deln = p0->Dim(0);
    } else if( e->NParam() == 2) {
      in1 = static_cast<T1*>(p0->Convert2( aTy, BaseGDL::COPY));
      in2 = static_cast<T1*>(p1->Convert2( aTy, BaseGDL::COPY));
      ptr1 = &(*in1)[0];
      ptr2 = &(*in2)[0];
      ptr3 = NULL;
    } else {
      in1 = static_cast<T1*>(p0->Convert2( aTy, BaseGDL::COPY));
      in2 = static_cast<T1*>(p1->Convert2( aTy, BaseGDL::COPY));
      in3 = static_cast<T1*>(p2->Convert2( aTy, BaseGDL::COPY));
      ptr1 = &(*in1)[0];
      ptr2 = &(*in2)[0];
      ptr3 = &(*in3)[0];
      third = true;
    }

#ifdef USE_LIBPROJ4
    // MAP conversion (xt = 3)
    if (xt == 3) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }

      // ll -> xy
      // lam = longitude  phi = latitude
      if (e->KeywordSet("DATA") || (!e->KeywordSet("DEVICE") && 
				    !e->KeywordSet("NORMAL"))) {
	if (!e->KeywordSet("TO_DEVICE") && 
	    !e->KeywordSet("TO_NORMAL")) {
	  for( SizeT i = 0; i<nrows; ++i) {	
	    (*res)[ires++] = (*ptr1);
	    (*res)[ires++] = (*ptr2);
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
	} else if (e->KeywordSet("TO_NORMAL")) {
	  LPTYPE idata;
	  XYTYPE odata;
	  for( SizeT i = 0; i<nrows; ++i) {	
	    idata.lam = (*ptr1) * DEG_TO_RAD;
	    idata.phi = (*ptr2) * DEG_TO_RAD;
	    odata = PJ_FWD(idata, ref);
	    (*res)[ires++] = odata.x * sx[1] + sx[0];
	    (*res)[ires++] = odata.y * sy[1] + sy[0];
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
	} else if (e->KeywordSet("TO_DEVICE")) {
	  LPTYPE idata;
	  XYTYPE odata;
	  for( SizeT i = 0; i<nrows; ++i) {	
	    idata.lam = (*ptr1) * DEG_TO_RAD;
	    idata.phi = (*ptr2) * DEG_TO_RAD;
	    odata = PJ_FWD(idata, ref);
	    (*res)[ires++] = xv * (odata.x * sx[1] + sx[0]);
	    (*res)[ires++] = yv * (odata.y * sy[1] + sy[0]);
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
	}
	// xy -> ll
      } else if (e->KeywordSet("NORMAL")) {
        if (e->KeywordSet("TO_DEVICE")) {
          for( SizeT i = 0; i<nrows; ++i) {
            (*res)[ires++] = xv * (*ptr1);
            (*res)[ires++] = yv * (*ptr2);
            if (third)
              (*res)[ires++] = (*ptr3);
            else
              ires++;
            ptr1 += deln;
            ptr2 += deln;
            ptr3 += deln;
          }
        } else {
	  XYTYPE idata;
	  LPTYPE odata;
	  for( SizeT i = 0; i<nrows; ++i) {	
	    idata.x = ((*ptr1) - sx[0]) / sx[1];
	    idata.y = ((*ptr2) - sy[0]) / sy[1];
	    odata = PJ_INV(idata, ref);
	    (*res)[ires++] = odata.lam * RAD_TO_DEG;
	    (*res)[ires++] = odata.phi * RAD_TO_DEG;
	    ptr1++;
	    ptr2++;
	    ires++;
	  }
        }
      } else if (e->KeywordSet("DEVICE")) {
	XYTYPE idata;
	LPTYPE odata;
	for( SizeT i = 0; i<nrows; ++i) {	
	  idata.x = ((*ptr1) / xv - sx[0]) / sx[1];
	  idata.y = ((*ptr2) / yv - sy[0]) / sy[1];
	  odata = PJ_INV(idata, ref);
	  (*res)[ires++] = odata.lam * RAD_TO_DEG;
	  (*res)[ires++] = odata.phi * RAD_TO_DEG;
	  ptr1++;
	  ptr2++;
	  ires++;
	}
      }
      // Change Inf to Nan
      {
        static DStructGDL *Values = SysVar::Values();
        DDouble d_nan = (*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
        for( SizeT i = 0; i<res->N_Elements(); ++i) {	
          if (isinf((DDouble) (*res)[i]) != 0) (*res)[i] = d_nan;
        }
      }
      return res;
    }
#endif

    // in: DATA  out: NORMAL/DEVICE
    if (e->KeywordSet("DATA") || (!e->KeywordSet("DEVICE") && 
	!e->KeywordSet("NORMAL"))) {
      for( SizeT i = 0; i<nrows; ++i) {	
	if (xt == 0)
	  (*res)[ires++] = xv * (sx[0] + sx[1] * (*ptr1));
	else
	  (*res)[ires++] = xv * (sx[0] + sx[1] * log10((*ptr1)));

	if (yt == 0)
	  (*res)[ires++] = yv * (sy[0] + sy[1] * (*ptr2));
	else
	  (*res)[ires++] = yv * (sy[0] + sy[1] * log10((*ptr2)));

	if (third)
	  (*res)[ires++] = sz[0] + sz[1] * (*ptr3);
	else
	  ires++;
	ptr1 += deln;
	ptr2 += deln;
	ptr3 += deln;
      }
    // in: NORMAL  out: DEVICE/DATA
    } else if (e->KeywordSet("NORMAL")) {
      if (e->KeywordSet("TO_DEVICE")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  (*res)[ires++] = xv * (*ptr1);
	  (*res)[ires++] = yv * (*ptr2);
	  if (third)
	    (*res)[ires++] = (*ptr3);
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      } else if (!e->KeywordSet("TO_NORMAL")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  if (xt == 0)
	    (*res)[ires++] = ((*ptr1) - sx[0]) / sx[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr1) - sx[0]) / sx[1]);
	
	  if (yt == 0)
	    (*res)[ires++] = ((*ptr2) - sy[0]) / sy[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr2) - sy[0]) / sy[1]);

	  if (third)
	    (*res)[ires++] = ((*ptr3) - sz[0]) / sz[1];
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      }
    // in: DEVICE  out: NORMAL/DATA
    } else if (e->KeywordSet("DEVICE")) {
      if (e->KeywordSet("TO_NORMAL")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  (*res)[ires++] = (*ptr1) / xv;
	  (*res)[ires++] = (*ptr2) / yv;
	  if (third)
	    (*res)[ires++] = (*ptr3);
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      } else if (!e->KeywordSet("TO_DEVICE")) {
	for( SizeT i = 0; i<nrows; ++i) {
	  if (xt == 0)
	    (*res)[ires++] = ((*ptr1) / xv - sx[0]) / sx[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr1) / xv - sx[0]) / sx[1]);
	  
	  if (yt == 0)
	    (*res)[ires++] = ((*ptr2) / yv - sy[0]) / sy[1];
	  else
	    (*res)[ires++] = pow(10.,((*ptr2) / yv - sy[0]) / sy[1]);
	  
	  if (third)
	    (*res)[ires++] = ((*ptr3) - sz[0]) / sz[1];
	  else
	    ires++;
	  ptr1 += deln;
	  ptr2 += deln;
	  ptr3 += deln;
	}
      }	
    }
    return res;
  }


  BaseGDL* convert_coord( EnvT* e) 
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0;
    BaseGDL* p1;
    BaseGDL* p2;

    p0 = e->GetParDefined( 0);
    if (nParam >= 2)
      p1 = e->GetParDefined( 1);
    if (nParam == 3)
      p2 = e->GetParDefined( 2);

    DDouble *sx, *sy;
    GetSFromPlotStructs(&sx, &sy);

    static DStructGDL* zStruct = SysVar::Z();
    static unsigned szTag = zStruct->Desc()->TagIndex( "S");
    DDouble *sz = &(*static_cast<DDoubleGDL*>( zStruct->GetTag( szTag, 0)))[0];

    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static unsigned xtTag = xStruct->Desc()->TagIndex( "TYPE");
    static unsigned ytTag = yStruct->Desc()->TagIndex( "TYPE");
    static unsigned ztTag = zStruct->Desc()->TagIndex( "TYPE");
    DLong xt = (*static_cast<DLongGDL*>( xStruct->GetTag( xtTag, 0)))[0];
    DLong yt = (*static_cast<DLongGDL*>( yStruct->GetTag( ytTag, 0)))[0];

    DLong xv=1, yv=1;
    int xSize, ySize, xPos, yPos;
    // Use Size in lieu of VSize
    Graphics* actDevice = Graphics::GetDevice();
    DLong wIx = actDevice->ActWin();
    if( wIx == -1) {
      DStructGDL* dStruct = SysVar::D();
      static unsigned xsizeTag = dStruct->Desc()->TagIndex( "X_SIZE");
      static unsigned ysizeTag = dStruct->Desc()->TagIndex( "Y_SIZE");
      xSize = (*static_cast<DLongGDL*>( dStruct->GetTag( xsizeTag, 0)))[0];
      ySize = (*static_cast<DLongGDL*>( dStruct->GetTag( ysizeTag, 0)))[0];
    } else {
      bool success = actDevice->WSize(wIx, &xSize, &ySize, &xPos, &yPos);
    }
    if ( e->KeywordSet("DEVICE") || e->KeywordSet("TO_DEVICE")) {
      xv = xSize;
      yv = ySize;
    }

    /*
    static xVSTag = dSysVarDesc->TagIndex( "X_VSIZE");
    static yVSTag = dSysVarDesc->TagIndex( "Y_VSIZE");

    DLong xv = (*static_cast<DLongGDL*>( dStruct->GetTag( xvTag, 0)))[0];
    DLong yv = (*static_cast<DLongGDL*>( dStruct->GetTag( yvTag, 0)))[0];
    */

    if (p0->Type() == DOUBLE || e->KeywordSet("DOUBLE")) {
      return convert_coord_template<DDoubleGDL, DDouble>
	( e, p0, p1, p2, sx, sy, sz, xv, yv, xt, yt);
    } else {
      return convert_coord_template<DFloatGDL, DFloat>
	( e, p0, p1, p2, sx, sy, sz, xv, yv, xt, yt);
    }
  }


} // namespace

