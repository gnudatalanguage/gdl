//      Contour plotter.
//
// Copyright (C) 1995, 2000, 2001 Maurice LeBrun
// Copyright (C) 2000, 2002 Joao Cardoso
// Copyright (C) 2000-2014 Alan W. Irwin
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

#ifdef MSDOS
#pragma optimize("",off)
#endif

// Static function prototypes.

static void
plcntr( PLF2EVAL_callback plf2eval, PLPointer plf2eval_data,
        PLINT nx, PLINT ny, PLINT kx, PLINT lx,
        PLINT ky, PLINT ly, PLFLT flev, PLINT **ipts,
        PLTRANSFORM_callback pltr, PLPointer pltr_data );

static void
pldrawcn( PLF2EVAL_callback plf2eval, PLPointer plf2eval_data,
          PLINT nx, PLINT ny, PLINT kx, PLINT lx,
          PLINT ky, PLINT ly, PLFLT flev, char *flabel, PLINT kcol, PLINT krow,
          PLFLT lastx, PLFLT lasty, PLINT startedge,
          PLINT **ipts, PLFLT *distance, PLINT *lastindex,
          PLTRANSFORM_callback pltr, PLPointer pltr_data );

static void
plfloatlabel( PLFLT value, char *string, PLINT len );

static PLFLT
plP_pcwcx( PLINT x );

static PLFLT
plP_pcwcy( PLINT y );

static void
pl_drawcontlabel( PLFLT tpx, PLFLT tpy, char *flabel, PLFLT *distance, PLINT *lastindex );

// Error flag for aborts

static int error;

//**************************************
//
// Defaults for contour label printing.
//
//**************************************

// Font height for contour labels (normalized)
static PLFLT
    contlabel_size = 0.3;

// Offset of label from contour line (if set to 0.0, labels are printed on the lines).
static PLFLT
    contlabel_offset = 0.006;

// Spacing parameter for contour labels
static PLFLT
    contlabel_space = 0.1;

// Activate labels, default off
static PLINT
    contlabel_active = 0;

// If the contour label exceed 10^(limexp) or 10^(-limexp), the exponential format is used
static PLINT
    limexp = 4;

// Number of significant digits
static PLINT
    sigprec = 2;

//******* contour lines storage ***************************

static CONT_LEVEL *startlev = NULL;
static CONT_LEVEL *currlev;
static CONT_LINE  *currline;

static int        cont3d = 0;

static CONT_LINE *
alloc_line( void )
{
    CONT_LINE *line;

    if ( ( line = (CONT_LINE *) malloc( sizeof ( CONT_LINE ) ) ) == NULL )
    {
        plexit( "alloc_line: Insufficient memory" );
    }

    line->x = (PLFLT *) malloc( LINE_ITEMS * sizeof ( PLFLT ) );
    line->y = (PLFLT *) malloc( LINE_ITEMS * sizeof ( PLFLT ) );

    if ( ( line->x == NULL ) || ( line->y == NULL ) )
    {
        plexit( "alloc_line: Insufficient memory" );
    }

    line->npts = 0;
    line->next = NULL;

    return line;
}

static CONT_LEVEL *
alloc_level( PLFLT level )
{
    CONT_LEVEL *node;

    if ( ( node = (CONT_LEVEL *) malloc( sizeof ( CONT_LEVEL ) ) ) == NULL )
    {
        plexit( "alloc_level: Insufficient memory" );
    }
    node->level = level;
    node->next  = NULL;
    node->line  = alloc_line( );

    return node;
}

static void
realloc_line( CONT_LINE *line )
{
    if ( ( ( line->x = (PLFLT *) realloc( line->x,
                 (size_t) ( line->npts + LINE_ITEMS ) * sizeof ( PLFLT ) ) ) == NULL ) ||
         ( ( line->y = (PLFLT *) realloc( line->y,
                 (size_t) ( line->npts + LINE_ITEMS ) * sizeof ( PLFLT ) ) ) == NULL ) )
        plexit( "realloc_line: Insufficient memory" );
}


// new contour level
static void
cont_new_store( PLFLT level )
{
    if ( cont3d )
    {
        if ( startlev == NULL )
        {
            startlev = alloc_level( level );
            currlev  = startlev;
        }
        else
        {
            currlev->next = alloc_level( level );
            currlev       = currlev->next;
        }
        currline = currlev->line;
    }
}

void
cont_clean_store( CONT_LEVEL *ct )
{
    CONT_LINE  *tline, *cline;
    CONT_LEVEL *tlev, *clevel;

    if ( ct != NULL )
    {
        clevel = ct;

        do
        {
            cline = clevel->line;
            do
            {
#ifdef CONT_PLOT_DEBUG
                plP_movwor( cline->x[0], cline->y[0] );
                for ( j = 1; j < cline->npts; j++ )
                    plP_drawor( cline->x[j], cline->y[j] );
#endif
                tline = cline->next;
                free( cline->x );
                free( cline->y );
                free( cline );
                cline = tline;
            }
            while ( cline != NULL );
            tlev = clevel->next;
            free( clevel );
            clevel = tlev;
        }
        while ( clevel != NULL );
        startlev = NULL;
    }
}

static void
cont_xy_store( PLFLT xx, PLFLT yy )
{
    if ( cont3d )
    {
        PLINT pts = currline->npts;

        if ( pts % LINE_ITEMS == 0 )
            realloc_line( currline );

        currline->x[pts] = xx;
        currline->y[pts] = yy;
        currline->npts++;
    }
    else
        plP_drawor( xx, yy );
}

