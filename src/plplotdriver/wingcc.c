//      PLplot WIN32 under GCC device driver.
//
// Copyright (C) 2004  Andrew Roach
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
//
#include "plDevs.h"

#ifdef PLD_wingcc

#include <string.h>
#include <windows.h>
#if !defined ( __CYGWIN__ )
#include <tchar.h>
#else
#include <winnt.h>
#define _T( a )    __TEXT( a )
#endif
#ifdef _WIN64
#define GWL_USERDATA    GWLP_USERDATA
#define GCL_HCURSOR     GCLP_HCURSOR
#endif

#include "plplotP.h"
#include "drivers.h"
#include "plevent.h"

#ifdef PL_HAVE_FREETYPE

//
//  Freetype support has been added to the wingcc driver using the
//  plfreetype.c module, and implemented as a driver-specific optional extra
//  invoked via the -drvopt command line toggle. It uses the
//  "PLESC_HAS_TEXT" command for rendering within the driver.
//
//  Freetype support is turned on/off at compile time by defining
//  "PL_HAVE_FREETYPE".
//
//  To give the user some level of control over the fonts that are used,
//  environmental variables can be set to over-ride the definitions used by
//  the five default plplot fonts.
//
//  Freetype rendering is used with the command line "-drvopt text".
//  Anti-aliased fonts can be used by issuing "-drvopt text,smooth"
//

#include "plfreetype.h"

#ifndef max_number_of_grey_levels_used_in_text_smoothing
#define max_number_of_grey_levels_used_in_text_smoothing    64
#endif

#endif


// Device info

PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_wingcc = "wingcc:Win32 (GCC):1:wingcc:9:wingcc\n";

// Struct to hold device-specific info.
// NOTE:  This struct is being used externally by the GNU Data Language
// project.  They have copied the struct definition into their code
// in order to access the hwnd, hdc, and waiting members.  Until an
// alternative method can be devised, changes to this struct should be
// avoided--at a minimum new member should be placed after the waiting
// member, which should avoid breaking GDL (barring a memory alignment
// optimization by the compiler).
typedef struct
{
    PLFLT scale;                     // scaling factor to "blow up" to the "virtual" page in removing hidden lines
    PLINT width;                     // Window width (which can change)
    PLINT height;                    // Window Height

    PLFLT PRNT_scale;
    PLINT PRNT_width;
    PLINT PRNT_height;

    char  FT_smooth_text;
//
// WIN32 API variables
//

    COLORREF          colour;                       // Current Colour
    COLORREF          oldcolour;                    // Used for high-speed background erasing
    MSG               msg;                          // A Win32 message structure.
    WNDCLASSEX        wndclass;                     // An extended window class structure.
    HWND              hwnd;                         // Handle for the main window.
    HPEN              pen;                          // Windows pen used for drawing
    HDC               hdc;                          // Driver Context
    HDC               hdc2;                         // Driver Context II - used for Blitting
    HDC               SCRN_hdc;                     // The screen's context
    HDC               PRNT_hdc;                     // used for printing
    PAINTSTRUCT       ps;                           // used to paint the client area of a window owned by that application
    RECT              rect;                         // defines the coordinates of the upper-left and lower-right corners of a rectangle
    RECT              oldrect;                      // used for re-sizing comparisons
    RECT              paintrect;                    // used for painting etc...
    HBRUSH            fillbrush;                    // brush used for fills
    HCURSOR           cursor;                       // Current windows cursor for this window
    HBITMAP           bitmap;                       // Bitmap of current display; used for fast redraws via blitting
    HGDIOBJ           oldobject;                    // Used for tracking objects probably not really needed but
    HMENU             PopupMenu;

    PLINT             draw_mode;
    char              truecolour;      // Flag to indicate 24 bit mode
    char              waiting;         // Flag to indicate drawing is done, and it is waiting;
                                       // we only do a windows redraw if plplot is plotting
    char              enterresize;     // Used to keep track of reszing messages from windows
    char              already_erased;  // Used to track first and only first backgroudn erases

    struct wingcc_Dev *push;           // A copy of the entire structure used when printing
                                       // We push and pop it off a virtual stack
} wingcc_Dev;


void plD_dispatch_init_wingcc( PLDispatchTable *pdt );

void plD_init_wingcc( PLStream * );
void plD_line_wingcc( PLStream *, short, short, short, short );
void plD_polyline_wingcc( PLStream *, short *, short *, PLINT );
void plD_eop_wingcc( PLStream * );
void plD_bop_wingcc( PLStream * );
void plD_tidy_wingcc( PLStream * );
void plD_wait_wingcc( PLStream * );
void plD_state_wingcc( PLStream *, PLINT );
void plD_esc_wingcc( PLStream *, PLINT, void * );

#ifdef PL_HAVE_FREETYPE

static void plD_pixel_wingcc( PLStream *pls, short x, short y );
static void plD_pixelV_wingcc( PLStream *pls, short x, short y );
static PLINT plD_read_pixel_wingcc( PLStream *pls, short x, short y );
static void plD_set_pixel_wingcc( PLStream *pls, short x, short y, PLINT colour );
static void plD_set_pixelV_wingcc( PLStream *pls, short x, short y, PLINT colour );
static void init_freetype_lv1( PLStream *pls );
static void init_freetype_lv2( PLStream *pls );

#endif


//--------------------------------------------------------------------------
//  Local Function definitions and function-like defines
//--------------------------------------------------------------------------

static int GetRegValue( TCHAR *key_name, TCHAR *key_word, char *buffer, int size );
static int SetRegValue( TCHAR *key_name, TCHAR *key_word, char *buffer, int dwType, int size );
static void Resize( PLStream *pls );
static void plD_fill_polygon_wingcc( PLStream *pls );
static void CopySCRtoBMP( PLStream *pls );
static void PrintPage( PLStream *pls );
static void UpdatePageMetrics( PLStream *pls, char flag );

#define SetRegStringValue( a, b, c )       SetRegValue( a, b, c, REG_SZ, strlen( c ) + 1 )
#define SetRegBinaryValue( a, b, c, d )    SetRegValue( a, b, (char *) c, REG_BINARY, d )
#define SetRegIntValue( a, b, c )          SetRegValue( a, b, (char *) c, REG_DWORD, 4 )
#define GetRegStringValue( a, b, c, d )    GetRegValue( a, b, c, d )
#define GetRegIntValue( a, b, c )          GetRegValue( a, b, (char *) c, 4 )
#define GetRegBinaryValue( a, b, c, d )    GetRegValue( a, b, (char *) c, d )

