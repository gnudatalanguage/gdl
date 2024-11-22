//  Handle plot buffer.
//
//  Copyright (C) 1992  Maurice LeBrun
//  Copyright (C) 2004-2015 Alan W. Irwin
//  Copyright (C) 2005  Thomas J. Duck
//  Copyright (C) 2006  Jim Dishaw
//
//  This file is part of PLplot.
//
//  PLplot is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License as published
//  by the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  PLplot is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public License
//  along with PLplot; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//

#define NEED_PLDEBUG
#include "plplotP.h"
#include "drivers.h"
#include "metadefs.h"

#include <string.h>

#if defined ( _MSC_VER ) && _MSC_VER <= 1500
// Older versions of Visual Studio (2005 perhaps 2008) do not define uint8_t
// The newer versions of Visual Studio will not install on Vista or older
// versions of Windows.
typedef unsigned char        uint8_t;
typedef unsigned short int   uint16_t;
#endif

//
// Function prototypes
//

// Public
void * plbuf_save( PLStream *pls, void *state );

// Private
static void     check_buffer_size( PLStream *pls, size_t data_size );

static int      rd_command( PLStream *pls, U_CHAR *p_c );
static void     rd_data( PLStream *pls, void *buf, size_t buf_size );
static void     rd_data_no_copy( PLStream *pls, void **buf, size_t buf_size );

static void     wr_command( PLStream *pls, U_CHAR c );
static void     wr_data( PLStream *pls, void *buf, size_t buf_size );

static void     plbuf_control( PLStream *pls, U_CHAR c );
static void     plbuf_fill( PLStream *pls );
static void     plbuf_swin( PLStream *pls, PLWindow *plwin );

static void     rdbuf_init( PLStream *pls );
static void     rdbuf_line( PLStream *pls );
static void     rdbuf_polyline( PLStream *pls );
static void     rdbuf_eop( PLStream *pls );
static void     rdbuf_bop( PLStream *pls );
static void     rdbuf_state( PLStream *pls );
static void     rdbuf_esc( PLStream *pls );
static void     rdbuf_image( PLStream *pls );
static void     rdbuf_text( PLStream *pls );
static void     rdbuf_text_unicode( PLINT op, PLStream *pls );
static void     rdbuf_fill( PLStream *pls );
static void     rdbuf_clip( PLStream *pls );
static void     rdbuf_swin( PLStream *pls );
static void     rdbuf_di( PLStream *pls );
static void     rdbuf_setsub( PLStream *pls );
static void     rdbuf_ssub( PLStream *pls );

//--------------------------------------------------------------------------
// Plplot internal interface to the plot buffer
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// plbuf_init()
//
// Initialize device.
//--------------------------------------------------------------------------

void
plbuf_init( PLStream *pls )
{
    dbug_enter( "plbuf_init" );

    // Indicate that this buffer is not being read
    pls->plbuf_read = FALSE;

    if ( pls->plbuf_buffer == NULL )
    {
        // We have not allocated a buffer, so do it now
        pls->plbuf_buffer_grow = 128 * 1024;

        if ( ( pls->plbuf_buffer = malloc( pls->plbuf_buffer_grow ) ) == NULL )
            plexit( "plbuf_init: Error allocating plot buffer." );

        pls->plbuf_buffer_size = pls->plbuf_buffer_grow;
        pls->plbuf_top         = 0;
        pls->plbuf_readpos     = 0;
    }
    else
    {
        // Buffer is allocated, move the top to the beginning
        pls->plbuf_top = 0;
    }
}

//--------------------------------------------------------------------------
// plbuf_eop()
//
// End of page.
//--------------------------------------------------------------------------

void
plbuf_eop( PLStream * pls )
{
    dbug_enter( "plbuf_eop" );

    wr_command( pls, (U_CHAR) EOP );
}

//--------------------------------------------------------------------------
// plbuf_bop()
//
// Set up for the next page.
// To avoid problems redisplaying partially filled pages, on each BOP the
// old data in the buffer is ignored by setting the top back to the
// beginning of the buffer.
//
// Also write state information to ensure the next page is correct.
//--------------------------------------------------------------------------

void
plbuf_bop( PLStream *pls )
{
    dbug_enter( "plbuf_bop" );

    plbuf_tidy( pls );

    // Move the top to the beginning
    pls->plbuf_top = 0;

    wr_command( pls, (U_CHAR) BOP );

    // Save the current configuration (e.g. colormap, current colors) to
    // allow plRemakePlot to correctly regenerate the plot

    // Save the current colors.  Do not use the PLSTATE_* commands
    // because the color might have already been set by the driver
    // during initialization and this would result in an extraneous
    // color command being sent.  The goal is to preserve the current
    // color state so that rdbuf_bop can restore it.  This needs
    // to occur immediately after the BOP command so that it can be
    // read by rdbuf_bop.
    wr_data( pls, &( pls->icol0 ), sizeof ( pls->icol0 ) );
    wr_data( pls, &( pls->icol1 ), sizeof ( pls->icol1 ) );
    wr_data( pls, &( pls->curcolor ), sizeof ( pls->curcolor ) );
    wr_data( pls, &( pls->curcmap ), sizeof ( pls->curcmap ) );
    wr_data( pls, &pls->nsubx, sizeof ( pls->nsubx ) );
    wr_data( pls, &pls->nsuby, sizeof ( pls->nsuby ) );

    // Save all the other state parameters via the plbuf_state function
    plbuf_state( pls, PLSTATE_CMAP0 );
    plbuf_state( pls, PLSTATE_CMAP1 );
    plbuf_state( pls, PLSTATE_WIDTH );
    plbuf_state( pls, PLSTATE_FILL );
    plbuf_state( pls, PLSTATE_CHR );
    plbuf_state( pls, PLSTATE_SYM );
    plbuf_state( pls, PLSTATE_EOFILL );

    //pls->difilt may also be set pre plinit() and is used
    //in plbop for text scaling.
    wr_data( pls, &pls->difilt, sizeof ( pls->difilt ) );
}

//--------------------------------------------------------------------------
//
// plbuf_setsub()
//
// Set the subpage. Required to carry out commands which apply to just one
// sub page such as plclear
//--------------------------------------------------------------------------

void
plbuf_setsub( PLStream * pls )
{
    dbug_enter( "plbuf_setsub" );

    wr_command( pls, (U_CHAR) SETSUB );
    wr_data( pls, &pls->cursub, sizeof ( pls->cursub ) );
}

//--------------------------------------------------------------------------
//
// plbuf_ssub()
//
// Set the number of subpages. Required to carry out commands which apply to
// just one sub page such as plclear
//--------------------------------------------------------------------------

void
plbuf_ssub( PLStream * pls )
{
    dbug_enter( "plbuf_setsub" );

    wr_command( pls, (U_CHAR) SSUB );
    wr_data( pls, &pls->nsubx, sizeof ( pls->nsubx ) );
    wr_data( pls, &pls->nsuby, sizeof ( pls->nsuby ) );
}

//--------------------------------------------------------------------------
// plbuf_tidy()
//
// Close graphics file
//--------------------------------------------------------------------------

void
plbuf_tidy( PLStream * PL_UNUSED( pls ) )
{
    dbug_enter( "plbuf_tidy" );
}

//--------------------------------------------------------------------------
// plbuf_line()
//
// Draw a line in the current color from (x1,y1) to (x2,y2).
//--------------------------------------------------------------------------

