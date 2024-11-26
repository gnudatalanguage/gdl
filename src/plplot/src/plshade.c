//      Functions to shade regions on the basis of value.
//      Can be used to shade contour plots or alone.
//      Copyright 1993 Wesley Ebisuzaki
//
// Copyright (C) 2004-2014 Alan W. Irwin
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

//--------------------------------------------------------------------------
// Call syntax for plshade():
//
// void plshade(PLFLT *a, PLINT nx, PLINT ny, char *defined,
//	PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
//      PLFLT shade_min, PLFLT shade_max,
//      PLINT sh_color, PLFLT sh_width, PLINT min_color, PLFLT min_width,
//      PLINT max_color, PLFLT max_width, void (*fill)(), PLINT
//      rectangular, ...)
//
// arguments:
//
//      PLFLT &(a[0][0])
//
// Contains array to be plotted. The array must have been declared as
// PLFLT a[nx][ny].  See following note on fortran-style arrays.
//
//      PLINT nx, ny
//
// Dimension of array "a".
//
//      char &(defined[0][0])
//
// Contains array of flags, 1 = data is valid, 0 = data is not valid.
// This array determines which sections of the data is to be plotted.
// This argument can be NULL if all the values are valid.  Must have been
// declared as char defined[nx][ny].
//
//      PLFLT xmin, xmax, ymin, ymax
//
// Defines the "grid" coordinates.  The data a[0][0] has a position of
// (xmin,ymin).
//
//      void (*mapform)()
//
// Transformation from `grid' coordinates to world coordinates.  This
// pointer to a function can be NULL in which case the grid coordinates
// are the same as the world coordinates.
//
//      PLFLT shade_min, shade_max
//
// Defines the interval to be shaded. If shade_max <= shade_min, plshade
// does nothing.
//
//	PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width
//
// Defines color map, color map index, and width used by the fill pattern.
//
//      PLINT min_color, PLFLT min_width, PLINT max_color, PLFLT max_width
//
// Defines pen color, width used by the boundary of shaded region. The min
// values are used for the shade_min boundary, and the max values are used
// on the shade_max boundary.  Set color and width to zero for no plotted
// boundaries.
//
//      void (*fill)()
//
// Routine used to fill the region.  Use plfill.  Future version of plplot
// may have other fill routines.
//
//      PLINT rectangular
//
// Flag. Set to 1 if rectangles map to rectangles after (*mapform)() else
// set to zero. If rectangular is set to 1, plshade tries to save time by
// filling large rectangles.  This optimization fails if (*mapform)()
// distorts the shape of rectangles.  For example a plot in polor
// coordinates has to have rectangular set to zero.
//
// Example mapform's:
//
// Grid to world coordinate transformation.
// This example goes from a r-theta to x-y for a polar plot.
//
// void mapform(PLINT n, PLFLT *x, PLFLT *y) {
//      int i;
//      double r, theta;
//      for (i = 0; i < n; i++) {
//          r = x[i];
//          theta = y[i];
//          x[i] = r*cos(theta);
//          y[i] = r*sin(theta);
//      }
// }
//
// Grid was in cm, convert to world coordinates in inches.
// Expands in x direction.
//
// void mapform(PLINT n, PLFLT *x, PLFLT *y) {
//      int i;
//      for (i = 0; i < n; i++) {
//              x[i] = (1.0 / 2.5) * x[i];
//              y[i] = (1.0 / 2.5) * y[i];
//      }
// }
//
//--------------------------------------------------------------------------

#include "plplotP.h"
#include <float.h>

#define NEG                  1
#define POS                  8
#define OK                   0
#define UNDEF                64
#define NUMBER_BISECTIONS    10

#define linear( val1, val2, level )    ( ( level - val1 ) / ( val2 - val1 ) )

// Global variables

static PLFLT sh_max, sh_min;
static int   min_points, max_points, n_point;
static int   min_pts[4], max_pts[4];
static PLINT pen_col_min, pen_col_max;
static PLFLT pen_wd_min, pen_wd_max;
static PLFLT int_val;

// Function prototypes

static void
set_cond( register int *cond, register PLFLT *a, register PLINT n );

static int
find_interval( PLFLT a0, PLFLT a1, PLINT c0, PLINT c1, PLFLT *x );

static void
selected_polygon( PLFILL_callback fill, PLDEFINED_callback defined,
                  PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT v1, PLINT v2, PLINT v3, PLINT v4 );

static void
exfill( PLFILL_callback fill, PLDEFINED_callback defined,
        int n, PLFLT_VECTOR x, PLFLT_VECTOR y );

static void
big_recl( int *cond_code, register int ny, int dx, int dy,
          int *ix, int *iy );

static void
draw_boundary( PLINT slope, PLFLT *x, PLFLT *y );

static PLINT
plctest( PLFLT *x, PLFLT level );

static PLINT
plctestez( PLFLT *a, PLINT nx, PLINT ny, PLINT ix,
           PLINT iy, PLFLT level );

