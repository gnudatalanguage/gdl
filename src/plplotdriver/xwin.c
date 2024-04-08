//      PLplot X-windows device driver.
//
// Copyright (C) 2004  Maurice LeBrun
// Copyright (C) 2004  Joao Cardoso
// Copyright (C) 2004  Rafael Laboissiere
// Copyright (C) 2004  Andrew Ross
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

#define DEBUG

#ifdef PLD_xwin
#define NEED_PLDEBUG
#include "plplotP.h"
#include "plxwd.h"
#include "drivers.h"
#include "plevent.h"

#ifdef PL_USE_PTHREADS_XWIN
#include <pthread.h>
#include <signal.h>
int    pthread_mutexattr_settype( pthread_mutexattr_t *attr, int kind );
static void events_thread( void *pls );
static pthread_mutex_t events_mutex;
static int             already = 0;
#endif

// Device info
PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_xwin = "xwin:X-Window (Xlib):1:xwin:5:xw\n";

static int synchronize = 0;                   // change to 1 for X synchronized operation
                                              // Use "-drvopt sync" cmd line option to set.

static int nobuffered = 0;                    // make it a buffered device by default
                                              // use "-drvopt unbuffered" to make it unbuffered

static int noinitcolors = 0;                  // Call InitColors by default
                                              // use "-drvopt noinitcolors" to leave colors uninitialized

static int defaultvisual = 1;                 // use "-drvopt defvis=0" to not use the default visual

static int usepthreads = 1;                   // use "-drvopt usepth=0" to not use pthreads to redisplay

//
// When -drvopt defvis is defined, DefaultVisual() is used to get the visual.
// Otherwise, the visual is obtained using XGetVisualInfo() to make a match.
//

//#define HACK_STATICCOLOR

// Number of instructions to skip between querying the X server for events

#define MAX_INSTR    20

// The xwin driver uses the xscale and yscale values to convert from virtual
// to real pixels using the current size in pixels of the display window.
// We define PHYSICAL here so that PLplot core knows about this rescaling
// and mm values are converted to virtual pixels at a ratio consistent with
// a constant ratio of DPMM real pixels per mm.
#define PHYSICAL    1

// These need to be distinguished since the handling is slightly different.

#define LOCATE_INVOKED_VIA_API       1
#define LOCATE_INVOKED_VIA_DRIVER    2

// Set constants for dealing with colormap.  In brief:
//
//      --- custom colormaps ---
// ccmap		When set, turns on custom color map
// CCMAP_XWM_COLORS	Number of low "pixel" values to copy.
//
//      --- r/w colormaps --
// RWMAP_CMAP1_COLORS	Color map 1 entries.
// RWMAP_MAX_COLORS	Maximum colors in RW colormaps
//
//      --- r/o colormaps --
// ROMAP_CMAP1_COLORS	Color map 1 entries.
// TC_CMAP1_COLORS	TrueColor visual Color map 1 entries.
//
// See Init_CustomCmap() and  Init_DefaultCmap() for more info.
// Set ccmap at your own risk -- still under development.
//

// plplot_ccmap is statically defined in plxwd.h.  Note that
// plframe.c also includes that header and uses the variable.

#define CCMAP_XWM_COLORS      70

#define RWMAP_CMAP1_COLORS    50
#define RWMAP_MAX_COLORS      256

#define ROMAP_CMAP1_COLORS    50
#define TC_CMAP1_COLORS       200

// Variables to hold RGB components of given colormap.
// Used in an ugly hack to get past some X11R5 and TK limitations.

static int    sxwm_colors_set;
static XColor sxwm_colors[RWMAP_MAX_COLORS];

// Keep pointers to all the displays in use

static XwDisplay *xwDisplay[PLXDISPLAYS];

// Function prototypes

// Driver entry and dispatch setup

void plD_dispatch_init_xw( PLDispatchTable *pdt );

void plD_init_xw( PLStream * );
void plD_line_xw( PLStream *, short, short, short, short );
void plD_polyline_xw( PLStream *, short *, short *, PLINT );
void plD_eop_xw( PLStream * );
void plD_bop_xw( PLStream * );
void plD_tidy_xw( PLStream * );
void plD_wait_xw( PLStream * );
void plD_state_xw( PLStream *, PLINT );
void plD_esc_xw( PLStream *, PLINT, void * );

// Initialization

static void  OpenXwin( PLStream *pls );
static void  Init( PLStream *pls );
static void  InitMain( PLStream *pls );
static void  InitColors( PLStream *pls );
static void  AllocCustomMap( PLStream *pls );
static void  AllocCmap0( PLStream *pls );
static void  AllocCmap1( PLStream *pls );
static void  MapMain( PLStream *pls );
static void  CreatePixmap( PLStream *pls );
static void  GetVisual( PLStream *pls );
static void  AllocBGFG( PLStream *pls );
static int   AreWeGrayscale( Display *display );

// Routines to poll the X-server

static void  WaitForPage( PLStream *pls );
static void  CheckForEvents( PLStream *pls );
static void  HandleEvents( PLStream *pls );

// Event handlers

static void  MasterEH( PLStream *pls, XEvent *event );
static void  ClientEH( PLStream *pls, XEvent *event );
static void  ExposeEH( PLStream *pls, XEvent *event );
static void  ResizeEH( PLStream *pls, XEvent *event );
static void  MotionEH( PLStream *pls, XEvent *event );
static void  EnterEH( PLStream *pls, XEvent *event );
static void  LeaveEH( PLStream *pls, XEvent *event );
static void  KeyEH( PLStream *pls, XEvent *event );
static void  ButtonEH( PLStream *pls, XEvent *event );
static void  LookupXKeyEvent( PLStream *pls, XEvent *event );
static void  LookupXButtonEvent( PLStream *pls, XEvent *event );

static void  ProcessKey( PLStream *pls );
static void  LocateKey( PLStream *pls );
static void  ProcessButton( PLStream *pls );
static void  LocateButton( PLStream *pls );
static void  Locate( PLStream *pls );

// Routines for manipulating graphic crosshairs

static void  CreateXhairs( PLStream *pls );
static void  DestroyXhairs( PLStream *pls );
static void  DrawXhairs( PLStream *pls, int x0, int y0 );
static void  UpdateXhairs( PLStream *pls );

// Escape function commands

static void  ExposeCmd( PLStream *pls, PLDisplay *ptr );
static void  RedrawCmd( PLStream *pls );
static void  ResizeCmd( PLStream *pls, PLDisplay *ptr );
static void  ConfigBufferingCmd( PLStream *pls, PLBufferingCB *ptr );
static void  GetCursorCmd( PLStream *pls, PLGraphicsIn *ptr );
static void  FillPolygonCmd( PLStream *pls );
static void  XorMod( PLStream *pls, PLINT *mod );
static void  DrawImage( PLStream *pls );

// Miscellaneous

static void  StoreCmap0( PLStream *pls );
static void  StoreCmap1( PLStream *pls );
static void  imageops( PLStream *pls, PLINT *ptr );
static void  SetBGFG( PLStream *pls );
#ifdef DUMMY
static void  SaveColormap( Display *display, Colormap colormap );
#endif
static void  PLColor_to_XColor( PLColor *plcolor, XColor *xcolor );
static void  PLColor_from_XColor( PLColor *plcolor, XColor *xcolor );

static DrvOpt xwin_options[] = { { "sync",         DRV_INT, &synchronize,   "Synchronized X server operation (0|1)" },
                                 { "nobuffered",   DRV_INT, &nobuffered,    "Sets unbuffered operation (0|1)"       },
                                 { "noinitcolors", DRV_INT, &noinitcolors,  "Sets cmap0 allocation (0|1)"           },
                                 { "defvis",       DRV_INT, &defaultvisual, "Use the Default Visual (0|1)"          },
                                 { "usepth",       DRV_INT, &usepthreads,   "Use pthreads (0|1)"                    },
                                 { NULL,           DRV_INT, NULL,           NULL                                    } };

//define LINE2D, POLYLINE2D
#define LINE2D plD_line_xw
#define POLYLINE2D plD_polyline_xw
#include "plplot3d.h"

void plD_dispatch_init_xw( PLDispatchTable *pdt )
{
  currDispatchTab=pdt;
  Status3D=0;
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "X-Window (Xlib)";
    pdt->pl_DevName = "xwin";
#endif
    pdt->pl_type     = plDevType_Interactive;
    pdt->pl_seq      = 5;
    pdt->pl_init     = (plD_init_fp) plD_init_xw;
    pdt->pl_line     = (plD_line_fp) plD_line_xw;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_xw;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_xw;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_xw;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_xw;
    pdt->pl_state    = (plD_state_fp) plD_state_xw;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_xw;
    pdt->pl_wait     = (plD_wait_fp) plD_wait_xw;
}

//--------------------------------------------------------------------------
// plD_init_xw()
//
// Initialize device.
// X-dependent stuff done in OpenXwin() and Init().
//--------------------------------------------------------------------------

void
plD_init_xw( PLStream *pls )
{
    XwDev *dev;
    PLFLT pxlx, pxly;
    int   xmin = 0;
    int   xmax = PIXELS_X - 1;
    int   ymin = 0;
    int   ymax = PIXELS_Y - 1;

    dbug_enter( "plD_init_xw" );

    pls->termin      = 1;       // Is an interactive terminal
    pls->dev_flush   = 1;       // Handle our own flushes
    pls->dev_fill0   = 1;       // Handle solid fills
    pls->plbuf_write = 1;       // Activate plot buffer
    pls->dev_fastimg = 1;       // is a fast image device
    pls->dev_xor     = 1;       // device support xor mode

#ifndef PL_USE_PTHREADS_XWIN
    usepthreads = 0;
#endif

    plParseDrvOpts( xwin_options );

#ifndef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        plwarn( "You said you want pthreads, but they are not available." );
#endif

    if ( nobuffered )
        pls->plbuf_write = 0;   // deactivate plot buffer

// The real meat of the initialization done here

    if ( pls->dev == NULL )
        OpenXwin( pls );

    dev = (XwDev *) pls->dev;

    Init( pls );

// Get ready for plotting

    dev->xlen = (short) ( xmax - xmin );
    dev->ylen = (short) ( ymax - ymin );

    dev->xscale_init = (double) dev->init_width / (double) dev->xlen;
    dev->yscale_init = (double) dev->init_height / (double) dev->ylen;

    dev->xscale = dev->xscale_init;
    dev->yscale = dev->yscale_init;

#if PHYSICAL
    pxlx = DPMM / dev->xscale;
    pxly = DPMM / dev->yscale;
#else
    pxlx = (double) PIXELS_X / LPAGE_X;
    pxly = (double) PIXELS_Y / LPAGE_Y;
#endif

    plP_setpxl( pxlx, pxly );
    plP_setphy( xmin, xmax, ymin, ymax );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
    {
        pthread_mutexattr_t mutexatt;
        pthread_attr_t      pthattr;

        if ( !already )
        {
            pthread_mutexattr_init( &mutexatt );
            if ( pthread_mutexattr_settype( &mutexatt, PLPLOT_MUTEX_RECURSIVE ) )
                plexit( "xwin: pthread_mutexattr_settype() failed!\n" );

            pthread_mutex_init( &events_mutex, &mutexatt );
            already = 1;
        }
        else
        {
            pthread_mutex_lock( &events_mutex );
            already++;
            pthread_mutex_unlock( &events_mutex );
        }

        pthread_attr_init( &pthattr );
        pthread_attr_setdetachstate( &pthattr, PTHREAD_CREATE_JOINABLE );

        if ( pthread_create( &( dev->updater ), &pthattr, ( void *( * )( void * ) ) & events_thread, (void *) pls ) )
        {
            pthread_mutex_lock( &events_mutex );
            already--;
            pthread_mutex_unlock( &events_mutex );

            if ( already == 0 )
            {
                pthread_mutex_destroy( &events_mutex );
                plexit( "xwin: pthread_create() failed!\n" );
            }
            else
                plwarn( "xwin: couldn't create thread for this plot window!\n" );
        }
    }
#endif
}

//--------------------------------------------------------------------------
// plD_line_xw()
//
// Draw a line in the current color from (x1,y1) to (x2,y2).
//--------------------------------------------------------------------------

void
plD_line_xw( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    int       x1 = x1a, y1 = y1a, x2 = x2a, y2 = y2a;

    dbug_enter( "plD_line_xw" );
    
#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    CheckForEvents( pls );

    y1 = dev->ylen - y1;
    y2 = dev->ylen - y2;

    x1 = (int) ( x1 * dev->xscale );
    x2 = (int) ( x2 * dev->xscale );
    y1 = (int) ( y1 * dev->yscale );
    y2 = (int) ( y2 * dev->yscale );

    if ( dev->write_to_window )
        XDrawLine( xwd->display, dev->window, dev->gc, x1, y1, x2, y2 );

    if ( dev->write_to_pixmap )
        XDrawLine( xwd->display, dev->pixmap, dev->gc, x1, y1, x2, y2 );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif
}

//--------------------------------------------------------------------------
// XorMod()
//
// Enter xor mode ( mod != 0) or leave it ( mode = 0)
//--------------------------------------------------------------------------

