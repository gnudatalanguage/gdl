// Plots a simple stripchart.
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
// ToDo: better way of clearing plot. search for `plvsta'.
//

#include "plplotP.h"

// Data declarations for stripcharts.

#define PEN    4

typedef struct
{
    PLFLT xmin, xmax, ymin, ymax, xjump, xlen;
    PLFLT wxmin, wxmax, wymin, wymax;   // FIXME - some redundancy might exist
    char  *xspec, *yspec, *labx, *laby, *labtop;
    PLINT y_ascl, acc, colbox, collab;
    PLFLT xlpos, ylpos;
    PLFLT *x[PEN], *y[PEN];
    PLINT npts[PEN], nptsmax[PEN];
    PLINT colline[PEN], styline[PEN];
    char  *legline[PEN];
} PLStrip;

static int     sid;                     // strip id number
#define MAX_STRIPC    1000              // Max allowed
static PLStrip *strip[MAX_STRIPC];      // Array of pointers
static PLStrip *stripc;                 // current strip chart

// Generates a complete stripchart plot.

static void
plstrip_gen( PLStrip *strip );

// draw legend

static void
plstrip_legend( PLStrip *strip, int flag );

//--------------------------------------------------------------------------
// plstripc
//
// Create 1d stripchart.
//--------------------------------------------------------------------------

void
c_plstripc( PLINT *id, PLCHAR_VECTOR xspec, PLCHAR_VECTOR yspec,
            PLFLT xmin, PLFLT xmax, PLFLT xjump, PLFLT ymin, PLFLT ymax,
            PLFLT xlpos, PLFLT ylpos,
            PLINT y_ascl, PLINT acc,
            PLINT colbox, PLINT collab,
            PLINT_VECTOR colline, PLINT_VECTOR styline, PLCHAR_MATRIX legline,
            PLCHAR_VECTOR labx, PLCHAR_VECTOR laby, PLCHAR_VECTOR labtop )
{
    int i;

// Get a free strip id and allocate it

    for ( i = 0; i < MAX_STRIPC; i++ )
        if ( strip[i] == NULL )
            break;

    if ( i == MAX_STRIPC )
    {
        plabort( "plstripc: Cannot create new strip chart" );
        *id = -1;
        return;
    }
    else
    {
        sid        = *id = i;
        strip[sid] = (PLStrip *) calloc( 1, (size_t) sizeof ( PLStrip ) );
        if ( strip[sid] == NULL )
        {
            plabort( "plstripc: Out of memory." );
            *id = -1;
            return;
        }
    }

// Fill up the struct with all relevant info

    stripc = strip[sid];

    for ( i = 0; i < PEN; i++ )
    {
        stripc->npts[i]    = 0;
        stripc->nptsmax[i] = 100;
        stripc->colline[i] = colline[i];
        stripc->styline[i] = styline[i];
        stripc->legline[i] = plstrdup( legline[i] );
        stripc->x[i]       = (PLFLT *) malloc( (size_t) sizeof ( PLFLT ) * (size_t) ( stripc->nptsmax[i] ) );
        stripc->y[i]       = (PLFLT *) malloc( (size_t) sizeof ( PLFLT ) * (size_t) ( stripc->nptsmax[i] ) );
        if ( stripc->x[i] == NULL || stripc->y[i] == NULL )
        {
            plabort( "plstripc: Out of memory." );
            plstripd( sid );
            *id = -1;
            return;
        }
    }

    stripc->xlpos  = xlpos;     // legend position [0..1]
    stripc->ylpos  = ylpos;
    stripc->xmin   = xmin;      // initial bounding box
    stripc->xmax   = xmax;
    stripc->ymin   = ymin;
    stripc->ymax   = ymax;
    stripc->xjump  = xjump;              // jump x step(%) when x attains xmax (xmax is then set to xmax+xjump)
    stripc->xlen   = xmax - xmin;        // length of x scale
    stripc->y_ascl = y_ascl;             // autoscale y between x jump scale
    stripc->acc    = acc;                // accumulate plot (not really stripchart)
    stripc->xspec  = plstrdup( xspec );  // x axis specification
    stripc->yspec  = plstrdup( yspec );
    stripc->labx   = plstrdup( labx );   // x label
    stripc->laby   = plstrdup( laby );
    stripc->labtop = plstrdup( labtop ); // title
    stripc->colbox = colbox;             // box color
    stripc->collab = collab;             // label color

// Generate the plot

    plstrip_gen( stripc );
    plstrip_legend( stripc, 1 );
}

