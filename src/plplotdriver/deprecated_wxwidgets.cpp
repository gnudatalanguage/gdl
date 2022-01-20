// Copyright (C) 2005  Werner Smekal, Sjaak Verdoold
// Copyright (C) 2005  Germain Carrera Corraleche
// Copyright (C) 1999  Frank Huebner
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
// - NA
//

// wxwidgets headers
#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/except.h>

#include "plDevs.h"

// plplot headers
#include "plplotP.h"
#include "drivers.h"

// C/C++ headers
#include <cstdio>

#include "deprecated_wxwidgets.h"

// Static function prototypes
#ifdef PL_HAVE_FREETYPE
static void plD_pixel_wxwidgets( PLStream *pls, short x, short y );
static PLINT plD_read_pixel_wxwidgets( PLStream *pls, short x, short y );
static void plD_set_pixel_wxwidgets( PLStream *pls, short x, short y, PLINT colour );
static void init_freetype_lv1( PLStream *pls );
static void init_freetype_lv2( PLStream *pls );
#endif

// private functions needed by the wxwidgets Driver
static void install_buffer( PLStream *pls );
static void wxRunApp( PLStream *pls, bool runonce = false );
static void GetCursorCmd( PLStream *pls, PLGraphicsIn *ptr );
static void fill_polygon( PLStream *pls );

#ifdef __WXMAC__
        #include <Carbon/Carbon.h>
extern "C" { void CPSEnableForegroundOperation( ProcessSerialNumber* psn ); }
#endif

DECLARE_PLAPP( wxPLplotApp )

//--------------------------------------------------------------------------
//  void Log_Verbose( const char *fmt, ... )
//
//  Print verbose debug message to stderr (printf style).
//--------------------------------------------------------------------------
void Log_Verbose( const char *fmt, ... )
{
#ifdef _DEBUG_VERBOSE
    va_list args;
    va_start( args, fmt );
    fprintf( stderr, "Verbose: " );
    vfprintf( stderr, fmt, args );
    fprintf( stderr, "\n" );
    va_end( args );
    fflush( stderr );
#else
    (void) fmt;        // Cast to void to silence compiler warnings about unused paraemeter
#endif
}


//--------------------------------------------------------------------------
//  void Log_Debug( const char *fmt, ... )
//
//  Print debug message to stderr (printf style).
//--------------------------------------------------------------------------
void Log_Debug( const char *fmt, ... )
{
#ifdef _DEBUG
    va_list args;
    va_start( args, fmt );
    fprintf( stderr, "Debug: " );
    vfprintf( stderr, fmt, args );
    fprintf( stderr, "\n" );
    va_end( args );
    fflush( stderr );
#else
    (void) fmt;        // Cast to void to silence compiler warnings about unused paraemeter
#endif
}


//--------------------------------------------------------------------------
//  In the following you'll find the driver functions which are
//  are needed by the plplot core.
//--------------------------------------------------------------------------

// Device info
#ifdef __cplusplus
extern "C" {
#endif

PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_wxwidgets =
#ifdef PLD_wxwidgets
    "wxwidgets:wxWidgets Driver:1:wxwidgets:51:wxwidgets\n"
#endif
#ifdef PLD_wxpng
    "wxpng:wxWidgets PNG Driver:0:wxwidgets:52:wxpng\n"
#endif
;

#ifdef __cplusplus
}
#endif


//--------------------------------------------------------------------------
//  wxPLDevBase::wxPLDevBase( void )
//
//  Contructor of base class of wxPLDev classes.
//--------------------------------------------------------------------------
wxPLDevBase::wxPLDevBase( int bcknd ) : backend( bcknd )
{
    // Log_Verbose( "wxPLDevBase::wxPLDevBase()" );

    ready    = false;
    ownGUI   = false;
    waiting  = false;
    resizing = false;
    exit     = false;

    comcount = 0;

    m_frame = NULL;
    xpos    = 0;
    ypos    = 0;
    // width, height are set in plD_init_wxwidgets
    // bm_width, bm_height are set in install_buffer

    // xmin, xmax, ymin, ymax are set in plD_init_wxwidgets
    // scalex, scaley are set in plD_init_wxwidgets

    plstate_width  = false;
    plstate_color0 = false;
    plstate_color1 = false;

    locate_mode = 0;
    draw_xhair  = false;

    newclipregion = true;
    clipminx      = 1024;
    clipmaxx      = 0;
    clipminy      = 800;
    clipmaxy      = 0;

    freetype    = 0;
    smooth_text = 0;

    devName = (const char **) malloc( NDEV * sizeof ( char** ) );
    memset( devName, '\0', NDEV * sizeof ( char** ) );
    devDesc = (const char **) malloc( NDEV * sizeof ( char** ) );
    memset( devDesc, '\0', NDEV * sizeof ( char** ) );
    ndev = NDEV;

    lineSpacing = 1.0;
}


wxPLDevBase::~wxPLDevBase( void )
{
    if ( devDesc )
        free( devDesc );
    if ( devName )
        free( devName );
}


void wxPLDevBase::AddtoClipRegion( int x1, int y1, int x2, int y2 )
{
    newclipregion = false;
    if ( x1 < x2 )
    {
        if ( x1 < clipminx )
            clipminx = x1;
        if ( x2 > clipmaxx )
            clipmaxx = x2;
    }
    else
    {
        if ( x2 < clipminx )
            clipminx = x2;
        if ( x1 > clipmaxx )
            clipmaxx = x1;
    }
    if ( y1 < y2 )
    {
        if ( y1 < clipminy )
            clipminy = y1;
        if ( y2 > clipmaxy )
            clipmaxy = y2;
    }
    else
    {
        if ( y2 < clipminy )
            clipminy = y2;
        if ( y1 > clipmaxy )
            clipmaxy = y1;
    }
}


