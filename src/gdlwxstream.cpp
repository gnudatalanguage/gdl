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
#include <wx/graphics.h>


GDLWXStream::GDLWXStream( int width, int height )
: GDLGStream( width, height,"wxwidgets")
  , streamDC(NULL)
  , streamBitmap(NULL)
  , m_width(width), m_height(height)
  , container(NULL)
{
  streamDC = new wxMemoryDC();
  streamBitmap = new wxBitmap( width, height, 24 );
  streamDC->SelectObject( *streamBitmap);
  if( !streamDC->IsOk())
  {
    streamDC->SelectObject( wxNullBitmap );
    delete streamBitmap;
    delete streamDC;
    throw GDLException("GDLWXStream: Failed to create DC.");
  }

  spage(0,0, width, height, 0, 0 ); //width and height have importance. dpi is best left to plplot.

//select the fonts in all cases...
// If wxwidgets have freetype (determined by PL_HAVE_FREETYPE, see src/plplot/modules/freetype, driver will have freetype enabled.
  std::string what = "hrshsym=0,text=1"; //no smooth available
  setopt("drvopt", what.c_str());

//init the driver...  
//plplot switched from PLESC_DEVINIT to dev_data for wxwidgets around version 5.11
//#define PLPLOT_TEST_VERSION_NUMBER PLPLOT_VERSION_MAJOR*1000+PLPLOT_VERSION_MINOR
//#if (PLPLOT_TEST_VERSION_NUMBER > 5010)
  this->pls->dev_data=(void*)streamDC;
//#endif
  init();
  
  // in our copy of the stae of plplot trimmed for our useage, we use their old but fast driver.
  // we can then set the font to hershey or freetype. (the plplot new driver was buggy with hershey anyway)
  PLINT doFont = ((PLINT) SysVar::GetPFont()>-1) ? 1 : 0;
  pls->dev_text=doFont;
  
  
  plstream::cmd(PLESC_DEVINIT, (void*)streamDC );
    
   // no pause on win destruction
    plstream::spause( false);

    // extended fonts
    plstream::fontld( 1);

    // we want color
    plstream::scolor( 1);

    PLINT r[ctSize], g[ctSize], b[ctSize];
    GDLCT* myCT=GraphicsDevice::GetGUIDevice( )->GetCT();
    myCT->Get( r, g, b);
    SetColorMap0( r, g, b, ctSize); //set colormap 0 to 256 values
    SetColorMap1( r, g, b, ctSize); //set colormap 1 to 256 values

    // need to be called initially. permit to fix things
    plstream::ssub( 1, 1 ); // plstream below stays with ONLY ONE page
    plstream::adv(0); //-->this one is the 1st and only pladv
    // load font
    plstream::font( 1);
    plstream::vpor(0,1,0,1);
    plstream::wind(0,1,0,1);

    ssub(1,1);
    SetPageDPMM();
    DefaultCharSize();
    adv(0); //this is for us (counters) //needs DefaultCharSize
    clear();
}

GDLWXStream::~GDLWXStream()
{
  //all WXStreams are in a wxWidget, either alone (plot window) or widget architecture (widget_draw)
  // destroying the stream must destroy the widget or the plot window.
  // we delete the bitmaps
  streamDC->SelectObject( wxNullBitmap );
  delete streamBitmap;
  delete streamDC;
  DestroyContainer();
}

void GDLWXStream::EventHandler() {
  if (!valid) return;
// GraphicsDevice::GetDevice()->TidyWindowsList(); //necessary since we removed TidyWindowList() from GraphicsMultiDevice::EventHandler()
  // plplot event handler
//  plstream::cmd(PLESC_EH, NULL);
}

void GDLWXStream::SetGdlxwGraphicsPanel(gdlwxGraphicsPanel* w, bool isPlot)
{
  container = w;
  isplot=isPlot;
}

