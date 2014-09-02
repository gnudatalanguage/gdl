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
#include <gsl/gsl_const_mksa.h> // GSL_CONST_MKSA_INCH

namespace lib {

  using namespace std;

  void device( EnvT* e)
  {

    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    //GET functions are examined BEFORE setting functions.
    //GET_CURRENT_FONT ? //TODO
    
    // GET_DECOMPOSED ?
    {
    static int get_decomposedIx = e->KeywordIx( "GET_DECOMPOSED" );
    if ( e->KeywordPresent( get_decomposedIx ) )
      {
        DLong value = actDevice->GetDecomposed( );
        if ( value == -1 )
          e->Throw( "Keyword GET_DECOMPOSED not allowed for call to: DEVICE" );
        else
          e->SetKW( get_decomposedIx, new DLongGDL( value ) );
      }
    }
    
    //GET_FONTNAMES? //TODO
    //GET_FONTNUM? //TODO
    
    // GET_GRAPHICS_FUNCTION
    {
    static int get_graphics_FunctionIx = e->KeywordIx( "GET_GRAPHICS_FUNCTION");
    if( e->KeywordPresent( get_graphics_FunctionIx)) 
      {
        DLong value = actDevice->GetGraphicsFunction();
        if(value == -1)
          e->Throw( "Keyword GET_GRAPHICS_FUNCTION not allowed for call to: DEVICE");
        else 
          e->SetKW( get_graphics_FunctionIx, new DLongGDL( value));
      }
    }
    
    // GET_PAGE_SIZE ?
    {
     static int get_page_sizeIx = e->KeywordIx("GET_PAGE_SIZE");
      if( e->KeywordPresent( get_page_sizeIx)) 
      {
       DIntGDL* value = actDevice->GetPageSize();
       if (value == NULL) 
          e->Throw( "Keyword GET_PAGE_SIZE not allowed for call to: DEVICE");
       else 
          e->SetKW( get_page_sizeIx, value );
      }
    }

    // GET_PIXEL_DEPTH ?
    {
      static int get_pixel_depthIx = e->KeywordIx( "GET_PIXEL_DEPTH");
      if( e->KeywordPresent( get_pixel_depthIx)) 
      {
        DLong value = actDevice->GetPixelDepth();
        if(value == -1)
          e->Throw( "Keyword GET_PIXEL_DEPTH not allowed for call to: DEVICE");
        else 
          e->SetKW( get_pixel_depthIx, new DLongGDL( value ));
      }
    }

    // GET_SCREEN_SIZE
    {
      static int get_screen_sizeIx = e->KeywordIx("GET_SCREEN_SIZE");
      if( e->KeywordPresent( get_screen_sizeIx)) 
      {
       DIntGDL* value = actDevice->GetScreenSize();
       if (value == NULL) 
          e->Throw( "Keyword GET_SCREEN_SIZE not allowed for call to: DEVICE");
       else 
          e->SetKW( get_screen_sizeIx, value);
      }
    }
   
    // GET_VISUAL_DEPTH ?
    {
      static int get_visual_depthIx = e->KeywordIx( "GET_VISUAL_DEPTH");
      if( e->KeywordPresent( get_visual_depthIx)) 
      {
        DLong value = actDevice->GetVisualDepth();
        if(value == -1)
          e->Throw( "Keyword GET_VISUAL_DEPTH not allowed for call to: DEVICE");
        else 
          e->SetKW( get_visual_depthIx, new DLongGDL( value));
      }
    }
    
    // GET_VISUAL_NAME ?
    {
      static int get_visual_nameIx = e->KeywordIx( "GET_VISUAL_NAME");
      if( e->KeywordPresent( get_visual_nameIx)) 
      {
        DString value = actDevice->GetVisualName();
        if(value == "")
          e->Throw( "Keyword GET_VISUAL_NAME not allowed for call to: DEVICE");
        else 
          e->SetKW( get_visual_nameIx, new DStringGDL( value));
      }
    } 

    // GET_WINDOW_POSITION ?
    {
     static int get_window_positionIx = e->KeywordIx("GET_WINDOW_POSITION");
      if( e->KeywordPresent( get_window_positionIx)) 
      {
       DIntGDL* value = actDevice->GetWindowPosition();
       if (value == NULL) 
          e->Throw( "Keyword GET_WINDOW_POSITION not allowed for call to: DEVICE");
       else 
          e->SetKW( get_window_positionIx, value);
      }
    }

    // GET_WRITE_MASK ? 
    {
    static int get_write_maskIx = e->KeywordIx( "GET_WRITE_MASK");
      if( e->KeywordPresent( get_write_maskIx)) 
      {
        DLong value = actDevice->GetWriteMask();
        if(value == -1)
          e->Throw( "Keyword GET_WRITE_MASK not allowed for call to: DEVICE");
        else 
          e->SetKW( get_write_maskIx, new DLongGDL( value));
      }
    }    

    // WINDOW_STATE ?
    {
    static int window_stateIx = e->KeywordIx( "WINDOW_STATE");
      if( e->KeywordPresent( window_stateIx)) 
      {
        DByteGDL* value = actDevice->WindowState();
        if(value == NULL)
          e->Throw( "Keyword WINDOW_STATE not allowed for call to: DEVICE");
        else 
          e->SetKW( window_stateIx, value);
      }
    } 

    // CLOSE_FILE {{{
    {
      static int closeFileIx = e->KeywordIx( "CLOSE_FILE"); 
      if( e->KeywordSet( closeFileIx))
	{
	  bool success = actDevice->CloseFile();
	  if( !success)
	    e->Throw( "Current device does not support keyword CLOSE_FILE.");
	}
    }

    // Z_BUFFERING 
    {
      static int z_bufferingIx = e->KeywordIx( "Z_BUFFERING"); 
      BaseGDL* z_buffering = e->GetKW( z_bufferingIx);
      if( z_buffering != NULL)
	{
	  bool success = actDevice->ZBuffering( e->KeywordSet( z_bufferingIx));
	  if( !success)
	    e->Throw( "Current device does not support keyword Z_BUFFERING.");
	}
    }

    // SET_RESOLUTION 
    {
      static int set_resolutionIx = e->KeywordIx( "SET_RESOLUTION"); 
      BaseGDL* set_resolution = e->GetKW( set_resolutionIx);
      if( set_resolution != NULL)
	{
	  DLongGDL* resolution = e->GetKWAs<DLongGDL>( set_resolutionIx);
	  if( resolution->N_Elements() != 2)
	    e->Throw( "Keyword array parameter SET_RESOLUTION must have 2 elements.");
	  DLong x = (*resolution)[0];
	  DLong y = (*resolution)[1];

	  if( x<0 || y<0)
	    e->Throw( "Value of Resolution is out of allowed range.");

	  bool success = actDevice->SetResolution( x, y);
	  if( !success)
	    e->Throw( "Current device does not support keyword SET_RESOLUTION.");
	}
    }

    // DECOMPOSED 
    {
      static int decomposedIx = e->KeywordIx( "DECOMPOSED"); 
      BaseGDL* decomposed = e->GetKW( decomposedIx);
      if( decomposed != NULL)
	{
	  bool success = actDevice->Decomposed( e->KeywordSet( decomposedIx));
	  if( !success)
	    e->Throw( "Current device does not support keyword DECOMPOSED.");
	}
    }  

    // SET_GRAPHICS_FUNCTION
    {
      static int set_graphicsFunctionIx = e->KeywordIx( "SET_GRAPHICS_FUNCTION");
      BaseGDL* set_gfunction = e->GetKW( set_graphicsFunctionIx);
      if( set_gfunction != NULL)
	{
	  DLongGDL* gfunction = e->GetKWAs<DLongGDL>( set_graphicsFunctionIx);
	  bool success = actDevice->SetGraphicsFunction((*gfunction)[0]);
	  if( !success)
	    e->Throw( "Current device does not support keyword SET_GRAPHICS_FUNCTION.");
	}
    }
    // CURSOR_STANDARD
    {
      static int cursorStandardIx = e->KeywordIx( "CURSOR_STANDARD");
      BaseGDL* res = e->GetKW( cursorStandardIx);
      if( res != NULL)
	{
	  DLongGDL* val = e->GetKWAs<DLongGDL>( cursorStandardIx);
	  bool success = actDevice->CursorStandard((*val)[0]);
	  if( !success)
	    e->Throw( "Current device does not support keyword CURSOR_STANDARD.");
	}
    }
    // RETAIN
    {
      static int valIx = e->KeywordIx( "RETAIN");
      BaseGDL* res = e->GetKW( valIx);
      if( res != NULL)
	{
	  DLongGDL* val = e->GetKWAs<DLongGDL>( valIx);
	  bool success = actDevice->SetBackingStore((*val)[0]);
	  if( !success)
	    e->Throw( "Current device does not support keyword RETAIN.");
	}
    }
    // CURSOR_CROSSHAIR
    {
      static int valIx = e->KeywordIx( "CURSOR_CROSSHAIR");
      BaseGDL* res = e->GetKW( valIx);
      if( res != NULL)
	{
	  bool success = actDevice->CursorCrosshair();
	  if( !success)
	    e->Throw( "Current device does not support keyword CURSOR_CROSSHAIR.");
	}
    }
    // CURSOR_ORIGINAL (WARNING: SAME CODE AS  CURSOR_CROSSHAIR!)
    {
      static int valIx = e->KeywordIx( "CURSOR_ORIGINAL");
      BaseGDL* res = e->GetKW( valIx);
      if( res != NULL)
	{
	  bool success = actDevice->CursorCrosshair();
	  if( !success)
	    e->Throw( "Current device does not support keyword CURSOR_ORIGINAL.");
	}
    }

    // FILENAME 
    {
      static int fileNameIx = e->KeywordIx( "FILENAME"); 
      BaseGDL* fileName = e->GetKW( fileNameIx);
      if( fileName != NULL)
	{
	  DString fName;
	  e->AssureStringScalarKW( fileNameIx, fName);
	  if( fName == "")
	    e->Throw( "Null filename not allowed.");
	  WordExp(fName);
	  bool success = actDevice->SetFileName( fName);
	  if( !success)
	    e->Throw( "Current device does not support keyword FILENAME.");
	}
    }

    // LANDSCAPE and PORTRAIT need to be executed before XSIZE, YSIZE, XOFFSET and YOFFSET!
    {
      static int portraitIx = e->KeywordIx( "PORTRAIT");
      static int landscapeIx = e->KeywordIx( "LANDSCAPE"); 
      if (e->KeywordSet(portraitIx) && e->KeywordSet(landscapeIx)) 
        Warning("Warning: both PORTRAIT and LANDSCAPE specified!");

      // LANDSCAPE 
      {
        if (e->GetKW(landscapeIx) != NULL)
	  {
	    bool success = actDevice->SetLandscape();
	    if (!success) e->Throw("Current device does not support keyword LANDSCAPE");
	  }
      }

      // PORTRAIT 
      {
        if (e->GetKW(portraitIx) != NULL)
	  {
	    bool success = actDevice->SetPortrait();
	    if (!success) e->Throw("Current device does not support keyword PORTRAIT");
	  }
      }
    }

    {
      static int inchesIx = e->KeywordIx( "INCHES");
      // XOFFSET 
      {
        static int xOffsetIx = e->KeywordIx( "XOFFSET");
        BaseGDL* xOffsetKW = e->GetKW( xOffsetIx);
        if( xOffsetKW != NULL)
	  {
	    DFloat xOffsetValue;
	    e->AssureFloatScalarKW( xOffsetIx, xOffsetValue);
	    bool success = actDevice->SetXOffset( xOffsetValue 
						  * (e->KeywordPresent(inchesIx) ? 100. * GSL_CONST_MKSA_INCH : 1.)
						  );
	    if( !success)
	      e->Throw( "Current device does not support keyword XOFFSET.");
	  } 
      }

      // YOFFSET 
      {
        static int yOffsetIx = e->KeywordIx( "YOFFSET");
        BaseGDL* yOffsetKW = e->GetKW( yOffsetIx);
        if( yOffsetKW != NULL)
	  {
	    DFloat yOffsetValue;
	    e->AssureFloatScalarKW( yOffsetIx, yOffsetValue);
	    bool success = actDevice->SetYOffset( yOffsetValue 
						  * (e->KeywordPresent(inchesIx) ? 100. * GSL_CONST_MKSA_INCH : 1.)
						  );
	    if( !success)
	      e->Throw( "Current device does not support keyword YOFFSET.");
	  } 
      }

      // XSIZE 
      {
        static int xSizeIx = e->KeywordIx( "XSIZE");
        BaseGDL* xSizeKW = e->GetKW( xSizeIx);
        if( xSizeKW != NULL)
	  {
	    DFloat xSizeValue;
	    e->AssureFloatScalarKW( xSizeIx, xSizeValue);
	    bool success = actDevice->SetXPageSize( xSizeValue 
						    * (e->KeywordPresent(inchesIx) ? 100. * GSL_CONST_MKSA_INCH : 1.)
						    );
	    if( !success)
	      e->Throw( "Current device does not support keyword XSIZE.");
	  } 
      }

      // YSIZE 
      {
        static int ySizeIx = e->KeywordIx( "YSIZE");
        BaseGDL* ySizeKW = e->GetKW( ySizeIx);
        if( ySizeKW != NULL)
	  {
	    DFloat ySizeValue;
	    e->AssureFloatScalarKW( ySizeIx, ySizeValue);
	    bool success = actDevice->SetYPageSize( ySizeValue
						    * (e->KeywordPresent(inchesIx) ? 100. * GSL_CONST_MKSA_INCH : 1.)
						    );
	    if( !success)
	      e->Throw( "Current device does not support keyword YSIZE.");
	  } 
      }
    }

    // SCALE_FACTOR 
    {
      static int scaleIx = e->KeywordIx( "SCALE_FACTOR");
      BaseGDL* scaleKW = e->GetKW( scaleIx);
      if( scaleKW != NULL)
	{
	  DFloat scaleValue;
	  e->AssureFloatScalarKW( scaleIx, scaleValue);
	  bool success = actDevice->SetScale( scaleValue);
	  if( !success)
	    e->Throw( "Current device does not support keyword SCALE.");
	} 
    }

    // COLOR 
    {
      // TODO: turn off with COLOR=0?
      static int colorIx = e->KeywordIx( "COLOR");
      BaseGDL* colorKW = e->GetKW( colorIx);
      if( colorKW != NULL)
	{
	  DLong colorValue;
	  e->AssureLongScalarKW( colorIx, colorValue);
	  bool success = actDevice->SetColor(colorValue);
	  if( !success) e->Throw( "Current device does not support keyword COLOR.");
	} 
    }

    // ENCAPSULATED 
    {
      static int encapsulatedIx = e->KeywordIx( "ENCAPSULATED");
      BaseGDL* encapsulatedKW = e->GetKW( encapsulatedIx);
      if( encapsulatedKW != NULL)
	{
	  bool success;
	  if ((*e->GetKWAs<DIntGDL>(encapsulatedIx))[0] == 0)
	    success = actDevice->SetEncapsulated(false);
	  else
	    success = actDevice->SetEncapsulated(true);
	  if (!success) e->Throw( "Current device does not support keyword ENCAPSULATED.");
	} 
    }

  }

} // namespace

