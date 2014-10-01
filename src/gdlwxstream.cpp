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
  SETOPT( "text", "1" ); // use freetype?
  SETOPT( "smooth", "1" );  // antialiased text?
  this->plstream::init();
  plstream::cmd(PLESC_DEVINIT, (void*)m_dc );

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

  wxSize size = wxSize( width, height);
  plstream::cmd(PLESC_RESIZE, (void*)&size );
  m_width = width;
  m_height = height;
}

void GDLWXStream::WarpPointer(DLong x, DLong y) {
//  int xx=x;
//  int yy=y;
//  wxWindow *w =  m_dc->GetWindow();
//  w->WarpPointer(xx,yy); //crash??
}

//bool GDLWXStream::GetGin(PLGraphicsIn *gin, int mode) {
//
//  enum CursorOpt {
//    NOWAIT = 0,
//    WAIT, //1
//    CHANGE, //2
//    DOWN, //3
//    UP //4
//  };
//  return true;
//}

void GDLWXStream::Init()
{
  this->plstream::init();

  set_stream(); // private
// test :  gdlFrame->Show();
}


void GDLWXStream::RenewPlot()
{
  plstream::cmd( PLESC_CLEAR, NULL );
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

//FALSE: REPLACE With Clear(DLong chan) as in X
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
  plstream::cmd( PLESC_FLUSH, NULL );
  wxMemoryDC temp_dc;
  temp_dc.SelectObject(*m_bitmap);
  wxImage image=m_bitmap->ConvertToImage();
  unsigned char* mem=image.GetData();
  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);

  PLINT xsize = m_width;
  PLINT ysize = m_height;
  PLINT kxLimit = xsize - xoff;
  PLINT kyLimit = ysize - yoff;
  if (nx < kxLimit) kxLimit = nx;
  if (ny < kyLimit) kyLimit = ny;

  if ( nx > 0 && ny > 0 ) {
    SizeT p = (ysize - yoff - 1)*3*xsize;
    for ( int iy = 0; iy < kyLimit; ++iy ) {
      SizeT rowStart = p;
      p += xoff*3;
      for ( int ix = 0; ix < kxLimit; ++ix ) {
        if ( trueColorOrder == 0 && chan == 0 ) {
          mem[p++] = pls->cmap0[idata[iy * nx + ix]].r;
          mem[p++] = pls->cmap0[idata[iy * nx + ix]].g;
          mem[p++] = pls->cmap0[idata[iy * nx + ix]].b;
        } else {
          if ( chan == 0 ) {
            if ( trueColorOrder == 1 ) {
              mem[p++] = idata[3 * (iy * nx + ix) + 0]; 
              mem[p++] = idata[3 * (iy * nx + ix) + 1];
              mem[p++] = idata[3 * (iy * nx + ix) + 2];
            } else if ( trueColorOrder == 2 ) {
              mem[p++] = idata[nx * (iy * 3 + 0) + ix];
              mem[p++] = idata[nx * (iy * 3 + 1) + ix];
              mem[p++] = idata[nx * (iy * 3 + 2) + ix];
            } else if ( trueColorOrder == 3 ) {
              mem[p++] = idata[nx * (0 * ny + iy) + ix];
              mem[p++] = idata[nx * (1 * ny + iy) + ix];
              mem[p++] = idata[nx * (2 * ny + iy) + ix];
            }
          } else {
            if ( chan == 1 ) {
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
              p += 2;
            } else if ( chan == 2 ) {
              p ++;
              mem[p++] = idata[1 * (iy * nx + ix) + 1];
              p ++;
            } else if ( chan == 3 ) {
              p += 2;
              mem[p++] = idata[1 * (iy * nx + ix) + 2];
            }
          }
        }
      }
      p = rowStart - (xsize*3);  
    }
  }
  m_dc->DrawBitmap(image,0,0);
  image.Destroy();
  temp_dc.SelectObject( wxNullBitmap);
  *m_bitmap = m_dc->GetAsBitmap(); 
  return true;
}
//bool GDLWXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
//        DLong trueColorOrder, DLong chan) {
// Version using wxAlphaPixelData, found starting version 3.0, for future Alpha Uses.
//  int wxOKforThis=wxMAJOR_VERSION*10000+wxMINOR_VERSION*100+wxRELEASE_NUMBER;
//  if (wxOKforThis<30001) return false; //wxWorks bug in earlier versions prevent the following code to work!
//
//  plstream::cmd( PLESC_FLUSH, NULL );
//  wxMemoryDC temp_dc;
//  temp_dc.SelectObject(*m_bitmap);
//  wxAlphaPixelData data( *m_bitmap );
//  if ( !data ) {
//    cerr << " ... raw access to bitmap data unavailable, do something else ..." << endl;
//    return false;
//  }
//
//  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
//  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);
//
//  PLINT kxLimit = m_bitmap->GetWidth() - xoff;
//  PLINT kyLimit = m_bitmap->GetHeight() - yoff;
//  if (nx < kxLimit) kxLimit = nx;
//  if (ny < kyLimit) kyLimit = ny;
//
//  if (nx > 0 && ny >0 ) {
//    wxAlphaPixelData::Iterator p( data );
//    p.OffsetY( data, m_bitmap->GetHeight( ) - yoff - 1 );
//    for ( int iy = 0; iy < kyLimit; ++iy ) {
//      wxAlphaPixelData::Iterator rowStart = p;
//      p.OffsetX( data, xoff );
//      for ( int ix = 0; ix < kxLimit; ++ix ) {
//        if ( trueColorOrder == 0 && chan == 0 ) {
//          p.Red( ) = pls->cmap0[idata[iy * nx + ix]].r;
//          p.Green( ) = pls->cmap0[idata[iy * nx + ix]].g;
//          p.Blue( ) = pls->cmap0[idata[iy * nx + ix]].b;
//        } else {
//          if ( chan == 0 ) {
//            if ( trueColorOrder == 1 ) {
//              p.Red( ) = idata[3 * (iy * nx + ix) + 0];
//              p.Green( ) = idata[3 * (iy * nx + ix) + 1];
//              p.Blue( ) = idata[3 * (iy * nx + ix) + 2];
//            } else if ( trueColorOrder == 2 ) {
//              p.Red( ) = idata[nx * (iy * 3 + 0) + ix];
//              p.Green( ) = idata[nx * (iy * 3 + 1) + ix];
//              p.Blue( ) = idata[nx * (iy * 3 + 2) + ix];
//            } else if ( trueColorOrder == 3 ) {
//              p.Red( ) = idata[nx * (0 * ny + iy) + ix];
//              p.Green( ) = idata[nx * (1 * ny + iy) + ix];
//              p.Blue( ) = idata[nx * (2 * ny + iy) + ix];
//            }
//          } else {
//            if ( chan == 1 ) {
//              p.Red( ) = idata[1 * (iy * nx + ix) + 0];
//            } else if ( chan == 2 ) {
//              p.Green( ) = idata[1 * (iy * nx + ix) + 1];
//            } else if ( chan == 3 ) {
//              p.Blue( ) = idata[1 * (iy * nx + ix) + 2];
//            }
//          }
//        }
//        p.Alpha( ) = 255;
//        p.OffsetX( data, 1 );
//      }
//      p = rowStart;
//      p.OffsetY( data, -1 );
//    }
//  }
//  m_dc->Blit(0, 0, m_width, m_height, &temp_dc, 0, 0);
//  temp_dc.SelectObject( wxNullBitmap);
//  *m_bitmap = m_dc->GetAsBitmap(); 
//  return true;
//}
bool GDLWXStream::SetGraphicsFunction( long value) {
  cerr<<"Set Graphics Function not ready for wxWindow draw panel, please contribute."<<endl;
 return true;
}