void wxPLDevBase::PSDrawText( PLUNICODE* ucs4, int ucs4Len, bool drawText )
{
    int  i = 0;

    char utf8_string[max_string_length];
    char utf8[5];
    memset( utf8_string, '\0', max_string_length );

    // Get PLplot escape character
    char plplotEsc;
    plgesc( &plplotEsc );

    //Reset the size metrics
    textWidth         = 0;
    textHeight        = 0;
    superscriptHeight = 0;
    subscriptDepth    = 0;

    while ( i < ucs4Len )
    {
        if ( ucs4[i] < PL_FCI_MARK )                // not a font change
        {
            if ( ucs4[i] != (PLUNICODE) plplotEsc ) // a character to display
            {
                ucs4_to_utf8( ucs4[i], utf8 );
                strncat( utf8_string, utf8,
                    sizeof ( utf8_string ) - strlen( utf8_string ) - 1 );
                i++;
                continue;
            }
            i++;
            if ( ucs4[i] == (PLUNICODE) plplotEsc ) // a escape character to display
            {
                ucs4_to_utf8( ucs4[i], utf8 );
                strncat( utf8_string, utf8,
                    sizeof ( utf8_string ) - strlen( utf8_string ) - 1 );
                i++;
                continue;
            }
            else
            {
                if ( ucs4[i] == (PLUNICODE) 'u' ) // Superscript
                {                                 // draw string so far
                    PSDrawTextToDC( utf8_string, drawText );

                    // change font scale
                    if ( yOffset < -0.0001 )
                        fontScale *= 1.25; // Subscript scaling parameter
                    else
                        fontScale *= 0.8;  // Subscript scaling parameter
                    PSSetFont( fci );

                    yOffset += scaley * fontSize * fontScale / 2.;
                }
                if ( ucs4[i] == (PLUNICODE) 'd' ) // Subscript
                {                                 // draw string so far
                    PSDrawTextToDC( utf8_string, drawText );

                    // change font scale
                    double old_fontScale = fontScale;
                    if ( yOffset > 0.0001 )
                        fontScale *= 1.25; // Subscript scaling parameter
                    else
                        fontScale *= 0.8;  // Subscript scaling parameter
                    PSSetFont( fci );

                    yOffset -= scaley * fontSize * old_fontScale / 2.;
                }
                if ( ucs4[i] == (PLUNICODE) '-' ) // underline
                {                                 // draw string so far
                    PSDrawTextToDC( utf8_string, drawText );

                    underlined = !underlined;
                    PSSetFont( fci );
                }
                if ( ucs4[i] == (PLUNICODE) '+' ) // overline
                {                                 // not implemented yet
                }
                i++;
            }
        }
        else // a font change
        {
            // draw string so far
            PSDrawTextToDC( utf8_string, drawText );

            // get new font
            fci = ucs4[i];
            PSSetFont( fci );
            i++;
        }
    }

    PSDrawTextToDC( utf8_string, drawText );
}