void GDLWXStream::Update()
{
  if (this->valid && container != NULL) {
    container->Refresh();
    container->Update(); //solve 1643
    GDLWidget::CallWXEventLoop();
  }
}
////should be used when one does not recreate a wxstream each time size changes...
void GDLWXStream::SetSize( wxSize s )
{
  if ( s.x<1 || s.y <1) return;
  //Due to a 'bug' in PLPLOT, we need to resize streamDC by destroying it and getting a better one, instead of just using PLESC_RESIZE.
  streamDC->SelectObject( wxNullBitmap );
  delete streamBitmap;
  delete streamDC; //only solution to have it work with plplot-5.15 
  streamDC = new wxMemoryDC;
  container->SetStream(this); //act change of streamDC
  streamBitmap = new wxBitmap( s.x, s.y, 24 );
  streamDC->SelectObject( *streamBitmap );
  if( !streamDC->IsOk())
  {
    streamDC->SelectObject( wxNullBitmap );
    delete streamBitmap;
    delete streamDC;
    throw GDLException("GDLWXStream: Failed to resize DC.");
  }
  this->set_stream();
  this->cmd( PLESC_DEVINIT, (void *) streamDC );
  this->cmd(PLESC_RESIZE, (void*)&s );
  m_width = s.x;
  m_height = s.y;
  Update();
  //this because we use the old widget driver, which is fast but insane. We should not have such problems with the new driver, except that it is so slow..
    SetPageDPMM();
    DefaultCharSize();
}

void GDLWXStream::WarpPointer(DLong x, DLong y) {
  if (container) {
    int xx=x;
    int yy=y;
    wxPanel *p = static_cast<wxPanel*>(container);
    p->WarpPointer(xx,container->GetSize().y-yy);
  }
}

void GDLWXStream::Init()
{
  this->plstream::init();

//  set_stream(); // private
// test :  gdlFrame->Show();
}


void GDLWXStream::RenewPlot()
{
  plstream::cmd( PLESC_CLEAR, NULL );
  replot();
}

void GDLWXStream::GetGeometry( long& xSize, long& ySize)
{
  // plplot does not return the real size
  xSize = m_width;
  ySize = m_height;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLWXStream::GetGeometry(%ld %ld)\n", xSize, ySize);
}

unsigned long GDLWXStream::GetWindowDepth() {
  return wxDisplayDepth();
}

// GD removed use of (wrong) plplot erase function
void GDLWXStream::Clear() {
  DByte r = (GraphicsDevice::GetDevice()->BackgroundR());
  DByte g = (GraphicsDevice::GetDevice()->BackgroundG());
  DByte b = (GraphicsDevice::GetDevice()->BackgroundB());
  streamDC->SetBackground(wxBrush(wxColour(r,g,b)));
  streamDC->Clear();
  Update(); //see #1509
}

void GDLWXStream::Clear(DLong chan) {
  DByte r = (GraphicsDevice::GetDevice()->BackgroundR());
  DByte g = (GraphicsDevice::GetDevice()->BackgroundG());
  DByte b = (GraphicsDevice::GetDevice()->BackgroundB());
  wxBitmap bmp=streamDC->GetAsBitmap();
  wxImage image=bmp.ConvertToImage();
  unsigned char* pixels=image.GetData();
  SizeT size=image.GetHeight()*image.GetWidth();
  DByte* colorComponent=&r;
    switch(chan){
      case 0: //R
        colorComponent=&r;
        break;
      case 1:
        colorComponent=&g;
        break;
      case 2:
        colorComponent=&b;
        break;
    }
  for (auto i=0; i<size; ++i) pixels[3*i+chan]=*colorComponent; 
  streamDC->DrawBitmap(wxBitmap(image,3),0,0);
  Update(); //see #1509
}

