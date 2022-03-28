// PLplot MEM (in user-supplied memory) device driver.
// The idea here is that the user will specify the Y by X by RGB
// area in which to plot using the plsmem function (added by me).
//
// This is a bare-bones driver which allows one to plot on an existing
// image in memory.  This is useful if the user has an image in memory
// that he wants to decorate with PLPLOT.
//
// Contributed by Doug Hunt
// Included in PLplot by Rafael Laboissiere on Sat Feb 22 18:34:06 CET 2003
//

#include "plDevs.h"

#ifdef PLD_mem

#include "plplotP.h"
#include "drivers.h"

// Device info
PLDLLIMPEXP_DRIVER const char* plD_DEVICE_INFO_mem = "mem:User-supplied memory device:-1:mem:46:mem\n";

void plD_dispatch_init_mem( PLDispatchTable *pdt );

void plD_init_mem( PLStream * );
void plD_line_mem( PLStream *, short, short, short, short );
void plD_polyline_mem( PLStream *, short *, short *, PLINT );
void plD_eop_mem( PLStream * );
void plD_bop_mem( PLStream * );
void plD_tidy_mem( PLStream * );
void plD_state_mem( PLStream *, PLINT );
void plD_esc_mem( PLStream *, PLINT, void * );

#undef MAX
#undef ABS
#define MAX( a, b )    ( ( a > b ) ? a : b )
#define ABS( a )       ( ( a < 0 ) ? -a : a )

#define MAX_INTENSITY    255

void plD_dispatch_init_mem( PLDispatchTable *pdt )
{
#ifndef ENABLE_DYNDRIVERS
    pdt->pl_MenuStr = "User-supplied memory device";
    pdt->pl_DevName = "mem";
#endif
    pdt->pl_type     = plDevType_Null;
    pdt->pl_seq      = 45;
    pdt->pl_init     = (plD_init_fp) plD_init_mem;
    pdt->pl_line     = (plD_line_fp) plD_line_mem;
    pdt->pl_polyline = (plD_polyline_fp) plD_polyline_mem;
    pdt->pl_eop      = (plD_eop_fp) plD_eop_mem;
    pdt->pl_bop      = (plD_bop_fp) plD_bop_mem;
    pdt->pl_tidy     = (plD_tidy_fp) plD_tidy_mem;
    pdt->pl_state    = (plD_state_fp) plD_state_mem;
    pdt->pl_esc      = (plD_esc_fp) plD_esc_mem;
}

//--------------------------------------------------------------------------
// plD_init_mem()
//
// Initialize device (terminal).
//--------------------------------------------------------------------------

void
plD_init_mem( PLStream *pls )
{
    // plsmem must have already been called to set pls->dev to the
    // user supplied plotting area.  The dimensions of the plot area
    // have also been set by plsmem.  Verify this.
    //

    if ( ( pls->phyxma == 0 ) || ( pls->dev == NULL ) )
    {
        plexit( "Must call plsmem first to set user plotting area!" );
    }

    if ( pls->dev_mem_alpha == 1 )
    {
        plexit( "The mem driver does not support alpha values! Use plsmem!" );
    }

    plP_setpxl( (PLFLT) 4, (PLFLT) 4 ); // rough pixels/mm on *my* screen


    pls->color     = 1;         // Is a color device
    pls->dev_fill0 = 0;         // Handle solid fills
    pls->dev_fill1 = 0;         // Use PLplot core fallback for pattern fills
    pls->nopause   = 1;         // Don't pause between frames
}

#define sign( a )    ( ( a < 0 ) ? -1 : ( ( a == 0 ) ? 0 : 1 ) )

void
plD_line_mem( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    int           i;
    PLINT         idx;
    int           x1 = x1a, y1 = y1a, x2 = x2a, y2 = y2a;
    PLINT         x1b, y1b, x2b, y2b;
    PLFLT         length, fx, fy, dx, dy;
    unsigned char *mem = (unsigned char *) pls->dev;
    PLINT         xm   = pls->phyxma;
    PLINT         ym   = pls->phyyma;

    // Take mirror image, since (0,0) must be at top left

    y1 = ym - ( y1 - 0 );
    y2 = ym - ( y2 - 0 );

    x1b    = x1, x2b = x2, y1b = y1, y2b = y2;
    length = (PLFLT) sqrt( (double)
        ( ( x2b - x1b ) * ( x2b - x1b ) + ( y2b - y1b ) * ( y2b - y1b ) ) );

    if ( length == 0. )
        length = 1.;
    dx = ( x2 - x1 ) / length;
    dy = ( y2 - y1 ) / length;

    fx = x1;
    fy = y1;
    mem[3 * xm * y1 + 3 * x1 + 0] = pls->curcolor.r;
    mem[3 * xm * y1 + 3 * x1 + 1] = pls->curcolor.g;
    mem[3 * xm * y1 + 3 * x1 + 2] = pls->curcolor.b;

    mem[3 * xm * y2 + 3 * x2 + 0] = pls->curcolor.r;
    mem[3 * xm * y2 + 3 * x2 + 1] = pls->curcolor.g;
    mem[3 * xm * y2 + 3 * x2 + 2] = pls->curcolor.b;

    for ( i = 1; i <= (int) length; i++ )
    {
        fx          += dx;
        fy          += dy;
        idx          = 3 * xm * (PLINT) fy + 3 * (PLINT) fx;
        mem[idx + 0] = pls->curcolor.r;
        mem[idx + 1] = pls->curcolor.g;
        mem[idx + 2] = pls->curcolor.b;
    }
}

void
plD_polyline_mem( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    int i;
    for ( i = 0; i < npts - 1; i++ )
        plD_line_mem( pls, xa[i], ya[i], xa[i + 1], ya[i + 1] );
}

void
plD_eop_mem( PLStream *pls )
{
    // Set the 'dev' member (which holds the user supplied memory image)
    // to NULL here so it won't be freed when PLplot is closed.
    // (the user is responsible for freeing it when ready).
    //
    pls->dev = NULL;
}

void
plD_bop_mem( PLStream * PL_UNUSED( pls ) )
{
// Nothing to do here
}

void
plD_tidy_mem( PLStream * PL_UNUSED( pls ) )
{
// Nothing to do here
}

void
plD_state_mem( PLStream * PL_UNUSED( pls ), PLINT PL_UNUSED( op ) )
{
// Nothing to do here
}

void
plD_esc_mem( PLStream *PL_UNUSED( pls ), PLINT PL_UNUSED( op ), void * PL_UNUSED( ptr ) )
{
// Nothing to do here
}

#endif                          // PLD_mem