static void
cont_mv_store( PLFLT xx, PLFLT yy )
{
    if ( cont3d )
    {
        if ( currline->npts != 0 ) // not an empty list, allocate new
        {
            currline->next = alloc_line( );
            currline       = currline->next;
        }

        // and fill first element
        currline->x[0] = xx;
        currline->y[0] = yy;
        currline->npts = 1;
    }
    else
        plP_movwor( xx, yy );
}

// small routine to set offset and spacing of contour labels, see desciption above
void c_pl_setcontlabelparam( PLFLT offset, PLFLT size, PLFLT spacing, PLINT active )
{
    contlabel_offset = offset;
    contlabel_size   = size;
    contlabel_space  = spacing;
    contlabel_active = active;
}

// small routine to set the format of the contour labels, description of limexp and prec see above
void c_pl_setcontlabelformat( PLINT lexp, PLINT sigdig )
{
    limexp  = lexp;
    sigprec = sigdig;
}

static void pl_drawcontlabel( PLFLT tpx, PLFLT tpy, char *flabel, PLFLT *distance, PLINT *lastindex )
{
    PLFLT delta_x, delta_y;
    PLINT currx_old, curry_old;

    delta_x = plP_pcdcx( plsc->currx ) - plP_pcdcx( plP_wcpcx( tpx ) );
    delta_y = plP_pcdcy( plsc->curry ) - plP_pcdcy( plP_wcpcy( tpy ) );

    currx_old = plsc->currx;
    curry_old = plsc->curry;

    *distance += sqrt( delta_x * delta_x + delta_y * delta_y );

    plP_drawor( tpx, tpy );

    if ( (int) ( fabs( *distance / contlabel_space ) ) > *lastindex )
    {
        PLFLT scale, vec_x, vec_y, mx, my, dev_x, dev_y, off_x, off_y;

        vec_x = tpx - plP_pcwcx( currx_old );
        vec_y = tpy - plP_pcwcy( curry_old );

        // Ensure labels appear the right way up
        if ( vec_x < 0 )
        {
            vec_x = -vec_x;
            vec_y = -vec_y;
        }

        mx = (double) plsc->wpxscl / (double) plsc->phyxlen;
        my = (double) plsc->wpyscl / (double) plsc->phyylen;

        dev_x = -my * vec_y / mx;
        dev_y = mx * vec_x / my;

        scale = sqrt( ( mx * mx * dev_x * dev_x + my * my * dev_y * dev_y ) /
            ( contlabel_offset * contlabel_offset ) );

        off_x = dev_x / scale;
        off_y = dev_y / scale;

        plptex( tpx + off_x, tpy + off_y, vec_x, vec_y, 0.5, flabel );
        plP_movwor( tpx, tpy );
        ( *lastindex )++;
    }
    else
        plP_movwor( tpx, tpy );
}


// Format  contour labels. Arguments:
// value:  floating point number to be formatted
// string: the formatted label, plptex must be called with it to actually
// print the label
//

static void plfloatlabel( PLFLT value, char *string, PLINT len )
{
    PLINT setpre, precis;
    // form[10] gives enough space for all non-malicious formats.
    // tmpstring[15] gives enough room for 3 digits in a negative exponent
    // or 4 digits in a positive exponent + null termination.  That
    // should be enough for all non-malicious use.
    // Obviously there are security issues here that
    // should be addressed as well.
    //
#define FORM_LEN         10
#define TMPSTRING_LEN    15
    char  form[FORM_LEN], tmpstring[TMPSTRING_LEN];
    PLINT exponent = 0;
    PLFLT mant, tmp;

    PLINT prec = sigprec;

    plP_gprec( &setpre, &precis );

    if ( setpre )
        prec = precis;

    if ( value > 0.0 )
        tmp = log10( value );
    else if ( value < 0.0 )
        tmp = log10( -value );
    else
        tmp = 0;

    if ( tmp >= 0.0 )
        exponent = (int) tmp;
    else if ( tmp < 0.0 )
    {
        tmp = -tmp;
        if ( floor( tmp ) < tmp )
            exponent = -(int) ( floor( tmp ) + 1.0 );
        else
            exponent = -(int) ( floor( tmp ) );
    }

    mant = value / pow( 10.0, exponent );

    if ( mant != 0.0 )
        mant = (int) ( mant * pow( 10.0, prec - 1 ) + 0.5 * mant / fabs( mant ) ) / pow( 10.0, prec - 1 );

    snprintf( form, FORM_LEN, "%%.%df", prec - 1 );
    snprintf( string, (size_t) len, form, mant );
    snprintf( tmpstring, TMPSTRING_LEN, "#(229)10#u%d", exponent );
    strncat( string, tmpstring, (size_t) len - strlen( string ) - 1 );

    if ( abs( exponent ) < limexp || value == 0.0 )
    {
        value = pow( 10.0, exponent ) * mant;

        if ( exponent >= 0 )
            prec = prec - 1 - exponent;
        else
            prec = prec - 1 + abs( exponent );

        if ( prec < 0 )
            prec = 0;

        snprintf( form, FORM_LEN, "%%.%df", (int) prec );
        snprintf( string, (size_t) len, form, value );
    }
}

// physical coords (x) to world coords

static PLFLT
plP_pcwcx( PLINT x )
{
    return ( ( x - plsc->wpxoff ) / plsc->wpxscl );
}

// physical coords (y) to world coords