static void
XorMod( PLStream *pls, PLINT *mod )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    if ( *mod == 0 )
        XSetFunction( xwd->display, dev->gc, GXcopy );
    else
        XSetFunction( xwd->display, dev->gc, GXxor );
}

//--------------------------------------------------------------------------
// plD_polyline_xw()
//
// Draw a polyline in the current color from (x1,y1) to (x2,y2).
//--------------------------------------------------------------------------

void
plD_polyline_xw( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    PLINT     i;
    XPoint    _pts[PL_MAXPOLY];
    XPoint    *pts;

    if ( npts > PL_MAXPOLY )
    {
        pts = (XPoint *) malloc( sizeof ( XPoint ) * (size_t) npts );
    }
    else
    {
        pts = _pts;
    }

    dbug_enter( "plD_polyline_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    CheckForEvents( pls );

    for ( i = 0; i < npts; i++ )
    {
        pts[i].x = (short) ( dev->xscale * xa[i] );
        pts[i].y = (short) ( dev->yscale * ( dev->ylen - ya[i] ) );
    }

    if ( dev->write_to_window )
        XDrawLines( xwd->display, dev->window, dev->gc, pts, npts,
            CoordModeOrigin );

    if ( dev->write_to_pixmap )
        XDrawLines( xwd->display, dev->pixmap, dev->gc, pts, npts,
            CoordModeOrigin );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif

    if ( npts > PL_MAXPOLY )
    {
        free( pts );
    }
}

//--------------------------------------------------------------------------
// plD_eop_xw()
//
// End of page.  User must hit return (or third mouse button) to continue.
//--------------------------------------------------------------------------

void
plD_eop_xw( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    dbug_enter( "plD_eop_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    XFlush( xwd->display );
    if ( pls->db )
        ExposeCmd( pls, NULL );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif
}

//--------------------------------------------------------------------------
// plD_bop_xw()
//
// Set up for the next page.
//--------------------------------------------------------------------------

void
plD_bop_xw( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    dbug_enter( "plD_bop_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    dev->bgcolor = xwd->cmap0[0];

    if ( dev->write_to_window )
    {
        XSetWindowBackground( xwd->display, dev->window, dev->bgcolor.pixel );
        XSetBackground( xwd->display, dev->gc, dev->bgcolor.pixel );
        XClearWindow( xwd->display, dev->window );
    }
    if ( dev->write_to_pixmap )
    {
        XSetForeground( xwd->display, dev->gc, dev->bgcolor.pixel );
        XFillRectangle( xwd->display, dev->pixmap, dev->gc, 0, 0,
            dev->width, dev->height );
        XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
    }
    XSync( xwd->display, 0 );
    pls->page++;

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif
}

//--------------------------------------------------------------------------
// plD_tidy_xw()
//
// Close graphics file
//--------------------------------------------------------------------------

void
plD_tidy_xw( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    dbug_enter( "plD_tidy_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
    {
        pthread_mutex_lock( &events_mutex );
        if ( pthread_cancel( dev->updater ) == 0 )
            pthread_join( dev->updater, NULL );

        pthread_mutex_unlock( &events_mutex );
        if ( --already == 0 )
            pthread_mutex_destroy( &events_mutex );
    }
#endif

    if ( dev->is_main )
    {
        XDestroyWindow( xwd->display, dev->window );
        if ( dev->write_to_pixmap )
            XFreePixmap( xwd->display, dev->pixmap );
        XFlush( xwd->display );
    }

    xwd->nstreams--;
    if ( xwd->nstreams == 0 )
    {
        int ixwd = xwd->ixwd;
        XFreeGC( xwd->display, dev->gc );
        XFreeGC( xwd->display, xwd->gcXor );
        XCloseDisplay( xwd->display );
        free_mem( xwd->cmap0 );
        free_mem( xwd->cmap1 );
        free_mem( xwDisplay[ixwd] );
    }
    // ANR: if we set this here the tmp file will not be closed
    // See also comment in tkwin.c
    //pls->plbuf_write = 0;
}

//--------------------------------------------------------------------------
// plD_wait_xw()
//
// Wait for user input
//--------------------------------------------------------------------------

void
plD_wait_xw( PLStream *pls )
{
    XwDev *dev = (XwDev *) pls->dev;
    dbug_enter( "plD_eop_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    if ( dev->is_main )
        WaitForPage( pls );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif
}

//--------------------------------------------------------------------------
// plD_state_xw()
//
// Handle change in PLStream state (color, pen width, fill attribute, etc).
//--------------------------------------------------------------------------

void
plD_state_xw( PLStream *pls, PLINT op )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    dbug_enter( "plD_state_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    CheckForEvents( pls );

    switch ( op )
    {
    case PLSTATE_WIDTH:
        XSetLineAttributes( xwd->display, dev->gc, (unsigned int) pls->width,
            LineSolid, CapRound, JoinMiter );
        break;

    case PLSTATE_COLOR0: {
        int icol0 = pls->icol0;
        if ( xwd->color )
        {
            if ( icol0 == PL_RGB_COLOR )
            {
                PLColor_to_XColor( &pls->curcolor, &dev->curcolor );
                if ( !XAllocColor( xwd->display, xwd->map, &dev->curcolor ) )
                {
                    fprintf( stderr, "Warning: could not allocate color\n" );
                    dev->curcolor.pixel = xwd->fgcolor.pixel;
                }
            }
            else
            {
                dev->curcolor = xwd->cmap0[icol0];
            }
            XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
        }
        else
        {
            dev->curcolor = xwd->fgcolor;
            XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
        }
        break;
    }

    case PLSTATE_COLOR1: {
        int icol1;

        if ( xwd->ncol1 == 0 )
            AllocCmap1( pls );

        if ( xwd->ncol1 < 2 )
            break;

        icol1 = ( pls->icol1 * ( xwd->ncol1 - 1 ) ) / ( pls->ncol1 - 1 );
        if ( xwd->color )
            dev->curcolor = xwd->cmap1[icol1];
        else
            dev->curcolor = xwd->fgcolor;

        XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
        break;
    }

    case PLSTATE_CMAP0:
        SetBGFG( pls );
        // If ncol0 has changed, need to reallocate
        if ( pls->ncol0 != xwd->ncol0 )
            AllocCmap0( pls );
        StoreCmap0( pls );
        break;

    case PLSTATE_CMAP1:
        StoreCmap1( pls );
        break;
    }

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif
}

//--------------------------------------------------------------------------
// plD_esc_xw()
//
// Escape function.
//
// Functions:
//
//    PLESC_EH	Handle pending events
//    PLESC_EXPOSE	Force an expose
//    PLESC_FILL	Fill polygon
//    PLESC_FLUSH	Flush X event buffer
//    PLESC_GETC	Get coordinates upon mouse click
//    PLESC_REDRAW	Force a redraw
//    PLESC_RESIZE	Force a resize
//    PLESC_XORMOD      set/reset xor mode
//    PLESC_IMAGE     draw the image in a fast way
//    PLESC_IMAGEOPS: image related operations:
//         ZEROW2D      disable writing to display
//         ZEROW2B      disable writing to buffer
//         ONEW2D       enable  writing to display
//         ONEW2B       enable  writing to buffer
//    PLESC_PL2DEVCOL	convert PLColor to device color (XColor)
//    PLESC_DEV2PLCOL	convert device color (XColor) to PLColor
//    PLESC_SETBGFG	set BG, FG colors
//    PLESC_DEVINIT	alternate device initialization
//
// Note the [GET|SET]DEVCOL functions go through the intermediary stream
// variable tmpcolor to keep the syntax simple.
//--------------------------------------------------------------------------

void
plD_esc_xw( PLStream *pls, PLINT op, void *ptr )
{
    dbug_enter( "plD_esc_xw" );

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_lock( &events_mutex );
#endif

    switch ( op )
    {
    case PLESC_EH:
        HandleEvents( pls );
        break;

    case PLESC_EXPOSE:
        ExposeCmd( pls, (PLDisplay *) ptr );
        break;

    case PLESC_FILL:
        if (Status3D == 1) { //enable use everywhere.
          //perform conversion on the fly
          for (PLINT i = 0; i < pls->dev_npts; ++i) {
            int x = pls->dev_x[i];
            int y = pls->dev_y[i];
            // 3D convert, must take into account that y is inverted.
            SelfTransform3D(&x, &y);

            pls->dev_x[i] = x;
            pls->dev_y[i] = y;
          }
        }
        FillPolygonCmd( pls );
        break;

    case PLESC_FLUSH: {
        XwDev     *dev = (XwDev *) pls->dev;
        XwDisplay *xwd = (XwDisplay *) dev->xwd;
        HandleEvents( pls );
        XFlush( xwd->display );
        break;
    }
    case PLESC_GETC:
        GetCursorCmd( pls, (PLGraphicsIn *) ptr );
        break;

    case PLESC_REDRAW:
        RedrawCmd( pls );
        break;

    case PLESC_RESIZE:
        ResizeCmd( pls, (PLDisplay *) ptr );
        break;

    case PLESC_XORMOD:
        XorMod( pls, (PLINT *) ptr );
        break;

    case PLESC_DOUBLEBUFFERING:
        ConfigBufferingCmd( pls, (PLBufferingCB *) ptr );
        break;

    case PLESC_IMAGE:
        DrawImage( pls );
        break;

    case PLESC_IMAGEOPS:
        imageops( pls, (PLINT *) ptr );
        break;

    case PLESC_PL2DEVCOL:
        PLColor_to_XColor( &pls->tmpcolor, (XColor *) ptr );
        break;

    case PLESC_DEV2PLCOL:
        PLColor_from_XColor( &pls->tmpcolor, (XColor *) ptr );
        break;

    case PLESC_SETBGFG:
        SetBGFG( pls );
        break;

    case PLESC_DEVINIT:
        OpenXwin( pls );
        break;
        
    case PLESC_3D:
        Set3D( ptr );
        break;

    case PLESC_2D:
        UnSet3D();
        break;
    }

#ifdef PL_USE_PTHREADS_XWIN
    if ( usepthreads )
        pthread_mutex_unlock( &events_mutex );
#endif
}

//--------------------------------------------------------------------------
// GetCursorCmd()
//
// Waits for a graphics input event and returns coordinates.
//--------------------------------------------------------------------------

static void
GetCursorCmd( PLStream *pls, PLGraphicsIn *ptr )
{
    XwDev        *dev = (XwDev *) pls->dev;
    XwDisplay    *xwd = (XwDisplay *) dev->xwd;
    XEvent       event;
    PLGraphicsIn *gin = &( dev->gin );

// Initialize

    plGinInit( gin );
    dev->locate_mode = LOCATE_INVOKED_VIA_API;
    CreateXhairs( pls );

// Run event loop until a point is selected

    while ( gin->pX < 0 && dev->locate_mode )
    {
        // XWindowEvent( xwd->display, dev->window, dev->event_mask, &event );
        XNextEvent( xwd->display, &event );
        MasterEH( pls, &event );
    }
    *ptr = *gin;
    if ( dev->locate_mode )
    {
        dev->locate_mode = 0;
        DestroyXhairs( pls );
    }
}

//--------------------------------------------------------------------------
// FillPolygonCmd()
//
// Fill polygon described in points pls->dev_x[] and pls->dev_y[].
// Only solid color fill supported.
//--------------------------------------------------------------------------

static void
FillPolygonCmd( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XPoint    _pts[PL_MAXPOLY];
    XPoint    *pts;
    int       i;

    if ( pls->dev_npts > PL_MAXPOLY )
    {
        pts = (XPoint *) malloc( sizeof ( XPoint ) * (size_t) ( pls->dev_npts ) );
    }
    else
    {
        pts = _pts;
    }

    CheckForEvents( pls );

    for ( i = 0; i < pls->dev_npts; i++ )
    {
        pts[i].x = (short) ( dev->xscale * pls->dev_x[i] );
        pts[i].y = (short) ( dev->yscale * ( dev->ylen - pls->dev_y[i] ) );
    }

// Fill polygons

    if ( dev->write_to_window )
        XFillPolygon( xwd->display, dev->window, dev->gc,
            pts, pls->dev_npts, Complex, CoordModeOrigin );

    if ( dev->write_to_pixmap )
        XFillPolygon( xwd->display, dev->pixmap, dev->gc,
            pts, pls->dev_npts, Complex, CoordModeOrigin );

// If in debug mode, draw outline of boxes being filled

#ifdef DEBUG
    if ( pls->debug )
    {
        XSetForeground( xwd->display, dev->gc, xwd->fgcolor.pixel );
        if ( dev->write_to_window )
            XDrawLines( xwd->display, dev->window, dev->gc, pts, pls->dev_npts,
                CoordModeOrigin );

        if ( dev->write_to_pixmap )
            XDrawLines( xwd->display, dev->pixmap, dev->gc, pts, pls->dev_npts,
                CoordModeOrigin );

        XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
    }
#endif

    if ( pls->dev_npts > PL_MAXPOLY )
    {
        free( pts );
    }
}

//--------------------------------------------------------------------------
// OpenXwin()
//
// Performs basic driver initialization, without actually opening or
// modifying a window.  May be called by the outside world before plinit
// in case the caller needs early access to the driver internals (not
// very common -- currently only used by the plframe widget).
//--------------------------------------------------------------------------

static void
OpenXwin( PLStream *pls )
{
    XwDev     *dev;
    XwDisplay *xwd;
    int       i;

    dbug_enter( "OpenXwin" );

// Allocate and initialize device-specific data

    if ( pls->dev != NULL )
        plwarn( "OpenXwin: device pointer is already set" );

    pls->dev = calloc( 1, (size_t) sizeof ( XwDev ) );
    if ( pls->dev == NULL )
        plexit( "plD_init_xw: Out of memory." );

    dev = (XwDev *) pls->dev;

// Variables used in querying the X server for events

    dev->instr     = 0;
    dev->max_instr = MAX_INSTR;

// See if display matches any already in use, and if so use that

    dev->xwd = NULL;
    for ( i = 0; i < PLXDISPLAYS; i++ )
    {
        if ( xwDisplay[i] == NULL )
        {
            continue;
        }
        else if ( pls->FileName == NULL && xwDisplay[i]->displayName == NULL )
        {
            dev->xwd = xwDisplay[i];
            break;
        }
        else if ( pls->FileName == NULL || xwDisplay[i]->displayName == NULL )
        {
            continue;
        }
        else if ( strcmp( xwDisplay[i]->displayName, pls->FileName ) == 0 )
        {
            dev->xwd = xwDisplay[i];
            break;
        }
    }

// If no display matched, create a new one

    if ( dev->xwd == NULL )
    {
        dev->xwd = (XwDisplay *) calloc( 1, (size_t) sizeof ( XwDisplay ) );
        if ( dev->xwd == NULL )
            plexit( "Init: Out of memory." );

        for ( i = 0; i < PLXDISPLAYS; i++ )
        {
            if ( xwDisplay[i] == NULL )
                break;
        }
        if ( i == PLXDISPLAYS )
            plexit( "Init: Out of xwDisplay's." );

        xwDisplay[i]  = xwd = (XwDisplay *) dev->xwd;
        xwd->nstreams = 1;

// Open display
#ifdef PL_USE_PTHREADS_XWIN
        if ( usepthreads )
            if ( !XInitThreads() )
                plexit( "xwin: XInitThreads() not successful." );
#endif
        xwd->display = XOpenDisplay( pls->FileName );
        if ( xwd->display == NULL )
        {
            plexit( "Can't open display" );
        }
        xwd->displayName = pls->FileName;
        xwd->screen      = DefaultScreen( xwd->display );
        if ( synchronize )
            XSynchronize( xwd->display, 1 );

        // Get colormap and visual
        xwd->map = DefaultColormap( xwd->display, xwd->screen );
        GetVisual( pls );

        //
        // Figure out if we have a color display or not.
        // Default is color IF the user hasn't specified and IF the output device
        // is not grayscale.
        //
        if ( pls->colorset )
            xwd->color = pls->color;
        else
        {
            pls->color = 1;
            xwd->color = !AreWeGrayscale( xwd->display );
        }

        // Allocate space for colors
        // Note cmap1 allocation is deferred
        xwd->ncol0_alloc = pls->ncol0;
        xwd->cmap0       = (XColor *) calloc( (size_t) ( pls->ncol0 ), sizeof ( XColor ) );
        if ( xwd->cmap0 == 0 )
            plexit( "couldn't allocate space for cmap0 colors" );

        // Allocate & set background and foreground colors
        AllocBGFG( pls );
        SetBGFG( pls );
    }

// Display matched, so use existing display data

    else
    {
        xwd = (XwDisplay *) dev->xwd;
        xwd->nstreams++;
    }
    xwd->ixwd = i;
}

//--------------------------------------------------------------------------
// Init()
//
// Xlib initialization routine.
//
// Controlling routine for X window creation and/or initialization.
// The user may customize the window in the following ways:
//
// display:	pls->OutFile (use plsfnam() or -display option)
// size:	pls->xlength, pls->ylength (use plspage() or -geo option)
// bg color:	pls->cmap0[0] (use plscolbg() or -bg option)
//--------------------------------------------------------------------------

static void
Init( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    Window    root;
    int       x, y;

    dbug_enter( "Init" );

// If not plotting into a child window, need to create main window now

    if ( pls->window_id == 0 )
    {
        dev->is_main = TRUE;
        InitMain( pls );
    }
    else
    {
        dev->is_main = FALSE;
        dev->window  = (Window) pls->window_id;
    }

// Initialize colors

    if ( noinitcolors == 0 )
        InitColors( pls );
    XSetWindowColormap( xwd->display, dev->window, xwd->map );

// Set up GC for ordinary draws

    if ( !dev->gc )
        dev->gc = XCreateGC( xwd->display, dev->window, 0, 0 );

// Set up GC for rubber-band draws

    if ( !xwd->gcXor )
    {
        XGCValues     gcValues;
        unsigned long mask;

        gcValues.background = xwd->cmap0[0].pixel;
        gcValues.foreground = 0xFF;
        gcValues.function   = GXxor;
        mask = GCForeground | GCBackground | GCFunction;

        xwd->gcXor = XCreateGC( xwd->display, dev->window, mask, &gcValues );
    }

// Get initial drawing area dimensions

    (void) XGetGeometry( xwd->display, dev->window, &root, &x, &y,
        &dev->width, &dev->height, &dev->border, &xwd->depth );

    dev->init_width  = (long) dev->width;
    dev->init_height = (long) dev->height;

// Set up flags that determine what we are writing to
// If nopixmap is set, ignore db

    if ( pls->nopixmap )
    {
        dev->write_to_pixmap = 0;
        pls->db = 0;
    }
    else
    {
        dev->write_to_pixmap = 1;
    }
    dev->write_to_window = !pls->db;

// Create pixmap for holding plot image (for expose events).

    if ( dev->write_to_pixmap )
        CreatePixmap( pls );

// Set drawing color

    plD_state_xw( pls, PLSTATE_COLOR0 );

    XSetWindowBackground( xwd->display, dev->window, xwd->cmap0[0].pixel );
    XSetBackground( xwd->display, dev->gc, xwd->cmap0[0].pixel );

// Set fill rule.
    if ( pls->dev_eofill )
        XSetFillRule( xwd->display, dev->gc, EvenOddRule );
    else
        XSetFillRule( xwd->display, dev->gc, WindingRule );

// If main window, need to map it and wait for exposure
// Ugly Patch: use pls->arrow_npts=999 to make window hiden and no MapMain
    int hide=(pls->arrow_npts==999);
    if ( dev->is_main && !hide)
        MapMain( pls );
}

//--------------------------------------------------------------------------
// InitMain()
//
// Create main window using standard Xlib calls.
//--------------------------------------------------------------------------

static void
InitMain( PLStream *pls )
{
    XwDev      *dev = (XwDev *) pls->dev;
    XwDisplay  *xwd = (XwDisplay *) dev->xwd;

    Window     root;
    XSizeHints hint;
    int        x, y;
    U_INT      width, height, border, depth;

    dbug_enter( "InitMain" );

// Get root window geometry

    (void) XGetGeometry( xwd->display, DefaultRootWindow( xwd->display ),
        &root, &x, &y, &width, &height, &border, &depth );

// Set window size
// Need to be careful to set XSizeHints flags correctly

    hint.flags = 0;
    if ( pls->xlength == 0 && pls->ylength == 0 )
        hint.flags |= PSize;
    else
        hint.flags |= USSize;

    if ( pls->xlength == 0 )
        pls->xlength = (PLINT) ( width * 0.75 );
    if ( pls->ylength == 0 )
        pls->ylength = (PLINT) ( height * 0.75 );

    if ( pls->xlength > (short) width )
        pls->xlength = (PLINT) ( width - dev->border * 2 );
    if ( pls->ylength > (short) height )
        pls->ylength = (PLINT) ( height - dev->border * 2 );

    hint.width  = (int) pls->xlength;
    hint.height = (int) pls->ylength;
    dev->border = 5;

// Set window position if specified by the user.
// Otherwise leave up to the window manager

    if ( pls->xoffset != 0 || pls->yoffset != 0 )
    {
        hint.flags |= USPosition;
        hint.x      = (int) pls->xoffset;
        hint.y      = (int) pls->yoffset;
    }
    else
    {
        hint.x = 0;
        hint.y = 0;
    }

// Window creation

    dev->window =
        XCreateWindow( xwd->display,
            DefaultRootWindow( xwd->display ),
            hint.x, hint.y, (unsigned int) hint.width, (unsigned int) hint.height,
            dev->border, (int) xwd->depth,
            InputOutput, xwd->visual,
            0, NULL );

    XSetStandardProperties( xwd->display, dev->window, pls->plwindow, pls->plwindow,
        None, 0, 0, &hint );
}

//--------------------------------------------------------------------------
// MapMain()
//
// Sets up event handlers, maps main window and waits for exposure.
//--------------------------------------------------------------------------

static void
MapMain( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XEvent    event;

    dbug_enter( "MapMain" );

// Input event selection

    dev->event_mask =
        ButtonPressMask |
        KeyPressMask |
        ExposureMask |
        ButtonMotionMask |     // drag
        StructureNotifyMask;

    XSelectInput( xwd->display, dev->window, dev->event_mask );

// Window mapping

    XMapRaised( xwd->display, dev->window );

    Atom wmDelete = XInternAtom( xwd->display, "WM_DELETE_WINDOW", False );
    XSetWMProtocols( xwd->display, dev->window, &wmDelete, 1 );

// Wait for exposure
// Remove extraneous expose events from the event queue

    for (;; )
    {
        XWindowEvent( xwd->display, dev->window, dev->event_mask, &event );
        if ( event.type == Expose )
        {
            while ( XCheckWindowEvent( xwd->display, dev->window,
                ExposureMask, &event ) )
                ;
            break;
        }
    }
}

//--------------------------------------------------------------------------
// WaitForPage()
//
// This routine waits for the user to advance the plot, while handling
// all other events.
//--------------------------------------------------------------------------

static void
WaitForPage( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XEvent    event;

    dbug_enter( "WaitForPage" );

    while ( !dev->exit_eventloop )
    {
        // XWindowEvent( xwd->display, dev->window, dev->event_mask, &event );
        XNextEvent( xwd->display, &event );
        MasterEH( pls, &event );
    }
    dev->exit_eventloop = FALSE;
}

//--------------------------------------------------------------------------
// events_thread()
//
// This function is being running continuously by a thread and is
// responsible for dealing with expose and resize X events, in
// the case that the main program is too busy to honor them.
//
// Dealing with other X events is possible, but not desirable,
// e.g. treating the "Q" or right-mouse-button would terminate
// the thread (if this is desirable, the thread should kill the
// main program -- not thread aware -- and kill itself afterward).
//
// This works pretty well, but the main program *must* be linked
// with the pthread library, although not being thread aware.
// This happens automatically when linking against libplplot.so,
// but when building modules for extending some language such as
// Python or Octave, the language providing binary itself must be
// relinked with -lpthread.
//
//--------------------------------------------------------------------------

#ifdef PL_USE_PTHREADS_XWIN
static void
events_thread( void *pls )
{
    if ( usepthreads )
    {
        PLStream        *lpls = (PLStream *) pls;
        XwDev           *dev  = (XwDev *) lpls->dev;
        XwDisplay       *xwd  = (XwDisplay *) dev->xwd;
        PLStream        *oplsc;
        struct timespec delay;
        XEvent          event;
        long            event_mask;
        sigset_t        set;

        //
        // only treats exposures and resizes, but remove usual events from queue,
        // as it can be disturbing to not have them acknowledged in real time,
        // because the program is busy, and suddenly all being acknowledged.
        // Also, the locator ("L" key) is sluggish if driven by the thread.
        //
        // But this approach is a problem when the thread removes events
        // from the queue while the program is responsible! The user hits 'L'
        // and nothing happens, as the thread removes it.
        //
        // Perhaps the "Q" key should have a different treatment, quiting the
        // program anyway?
        //
        // Changed: does not remove non treated events from the queue
        //

        event_mask = ExposureMask | StructureNotifyMask;

        // block all signal for this thread
        sigemptyset( &set );
        // sigfillset(&set);  can't be all signals, decide latter
        sigaddset( &set, SIGINT );

        sigprocmask( SIG_BLOCK, &set, NULL );

        pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
        pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );

        delay.tv_sec  = 0;
        delay.tv_nsec = 10000000; // this thread runs 10 times a second. (1/10 ms)

        while ( 1 )
        {
            pthread_mutex_lock( &events_mutex );

            if ( dev->is_main && !lpls->plbuf_read &&
                 ++dev->instr % dev->max_instr == 0 )
            {
                dev->instr = 0;
                while ( XCheckWindowEvent( xwd->display, dev->window, event_mask, &event ) )
                {
                    // As ResizeEH() ends up calling plRemakePlot(), that
                    // indirectly uses the current stream, one needs to
                    // temporarily set plplot current stream to this thread's
                    // stream

                    oplsc = plsc;
                    plsc  = lpls;
                    switch ( event.type )
                    {
                    case Expose:
                        ExposeEH( lpls, &event );
                        break;
                    case ConfigureNotify:
                        ResizeEH( lpls, &event );
                        break;
                    }
                    plsc = oplsc;
                }
            }

            pthread_mutex_unlock( &events_mutex );
            nanosleep( &delay, NULL ); // 10ms in linux
            /* pthread_yield(NULL); */ /* this puts too much load on the CPU */
        }
    }
}
#endif

//--------------------------------------------------------------------------
// CheckForEvents()
//
// A front-end to HandleEvents(), which is only called if certain conditions
// are satisfied:
//
// - must be the creator of the main window (i.e. PLplot is handling the
//   X event loop by polling).
// - must not be in the middle of a plot redisplay (else the order of event
//   handling can become circuitous).
// - only query X for events and process them every dev->max_instr times
//   this function is called (good for performance since querying X is a
//   nontrivial performance hit).
//--------------------------------------------------------------------------

static void
CheckForEvents( PLStream *pls )
{
    XwDev *dev = (XwDev *) pls->dev;

    if ( dev->is_main &&
         !pls->plbuf_read &&
         ++dev->instr % dev->max_instr == 0 )
    {
        dev->instr = 0;
        HandleEvents( pls );
    }
}

//--------------------------------------------------------------------------
// HandleEvents()
//
// Just a front-end to MasterEH(), for use when not actually waiting for an
// event but only checking the event queue.
//--------------------------------------------------------------------------

static void
HandleEvents( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XEvent    event;

    while ( XCheckTypedWindowEvent( xwd->display, dev->window,
        ClientMessage, &event ) ||
            XCheckWindowEvent( xwd->display, dev->window,
                dev->event_mask, &event ) )
        MasterEH( pls, &event );
}

//--------------------------------------------------------------------------
// MasterEH()
//
// Master X event handler routine.
// Redirects control to routines to handle:
//    - keyboard events
//    - mouse events
//    - expose events
//    - resize events
//
// By supplying a master event handler, the user can take over all event
// processing.  If events other than those trapped by PLplot need handling,
// just call XSelectInput with the appropriate flags.  The default PLplot
// event handling can be modified arbitrarily by changing the event struct.
//--------------------------------------------------------------------------

static void
MasterEH( PLStream *pls, XEvent *event )
{
    XwDev *dev = (XwDev *) pls->dev;

    if ( dev->MasterEH != NULL )
        ( *dev->MasterEH )( pls, event );

    switch ( event->type )
    {
    case KeyPress:
        KeyEH( pls, event );
        break;

    case ButtonPress:
        ButtonEH( pls, event );
        break;

    case Expose:
        ExposeEH( pls, event );
        break;

    case ConfigureNotify:
        ResizeEH( pls, event );
        break;

    case MotionNotify:
        if ( event->xmotion.state )
            ButtonEH( pls, event ); // drag
        MotionEH( pls, event );
        break;

    case EnterNotify:
        EnterEH( pls, event );
        break;

    case LeaveNotify:
        LeaveEH( pls, event );
        break;

    case ClientMessage:
        ClientEH( pls, event );
        break;
    }
}

//--------------------------------------------------------------------------
// ClientEH()
//
// Event handler routine for client message events (WM_DELETE_WINDOW)
//--------------------------------------------------------------------------

static void
ClientEH( PLStream *pls, XEvent *event )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    if ( (Atom) event->xclient.data.l[0] == XInternAtom( xwd->display, "WM_DELETE_WINDOW", False ) )
    {
        pls->nopause        = TRUE;
        pls->stream_closed  = TRUE;
        dev->exit_eventloop = TRUE;
        // plexit( "" );
    }
}


//--------------------------------------------------------------------------
// KeyEH()
//
// Event handler routine for keyboard events.
//--------------------------------------------------------------------------

static void
KeyEH( PLStream *pls, XEvent *event )
{
    XwDev *dev = (XwDev *) pls->dev;

    dbug_enter( "KeyEH" );

    LookupXKeyEvent( pls, event );
    if ( dev->locate_mode )
        LocateKey( pls );
    else
        ProcessKey( pls );
}

//--------------------------------------------------------------------------
// ButtonEH()
//
// Event handler routine for ButtonPress events.
//--------------------------------------------------------------------------

static void
ButtonEH( PLStream *pls, XEvent *event )
{
    XwDev *dev = (XwDev *) pls->dev;

    dbug_enter( "ButtonEH" );

    LookupXButtonEvent( pls, event );
    if ( dev->locate_mode )
        LocateButton( pls );
    else
        ProcessButton( pls );
}

//--------------------------------------------------------------------------
// LookupXKeyEvent()
//
// Fills in the PLGraphicsIn from an XKeyEvent.  The PLGraphicsIn keysym is
// the same as the X keysym for all cases except for control keys that have
// ASCII equivalents, i.e.:
//
// Name		      X-keysym	ASCII	Ctrl-key
// ----		      --------	-----	--------
// XK_BackSpace        0xFF08   0x08      ^H
// XK_Tab              0xFF09   0x09      ^I
// XK_Linefeed         0xFF0A   0x0A      ^J
// XK_Return           0xFF0D   0x0D      ^M
// XK_Escape           0xFF1B   0x1B      ^[
// XK_Delete           0xFFFF   0xFF     (none)
//
// The ASCII representation of these characters is used for the PLGraphicsIn
// keysym to simplify code logic.  It turns out that the X keysyms are
// identical to the ASCII values with the upper 8 bits set.
//--------------------------------------------------------------------------

static void
LookupXKeyEvent( PLStream *pls, XEvent *event )
{
    XwDev          *dev      = (XwDev *) pls->dev;
    PLGraphicsIn   *gin      = &( dev->gin );
    XKeyEvent      *keyEvent = (XKeyEvent *) event;
    KeySym         keysym;
    int            nchars, ncmax = PL_MAXKEY - 1;
    XComposeStatus cs;

    gin->pX = keyEvent->x;
    gin->pY = keyEvent->y;
    gin->dX = (PLFLT) keyEvent->x / ( dev->width - 1 );
    gin->dY = 1.0 - (PLFLT) keyEvent->y / ( dev->height - 1 );

    gin->state = keyEvent->state;

    nchars = XLookupString( keyEvent, gin->string, ncmax, &keysym, &cs );
    gin->string[nchars] = '\0';

    pldebug( "LookupXKeyEvent",
        "Keysym %x, translation: %s\n", keysym, gin->string );

    switch ( keysym )
    {
    case XK_BackSpace:
    case XK_Tab:
    case XK_Linefeed:
    case XK_Return:
    case XK_Escape:
    case XK_Delete:
        gin->keysym = 0xFF & keysym;
        break;

    default:
        gin->keysym = (unsigned int) keysym;
    }
}

//--------------------------------------------------------------------------
// LookupXButtonEvent()
//
// Fills in the PLGraphicsIn from an XButtonEvent.
//--------------------------------------------------------------------------

static void
LookupXButtonEvent( PLStream *pls, XEvent *event )
{
    XwDev        *dev         = (XwDev *) pls->dev;
    PLGraphicsIn *gin         = &( dev->gin );
    XButtonEvent *buttonEvent = (XButtonEvent *) event;

    pldebug( "LookupXButtonEvent",
        "Button: %d, x: %d, y: %d\n",
        buttonEvent->button, buttonEvent->x, buttonEvent->y );

    gin->pX = buttonEvent->x;
    gin->pY = buttonEvent->y;
    gin->dX = (PLFLT) buttonEvent->x / ( dev->width - 1 );
    gin->dY = 1.0 - (PLFLT) buttonEvent->y / ( dev->height - 1 );

    gin->button = buttonEvent->button;
    gin->state  = buttonEvent->state;
    gin->keysym = 0x20;
}

//--------------------------------------------------------------------------
// ProcessKey()
//
// Process keyboard events other than locate input.
//--------------------------------------------------------------------------

static void
ProcessKey( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    PLGraphicsIn *gin = &( dev->gin );

    dbug_enter( "ProcessKey" );

// Call user keypress event handler.  Since this is called first, the user
// can disable all internal event handling by setting key.keysym to 0.
//
    if ( pls->KeyEH != NULL )
        ( *pls->KeyEH )( gin, pls->KeyEH_data, &dev->exit_eventloop );

// Handle internal events

    switch ( gin->keysym )
    {
    case PLK_Return:
    case PLK_Linefeed:
    case PLK_Next:
        // Advance to next page (i.e. terminate event loop) on a <eol>
        // Check for both <CR> and <LF> for portability, also a <Page Down>
        dev->exit_eventloop = TRUE;
        break;

    case 'Q':
        // Terminate on a 'Q' (not 'q', since it's too easy to hit by mistake)
        pls->nopause = TRUE;
        plexit( "" );
        break;

    case 'L':
        // Begin locate mode
        dev->locate_mode = LOCATE_INVOKED_VIA_DRIVER;
        CreateXhairs( pls );
        break;
    }
}

//--------------------------------------------------------------------------
// ProcessButton()
//
// Process ButtonPress events other than locate input.
// On:
//   Button1: nothing (except when in locate mode, see ButtonLocate)
//   Button2: nothing
//   Button3: set page advance flag
//--------------------------------------------------------------------------

static void
ProcessButton( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    PLGraphicsIn *gin = &( dev->gin );

    dbug_enter( "ProcessButton" );

// Call user event handler.  Since this is called first, the user can
// disable all PLplot internal event handling by setting gin->button to 0.
//
    if ( pls->ButtonEH != NULL )
        ( *pls->ButtonEH )( gin, pls->ButtonEH_data, &dev->exit_eventloop );

// Handle internal events

    switch ( gin->button )
    {
    case Button3:
        dev->exit_eventloop = TRUE;
        break;
    }
}

//--------------------------------------------------------------------------
// LocateKey()
//
// Front-end to locate handler for KeyPress events.
// Provides for a number of special effects:
//
//  <Escape>		Ends locate mode
//  <Cursor>		Moves cursor one pixel in the specified direction
//  <Mod-Cursor>	Accelerated cursor movement (5x for each modifier)
//--------------------------------------------------------------------------

static void
LocateKey( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    XwDisplay    *xwd = (XwDisplay *) dev->xwd;
    PLGraphicsIn *gin = &( dev->gin );

// End locate mode on <Escape>

    if ( gin->keysym == PLK_Escape )
    {
        dev->locate_mode = 0;
        DestroyXhairs( pls );
        plGinInit( gin );
    }

// Ignore modifier keys

    else if ( IsModifierKey( gin->keysym ) )
    {
        plGinInit( gin );
    }

// Now handle cursor keys

    else if ( IsCursorKey( gin->keysym ) )
    {
        int x1, y1, dx = 0, dy = 0;
        int xmin = 0, xmax = (int) dev->width - 1, ymin = 0, ymax = (int) dev->height - 1;

        switch ( gin->keysym )
        {
        case PLK_Left:
            dx = -1;
            break;
        case PLK_Right:
            dx = 1;
            break;
        case PLK_Up:
            dy = -1;
            break;
        case PLK_Down:
            dy = 1;
            break;
        }

        // Each modifier key added increases the multiplication factor by 5

        // Shift

        if ( gin->state & 0x01 )
        {
            dx *= 5;
            dy *= 5;
        }

        // Caps Lock

        if ( gin->state & 0x02 )
        {
            dx *= 5;
            dy *= 5;
        }

        // Control

        if ( gin->state & 0x04 )
        {
            dx *= 5;
            dy *= 5;
        }

        // Alt

        if ( gin->state & 0x08 )
        {
            dx *= 5;
            dy *= 5;
        }

        // Bounds checking so that we don't send cursor out of window

        x1 = gin->pX + dx;
        y1 = gin->pY + dy;

        if ( x1 < xmin )
            dx = xmin - gin->pX;
        if ( y1 < ymin )
            dy = ymin - gin->pY;
        if ( x1 > xmax )
            dx = xmax - gin->pX;
        if ( y1 > ymax )
            dy = ymax - gin->pY;

        // Engage...

        XWarpPointer( xwd->display, dev->window, None, 0, 0, 0, 0, dx, dy );
        plGinInit( gin );
    }

// Call ordinary locate handler

    else
    {
        Locate( pls );
    }
}

//--------------------------------------------------------------------------
// LocateButton()
//
// Front-end to locate handler for ButtonPress events.
// Only passes control to Locate() for Button1 presses.
//--------------------------------------------------------------------------

static void
LocateButton( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    PLGraphicsIn *gin = &( dev->gin );

    switch ( gin->button )
    {
    case Button1:
        Locate( pls );
        break;
    }
}

//--------------------------------------------------------------------------
// Locate()
//
// Handles locate mode events.
//
// In locate mode: move cursor to desired location and select by pressing a
// key or by clicking on the mouse (if available).  Typically the world
// coordinates of the selected point are reported.
//
// There are two ways to enter Locate mode -- via the API, or via a driver
// command.  The API entry point is the call plGetCursor(), which initiates
// locate mode and does not return until input has been obtained.  The
// driver entry point is by entering a 'L' while the driver is waiting for
// events.
//
// Locate mode input is reported in one of three ways:
// 1. Through a returned PLGraphicsIn structure, when user has specified a
//    locate handler via (*pls->LocateEH).
// 2. Through a returned PLGraphicsIn structure, when locate mode is invoked
//    by a plGetCursor() call.
// 3. Through writes to stdout, when locate mode is invoked by a driver
//    command and the user has not supplied a locate handler.
//
// Hitting <Escape> will at all times end locate mode.  Other keys will
// typically be interpreted as locator input.  Selecting a point out of
// bounds will end locate mode unless the user overrides with a supplied
// Locate handler.
//--------------------------------------------------------------------------

static void
Locate( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    PLGraphicsIn *gin = &( dev->gin );

// Call user locate mode handler if provided

    if ( pls->LocateEH != NULL )
        ( *pls->LocateEH )( gin, pls->LocateEH_data, &dev->locate_mode );

// Use default procedure

    else
    {
        // Try to locate cursor

        if ( plTranslateCursor( gin ) )
        {
            // If invoked by the API, we're done
            // Otherwise send report to stdout

            if ( dev->locate_mode == LOCATE_INVOKED_VIA_DRIVER )
            {
                pltext();
                if ( gin->keysym < 0xFF && isprint( gin->keysym ) )
                    printf( "%f %f %c\n", gin->wX, gin->wY, gin->keysym );
                else
                    printf( "%f %f 0x%02x\n", gin->wX, gin->wY, gin->keysym );

                plgra();
            }
        }
        else
        {
            // Selected point is out of bounds, so end locate mode

            dev->locate_mode = 0;
            DestroyXhairs( pls );
        }
    }
}

//--------------------------------------------------------------------------
// MotionEH()
//
// Event handler routine for MotionNotify events.
// If drawing crosshairs, the first and last draws must be done "by hand".
//--------------------------------------------------------------------------

static void
MotionEH( PLStream *pls, XEvent *event )
{
    XwDev        *dev         = (XwDev *) pls->dev;
    XMotionEvent *motionEvent = (XMotionEvent *) event;

    if ( dev->drawing_xhairs )
    {
        DrawXhairs( pls, motionEvent->x, motionEvent->y );
    }
}

//--------------------------------------------------------------------------
// EnterEH()
//
// Event handler routine for EnterNotify events.  Only called if drawing
// crosshairs -- a draw must be done here to start off the new set.
//--------------------------------------------------------------------------

static void
EnterEH( PLStream *pls, XEvent *event )
{
    XwDev          *dev           = (XwDev *) pls->dev;
    XCrossingEvent *crossingEvent = (XCrossingEvent *) event;

    DrawXhairs( pls, crossingEvent->x, crossingEvent->y );
    dev->drawing_xhairs = 1;
}

//--------------------------------------------------------------------------
// LeaveEH()
//
// Event handler routine for EnterNotify or LeaveNotify events.
// If drawing crosshairs, a draw must be done here to start off the new
// set or erase the last set.
//--------------------------------------------------------------------------

static void
LeaveEH( PLStream *pls, XEvent * PL_UNUSED( event ) )
{
    XwDev *dev = (XwDev *) pls->dev;

    UpdateXhairs( pls );
    dev->drawing_xhairs = 0;
}

//--------------------------------------------------------------------------
// CreateXhairs()
//
// Creates graphic crosshairs at current pointer location.
//--------------------------------------------------------------------------

static void
CreateXhairs( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    XwDisplay    *xwd = (XwDisplay *) dev->xwd;
    Window       root, child;
    int          root_x, root_y, win_x, win_y;
    unsigned int mask;
    XEvent       event;

// Get a crosshair cursor and switch to it.

    if ( !xwd->xhair_cursor )
        xwd->xhair_cursor = XCreateFontCursor( xwd->display, XC_crosshair );

    XDefineCursor( xwd->display, dev->window, xwd->xhair_cursor );

// Find current pointer location and draw graphic crosshairs if pointer is
// inside our window

    if ( XQueryPointer( xwd->display, dev->window, &root, &child,
        &root_x, &root_y, &win_x, &win_y, &mask ) )
    {
        if ( win_x >= 0 && win_x < (int) dev->width &&
             win_y >= 0 && win_y < (int) dev->height )
        {
            DrawXhairs( pls, win_x, win_y );
            dev->drawing_xhairs = 1;
        }
    }

// Sync the display and then throw away all pending motion events

    XSync( xwd->display, 0 );
    while ( XCheckWindowEvent( xwd->display, dev->window,
        PointerMotionMask, &event ) )
        ;

// Catch PointerMotion and crossing events so we can update them properly

    dev->event_mask |= PointerMotionMask | EnterWindowMask | LeaveWindowMask;
    XSelectInput( xwd->display, dev->window, dev->event_mask );
}

//--------------------------------------------------------------------------
// DestroyXhairs()
//
// Destroys graphic crosshairs.
//--------------------------------------------------------------------------

static void
DestroyXhairs( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

// Switch back to boring old pointer

    XUndefineCursor( xwd->display, dev->window );

// Don't catch PointerMotion or crossing events any more

    dev->event_mask &=
        ~PointerMotionMask & ~EnterWindowMask & ~LeaveWindowMask;
    XSelectInput( xwd->display, dev->window, dev->event_mask );

// This draw removes the last set of graphic crosshairs

    UpdateXhairs( pls );
    dev->drawing_xhairs = 0;
}

//--------------------------------------------------------------------------
// DrawXhairs()
//
// Draws graphic crosshairs at (x0, y0).  The first draw erases the old set.
//--------------------------------------------------------------------------

static void
DrawXhairs( PLStream *pls, int x0, int y0 )
{
    XwDev *dev = (XwDev *) pls->dev;

    int   xmin = 0, xmax = (int) dev->width - 1;
    int   ymin = 0, ymax = (int) dev->height - 1;

    if ( dev->drawing_xhairs )
        UpdateXhairs( pls );

    dev->xhair_x[0].x = (short) xmin; dev->xhair_x[0].y = (short) y0;
    dev->xhair_x[1].x = (short) xmax; dev->xhair_x[1].y = (short) y0;

    dev->xhair_y[0].x = (short) x0; dev->xhair_y[0].y = (short) ymin;
    dev->xhair_y[1].x = (short) x0; dev->xhair_y[1].y = (short) ymax;

    UpdateXhairs( pls );
}

//--------------------------------------------------------------------------
// UpdateXhairs()
//
// Updates graphic crosshairs.  If already there, they are erased.
//--------------------------------------------------------------------------

static void
UpdateXhairs( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_x, 2,
        CoordModeOrigin );

    XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_y, 2,
        CoordModeOrigin );
}

//--------------------------------------------------------------------------
// ExposeEH()
//
// Event handler routine for expose events.
// Front end to ExposeCmd() to deal with wierdnesses of Xlib.
//--------------------------------------------------------------------------

static void
ExposeEH( PLStream *pls, XEvent *event )
{
    XwDev        *dev         = (XwDev *) pls->dev;
    XwDisplay    *xwd         = (XwDisplay *) dev->xwd;
    XExposeEvent *exposeEvent = (XExposeEvent *) event;
    PLDisplay    pldis;
    int          redrawn;

    dbug_enter( "ExposeEH" );

    pldebug( "ExposeEH",
        "x = %d, y = %d, width = %d, height = %d, count = %d, pending = %d\n",
        exposeEvent->x, exposeEvent->y,
        exposeEvent->width, exposeEvent->height,
        exposeEvent->count, XPending( xwd->display ) );

// Handle expose
// If we have anything overlaid (like crosshairs), we need to refresh the
// entire plot in order to have a predictable outcome.  In this case we
// need to first clear window.  Otherwise it's better to not clear it, for a
// smoother redraw (unobscured regions appear to stay the same).

    if ( dev->drawing_xhairs )
    {
        XClearWindow( xwd->display, dev->window );
        ExposeCmd( pls, NULL );
        UpdateXhairs( pls );
        redrawn = 1;
    }
    else
    {
        pldis.x      = (unsigned int) exposeEvent->x;
        pldis.y      = (unsigned int) exposeEvent->y;
        pldis.width  = (unsigned int) exposeEvent->width;
        pldis.height = (unsigned int) exposeEvent->height;

        ExposeCmd( pls, &pldis );
        redrawn = !dev->write_to_pixmap;
    }

// If entire plot was redrawn, remove extraneous events from the queue

    if ( redrawn )
        while ( XCheckWindowEvent( xwd->display, dev->window,
            ExposureMask | StructureNotifyMask, event ) )
            ;
}

//--------------------------------------------------------------------------
// ResizeEH()
//
// Event handler routine for resize events.
// Front end to ResizeCmd() to deal with wierdnesses of Xlib.
//--------------------------------------------------------------------------

static void
ResizeEH( PLStream *pls, XEvent *event )
{
    XwDev           *dev         = (XwDev *) pls->dev;
    XwDisplay       *xwd         = (XwDisplay *) dev->xwd;
    XConfigureEvent *configEvent = (XConfigureEvent *) event;
    PLDisplay       pldis;

    dbug_enter( "ResizeEH" );

    pldis.width  = (unsigned int) configEvent->width;
    pldis.height = (unsigned int) configEvent->height;

// Only need to resize if size is actually changed

    if ( pldis.width == dev->width && pldis.height == dev->height )
        return;

    pldebug( "ResizeEH",
        "x = %d, y = %d, pending = %d\n",
        configEvent->width, configEvent->height, XPending( xwd->display ) );

// Handle resize

    ResizeCmd( pls, &pldis );
    if ( dev->drawing_xhairs )
    {
        UpdateXhairs( pls );
    }

// Remove extraneous Expose and ConfigureNotify events from the event queue
// Exposes do not need to be handled since we've redrawn the whole plot

    XFlush( xwd->display );
    while ( XCheckWindowEvent( xwd->display, dev->window,
        ExposureMask | StructureNotifyMask, event ) )
        ;
}

//--------------------------------------------------------------------------
// ExposeCmd()
//
// Event handler routine for expose events.
// These are "pure" exposures (no resize), so don't need to clear window.
//--------------------------------------------------------------------------

static void
ExposeCmd( PLStream *pls, PLDisplay *pldis )
{
    XwDev        *dev = (XwDev *) pls->dev;
    XwDisplay    *xwd = (XwDisplay *) dev->xwd;
    int          x, y;
    unsigned int width, height;

    dbug_enter( "ExposeCmd" );

// Return if plD_init_xw hasn't been called yet

    if ( dev == NULL )
    {
        plwarn( "ExposeCmd: Illegal call -- driver uninitialized" );
        return;
    }

// Exposed area.  If unspecified, the entire window is used.

    if ( pldis == NULL )
    {
        x      = 0;
        y      = 0;
        width  = dev->width;
        height = dev->height;
    }
    else
    {
        x      = (int) pldis->x;
        y      = (int) pldis->y;
        width  = pldis->width;
        height = pldis->height;
    }

// Usual case: refresh window from pixmap
// DEBUG option: draws rectangle around refreshed region

    XSync( xwd->display, 0 );
    if ( dev->write_to_pixmap )
    {
        XCopyArea( xwd->display, dev->pixmap, dev->window, dev->gc,
            x, y, width, height, x, y );
        XSync( xwd->display, 0 );
#ifdef DEBUG
        if ( pls->debug )
        {
            XPoint pts[5];
            int    x0 = x, x1 = x + (int) width, y0 = y, y1 = y + (int) height;
            pts[0].x = (short) x0; pts[0].y = (short) y0;
            pts[1].x = (short) x1; pts[1].y = (short) y0;
            pts[2].x = (short) x1; pts[2].y = (short) y1;
            pts[3].x = (short) x0; pts[3].y = (short) y1;
            pts[4].x = (short) x0; pts[4].y = (short) y0;

            XDrawLines( xwd->display, dev->window, dev->gc, pts, 5,
                CoordModeOrigin );
        }
#endif
    }
    else
    {
        plRemakePlot( pls );
        XFlush( xwd->display );
    }
}

//--------------------------------------------------------------------------
// ResizeCmd()
//
// Event handler routine for resize events.
//--------------------------------------------------------------------------

static void
ResizeCmd( PLStream *pls, PLDisplay *pldis )
{
    XwDev     *dev            = (XwDev *) pls->dev;
    XwDisplay *xwd            = (XwDisplay *) dev->xwd;
    int       write_to_window = dev->write_to_window;

    dbug_enter( "ResizeCmd" );

// Return if plD_init_xw hasn't been called yet

    if ( dev == NULL )
    {
        plwarn( "ResizeCmd: Illegal call -- driver uninitialized" );
        return;
    }

// Return if pointer to window not specified.

    if ( pldis == NULL )
    {
        plwarn( "ResizeCmd: Illegal call -- window pointer uninitialized" );
        return;
    }

// Reset current window bounds

    dev->width  = pldis->width;
    dev->height = pldis->height;

    dev->xscale = dev->width / (double) dev->init_width;
    dev->yscale = dev->height / (double) dev->init_height;

    dev->xscale = dev->xscale * dev->xscale_init;
    dev->yscale = dev->yscale * dev->yscale_init;

#if PHYSICAL
    {
        PLFLT pxlx = DPMM / dev->xscale;
        PLFLT pxly = DPMM / dev->yscale;
        plP_setpxl( pxlx, pxly );
    }
#endif

// Note: the following order MUST be obeyed -- if you instead redraw into
// the window and then copy it to the pixmap, off-screen parts of the window
// may contain garbage which is then transferred to the pixmap (and thus
// will not go away after an expose).
//

// Resize pixmap using new dimensions

    if ( dev->write_to_pixmap )
    {
        dev->write_to_window = 0;
        XFreePixmap( xwd->display, dev->pixmap );
        CreatePixmap( pls );
    }

// This allows an external agent to take over the redraw
    if ( pls->ext_resize_draw )
        return;

// Initialize & redraw (to pixmap, if available).

    if ( dev->write_to_pixmap )
    {
        XSetForeground( xwd->display, dev->gc, dev->bgcolor.pixel );
        XFillRectangle( xwd->display, dev->pixmap, dev->gc, 0, 0,
            dev->width, dev->height );
        XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
    }
    if ( dev->write_to_window )
    {
        XClearWindow( xwd->display, dev->window );
    }
    plRemakePlot( pls );
    XSync( xwd->display, 0 );

// If pixmap available, fake an expose

    if ( dev->write_to_pixmap )
    {
        dev->write_to_window = write_to_window;
        XCopyArea( xwd->display, dev->pixmap, dev->window, dev->gc, 0, 0,
            dev->width, dev->height, 0, 0 );
        XSync( xwd->display, 0 );
    }
}

//--------------------------------------------------------------------------
// ConfigBufferingCmd()
//
// Allows a widget to manipulate the double buffering support in the
// xwin dirver.
//--------------------------------------------------------------------------

static void ConfigBufferingCmd( PLStream *pls, PLBufferingCB *ptr )
{
    XwDev *dev = (XwDev *) pls->dev;

    switch ( ptr->cmd )
    {
    case PLESC_DOUBLEBUFFERING_ENABLE:
        dev->write_to_window = 0;
        pls->db = 1;
        break;

    case PLESC_DOUBLEBUFFERING_DISABLE:
        dev->write_to_window = 1;
        pls->db = 0;
        break;

    case PLESC_DOUBLEBUFFERING_QUERY:
        ptr->result = pls->db;
        break;

    default:
        printf( "Unrecognized buffering request ignored.\n" );
        break;
    }
}

//--------------------------------------------------------------------------
// RedrawCmd()
//
// Handles page redraw without resize (pixmap does not get reallocated).
// Calling this makes sure all necessary housekeeping gets done.
//--------------------------------------------------------------------------

static void
RedrawCmd( PLStream *pls )
{
    XwDev     *dev            = (XwDev *) pls->dev;
    XwDisplay *xwd            = (XwDisplay *) dev->xwd;
    int       write_to_window = dev->write_to_window;

    dbug_enter( "RedrawCmd" );

// Return if plD_init_xw hasn't been called yet

    if ( dev == NULL )
    {
        plwarn( "RedrawCmd: Illegal call -- driver uninitialized" );
        return;
    }

// Initialize & redraw (to pixmap, if available).

    if ( dev->write_to_pixmap )
    {
        dev->write_to_window = 0;
        XSetForeground( xwd->display, dev->gc, dev->bgcolor.pixel );
        XFillRectangle( xwd->display, dev->pixmap, dev->gc, 0, 0,
            dev->width, dev->height );
        XSetForeground( xwd->display, dev->gc, dev->curcolor.pixel );
    }
    if ( dev->write_to_window )
    {
        XClearWindow( xwd->display, dev->window );
    }
    plRemakePlot( pls );
    XSync( xwd->display, 0 );

    dev->write_to_window = write_to_window;

// If pixmap available, fake an expose

    if ( dev->write_to_pixmap )
    {
        XCopyArea( xwd->display, dev->pixmap, dev->window, dev->gc, 0, 0,
            dev->width, dev->height, 0, 0 );
        XSync( xwd->display, 0 );
    }
}

//--------------------------------------------------------------------------
// CreatePixmapErrorHandler()
//
// Error handler used in CreatePixmap() to catch errors in allocating
// storage for pixmap.  This way we can nicely substitute redraws for
// pixmap copies if the server has insufficient memory.
//--------------------------------------------------------------------------

static unsigned char CreatePixmapStatus;

static int
CreatePixmapErrorHandler( Display *display, XErrorEvent *error )
{
    CreatePixmapStatus = error->error_code;
    if ( error->error_code != BadAlloc )
    {
        char buffer[256];
        XGetErrorText( display, error->error_code, buffer, 256 );
        fprintf( stderr, "Error in XCreatePixmap: %s.\n", buffer );
    }
    return 1;
}

//--------------------------------------------------------------------------
// CreatePixmap()
//
// This routine creates a pixmap, doing error trapping in case there
// isn't enough memory on the server.
//--------------------------------------------------------------------------

static void
CreatePixmap( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    int       ( *oldErrorHandler )( Display *, XErrorEvent * );

    oldErrorHandler = XSetErrorHandler( CreatePixmapErrorHandler );

    CreatePixmapStatus = Success;
    pldebug( "CreatePixmap",
        "creating pixmap: width = %d, height = %d, depth = %d\n",
        dev->width, dev->height, xwd->depth );

    dev->pixmap = XCreatePixmap( xwd->display, dev->window,
        dev->width, dev->height, xwd->depth );
    XSync( xwd->display, 0 );
    if ( CreatePixmapStatus != Success )
    {
        dev->write_to_pixmap = 0;
        dev->write_to_window = 1;
        pls->db = 0;
        fprintf( stderr, "\n\
Warning: pixmap could not be allocated (insufficient memory on server).\n\
Driver will redraw the entire plot to handle expose events.\n" );
    }

    XSetErrorHandler( oldErrorHandler );
}

//--------------------------------------------------------------------------
// GetVisual()
//
// Get visual info.  In order to safely use a visual other than that of
// the parent (which hopefully is that returned by DefaultVisual), you
// must first find (using XGetRGBColormaps) or create a colormap matching
// this visual and then set the colormap window attribute in the
// XCreateWindow attributes and valuemask arguments.  I don't do this
// right now, so this is turned off by default.
//--------------------------------------------------------------------------

static void
GetVisual( PLStream *pls )
{
    XwDev     *dev            = (XwDev *) pls->dev;
    XwDisplay *xwd            = (XwDisplay *) dev->xwd;
    int       visuals_matched = 0;

    dbug_enter( "GetVisual" );

    if ( !defaultvisual )
    {
        XVisualInfo vTemplate, *visualList;

// Try for an 8 plane display, if unavailable go for the default

        vTemplate.screen = xwd->screen;
        vTemplate.depth  = 8;

        visualList = XGetVisualInfo( xwd->display,
            VisualScreenMask | VisualDepthMask,
            &vTemplate, &visuals_matched );

#ifdef HACK_STATICCOLOR
        if ( visuals_matched )
        {
            int i, found = 0;
            printf( "visuals_matched = %d\n", visuals_matched );
            for ( i = 0; i < visuals_matched && !found; i++ )
            {
                Visual *v = visualList[i].visual;
                printf( "Checking visual %d: ", i );
                switch ( v->class )
                {
                case PseudoColor:
                    printf( "PseudoColor\n" );
                    break;
                case GrayScale:
                    printf( "GrayScale\n" );
                    break;
                case DirectColor:
                    printf( "DirectColor\n" );
                    break;
                case TrueColor:
                    printf( "TrueColor\n" );
                    break;
                case StaticColor:
                    printf( "StaticColor\n" );
                    break;
                case StaticGray:
                    printf( "StaticGray\n" );
                    break;
                default:
                    printf( "Unknown.\n" );
                    break;
                }
                if ( v->class == StaticColor )
                {
                    xwd->visual = v;
                    xwd->depth  = visualList[i].depth;
                    found       = 1;
                }
            }
            if ( !found )
            {
                plexit( "Unable to get a StaticColor visual." );
            }
            printf( "Found StaticColor visual, depth=%d\n", xwd->depth );
        }
#else
        if ( visuals_matched )
        {
            xwd->visual = visualList->visual;   // Choose first match.
            xwd->depth  = (unsigned int) vTemplate.depth;
        }
#endif  // HACK_STATICCOLOR
    }

    if ( !visuals_matched )
    {
        xwd->visual = DefaultVisual( xwd->display, xwd->screen );
        xwd->depth  = (unsigned int) DefaultDepth( xwd->display, xwd->screen );
    }

// Check to see if we expect to be able to allocate r/w color cells.

    switch ( xwd->visual->class )
    {
    case TrueColor:
    case StaticColor:
    case StaticGray:
        xwd->rw_cmap = 0;
        break;
    default:
        xwd->rw_cmap = 1;
    }

/*xwd->rw_cmap = 0;*/ /* debugging hack. */

// Just for kicks, see what kind of visual we got.

    if ( pls->verbose )
    {
        fprintf( stderr, "XVisual class == " );
        switch ( xwd->visual->class )
        {
        case PseudoColor:
            fprintf( stderr, "PseudoColor\n" );
            break;
        case GrayScale:
            fprintf( stderr, "GrayScale\n" );
            break;
        case DirectColor:
            fprintf( stderr, "DirectColor\n" );
            break;
        case TrueColor:
            fprintf( stderr, "TrueColor\n" );
            break;
        case StaticColor:
            fprintf( stderr, "StaticColor\n" );
            break;
        case StaticGray:
            fprintf( stderr, "StaticGray\n" );
            break;
        default:
            fprintf( stderr, "Unknown.\n" );
            break;
        }
        fprintf( stderr, "xwd->rw_cmap = %d\n", xwd->rw_cmap );
    }
}

//--------------------------------------------------------------------------
// AllocBGFG()
//
// Allocate background & foreground colors.  If possible, I choose pixel
// values such that the fg pixel is the xor of the bg pixel, to make
// rubber-banding easy to see.
//--------------------------------------------------------------------------

static void
AllocBGFG( PLStream *pls )
{
    XwDev         *dev = (XwDev *) pls->dev;
    XwDisplay     *xwd = (XwDisplay *) dev->xwd;

    int           i, j, npixels;
    unsigned long plane_masks[1], pixels[RWMAP_MAX_COLORS];

    dbug_enter( "AllocBGFG" );

// If not on a color system, just return
    if ( !xwd->color )
        return;

    if ( xwd->rw_cmap &&
         // r/w color maps
         XAllocColorCells( xwd->display, xwd->map, False,
             plane_masks, 0, pixels, 1 ) )
    {
        // background
        xwd->cmap0[0].pixel = pixels[0];
    }
    else
    {
        // r/o color maps
        xwd->cmap0[0].pixel = BlackPixel( xwd->display, xwd->screen );
        xwd->fgcolor.pixel  = WhitePixel( xwd->display, xwd->screen );
        if ( xwd->rw_cmap && pls->verbose )
            fprintf( stderr, "Downgrading to r/o cmap.\n" );
        xwd->rw_cmap = 0;
        return;
    }

// Allocate as many colors as we can

    npixels = RWMAP_MAX_COLORS;
    for (;; )
    {
        if ( XAllocColorCells( xwd->display, xwd->map, False,
            plane_masks, 0, pixels, (unsigned int) npixels ) )
            break;
        npixels--;
        if ( npixels == 0 )
            break;
    }

// Find the color with pixel = xor of the bg color pixel.
// If a match isn't found, the last pixel allocated is used.

    for ( i = 0; i < npixels - 1; i++ )
    {
        if ( pixels[i] == ( ~xwd->cmap0[0].pixel & 0xFF ) )
            break;
    }

// Use this color cell for our foreground color.  Then free the rest.

    xwd->fgcolor.pixel = pixels[i];
    for ( j = 0; j < npixels; j++ )
    {
        if ( j != i )
            XFreeColors( xwd->display, xwd->map, &pixels[j], 1, 0 );
    }
}

//--------------------------------------------------------------------------
// SetBGFG()
//
// Set background & foreground colors.  Foreground over background should
// have high contrast.
//--------------------------------------------------------------------------

static void
SetBGFG( PLStream *pls )
{
    XwDev        *dev = (XwDev *) pls->dev;
    XwDisplay    *xwd = (XwDisplay *) dev->xwd;

    PLColor      fgcolor;
    unsigned int gslevbg, gslevfg;

    dbug_enter( "SetBGFG" );

//
// Set background color.
//
// Background defaults to black on color screens, white on grayscale (many
// grayscale monitors have poor contrast, and black-on-white looks better).
//

    if ( !xwd->color )
    {
        pls->cmap0[0].r = pls->cmap0[0].g = pls->cmap0[0].b = 0xFF;
    }
    gslevbg = (unsigned int) ( ( (long) pls->cmap0[0].r +
                                 (long) pls->cmap0[0].g +
                                 (long) pls->cmap0[0].b ) / 3 );

    PLColor_to_XColor( &pls->cmap0[0], &xwd->cmap0[0] );

//
// Set foreground color.
//
// Used for grayscale output, since otherwise the plots can become nearly
// unreadable (i.e. if colors get mapped onto grayscale values).  In this
// case it becomes the grayscale level for all draws, and is taken to be
// black if the background is light, and white if the background is dark.
// Note that white/black allocations never fail.
//

    if ( gslevbg > 0x7F )
        gslevfg = 0;
    else
        gslevfg = 0xFF;

    fgcolor.r = fgcolor.g = fgcolor.b = (unsigned char) gslevfg;

    PLColor_to_XColor( &fgcolor, &xwd->fgcolor );

// Now store

    if ( xwd->rw_cmap && xwd->color )
    {
        XStoreColor( xwd->display, xwd->map, &xwd->fgcolor );
        XStoreColor( xwd->display, xwd->map, &xwd->cmap0[0] );
    }
    else
    {
        XAllocColor( xwd->display, xwd->map, &xwd->cmap0[0] );
        XAllocColor( xwd->display, xwd->map, &xwd->fgcolor );
    }
}

//--------------------------------------------------------------------------
// InitColors()
//
// Does all color initialization.
//--------------------------------------------------------------------------

static void
InitColors( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    dbug_enter( "InitColors" );

// Allocate and initialize color maps.
// Defer cmap1 allocation until it's actually used

    if ( xwd->color )
    {
        if ( plplot_ccmap )
        {
            AllocCustomMap( pls );
        }
        else
        {
            AllocCmap0( pls );
        }
    }
}

//--------------------------------------------------------------------------
// AllocCustomMap()
//
// Initializes custom color map and all the cruft that goes with it.
//
// Assuming all color X displays do 256 colors, the breakdown is as follows:
//
// CCMAP_XWM_COLORS
//	Number of low "pixel" values to copy.  These are typically allocated
//	first, thus are in use by the window manager. I copy them to reduce
//	flicker.
//
//
// RWMAP_CMAP1_COLORS
//	Color map 1 entries.  There should be as many as practical available
//	for smooth shading.  On the order of 50-100 is pretty reasonable.  You
//	don't really need all 256, especially if all you're going to do is to
//	print it to postscript (which doesn't have any intrinsic limitation on
//	the number of colors).
//
// It's important to leave some extra colors unallocated for Tk.  In
// particular the palette tools require a fair amount.  I recommend leaving
// at least 40 or so free.
//--------------------------------------------------------------------------

static void
AllocCustomMap( PLStream *pls )
{
    XwDev         *dev = (XwDev *) pls->dev;
    XwDisplay     *xwd = (XwDisplay *) dev->xwd;

    XColor        xwm_colors[RWMAP_MAX_COLORS];
    int           i, npixels;
    unsigned long plane_masks[1], pixels[RWMAP_MAX_COLORS];

    dbug_enter( "AllocCustomMap" );

// Determine current default colors

    for ( i = 0; i < RWMAP_MAX_COLORS; i++ )
    {
        xwm_colors[i].pixel = (unsigned long int) i;
    }
    XQueryColors( xwd->display, xwd->map, xwm_colors, RWMAP_MAX_COLORS );

// Allocate cmap0 colors in the default colormap.
// The custom cmap0 colors are later stored at the same pixel values.
// This is a really cool trick to reduce the flicker when changing colormaps.
//

    AllocCmap0( pls );
    XAllocColor( xwd->display, xwd->map, &xwd->fgcolor );

// Create new color map

    xwd->map = XCreateColormap( xwd->display, DefaultRootWindow( xwd->display ),
        xwd->visual, AllocNone );

// Now allocate all colors so we can fill the ones we want to copy

    npixels = RWMAP_MAX_COLORS;
    for (;; )
    {
        if ( XAllocColorCells( xwd->display, xwd->map, False,
            plane_masks, 0, pixels, (unsigned int) npixels ) )
            break;
        npixels--;
        if ( npixels == 0 )
            plexit( "couldn't allocate any colors" );
    }

// Fill the low colors since those are in use by the window manager

    for ( i = 0; i < CCMAP_XWM_COLORS; i++ )
    {
        XStoreColor( xwd->display, xwd->map, &xwm_colors[i] );
        pixels[xwm_colors[i].pixel] = 0;
    }

// Fill the ones we will use in cmap0

    for ( i = 0; i < xwd->ncol0; i++ )
    {
        XStoreColor( xwd->display, xwd->map, &xwd->cmap0[i] );
        pixels[xwd->cmap0[i].pixel] = 0;
    }

// Finally, if the colormap was saved by an external agent, see if there are
// any differences from the current default map and save those!  A very cool
// (or sick, depending on how you look at it) trick to get over some X and
// Tk limitations.
//

    if ( sxwm_colors_set )
    {
        for ( i = 0; i < RWMAP_MAX_COLORS; i++ )
        {
            if ( ( xwm_colors[i].red != sxwm_colors[i].red ) ||
                 ( xwm_colors[i].green != sxwm_colors[i].green ) ||
                 ( xwm_colors[i].blue != sxwm_colors[i].blue ) )
            {
                if ( pixels[i] != 0 )
                {
                    XStoreColor( xwd->display, xwd->map, &xwm_colors[i] );
                    pixels[i] = 0;
                }
            }
        }
    }

// Now free the ones we're not interested in

    for ( i = 0; i < npixels; i++ )
    {
        if ( pixels[i] != 0 )
            XFreeColors( xwd->display, xwd->map, &pixels[i], 1, 0 );
    }

// Allocate colors in cmap 1

    AllocCmap1( pls );
}

//--------------------------------------------------------------------------
// AllocCmap0()
//
// Allocate & initialize cmap0 entries.
//--------------------------------------------------------------------------

static void
AllocCmap0( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    int       i;

    dbug_enter( "AllocCmap0" );

// Free all previous colors.  This should work for both rw & ro colormaps
    for ( i = 1; i < xwd->ncol0; i++ )
    {
        unsigned long pixel = xwd->cmap0[i].pixel;
        XFreeColors( xwd->display, xwd->map, &pixel, 1, 0 );
    }

// If the number of colors increased, need to allocate enough space for them
    if ( pls->ncol0 > xwd->ncol0_alloc )
    {
        xwd->ncol0_alloc = pls->ncol0;
        xwd->cmap0       = (XColor *)
                           realloc( xwd->cmap0, (size_t) pls->ncol0 * sizeof ( XColor ) );
        if ( xwd->cmap0 == 0 )
            plexit( "couldn't allocate space for cmap0 colors" );
    }

    if ( xwd->rw_cmap )
    {
        int           npixels;
        unsigned long plane_masks[1], pixels[RWMAP_MAX_COLORS];

        // Allocate and assign colors in cmap 0

        npixels = pls->ncol0 - 1;
        for (;; )
        {
            if ( XAllocColorCells( xwd->display, xwd->map, False,
                plane_masks, 0, &pixels[1], (unsigned int) npixels ) )
                break;
            npixels--;
            if ( npixels == 0 )
                plexit( "couldn't allocate any colors" );
        }

        xwd->ncol0 = npixels + 1;
        for ( i = 1; i < xwd->ncol0; i++ )
        {
            xwd->cmap0[i].pixel = pixels[i];
        }

        StoreCmap0( pls );
    }
    else
    {
        if ( pls->verbose )
            fprintf( stderr, "Attempting to allocate r/o colors in cmap0.\n" );

        for ( i = 1; i < pls->ncol0; i++ )
        {
            int    r;
            XColor c;
            PLColor_to_XColor( &pls->cmap0[i], &c );
            r = XAllocColor( xwd->display, xwd->map, &c );
            if ( pls->verbose )
                fprintf( stderr, "i=%d, r=%d, pixel=%d\n", i, r, (int) c.pixel );
            if ( r )
            {
                xwd->cmap0[i]       = c;
                xwd->cmap0[i].pixel = c.pixel; // needed for deallocation
            }
            else
            {
                XColor screen_def, exact_def;

                if ( pls->verbose )
                    fprintf( stderr,
                        "color alloc failed, trying by name: %s.\n",
                        pls->cmap0[i].name );

                // Hmm, didn't work, try another approach.
                r = XAllocNamedColor( xwd->display, xwd->map,
                    pls->cmap0[i].name,
                    &screen_def, &exact_def );

//                 xwd->cmap0[i] = screen_def;

                if ( r )
                {
                    if ( pls->verbose )
                        fprintf( stderr, "yes, got a color by name.\n" );

                    xwd->cmap0[i]       = screen_def;
                    xwd->cmap0[i].pixel = screen_def.pixel;
                }
                else
                {
                    r = XAllocNamedColor( xwd->display, xwd->map,
                        "white",
                        &screen_def, &exact_def );
                    if ( r )
                    {
                        xwd->cmap0[i]       = screen_def;
                        xwd->cmap0[i].pixel = screen_def.pixel;
                    }
                    else
                        printf( "Can't find white?! Giving up...\n" );
                }
            }
        }
        xwd->ncol0 = i;

        if ( pls->verbose )
            fprintf( stderr, "Allocated %d colors in cmap0.\n", xwd->ncol0 );
    }
}

//--------------------------------------------------------------------------
// AllocCmap1()
//
// Allocate & initialize cmap1 entries.
//--------------------------------------------------------------------------

static void
AllocCmap1( PLStream *pls )
{
    XwDev         *dev = (XwDev *) pls->dev;
    XwDisplay     *xwd = (XwDisplay *) dev->xwd;

    int           i, j, npixels;
    unsigned long plane_masks[1], pixels[RWMAP_MAX_COLORS];

    dbug_enter( "AllocCmap1" );

    if ( xwd->rw_cmap )
    {
        if ( pls->verbose )
            fprintf( stderr, "Attempting to allocate r/w colors in cmap1.\n" );

        // If using the default color map, must severely limit number of colors
        // otherwise TK won't have enough.

        npixels = MAX( 2, MIN( RWMAP_CMAP1_COLORS, pls->ncol1 ) );
        for (;; )
        {
            if ( XAllocColorCells( xwd->display, xwd->map, False,
                plane_masks, 0, pixels, (unsigned int) npixels ) )
                break;
            npixels--;
            if ( npixels == 0 )
                break;
        }

        if ( npixels < 2 )
        {
            xwd->ncol1 = -1;
            fprintf( stderr, "Warning: unable to allocate sufficient colors in cmap1.\n" );
            return;
        }

        xwd->ncol1 = npixels;
        if ( pls->verbose )
            fprintf( stderr, "AllocCmap1 (xwin.c): Allocated %d colors in cmap1.\n", npixels );

        // Allocate space if it hasn't been done yet
        if ( !xwd->cmap1 )
        {
            xwd->ncol1_alloc = xwd->ncol1;
            xwd->cmap1       = (XColor *) calloc( (size_t) ( xwd->ncol1 ), sizeof ( XColor ) );
            if ( !xwd->cmap1 )
                plexit( "couldn't allocate space for cmap1 colors" );
        }

        // Don't assign pixels sequentially, to avoid strange problems with xor
        // GC's.  Skipping by 2 seems to do the job best.

        for ( j = i = 0; i < xwd->ncol1; i++ )
        {
            while ( pixels[j] == 0 )
                j++;

            xwd->cmap1[i].pixel = pixels[j];
            pixels[j]           = 0;

            j += 2;
            if ( j >= xwd->ncol1 )
                j = 0;
        }

        StoreCmap1( pls );
    }
    else
    {
        int     r, ncolors;
        PLColor cmap1color;
        XColor  xcol;

        if ( pls->verbose )
            fprintf( stderr, "Attempting to allocate r/o colors in cmap1.\n" );

        switch ( xwd->visual->class )
        {
        case TrueColor:
            ncolors = TC_CMAP1_COLORS;
            break;
        default:
            ncolors = ROMAP_CMAP1_COLORS;
        }

        // Allocate space if it hasn't been done yet
        if ( !xwd->cmap1 )
        {
            xwd->ncol1_alloc = ncolors;
            xwd->cmap1       = (XColor *) calloc( (size_t) ncolors, sizeof ( XColor ) );
            if ( !xwd->cmap1 )
                plexit( "couldn't allocate space for cmap1 colors" );
        }

        for ( i = 0; i < ncolors; i++ )
        {
            plcol_interp( pls, &cmap1color, i, ncolors );
            PLColor_to_XColor( &cmap1color, &xcol );

            r = XAllocColor( xwd->display, xwd->map, &xcol );
            if ( pls->verbose )
                fprintf( stderr, "i=%d, r=%d, pixel=%d\n", i, r, (int) xcol.pixel );
            if ( r )
                xwd->cmap1[i] = xcol;
            else
                break;
        }
        if ( i < ncolors )
        {
            xwd->ncol1 = -1;
            fprintf( stderr,
                "Warning: unable to allocate sufficient colors in cmap1\n" );
            return;
        }
        else
        {
            xwd->ncol1 = ncolors;
            if ( pls->verbose )
                fprintf( stderr, "AllocCmap1 (xwin.c): Allocated %d colors in cmap1\n", ncolors );
        }
    }
}

//--------------------------------------------------------------------------
// StoreCmap0()
//
// Stores cmap 0 entries in X-server colormap.
//--------------------------------------------------------------------------

static void
StoreCmap0( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    int       i;

    if ( !xwd->color )
        return;

    for ( i = 1; i < xwd->ncol0; i++ )
    {
        PLColor_to_XColor( &pls->cmap0[i], &xwd->cmap0[i] );
        if ( xwd->rw_cmap )
            XStoreColor( xwd->display, xwd->map, &xwd->cmap0[i] );
        else
            XAllocColor( xwd->display, xwd->map, &xwd->cmap0[i] );
    }
}

//--------------------------------------------------------------------------
// StoreCmap1()
//
// Stores cmap 1 entries in X-server colormap.
//--------------------------------------------------------------------------

static void
StoreCmap1( PLStream *pls )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    PLColor   cmap1color;
    int       i;

    if ( !xwd->color )
        return;

    for ( i = 0; i < xwd->ncol1; i++ )
    {
        plcol_interp( pls, &cmap1color, i, xwd->ncol1 );
        PLColor_to_XColor( &cmap1color, &xwd->cmap1[i] );
        if ( xwd->rw_cmap )
            XStoreColor( xwd->display, xwd->map, &xwd->cmap1[i] );
        else
            XAllocColor( xwd->display, xwd->map, &xwd->cmap1[i] );
    }
}

