//! @file
//!
//! 3d plot routines.
//!
// Copyright (C) 2004-2014 Alan W. Irwin
// Copyright (C) 2004  Joao Cardoso
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

#include "plplotP.h"

// Internal constants

#define  BINC    50             // Block size for memory allocation

static PLINT pl3mode = 0;       // 0 3d solid; 1 mesh plot
static PLINT pl3upv  = 1;       // 1 update view; 0 no update

static PLINT zbflg = 0, zbcol;
static PLFLT zbtck, zbwidth;

static PLINT *oldhiview = NULL;
static PLINT *oldloview = NULL;
static PLINT *newhiview = NULL;
static PLINT *newloview = NULL;
static PLINT *utmp      = NULL;
static PLINT *vtmp      = NULL;
static PLFLT *ctmp      = NULL;

static PLINT mhi, xxhi, newhisize;
static PLINT mlo, xxlo, newlosize;

// Light source for shading
static PLFLT xlight, ylight, zlight;
static PLINT falsecolor = 0;
static PLFLT fc_minz, fc_maxz;

// Prototypes for static functions

static void plgrid3( PLFLT );
static void plnxtv( PLINT *, PLINT *, PLFLT*, PLINT, PLINT );
static void
plside3( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny, PLINT opt );
static void
plt3zz( PLINT x0, PLINT y0, PLINT dx, PLINT dy, PLINT flag, PLINT *init,
        PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
        PLINT *u, PLINT *v, PLFLT* c );
static void plnxtvhi( PLINT *, PLINT *, PLFLT*, PLINT, PLINT );
static void plnxtvlo( PLINT *, PLINT *, PLFLT*, PLINT, PLINT );
static void plnxtvhi_draw( PLINT *u, PLINT *v, PLFLT* c, PLINT n );

static void savehipoint( PLINT, PLINT );
static void savelopoint( PLINT, PLINT );
static void swaphiview( void );
static void swaploview( void );
static void myexit( PLCHAR_VECTOR );
static void myabort( PLCHAR_VECTOR );
static void freework( void );
static int  plabv( PLINT, PLINT, PLINT, PLINT, PLINT, PLINT );
static void pl3cut( PLINT, PLINT, PLINT, PLINT, PLINT,
                    PLINT, PLINT, PLINT, PLINT *, PLINT * );
static PLFLT plGetAngleToLight( PLFLT* x, PLFLT* y, PLFLT* z );
static void plP_draw3d( PLINT x, PLINT y, PLFLT *c, PLINT j, PLINT move );
//static void plxyindexlimits( PLINT instart, PLINT inn,
//                             PLINT *inarray_min, PLINT *inarray_max,
//                             PLINT *outstart, PLINT *outn, PLINT outnmax,
//                             PLINT *outarray_min, PLINT *outarray_max );


// #define MJL_HACK 1
#if MJL_HACK
static void plP_fill3( PLINT x0, PLINT y0, PLINT x1, PLINT y1,
                       PLINT x2, PLINT y2, PLINT j );
static void plP_fill4( PLINT x0, PLINT y0, PLINT x1, PLINT y1,
                       PLINT x2, PLINT y2, PLINT x3, PLINT y3, PLINT j );
#endif

//--------------------------------------------------------------------------
// void plsetlightsource(x, y, z)
//
// Sets the position of the light source.
//--------------------------------------------------------------------------

void
c_pllightsource( PLFLT x, PLFLT y, PLFLT z )
{
    xlight = x;
    ylight = y;
    zlight = z;
}

//--------------------------------------------------------------------------
// void plmesh(x, y, z, nx, ny, opt)
//
// Plots a mesh representation of the function z[x][y]. The x values
// are stored as x[0..nx-1], the y values as y[0..ny-1], and the
// z values are in the 2-d array z[][]. The integer "opt" specifies:
// see plmeshc() below.
//--------------------------------------------------------------------------

void
c_plmesh( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny, PLINT opt )
{
    plfplot3dc( x, y, plf2ops_c(), (PLPointer) z, nx, ny, opt | MESH, NULL, 0 );
}

void
plfmesh( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
         PLINT nx, PLINT ny, PLINT opt )
{
    plfplot3dc( x, y, zops, zp, nx, ny, opt | MESH, NULL, 0 );
}

//--------------------------------------------------------------------------
// void plmeshc(x, y, z, nx, ny, opt, clevel, nlevel)
//
// Plots a mesh representation of the function z[x][y]. The x values
// are stored as x[0..nx-1], the y values as y[0..ny-1], and the
// z values are in the 2-d array z[][]. The integer "opt" specifies:
//
// DRAW_LINEX   draw lines parallel to the X axis
// DRAW_LINEY   draw lines parallel to the Y axis
// DRAW_LINEXY  draw lines parallel to both the X and Y axis
// MAG_COLOR    draw the mesh with a color dependent of the magnitude
// BASE_CONT    draw contour plot at bottom xy plane
// TOP_CONT     draw contour plot at top xy plane (not yet)
// DRAW_SIDES   draw sides
//
// or any bitwise combination, e.g. "MAG_COLOR | DRAW_LINEX"
//
//--------------------------------------------------------------------------

void
c_plmeshc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny, PLINT opt,
           PLFLT_VECTOR clevel, PLINT nlevel )
{
    plfplot3dc( x, y, plf2ops_c(), (PLPointer) z, nx, ny, opt | MESH, clevel, nlevel );
}

void
plfmeshc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
          PLINT nx, PLINT ny, PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel )
{
    plfplot3dc( x, y, zops, zp, nx, ny, opt | MESH, clevel, nlevel );
}

// clipping helper for 3d polygons

int
plP_clip_poly( int Ni, PLFLT *Vi[3], int axis, PLFLT dir, PLFLT offset )
{
    int   anyout = 0;
    PLFLT _in[PL_MAXPOLY], _T[3][PL_MAXPOLY];
    PLFLT *in, *T[3], *TT = NULL;
    int   No = 0;
    int   i, j, k;

    if ( Ni > PL_MAXPOLY )
    {
        in = (PLFLT *) malloc( sizeof ( PLFLT ) * (size_t) Ni );
        TT = (PLFLT *) malloc( 3 * sizeof ( PLFLT ) * (size_t) Ni );

        if ( in == NULL || TT == NULL )
        {
            plexit( "plP_clip_poly: insufficient memory for large polygon" );
        }

        T[0] = &TT[0];
        T[1] = &TT[Ni];
        T[2] = &TT[2 * Ni];
    }
    else
    {
        in   = _in;
        T[0] = &_T[0][0];
        T[1] = &_T[1][0];
        T[2] = &_T[2][0];
    }

    for ( i = 0; i < Ni; i++ )
    {
        in[i]   = Vi[axis][i] * dir + offset;
        anyout += in[i] < 0;
    }

    // none out
    if ( anyout == 0 )
        return Ni;

    // all out
    if ( anyout == Ni )
    {
        return 0;
    }

    // some out
    // copy over to a temp vector
    for ( i = 0; i < 3; i++ )
    {
        for ( j = 0; j < Ni; j++ )
        {
            T[i][j] = Vi[i][j];
        }
    }
    // copy back selectively
    for ( i = 0; i < Ni; i++ )
    {
        j = ( i + 1 ) % Ni;

        if ( in[i] >= 0 && in[j] >= 0 )
        {
            for ( k = 0; k < 3; k++ )
                Vi[k][No] = T[k][j];
            No++;
        }
        else if ( in[i] >= 0 && in[j] < 0 )
        {
            PLFLT u = in[i] / ( in[i] - in[j] );
            for ( k = 0; k < 3; k++ )
                Vi[k][No] = T[k][i] * ( 1 - u ) + T[k][j] * u;
            No++;
        }
        else if ( in[i] < 0 && in[j] >= 0 )
        {
            PLFLT u = in[i] / ( in[i] - in[j] );
            for ( k = 0; k < 3; k++ )
                Vi[k][No] = T[k][i] * ( 1 - u ) + T[k][j] * u;
            No++;
            for ( k = 0; k < 3; k++ )
                Vi[k][No] = T[k][j];
            No++;
        }
    }

    if ( Ni > PL_MAXPOLY )
    {
        free( in );
        free( TT );
    }

    return No;
}

// helper for plsurf3d, similar to c_plfill3()
static void
shade_triangle( PLFLT x0, PLFLT y0, PLFLT z0,
                PLFLT x1, PLFLT y1, PLFLT z1,
                PLFLT x2, PLFLT y2, PLFLT z2 )
{
    int   i;
    // arrays for interface to core functions
    short u[6], v[6];
    PLFLT x[6], y[6], z[6];
    int   n;
    PLFLT xmin, xmax, ymin, ymax, zmin, zmax, zscale;
    PLFLT *V[3];

    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );

    x[0] = x0; x[1] = x1; x[2] = x2;
    y[0] = y0; y[1] = y1; y[2] = y2;
    z[0] = z0; z[1] = z1; z[2] = z2;
    n    = 3;

    V[0] = x; V[1] = y; V[2] = z;

    n = plP_clip_poly( n, V, 0, 1, -xmin );
    n = plP_clip_poly( n, V, 0, -1, xmax );
    n = plP_clip_poly( n, V, 1, 1, -ymin );
    n = plP_clip_poly( n, V, 1, -1, ymax );
    n = plP_clip_poly( n, V, 2, 1, -zmin );
    n = plP_clip_poly( n, V, 2, -1, zmax );

    if ( n > 0 )
    {
        if ( falsecolor )
            plcol1( ( ( z[0] + z[1] + z[2] ) / 3. - fc_minz ) / ( fc_maxz - fc_minz ) );
        else
            plcol1( plGetAngleToLight( x, y, z ) );

        for ( i = 0; i < n; i++ )
        {
            u[i] = (short) plP_wcpcx( plP_w3wcx( x[i], y[i], z[i] ) );
            v[i] = (short) plP_wcpcy( plP_w3wcy( x[i], y[i], z[i] ) );
        }
        u[n] = u[0];
        v[n] = v[0];

#ifdef SHADE_DEBUG // show triangles
        plcol0( 1 );
        x[3] = x[0]; y[3] = y[0]; z[3] = z[0];
        plline3( 4, x, y, z );
#else   // fill triangles
        plP_fill( u, v, n + 1 );
#endif
    }
}

//--------------------------------------------------------------------------
// void plsurf3d(x, y, z, nx, ny, opt, clevel, nlevel)
//
// Plots the 3-d surface representation of the function z[x][y].
// The x values are stored as x[0..nx-1], the y values as y[0..ny-1],
//  and the z values are in the 2-d array z[][].  The integer "opt" specifies:
// see plsurf3dl() below.
//--------------------------------------------------------------------------

void
c_plsurf3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny,
            PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel )
{
    plfsurf3d( x, y, plf2ops_c(), (PLPointer) z, nx, ny,
        opt, clevel, nlevel );
}

void
plfsurf3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
           PLINT nx, PLINT ny, PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel )
{
    PLINT i;
    PLINT *indexymin = (PLINT *) malloc( (size_t) nx * sizeof ( PLINT ) );
    PLINT *indexymax = (PLINT *) malloc( (size_t) nx * sizeof ( PLINT ) );

    if ( !indexymin || !indexymax )
        plexit( "plsurf3d: Out of memory." );
    for ( i = 0; i < nx; i++ )
    {
        indexymin[i] = 0;
        indexymax[i] = ny;
    }
    plfsurf3dl( x, y, zops, zp, nx, ny, opt, clevel, nlevel,
        0, nx, indexymin, indexymax );
    free_mem( indexymin );
    free_mem( indexymax );
}

