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



GDLWXStream::GDLWXStream( int width, int height )
: GDLGStream( width, height, "wxwidgets")
  , streamDC(NULL)
  , streamBitmap(NULL)
  , m_width(width), m_height(height)
  , container(NULL)
{
  streamDC = new wxMemoryDC();
  streamBitmap = new wxBitmap( width, height, 32);
  streamDC->SelectObject( *streamBitmap);
  if( !streamDC->IsOk())
  {
    streamDC->SelectObject( wxNullBitmap );
    delete streamBitmap;
    delete streamDC;
    throw GDLException("GDLWXStream: Failed to create DC.");
  }
  setopt("drvopt", "hrshsym=0,text=0" ); //no hershey; WE USE TT fonts (antialiasing very nice and readable. Moreover, big bug somewhere with hershey fonts).

  spage(0,0, width, height, 0, 0 ); //width and height have importance. dpi is best left to plplot, as it is always false but not correctable.
  
  //plplot switched from PLESC_DEVINIT to dev_data for wxwidgets around version 5.11
#define PLPLOT_TEST_VERSION_NUMBER PLPLOT_VERSION_MAJOR*1000+PLPLOT_VERSION_MINOR
#if (PLPLOT_TEST_VERSION_NUMBER > 5010)
  this->pls->dev_data=(void*)streamDC;
#endif
  init();
  plstream::cmd(PLESC_DEVINIT, (void*)streamDC );
    
   // no pause on win destruction
    spause( false);

    // extended fonts
    fontld( 1);

    // we want color
    scolor( 1);

    PLINT r[ctSize], g[ctSize], b[ctSize];
    GDLCT* myCT=GraphicsDevice::GetGUIDevice( )->GetCT();
    myCT->Get( r, g, b);
    SetColorMap0( r, g, b, ctSize); //set colormap 0 to 256 values

    // need to be called initially. permit to fix things
    ssub(1,1);
    adv(0);
    // load font
    font( 1);
    vpor(0,1,0,1);
    wind(0,1,0,1);
    DefaultCharSize();
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
  if( this->valid && container != NULL) {
    container->RepaintGraphics();
    //will be updated by eventloop.
//#ifdef __WXMAC__
//  wxTheApp->Yield();
//#else
//  wxGetApp().MainLoop(); //central loop for wxEvents!
//#endif
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
  streamBitmap = new wxBitmap( s.x, s.y, 32 );
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
      ::c_plclear();
      plstream::scolbg(red,green,blue); //resets col[0]
}

//FALSE: REPLACE With Clear(DLong chan) as in X //TBD
void GDLWXStream::Clear(DLong bColor) {
  Clear();
}

bool GDLWXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
        DLong trueColorOrder, DLong chan) {
//  plstream::cmd( PLESC_FLUSH, NULL );
//  Update();
  DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
  wxMemoryDC temp_dc;
  temp_dc.SelectObject(*streamBitmap);
  wxImage image=streamBitmap->ConvertToImage();
  unsigned char* mem=image.GetData();
  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);

  PLINT xsize = m_width;
  PLINT ysize = m_height;
  PLINT kxLimit = xsize - xoff;
  PLINT kyLimit = ysize - yoff;
  if (nx < kxLimit) kxLimit = nx;
  if (ny < kyLimit) kyLimit = ny;

  if (nx > 0 && ny > 0) {
    SizeT p = (ysize - yoff - 1)*3 * xsize;
    if (trueColorOrder == 0 && chan == 0) {
      if (decomposed == 1) {
        for (int iy = 0; iy < kyLimit; ++iy) {
          SizeT rowStart = p;
          p += xoff * 3;
          for (int ix = 0; ix < kxLimit; ++ix) {
            mem[p++] = idata[iy * nx + ix];
            mem[p++] = idata[iy * nx + ix];
            mem[p++] = idata[iy * nx + ix];
          }
          p = rowStart - (xsize * 3);
        }
      } else {
        for (int iy = 0; iy < kyLimit; ++iy) {
          SizeT rowStart = p;
          p += xoff * 3;
          for (int ix = 0; ix < kxLimit; ++ix) {
            mem[p++] = pls->cmap0[idata[iy * nx + ix]].r;
            mem[p++] = pls->cmap0[idata[iy * nx + ix]].g;
            mem[p++] = pls->cmap0[idata[iy * nx + ix]].b;
          }
          p = rowStart - (xsize * 3);
        }
      }
    } else {
      if (chan == 0) {
        if (trueColorOrder == 1) {
          for (int iy = 0; iy < kyLimit; ++iy) {
            SizeT rowStart = p;
            p += xoff * 3;
            for (int ix = 0; ix < kxLimit; ++ix) {
              mem[p++] = idata[3 * (iy * nx + ix) + 0];
              mem[p++] = idata[3 * (iy * nx + ix) + 1];
              mem[p++] = idata[3 * (iy * nx + ix) + 2];
            }
            p = rowStart - (xsize * 3);
          }
        } else if (trueColorOrder == 2) {
          for (int iy = 0; iy < kyLimit; ++iy) {
            SizeT rowStart = p;
            p += xoff * 3;
            for (int ix = 0; ix < kxLimit; ++ix) {
              mem[p++] = idata[nx * (iy * 3 + 0) + ix];
              mem[p++] = idata[nx * (iy * 3 + 1) + ix];
              mem[p++] = idata[nx * (iy * 3 + 2) + ix];
            }
            p = rowStart - (xsize * 3);
          }
        } else if (trueColorOrder == 3) {
          for (int iy = 0; iy < kyLimit; ++iy) {
            SizeT rowStart = p;
            p += xoff * 3;
            for (int ix = 0; ix < kxLimit; ++ix) {
              mem[p++] = idata[nx * (0 * ny + iy) + ix];
              mem[p++] = idata[nx * (1 * ny + iy) + ix];
              mem[p++] = idata[nx * (2 * ny + iy) + ix];
            }
            p = rowStart - (xsize * 3);
          }
        }
      } else { //1 byte bitmap passed.
        if (chan == 1) {
          for (int iy = 0; iy < kyLimit; ++iy) {
            SizeT rowStart = p;
            p += xoff * 3;
            for (int ix = 0; ix < kxLimit; ++ix) {
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
              p += 2;
            }
            p = rowStart - (xsize * 3);
          }
        } else if (chan == 2) {
          for (int iy = 0; iy < kyLimit; ++iy) {
            SizeT rowStart = p;
            p += xoff * 3;
            for (int ix = 0; ix < kxLimit; ++ix) {
              p++;
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
              p++;
            }
            p = rowStart - (xsize * 3);
          }
        } else if (chan == 3) {
          for (int iy = 0; iy < kyLimit; ++iy) {
            SizeT rowStart = p;
            p += xoff * 3;
            for (int ix = 0; ix < kxLimit; ++ix) {
              p += 2;
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
            }
            p = rowStart - (xsize * 3);
          }
        }
      }
    }
  }
  streamDC->DrawBitmap(image,0,0);
  image.Destroy();
  temp_dc.SelectObject( wxNullBitmap);
  *streamBitmap = streamDC->GetAsBitmap();
  Update();  //very much necessary for wxWidgets!
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
      streamDC->SetLogicalFunction( wxXOR);
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
  resx = reso.x/2.54;
  resy = reso.y/2.54;
  return true;
}
void GDLWXStream::DefineSomeWxCursors(){

#include "otherdevices/gdlcursors.h"
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
BaseGDL* GDLWXStream::GetFontnames(DString pattern) {
  if (pattern.length()<=0) return NULL;
  wxFontEnumerator fontEnumerator;
  fontEnumerator.EnumerateFacenames();
  int nFacenames = fontEnumerator.GetFacenames().GetCount();
  // we are supposed to select only entries lexically corresponding to 'pattern'.
  //first check who passes (ugly)
  wxString wxPattern(pattern);
  wxPattern=wxPattern.Upper();
  std::vector<int> good;
  for (int i=0; i< nFacenames; ++i) if (fontEnumerator.GetFacenames().Item(i).Upper().Matches(wxPattern)) { good.push_back(i); }
  if (good.size() == 0) return NULL;
  //then get them
  DStringGDL* myList=new DStringGDL(dimension(good.size()));
  for (int i=0; i< good.size(); ++i) (*myList)[i].assign(fontEnumerator.GetFacenames().Item(good[i]).mb_str(wxConvUTF8));
  return myList;
}
DLong GDLWXStream::GetFontnum(DString pattern){
  if (this->GetFontnames(pattern) == NULL) return 0;
  if (pattern.length()==0) return 0;
  return this->GetFontnames(pattern)->N_Elements();
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

DByteGDL* GDLWXStream::GetBitmapData() {
    wxImage image=streamBitmap->ConvertToImage();
    unsigned char* mem=image.GetData();
    if ( mem == NULL ) return NULL;    

    unsigned int nx = streamBitmap->GetWidth();
    unsigned int ny = streamBitmap->GetHeight();

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