//--------------------------------------------------------------------------
// PLColor_to_XColor()
//
// Copies the supplied PLColor to an XColor, padding with bits as necessary
// (a PLColor uses 8 bits for color storage, while an XColor uses 16 bits).
// The argument types follow the same order as in the function name.
//--------------------------------------------------------------------------

#define ToXColor( a )     ( ( ( 0xFF & ( a ) ) << 8 ) | ( a ) )
#define ToPLColor( a )    ( ( (U_LONG) a ) >> 8 )

static void
PLColor_to_XColor( PLColor *plcolor, XColor *xcolor )
{
    xcolor->red   = (short unsigned) ToXColor( plcolor->r );
    xcolor->green = (short unsigned) ToXColor( plcolor->g );
    xcolor->blue  = (short unsigned) ToXColor( plcolor->b );
    xcolor->flags = DoRed | DoGreen | DoBlue;
}

//--------------------------------------------------------------------------
// PLColor_from_XColor()
//
// Copies the supplied XColor to a PLColor, stripping off bits as
// necessary.  See the previous routine for more info.
//--------------------------------------------------------------------------

static void
PLColor_from_XColor( PLColor *plcolor, XColor *xcolor )
{
    plcolor->r = (unsigned char) ToPLColor( xcolor->red );
    plcolor->g = (unsigned char) ToPLColor( xcolor->green );
    plcolor->b = (unsigned char) ToPLColor( xcolor->blue );
}