//--------------------------------------------------------------------------
// void plsurf3dl(x, y, z, nx, ny, opt, clevel, nlevel,
// indexxmin, indexxmax, indexymin, indexymax)
//
// Plots the 3-d surface representation of the function z[x][y].
// The x values are stored as x[0..nx-1], the y values as y[0..ny-1],
//  and the z values are in the 2-d array z[][].
//
//
// BASE_CONT    draw contour plot at bottom xy plane
// TOP_CONT     draw contour plot at top xy plane (not implemented)
// SURF_CONT    draw contour at surface
// FACETED      each square that makes up the surface is faceted
// DRAW_SIDES   draw sides
// MAG_COLOR    the surface is colored according to the value of z;
//               if not defined, the surface is colored according to the
//               intensity of the reflected light in the surface from a light
//               source whose position is set using c_pllightsource()
//
// or any bitwise combination, e.g. "MAG_COLOR | SURF_CONT | BASE_CONT"
//
// indexymin and indexymax are arrays which specify the y index range
// (following the convention that the upper range limit is one more than
// actual index limit) for an x index range of indexxmin, indexxmax.
// This code is a complete departure from the approach taken in the old version
// of this routine. Formerly to code attempted to use the logic for the hidden
// line algorithm to draw the hidden surface. This was really hard. This code
// below uses a simple back to front (painters) algorithm. All the
// triangles are drawn.
//
// There are multitude of ways this code could be optimized. Given the
// problems with the old code, I tried to focus on clarity here.
//--------------------------------------------------------------------------

void
c_plsurf3dl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z, PLINT nx, PLINT ny,
             PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel,
             PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax )
{
    plfsurf3dl( x, y, plf2ops_c(), (PLPointer) z, nx, ny,
        opt, clevel, nlevel, indexxmin, indexxmax, indexymin, indexymax );
}

void
plfsurf3dl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
            PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel,
            PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax )
{
    PLFLT      cxx, cxy, cyx, cyy, cyz;
    PLINT      i, j, k;
    PLINT      ixDir, ixOrigin, iyDir, iyOrigin, nFast, nSlow;
    PLINT      ixFast, ixSlow, iyFast, iySlow;
    PLINT      iFast, iSlow;
    PLFLT      xmin, xmax, ymin, ymax, zmin, zmax, zscale;
    PLFLT      xm, ym, zm;
    PLINT      ixmin = 0, ixmax = nx, iymin = 0, iymax = ny;
    PLFLT      xx[3], yy[3], zz[3];
    PLFLT      px[4], py[4], pz[4];
    CONT_LEVEL *cont, *clev;
    CONT_LINE  *cline;
    int        ct, ix, iy, iftriangle;
    PLINT      color = plsc->icol0;
    PLFLT      width = plsc->width;
    PLFLT      ( *getz )( PLPointer, PLINT, PLINT ) = zops->get;

    if ( plsc->level < 3 )
    {
        myabort( "plsurf3dl: Please set up window first" );
        return;
    }

    if ( nx <= 0 || ny <= 0 )
    {
        myabort( "plsurf3dl: Bad array dimensions." );
        return;
    }

    //
    // Don't use the data z value to scale the color, use the z axis
    // values set by plw3d()
    //
    // plMinMax2dGrid(z, nx, ny, &fc_maxz, &fc_minz);
    //

    fc_minz = plsc->ranmi;
    fc_maxz = plsc->ranma;
    if ( fc_maxz == fc_minz )
    {
        plwarn( "plsurf3dl: Maximum and minimum Z values are equal! \"fixing\"..." );
        fc_maxz = fc_minz + 1e-6;
    }

    if ( opt & MAG_COLOR )
        falsecolor = 1;
    else
        falsecolor = 0;

    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );
    if ( zmin > zmax )
    {
        PLFLT t = zmin;
        zmin = zmax;
        zmax = t;
    }

    // Check that points in x and in y are strictly increasing  and in range

    for ( i = 0; i < nx - 1; i++ )
    {
        if ( x[i] >= x[i + 1] )
        {
            myabort( "plsurf3dl: X array must be strictly increasing" );
            return;
        }
        if ( x[i] < xmin && x[i + 1] >= xmin )
            ixmin = i;
        if ( x[i + 1] > xmax && x[i] <= xmax )
            ixmax = i + 2;
    }
    for ( i = 0; i < ny - 1; i++ )
    {
        if ( y[i] >= y[i + 1] )
        {
            myabort( "plsurf3dl: Y array must be strictly increasing" );
            return;
        }
        if ( y[i] < ymin && y[i + 1] >= ymin )
            iymin = i;
        if ( y[i + 1] > ymax && y[i] <= ymax )
            iymax = i + 2;
    }

    // get the viewing parameters
    plP_gw3wc( &cxx, &cxy, &cyx, &cyy, &cyz );

    // we're going to draw from back to front

    // iFast will index the dominant (fastest changing) dimension
    // iSlow will index the slower changing dimension
    //
    // iX indexes the X dimension
    // iY indexes the Y dimension

    // get direction for X
    if ( cxy >= 0 )
    {
        ixDir    = 1;     // direction in X
        ixOrigin = ixmin; // starting point
    }
    else
    {
        ixDir    = -1;
        ixOrigin = ixmax - 1;
    }
    // get direction for Y
    if ( cxx >= 0 )
    {
        iyDir    = -1;
        iyOrigin = iymax - 1;
    }
    else
    {
        iyDir    = 1;
        iyOrigin = iymin;
    }
    // figure out which dimension is dominant
    if ( fabs( cxx ) > fabs( cxy ) )
    {
        // X is dominant
        nFast = ixmax - ixmin;  // samples in the Fast direction
        nSlow = iymax - iymin;  // samples in the Slow direction

        ixFast = ixDir; ixSlow = 0;
        iyFast = 0;     iySlow = iyDir;
    }
    else
    {
        nFast = iymax - iymin;
        nSlow = ixmax - ixmin;

        ixFast = 0;     ixSlow = ixDir;
        iyFast = iyDir; iySlow = 0;
    }

    // we've got to draw the background grid first, hidden line code has to draw it last
    if ( zbflg )
    {
        PLFLT bx[3], by[3], bz[3];
        PLFLT tick = zbtck, tp;
        PLINT nsub = 0;

        // get the tick spacing
        pldtik( zmin, zmax, &tick, &nsub, FALSE );

        // determine the vertices for the background grid line
        bx[0] = ( ixOrigin != ixmin && ixFast == 0 ) || ixFast > 0 ? xmax : xmin;
        by[0] = ( iyOrigin != iymin && iyFast == 0 ) || iyFast > 0 ? ymax : ymin;
        bx[1] = ixOrigin != ixmin ? xmax : xmin;
        by[1] = iyOrigin != iymin ? ymax : ymin;
        bx[2] = ( ixOrigin != ixmin && ixSlow == 0 ) || ixSlow > 0 ? xmax : xmin;
        by[2] = ( iyOrigin != iymin && iySlow == 0 ) || iySlow > 0 ? ymax : ymin;

        plwidth( zbwidth );
        plcol0( zbcol );
        for ( tp = tick * floor( zmin / tick ) + tick; tp <= zmax; tp += tick )
        {
            bz[0] = bz[1] = bz[2] = tp;
            plline3( 3, bx, by, bz );
        }
        // draw the vertical line at the back corner
        bx[0] = bx[1];
        by[0] = by[1];
        bz[0] = zmin;
        plline3( 2, bx, by, bz );
        plwidth( width );
        plcol0( color );
    }

    // If enabled, draw the contour at the base

    // The contour plotted at the base will be identical to the one obtained
    // with c_plcont(). The contour plotted at the surface is simple minded, but
    // can be improved by using the contour data available.
    //

    if ( clevel != NULL && opt & BASE_CONT )
    {
#define NPTS    100
        int      np = NPTS;
        PLFLT    **zstore;
        PLcGrid2 cgrid2;
        PLFLT    *zzloc = (PLFLT *) malloc( (size_t) NPTS * sizeof ( PLFLT ) );
        if ( zzloc == NULL )
            plexit( "plsurf3dl: Insufficient memory" );

        // get the contour lines

        // prepare cont_store input
        cgrid2.nx = nx;
        cgrid2.ny = ny;
        plAlloc2dGrid( &cgrid2.xg, nx, ny );
        plAlloc2dGrid( &cgrid2.yg, nx, ny );
        plAlloc2dGrid( &zstore, nx, ny );

        for ( i = indexxmin; i < indexxmax; i++ )
        {
            for ( j = 0; j < indexymin[i]; j++ )
            {
                cgrid2.xg[i][j] = x[i];
                cgrid2.yg[i][j] = y[indexymin[i]];
                zstore[i][j]    = getz( zp, i, indexymin[i] );
            }
            for ( j = indexymin[i]; j < indexymax[i]; j++ )
            {
                cgrid2.xg[i][j] = x[i];
                cgrid2.yg[i][j] = y[j];
                zstore[i][j]    = getz( zp, i, j );
            }
            for ( j = indexymax[i]; j < ny; j++ )
            {
                cgrid2.xg[i][j] = x[i];
                cgrid2.yg[i][j] = y[indexymax[i] - 1];
                zstore[i][j]    = getz( zp, i, indexymax[i] - 1 );
            }
        }
        // Fill cont structure with contours.
        cont_store( (PLFLT_MATRIX) zstore, nx, ny, indexxmin + 1, indexxmax, 1, ny,
            clevel, nlevel, pltr2, (void *) &cgrid2, &cont );

        // Free the 2D input arrays to cont_store since not needed any more.
        plFree2dGrid( zstore, nx, ny );
        plFree2dGrid( cgrid2.xg, nx, ny );
        plFree2dGrid( cgrid2.yg, nx, ny );

        // follow the contour levels and lines
        clev = cont;
        do  // for each contour level
        {
            cline = clev->line;
            do  // there are several lines that make up the contour
            {
                if ( cline->npts > np )
                {
                    np = cline->npts;
                    if ( ( zzloc = (PLFLT *) realloc( zzloc, (size_t) np * sizeof ( PLFLT ) ) ) == NULL )
                    {
                        plexit( "plsurf3dl: Insufficient memory" );
                    }
                }
                for ( j = 0; j < cline->npts; j++ )
                    zzloc[j] = plsc->ranmi;
                if ( cline->npts > 0 )
                {
                    plcol1( ( clev->level - fc_minz ) / ( fc_maxz - fc_minz ) );
                    plline3( cline->npts, cline->x, cline->y, zzloc );
                }
                cline = cline->next;
            }
            while ( cline != NULL );
            clev = clev->next;
        }
        while ( clev != NULL );

        cont_clean_store( cont ); // now release the memory
        free( zzloc );
    }

    // Now we can iterate over the grid drawing the quads
    for ( iSlow = 0; iSlow < nSlow - 1; iSlow++ )
    {
        for ( iFast = 0; iFast < nFast - 1; iFast++ )
        {
            // get the 4 corners of the Quad, which are
            //
            //       0--2
            //       |  |
            //       1--3
            //

            xm = ym = zm = 0.;

            iftriangle = 1;
            for ( i = 0; i < 2; i++ )
            {
                for ( j = 0; j < 2; j++ )
                {
                    // we're transforming from Fast/Slow coordinates to x/y coordinates
                    // note, these are the indices, not the values
                    ix = ixFast * ( iFast + i ) + ixSlow * ( iSlow + j ) + ixOrigin;
                    iy = iyFast * ( iFast + i ) + iySlow * ( iSlow + j ) + iyOrigin;

                    if ( indexxmin <= ix && ix < indexxmax &&
                         indexymin[ix] <= iy && iy < indexymax[ix] )
                    {
                        xm += px[2 * i + j] = x[ix];
                        ym += py[2 * i + j] = y[iy];
                        zm += pz[2 * i + j] = getz( zp, ix, iy );
                    }
                    else
                    {
                        iftriangle = 0;
                        break;
                    }
                }
                if ( iftriangle == 0 )
                    break;
            }

            if ( iftriangle == 0 )
                continue;
            // the "mean point" of the quad, common to all four triangles
            // -- perhaps not a good thing to do for the light shading

            xm /= 4.; ym /= 4.; zm /= 4.;

            // now draw the quad as four triangles

            for ( i = 1; i < 3; i++ )
            {
                for ( j = 0; j < 4; j += 3 )
                {
                    shade_triangle( px[j], py[j], pz[j], xm, ym, zm, px[i], py[i], pz[i] );
                }
            }

            // After shading completed for a quad, render surface contours.
            if ( clevel != NULL && ( opt & SURF_CONT ) )
            {
                for ( i = 1; i < 3; i++ )
                {
                    for ( j = 0; j < 4; j += 3 )
#define min3( a, b, c )    ( MIN( ( MIN( a, b ) ), c ) )
#define max3( a, b, c )    ( MAX( ( MAX( a, b ) ), c ) )

                    {
                        for ( k = 0; k < nlevel; k++ )
                        {
                            if ( clevel[k] >= min3( pz[i], zm, pz[j] ) && clevel[k] < max3( pz[i], zm, pz[j] ) )
                            {
                                ct = 0;
                                if ( clevel[k] >= MIN( pz[i], zm ) && clevel[k] < MAX( pz[i], zm ) )     // p0-pm
                                {
                                    xx[ct] = ( ( clevel[k] - pz[i] ) * ( xm - px[i] ) ) / ( zm - pz[i] ) + px[i];
                                    yy[ct] = ( ( clevel[k] - pz[i] ) * ( ym - py[i] ) ) / ( zm - pz[i] ) + py[i];
                                    ct++;
                                }

                                if ( clevel[k] >= MIN( pz[i], pz[j] ) && clevel[k] < MAX( pz[i], pz[j] ) )     // p0-p1
                                {
                                    xx[ct] = ( ( clevel[k] - pz[i] ) * ( px[j] - px[i] ) ) / ( pz[j] - pz[i] ) + px[i];
                                    yy[ct] = ( ( clevel[k] - pz[i] ) * ( py[j] - py[i] ) ) / ( pz[j] - pz[i] ) + py[i];
                                    ct++;
                                }

                                if ( clevel[k] >= MIN( pz[j], zm ) && clevel[k] < MAX( pz[j], zm ) )     // p1-pm
                                {
                                    xx[ct] = ( ( clevel[k] - pz[j] ) * ( xm - px[j] ) ) / ( zm - pz[j] ) + px[j];
                                    yy[ct] = ( ( clevel[k] - pz[j] ) * ( ym - py[j] ) ) / ( zm - pz[j] ) + py[j];
                                    ct++;
                                }

                                if ( ct == 2 )
                                {
                                    // yes, xx and yy are the intersection points of the triangle with
                                    // the contour line -- draw a straight line betweeen the points
                                    // -- at the end this will make up the contour line

                                    // surface contour with color set by user
                                    plcol0( color );
                                    zz[0] = zz[1] = clevel[k];
                                    plline3( 2, xx, yy, zz );

                                    // don't break; one triangle can span various contour levels
                                }
                                else
                                    plwarn( "plsurf3dl: ***ERROR***\n" );
                            }
                        }
                    }
                }
            }
        }
    }

    if ( opt & FACETED )
    {
        plcol0( 0 );
        plfplot3dcl( x, y, zops, zp, nx, ny, MESH | DRAW_LINEXY, NULL, 0,
            indexxmin, indexxmax, indexymin, indexymax );
    }

    if ( opt & DRAW_SIDES ) // the sides look ugly !!!
    {                       // draw one more row with all the Z's set to zmin
        plP_grange( &zscale, &zmin, &zmax );

        iSlow      = nSlow - 1;
        iftriangle = 1;
        for ( iFast = 0; iFast < nFast - 1; iFast++ )
        {
            for ( i = 0; i < 2; i++ )
            {
                ix = ixFast * ( iFast + i ) + ixSlow * iSlow + ixOrigin;
                iy = iyFast * ( iFast + i ) + iySlow * iSlow + iyOrigin;
                if ( indexxmin <= ix && ix < indexxmax &&
                     indexymin[ix] <= iy && iy < indexymax[ix] )
                {
                    px[2 * i] = x[ix];
                    py[2 * i] = y[iy];
                    pz[2 * i] = getz( zp, ix, iy );
                }
                else
                {
                    iftriangle = 0;
                    break;
                }
            }
            if ( iftriangle == 0 )
                break;
            // now draw the quad as two triangles (4 might be better)

            shade_triangle( px[0], py[0], pz[0], px[2], py[2], pz[2], px[0], py[0], zmin );
            shade_triangle( px[2], py[2], pz[2], px[2], py[2], zmin, px[0], py[0], zmin );
        }

        iFast      = nFast - 1;
        iftriangle = 1;
        for ( iSlow = 0; iSlow < nSlow - 1; iSlow++ )
        {
            for ( i = 0; i < 2; i++ )
            {
                ix = ixFast * iFast + ixSlow * ( iSlow + i ) + ixOrigin;
                iy = iyFast * iFast + iySlow * ( iSlow + i ) + iyOrigin;
                if ( indexxmin <= ix && ix < indexxmax &&
                     indexymin[ix] <= iy && iy < indexymax[ix] )
                {
                    px[2 * i] = x[ix];
                    py[2 * i] = y[iy];
                    pz[2 * i] = getz( zp, ix, iy );
                }
                else
                {
                    iftriangle = 0;
                    break;
                }
            }
            if ( iftriangle == 0 )
                break;

            // now draw the quad as two triangles (4 might be better)
            shade_triangle( px[0], py[0], pz[0], px[2], py[2], pz[2], px[0], py[0], zmin );
            shade_triangle( px[2], py[2], pz[2], px[2], py[2], zmin, px[0], py[0], zmin );
        }
    }
}