//--------------------------------------------------------------------------
//  void common_init(  PLStream *pls )
//
//  Basic initialization for all devices.
//--------------------------------------------------------------------------
wxPLDevBase* common_init( PLStream *pls )
{
    // Log_Verbose( "common_init()" );

    wxPLDevBase* dev;
    PLFLT      downscale, downscale2;

    // default options
    static PLINT freetype    = -1;
    static PLINT smooth_text = 1;
    static PLINT text        = -1;
    static PLINT hrshsym     = 0;

    // default backend uses wxGraphicsContext, if not available
    // the agg library will be used, if not available the basic
    // backend will be used.
    static PLINT backend = wxBACKEND_DC;
  #if wxUSE_GRAPHICS_CONTEXT
    backend = wxBACKEND_GC;
        #else
                #ifdef HAVE_AGG
    backend = wxBACKEND_AGG;
                #endif
        #endif

    DrvOpt wx_options[] = {
#ifdef PL_HAVE_FREETYPE
        { "freetype", DRV_INT, &freetype,    "Use FreeType library"                                                             },
        { "smooth",   DRV_INT, &smooth_text, "Turn text smoothing on (1) or off (0)"                                            },
#endif
        { "hrshsym",  DRV_INT, &hrshsym,     "Use Hershey symbol set (hrshsym=0|1)"                                             },
        { "backend",  DRV_INT, &backend,     "Choose backend: (0) standard, (1) using AGG library, (2) using wxGraphicsContext" },
        { "text",     DRV_INT, &text,        "Use own text routines (text=0|1)"                                                 },
        { NULL,       DRV_INT, NULL,         NULL                                                                               }
    };

    // Check for and set up driver options
    plParseDrvOpts( wx_options );

    // allocate memory for the device storage
    switch ( backend )
    {
    case wxBACKEND_GC:
        // in case wxGraphicsContext isn't available, the next backend (agg
        // if available) in this list will be used
#if wxUSE_GRAPHICS_CONTEXT
        dev = new wxPLDevGC;
        // by default the own text routines are used for wxGC
        if ( text == -1 )
            text = 1;
        freetype = 0; // this backend is vector oriented and doesn't know pixels
        break;
#endif
    case wxBACKEND_AGG:
        // in case the agg library isn't available, the standard backend
        // will be used
#ifdef HAVE_AGG
        dev = new wxPLDevAGG;
        // by default the freetype text routines are used for wxAGG
        text = 0; // text processing doesn't work yet for the AGG backend
        if ( freetype == -1 )
            freetype = 1;
        break;
#endif
    default:
        dev = new wxPLDevDC;
        // by default the own text routines are used for wxDC
        if ( text == -1 )
        {
            if ( freetype != 1 )
                text = 1;
            else
                text = 0;
        }
        if ( freetype == -1 )
            freetype = 0;
        break;
    }
    if ( dev == NULL )
    {
        plexit( "Insufficient memory" );
    }
    pls->dev = (void *) dev;

// be verbose and write out debug messages
#ifdef _DEBUG
    pls->verbose = 1;
    pls->debug   = 1;
#endif

    pls->color     = 1;             // Is a color device
    pls->dev_flush = 1;             // Handles flushes
    pls->dev_fill0 = 1;             // Can handle solid fills
    pls->dev_fill1 = 0;             // Can't handle pattern fills
    pls->dev_dash  = 0;
    pls->dev_clear = 1;             // driver supports clear

    if ( text )
    {
        pls->dev_text    = 1; // want to draw text
        pls->dev_unicode = 1; // want unicode
        if ( hrshsym )
            pls->dev_hrshsym = 1;
    }

#ifdef PL_HAVE_FREETYPE
    // own text routines have higher priority over freetype
    // if text and freetype option are set to 1
    if ( !text )
    {
        dev->smooth_text = smooth_text;
        dev->freetype    = freetype;
    }

    if ( dev->freetype )
    {
        pls->dev_text    = 1; // want to draw text
        pls->dev_unicode = 1; // want unicode
        if ( hrshsym )
            pls->dev_hrshsym = 1;

        init_freetype_lv1( pls );
        FT_Data* FT = (FT_Data *) pls->FT;
        FT->want_smooth_text = smooth_text;
    }
#endif

    // initialize frame size and position
    if ( pls->xlength <= 0 || pls->ylength <= 0 )
        plspage( 0.0, 0.0, (PLINT) ( CANVAS_WIDTH * DEVICE_PIXELS_PER_IN ),
            (PLINT) ( CANVAS_HEIGHT * DEVICE_PIXELS_PER_IN ), 0, 0 );

    dev->width    = pls->xlength;
    dev->height   = pls->ylength;
    dev->clipminx = pls->xlength;
    dev->clipminy = pls->ylength;

    if ( pls->xoffset != 0 || pls->yoffset != 0 )
    {
        dev->xpos = (int) ( pls->xoffset );
        dev->ypos = (int) ( pls->yoffset );
    }


    // If portrait mode, apply a rotation and set freeaspect
    if ( pls->portrait )
    {
        plsdiori( (PLFLT) ( 4 - ORIENTATION ) );
        pls->freeaspect = 1;
    }

    // Set the number of pixels per mm
    plP_setpxl( (PLFLT) VIRTUAL_PIXELS_PER_MM, (PLFLT) VIRTUAL_PIXELS_PER_MM );

    // Set up physical limits of plotting device (in drawing units)
    downscale  = (double) dev->width / (double) ( PIXELS_X - 1 );
    downscale2 = (double) dev->height / (double) PIXELS_Y;
    if ( downscale < downscale2 )
        downscale = downscale2;
    plP_setphy( (PLINT) 0, (PLINT) ( dev->width / downscale ),
        (PLINT) 0, (PLINT) ( dev->height / downscale ) );

    // get physical device limits coordinates
    plP_gphy( &dev->xmin, &dev->xmax, &dev->ymin, &dev->ymax );

    // setting scale factors
    dev->scalex = (PLFLT) ( dev->xmax - dev->xmin ) / ( dev->width );
    dev->scaley = (PLFLT) ( dev->ymax - dev->ymin ) / ( dev->height );

    // set dpi
    plspage( VIRTUAL_PIXELS_PER_IN / dev->scalex, VIRTUAL_PIXELS_PER_IN / dev->scaley, 0, 0, 0, 0 );

#ifdef PL_HAVE_FREETYPE
    if ( dev->freetype )
        init_freetype_lv2( pls );
#endif

    // find out what file drivers are available
    plgFileDevs( &dev->devDesc, &dev->devName, &dev->ndev );

    return dev;
}


#ifdef PLD_wxwidgets

//--------------------------------------------------------------------------
//  void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
//
//  Make wxwidgets driver functions known to plplot.
//--------------------------------------------------------------------------
void plD_dispatch_init_wxwidgets( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "wxWidgets DC";
    pdt->pl_DevName = "wxwidgets";
#endif
    pdt->pl_type     = plDevType_Interactive;
    pdt->pl_seq      = 51;
    pdt->pl_init     = (plD_init_fp) plD_init_wxwidgets;
    pdt->pl_line     = (plD_line_fp) plD_line_wxwidgets;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_wxwidgets;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_wxwidgets;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_wxwidgets;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_wxwidgets;
    pdt->pl_state    = (plD_state_fp) plD_state_wxwidgets;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_wxwidgets;
}

