// All routines that help to create a discrete legend (pllegend) or
// a continuous legend (plcolorbar).
//
// Copyright (C) 2010-2013  Hezekiah M. Carty
// Copyright (C) 2010-2014 Alan W. Irwin
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
//! Legend routines.
//!
#include "plplotP.h"

//--------------------------------------------------------------------------
//! Determine arguments of the last call to plvpor.  This gives a different
//! result than plgvpd if there is more than one subpage per page.
//!
//! @param p_xmin Pointer to a location that contains (after the call)
//! the normalized subpage coordinate of the left-hand edge of the
//! viewport.
//! @param p_xmax Pointer to a location that contains (after the call)
//! the normalized subpage coordinate of the right-hand edge of the
//! viewport.
//! @param p_ymin Pointer to a location that contains (after the call)
//! the normalized subpage coordinate of the bottom edge of the
//! viewport.
//! @param p_ymax Pointer to a location that contains (after the call)
//! the normalized subpage coordinate of the top edge of the viewport.

static void plgvpsp( PLFLT *p_xmin, PLFLT *p_xmax, PLFLT *p_ymin, PLFLT *p_ymax )
{
    if ( plsc->level < 1 )
    {
        plabort( "plgvpsp: Please call plinit first" );
        return;
    }
    if ( ( plsc->cursub <= 0 ) || ( plsc->cursub > ( plsc->nsubx * plsc->nsuby ) ) )
    {
        plabort( "plgvpsp: Please call pladv or plenv to go to a subpage" );
        return;
    }
    *p_xmin = ( plsc->vpdxmi - plsc->spdxmi ) / ( plsc->spdxma - plsc->spdxmi );
    *p_xmax = ( plsc->vpdxma - plsc->spdxmi ) / ( plsc->spdxma - plsc->spdxmi );
    *p_ymin = ( plsc->vpdymi - plsc->spdymi ) / ( plsc->spdyma - plsc->spdymi );
    *p_ymax = ( plsc->vpdyma - plsc->spdymi ) / ( plsc->spdyma - plsc->spdymi );
}

//--------------------------------------------------------------------------
//! Calculate parameters that help determine the position of the top
//! left of the legend in adopted coordinates.  See pllegend
//! documentation for definition of adopted coordinates.
//!
//! @param position Control variable containing valid combinations of the
//! following control bits that specify the 16 standard positions of
//! the legend: PL_POSITION_LEFT, PL_POSITION_RIGHT, PL_POSITION_TOP,
//! PL_POSITION_BOTTOM, PL_POSITION_INSIDE, and PL_POSITION_OUTSIDE.
//! @param legend_width Total legend width in adopted coordinates.
//! @param legend_height Total legend height in adopted coordinates.
//! @param x_legend_position Pointer to a location that contains
//! (after the call) the X value in adopted coordinates of
//! one of the 16 standard legend positions specified by position.
//! @param y_legend_position Pointer to a location that contains
//! (after the call) the Y equivalent of x_legend_position.
//! @param xsign Pointer to a location that contains (after the call)
//! the sign of the X offset relative to the standard legend position
//! specified by position.
//! @param ysign Pointer to a location that contains (after the call)
//! the Y equivalent to xsign.
//!

static void legend_position( PLINT position, PLFLT legend_width, PLFLT legend_height,
                             PLFLT *x_legend_position, PLFLT *y_legend_position,
                             PLFLT *xsign, PLFLT *ysign )
{
    // xorigin, yorigin, xlegend, and ylegend are all calculated for
    // one of the 16 standard positions specified by position and are
    // expressed in adopted coordinates.  xorigin is the X value of
    // the reference point of the adopted coordinates.  yorigin is the
    // Y value of the reference point of the adopted coordinates.
    // xlegend is the X coordinate of the top-left of the legend box
    // relative to the legend box reference point.  ylegend is the y
    // coordinate of the top-left of the legend box relative to the
    // legend box reference point.

    PLFLT xorigin = 0.0, yorigin = 0.0, xlegend = 0.0, ylegend = 0.0;
    // By default the sign of the x and y offsets is positive.
    *xsign = 1.;
    *ysign = 1.;
    if ( position & PL_POSITION_RIGHT )
    {
        xorigin = 1.;
        if ( position & PL_POSITION_TOP )
        {
            yorigin = 1.;
            if ( position & PL_POSITION_INSIDE )
            {
                xlegend = -legend_width;
                ylegend = 0.;
                *xsign  = -1.;
                *ysign  = -1.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                xlegend = 0.;
                ylegend = legend_height;
            }
            else
            {
                plexit( "legend_position: internal logic error 1" );
            }
        }
        else if ( !( position & PL_POSITION_TOP ) && !( position & PL_POSITION_BOTTOM ) )
        {
            yorigin = 0.5;
            ylegend = 0.5 * legend_height;
            if ( position & PL_POSITION_INSIDE )
            {
                xlegend = -legend_width;
                *xsign  = -1.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                xlegend = 0.;
            }
            else
            {
                plexit( "legend_position: internal logic error 2" );
            }
        }
        else if ( position & PL_POSITION_BOTTOM )
        {
            yorigin = 0.;
            if ( position & PL_POSITION_INSIDE )
            {
                xlegend = -legend_width;
                ylegend = legend_height;
                *xsign  = -1.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                xlegend = 0.;
                ylegend = 0.;
                *ysign  = -1.;
            }
            else
            {
                plexit( "legend_position: internal logic error 3" );
            }
        }
        else
        {
            plexit( "legend_position: internal logic error 4" );
        }
    }
    else if ( !( position & PL_POSITION_RIGHT ) && !( position & PL_POSITION_LEFT ) )
    {
        xorigin = 0.5;
        xlegend = -0.5 * legend_width;
        if ( position & PL_POSITION_TOP )
        {
            yorigin = 1.;
            if ( position & PL_POSITION_INSIDE )
            {
                ylegend = 0.;
                *ysign  = -1.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                ylegend = legend_height;
            }
            else
            {
                plexit( "legend_position: internal logic error 5" );
            }
        }
        else if ( position & PL_POSITION_BOTTOM )
        {
            yorigin = 0.;
            if ( position & PL_POSITION_INSIDE )
            {
                ylegend = legend_height;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                ylegend = 0.;
                *ysign  = -1.;
            }
            else
            {
                plexit( "legend_position: internal logic error 6" );
            }
        }
        else
        {
            plexit( "legend_position: internal logic error 7" );
        }
    }
    else if ( position & PL_POSITION_LEFT )
    {
        xorigin = 0.;
        if ( position & PL_POSITION_TOP )
        {
            yorigin = 1.;
            if ( position & PL_POSITION_INSIDE )
            {
                xlegend = 0.;
                ylegend = 0.;
                *ysign  = -1.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                xlegend = -legend_width;
                ylegend = legend_height;
                *xsign  = -1.;
            }
            else
            {
                plexit( "legend_position: internal logic error 8" );
            }
        }
        else if ( !( position & PL_POSITION_TOP ) && !( position & PL_POSITION_BOTTOM ) )
        {
            yorigin = 0.5;
            ylegend = 0.5 * legend_height;
            if ( position & PL_POSITION_INSIDE )
            {
                xlegend = 0.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                xlegend = -legend_width;
                *xsign  = -1.;
            }
            else
            {
                plexit( "legend_position: internal logic error 9" );
            }
        }
        else if ( position & PL_POSITION_BOTTOM )
        {
            yorigin = 0.;
            if ( position & PL_POSITION_INSIDE )
            {
                ylegend = legend_height;
                xlegend = 0.;
            }
            else if ( position & PL_POSITION_OUTSIDE )
            {
                xlegend = -legend_width;
                ylegend = 0.;
                *xsign  = -1.;
                *ysign  = -1.;
            }
            else
            {
                plexit( "legend_position: internal logic error 10" );
            }
        }
        else
        {
            plexit( "legend_position: internal logic error 11" );
        }
    }
    else
    {
        plexit( "legend_position: internal logic error 12" );
    }
    *x_legend_position = xorigin + xlegend;
    *y_legend_position = yorigin + ylegend;
}

//--------------------------------------------------------------------------
//! Obtain ratio of normalized subpage to mm coordinates in both x and y.
//!
//! @param x_subpage_per_mm Pointer to a location that contains (after the call) the x ratio.
//! @param y_subpage_per_mm Pointer to a location that contains (after the call) the y ratio.
//!

static void get_subpage_per_mm( PLFLT *x_subpage_per_mm, PLFLT *y_subpage_per_mm )
{
    // Size of subpage in mm
    PLFLT mxmin, mxmax, mymin, mymax;
    plgspa( &mxmin, &mxmax, &mymin, &mymax );
    *x_subpage_per_mm = 1. / ( mxmax - mxmin );
    *y_subpage_per_mm = 1. / ( mymax - mymin );
}

//--------------------------------------------------------------------------
//! Obtain character or symbol height in (y) in normalized subpage coordinates.
//!
//! @param ifcharacter TRUE obtain character height, FALSE obtain symbol
//! height.
//! @return character or symbol height.
//!

static PLFLT get_character_or_symbol_height( PLBOOL ifcharacter )
{
    // Character height in mm
    PLFLT default_mm, char_height_mm;
    PLFLT x_subpage_per_mm, y_subpage_per_mm;

    if ( ifcharacter )
    {
        plgchr( &default_mm, &char_height_mm );
    }
    else
    {
        default_mm     = plsc->symdef;
        char_height_mm = plsc->symht;
    }
    get_subpage_per_mm( &x_subpage_per_mm, &y_subpage_per_mm );
    return ( char_height_mm * y_subpage_per_mm );
}

//--------------------------------------------------------------------------
//! Convert from adopted X coordinate to normalized subpage X
//! coordinate.  The *_adopted variables are the normalized subpage
//! limits of the adopted viewport which is either the external
//! viewport (PL_POSITION_VIEWPORT bit set in the position parameter)
//! or the subpage itself (PL_POSITION_SUBPAGE bit set in the position
//! parameter, in which case this is an identity transform).  See
//! pllegend documentation for more details concerning the adopted
//! coordinates.
//!
//! @param nx Adopted X coordinate.
//!

#define adopted_to_subpage_x( nx )    ( ( xdmin_adopted ) + ( nx ) * ( ( xdmax_adopted ) - ( xdmin_adopted ) ) )

//--------------------------------------------------------------------------
//! Convert from normalized subpage X coordinate to adopted X
//! coordinate.  The *_adopted variables are the normalized subpage
//! limits of the adopted viewport which is either the external
//! viewport (PL_POSITION_VIEWPORT bit set in the position parameter)
//! or the subpage itself (PL_POSITION_SUBPAGE bit set in the position
//! parameter, in which case this is an identity transform).  See
//! pllegend documentation for more details concerning the adopted
//! coordinates.
//!
//! @param nx Normalized subpage X coordinate.
//!

#define subpage_to_adopted_x( nx )    ( ( nx - xdmin_adopted ) / ( ( xdmax_adopted ) - ( xdmin_adopted ) ) )

//--------------------------------------------------------------------------
//! Convert from adopted Y coordinate to normalized subpage Y
//! coordinate.  The *_adopted variables are the normalized subpage
//! limits of the adopted viewport which is either the external
//! viewport (PL_POSITION_VIEWPORT bit set in the position parameter)
//! or the subpage itself (PL_POSITION_SUBPAGE bit set in the position
//! parameter, in which case this is an identity transform).  See
//! pllegend documentation for more details concerning the adopted
//! coordinates.
//!
//! @param ny Adopted Y coordinate.
//!

#define adopted_to_subpage_y( ny )    ( ( ydmin_adopted ) + ( ny ) * ( ( ydmax_adopted ) - ( ydmin_adopted ) ) )

