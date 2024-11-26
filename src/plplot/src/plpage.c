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

//! @file
//!
//! Page/subpage handling routines.
//!

#include "plplotP.h"
#include "drivers.h"

//--------------------------------------------------------------------------
//! Advance to subpage "page" or to the next page if "page" = 0.
//!
//! @param page Subpage identifier or 0
//!
void
c_pladv( PLINT page )
{
    if ( plsc->level < 1 )
    {
        plabort( "pladv: Please call plinit first" );
        return;
    }

    if ( page > 0 && page <= plsc->nsubx * plsc->nsuby )
        plsc->cursub = page;

    else if ( page == 0 )
    {
        if ( plsc->cursub >= plsc->nsubx * plsc->nsuby )
        {
            plP_eop();
            plP_wait();
            plP_bop();
            plsc->cursub = 1;
        }
        else
            plsc->cursub++;
    }
    else
    {
        plabort( "pladv: Invalid subpage number" );
        return;
    }

    plP_setsub();
}

//--------------------------------------------------------------------------
//! Clear current subpage.  Subpages can be set with pladv before
//! calling plclear. Not all drivers support this.
//
void
c_plclear( void )
{
    if ( plsc->level < 1 )
    {
        plabort( "plclear: Please call plinit first" );
        return;
    }

    if ( plsc->dev_clear )
        plP_esc( PLESC_CLEAR, NULL );
    else   // driver does not support clear, fill using background color

    {
        short x[5], y[5];
        int   ocolor = plsc->icol0;

        x[0] = x[3] = x[4] = (short) plsc->sppxmi;
        x[1] = x[2] = (short) plsc->sppxma;
        y[0] = y[1] = y[4] = (short) plsc->sppymi;
        y[2] = y[3] = (short) plsc->sppyma;
        plcol0( 0 );
        plP_fill( x, y, 5 );
        plcol0( ocolor );
    }
}

//--------------------------------------------------------------------------
//! End current page.
//!
void
c_pleop( void )
{
    if ( plsc->level < 1 )
    {
        plabort( "pleop: Please call plinit first" );
        return;
    }

    plsc->cursub = plsc->nsubx * plsc->nsuby;
    plP_eop();
    plP_wait();
}

//--------------------------------------------------------------------------
//! Start new page. Should only be used with pleop().
//!
void
c_plbop( void )
{
    if ( plsc->level < 1 )
    {
        plabort( "plbop: Please call plinit first" );
        return;
    }
    plP_bop();
    plsc->cursub = 1;
    plP_setsub();
}

//--------------------------------------------------------------------------
//! Set up plot parameters according to the number of subpages.
//!
void
plP_subpInit( void )
{
    PLFLT scale, size_chr, size_sym, size_maj, size_min, theta, rat;

// Subpage checks

    if ( plsc->nsubx <= 0 )
        plsc->nsubx = 1;
    if ( plsc->nsuby <= 0 )
        plsc->nsuby = 1;

    plsc->cursub = 0;

//
// Set default sizes
// Global scaling:
//	Normalize to the page length for more uniform results.
//      A virtual page length of 200 mm is assumed.
// Subpage scaling:
//	Reduce sizes with plot area (non-proportional, so that character
//	size doesn't get too small).
//
    scale = 0.5 *
            ( ( plsc->phyxma - plsc->phyxmi ) / plsc->xpmm +
              ( plsc->phyyma - plsc->phyymi ) / plsc->ypmm ) / 200.0;

    // Take account of scaling caused by change of orientation
    if ( plsc->difilt & PLDI_ORI )
    {
        theta = 0.5 * M_PI * plsc->diorot;
        rat   = ( ( plsc->phyxma - plsc->phyxmi ) / plsc->xpmm ) /
                ( ( plsc->phyyma - plsc->phyymi ) / plsc->ypmm );
        rat    = MAX( rat, 1.0 / rat );
        rat    = fabs( cos( theta ) ) + rat * fabs( sin( theta ) );
        scale /= rat;
    }

    if ( plsc->nsuby > 1 )
        scale /= sqrt( (double) plsc->nsuby );

    size_chr = 4.0;
    size_sym = 4.0;             // All these in virtual plot units
    size_maj = 3.0;
    size_min = 1.5;

    plschr( size_chr * scale, 1.0 );
    plssym( size_sym * scale, 1.0 );
    plsmaj( size_maj * scale, 1.0 );
    plsmin( size_min * scale, 1.0 );
}

