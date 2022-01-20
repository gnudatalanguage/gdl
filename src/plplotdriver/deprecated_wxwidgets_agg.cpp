// Copyright (C) 2008  Werner Smekal
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//

// TODO:
// - let the AGG library process the text. In the moment most of the relevant code
//   is commented out, since there are problems with the affine transformation
//

// wxwidgets headers
#include <wx/wx.h>
#include <wx/strconv.h>

#include "plDevs.h"

// plplot headers
#include "plplotP.h"
#include "plfci-truetype.h"

// std and driver headers
#include "deprecated_wxwidgets.h"
#include <wchar.h>

// helper functions
#if !defined ( _WIN32 ) || defined ( __GNUC__ )
  #include <unistd.h>
#else
  #define F_OK    1
  #include <stdio.h>
int access( char *filename, int flag )
{
    FILE *infile;
    infile = fopen( filename, "r" );
    if ( infile != NULL )
    {
        fclose( infile );
        return 0;
    }
    else
        return 1;
}
#endif

#define makeunixslash( b )    do { char *I; for ( I = b; *I != 0; *I++ ) if ( *I == '\\' ) *I = '/';} while ( 0 )

//--------------------------------------------------------------------------
//  wxPLDevAGG::wxPLDevAGG()
//
//  Constructor of the AGG wxWidgets device based on the wxPLDevBase
//  class. Initialisations of variables and objects are done.
//--------------------------------------------------------------------------
wxPLDevAGG::wxPLDevAGG() :
    wxPLDevBase( wxBACKEND_AGG ),
    mRenderingBuffer(),
    mPixFormat( mRenderingBuffer ),
    mRendererBase( mPixFormat ),
    mRendererSolid( mRendererBase ),

    mPath(),
    mTransform(),
    mConvCurve( mPath ),
    mConvStroke( mConvCurve ),
    mPathTransform( mConvCurve, mTransform ),
    mStrokeTransform( mConvStroke, mTransform ),

    mFontEngine(),
    mFontManager( mFontEngine ),
    mCurves( mFontManager.path_adaptor() ),
    mContour( mCurves ),

    mBuffer( NULL ),
    mStrokeWidth( 1.0 ),
    mStrokeOpacity( 255 ),
    mColorRedStroke( 255 ),
    mColorGreenStroke( 255 ),
    mColorBlueStroke( 255 ),
    mColorRedFill( 0 ),
    mColorGreenFill( 0 ),
    mColorBlueFill( 0 )
{
    mCurves.approximation_scale( 2.0 );
    mContour.auto_detect_orientation( false );
    mConvStroke.line_join( agg::round_join );
    mConvStroke.line_cap( agg::round_cap );

    // determine font directory
#if defined ( _WIN32 )
    //static char *default_font_names[]={"arial.ttf","times.ttf","timesi.ttf","arial.ttf",
    //                                 "symbol.ttf"};
    // char WINDIR_PATH[255];
    // char *b;
    // b=getenv("WINDIR");
    // strncpy(WINDIR_PATH,b,255);

//
// Work out if we have Win95+ or Win3.?... sort of.
// Actually, this just tries to find the place where the fonts live by looking
// for arial, which should be on all windows machines.
// At present, it only looks in two places, on one drive. I might change this
// soon.
//
    //if (WINDIR_PATH==NULL)
    // {
    //  if (access("c:\\windows\\fonts\\arial.ttf", F_OK)==0) {
    //      strcpy(font_dir,"c:/windows/fonts/");
    //  }
    //  else if ( access("c:\\windows\\system\\arial.ttf", F_OK)==0) {
    //      strcpy(font_dir,"c:/windows/system/");
    //  }
    //  else
    //  plwarn("Could not find font path; I sure hope you have defined fonts manually !");
    // }
    // else
    // {
    // strncat(WINDIR_PATH,"\\fonts\\arial.ttf",255);
    // if (access(WINDIR_PATH, F_OK)==0)
    //  {
    //    b=strrchr(WINDIR_PATH,'\\');
    //    b++;
    //b=0;
    //    makeunixslash(WINDIR_PATH);
    //    strcpy(font_dir,WINDIR_PATH);
    //  }
    // else
    //  plwarn("Could not find font path; I sure hope you have defined fonts manually !");
    // }
    //
    // if (pls->debug) fprintf( stderr, "%s\n", font_dir ) ;
#else
    //  For Unix systems, we will set the font path up a little differently in
    //  that the configured PL_FREETYPE_FONT_DIR has been set as the default path,
    //  but the user can override this by setting the environmental variable
    //  "PLPLOT_FREETYPE_FONT_DIR" to something else.
    //  NOTE WELL - the trailing slash must be added for now !
    //
    // const char *str;
    //
    // fontdir.Clear();
    // if( (str=getenv("PLPLOT_FREETYPE_FONT_DIR"))!=NULL )
    //      fontdir.Append( wxString(str, wxConvFile) );
    // else
    //      fontdir.Append( wxT(PL_FREETYPE_FONT_DIR) );
    //
    // //printf("fontdir=%s, len=%d\n", fontdir.c_str(), fontdir.Length() );
#endif
}


