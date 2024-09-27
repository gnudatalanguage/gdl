//  Macros and prototypes for the PLplot package.  This header file must
//  be included by all user codes.
//
//  Note: some systems allow the Fortran & C namespaces to clobber each
//  other.  So for PLplot to work from Fortran, we do some rather nasty
//  things to the externally callable C function names.  This shouldn't
//  affect any user programs in C as long as this file is included.
//
// Copyright (C) 1992 Tony Richardson.
// Copyright (C) 1992-2001 Geoffrey Furnish
// Copyright (C) 1992-2002 Maurice LeBrun
// Copyright (C) 1996 Rady Shouman
// Copyright (C) 2000-2019 Alan W. Irwin
// Copyright (C) 2001-2003 Joao Cardoso
// Copyright (C) 2001-2005 Rafael Laboissiere
// Copyright (C) 2004-2013 Andrew Ross
// Copyright (C) 2005-2008 Arjen Markus
// Copyright (C) 2006-2011 Hazen Babcock
// Copyright (C) 2008-2009 Werner Smekal
// Copyright (C) 2009-2013 Hezekiah M. Carty
// Copyright (C) 2013 Jerry Bauck
// Copyright (C) 2014-2018 Phil Rosenberg

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

#ifndef __PLPLOT_H__
#define __PLPLOT_H__

#include "plConfig.h"

//--------------------------------------------------------------------------
//    USING PLplot
//
// To use PLplot from C or C++, it is only necessary to
//
//      #include "plplot.h"
//
// This file does all the necessary setup to make PLplot accessible to
// your program as documented in the manual.  Additionally, this file
// allows you to request certain behavior by defining certain symbols
// before inclusion.  At the moment the only one is:
//
// #define DOUBLE	or..
// #define PL_DOUBLE
//
// This causes PLplot to use doubles instead of floats.  Use the type
// PLFLT everywhere in your code, and it will always be the right thing.
//
// Note: most of the functions visible here begin with "pl", while all
// of the data types and switches begin with "PL".  Eventually everything
// will conform to this rule in order to keep namespace pollution of the
// user code to a minimum.  All the PLplot source files actually include
// "plplotP.h", which includes this file as well as all the internally-
// visible declarations, etc.
//--------------------------------------------------------------------------

// The majority of PLplot source files require these, so..
// Under ANSI C, they can be included any number of times.

#include <stdio.h>
#include <stdlib.h>

//--------------------------------------------------------------------------
//        SYSTEM IDENTIFICATION
//
// Several systems are supported directly by PLplot.  In order to avoid
// confusion, one id macro per system is used.  Since different compilers
// may predefine different system id macros, we need to check all the
// possibilities, and then set the one we will be referencing.  These are:
//
// __cplusplus                Any C++ compiler
// __unix                     Any Unix-like system
// __hpux                     Any HP/UX system
// __aix                      Any AIX system
// __linux                    Linux for i386
// (others...)
//
//--------------------------------------------------------------------------

#ifdef unix                     // the old way
#ifndef __unix
#define __unix
#endif
#endif

#if 0
#if defined ( __GNUC__ ) && __GNUC__ > 3
// If gcc 4.x, then turn off all visibility of symbols unless
// specified as visible using PLDLLIMPEXP.
//#pragma GCC visibility push(hidden)
// temporary until issues with above hidden can be sorted out
  #pragma GCC visibility push(default)
#endif
#endif
// Make sure Unix systems define "__unix"

#if defined ( SX ) ||                                 /* NEC Super-UX */      \
    ( defined ( _IBMR2 ) && defined ( _AIX ) ) ||     /* AIX */               \
    defined ( __hpux ) ||                             /* HP/UX */             \
    defined ( sun ) ||                                /* SUN */               \
    defined ( CRAY ) ||                               /* Cray */              \
    defined ( __convexc__ ) ||                        /* CONVEX */            \
    ( defined ( __alpha ) && defined ( __osf__ ) ) || /* DEC Alpha AXP/OSF */ \
    defined ( __APPLE__ )                             // Max OS-X
#ifndef __unix
#define __unix
#endif
#endif

//--------------------------------------------------------------------------
// dll functions
//--------------------------------------------------------------------------
#include "pldll.h"

// Macro to mark function parameters as unused.
// For gcc this uses the unused attribute to remove compiler warnings.
// For all compilers the parameter name is also mangled to prevent
// accidental use.
#ifdef PL_UNUSED
#elif defined ( __GNUC__ )
# define PL_UNUSED( x )    UNUSED_ ## x __attribute__( ( unused ) )
#else
# define PL_UNUSED( x )    UNUSED_ ## x
#endif

//--------------------------------------------------------------------------
// Base types for PLplot
//
// Only those that are necessary for function prototypes are defined here.
// Notes:
//
// short is currently used for device page coordinates, so they are
// bounded by (-32767, 32767).  This gives a max resolution of about 3000
// dpi, and improves performance in some areas over using a PLINT.
//
// PLUNICODE should be a 32-bit unsigned integer on all platforms.
// For now, we are using unsigned int for our Linux ix86 unicode experiments,
// but that doesn't guarantee 32 bits exactly on all platforms so this will
// be subject to change.
//--------------------------------------------------------------------------

#if defined ( PL_DOUBLE ) || defined ( DOUBLE )
typedef double   PLFLT;
#define PLFLT_MAX         DBL_MAX
#define PLFLT_MIN         DBL_MIN
#define PLFLT_HUGE_VAL    HUGE_VAL
#else
typedef float    PLFLT;
#define PLFLT_MAX         FLT_MAX
#define PLFLT_MIN         FLT_MIN
#define PLFLT_HUGE_VAL    HUGE_VALF
#endif

#if ( defined ( PL_HAVE_STDINT_H ) && !defined ( __cplusplus ) ) || \
    ( defined ( __cplusplus ) && defined ( PL_HAVE_CXX_STDINT_H ) )
#include <stdint.h>
// This is apparently portable if stdint.h exists.
typedef uint32_t       PLUINT;
typedef int32_t        PLINT;
typedef int64_t        PLINT64;
#define PLINT_MIN    INT32_MIN
#define PLINT_MAX    INT32_MAX
#else
// A reasonable back-up in case stdint.h does not exist on the platform.
typedef unsigned int   PLUINT;
typedef int            PLINT;
typedef __int64        PLINT64;
// for Visual C++ 2003 and later INT_MIN must be used, otherwise
//  PLINT_MIN is unsigned and 2147483648 NOT -2147483648, see
//  http://msdn.microsoft.com/en-us/library/4kh09110(VS.71).aspx for
//  details
#if defined ( _MSC_VER ) && _MSC_VER >= 1310
  #include <Limits.h>
  #define PLINT_MIN    INT_MIN
#else
  #define PLINT_MIN    -2147483648
#endif
//
// typedef unsigned int PLUINT;
// typedef int PLINT;
// typedef long long PLINT64;
//
#endif

// For identifying unicode characters
typedef PLUINT   PLUNICODE;

// For identifying logical (boolean) arguments
typedef PLINT    PLBOOL;

// typedefs for generic pointers.

// generic pointer to mutable object:
typedef void *   PLPointer;

// PLFLT first element pointers which are used to point to the first
// element of a contigous block of memory containing a PLFLT array with
// an arbitrary number of dimensions.

// mutable version
typedef PLFLT *       PLFLT_NC_FE_POINTER;
// immutable version
typedef const PLFLT * PLFLT_FE_POINTER;

// typedefs that are typically used for passing scalar, vector, and
// matrix arguments to functions.  The NC attribute concerns pointers
// to mutable objects, where the objects are used for passing values
// that are either output only or both input and output.  Pointers whose
// name does not contain the NC attribute point to immutable objects
// which are strictly input and guaranteed to be unchanged by the function.
//

// Pointers to mutable scalars:
typedef PLINT *               PLINT_NC_SCALAR;
typedef PLBOOL *              PLBOOL_NC_SCALAR;
typedef PLUNICODE *           PLUNICODE_NC_SCALAR;
typedef char *                PLCHAR_NC_SCALAR;
typedef PLFLT *               PLFLT_NC_SCALAR;

// Pointers to mutable vectors:
typedef PLINT *               PLINT_NC_VECTOR;
typedef char *                PLCHAR_NC_VECTOR;
typedef PLFLT *               PLFLT_NC_VECTOR;

// Pointers to immutable vectors:
typedef const PLINT *         PLINT_VECTOR;
typedef const PLBOOL *        PLBOOL_VECTOR;
typedef const char *          PLCHAR_VECTOR;
typedef const PLFLT *         PLFLT_VECTOR;

// Pointers to mutable 2-dimensional matrices:
typedef char **               PLCHAR_NC_MATRIX;
typedef PLFLT **              PLFLT_NC_MATRIX;

// Pointers to immutable 2-dimensional matrices,
// (i.e., pointers to const pointers to const values):
typedef const char * const *  PLCHAR_MATRIX;
typedef const PLFLT * const * PLFLT_MATRIX;

// Callback-related typedefs
typedef void ( *PLMAPFORM_callback )( PLINT n, PLFLT_NC_VECTOR x, PLFLT_NC_VECTOR y );
typedef void ( *PLTRANSFORM_callback )( PLFLT x, PLFLT y, PLFLT_NC_SCALAR xp, PLFLT_NC_SCALAR yp, PLPointer data );
typedef void ( *PLLABEL_FUNC_callback )( PLINT axis, PLFLT value, PLCHAR_NC_VECTOR label, PLINT length, PLPointer data );
typedef PLFLT ( *PLF2EVAL_callback )( PLINT ix, PLINT iy, PLPointer data );
typedef void ( *PLFILL_callback )( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y );
typedef PLINT ( *PLDEFINED_callback )( PLFLT x, PLFLT y );

//--------------------------------------------------------------------------
// Complex data types and other good stuff
//--------------------------------------------------------------------------

// Switches for escape function call.
// Some of these are obsolete but are retained in order to process
// old metafiles

