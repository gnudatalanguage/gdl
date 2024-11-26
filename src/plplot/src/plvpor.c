//      Functions dealing with viewports.
//
// Copyright (C) 2004  Joao Cardoso
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

static void
c_plenvi( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
          PLINT just, PLINT axis, PLINT old );

//--------------------------------------------------------------------------
// void plenv()
//
// Simple interface for defining viewport and window.
//
// The "just" parameter control how the axes will be scaled:
//
//       just=-1 : The scales will not be set, the user must set up the scale
//                   before calling plenv() using plsvpa(), plvasp() or other;
//       just= 0 : The scales will be set up to optimize plot area;
//       just= 1 : The scales will be the same;
//       just= 2 : The axes will be equal, the plot box will be square.
//
// The "axis" parameter is interpreted as follows:
//
//	axis=-2 : draw no box, no tick marks, no numeric tick labels, no axes.
//	axis=-1 : draw box only.
//	axis= 0 : Draw box, ticks, and numeric tick labels.
//	axis= 1 : Also draw coordinate axes at X=0, and Y=0.
//	axis= 2 : Also draw a grid at major tick positions in both coordinates.
//	axis= 3 : Same as 2, but the grid will be also at the minor ticks.
//	axis=10 : Same as 0 except Logarithmic X tick marks. (The X data have
//      to be converted to logarithms separately.)
//	axis=11 : Same as 1 except Logarithmic X tick marks. (The X data have
//      to be converted to logarithms separately.)
//	axis=12 : Same as 2 except Logarithmic X tick marks. (The X data have
//      to be converted to logarithms separately.)
//      axis=13 : Same as 12, but the grid will be also at the minor ticks.
//	axis=20 : Same as 0 except Logarithmic Y tick marks. (The Y data have
//      to be converted to logarithms separately.)
//	axis=21 : Same as 1 except Logarithmic Y tick marks. (The Y data have
//      to be converted to logarithms separately.)
//	axis=22 : Same as 2 except Logarithmic Y tick marks. (The Y data have
//      to be converted to logarithms separately.)
//      axis=23 : Same as 22, but the grid will be also at the minor ticks.
//	axis=30 : Same as 0 except Logarithmic X,Y tick marks. (The X,Y data have
//      to be converted to logarithms separately.)
//	axis=31 : Same as 1 except Logarithmic X,Y tick marks. (The X,Y data have
//      to be converted to logarithms separately.)
//	axis=32 : Same as 2 except Logarithmic X,Y tick marks. (The X,Y data have
//      to be converted to logarithms separately.)
//      axis=33 : Same as 32, but the grid will be also at the minor ticks.
//	axis=40 : Same as 0 except date / time X tick marks.
//	axis=41 : Same as 1 except date / time X tick marks.
//	axis=42 : Same as 2 except date / time X tick marks.
//      axis=43 : Same as 42, but the grid will be also at the minor ticks.
//	axis=50 : Same as 0 except date / time Y tick marks.
//	axis=51 : Same as 1 except date / time Y tick marks.
//	axis=52 : Same as 2 except date / time Y tick marks.
//      axis=53 : Same as 52, but the grid will be also at the minor ticks.
//	axis=60 : Same as 0 except date / time X,Y tick marks.
//	axis=61 : Same as 1 except date / time X,Y tick marks.
//	axis=62 : Same as 2 except date / time X,Y tick marks.
//      axis=63 : Same as 62, but the grid will be also at the minor ticks.
//      axis=70 : Same as 0 except custom X,Y labels.
//      axis=71 : Same as 1 except custom X,Y labels.
//      axis=72 : Same as 2 except custom X,Y labels.
//      axis=73 : Same as 72, but the grid will be also at the minor ticks.
//--------------------------------------------------------------------------

void
c_plenv( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
         PLINT just, PLINT axis )
{
    c_plenvi( xmin, xmax, ymin, ymax, just, axis, 1 );
}

//--------------------------------------------------------------------------
// void plenv0()
//
// same as plenv() above, but if in multiplot mode does not advance the subpage,
// instead clears it.
//--------------------------------------------------------------------------

void
c_plenv0( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
          PLINT just, PLINT axis )
{
    c_plenvi( xmin, xmax, ymin, ymax, just, axis, 0 );
}