//--------------------------------------------------------------------------
// void plot3d(x, y, z, nx, ny, opt, side)
//
// Plots a 3-d representation of the function z[x][y]. The x values
// are stored as x[0..nx-1], the y values as y[0..ny-1], and the z
// values are in the 2-d array z[][]. The integer "opt" specifies:
// see plot3dcl() below
//--------------------------------------------------------------------------

void
c_plot3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z,
          PLINT nx, PLINT ny, PLINT opt, PLBOOL side )
{
    plfplot3dc( x, y, plf2ops_c(), (PLPointer) z, nx, ny, opt | ( side != 0 ? DRAW_SIDES : 0 ), NULL, 0 );
}

void
plfplot3d( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
           PLINT nx, PLINT ny, PLINT opt, PLBOOL side )
{
    plfplot3dc( x, y, zops, zp, nx, ny, opt | ( side != 0 ? DRAW_SIDES : 0 ), NULL, 0 );
}

//--------------------------------------------------------------------------
// void plot3dc(x, y, z, nx, ny, opt, clevel, nlevel)
//
// Plots a 3-d representation of the function z[x][y]. The x values
// are stored as x[0..nx-1], the y values as y[0..ny-1], and the z
// values are in the 2-d array z[][]. The integer "opt" specifies:
// see plot3dcl() below
//--------------------------------------------------------------------------

void
c_plot3dc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z,
           PLINT nx, PLINT ny, PLINT opt,
           PLFLT_VECTOR clevel, PLINT nlevel )
{
    plfplot3dcl( x, y, plf2ops_c(), (PLPointer) z, nx, ny, opt, clevel, nlevel, 0, 0, NULL, NULL );
}

void
plfplot3dc( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
            PLINT nx, PLINT ny, PLINT opt, PLFLT_VECTOR clevel, PLINT nlevel )
{
    plfplot3dcl( x, y, zops, zp, nx, ny, opt, clevel, nlevel, 0, 0, NULL, NULL );
}

//--------------------------------------------------------------------------
// void plot3dcl(x, y, z, nx, ny, opt, clevel, nlevel,
//       indexxmin, indexxmax, indexymin, indexymax)
//
// Plots a 3-d representation of the function z[x][y]. The x values
// are stored as x[0..nx-1], the y values as y[0..ny-1], and the z
// values are in the 2-d array z[][]. The integer "opt" specifies:
//
//  DRAW_LINEX :  Draw lines parallel to x-axis
//  DRAW_LINEY :  Draw lines parallel to y-axis
//  DRAW_LINEXY:  Draw lines parallel to both axes
//  MAG_COLOR:    Magnitude coloring of wire frame
//  BASE_CONT:    Draw contour at bottom xy plane
//  TOP_CONT:     Draw contour at top xy plane (not yet)
//  DRAW_SIDES:   Draw sides around the plot
//  MESH:       Draw the "under" side of the plot
//
// or any bitwise combination, e.g. "MAG_COLOR | DRAW_LINEX"
// indexymin and indexymax are arrays which specify the y index limits
// (following the convention that the upper range limit is one more than
// actual index limit) for an x index range of indexxmin, indexxmax.
//--------------------------------------------------------------------------

void
c_plot3dcl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLFLT_MATRIX z,
            PLINT nx, PLINT ny, PLINT opt,
            PLFLT_VECTOR clevel, PLINT nlevel,
            PLINT indexxmin, PLINT indexxmax, PLINT_VECTOR indexymin, PLINT_VECTOR indexymax )
{
    plfplot3dcl( x, y, plf2ops_c(), (PLPointer) z, nx, ny,
        opt, clevel, nlevel, indexxmin, indexxmax, indexymin, indexymax );
}

//--------------------------------------------------------------------------
//! Plots a 3-d representation of the virtual function z, which is represented
//! by zops and zp.
//!
//! @param x The x values are stored as x[0..nx-1]
//! @param y The y values are stored as y[0..ny-1]
//! @param zops Pointer to a plf2ops_t structure of function pointers (see
//! plf2opts_t in plplot.h) which define how to perform various manipulations
//! (including retrieval) on the elements of the the 2D data field pointed to
//! by zp.  Pointers suitable for passing as zops can be obtained for some
//! predefined types of 2-d data storage by calling one of the plf2ops_*()
//! functions (see plf2ops.c) or you can create your own set for arbitrary 2-d
//! storage formats.
//! @param zp Pointer to 2D data field.  This pointer is passed to the
//! functions of zops whenever the 2D field needs to be manipulated.  The
//! combination of zops and zp provides total flexibility in how the underlying
//! data values are managed.
//! @param nx The number of values in x.
//! @param ny The number of values in y.
//! @param opt Specifies options for the plot.  It can be a bitwise OR-ing of
//! these:
//!   - DRAW_LINEX :  Draw lines parallel to x-axis
//!   - DRAW_LINEY :  Draw lines parallel to y-axis
//!   - DRAW_LINEXY:  Draw lines parallel to both axes
//!   - MAG_COLOR:    Magnitude coloring of wire frame
//!   - BASE_CONT:    Draw contour at bottom xy plane
//!   - TOP_CONT:     Draw contour at top xy plane (not yet)
//!   - DRAW_SIDES:   Draw sides around the plot
//!   - MESH:         Draw the "under" side of the plot
//! or any bitwise OR'd combination, e.g. "MAG_COLOR | DRAW_LINEX"
//! @param clevel z values at which to draw contours
//! @param nlevel Number of values in clevels
//! @param PL_UNUSED( indexxmin ) Not used.
//! @param PL_UNUSED( indexxmax ) Not used.
//! @param PL_UNUSED( indexymin ) Not used.
//! @param PL_UNUSED( indexymax ) Not used.
//!
//--------------------------------------------------------------------------

void
plfplot3dcl( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp,
             PLINT nx, PLINT ny, PLINT opt,
             PLFLT_VECTOR clevel, PLINT nlevel,
             PLINT PL_UNUSED( indexxmin ), PLINT PL_UNUSED( indexxmax ), PLINT_VECTOR PL_UNUSED( indexymin ), PLINT_VECTOR PL_UNUSED( indexymax ) )
{
    PLFLT cxx, cxy, cyx, cyy, cyz;
    PLINT init, ix, iy, color;
    PLFLT width;
    PLFLT xmin, xmax, ymin, ymax, zmin, zmax, zscale;
    PLINT ixmin   = 0, ixmax = nx - 1, iymin = 0, iymax = ny - 1;
    PLINT clipped = 0, base_cont = 0, side = 0;
    PLFLT ( *getz )( PLPointer, PLINT, PLINT ) = zops->get;
    PLFLT *_x = NULL, *_y = NULL, **_z = NULL;
    PLFLT_VECTOR x_modified, y_modified;
    int i;

    pl3mode = 0;

    if ( plsc->level < 3 )
    {
        myabort( "plot3dcl: Please set up window first" );
        return;
    }

    if ( ( opt & 3 ) == 0 )
    {
        myabort( "plot3dcl: Bad option" );
        return;
    }

    if ( nx <= 0 || ny <= 0 )
    {
        myabort( "plot3dcl: Bad array dimensions." );
        return;
    }

    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );
    if ( zmin > zmax )
    {
        PLFLT t = zmin;
        zmin = zmax;
        zmax = t;
    }

