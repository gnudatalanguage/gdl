//      Predefined 2-D data access functions.
//
// Copyright (C) 2010 David H. E. MacMahon
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

#include "plplotP.h"

//
// 2-D data access functions for data stored in (PLFLT **), such as the C
// variable z declared as...
//
//   PLFLT z[nx][ny];
//
// These functions are named plf2OP1, where OP is "get", "set", etc.  The
// plf2ops_t instance named "plf2ops1" is also defined below.
//

static PLFLT
plf2ops_c_get( PLPointer p, PLINT ix, PLINT iy )
{
    return ( (PLFLT **) p )[ix][iy];
}

static PLFLT
plf2ops_c_f2eval( PLINT ix, PLINT iy, PLPointer p )
{
    return ( (PLFLT **) p )[ix][iy];
}

static PLFLT
plf2ops_c_set( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( (PLFLT **) p )[ix][iy] = z );
}

static PLFLT
plf2ops_c_add( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( (PLFLT **) p )[ix][iy] += z );
}

static PLFLT
plf2ops_c_sub( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( (PLFLT **) p )[ix][iy] -= z );
}

static PLFLT
plf2ops_c_mul( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( (PLFLT **) p )[ix][iy] *= z );
}

static PLFLT
plf2ops_c_div( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( (PLFLT **) p )[ix][iy] /= z );
}

static PLINT
plf2ops_c_isnan( PLPointer p, PLINT ix, PLINT iy )
{
    return isnan( ( (PLFLT **) p )[ix][iy] );
}

static void
plf2ops_c_minmax( PLPointer p, PLINT nx, PLINT ny, PLFLT *zmin, PLFLT *zmax )
{
    int   i, j;
    PLFLT min, max;
    PLFLT **z = (PLFLT **) p;

    if ( !isfinite( z[0][0] ) )
    {
        max = -HUGE_VAL;
        min = HUGE_VAL;
    }
    else
        min = max = z[0][0];

    for ( i = 0; i < nx; i++ )
    {
        for ( j = 0; j < ny; j++ )
        {
            if ( !isfinite( z[i][j] ) )
                continue;
            if ( z[i][j] < min )
                min = z[i][j];
            if ( z[i][j] > max )
                max = z[i][j];
        }
    }
    *zmin = min;
    *zmax = max;
}

static plf2ops_t s_plf2ops_c = {
    plf2ops_c_get,
    plf2ops_c_set,
    plf2ops_c_add,
    plf2ops_c_sub,
    plf2ops_c_mul,
    plf2ops_c_div,
    plf2ops_c_isnan,
    plf2ops_c_minmax,
    plf2ops_c_f2eval
};

PLF2OPS
plf2ops_c()
{
    return &s_plf2ops_c;
}

//
// 2-D data access functions for data stored in (PLfGrid2 *), with the
// PLfGrid2's "f" field treated as type (PLFLT **).
//

static PLFLT
plf2ops_grid_c_get( PLPointer p, PLINT ix, PLINT iy )
{
    return ( ( (PLfGrid2 *) p )->f )[ix][iy];
}

static PLFLT
plf2ops_grid_c_f2eval( PLINT ix, PLINT iy, PLPointer p )
{
    return ( ( (PLfGrid2 *) p )->f )[ix][iy];
}

static PLFLT
plf2ops_grid_c_set( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( ( (PLfGrid2 *) p )->f )[ix][iy] = z );
}

static PLFLT
plf2ops_grid_c_add( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( ( (PLfGrid2 *) p )->f )[ix][iy] += z );
}

static PLFLT
plf2ops_grid_c_sub( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( ( (PLfGrid2 *) p )->f )[ix][iy] -= z );
}

static PLFLT
plf2ops_grid_c_mul( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( ( (PLfGrid2 *) p )->f )[ix][iy] *= z );
}

static PLFLT
plf2ops_grid_c_div( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    return ( ( ( (PLfGrid2 *) p )->f )[ix][iy] /= z );
}

static PLINT
plf2ops_grid_c_isnan( PLPointer p, PLINT ix, PLINT iy )
{
    return isnan( ( ( (PLfGrid2 *) p )->f )[ix][iy] );
}

static void
plf2ops_grid_c_minmax( PLPointer p, PLINT nx, PLINT ny, PLFLT *zmin, PLFLT *zmax )
{
    int      i, j;
    PLFLT    min, max;
    PLfGrid2 *g  = (PLfGrid2 *) p;
    PLFLT    **z = g->f;

    // Ignore passed in parameters
    nx = g->nx;
    ny = g->ny;

    if ( !isfinite( z[0][0] ) )
    {
        max = -HUGE_VAL;
        min = HUGE_VAL;
    }
    else
        min = max = z[0][0];

    for ( i = 0; i < nx; i++ )
    {
        for ( j = 0; j < ny; j++ )
        {
            if ( !isfinite( z[i][j] ) )
                continue;
            if ( z[i][j] < min )
                min = z[i][j];
            if ( z[i][j] > max )
                max = z[i][j];
        }
    }
    *zmin = min;
    *zmax = max;
}

static plf2ops_t s_plf2ops_grid_c = {
    plf2ops_grid_c_get,
    plf2ops_grid_c_set,
    plf2ops_grid_c_add,
    plf2ops_grid_c_sub,
    plf2ops_grid_c_mul,
    plf2ops_grid_c_div,
    plf2ops_grid_c_isnan,
    plf2ops_grid_c_minmax,
    plf2ops_grid_c_f2eval
};

