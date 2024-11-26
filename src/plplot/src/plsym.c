//      Point, symbol, and string plotting routines.
//      Also font management code.  See the description of plLibOpen() for
//      the search path used in finding the font files.
//
// Copyright (C) 1992 Geoffrey Furnish
// Copyright (C) 1993-2002 Maurice LeBrun
// Copyright (C) 1996 Rady Shouman
// Copyright (C) 2000-2019 Alan W. Irwin
// Copyright (C) 2001 Joao Cardoso
// Copyright (C) 2002 Vince Darley
// Copyright (C) 2003-2005 Rafael Laboissiere
// Copyright (C) 2004-2005 Andrew Roach
// Copyright (C) 2004-2011 Andrew Ross
// Copyright (C) 2005 Thomas Duck
// Copyright (C) 2006-2010 Hazen Babcock
// Copyright (C) 2009 Werner Smekal
// Copyright (C) 2010 Hezekiah M. Carty
// Copyright (C) 2015 Phil Rosenberg
// Copyright (C) 2015 jdishaw
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

//! @file
//!
//! Point-, symbol-, and string-plotting routines.
//!

#ifndef __PLSYM_H__
#define __PLSYM_H__
#define DEBUG
#define NEED_PLDEBUG

#include "plplotP.h"
#include <float.h>
#include <ctype.h>
#include "plhershey-unicode.h"

// Declarations

static short int   *fntlkup;
static short int   *fntindx;
static signed char *fntbffr;
static short int   numberfonts, numberchars;
static short int   indxleng;

static short       fontloaded = 0;
// moved to plstr.h, plsc->cfont  static PLINT font = 1;  current font

#define PLMAXSTR    300
#define STLEN       250

static const char  font_types[] = "nris";

static short       symbol_buffer[PLMAXSTR];
static signed char xygrid[STLEN];

int hershey2unicode( int in );

// Static function prototypes

static void
pldeco( short int **sym, PLINT *length, PLCHAR_VECTOR text );

static void
plchar( signed char *xygrid, PLFLT *xform, PLINT base, PLINT oline, PLINT uline,
        PLINT refx, PLINT refy, PLFLT scale, PLFLT xpmm, PLFLT ypmm,
        PLFLT *p_xorg, PLFLT *p_yorg, PLFLT *p_width );

static PLINT
plcvec( PLINT ch, signed char **xygr );

static void
plhrsh2( PLINT ch, PLINT x, PLINT y );

//--------------------------------------------------------------------------
//! Plot a glyph at the specified points.  (This function largely
//! supersedes plpoin and plsym because many[!] more glyphs are
//! accessible with plstring.)  The glyph is specified with a PLplot
//! user string.  Note that the user string is not actually limited to
//! one glyph so it is possible (but not normally useful) to plot more
//! than one glyph at the specified points with this function.  As
//! with plmtex and plptex, the user string can contain FCI escapes to
//! determine the font, UTF-8 code to determine the glyph or else
//! PLplot escapes for Hershey or unicode text to determine the glyph.
//! @param n Number of points in x and y arrays.
//! @param x Array of X coordinates of points.
//! @param y Array of Y coordinates of points.
//! @param string PLplot user string corresponding to the glyph to
//! be plotted at each of the n points.
//--------------------------------------------------------------------------

void
c_plstring( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLCHAR_VECTOR string )
{
    PLINT i;
    for ( i = 0; i < n; i++ )
    {
        c_plptex( x[i], y[i], 1., 0., 0.5, string );
    }
}

//--------------------------------------------------------------------------
//! Plot a glyph at the specified points.  (This function is largely
//! superseded by plstring which gives access to many[!] more glyphs.)
//! @param n Number of points in x and y arrays.
//! @param x Pointer to an array with X coordinates of points.
//! @param y Pointer to an array with Y coordinates of points.
//! @param code Hershey symbol code corresponding to a glyph to be
//! plotted at each of the n points.
//--------------------------------------------------------------------------

void
c_plsym( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT code )
{
    PLINT i;
    PLFLT xt, yt;

    if ( plsc->level < 3 )
    {
        plabort( "plsym: Please set up window first" );
        return;
    }
    if ( code < 0 )
    {
        plabort( "plsym: Invalid code" );
        return;
    }

    for ( i = 0; i < n; i++ )
    {
        TRANSFORM( x[i], y[i], &xt, &yt );
        plhrsh( code, plP_wcpcx( xt ), plP_wcpcy( yt ) );
    }
}

//--------------------------------------------------------------------------
//! Plot a glyph at the specified points.  (This function is largely
//! superseded by plstring which gives access to many[!] more glyphs.)
//! code=-1 means try to just draw a point.  Right now it's just a
//! move and a draw at the same place.  Not ideal, since a
//! sufficiently intelligent output device may optimize it away, or
//! there may be faster ways of doing it.  This is OK for now, though,
//! and offers a 4X speedup over drawing a Hershey font "point" (which
//! is actually diamond shaped and and therefore takes 4 strokes to
//! draw).  If 0 < code < 32, then a useful (but small subset) of
//! Hershey symbols is plotted.  If 32 <= code <= 127 the
//! corresponding printable ASCII character is plotted.
//! @param n Number of points in x and y arrays.
//! @param x Pointer to an array with X coordinates of points.
//! @param y Pointer to an array with Y coordinates of points.
//! @param code Hershey symbol code (in "ascii-indexed" form with
//! -1 <= code <= 127) corresponding to a glyph to be plotted at each
//! of the n points.
//--------------------------------------------------------------------------

void
c_plpoin( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT code )
{
    PLINT i, sym, ifont = plsc->cfont;
    PLFLT xt, yt;

    if ( plsc->level < 3 )
    {
        plabort( "plpoin: Please set up window first" );
        return;
    }
    if ( code < -1 || code > 127 )
    {
        plabort( "plpoin: Invalid code" );
        return;
    }

    if ( code == -1 )
    {
        for ( i = 0; i < n; i++ )
        {
            TRANSFORM( x[i], y[i], &xt, &yt );
            pljoin( xt, yt, xt, yt );
        }
    }
    else
    {
        if ( ifont > numberfonts )
            ifont = 1;
        sym = *( fntlkup + ( ifont - 1 ) * numberchars + code );
        // One-time diagnostic output.
        // fprintf(stdout, "plploin code, sym = %d, %d\n", code, sym);

        for ( i = 0; i < n; i++ )
        {
            TRANSFORM( x[i], y[i], &xt, &yt );
            plhrsh( sym, plP_wcpcx( xt ), plP_wcpcy( yt ) );
        }
    }
}

//--------------------------------------------------------------------------
//! Plot a glyph at the specified 3D points.  (This function is
//! largely superseded by plstring3 which gives access to many[!] more
//! glyphs.)  Set up the call to this function similar to what is done
//! for plline3.  code=-1 means try to just draw a point.  Right now
//! it's just a move and a draw at the same place.  Not ideal, since a
//! sufficiently intelligent output device may optimize it away, or
//! there may be faster ways of doing it.  This is OK for now, though,
//! and offers a 4X speedup over drawing a Hershey font "point" (which
//! is actually diamond shaped and therefore takes 4 strokes to draw).
//! If 0 < code < 32, then a useful (but small subset) of Hershey
//! symbols is plotted.  If 32 <= code <= 127 the corresponding
//! printable ASCII character is plotted.
//! @param n Number of points in x, y, and z arrays.
//! @param x Pointer to an array with X coordinates of points.
//! @param y Pointer to an array with Y coordinates of points.
//! @param z Pointer to an array with Z coordinates of points.
//! @param code Hershey symbol code (in "ascii-indexed" form with
//! -1 <= code <= 127) corresponding to a glyph to be plotted at each
//! of the n points.
//--------------------------------------------------------------------------

void
c_plpoin3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLINT code )
{
    PLINT i, sym, ifont = plsc->cfont;
    PLFLT u, v;
    PLFLT xmin, xmax, ymin, ymax, zmin, zmax, zscale;

    if ( plsc->level < 3 )
    {
        plabort( "plpoin3: Please set up window first" );
        return;
    }
    if ( code < -1 || code > 127 )
    {
        plabort( "plpoin3: Invalid code" );
        return;
    }

    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );

    if ( code == -1 )
    {
        for ( i = 0; i < n; i++ )
        {
            if ( x[i] >= xmin && x[i] <= xmax &&
                 y[i] >= ymin && y[i] <= ymax &&
                 z[i] >= zmin && z[i] <= zmax )
            {
                u = plP_wcpcx( plP_w3wcx( x[i], y[i], z[i] ) );
                v = plP_wcpcy( plP_w3wcy( x[i], y[i], z[i] ) );
                plP_movphy( (PLINT) u, (PLINT) v );
                plP_draphy( (PLINT) u, (PLINT) v );
            }
        }
    }
    else
    {
        if ( ifont > numberfonts )
            ifont = 1;
        sym = *( fntlkup + ( ifont - 1 ) * numberchars + code );

        for ( i = 0; i < n; i++ )
        {
            if ( x[i] >= xmin && x[i] <= xmax &&
                 y[i] >= ymin && y[i] <= ymax &&
                 z[i] >= zmin && z[i] <= zmax )
            {
                u = plP_wcpcx( plP_w3wcx( x[i], y[i], z[i] ) );
                v = plP_wcpcy( plP_w3wcy( x[i], y[i], z[i] ) );
                plhrsh( sym, (PLINT) u, (PLINT) v );
            }
        }
    }
}