// Check that points in x and in y are strictly increasing

    for ( i = 0; i < nx - 1; i++ )
    {
        if ( x[i] >= x[i + 1] )
        {
            myabort( "plot3dcl: X array must be strictly increasing" );
            return;
        }
    }
    for ( i = 0; i < ny - 1; i++ )
    {
        if ( y[i] >= y[i + 1] )
        {
            myabort( "plot3dcl: Y array must be strictly increasing" );
            return;
        }
    }

    if ( opt & MESH )
        pl3mode = 1;

    if ( opt & DRAW_SIDES )
        side = 1;

    // figure out the part of the data to use
    if ( xmin < x[0] )
        xmin = x[0];
    if ( xmax > x[nx - 1] )
        xmax = x[nx - 1];
    if ( ymin < y[0] )
        ymin = y[0];
    if ( ymax > y[ny - 1] )
        ymax = y[ny - 1];
    for ( ixmin = 0; ixmin < nx - 1 && x[ixmin + 1] < xmin; ixmin++ )
    {
    }
    for ( ixmax = nx - 1; ixmax > 0 && x[ixmax - 1] > xmax; ixmax-- )
    {
    }
    for ( iymin = 0; iymin < ny - 1 && y[iymin + 1] < ymin; iymin++ )
    {
    }
    for ( iymax = ny - 1; iymax > 0 && y[iymax - 1] > ymax; iymax-- )
    {
    }
    //fprintf(stderr, "(%d,%d) %d %d %d %d\n", nx, ny, ixmin, ixmax, iymin, iymax);
    // do we need to clip?
    if ( ixmin > 0 || ixmax < nx - 1 || iymin > 0 || iymax < ny - 1 )
    {
        // adjust the input so it stays within bounds
        int _nx = ixmax - ixmin + 1;
        int _ny = iymax - iymin + 1;
        PLFLT ty0, ty1, tx0, tx1;
        int j;

        if ( _nx <= 1 || _ny <= 1 )
        {
            myabort( "plot3dcl: selected x or y range has no data" );
            return;
        }

        // allocate storage for new versions of the input vectors
        if ( ( ( _x = (PLFLT *) malloc( (size_t) _nx * sizeof ( PLFLT ) ) ) == NULL ) ||
             ( ( _y = (PLFLT *) malloc( (size_t) _ny * sizeof ( PLFLT ) ) ) == NULL ) ||
             ( ( _z = (PLFLT **) malloc( (size_t) _nx * sizeof ( PLFLT* ) ) ) == NULL ) )
        {
            plexit( "c_plot3dcl: Insufficient memory" );
        }

        clipped = 1;

        // copy over the independent variables
        _x[0]       = xmin;
        _x[_nx - 1] = xmax;
        for ( i = 1; i < _nx - 1; i++ )
            _x[i] = x[ixmin + i];
        _y[0]       = ymin;
        _y[_ny - 1] = ymax;
        for ( i = 1; i < _ny - 1; i++ )
            _y[i] = y[iymin + i];

        // copy the data array so we can interpolate around the edges
        for ( i = 0; i < _nx; i++ )
        {
            if ( ( _z[i] = (PLFLT *) malloc( (size_t) _ny * sizeof ( PLFLT ) ) ) == NULL )
            {
                plexit( "c_plot3dcl: Insufficient memory" );
            }
        }

        // interpolation factors for the 4 edges
        ty0 = ( _y[0] - y[iymin] ) / ( y[iymin + 1] - y[iymin] );
        ty1 = ( _y[_ny - 1] - y[iymax - 1] ) / ( y[iymax] - y[iymax - 1] );
        tx0 = ( _x[0] - x[ixmin] ) / ( x[ixmin + 1] - x[ixmin] );
        tx1 = ( _x[_nx - 1] - x[ixmax - 1] ) / ( x[ixmax] - x[ixmax - 1] );
        for ( i = 0; i < _nx; i++ )
        {
            if ( i == 0 )
            {
                _z[i][0] = getz( zp, ixmin, iymin ) * ( 1 - ty0 ) * ( 1 - tx0 ) + getz( zp, ixmin, iymin + 1 ) * ( 1 - tx0 ) * ty0
                           + getz( zp, ixmin + 1, iymin ) * tx0 * ( 1 - ty0 ) + getz( zp, ixmin + 1, iymin + 1 ) * tx0 * ty0;
                for ( j = 1; j < _ny - 1; j++ )
                    _z[i][j] = getz( zp, ixmin, iymin + j ) * ( 1 - tx0 ) + getz( zp, ixmin + 1, iymin + j ) * tx0;
                _z[i][_ny - 1] = getz( zp, ixmin, iymax - 1 ) * ( 1 - tx0 ) * ( 1 - ty1 ) + getz( zp, ixmin, iymax ) * ( 1 - tx0 ) * ty1
                                 + getz( zp, ixmin + 1, iymax - 1 ) * tx0 * ( 1 - ty1 ) + getz( zp, ixmin + 1, iymax ) * tx0 * ty1;
            }
            else if ( i == _nx - 1 )
            {
                _z[i][0] = getz( zp, ixmax - 1, iymin ) * ( 1 - tx1 ) * ( 1 - ty0 ) + getz( zp, ixmax - 1, iymin + 1 ) * ( 1 - tx1 ) * ty0
                           + getz( zp, ixmax, iymin ) * tx1 * ( 1 - ty0 ) + getz( zp, ixmax, iymin + 1 ) * tx1 * ty0;
                for ( j = 1; j < _ny - 1; j++ )
                    _z[i][j] = getz( zp, ixmax - 1, iymin + j ) * ( 1 - tx1 ) + getz( zp, ixmax, iymin + j ) * tx1;
                _z[i][_ny - 1] = getz( zp, ixmax - 1, iymax - 1 ) * ( 1 - tx1 ) * ( 1 - ty1 ) + getz( zp, ixmax, iymax ) * ( 1 - tx1 ) * ty1
                                 + getz( zp, ixmax, iymax - 1 ) * tx1 * ( 1 - ty1 ) + getz( zp, ixmax, iymax ) * tx1 * ty1;
            }
            else
            {
                _z[i][0] = getz( zp, ixmin + i, iymin ) * ( 1 - ty0 ) + getz( zp, ixmin + i, iymin + 1 ) * ty0;
                for ( j = 1; j < _ny - 1; j++ )
                    _z[i][j] = getz( zp, ixmin + i, iymin + j );
                _z[i][_ny - 1] = getz( zp, ixmin + i, iymax - 1 ) * ( 1 - ty1 ) + getz( zp, ixmin + i, iymax ) * ty1;
            }
            for ( j = 0; j < _ny; j++ )
            {
                if ( _z[i][j] < zmin )
                    _z[i][j] = zmin;
                else if ( _z[i][j] > zmax )
                    _z[i][j] = zmax;
            }
        }
        // replace the input with our clipped versions
        zp   = (PLPointer) _z;
        getz = plf2ops_c()->get;
        nx   = _nx;
        ny   = _ny;
        // Do not want to modify input x and y (const modifier)
        x_modified = (PLFLT_VECTOR) _x;
        y_modified = (PLFLT_VECTOR) _y;
    }
    else
    {
        x_modified = x;
        y_modified = y;
    }

    // From here on must use x_modified and y_modified rather than
    // x and y.
    if ( ( opt & BASE_CONT ) || ( opt & TOP_CONT ) || ( opt & MAG_COLOR ) )
    {
        //
        // Don't use the data z value to scale the color, use the z axis
        // values set by plw3d()
        //
        // plMinMax2dGrid(z, nx, ny, &fc_maxz, &fc_minz);
        //

        fc_minz = plsc->ranmi;
        fc_maxz = plsc->ranma;

        if ( fc_maxz == fc_minz )
        {
            plwarn( "plot3dcl: Maximum and minimum Z values are equal! \"fixing\"..." );
            fc_maxz = fc_minz + 1e-6;
        }
    }

    if ( opt & BASE_CONT )    // If enabled, draw the contour at the base.
    {
        if ( clevel != NULL && nlevel != 0 )
        {
            base_cont = 1;
            // even if MESH is not set, "set it",
            // as the base contour can only be done in this case
            pl3mode = 1;
        }
    }

    if ( opt & MAG_COLOR )    // If enabled, use magnitude colored wireframe
    {
        if ( ( ctmp = (PLFLT *) malloc( (size_t) ( 2 * MAX( nx, ny ) ) * sizeof ( PLFLT ) ) ) == NULL )
        {
            plexit( "c_plot3dcl: Insufficient memory" );
        }
    }
    else
        ctmp = NULL;

    // next logic only knows opt = 1 | 2 | 3, make sure that it only gets that
    opt &= DRAW_LINEXY;

    // Allocate work arrays

    utmp = (PLINT *) malloc( (size_t) ( 2 * MAX( nx, ny ) ) * sizeof ( PLINT ) );
    vtmp = (PLINT *) malloc( (size_t) ( 2 * MAX( nx, ny ) ) * sizeof ( PLINT ) );

    if ( !utmp || !vtmp )
        myexit( "plot3dcl: Out of memory." );

    plP_gw3wc( &cxx, &cxy, &cyx, &cyy, &cyz );
    init = 1;
