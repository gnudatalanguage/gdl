//  Maurice LeBrun			mjl@dino.ph.utexas.edu
//  Institute for Fusion Studies	University of Texas at Austin
//
//  Copyright (C) 1993-2005 Maurice LeBrun
//  Copyright (C) 1995 Rady Shouman
//  Copyright (C) 1998-2000 Geoffrey Furnish
//  Copyright (C) 2000-2019 Alan W. Irwin
//  Copyright (C) 2001 Joao Cardoso
//  Copyright (C) 2004-2011 Andrew Ross
//  Copyright (C) 2004-2005 Rafael Laboissiere
//  Copyright (C) 2007 Andrew Roach
//  Copyright (C) 2008-2009 Werner Smekal
//  Copyright (C) 2009-2011 Hazen Babcock
//  Copyright (C) 2009-2010 Hezekiah M. Carty
//  Copyright (C) 2015 Jim Dishaw
//  Copyright (C) 2017 Phil Rosenberg
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
//  Some parts of this code were derived from "xterm.c" and "ParseCmd.c" of
//  the X-windows Version 11 distribution.  The copyright notice is
//  reproduced here:
//
// Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
// and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
//
//                      All Rights Reserved
//
//  The full permission notice is given in the PLplot documentation.
//
//--------------------------------------------------------------------------
//
//! @file
//!  This file contains routines to extract & process command flags.  The
//!  command flags recognized by PLplot are stored in the "ploption_table"
//!  structure, along with strings giving the syntax, long help message, and
//!  option handler.
//!
//!  The command line parser -- plparseopts() -- removes all recognized flags
//!  (decreasing argc accordingly), so that invalid input may be readily
//!  detected.  It can also be used to process user command line flags.  The
//!  user can merge an option table of type PLOptionTable into the internal
//!  option table info structure using plMergeOpts().  Or, the user can
//!  specify that ONLY the external table(s) be parsed by calling
//!  plClearOpts() before plMergeOpts().
//!
//!  The default action taken by plparseopts() is as follows:
//!      - Returns with an error if an unrecognized option or badly formed
//!        option-value pair are encountered.
//!      - Returns immediately (return code 0) when the first non-option
//!        command line argument is found.
//!      - Returns with the return code of the option handler, if one
//!        was called.
//!      - Deletes command line arguments from argv list as they are found,
//!        and decrements argc accordingly.
//!      - Does not show "invisible" options in usage or help messages.
//!      - Assumes the program name is contained in argv[0].
//!
//!  These behaviors may be controlled through the "mode" argument, which can
//!  have the following bits set:
//!
//!  PL_PARSE_FULL -- Full parsing of command line and all error messages
//!  enabled, including program exit when an error occurs.  Anything on the
//!  command line that isn't recognized as a valid option or option argument
//!  is flagged as an error.
//!
//!  PL_PARSE_QUIET -- Turns off all output except in the case of
//!  errors.
//!
//!  PL_PARSE_NODELETE -- Turns off deletion of processed arguments.
//!
//!  PL_PARSE_SHOWALL -- Show invisible options
//!
//!  PL_PARSE_NOPROGRAM -- Specified if argv[0] is NOT a pointer to the
//!  program name.
//!
//!  PL_PARSE_NODASH -- Set if leading dash is NOT required.
//!
//!  PL_PARSE_SKIP -- Set to quietly skip over any unrecognized args.
//!
//!  Note: if you want to have both your option and a PLplot option of the
//!  same name processed (e.g. the -v option in plrender), do the following:
//!      1. Tag your option with PL_OPT_NODELETE
//!      2. Give it an option handler that uses a return code of 1.
//!      3. Merge your option table in.
//!  By merging your table, your option will be processed before the PLplot
//!  one.  The PL_OPT_NODELETE ensures that the option string is not deleted
//!  from the argv list, and the return code of 1 ensures that the parser
//!  continues looking for it.
//!
//!  See plrender.c for examples of actual usage.
//!

#include "plplotP.h"
#include <ctype.h>
#include <errno.h>

#ifdef HAVE_CRT_EXTERNS_H
//
// This include file has the declaration for _NSGetArgc().  See below.
//
#include <crt_externs.h>
#endif

// Support functions

static int  ParseOpt( int *, char ***, int *, char ***, PLOptionTable * );
static int  ProcessOpt( char *, PLOptionTable *, int *, char ***, int * );
static int  GetOptarg( char **, int *, char ***, int * );
static void Help( void );
static void Syntax( void );

// Option handlers