#define PLESC_SET_RGB                   1  // obsolete
#define PLESC_ALLOC_NCOL                2  // obsolete
#define PLESC_SET_LPB                   3  // obsolete
#define PLESC_EXPOSE                    4  // handle window expose
#define PLESC_RESIZE                    5  // handle window resize
#define PLESC_REDRAW                    6  // handle window redraw
#define PLESC_TEXT                      7  // switch to text screen
#define PLESC_GRAPH                     8  // switch to graphics screen
#define PLESC_FILL                      9  // fill polygon
#define PLESC_DI                        10 // handle DI command
#define PLESC_FLUSH                     11 // flush output
#define PLESC_EH                        12 // handle Window events
#define PLESC_GETC                      13 // get cursor position
#define PLESC_SWIN                      14 // set window parameters
#define PLESC_DOUBLEBUFFERING           15 // configure double buffering
#define PLESC_XORMOD                    16 // set xor mode
#define PLESC_SET_COMPRESSION           17 // AFR: set compression
#define PLESC_CLEAR                     18 // RL: clear graphics region
#define PLESC_DASH                      19 // RL: draw dashed line
#define PLESC_HAS_TEXT                  20 // driver draws text
#define PLESC_IMAGE                     21 // handle image
#define PLESC_IMAGEOPS                  22 // plimage related operations
#define PLESC_PL2DEVCOL                 23 // convert PLColor to device color
#define PLESC_DEV2PLCOL                 24 // convert device color to PLColor
#define PLESC_SETBGFG                   25 // set BG, FG colors
#define PLESC_DEVINIT                   26 // alternate device initialization
#define PLESC_GETBACKEND                27 // get used backend of (wxWidgets) driver - no longer used
#define PLESC_BEGIN_TEXT                28 // get ready to draw a line of text
#define PLESC_TEXT_CHAR                 29 // render a character of text
#define PLESC_CONTROL_CHAR              30 // handle a text control character (super/subscript, etc.)
#define PLESC_END_TEXT                  31 // finish a drawing a line of text
#define PLESC_START_RASTERIZE           32 // start rasterized rendering
#define PLESC_END_RASTERIZE             33 // end rasterized rendering
#define PLESC_ARC                       34 // render an arc
#define PLESC_GRADIENT                  35 // render a gradient
#define PLESC_MODESET                   36 // set drawing mode
#define PLESC_MODEGET                   37 // get drawing mode
#define PLESC_FIXASPECT                 38 // set or unset fixing the aspect ratio of the plot
#define PLESC_IMPORT_BUFFER             39 // set the contents of the buffer to a specified byte string
#define PLESC_APPEND_BUFFER             40 // append the given byte string to the buffer
#define PLESC_FLUSH_REMAINING_BUFFER    41 // flush the remaining buffer e.g. after new data was appended

// Alternative unicode text handling control characters
#define PLTEXT_FONTCHANGE               0 // font change in the text stream
#define PLTEXT_SUPERSCRIPT              1 // superscript in the text stream
#define PLTEXT_SUBSCRIPT                2 // subscript in the text stream
#define PLTEXT_BACKCHAR                 3 // back-char in the text stream
#define PLTEXT_OVERLINE                 4 // toggle overline in the text stream
#define PLTEXT_UNDERLINE                5 // toggle underline in the text stream

// image operations
#define ZEROW2B                         1
#define ZEROW2D                         2
#define ONEW2B                          3
#define ONEW2D                          4

// Window parameter tags

#define PLSWIN_DEVICE    1              // device coordinates
#define PLSWIN_WORLD     2              // world coordinates

// Axis label tags
#define PL_X_AXIS        1              // The x-axis
#define PL_Y_AXIS        2              // The y-axis
#define PL_Z_AXIS        3              // The z-axis

// PLplot Option table & support constants

// Option-specific settings

#define PL_OPT_ENABLED      0x0001      // Obsolete
#define PL_OPT_ARG          0x0002      // Option has an argument
#define PL_OPT_NODELETE     0x0004      // Don't delete after processing
#define PL_OPT_INVISIBLE    0x0008      // Make invisible
#define PL_OPT_DISABLED     0x0010      // Processing is disabled

// Option-processing settings -- mutually exclusive

#define PL_OPT_FUNC      0x0100         // Call handler function
#define PL_OPT_BOOL      0x0200         // Set *var = 1
#define PL_OPT_INT       0x0400         // Set *var = atoi(optarg)
#define PL_OPT_FLOAT     0x0800         // Set *var = atof(optarg)
#define PL_OPT_STRING    0x1000         // Set var = optarg

// Global mode settings
// These override per-option settings

#define PL_PARSE_PARTIAL              0x0000 // For backward compatibility
#define PL_PARSE_FULL                 0x0001 // Process fully & exit if error
#define PL_PARSE_QUIET                0x0002 // Don't issue messages
#define PL_PARSE_NODELETE             0x0004 // Don't delete options after
                                             // processing
#define PL_PARSE_SHOWALL              0x0008 // Show invisible options
#define PL_PARSE_OVERRIDE             0x0010 // Obsolete
#define PL_PARSE_NOPROGRAM            0x0020 // Program name NOT in *argv[0]..
#define PL_PARSE_NODASH               0x0040 // Set if leading dash NOT required
#define PL_PARSE_SKIP                 0x0080 // Skip over unrecognized args

// FCI (font characterization integer) related constants.
#define PL_FCI_MARK                   0x80000000
#define PL_FCI_IMPOSSIBLE             0x00000000
#define PL_FCI_HEXDIGIT_MASK          0xf
#define PL_FCI_HEXPOWER_MASK          0x7
#define PL_FCI_HEXPOWER_IMPOSSIBLE    0xf
// These define hexpower values corresponding to each font attribute.
#define PL_FCI_FAMILY                 0x0
#define PL_FCI_STYLE                  0x1
#define PL_FCI_WEIGHT                 0x2
// These are legal values for font family attribute
#define PL_FCI_SANS                   0x0
#define PL_FCI_SERIF                  0x1
#define PL_FCI_MONO                   0x2
#define PL_FCI_SCRIPT                 0x3
#define PL_FCI_SYMBOL                 0x4
// These are legal values for font style attribute
#define PL_FCI_UPRIGHT                0x0
#define PL_FCI_ITALIC                 0x1
#define PL_FCI_OBLIQUE                0x2
// These are legal values for font weight attribute
#define PL_FCI_MEDIUM                 0x0
#define PL_FCI_BOLD                   0x1

// Option table definition

typedef struct
{
    PLCHAR_VECTOR opt;
    int ( *handler )( PLCHAR_VECTOR, PLCHAR_VECTOR, PLPointer );
    PLPointer     client_data;
    PLPointer     var;
    long          mode;
    PLCHAR_VECTOR syntax;
    PLCHAR_VECTOR desc;
} PLOptionTable;

// PLplot Graphics Input structure

#define PL_MAXKEY    16

//Masks for use with PLGraphicsIn::state
//These exactly coincide with the X11 masks
//from X11/X.h, however the values 1<<3 to
//1<<7 aparently may vary depending upon
//X implementation and keyboard
// Numerical #defines are parsed further to help determine
// additional files such as ../bindings/swig-support/plplotcapi.i
// so must #define numerical #defines with numbers rather than C operators
// such as <<.
#define PL_MASK_SHIFT      0x1    // ( 1 << 0 )
#define PL_MASK_CAPS       0x2    // ( 1 << 1 )
#define PL_MASK_CONTROL    0x4    // ( 1 << 2 )
#define PL_MASK_ALT        0x8    // ( 1 << 3 )
#define PL_MASK_NUM        0x10   // ( 1 << 4 )
#define PL_MASK_ALTGR      0x20   //  ( 1 << 5 )
#define PL_MASK_WIN        0x40   // ( 1 << 6 )
#define PL_MASK_SCROLL     0x80   // ( 1 << 7 )
#define PL_MASK_BUTTON1    0x100  // ( 1 << 8 )
#define PL_MASK_BUTTON2    0x200  // ( 1 << 9 )
#define PL_MASK_BUTTON3    0x400  // ( 1 << 10 )
#define PL_MASK_BUTTON4    0x800  // ( 1 << 11 )
#define PL_MASK_BUTTON5    0x1000 // ( 1 << 12 )

typedef struct
{
    int          type;              // of event (CURRENTLY UNUSED)
    unsigned int state;             // key or button mask
    unsigned int keysym;            // key selected
    unsigned int button;            // mouse button selected
    PLINT        subwindow;         // subwindow (alias subpage, alias subplot) number
    char         string[PL_MAXKEY]; // translated string
    int          pX, pY;            // absolute device coordinates of pointer
    PLFLT        dX, dY;            // relative device coordinates of pointer
    PLFLT        wX, wY;            // world coordinates of pointer
} PLGraphicsIn;

// Structure for describing the plot window

#define PL_MAXWINDOWS    64     // Max number of windows/page tracked

typedef struct
{
    PLFLT dxmi, dxma, dymi, dyma;       // min, max window rel dev coords
    PLFLT wxmi, wxma, wymi, wyma;       // min, max window world coords
} PLWindow;

// Structure for doing display-oriented operations via escape commands
// May add other attributes in time

typedef struct
{
    unsigned int x, y;                  // upper left hand corner
    unsigned int width, height;         // window dimensions
} PLDisplay;

// Macro used (in some cases) to ignore value of argument
// I don't plan on changing the value so you can hard-code it

#define PL_NOTSET    ( -42 )

// See plcont.c for examples of the following

//
// PLfGrid is for passing (as a pointer to the first element) an arbitrarily
// dimensioned array.  The grid dimensions MUST be stored, with a maximum of 3
// dimensions assumed for now.
//

typedef struct
{
    PLFLT_FE_POINTER f;
    PLINT            nx, ny, nz;
} PLfGrid;

//
// PLfGrid2 is for passing (as an array of pointers) a 2d function array.  The
// grid dimensions are passed for possible bounds checking.
//

typedef struct
{
    PLFLT_NC_MATRIX f;
    PLINT           nx, ny;
} PLfGrid2;

//
// NOTE: a PLfGrid3 is a good idea here but there is no way to exploit it yet
// so I'll leave it out for now.
//

//
// PLcGrid is for passing (as a pointer to the first element) arbitrarily
// dimensioned coordinate transformation arrays.  The grid dimensions MUST be
// stored, with a maximum of 3 dimensions assumed for now.
//

typedef struct
{
    PLFLT_NC_FE_POINTER xg, yg, zg;
    PLINT nx, ny, nz;
} PLcGrid;

//
// PLcGrid2 is for passing (as arrays of pointers) 2d coordinate
// transformation arrays.  The grid dimensions are passed for possible bounds
// checking.
//

typedef struct
{
    PLFLT_NC_MATRIX xg, yg, zg;
    PLINT           nx, ny;
} PLcGrid2;

//
// NOTE: a PLcGrid3 is a good idea here but there is no way to exploit it yet
// so I'll leave it out for now.
//

// Color limits:

// Default number of colors for cmap0 and cmap1.
#define PL_DEFAULT_NCOL0    16
#define PL_DEFAULT_NCOL1    128
// minimum and maximum PLINT RGB values.
#define MIN_PLINT_RGB       0
#define MAX_PLINT_RGB       255
// minimum and maximum PLFLT cmap1 color index values.
#define MIN_PLFLT_CMAP1     0.
#define MAX_PLFLT_CMAP1     1.
// minimum and maximum PLFLT alpha values.
#define MIN_PLFLT_ALPHA     0.
#define MAX_PLFLT_ALPHA     1.

// PLColor is the usual way to pass an rgb color value.

typedef struct
{
    unsigned char r;            // red
    unsigned char g;            // green
    unsigned char b;            // blue
    PLFLT         a;            // alpha (or transparency)
    PLCHAR_VECTOR name;
} PLColor;

// PLControlPt is how cmap1 control points are represented.

typedef struct
{
    PLFLT c1;                   // hue or red
    PLFLT c2;                   // lightness or green
    PLFLT c3;                   // saturation or blue
    PLFLT p;                    // position
    PLFLT a;                    // alpha (or transparency)
    int   alt_hue_path;         // if set, interpolate through h=0
} PLControlPt;

