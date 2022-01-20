// Copyright (C) 2005  Werner Smekal, Sjaak Verdoold
// Copyright (C) 2005  Germain Carrera Corraleche
// Copyright (C) 1999  Frank Huebner
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

// TODO:
// - text clipping
// - implement AddToClipRegion for text correctly
//

// wxwidgets headers
#include <wx/wx.h>

#include "plDevs.h"

// plplot headers
#include "plplotP.h"

// std and driver headers
#include <cmath>
#include "deprecated_wxwidgets.h"


//--------------------------------------------------------------------------
//  wxPLDevDC::wxPLDevDC( void )
//
//  Constructor of the standard wxWidgets device based on the wxPLDevBase
//  class. Only some initialisations are done.
//--------------------------------------------------------------------------
wxPLDevDC::wxPLDevDC( void ) : wxPLDevBase( wxBACKEND_DC )
{
    m_dc       = NULL;
    m_bitmap   = NULL;
    m_font     = NULL;
    underlined = false;
}


//--------------------------------------------------------------------------
//  wxPLDevDC::~wxPLDevDC( void )
//
//  The deconstructor frees memory allocated by the device.
//--------------------------------------------------------------------------
wxPLDevDC::~wxPLDevDC()
{
    if ( ownGUI )
    {
        if ( m_dc )
        {
            ( (wxMemoryDC *) m_dc )->SelectObject( wxNullBitmap );
            delete m_dc;
        }
        if ( m_bitmap )
            delete m_bitmap;
    }

    if ( m_font )
        delete m_font;
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::DrawLine( short x1a, short y1a, short x2a, short y2a )
//
//  Draw a line from (x1a, y1a) to (x2a, y2a).
//--------------------------------------------------------------------------
void wxPLDevDC::DrawLine( short x1a, short y1a, short x2a, short y2a )
{
    x1a = (short) ( x1a / scalex ); y1a = (short) ( height - y1a / scaley );
    x2a = (short) ( x2a / scalex );        y2a = (short) ( height - y2a / scaley );

    m_dc->DrawLine( (wxCoord) x1a, (wxCoord) y1a, (wxCoord) x2a, (wxCoord) y2a );

    AddtoClipRegion( (int) x1a, (int) y1a, (int) x2a, (int) y2a );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::DrawPolyline( short *xa, short *ya, PLINT npts )
//
//  Draw a poly line - coordinates are in the xa and ya arrays.
//--------------------------------------------------------------------------
void wxPLDevDC::DrawPolyline( short *xa, short *ya, PLINT npts )
{
    wxCoord x1a, y1a, x2a, y2a;

    x2a = (wxCoord) ( xa[0] / scalex );
    y2a = (wxCoord) ( height - ya[0] / scaley );
    for ( PLINT i = 1; i < npts; i++ )
    {
        x1a = x2a; y1a = y2a;
        x2a = (wxCoord) ( xa[i] / scalex );
        y2a = (wxCoord) ( height - ya[i] / scaley );

        m_dc->DrawLine( x1a, y1a, x2a, y2a );

        AddtoClipRegion( (int) x1a, (int) y1a, (int) x2a, (int) y2a );
    }
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb,
//                                   PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
//
//  Clear parts ((x1,y1) to (x2,y2)) of the background in color (bgr,bgg,bgb).
//--------------------------------------------------------------------------
void wxPLDevDC::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb,
                                 PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
{
    if ( x1 < 0 )
        x1 = 0;
    else
        x1 = (PLINT) ( x1 / scalex );
    if ( y1 < 0 )
        y1 = 0;
    else
        y1 = (PLINT) ( height - y1 / scaley );
    if ( x2 < 0 )
        x2 = width;
    else
        x2 = (PLINT) ( x2 / scalex );
    if ( y2 < 0 )
        y2 = height;
    else
        y2 = (PLINT) ( height - y2 / scaley );

    const wxPen   oldPen   = m_dc->GetPen();
    const wxBrush oldBrush = m_dc->GetBrush();

    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( bgr, bgg, bgb ), 1, wxSOLID ) ) );
    m_dc->SetBrush( wxBrush( wxColour( bgr, bgg, bgb ) ) );
    m_dc->DrawRectangle( x1, y1, x2 - x1, y2 - y1 );

    m_dc->SetPen( oldPen );
    m_dc->SetBrush( oldBrush );

    AddtoClipRegion( x1, y1, x2, y2 );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::FillPolygon( PLStream *pls )
//
//  Draw a filled polygon.
//--------------------------------------------------------------------------
void wxPLDevDC::FillPolygon( PLStream *pls )
{
    wxPoint *points = new wxPoint[pls->dev_npts];
    wxCoord xoffset = 0;
    wxCoord yoffset = 0;

    for ( int i = 0; i < pls->dev_npts; i++ )
    {
        points[i].x = (int) ( pls->dev_x[i] / scalex );
        points[i].y = (int) ( height - pls->dev_y[i] / scaley );
        if ( i > 0 )
            AddtoClipRegion( points[i - 1].x, points[i - 1].y, points[i].x, points[i].y );
    }

    if ( pls->dev_eofill )
    {
        m_dc->DrawPolygon( pls->dev_npts, points, xoffset, yoffset, wxODDEVEN_RULE );
    }
    else
    {
        m_dc->DrawPolygon( pls->dev_npts, points, xoffset, yoffset, wxWINDING_RULE );
    }
    delete[] points;
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::BlitRectangle( wxDC* dc, int vX, int vY,
//                                 int vW, int vH )
//
//  Copy/Blit a rectangle ((vX,vY) to (vX+vW,vY+vH)) into given dc.
//--------------------------------------------------------------------------
void wxPLDevDC::BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH )
{
    if ( m_dc )
        dc->Blit( vX, vY, vW, vH, m_dc, vX, vY );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::CreateCanvas( void )
//
//  Create canvas (bitmap and dc) if the driver provides the GUI.
//--------------------------------------------------------------------------
void wxPLDevDC::CreateCanvas()
{
    if ( ownGUI )
    {
        if ( !m_dc )
            m_dc = new wxMemoryDC();

        ( (wxMemoryDC *) m_dc )->SelectObject( wxNullBitmap ); // deselect bitmap
        if ( m_bitmap )
            delete m_bitmap;
        m_bitmap = new wxBitmap( bm_width, bm_height, 32 );
        ( (wxMemoryDC *) m_dc )->SelectObject( *m_bitmap ); // select new bitmap
    }
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::SetWidth( PLStream *pls )
//
//  Set the width of the drawing pen.
//--------------------------------------------------------------------------
void wxPLDevDC::SetWidth( PLStream *pls )
{
    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b ),
                         pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::SetColor0( PLStream *pls )
//
//  Set color from colormap 0.
//--------------------------------------------------------------------------
void wxPLDevDC::SetColor0( PLStream *pls )
{
    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a * 255 ),
                         pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
    m_dc->SetBrush( wxBrush( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a * 255 ) ) );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::SetColor1( PLStream *pls )
//
//  Set color from colormap 1.
//--------------------------------------------------------------------------
void wxPLDevDC::SetColor1( PLStream *pls )
{
    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a * 255 ),
                         pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
    m_dc->SetBrush( wxBrush( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b, pls->curcolor.a * 255 ) ) );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::SetExternalBuffer( void* dc )
//
//  Adds a dc to the device. In that case, the drivers doesn't provide
//  a GUI.
//--------------------------------------------------------------------------
void wxPLDevDC::SetExternalBuffer( void* dc )
{
    m_dc   = (wxDC *) dc; // Add the dc to the device
    ready  = true;
    ownGUI = false;
}


#ifdef PL_HAVE_FREETYPE

//--------------------------------------------------------------------------
//  void wxPLDevDC::PutPixel( short x, short y, PLINT color )
//
//  Draw a pixel in color color @ (x,y).
//--------------------------------------------------------------------------
void wxPLDevDC::PutPixel( short x, short y, PLINT color )
{
    const wxPen oldpen = m_dc->GetPen();
    m_dc->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( GetRValue( color ), GetGValue( color ), GetBValue( color ) ),
                         1, wxSOLID ) ) );
    m_dc->DrawPoint( x, y );
    AddtoClipRegion( x, y, x, y );
    m_dc->SetPen( oldpen );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::PutPixel( short x, short y )