//--------------------------------------------------------------------------
//  plD_init_wxwidgets( PLStream* pls )
//
//  Initialize wxWidgets device.
//--------------------------------------------------------------------------
void plD_init_wxwidgets( PLStream* pls )
{
    // Log_Verbose( "plD_init_wxwidgets()" );

    wxPLDevBase* dev;
    dev = common_init( pls );

    pls->plbuf_write = 1;             // use the plot buffer!
    pls->termin      = 1;             // interactive device
    pls->graphx      = GRAPHICS_MODE; //  No text mode for this driver (at least for now, might add a console window if I ever figure it out and have the inclination)

    dev->showGUI    = true;
    dev->bitmapType = (wxBitmapType) 0;
}

#endif  // PLD_wxwidgets


#ifdef PLD_wxpng

//--------------------------------------------------------------------------
//  void plD_dispatch_init_wxpng( PLDispatchTable *pdt )
//
//  Make wxpng driver functions known to plplot.
//--------------------------------------------------------------------------
void plD_dispatch_init_wxpng( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "wxWidgets PNG driver";
    pdt->pl_DevName = "wxpng";
#endif
    pdt->pl_type     = plDevType_FileOriented;
    pdt->pl_seq      = 52;
    pdt->pl_init     = (plD_init_fp) plD_init_wxpng;
    pdt->pl_line     = (plD_line_fp) plD_line_wxwidgets;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_wxwidgets;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_wxwidgets;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_wxwidgets;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_wxwidgets;
    pdt->pl_state    = (plD_state_fp) plD_state_wxwidgets;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_wxwidgets;
}

//--------------------------------------------------------------------------
//  void plD_init_wxpng( PLStream *pls )
//
//  Initialize wxpng device.
//--------------------------------------------------------------------------
void plD_init_wxpng( PLStream *pls )
{
    // Log_Verbose( "plD_init_wxwidgets()" );

    wxPLDevBase* dev;
    dev = common_init( pls );

    // Initialize family file info
    plFamInit( pls );

    // Prompt for a file name if not already set.
    plOpenFile( pls );

    pls->plbuf_write = 1;             // use the plot buffer!
    pls->dev_flush   = 0;             // No need for flushes
    pls->termin      = 0;             // file oriented device
    pls->graphx      = GRAPHICS_MODE; //  No text mode for this driver (at least for now, might add a console window if I ever figure it out and have the inclination)
    pls->page        = 0;

    dev->showGUI    = false;
    dev->bitmapType = wxBITMAP_TYPE_PNG;
}

#endif  // PLD_wxpng


//--------------------------------------------------------------------------
//  void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a,
//													 short x2a, short y2a )
//
//  Draws a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void plD_line_wxwidgets( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    // Log_Verbose( "plD_line_wxwidgets(x1a=%d, y1a=%d, x2a=%d, y2a=%d)", x1a, y1a, x2a, y2a );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    if ( !( dev->ready ) )
        install_buffer( pls );

    dev->DrawLine( x1a, y1a, x2a, y2a );

    if ( !( dev->resizing ) && dev->ownGUI )
    {
        dev->comcount++;
        if ( dev->comcount > MAX_COMCOUNT )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
    }
}


//--------------------------------------------------------------------------
//  void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya,
//															 PLINT npts )
//
//  Draw a poly line - points are in xa and ya arrays.
//--------------------------------------------------------------------------
void plD_polyline_wxwidgets( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    // Log_Verbose( "plD_polyline_wxwidgets()" );

    // should be changed to use the wxDC::DrawLines function?
    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    if ( !( dev->ready ) )
        install_buffer( pls );

    dev->DrawPolyline( xa, ya, npts );

    if ( !( dev->resizing ) && dev->ownGUI )
    {
        dev->comcount++;
        if ( dev->comcount > MAX_COMCOUNT )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
    }
}


//--------------------------------------------------------------------------
//  void plD_eop_wxwidgets( PLStream *pls )
//
//  End of Page. This function is called if a "end of page" is send by the
//  user. This command is ignored if we have the plot embedded in a
//  wxWidgets application, otherwise the application created by the device
//  takes over.
//--------------------------------------------------------------------------
void plD_eop_wxwidgets( PLStream *pls )
{
    // Log_Verbose( "plD_eop_wxwidgets()" );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    if ( dev->bitmapType )
    {
        wxMemoryDC memDC;
        wxBitmap   bitmap( dev->width, dev->height, -1 );
        memDC.SelectObject( bitmap );

        dev->BlitRectangle( &memDC, 0, 0, dev->width, dev->height );
        wxImage             buffer = bitmap.ConvertToImage();
        wxFFileOutputStream fstream( pls->OutFile );
        if ( !( buffer.SaveFile( fstream, dev->bitmapType ) ) )
            puts( "Troubles saving file!" );
        memDC.SelectObject( wxNullBitmap );
    }

    if ( dev->ownGUI && !dev->resizing )
    {
        if ( pls->nopause || !dev->showGUI )
            wxRunApp( pls, true );
        else
            wxRunApp( pls );
    }
}