//--------------------------------------------------------------------------
//! Plot a glyph at the specified 3D points.  (This function
//! largely supersedes plpoin3 because many[!] more glyphs are
//! accessible with plstring3).  Set up the call to this function
//! similar to what is done for plline3.  The glyph is specified with
//! a PLplot user string.  Note that the user string is not actually
//! limited to one glyph so it is possible (but not normally useful)
//! to plot more than one glyph at the specified points with this
//! function.  As with plmtex and plptex, the user string can contain
//! FCI escapes to determine the font, UTF-8 code to determine the
//! glyph or else PLplot escapes for Hershey or unicode text to
//! determine the glyph.
//! @param n Number of points in x, y, and z arrays.
//! @param x Array of X coordinates of points.
//! @param y Array of Y coordinates of points.
//! @param z Array of Z coordinates of points.
//! @param string PLplot user string corresponding to the glyph to
//! be plotted at each of the n points.
//--------------------------------------------------------------------------

void
c_plstring3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLCHAR_VECTOR string )
{
    PLINT i;
    PLFLT u, v;
    PLFLT xmin, xmax, ymin, ymax, zmin, zmax, zscale;

    if ( plsc->level < 3 )
    {
        plabort( "plstring3: Please set up window first" );
        return;
    }

    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );

    for ( i = 0; i < n; i++ )
    {
        if ( x[i] >= xmin && x[i] <= xmax &&
             y[i] >= ymin && y[i] <= ymax &&
             z[i] >= zmin && z[i] <= zmax )
        {
            u = plP_w3wcx( x[i], y[i], z[i] );
            v = plP_w3wcy( x[i], y[i], z[i] );
            c_plptex( u, v, 1., 0., 0.5, string );
        }
    }
}

//--------------------------------------------------------------------------
// void plhrsh(PLINT ch, PLINT x, PLINT y)
//    PLINT ch - hershey code to plot
//    PLINT x - device-world x coordinate of hershey character
//    PLINT y - device-world y coordinate of hershey character
//
//  Writes the Hershey symbol "ch" centred at the physical coordinate (x,y).
//  This function is now just a "spoof" front end to the old plhersh,
//  which has now been renamed to plhrsh2(). All this function does is
//  decide whether or not we should render natively as unicode, and then
//  convert between hershey and unicode.
//
//  If the function KNOWS there isn't a unicode equivalent, then it will
//  try to render it as a hershey font. Understandably, this might make
//  testing out the unicode functions a little tricky, so if you want
//  to disable this behaviour, recompile with PL_TEST_FOR_MISSING_GLYPHS
//  defined.
//--------------------------------------------------------------------------

void
plhrsh( PLINT ch, PLINT x, PLINT y )
{
    EscText   args;
    int       idx;
    PLUNICODE unicode_char;

    // Check to see if the device understands unicode and wants to draw
    // symbols.
    //
    if ( ( plsc->dev_text ) && ( plsc->dev_unicode ) && ( !plsc->dev_hrshsym ) )
    {
        // Get the index in the lookup table and the unicode character
        idx = plhershey2unicode( ch );
        if ( 0 <= idx && idx <= number_of_entries_in_hershey_to_unicode_table )
            unicode_char = hershey_to_unicode_lookup_table[idx].Unicode;
        else
            unicode_char = (PLUNICODE) 0x00;

        pldebug( "plhrsh", "ch, idx, unicode_char = %d, %d, %#x\n", ch, idx, unicode_char );
        //
        //  Test to see if there is a defined unicode glyph for this hershey
        //  code; if there isn't, then we pass the glyph to plhersh, and have
        //  it rendered the old fashioned way.
        //  Otherwise, we let the driver render it as unicode
        //

        if ( ( unicode_char == 0 ) || ( idx == -1 ) )
        {
#ifndef PL_TEST_FOR_MISSING_GLYPHS
            plhrsh2( ch, x, y );
#endif
        }
        else
        {
            PLUNICODE plhrsh_unicode_buffer[3], fci;
            PLFLT     xform[] = { 1.0, 0.0, 0.0, 1.0 };
            char      esc;

            // Get the current escape character
            plgesc( &esc );

            // Setup to render a unicode character
            args.text_type    = PL_STRING_SYMBOL;
            args.unicode_char = unicode_char;
            if ( 0 <= idx && idx <= number_of_entries_in_hershey_to_unicode_table )
                args.font_face = hershey_to_unicode_lookup_table[idx].Font;
            else
                // Unknown font face indicated by 0 value.
                args.font_face = 0;
            // Comment out to fix problem with ps, psttf drivers
            //args.base = 1;
            args.base   = 0;
            args.just   = 0.5;
            args.xform  = xform;
            args.x      = x;
            args.y      = y;
            args.string = NULL;
            args.symbol = ch;

            // Get address of the unicode buffer (even though it is
            // currently static)
            args.unicode_array       = &plhrsh_unicode_buffer[0];
            args.unicode_array_len   = 1;
            plhrsh_unicode_buffer[0] = unicode_char;
            // watch out for escape character and unescape it by appending
            // one extra.
            if ( unicode_char == (PLUNICODE) esc )
            {
                args.unicode_array_len   = 2;
                plhrsh_unicode_buffer[1] = unicode_char;
            }

            // No need to change font back since only one character.

            // Swap the sym and chr information so that the text
            // rendering (which uses chrht and chrdef) will
            // render the symbol correctly
            plsc->original_chrht  = plsc->chrht;
            plsc->original_chrdef = plsc->chrdef;
            plsc->chrht           = plsc->symht;
            plsc->chrdef          = plsc->symdef;

            if ( plsc->alt_unicode )
            {
                // Character at a time method
                plgfci( &fci );
                args.n_fci  = fci;
                args.n_char = unicode_char;

                plP_esc( PLESC_BEGIN_TEXT, &args );
                plP_esc( PLESC_TEXT_CHAR, &args );
                plP_esc( PLESC_END_TEXT, &args );
            }
            else
            {
                // "array method"
                plP_esc( PLESC_HAS_TEXT, &args );
            }

            plsc->chrht  = plsc->original_chrht;
            plsc->chrdef = plsc->original_chrdef;
        }
    }
    else
    {
        plhrsh2( ch, x, y );
    }
}

//--------------------------------------------------------------------------
// void plhrsh2()
//
// Writes the Hershey symbol "ch" centred at the physical coordinate (x,y).
//--------------------------------------------------------------------------

static void
plhrsh2( PLINT ch, PLINT x, PLINT y )
{
    PLINT       cx, cy, k, penup, style;
    signed char *vxygrid = 0;
    PLFLT       scale, xscale, yscale;
    PLINT       llx[STLEN], lly[STLEN], l = 0;

    penup = 1;
    scale = 0.05 * plsc->symht;

    if ( !plcvec( ch, &vxygrid ) )
    {
        plP_movphy( x, y );
        return;
    }

// Line style must be continuous

    style     = plsc->nms;
    plsc->nms = 0;

// Compute how many physical pixels correspond to a character pixel

    xscale = scale * plsc->xpmm;
    yscale = scale * plsc->ypmm;

    k = 4;
    for (;; )
    {
        cx = vxygrid[k++];
        cy = vxygrid[k++];
        if ( cx == 64 && cy == 64 )
        {
            if ( l )
            {
                plP_draphy_poly( llx, lly, l );
                l = 0;
            }
            plP_movphy( x, y );
            plsc->nms = style;
            return;
        }
        else if ( cx == 64 && cy == 0 )
            penup = 1;
        else
        {
            if ( penup == 1 )
            {
                if ( l )
                {
                    plP_draphy_poly( llx, lly, l );
                    l = 0;
                }
                llx[l]   = ROUND( x + xscale * cx );
                lly[l++] = ROUND( y + yscale * cy );
                plP_movphy( llx[l - 1], lly[l - 1] );
                penup = 0;
            }
            else
            {
                llx[l]   = ROUND( x + xscale * cx );
                lly[l++] = ROUND( y + yscale * cy );
            }
        }
    }
}