#include <wx/rawbmp.h>
bool GDLWXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
        DLong trueColorOrder, DLong chan) {
  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);

  DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();

  wxBitmap bmp=wxBitmap(nx,ny,24);
  //to work on a plane we need to have as start the copy of screen pixels
  wxMemoryDC temp_dc2;
  temp_dc2.SelectObject(bmp);
  temp_dc2.Blit(0, 0, nx, ny, streamDC, xoff, m_height - yoff - ny);
  temp_dc2.SelectObject(wxNullBitmap);
  
  wxNativePixelData data(bmp);
if ( !data )
{
    // ... raw access to bitmap data unavailable, do something else ...
    return false;
  }

  
wxNativePixelData::Iterator p(data);

  if (trueColorOrder == 0 && chan == 0) {
    if (decomposed == 1) {
      for (int y = 0; y < ny; ++y) {
        int j0 = (ny - 1 - y) * nx;
        wxNativePixelData::Iterator rowStart = p;
        for (int x = 0; x < nx; ++x, ++p) {
          p.Red() = p.Green() = p.Blue() = idata[j0 + x ];
        }
        p = rowStart;
        p.OffsetY(data, 1);
      }
    } else {
      for (int y = 0; y < ny; ++y) {
        int j0 = (ny - 1 - y) * nx;
        wxNativePixelData::Iterator rowStart = p;
        for (int x = 0; x < nx; ++x, ++p) {
          p.Red() = pls->cmap0[idata[j0 + x ]].r;
          p.Green() = pls->cmap0[idata[j0 + x ]].g;
          p.Blue() = pls->cmap0[idata[j0 + x ]].b;
        }
        p = rowStart;
        p.OffsetY(data, 1);
      }
    }
  } else {
    if (chan == 0) {
      if (trueColorOrder == 1) {
        for (int y = 0; y < ny; ++y) {
          int j0 =3 * (ny -1 -y) * nx;
          wxNativePixelData::Iterator rowStart = p;
          for (int x = 0; x < nx; ++x, ++p) {
            int j1=j0 + 3 * x;
            p.Red()   = idata[j1];
            p.Green() = idata[j1+ 1];
            p.Blue()  = idata[j1+ 2];
          }
          p = rowStart;
          p.OffsetY(data, 1);
        }
      } else if (trueColorOrder == 2) {
        for (int y = 0; y < ny; ++y) {
          int j0 = nx * (ny - 1 - y) * 3 ;
          wxNativePixelData::Iterator rowStart = p;
          for (int x = 0; x < nx; ++x, ++p) {
            int j1=j0 + x;
            p.Red() = idata[j1];
            p.Green() = idata[j1 + nx * 1];
            p.Blue() = idata[j1 + nx * 2];
          }
          p = rowStart;
          p.OffsetY(data, 1);
        }
      } else if (trueColorOrder == 3) {
          for (int y = 0; y < ny; ++y) {
              wxNativePixelData::Iterator rowStart = p;
            for (int x = 0; x < nx; ++x, ++p) {
              int j1 =  nx * (ny - 1 - y) + x;
              p.Red() = idata[ j1];
              p.Green() = idata[nx * ny + j1];
              p.Blue() = idata[2 * nx * ny +j1];
            }
            p = rowStart;
            p.OffsetY(data, 1);
          }
      }
    } else { //1 byte bitmap passed.
      if (chan == 1) {
        for (int y = 0; y < ny; ++y) {
          int j0 = (ny - 1 - y) * nx;
          wxNativePixelData::Iterator rowStart = p;
          for (int x = 0; x < nx; ++x, ++p) {
            p.Red() = idata[j0 + x ];
          }
          p = rowStart;
          p.OffsetY(data, 1);
        }
      } else if (chan == 2) {
        for (int y = 0; y < ny; ++y) {
          int j0 = (ny - 1 - y) * nx;
          wxNativePixelData::Iterator rowStart = p;
          for (int x = 0; x < nx; ++x, ++p) {
            p.Green() = idata[j0 + x ];
          }
          p = rowStart;
          p.OffsetY(data, 1);
        }
      } else if (chan == 3) {
        for (int y = 0; y < ny; ++y) {
          int j0 = (ny - 1 - y) * nx;
          wxNativePixelData::Iterator rowStart = p;
          for (int x = 0; x < nx; ++x, ++p) {
            p.Blue() = idata[j0 + x ];
          }
          p = rowStart;
          p.OffsetY(data, 1);
        }
      }
    }
  }

  wxMemoryDC temp_dc;
  temp_dc.SelectObject(bmp);
  streamDC->Blit(xoff, m_height-yoff-ny, nx, ny, &temp_dc, 0, 0);
  temp_dc.SelectObject(wxNullBitmap);
  Update(); //see #1509
  return true;
}