//--------------------------------------------------------------------------
//  void plD_bop_wxwidgets( PLStream *pls )
//
//  Begin of page. Before any plot command, this function is called, If we
//  have already a dc the background is cleared in background color and some
//  state calls are resent - this is because at the first call of this
//  function, a dc does most likely not exist, but state calls are recorded
//  and when a new dc is created this function is called again.
//--------------------------------------------------------------------------
void plD_bop_wxwidgets( PLStream *pls )
{
    // Log_Verbose( "plD_bop_wxwidgets()" );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    if ( dev->ready )
    {
        //if( pls->termin==0 ) {
        // plGetFam( pls );
        // force new file if pls->family set for all subsequent calls to plGetFam
        // n.b. putting this after plGetFam call is important since plinit calls
        // bop, and you don't want the familying sequence started until after
        // that first call to bop.

        // n.b. pls->dev can change because of an indirect call to plD_init_png
        // from plGetFam if familying is enabled.  Thus, wait to define dev until
        // now.
        //dev = (wxPLDevBase*)pls->dev;
        //
        // pls->famadv = 1;
        // pls->page++;
        // }

        // clear background
        PLINT bgr, bgg, bgb;                  // red, green, blue
        plgcolbg( &bgr, &bgg, &bgb );         // get background color information
        dev->ClearBackground( bgr, bgg, bgb );

        // Replay escape calls that come in before PLESC_DEVINIT.  All of them
        // required a DC that didn't exist yet.
        //
        if ( dev->plstate_width )
            plD_state_wxwidgets( pls, PLSTATE_WIDTH );
        dev->plstate_width = false;

        if ( dev->plstate_color0 )
            plD_state_wxwidgets( pls, PLSTATE_COLOR0 );
        dev->plstate_color0 = false;

        if ( dev->plstate_color1 )
            plD_state_wxwidgets( pls, PLSTATE_COLOR1 );
        dev->plstate_color1 = false;

        // why this? xwin driver has this
        // pls->page++;
    }
}


//--------------------------------------------------------------------------
//  void plD_tidy_wxwidgets( PLStream *pls )
//
//  This function is called, if all plots are done.
//--------------------------------------------------------------------------
void plD_tidy_wxwidgets( PLStream *pls )
{
    // Log_Verbose( "plD_tidy_wxwidgets()" );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

#ifdef PL_HAVE_FREETYPE
    if ( dev->freetype )
    {
        FT_Data *FT = (FT_Data *) pls->FT;
        plscmap0n( FT->ncol0_org );
        plD_FreeType_Destroy( pls );
    }
#endif

    if ( dev->ownGUI )
    {
        wxPLGetApp().RemoveFrame( dev->m_frame );
        if ( !wxPLGetApp().FrameCount() )
            wxUninitialize();
    }

    delete dev;
    pls->dev = NULL; // since in plcore.c pls->dev is free_mem'd
}


//--------------------------------------------------------------------------
//  void plD_state_wxwidgets( PLStream *pls, PLINT op )
//
//  Handler for several state codes. Here we take care of setting the width
//  and color of the pen.
//--------------------------------------------------------------------------
void plD_state_wxwidgets( PLStream *pls, PLINT op )
{
    // Log_Verbose( "plD_state_wxwidgets(op=%d)", op );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    switch ( op )
    {
    case PLSTATE_WIDTH: // 1
        if ( dev->ready )
            dev->SetWidth( pls );
        else
            dev->plstate_width = true;
        break;

    case PLSTATE_COLOR0: // 2
        if ( dev->ready )
            dev->SetColor0( pls );
        else
            dev->plstate_color0 = true;
        break;

    case PLSTATE_COLOR1: // 3
        if ( dev->ready )
            dev->SetColor1( pls );
        else
            dev->plstate_color1 = true;
        break;

    //For all these state changes we don't need to do anything
    //and if they occur before/during initialization we don't
    //want to call install_buffer
    case PLSTATE_FILL:
    case PLSTATE_CMAP0:
    case PLSTATE_CMAP1:
    case PLSTATE_CHR:
    case PLSTATE_SYM:
        break;

    default:
        if ( !( dev->ready ) )
            install_buffer( pls );
    }
}


//--------------------------------------------------------------------------
//  void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
//
//  Handler for several escape codes. Here we take care of filled polygons,
//  XOR or copy mode, initialize device (install dc from outside), and if
//  there is freetype support, rerendering of text.
//--------------------------------------------------------------------------
void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
{
    // Log_Verbose( "plD_esc_wxwidgets(op=%d, ptr=%x)", op, ptr );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    switch ( op )
    {
    case PLESC_FILL:
        fill_polygon( pls );
        break;

    case PLESC_XORMOD:
        // switch between wxXOR and wxCOPY
        // if( dev->ready ) {
        //                  if( dev->m_dc->GetLogicalFunction() == wxCOPY )
        //                          dev->m_dc->SetLogicalFunction( wxXOR );
        //                  else if( dev->m_dc->GetLogicalFunction() == wxXOR )
        //                          dev->m_dc->SetLogicalFunction( wxCOPY );
        //          }
        break;

    case PLESC_DEVINIT:
        dev->SetExternalBuffer( ptr );

        // replay begin of page call and state settings
        plD_bop_wxwidgets( pls );
        break;

    case PLESC_HAS_TEXT:
        if ( !( dev->ready ) )
            install_buffer( pls );

        if ( dev->freetype )
        {
#ifdef PL_HAVE_FREETYPE
            plD_render_freetype_text( pls, (EscText *) ptr );
#endif
        }
        else
            dev->ProcessString( pls, (EscText *) ptr );
        break;

    case PLESC_RESIZE:
    {
        wxSize* size = (wxSize *) ptr;
        wx_set_size( pls, size->GetWidth(), size->GetHeight() );
    }
    break;

    case PLESC_CLEAR:
        if ( !( dev->ready ) )
            install_buffer( pls );
        // Since the plot is updated only every MAX_COMCOUNT commands (usually 5000)
        //       before we clear the screen we need to show the plot at least once :)
        if ( !( dev->resizing ) && dev->ownGUI )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
        dev->ClearBackground( pls->cmap0[0].r, pls->cmap0[0].g, pls->cmap0[0].b,
            pls->sppxmi, pls->sppymi, pls->sppxma, pls->sppyma );
        break;

    case PLESC_FLUSH:        // forced update of the window
        if ( !( dev->resizing ) && dev->ownGUI )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
        break;

    case PLESC_GETC:
        if ( dev->ownGUI )
            GetCursorCmd( pls, (PLGraphicsIn *) ptr );
        break;

    case PLESC_GETBACKEND:
        *( (int *) ptr ) = dev->backend;
        break;

    default:
        break;
    }
}