//--------------------------------------------------------------------------
//! Convert from normalized subpage Y coordinate to adopted Y
//! coordinate.  The *_adopted variables are the normalized subpage
//! limits of the adopted viewport which is either the external
//! viewport (PL_POSITION_VIEWPORT bit set in the position parameter)
//! or the subpage itself (PL_POSITION_SUBPAGE bit set in the position
//! parameter, in which case this is an identity transform).  See
//! pllegend documentation for more details concerning the adopted
//! coordinates.
//!
//! @param ny Normalized subpage Y coordinate.
//!

#define subpage_to_adopted_y( ny )    ( ( ny - ydmin_adopted ) / ( ( ydmax_adopted ) - ( ydmin_adopted ) ) )

//--------------------------------------------------------------------------
//! Plot legend using discretely annotated filled boxes, lines, and/or lines
//! of symbols.
//!
//! (N.B. the adopted coordinate system used for some of the parameters
//! is defined in the documentation of the position parameter.)
//!
//! @param p_legend_width Pointer to a location which contains (after
//! the call) the legend width in adopted coordinates.  This quantity
//! is calculated from the plot_width and text_offset arguments, the
//! ncolumn argument (possibly modified inside the routine depending
//! on the nlegend and nrow arguments), and the length (calculated
//! internally) of the longest text string.
//! @param p_legend_height Pointer to a location which contains (after
//! the call) the legend height in adopted coordinates.  This
//! quantity is calculated from the text_scale and text_spacing
//! arguments and the nrow argument (possibly modified inside the
//! routine depending on the nlegend and ncolumn arguments).
//! @param opt This variable contains bits which control the overall
//! legend.  If the PL_LEGEND_TEXT_LEFT bit is set, put the text area
//! on the left of the legend and the plotted area on the right.
//! Otherwise, put the text area on the right of the legend and the
//! plotted area on the left.  If the PL_LEGEND_BACKGROUND bit is set,
//! plot a (semi-transparent) background for the legend.  If the
//! PL_LEGEND_BOUNDING_BOX bit is set, plot a bounding box for the
//! legend.  If the PL_LEGEND_ROW_MAJOR bit is set and both (of the
//! possibly internally transformed) nrow > 1 and ncolumn > 1, then
//! plot the resulting array of legend entries in row-major order.
//! Otherwise, plot the legend entries in column-major order.
//! @param position This variable contains bits which control the
//! overall position of the legend and the definition of the adopted
//! coordinates used for positions just like what is done for the
//! position argument for plcolorbar.  However, note that the
//! defaults for the position bits (see below) are different than the
//! plcolorbar case.  The combination of the PL_POSITION_LEFT,
//! PL_POSITION_RIGHT, PL_POSITION_TOP, PL_POSITION_BOTTOM,
//! PL_POSITION_INSIDE, and PL_POSITION_OUTSIDE bits specifies one of
//! the 16 possible standard positions (the 4 corners and 4 centers of
//! the sides for both the inside and outside cases) of the legend
//! relative to the adopted coordinate system.  The corner positions
//! are specified by the appropriate combination of two of the
//! PL_POSITION_LEFT, PL_POSITION_RIGHT, PL_POSITION_TOP, and
//! PL_POSITION_BOTTOM bits while the sides are specified by a single
//! value of one of those bits.  The adopted coordinates are
//! normalized viewport coordinates if the PL_POSITION_VIEWPORT bit is
//! set or normalized subpage coordinates if the PL_POSITION_SUBPAGE
//! bit is set.  Default position bits: If none of PL_POSITION_LEFT,
//! PL_POSITION_RIGHT, PL_POSITION_TOP, or PL_POSITION_BOTTOM are set,
//! then the combination of PL_POSITION_RIGHT and PL_POSITION_TOP.  If
//! neither of PL_POSITION_INSIDE or PL_POSITION_OUTSIDE is set, use
//! PL_POSITION_INSIDE.  If neither of PL_POSITION_VIEWPORT or
//! PL_POSITION_SUBPAGE is set, use PL_POSITION_VIEWPORT.
//! @param x X offset of the legend position in adopted coordinates
//! from the specified standard position of the legend.  For positive
//! x, the direction of motion away from the standard position is
//! inward/outward from the standard corner positions or standard left
//! or right positions if the PL_POSITION_INSIDE/PL_POSITION_OUTSIDE
//! bit is set in position.  For the standard top or bottom positions,
//! the direction of motion for positive x is toward positive X.
//! @param y Y offset of the legend position in adopted coordinates
//! from the specified standard position of the legend.  For positive
//! y, the direction of motion away from the standard position is
//! inward/outward from the standard corner positions or standard top
//! or bottom positions if the PL_POSITION_INSIDE/PL_POSITION_OUTSIDE
//! bit is set in position.  For the standard left or right positions,
//! the direction of motion for positive y is toward positive Y.
//! @param plot_width Horizontal width in adopted coordinates of the
//! plot area (where colored boxes, lines, and/or symbols are drawn in
//! the legend).
//! @param bg_color The cmap0 color of the background for the legend
//! (PL_LEGEND_BACKGROUND).
//! @param bb_color The cmap0 color of the bounding-box
//! line for the legend (PL_LEGEND_BOUNDING_BOX).
//! @param bb_style The pllsty style number for the bounding-box line
//! for the legend (PL_LEGEND_BOUNDING_BOX).
//! @param nrow Number of rows in the matrix used to render the
//! nlegend legend entries.  For internal transformations of nrow, see
//! further remarks under nlegend.
//! @param ncolumn Number of columns in the matrix used to render the
//! nlegend legend entries.  For internal transformations of ncolumn,
//! see further remarks under nlegend.
//! @param nlegend Number of legend entries.  The above nrow and
//! ncolumn values are transformed internally to be consistent with
//! nlegend.  If either nrow or ncolumn is non-positive it is replaced
//! by 1.  If the resulting product of nrow and ncolumn is less than
//! nlegend, the smaller of the two (or nrow, if nrow == ncolumn) is
//! increased so the product is >= nlegend.  Thus, for example, the
//! common nrow = 0, ncolumn = 0 case is transformed internally to
//! nrow = nlegend, ncolumn = 1; i.e., the usual case of a legend
//! rendered as a single column.
//! @param opt_array Array of nlegend values of options to control
//! each individual plotted area corresponding to a legend entry.  If
//! the PL_LEGEND_NONE bit is set, then nothing is plotted in the
//! plotted area.  If the PL_LEGEND_COLOR_BOX, PL_LEGEND_LINE, and/or
//! PL_LEGEND_SYMBOL bits are set, the plotted area corresponding to a
//! legend entry is specified with a colored box; a line; and/or a
//! line of symbols.
//! @param text_offset Offset of the text area from the plot area in
//! units of character width.
//! @param text_scale Character height scale for text annotations.
//! @param text_spacing Vertical spacing in units of the character
//! height from one legend entry to the next.
//! @param text_justification Justification parameter used for text
//! justification.  The most common values of text_justification are
//! 0., 0.5, or 1. corresponding to a text that is left justified,
//! centred, or right justified within the text area, but other values
//! are allowed as well.
//! @param text_colors Array of nlegend cmap0 text colors.
//! @param text Array of nlegend pointers to null-terminated text
//! annotation strings.  Like other PLplot strings specified by the
//! user, the string must be UTF-8 (including its ascii subset) and
//! may include any of the PLplot text escapes.
//! @param box_colors Array of nlegend cmap0 colors for
//! the discrete colored boxes (PL_LEGEND_COLOR_BOX).
//! @param box_patterns Array of nlegend patterns (plpsty indices)
//! for the discrete colored boxes (PL_LEGEND_COLOR_BOX).
//! @param box_scales Array of nlegend scales (units of fraction of
//! character height) for the height of the discrete colored boxes
//! (PL_LEGEND_COLOR_BOX).
//! @param box_line_widths Array of nlegend line widths for the
//! patterns specified by box_patterns (PL_LEGEND_COLOR_BOX).
//! @param line_colors Array of nlegend cmap0 line colors
//! (PL_LEGEND_LINE).
//! @param line_styles Array of nlegend line styles (pllsty indices)
//! (PL_LEGEND_LINE).
//! @param line_widths Array of nlegend line widths (PL_LEGEND_LINE).
//! @param symbol_colors Array of nlegend cmap0 symbol colors
//! (PL_LEGEND_SYMBOL).
//! @param symbol_scales Array of nlegend scale values for the
//! symbol height (PL_LEGEND_SYMBOL).
//! @param symbol_numbers Array of nlegend numbers of symbols to be
//! drawn across the width of the plotted area (PL_LEGEND_SYMBOL).
//! @param symbols Array of nlegend pointers to null-terminated
//! strings which represent the glyph for the symbol to be plotted
//! (PL_LEGEND_SYMBOL).  Like other PLplot strings specified by the
//! user, the string must be UTF-8 (including its ascii subset) and
//! may include any of the PLplot text escapes.
//!

void
c_pllegend( PLFLT *p_legend_width, PLFLT *p_legend_height,
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
            PLINT_VECTOR symbol_numbers, PLCHAR_MATRIX symbols )