static void
c_plenvi( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax,
          PLINT just, PLINT axis, PLINT old )
{
    PLFLT lb, rb, tb, bb, dx, dy;
    PLFLT xsize, ysize, size, xscale, yscale, scale;
    PLFLT spxmin, spxmax, spymin, spymax;
    PLFLT vpxmin, vpxmax, vpymin, vpymax;

    if ( plsc->level < 1 )
    {
        plabort( "plenv: Please call plinit first" );
        return;
    }
    if ( xmin == xmax )
    {
        plabort( "plenv: Invalid xmin and xmax arguments" );
        return;
    }
    if ( ymin == ymax )
    {
        plabort( "plenv: Invalid ymin and ymax arguments" );
        return;
    }
    if ( just < -1 || just > 2 )
    {
        plabort( "plenv: Invalid just option" );
        return;
    }

    if ( plsc->nsubx * plsc->nsuby == 1 ) // not multiplot mode
        old = 1;

    if ( old == 1 )
        pladv( 0 );
    else
        plclear();

    if ( just == 0 )
        plvsta();
    else if ( just == 1 )
    {
        lb = 8.0 * plsc->chrht;
        rb = 5.0 * plsc->chrht;
        tb = 5.0 * plsc->chrht;
        bb = 5.0 * plsc->chrht;
        dx = ABS( xmax - xmin );
        dy = ABS( ymax - ymin );
        plgspa( &spxmin, &spxmax, &spymin, &spymax );
        xsize  = spxmax - spxmin;
        ysize  = spymax - spymin;
        xscale = dx / ( xsize - lb - rb );
        yscale = dy / ( ysize - tb - bb );
        scale  = MAX( xscale, yscale );
        vpxmin = MAX( lb, 0.5 * ( xsize - dx / scale ) );
        vpxmax = vpxmin + ( dx / scale );
        vpymin = MAX( bb, 0.5 * ( ysize - dy / scale ) );
        vpymax = vpymin + ( dy / scale );
        plsvpa( vpxmin, vpxmax, vpymin, vpymax );
    }
    else if ( just == 2 )
    {
        lb = 8.0 * plsc->chrht;
        rb = 5.0 * plsc->chrht;
        tb = 5.0 * plsc->chrht;
        bb = 5.0 * plsc->chrht;
        plgspa( &spxmin, &spxmax, &spymin, &spymax );
        xsize  = spxmax - spxmin;
        ysize  = spymax - spymin;
        size   = MIN( xsize - lb - rb, ysize - tb - bb );
        dx     = ( xsize - size - lb - rb ) / 2;
        vpxmin = lb + dx;
        vpxmax = vpxmin + size;
        dy     = ( ysize - size - bb - tb ) / 2;
        vpymin = bb + dy;
        vpymax = vpymin + size;
        plsvpa( vpxmin, vpxmax, vpymin, vpymax );
    }

    plwind( xmin, xmax, ymin, ymax );

    switch ( axis )
    {
    case -2:
        break;
    case -1:
        plbox( "bc", (PLFLT) 0.0, 0, "bc", (PLFLT) 0.0, 0 );
        break;
    case 0:
        plbox( "bcnst", (PLFLT) 0.0, 0, "bcnstv", (PLFLT) 0.0, 0 );
        break;
    case 1:
        plbox( "abcnst", (PLFLT) 0.0, 0, "abcnstv", (PLFLT) 0.0, 0 );
        break;
    case 2:
        plbox( "abcgnst", (PLFLT) 0.0, 0, "abcgnstv", (PLFLT) 0.0, 0 );
        break;
    case 3:
        plbox( "abcgnsth", (PLFLT) 0.0, 0, "abcgnstvh", (PLFLT) 0.0, 0 );
        break;
    case 10:
        plbox( "bclnst", (PLFLT) 0.0, 0, "bcnstv", (PLFLT) 0.0, 0 );
        break;
    case 11:
        plbox( "abclnst", (PLFLT) 0.0, 0, "abcnstv", (PLFLT) 0.0, 0 );
        break;
    case 12:
        plbox( "abcglnst", (PLFLT) 0.0, 0, "abcgnstv", (PLFLT) 0.0, 0 );
        break;
    case 13:
        plbox( "abcglnsth", (PLFLT) 0.0, 0, "abcgnstvh", (PLFLT) 0.0, 0 );
        break;
    case 20:
        plbox( "bcnst", (PLFLT) 0.0, 0, "bclnstv", (PLFLT) 0.0, 0 );
        break;
    case 21:
        plbox( "abcnst", (PLFLT) 0.0, 0, "abclnstv", (PLFLT) 0.0, 0 );
        break;
    case 22:
        plbox( "abcgnst", (PLFLT) 0.0, 0, "abcglnstv", (PLFLT) 0.0, 0 );
        break;
    case 23:
        plbox( "abcgnsth", (PLFLT) 0.0, 0, "abcglnstvh", (PLFLT) 0.0, 0 );
        break;
    case 30:
        plbox( "bclnst", (PLFLT) 0.0, 0, "bclnstv", (PLFLT) 0.0, 0 );
        break;
    case 31:
        plbox( "abclnst", (PLFLT) 0.0, 0, "abclnstv", (PLFLT) 0.0, 0 );
        break;
    case 32:
        plbox( "abcglnst", (PLFLT) 0.0, 0, "abcglnstv", (PLFLT) 0.0, 0 );
        break;
    case 33:
        plbox( "abcglnsth", (PLFLT) 0.0, 0, "abcglnstvh", (PLFLT) 0.0, 0 );
        break;
    case 40:
        plbox( "bcdnst", (PLFLT) 0.0, 0, "bcnstv", (PLFLT) 0.0, 0 );
        break;
    case 41:
        plbox( "abcdnst", (PLFLT) 0.0, 0, "abcnstv", (PLFLT) 0.0, 0 );
        break;
    case 42:
        plbox( "abcgdnst", (PLFLT) 0.0, 0, "abcgnstv", (PLFLT) 0.0, 0 );
        break;
    case 43:
        plbox( "abcgdnsth", (PLFLT) 0.0, 0, "abcgnstvh", (PLFLT) 0.0, 0 );
        break;
    case 50:
        plbox( "bcnst", (PLFLT) 0.0, 0, "bcdnstv", (PLFLT) 0.0, 0 );
        break;
    case 51:
        plbox( "abcnst", (PLFLT) 0.0, 0, "abcdnstv", (PLFLT) 0.0, 0 );
        break;
    case 52:
        plbox( "abcgnst", (PLFLT) 0.0, 0, "abcgdnstv", (PLFLT) 0.0, 0 );
        break;
    case 53:
        plbox( "abcgnsth", (PLFLT) 0.0, 0, "abcgdnstvh", (PLFLT) 0.0, 0 );
        break;
    case 60:
        plbox( "bcdnst", (PLFLT) 0.0, 0, "bcdnstv", (PLFLT) 0.0, 0 );
        break;
    case 61:
        plbox( "abcdnst", (PLFLT) 0.0, 0, "abcdnstv", (PLFLT) 0.0, 0 );
        break;
    case 62:
        plbox( "abcgdnst", (PLFLT) 0.0, 0, "abcgdnstv", (PLFLT) 0.0, 0 );
        break;
    case 63:
        plbox( "abcgdnsth", (PLFLT) 0.0, 0, "abcgdnstvh", (PLFLT) 0.0, 0 );
        break;
    case 70:
        plbox( "bcnost", (PLFLT) 0.0, 0, "bcnostv", (PLFLT) 0.0, 0 );
        break;
    case 71:
        plbox( "abcnost", (PLFLT) 0.0, 0, "abcnostv", (PLFLT) 0.0, 0 );
        break;
    case 72:
        plbox( "abcgnost", (PLFLT) 0.0, 0, "abcgnostv", (PLFLT) 0.0, 0 );
        break;
    case 73:
        plbox( "abcgnosth", (PLFLT) 0.0, 0, "abcgnostvh", (PLFLT) 0.0, 0 );
        break;
    default:
        plwarn( "plenv: Invalid axis argument" );
    }
}

