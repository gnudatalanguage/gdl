//      Routines for setting up world coordinates of the current viewport.
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

#define  dtr    0.01745329252

//--------------------------------------------------------------------------
// void plwind()
//
// Set up world coordinates of the viewport boundaries (2d plots).
//--------------------------------------------------------------------------

void
c_plwind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax )
{
    PLFLT    dx, dy, mmxmi, mmxma, mmymi, mmyma;
    PLFLT    xvpwxmin, xvpwxmax, xvpwymin, xvpwymax;
    PLWindow w;

    if ( plsc->level < 2 )
    {
        plabort( "plwind: Please set up viewport first" );
        return;
    }

// Best to just warn and recover on bounds errors

    if ( xmin == xmax )
    {
        plwarn( "plwind: Invalid window limits in x." );
        xmin--; xmax++;
    }
    if ( ymin == ymax )
    {
        plwarn( "plwind: Invalid window limits in y." );
        ymin--; ymax++;
    }

    plsc->vpwxmi = xmin;
    plsc->vpwxma = xmax;
    plsc->vpwymi = ymin;
    plsc->vpwyma = ymax;

// The true plot window is made slightly larger than requested so that
// the end limits will be on the graph
// Get the (slightly extended) window limits.
    plP_xgvpw( &xvpwxmin, &xvpwxmax, &xvpwymin, &xvpwymax );
// Compute the scaling between coordinate systems

    dx = xvpwxmax - xvpwxmin;
    dy = xvpwymax - xvpwymin;
    plsc->wpxscl = ( plsc->vppxma - plsc->vppxmi ) / dx;
    plsc->wpxoff = ( xmax * plsc->vppxmi - xmin * plsc->vppxma ) / dx;
    plsc->wpyscl = ( plsc->vppyma - plsc->vppymi ) / dy;
    plsc->wpyoff = ( ymax * plsc->vppymi - ymin * plsc->vppyma ) / dy;

    mmxmi = plP_dcmmx( plsc->vpdxmi );
    mmxma = plP_dcmmx( plsc->vpdxma );
    mmymi = plP_dcmmy( plsc->vpdymi );
    mmyma = plP_dcmmy( plsc->vpdyma );

// Set transformation variables for world coordinates to mm

    plsc->wmxscl = ( mmxma - mmxmi ) / dx;
    plsc->wmxoff = ( xmax * mmxmi - xmin * mmxma ) / dx;
    plsc->wmyscl = ( mmyma - mmymi ) / dy;
    plsc->wmyoff = ( ymax * mmymi - ymin * mmyma ) / dy;

// Set transformation variables for world coordinates to device coords

    plsc->wdxscl = plsc->wmxscl * plsc->xpmm / ( plsc->phyxma - plsc->phyxmi );
    plsc->wdxoff = plsc->wmxoff * plsc->xpmm / ( plsc->phyxma - plsc->phyxmi );
    plsc->wdyscl = plsc->wmyscl * plsc->ypmm / ( plsc->phyyma - plsc->phyymi );
    plsc->wdyoff = plsc->wmyoff * plsc->ypmm / ( plsc->phyyma - plsc->phyymi );

// Register plot window attributes

    w.dxmi = plsc->vpdxmi;
    w.dxma = plsc->vpdxma;
    w.dymi = plsc->vpdymi;
    w.dyma = plsc->vpdyma;

    w.wxmi = xvpwxmin;
    w.wxma = xvpwxmax;
    w.wymi = xvpwymin;
    w.wyma = xvpwymax;

    plP_swin( &w );

// Go to level 3

    plsc->level = 3;
}

//--------------------------------------------------------------------------
// void plw3d()
//
// Set up a window for three-dimensional plotting. The data are mapped
// into a box with world coordinate size "basex" by "basey" by "height",
// with the base being symmetrically positioned about zero. Thus
// the mapping between data 3-d and world 3-d coordinates is given by:
//
//   x = xmin   =>   wx = -0.5*basex
//   x = xmax   =>   wx =  0.5*basex
//   y = ymin   =>   wy = -0.5*basey
//   y = ymax   =>   wy =  0.5*basey
//   z = zmin   =>   wz =  0.0
//   z = zmax   =>   wz =  height
//
// The world coordinate box is then viewed from position "alt"-"az",
// measured in degrees. For proper operation, 0 <= alt <= 90 degrees,
// but az can be any value.
//--------------------------------------------------------------------------