void
plbuf_line( PLStream *pls, short x1a, short y1a, short x2a, short y2a )
{
    short xpl[2], ypl[2];

    dbug_enter( "plbuf_line" );

    wr_command( pls, (U_CHAR) LINE );

    xpl[0] = x1a;
    xpl[1] = x2a;
    ypl[0] = y1a;
    ypl[1] = y2a;

    //store the clipping information first
    //wr_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
//    wr_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
//    wr_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
//    wr_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );

    //then the line data
    wr_data( pls, xpl, sizeof ( short ) * 2 );
    wr_data( pls, ypl, sizeof ( short ) * 2 );
}

//--------------------------------------------------------------------------
// plbuf_polyline()
//
// Draw a polyline in the current color.
//--------------------------------------------------------------------------

void
plbuf_polyline( PLStream *pls, short *xa, short *ya, PLINT npts )
{
    dbug_enter( "plbuf_polyline" );

    wr_command( pls, (U_CHAR) POLYLINE );

    //store the clipping information first
    //wr_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
//    wr_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
//    wr_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
//    wr_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );

    //then the number of points
    wr_data( pls, &npts, sizeof ( PLINT ) );

    //then the point data
    wr_data( pls, xa, sizeof ( short ) * (size_t) npts );
    wr_data( pls, ya, sizeof ( short ) * (size_t) npts );
}

//--------------------------------------------------------------------------
// plbuf_state()
//
// Handle change in PLStream state (color, pen width, fill attribute, etc).
//--------------------------------------------------------------------------

void
plbuf_state( PLStream *pls, PLINT op )
{
    dbug_enter( "plbuf_state" );

    wr_command( pls, (U_CHAR) CHANGE_STATE );
    wr_command( pls, (U_CHAR) op );

    switch ( op )
    {
    case PLSTATE_WIDTH:
        wr_data( pls, &( pls->width ), sizeof ( pls->width ) );
        break;

    case PLSTATE_COLOR0:
        wr_data( pls, &( pls->icol0 ), sizeof ( pls->icol0 ) );
        if ( pls->icol0 == PL_RGB_COLOR )
        {
            wr_data( pls, &( pls->curcolor.r ), sizeof ( pls->curcolor.r ) );
            wr_data( pls, &( pls->curcolor.g ), sizeof ( pls->curcolor.g ) );
            wr_data( pls, &( pls->curcolor.b ), sizeof ( pls->curcolor.b ) );
            wr_data( pls, &( pls->curcolor.a ), sizeof ( pls->curcolor.a ) );
        }
        break;

    case PLSTATE_COLOR1:
        wr_data( pls, &( pls->icol1 ), sizeof ( pls->icol1 ) );
        break;

    case PLSTATE_FILL:
        wr_data( pls, &( pls->patt ), sizeof ( pls->patt ) );
        wr_data( pls, &( pls->nps ), sizeof ( pls->nps ) );
        wr_data( pls, &( pls->inclin[0] ), sizeof ( pls->inclin ) );
        wr_data( pls, &( pls->delta[0] ), sizeof ( pls->delta ) );
        break;

    case PLSTATE_CMAP0:
        // Save the number of colors in the palatte
        wr_data( pls, &( pls->ncol0 ), sizeof ( pls->ncol0 ) );
        // Save the color palatte
        wr_data( pls, &( pls->cmap0[0] ), sizeof ( PLColor ) * pls->ncol0 );
        break;

    case PLSTATE_CMAP1:
        // Save the number of colors in the palatte
        wr_data( pls, &( pls->ncol1 ), sizeof ( pls->ncol1 ) );
        // Save the color palatte
        wr_data( pls, &( pls->cmap1[0] ), sizeof ( PLColor ) * pls->ncol1 );
        break;

    case PLSTATE_CHR:
        //save the chrdef and chrht parameters
        wr_data( pls, &( pls->chrdef ), sizeof ( pls->chrdef ) );
        wr_data( pls, &( pls->chrht ), sizeof ( pls->chrht ) );
        break;

    case PLSTATE_SYM:
        //save the symdef and symht parameters
        wr_data( pls, &( pls->symdef ), sizeof ( pls->symdef ) );
        wr_data( pls, &( pls->symht ), sizeof ( pls->symht ) );
        break;
    case PLSTATE_EOFILL:
        //As far as I can see this is only possible as a one way change
        //setting PLStream::dev_eofill to 1. However, in case this
        //behaviour changes in the future we may as well store the value
        wr_data( pls, &( pls->dev_eofill ), sizeof ( pls->dev_eofill ) );
        break;
    }
}


//--------------------------------------------------------------------------
// plbuf_image()
//
// write image described in points pls->dev_x[], pls->dev_y[], pls->dev_z[].
//                      pls->nptsX, pls->nptsY.
//--------------------------------------------------------------------------

static void
plbuf_image( PLStream *pls, IMG_DT *img_dt )
{
    PLINT npts = pls->dev_nptsX * pls->dev_nptsY;

    dbug_enter( "plbuf_image" );

    wr_data( pls, &pls->dev_nptsX, sizeof ( PLINT ) );
    wr_data( pls, &pls->dev_nptsY, sizeof ( PLINT ) );

    wr_data( pls, &img_dt->xmin, sizeof ( PLFLT ) );
    wr_data( pls, &img_dt->ymin, sizeof ( PLFLT ) );
    wr_data( pls, &img_dt->dx, sizeof ( PLFLT ) );
    wr_data( pls, &img_dt->dy, sizeof ( PLFLT ) );

    wr_data( pls, &pls->dev_zmin, sizeof ( short ) );
    wr_data( pls, &pls->dev_zmax, sizeof ( short ) );

    wr_data( pls, pls->dev_ix, sizeof ( short ) * (size_t) npts );
    wr_data( pls, pls->dev_iy, sizeof ( short ) * (size_t) npts );
    wr_data( pls, pls->dev_z,
        sizeof ( unsigned short )
        * (size_t) ( ( pls->dev_nptsX - 1 ) * ( pls->dev_nptsY - 1 ) ) );
}

//--------------------------------------------------------------------------
// plbuf_text()
//
// Handle text call.
//--------------------------------------------------------------------------

static void
plbuf_text( PLStream *pls, EscText *text )
{
    dbug_enter( "plbuf_text" );

    // Check for missing data.
    if ( text == NULL )
        return;

    // Store the state information needed to render the text

    wr_data( pls, &pls->chrht, sizeof ( pls->chrht ) );
    wr_data( pls, &pls->diorot, sizeof ( pls->diorot ) );
    //wr_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
//    wr_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
//    wr_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
//    wr_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );

    // Store the text layout information

    wr_data( pls, &text->base, sizeof ( text->base ) );
    wr_data( pls, &text->just, sizeof ( text->just ) );
    wr_data( pls, text->xform, sizeof ( text->xform[0] ) * 4 );
    wr_data( pls, &text->x, sizeof ( text->x ) );
    wr_data( pls, &text->y, sizeof ( text->y ) );
    wr_data( pls, &text->refx, sizeof ( text->refx ) );
    wr_data( pls, &text->refy, sizeof ( text->refy ) );
    wr_data( pls, &text->font_face, sizeof ( text->font_face ) );

    // Store the text

    if ( pls->dev_unicode )
    {
        PLUNICODE fci;

        // Retrieve and store the font characterization integer
        plgfci( &fci );

        wr_data( pls, &fci, sizeof ( fci ) );

        wr_data( pls, &text->unicode_array_len, sizeof ( U_SHORT ) );
        if ( text->unicode_array_len )
            wr_data( pls,
                text->unicode_array,
                sizeof ( PLUNICODE ) * text->unicode_array_len );
    }
    else
    {
        U_SHORT len;

        // len + 1 to copy the NUL termination
        len = strlen( text->string ) + 1;
        wr_data( pls, &len, sizeof ( len ) );
        if ( len > 0 )
            wr_data( pls, (void *) text->string, sizeof ( char ) * len );
    }
}