//--------------------------------------------------------------------------
//  Some debugging macros
//--------------------------------------------------------------------------

#if defined ( _MSC_VER )
  #define Debug( a )           do { if ( pls->debug ) { fprintf( stderr, ( a ) ); } } while ( 0 )
  #define Debug2( a, b )       do { if ( pls->debug ) { fprintf( stderr, ( a ), ( b ) ); } } while ( 0 )
  #define Debug3( a, b, c )    do { if ( pls->debug ) { fprintf( stderr, ( a ), ( b ), ( c ) ); } } while ( 0 )
#elif defined ( __BORLANDC__ )
  #define Debug     if ( pls->debug ) printf
  #define Debug2    if ( pls->debug ) printf
  #define Debug3    if ( pls->debug ) printf
#else
  #define Verbose( ... )     do { if ( pls->verbose ) { fprintf( stderr, __VA_ARGS__ ); } } while ( 0 )
  #define Debug( ... )       do { if ( pls->debug ) { fprintf( stderr, __VA_ARGS__ ); } } while ( 0 )
  #define Debug2( ... )      do { if ( pls->debug ) { fprintf( stderr, __VA_ARGS__ ); } } while ( 0 )
  #define Debug3( ... )      do { if ( pls->debug ) { fprintf( stderr, __VA_ARGS__ ); } } while ( 0 )
#endif

#define ReportWinError()     do {                                                 \
        LPVOID lpMsgBuf;                                                          \
        FormatMessage(                                                            \
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,                  \
    NULL, GetLastError(),                                                         \
    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPTSTR) &lpMsgBuf, 0, NULL );   \
        MessageBox( NULL, lpMsgBuf, "GetLastError", MB_OK | MB_ICONINFORMATION ); \
        LocalFree( lpMsgBuf ); } while ( 0 )

#define CrossHairCursor()    do {                                   \
        dev->cursor = LoadCursor( NULL, IDC_CROSS );                \
        SetClassLong( dev->hwnd, GCL_HCURSOR, (long) dev->cursor ); \
        SetCursor( dev->cursor ); } while ( 0 )

#define NormalCursor()       do {                                          \
        dev->cursor = LoadCursor( NULL, IDC_ARROW );                       \
        SetClassLongPtr( dev->hwnd, GCL_HCURSOR, (LONG_PTR) dev->cursor ); \
        SetCursor( dev->cursor ); } while ( 0 )

#define BusyCursor()         do {                                          \
        dev->cursor = LoadCursor( NULL, IDC_WAIT );                        \
        SetClassLongPtr( dev->hwnd, GCL_HCURSOR, (LONG_PTR) dev->cursor ); \
        SetCursor( dev->cursor ); } while ( 0 )

#define PopupPrint       0x08A1
#define PopupNextPage    0x08A2
#define PopupQuit        0x08A3


void plD_dispatch_init_wingcc( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "Win32 GCC device";
    pdt->pl_DevName = "wingcc";
#endif
    pdt->pl_type     = plDevType_Interactive;
    pdt->pl_seq      = 9;
    pdt->pl_init     = (plD_init_fp) plD_init_wingcc;
    pdt->pl_line     = (plD_line_fp) plD_line_wingcc;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_wingcc;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_wingcc;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_wingcc;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_wingcc;
    pdt->pl_state    = (plD_state_fp) plD_state_wingcc;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_wingcc;
    pdt->pl_wait     = (plD_wait_fp) plD_wait_wingcc;
}

static TCHAR* szWndClass = _T( "PlplotWin" );


//--------------------------------------------------------------------------
// This is the window function for the plot window. Whenever a message is
// dispatched using DispatchMessage (or sent with SendMessage) this function
// gets called with the contents of the message.
//--------------------------------------------------------------------------