//--------------------------------------------------------------------------
//  static void fill_polygon( PLStream *pls )
//
//  Fill polygon described in points pls->dev_x[] and pls->dev_y[].
//--------------------------------------------------------------------------
static void fill_polygon( PLStream *pls )
{
    // Log_Verbose( "fill_polygon(), npts=%d, x[0]=%d, y[0]=%d", pls->dev_npts, pls->dev_y[0], pls->dev_y[0] );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    if ( !( dev->ready ) )
        install_buffer( pls );

    dev->FillPolygon( pls );

    if ( !( dev->resizing ) && dev->ownGUI )
    {
        dev->comcount += 10;
        if ( dev->comcount > MAX_COMCOUNT )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
    }
}


//--------------------------------------------------------------------------
//  void wx_set_size( PLStream* pls, int width, int height )
//
//  Adds a dc to the stream. The associated device is attached to the canvas
//  as the property "dev".
//--------------------------------------------------------------------------
void wx_set_size( PLStream* pls, int width, int height )
{
    // TODO: buffer must be resized here or in wxplotstream
    // Log_Verbose( "wx_set_size()" );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    // set new size and scale parameters
    dev->width  = width;
    dev->height = height;
    dev->scalex = (PLFLT) ( dev->xmax - dev->xmin ) / dev->width;
    dev->scaley = (PLFLT) ( dev->ymax - dev->ymin ) / dev->height;

    // recalculate the dpi used in calculation of fontsize
    pls->xdpi = VIRTUAL_PIXELS_PER_IN / dev->scalex;
    pls->ydpi = VIRTUAL_PIXELS_PER_IN / dev->scaley;

    // clear background if we have a dc, since it's invalid (TODO: why, since in bop
    // it must be cleared anyway?)
    if ( dev->ready )
    {
        PLINT bgr, bgg, bgb;                  // red, green, blue
        plgcolbg( &bgr, &bgg, &bgb );         // get background color information

        dev->CreateCanvas();
        dev->ClearBackground( bgr, bgg, bgb );
    }

    // freetype parameters must also be changed
#ifdef PL_HAVE_FREETYPE
    if ( dev->freetype )
    {
        FT_Data *FT = (FT_Data *) pls->FT;
        FT->scalex = dev->scalex;
        FT->scaley = dev->scaley;
        FT->ymax   = dev->height;
    }
#endif
}


//--------------------------------------------------------------------------
//  int plD_errorexithandler_wxwidgets( const char *errormessage )
//
//  If an PLplot error occurs, this function shows a dialog regarding
//  this error and than exits.
//--------------------------------------------------------------------------
int plD_errorexithandler_wxwidgets( const char *errormessage )
{
    if ( errormessage[0] )
    {
        wxMessageDialog dialog( 0, wxString( errormessage, *wxConvCurrent ), wxString( "wxWidgets PLplot App error", *wxConvCurrent ), wxOK | wxICON_ERROR );
        dialog.ShowModal();
    }

    return 0;
}


//--------------------------------------------------------------------------
//  void plD_erroraborthandler_wxwidgets( const char *errormessage )
//
//  If PLplot aborts, this function shows a dialog regarding
//  this error.
//--------------------------------------------------------------------------
void plD_erroraborthandler_wxwidgets( const char *errormessage )
{
    if ( errormessage[0] )
    {
        wxMessageDialog dialog( 0, ( wxString( errormessage, *wxConvCurrent ) + wxString( " aborting operation...", *wxConvCurrent ) ), wxString( "wxWidgets PLplot App abort", *wxConvCurrent ), wxOK | wxICON_ERROR );
        dialog.ShowModal();
    }
}




#ifdef PL_HAVE_FREETYPE

//--------------------------------------------------------------------------
//  static void plD_pixel_wxwidgets( PLStream *pls, short x, short y )
//
//  callback function, of type "plD_pixel_fp", which specifies how a single
//  pixel is set in the current colour.
//--------------------------------------------------------------------------
static void plD_pixel_wxwidgets( PLStream *pls, short x, short y )
{
    // Log_Verbose( "plD_pixel_wxwidgets" );

    wxPLDevBase *dev = (wxPLDevBase *) pls->dev;

    if ( !( dev->ready ) )
        install_buffer( pls );

    dev->PutPixel( x, y );

    if ( !( dev->resizing ) && dev->ownGUI )
    {
        dev->comcount++;
        if ( dev->comcount > MAX_COMCOUNT )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
    }
}


