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

#ifdef HAVE_LIBWXWIDGETS

#include "includefirst.hpp"

#include <plplot/plstream.h>
#include <plplot/plplotP.h>

#include "gdlwidget.hpp"
#include "gdlwxstream.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif


GDLWXStream::GDLWXStream( int width, int height )
: GDLGStream( width, height, "wxwidgets")
  , m_dc(NULL)
  , m_bitmap(NULL)
  , m_width(width), m_height(height)
  , gdlWindow(NULL)
{
  m_dc = new wxMemoryDC();
  m_bitmap = new wxBitmap( width, height, -1 );
  m_dc->SelectObject( *m_bitmap);
  if( !m_dc->IsOk())
  {
    m_dc->SelectObject( wxNullBitmap );
    delete m_bitmap;
    delete m_dc;
    throw GDLException("GDLWXStream: Failed to create DC.");
  }

  //::plstream();
//   sdev( "wxwidgets" );
  spage( 0.0, 0.0, m_width, m_height, 0, 0 );
//   SETOPT( "text", "1" ); // use freetype?
//   SETOPT( "smooth", "1" );  // antialiased text?
  this->plstream::init();

  plP_esc( PLESC_DEVINIT, (void*)m_dc );

  plstream::set_stream();
//   plgpls( &thePls); // in GDLGStream
}


GDLWXStream::~GDLWXStream()
{
  m_dc->SelectObject( wxNullBitmap );
  delete m_bitmap;
  delete m_dc;
}

void GDLWXStream::SetGDLWindow(GDLWindow* w)
{
  gdlWindow = w;
}

void GDLWXStream::Update()
{
  if( gdlWindow != NULL)
    gdlWindow->Update();
}

// void GDLWXStream::set_stream()
// {
// }

void GDLWXStream::SetSize( int width, int height )
{
  m_dc->SelectObject( wxNullBitmap );
  delete m_bitmap;
  m_bitmap = new wxBitmap( width, height, -1 );
  m_dc->SelectObject( *m_bitmap);
  if( !m_dc->IsOk())
  {
    m_dc->SelectObject( wxNullBitmap );
    delete m_bitmap;
    delete m_dc;
    throw GDLException("GDLWXStream: Failed to resize DC.");
  }

  //   plP_esc( PLESC_CLEAR, NULL );
  wxSize size = wxSize( width, height);
  plP_esc( PLESC_RESIZE, (void*)&size);
  m_width = width;
  m_height = height;
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

void GDLWXStream::GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff)
{
  // plplot does not return the real size
  xSize = m_width;
  ySize = m_height;
  xoff =  0; //false with X11
  yoff =  0; //false with X11
  PLFLT xp; PLFLT yp; 
  PLINT xleng; PLINT yleng;
  PLINT plxoff; PLINT plyoff;
//   plstream::gpage( xp, yp, xleng, yleng, plxoff, plyoff);
//   //warning neither X11 nor plplot give the good value for the position of the window!!!!
//   xoff = plxoff; //not good either!!!
//   yoff = plyoff; // idem
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLWXStream::GetGeometry(%ld %ld %ld %ld)\n", xSize, ySize, xoff, yoff);
}

#endif