static PLFLT
plP_pcwcy( PLINT y )
{
    return ( ( y - plsc->wpyoff ) / plsc->wpyscl );
}

//--------------------------------------------------------------------------
// plf2eval1()
//
// Does a lookup from a 2d function array.  Array is of type (PLFLT **),
// and is column dominant (normal C ordering).
//--------------------------------------------------------------------------

PLFLT
plf2eval1( PLINT ix, PLINT iy, PLPointer plf2eval_data )
{
    PLFLT value;
    PLFLT **z = (PLFLT **) plf2eval_data;

    value = z[ix][iy];

    return value;
}

//--------------------------------------------------------------------------
// plf2eval2()
//
// Does a lookup from a 2d function array.  plf2eval_data is treated as type
// (PLfGrid2 *).
//--------------------------------------------------------------------------

PLFLT
plf2eval2( PLINT ix, PLINT iy, PLPointer plf2eval_data )
{
    PLFLT    value;
    PLfGrid2 *grid = (PLfGrid2 *) plf2eval_data;

    value = grid->f[ix][iy];

    return value;
}

//--------------------------------------------------------------------------
// plf2eval()
//
// Does a lookup from a 2d function array.  Array is of type (PLFLT *), and
// is column dominant (normal C ordering).  You MUST fill the ny maximum
// array index entry in the PLfGrid struct.
//--------------------------------------------------------------------------

PLFLT
plf2eval( PLINT ix, PLINT iy, PLPointer plf2eval_data )
{
    PLFLT   value;
    PLfGrid *grid = (PLfGrid *) plf2eval_data;

    value = grid->f[ix * grid->ny + iy];

    return value;
}

//--------------------------------------------------------------------------
// plf2evalr()
//
// Does a lookup from a 2d function array.  Array is of type (PLFLT *), and
// is row dominant (Fortran ordering).  You MUST fill the nx maximum array
// index entry in the PLfGrid struct.
//--------------------------------------------------------------------------

PLFLT
plf2evalr( PLINT ix, PLINT iy, PLPointer plf2eval_data )
{
    PLFLT   value;
    PLfGrid *grid = (PLfGrid *) plf2eval_data;

    value = grid->f[ix + iy * grid->nx];

    return value;
}

//--------------------------------------------------------------------------
//
// cont_store:
//
// Draw contour lines in memory.
// cont_clean_store() must be called after use to release allocated memory.
//
//--------------------------------------------------------------------------

void
cont_store( PLFLT_MATRIX f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
            PLINT ky, PLINT ly, PLFLT_VECTOR clevel, PLINT nlevel,
            PLTRANSFORM_callback pltr, PLPointer pltr_data,
            CONT_LEVEL **contour )
{
    cont3d = 1;

    plcont( f, nx, ny, kx, lx, ky, ly, clevel, nlevel,
        pltr, pltr_data );

    *contour = startlev;
    cont3d   = 0;
}

//--------------------------------------------------------------------------
// void plcont()
//
// Draws a contour plot from data in f(nx,ny).  Is just a front-end to
// plfcont, with a particular choice for f2eval and f2eval_data.
//--------------------------------------------------------------------------

void
c_plcont( PLFLT_MATRIX f, PLINT nx, PLINT ny, PLINT kx, PLINT lx,
          PLINT ky, PLINT ly, PLFLT_VECTOR clevel, PLINT nlevel,
          PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plfcont( plf2eval1, (PLPointer) f,
        nx, ny, kx, lx, ky, ly, clevel, nlevel,
        pltr, pltr_data );
}

//--------------------------------------------------------------------------
// void plfcont()
//
// Draws a contour plot using the function evaluator f2eval and data stored
// by way of the f2eval_data pointer.  This allows arbitrary organizations
// of 2d array data to be used.
//
// The subrange of indices used for contouring is kx to lx in the x
// direction and from ky to ly in the y direction. The array of contour
// levels is clevel(nlevel), and "pltr" is the name of a function which
// transforms array indicies into world coordinates.
//
// Note that the fortran-like minimum and maximum indices (kx, lx, ky, ly)
// are translated into more C-like ones.  I've only kept them as they are
// for the plfcont() argument list because of backward compatibility.
//--------------------------------------------------------------------------

void
plfcont( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
         PLINT nx, PLINT ny, PLINT kx, PLINT lx,
         PLINT ky, PLINT ly, PLFLT_VECTOR clevel, PLINT nlevel,
         PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLINT i, **ipts;

    if ( pltr == NULL )
    {
        // If pltr is undefined, abort with an error.
        plabort( "plfcont: The pltr callback must be defined" );
        return;
    }

    if ( kx < 1 || kx >= lx )
    {
        plabort( "plfcont: indices must satisfy  1 <= kx <= lx <= nx" );
        return;
    }
    if ( ky < 1 || ky >= ly )
    {
        plabort( "plfcont: indices must satisfy  1 <= ky <= ly <= ny" );
        return;
    }

    if ( ( ipts = (PLINT **) malloc( (size_t) nx * sizeof ( PLINT * ) ) ) == NULL )
    {
        plexit( "plfcont: Insufficient memory" );
    }

    for ( i = 0; i < nx; i++ )
    {
        if ( ( ipts[i] = (PLINT *) malloc( (size_t) ny * sizeof ( PLINT * ) ) ) == NULL )
        {
            plexit( "plfcont: Insufficient memory" );
        }
    }

    for ( i = 0; i < nlevel; i++ )
    {
        plcntr( f2eval, f2eval_data,
            nx, ny, kx - 1, lx - 1, ky - 1, ly - 1, clevel[i], ipts,
            pltr, pltr_data );

        if ( error )
        {
            error = 0;
            goto done;
        }
    }

done:
    for ( i = 0; i < nx; i++ )
    {
        free( (void *) ipts[i] );
    }
    free( (void *) ipts );
}