//--------------------------------------------------------------------------
// plbuf_text_unicode()
//
// Handle text buffering for the new unicode pathway.
//--------------------------------------------------------------------------

static void
plbuf_text_unicode( PLStream *pls, EscText *text )
{
    PLUNICODE fci;

    dbug_enter( "plbuf_text_unicode" );
}


//--------------------------------------------------------------------------
// plbuf_esc()
//
// Escape function.  Note that any data written must be in device
// independent form to maintain the transportability of the metafile.
//
// Functions:
//
//	PLESC_FILL	    Fill polygon
//	PLESC_SWIN	    Set plot window parameters
//      PLESC_IMAGE         Draw image
//      PLESC_HAS_TEXT      Draw PostScript text
//	PLESC_CLEAR	    Clear Background
//	PLESC_START_RASTERIZE
//	PLESC_END_RASTERIZE Start and stop rasterization
//--------------------------------------------------------------------------

void
plbuf_esc( PLStream *pls, PLINT op, void *ptr )
{
    plbuffer *buffer;
    dbug_enter( "plbuf_esc" );

    wr_command( pls, (U_CHAR) ESCAPE );
    wr_command( pls, (U_CHAR) op );

    switch ( op )
    {
    case PLESC_FILL:
        plbuf_fill( pls );
        break;

    case PLESC_SWIN:
        plbuf_swin( pls, (PLWindow *) ptr );
        break;

    case PLESC_IMAGE:
        plbuf_image( pls, (IMG_DT *) ptr );
        break;

    // Unicode and non-Unicode text handling
    case PLESC_HAS_TEXT:
        plbuf_text( pls, (EscText *) ptr );
        break;

    // Alternate Unicode text handling
    case PLESC_BEGIN_TEXT:
    case PLESC_TEXT_CHAR:
    case PLESC_CONTROL_CHAR:
    case PLESC_END_TEXT:
        // The alternative unicode processing is not correctly implemented
        // and is currently only used by Cairo, which handles its own
        // redraws.  Skip further processing for now

        //plbuf_text_unicode( pls, (EscText *) ptr );
        break;

    case PLESC_IMPORT_BUFFER:
    {
        size_t extraSize;
        buffer    = (plbuffer *) ptr;
        extraSize = buffer->size > pls->plbuf_top ? buffer->size - pls->plbuf_top : 0;
        check_buffer_size( pls, extraSize );
        memcpy( pls->plbuf_buffer, buffer->buffer, buffer->size );
        pls->plbuf_top = buffer->size;
        break;
    }

    case PLESC_APPEND_BUFFER:
        buffer = (plbuffer *) ptr;
        check_buffer_size( pls, buffer->size );
        memcpy( (char *) ( pls->plbuf_buffer ) + pls->plbuf_top, buffer->buffer, buffer->size );
        pls->plbuf_top += buffer->size;
        break;

    case PLESC_FLUSH_REMAINING_BUFFER:
        plFlushBuffer( pls, FALSE, (size_t) ( -1 ) );
        break;

#if 0
    // These are a no-op.  They just need an entry in the buffer.
    case PLESC_CLEAR:
    case PLESC_START_RASTERIZE:
    case PLESC_END_RASTERIZE:
        break;
#endif
    }
}

//--------------------------------------------------------------------------
// plbuf_di()
//
// Driver interface function. Saves all info needed for a call to pldi_ini()
// e.g.orientation etc.
//--------------------------------------------------------------------------
void plbuf_di( PLStream *pls )
{
    wr_command( pls, DRIVER_INTERFACE );

    wr_data( pls, &pls->difilt, sizeof ( pls->difilt ) );
    wr_data( pls, &pls->dipxmin, sizeof ( pls->dipxmin ) );
    wr_data( pls, &pls->dipymin, sizeof ( pls->dipymin ) );
    wr_data( pls, &pls->dipxmax, sizeof ( pls->dipxmax ) );
    wr_data( pls, &pls->dipymax, sizeof ( pls->dipymax ) );
    wr_data( pls, &pls->aspect, sizeof ( pls->aspect ) );
    wr_data( pls, &pls->mar, sizeof ( pls->mar ) );
    wr_data( pls, &pls->jx, sizeof ( pls->jx ) );
    wr_data( pls, &pls->jy, sizeof ( pls->jy ) );
    wr_data( pls, &pls->diorot, sizeof ( pls->diorot ) );
    wr_data( pls, &pls->dimxmin, sizeof ( pls->dimxmin ) );
    wr_data( pls, &pls->dimymin, sizeof ( pls->dimymin ) );
    wr_data( pls, &pls->dimxmax, sizeof ( pls->dimxmax ) );
    wr_data( pls, &pls->dimymax, sizeof ( pls->dimymax ) );
    wr_data( pls, &pls->dimxpmm, sizeof ( pls->dimxpmm ) );
    wr_data( pls, &pls->dimypmm, sizeof ( pls->dimypmm ) );
}

//--------------------------------------------------------------------------
// plbuf_fill()
//
// Fill polygon described in points pls->dev_x[] and pls->dev_y[].
//--------------------------------------------------------------------------

static void
plbuf_fill( PLStream *pls )
{
    dbug_enter( "plbuf_fill" );

    wr_data( pls, &pls->dev_npts, sizeof ( PLINT ) );
    wr_data( pls, pls->dev_x, sizeof ( short ) * (size_t) pls->dev_npts );
    wr_data( pls, pls->dev_y, sizeof ( short ) * (size_t) pls->dev_npts );
}

//--------------------------------------------------------------------------
// plbuf_clip
//
// Set the clipping limits
//--------------------------------------------------------------------------
void
plbuf_clip( PLStream *pls )
{
    dbug_enter( "plbuf_clip" );

    wr_command( pls, (U_CHAR) CLIP );

    wr_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
    wr_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
    wr_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
    wr_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );
}

//--------------------------------------------------------------------------
// plbuf_swin()
//
// Set up plot window parameters.
//--------------------------------------------------------------------------

static void
plbuf_swin( PLStream *pls, PLWindow *plwin )
{
    dbug_enter( "plbuf_swin" );

    wr_data( pls, &plwin->dxmi, sizeof ( plwin->dxmi ) );
    wr_data( pls, &plwin->dxma, sizeof ( plwin->dxma ) );
    wr_data( pls, &plwin->dymi, sizeof ( plwin->dymi ) );
    wr_data( pls, &plwin->dyma, sizeof ( plwin->dyma ) );

    wr_data( pls, &plwin->wxmi, sizeof ( plwin->wxmi ) );
    wr_data( pls, &plwin->wxma, sizeof ( plwin->wxma ) );
    wr_data( pls, &plwin->wymi, sizeof ( plwin->wymi ) );
    wr_data( pls, &plwin->wyma, sizeof ( plwin->wyma ) );
}

//--------------------------------------------------------------------------
// Routines to read from & process the plot buffer.
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// plbuf_write()
//
// Provides an interface for other parts of PLplot (e.g. plmetafile.c) to
// write into the buffer.  The reason why wr_command and wr_data are not
// exposed is to help the optimizer to inline those two functions.
//--------------------------------------------------------------------------
void plbuf_write( PLStream *pls, void *data, size_t bytes )
{
    wr_data( pls, data, bytes );
}

//--------------------------------------------------------------------------
// rdbuf_init()
//
// Initialize device.
//--------------------------------------------------------------------------