{
    // Legend position
    PLFLT plot_x, plot_x_end, plot_x_subpage, plot_x_end_subpage;
    PLFLT plot_y, plot_y_subpage;
    PLFLT text_x, text_y, text_x_subpage, text_y_subpage;
    // Character height (normalized subpage coordinates)
    PLFLT character_height, character_width, symbol_width = 0.0;
    // x, y-position of the current legend entry
    PLFLT ty, xshift, drow, dcolumn;
    // Positions of the legend entries
    PLFLT dxs, *xs = NULL, *ys = NULL, xl[2], yl[2], xbox[4], ybox[4];
    PLINT i, j;
    // Active attributes to be saved and restored afterward.
    PLINT col0_save       = plsc->icol0,
          line_style_save = plsc->line_style,
          pattern_save    = plsc->patt;
    PLFLT line_width_save = plsc->width;
    PLFLT text_scale_save = plsc->chrht / plsc->chrdef;
    // Saved external world coordinates of viewport.
    PLFLT xwmin_save, xwmax_save, ywmin_save, ywmax_save;
    // Saved external normalized coordinates of viewport.
    // (These are actual values used only for the restore.)
    PLFLT xdmin_save = 0.0, xdmax_save = 0.0, ydmin_save = 0.0, ydmax_save = 0.0;
    // Limits of adopted coordinates used to calculate all coordinate
    // transformations.
    PLFLT xdmin_adopted = 0.0, xdmax_adopted = 0.0, ydmin_adopted = 0.0, ydmax_adopted = 0.0;

    PLFLT x_subpage_per_mm, y_subpage_per_mm, text_width0 = 0., text_width;
    PLFLT width_border, column_separation,
          legend_width, legend_height, legend_width_ac, legend_height_ac;
    PLFLT x_legend_position, y_legend_position, xsign, ysign;

    PLINT some_symbols       = 0;
    PLINT max_symbol_numbers = 0;
    PLINT irow       = 0, icolumn = 0;
    int   some_boxes = 0, some_lines = 0;

    // Default nrow, ncolumn.
    nrow    = MAX( nrow, 1 );
    ncolumn = MAX( ncolumn, 1 );
    if ( nrow * ncolumn < nlegend )
    {
        // Make smaller one large enough to accomodate nlegend.
        if ( ncolumn < nrow )
            ncolumn = ( nlegend % nrow ) ? ( nlegend / nrow ) + 1 : nlegend / nrow;
        else
            nrow = ( nlegend % ncolumn ) ? ( nlegend / ncolumn ) + 1 : nlegend / ncolumn;
    }
    // fprintf(stdout, "nrow, ncolumn = %d, %d\n", nrow, ncolumn);

    // Default position flags and sanity checks for position flags.
    if ( !( position & PL_POSITION_RIGHT ) && !( position & PL_POSITION_LEFT ) && !( position & PL_POSITION_TOP ) && !( position & PL_POSITION_BOTTOM ) )
    {
        position = position | PL_POSITION_RIGHT | PL_POSITION_TOP;
    }
    else if ( ( position & PL_POSITION_RIGHT ) && ( position & PL_POSITION_LEFT ) )
    {
        plabort( "pllegend: PL_POSITION_RIGHT and PL_POSITION_LEFT cannot be simultaneously set." );
        return;
    }

    else if ( ( position & PL_POSITION_TOP ) && ( position & PL_POSITION_BOTTOM ) )
    {
        plabort( "pllegend: PL_POSITION_TOP and PL_POSITION_BOTTOM cannot be simultaneously set." );
        return;
    }

    if ( !( position & PL_POSITION_INSIDE ) && !( position & PL_POSITION_OUTSIDE ) )
    {
        position = position | PL_POSITION_INSIDE;
    }
    else if ( ( position & PL_POSITION_INSIDE ) && ( position & PL_POSITION_OUTSIDE ) )
    {
        plabort( "pllegend: PL_POSITION_INSIDE and PL_POSITION_OUTSIDE cannot be simultaneously set." );
        return;
    }

    if ( !( position & PL_POSITION_VIEWPORT ) && !( position & PL_POSITION_SUBPAGE ) )
    {
        position = position | PL_POSITION_VIEWPORT;
    }
    else if ( ( position & PL_POSITION_VIEWPORT ) && ( position & PL_POSITION_SUBPAGE ) )
    {
        plabort( "pllegend: PL_POSITION_VIEWPORT and PL_POSITION_SUBPAGE cannot be simultaneously set." );
        return;
    }

    // xdmin_save, etc., are the actual external relative viewport
    // coordinates within the current sub-page used only for
    // restoration at the end.
    plgvpsp( &xdmin_save, &xdmax_save, &ydmin_save, &ydmax_save );

    // Choose adopted coordinates.
    if ( position & PL_POSITION_SUBPAGE )
        plvpor( 0., 1., 0., 1. );

    // xdmin_adopted, etc., are the adopted limits of the coordinates
    // within the current sub-page used for all coordinate
    // transformations.
    // If position & PL_POSITION_VIEWPORT is true, these limits
    // are the external relative viewport limits.
    // If position & PL_POSITION_SUBPAGE is true, these
    // coordinates are the relative subpage coordinates.
    plgvpsp( &xdmin_adopted, &xdmax_adopted, &ydmin_adopted, &ydmax_adopted );

    // xwmin_save, etc., are the external world coordinates corresponding
    // to the external viewport boundaries.
    plgvpw( &xwmin_save, &xwmax_save, &ywmin_save, &ywmax_save );

    // Internal viewport corresponds to sub-page so that all legends will
    // be clipped at sub-page boundaries.
    plvpor( 0., 1., 0., 1. );

    // Internal world coordinates are the same as normalized internal
    // viewport coordinates which are the same as normalized subpage coordinates.
    plwind( 0., 1., 0., 1. );

    for ( i = 0; i < nlegend; i++ )
    {
        if ( opt_array[i] & PL_LEGEND_COLOR_BOX )
            some_boxes = 1;
        if ( opt_array[i] & PL_LEGEND_LINE )
            some_lines = 1;
        if ( opt_array[i] & PL_LEGEND_SYMBOL )
        {
            if ( symbol_numbers != NULL )
                max_symbol_numbers = MAX( max_symbol_numbers, symbol_numbers[i] );
            some_symbols = 1;
        }
    }

    // Sanity checks on NULL arrays:
    if ( some_boxes && ( box_colors == NULL || box_patterns == NULL || box_scales == NULL || box_line_widths == NULL ) )
    {
        plabort( "pllegend: all box_* arrays must be defined when the PL_LEGEND_COLOR_BOX bit is set." );
        return;
    }

    if ( some_lines && ( line_colors == NULL || line_styles == NULL || line_widths == NULL ) )
    {
        plabort( "pllegend: all line_* arrays must be defined when the PL_LEGEND_LINE bit is set." );
        return;
    }

    if ( some_symbols && ( symbol_colors == NULL || symbol_scales == NULL || symbol_numbers == NULL ) )
    {
        plabort( "pllegend: all symbol_* arrays must be defined when the PL_LEGEND_SYMBOL bit is set." );
        return;
    }

    // Get character height and width in normalized subpage coordinates.
    character_height = get_character_or_symbol_height( TRUE );
    character_width  = character_height;

    // Calculate maximum width of text area.
    plschr( 0., text_scale );
    for ( i = 0; i < nlegend; i++ )
    {
        // units are mm.
        text_width0 = MAX( text_width0, plstrl( text[i] ) );
    }
    get_subpage_per_mm( &x_subpage_per_mm, &y_subpage_per_mm );

    // units are normalized subpage coordinates.
    text_width0 = x_subpage_per_mm * text_width0;

    // Allow gap on end closest to legend plot.
    text_width = text_width0 + text_offset * character_width;

    // Allow small border area where only the background is plotted
    // for left and right of legend.  0.4 seems to be a reasonable factor
    // that gives a good-looking result.
    width_border = 0.4 * character_width;
    // Separate columns (if any) by 2.0 * character_width.
    column_separation = 2.0 * character_width;

    // Total width and height of legend area in normalized subpage coordinates.
    legend_width = 2. * width_border + ( ncolumn - 1 ) * column_separation +
                   ncolumn * ( text_width +
                               adopted_to_subpage_x( plot_width ) - adopted_to_subpage_x( 0. ) );
    legend_height = nrow * text_spacing * character_height;

    // Total width and height of legend area in adopted coordinates.

    legend_width_ac  = subpage_to_adopted_x( legend_width ) - subpage_to_adopted_x( 0. );
    legend_height_ac = subpage_to_adopted_y( legend_height ) - subpage_to_adopted_y( 0. );
    *p_legend_width  = legend_width_ac;
    *p_legend_height = legend_height_ac;

    // dcolumn is the spacing from one column to the next and
    // drow is the spacing from one row to the next.
    dcolumn = column_separation + text_width +
              adopted_to_subpage_x( plot_width ) - adopted_to_subpage_x( 0. );
    drow = text_spacing * character_height;

    legend_position( position, legend_width_ac, legend_height_ac, &x_legend_position, &y_legend_position, &xsign, &ysign );
    plot_x     = x * xsign + x_legend_position;
    plot_y     = y * ysign + y_legend_position;
    plot_x_end = plot_x + plot_width;
    // Normalized subpage coordinates for legend plots
    plot_x_subpage     = adopted_to_subpage_x( plot_x );
    plot_y_subpage     = adopted_to_subpage_y( plot_y );
    plot_x_end_subpage = adopted_to_subpage_x( plot_x_end );

    // Get normalized subpage positions of the start of the legend text
    text_x         = plot_x_end;
    text_y         = plot_y;
    text_x_subpage = adopted_to_subpage_x( text_x ) +
                     text_offset * character_width;
    text_y_subpage = adopted_to_subpage_y( text_y );

    if ( opt & PL_LEGEND_BACKGROUND )
    {
        PLFLT xbg[4] = {
            plot_x_subpage,
            plot_x_subpage,
            plot_x_subpage + legend_width,
            plot_x_subpage + legend_width,
        };
        PLFLT ybg[4] = {
            plot_y_subpage,
            plot_y_subpage - legend_height,
            plot_y_subpage - legend_height,
            plot_y_subpage,
        };
        plpsty( 0 );
        plcol0( bg_color );
        plfill( 4, xbg, ybg );
        plcol0( col0_save );
    }

    if ( opt & PL_LEGEND_BOUNDING_BOX )
    {
        PLFLT xbb[5] = {
            plot_x_subpage,
            plot_x_subpage,
            plot_x_subpage + legend_width,
            plot_x_subpage + legend_width,
            plot_x_subpage,
        };
        PLFLT ybb[5] = {
            plot_y_subpage,
            plot_y_subpage - legend_height,
            plot_y_subpage - legend_height,
            plot_y_subpage,
            plot_y_subpage,
        };
        pllsty( bb_style );
        plcol0( bb_color );
        plline( 5, xbb, ybb );
        plcol0( col0_save );
        pllsty( line_style_save );
    }

    if ( opt & PL_LEGEND_TEXT_LEFT )
    {
        // text area on left, plot area on right.
        text_x_subpage      = plot_x_subpage;
        plot_x_subpage     += text_width;
        plot_x_end_subpage += text_width;
    }
    // adjust border after background is drawn.
    plot_x_subpage     += width_border;
    plot_x_end_subpage += width_border;
    text_x_subpage     += width_border;

    if ( some_symbols )
    {
        max_symbol_numbers = MAX( 2, max_symbol_numbers );
        if ( ( ( xs = (PLFLT *) malloc( (size_t) max_symbol_numbers * sizeof ( PLFLT ) ) ) == NULL ) ||
             ( ( ys = (PLFLT *) malloc( (size_t) max_symbol_numbers * sizeof ( PLFLT ) ) ) == NULL ) )
        {
            plexit( "pllegend: Insufficient memory" );
        }

        // Get symbol width in normalized subpage coordinates if symbols are plotted to
        // adjust ends of line of symbols.
        // AWI, no idea why must use 0.5 factor to get ends of symbol lines
        // to line up approximately correctly with plotted legend lines.
        // Factor should be unity.
        symbol_width = 0.5 * get_character_or_symbol_height( TRUE );
    }

    // Draw each legend entry
    for ( i = 0; i < nlegend; i++ )
    {
        // y position of text, lines, symbols, and/or centre of cmap0 box.
        ty     = text_y_subpage - ( (double) irow + 0.5 ) * drow;
        xshift = (double) icolumn * dcolumn;
        // Label/name for the legend
        plcol0( text_colors[i] );
        plschr( 0., text_scale );
        plptex( text_x_subpage + xshift + text_justification * text_width0, ty, 0.1, 0.0, text_justification, text[i] );

        if ( !( opt_array[i] & PL_LEGEND_NONE ) )
        {
            if ( opt_array[i] & PL_LEGEND_COLOR_BOX )
            {
                plcol0( box_colors[i] );
                plpsty( box_patterns[i] );
                plwidth( box_line_widths[i] );
                xbox[0] = plot_x_subpage + xshift;
                xbox[1] = xbox[0];
                xbox[2] = plot_x_end_subpage + xshift;
                xbox[3] = xbox[2];
                ybox[0] = ty + 0.5 * drow * box_scales[i];
                ybox[1] = ty - 0.5 * drow * box_scales[i];
                ybox[2] = ty - 0.5 * drow * box_scales[i];
                ybox[3] = ty + 0.5 * drow * box_scales[i];
                plfill( 4, xbox, ybox );
                pllsty( line_style_save );
                plwidth( line_width_save );
            }
            if ( opt_array[i] & PL_LEGEND_LINE )
            {
                plcol0( line_colors[i] );
                pllsty( line_styles[i] );
                plwidth( line_widths[i] );
                xl[0] = plot_x_subpage + xshift;
                xl[1] = plot_x_end_subpage + xshift;
                yl[0] = ty;
                yl[1] = ty;
                plline( 2, xl, yl );
                pllsty( line_style_save );
                plwidth( line_width_save );
            }

            if ( opt_array[i] & PL_LEGEND_SYMBOL )
            {
                plcol0( symbol_colors[i] );
                plschr( 0., symbol_scales[i] );
                dxs = ( plot_x_end_subpage - plot_x_subpage - symbol_width ) / (double) ( MAX( symbol_numbers[i], 2 ) - 1 );
                for ( j = 0; j < symbol_numbers[i]; j++ )
                {
                    xs[j] = plot_x_subpage + xshift +
                            0.5 * symbol_width + dxs * (double) j;
                    ys[j] = ty;
                }
                plstring( symbol_numbers[i], xs, ys, symbols[i] );
            }
        }

        // Set irow, icolumn for next i value.
        if ( opt & PL_LEGEND_ROW_MAJOR )
        {
            icolumn++;
            if ( icolumn >= ncolumn )
            {
                icolumn = 0;
                irow++;
            }
        }
        else
        {
            irow++;
            if ( irow >= nrow )
            {
                irow = 0;
                icolumn++;
            }
        }
    }
    if ( some_symbols )
    {
        free( xs );
        free( ys );
    }

    // Restore
    plcol0( col0_save );
    plschr( 0., text_scale_save );
    plpsty( pattern_save );
    plvpor( xdmin_save, xdmax_save, ydmin_save, ydmax_save );
    plwind( xwmin_save, xwmax_save, ywmin_save, ywmax_save );

    return;
}

