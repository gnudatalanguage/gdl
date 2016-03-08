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
#if ( (PLPLOT_VERSION_MAJOR < 6) && (PLPLOT_VERSION_MINOR < 10) )
  if ( GetEnvString("GDL_WX_BACKEND") == "2" )  SETOPT("drvopt", "hrshsym=1,backend=2,text=0" );
  else if ( GetEnvString("GDL_WX_BACKEND") == "1") SETOPT("drvopt", "hrshsym=1,backend=1,text=0" ); 
  else  SETOPT("drvopt", "hrshsym=1,backend=0,text=0" ); // do not use freetype. Backend=0 enable compatibility (sort of) with X11 behaviour in plots. To be augmented one day...
#else
  else  SETOPT("drvopt", "hrshsym=1,text=0" ); //
#endif
//  spage( 0.0, 0.0, 0, 0, 0, 0 ); //width and height have no importance, they are recomputed inside driver anyway!
//        PLFLT pageRatio=width/height;
//        std::string as = i2s( pageRatio);
//        SETOPT( "a", as.c_str());
  this->plstream::init();
  plstream::cmd(PLESC_DEVINIT, (void*)m_dc );
//  bool fixed=true;  //only for plplot_version_major > 10
//  plstream::cmd(PLESC_FIXASPECT, (void*)&fixed );
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

void GDLWXStream::SetSize( int width, int height )
{
  if ( width<1 || height <1) return;
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
//  wxSize screenPPM = m_dc->GetPPI(); //integer. Loss of precision if converting to PPM using wxSize operators.
  wxSize size = wxSize( width, height);

//  PLFLT def,cur,ofact,fact;
//  plgchr(&def,&cur); //cerr<<"before: "<<def;
//  ofact = 80.0/pls->ydpi;
////  cerr<<", ofact= "<<ofact; 

  plstream::cmd(PLESC_RESIZE, (void*)&size );
  m_width = width;
  m_height = height;
//  plgchr(&def,&cur); // cerr<<" ,after= "<<def;
//  fact = 80.0/pls->ydpi;
////  cerr<<", fact= "<<fact;
//  def *= fact/ofact;
////  cerr<<", new: " <<def<<endl;
//////  cerr << xp << ", " << yp << ", " << xleng * xp << ", " << yleng * yp << ", " << xoff << ", " << yoff << "," << &event << endl;
//  this->RenewPlplotDefaultCharsize( def );
}

void GDLWXStream::WarpPointer(DLong x, DLong y) {
  int xx=x;
  int yy=y;
  wxPanel *p = static_cast<wxPanel*>(gdlWindow);
  p->WarpPointer(xx,gdlWindow->GetSize().y-yy);
}

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
  return wxDisplayDepth();
}

void GDLWXStream::Clear() {
      PLINT red,green,blue;
      DByte r,g,b;
      PLINT red0,green0,blue0;
      
      GraphicsDevice::GetCT()->Get(0,r,g,b);red=r;green=g;blue=b;
      
      red0=GraphicsDevice::GetDevice()->BackgroundR();
      green0=GraphicsDevice::GetDevice()->BackgroundG();
      blue0=GraphicsDevice::GetDevice()->BackgroundB();
      plstream::scolbg(red0,green0,blue0); //overwrites col[0]
      ::c_plbop();
//      ::c_plclear();
      plstream::scolbg(red,green,blue); //resets col[0]
}

//FALSE: REPLACE With Clear(DLong chan) as in X //TBD
void GDLWXStream::Clear(DLong bColor) {
  Clear();
}