static void
rdbuf_init( PLStream * PL_UNUSED( pls ) )
{
    dbug_enter( "rdbuf_init" );
}

//--------------------------------------------------------------------------
// rdbuf_eop()
//
// End of page.
//--------------------------------------------------------------------------

static void
rdbuf_eop( PLStream * PL_UNUSED( pls ) )
{
    dbug_enter( "rdbuf_eop" );
}

//--------------------------------------------------------------------------
// rdbuf_bop()
//
// Set up for the next page.
//--------------------------------------------------------------------------

static void
rdbuf_bop( PLStream *pls )
{
    U_CHAR cmd = 0;
    int    nRead;

    dbug_enter( "rdbuf_bop" );

    pls->nplwin = 0;

    // Read the current color state from the plot buffer
    rd_data( pls, &( pls->icol0 ), sizeof ( pls->icol0 ) );
    rd_data( pls, &( pls->icol1 ), sizeof ( pls->icol1 ) );
    rd_data( pls, &( pls->curcolor ), sizeof ( pls->curcolor ) );
    rd_data( pls, &( pls->curcmap ), sizeof ( pls->curcmap ) );
    rd_data( pls, &( pls->nsubx ), sizeof ( pls->nsubx ) );
    rd_data( pls, &( pls->nsuby ), sizeof ( pls->nsuby ) );

    // We need to read the colormaps that were stored by plbuf_bop
    // now because when plP_state is used to set the color, a wrong
    // colormap will be used if it was changed.

    // Read the command (should be CHANGE_STATE PLSTATE_CMAP0)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading first change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // Read the command (should be CHANGE_STATE PLSTATE_CMAP1)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading second change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // Read the command (should be CHANGE_STATE PLSTATE_WIDTH)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading third change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // Read the command (should be CHANGE_STATE PLSTATE_FILL)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading fourth change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // Read the command (should be CHANGE_STATE PLSTATE_CHR)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading fifth change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // Read the command (should be CHANGE_STATE PLSTATE_SYM)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading sixth change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // Read the command (should be CHANGE_STATE PLSTATE_EOFILL)
    nRead = rd_command( pls, &cmd );
    if ( nRead == 0 || cmd != CHANGE_STATE )
    {
        plabort( "rdbuf_bop: Error reading seventh change state" );
        return;
    }
    plbuf_control( pls, cmd );

    // and now we can set the color
    if ( pls->curcmap == 0 )
    {
        plP_state( PLSTATE_COLOR0 );
    }
    else
    {
        plP_state( PLSTATE_COLOR1 );
    }

    //read DI
    rd_data( pls, &( pls->difilt ), sizeof ( pls->difilt ) );

    plP_bop();
}

//--------------------------------------------------------------------------
// rdbuf_setsub()
//
// set the subpage
//--------------------------------------------------------------------------

static void
rdbuf_setsub( PLStream *pls )
{
    rd_data( pls, (void *) ( &pls->cursub ), sizeof ( pls->cursub ) );
    plP_setsub();
}

//--------------------------------------------------------------------------
// rdbuf_ssub()
//
// set the subpage number of subpages
//--------------------------------------------------------------------------

static void
rdbuf_ssub( PLStream *pls )
{
    rd_data( pls, (void *) ( &pls->nsubx ), sizeof ( pls->nsubx ) );
    rd_data( pls, (void *) ( &pls->nsuby ), sizeof ( pls->nsuby ) );
    c_plssub( pls->nsubx, pls->nsuby );
}

//--------------------------------------------------------------------------
// rdbuf_line()
//
// Draw a line in the current color from (x1,y1) to (x2,y2).
//--------------------------------------------------------------------------

static void
rdbuf_line( PLStream *pls )
{
    short *xpl, *ypl;
    PLINT npts = 2;

    dbug_enter( "rdbuf_line" );

    //read the clipping data first
    //rd_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
//    rd_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
//    rd_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
//    rd_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );

    //then the line data
    // Use the "no copy" version because the endpoint data array does
    // not need to persist outside of this function
    rd_data_no_copy( pls, (void **) &xpl, sizeof ( short ) * (size_t) npts );
    rd_data_no_copy( pls, (void **) &ypl, sizeof ( short ) * (size_t) npts );

    plP_line( xpl, ypl );
}

//--------------------------------------------------------------------------
// rdbuf_polyline()
//
// Draw a polyline in the current color.
//--------------------------------------------------------------------------

static void
rdbuf_polyline( PLStream *pls )
{
    short *xpl, *ypl;
    PLINT npts;

    dbug_enter( "rdbuf_polyline" );

    //read the clipping data first
    //rd_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
//    rd_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
//    rd_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
//    rd_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );

    //then the number of points
    rd_data( pls, &npts, sizeof ( PLINT ) );

    //then the line data
    // Use the "no copy" version because the node data array does
    // not need to persist outside of ths function
    rd_data_no_copy( pls, (void **) &xpl, sizeof ( short ) * (size_t) npts );
    rd_data_no_copy( pls, (void **) &ypl, sizeof ( short ) * (size_t) npts );

    plP_polyline( xpl, ypl, npts );
}

//--------------------------------------------------------------------------
// rdbuf_state()
//
// Handle change in PLStream state (color, pen width, fill attribute, etc).
//--------------------------------------------------------------------------