//--------------------------------------------------------------------------
//! Remove specified ascii characters from null-terminated string.
//!
//! @param string Null-terminated string where specified characters
//! (if present) are removed.
//! @param characters Null-terminated string consisting of ascii characters
//! to be removed from string.

static void
remove_characters( char *string, PLCHAR_VECTOR characters )
{
    char          *src, *dst;
    PLCHAR_VECTOR ptr;
    for ( src = dst = string; *src != '\0'; src++ )
    {
        ptr = characters;
        while ( ( *ptr != '\0' ) && ( *src != *ptr ) )
            ptr++;
        if ( *src != *ptr )
        {
            *dst = *src;
            dst++;
        }
    }
    *dst = '\0';
}

//--------------------------------------------------------------------------
//! Draw triangular end-caps for color bars.
//!
//! @param if_edge If if_edge is true/false, draw an edge/no edge around
//! the triangle.
//! @param orientation This variable defines the orientation of
//! the triangle.  The triangle points to the right, up, left, or down
//! if orientation contains PL_COLORBAR_ORIENT_RIGHT,
//! PL_COLORBAR_ORIENT_TOP, PL_COLORBAR_ORIENT_LEFT, or
//! PL_COLORBAR_ORIENT_BOTTOM bits.
//! @param xmin Minimum world X coordinate of rectangle
//! inscribing the triangle.
//! @param xmax Maximum world X coordinate of rectangle
//! inscribing the triangle.
//! @param ymin Minimum world Y coordinate of rectangle
//! inscribing the triangle.
//! @param ymax Maximum world Y coordinate of rectangle
//! inscribing the triangle.
//! @param color The cmap1 color used to fill the end cap.
//!

static void
draw_cap( PLBOOL if_edge, PLINT orientation, PLFLT xmin, PLFLT xmax,
          PLFLT ymin, PLFLT ymax, PLFLT color )
{
    // Save current drawing color.
    PLINT col0_save = plsc->icol0;
    PLFLT xhalf     = 0.5 * ( xmin + xmax );
    PLFLT yhalf     = 0.5 * ( ymin + ymax );

    // World coordinates for the triangle.  Due to setup in the
    // plcolorbar routine that calls this, these are also normalized
    // subpage coordinates.
    PLFLT xs[3];
    PLFLT ys[3];

    if ( orientation == PL_COLORBAR_ORIENT_RIGHT )
    {
        xs[0] = xmin;
        ys[0] = ymin;
        xs[1] = xmax;
        ys[1] = yhalf;
        xs[2] = xmin;
        ys[2] = ymax;
    }
    else if ( orientation == PL_COLORBAR_ORIENT_TOP )
    {
        xs[0] = xmax;
        ys[0] = ymin;
        xs[1] = xhalf;
        ys[1] = ymax;
        xs[2] = xmin;
        ys[2] = ymin;
    }
    else if ( orientation == PL_COLORBAR_ORIENT_LEFT )
    {
        xs[0] = xmax;
        ys[0] = ymax;
        xs[1] = xmin;
        ys[1] = yhalf;
        xs[2] = xmax;
        ys[2] = ymin;
    }
    else if ( orientation == PL_COLORBAR_ORIENT_BOTTOM )
    {
        xs[0] = xmin;
        ys[0] = ymax;
        xs[1] = xhalf;
        ys[1] = ymin;
        xs[2] = xmax;
        ys[2] = ymax;
    }
    else
    {
        plexit( "draw_cap: internal error. Incorrect orientation" );
    }

    plcol1( color );
    plfill( 3, xs, ys );
    // Restore the drawing color
    plcol0( col0_save );

    // Draw cap outline
    if ( if_edge )
        plline( 3, xs, ys );
}

//--------------------------------------------------------------------------
//! Draw box (when if_bb FALSE) around color bar with possible tick
//! marks, numerical labels of those tick marks, and exponent
//! decorations of that box or else (when if_bb TRUE) calculate
//! bounding box of that decorated box.
//!
//! @param if_bb If if_bb is TRUE evaluate bounding box of decorated
//! box.  If if_bb is FALSE draw the decorated box.
//! @param opt Can contain the same control bits as the opt argument
//! for plcolorbar.  However, the only bits that are relevant here are
//! PL_COLORBAR_SHADE, PL_COLORBAR_SHADE_LABEL, and
//! PL_COLORBAR_ORIENT_(RIGHT|TOP|LEFT|BOTTOM).  For full
//! documentation of these bits, see the documentation of opt for
//! plcolorbar.
//! @param axis_opts Axis options for the color bar's major axis, as
//! for plcolorbar.
//! @param if_edge If if_edge is TRUE (FALSE) do (do not) draw the edge of
//! the colorbox.
//! @param ticks As for plcolorbar.
//! @param sub_ticks As for plcolorbar.
//! @param n_values As for plcolorbar.
//! @param values As for plcolorbar.

static void
draw_box( PLBOOL if_bb, PLINT opt, PLCHAR_VECTOR axis_opts, PLBOOL if_edge,
          PLFLT ticks, PLINT sub_ticks, PLINT n_values, PLFLT_VECTOR values )
{
    // axis option strings.
    PLCHAR_VECTOR edge_string;
    size_t        length_axis_opts = strlen( axis_opts );
    char          *local_axis_opts;

    // local_axis_opts is local version that can be modified from
    // const input version.
    if ( ( local_axis_opts = (char *) malloc( ( length_axis_opts + 1 ) * sizeof ( char ) ) ) == NULL )
    {
        plexit( "draw_box: Insufficient memory" );
    }
    strcpy( local_axis_opts, axis_opts );

    plsc->if_boxbb = if_bb;
    // Draw numerical labels and tick marks if this is a shade color bar
    // TODO: A better way to handle this would be to update the
    // internals of plbox to support custom tick and label positions
    // along an axis.

    if ( opt & PL_COLORBAR_SHADE && opt & PL_COLORBAR_SHADE_LABEL )
    {
        if ( opt & PL_COLORBAR_ORIENT_RIGHT || opt & PL_COLORBAR_ORIENT_LEFT )
            label_box_custom( local_axis_opts, n_values, values, "", 0, NULL );
        else
            label_box_custom( "", 0, NULL, local_axis_opts, n_values, values );
        if ( if_bb )
        {
            plsc->if_boxbb = FALSE;
            free( local_axis_opts );
            return;
        }
        // Exclude ticks for plbox call below since those tick marks and
        // associated labels have already been handled in a custom way above.
        remove_characters( local_axis_opts, "TtXx" );
    }

    // Draw the outline for the entire color bar, tick marks, tick labels.

    if ( if_edge )
        edge_string = "bc";
    else
        edge_string = "uw";
    if ( opt & PL_COLORBAR_ORIENT_TOP || opt & PL_COLORBAR_ORIENT_BOTTOM )
    {
        plbox( edge_string, 0.0, 0, local_axis_opts, ticks, sub_ticks );
    }
    else
    {
        plbox( local_axis_opts, ticks, sub_ticks, edge_string, 0.0, 0 );
    }
    plsc->if_boxbb = FALSE;

    free( local_axis_opts );
}

//--------------------------------------------------------------------------
//! Draw label (when if_bb FALSE) with appropriate position relative to the
//! "inner" bounding-box of the decorated box around the color bar
//! or else (when if_bb TRUE) calculate combined bounding box of that label
//! + decorated box.
//!
//! @param if_bb If if_bb is TRUE evaluate bounding box of combined
//! decorated box + label.  If if_bb is FALSE draw the label.
//! @param opt Can contain the same control bits as the opt argument
//! for plcolorbar.  However, the only bits that are relevant here are
//! PL_COLORBAR_ORIENT_(RIGHT|TOP|LEFT|BOTTOM), and
//! PL_COLORBAR_LABEL_(RIGHT|TOP|LEFT|BOTTOM).  For full documentation
//! of these bits, see the documentation of opt for plcolorbar.
//! @param label Text label for the color bar.  No label is drawn if no
//! label position is specified with one of the
//! PL_COLORBAR_LABEL_(RIGHT|TOP|LEFT|BOTTOM) bits in opt.