// A PLBufferingCB is a control block for interacting with devices
// that support double buffering.

typedef struct
{
    PLINT cmd;
    PLINT result;
} PLBufferingCB;

#define PLESC_DOUBLEBUFFERING_ENABLE     1
#define PLESC_DOUBLEBUFFERING_DISABLE    2
#define PLESC_DOUBLEBUFFERING_QUERY      3

typedef struct
{
    PLFLT exp_label_disp;
    PLFLT exp_label_pos;
    PLFLT exp_label_just;
} PLLabelDefaults;

//
// typedefs for access methods for arbitrary (i.e. user defined) data storage
//

//
// This type of struct holds pointers to functions that are used to
// get, set, modify, and test individual 2-D data points referenced by
// a PLPointer or PLPointer.  How these
// generic pointers are used depends entirely on the functions
// that implement the various operations.  Certain common data
// representations have predefined instances of this structure
// prepopulated with pointers to predefined functions.
//

typedef struct
{
    PLFLT ( *get )( PLPointer p, PLINT ix, PLINT iy );
    PLFLT ( *set )( PLPointer p, PLINT ix, PLINT iy, PLFLT z );
    PLFLT ( *add )( PLPointer p, PLINT ix, PLINT iy, PLFLT z );
    PLFLT ( *sub )( PLPointer p, PLINT ix, PLINT iy, PLFLT z );
    PLFLT ( *mul )( PLPointer p, PLINT ix, PLINT iy, PLFLT z );
    PLFLT ( *div )( PLPointer p, PLINT ix, PLINT iy, PLFLT z );
    PLINT ( *is_nan )( PLPointer p, PLINT ix, PLINT iy );
    void ( *minmax )( PLPointer p, PLINT nx, PLINT ny, PLFLT_NC_SCALAR zmin, PLFLT_NC_SCALAR zmax );
    //
    // f2eval is backwards compatible signature for "f2eval" functions that
    // existed before plf2ops "operator function families" were used.
    //
    PLFLT ( *f2eval )( PLINT ix, PLINT iy, PLPointer p );
} plf2ops_t;

//
// A typedef to facilitate declaration of a pointer to a plfops_t structure.
//

typedef plf2ops_t * PLF2OPS;

//
// A struct to pass a buffer around
//
typedef struct
{
    size_t    size;
    PLPointer buffer;
} plbuffer;

//--------------------------------------------------------------------------
//		BRAINDEAD-ness
//
// Some systems allow the Fortran & C namespaces to clobber each other.
// For PLplot to work from Fortran on these systems, we must name the the
// externally callable C functions something other than their Fortran entry
// names.  In order to make this as easy as possible for the casual user,
// yet reversible to those who abhor my solution, I have done the
// following:
//
//	The C-language bindings are actually different from those
//	described in the manual.  Macros are used to convert the
//	documented names to the names used in this package.  The
//	user MUST include plplot.h in order to get the name
//	redefinition correct.
//
// Sorry to have to resort to such an ugly kludge, but it is really the
// best way to handle the situation at present.  If all available
// compilers offer a way to correct this stupidity, then perhaps we can
// eventually reverse it.
//
// If you feel like screaming at someone (I sure do), please
// direct it at your nearest system vendor who has a braindead shared
// C/Fortran namespace.  Some vendors do offer compiler switches that
// change the object names, but then everybody who wants to use the
// package must throw these same switches, leading to no end of trouble.
//
// Note that this definition should not cause any noticable effects except
// when debugging PLplot calls, in which case you will need to remember
// the real function names (same as before but with a 'c_' prepended).
//
// Also, to avoid macro conflicts, the BRAINDEAD part must not be expanded
// in the stub routines.
//
// Aside: the reason why a shared Fortran/C namespace is deserving of the
// BRAINDEAD characterization is that it completely precludes the the kind
// of universal API that is attempted (more or less) with PLplot, without
// Herculean efforts (e.g. remapping all of the C bindings by macros as
// done here).  The vendors of such a scheme, in order to allow a SINGLE
// type of argument to be passed transparently between C and Fortran,
// namely, a pointer to a conformable data type, have slammed the door on
// insertion of stub routines to handle the conversions needed for other
// data types.  Intelligent linkers could solve this problem, but these are
// not anywhere close to becoming universal.  So meanwhile, one must live
// with either stub routines for the inevitable data conversions, or a
// different API.  The former is what is used here, but is made far more
// difficult in a braindead shared Fortran/C namespace.
//--------------------------------------------------------------------------

#ifndef BRAINDEAD
#define BRAINDEAD
#endif

#ifdef BRAINDEAD

#ifndef __PLSTUBS_H__   // i.e. do not expand this in the stubs

#define    pl_setcontlabelformat    c_pl_setcontlabelformat
#define    pl_setcontlabelparam     c_pl_setcontlabelparam
#define    pladv                    c_pladv
#define    plarc                    c_plarc
#define    plaxes                   c_plaxes
#define    plbin                    c_plbin
#define    plbop                    c_plbop
#define    plbox                    c_plbox
#define    plbox3                   c_plbox3
//#define    plbtime                  c_plbtime
#define    plcalc_world             c_plcalc_world
#define    plclear                  c_plclear
#define    plcol0                   c_plcol0
#define    plcol1                   c_plcol1
#define    plcolorbar               c_plcolorbar
#define    plconfigtime             c_plconfigtime
#define    plcont                   c_plcont
#define    plcpstrm                 c_plcpstrm
//#define    plctime                  c_plctime
#define    plend                    c_plend
#define    plend1                   c_plend1
#define    plenv                    c_plenv
#define    plenv0                   c_plenv0
#define    pleop                    c_pleop
#define    plerrx                   c_plerrx
#define    plerry                   c_plerry
#define    plfamadv                 c_plfamadv
#define    plfill                   c_plfill
#define    plfill3                  c_plfill3
#define    plflush                  c_plflush
#define    plfont                   c_plfont
#define    plfontld                 c_plfontld
#define    plgchr                   c_plgchr
#define    plgcmap1_range           c_plgcmap1_range
#define    plgcol0                  c_plgcol0
#define    plgcol0a                 c_plgcol0a
#define    plgcolbg                 c_plgcolbg
#define    plgcolbga                c_plgcolbga
#define    plgcompression           c_plgcompression
#define    plgdev                   c_plgdev
#define    plgdidev                 c_plgdidev
#define    plgdiori                 c_plgdiori
#define    plgdiplt                 c_plgdiplt
#define    plgdrawmode              c_plgdrawmode
#define    plgfam                   c_plgfam
#define    plgfci                   c_plgfci
#define    plgfnam                  c_plgfnam
#define    plgfont                  c_plgfont
#define    plglevel                 c_plglevel
#define    plgpage                  c_plgpage
#define    plgra                    c_plgra
#define    plgradient               c_plgradient
#define    plgriddata               c_plgriddata
#define    plgspa                   c_plgspa
#define    plgstrm                  c_plgstrm
#define    plgver                   c_plgver
#define    plgvpd                   c_plgvpd
#define    plgvpw                   c_plgvpw
#define    plgxax                   c_plgxax
#define    plgyax                   c_plgyax
#define    plgzax                   c_plgzax
#define    plhist                   c_plhist
#define    plhlsrgb                 c_plhlsrgb
#define    plimage                  c_plimage
#define    plimagefr                c_plimagefr
#define    plinit                   c_plinit
#define    pljoin                   c_pljoin
#define    pllab                    c_pllab
#define    pllegend                 c_pllegend
#define    pllightsource            c_pllightsource
#define    plline                   c_plline
#define    plpath                   c_plpath
#define    plline3                  c_plline3
#define    pllsty                   c_pllsty
#define    plmap                    c_plmap
#define    plmapline                c_plmapline
#define    plmapstring              c_plmapstring
#define    plmaptex                 c_plmaptex
#define    plmapfill                c_plmapfill
#define    plmeridians              c_plmeridians
#define    plmesh                   c_plmesh
#define    plmeshc                  c_plmeshc
#define    plmkstrm                 c_plmkstrm
#define    plmtex                   c_plmtex
#define    plmtex3                  c_plmtex3
#define    plot3d                   c_plot3d
#define    plot3dc                  c_plot3dc
#define    plot3dcl                 c_plot3dcl
#define    plparseopts              c_plparseopts
#define    plpat                    c_plpat
#define    plpoin                   c_plpoin
#define    plpoin3                  c_plpoin3
#define    plpoly3                  c_plpoly3
#define    plprec                   c_plprec
#define    plpsty                   c_plpsty
#define    plptex                   c_plptex
#define    plptex3                  c_plptex3
#define    plrandd                  c_plrandd
#define    plreplot                 c_plreplot
#define    plrgbhls                 c_plrgbhls
#define    plschr                   c_plschr
#define    plscmap0                 c_plscmap0
#define    plscmap0a                c_plscmap0a
#define    plscmap0n                c_plscmap0n
#define    plscmap1                 c_plscmap1
#define    plscmap1a                c_plscmap1a
#define    plscmap1l                c_plscmap1l
#define    plscmap1la               c_plscmap1la
#define    plscmap1n                c_plscmap1n
#define    plscmap1_range           c_plscmap1_range
#define    plscol0                  c_plscol0
#define    plscol0a                 c_plscol0a
#define    plscolbg                 c_plscolbg
#define    plscolbga                c_plscolbga
#define    plscolor                 c_plscolor
#define    plscompression           c_plscompression
#define    plsdev                   c_plsdev
#define    plsdidev                 c_plsdidev
#define    plsdimap                 c_plsdimap
#define    plsdiori                 c_plsdiori
#define    plsdiplt                 c_plsdiplt
#define    plsdiplz                 c_plsdiplz
#define    plsdrawmode              c_plsdrawmode
#define    plseed                   c_plseed
#define    plsesc                   c_plsesc
#define    plsetopt                 c_plsetopt
#define    plsfam                   c_plsfam
#define    plsfci                   c_plsfci
#define    plsfnam                  c_plsfnam
#define    plsfont                  c_plsfont
#define    plshade                  c_plshade
#ifdef PL_DEPRECATED
#define    plshade1                 c_plshade1
#endif // PL_DEPRECATED
#define    plshades                 c_plshades
#define    plslabelfunc             c_plslabelfunc
#define    plsmaj                   c_plsmaj
#define    plsmem                   c_plsmem
#define    plsmema                  c_plsmema
#define    plsmin                   c_plsmin
#define    plsori                   c_plsori
#define    plspage                  c_plspage
#define    plspal0                  c_plspal0
#define    plspal1                  c_plspal1
#define    plspause                 c_plspause
#define    plsstrm                  c_plsstrm
#define    plssub                   c_plssub
#define    plssym                   c_plssym
#define    plstar                   c_plstar
#define    plstart                  c_plstart
#define    plstransform             c_plstransform
#define    plstring                 c_plstring
#define    plstring3                c_plstring3
#define    plstripa                 c_plstripa
#define    plstripc                 c_plstripc
#define    plstripd                 c_plstripd
#define    plstyl                   c_plstyl
#define    plsurf3d                 c_plsurf3d
#define    plsurf3dl                c_plsurf3dl
#define    plsvect                  c_plsvect
#define    plsvpa                   c_plsvpa
#define    plsxax                   c_plsxax
#define    plsyax                   c_plsyax
#define    plsym                    c_plsym
#define    plszax                   c_plszax
#define    pltext                   c_pltext
#define    pltimefmt                c_pltimefmt
#define    plvasp                   c_plvasp
#define    plvect                   c_plvect
#define    plvpas                   c_plvpas
#define    plvpor                   c_plvpor
#define    plvsta                   c_plvsta
#define    plw3d                    c_plw3d
#define    plwidth                  c_plwidth
#define    plwind                   c_plwind
#define    plxormod                 c_plxormod