//--------------------------------------------------------------------------
//! Set up the subpage boundaries according to the current subpage selected.
//!
void
plP_setsub( void )
{
    PLINT ix, iy;

    ix = ( plsc->cursub - 1 ) % plsc->nsubx;
    iy = plsc->nsuby - ( plsc->cursub - 1 ) / plsc->nsubx;

    plsc->spdxmi = (PLFLT) ( ix ) / (PLFLT) ( plsc->nsubx );
    plsc->spdxma = (PLFLT) ( ix + 1 ) / (PLFLT) ( plsc->nsubx );
    plsc->spdymi = (PLFLT) ( iy - 1 ) / (PLFLT) ( plsc->nsuby );
    plsc->spdyma = (PLFLT) ( iy ) / (PLFLT) ( plsc->nsuby );

    plsc->sppxmi = plP_dcpcx( plsc->spdxmi );
    plsc->sppxma = plP_dcpcx( plsc->spdxma );
    plsc->sppymi = plP_dcpcy( plsc->spdymi );
    plsc->sppyma = plP_dcpcy( plsc->spdyma );

    plP_sclp( plsc->sppxmi, plsc->sppxma, plsc->sppymi, plsc->sppyma );

    if ( plsc->plbuf_write )
        plbuf_setsub( plsc );
}

//--------------------------------------------------------------------------
//! Get subpage boundaries in absolute coordinates (mm from bottom
//! left-hand corner of page).
//!
//! @param xmin Pointer to PLFLT containing minimum x boundary after call
//! @param xmax Pointer to PLFLT containing maximum x boundary after call
//! @param ymin Pointer to PLFLT containing minimum y boundary after call
//! @param ymax Pointer to PLFLT containing maximum y boundary after call
//!
void
c_plgspa( PLFLT *xmin, PLFLT *xmax, PLFLT *ymin, PLFLT *ymax )
{
    if ( plsc->level < 1 )
    {
        plabort( "plgspa: Please call plinit first" );
        return;
    }
    *xmin = plP_dcmmx( plsc->spdxmi );
    *xmax = plP_dcmmx( plsc->spdxma );
    *ymin = plP_dcmmy( plsc->spdymi );
    *ymax = plP_dcmmy( plsc->spdyma );
}

//--------------------------------------------------------------------------
//! Wait for graphics input event and translate to world coordinates.
//!
//! @author Paul Casteels.
//! @param plg Pointer to PLGraphicsIn
//! @return 0 if no translation to world coordinates is possible.
//! @see PLGraphicsIn
//!
PLINT
plGetCursor( PLGraphicsIn *plg )
{
    plP_esc( PLESC_GETC, plg );
    return plTranslateCursor( plg );
}

//--------------------------------------------------------------------------
//! Translates cursor position from relative device coordinates to world
//! coordinates.
//!
//! @author Paul Casteels, modified by Alan W. Irwin
//! @param plg Pointer to PLGraphicsIn
//! @return 0 if no translation to world coordinates is possible.
//!
PLINT
plTranslateCursor( PLGraphicsIn *plg )
{
    int window;
    c_plcalc_world( plg->dX, plg->dY, &plg->wX, &plg->wY,
        (PLINT *) &window );
    if ( window >= 0 )
    {
        plg->subwindow = window;
        return 1;
    }
    else
        return 0;
}

//--------------------------------------------------------------------------
//! Calculate world coordinates wx, and wy from relative device
//! coordinates, rx and ry.  Also, return the window index for which
//! the world coordinates are valid. window is set to -1 and wx and wy
//! to 0. if rx and ry do not correspond to valid world coordinates
//! for any currently existing window.
//!
//! @author Paul Casteels, modified by Alan W. Irwin.
//! @param rx Relative x device coordinates
//! @param ry Relative y device coordinates
//! @param wx Pointer to x world coordinate (after call)
//! @param wy Pointer to y world coordinate (after call)
//! @param window Pointer index of window for which the world coordinates
//! are valid
//!
void
c_plcalc_world( PLFLT rx, PLFLT ry, PLFLT *wx, PLFLT *wy, PLINT *window )
{
    int      i;
    int      lastwin  = plsc->nplwin - 1;
    int      firstwin = MAX( plsc->nplwin - PL_MAXWINDOWS, 0 );
    PLWindow *w;

    for ( i = lastwin; i >= firstwin; i-- )
    {
        w = &plsc->plwin[i % PL_MAXWINDOWS];
        if ( ( rx >= w->dxmi ) &&
             ( rx <= w->dxma ) &&
             ( ry >= w->dymi ) &&
             ( ry <= w->dyma ) )
        {
            *wx = w->wxmi + ( rx - w->dxmi ) *
                  ( w->wxma - w->wxmi ) / ( w->dxma - w->dxmi );

            *wy = w->wymi + ( ry - w->dymi ) *
                  ( w->wyma - w->wymi ) / ( w->dyma - w->dymi );

            *window = i;

            return;
        }
    }
    // No valid window found with these relative coordinates.
    *wx     = 0.;
    *wy     = 0.;
    *window = -1;
    return;
}
