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
#include "graphics.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

#define PLK_Escape            0x1B
#define free_mem(a) \
    if (a != NULL) { free((void *) a); a = NULL; }

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


  void device( EnvT* e)
  {
    // CLOSE for z-buffer device
    static int closeFileIx = e->KeywordIx( "CLOSE_FILE"); 
    static int fileNameIx = e->KeywordIx( "FILENAME"); 
    static int decomposedIx = e->KeywordIx( "DECOMPOSED"); 
    static int get_decomposedIx = e->KeywordIx( "GET_DECOMPOSED"); 
    static int z_bufferingIx = e->KeywordIx( "Z_BUFFERING"); 
    static int set_resolutionIx = e->KeywordIx( "SET_RESOLUTION"); 
    static int get_visual_depthIx = e->KeywordIx( "GET_VISUAL_DEPTH");
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

    if (e->KeywordPresent( get_visual_depthIx))
    {
      {
        DStructGDL* dStruct = SysVar::D(); 
        static unsigned nameTag = dStruct->Desc()->TagIndex( "NAME");
        if ((*static_cast<DStringGDL*>( dStruct->GetTag( nameTag, 0)))[0] != "X")
          e->Throw("GET_VISUAL_DEPTH is not supported by current device");
      }
      Display* display = XOpenDisplay(NULL);
      if (display == NULL) 
        e->Throw("Cannot connect to X server");
      int depth = DefaultDepth(display, DefaultScreen(display));
      XCloseDisplay(display);
      e->SetKW( get_visual_depthIx, new DLongGDL( depth));
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

    if (device == "PS" || device == "SVG") {
      static DStructGDL* pStruct = SysVar::P();
      static unsigned noEraseTag = pStruct->Desc()->TagIndex( "NOERASE");
      (*static_cast<DLongGDL*>( pStruct->GetTag( noEraseTag, 0)))[0] = 1;
    }
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
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    DLong wIx = 0;
    if (nParam != 0) e->AssureLongScalarPar( 0, wIx);
    else wIx = actDevice->ActWin();
    bool show = true;
    if (nParam == 2) { 
      DIntGDL *showval = e->GetParAs<DIntGDL>(1);
      show = (*showval)[0] != 0;
    }
    // TODO: iconify
    if (!actDevice->WShow( wIx, show, false)) 
      e->Throw( "Window is closed and unavailable.");
  }

  void wdelete( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam();
    if( nParam == 0)
      {
	DLong wIx = actDevice->ActWin();
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
			      " out of range or no more windows.");
	return;
      }

    for( SizeT i=0; i<nParam; i++)
      {
	DLong wIx;
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
  
  // this code is issued from
  // http://users.actcom.co.il/~choo/lupg/tutorials/xlib-programming/xlib-programming.html#display_info
  // http://wwwcgi.rdg.ac.uk:8081/cgi-bin/cgiwrap/wsi14/poplog/pop11/ref/XHeightMMOfScreen 
  BaseGDL* get_screen_size( EnvT* e)
  //void GetScreenSize( EnvT* e)
  {
    SizeT nParam=e->NParam(); 
    
    if ( nParam > 1) e->Throw( "Incorrect number of arguments.");
    char *TheDisplay=NULL;

    if ( nParam == 1) {
      DString GivenDisplay;
      //Message( "Warning, we don't know exactly how to manage DISPLAY now.");
      // this code was tested successfully on Ubuntu and OSX, through network
      // and also with "strange" like 'localhost:0.0', '0:0' and "/tmp/launch-OfEkVY/:0" (OSX)
      e->AssureStringScalarPar(0, GivenDisplay);
      TheDisplay = new char [GivenDisplay.size()+1];
      strcpy (TheDisplay, GivenDisplay.c_str());
    }

    /* open the connection to the display */
    Display* display;
    display = XOpenDisplay(TheDisplay);
    if (display == NULL)
      e->Throw("Cannot connect to X server");//, TheDisplay.c_str());
    
    int screen_num;
    
    /* these variables will store the size of the screen, in pixels.    */
    int screen_width;
    int screen_height;
   
    /* this variable will be used to store the ID of the root window of our */
    /* screen. Each screen always has a root window that covers the whole   */
    /* screen, and always exists.                                           */
    Window root_window;
    
    /* check the number of the default screen for our X server. */
    screen_num = DefaultScreen(display);
    
    /* find the width of the default screen of our X server, in pixels. */
    screen_width = DisplayWidth(display, screen_num);
   
    /* find the height of the default screen of our X server, in pixels. */
    screen_height = DisplayHeight(display, screen_num);

    int debug=0;
    if (debug ==1) {
      cout << "Nb pixels: " << screen_width << " x " << screen_height << endl;
    }

    static int resolutionIx = e->KeywordIx( "RESOLUTION");
    if( e->KeywordPresent( resolutionIx)) {
      int screen_width_mm;
      int screen_height_mm;
      screen_width_mm = DisplayWidthMM(display, screen_num);
      screen_height_mm = DisplayHeightMM(display, screen_num);
      DDoubleGDL* resolution;
      resolution = new DDoubleGDL(2, BaseGDL::NOZERO);
      (*resolution)[0]=(screen_width_mm/10.)/screen_width;
      (*resolution)[1]=(screen_height_mm/10.)/screen_height;
      e->SetKW(0, resolution);
      if (debug ==1) {
	cout << "screen (mm): " << screen_width_mm << " x " << screen_height_mm << endl;
	cout << "pixels (mm): " << (*resolution)[0] << " x " << (*resolution)[1] << endl;
      }
    }
    
    XCloseDisplay(display);
    DDoubleGDL* res;
    res = new DDoubleGDL(2, BaseGDL::NOZERO);
    (*res)[0]=screen_width;
    (*res)[1]=screen_height;
    return res->Convert2(FLOAT, BaseGDL::CONVERT);
  }


// get cursor from plPlot     AC February 2008
// known limitations : WAIT parameter and similar keywords not fully managed (wait, nowait ...)

  void cursor( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    //cout << actDevice->Name() << endl;
    if (actDevice->Name() != "X") {
      e->Throw( "Routine is not defined for current graphics device.");
    }

    int debug=0;
    SizeT nParam=e->NParam( 1); 

    if ( nParam < 2 || nParam > 3) 
      {
	e->Throw( "Incorrect number of arguments.");
      }

    e->AssureGlobalPar( 0);
    e->AssureGlobalPar( 1);
    
    GDLGStream *plg  = GetPlotStream( e);

    static PLGraphicsIn gin;

    // content of : plGetCursor();
    // [retval, state, keysym, button, string, pX, pY, dX, dY, wX, wY, subwin]
    
    PLINT plplot_level;
    plg->glevel (plplot_level);   
    if (debug) cout << "Plplot_level : " << plplot_level<< endl;
    // when level < 2, we have to read if ![x|y].crange exist
    // if not, we have to build a [0,1]/[0,1] window
    if (plplot_level < 2) {
      plg->NextPlot();

      plg->vpor(0,1,0,1);
      plg->wind(0,1,0,1);

      /* we are not ready for the correct way (rebuilding a window following
	 stored info)

      // we have to read them back from !x.crange and !y.crange
      PLFLT xStart, xEnd, yStart, yEnd;
      get_axis_crange("X", xStart, xEnd);
      get_axis_crange("Y", yStart, yEnd);
      if ((xStart == xEnd ) || ( xEnd ==0.0)) {
	xStart=0.0;
	xEnd=1.0;
      }
      if ((yStart == yEnd ) || ( yEnd ==0.0)) {
	yStart=0.0;
	yEnd=1.0;
      }
      AC_debug("crange", xStart, xEnd, yStart, yEnd);

      bool okVPWC = SetVP_WC( e, plg, NULL, NULL, 0, 0,
			      xStart, xEnd, yStart, yEnd, false, false);
      if( !okVPWC) return;
      */
    }

    DLong wait=1;

    if (nParam == 3) {
      e->AssureLongScalarPar( 2, wait);
    }

    if ((wait == 1) || (wait == 3) || (wait == 4) ||
	e->KeywordSet("WAIT") ||
	e->KeywordSet("DOWN") ||
	e->KeywordSet("UP") ) {
      //cout << "Sorry, this option is currently not *really* managed. Help welcome" << endl;
      // we toggle to "wait == 1" (the mouse can move but we return if mouse is pressed)
      wait=1;
    }

    int mode=0; // just a flag to manage the general case (cursor,x,y)
    
    if ((wait == 0) || e->KeywordSet("NOWAIT")) {
      gin.button=1;
      plg->GetCursor(&gin);
      gin.button=0;
      mode=1;
      wait=0;
    }
    if (wait == 1) {
      while (1) {
	plg->GetCursor(&gin);
	if (gin.button > 0) break;
      }
      mode=1;
    }    
    if ((wait == 2) || e->KeywordSet("CHANGE")) {
      plg->GetCursor(&gin);
      long RefX, RefY;
      RefX=gin.pX;
      RefY=gin.pY;
      if (gin.button == 0) {
	while (1) {
	  plg->GetCursor(&gin);
	  if (abs(RefX-gin.dX) >0 || abs(RefY-gin.dY) >0) break;
	  if (gin.button > 0) break;
	}
      }
      mode=1;
    }

    if (mode == 0) {
      while (1) {
	plg->GetCursor(&gin);
	// TODO: When no Mouse, should be extended later to any key of the keyboard 
	if (gin.keysym == PLK_Escape) break;
	if (gin.button > 0) break;
      }
    }
    
    if (debug) {
      // plg->text();
      cout << "mouse button : " << gin.button << endl;
      cout << "keysym : " << gin.keysym << endl;
      //plg->gra();
      /* if (gin.keysym < 0xFF && isprint(gin.keysym))
	 cout << "wx = " << gin.wX << ", wy = " << gin.wY <<
	 ", dx = " << gin.dX << ",  dy = " << gin.dY <<
	 ",  c = '" << gin.keysym << "'" << endl;
	 plg->gra(); */      
    }

    if (e->KeywordSet("DEVICE")) {
      PLFLT xp, yp;
      PLINT xleng, yleng, xoff, yoff;
      plg->gpage (xp, yp, xleng, yleng, xoff, yoff);

      DLongGDL* xLong;
      DLongGDL* yLong;
      xLong=new DLongGDL(gin.pX);
      yLong=new DLongGDL(yleng-gin.pY);
      
      e->SetPar(0, xLong);
      e->SetPar(1, yLong);
    } else {
      DDoubleGDL* x;
      DDoubleGDL* y;
      if (e->KeywordSet("NORMAL")) {
	x=new DDoubleGDL(gin.dX );
	y=new DDoubleGDL(gin.dY );
      } else { // default (/data)
	// bad info outside the window (following PLPlot meaning)
	// TODO : we can compute that using !x.s and !y.s
#ifdef USE_LIBPROJ4
        bool mapSet = false;
        get_mapset(mapSet);
        if (!mapSet) 
        {
#endif
 	  x = new DDoubleGDL(gin.wX );
	  y = new DDoubleGDL(gin.wY );
#ifdef USE_LIBPROJ4
        } 
        else 
        {
          PROJTYPE* ref = map_init();
          if (ref == NULL) e->Throw("Projection initialization failed.");
          XYTYPE idata;
          idata.x = gin.wX;
          idata.y = gin.wY;
          LPTYPE odata = PJ_INV(idata, ref);
          x = new DDoubleGDL(odata.lam * RAD_TO_DEG);
          y = new DDoubleGDL(odata.phi * RAD_TO_DEG);
        }
#endif
      }
      e->SetPar(0, x);
      e->SetPar(1, y);
    }
    
    // we update the !Mouse structure (4 fields, only 3 managed up to now)
    DStructGDL* Struct= SysVar::Mouse();
    if (Struct!=NULL) {
      static unsigned xMouseTag = Struct->Desc()->TagIndex("X");
      (*static_cast<DLongGDL*>(Struct->GetTag(xMouseTag)))[0] = gin.pX;
      static unsigned yMouseTag = Struct->Desc()->TagIndex("Y");
      (*static_cast<DLongGDL*>(Struct->GetTag(yMouseTag)))[0] = gin.pY;
      static unsigned ButtonMouseTag = Struct->Desc()->TagIndex("BUTTON");
      if (gin.button == 3) gin.button=4; // 4 values only (0,1,2,4)
      (*static_cast<DLongGDL*>(Struct->GetTag(ButtonMouseTag)))[0]=gin.button;
    }
  }
 
  void plot( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid=true;
    DDoubleGDL *yVal, *xVal;
    SizeT xEl, yEl;
    auto_ptr<BaseGDL> xval_guard;

    if( nParam == 1)
      {
	yVal = e->GetParAs< DDoubleGDL>( 0);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
	yEl = yVal->N_Elements();
	xVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	xval_guard.reset( xVal); // delete upon exit
	xEl = yEl;
      }
    else
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
        if (xVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
	xEl = xVal->N_Elements();
	yVal = e->GetParAs< DDoubleGDL>( 1);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(1));
	yEl = yVal->N_Elements();
      }
    DLong minEl = (xEl < yEl)? xEl : yEl;

    if ( e->KeywordSet( "POLAR")) {
       e->Throw( "Sorry, POLAR keyword not ready");
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
    gkw_axis_margin(e, "X", xMarginL, xMarginR);
    gkw_axis_margin(e, "Y", yMarginB, yMarginT);

    // x and y range
    //    DDouble xStart = xVal->min(); 
    //DDouble xEnd   = xVal->max(); 
    //DDouble yStart = yVal->min(); 
    //DDouble yEnd   = yVal->max(); 
    //    cout << xStart << " " << xEnd << endl;
    
    bool debug=false;
    DLong MyXminEl;
    DLong MyXmaxEl;
    //   BaseGDL *x, *y;
    xVal->MinMax( &MyXminEl, &MyXmaxEl, NULL, NULL, true);
    DDouble xStart = (*xVal)[MyXminEl];
    DDouble xEnd = (*xVal)[MyXmaxEl];
    if (debug) { cout << "X indices (min,max) " << MyXminEl << " " << MyXmaxEl << endl;
    cout << "X values (min,max)  " << xStart << " " << xEnd << endl;}

    DLong MyYminEl;
    DLong MyYmaxEl;
    //   BaseGDL *x, *y;
    yVal->MinMax( &MyYminEl, &MyYmaxEl, NULL, NULL, true);
    DDouble yStart = (*yVal)[MyYminEl];
    DDouble yEnd = (*yVal)[MyYmaxEl];
    if (debug) {cout << "Y indices (min,max) " << MyYminEl << " " << MyYmaxEl << endl;
    cout << "Y values (min,max)  " << yStart << " " << yEnd << endl;}

    //    cout << yStart << " " << yEnd << endl;

    DDouble xStartRaw = xStart;
    DDouble yStartRaw = yStart;

//     bool x0Range = (xStart == xEnd);
//     bool y0Range = (yStart == yEnd);

    // PLOT defines the setting but does not read it
    //    get_axis_type("X", xLog);
    //    get_axis_type("Y", yLog);

    // keyword overrides
    bool xLog, yLog;
    static int xLogIx = e->KeywordIx( "XLOG");
    static int yLogIx = e->KeywordIx( "YLOG");
    xLog = e->KeywordSet( xLogIx);
    yLog = e->KeywordSet( yLogIx);

    DLong xnozero=1, ynozero=0;
    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    if ((xStyle & 1) != 1 && xLog == false) {
      PLFLT intv;
      intv = AutoIntvAC(xStart, xEnd, xnozero);
    }
    if ((yStyle & 1) != 1 && yLog == false) {
      PLFLT intv;
      intv = AutoIntvAC(yStart, yEnd, ynozero);
    }

    // Please remember the {X|Y}range overwrite the data range
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, xnozero);
    gkw_axis_range(e, "Y", yStart, yEnd, ynozero);

    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    // AC: should be now useless (done in AutoIntvAC)
    //if( xStart > 0 && xnozero == 0 && xLog == false) xStart = 0; 
    //if( yStart > 0 && ynozero == 0 && yLog == false) yStart = 0; 

    // no more useful
    //    if(xEnd == xStart) xEnd=xStart+1;
    // AC
    //if(yEnd == yStart) yEnd=yStart+1;
     if (debug) {cout << " Y range" << yStart << " e "<< yEnd << endl;}

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

    if( xLog && xStartRaw <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && yStartRaw <= 0.0)
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
    if (pos == NULL) pos = (DFloatGDL*) 0xF;
    /*
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }
    */


    DFloat charsize, xCharSize, yCharSize;
    // *** start drawing
    GDLGStream* actStream = GetPlotStream( e); 
    gkw_background(e, actStream);  //BACKGROUND
    gkw_color(e, actStream);       //COLOR

    gkw_psym(e, actStream, line, psym);//PSYM
    gkw_charsize(e, actStream, charsize);    //CHARSIZE
    gkw_axis_charsize(e, "X",xCharSize);//XCHARSIZE
    gkw_axis_charsize(e, "Y",yCharSize);//YCHARSIZE


    // Turn off map projection processing
    set_mapset(0);

    gkw_noerase(e, actStream);     //NOERASE

    DLong noErase = 0;
    if( e->KeywordSet( "NOERASE")) noErase = 1;
    if( !noErase) actStream->Clear();

    // Get device name
    DStructGDL* dStruct = SysVar::D();
    static unsigned nameTag = dStruct->Desc()->TagIndex( "NAME");
    DString d_name = 
      (*static_cast<DStringGDL*>( dStruct->GetTag( nameTag, 0)))[0];
    // if PS and not noErase (ie, erase) then set !p.noerase=0    
    if ((d_name == "PS" || d_name == "SVG") && !noErase) {
      static DStructGDL* pStruct = SysVar::P();
      static unsigned noEraseTag = pStruct->Desc()->TagIndex( "NOERASE");
      (*static_cast<DLongGDL*>( pStruct->GetTag( noEraseTag, 0)))[0] = 0;
    }

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

    // Get viewpoint parameters and store in WINDOW & S
    PLFLT p_xmin, p_xmax, p_ymin, p_ymax;
    actStream->gvpd (p_xmin, p_xmax, p_ymin, p_ymax);

    DStructGDL* Struct=NULL;
    Struct = SysVar::X();
    static unsigned windowTag = Struct->Desc()->TagIndex( "WINDOW");
    static unsigned sTag = Struct->Desc()->TagIndex( "S");
    if(Struct != NULL) {
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[0] = p_xmin;
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[1] = p_xmax;

      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] = 
	(p_xmin*xEnd - p_xmax*xStart) / (xEnd - xStart);
      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] = 
	(p_xmax - p_xmin) / (xEnd - xStart);
      
    }

    Struct = SysVar::Y();
    if(Struct != NULL) {
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[0] = p_ymin;
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[1] = p_ymax;

      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] = 
	(p_ymin*yEnd - p_ymax*yStart) / (yEnd - yStart);
      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] = 
	(p_ymax - p_ymin) / (yEnd - yStart);
    }


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
    set_axis_crange("Y", minVal, maxVal);    


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
    auto_ptr<BaseGDL> xval_guard;

    if ( e->KeywordSet( "POLAR")) {
       e->Throw( "Soory, POLAR keyword not ready");
      }

    SizeT xEl;
    SizeT yEl;
    if( nParam == 1)
      {
	yVal = e->GetParAs< DDoubleGDL>( 0);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
	yEl = yVal->N_Elements();
	
	xVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	xval_guard.reset( xVal); // delete upon exit
	xEl = yEl;
      }
    else
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
        if (xVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
	xEl = xVal->N_Elements();

	yVal = e->GetParAs< DDoubleGDL>( 1);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(1));
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

    bool xLog;
    bool yLog;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;

    GDLGStream* actStream = GetPlotStream( e); 
    
    // start drawing
    gkw_background(e, actStream,false);
    gkw_color(e, actStream);
    //    gkw_noerase(e, actStream, true);
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
  } // oplot

  // PLOTS
  void plots( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid, line;
    valid=true;
    DLong psym;
    DDoubleGDL* xVal;
    DDoubleGDL* yVal;
    DDoubleGDL* zVal;
    SizeT xEl, yEl, zEl;
    auto_ptr<BaseGDL> xval_guard;
    auto_ptr<BaseGDL> yval_guard;

    if( nParam == 1)
      {
	BaseGDL* p0;
	p0 = e->GetParDefined( 0);  
	if (p0->Dim(0) != 2)
	  e->Throw( "When only 1 param, dims must be (2,n)");

	DDoubleGDL *val = e->GetParAs< DDoubleGDL>( 0);

	xEl = p0->N_Elements() / p0->Dim(0);
	xVal = new DDoubleGDL( dimension( xEl), BaseGDL::NOZERO);
	xval_guard.reset( xVal); // delete upon exit

	yEl = p0->N_Elements() / p0->Dim(0);
	yVal = new DDoubleGDL( dimension( yEl), BaseGDL::NOZERO);
	yval_guard.reset( yVal); // delete upon exit

	for( SizeT i = 0; i < xEl; i++) {
	  (*xVal)[i] = (*val)[2*i];
	  (*yVal)[i] = (*val)[2*i+1];
	}
      }
    else if(nParam == 2)
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
	xEl = xVal->N_Elements();

	yVal = e->GetParAs< DDoubleGDL>( 1);
	yEl = yVal->N_Elements();
      }
    else if(nParam == 3)
      {
	zVal = e->GetParAs< DDoubleGDL>( 2);
	zEl = zVal->N_Elements();

	if ((*zVal)[0] == 0 && zEl == 1) {
	  xVal = e->GetParAs< DDoubleGDL>( 0);
	  xEl = xVal->N_Elements();

	  yVal = e->GetParAs< DDoubleGDL>( 1);
	  yEl = yVal->N_Elements();
	} else {
	  e->Throw( "Three dimensional PLOTS not yet implemented.");
	}
      }

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
    