#endif  // __PLSTUBS_H__

#endif  // BRAINDEAD

//--------------------------------------------------------------------------
//		Function Prototypes
//--------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// All void types

// C routines callable from stub routines come first

// set the format of the contour labels

PLDLLIMPEXP void
c_pl_setcontlabelformat( PLINT lexp, PLINT sigdig );

// set offset and spacing of contour labels

PLDLLIMPEXP void
c_pl_setcontlabelparam( PLFLT offset, PLFLT size, PLFLT spacing, PLINT active );

// Advance to subpage "page", or to the next one if "page" = 0.

PLDLLIMPEXP void
c_pladv( PLINT page );

// Plot an arc

PLDLLIMPEXP void
c_plarc( PLFLT x, PLFLT y, PLFLT a, PLFLT b, PLFLT angle1, PLFLT angle2,
         PLFLT rotate, PLBOOL fill );

// This functions similarly to plbox() except that the origin of the axes
// is placed at the user-specified point (x0, y0).

PLDLLIMPEXP void
c_plaxes( PLFLT x0, PLFLT y0, PLCHAR_VECTOR xopt, PLFLT xtick, PLINT nxsub,
          PLCHAR_VECTOR yopt, PLFLT ytick, PLINT nysub );

// Plot a histogram using x to store data values and y to store frequencies

// Flags for plbin() - opt argument
#define PL_BIN_DEFAULT     0x0
#define PL_BIN_CENTRED     0x1
#define PL_BIN_NOEXPAND    0x2
#define PL_BIN_NOEMPTY     0x4

PLDLLIMPEXP void
c_plbin( PLINT nbin, PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT opt );

//// Calculate broken-down time from continuous time for current stream.
//PLDLLIMPEXP void
//c_plbtime( PLINT_NC_SCALAR year, PLINT_NC_SCALAR month, PLINT_NC_SCALAR day, PLINT_NC_SCALAR hour, PLINT_NC_SCALAR min, PLFLT_NC_SCALAR sec, PLFLT ctime );

// Start new page.  Should only be used with pleop().

PLDLLIMPEXP void
c_plbop( void );

// This draws a box around the current viewport.

PLDLLIMPEXP void
c_plbox( PLCHAR_VECTOR xopt, PLFLT xtick, PLINT nxsub,
         PLCHAR_VECTOR yopt, PLFLT ytick, PLINT nysub );

// This is the 3-d analogue of plbox().

PLDLLIMPEXP void
c_plbox3( PLCHAR_VECTOR xopt, PLCHAR_VECTOR xlabel, PLFLT xtick, PLINT nxsub,
          PLCHAR_VECTOR yopt, PLCHAR_VECTOR ylabel, PLFLT ytick, PLINT nysub,
          PLCHAR_VECTOR zopt, PLCHAR_VECTOR zlabel, PLFLT ztick, PLINT nzsub );

// Calculate world coordinates and subpage from relative device coordinates.

PLDLLIMPEXP void
c_plcalc_world( PLFLT rx, PLFLT ry, PLFLT_NC_SCALAR wx, PLFLT_NC_SCALAR wy, PLINT_NC_SCALAR window );

// Clear current subpage.

PLDLLIMPEXP void
c_plclear( void );

// Set color, map 0.  Argument is integer between 0 and 15.

PLDLLIMPEXP void
c_plcol0( PLINT icol0 );

// Set color, map 1.  Argument is a float between 0. and 1.

PLDLLIMPEXP void
c_plcol1( PLFLT col1 );

//// Configure transformation between continuous and broken-down time (and
//// vice versa) for current stream.
//PLDLLIMPEXP void
//c_plconfigtime( PLFLT scale, PLFLT offset1, PLFLT offset2, PLINT ccontrol, PLBOOL ifbtime_offset, PLINT year, PLINT month, PLINT day, PLINT hour, PLINT min, PLFLT sec );

// Draws a contour plot from data in f(nx,ny).  Is just a front-end to
// plfcont, with a particular choice for f2eval and f2eval_data.
//