static void
plshade_int( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
             PLF2EVAL_callback c2eval, PLPointer c2eval_data,
             PLDEFINED_callback defined,
             PLINT nx, PLINT ny,
             PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
             PLFLT shade_min, PLFLT shade_max,
             PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
             PLINT min_color, PLFLT min_width,
             PLINT max_color, PLFLT max_width,
             PLFILL_callback fill, PLINT rectangular,
             PLTRANSFORM_callback pltr, PLPointer pltr_data );

// N.B. This routine only needed by the Fortran interface to distinguish
// the case where pltr and pltr_data are NULL.  So don't put declaration in
// header which might encourage others to use this in some other context.
PLDLLIMPEXP void
plshades_null( PLFLT_MATRIX a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
               PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
               PLFLT_VECTOR clevel, PLINT nlevel, PLFLT fill_width,
               PLINT cont_color, PLFLT cont_width,
               PLFILL_callback fill, PLINT rectangular )
{
    plfshades( plf2ops_c(), (PLPointer) a, nx, ny, defined,
        xmin, xmax, ymin, ymax,
        clevel, nlevel, fill_width,
        cont_color, cont_width,
        fill, rectangular,
        NULL, NULL );
}

//--------------------------------------------------------------------------
// plshades()
//
// Shade regions via a series of calls to plshade.
// All arguments are the same as plshade except the following:
// clevel is a pointer to an array of values representing
// the shade edge values, nlevel-1 is
// the number of different shades, (nlevel is the number of shade edges),
// fill_width is the pattern fill width, and cont_color and cont_width
// are the color and width of the contour drawn at each shade edge.
// (if cont_color <= 0 or cont_width <=0, no such contours are drawn).
//--------------------------------------------------------------------------

void c_plshades( PLFLT_MATRIX a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
                 PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                 PLFLT_VECTOR clevel, PLINT nlevel, PLFLT fill_width,
                 PLINT cont_color, PLFLT cont_width,
                 PLFILL_callback fill, PLINT rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plfshades( plf2ops_c(), (PLPointer) a, nx, ny, defined,
        xmin, xmax, ymin, ymax,
        clevel, nlevel, fill_width,
        cont_color, cont_width,
        fill, rectangular,
        pltr, pltr_data );
}

//--------------------------------------------------------------------------
// plfshades()
//
// Shade regions via a series of calls to plfshade1.
// All arguments are the same as plfshade1 except the following:
// clevel is a pointer to an array of values representing
// the shade edge values, nlevel-1 is
// the number of different shades, (nlevel is the number of shade edges),
// fill_width is the pattern fill width, and cont_color and cont_width
// are the color and width of the contour drawn at each shade edge.
// (if cont_color <= 0 or cont_width <=0, no such contours are drawn).
//--------------------------------------------------------------------------

void
plfshades( PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
           PLDEFINED_callback defined,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
           PLFLT_VECTOR clevel, PLINT nlevel, PLFLT fill_width,
           PLINT cont_color, PLFLT cont_width,
           PLFILL_callback fill, PLINT rectangular,
           PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLFLT shade_min, shade_max, shade_color;
    PLINT i, init_color;
    PLFLT init_width, color_min, color_max, color_range;

    // Color range to use
    color_min   = plsc->cmap1_min;
    color_max   = plsc->cmap1_max;
    color_range = color_max - color_min;

    for ( i = 0; i < nlevel - 1; i++ )
    {
        shade_min   = clevel[i];
        shade_max   = clevel[i + 1];
        shade_color = color_min + i / (PLFLT) ( nlevel - 2 ) * color_range;
        // The constants in order mean
        // (1) color map1,
        // (0, 0, 0, 0) all edge effects will be done with plcont rather
        // than the normal plshade drawing which gets partially blocked
        // when sequential shading is done as in the present case

        plfshade1( zops, zp, nx, ny, defined, xmin, xmax, ymin, ymax,
            shade_min, shade_max,
            1, shade_color, fill_width,
            0, 0, 0, 0,
            fill, rectangular, pltr, pltr_data );
    }
    if ( cont_color > 0 && cont_width > 0 )
    {
        init_color = plsc->icol0;
        init_width = plsc->width;
        plcol0( cont_color );
        plwidth( cont_width );
        if ( pltr )
        {
            plfcont( zops->f2eval, zp, nx, ny, 1, nx, 1, ny, clevel, nlevel, pltr, pltr_data );
        }
        else
        {
            // For this case use the same interpretation that occurs internally
            // for plshade.  That is set up x and y grids that map from the
            // index ranges to xmin, xmax, ymin, ymax, and use those grids
            // for the plcont call.
            //
            PLcGrid cgrid1;
            PLFLT   *x, *y;
            cgrid1.nx = nx;
            cgrid1.ny = ny;
            x         = (PLFLT *) malloc( (size_t) nx * sizeof ( PLFLT ) );
            if ( x == NULL )
                plexit( "plfshades: Out of memory for x" );
            cgrid1.xg = x;
            for ( i = 0; i < nx; i++ )
                cgrid1.xg[i] = xmin + ( xmax - xmin ) * (float) i / (float) ( nx - 1 );
            y = (PLFLT *) malloc( (size_t) ny * sizeof ( PLFLT ) );
            if ( y == NULL )
                plexit( "plfshades: Out of memory for y" );
            cgrid1.yg = y;
            for ( i = 0; i < ny; i++ )
                cgrid1.yg[i] = ymin + ( ymax - ymin ) * (float) i / (float) ( ny - 1 );
            plfcont( zops->f2eval, zp, nx, ny, 1, nx, 1, ny, clevel, nlevel,
                pltr1, (void *) &cgrid1 );
            free( x );
            free( y );
        }
        plcol0( init_color );
        plwidth( init_width );
    }
}