LRESULT CALLBACK PlplotWndProc( HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam )
{
    PLStream   *pls = NULL;
    wingcc_Dev *dev = NULL;

//
// The window carries a 32bit user defined pointer which points to the
// plplot stream (pls). This is used for tracking the window.
// Unfortunately, this is "attached" to the window AFTER it is created
// so we can not initialise PLStream or wingcc_Dev "blindly" because
// they may not yet have been initialised.
// WM_CREATE is called before we get to initialise those variables, so
// we wont try to set them.
//

    if ( nMsg == WM_CREATE )
    {
        return ( 0 );
    }
    else
    {
#ifndef _WIN64
#undef GetWindowLongPtr
#define GetWindowLongPtr    GetWindowLong
#endif
        pls = (PLStream *) GetWindowLongPtr( hwnd, GWL_USERDATA ); // Try to get the address to pls for this window
        if ( pls )                                                 // If we got it, then we will initialise this windows plplot private data area
        {
            dev = (wingcc_Dev *) pls->dev;
        }
    }

//
// Process the windows messages
//
// Everything except WM_CREATE is done here and it is generally hoped that
// pls and dev are defined already by this stage.
// That will be true MOST of the time. Some times WM_PAINT will be called
// before we get to initialise the user data area of the window with the
// pointer to the windows plplot stream
//

    switch ( nMsg )
    {
    case WM_DESTROY:
        if ( dev )
            Debug( "WM_DESTROY\t" );
        PostQuitMessage( 0 );
        return ( 0 );
        break;

    case WM_PAINT:
        if ( dev )
        {
            Debug( "WM_PAINT\t" );
            if ( GetUpdateRect( dev->hwnd, &dev->paintrect, TRUE ) )
            {
                BusyCursor();
                BeginPaint( dev->hwnd, &dev->ps );

                if ( ( dev->waiting == 1 ) && ( dev->already_erased == 1 ) )
                {
                    Debug( "Remaking\t" );

                    if ( dev->ps.fErase )
                    {
                        dev->oldcolour = SetBkColor( dev->hdc, RGB( pls->cmap0[0].r, pls->cmap0[0].g, pls->cmap0[0].b ) );
                        ExtTextOut( dev->hdc, 0, 0, ETO_OPAQUE, &dev->rect, _T( "" ), 0, 0 );
                        SetBkColor( dev->hdc, dev->oldcolour );
                    }

                    plRemakePlot( pls );
                    CopySCRtoBMP( pls );
                    dev->already_erased++;
                }
                else if ( ( dev->waiting == 1 ) && ( dev->already_erased == 2 ) )
                {
                    dev->oldobject = SelectObject( dev->hdc2, dev->bitmap );
                    BitBlt( dev->hdc, dev->paintrect.left, dev->paintrect.top,
                        dev->paintrect.right, dev->paintrect.bottom,
                        dev->hdc2, dev->paintrect.left, dev->paintrect.top, SRCCOPY );
                    SelectObject( dev->hdc2, dev->oldobject );
                }

                EndPaint( dev->hwnd, &dev->ps );
                NormalCursor();
                return ( 0 );
            }
        }
        return ( 1 );
        break;

    case WM_SIZE:
        if ( dev )
        {
            Debug( "WM_SIZE\t" );

            if ( dev->enterresize == 0 )
                Resize( pls );
        }
        return ( 0 );
        break;

    case WM_ENTERSIZEMOVE:
        if ( dev )
        {
            Debug( "WM_ENTERSIZEMOVE\t" );
            dev->enterresize = 1;
        }
        return ( 0 );
        break;


    case WM_EXITSIZEMOVE:
        if ( dev )
        {
            Debug( "WM_EXITSIZEMOVE\t" );
            Resize( pls );
            dev->enterresize = 0;     // Reset the variables that track sizing ops
        }
        return ( 0 );
        break;

    case WM_ERASEBKGND:

        if ( dev )
        {
            if ( dev->already_erased == 0 )
            {
                Debug( "WM_ERASEBKGND\t" );

                //
                //    This is a new "High Speed" way of filling in the background.
                //    supposidely this executes faster than creating a brush and
                //    filling a rectangle - go figure ?
                //

                dev->oldcolour = SetBkColor( dev->hdc, RGB( pls->cmap0[0].r, pls->cmap0[0].g, pls->cmap0[0].b ) );
                ExtTextOut( dev->hdc, 0, 0, ETO_OPAQUE, &dev->rect, _T( "" ), 0, 0 );
                SetBkColor( dev->hdc, dev->oldcolour );

                dev->already_erased = 1;
                return ( 1 );
            }
        }
        return ( 0 );
        break;

    case WM_COMMAND:
        if ( dev )
            Debug( "WM_COMMAND\t" );
        return ( 0 );
        break;
    }

    // If we don't handle a message completely we hand it to the system
    // provided default window function.
    return DefWindowProc( hwnd, nMsg, wParam, lParam );
}


//--------------------------------------------------------------------------
// plD_init_wingcc()
//
// Initialize device (terminal).
//--------------------------------------------------------------------------

void
plD_init_wingcc( PLStream *pls )
{
    wingcc_Dev *dev;

#ifdef PL_HAVE_FREETYPE
    static int freetype    = 0;
    static int smooth_text = 0;
    static int save_reg    = 0;
    FT_Data    *FT;

//
//  Variables used for reading the registary keys
//  might eventually add a user defined pallette here, but for now it just does freetype
//
    TCHAR key_name[]       = _T( "Software\\PLplot\\wingcc" );
    TCHAR Keyword_text[]   = _T( "freetype" );
    TCHAR Keyword_smooth[] = _T( "smooth" );
#endif

    DrvOpt wingcc_options[] = {
#ifdef PL_HAVE_FREETYPE
        { "text",   DRV_INT, &freetype,    "Use driver text (FreeType)"            },
        { "smooth", DRV_INT, &smooth_text, "Turn text smoothing on (1) or off (0)" },
        { "save",   DRV_INT, &save_reg,    "Save defaults to registary"            },

#endif
        { NULL,     DRV_INT, NULL,         NULL                                    }
    };

//
// Variable for storing the program name
//
    TCHAR *program;
    int   programlength;

// Allocate and initialize device-specific data

    if ( pls->dev != NULL )
        free( (void *) pls->dev );

    pls->dev = calloc( 1, (size_t) sizeof ( wingcc_Dev ) );
    if ( pls->dev == NULL )
        plexit( "plD_init_wingcc_Dev: Out of memory." );

    dev = (wingcc_Dev *) pls->dev;

    pls->icol0 = 1;                   // Set a fall back pen colour in case user doesn't

    pls->termin      = 1;             // interactive device
    pls->graphx      = GRAPHICS_MODE; //  No text mode for this driver (at least for now, might add a console window if I ever figure it out and have the inclination)
    pls->dev_fill0   = 1;             // driver can do solid area fills
    pls->dev_xor     = 1;             // driver supports xor mode
    pls->dev_clear   = 0;             // driver does not support clear - what is the proper API?
    pls->dev_dash    = 0;             // driver can not do dashed lines (yet)
    pls->plbuf_write = 1;             // driver uses the buffer for redraws

    if ( !pls->colorset )
        pls->color = 1;


#ifdef PL_HAVE_FREETYPE

//
//  Read registry to see if the user has set up default values
//  for text and smoothing. These will be overriden by anything that
//  might be given on the command line, so we will load the
//  values right into the same memory slots we pass to plParseDrvOpts
//

    GetRegIntValue( key_name, Keyword_text, &freetype );
    GetRegIntValue( key_name, Keyword_smooth, &smooth_text );

#endif

// Check for and set up driver options

    plParseDrvOpts( wingcc_options );

#ifdef PL_HAVE_FREETYPE

//
//  We will now save the settings to the registary if the user wants
//

    if ( save_reg == 1 )
    {
        SetRegIntValue( key_name, Keyword_text, &freetype );
        SetRegIntValue( key_name, Keyword_smooth, &smooth_text );
    }

#endif

// Set up device parameters

    if ( pls->xlength <= 0 || pls->ylength <= 0 )
    {
        // use default width, height of 800x600 if not specifed by -geometry option
        // or plspage
        plspage( 0., 0., 800, 600, 0, 0 );
    }

    dev->width  = pls->xlength - 1;     // should I use -1 or not???
    dev->height = pls->ylength - 1;

//
// Begin initialising the window
//

    // Initialize the entire structure to zero.
    memset( &dev->wndclass, 0, sizeof ( WNDCLASSEX ) );

    // This class is called WinTestWin
    dev->wndclass.lpszClassName = szWndClass;

    // cbSize gives the size of the structure for extensibility.
    dev->wndclass.cbSize = sizeof ( WNDCLASSEX );

    // All windows of this class redraw when resized.
    dev->wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC | CS_PARENTDC;

    // All windows of this class use the WndProc window function.
    dev->wndclass.lpfnWndProc = PlplotWndProc;

    // This class is used with the current program instance.

    dev->wndclass.hInstance = GetModuleHandle( NULL );

    // Use standard application icon and arrow cursor provided by the OS
    dev->wndclass.hIcon   = LoadIcon( NULL, IDI_APPLICATION );
    dev->wndclass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );
    dev->wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
    // Color the background white
    dev->wndclass.hbrBackground = NULL;

    dev->wndclass.cbWndExtra = sizeof ( pls );


    //
    // Now register the window class for use.
    //

    RegisterClassEx( &dev->wndclass );

    //
    //convert the program name to wide char if needed
    //

