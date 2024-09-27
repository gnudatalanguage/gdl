// Copyright (C) 2002, 2004, 2005  Andrew Roach
// Copyright (C) 2002  Maurice LeBrun
// Copyright (C) 2002-2014 Alan W. Irwin
// Copyright (C) 2003, 2004  Joao Cardoso
// Copyright (C) 2003, 2004, 2005  Rafael Laboissiere
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
//
//                  Support routines for freetype font engine
//
//  This file contains a series of support routines for drivers interested
//  in using freetype rendered fonts instead of plplot plotter fonts.
//  Freetype supports a gerth of font formats including TrueType, OpenType,
//  Adobe Type1, Type42 etc... the list seems almost endless. Any bitmap
//  driver should be able to use any of these freetype fonts from plplot if
//  these routines are properly initialised.
//
//  Freetype support is not intended to be a "feature" of the common API,
//  but is  implemented as a driver-specific optional extra invoked via the
//  -drvopt command line toggle. It is intended to be used in the context of
//  "PLESC_HAS_TEXT" for any bitmap drivers without native font support.
//  Implementing freetype in this manner minimise changes to the overall
//  API. Because of this approach, there is not a "wealth" of font options
//  available to the programmer. You can not do anything you can't do for a
//  normal freetype plotter font like boldface. You can do most of the
//  things that you can do with a plotter font however, like greek
//  characters superscripting, and selecting one of the four "pre-defined"
//  plplot font types. At present underlining and overlining are not
//  supported.
//
//  To give the user some level of control over the fonts that are used,
//  environmental variables can be set to over-ride the definitions used by
//  the five default plplot fonts.
//
//  The exact syntax for evoking freetype fonts is dependant on each
//  driver, but for the GD and GNUSVGA drivers I have followed the syntax of
//  the PS driver and use the command-line switch of "-drvopt text" to
//  activate the feature, and suggest other programmers do the same for
//  commonality.
//
//  Both anti-aliased and monochrome font rendering is supported by these
//  routines. How these are evoked depends on the programmer, but with the
//  GD and GNUSVGA driver families I have used the command-line switch
//  "-drvopt smooth" to activate the feature; but, considering you also need
//  to turn freetype on, it would probably really be more like "-drvopt
//  text,smooth".
//
//

#include "plConfig.h"
#if !PL_HAVE_UNISTD_H
  #define F_OK    1
  #include <stdio.h>
int access( char *filename, int flag )
{
    FILE *infile;
    infile = fopen( filename, "r" );
    if ( infile != NULL )
    {
        fclose( infile );
        return 0;
    }
    else
    {
        return 1;
    }
}
#else
  #include <unistd.h>
#endif

#define makeunixslash( b )    do { char *I; for ( I = b; *I != 0; *I++ ) if ( *I == '\\' ) *I = '/';} while ( 0 )

#include "plDevs.h"
#include "plplotP.h"
#include "drivers.h"
#ifdef PL_HAVE_FREETYPE
#include "plfreetype.h"
#include "plfci-truetype.h"

#define FT_Data    _FT_Data_

// Font lookup table that is constructed in plD_FreeType_init
PLDLLIMPEXP_DATA( FCI_to_FontName_Table ) FontLookup[N_TrueTypeLookup];
//              TOP LEVEL DEFINES

//  Freetype lets you set the text size absolutely. It also takes into
//  account the DPI when doing so. So does plplot. Why, then, is it that the
//  size of the text drawn by plplot is bigger than the text drawn by
//  freetype when given IDENTICAL parameters ? Perhaps I am missing
//  something somewhere, but to fix this up we use TEXT_SCALING_FACTOR to
//  set a scaling factor to try and square things up a bit.
//

#define TEXT_SCALING_FACTOR    .7

// default size of temporary text buffer
// If we wanted to be fancy we could add sizing, but this should be big enough

#define NTEXT_ALLOC    1024

//--------------------------------------------------------------------------
//  Some debugging macros
//--------------------------------------------------------------------------

#define Debug6( a, b, c, d, e, f )    do { if ( pls->debug ) { fprintf( stderr, a, b, c, d, e, f ); } } while ( 0 )


//              FUNCTION PROTOTYPES

//  Public prototypes, generally available to the API

void plD_FreeType_init( PLStream *pls );
void plD_render_freetype_text( PLStream *pls, EscText *args );
void plD_FreeType_Destroy( PLStream *pls );
void pl_set_extended_cmap0( PLStream *pls, int ncol0_width, int ncol0_org );
void pl_RemakeFreeType_text_from_buffer( PLStream *pls );
void plD_render_freetype_sym( PLStream *pls, EscText *args );

//  Private prototypes for use in this file only

static void FT_PlotChar( PLStream *pls, FT_Data *FT, FT_GlyphSlot slot, int x, int y );
static void FT_SetFace( PLStream *pls, PLUNICODE fci );
static PLFLT CalculateIncrement( int bg, int fg, int levels );

// These are never defined, maybe they will be used in the future?
//
// static void pl_save_FreeType_text_to_buffer (PLStream *pls, EscText *args);
// static FT_ULong hershey_to_unicode (char in);
//
//

static void FT_WriteStrW( PLStream *pls, const PLUNICODE  *text, short len, int x, int y );
static void FT_StrX_YW( PLStream *pls, const PLUNICODE *text, short len, int *xx, int *yy, int *overyy, int *underyy );

//--------------------------------------------------------------------------
// FT_StrX_YW()
//
// Returns the dimensions of the text box. It does this by fully parsing
// the supplied text through the rendering engine. It does everything
// but draw the text. This seems, to me, the easiest and most accurate
// way of determining the text's dimensions. If/when caching is added,
// the CPU hit for this "double processing" will be minimal.
//--------------------------------------------------------------------------