static void
draw_label( PLBOOL if_bb, PLINT opt, PLCHAR_VECTOR label )
{
    // Justification of label text
    PLFLT just = 0.0;

    // How far away from the axis should the label be drawn in units of
    // the character height?
    PLFLT label_offset = 1.2;

    // For building plmtex option string.
#define max_opts    25
    char opt_label[max_opts];
    char perp;

    // To help sanity check number of specified labels.
    PLINT nlabel = 0;

    // aspect ratio of physical area of subpage.
    //PLFLT aspspp = ( ( plsc->sppxma - plsc->sppxmi ) / plsc->xpmm ) /
    //               ( ( plsc->sppyma - plsc->sppymi ) / plsc->ypmm );

    // Character height in y and x normalized subpage coordinates.
    //PLFLT character_height_y = get_character_or_symbol_height( TRUE );
    // character height _in normalized subpage coordinates_ is smaller
    // in the x direction if the subpage aspect ratio is larger than one.
    //PLFLT character_height_x = character_height_y / aspspp;

    // Ratio of normalized subpage coordinates to mm coordinates in
    // x and y.
    //PLFLT spxpmm          = plsc->xpmm / ( plsc->sppxma - plsc->sppxmi );
    //PLFLT spypmm          = plsc->ypmm / ( plsc->sppyma - plsc->sppymi );
    PLFLT label_length_mm = plstrl( label );

    PLFLT parallel_height_mm = 0.0, perpendicular_height_mm = 0.0,
          default_mm, char_height_mm;

    // Only honor first bit in list of
    // PL_COLORBAR_LABEL_(RIGHT|TOP|LEFT|BOTTOM).
    if ( opt & PL_COLORBAR_LABEL_RIGHT )
    {
        nlabel = 1;
    }
    if ( opt & PL_COLORBAR_LABEL_TOP )
    {
        if ( nlabel == 1 )
            opt = opt & ~PL_COLORBAR_LABEL_TOP;
        else
            nlabel = 1;
    }
    if ( opt & PL_COLORBAR_LABEL_LEFT )
    {
        if ( nlabel == 1 )
            opt = opt & ~PL_COLORBAR_LABEL_LEFT;
        else
            nlabel = 1;
    }
    if ( opt & PL_COLORBAR_LABEL_BOTTOM )
    {
        if ( nlabel == 1 )
            opt = opt & ~PL_COLORBAR_LABEL_BOTTOM;
        else
            nlabel = 1;
    }
    // If no label wanted, then return.
    if ( nlabel == 0 )
        return;

    plgchr( &default_mm, &char_height_mm );

    // Start preparing data to help plot the label or
    // calculate the corresponding bounding box changes.

    if ( if_bb )
    {
        // Bounding-box limits are the viewport limits in mm before corrections
        // for decorations are applied.
        plsc->boxbb_xmin = plsc->vppxmi / plsc->xpmm;
        plsc->boxbb_xmax = plsc->vppxma / plsc->xpmm;
        plsc->boxbb_ymin = plsc->vppymi / plsc->ypmm;
        plsc->boxbb_ymax = plsc->vppyma / plsc->ypmm;

        // For labels written parallel to axis, label_offset of zero
        // corresponds to character centred on edge so should add 0.5 to
        // height to obtain bounding box edge in direction away from
        // edge.  However, experimentally found 0.8 gave a better
        // looking result.
        parallel_height_mm = ( label_offset + 0.8 ) * char_height_mm;

        // For labels written perpendicular to axis, label_offset of
        // zero corresponds to a character whose edge just touches the
        // edge of the box so should add 0. to label_offset (corrected
        // by -0.5 below) to obtain bounding box edge in direction away
        // from edge, and that value apparently works.
        perpendicular_height_mm = ( label_offset - 0.5 + 0.0 ) * char_height_mm;
    }
    if ( opt & PL_COLORBAR_LABEL_LEFT )
    {
        if ( opt & PL_COLORBAR_ORIENT_TOP || opt & PL_COLORBAR_ORIENT_BOTTOM )
        {
            perp = '\0';
            just = 0.5;
            if ( if_bb )
            {
                plsc->boxbb_xmin = MIN( plsc->boxbb_xmin, plsc->vppxmi /
                    plsc->xpmm - parallel_height_mm );
                plsc->boxbb_ymin = MIN( plsc->boxbb_ymin,
                    0.5 * ( plsc->vppymi + plsc->vppyma ) /
                    plsc->ypmm - 0.5 * label_length_mm );
                plsc->boxbb_ymax = MAX( plsc->boxbb_ymax,
                    0.5 * ( plsc->vppymi + plsc->vppyma ) /
                    plsc->ypmm + 0.5 * label_length_mm );
            }
        }
        else
        {
            perp          = 'v';
            just          = 1.0;
            label_offset -= 0.5;
            if ( if_bb )
            {
                plsc->boxbb_xmin = MIN( plsc->boxbb_xmin, plsc->vppxmi /
                    plsc->xpmm - perpendicular_height_mm - label_length_mm );
            }
        }
        snprintf( opt_label, max_opts, "l%c", perp );
    }
    else if ( opt & PL_COLORBAR_LABEL_RIGHT )
    {
        if ( opt & PL_COLORBAR_ORIENT_TOP || opt & PL_COLORBAR_ORIENT_BOTTOM )
        {
            perp = '\0';
            just = 0.5;
            if ( if_bb )
            {
                plsc->boxbb_xmax = MAX( plsc->boxbb_xmax, plsc->vppxma /
                    plsc->xpmm + parallel_height_mm );
                plsc->boxbb_ymin = MIN( plsc->boxbb_ymin,
                    0.5 * ( plsc->vppymi + plsc->vppyma ) /
                    plsc->ypmm - 0.5 * label_length_mm );
                plsc->boxbb_ymax = MAX( plsc->boxbb_ymax,
                    0.5 * ( plsc->vppymi + plsc->vppyma ) /
                    plsc->ypmm + 0.5 * label_length_mm );
            }
        }
        else
        {
            perp          = 'v';
            just          = 0.0;
            label_offset -= 0.5;
            if ( if_bb )
            {
                plsc->boxbb_xmax = MAX( plsc->boxbb_xmax, plsc->vppxma /
                    plsc->xpmm + perpendicular_height_mm + label_length_mm );
            }
        }
        snprintf( opt_label, max_opts, "r%c", perp );
    }
    else if ( opt & PL_COLORBAR_LABEL_TOP )
    {
        perp = '\0';
        just = 0.5;
        snprintf( opt_label, max_opts, "t%c", perp );
        if ( if_bb )
        {
            plsc->boxbb_ymax = MAX( plsc->boxbb_ymax, plsc->vppyma /
                plsc->ypmm + parallel_height_mm );
            plsc->boxbb_xmin = MIN( plsc->boxbb_xmin,
                0.5 * ( plsc->vppxmi + plsc->vppxma ) /
                plsc->xpmm - 0.5 * label_length_mm );
            plsc->boxbb_xmax = MAX( plsc->boxbb_xmax,
                0.5 * ( plsc->vppxmi + plsc->vppxma ) /
                plsc->xpmm + 0.5 * label_length_mm );
        }
    }
    else if ( opt & PL_COLORBAR_LABEL_BOTTOM )
    {
        perp = '\0';
        just = 0.5;
        snprintf( opt_label, max_opts, "b%c", perp );
        if ( if_bb )
        {
            plsc->boxbb_ymin = MIN( plsc->boxbb_ymin, plsc->vppymi /
                plsc->ypmm - parallel_height_mm );
            plsc->boxbb_xmin = MIN( plsc->boxbb_xmin,
                0.5 * ( plsc->vppxmi + plsc->vppxma ) /
                plsc->xpmm - 0.5 * label_length_mm );
            plsc->boxbb_xmax = MAX( plsc->boxbb_xmax,
                0.5 * ( plsc->vppxmi + plsc->vppxma ) /
                plsc->xpmm + 0.5 * label_length_mm );
        }
    }
    if ( !if_bb )
        plmtex( opt_label, label_offset, 0.5, just, label );
}

//--------------------------------------------------------------------------
//! Calculate important positional limits for color bar as a function
//! of the current bounding box limits and prior bounding-box height.
//! plvpor(0, prior_bb_width, 0, prior_bb_height) is called before
//! each bounding-box calculation and this calculate_limits call,
//! where prior_bb_width and prior_bb_height are the width and height
//! of the prior bounding box in subpage coordinates.  Thus, an
//! arbitrary but convenient offset is given to old and new bounding
//! box limits because the actual offset is not known until
//! calculate_limits is finished.
//!
// dx_subpage, dy_subpageDifferences between normalized subpage coordinates for the old
// bounding box and the new one.

static void
calculate_limits( PLINT position, PLFLT x, PLFLT y,
                  PLFLT xdmin_adopted, PLFLT xdmax_adopted, PLFLT ydmin_adopted, PLFLT ydmax_adopted,
                  PLFLT prior_bb_height,
                  PLFLT *p_colorbar_width_bb, PLFLT *p_colorbar_height_bb,
                  PLFLT *p_colorbar_width_ac, PLFLT *p_colorbar_height_ac,
                  PLFLT *p_plot_x_subpage_bb, PLFLT *p_plot_y_subpage_bb,
                  PLFLT *p_dx_subpage, PLFLT *p_dy_subpage
                  )
{
    PLFLT x_colorbar_position, y_colorbar_position, xsign, ysign;
    PLFLT plot_x, plot_y;
    // Ratio of normalized subpage coordinates to mm coordinates in
    // x and y.
    PLFLT spxpmm = plsc->xpmm / ( plsc->sppxma - plsc->sppxmi );
    PLFLT spypmm = plsc->ypmm / ( plsc->sppyma - plsc->sppymi );

    // New bounding box width and height in normalized subpage coordinates.
    *p_colorbar_width_bb  = ( plsc->boxbb_xmax - plsc->boxbb_xmin ) * spxpmm;
    *p_colorbar_height_bb = ( plsc->boxbb_ymax - plsc->boxbb_ymin ) * spypmm;

    // Offsets (in sense of prior minus current) of upper left corner of prior bounding box
    // relative to current bounding box in normalized subpage coordinates.  From
    // the above comments, the upper left corner of prior bounding box
    // has the coordinates (0., prior_bb_height).
    *p_dx_subpage = -plsc->boxbb_xmin * spxpmm;
    *p_dy_subpage = prior_bb_height - plsc->boxbb_ymax * spypmm;

    // Total width and height of new bounding box in adopted
    // coordinates.
    *p_colorbar_width_ac = subpage_to_adopted_x( *p_colorbar_width_bb ) -
                           subpage_to_adopted_x( 0. );
    *p_colorbar_height_ac = subpage_to_adopted_y( *p_colorbar_height_bb ) -
                            subpage_to_adopted_y( 0. );

    // Calculate parameters that help determine the position of the top
    // left of the legend in adopted coordinates.  See pllegend
    // documentation for definition of adopted coordinates.
    legend_position( position, *p_colorbar_width_ac, *p_colorbar_height_ac, &x_colorbar_position, &y_colorbar_position, &xsign, &ysign );
    plot_x = x * xsign + x_colorbar_position;
    plot_y = y * ysign + y_colorbar_position;
    // Calculate normalized subpage coordinates of top left of new bounding box.
    *p_plot_x_subpage_bb = adopted_to_subpage_x( plot_x );
    *p_plot_y_subpage_bb = adopted_to_subpage_y( plot_y );
}