//--------------------------------------------------------------------------
// void pllab()
//
// Simple routine for labelling graphs.
//--------------------------------------------------------------------------

void
c_pllab( PLCHAR_VECTOR xlabel, PLCHAR_VECTOR ylabel, PLCHAR_VECTOR tlabel )
{
    if ( plsc->level < 2 )
    {
        plabort( "pllab: Please set up viewport first" );
        return;
    }

    plmtex( "t", (PLFLT) 2.0, (PLFLT) 0.5, (PLFLT) 0.5, tlabel );
    plmtex( "b", (PLFLT) 3.2, (PLFLT) 0.5, (PLFLT) 0.5, xlabel );
    plmtex( "l", (PLFLT) 5.0, (PLFLT) 0.5, (PLFLT) 0.5, ylabel );
}

//--------------------------------------------------------------------------
// void plmtex()
//
// Prints out "text" at specified position relative to viewport
// (may be inside or outside)
//
// side	String which is one of the following:
//     B or b  :  Bottom of viewport
//     T or t  :  Top of viewport
//     BV or bv : Bottom of viewport, vertical text
//     TV or tv : Top of viewport, vertical text
//     L or l  :  Left of viewport
//     R or r  :  Right of viewport
//     LV or lv : Left of viewport, vertical text
//     RV or rv : Right of viewport, vertical text
//
// disp Displacement from specified edge of viewport, measured outwards from
//	the viewport in units of the current character height. The
//	centerlines of the characters are aligned with the specified
//	position.
//
// pos	Position of the reference point of the string relative to the
//	viewport edge, ranging from 0.0 (left-hand edge) to 1.0 (right-hand
//	edge)
//
// just	Justification of string relative to reference point
//	just = 0.0 => left hand edge of string is at reference
//	just = 1.0 => right hand edge of string is at reference
//	just = 0.5 => center of string is at reference
//--------------------------------------------------------------------------

void
c_plmtex( PLCHAR_VECTOR side, PLFLT disp, PLFLT pos, PLFLT just,
          PLCHAR_VECTOR text )
{
    PLINT clpxmi, clpxma, clpymi, clpyma;
    PLINT vert, refx, refy, x, y;
    PLFLT xdv, ydv, xmm, ymm, refxmm, refymm, shift, xform[4];
    PLFLT chrdef, chrht;
    PLFLT dispx, dispy;

    if ( plsc->level < 2 )
    {
        plabort( "plmtex: Please set up viewport first" );
        return;
    }

// Open clip limits to subpage limits

    plP_gclp( &clpxmi, &clpxma, &clpymi, &clpyma ); // get and store current clip limits
    plP_sclp( plsc->sppxmi, plsc->sppxma, plsc->sppymi, plsc->sppyma );

    if ( plP_stindex( side, "BV" ) != -1 || plP_stindex( side, "bv" ) != -1 )
    {
        vert  = 1;
        xdv   = plsc->vpdxmi + ( plsc->vpdxma - plsc->vpdxmi ) * pos;
        ydv   = plsc->vpdymi;
        dispx = 0;
        dispy = -disp;
    }
    else if ( plP_stindex( side, "TV" ) != -1 || plP_stindex( side, "tv" ) != -1 )
    {
        vert  = 1;
        xdv   = plsc->vpdxmi + ( plsc->vpdxma - plsc->vpdxmi ) * pos;
        ydv   = plsc->vpdyma;
        dispx = 0;
        dispy = disp;
    }
    else if ( plP_stsearch( side, 'b' ) )
    {
        vert  = 0;
        xdv   = plsc->vpdxmi + ( plsc->vpdxma - plsc->vpdxmi ) * pos;
        ydv   = plsc->vpdymi;
        dispx = 0;
        dispy = -disp;
    }
    else if ( plP_stsearch( side, 't' ) )
    {
        vert  = 0;
        xdv   = plsc->vpdxmi + ( plsc->vpdxma - plsc->vpdxmi ) * pos;
        ydv   = plsc->vpdyma;
        dispx = 0;
        dispy = disp;
    }
    else if ( plP_stindex( side, "LV" ) != -1 || plP_stindex( side, "lv" ) != -1 )
    {
        vert  = 0;
        xdv   = plsc->vpdxmi;
        ydv   = plsc->vpdymi + ( plsc->vpdyma - plsc->vpdymi ) * pos;
        dispx = -disp;
        dispy = 0;
    }
    else if ( plP_stindex( side, "RV" ) != -1 || plP_stindex( side, "rv" ) != -1 )
    {
        vert  = 0;
        xdv   = plsc->vpdxma;
        ydv   = plsc->vpdymi + ( plsc->vpdyma - plsc->vpdymi ) * pos;
        dispx = disp;
        dispy = 0;
    }
    else if ( plP_stsearch( side, 'l' ) )
    {
        vert  = 1;
        xdv   = plsc->vpdxmi;
        ydv   = plsc->vpdymi + ( plsc->vpdyma - plsc->vpdymi ) * pos;
        dispx = -disp;
        dispy = 0;
    }
    else if ( plP_stsearch( side, 'r' ) )
    {
        vert  = 1;
        xdv   = plsc->vpdxma;
        ydv   = plsc->vpdymi + ( plsc->vpdyma - plsc->vpdymi ) * pos;
        dispx = disp;
        dispy = 0;
    }
    else
    {
        plP_sclp( clpxmi, clpxma, clpymi, clpyma ); // restore initial clip limits
        return;
    }

// Transformation matrix

    if ( vert != 0 )
    {
        xform[0] = 0.0;
        xform[1] = -1.0;
        xform[2] = 1.0;
        xform[3] = 0.0;
    }
    else
    {
        xform[0] = 1.0;
        xform[1] = 0.0;
        xform[2] = 0.0;
        xform[3] = 1.0;
    }

// Convert to physical units (mm) and compute shifts

    plgchr( &chrdef, &chrht );
    shift = ( just == 0.0 ) ? 0.0 : plstrl( text ) * just;

    xmm    = plP_dcmmx( xdv ) + dispx * chrht;
    ymm    = plP_dcmmy( ydv ) + dispy * chrht;
    refxmm = xmm - shift * xform[0];
    refymm = ymm - shift * xform[2];

// Convert to device units (pixels) and call text plotter

    x    = plP_mmpcx( xmm );
    y    = plP_mmpcy( ymm );
    refx = plP_mmpcx( refxmm );
    refy = plP_mmpcy( refymm );

    plP_text( 0, just, xform, x, y, refx, refy, text );
    plP_sclp( clpxmi, clpxma, clpymi, clpyma ); // restore clip limits
}

//--------------------------------------------------------------------------
// void plptex()
//
// Prints out "text" at world cooordinate (wx,wy). The text may be
// at any angle "angle" relative to the horizontal. The parameter
// "just" adjusts the horizontal justification of the string:
//	just = 0.0 => left hand edge of string is at (wx,wy)
//	just = 1.0 => right hand edge of string is at (wx,wy)
//	just = 0.5 => center of string is at (wx,wy) etc.
//--------------------------------------------------------------------------

void
c_plptex( PLFLT wx, PLFLT wy, PLFLT dx, PLFLT dy, PLFLT just, PLCHAR_VECTOR text )
{
    PLINT x, y, refx, refy;
    PLFLT xdv, ydv, xmm, ymm, refxmm, refymm, shift, cc, ss;
    PLFLT xform[4], diag;
    PLFLT chrdef, chrht;
    PLFLT dispx, dispy;
    PLFLT wxt, wyt, dxt, dyt;

    if ( plsc->level < 3 )
    {
        plabort( "plptex: Please set up window first" );
        return;
    }

    // Transform both the origin and offset values
    TRANSFORM( wx, wy, &wxt, &wyt );
    TRANSFORM( wx + dx, wy + dy, &dxt, &dyt );
    dxt = dxt - wxt;
    dyt = dyt - wyt;
    if ( dxt == 0.0 && dyt == 0.0 )
    {
        dxt = 1.0;
        dyt = 0.0;
    }

    cc   = plsc->wmxscl * dxt;
    ss   = plsc->wmyscl * dyt;
    diag = sqrt( cc * cc + ss * ss );
    cc  /= diag;
    ss  /= diag;

    xform[0] = cc;
    xform[1] = -ss;
    xform[2] = ss;
    xform[3] = cc;

    xdv = plP_wcdcx( wxt );
    ydv = plP_wcdcy( wyt );

    dispx = 0.;
    dispy = 0.;

// Convert to physical units (mm) and compute shifts

    plgchr( &chrdef, &chrht );
    shift = ( just == 0.0 ) ? 0.0 : plstrl( text ) * just;

    xmm    = plP_dcmmx( xdv ) + dispx * chrht;
    ymm    = plP_dcmmy( ydv ) + dispy * chrht;
    refxmm = xmm - shift * xform[0];
    refymm = ymm - shift * xform[2];

    x    = plP_mmpcx( xmm );
    y    = plP_mmpcy( ymm );
    refx = plP_mmpcx( refxmm );
    refy = plP_mmpcy( refymm );

    plP_text( 0, just, xform, x, y, refx, refy, text );
}