//     if( xLog)
//       {
// 	xStart = pow(10.0,xStart);
// 	xEnd   = pow(10.0,xEnd);
//       }
//     if( yLog)
//       {
// 	yStart = pow(10.0,yStart);
// 	yEnd   = pow(10.0,yEnd);
//       }
    
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

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)
    LPTYPE idata;
    XYTYPE odata;

    get_mapset(mapSet);

    if ( mapSet) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }
    }
#endif

    // Determine data coordinate limits (if mapSet is true)
    // These are computed from window and scaling axis system
    // variables because map routines change these directly.
    DDouble *sx;
    DDouble *sy;
    DStructGDL* xStruct = SysVar::X();
    DStructGDL* yStruct = SysVar::Y();
    unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    unsigned syTag = yStruct->Desc()->TagIndex( "S");
    sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];
    
    DFloat *wx;
    DFloat *wy;
    unsigned xwindowTag = xStruct->Desc()->TagIndex( "WINDOW");
    unsigned ywindowTag = yStruct->Desc()->TagIndex( "WINDOW");
    wx = &(*static_cast<DFloatGDL*>( xStruct->GetTag( xwindowTag, 0)))[0];
    wy = &(*static_cast<DFloatGDL*>( yStruct->GetTag( ywindowTag, 0)))[0];
    
    xStart = (wx[0] - sx[0]) / sx[1];
    xEnd   = (wx[1] - sx[0]) / sx[1];
    yStart = (wy[0] - sy[0]) / sy[1];
    yEnd   = (wy[1] - sy[0]) / sy[1];


    if(e->KeywordSet("DEVICE")) {
      PLFLT xpix, ypix;
      PLINT xleng, yleng, xoff, yoff;
      actStream->gpage(xpix, ypix,xleng, yleng, xoff, yoff);
      xStart=0; xEnd=xleng;
      yStart=0; yEnd=yleng;
      xLog = false; yLog = false;
      actStream->NoSub();
    } else if(e->KeywordSet("NORMAL")) {
      xStart = 0;
      xEnd   = 1;
      yStart = 0;
      yEnd   = 1;
      actStream->NoSub();
      actStream->vpor(0, 1, 0, 1);
      xLog = false; yLog = false;
    } else {
      actStream->NoSub();
      actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
    }

    minVal=yStart; maxVal=yEnd;

    //CLIPPING
    DLong noclip = 1;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if( noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	DDoubleGDL* clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
	if( clippingD != NULL)
	    Clipping( clippingD, xStart, xEnd, minVal, maxVal);
      }