void
FT_StrX_YW( PLStream *pls, const PLUNICODE *text, short len, int *xx, int *yy, int *overyy, int *underyy )
{
    FT_Data   *FT = (FT_Data *) pls->FT;
    short     i   = 0;
    FT_Vector akerning, adjust;
    int       x = 0, y = 0, startingy;
    char      esc;

    plgesc( &esc );

//
// Things seems to work better with this line than without it;
// I guess because there is no vertical kerning or advancement for most
// non-transformed fonts, so we need to define *something* for the y height,
// and this is the best thing I could think of.
//

    y        -= (int) FT->face->size->metrics.height;
    startingy = y;
    *yy       = y; //note height is negative!
    *overyy   = 0;
    *underyy  = 0;
    adjust.x  = 0;
    adjust.y  = 0;

// walk through the text character by character
    for ( i = 0; i < len; i++ )
    {
        if ( ( text[i] == (PLUNICODE) esc ) && ( text[i - 1] != (PLUNICODE) esc ) )
        {
            if ( text[i + 1] == (PLUNICODE) esc )
                continue;

            switch ( text[i + 1] )
            {
            case 'u': // super script
            case 'U': // super script
                adjust.y = FT->face->size->metrics.height / 2;
                adjust.x = 0;
                FT_Vector_Transform( &adjust, &FT->matrix );
                x += (int) adjust.x;
                y -= (int) adjust.y;
                //calculate excess height from superscripts, this will need changing if scale of sub/superscripts changes
                *overyy = y - startingy < *overyy ? y - startingy : *overyy;
                i++;
                break;

            case 'd': // subscript
            case 'D': // subscript
                adjust.y = -FT->face->size->metrics.height / 2;
                adjust.x = 0;
                FT_Vector_Transform( &adjust, &FT->matrix );
                x += (int) adjust.x;
                y -= (int) adjust.y;
                //calculate excess depth from subscripts, this will need changing if scale of sub/superscripts changes
                *underyy = startingy - y < *underyy ? startingy - y : *underyy;
                i++;
                break;
            }
        }
        else if ( text[i] & PL_FCI_MARK )
        {
            // FCI in text stream; change font accordingly.
            FT_SetFace( pls, text[i] );
            *yy = (int) ( FT->face->size->metrics.height > -*yy  ? -FT->face->size->metrics.height : *yy );
        }
        else
        {
            // see if we have kerning for the particular character pair
            if ( ( i > 0 ) && FT_HAS_KERNING( FT->face ) )
            {
                FT_Get_Kerning( FT->face,
                    text[i - 1],
                    text[i],
                    ft_kerning_default,
                    &akerning );
                x += (int) ( akerning.x >> 6 );        // add (or subtract) the kerning
            }

            //
            // Next we load the char. This also draws the char, transforms it, and
            // converts it to a bitmap. At present this is a bit wasteful, but
            // if/when I add cache support, then this data won't go to waste.
            // Since there is no sense in going to the trouble of doing anti-aliasing
            // calculations since we aren't REALLY plotting anything, we will render
            // this as monochrome since it is probably marginally quicker. If/when
            // cache support is added, naturally this will have to change.
            //

            FT_Load_Char( FT->face, text[i], FT_LOAD_MONOCHROME + FT_LOAD_RENDER );

            //
            // Add in the "advancement" needed to position the cursor for the next
            // character. Unless the text is transformed, "y" will always be zero.
            // Y is negative because freetype does things upside down
            //

            x += (int) ( FT->face->glyph->advance.x );
            y -= (int) ( FT->face->glyph->advance.y );
        }
    }

//
// Convert from unit of 1/64 of a pixel to pixels, and do it real fast with
// a bitwise shift (mind you, any decent compiler SHOULD optimise /64 this way
// anyway...)
//

// (RL, on 2005-01-23) Removed the shift bellow to avoid truncation errors
// later.
//yy=y>> 6;
//xx=x>> 6;
//
    *xx = x;
}

//--------------------------------------------------------------------------
// FT_WriteStrW()
//
// Writes a string of FT text at the current cursor location.
// most of the code here is identical to "FT_StrX_Y" and I will probably
// collapse the two into some more efficient code eventually.
//--------------------------------------------------------------------------

void
FT_WriteStrW( PLStream *pls, const PLUNICODE *text, short len, int x, int y )
{
    FT_Data   *FT = (FT_Data *) pls->FT;
    short     i   = 0, last_char = -1;
    FT_Vector akerning, adjust;
    char      esc;

    plgesc( &esc );


//
//  Adjust for the descender - make sure the font is nice and centred
//  vertically. Freetype assumes we have a base-line, but plplot thinks of
//  centre-lines, so that's why we have to do this. Since this is one of our
//  own adjustments, rather than a freetype one, we have to run it through
//  the transform matrix manually.
//
//  For some odd reason, this works best if we triple the
//  descender's height and then adjust the height later on...
//  Don't ask me why, 'cause I don't know. But it does seem to work.
//
//  I really wish I knew *why* it worked better though...
//
//   y-=FT->face->descender >> 6;
//

#ifdef DODGIE_DECENDER_HACK
    adjust.y = ( FT->face->descender >> 6 ) * 3;
#else
    adjust.y = ( FT->face->descender >> 6 );
#endif

// (RL) adjust.y is zeroed below,, making the code above (around
// DODGIE_DECENDER_HACK) completely useless.  This is necessary for
// getting the vertical alignment of text right, which is coped with
// in function plD_render_freetype_text now.
//

    adjust.x = 0;
    adjust.y = 0;
    FT_Vector_Transform( &adjust, &FT->matrix );
    x += (int) adjust.x;
    y -= (int) adjust.y;

// (RL, on 2005-01-25) The computation of cumulated glyph width within
// the text is done now with full precision, using 26.6 Freetype
// arithmetics.  We should then shift the x and y variables by 6 bits,
// as below.  Inside the character for loop, all operations regarding
// x and y will be done in 26.6 mode and  these variables will be
// converted to integers when passed to FT_PlotChar.  Notrice that we
// are using ROUND and float division instead of ">> 6" now.  This
// minimizes truncation errors.
//

    x <<= 6;
    y <<= 6;

// walk through the text character by character

    for ( i = 0; i < len; i++ )
    {
        if ( ( text[i] == (PLUNICODE) esc ) && ( text[i - 1] != (PLUNICODE) esc ) )
        {
            if ( text[i + 1] == (PLUNICODE) esc )
                continue;

            switch ( text[i + 1] )
            {
            //
            //  We run the OFFSET for the super-script and sub-script through the
            //  transformation matrix so we can calculate nice and easy the required
            //  offset no matter what's happened rotation wise. Everything else, like
            //  kerning and advancing from character to character is transformed
            //  automatically by freetype, but since the superscript/subscript is a
            //  feature of plplot, and not freetype, we have to make allowances.
            //

            case 'u': // super script
            case 'U': // super script
                adjust.y = FT->face->size->metrics.height / 2;
                adjust.x = 0;
                FT_Vector_Transform( &adjust, &FT->matrix );
                x += (int) adjust.x;
                y -= (int) adjust.y;
                i++;
                break;

            case 'd': // subscript
            case 'D': // subscript
                adjust.y = -FT->face->size->metrics.height / 2;
                adjust.x = 0;
                FT_Vector_Transform( &adjust, &FT->matrix );
                x += (int) adjust.x;
                y -= (int) adjust.y;
                i++;
                break;
            }
        }
        else if ( text[i] & PL_FCI_MARK )
        {
            // FCI in text stream; change font accordingly.
            FT_SetFace( pls, text[i] );
            FT = (FT_Data *) pls->FT;
            FT_Set_Transform( FT->face, &FT->matrix, &FT->pos );
        }
        else
        {
            // see if we have kerning for the particular character pair
            if ( ( last_char != -1 ) && ( i > 0 ) && FT_HAS_KERNING( FT->face ) )
            {
                FT_Get_Kerning( FT->face,
                    text[last_char],
                    text[i],
                    ft_kerning_default, &akerning );
                x += (int) akerning.x;        // add (or subtract) the kerning
                y -= (int) akerning.y;        // Do I need this in case of rotation ?
            }


            FT_Load_Char( FT->face, text[i], ( FT->smooth_text == 0 ) ? FT_LOAD_MONOCHROME + FT_LOAD_RENDER : FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT );
            FT_PlotChar( pls, FT, FT->face->glyph,
                ROUND( x / 64.0 ), ROUND( y / 64.0 ) );          // render the text

            x += (int) FT->face->glyph->advance.x;
            y -= (int) FT->face->glyph->advance.y;

            last_char = i;
        }
    } // end for
}