//--------------------------------------------------------------------------
// void plstr()
//
// Prints out a "string" at reference position with physical coordinates
// (refx,refy). The coordinates of the vectors defining the string are
// passed through the linear mapping defined by the 2 x 2 matrix xform()
// before being plotted.  The reference position is at the left-hand edge of
// the string. If base = 1, it is aligned with the baseline of the string.
// If base = 0, it is aligned with the center of the character box.
//
// Note, all calculations are done in terms of millimetres. These are scaled
// as necessary before plotting the string on the page.
//--------------------------------------------------------------------------

void
plstr( PLINT base, PLFLT *xform, PLINT refx, PLINT refy, PLCHAR_VECTOR string )
{
    short int   *symbol;
    signed char *vxygrid = 0;

    PLINT       ch, i, length, level = 0, style, oline = 0, uline = 0;
    PLFLT       width = 0., xorg = 0., yorg = 0., def, ht, dscale, scale;
    PLFLT       old_sscale, sscale, old_soffset, soffset;

    plgchr( &def, &ht );
    dscale = 0.05 * ht;
    scale  = dscale;

// Line style must be continuous

    style     = plsc->nms;
    plsc->nms = 0;

    pldeco( &symbol, &length, string );

    for ( i = 0; i < length; i++ )
    {
        ch = symbol[i];
        if ( ch == -1 )   // superscript
        {
            plP_script_scale( TRUE, &level,
                &old_sscale, &sscale, &old_soffset, &soffset );
            yorg  = 16.0 * dscale * soffset;
            scale = dscale * sscale;
        }
        else if ( ch == -2 )   // subscript
        {
            plP_script_scale( FALSE, &level,
                &old_sscale, &sscale, &old_soffset, &soffset );
            yorg  = -16.0 * dscale * soffset;
            scale = dscale * sscale;
        }
        else if ( ch == -3 ) // back-char
            xorg -= width * scale;
        else if ( ch == -4 ) // toogle overline
            oline = !oline;
        else if ( ch == -5 ) // toogle underline
            uline = !uline;
        else
        {
            if ( plcvec( ch, &vxygrid ) )
                plchar( vxygrid, xform, base, oline, uline, refx, refy, scale,
                    plsc->xpmm, plsc->ypmm, &xorg, &yorg, &width );
        }
    }
    plsc->nms = style;
}

//--------------------------------------------------------------------------
// plchar()
//
// Plots out a given stroke font character.
//--------------------------------------------------------------------------

static void
plchar( signed char *vxygrid, PLFLT *xform, PLINT base, PLINT oline, PLINT uline,
        PLINT refx, PLINT refy, PLFLT scale, PLFLT xpmm, PLFLT ypmm,
        PLFLT *p_xorg, PLFLT *p_yorg, PLFLT *p_width )
{
    PLINT xbase, ybase, ydisp, lx, ly, cx, cy;
    PLINT k, penup;
    PLFLT x, y;
    PLINT llx[STLEN], lly[STLEN], l = 0;

    xbase    = vxygrid[2];
    *p_width = vxygrid[3] - xbase;
    if ( base == 0 )
    {
        ybase = 0;
        ydisp = vxygrid[0];
    }
    else
    {
        ybase = vxygrid[0];
        ydisp = 0;
    }
    k     = 4;
    penup = 1;

    for (;; )
    {
        cx = vxygrid[k++];
        cy = vxygrid[k++];
        if ( cx == 64 && cy == 64 )
        {
            if ( l )
            {
                plP_draphy_poly( llx, lly, l );
                l = 0;
            }
            break;
        }
        if ( cx == 64 && cy == 0 )
        {
            if ( l )
            {
                plP_draphy_poly( llx, lly, l );
                l = 0;
            }
            penup = 1;
        }
        else
        {
            x  = *p_xorg + ( cx - xbase ) * scale;
            y  = *p_yorg + ( cy - ybase ) * scale;
            lx = refx + ROUND( xpmm * ( xform[0] * x + xform[1] * y ) );
            ly = refy + ROUND( ypmm * ( xform[2] * x + xform[3] * y ) );
            if ( penup == 1 )
            {
                if ( l )
                {
                    plP_draphy_poly( llx, lly, l );
                    l = 0;
                }
                llx[l]   = lx;
                lly[l++] = ly; // store 1st point !
                plP_movphy( lx, ly );
                penup = 0;
            }
            else
            {
                llx[l]   = lx;
                lly[l++] = ly;
            }
        }
    }

    if ( oline )
    {
        x  = *p_xorg;
        y  = *p_yorg + ( 30 + ydisp ) * scale;
        lx = refx + ROUND( xpmm * ( xform[0] * x + xform[1] * y ) );
        ly = refy + ROUND( ypmm * ( xform[2] * x + xform[3] * y ) );
        plP_movphy( lx, ly );
        x  = *p_xorg + *p_width * scale;
        lx = refx + ROUND( xpmm * ( xform[0] * x + xform[1] * y ) );
        ly = refy + ROUND( ypmm * ( xform[2] * x + xform[3] * y ) );
        plP_draphy( lx, ly );
    }
    if ( uline )
    {
        x  = *p_xorg;
        y  = *p_yorg + ( -5 + ydisp ) * scale;
        lx = refx + ROUND( xpmm * ( xform[0] * x + xform[1] * y ) );
        ly = refy + ROUND( ypmm * ( xform[2] * x + xform[3] * y ) );
        plP_movphy( lx, ly );
        x  = *p_xorg + *p_width * scale;
        lx = refx + ROUND( xpmm * ( xform[0] * x + xform[1] * y ) );
        ly = refy + ROUND( ypmm * ( xform[2] * x + xform[3] * y ) );
        plP_draphy( lx, ly );
    }
    *p_xorg = *p_xorg + *p_width * scale;
}

//--------------------------------------------------------------------------
// PLFLT plstrl()
//
// Computes the length of a string in mm, including escape sequences.
//--------------------------------------------------------------------------

PLFLT
plstrl( PLCHAR_VECTOR string )
{
    short int   *symbol;
    signed char *vxygrid = 0;
    PLINT       ch, i, length, level = 0;
    PLFLT       width = 0., xorg = 0., dscale, scale, def, ht;

    // If the driver will compute string lengths for us then we ask
    // it do so by setting get_string_length flag. When this is set
    // the driver will set the string_length variable instead of
    // actually rendering the string.
    // Note we must make sure that this text command does not end up
    // in the buffer.
    //
    // TODO:
    //   Is plmtex the best string diplay routine to use?
    //   Will this work for buffered plots?

    if ( plsc->has_string_length )
    {
        PLINT plbuf_write = plsc->plbuf_write;
        plsc->plbuf_write       = FALSE;
        plsc->get_string_length = 1;
        c_plmtex( "t", 0.0, 0.0, 0.0, string );
        plsc->get_string_length = 0;
        plsc->plbuf_write       = plbuf_write;
        return (PLFLT) plsc->string_length;
    }


    plgchr( &def, &ht );
    dscale = 0.05 * ht;
    scale  = dscale;
    pldeco( &symbol, &length, string );

    for ( i = 0; i < length; i++ )
    {
        ch = symbol[i];
        if ( ch == -1 )
        {
            level++;
            scale = dscale * pow( 0.75, (double) ABS( level ) );
        }
        else if ( ch == -2 )
        {
            level--;
            scale = dscale * pow( 0.75, (double) ABS( level ) );
        }
        else if ( ch == -3 )
            xorg -= width * scale;
        else if ( ch == -4 || ch == -5 )
            ;
        else
        {
            if ( plcvec( ch, &vxygrid ) )
            {
                width = vxygrid[3] - vxygrid[2];
                xorg += width * scale;
            }
        }
    }
    return (PLFLT) xorg;
}

//--------------------------------------------------------------------------
// PLINT plcvec()
//
// Gets the character digitisation of Hershey table entry "char".
// Returns 1 if there is a valid entry.
//--------------------------------------------------------------------------

static PLINT
plcvec( PLINT ch, signed char **xygr )
{
    PLINT       k = 0, ib;
    signed char x, y;

    ch--;
    if ( ch < 0 || ch >= indxleng )
        return (PLINT) 0;
    ib = fntindx[ch] - 2;
    if ( ib == -2 )
        return (PLINT) 0;

    do
    {
        ib++;
        x           = fntbffr[2 * ib];
        y           = fntbffr[2 * ib + 1];
        xygrid[k++] = x;
        xygrid[k++] = y;
    } while ( ( x != 64 || y != 64 ) && k <= ( STLEN - 2 ) );

    if ( k == ( STLEN - 1 ) )
    {
        // This is bad if we get here
        xygrid[k] = 64;
        xygrid[k] = 64;
    }

    *xygr = xygrid;
    return (PLINT) 1;
}