PLDLLIMPEXP void
c_plcont( PLFLT_MATRIX f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
          PLINT ky, PLINT ly, PLFLT_VECTOR clevel, PLINT nlevel,
          PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Draws a contour plot using the function evaluator f2eval and data stored
// by way of the f2eval_data pointer.  This allows arbitrary organizations
// of 2d array data to be used.
//

PLDLLIMPEXP void
plfcont( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
         PLINT nx, PLINT ny, PLINT kx, PLINT lx,
         PLINT ky, PLINT ly, PLFLT_VECTOR clevel, PLINT nlevel,
         PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Copies state parameters from the reference stream to the current stream.

PLDLLIMPEXP void
c_plcpstrm( PLINT iplsr, PLBOOL flags );

//// Calculate continuous time from broken-down time for current stream.
//PLDLLIMPEXP void
//c_plctime( PLINT year, PLINT month, PLINT day, PLINT hour, PLINT min, PLFLT sec, PLFLT_NC_SCALAR ctime );

// Converts input values from relative device coordinates to relative plot
// coordinates.

PLDLLIMPEXP void
pldid2pc( PLFLT_NC_SCALAR xmin, PLFLT_NC_SCALAR ymin, PLFLT_NC_SCALAR xmax, PLFLT_NC_SCALAR ymax );

// Converts input values from relative plot coordinates to relative
// device coordinates.

PLDLLIMPEXP void
pldip2dc( PLFLT_NC_SCALAR xmin, PLFLT_NC_SCALAR ymin, PLFLT_NC_SCALAR xmax, PLFLT_NC_SCALAR ymax );

// End a plotting session for all open streams.

PLDLLIMPEXP void
c_plend( void );

// End a plotting session for the current stream only.

PLDLLIMPEXP void
c_plend1( void );

// Simple interface for defining viewport and window.

PLDLLIMPEXP void
c_plenv( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
         PLINT just, PLINT axis );


// similar to plenv() above, but in multiplot mode does not advance the subpage,
// instead the current subpage is cleared

PLDLLIMPEXP void
c_plenv0( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
          PLINT just, PLINT axis );

// End current page.  Should only be used with plbop().

PLDLLIMPEXP void
c_pleop( void );

// Plot horizontal error bars (xmin(i),y(i)) to (xmax(i),y(i))

PLDLLIMPEXP void
c_plerrx( PLINT n, PLFLT_VECTOR xmin, PLFLT_VECTOR xmax, PLFLT_VECTOR y );

// Plot vertical error bars (x,ymin(i)) to (x(i),ymax(i))

PLDLLIMPEXP void
c_plerry( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR ymin, PLFLT_VECTOR ymax );

// Advance to the next family file on the next new page

PLDLLIMPEXP void
c_plfamadv( void );

// Pattern fills the polygon bounded by the input points.

PLDLLIMPEXP void
c_plfill( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y );

// Pattern fills the 3d polygon bounded by the input points.

PLDLLIMPEXP void
c_plfill3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z );

// Flushes the output stream.  Use sparingly, if at all.

PLDLLIMPEXP void
c_plflush( void );

// Sets the global font flag to 'ifont'.

PLDLLIMPEXP void
c_plfont( PLINT ifont );

// Load specified font set.

PLDLLIMPEXP void
c_plfontld( PLINT fnt );

// Get character default height and current (scaled) height

PLDLLIMPEXP void
c_plgchr( PLFLT_NC_SCALAR p_def, PLFLT_NC_SCALAR p_ht );

// Get the color map 1 range used in continuous plots

PLDLLIMPEXP void
c_plgcmap1_range( PLFLT_NC_SCALAR min_color, PLFLT_NC_SCALAR max_color );

// Returns 8 bit RGB values for given color from color map 0

PLDLLIMPEXP void
c_plgcol0( PLINT icol0, PLINT_NC_SCALAR r, PLINT_NC_SCALAR g, PLINT_NC_SCALAR b );

// Returns 8 bit RGB values for given color from color map 0 and alpha value

PLDLLIMPEXP void
c_plgcol0a( PLINT icol0, PLINT_NC_SCALAR r, PLINT_NC_SCALAR g, PLINT_NC_SCALAR b, PLFLT_NC_SCALAR alpha );

// Returns the background color by 8 bit RGB value

PLDLLIMPEXP void
c_plgcolbg( PLINT_NC_SCALAR r, PLINT_NC_SCALAR g, PLINT_NC_SCALAR b );

// Returns the background color by 8 bit RGB value and alpha value

PLDLLIMPEXP void
c_plgcolbga( PLINT_NC_SCALAR r, PLINT_NC_SCALAR g, PLINT_NC_SCALAR b, PLFLT_NC_SCALAR alpha );

// Returns the current compression setting

PLDLLIMPEXP void
c_plgcompression( PLINT_NC_SCALAR compression );

// Get the current device (keyword) name

PLDLLIMPEXP void
c_plgdev( PLCHAR_NC_VECTOR p_dev );

// Retrieve current window into device space

PLDLLIMPEXP void
c_plgdidev( PLFLT_NC_SCALAR p_mar, PLFLT_NC_SCALAR p_aspect, PLFLT_NC_SCALAR p_jx, PLFLT_NC_SCALAR p_jy );

// Get plot orientation

PLDLLIMPEXP void
c_plgdiori( PLFLT_NC_SCALAR p_rot );

// Retrieve current window into plot space

PLDLLIMPEXP void
c_plgdiplt( PLFLT_NC_SCALAR p_xmin, PLFLT_NC_SCALAR p_ymin, PLFLT_NC_SCALAR p_xmax, PLFLT_NC_SCALAR p_ymax );

// Get the drawing mode

PLDLLIMPEXP PLINT
c_plgdrawmode( void );

// Get FCI (font characterization integer)

PLDLLIMPEXP void
c_plgfci( PLUNICODE_NC_SCALAR p_fci );

// Get family file parameters

PLDLLIMPEXP void
c_plgfam( PLINT_NC_SCALAR p_fam, PLINT_NC_SCALAR p_num, PLINT_NC_SCALAR p_bmax );

// Get the (current) output file name.  Must be preallocated to >80 bytes

PLDLLIMPEXP void
c_plgfnam( PLCHAR_NC_VECTOR fnam );

// Get the current font family, style and weight

PLDLLIMPEXP void
c_plgfont( PLINT_NC_SCALAR p_family, PLINT_NC_SCALAR p_style, PLINT_NC_SCALAR p_weight );

// Get the (current) run level.

PLDLLIMPEXP void
c_plglevel( PLINT_NC_SCALAR p_level );

// Get output device parameters.

PLDLLIMPEXP void
c_plgpage( PLFLT_NC_SCALAR p_xp, PLFLT_NC_SCALAR p_yp,
           PLINT_NC_SCALAR p_xleng, PLINT_NC_SCALAR p_yleng, PLINT_NC_SCALAR p_xoff, PLINT_NC_SCALAR p_yoff );

// Switches to graphics screen.

PLDLLIMPEXP void
c_plgra( void );

// Draw gradient in polygon.

PLDLLIMPEXP void
c_plgradient( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT angle );

// grid irregularly sampled data

PLDLLIMPEXP void
c_plgriddata( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLINT npts,
              PLFLT_VECTOR xg, PLINT nptsx, PLFLT_VECTOR yg, PLINT nptsy,
              PLFLT_NC_MATRIX zg, PLINT type, PLFLT data );

PLDLLIMPEXP void
plfgriddata( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLINT npts,
             PLFLT_VECTOR xg, PLINT nptsx, PLFLT_VECTOR yg, PLINT nptsy,
             PLF2OPS zops, PLPointer zgp, PLINT type, PLFLT data );

// type of gridding algorithm for plgriddata()

#define GRID_CSA       1 // Bivariate Cubic Spline approximation
#define GRID_DTLI      2 // Delaunay Triangulation Linear Interpolation
#define GRID_NNI       3 // Natural Neighbors Interpolation
#define GRID_NNIDW     4 // Nearest Neighbors Inverse Distance Weighted
#define GRID_NNLI      5 // Nearest Neighbors Linear Interpolation
#define GRID_NNAIDW    6 // Nearest Neighbors Around Inverse Distance Weighted

// Get subpage boundaries in absolute coordinates

PLDLLIMPEXP void
c_plgspa( PLFLT_NC_SCALAR xmin, PLFLT_NC_SCALAR xmax, PLFLT_NC_SCALAR ymin, PLFLT_NC_SCALAR ymax );

// Get current stream number.

PLDLLIMPEXP void
c_plgstrm( PLINT_NC_SCALAR p_strm );

// Get the current library version number

PLDLLIMPEXP void
c_plgver( PLCHAR_NC_VECTOR p_ver );

// Get viewport boundaries in normalized device coordinates

PLDLLIMPEXP void
c_plgvpd( PLFLT_NC_SCALAR p_xmin, PLFLT_NC_SCALAR p_xmax, PLFLT_NC_SCALAR p_ymin, PLFLT_NC_SCALAR p_ymax );

// Get viewport boundaries in world coordinates

PLDLLIMPEXP void
c_plgvpw( PLFLT_NC_SCALAR p_xmin, PLFLT_NC_SCALAR p_xmax, PLFLT_NC_SCALAR p_ymin, PLFLT_NC_SCALAR p_ymax );

// Get x axis labeling parameters

PLDLLIMPEXP void
c_plgxax( PLINT_NC_SCALAR p_digmax, PLINT_NC_SCALAR p_digits );

// Get y axis labeling parameters

PLDLLIMPEXP void
c_plgyax( PLINT_NC_SCALAR p_digmax, PLINT_NC_SCALAR p_digits );

// Get z axis labeling parameters

PLDLLIMPEXP void
c_plgzax( PLINT_NC_SCALAR p_digmax, PLINT_NC_SCALAR p_digits );

// Draws a histogram of n values of a variable in array data[0..n-1]

// Flags for plhist() - opt argument; note: some flags are passed to
// plbin() for the actual plotting
#define PL_HIST_DEFAULT            0x00
#define PL_HIST_NOSCALING          0x01
#define PL_HIST_IGNORE_OUTLIERS    0x02
#define PL_HIST_NOEXPAND           0x08
#define PL_HIST_NOEMPTY            0x10

PLDLLIMPEXP void
c_plhist( PLINT n, PLFLT_VECTOR data, PLFLT datmin, PLFLT datmax,
          PLINT nbin, PLINT opt );

// Functions for converting between HLS and RGB color space

PLDLLIMPEXP void
c_plhlsrgb( PLFLT h, PLFLT l, PLFLT s, PLFLT_NC_SCALAR p_r, PLFLT_NC_SCALAR p_g, PLFLT_NC_SCALAR p_b );

// Initializes PLplot, using preset or default options

PLDLLIMPEXP void
c_plinit( void );

// Draws a line segment from (x1, y1) to (x2, y2).

PLDLLIMPEXP void
c_pljoin( PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 );

// Simple routine for labelling graphs.

PLDLLIMPEXP void
c_pllab( PLCHAR_VECTOR xlabel, PLCHAR_VECTOR ylabel, PLCHAR_VECTOR tlabel );

//flags used for position argument of both pllegend and plcolorbar
#define PL_POSITION_NULL             0x0
#define PL_POSITION_LEFT             0x1
#define PL_POSITION_RIGHT            0x2
#define PL_POSITION_TOP              0x4
#define PL_POSITION_BOTTOM           0x8
#define PL_POSITION_INSIDE           0x10
#define PL_POSITION_OUTSIDE          0x20
#define PL_POSITION_VIEWPORT         0x40
#define PL_POSITION_SUBPAGE          0x80

// Flags for pllegend.
#define PL_LEGEND_NULL               0x0
#define PL_LEGEND_NONE               0x1
#define PL_LEGEND_COLOR_BOX          0x2
#define PL_LEGEND_LINE               0x4
#define PL_LEGEND_SYMBOL             0x8
#define PL_LEGEND_TEXT_LEFT          0x10
#define PL_LEGEND_BACKGROUND         0x20
#define PL_LEGEND_BOUNDING_BOX       0x40
#define PL_LEGEND_ROW_MAJOR          0x80

// Flags for plcolorbar
#define PL_COLORBAR_NULL             0x0
#define PL_COLORBAR_LABEL_LEFT       0x1
#define PL_COLORBAR_LABEL_RIGHT      0x2
#define PL_COLORBAR_LABEL_TOP        0x4
#define PL_COLORBAR_LABEL_BOTTOM     0x8
#define PL_COLORBAR_IMAGE            0x10
#define PL_COLORBAR_SHADE            0x20
#define PL_COLORBAR_GRADIENT         0x40
#define PL_COLORBAR_CAP_NONE         0x80
#define PL_COLORBAR_CAP_LOW          0x100
#define PL_COLORBAR_CAP_HIGH         0x200
#define PL_COLORBAR_SHADE_LABEL      0x400
#define PL_COLORBAR_ORIENT_RIGHT     0x800
#define PL_COLORBAR_ORIENT_TOP       0x1000
#define PL_COLORBAR_ORIENT_LEFT      0x2000
#define PL_COLORBAR_ORIENT_BOTTOM    0x4000
#define PL_COLORBAR_BACKGROUND       0x8000
#define PL_COLORBAR_BOUNDING_BOX     0x10000

// Flags for drawing mode
#define PL_DRAWMODE_UNKNOWN          0x0
#define PL_DRAWMODE_DEFAULT          0x1
#define PL_DRAWMODE_REPLACE          0x2
#define PL_DRAWMODE_XOR              0x4

// Routine for drawing discrete line, symbol, or cmap0 legends
PLDLLIMPEXP void
c_pllegend( PLFLT_NC_SCALAR p_legend_width, PLFLT_NC_SCALAR p_legend_height,
            PLINT opt, PLINT position, PLFLT x, PLFLT y, PLFLT plot_width,
            PLINT bg_color, PLINT bb_color, PLINT bb_style,
            PLINT nrow, PLINT ncolumn,
            PLINT nlegend, PLINT_VECTOR opt_array,
            PLFLT text_offset, PLFLT text_scale, PLFLT text_spacing,
            PLFLT text_justification,
            PLINT_VECTOR text_colors, PLCHAR_MATRIX text,
            PLINT_VECTOR box_colors, PLINT_VECTOR box_patterns,
            PLFLT_VECTOR box_scales, PLFLT_VECTOR box_line_widths,
            PLINT_VECTOR line_colors, PLINT_VECTOR line_styles,
            PLFLT_VECTOR line_widths,
            PLINT_VECTOR symbol_colors, PLFLT_VECTOR symbol_scales,
            PLINT_VECTOR symbol_numbers, PLCHAR_MATRIX symbols );

// Routine for drawing continuous colour legends
PLDLLIMPEXP void
c_plcolorbar( PLFLT_NC_SCALAR p_colorbar_width, PLFLT_NC_SCALAR p_colorbar_height,
              PLINT opt, PLINT position, PLFLT x, PLFLT y,
              PLFLT x_length, PLFLT y_length,
              PLINT bg_color, PLINT bb_color, PLINT bb_style,
              PLFLT low_cap_color, PLFLT high_cap_color,
              PLINT cont_color, PLFLT cont_width,
              PLINT n_labels, PLINT_VECTOR label_opts, PLCHAR_MATRIX labels,
              PLINT n_axes, PLCHAR_MATRIX axis_opts,
              PLFLT_VECTOR ticks, PLINT_VECTOR sub_ticks,
              PLINT_VECTOR n_values, PLFLT_MATRIX values );

// Sets position of the light source
PLDLLIMPEXP void
c_pllightsource( PLFLT x, PLFLT y, PLFLT z );

// Draws line segments connecting a series of points.

PLDLLIMPEXP void
c_plline( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y );

// Draws a line in 3 space.

PLDLLIMPEXP void
c_plline3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z );

// Set line style.

PLDLLIMPEXP void
c_pllsty( PLINT lin );

// Plot continental outline in world coordinates

PLDLLIMPEXP void
c_plmap( PLMAPFORM_callback mapform, PLCHAR_VECTOR name,
         PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy );

// Plot map outlines

PLDLLIMPEXP void
c_plmapline( PLMAPFORM_callback mapform, PLCHAR_VECTOR name,
             PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
             PLINT_VECTOR plotentries, PLINT nplotentries );

// Plot map points

PLDLLIMPEXP void
c_plmapstring( PLMAPFORM_callback mapform,
               PLCHAR_VECTOR name, PLCHAR_VECTOR string,
               PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
               PLINT_VECTOR plotentries, PLINT nplotentries );

// Plot map text

PLDLLIMPEXP void
c_plmaptex( PLMAPFORM_callback mapform,
            PLCHAR_VECTOR name, PLFLT dx, PLFLT dy, PLFLT just, PLCHAR_VECTOR text,
            PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
            PLINT plotentry );

// Plot map fills

PLDLLIMPEXP void
c_plmapfill( PLMAPFORM_callback mapform,
             PLCHAR_VECTOR name, PLFLT minx, PLFLT maxx, PLFLT miny, PLFLT maxy,
             PLINT_VECTOR plotentries, PLINT nplotentries );

