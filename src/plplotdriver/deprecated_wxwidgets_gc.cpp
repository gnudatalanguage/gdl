// Copyright (C) 2008  Werner Smekal
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
#include "deprecated_wxwidgets.h"

#include <wx/string.h>
#include <string>

// only compile code if wxGraphicsContext available
#if wxUSE_GRAPHICS_CONTEXT

wxPLDevGC::wxPLDevGC( void ) : wxPLDevBase( wxBACKEND_GC )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    m_dc       = NULL;
    m_bitmap   = NULL;
    m_context  = NULL;
    m_font     = NULL;
    underlined = false;
}


wxPLDevGC::~wxPLDevGC()
{
    // Log_Verbose( "%s", __FUNCTION__ );

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

    delete m_font;
    delete m_context;
}


void wxPLDevGC::DrawLine( short x1a, short y1a, short x2a, short y2a )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    wxDouble       x1 = x1a / scalex;
    wxDouble       y1 = height - y1a / scaley;
    wxDouble       x2 = x2a / scalex;
    wxDouble       y2 = height - y2a / scaley;

    wxGraphicsPath path = m_context->CreatePath();
    path.MoveToPoint( x1, y1 );
    path.AddLineToPoint( x2, y2 );
    m_context->StrokePath( path );

    AddtoClipRegion( (int) x1, (int) y1, (int) x2, (int) y2 );
}


void wxPLDevGC::DrawPolyline( short *xa, short *ya, PLINT npts )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    wxGraphicsPath path = m_context->CreatePath();
    path.MoveToPoint( xa[0] / scalex, height - ya[0] / scaley );
    for ( PLINT i = 1; i < npts; i++ )
        path.AddLineToPoint( xa[i] / scalex, height - ya[i] / scaley );
    m_context->StrokePath( path );

    wxDouble x, y, w, h;
    path.GetBox( &x, &y, &w, &h );
    AddtoClipRegion( (int) x, (int) y, (int) ( x + w ), (int) ( y + h ) );
}


void wxPLDevGC::ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1, PLINT y1, PLINT x2, PLINT y2 )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    wxDouble x1a, y1a, x2a, y2a;

    if ( x1 < 0 )
        x1a = 0;
    else
        x1a = x1 / scalex;
    if ( y1 < 0 )
        y1a = 0;
    else
        y1a = height - y1 / scaley;
    if ( x2 < 0 )
        x2a = width;
    else
        x2a = x2 / scalex;
    if ( y2 < 0 )
        y2a = height;
    else
        y2a = height - y2 / scaley;

    m_context->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( bgr, bgg, bgb ), 1, wxSOLID ) ) );
    m_context->SetBrush( wxBrush( wxColour( bgr, bgg, bgb ) ) );
    m_context->DrawRectangle( x1a, y1a, x2a - x1a, y2a - y1a );

    m_context->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( mColorRedStroke, mColorGreenStroke,
                                  mColorBlueStroke, mStrokeOpacity ),
                              1, wxSOLID ) ) );
    m_context->SetBrush( wxBrush( wxColour( mColorRedFill, mColorGreenFill, mColorBlueFill, mStrokeOpacity ) ) );

    AddtoClipRegion( (int) x1a, (int) y1a, (int) x2a, (int) y2a );
}