bool GDLWXStream::SetGraphicsFunction( long value) {
  //use switch since passing an enum to a function is problematic for some compilers, grrrrrrrrrrrr!
  value=(value<0)?0:(value>15)?15:value;
  switch ( value ) {
    case 0: //wxCLEAR:
      streamDC->SetLogicalFunction( wxCLEAR);
      break;
    case 1: //wxAND:
      streamDC->SetLogicalFunction( wxAND);
      break;
    case 2: //wxAND_REVERSE:
      streamDC->SetLogicalFunction( wxAND_REVERSE);
      break;
    default:
    case 3: //wxCOPY:
      streamDC->SetLogicalFunction( wxCOPY);
      break;
    case 4: //wxAND_INVERT:
      streamDC->SetLogicalFunction( wxAND_INVERT);
      break;
    case 5: //wxNO_OP:
      streamDC->SetLogicalFunction( wxNO_OP);
      break;
    case 6: //wxXOR
      streamDC->SetLogicalFunction(  wxINVERT /*wxXOR*/); //at least permits box_cursor to be used in some cases. CAIRO has no 'old' graphic functions.
      break;    
    case 7: //wxOR:
      streamDC->SetLogicalFunction( wxOR);
      break;
    case 8: //wxNOR:
      streamDC->SetLogicalFunction( wxNOR);
      break;
    case 9: //wxEQUIV:
      streamDC->SetLogicalFunction( wxEQUIV);
      break;
    case 10: //wxINVERT:
      streamDC->SetLogicalFunction( wxINVERT);
      break;
    case 11: //wxOR_REVERSE:
      streamDC->SetLogicalFunction( wxOR_REVERSE);
      break;
    case 12: //wxSRC_INVERT:
      streamDC->SetLogicalFunction( wxSRC_INVERT);
      break;
    case 13: //wxOR_INVERT:
      streamDC->SetLogicalFunction( wxOR_INVERT);
      break;
    case 14: //wxNAND:
      streamDC->SetLogicalFunction( wxNAND);
      break;
    case 15: //wxSET:
      streamDC->SetLogicalFunction( wxSET);
      break;
  }
 return true;
}

bool GDLWXStream::GetWindowPosition(long& xpos, long& ypos ) {
  wxRect r=container->GetScreenRect();
  xpos=r.x;
  ypos=r.y;
 return true;
}

bool GDLWXStream::GetScreenResolution(double& resx, double& resy) {
  wxScreenDC *temp_dc=new wxScreenDC();
  wxSize reso=temp_dc->GetPPI();
  resx = reso.x/INCHToCM;
  resy = reso.y/INCHToCM;
  return true;
}
void GDLWXStream::DefineSomeWxCursors(){

#include "gdlcursors.h"
  for (int cnum=0; cnum<nglyphs; cnum++) {
  char* glyph=(char*)&(glyphs[glyphs_offset[cnum]]);
  char* glyph_mask=(char*)&(glyphs_mask[glyphs_mask_offset[cnum]]);
  int nx=glyphs_dims[cnum*2];
  int ny=glyphs_dims[cnum*2+1];
  int hotspot_x=glyphs_hotspot[cnum*2];
  int hotspot_y=glyphs_hotspot[cnum*2+1];
#ifdef __WXMSW__
    for(int i=0; i< (nx/8+1)*ny; ++i) {glyph[i]=~glyph[i]; glyph_mask[i]=~glyph_mask[i];}
    wxBitmap glyph_bitmap(glyph, nx,ny);
    wxBitmap glyph_mask_bitmap(glyph_mask, nx,ny);
    glyph_bitmap.SetMask(new wxMask(glyph_mask_bitmap));
    wxImage glyph_image = glyph_bitmap.ConvertToImage();
    glyph_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotspot_x);
    glyph_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotspot_y);
    wxCursor glyph_cursor = wxCursor(glyph_image);
    gdlwxCursors.push_back(glyph_cursor);
