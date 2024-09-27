//  Internal (private) macros and prototypes for the PLplot package.  This
//  header file must be included before all others, including system header
//  files.  This file is typically needed when including driver specific
//  header files (e.g. pltkd.h).
//
//  Copyright (C) 1993, 1994, 1995  by
//  Maurice J. LeBrun, Geoff Furnish, Tony Richardson.
//
//  Copyright (C) 2004  Rafael Laboissiere
//  Copyright (C) 2004  Joao Cardoso
//  Copyright (C) 2004  Andrew Roach
//  Copyright (C) 2006  Andrew Ross
//  Copyright (C) 2006  Hazen Babcock
//  Copyright (C) 2001-2015  Alan W. Irwin
//
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
//

#ifndef __PLPLOTP_H__
#define __PLPLOTP_H__

//--------------------------------------------------------------------------
// Select environment.  Must be done before anything else.
//
// Basically we want the widest range of system services that are available.
// Fortunately on many systems, that is the default.  To get "everything",
// one of the following must be defined, as appropriate:
//
// _GNU_SOURCE     on Linux (default)
// _OSF_SOURCE     on OSF1 (default)
// _HPUX_SOURCE    on HP (not default)
// _ALL_SOURCE     on AIX (no idea)
//
// To see where these are set, do the following:
//
//    cd /usr/include; grep SOURCE *.h | fgrep 'define '
//
// and the file containing lots of these is the one you want (features.h on
// Linux, standards.h on OSF1, etc).  Follow the logic to see what needs to be
// defined to get "everything", i.e. POSIX.*, XOPEN, etc.
//
// Note that for specific functionality, we test using autoconf.  Still it's
// best to stick to ANSI C, POSIX.1, and POSIX.2, in that order, for maximum
// portability.
//--------------------------------------------------------------------------

// HPUX - if this is no longer needed, please remove it
#ifdef _HPUX
#define _HPUX_SOURCE
#endif

// A/IX - if this is no longer needed, please remove it
#ifdef _AIX
#define _ALL_SOURCE
#endif

// Add others here as needed.

//--------------------------------------------------------------------------
//	Configuration settings
//
// Some of the macros set during configuration are described here.
//
// If HAVE_TERMIOS_H is set, we employ POSIX.1 tty terminal I/O.
//
// If STDC_HEADERS is defined, the system's libc is ANSI-compliant.
// ANSI libc calls are used for: (a) setting up handlers to be called
// before program exit (via the "atexit" call), and (b) for seek
// operations.  Again, the code is usable without these.  An ANSI libc
// should be available, given the requirement of an ANSI compiler.  Some
// reasons why not: (a) the vendor didn't supply a complete ANSI
// environment, or (b) the ANSI libc calls are buggy, or (c) you ported
// gcc to your system but not glibc (for whatever reason).  Note: without
// an ANSI C lib, if you ^C out of a program using one of the PLplot tek
// drivers, your terminal may be left in a strange state.
//--------------------------------------------------------------------------

#include "plConfig.h"
#ifdef caddr_t
#undef caddr_t
#ifndef __USE_BSD
typedef char * caddr_t;
#endif
#endif

// System headers

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#ifdef PL_HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <locale.h>

// (AM) Define M_PI if the platform does not include it
// (MSVC for instance)
#if !defined ( M_PI )
#define M_PI    3.14159265358979323846
#endif

//
// Macros for file positioning.  I tried switching to f[sg]etpos() because I
// like the semantics better, but ran into the problem that fpos_t is not
// always a base type (it may be a struct).  This is a problem because the
// metafile driver needs to write relative offsets into the file itself.  So
// instead we use f{seek,tell} at a low level but keep the f[sg]etpos
// semantics using these macros.
//

#ifdef STDC_FPOS_T
#undef STDC_FPOS_T
#endif

#ifdef STDC_FPOS_T
#define FPOS_T    fpos_t
#define pl_fsetpos( a, b )    fsetpos( a, b )
#define pl_fgetpos( a, b )    fgetpos( a, b )

#else
#define FPOS_T    long
#define pl_fsetpos( a, b )    fseek( a, *b, 0 )
#define pl_fgetpos( a, b )    ( -1L == ( *b = ftell( a ) ) )
#endif

#include "pldll.h"

// Include all externally-visible definitions and prototypes
// plplot.h also includes some handy system header files

#include "plplot.h"

// plstream definition

#include "plstrm.h"

// If not including this file from inside of plcore.h, declare plsc

#ifndef __PLCORE_H__
#ifdef __cplusplus
extern "C" {
#endif
// extern PLStream PLDLLIMPORT *plsc;
extern PLDLLIMPEXP_DATA( PLStream * ) plsc;
#ifdef __cplusplus
}
#endif
#include "pldebug.h"
#endif

//--------------------------------------------------------------------------
//                       Utility macros
//--------------------------------------------------------------------------

#ifndef TRUE
#define TRUE     1
#define FALSE    0
#endif