//--------------------------------------------------------------------------
// void pldeco()
//
// Decode a character string, and return an array of float integer symbol
// numbers. This routine is responsible for interpreting all escape sequences.
// At present the following escape sequences are defined (the letter following
// the <esc> may be either upper or lower case):
//
// <esc>u	: up one level (returns -1)
// <esc>d	: down one level (returns -2)
// <esc>b	: backspace (returns -3)
// <esc>+	: toggles overline mode (returns -4)
// <esc>-	: toggles underline mode (returns -5)
// <esc><esc>	: <esc>
// <esc>gx	: greek letter corresponding to roman letter x
// <esc>fn	: switch to Normal font
// <esc>fr	: switch to Roman font
// <esc>fi	: switch to Italic font
// <esc>fs	: switch to Script font
// <esc>(nnn)	: Hershey symbol number nnn (any number of digits)
//
// The escape character defaults to '#', but can be changed to any of
// [!#$%&*@^~] via a call to plsesc.
//--------------------------------------------------------------------------

static void
pldeco( short int **symbol, PLINT *length, PLCHAR_VECTOR text )
{
    PLINT     ch, ifont = plsc->cfont, ig, j = 0, lentxt = (PLINT) strlen( text );
    char      test, esc;
    short int *sym = symbol_buffer;

// Initialize parameters.

    *length = 0;
    *symbol = symbol_buffer;
    plgesc( &esc );
    if ( ifont > numberfonts )
        ifont = 1;

// Get next character; treat non-printing characters as spaces.

    while ( j < lentxt )
    {
        if ( *length >= PLMAXSTR )
            return;
        test = text[j++];
        ch   = test;
        if ( ch < 0 || ch > 175 )
            ch = 32;

        // Test for escape sequence (#)

        if ( ch == esc && ( lentxt - j ) >= 1 )
        {
            test = text[j++];
            if ( test == esc )
                sym[( *length )++] = *( fntlkup + ( ifont - 1 ) * numberchars + ch );

            else if ( test == 'u' || test == 'U' )
                sym[( *length )++] = -1;

            else if ( test == 'd' || test == 'D' )
                sym[( *length )++] = -2;

            else if ( test == 'b' || test == 'B' )
                sym[( *length )++] = -3;

            else if ( test == '+' )
                sym[( *length )++] = -4;

            else if ( test == '-' )
                sym[( *length )++] = -5;

            else if ( test == '(' )
            {
                sym[*length] = 0;
                while ( '0' <= text[j] && text[j] <= '9' )
                {
                    sym[*length] = (short) ( (int) sym[*length] * 10 + text[j] - '0' );
                    j++;
                }
                ( *length )++;
                if ( text[j] == ')' )
                    j++;
            }
            else if ( test == 'f' || test == 'F' )
            {
                test  = text[j++];
                ifont = 1 + plP_strpos( font_types,
                    isupper( test ) ? tolower( test ) : test );
                if ( ifont == 0 || ifont > numberfonts )
                    ifont = 1;
            }
            else if ( test == 'g' || test == 'G' )
            {
                test = text[j++];
                ig   = plP_strpos( plP_greek_mnemonic, test ) + 1;
                // This accesses the Hershey glyphs using the same
                // "ascii" index as plpoin.  So the order of the Greek
                // glyphs in this case depends on the subhersh[0-3]
                // indices in fonts/font11.c which for lower-case epsilon,
                // theta, and phi substitutes (684, 685, and 686) for
                // (631, 634, and 647) in the compact case and (2184,
                // 2185, and 2186) for (2131, 2134, and 2147) in the
                // extended case.
                sym[( *length )++] =
                    *( fntlkup + ( ifont - 1 ) * numberchars + 127 + ig );
            }
            else
            {
                ;
            }
        }
        else
        {
            // Decode character.
            // >>PC<< removed increment from following expression to fix
            // compiler bug

            sym[( *length )] = *( fntlkup + ( ifont - 1 ) * numberchars + ch );
            ( *length )++;
        }
    }
}

//--------------------------------------------------------------------------
// PLINT plP_strpos()
//
// Searches string str for first occurence of character chr.  If found
// the position of the character in the string is returned (the first
// character has position 0).  If the character is not found a -1 is
// returned.
//--------------------------------------------------------------------------

PLINT
plP_strpos( PLCHAR_VECTOR str, int chr )
{
    char *temp;

    if ( ( temp = strchr( str, chr ) ) )
        return (PLINT) ( temp - str );
    else
        return (PLINT) -1;
}

//--------------------------------------------------------------------------
// PLINT plP_stindex()
//
// Similar to strpos, but searches for occurence of string str2.
//--------------------------------------------------------------------------

PLINT
plP_stindex( PLCHAR_VECTOR str1, PLCHAR_VECTOR str2 )
{
    PLINT base, str1ind, str2ind;

    for ( base = 0; *( str1 + base ) != '\0'; base++ )
    {
        for ( str1ind = base, str2ind = 0; *( str2 + str2ind ) != '\0' &&
              *( str2 + str2ind ) == *( str1 + str1ind ); str1ind++, str2ind++ )
            ;

        if ( *( str2 + str2ind ) == '\0' )
            return (PLINT) base;
    }
    return (PLINT) -1;          // search failed
}

//--------------------------------------------------------------------------
// PLBOOL plP_stsearch()
//
// Searches string str for character chr (case insensitive).
//--------------------------------------------------------------------------

PLBOOL
plP_stsearch( PLCHAR_VECTOR str, int chr )
{
    if ( strchr( str, chr ) )
        return TRUE;
    else if ( strchr( str, toupper( chr ) ) )
        return TRUE;
    else
        return FALSE;
}

//--------------------------------------------------------------------------
//! Calculate scale of font size and scale of magnitude of vertical
//! offset associated with superscripts and subscripts.
//! Notes on arguments: ifupper must be either TRUE or FALSE on every
//! call to plP_script_scale.  The contents of the location pointed to
//! by the level pointer must be zero on the first call to
//! plP_script_scale, but not modified externally from then on.  The
//! contents of the locations pointed to by all other pointer
//! arguments are initialized internally, and should not be modified
//! externally.
//!
//! @param ifupper Value which is TRUE if superscripting, i.e., if
//! incrementing the previous level, and FALSE if subscripting, i.e.,
//! decrementing the previous level.
//! @param level Pointer to a location which contains the value of the
//! superscript/subscript level.  That value is 0, +-1, +-2, etc., for
//! no superscript/subscript, the first level of
//! superscript/subscript, the second level of superscript/subscript,
//! etc.  Before the call the value is the old level, and after the
//! call the value will be incremented (ifupper TRUE) or decremented
//! (ifupper FALSE) from the previous value.
//! @param old_scale A pointer to a location that contains after the
//! call the old font size scale value.
//! @param scale A pointer to a location that contains after the call
//! the font size scale value.  This value is 0.75^{|level|} where
//! |level| is the magnitude of the value of the superscript/subscript
//! level after the call.
//! @param old_offset A pointer to a location that contains after the
//! call the old value of the magnitude of the superscript/subscript
//! offset.
//! @param offset A pointer to a location that contains after the call
//! the value of the magnitude of the superscript/subscript offset
//! which is zero for |level|=0 and sum_{i=1}^{i=|level|} 0.75^{i-1},
//! otherwise.

void
plP_script_scale( PLBOOL ifupper, PLINT *level,
                  PLFLT *old_scale, PLFLT *scale,
                  PLFLT *old_offset, PLFLT *offset )
{
    if ( *level == 0 )
    {
        *old_scale  = 1.;
        *old_offset = 0.;
    }
    else
    {
        *old_scale  = *scale;
        *old_offset = *offset;
    }
    if ( ( *level >= 0 && ifupper ) || ( *level <= 0 && !ifupper ) )
    {
        // If superscript of subscript moves further away from centerline....
        *scale  = 0.75 * *old_scale;
        *offset = *old_offset + *old_scale;
    }
    else
    {
        // If superscript of subscript moves closer to centerline....
        *scale  = *old_scale / 0.75;
        *offset = *old_offset - *scale;
    }
    if ( ifupper )
        ( *level )++;
    else
        ( *level )--;
}

//--------------------------------------------------------------------------
// void c_plfont(ifont)
//
// Sets the global font flag to 'ifont'.
//--------------------------------------------------------------------------