//--------------------------------------------------------------------------
// AreWeGrayscale(Display *display)
//
// Determines if we're using a monochrome or grayscale device.
// gmf 11-8-91; Courtesy of Paul Martz of Evans and Sutherland.
// Altered Andrew Ross 26-01-2004 to fix memory leak.
//--------------------------------------------------------------------------

static int
AreWeGrayscale( Display *display )
{
#if defined ( __cplusplus ) || defined ( c_plusplus )
#define THING    c_class
#else
#define THING    class
#endif

    XVisualInfo *visuals;
    int         nitems, i, igray;

    // get a list of info on the visuals available
    visuals = XGetVisualInfo( display, 0, NULL, &nitems );

    igray = 1;
    // check the list looking for non-monochrome visual classes
    for ( i = 0; i < nitems; i++ )
        if ( ( visuals[i].THING != GrayScale ) &&
             ( visuals[i].THING != StaticGray ) )
        {
            igray = 0;
            break;
        }

    XFree( visuals );
    // if igray = 1 only StaticGray and GrayScale classes available
    return igray;
}

#ifdef DUMMY
//--------------------------------------------------------------------------
// SaveColormap()  **** DUMMY, NOT USED ANYMORE ***
//
// Saves RGB components of given colormap.
// Used in an ugly hack to get past some X11R5 and TK limitations.
// This isn't guaranteed to work under all circumstances, but hopefully
// in the future there will be a nicer way to accomplish the same thing.
//
// Note: I tried using XCopyColormapAndFree to do the same thing, but under
// HPUX 9.01/VUE/X11R5 at least it doesn't preserve the previous read-only
// color cell allocations made by Tk.  Is this a bug?  Have to look at the
// source to find out.
//--------------------------------------------------------------------------