//--------------------------------------------------------------------------
//! Plot color bar for image, shade or gradient plots.
//!
//! @param p_colorbar_width Pointer to a location which contains
//! (after the call) the labelled and decorated color bar width in
//! adopted coordinates.
//! @param p_colorbar_height Pointer to a location which contains
//! (after the call) the labelled and decorated color bar height in
//! adopted coordinates.
//! @param opt This variable contains bits which control the overall
//! color bar.  The orientation (direction of the maximum value) of
//! the color bar is specified with PL_COLORBAR_ORIENT_(RIGHT, TOP,
//! LEFT, BOTTOM).  If none of these bits are specified, the default
//! orientation is toward the top if the colorbar is placed on the
//! left or right of the viewport or toward the right if the colorbar
//! is placed on the top or bottom of the viewport.  If the
//! PL_COLORBAR_BACKGROUND bit is set, plot a (semi-transparent)
//! background for the color bar.  If the PL_COLORBAR_BOUNDING_BOX bit
//! is set, plot a bounding box for the color bar.  The type of color
//! bar must be specified with one of PL_COLORBAR_IMAGE,
//! PL_COLORBAR_SHADE, or PL_COLORBAR_GRADIENT.  If more than one of
//! those bits is set only the first one in the above list is honored.
//! The position of the (optional) label/title can be specified with
//! PL_COLORBAR_LABEL_(RIGHT|TOP|LEFT|BOTTOM).  If no label position
//! bit is set then no label will be drawn.  If more than one of this
//! list of bits is specified, only the first one on the list is
//! honored.  End-caps for the color bar can added with
//! PL_COLORBAR_CAP_LOW and PL_COLORBAR_CAP_HIGH.  If a particular
//! color bar cap option is not specified then no cap will be drawn
//! for that end.  As a special case for PL_COLORBAR_SHADE, the option
//! PL_COLORBAR_SHADE_LABEL can be specified.  If this option is
//! provided then any tick marks and tick labels will be placed at the
//! breaks between shaded segments.  TODO: This should be expanded to
//! support custom placement of tick marks and tick labels at custom
//! value locations for any color bar type.
//! @param position This variable contains bits which control the
//! overall position of the color bar and the definition of the
//! adopted coordinates used for positions just like what is done for
//! the position argument for pllegend.  However, note that the
//! defaults for the position bits (see below) are different than the
//! pllegend case.  The combination of the PL_POSITION_LEFT,
//! PL_POSITION_RIGHT, PL_POSITION_TOP, PL_POSITION_BOTTOM,
//! PL_POSITION_INSIDE, and PL_POSITION_OUTSIDE bits specifies one of
//! the 16 possible standard positions (the 4 corners and centers of
//! the 4 sides for both the inside and outside cases) of the color
//! bar relative to the adopted coordinate system.  The corner
//! positions are specified by the appropriate combination of two of
//! the PL_POSITION_LEFT, PL_POSITION_RIGHT, PL_POSITION_TOP, and
//! PL_POSITION_BOTTOM bits while the sides are specified by a single
//! value of one of those bits.  The adopted coordinates are
//! normalized viewport coordinates if the PL_POSITION_VIEWPORT bit is
//! set or normalized subpage coordinates if the PL_POSITION_SUBPAGE
//! bit is set.  Default position bits: If none of PL_POSITION_LEFT,
//! PL_POSITION_RIGHT, PL_POSITION_TOP, or PL_POSITION_BOTTOM are set,
//! then use PL_POSITION_RIGHT.  If neither of PL_POSITION_INSIDE or
//! PL_POSITION_OUTSIDE is set, use PL_POSITION_OUTSIDE.  If neither
//! of PL_POSITION_VIEWPORT or PL_POSITION_SUBPAGE is set, use
//! PL_POSITION_VIEWPORT.
//! @param x X offset of the color bar position in adopted coordinates
//! from the specified standard position of the color bar.  For positive
//! x, the direction of motion away from the standard position is
//! inward/outward from the standard corner positions or standard left
//! or right positions if the PL_POSITION_INSIDE/PL_POSITION_OUTSIDE
//! bit is set in position.  For the standard top or bottom positions,
//! the direction of motion for positive x is toward positive X.
//! @param y Y offset of the color bar position in adopted coordinates
//! from the specified standard position of the color bar.  For positive
//! y, the direction of motion away from the standard position is
//! inward/outward from the standard corner positions or standard top
//! or bottom positions if the PL_POSITION_INSIDE/PL_POSITION_OUTSIDE
//! bit is set in position.  For the standard left or right positions,
//! the direction of motion for positive y is toward positive Y.
//! @param x_length Length of the body of the color bar in the X
//! direction in adopted coordinates.
//! @param y_length Length of the body of the color bar in the Y
//! direction in adopted coordinates.
//! @param bg_color The cmap0 color of the background for the color bar
//! (PL_COLORBAR_BACKGROUND).
//! @param bb_color The cmap0 color of the bounding-box
//! line for the color bar (PL_COLORBAR_BOUNDING_BOX).
//! @param bb_style The pllsty style number for the bounding-box line
//! for the color bar (PL_COLORBAR_BOUNDING_BOX).
//! @param low_cap_color The cmap1 color of the low-end color bar cap, if it is drawn (PL_COLORBAR_CAP_LOW).
//! @param high_cap_color The cmap1 color of the high-end color bar cap, if it is drawn (PL_COLORBAR_CAP_HIGH).
//! @param cont_color The cmap0 contour color for PL_COLORBAR_SHADE
//! plots.  This value is passed directly to plshades, so it will be
//! interpreted according to the design of plshades.
//! @param cont_width Contour width for PL_COLORBAR_SHADE plots.  This is
//! passed directly to plshades, so it will be interpreted according to the
//! design of plshades.
//! @param n_labels Number of labels to place around the color bar.
//! @param label_opts An array of n_labels opt values to help calculate the
//! combined opt value for each of the labels.  For each
//! different label the combined opt value is the bitwise OR of the
//! overall opt value and the relevant label_opts array element.  The
//! only bits that are relevant in a combined opt value for a label
//! are PL_COLORBAR_ORIENT_(RIGHT|TOP|LEFT|BOTTOM) and
//! PL_COLORBAR_LABEL_(RIGHT|TOP|LEFT|BOTTOM) (which are documented in
//! the documentation of the overall opt value).
//! @param labels n_labels text labels for the color bar. No label is
//! drawn if no label position is specified with one of the
//! PL_COLORBAR_LABEL_(RIGHT|TOP|LEFT|BOTTOM) bits in the bitwise OR
//! of the opt value with the relevant label_opts array element.
//! @param n_axes Number of axis definitions provided.  This value
//! must be greater than 0.  It is typically 1 (numerical axis labels
//! are provided for one of the long edges of the color bar), but it
//! can be larger if multiple numerical axis labels for the long edges
//! of the color bar are desired.
//! @param axis_opts An array of n_axes axis options (interpreted as for
//! plbox) for the color bar's axis definitions.
//! @param ticks An array of n_axes values of the spacing of the major
//! tick marks (interpreted as for plbox) for the color bar's axis
//! definitions.
//! @param sub_ticks An array of n_axes values of the number of
//! subticks (interpreted as for plbox) for the color bar's axis
//! definitions.
//! @param n_values An array containing the number of elements in each of the
//! n_axes rows of the two-dimensional values array.
//! @param values A two-dimensional array containing the numeric
//! values for the data range represented by the color bar.  For a row
//! index of i_axis (where 0 < i_axis < n_axes), the number of
//! elements in the row is specified by n_values[i_axis].  For
//! PL_COLORBAR_IMAGE and PL_COLORBAR_GRADIENT the number of elements
//! is 2, and the corresponding row elements of the values array are
//! the minimum and maximum value represented by the colorbar.  For
//! PL_COLORBAR_SHADE, the number and values of the elements of a
//! row of the values array is interpreted the same
//! as the nlevel and clevel arguments of plshades.
//!