#ifdef UNICODE
    printf( pls->program );
    programlength = strlen( pls->program ) + 1;
    program       = malloc( programlength * sizeof ( TCHAR ) );
    MultiByteToWideChar( CP_UTF8, 0, pls->program, programlength, program, programlength );
#else
    program = pls->program;
#endif
    //
    // Create our main window using that window class.
    //
    dev->hwnd = CreateWindowEx( WS_EX_WINDOWEDGE + WS_EX_LEFT,
        szWndClass,                                         // Class name
        program,                                            // Caption
        WS_OVERLAPPEDWINDOW,                                // Style
        pls->xoffset,                                       // Initial x (use default)
        pls->yoffset,                                       // Initial y (use default)
        pls->xlength,                                       // Initial x size (use default)
        pls->ylength,                                       // Initial y size (use default)
        NULL,                                               // No parent window
        NULL,                                               // No menu
        dev->wndclass.hInstance,                            // This program instance
        NULL                                                // Creation parameters
        );

#ifdef UNICODE
    free( program );
#endif

//
// Attach a pointer to the stream to the window's user area
// this pointer will be used by the windows call back for
// process this window
//

#ifdef _WIN64
    SetWindowLongPtr( dev->hwnd, GWL_USERDATA, (LONG_PTR) pls );
#else
    SetWindowLong( dev->hwnd, GWL_USERDATA, (LONG) pls );
#endif

    dev->SCRN_hdc = dev->hdc = GetDC( dev->hwnd );

//
//  Setup the popup menu
//

    dev->PopupMenu = CreatePopupMenu();
    AppendMenu( dev->PopupMenu, MF_STRING, PopupPrint, _T( "Print" ) );
    AppendMenu( dev->PopupMenu, MF_STRING, PopupNextPage, _T( "Next Page" ) );
    AppendMenu( dev->PopupMenu, MF_STRING, PopupQuit, _T( "Quit" ) );

#ifdef PL_HAVE_FREETYPE

    if ( freetype )
    {
        pls->dev_text    = 1; // want to draw text
        pls->dev_unicode = 1; // want unicode
        init_freetype_lv1( pls );
        FT = (FT_Data *) pls->FT;
        FT->want_smooth_text = smooth_text;
    }

#endif



    plD_state_wingcc( pls, PLSTATE_COLOR0 );
    //
    // Display the window which we just created (using the nShow
    // passed by the OS, which allows for start minimized and that
    // sort of thing).
    //
    ShowWindow( dev->hwnd, SW_SHOWDEFAULT );
    SetForegroundWindow( dev->hwnd );

    //
    // Set up the DPI etc...
    //


    if ( pls->xdpi <= 0 ) // Get DPI from windows
    {
        plspage( GetDeviceCaps( dev->hdc, HORZRES ) / GetDeviceCaps( dev->hdc, HORZSIZE ) * 25.4,
            GetDeviceCaps( dev->hdc, VERTRES ) / GetDeviceCaps( dev->hdc, VERTSIZE ) * 25.4, 0, 0, 0, 0 );
    }
    else
    {
        pls->ydpi = pls->xdpi;        // Set X and Y dpi's to the same value
    }


//
//  Now we have to find out, from windows, just how big our drawing area is
//  when we specified the page size earlier on, that includes the borders,
//  title bar etc... so now that windows has done all its initialisations,
//  we will ask how big the drawing area is, and tell plplot
//

    GetClientRect( dev->hwnd, &dev->rect );
    dev->width  = dev->rect.right;
    dev->height = dev->rect.bottom;

    if ( dev->width > dev->height )           // Work out the scaling factor for the
    {                                         // "virtual" (oversized) page
        dev->scale = (PLFLT) ( PIXELS_X - 1 ) / dev->width;
    }
    else
    {
        dev->scale = (PLFLT) PIXELS_Y / dev->height;
    }

    Debug2( "Scale = %f (FLT)\n", dev->scale );

    plP_setpxl( dev->scale * pls->xdpi / 25.4, dev->scale * pls->ydpi / 25.4 );
    plP_setphy( 0, (PLINT) ( dev->scale * dev->width ), 0, (PLINT) ( dev->scale * dev->height ) );

    // Set fill rule.
    if ( pls->dev_eofill )
        SetPolyFillMode( dev->hdc, ALTERNATE );
    else
        SetPolyFillMode( dev->hdc, WINDING );

#ifdef PL_HAVE_FREETYPE
    if ( pls->dev_text )
    {
        init_freetype_lv2( pls );
    }
#endif
}

//--------------------------------------------------------------------------
// plD_line_wingcc()
//
// Draw a line in the current color from (x1,y1) to (x2,y2).
//--------------------------------------------------------------------------

void
plD_line_wingcc( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
    POINT      points[2];


    points[0].x = (LONG) ( x1a / dev->scale );
    points[1].x = (LONG) ( x2a / dev->scale );
    points[0].y = (LONG) ( dev->height - ( y1a / dev->scale ) );
    points[1].y = (LONG) ( dev->height - ( y2a / dev->scale ) );

    dev->oldobject = SelectObject( dev->hdc, dev->pen );

    if ( points[0].x != points[1].x || points[0].y != points[1].y )
    {
        Polyline( dev->hdc, points, 2 );
    }
    else
    {
        SetPixel( dev->hdc, points[0].x, points[0].y, dev->colour );
    }
    SelectObject( dev->hdc, dev->oldobject );
}