// N.B. This routine only needed by the Fortran interface to distinguish
// the case where pltr and pltr_data are NULL.  So don't put declaration in
// header which might encourage others to use this in some other context.
PLDLLIMPEXP void
plshade_null( PLFLT_MATRIX a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
              PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
              PLFLT shade_min, PLFLT shade_max,
              PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
              PLINT min_color, PLFLT min_width,
              PLINT max_color, PLFLT max_width,
              PLFILL_callback fill, PLINT rectangular )
{
    plshade_int( plf2eval1, (PLPointer) a,
        NULL, NULL,
//	     plc2eval, (PLPointer) &cgrid,
        defined, nx, ny, xmin,
        xmax, ymin, ymax, shade_min, shade_max,
        sh_cmap, sh_color, sh_width,
        min_color, min_width, max_color, max_width,
        fill, rectangular, NULL, NULL );
}

//--------------------------------------------------------------------------
// plshade()
//
// Shade region.
// This interface to plfshade() assumes the 2d function array is passed
// via a (PLFLT **), and is column-dominant (normal C ordering).
//--------------------------------------------------------------------------

void c_plshade( PLFLT_MATRIX a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
                PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                PLFLT shade_min, PLFLT shade_max,
                PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                PLINT min_color, PLFLT min_width,
                PLINT max_color, PLFLT max_width,
                PLFILL_callback fill, PLINT rectangular,
                PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plshade_int( plf2eval1, (PLPointer) a,
        NULL, NULL,
//	     plc2eval, (PLPointer) &cgrid,
        defined, nx, ny, xmin,
        xmax, ymin, ymax, shade_min, shade_max,
        sh_cmap, sh_color, sh_width,
        min_color, min_width, max_color, max_width,
        fill, rectangular, pltr, pltr_data );
}

#ifdef PL_DEPRECATED
// plshade1 deprecated as of plplot-5.14.0

//--------------------------------------------------------------------------
// plshade1()
//
// Shade region.
// This interface to plfshade() assumes the 2d function array is passed
// via a (PLFLT *), and is column-dominant (normal C ordering).
//--------------------------------------------------------------------------

void c_plshade1( PLFLT_VECTOR a, PLINT nx, PLINT ny, PLDEFINED_callback defined,
                 PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
                 PLFLT shade_min, PLFLT shade_max,
                 PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
                 PLINT min_color, PLFLT min_width,
                 PLINT max_color, PLFLT max_width,
                 PLFILL_callback fill, PLINT rectangular,
                 PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLfGrid grid;

    grid.f  = a;
    grid.nx = nx;
    grid.ny = ny;

    plshade_int( plf2eval, ( PLPointer ) & grid,
        NULL, NULL,
//	     plc2eval, (PLPointer) &cgrid,
        defined, nx, ny, xmin,
        xmax, ymin, ymax, shade_min, shade_max,
        sh_cmap, sh_color, sh_width,
        min_color, min_width, max_color, max_width,
        fill, rectangular, pltr, pltr_data );
}
#endif //PL_DEPRECATED

//--------------------------------------------------------------------------
// plfshade()
//
// Shade region.
// Array values are determined by the input function and the passed data.
//--------------------------------------------------------------------------

void
plfshade( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
          PLF2EVAL_callback c2eval, PLPointer c2eval_data,
          PLINT nx, PLINT ny,
          PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
          PLFLT shade_min, PLFLT shade_max,
          PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
          PLINT min_color, PLFLT min_width,
          PLINT max_color, PLFLT max_width,
          PLFILL_callback fill, PLINT rectangular,
          PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plshade_int( f2eval, f2eval_data, c2eval, c2eval_data,
        NULL,
        nx, ny, xmin, xmax, ymin, ymax,
        shade_min, shade_max, sh_cmap, sh_color, sh_width,
        min_color, min_width, max_color, max_width,
        fill, rectangular, pltr, pltr_data );
}

//--------------------------------------------------------------------------
// plfshade1()
//
// Shade region.
//
// This function is a plf2ops variant of c_plfshade and c_plfshade1.  It
// differs from plfshade in that it supports a "defined" callback (like
// c_plshade and c_plfshade1) rather than a "defined mask" (like plfshade
// even though it is not yet implemented).
//--------------------------------------------------------------------------