//--------------------------------------------------------------------------
//  wxPLDevAGG::~wxPLDevAGG()
//
//  Deconstructor frees allocated buffer.
//--------------------------------------------------------------------------
wxPLDevAGG::~wxPLDevAGG()
{
    if ( ownGUI )
        if ( mBuffer )
            delete mBuffer;
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::drawPath( drawPathFlag flag )
//
//  Common function which either draws a stroke along a path or a filled
//  polygon surrounded by a stroke depending on flag.
//--------------------------------------------------------------------------
void wxPLDevAGG::drawPath( drawPathFlag flag )
{
    mRasterizer.reset();

    switch ( flag )
    {
    case Stroke:
        if ( mStrokeOpacity && mStrokeWidth > 0.0 )
        {
            mConvStroke.width( mStrokeWidth );
            mRasterizer.add_path( mStrokeTransform );
            mRendererSolid.color( agg::rgba8( mColorRedStroke, mColorGreenStroke, mColorBlueStroke, mStrokeOpacity ) );
            agg::render_scanlines( mRasterizer, mScanLine, mRendererSolid );
        }
        break;
    case FillAndStroke:
        if ( mStrokeOpacity )
        {
            mRasterizer.add_path( mPathTransform );
            mRendererSolid.color( agg::rgba8( mColorRedStroke, mColorGreenStroke, mColorBlueStroke, mStrokeOpacity ) );
            agg::render_scanlines( mRasterizer, mScanLine, mRendererSolid );
        }

        if ( mStrokeOpacity && mStrokeWidth > 0.0 )
        {
            mConvStroke.width( mStrokeWidth );
            mRasterizer.add_path( mStrokeTransform );
            mRendererSolid.color( agg::rgba8( mColorRedStroke, mColorGreenStroke, mColorBlueStroke, mStrokeOpacity ) );
            agg::render_scanlines( mRasterizer, mScanLine, mRendererSolid );
        }
        break;
    }
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::DrawLine( short x1a, short y1a, short x2a, short y2a )
//
//  Draw a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void wxPLDevAGG::DrawLine( short x1a, short y1a, short x2a, short y2a )
{
    mPath.remove_all();
    mPath.move_to( x1a, y1a );
    mPath.line_to( x2a, y2a );

    if ( !resizing && ownGUI )
        AGGAddtoClipRegion( x1a, y1a, x2a, y2a );

    drawPath( Stroke );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::DrawPolyline( short *xa, short *ya, PLINT npts )
//
//  Draw a poly line - coordinates are in the xa and ya arrays.
//--------------------------------------------------------------------------
void wxPLDevAGG::DrawPolyline( short *xa, short *ya, PLINT npts )
{
    mPath.remove_all();
    mPath.move_to( xa[0], ya[0] );
    for ( PLINT i = 1; i < npts; i++ )
    {
        mPath.line_to( xa[i], ya[i] );
        if ( !resizing && ownGUI )
            AGGAddtoClipRegion( xa[i - 1], ya[i - 1], xa[i], ya[i] );
    }

    drawPath( Stroke );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb,
//                                    PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
//
//  Clear parts ((x1,y1) to (x2,y2)) of the background in color (bgr,bgg,bgb).
//--------------------------------------------------------------------------
void wxPLDevAGG::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
{
    if ( x1 < 0 && y1 < 0 && x2 < 0 && y2 < 0 )
    {
        mRendererBase.clear( agg::rgba8( bgr, bgg, bgb ) );
        if ( !resizing && ownGUI )
            AddtoClipRegion( 0, 0, width, height );
    }
    else
    {
        mPath.remove_all();
        mPath.move_to( x1, y1 );
        mPath.line_to( x2, y1 );
        mPath.line_to( x2, y2 );
        mPath.line_to( x1, y2 );
        mPath.close_polygon();

        mRasterizer.reset();
        mRasterizer.add_path( mPathTransform );
        mRendererSolid.color( agg::rgba8( bgr, bgg, bgb, 255 ) );
        agg::render_scanlines( mRasterizer, mScanLine, mRendererSolid );

        mConvStroke.width( 1.0 );
        mRasterizer.add_path( mStrokeTransform );
        mRendererSolid.color( agg::rgba8( bgr, bgg, bgb, 255 ) );
        agg::render_scanlines( mRasterizer, mScanLine, mRendererSolid );

        if ( !resizing && ownGUI )
            AGGAddtoClipRegion( x1, y1, x2, y2 );
    }
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::AGGAddtoClipRegion( short x1, short y1,
//                                       short x2, short y2 )
//
//  Adds the region (x1,y1)-(x2,y2) to the regions which needs to be
//  updated/redrawn.
//--------------------------------------------------------------------------
void wxPLDevAGG::AGGAddtoClipRegion( short x1, short y1, short x2, short y2 )
{
    double x1d = x1, x2d = x2, y1d = y1, y2d = y2;

    mTransform.transform( &x1d, &y1d );
    mTransform.transform( &x2d, &y2d );
    AddtoClipRegion( (int) floor( x1d ), (int) floor( y1d ), (int) ceil( x2d ), (int) ceil( y2d ) );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::FillPolygon( PLStream *pls )
//
//  Draw a filled polygon.
//--------------------------------------------------------------------------
void wxPLDevAGG::FillPolygon( PLStream *pls )
{
    short *xa = pls->dev_x;
    short *ya = pls->dev_y;

    mPath.remove_all();
    mPath.move_to( xa[0], ya[0] );
    for ( PLINT i = 1; i < pls->dev_npts; i++ )
    {
        mPath.line_to( xa[i], ya[i] );
        if ( !resizing && ownGUI )
            AGGAddtoClipRegion( xa[i - 1], ya[i - 1], xa[i], ya[i] );
    }
    mPath.line_to( xa[0], ya[0] );
    mPath.close_polygon();

    drawPath( FillAndStroke );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::BlitRectangle( wxDC* dc, int vX, int vY,
//                                  int vW, int vH )
//
//  Copy/Blit a rectangle ((vX,vY) to (vX+vW,vY+vH)) into given dc.
//--------------------------------------------------------------------------
void wxPLDevAGG::BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH )
{
    if ( mBuffer )
    {
        wxMemoryDC MemoryDC;
        wxBitmap   bitmap( mBuffer->GetSubImage( wxRect( vX, vY, vW, vH ) ), -1 );
        MemoryDC.SelectObject( bitmap );
        dc->Blit( vX, vY, vW, vH, &MemoryDC, 0, 0 );
        MemoryDC.SelectObject( wxNullBitmap );
    }
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::CreateCanvas( void )
//
//  Create canvas (bitmap and dc) if the driver provides the GUI.
//--------------------------------------------------------------------------
void wxPLDevAGG::CreateCanvas()
{
    if ( ownGUI )
    {
        // get a new wxImage (image buffer)
        if ( mBuffer )
            delete mBuffer;
        mBuffer = new wxImage( bm_width, bm_height );
        mRenderingBuffer.attach( mBuffer->GetData(), bm_width, bm_height, bm_width * 3 );
    }
    else
        mRenderingBuffer.attach( mBuffer->GetData(), width, height, width * 3 );

    mRendererBase.reset_clipping( true );
    mTransform.reset();
    mTransform.premultiply( agg::trans_affine_translation( 0.0, height ) );
    mTransform.premultiply( agg::trans_affine_scaling( 1.0 / scalex, -1.0 / scaley ) );
    mStrokeWidth = ( scalex + scaley ) / 2.0;
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::SetWidth( PLStream *pls )
//
//  Set the width of the drawing pen.
//--------------------------------------------------------------------------
void wxPLDevAGG::SetWidth( PLStream *pls )
{
    mStrokeWidth = ( scalex + scaley ) / 2.0 * ( pls->width > 0 ? pls->width : 1 ); // TODO: why and when ist width 0???
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::SetColor0( PLStream *pls )
//
//  Set color from colormap 0.
//--------------------------------------------------------------------------
void wxPLDevAGG::SetColor0( PLStream *pls )
{
    mColorRedStroke   = pls->curcolor.r;
    mColorGreenStroke = pls->curcolor.g;
    mColorBlueStroke  = pls->curcolor.b;
    mStrokeOpacity    = (wxUint8) ( pls->curcolor.a * 255 );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::SetColor1( PLStream *pls )
//
//  Set color from colormap 1.
//--------------------------------------------------------------------------
void wxPLDevAGG::SetColor1( PLStream *pls )
{
    mColorRedStroke   = pls->curcolor.r;
    mColorGreenStroke = pls->curcolor.g;
    mColorBlueStroke  = pls->curcolor.b;
    mStrokeOpacity    = (wxUint8) ( pls->curcolor.a * 255 );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::SetExternalBuffer( void* dc )
//
//  Adds a dc to the device. In that case, the drivers doesn't provide
//  a GUI. A new buffer (image) will be created and set up.
//--------------------------------------------------------------------------
void wxPLDevAGG::SetExternalBuffer( void* image )
{
    mBuffer = (wxImage *) image; // Add the image to the device
    mRenderingBuffer.attach( mBuffer->GetData(), width, height, width * 3 );

    mRendererBase.reset_clipping( true );
    mTransform.reset();
    mTransform.premultiply( agg::trans_affine_translation( 0.0, height ) );
    mTransform.premultiply( agg::trans_affine_scaling( 1.0 / scalex, -1.0 / scaley ) );
    mStrokeWidth = ( scalex + scaley ) / 2.0;

    ready  = true;
    ownGUI = false;
}


#ifdef PL_HAVE_FREETYPE

//--------------------------------------------------------------------------
//  void wxPLDevAGG::PutPixel( short x, short y, PLINT color )
//
//  Draw a pixel in color color @ (x,y).
//--------------------------------------------------------------------------
void wxPLDevAGG::PutPixel( short x, short y, PLINT color )
{
    mBuffer->SetRGB( x, y, GetRValue( color ), GetGValue( color ), GetBValue( color ) );
    AddtoClipRegion( x, y, x, y );
}


//--------------------------------------------------------------------------
//  void wxPLDevAGG::PutPixel( short x, short y )
//
//  Draw a pixel in current color @ (x,y).
//--------------------------------------------------------------------------
void wxPLDevAGG::PutPixel( short x, short y )
{
    mBuffer->SetRGB( x, y, mColorRedStroke, mColorGreenStroke, mColorBlueStroke );
    AddtoClipRegion( x, y, x, y );
}


//--------------------------------------------------------------------------
//  PLINT wxPLDevAGG::GetPixel( short x, short y )
//
//  Get color information from pixel @ (x,y).
//--------------------------------------------------------------------------
PLINT wxPLDevAGG::GetPixel( short x, short y )
{
    return RGB( mBuffer->GetRed( x, y ), mBuffer->GetGreen( x, y ), mBuffer->GetBlue( x, y ) );
}

#endif // PL_HAVE_FREETYPE


void wxPLDevAGG::PSDrawTextToDC( char* utf8_string, bool drawText )
{
    // Log_Verbose( "%s", __FUNCTION__ );
    printf( "utf8_string=%s\n", utf8_string );

    double start_x = 0.0;
    double start_y = 0.0;

    //wchar_t str[512];
    //size_t len=wxConvUTF8.ToWChar( str, 512, utf8_string );
    size_t len   = strlen( utf8_string );
    char   * str = utf8_string;
    printf( "len=%lu\n", (unsigned long) len );

    const agg::glyph_cache* glyph;
    if ( !drawText )
    {
        double x         = 0;
        double y         = 0;
        bool   first     = true;
        char   * saveStr = str;
        while ( *str && len )
        {
            glyph = mFontManager.glyph( *str );
            if ( glyph )
            {
                if ( !first )
                    mFontManager.add_kerning( &x, &y );
                x    += glyph->advance_x;
                y    += glyph->advance_y;
                first = false;
            }
            textHeight = textHeight > ( glyph->bounds.y2 - glyph->bounds.y1 + yOffset ) ?
                         textHeight : ( glyph->bounds.y2 - glyph->bounds.y1 + yOffset );
            ++str; --len;
        }
        textWidth = x;
        printf( "str: %s, textWidth=%lf\n", saveStr, textWidth );
    }
    else
    {
        for ( size_t i = 0; i < len && str[i]; i++ )
        {
            glyph = mFontManager.glyph( str[i] );
            if ( glyph )
            {
                printf( "before: start_x=%f, start_y=%f\n", start_x, start_y );
                if ( i )
                    mFontManager.add_kerning( &start_x, &start_y );
                printf( "after: start_x=%f, start_y=%f\n", start_x, start_y );
                mFontManager.init_embedded_adaptors( glyph, start_x, start_y );

                mRendererSolid.color( agg::rgba8( mColorRedStroke, mColorGreenStroke, mColorBlueStroke, mStrokeOpacity ) );
                agg::render_scanlines( mFontManager.gray8_adaptor(), mFontManager.gray8_scanline(), mRendererSolid );

                start_x += glyph->advance_x / scalex;
                //start_y += glyph->advance_y/scaley;
            }
        }
    }

    memset( utf8_string, '\0', max_string_length );
}


void wxPLDevAGG::PSSetFont( PLUNICODE fci )
{
    // convert the fci to Base14/Type1 font information
    wxString fontname = fontdir + wxString( plP_FCI2FontName( fci, TrueTypeLookup, N_TrueTypeLookup ), *wxConvCurrent );

    if ( !mFontEngine.load_font( "/usr/share/fonts/truetype/freefont/FreeSans.ttf", 0, agg::glyph_ren_agg_gray8 ) )
        plabort( "Font could not be loaded" );
    //mFontEngine.load_font( "c:\\windows\\fonts\\arial.ttf", 0, agg::glyph_ren_agg_gray8 );
    mFontEngine.height( fontSize * fontScale );
    mFontEngine.width( fontSize * fontScale );
    mFontEngine.hinting( true );
    mFontEngine.flip_y( false );
    mContour.width( fontSize * fontScale * 0.2 );
}


void wxPLDevAGG::ProcessString( PLStream* pls, EscText* args )
{
    plabort( "The AGG backend can't process the text yet own its own!" );

    // Check that we got unicode, warning message and return if not
    if ( args->unicode_array_len == 0 )
    {
        printf( "Non unicode string passed to a wxWidgets driver, ignoring\n" );
        return;
    }

    // Check that unicode string isn't longer then the max we allow
    if ( args->unicode_array_len >= 500 )
    {
        printf( "Sorry, the wxWidgets drivers only handles strings of length < %d\n", 500 );
        return;
    }

    // Calculate the font size (in pixels)
    fontSize = pls->chrht * DEVICE_PIXELS_PER_MM * 1.2 * scaley;

    // calculate rotation of text
    plRotationShear( args->xform, &rotation, &shear, &stride );
    rotation -= pls->diorot * M_PI / 2.0;
    cos_shear = cos( shear );
    sin_shear = sin( shear );

    PSDrawText( args->unicode_array, args->unicode_array_len, false );
    printf( "textWidth=%f, textHeight=%f\n", textWidth, textHeight );

    agg::trans_affine mtx;
    mtx.reset();
    mtx *= agg::trans_affine_translation( args->x, args->y );
    //mtx *= agg::trans_affine_rotation( rotation );
    //mtx *= agg::trans_affine_skewing( shear, shear );
    mtx *= mTransform;
    mtx *= agg::trans_affine_translation( -args->just * textWidth / scalex, -0.5 * textHeight );
    mtx *= agg::trans_affine_translation( -args->just * textWidth / scalex, -0.5 * textHeight );
    mFontEngine.transform( mtx );

    PSDrawText( args->unicode_array, args->unicode_array_len, true );

    AddtoClipRegion( 0, 0, width, height );
}