bool GDLWXStream::GetWindowPosition(long& xpos, long& ypos ) {
  cerr<<"Get Window Position not ready for wxWindow draw panel, please contribute."<<endl;
  xpos=0;
  ypos=0;
 return true;
}

bool GDLWXStream::GetScreenResolution(double& resx, double& resy) {
  wxScreenDC *temp_dc=new wxScreenDC();
  wxSize reso=temp_dc->GetPPI();
  resx = reso.x/2.54;
  resy = reso.y/2.54;
  return true;
}
bool GDLWXStream::CursorStandard(int cursorNumber)
{
 cerr<<"Cursor Setting not ready for wxWindow draw panel, please contribute."<<endl;
 return true;
}
DLong GDLWXStream::GetVisualDepth() {
return 24;
}
DString GDLWXStream::GetVisualName() {
static const char* visual="TrueColor";
return visual;
}
BaseGDL* GDLWXStream::GetImage( EnvT* e){

    plstream::cmd( PLESC_FLUSH, NULL );
    wxMemoryDC temp_dc;
    temp_dc.SelectObject(*m_bitmap);
    wxImage image=m_bitmap->ConvertToImage();
    unsigned char* mem=image.GetData();
    if ( mem == NULL ) e->Throw("No TV to read! (GDL internal error?)");
    if (e->KeywordSet("WORDS")) e->Throw( "WORDS keyword not yet supported.");
    DLong orderVal=SysVar::TV_ORDER();
    e->AssureLongScalarKWIfPresent( "ORDER", orderVal);
    
    SizeT dims[3],datadims[3];
    
    DByteGDL* res;

    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);
    if (tru > 3 || tru < 0) e->Throw("Value of TRUE keyword is out of allowed range.");

    DLong channel=-1;

    unsigned int xMaxSize = m_bitmap->GetWidth();
    unsigned int yMaxSize = m_bitmap->GetHeight();
    unsigned int x_gdl=0;
    unsigned int y_gdl=0;
    unsigned int nx_gdl=xMaxSize;
    unsigned int ny_gdl=yMaxSize;

    bool error=false;
    bool hasXsize=false;
    bool hasYsize=false;
    int nParam = e->NParam();
    if (nParam >= 4) {
      DLongGDL* Ny = e->GetParAs<DLongGDL>(3);
      ny_gdl=(*Ny)[0];
      hasYsize=true;
    }
    if (nParam >= 3) {
      DLongGDL* Nx = e->GetParAs<DLongGDL>(2);
      nx_gdl=(*Nx)[0];
      hasXsize=true;
    }
    if (nParam >= 2) {
      DLongGDL* y0 = e->GetParAs<DLongGDL>(1);
      y_gdl=(*y0)[0];
    }
    if (nParam >= 1) {
      DLongGDL* x0 = e->GetParAs<DLongGDL>(0);
      x_gdl=(*x0)[0];
    }
    if (nParam == 5) {
      DLongGDL* ChannelGdl = e->GetParAs<DLongGDL>(4);
      channel=(*ChannelGdl)[0]; 
    }
    e->AssureLongScalarKWIfPresent( "CHANNEL", channel);
    if (channel > 3) e->Throw("Value of Channel is out of allowed range.");

    if (!(hasXsize))nx_gdl-=x_gdl; 
    if (!(hasYsize))ny_gdl-=y_gdl;
    
    DLong xref,xval,xinc,yref,yval,yinc,xmax11,ymin11;
    int x_11=0;
    int y_11=0;
    xref=0;xval=0;xinc=1;
    yref=yMaxSize-1;yval=0;yinc=-1;
    
    x_11=xval+(x_gdl-xref)*xinc;
    y_11=yval+(y_gdl-yref)*yinc;
    xmax11=xval+(x_gdl+nx_gdl-1-xref)*xinc;    
    ymin11=yval+(y_gdl+ny_gdl-1-yref)*yinc;
    if (y_11 < 0 || y_11 > yMaxSize-1) error=true;
    if (x_11 < 0 || x_11 > xMaxSize-1) error=true;
    if (xmax11 < 0 || xmax11 > xMaxSize-1) error=true;
    if (ymin11 < 0 || ymin11 > yMaxSize-1) error=true;
    if (error) e->Throw("Value of Area is out of allowed range.");

    datadims[0] = m_bitmap->GetWidth();
    datadims[1] = m_bitmap->GetHeight();
    datadims[2] = 3;
    dimension datadim(datadims, (SizeT) 3);
    DByteGDL *bitmap = new DByteGDL( datadim, BaseGDL::ZERO);
    PLINT nx = m_bitmap->GetWidth();
    PLINT ny = m_bitmap->GetHeight();
    SizeT p = 0;
    for ( int iy = 0; iy < ny; ++iy ) {
      for ( int ix = 0; ix < nx; ++ix ) {
        (*bitmap)[3 * (iy * nx + ix) + 0] = mem[p++];
        (*bitmap)[3 * (iy * nx + ix) + 1] = mem[p++];
        (*bitmap)[3 * (iy * nx + ix) + 2] = mem[p++];
      }
    }
    image.Destroy();