static void
rdbuf_state( PLStream *pls )
{
    U_CHAR op;

    dbug_enter( "rdbuf_state" );

    rd_data( pls, &op, sizeof ( U_CHAR ) );

    switch ( op )
    {
    case PLSTATE_WIDTH: {
        rd_data( pls, &( pls->width ), sizeof ( pls->width ) );
        plP_state( PLSTATE_WIDTH );

        break;
    }

    case PLSTATE_COLOR0: {
        U_CHAR r, g, b;
        PLFLT  a;

        rd_data( pls, &( pls->icol0 ), sizeof ( pls->icol0 ) );
        if ( pls->icol0 == PL_RGB_COLOR )
        {
            rd_data( pls, &r, sizeof ( U_CHAR ) );
            rd_data( pls, &g, sizeof ( U_CHAR ) );
            rd_data( pls, &b, sizeof ( U_CHAR ) );
            rd_data( pls, &a, sizeof ( U_CHAR ) );
        }
        else
        {
            if ( pls->icol0 >= pls->ncol0 )
            {
                char buffer[256];
                snprintf( buffer, 256,
                    "rdbuf_state: Invalid color map entry: %d",
                    pls->icol0 );
                plabort( buffer );
                return;
            }
            r = pls->cmap0[pls->icol0].r;
            g = pls->cmap0[pls->icol0].g;
            b = pls->cmap0[pls->icol0].b;
            a = pls->cmap0[pls->icol0].a;
        }
        pls->curcolor.r = r;
        pls->curcolor.g = g;
        pls->curcolor.b = b;
        pls->curcolor.a = a;
        pls->curcmap    = 0;

        plP_state( PLSTATE_COLOR0 );
        break;
    }

    case PLSTATE_COLOR1: {
        rd_data( pls, &( pls->icol1 ), sizeof ( pls->icol1 ) );

        pls->curcolor.r = pls->cmap1[pls->icol1].r;
        pls->curcolor.g = pls->cmap1[pls->icol1].g;
        pls->curcolor.b = pls->cmap1[pls->icol1].b;
        pls->curcolor.a = pls->cmap1[pls->icol1].a;
        pls->curcmap    = 1;

        plP_state( PLSTATE_COLOR1 );
        break;
    }

    case PLSTATE_FILL: {
        PLINT patt, nps, inclin[2], delta[2];
        rd_data( pls, &patt, sizeof ( patt ) );
        rd_data( pls, &nps, sizeof ( nps ) );
        rd_data( pls, &inclin[0], sizeof ( inclin ) );
        rd_data( pls, &delta[0], sizeof ( delta ) );
        if ( nps != 0 )
            c_plpat( nps, inclin, delta );
        pls->patt = patt;         //this must be second as c_plpat sets pls->patt to an nvalid value
        break;
    }

    case PLSTATE_CMAP0: {
        PLINT  ncol;
        size_t size;

        rd_data( pls, &ncol, sizeof ( ncol ) );

        // Calculate the memory size for this color palatte
        size = (size_t) ncol * sizeof ( PLColor );

        if ( pls->ncol0 == 0 || pls->ncol0 != ncol )
        {
            // The current palatte is empty or the current palatte is not
            // correctly sized, thus we need allocate a new one

            // If we have a colormap, discard it because we do not use
            // realloc().  We are going to read the colormap from the buffer
            if ( pls->cmap0 != NULL )
                free( pls->cmap0 );

            if ( ( pls->cmap0 = (PLColor *) malloc( size ) ) == NULL )
            {
                plexit( "Insufficient memory for colormap 0" );
            }
        }

        // Now read the colormap from the buffer
        rd_data( pls, &( pls->cmap0[0] ), size );
        pls->ncol0 = ncol;

        plP_state( PLSTATE_CMAP0 );
        break;
    }

    case PLSTATE_CMAP1: {
        PLINT  ncol;
        size_t size;

        rd_data( pls, &ncol, sizeof ( ncol ) );

        // Calculate the memory size for this color palatte
        size = (size_t) ncol * sizeof ( PLColor );

        if ( pls->ncol1 == 0 || pls->ncol1 != ncol )
        {
            // The current palatte is empty or the current palatte is not
            // correctly sized, thus we need allocate a new one

            // If we have a colormap, discard it because we do not use
            // realloc().  We are going to read the colormap from the buffer
            if ( pls->cmap1 != NULL )
                free( pls->cmap1 );

            if ( ( pls->cmap1 = (PLColor *) malloc( size ) ) == NULL )
            {
                plexit( "Insufficient memory for colormap 1" );
            }
        }

        // Now read the colormap from the buffer
        rd_data( pls, &( pls->cmap1[0] ), size );
        pls->ncol1 = ncol;

        plP_state( PLSTATE_CMAP1 );
        break;
    }

    case PLSTATE_CHR: {
        //read the chrdef and chrht parameters
        rd_data( pls, &( pls->chrdef ), sizeof ( pls->chrdef ) );
        rd_data( pls, &( pls->chrht ), sizeof ( pls->chrht ) );
        break;
    }

    case PLSTATE_SYM: {
        //read the symdef and symht parameters
        rd_data( pls, &( pls->symdef ), sizeof ( pls->symdef ) );
        rd_data( pls, &( pls->symht ), sizeof ( pls->symht ) );
        break;
    }

    case PLSTATE_EOFILL: {
        rd_data( pls, &( pls->dev_eofill ), sizeof ( pls->dev_eofill ) );
        plP_state( PLSTATE_EOFILL );
    }
    }
}

//--------------------------------------------------------------------------
// rdbuf_esc()
//
// Escape function.
// Must fill data structure with whatever data that was written,
// then call escape function.
//
// Note: it is best to only call the escape function for op-codes that
// are known to be supported.
//
// Functions:
//
//	PLESC_FILL	    Fill polygon
//	PLESC_SWIN	    Set plot window parameters
//      PLESC_IMAGE         Draw image
//      PLESC_HAS_TEXT      Draw PostScript text
//      PLESC_BEGIN_TEXT    Commands for the alternative unicode text
//      PLESC_TEXT_CHAR     handling path
//      PLESC_CONTROL_CHAR
//      PLESC_END_TEXT
//	PLESC_CLEAR	    Clear Background
//--------------------------------------------------------------------------

static void
rdbuf_esc( PLStream *pls )
{
    U_CHAR op;

    dbug_enter( "rdbuf_esc" );

    rd_data( pls, &op, sizeof ( U_CHAR ) );

    switch ( op )
    {
    case PLESC_FILL:
        rdbuf_fill( pls );
        break;
    case PLESC_SWIN:
        rdbuf_swin( pls );
        break;
    case PLESC_IMAGE:
        rdbuf_image( pls );
        break;
    case PLESC_HAS_TEXT:
        rdbuf_text( pls );
        break;
    case PLESC_BEGIN_TEXT:
    case PLESC_TEXT_CHAR:
    case PLESC_CONTROL_CHAR:
    case PLESC_END_TEXT:
        // Disable for now because alternative unicode processing is
        // not correctly implemented

        //rdbuf_text_unicode( op, pls );
        break;
    case PLESC_IMPORT_BUFFER:
        // Place holder until an appropriate action is determined.
        // Should this even be an ESC operation?
        break;
    case PLESC_CLEAR:
        plP_esc( PLESC_CLEAR, NULL );
        break;
    case PLESC_START_RASTERIZE:
        plP_esc( PLESC_START_RASTERIZE, NULL );
        break;
    case PLESC_END_RASTERIZE:
        plP_esc( PLESC_END_RASTERIZE, NULL );
        break;
    }
}

//--------------------------------------------------------------------------
// rdbuf_fill()
//
// Fill polygon described by input points.
//--------------------------------------------------------------------------

static void
rdbuf_fill( PLStream *pls )
{
    short *xpl, *ypl;
    PLINT npts;

    dbug_enter( "rdbuf_fill" );

    rd_data( pls, &npts, sizeof ( PLINT ) );

    rd_data_no_copy( pls, (void **) &xpl, sizeof ( short ) * (size_t) npts );
    rd_data_no_copy( pls, (void **) &ypl, sizeof ( short ) * (size_t) npts );

    plP_fill( xpl, ypl, npts );
}

//--------------------------------------------------------------------------
//
// rdbuf_clip()
//
//
//--------------------------------------------------------------------------
static void
rdbuf_clip( PLStream *pls )
{
    rd_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
    rd_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
    rd_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
    rd_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );
}

//--------------------------------------------------------------------------
// rdbuf_image()
//
// .
//--------------------------------------------------------------------------

