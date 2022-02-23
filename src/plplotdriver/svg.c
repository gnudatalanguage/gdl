// November 8, 2006
//
// PLplot driver for SVG 1.1 (http://www.w3.org/Graphics/SVG/)
//
// Copyright (C) 2006 Hazen Babcock
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

//---------------------------------------------
// Header files, defines and local variables
// ---------------------------------------------

#include <stdarg.h>
#include <math.h>

// PLplot header files

#include "plplotP.h"
#include "drivers.h"

// constants

#define SVG_Default_X      720
#define SVG_Default_Y      540

#define POINTS_PER_INCH    72

#define MAX_STRING_LEN     1000

// This has been generated empirically by looking carefully at results from
// examples 1 and 2.

#define FONT_SIZE_RATIO      1.34
#define FONT_SHIFT_RATIO     0.705
#define FONT_SHIFT_OFFSET    0.5

// local variables

PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_svg = "svg:Scalable Vector Graphics (SVG 1.1):1:svg:57:svg\n";

static int    already_warned = 0;

static int    text_clipping = 1;
static DrvOpt svg_options[] = { { "text_clipping", DRV_INT, &text_clipping, "Use text clipping (text_clipping=0|1)" } };

typedef struct
{
    short textClipping;
    int   which_clip;
    int   canvasXSize;
    int   canvasYSize;
    PLFLT scale;
    int   svgIndent;
    FILE  *svgFile;
    int   gradient_index;
    //  char curColor[7];
} SVG;

// font stuff

// Debugging extras

//-----------------------------------------------
// function declarations
// -----------------------------------------------

// Functions for writing XML SVG tags to a file

static void svg_open( SVG *, const char * );
static void svg_open_end( SVG * );
static void svg_attr_value( SVG *, const char *, const char * );
static void svg_attr_values( SVG *, const char *, const char *, ... );
static void svg_close( SVG *, const char * );
static void svg_general( SVG *, const char * );
static void svg_indent( SVG * );
static void svg_stroke_width( PLStream * );
static void svg_stroke_color( PLStream * );
static void svg_fill_color( PLStream * );
static void svg_fill_background_color( PLStream * );
static int svg_family_check( PLStream * );


// General

static void poly_line( PLStream *, short *, short *, PLINT, short );
static void gradient( PLStream *, short *, short *, PLINT );
static void write_hex( FILE *, unsigned char );
static void write_unicode( FILE *, PLUNICODE );
static void specify_font( FILE *, PLUNICODE );

// String processing

static void proc_str( PLStream *, EscText * );

// PLplot interface functions

void plD_dispatch_init_svg( PLDispatchTable *pdt );
void plD_init_svg( PLStream * );
void plD_line_svg( PLStream *, short, short, short, short );
void plD_polyline_svg( PLStream *, short *, short *, PLINT );
void plD_eop_svg( PLStream * );
void plD_bop_svg( PLStream * );
void plD_tidy_svg( PLStream * );
void plD_state_svg( PLStream *, PLINT );
void plD_esc_svg( PLStream *, PLINT, void * );

//--------------------------------------------------------------------------
// dispatch_init_init()
//
// Initialize device dispatch table
//--------------------------------------------------------------------------

void plD_dispatch_init_svg( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "Scalable Vector Graphics (SVG 1.1)";
    pdt->pl_DevName = "svg";
#endif
    pdt->pl_type     = plDevType_FileOriented;
    pdt->pl_seq      = 57;
    pdt->pl_init     = (plD_init_fp) plD_init_svg;
    pdt->pl_line     = (plD_line_fp) plD_line_svg;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_svg;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_svg;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_svg;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_svg;
    pdt->pl_state    = (plD_state_fp) plD_state_svg;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_svg;
}

//--------------------------------------------------------------------------
// svg_init()
//
// Initialize device
//--------------------------------------------------------------------------