//    Comment out to fix bug [1560714] JMG 06/09/27
//    if( xLog)
//      {
//	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
//	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
//      }

    if( yLog)
      {
	if( yStart <= 0.0) yStart = 0.0; else yStart = log10( yStart);
	if( yEnd   <= 0.0) return; else yEnd = log10( yEnd);
      }

    actStream->wind( xStart, xEnd, yStart, yEnd);


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
  } // plots

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
    
// pen thickness for axis
actStream->wid( 0);


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

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)
    LPTYPE idata;
    XYTYPE odata;

    get_mapset(mapSet);

    if ( mapSet) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }
    }
#endif


    // Determine data coordinate limits
    // These are computed from window and scaling axis system
    // variables because map routines change these directly.
    DDouble *sx;
    DDouble *sy;
    DStructGDL* xStruct = SysVar::X();
    DStructGDL* yStruct = SysVar::Y();
    unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    unsigned syTag = yStruct->Desc()->TagIndex( "S");
    sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];
    
    DFloat *wx;
    DFloat *wy;
    unsigned xwindowTag = xStruct->Desc()->TagIndex( "WINDOW");
    unsigned ywindowTag = yStruct->Desc()->TagIndex( "WINDOW");
    wx = &(*static_cast<DFloatGDL*>( xStruct->GetTag( xwindowTag, 0)))[0];
    wy = &(*static_cast<DFloatGDL*>( yStruct->GetTag( ywindowTag, 0)))[0];
    
    xStart = (wx[0] - sx[0]) / sx[1];
    xEnd   = (wx[1] - sx[0]) / sx[1];
    yStart = (wy[0] - sy[0]) / sy[1];
    yEnd   = (wy[1] - sy[0]) / sy[1];


    if(e->KeywordSet("DEVICE")) {
      PLFLT xpix, ypix;
      PLINT xleng, yleng, xoff, yoff;
      actStream->gpage(xpix, ypix,xleng, yleng, xoff, yoff);
      xStart=0; xEnd=xleng;
      yStart=0; yEnd=yleng;
      xLog = false; yLog = false;
      actStream->NoSub();
    } else if(e->KeywordSet("NORMAL")) {
      xStart = 0;
      xEnd   = 1;
      yStart = 0;
      yEnd   = 1;
      actStream->NoSub();
      actStream->vpor(0, 1, 0, 1);
      xLog = false; yLog = false;
    } else {
      actStream->NoSub();
      actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
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

    if( yLog)
      {
	if( yStart <= 0.0) yStart = 0.0; else yStart = log10( yStart);
	if( yEnd   <= 0.0) return; else yEnd = log10( yEnd);
      }

    actStream->wind( xStart, xEnd, yStart, yEnd);

    // ranges are already log10
//     if( xLog)
//       {
// 	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
// 	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
//       }
//     if( yLog)
//       {
// 	if( minVal <= 0.0) minVal = 0.0; else minVal = log10( minVal);
// 	if( maxVal <= 0.0) return; else maxVal = log10( maxVal);
//       }

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

#ifdef USE_LIBPROJ4
	if (mapSet && !e->KeywordSet("NORMAL")) {
	  idata.lam = x * DEG_TO_RAD;
	  idata.phi = y * DEG_TO_RAD;
	  odata = PJ_FWD(idata, ref);
	  x = odata.x;
	  y = odata.y;
	}
#endif

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
    //    DDoubleGDL* zValT;
    auto_ptr<BaseGDL> xval_guard;
    auto_ptr<BaseGDL> yval_guard;

    SizeT xEl;
    SizeT yEl;
    SizeT zEl;

    if (nParam == 2 || nParam > 3) {
      e->Throw( "Incorrect number of arguments.");
    }
    
    BaseGDL* p0 = e->GetNumericArrayParDefined( 0)->Transpose( NULL);
    auto_ptr<BaseGDL> p0_guard;
    zVal = static_cast<DDoubleGDL*>
      (p0->Convert2( DOUBLE, BaseGDL::COPY));
    p0_guard.reset( p0); // delete upon exit

    if(zVal->Dim(0) == 1)
      e->Throw( "Array must have 2 dimensions:"
			  +e->GetParString(0));    
    xEl = zVal->Dim(1);
    yEl = zVal->Dim(0);
    if (nParam == 1) {
      xVal = new DDoubleGDL( dimension( xEl), BaseGDL::INDGEN);
      xval_guard.reset( xVal); // delete upon exit
      yVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
      yval_guard.reset( yVal); // delete upon exit
    }

    if (nParam == 3) {
      
      xVal = e->GetParAs< DDoubleGDL>( 1);
      yVal = e->GetParAs< DDoubleGDL>( 2);

      if (xVal->Rank() > 2)
	e->Throw( "X, Y, or Z array dimensions are incompatible.");
      
      if (yVal->Rank() > 2)
	e->Throw( "X, Y, or Z array dimensions are incompatible.");
      
      if (xVal->Rank() == 1) {
	if (xEl != xVal->Dim(0))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
	}
      
      if (yVal->Rank() == 1) {
	if (yEl != yVal->Dim(0))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
      }

      if (xVal->Rank() == 2) {
	if((xVal->Dim(0) != xEl) && (xVal->Dim(1) != yEl))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
      }
      
      if (yVal->Rank() == 2) {
	if((yVal->Dim(0) != xEl) && (yVal->Dim(1) != yEl))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
      }
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
     DDouble xStart;// = xVal->min(); 
     DDouble xEnd;//   = xVal->max(); 
GetMinMaxVal( xVal, &xStart, &xEnd);
     DDouble yStart;// = yVal->min(); 
     DDouble yEnd;//   = yVal->max(); 
GetMinMaxVal( yVal, &yStart, &yEnd);
     DDouble zStart;// = zVal->min(); 
     DDouble zEnd;//   = zVal->max(); 
GetMinMaxVal( zVal, &zStart, &zEnd);

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
		   " must have 2 elements.");
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
		   " must have 2 elements.");
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
		   " must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* zRangeF = static_cast<DFloatGDL*>
	  ( zRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( zRangeF);
	zStart = (*zRangeF)[0];
	zEnd = (*zRangeF)[1];
      }

    // AC is it useful ? Why not for Y ?
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

    DLong noErase = p_noErase;
    if( e->KeywordSet( "NOERASE")) noErase = 1;

    DDouble ticklen = p_ticklen;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);

    // POSITION
    PLFLT xScale = 1.0;
    PLFLT yScale = 1.0;
    //    PLFLT scale = 1.0;
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    /*
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }
    */

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
    gkw_background(e, actStream);  //BACKGROUND   
    gkw_color(e, actStream);       //COLOR

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

    /*
    if((temp_linestyle > 0) && (temp_linestyle < 9) )
	linestyle=temp_linestyle;
    else if((linestyle > 0) && (linestyle < 9) )
	linestyle=linestyle+1;
    else 
	linestyle=1;
    */

    linestyle=temp_linestyle+1;

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


    // 1 DIM X & Y
    if (xVal->Rank() == 1 && yVal->Rank() == 1) {
      
      PLFLT** z;
      actStream->Alloc2dGrid(&z,xEl,yEl);
      for( SizeT ii=0; ii<xEl ; ii++) {
	for( SizeT jj=0; jj<yEl ; jj++) {
	  z[ii][jj] = (*zVal)[ii*yEl+jj];
	}
      }
      actStream->mesh(static_cast<PLFLT*> (&(*xVal)[0]), 
		      static_cast<PLFLT*> (&(*yVal)[0]), 
		      z, (long int) xEl, (long int) yEl, 3);
      //delete[] z;
      free_mem(z);
    }

    // 2 DIM X & Y
    if (xVal->Rank() == 2 && yVal->Rank() == 2) {

      PLFLT** z1 = new PLFLT*[xEl];
      PLFLT* xVec1 = new PLFLT[xEl];
      PLFLT* yVec1 = new PLFLT[xEl];

      for( SizeT j=0; j<yEl; j++) {
	for( SizeT i=0; i<xEl; i++) {
	  z1[i] = &(*zVal)[i*yEl+j];
	  xVec1[i] = (*xVal)[j*xEl+i];
	  yVec1[i] = (*yVal)[j*xEl+i];
	}
	lib::mesh_nr(xVec1, yVec1, z1, (long int) xEl, 1,1);
      }
      delete[] z1;
      delete[] xVec1;
      delete[] yVec1;

      //
      PLFLT** z2 = new PLFLT*[yEl];
      PLFLT* xVec2 = new PLFLT[yEl];
      PLFLT* yVec2 = new PLFLT[yEl];
      
      for( SizeT j=0; j<xEl; j++) {
	for( SizeT i=0; i< yEl; i++) {
	  z2[i] = &(*zVal)[j*yEl+i];       
	  xVec2[i] = (*xVal)[i*xEl+j];
	  yVec2[i] = (*yVal)[i*xEl+j];
	}
	lib::mesh_nr(xVec2, yVec2, z2, 1, (long int) yEl, 2);
      }
      delete[] z2;
      delete[] xVec2;
      delete[] yVec2;
    }

    // title and sub title
    actStream->schr( 0.0, 1.25*actH/defH);
    actStream->mtex("t",1.25,0.5,0.5,title.c_str());
    actStream->schr( 0.0, actH/defH); // charsize is reset here
    actStream->mtex("b",5.4,0.5,0.5,subTitle.c_str());
    
    actStream->lsty(1);//reset linestyle
    actStream->flush();


    // set ![XY].CRANGE
    set_axis_crange("X", xStart, xEnd);
    set_axis_crange("Y", yStart, yEnd);

    //set ![x|y].type
    set_axis_type("X",xLog);
    set_axis_type("Y",yLog);
  } // surface

  struct mypltr_passinfo // {{{
  {
    PLFLT spa[4];
#ifdef USE_LIBPROJ4
    PLFLT sx[2], sy[2];
    LPTYPE* idata;
    XYTYPE* odata; 
    PROJTYPE* ref; 
    DDouble d_nan;
    bool mapSet;
#endif
  }; // }}}

  void mypltr(PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data) // {{{
  {
    PLFLT tr[6]={0.0,0.0,0.0,0.0,0.0,0.0};
    struct mypltr_passinfo *ptr = (mypltr_passinfo* )pltr_data;

    tr[0] = ptr->spa[0];
    tr[4] = ptr->spa[1];
    tr[2] = ptr->spa[2];
    //    tr[5] = ptr->spa[4];
    tr[5] = ptr->spa[3];

    // conversion from array indices to data coord
    x = tr[0] * x + tr[2];
    y = tr[4] * y + tr[5];
    
    // conversion from lon / lat to projected values (in normal coordinates)
#ifdef USE_LIBPROJ4
    if (ptr->mapSet)
    {
      // Convert from lon/lat in degrees to radians
      ptr->idata->lam = x * DEG_TO_RAD;
      ptr->idata->phi = y * DEG_TO_RAD;
      
      // Convert from lon/lat in radians to data coord
      *ptr->odata = PJ_FWD(*ptr->idata, ptr->ref);
      x = ptr->odata->x;
      y = ptr->odata->y;

      // handling inf points (not sure if this is needed?)
      if (!isfinite(x) || !isfinite(y)) x = y = ptr->d_nan;
    }
#endif

    // assignment to pointers passed in arguments
    *tx = x;
    *ty = y;
  } // }}}

  void contour( EnvT* e)
  {
    int debug=0;
    
    SizeT nParam=e->NParam( 1); 
    
    DDoubleGDL* zVal;
    DDoubleGDL* yVal;
    DDoubleGDL* xVal;
    //    DDoubleGDL* zValT;
    auto_ptr<BaseGDL> xval_guard;
    auto_ptr<BaseGDL> yval_guard;
    auto_ptr<BaseGDL> p0_guard;

    SizeT xEl;
    SizeT yEl;
    SizeT zEl;
    if( nParam == 1)
      {
	BaseGDL* p0 = e->GetNumericArrayParDefined( 0)->Transpose( NULL);

	zVal = static_cast<DDoubleGDL*>
	  (p0->Convert2( DOUBLE, BaseGDL::COPY));
	p0_guard.reset( p0); // delete upon exit

	xEl = zVal->Dim(1);
	yEl = zVal->Dim(0);

	if(zVal->Dim(0) == 1)
	  e->Throw(  "Array must have 2 dimensions: "
			      +e->GetParString(0));

	xVal = new DDoubleGDL( dimension( xEl), BaseGDL::INDGEN);
	xval_guard.reset( xVal); // delete upon exit
	yVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	yval_guard.reset( yVal); // delete upon exit
      } else if ( nParam == 2 || nParam > 3) {
	e->Throw( "Incorrect number of arguments.");
      } else {
	BaseGDL* p0 = e->GetNumericArrayParDefined( 0)->Transpose( NULL);
	zVal = static_cast<DDoubleGDL*>
	  (p0->Convert2( DOUBLE, BaseGDL::COPY));
	p0_guard.reset( p0); // delete upon exit

	if(zVal->Dim(0) == 1)
	  e->Throw( "Array must have 2 dimensions: "
			      +e->GetParString(0));

	xVal = e->GetParAs< DDoubleGDL>( 1);
	yVal = e->GetParAs< DDoubleGDL>( 2);

	if (xVal->Rank() > 2)
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");

	if (yVal->Rank() > 2)
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");

	if (xVal->Rank() == 1) {
	  xEl = xVal->Dim(0);

	  if(xEl != zVal->Dim(1))
	    e->Throw( "X, Y, or Z array dimensions are incompatible.");
	}

	if (yVal->Rank() == 1) {
	  yEl = yVal->Dim(0);

	  if(yEl != zVal->Dim(0))
	    e->Throw( "X, Y, or Z array dimensions are incompatible.");
	}

	if (xVal->Rank() == 2) {
	  if((xVal->Dim(0) != zVal->Dim(1)) && (xVal->Dim(1) != zVal->Dim(0)))
	    e->Throw( "X, Y, or Z array dimensions are incompatible.");
	}

	if (yVal->Rank() == 2) {
	  if((yVal->Dim(0) != zVal->Dim(1)) && (yVal->Dim(1) != zVal->Dim(0)))
	    e->Throw( "X, Y, or Z array dimensions are incompatible.");
	}
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
    DDouble xStart;// = xVal->min(); 
    DDouble xEnd;//   = xVal->max(); 
    GetMinMaxVal( xVal, &xStart, &xEnd);
    
    DDouble yStart;// = yVal->min(); 
    DDouble yEnd;//   = yVal->max(); 
    GetMinMaxVal( yVal, &yStart, &yEnd);

    DDouble zStart;// = zVal->min(); 
    DDouble zEnd;//   = zVal->max(); 
    GetMinMaxVal( zVal, &zStart, &zEnd);
    
    if ((xStyle & 1) != 1) {
      PLFLT intv;
      intv = AutoIntvAC(xStart, xEnd, false );
    }

    if ((yStyle & 1) != 1) {
      PLFLT intv;
      intv = AutoIntvAC(yStart, yEnd, false );
    }
    
    if ((zStyle & 1) != 1) {
      PLFLT zintv;
      zintv = AutoIntvAC(zStart, zEnd, false );
    }

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
		   " must have 2 elements.");
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
		   " must have 2 elements.");
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
		   " must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* zRangeF = static_cast<DFloatGDL*>
	  ( zRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( zRangeF);
	zStart = (*zRangeF)[0];
	zEnd = (*zRangeF)[1];
      }

    bool mapSet = false;
