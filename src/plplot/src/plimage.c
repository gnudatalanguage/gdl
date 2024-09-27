// plimage()
//
// Author: Alessandro Mirone, Nov 2001
// Adapted: Joao Cardoso
// Updated: Hezekiah Carty 2008
//
// Copyright (C) 2004  Alan W. Irwin
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

#define COLOR_MIN        0.0
#define COLOR_MAX        1.0
#define COLOR_NO_PLOT    ( -1.0 )

// Get better names, those are too cryptic!
//
// ZEROW2B: zero writing to buffer ?
// ZEROW2D: zero writing to display ?
// ONEW2B: set writing to buffer ?
// ONEW2D: set writing to display ?
//

void
NoBufferNoPixmap()
{
    PLINT op = ZEROW2B;

    plsc->plbuf_write = 0; // TODO: store previous state
    plP_esc( PLESC_EXPOSE, NULL );
    plP_esc( PLESC_IMAGEOPS, &op );
}

void
RestoreWrite2BufferPixmap()
{
    PLINT op = ONEW2B;

    plsc->plbuf_write = 1; // TODO: revert from previous state
    plP_esc( PLESC_IMAGEOPS, &op );
}

//
// Unused functions - comment out
//
//void
//disabledisplay()
//{
//    PLINT op = ZEROW2D;
//
//    plP_esc( PLESC_IMAGEOPS, &op );
//}
//
//void
//enabledisplay()
//{
//    PLINT op = ONEW2D;
//
//    plP_esc( PLESC_IMAGEOPS, &op );
//    plP_esc( PLESC_EXPOSE, NULL );
//}
//


//
// NOTE: The plshade* functions require that both pltr and pltr_data are set
// in order for pltr to be used.  plimageslow does NOT require this, so it is
// up to the user to make sure pltr_data is something non-NULL if pltr
// requires it.
// Plottable values in idata must be scaled between COLOR_MIN and COLOR_MAX.
// This is an internal function, and should not be used directly.  Its
// interface may change.
//
void
plimageslow( PLFLT *idata, PLINT nx, PLINT ny,
             PLFLT xmin, PLFLT ymin, PLFLT dx, PLFLT dy,
             PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    // Indices
    PLINT ix, iy, i;
    // Float coordinates
    PLFLT xf[4], yf[4];
    // Translated (by pltr) coordinates
    PLFLT tx, ty;
    // The corners of a single filled region
    // int corners[4]; - unreferenced
    // The color to use in the fill
    PLFLT color;

    plP_esc( PLESC_START_RASTERIZE, NULL );
    for ( ix = 0; ix < nx; ix++ )
    {
        for ( iy = 0; iy < ny; iy++ )
        {
            // Only plot values within in appropriate range
            color = idata[ix * ny + iy];
            if ( color == COLOR_NO_PLOT )
                continue;

            // The color value has to be scaled to 0.0 -> 1.0 plcol1 color values
            plcol1( color / COLOR_MAX );

            xf[0] = xf[1] = ix;
            xf[2] = xf[3] = ix + 1;
            yf[0] = yf[3] = iy;
            yf[1] = yf[2] = iy + 1;

            if ( pltr )
            {
                for ( i = 0; i < 4; i++ )
                {
                    // Translate the points
                    ( *pltr )( xf[i], yf[i], &tx, &ty, pltr_data );
                    xf[i] = tx;
                    yf[i] = ty;
                }
            }
            else
            {
                for ( i = 0; i < 4; i++ )
                {
                    // Automatic translation to the specified plot area
                    xf[i] = xmin + xf[i] * dx;
                    yf[i] = ymin + yf[i] * dy;
                }
            }
            plfill( 4, xf, yf );
        }
    }
    plP_esc( PLESC_END_RASTERIZE, NULL );
}