//--------------------------------------------------------------------------
// plD_polyline_wingcc()
//
// Draw a polyline in the current color.
//--------------------------------------------------------------------------

void
plD_polyline_wingcc( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
    int        i;
    POINT      *points = NULL;

    if ( npts > 0 )
    {
        points = GlobalAlloc( GMEM_ZEROINIT | GMEM_FIXED, (size_t) npts * sizeof ( POINT ) );
        if ( points != NULL )
        {
            for ( i = 0; i < npts; i++ )
            {
                points[i].x = (LONG) ( xa[i] / dev->scale );
                points[i].y = (LONG) ( dev->height - ( ya[i] / dev->scale ) );
            }
            dev->oldobject = SelectObject( dev->hdc, dev->pen );
            Polyline( dev->hdc, points, npts );
            SelectObject( dev->hdc, dev->oldobject );
            GlobalFree( points );
        }
        else
        {
            plexit( "Could not allocate memory to \"plD_polyline_wingcc\"\n" );
        }
    }
}

//--------------------------------------------------------------------------
// plD_fill_polygon_wingcc()
//
// Fill polygon described in points pls->dev_x[] and pls->dev_y[].
//--------------------------------------------------------------------------

static void
plD_fill_polygon_wingcc( PLStream *pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
    int        i;
    POINT      *points = NULL;
    HPEN       hpen, hpenOld;

    if ( pls->dev_npts > 0 )
    {
        points = GlobalAlloc( GMEM_ZEROINIT, (size_t) pls->dev_npts * sizeof ( POINT ) );

        if ( points == NULL )
            plexit( "Could not allocate memory to \"plD_fill_polygon_wingcc\"\n" );

        for ( i = 0; i < pls->dev_npts; i++ )
        {
            points[i].x = (PLINT) ( pls->dev_x[i] / dev->scale );
            points[i].y = (PLINT) ( dev->height - ( pls->dev_y[i] / dev->scale ) );
        }

        dev->fillbrush = CreateSolidBrush( dev->colour );
        hpen           = CreatePen( PS_SOLID, 1, dev->colour );
        dev->oldobject = SelectObject( dev->hdc, dev->fillbrush );
        hpenOld        = SelectObject( dev->hdc, hpen );
        Polygon( dev->hdc, points, pls->dev_npts );
        SelectObject( dev->hdc, dev->oldobject );
        DeleteObject( dev->fillbrush );
        SelectObject( dev->hdc, hpenOld );
        DeleteObject( hpen );
        GlobalFree( points );
    }
}

//--------------------------------------------------------------------------
//    static void CopySCRtoBMP(PLStream *pls)
//       Function copies the screen contents into a bitmap which is
//       later used for fast redraws of the screen (when it gets corrupted)
//       rather than remaking the plot from the plot buffer.
//--------------------------------------------------------------------------

static void CopySCRtoBMP( PLStream *pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    //
    //   Clean up the old bitmap and DC
    //

    if ( dev->hdc2 != NULL )
        DeleteDC( dev->hdc2 );
    if ( dev->bitmap != NULL )
        DeleteObject( dev->bitmap );

    dev->hdc2 = CreateCompatibleDC( dev->hdc );
    GetClientRect( dev->hwnd, &dev->rect );
    dev->bitmap    = CreateCompatibleBitmap( dev->hdc, dev->rect.right, dev->rect.bottom );
    dev->oldobject = SelectObject( dev->hdc2, dev->bitmap );
    BitBlt( dev->hdc2, 0, 0, dev->rect.right, dev->rect.bottom, dev->hdc, 0, 0, SRCCOPY );
    SelectObject( dev->hdc2, dev->oldobject );
}



void
plD_eop_wingcc( PLStream *pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    Debug( "End of the page\n" );
    CopySCRtoBMP( pls );
    dev->already_erased = 2;

    NormalCursor();
}

//--------------------------------------------------------------------------
//  Beginning of the new page
//--------------------------------------------------------------------------
void
plD_bop_wingcc( PLStream *pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
#ifdef PL_HAVE_FREETYPE
    FT_Data    *FT = (FT_Data *) pls->FT;
#endif
    Debug( "Start of Page\t" );

//
//  Turn the cursor to a busy sign, clear the page by "invalidating" it
//  then reset the colours and pen width
//

    BusyCursor();
    dev->already_erased = 0;
    RedrawWindow( dev->hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW );

    plD_state_wingcc( pls, PLSTATE_COLOR0 );
}

void
plD_tidy_wingcc( PLStream *pls )
{
    wingcc_Dev *dev = NULL;

#ifdef PL_HAVE_FREETYPE
    if ( pls->dev_text )
    {
        FT_Data *FT = (FT_Data *) pls->FT;
        plscmap0n( FT->ncol0_org );
        plD_FreeType_Destroy( pls );
    }
#endif
    Debug( "plD_tidy_wingcc" );

    if ( pls->dev != NULL )
    {
        dev = (wingcc_Dev *) pls->dev;

        DeleteMenu( dev->PopupMenu, PopupPrint, 0 );
        DeleteMenu( dev->PopupMenu, PopupNextPage, 0 );
        DeleteMenu( dev->PopupMenu, PopupQuit, 0 );
        DestroyMenu( dev->PopupMenu );

        if ( dev->hdc2 != NULL )
            DeleteDC( dev->hdc2 );
        if ( dev->hdc != NULL )
            ReleaseDC( dev->hwnd, dev->hdc );
        if ( dev->bitmap != NULL )
            DeleteObject( dev->bitmap );

        free_mem( pls->dev );
    }
}