void
plfshade1( PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
           PLDEFINED_callback defined,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
           PLFLT shade_min, PLFLT shade_max,
           PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
           PLINT min_color, PLFLT min_width,
           PLINT max_color, PLFLT max_width,
           PLFILL_callback fill, PLINT rectangular,
           PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plshade_int( zops->f2eval, zp,
        NULL, NULL,
//	     plc2eval, (PLPointer) &cgrid,
        defined, nx, ny, xmin,
        xmax, ymin, ymax, shade_min, shade_max,
        sh_cmap, sh_color, sh_width,
        min_color, min_width, max_color, max_width,
        fill, rectangular, pltr, pltr_data );
}

//--------------------------------------------------------------------------
// plshade_int()
//
// Shade region -- this routine does all the work
//
// This routine is internal so the arguments can and will change.
// To retain some compatibility between versions, you must go through
// some stub routine!
//
// 4/95
//
// parameters:
//
// f2eval, f2eval_data:  data to plot
// defined: defined mask (old API - implimented)
// nx, ny: array dimensions
// xmin, xmax, ymin, ymax: grid coordinates
// shade_min, shade_max: shade region with values between ...
// sh_cmap, sh_color, sh_width: shading parameters, width is only for hatching
// min_color, min_width: line parameters for boundary (minimum)
// max_color, max_width: line parameters for boundary (maximum)
//     set min_width == 0 and max_width == 0 for no contours
// fill: fill function, set to NULL for no shading (contour plot)
// rectangular: flag set to 1 if pltr() maps rectangles to rectangles
//     this helps optimize the plotting
// pltr: function to map from grid to plot coordinates
//
//
//--------------------------------------------------------------------------