void
grimage( short *x, short *y, unsigned short *z, PLINT nx, PLINT ny )
{
    plsc->dev_ix    = x;
    plsc->dev_iy    = y;
    plsc->dev_z     = z;
    plsc->dev_nptsX = nx;
    plsc->dev_nptsY = ny;

    plP_esc( PLESC_IMAGE, NULL );
}

//--------------------------------------------------------------------------
// plimagefr_null
//
// arguments are
//   idata: array containing image data
//   nx: dimension of the array in the X axis.
//   ny: dimension of the  array in the Y axis
//   The array data is indexed like data[ix][iy]
//
//   xmin, xmax, ymin, ymax:
//       data[0][0] corresponds to (xmin,ymin)
//       data[nx-1][ny-1] to (xmax,ymax)
//
//   zmin, zmax:
//       only data within bounds zmin <= data <= zmax will be
//       plotted. If zmin == zmax, all data will be ploted.
//
//   valuemin, valuemax:
//       The minimum and maximum values to use for value -> color
//       mappings.  A value in idata of valuemin or less will have
//       color 0.0 and a value in idata of valuemax or greater will
//       have color 1.0.  Values between valuemin and valuemax will
//       map linearly to to the colors between 0.0 and 1.0.
//       If you do not want to display values outside of the
//       (valuemin -> valuemax) range, then set zmin = valuemin and
//       zmax = valuemax.
//       This allows for multiple plots to use the same color scale
//       with a consistent value -> color mapping, regardless of the
//       image content.
//
//--------------------------------------------------------------------------

// N.B. This routine only needed by the Fortran interface to distinguish
// the case where pltr and pltr_data are NULL.  So don't put declaration in
// header which might encourage others to use this in some other context.
PLDLLIMPEXP void
plimagefr_null( PLFLT_MATRIX idata, PLINT nx, PLINT ny,
                PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
                PLFLT valuemin, PLFLT valuemax )
{
    plfimagefr( plf2ops_c(), (PLPointer) idata, nx, ny,
        xmin, xmax, ymin, ymax, zmin, zmax,
        valuemin, valuemax, NULL, NULL );
}

//--------------------------------------------------------------------------
// plimagefr
//
// arguments are
//   idata: array containing image data
//   nx: dimension of the array in the X axis.
//   ny: dimension of the  array in the Y axis
//   The array data is indexed like data[ix][iy]
//
//   xmin, xmax, ymin, ymax:
//       data[0][0] corresponds to (xmin,ymin)
//       data[nx-1][ny-1] to (xmax,ymax)
//
//   zmin, zmax:
//       only data within bounds zmin <= data <= zmax will be
//       plotted. If zmin == zmax, all data will be ploted.
//
//   valuemin, valuemax:
//       The minimum and maximum values to use for value -> color
//       mappings.  A value in idata of valuemin or less will have
//       color 0.0 and a value in idata of valuemax or greater will
//       have color 1.0.  Values between valuemin and valuemax will
//       map linearly to to the colors between 0.0 and 1.0.
//       If you do not want to display values outside of the
//       (valuemin -> valuemax) range, then set zmin = valuemin and
//       zmax = valuemax.
//       This allows for multiple plots to use the same color scale
//       with a consistent value -> color mapping, regardless of the
//       image content.
//
//--------------------------------------------------------------------------
void
c_plimagefr( PLFLT_MATRIX idata, PLINT nx, PLINT ny,
             PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
             PLFLT valuemin, PLFLT valuemax,
             PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    plfimagefr( plf2ops_c(), (PLPointer) idata, nx, ny,
        xmin, xmax, ymin, ymax, zmin, zmax,
        valuemin, valuemax, pltr, pltr_data );
}