static int opt_a( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_auto_path( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_bg( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_bufmax( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_bufmax( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_cmap0( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_cmap1( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_db( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_debug( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_dev( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_dev_compression( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_dpi( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_drvopt( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_eofill( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_fam( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_fbeg( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_fflen( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_finc( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_freeaspect( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_fsiz( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_geo( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_h( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_hack( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_jx( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_jy( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_locale( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_mar( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_mfi( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_mfo( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_ncol0( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_ncol1( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_nopixmap( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_np( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_o( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_ori( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_plserver( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_plwindow( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_portrait( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_px( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_py( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_server_name( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_tk_file( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_v( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_verbose( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_width( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );
static int opt_wplt( PLCHAR_VECTOR, PLCHAR_VECTOR, void * );

// Global variables

static PLCHAR_VECTOR program = NULL;
static PLCHAR_VECTOR usage   = NULL;

static int           mode_full;
static int           mode_quiet;
static int           mode_nodelete;
static int           mode_showall;
static int           mode_noprogram;
static int           mode_nodash;
static int           mode_skip;

// Temporary buffer used for parsing

#define OPTMAX    1024
static char opttmp[OPTMAX];

//--------------------------------------------------------------------------
//!
//! PLPLOT options data structure definition.
//!
//! The table is defined as follows
//!
//! typedef struct {
//!     PLCHAR_VECTOR opt;
//!     int  (*handler)	(PLCHAR_VECTOR, PLCHAR_VECTOR, void *);
//!     void *client_data;
//!     void *var;
//!     long mode;
//!     PLCHAR_VECTOR syntax;
//!     PLCHAR_VECTOR desc;
//! } PLOptionTable;
//!
//! where each entry has the following meaning:
//!
//! opt		option string
//! handler	pointer to function for processing the option and
//!		 (optionally) its argument
//! client_data	pointer to data that gets passed to (*handler)
//! var		address of variable to set based on "mode"
//! mode		governs handling of option (see below)
//! syntax	short syntax description
//! desc		long syntax description
//!
//! The syntax and or desc strings can be NULL if the option is never to be
//! described.  Usually this is only used for obsolete arguments; those we
//! just wish to hide from normal use are better made invisible (which are
//! made visible by either specifying -showall first or PL_PARSE_SHOWALL).
//!
//! The mode bits are:
//!
//! PL_OPT_ARG		Option has an argument
//! PL_OPT_NODELETE	Don't delete after processing
//! PL_OPT_INVISIBLE	Make invisible (usually for debugging)
//! PL_OPT_DISABLED	Ignore this option
//!
//! The following mode bits cause the option to be processed as specified:
//!
//! PL_OPT_FUNC		Call function handler (opt, opt_arg)
//! PL_OPT_BOOL		Set *var=1
//! PL_OPT_INT		Set *var=atoi(opt_arg)
//! PL_OPT_FLOAT		Set *var=atof(opt_arg)
//! PL_OPT_STRING	Set *var=opt_arg
//!
//! where opt points to the option string and opt_arg points to the
//! argument string.
//!
//--------------------------------------------------------------------------

static PLOptionTable ploption_table[] = {
    {
        "showall",              // Turns on invisible options
        NULL,
        NULL,
        &mode_showall,
        PL_OPT_BOOL | PL_OPT_INVISIBLE,
        "-showall",
        "Turns on invisible options"
    },
    {
        "h",                    // Help
        opt_h,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-h",
        "Print out this message"
    },
    {
        "v",                    // Version
        opt_v,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-v",
        "Print out the PLplot library version number"
    },
    {
        "verbose",              // Be more verbose than usual
        opt_verbose,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-verbose",
        "Be more verbose than usual"
    },
    {
        "debug",                // Print debugging info
        opt_debug,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-debug",
        "Print debugging info (implies -verbose)"
    },
    {
        "hack",                 // Enable driver-specific hack(s)
        opt_hack,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_INVISIBLE,
        "-hack",
        "Enable driver-specific hack(s)"
    },
    {
        "dev",                  // Output device
        opt_dev,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-dev name",
        "Output device name"
    },
    {
        "o",                    // Output filename
        opt_o,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-o name",
        "Output filename"
    },
    {
        "display",              // X server
        opt_o,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-display name",
        "X server to contact"
    },
    {
        "px",                   // Plots per page in x
        opt_px,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-px number",
        "Plots per page in x"
    },
    {
        "py",                   // Plots per page in y
        opt_py,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-py number",
        "Plots per page in y"
    },
    {
        "geometry",             // Geometry
        opt_geo,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-geometry geom",
        "Window size/position specified as in X, e.g., 400x300, 400x300-100+200, +100-200, etc."
    },
    {
        "geo",                  // Geometry (alias)
        opt_geo,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG | PL_OPT_INVISIBLE,
        "-geo geom",
        "Window size/position specified as in X, e.g., 400x300, 400x300-100+200, +100-200, etc."
    },
    {
        "wplt",                 // Plot window
        opt_wplt,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-wplt xl,yl,xr,yr",
        "Relative coordinates [0-1] of window into plot"
    },
    {
        "mar",                  // Margin
        opt_mar,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-mar margin",
        "Margin space in relative coordinates (0 to 0.5, def 0)"
    },
    {
        "a",                    // Aspect ratio
        opt_a,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-a aspect",
        "Page aspect ratio (def: same as output device)"
    },
    {
        "jx",                   // Justification in x
        opt_jx,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-jx justx",
        "Page justification in x (-0.5 to 0.5, def 0)"
    },
    {
        "jy",                   // Justification in y
        opt_jy,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-jy justy",
        "Page justification in y (-0.5 to 0.5, def 0)"
    },
    {
        "ori",                  // Orientation
        opt_ori,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-ori orient",
        "Plot orientation (0,1,2,3=landscape,portrait,seascape,upside-down)"
    },
    {
        "freeaspect",           // floating aspect ratio
        opt_freeaspect,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-freeaspect",
        "Allow aspect ratio to adjust to orientation swaps"
    },
    {
        "portrait",             // floating aspect ratio
        opt_portrait,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-portrait",
        "Sets portrait mode (both orientation and aspect ratio)"
    },
    {
        "width",                // Pen width
        opt_width,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-width width",
        "Sets pen width (0 <= width)"
    },
    {
        "bg",                   // Background color
        opt_bg,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-bg color",
        "Background color (e.g., FF0000=opaque red, 0000FF_0.1=blue with alpha of 0.1)"
    },
    {
        "ncol0",                // Allocated colors in cmap 0
        opt_ncol0,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-ncol0 n",
        "Number of colors to allocate in cmap 0 (upper bound)"
    },
    {
        "ncol1",                // Allocated colors in cmap 1
        opt_ncol1,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-ncol1 n",
        "Number of colors to allocate in cmap 1 (upper bound)"
    },
    {
        "fam",                  // Familying on switch
        opt_fam,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-fam",
        "Create a family of output files"
    },
    {
        "fsiz",                 // Family file size
        opt_fsiz,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-fsiz size[kKmMgG]",
        "Output family file size (e.g. -fsiz 0.5G, def MB)"
    },
    {
        "fbeg",                 // Family starting member
        opt_fbeg,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-fbeg number",
        "First family member number on output"
    },
    {
        "finc",                 // Family member increment
        opt_finc,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-finc number",
        "Increment between family members"
    },
    {
        "fflen",                // Family member min field width
        opt_fflen,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-fflen length",
        "Family member number minimum field width"
    },
    {
        "nopixmap",             // Do not use pixmaps
        opt_nopixmap,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-nopixmap",
        "Don't use pixmaps in X-based drivers"
    },
    {
        "db",                   // Double buffering on switch
        opt_db,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-db",
        "Double buffer X window output"
    },
    {
        "np",                   // Page pause off switch
        opt_np,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-np",
        "No pause between pages"
    },
    {
        "bufmax",               // # bytes sent before flushing output
        opt_bufmax,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG | PL_OPT_INVISIBLE,
        "-bufmax",
        "bytes sent before flushing output"
    },
    {
        "server_name",          // Main window name of server
        opt_server_name,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-server_name name",
        "Main window name of PLplot server (tk driver)"
    },
    {
        "plserver",             // PLplot server name
        opt_plserver,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG | PL_OPT_INVISIBLE,
        "-plserver name",
        "Invoked name of PLplot server (tk driver)"
    },
    {
        "plwindow",             // PLplot container window name
        opt_plwindow,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG | PL_OPT_INVISIBLE,
        "-plwindow name",
        "Name of PLplot container window (tk driver)"
    },
    {
        "auto_path",            // Additional directory(s) to autoload
        opt_auto_path,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG | PL_OPT_INVISIBLE,
        "-auto_path dir",
        "Additional directory(s) to autoload (tk driver)"
    },
    {
        "tk_file",  // -file option for plserver
        opt_tk_file,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG | PL_OPT_INVISIBLE,
        "-tk_file file",
        "file for plserver (tk driver)"
    },
    {
        "dpi",                  // Dots per inch
        opt_dpi,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-dpi dpi",
        "Resolution, in dots per inch (e.g. -dpi 360x360)"
    },
    {
        "compression",                  // compression
        opt_dev_compression,
        NULL,
        NULL,
        PL_OPT_FUNC | PL_OPT_ARG,
        "-compression num",
        "Sets compression level in supporting devices"
    },
    {
        "cmap0",
        opt_cmap0,
        NULL,
        NULL,
        PL_OPT_ARG | PL_OPT_FUNC,
        "-cmap0 file name",
        "Initializes color table 0 from a cmap0.pal format file in one of standard PLplot paths."
    },
    {
        "cmap1",
        opt_cmap1,
        NULL,
        NULL,
        PL_OPT_ARG | PL_OPT_FUNC,
        "-cmap1 file name",
        "Initializes color table 1 from a cmap1.pal format file in one of standard PLplot paths."
    },
    {
        "locale",
        opt_locale,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-locale",
        "Use locale environment (e.g., LC_ALL, LC_NUMERIC, or LANG) to set LC_NUMERIC locale (which affects decimal point separator)."
    },
    {
        "eofill",
        opt_eofill,
        NULL,
        NULL,
        PL_OPT_FUNC,
        "-eofill",
        "For the case where the boundary of the filled region is self-intersecting, use the even-odd fill rule rather than the default nonzero fill rule."
    },
    {
        "drvopt",               // Driver specific options
        opt_drvopt,
        NULL,
        NULL,
        PL_OPT_ARG | PL_OPT_FUNC,
        "-drvopt option[=value][,option[=value]]*",
        "Driver specific options"
    },
    {
        "mfo",                  // Metafile output option
        opt_mfo,
        NULL,
        NULL,
        PL_OPT_ARG | PL_OPT_FUNC,
        "-mfo PLplot metafile name",
        "Write the plot to the specified PLplot metafile"
    },
    {
        "mfi",                  // Metafile output option
        opt_mfi,
        NULL,
        NULL,
        PL_OPT_ARG | PL_OPT_FUNC,
        "-mfi PLplot metafile name",
        "Read the specified PLplot metafile"
    },
    {
        NULL,                   // option
        NULL,                   // handler
        NULL,                   // client data
        NULL,                   // address of variable to set
        0,                      // mode flag
        NULL,                   // short syntax
        NULL
    }                           // long syntax
};

static PLCHAR_VECTOR plplot_notes[] = {
    "All parameters must be white-space delimited.  Some options are driver",
    "dependent.  Please see the PLplot reference document for more detail.",
    NULL
};

//--------------------------------------------------------------------------
//! @struct PLOptionInfo
//!
//! Array of option tables and associated info.
//!
//! The user may merge up to PL_MAX_OPT_TABLES custom option tables (of type
//! PLOptionTable) with the internal one.  The resulting treatment is simple,
//! powerful, and robust.  The tables are parsed in the order of last added
//! first, to the internal table last.  If multiple options of the same name
//! occur, only the first parsed is "seen", thus, the user can easily
//! override any PLplot internal option merely by providing the same option.
//! This same precedence is followed when printing help and usage messages,
//! with each set of options given separately.  See example usage in
//! plrender.c.
//--------------------------------------------------------------------------

typedef struct
{
    PLOptionTable *options;
    PLCHAR_VECTOR name;
    const char    **notes;
} PLOptionInfo;

PLOptionInfo ploption_info_default = {
    ploption_table,
    "PLplot options",
    plplot_notes
};

#define PL_MAX_OPT_TABLES    10
PLOptionInfo ploption_info[PL_MAX_OPT_TABLES] = {
    {
        ploption_table,
        "PLplot options",
        plplot_notes
    }
};

// The structure that hold the driver specific command line options

typedef struct DrvOptCmd
{
    char *option;
    char *value;
    struct DrvOptCmd *next;
} DrvOptCmd;

// the variable where opt_drvopt() stores the driver specific command line options
static DrvOptCmd drv_opt = { NULL, NULL, NULL };

static int       tables = 1;

PLINT
c_plsetopt( PLCHAR_VECTOR opt, PLCHAR_VECTOR opt_arg )
{
    int  mode = 0, argc = 2, status;
    char *argv[3];

    argv[0] = (char *) opt;
    argv[1] = (char *) opt_arg;
    argv[2] = NULL;
    mode    =
        PL_PARSE_QUIET |
        PL_PARSE_NODELETE |
        PL_PARSE_NOPROGRAM |
        PL_PARSE_NODASH;

    status = plparseopts( &argc, argv, mode );
    if ( status )
    {
        fprintf( stderr, "plsetopt: Unrecognized option %s\n", opt );
    }
    return status;
}

//--------------------------------------------------------------------------
// plMergeOpts()
//
//! Merge user option table info structure with internal one.
//!
//! @param options ?
//! @param name ?
//! @param notes ?
//!
//--------------------------------------------------------------------------

PLINT
plMergeOpts( PLOptionTable *options, PLCHAR_VECTOR name, PLCHAR_VECTOR *notes )
{
    PLOptionTable *tab;

    pllib_init();

// Check to make sure option table has been terminated correctly

    for ( tab = (PLOptionTable *) options; tab->opt; tab++ )
        ;

// We've reached the last table entry.  All the subentries must be NULL or 0

    if ( ( tab->handler != NULL ) ||
         ( tab->client_data != NULL ) ||
         ( tab->var != NULL ) ||
         ( tab->mode != 0 ) ||
         ( tab->syntax != NULL ) ||
         ( tab->desc != NULL ) )
    {
        plabort( "plMergeOpts: input table improperly terminated" );
        return 1;
    }

// No room for more tables

    if ( tables++ >= PL_MAX_OPT_TABLES )
    {
        plabort( "plMergeOpts: max tables limit exceeded, table not merged" );
        return 1;
    }

    ploption_info[tables - 1].options = options;
    ploption_info[tables - 1].name    = name;
    ploption_info[tables - 1].notes   = notes;

    return 0;
}

//--------------------------------------------------------------------------
// plClearOpts()
//
//! Clear internal option table info structure.
//!
//--------------------------------------------------------------------------

void
plClearOpts( void )
{
    tables = 0;
}

//--------------------------------------------------------------------------
// plResetOpts()
//
//! Reset internal option table info structure.
//!
//--------------------------------------------------------------------------

void
plResetOpts( void )
{
    ploption_info[0] = ploption_info_default;
    tables           = 1;
}

//--------------------------------------------------------------------------
// plparseopts()
//
//! Process options list using current ploptions_info structure.
//! An error in parsing the argument list causes a program exit if
//! mode_full is set, otherwise the function returns with an error.
//!
//! @param p_argc pointer to a value that ONLY keeps track of number of arguments after processing.
//! @param argv ?
//! @param mode ?
//!
//! @returns 0 if successful.
//!
//--------------------------------------------------------------------------

PLINT
c_plparseopts( int *p_argc, char **argv, PLINT mode )
{
    char **argsave, **argend;
    int  i, myargc, myargcsave, status = 0;


// Initialize

    mode_full      = mode & PL_PARSE_FULL;
    mode_quiet     = mode & PL_PARSE_QUIET;
    mode_nodelete  = mode & PL_PARSE_NODELETE;
    mode_showall   = mode & PL_PARSE_SHOWALL;
    mode_noprogram = mode & PL_PARSE_NOPROGRAM;
    mode_nodash    = mode & PL_PARSE_NODASH;
    mode_skip      = mode & PL_PARSE_SKIP;

    myargc = ( *p_argc );
    argend = argv + myargc;

// If program name is first argument, save and advance

    if ( !mode_noprogram )
    {
        // Just in case plparseopts has been called previously (e.g., with PL_PARSE_NODELETE).
        if ( plsc->program )
            free_mem( plsc->program );

        // If plparseopts is not called again, this is freed in plend1.
        plsc->program = plstrdup( argv[0] );
        program       = (PLCHAR_VECTOR) plsc->program;
        --myargc; ++argv;
    }
    if ( myargc == 0 )
        return 0;

    // Process the command line

    // Special hack to deal with -debug option before
    // pllib_init() is called.
    argsave    = argv;
    myargcsave = myargc;
    for (; myargc > 0; --myargc, ++argv )
    {
        // Allow for "holes" in argv list
        if ( *argv == NULL || *argv[0] == '\0' )
            continue;

        if ( ( !mode_nodash && !strcmp( *argv, "-debug" ) ) || ( mode_nodash && !strcmp( *argv, "debug" ) ) )
        {
            //fprintf(stderr, "Found debug option in argv\n");
            // Loop over all options tables, starting with the last
            for ( i = tables - 1; i >= 0; i-- )
            {
                // Check option table for option

                status = ParseOpt( &myargc, &argv, p_argc, &argsave,
                    ploption_info[i].options );

                if ( !status )
                    break;
            }
            break;
        }
    }
    // Restore pointers to condition before the above loop
    // Although array length and content stored in those pointers
    // is likely changed.
    myargc = myargcsave;
    argv   = argsave;

    pllib_init();

    argsave = argv;
    for (; myargc > 0; --myargc, ++argv )
    {
        // Allow for "holes" in argv list
        if ( *argv == NULL || *argv[0] == '\0' )
            continue;

        // Loop over all options tables, starting with the last
        for ( i = tables - 1; i >= 0; i-- )
        {
            // Check option table for option

            status = ParseOpt( &myargc, &argv, p_argc, &argsave,
                ploption_info[i].options );

            if ( !status )
                break;
        }

        // Handle error return as specified by the mode flag

        if ( status == -1 )
        {
            // No match.  Keep going if mode_skip is set, otherwise abort if
            // fully parsing, else return without error.

            status = 0;

            if ( mode_skip )
            {
                if ( !mode_nodelete )
                    *argsave++ = *argv;
                continue;
            }
            if ( !mode_quiet && mode_full )
            {
                fprintf( stderr, "\nBad command line option \"%s\"\n", argv[0] );
                plOptUsage();
            }
            if ( mode_full )
                exit( 1 );

            break;
        }
        else if ( status == 1 )
        {
            // Illegal or badly formed

            if ( !mode_quiet )
            {
                fprintf( stderr, "\nBad command line option \"%s\"\n", argv[0] );
                plOptUsage();
            }
            if ( mode_full )
                exit( 1 );

            break;
        }
        else if ( status == 2 )
        {
            // Informational option encountered (-h or -v)

            exit( 0 );
        }
    }

// Compress and NULL-terminate argv

    if ( !mode_nodelete )
    {
        for ( i = 0; i < myargc; i++ )
            *argsave++ = *argv++;

        if ( argsave < argend )
        {
            *argsave = NULL;
#ifdef HAVE_NSGETARGC
            //
            // Modify the global argc variable to match the shortened argv.
            // The global argc and argv must be kept consistent so that future
            // users of them (e.g. libraries loaded later with a device driver)
            // will not try to dereference the null pointer at the end of the
            // shortened argv array.
            //
            *_NSGetArgc() = *p_argc;
#endif
        }
    }

    return status;
}

//--------------------------------------------------------------------------
// ParseOpt()
//
//! Parses & determines appropriate action for input flag.
//!
//! @param p_myargc ?
//! @param p_argv ?
//! @param p_argc pointer to a value that ONLY keeps track of number of arguments after processing.
//! @param p_argsave ?
//! @param option_table ?
//!
//! @returns ?
//!
//--------------------------------------------------------------------------

static int
ParseOpt( int *p_myargc, char ***p_argv, int *p_argc, char ***p_argsave,
          PLOptionTable *option_table )
{
    PLOptionTable *tab;
    char          *opt;

// Only handle actual flags and their arguments

    if ( mode_nodash || ( *p_argv )[0][0] == '-' )
    {
        opt = ( *p_argv )[0];
        if ( *opt == '-' )
            opt++;

        for ( tab = option_table; tab->opt; tab++ )
        {
            // Skip if option not enabled

            if ( tab->mode & PL_OPT_DISABLED )
                continue;

            // Try to match it

            if ( *opt == *tab->opt && !strcmp( opt, tab->opt ) )
            {
                // Option matched, so remove from argv list if applicable.

                if ( !mode_nodelete )
                {
                    if ( tab->mode & PL_OPT_NODELETE )
                        ( *( *p_argsave )++ ) = ( **p_argv );
                    else
                        --( *p_argc );
                }

                // Process option (and argument if applicable)

                return ( ProcessOpt( opt, tab, p_myargc, p_argv, p_argc ) );
            }
        }
    }

    return -1;
}

//--------------------------------------------------------------------------
// ProcessOpt()
//
//! Process option (and argument if applicable).
//!
//! @param opt ?
//! @param tab ?
//! @param p_myargc ?
//! @param p_argv ?
//! @param p_argc pointer to a value that ONLY keeps track of number of arguments after processing.
//!
//! @returns 0 if successful.
//--------------------------------------------------------------------------

static int
ProcessOpt( char * opt, PLOptionTable *tab, int *p_myargc, char ***p_argv,
            int *p_argc )
{
    int  need_arg, res;
    char *opt_arg = NULL;

// Get option argument if necessary

    need_arg = PL_OPT_ARG | PL_OPT_INT | PL_OPT_FLOAT | PL_OPT_STRING;

    if ( tab->mode & need_arg )
    {
        if ( GetOptarg( &opt_arg, p_myargc, p_argv, p_argc ) )
            return 1;
    }

// Process argument

    switch ( tab->mode & 0xFF00 )
    {
    case PL_OPT_FUNC:

        // Call function handler to do the job

        if ( tab->handler == NULL )
        {
            fprintf( stderr,
                "ProcessOpt: no handler specified for option %s\n",
                tab->opt );
            return 1;
        }

        if ( mode_nodelete && opt_arg )
        {
            // Make a copy, since handler may mung opt_arg with strtok()
            char *copy =
                (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) );
            if ( copy == NULL )
            {
                plabort( "ProcessOpt: out of memory" );
                return 1;
            }
            strcpy( copy, opt_arg );
            res = ( ( *tab->handler )( opt, copy, tab->client_data ) );
            free( (void *) copy );
            return res;
        }
        else
        {
            return ( ( *tab->handler )( opt, opt_arg, tab->client_data ) );
        }

    case PL_OPT_BOOL:

        // Set *var as a boolean

        if ( tab->var == NULL )
        {
            fprintf( stderr,
                "ProcessOpt: no variable specified for option %s\n",
                tab->opt );
            return 1;
        }
        *(int *) tab->var = 1;
        break;

    case PL_OPT_INT:

        // Set *var as an int

        if ( tab->var == NULL )
        {
            fprintf( stderr,
                "ProcessOpt: no variable specified for option %s\n",
                tab->opt );
            return 1;
        }
        *(int *) tab->var = atoi( opt_arg );
        break;

    case PL_OPT_FLOAT:

        // Set *var as a float

        if ( tab->var == NULL )
        {
            fprintf( stderr,
                "ProcessOpt: no variable specified for option %s\n",
                tab->opt );
            return 1;
        }
        *(PLFLT *) tab->var = atof( opt_arg );
        break;

    case PL_OPT_STRING:

        // Set var (can be NULL initially) to point to opt_arg string

        *(char **) tab->var = opt_arg;
        break;

    default:

        // Somebody messed up..

        fprintf( stderr,
            "ProcessOpt: invalid processing mode for option %s\n",
            tab->opt );
        return 1;
    }
    return 0;
}

//--------------------------------------------------------------------------
// GetOptarg()
//
//! Retrieves an option argument.
//! If an error occurs here it is a true syntax error.
//!
//! @param popt_arg ?
//! @param p_myargc ?
//! @param p_argv ?
//! @param p_argc pointer to a value that ONLY keeps track of number of arguments after processing.
//!
//! @returns 0 if successful.
//!
//--------------------------------------------------------------------------

static int
GetOptarg( char **popt_arg, int *p_myargc, char ***p_argv, int *p_argc )
{
    int result = 0;

    --( *p_myargc );

    if ( ( *p_myargc ) <= 0 )           // oops, no more arguments
        result = 1;

    if ( !result )
    {
        ( *p_argv )++;
        // Skip -bg argument checking since, for example, "-ffffff" is
        // valid but would be considered invalid by the crude test at
        // the end of the if.  Instead, -bg always consumes the next
        // argument (which exists according to the test above) in any
        // form, and that argument is checked for validity by the
        //  opt_bg routine.
        if ( strstr( ( ( *p_argv ) - 1 )[0], "-bg" ) != ( ( *p_argv ) - 1 )[0] && ( *p_argv )[0][0] == '-' && isalpha( ( *p_argv )[0][1] ) )
        {
            ( *p_argv )--;                // oops, next arg is a flag
            result = 1;
        }
    }

    if ( !result )                      // yeah, the user got it right
    {
        if ( !mode_nodelete )
            ( *p_argc )--;
        *popt_arg = ( *p_argv )[0];
    }
    else
    {
        if ( !mode_quiet )
        {
            fprintf( stderr, "Argument missing for %s option.\n", ( *p_argv )[0] );
            plOptUsage();
        }
    }
    return result;
}

//--------------------------------------------------------------------------
// plSetUsage()
//
//! Set the strings used in usage and syntax messages.
//!
//! @param program_string The program name.
//! @param usage_string String describing how to use the program.
//!
//--------------------------------------------------------------------------

void
plSetUsage( PLCHAR_VECTOR program_string, PLCHAR_VECTOR usage_string )
{
    if ( program_string != NULL )
        program = program_string;

    if ( usage_string != NULL )
        usage = usage_string;
}

//--------------------------------------------------------------------------
// plOptUsage()
//
//! Print usage & syntax message.
//!
//--------------------------------------------------------------------------

void
plOptUsage( void )
{
    if ( usage == NULL )
        fprintf( stderr, "\nUsage:\n        %s [options]\n", program );
    else
        fputs( usage, stderr );

    Syntax();

    fprintf( stderr, "\n\nType %s -h for a full description.\n\n",
        program );
}

//--------------------------------------------------------------------------
// Syntax()
//
//! Print short syntax message.
//!
//--------------------------------------------------------------------------

static void
Syntax( void )
{
    PLOptionTable *tab;
    int           i, col, len;

// Loop over all options tables

    for ( i = tables - 1; i >= 0; i-- )
    {
        // Introducer

        if ( ploption_info[i].name )
            fprintf( stderr, "\n%s:", ploption_info[i].name );
        else
            fputs( "\nUser options:", stderr );

        // Print syntax for each option

        col = 80;
        for ( tab = ploption_info[i].options; tab->opt; tab++ )
        {
            if ( tab->mode & PL_OPT_DISABLED )
                continue;

            if ( !mode_showall && ( tab->mode & PL_OPT_INVISIBLE ) )
                continue;

            if ( tab->syntax == NULL )
                continue;

            len = 3 + (int) strlen( tab->syntax );              // space [ string ]
            if ( col + len > 79 )
            {
                fprintf( stderr, "\n   " );               // 3 spaces
                col = 3;
            }
            fprintf( stderr, " [%s]", tab->syntax );
            col += len;
        }
        fprintf( stderr, "\n" );
    }
}

//--------------------------------------------------------------------------
// Help()
//
//! Print long help message.
//!
//--------------------------------------------------------------------------

static void
Help( void )
{
    PLOptionTable *tab;
    const char    **note;
    int           i;
    FILE          *outfile = stderr;

#ifdef HAVE_POPEN
    FILE *pager = NULL;
    if ( getenv( "PAGER" ) != NULL )
        pager = (FILE *) popen( "$PAGER", "w" );
    if ( pager == NULL )
        pager = (FILE *) popen( "more", "w" );
    if ( pager != NULL )
        outfile = pager;
#endif

// Usage line

    if ( usage == NULL )
        fprintf( outfile, "\nUsage:\n        %s [options]\n", program );
    else
        fputs( usage, outfile );

// Loop over all options tables

    for ( i = tables - 1; i >= 0; i-- )
    {
        // Introducer

        if ( ploption_info[i].name )
            fprintf( outfile, "\n%s:\n", ploption_info[i].name );
        else
            fputs( "\nUser options:\n", outfile );

        // Print description for each option

        for ( tab = ploption_info[i].options; tab->opt; tab++ )
        {
            if ( tab->mode & PL_OPT_DISABLED )
                continue;

            if ( !mode_showall && ( tab->mode & PL_OPT_INVISIBLE ) )
                continue;

            if ( tab->desc == NULL )
                continue;

            if ( tab->mode & PL_OPT_INVISIBLE )
                fprintf( outfile, " *  %-20s %s\n", tab->syntax, tab->desc );
            else
                fprintf( outfile, "    %-20s %s\n", tab->syntax, tab->desc );
        }

        // Usage notes

        if ( ploption_info[i].notes )
        {
            putc( '\n', outfile );
            for ( note = ploption_info[i].notes; *note; note++ )
            {
                fputs( *note, outfile );
                putc( '\n', outfile );
            }
        }
    }

#ifdef HAVE_POPEN
    if ( pager != NULL )
        pclose( pager );
#endif
}

//--------------------------------------------------------------------------
// plParseDrvOpts
//
//! Parse driver specific options
//!
//! @param acc_opt ?
//!
//! @returns 0 if successful.
//!
//--------------------------------------------------------------------------

int
plParseDrvOpts( DrvOpt *acc_opt )
{
    DrvOptCmd *drvp;
    DrvOpt    *t;
    int       fl;
    char      msg[80];
    memset( msg, '\0', sizeof ( msg ) );

    if ( !drv_opt.option )
        return 1;

    drvp = &drv_opt;
    do
    {
        t = acc_opt; fl = 0;
        while ( t->opt )
        {
            if ( strcmp( drvp->option, t->opt ) == 0 )
            {
                fl = 1;
                switch ( t->type )
                {
                case DRV_STR:
                    *(char **) ( t->var_ptr ) = ( drvp->value );
#ifdef DEBUG
                    fprintf( stderr, "plParseDrvOpts: %s %s\n", t->opt, *(char **) t->var_ptr );
#endif
                    break;

                case DRV_INT:
                    if ( sscanf( drvp->value, "%d", (int *) t->var_ptr ) != 1 )
                    {
                        snprintf( msg, sizeof ( msg ) - 1, "Incorrect argument to '%s' option", drvp->option );
                        plexit( msg );
                    }
#ifdef DEBUG
                    fprintf( stderr, "plParseDrvOpts: %s %d\n", t->opt, *(int *) t->var_ptr );
#endif
                    break;

                case DRV_FLT:
                    if ( sscanf( drvp->value, "%f", (float *) t->var_ptr ) != 1 )
                    {
                        snprintf( msg, sizeof ( msg ) - 1, "Incorrect argument to '%s' option", drvp->option );
                        plexit( msg );
                    }
#ifdef DEBUG
                    fprintf( stderr, "plParseDrvOpts: %s %f\n", t->opt, *(float *) t->var_ptr );
#endif
                    break;
                }
            }
            t++;
        }

        if ( !fl )
        {
            snprintf( msg, sizeof ( msg ) - 1, "Option '%s' not recognized.\n\nRecognized options for this driver are:\n", drvp->option );
            plwarn( msg );
            plHelpDrvOpts( acc_opt );
            plexit( "" );
        }
    }
    while ( ( drvp = drvp->next ) )
    ;

    return 0;
}

//--------------------------------------------------------------------------
// plHelpDrvOpts
//
//! Give driver specific help
//!
//! @param acc_opt ?
//!
//--------------------------------------------------------------------------

void
plHelpDrvOpts( DrvOpt *acc_opt )
{
    DrvOpt *t;

    t = acc_opt;
    while ( t->opt )
    {
        fprintf( stderr, "%s:\t%s\n", t->opt, t->hlp_msg );
        t++;
    }
}

//--------------------------------------------------------------------------
// tidyDrvOpts
//
//! Tidy up and free memory associated with driver options
//!
//--------------------------------------------------------------------------

void
plP_FreeDrvOpts()
{
    DrvOptCmd *drvp, *drvpl;

    drvp = &drv_opt;
    do
    {
        drvpl = drvp;
        drvp  = drvpl->next;

        free( drvpl->option );
        free( drvpl->value );
        // Free additional DrvOptCmd variables -
        // first entry in list is a static global variable
        if ( drvpl != &drv_opt )
            free( drvpl );
    } while ( drvp != NULL );

    // initialize drv_opt if it's used again
    drv_opt.option = NULL;
    drv_opt.value  = NULL;
    drv_opt.next   = NULL;
}


//--------------------------------------------------------------------------
// Option handlers
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// opt_h()
//
//! Performs appropriate action for option "h":
//! Issues help message
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 2.
//!
//--------------------------------------------------------------------------

static int
opt_h( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    if ( !mode_quiet )
        Help();

    return 2;
}

//--------------------------------------------------------------------------
// opt_v()
//
//! Performs appropriate action for option "v":
//! Issues version message
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 2.
//!
//--------------------------------------------------------------------------

static int
opt_v( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    if ( !mode_quiet )
        fprintf( stderr, "PLplot library version: %s\n", PLPLOT_VERSION );

    return 2;
}

//--------------------------------------------------------------------------
// opt_verbose()
//
//! Performs appropriate action for option "verbose":
//! Turn on verbosity flag
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_verbose( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->verbose = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_debug()
//
//! Performs appropriate action for option "debug":
//! Turn on debugging flag
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_debug( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->debug   = 1;
    plsc->verbose = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_hack()
//
//! Performs appropriate action for option "hack":
//! Enables driver-specific hack(s)
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_hack( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->hack = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_dev()
//
//! Performs appropriate action for option "dev":
//! Sets output device keyword
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg The name of the output device.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_dev( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsdev( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_o()
//
//! Performs appropriate action for option "o":
//! Sets output file name
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg The file family name.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_o( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsfnam( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_mar()
//
//! Performs appropriate action for option "mar":
//! Sets relative margin width
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot margin width.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_mar( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsdidev( atof( opt_arg ), PL_NOTSET, PL_NOTSET, PL_NOTSET );
    return 0;
}

//--------------------------------------------------------------------------
// opt_a()
//
//! Performs appropriate action for option "a":
//! Sets plot aspect ratio on page
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot aspect ratio.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_a( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsdidev( PL_NOTSET, atof( opt_arg ), PL_NOTSET, PL_NOTSET );
    return 0;
}

//--------------------------------------------------------------------------
// opt_jx()
//
//! Performs appropriate action for option "jx":
//! Sets relative justification in x
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot relative justification in x(?)
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_jx( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsdidev( PL_NOTSET, PL_NOTSET, atof( opt_arg ), PL_NOTSET );
    return 0;
}

//--------------------------------------------------------------------------
// opt_jy()
//
//! Performs appropriate action for option "jy":
//! Sets relative justification in y
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot relative justification in y(?)
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_jy( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsdidev( PL_NOTSET, PL_NOTSET, PL_NOTSET, atof( opt_arg ) );
    return 0;
}

//--------------------------------------------------------------------------
// opt_ori()
//
//! Performs appropriate action for option "ori":
//! Sets orientation
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot orientation.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_ori( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsdiori( atof( opt_arg ) );
    return 0;
}

//--------------------------------------------------------------------------
// opt_freeaspect()
//
//! Performs appropriate action for option "freeaspect":
//! Allow aspect ratio to adjust to orientation swaps.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_freeaspect( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->freeaspect = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_portrait()
//
//! Performs appropriate action for option "portrait":
//! Set portrait mode.  If plsc->portrait = 1, then the orientation for certain
//! drivers is changed by 90 deg to portrait orientation from the default
//! landscape orientation used by PLplot while the aspect ratio allowed to
//! adjust using freeaspect.
//! N.B. the driver list where this flag is honored is currently limited
//! to psc, ps, and pstex.  A 90 deg rotation is just not
//! appropriate for certain other drivers.  These drivers where portrait
//! mode is ignored include display drivers (e.g., xwin, tk), drivers
//! which are subequently going to be transformed to another form
//! (e.g., meta), or drivers which are normally used for web
//! publishing (e.g., png, jpeg).  That said, the case is not entirely clear
//! for all drivers so the list of drivers where portrait mode is honored
//! may increase in the future. To add to the list simply copy the small
//! bit of code from  ps.c that has to do with pls->portrait to the
//! appropriate driver file.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_portrait( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->portrait = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_width()
//
//! Performs appropriate action for option "width":
//! Sets pen width
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot pen width.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_width( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    double width;

    width = atof( opt_arg );
    if ( width < 0. )
    {
        fprintf( stderr, "?invalid width\n" );
        return 1;
    }
    else
    {
        plwidth( width );
        plsc->widthlock = 1;
    }
    return 0;
}

//--------------------------------------------------------------------------
// opt_bg()
//
//! Performs appropriate action for option "bg":
//! Sets background color (rgb represented in hex on command line) and alpha
//! (represented as floating point on the command line with underscore
//! delimiter), e.g.,
//! -bg ff0000 (set background to red with an alpha value of MAX_PLFLT_ALPHA ==> opaque)
//! -bg ff0000_0.1 (set background to red with an alpha value of 0.1)
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Background RGB color in hex (in 3-digit of 6-digit format)
//! followed by optional combination of "_" + floating-point alpha value.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_bg( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    PLCHAR_VECTOR rgb;
    char          *color_field, *color_field_wp, *color_field_wp_alt, *alpha_field;
    char          *endptr;
    long          bgcolor;
    PLINT         r, g, b;
    PLFLT         a;
    int           save_errno;

    // Strip off leading "#" (TK-ism) if present.

    if ( *opt_arg == '#' )
        rgb = opt_arg + 1;
    else
        rgb = opt_arg;

    strncpy( opttmp, rgb, OPTMAX - 1 );
    opttmp[OPTMAX - 1] = '\0';

    //fprintf( stderr, "-bg option = %s\n", opttmp );
    color_field = opttmp;
    alpha_field = strchr( opttmp, '_' );
    if ( alpha_field != NULL )
    {
        // null-terminate color_field at the position of the delimiter.
        *alpha_field = '\0';
        // point alpha_field at the position one beyond the delimiter.
        alpha_field++;
    }
    else
    {
        // If no delimiter, then assume opaque.
        alpha_field = "MAX_PLFLT_ALPHA";
    }

    //fprintf( stderr, "color_field = %s\n", color_field );
    //fprintf( stderr, "alpha_field = %s\n", alpha_field );

    // Parse color_field
    errno      = 0; // To distinguish success/failure after call
    bgcolor    = strtol( color_field, &endptr, 16 );
    save_errno = errno;

    // Check for various possible errors

    if ( ( errno == ERANGE && ( bgcolor == LONG_MIN || bgcolor == LONG_MAX ) ) || ( errno != 0 && bgcolor == 0 ) )
    {
        plwarn( "opt_bg: parsing of color_field as hex to a long caused integer overflow so use (warning) red" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        errno = save_errno;
        perror( "opt_bg strtol issue" );
        fprintf( stderr, "color_field = %s\n", color_field );
        color_field = "ff0000";
        fprintf( stderr, "derived color_field = %s\n", color_field );
        bgcolor = strtol( color_field, &endptr, 16 );
        fprintf( stderr, "derived bgcolor = %#lx\n", bgcolor );
    }
    else if ( endptr == color_field )
    {
        plwarn( "opt_bg: color_field could not be parsed as hex to a long so use (warning) red" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        fprintf( stderr, "color_field = %s\n", color_field );
        color_field = "ff0000";
        fprintf( stderr, "derived color_field = %s\n", color_field );
        bgcolor = strtol( color_field, &endptr, 16 );
        fprintf( stderr, "derived bgcolor = %#lx\n", bgcolor );
    }
    else if ( *endptr != '\0' )
    {
        plwarn( "opt_bg: color_field could be parsed as hex to a long but there was trailing garbage which was ignored" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        fprintf( stderr, "color_field = %s\n", color_field );
        // Trim trailing garbage off of color_field.
        *endptr = '\0';
        fprintf( stderr, "derived color_field = %s\n", color_field );
        fprintf( stderr, "derived bgcolor = %#lx\n", bgcolor );
    }

    // If bgcolor has 3 digits, each is "doubled" (i.e. ABC becomes AABBCC).

    // Find color_field without prefix where that prefix consists of optional whitespace followed
    // by optional sign followed by optional 0x or 0X.
    color_field_wp = color_field;
    while ( isspace( (unsigned char) *color_field_wp ) )
        color_field_wp++;
    if ( ( color_field_wp_alt = strstr( color_field_wp, "+" ) ) == color_field_wp ||
         ( color_field_wp_alt = strstr( color_field_wp, "-" ) ) == color_field_wp )
        color_field_wp += 1;
    if ( ( color_field_wp_alt = strstr( color_field_wp, "0x" ) ) == color_field_wp ||
         ( color_field_wp_alt = strstr( color_field_wp, "0X" ) ) == color_field_wp )
        color_field_wp += 2;

    switch ( strlen( color_field_wp ) )
    {
    case 3:
        r = (PLINT) ( ( bgcolor & 0xF00 ) >> 8 );
        g = (PLINT) ( ( bgcolor & 0x0F0 ) >> 4 );
        b = (PLINT) ( bgcolor & 0x00F );

        r = r | ( r << 4 );
        g = g | ( g << 4 );       // doubling
        b = b | ( b << 4 );
        break;

    case 6:
        r = (PLINT) ( ( bgcolor & 0xFF0000 ) >> 16 );
        g = (PLINT) ( ( bgcolor & 0x00FF00 ) >> 8 );
        b = (PLINT) ( bgcolor & 0x0000FF );
        break;

    default:
        plwarn( "opt_bg: color_field without prefix is not of the correct form.  Therefore use (warning) red" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        fprintf( stderr, "color_field = %s\n", color_field );
        fprintf( stderr, "%s\n", "The correct form of color_field without prefix is exactly 3 or 6 hex digits" );
        fprintf( stderr, "color_field without prefix = %s\n", color_field_wp );
        r = MAX_PLINT_RGB;
        g = 0;
        b = 0;
        fprintf( stderr, "derived r, g, b = %d, %d, %d\n", r, g, b );
    }

    // Parse alpha_field using strtod and checking for all potential issues.
    errno      = 0; // To distinguish success/failure after call
    a          = (PLFLT) strtod( alpha_field, &endptr );
    save_errno = errno;

    // Check for various possible errors

    if ( errno == ERANGE && ( a == HUGE_VAL || a == -HUGE_VAL ) )
    {
        plwarn( "opt_bg: parsing of alpha_field to a double caused floating overflow so use opaque" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        errno = save_errno;
        perror( "opt_bg strtod issue" );
        fprintf( stderr, "alpha_field = %s\n", alpha_field );
        a = MAX_PLFLT_ALPHA;
        fprintf( stderr, "derived alpha value = %e\n", a );
    }
    else if ( endptr == alpha_field )
    {
        plwarn( "opt_bg: alpha_field could not be parsed to a double so use opaque" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        fprintf( stderr, "alpha_field = %s\n", alpha_field );
        a = MAX_PLFLT_ALPHA;
        fprintf( stderr, "derived alpha value = %e\n", a );
    }
    else if ( *endptr != '\0' )
    {
        plwarn( "opt_bg: alpha_field could be parsed to a double but there was trailing garbage which was ignored" );
        fprintf( stderr, "%s\n", "Further information relevant to this warning:" );
        fprintf( stderr, "alpha_field = %s\n", alpha_field );
        fprintf( stderr, "derived alpha value = %e\n", a );
    }

    //fprintf( stderr, "r, g, b, alpha = %d, %d, %d, %e\n", r, g, b, a );
    plscolbga( r, g, b, a );

    return 0;
}

//--------------------------------------------------------------------------
// opt_ncol0()
//
//! Performs appropriate action for option "ncol0":
//! Sets number of colors to allocate in cmap 0 (upper bound).
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Number of color map 0 colors.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_ncol0( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->ncol0 = atoi( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_ncol1()
//
//! Performs appropriate action for option "ncol1":
//! Sets number of colors to allocate in cmap 1 (upper bound).
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Number of color map 1 colors.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_ncol1( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->ncol1 = atoi( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_wplt()
//
//! Performs appropriate action for option "wplt":
//! Sets (zoom) window into plot (e.g. "0,0,0.5,0.5")
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Zoom setting.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_wplt( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    char  *field;
    PLFLT xl, yl, xr, yr;

    strncpy( opttmp, opt_arg, OPTMAX - 1 );
    opttmp[OPTMAX - 1] = '\0';

    if ( ( field = strtok( opttmp, "," ) ) == NULL )
        return 1;

    xl = atof( field );

    if ( ( field = strtok( NULL, "," ) ) == NULL )
        return 1;

    yl = atof( field );

    if ( ( field = strtok( NULL, "," ) ) == NULL )
        return 1;

    xr = atof( field );

    if ( ( field = strtok( NULL, "," ) ) == NULL )
        return 1;

    yr = atof( field );

    plsdiplt( xl, yl, xr, yr );
    return 0;
}

//--------------------------------------------------------------------------
// opt_drvopt()
//
//! Get driver specific options in the form <option[=value]>[,option[=value]]*
//! If "value" is not specified, it defaults to "1".
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg The driver specific option.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_drvopt( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    char      t, *tt, *option, *value;
    int       fl = 0;
    DrvOptCmd *drvp;

    option = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) );
    if ( option == NULL )
        plexit( "opt_drvopt: Out of memory!?" );

    value = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) );
    if ( value == NULL )
        plexit( "opt_drvopt: Out of memory!?" );

    drvp    = &drv_opt;
    *option = *value = '\0';
    tt      = option;
    while ( ( t = *opt_arg++ ) )
    {
        switch ( t )
        {
        case ',':
            if ( fl )
                fl = 0;
            else
            {
                value[0] = '1';
                value[1] = '\0';
            }

            *tt          = '\0'; tt = option;
            drvp->option = plstrdup( option );                                               // it should not be release, because of familying
            drvp->value  = plstrdup( value );                                                // don't release
            drvp->next   = (DrvOptCmd *) malloc( sizeof ( DrvOptCmd ) );                     // don't release
            if ( drvp->next == NULL )
                plexit( "opt_drvopt: Out of memory!?\n" );

            drvp = drvp->next;
            break;

        case '=':
            fl  = 1;
            *tt = '\0'; tt = value;
            break;

        default:
            *tt++ = t;
        }
    }

    *tt = '\0';
    if ( !fl )
    {
        value[0] = '1';
        value[1] = '\0';
    }

    drvp->option = plstrdup( option );                       // don't release
    drvp->value  = plstrdup( value );                        // don't release
    drvp->next   = NULL;

#ifdef DEBUG
    fprintf( stderr, "\nopt_drvopt: -drvopt parsed options:\n" );
    drvp = &drv_opt;
    do
        fprintf( stderr, "%s %s\n", drvp->option, drvp->value );
    while ( drvp = drvp->next );
    fprintf( stderr, "\n" );
#endif

    free( option ); free( value );

    return 0;
}

//--------------------------------------------------------------------------
// opt_fam()
//
//! Performs appropriate action for option "fam":
//! Enables family output files
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_fam( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsfam( 1, -1, -1 );
    return 0;
}

//--------------------------------------------------------------------------
// opt_fsiz()
//
//! Performs appropriate action for option "fsiz":
//! Sets size of a family member file (may be somewhat larger since eof must
//! occur at a page break).  Also turns on familying.  Example usage:
//!
//!	-fsiz 5M	(5 MB)
//!	-fsiz 300K	(300 KB)
//!	-fsiz .3M	(same)
//!	-fsiz .5G	(half a GB)
//!
//! Note case of the trailing suffix doesn't matter.
//! If no suffix, defaults to MB.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Family size setting.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_fsiz( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    PLINT  bytemax;
    size_t len        = strlen( opt_arg );
    char   lastchar   = opt_arg[len - 1];
    PLFLT  multiplier = 1.0e6;
    char   *spec      = (char *) malloc( len + 1 );

    if ( spec == NULL )
        plexit( "opt_fsiz: Insufficient memory" );

// Interpret optional suffix

    switch ( lastchar )
    {
    case 'k':
    case 'K':
        multiplier = 1.0e3; len--;
        break;
    case 'm':
    case 'M':
        multiplier = 1.0e6; len--;
        break;
    case 'g':
    case 'G':
        multiplier = 1.0e9; len--;
        break;
    }
    strncpy( spec, opt_arg, len );
    spec[len] = '\0';

    bytemax = (PLINT) ( multiplier * atof( spec ) );
    if ( bytemax <= 0 )
    {
        fprintf( stderr, "?invalid file size %d. 2.14G is the maximum.\n", bytemax );
        return 1;
    }
    plsfam( 1, -1, bytemax );

    free( spec );
    return 0;
}

//--------------------------------------------------------------------------
// opt_fbeg()
//
//! Performs appropriate action for option "fbeg":
//! Starts with the specified family member number.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Number of the first plot.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_fbeg( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->member = atoi( opt_arg );

    return 0;
}

//--------------------------------------------------------------------------
// opt_finc()
//
//! Performs appropriate action for option "finc":
//! Specify increment between family members.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Amount to increment the plot number between plots.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_finc( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->finc = atoi( opt_arg );

    return 0;
}

//--------------------------------------------------------------------------
// opt_fflen()
//
//! Performs appropriate action for option "fflen":
//! Specify minimum field length for family member number.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Size of the family number field (e.g. "1", "01", "001" ?)
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_fflen( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->fflen = atoi( opt_arg );

    return 0;
}

//--------------------------------------------------------------------------
// opt_np()
//
//! Performs appropriate action for option "np":
//! Disables pause between pages
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_np( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plspause( 0 );
    return 0;
}

//--------------------------------------------------------------------------
// opt_nopixmap()
//
//! Performs appropriate action for option "nopixmap":
//! Disables use of pixmaps in X drivers
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_nopixmap( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->nopixmap = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_db()
//
//! Performs appropriate action for option "db":
//! Double buffer X output (update only done on eop or Expose)
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_db( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->db = 1;
    return 0;
}

//--------------------------------------------------------------------------
// opt_bufmax()
//
//! Performs appropriate action for option "bufmax":
//! Sets size of data buffer for tk driver
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Size of the data buffer for the tk driver.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_bufmax( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->bufmax = atoi( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_server_name()
//
//! Performs appropriate action for option "server_name":
//! Sets main window name of server (Tcl/TK driver only)
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg The name of the main window.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_server_name( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->server_name = plstrdup( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_plserver()
//
//! Performs appropriate action for option "plserver":
//! Sets name to use when invoking server (Tcl/TK driver only)
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Name of Tcl/TK server (?).
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_plserver( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->plserver = plstrdup( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_plwindow()
//
//! Performs appropriate action for option "plwindow":
//! Sets PLplot window name
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Name of the window.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_plwindow( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    if ( ( plsc->plwindow = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) ) ) == NULL )
    {
        plexit( "opt_plwindow: Insufficient memory" );
    }
    strcpy( plsc->plwindow, opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_auto_path()
//
//! Performs appropriate action for option "auto_path":
//! Sets additional directories to autoload
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Additional directories to add the the load path (?).
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_auto_path( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plsc->auto_path = plstrdup( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_px()
//
//! Performs appropriate action for option "px":
//! Set packing in x
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg X packing (?).
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_px( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plssub( atoi( opt_arg ), -1 );
    return 0;
}

//--------------------------------------------------------------------------
// opt_py()
//
//! Performs appropriate action for option "py":
//! Set packing in y
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Y packing (?).
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_py( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plssub( -1, atoi( opt_arg ) );
    return 0;
}

//--------------------------------------------------------------------------
// opt_geo()
//
//! Performs appropriate action for option "geo": Set geometry for
//! output window, i.e., "-geometry WIDTHxHEIGHT+XOFF+YOFF" where
//! WIDTHxHEIGHT, +XOFF+YOFF, or both must be present, and +XOFF+YOFF
//! stands for one of the four combinations +XOFF+YOFF, +XOFF-YOFF,
//! -XOFF+YOFF, and -XOFF-YOFF.  Some examples are the following:
//! -geometry 400x300, -geometry -100+200, and -geometry 400x300-100+200.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Plot geometry descriptor.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_geo( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    int   numargs;
    PLFLT xdpi = 0., ydpi = 0.;
    PLINT xwid, ywid, xoff, yoff;

// The TK driver uses the geometry string directly

    if ( ( plsc->geometry = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) ) ) == NULL )
    {
        plexit( "opt_geo: Insufficient memory" );
    }

    strcpy( plsc->geometry, opt_arg );

    numargs = sscanf( opt_arg, "%dx%d%d%d", &xwid, &ywid, &xoff, &yoff );
    if ( numargs == 2 )
    {
        xoff = 0;
        yoff = 0;
        if ( xwid == 0 )
            fprintf( stderr, "?invalid xwid in -geometry %s\n", opt_arg );
        if ( ywid == 0 )
            fprintf( stderr, "?invalid ywid in -geometry %s\n", opt_arg );
        if ( xwid < 0 )
        {
            fprintf( stderr, "?invalid xwid in -geometry %s\n", opt_arg );
            return 1;
        }
        if ( ywid < 0 )
        {
            fprintf( stderr, "?invalid ywid in -geometry %s\n", opt_arg );
            return 1;
        }
    }
    else if ( numargs == 4 )
    {
        if ( xwid == 0 )
            fprintf( stderr, "?invalid xwid in -geometry %s\n", opt_arg );
        if ( ywid == 0 )
            fprintf( stderr, "?invalid ywid in -geometry %s\n", opt_arg );
        if ( xwid < 0 )
        {
            fprintf( stderr, "?invalid xwid in -geometry %s\n", opt_arg );
            return 1;
        }
        if ( ywid < 0 )
        {
            fprintf( stderr, "?invalid ywid in -geometry %s\n", opt_arg );
            return 1;
        }
        if ( abs( xoff ) == 0 )
            fprintf( stderr, "?invalid xoff in -geometry %s\n", opt_arg );
        if ( abs( yoff ) == 0 )
            fprintf( stderr, "?invalid yoff in -geometry %s\n", opt_arg );
    }
    else
    {
        numargs = sscanf( opt_arg, "%d%d", &xoff, &yoff );
        if ( numargs == 2 )
        {
            xwid = 0;
            ywid = 0;
            if ( abs( xoff ) == 0 )
                fprintf( stderr, "?invalid xoff in -geometry %s\n", opt_arg );
            if ( abs( yoff ) == 0 )
                fprintf( stderr, "?invalid yoff in -geometry %s\n", opt_arg );
        }
        else
        {
            fprintf( stderr, "?invalid -geometry %s\n", opt_arg );
            return 1;
        }
    }
    //fprintf( stderr, "xwid, ywid, xoff, yoff = %d, %d, %d, %d\n", xwid, ywid, xoff, yoff );
    plspage( xdpi, ydpi, xwid, ywid, xoff, yoff );
    return 0;
}

//--------------------------------------------------------------------------
// opt_tk_file()
//
//! File name for plserver tk_file option
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Tk file name.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_tk_file( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    if ( ( plsc->tk_file = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) ) ) == NULL )
    {
        plexit( "opt_tk_file: Insufficient memory" );
    }

    strcpy( plsc->tk_file, opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_dpi()
//
//! Performs appropriate action for option "dpi":
//! Set dpi resolution for output device
//!   e.g.,  "-dpi 600x300", will set X dpi to 600 and Y dpi to 300
//!              or
//!   e.g., "-dpi 1200"
//! Will set both X and Y dpi to 1200 dpi
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg DPI descriptor string.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_dpi( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    char  *field;
    PLFLT xdpi = 0., ydpi = 0.;
    PLINT xwid = 0, ywid = 0, xoff = 0, yoff = 0;

    strncpy( opttmp, opt_arg, OPTMAX - 1 );
    opttmp[OPTMAX - 1] = '\0';
    if ( strchr( opttmp, 'x' ) )
    {
        field = strtok( opttmp, "x" );
        xdpi  = atof( field );
        if ( xdpi == 0 )
            fprintf( stderr, "?invalid xdpi\n" );

        if ( ( field = strtok( NULL, " " ) ) == NULL )
            return 1;

        ydpi = atof( field );
        if ( ydpi == 0 )
            fprintf( stderr, "?invalid ydpi\n" );
    }
    else
    {
        xdpi = atof( opttmp );
        ydpi = xdpi;
        if ( xdpi == 0 )
            return 1;
    }

    plspage( xdpi, ydpi, xwid, ywid, xoff, yoff );
    return 0;
}

//--------------------------------------------------------------------------
// opt_dev_compression()
//
//! Sets device compression
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Device compression (?).
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_dev_compression( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    PLINT comp = 0;

    comp = atoi( opt_arg );
    if ( comp == 0 )
    {
        fprintf( stderr, "?invalid compression\n" );
        return 1;
    }
    plscompression( comp );

    return 0;
}

//--------------------------------------------------------------------------
// opt_cmap0()
//
//! Sets color table 0 based on a cmap0.pal file.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Name of color map 0 .pal file.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_cmap0( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plspal0( opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_cmap1()
//
//! Sets color table 1 based on a cmap1.pal file.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Name of a color map 1 .pal file.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_cmap1( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    plspal1( opt_arg, TRUE );
    return 0;
}

//--------------------------------------------------------------------------
// opt_locale()
//
//! Make PLplot portable to all LC_NUMERIC locales.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_locale( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    char *locale;
    if ( ( locale = setlocale( LC_NUMERIC, "" ) ) )
    {
        printf( "LC_NUMERIC locale set to \"%s\"\n", locale );
    }
    else
    {
        plwarn( "Could not use invalid environment (e.g., LC_ALL, LC_NUMERIC, or LANG) to set LC_NUMERIC locale.  Falling back to LC_NUMERIC \"C\" locale instead.\n" );
        if ( !( locale = setlocale( LC_NUMERIC, "C" ) ) )
        {
            plexit( "Your platform is seriously broken.  Not even a \"C\" locale could be set." );
        }
    }
    return 0;
}

//--------------------------------------------------------------------------
// opt_eofill()
//
//! For the case where the boundary of the filled region is
//! self-intersecting, use the even-odd fill rule rather than the
//! default nonzero fill rule.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param PL_UNUSED( opt_arg ) Not used.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_eofill( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR PL_UNUSED( opt_arg ), void * PL_UNUSED( client_data ) )
{
    plsc->dev_eofill = 1;
    if ( plsc->level > 0 )
        plP_state( PLSTATE_EOFILL );
    return 0;
}

//--------------------------------------------------------------------------
// opt_mfo()
//
//! Sets the filename of the PLplot metafile that will be written.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg  Output PLplot metafile.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_mfo( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    if ( ( plsc->mf_outfile = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) ) ) == NULL )
    {
        plexit( "opt_mfo: Insufficient memory" );
    }

    strcpy( plsc->mf_outfile, opt_arg );
    return 0;
}

//--------------------------------------------------------------------------
// opt_mfi()
//
//! Sets the filename of the PLplot metafile that will be read.
//!
//! @param PL_UNUSED( opt ) Not used.
//! @param opt_arg Input PLplot metafile.
//! @param PL_UNUSED( client_data ) Not used.
//!
//! returns 0.
//!
//--------------------------------------------------------------------------

static int
opt_mfi( PLCHAR_VECTOR PL_UNUSED( opt ), PLCHAR_VECTOR opt_arg, void * PL_UNUSED( client_data ) )
{
    if ( ( plsc->mf_infile = (char *) malloc( (size_t) ( 1 + strlen( opt_arg ) ) * sizeof ( char ) ) ) == NULL )
    {
        plexit( "opt_mfi: Insufficient memory" );
    }

    strcpy( plsc->mf_infile, opt_arg );
    return 0;
}