static void
plshade_int( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
             PLF2EVAL_callback c2eval, PLPointer PL_UNUSED( c2eval_data ), //c2eval is unused.
             PLDEFINED_callback defined,
             PLINT nx, PLINT ny,
             PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
             PLFLT shade_min, PLFLT shade_max,
             PLINT sh_cmap, PLFLT sh_color, PLFLT sh_width,
             PLINT min_color, PLFLT min_width,
             PLINT max_color, PLFLT max_width,
             PLFILL_callback fill, PLINT rectangular,
             PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLINT n, slope = 0, ix, iy;
    int   count, i, j, nxny;
    PLFLT *a, *a0, *a1, dx, dy;
    PLFLT x[8], y[8], xp[2], tx, ty, init_width;
    int   *c, *c0, *c1;

    (void) c2eval;   // Cast to void to silence compiler warning about unused parameter

    if ( plsc->level < 3 )
    {
        plabort( "plfshade: window must be set up first" );
        return;
    }

    if ( nx <= 0 || ny <= 0 )
    {
        plabort( "plfshade: nx and ny must be positive" );
        return;
    }

    if ( shade_min >= shade_max )
    {
        plabort( "plfshade: shade_max must exceed shade_min" );
        return;
    }

    if ( pltr == NULL && plsc->coordinate_transform == NULL )
        rectangular = 1;

    int_val    = shade_max - shade_min;
    init_width = plsc->width;

    pen_col_min = min_color;
    pen_col_max = max_color;

    pen_wd_min = min_width;
    pen_wd_max = max_width;

    plstyl( (PLINT) 0, NULL, NULL );
    plwidth( sh_width );
    if ( fill != NULL )
    {
        switch ( sh_cmap )
        {
        case 0:
            plcol0( (PLINT) sh_color );
            break;
        case 1:
            plcol1( sh_color );
            break;
        default:
            plabort( "plfshade: invalid color map selection" );
            return;
        }
    }
    // alloc space for value array, and initialize
    // This is only a temporary kludge
    nxny = nx * ny;
    if ( ( a = (PLFLT *) malloc( (size_t) nxny * sizeof ( PLFLT ) ) ) == NULL )
    {
        plabort( "plfshade: unable to allocate memory for value array" );
        return;
    }

    for ( ix = 0; ix < nx; ix++ )
        for ( iy = 0; iy < ny; iy++ )
            a[iy + ix * ny] = f2eval( ix, iy, f2eval_data );

    // alloc space for condition codes

    if ( ( c = (int *) malloc( (size_t) nxny * sizeof ( int ) ) ) == NULL )
    {
        plabort( "plfshade: unable to allocate memory for condition codes" );
        free( a );
        return;
    }

    sh_min = shade_min;
    sh_max = shade_max;

    set_cond( c, a, nxny );
    dx = ( xmax - xmin ) / ( nx - 1 );
    dy = ( ymax - ymin ) / ( ny - 1 );
    a0 = a;
    a1 = a + ny;
    c0 = c;
    c1 = c + ny;

    for ( ix = 0; ix < nx - 1; ix++ )
    {
        for ( iy = 0; iy < ny - 1; iy++ )
        {
            count = c0[iy] + c0[iy + 1] + c1[iy] + c1[iy + 1];

            // No filling needs to be done for these cases

            if ( count >= UNDEF )
                continue;
            if ( count == 4 * POS )
                continue;
            if ( count == 4 * NEG )
                continue;

            // Entire rectangle can be filled

            if ( count == 4 * OK )
            {
                // find biggest rectangle that fits
                if ( rectangular )
                {
                    big_recl( c0 + iy, ny, nx - ix, ny - iy, &i, &j );
                }
                else
                {
                    i = j = 1;
                }
                x[0] = x[1] = ix;
                x[2] = x[3] = ix + i;
                y[0] = y[3] = iy;
                y[1] = y[2] = iy + j;

                if ( pltr )
                {
                    for ( i = 0; i < 4; i++ )
                    {
                        ( *pltr )( x[i], y[i], &tx, &ty, pltr_data );
                        x[i] = tx;
                        y[i] = ty;
                    }
                }
                else
                {
                    for ( i = 0; i < 4; i++ )
                    {
                        x[i] = xmin + x[i] * dx;
                        y[i] = ymin + y[i] * dy;
                    }
                }
                if ( fill != NULL )
                    exfill( fill, defined, (PLINT) 4, x, y );
                iy += j - 1;
                continue;
            }

            // Only part of rectangle can be filled

            n_point = min_points = max_points = 0;
            n       = find_interval( a0[iy], a0[iy + 1], c0[iy], c0[iy + 1], xp );
            for ( j = 0; j < n; j++ )
            {
                x[j] = ix;
                y[j] = iy + xp[j];
            }

            i = find_interval( a0[iy + 1], a1[iy + 1],
                c0[iy + 1], c1[iy + 1], xp );

            for ( j = 0; j < i; j++ )
            {
                x[j + n] = ix + xp[j];
                y[j + n] = iy + 1;
            }
            n += i;

            i = find_interval( a1[iy + 1], a1[iy], c1[iy + 1], c1[iy], xp );
            for ( j = 0; j < i; j++ )
            {
                x[n + j] = ix + 1;
                y[n + j] = iy + 1 - xp[j];
            }
            n += i;

            i = find_interval( a1[iy], a0[iy], c1[iy], c0[iy], xp );
            for ( j = 0; j < i; j++ )
            {
                x[n + j] = ix + 1 - xp[j];
                y[n + j] = iy;
            }
            n += i;

            if ( pltr )
            {
                for ( i = 0; i < n; i++ )
                {
                    ( *pltr )( x[i], y[i], &tx, &ty, pltr_data );
                    x[i] = tx;
                    y[i] = ty;
                }
            }
            else
            {
                for ( i = 0; i < n; i++ )
                {
                    x[i] = xmin + x[i] * dx;
                    y[i] = ymin + y[i] * dy;
                }
            }

            if ( min_points == 4 )
                slope = plctestez( a, nx, ny, ix, iy, shade_min );
            if ( max_points == 4 )
                slope = plctestez( a, nx, ny, ix, iy, shade_max );

            // n = number of end of line segments
            // min_points = number times shade_min meets edge
            // max_points = number times shade_max meets edge

            // special cases: check number of times a contour is in a box

            switch ( ( min_points << 3 ) + max_points )
            {
            case 000:
            case 020:
            case 002:
            case 022:
                if ( fill != NULL && n > 0 )
                    exfill( fill, defined, n, x, y );
                break;
            case 040:   // 2 contour lines in box
            case 004:
                if ( n != 6 )
                    fprintf( stderr, "plfshade err n=%d !6", (int) n );
                if ( slope == 1 && c0[iy] == OK )
                {
                    if ( fill != NULL )
                        exfill( fill, defined, n, x, y );
                }
                else if ( slope == 1 )
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 2, -1 );
                    selected_polygon( fill, defined, x, y, 3, 4, 5, -1 );
                }
                else if ( c0[iy + 1] == OK )
                {
                    if ( fill != NULL )
                        exfill( fill, defined, n, x, y );
                }
                else
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 5, -1 );
                    selected_polygon( fill, defined, x, y, 2, 3, 4, -1 );
                }
                break;
            case 044:
                if ( n != 8 )
                    fprintf( stderr, "plfshade err n=%d !8", (int) n );
                if ( slope == 1 )
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 2, 3 );
                    selected_polygon( fill, defined, x, y, 4, 5, 6, 7 );
                }
                else
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 6, 7 );
                    selected_polygon( fill, defined, x, y, 2, 3, 4, 5 );
                }
                break;
            case 024:
            case 042:
                // 3 contours
                if ( n != 7 )
                    fprintf( stderr, "plfshade err n=%d !7", (int) n );

                if ( ( c0[iy] == OK || c1[iy + 1] == OK ) && slope == 1 )
                {
                    if ( fill != NULL )
                        exfill( fill, defined, n, x, y );
                }
                else if ( ( c0[iy + 1] == OK || c1[iy] == OK ) && slope == 0 )
                {
                    if ( fill != NULL )
                        exfill( fill, defined, n, x, y );
                }

                else if ( c0[iy] == OK )
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 6, -1 );
                    selected_polygon( fill, defined, x, y, 2, 3, 4, 5 );
                }
                else if ( c0[iy + 1] == OK )
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 2, -1 );
                    selected_polygon( fill, defined, x, y, 3, 4, 5, 6 );
                }
                else if ( c1[iy + 1] == OK )
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 5, 6 );
                    selected_polygon( fill, defined, x, y, 2, 3, 4, -1 );
                }
                else if ( c1[iy] == OK )
                {
                    selected_polygon( fill, defined, x, y, 0, 1, 2, 3 );
                    selected_polygon( fill, defined, x, y, 4, 5, 6, -1 );
                }
                else
                {
                    fprintf( stderr, "plfshade err logic case 024:042\n" );
                }
                break;
            default:
                fprintf( stderr, "prog err switch\n" );
                break;
            }
            draw_boundary( slope, x, y );

            if ( fill != NULL )
            {
                plwidth( sh_width );
                if ( sh_cmap == 0 )
                    plcol0( (PLINT) sh_color );
                else if ( sh_cmap == 1 )
                    plcol1( sh_color );
            }
        }

        a0  = a1;
        c0  = c1;
        a1 += ny;
        c1 += ny;
    }

    free( c );
    free( a );
    plwidth( init_width );
}

