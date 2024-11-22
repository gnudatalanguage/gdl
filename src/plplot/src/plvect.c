//      Vector plotting routines.
//
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

#define NEED_PLDEBUG
#include "plplotP.h"
#include <float.h>
#include <ctype.h>

// Static function prototypes

static void plP_plotvect( PLFLT x, PLFLT y, PLFLT u, PLFLT v, PLFLT scale );

//--------------------------------------------------------------------------
// N.B. This routine only needed by the Fortran interface to distinguish
// the case where both arrowx and arrowy are NULL.  So don't put declaration in
// header which might encourage others to use this in some other context.
//--------------------------------------------------------------------------
PLDLLIMPEXP void
plsvect_null( void )
{
    c_plsvect( NULL, NULL, 0, 0 );
}

//--------------------------------------------------------------------------
// void c_plsvect()
//
// Set the style of the arrow used by plvect
//--------------------------------------------------------------------------

void
c_plsvect( PLFLT_VECTOR arrowx, PLFLT_VECTOR arrowy, PLINT npts, PLBOOL fill )
{
    int   i;
    PLFLT def_arrow_x[6] = { -0.5, 0.5, 0.3, 0.5, 0.3, 0.5 };
    PLFLT def_arrow_y[6] = { 0.0, 0.0, 0.2, 0.0, -0.2, 0.0 };

    if ( plsc->arrow_x )
        free_mem( plsc->arrow_x );
    if ( plsc->arrow_y )
        free_mem( plsc->arrow_y );

    // Reset default arrow if null pointers are passed.
    if ( arrowx == NULL && arrowy == NULL )
    {
        arrowx = def_arrow_x;
        arrowy = def_arrow_y;
        npts   = 6;
        fill   = 0;
    }

    if ( ( ( plsc->arrow_x = (PLFLT *) malloc( (size_t) npts * sizeof ( PLFLT ) ) ) == NULL ) ||
         ( ( plsc->arrow_y = (PLFLT *) malloc( (size_t) npts * sizeof ( PLFLT ) ) ) == NULL ) )
    {
        plexit( "c_plsvect: Insufficient memory" );
    }

    plsc->arrow_npts = npts;
    plsc->arrow_fill = fill;
    for ( i = 0; i < npts; i++ )
    {
        plsc->arrow_x[i] = arrowx[i];
        plsc->arrow_y[i] = arrowy[i];
    }
}

//
// Plot an individual vector
//
static void
plP_plotvect( PLFLT x, PLFLT y, PLFLT u, PLFLT v, PLFLT scale )
{
    PLFLT uu, vv, px0, py0, dpx, dpy;
    PLFLT xt, yt;
    // Unnecessarily initialize a_y to quiet a -O1 -Wuninitialized warning
    // which is a false alarm.  (If something goes wrong with the
    // a_x malloc below any further use of a_y does not occur.)
    PLINT *a_x, *a_y = NULL;
    int   j;

    uu = scale * u;
    vv = scale * v;

    if ( uu == 0.0 && vv == 0.0 )
        return;

    if ( ( ( a_x = (PLINT *) malloc( sizeof ( PLINT ) * (size_t) ( plsc->arrow_npts ) ) ) == NULL ) ||
         ( ( a_y = (PLINT *) malloc( sizeof ( PLINT ) * (size_t) ( plsc->arrow_npts ) ) ) == NULL ) )
    {
        plexit( "plP_plotvect: Insufficient memory" );
    }

    TRANSFORM( x, y, &xt, &yt );
    px0 = plP_wcpcx( xt );
    py0 = plP_wcpcy( yt );

    pldebug( "plP_plotvect", "%f %f %d %d\n", x, y, px0, py0 );

    TRANSFORM( x + 0.5 * uu, y + 0.5 * vv, &xt, &yt );
    //printf("plvect: %f %f %f %f %f %f %f\n",scale, x,0.5*uu, y,0.5*vv, xt, yt);
    dpx = plP_wcpcx( xt ) - px0;
    dpy = plP_wcpcy( yt ) - py0;

    // transform arrow -> a

    for ( j = 0; j < plsc->arrow_npts; j++ )
    {
        a_x[j] = (PLINT) ( plsc->arrow_x[j] * dpx - plsc->arrow_y[j] * dpy + px0 );
        a_y[j] = (PLINT) ( plsc->arrow_x[j] * dpy + plsc->arrow_y[j] * dpx + py0 );
    }

    // draw the arrow
    plP_draphy_poly( a_x, a_y, plsc->arrow_npts );
    if ( plsc->arrow_fill )
    {
        plP_plfclp( a_x, a_y, plsc->arrow_npts, plsc->clpxmi, plsc->clpxma,
            plsc->clpymi, plsc->clpyma, plP_fill );
    }

    free( (void *) a_x );
    free( (void *) a_y );
}

