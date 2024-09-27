//      Contains declarations for core plplot data structures.  This file
//      should be included only by plcore.c.
//
//  Copyright (C) 2004  Andrew Roach
//  Copyright (C) 2005  Thomas J. Duck
//
//  This file is part of PLplot.
//
//  PLplot is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License as published
//  by the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  PLplot is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public License
//  along with PLplot; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#ifndef __PLCORE_H__
#define __PLCORE_H__

#include "plplotP.h"
#include "drivers.h"
#include "plDevs.h"
#include "disptab.h"

#ifdef ENABLE_DYNDRIVERS
  #ifndef LTDL_WIN32
    #include <ltdl.h>
  #else
    #include "ltdl_win32.h"
  #endif
typedef lt_ptr ( *PLDispatchInit )( PLDispatchTable *pdt );
#else
typedef void ( *PLDispatchInit )( PLDispatchTable *pdt );
#endif

#ifdef HAVE_LIBUNICODE
#include <unicode.h>
#endif


// Static function prototypes

static const char     *utf8_to_ucs4( const char *ptr, PLUNICODE *unichar );
static void     grline( short *, short *, PLINT );
static void     grpolyline( short *, short *, PLINT );
static void     grfill( short *, short *, PLINT );
static void     grgradient( short *, short *, PLINT );
static void     plSelectDev( void );
static void     pldi_ini( void );
static void     calc_diplt( void );
static void     calc_didev( void );
static void     calc_diori( void );
static void     calc_dimap( void );
static void     plgdevlst( const char **, const char **, int *, int );

static void     plInitDispatchTable( void );

static void     plLoadDriver( void );

// Static variables

static PLINT xscl[PL_MAXPOLY], yscl[PL_MAXPOLY];

static PLINT initfont = 1;      // initial font: extended by default

static PLINT lib_initialized = 0;

//--------------------------------------------------------------------------
// Allocate a PLStream data structure (defined in plstrm.h).
//
// This struct contains a copy of every variable that is stream dependent.
// Only the first [index=0] stream is statically allocated; the rest
// are dynamically allocated when you switch streams (yes, it is legal
// to only initialize the first element of the array of pointers).
//--------------------------------------------------------------------------

static PLStream pls0;                             // preallocated stream
static PLINT    ipls;                             // current stream number

static PLStream *pls[PL_NSTREAMS] = { &pls0 };    // Array of stream pointers

// Current stream pointer.  Global, for easier access to state info

PLDLLIMPEXP_DATA( PLStream ) * plsc = &pls0;

// Only now can we include this

#include "pldebug.h"

//--------------------------------------------------------------------------
// Initialize dispatch table.
//
// Each device is selected by the appropriate define, passed in from the
// makefile.  When installing plplot you may wish to exclude devices not
// present on your system in order to reduce screen clutter.
//
// If you hit a <CR> in response to the plinit() prompt, you get the FIRST
// one active below, so arrange them accordingly for your system (i.e. all
// the system-specific ones should go first, since they won't appear on
// most systems.)
//--------------------------------------------------------------------------

static PLDispatchTable **dispatch_table = 0;
static int             npldrivers       = 0;

static PLDispatchInit  static_device_initializers[] = {
// Same order (by source filename and by device) as in drivers.h.
// ps and psttf are special cases where two devices are handled with
// one PLD macro.
// xwin is special case where the macro name is PLD_xwin but the
// function name in drivers.h is plD_dispatch_init_xw

#if defined ( PLD_aqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_aqt,
#endif
#if defined ( PLD_epscairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_epscairo,
#endif
#if defined ( PLD_extcairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_extcairo,
#endif
#if defined ( PLD_memcairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_memcairo,
#endif
#if defined ( PLD_pdfcairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pdfcairo,
#endif
#if defined ( PLD_pngcairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pngcairo,
#endif
#if defined ( PLD_pscairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pscairo,
#endif
#if defined ( PLD_svgcairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_svgcairo,
#endif
#if defined ( PLD_wincairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_wincairo,
#endif
#if defined ( PLD_xcairo ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_xcairo,
#endif
#if defined ( PLD_cgm ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_cgm,
#endif
#if defined ( PLD_gif ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_gif,
#endif
#if defined ( PLD_jpeg ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_jpeg,
#endif
#if defined ( PLD_png ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_png,
#endif
#if defined ( PLD_mem ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_mem,
#endif
#if defined ( PLD_ntk ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_ntk,
#endif
#if defined ( PLD_null ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_null,
#endif
#if defined ( PLD_pdf ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pdf,
#endif
#if defined ( PLD_plm ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_plm,
#endif
#if defined ( PLD_ps ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_ps,
#endif
#if defined ( PLD_psc ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_psc,
#endif
#if defined ( PLD_pstex ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pstex,
#endif
#if defined ( PLD_psttf ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_psttf,
#endif
#if defined ( PLD_psttfc ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_psttfc,
#endif
#if defined ( PLD_bmpqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_bmpqt,
#endif
#if defined ( PLD_epsqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_epsqt,
#endif
#if defined ( PLD_extqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_extqt,
#endif
#if defined ( PLD_jpgqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_jpgqt,
#endif
#if defined ( PLD_memqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_memqt,
#endif
#if defined ( PLD_pdfqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pdfqt,
#endif
#if defined ( PLD_pngqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_pngqt,
#endif
#if defined ( PLD_ppmqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_ppmqt,
#endif
#if defined ( PLD_qtwidget ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_qtwidget,
#endif
#if defined ( PLD_svgqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_svgqt,
#endif
#if defined ( PLD_tiffqt ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_tiffqt,
#endif
#if defined ( PLD_svg ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_svg,
#endif
#if defined ( PLD_tk ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_tk,
#endif
#if defined ( PLD_tkwin ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_tkwin,
#endif
#if defined ( PLD_wingcc ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_wingcc,
#endif
#if defined ( PLD_wxpng ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_wxpng,
#endif
#if defined ( PLD_wxwidgets ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_wxwidgets,
#endif
#if defined ( PLD_xfig ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_xfig,
#endif
#if defined ( PLD_xwin ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_xw,
#endif
#if defined ( PLD_wingdi ) && !defined ( ENABLE_DYNDRIVERS )
    plD_dispatch_init_wingdi,
#endif
    NULL
};

static int             nplstaticdevices = ( sizeof ( static_device_initializers ) /
                                            sizeof ( PLDispatchInit ) ) - 1;
static int             npldynamicdevices = 0;

//--------------------------------------------------------------------------
// Stuff to support the loadable device drivers.
//--------------------------------------------------------------------------

#ifdef ENABLE_DYNDRIVERS
typedef struct
{
    char *devnam;
    char *description;
    char *drvnam;
    char *tag;
    int  drvidx;
} PLLoadableDevice;

typedef struct
{
    char        *drvnam;
    lt_dlhandle dlhand;
} PLLoadableDriver;

static PLLoadableDevice *loadable_device_list;
static PLLoadableDriver *loadable_driver_list;

static int nloadabledrivers = 0;

#endif

#endif  // __PLCORE_H__