static void
rdbuf_image( PLStream *pls )
{
    // Unnecessarily initialize dev_iy and dev_z to quiet -O1
    // -Wuninitialized warnings which are false alarms.  (If something
    // goes wrong with the dev_ix malloc below any further use of
    // dev_iy and dev_z does not occur.  Similarly, if something goes
    // wrong with the dev_iy malloc below any further use of dev_z
    // does not occur.)
    short          *dev_ix, *dev_iy = NULL;
    unsigned short *dev_z = NULL, dev_zmin, dev_zmax;
    PLINT          nptsX, nptsY, npts;
    PLFLT          xmin, ymin, dx, dy;

    dbug_enter( "rdbuf_image" );

    rd_data( pls, &nptsX, sizeof ( PLINT ) );
    rd_data( pls, &nptsY, sizeof ( PLINT ) );
    npts = nptsX * nptsY;

    rd_data( pls, &xmin, sizeof ( PLFLT ) );
    rd_data( pls, &ymin, sizeof ( PLFLT ) );
    rd_data( pls, &dx, sizeof ( PLFLT ) );
    rd_data( pls, &dy, sizeof ( PLFLT ) );

    rd_data( pls, &dev_zmin, sizeof ( short ) );
    rd_data( pls, &dev_zmax, sizeof ( short ) );

    // NOTE:  Even though for memory buffered version all the data is in memory,
    // we still allocate and copy the data because I think that method works
    // better in a multithreaded environment.  I could be wrong.
    //
    if ( ( ( dev_ix = (short *) malloc( (size_t) npts * sizeof ( short ) ) ) == NULL ) ||
         ( ( dev_iy = (short *) malloc( (size_t) npts * sizeof ( short ) ) ) == NULL ) ||
         ( ( dev_z = (unsigned short *) malloc( (size_t) ( ( nptsX - 1 ) * ( nptsY - 1 ) ) * sizeof ( unsigned short ) ) ) == NULL ) )
        plexit( "rdbuf_image: Insufficient memory" );

    rd_data( pls, dev_ix, sizeof ( short ) * (size_t) npts );
    rd_data( pls, dev_iy, sizeof ( short ) * (size_t) npts );
    rd_data( pls, dev_z,
        sizeof ( unsigned short )
        * (size_t) ( ( nptsX - 1 ) * ( nptsY - 1 ) ) );

    //
    // COMMENTED OUT by Hezekiah Carty
    // Commented (hopefullly temporarily) until the dev_fastimg rendering
    // path can be updated to support the new plimage internals. In the
    // meantime this function is not actually used so the issue of how to
    // update the code to support the new interface can be ignored.
    //
    //plP_image(dev_ix, dev_iy, dev_z, nptsX, nptsY, xmin, ymin, dx, dy, dev_zmin, dev_zmax);

    free( dev_ix );
    free( dev_iy );
    free( dev_z );
}

//--------------------------------------------------------------------------
// rdbuf_swin()
//
// Set up plot window parameters.
//--------------------------------------------------------------------------

static void
rdbuf_swin( PLStream *pls )
{
    PLWindow plwin;

    dbug_enter( "rdbuf_swin" );

    rd_data( pls, &plwin.dxmi, sizeof ( plwin.dxmi ) );
    rd_data( pls, &plwin.dxma, sizeof ( plwin.dxma ) );
    rd_data( pls, &plwin.dymi, sizeof ( plwin.dymi ) );
    rd_data( pls, &plwin.dyma, sizeof ( plwin.dyma ) );

    rd_data( pls, &plwin.wxmi, sizeof ( plwin.wxmi ) );
    rd_data( pls, &plwin.wxma, sizeof ( plwin.wxma ) );
    rd_data( pls, &plwin.wymi, sizeof ( plwin.wymi ) );
    rd_data( pls, &plwin.wyma, sizeof ( plwin.wyma ) );

    plP_swin( &plwin );
}


//--------------------------------------------------------------------------
// rdbuf_swin()
//
// Set up driver interface data
//--------------------------------------------------------------------------
static void
rdbuf_di( PLStream *pls )
{
    PLINT difilt;
    PLFLT rot;
    PLFLT dimxmin, dimxmax, dimymin, dimymax, dimxpmm, dimypmm;
    PLFLT dipxmin, dipymin, dipxmax, dipymax;
    PLFLT aspect, mar, jx, jy;
    rd_data( pls, &difilt, sizeof ( difilt ) );
    rd_data( pls, &dipxmin, sizeof ( dipxmin ) );
    rd_data( pls, &dipymin, sizeof ( dipymin ) );
    rd_data( pls, &dipxmax, sizeof ( dipxmax ) );
    rd_data( pls, &dipymax, sizeof ( dipymax ) );
    rd_data( pls, &aspect, sizeof ( aspect ) );
    rd_data( pls, &mar, sizeof ( mar ) );
    rd_data( pls, &jx, sizeof ( jx ) );
    rd_data( pls, &jy, sizeof ( jy ) );
    rd_data( pls, &rot, sizeof ( rot ) );
    rd_data( pls, &dimxmin, sizeof ( dimxmin ) );
    rd_data( pls, &dimymin, sizeof ( dimymin ) );
    rd_data( pls, &dimxmax, sizeof ( dimxmax ) );
    rd_data( pls, &dimymax, sizeof ( dimymax ) );
    rd_data( pls, &dimxpmm, sizeof ( dimxpmm ) );
    rd_data( pls, &dimypmm, sizeof ( dimypmm ) );
    if ( difilt & PLDI_MAP )
        c_plsdimap( dimxmin, dimxmax, dimymin, dimymax, dimxpmm, dimypmm );
    if ( difilt & PLDI_ORI )
        c_plsdiori( rot );
    if ( difilt & PLDI_PLT )
        c_plsdiplt( dipxmin, dipymin, dipxmax, dipymax );
    if ( difilt & PLDI_DEV )
        c_plsdidev( mar, aspect, jx, jy );
}

//--------------------------------------------------------------------------
// rdbuf_text()
//
// Render text through the driver.
//--------------------------------------------------------------------------

static void
rdbuf_text( PLStream *pls )
{
    EscText text;
    PLFLT   xform[4];

    dbug_enter( "rdbuf_text" );

    text.xform = xform;

    // Read the state information

    rd_data( pls, &pls->chrht, sizeof ( pls->chrht ) );
    rd_data( pls, &pls->diorot, sizeof ( pls->diorot ) );
    //rd_data( pls, &pls->clpxmi, sizeof ( pls->clpxmi ) );
//    rd_data( pls, &pls->clpxma, sizeof ( pls->clpxma ) );
//    rd_data( pls, &pls->clpymi, sizeof ( pls->clpymi ) );
//    rd_data( pls, &pls->clpyma, sizeof ( pls->clpyma ) );

    // Read the text layout information

    rd_data( pls, &text.base, sizeof ( text.base ) );
    rd_data( pls, &text.just, sizeof ( text.just ) );
    rd_data( pls, text.xform, sizeof ( text.xform[0] ) * 4 );
    rd_data( pls, &text.x, sizeof ( text.x ) );
    rd_data( pls, &text.y, sizeof ( text.y ) );
    rd_data( pls, &text.refx, sizeof ( text.refx ) );
    rd_data( pls, &text.refy, sizeof ( text.refy ) );
    rd_data( pls, &text.font_face, sizeof ( text.font_face ) );

    // Initialize text arrays to NULL.  This protects drivers that
    // determine the text representation by looking at which members
    // are set.
    text.unicode_array_len = 0;
    text.unicode_array     = NULL;
    text.string            = NULL;

    // Read in the text
    if ( pls->dev_unicode )
    {
        PLUNICODE fci;

        rd_data( pls, &fci, sizeof ( fci ) );
        plsfci( fci );

        rd_data( pls, &text.unicode_array_len, sizeof ( U_SHORT ) );
        if ( text.unicode_array_len )
        {
            // Set the pointer to the unicode data in the buffer.  This avoids
            // allocating and freeing memory
            rd_data_no_copy(
                pls,
                (void **) ( &text.unicode_array ),
                sizeof ( PLUNICODE ) * text.unicode_array_len );
        }
    }
    else
    {
        U_SHORT len;

        rd_data( pls, &len, sizeof ( len ) );
        if ( len > 0 )
        {
            // Set the pointer to the string data in the buffer.  This avoids
            // allocating and freeing memory
            rd_data_no_copy(
                pls,
                (void **) ( &text.string ),
                sizeof ( char ) * len );
        }
    }

    plP_esc( PLESC_HAS_TEXT, &text );
}

//--------------------------------------------------------------------------
// rdbuf_text_unicode()
//
// Draw text for the new unicode handling pathway.
// This currently does nothing but is here as a placehlder for the future
//--------------------------------------------------------------------------

static void
rdbuf_text_unicode( PLINT op, PLStream *pls )
{
    dbug_enter( "rdbuf_text_unicode" );
}

