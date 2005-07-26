/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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
//#include "dstructgdl.hpp"

#include "graphics.hpp"
#include "plotting.hpp"

namespace lib {

  using namespace std;

  void device( EnvT* e)
  {
    // CLOSE for z-buffer device
    static int closeFileIx = e->KeywordIx( "CLOSE_FILE"); 
    static int fileNameIx = e->KeywordIx( "FILENAME"); 
    static int decomposedIx = e->KeywordIx( "DECOMPOSED"); 
    static int get_decomposedIx = e->KeywordIx( "GET_DECOMPOSED"); 
    static int z_bufferingIx = e->KeywordIx( "Z_BUFFERING"); 
    static int set_resolutionIx = e->KeywordIx( "SET_RESOLUTION"); 
    //    static int landscapeIx = e->KeywordIx( "LANDSCAPE"); 
    //    static int portraitIx = e->KeywordIx( "PORTRAIT");
 
    Graphics* actDevice = Graphics::GetDevice();

    if( e->KeywordSet( closeFileIx))
      {
	bool success = actDevice->CloseFile();
	if( !success)
	  e->Throw( "Current device does not support "
		    "keyword CLOSE_FILE.");
      }

    BaseGDL* z_buffering = e->GetKW( z_bufferingIx);
    if( z_buffering != NULL)
      {
	bool success = actDevice->ZBuffering( e->KeywordSet( z_bufferingIx));
	if( !success)
	  e->Throw( "Current device does not support "
		    "keyword Z_BUFFERING.");
      }

    BaseGDL* set_resolution = e->GetKW( set_resolutionIx);
    if( set_resolution != NULL)
      {
	DLongGDL* resolution = e->GetKWAs<DLongGDL>( set_resolutionIx);
	if( resolution->N_Elements() != 2)
	  e->Throw( "Keyword array parameter SET_RESOLUTION must have "
		    "2 elements.");
	DLong x = (*resolution)[0];
	DLong y = (*resolution)[1];

	if( x<0 || y<0)
	  e->Throw( "Value of Resolution is out of allowed range.");

	bool success = actDevice->SetResolution( x, y);
	if( !success)
	  e->Throw( "Current device does not support "
		    "keyword SET_RESOLUTION.");
      }


    BaseGDL* decomposed = e->GetKW( decomposedIx);
    if( decomposed != NULL)
	{
	  bool success = actDevice->Decomposed( e->KeywordSet( decomposedIx));
	  if( !success)
	    e->Throw( "Current device does not support "
		      "keyword DECOMPOSED.");
 	}


    if( e->KeywordPresent( get_decomposedIx)) {
      DLong value = actDevice->GetDecomposed();
      if(value == -1)
	e->Throw( "Current device does not support "
		  "keyword GET_DECOMPOSED.");
      else {
	e->SetKW( get_decomposedIx, new DLongGDL( value));
      }
    }


    BaseGDL* fileName = e->GetKW( fileNameIx);
    if( fileName != NULL)
      {
	DString fName;
	e->AssureStringScalarKW( fileNameIx, fName);
	if( fName == "")
	  e->Throw( "Null filename not allowed.");
	bool success = actDevice->SetFileName( fName);
	if( !success)
	  e->Throw( "Current device does not support "
		    "keyword FILENAME.");
      }
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
  }

  void window( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      e->Throw( "Routine is not defined for current "
		"graphics device.");

    SizeT nParam=e->NParam();

    DLong wIx = 0;
    if( e->KeywordSet( 1)) // FREE
      {
	wIx = actDevice->WAdd();
	if( wIx == -1)
	  e->Throw( "No more window handles left.");
      }
    else
      {
	if( nParam == 1)
	  {
	    e->AssureLongScalarPar( 0, wIx);
	    if( wIx < 0 || wIx >= maxWin)
	      e->Throw( "Window number "+i2s(wIx)+
			" out of range.");
	  }
      }

    DString title;
    if( e->KeywordPresent( 4)) // TITLE
      {
	e->AssureScalarKW< DStringGDL>( 4, title);
      }
    else
      {
	title = "GDL "+i2s( wIx);
      }

    DLong xPos = 0;
    if( e->KeywordPresent( 5)) // XPOS
	e->AssureLongScalarKW( 5, xPos);
    DLong yPos = 0;
    if( e->KeywordPresent( 6)) // YPOS
	e->AssureLongScalarKW( 6, yPos);

    DLong xSize = 640;
    e->AssureLongScalarKWIfPresent( "XSIZE", xSize);
    DLong ySize = 512;
    e->AssureLongScalarKWIfPresent( "YSIZE", ySize);

    if( xSize <= 0 || ySize <= 0 || xPos < 0 || yPos < 0)
      e->Throw(  "Unable to create window "
		 "(BadValue (integer parameter out of range for "
		 "operation)).");
    
    bool success = actDevice->WOpen( wIx, title, xSize, ySize, xPos, yPos);
    if( !success)
      e->Throw(  "Unable to create window.");
  }

  void wset( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    DLong wIx = 0;
    if( nParam != 0)
      {
	e->AssureLongScalarPar( 0, wIx);
      }
    if( wIx == -1) wIx = actDevice->ActWin();
    if( wIx == -1) 
      e->Throw( "Window is closed and unavailable.");

    if( wIx == 0)
      {
	if( actDevice->ActWin() == -1)
	  {
	    bool success = actDevice->WOpen( 0, "GDL 0", 640, 512, 0, 0);
	    if( !success)
	      e->Throw( "Unable to create window.");
	    return;
	  }
      }

    bool success = actDevice->WSet( wIx);
    if( !success)
      e->Throw( "Window is closed and unavailable.");
  }

  void wshow( EnvT* e)
  {}

  void wdelete( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    if( nParam == 0)
      {
	int wIx = actDevice->ActWin();
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
			      " out of range or no more windows.");
	return;
      }

    for( SizeT i=0; i<nParam; i++)
      {
	int wIx;
	e->AssureLongScalarPar( i, wIx);
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
		    " out of range or no more windows.");
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
    DLong n = (DLong) floor(log10(x/3.5));
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
    DLong n = (DLong) floor(log10(x/2.82));
    DDouble y = (x / (2.82 * pow(10.,static_cast<double>(n))));
    DLong m;
    if (y >= 1 && y < 2)
      m = 1;
    else if (y >= 2 && y < 4.47)
      m = 2;
    else if (y >= 4.47)
      m = 5;