//--------------------------------------------------------------------------
// void plcntr()
//
// The contour for a given level is drawn here.  Note iscan has nx
// elements. ixstor and iystor each have nstor elements.
//--------------------------------------------------------------------------

static void
plcntr( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
        PLINT nx, PLINT ny, PLINT kx, PLINT lx,
        PLINT ky, PLINT ly, PLFLT flev, PLINT **ipts,
        PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLINT kcol, krow, lastindex;
    PLFLT distance;
    PLFLT save_def, save_scale;

    char  flabel[30];
    plgchr( &save_def, &save_scale );
    save_scale = save_scale / save_def;

    cont_new_store( flev );

    // format contour label for plptex and define the font height of the labels
    plfloatlabel( flev, flabel, 30 );
    plschr( 0.0, contlabel_size );

    // Clear array for traversed squares
    for ( kcol = kx; kcol < lx; kcol++ )
    {
        for ( krow = ky; krow < ly; krow++ )
        {
            ipts[kcol][krow] = 0;
        }
    }


    for ( krow = ky; krow < ly; krow++ )
    {
        for ( kcol = kx; kcol < lx; kcol++ )
        {
            if ( ipts[kcol][krow] == 0 )
            {
                // Follow and draw a contour
                pldrawcn( f2eval, f2eval_data,
                    nx, ny, kx, lx, ky, ly, flev, flabel, kcol, krow,
                    0.0, 0.0, -2, ipts, &distance, &lastindex,
                    pltr, pltr_data );

                if ( error )
                    return;
            }
        }
    }
    plschr( save_def, save_scale );
}

//--------------------------------------------------------------------------
// void pldrawcn()
//
// Follow and draw a contour.
//--------------------------------------------------------------------------