#ifdef USE_LIBPROJ4
    get_mapset(mapSet);
#endif

    DDouble minVal = zStart;
    DDouble maxVal = zEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    // AC july 2008 please remember that data sweep out by that
    // are really processed like MISSING data (NaN ...)
    if (minVal > zStart) cout << "This MIN_VALUE is not ready, sorry. Help welcome." <<endl;
    if (maxVal < zEnd) cout << "This MAX_VALUE is not ready, sorry. Help welcome." <<endl;
    //cout << "MIN_VALUE" << minVal << endl;
    //cout << "MAX_VALUE" << maxVal << endl;

    DLong xTicks=0, yTicks=0, zTicks=0;
    e->AssureLongScalarKWIfPresent( "XTICKS", xTicks);
    e->AssureLongScalarKWIfPresent( "YTICKS", yTicks);
    e->AssureLongScalarKWIfPresent( "ZTICKS", zTicks);

    DLong xMinor=0, yMinor=0, zMinor=0; 
    e->AssureLongScalarKWIfPresent( "XMINOR", xMinor);
    e->AssureLongScalarKWIfPresent( "YMINOR", yMinor);
    e->AssureLongScalarKWIfPresent( "ZMINOR", zMinor);

    DString xTickformat, yTickformat, zTickformat;
    e->AssureStringScalarKWIfPresent( "XTICKFORMAT", xTickformat);
    e->AssureStringScalarKWIfPresent( "YTICKFORMAT", yTickformat);
    e->AssureStringScalarKWIfPresent( "ZTICKFORMAT", zTickformat);

    bool xLog = e->KeywordSet( "XLOG");
    bool yLog = e->KeywordSet( "YLOG");
    bool zLog = e->KeywordSet( "ZLOG");
    if( xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && yStart <= 0.0)
      Warning( "PLOT: Infinite y plot range.");
    if( zLog && zStart <= 0.0)
      Warning( "PLOT: Infinite z plot range.");

    DDouble ticklen = p_ticklen;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);
						 
    DLong noErase = p_noErase;
    if( e->KeywordSet( "NOERASE")) noErase = 1;
    
    // POSITION
    PLFLT xScale = 1.0;
    PLFLT yScale = 1.0;
    //    PLFLT scale = 1.0;
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    if (pos == NULL) pos = (DFloatGDL*) 0xF;
    /*
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }
    */

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

   // CHARTHICK (thickness of "char")
    PLINT charthick=1;

    GDLGStream* actStream = GetPlotStream( e); 

    static int overplotKW = e->KeywordIx("OVERPLOT");
    bool overplot = e->KeywordSet( overplotKW);
    
    DDouble *sx, *sy;
    DFloat *wx, *wy;
    unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    unsigned syTag = yStruct->Desc()->TagIndex( "S");
    unsigned xwindowTag = xStruct->Desc()->TagIndex( "WINDOW");
    unsigned ywindowTag = yStruct->Desc()->TagIndex( "WINDOW");
    sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];
    wx = &(*static_cast<DFloatGDL*>( xStruct->GetTag( xwindowTag, 0)))[0];
    wy = &(*static_cast<DFloatGDL*>( yStruct->GetTag( ywindowTag, 0)))[0];

    // mapping only in OVERPLOT mode
    if (!overplot) set_mapset(0);

    if (overplot) 
    {
      //rewrite these quantities
      if (!mapSet) 
      {
        get_axis_crange("X", xStart, xEnd);
        get_axis_crange("Y", yStart, yEnd);
      } 
      else 
      {
        xStart = (wx[0] - sx[0]) / sx[1];
        xEnd   = (wx[1] - sx[0]) / sx[1];
        yStart = (wy[0] - sy[0]) / sy[1];
        yEnd   = (wy[1] - sy[0]) / sy[1];
      }
      get_axis_margin("X",xMarginL, xMarginR);
      get_axis_margin("Y",yMarginB, yMarginF);
      get_axis_type("X", xLog);
      get_axis_type("Y", yLog);
      DFloat charsizeF;
      gkw_charsize(e,actStream, charsizeF, false);
      charsize=charsizeF;
      pos = NULL;
    }

   // *** start drawing
    gkw_background(e, actStream);  //BACKGROUND
    gkw_color(e, actStream);       //COLOR

    if (!overplot) {
      actStream->NextPlot( !noErase);
      if( !noErase) actStream->Clear();
    }

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
    
    if (!overplot || !mapSet)
    {
      // viewport and world coordinates
      bool okVPWC = SetVP_WC( e, actStream, overplot?NULL:pos, clippingD,
                            xLog, yLog,
                            xMarginL, xMarginR, yMarginB, yMarginF,
                            xStart, xEnd, yStart, yEnd);
      if( !okVPWC) return;
    } else {
      // not using SetVP_WC as it seem to always select full window for plotting (FIXME)
      actStream->NoSub();
      actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
      actStream->wind( xStart, xEnd, yStart, yEnd);
    }

    // managing the levels list OR the nlevels value

    PLINT nlevel;
    PLFLT *clevel;
    ArrayGuard<PLFLT> clevel_guard;

    if (debug == 1) {
      cout << "zStart :" << zStart <<", zEnd :" << zEnd << endl;
    }

    // we need to define the NaN value
    static DStructGDL *Values =  SysVar::Values();       
    DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0]; 
    
    static int levelsix = e->KeywordIx( "LEVELS"); 

    BaseGDL* b_levels=e->GetKW(levelsix);
    if(b_levels != NULL) {
      DDoubleGDL* d_levels = e->GetKWAs<DDoubleGDL>( levelsix);
      nlevel = d_levels->N_Elements();
      clevel = (PLFLT *) &(*d_levels)[0];
      // are the levels ordered ?
      for ( SizeT i=1; i<nlevel; i++) {
	if (clevel[i] <= clevel[i-1]) 
	  e->Throw( "Contour levels must be in increasing order.");
      }      
    } else {
      PLFLT zintv;
      // Jo: added keyword NLEVELS
      if (e->KeywordSet( "NLEVELS")) {
      	e->AssureLongScalarKWIfPresent( "NLEVELS", nlevel);
	if (nlevel <= 0) nlevel= 2;  // AC: mimication of IDL

        // cokhavim: IDL does this...
        zintv = (PLFLT) ((zEnd - zStart) / (nlevel+1));
        // ... but I think this is better:
        // if (e->KeywordSet( "FILL")) zintv = (PLFLT) ((zEnd - zStart) / (nlevel));
        // else zintv = (PLFLT) ((zEnd - zStart) / (nlevel+1));

        // SA: this indeed seems better in some cases; however, it makes calls
        //     with and without the /FILL keyword behave differently. As a result,
        //     when overlaing contours over a filled contour, the contours do not match, e.g.:
        //     a=dist(7) & contour,a,/fill,nl=5 & contour,a,/over,/foll,nl=5

      } else {
	zintv = AutoTick(zEnd - zStart);
	nlevel = (PLINT) floor((zEnd - zStart) / zintv);
        // SA: sanity check to prevent segfaults, e.g. with solely non-finite values 
        if (zintv == 0 || nlevel < 0) nlevel = 0; 
      }
      if (debug) cout << "internal Nlevels == " << nlevel << endl;




//       clevel = new PLFLT[nlevel+1];
//       clevel_guard.Reset( clevel);
//       // Jo: fixed clevel to account for non-zero zMin
//       for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = zintv * (i-1) + zStart;
//       //for( SizeT i=0; i<=nlevel; i++) clevel[i] = zintv * i + zStart;
DDouble offset=0.;
if (e->KeywordSet( "FILL")) { nlevel = nlevel + 1; offset=zintv;}
clevel = new PLFLT[nlevel];
clevel_guard.Reset( clevel);
//IDL does this:
// for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = zintv * i + zStart;
//but I think this is better:
for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = zintv * i + zStart - offset;
clevel[nlevel-1]=zEnd; //make this explicit

    }
    // AC would like to check the values (nlevel and values) ...
    if (debug == 1) {
      cout << "Nlevels == " << nlevel << endl;
      for (SizeT i=0; i<=nlevel; i++) cout << i << " " << clevel[i] << endl;
    }
    