#elif defined(__WXGTK__) or defined(__WXMOTIF__)
    wxCursor glyph_cursor= wxCursor(glyph,nx,ny,hotspot_x,hotspot_y,glyph_mask); //, wxWHITE, wxBLACK);
    gdlwxCursors.push_back(glyph_cursor);
#endif
  }
}
bool GDLWXStream::CursorStandard(int cursorNumber)
{
  if (cursorNumber == -1) { //device,/CURSOR_ORIGINAL
    container->SetCursor(wxNullCursor); //back to default
    return true;
  }
  if (cursorNumber == -2) { //device,/CURSOR_CROSS
    container->SetCursor(wxCursor(wxCURSOR_CROSS));
    return true;
  }
  if (gdlwxCursors.size() < 1) DefineSomeWxCursors();
  int cnum=cursorNumber/2;
  if (cnum < 0) cnum=0;
  if (cnum > (gdlwxCursors.size()-1) ) cnum=gdlwxCursors.size()-1;
  container->SetCursor(gdlwxCursors[cnum]);
 return true;
}
bool GDLWXStream::CursorImage(char* v, int x, int y, char* m)
{
#ifdef __WXMSW__
  for(int i=0; i< 31; ++i) v[i]=~v[i];
    wxBitmap bitmap(v, 16, 16);
    if (m) {
     for(int i=0; i< 31; ++i) m[i]=~m[i];
      wxBitmap mask_bitmap(m, 16,16);
      bitmap.SetMask(new wxMask(mask_bitmap));
    }
    wxImage image = bitmap.ConvertToImage();
    image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, x);
    image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, y);
    wxCursor cursor = wxCursor(image);
    container->SetCursor(cursor);
#elif defined(__WXGTK__) or defined(__WXMOTIF__)
    wxCursor cursor= wxCursor(v,16,16,x,y,m); //, wxWHITE, wxBLACK);
   container->SetCursor(cursor);
#endif
 return true;
}
DLong GDLWXStream::GetVisualDepth() {
return 24;
}

void GDLWXStream::SetCurrentFont(std::string fontname){
  if (fontname.size() > 0) {
   wxFont font=wxFont(wxString(fontname.c_str( ), wxConvLibc));
   if (!font.IsSameAs(wxNullFont)) streamDC->SetFont(font);
  }
}
DString GDLWXStream::GetVisualName() {
static const char* visual="TrueColor";
return visual;
}