//--------------------------------------------------------------------------
// set_cond()
//
// Fills out condition code array.
//--------------------------------------------------------------------------

static void
set_cond( register int *cond, register PLFLT *a, register PLINT n )
{
    while ( n-- )
    {
        if ( *a < sh_min )
            *cond++ = NEG;
        else if ( *a > sh_max )
            *cond++ = POS;
        else if ( isnan( *a ) ) //check for nans and set cond to undefined
            *cond++ = UNDEF;
        else
            *cond++ = OK;
        a++;
    }
}

//--------------------------------------------------------------------------
// find_interval()
//
// Two points x(0) = a0, (condition code c0) x(1) = a1, (condition code c1)
// return interval on the line that are shaded
//
// returns 0 : no points to be shaded 1 : x[0] <= x < 1 is the interval 2 :
// x[0] <= x <= x[1] < 1 interval to be shaded n_point, max_points,
// min_points are incremented location of min/max_points are stored
//--------------------------------------------------------------------------

static int
find_interval( PLFLT a0, PLFLT a1, PLINT c0, PLINT c1, PLFLT *x )
{
    register int n;

    n = 0;
    if ( c0 == OK )
    {
        x[n++] = 0.0;
        n_point++;
    }
    if ( c0 == c1 )
        return n;

    if ( c0 == NEG || c1 == POS )
    {
        if ( c0 == NEG )
        {
            x[n++] = linear( a0, a1, sh_min );
            min_pts[min_points++] = n_point++;
        }
        if ( c1 == POS )
        {
            x[n++] = linear( a0, a1, sh_max );
            max_pts[max_points++] = n_point++;
        }
    }
    if ( c0 == POS || c1 == NEG )
    {
        if ( c0 == POS )
        {
            x[n++] = linear( a0, a1, sh_max );
            max_pts[max_points++] = n_point++;
        }
        if ( c1 == NEG )
        {
            x[n++] = linear( a0, a1, sh_min );
            min_pts[min_points++] = n_point++;
        }
    }
    return n;
}

//--------------------------------------------------------------------------
// selected_polygon()
//
// Draws a polygon from points in x[] and y[].
// Point selected by v1..v4
//--------------------------------------------------------------------------

static void
selected_polygon( PLFILL_callback fill, PLDEFINED_callback defined,
                  PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT v1, PLINT v2, PLINT v3, PLINT v4 )
{
    register PLINT n = 0;
    PLFLT          xx[4], yy[4];

    if ( fill == NULL )
        return;
    if ( v1 >= 0 )
    {
        xx[n]   = x[v1];
        yy[n++] = y[v1];
    }
    if ( v2 >= 0 )
    {
        xx[n]   = x[v2];
        yy[n++] = y[v2];
    }
    if ( v3 >= 0 )
    {
        xx[n]   = x[v3];
        yy[n++] = y[v3];
    }
    if ( v4 >= 0 )
    {
        xx[n]   = x[v4];
        yy[n++] = y[v4];
    }
    exfill( fill, defined, n, (PLFLT *) xx, (PLFLT *) yy );
}

//--------------------------------------------------------------------------
// bisect()
//
// Find boundary recursively by bisection.
// (x1, y1) is in the defined region, while (x2, y2) in the undefined one.
// The result is returned in
//--------------------------------------------------------------------------

static void
bisect( PLDEFINED_callback defined, PLINT niter,
        PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2, PLFLT* xb, PLFLT* yb )
{
    PLFLT xm;
    PLFLT ym;

    if ( niter == 0 )
    {
        *xb = x1;
        *yb = y1;
        return;
    }

    xm = ( x1 + x2 ) / 2.;
    ym = ( y1 + y2 ) / 2.;

    if ( defined( xm, ym ) )
        bisect( defined, niter - 1, xm, ym, x2, y2, xb, yb );
    else
        bisect( defined, niter - 1, x1, y1, xm, ym, xb, yb );
}