//     // Jo: added keyword FILL

    PLINT  &nlevel_fill = nlevel;
    PLFLT* &clevel_fill = clevel;

//     PLFLT *clevel_fill;
//     ArrayGuard<PLFLT> clevel_fill_guard;
//     PLINT nlevel_fill;
//     if (e->KeywordSet( "FILL")) {
//       // To ensure that the highest level is filled, define a new
//       // clevel to include highest value of z:   
//       // modif by AC to manage the exception (nlevel=1)
//       if (nlevel > 1) {
// 	nlevel_fill=nlevel+1;
// 	clevel_fill = new PLFLT[nlevel_fill];
// 	clevel_fill_guard.Reset( clevel_fill);
// 	clevel_fill[nlevel_fill-1] = clevel[nlevel - 1] < zEnd ? zEnd : clevel[nlevel - 1] + 1.;
// 	for( SizeT i=0; i<nlevel; i++) clevel_fill[i] = clevel[i];
//       } else {
// 	nlevel_fill=3;
// 	clevel_fill = new PLFLT[nlevel_fill];
// 	clevel_fill_guard.Reset( clevel_fill);
//         clevel_fill[0] = clevel[0] > zStart ? zStart : clevel[0] - 1.;
//         clevel_fill[1] = clevel[0];
//         clevel_fill[2] = clevel[0] < zEnd ? zEnd : clevel[0] + 1.;
//       }
      