static void
pldrawcn( PLF2EVAL_callback f2eval, PLPointer f2eval_data,
          PLINT nx, PLINT ny, PLINT kx, PLINT lx,
          PLINT ky, PLINT ly, PLFLT flev, char *flabel, PLINT kcol, PLINT krow,
          PLFLT lastx, PLFLT lasty, PLINT startedge, PLINT **ipts,
          PLFLT *distance, PLINT *lastindex,
          PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLFLT f[4];
    PLFLT px[4], py[4], locx[4], locy[4];
    PLINT iedge[4];
    PLINT i, j, k, num, first, inext, kcolnext, krownext, sfi, sfj;


    ( *pltr )( kcol, krow + 1, &px[0], &py[0], pltr_data );
    ( *pltr )( kcol, krow, &px[1], &py[1], pltr_data );
    ( *pltr )( kcol + 1, krow, &px[2], &py[2], pltr_data );
    ( *pltr )( kcol + 1, krow + 1, &px[3], &py[3], pltr_data );

    f[0] = f2eval( kcol, krow + 1, f2eval_data ) - flev;
    f[1] = f2eval( kcol, krow, f2eval_data ) - flev;
    f[2] = f2eval( kcol + 1, krow, f2eval_data ) - flev;
    f[3] = f2eval( kcol + 1, krow + 1, f2eval_data ) - flev;

    for ( i = 0, j = 1; i < 4; i++, j = ( j + 1 ) % 4 )
    {
        // Use intermediates to avoid possible floating point
        // under / over flow during multiplication.
        sfi      = ( f[i] > 0.0 ) ? 1 : ( ( f[i] < 0.0 ) ? -1 : 0 );
        sfj      = ( f[j] > 0.0 ) ? 1 : ( ( f[j] < 0.0 ) ? -1 : 0 );
        iedge[i] = ( sfi * sfj > 0 ) ? -1 : ( ( sfi * sfj < 0 ) ? 1 : 0 );
    }

    // Mark this square as done
    ipts[kcol][krow] = 1;

    // Check if no contour has been crossed i.e. iedge[i] = -1
    if ( ( iedge[0] == -1 ) && ( iedge[1] == -1 ) && ( iedge[2] == -1 )
         && ( iedge[3] == -1 ) )
        return;

    // Check if this is a completely flat square - in which case
    // ignore it
    if ( ( f[0] == 0.0 ) && ( f[1] == 0.0 ) && ( f[2] == 0.0 ) &&
         ( f[3] == 0.0 ) )
        return;

    // Calculate intersection points
    num = 0;
    if ( startedge < 0 )
    {
        first = 1;
    }
    else
    {
        locx[num] = lastx;
        locy[num] = lasty;
        num++;
        first = 0;
    }
    for ( k = 0, i = ( startedge < 0 ? 0 : startedge ); k < 4; k++, i = ( i + 1 ) % 4 )
    {
        if ( i == startedge )
            continue;

        // If the contour is an edge check it hasn't already been done
        if ( f[i] == 0.0 && f[( i + 1 ) % 4] == 0.0 )
        {
            kcolnext = kcol;
            krownext = krow;
            if ( i == 0 )
                kcolnext--;
            if ( i == 1 )
                krownext--;
            if ( i == 2 )
                kcolnext++;
            if ( i == 3 )
                krownext++;
            if ( ( kcolnext < kx ) || ( kcolnext >= lx ) ||
                 ( krownext < ky ) || ( krownext >= ly ) ||
                 ( ipts[kcolnext][krownext] == 1 ) )
                continue;
        }
        if ( ( iedge[i] == 1 ) || ( f[i] == 0.0 ) )
        {
            j = ( i + 1 ) % 4;
            if ( f[i] != 0.0 )
            {
                locx[num] = ( px[i] * fabs( f[j] ) + px[j] * fabs( f[i] ) ) / fabs( f[j] - f[i] );
                locy[num] = ( py[i] * fabs( f[j] ) + py[j] * fabs( f[i] ) ) / fabs( f[j] - f[i] );
            }
            else
            {
                locx[num] = px[i];
                locy[num] = py[i];
            }
            // If this is the start of the contour then move to the point
            if ( first == 1 )
            {
                cont_mv_store( locx[num], locy[num] );
                first      = 0;
                *distance  = 0;
                *lastindex = 0;
            }
            else
            {
                // Link to the next point on the contour
                if ( contlabel_active )
                    pl_drawcontlabel( locx[num], locy[num], flabel, distance, lastindex );
                else
                    cont_xy_store( locx[num], locy[num] );
                // Need to follow contour into next grid box
                // Easy case where contour does not pass through corner
                if ( f[i] != 0.0 )
                {
                    kcolnext = kcol;
                    krownext = krow;
                    inext    = ( i + 2 ) % 4;
                    if ( i == 0 )
                        kcolnext--;
                    if ( i == 1 )
                        krownext--;
                    if ( i == 2 )
                        kcolnext++;
                    if ( i == 3 )
                        krownext++;
                    if ( ( kcolnext >= kx ) && ( kcolnext < lx ) &&
                         ( krownext >= ky ) && ( krownext < ly ) &&
                         ( ipts[kcolnext][krownext] == 0 ) )
                    {
                        pldrawcn( f2eval, f2eval_data,
                            nx, ny, kx, lx, ky, ly, flev, flabel,
                            kcolnext, krownext,
                            locx[num], locy[num], inext, ipts,
                            distance, lastindex,
                            pltr, pltr_data );
                    }
                }
                // Hard case where contour passes through corner
                // This is still not perfect - it may lose the contour
                // which won't upset the contour itself (we can find it
                // again later) but might upset the labelling
                else
                {
                    kcolnext = kcol;
                    krownext = krow;
                    inext    = ( i + 2 ) % 4;
                    if ( i == 0 )
                    {
                        kcolnext--; krownext++;
                    }
                    if ( i == 1 )
                    {
                        krownext--; kcolnext--;
                    }
                    if ( i == 2 )
                    {
                        kcolnext++; krownext--;
                    }
                    if ( i == 3 )
                    {
                        krownext++; kcolnext++;
                    }
                    if ( ( kcolnext >= kx ) && ( kcolnext < lx ) &&
                         ( krownext >= ky ) && ( krownext < ly ) &&
                         ( ipts[kcolnext][krownext] == 0 ) )
                    {
                        pldrawcn( f2eval, f2eval_data,
                            nx, ny, kx, lx, ky, ly, flev, flabel,
                            kcolnext, krownext,
                            locx[num], locy[num], inext, ipts,
                            distance, lastindex,
                            pltr, pltr_data );
                    }
                }
                if ( first == 1 )
                {
                    // Move back to first point
                    cont_mv_store( locx[num], locy[num] );
                    first      = 0;
                    *distance  = 0;
                    *lastindex = 0;
                    first      = 0;
                }
                else
                {
                    first = 1;
                }
                num++;
            }
        }
    }
}

//--------------------------------------------------------------------------
// pltr0()
//
// Identity transformation.
//--------------------------------------------------------------------------

void
pltr0( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer PL_UNUSED( pltr_data ) )
{
    *tx = x;
    *ty = y;
}

//--------------------------------------------------------------------------
// pltr1()
//
// Does linear interpolation from singly dimensioned coord arrays.
//
// Just abort for now if coordinates are out of bounds (don't think it's
// possible, but if so we could use linear extrapolation).
//--------------------------------------------------------------------------

void
pltr1( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data )
{
    PLINT   ul, ur, vl, vr;
    PLFLT   du, dv;
    PLFLT   xl, xr, yl, yr;

    PLcGrid *grid = (PLcGrid *) pltr_data;
    PLFLT   *xg   = grid->xg;
    PLFLT   *yg   = grid->yg;
    PLINT   nx    = grid->nx;
    PLINT   ny    = grid->ny;

    ul = (PLINT) x;
    ur = ul + 1;
    du = x - ul;

    vl = (PLINT) y;
    vr = vl + 1;
    dv = y - vl;

    if ( x < 0 || x > nx - 1 || y < 0 || y > ny - 1 )
    {
        plexit( "pltr1: Invalid coordinates" );
    }

// Look up coordinates in row-dominant array.
// Have to handle right boundary specially -- if at the edge, we'd better
// not reference the out of bounds point.
//

    xl = xg[ul];
    yl = yg[vl];

    if ( ur == nx )
    {
        *tx = xl;
    }
    else
    {
        xr  = xg[ur];
        *tx = xl * ( 1 - du ) + xr * du;
    }
    if ( vr == ny )
    {
        *ty = yl;
    }
    else
    {
        yr  = yg[vr];
        *ty = yl * ( 1 - dv ) + yr * dv;
    }
}