void
plfimagefr( PLF2OPS idataops, PLPointer idatap, PLINT nx, PLINT ny,
            PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
            PLFLT valuemin, PLFLT valuemax,
            PLTRANSFORM_callback pltr, PLPointer pltr_data )
{
    PLINT ix, iy;
    PLFLT dx, dy;
    // z holds scaled image pixel values
    PLFLT *z;
    // This is used when looping through the image array, checking to
    // make sure the values are within an acceptable range.
    PLFLT datum;
    // Color palette 0 color in use before the plimage* call
    PLINT init_color;
    // Color range
    PLFLT color_min, color_max, color_range;

    if ( plsc->level < 3 )
    {
        plabort( "plimagefr: window must be set up first" );
        return;
    }

    if ( nx <= 0 || ny <= 0 )
    {
        plabort( "plimagefr: nx and ny must be positive" );
        return;
    }

    if ( ( z = (PLFLT *) malloc( (size_t) ( ny * nx ) * sizeof ( PLFLT ) ) ) == NULL )
    {
        plexit( "plimagefr: Insufficient memory" );
    }

    // Save the currently-in-use color.
    init_color = plsc->icol0;

    // If no acceptable data range is given, then set the min/max data range
    // to include all of the given data.
    if ( zmin == zmax )
    {
        // Find the minimum and maximum values in the image
        idataops->minmax( idatap, nx, ny, &zmin, &zmax );
    }

    // Calculate the size of the color range to use
    color_min   = plsc->cmap1_min;
    color_max   = plsc->cmap1_max;
    color_range = color_max - color_min;

    // Go through the image values and scale them to fit in
    // the COLOR_MIN to COLOR_MAX range.
    // Any values greater than valuemax are set to valuemax,
    // and values less than valuemin are set to valuemin.
    // Any values outside of zmin to zmax are flagged so they
    // are not plotted.
    for ( ix = 0; ix < nx; ix++ )
    {
        for ( iy = 0; iy < ny; iy++ )
        {
            if ( valuemin == valuemax )
            {
                // If valuemin == valuemax, avoid dividing by zero.
                z[ix * ny + iy] = ( color_max + color_min ) / 2.0;
            }
            else
            {
                datum = idataops->get( idatap, ix, iy );
                if ( isnan( datum ) || datum < zmin || datum > zmax )
                {
                    // Set to a guaranteed-not-to-plot value
                    z[ix * ny + iy] = COLOR_NO_PLOT;
                }
                else
                {
                    if ( datum < valuemin )
                    {
                        datum = valuemin;
                    }
                    else if ( datum > valuemax )
                    {
                        datum = valuemax;
                    }
                    // Set to a value scaled between color_min and color_max.
                    z[ix * ny + iy] =
                        color_min + ( datum - valuemin + COLOR_MIN ) / ( valuemax - valuemin ) * COLOR_MAX * color_range;
                }
            }
        }
    }

    // dx and dy are the plot-coordinates pixel sizes for an untransformed
    // image
    dx = ( xmax - xmin ) / (PLFLT) ( nx - 1 );
    dy = ( ymax - ymin ) / (PLFLT) ( ny - 1 );

    plP_image( z, nx, ny, xmin, ymin, dx, dy, pltr, pltr_data );

    plcol0( init_color );

    free( z );
}

//--------------------------------------------------------------------------
// plimage
//
// arguments are
//   idata: array containing image data
//   nx: dimension of the array in the X axis.
//   ny: dimension of the  array in the Y axis
//   The array data is indexed like data[ix][iy]
//
//   xmin, xmax, ymin, ymax:
//       data[0][0] corresponds to (xmin,ymin)
//       data[nx-1][ny-1] to (xmax,ymax)
//
//   zmin, zmax:
//       only data within bounds zmin <= data <= zmax will be
//       plotted. If zmin == zmax, all data will be ploted.
//
//   Dxmin, Dxmax, Dymin, Dymax:
//       plots only the window of points whose(x,y)'s fall
//       inside the [Dxmin->Dxmax]X[Dymin->Dymax] window
//
//--------------------------------------------------------------------------
void
c_plimage( PLFLT_MATRIX idata, PLINT nx, PLINT ny,
           PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
           PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax )
{
    plfimage( plf2ops_c(), (PLPointer) idata, nx, ny,
        xmin, xmax, ymin, ymax, zmin, zmax,
        Dxmin, Dxmax, Dymin, Dymax );
}

