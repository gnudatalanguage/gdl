/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
    static int closeFileIx = e->KeywordIx( "CLOSE_FILE"); 
    static int fileNameIx = e->KeywordIx( "FILENAME"); 
    //    static int landscapeIx = e->KeywordIx( "LANDSCAPE"); 
    //    static int portraitIx = e->KeywordIx( "PORTRAIT");
 
    Graphics* actDevice = Graphics::GetDevice();

    if( e->KeywordSet( closeFileIx))
      {
	bool success = actDevice->CloseFile();
	if( !success)
	  throw GDLException( e->CallingNode(),
			      "DEVICE: Current device does not support "
			      "keyword CLOSE_FILE.");
      }

    BaseGDL* fileName = e->GetKW( fileNameIx);
    if( fileName != NULL)
      {
	DString fName;
	e->AssureStringScalarKW( fileNameIx, fName);
	if( fName == "")
	  throw GDLException( e->CallingNode(),
			      "DEVICE: Null filename not allowed.");
	bool success = actDevice->SetFileName( fName);
	if( !success)
	  throw GDLException( e->CallingNode(),
			      "DEVICE: Current device does not support "
			      "keyword FILENAME.");
      }
  }

  void set_plot( EnvT* e) // modifies !D system variable
  {
    SizeT nParam=e->NParam();
    if( nParam < 1)
      throw GDLException( e->CallingNode(),
			  "SET_PLOT: Incorrect number of arguments.");
    DString device;
    e->AssureScalarPar<DStringGDL>( 0, device);

    // this is the device name
    device = StrUpCase( device);
    
    bool success = Graphics::SetDevice( device);
    if( !success)
      throw GDLException( e->CallingNode(),
			  "SET_PLOT: Device not supported/unknown: "+device);
  }

  void window( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      throw GDLException( e->CallingNode(),
			  "WINDOW: Routine is not defined for current "
			  "graphics device.");

    SizeT nParam=e->NParam();

    DLong wIx = 0;
    if( e->KeywordSet( 1)) // FREE
      {
	wIx = actDevice->WAdd();
	if( wIx == -1)
	  throw GDLException( e->CallingNode(),
			      "WINDOW: No more window handles left.");
      }
    else
      {
	if( nParam == 1)
	  {
	    e->AssureLongScalarPar( 0, wIx);
	    if( wIx < 0 || wIx >= maxWin)
	      throw GDLException( e->CallingNode(),
				  "WINDOW: Window number "+i2s(wIx)+
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
      throw GDLException( e->CallingNode(), "WINDOW: Unable to create window "
			  "(BadValue (integer parameter out of range for "
			  "operation)).");
    
    bool success = actDevice->WOpen( wIx, title, xSize, ySize, xPos, yPos);
    if( !success)
      throw GDLException( e->CallingNode(), "WINDOW: Unable to create window.");
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
      throw GDLException( e->CallingNode(),
			  "WSET: Window is closed and unavailable.");

    if( wIx == 0)
      {
	if( actDevice->ActWin() == -1)
	  {
	    bool success = actDevice->WOpen( 0, "GDL 0", 640, 512, 0, 0);
	    if( !success)
	      throw GDLException( e->CallingNode(), 
				  "WSET: Unable to create window.");
	    return;
	  }
      }

    bool success = actDevice->WSet( wIx);
    if( !success)
      throw GDLException( e->CallingNode(), 
			  "WSET: Window is closed and unavailable.");
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
	  throw GDLException( e->CallingNode(),
			      "WINDOW: Window number "+i2s(wIx)+
			      " out of range or no more windows.");
	return;
      }

    for( SizeT i=0; i<nParam; i++)
      {
	int wIx;
	e->AssureLongScalarPar( i, wIx);
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  throw GDLException( e->CallingNode(),
			      "WINDOW: Window number "+i2s(wIx)+
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
	    delete p1; p1 = r;
	    delete p2; p2 = r;
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

  // !P
  void GetPData( DLong& p_background,
		 DLong& p_noErase, DLong& p_color, DLong& p_psym,DLong& p_linestyle,
		 DFloat& p_symsize, DFloat& p_charsize, DFloat& p_thick,
		 DString& p_title, DString& p_subTitle)
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
  }

  // !X, !Y, !Z
  void GetAxisData( DStructGDL* xStruct,
		    DLong& style, DString& title, DFloat& charSize,
		    DFloat& margin0, DFloat& margin1)
  {		    
    static unsigned styleTag = xStruct->Desc()->TagIndex( "STYLE");
    static unsigned marginTag = xStruct->Desc()->TagIndex( "MARGIN");
    static unsigned axisTitleTag = xStruct->Desc()->TagIndex( "TITLE");
    static unsigned axischarsizeTag = xStruct->Desc()->TagIndex( "CHARSIZE");
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
  }


  void plot_core(PLFLT xl, PLFLT yl, 
		 PLFLT xh, PLFLT yh, 
		 PLFLT xmin, PLFLT ymin, 
		 PLFLT xmax, PLFLT ymax,
		 PLFLT mg, GDLGStream* actStream)
  {
    //mg=(ymax-ymin)/(xmax-xmin)      

    /*Possible cases
      m'=(ymax-ymin)/(xmax-xmin)       == mg
      m=(yh-yl)/(xh-xl)
      xmin < xl < xh < xmax && ymin < yl < yh < ymax
      Inside the box
      xmin > xl || ymin > yl
           m > m'
	      under the x axis
	      find the y=0 crossing
	      yln=ymin, xln=ymin + (yl/m)-xl
           m < m'
              left of the y axis
	      find the x=0 crossing
	      xln=xmin, yln = yl-m(xl-xmin)

      xmax < xl || ymax < yl
           m > m'
	      above the x axis
	      find the y=0 crossing
	      yhn=ymax, xhn=ymax + (yh/m)-xh
           m < m'
              right of the y axis
	      find the x=0 crossing
	      xhn=xmax, yhn = yh-m*(xh-xmax)
    */

    PLFLT m=(yh-yl)/(xh-xl);
    PLFLT xlow, ylow, xhigh, yhigh;
    if(xl < xmin || yl < ymin)
      {
	if( m > mg)
	{
	  ylow=ymin;
	  xlow=ymin + (xl-(yl-ymin)/m);
	}
	else 
	{
	  xlow=xmin;
	  ylow=yl-m*(xl-xmin);
	}
	xhigh=xh;
	yhigh=yh;
      }
    else if(xh > xmax || yh > ymax)
      {
	if( m > mg)
	{
	  yhigh=ymax;
	  xhigh=xh-(yh-yhigh)/m;
	}
	else 
	{
	  xhigh=xmax;
	  yhigh=yh-m*(xh-xmax);
	}
	xlow=xl;
	ylow=yl;
      }
    else 
      {
	xhigh=xh;
	xlow=xl;
	yhigh=yh;
	ylow=yl;
      }
    cout<<xlow<<","<<ylow<<","<<xhigh<<","<<yhigh<<endl;
    actStream->join(xlow, ylow, xhigh, yhigh);

  }

  void plot( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam( 1); 

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
    DLong minEl = (xEl > yEl)? xEl : yEl;

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
    
    GetPData( p_background,
	      p_noErase, p_color, p_psym,p_linestyle,
	      p_symsize, p_charsize, p_thick,
	      p_title, p_subTitle);

    // !X, !Y (also used below)
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    DLong xStyle; 
    DLong yStyle; 
    DString xTitle; 
    DString yTitle; 
    DFloat x_CharSize; 
    DFloat y_CharSize; 
    DFloat xMarginL; 
    DFloat xMarginR; 
    DFloat yMarginB; 
    DFloat yMarginT; 
    GetAxisData( xStruct, xStyle, xTitle, x_CharSize, xMarginL, xMarginR);
    GetAxisData( yStruct, yStyle, yTitle, y_CharSize, yMarginB, yMarginT);
    
    // [XY]STYLE
    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);

    // TITLE
    DString title = p_title;
    DString subTitle = p_subTitle;
    e->AssureStringScalarKWIfPresent( "TITLE", title);
    e->AssureStringScalarKWIfPresent( "SUBTITLE", subTitle);

    // AXIS TITLE
    e->AssureStringScalarKWIfPresent( "XTITLE", xTitle);
    e->AssureStringScalarKWIfPresent( "YTITLE", yTitle);

    // MARGIN
    static int xMarginEnvIx = e->KeywordIx( "XMARGIN"); 
    static int yMarginEnvIx = e->KeywordIx( "YMARGIN"); 
    BaseGDL* xMargin = e->GetKW( xMarginEnvIx);
    BaseGDL* yMargin = e->GetKW( yMarginEnvIx);
    if( xMargin != NULL)
      {
	if( xMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter XMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* xMarginF = static_cast<DFloatGDL*>
	  ( xMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( xMarginF);
	xMarginL = (*xMarginF)[0];
	if( xMarginF->N_Elements() > 1)
	  xMarginR = (*xMarginF)[1];
      }
    if( yMargin != NULL)
      {
	if( yMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter YMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* yMarginF = static_cast<DFloatGDL*>
	  ( yMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( yMarginF);
	yMarginB = (*yMarginF)[0];
	if( yMarginF->N_Elements() > 1)
	  yMarginT = (*yMarginF)[1];
      }

    // x and y range
    DDouble xStart = xVal->min(); 
    DDouble xEnd   = xVal->max(); 
    DDouble yStart = yVal->min(); 
    DDouble yEnd   = yVal->max(); 

    //[x|y]range keyword
    static int yRangeEnvIx = e->KeywordIx("YRANGE");
    static int xRangeEnvIx = e->KeywordIx("XRANGE");
    BaseGDL* xRange = e->GetKW( xRangeEnvIx);
    BaseGDL* yRange = e->GetKW( yRangeEnvIx);
    
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
    else 
      {
    if( yStart > 0.0 && !e->KeywordSet( "YNOZERO") && ((yStyle & 16) == 0))  
      yStart = 0.0;
      }

    DDouble minVal = yStart;
    DDouble maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    bool xLog = e->KeywordSet( "XLOG");
    bool yLog = e->KeywordSet( "YLOG");
    if( xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && minVal <= 0.0)
      Warning( "PLOT: Infinite y plot range.");

    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;

    DLong background = p_background;
    e->AssureLongScalarKWIfPresent( "BACKGROUND", background);

    DLong color = p_color;
    e->AssureLongScalarKWIfPresent( "COLOR", color);

    DLong noErase = p_noErase;
    if( e->KeywordSet( "NOERASE")) noErase = 1;
    
    // PSYM
    bool line = false;
    DLong psym = p_psym;
    e->AssureLongScalarKWIfPresent( "PSYM", psym);
    if( psym > 10 || psym < -8 || psym == 9)
      throw GDLException( e->CallingNode(), 
			  "PLOT: PSYM (plotting symbol) out of range.");
    if( psym <= 0)
      {
	line = true;
	psym = -psym;
      }

    // SYMSIZE
    DDouble symsize = p_symsize;
    e->AssureDoubleScalarKWIfPresent( "SYMSIZE", symsize);
    if( symsize < 0.0) symsize = -symsize;
    if( symsize == 0.0) symsize = 1.0;
    
    // CHARSIZE
    DDouble charsize = p_charsize;
    e->AssureDoubleScalarKWIfPresent( "CHARSIZE", charsize);
    if( charsize <= 0.0) charsize = 1.0;

    // AXIS CHARSIZE
    DDouble xCharSize = x_CharSize;
    e->AssureDoubleScalarKWIfPresent( "XCHARSIZE", xCharSize);
    if( xCharSize <= 0.0) xCharSize = 1.0;
    DDouble yCharSize = y_CharSize;
    e->AssureDoubleScalarKWIfPresent( "YCHARSIZE", yCharSize);
    if( yCharSize <= 0.0) yCharSize = 1.0;

    // THICK
    DDouble thick = p_thick;
    e->AssureDoubleScalarKWIfPresent( "THICK", thick);

    GDLGStream* actStream = actDevice->GetStream();
    if( actStream == NULL)
      {
	DString title = "GDL 0";
	bool success = actDevice->WOpen( 0, title, 640, 512, 0, 0);
	if( !success)
	  throw GDLException( e->CallingNode(), 
			      "PLOT: Unable to create window.");
	actStream = actDevice->GetStream();
	if( actStream == NULL)
	  {
	    cerr << "PLOT: Internal error: plstream not set." << endl;
	    exit( EXIT_FAILURE);
	  }
      }
    
    // start drawing

    actStream->Background( background);
    actStream->Color( color);

    actStream->NextPlot( !noErase);
    if( !noErase) actStream->Clear();
    
    // plplot stuff
    // set the charsize
    actStream->schr( 0.0, charsize);

    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT);
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;
    PLFLT defH, actH;
    actStream->gchr( defH, actH);

    PLFLT xML = xMarginL * actH / scrX;
    PLFLT xMR = xMarginR * actH / scrX;

    const float yCharExtension = 1.5;
    PLFLT yMB = yMarginB * actH / scrY * yCharExtension;
    PLFLT yMT = yMarginT * actH / scrY * yCharExtension;

    if( xML+xMR >= 1.0)
      throw GDLException( e->CallingNode(), 
			  "PLOT: XMARGIN to large.");
    if( yMB+yMT >= 1.0)
      throw GDLException( e->CallingNode(), 
			  "PLOT: YMARGIN to large.");

    // viewport
    actStream->vpor( xML, 1.0-xMR, yMB, 1.0-yMT);

    // world coordinates for viewport
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
    
    PLFLT mg=(maxVal-minVal)/(xEnd-xStart);
    actStream->wind( xStart, xEnd, minVal, maxVal);
    //linestyle
    DLong linestyle = p_linestyle ;
    DLong temp_linestyle=0;
    e->AssureLongScalarKWIfPresent( "LINESTYLE", temp_linestyle);

    if((temp_linestyle > 0) && (temp_linestyle < 9) )
      {
	linestyle=temp_linestyle;
      }
    else if((linestyle > 0) && (linestyle < 9) )
      {
	linestyle=linestyle+1;
      }
    else 
      {
	linestyle=1;
      }


    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt = "bcnst";
    string yOpt = "bcnstv";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";
    
    // axis titles
    actStream->schr( 0.0, actH/defH * xCharSize);
    actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    actStream->box( xOpt.c_str(), 0.0, 0, "", 0.0, 0);

    actStream->schr( 0.0, actH/defH * yCharSize);
    actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    actStream->box( "", 0.0, 0, yOpt.c_str(), 0.0, 0);
    
    // title and sub title
    actStream->schr( 0.0, 1.25*actH/defH);
    actStream->mtex("t",1.25,0.5,0.5,title.c_str());
    actStream->schr( 0.0, actH/defH); // charsize is reset here
    actStream->mtex("b",5.4,0.5,0.5,subTitle.c_str());

    // pen thickness for plot
    actStream->wid( static_cast<PLINT>(floor( thick-0.5)));

    // symbol size
    actStream->ssym( 0.0, symsize);

    // plot the data
    actStream->lsty(linestyle);
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
			plot_core(x1,y1,x,y,
				  xStart,minVal,xEnd,maxVal,
				  mg,actStream);
		      }
		  }
	      }
	    else if( psym == 10)
	      {	// histogram
		PLFLT y1 = static_cast<PLFLT>( (*yVal)[i-1]);

		if( !yLog || y1 > 0.0)
		  {
		    if( yLog) y1 = log10( y1);
		    if( y1 >= minVal && y1 <= maxVal)
		      {
			PLFLT x1 = static_cast<PLFLT>( (*xVal)[i-1]);
			
			if( !xLog || x1 > 0.0)
			  {
			    if( xLog) x1 = log10( x1);
			    
			    actStream->join(x1,y1,(x1+x)/2.0,y1);
			    actStream->join((x1+x)/2.0,y1,(x1+x)/2.0,y);
			    actStream->join((x1+x)/2.0,y,x,y);
			  }
		      }
		  }
	      }
	      }
	if( psym == 0 || psym == 10) continue;

	// translation plplot symbols - GDL symbols
	// for now usersym is a circle
	const PLINT codeArr[]={ 0,2,3,1,11,7,6,5,4};
	
	actStream->poin(1,&x,&y,codeArr[psym]);
      }
    actStream->lsty(1);//reset linestyle
    actStream->flush();
    
    // set ![XY].CRANGE
    static unsigned crangeTag = xStruct->Desc()->TagIndex( "CRANGE");
    (*static_cast<DDoubleGDL*>( xStruct->Get( crangeTag, 0)))[0] = xStart;
    (*static_cast<DDoubleGDL*>( xStruct->Get( crangeTag, 0)))[1] = xEnd;
    (*static_cast<DDoubleGDL*>( yStruct->Get( crangeTag, 0)))[0] = minVal;
    (*static_cast<DDoubleGDL*>( yStruct->Get( crangeTag, 0)))[1] = maxVal;

    //set ![x|y].type
    static unsigned xtypeTag = xStruct->Desc()->TagIndex("TYPE");
    static unsigned ytypeTag = yStruct->Desc()->TagIndex("TYPE");
    (*static_cast<DLongGDL*>(xStruct->Get(xtypeTag, 0)))[0] =xLog;
    (*static_cast<DLongGDL*>(yStruct->Get(xtypeTag, 0)))[0] = yLog;

  }

  void oplot( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam( 1); 

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
    DLong minEl = (xEl > yEl)? xEl : yEl;
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
    GetPData( p_background,
	      p_noErase, p_color, p_psym,p_linestyle,
	      p_symsize, p_charsize, p_thick,
	      p_title, p_subTitle);

    // !X, !Y (also used below)
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    DLong xStyle; 
    DLong yStyle; 
    DString xTitle; 
    DString yTitle; 
    DFloat x_CharSize; 
    DFloat y_CharSize; 
    DFloat xMarginL; 
    DFloat xMarginR; 
    DFloat yMarginB; 
    DFloat yMarginT; 
    GetAxisData( xStruct, xStyle, xTitle, x_CharSize, xMarginL, xMarginR);
    GetAxisData( yStruct, yStyle, yTitle, y_CharSize, yMarginB, yMarginT);
    
    // get ![XY].CRANGE
    static unsigned crangeTag = xStruct->Desc()->TagIndex( "CRANGE");
    DDouble xStart = 
      (*static_cast<DDoubleGDL*>( xStruct->Get( crangeTag, 0)))[0];
    DDouble xEnd = 
      (*static_cast<DDoubleGDL*>( xStruct->Get( crangeTag, 0)))[1];
    DDouble yStart =
      (*static_cast<DDoubleGDL*>( yStruct->Get( crangeTag, 0)))[0];
    DDouble yEnd =
    (*static_cast<DDoubleGDL*>( yStruct->Get( crangeTag, 0)))[1];

    DDouble minVal;
    DDouble maxVal;


    static unsigned xtypeTag = xStruct->Desc()->TagIndex("TYPE");
    static unsigned ytypeTag = yStruct->Desc()->TagIndex("TYPE");
    bool xLog =  
      (*static_cast<DLongGDL*>(xStruct->Get(xtypeTag, 0)))[0] ? 1:0;

    bool yLog =  
      (*static_cast<DLongGDL*>(yStruct->Get(xtypeTag, 0)))[0] ? 1:0;
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;

    DLong background = p_background;

    DLong color = p_color;
    e->AssureLongScalarKWIfPresent( "COLOR", color);

    DLong noErase = 1;
    // PSYM
    bool line = false;
    DLong psym = p_psym;
    e->AssureLongScalarKWIfPresent( "PSYM", psym);
    if( psym > 10 || psym < -8 || psym == 9)
      throw GDLException( e->CallingNode(), 
			  "OPLOT: PSYM (plotting symbol) out of range.");
    if( psym <= 0)
      {
	line = true;
	psym = -psym;
      }

    //linestyle
    DLong linestyle = p_linestyle ;
    DLong temp_linestyle = 0; 
    e->AssureLongScalarKWIfPresent( "LINESTYLE", linestyle);
    if((temp_linestyle > 0) && (temp_linestyle < 9) )
      {
	linestyle=temp_linestyle;
      }
    else if((linestyle > 0) && (linestyle < 9) )
      {
	linestyle=linestyle+1;
      }
    else 
      {
	linestyle=1;
      }

    // SYMSIZE
    DDouble symsize = p_symsize;
    e->AssureDoubleScalarKWIfPresent( "SYMSIZE", symsize);
    if( symsize < 0.0) symsize = -symsize;
    if( symsize == 0.0) symsize = 1.0;
    // THICK
    DDouble thick = p_thick;
    e->AssureDoubleScalarKWIfPresent( "THICK", thick);

    GDLGStream* actStream = actDevice->GetStream();
    if( actStream == NULL)
      {
	DString title = "GDL 0";
	bool success = actDevice->WOpen( 0, title, 640, 512, 0, 0);
	if( !success)
	  throw GDLException( e->CallingNode(), 
			      "OPLOT: Unable to create window.");
	actStream = actDevice->GetStream();
	if( actStream == NULL)
	  {
	    cerr << "OPLOT: Internal error: plstream not set." << endl;
	    exit( EXIT_FAILURE);
	  }
      }
    
    // start drawing
    actStream->Background( background);
    actStream->Color( color);

    // plplot stuff
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT);
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;
    PLFLT defH, actH;
    actStream->gchr( defH, actH);
    PLFLT xML = xMarginL * actH / scrX;
    PLFLT xMR = xMarginR * actH / scrX;

    const float yCharExtension = 1.5;
    PLFLT yMB = yMarginB * actH / scrY * yCharExtension;
    PLFLT yMT = yMarginT * actH / scrY * yCharExtension;
    if( xML+xMR >= 1.0)
      throw GDLException( e->CallingNode(), 
			  "OPLOT: XMARGIN to large.");
    if( yMB+yMT >= 1.0)
      throw GDLException( e->CallingNode(), 
			  "OPLOT: YMARGIN to large.");

    // viewport

    if((yStart == yEnd) || (xStart == xEnd))
      {
	if(yStart != 0.0 and yStart==yEnd)
	  Message("OPLOT !y.CRANGE ERROR, setting to [0,1]");
	yStart=0;//yVal->min();
	yEnd=1;//yVal->max();

	if(xStart != 0.0 and xStart==xEnd)
	  Message("OPLOT !x.CRANGE ERROR, resetting range to data");
	xStart=0;//xVal->min();
	xEnd=1;//xVal->max();
	
	(*static_cast<DDoubleGDL*>( xStruct->Get( crangeTag, 0)))[0] = xStart;
	(*static_cast<DDoubleGDL*>( xStruct->Get( crangeTag, 0)))[1] = xEnd;
	    
	(*static_cast<DDoubleGDL*>( yStruct->Get( crangeTag, 0)))[0] = yStart;
	(*static_cast<DDoubleGDL*>( yStruct->Get( crangeTag, 0)))[1] = yEnd;

	actStream->vpor(0,1,0,1);
      }
    else 
      {
	actStream->vpor( xML, 1.0-xMR, yMB, 1.0-yMT);
      }

    minVal = yStart;
    maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);


    // world coordinates for viewport
    /*    if( xLog)
      {
	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
      }
    if( yLog)
      {
	if( minVal <= 0.0) minVal = 0.0; else minVal = log10( minVal);
	if( maxVal <= 0.0) return; else maxVal = log10( maxVal);
	}*/
    PLFLT mg=(maxVal-minVal)/(xEnd-xStart);
    actStream->wind( xStart, xEnd, minVal, maxVal);
    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt = "bcnst";
    string yOpt = "bcnstv";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";
    
    // pen thickness for plot
    actStream->wid( static_cast<PLINT>(floor( thick-0.5)));

    // symbol size
    actStream->ssym( 0.0, symsize);

    // plot the data
    actStream->lsty(linestyle);
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
			plot_core(x1,y1, x, y, 
				  xStart, minVal, xEnd, maxVal,
				  mg, actStream);
			
		      }
		  }
	      }
	    else if( psym == 10)
	      {	// histogram
		PLFLT y1 = static_cast<PLFLT>( (*yVal)[i-1]);

		if( !yLog || y1 > 0.0)
		  {
		    if( yLog) y1 = log10( y1);
		    if( y1 >= minVal && y1 <= maxVal)
		      {
			PLFLT x1 = static_cast<PLFLT>( (*xVal)[i-1]);
			
			if( !xLog || x1 > 0.0)
			  {
			    if( xLog) x1 = log10( x1);
			    
			    actStream->join(x1,y1,(x1+x)/2.0,y1);
			    actStream->join((x1+x)/2.0,y1,(x1+x)/2.0,y);
			    actStream->join((x1+x)/2.0,y,x,y);
			  }
		      }
		  }
	      }      
	  }
	if( psym == 0 || psym == 10) continue;

	// translation plplot symbols - GDL symbols
	// for now usersym is a circle
	const PLINT codeArr[]={ 0,2,3,1,11,7,6,5,4};
	
	actStream->poin(1,&x,&y,codeArr[psym]);
      }
    actStream->lsty(1);//reset linestyle
    actStream->flush();
  }

  void plots( EnvT* e)
  {
  }

  void xyouts( EnvT* e)
  {
  }
} // namespace