void
plD_wait_wingcc( PLStream * pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    Debug( "Wait for user input\n" );

    dev->waiting = 1;
    while ( dev->waiting == 1 && GetMessage( &dev->msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &dev->msg );
        switch ( (int) dev->msg.message )
        {
        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
            TrackPopupMenu( dev->PopupMenu, TPM_CENTERALIGN | TPM_RIGHTBUTTON, LOWORD( dev->msg.lParam ),
                HIWORD( dev->msg.lParam ), 0, dev->hwnd, NULL );
            break;

        case WM_CHAR:
            if ( ( (TCHAR) ( dev->msg.wParam ) == 32 ) ||
                 ( (TCHAR) ( dev->msg.wParam ) == 13 ) )
            {
                dev->waiting = 0;
            }
            else if ( ( (TCHAR) ( dev->msg.wParam ) == 27 ) ||
                      ( (TCHAR) ( dev->msg.wParam ) == 'q' ) ||
                      ( (TCHAR) ( dev->msg.wParam ) == 'Q' ) )
            {
                dev->waiting = 0;
                PostQuitMessage( 0 );
            }
            break;

        case WM_LBUTTONDBLCLK:
            Debug( "WM_LBUTTONDBLCLK\t" );
            dev->waiting = 0;
            break;

        case WM_COMMAND:
            switch ( LOWORD( dev->msg.wParam ) )
            {
            case PopupPrint:
                Debug( "PopupPrint" );
                PrintPage( pls );
                break;
            case PopupNextPage:
                Debug( "PopupNextPage" );
                dev->waiting = 0;
                break;
            case PopupQuit:
                Debug( "PopupQuit" );
                dev->waiting = 0;
                PostQuitMessage( 0 );
                break;
            }
            break;

        default:
            DispatchMessage( &dev->msg );
            break;
        }
    }
}


//--------------------------------------------------------------------------
// plD_state_wingcc()
//
// Handle change in PLStream state (color, pen width, fill attribute, etc).
//--------------------------------------------------------------------------

void
plD_state_wingcc( PLStream *pls, PLINT op )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    switch ( op )
    {
    case PLSTATE_COLOR0:
    case PLSTATE_COLOR1:
        dev->colour = RGB( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b );
        break;

    case PLSTATE_CMAP0:
    case PLSTATE_CMAP1:
        dev->colour = RGB( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b );
        break;
    }

    if ( dev->pen != NULL )
        DeleteObject( dev->pen );
    dev->pen = CreatePen( PS_SOLID, pls->width, dev->colour );
}

//--------------------------------------------------------------------------
// GetCursorCmd()
//
// Handle events connected to selecting points (modelled after xwin)
//--------------------------------------------------------------------------

static void
GetCursorCmd( PLStream *pls, PLGraphicsIn *gin )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    HCURSOR    crosshair;
    HCURSOR    previous;

    plGinInit( gin );

    crosshair = LoadCursor( GetModuleHandle( NULL ), IDC_CROSS );
    previous  = SetCursor( crosshair );

    while ( gin->pX < 0 )
    {
        GetMessage( &dev->msg, NULL, 0, 0 );
        TranslateMessage( &dev->msg );
        switch ( (int) dev->msg.message )
        {
        case WM_LBUTTONDOWN:
            if ( dev->msg.wParam & MK_LBUTTON )
            {
                gin->pX = dev->msg.pt.x;
                gin->pY = dev->msg.pt.y;
                gin->dX = (PLFLT) gin->pX / ( dev->width - 1 );
                gin->dY = 1.0 - (PLFLT) gin->pY / ( dev->height - 1 );

                gin->button = 1; // AM: there is no macro to indicate the pressed button!
                gin->state  = 0; // AM: is there an equivalent under Windows?
                gin->keysym = 0x20;
            }
            break;
        case WM_CHAR:
            gin->pX = dev->msg.pt.x;
            gin->pY = dev->msg.pt.y;
            gin->dX = (PLFLT) gin->pX / ( dev->width - 1 );
            gin->dY = 1.0 - (PLFLT) gin->pY / ( dev->height - 1 );

            gin->button = 0;
            gin->state  = 0;
            gin->keysym = dev->msg.wParam;

            break;
        }
    }

    // Restore the previous cursor
    SetCursor( previous );

//    if ( GetCursorPos(&p) )
//    {
//        if ( ScreenToClient( dev->hwnd, &p ) )
//        {
//            // Fill the fields, but actually we need to run the event loop
//            // We need to call GetMessage() in a loop. Unclear as yet to the
//            // actual interface: key/button presses?
//        }
//    }
}

//--------------------------------------------------------------------------
// plD_esc_wingcc()
//
// Handle PLplot escapes
//--------------------------------------------------------------------------

void
plD_esc_wingcc( PLStream *pls, PLINT op, void *ptr )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    switch ( op )
    {
    case PLESC_GETC:
        GetCursorCmd( pls, (PLGraphicsIn *) ptr );
        break;

    case PLESC_FILL:
        plD_fill_polygon_wingcc( pls );
        break;

    case PLESC_DOUBLEBUFFERING:
        break;

    case PLESC_XORMOD:
        if ( *(PLINT *) ( ptr ) == 0 )
            SetROP2( dev->hdc, R2_COPYPEN );
        else
            SetROP2( dev->hdc, R2_XORPEN );
        break;

#ifdef PL_HAVE_FREETYPE
    case PLESC_HAS_TEXT:
        plD_render_freetype_text( pls, (EscText *) ptr );
        break;

//     case PLESC_LIKES_UNICODE:
//      plD_render_freetype_sym(pls, (EscText *)ptr);
//      break;

#endif
    }
}

//--------------------------------------------------------------------------
// static void Resize( PLStream *pls )
//
// This function calculates how to resize a window after a message has been
// received from windows telling us the window has been changed.
// It tries to recalculate the scale of the window so everything works out
// just right.
// The window is only resized if plplot has finished all of its plotting.
// That means that if you resize while a picture is being plotted,
// unpredictable results may result. The reason I do this is because the
// resize function calls redraw window, which replays the whole plot.
//--------------------------------------------------------------------------

static void Resize( PLStream *pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
#ifdef PL_HAVE_FREETYPE
    FT_Data    *FT = (FT_Data *) pls->FT;
#endif
    Debug( "Resizing" );

    if ( dev->waiting == 1 )     // Only resize the window IF plplot has finished with it
    {
        memcpy( &dev->oldrect, &dev->rect, sizeof ( RECT ) );
        GetClientRect( dev->hwnd, &dev->rect );
        Debug3( "[%d %d]", dev->rect.right, dev->rect.bottom );

        if ( ( dev->rect.right > 0 ) && ( dev->rect.bottom > 0 ) )            // Check to make sure it isn't just minimised (i.e. zero size)
        {
            if ( memcmp( &dev->rect, &dev->oldrect, sizeof ( RECT ) ) != 0 )  // See if the window's changed size or not
            {
                dev->already_erased = 0;
                dev->width          = dev->rect.right;
                dev->height         = dev->rect.bottom;
                if ( dev->width > dev->height )     // Work out the scaling factor for the
                {                                   // "virtual" (oversized) page
                    dev->scale = (PLFLT) ( PIXELS_X - 1 ) / dev->width;
                }
                else
                {
                    dev->scale = (PLFLT) PIXELS_Y / dev->height;
                }

#ifdef PL_HAVE_FREETYPE
                if ( FT )
                {
                    FT->scale = dev->scale;
                    FT->ymax  = dev->height;
                }
#endif
            }
            RedrawWindow( dev->hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW );
        }
        else
        {
            memcpy( &dev->rect, &dev->oldrect, sizeof ( RECT ) ); // restore the old size to current size since the window is minimised
        }
    }
}