//--------------------------------------------------------------------------
// exfill()
//
// Fills a polygon from points in x[] and y[] with all points in
// undefined regions dropped and replaced by points at the bisected
// edge of the defined region.
// Note, undefined regions that are confined to the areas between polygon
// points are completely ignored.  Also, a range of undefined polygon points
// are simply replaced with a straight line with accurately bisected end
// points.  So this routine can produce problematic plotted results
// if the polygon is not a lot smaller than the typical resolution of
// the defined region.
//--------------------------------------------------------------------------

static void
exfill( PLFILL_callback fill, PLDEFINED_callback defined,
        int n, PLFLT_VECTOR x, PLFLT_VECTOR y )
{
    if ( n < 3 )
    {
        plabort( "exfill: Not enough points in object" );
        return;
    }

    if ( defined == NULL )

        ( *fill )( n, x, y );

    else
    {
        PLFLT *xx;
        PLFLT *yy;
        PLFLT xb, yb;
        PLINT count      = 0;
        PLINT im1        = n - 1;
        PLINT is_defined = defined( x[im1], y[im1] );
        PLINT i;

        // Slightly less than 2 n points are required for xx, yy, but
        // allocate room for 2 n to be safe.
        if ( ( xx = (PLFLT *) malloc( 2 * (size_t) n * sizeof ( PLFLT ) ) ) == NULL )
            plexit( "exfill: out of memory for xx" );
        if ( ( yy = (PLFLT *) malloc( 2 * (size_t) n * sizeof ( PLFLT ) ) ) == NULL )
            plexit( "exfill: out of memory for yy." );

        for ( i = 0; i < n; i++ )
        {
            // is_defined tells whether im1 point was in defined region.
            if ( defined( x[i], y[i] ) )
            {
                if ( !is_defined )
                {
                    // Cross from undefined (at im1) to defined region.
                    // Bisect for the first point inside the defined region
                    // and add it to xx, yy.
                    bisect( defined, NUMBER_BISECTIONS,
                        x[i], y[i], x[im1], y[im1], &xb, &yb );
                    xx[count]   = xb;
                    yy[count++] = yb;
                }
                // x[i], y[i] known to be in defined region so add this
                // point to xx, yy.
                xx[count]   = x[i];
                yy[count++] = y[i];
                is_defined  = 1;
            }
            else
            {
                if ( is_defined )
                {
                    // Cross from defined (at im1) to undefined region.
                    // Bisect for the last point in the defined region and
                    // add it to xx, yy.
                    bisect( defined, NUMBER_BISECTIONS,
                        x[im1], y[im1], x[i], y[i], &xb, &yb );
                    xx[count]   = xb;
                    yy[count++] = yb;
                    is_defined  = 0;
                }
            }
            im1 = i;
        }

        if ( count >= 3 )
            ( *fill )( count, (PLFLT_VECTOR) xx, (PLFLT_VECTOR) yy );

        free( xx );
        free( yy );
    }
}

//--------------------------------------------------------------------------
// big_recl()
//
// find a big rectangle for shading
//
// 2 goals: minimize calls to (*fill)()
//  keep ratio 1:3 for biggest rectangle
//
// only called by plshade()
//
// assumed that a 1 x 1 square already fits
//
// c[] = condition codes
// ny = c[1][0] == c[ny]  (you know what I mean)
//
// returns ix, iy = length of rectangle in grid units
//
// ix < dx - 1
// iy < dy - 1
//
// If iy == 1 -> ix = 1 (so that cond code can be set to skip)
//--------------------------------------------------------------------------

#define RATIO    3
#define COND( x, y )    cond_code[x * ny + y]

static void
big_recl( int *cond_code, register int ny, int dx, int dy,
          int *ix, int *iy )
{
    int          ok_x, ok_y, j;
    register int i, x, y;
    register int *cond;

    // ok_x = ok to expand in x direction
    // x = current number of points in x direction

    ok_x = ok_y = 1;
    x    = y = 2;

    while ( ok_x || ok_y )
    {
#ifdef RATIO
        if ( RATIO * x <= y || RATIO * y <= x )
            break;
#endif
        if ( ok_y )
        {
            // expand in vertical
            ok_y = 0;
            if ( y == dy )
                continue;
            cond = &COND( 0, y );
            for ( i = 0; i < x; i++ )
            {
                if ( *cond != OK )
                    break;
                cond += ny;
            }
            if ( i == x )
            {
                // row is ok
                y++;
                ok_y = 1;
            }
        }
        if ( ok_x )
        {
            if ( y == 2 )
                break;
            // expand in x direction
            ok_x = 0;
            if ( x == dx )
                continue;
            cond = &COND( x, 0 );
            for ( i = 0; i < y; i++ )
            {
                if ( *cond++ != OK )
                    break;
            }
            if ( i == y )
            {
                // column is OK
                x++;
                ok_x = 1;
            }
        }
    }

    // found the largest rectangle of 'ix' by 'iy'
    *ix = --x;
    *iy = --y;

    // set condition code to UNDEF in interior of rectangle

    for ( i = 1; i < x; i++ )
    {
        cond = &COND( i, 1 );
        for ( j = 1; j < y; j++ )
        {
            *cond++ = UNDEF;
        }
    }
}