#define PAD 3

  if ( tru == 0 ) {
    dims[0] = nx_gdl;
    dims[1] = ny_gdl;
    dimension dim( dims, (SizeT) 2 );
    res = new DByteGDL( dim, BaseGDL::ZERO );
    if ( channel <= 0 ) { //channel not given, return max of the 3 channels
      DByte mx, mx1;

      for ( SizeT i = 0; i < dims[0] * dims[1]; ++i ) {
        mx = (*bitmap)[PAD * i];
        mx1 = (*bitmap)[PAD * i + 1];
        if ( mx1 > mx ) mx = mx1;
        mx1 = (*bitmap)[PAD * i + 2];
        if ( mx1 > mx ) mx = mx1;
        (*res)[i] = mx;
      }
    } else {
      for ( SizeT i = 0; i < dims[0] * dims[1]; ++i ) {
        (*res)[i] = (*bitmap)[PAD * i + channel]; //0=R,1:G,2:B,3:Alpha
      }
    }
    GDLDelete(bitmap);
    // Reflect about y-axis
    if ( orderVal == 0 ) res->Reverse( 1 );
    return res;

  } else {
    dims[0] = 3;
    dims[1] = nx_gdl;
    dims[2] = ny_gdl;
    dimension dim( dims, (SizeT) 3 );
    res = new DByteGDL( dim, BaseGDL::NOZERO );
    for ( SizeT i = 0, kpad = 0; i < dims[1] * dims[2]; ++i ) {
      for ( SizeT j = 0; j < 3; ++j ) ( *res )[(i + 1)*3 - (j + 1)] = (*bitmap)[kpad++];
      kpad++;
    }
    GDLDelete(bitmap);
    // Reflect about y-axis
    if ( orderVal == 0 ) res->Reverse( 2 );

    DUInt* perm = new DUInt[3];
    if ( tru == 1 ) {
      return res;
    } else if ( tru == 2 ) {
      perm[0] = 1;
      perm[1] = 0;
      perm[2] = 2;
      return res->Transpose( perm );
    } else if ( tru == 3 ) {
      perm[0] = 1;
      perm[1] = 2;
      perm[2] = 0;
      return res->Transpose( perm );
    }
  }
  assert( false );
  return NULL;
#undef PAD 
}
#endif