//--------------------------------------------------------------------------
// int SetRegValue(char *key_name, char *key_word, char *buffer,int dwType, int size)
//
//  Function set the registry; if registary entry does not exist, it is
//  created. Actually, the key is created before it is set just to make sure
//  that is is there !
//--------------------------------------------------------------------------

static int SetRegValue( TCHAR *key_name, TCHAR *key_word, char *buffer, int dwType, int size )
{
    int   j = 0;

    DWORD lpdwDisposition;
    HKEY  hKey;

    j = RegCreateKeyEx(
        HKEY_CURRENT_USER,
        key_name,
        0,                                         // reserved
        NULL,                                      // address of class string
        REG_OPTION_NON_VOLATILE,                   // special options flag
        KEY_WRITE,                                 // desired security access
        NULL,                                      // address of key security structure
        &hKey,                                     // address of buffer for opened handle
        &lpdwDisposition                           // address of disposition value buffer
        );

    if ( j == ERROR_SUCCESS )
    {
        RegSetValueEx( hKey, key_word, 0, dwType, buffer, size );
        RegCloseKey( hKey );
    }
    return ( j );
}

//--------------------------------------------------------------------------
// int GetRegValue(char *key_name, char *key_word, char *buffer, int size)
//
//  Function reads the registry and gets a string value from it
//  buffer must be allocated by the caller, and the size is given in the size
//  paramater.
//  Return code is 1 for success, and 0 for failure.
//--------------------------------------------------------------------------

static int GetRegValue( TCHAR *key_name, TCHAR *key_word, char *buffer, int size )
{
    int  ret = 0;
    HKEY hKey;
    int  dwType;
    int  dwSize = size;

    if ( RegOpenKeyEx( HKEY_CURRENT_USER, key_name, 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
    {
        if ( RegQueryValueEx( hKey, key_word, 0, (LPDWORD) &dwType, buffer, (LPDWORD) &dwSize ) == ERROR_SUCCESS )
        {
            ret = 1;
        }
        RegCloseKey( hKey );
    }
    return ( ret );
}

#ifdef PL_HAVE_FREETYPE

//--------------------------------------------------------------------------
//  void plD_pixel_wingcc (PLStream *pls, short x, short y)
//
//  callback function, of type "plD_pixel_fp", which specifies how a single
//  pixel is set in the current colour.
//--------------------------------------------------------------------------

static void plD_pixel_wingcc( PLStream *pls, short x, short y )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    SetPixel( dev->hdc, x, y, dev->colour );
}

static void plD_pixelV_wingcc( PLStream *pls, short x, short y )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    SetPixelV( dev->hdc, x, y, dev->colour );
}

//--------------------------------------------------------------------------
//  void plD_set_pixelV_wingcc (PLStream *pls, short x, short y,PLINT colour)
//
//  callback function, of type "plD_set_pixel_fp", which specifies how a
//  single pixel is set in the s[ecified colour. This colour
//  by-passes plplot's internal table, and directly 'hits the hardware'.
//--------------------------------------------------------------------------

static void plD_set_pixel_wingcc( PLStream *pls, short x, short y, PLINT colour )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    SetPixel( dev->hdc, x, y, colour );
}

static void plD_set_pixelV_wingcc( PLStream *pls, short x, short y, PLINT colour )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    SetPixelV( dev->hdc, x, y, colour );
}


//--------------------------------------------------------------------------
//  void plD_read_pixel_wingcc (PLStream *pls, short x, short y)
//
//  callback function, of type "plD_pixel_fp", which specifies how a single
//  pixel is read.
//--------------------------------------------------------------------------
static PLINT plD_read_pixel_wingcc( PLStream *pls, short x, short y )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    return ( GetPixel( dev->hdc, x, y ) );
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
    FT_Data    *FT;
    int        x;
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    plD_FreeType_init( pls );

    FT = (FT_Data *) pls->FT;



//
//  Work out if our device support "fast" pixel setting
//  and if so, use that instead of "slow" pixel setting
//

    x = GetDeviceCaps( dev->hdc, RASTERCAPS );

    if ( x & RC_BITBLT )
        FT->pixel = (plD_pixel_fp) plD_pixelV_wingcc;
    else
        FT->pixel = (plD_pixel_fp) plD_pixel_wingcc;

//
//  See if we have a 24 bit device (or better), in which case
//  we can use the better antialaaising.
//

    if ( GetDeviceCaps( dev->hdc, BITSPIXEL ) > 24 )
    {
        FT->BLENDED_ANTIALIASING = 1;
        FT->read_pixel           = (plD_read_pixel_fp) plD_read_pixel_wingcc;

        if ( x & RC_BITBLT )
            FT->set_pixel = (plD_set_pixel_fp) plD_set_pixelV_wingcc;
        else
            FT->set_pixel = (plD_set_pixel_fp) plD_set_pixel_wingcc;
    }
}

//--------------------------------------------------------------------------
//  void init_freetype_lv2 (PLStream *pls)
//
//  "Level 2" initialisation of the freetype library.
//  "Level 2" fills in a few setting that aren't public until after the
//  graphics sub-system has been initialised.
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
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
    FT_Data    *FT  = (FT_Data *) pls->FT;

    FT->scale    = dev->scale;
    FT->ymax     = dev->height;
    FT->invert_y = 1;

    if ( ( FT->want_smooth_text == 1 ) && ( FT->BLENDED_ANTIALIASING == 0 ) )           // do we want to at least *try* for smoothing ?
    {
        FT->ncol0_org   = pls->ncol0;                                                   // save a copy of the original size of ncol0
        FT->ncol0_xtra  = 16777216 - ( pls->ncol1 + pls->ncol0 );                       // work out how many free slots we have
        FT->ncol0_width = max_number_of_grey_levels_used_in_text_smoothing;             // find out how many different shades of anti-aliasing we can do
        FT->ncol0_width = max_number_of_grey_levels_used_in_text_smoothing;             // set a maximum number of shades
        plscmap0n( FT->ncol0_org + ( FT->ncol0_width * pls->ncol0 ) );                  // redefine the size of cmap0
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
        FT->smooth_text = 1;                                                       // Yippee ! We had success setting up the extended cmap0
    }
    else if ( ( FT->want_smooth_text == 1 ) && ( FT->BLENDED_ANTIALIASING == 1 ) ) // If we have a truecolour device, we wont even bother trying to change the palette
    {
        FT->smooth_text = 1;
    }
}
#endif