//--------------------------------------------------------------------------
// FT_PlotChar()
//
// Plots an individual character. I know some of this stuff, like colour
// could be parsed from plstream, but it was just quicker this way.
//--------------------------------------------------------------------------

void
FT_PlotChar( PLStream *pls, FT_Data *FT, FT_GlyphSlot slot,
             int x, int y )
{
    unsigned char bittest;
    short         i, k, j;
    int           n = slot->bitmap.pitch;
    int           current_pixel_colour;
    int           R, G, B;
    PLFLT         alpha_a;
    //PLFLT         alpha_b;
    int           xx;
    short         imin, imax, kmin, kmax;

    // Corners of the clipping rectangle
    PLINT clipxmin, clipymin, clipxmax, clipymax, tmp;
    PLINT clpxmi, clpxma, clpymi, clpyma;

    // Convert clipping box into normal coordinates
    clipxmin = pls->clpxmi;
    clipxmax = pls->clpxma;
    clipymin = pls->clpymi;
    clipymax = pls->clpyma;

    if ( plsc->difilt )
    {
        difilt( &clipxmin, &clipymin, 1, &clpxmi, &clpxma, &clpymi, &clpyma );
        difilt( &clipxmax, &clipymax, 1, &clpxmi, &clpxma, &clpymi, &clpyma );
    }


    if ( FT->scale != 0.0 )    // scale was set
    {
        clipxmin = (PLINT) ( clipxmin / FT->scale );
        clipxmax = (PLINT) ( clipxmax / FT->scale );
        if ( FT->invert_y == 1 )
        {
            clipymin = (PLINT) ( FT->ymax - ( clipymin / FT->scale ) );
            clipymax = (PLINT) ( FT->ymax - ( clipymax / FT->scale ) );
        }
        else
        {
            clipymin = (PLINT) ( clipymin / FT->scale );
            clipymax = (PLINT) ( clipymax / FT->scale );
        }
    }
    else
    {
        clipxmin = (PLINT) ( clipxmin / FT->scalex );
        clipxmax = (PLINT) ( clipxmax / FT->scalex );

        if ( FT->invert_y == 1 )
        {
            clipymin = (PLINT) ( FT->ymax - ( clipymin / FT->scaley ) );
            clipymax = (PLINT) ( FT->ymax - ( clipymax / FT->scaley ) );
        }
        else
        {
            clipymin = (PLINT) ( clipymin / FT->scaley );
            clipymax = (PLINT) ( clipymax / FT->scaley );
        }
    }
    if ( clipxmin > clipxmax )
    {
        tmp      = clipxmax;
        clipxmax = clipxmin;
        clipxmin = tmp;
    }
    if ( clipymin > clipymax )
    {
        tmp      = clipymax;
        clipymax = clipymin;
        clipymin = tmp;
    }

    // Comment this out as it fails for cases where we want to plot text
    // in the background font, i.e. example 24.
    //
    //if ((slot->bitmap.pixel_mode==ft_pixel_mode_mono)||(pls->icol0==0)) {
    if ( slot->bitmap.pixel_mode == ft_pixel_mode_mono )
    {
        x += slot->bitmap_left;
        y -= slot->bitmap_top;

        imin = (short) MAX( 0, clipymin - y );
        imax = (short) MIN( slot->bitmap.rows, clipymax - y );
        for ( i = imin; i < imax; i++ )
        {
            for ( k = 0; k < n; k++ )
            {
                bittest = 128;
                for ( j = 0; j < 8; j++ )
                {
                    if ( ( bittest & (unsigned char) slot->bitmap.buffer[( i * n ) + k] ) == bittest )
                    {
                        xx = x + ( k * 8 ) + j;
                        if ( ( xx >= clipxmin ) && ( xx <= clipxmax ) )
                            FT->pixel( pls, xx, y + i );
                    }
                    bittest >>= 1;
                }
            }
        }
    }

// this is the anti-aliased stuff

    else
    {
        x += slot->bitmap_left;
        y -= slot->bitmap_top;

        imin = (short) MAX( 0, clipymin - y );
        imax = (short) MIN( slot->bitmap.rows, clipymax - y );
        kmin = (short) MAX( 0, clipxmin - x );
        kmax = (short) MIN( slot->bitmap.width, clipxmax - x );
        for ( i = imin; i < imax; i++ )
        {
            for ( k = kmin; k < kmax; k++ )
            {
                FT->shade = ( slot->bitmap.buffer[( i * slot->bitmap.width ) + k] );
                if ( FT->shade > 0 )
                {
                    if ( ( FT->BLENDED_ANTIALIASING == 1 ) && ( FT->read_pixel != NULL ) )
                    // The New anti-aliasing technique
                    {
                        if ( FT->shade == 255 )
                        {
                            FT->pixel( pls, x + k, y + i );
                        }
                        else
                        {
                            current_pixel_colour = FT->read_pixel( pls, x + k, y + i );

                            G       = GetGValue( current_pixel_colour );
                            R       = GetRValue( current_pixel_colour );
                            B       = GetBValue( current_pixel_colour );
                            alpha_a = (float) FT->shade / 255.0;

                            // alpha_b=1.0-alpha_a;
                            // R=(plsc->curcolor.r*alpha_a)+(R*alpha_b);
                            // G=(plsc->curcolor.g*alpha_a)+(G*alpha_b);
                            // B=(plsc->curcolor.b*alpha_a)+(B*alpha_b);
                            //

                            //  This next bit of code is, I *think*, computationally
                            //  more efficient than the bit above. It results in
                            //  an indistinguishable plot, but file sizes are different
                            //  suggesting subtle variations doubtless caused by rounding
                            //  and/or floating point conversions. Questions are - which is
                            //  better ? Which is more "correct" ? Does it make a difference ?
                            //  Is one faster than the other so that you'd ever notice ?
                            //

                            R = (int) ( ( ( plsc->curcolor.r - R ) * alpha_a ) + R );
                            G = (int) ( ( ( plsc->curcolor.g - G ) * alpha_a ) + G );
                            B = (int) ( ( ( plsc->curcolor.b - B ) * alpha_a ) + B );

                            FT->set_pixel( pls, x + k, y + i, RGB( R > 255 ? 255 : R, G > 255 ? 255 : G, B > 255 ? 255 : B ) );
                        }
                    }
                    else     // The old anti-aliasing technique
                    {
                        FT->col_idx    = FT->ncol0_width - ( ( FT->ncol0_width * FT->shade ) / 255 );
                        FT->last_icol0 = pls->icol0;
                        plcol0( pls->icol0 + ( FT->col_idx * ( FT->ncol0_org - 1 ) ) );
                        FT->pixel( pls, x + k, y + i );
                        plcol0( FT->last_icol0 );
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
// plD_FreeType_init()
//
// Allocates memory to Freetype structure
// Initialises the freetype library.
// Initialises freetype structure
//--------------------------------------------------------------------------

void plD_FreeType_init( PLStream *pls )
{
    FT_Data       *FT;
    char          *a;
// font paths and file names can be long so leave generous (1024) room
    char          font_dir[PLPLOT_MAX_PATH];
    // N.B. must be in exactly same order as TrueTypeLookup
    PLCHAR_VECTOR env_font_names[N_TrueTypeLookup] = {
        "PLPLOT_FREETYPE_SANS_FONT",
        "PLPLOT_FREETYPE_SERIF_FONT",
        "PLPLOT_FREETYPE_MONO_FONT",
        "PLPLOT_FREETYPE_SCRIPT_FONT",
        "PLPLOT_FREETYPE_SYMBOL_FONT",
        "PLPLOT_FREETYPE_SANS_ITALIC_FONT",
        "PLPLOT_FREETYPE_SERIF_ITALIC_FONT",
        "PLPLOT_FREETYPE_MONO_ITALIC_FONT",
        "PLPLOT_FREETYPE_SCRIPT_ITALIC_FONT",
        "PLPLOT_FREETYPE_SYMBOL_ITALIC_FONT",
        "PLPLOT_FREETYPE_SANS_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SERIF_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_MONO_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SCRIPT_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SYMBOL_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SANS_BOLD_FONT",
        "PLPLOT_FREETYPE_SERIF_BOLD_FONT",
        "PLPLOT_FREETYPE_MONO_BOLD_FONT",
        "PLPLOT_FREETYPE_SCRIPT_BOLD_FONT",
        "PLPLOT_FREETYPE_SYMBOL_BOLD_FONT",
        "PLPLOT_FREETYPE_SANS_BOLD_ITALIC_FONT",
        "PLPLOT_FREETYPE_SERIF_BOLD_ITALIC_FONT",
        "PLPLOT_FREETYPE_MONO_BOLD_ITALIC_FONT",
        "PLPLOT_FREETYPE_SCRIPT_BOLD_ITALIC_FONT",
        "PLPLOT_FREETYPE_SYMBOL_BOLD_ITALIC_FONT",
        "PLPLOT_FREETYPE_SANS_BOLD_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SERIF_BOLD_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_MONO_BOLD_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SCRIPT_BOLD_OBLIQUE_FONT",
        "PLPLOT_FREETYPE_SYMBOL_BOLD_OBLIQUE_FONT"
    };
    short         i;

#if defined ( MSDOS ) || defined ( _WIN32 )
    static char *default_font_names[] = { "arial.ttf", "times.ttf", "timesi.ttf", "arial.ttf",
                                          "symbol.ttf" };
    char        WINDIR_PATH[PLPLOT_MAX_PATH];
    char        *b;
    b = getenv( "WINDIR" );
    strncpy( WINDIR_PATH, b, PLPLOT_MAX_PATH - 1 );
    WINDIR_PATH[PLPLOT_MAX_PATH - 1] = '\0';
#else
    PLCHAR_VECTOR default_unix_font_dir = PL_FREETYPE_FONT_DIR;
#endif


    if ( pls->FT )
    {
        plwarn( "Freetype seems already to have been initialised!" );
        return;
    }

    if ( ( pls->FT = calloc( 1, (size_t) sizeof ( FT_Data ) ) ) == NULL )
        plexit( "Could not allocate memory for Freetype" );

    FT = (FT_Data *) pls->FT;

    if ( ( FT->textbuf = calloc( NTEXT_ALLOC, 1 ) ) == NULL )
        plexit( "Could not allocate memory for Freetype text buffer" );

    if ( FT_Init_FreeType( &FT->library ) )
        plexit( "Could not initialise Freetype library" );

    // set to an impossible value for an FCI
    FT->fci = PL_FCI_IMPOSSIBLE;

#if defined ( MSDOS ) || defined ( _WIN32 )

// First check for a user customised location and if
// the fonts aren't found there try the default Windows
// locations
    if ( ( a = getenv( "PLPLOT_FREETYPE_FONT_DIR" ) ) != NULL )
        strncpy( font_dir, a, PLPLOT_MAX_PATH - 1 );
    else if ( strlen( PL_FREETYPE_FONT_DIR ) > 0 )
        strncpy( font_dir, PL_FREETYPE_FONT_DIR, PLPLOT_MAX_PATH - 1 );
    else if ( WINDIR_PATH == NULL )
    {
        //try to guess the font location by looking for arial font on C:
        if ( access( "c:\\windows\\fonts\\arial.ttf", F_OK ) == 0 )
        {
            strcpy( font_dir, "c:/windows/fonts/" );
        }
        else if ( access( "c:\\windows\\system\\arial.ttf", F_OK ) == 0 )
        {
            strcpy( font_dir, "c:/windows/system/" );
        }
        else
            plwarn( "Could not find font path; I sure hope you have defined fonts manually !" );
    }
    else
    {
        //Try to guess the font location by looking for Arial font in the Windows Path
        strncat( WINDIR_PATH, "\\fonts\\arial.ttf", PLPLOT_MAX_PATH - 1 - strlen( WINDIR_PATH ) );
        if ( access( WINDIR_PATH, F_OK ) == 0 )
        {
            b = strrchr( WINDIR_PATH, '\\' );
            b++;
            *b = 0;
            makeunixslash( WINDIR_PATH );
            strcpy( font_dir, WINDIR_PATH );
        }
        else
            plwarn( "Could not find font path; I sure hope you have defined fonts manually !" );
    }
    font_dir[PLPLOT_MAX_PATH - 1] = '\0';

    if ( pls->debug )
        fprintf( stderr, "%s\n", font_dir );
#else

//
//  For Unix systems, we will set the font path up a little differently in
//  that the configured PL_FREETYPE_FONT_DIR has been set as the default path,
//  but the user can override this by setting the environmental variable
//  "PLPLOT_FREETYPE_FONT_DIR" to something else.
//  NOTE WELL - the trailing slash must be added for now !
//

    if ( ( a = getenv( "PLPLOT_FREETYPE_FONT_DIR" ) ) != NULL )
        strncpy( font_dir, a, PLPLOT_MAX_PATH - 1 );
    else
        strncpy( font_dir, default_unix_font_dir, PLPLOT_MAX_PATH - 1 );

    font_dir[PLPLOT_MAX_PATH - 1] = '\0';
#endif

//
// The driver looks for N_TrueTypeLookup  environmental variables
// where the path and name of these fonts can be OPTIONALLY set,
// overriding the configured default values.
//

    for ( i = 0; i < N_TrueTypeLookup; i++ )
    {
        if ( ( a = getenv( env_font_names[i] ) ) != NULL )
        {
//
//  Work out if we have been given an absolute path to a font name, or just
//  a font name sans-path. To do this we will look for a directory separator
//  character, which means some system specific junk. DJGPP is all wise, and
//  understands both Unix and DOS conventions. DOS only knows DOS, and
//  I assume everything else knows Unix-speak. (Why Bill, didn't you just
//  pay the extra 15c and get a REAL separator???)
//

#ifdef MSDOS
            if ( a[1] == ':' )                        // check for MS-DOS absolute path
#else
            if ( ( a[0] == '/' ) || ( a[0] == '~' ) ) // check for unix abs path
#endif
                strncpy( FT->font_name[i], a, PLPLOT_MAX_PATH - 1 );

            else
            {
                strncpy( FT->font_name[i], font_dir, PLPLOT_MAX_PATH - 1 );
                strncat( FT->font_name[i], a, PLPLOT_MAX_PATH - 1 - strlen( FT->font_name[i] ) );
            }
        }
        else
        {
            strncpy( FT->font_name[i], font_dir, PLPLOT_MAX_PATH - 1 );
            strncat( FT->font_name[i], (PLCHAR_VECTOR) TrueTypeLookup[i].pfont, PLPLOT_MAX_PATH - 1 - strlen( FT->font_name[i] ) );
        }
        FT->font_name[i][PLPLOT_MAX_PATH - 1] = '\0';

        {
            FILE *infile;
            if ( ( infile = fopen( FT->font_name[i], "r" ) ) == NULL )
            {
                char msgbuf[1024];
                snprintf( msgbuf, 1024,
                    "plD_FreeType_init: Could not find the freetype compatible font:\n %s",
                    FT->font_name[i] );
                plwarn( msgbuf );
            }
            else
            {
                fclose( infile );
            }
        }
        FontLookup[i].fci = TrueTypeLookup[i].fci;
        if ( FT->font_name[i][0] == '\0' )
            FontLookup[i].pfont = NULL;
        else
            FontLookup[i].pfont = (unsigned char *) FT->font_name[i];
    }
//
// Next, we check to see if -drvopt has been used on the command line to
// over-ride any settings
//
}


//--------------------------------------------------------------------------
// FT_SetFace( PLStream *pls, PLUNICODE fci )
//
// Sets up the font face and size
//--------------------------------------------------------------------------

void FT_SetFace( PLStream *pls, PLUNICODE fci )
{
    FT_Data *FT       = (FT_Data *) pls->FT;
    double  font_size = pls->chrht * 72 / 25.4; // font_size in points, chrht is in mm

    // save a copy of character height and resolution
    FT->chrht = pls->chrht;
    FT->xdpi  = pls->xdpi;
    FT->ydpi  = pls->ydpi;

    if ( fci != FT->fci )
    {
        PLCHAR_VECTOR font_name = plP_FCI2FontName( fci, FontLookup, N_TrueTypeLookup );
        if ( font_name == NULL )
        {
            if ( FT->fci == PL_FCI_IMPOSSIBLE )
                plexit( "FT_SetFace: Bad FCI and no previous valid font to fall back on" );
            else
                plwarn( "FT_SetFace: Bad FCI.  Falling back to previous font." );
        }
        else
        {
            FT->fci = fci;

            if ( FT->face != NULL )
            {
                FT_Done_Face( FT->face );
                FT->face = NULL;
            }

            if ( FT->face == NULL )
            {
                if ( FT_New_Face( FT->library, font_name, 0, &FT->face ) )
                    plexit( "FT_SetFace: Error loading a font in freetype" );
            }

            //check if the charmap was loaded correctly - freetype only checks for a unicode charmap
            //if it is not set then use the first found charmap in the font
            if ( FT->face->charmap == NULL )
                FT_Select_Charmap( FT->face, FT->face->charmaps[0]->encoding );
        }
    }
    FT_Set_Char_Size( FT->face, 0,
        (FT_F26Dot6) ( font_size * 64 / TEXT_SCALING_FACTOR ), (FT_UInt) pls->xdpi,
        (FT_UInt) pls->ydpi );
}

//--------------------------------------------------------------------------
// plD_render_freetype_text()
//
// Transforms the font
// calculates real-world bitmap coordinates from plplot ones
// renders text using freetype
//--------------------------------------------------------------------------

void plD_render_freetype_text( PLStream *pls, EscText *args )
{
    FT_Data   *FT = (FT_Data *) pls->FT;
    int       x, y;
    int       w  = 0, h = 0, overh = 0, underh = 0;
    PLFLT     *t = args->xform;
    FT_Matrix matrix;
    PLFLT     angle = PI * pls->diorot / 2;
    PLUNICODE *line = args->unicode_array;
    int       linelen;
    int       prevlineheights = 0;

// Used later in a commented out section (See Rotate The Page), if that
// section will never be used again, remove these as well.
//      PLINT clxmin, clxmax, clymin, clymax;
//
    PLFLT     Sin_A, Cos_A;
    FT_Vector adjust;
    PLUNICODE fci;
    FT_Fixed  height;
    PLFLT     height_factor;

    if ( ( args->unicode_array_len > 0 ) )
    {
//
//   Work out if either the font size, the font face or the
//   resolution has changed.
//   If either has, then we will reload the font face.
//
        plgfci( &fci );
        if ( ( FT->fci != fci ) || ( FT->chrht != pls->chrht ) || ( FT->xdpi != pls->xdpi ) || ( FT->ydpi != pls->ydpi ) )
            FT_SetFace( pls, fci );


//  this will help work out underlining and overlining

        Debug6( "%s %d %d %d %d\n", "plD_render_freetype_text:",
            FT->face->underline_position >> 6,
            FT->face->descender >> 6,
            FT->face->ascender >> 6,
            ( ( FT->face->underline_position * -1 ) + FT->face->ascender ) >> 6 );



//
// Split the text into lines based on the newline character
//
        while ( line < args->unicode_array + args->unicode_array_len )
        {
            linelen = 0;
            while ( line[linelen] != '\n' && line + linelen < args->unicode_array + args->unicode_array_len )
                ++linelen;

//
//  Now we work out how long the text is (for justification etc...) and how
//  high the text is. This is done on UN-TRANSFORMED text, since we will
//  apply our own transformations on it later, so it's necessary for us
//  to to turn all transformations off first, before calling the function
//  that calculates the text size.
//

            FT->matrix.xx = 0x10000;
            FT->matrix.xy = 0x00000;
            FT->matrix.yx = 0x00000;
            FT->matrix.yy = 0x10000;

            FT_Vector_Transform( &FT->pos, &FT->matrix );
            FT_Set_Transform( FT->face, &FT->matrix, &FT->pos );

            FT_StrX_YW( pls, line, (short) linelen, &w, &h, &overh, &underh );

//
//      Set up the transformation Matrix
//
// Fortunately this is almost identical to plplot's own transformation matrix;
// you have NO idea how much effort that saves ! Some params are in a
// different order, and Freetype wants integers whereas plplot likes floats,
// but such differences are quite trivial.
//
// For some odd reason, this needs to be set a different way for DJGPP. Why ?
// I wish I knew.
//

// (RL, on 2005-01-21) The height_factor variable is introduced below.
// It is used here and farther below when computing the vertical
// adjustment.  The rationale for its introduction is as follow: up to
// now, the text produced with Hershey fonts was systematically taller
// than the same text produced with TT fonts, and tha by a factor of
// around 1.125 (I discovered this empirically).  This corresponds
// roughly to the ratio between total height and the ascender of some
// TT faces.  Hence the computation below.  Remember that descender is
// always a negative quantity.
//

            height_factor = (PLFLT) ( FT->face->ascender - FT->face->descender )
                            / FT->face->ascender;
            height = (FT_Fixed) ( 0x10000 * height_factor );

#ifdef DJGPP
            FT->matrix.xx = (FT_Fixed) ( (PLFLT) height * t[0] );
            FT->matrix.xy = (FT_Fixed) ( (PLFLT) height * t[2] );
            FT->matrix.yx = (FT_Fixed) ( (PLFLT) height * t[1] );
            FT->matrix.yy = (FT_Fixed) ( (PLFLT) height * t[3] );
#else
            FT->matrix.xx = (FT_Fixed) ( (PLFLT) height * t[0] );
            FT->matrix.xy = (FT_Fixed) ( (PLFLT) height * t[1] );
            FT->matrix.yx = (FT_Fixed) ( (PLFLT) height * t[2] );
            FT->matrix.yy = (FT_Fixed) ( (PLFLT) height * t[3] );
#endif


//                            Rotate the Font
//
//  If the page has been rotated using -ori, this is where we rotate the
//  font to point in the right direction. To make things nice and easy, we
//  will use freetypes matrix math stuff to do this for us.
//

            Cos_A = cos( angle );
            Sin_A = sin( angle );

            matrix.xx = (FT_Fixed) ( (PLFLT) 0x10000 * Cos_A );

#ifdef DJGPP
            matrix.xy = (FT_Fixed) ( (PLFLT) 0x10000 * Sin_A * -1.0 );
            matrix.yx = (FT_Fixed) ( (PLFLT) 0x10000 * Sin_A );
#else
            matrix.xy = (FT_Fixed) ( (PLFLT) 0x10000 * Sin_A );
            matrix.yx = (FT_Fixed) ( (PLFLT) 0x10000 * Sin_A * -1.0 );
#endif

            matrix.yy = (FT_Fixed) ( (PLFLT) 0x10000 * Cos_A );

            FT_Matrix_Multiply( &matrix, &FT->matrix );


//       Calculate a Vector from the matrix
//
// This is closely related to the "transform matrix".
// The matrix is used for rendering the glyph, while the vector is used for
// calculating offsets of the text box, so we need both. Why ? I dunno, but
// we have to live with it, and it works...
//


            FT_Vector_Transform( &FT->pos, &FT->matrix );


//    Transform the font face
//
// This is where our matrix transformation is calculated for the font face.
// This is only done once for each unique transformation since it is "sticky"
// within the font. Font rendering is done later, using the supplied matrix,
// but invisibly to us from here on. I don't believe the vector is used, but
// it is asked for.
//

            FT_Set_Transform( FT->face, &FT->matrix, &FT->pos );


//                            Rotate the Page
//
//  If the page has been rotated using -ori, this is we recalculate the
//  reference point for the text using plplot functions.
//

//   difilt(&args->x, &args->y, 1, &clxmin, &clxmax, &clymin, &clymax);


//
//   Convert into normal coordinates from virtual coordinates
//

            if ( FT->scale != 0.0 )             // scale was set
            {
                x = (int) ( args->x / FT->scale );

                if ( FT->invert_y == 1 )
                    y = (int) ( FT->ymax - ( args->y / FT->scale ) );
                else
                    y = (int) ( args->y / FT->scale );
            }
            else
            {
                x = (int) ( args->x / FT->scalex );

                if ( FT->invert_y == 1 )
                    y = (int) ( FT->ymax - ( args->y / FT->scaley ) );
                else
                    y = (int) ( args->y / FT->scaley );
            }

            //          Adjust for the justification and character height
            //
            //  Eeeksss... this wasn't a nice bit of code to work out, let me tell you.
            //  I could not work out an entirely satisfactory solution that made
            //  logical sense, so came up with an "illogical" one as well.
            //  The logical one works fine for text in the normal "portrait"
            //  orientation, and does so for reasons you might expect it to work; But
            //  for all other orientations, the text's base line is either a little
            //  high, or a little low. This is because of the way the base-line pos
            //  is calculated from the decender height. The "dodgie" way of calculating
            //  the position is to use the character height here, then adjust for the
            //  decender height by a three-fold factor later on. That approach seems to
            //  work a little better for rotated pages, but why it should be so, I
            //  don't understand. You can compile in or out which way you want it by
            //  defining "DODGIE_DECENDER_HACK".
            //
            //  note: the logic of the page rotation coming up next is that we pump in
            //  the justification factor and then use freetype to rotate and transform
            //  the values, which we then use to change the plotting location.
            //


#ifdef DODGIE_DECENDER_HACK
            adjust.y = h;
#else
            adjust.y = 0;
#endif

// (RL, on 2005-01-24) The code below uses floating point and division
// operations instead of integer shift used before. This is slower but
// gives accurate placement of text in plots.
//

// (RL, on 2005-01-21) The hack below is intended to align single
// glyphs being generated via plpoin.  The way to detect this
// situation is completely hackish, I must admit, by checking whether the
// length of the Unicode array is equal 2 and whether the first
// character is actually a font-changing command to font number 4 (for
// symbols).  This is ugly because it depends on definitions set
// elsewhere, but it works.
//
// The computation of the vertical and horizontal adjustments are
// based on the bouding box of the glyph being loaded (since there is
// only one glyph in the string in this case, we are okay here).
//

            if ( ( args->unicode_array_len == 2 )
                 && ( args->unicode_array[0] == ( PL_FCI_MARK | 0x004 ) ) )
            {
                adjust.x = (FT_Pos) ( args->just * ROUND( (PLFLT) FT->face->glyph->metrics.width / 64.0 ) );
                adjust.y = (FT_Pos) ROUND( (PLFLT) FT->face->glyph->metrics.height / 128.0 );
            }
            else
            {
// (RL, on 2005-01-21) The vertical adjustment is set below, making
// the DODGIE conditional moot.  I use the value of h as return by FT_StrX_YW,
// which should correspond to the total height of the text being
// drawn.  Freetype aligns text around the baseline, while PLplot
// aligns to the center of the ascender portion.  We must then adjust
// by half of the ascender and this is why there is a division by
// height_factor below.
//

                adjust.y = (FT_Pos)
                           ROUND( (PLFLT) FT->face->size->metrics.height / height_factor / 128.0 - ( prevlineheights + overh ) / 64.0 );
                adjust.x = (FT_Pos) ( args->just * ROUND( w / 64.0 ) );
            }

            FT_Vector_Transform( &adjust, &FT->matrix );             // was /&matrix); -  was I using the wrong matrix all this time ?

            x -= (int) adjust.x;
            y += (int) adjust.y;

            FT_WriteStrW( pls, line, (short) linelen, x, y );             // write it out

//
// Move to the next line
//
            line            += linelen + 1;
            prevlineheights += h + overh + underh;
        }
    }
    else
    {
        plD_render_freetype_sym( pls, args );
    }
}

//--------------------------------------------------------------------------
// plD_FreeType_Destroy()
//
// Restores cmap0 if it had been modifed for anti-aliasing
// closes the freetype library.
// Deallocates memory to the Freetype structure
//--------------------------------------------------------------------------

void plD_FreeType_Destroy( PLStream *pls )
{
    FT_Data *FT = (FT_Data *) pls->FT;
    //extern int FT_Done_Library( FT_Library library );

    if ( FT )
    {
        if ( ( FT->smooth_text == 1 ) && ( FT->BLENDED_ANTIALIASING == 0 ) )
            plscmap0n( FT->ncol0_org );
        if ( FT->textbuf )
            free( FT->textbuf );
        FT_Done_Library( FT->library );
        free( pls->FT );
        pls->FT = NULL;
    }
}

//--------------------------------------------------------------------------
// PLFLT CalculateIncrement( int bg, int fg, int levels)
//
// Takes the value of the foreground, and the background, and when
// given the number of desired steps, calculates how much to incriment
// a value to transition from fg to bg.
// This function only does it for one colour channel at a time.
//--------------------------------------------------------------------------

static PLFLT CalculateIncrement( int bg, int fg, int levels )
{
    PLFLT ret = 0;

    if ( levels > 1 )
    {
        if ( fg > bg )
            ret = ( ( fg + 1 ) - bg ) / levels;
        else if ( fg < bg )
            ret = ( ( ( fg - 1 ) - bg ) / levels );
    }
    return ( ret );
}

//--------------------------------------------------------------------------
// void pl_set_extended_cmap0(PLStream *pls, int ncol0_width, int ncol0_org)
//
//  ncol0_width - how many greyscale levels to accolate to each CMAP0 entry
//  ncol0_org - the originl number of CMAP0 entries.
//
//  This function calcualtes and sets an extended CMAP0 entry for the
//  driver. It is assumed that the caller has checked to make sure there is
//  room for extending CMAP0 already.
//
//  NOTES
//  We don't bother calculating an entry for CMAP[0], the background.
//  It is assumed the caller has already expanded the size of CMAP[0]
//--------------------------------------------------------------------------

void pl_set_extended_cmap0( PLStream *pls, int ncol0_width, int ncol0_org )
{
    int   i, j, k;
    int   r, g, b;
    PLFLT r_inc, g_inc, b_inc;

    for ( i = 1; i < ncol0_org; i++ )
    {
        r = pls->cmap0[i].r;
        g = pls->cmap0[i].g;
        b = pls->cmap0[i].b;

        r_inc = CalculateIncrement( pls->cmap0[0].r, r, ncol0_width );
        g_inc = CalculateIncrement( pls->cmap0[0].g, g, ncol0_width );
        b_inc = CalculateIncrement( pls->cmap0[0].b, b, ncol0_width );

        for ( j = 0, k = ncol0_org + i - 1; j < ncol0_width; j++, k += ( ncol0_org - 1 ) )
        {
            r -= (int) r_inc;
            g -= (int) g_inc;
            b -= (int) b_inc;
            if ( ( r < 0 ) || ( g < 0 ) || ( b < 0 ) )
                plscol0( k, 0, 0, 0 );
            else
                plscol0( k, ( r > 0xff ? 0xff : r ), ( g > 0xff ? 0xff : g ), ( b > 0xff ? 0xff : b ) );
        }
    }
}


//--------------------------------------------------------------------------
// plD_render_freetype_sym( PLStream *pls, EscText *args )
//   PLStream *pls - pointer to plot stream
//   EscText *args - pointer to standard "string" object.
//
//  This function is a simple rendering function which draws a single
//  character at a time. The function is an alternative to the text
//  functions which are considerably, and needlessly, more complicated
//  than what we need here.
//--------------------------------------------------------------------------


void plD_render_freetype_sym( PLStream *pls, EscText *args )
{
    FT_Data   *FT = (FT_Data *) pls->FT;
    int       x, y;
    FT_Vector adjust;
    PLUNICODE fci;

    if ( FT->scale != 0.0 )    // scale was set
    {
        x = (int) ( args->x / FT->scale );

        if ( FT->invert_y == 1 )
            y = (int) ( FT->ymax - ( args->y / FT->scale ) );
        else
            y = (int) ( args->y / FT->scale );
    }
    else
    {
        x = (int) ( args->x / FT->scalex );

        if ( FT->invert_y == 1 )
            y = (int) ( FT->ymax - ( args->y / FT->scaley ) );
        else
            y = (int) ( args->y / FT->scaley );
    }


//
//  Adjust for the descender - make sure the font is nice and centred
//  vertically. Freetype assumes we have a base-line, but plplot thinks of
//  centre-lines, so that's why we have to do this. Since this is one of our
//  own adjustments, rather than a freetype one, we have to run it through
//  the transform matrix manually.
//
//  For some odd reason, this works best if we triple the
//  descender's height and then adjust the height later on...
//  Don't ask me why, 'cause I don't know. But it does seem to work.
//
//  I really wish I knew *why* it worked better though...
//
//   y-=FT->face->descender >> 6;
//

#ifdef DODGIE_DECENDER_HACK
    adjust.y = ( FT->face->descender >> 6 ) * 3;
#else
    adjust.y = ( FT->face->descender >> 6 );
#endif

    adjust.x = 0;
    FT_Vector_Transform( &adjust, &FT->matrix );
    x += (int) adjust.x;
    y -= (int) adjust.y;

    plgfci( &fci );
    FT_SetFace( pls, fci );

    FT = (FT_Data *) pls->FT;
    FT_Set_Transform( FT->face, &FT->matrix, &FT->pos );

    FT_Load_Char( FT->face, args->unicode_char, ( FT->smooth_text == 0 ) ? FT_LOAD_MONOCHROME + FT_LOAD_RENDER : FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT );

//
// Now we have to try and componsate for the fact that the freetype glyphs are left
// justified, and plplot's glyphs are centred. To do this, we will just work out the
// advancment, halve it, and take it away from the x position. This wont be 100%
// accurate because "spacing" is factored into the right hand side of the glyph,
// but it is as good a way as I can think of.
//

    x -= (int) ( ( FT->face->glyph->advance.x >> 6 ) / 2 );
    FT_PlotChar( pls, FT, FT->face->glyph, x, y ); // render the text
}




#else
int
plfreetype()
{
    return 0;
}

#endif