// Used to help ensure everything malloc'ed gets freed

#define free_mem( a ) \
    if ( a != NULL ) { free( (void *) a ); a = NULL; }

// Allows multi-argument setup calls to not affect selected arguments

#define plsetvar( a, b ) \
    if ( b != PL_NOTSET ) a = b;

// Lots of cool math macros

#ifndef MAX
#define MAX( a, b )                      ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef MIN
#define MIN( a, b )                      ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef ABS
#define ABS( a )                         ( ( a ) < 0 ? -( a ) : ( a ) )
#endif
#ifndef ROUND
#define ROUND( a )                       (PLINT) ( ( a ) < 0. ? ( ( a ) - .5 ) : ( ( a ) + .5 ) )
#endif
#ifndef BETW
#define BETW( ix, ia, ib )               ( ( ( ix ) <= ( ia ) && ( ix ) >= ( ib ) ) || ( ( ix ) >= ( ia ) && ( ix ) <= ( ib ) ) )
#endif
#ifndef SSQR
#define SSQR( a, b )                     sqrt( ( a ) * ( a ) + ( b ) * ( b ) )
#endif
#ifndef SIGN
#define SIGN( a )                        ( ( a ) < 0 ? -1 : 1 )
#endif
#ifndef TRANSFORM
#define TRANSFORM( x, y, xnew, ynew )    if ( plsc->coordinate_transform ) { plsc->coordinate_transform( ( x ), ( y ), ( xnew ), ( ynew ), plsc->coordinate_transform_data ); } else { *xnew = x; *ynew = y; }
#endif

// A coordinate value that should never occur

#define PL_UNDEFINED    -9999999

// Declarations for save string functions

#ifdef PL_HAVE_SNPRINTF
// In case only _snprintf is declared (as for Visual C++ and
// Borland compiler toolset) we redefine the function names
  #ifdef _PL_HAVE_SNPRINTF
    #define snprintf    _snprintf
    #define snscanf     _snscanf
  #endif // _PL_HAVE_SNPRINTF
#else // !PL_HAVE_SNPRINTF
// declare dummy functions which just call the unsafe
// functions ignoring the size of the string
int plsnprintf( char *buffer, int n, PLCHAR_VECTOR format, ... );
int plsnscanf( PLCHAR_VECTOR buffer, int n, PLCHAR_VECTOR format, ... );
  #define snprintf    plsnprintf
  #define snscanf     plsnscanf
#endif // PL_HAVE_SNPRINTF

//--------------------------------------------------------------------------
// Add in missing isnan / isinf functions on some platforms
//--------------------------------------------------------------------------

#if defined ( PL__HAVE_ISNAN )
#  define isnan    _isnan
#  if defined ( _MSC_VER )
#    include <float.h>
#  endif
#endif
#if defined ( PL__HAVE_ISINF )
#  define isinf       _isinf
#endif
#if defined ( PL_HAVE_FINITE )
#  define isfinite    finite
#endif
#if defined ( PL__HAVE_FINITE )
#  define isfinite    _finite
#endif

// Note these replacements follow the old BSD convention and not
// C99. In particular isinf does not distinguish +/- inf.
#if !defined ( PL_HAVE_ISNAN )
#  define isnan( x )       ( ( x ) != ( x ) )
#endif
#if !defined ( PL_HAVE_ISINF )
#  define isinf( x )       ( !isnan( x ) && isnan( x - x ) )
#endif
#if !defined ( PL_HAVE_ISFINITE )
#  define isfinite( x )    ( !isnan( x - x ) )
#endif

// Check if C99 HUGE_VAL macro is available - if not then
// define a replacement
#ifndef HUGE_VAL
#define HUGE_VAL    ( 1.0 / 0.0 )
#endif

//--------------------------------------------------------------------------
//                       PLPLOT control macros
//--------------------------------------------------------------------------

// Some constants

#define PL_MAXPOLY       256    // Max segments in polyline or polygon
#define PL_NSTREAMS      100    // Max number of concurrent streams.
#define PL_RGB_COLOR     -1     // A hack

#define TEXT_MODE        0
#define GRAPHICS_MODE    1
#ifndef PI
#define PI               3.1415926535897932384
#endif

// These define the virtual coordinate system used by the metafile driver.
// Others are free to use it, or some variation, or define their own.

// Note desktop monitors of reasonable quality typically have 0.25 mm spacing
// between dots which corresponds to 4.0 dots per mm.  The parameters here
// roughly correspond to a 14" monitor at 1024x768 resolution, which should
// work fine at other sizes/resolutions.  The number of virtual dots per mm is
// scaled by a factor of 32, with pixels scaled accordingly.  The customary
// x/y ratio of 4:3 is used.
//

