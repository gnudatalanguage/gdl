// Copyright (C) 2015  Phil Rosenberg
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

#include "wxwidgets.h"

bool g_weInitializedWx = false;

#ifdef __WXMAC__
        #include <Carbon/Carbon.h>
extern "C" { void CPSEnableForegroundOperation( ProcessSerialNumber* psn ); }
#endif


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
    (void) fmt;        // Cast to void to silence compiler warnings about unused parameter
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
    (void) fmt;        // Cast to void to silence compiler warnings about unused parameter
#endif
}

//--------------------------------------------------------------------------
//  In the following you'll find the driver functions which are
//  needed by the plplot core.
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
    pdt->pl_wait     = (plD_wait_fp) plD_wait_wxwidgets;
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
    PLPLOT_wxLogDebug( "plD_init_wxwidgets(): enter" );

    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( device )
            throw( "plD_init_wxwidgets called when a initialization has already occurred." );

        //initialise wxWidgets - this is required in order to allow some wxWidgets functions to
        //be called from within the driver when the user isn't passing a wxDC in. See e.g
        //http://stackoverflow.com/questions/208373
        if ( !wxTheApp )
        {
            wxApp::SetInstance( new wxApp() );
            int argc = 0;
            char* argv[1];
            g_weInitializedWx = wxEntryStart( argc, (char **) NULL );
            if ( !g_weInitializedWx )
                throw( "plD_init_wxWidgets could not initialise wxWidgets" );
        }
        else
            g_weInitializedWx = false;

        // default options
        static PLINT text    = 1;
        static PLINT hrshsym = 0;
        static char  *mfo    = NULL;

        DrvOpt       wx_options[] = {
            { "hrshsym", DRV_INT, &hrshsym, "Use Hershey symbol set (hrshsym=0|1)" },
            { "text",    DRV_INT, &text,    "Use own text routines (text=0|1)"     },
            { "mfo",     DRV_STR, &mfo,     "output metafile"                      },
            { NULL,      DRV_INT, NULL,     NULL                                   }
        };

        // Check for and set up driver options
        plParseDrvOpts( wx_options );

        // by default the own text routines are used for wxDC
        if ( text == -1 )
            text = 0;

        // create the new device
        device = new wxPLDevice( pls, mfo, text, hrshsym );

        // If portrait mode, apply a rotation and set freeaspect
        if ( pls->portrait )
        {
            plsdiori( (PLFLT) ( 4 - ORIENTATION ) );
            pls->freeaspect = 1;
        }

        //indicate that we support getting string length
        pls->has_string_length = 1;

        //indicate that we support xor rendering
        pls->dev_xor = 1;
    }
    catch ( const char *message )
    {
        plabort( message );
        plabort( "error in plD_init_wxwidgets." );
        pls->dev = NULL;
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_init_wxwidgets." );
    }
    PLPLOT_wxLogDebug( "plD_init_wxwidgets(): leave" );
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
    PLPLOT_wxLogDebug( "plD_init_wxpng()" );

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
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            throw( "plD_line_wxwidgets called before initialization." );
        device->DrawLine( x1a, y1a, x2a, y2a );
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_line_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_line_wxwidgets." );
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
    // PLPLOT_wxLogDebug( "plD_polyline_wxwidgets()" );
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            throw( "plD_polyline_wxwidgets called before initialization." );
        device->DrawPolyline( xa, ya, npts );
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_polyline_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_polyline_wxwidgets." );
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
    // PLPLOT_wxLogDebug( "plD_eop_wxwidgets()" );
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            throw( "plD_eop_wxwidgets called before initialization." );
        if ( pls->nopause )
            device->EndPage( pls );
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_eop_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_eop_wxwidgets." );
    }
}


//--------------------------------------------------------------------------
//  void plD_wait_wxwidgets( PLStream *pls )
//
//  Wait for user input. This command is ignored if we have the plot embedded in a
//  wxWidgets application, otherwise the application created by the device
//  takes over.
//--------------------------------------------------------------------------
void plD_wait_wxwidgets( PLStream *pls )
{
    // PLPLOT_wxLogDebug( "plD_wait_wxwidgets()" );
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            throw( "plD_wait_wxwidgets called before initialization." );
        device->EndPage( pls );
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_wait_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_wait_wxwidgets." );
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
    // PLPLOT_wxLogDebug( "plD_bop_wxwidgets()" );
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            throw( "plD_bop_wxwidgets called before initialization." );
        device->BeginPage( pls );
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_bop_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_bop_wxwidgets." );
    }
}

//--------------------------------------------------------------------------
//  void plD_tidy_wxwidgets( PLStream *pls )
//
//  This function is called, if all plots are done.
//--------------------------------------------------------------------------
void plD_tidy_wxwidgets( PLStream *pls )
{
    // PLPLOT_wxLogDebug( "plD_tidy_wxwidgets()" );
    if ( !pls->dev )
        return;
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( device )
        {
            device->PreDestructorTidy( pls );
            delete device;
        }
        pls->dev = NULL;         //so it doesn't get freed elsewhere
        if ( g_weInitializedWx )
            wxEntryCleanup();
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_tidy_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_tidy_wxwidgets." );
    }
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
    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            return;             //we can call state functions before initialization so just return
        switch ( op )
        {
        case PLSTATE_WIDTH:         // 1
            device->SetWidth( pls );
            break;

        case PLSTATE_COLOR0:         // 2
            device->SetColor( pls );
            break;

        case PLSTATE_COLOR1:         // 3
            device->SetColor( pls );
            break;
        }
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_state_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_state_wxwidgets." );
    }
}

//--------------------------------------------------------------------------
//  void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
//
//  Handler for several escape codes. Here we take care of filled polygons,
//  XOR or copy mode, initialize device (install dc from outside), and if
//  there is freetype support, re-rendering of text.
//--------------------------------------------------------------------------
void plD_esc_wxwidgets( PLStream *pls, PLINT op, void *ptr )
{
    // Log_Verbose( "plD_esc_wxwidgets(op=%d, ptr=%x)", op, ptr );

    if ( !pls->dev )
        return;

    try
    {
        wxPLDevice *device = (wxPLDevice *) pls->dev;
        if ( !device )
            throw( "plD_esc_wxwidgets called before initialization." );
        switch ( op )
        {
        case PLESC_FILL:
            device->FillPolygon( pls );
            break;

        case PLESC_XORMOD:
            device->SetXorMode( *( (PLINT *) ptr ) == 0 ? false : true );
            break;

        case PLESC_DEVINIT:
            device->SetDC( pls, (wxDC *) ptr );
            break;

        case PLESC_HAS_TEXT:
            device->drawText( pls, (EscText *) ptr );
            break;

        case PLESC_RESIZE:
        {
            wxSize* size = (wxSize *) ptr;
            device->SetSize( pls, size->GetWidth(), size->GetHeight() );
        }
        break;

        case PLESC_CLEAR:
            device->ClearBackground( pls, pls->sppxmi, pls->sppymi,
                pls->sppxma, pls->sppyma );
            break;

        case PLESC_FLUSH:                // forced update of the window
            device->Flush( pls );
            break;

        case PLESC_GETC:
            device->Locate( pls, (PLGraphicsIn *) ptr );
            break;

        case PLESC_FIXASPECT:
            device->FixAspectRatio( *( (bool *) ptr ) );
            break;

        default:
            break;
        }
    }
    catch ( const char* message )
    {
        plabort( message );
        plabort( "error in plD_tidy_wxwidgets." );
    }
    catch ( ... )
    {
        plabort( "unknown error in plD_tidy_wxwidgets." );
    }
}