void plD_init_svg( PLStream *pls )
{
    SVG *aStream;

    pls->termin  = 0;                   // not an interactive device
    pls->color   = 1;                   // supports color
    pls->width   = 1;
    pls->verbose = 1;
    pls->bytecnt = 0;
    //pls->debug = 1;
    pls->dev_text     = 1;      // handles text
    pls->dev_unicode  = 1;      // wants text as unicode
    pls->page         = 0;
    pls->dev_fill0    = 1;      // driver generates solid fills
    pls->dev_fill1    = 0;      // Use PLplot core fallback for pattern fills
    pls->dev_gradient = 1;      // driver renders gradient

    pls->graphx = GRAPHICS_MODE;

    if ( !pls->colorset )
        pls->color = 1;

    // Initialize family file info
    plFamInit( pls );

    // Prompt for a file name if not already set
    plOpenFile( pls );
// Allocate and initialize device-specific data

    if ( pls->dev != NULL )
        free( (void *) pls->dev );

    pls->dev = calloc( 1, (size_t) sizeof ( SVG ) );
    if ( pls->dev == NULL )
        plexit( "plD_init_svg: Out of memory." );

    aStream = (SVG *) pls->dev;

    // Set the bounds for plotting in points (unit of 1/72 of an inch).  Default is SVG_Default_X points x SVG_Default_Y points unless otherwise specified by plspage or -geometry option.

    if ( pls->xlength <= 0 || pls->ylength <= 0 )
    {
        aStream->canvasXSize = SVG_Default_X;
        aStream->canvasYSize = SVG_Default_Y;
    }
    else
    {
        aStream->canvasXSize = pls->xlength;
        aStream->canvasYSize = pls->ylength;
    }
    // Calculate ratio of (larger) internal PLplot coordinates to external
    // coordinates used for svg file.
    if ( aStream->canvasXSize > aStream->canvasYSize )
        aStream->scale = (PLFLT) ( PIXELS_X - 1 ) / (PLFLT) aStream->canvasXSize;
    else
        aStream->scale = (PLFLT) PIXELS_Y / (PLFLT) aStream->canvasYSize;
    plP_setphy( (PLINT) 0, (PLINT) ( aStream->scale * aStream->canvasXSize ), (PLINT) 0, (PLINT) ( aStream->scale * aStream->canvasYSize ) ); // Scaled points.

    plP_setpxl( aStream->scale * POINTS_PER_INCH / 25.4, aStream->scale * POINTS_PER_INCH / 25.4 );                                           // Scaled points/mm.

    aStream->svgFile = pls->OutFile;

    // Handle the text clipping option
    plParseDrvOpts( svg_options );

    // Turn on text clipping if the user desires this
    if ( text_clipping )
    {
        aStream->textClipping = 1;
    }
    aStream->textClipping = (short) text_clipping;

    aStream->svgIndent      = 0;
    aStream->gradient_index = 0;
    svg_general( aStream, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    svg_general( aStream, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n" );
    svg_general( aStream, "        \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" );
}

//--------------------------------------------------------------------------
// svg_bop()
//
// Set up for the next page.
//--------------------------------------------------------------------------

void plD_bop_svg( PLStream *pls )
{
    SVG *aStream;

    // Plot familying stuff. Not really understood, just copying gd.c
    plGetFam( pls );
// n.b. pls->dev can change because of an indirect call to plD_init_svg
// from plGetFam if familying is enabled.  Thus, wait to define aStream until
// now.
    aStream = pls->dev;

    pls->famadv = 1;
    pls->page++;
    if ( svg_family_check( pls ) )
    {
        return;
    }

    // write opening svg tag for the new page

    svg_open( aStream, "svg" );
    svg_attr_value( aStream, "xmlns", "http://www.w3.org/2000/svg" );
    svg_attr_value( aStream, "xmlns:xlink", "http://www.w3.org/1999/xlink" );
    svg_attr_value( aStream, "version", "1.1" );
    // svg_attr_values("width", "%dcm", (int)((double)canvasXSize/POINTS_PER_INCH * 2.54));
    // svg_attr_values("height", "%dcm", (int)((double)canvasYSize/POINTS_PER_INCH * 2.54));
    svg_attr_values( aStream, "width", "%dpt", aStream->canvasXSize );
    svg_attr_values( aStream, "height", "%dpt", aStream->canvasYSize );
    svg_attr_values( aStream, "viewBox", "%d %d %d %d", 0, 0, aStream->canvasXSize, aStream->canvasYSize );
    svg_general( aStream, ">\n" );

    // set the background by drawing a rectangle that is the size of
    // of the canvas and filling it with the background color.

    svg_open( aStream, "rect" );
    svg_attr_values( aStream, "x", "%d", 0 );
    svg_attr_values( aStream, "y", "%d", 0 );
    svg_attr_values( aStream, "width", "%d", aStream->canvasXSize );
    svg_attr_values( aStream, "height", "%d", aStream->canvasYSize );
    svg_attr_value( aStream, "stroke", "none" );
    svg_fill_background_color( pls );
    svg_open_end( aStream );

    // invert the coordinate system so that PLplot graphs appear right side up

    svg_open( aStream, "g" );
    svg_attr_values( aStream, "transform", "matrix(1 0 0 -1 0 %d)", aStream->canvasYSize );
    svg_general( aStream, ">\n" );
}

//--------------------------------------------------------------------------
// svg_line()
//
// Draw a line in the current color from (x1,y1) to (x2,y2).
//--------------------------------------------------------------------------

void plD_line_svg( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    SVG *aStream;

    aStream = pls->dev;

    if ( svg_family_check( pls ) )
    {
        return;
    }
    svg_open( aStream, "polyline" );
    svg_stroke_width( pls );
    svg_stroke_color( pls );
    svg_attr_value( aStream, "fill", "none" );
    // svg_attr_value(aStream, "shape-rendering", "crispEdges");
    svg_attr_values( aStream, "points", "%r,%r %r,%r", (double) x1a / aStream->scale, (double) y1a / aStream->scale, (double) x2a / aStream->scale, (double) y2a / aStream->scale );
    svg_open_end( aStream );
}

//--------------------------------------------------------------------------
// svg_polyline()
//
// Draw a polyline in the current color.
//--------------------------------------------------------------------------

void plD_polyline_svg( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    if ( svg_family_check( pls ) )
    {
        return;
    }
    poly_line( pls, xa, ya, npts, 0 );
}

//--------------------------------------------------------------------------
// svg_eop()
//
// End of page
//--------------------------------------------------------------------------

void plD_eop_svg( PLStream *pls )
{
    SVG *aStream;

    aStream = pls->dev;

    if ( svg_family_check( pls ) )
    {
        return;
    }
    // write the closing svg tag

    svg_close( aStream, "g" );
    svg_close( aStream, "svg" );
}

//--------------------------------------------------------------------------
// svg_tidy()
//
// Close graphics file or otherwise clean up.
//--------------------------------------------------------------------------

void plD_tidy_svg( PLStream *pls )
{
    if ( svg_family_check( pls ) )
    {
        return;
    }
    plCloseFile( pls );
}

//--------------------------------------------------------------------------
// plD_state_svg()
//
// Handle change in PLStream state (color, pen width, fill attribute, etc).
//
// Nothing is done here because these attributes are aquired from
// PLStream for each element that is drawn.
//--------------------------------------------------------------------------

void plD_state_svg( PLStream *PL_UNUSED( pls ), PLINT PL_UNUSED( op ) )
{
}

//--------------------------------------------------------------------------
// svg_esc()
//
// Escape function.
//--------------------------------------------------------------------------

void plD_esc_svg( PLStream *pls, PLINT op, void *ptr )
{
    if ( svg_family_check( pls ) )
    {
        return;
    }
    switch ( op )
    {
    case PLESC_FILL:      // fill polygon
        poly_line( pls, pls->dev_x, pls->dev_y, pls->dev_npts, 1 );
        break;
    case PLESC_GRADIENT:      // render gradient inside polygon
        gradient( pls, pls->dev_x, pls->dev_y, pls->dev_npts );
        break;
    case PLESC_HAS_TEXT:  // render text
        proc_str( pls, (EscText *) ptr );
        break;
    }
}

//--------------------------------------------------------------------------
// poly_line()
//
// Handles drawing filled and unfilled polygons
//--------------------------------------------------------------------------

void poly_line( PLStream *pls, short *xa, short *ya, PLINT npts, short fill )
{
    int i;
    SVG *aStream;

    aStream = pls->dev;

    svg_open( aStream, "polyline" );
    if ( fill )
    {
        // Two adjacent regions will put non-zero width boundary strokes on top
        // of each other on their common boundary.  Thus, a stroke on the boundary
        // of a filled region is generally a bad idea when the fill is partially
        // opaque because the partial opacity of the two boundary strokes which
        // are on top of each other will mutually interfere and produce a
        // bad-looking result.  On the other hand, for completely opaque fills
        // a boundary stroke is a good idea since if it is of sufficient width
        // it will keep the background from leaking through at the anti-aliased
        // edges of filled regions that have a common boundary with other
        // filled regions.
        if ( pls->curcolor.a < 0.99 )
        {
            svg_attr_value( aStream, "stroke", "none" );
        }
        else
        {
            svg_stroke_width( pls );
            svg_stroke_color( pls );
        }
        svg_fill_color( pls );
        if ( pls->dev_eofill )
            svg_attr_value( aStream, "fill-rule", "evenodd" );
        else
            svg_attr_value( aStream, "fill-rule", "nonzero" );
    }
    else
    {
        svg_stroke_width( pls );
        svg_stroke_color( pls );
        svg_attr_value( aStream, "fill", "none" );
    }
    //svg_attr_value(aStream, "shape-rendering", "crispEdges");
    svg_indent( aStream );
    fprintf( aStream->svgFile, "points=\"" );
    for ( i = 0; i < npts; i++ )
    {
        fprintf( aStream->svgFile, "%.2f,%.2f ", (double) xa[i] / aStream->scale, (double) ya[i] / aStream->scale );
        if ( ( ( i + 1 ) % 10 ) == 0 )
        {
            fprintf( aStream->svgFile, "\n" );
            svg_indent( aStream );
        }
    }
    fprintf( aStream->svgFile, "\"/>\n" );
    aStream->svgIndent -= 2;
}

//--------------------------------------------------------------------------
// gradient()
//
// Draws gradient
//--------------------------------------------------------------------------

void gradient( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    int  i;
    // 27 should be the maximum needed below, but be generous.
    char buffer[50];
    SVG  *aStream;

    aStream = pls->dev;

    svg_open( aStream, "g>" );
    svg_open( aStream, "defs>" );
    svg_open( aStream, "linearGradient" );
    // Allows ~2^31 unique gradient id's, gradient_index incremented below.
    sprintf( buffer, "MyGradient%010d", aStream->gradient_index );
    svg_attr_value( aStream, "id", buffer );
    svg_attr_value( aStream, "gradientUnits", "userSpaceOnUse" );
    sprintf( buffer, "%.2f", pls->xgradient[0] / aStream->scale );
    svg_attr_value( aStream, "x1", buffer );
    sprintf( buffer, "%.2f", pls->ygradient[0] / aStream->scale );
    svg_attr_value( aStream, "y1", buffer );
    sprintf( buffer, "%.2f", pls->xgradient[1] / aStream->scale );
    svg_attr_value( aStream, "x2", buffer );
    sprintf( buffer, "%.2f", pls->ygradient[1] / aStream->scale );
    svg_attr_value( aStream, "y2", buffer );
    svg_general( aStream, ">\n" );

    for ( i = 0; i < pls->ncol1; i++ )
    {
        svg_indent( aStream );
        fprintf( aStream->svgFile, "<stop offset=\"%.3f\" ",
            (double) i / (double) ( pls->ncol1 - 1 ) );
        fprintf( aStream->svgFile, "stop-color=\"#" );
        write_hex( aStream->svgFile, pls->cmap1[i].r );
        write_hex( aStream->svgFile, pls->cmap1[i].g );
        write_hex( aStream->svgFile, pls->cmap1[i].b );
        fprintf( aStream->svgFile, "\" " );
        fprintf( aStream->svgFile, "stop-opacity=\"%.3f\"/>\n", pls->cmap1[i].a );
    }

    svg_close( aStream, "linearGradient" );
    svg_close( aStream, "defs" );
    svg_open( aStream, "polyline" );
    sprintf( buffer, "url(#MyGradient%010d)", aStream->gradient_index++ );
    svg_attr_value( aStream, "fill", buffer );
    svg_indent( aStream );
    fprintf( aStream->svgFile, "points=\"" );
    for ( i = 0; i < npts; i++ )
    {
        fprintf( aStream->svgFile, "%.2f,%.2f ", (double) xa[i] / aStream->scale, (double) ya[i] / aStream->scale );
        if ( ( ( i + 1 ) % 10 ) == 0 )
        {
            fprintf( aStream->svgFile, "\n" );
            svg_indent( aStream );
        }
    }
    fprintf( aStream->svgFile, "\"/>\n" );
    aStream->svgIndent -= 2;
    svg_close( aStream, "g" );
}

//--------------------------------------------------------------------------
// proc_str()
//
// Processes strings for display.
//
// NOTE:
//
// (1) This was tested on Firefox and Camino where it seemed to display
// text properly. However, it isn't obvious to me that these browsers
// conform to the specification. Basically the issue is that some of
// the text properties (i.e. dy) that you specify inside a tspan element
// remain in force until the end of the text element. It would seem to
// me that they should only apply inside the tspan tag. To get around
// this, and because it was easier anyway, I used what is essentially
// a list of tspan tags rather than a tree of tspan tags. Perhaps
// better described as a tree with one branch?
//
// (2) To deal with the some whitespace annoyances, the entire text
// element must be written on a single line. If there are lots of
// format characters then this line might end up being too long
// for some SVG implementations.
//
// (3) Text placement is not ideal. Vertical offset seems to be
// particularly troublesome.
//
// (4) See additional notes in specify_font re. to sans / serif
//
//--------------------------------------------------------------------------

void proc_str( PLStream *pls, EscText *args )
{
    char         plplot_esc;
    short        i;
    short        totalTags = 1;
    short        ucs4Len   = (short) args->unicode_array_len;
    double       ftHt, scaled_offset, scaled_ftHt;
    PLUNICODE    fci;
    PLINT        rcx[4], rcy[4];
    static PLINT prev_rcx[4], prev_rcy[4];
    PLFLT        rotation, shear, stride, cos_rot, sin_rot, sin_shear, cos_shear;
    PLFLT        t[4];
    int          glyph_size, sum_glyph_size;
    short        if_write;
    //   PLFLT *t = args->xform;
    PLUNICODE    *ucs4 = args->unicode_array;
    SVG          *aStream;
    PLFLT        old_sscale, sscale, old_soffset, soffset, old_dup, ddup;
    PLINT        level;
    PLINT        same_clip;

    // check that we got unicode
    if ( ucs4Len == 0 )
    {
        printf( "Non unicode string passed to SVG driver, ignoring\n" );
        return;
    }

    // get plplot escape character and the current font
    plgesc( &plplot_esc );
    plgfci( &fci );

    // determine the font height in points.
    ftHt = FONT_SIZE_RATIO * pls->chrht * POINTS_PER_INCH / 25.4;

    // Setup & apply text clipping area if desired
    aStream = (SVG *) pls->dev;
    if ( aStream->textClipping )
    {
        // Use PLplot core routine difilt_clip to appropriately
        // transform the coordinates of the clipping rectangle
        difilt_clip( rcx, rcy );
        same_clip = TRUE;
        if ( aStream->which_clip == 0 )
        {
            same_clip = FALSE;
        }
        else
        {
            for ( i = 0; i < 4; i++ )
            {
                if ( rcx[i] != prev_rcx[i] ||
                     rcy[i] != prev_rcy[i] )
                    same_clip = FALSE;
            }
        }
        if ( !same_clip )
        {
            svg_open( aStream, "clipPath" );
            svg_attr_values( aStream, "id", "text-clipping%d", aStream->which_clip );
            svg_general( aStream, ">\n" );

            // Output a polygon to represent the clipping region.
            svg_open( aStream, "polygon" );
            svg_attr_values( aStream,
                "points",
                "%f,%f %f,%f %f,%f %f,%f",
                ( (PLFLT) rcx[0] ) / aStream->scale,
                ( (PLFLT) rcy[0] ) / aStream->scale,
                ( (PLFLT) rcx[1] ) / aStream->scale,
                ( (PLFLT) rcy[1] ) / aStream->scale,
                ( (PLFLT) rcx[2] ) / aStream->scale,
                ( (PLFLT) rcy[2] ) / aStream->scale,
                ( (PLFLT) rcx[3] ) / aStream->scale,
                ( (PLFLT) rcy[3] ) / aStream->scale );
            svg_open_end( aStream );

            svg_close( aStream, "clipPath" );
            for ( i = 0; i < 4; i++ )
            {
                prev_rcx[i] = rcx[i];
                prev_rcy[i] = rcy[i];
            }
            aStream->which_clip++;
        }
        svg_open( aStream, "g" );
        svg_attr_values( aStream, "clip-path",
            "url(#text-clipping%d)", aStream->which_clip - 1 );
        svg_general( aStream, ">\n" );
    }

    // This draws the clipping region on the screen which can
    // be very helpful for debugging.

    //
    // svg_open(aStream, "polygon");
    // svg_attr_values(aStream,
    //              "points",
    //              "%f,%f %f,%f %f,%f %f,%f",
    //              ((PLFLT)rcx[0])/aStream->scale,
    //              ((PLFLT)rcy[0])/aStream->scale,
    //              ((PLFLT)rcx[1])/aStream->scale,
    //              ((PLFLT)rcy[1])/aStream->scale,
    //              ((PLFLT)rcx[2])/aStream->scale,
    //              ((PLFLT)rcy[2])/aStream->scale,
    //              ((PLFLT)rcx[3])/aStream->scale,
    //              ((PLFLT)rcy[3])/aStream->scale);
    // svg_stroke_width(pls);
    // svg_stroke_color(pls);
    // svg_attr_value(aStream, "fill", "none");
    // svg_open_end(aStream);
    //

    // Calculate the tranformation matrix for SVG based on the
    // transformation matrix provided by PLplot.
    plRotationShear( args->xform, &rotation, &shear, &stride );
    // N.B. Experimentally, I (AWI) have found the svg rotation angle is
    // the negative of the libcairo rotation angle, and the svg shear angle
    // is pi minus the libcairo shear angle.
    rotation -= pls->diorot * PI / 2.0;
    cos_rot   = cos( rotation );
    sin_rot   = -sin( rotation );
    sin_shear = sin( shear );
    cos_shear = -cos( shear );
    t[0]      = cos_rot * stride;
    t[1]      = -sin_rot * stride;
    t[2]      = cos_rot * sin_shear + sin_rot * cos_shear;
    t[3]      = -sin_rot * sin_shear + cos_rot * cos_shear;

    //--------------
    // open text tag
    // --------------

    svg_open( aStream, "text" );

    svg_attr_value( aStream, "dominant-baseline", "no-change" );

    // set font color
    svg_fill_color( pls );

    // white space preserving mode
    svg_attr_value( aStream, "xml:space", "preserve" );

    // set the font size
    svg_attr_values( aStream, "font-size", "%d", (int) ftHt );

    // Apply coordinate transform for text display.
    // The transformation also defines the location of the text in x and y.
    svg_attr_values( aStream, "transform", "matrix(%f %f %f %f %f %f)",
        t[0], t[1], t[2], t[3],
        (double) ( args->x / aStream->scale ),
        (double) ( args->y / aStream->scale ) );


    //----------------------------------------------------------
    // Write the text with formatting
    // We just keep stacking up tspan tags, then close them all
    // after we have written out all of the text.
    // ----------------------------------------------------------

    // For if_write = 0, we write nothing and instead accumulate the
    // sum_glyph_size from the fontsize of the individual glyphs which
    // is then used to figure out the initial x position from text-anchor and
    // args->just that is used to write out the SVG xml for if_write = 1.

    glyph_size     = (int) ftHt;
    sum_glyph_size = 0;
    if_write       = 0;
    while ( if_write < 2 )
    {
        if ( if_write == 1 )
        {
            //printf("number of characters = %f\n", sum_glyph_size/ftHt);
            // The above coordinate transform defines the _raw_ x position of the
            // text without justification so this attribute value depends on
            // text-anchor and args->just*sum_glyph_size
            // N.B. sum_glyph_size calculation only correct for monospaced fonts
            // so generally sum_glyph_size will be overestimated by various amounts
            // depending on what glyphs are to be rendered, the font, etc.  However,
            // this correction is differential respect to the end points or the
            // middle so you should be okay so long as you don't deviate too far
            // from those anchor points.
            if ( args->just < 0.33 )
            {
                svg_attr_value( aStream, "text-anchor", "start" ); // left justification
                svg_attr_values( aStream, "x", "%f", (double) ( -args->just * sum_glyph_size ) );
            }
            else if ( args->just > 0.66 )
            {
                svg_attr_value( aStream, "text-anchor", "end" ); // right justification
                svg_attr_values( aStream, "x", "%f", (double) ( ( 1. - args->just ) * sum_glyph_size ) );
            }
            else
            {
                svg_attr_value( aStream, "text-anchor", "middle" ); // center
                svg_attr_values( aStream, "x", "%f", (double) ( ( 0.5 - args->just ) * sum_glyph_size ) );
            }

            // The text goes at zero in y since the above
            // coordinate transform defines the y position of the text
            svg_attr_values( aStream, "y", "%f",
                FONT_SHIFT_RATIO * 0.5 * ftHt +
                FONT_SHIFT_OFFSET );

            fprintf( aStream->svgFile, ">" );

            // specify the initial font
            specify_font( aStream->svgFile, fci );
        }
        i           = 0;
        scaled_ftHt = ftHt;
        level       = 0;
        ddup        = 0.;
        while ( i < ucs4Len )
        {
            if ( ucs4[i] < PL_FCI_MARK )                 // not a font change
            {
                if ( ucs4[i] != (PLUNICODE) plplot_esc ) // a character to display
                {
                    if ( if_write )
                    {
                        write_unicode( aStream->svgFile, ucs4[i] );
                    }
                    else
                    {
                        sum_glyph_size += glyph_size;
                    }
                    i++;
                    continue;
                }
                i++;
                if ( ucs4[i] == (PLUNICODE) plplot_esc ) // a escape character to display
                {
                    if ( if_write )
                    {
                        write_unicode( aStream->svgFile, ucs4[i] );
                    }
                    else
                    {
                        sum_glyph_size += glyph_size;
                    }
                    i++;
                    continue;
                }
                else
                {
                    // super/subscript logic follows that in plstr routine (plsym.c)
                    // for Hershey fonts. Factor of FONT_SHIFT_RATIO*0.80 is empirical
                    // adjustment.
                    if ( ucs4[i] == (PLUNICODE) 'u' ) // Superscript
                    {
                        plP_script_scale( TRUE, &level,
                            &old_sscale, &sscale, &old_soffset, &soffset );
                        // The correction for the difference in magnitude
                        // between the baseline and middle coordinate systems
                        // for superscripts should be
                        // 0.5*(base font size - superscript/subscript font size).
                        old_dup = ddup;
                        ddup    = 0.5 * ( 1.0 - sscale );
                        if ( level <= 0 )
                        {
                            scaled_offset = FONT_SHIFT_RATIO * ftHt * ( 0.80 * ( soffset - old_soffset ) - ( ddup - old_dup ) );
                        }
                        else
                        {
                            scaled_offset = -FONT_SHIFT_RATIO * ftHt * ( 0.80 * ( soffset - old_soffset ) + ( ddup - old_dup ) );
                        }
                        scaled_ftHt = sscale * ftHt;
                        if ( if_write )
                        {
                            totalTags++;
                            fprintf( aStream->svgFile, "<tspan dy=\"%f\" font-size=\"%d\">", scaled_offset, (int) scaled_ftHt );
                        }
                        else
                        {
                            glyph_size = (int) scaled_ftHt;
                        }
                    }
                    if ( ucs4[i] == (PLUNICODE) 'd' ) // Subscript
                    {
                        plP_script_scale( FALSE, &level,
                            &old_sscale, &sscale, &old_soffset, &soffset );
                        // The correction for the difference in magnitude
                        // between the baseline and middle coordinate systems
                        // for superscripts should be
                        // 0.5*(base font size - superscript/subscript font size).
                        old_dup = ddup;
                        ddup    = 0.5 * ( 1.0 - sscale );
                        if ( level < 0 )
                        {
                            scaled_offset = FONT_SHIFT_RATIO * ftHt * ( 0.80 * ( soffset - old_soffset ) - ( ddup - old_dup ) );
                        }
                        else
                        {
                            scaled_offset = -FONT_SHIFT_RATIO * ftHt * ( 0.80 * ( soffset - old_soffset ) + ( ddup - old_dup ) );
                        }
                        scaled_ftHt = sscale * ftHt;
                        if ( if_write )
                        {
                            totalTags++;
                            fprintf( aStream->svgFile, "<tspan dy=\"%f\" font-size=\"%d\">", scaled_offset, (int) scaled_ftHt );
                        }
                        else
                        {
                            glyph_size = (int) scaled_ftHt;
                        }
                    }
                    i++;
                }
            }
            else // a font change
            {
                if ( if_write )
                {
                    specify_font( aStream->svgFile, ucs4[i] );
                    totalTags++;
                }
                i++;
            }
        }
        if_write++;
    }

    //----------------------------------------------
    // close out all the tspan tags and the text tag
    // ----------------------------------------------

    for ( i = 0; i < totalTags; i++ )
    {
        fprintf( aStream->svgFile, "</tspan>" );
    }
    // The following commented out (by AWI) because it is a bad idea to
    // put line ends in the middle of a text tag.  This was the key to
    // all the text rendering issues we had.
    //fprintf(svgFile,"\n");
    // For the same reason use fprintf and svgIndent -= 2;
    // to close the text tag rather than svg_close("text"); since
    // we don't want indentation spaces entering the text.
    // svg_close("text");
    fprintf( aStream->svgFile, "</text>\n" );
    aStream->svgIndent -= 2;
    if ( aStream->textClipping )
    {
        svg_close( aStream, "g" );
    }
}

//--------------------------------------------------------------------------
// svg_open ()
//
// Used to open a new XML expression, sets the indent level appropriately
//--------------------------------------------------------------------------

void svg_open( SVG *aStream, const char *tag )
{
    svg_indent( aStream );
    fprintf( aStream->svgFile, "<%s\n", tag );
    aStream->svgIndent += 2;
}

//--------------------------------------------------------------------------
// svg_open_end ()
//
// Used to end the opening of a new XML expression i.e. add
// the final ">".
//--------------------------------------------------------------------------

void svg_open_end( SVG *aStream )
{
    svg_indent( aStream );
    fprintf( aStream->svgFile, "/>\n" );
    aStream->svgIndent -= 2;
}

//--------------------------------------------------------------------------
// svg_attr_value ()
//
// Prints two strings to svgFile as a XML attribute value pair
// i.e. foo="bar"
//--------------------------------------------------------------------------

void svg_attr_value( SVG *aStream, const char *attribute, const char *value )
{
    svg_indent( aStream );
    fprintf( aStream->svgFile, "%s=\"%s\"\n", attribute, value );
}

//--------------------------------------------------------------------------
// svg_attr_values ()
//
// Prints a string and a bunch of numbers / strings as a XML attribute
// value pair i.e. foo="0 10 1.0 5.3 bar"
//
// This function is derived from an example in
// "The C Programming Language" by Kernighan and Ritchie.
//
//--------------------------------------------------------------------------

void svg_attr_values( SVG *aStream, const char *attribute, const char *format, ... )
{
    va_list    ap;
    const char *p, *sval;
    int        ival;
    double     dval;

    svg_indent( aStream );
    fprintf( aStream->svgFile, "%s=\"", attribute );
    va_start( ap, format );
    for ( p = format; *p; p++ )
    {
        if ( *p != '%' )
        {
            fprintf( aStream->svgFile, "%c", *p );
            continue;
        }
        switch ( *++p )
        {
        case 'd':
            ival = va_arg( ap, int );
            fprintf( aStream->svgFile, "%d", ival );
            break;
        case 'f':
            dval = va_arg( ap, double );
            fprintf( aStream->svgFile, "%f", dval );
            break;
        case 'r':
            // r is non-standard, but use it here to format rounded value
            dval = va_arg( ap, double );
            fprintf( aStream->svgFile, "%.2f", dval );
            break;
        case 's':
            sval = va_arg( ap, char * );
            fprintf( aStream->svgFile, "%s", sval );
            break;
        default:
            fprintf( aStream->svgFile, "%c", *p );
            break;
        }
    }
    fprintf( aStream->svgFile, "\"\n" );
    va_end( ap );
}

//--------------------------------------------------------------------------
// svg_close ()
//
// Used to close a XML expression, sets the indent level appropriately
//--------------------------------------------------------------------------

void svg_close( SVG *aStream, const char *tag )
{
    aStream->svgIndent -= 2;
    svg_indent( aStream );
    if ( strlen( tag ) > 0 )
    {
        fprintf( aStream->svgFile, "</%s>\n", tag );
    }
    else
    {
        fprintf( aStream->svgFile, "/>\n" );
    }
}

//--------------------------------------------------------------------------
// svg_general ()
//
// Used to print any text into the svgFile
//--------------------------------------------------------------------------

void svg_general( SVG *aStream, const char *text )
{
    svg_indent( aStream );
    fprintf( aStream->svgFile, "%s", text );
}

//--------------------------------------------------------------------------
// svg_indent ()
//
// Indents properly based on the current indent level
//--------------------------------------------------------------------------

void svg_indent( SVG *aStream )
{
    short i;
    for ( i = 0; i < aStream->svgIndent; i++ )
    {
        fprintf( aStream->svgFile, " " );
    }
}

//--------------------------------------------------------------------------
// svg_stroke_width ()
//
// sets the stroke width based on the current width
//--------------------------------------------------------------------------

void svg_stroke_width( PLStream *pls )
{
    SVG *aStream;

    aStream = pls->dev;
    svg_indent( aStream );
    fprintf( aStream->svgFile, "stroke-width=\"%e\"\n", pls->width );
}

//--------------------------------------------------------------------------
// svg_stroke_color ()
//
// sets the stroke color based on the current color
//--------------------------------------------------------------------------

void svg_stroke_color( PLStream *pls )
{
    SVG *aStream;

    aStream = pls->dev;
    svg_indent( aStream );
    fprintf( aStream->svgFile, "stroke=\"#" );
    write_hex( aStream->svgFile, pls->curcolor.r );
    write_hex( aStream->svgFile, pls->curcolor.g );
    write_hex( aStream->svgFile, pls->curcolor.b );
    fprintf( aStream->svgFile, "\"\n" );
    svg_indent( aStream );
    fprintf( aStream->svgFile, "stroke-opacity=\"%f\"\n", pls->curcolor.a );
}

//--------------------------------------------------------------------------
// svg_fill_color ()
//
// sets the fill color based on the current color
//--------------------------------------------------------------------------

void svg_fill_color( PLStream *pls )
{
    SVG *aStream;

    aStream = pls->dev;
    svg_indent( aStream );
    fprintf( aStream->svgFile, "fill=\"#" );
    write_hex( aStream->svgFile, pls->curcolor.r );
    write_hex( aStream->svgFile, pls->curcolor.g );
    write_hex( aStream->svgFile, pls->curcolor.b );
    fprintf( aStream->svgFile, "\"\n" );
    svg_indent( aStream );
    fprintf( aStream->svgFile, "fill-opacity=\"%f\"\n", pls->curcolor.a );
}

//--------------------------------------------------------------------------
// svg_fill_background_color ()
//
// sets the background fill color based on the current background color
//--------------------------------------------------------------------------

void svg_fill_background_color( PLStream *pls )
{
    SVG *aStream;

    aStream = pls->dev;
    svg_indent( aStream );
    fprintf( aStream->svgFile, "fill=\"#" );
    write_hex( aStream->svgFile, pls->cmap0[0].r );
    write_hex( aStream->svgFile, pls->cmap0[0].g );
    write_hex( aStream->svgFile, pls->cmap0[0].b );
    fprintf( aStream->svgFile, "\"\n" );
    svg_indent( aStream );
    fprintf( aStream->svgFile, "fill-opacity=\"%f\"\n", pls->cmap0[0].a );
}

//--------------------------------------------------------------------------
// svg_family_check ()
//
// support function to help supress more than one page if family file
// output not specified by the user  (e.g., with the -fam command-line option).
//--------------------------------------------------------------------------

int svg_family_check( PLStream *pls )
{
    if ( pls->family || pls->page == 1 )
    {
        return 0;
    }
    else
    {
        if ( !already_warned )
        {
            already_warned = 1;
            plwarn( "All pages after the first skipped because family file output not specified.\n" );
        }
        return 1;
    }
}

//--------------------------------------------------------------------------
// write_hex ()
//
// writes a unsigned char as an appropriately formatted hex value
//--------------------------------------------------------------------------

void write_hex( FILE *svgFile, unsigned char val )
{
    if ( val < 16 )
    {
        fprintf( svgFile, "0%X", val );
    }
    else
    {
        fprintf( svgFile, "%X", val );
    }
}

//--------------------------------------------------------------------------
// write_unicode ()
//
// writes a unicode character, appropriately formatted (i.e. &#xNNN)
// with invalid xml characters replaced by ' '.
//--------------------------------------------------------------------------

void write_unicode( FILE *svgFile, PLUNICODE ucs4_char )
{
    if ( ucs4_char >= ' ' || ucs4_char == '\t' || ucs4_char == '\n' || ucs4_char == '\r' )
        fprintf( svgFile, "&#x%x;", ucs4_char );
    else
        fprintf( svgFile, "&#x%x;", ' ' );
}

//--------------------------------------------------------------------------
// specify_font ()
//
// Note:
// We don't actually specify a font, just the fonts properties.
// The hope is that this will give the display program the freedom
// to choose the font with the glyphs that it needs to display
// the text.
//
// Known Issues:
// (1) On OS-X 10.4 with Firefox and Camino the "serif" font-family
// looks more like the "italic" font-style.
//
//--------------------------------------------------------------------------

void specify_font( FILE *svgFile, PLUNICODE ucs4_char )
{
    fprintf( svgFile, "<tspan " );

    // sans, serif, mono, script, symbol

    if ( ( ucs4_char & 0x00F ) == 0x000 )
    {
        fprintf( svgFile, "font-family=\"sans-serif\" " );
    }
    else if ( ( ucs4_char & 0x00F ) == 0x001 )
    {
        fprintf( svgFile, "font-family=\"serif\" " );
    }
    else if ( ( ucs4_char & 0x00F ) == 0x002 )
    {
        fprintf( svgFile, "font-family=\"mono-space\" " );
    }
    else if ( ( ucs4_char & 0x00F ) == 0x003 )
    {
        fprintf( svgFile, "font-family=\"cursive\" " );
    }
    else if ( ( ucs4_char & 0x00F ) == 0x004 )
    {
        // this should be symbol, but that doesn't seem to be available
        fprintf( svgFile, "font-family=\"sans-serif\" " );
    }

    // normal, italic, oblique

    if ( ( ucs4_char & 0x0F0 ) == 0x000 )
    {
        fprintf( svgFile, "font-style=\"normal\" " );
    }
    else if ( ( ucs4_char & 0x0F0 ) == 0x010 )
    {
        fprintf( svgFile, "font-style=\"italic\" " );
    }
    else if ( ( ucs4_char & 0x0F0 ) == 0x020 )
    {
        fprintf( svgFile, "font-style=\"oblique\" " );
    }

    // normal, bold

    if ( ( ucs4_char & 0xF00 ) == 0x000 )
    {
        fprintf( svgFile, "font-weight=\"normal\">" );
    }
    else if ( ( ucs4_char & 0xF00 ) == 0x100 )
    {
        fprintf( svgFile, "font-weight=\"bold\">" );
    }
}
