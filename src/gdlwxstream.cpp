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

#ifdef HAVE_LIBWXWIDGETS

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
  m_bitmap = new wxBitmap( width, height, 32);
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
}


GDLWXStream::~GDLWXStream()
{
  m_dc->SelectObject( wxNullBitmap );
  delete m_bitmap;
  delete m_dc;
}

void GDLWXStream::SetGDLDrawPanel(GDLDrawPanel* w)
{
  gdlWindow = w;
}

void GDLWXStream::Update()
{
  if( this->valid && gdlWindow != NULL)
    gdlWindow->Update();
}

// void GDLWXStream::set_stream()
// {
// }

void GDLWXStream::SetSize( int width, int height )
{
  m_dc->SelectObject( wxNullBitmap );
  delete m_bitmap;
  m_bitmap = new wxBitmap( width, height, 32 );
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
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLWXStream::GetGeometry(%ld %ld %ld %ld)\n", xSize, ySize, xoff, yoff);
}

unsigned long GDLWXStream::GetWindowDepth() {
  return 24;
}

void GDLWXStream::Clear() {
  ::c_plbop();
}

void GDLWXStream::Clear(DLong bColor) {
  PLINT r0, g0, b0;
  PLINT r1, g1, b1;
  DByte rb, gb, bb;

  // Get current background color
  plgcolbg(&r0, &g0, &b0);

  // Get desired background color
  GDLCT* actCT = GraphicsDevice::GetCT();
  actCT->Get(bColor, rb, gb, bb);

  // Convert to PLINT from GDL_BYTE
  r1 = (PLINT) rb;
  g1 = (PLINT) gb;
  b1 = (PLINT) bb;
  // this mimics better the *DL behaviour.
  ::c_plbop();
  plscolbg(r1, g1, b1);

}

bool GDLWXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
        DLong trueColorOrder, DLong chan) {
  int wxOKforThis=wxMAJOR_VERSION*10000+wxMINOR_VERSION*100+wxRELEASE_NUMBER;
  if (wxOKforThis<30001) return false; //wxWorks bug in earlier versions prevent the following code to work!

  //the following 2 tests cannot happen i think. I keep them for safety.
  if (pls->level < 3) {
    std::cerr << "plimage: window must be set up first" << std::endl; //plabort() not available anymore!
    return false;
  }

  if (nx <= 0 || ny <= 0) {
    std::cerr << "plimage: nx and ny must be positive" << std::endl;
    return false;
  }

  plP_esc( PLESC_FLUSH, NULL );
  wxMemoryDC temp_dc;
  temp_dc.SelectObject(*m_bitmap);
  wxAlphaPixelData data( *m_bitmap );
  if ( !data ) {
    cerr << " ... raw access to bitmap data unavailable, do something else ..." << endl;
    return false;
  }

  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);

  PLINT kxLimit = m_bitmap->GetWidth() - xoff;
  PLINT kyLimit = m_bitmap->GetHeight() - yoff;
  if (nx < kxLimit) kxLimit = nx;
  if (ny < kyLimit) kyLimit = ny;

  if (nx > 0 && ny >0 ) {
    wxAlphaPixelData::Iterator p( data );
    p.OffsetY( data, m_bitmap->GetHeight( ) - yoff - 1 );
    for ( int iy = 0; iy < kyLimit; ++iy ) {
      wxAlphaPixelData::Iterator rowStart = p;
      p.OffsetX( data, xoff );
      for ( int ix = 0; ix < kxLimit; ++ix ) {
        if ( trueColorOrder == 0 && chan == 0 ) {
          p.Red( ) = pls->cmap0[idata[iy * nx + ix]].r;
          p.Green( ) = pls->cmap0[idata[iy * nx + ix]].g;
          p.Blue( ) = pls->cmap0[idata[iy * nx + ix]].b;
        } else {
          if ( chan == 0 ) {
            if ( trueColorOrder == 1 ) {
              p.Red( ) = idata[3 * (iy * nx + ix) + 0];
              p.Green( ) = idata[3 * (iy * nx + ix) + 1];
              p.Blue( ) = idata[3 * (iy * nx + ix) + 2];
            } else if ( trueColorOrder == 2 ) {
              p.Red( ) = idata[nx * (iy * 3 + 0) + ix];
              p.Green( ) = idata[nx * (iy * 3 + 1) + ix];
              p.Blue( ) = idata[nx * (iy * 3 + 2) + ix];
            } else if ( trueColorOrder == 3 ) {
              p.Red( ) = idata[nx * (0 * ny + iy) + ix];
              p.Green( ) = idata[nx * (1 * ny + iy) + ix];
              p.Blue( ) = idata[nx * (2 * ny + iy) + ix];
            }
          } else {
            if ( chan == 1 ) {
              p.Red( ) = idata[1 * (iy * nx + ix) + 0];
            } else if ( chan == 2 ) {
              p.Green( ) = idata[1 * (iy * nx + ix) + 1];
            } else if ( chan == 3 ) {
              p.Blue( ) = idata[1 * (iy * nx + ix) + 2];
            }
          }
        }
        p.Alpha( ) = 255;
        p.OffsetX( data, 1 );
      }
      p = rowStart;
      p.OffsetY( data, -1 );
    }
  }
  m_dc->Blit(0, 0, m_width, m_height, &temp_dc, 0, 0);
  temp_dc.SelectObject( wxNullBitmap);
  *m_bitmap = m_dc->GetAsBitmap(); 
  return true;
}
#endif