//--------------------------------------------------------------------------
// draw_boundary()
//
// Draw boundaries of contour regions based on min_pts[], and max_pts[].
//--------------------------------------------------------------------------

static void
draw_boundary( PLINT slope, PLFLT *x, PLFLT *y )
{
    int i;

    if ( pen_col_min != 0 && pen_wd_min != 0 && min_points != 0 )
    {
        plcol0( pen_col_min );
        plwidth( pen_wd_min );
        if ( min_points == 4 && slope == 0 )
        {
            // swap points 1 and 3
            i          = min_pts[1];
            min_pts[1] = min_pts[3];
            min_pts[3] = i;
        }
        pljoin( x[min_pts[0]], y[min_pts[0]], x[min_pts[1]], y[min_pts[1]] );
        if ( min_points == 4 )
        {
            pljoin( x[min_pts[2]], y[min_pts[2]], x[min_pts[3]],
                y[min_pts[3]] );
        }
    }
    if ( pen_col_max != 0 && pen_wd_max != 0 && max_points != 0 )
    {
        plcol0( pen_col_max );
        plwidth( pen_wd_max );
        if ( max_points == 4 && slope == 0 )
        {
            // swap points 1 and 3
            i          = max_pts[1];
            max_pts[1] = max_pts[3];
            max_pts[3] = i;
        }
        pljoin( x[max_pts[0]], y[max_pts[0]], x[max_pts[1]], y[max_pts[1]] );
        if ( max_points == 4 )
        {
            pljoin( x[max_pts[2]], y[max_pts[2]], x[max_pts[3]],
                y[max_pts[3]] );
        }
    }
}

//--------------------------------------------------------------------------
//
// plctest( &(x[0][0]), PLFLT level)
// where x was defined as PLFLT x[4][4];
//
// determines if the contours associated with level have
// positive slope or negative slope in the box:
//
//  (2,3)   (3,3)
//
//  (2,2)   (3,2)
//
// this is heuristic and can be changed by the user
//
// return 1 if positive slope
//        0 if negative slope
//
// algorithmn:
//      1st test:
//	find length of contours assuming positive and negative slopes
//      if the length of the negative slope contours is much bigger
//	than the positive slope, then the slope is positive.
//      (and vice versa)
//      (this test tries to minimize the length of contours)
//
//      2nd test:
//      if abs((top-right corner) - (bottom left corner)) >
//	   abs((top-left corner) - (bottom right corner)) ) then
//		return negatiave slope.
//      (this test tries to keep the slope for different contour levels
//	the same)
//--------------------------------------------------------------------------

#define X( a, b )    ( x[a * 4 + b] )
#define POSITIVE_SLOPE    (PLINT) 1
#define NEGATIVE_SLOPE    (PLINT) 0
#define RATIO_SQ          6.0

static PLINT
plctest( PLFLT *x, PLFLT PL_UNUSED( level ) )
{
    int    i, j;
    double t[4], sorted[4], temp;

    sorted[0] = t[0] = X( 1, 1 );
    sorted[1] = t[1] = X( 2, 2 );
    sorted[2] = t[2] = X( 1, 2 );
    sorted[3] = t[3] = X( 2, 1 );

    for ( j = 1; j < 4; j++ )
    {
        temp = sorted[j];
        i    = j - 1;
        while ( i >= 0 && sorted[i] > temp )
        {
            sorted[i + 1] = sorted[i];
            i--;
        }
        sorted[i + 1] = temp;
    }
    // sorted[0] == min

    // find min contour
    temp = int_val * ceil( sorted[0] / int_val );
    if ( temp < sorted[1] )
    {
        // one contour line
        for ( i = 0; i < 4; i++ )
        {
            if ( t[i] < temp )
                return i / 2;
        }
    }

    // find max contour
    temp = int_val * floor( sorted[3] / int_val );
    if ( temp > sorted[2] )
    {
        // one contour line
        for ( i = 0; i < 4; i++ )
        {
            if ( t[i] > temp )
                return i / 2;
        }
    }
    // nothing better to do - be consistant
    return POSITIVE_SLOPE;
}

//--------------------------------------------------------------------------
// plctestez
//
// second routine - easier to use
// fills in x[4][4] and calls plctest
//
// test location a[ix][iy] (lower left corner)
//--------------------------------------------------------------------------

static PLINT
plctestez( PLFLT *a, PLINT nx, PLINT ny, PLINT ix,
           PLINT iy, PLFLT level )
{
    PLFLT x[4][4];
    int   i, j, ii, jj;

    for ( i = 0; i < 4; i++ )
    {
        ii = ix + i - 1;
        ii = MAX( 0, ii );
        ii = MIN( ii, nx - 1 );
        for ( j = 0; j < 4; j++ )
        {
            jj      = iy + j - 1;
            jj      = MAX( 0, jj );
            jj      = MIN( jj, ny - 1 );
            x[i][j] = a[ii * ny + jj];
        }
    }
    return plctest( &( x[0][0] ), level );
}