// Plot the latitudes and longitudes on the background.

PLDLLIMPEXP void
c_plmeridians( PLMAPFORM_callback mapform,
               PLFLT dlong, PLFLT dlat,
               PLFLT minlong, PLFLT maxlong, PLFLT minlat, PLFLT maxlat );

// Plots a mesh representation of the function z[x][y].

PLDLLIMPEXP void
c_plmesh( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny, PLINT opt );

// Like plmesh, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfmesh( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
         PLINT nx, PLINT ny, PLINT opt );

// Plots a mesh representation of the function z[x][y] with contour

PLDLLIMPEXP void
c_plmeshc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny, PLINT opt,
           PLFLT_VECTOR clevel, PLINT nlevel );

// Like plmeshc, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfmeshc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
          PLINT nx, PLINT ny, PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel );

// Creates a new stream and makes it the default.

PLDLLIMPEXP void
c_plmkstrm( PLINT_NC_SCALAR p_strm );

// Prints out "text" at specified position relative to viewport

PLDLLIMPEXP void
c_plmtex( PLCHAR_VECTOR side, PLFLT disp, PLFLT pos, PLFLT just,
          PLCHAR_VECTOR text );

// Prints out "text" at specified position relative to viewport (3D)

PLDLLIMPEXP void
c_plmtex3( PLCHAR_VECTOR side, PLFLT disp, PLFLT pos, PLFLT just,
           PLCHAR_VECTOR text );

// Plots a 3-d representation of the function z[x][y].

PLDLLIMPEXP void
c_plot3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z,
          PLINT nx, PLINT ny, PLINT opt, PLBOOL side );

// Like plot3d, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfplot3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
           PLINT nx, PLINT ny, PLINT opt, PLBOOL side );

// Plots a 3-d representation of the function z[x][y] with contour.

PLDLLIMPEXP void
c_plot3dc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z,
           PLINT nx, PLINT ny, PLINT opt,
           PLFLT_VECTOR clevel, PLINT nlevel );

// Like plot3dc, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfplot3dc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
            PLINT nx, PLINT ny, PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel );

// Plots a 3-d representation of the function z[x][y] with contour and
// y index limits.

PLDLLIMPEXP void
c_plot3dcl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z,
            PLINT nx, PLINT ny, PLINT opt,
            PLFLT_VECTOR clevel, PLINT nlevel,
            PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax );

// Like plot3dcl, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfplot3dcl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
             PLINT nx, PLINT ny, PLINT opt,
             PLFLT_VECTOR clevel, PLINT nlevel,
             PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax );

//
// definitions for the opt argument in plot3dc() and plsurf3d()
//
// DRAW_LINEX *must* be 1 and DRAW_LINEY *must* be 2, because of legacy code!
//

#define DRAW_LINEX     0x001  // draw lines parallel to the X axis
#define DRAW_LINEY     0x002  // draw lines parallel to the Y axis
#define DRAW_LINEXY    0x003  // draw lines parallel to both the X and Y axis
#define MAG_COLOR      0x004  // draw the mesh with a color dependent of the magnitude
#define BASE_CONT      0x008  // draw contour plot at bottom xy plane
#define TOP_CONT       0x010  // draw contour plot at top xy plane
#define SURF_CONT      0x020  // draw contour plot at surface
#define DRAW_SIDES     0x040  // draw sides
#define FACETED        0x080  // draw outline for each square that makes up the surface
#define MESH           0x100  // draw mesh

//
//  valid options for plot3dc():
//
//  DRAW_SIDES, BASE_CONT, TOP_CONT (not yet),
//  MAG_COLOR, DRAW_LINEX, DRAW_LINEY, DRAW_LINEXY.
//
//  valid options for plsurf3d():
//
//  MAG_COLOR, BASE_CONT, SURF_CONT, FACETED, DRAW_SIDES.
//

// Set fill pattern directly.

PLDLLIMPEXP void
c_plpat( PLINT nlin, PLINT_VECTOR inc, PLINT_VECTOR del );

// Draw a line connecting two points, accounting for coordinate transforms

PLDLLIMPEXP void
c_plpath( PLINT n, PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 );

// Plots array y against x for n points using ASCII code "code".

PLDLLIMPEXP void
c_plpoin( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT code );

// Draws a series of points in 3 space.

PLDLLIMPEXP void
c_plpoin3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLINT code );

// Draws a polygon in 3 space.

PLDLLIMPEXP void
c_plpoly3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLBOOL_VECTOR draw, PLBOOL ifcc );

// Set the floating point precision (in number of places) in numeric labels.

PLDLLIMPEXP void
c_plprec( PLINT setp, PLINT prec );

// Set fill pattern, using one of the predefined patterns.

PLDLLIMPEXP void
c_plpsty( PLINT patt );

// Prints out "text" at world cooordinate (x,y).

PLDLLIMPEXP void
c_plptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just, PLCHAR_VECTOR text );

// Prints out "text" at world cooordinate (x,y,z).

PLDLLIMPEXP void
c_plptex3( PLFLT wx, PLFLT wy, PLFLT wz, PLFLT dx, PLFLT dy, PLFLT dz,
           PLFLT sx, PLFLT sy, PLFLT sz, PLFLT just, PLCHAR_VECTOR text );

// Random number generator based on Mersenne Twister.
// Obtain real random number in range [0,1].

PLDLLIMPEXP PLFLT
c_plrandd( void );

// Replays contents of plot buffer to current device/file.

PLDLLIMPEXP void
c_plreplot( void );

// Functions for converting between HLS and RGB color space

PLDLLIMPEXP void
c_plrgbhls( PLFLT r, PLFLT g, PLFLT b, PLFLT_NC_SCALAR p_h, PLFLT_NC_SCALAR p_l, PLFLT_NC_SCALAR p_s );

// Set character height.

PLDLLIMPEXP void
c_plschr( PLFLT def, PLFLT scale );

// Set color map 0 colors by 8 bit RGB values

PLDLLIMPEXP void
c_plscmap0( PLINT_VECTOR r, PLINT_VECTOR g, PLINT_VECTOR b, PLINT ncol0 );

// Set color map 0 colors by 8 bit RGB values and alpha values

PLDLLIMPEXP void
c_plscmap0a( PLINT_VECTOR r, PLINT_VECTOR g, PLINT_VECTOR b, PLFLT_VECTOR alpha, PLINT ncol0 );

// Set number of colors in cmap 0

PLDLLIMPEXP void
c_plscmap0n( PLINT ncol0 );

// Set color map 1 colors by 8 bit RGB values

PLDLLIMPEXP void
c_plscmap1( PLINT_VECTOR r, PLINT_VECTOR g, PLINT_VECTOR b, PLINT ncol1 );

// Set color map 1 colors by 8 bit RGB and alpha values

PLDLLIMPEXP void
c_plscmap1a( PLINT_VECTOR r, PLINT_VECTOR g, PLINT_VECTOR b, PLFLT_VECTOR alpha, PLINT ncol1 );

// Set color map 1 colors using a piece-wise linear relationship between
// intensity [0,1] (cmap 1 index) and position in HLS or RGB color space.

PLDLLIMPEXP void
c_plscmap1l( PLBOOL itype, PLINT npts, PLFLT_VECTOR intensity,
             PLFLT_VECTOR coord1, PLFLT_VECTOR coord2, PLFLT_VECTOR coord3, PLBOOL_VECTOR alt_hue_path );

// Set color map 1 colors using a piece-wise linear relationship between
// intensity [0,1] (cmap 1 index) and position in HLS or RGB color space.
// Will also linear interpolate alpha values.

PLDLLIMPEXP void
c_plscmap1la( PLBOOL itype, PLINT npts, PLFLT_VECTOR intensity,
              PLFLT_VECTOR coord1, PLFLT_VECTOR coord2, PLFLT_VECTOR coord3, PLFLT_VECTOR alpha, PLBOOL_VECTOR alt_hue_path );

// Set number of colors in cmap 1

PLDLLIMPEXP void
c_plscmap1n( PLINT ncol1 );

// Set the color map 1 range used in continuous plots

PLDLLIMPEXP void
c_plscmap1_range( PLFLT min_color, PLFLT max_color );

// Set a given color from color map 0 by 8 bit RGB value

PLDLLIMPEXP void
c_plscol0( PLINT icol0, PLINT r, PLINT g, PLINT b );

// Set a given color from color map 0 by 8 bit RGB value

PLDLLIMPEXP void
c_plscol0a( PLINT icol0, PLINT r, PLINT g, PLINT b, PLFLT alpha );

// Set the background color by 8 bit RGB value

PLDLLIMPEXP void
c_plscolbg( PLINT r, PLINT g, PLINT b );

// Set the background color by 8 bit RGB value and alpha value

PLDLLIMPEXP void
c_plscolbga( PLINT r, PLINT g, PLINT b, PLFLT alpha );

// Used to globally turn color output on/off

PLDLLIMPEXP void
c_plscolor( PLINT color );

// Set the compression level

PLDLLIMPEXP void
c_plscompression( PLINT compression );

// Set the device (keyword) name

PLDLLIMPEXP void
c_plsdev( PLCHAR_VECTOR devname );

// Set window into device space using margin, aspect ratio, and
// justification

PLDLLIMPEXP void
c_plsdidev( PLFLT mar, PLFLT aspect, PLFLT jx, PLFLT jy );

// Set up transformation from metafile coordinates.

PLDLLIMPEXP void
c_plsdimap( PLINT dimxmin, PLINT dimxmax, PLINT dimymin, PLINT dimymax,
            PLFLT dimxpmm, PLFLT dimypmm );

// Set plot orientation, specifying rotation in units of pi/2.

PLDLLIMPEXP void
c_plsdiori( PLFLT rot );

// Set window into plot space

PLDLLIMPEXP void
c_plsdiplt( PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax );

// Set window into plot space incrementally (zoom)

PLDLLIMPEXP void
c_plsdiplz( PLFLT xmin, PLFLT ymin, PLFLT xmax, PLFLT ymax );

// Set the drawing mode
PLDLLIMPEXP void
c_plsdrawmode( PLINT mode );

// Set seed for internal random number generator

PLDLLIMPEXP void
c_plseed( unsigned int seed );

// Set the escape character for text strings.

PLDLLIMPEXP void
c_plsesc( char esc );

// Set family file parameters

PLDLLIMPEXP void
c_plsfam( PLINT fam, PLINT num, PLINT bmax );

// Set FCI (font characterization integer)

PLDLLIMPEXP void
c_plsfci( PLUNICODE fci );

// Set the output file name.

PLDLLIMPEXP void
c_plsfnam( PLCHAR_VECTOR fnam );

// Set the current font family, style and weight

PLDLLIMPEXP void
c_plsfont( PLINT family, PLINT style, PLINT weight );

// Shade region.

PLDLLIMPEXP void
c_plshade( PLFLT_MATRIX a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
           PLFLT shade_min, PLFLT shade_max,
           PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
           PLINT min_color, PLFLT min_width,
           PLINT max_color, PLFLT max_width,
           PLFILL_callback fill, PLBOOL rectangular,
           PLTRANSFORM_callback pltr, PLPointer pltr_data );