//--------------------------------------------------------------------------
// void plvsta()
//
// Defines a "standard" viewport with seven character heights for
// the left margin and four character heights everywhere else.
//--------------------------------------------------------------------------

void
c_plvsta( void )
{
    PLFLT xmin, xmax, ymin, ymax;
    PLFLT lb, rb, tb, bb;

    if ( plsc->level < 1 )
    {
        plabort( "plvsta: Please call plinit first" );
        return;
    }

//  Find out position of subpage boundaries in millimetres, reduce by
//  the desired border, and convert back into normalized subpage
//  coordinates

    lb = 8.0 * plsc->chrht;
    rb = 5.0 * plsc->chrht;
    tb = 5.0 * plsc->chrht;
    bb = 5.0 * plsc->chrht;

    xmin = plP_dcscx( plP_mmdcx( (PLFLT) ( plP_dcmmx( plsc->spdxmi ) + lb ) ) );
    xmax = plP_dcscx( plP_mmdcx( (PLFLT) ( plP_dcmmx( plsc->spdxma ) - rb ) ) );
    ymin = plP_dcscy( plP_mmdcy( (PLFLT) ( plP_dcmmy( plsc->spdymi ) + tb ) ) );
    ymax = plP_dcscy( plP_mmdcy( (PLFLT) ( plP_dcmmy( plsc->spdyma ) - bb ) ) );

    plvpor( xmin, xmax, ymin, ymax );
}