static void
SaveColormap( Display *display, Colormap colormap )
{
    int i;

    if ( !plplot_ccmap )
        return;

    sxwm_colors_set = 1;
    for ( i = 0; i < RWMAP_MAX_COLORS; i++ )
    {
        sxwm_colors[i].pixel = i;
    }
    XQueryColors( display, colormap, sxwm_colors, RWMAP_MAX_COLORS );
//
//  printf("\nAt startup, default colors are: \n\n");
//  for (i = 0; i < RWMAP_MAX_COLORS; i++) {
//      printf(" i: %d,  pixel: %d,  r: %d,  g: %d,  b: %d\n",
//             i, sxwm_colors[i].pixel,
//             sxwm_colors[i].red, sxwm_colors[i].green, sxwm_colors[i].blue);
//  }
//
}
#endif

//--------------------------------------------------------------------------
// GetImageErrorHandler()
//
// Error handler used in XGetImage() to catch errors when pixmap or window
// are not completely viewable.
//--------------------------------------------------------------------------

static int
GetImageErrorHandler( Display *display, XErrorEvent *error )
{
    if ( error->error_code != BadMatch )
    {
        char buffer[256];
        XGetErrorText( display, error->error_code, buffer, 256 );
        fprintf( stderr, "xwin: Error in XGetImage: %s.\n", buffer );
    }
    return 1;
}

