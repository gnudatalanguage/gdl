//      Routines for drawing error bars and tick marks.
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

#include "plplotP.h"

//--------------------------------------------------------------------------
// void plwxtik()
//
// Draws a tick parallel to x, using world coordinates
//--------------------------------------------------------------------------
void
plwxtik( PLFLT x, PLFLT y, PLBOOL minor, PLBOOL invert )
{
    PLINT length, below, above;
    PLFLT height;
    if ( minor )
    {
        // Minor tick
        height = plsc->minht;
    }
    else
    {
        // Major tick
        height = plsc->majht;
    }
    length = MAX( ROUND( height * plsc->ypmm ), 1 );

    if ( invert )
    {
        below = 0;
        above = length;
    }
    else
    {
        below = length;
        above = 0;
    }
    // Actually draw the tick
    plxtik( plP_wcpcx( x ), plP_wcpcy( y ), below, above );
}

//--------------------------------------------------------------------------
// void plwytik()
//
// Draws a tick parallel to y, using world coordinates
//--------------------------------------------------------------------------
void
plwytik( PLFLT x, PLFLT y, PLBOOL minor, PLBOOL invert )
{
    PLINT length, below, above;
    PLFLT height;
    if ( minor )
    {
        // Minor tick
        height = plsc->minht;
    }
    else
    {
        // Major tick
        height = plsc->majht;
    }
    length = MAX( ROUND( height * plsc->xpmm ), 1 );

    if ( invert )
    {
        below = 0;
        above = length;
    }
    else
    {
        below = length;
        above = 0;
    }
    // Actually draw the tick
    plytik( plP_wcpcx( x ), plP_wcpcy( y ), below, above );
}

//--------------------------------------------------------------------------
// void plxtik()
//
// Draws a tick parallel to x.
//--------------------------------------------------------------------------

void
plxtik( PLINT x, PLINT y, PLINT below, PLINT above )
{
    plP_movphy( x, y - below );
    plP_draphy( x, y + above );
}

//--------------------------------------------------------------------------
// void plytik()
//
// Draws a tick parallel to y.
//--------------------------------------------------------------------------

void
plytik( PLINT x, PLINT y, PLINT left, PLINT right )
{
    plP_movphy( x - left, y );
    plP_draphy( x + right, y );
}

//--------------------------------------------------------------------------
// void plstik()
//
// Draws a slanting tick at position (mx,my) (measured in mm) of
// vector length (dx,dy).
//--------------------------------------------------------------------------

void
plstik( PLFLT mx, PLFLT my, PLFLT dx, PLFLT dy )
{
    plP_movphy( plP_mmpcx( mx ), plP_mmpcy( my ) );
    plP_draphy( plP_mmpcx( (PLFLT) ( mx + dx ) ), plP_mmpcy( (PLFLT) ( my + dy ) ) );
}

//--------------------------------------------------------------------------
// void plerx1()
//
// Plot single horizontal error bar.
//--------------------------------------------------------------------------

static void
plerx1( PLFLT xmin, PLFLT xmax, PLFLT y )
{
    PLINT yminor;

    yminor = (PLINT) ( MAX( 1.0, plsc->minht * plsc->ypmm ) );
    plxtik( plP_wcpcx( xmin ), plP_wcpcy( y ), yminor, yminor );
    plP_movwor( xmin, y );
    plP_drawor( xmax, y );
    plxtik( plP_wcpcx( xmax ), plP_wcpcy( y ), yminor, yminor );
}

//--------------------------------------------------------------------------
// void plery1()
//
// Plot single vertical error bar.
//--------------------------------------------------------------------------

static void
plery1( PLFLT x, PLFLT ymin, PLFLT ymax )
{
    PLINT xminor;

    xminor = (PLINT) ( MAX( 1.0, plsc->minht * plsc->xpmm ) );
    plytik( plP_wcpcx( x ), plP_wcpcy( ymin ), xminor, xminor );
    plP_movwor( x, ymin );
    plP_drawor( x, ymax );
    plytik( plP_wcpcx( x ), plP_wcpcy( ymax ), xminor, xminor );
}

//--------------------------------------------------------------------------
// void plerrx()
//
// Plot horizontal error bars (xmin(i),y(i)) to (xmax(i),y(i)).
//--------------------------------------------------------------------------

void
c_plerrx( PLINT n, PLFLT_VECTOR xmin, PLFLT_VECTOR xmax, PLFLT_VECTOR y )
{
    PLINT i;

    if ( plsc->level < 3 )
    {
        plabort( "plerrx: Please set up window first" );
        return;
    }

    for ( i = 0; i < n; i++ )
        plerx1( xmin[i], xmax[i], y[i] );
}

//--------------------------------------------------------------------------
// void plerry()
//
// Plot vertical error bars (x,ymin(i)) to (x(i),ymax(i)).
//--------------------------------------------------------------------------

void
c_plerry( PLINT n, PLFLT_VECTOR x, PLFLT_VECTOR ymin, PLFLT_VECTOR ymax )
{
    PLINT i;

    if ( plsc->level < 3 )
    {
        plabort( "plerry: Please set up window first" );
        return;
    }

    for ( i = 0; i < n; i++ )
        plery1( x[i], ymin[i], ymax[i] );
}