void
c_plw3d( PLFLT basex, PLFLT basey, PLFLT height, PLFLT xmin,
         PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT zmin,
         PLFLT zmax, PLFLT alt, PLFLT az )
{
    PLFLT xmin_adjusted, xmax_adjusted, ymin_adjusted, ymax_adjusted, zmin_adjusted, zmax_adjusted, d;
    PLFLT cx, cy, saz, caz, salt, calt, zscale;

    if ( plsc->level < 3 )
    {
        plabort( "plw3d: Please set up 2-d window first" );
        return;
    }
    if ( basex <= 0.0 || basey <= 0.0 || height <= 0.0 )
    {
        plabort( "plw3d: Invalid world coordinate boxsize" );
        return;
    }
    if ( xmin == xmax || ymin == ymax || zmin == zmax )
    {
        plabort( "plw3d: Invalid axis range" );
        return;
    }
    if ( alt < 0.0 || alt > 90.0 )
    {
        plabort( "plw3d: Altitude must be between 0 and 90 degrees" );
        return;
    }

    d             = 1.0e-5 * ( xmax - xmin );
    xmax_adjusted = xmax + d;
    xmin_adjusted = xmin - d;
    d             = 1.0e-5 * ( ymax - ymin );
    ymax_adjusted = ymax + d;
    ymin_adjusted = ymin - d;
    d             = 1.0e-5 * ( zmax - zmin );
    zmax_adjusted = zmax + d;
    zmin_adjusted = zmin - d;
    cx            = basex / ( xmax_adjusted - xmin_adjusted );
    cy            = basey / ( ymax_adjusted - ymin_adjusted );
    zscale        = height / ( zmax_adjusted - zmin_adjusted );
    saz           = sin( dtr * az );
    caz           = cos( dtr * az );
    salt          = sin( dtr * alt );
    calt          = cos( dtr * alt );

    plsc->domxmi = xmin_adjusted;
    plsc->domxma = xmax_adjusted;
    plsc->domymi = ymin_adjusted;
    plsc->domyma = ymax_adjusted;
    plsc->zzscl  = zscale;
    plsc->ranmi  = zmin_adjusted;
    plsc->ranma  = zmax_adjusted;

    plsc->base3x = basex;
    plsc->base3y = basey;
    plsc->basecx = 0.5 * ( xmin_adjusted + xmax_adjusted );
    plsc->basecy = 0.5 * ( ymin_adjusted + ymax_adjusted );
// Mathematical explanation of the 3 transformations of coordinates:
// (I) Scaling:
//     x' = cx*(x-x_mid) = cx*(x-plsc->basecx)
//     y' = cy*(y-y_mid) = cy*(y-plsc->basecy)
//     z' = zscale*(z-zmin_adjusted) = zscale*(z-plsc->ranmi)
// (II) Rotation about z' axis clockwise by the angle of the azimut when
//      looking from the top in a right-handed coordinate system.
//     x''          x'
//     y'' =  M_1 * y'
//     z''          z'
//    where the rotation matrix M_1 (see any mathematical physics book such
//    as Mathematical Methods in the Physical Sciences by Boas) is
//    caz          -saz       0
//    saz           caz       0
//     0             0        1
// (III) Rotation about x'' axis by 90 deg - alt to bring z''' axis
//      coincident with line of sight and x''' and y''' corresponding to
//      x and y coordinates in the 2D plane of the plot.
//     x'''          x''
//     y''' =  M_2 * y''
//     z'''          z''
//    where the rotation matrix M_2 is
//     1            0         0
//     0           salt      calt
//     0          -calt      salt
// Note
//     x'''          x'
//     y''' =  M *   y'
//     z'''          z'
// where M = M_2*M_1 is given by
//          caz      -saz     0
//     salt*saz  salt*caz    calt
//    -calt*saz -calt*caz    salt
// plP_w3wcx and plP_w3wcy take the combination of the plsc->basecx,
// plsc->basecy, plsc->ranmi, plsc->cxx, plsc->cxy, plsc->cyx, plsc->cyy, and
// plsc->cyz data stored here to implement the combination of the 3
// transformations to determine x''' and y''' from x, y, and z.
//
    plsc->cxx = cx * caz;
    plsc->cxy = -cy * saz;
    plsc->cyx = cx * saz * salt;
    plsc->cyy = cy * caz * salt;
    plsc->cyz = zscale * calt;
    plsc->czx = -cx * calt * saz;
    plsc->czy = -cy * calt * caz;
    plsc->czz = zscale * salt;
}