bool GDLWXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
        DLong trueColorOrder, DLong chan) {
//  plstream::cmd( PLESC_FLUSH, NULL );
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
          } else { //1 byte bitmap passed.
            if ( chan == 1 ) {
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
              p += 2;
            } else if ( chan == 2 ) {
              p ++;
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
              p ++;
            } else if ( chan == 3 ) {
              p += 2;
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
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
  Update();
  return true;
}

bool GDLWXStream::SetGraphicsFunction( long value) {
  //use switch since passing an enum to a function is problematic for some compilers, grrrrrrrrrrrr!
  value=(value<0)?0:(value>15)?15:value;
  switch ( value ) {
    case 0: //wxCLEAR:
      m_dc->SetLogicalFunction( wxCLEAR);
      break;
    case 1: //wxAND:
      m_dc->SetLogicalFunction( wxAND);
      break;
    case 2: //wxAND_REVERSE:
      m_dc->SetLogicalFunction( wxAND_REVERSE);
      break;
    default:
    case 3: //wxCOPY:
      m_dc->SetLogicalFunction( wxCOPY);
      break;
    case 4: //wxAND_INVERT:
      m_dc->SetLogicalFunction( wxAND_INVERT);
      break;
    case 5: //wxNO_OP:
      m_dc->SetLogicalFunction( wxNO_OP);
      break;
    case 6: //wxXOR:
      m_dc->SetLogicalFunction( wxXOR);
      break;
    case 7: //wxNOR:
      m_dc->SetLogicalFunction( wxNOR);
      break;
    case 8: //wxEQUIV:
      m_dc->SetLogicalFunction( wxEQUIV);
      break;
    case 9: //wxINVERT:
      m_dc->SetLogicalFunction( wxINVERT);
      break;
    case 10: //wxOR_REVERSE:
      m_dc->SetLogicalFunction( wxOR_REVERSE);
      break;
    case 11: //wxSRC_INVERT:
      m_dc->SetLogicalFunction( wxSRC_INVERT);
      break;
    case 12: //wxOR_INVERT:
      m_dc->SetLogicalFunction( wxOR_INVERT);
      break;
    case 13: //wxNAND:
      m_dc->SetLogicalFunction( wxNAND);
      break;
    case 14: //wxSET:
      m_dc->SetLogicalFunction( wxSET);
      break;
  }
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

DByteGDL* GDLWXStream::GetBitmapData() {
//    plstream::cmd( PLESC_FLUSH, NULL );
    wxMemoryDC temp_dc;
    temp_dc.SelectObject(*m_bitmap);
    wxImage image=m_bitmap->ConvertToImage();
    unsigned char* mem=image.GetData();
    if ( mem == NULL ) return NULL;    

    unsigned int nx = m_bitmap->GetWidth();
    unsigned int ny = m_bitmap->GetHeight();

    SizeT datadims[3];
    datadims[0] = nx;
    datadims[1] = ny;
    datadims[2] = 3;
    dimension datadim(datadims, (SizeT) 3);
    DByteGDL *bitmap = new DByteGDL( datadim, BaseGDL::NOZERO);
    //PADDING is 3BPP -- we revert Y to respect IDL default
    SizeT kpad = 0;
    for ( SizeT iy =0; iy < ny ; ++iy ) {
      for ( SizeT ix = 0; ix < nx; ++ix ) {
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 0] =  mem[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 1] =  mem[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 2] =  mem[kpad++];
      }
    }
    image.Destroy();
    return bitmap;
}

void GDLWXStream::Raise() {
  wxTopLevelWindow *tl = static_cast<wxTopLevelWindow*>(gdlWindow->GetParent());
  if (tl) tl->Raise();
}

void GDLWXStream::Lower() {
  wxTopLevelWindow *tl = static_cast<wxTopLevelWindow*>(gdlWindow->GetParent());
  if (tl) tl->Lower();
}

void GDLWXStream::Iconic() {
  wxTopLevelWindow *tl = static_cast<wxTopLevelWindow*>(gdlWindow->GetParent());
  if (tl) tl->Iconize(true);
}

void GDLWXStream::DeIconic() {
  wxTopLevelWindow *tl = static_cast<wxTopLevelWindow*>(gdlWindow->GetParent());
  if (tl) tl->Iconize(false);
}
bool GDLWXStream::UnsetFocus(){  
  wxTopLevelWindow *tl = static_cast<wxTopLevelWindow*>(gdlWindow->GetParent());
  if (tl) tl->Disable();
    return true;}

bool GDLWXStream::GetGin(PLGraphicsIn *gin, int mode) {

  enum CursorOpt {
    NOWAIT = 0,
    WAIT, //1
    CHANGE, //2
    DOWN, //3
    UP //4
  };
  wxMouseState mouse=wxGetMouseState();
  wxPoint mousePoint = wxGetMousePosition ();
  unsigned int state=0, ostate=0; //start with null state
  unsigned int button=0; //gets the button state 1,2,3
  int x,y;
  x = mouse.GetX();
  y = mouse.GetY();
  gin->pX = x;
  gin->pY = y;

  //state is like the button state value, combination of all buttonstate masks of X11. It is merely to know what buttons are down
  if (mouse.LeftIsDown())  {button = 1; ostate |= 1;}
  if (mouse.MiddleIsDown()){button = 2; ostate |= 2;}
  if (mouse.RightIsDown()) {button = 3; ostate |= 4;}
#if wxCHECK_VERSION(3,0,0)
  if (mouse.Aux1IsDown())  {button = 4; ostate |= 8;}
  if (mouse.Aux2IsDown())  {button = 5; ostate |= 16;}  
#endif
  //return if NOWAIT or WAIT and Button pressed
  UnsetFocus();  // first try to get out of focus.
  if ((mode == NOWAIT) || (mode == WAIT && button > 0)) {
    state=ostate;
    goto end; //else wait below for a down...
  }
  while (1) { //poll repeatedly -- waiting for event would be better but needs start locally the wx eventloop.
     if (gdlWindow->GetScreenRect().Contains(wxGetMousePosition())) { //if cursor is in the window...
       mouse=wxGetMouseState();
       x = mouse.GetX();
       y = mouse.GetY();
       if (mouse.LeftIsDown())   {button = 1; state |= 1;}
       if (mouse.MiddleIsDown()) {button = 2; state |= 2;}
       if (mouse.RightIsDown())  {button = 3; state |= 4;}
#if wxCHECK_VERSION(3,0,0)
       if (mouse.Aux1IsDown())   {button = 4; state |= 8;}
       if (mouse.Aux2IsDown())   {button = 5; state |= 16;}
#endif
       int change=(state^ostate);
       //is it a button up or down?
       int diff=(state&change)-(ostate&change);
       bool ButtonRelease=( diff < 0 ); //from 1 to 0: negative
       if (change > 0) { //press or release
         gin->pX = x;
         gin->pY = y;
         if (mode==CHANGE) goto end;
         if (mode==UP && ButtonRelease) goto end;
         if (!ButtonRelease && (mode==WAIT || mode==DOWN) ) goto end;
       }
       if ( (pow((float)x-gin->pX,2)+pow((float)y-gin->pY,2) > 0) && mode==CHANGE) {
         gin->pX = x;
         gin->pY = y;
         goto end;
       }
       ostate=state; //update state
       state=0; 
       gin->pX = x; //update new position
       gin->pY = y;
     }
  //We must get out of this loop sometime!
       wxMilliSleep( 50 );      // Sleep a bit to prevent CPU overuse
        
        if ( sigControlC )
          return false;
  }
end:
  //convert screen to client coord
  gdlWindow->ScreenToClient(&x,&y); //now in coordinates
  gin->pX = x;
  gin->pY = gdlWindow->GetSize().y - y;
  gin->dX = (PLFLT) gin->pX / ( gdlWindow->GetSize().x - 1);
  gin->dY = (PLFLT) gin->pY / ( gdlWindow->GetSize().y - 1);
  gin->string[0] = '\0';
  gin->keysym = 0x20;
  gin->button = button;
  return true;
}

#endif