void
c_plfont( PLINT ifont )
{
    PLUNICODE fci = PL_FCI_MARK;
    if ( plsc->level < 1 )
    {
        plabort( "plfont: Please call plinit first" );
        return;
    }
    if ( ifont < 1 || ifont > 4 )
    {
        plabort( "plfont: Invalid font" );
        return;
    }

    plsc->cfont = ifont;

    // Provide some degree of forward compatibility if dealing with
    // unicode font. But better procedure is to call plsfci directly rather
    // than using this lame Hershey font interface.
    //
    switch ( ifont )
    {
    case 1:
        // normal = (medium, upright, sans serif)
        plP_hex2fci( PL_FCI_SANS, PL_FCI_FAMILY, &fci );
        plsfci( fci );
        break;
    // roman = (medium, upright, serif)
    case 2:
        plP_hex2fci( PL_FCI_SERIF, PL_FCI_FAMILY, &fci );
        plsfci( fci );
        break;
    // italic = (medium, italic, serif)
    case 3:
        plP_hex2fci( PL_FCI_ITALIC, PL_FCI_STYLE, &fci );
        plP_hex2fci( PL_FCI_SERIF, PL_FCI_FAMILY, &fci );
        plsfci( fci );
        break;
    // script = (medium, upright, script)
    case 4:
        plP_hex2fci( PL_FCI_SCRIPT, PL_FCI_FAMILY, &fci );
        plsfci( fci );
        break;
    }
}

//--------------------------------------------------------------------------
// void plfntld(fnt)
//
// Loads either the standard or extended font.
//--------------------------------------------------------------------------

void
plfntld( PLINT fnt )
{
    static PLINT charset;
    short        bffrleng;
    PDFstrm      *pdfs;

    if ( fontloaded && ( charset == fnt ) )
        return;

    plfontrel();
    fontloaded = 1;
    charset    = fnt;

    if ( fnt )
        pdfs = plLibOpenPdfstrm( PL_XFONT );
    else
        pdfs = plLibOpenPdfstrm( PL_SFONT );

    if ( pdfs == NULL )
        plexit( "Unable to either (1) open/find or (2) allocate memory for the font file" );

// Read fntlkup[]

    pdf_rd_2bytes( pdfs, (U_SHORT *) &bffrleng );
    numberfonts = bffrleng / 256;
    numberchars = bffrleng & 0xff;
    bffrleng    = (short) ( numberfonts * numberchars );
    fntlkup     = (short int *) malloc( (size_t) bffrleng * sizeof ( short int ) );
    if ( !fntlkup )
        plexit( "plfntld: Out of memory while allocating font buffer." );

    pdf_rd_2nbytes( pdfs, (U_SHORT *) fntlkup, bffrleng );

// Read fntindx[]

    pdf_rd_2bytes( pdfs, (U_SHORT *) &indxleng );
    fntindx = (short int *) malloc( (size_t) indxleng * sizeof ( short int ) );
    if ( !fntindx )
        plexit( "plfntld: Out of memory while allocating font buffer." );

    pdf_rd_2nbytes( pdfs, (U_SHORT *) fntindx, indxleng );

// Read fntbffr[]
// Since this is an array of char, there are no endian problems

    pdf_rd_2bytes( pdfs, (U_SHORT *) &bffrleng );
    fntbffr = (signed char *) malloc( 2 * (size_t) bffrleng * sizeof ( signed char ) );
    if ( !fntbffr )
        plexit( "plfntld: Out of memory while allocating font buffer." );

#if PLPLOT_USE_TCL_CHANNELS
    pdf_rdx( fntbffr, sizeof ( signed char ) * (size_t) ( 2 * bffrleng ), pdfs );
#else
    plio_fread( (void *) fntbffr, (size_t) sizeof ( signed char ),
        (size_t) ( 2 * bffrleng ), pdfs->file );
#endif

// Done

    pdf_close( pdfs );
}

//--------------------------------------------------------------------------
// void plfontrel()
//
// Release memory for fonts.
//--------------------------------------------------------------------------

void
plfontrel( void )
{
    if ( fontloaded )
    {
        free_mem( fntindx )
        free_mem( fntbffr )
        free_mem( fntlkup )
        fontloaded = 0;
    }
}

//--------------------------------------------------------------------------
//  int plhershey2unicode ( int in )
//
//  Function searches for in, the input hershey code, in a lookup table and
//  returns the corresponding index in that table.
//  Using this index you can work out the unicode equivalent as well as
//  the closest approximate to the font-face. If the returned index is
//  -1 then no match was possible.
//
//  Two versions of the function exist, a simple linear search version,
//  and a more complex, but significantly faster, binary search version.
//  If there seem to be problems with the binary search method, the brain-dead
//  linear search can be enabled by defining SIMPLE_BUT_SAFE_HERSHEY_LOOKUP
//  at compile time.
//--------------------------------------------------------------------------

int plhershey2unicode( int in )
{
#ifdef SIMPLE_BUT_SAFE_HERSHEY_LOOKUP
    int ret = -1;
    int i;

    for ( i = 0; ( i < number_of_entries_in_hershey_to_unicode_table ) && ( ret == -1 ); i++ )
    {
        if ( hershey_to_unicode_lookup_table[i].Hershey == in )
            ret = i;
    }

    return ( ret );

#else

    int jlo = -1, jmid, jhi = number_of_entries_in_hershey_to_unicode_table;
    while ( jhi - jlo > 1 )
    {
        // Note that although jlo or jhi can be just outside valid
        // range (see initialization above) because of while condition
        // jlo < jmid < jhi and jmid must be in valid range.
        //
        jmid = ( jlo + jhi ) / 2;
        // convert hershey_to_unicode_lookup_table[jmid].Hershey to signed
        // integer since we don't lose information - the number range
        // is from 1 and 2932 at the moment
        if ( in > (int) ( hershey_to_unicode_lookup_table[jmid].Hershey ) )
            jlo = jmid;
        else if ( in < (int) ( hershey_to_unicode_lookup_table[jmid].Hershey ) )
            jhi = jmid;
        else
            // We have found it!
            // in == hershey_to_unicode_lookup_table[jmid].Hershey
            //
            return ( jmid );
    }
    // jlo is invalid or it is valid and in > hershey_to_unicode_lookup_table[jlo].Hershey.
    // jhi is invalid or it is valid and in < hershey_to_unicode_lookup_table[jhi].Hershey.
    // All these conditions together imply in cannot be found in
    // hershey_to_unicode_lookup_table[j].Hershey, for all j.
    //
    return ( -1 );
#endif
}

//--------------------------------------------------------------------------
//  char *
//  plP_FCI2FontName ( PLUNICODE fci,
//                     const FCI_to_FontName_Table lookup[], const int nlookup)
//
//  Function takes an input FCI (font characterization integer) index,
//  looks through the lookup table (which must be sorted by PLUNICODE fci),
//  then returns the corresponding pointer to a valid font name.  If the FCI
//  index is not present the returned value is NULL.
//--------------------------------------------------------------------------

PLCHAR_VECTOR
plP_FCI2FontName( PLUNICODE fci,
                  const FCI_to_FontName_Table lookup[], const int nlookup )
{
    int jlo = -1, jmid, jhi = nlookup;
    while ( jhi - jlo > 1 )
    {
        // Note that although jlo or jhi can be just outside valid
        // range (see initialization above) because of while condition
        // jlo < jmid < jhi and jmid must be in valid range.
        //
        jmid = ( jlo + jhi ) / 2;
        if ( fci > lookup[jmid].fci )
            jlo = jmid;
        else if ( fci < lookup[jmid].fci )
            jhi = jmid;
        else
            // We have found it!
            // fci == lookup[jmid].fci
            //
            return (PLCHAR_VECTOR) ( lookup[jmid].pfont );
    }
    // jlo is invalid or it is valid and fci > lookup[jlo].Unicode.
    // jhi is invalid or it is valid and fci < lookup[jhi].Unicode.
    // All these conditions together imply fci index cannot be found in lookup.
    // Mark lookup failure with NULL pointer.
    //
    return ( NULL );
}

//--------------------------------------------------------------------------
// void plmtex3()
//
// This is the 3d equivalent of plmtex(). It prints out "text" at specified
// position relative to viewport (may be inside or outside)
//
// side	String contains one or more of the following characters
//  x,y,z : Specify which axis is to be labeled
//  p,s   : Label the "primary" or the "secondary" axis. The "primary" axis
//            being somewhat arbitrary, but basically it is the one that you'd
//            expect to labeled in a 3d graph of standard orientation. Example:
//            for z this would be the left hand axis.
//  v     : draw the text perpendicular to the axis.
//
// disp Displacement from specified edge of axis, measured outwards from
//	the axis in units of the current character height. The
//	centerlines of the characters are aligned with the specified
//	position.
//
// pos	Position of the reference point of the string relative to the
//	axis ends, ranging from 0.0 (left-hand end) to 1.0 (right-hand
//	end)
//
// just	Justification of string relative to reference point
//	just = 0.0 => left hand edge of string is at reference
//	just = 1.0 => right hand edge of string is at reference
//	just = 0.5 => center of string is at reference
//
// All calculations are done in physical coordinates.
//
//--------------------------------------------------------------------------