PLF2OPS
plf2ops_grid_c()
{
    return &s_plf2ops_grid_c;
}

//
// 2-D data access functions for data stored in (PLfGrid2 *), with the
// PLfGrid2's "f" field treated as type (PLFLT *) pointing to 2-D data stored
// in row-major order.  In the context of plotting, it might be easier to think
// of it as "X-major" order.  In this ordering, values for a single X index are
// stored in consecutive memory locations.
//

static PLFLT
plf2ops_grid_row_major_get( PLPointer p, PLINT ix, PLINT iy )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( (PLFLT *) g->f )[ix * g->ny + iy];
}

static PLFLT
plf2ops_grid_row_major_f2eval( PLINT ix, PLINT iy, PLPointer p )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( (PLFLT *) g->f )[ix * g->ny + iy];
}

static PLFLT
plf2ops_grid_row_major_set( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix * g->ny + iy] = z );
}

static PLFLT
plf2ops_grid_row_major_add( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix * g->ny + iy] += z );
}

static PLFLT
plf2ops_grid_row_major_sub( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix * g->ny + iy] -= z );
}

static PLFLT
plf2ops_grid_row_major_mul( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix * g->ny + iy] *= z );
}

static PLFLT
plf2ops_grid_row_major_div( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix * g->ny + iy] /= z );
}

static PLINT
plf2ops_grid_row_major_isnan( PLPointer p, PLINT ix, PLINT iy )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return isnan( ( (PLFLT *) g->f )[ix * g->ny + iy] );
}

static void
plf2ops_grid_xxx_major_minmax( PLPointer p, PLINT nx, PLINT ny, PLFLT *zmin, PLFLT *zmax )
{
    int      i;
    PLFLT    min, max;
    PLfGrid2 *g = (PLfGrid2 *) p;
    PLFLT    *z = (PLFLT *) ( (PLFLT *) g->f );

    // Ignore passed in parameters
    nx = g->nx;
    ny = g->ny;

    if ( !isfinite( z[0] ) )
    {
        max = -HUGE_VAL;
        min = HUGE_VAL;
    }
    else
        min = max = z[0];

    for ( i = 0; i < nx * ny; i++ )
    {
        if ( !isfinite( z[i] ) )
            continue;
        if ( z[i] < min )
            min = z[i];
        if ( z[i] > max )
            max = z[i];
    }
    *zmin = min;
    *zmax = max;
}

static plf2ops_t s_plf2ops_grid_row_major = {
    plf2ops_grid_row_major_get,
    plf2ops_grid_row_major_set,
    plf2ops_grid_row_major_add,
    plf2ops_grid_row_major_sub,
    plf2ops_grid_row_major_mul,
    plf2ops_grid_row_major_div,
    plf2ops_grid_row_major_isnan,
    plf2ops_grid_xxx_major_minmax,
    plf2ops_grid_row_major_f2eval
};

PLF2OPS
plf2ops_grid_row_major()
{
    return &s_plf2ops_grid_row_major;
}

//
// 2-D data access functions for data stored in (PLfGrid2 *), with the
// PLfGrid2's "f" field treated as type (PLFLT *) pointing to 2-D data stored
// in column-major order.  In the context of plotting, it might be easier to
// think of it as "Y-major" order.  In this ordering, values for a single Y
// index are stored in consecutive memory locations.
//

static PLFLT
plf2ops_grid_col_major_get( PLPointer p, PLINT ix, PLINT iy )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( (PLFLT *) g->f )[ix + g->nx * iy];
}

static PLFLT
plf2ops_grid_col_major_f2eval( PLINT ix, PLINT iy, PLPointer p )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( (PLFLT *) g->f )[ix + g->nx * iy];
}

static PLFLT
plf2ops_grid_col_major_set( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix + g->nx * iy] = z );
}

static PLFLT
plf2ops_grid_col_major_add( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix + g->nx * iy] += z );
}

static PLFLT
plf2ops_grid_col_major_sub( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix + g->nx * iy] -= z );
}

static PLFLT
plf2ops_grid_col_major_mul( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix + g->nx * iy] *= z );
}

static PLFLT
plf2ops_grid_col_major_div( PLPointer p, PLINT ix, PLINT iy, PLFLT z )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return ( ( (PLFLT *) g->f )[ix + g->nx * iy] /= z );
}

static PLINT
plf2ops_grid_col_major_isnan( PLPointer p, PLINT ix, PLINT iy )
{
    PLfGrid2 *g = (PLfGrid2 *) p;
    return isnan( ( (PLFLT *) g->f )[ix + g->nx * iy] );
}

plf2ops_t s_plf2ops_grid_col_major = {
    plf2ops_grid_col_major_get,
    plf2ops_grid_col_major_set,
    plf2ops_grid_col_major_add,
    plf2ops_grid_col_major_sub,
    plf2ops_grid_col_major_mul,
    plf2ops_grid_col_major_div,
    plf2ops_grid_col_major_isnan,
    plf2ops_grid_xxx_major_minmax,
    plf2ops_grid_col_major_f2eval
};

PLF2OPS
plf2ops_grid_col_major()
{
    return &s_plf2ops_grid_col_major;
}