//--------------------------------------------------------------------------
// void plvpor()
//
// Creates a viewport with the specified normalized subpage coordinates.
//--------------------------------------------------------------------------

void
c_plvpor( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax )
{
    if ( plsc->level < 1 )
    {
        plabort( "plvpor: Please call plinit first" );
        return;
    }
    if ( ( xmin >= xmax ) || ( ymin >= ymax ) )
    {
        plabort( "plvpor: Invalid limits" );
        return;
    }
    if ( ( plsc->cursub <= 0 ) || ( plsc->cursub > ( plsc->nsubx * plsc->nsuby ) ) )
    {
        plabort( "plvpor: Please call pladv or plenv to go to a subpage" );
        return;
    }

    plsc->vpdxmi = plsc->spdxmi + ( plsc->spdxma - plsc->spdxmi ) * xmin;
    plsc->vpdxma = plsc->spdxmi + ( plsc->spdxma - plsc->spdxmi ) * xmax;
    plsc->vpdymi = plsc->spdymi + ( plsc->spdyma - plsc->spdymi ) * ymin;
    plsc->vpdyma = plsc->spdymi + ( plsc->spdyma - plsc->spdymi ) * ymax;

    plsc->vppxmi = plP_dcpcx( plsc->vpdxmi );
    plsc->vppxma = plP_dcpcx( plsc->vpdxma );
    plsc->vppymi = plP_dcpcy( plsc->vpdymi );
    plsc->vppyma = plP_dcpcy( plsc->vpdyma );

    plP_sclp( MAX( plsc->vppxmi, plsc->phyxmi ), MIN( plsc->vppxma, plsc->phyxma ),
        MAX( plsc->vppymi, plsc->phyymi ), MIN( plsc->vppyma, plsc->phyyma ) );

    plsc->level = 2;
}

//--------------------------------------------------------------------------
// void plvpas()
//
// Creates the largest viewport of the specified aspect ratio that fits
// within the specified normalized subpage coordinates.
//--------------------------------------------------------------------------

void
c_plvpas( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax, PLFLT aspect )
{
    PLFLT spxmin, spxmax, spymin, spymax;
    PLFLT vpxmin, vpxmax, vpymin, vpymax;
    PLFLT xsize, ysize, nxsize, nysize;
    PLFLT xoffset, yoffset;

    if ( plsc->level < 1 )
    {
        plabort( "plvpas: Please call plinit first" );
        return;
    }
    if ( ( xmin >= xmax ) || ( ymin >= ymax ) )
    {
        plabort( "plvpas: Invalid limits" );
        return;
    }

    if ( aspect <= 0.0 )
    {
        c_plvpor( xmin, xmax, ymin, ymax );
        return;
    }

    plgspa( &spxmin, &spxmax, &spymin, &spymax );

    xsize = spxmax - spxmin;
    ysize = spymax - spymin;

    xoffset = xsize * xmin;
    yoffset = ysize * ymin;

    spxmax = spxmin + xsize * xmax;
    spxmin = spxmin + xsize * xmin;
    spymax = spymin + ysize * ymax;
    spymin = spymin + ysize * ymin;

    // Adjust size for the requested edging
    xsize = spxmax - spxmin;
    ysize = spymax - spymin;

    if ( aspect * xsize > ysize )
    {
        nxsize = ysize / aspect;
        nysize = ysize;
    }
    else
    {
        nxsize = xsize;
        nysize = xsize * aspect;
    }

// center plot within page

    vpxmin = 0.5 * ( xsize - nxsize ) + xoffset;
    vpxmax = vpxmin + nxsize;
    vpymin = 0.5 * ( ysize - nysize ) + yoffset;
    vpymax = vpymin + nysize;

    plsvpa( vpxmin, vpxmax, vpymin, vpymax );
}