void wxPLDevGC::FillPolygon( PLStream *pls )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    bool isRect = false;
    short* x    = pls->dev_x;
    short* y    = pls->dev_y;

    if ( pls->dev_npts == 4 )     // Check if it's a rectangle. If so, it can be made faster to display
    {
        if ( x[0] == x[1] && x[2] == x[3] && y[0] == y[3] && y[1] == y[2] )
            isRect = true;
        else if ( x[0] == x[3] && x[1] == x[2] && y[0] == y[1] && y[2] == y[3] )
            isRect = true;
    }
    if ( pls->dev_npts == 5 )
    {
        if ( x[0] == x[4] && y[0] == y[4] )
        {
            if ( x[0] == x[1] && x[2] == x[3] && y[0] == y[3] && y[1] == y[2] )
                isRect = true;
            else if ( x[0] == x[3] && x[1] == x[2] && y[0] == y[1] && y[2] == y[3] )
                isRect = true;
        }
    }

    if ( isRect )    //isRect) {
    {
        double x1, y1, x2, y2, x0, y0, w, h;

        x1 = x[0] / scalex;
        x2 = x[2] / scalex;
        y1 = height - y[0] / scaley;
        y2 = height - y[2] / scaley;

        if ( x1 < x2 )
        {
            x0 = x1;
            w  = x2 - x1;
        }
        else
        {
            x0 = x2;
            w  = x1 - x2;
        }
        if ( y1 < y2 )
        {
            y0 = y1;
            h  = y2 - y1;
        }
        else
        {
            y0 = y2;
            h  = y1 - y2;
        }
        m_context->DrawRectangle( x0, y0, w, h );
        AddtoClipRegion( (int) x0, (int) y0, (int) w, (int) h );
    }
    else
    {
        wxGraphicsPath path = m_context->CreatePath();
        path.MoveToPoint( x[0] / scalex, height - y[0] / scaley );
        for ( int i = 1; i < pls->dev_npts; i++ )
            path.AddLineToPoint( x[i] / scalex, height - y[i] / scaley );
        path.CloseSubpath();

        if ( pls->dev_eofill )
            m_context->DrawPath( path, wxODDEVEN_RULE );
        else
            m_context->DrawPath( path, wxWINDING_RULE );

        wxDouble x, y, w, h;
        path.GetBox( &x, &y, &w, &h );

        AddtoClipRegion( (int) x, (int) y, (int) ( x + w ), (int) ( y + h ) );
    }
}


void wxPLDevGC::BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    if ( m_dc )
        dc->Blit( vX, vY, vW, vH, m_dc, vX, vY );
}


void wxPLDevGC::CreateCanvas()
{
    // Log_Verbose( "%s", __FUNCTION__ );

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

    if ( m_dc )
    {
        delete m_context;
        m_context = wxGraphicsContext::Create( *( (wxMemoryDC *) m_dc ) );
    }
}


void wxPLDevGC::SetWidth( PLStream *pls )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    m_context->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( mColorRedStroke, mColorGreenStroke,
                                  mColorBlueStroke, mStrokeOpacity ),
                              pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
}


void wxPLDevGC::SetColor0( PLStream *pls )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    mColorRedStroke   = pls->curcolor.r;
    mColorGreenStroke = pls->curcolor.g;
    mColorBlueStroke  = pls->curcolor.b;
    mColorRedFill     = pls->curcolor.r;
    mColorGreenFill   = pls->curcolor.g;
    mColorBlueFill    = pls->curcolor.b;
    mStrokeOpacity    = (unsigned char) ( pls->curcolor.a * 255 );

    m_context->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( mColorRedStroke, mColorGreenStroke,
                                  mColorBlueStroke, mStrokeOpacity ),
                              pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
    m_context->SetBrush( wxBrush( wxColour( mColorRedFill, mColorGreenFill, mColorBlueFill, mStrokeOpacity ) ) );
}


void wxPLDevGC::SetColor1( PLStream *pls )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    mColorRedStroke   = pls->curcolor.r;
    mColorGreenStroke = pls->curcolor.g;
    mColorBlueStroke  = pls->curcolor.b;
    mColorRedFill     = pls->curcolor.r;
    mColorGreenFill   = pls->curcolor.g;
    mColorBlueFill    = pls->curcolor.b;
    mStrokeOpacity    = (unsigned char) ( pls->curcolor.a * 255 );

    m_context->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( mColorRedStroke, mColorGreenStroke,
                                  mColorBlueStroke, mStrokeOpacity ),
                              pls->width > 0 ? pls->width : 1, wxSOLID ) ) );
    m_context->SetBrush( wxBrush( wxColour( mColorRedFill, mColorGreenFill, mColorBlueFill, mStrokeOpacity ) ) );
}