//--------------------------------------------------------------------------
// plRemakePlot()
//
// Rebuilds plot from plot buffer, usually in response to a window
// resize or exposure event.
//--------------------------------------------------------------------------

void
plRemakePlot( PLStream *pls )
{
    //set the current status to end of page, so that the begin page code
    //gets called
    plsc->page_status = AT_EOP;
    plFlushBuffer( pls, TRUE, (size_t) ( -1 ) );
}

//--------------------------------------------------------------------------
// plFlushBuffer( )
//
// Flush the current contents of the buffer to the plot either restarting
// from the beginning of the buffer or continuing from the current read
// location. Setting amount to -1 will flush to the end, setting it to
// another value flushes until at least that amount has been flushed then
// stops
//--------------------------------------------------------------------------

void
plFlushBuffer( PLStream *pls, PLBOOL restart, size_t amount )
{
    U_CHAR c;
    PLINT  plbuf_write;
    PLINT  cursub;

    dbug_enter( "plRemakePlot" );

    // Change the status of the flags before checking for a buffer.
    // Actually, more thought is needed if we want to support multithreaded
    // code correctly, specifically the case where two threads are using
    // the same plot stream (e.g. one thread is drawing the plot and another
    // thread is processing window manager messages).
    //
    plbuf_write      = pls->plbuf_write;
    cursub           = pls->cursub;
    pls->plbuf_write = FALSE;
    pls->plbuf_read  = TRUE;

    if ( pls->plbuf_buffer )
    {
        // State saving variables
        PLStream *save_current_pls;
        size_t   finalReadPos;

        // Save state

        // Need to change where plsc (current plot stream) points to before
        // processing the commands.  If we have multiple plot streams, this
        // will prevent the commands from going to the wrong plot stream.
        //
        save_current_pls = plsc;

        // Make the current plot stream the one passed by the caller
        plsc = pls;

        if ( restart )
        {
            pls->plbuf_readpos = 0;

            //end any current page on the destination stream.
            //This will do nothing if we are already at the end
            //of a page.
            //Doing this ensures that the first bop command in the
            //buffer actually does something
            //plP_eop();
        }

        finalReadPos = amount == (size_t) ( -1 ) ? pls->plbuf_top : MIN( pls->plbuf_readpos + amount, pls->plbuf_top );

        // Replay the plot command buffer
        while ( rd_command( pls, &c ) && pls->plbuf_readpos < finalReadPos )
        {
            plbuf_control( pls, c );
        }

        // Restore the original current plot stream
        plsc = save_current_pls;
    }

    // Restore the state of the passed plot stream
    pls->plbuf_read  = FALSE;
    pls->plbuf_write = plbuf_write;
    pls->cursub      = cursub;
}

//--------------------------------------------------------------------------
// plbuf_control()
//
// Processes commands read from the plot buffer.
//--------------------------------------------------------------------------

static void
plbuf_control( PLStream *pls, U_CHAR c )
{
    static U_CHAR c_old   = 0;
    static U_CHAR esc_old = 0;

    dbug_enter( "plbuf_control" );

    //#define CLOSE              2
    //#define LINETO             10
    //#define END_OF_FIELD       255

    switch ( (int) c )
    {
    case INITIALIZE:
        rdbuf_init( pls );
        break;

    case EOP:
        rdbuf_eop( pls );
        break;

    case BOP0:
    case BOP:
        rdbuf_bop( pls );
        break;

    case CHANGE_STATE:
        rdbuf_state( pls );
        break;

    case LINE:
        rdbuf_line( pls );
        break;

    case POLYLINE:
        rdbuf_polyline( pls );
        break;

    case ESCAPE:
        esc_old = *( (U_CHAR *) ( pls->plbuf_buffer ) + pls->plbuf_readpos );
        rdbuf_esc( pls );
        break;

    case DRIVER_INTERFACE:
        rdbuf_di( pls );
        break;

    case SETSUB:
        rdbuf_setsub( pls );
        break;

    case SSUB:
        rdbuf_ssub( pls );
        break;

    case CLIP:
        rdbuf_clip( pls );
        break;

    // Obsolete commands, left here to maintain compatibility with previous
    // version of plot metafiles
    case SWITCH_TO_TEXT:    // Obsolete, replaced by ESCAPE
    case SWITCH_TO_GRAPH:   // Obsolete, replaced by ESCAPE
    case NEW_COLOR:         // Obsolete, replaced by CHANGE_STATE
    case NEW_COLOR1:
    case NEW_WIDTH:         // Obsolete, replaced by CHANGE_STATE
    case ADVANCE:           // Obsolete, BOP/EOP used instead
        pldebug( "plbuf_control", "Obsolete command %d, ignoring\n", c );
        break;

    default:
        pldebug( "plbuf_control", "Unrecognized command %d, previous %d\n",
            c, c_old );
        plexit( "Unrecognized command" );
    }
    c_old = c;
}

//--------------------------------------------------------------------------
// rd_command()
//
// Read & return the next command
//--------------------------------------------------------------------------

static int
rd_command( PLStream *pls, U_CHAR *p_c )
{
    int count;

    if ( pls->plbuf_readpos < pls->plbuf_top )
    {
        *p_c = *(U_CHAR *) ( (uint8_t *) pls->plbuf_buffer + pls->plbuf_readpos );

        // Advance the buffer position to maintain two-byte alignment
        pls->plbuf_readpos += sizeof ( uint16_t );

        count = sizeof ( U_CHAR );
    }
    else
    {
        count = 0;
    }

    return ( count );
}

//--------------------------------------------------------------------------
// rd_data()
//
// Read the data associated with the command
//--------------------------------------------------------------------------

static void
rd_data( PLStream *pls, void *buf, size_t buf_size )
{
    memcpy( buf, (uint8_t *) pls->plbuf_buffer + pls->plbuf_readpos, buf_size );

    // Advance position but maintain alignment
    pls->plbuf_readpos += ( buf_size + ( buf_size % sizeof ( uint16_t ) ) );
}

//--------------------------------------------------------------------------
// rd_data_no_copy()
//
// Read the data associated with the command by setting a pointer to the
// position in the plot buffer.  This avoids having to allocate space
// and doing a memcpy.  Useful for commands that do not need the data
// to persist (like LINE and POLYLINE).  Do not use for commands that
// has data that needs to persist or are freed elsewhere (like COLORMAPS).
//--------------------------------------------------------------------------

static void
rd_data_no_copy( PLStream *pls, void **buf, size_t buf_size )
{
    ( *buf ) = (uint8_t *) pls->plbuf_buffer + pls->plbuf_readpos;

    // Advance position but maintain alignment
    pls->plbuf_readpos += ( buf_size + ( buf_size % sizeof ( uint16_t ) ) );
}

//--------------------------------------------------------------------------
// check_buffer_size()
//
// Checks that the buffer has space to store the desired amount of data.
// If not, the buffer is resized to accomodate the request
//--------------------------------------------------------------------------
static void
check_buffer_size( PLStream *pls, size_t data_size )
{
    size_t required_size;

    required_size = pls->plbuf_top + data_size;

    if ( required_size >= pls->plbuf_buffer_size )
    {
        if ( pls->plbuf_buffer_grow == 0 )
            pls->plbuf_buffer_grow = 128 * 1024;

        // Not enough space, need to grow the buffer before memcpy
        // Must make sure the increase is enough for this data, so
        // Determine the amount of space required and grow in multiples
        // of plbuf_buffer_grow
        pls->plbuf_buffer_size += pls->plbuf_buffer_grow *
                                  ( ( required_size
                                      - pls->plbuf_buffer_size )
                                    / pls->plbuf_buffer_grow
                                    + 1 );

        if ( pls->verbose )
            printf( "Growing buffer to %d KB\n",
                (int) ( pls->plbuf_buffer_size / 1024 ) );

        if ( ( pls->plbuf_buffer
                   = realloc( pls->plbuf_buffer, pls->plbuf_buffer_size )
                   ) == NULL )
            plexit( "plbuf buffer grow:  Plot buffer grow failed" );
    }
}