#define PIXELS_X    32768                // Number of virtual pixels in x
#define PIXELS_Y    24576                // Number of virtual pixels in x
#define DPMM        4.                   // dots per mm
#define VDPMM       ( DPMM * 32 )        // virtual dots per mm
#define LPAGE_X     ( PIXELS_X / VDPMM ) // virtual page length in x in mm (256)
#define LPAGE_Y     ( PIXELS_Y / VDPMM ) // virtual page length in y in mm (192)

// Constants that help to define ideal plspage arguments for devices

#define PLPLOT_MM_PER_INCH        25.4
#define PLPLOT_POINTS_PER_INCH    72.
#define PLPLOT_WIDTH_A4_MM        297.
#define PLPLOT_HEIGHT_A4_MM       210.
#define PLPLOT_WIDTH_A5_MM        210.
#define PLPLOT_HEIGHT_A5_MM       148.

// Devices with pixel units for sizes.

// Adopt this default value as reasonable approximation for typical
// LCD monitors, but note that the user can replace this default by
// setting plsc->[xy]dpi values (e.g., by using the -dpi command-line
// option or by calling plspage with non-zero first two arguments).
#define PLPLOT_DEFAULT_PIXELS_PER_INCH    90.
// These default pixel dimensions correspond to A5 size if actual
// pixels per inch is PLPLOT_DEFAULT_PIXELS_PER_INCH
#define PLPLOT_DEFAULT_WIDTH_PIXELS       ( (int) ( PLPLOT_WIDTH_A5_MM * PLPLOT_DEFAULT_PIXELS_PER_INCH / PLPLOT_MM_PER_INCH ) )
#define PLPLOT_DEFAULT_HEIGHT_PIXELS      ( (int) ( PLPLOT_HEIGHT_A5_MM * PLPLOT_DEFAULT_PIXELS_PER_INCH / PLPLOT_MM_PER_INCH ) )

// Devices with real world units for sizes.

// N.B. these devices normally use fixed values of units per inch,
// i.e., they ignore any attempt by users to set plsc->[xy]dpi values
// (e.g., by using the -dpi command-line option or by calling plspage
// with non-zero first two arguments).

// 1. Devices with mm units for sizes.
// These default mm dimensions correspond to A4 size.
#define PLPLOT_DEFAULT_WIDTH_MM     ( (int) ( PLPLOT_WIDTH_A4_MM )
#define PLPLOT_DEFAULT_HEIGHT_MM    ( (int) ( PLPLOT_HEIGHT_A4_MM )

// 2. Devices with points (= 1/72 inch) units for sizes.
// These default points dimensions correspond to A4 size.
#define PLPLOT_DEFAULT_WIDTH_POINTS     ( (int) ( PLPLOT_WIDTH_A4_MM * PLPLOT_POINTS_PER_INCH / PLPLOT_MM_PER_INCH ) )
#define PLPLOT_DEFAULT_HEIGHT_POINTS    ( (int) ( PLPLOT_HEIGHT_A4_MM * PLPLOT_POINTS_PER_INCH / PLPLOT_MM_PER_INCH ) )

// This defines the first argument of the plRotPhy invocation that is made
// in a number of device drivers (e.g., found in ps.c
// and pstex.c) to rotate them "permanently" from portrait mode to non-
// portrait mode.  ORIENTATION of 1 corresponds to seascape mode (90 deg
// clockwise rotation from portrait).  This is the traditional value
// effectively used in all older versions of PLplot. ORIENTATION of 3
// corresponds to landscape mode (90 deg *counter*-clockwise rotation from
// portrait) which is the new default non-portrait orientation.

#define ORIENTATION    3

// Switches for state function call.

#define PLSTATE_WIDTH     1            // pen width
#define PLSTATE_COLOR0    2            // change to color in cmap 0
#define PLSTATE_COLOR1    3            // change to color in cmap 1
#define PLSTATE_FILL      4            // set area fill to a standard pattern
#define PLSTATE_CMAP0     5            // change to cmap 0
#define PLSTATE_CMAP1     6            // change to cmap 1
#define PLSTATE_CHR       7            // change character height
#define PLSTATE_SYM       8            // change symbol size
#define PLSTATE_EOFILL    9            // change to even odd fill rule

// Enum for flagging the render status of PLplot
enum { AT_BOP, DRAWING, AT_EOP };


// Bit switches used in the driver interface

#define PLDI_MAP    0x01
#define PLDI_ORI    0x02
#define PLDI_PLT    0x04
#define PLDI_DEV    0x08

// Default size for family files, in KB.

#ifndef PL_FILESIZE_KB
#define PL_FILESIZE_KB    1000
#endif

// Font file names.

#define PLPLOT5_FONTS

#ifdef PLPLOT5_FONTS
#define PL_XFONT    "plxtnd5.fnt"
#define PL_SFONT    "plstnd5.fnt"
#else
#define PL_XFONT    "plxtnd4.fnt"
#define PL_SFONT    "plstnd4.fnt"
#endif