#ifdef PL_DEPRECATED
PLDLLIMPEXP void
c_plshade1( PLFLT_FE_POINTER a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
            PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
            PLFLT shade_min, PLFLT shade_max,
            PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
            PLINT min_color, PLFLT min_width,
            PLINT max_color, PLFLT max_width,
            PLFILL_callback fill, PLBOOL rectangular,
            PLTRANSFORM_callback pltr, PLPointer pltr_data );
#endif // PL_DEPRECATED

PLDLLIMPEXP void
c_plshades( PLFLT_MATRIX a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
            PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
            PLFLT_VECTOR clevel, PLINT nlevel, PLFLT fill_width,
            PLINT cont_color, PLFLT cont_width,
            PLFILL_callback fill, PLBOOL rectangular,
            PLTRANSFORM_callback pltr, PLPointer pltr_data );

PLDLLIMPEXP void
plfshades( PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
           PLDEFINED_callback defined,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
           PLFLT_VECTOR clevel, PLINT nlevel, PLFLT fill_width,
           PLINT cont_color, PLFLT cont_width,
           PLFILL_callback fill, PLINT rectangular,
           PLTRANSFORM_callback pltr, PLPointer pltr_data );

PLDLLIMPEXP void
plfshade( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
          PLF2EVAL_callback c2eval, PLPointer c2eval_data,
          PLINT nx, PLINT ny,
          PLFLT left, PLFLT right, PLFLT bottom, PLFLT top,
          PLFLT shade_min, PLFLT shade_max,
          PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
          PLINT min_color, PLFLT min_width,
          PLINT max_color, PLFLT max_width,
          PLFILL_callback fill, PLBOOL rectangular,
          PLTRANSFORM_callback pltr, PLPointer pltr_data );

PLDLLIMPEXP void
plfshade1( PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
           PLDEFINED_callback defined,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
           PLFLT shade_min, PLFLT shade_max,
           PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
           PLINT min_color, PLFLT min_width,
           PLINT max_color, PLFLT max_width,
           PLFILL_callback fill, PLINT rectangular,
           PLTRANSFORM_callback pltr, PLPointer pltr_data );

// Setup a user-provided custom labeling function

PLDLLIMPEXP void
c_plslabelfunc( PLLABEL_FUNC_callback label_func, PLPointer label_data );

// Set up lengths of major tick marks.

PLDLLIMPEXP void
c_plsmaj( PLFLT def, PLFLT scale );

// Set the RGB memory area to be plotted (with the 'mem' or 'memcairo' drivers)

PLDLLIMPEXP void
c_plsmem( PLINT maxx, PLINT maxy, PLPointer plotmem );

// Set the RGBA memory area to be plotted (with the 'memcairo' driver)

PLDLLIMPEXP void
c_plsmema( PLINT maxx, PLINT maxy, PLPointer plotmem );

// Set up lengths of minor tick marks.

PLDLLIMPEXP void
c_plsmin( PLFLT def, PLFLT scale );

// Set orientation.  Must be done before calling plinit.

PLDLLIMPEXP void
c_plsori( PLINT ori );

// Set output device parameters.  Usually ignored by the driver.

PLDLLIMPEXP void
c_plspage( PLFLT xp, PLFLT yp, PLINT xleng, PLINT yleng,
           PLINT xoff, PLINT yoff );

// Set the colors for color table 0 from a cmap0 file

PLDLLIMPEXP void
c_plspal0( PLCHAR_VECTOR filename );

// Set the colors for color table 1 from a cmap1 file

PLDLLIMPEXP void
c_plspal1( PLCHAR_VECTOR filename, PLBOOL interpolate );

// Set the pause (on end-of-page) status

PLDLLIMPEXP void
c_plspause( PLBOOL pause );

// Set stream number.

PLDLLIMPEXP void
c_plsstrm( PLINT strm );

// Set the number of subwindows in x and y

PLDLLIMPEXP void
c_plssub( PLINT nx, PLINT ny );

// Set symbol height.

PLDLLIMPEXP void
c_plssym( PLFLT def, PLFLT scale );

// Initialize PLplot, passing in the windows/page settings.

PLDLLIMPEXP void
c_plstar( PLINT nx, PLINT ny );

// Initialize PLplot, passing the device name and windows/page settings.

PLDLLIMPEXP void
c_plstart( PLCHAR_VECTOR devname, PLINT nx, PLINT ny );

// Set the coordinate transform

PLDLLIMPEXP void
c_plstransform( PLTRANSFORM_callback coordinate_transform, PLPointer coordinate_transform_data );

// Prints out the same string repeatedly at the n points in world
// coordinates given by the x and y arrays.  Supersedes plpoin and
// plsymbol for the case where text refers to a unicode glyph either
// directly as UTF-8 or indirectly via the standard text escape
// sequences allowed for PLplot input strings.

PLDLLIMPEXP void
c_plstring( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLCHAR_VECTOR string );

// Prints out the same string repeatedly at the n points in world
// coordinates given by the x, y, and z arrays.  Supersedes plpoin3
// for the case where text refers to a unicode glyph either directly
// as UTF-8 or indirectly via the standard text escape sequences
// allowed for PLplot input strings.

PLDLLIMPEXP void
c_plstring3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLCHAR_VECTOR string );

// Add a point to a stripchart.

PLDLLIMPEXP void
c_plstripa( PLINT id, PLINT pen, PLFLT x, PLFLT y );

// Create 1d stripchart

PLDLLIMPEXP void
c_plstripc( PLINT_NC_SCALAR id, PLCHAR_VECTOR xspec, PLCHAR_VECTOR yspec,
            PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
            PLFLT xlpos, PLFLT ylpos,
            PLBOOL y_ascl, PLBOOL acc,
            PLINT colbox, PLINT collab,
            PLINT_VECTOR colline, PLINT_VECTOR styline, PLCHAR_MATRIX legline,
            PLCHAR_VECTOR labx, PLCHAR_VECTOR laby, PLCHAR_VECTOR labtop );

// Deletes and releases memory used by a stripchart.

PLDLLIMPEXP void
c_plstripd( PLINT id );

// plots a 2d image (or a matrix too large for plshade() )

PLDLLIMPEXP void
c_plimagefr( PLFLT_MATRIX idata, PLINT nx, PLINT ny,
             PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
             PLFLT valuemin, PLFLT valuemax,
             PLTRANSFORM_callback pltr, PLPointer pltr_data );

//
// Like plimagefr, but uses an evaluator function to access image data from
// idatap.  getminmax is only used if zmin == zmax.
//

PLDLLIMPEXP void
plfimagefr( PLF2OPS idataops, PLPointer idatap, PLINT nx, PLINT ny,
            PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
            PLFLT valuemin, PLFLT valuemax,
            PLTRANSFORM_callback pltr, PLPointer pltr_data );

// plots a 2d image (or a matrix too large for plshade() ) - colors
// automatically scaled

PLDLLIMPEXP void
c_plimage( PLFLT_MATRIX idata, PLINT nx, PLINT ny,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
           PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax );

//
// Like plimage, but uses an operator functions to access image data from
// idatap.
//

PLDLLIMPEXP void
plfimage( PLF2OPS idataops, PLPointer idatap, PLINT nx, PLINT ny,
          PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
          PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax );

// Set up a new line style

PLDLLIMPEXP void
c_plstyl( PLINT nms, PLINT_VECTOR mark, PLINT_VECTOR space );

// Plots the 3d surface representation of the function z[x][y].

PLDLLIMPEXP void
c_plsurf3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny,
            PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel );

// Like plsurf3d, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfsurf3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
           PLINT nx, PLINT ny, PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel );

// Plots the 3d surface representation of the function z[x][y] with y
// index limits.

PLDLLIMPEXP void
c_plsurf3dl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny,
             PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel,
             PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax );

// Like plsurf3dl, but uses an evaluator function to access z data from zp

PLDLLIMPEXP void
plfsurf3dl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
            PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel,
            PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax );

// Set arrow style for vector plots.
PLDLLIMPEXP void
c_plsvect( PLFLT_VECTOR arrowx, PLFLT_VECTOR arrowy, PLINT npts, PLBOOL fill );

// Sets the edges of the viewport to the specified absolute coordinates

PLDLLIMPEXP void
c_plsvpa( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

// Set x axis labeling parameters

PLDLLIMPEXP void
c_plsxax( PLINT digmax, PLINT digits );

// Set inferior X window

PLDLLIMPEXP void
plsxwin( PLINT window_id );

// Set y axis labeling parameters

PLDLLIMPEXP void
c_plsyax( PLINT digmax, PLINT digits );

// Plots array y against x for n points using Hershey symbol "code"

PLDLLIMPEXP void
c_plsym( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT code );

// Set z axis labeling parameters

PLDLLIMPEXP void
c_plszax( PLINT digmax, PLINT digits );

// Switches to text screen.

PLDLLIMPEXP void
c_pltext( void );

// Set the format for date / time labels for current stream.

PLDLLIMPEXP void
c_pltimefmt( PLCHAR_VECTOR fmt );

// Sets the edges of the viewport with the given aspect ratio, leaving
// room for labels.

PLDLLIMPEXP void
c_plvasp( PLFLT aspect );

// Creates the largest viewport of the specified aspect ratio that fits
// within the specified normalized subpage coordinates.

// simple arrow plotter.

PLDLLIMPEXP void
c_plvect( PLFLT_MATRIX u, PLFLT_MATRIX v, PLINT nx, PLINT ny, PLFLT scale,
          PLTRANSFORM_callback pltr, PLPointer pltr_data );

//
// Routine to plot a vector array with arbitrary coordinate
// and vector transformations
//
PLDLLIMPEXP void
plfvect( PLF2EVAL_callback getuv, PLPointer up, PLPointer vp,
         PLINT nx, PLINT ny, PLFLT scale,
         PLTRANSFORM_callback pltr, PLPointer pltr_data );

PLDLLIMPEXP void
c_plvpas( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect );

// Creates a viewport with the specified normalized subpage coordinates.

PLDLLIMPEXP void
c_plvpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

// Defines a "standard" viewport with seven character heights for
// the left margin and four character heights everywhere else.

PLDLLIMPEXP void
c_plvsta( void );

// Set up a window for three-dimensional plotting.

PLDLLIMPEXP void
c_plw3d( PLFLT basex, PLFLT basey, PLFLT height, PLFLT xmin,
         PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin,
         PLFLT zmax, PLFLT alt, PLFLT az );

// Set pen width.

PLDLLIMPEXP void
c_plwidth( PLFLT width );

// Set up world coordinates of the viewport boundaries (2d plots).

PLDLLIMPEXP void
c_plwind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax );

// Set xor mode; mode = 1-enter, 0-leave, status = 0 if not interactive device

PLDLLIMPEXP void
c_plxormod( PLBOOL mode, PLBOOL_NC_SCALAR status );