void
plfimage( PLF2OPS idataops, PLPointer idatap, PLINT nx, PLINT ny,
          PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin, PLFLT zmax,
          PLFLT Dxmin, PLFLT Dxmax, PLFLT Dymin, PLFLT Dymax )
{
    PLINT   ix, iy, ixx, iyy, xm, ym, nnx, nny;
    PLFLT   data_min, data_max, dx, dy;
    // z holds the subimage (Dxmin, Dymin) - (Dxmax, Dymax)
    PLFLT   **z;
    PLF2OPS zops;
    // Was any space allocated for z?
    PLBOOL  copied;
    copied = FALSE;

    if ( nx <= 0 || ny <= 0 )
    {
        plabort( "plimage: nx and ny must be positive" );
        return;
    }

    if ( Dxmin < xmin || Dxmax > xmax || Dymin < ymin || Dymax > ymax )
    {
        plabort( "plimage: Dxmin or Dxmax or Dymin or Dymax not compatible with xmin or xmax or ymin or ymax." );
        return;
    }

    if ( Dxmax < Dxmin || xmax < xmin || Dymax < Dymin || ymax < ymin )
    {
        plabort( "plimage: All (Dxmin < Dxmax) and (Dymin < Dymax) and (xmin < xmax) and (ymin < ymax) must hold." );
        return;
    }

    // Find the minimum and maximum values in the image.  Use these values to
    // for the color scale range.
    idataops->minmax( idatap, nx, ny, &data_min, &data_max );

    if ( xmin == Dxmin && xmax == Dxmax && ymin == Dymin && ymax == Dymax )
    {
        // If the whole image should be shown, then no copying is needed.
        z    = (PLFLT **) idatap;
        zops = idataops;
        nnx  = nx;
        nny  = ny;
    }
    else
    {
        // dx and dy are the plot-coordinates pixel sizes for an untransformed
        // image
        dx = ( xmax - xmin ) / (PLFLT) ( nx - 1 );
        dy = ( ymax - ymin ) / (PLFLT) ( ny - 1 );

        // Pixel dimensions of the (Dxmin, Dymin) to (Dxmax, Dymax) box
        nnx = (PLINT) ceil( ( Dxmax - Dxmin ) / dx ) + 1;
        nny = (PLINT) ceil( ( Dymax - Dymin ) / dy ) + 1;

        // Call plimagefr with the value -> color range mapped to the minimum
        // Offsets for the idata indices to select
        // (Dxmin, Dymin) to (Dxmax, Dymax)
        xm = (PLINT) floor( ( Dxmin - xmin ) / dx );
        ym = (PLINT) floor( ( Dymin - ymin ) / dy );

        // Allocate space for the sub-image
        plAlloc2dGrid( &z, nnx, nny );
        zops = plf2ops_c();

        // Go through the image and select the pixels within the given
        // (Dxmin, Dymin) - (Dxmax, Dymax) window.
        ixx = -1;
        for ( ix = xm; ix < xm + nnx; ix++ )
        {
            ixx++; iyy = 0;
            for ( iy = ym; iy < ym + nny; iy++ )
            {
                z[ixx][iyy++] = idataops->get( idatap, ix, iy );
            }
        }

        // Set the appropriate values to pass in to plimagefr
        copied = TRUE;
    }

    plfimagefr( zops, (PLPointer) z, nnx, nny, Dxmin, Dxmax, Dymin, Dymax, zmin, zmax,
        data_min, data_max, NULL, NULL );

    // Only free the memory if it was allocated by us...
    if ( copied == TRUE )
    {
        plFree2dGrid( z, nnx, nny );
    }
}