//       if (debug ==1 ) {
// 	cout << "zStart "<< zStart << " zEnd "<< zEnd <<endl ;
// 	for( SizeT i=0; i<nlevel_fill; i++) cout << i << " " << clevel_fill[i] << endl;
//       }
//     }

//     // levels outside limits are changed ...
//     for (SizeT i=0; i<=nlevel; i++) {
//       if (clevel[i] < zStart) clevel[i]=zStart;
//       if (clevel[i] > zEnd) clevel[i]=zEnd;
//     }



    // pen thickness for plot
    actStream->wid( static_cast<PLINT>(floor( thick-0.5)));

    // labeling
    bool label = false;
    if (e->KeywordSet("FOLLOW") || e->KeywordSet("C_CHARSIZE")) label = true;
    // TODO: if (e->KeywordSet("C_ANNOTATION") || e->KeywordSet("C_CHARTHICK") || e->KeywordSet("C_LABELS")) label = true;  
    if (e->KeywordSet("FILL")) label = false;
    if (label)
    { 
      // IDL default: 3/4 of the axis charsize (CHARSIZE keyword or !P.CHARSIZE)
      // PlPlot default: .3
      DFloat label_size = .75 * charsize;
      if (e->KeywordSet("C_CHARSIZE")) e->AssureFloatScalarKWIfPresent("C_CHARSIZE", label_size);
      //usage: setcontlabelparam(PLFLT offset, PLFLT size, PLFLT spacing, PLINT active);
      actStream->setcontlabelparam(0.0, (PLFLT)label_size, .3, true);
    }