//--------------------------------------------------------------------------
// The following environment variables are defined:
//
//	PLPLOT_BIN      # where to find executables
//	PLPLOT_LIB      # where to find library files (fonts, maps, etc)
//	PLPLOT_TCL      # where to find tcl scripts
//
//	PLPLOT_HOME     # basename of plplot hierarchy
//
// search order:
//	1)	the most specific possible locators, one of
//			$(PLPLOT_BIN)
//			$(PLPLOT_LIB)
//			$(PLPLOT_TCL)
//		as appropriate
//
//	2)	the current directory
//
//	3)	one of  $(PLPLOT_HOME)/bin
//			$(PLPLOT_HOME)/lib
//			$(PLPLOT_HOME)/tcl
//		as appropriate
//
//	4)	as appropriate, the compile-time (Makefile)
//		BIN_DIR, LIB_DIR, TCL_DIR
//
//  8 Jun 1994  mj olesen (olesen@weber.me.queensu.ca)
//
// Other notes:
//
// In addition to the directories above, the following are also used:
//
// Lib file search path: PLLIBDEV (see plctrl.c).  This is checked last,
// and is a system-dependent hardwired location.
//
// Tcl search path: $HOME/tcl is searched before the install location,
// TCL_DIR.
//--------------------------------------------------------------------------

#define PLPLOT_BIN_ENV     "PLPLOT_BIN"
#define PLPLOT_LIB_ENV     "PLPLOT_LIB"
#define PLPLOT_TCL_ENV     "PLPLOT_TCL"
#define PLPLOT_HOME_ENV    "PLPLOT_HOME"

// Maximum size for path strings in the plplot code
#define PLPLOT_MAX_PATH    1024

//
//   Some stuff that is included (and compiled into) plsym.h
//   Other modules might want this, so we will "extern" it
//
//

#ifndef __PLSYM_H__

typedef struct
{
    unsigned int Hershey;
    PLUNICODE    Unicode;
    char         Font;
} Hershey_to_Unicode_table;

extern int number_of_entries_in_hershey_to_unicode_table;
extern Hershey_to_Unicode_table hershey_to_unicode_lookup_table[];


#endif

// Greek character translation array (defined in plcore.c)
extern const char plP_greek_mnemonic[];

//--------------------------------------------------------------------------
//		Function Prototypes
//
// These typically should not be called directly by the user.
//--------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Number of elements in the affine vector representation of the affine
// transformation matrix.
#define NAFFINE    6

// Returns affine identity matrix

PLDLLIMPEXP void
plP_affine_identity( PLFLT *affine_vector );

// Translate new coordinate system axes relative to the old.

PLDLLIMPEXP void
plP_affine_translate( PLFLT *affine_vector, PLFLT xtranslate, PLFLT ytranslate );

// Scale new coordinate system axes relative to the old.

PLDLLIMPEXP void
plP_affine_scale( PLFLT *affine_vector, PLFLT xscale, PLFLT yscale );

// Rotate new coordinate system axes relative to the old.
// angle is in degrees.

PLDLLIMPEXP void
plP_affine_rotate( PLFLT *affine_vector, PLFLT angle );

// Skew new X coordinate axis relative to the old.
// angle is in degrees.

PLDLLIMPEXP void
plP_affine_xskew( PLFLT *affine_vector, PLFLT angle );

// Skew new Y coordinate axis relative to the old.
// angle is in degrees.

PLDLLIMPEXP void
plP_affine_yskew( PLFLT *affine_vector, PLFLT angle );

// Multiply two affine transformation matrices to form a third.
//
// A = B * C
//
//

PLDLLIMPEXP void
plP_affine_multiply(
    PLFLT *affine_vectorA,
    PLFLT_VECTOR affine_vectorB,
    PLFLT_VECTOR affine_vectorC );

// Determines interval between numeric labels

void
pldtik( PLFLT vmin, PLFLT vmax, PLFLT *tick, PLINT *nsubt, PLBOOL ld );

// Writes numeric labels on side(s) of box in custom locations

void
label_box_custom( PLCHAR_VECTOR xopt, PLINT n_xticks, PLFLT_VECTOR xticks, PLCHAR_VECTOR yopt, PLINT n_yticks, PLFLT_VECTOR yticks );

// Determine factor to convert date / time in seconds to more natural
// units

void
pldtfac( PLFLT vmin, PLFLT vmax, PLFLT *factor, PLFLT *tstart );

// Determines precision of box labels

void
pldprec( PLFLT vmin, PLFLT vmax, PLFLT tick, PLINT lf,
         PLINT *mode, PLINT *prec, PLINT digmax, PLINT *scale );

// Draws a polyline within the clip limits.

void
plP_pllclp( PLINT *x, PLINT *y, PLINT npts,
            PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax,
            void ( *draw )( short *, short *, PLINT ) );

// Fills a polygon within the clip limits.

void
plP_plfclp( PLINT *x, PLINT *y, PLINT npts,
            PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax,
            void ( *draw )( short *, short *, PLINT ) );