//--------------------------------------------------------------------------
// pltr2()
//
// Does linear interpolation from doubly dimensioned coord arrays (column
// dominant, as per normal C 2d arrays).
//
// This routine includes lots of checks for out of bounds.  This would occur
// occasionally due to some bugs in the contour plotter (now fixed).  If an
// out of bounds coordinate is obtained, the boundary value is provided
// along with a warning.  These checks should stay since no harm is done if
// if everything works correctly.
//--------------------------------------------------------------------------

void
pltr2( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data )
{
    PLINT    ul, ur, vl, vr;
    PLFLT    du, dv;
    PLFLT    xll, xlr, xrl, xrr;
    PLFLT    yll, ylr, yrl, yrr;
    PLFLT    xmin, xmax, ymin, ymax;

    PLcGrid2 *grid = (PLcGrid2 *) pltr_data;
    PLFLT    **xg  = grid->xg;
    PLFLT    **yg  = grid->yg;
    PLINT    nx    = grid->nx;
    PLINT    ny    = grid->ny;

    ul = (PLINT) x;
    ur = ul + 1;
    du = x - ul;

    vl = (PLINT) y;
    vr = vl + 1;
    dv = y - vl;

    xmin = 0;
    xmax = nx - 1;
    ymin = 0;
    ymax = ny - 1;

    if ( x < xmin || x > xmax || y < ymin || y > ymax )
    {
        plwarn( "pltr2: Invalid coordinates" );
        if ( x < xmin )
        {
            if ( y < ymin )
            {
                *tx = xg[0][0];
                *ty = yg[0][0];
            }
            else if ( y > ymax )
            {
                *tx = xg[0][ny - 1];
                *ty = yg[0][ny - 1];
            }
            else
            {
                xll = xg[0][vl];
                yll = yg[0][vl];
                xlr = xg[0][vr];
                ylr = yg[0][vr];

                *tx = xll * ( 1 - dv ) + xlr * ( dv );
                *ty = yll * ( 1 - dv ) + ylr * ( dv );
            }
        }
        else if ( x > xmax )
        {
            if ( y < ymin )
            {
                *tx = xg[nx - 1][0];
                *ty = yg[nx - 1][0];
            }
            else if ( y > ymax )
            {
                *tx = xg[nx - 1][ny - 1];
                *ty = yg[nx - 1][ny - 1];
            }
            else
            {
                xll = xg[nx - 1][vl];
                yll = yg[nx - 1][vl];
                xlr = xg[nx - 1][vr];
                ylr = yg[nx - 1][vr];

                *tx = xll * ( 1 - dv ) + xlr * ( dv );
                *ty = yll * ( 1 - dv ) + ylr * ( dv );
            }
        }
        else
        {
            if ( y < ymin )
            {
                xll = xg[ul][0];
                xrl = xg[ur][0];
                yll = yg[ul][0];
                yrl = yg[ur][0];

                *tx = xll * ( 1 - du ) + xrl * ( du );
                *ty = yll * ( 1 - du ) + yrl * ( du );
            }
            else if ( y > ymax )
            {
                xlr = xg[ul][ny - 1];
                xrr = xg[ur][ny - 1];
                ylr = yg[ul][ny - 1];
                yrr = yg[ur][ny - 1];

                *tx = xlr * ( 1 - du ) + xrr * ( du );
                *ty = ylr * ( 1 - du ) + yrr * ( du );
            }
        }
    }

// Normal case.
// Look up coordinates in row-dominant array.
// Have to handle right boundary specially -- if at the edge, we'd
// better not reference the out of bounds point.
//

    else
    {
        xll = xg[ul][vl];
        yll = yg[ul][vl];

        // ur is out of bounds

        if ( ur == nx && vr < ny )
        {
            xlr = xg[ul][vr];
            ylr = yg[ul][vr];

            *tx = xll * ( 1 - dv ) + xlr * ( dv );
            *ty = yll * ( 1 - dv ) + ylr * ( dv );
        }

        // vr is out of bounds

        else if ( ur < nx && vr == ny )
        {
            xrl = xg[ur][vl];
            yrl = yg[ur][vl];

            *tx = xll * ( 1 - du ) + xrl * ( du );
            *ty = yll * ( 1 - du ) + yrl * ( du );
        }

        // both ur and vr are out of bounds

        else if ( ur == nx && vr == ny )
        {
            *tx = xll;
            *ty = yll;
        }

        // everything in bounds

        else
        {
            xrl = xg[ur][vl];
            xlr = xg[ul][vr];
            xrr = xg[ur][vr];

            yrl = yg[ur][vl];
            ylr = yg[ul][vr];
            yrr = yg[ur][vr];

            *tx = xll * ( 1 - du ) * ( 1 - dv ) + xlr * ( 1 - du ) * ( dv ) +
                  xrl * ( du ) * ( 1 - dv ) + xrr * ( du ) * ( dv );

            *ty = yll * ( 1 - du ) * ( 1 - dv ) + ylr * ( 1 - du ) * ( dv ) +
                  yrl * ( du ) * ( 1 - dv ) + yrr * ( du ) * ( dv );
        }
    }
}

//--------------------------------------------------------------------------
// pltr2p()
//
// Just like pltr2() but uses pointer arithmetic to get coordinates from 2d
// grid tables.  This form of grid tables is compatible with those from
// PLplot 4.0.  The grid data must be pointed to by a PLcGrid structure.
//--------------------------------------------------------------------------