//--------------------------------------------------------------------------
//  static void plD_pixel_wxwidgets( PLStream *pls, short x, short y )
//
//  callback function, of type "plD_pixel_fp", which specifies how a single
//  pixel is set in the current colour.
//--------------------------------------------------------------------------
static void plD_set_pixel_wxwidgets( PLStream *pls, short x, short y, PLINT colour )
{
    // Log_Verbose( "plD_set_pixel_wxwidgets" );

    wxPLDevBase *dev = (wxPLDevBase *) pls->dev;

    if ( !( dev->ready ) )
        install_buffer( pls );

    dev->PutPixel( x, y, colour );

    if ( !( dev->resizing ) && dev->ownGUI )
    {
        dev->comcount++;
        if ( dev->comcount > MAX_COMCOUNT )
        {
            wxRunApp( pls, true );
            dev->comcount = 0;
        }
    }
}


//--------------------------------------------------------------------------
//  void plD_read_pixel_wxwidgets (PLStream *pls, short x, short y)
//
//  callback function, of type "plD_pixel_fp", which specifies how a single
//  pixel is read.
//--------------------------------------------------------------------------
static PLINT plD_read_pixel_wxwidgets( PLStream *pls, short x, short y )
{
    // Log_Verbose( "plD_read_pixel_wxwidgets" );

    wxPLDevBase *dev = (wxPLDevBase *) pls->dev;

    if ( !( dev->ready ) )
        install_buffer( pls );

    return dev->GetPixel( x, y );
}


//--------------------------------------------------------------------------
//  void init_freetype_lv1 (PLStream *pls)
//
//  "level 1" initialisation of the freetype library.
//  "Level 1" initialisation calls plD_FreeType_init(pls) which allocates
//  memory to the pls->FT structure, then sets up the pixel callback
//  function.
//--------------------------------------------------------------------------
static void init_freetype_lv1( PLStream *pls )
{
    // Log_Verbose( "init_freetype_lv1" );

    plD_FreeType_init( pls );

    FT_Data *FT = (FT_Data *) pls->FT;
    FT->pixel = (plD_pixel_fp) plD_pixel_wxwidgets;

    //
    //  See if we have a 24 bit device (or better), in which case
    //  we can use the better antialising.
    //
    // the bitmap we are using in the antialized case has always
    // 32 bit depth
    FT->BLENDED_ANTIALIASING = 1;
    FT->read_pixel           = (plD_read_pixel_fp) plD_read_pixel_wxwidgets;
    FT->set_pixel            = (plD_set_pixel_fp) plD_set_pixel_wxwidgets;
}


//--------------------------------------------------------------------------
//  void init_freetype_lv2 (PLStream *pls)
//
//  "Level 2" initialisation of the freetype library.
//  "Level 2" fills in a few setting that aren't public until after the
//  graphics sub-syetm has been initialised.
//  The "level 2" initialisation fills in a few things that are defined
//  later in the initialisation process for the GD driver.
//
//  FT->scale is a scaling factor to convert co-ordinates. This is used by
//  the GD and other drivers to scale back a larger virtual page and this
//  eliminate the "hidden line removal bug". Set it to 1 if your device
//  doesn't have scaling.
//
//  Some coordinate systems have zero on the bottom, others have zero on
//  the top. Freetype does it one way, and most everything else does it the
//  other. To make sure everything is working ok, we have to "flip" the
//  coordinates, and to do this we need to know how big in the Y dimension
//  the page is, and whether we have to invert the page or leave it alone.
//
//  FT->ymax specifies the size of the page FT->invert_y=1 tells us to
//  invert the y-coordinates, FT->invert_y=0 will not invert the
//  coordinates.
//--------------------------------------------------------------------------

static void init_freetype_lv2( PLStream *pls )
{
    // Log_Verbose( "init_freetype_lv2" );

    wxPLDevBase *dev = (wxPLDevBase *) pls->dev;
    FT_Data     *FT  = (FT_Data *) pls->FT;

    FT->scalex      = dev->scalex;
    FT->scaley      = dev->scaley;
    FT->ymax        = dev->height;
    FT->invert_y    = 1;
    FT->smooth_text = 0;

    if ( ( FT->want_smooth_text == 1 ) && ( FT->BLENDED_ANTIALIASING == 0 ) ) // do we want to at least *try* for smoothing ?
    {
        FT->ncol0_org   = pls->ncol0;                                         // save a copy of the original size of ncol0
        FT->ncol0_xtra  = 16777216 - ( pls->ncol1 + pls->ncol0 );             // work out how many free slots we have
        FT->ncol0_width = FT->ncol0_xtra / ( pls->ncol0 - 1 );                // find out how many different shades of anti-aliasing we can do
        if ( FT->ncol0_width > 4 )                                            // are there enough colour slots free for text smoothing ?
        {
            if ( FT->ncol0_width > max_number_of_grey_levels_used_in_text_smoothing )
                FT->ncol0_width = max_number_of_grey_levels_used_in_text_smoothing;           // set a maximum number of shades
            plscmap0n( FT->ncol0_org + ( FT->ncol0_width * pls->ncol0 ) );                    // redefine the size of cmap0
            // the level manipulations are to turn off the plP_state(PLSTATE_CMAP0)
            // call in plscmap0 which (a) leads to segfaults since the GD image is
            // not defined at this point and (b) would be inefficient in any case since
            // setcmap is always called later (see plD_bop_png) to update the driver
            // color palette to be consistent with cmap0.
            {
                PLINT level_save;
                level_save = pls->level;
                pls->level = 0;
                pl_set_extended_cmap0( pls, FT->ncol0_width, FT->ncol0_org ); // call the function to add the extra cmap0 entries and calculate stuff
                pls->level = level_save;
            }
            FT->smooth_text = 1; // Yippee ! We had success setting up the extended cmap0
        }
        else
            plwarn( "Insufficient colour slots available in CMAP0 to do text smoothing." );
    }
    else if ( ( FT->want_smooth_text == 1 ) && ( FT->BLENDED_ANTIALIASING == 1 ) )    // If we have a truecolour device, we wont even bother trying to change the palette
    {
        FT->smooth_text = 1;
    }
}