// Clip a polygon to the 3d bounding plane
int
plP_clip_poly( int Ni, PLFLT *Vi[3], int axis, PLFLT dir, PLFLT offset );

// Get clipped endpoints

int
plP_clipline( PLINT *p_x1, PLINT *p_y1, PLINT *p_x2, PLINT *p_y2,
              PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax );

// Stores hex digit value into FCI (font characterization integer).
void
plP_hex2fci( unsigned char hexdigit, unsigned char hexpower, PLUNICODE *pfci );

// Retrieves hex digit value from FCI (font characterization integer).
PLDLLIMPEXP void
plP_fci2hex( PLUNICODE fci, unsigned char *phexdigit, unsigned char hexpower );

// Pattern fills in software the polygon bounded by the input points.

PLDLLIMPEXP void
plfill_soft( short *x, short *y, PLINT npts );

// In case of an abort this routine is called.  It just prints out an
// error message and tries to clean up as much as possible.

PLDLLIMPEXP void
plexit( PLCHAR_VECTOR errormsg );

// Just a front-end to exit().

void
pl_exit( void );

// A handy way to issue warnings, if need be.

PLDLLIMPEXP void
plwarn( PLCHAR_VECTOR errormsg );

// Same as plwarn(), but appends ", aborting plot" to the error message

PLDLLIMPEXP void
plabort( PLCHAR_VECTOR errormsg );

// Loads either the standard or extended font.

void
plfntld( PLINT fnt );

// Release memory for fonts.

void
plfontrel( void );

// A replacement for strdup(), which isn't portable.

PLDLLIMPEXP char *
plstrdup( PLCHAR_VECTOR src );

// Bin up cmap 1 space and assign colors to make inverse mapping easy.

PLDLLIMPEXP void
plcmap1_calc( void );

// Draws a slanting tick at position (mx,my) (measured in mm) of
// vector length (dx,dy).

void
plstik( PLFLT mx, PLFLT my, PLFLT dx, PLFLT dy );

// Prints out a "string" at reference position with physical coordinates
// (refx,refy).

void
plstr( PLINT base, PLFLT *xform, PLINT refx, PLINT refy, PLCHAR_VECTOR string );

// Draws a tick parallel to x.

void
plxtik( PLINT x, PLINT y, PLINT below, PLINT above );

// Draws a tick parallel to y.

void
plytik( PLINT x, PLINT y, PLINT left, PLINT right );

// Driver interface filter --
// passes all coordinates through a variety of filters.

PLDLLIMPEXP void
difilt( PLINT *, PLINT *, PLINT,
        PLINT *, PLINT *, PLINT *, PLINT * );

// Transforms the clipping region coordinates as necessary
// based on the current plot orientation, etc.

PLDLLIMPEXP void
difilt_clip( PLINT *, PLINT * );

// Calculate scale of font size and scale of magnitude of vertical
// offset associated with superscripts and subscripts.

PLDLLIMPEXP void
plP_script_scale( PLBOOL ifupper, PLINT *level,
                  PLFLT *old_scale, PLFLT *scale,
                  PLFLT *old_offset, PLFLT *offset );

// Driver draws text

void
plP_text( PLINT base, PLFLT just, PLFLT *xform, PLINT x, PLINT y,
          PLINT refx, PLINT refy, PLCHAR_VECTOR string );

// Save LC_NUMERIC locale string, then set "C" locale to protect
// parts of PLplot which absolutely demand the LC_NUMERIC "C" locale.

PLDLLIMPEXP char *
plsave_set_locale( void );

// Restore LC_NUMERIC locale that was determined by plsave_set_locale.

PLDLLIMPEXP void
plrestore_locale( char * save_lc_numeric_locale );

// Writes the Hershey symbol "ch" centred at the physical coordinate (x,y).
void
plhrsh( PLINT ch, PLINT x, PLINT y );

// where should structure definitions that must be seen by drivers and core source files, be?

// structure to be used by plcore.c and anydriver.c, related to plP_text()

typedef struct
{
    // Indicates the type of text stored in the structure.  This flag
    // is used by the plot metafiles to correctly store and then render
    // the contents.
    enum { PL_STRING_TEXT, PL_STRING_SYMBOL } text_type;

    // Positioning settings
    PLINT base;                    // ref point at base(1) or center(0) of text. Currently plplot only use 0
    PLFLT just;                    // continuos justification, 0 left, 0.5 center, 1 right
    PLFLT *xform;                  // transformation (rotation) matrix

    // raw reference point--after any transformation
    PLINT x;
    PLINT y;

    // processed ref. point--after justification, displacement, etc, processing
    PLINT refx;
    PLINT refy;

    // font face OPTIONALLY used for rendering hershey codes
    char  font_face;

    // The following 3 fields are used by the alternative text handling pathway.
    // The alternative text handling pathway allows the driver to process
    // each character individually for unicode font handling
    // See drivers/cairo.h for details about how this works.
    PLUNICODE n_fci;               // font storage
    PLUNICODE n_char;              // character storage
    PLINT     n_ctrl_char;         // control character

    // Used by plsym to store a unicode character for use by plfreetype
    PLUNICODE unicode_char;        // an int to hold either a Hershey, ASC-II, or Unicode value for plsym calls

    // Used to store a processed unicode string.  Used by plsym and
    // text rendering by the driver
    PLUNICODE      *unicode_array; // a pointer to an array of ints holding either a Hershey, ASC-II, or Unicode value for cached plsym
    unsigned short unicode_array_len;

    // Non-unicode strings and unprocessed string in the unicode case
    const char     *string;        // text to draw
    PLINT          symbol;         // plot symbol to draw
}EscText;