static void plstrip_legend( PLStrip *stripcloc, int first )
{
    int   i;
    PLFLT sc, dy;

// draw legend

    plgchr( &sc, &dy );
    sc = dy = dy / 100;
    plwind( -0.01, 1.01, -0.01, 1.01 );
    for ( i = 0; i < PEN; i++ )
    {
        if ( stripcloc->npts[i] || first )
        {
            plcol0( stripcloc->colline[i] ); pllsty( stripcloc->styline[i] );
            pljoin( stripcloc->xlpos, stripcloc->ylpos - sc, stripcloc->xlpos + 0.1, stripcloc->ylpos - sc );
            plcol0( stripcloc->collab );
            plptex( stripcloc->xlpos + 0.11, stripcloc->ylpos - sc, 0., 0., 0, stripcloc->legline[i] ); sc += dy;
        }
    }
    plwind( stripcloc->xmin, stripcloc->xmax, stripcloc->ymin, stripcloc->ymax );
    plflush();
}

//--------------------------------------------------------------------------
// plstrip_gen
//
// Generates a complete stripchart plot.  Used either initially or
// during rescaling.
//--------------------------------------------------------------------------

static void plstrip_gen( PLStrip *striploc )
{
    int i;

// Set up window

    plvpor( 0, 1, 0, 1 );
    plwind( 0, 1, 0, 1 );
    plcol0( 0 ); plpsty( 0 );
    plclear();
    plvsta();

// Draw box and same window dimensions
    striploc->wxmin = striploc->xmin; striploc->wxmax = striploc->xmax;
    striploc->wymin = striploc->ymin; striploc->wymax = striploc->ymax; // FIXME - can exist some redundancy here

    plwind( striploc->xmin, striploc->xmax, striploc->ymin, striploc->ymax );

    pllsty( 1 );
    plcol0( striploc->colbox );
    plbox( striploc->xspec, 0.0, 0, striploc->yspec, 0.0, 0 );

    plcol0( striploc->collab );
    pllab( striploc->labx, striploc->laby, striploc->labtop );

    for ( i = 0; i < PEN; i++ )
    {
        if ( striploc->npts[i] > 0 )
        {
            plcol0( striploc->colline[i] ); pllsty( striploc->styline[i] );
            plline( striploc->npts[i], striploc->x[i], striploc->y[i] );
        }
    }

    plstrip_legend( striploc, 0 );
}

//--------------------------------------------------------------------------
// plstripa
//
// Add a point to a stripchart.
// Allocates memory and rescales as necessary.
//--------------------------------------------------------------------------