#ifdef USE_LIBPROJ4
    static LPTYPE idata;
    static XYTYPE odata;
    static PROJTYPE* ref;
    if (mapSet)
    {
      ref = map_init();
      if ( ref == NULL) e->Throw( "Projection initialization failed.");
    }
#endif

    // starting plotting the data
    struct mypltr_passinfo passinfo;

    // 1 DIM X & Y
    if (xVal->Rank() == 1 && yVal->Rank() == 1) {
      PLFLT spa[4];
      
      // don't forgot we have to use the real limits, not the adjusted ones
      DDouble xMin;// = zVal->min(); 
      DDouble xMax;//   = zVal->max(); 
      GetMinMaxVal( xVal, &xMin, &xMax);
      DDouble yMin;// = zVal->min(); 
      DDouble yMax;//   = zVal->max(); 
      GetMinMaxVal( yVal, &yMin, &yMax);
 
      passinfo.spa[0] = (xMax - xMin) / (xEl - 1);
      passinfo.spa[1] = (yMax - yMin) / (yEl - 1);
      passinfo.spa[2] = xMin;
      passinfo.spa[3] = yMin;

#ifdef USE_LIBPROJ4
      passinfo.mapSet = mapSet;
      if (mapSet) // which imposes overplotting
      {
        passinfo.idata = &idata;
        passinfo.odata = &odata;
        passinfo.ref = ref;
        passinfo.d_nan = d_nan;

        passinfo.sx[0] = sx[0];
        passinfo.sx[1] = sx[1];
        passinfo.sy[0] = sy[0];
        passinfo.sy[1] = sy[1];
      }
#endif

      PLFLT** z = new PLFLT*[xEl];
      for( SizeT i=0; i<xEl; i++) z[i] = &(*zVal)[i*yEl];
      
      // plplot knows how to manage NaN but not Infinity ...
      // we remplace Infinity by Nan
      for( SizeT i=0; i<xEl*yEl; i++) {
	if (isinf((*zVal)[i])) (*z)[i]= d_nan;
      }
      // a draft for MaxVal ...
      //      if (maxVal < zEnd) {
      //;	for( SizeT i=0; i<xEl*yEl; i++) {
      //if ((*zVal)[i] > maxVal) (*z)[i]= d_nan;
      //	}
      //}
      
      //      gkw_linestyle(e, actStream);
      //actStream->lsty(2);
      //
      // AC 18 juin 2007 LineStyle and contour
      // NOT READY NOW
      // here we plot the axis 
      // actStream->cont(z, xEl, yEl, 1, xEl, 1, yEl, 
      //		      clevel, 0, mypltr, static_cast<void*>( spa));
      // we recover the linestyle if a !p.linestyle does exist
      //gkw_linestyle_c(e, actStream, TRUE);


      if (e->KeywordSet( "FILL")) {
	// the "clevel_fill, nlevel_fill" have been computed before
        actStream->shades(z, xEl, yEl, NULL, xStart, xEnd, yStart, yEnd,
 			  clevel_fill, nlevel_fill, 2, 0, 0, plstream::fill,
// 			  clevel, nlevel, 2, 0, 0, plstream::fill,
//			  false, mypltr, static_cast<void*>( spa));
                          false, mypltr, static_cast<void*>(&passinfo));
	
	// Redraw the axes just in case the filling overlaps them
	//actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);
	//actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);
	// pen thickness for axis
	actStream->wid(charthick);
      } else {
        actStream->cont(z, xEl, yEl, 1, xEl, 1, yEl, 
                        clevel, nlevel, mypltr, static_cast<void*>(&passinfo)) ;

      }
      delete[] z;
    }
    
    if (xVal->Rank() == 2 && yVal->Rank() == 2) {
      // FIXME: mapping not supported here yet
      
      PLcGrid2 cgrid2;
      actStream->Alloc2dGrid(&cgrid2.xg,xVal->Dim(0),xVal->Dim(1));
      actStream->Alloc2dGrid(&cgrid2.yg,xVal->Dim(0),xVal->Dim(1));
      cgrid2.nx = xVal->Dim(0);
      cgrid2.ny = xVal->Dim(1);

      for( SizeT i=0; i<xVal->Dim(0); i++) {
	for( SizeT j=0; j<xVal->Dim(1); j++) {
 	  cgrid2.xg[i][j] = (*xVal)[j*(xVal->Dim(0))+i];
	  cgrid2.yg[i][j] = (*yVal)[j*(xVal->Dim(0))+i];
	}
      }

      PLFLT** z = new PLFLT*[xVal->Dim(0)];
      for( SizeT i=0; i<xVal->Dim(0); i++) z[i] = &(*zVal)[i*xVal->Dim(1)];

      for( SizeT i=0; i<xVal->Dim(0)*xVal->Dim(1); i++) {
	if (isinf((*zVal)[i])) (*z)[i]= d_nan;
      }

      if (e->KeywordSet( "FILL")) {
	// the "clevel_fill, nlevel_fill" have been computed before
        actStream->shades(z, xVal->Dim(0), xVal->Dim(1), 
			  NULL, xStart, xEnd, yStart, yEnd,
 			  clevel_fill, nlevel_fill, 2, 0, 0, plstream::fill,
// 			  clevel, nlevel, 2, 0, 0, plstream::fill,
			  false, plstream::tr2, (void *) &cgrid2 );

	// Redraw the axes just in case the filling overlaps them       
	//actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);
	//actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);
	// pen thickness for axis
	actStream->wid(charthick);
      } else {	      
	actStream->cont(z, xVal->Dim(0), xVal->Dim(1), 
			1, xVal->Dim(0), 1, xVal->Dim(1), clevel, nlevel,
			plstream::tr2, (void *) &cgrid2 );
      }
      actStream->Free2dGrid(cgrid2.xg,xVal->Dim(0),xVal->Dim(1));
      actStream->Free2dGrid(cgrid2.yg,xVal->Dim(0),xVal->Dim(1));

      // AC june 07 : symetry for the previous case
      delete[] z;
    }

    //Draw axes after the data because /fill could potentially overlap the axes.
    //... if keyword "OVERPLOT" is not set
    if (!overplot) 
    {
      gkw_background(e, actStream);  //BACKGROUND
      gkw_color(e, actStream);       //COLOR

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
      PLFLT xintv;
      if (xTicks == 0) {
        xintv = AutoTick(xEnd-xStart);
      } else {
        xintv = (xEnd - xStart) / xTicks;
      }
      //Draw axis if keyword "OVERPLOT" is not set
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

      // Get viewpoint parameters and store in WINDOW & S
      PLFLT p_xmin, p_xmax, p_ymin, p_ymax;
      actStream->gvpd (p_xmin, p_xmax, p_ymin, p_ymax);

      DStructGDL* Struct=NULL;
      Struct = SysVar::X();
      static unsigned windowTag = Struct->Desc()->TagIndex( "WINDOW");
      static unsigned sTag = Struct->Desc()->TagIndex( "S");
      if(Struct != NULL) 
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

      // title and sub title
      actStream->schr( 0.0, 1.25*actH/defH);
      actStream->mtex("t",1.25,0.5,0.5,title.c_str());
      actStream->schr( 0.0, actH/defH); // charsize is reset here
      actStream->mtex("b",5.4,0.5,0.5,subTitle.c_str());
    
    }

    actStream->flush();
    actStream->lsty(1);//reset linestyle

    if (!overplot)
    {
      // set ![XY].CRANGE
      set_axis_crange("X", xStart, xEnd);
      set_axis_crange("Y", yStart, yEnd);

      //set ![x|y].type
      set_axis_type("X",xLog);
      set_axis_type("Y",yLog);
    }
  } // contour		 
  
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
      intv = AutoIntvAC(xStart, xEnd, false );
      //      intv = AutoIntv(xEnd-xStart);
      // xEnd = ceil(xEnd/intv) * intv;
      // xStart = floor(xStart/intv) * intv;
    }

    if ((yStyle & 1) != 1 && yAxis) {
      PLFLT intv;
      intv = AutoIntvAC(xStart, xEnd, false );
      //      intv = AutoIntv(yEnd-yStart);
      //yEnd = ceil(yEnd/intv) * intv;
      //yStart = floor(yStart/intv) * intv;
    }


    DLong ynozero, xnozero;
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, ynozero);
    gkw_axis_range(e, "Y", yStart, yEnd, xnozero);

    // AC nomore useful
    // if(xEnd == xStart) xEnd=xStart+1;

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
    gkw_noerase(e, actStream, true);     //NOERASE
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

    DDouble *sx;
    DDouble *sy;
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    static unsigned syTag = yStruct->Desc()->TagIndex( "S");
    sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];

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

    DDouble *sx;
    DDouble *sy;
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    static unsigned syTag = yStruct->Desc()->TagIndex( "S");
    sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];

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

    a->Color( color, decomposed);  
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
      for( SizeT i = 0; i<res->N_Elements(); ++i) {	
	if (isinf((DDouble) (*res)[i]) != 0)
	  (*res)[i] = 1e300000/1e300000;
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
	    (*res)[ires++] = ((*ptr2) / yv - sx[0]) / sy[1];
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

    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static DStructGDL* zStruct = SysVar::Z();
    static unsigned sxTag = xStruct->Desc()->TagIndex( "S");
    static unsigned syTag = yStruct->Desc()->TagIndex( "S");
    static unsigned szTag = zStruct->Desc()->TagIndex( "S");
    DDouble *sx = &(*static_cast<DDoubleGDL*>( xStruct->GetTag( sxTag, 0)))[0];
    DDouble *sy = &(*static_cast<DDoubleGDL*>( yStruct->GetTag( syTag, 0)))[0];
    DDouble *sz = &(*static_cast<DDoubleGDL*>( zStruct->GetTag( szTag, 0)))[0];
    static unsigned xtTag = xStruct->Desc()->TagIndex( "TYPE");
    static unsigned ytTag = yStruct->Desc()->TagIndex( "TYPE");
    static unsigned ztTag = zStruct->Desc()->TagIndex( "TYPE");
    DLong xt = (*static_cast<DLongGDL*>( xStruct->GetTag( xtTag, 0)))[0];
    DLong yt = (*static_cast<DLongGDL*>( xStruct->GetTag( ytTag, 0)))[0];

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

