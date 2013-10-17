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

#include "includefirst.hpp"

#include <plplot/plplotP.h>

#include "gdlwxstream.hpp"

GDLWXStream::GDLWXStream( wxDC *dc, int width, int height )
: GDLGStream( width, height, "wxwidgets")
  , m_dc(dc), m_width(width), m_height(height)
{
//   ::plstream();
  sdev( "wxwidgets" );
  spage( 0.0, 0.0, m_width, m_height, 0, 0 );
  setopt( "text", "1" ); // use freetype?
  setopt( "smooth", "1" );  // antialiased text?
//   init();

// segv.

  plP_esc( PLESC_DEVINIT, (void*)m_dc );

  set_stream(); // private
//   plgpls( &thePls); // in GDLGStream
}

void GDLWXStream::set_stream()
{
  plstream::set_stream();
}

void GDLWXStream::SetSize( int width, int height )
{
  plP_esc( PLESC_CLEAR, NULL );
  wxSize size = wxSize( width, height);
  plP_esc( PLESC_RESIZE, (void*)&size);
}



void GDLWXStream::Init()
{
  this->plstream::init();

  set_stream(); // private

//   plgpls( &thePls); // in GDLGStream
}


void GDLWXStream::RenewPlot()
{
  plP_esc( PLESC_CLEAR, NULL );
  replot();
}