//--------------------------------------------------------------------------
// DrawImage()
//
// Fill polygon described in points pls->dev_x[] and pls->dev_y[].
// Only solid color fill supported.
//--------------------------------------------------------------------------

static void
DrawImage( PLStream *pls )
{
    XwDev     *dev  = (XwDev *) pls->dev;
    XwDisplay *xwd  = (XwDisplay *) dev->xwd;
    XImage    *ximg = NULL;
    XColor    curcolor;
    PLINT     xmin, xmax, ymin, ymax, icol1;

    int       ( *oldErrorHandler )( Display *, XErrorEvent * );

    float     mlr, mtb;
    float     blt, brt, brb, blb;
    float     left, right;
    int       kx, ky;
    int       nx, ny, ix, iy;
    int       i, corners[4], r[4] = { 0, 0, 0, 0 };

    struct
    {
        float x, y;
    } Ppts[4];

    CheckForEvents( pls );

    xmin = (PLINT) ( dev->xscale * pls->imclxmin );
    xmax = (PLINT) ( dev->xscale * pls->imclxmax );
    ymin = (PLINT) ( dev->yscale * pls->imclymin );
    ymax = (PLINT) ( dev->yscale * pls->imclymax );

    nx = pls->dev_nptsX;
    ny = pls->dev_nptsY;

// XGetImage() call fails if either the pixmap or window is not fully viewable!
    oldErrorHandler = XSetErrorHandler( GetImageErrorHandler );

    XFlush( xwd->display );
    if ( dev->write_to_pixmap )
        ximg = XGetImage( xwd->display, dev->pixmap, 0, 0, dev->width, dev->height,
            AllPlanes, ZPixmap );

    if ( dev->write_to_window )
        ximg = XGetImage( xwd->display, dev->window, 0, 0, dev->width, dev->height,
            AllPlanes, ZPixmap );

    XSetErrorHandler( oldErrorHandler );

    if ( ximg == NULL )
    {
        plabort( "Can't get image, the window must be partly off-screen, move it to fit screen" );
        return;
    }

    if ( xwd->ncol1 == 0 )
        AllocCmap1( pls );
    if ( xwd->ncol1 < 2 )
        return;

// translate array for rotation
    switch ( (int) ( pls->diorot - 4. * floor( pls->diorot / 4. ) ) )
    {
    case 0:
        r[0] = 0; r[1] = 1; r[2] = 2; r[3] = 3; break;
    case 1:
        r[0] = 1; r[1] = 2; r[2] = 3; r[3] = 0; break;
    case 2:
        r[0] = 2; r[1] = 3; r[2] = 0; r[3] = 1; break;
    case 3:
        r[0] = 3; r[1] = 0; r[2] = 1; r[3] = 2;
    }

    // after rotation and coordinate translation, each fill
    // lozangue will have coordinates (Ppts), slopes (m...)
    // and y intercepts (b...):
    //
    //        Ppts[3]
    //          **
    // mlr,blt *  * mtb,brt
    //        *    *
    //Ppts[0]<      > Ppts[2]
    //        *    *
    // mtb,blt *  * mlr,brb
    //          **
    //        Ppts[1]
    //

// slope of left/right and top/bottom edges
    mlr = (float) ( ( dev->yscale * ( pls->dev_iy[1] - pls->dev_iy[0] ) ) /
                    ( dev->xscale * ( pls->dev_ix[1] - pls->dev_ix[0] ) ) );

    mtb = (float) ( ( dev->yscale * ( pls->dev_iy[ny] - pls->dev_iy[0] ) ) /
                    ( dev->xscale * ( pls->dev_ix[ny] - pls->dev_ix[0] ) ) );

    for ( ix = 0; ix < nx - 1; ix++ )
    {
        for ( iy = 0; iy < ny - 1; iy++ )
        {
            corners[0] = ix * ny + iy;             // [ix][iy]
            corners[1] = ( ix + 1 ) * ny + iy;     // [ix+1][iy]
            corners[2] = ( ix + 1 ) * ny + iy + 1; // [ix+1][iy+1]
            corners[3] = ix * ny + iy + 1;         // [ix][iy+1]

            for ( i = 0; i < 4; i++ )
            {
                Ppts[i].x = (float) ( dev->xscale * ( pls->dev_ix[corners[r[i]]] ) );
                Ppts[i].y = (float) ( dev->yscale * ( pls->dev_iy[corners[r[i]]] ) );
            }

            // if any corner is inside the draw area
            if ( Ppts[0].x >= xmin || Ppts[2].x <= xmax ||
                 Ppts[1].y >= ymin || Ppts[3].y <= ymax )
            {
                Ppts[0].x = MAX( Ppts[0].x, (float) xmin );
                Ppts[2].x = MIN( Ppts[2].x, (float) xmax );
                Ppts[1].y = MAX( Ppts[1].y, (float) ymin );
                Ppts[3].y = MIN( Ppts[3].y, (float) ymax );

                // the Z array has size (nx-1)*(ny-1)
                icol1 = pls->dev_z[ix * ( ny - 1 ) + iy];

                // only plot points within zmin/zmax range
                if ( icol1 < pls->dev_zmin || icol1 > pls->dev_zmax )
                    continue;

                icol1 = (PLINT) ( (float) icol1 / (float) USHRT_MAX * (float) ( xwd->ncol1 - 1 ) );
                if ( xwd->color )
                    curcolor = xwd->cmap1[icol1];
                else
                    curcolor = xwd->fgcolor;

                // Fill square between current and next points.

                // If the fill area is a single dot, accelerate the fill.
                if ( ( fabs( Ppts[2].x - Ppts[0].x ) == 1 ) &&
                     ( fabs( Ppts[3].y - Ppts[1].y ) == 1 ) )
                {
                    XPutPixel( ximg, (int) Ppts[0].x, (int) dev->height - 1 - (int) Ppts[0].y, (unsigned long) curcolor.pixel );

                    // integer rotate, accelerate
                }
                else if ( pls->diorot == floor( pls->diorot ) )
                {
                    for ( ky = (int) Ppts[1].y; ky < (int) Ppts[3].y; ky++ )
                        for ( kx = (int) Ppts[0].x; kx < (int) Ppts[2].x; kx++ )
                            XPutPixel( ximg, kx, (int) dev->height - 1 - ky, (unsigned int) curcolor.pixel );

                    // lozangue, scanline fill it
                }
                else
                {
                    // y interception point of left/right top/bottom edges
                    blt = Ppts[0].y - mlr * Ppts[0].x;
                    brb = Ppts[2].y - mlr * Ppts[2].x;

                    brt = Ppts[2].y - mtb * Ppts[2].x;
                    blb = Ppts[0].y - mtb * Ppts[0].x;

                    for ( ky = (int) Ppts[1].y; ky < (int) Ppts[3].y; ky++ )
                    {
                        left  = MAX( ( ( (float) ky - blt ) / mlr ), ( ( (float) ky - blb ) / mtb ) );
                        right = MIN( ( ( (float) ky - brt ) / mtb ), ( ( (float) ky - brb ) / mlr ) );
                        for ( kx = (int) Ppts[0].x; kx < (int) Ppts[2].x; kx++ )
                        {
                            if ( kx >= rint( left ) && kx <= rint( right ) )
                            {
                                XPutPixel( ximg, kx, (int) dev->height - 1 - ky, (unsigned int) curcolor.pixel );
                            }
                        }
                    }
                }
            }
        }
    }

    if ( dev->write_to_pixmap )
        XPutImage( xwd->display, dev->pixmap, dev->gc, ximg, 0, 0, 0, 0, dev->width, dev->height );

    if ( dev->write_to_window )
        XPutImage( xwd->display, dev->window, dev->gc, ximg, 0, 0,
            0, 0, dev->width, dev->height );

    XDestroyImage( ximg );
}

static void
imageops( PLStream *pls, PLINT *ptr )
{
    XwDev     *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

// TODO: store/revert to/from previous state

    switch ( *ptr )
    {
    case ZEROW2D:
        dev->write_to_window = 0;
        break;

    case ONEW2D:
        dev->write_to_window = 1;
        break;

    case ZEROW2B:
        dev->write_to_pixmap = 0;
        break;

    case ONEW2B:
        XFlush( xwd->display );
        dev->write_to_pixmap = 1;
        break;
    }
}

#else
int
pldummy_xwin()
{
    return 0;
}

#endif                          // PLD_xwin