//
// void plfvect()
//
// Routine to plot a vector array with arbitrary coordinate
// and vector transformations
//
void plfvect( PLF2EVAL_callback getuv, PLPointer up, PLPointer vp,
              PLINT nx, PLINT ny, PLFLT scale,
              PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLINT i, j, i1, j1;
    PLFLT **u, **v, **x, **y;
    PLFLT lscale, dx, dy, dxmin, dymin, umax, vmax;

    if ( pltr == NULL )
    {
        // If pltr is undefined, abort with an error.
        plabort( "plfvect: The pltr callback must be defined" );
        return;
    }

    plAlloc2dGrid( &u, nx, ny );
    plAlloc2dGrid( &v, nx, ny );
    plAlloc2dGrid( &x, nx, ny );
    plAlloc2dGrid( &y, nx, ny );

    for ( j = 0; j < ny; j++ )
    {
        for ( i = 0; i < nx; i++ )
        {
            u[i][j] = getuv( i, j, up );
            v[i][j] = getuv( i, j, vp );
            pltr( (PLFLT) i, (PLFLT) j, &x[i][j], &y[i][j], pltr_data );
        }
    }

    // Calculate apropriate scaling if necessary
    if ( scale <= 0.0 )
    {
        if ( nx <= 1 && ny <= 1 )
        {
            fprintf( stderr, "plfvect: not enough points for autoscaling\n" );
            return;
        }
        dxmin = 10E10;
        dymin = 10E10;
        for ( j = 0; j < ny; j++ )
        {
            for ( i = 0; i < nx; i++ )
            {
                for ( j1 = j; j1 < ny; j1++ )
                {
                    for ( i1 = 0; i1 < nx; i1++ )
                    {
                        dx = fabs( x[i1][j1] - x[i][j] );
                        dy = fabs( y[i1][j1] - y[i][j] );
                        if ( dx > 0 )
                        {
                            dxmin = ( dx < dxmin ) ? dx : dxmin;
                        }
                        if ( dy > 0 )
                        {
                            dymin = ( dy < dymin ) ? dy : dymin;
                        }
                    }
                }
            }
        }
        umax = u[0][0];
        vmax = v[0][0];
        for ( j = 0; j < ny; j++ )
        {
            for ( i = 0; i < nx; i++ )
            {
                umax = ( u[i][j] > umax ) ? u[i][j] : umax;
                vmax = ( v[i][j] > vmax ) ? v[i][j] : vmax;
            }
        }
        if ( umax != 0.0 )
        {
            dxmin = dxmin / umax;
        }
        else
        {
            dxmin = 10E10;
        }
        if ( vmax != 0.0 )
        {
            dymin = dymin / vmax;
        }
        else
        {
            dymin = 10E10;
        }
        lscale = 1.5 * MIN( dxmin, dymin );
        if ( scale < 0.0 )
        {
            scale = -scale * lscale;
        }
        else
        {
            scale = lscale;
        }
    }

    for ( j = 0; j < ny; j++ )
    {
        for ( i = 0; i < nx; i++ )
        {
            plP_plotvect( x[i][j], y[i][j], u[i][j], v[i][j], scale );
        }
    }

    plFree2dGrid( u, nx, ny );
    plFree2dGrid( v, nx, ny );
    plFree2dGrid( x, nx, ny );
    plFree2dGrid( y, nx, ny );
}

void
c_plvect( PLFLT_MATRIX u, PLFLT_MATRIX v, PLINT nx, PLINT ny, PLFLT scale,
          PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plfvect( plf2eval1, (PLPointer) u, (PLPointer) v,
        nx, ny, scale, pltr, pltr_data );
}