void
c_plmtex3( PLCHAR_VECTOR side, PLFLT disp, PLFLT pos, PLFLT just, PLCHAR_VECTOR text )
{
    // local storage
    PLFLT xmin, xmax, ymin, ymax, zmin, zmax, zscale;
    PLFLT chrdef, chrht;

    // calculated
    PLFLT xpc, ypc, xrefpc, yrefpc;
    PLFLT epx1 = 0.0, epy1 = 0.0, epx2 = 0.0, epy2 = 0.0, epx3 = 0.0, epy3 = 0.0;
    PLFLT dispx, dispy, xform[4];
    PLFLT shift, theta, temp;

    // check that the plotting environment is set up
    if ( plsc->level < 3 )
    {
        plabort( "plmtex3: Please set up window first" );
        return;
    }

    // get plotting environment information
    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );
    plgchr( &chrdef, &chrht );

    // handle x/y axises
    if ( ( plP_stindex( side, "x" ) != -1 ) || ( plP_stindex( side, "y" ) != -1 ) )
    {
        // get the locations of the end points of the relevant axis

        // x axis label
        if ( plP_stindex( side, "x" ) != -1 )
        {
            // primary
            if ( plP_stindex( side, "p" ) != -1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
                epx2 = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );
            }
            else
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );
                epx2 = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
            }
        }
        else
        {
            if ( plP_stindex( side, "p" ) != -1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
                epx2 = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );
            }
            else
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );
                epx2 = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
            }
        }

        // text always goes from left to right
        if ( epx1 > epx2 )
        {
            temp = epx1;
            epx1 = epx2;
            epx2 = temp;
            temp = epy1;
            epy1 = epy2;
            epy2 = temp;
            // recalculate position assuming the user specified
            // it in the min -> max direction of the axis.
            pos = 1.0 - pos;
        }

        // calculate location of text center point

        // 1. calculate the angle of the axis we are to
        // draw the text on relative to the horizontal

        if ( ( epx2 - epx1 ) != 0.0 )
        {
            theta = atan( ( epy2 - epy1 ) / ( epx2 - epx1 ) );
        }
        else
        {
            if ( epy2 > epy1 )
            {
                theta = 0.5 * PI;
            }
            else
            {
                theta = -0.5 * PI;
            }
        }

        // 2. calculate the perpendicular vector

        dispy = disp * chrht;

        // 3. calculate x & y center points

        xpc = pos * ( epx2 - epx1 ) + epx1;
        ypc = pos * ( epy2 - epy1 ) + epy1;

        // 4. compute reference point
        //  It appears that drivers that cannot handle text justification
        //   use this as the starting point of the string.
        //  Calculations must be done in millimeters for this part
        //   so we convert to mm, do the calculation and convert back.
        //  The calculation is also dependent of the orientation
        //   (perpendicular or parallel) of the text.

        xpc = plP_dcmmx( plP_pcdcx( (PLINT) xpc ) );
        ypc = plP_dcmmy( plP_pcdcy( (PLINT) ypc ) ) - dispy;

        shift = plstrl( text ) * just;

        if ( plP_stindex( side, "v" ) != -1 )
        {
            xrefpc = xpc;
            yrefpc = ypc - shift;
        }
        else
        {
            xrefpc = xpc - cos( theta ) * shift;
            yrefpc = ypc - sin( theta ) * shift;
        }

        xpc    = plP_mmpcx( xpc );
        ypc    = plP_mmpcy( ypc );
        xrefpc = plP_mmpcx( xrefpc );
        yrefpc = plP_mmpcy( yrefpc );

        // 5. compute transform matrix & draw text

        // perpendicular, rotate 90 degrees & shear

        if ( plP_stindex( side, "v" ) != -1 )
        {
            xform[0] = 0.0;
            xform[1] = -cos( theta );
            xform[2] = 1.0;
            xform[3] = -sin( theta );
            plP_text( 0, just, xform, (PLINT) xpc, (PLINT) ypc, (PLINT) xrefpc, (PLINT) yrefpc, text );
        }

        // parallel, rotate & shear by angle
        else
        {
            xform[0] = cos( theta );
            xform[1] = 0.0;
            xform[2] = sin( theta );
            xform[3] = 1.0;

            plP_text( 0, just, xform, (PLINT) xpc, (PLINT) ypc, (PLINT) xrefpc, (PLINT) yrefpc, text );
        }
    }

    // handle z axises
    if ( plP_stindex( side, "z" ) != -1 )
    {
        // Find the left most of the 4 z axis options for "primary"
        // Also find the location of frontmost point in the graph,
        //  which will be needed to calculate at what angle to shear
        //  the text.

        if ( plP_stindex( side, "p" ) != -1 )
        {
            epx1 = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
            epy1 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
            epy2 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmax ) );
            epx3 = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
            epy3 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );

            if ( plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) ) < epx1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmax ) );
                epx3 = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
                epy3 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
            }

            if ( plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) ) < epx1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmax ) );
                epx3 = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
                epy3 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
            }

            if ( plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) ) < epx1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmax ) );
                epx3 = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
                epy3 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );
            }
        }

        // find the right most of the 4 z axis options for "primary"
        if ( plP_stindex( side, "s" ) != -1 )
        {
            epx1 = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
            epy1 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
            epy2 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmax ) );
            epx3 = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
            epy3 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );

            if ( plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) ) > epx1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmin, ymax, zmax ) );
                epx3 = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
                epy3 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
            }

            if ( plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) ) > epx1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmax ) );
                epx3 = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
                epy3 = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
            }

            if ( plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) ) > epx1 )
            {
                epx1 = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
                epy1 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
                epy2 = plP_wcpcy( plP_w3wcy( xmax, ymax, zmax ) );
                epx3 = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
                epy3 = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );
            }
        }

        // Calculate location of text center point.
        // This is very similiar for the z axis.

        // primary and secondary have to be handled separately here

        if ( plP_stindex( side, "p" ) != -1 )
        {
            // 1. Calculate the angle of the axis we are to
            // draw the text on relative to the horizontal.

            if ( ( epx3 - epx1 ) != 0.0 )
            {
                theta = atan( ( epy3 - epy1 ) / ( epx3 - epx1 ) );
            }
            else
            {
                if ( epy3 > epy1 )
                {
                    theta = 0.5 * PI;
                }
                else
                {
                    theta = -0.5 * PI;
                }
            }

            // 2. Calculate the perpendicular vector.

            dispx = -cos( theta ) * disp * chrht;
            dispy = -sin( theta ) * disp * chrht;
        }
        else
        {
            if ( ( epx1 - epx3 ) != 0.0 )
            {
                theta = -atan( ( epy3 - epy1 ) / ( epx1 - epx3 ) );
            }
            else
            {
                if ( epy3 > epy1 )
                {
                    theta = -0.5 * PI;
                }
                else
                {
                    theta = 0.5 * PI;
                }
            }

            dispx = cos( theta ) * disp * chrht;
            dispy = sin( theta ) * disp * chrht;
        }

        // 3. Calculate x & y center points.

        xpc = epx1;
        ypc = pos * ( epy2 - epy1 ) + epy1;

        // 4. Compute the reference point.

        xpc = plP_dcmmx( plP_pcdcx( (PLINT) xpc ) ) + dispx;
        ypc = plP_dcmmy( plP_pcdcy( (PLINT) ypc ) ) + dispy;

        shift = plstrl( text ) * just;

        if ( plP_stindex( side, "v" ) != -1 )
        {
            xrefpc = xpc - cos( theta ) * shift;
            yrefpc = ypc - sin( theta ) * shift;
        }
        else
        {
            xrefpc = xpc;
            yrefpc = ypc - shift;
        }

        xpc    = plP_mmpcx( xpc );
        ypc    = plP_mmpcy( ypc );
        xrefpc = plP_mmpcx( xrefpc );
        yrefpc = plP_mmpcy( yrefpc );

        // 5. Compute transform matrix & draw text.

        if ( plP_stindex( side, "v" ) != -1 )
        {
            xform[0] = cos( theta );
            xform[1] = 0.0;
            xform[2] = sin( theta );
            xform[3] = 1.0;

            plP_text( 0, just, xform, (PLINT) xpc, (PLINT) ypc, (PLINT) xrefpc, (PLINT) yrefpc, text );
        }

        else
        {
            xform[0] = 0.0;
            xform[1] = -cos( theta );
            xform[2] = 1.0;
            xform[3] = -sin( theta );

            plP_text( 0, just, xform, (PLINT) xpc, (PLINT) ypc, (PLINT) xrefpc, (PLINT) yrefpc, text );
        }
    }
}