//
// structure that contains driver specific information, to be used by
// plargs.c and anydriver.c, related to plParseDrvOpts() and plHelpDrvOpts()
//

typedef struct
{
    PLCHAR_VECTOR opt;      // a string with the name of the option
    PLINT         type;     // the type of the variable to be set, see bellow the available types
    void          *var_ptr; // a pointer to the variable to be set
    PLCHAR_VECTOR hlp_msg;  // help message of the option
} DrvOpt;

// the available variable types, DrvOpt.type, for driver specific options

enum { DRV_INT, DRV_FLT, DRV_STR };

// parse driver specific options, as in -drvopt <option[=value]>*

PLDLLIMPEXP int
plParseDrvOpts( DrvOpt * );

// give help on driver specific options

void
plHelpDrvOpts( DrvOpt * );

//
// structures to store contour lines
//

#define LINE_ITEMS    20

typedef struct cont_line
{
    PLFLT            *x;
    PLFLT            *y;
    PLINT            npts;
    struct cont_line *next;
} CONT_LINE;

typedef struct cont_level
{
    // INDENT-OFF (stop infinite recursion)
    PLFLT level;             // contour level
    struct cont_line  *line; // contour line struct
    struct cont_level *next; // contour level struct
    // INDENT-ON
} CONT_LEVEL;

void
cont_store( PLFLT_MATRIX f, PLINT nx, PLINT ny,
            PLINT kx, PLINT lx, PLINT ky, PLINT ly,
            PLFLT_VECTOR clevel, PLINT nlevel,
            void ( *pltr )( PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer ),
            PLPointer pltr_data,
            CONT_LEVEL **contour );

void
cont_clean_store( CONT_LEVEL *ct );

// Get the viewport boundaries in world coordinates, expanded slightly

void
plP_xgvpw( PLFLT *p_xmin, PLFLT *p_xmax, PLFLT *p_ymin, PLFLT *p_ymax );

// Get x-y domain in world coordinates for 3d plots

void
plP_gdom( PLFLT *p_xmin, PLFLT *p_xmax, PLFLT *p_ymin, PLFLT *p_ymax );

// Get vertical (z) scale parameters for 3-d plot

void
plP_grange( PLFLT *p_zscl, PLFLT *p_zmin, PLFLT *p_zmax );

// Get parameters used in 3d plots

void
plP_gw3wc( PLFLT *p_dxx, PLFLT *p_dxy, PLFLT *p_dyx, PLFLT *p_dyy,
           PLFLT *p_dyz );

// Get clip boundaries in physical coordinates

void
plP_gclp( PLINT *p_ixmin, PLINT *p_ixmax, PLINT *p_iymin, PLINT *p_iymax );

// Set clip boundaries in physical coordinates

void
plP_sclp( PLINT ixmin, PLINT ixmax, PLINT iymin, PLINT iymax );

// Get physical device limits in physical coordinates

PLDLLIMPEXP void
plP_gphy( PLINT *p_ixmin, PLINT *p_ixmax, PLINT *p_iymin, PLINT *p_iymax );

// Get number of subpages on physical device and current subpage

PLDLLIMPEXP void
plP_gsub( PLINT *p_nx, PLINT *p_ny, PLINT *p_cs );

// Set number of subpages on physical device and current subpage

PLDLLIMPEXP void
plP_ssub( PLINT nx, PLINT ny, PLINT cs );

// Set up plot parameters according to the number of subpages.

void
plP_subpInit( void );

// Get number of pixels to a millimeter

PLDLLIMPEXP void
plP_gpixmm( PLFLT *p_x, PLFLT *p_y );

// All the drivers call this to set physical pixels/mm.

PLDLLIMPEXP void
plP_setpxl( PLFLT xpmm0, PLFLT ypmm0 );

// Get background parameters (including line width) for 3d plot.

void
plP_gzback( PLINT **zbf, PLINT **zbc, PLFLT **zbt, PLFLT **zbw );

// Move to physical coordinates (x,y).

void
plP_movphy( PLINT x, PLINT y );

// Draw to physical coordinates (x,y).

void
plP_draphy( PLINT x, PLINT y );

// Move to world coordinates (x,y).