void c_plstripa( PLINT id, PLINT p, PLFLT x, PLFLT y )
{
    int j, yasc = 0, istart;

    if ( p >= PEN )
    {
        plabort( "Non existent pen" );
        return;
    }

    if ( ( id < 0 ) || ( id >= MAX_STRIPC ) ||
         ( ( stripc = strip[id] ) == NULL ) )
    {
        plabort( "Non existent stripchart" );
        return;
    }

// Add new point, allocating memory if necessary

    if ( ++stripc->npts[p] > stripc->nptsmax[p] )
    {
        stripc->nptsmax[p] += 32;
        stripc->x[p]        = (PLFLT *) realloc( (void *) stripc->x[p], sizeof ( PLFLT ) * (size_t) ( stripc->nptsmax[p] ) );
        stripc->y[p]        = (PLFLT *) realloc( (void *) stripc->y[p], sizeof ( PLFLT ) * (size_t) ( stripc->nptsmax[p] ) );
        if ( stripc->x[p] == NULL || stripc->y[p] == NULL )
        {
            plabort( "plstripc: Out of memory." );
            plstripd( id );
            return;
        }
    }

    stripc->x[p][stripc->npts[p] - 1] = x;
    stripc->y[p][stripc->npts[p] - 1] = y;

    stripc->xmax = x;

    if ( stripc->y_ascl == 1 && ( y > stripc->ymax || y < stripc->ymin ) )
        yasc = 1;

    if ( y > stripc->ymax )
        stripc->ymax = stripc->ymin + 1.1 * ( y - stripc->ymin );
    if ( y < stripc->ymin )
        stripc->ymin = stripc->ymax - 1.1 * ( stripc->ymax - y );

// Now either plot new point or regenerate plot

    if ( stripc->xmax - stripc->xmin < stripc->xlen )
    {
        if ( yasc == 0 )
        {
            // If user has changed subwindow, make shure we have the correct one
            plvsta();
            plwind( stripc->wxmin, stripc->wxmax, stripc->wymin, stripc->wymax );   // FIXME - can exist some redundancy here
            plcol0( stripc->colline[p] ); pllsty( stripc->styline[p] );
            if ( ( stripc->npts[p] - 2 ) < 0 )
                plP_movwor( stripc->x[p][stripc->npts[p] - 1], stripc->y[p][stripc->npts[p] - 1] );
            else
                plP_movwor( stripc->x[p][stripc->npts[p] - 2], stripc->y[p][stripc->npts[p] - 2] );
            plP_drawor( stripc->x[p][stripc->npts[p] - 1], stripc->y[p][stripc->npts[p] - 1] );
            plflush();
        }
        else
        {
            stripc->xmax = stripc->xmin + stripc->xlen;
            plstrip_gen( stripc );
        }
    }
    else
    {
// Regenerating plot
        if ( stripc->acc == 0 )
        {
            for ( j = 0; j < PEN; j++ )
            {
                if ( stripc->npts[j] > 0 )
                {
                    istart = 0;
                    while ( stripc->x[j][istart] < stripc->xmin + stripc->xlen * stripc->xjump )
                        istart++;

                    stripc->npts[j] = stripc->npts[j] - istart;
                    memcpy( &stripc->x[j][0], &stripc->x[j][istart], (size_t) ( stripc->npts[j] ) * sizeof ( PLFLT ) );
                    memcpy( &stripc->y[j][0], &stripc->y[j][istart], (size_t) ( stripc->npts[j] ) * sizeof ( PLFLT ) );
                }
            }
        }
        else
            stripc->xlen = stripc->xlen * ( 1 + stripc->xjump );

        if ( stripc->acc == 0 )
            stripc->xmin = stripc->xmin + stripc->xlen * stripc->xjump;
        else
            stripc->xmin = stripc->x[p][0];
        stripc->xmax = stripc->xmax + stripc->xlen * stripc->xjump;

        plstrip_gen( stripc );
    }
}

//--------------------------------------------------------------------------
// plstripd
//
// Deletes and releases memory used by a stripchart.
//--------------------------------------------------------------------------

void c_plstripd( PLINT id )
{
    int i;

    if ( ( id < 0 ) || ( id >= MAX_STRIPC ) ||
         ( ( stripc = strip[id] ) == NULL ) )
    {
        plabort( "Non existent stripchart" );
        return;
    }

    for ( i = 0; i < PEN; i++ )
    {
        if ( stripc->npts[i] )
        {
            free( (void *) stripc->x[i] );
            free( (void *) stripc->y[i] );
            free( stripc->legline[i] );
        }
    }

    free( stripc->xspec );
    free( stripc->yspec );
    free( stripc->labx );
    free( stripc->laby );
    free( stripc->labtop );
    free( (void *) stripc );
    strip[id] = NULL;
}