void
pltr2p( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, PLPointer pltr_data )
{
    PLINT   ul, ur, vl, vr;
    PLFLT   du, dv;
    PLFLT   xll, xlr, xrl, xrr;
    PLFLT   yll, ylr, yrl, yrr;
    PLFLT   xmin, xmax, ymin, ymax;

    PLcGrid *grid = (PLcGrid *) pltr_data;
    PLFLT   *xg   = grid->xg;
    PLFLT   *yg   = grid->yg;
    PLINT   nx    = grid->nx;
    PLINT   ny    = grid->ny;

    ul = (PLINT) x;
    ur = ul + 1;
    du = x - ul;

    vl = (PLINT) y;
    vr = vl + 1;
    dv = y - vl;

    xmin = 0;
    xmax = nx - 1;
    ymin = 0;
    ymax = ny - 1;

    if ( x < xmin || x > xmax || y < ymin || y > ymax )
    {
        plwarn( "pltr2p: Invalid coordinates" );
        if ( x < xmin )
        {
            if ( y < ymin )
            {
                *tx = *xg;
                *ty = *yg;
            }
            else if ( y > ymax )
            {
                *tx = *( xg + ( ny - 1 ) );
                *ty = *( yg + ( ny - 1 ) );
            }
            else
            {
                ul  = 0;
                xll = *( xg + ul * ny + vl );
                yll = *( yg + ul * ny + vl );
                xlr = *( xg + ul * ny + vr );
                ylr = *( yg + ul * ny + vr );

                *tx = xll * ( 1 - dv ) + xlr * ( dv );
                *ty = yll * ( 1 - dv ) + ylr * ( dv );
            }
        }
        else if ( x > xmax )
        {
            if ( y < ymin )
            {
                *tx = *( xg + ( ny - 1 ) * nx );
                *ty = *( yg + ( ny - 1 ) * nx );
            }
            else if ( y > ymax )
            {
                *tx = *( xg + ( ny - 1 ) + ( nx - 1 ) * ny );
                *ty = *( yg + ( ny - 1 ) + ( nx - 1 ) * ny );
            }
            else
            {
                ul  = nx - 1;
                xll = *( xg + ul * ny + vl );
                yll = *( yg + ul * ny + vl );
                xlr = *( xg + ul * ny + vr );
                ylr = *( yg + ul * ny + vr );

                *tx = xll * ( 1 - dv ) + xlr * ( dv );
                *ty = yll * ( 1 - dv ) + ylr * ( dv );
            }
        }
        else
        {
            if ( y < ymin )
            {
                vl  = 0;
                xll = *( xg + ul * ny + vl );
                xrl = *( xg + ur * ny + vl );
                yll = *( yg + ul * ny + vl );
                yrl = *( yg + ur * ny + vl );

                *tx = xll * ( 1 - du ) + xrl * ( du );
                *ty = yll * ( 1 - du ) + yrl * ( du );
            }
            else if ( y > ymax )
            {
                vr  = ny - 1;
                xlr = *( xg + ul * ny + vr );
                xrr = *( xg + ur * ny + vr );
                ylr = *( yg + ul * ny + vr );
                yrr = *( yg + ur * ny + vr );

                *tx = xlr * ( 1 - du ) + xrr * ( du );
                *ty = ylr * ( 1 - du ) + yrr * ( du );
            }
        }
    }

// Normal case.
// Look up coordinates in row-dominant array.
// Have to handle right boundary specially -- if at the edge, we'd better
// not reference the out of bounds point.
//

    else
    {
        xll = *( xg + ul * ny + vl );
        yll = *( yg + ul * ny + vl );

        // ur is out of bounds

        if ( ur == nx && vr < ny )
        {
            xlr = *( xg + ul * ny + vr );
            ylr = *( yg + ul * ny + vr );

            *tx = xll * ( 1 - dv ) + xlr * ( dv );
            *ty = yll * ( 1 - dv ) + ylr * ( dv );
        }

        // vr is out of bounds

        else if ( ur < nx && vr == ny )
        {
            xrl = *( xg + ur * ny + vl );
            yrl = *( yg + ur * ny + vl );

            *tx = xll * ( 1 - du ) + xrl * ( du );
            *ty = yll * ( 1 - du ) + yrl * ( du );
        }

        // both ur and vr are out of bounds

        else if ( ur == nx && vr == ny )
        {
            *tx = xll;
            *ty = yll;
        }

        // everything in bounds

        else
        {
            xrl = *( xg + ur * ny + vl );
            xlr = *( xg + ul * ny + vr );
            xrr = *( xg + ur * ny + vr );

            yrl = *( yg + ur * ny + vl );
            ylr = *( yg + ul * ny + vr );
            yrr = *( yg + ur * ny + vr );

            *tx = xll * ( 1 - du ) * ( 1 - dv ) + xlr * ( 1 - du ) * ( dv ) +
                  xrl * ( du ) * ( 1 - dv ) + xrr * ( du ) * ( dv );

            *ty = yll * ( 1 - du ) * ( 1 - dv ) + ylr * ( 1 - du ) * ( dv ) +
                  yrl * ( du ) * ( 1 - dv ) + yrr * ( du ) * ( dv );
        }
    }
}

//--------------------------------------------------------------------------
// pltr2f()
//
// Does linear interpolation from doubly dimensioned coord arrays
// (row dominant, i.e. Fortran ordering).
//
// This routine includes lots of checks for out of bounds.  This would
// occur occasionally due to a bug in the contour plotter that is now fixed.
// If an out of bounds coordinate is obtained, the boundary value is provided
// along with a warning.  These checks should stay since no harm is done if
// if everything works correctly.
//--------------------------------------------------------------------------