//
//  Draw a pixel in current color @ (x,y).
//--------------------------------------------------------------------------
void wxPLDevDC::PutPixel( short x, short y )
{
    m_dc->DrawPoint( x, y );
    AddtoClipRegion( x, y, x, y );
}


//--------------------------------------------------------------------------
//  PLINT wxPLDevDC::GetPixel( short x, short y )
//
//  Get color information from pixel @ (x,y).
//--------------------------------------------------------------------------
PLINT wxPLDevDC::GetPixel( short x, short y )
{
#ifdef __WXGTK__
    // The GetPixel method is incredible slow for wxGTK. Therefore we set the colour
    // always to the background color, since this is the case anyway 99% of the time.
    PLINT bgr, bgg, bgb;           // red, green, blue
    (void) x;  (void) y;           // Cast to void to silence compiler warnings about unused parameters
    plgcolbg( &bgr, &bgg, &bgb );  // get background color information
    return RGB( bgr, bgg, bgb );
#else
    wxColour col;
    m_dc->GetPixel( x, y, &col );
    return RGB( col.Red(), col.Green(), col.Blue() );
#endif
}

#endif // PL_HAVE_FREETYPE


//--------------------------------------------------------------------------
//  void wxPLDevDC::PSDrawTextToDC( char* utf8_string, bool drawText )
//
//  Draw utf8 text to screen if drawText==true. Otherwise determine
//  width and height of text.
//--------------------------------------------------------------------------
void wxPLDevDC::PSDrawTextToDC( char* utf8_string, bool drawText )
{
    wxCoord  w, h, d, l;

    wxString str( wxConvUTF8.cMB2WC( utf8_string ), *wxConvCurrent );

    m_dc->GetTextExtent( str, &w, &h, &d, &l );

    if ( drawText )
    {
        m_dc->DrawRotatedText( str, (wxCoord) ( posX - yOffset / scaley * sin_rot ),
            (wxCoord) ( height - (wxCoord) ( posY + yOffset * cos_rot / scaley ) ),
            rotation * 180.0 / M_PI );
        posX += (PLINT) ( w * cos_rot );
        posY += (PLINT) ( w * sin_rot );
    }

    textWidth += w;

    //keep track of the height of superscript text, the depth of subscript
    //text and the height of regular text
    if ( yOffset > 0.0001 )
    {
        //determine the height the text would have if it were full size
        double currentOffset = yOffset;
        double currentHeight = h;
        while ( currentOffset > 0.0001 )
        {
            currentOffset -= scaley * fontSize * fontScale / 2.;
            currentHeight *= 1.25;
        }
        textHeight = (wxCoord) textHeight > ( currentHeight )
                     ? textHeight
                     : currentHeight;
        //work out the height including superscript
        superscriptHeight = superscriptHeight > ( currentHeight + yOffset / scaley )
                            ? superscriptHeight
                            : static_cast<int>( ( currentHeight + yOffset / scaley ) );
    }
    else if ( yOffset < -0.0001 )
    {
        //determine the height the text would have if it were full size
        double currentOffset = yOffset;
        double currentHeight = h;
        double currentDepth  = d;
        while ( currentOffset < -0.0001 )
        {
            currentOffset += scaley * fontSize * fontScale * 1.25 / 2.;
            currentHeight *= 1.25;
            currentDepth  *= 1.25;
        }
        textHeight = (wxCoord) textHeight > currentHeight ? textHeight : currentHeight;
        //work out the additional depth for subscript note an assumption has been made
        //that the font size of (non-superscript and non-subscript) text is the same
        //along a line. Currently there is no escape to change font size mid string
        //so this should be fine
        subscriptDepth = (wxCoord) subscriptDepth > ( ( -yOffset / scaley + h + d ) - ( currentDepth + textHeight ) )
                         ? subscriptDepth
                         : ( ( -yOffset / scaley + h + d ) - ( currentDepth + textHeight ) );
        subscriptDepth = subscriptDepth > 0 ? subscriptDepth : 0;
    }
    else
        textHeight = (wxCoord) textHeight > ( h ) ? textHeight : h;

    memset( utf8_string, '\0', max_string_length );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::PSSetFont( PLUNICODE fci )
//
//  Set font defined by fci.
//--------------------------------------------------------------------------
void wxPLDevDC::PSSetFont( PLUNICODE fci )
{
    unsigned char fontFamily, fontStyle, fontWeight;

    plP_fci2hex( fci, &fontFamily, PL_FCI_FAMILY );
    plP_fci2hex( fci, &fontStyle, PL_FCI_STYLE );
    plP_fci2hex( fci, &fontWeight, PL_FCI_WEIGHT );

    if ( m_font )
        delete m_font;

    m_font = wxFont::New( (int) ( fontSize * fontScale < 4 ? 4 : fontSize * fontScale ),
        fontFamilyLookup[fontFamily],
        fontStyleLookup[fontStyle] | fontWeightLookup[fontWeight] );
    m_font->SetUnderlined( underlined );
    m_dc->SetFont( *m_font );
}


//--------------------------------------------------------------------------
//  void wxPLDevDC::ProcessString( PLStream* pls, EscText* args )
//
//  This is the main function which processes the unicode text strings.
//  Font size, rotation and color are set, width and height of the
//  text string is determined and then the string is drawn to the canvas.
//--------------------------------------------------------------------------
void wxPLDevDC::ProcessString( PLStream* pls, EscText* args )
{
    // Check that we got unicode, warning message and return if not
    if ( args->unicode_array_len == 0 )
    {
        printf( "Non unicode string passed to the wxWidgets driver, ignoring\n" );
        return;
    }

    // Check that unicode string isn't longer then the max we allow
    if ( args->unicode_array_len >= 500 )
    {
        printf( "Sorry, the wxWidgets drivers only handles strings of length < %d\n", 500 );
        return;
    }

    // Calculate the font size (in pixels)
    fontSize = pls->chrht * VIRTUAL_PIXELS_PER_MM / scaley * 1.3;

    // Use PLplot core routine to get the corners of the clipping rectangle
    PLINT rcx[4], rcy[4];
    difilt_clip( rcx, rcy );

    wxPoint cpoints[4];
    for ( int i = 0; i < 4; i++ )
    {
        cpoints[i].x = rcx[i] / scalex;
        cpoints[i].y = height - rcy[i] / scaley;
    }
    wxDCClipper clip( *m_dc, wxRegion( 4, cpoints ) );

    // calculate rotation of text
    plRotationShear( args->xform, &rotation, &shear, &stride );
    rotation -= pls->diorot * M_PI / 2.0;
    cos_rot   = cos( rotation );
    sin_rot   = sin( rotation );

    // Set font color
    m_dc->SetTextForeground( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b ) );
    m_dc->SetTextBackground( wxColour( pls->curcolor.r, pls->curcolor.g, pls->curcolor.b ) );

    PLUNICODE *lineStart     = args->unicode_array;
    int       lineLen        = 0;
    bool      lineFeed       = false;
    bool      carriageReturn = false;
    wxCoord   paraHeight     = 0;
    // Get the curent font
    fontScale = 1.0;
    yOffset   = 0.0;
    plgfci( &fci );
    PSSetFont( fci );
    while ( lineStart != args->unicode_array + args->unicode_array_len )
    {
        while ( lineStart + lineLen != args->unicode_array + args->unicode_array_len
                && *( lineStart + lineLen ) != (PLUNICODE) '\n' )
        {
            lineLen++;
        }
        //set line feed for the beginning of this line and
        //carriage return for the end
        lineFeed       = carriageReturn;
        carriageReturn = lineStart + lineLen != args->unicode_array + args->unicode_array_len
                         && *( lineStart + lineLen ) == (PLUNICODE) ( '\n' );
        if ( lineFeed )
            paraHeight += textHeight + subscriptDepth;

        //remember the text parameters so they can be restored
        double    startingFontScale = fontScale;
        double    startingYOffset   = yOffset;
        PLUNICODE startingFci       = fci;

        // determine extent of text
        posX = args->x / scalex;
        posY = args->y / scaley;

        PSDrawText( lineStart, lineLen, false );

        if ( lineFeed && superscriptHeight > textHeight )
            paraHeight += superscriptHeight - textHeight;

        // actually draw text, resetting the font first
        fontScale = startingFontScale;
        yOffset   = startingYOffset;
        fci       = startingFci;
        PSSetFont( fci );
        posX = (PLINT) ( args->x / scalex - ( args->just * textWidth ) * cos_rot - ( 0.5 * textHeight - paraHeight * lineSpacing ) * sin_rot ); //move to set alignment
        posY = (PLINT) ( args->y / scaley - ( args->just * textWidth ) * sin_rot + ( 0.5 * textHeight - paraHeight * lineSpacing ) * cos_rot );
        PSDrawText( lineStart, lineLen, true );                                                                                                 //draw text

        lineStart += lineLen;
        if ( carriageReturn )
            lineStart++;
        lineLen = 0;
    }
    //posX = args->x;
    //posY = args->y;
    //PSDrawText( args->unicode_array, args->unicode_array_len, false );

    //posX = (PLINT) ( args->x - ( ( args->just * textWidth ) * cos_rot + ( 0.5 * textHeight ) * sin_rot ) * scalex );
    //posY = (PLINT) ( args->y - ( ( args->just * textWidth ) * sin_rot - ( 0.5 * textHeight ) * cos_rot ) * scaley );
    //PSDrawText( args->unicode_array, args->unicode_array_len, true );

    AddtoClipRegion( 0, 0, width, height );
}