//--------------------------------------------------------------------------
// void plvasp()
//
// Sets the edges of the viewport with the given aspect ratio, leaving
// room for labels.
//--------------------------------------------------------------------------

void
c_plvasp( PLFLT aspect )
{
    PLFLT spxmin, spxmax, spymin, spymax;
    PLFLT vpxmin, vpxmax, vpymin, vpymax;
    PLFLT xsize, ysize, nxsize, nysize;
    PLFLT lb, rb, tb, bb;

    if ( plsc->level < 1 )
    {
        plabort( "plvasp: Please call plinit first" );
        return;
    }

    lb = 8.0 * plsc->chrht;
    rb = 5.0 * plsc->chrht;
    tb = 5.0 * plsc->chrht;
    bb = 5.0 * plsc->chrht;

    plgspa( &spxmin, &spxmax, &spymin, &spymax );
    xsize  = spxmax - spxmin;
    ysize  = spymax - spymin;
    xsize -= lb + rb;           // adjust for labels
    ysize -= bb + tb;
    if ( aspect * xsize > ysize )
    {
        nxsize = ysize / aspect;
        nysize = ysize;
    }
    else
    {
        nxsize = xsize;
        nysize = xsize * aspect;
    }

// center plot within page

    vpxmin = .5 * ( xsize - nxsize ) + lb;
    vpxmax = vpxmin + nxsize;
    vpymin = .5 * ( ysize - nysize ) + bb;
    vpymax = vpymin + nysize;

    plsvpa( vpxmin, vpxmax, vpymin, vpymax );
}

//--------------------------------------------------------------------------
// void plsvpa()
//
// Sets the edges of the viewport to the specified absolute coordinates
// (mm), measured with respect to the current subpage boundaries.
//--------------------------------------------------------------------------

void
c_plsvpa( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax )
{
    PLFLT sxmin, symin;

    if ( plsc->level < 1 )
    {
        plabort( "plsvpa: Please call plinit first" );
        return;
    }
    if ( ( xmin >= xmax ) || ( ymin >= ymax ) )
    {
        plabort( "plsvpa: Invalid limits" );
        return;
    }
    if ( ( plsc->cursub <= 0 ) || ( plsc->cursub > ( plsc->nsubx * plsc->nsuby ) ) )
    {
        plabort( "plsvpa: Please call pladv or plenv to go to a subpage" );
        return;
    }

    sxmin = plP_dcmmx( plsc->spdxmi );
    symin = plP_dcmmy( plsc->spdymi );

    plsc->vpdxmi = plP_mmdcx( (PLFLT) ( sxmin + xmin ) );
    plsc->vpdxma = plP_mmdcx( (PLFLT) ( sxmin + xmax ) );
    plsc->vpdymi = plP_mmdcy( (PLFLT) ( symin + ymin ) );
    plsc->vpdyma = plP_mmdcy( (PLFLT) ( symin + ymax ) );

    plsc->vppxmi = plP_dcpcx( plsc->vpdxmi );
    plsc->vppxma = plP_dcpcx( plsc->vpdxma );
    plsc->vppymi = plP_dcpcy( plsc->vpdymi );
    plsc->vppyma = plP_dcpcy( plsc->vpdyma );

    plP_sclp( plP_dcpcx( plsc->vpdxmi ), plP_dcpcx( plsc->vpdxma ),
        plP_dcpcy( plsc->vpdymi ), plP_dcpcy( plsc->vpdyma ) );

    plsc->level = 2;
}