DByteGDL* GDLWXStream::GetBitmapData(int xoff, int yoff, int nx, int ny) {
//this function is unprotected from wron xoff,  yoff etc bad values.
  int orig_nx = streamBitmap->GetWidth();
  int orig_ny = streamBitmap->GetHeight();
  SizeT datadims[3];
  datadims[0] = nx;
  datadims[1] = ny;
  datadims[2] = 3;
  dimension datadim(datadims, (SizeT) 3);
  DByteGDL *bitmapgdl = new DByteGDL(datadim, BaseGDL::NOZERO);
  DByte* bmp = static_cast<DByte*> (bitmapgdl->DataAddr());
 
  wxRect sub=wxRect(xoff, orig_ny-ny-yoff, nx, ny);
  streamDC->SelectObject(wxNullBitmap);
  wxBitmap subb=streamBitmap->GetSubBitmap(sub);
  if (!subb.Ok()) throw GDLException("Value of Area is out of allowed range.");
  wxNativePixelData data(subb);
  if (!data) {
    streamDC->SelectObject(*streamBitmap);
    // ... raw access to bitmap data unavailable, do something else ...
    return NULL;
  }

  wxNativePixelData::Iterator p(data);

  for (int y = 0; y < ny; ++y) {
    auto j0 = 3 * (ny - 1 - y) * nx;
    wxNativePixelData::Iterator rowStart = p;
    for (int x = j0; x < j0 + 3 * nx;) {
      bmp[x++ ] = p.Red();
      bmp[x++ ] = p.Green();
      bmp[x++ ] = p.Blue();
      p++;
    }
    p = rowStart;
    p.OffsetY(data, 1);
  }
    streamDC->SelectObject(*streamBitmap);
  return bitmapgdl;
}

void GDLWXStream::Raise() {
this->GetMyContainer()->RaisePanel();
}

void GDLWXStream::Lower() {
this->GetMyContainer()->LowerPanel();
}

void GDLWXStream::Iconic() {
 this->GetMyContainer()->IconicPanel();
}

void GDLWXStream::DeIconic() {
 this->GetMyContainer()->DeIconicPanel();
}

bool GDLWXStream::UnsetFocus(){  
 return false; //UnsetFocus is dangerous: it prevents using wxEvents correctly.
}

bool GDLWXStream::GetGin(PLGraphicsIn *gin, int mode) {
  if (container==NULL) return false;
  enum CursorOpt {
    NOWAIT = 0,
    WAIT, //1
    CHANGE, //2
    DOWN, //3
    UP //4
  };
  Update();
  wxMouseState mouse=wxGetMouseState();
  wxPoint mousePoint = wxGetMousePosition ();
  unsigned int state=0, ostate=0; //start with null state
  unsigned int button=0; //gets the button state 1,2,3
  int x,y;
  x = 0;
  y = 0;
   if (container->GetScreenRect().Contains(wxGetMousePosition())) { //if cursor is in the window...
     mouse=wxGetMouseState();
     x = mouse.GetX();
     y = mouse.GetY();
   }

  //state is like the button state value, combination of all buttonstate masks of X11. It is merely to know what buttons are down
  if (mouse.LeftIsDown())  {button = 1; ostate |= 1;}
  if (mouse.MiddleIsDown()){button = 2; ostate |= 2;}
  if (mouse.RightIsDown()) {button = 3; ostate |= 4;}
#if wxCHECK_VERSION(3,0,0)
  if (mouse.Aux1IsDown())  {button = 4; ostate |= 8;}
  if (mouse.Aux2IsDown())  {button = 5; ostate |= 16;}  
#endif
  //return if NOWAIT or WAIT and Button pressed
  if ((mode == NOWAIT) || (mode == WAIT && button > 0)) {
    state=ostate;
    goto end; //else wait below for a down...
  }
  while (1) { //poll repeatedly -- waiting for event would be better but needs start locally the wx eventloop.
     if (container->GetScreenRect().Contains(wxGetMousePosition())) { //if cursor is in the window...
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
       if ( ( (x-gin->pX)*(x-gin->pX)+(y-gin->pY)*(y-gin->pY) > 0 ) && mode==CHANGE) {
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
  container->ScreenToClient(&x,&y); //now in coordinates
  gin->pX = x;
  gin->pY = container->GetSize().y - y;
  gin->dX = (PLFLT) gin->pX / ( container->GetSize().x - 1);
  gin->dY = (PLFLT) gin->pY / ( container->GetSize().y - 1);
  gin->string[0] = '\0';
  gin->keysym = 0x20;
  gin->button = button;
  return true;
}

#endif
