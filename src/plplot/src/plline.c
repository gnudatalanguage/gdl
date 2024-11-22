//      Routines dealing with line generation.
//
// Copyright (C) 2004  Maurice LeBrun
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

#include "plplotP.h"

#define INSIDE( ix, iy )    ( BETW( ix, xmin, xmax ) && BETW( iy, ymin, ymax ) )

static PLINT xline[PL_MAXPOLY], yline[PL_MAXPOLY];

static PLINT lastx = PL_UNDEFINED, lasty = PL_UNDEFINED;

// Function prototypes

// Draws a polyline within the clip limits.

static void
pllclp( PLINT *x, PLINT *y, PLINT npts );

// General line-drawing routine.  Takes line styles into account.

static void
genlin( short *x, short *y, PLINT npts );

// Draws a dashed line to the specified point from the previous one.

static void
grdashline( short *x, short *y );

// Determines if a point is inside a polygon or not

// Interpolate between two points in n steps

static PLFLT *
interpolate_between( int n, PLFLT a, PLFLT b );

//--------------------------------------------------------------------------
// void pljoin()
//
// Draws a line segment from (x1, y1) to (x2, y2).
//--------------------------------------------------------------------------

void
c_pljoin( PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 )
{
    plP_movwor( x1, y1 );
    plP_drawor( x2, y2 );
}

//--------------------------------------------------------------------------
// void plline()
//
// Draws line segments connecting a series of points.
//--------------------------------------------------------------------------

void
c_plline( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y )
{
    if ( plsc->level < 3 )
    {
        plabort( "plline: Please set up window first" );
        return;
    }
    plP_drawor_poly( x, y, n );
}

//--------------------------------------------------------------------------
// void plpath()
//
// Draws a line segment from (x1, y1) to (x2, y2).  If a coordinate
// transform is defined then break the line up in to n pieces to approximate
// the path.  Otherwise it simply calls pljoin().
//--------------------------------------------------------------------------

void
c_plpath( PLINT n, PLFLT x1, PLFLT y1, PLFLT x2, PLFLT y2 )
{
    PLFLT *xs, *ys;

    if ( plsc->coordinate_transform == NULL )
    {
        // No transform, so fall back on pljoin for a normal straight line
        pljoin( x1, y1, x2, y2 );
    }
    else
    {
        // Approximate the path in transformed space with a sequence of line
        // segments.
        xs = interpolate_between( n, x1, x2 );
        ys = interpolate_between( n, y1, y2 );
        if ( xs == NULL || ys == NULL )
        {
            plexit( "c_plpath: Insufficient memory" );
            return;
        }
        plline( n, xs, ys );
        // plP_interpolate allocates memory, so we have to free it here.
        free( xs );
        free( ys );
    }
}