//--------------------------------------------------------------------------
//		Functions for use from C or C++ only
//--------------------------------------------------------------------------

// Returns a list of file-oriented device names and their menu strings

PLDLLIMPEXP void
plgFileDevs( PLCHAR_VECTOR **p_menustr, PLCHAR_VECTOR **p_devname, int *p_ndev );

// Returns a list of all device names and their menu strings

PLDLLIMPEXP void
plgDevs( PLCHAR_VECTOR **p_menustr, PLCHAR_VECTOR **p_devname, int *p_ndev );

// Set the function pointer for the keyboard event handler

PLDLLIMPEXP void
plsKeyEH( void ( *KeyEH )( PLGraphicsIn *, PLPointer, int * ), PLPointer KeyEH_data );

// Set the function pointer for the (mouse) button event handler

PLDLLIMPEXP void
plsButtonEH( void ( *ButtonEH )( PLGraphicsIn *, PLPointer, int * ),
             PLPointer ButtonEH_data );

// Sets an optional user bop handler

PLDLLIMPEXP void
plsbopH( void ( *handler )( PLPointer, int * ), PLPointer handler_data );

// Sets an optional user eop handler

PLDLLIMPEXP void
plseopH( void ( *handler )( PLPointer, int * ), PLPointer handler_data );

// Set the variables to be used for storing error info

PLDLLIMPEXP void
plsError( PLINT_NC_SCALAR errcode, PLCHAR_NC_VECTOR errmsg );

// Sets an optional user exit handler.

PLDLLIMPEXP void
plsexit( int ( *handler )( PLCHAR_VECTOR ) );

// Sets an optional user abort handler.

PLDLLIMPEXP void
plsabort( void ( *handler )( PLCHAR_VECTOR ) );

// Transformation routines

// Identity transformation.

PLDLLIMPEXP void
pltr0( PLFLT x, PLFLT y, PLFLT_NC_SCALAR tx, PLFLT_NC_SCALAR ty, PLPointer pltr_data );

// Does linear interpolation from singly dimensioned coord arrays.

PLDLLIMPEXP void
pltr1( PLFLT x, PLFLT y, PLFLT_NC_SCALAR tx, PLFLT_NC_SCALAR ty, PLPointer pltr_data );

// Does linear interpolation from doubly dimensioned coord arrays
// (column dominant, as per normal C 2d arrays).

PLDLLIMPEXP void
pltr2( PLFLT x, PLFLT y, PLFLT_NC_SCALAR tx, PLFLT_NC_SCALAR ty, PLPointer pltr_data );

// Just like pltr2() but uses pointer arithmetic to get coordinates from
// 2d grid tables.

PLDLLIMPEXP void
pltr2p( PLFLT x, PLFLT y, PLFLT_NC_SCALAR tx, PLFLT_NC_SCALAR ty, PLPointer pltr_data );

// Does linear interpolation from doubly dimensioned coord arrays
// (row dominant, i.e. Fortran ordering).

PLDLLIMPEXP void
pltr2f( PLFLT x, PLFLT y, PLFLT_NC_SCALAR tx, PLFLT_NC_SCALAR ty, PLPointer pltr_data );

//
// Returns a pointer to a plf2ops_t stucture with pointers to functions for
// accessing 2-D data referenced as (PLFLT **), such as the C variable z
// declared as...
//
//   PLFLT z[nx][ny];
//

PLDLLIMPEXP PLF2OPS
plf2ops_c( void );

//
// Returns a pointer to a plf2ops_t stucture with pointers to functions for accessing 2-D data
// referenced as (PLfGrid2 *), where the PLfGrid2's "f" is treated as type
// (PLFLT **).
//

PLDLLIMPEXP PLF2OPS
plf2ops_grid_c( void );

//
// Returns a pointer to a plf2ops_t stucture with pointers to functions for
// accessing 2-D data stored in (PLfGrid2 *), with the PLfGrid2's "f" field
// treated as type (PLFLT *) pointing to 2-D data stored in row-major order.
// In the context of plotting, it might be easier to think of it as "X-major"
// order.  In this ordering, values for a single X index are stored in
// consecutive memory locations.
//

PLDLLIMPEXP PLF2OPS
plf2ops_grid_row_major( void );

//
// Returns a pointer to a plf2ops_t stucture with pointers to functions for
// accessing 2-D data stored in (PLfGrid2 *), with the PLfGrid2's "f" field
// treated as type (PLFLT *) pointing to 2-D data stored in column-major order.
// In the context of plotting, it might be easier to think of it as "Y-major"
// order.  In this ordering, values for a single Y index are stored in
// consecutive memory locations.
//

PLDLLIMPEXP PLF2OPS
plf2ops_grid_col_major( void );


// Function evaluators (Should these be deprecated in favor of plf2ops?)

//
// Does a lookup from a 2d function array.  plf2eval_data is treated as type
// (PLFLT **) and data for (ix,iy) is returned from...
//
// plf2eval_data[ix][iy];
//

PLDLLIMPEXP PLFLT
plf2eval1( PLINT ix, PLINT iy, PLPointer plf2eval_data );

//
// Does a lookup from a 2d function array.  plf2eval_data is treated as type
// (PLfGrid2 *) and data for (ix,iy) is returned from...
//
// plf2eval_data->f[ix][iy];
//

PLDLLIMPEXP PLFLT
plf2eval2( PLINT ix, PLINT iy, PLPointer plf2eval_data );

//
// Does a lookup from a 2d function array.  plf2eval_data is treated as type
// (PLfGrid *) and data for (ix,iy) is returned from...
//
// plf2eval_data->f[ix * plf2eval_data->ny + iy];
//
// This is commonly called "row-major order", but in the context of plotting,
// it might be easier to think of it as "X-major order".  In this ordering,
// values for a single X index are stored in consecutive memory locations.
// This is also known as C ordering.
//

PLDLLIMPEXP PLFLT
plf2eval( PLINT ix, PLINT iy, PLPointer plf2eval_data );

//
// Does a lookup from a 2d function array.  plf2eval_data is treated as type
// (PLfGrid *) and data for (ix,iy) is returned from...
//
// plf2eval_data->f[ix + iy * plf2eval_data->nx];
//
// This is commonly called "column-major order", but in the context of
// plotting, it might be easier to think of it as "Y-major order".  In this
// ordering, values for a single Y index are stored in consecutive memory
// locations.  This is also known as FORTRAN ordering.
//

PLDLLIMPEXP PLFLT
plf2evalr( PLINT ix, PLINT iy, PLPointer plf2eval_data );

// Command line parsing utilities

// Clear internal option table info structure.

PLDLLIMPEXP void
plClearOpts( void );

// Reset internal option table info structure.

PLDLLIMPEXP void
plResetOpts( void );

// Merge user option table into internal info structure.

PLDLLIMPEXP PLINT
plMergeOpts( PLOptionTable *options, PLCHAR_VECTOR name, PLCHAR_VECTOR *notes );

// Set the strings used in usage and syntax messages.

PLDLLIMPEXP void
plSetUsage( PLCHAR_VECTOR program_string, PLCHAR_VECTOR usage_string );

// Process input strings, treating them as an option and argument pair.
// The first is for the external API, the second the work routine declared
// here for backward compatibilty.

PLDLLIMPEXP PLINT
c_plsetopt( PLCHAR_VECTOR opt, PLCHAR_VECTOR optarg );

// Process options list using current options info.

PLDLLIMPEXP PLINT
c_plparseopts( int *p_argc, PLCHAR_NC_MATRIX argv, PLINT mode );

// Print usage & syntax message.

PLDLLIMPEXP void
plOptUsage( void );

// Miscellaneous

// Set the output file pointer

PLDLLIMPEXP void
plgfile( FILE **p_file );

// Get the output file pointer

PLDLLIMPEXP void
plsfile( FILE *file );

// Get the escape character for text strings.

PLDLLIMPEXP void
plgesc( PLCHAR_NC_SCALAR p_esc );

// Front-end to driver escape function.

PLDLLIMPEXP void
pl_cmd( PLINT op, PLPointer ptr );

// Return full pathname for given file if executable

PLDLLIMPEXP PLINT
plFindName( PLCHAR_NC_VECTOR p );

// Looks for the specified executable file according to usual search path.

PLDLLIMPEXP PLCHAR_NC_VECTOR
plFindCommand( PLCHAR_VECTOR fn );

// Gets search name for file by concatenating the dir, subdir, and file
// name, allocating memory as needed.

PLDLLIMPEXP void
plGetName( PLCHAR_VECTOR dir, PLCHAR_VECTOR subdir, PLCHAR_VECTOR filename, PLCHAR_NC_VECTOR *filespec );

// Prompts human to input an integer in response to given message.

PLDLLIMPEXP PLINT
plGetInt( PLCHAR_VECTOR s );

// Prompts human to input a float in response to given message.

PLDLLIMPEXP PLFLT
plGetFlt( PLCHAR_VECTOR s );

// C, C++ only.  Determine the Iliffe column vector of pointers to PLFLT row
// vectors corresponding to a 2D matrix of PLFLT's that is statically
// allocated.

PLDLLIMPEXP void
plStatic2dGrid( PLFLT_NC_MATRIX zIliffe, PLFLT_VECTOR zStatic, PLINT nx, PLINT ny );

// C, C++ only.  Allocate a block of memory for use as a 2-d grid of PLFLT's organized
// as an Iliffe column vector of pointers to PLFLT row vectors.

PLDLLIMPEXP void
plAlloc2dGrid( PLFLT_NC_MATRIX *f, PLINT nx, PLINT ny );

// Frees a block of memory allocated with plAlloc2dGrid().

PLDLLIMPEXP void
plFree2dGrid( PLFLT_NC_MATRIX f, PLINT nx, PLINT ny );

// Find the maximum and minimum of a 2d matrix allocated with plAllc2dGrid().

PLDLLIMPEXP void
plMinMax2dGrid( PLFLT_MATRIX f, PLINT nx, PLINT ny, PLFLT_NC_SCALAR fmax, PLFLT_NC_SCALAR fmin );

// Wait for graphics input event and translate to world coordinates

PLDLLIMPEXP PLINT
plGetCursor( PLGraphicsIn *gin );

// Translates relative device coordinates to world coordinates.

PLDLLIMPEXP PLINT
plTranslateCursor( PLGraphicsIn *gin );

// Set the pointer to the data used in driver initialisation

// N.B. Currently used only by the wxwidgets device driver and
// associated binding.  This function might be used for other device drivers
// later on whether written in c++ or c.  But this function is not part of the
// common API and should not be propagated to any binding other than
// c++.

PLDLLIMPEXP void
plsdevdata( PLPointer data );

// Set format for date / time labels. - reported from pltime.c
PLDLLIMPEXP void
c_pltimefmt( PLCHAR_VECTOR fmt );

#ifdef __cplusplus
}
#endif
#if 0
#if defined ( __GNUC__ ) && __GNUC__ > 3
  #pragma GCC visibility pop
#endif
#endif

#endif  // __PLPLOT_H__