void
plP_movwor( PLFLT x, PLFLT y );

// Draw to world coordinates (x,y).

void
plP_drawor( PLFLT x, PLFLT y );

// Draw polyline in physical coordinates.

void
plP_draphy_poly( PLINT *x, PLINT *y, PLINT n );

// Draw polyline in world coordinates.

void
plP_drawor_poly( PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT n );

// Sets up physical limits of plotting device.

PLDLLIMPEXP void
plP_setphy( PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax );

// Set up the subpage boundaries according to the current subpage selected

PLDLLIMPEXP void
plP_setsub( void );

// Get the floating point precision (in number of places) in numeric labels.

void
plP_gprec( PLINT *p_setp, PLINT *p_prec );

// Get the date / time format for numeric labels

const char *
plP_gtimefmt( void );

// Computes the length of a string in mm, including escape sequences.

PLFLT
plstrl( PLCHAR_VECTOR string );

// Similar to strpos, but searches for occurence of string str2.

PLINT
plP_stindex( PLCHAR_VECTOR str1, PLCHAR_VECTOR str2 );

// Searches string str for first occurence of character chr.

PLDLLIMPEXP PLINT
plP_strpos( PLCHAR_VECTOR str, int chr );

// Searches string str for character chr (case insensitive).

PLBOOL
plP_stsearch( PLCHAR_VECTOR str, int chr );

// Conversion functions

// device coords to physical coords (x)

PLINT
plP_dcpcx( PLFLT x );

// device coords to physical coords (y)

PLINT
plP_dcpcy( PLFLT y );

// millimeters from bottom left-hand corner to physical coords (x)

PLINT
plP_mmpcx( PLFLT x );

// millimeters from bottom left-hand corner to physical coords (y)

PLINT
plP_mmpcy( PLFLT y );

// world coords to physical coords (x)

PLINT
plP_wcpcx( PLFLT x );

// world coords to physical coords (y)

PLINT
plP_wcpcy( PLFLT y );

// physical coords to device coords (x)

PLFLT
plP_pcdcx( PLINT x );

// physical coords to device coords (y)

PLFLT
plP_pcdcy( PLINT y );

// millimeters from bottom left corner to device coords (x)

PLFLT
plP_mmdcx( PLFLT x );

// millimeters from bottom left corner to device coords (y)

PLFLT
plP_mmdcy( PLFLT y );

// world coords into device coords (x)

PLFLT
plP_wcdcx( PLFLT x );

// world coords into device coords (y)

PLFLT
plP_wcdcy( PLFLT y );

// subpage coords to device coords (x)

PLFLT
plP_scdcx( PLFLT x );

// subpage coords to device coords (y)

PLFLT
plP_scdcy( PLFLT y );

// device coords to millimeters from bottom left-hand corner (x)

PLFLT
plP_dcmmx( PLFLT x );

// device coords to millimeters from bottom left-hand corner (y)

PLFLT
plP_dcmmy( PLFLT y );

// world coords into millimeters (x)

PLFLT
plP_wcmmx( PLFLT x );

// world coords into millimeters (y)

PLFLT
plP_wcmmy( PLFLT y );

// device coords to subpage coords (x)

PLFLT
plP_dcscx( PLFLT x );

// device coords to subpage coords (y)

PLFLT
plP_dcscy( PLFLT y );

// 3-d coords to 2-d projection (x)

PLFLT
plP_w3wcx( PLFLT x, PLFLT y, PLFLT z );

// 3-d coords to 2-d projection (y)

PLFLT
plP_w3wcy( PLFLT x, PLFLT y, PLFLT z );

// 3-d coords to 2-d projection (z)

PLFLT
plP_w3wcz( PLFLT x, PLFLT y, PLFLT z );

// Returns the rotation and shear angle and stride from a plplot transformation matrix

PLDLLIMPEXP void
plRotationShear( PLFLT *xFormMatrix, PLFLT *rotation, PLFLT *shear, PLFLT *stride );

// Test whether a point is in a polygon.
int
plP_pointinpolygon( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y,
                    PLFLT xp, PLFLT yp );

// Driver calls

// Initialize device.

void
plP_init( void );

// Draw line between two points

void
plP_line( short *x, short *y );

// Draw polyline

void
plP_polyline( short *x, short *y, PLINT npts );

// Fill polygon

PLDLLIMPEXP void
plP_fill( short *x, short *y, PLINT npts );

// Render gradient

void
plP_gradient( short *x, short *y, PLINT npts );

// draw image

void
plP_image( PLFLT *z, PLINT nx, PLINT ny, PLFLT xmin, PLFLT ymin, PLFLT dx, PLFLT dy,
           void ( *pltr )( PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer ), PLPointer pltr_data );

// Structure for holding arc data
typedef struct
{
    PLFLT  x;
    PLFLT  y;
    PLFLT  a;
    PLFLT  b;
    PLFLT  angle1;
    PLFLT  angle2;
    PLFLT  rotate;
    PLBOOL fill;
} arc_struct;