//--------------------------------------------------------------------------
//  void wx_set_dc( PLStream* pls, wxDC* dc )
//
//  Adds a dc to the stream. The associated device is attached to the canvas
//  as the property "dev".
//--------------------------------------------------------------------------
void wxPLDevGC::SetExternalBuffer( void* dc )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    m_dc      = (wxDC *) dc; // Add the dc to the device
    m_context = wxGraphicsContext::Create( *( (wxMemoryDC *) m_dc ) );
    ready     = true;
    ownGUI    = false;
}


#ifdef PL_HAVE_FREETYPE

void wxPLDevGC::PutPixel( short x, short y, PLINT color )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    const wxPen oldpen = m_dc->GetPen();
    m_context->SetPen( *( wxThePenList->FindOrCreatePen( wxColour( GetRValue( color ), GetGValue( color ), GetBValue( color ) ),
                              1, wxSOLID ) ) );
    //m_context->DrawPoint( x, y );
    AddtoClipRegion( x, y, x, y );
    m_context->SetPen( oldpen );
}

void wxPLDevGC::PutPixel( short x, short y )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    //m_dc->DrawPoint( x, y );
    AddtoClipRegion( x, y, x, y );
}

PLINT wxPLDevGC::GetPixel( short x, short y )
{
    // Log_Verbose( "%s", __FUNCTION__ );

  #ifdef __WXGTK__
    // Cast function parameters to (void) to silence compiler warnings about unused parameters
    (void) x;
    (void) y;
    // The GetPixel method is incredible slow for wxGTK. Therefore we set the colour
    // always to the background color, since this is the case anyway 99% of the time.
    PLINT bgr, bgg, bgb;           // red, green, blue
    plgcolbg( &bgr, &bgg, &bgb );  // get background color information
    return RGB( bgr, bgg, bgb );
#else
    wxColour col;
    m_dc->GetPixel( x, y, &col );
    return RGB( col.Red(), col.Green(), col.Blue() );
#endif
}

#endif // PL_HAVE_FREETYPE


void wxPLDevGC::PSDrawTextToDC( char* utf8_string, bool drawText )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    wxDouble w, h, d, l;

    wxString str( wxConvUTF8.cMB2WC( utf8_string ), *wxConvCurrent );

    w = 0;
    m_context->GetTextExtent( str, &w, &h, &d, &l );

    if ( drawText )
    {
        m_context->DrawText( str, 0, -yOffset / scaley );
        m_context->Translate( w, 0 );
    }

    textWidth += static_cast<int>( w );

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
        textHeight = textHeight > ( currentHeight )
                     ? textHeight
                     : static_cast<int>( ( currentHeight ) );
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
        textHeight = textHeight > currentHeight ? textHeight : static_cast<int>( ( currentHeight ) );
        //work out the additional depth for subscript note an assumption has been made
        //that the font size of (non-superscript and non-subscript) text is the same
        //along a line. Currently there is no escape to change font size mid string
        //so this should be fine
        subscriptDepth = subscriptDepth > ( ( -yOffset / scaley + h + d ) - ( currentDepth + textHeight ) )
                         ? subscriptDepth
                         : static_cast<int>( ( -yOffset / scaley + h + d ) - ( currentDepth + textHeight ) );
        subscriptDepth = subscriptDepth > 0 ? subscriptDepth : 0;
    }
    else
        textHeight = textHeight > h ? textHeight : static_cast<int>( h );

    memset( utf8_string, '\0', max_string_length );
}


void wxPLDevGC::PSSetFont( PLUNICODE fci )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    unsigned char fontFamily, fontStyle, fontWeight;

    plP_fci2hex( fci, &fontFamily, PL_FCI_FAMILY );
    plP_fci2hex( fci, &fontStyle, PL_FCI_STYLE );
    plP_fci2hex( fci, &fontWeight, PL_FCI_WEIGHT );
    if ( m_font )
        delete m_font;
    m_font = wxFont::New( static_cast<int>( fontSize * fontScale ),
        fontFamilyLookup[fontFamily],
        fontStyleLookup[fontStyle] | fontWeightLookup[fontWeight] );
    m_font->SetUnderlined( underlined );
    m_context->SetFont( *m_font, wxColour( textRed, textGreen, textBlue ) );
}