//--------------------------------------------------------------------------
//  static void UpdatePageMetrics ( PLStream *pls, char flag )
//
//      UpdatePageMetrics is a simple function which simply gets new vales for
//      a changed DC, be it swapping from printer to screen or vice-versa.
//      The flag variable is used to tell the function if it is updating
//      from the printer (1) or screen (0).
//--------------------------------------------------------------------------

static void UpdatePageMetrics( PLStream *pls, char flag )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  #ifdef PL_HAVE_FREETYPE
    FT_Data    *FT = (FT_Data *) pls->FT;
  #endif

    if ( flag == 1 )
    {
        dev->width  = GetDeviceCaps( dev->hdc, HORZRES ); // Get the page size from the printer
        dev->height = GetDeviceCaps( dev->hdc, VERTRES );
    }
    else
    {
        GetClientRect( dev->hwnd, &dev->rect );
        dev->width  = dev->rect.right;
        dev->height = dev->rect.bottom;
    }

    if ( dev->width > dev->height )     // Work out the scaling factor for the
    {                                   // "virtual" (oversized) page
        dev->scale = (PLFLT) ( PIXELS_X - 1 ) / dev->width;
    }
    else
    {
        dev->scale = (PLFLT) PIXELS_Y / dev->height;
    }

  #ifdef PL_HAVE_FREETYPE
    if ( FT )           // If we are using freetype, then set it up next
    {
        FT->scale = dev->scale;
        FT->ymax  = dev->height;
        if ( GetDeviceCaps( dev->hdc, RASTERCAPS ) & RC_BITBLT )
            FT->pixel = (plD_pixel_fp) plD_pixelV_wingcc;
        else
            FT->pixel = (plD_pixel_fp) plD_pixel_wingcc;
    }
  #endif

    pls->xdpi = GetDeviceCaps( dev->hdc, HORZRES ) / GetDeviceCaps( dev->hdc, HORZSIZE ) * 25.4;
    pls->ydpi = GetDeviceCaps( dev->hdc, VERTRES ) / GetDeviceCaps( dev->hdc, VERTSIZE ) * 25.4;
    plP_setpxl( dev->scale * pls->xdpi / 25.4, dev->scale * pls->ydpi / 25.4 );
    plP_setphy( 0, (PLINT) ( dev->scale * dev->width ), 0, (PLINT) ( dev->scale * dev->height ) );
}

//--------------------------------------------------------------------------
//  static void PrintPage ( PLStream *pls )
//
//     Function brings up a standard printer dialog and, after the user
//     has selected a printer, replots the current page to the windows
//     printer.
//--------------------------------------------------------------------------

static void PrintPage( PLStream *pls )
{
    wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  #ifdef PL_HAVE_FREETYPE
    FT_Data    *FT = (FT_Data *) pls->FT;
  #endif
    PRINTDLG   Printer;
    DOCINFO    docinfo;

    //
    //    Reset the docinfo structure to 0 and set it's fields up
    //    This structure is used to supply a name to the print queue
    //

    ZeroMemory( &docinfo, sizeof ( docinfo ) );
    docinfo.cbSize      = sizeof ( docinfo );
    docinfo.lpszDocName = _T( "Plplot Page" );

    //
    //   Reset out printer structure to zero and initialise it
    //

    ZeroMemory( &Printer, sizeof ( PRINTDLG ) );
    Printer.lStructSize = sizeof ( PRINTDLG );
    Printer.hwndOwner   = dev->hwnd;
    Printer.Flags       = PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    Printer.nCopies     = 1;

    //
    //   Call the printer dialog function.
    //   If the user has clicked on "Print", then we will continue
    //   processing and print out the page.
    //

    if ( PrintDlg( &Printer ) != 0 )
    {
        //
        //  Before doing anything, we will take some backup copies
        //  of the existing values for page size and the like, because
        //  all we are going to do is a quick and dirty modification
        //  of plplot's internals to match the new page size and hope
        //  it all works out ok. After that, we will manip the values,
        //  and when all is done, restore them.
        //

        if ( ( dev->push = GlobalAlloc( GMEM_ZEROINIT, sizeof ( wingcc_Dev ) ) ) != NULL )
        {
            BusyCursor();
            memcpy( dev->push, dev, sizeof ( wingcc_Dev ) );

            dev->hdc = dev->PRNT_hdc = Printer.hDC; // Copy the printer HDC

            UpdatePageMetrics( pls, 1 );

          #ifdef PL_HAVE_FREETYPE
            if ( FT )                                  // If we are using freetype, then set it up next
            {
                dev->FT_smooth_text = FT->smooth_text; // When printing, we don't want smoothing
                FT->smooth_text     = 0;
            }
          #endif

            //
            //   Now the stuff that actually does the printing !!
            //

            StartDoc( dev->hdc, &docinfo );
            plRemakePlot( pls );
            EndDoc( dev->hdc );

            //
            //  Now to undo everything back to what it was for the screen
            //

            dev->hdc = dev->SCRN_hdc;  // Reset the screen HDC to the default
            UpdatePageMetrics( pls, 0 );

          #ifdef PL_HAVE_FREETYPE
            if ( FT )           // If we are using freetype, then set it up next
            {
                FT->smooth_text = dev->FT_smooth_text;
            }
          #endif
            memcpy( dev, dev->push, sizeof ( wingcc_Dev ) ); // POP our "stack" now to restore the values

            GlobalFree( dev->push );
            NormalCursor();
            RedrawWindow( dev->hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW );
        }
    }
}



#else
int
pldummy_wingcc()
{
    return ( 0 );
}

#endif                          // PLD_wingccdev