// End of page

PLDLLIMPEXP void
plP_eop( void );

// End of page

PLDLLIMPEXP void
plP_bop( void );

// Tidy up device (flush buffers, close file, etc.)

void
plP_tidy( void );

// Change state.

PLDLLIMPEXP void
plP_state( PLINT op );

// Escape function, for driver-specific commands.

PLDLLIMPEXP void
plP_esc( PLINT op, void *ptr );

// Set up plot window parameters.

void
plP_swin( PLWindow *plwin );

// Wait for user input

PLDLLIMPEXP void
plP_wait( void );

// Return file pointer to lib file.

FILE *
plLibOpen( PLCHAR_VECTOR fn );

// Does required startup initialization of library.

PLDLLIMPEXP void
pllib_init( void );

// Does preliminary setup of device driver.

PLDLLIMPEXP void
pllib_devinit( void );

// Utility to copy one PLColor to another.

PLDLLIMPEXP void
pl_cpcolor( PLColor *to, PLColor *from );

// Does required startup initialization of a stream.

void
plstrm_init( void );

// Builds a list of the active devices/streams by device name

void
PLDLLIMPEXP plP_getinitdriverlist( char *names );

// Checks a give list of device names against active streams and returns the number of matches

PLINT
plP_checkdriverinit( char *names );

// disable writing to plot buffer and pixmap
void
NoBufferNoPixmap( void );

// restart writing to plot buffer and pixmap
void
RestoreWrite2BufferPixmap( void );

void
grimage( short *x, short *y, unsigned short *z, PLINT nx, PLINT ny );

PLDLLIMPEXP int
plInBuildTree( void );

void
plimageslow( PLFLT *idata, PLINT nx, PLINT ny,
             PLFLT xmin, PLFLT ymin, PLFLT dx, PLFLT dy,
             void ( *pltr )( PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer ),
             PLPointer pltr_data );

typedef struct
{
    PLFLT xmin, ymin, dx, dy;
} IMG_DT;

//
// void plfvect()
//
// Internal routine to plot a vector array with arbitrary coordinate
// and vector transformations.
// This is not currently intended to be called direct by the user
//
PLDLLIMPEXP void
plfvect( PLFLT ( *plf2eval )( PLINT, PLINT, PLPointer ),
         PLPointer f2evalv_data, PLPointer f2evalc_data,
         PLINT nx, PLINT ny, PLFLT scale,
         void ( *pltr )( PLFLT, PLFLT, PLFLT *, PLFLT *, PLPointer ),
         PLPointer pltr_data );

//
//  Internal function to get an index to the hershey table
//
int
plhershey2unicode( int in );

// struct used for FCI to FontName lookups.
typedef struct
{
    PLUNICODE           fci;
    const unsigned char *pfont;
} FCI_to_FontName_Table;

// Internal function to obtain a pointer to a valid font name.
PLDLLIMPEXP const char *
plP_FCI2FontName( PLUNICODE fci,
                  const FCI_to_FontName_Table lookup[], const int nlookup );


// Internal function to free memory from driver options
void
plP_FreeDrvOpts( void );

// Convert a ucs4 unichar to utf8 char string
PLDLLIMPEXP int
ucs4_to_utf8( PLUNICODE unichar, char *ptr );

//
// Wrapper functions for the system IO routines fread, fwrite
//

// wraps fwrite

void
plio_fwrite( void *, size_t, size_t, FILE * );

// wraps fread

void
plio_fread( void *, size_t, size_t, FILE * );

// wraps fgets

void
plio_fgets( char *, int, FILE * );

// Draws a tick parallel to x, using world coordinates
void
plwxtik( PLFLT x, PLFLT y, PLBOOL minor, PLBOOL invert );

// Draws a tick parallel to y, using world coordinates
void
plwytik( PLFLT x, PLFLT y, PLBOOL minor, PLBOOL invert );

// get drivers directory

#ifdef ENABLE_DYNDRIVERS
PLDLLIMPEXP const char*
plGetDrvDir( void );
#endif

#ifdef PL_HAVE_FREETYPE
PLDLLIMPEXP void
plD_FreeType_init( PLStream * );

PLDLLIMPEXP void
plD_render_freetype_text( PLStream *, EscText * );

PLDLLIMPEXP void
plD_FreeType_Destroy( PLStream * );

PLDLLIMPEXP void
pl_set_extended_cmap0( PLStream *, int, int );

#endif

// Create a temporary file securely
PLDLLIMPEXP FILE *
pl_create_tempfile( char **fname );

// Create a temporary fifo securely
PLDLLIMPEXP char *
pl_create_tempfifo( const char **p_fifoname, const char **p_dirname );

// Read a meta file
PLDLLIMPEXP void
plreadmetafile( char *infile );

#ifdef __cplusplus
}
#endif

#endif  // __PLPLOTP_H__