//--------------------------------------------------------------------------
// void plline3(n, x, y, z)
//
// Draws a line in 3 space.  You must first set up the viewport, the
// 2d viewing window (in world coordinates), and the 3d normalized
// coordinate box.  See x18c.c for more info.
//
// This version adds clipping against the 3d bounding box specified in plw3d
//--------------------------------------------------------------------------
void
c_plline3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z )
{
    int   i;
    PLFLT vmin[3], vmax[3], zscale;

    if ( plsc->level < 3 )
    {
        plabort( "plline3: Please set up window first" );
        return;
    }

    // get the bounding box in 3d
    plP_gdom( &vmin[0], &vmax[0], &vmin[1], &vmax[1] );
    plP_grange( &zscale, &vmin[2], &vmax[2] );

    // interate over the vertices
    for ( i = 0; i < n - 1; i++ )
    {
        PLFLT p0[3], p1[3];
        int   axis;

        // copy the end points of the segment to allow clipping
        p0[0] = x[i]; p0[1] = y[i]; p0[2] = z[i];
        p1[0] = x[i + 1]; p1[1] = y[i + 1]; p1[2] = z[i + 1];

        // check against each axis of the bounding box
        for ( axis = 0; axis < 3; axis++ )
        {
            if ( p0[axis] < vmin[axis] ) // first out
            {
                if ( p1[axis] < vmin[axis] )
                {
                    break; // both endpoints out so quit
                }
                else
                {
                    int   j;
                    // interpolate to find intersection with box
                    PLFLT t = ( vmin[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                    p0[axis] = vmin[axis];
                    for ( j = 1; j < 3; j++ )
                    {
                        int k = ( axis + j ) % 3;
                        p0[k] = ( 1 - t ) * p0[k] + t * p1[k];
                    }
                }
            }
            else if ( p1[axis] < vmin[axis] ) // second out
            {
                int   j;
                // interpolate to find intersection with box
                PLFLT t = ( vmin[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                p1[axis] = vmin[axis];
                for ( j = 1; j < 3; j++ )
                {
                    int k = ( axis + j ) % 3;
                    p1[k] = ( 1 - t ) * p0[k] + t * p1[k];
                }
            }
            if ( p0[axis] > vmax[axis] ) // first out
            {
                if ( p1[axis] > vmax[axis] )
                {
                    break; // both out so quit
                }
                else
                {
                    int   j;
                    // interpolate to find intersection with box
                    PLFLT t = ( vmax[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                    p0[axis] = vmax[axis];
                    for ( j = 1; j < 3; j++ )
                    {
                        int k = ( axis + j ) % 3;
                        p0[k] = ( 1 - t ) * p0[k] + t * p1[k];
                    }
                }
            }
            else if ( p1[axis] > vmax[axis] ) // second out
            {
                int   j;
                // interpolate to find intersection with box
                PLFLT t = ( vmax[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                p1[axis] = vmax[axis];
                for ( j = 1; j < 3; j++ )
                {
                    int k = ( axis + j ) % 3;
                    p1[k] = ( 1 - t ) * p0[k] + t * p1[k];
                }
            }
        }
        // if we made it to here without "break"ing out of the loop, the
        // remaining segment is visible
        if ( axis == 3 ) //  not clipped away
        {
            PLFLT u0, v0, u1, v1;
            u0 = plP_wcpcx( plP_w3wcx( p0[0], p0[1], p0[2] ) );
            v0 = plP_wcpcy( plP_w3wcy( p0[0], p0[1], p0[2] ) );
            u1 = plP_wcpcx( plP_w3wcx( p1[0], p1[1], p1[2] ) );
            v1 = plP_wcpcy( plP_w3wcy( p1[0], p1[1], p1[2] ) );
            plP_movphy( (PLINT) u0, (PLINT) v0 );
            plP_draphy( (PLINT) u1, (PLINT) v1 );
        }
    }
    return;
}
//--------------------------------------------------------------------------
// void plpoly3( n, x, y, z, draw, ifcc )
//
// Draws a polygon in 3 space.  This differs from plline3() in that
// this attempts to determine if the polygon is viewable.  If the back
// of polygon is facing the viewer, then it isn't drawn.  If this
// isn't what you want, then use plline3 instead.
//
// n specifies the number of points.  They are assumed to be in a
// plane, and the directionality of the plane is determined from the
// first three points.  Additional points do not /have/ to lie on the
// plane defined by the first three, but if they do not, then the
// determiniation of visibility obviously can't be 100% accurate...
// So if you're 3 space polygons are too far from planar, consider
// breaking them into smaller polygons.  "3 points define a plane" :-).
//
// For ifcc == 1, the directionality of the polygon is determined by assuming
// the points are laid out in counter-clockwise order.
//
// For ifcc == 0, the directionality of the polygon is determined by assuming
// the points are laid out in clockwise order.
//
// BUGS:  If one of the first two segments is of zero length, or if
// they are colinear, the calculation of visibility has a 50/50 chance
// of being correct.  Avoid such situations :-).  See x18c for an
// example of this problem.  (Search for "20.1").
//--------------------------------------------------------------------------

void
c_plpoly3( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_VECTOR z, PLBOOL_VECTOR draw, PLBOOL ifcc )
{
    int   i;
    PLFLT vmin[3], vmax[3], zscale;
    PLFLT u1, v1, u2, v2, u3, v3;
    PLFLT c;

    if ( plsc->level < 3 )
    {
        plabort( "plpoly3: Please set up window first" );
        return;
    }

    if ( n < 3 )
    {
        plabort( "plpoly3: Must specify at least 3 points" );
        return;
    }

// Now figure out which side this is.

    u1 = plP_wcpcx( plP_w3wcx( x[0], y[0], z[0] ) );
    v1 = plP_wcpcy( plP_w3wcy( x[0], y[0], z[0] ) );

    u2 = plP_wcpcx( plP_w3wcx( x[1], y[1], z[1] ) );
    v2 = plP_wcpcy( plP_w3wcy( x[1], y[1], z[1] ) );

    u3 = plP_wcpcx( plP_w3wcx( x[2], y[2], z[2] ) );
    v3 = plP_wcpcy( plP_w3wcy( x[2], y[2], z[2] ) );

    c = ( u1 - u2 ) * ( v3 - v2 ) - ( v1 - v2 ) * ( u3 - u2 );

    if ( c * ( 1 - 2 * ABS( ifcc ) ) < 0. )
        return;

    // get the bounding box in 3d
    plP_gdom( &vmin[0], &vmax[0], &vmin[1], &vmax[1] );
    plP_grange( &zscale, &vmin[2], &vmax[2] );

    // interate over the vertices
    for ( i = 0; i < n - 1; i++ )
    {
        PLFLT p0[3], p1[3];
        int   axis;

        // copy the end points of the segment to allow clipping
        p0[0] = x[i]; p0[1] = y[i]; p0[2] = z[i];
        p1[0] = x[i + 1]; p1[1] = y[i + 1]; p1[2] = z[i + 1];

        // check against each axis of the bounding box
        for ( axis = 0; axis < 3; axis++ )
        {
            if ( p0[axis] < vmin[axis] ) // first out
            {
                if ( p1[axis] < vmin[axis] )
                {
                    break; // both endpoints out so quit
                }
                else
                {
                    int   j;
                    // interpolate to find intersection with box
                    PLFLT t = ( vmin[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                    p0[axis] = vmin[axis];
                    for ( j = 1; j < 3; j++ )
                    {
                        int k = ( axis + j ) % 3;
                        p0[k] = ( 1 - t ) * p0[k] + t * p1[k];
                    }
                }
            }
            else if ( p1[axis] < vmin[axis] ) // second out
            {
                int   j;
                // interpolate to find intersection with box
                PLFLT t = ( vmin[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                p1[axis] = vmin[axis];
                for ( j = 1; j < 3; j++ )
                {
                    int k = ( axis + j ) % 3;
                    p1[k] = ( 1 - t ) * p0[k] + t * p1[k];
                }
            }
            if ( p0[axis] > vmax[axis] ) // first out
            {
                if ( p1[axis] > vmax[axis] )
                {
                    break; // both out so quit
                }
                else
                {
                    int   j;
                    // interpolate to find intersection with box
                    PLFLT t = ( vmax[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                    p0[axis] = vmax[axis];
                    for ( j = 1; j < 3; j++ )
                    {
                        int k = ( axis + j ) % 3;
                        p0[k] = ( 1 - t ) * p0[k] + t * p1[k];
                    }
                }
            }
            else if ( p1[axis] > vmax[axis] ) // second out
            {
                int   j;
                // interpolate to find intersection with box
                PLFLT t = ( vmax[axis] - p0[axis] ) / ( p1[axis] - p0[axis] );
                p1[axis] = vmax[axis];
                for ( j = 1; j < 3; j++ )
                {
                    int k = ( axis + j ) % 3;
                    p1[k] = ( 1 - t ) * p0[k] + t * p1[k];
                }
            }
        }
        // if we made it to here without "break"ing out of the loop, the
        // remaining segment is visible
        if ( axis == 3 && draw[i] ) //  not clipped away
        {
            u1 = plP_wcpcx( plP_w3wcx( p0[0], p0[1], p0[2] ) );
            v1 = plP_wcpcy( plP_w3wcy( p0[0], p0[1], p0[2] ) );
            u2 = plP_wcpcx( plP_w3wcx( p1[0], p1[1], p1[2] ) );
            v2 = plP_wcpcy( plP_w3wcy( p1[0], p1[1], p1[2] ) );
            plP_movphy( (PLINT) u1, (PLINT) v1 );
            plP_draphy( (PLINT) u2, (PLINT) v2 );
        }
    }
    return;
}

//--------------------------------------------------------------------------
// void plstyl()
//
// Set up a new line style of "nms" elements, with mark and space
// lengths given by arrays "mark" and "space".
//--------------------------------------------------------------------------

void
c_plstyl( PLINT nms, PLINT_VECTOR mark, PLINT_VECTOR space )
{
    short int i;
    short int flag;

    if ( plsc->level < 1 )
    {
        plabort( "plstyl: Please call plinit first" );
        return;
    }
    if ( ( nms < 0 ) || ( nms > 10 ) )
    {
        plabort( "plstyl: Broken lines cannot have <0 or >10 elements" );
        return;
    }
    flag = 1;
    for ( i = 0; i < nms; i++ )
    {
        if ( ( mark[i] < 0 ) || ( space[i] < 0 ) )
        {
            plabort( "plstyl: Mark and space lengths must be > 0" );
            return;
        }
        if ( ( mark[i] != 0 ) || ( space[i] != 0 ) )
        {
            flag = 0;
        }
    }
    // Check for blank style
    if ( ( nms > 0 ) && ( flag == 1 ) )
    {
        plabort( "plstyl: At least one mark or space must be > 0" );
        return;
    }

    plsc->nms = nms;
    for ( i = 0; i < nms; i++ )
    {
        plsc->mark[i]  = mark[i];
        plsc->space[i] = space[i];
    }

    plsc->curel   = 0;
    plsc->pendn   = 1;
    plsc->timecnt = 0;
    plsc->alarm   = nms > 0 ? mark[0] : 0;
}

//--------------------------------------------------------------------------
// void plP_movphy()
//
// Move to physical coordinates (x,y).
//--------------------------------------------------------------------------

void
plP_movphy( PLINT x, PLINT y )
{
    plsc->currx = x;
    plsc->curry = y;
}

//--------------------------------------------------------------------------
// void plP_draphy()
//
// Draw to physical coordinates (x,y).
//--------------------------------------------------------------------------

void
plP_draphy( PLINT x, PLINT y )
{
    xline[0] = plsc->currx;
    xline[1] = x;
    yline[0] = plsc->curry;
    yline[1] = y;

    pllclp( xline, yline, 2 );
}

//--------------------------------------------------------------------------
// void plP_movwor()
//
// Move to world coordinates (x,y).
//--------------------------------------------------------------------------

void
plP_movwor( PLFLT x, PLFLT y )
{
    PLFLT xt, yt;
    TRANSFORM( x, y, &xt, &yt );

    plsc->currx = plP_wcpcx( xt );
    plsc->curry = plP_wcpcy( yt );
}

//--------------------------------------------------------------------------
// void plP_drawor()
//
// Draw to world coordinates (x,y).
//--------------------------------------------------------------------------

void
plP_drawor( PLFLT x, PLFLT y )
{
    PLFLT xt, yt;
    TRANSFORM( x, y, &xt, &yt );

    xline[0] = plsc->currx;
    xline[1] = plP_wcpcx( xt );
    yline[0] = plsc->curry;
    yline[1] = plP_wcpcy( yt );

    pllclp( xline, yline, 2 );
}

//--------------------------------------------------------------------------
// void plP_draphy_poly()
//
// Draw polyline in physical coordinates.
// Need to draw buffers in increments of (PL_MAXPOLY-1) since the
// last point must be repeated (for solid lines).
//--------------------------------------------------------------------------

void
plP_draphy_poly( PLINT *x, PLINT *y, PLINT n )
{
    PLINT i, j, ib, ilim;

    for ( ib = 0; ib < n; ib += PL_MAXPOLY - 1 )
    {
        ilim = MIN( PL_MAXPOLY, n - ib );

        for ( i = 0; i < ilim; i++ )
        {
            j        = ib + i;
            xline[i] = x[j];
            yline[i] = y[j];
        }
        pllclp( xline, yline, ilim );
    }
}

//--------------------------------------------------------------------------
// void plP_drawor_poly()
//
// Draw polyline in world coordinates.
// Need to draw buffers in increments of (PL_MAXPOLY-1) since the
// last point must be repeated (for solid lines).
//--------------------------------------------------------------------------

void
plP_drawor_poly( PLFLT_VECTOR x, PLFLT_VECTOR y, PLINT n )
{
    PLINT i, j, ib, ilim;
    PLFLT xt, yt;

    for ( ib = 0; ib < n; ib += PL_MAXPOLY - 1 )
    {
        ilim = MIN( PL_MAXPOLY, n - ib );

        for ( i = 0; i < ilim; i++ )
        {
            j = ib + i;
            TRANSFORM( x[j], y[j], &xt, &yt );
            xline[i] = plP_wcpcx( xt );
            yline[i] = plP_wcpcy( yt );
        }
        pllclp( xline, yline, ilim );
    }
}

//--------------------------------------------------------------------------
// void pllclp()
//
// Draws a polyline within the clip limits.
// Merely a front-end to plP_pllclp().
//--------------------------------------------------------------------------

static void
pllclp( PLINT *x, PLINT *y, PLINT npts )
{
    plP_pllclp( x, y, npts, plsc->clpxmi, plsc->clpxma,
        plsc->clpymi, plsc->clpyma, genlin );
}

//--------------------------------------------------------------------------
// void plP_pllclp()
//
// Draws a polyline within the clip limits.
//
// (AM)
// Wanted to change the type of xclp, yclp to avoid overflows!
// But that changes the type for the drawing routines too!
//--------------------------------------------------------------------------

void
plP_pllclp( PLINT *x, PLINT *y, PLINT npts,
            PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax,
            void ( *draw )( short *, short *, PLINT ) )
{
    PLINT x1, x2, y1, y2;
    PLINT i, iclp = 0;

    short _xclp[PL_MAXPOLY], _yclp[PL_MAXPOLY];
    short *xclp = NULL, *yclp = NULL;
    int   drawable;

    if ( npts < PL_MAXPOLY )
    {
        xclp = _xclp;
        yclp = _yclp;
    }
    else
    {
        if ( ( ( xclp = (short *) malloc( (size_t) npts * sizeof ( short ) ) ) == NULL ) ||
             ( ( yclp = (short *) malloc( (size_t) npts * sizeof ( short ) ) ) == NULL ) )
        {
            plexit( "plP_pllclp: Insufficient memory" );
        }
    }

    for ( i = 0; i < npts - 1; i++ )
    {
        x1 = x[i];
        x2 = x[i + 1];
        y1 = y[i];
        y2 = y[i + 1];

        drawable = ( INSIDE( x1, y1 ) && INSIDE( x2, y2 ) );
        if ( !drawable )
            drawable = !plP_clipline( &x1, &y1, &x2, &y2,
                xmin, xmax, ymin, ymax );

        if ( drawable )
        {
// First point of polyline.

            if ( iclp == 0 )
            {
                xclp[iclp] = (short) x1;
                yclp[iclp] = (short) y1;
                iclp++;
                xclp[iclp] = (short) x2;
                yclp[iclp] = (short) y2;
            }

// Not first point.  Check if first point of this segment matches up to
// previous point, and if so, add it to the current polyline buffer.

            else if ( x1 == xclp[iclp] && y1 == yclp[iclp] )
            {
                iclp++;
                xclp[iclp] = (short) x2;
                yclp[iclp] = (short) y2;
            }

// Otherwise it's time to start a new polyline

            else
            {
                if ( iclp + 1 >= 2 )
                    ( *draw )( xclp, yclp, iclp + 1 );
                iclp       = 0;
                xclp[iclp] = (short) x1;
                yclp[iclp] = (short) y1;
                iclp++;
                xclp[iclp] = (short) x2;
                yclp[iclp] = (short) y2;
            }
        }
    }

// Handle remaining polyline

    if ( iclp + 1 >= 2 )
        ( *draw )( xclp, yclp, iclp + 1 );

    plsc->currx = x[npts - 1];
    plsc->curry = y[npts - 1];

    if ( xclp != _xclp )
    {
        free( xclp );
        free( yclp );
    }
}

//--------------------------------------------------------------------------
// int plP_clipline()
//
// Get clipped endpoints
//--------------------------------------------------------------------------

int
plP_clipline( PLINT *p_x1, PLINT *p_y1, PLINT *p_x2, PLINT *p_y2,
              PLINT xmin, PLINT xmax, PLINT ymin, PLINT ymax )
{
    PLINT  t, dx, dy, flipx, flipy;
    double dydx = 0, dxdy = 0;

// If both points are outside clip region with no hope of intersection,
// return with an error

    if ( ( *p_x1 <= xmin && *p_x2 <= xmin ) ||
         ( *p_x1 >= xmax && *p_x2 >= xmax ) ||
         ( *p_y1 <= ymin && *p_y2 <= ymin ) ||
         ( *p_y1 >= ymax && *p_y2 >= ymax ) )
        return 1;

// If one of the coordinates is not finite then return with an error
    if ( ( *p_x1 == PLINT_MIN ) || ( *p_y1 == PLINT_MIN ) ||
         ( *p_x2 == PLINT_MIN ) || ( *p_y2 == PLINT_MIN ) )
        return 1;

    flipx = 0;
    flipy = 0;

    if ( *p_x2 < *p_x1 )
    {
        *p_x1 = 2 * xmin - *p_x1;
        *p_x2 = 2 * xmin - *p_x2;
        xmax  = 2 * xmin - xmax;
        t     = xmax;
        xmax  = xmin;
        xmin  = t;
        flipx = 1;
    }

    if ( *p_y2 < *p_y1 )
    {
        *p_y1 = 2 * ymin - *p_y1;
        *p_y2 = 2 * ymin - *p_y2;
        ymax  = 2 * ymin - ymax;
        t     = ymax;
        ymax  = ymin;
        ymin  = t;
        flipy = 1;
    }

    dx = *p_x2 - *p_x1;
    dy = *p_y2 - *p_y1;

    if ( dx != 0 && dy != 0 )
    {
        dydx = (double) dy / (double) dx;
        dxdy = 1. / dydx;
    }

    if ( *p_x1 < xmin )
    {
        if ( dx != 0 && dy != 0 )
            *p_y1 = *p_y1 + ROUND( ( xmin - *p_x1 ) * dydx );
        *p_x1 = xmin;
    }

    if ( *p_y1 < ymin )
    {
        if ( dx != 0 && dy != 0 )
            *p_x1 = *p_x1 + ROUND( ( ymin - *p_y1 ) * dxdy );
        *p_y1 = ymin;
    }

    if ( *p_x1 >= xmax || *p_y1 >= ymax )
        return 1;

    if ( *p_y2 > ymax )
    {
        if ( dx != 0 && dy != 0 )
            *p_x2 = *p_x2 - ROUND( ( *p_y2 - ymax ) * dxdy );
        *p_y2 = ymax;
    }

    if ( *p_x2 > xmax )
    {
        if ( dx != 0 && dy != 0 )
            *p_y2 = *p_y2 - ROUND( ( *p_x2 - xmax ) * dydx );
        *p_x2 = xmax;
    }

    if ( flipx )
    {
        *p_x1 = 2 * xmax - *p_x1;
        *p_x2 = 2 * xmax - *p_x2;
    }

    if ( flipy )
    {
        *p_y1 = 2 * ymax - *p_y1;
        *p_y2 = 2 * ymax - *p_y2;
    }

    return 0;
}

//--------------------------------------------------------------------------
// void genlin()
//
// General line-drawing routine.  Takes line styles into account.
// If only 2 points are in the polyline, it is more efficient to use
// plP_line() rather than plP_polyline().
//--------------------------------------------------------------------------

static void
genlin( short *x, short *y, PLINT npts )
{
// Check for solid line

    if ( plsc->nms == 0 )
    {
        if ( npts == 2 )
            plP_line( x, y );
        else
            plP_polyline( x, y, npts );
    }

// Right now dashed lines don't use polyline capability -- this
// should be improved

    else
    {
        PLINT i;

        // Call escape sequence to draw dashed lines, only for drivers
        // that have this capability
        if ( plsc->dev_dash )
        {
            plsc->dev_npts = npts;
            plsc->dev_x    = x;
            plsc->dev_y    = y;
            plP_esc( PLESC_DASH, NULL );
            return;
        }

        for ( i = 0; i < npts - 1; i++ )
        {
            grdashline( x + i, y + i );
        }
    }
}

//--------------------------------------------------------------------------
// void grdashline()
//
// Draws a dashed line to the specified point from the previous one.
//--------------------------------------------------------------------------

static void
grdashline( short *x, short *y )
{
    PLINT  nx, ny, nxp, nyp, incr, temp;
    PLINT  modulo, dx, dy, i, xtmp, ytmp;
    PLINT  tstep, pix_distance, j;
    int    loop_x;
    short  xl[2], yl[2];
    double nxstep, nystep;

// Check if pattern needs to be restarted

    if ( x[0] != lastx || y[0] != lasty )
    {
        plsc->curel   = 0;
        plsc->pendn   = 1;
        plsc->timecnt = 0;
        plsc->alarm   = plsc->mark[0];
    }

    lastx = xtmp = x[0];
    lasty = ytmp = y[0];

    if ( x[0] == x[1] && y[0] == y[1] )
        return;

    nx  = x[1] - x[0];
    dx  = ( nx > 0 ) ? 1 : -1;
    nxp = ABS( nx );

    ny  = y[1] - y[0];
    dy  = ( ny > 0 ) ? 1 : -1;
    nyp = ABS( ny );

    if ( nyp > nxp )
    {
        modulo = nyp;
        incr   = nxp;
        loop_x = 0;
    }
    else
    {
        modulo = nxp;
        incr   = nyp;
        loop_x = 1;
    }

    temp = modulo / 2;

// Compute the timer step

    nxstep = nxp * plsc->umx;
    nystep = nyp * plsc->umy;
    tstep  = (PLINT) ( sqrt( nxstep * nxstep + nystep * nystep ) / modulo );
    if ( tstep < 1 )
        tstep = 1;

    // tstep is distance per pixel moved

    i = 0;
    while ( i < modulo )
    {
        pix_distance = ( plsc->alarm - plsc->timecnt + tstep - 1 ) / tstep;
        i           += pix_distance;
        if ( i > modulo )
            pix_distance -= ( i - modulo );
        plsc->timecnt += pix_distance * tstep;

        temp += pix_distance * incr;
        j     = temp / modulo;
        temp  = temp % modulo;

        if ( loop_x )
        {
            xtmp += pix_distance * dx;
            ytmp += j * dy;
        }
        else
        {
            xtmp += j * dx;
            ytmp += pix_distance * dy;
        }
        if ( plsc->pendn != 0 )
        {
            xl[0] = (short) lastx;
            yl[0] = (short) lasty;
            xl[1] = (short) xtmp;
            yl[1] = (short) ytmp;
            plP_line( xl, yl );
        }

// Update line style variables when alarm goes off

        while ( plsc->timecnt >= plsc->alarm )
        {
            if ( plsc->pendn != 0 )
            {
                plsc->pendn    = 0;
                plsc->timecnt -= plsc->alarm;
                plsc->alarm    = plsc->space[plsc->curel];
            }
            else
            {
                plsc->pendn    = 1;
                plsc->timecnt -= plsc->alarm;
                plsc->curel++;
                if ( plsc->curel >= plsc->nms )
                    plsc->curel = 0;
                plsc->alarm = plsc->mark[plsc->curel];
            }
        }
        lastx = xtmp;
        lasty = ytmp;
    }
}

//--------------------------------------------------------------------------
// interpolate_between()
//
// Returns a pointer to an array of PLFLT values which interpolate in n steps
// from a to b.
// Note:
// The returned array is allocated by the function and needs to be freed by
// the function's caller.
// If the return value is NULL, the allocation failed and it is up to the
// caller to handle the error.
//--------------------------------------------------------------------------

PLFLT *interpolate_between( PLINT n, PLFLT a, PLFLT b )
{
    PLFLT *values;
    PLFLT step_size;
    int   i;

    if ( ( values = (PLFLT *) malloc( (size_t) n * sizeof ( PLFLT ) ) ) == NULL )
    {
        return NULL;
    }

    step_size = ( b - a ) / (PLFLT) ( n - 1 );
    for ( i = 0; i < n; i++ )
    {
        values[i] = a + step_size * (PLFLT) i;
    }

    return values;
}