void
pltr2f( PLFLT x, PLFLT y, PLFLT *tx, PLFLT *ty, void *pltr_data )
{
    PLINT   ul, ur, vl, vr;
    PLFLT   du, dv;
    PLFLT   xll, xlr, xrl, xrr;
    PLFLT   yll, ylr, yrl, yrr;
    PLFLT   xmin, xmax, ymin, ymax;

    PLcGrid *cgrid = (PLcGrid *) pltr_data;
    PLFLT   *xg    = cgrid->xg;
    PLFLT   *yg    = cgrid->yg;
    PLINT   nx     = cgrid->nx;
    PLINT   ny     = cgrid->ny;

    ul = (PLINT) x;
    ur = ul + 1;
    du = x - ul;

    vl = (PLINT) y;
    vr = vl + 1;
    dv = y - vl;

    xmin = 0;
    xmax = nx - 1;
    ymin = 0;
    ymax = ny - 1;

    if ( x < xmin || x > xmax || y < ymin || y > ymax )
    {
        plwarn( "pltr2f: Invalid coordinates" );

        if ( x < xmin )
        {
            if ( y < ymin )
            {
                *tx = *xg;
                *ty = *yg;
            }
            else if ( y > ymax )
            {
                *tx = *( xg + ( ny - 1 ) * nx );
                *ty = *( yg + ( ny - 1 ) * nx );
            }
            else
            {
                ul  = 0;
                xll = *( xg + ul + vl * nx );
                yll = *( yg + ul + vl * nx );
                xlr = *( xg + ul + vr * nx );
                ylr = *( yg + ul + vr * nx );

                *tx = xll * ( 1 - dv ) + xlr * ( dv );
                *ty = yll * ( 1 - dv ) + ylr * ( dv );
            }
        }
        else if ( x > xmax )
        {
            if ( y < ymin )
            {
                *tx = *( xg + ( nx - 1 ) );
                *ty = *( yg + ( nx - 1 ) );
            }
            else if ( y > ymax )
            {
                *tx = *( xg + ( nx - 1 ) + ( ny - 1 ) * nx );
                *ty = *( yg + ( nx - 1 ) + ( ny - 1 ) * nx );
            }
            else
            {
                ul  = nx - 1;
                xll = *( xg + ul + vl * nx );
                yll = *( yg + ul + vl * nx );
                xlr = *( xg + ul + vr * nx );
                ylr = *( yg + ul + vr * nx );

                *tx = xll * ( 1 - dv ) + xlr * ( dv );
                *ty = yll * ( 1 - dv ) + ylr * ( dv );
            }
        }
        else
        {
            if ( y < ymin )
            {
                vl  = 0;
                xll = *( xg + ul + vl * nx );
                xrl = *( xg + ur + vl * nx );
                yll = *( yg + ul + vl * nx );
                yrl = *( yg + ur + vl * nx );

                *tx = xll * ( 1 - du ) + xrl * ( du );
                *ty = yll * ( 1 - du ) + yrl * ( du );
            }
            else if ( y > ymax )
            {
                vr  = ny - 1;
                xlr = *( xg + ul + vr * nx );
                xrr = *( xg + ur + vr * nx );
                ylr = *( yg + ul + vr * nx );
                yrr = *( yg + ur + vr * nx );

                *tx = xlr * ( 1 - du ) + xrr * ( du );
                *ty = ylr * ( 1 - du ) + yrr * ( du );
            }
        }
    }

// Normal case.
// Look up coordinates in row-dominant array.
// Have to handle right boundary specially -- if at the edge, we'd
// better not reference the out of bounds point.

    else
    {
        xll = *( xg + ul + vl * nx );
        yll = *( yg + ul + vl * nx );

// ur is out of bounds

        if ( ur == nx && vr < ny )
        {
            xlr = *( xg + ul + vr * nx );
            ylr = *( yg + ul + vr * nx );

            *tx = xll * ( 1 - dv ) + xlr * ( dv );
            *ty = yll * ( 1 - dv ) + ylr * ( dv );
        }

// vr is out of bounds

        else if ( ur < nx && vr == ny )
        {
            xrl = *( xg + ur + vl * nx );
            yrl = *( yg + ur + vl * nx );

            *tx = xll * ( 1 - du ) + xrl * ( du );
            *ty = yll * ( 1 - du ) + yrl * ( du );
        }

// both ur and vr are out of bounds

        else if ( ur == nx && vr == ny )
        {
            *tx = xll;
            *ty = yll;
        }

// everything in bounds

        else
        {
            xrl = *( xg + ur + vl * nx );
            xlr = *( xg + ul + vr * nx );
            xrr = *( xg + ur + vr * nx );

            yrl = *( yg + ur + vl * nx );
            ylr = *( yg + ul + vr * nx );
            yrr = *( yg + ur + vr * nx );
            *tx = xll * ( 1 - du ) * ( 1 - dv ) + xlr * ( 1 - du ) * ( dv ) +
                  xrl * ( du ) * ( 1 - dv ) + xrr * ( du ) * ( dv );

            *ty = yll * ( 1 - du ) * ( 1 - dv ) + ylr * ( 1 - du ) * ( dv ) +
                  yrl * ( du ) * ( 1 - dv ) + yrr * ( du ) * ( dv );
        }
    }
}