//--------------------------------------------------------------------------
// void plptex3()
//
// Prints out "text" at world cooordinate (wx,wy,wz).
//
// The text is drawn parallel to the line between (wx,wy,wz) and
// (wx+dx,wy+dy,wz+dz).
//
// The text is sheared so that it is "vertically" parallel to the
// line between (wx,wy,wz) and (wx+sx, wy+sy, wz+sz). If sx=sy=sz=0 then
// the text is simply rotated to parallel to the baseline.
//
// "just" adjusts the horizontal justification of the string:
//	just = 0.0 => left hand edge of string is at (wx,wy)
//	just = 1.0 => right hand edge of string is at (wx,wy)
//	just = 0.5 => center of string is at (wx,wy) etc.
//
// Calculations are done in physical coordinates.
//
//--------------------------------------------------------------------------

void
c_plptex3( PLFLT wx, PLFLT wy, PLFLT wz, PLFLT dx, PLFLT dy, PLFLT dz,
           PLFLT sx, PLFLT sy, PLFLT sz, PLFLT just, PLCHAR_VECTOR text )
{
    PLFLT xpc, ypc, xrefpc, yrefpc, xdpc, ydpc, xspc, yspc, ld, ls, cp, shift;
    PLFLT x_o, y_o, z_o, x_dx, y_dy, z_dz;
    PLFLT theta, phi, stride, xform[6], affineL[6], cosphi;

    // check that the plotting environment is set up
    if ( plsc->level < 3 )
    {
        plabort( "plptex3: Please set up window first" );
        return;
    }

    // compute text x,y location in physical coordinates
    xpc = plP_wcpcx( plP_w3wcx( wx, wy, wz ) );
    ypc = plP_wcpcy( plP_w3wcy( wx, wy, wz ) );

    // determine angle to rotate text in the x-y plane
    xdpc  = plP_wcpcx( plP_w3wcx( wx + dx, wy + dy, wz + dz ) );
    ydpc  = plP_wcpcy( plP_w3wcy( wx + dx, wy + dy, wz + dz ) );
    theta = atan2( ydpc - ypc, xdpc - xpc );

    // Determine angle to shear text in the x-y plane. This is a little
    // messy, but basically the idea is:
    //
    // Compute the dot product of the vector d and the vector s to
    // determine the angle between them (acos(t) = d . s / |d| |s|).
    // Then because acos will return a number from 0.0 to PI, i.e.
    // only in quadrants 1 or 2, compute the cross product of the
    // two vectors. If this is negative then the angle is adjusted
    // 0.0 to -PI.

    if ( ( sx == 0.0 ) && ( sy == 0.0 ) && ( sz == 0.0 ) )
    {
        phi = 0.0;
    }
    else
    {
        xspc = plP_wcpcx( plP_w3wcx( wx + sx, wy + sy, wz + sz ) );
        yspc = plP_wcpcy( plP_w3wcy( wx + sx, wy + sy, wz + sz ) );
        ld   = sqrt( ( xpc - xdpc ) * ( xpc - xdpc ) + ( ypc - ydpc ) * ( ypc - ydpc ) );
        ls   = sqrt( ( xpc - xspc ) * ( xpc - xspc ) + ( ypc - yspc ) * ( ypc - yspc ) );
        phi  = acos( ( ( xdpc - xpc ) * ( xspc - xpc ) + ( ydpc - ypc ) * ( yspc - ypc ) ) / ( ld * ls ) );
        cp   = ( xdpc - xpc ) * ( yspc - ypc ) - ( ydpc - ypc ) * ( xspc - xpc );
        if ( cp < 0.0 )
        {
            phi = -phi;
        }
        phi = 0.5 * PI - phi;
    }

    // Determine how to adjust the "stride" of the text to make it
    // appear that it is going into (or out of) the page. Basically
    // scale the x baseline of the text by the normalized length of
    // the d vector projected into the x-y plane.
    x_o  = plP_w3wcx( wx, wy, wz );
    y_o  = plP_w3wcy( wx, wy, wz );
    z_o  = plP_w3wcz( wx, wy, wz );
    x_dx = x_o - plP_w3wcx( wx + dx, wy + dy, wz + dz );
    y_dy = y_o - plP_w3wcy( wx + dx, wy + dy, wz + dz );
    z_dz = z_o - plP_w3wcz( wx + dx, wy + dy, wz + dz );

    stride = sqrt( x_dx * x_dx + y_dy * y_dy );
    stride = stride / sqrt( x_dx * x_dx + y_dy * y_dy + z_dz * z_dz );

    // compute the reference point
    xpc = plP_dcmmx( plP_pcdcx( (PLINT) xpc ) );
    ypc = plP_dcmmy( plP_pcdcy( (PLINT) ypc ) );

    shift  = plstrl( text ) * just;
    xrefpc = xpc - cos( theta ) * shift * stride;
    yrefpc = ypc - sin( theta ) * shift * stride;

    xpc    = plP_mmpcx( xpc );
    ypc    = plP_mmpcy( ypc );
    xrefpc = plP_mmpcx( xrefpc );
    yrefpc = plP_mmpcy( yrefpc );

    // compute the transform
    // This affine transformation corresponds to transforming from old
    // coordinates to new coordinates by rotating axes, y shearing
    // or (y skewing), and scaling.
    // Comment out the explicit xform calculations because we use
    // the affine utilities for that calculation instead.
    //
    // xform[0] = cos( theta ) * stride;
    // xform[1] = cos( theta ) * sin( phi ) - sin( theta ) * cos( phi );
    // xform[2] = sin( theta ) * stride;
    // xform[3] = sin( theta ) * sin( phi ) + cos( theta ) * cos( phi );
    //
    plP_affine_rotate( xform, 180. * theta / PI );
    plP_affine_yskew( affineL, -180. * phi / PI );
    plP_affine_multiply( xform, affineL, xform );
    cosphi = cos( phi );
    if ( fabs( cosphi ) > 1.e-300 )
        plP_affine_scale( affineL, 1. / stride, 1. / cosphi );
    else
        plP_affine_scale( affineL, 1. / stride, 1.e300 );
    plP_affine_multiply( xform, affineL, xform );

    plP_text( 0, just, xform, (PLINT) xpc, (PLINT) ypc, (PLINT) xrefpc, (PLINT) yrefpc, text );
}

//--------------------------------------------------------------------------
// void plsfont()
//
// Set the family, style and weight of the current font.
// This is a user-friendly front-end to plsfci.
// Note: A negative value signifies that this element should not be changed.
//--------------------------------------------------------------------------
void
c_plsfont( PLINT family, PLINT style, PLINT weight )
{
    PLUNICODE fci;

    plgfci( &fci );

    if ( family >= 0 )
    {
        // Bounds checking assumes symbol is last font
        if ( family > PL_FCI_SYMBOL )
            plwarn( "plsfont: Value for family is out of range" );
        else
            plP_hex2fci( (unsigned char) family, PL_FCI_FAMILY, &fci );
    }

    if ( style >= 0 )
    {
        // Bounds checking assumes oblique is last style
        if ( style > PL_FCI_OBLIQUE )
            plwarn( "plsfont: Value for style is out of range" );
        else
            plP_hex2fci( (unsigned char) style, PL_FCI_STYLE, &fci );
    }

    if ( weight >= 0 )
    {
        // Bounds checking assumes bold is last weight
        if ( weight > PL_FCI_BOLD )
            plwarn( "plsfont: Value for weight is out of range" );
        else
            plP_hex2fci( (unsigned char) weight, PL_FCI_WEIGHT, &fci );
    }

    plsfci( fci );
}

//--------------------------------------------------------------------------
// void plgfont()
//
// Get the family, style and weight of the current font.
// This is a user-friendly front-end to plgfci.
// Note: A NULL pointer signifies that this value should not be returned.
//--------------------------------------------------------------------------
void
c_plgfont( PLINT *p_family, PLINT *p_style, PLINT *p_weight )
{
    PLUNICODE     fci;
    unsigned char val;

    plgfci( &fci );

    if ( p_family )
    {
        plP_fci2hex( fci, &val, PL_FCI_FAMILY );
        *p_family = (PLINT) val;
    }

    if ( p_style )
    {
        plP_fci2hex( fci, &val, PL_FCI_STYLE );
        *p_style = (PLINT) val;
    }

    if ( p_weight )
    {
        plP_fci2hex( fci, &val, PL_FCI_WEIGHT );
        *p_weight = (PLINT) val;
    }
}


#undef PLSYM_H
#endif