void
c_plcolorbar( PLFLT *p_colorbar_width, PLFLT *p_colorbar_height,
              PLINT opt, PLINT position, PLFLT x, PLFLT y,
              PLFLT x_length, PLFLT y_length,
              PLINT bg_color, PLINT bb_color, PLINT bb_style,
              PLFLT low_cap_color, PLFLT high_cap_color,
              PLINT cont_color, PLFLT cont_width,
              PLINT n_labels, PLINT_VECTOR label_opts, PLCHAR_MATRIX labels,
              PLINT n_axes, PLCHAR_MATRIX axis_opts,
              PLFLT_VECTOR ticks, PLINT_VECTOR sub_ticks,
              PLINT_VECTOR n_values, PLFLT_MATRIX values )
{
    // Min and max values
    // Assumes that the values array is sorted from smallest to largest
    // OR from largest to smallest.
    // Unnecessarily initialize min_value, max_value, and max_abs
    // to quiet -O1 -Wuninitialized warnings that are false alarms.
    // (n_axes is guaranteed to be 1 or greater so that the loops that
    // define these values must be executed at least once.)
    PLFLT min_value = 0., max_value = 0., max_abs = 0.;
    // Length of cap in orientation direction in normalized subpage
    // coordinates and mm.
    PLFLT cap_extent, cap_extent_mm;

    // The color bar cap is an equilateral triangle with cap_angle
    // the angle (in degrees) of the unequal angle pointing in the
    // direction of the orientation of the cap.  In other words,
    // cap_angle completely controls the shape of the triangle, but
    // not its scale.
    PLFLT cap_angle = 45.;
    // Ratio of length of cap in orientation direction
    // to the width of the bar (and cap) in the direction
    // perpendicular to the orientation in physical coordinates
    // (i.e., independent of aspect ratio).
    PLFLT cap_ratio = 0.5 / tan( PI / 360. * cap_angle );

    // aspect ratio of physical area of subpage.
    PLFLT aspspp = ( ( plsc->sppxma - plsc->sppxmi ) / plsc->xpmm ) /
                   ( ( plsc->sppyma - plsc->sppymi ) / plsc->ypmm );

    // Min and max colors
    PLFLT min_color, max_color;

    // Saved external world coordinates of viewport.
    PLFLT xwmin_save, xwmax_save, ywmin_save, ywmax_save;
    // Saved external normalized coordinates of viewport.
    // (These are actual values used only for the restore.)
    PLFLT xdmin_save = 0.0, xdmax_save = 0.0, ydmin_save = 0.0, ydmax_save = 0.0;

    // Limits of adopted coordinates used to calculate all coordinate
    // transformations.
    PLFLT xdmin_adopted = 0.0, xdmax_adopted = 0.0, ydmin_adopted = 0.0, ydmax_adopted = 0.0;

    // Active attributes to be saved and restored afterward.
    PLINT col0_save       = plsc->icol0,
          line_style_save = plsc->line_style;

    // Normalized subpage coordinates of top left of the bounding box.
    PLFLT plot_x_subpage_bb, plot_y_subpage_bb;

    // color bar width and height in normalized subpage coordinates.
    // No suffix refers to bonding box of undecorated color bar, d
    // suffix refers to bounding box of decorated color bar, and l
    // suffix refers to bounding box of labelled and decorated
    // color bar.
    PLFLT colorbar_width, colorbar_height,
          colorbar_width_d, colorbar_height_d,
          colorbar_width_l, colorbar_height_l;

    // ac suffix refers to latest color bar_width (d or l suffix) converted to
    // adopted coordinates.
    // mm suffix refers to colorbar_width and colorbar_height (with no suffix)
    // converted from normalized subpage coordinates to mm.
    PLFLT colorbar_width_ac, colorbar_height_ac,
          colorbar_width_mm, colorbar_height_mm;

    // Change in normalized subpage coordinates of the top left of
    // undecorated color bar.  (The omd suffix refers to original
    // color bar minus decorated color bar, and the dml suffix refers to
    // decorated color bar minus labelled and decorated color bar.)
    PLFLT dx_subpage_omd, dy_subpage_omd, dx_subpage_dml, dy_subpage_dml;
    PLFLT dx_subpage_omd_accu = 0.0, dy_subpage_omd_accu = 0.0, dx_subpage_dml_accu = 0.0, dy_subpage_dml_accu = 0.0;
    // Normalized subpage coordinates of the top left of undecorated
    // color bar,
    PLFLT plot_x_subpage, plot_y_subpage;

    // Position of the undecorated color bar in normalized subpage coordinates.
    PLFLT vx_min = 0.0, vx_max = 0.0, vy_min = 0.0, vy_max = 0.0;

    // World coordinate limits describing undecorated color bar.
    PLFLT wx_min = 0.0, wx_max = 0.0, wy_min = 0.0, wy_max = 0.0;

    // The data to plot
    PLFLT **color_data;

    // Setting up the data for display
    PLINT  i, j, ni = 0, nj = 0, n_steps;
    PLFLT  step_size;

    PLBOOL if_edge   = 0;
    PLBOOL if_edge_b = 0, if_edge_c = 0, if_edge_u = 0, if_edge_w = 0;

    // Ratio of normalized subpage coordinates to mm coordinates in
    // x and y.
    PLFLT spxpmm = plsc->xpmm / ( plsc->sppxma - plsc->sppxmi );
    PLFLT spypmm = plsc->ypmm / ( plsc->sppyma - plsc->sppymi );

    // plvpor limits for label.
    PLFLT label_vpor_xmin, label_vpor_xmax, label_vpor_ymin, label_vpor_ymax;

    // Default position flags and sanity checks for position flags.
    if ( !( position & PL_POSITION_RIGHT ) && !( position & PL_POSITION_LEFT ) && !( position & PL_POSITION_TOP ) && !( position & PL_POSITION_BOTTOM ) )
    {
        position = position | PL_POSITION_RIGHT;
    }
    else if ( ( position & PL_POSITION_RIGHT ) && ( position & PL_POSITION_LEFT ) )
    {
        plabort( "plcolorbar: PL_POSITION_RIGHT and PL_POSITION_LEFT cannot be simultaneously set." );
        return;
    }

    else if ( ( position & PL_POSITION_TOP ) && ( position & PL_POSITION_BOTTOM ) )
    {
        plabort( "plcolorbar: PL_POSITION_TOP and PL_POSITION_BOTTOM cannot be simultaneously set." );
        return;
    }

    if ( !( position & PL_POSITION_INSIDE ) && !( position & PL_POSITION_OUTSIDE ) )
    {
        position = position | PL_POSITION_OUTSIDE;
    }
    else if ( ( position & PL_POSITION_INSIDE ) && ( position & PL_POSITION_OUTSIDE ) )
    {
        plabort( "plcolorbar: PL_POSITION_INSIDE and PL_POSITION_OUTSIDE cannot be simultaneously set." );
        return;
    }

    if ( !( position & PL_POSITION_VIEWPORT ) && !( position & PL_POSITION_SUBPAGE ) )
    {
        position = position | PL_POSITION_VIEWPORT;
    }
    else if ( ( position & PL_POSITION_VIEWPORT ) && ( position & PL_POSITION_SUBPAGE ) )
    {
        plabort( "plcolorbar: PL_POSITION_VIEWPORT and PL_POSITION_SUBPAGE cannot be simultaneously set." );
        return;
    }

    // Sanity check for NULL label arrays.
    if ( n_labels > 0 && ( label_opts == NULL || labels == NULL ) )
    {
        plabort( "plcolorbar: label_opts and labels arrays must be defined when n_labels > 0." );
        return;
    }

    if ( n_axes < 1 )
    {
        plabort( "plcolorbar: At least one axis must be specified" );
        return;
    }

    // xdmin_save, etc., are the actual external relative viewport
    // coordinates within the current sub-page used only for
    // restoration at the end.
    plgvpsp( &xdmin_save, &xdmax_save, &ydmin_save, &ydmax_save );

    // Choose adopted coordinates.
    if ( position & PL_POSITION_SUBPAGE )
        plvpor( 0., 1., 0., 1. );

    // xdmin_adopted, etc., are the adopted limits of the coordinates
    // within the current sub-page used for all coordinate
    // transformations.
    // If position & PL_POSITION_VIEWPORT is true, these limits
    // are the external relative viewport limits.
    // If position & PL_POSITION_SUBPAGE is true, these
    // coordinates are the relative subpage coordinates.
    plgvpsp( &xdmin_adopted, &xdmax_adopted, &ydmin_adopted, &ydmax_adopted );

    // xwmin_save, etc., are the external world coordinates corresponding
    // to the external viewport boundaries.
    plgvpw( &xwmin_save, &xwmax_save, &ywmin_save, &ywmax_save );

    // Default orientation.
    if ( !( opt & PL_COLORBAR_ORIENT_RIGHT ||
            opt & PL_COLORBAR_ORIENT_TOP ||
            opt & PL_COLORBAR_ORIENT_LEFT ||
            opt & PL_COLORBAR_ORIENT_BOTTOM ) )
    {
        if ( position & PL_POSITION_LEFT || position & PL_POSITION_RIGHT )
            opt = opt | PL_COLORBAR_ORIENT_TOP;
        else
            opt = opt | PL_COLORBAR_ORIENT_RIGHT;
    }

    for ( i = 0; i < n_axes; i++ )
    {
        if_edge_b = if_edge_b || plP_stsearch( axis_opts[i], 'b' );
        if_edge_c = if_edge_c || plP_stsearch( axis_opts[i], 'c' );
        if_edge_u = if_edge_u || plP_stsearch( axis_opts[i], 'u' );
        if_edge_w = if_edge_w || plP_stsearch( axis_opts[i], 'w' );
    }
    if_edge = if_edge_b && if_edge_c && !( if_edge_u || if_edge_w );

    // Assumes that the colors array is sorted from smallest to largest.
    plgcmap1_range( &min_color, &max_color );

    // Width and height of the undecorated color bar in normalized
    // subpage coordinates and mm.
    colorbar_width = adopted_to_subpage_x( x_length ) -
                     adopted_to_subpage_x( 0. );
    colorbar_height = adopted_to_subpage_y( y_length ) -
                      adopted_to_subpage_y( 0. );
    colorbar_width_d   = colorbar_width;
    colorbar_height_d  = colorbar_height;
    colorbar_width_mm  = colorbar_width / spxpmm;
    colorbar_height_mm = colorbar_height / spypmm;
    // Extent of cap in normalized subpage coordinates in either X or Y
    // direction as appropriate in normalized subpage coordinates and  mm.
    if ( opt & PL_COLORBAR_ORIENT_RIGHT || opt & PL_COLORBAR_ORIENT_LEFT )
    {
        cap_extent    = cap_ratio * colorbar_height / aspspp;
        cap_extent_mm = cap_extent / spxpmm;
    }
    else
    {
        cap_extent    = cap_ratio * colorbar_width * aspspp;
        cap_extent_mm = cap_extent / spypmm;
    }

    for ( i = n_axes - 1; i >= 0; i-- )
    {
        min_value = values[i][0];
        max_value = values[i][ n_values[i] - 1 ];
        max_abs   = MAX( fabs( min_value ), fabs( max_value ) );

        // Specify the proper window ranges for color bar depending on
        // orientation.
        if ( opt & PL_COLORBAR_ORIENT_RIGHT )
        {
            wx_min = min_value;
            wx_max = max_value;
            wy_min = 0.0;
            wy_max = max_abs;
        }
        else if ( opt & PL_COLORBAR_ORIENT_TOP )
        {
            wx_min = 0.0;
            wx_max = max_abs;
            wy_min = min_value;
            wy_max = max_value;
        }
        else if ( opt & PL_COLORBAR_ORIENT_LEFT )
        {
            wx_min = max_value;
            wx_max = min_value;
            wy_min = 0.0;
            wy_max = max_abs;
        }
        else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
        {
            wx_min = 0.0;
            wx_max = max_abs;
            wy_min = max_value;
            wy_max = min_value;
        }
        else
        {
            plabort( "plcolorbar: Invalid PL_COLORBAR_ORIENT_* bits" );
        }

        // Viewport has correct size but has a shifted zero-point
        // convention required by bounding-box calculations in draw_box,
        // further bounding-box calculations in the cap_extent section
        // below, and also in the calculate_limits call below that.
        plvpor( 0., colorbar_width_d, 0., colorbar_height_d );
        plwind( wx_min, wx_max, wy_min, wy_max );

        // Calculate the bounding box for decorated (i.e., including tick
        // marks + numerical tick labels) box.
        draw_box( TRUE, opt, axis_opts[i], if_edge,
            ticks[i], sub_ticks[i], n_values[i], values[i] );

        if ( i == n_axes - 1 )
        {
            if ( opt & PL_COLORBAR_CAP_LOW )
            {
                if ( opt & PL_COLORBAR_ORIENT_RIGHT )
                    plsc->boxbb_xmin = MIN( plsc->boxbb_xmin, -cap_extent_mm );
                if ( opt & PL_COLORBAR_ORIENT_TOP )
                    plsc->boxbb_ymin = MIN( plsc->boxbb_ymin, -cap_extent_mm );
                if ( opt & PL_COLORBAR_ORIENT_LEFT )
                    plsc->boxbb_xmax = MAX( plsc->boxbb_xmax, colorbar_width_mm + cap_extent_mm );
                if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
                    plsc->boxbb_ymax = MAX( plsc->boxbb_ymax, colorbar_height_mm + cap_extent_mm );
            }
            if ( opt & PL_COLORBAR_CAP_HIGH )
            {
                if ( opt & PL_COLORBAR_ORIENT_RIGHT )
                    plsc->boxbb_xmax = MAX( plsc->boxbb_xmax, colorbar_width_mm + cap_extent_mm );
                if ( opt & PL_COLORBAR_ORIENT_TOP )
                    plsc->boxbb_ymax = MAX( plsc->boxbb_ymax, colorbar_height_mm + cap_extent_mm );
                if ( opt & PL_COLORBAR_ORIENT_LEFT )
                    plsc->boxbb_xmin = MIN( plsc->boxbb_xmin, -cap_extent_mm );
                if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
                    plsc->boxbb_ymin = MIN( plsc->boxbb_ymin, -cap_extent_mm );
            }
        }

        // Calculate limits relevant to label position.
        calculate_limits( position, x, y,
            xdmin_adopted, xdmax_adopted, ydmin_adopted, ydmax_adopted,
            colorbar_height_d,
            &colorbar_width_d, &colorbar_height_d,
            &colorbar_width_ac, &colorbar_height_ac,
            &plot_x_subpage_bb, &plot_y_subpage_bb,
            &dx_subpage_omd, &dy_subpage_omd );

        // Viewport has correct size but has a shifted zero point
        // convention required by bounding-box calculations in draw_label
        // and further calculations in calculate_limits.
        plvpor( 0., colorbar_width_d, 0., colorbar_height_d );

        dx_subpage_omd_accu += dx_subpage_omd;
        dy_subpage_omd_accu += dy_subpage_omd;
    }

    // Capture the current bounding box dimensions
    colorbar_width_l  = colorbar_width_d;
    colorbar_height_l = colorbar_height_d;

    for ( i = 0; i < n_labels; i++ )
    {
        // Viewport has correct size but has a shifted zero-point
        // convention required by bounding-box calculations in draw_box,
        // further bounding-box calculations in the cap_extent section
        // below, and also in the calculate_limits call below that.
        plvpor( 0., colorbar_width_l, 0., colorbar_height_l );
        plwind( wx_min, wx_max, wy_min, wy_max );

        // Calculate the bounding box for combined label + decorated box.
        draw_label( TRUE, opt | label_opts[i], labels[i] );

        // Calculate overall limits.
        calculate_limits( position, x, y,
            xdmin_adopted, xdmax_adopted, ydmin_adopted, ydmax_adopted,
            colorbar_height_l,
            &colorbar_width_l, &colorbar_height_l,
            &colorbar_width_ac, &colorbar_height_ac,
            &plot_x_subpage_bb, &plot_y_subpage_bb,
            &dx_subpage_dml, &dy_subpage_dml );

        dx_subpage_dml_accu += dx_subpage_dml;
        dy_subpage_dml_accu += dy_subpage_dml;
    }

    // Normalized subpage coordinates (top-left corner) for undecorated
    // color bar
    plot_x_subpage = plot_x_subpage_bb + dx_subpage_omd_accu + dx_subpage_dml_accu;
    plot_y_subpage = plot_y_subpage_bb + dy_subpage_omd_accu + dy_subpage_dml_accu;

    // Coordinates of bounding box for decorated color bar (without overall label).
    label_vpor_xmin = plot_x_subpage_bb + dx_subpage_dml_accu;
    label_vpor_xmax = label_vpor_xmin + colorbar_width_d;
    label_vpor_ymax = plot_y_subpage_bb + dy_subpage_dml_accu;
    label_vpor_ymin = label_vpor_ymax - colorbar_height_d;

    // Return bounding box width and height in adopted coordinates for
    // labelled and decorated color bar.
    *p_colorbar_width  = colorbar_width_ac;
    *p_colorbar_height = colorbar_height_ac;

    // Specify the proper viewport ranges for color bar depending on
    // orientation.
    if ( opt & PL_COLORBAR_ORIENT_RIGHT )
    {
        vx_min = plot_x_subpage;
        vx_max = plot_x_subpage + colorbar_width;
        vy_min = plot_y_subpage - colorbar_height;
        vy_max = plot_y_subpage;
    }
    else if ( opt & PL_COLORBAR_ORIENT_TOP )
    {
        vx_min = plot_x_subpage;
        vx_max = plot_x_subpage + colorbar_width;
        vy_min = plot_y_subpage - colorbar_height;
        vy_max = plot_y_subpage;
    }
    else if ( opt & PL_COLORBAR_ORIENT_LEFT )
    {
        vx_min = plot_x_subpage;
        vx_max = plot_x_subpage + colorbar_width;
        vy_min = plot_y_subpage - colorbar_height;
        vy_max = plot_y_subpage;
    }
    else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
    {
        vx_min = plot_x_subpage;
        vx_max = plot_x_subpage + colorbar_width;
        vy_min = plot_y_subpage - colorbar_height;
        vy_max = plot_y_subpage;
    }
    else
    {
        plabort( "plcolorbar: Invalid PL_COLORBAR_ORIENT_* bits" );
    }

    // Viewport and world coordinate ranges for bounding-box.
    plvpor( 0., 1., 0., 1. );
    plwind( 0., 1., 0., 1. );

    if ( opt & PL_COLORBAR_BACKGROUND )
    {
        PLFLT xbg[4] = {
            plot_x_subpage_bb,
            plot_x_subpage_bb,
            plot_x_subpage_bb + colorbar_width_l,
            plot_x_subpage_bb + colorbar_width_l,
        };
        PLFLT ybg[4] = {
            plot_y_subpage_bb,
            plot_y_subpage_bb - colorbar_height_l,
            plot_y_subpage_bb - colorbar_height_l,
            plot_y_subpage_bb,
        };
        plpsty( 0 );
        plcol0( bg_color );
        plfill( 4, xbg, ybg );
        plcol0( col0_save );
    }

    // Viewport and world coordinate ranges for color bar.
    plvpor( vx_min, vx_max, vy_min, vy_max );
    plwind( wx_min, wx_max, wy_min, wy_max );

    // What kind of color bar are we making?
    if ( opt & PL_COLORBAR_IMAGE )
    {
        // Interpolate
        // TODO: Should this be decided with an extra opt option instead of by
        // counting n_values?
        if ( n_values[0] == 2 )
        {
            // Use the same number of steps as there are steps in
            // color palette 1.
            // TODO: Determine a better way to specify the steps here?
            n_steps   = plsc->ncol1;
            step_size = ( max_value - min_value ) / (PLFLT) n_steps;
            if ( opt & PL_COLORBAR_ORIENT_RIGHT )
            {
                ni = n_steps;
                nj = 2;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = min_value + (PLFLT) i * step_size;
                    }
                }
            }
            else if ( opt & PL_COLORBAR_ORIENT_TOP )
            {
                ni = 2;
                nj = n_steps;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = min_value + (PLFLT) j * step_size;
                    }
                }
            }
            else if ( opt & PL_COLORBAR_ORIENT_LEFT )
            {
                ni = n_steps;
                nj = 2;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = max_value - (PLFLT) i * step_size;
                    }
                }
            }
            else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
            {
                ni = 2;
                nj = n_steps;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = max_value - (PLFLT) j * step_size;
                    }
                }
            }
            else
            {
                plabort( "plcolorbar: Invalid orientation bits" );
            }
        }
        // No interpolation - use values array as-is
        else
        {
            n_steps = n_values[0];
            // Use the provided values in this case.
            if ( opt & PL_COLORBAR_ORIENT_RIGHT )
            {
                ni = n_steps;
                nj = 2;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = values[0][i];
                    }
                }
            }
            else if ( opt & PL_COLORBAR_ORIENT_TOP )
            {
                ni = 2;
                nj = n_steps;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = values[0][j];
                    }
                }
            }
            else if ( opt & PL_COLORBAR_ORIENT_LEFT )
            {
                ni = n_steps;
                nj = 2;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = values[0][ni - 1 - i];
                    }
                }
            }
            else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
            {
                ni = 2;
                nj = n_steps;
                plAlloc2dGrid( &color_data, ni, nj );
                for ( i = 0; i < ni; i++ )
                {
                    for ( j = 0; j < nj; j++ )
                    {
                        color_data[i][j] = values[0][nj - 1 - j];
                    }
                }
            }
            else
            {
                plabort( "plcolorbar: Invalid side" );
            }
        }
        // Draw the color bar
        plimage( (PLFLT_MATRIX) color_data, ni, nj, wx_min, wx_max, wy_min, wy_max,
            min_value, max_value, wx_min, wx_max, wy_min, wy_max );
        plFree2dGrid( color_data, ni, nj );
    }
    else if ( opt & PL_COLORBAR_SHADE )
    {
        // Transform grid
        // The transform grid is used to make the size of the shaded segments
        // scale relative to other segments.  For example, if segment A
        // makes up 10% of the scale and segment B makes up 20% of the scale
        // then segment B will be twice the length of segment A.
        PLcGrid grid;
        PLFLT   grid_axis[2] = { 0.0, max_abs };
        n_steps = n_values[0];
        // Use the provided values.
        if ( opt & PL_COLORBAR_ORIENT_RIGHT )
        {
            grid.xg = (PLFLT *) values[0];
            grid.yg = grid_axis;
            grid.nx = n_steps;
            grid.ny = 2;
            ni      = n_steps;
            nj      = 2;
            plAlloc2dGrid( &color_data, ni, nj );
            for ( i = 0; i < ni; i++ )
            {
                for ( j = 0; j < nj; j++ )
                {
                    color_data[i][j] = values[0][i];
                }
            }
        }
        else if ( opt & PL_COLORBAR_ORIENT_TOP )
        {
            grid.xg = grid_axis;
            grid.yg = (PLFLT *) values[0];
            grid.nx = 2;
            grid.ny = n_steps;
            ni      = 2;
            nj      = n_steps;
            plAlloc2dGrid( &color_data, ni, nj );
            for ( i = 0; i < ni; i++ )
            {
                for ( j = 0; j < nj; j++ )
                {
                    color_data[i][j] = values[0][j];
                }
            }
        }
        else if ( opt & PL_COLORBAR_ORIENT_LEFT )
        {
            grid.xg = (PLFLT *) values[0];
            grid.yg = grid_axis;
            grid.nx = n_steps;
            grid.ny = 2;
            ni      = n_steps;
            nj      = 2;
            plAlloc2dGrid( &color_data, ni, nj );
            for ( i = 0; i < ni; i++ )
            {
                for ( j = 0; j < nj; j++ )
                {
                    color_data[i][j] = values[0][ni - 1 - i];
                }
            }
        }
        else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
        {
            grid.xg = grid_axis;
            grid.yg = (PLFLT *) values[0];
            grid.nx = 2;
            grid.ny = n_steps;
            ni      = 2;
            nj      = n_steps;
            plAlloc2dGrid( &color_data, ni, nj );
            for ( i = 0; i < ni; i++ )
            {
                for ( j = 0; j < nj; j++ )
                {
                    color_data[i][j] = values[0][nj - 1 - j];
                }
            }
        }
        else
        {
            plabort( "plcolorbar: Invalid orientation" );
        }

        // Draw the color bar
        plshades( (PLFLT_MATRIX) color_data, ni, nj, NULL, wx_min, wx_max, wy_min, wy_max,
            values[0], n_steps, 0, cont_color, cont_width, plfill, TRUE,
            pltr1, (void *) ( &grid ) );
        plFree2dGrid( color_data, ni, nj );
    }
    else if ( opt & PL_COLORBAR_GRADIENT )
    {
        PLFLT xs[4], ys[4];
        PLFLT angle = 0.0;
        xs[0] = wx_min;
        ys[0] = wy_min;
        xs[1] = wx_max;
        ys[1] = wy_min;
        xs[2] = wx_max;
        ys[2] = wy_max;
        xs[3] = wx_min;
        ys[3] = wy_max;
        // Make sure the gradient runs in the proper direction
        if ( opt & PL_COLORBAR_ORIENT_RIGHT )
        {
            angle = 0.0;
        }
        else if ( opt & PL_COLORBAR_ORIENT_TOP )
        {
            angle = 90.0;
        }
        else if ( opt & PL_COLORBAR_ORIENT_LEFT )
        {
            angle = 180.0;
        }
        else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
        {
            angle = 270.0;
        }
        else
        {
            plabort( "plcolorbar: Invalid orientation" );
        }
        plgradient( 4, xs, ys, angle );
    }
    else
    {
        plabort( "plcolorbar: One of PL_COLORBAR_IMAGE, PL_COLORBAR_SHADE, or PL_COLORBAR_GRADIENT bits must be set in opt" );
    }

    // Restore the previous drawing color to use for outlines and text
    plcol0( col0_save );

    // Draw end-caps

    // Viewport and world coordinate ranges for cap.
    plvpor( 0., 1., 0., 1. );
    plwind( 0., 1., 0., 1. );

    if ( opt & PL_COLORBAR_CAP_LOW )
    {
        // Draw a filled triangle (cap/arrow) at the low end of the scale
        if ( opt & PL_COLORBAR_ORIENT_RIGHT )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_LEFT,
                plot_x_subpage - cap_extent, plot_x_subpage,
                plot_y_subpage - colorbar_height, plot_y_subpage,
                low_cap_color );
        else if ( opt & PL_COLORBAR_ORIENT_TOP )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_BOTTOM,
                plot_x_subpage, plot_x_subpage + colorbar_width,
                plot_y_subpage - colorbar_height - cap_extent, plot_y_subpage - colorbar_height,
                low_cap_color );
        else if ( opt & PL_COLORBAR_ORIENT_LEFT )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_RIGHT,
                plot_x_subpage + colorbar_width, plot_x_subpage + colorbar_width + cap_extent,
                plot_y_subpage - colorbar_height, plot_y_subpage,
                low_cap_color );
        else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_TOP,
                plot_x_subpage, plot_x_subpage + colorbar_width,
                plot_y_subpage, plot_y_subpage + cap_extent,
                low_cap_color );
    }
    if ( opt & PL_COLORBAR_CAP_HIGH )
    {
        // Draw a filled triangle (cap/arrow) at the high end of the scale
        if ( opt & PL_COLORBAR_ORIENT_RIGHT )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_RIGHT,
                plot_x_subpage + colorbar_width, plot_x_subpage + colorbar_width + cap_extent,
                plot_y_subpage - colorbar_height, plot_y_subpage,
                high_cap_color );
        else if ( opt & PL_COLORBAR_ORIENT_TOP )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_TOP,
                plot_x_subpage, plot_x_subpage + colorbar_width,
                plot_y_subpage, plot_y_subpage + cap_extent,
                high_cap_color );
        else if ( opt & PL_COLORBAR_ORIENT_LEFT )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_LEFT,
                plot_x_subpage - cap_extent, plot_x_subpage,
                plot_y_subpage - colorbar_height, plot_y_subpage,
                high_cap_color );
        else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
            draw_cap( if_edge, PL_COLORBAR_ORIENT_BOTTOM,
                plot_x_subpage, plot_x_subpage + colorbar_width,
                plot_y_subpage - colorbar_height - cap_extent, plot_y_subpage - colorbar_height,
                high_cap_color );
    }

    for ( i = n_axes - 1; i >= 0; i-- )
    {
        min_value = values[i][0];
        max_value = values[i][ n_values[i] - 1 ];
        max_abs   = MAX( fabs( min_value ), fabs( max_value ) );

        // Specify the proper window ranges for color bar depending on
        // orientation.
        if ( opt & PL_COLORBAR_ORIENT_RIGHT )
        {
            wx_min = min_value;
            wx_max = max_value;
            wy_min = 0.0;
            wy_max = max_abs;
        }
        else if ( opt & PL_COLORBAR_ORIENT_TOP )
        {
            wx_min = 0.0;
            wx_max = max_abs;
            wy_min = min_value;
            wy_max = max_value;
        }
        else if ( opt & PL_COLORBAR_ORIENT_LEFT )
        {
            wx_min = max_value;
            wx_max = min_value;
            wy_min = 0.0;
            wy_max = max_abs;
        }
        else if ( opt & PL_COLORBAR_ORIENT_BOTTOM )
        {
            wx_min = 0.0;
            wx_max = max_abs;
            wy_min = max_value;
            wy_max = min_value;
        }
        else
        {
            plabort( "plcolorbar: Invalid PL_COLORBAR_ORIENT_* bits" );
        }

        // Viewport and world coordinate ranges for box.
        plvpor( vx_min, vx_max, vy_min, vy_max );
        plwind( wx_min, wx_max, wy_min, wy_max );

        // draw decorated (i.e., including tick marks + numerical tick
        // labels) box.
        draw_box( FALSE, opt, axis_opts[i], if_edge,
            ticks[i], sub_ticks[i], n_values[i], values[i] );
    }

    // Viewport and world coordinate ranges for bounding-box.
    plvpor( 0., 1., 0., 1. );
    plwind( 0., 1., 0., 1. );

    if ( opt & PL_COLORBAR_BOUNDING_BOX )
    {
        PLFLT xbb[5] = {
            plot_x_subpage_bb,
            plot_x_subpage_bb,
            plot_x_subpage_bb + colorbar_width_l,
            plot_x_subpage_bb + colorbar_width_l,
            plot_x_subpage_bb,
        };
        PLFLT ybb[5] = {
            plot_y_subpage_bb,
            plot_y_subpage_bb - colorbar_height_l,
            plot_y_subpage_bb - colorbar_height_l,
            plot_y_subpage_bb,
            plot_y_subpage_bb,
        };
        pllsty( bb_style );
        plcol0( bb_color );
        plline( 5, xbb, ybb );
        plcol0( col0_save );
        pllsty( line_style_save );
    }

    // Write label.
    // Viewport coordinate ranges for label.
    plvpor( label_vpor_xmin, label_vpor_xmax, label_vpor_ymin, label_vpor_ymax );
    for ( i = 0; i < n_labels; i++ )
    {
        draw_label( FALSE, opt | label_opts[i], labels[i] );
    }

    // Restore previous plot characteristics.
    plcol0( col0_save );
    plvpor( xdmin_save, xdmax_save, ydmin_save, ydmax_save );
    plwind( xwmin_save, xwmax_save, ywmin_save, ywmax_save );

    return;
}