void wxPLDevGC::ProcessString( PLStream* pls, EscText* args )
{
    // Log_Verbose( "%s", __FUNCTION__ );

    // Check that we got unicode, warning message and return if not
    if ( args->unicode_array_len == 0 )
    {
        printf( "Non unicode string passed to a cairo driver, ignoring\n" );
        return;
    }

    // Check that unicode string isn't longer then the max we allow
    if ( args->unicode_array_len >= max_string_length )
    {
        printf( "Sorry, the wxWidgets drivers only handles strings of length < %d\n", max_string_length );
        return;
    }

    // Calculate the font size (in pixels)
    fontSize = pls->chrht * VIRTUAL_PIXELS_PER_MM / scaley * 1.3;

    // Use PLplot core routine to get the corners of the clipping rectangle
    PLINT rcx[4], rcy[4];
    difilt_clip( rcx, rcy );

#ifdef __WXOSX_COCOA__
    wxPoint topLeft( width, height ), bottomRight( -1, -1 );
    for ( int i = 0; i < 4; i++ )
    {
        topLeft.x     = topLeft.x > ( rcx[i] / scalex ) ? ( rcx[i] / scalex ) : topLeft.x;
        topLeft.y     = topLeft.y > ( height - rcy[i] / scaley ) ? ( height - rcy[i] / scaley ) : topLeft.y;
        bottomRight.x = bottomRight.x < ( rcx[i] / scalex ) ? ( rcx[i] / scalex ) : bottomRight.x;
        bottomRight.y = bottomRight.y < ( height - rcy[i] / scaley ) ? ( height - rcy[i] / scaley ) : bottomRight.y;
    }

    m_context->Clip( wxRegion( topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y ) );
    // m_context->Clip( wxRegion( topLeft, bottomRight) );  // this wxRegion constructor doesn't work in wxWidgets 2.9.2/Cocoa
#else
    wxPoint cpoints[4];
    for ( int i = 0; i < 4; i++ )
    {
        cpoints[i].x = rcx[i] / scalex;
        cpoints[i].y = height - rcy[i] / scaley;
    }
    m_context->Clip( wxRegion( 4, cpoints ) );
#endif

    // text color
    textRed   = pls->curcolor.r;
    textGreen = pls->curcolor.g;
    textBlue  = pls->curcolor.b;

    // calculate rotation of text
    plRotationShear( args->xform, &rotation, &shear, &stride );
    rotation -= pls->diorot * M_PI / 2.0;
    cos_rot   = cos( rotation );
    sin_rot   = sin( rotation );
    cos_shear = cos( shear );
    sin_shear = sin( shear );


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
        PSDrawText( lineStart, lineLen, false );

        if ( lineFeed && superscriptHeight > textHeight )
            paraHeight += superscriptHeight - textHeight;

        // actually draw text, resetting the font first
        fontScale = startingFontScale;
        yOffset   = startingYOffset;
        fci       = startingFci;
        PSSetFont( fci );
        m_context->PushState();                                              //save current position
        m_context->Translate( args->x / scalex, height - args->y / scaley ); //move to text starting position
        wxGraphicsMatrix matrix = m_context->CreateMatrix(
            cos_rot * stride, -sin_rot * stride,
            cos_rot * sin_shear + sin_rot * cos_shear,
            -sin_rot * sin_shear + cos_rot * cos_shear,
            0.0, 0.0 );                                                                                //create rotation transformation matrix
        m_context->ConcatTransform( matrix );                                                          //rotate
        m_context->Translate( -args->just * textWidth, -0.5 * textHeight + paraHeight * lineSpacing ); //move to set alignment
        PSDrawText( lineStart, lineLen, true );                                                        //draw text
        m_context->PopState();                                                                         //return to original position

        lineStart += lineLen;
        if ( carriageReturn )
            lineStart++;
        lineLen = 0;
    }


    AddtoClipRegion( 0, 0, width, height );

    m_context->ResetClip();
}

#endif