// Call 3d line plotter.  Each viewing quadrant
// (perpendicular to x-y plane) must be handled separately.
    if ( cxx >= 0.0 && cxy <= 0.0 )
    {
        if ( opt == DRAW_LINEY )
            plt3zz( 1, ny, 1, -1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( iy = 2; iy <= ny; iy++ )
                plt3zz( 1, iy, 1, -1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
        if ( opt == DRAW_LINEX )
            plt3zz( 1, ny, 1, -1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( ix = 1; ix <= nx - 1; ix++ )
                plt3zz( ix, ny, 1, -1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
    }

    else if ( cxx <= 0.0 && cxy <= 0.0 )
    {
        if ( opt == DRAW_LINEX )
            plt3zz( nx, ny, -1, -1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( ix = 2; ix <= nx; ix++ )
                plt3zz( ix, ny, -1, -1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
        if ( opt == DRAW_LINEY )
            plt3zz( nx, ny, -1, -1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( iy = ny; iy >= 2; iy-- )
                plt3zz( nx, iy, -1, -1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
    }

    else if ( cxx <= 0.0 && cxy >= 0.0 )
    {
        if ( opt == DRAW_LINEY )
            plt3zz( nx, 1, -1, 1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( iy = ny - 1; iy >= 1; iy-- )
                plt3zz( nx, iy, -1, 1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
        if ( opt == DRAW_LINEX )
            plt3zz( nx, 1, -1, 1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( ix = nx; ix >= 2; ix-- )
                plt3zz( ix, 1, -1, 1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
    }

    else if ( cxx >= 0.0 && cxy >= 0.0 )
    {
        if ( opt == DRAW_LINEX )
            plt3zz( 1, 1, 1, 1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( ix = nx - 1; ix >= 1; ix-- )
                plt3zz( ix, 1, 1, 1, opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
        if ( opt == DRAW_LINEY )
            plt3zz( 1, 1, 1, 1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        else
        {
            for ( iy = 1; iy <= ny - 1; iy++ )
                plt3zz( 1, iy, 1, 1, -opt, &init, x_modified, y_modified, zops, zp, nx, ny, utmp, vtmp, ctmp );
        }
    }

    // draw contour at the base. Not 100%! Why?

    if ( base_cont )
    {
        int np = NPTS, j;
        CONT_LEVEL *cont, *clev;
        CONT_LINE *cline;

        PLINT *uu = (PLINT *) malloc( (size_t) NPTS * sizeof ( PLINT ) );
        PLINT *vv = (PLINT *) malloc( (size_t) NPTS * sizeof ( PLINT ) );
        // prepare cont_store input
        PLFLT **zstore;
        PLcGrid2 cgrid2;

        if ( ( uu == NULL ) || ( vv == NULL ) )
        {
            plexit( "c_plot3dcl: Insufficient memory" );
        }

        cgrid2.nx = nx;
        cgrid2.ny = ny;
        plAlloc2dGrid( &cgrid2.xg, nx, ny );
        plAlloc2dGrid( &cgrid2.yg, nx, ny );
        plAlloc2dGrid( &zstore, nx, ny );

        for ( i = 0; i < nx; i++ )
        {
            for ( j = 0; j < ny; j++ )
            {
                cgrid2.xg[i][j] = x_modified[i];
                cgrid2.yg[i][j] = y_modified[j];
                zstore[i][j]    = getz( zp, i, j );
            }
        }

        pl3upv = 0;

        // Fill cont structure with contours.
        cont_store( (PLFLT_MATRIX) zstore, nx, ny, 1, nx, 1, ny,
            clevel, nlevel, pltr2, (void *) &cgrid2, &cont );

        // Free the 2D input arrays to cont_store since not needed any more.
        plFree2dGrid( zstore, nx, ny );
        plFree2dGrid( cgrid2.xg, nx, ny );
        plFree2dGrid( cgrid2.yg, nx, ny );

        // follow the contour levels and lines
        clev = cont;
        do  // for each contour level
        {
            cline = clev->line;
            do  // there are several lines that make up each contour
            {
                int cx, k, l, m, start, end;
                PLFLT tx, ty;
                if ( cline->npts > np )
                {
                    np = cline->npts;
                    if ( ( ( uu = (PLINT *) realloc( uu, (size_t) np * sizeof ( PLINT ) ) ) == NULL ) ||
                         ( ( vv = (PLINT *) realloc( vv, (size_t) np * sizeof ( PLINT ) ) ) == NULL ) )
                    {
                        plexit( "c_plot3dcl: Insufficient memory" );
                    }
                }

                // the hidden line plotter plnxtv() only works OK if the x points are in increasing order.
                // As this does not always happens, the situation must be detected and the line segment
                // must be reversed before being plotted
                i = 0;
                if ( cline->npts > 0 )
                {
                    do
                    {
                        plcol1( ( clev->level - fc_minz ) / ( fc_maxz - fc_minz ) );
                        cx = plP_wcpcx( plP_w3wcx( cline->x[i], cline->y[i], plsc->ranmi ) );
                        for ( j = i; j < cline->npts; j++ ) // convert to 2D coordinates
                        {
                            uu[j] = plP_wcpcx( plP_w3wcx( cline->x[j], cline->y[j], plsc->ranmi ) );
                            vv[j] = plP_wcpcy( plP_w3wcy( cline->x[j], cline->y[j], plsc->ranmi ) );
                            if ( uu[j] < cx ) // find turn back point
                                break;
                            else
                                cx = uu[j];
                        }
                        plnxtv( &uu[i], &vv[i], NULL, j - i, 0 ); // plot line with increasing x

                        if ( j < cline->npts )                    // line not yet finished,
                        {
                            start = j - 1;
                            for ( i = start; i < cline->npts; i++ ) // search turn forward point
                            {
                                uu[i] = plP_wcpcx( plP_w3wcx( cline->x[i], cline->y[i], plsc->ranmi ) );
                                if ( uu[i] > cx )
                                    break;
                                else
                                    cx = uu[i];
                            }
                            end = i - 1;

                            for ( k = 0; k < ( end - start + 1 ) / 2; k++ ) // reverse line segment
                            {
                                l           = start + k;
                                m           = end - k;
                                tx          = cline->x[l];
                                ty          = cline->y[l];
                                cline->x[l] = cline->x[m];
                                cline->y[l] = cline->y[m];
                                cline->x[m] = tx;
                                cline->y[m] = ty;
                            }

                            // convert to 2D coordinates
                            for ( j = start; j <= end; j++ )
                            {
                                uu[j] = plP_wcpcx( plP_w3wcx( cline->x[j], cline->y[j], plsc->ranmi ) );
                                vv[j] = plP_wcpcy( plP_w3wcy( cline->x[j], cline->y[j], plsc->ranmi ) );
                            }
                            plnxtv( &uu[start], &vv[start], NULL, end - start + 1, 0 ); // and plot it

                            cline->x[end] = cline->x[start];
                            cline->y[end] = cline->y[start];
                            i             = end; // restart where it was left
                        }
                    } while ( j < cline->npts && i < cline->npts );
                }
                cline = cline->next;
            }
            while ( cline != NULL );
            clev = clev->next;
        }
        while ( clev != NULL );

        cont_clean_store( cont ); // now release the contour memory
        pl3upv = 1;
        free( uu );
        free( vv );
    }

// Finish up by drawing sides, background grid (both are optional)

    if ( side )
        plside3( x_modified, y_modified, zops, zp, nx, ny, opt );

    if ( zbflg )
    {
        color = plsc->icol0;
        width = plsc->width;
        plwidth( zbwidth );
        plcol0( zbcol );
        plgrid3( zbtck );
        plwidth( width );
        plcol0( color );
    }

    freework();

    if ( clipped )
    {
        free( _x );
        free( _y );
        for ( i = 0; i < nx; i++ )
            free( _z[i] );
        free( _z );
    }
}

//--------------------------------------------------------------------------
// void plxyindexlimits()
//
// Transform from y array limits to corresponding x array limits (or vice
// versa).
//
// N.B. we follow the convention here that all upper range limits are one
// more than the actual last index.
// instart (>= 0) through inn is the index range where
// the input inarray_min and inarray_max arrays are defined.
//
// outstart (>= 0), through outn (with outn <= outnmax) is the index range
// where the output outarray_min and outarray_max arrays are defined.
//
// In order to assure the transformation from y array limits to x array limits
// (or vice versa) is single-valued, this programme plaborts if the
// inarray_min array has a maximum or inarray_max array has a minimum.
//--------------------------------------------------------------------------

//static void
//plxyindexlimits( PLINT instart, PLINT inn,
//                 PLINT *inarray_min, PLINT *inarray_max,
//                 PLINT *outstart, PLINT *outn, PLINT outnmax,
//                 PLINT *outarray_min, PLINT *outarray_max )
//{
//    PLINT i, j;
//    if ( inn < 0 )
//    {
//        myabort( "plxyindexlimits: Must have instart >= 0" );
//        return;
//    }
//    if ( inn - instart <= 0 )
//    {
//        myabort( "plxyindexlimits: Must have at least 1 defined point" );
//        return;
//    }
//    *outstart = inarray_min[instart];
//    *outn     = inarray_max[instart];
//    for ( i = instart; i < inn; i++ )
//    {
//        *outstart = MIN( *outstart, inarray_min[i] );
//        *outn     = MAX( *outn, inarray_max[i] );
//        if ( i + 2 < inn )
//        {
//            if ( inarray_min[i] < inarray_min[i + 1] &&
//                 inarray_min[i + 1] > inarray_min[i + 2] )
//            {
//                myabort( "plxyindexlimits: inarray_min must not have a maximum" );
//                return;
//            }
//            if ( inarray_max[i] > inarray_max[i + 1] &&
//                 inarray_max[i + 1] < inarray_max[i + 2] )
//            {
//                myabort( "plxyindexlimits: inarray_max must not have a minimum" );
//                return;
//            }
//        }
//    }
//    if ( *outstart < 0 )
//    {
//        myabort( "plxyindexlimits: Must have all elements of inarray_min >= 0" );
//        return;
//    }
//    if ( *outn > outnmax )
//    {
//        myabort( "plxyindexlimits: Must have all elements of inarray_max <= outnmax" );
//        return;
//    }
//    for ( j = *outstart; j < *outn; j++ )
//    {
//        i = instart;
//        // Find first valid i for this j.
//        while ( i < inn && !( inarray_min[i] <= j && j < inarray_max[i] ) )
//            i++;
//        if ( i < inn )
//            outarray_min[j] = i;
//        else
//        {
//            myabort( "plxyindexlimits: bad logic; invalid i should never happen" );
//            return;
//        }
//        // Find next invalid i for this j.
//        while ( i < inn && ( inarray_min[i] <= j && j < inarray_max[i] ) )
//            i++;
//        outarray_max[j] = i;
//    }
//}

//--------------------------------------------------------------------------
// void plP_gzback()
//
// Get background parameters for 3d plot.
//--------------------------------------------------------------------------

void
plP_gzback( PLINT **zbf, PLINT **zbc, PLFLT **zbt, PLFLT **zbw )
{
    *zbf = &zbflg;
    *zbc = &zbcol;
    *zbt = &zbtck;
    *zbw = &zbwidth;
}

//--------------------------------------------------------------------------
// PLFLT plGetAngleToLight()
//
// Gets cos of angle between normal to a polygon and a light source.
// Requires at least 3 elements, forming non-parallel lines
// in the arrays.
//--------------------------------------------------------------------------

static PLFLT
plGetAngleToLight( PLFLT* x, PLFLT* y, PLFLT* z )
{
    PLFLT vx1, vx2, vy1, vy2, vz1, vz2;
    PLFLT px, py, pz;
    PLFLT vlx, vly, vlz;
    PLFLT mag1, mag2;
    PLFLT cosangle;

    vx1 = x[1] - x[0];
    vx2 = x[2] - x[1];
    vy1 = y[1] - y[0];
    vy2 = y[2] - y[1];
    vz1 = z[1] - z[0];
    vz2 = z[2] - z[1];

// Find vector perpendicular to the face
    px   = vy1 * vz2 - vz1 * vy2;
    py   = vz1 * vx2 - vx1 * vz2;
    pz   = vx1 * vy2 - vy1 * vx2;
    mag1 = px * px + py * py + pz * pz;

// Vectors were parallel!
    if ( mag1 == 0 )
        return 1;

    vlx  = xlight - x[0];
    vly  = ylight - y[0];
    vlz  = zlight - z[0];
    mag2 = vlx * vlx + vly * vly + vlz * vlz;
    if ( mag2 == 0 )
        return 1;

// Now have 3 vectors going through the first point on the given surface
    cosangle = fabs( ( vlx * px + vly * py + vlz * pz ) / sqrt( mag1 * mag2 ) );

// In case of numerical rounding
    if ( cosangle > 1 )
        cosangle = 1;
    return cosangle;
}

//--------------------------------------------------------------------------
// void plt3zz()
//
// Draws the next zig-zag line for a 3-d plot.  The data is stored in array
// z[][] as a function of x[] and y[], and is plotted out starting at index
// (x0,y0).
//
// Depending on the state of "flag", the sequence of data points sent to
// plnxtv is altered so as to allow cross-hatch plotting, or plotting
// parallel to either the x-axis or the y-axis.
//--------------------------------------------------------------------------

static void
plt3zz( PLINT x0, PLINT y0, PLINT dx, PLINT dy, PLINT flag, PLINT *init,
        PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny,
        PLINT *u, PLINT *v, PLFLT* c )
{
    PLINT n = 0;
    PLFLT x2d, y2d;
    PLFLT ( *getz )( PLPointer, PLINT, PLINT ) = zops->get;

    while ( 1 <= x0 && x0 <= nx && 1 <= y0 && y0 <= ny )
    {
        x2d  = plP_w3wcx( x[x0 - 1], y[y0 - 1], getz( zp, x0 - 1, y0 - 1 ) );
        y2d  = plP_w3wcy( x[x0 - 1], y[y0 - 1], getz( zp, x0 - 1, y0 - 1 ) );
        u[n] = plP_wcpcx( x2d );
        v[n] = plP_wcpcy( y2d );
        if ( c != NULL )
            c[n] = ( getz( zp, x0 - 1, y0 - 1 ) - fc_minz ) / ( fc_maxz - fc_minz );

        switch ( flag )
        {
        case -3:
            y0  += dy;
            flag = -flag;
            break;
        case -2:
            y0 += dy;
            break;
        case -1:
            y0  += dy;
            flag = -flag;
            break;
        case 1:
            x0 += dx;
            break;
        case 2:
            x0  += dx;
            flag = -flag;
            break;
        case 3:
            x0  += dx;
            flag = -flag;
            break;
        }
        n++;
    }

    if ( flag == 1 || flag == -2 )
    {
        if ( flag == 1 )
        {
            x0 -= dx;
            y0 += dy;
        }
        else if ( flag == -2 )
        {
            y0 -= dy;
            x0 += dx;
        }
        if ( 1 <= x0 && x0 <= nx && 1 <= y0 && y0 <= ny )
        {
            x2d  = plP_w3wcx( x[x0 - 1], y[y0 - 1], getz( zp, x0 - 1, y0 - 1 ) );
            y2d  = plP_w3wcy( x[x0 - 1], y[y0 - 1], getz( zp, x0 - 1, y0 - 1 ) );
            u[n] = plP_wcpcx( x2d );
            v[n] = plP_wcpcy( y2d );
            if ( c != NULL )
                c[n] = ( getz( zp, x0 - 1, y0 - 1 ) - fc_minz ) / ( fc_maxz - fc_minz );
            n++;
        }
    }

// All the setup is done.  Time to do the work.

    plnxtv( u, v, c, n, *init );
    *init = 0;
}

//--------------------------------------------------------------------------
// void plside3()
//
// This routine draws sides around the front of the 3d plot so that
// it does not appear to float.
//--------------------------------------------------------------------------

static void
plside3( PLFLT_VECTOR x, PLFLT_VECTOR y, PLF2OPS zops, PLPointer zp, PLINT nx, PLINT ny, PLINT opt )
{
    PLINT i;
    PLFLT cxx, cxy, cyx, cyy, cyz;
    PLFLT xmin, ymin, zmin, xmax, ymax, zmax, zscale;
    PLFLT tx, ty, ux, uy;
    PLFLT ( *getz )( PLPointer, PLINT, PLINT ) = zops->get;

    plP_gw3wc( &cxx, &cxy, &cyx, &cyy, &cyz );
    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin, &zmax );

// Get x, y coordinates of legs and plot

    if ( cxx >= 0.0 && cxy <= 0.0 )
    {
        if ( opt != 1 )
        {
            for ( i = 0; i < nx; i++ )
            {
                tx = plP_w3wcx( x[i], y[0], zmin );
                ty = plP_w3wcy( x[i], y[0], zmin );
                ux = plP_w3wcx( x[i], y[0], getz( zp, i, 0 ) );
                uy = plP_w3wcy( x[i], y[0], getz( zp, i, 0 ) );
                pljoin( tx, ty, ux, uy );
            }
        }
        if ( opt != 2 )
        {
            for ( i = 0; i < ny; i++ )
            {
                tx = plP_w3wcx( x[0], y[i], zmin );
                ty = plP_w3wcy( x[0], y[i], zmin );
                ux = plP_w3wcx( x[0], y[i], getz( zp, 0, i ) );
                uy = plP_w3wcy( x[0], y[i], getz( zp, 0, i ) );
                pljoin( tx, ty, ux, uy );
            }
        }
    }
    else if ( cxx <= 0.0 && cxy <= 0.0 )
    {
        if ( opt != 1 )
        {
            for ( i = 0; i < nx; i++ )
            {
                tx = plP_w3wcx( x[i], y[ny - 1], zmin );
                ty = plP_w3wcy( x[i], y[ny - 1], zmin );
                ux = plP_w3wcx( x[i], y[ny - 1], getz( zp, i, ny - 1 ) );
                uy = plP_w3wcy( x[i], y[ny - 1], getz( zp, i, ny - 1 ) );
                pljoin( tx, ty, ux, uy );
            }
        }
        if ( opt != 2 )
        {
            for ( i = 0; i < ny; i++ )
            {
                tx = plP_w3wcx( x[0], y[i], zmin );
                ty = plP_w3wcy( x[0], y[i], zmin );
                ux = plP_w3wcx( x[0], y[i], getz( zp, 0, i ) );
                uy = plP_w3wcy( x[0], y[i], getz( zp, 0, i ) );
                pljoin( tx, ty, ux, uy );
            }
        }
    }
    else if ( cxx <= 0.0 && cxy >= 0.0 )
    {
        if ( opt != 1 )
        {
            for ( i = 0; i < nx; i++ )
            {
                tx = plP_w3wcx( x[i], y[ny - 1], zmin );
                ty = plP_w3wcy( x[i], y[ny - 1], zmin );
                ux = plP_w3wcx( x[i], y[ny - 1], getz( zp, i, ny - 1 ) );
                uy = plP_w3wcy( x[i], y[ny - 1], getz( zp, i, ny - 1 ) );
                pljoin( tx, ty, ux, uy );
            }
        }
        if ( opt != 2 )
        {
            for ( i = 0; i < ny; i++ )
            {
                tx = plP_w3wcx( x[nx - 1], y[i], zmin );
                ty = plP_w3wcy( x[nx - 1], y[i], zmin );
                ux = plP_w3wcx( x[nx - 1], y[i], getz( zp, nx - 1, i ) );
                uy = plP_w3wcy( x[nx - 1], y[i], getz( zp, nx - 1, i ) );
                pljoin( tx, ty, ux, uy );
            }
        }
    }
    else if ( cxx >= 0.0 && cxy >= 0.0 )
    {
        if ( opt != 1 )
        {
            for ( i = 0; i < nx; i++ )
            {
                tx = plP_w3wcx( x[i], y[0], zmin );
                ty = plP_w3wcy( x[i], y[0], zmin );
                ux = plP_w3wcx( x[i], y[0], getz( zp, i, 0 ) );
                uy = plP_w3wcy( x[i], y[0], getz( zp, i, 0 ) );
                pljoin( tx, ty, ux, uy );
            }
        }
        if ( opt != 2 )
        {
            for ( i = 0; i < ny; i++ )
            {
                tx = plP_w3wcx( x[nx - 1], y[i], zmin );
                ty = plP_w3wcy( x[nx - 1], y[i], zmin );
                ux = plP_w3wcx( x[nx - 1], y[i], getz( zp, nx - 1, i ) );
                uy = plP_w3wcy( x[nx - 1], y[i], getz( zp, nx - 1, i ) );
                pljoin( tx, ty, ux, uy );
            }
        }
    }
}

//--------------------------------------------------------------------------
// void plgrid3()
//
// This routine draws a grid around the back side of the 3d plot with
// hidden line removal.
//--------------------------------------------------------------------------

static void
plgrid3( PLFLT tick )
{
    PLFLT xmin, ymin, zmin, xmax, ymax, zmax, zscale;
    PLFLT cxx, cxy, cyx, cyy, cyz, zmin_in, zmax_in;
    PLINT u[3], v[3];
    PLINT nsub = 0;
    PLFLT tp;

    plP_gw3wc( &cxx, &cxy, &cyx, &cyy, &cyz );
    plP_gdom( &xmin, &xmax, &ymin, &ymax );
    plP_grange( &zscale, &zmin_in, &zmax_in );
    zmin = ( zmax_in > zmin_in ) ? zmin_in : zmax_in;
    zmax = ( zmax_in > zmin_in ) ? zmax_in : zmin_in;

    pldtik( zmin, zmax, &tick, &nsub, FALSE );
    tp     = tick * floor( zmin / tick ) + tick;
    pl3upv = 0;

    if ( cxx >= 0.0 && cxy <= 0.0 )
    {
        while ( tp <= zmax )
        {
            u[0] = plP_wcpcx( plP_w3wcx( xmin, ymax, tp ) );
            v[0] = plP_wcpcy( plP_w3wcy( xmin, ymax, tp ) );
            u[1] = plP_wcpcx( plP_w3wcx( xmax, ymax, tp ) );
            v[1] = plP_wcpcy( plP_w3wcy( xmax, ymax, tp ) );
            u[2] = plP_wcpcx( plP_w3wcx( xmax, ymin, tp ) );
            v[2] = plP_wcpcy( plP_w3wcy( xmax, ymin, tp ) );
            plnxtv( u, v, 0, 3, 0 );

            tp += tick;
        }
        u[0] = plP_wcpcx( plP_w3wcx( xmax, ymax, zmin ) );
        v[0] = plP_wcpcy( plP_w3wcy( xmax, ymax, zmin ) );
        u[1] = plP_wcpcx( plP_w3wcx( xmax, ymax, zmax ) );
        v[1] = plP_wcpcy( plP_w3wcy( xmax, ymax, zmax ) );
        plnxtv( u, v, 0, 2, 0 );
    }
    else if ( cxx <= 0.0 && cxy <= 0.0 )
    {
        while ( tp <= zmax )
        {
            u[0] = plP_wcpcx( plP_w3wcx( xmax, ymax, tp ) );
            v[0] = plP_wcpcy( plP_w3wcy( xmax, ymax, tp ) );
            u[1] = plP_wcpcx( plP_w3wcx( xmax, ymin, tp ) );
            v[1] = plP_wcpcy( plP_w3wcy( xmax, ymin, tp ) );
            u[2] = plP_wcpcx( plP_w3wcx( xmin, ymin, tp ) );
            v[2] = plP_wcpcy( plP_w3wcy( xmin, ymin, tp ) );
            plnxtv( u, v, 0, 3, 0 );

            tp += tick;
        }
        u[0] = plP_wcpcx( plP_w3wcx( xmax, ymin, zmin ) );
        v[0] = plP_wcpcy( plP_w3wcy( xmax, ymin, zmin ) );
        u[1] = plP_wcpcx( plP_w3wcx( xmax, ymin, zmax ) );
        v[1] = plP_wcpcy( plP_w3wcy( xmax, ymin, zmax ) );
        plnxtv( u, v, 0, 2, 0 );
    }
    else if ( cxx <= 0.0 && cxy >= 0.0 )
    {
        while ( tp <= zmax )
        {
            u[0] = plP_wcpcx( plP_w3wcx( xmax, ymin, tp ) );
            v[0] = plP_wcpcy( plP_w3wcy( xmax, ymin, tp ) );
            u[1] = plP_wcpcx( plP_w3wcx( xmin, ymin, tp ) );
            v[1] = plP_wcpcy( plP_w3wcy( xmin, ymin, tp ) );
            u[2] = plP_wcpcx( plP_w3wcx( xmin, ymax, tp ) );
            v[2] = plP_wcpcy( plP_w3wcy( xmin, ymax, tp ) );
            plnxtv( u, v, 0, 3, 0 );

            tp += tick;
        }
        u[0] = plP_wcpcx( plP_w3wcx( xmin, ymin, zmin ) );
        v[0] = plP_wcpcy( plP_w3wcy( xmin, ymin, zmin ) );
        u[1] = plP_wcpcx( plP_w3wcx( xmin, ymin, zmax ) );
        v[1] = plP_wcpcy( plP_w3wcy( xmin, ymin, zmax ) );
        plnxtv( u, v, 0, 2, 0 );
    }
    else if ( cxx >= 0.0 && cxy >= 0.0 )
    {
        while ( tp <= zmax )
        {
            u[0] = plP_wcpcx( plP_w3wcx( xmin, ymin, tp ) );
            v[0] = plP_wcpcy( plP_w3wcy( xmin, ymin, tp ) );
            u[1] = plP_wcpcx( plP_w3wcx( xmin, ymax, tp ) );
            v[1] = plP_wcpcy( plP_w3wcy( xmin, ymax, tp ) );
            u[2] = plP_wcpcx( plP_w3wcx( xmax, ymax, tp ) );
            v[2] = plP_wcpcy( plP_w3wcy( xmax, ymax, tp ) );
            plnxtv( u, v, 0, 3, 0 );

            tp += tick;
        }
        u[0] = plP_wcpcx( plP_w3wcx( xmin, ymax, zmin ) );
        v[0] = plP_wcpcy( plP_w3wcy( xmin, ymax, zmin ) );
        u[1] = plP_wcpcx( plP_w3wcx( xmin, ymax, zmax ) );
        v[1] = plP_wcpcy( plP_w3wcy( xmin, ymax, zmax ) );
        plnxtv( u, v, 0, 2, 0 );
    }
    pl3upv = 1;
}

//--------------------------------------------------------------------------
// void plnxtv()
//
// Draw the next view of a 3-d plot. The physical coordinates of the
// points for the next view are placed in the n points of arrays u and
// v. The silhouette found so far is stored in the heap as a set of m peak
// points.
//
// These routines dynamically allocate memory for hidden line removal.
// Memory is allocated in blocks of 2*BINC*sizeof(PLINT) bytes.  Large
// values of BINC give better performance but also allocate more memory
// than is needed. If your 3D plots are very "spiky" or you are working
// with very large matrices then you will probably want to increase BINC.
//--------------------------------------------------------------------------

static void
plnxtv( PLINT *u, PLINT *v, PLFLT* c, PLINT n, PLINT init )
{
    plnxtvhi( u, v, c, n, init );

    if ( pl3mode )
        plnxtvlo( u, v, c, n, init );
}

//--------------------------------------------------------------------------
// void plnxtvhi()
//
// Draw the top side of the 3-d plot.
//--------------------------------------------------------------------------

static void
plnxtvhi( PLINT *u, PLINT *v, PLFLT* c, PLINT n, PLINT init )
{
    //
    // For the initial set of points, just display them and store them as the
    // peak points.
    //
    if ( init == 1 )
    {
        int i;
        oldhiview = (PLINT *) malloc( (size_t) ( 2 * n ) * sizeof ( PLINT ) );
        if ( !oldhiview )
            myexit( "plnxtvhi: Out of memory." );

        oldhiview[0] = u[0];
        oldhiview[1] = v[0];
        plP_draw3d( u[0], v[0], c, 0, 1 );
        for ( i = 1; i < n; i++ )
        {
            oldhiview[2 * i]     = u[i];
            oldhiview[2 * i + 1] = v[i];
            plP_draw3d( u[i], v[i], c, i, 0 );
        }
        mhi = n;
        return;
    }

    //
    // Otherwise, we need to consider hidden-line removal problem. We scan
    // over the points in both the old (i.e. oldhiview[]) and new (i.e. u[]
    // and v[]) arrays in order of increasing x coordinate.  At each stage, we
    // find the line segment in the other array (if one exists) that straddles
    // the x coordinate of the point. We have to determine if the point lies
    // above or below the line segment, and to check if the below/above status
    // has changed since the last point.
    //
    // If pl3upv = 0 we do not update the view, this is useful for drawing
    // lines on the graph after we are done plotting points.  Hidden line
    // removal is still done, but the view is not updated.
    //
    xxhi = 0;
    if ( pl3upv != 0 )
    {
        newhisize = 2 * ( mhi + BINC );
        if ( newhiview != NULL )
        {
            newhiview =
                (PLINT *) realloc( (void *) newhiview,
                    (size_t) newhisize * sizeof ( PLINT ) );
        }
        else
        {
            newhiview =
                (PLINT *) malloc( (size_t) newhisize * sizeof ( PLINT ) );
        }
        if ( !newhiview )
            myexit( "plnxtvhi: Out of memory." );
    }

    // Do the draw or shading with hidden line removal

    plnxtvhi_draw( u, v, c, n );

    // Set oldhiview

    swaphiview();
}

//--------------------------------------------------------------------------
// void plnxtvhi_draw()
//
// Draw the top side of the 3-d plot.
//--------------------------------------------------------------------------

static void
plnxtvhi_draw( PLINT *u, PLINT *v, PLFLT* c, PLINT n )
{
    PLINT i   = 0, j = 0, first = 1;
    PLINT sx1 = 0, sx2 = 0, sy1 = 0, sy2 = 0;
    PLINT su1, su2, sv1, sv2;
    PLINT cx, cy, px, py;
    PLINT seg, ptold, lstold = 0, pthi, pnewhi = 0, newhi, change, ochange = 0;

//
// (oldhiview[2*i], oldhiview[2*i]) is the i'th point in the old array
// (u[j], v[j]) is the j'th point in the new array
//

//
// First attempt at 3d shading.  It works ok for simple plots, but
// will just not draw faces, or draw them overlapping for very
// jagged plots
//

    while ( i < mhi || j < n )
    {
        //
        // The coordinates of the point under consideration are (px,py).  The
        // line segment joins (sx1,sy1) to (sx2,sy2).  "ptold" is true if the
        // point lies in the old array. We set it by comparing the x coordinates
        // of the i'th old point and the j'th new point, being careful if we
        // have fallen past the edges. Having found the point, load up the point
        // and segment coordinates appropriately.
        //

        ptold = ( j >= n || ( i < mhi && oldhiview[2 * i] < u[j] ) );
        if ( ptold )
        {
            px  = oldhiview[2 * i];
            py  = oldhiview[2 * i + 1];
            seg = j > 0 && j < n;
            if ( seg )
            {
                sx1 = u[j - 1];
                sy1 = v[j - 1];
                sx2 = u[j];
                sy2 = v[j];
            }
        }
        else
        {
            px  = u[j];
            py  = v[j];
            seg = i > 0 && i < mhi;
            if ( seg )
            {
                sx1 = oldhiview[2 * ( i - 1 )];
                sy1 = oldhiview[2 * ( i - 1 ) + 1];
                sx2 = oldhiview[2 * i];
                sy2 = oldhiview[2 * i + 1];
            }
        }

        //
        // Now determine if the point is higher than the segment, using the
        // logical function "above". We also need to know if it is the old view
        // or the new view that is higher. "newhi" is set true if the new view
        // is higher than the old.
        //
        if ( seg )
            pthi = plabv( px, py, sx1, sy1, sx2, sy2 );
        else
            pthi = 1;

        newhi = ( ptold && !pthi ) || ( !ptold && pthi );
        //
        // The last point and this point lie on different sides of
        // the current silouette
        //
        change = ( newhi && !pnewhi ) || ( !newhi && pnewhi );

        //
        // There is a new intersection point to put in the peak array if the
        // state of "newhi" changes.
        //
        if ( first )
        {
            plP_draw3d( px, py, c, j, 1 );
            first  = 0;
            lstold = ptold;
            savehipoint( px, py );
            pthi    = 0;
            ochange = 0;
        }
        else if ( change )
        {
            //
            // Take care of special cases at end of arrays.  If pl3upv is 0 the
            // endpoints are not connected to the old view.
            //
            if ( pl3upv == 0 && ( ( !ptold && j == 0 ) || ( ptold && i == 0 ) ) )
            {
                plP_draw3d( px, py, c, j, 1 );
                lstold  = ptold;
                pthi    = 0;
                ochange = 0;
            }
            else if ( pl3upv == 0 &&
                      ( ( !ptold && i >= mhi ) || ( ptold && j >= n ) ) )
            {
                plP_draw3d( px, py, c, j, 1 );
                lstold  = ptold;
                pthi    = 0;
                ochange = 0;
            }
            else
            {
                //
                // If pl3upv is not 0 then we do want to connect the current line
                // with the previous view at the endpoints.  Also find intersection
                // point with old view.
                //
                if ( i == 0 )
                {
                    sx1 = oldhiview[0];
                    sy1 = -1;
                    sx2 = oldhiview[0];
                    sy2 = oldhiview[1];
                }
                else if ( i >= mhi )
                {
                    sx1 = oldhiview[2 * ( mhi - 1 )];
                    sy1 = oldhiview[2 * ( mhi - 1 ) + 1];
                    sx2 = oldhiview[2 * ( mhi - 1 )];
                    sy2 = -1;
                }
                else
                {
                    sx1 = oldhiview[2 * ( i - 1 )];
                    sy1 = oldhiview[2 * ( i - 1 ) + 1];
                    sx2 = oldhiview[2 * i];
                    sy2 = oldhiview[2 * i + 1];
                }

                if ( j == 0 )
                {
                    su1 = u[0];
                    sv1 = -1;
                    su2 = u[0];
                    sv2 = v[0];
                }
                else if ( j >= n )
                {
                    su1 = u[n - 1];
                    sv1 = v[n - 1];
                    su2 = u[n - 1];
                    sv2 = -1;
                }
                else
                {
                    su1 = u[j - 1];
                    sv1 = v[j - 1];
                    su2 = u[j];
                    sv2 = v[j];
                }

                // Determine the intersection

                pl3cut( sx1, sy1, sx2, sy2, su1, sv1, su2, sv2, &cx, &cy );
                if ( cx == px && cy == py )
                {
                    if ( lstold && !ochange )
                        plP_draw3d( px, py, c, j, 1 );
                    else
                        plP_draw3d( px, py, c, j, 0 );

                    savehipoint( px, py );
                    lstold = 1;
                    pthi   = 0;
                }
                else
                {
                    if ( lstold && !ochange )
                        plP_draw3d( cx, cy, c, j, 1 );
                    else
                        plP_draw3d( cx, cy, c, j, 0 );

                    lstold = 1;
                    savehipoint( cx, cy );
                }
                ochange = 1;
            }
        }

        // If point is high then draw plot to point and update view.

        if ( pthi )
        {
            if ( lstold && ptold )
                plP_draw3d( px, py, c, j, 1 );
            else
                plP_draw3d( px, py, c, j, 0 );

            savehipoint( px, py );
            lstold  = ptold;
            ochange = 0;
        }
        pnewhi = newhi;

        if ( ptold )
            i++;
        else
            j++;
    }
}

//--------------------------------------------------------------------------
// void  plP_draw3d()
//
// Does a simple move or line draw.
//--------------------------------------------------------------------------

static void
plP_draw3d( PLINT x, PLINT y, PLFLT *c, PLINT j, PLINT move )
{
    if ( move )
        plP_movphy( x, y );
    else
    {
        if ( c != NULL )
            plcol1( c[j - 1] );
        plP_draphy( x, y );
    }
}

//--------------------------------------------------------------------------
// void plnxtvlo()
//
// Draw the bottom side of the 3-d plot.
//--------------------------------------------------------------------------

static void
plnxtvlo( PLINT *u, PLINT *v, PLFLT*c, PLINT n, PLINT init )
{
    PLINT i, j, first;
    PLINT sx1 = 0, sx2 = 0, sy1 = 0, sy2 = 0;
    PLINT su1, su2, sv1, sv2;
    PLINT cx, cy, px, py;
    PLINT seg, ptold, lstold = 0, ptlo, pnewlo, newlo, change, ochange = 0;

    first  = 1;
    pnewlo = 0;

    //
    // For the initial set of points, just display them and store them as the
    // peak points.
    //
    if ( init == 1 )
    {
        oldloview = (PLINT *) malloc( (size_t) ( 2 * n ) * sizeof ( PLINT ) );
        if ( !oldloview )
            myexit( "\nplnxtvlo: Out of memory." );

        plP_draw3d( u[0], v[0], c, 0, 1 );
        oldloview[0] = u[0];
        oldloview[1] = v[0];
        for ( i = 1; i < n; i++ )
        {
            plP_draw3d( u[i], v[i], c, i, 0 );
            oldloview[2 * i]     = u[i];
            oldloview[2 * i + 1] = v[i];
        }
        mlo = n;
        return;
    }

    //
    // Otherwise, we need to consider hidden-line removal problem. We scan
    // over the points in both the old (i.e. oldloview[]) and new (i.e. u[]
    // and v[]) arrays in order of increasing x coordinate.  At each stage, we
    // find the line segment in the other array (if one exists) that straddles
    // the x coordinate of the point. We have to determine if the point lies
    // above or below the line segment, and to check if the below/above status
    // has changed since the last point.
    //
    // If pl3upv = 0 we do not update the view, this is useful for drawing
    // lines on the graph after we are done plotting points.  Hidden line
    // removal is still done, but the view is not updated.
    //
    xxlo = 0;
    i    = 0;
    j    = 0;
    if ( pl3upv != 0 )
    {
        newlosize = 2 * ( mlo + BINC );
        if ( newloview != NULL )
        {
            newloview =
                (PLINT *) realloc( (void *) newloview,
                    (size_t) newlosize * sizeof ( PLINT ) );
        }
        else
        {
            newloview =
                (PLINT *) malloc( (size_t) newlosize * sizeof ( PLINT ) );
        }
        if ( !newloview )
            myexit( "plnxtvlo: Out of memory." );
    }

    //
    // (oldloview[2*i], oldloview[2*i]) is the i'th point in the old array
    // (u[j], v[j]) is the j'th point in the new array.
    //
    while ( i < mlo || j < n )
    {
        //
        // The coordinates of the point under consideration are (px,py).  The
        // line segment joins (sx1,sy1) to (sx2,sy2).  "ptold" is true if the
        // point lies in the old array. We set it by comparing the x coordinates
        // of the i'th old point and the j'th new point, being careful if we
        // have fallen past the edges. Having found the point, load up the point
        // and segment coordinates appropriately.
        //
        ptold = ( j >= n || ( i < mlo && oldloview[2 * i] < u[j] ) );
        if ( ptold )
        {
            px  = oldloview[2 * i];
            py  = oldloview[2 * i + 1];
            seg = j > 0 && j < n;
            if ( seg )
            {
                sx1 = u[j - 1];
                sy1 = v[j - 1];
                sx2 = u[j];
                sy2 = v[j];
            }
        }
        else
        {
            px  = u[j];
            py  = v[j];
            seg = i > 0 && i < mlo;
            if ( seg )
            {
                sx1 = oldloview[2 * ( i - 1 )];
                sy1 = oldloview[2 * ( i - 1 ) + 1];
                sx2 = oldloview[2 * i];
                sy2 = oldloview[2 * i + 1];
            }
        }

        //
        // Now determine if the point is lower than the segment, using the
        // logical function "above". We also need to know if it is the old view
        // or the new view that is lower. "newlo" is set true if the new view is
        // lower than the old.
        //
        if ( seg )
            ptlo = !plabv( px, py, sx1, sy1, sx2, sy2 );
        else
            ptlo = 1;

        newlo  = ( ptold && !ptlo ) || ( !ptold && ptlo );
        change = ( newlo && !pnewlo ) || ( !newlo && pnewlo );

        //
        // There is a new intersection point to put in the peak array if the
        // state of "newlo" changes.
        //
        if ( first )
        {
            plP_draw3d( px, py, c, j, 1 );
            first  = 0;
            lstold = ptold;
            savelopoint( px, py );
            ptlo    = 0;
            ochange = 0;
        }
        else if ( change )
        {
            //
            // Take care of special cases at end of arrays.  If pl3upv is 0 the
            // endpoints are not connected to the old view.
            //
            if ( pl3upv == 0 && ( ( !ptold && j == 0 ) || ( ptold && i == 0 ) ) )
            {
                plP_draw3d( px, py, c, j, 1 );
                lstold  = ptold;
                ptlo    = 0;
                ochange = 0;
            }
            else if ( pl3upv == 0 &&
                      ( ( !ptold && i >= mlo ) || ( ptold && j >= n ) ) )
            {
                plP_draw3d( px, py, c, j, 1 );
                lstold  = ptold;
                ptlo    = 0;
                ochange = 0;
            }

            //
            // If pl3upv is not 0 then we do want to connect the current line
            // with the previous view at the endpoints.  Also find intersection
            // point with old view.
            //
            else
            {
                if ( i == 0 )
                {
                    sx1 = oldloview[0];
                    sy1 = 100000;
                    sx2 = oldloview[0];
                    sy2 = oldloview[1];
                }
                else if ( i >= mlo )
                {
                    sx1 = oldloview[2 * ( mlo - 1 )];
                    sy1 = oldloview[2 * ( mlo - 1 ) + 1];
                    sx2 = oldloview[2 * ( mlo - 1 )];
                    sy2 = 100000;
                }
                else
                {
                    sx1 = oldloview[2 * ( i - 1 )];
                    sy1 = oldloview[2 * ( i - 1 ) + 1];
                    sx2 = oldloview[2 * i];
                    sy2 = oldloview[2 * i + 1];
                }

                if ( j == 0 )
                {
                    su1 = u[0];
                    sv1 = 100000;
                    su2 = u[0];
                    sv2 = v[0];
                }
                else if ( j >= n )
                {
                    su1 = u[n - 1];
                    sv1 = v[n - 1];
                    su2 = u[n];
                    sv2 = 100000;
                }
                else
                {
                    su1 = u[j - 1];
                    sv1 = v[j - 1];
                    su2 = u[j];
                    sv2 = v[j];
                }

                // Determine the intersection

                pl3cut( sx1, sy1, sx2, sy2, su1, sv1, su2, sv2, &cx, &cy );
                if ( cx == px && cy == py )
                {
                    if ( lstold && !ochange )
                        plP_draw3d( px, py, c, j, 1 );
                    else
                        plP_draw3d( px, py, c, j, 0 );

                    savelopoint( px, py );
                    lstold = 1;
                    ptlo   = 0;
                }
                else
                {
                    if ( lstold && !ochange )
                        plP_draw3d( cx, cy, c, j, 1 );
                    else
                        plP_draw3d( cx, cy, c, j, 0 );

                    lstold = 1;
                    savelopoint( cx, cy );
                }
                ochange = 1;
            }
        }

        // If point is low then draw plot to point and update view.

        if ( ptlo )
        {
            if ( lstold && ptold )
                plP_draw3d( px, py, c, j, 1 );
            else
                plP_draw3d( px, py, c, j, 0 );

            savelopoint( px, py );
            lstold  = ptold;
            ochange = 0;
        }

        pnewlo = newlo;

        if ( ptold )
            i = i + 1;
        else
            j = j + 1;
    }

    // Set oldloview

    swaploview();
}

//--------------------------------------------------------------------------
// savehipoint
// savelopoint
//
// Add a point to the list of currently visible peaks/valleys, when
// drawing the top/bottom surface, respectively.
//--------------------------------------------------------------------------

static void
savehipoint( PLINT px, PLINT py )
{
    if ( pl3upv == 0 )
        return;

    if ( xxhi >= newhisize )      // allocate additional space
    {
        newhisize += 2 * BINC;
        newhiview  = (PLINT *) realloc( (void *) newhiview,
            (size_t) newhisize * sizeof ( PLINT ) );
        if ( !newhiview )
            myexit( "savehipoint: Out of memory." );
    }

    newhiview[xxhi] = px;
    xxhi++;
    newhiview[xxhi] = py;
    xxhi++;
}

static void
savelopoint( PLINT px, PLINT py )
{
    if ( pl3upv == 0 )
        return;

    if ( xxlo >= newlosize )      // allocate additional space
    {
        newlosize += 2 * BINC;
        newloview  = (PLINT *) realloc( (void *) newloview,
            (size_t) newlosize * sizeof ( PLINT ) );
        if ( !newloview )
            myexit( "savelopoint: Out of memory." );
    }

    newloview[xxlo] = px;
    xxlo++;
    newloview[xxlo] = py;
    xxlo++;
}

//--------------------------------------------------------------------------
// swaphiview
// swaploview
//
// Swaps the top/bottom views.  Need to do a real swap so that the
// memory cleanup routine really frees everything (and only once).
//--------------------------------------------------------------------------

static void
swaphiview( void )
{
    PLINT *tmp;

    if ( pl3upv != 0 )
    {
        mhi       = xxhi / 2;
        tmp       = oldhiview;
        oldhiview = newhiview;
        newhiview = tmp;
    }
}

static void
swaploview( void )
{
    PLINT *tmp;

    if ( pl3upv != 0 )
    {
        mlo       = xxlo / 2;
        tmp       = oldloview;
        oldloview = newloview;
        newloview = tmp;
    }
}

//--------------------------------------------------------------------------
// freework
//
// Frees memory associated with work arrays
//--------------------------------------------------------------------------

static void
freework( void )
{
    free_mem( oldhiview );
    free_mem( oldloview );
    free_mem( newhiview );
    free_mem( newloview );
    free_mem( vtmp );
    free_mem( utmp );
    free_mem( ctmp );
}

//--------------------------------------------------------------------------
// myexit
//
// Calls plexit, cleaning up first.
//--------------------------------------------------------------------------

static void
myexit( PLCHAR_VECTOR msg )
{
    freework();
    plexit( msg );
}

//--------------------------------------------------------------------------
// myabort
//
// Calls plabort, cleaning up first.
// Caller should return to the user program.
//--------------------------------------------------------------------------

static void
myabort( PLCHAR_VECTOR msg )
{
    freework();
    plabort( msg );
}

//--------------------------------------------------------------------------
// int plabv()
//
// Determines if point (px,py) lies above the line joining (sx1,sy1) to
// (sx2,sy2). It only works correctly if sx1 <= px <= sx2.
//--------------------------------------------------------------------------

static int
plabv( PLINT px, PLINT py, PLINT sx1, PLINT sy1, PLINT sx2, PLINT sy2 )
{
    int above;

    if ( py >= sy1 && py >= sy2 )
        above = 1;
    else if ( py < sy1 && py < sy2 )
        above = 0;
    else if ( (double) ( sx2 - sx1 ) * ( py - sy1 ) >=
              (double) ( px - sx1 ) * ( sy2 - sy1 ) )
        above = 1;
    else
        above = 0;

    return above;
}

//--------------------------------------------------------------------------
// void pl3cut()
//
// Determines the point of intersection (cx,cy) between the line joining
// (sx1,sy1) to (sx2,sy2) and the line joining (su1,sv1) to (su2,sv2).
//--------------------------------------------------------------------------

static void
pl3cut( PLINT sx1, PLINT sy1, PLINT sx2, PLINT sy2,
        PLINT su1, PLINT sv1, PLINT su2, PLINT sv2, PLINT *cx, PLINT *cy )
{
    PLINT x21, y21, u21, v21, yv1, xu1, a, b;
    double fa, fb;

    x21 = sx2 - sx1;
    y21 = sy2 - sy1;
    u21 = su2 - su1;
    v21 = sv2 - sv1;
    yv1 = sy1 - sv1;
    xu1 = sx1 - su1;

    a  = x21 * v21 - y21 * u21;
    fa = (double) a;
    if ( a == 0 )
    {
        if ( sx2 < su2 )
        {
            *cx = sx2;
            *cy = sy2;
        }
        else
        {
            *cx = su2;
            *cy = sv2;
        }
        return;
    }
    else
    {
        b   = yv1 * u21 - xu1 * v21;
        fb  = (double) b;
        *cx = (PLINT) ( sx1 + ( fb * x21 ) / fa + .5 );
        *cy = (PLINT) ( sy1 + ( fb * y21 ) / fa + .5 );
    }
}

//--------------------------------------------------------------------------
// void plRotationShear
//
// Calculates the rotation and shear angles from a plplot transformation matrix
//
// N.B. the plot transformation matrix
//
// [xFormMatrix[0] xFormMatrix[2]]
// [xFormMatrix[1] xFormMatrix[3]]
//
// is calculated as
//
// [stride cos(t)    stride sin(t)]
// [sin(p-t)              cos(p-t)]
//
// where t is the rotation angle and p is the shear angle.
// The purpose of this routine is to determine stride, rotation angle,
// and shear angle from xFormMatrix.
//
// For information only, xFormMatrix is the product of the following
// rotation, skew(shear), and scale matrices:
//
//  [stride    0] [1      0] [cos(t)  sin(t)]
//  [0    cos(p)] [tan(p) 1] [-sin(t) cos(t)]
//
//--------------------------------------------------------------------------

void
plRotationShear( PLFLT *xFormMatrix, PLFLT *rotation, PLFLT *shear, PLFLT *stride )
{
    PLFLT smr;
    *stride = sqrt( xFormMatrix[0] * xFormMatrix[0] + xFormMatrix[2] * xFormMatrix[2] );

    // Calculate rotation in range from -pi to pi.
    *rotation = atan2( xFormMatrix[2], xFormMatrix[0] );

    // Calculate shear - rotation in range from -pi to pi.
    smr = atan2( xFormMatrix[1], xFormMatrix[3] );

    // Calculate shear in range from -2 pi to 2 pi.
    *shear = smr + *rotation;

    // Calculate shear in range from -pi to pi.
    if ( *shear < -PI )
        *shear += 2. * PI;
    else if ( *shear > PI )
        *shear -= 2. * PI;

    // Actually must honour some convention to calculate the negative
    // of the shear angle instead of the shear angle. Why??
    *shear = -*shear;
    // Comment out the modified old logic which determines the negative
    // of the shear angle in a more complicated way.  Note, the modification
    // to divide the asin argument by *stride which solved a long-standing
    // bug (as does the above logic in a simpler way).
    //
    //shear = -asin( (xFormMatrix[0] * xFormMatrix[1] +
    //               xFormMatrix[2] * xFormMatrix[3] )/ *stride);
    //

    // Compute the cross product of the vectors [1,0] and [0,1] to
    // determine if we need to make a "quadrant 3,4" adjustment
    // to the shear angle.

    //
    // if ( xFormMatrix[0] * xFormMatrix[3] - xFormMatrix[1] * xFormMatrix[2] < 0.0 )
    // {
    //shear = -( M_PI + *shear );
    // }
    //
}

