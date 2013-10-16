/***************************************************************************
  gdlwxstream.cpp  - adapted from plplot wxWidgets driver documentation
                             -------------------
    begin                : Wed Oct 16 2013
    copyright            : (C) 2013 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef FOR_IMPORT

#include "gdlwxstream.hpp"


void GDLWXStream::Init()
{
  // plstream::init() calls exit() if it cannot establish a connection with X-server
  if( (my_stream == NULL) || (MemPlotDC_width!=width) || (MemPlotDC_height!=height) ) 
  {
    
    
    memPlotDC->SelectObject( wxNullBitmap );
    if( MemPlotDCBitmap )
      delete MemPlotDCBitmap;

    memPlotDCBitmap = new wxBitmap( width, height, -1 );
    memPlotDC->SelectObject( *MemPlotDCBitmap );

    my_stream = new GDLWXStream( (wxDC*)MemPlotDC, MemPlotDC_width, MemPlotDC_height );

    my_stream->SetSize( width, height);
    my_stream->replot();

    memPlotDC_width = width;
    memPlotDC_height = height;
  }

//      if( !plstreamInitCalled)
//    {
  SetOpt( "text", "1" ); // use freetype?
  SetOpt( "smooth", "1" );  // antialiased text?
   this->plstream::init();
//  		plstreamInitCalled = true;
// 	}
		
  //  set_stream(); // private
  plgpls( &pls);
}


GDLWXStream::GDLWXStream( wxDC *dc, int width, int height )
: GDLGStream( width, height, "wxwidgets")
  m_dc(dc), m_width(width), m_height(height)
{
  init();
  plP_esc( PLESC_DEVINIT, (void*)m_dc );
}

void GDLWXStream::set_stream()
{
  plstream::set_stream();
}

void GDLWXStream::SetSize( int width, int height )
{
  m_width=width;
  m_height=height;
  plP_esc( PLESC_CLEAR, NULL );
  wxSize size( m_width, m_height );
  plP_esc( PLESC_RESIZE, (void*)&size );
}

void GDLWXStream::RenewPlot()
{
  plP_esc( PLESC_CLEAR, NULL );
  replot();
}






//The OnPaint() event handler looks like this (double buffering is used here)
void plotwindow::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
  int width, height;
  GetSize( &width, &height );

	// Check if we window was resized (or dc is invalid)
	if( (my_stream == NULL) || (MemPlotDC_width!=width) || (MemPlotDC_height!=height) ) {
    MemPlotDC->SelectObject( wxNullBitmap );
    if( MemPlotDCBitmap )
      delete MemPlotDCBitmap;
    MemPlotDCBitmap = new wxBitmap( width, height, -1 );
    MemPlotDC->SelectObject( *MemPlotDCBitmap );
		my_stream->SetSize( width, height );
    my_stream->replot();
    MemPlotDC_width = width;
    MemPlotDC_height = height;
	}

  wxPaintDC dc( this );
  dc.SetClippingRegion( GetUpdateRegion() );
  dc.BeginDrawing();
  dc.Blit( 0, 0, width, height, MemPlotDC, 0, 0 );
  dc.EndDrawing();
}

// The whole PLplot API is then available via the my_stream object.
#endif