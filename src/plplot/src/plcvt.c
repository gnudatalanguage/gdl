//      Coordinate transformation routines.
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
// Transformations returning physical coordinates.
//--------------------------------------------------------------------------

// device coords to physical coords (x)

PLINT
plP_dcpcx( PLFLT x )
{
    return ( ROUND( plsc->phyxmi + plsc->phyxlen * x ) );
}

// device coords to physical coords (y)

PLINT
plP_dcpcy( PLFLT y )
{
    return ( ROUND( plsc->phyymi + plsc->phyylen * y ) );
}

// millimeters from bottom left-hand corner to physical coords (x)

PLINT
plP_mmpcx( PLFLT x )
{
    return ( ROUND( plsc->phyxmi + plsc->xpmm * x ) );
}

// millimeters from bottom left-hand corner to physical coords (y)

PLINT
plP_mmpcy( PLFLT y )
{
    return ( ROUND( plsc->phyymi + plsc->ypmm * y ) );
}

// world coords to physical coords (x)

PLINT
plP_wcpcx( PLFLT x )
{
    if ( !isfinite( x ) )
        return PLINT_MIN;
    return ( ROUND( plsc->wpxoff + plsc->wpxscl * x ) );
}

// world coords to physical coords (y)

PLINT
plP_wcpcy( PLFLT y )
{
    if ( !isfinite( y ) )
        return PLINT_MIN;
    return ( ROUND( plsc->wpyoff + plsc->wpyscl * y ) );
}

//--------------------------------------------------------------------------
// Transformations returning device coordinates.
//--------------------------------------------------------------------------

// physical coords to device coords (x)

PLFLT
plP_pcdcx( PLINT x )
{
    return (PLFLT) ( ( x - plsc->phyxmi ) / (double) plsc->phyxlen );
}

// physical coords to device coords (y)

PLFLT
plP_pcdcy( PLINT y )
{
    return (PLFLT) ( ( y - plsc->phyymi ) / (double) plsc->phyylen );
}

// millimeters from bottom left corner to device coords (x)

PLFLT
plP_mmdcx( PLFLT x )
{
    return ( (PLFLT) ( x * plsc->xpmm / ABS( plsc->phyxma - plsc->phyxmi ) ) );
}

// millimeters from bottom left corner to device coords (y)

PLFLT
plP_mmdcy( PLFLT y )
{
    return ( (PLFLT) ( y * plsc->ypmm / ABS( plsc->phyyma - plsc->phyymi ) ) );
}

// world coords into device coords (x)

PLFLT
plP_wcdcx( PLFLT x )
{
    return ( (PLFLT) ( plsc->wdxoff + plsc->wdxscl * x ) );
}

// world coords into device coords (y)

PLFLT
plP_wcdcy( PLFLT y )
{
    return ( (PLFLT) ( plsc->wdyoff + plsc->wdyscl * y ) );
}

// subpage coords to device coords (x)

PLFLT
plP_scdcx( PLFLT x )
{
    return ( (PLFLT) ( plsc->spdxmi + ( plsc->spdxma - plsc->spdxmi ) * x ) );
}

// subpage coords to device coords (y)

PLFLT
plP_scdcy( PLFLT y )
{
    return ( (PLFLT) ( plsc->spdymi + ( plsc->spdyma - plsc->spdymi ) * y ) );
}

//--------------------------------------------------------------------------
// Transformations returning millimeters.
//--------------------------------------------------------------------------

// device coords to millimeters from bottom left-hand corner (x)

PLFLT
plP_dcmmx( PLFLT x )
{
    return ( (PLFLT) ( x * ABS( plsc->phyxma - plsc->phyxmi ) / plsc->xpmm ) );
}

// device coords to millimeters from bottom left-hand corner (y)

PLFLT
plP_dcmmy( PLFLT y )
{
    return ( (PLFLT) ( y * ABS( plsc->phyyma - plsc->phyymi ) / plsc->ypmm ) );
}

// world coords into millimeters (x)

PLFLT
plP_wcmmx( PLFLT x )
{
    return ( (PLFLT) ( plsc->wmxoff + plsc->wmxscl * x ) );
}

// world coords into millimeters (y)

PLFLT
plP_wcmmy( PLFLT y )
{
    return ( (PLFLT) ( plsc->wmyoff + plsc->wmyscl * y ) );
}

//--------------------------------------------------------------------------
// Transformations returning subpage coordinates.
//--------------------------------------------------------------------------

// device coords to subpage coords (x)

PLFLT
plP_dcscx( PLFLT x )
{
    return ( (PLFLT) ( ( x - plsc->spdxmi ) / ( plsc->spdxma - plsc->spdxmi ) ) );
}

// device coords to subpage coords (y)

PLFLT
plP_dcscy( PLFLT y )
{
    return ( (PLFLT) ( ( y - plsc->spdymi ) / ( plsc->spdyma - plsc->spdymi ) ) );
}

//--------------------------------------------------------------------------
// 3-d plot transformations.
//--------------------------------------------------------------------------

// 3-d coords to 2-d projection (x)
// See c_plw3d for a mathematical explanation of the transformation.

PLFLT
plP_w3wcx( PLFLT x, PLFLT y, PLFLT PL_UNUSED( z ) )
{
    return ( (PLFLT) ( ( x - plsc->basecx ) * plsc->cxx +
                       ( y - plsc->basecy ) * plsc->cxy ) );
}

// 3-d coords to 2-d projection (y)
// See c_plw3d for a mathematical explanation of the transformation.

PLFLT
plP_w3wcy( PLFLT x, PLFLT y, PLFLT z )
{
    return ( (PLFLT) ( ( x - plsc->basecx ) * plsc->cyx +
                       ( y - plsc->basecy ) * plsc->cyy +
                       ( z - plsc->ranmi ) * plsc->cyz ) );
}

// 3-d coords to 2-d projection (z), if that makes any sense...
// See c_plw3d for a mathematical explanation of the transformation.

PLFLT
plP_w3wcz( PLFLT x, PLFLT y, PLFLT z )
{
    return ( (PLFLT) ( ( x - plsc->basecx ) * plsc->czx +
                       ( y - plsc->basecy ) * plsc->czy +
                       ( z - plsc->ranmi ) * plsc->czz ) );
}