    PLFLT intv = (PLFLT) (m * pow(10.,static_cast<double>(n)));
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
      (*static_cast<DLongGDL*>( pStruct->Get( backgroundTag, 0)))[0];
    p_noErase = 
      (*static_cast<DLongGDL*>( pStruct->Get( noEraseTag, 0)))[0];
    p_color = 
      (*static_cast<DLongGDL*>( pStruct->Get( colorTag, 0)))[0];
    p_psym = 
      (*static_cast<DLongGDL*>( pStruct->Get( psymTag, 0)))[0];
    p_linestyle = 
      (*static_cast<DLongGDL*>( pStruct->Get( linestyleTag, 0)))[0];
    p_symsize = 
      (*static_cast<DFloatGDL*>( pStruct->Get( symsizeTag, 0)))[0];
    p_charsize = 
      (*static_cast<DFloatGDL*>( pStruct->Get( charsizeTag, 0)))[0];
    p_thick = 
      (*static_cast<DFloatGDL*>( pStruct->Get( thickTag, 0)))[0];
    p_title = 
      (*static_cast<DStringGDL*>( pStruct->Get( titleTag, 0)))[0];
    p_subTitle = 
      (*static_cast<DStringGDL*>( pStruct->Get( subTitleTag, 0)))[0];
    p_ticklen = 
      (*static_cast<DFloatGDL*>( pStruct->Get( ticklenTag, 0)))[0];
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
      (*static_cast<DLongGDL*>( xStruct->Get( styleTag, 0)))[0];
    title = 
      (*static_cast<DStringGDL*>( xStruct->Get( axisTitleTag, 0)))[0];
    charSize = 
      (*static_cast<DFloatGDL*>( xStruct->Get( axischarsizeTag, 0)))[0];
    margin0 = 
      (*static_cast<DFloatGDL*>( xStruct->Get( marginTag, 0)))[0];
    margin1 = 
      (*static_cast<DFloatGDL*>( xStruct->Get( marginTag, 0)))[1];
    ticklen = 
      (*static_cast<DFloatGDL*>( xStruct->Get( ticklenTag, 0)))[0];
  }

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
      
    const float yCharExtension = 1.5;
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
    if( pos != NULL)
      {
	PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
	for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	  position[ i] = (*pos)[ i];
	actStream->vpor(position[0],position[2],position[1],position[3]);
      }
    else
      {
	actStream->vpor( xML, 1.0-xMR, yMB, 1.0-yMT);
      }

    // CLIPPING
    if( clippingD != NULL)
	Clipping( clippingD, xStart, xEnd, minVal, maxVal);

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
	  
    // set world coordinates
    actStream->wind( xStart, xEnd, minVal, maxVal);

    return true;
  }

  void plot( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid=true;
    DDoubleGDL *yVal, *xVal;
    SizeT xEl, yEl;

    if( nParam == 1)
      {
	yVal = e->GetParAs< DDoubleGDL>( 0);
	yEl = yVal->N_Elements();
	xVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	e->Guard( xVal); // delete upon exit
	xEl = yEl;
      }
    else
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
	xEl = xVal->N_Elements();
	yVal = e->GetParAs< DDoubleGDL>( 1);
	yEl = yVal->N_Elements();
      }
    DLong psym;
    bool line;

    // !X, !Y (also used below)

    DLong xStyle=0, yStyle=0; 
    DString xTitle, yTitle; 
    DFloat x_CharSize, y_CharSize; 
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 

    // [XY]STYLE
    gkw_axis_style(e, "X", xStyle);
    gkw_axis_style(e, "Y", yStyle);

    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);

    // AXIS TITLE
    gkw_axis_title(e, "X", xTitle);
    gkw_axis_title(e, "Y", yTitle);
    // MARGIN
    gkw_axis_margin(e, "X",xMarginL, xMarginR);
    gkw_axis_margin(e, "Y",yMarginB, yMarginT);

    // x and y range
    DDouble xStart = xVal->min(); 
    DDouble xEnd   = xVal->max(); 
    DDouble yStart = yVal->min(); 
    DDouble yEnd   = yVal->max(); 

    if ((xStyle & 1) != 1) {
      PLFLT intv;
      intv = AutoIntv(xEnd-xStart);
      xEnd = ceil(xEnd/intv) * intv;
      xStart = floor(xStart/intv) * intv;
    }

    if ((yStyle & 1) != 1) {
      PLFLT intv;
      intv = AutoIntv(yEnd-yStart);
      yEnd = ceil(yEnd/intv) * intv;
      yStart = floor(yStart/intv) * intv;
    }

    DLong ynozero=0, xnozero=1;
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, xnozero);
    gkw_axis_range(e, "Y", yStart, yEnd, ynozero);
    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    if (xStart > 0 && xnozero == 0) xStart = 0; 
    if (yStart > 0 && ynozero == 0) yStart = 0; 

    if(xEnd == xStart) xEnd=xStart+1;


    DDouble minVal = yStart;
    DDouble maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    DLong xTicks=0, yTicks=0; 
    e->AssureLongScalarKWIfPresent( "XTICKS", xTicks);
    e->AssureLongScalarKWIfPresent( "YTICKS", yTicks);

    DLong xMinor=0, yMinor=0; 
    e->AssureLongScalarKWIfPresent( "XMINOR", xMinor);
    e->AssureLongScalarKWIfPresent( "YMINOR", yMinor);

    DString xTickformat, yTickformat;
    e->AssureStringScalarKWIfPresent( "XTICKFORMAT", xTickformat);
    e->AssureStringScalarKWIfPresent( "YTICKFORMAT", yTickformat);


    bool xLog, yLog;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    if( xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && minVal <= 0.0)
      Warning( "PLOT: Infinite y plot range.");

    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;


    DDouble ticklen = 0.02;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);

    DFloat xTicklen, yTicklen;
    e->AssureFloatScalarKWIfPresent( "XTICKLEN", xTicklen);
    e->AssureFloatScalarKWIfPresent( "YTICKLEN", yTicklen);
    // plsmin (def, scale);

						 
    // POSITION
    PLFLT xScale = 1.0;
    PLFLT yScale = 1.0;

    //    PLFLT scale = 1.0;
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }


    DFloat charsize, xCharSize, yCharSize;
    // *** start drawing
    GDLGStream* actStream = GetPlotStream( e); 
    gkw_background(e, actStream);  //BACKGROUND
    gkw_color(e, actStream);       //COLOR
    gkw_noerase(e, actStream);     //NOERASE
    gkw_psym(e, actStream, line, psym);//PSYM
    gkw_charsize(e, actStream, charsize);    //CHARSIZE
    gkw_axis_charsize(e, "X",xCharSize);//XCHARSIZE
    gkw_axis_charsize(e, "Y",yCharSize);//YCHARSIZE

    // plplot stuff
    // set the charsize (scale factor)
    DDouble charScale = 1.0;
    DLongGDL* pMulti = SysVar::GetPMulti();
    if( (*pMulti)[1] > 2 || (*pMulti)[2] > 2) charScale = 0.5;
    actStream->schr( 0.0, charsize * charScale);

    // get subpage in mm
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT); 
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;

    // get char size in mm (default, actual)
    PLFLT defH, actH;
    actStream->gchr( defH, actH);

    // CLIPPING
    DDoubleGDL* clippingD=NULL;
    DLong noclip=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
      }
    

    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, pos, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt="bc", yOpt="bc";
    if ((xStyle & 4) == 4) xOpt = "";
    if ((xStyle & 8) == 8) xOpt = "b";

    if ((yStyle & 4) == 4) yOpt = "";
    if ((yStyle & 8) == 8) yOpt = "b";

    if (xTicks == 1) xOpt += "t"; else xOpt += "st";
    if (yTicks == 1) yOpt += "tv"; else yOpt += "stv";

    if (xTickformat != "(A1)") xOpt += "n";
    if (yTickformat != "(A1)") yOpt += "n";


    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";


    // axis titles
    actStream->schr( 0.0, actH/defH * xCharSize);
    actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());

    // the axis (separate for x and y axis because of charsize)
    PLFLT xintv;
    if (xTicks == 0) {
      xintv = AutoTick(xEnd-xStart);
    } else {
      xintv = (xEnd - xStart) / xTicks;
    }
    actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);

    actStream->schr( 0.0, actH/defH * yCharSize);
    actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    PLFLT yintv;
    if (yTicks == 0) {
      yintv = AutoTick(yEnd-yStart);
    } else {
      yintv = (yEnd - yStart) / yTicks;
    }
    actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);

    // title and sub title
    gkw_title(e, actStream, actH/defH);

    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    // plot the data
    if(!e->KeywordSet("NODATA"))
      if(valid)
	valid=draw_polyline(e, actStream, 
			    xVal, yVal, xLog, yLog, 
			    yStart, yEnd, psym);

    actStream->lsty(1);//reset linestyle
    actStream->flush();

    // set ![XY].CRANGE
    set_axis_crange("X", xStart, xEnd);
    set_axis_crange("Y", yStart, yEnd);    


    //set ![x|y].type
    set_axis_type("X",xLog);
    set_axis_type("Y",yLog);
  } // plot

  void oplot( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid, line;
    valid=true;
    DLong psym;
    DDoubleGDL* yVal;
    DDoubleGDL* xVal;

    SizeT xEl;
    SizeT yEl;
    if( nParam == 1)
      {
	yVal = e->GetParAs< DDoubleGDL>( 0);
	yEl = yVal->N_Elements();
	
	xVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	e->Guard( xVal); // delete upon exit
	xEl = yEl;
      }
    else
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
	xEl = xVal->N_Elements();

	yVal = e->GetParAs< DDoubleGDL>( 1);
	yEl = yVal->N_Elements();
      }
    DLong minEl = (xEl < yEl)? xEl : yEl;


    // !X, !Y (also used below)
    DString xTitle, yTitle; 
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 

    get_axis_margin("X",xMarginL, xMarginR);
    get_axis_margin("Y",yMarginB, yMarginT);
    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    DDouble minVal;
    DDouble maxVal;
    bool xLog, yLog;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;

    GDLGStream* actStream = GetPlotStream( e); 
    
    // start drawing
    gkw_background(e, actStream,false);
    gkw_color(e, actStream);
    gkw_noerase(e, actStream,true);
    gkw_psym(e, actStream, line, psym);
    DFloat charsize;
    gkw_charsize(e,actStream, charsize, false);


    if( (yStart == yEnd) || (xStart == xEnd))
      {
	if( yStart != 0.0 && yStart == yEnd)
	  Message("OPLOT: !Y.CRANGE ERROR, setting to [0,1]");
	yStart = 0; //yVal->min();
	yEnd   = 1; //yVal->max();
	    
	if(xStart != 0.0 && xStart == xEnd)
	  Message("OPLOT: !X.CRANGE ERROR, resetting range to data");
	xStart = 0; //xVal->min();
	xEnd   = 1; //xVal->max();
	
	set_axis_crange("X", xStart, xEnd);
	set_axis_crange("Y", yStart, yEnd);
      }	    
    
    minVal = yStart;
    maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    // CLIPPING
    DDoubleGDL* clippingD=NULL;
    DLong noclip=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
      }
    

    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, NULL, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
    // pen thickness for axis
    actStream->wid( 0);

    // axis
    //    string xOpt = "bcnst";
    //string yOpt = "bcnstv";

    //    if( xLog) xOpt += "l";
    //if( yLog) yOpt += "l";
    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    // plot the data
    if(valid)
      valid=draw_polyline(e, actStream, 
			  xVal, yVal, xLog, yLog, 
			  yStart, yEnd, psym);

    actStream->lsty(1);//reset linestyle
    actStream->flush();
  }

  void plots( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid, line;
    valid=true;
    DLong psym;
    DDoubleGDL* yVal;
    DDoubleGDL* xVal;
    SizeT xEl, yEl;
    if( nParam == 1)
      {
	yVal = e->GetParAs< DDoubleGDL>( 0);
	yEl = yVal->N_Elements();
	if(yVal->Rank() ==2)
	  {
	    if(yVal->Dim(0) != 2)
	      e->Throw( "When only 1 param, dims must be (2,n) or (3,n)");
	    yVal = e->GetParAs< DDoubleGDL>( 0);
	    yEl=yVal->Dim(1);
	  }
	else if(yVal->Rank()==3)
	  {
	    if(yVal->Dim(0) != 3)
	      e->Throw( "When only 1 param, dims must be (2,n) or (3,n)");
	    yVal = e->GetParAs< DDoubleGDL>( 0);
	    yEl=yVal->Dim(1);
	    e->Throw( "(3,n) not implemented");
	  }
	else
	  {
	    e->Throw( "When only 1 param, dims must be (2,n) or (3,n)");
	  }
      }
    else if(nParam ==2)
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
	xEl = xVal->N_Elements();

	yVal = e->GetParAs< DDoubleGDL>( 1);
	yEl = yVal->N_Elements();
      }
    else 
      {
	e->Throw( "Three dimensional PLOTS not yet implemented.");
      }

    DLong minEl = (xEl < yEl)? xEl : yEl;

    // !X, !Y (also used below)
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 
    get_axis_margin("X",xMarginL, xMarginR);
    get_axis_margin("Y",yMarginB, yMarginT);
    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    bool xLog, yLog;
    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    DDouble minVal, maxVal;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;
    /*    DLong background = p_background;
    static int cix=e->KeywordIx("COLOR");
    BaseGDL* color_arr=e->GetKW(cix);
    DLongGDL* l_color_arr;

    if(color_arr != NULL)
      {
	l_color_arr=static_cast<DLongGDL*>
	  (color_arr->Convert2(LONG, BaseGDL::COPY));
	if(color_arr->N_Elements() < minEl && color_arr->N_Elements() > 1)
	  e->Throw( "Array "+e->GetParString(cix)+
		    " does not have enough elements for COLOR keyword.");
      }
    DLong color = p_color;

    if(color_arr != NULL)  
      if(color_arr->N_Elements() >= 1) 
	  	color=(*l_color_arr)[0];
    */

    GDLGStream* actStream = GetPlotStream( e); 
    
    // start drawing
    //    actStream->Background(background);
    //    actStream->Color(color);
    gkw_background(e, actStream, false);
    gkw_color(e, actStream);

    gkw_psym(e, actStream, line, psym);
    gkw_linestyle(e, actStream);
    gkw_symsize(e, actStream);
    gkw_thick(e, actStream);
    DFloat charsize;
    gkw_charsize(e,actStream, charsize, false);

    // plplot stuff
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT);
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;

    PLFLT xMR, xML, yMB, yMT;
    

    CheckMargin( e, actStream,
		 xMarginL, 
		 xMarginR, 
		 yMarginB, 
		 yMarginT,
		 xMR,
		 xML,
		 yMB,
		 yMT);
    // viewport
    if(e->KeywordSet("DATA") || 
       (!e->KeywordSet("NORMAL") && !e->KeywordSet("DEVICE")))
      {
	if( (yStart == yEnd) || (xStart == xEnd))
	  {
	    if( yStart != 0.0 && yStart == yEnd)
	      Message("PLOTS: !Y.CRANGE ERROR, setting to [0,1]");
	    yStart = 0; //yVal->min();
	    yEnd   = 1; //yVal->max();
	    
	    if(xStart != 0.0 && xStart == xEnd)
	      Message("PLOTS: !X.CRANGE ERROR, resetting range to data");
	    xStart = 0; //xVal->min();
	    xEnd   = 1; //xVal->max();
	    
	    set_axis_crange("X", xStart, xEnd);
	    set_axis_crange("Y", yStart, yEnd);

	  }	    
	
	minVal= yStart-yMB*(yEnd-yStart)/(1-yMT-yMB);
	yEnd  = yEnd  +yMT*(yEnd-yStart)/(1-yMT-yMB);
	yStart= minVal;
	minVal= xStart-xML*(xEnd-xStart)/(1-xML-xMR);
	xEnd  = xEnd  +xMR*(xEnd-xStart)/(1-xML-xMR);
	xStart= minVal;
      } 
    else if(e->KeywordSet("NORMAL"))
      {
	xStart = 0;
	xEnd   = 1;
	yStart = 0;
	yEnd   = 1;
	xLog = false; yLog = false;
	actStream->NoSub();
      }
    else if(e->KeywordSet("DEVICE"))
      {
	PLFLT xpix, ypix;
	PLINT xleng, yleng, xoff, yoff;
	actStream->gpage(xpix, ypix,xleng, yleng, xoff, yoff);
	xStart=0; xEnd=xleng;
	yStart=0; yEnd=yleng;
	xLog = false; yLog = false;
	actStream->NoSub();
      }

    minVal=yStart; maxVal=yEnd;

    //CLIPPING
    DLong noclip=1;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	DDoubleGDL* clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
	if( clippingD != NULL)
	    Clipping( clippingD, xStart, xEnd, minVal, maxVal);
      }

    if( xLog)
      {
	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
      }
    if( yLog)
      {
	if( minVal <= 0.0) minVal = 0.0; else minVal = log10( minVal);
	if( maxVal <= 0.0) return; else maxVal = log10( maxVal);
      }

    // viewport (full (sub-)window 
    actStream->vpor( 0, 1, 0, 1);
    // world coordinates
    actStream->wind( xStart, xEnd, minVal, maxVal);

    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    // plot the data
    if(valid)
      valid=draw_polyline(e, actStream, 
			  xVal, yVal, xLog, yLog, 
			  yStart, yEnd, psym);
    PLFLT x,y, x1, y1;
    

    actStream->lsty(1);//reset linestyle
    actStream->flush();
  }

  void xyouts( EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    DDoubleGDL* yVal, *xVal;
    DStringGDL* strVal;
    SizeT xEl, yEl,strEl;
    if(nParam == 1) 
      {
	//string only... 
	  e->Throw("String only, not implemented");
      }
    else if(nParam == 3)
      {
	xVal = e->GetParAs< DDoubleGDL>(0);
	xEl = xVal->N_Elements();
	yVal = e->GetParAs< DDoubleGDL>(1);
	yEl = yVal->N_Elements();
	strVal=e->GetParAs<DStringGDL>(2);
	strEl=strVal->N_Elements();
      }
    else
      {
	e->Throw("Not enough parameters. Either 1 parameter or 3 "
		 "parameters valid.");
      }
    //ok...
    DLong minEl = (xEl < yEl)? xEl:yEl;
    minEl=(minEl < strEl)? minEl:strEl;
    
    DFloat xMarginL, xMarginR,yMarginB, yMarginT; 
    get_axis_margin("X", xMarginL, xMarginR);
    get_axis_margin("Y", yMarginB, yMarginT);



    
    DDouble xStart, xEnd, yStart, yEnd;
    bool xLog, yLog;
    DDouble minVal, maxVal;

    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);


    /*    DLong background = p_background;
    static int cix=e->KeywordIx("COLOR");
    BaseGDL* color_arr=e->GetKW(cix);
    DLongGDL* l_color_arr;
    if(color_arr != NULL)
      {
	l_color_arr=static_cast<DLongGDL*>
	  (color_arr->Convert2(LONG, BaseGDL::COPY));
	if(color_arr->N_Elements() < minEl && color_arr->N_Elements() > 1)
	  e->Throw( "Array "+e->GetParString(cix)+
		    " does not have enough elements for COLOR keyword.");
      }
    DLong color = p_color;

    if(color_arr != NULL)  
      if(color_arr->N_Elements() >= 1) 
	color=(*l_color_arr)[0];
    */
    GDLGStream* actStream = GetPlotStream( e); 
    
    //start drawing
    //    actStream->Background( background);
    //    actStream->Color( color);
    //    gkw_background(e, actStream);
    gkw_color(e, actStream);

    PLFLT xMR, xML, yMB, yMT;
    CheckMargin( e, actStream,
		 xMarginL, 
		 xMarginR, 
		 yMarginB, 
		 yMarginT,
		 xMR, xML, yMB, yMT);

    // viewport
    if(e->KeywordSet("DATA") || 
       (!e->KeywordSet("NORMAL") && !e->KeywordSet("DEVICE")))
      {
	if((yStart == yEnd) || (xStart == xEnd))
	  {
	    if(yStart != 0.0 and yStart==yEnd)
	      Message("PLOTS: !Y.CRANGE ERROR, setting to [0,1]");
	    yStart=0;//yVal->min();
	    yEnd=1;//yVal->max();
	    
	    if(xStart != 0.0 and xStart==xEnd)
	      Message("PLOTS: !X.CRANGE ERROR, resetting range to data");
	    xStart=0;//xVal->min();
	    xEnd=1;//xVal->max();
	
	    set_axis_crange("X", xStart, xEnd);
	    set_axis_crange("Y", yStart, yEnd);
	    
	  }	    
	
	minVal=yStart-yMB*(yEnd-yStart)/(1-yMT-yMB);
	yEnd=yEnd  +yMT*(yEnd-yStart)/(1-yMT-yMB);
	yStart=minVal;
	minVal=xStart-xML*(xEnd-xStart)/(1-xML-xMR);
	xEnd  =xEnd  +xMR*(xEnd-xStart)/(1-xML-xMR);
	xStart=minVal;
      } 
    else if(e->KeywordSet("NORMAL"))
      {
	xStart=0;
	xEnd=1;
	yStart=0;
	yEnd=1;
	xLog = false; yLog = false;
	actStream->NoSub();
      }
    else if(e->KeywordSet("DEVICE"))
      {
	PLFLT xpix, ypix;
	PLINT xleng, yleng, xoff, yoff;
	actStream->gpage(xpix, ypix,xleng, yleng, xoff, yoff);
	xStart=0; xEnd=xleng;
	yStart=0; yEnd=yleng;
	xLog = false; yLog = false;
	actStream->NoSub();
      }

    minVal=yStart; maxVal=yEnd;

    //CLIPPING
    DLong noclip=1;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	DDoubleGDL* clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
	if( clippingD != NULL)
	    Clipping( clippingD, xStart, xEnd, minVal, maxVal);
      }

    if( xLog)
      {
	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
      }
    if( yLog)
      {
	if( minVal <= 0.0) minVal = 0.0; else minVal = log10( minVal);
	if( maxVal <= 0.0) return; else maxVal = log10( maxVal);
      }

    // viewport (full (sub-)window 
    actStream->vpor( 0, 1, 0, 1);
    // world coordinates
    actStream->wind( xStart, xEnd, minVal, maxVal);

    PLFLT x,y;
    string out;

    //orientation
    static int oix=e->KeywordIx("ORIENTATION");
    BaseGDL* orient=e->GetKW(oix);
    DDoubleGDL* d_orient;
    PLFLT p_orient, p_orient_x, p_orient_y;
    p_orient=0.0;
    p_orient_x=1.0;
    p_orient_y=0.0;

    if(orient != NULL)
      {
	d_orient=static_cast<DDoubleGDL*>
	  (orient->Convert2(DOUBLE, BaseGDL::COPY));
	if(orient->N_Elements() < minEl && orient->N_Elements() > 1)
	  e->Throw( "Array "+e->GetParString(oix)+
		    " does not have enough elements for ORIENTATION keyword.");
	p_orient=(*d_orient)[0];
	  while(p_orient < 0) p_orient+=360.0;
	  while(p_orient > 360.0) p_orient-=360.0;
      }

    p_orient_x=1.0*cos(p_orient*0.0174533);
    p_orient_y=1.0*sin(p_orient*0.0174533);

    //ALIGNMENT
    DDouble alignment = 0.0;
    e->AssureDoubleScalarKWIfPresent( "ALIGNMENT", alignment);

    //CHARSIZE
    DFloat charsize;
    gkw_charsize(e, actStream, charsize);

    if(minEl == 1)
      {
	x=static_cast<PLFLT>((*xVal)[0]);
	y=static_cast<PLFLT>((*yVal)[0]);

	if( yLog) if( y <= 0.0) goto skip; else y = log10( y);
	if( xLog) if( x <= 0.0) goto skip; else x = log10( x);

	out=(*strVal)[0];
	actStream->ptex(x,y,p_orient_x, p_orient_y,alignment,out.c_str());
      }
    else
      {
	for(int i=0; i<minEl;++i)
	  {
	    x=static_cast<PLFLT>((*xVal)[i]);
	    y=static_cast<PLFLT>((*yVal)[i]);

	    if( yLog) if( y <= 0.0) continue; else y = log10( y);
	    if( xLog) if( x <= 0.0) continue; else x = log10( x);

	    if(orient != NULL)
	      if(orient->N_Elements() > 1) 
		{
		  p_orient=(*d_orient)[i];
		  while(p_orient < 0) p_orient+=360.0;
		  while(p_orient > 360.0) p_orient-=360.0;
		  p_orient_x=1.0*cos(p_orient*0.0174533);
		  p_orient_y=1.0*sin(p_orient*0.0174533);
		}

	    /*	    if(color_arr != NULL)  
	      if(color_arr->N_Elements() > 1)
		actStream->Color((*l_color_arr)[i]);
	    */
	    out=(*strVal)[i];
	    actStream->ptex(x,y,p_orient_x, p_orient_y,alignment,out.c_str());
	  }
      }
    
  skip:
    actStream->flush();
  }

  void surface( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 

    DDoubleGDL* zVal;
    DDoubleGDL* yVal;
    DDoubleGDL* xVal;

    SizeT xEl;
    SizeT yEl;
    SizeT zEl;
    if( nParam == 1)
      {
	zVal = e->GetParAs< DDoubleGDL>( 0);
	if(zVal->Rank() != 2)
	  throw GDLException( e->CallingNode(),
			      "SURFACE: Array must have 2 dimensions:"
			      +e->GetParString(0));
	xEl = zVal->Dim(0);
	yEl = zVal->Dim(1);

	xVal = new DDoubleGDL( dimension( xEl), BaseGDL::INDGEN);
	e->Guard( xVal); // delete upon exit
	yVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	e->Guard( yVal); // delete upon exit
      }

    // !P 
    DLong p_background; 
    DLong p_noErase; 
    DLong p_color; 
    DLong p_psym; 
    DLong p_linestyle;
    DFloat p_symsize; 
    DFloat p_charsize; 
    DFloat p_thick; 
    DString p_title; 
    DString p_subTitle; 
    DFloat p_ticklen; 
    
    GetPData( p_background,
	      p_noErase, p_color, p_psym, p_linestyle,
	      p_symsize, p_charsize, p_thick,
	      p_title, p_subTitle, p_ticklen);

    // !X, !Y, !Z (also used below)
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static DStructGDL* zStruct = SysVar::Z();
    DLong xStyle; 
    DLong yStyle; 
    DLong zStyle; 
    DString xTitle; 
    DString yTitle; 
    DString zTitle; 
    DFloat x_CharSize; 
    DFloat y_CharSize; 
    DFloat z_CharSize; 
    DFloat xMarginL; 
    DFloat xMarginR; 
    DFloat yMarginB; 
    DFloat yMarginF; 
    DFloat zMarginB; 
    DFloat zMarginT; 
    DFloat xTicklen;
    DFloat yTicklen;
    DFloat zTicklen;
    GetAxisData( xStruct, xStyle, xTitle, x_CharSize, xMarginL, xMarginR,
		 xTicklen);
    GetAxisData( yStruct, yStyle, yTitle, y_CharSize, yMarginB, yMarginF,
		 yTicklen);
    GetAxisData( zStruct, zStyle, zTitle, z_CharSize, zMarginB, zMarginT,
		 zTicklen);
    
    // [XY]STYLE
    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);
    e->AssureLongScalarKWIfPresent( "ZSTYLE", zStyle);

    // TITLE
    DString title = p_title;
    DString subTitle = p_subTitle;
    e->AssureStringScalarKWIfPresent( "TITLE", title);
    e->AssureStringScalarKWIfPresent( "SUBTITLE", subTitle);

    // AXIS TITLE
    e->AssureStringScalarKWIfPresent( "XTITLE", xTitle);
    e->AssureStringScalarKWIfPresent( "YTITLE", yTitle);
    e->AssureStringScalarKWIfPresent( "ZTITLE", zTitle);

    // MARGIN (in characters)
    static int xMarginEnvIx = e->KeywordIx( "XMARGIN"); 
    static int yMarginEnvIx = e->KeywordIx( "YMARGIN"); 
    static int zMarginEnvIx = e->KeywordIx( "ZMARGIN"); 
    BaseGDL* xMargin = e->GetKW( xMarginEnvIx);
    BaseGDL* yMargin = e->GetKW( yMarginEnvIx);
    BaseGDL* zMargin = e->GetKW( zMarginEnvIx);
    if( xMargin != NULL)
      {
	if( xMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter XMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* xMarginFl = static_cast<DFloatGDL*>
	  ( xMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( xMarginFl);
	xMarginL = (*xMarginFl)[0];
	if( xMarginFl->N_Elements() > 1)
	  xMarginR = (*xMarginFl)[1];
      }
    if( yMargin != NULL)
      {
	if( yMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter YMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* yMarginFl = static_cast<DFloatGDL*>
	  ( yMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( yMarginFl);
	yMarginB = (*yMarginFl)[0];
	if( yMarginFl->N_Elements() > 1)
	  yMarginF = (*yMarginFl)[1];
      }
    if( zMargin != NULL)
      {
	if( zMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter ZMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* zMarginFl = static_cast<DFloatGDL*>
	  ( zMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( zMarginFl);
	zMarginB = (*zMarginFl)[0];
	if( zMarginFl->N_Elements() > 1)
	  zMarginT = (*zMarginFl)[1];
      }

    // x and y and z range
    DDouble xStart = xVal->min(); 
    DDouble xEnd   = xVal->max(); 
    DDouble yStart = yVal->min(); 
    DDouble yEnd   = yVal->max(); 
    DDouble zStart = zVal->min(); 
    DDouble zEnd   = zVal->max(); 


    //[x|y|z]range keyword
    static int zRangeEnvIx = e->KeywordIx("ZRANGE");
    static int yRangeEnvIx = e->KeywordIx("YRANGE");
    static int xRangeEnvIx = e->KeywordIx("XRANGE");
    BaseGDL* xRange = e->GetKW( xRangeEnvIx);
    BaseGDL* yRange = e->GetKW( yRangeEnvIx);
    BaseGDL* zRange = e->GetKW( zRangeEnvIx);
    
    if(xRange != NULL) 
      {
	if(xRange->N_Elements() != 2)
	  e->Throw("Keyword array parameter XRANGE"
		   "must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* xRangeF = static_cast<DFloatGDL*>
	  ( xRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( xRangeF);
	xStart = (*xRangeF)[0];
	xEnd = (*xRangeF)[1];
      }

    if(yRange != NULL)
      {
	if(yRange->N_Elements() != 2)
	  e->Throw("Keyword array parameter YRANGE"
		   "must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* yRangeF = static_cast<DFloatGDL*>
	  ( yRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( yRangeF);
	yStart = (*yRangeF)[0];
	yEnd = (*yRangeF)[1];
      }
    if(zRange != NULL)
      {
	if(zRange->N_Elements() != 2)
	  e->Throw("Keyword array parameter ZRANGE"
		   "must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* zRangeF = static_cast<DFloatGDL*>
	  ( zRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( zRangeF);
	zStart = (*zRangeF)[0];
	zEnd = (*zRangeF)[1];
      }


    if(xEnd == xStart) xEnd=xStart+1;

    DDouble minVal = zStart;
    DDouble maxVal = zEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);


    bool xLog = e->KeywordSet( "XLOG");
    bool yLog = e->KeywordSet( "YLOG");
    bool zLog = e->KeywordSet( "ZLOG");
    if( xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && yStart <= 0.0)
      Warning( "PLOT: Infinite y plot range.");
    if( zLog && zStart <= 0.0)
      Warning( "PLOT: Infinite z plot range.");

    DLong background = p_background;
    e->AssureLongScalarKWIfPresent( "BACKGROUND", background);

    DDouble ticklen = p_ticklen;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);
						 
    DLong color = p_color;

    e->AssureLongScalarKWIfPresent( "COLOR", color);

    DLong noErase = p_noErase;
    if( e->KeywordSet( "NOERASE")) noErase = 1;
    
    // POSITION
    PLFLT xScale = 1.0;
    PLFLT yScale = 1.0;
    //    PLFLT scale = 1.0;
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }

    // CHARSIZE
    DDouble charsize = p_charsize;
    e->AssureDoubleScalarKWIfPresent( "CHARSIZE", charsize);
    if( charsize <= 0.0) charsize = 1.0;
    //    charsize *= scale;

    // AXIS CHARSIZE
    DDouble xCharSize = x_CharSize;
    e->AssureDoubleScalarKWIfPresent( "XCHARSIZE", xCharSize);
    if( xCharSize <= 0.0) xCharSize = 1.0;

    DDouble yCharSize = y_CharSize;
    e->AssureDoubleScalarKWIfPresent( "YCHARSIZE", yCharSize);
    if( yCharSize <= 0.0) yCharSize = 1.0;
    //    yCharSize *= scale;

    DDouble zCharSize = z_CharSize;
    e->AssureDoubleScalarKWIfPresent( "ZCHARSIZE", zCharSize);
    if( zCharSize <= 0.0) zCharSize = 1.0;


    // THICK
    DFloat thick = p_thick;
    e->AssureFloatScalarKWIfPresent( "THICK", thick);

    GDLGStream* actStream = GetPlotStream( e); 
    
    // *** start drawing
    actStream->Background( background);
    actStream->Color( color);

    actStream->NextPlot( !noErase);
    if( !noErase) actStream->Clear();

    // plplot stuff
    // set the charsize (scale factor)
    DDouble charScale = 1.0;
    DLongGDL* pMulti = SysVar::GetPMulti();
    if( (*pMulti)[1] > 2 || (*pMulti)[2] > 2) charScale = 0.5;
    actStream->schr( 0.0, charsize * charScale);

#if 0
    // get subpage in mm
    PLFLT scrXL, scrXR, scrYB, scrYF;
    actStream->gspa( scrXL, scrXR, scrYB, scrYF); 
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYF-scrYB;
#endif

    // get char size in mm (default, actual)
    PLFLT defH, actH;
    actStream->gchr( defH, actH);

    // CLIPPING
    DDoubleGDL* clippingD=NULL;
    DLong noclip=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
      }
    

#if 0
    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, pos, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
#endif
    

    //linestyle
    DLong linestyle = p_linestyle ;
    DLong temp_linestyle=0;
    e->AssureLongScalarKWIfPresent( "LINESTYLE", temp_linestyle);

    if((temp_linestyle > 0) && (temp_linestyle < 9) )
	linestyle=temp_linestyle;
    else if((linestyle > 0) && (linestyle < 9) )
	linestyle=linestyle+1;
    else 
	linestyle=1;

    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt = "bcnst";
    string yOpt = "bcnstv";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";

#if 0    
    // axis titles
    actStream->schr( 0.0, actH/defH * xCharSize);
    actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    actStream->box( xOpt.c_str(), 0.0, 0, "", 0.0, 0);

    actStream->schr( 0.0, actH/defH * yCharSize);
    actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    actStream->box( "", 0.0, 0, yOpt.c_str(), 0.0, 0);
#endif

    // pen thickness for plot
    actStream->wid( static_cast<PLINT>(floor( thick-0.5)));


    // plot the data
    actStream->lsty(linestyle);
    PLFLT** z = new PLFLT*[xEl];
    for( SizeT i=0; i<xEl; i++) z[i] = &(*zVal)[i*yEl];

    actStream->vpor(0.0, 1.0, 0.0, 0.9);
    actStream->wind(-1.0, 1.0, -1.0, 1.5);

    const PLFLT alt[] = {33.0, 17.0};
    const PLFLT az[] = {24.0, 115.0};

    actStream->w3d( 1.0, 1.0, 1.2, 
		    xStart, xEnd, yStart, yEnd, minVal, maxVal,
		    alt[0], az[0] );

    actStream->box3( "bnstu", xTitle.c_str(), 0.0, 0,
		     "bnstu", yTitle.c_str(), 0.0, 0,
		     "bcdmnstuv", zTitle.c_str(), 0.0, 4 );


    actStream->mesh(static_cast<PLFLT*> (&(*xVal)[0]), 
		    static_cast<PLFLT*> (&(*yVal)[0]), 
		    z, (long int) xEl, (long int) yEl, 3);


    // title and sub title
    actStream->schr( 0.0, 1.25*actH/defH);
    actStream->mtex("t",1.25,0.5,0.5,title.c_str());
    actStream->schr( 0.0, actH/defH); // charsize is reset here
    actStream->mtex("b",5.4,0.5,0.5,subTitle.c_str());
    

    delete[] z;

    actStream->lsty(1);//reset linestyle
    actStream->flush();



    // set ![XY].CRANGE
    set_axis_crange("X", xStart, xEnd);
    set_axis_crange("Y", yStart, yEnd);

    //set ![x|y].type
    set_axis_type("X",xLog);
    set_axis_type("Y",yLog);
  } // surface


  void axis( EnvT* e)
  {
    SizeT nParam=e->NParam( 0); 
    bool valid=true;
    DDouble zVal, yVal, xVal;

    if (nParam >= 1) e->AssureDoubleScalarPar( 0, xVal);

    // !X, !Y (also used below)

    DLong xStyle=0, yStyle=0; 
    DString xTitle, yTitle; 
    DFloat x_CharSize, y_CharSize; 
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 
    DFloat xTicklen, yTicklen;

    bool xAxis=false, yAxis=false; 
    static int xaxisIx = e->KeywordIx( "XAXIS");
    static int yaxisIx = e->KeywordIx( "YAXIS");

    if( e->GetKW( xaxisIx) != NULL) xAxis = true;
    if( e->GetKW( yaxisIx) != NULL) yAxis = true;

    // [XY]STYLE
    gkw_axis_style(e, "X", xStyle);
    gkw_axis_style(e, "Y", yStyle);

    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);

    // AXIS TITLE
    gkw_axis_title(e, "X", xTitle);
    gkw_axis_title(e, "Y", yTitle);
    // MARGIN
    gkw_axis_margin(e, "X",xMarginL, xMarginR);
    gkw_axis_margin(e, "Y",yMarginB, yMarginT);


    // x and y range
    DDouble xStart;
    DDouble xEnd;
    DDouble yStart;
    DDouble yEnd;

    if ((xStyle & 1) != 1 && xAxis) {
      PLFLT intv;
      intv = AutoIntv(xEnd-xStart);
      xEnd = ceil(xEnd/intv) * intv;
      xStart = floor(xStart/intv) * intv;
    }

    if ((yStyle & 1) != 1 && yAxis) {
      PLFLT intv;
      intv = AutoIntv(yEnd-yStart);
      yEnd = ceil(yEnd/intv) * intv;
      yStart = floor(yStart/intv) * intv;
    }


    DLong ynozero, xnozero;
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, ynozero);
    gkw_axis_range(e, "Y", yStart, yEnd, xnozero);

    if(xEnd == xStart) xEnd=xStart+1;

    DDouble minVal = yStart;
    DDouble maxVal = yEnd;

    DLong xTicks=0, yTicks=0; 
    e->AssureLongScalarKWIfPresent( "XTICKS", xTicks);
    e->AssureLongScalarKWIfPresent( "YTICKS", yTicks);

    DLong xMinor=0, yMinor=0; 
    e->AssureLongScalarKWIfPresent( "XMINOR", xMinor);
    e->AssureLongScalarKWIfPresent( "YMINOR", yMinor);

    DString xTickformat, yTickformat;
    e->AssureStringScalarKWIfPresent( "XTICKFORMAT", xTickformat);
    e->AssureStringScalarKWIfPresent( "YTICKFORMAT", yTickformat);

    bool xLog, yLog;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    if( xLog && xStart <= 0.0)
      Warning( "AXIS: Infinite x plot range.");
    if( yLog && minVal <= 0.0)
      Warning( "AXIS: Infinite y plot range.");


    DDouble ticklen = 0.02;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);
						 
    DFloat charsize, xCharSize, yCharSize;
    // *** start drawing
    GDLGStream* actStream = GetPlotStream( e); 
    gkw_color(e, actStream);       //COLOR
    gkw_noerase(e, actStream);     //NOERASE
    gkw_charsize(e, actStream, charsize);    //CHARSIZE
    gkw_axis_charsize(e, "X",xCharSize);//XCHARSIZE
    gkw_axis_charsize(e, "Y",yCharSize);//YCHARSIZE

    // plplot stuff
    // set the charsize (scale factor)
    DDouble charScale = 1.0;
    DLongGDL* pMulti = SysVar::GetPMulti();
    if( (*pMulti)[1] > 2 || (*pMulti)[2] > 2) charScale = 0.5;
    actStream->schr( 0.0, charsize * charScale);

    // get subpage in mm
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT); 
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;

    // get char size in mm (default, actual)
    PLFLT defH, actH;
    actStream->gchr( defH, actH);

    /*
    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, pos, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
    */

    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt="bc", yOpt="bc";
    if ((xStyle & 4) == 4) xOpt = "";
    if ((xStyle & 8) == 8) xOpt = "b";

    if ((yStyle & 4) == 4) yOpt = "";
    if ((yStyle & 8) == 8) yOpt = "b";

    if (xTicks == 1) xOpt += "t"; else xOpt += "st";
    if (yTicks == 1) yOpt += "tv"; else yOpt += "stv";

    if (xTickformat != "(A1)") xOpt += "n";
    if (yTickformat != "(A1)") yOpt += "n";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";

    if (xAxis) {

      // axis titles
      actStream->schr( 0.0, actH/defH * xCharSize);
      actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());

      // the axis (separate for x and y axis because of charsize)
      PLFLT xintv;
      if (xTicks == 0) {
	xintv = AutoTick(xEnd-xStart);
      } else {
	xintv = (xEnd - xStart) / xTicks;
      }
      actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);
    }

    if (yAxis) {
      actStream->schr( 0.0, actH/defH * yCharSize);
      actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
      // the axis (separate for x and y axis because of charsize)
      PLFLT yintv;
      if (yTicks == 0) {
	yintv = AutoTick(yEnd-yStart);
      } else {
	yintv = (yEnd - yStart) / yTicks;
      }
      actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);
    }

    // title and sub title
    // axis has subtitle but no title, gkw_title requires both
    //    gkw_title(e, actStream, actH/defH);

    actStream->flush();
  } // axis


  void erase( EnvT* e)
  {
    GDLGStream* actStream = GetPlotStream( e); 
    actStream->Clear();
  }


  //CORE PLOT FUNCTION -> Draws a line along xVal, yVal
  template <typename T> bool draw_polyline(EnvT *e,  GDLGStream *a, T * xVal, T* yVal, 
					   bool xLog, bool yLog, DDouble yStart, DDouble yEnd, DLong psym=0
					   )
  {
    bool line=false;
    bool valid=true;
    DLong _psym=0;

    if(psym <0 ) {line=true; _psym=-psym;}
    else if(psym == 0 ) {line=true;_psym=psym;}
    else {_psym=psym;}
    DLong minEl = (xVal->N_Elements() < yVal->N_Elements())? xVal->N_Elements() : yVal->N_Elements();
    for( int i=0; i<minEl; ++i)
      {
	PLFLT y = static_cast<PLFLT>( (*yVal)[i]);
	if( yLog) if( y <= 0.0) continue; else y = log10( y);
	
	PLFLT x = static_cast<PLFLT>( (*xVal)[i]);
	if( xLog) if( x <= 0.0) continue; else x = log10( x);

	  if( i>0)
	  {
	    if( line)
	      {
		PLFLT y1 = static_cast<PLFLT>( (*yVal)[i-1]);
		
		if( !yLog || y1 > 0.0)
		  {
		    if( yLog) y1 = log10( y1);
		    PLFLT x1 = static_cast<PLFLT>( (*xVal)[i-1]);
		    
		    if( !xLog || x1 > 0.0)
		      {
			if( xLog) x1 = log10( x1);
			a->join(x1,y1,x,y);
		      }
		  }
	      }
	    else if( _psym == 10)
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
			    
			    a->join(x1,y1,(x1+x)/2.0,y1);
			    a->join((x1+x)/2.0,y1,(x1+x)/2.0,y);
			    a->join((x1+x)/2.0,y,x,y);
			  }
		      }
		  }
	      }
	  }
	if( _psym == 0 || _psym == 10) continue;
	
	// translation plplot symbols - GDL symbols
	// for now usersym is a circle
	const PLINT codeArr[]={ 0,2,3,1,11,7,6,5,4};
	
	a->poin(1,&x,&y,codeArr[_psym]);
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
	  (*static_cast<DFloatGDL*>( Struct->Get( marginTag, 0)))[0];
	end = 
	  (*static_cast<DFloatGDL*>( Struct->Get( marginTag, 0)))[1];
      }

    string MarginName=axis+"MARGIN";
    BaseGDL* Margin=e->GetKW(e->KeywordIx(MarginName));
    if(Margin !=NULL)
      {
	if(Margin->N_Elements() > 2)
	  e->Throw("Keyword array parameter "+MarginName+
		   "must have from 1 to 2 elements.");
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
  void gkw_background(EnvT * e, GDLGStream* a, bool kw)
  {
    static DStructGDL* pStruct = SysVar::P();
    DLong background = 
      (*static_cast<DLongGDL*>
       (pStruct->Get
	(pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
    if(kw)
      e->AssureLongScalarKWIfPresent( "BACKGROUND", background);
    a->Background( background);  
  }

  //COLOR
  void gkw_color(EnvT * e, GDLGStream* a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DLong color = 
      (*static_cast<DLongGDL*>( pStruct->Get( pStruct->Desc()->TagIndex("COLOR"), 0)))[0];
    e->AssureLongScalarKWIfPresent( "COLOR", color);
    a->Color( color);  
  }

  //NOERASE
  void gkw_noerase(EnvT* e,GDLGStream * a, bool noe)
  {
    DLong noErase=0;
    static bool evernoErase = false;

    DLongGDL* pMulti = SysVar::GetPMulti();
    if ((*pMulti)[0] == 0) evernoErase = false;

    if(!noe)
      {
	static DStructGDL* pStruct = SysVar::P();
	noErase= (*static_cast<DLongGDL*>( pStruct->Get(pStruct->Desc()->TagIndex("NOERASE"), 0)))[0];
	if(e->KeywordSet("NOERASE")) {
	  noErase=1;
	  evernoErase = true;
	}
      }
    else
      {
	noErase=1;
	evernoErase = true;
      }
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);

    if (pos == NULL) a->NextPlot( !noErase);
    if (pos != NULL) a->NoSub();

    if( !noErase && (*pMulti)[0] != 0 && evernoErase) a->Clear();

//     static DStructGDL* pStruct = SysVar::P();
//     if(pStruct!=NULL)
//       {
// 	static unsigned typeTag = pStruct->Desc()->TagIndex("NOERASE");   
// 	(*static_cast<DLongGDL*>(pStruct->Get(typeTag, 0)))[0] = noErase; 
//       }
  }


  //PSYM
  void gkw_psym(EnvT *e, GDLGStream *a, bool &line, DLong &psym)
  {
    static DStructGDL* pStruct = SysVar::P();
    psym= (*static_cast<DLongGDL*>
	   (pStruct->Get
	    (pStruct->Desc()->TagIndex("PSYM"), 0)))[0];

    line = false;
    e->AssureLongScalarKWIfPresent( "PSYM", psym);
    if( psym > 10 || psym < -8 || psym == 9)
      throw GDLException( e->CallingNode(), 
			  "PSYM (plotting symbol) out of range.");
  }

    //SYMSIZE
  void gkw_symsize(EnvT * e, GDLGStream* a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DFloat symsize = (*static_cast<DFloatGDL*>
		      (pStruct->Get
		       (pStruct->Desc()->TagIndex("SYMSIZE"), 0)))[0];
    e->AssureFloatScalarKWIfPresent( "SYMSIZE", symsize);
    if( symsize <= 0.0) symsize = 1.0;
    a->ssym(0.0, symsize);  
  }

  //CHARSIZE
  void gkw_charsize(EnvT * e, GDLGStream* a, DFloat& charsize, bool kw)
  {
    static DStructGDL* pStruct = SysVar::P();
    charsize = (*static_cast<DFloatGDL*>
			(pStruct->Get
			 ( pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    if(kw)
      e->AssureFloatScalarKWIfPresent( "CHARSIZE", charsize);

    if( charsize <= 0.0) charsize = 1.0;
    a->schr(0.0, charsize);  
  }
  //THICK
  void gkw_thick(EnvT * e, GDLGStream* a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DFloat thick = (*static_cast<DFloatGDL*>
		    (pStruct->Get
		     (pStruct->Desc()->TagIndex("THICK"), 0)))[0];

    e->AssureFloatScalarKWIfPresent( "THICK", thick);
    if( thick <= 0.0) thick = 1.0;
    a->wid( static_cast<PLINT>(floor( thick-0.5)));
  }

  //LINESTYLE
  void gkw_linestyle(EnvT *e, GDLGStream * a)
  {
    static DStructGDL* pStruct = SysVar::P();
    DLong linestyle= 
      (*static_cast<DLongGDL*>
       (pStruct->Get
	(pStruct->Desc()->TagIndex("LINESTYLE"), 0)))[0];
    DLong temp_linestyle=0;
    e->AssureLongScalarKWIfPresent( "LINESTYLE",temp_linestyle);

    if((temp_linestyle > 0) && (temp_linestyle < 9) )
	linestyle=temp_linestyle;
    else if((linestyle > 0) && (linestyle < 9) )
	linestyle=linestyle+1;
    else 
	linestyle=1;

    a->lsty(linestyle);
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
      (*static_cast<DStringGDL*>( pStruct->Get( titleTag, 0)))[0];
    DString subTitle =  
      (*static_cast<DStringGDL*>( pStruct->Get( subTitleTag, 0)))[0];
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
	(*static_cast<DDoubleGDL*>( Struct->Get( crangeTag, 0)))[0] = Start;
	(*static_cast<DDoubleGDL*>( Struct->Get( crangeTag, 0)))[1] = End;
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
	Start = (*static_cast<DDoubleGDL*>( Struct->Get( crangeTag, 0)))[0]; 
	End = (*static_cast<DDoubleGDL*>( Struct->Get( crangeTag, 0)))[1];
      }
  }

   void get_axis_type(string axis,bool &log)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(Struct !=NULL)
      log=((*static_cast<DLongGDL*>(Struct->Get(Struct->Desc()->TagIndex("TYPE"))))[0] ? 1:0);
    else
      log=0;
  }

  //axis type (log..)
  void set_axis_type(string axis, bool Type)
  {
    DStructGDL* Struct=NULL;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(Struct!=NULL)
      {
	static unsigned typeTag = Struct->Desc()->TagIndex("TYPE");   
	(*static_cast<DLongGDL*>(Struct->Get(typeTag, 0)))[0] =Type; 
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
	  (*static_cast<DFloatGDL*>( Struct->Get( charsizeTag, 0)))[0];
      }

    string Charsize_s=axis+"CHARSIZE";
    e->AssureFloatScalarKWIfPresent( Charsize_s, charsize);
    if(charsize <=0.0) charsize=1.0;
  }


  //STYLE
  void gkw_axis_style(EnvT *e, string axis,DLong &style)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    if(Struct != NULL)
      {
	static unsigned styleTag = Struct->Desc()->TagIndex( "STYLE");
	style = 
	  (*static_cast<DLongGDL*>( Struct->Get( styleTag, 0)))[0];
      }

    string StyleName=axis+"STYLE";

  }

  void gkw_axis_title(EnvT *e, string axis,DString &title)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    
    if(Struct != NULL)
      {
	static unsigned titleTag = Struct->Desc()->TagIndex("TITLE");
	title = 
	  (*static_cast<DStringGDL*>( Struct->Get( titleTag, 0)))[0];
      }

    string TitleName=axis+"TITLE";
    e->AssureStringScalarKWIfPresent( TitleName, title);

  }

  //GET RANGE
  void gkw_axis_range(EnvT *e, string axis,DDouble &start, DDouble &end, DLong & ynozero)
  {
    DStructGDL* Struct;
    if(axis=="X") Struct = SysVar::X();
    if(axis=="Y") Struct = SysVar::Y();
    string RangeName=axis+"RANGE";
    BaseGDL* Range=e->GetKW(e->KeywordIx(RangeName));
    if(Range !=NULL)
      {
	if(Range->N_Elements() != 2)
	  e->Throw("Keyword array parameter "+RangeName+
		   "must have 2 elements.");
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
	low = (*static_cast<DFloatGDL*>( Struct->Get( marginTag, 0)))[0]; 
	high = (*static_cast<DFloatGDL*>( Struct->Get( marginTag, 0)))[1];
      }
  }

} // namespace