#endif


//--------------------------------------------------------------------------
// GetCursorCmd()
//
// Waits for a graphics input event and returns coordinates.
//--------------------------------------------------------------------------
static void GetCursorCmd( PLStream* pls, PLGraphicsIn* ptr )
{
    // Log_Verbose( "GetCursorCmd" );

    wxPLDevBase  *dev = (wxPLDevBase *) pls->dev;
    PLGraphicsIn *gin = &( dev->gin );

    // Initialize
    plGinInit( gin );
    dev->locate_mode = LOCATE_INVOKED_VIA_API;
    dev->draw_xhair  = true;

    // Run event loop until a point is selected
    wxRunApp( pls, false );

    *ptr = *gin;
    if ( dev->locate_mode )
    {
        dev->locate_mode = 0;
        dev->draw_xhair  = false;
    }
}




//--------------------------------------------------------------------------
//  This part includes wxWidgets specific functions, which allow to
//  open a window from the command line, if needed.
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//  void install_buffer( PLStream *pls )
//
//  If this driver is called from a command line executable (and not
//  from within a wxWidgets program), this function prepares a DC and a
//  bitmap to plot into.
//--------------------------------------------------------------------------
static void install_buffer( PLStream *pls )
{
    // Log_Verbose( "install_buffer" );

    wxPLDevBase * dev   = (wxPLDevBase *) pls->dev;
    static bool initApp = false;

    if ( !initApp )
    {
        // this hack enables to have a GUI on Mac OSX even if the
        // program was called from the command line (and isn't a bundle)
#ifdef __WXMAC__
        ProcessSerialNumber psn;

        GetCurrentProcess( &psn );
        CPSEnableForegroundOperation( &psn );
        SetFrontProcess( &psn );
#endif

        // initialize wxWidgets
        wxInitialize();
        wxLog::GetActiveTarget();
        wxTRY {
            wxPLGetApp().CallOnInit();
        }
        wxCATCH_ALL( wxPLGetApp().OnUnhandledException(); plexit( "Can't init wxWidgets!" ); )
        initApp = true;
    }


    wxString title( pls->plwindow, *wxConvCurrent );
    switch ( dev->backend )
    {
    case wxBACKEND_DC:
        title += wxT( " - wxWidgets (basic)" );
        break;
    case wxBACKEND_GC:
        title += wxT( " - wxWidgets (wxGC)" );
        break;
    case wxBACKEND_AGG:
        title += wxT( " - wxWidgets (AGG)" );
        break;
    default:
        break;
    }
    dev->m_frame = new wxPLplotFrame( title, pls );
    wxPLGetApp().AddFrame( dev->m_frame );

    // set size and position of window
    dev->m_frame->SetClientSize( dev->width, dev->height );
    if ( dev->xpos != 0 || dev->ypos != 0 )
        dev->m_frame->SetSize( dev->xpos, dev->ypos,
            wxDefaultCoord, wxDefaultCoord,
            wxSIZE_USE_EXISTING );

    if ( dev->showGUI )
    {
        dev->m_frame->Show( true );
        dev->m_frame->Raise();
    }
    else
        dev->m_frame->Show( false );

    // get a DC and a bitmap or an imagebuffer
    dev->ownGUI    = true;
    dev->bm_width  = dev->width;
    dev->bm_height = dev->height;
    dev->CreateCanvas();
    dev->ready = true;

    // Set wx error handler for various errors in plplot
    plsexit( plD_errorexithandler_wxwidgets );
    plsabort( plD_erroraborthandler_wxwidgets );

    // replay command we may have missed
    plD_bop_wxwidgets( pls );
}


//--------------------------------------------------------------------------
//  void wxRunApp( PLStream *pls, bool runonce )
//
//  This is a hacked wxEntry-function, so that wxUninitialize is not
//  called twice. Here we actually start the wxApplication.
//--------------------------------------------------------------------------
static void wxRunApp( PLStream *pls, bool runonce )
{
    // Log_Verbose( "wxRunApp" );

    wxPLDevBase* dev = (wxPLDevBase *) pls->dev;

    dev->waiting = true;
    wxTRY
    {
        class CallOnExit
        {
public:
            // only call OnExit if exit is true (i.e. due an exception)
            ~CallOnExit() { if ( exit ) wxPLGetApp().OnExit();}
            bool exit;
        } callOnExit;

        callOnExit.exit = true;
        wxPLGetApp().SetAdvanceFlag( runonce );
        wxPLGetApp().SetRefreshFlag();

        // add an idle event is necessary for Linux (wxGTK2)
        // but not for Windows, but it doesn't harm
        wxIdleEvent event;
        wxPLGetApp().AddPendingEvent( event );
        wxPLGetApp().OnRun();           // start wxWidgets application
        callOnExit.exit = false;
    }
    wxCATCH_ALL( wxPLGetApp().OnUnhandledException(); plexit( "Problem running wxWidgets!" ); )

    if ( dev->exit )
    {
        wxPLGetApp().OnExit();
        plexit( "" );
    }

    dev->waiting = false;
}
