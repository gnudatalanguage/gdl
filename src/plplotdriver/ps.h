//
// These definitions are for the Postscript (ps.c) and
// Postscript/LaTeX (pstex.c) drivers
//

#ifndef __PS_H__
#define __PS_H__

#include "plunicode-type1.h"

// top level declarations

#define LINELENGTH    78
#define COPIES        1
#define XSIZE         (int) ( pls->xlength * ( pls->xdpi / 72. ) )
#define YSIZE         (int) ( pls->ylength * ( pls->ydpi / 72. ) )
#define ENLARGE       5
#define XPSSIZE       ENLARGE * XSIZE
#define YPSSIZE       ENLARGE * YSIZE
#define XOFFSET       (int) ( pls->xoffset * ( pls->xdpi / 72. ) )   // Margins --
#define YOFFSET       (int) ( pls->yoffset * ( pls->ydpi / 72. ) )   // .5 inches each
#define PSX           XPSSIZE - 1
#define PSY           YPSSIZE - 1
#define OF            pls->OutFile
#define MIN_WIDTH     1.  // Minimum pen width
#define MAX_WIDTH     30. // Maximum pen width
#define DEF_WIDTH     3.  // Default pen width

// These are for covering the page with the background color

#define XMIN    -XOFFSET * ENLARGE
#define XMAX    PSX + XOFFSET * ENLARGE
#define YMIN    -XOFFSET * ENLARGE
#define YMAX    PSY + XOFFSET * ENLARGE

// Struct to hold device-specific info.

typedef struct
{
    PLFLT pxlx, pxly;
    PLINT xold, yold;

    PLINT xmin, xmax, xlen;
    PLINT ymin, ymax, ylen;

    PLINT xmin_dev, xmax_dev, xlen_dev;
    PLINT ymin_dev, ymax_dev, ylen_dev;

    PLFLT xscale_dev, yscale_dev;

    int   llx, lly, urx, ury, ptcnt;
    // font variables.
    int   nlookup, if_symbol_font;
    const Unicode_to_Type1_table *lookup;

    // These are only used by the pstex driver for the additional
    // file required in this case
    long cur_pos;
    FILE *fp;
} PSDev;

void plD_init_pstex( PLStream * );
void plD_line_pstex( PLStream *, short, short, short, short );
void plD_polyline_pstex( PLStream *, short *, short *, PLINT );
void plD_eop_pstex( PLStream * );
void plD_bop_pstex( PLStream * );
void plD_tidy_pstex( PLStream * );
void plD_state_pstex( PLStream *, PLINT );
void plD_esc_pstex( PLStream *, PLINT, void * );

void plD_init_ps( PLStream * );
void plD_init_psc( PLStream * );
void plD_line_ps( PLStream *, short, short, short, short );
void plD_polyline_ps( PLStream *, short *, short *, PLINT );
void plD_eop_ps( PLStream * );
void plD_bop_ps( PLStream * );
void plD_tidy_ps( PLStream * );
void plD_state_ps( PLStream *, PLINT );
void plD_esc_ps( PLStream *, PLINT, void * );

void plD_init_psttf( PLStream * );
void plD_init_psttfc( PLStream * );
void plD_line_psttf( PLStream *, short, short, short, short );
void plD_polyline_psttf( PLStream *, short *, short *, PLINT );
void plD_eop_psttf( PLStream * );
void plD_bop_psttf( PLStream * );
void plD_tidy_psttf( PLStream * );
void plD_state_psttf( PLStream *, PLINT );
void plD_esc_psttf( PLStream *, PLINT, void * );

#endif // __PS_H__