//--------------------------------------------------------------------------
// wr_command()
//
// Write the next command
//--------------------------------------------------------------------------

static void
wr_command( PLStream *pls, U_CHAR c )
{
    check_buffer_size( pls, sizeof ( uint16_t ) );

    *(U_CHAR *) ( (uint8_t *) pls->plbuf_buffer + pls->plbuf_top ) = c;

    // Advance buffer position to maintain two-byte alignment.  This
    // will waste a little bit of space, but it prevents memory
    // alignment problems
    pls->plbuf_top += sizeof ( uint16_t );
}

//--------------------------------------------------------------------------
// wr_data()
//
// Write the data associated with a command
//--------------------------------------------------------------------------

static void
wr_data( PLStream *pls, void *buf, size_t buf_size )
{
    check_buffer_size( pls, buf_size + ( buf_size % sizeof ( uint16_t ) ) );
    memcpy( (uint8_t *) pls->plbuf_buffer + pls->plbuf_top, buf, buf_size );

    // Advance position but maintain alignment
    pls->plbuf_top += ( buf_size + ( buf_size % sizeof ( uint16_t ) ) );
}


//--------------------------------------------------------------------------
// Plot buffer state saving
//--------------------------------------------------------------------------

// plbuf_save(state)
//
// Saves the current state of the plot into a save buffer.  The
// original code used a temporary file for the plot buffer and memory
// to perserve colormaps.  That method does not offer a clean break
// between using memory buffers and file buffers.  This function
// preserves the same functionality by returning a data structure that
// saves the plot buffer.
//
// The caller passes an existing save buffer for reuse or NULL
// to force the allocation of a new buffer.  Since one malloc()
// is used for everything, the entire save buffer can be freed
// with one free() call.
//
//
struct _state
{
    size_t size;             // Size of the save buffer
    int    valid;            // Flag to indicate a valid save state
    void   *plbuf_buffer;
    size_t plbuf_buffer_size;
    size_t plbuf_top;
    size_t plbuf_readpos;
};

void * plbuf_save( PLStream *pls, void *state )
{
    size_t        save_size;
    struct _state *plot_state = (struct _state *) state;
    PLINT         i;
    U_CHAR        *buf; // Assume that this is byte-sized

    dbug_enter( "plbuf_save" );

    // If the plot buffer is not being used, there is no state to save
    if ( !pls->plbuf_write )
        return NULL;

    pls->plbuf_write = FALSE;
    pls->plbuf_read  = TRUE;

    // Determine the size of the buffer required to save everything.
    save_size = sizeof ( struct _state );

    // Only copy as much of the plot buffer that is being used
    save_size += pls->plbuf_top;

    // If a buffer exists, determine if we need to resize it
    if ( state != NULL )
    {
        // We have a save buffer, is it smaller than the current size
        // requirement?
        if ( plot_state->size < save_size )
        {
            // Yes, reallocate a larger one
            if ( ( plot_state = (struct _state *) realloc( state, save_size ) ) == NULL )
            {
                // NOTE: If realloc fails, then plot_state will be NULL.
                // This will leave the original buffer untouched, thus we
                // mark it as invalid and return it back to the caller.
                //
                plwarn( "plbuf: Unable to reallocate sufficient memory to save state" );
                plot_state->valid = 0;

                return state;
            }
            plot_state->size = save_size;
        }
    }
    else
    {
        // A buffer does not exist, so we need to allocate one
        if ( ( plot_state = (struct _state *) malloc( save_size ) ) == NULL )
        {
            plwarn( "plbuf: Unable to allocate sufficient memory to save state" );

            return NULL;
        }
        plot_state->size = save_size;
    }

    // At this point we have an appropriately sized save buffer.
    // We need to invalidate the state of the save buffer, since it
    // will not be valid until after everything is copied.  We use
    // this approach vice freeing the memory and returning a NULL pointer
    // in order to prevent allocating and freeing memory needlessly.
    //
    plot_state->valid = 0;

    // Point buf to the space after the struct _state
    buf = (U_CHAR *) ( plot_state + 1 );

    // Again, note, that we only copy the portion of the plot buffer that
    // is being used
    plot_state->plbuf_buffer_size = pls->plbuf_top;
    plot_state->plbuf_top         = pls->plbuf_top;
    plot_state->plbuf_readpos     = 0;

    // Create a pointer that points in the space we allocated after
    // struct _state
    plot_state->plbuf_buffer = (void *) buf;
    buf += pls->plbuf_top;

    // Copy the plot buffer to our new buffer.  Again, I must stress, that
    // we only are copying the portion of the plot buffer that is being used
    //
    if ( memcpy( plot_state->plbuf_buffer, pls->plbuf_buffer, pls->plbuf_top ) == NULL )
    {
        // This should never be NULL
        plwarn( "plbuf: Got a NULL in memcpy!" );
        return (void *) plot_state;
    }

    pls->plbuf_write = TRUE;
    pls->plbuf_read  = FALSE;

    plot_state->valid = 1;
    return (void *) plot_state;
}

// plbuf_restore(PLStream *, state)
//
// Restores the passed state
//
void plbuf_restore( PLStream *pls, void *state )
{
    struct _state *new_state = (struct _state *) state;

    dbug_enter( "plbuf_restore" );

    pls->plbuf_buffer      = new_state->plbuf_buffer;
    pls->plbuf_buffer_size = new_state->plbuf_buffer_size;
    pls->plbuf_top         = new_state->plbuf_top;
    pls->plbuf_readpos     = new_state->plbuf_readpos;
}

// plbuf_switch(PLStream *, state)
//
// Makes the passed state the current one.  Preserves the previous state
// by returning a save buffer.
//
// NOTE:  The current implementation can cause a memory leak under the
// following scenario:
//    1) plbuf_save() is called
//    2) plbuf_switch() is called
//    3) Commands are called which cause the plot buffer to grow
//    4) plbuf_swtich() is called
//
void * plbuf_switch( PLStream *pls, void *state )
{
    struct _state *new_state = (struct _state *) state;
    struct _state *prev_state;
    size_t        save_size;

    dbug_enter( "plbuf_switch" );

    // No saved state was passed, return a NULL--we hope the caller
    // is smart enough to notice
    //
    if ( state == NULL )
        return NULL;

    if ( !new_state->valid )
    {
        plwarn( "plbuf: Attempting to switch to an invalid saved state" );
        return NULL;
    }

    save_size = sizeof ( struct _state );

    if ( ( prev_state = (struct _state *) malloc( save_size ) ) == NULL )
    {
        plwarn( "plbuf: Unable to allocate memory to save state" );
        return NULL;
    }

    // Set some housekeeping variables
    prev_state->size  = save_size;
    prev_state->valid = 1;

    // Preserve the existing state
    prev_state->plbuf_buffer      = pls->plbuf_buffer;
    prev_state->plbuf_buffer_size = pls->plbuf_buffer_size;
    prev_state->plbuf_top         = pls->plbuf_top;
    prev_state->plbuf_readpos     = pls->plbuf_readpos;

    plbuf_restore( pls, new_state );

    return (void *) prev_state;
}
