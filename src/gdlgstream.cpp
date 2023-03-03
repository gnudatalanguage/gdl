/* *************************************************************************
                          gdlgstream.cpp  -  graphic stream
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include <iostream>

#include "graphicsdevice.hpp"
#include "gdlgstream.hpp"
#include "initsysvar.hpp"

using namespace std;

// bool GDLGStream::plstreamInitCalled = false;

// void PLPlotAbortHandler(const char *c)
// {
//   cout << "PLPlot abort handler: " << c << endl;
// }
// 
// int PLPlotExitHandler(const char *c)
// {
//   cout << "PLPlot exit handler: " << c << endl;
//   return 0;
// }
// 
// void GDLGStream::SetErrorHandlers()
// {
//   plsexit( PLPlotExitHandler);
//   plsabort( PLPlotAbortHandler);
// }

void GDLGStream::Thick(DFloat thick)
{
  //note that 'cmake' may not able to find correct value of HAVE_PLPLOT_WIDTH. Please report.
  // in the meantime, you may edit "config.h" by hand.
#ifdef HAVE_PLPLOT_WIDTH
    plstream::width(static_cast<PLFLT>(thick*thickFactor));
#else
    plstream::wid(static_cast<PLINT>(floor((thick*thickFactor)-0.5)));
#endif
}

#define BLACK 0
#define WHITE 16777215
void GDLGStream::Color( ULong color, DLong decomposed) {
    bool printer = (((*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0] & 512) == 512);
    bool bw = (((*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0] & 16) == 0); //in that case, 
    //plplot postscript driver uses gray levels instead of colorindex, and 1 is black, not 0 !!!
    if (decomposed == 0) {
      if (printer && (color & 0xFF) == 0) { color=(bw)?WHITE:BLACK; //note that if bw other colors will be a gray value 
        GDLGStream::SetColorMap1SingleColor(color);
      } else plstream::col0(color & 0xFF); //just set color index [0..255]. simple and fast.
    } else {
      if (printer && color == 0) color=(bw)?WHITE:BLACK;
      GDLGStream::SetColorMap1SingleColor(color);
    }
}
#undef BLACK

void GDLGStream::SetColorMap1SingleColor( ULong color)
{
    PLINT red[1],green[1],blue[1];
    red[0] = color & 0xFF;
    green[0] = (color >> 8)  & 0xFF;
    blue[0]=(color >> 16) & 0xFF;
    SetColorMap1(red, green, blue, 1);
    plstream::col1(0); 
}

void GDLGStream::SetColorMap1DefaultColors(PLINT ncolors, DLong decomposed)
{
  if (decomposed == 0) { //just copy Table0 to Table1 so that scale from 0 to 1 in table 1 goes through the whole table
    PLINT r[ctSize], g[ctSize], b[ctSize];
    GraphicsDevice::GetCT()->Get( r, g, b); 
    SetColorMap1(r, g, b, ctSize); 
  } else {
    PLFLT r[2], g[2], b[2], pos[2];
    r[0] = pos[0] = 0.0;
    r[1] = pos[1] = 1.0;
    g[0] = g[1] = 0.0;
    b[0] = b[1] = 0.0;
    SetColorMap1n(ncolors);
    SetColorMap1l(TRUE,2,pos,r, g, b, NULL); 
  }
}

void GDLGStream::SetColorMap1Table( PLINT tableSize, DLongGDL *colors,  DLong decomposed)
{ //cycle on passed colors to fill tableSize.
  DLong n=colors->N_Elements();
#ifdef _MSC_VER
  PLINT *r = (PLINT*)alloca(sizeof(PLINT)*tableSize);
  PLINT *g = (PLINT*)alloca(sizeof(PLINT)*tableSize);
  PLINT *b = (PLINT*)alloca(sizeof(PLINT)*tableSize);
#else
  PLINT r[tableSize], g[tableSize], b[tableSize];
#endif
  if (decomposed == 0) {
    PLINT red[ctSize], green[ctSize], blue[ctSize], col;
    GraphicsDevice::GetCT()->Get( red, green, blue);
    for (SizeT i=0; i< tableSize; ++i) {
      col = (*colors)[i%n]& 0xFF;
      r[i] = red[col];
      g[i] = green[col];
      b[i] = blue[col];
    }
  } else {
    PLINT col;
     for (SizeT i=0; i< tableSize; ++i) {
      col = (*colors)[i%n];
      r[i] =  col        & 0xFF;
      g[i] = (col >> 8)  & 0xFF;
      b[i] = (col >> 16) & 0xFF;   
     }
  }
  SetColorMap1(r, g, b, tableSize); 
}

DLong GDLGStream::ForceColorMap1Ramp(PLFLT minlight) {
  DLong old_decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
  if (old_decomposed == 0) { //just copy Table0 to Table1 so that scale from 0 to 1 in table 1 goes through the whole table
    PLINT r[ctSize], g[ctSize], b[ctSize];
    GraphicsDevice::GetCT()->Get(r, g, b);
    SetColorMap1(r, g, b, ctSize);
  } else {
  //force decomposed=false otherwise too difficult with silly plplot colortables.
  GraphicsDevice::GetDevice()->Decomposed(false);
  //fill table1 with grey ramp.
    PLFLT h[2], l[2], s[2], pos[2];
    bool rev[2];
    h[0] = h[1] = 0.0;
    l[0] = minlight;
    l[1] = 1;
    s[0] = s[1]= 0.0;
    rev[0] = rev[1] = false;
    pos[0] = 0; pos[1]=1;
    SetColorMap1n(256);
    SetColorMap1l(false,2,pos,h, l, s, rev); 
  }
  return old_decomposed;
}
#define WHITEB 255
void GDLGStream::Background( ULong color, DLong decomposed)
{
  if ((*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0] & 512 ) {  ;//printer like PostScript
      GraphicsDevice::GetDevice()->SetDeviceBckColor(WHITEB, WHITEB, WHITEB );
   return;
  }
  DByte r,g,b;
  PLINT red,green,blue;
  if (decomposed == 0) { //just an index
    GraphicsDevice::GetCT()->Get( color & 0xFF, r, g, b);
    red=r; green=g; blue=b;
  } else {
    red = color & 0xFF;
    green = (color >> 8)  & 0xFF;
    blue = (color >> 16) & 0xFF;
  }
  GraphicsDevice::GetDevice()->SetDeviceBckColor( red, green, blue);
}
void GDLGStream::DefaultBackground()
{
  if ((*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0] & 512 ) {  ;//printer like PostScript
    GraphicsDevice::GetDevice()->SetDeviceBckColor(WHITEB, WHITEB, WHITEB );
    return;
  }
  DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset 
  DLong background=(*static_cast<DLongGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("BACKGROUND"), 0)))[0];
  DByte r,g,b;
  PLINT red,green,blue;
  if (GraphicsDevice::GetDevice()->GetDecomposed() == 0) { //just an index
    GraphicsDevice::GetCT()->Get( background & 0xFF, r, g, b);
    red=r; green=g; blue=b;
  } else {
    red = background & 0xFF;
    green = (background >> 8)  & 0xFF;
    blue = (background >> 16) & 0xFF;
  }
  GraphicsDevice::GetDevice()->SetDeviceBckColor( red, green, blue);
}
#undef WHITEB

void GDLGStream::SetPageDPMM() {
  //This is supposed to be called each time DPI is changed, which happens only at creation of a new device.
  // contrary to the page size, that may change for intercative devices that can be resized.
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "SetPageDPMM()\n");
  PLINT level;
  plstream::glevel(level);
  if (level <= 1) return;
  if (thePage.nbPages == 0) return;
  plstream::ssub(1, 1);
  plstream::adv(0);
  PLFLT xdpi, ydpi;
  PLINT xleng, yleng, xoff, yoff;
  plstream::gpage( xdpi, ydpi, xleng, yleng, xoff, yoff); //so-called page parameters, the units vary pixels or mm (screen vs. printers)
//  std::cerr<<"xdpi "<<xdpi<<" ydpi "<<ydpi<<std::endl;
//  std::cerr<<"xleng "<<xleng<<" yleng "<<yleng<<std::endl;
  PLFLT charHeight=pls->chrht;
//  std::cerr<<"charHeight="<<charHeight<<std::endl;
  thePage.length = xleng;
  thePage.height = yleng;
  //get the mm values using gspa:
  PLFLT bxsize_mm, bysize_mm, offx_mm, offy_mm;
  PLFLT xmin, ymin, xmax, ymax;
  plstream::gspa(xmin, xmax, ymin, ymax); //subpage in mm
  bxsize_mm = xmax - xmin;
  bysize_mm = ymax - ymin;
//  std::cerr<<"reported page size inmm "<<bxsize_mm<<","<<bysize_mm<<std::endl;
  thePage.xsizemm = bxsize_mm;
  thePage.ysizemm = bysize_mm;
  //we need to rescale all plplot values such as X_PX_CM, X_CH_SIZE, X_SIZE combinations give the same results as combining plplot values.
  //the returned xdpi and chrht is not good. We need to adjust chrht
  offx_mm = xmin;
  offy_mm = ymin;
  // test if plplot is consistent:
  PLFLT theory = xoff / xdpi * 25.4;
  if (fabs(offx_mm - theory) > 1e-4) if (GDL_DEBUG_PLSTREAM)  fprintf(stderr, "plpot driver returns inconsistent x DPI values.\n");
  theory = yoff / ydpi * 25.4;
  if (fabs(offy_mm - theory) > 1e-4) if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "plpot driver returns inconsistent y DPI values.\n");
  theory = xleng / xdpi * 25.4;
  if (fabs(bxsize_mm - theory) > 1e-4) if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "plpot driver returns inconsistent x DPI values (2).\n");
  theory = yleng / ydpi * 25.4;
  if (fabs(bysize_mm - theory) > 1e-4) if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "plpot driver returns inconsistent y DPI values (2).\n");
  //we can derive the dpm in x and y which converts mm to device coords:
  thePage.xdpmm = xdpi/25.4; //abs(thePage.length / bxsize_mm);
  thePage.ydpmm = ydpi/25.4; //abs(thePage.height / bysize_mm);
}

bool GDLGStream::updatePageInfo() {
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "updatePageInfo():\n");
  if (thePage.nbPages == 0) {
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "            FAILED\n");
    return false;
  }
  long xsize, ysize;
  GetGeometry(xsize, ysize);
  if (thePage.length == xsize && thePage.height == ysize) return true;
  thePage.length = xsize;
  thePage.height = ysize;
  (*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("X_SIZE"), 0)))[0] = xsize;
  (*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("Y_SIZE"), 0)))[0] = ysize;
  (*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("X_VSIZE"), 0)))[0] = xsize;
  (*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("Y_VSIZE"), 0)))[0] = ysize;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "             %fx%f device units.\n", thePage.length, thePage.height);
  return true;
}

void GDLGStream::CurrentCharSize(PLFLT scale) {
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "CurrentCharsize()\n");
  if (gdlDefaultCharInitialized == 0) {
    if (updatePageInfo() == true) {
      GetPlplotDefaultCharSize();
    }
  }
  theCurrentChar.scale = scale;
  theCurrentChar.ndsx = scale * theDefaultChar.ndsx;
  theCurrentChar.ndsy = scale * theDefaultChar.ndsy;
  theCurrentChar.dsx = scale * theDefaultChar.dsx;
  theCurrentChar.dsy = scale * theDefaultChar.dsy;
  theCurrentChar.mmsx = scale * theDefaultChar.mmsx;
  theCurrentChar.mmsy = scale * theDefaultChar.mmsy;
  theCurrentChar.wsx = scale * theDefaultChar.wsx;
  theCurrentChar.wsy = scale * theDefaultChar.wsy;
  theCurrentChar.mmspacing = scale * theDefaultChar.mmspacing;
  theCurrentChar.nspacing = scale * theDefaultChar.nspacing;
  theCurrentChar.dspacing = scale * theDefaultChar.dspacing;
  theCurrentChar.wspacing = scale * theDefaultChar.wspacing;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "            sized by %f is %fx%f mm or %fx%f device or %fx%f world\n", scale, theCurrentChar.mmsx, theCurrentChar.mmsy, theCurrentChar.dsx, theCurrentChar.dsy, theCurrentChar.wsx, theCurrentChar.wsy);
}

void GDLGStream::UpdateCurrentCharWorldSize() {
  PLFLT x, y, dx, dy;
  DeviceToWorld(0, 0, x, y);
  DeviceToWorld(theDefaultChar.dsx, theDefaultChar.dsy, dx, dy);
  theDefaultChar.wsx = abs(dx - x);
  theDefaultChar.wsy = abs(dy - y);
  theCurrentChar.wsx = theCurrentChar.scale * theDefaultChar.wsx;
  theCurrentChar.wsy = theCurrentChar.scale * theDefaultChar.wsy;

  DeviceToWorld(0, theDefaultChar.dspacing, dx, dy);
  theDefaultChar.wspacing = abs(dy - y);

  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "UpdateCurrentCharWorldSize(%f,%f)\n",
    theCurrentChar.wsx, theCurrentChar.wsy);
}

void GDLGStream::updateBoxDeviceCoords() {
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "updateBoxDeviceCoords()\n");
  // world coordinates of current subpage boundaries and page boundaries
  NormedDeviceToWorld(0.0, 0.0, theBox.pageWorldCoordinates[0], theBox.pageWorldCoordinates[2]);
  NormedDeviceToWorld(1.0, 1.0, theBox.pageWorldCoordinates[1], theBox.pageWorldCoordinates[3]);
  NormToWorld(0.0, 0.0, theBox.subPageWorldCoordinates[0], theBox.subPageWorldCoordinates[2]);
  NormToWorld(1.0, 1.0, theBox.subPageWorldCoordinates[1], theBox.subPageWorldCoordinates[3]);
  NormToDevice(theBox.nx1, theBox.ny1, theBox.dx1, theBox.dy1);
  NormToDevice(theBox.nx2, theBox.ny2, theBox.dx2, theBox.dy2);
  theBox.dxsize=theBox.dx2-theBox.dx1;
  theBox.dysize=theBox.dy2-theBox.dy1;
}
void GDLGStream::syncPageInfo()
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "SyncPageInfo()\n");
  PLINT level;
  plstream::glevel(level);
  if (level > 1 && thePage.nbPages != 0) //we need to have a vpor defined, and a page!
  {
    thePage.subpage.dxoff = 0; //our subpages have 0 offset
    thePage.subpage.dyoff = 0;
    thePage.subpage.dxsize = thePage.length / thePage.nx;
    thePage.subpage.dysize = thePage.height / thePage.ny;
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "         subpage is %fx%f at [%f,%f] device units\n",
      thePage.subpage.dxsize, thePage.subpage.dysize, thePage.subpage.dxoff, thePage.subpage.dyoff);

  } else if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "       WARNING: not initalized\n");
}
void GDLGStream::DefaultCharSize() {
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLGStream::DefaultCharSize()\n");
  DStructGDL* d = SysVar::D();
  DStructDesc* s = d->Desc();
  int X_CH_SIZE = s->TagIndex("X_CH_SIZE");
  int Y_CH_SIZE = s->TagIndex("Y_CH_SIZE");
  DLong chx = (*static_cast<DLongGDL*> (d->GetTag(X_CH_SIZE, 0)))[0];
  DLong chy = (*static_cast<DLongGDL*> (d->GetTag(Y_CH_SIZE, 0)))[0];
  int FLAGS = s->TagIndex("FLAGS");
  DLong flags = (*static_cast<DLongGDL*> (d->GetTag(FLAGS, 0)))[0];
  if (flags & 0x1) {
    int X_PX_CM = s->TagIndex("X_PX_CM");
    int Y_PX_CM = s->TagIndex("Y_PX_CM");
    DFloat xpxcm = (*static_cast<DFloatGDL*> (d->GetTag(X_PX_CM, 0)))[0];
    DFloat ypxcm = (*static_cast<DFloatGDL*> (d->GetTag(Y_PX_CM, 0)))[0];
    setVariableCharacterSize(chx, 1.0, chy,xpxcm,ypxcm);
  } else {
    setFixedCharacterSize(chx, 1.0, chy);
  }
}
void GDLGStream::SetCharSize(DLong chx, DLong chy) {
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLGStream::SetCharSize()\n");
  DStructGDL* d = SysVar::D();
  DStructDesc* s = d->Desc();
  int FLAGS = s->TagIndex("FLAGS");
  DLong flags = (*static_cast<DLongGDL*> (d->GetTag(FLAGS, 0)))[0];
  if (flags & 0x1) {
    int X_PX_CM = s->TagIndex("X_PX_CM");
    int Y_PX_CM = s->TagIndex("Y_PX_CM");
    DFloat xpxcm = (*static_cast<DFloatGDL*> (d->GetTag(X_PX_CM, 0)))[0];
    DFloat ypxcm = (*static_cast<DFloatGDL*> (d->GetTag(Y_PX_CM, 0)))[0];
    setVariableCharacterSize(chx, 1.0, chy,xpxcm,ypxcm);
  } else {
    setFixedCharacterSize(chx, 1.0, chy);
  }
}
 
  void GDLGStream::GetPlplotDefaultCharSize()
  {
        
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GetPlPlotDefaultCharsize()\n");
    if (thePage.nbPages==0)   {return;}
    //dimensions in normalized, device and millimetres
    if (gdlDefaultCharInitialized==1) {if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"     Already initialized\n"); return;}
    theDefaultChar.scale=1.0;
    theDefaultChar.mmsx=pls->chrht; //millimeter
    theDefaultChar.mmsy=pls->chrht;
    theDefaultChar.ndsx=mm2ndx(theDefaultChar.mmsx); //normalized device
    theDefaultChar.ndsy=mm2ndy(theDefaultChar.mmsy);
    theDefaultChar.dsy=theDefaultChar.ndsy*thePage.height;
    theDefaultChar.dsx=theDefaultChar.ndsx*thePage.length;
    theDefaultChar.mmspacing=theLineSpacing_in_mm;
    theDefaultChar.nspacing=mm2ndy(theDefaultChar.mmspacing);
    theDefaultChar.dspacing=theDefaultChar.nspacing*thePage.height;
    theDefaultChar.wspacing=mm2wy(theDefaultChar.mmspacing);
    
    theDefaultChar.wsx=mm2wx(theDefaultChar.mmsx); //world
    theDefaultChar.wsy=mm2wy(theDefaultChar.mmsy);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f,%f (mm)\n",theDefaultChar.mmsx   ,theDefaultChar.mmsy ,theDefaultChar.mmspacing);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f,%f (norm)\n",theDefaultChar.ndsx ,theDefaultChar.ndsy ,theDefaultChar.nspacing);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f,%f (dev)\n",theDefaultChar.dsx   ,theDefaultChar.dsy  ,theDefaultChar.dspacing);
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"             %fx%f,%f (world)\n",theDefaultChar.wsx ,theDefaultChar.wsy  ,theDefaultChar.wspacing);
    gdlDefaultCharInitialized=1;
  }

void GDLGStream::NextPlot( bool erase )
{
  DLongGDL* pMulti = SysVar::GetPMulti();

  DLong nx = (*pMulti)[ 1];
  DLong ny = (*pMulti)[ 2];
  DLong nz = (*pMulti)[ 3];

  DLong dir = (*pMulti)[ 4];

  nx = (nx>0)?nx:1;
  ny = (ny>0)?ny:1;
  nz = (nz>0)?nz:1;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NextPlot(erase=%d)\n",erase);
  // set subpage numbers in X and Y
//  plstream::ssub( nx, ny ); // ssub does not change charsize it seems
  ssub( nx, ny, nz ); 
  DLong nsub=nx*ny*nz;
  DLong pMod = (*pMulti)[0] % (nsub);

//  if( (*pMulti)[0] <= 0 || (*pMulti)[0] == nx*ny) // clear and restart to first subpage
  if( pMod == 0 ) // clear and restart to first subpage
  {
    if( erase )
    {
      eop();           // overridden (for Z-buffer)
      //get background value (*not pen 0*, we try to avoid plplot's silly behaviour).
      //use it for bop(), then reset the pen 0 to correct value.

      PLINT red,green,blue;
      DByte r,g,b;
      PLINT red0,green0,blue0;
      
      GraphicsDevice::GetCT()->Get(0,r,g,b);red=r;green=g;blue=b;
      
      red0=GraphicsDevice::GetDevice()->BackgroundR();
      green0=GraphicsDevice::GetDevice()->BackgroundG();
      blue0=GraphicsDevice::GetDevice()->BackgroundB();
      plstream::scolbg(red0,green0,blue0); //overwrites col[0]
      plstream::bop(); // note: changes charsize
      plstream::scolbg(red,green,blue); //resets col[0]
    }

//    plstream::adv(1); //advance to first subpage
    adv(1); //advance to first subpage
    (*pMulti)[0] = nsub-1; //set PMULTI[0] to this page
  }
  else
  {
    if( dir == 0 )
    {
//      plstream::adv(nx*ny - pMod + 1);
      adv(nsub - pMod + 1);
    }
    else
    {
      int p = nsub - pMod;
      int pp = p*nx % (nx*ny) + p/ny + 1;
//      plstream::adv(pp);
      adv(pp);
    }
    if( erase )
    {
      --(*pMulti)[0];
    }
  }
  // restore charsize to default for newpage
  sizeChar(1.0);
}

void GDLGStream::NoSub()
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"NoSub()\n");
  ssub( 1, 1); // changes charsize ?
//plstream::adv( 0);
  adv( 0);
//  DefaultCharSize();
}


// default is a wrapper for gpage(). Is overriden by, e.g., X driver.
void GDLGStream::GetGeometry( long& xSize, long& ySize)
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLGStream::GetGeometry()\n");
  PLFLT xp; PLFLT yp; 
  PLINT xleng; PLINT yleng;
  PLINT plxoff; PLINT plyoff;
  plstream::gpage( xp, yp, xleng, yleng, plxoff, plyoff); //for X-Window, wrapper give sizes from X11, not plplot which seems bugged.
  // for PostScript, Page size is FIXED (720x540) and GDLPSStream::GetGeometry replies correctly
  
//since the page sizes for PS and EPS images are processed by GDL after plplot finishes 
//its work, gpage will not output correct sizes 
  DString name = (*static_cast<DStringGDL*>(
    SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("NAME"), 0)
  ))[0];
  if (name == "PS") { 
    xSize = (*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("X_SIZE"), 0)))[0];
    ySize = (*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("Y_SIZE"), 0)))[0];
  } else {
  xSize = xleng;
  ySize = yleng;
  }
  if (xSize<1.0||ySize<1) //plplot gives back crazy values! z-buffer for example!
  {
    PLFLT xmin,xmax,ymin,ymax;
    plstream::gspa(xmin,xmax,ymin,ymax); //subpage in mm
    xSize=min(1.0,xmax-xmin);
    ySize=min(1.0,ymax-ymin);
  }
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"    found (%ld %ld)\n", xSize, ySize);

}

// SA: embedded font attributes handling (IDL to plPlot syntax translation)
 std::string GDLGStream::TranslateFormatCodes(const char *in, double *stringLength=NULL) 
{
  bool debug = false;
  static char errmsg[] = "No such font:   ";
  static std::string begin_unicode="#[";
  static std::string end_unicode="]";
  static const size_t errmsglen = strlen(errmsg);
  static double fact[]={1.,0.9,0.666,0.5,0.45,0.33,0.2};
  double base=1.0;

  // TODO: 
  // - in IDL the D.FLAGS bit value ((!D.FLAGS AND 4096) EQ 0)
  //   is designed to indicate if the device does not support extended commands
  // - unicode substitution for non-unicode terminals results in plplot controll
  //   sequences being printed 
  // do something about handling of !C and !S and !R
  // - ... a look-up table instead of the long switch/case blocks ...
 
  size_t len = strlen(in);
  if (stringLength) *stringLength=0;
  // skip conversion if the string is empty
  if (len == 0) return "";


  int default_fnt = activeFontCodeNum;
  std::string out = std::string(internalFontCodes[default_fnt]);
//no, take current value, initialized to 3. was:  activeFontCodeNum = default_fnt; // (current font number from the above table)
  int curr_fnt = default_fnt; // (current font number from the above table)
  int next_fnt = default_fnt; // (next letter font - same as curr_fnt save for the case of !G, !W and !M commands)
  int curr_lev = 0; // (incremented with #u, decremented with #d)
  int curr_pos = 0; // (current position in string)
  int save_pos = 0; // (position in string used in !S/!R save/restore)

  for (size_t i = 0; i < len; i++) {
    if (in[i] == '!' && in[i + 1] != '!')
    {
      size_t j = 1; // number of characters analysed (after !)
      switch (in[i + 1])
      {
        case '1' : // 2-char codes begining with !1
        {
          switch (in[i + 2])
          {
            case '6' : // !16 : Cyrillic
              base=17./15.;//approx size of a hershey char with this font.
              j++; 
              out += internalFontCodes[activeFontCodeNum = curr_fnt = next_fnt = 10 - 48 + in[i + 2]];
              break;
            case '8' : // !18 : Triplex Italic
              base=16.5/15.;//approx size of a hershey char with this font.
              j++; 
              out += internalFontCodes[activeFontCodeNum = curr_fnt = next_fnt = 10 - 48 + in[i + 2]];
              break;
            case '0' : // !10 : Special characters
            case '1' : // !11 : Gothic English
            case '2' : // !12 : Simplex Script
            case '3' : // !13 : Complex Script
            case '4' : // !14 : Gothic Italian
            case '5' : // !15 : Gothic German
            case '7' : // !17 : Triplex Roman
            case '9' : // !19 : 
              base=1.0;
              j++; 
              out += internalFontCodes[activeFontCodeNum = curr_fnt = next_fnt = 10 - 48 + in[i + 2]];
              break;
            default : // illegal command / end of string
              errmsg[errmsglen - 2] = in[i + 1];
              errmsg[errmsglen - 1] = in[i + 2];
              j++;
              Warning(errmsg);
          }
          break;
        }
        case '2' : // 2-char codes begining with !2
        {
          switch (in[i + 2])
          {
            case '0' : // !20 : Miscellaneous
              base=17.5/15.;//approx size of a hershey char with this font.
              j++;
              out += internalFontCodes[activeFontCodeNum = curr_fnt = next_fnt = 20 - 48 + in[i + 2]];
              break;
            default : // illegal command / end of string
              errmsg[errmsglen - 2] = in[i + 1];
              errmsg[errmsglen - 1] = in[i + 2];
              j++;
              Warning(errmsg);
          }
          break;
        }

        case '7' : // complex greek
        case '8' : // complex italic
          base=16.5/15.; //approx size of a hershey char with these fonts.
          out += internalFontCodes[activeFontCodeNum = next_fnt = curr_fnt = in[i + 1] - 48];
          break;

        case '3' : // simplex roman
        case '4' : // greek script
        case '5' : // duplex roman
        case '6' : // complex roman
        case '9' : // Math/special characters
          base=1.0;
          out += internalFontCodes[activeFontCodeNum = next_fnt = curr_fnt = in[i + 1] - 48];
          break;

        case 'M' : case 'm' : // one Math/special character
          curr_fnt = 9;
          break;
        case 'G' : case 'g' : // one Gothic English character
          curr_fnt = 11;
          break;
        case 'W' : case 'w' : // one Simplex Script character
          curr_fnt = 12; 
          break;

        case 'X' : case 'x' : // reversion to entry font 
          out += internalFontCodes[curr_fnt = next_fnt = default_fnt];
          break;

        case 'S' : case 's' : // save position
          save_pos = curr_pos;
          break;

        case 'L' : case 'l' : // 2nd level subscript
          curr_lev--;
          out += "#d";
          // continues!
        case 'B' : case 'b' : case 'D' : case 'd' : // subscript
        case 'I' : case 'i' : // index
          curr_lev--;
          out += "#d";
          break;
        case 'R' : case 'r' : // restore position
          for (; save_pos < curr_pos; curr_pos--) out += "#b";
          // continues!
        //case 'A' : case 'a' : // shift above the division line
          //TODO: plplot seems not to support it ----> treated as upperscript for the moment
       // case 'B' : case 'b' : // shift below the division line
          //TODO: plplot seems not to support it ----> treated as subscript for the moment
        case 'N' : case 'n' : // back to normal size
          while (curr_lev != 0) 
          {
            if (curr_lev > 0) out += "#d", curr_lev--;
            else out += "#u", curr_lev++;
          }
          // assumed from examples in documentation
          //if (in[i + 1] == 'N' || in[i + 1] == 'n') out += internalFontCodes[curr_fnt = next_fnt = default_fnt];
          break;
        case 'A' : case 'a' : case 'U' : case 'u' : // superscript
        case 'E' : case 'e' : // exponent
          curr_lev++;
          out += "#u";
          break;
        case 'Z' : case 'z' : // unicode chars via "#[nnn]"
          // first, the only two examples from IDL doc:
          // - !Z(00B0) - degree symbol
          // - !Z(U+0F1) - Spanish "n" with tilde (as in El ni\~no)
          // searching for the left parenthesis
          if (in[i + 2] != '(') 
          {
            if (in[i + 2] == '!' && in[i + 3] == 'Z') break; // !Z!Z is valid
            if (in[i + 2] == '\0') break; // end of string after !Z is valid
            Warning("Error using Hershey characters: Parentheses required for !Z.");
            goto retrn;
          } 
          else 
          {
            // searching for the right parenthesis
            size_t right_par = i + 2;
            while (in[++right_par] != ')') if (right_par == len) 
            {
              Warning("Error using Hershey characters: Parentheses required for !Z.");
              goto retrn;
            }
            size_t chars = 0;
            while (j + i + 1 != right_par)
            {
              // tokenizing (plethora of other tokens is accepted by IDL!)
              while (++j + i + 1 != right_par && in[j + i + 1] != ',') chars++;
              if (in[j + i + 1 - chars] == 'U' && in[j + i + 2 - chars] == '+') chars-=2; // U+NNNN syntax
              if (chars > 4) 
              {
                Warning("Error using Hershey characters: !Z hexadecimal value too large.");
                goto retrn;
              }
              else if (chars > 0)
              {
                out += "#[0x";
                for (; chars > 0; chars--) out += in[j + i + 1 - chars];          
                out += "]";
                curr_pos++;
              }
            }
            j++; // right parenthesis
          }
          break;
        case '\0' : // end of string
        default : // unknown command
          j--;
          curr_pos++;
          out += "!";
          break;
      }
      i += j; 
    }
    else 
    {
      if (stringLength) *stringLength+=base*fact[curr_lev%7];
      curr_pos++;
      // handling IDL exclamation mark escape '!!'
      if (in[i] == '!') {
        i++;
        if (stringLength) *stringLength+=base*fact[curr_lev%7];
      }
      // handling plplot number sign escape '##'
      if 
      (
        curr_fnt !=  9 && 
        curr_fnt != 10 && 
        curr_fnt != 16 && 
        curr_fnt != 20 && 
        in[i] == '#'
      ) out += "##"; 
      else switch (curr_fnt)
      {
        case 9 : // math symbols
          switch (in[i])
          {
            case '!' : out += "#(2229)"; break; // vertical line
            case '%' : out += "#(218)";  break; // degree circle
            case 'X' : out += "#(227)";  break; // x (cross) sign
            case 'D' :                          // as below
            case 'd' : out += "#(2265)"; break; // partial deriv sign
            case 'r' : out += "#(2267)"; break; // square root sign bigger
            case 'R' : out += "#(2411)"; break; // square root sign biggest
            case 'S' : out += "#(2267)"; break; // square root sign 
            case 'I' : out += "#(2412)"; break; // big integral sign
            case 'i' : out += "#(2268)"; break; // small integral sign
            case '/' : out += "#(2237)"; break; // range (dot dash dot) sign
            case '=' : out += "#(2239)"; break; // non equal sign
            case '6' : out += "#(2261)"; break; // arrow right
            case '7' : out += "#(2262)"; break; // arrow up
            case '4' : out += "#(2263)"; break; // arrow left
            case '5' : out += "#(2264)"; break; // arrow down
            case 'f' : out += "#(2283)"; break; // female sex sign
            case 'm' : out += "#(2285)"; break; // male sex sign
            case '<' : out += "#(2407)"; break; // big curly brace left
            case '>' : out += "#(2408)"; break; // big curly brace right
            case '#' : out += "#(737)";  break; // two vertical lines
            case '$' : out += "#(766)";  break; // infty
            case '&' : out += "#(2276)"; break; // paragraph
            case 'P' : out += "#(2147)"; break; // phi
            case 'p' : 
              out += "#fsp"; 
              out += internalFontCodes[curr_fnt];    break; // p script
            case 'q' : 
              out += "#fsq"; 
              out += internalFontCodes[curr_fnt];    break; // q script
            case ':' : out += "#(2240)"; break; // equal by definition sign
            case '.' : out += "#(850)";  break; // filled dot
            case 'B' : out += "#(841)";  break; // empty square
            case 'F' : 
              out += "#fsF"; 
              out += internalFontCodes[curr_fnt];    break; // F script
            case 'J' : out += "#(2269)"; break; // closed path integral
            case 'O' : out += "#(2277)"; break; // 'upper' cross sign
            case 'o' : out += "#(2278)"; break; // double cross sign
            case 'j' : 
              out += "#fsj"; 
              out += internalFontCodes[curr_fnt];    break; // j italic
            case 's' : out += "#(687)";  break; // some greek zig-zag 
            case 't' : 
              out += "#fs#(634)";        break; // theta-like greek zig-zag 
            case 'A' : out += "#(2246)"; break; // similar / tilde
            case 'T' : out += "#(740)";  break; // three dots
            case 'U' : out += "#(741)";  break; // spades (card sign)
            case 'V' : out += "#(743)";  break; // diamonds (card sign)
            case 'W' : out += "#(745)";  break; // clover sign
            case 'u' : out += "#(742)";  break; // hearts (card sign)
            case 'v' : out += "#(744)";  break; // clubs (card sign)
            case 'w' : out += "#(746)";  break; // ?
            case '_' :                          // as below 
            case 127u : out += "#(830)"; break; // '---' sign
            case 'H' : out += "#(908)";  break; // ?
            case '@' : out += "#(2077)"; break; // ? kappa like
            case 'n' : out += "#(2281)"; break; // circle with dot inside
            case 'e' : out += "#(2260)"; break; // mirrored E
            case 'E' : out += "#(2279)"; break; // element of
            case 'G' : out += "#(2266)"; break; // nabla
            case 'l' : out += "#(2243)"; break; // less or equal
            case 'b' : out += "#(2244)"; break; // grater or equal
            case '?' : out += "#(2245)"; break; // proportional
            case '^' :                          // as below
            case '~' : out += "#(2247)"; break; // ^
            case '+' : out += "#(2233)"; break; // plus minus
            case '-' : out += "#(2234)"; break; // minus plus
            case '(' : out += "#(2403)"; break; // big bracket left
            case ')' : out += "#(2404)"; break; // big bracket right
            case '[' :                          // as below
            case '{' : out += "#(2405)"; break; // big rect. brace left
            case ']' :                          // as below
            case '}' : out += "#(2406)"; break; // big rect. brace right
            case 'h' : out += "#(909)";  break; // ? police badge-like
            case 'x' : out += "#(738)";  break; // perpendicular sign
            case 'a' : out += "#(739)";  break; // angle
            case 'c' : out += "#(823)";  break; // ?
            case '0' : out += "#(2256)"; break; // set theory C-like 
            case '1' : out += "#(2257)"; break; // set theory U-like
            case '2' : out += "#(2258)"; break; // set theory )-like
            case '3' : out += "#(2259)"; break; // set theory ^-like
            case 'N' : out += "#(2311)"; break; // double wave
            case '`' : out += "'";       break; // ` -> '
            // empty chars:
            case '8' : case '9' : case ';' : case 'K' : case 'L' : case 'M' : case 'Q' :
            case 'Y' : case 'Z' : case '\\' : case 'k' : case 'y' : case 'z' : case '|' :
            case 'g' : 
            // unsupported chars:
            case 'C' : // tick sign
              out += " "; break;
              break;
            default :
              if ((unsigned char)in[i] > 127) { out+=begin_unicode; out+=to_string((unsigned char)in[i]); out+=end_unicode; } else out.append(in, i, 1); //use unicode
              break;
          }
          break;
        case 20 : // misc symbols
          switch (in[i])
          {
            case 'b' : out += "#(851)";  break;  // filled square
            case ':' :                           // small filled triangle up
            case 'C' : out += "#(852)";  break;  // filled triangle up
            case 'D' : out += "#(854)";  break;  // filled triangle down
            case '/' : out += "#(2323)"; break;  // musical sharp sign
            case 'M' : out += "#(874)";  break;  // ?
            case 'K' : out += "#(870)";  break;  // palm sign
            case '^' :                           // as below
            case '~' : out += "#(834)";  break;  // upside down triangle
            case 'N' : out += "#(900)";  break;  // circle smallest
            case 'n' : out += "#(901)";  break;  // circle smaller
            case 'O' : out += "#(902)";  break;  // circle small
            case 'o' : out += "#(903)";  break;  // circle 
            case 'P' : out += "#(904)";  break;  // circle 
            case 'p' : out += "#(905)";  break;  // circle big
            case 'Q' : out += "#(906)";  break;  // circle bigger
            case 'q' : out += "#(907)";  break;  // circle biggest
            case '?' : out += "#(767)";  break;  // flash
            case '<' : out += "#(768)";  break;  // paragraph-like
            case 'A' : out += "#(754)";  break;  // upper semicircle filled
            case 'G' : out += "#(862)";  break;  // two hammers
            case 'E' : out += "#(856)";  break;  // star
            case 'e' : out += "#(857)";  break;  // flag
            case 'f' : out += "#(861)";  break;  // ?
            case 'g' : out += "#(863)";  break;  // tower / look-out
            case 'h' : out += "#(865)";  break;  // grave
            case 'L' : out += "#(872)";  break;  // deciduous tree
            case 'k' : out += "#(871)";  break;  // coniferous tree
            case '"' : out += "#(2409)"; break;  // big inverted-s-like shape
            case '$' : out += "#(2376)"; break;  // ?
            case '%' : out += "#(2382)"; break;  // ?
            case '`' : 
            case '\'' : out += "#(766)"; break;  // infty
            case '(' : out += "#(2374)"; break;  // natural (music)
            case ')' : out += "#(2375)"; break;  // flat (music)
            case '*' : out += "#(2372)"; break;  // ? minim (music)
            case '-' :                           // as below
            case '+' : out += "#(2371)"; break;  // ? 2xminim length note (music)
            case ',' : out += "#(2329)"; break;  // ? rest (music)
            case '.' : out += "#(2380)"; break;  // treble clef
            case '0' : out += "#(2306)"; break;  // ? Gothic-like m
            case '1' : out += "#(2307)"; break;  // ? underlined omega
            case '2' : out += "#(2308)"; break;  // ? Gothic-like m
            case '3' : out += "#(2309)"; break;  // NE double arrow
            case '4' : out += "#(2310)"; break;  // ?
            case '5' : out += "#(2311)"; break;  // ?
            case '6' : out += "#(2312)"; break;  // ?
            case '7' : out += "#(2317)"; break;  // ?
            case '8' : out += "#(2318)"; break;  // ?
            case '9' : out += "#(2319)"; break;  // ?
            case '=' : out += "#(2377)"; break;  // ?
            case '>' : out += "#(831)";  break;  // ?
            case 'V' : out += "#(2291)"; break;  // crescent
            case 'W' : out += "#(2293)"; break;  // 8-arm star
            case 'X' : out += "#(2295)"; break;  // ?
            case 'Y' : out += "#(2302)"; break;  // ?
            case 'Z' : out += "#(2304)"; break;  // 69 ;)
            case 'r' : out += "#(2282)"; break;  // ? female sex sign like
            case 'u' : out += "#(2290)"; break;  // ? P-like
            case 'v' : out += "#(2292)"; break;  // ?
            case 'w' : out += "#(2294)"; break;  // ? omega-like
            case 'x' : out += "#(2301)"; break;  // ? fountain-like
            case 'y' : out += "#(2303)"; break;  // ?
            case '[' :                           // as below
            case '{' : out += "#(2332)"; break;  // ? ladder-like
            case ']' :                           // as below
            case '}' : out += "#(2381)"; break;  // bass clef
            case '_' :                           // as below
            case 127 : out += "#(2410)"; break;  // big s-like shape
            case '!' : out += "#(764)";  break;  // S-like
            case '&' : out += "#(765)";  break;  // tilde-like half-infty sign
            case 'B' : out += "#(850)";  break;  // filled dot
            case 'F' : out += "#(860)";  break;  // anchor
            case 'I' : out += "#(866)";  break;  // cross
            case 'J' : out += "#(868)";  break;  // Jewish star
            case 'R' : out += "#(735)";  break;  // square-like
            case 'a' : out += "#(755)";  break;  // filled triangle 
            case 'i' : out += "#(867)";  break;  // small screscent
            case 'j' : out += "#(869)";  break;  // bell
            case ' ' : out += "#(2328)"; break; // ?
            case '#' : out += "#(2331)"; break; // ?
            case 'S' : out += "#(2284)"; break; // empty circel with plus sign inside
            case 'T' : out += "#(2287)"; break; // ?
            case 'U' : out += "#(2289)"; break; // ? greep psi-like
            case '\\' :  // as below
            case '|' : out += "#(833)";  break; // ? electrical ground sign-like
            case 'c' : out += "#(853)";  break; // filled triangle left
            case 'd' : out += "#(855)";  break; // filled triangle right
            case '@' : out += "#(832)";  break; // ^
            case 'H' : out += "#(864)";  break; // flower in a flowerpot
            case 's' : out += "#(2286)"; break; // 4-like
            case 't' : out += "#(2288)"; break; // ?
            case ';' : out += "#(840)";  break; // empty circle
            case 'l' : out += "#(873)";  break; // ?
            case 'z' : out += "#(2305)"; break; // ?
            case 'm' : out += "#(899)";  break; // smallest circle - dot
            default : 
              if ((unsigned char)in[i] > 127) { out+=begin_unicode; out+=to_string((unsigned char)in[i]); out+=end_unicode; } else out.append(in, i, 1); //use unicode
              break;
          }
          break;
        case 16 : // Cyrillic
          switch (in[i])
          { //         uppercase                           lowercase
            case 'A' : out += "#(2801)"; break; case 'a' : out += "#(2901)"; break; // [a]
            case 'B' : out += "#(2802)"; break; case 'b' : out += "#(2902)"; break; // [b]
            case 'C' : out += "#(2803)"; break; case 'c' : out += "#(2903)"; break; // [v]
            case 'D' : out += "#(2804)"; break; case 'd' : out += "#(2904)"; break; // [g]
            case 'E' : out += "#(2805)"; break; case 'e' : out += "#(2905)"; break; // [d]
            case 'F' : out += "#(2806)"; break; case 'f' : out += "#(2906)"; break; // [ye]
            case 'G' : out += "#(2807)"; break; case 'g' : out += "#(2907)"; break; // [zsh]
            case 'H' : out += "#(2808)"; break; case 'h' : out += "#(2908)"; break; // [z]
            case 'I' : out += "#(2809)"; break; case 'i' : out += "#(2909)"; break; // [i/e]
            case 'J' : out += "#(2810)"; break; case 'j' : out += "#(2910)"; break; // [ii]
            case 'K' : out += "#(2811)"; break; case 'k' : out += "#(2911)"; break; // [k]
            case 'L' : out += "#(2812)"; break; case 'l' : out += "#(2912)"; break; // [l]
            case 'M' : out += "#(2813)"; break; case 'm' : out += "#(2913)"; break; // [m]
            case 'N' : out += "#(2814)"; break; case 'n' : out += "#(2914)"; break; // [n]
            case 'O' : out += "#(2815)"; break; case 'o' : out += "#(2915)"; break; // [o]
            case 'P' : out += "#(2816)"; break; case 'p' : out += "#(2916)"; break; // [p]
            case 'Q' : out += "#(2817)"; break; case 'q' : out += "#(2917)"; break; // [r]
            case 'R' : out += "#(2818)"; break; case 'r' : out += "#(2918)"; break; // [s]
            case 'S' : out += "#(2819)"; break; case 's' : out += "#(2919)"; break; // [t]
            case 'T' : out += "#(2820)"; break; case 't' : out += "#(2920)"; break; // [u/woo]
            case 'U' : out += "#(2821)"; break; case 'u' : out += "#(2921)"; break; // [f]
            case 'V' : out += "#(2822)"; break; case 'v' : out += "#(2922)"; break; // [h]
            case 'W' : out += "#(2823)"; break; case 'w' : out += "#(2923)"; break; // [c]
            case 'X' : out += "#(2824)"; break; case 'x' : out += "#(2924)"; break; // [ch]
            case 'Y' : out += "#(2825)"; break; case 'y' : out += "#(2925)"; break; // [sh]
            case 'Z' : out += "#(2826)"; break; case 'z' : out += "#(2926)"; break; // [shch]
            case '#' : out += "#(2827)"; break; case '<' : out += "#(2927)"; break; // hard sign
            case '{' : case '[' : out += "#(2828)"; break; case '>' : out += "#(2928)"; break; // [y]
            case '}' : case ']' : out += "#(2829)"; break; case '@' : out += "#(2929)"; break; // soft sign
            case '%' : out += "#(2830)"; break; case '\\' : case '|' : out += "#(2930)"; break; // [eh]
            case '"' : out += "#(2831)"; break; case '^' : case '~' : out += "#(2931)"; break; // [yu]
            case 127u : case '_' : out += "#(2832)"; break; case ';' : out += "#(2932)"; break; // [ya]
            default : 
              if ((unsigned char)in[i] > 127) { out+=begin_unicode; out+=to_string((unsigned char)in[i]); out+=end_unicode; } else out.append(in, i, 1); //use unicode
              break;
          }
          break;
        case 4 : // greek letters
        case 7 :
          switch (in[i])
          {
            // non-equivalent letters
            case 'C' : out += "#gG"; break; case 'c' : out += "#gg"; break;
            case 'F' : out += "#gZ"; break; case 'f' : out += "#gz"; break;
            case 'G' : out += "#gY"; break; case 'g' : out += "#gy"; break;
            case 'J' : out += "#gK"; break; case 'j' : out += "#gk"; break;
            case 'K' : out += "#gL"; break; case 'k' : out += "#gl"; break;
            case 'L' : out += "#gM"; break; case 'l' : out += "#gm"; break;
            case 'M' : out += "#gN"; break; case 'm' : out += "#gn"; break;
            case 'N' : out += "#gC"; break; case 'n' : out += "#gc"; break;
            case 'Q' : out += "#gR"; break; case 'q' : out += "#gr"; break;
            case 'R' : out += "#gS"; break; case 'r' : out += "#gs"; break;
            case 'S' : out += "#gT"; break; case 's' : out += "#gt"; break;
            case 'T' : out += "#gU"; break; case 't' : out += "#gu"; break;
            case 'U' : out += "#gF"; break; case 'u' : out += "#gf"; break;
            case 'V' : out += "#gX"; break; case 'v' : out += "#gx"; break;
            case 'W' : out += "#gQ"; break; case 'w' : out += "#gq"; break;
            case 'X' : out += "#gW"; break; case 'x' : out += "#gw"; break;
            case 'Y' :                      case 'y' : out += "#(766)"; break;  
            case '{' : out += '['; break; case '}' : out += ']'; break;
            // equivalent letters
            case 'A' : case 'a' :
            case 'B' : case 'b' : 
            case 'D' : case 'd' :
            case 'E' : case 'e' :
            case 'H' : case 'h' :
            case 'I' : case 'i' :
            case 'O' : case 'o' :
            case 'P' : case 'p' :
              out += "#g"; out.append(in, i, 1); break;
            default : 
              if ((unsigned char)in[i] > 127) { out+=begin_unicode; out+=to_string((unsigned char)in[i]); out+=end_unicode; } else out.append(in, i, 1); //use unicode
              break;
          }
          break;
        case 3 : // simplex roman
          switch ((unsigned char)in[i])
          {
            // unsupported chars
            case '^' : case '~' : case '\\' :
              out += " "; 
              break;
            // 8th bit chars
            case 144u : out += "1"; break;
            case 154u : out += "#(218)"; break; // degree circle
            case 163u : out += "#(272)"; break; // pound sign
            case 167u : out += "#(2276)"; break; // paragraph
            case 169u : out += "#(274)"; break; // copyright sign
            case 174u : out += "#(273)"; break; // registered sign
            case 181u : out += "#gm"; break; // greek mu
            case 188u : out += "#(270)"; break; // 1/4
            case 189u : out += "#(261)"; break; // 1/2
            case 190u : out += "#(271)"; break; // 3/4
            case 215u : out += "#(846)"; break; // cross sign
            case 223u : out += "#fs#gb"; out += internalFontCodes[curr_fnt]; break; // beta script
            default :
              if ((unsigned char)in[i] > 127) { out+=begin_unicode; out+=to_string((unsigned char)in[i]); out+=end_unicode; } else out.append(in, i, 1); //use unicode
              break;
          }
          break;
        default : // simply pass the char
          if ((unsigned char)in[i] > 127) { out+=begin_unicode; out+=to_string((unsigned char)in[i]); out+=end_unicode; } else out.append(in, i, 1); //use unicode
          break;
      }
      curr_fnt = next_fnt;
    }
  }
  activeFontCodeNum = curr_fnt;
  //if gdlGetStringLength function is available, use it to give back a better value ("X" and "I" do not have the same width in hershey format!)
#if PLPLOT_PRIVATE_NOT_HIDDEN
  if (stringLength) *stringLength=gdlGetStringLength(out)/this->mmCharLength();
#endif
  return out;
retrn:
  activeFontCodeNum = curr_fnt;
  if (stringLength) *stringLength=0;
  cout << "ERROR: GDLGStream::TranslateFormatCodes(\"" << in << "\") = \"" << out << "\"" << endl;  
  return ""; 
}

void GDLGStream::setSymbolSize( PLFLT scale )
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"setSymbolScale(%f)\n",scale);
//  plstream::ssym(0.0, scale);
  theCurrentSymSize=scale;
}

void GDLGStream::setLineSpacing(PLFLT newSpacing)
{
  theLineSpacing_in_mm=newSpacing;
}
PLFLT GDLGStream::getSymbolSize(){return theCurrentSymSize;}
void GDLGStream::mtex( const char *side, PLFLT disp, PLFLT posit, PLFLT just,
                       const char *text)
{
   //plot does not handle !C
  size_t len = strlen(text);
  bool simple=true;
  for (size_t i = 0; i < len; i++) if (text[i] == '!' && text[i + 1] == 'C') {
    simple=false;
  }
  if (simple) {
    plstream::mtex(side,disp,posit,just,TranslateFormatCodes(text).c_str());
    return;
  }
  //complicated:
  double d=0;
  std::string s(text);
  std::string newline="!C";
  long pos = 0, oldpos=0;
  PLFLT ydisp=(1.0+nLineSpacing()/nCharHeight());
  std::vector<long> positions;
  while (pos != string::npos) {
    pos = s.find(newline, oldpos);
    while (text[pos-1] == '!') { pos = s.find(newline, pos+2); }
    positions.push_back(oldpos);
    positions.push_back(pos);
    oldpos=pos+2;
  }
  for (std::vector<long>::iterator it = positions.begin(); it != positions.end();) {
    oldpos=(*it++);
    pos=(*(it++));
    long l=pos-oldpos; 
    if (l<0) l=string::npos;
//    std::cerr<<pos<<":"<<l<<" "<<s.substr(oldpos,l)<<std::endl;
    plstream::mtex(side,disp,posit,just,TranslateFormatCodes(s.substr(oldpos,l).c_str()).c_str());
    disp+=ydisp;
  }
}

void GDLGStream::ptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just,
                       const char *text , double *stringCharLength)
{
  //plot does not handle !C
  size_t len = strlen(text);
  bool simple=true;
  for (size_t i = 0; i < len; i++) if (text[i] == '!' && text[i + 1] == 'C') {
    simple=false;
  }
  if (simple) {
    plstream::ptex(x,y,dx,dy,just,TranslateFormatCodes(text,stringCharLength).c_str());
    return;
  }
  //complicated:
  double d=0;
  std::string s(text);
  std::string newline="!C";
  long pos = 0, oldpos=0;
  PLFLT ydisp=(1.0+nLineSpacing()/nCharHeight())*wCharHeight();
  std::vector<long> positions;
  while (pos != string::npos) {
    pos = s.find(newline, oldpos);
    while (text[pos-1] == '!') { pos = s.find(newline, pos+2); }
    positions.push_back(oldpos);
    positions.push_back(pos);
    oldpos=pos+2;
  }
  for (std::vector<long>::iterator it = positions.begin(); it != positions.end();) {
    oldpos=(*it++);
    pos=(*(it++));
    long l=pos-oldpos; 
    if (l<0) l=string::npos;
//    std::cerr<<pos<<":"<<l<<" "<<s.substr(oldpos,l)<<std::endl;
    plstream::ptex(x,y,dx,dy,just,TranslateFormatCodes(s.substr(oldpos,l).c_str(),&d).c_str()) ;
    y-=ydisp;
    *stringCharLength=MAX(*stringCharLength,d);
  }
}

#define FUDGE_VARCHARSIZE sqrt(2)
//This defines the character size for SCALABLE character devices (POSTSCRIPT, SVG))
//The dimension of "average" character (given by X_CH_SIZE) is to be a physical (mm) size.
//this implies to have a correct value for DPI AND that the plplot driver is correctly written.
void GDLGStream::setVariableCharacterSize( PLFLT charwidthpixel, PLFLT scale , PLFLT lineSpacingpixel, PLFLT xpxcm, PLFLT ypxcm)
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"setVariableCharacterSize()\n");
  xpxcm/=FUDGE_VARCHARSIZE;
  ypxcm/=FUDGE_VARCHARSIZE;  //go figure why this is needed, but indeed it is needed!
  //tried by comparison of outputs of 
  // "set_plot,'ps' & !P.multi=[0,2,2]&a=dist(5)&for i=1,4 do begin&s=i*0.7& plot,a,psym=6,syms=s,chars=s,xtit="XXXX" $
  //  & xyouts,indgen(25),a,"M",ali=0.5,chars=s & end & !p.multi=0 & plots,[0.001,0.001,0.999,0.999,0.001],$
  //  [0.001,0.999,0.999,0.001,0.001],/norm & device,/close & set_plot,'x'

 if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"schr(width=%f, scale=%f, spacing=%f, xpxcm=%f, ypxcm=%f)\n",charwidthpixel,scale,lineSpacingpixel,xpxcm,ypxcm);
   PLFLT xdpi=xpxcm*INCHToCM;
   PLFLT ydpi=ypxcm*INCHToCM;
// GDL asks for pixels, plplot asks for mm size, but plplot's dpi is always wrong!
// to get 'charwidthpixel' with this 'wrongdpi', wee need to ask a (wrong) mm size of the character HEIGHT, 
// BUT we do not know the height/width ratio of the font used, plus the fact that some drivers make the wrong calculation!!!!
// AND!!! the expected pixel size (X_CH_SIZE) is the true pixel on screen (or paper), not the fake one
  PLFLT expectedheight_in_mm=charwidthpixel/xdpi*INCHToMM*DEFAULT_FONT_ASPECT_RATIO; //start with a height/width of FONT_ASPECT_RATIO (guessed) will be updated later.
 if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"current (fake?) dpi is %f : asking for a mm height size: %f)\n",ydpi, expectedheight_in_mm);
   plstream::schr(expectedheight_in_mm, 1); 
//trick: if 'em' is not 0, we have the character real width, in mm. It is assumed that when 0, then the size is OK
//if not 0, then we know the height/width ratio and can recompute the 'good' height that will give the 'good' width (in pixels) 
   PLFLT em=0;
#if PLPLOT_PRIVATE_NOT_HIDDEN
    em=gdlGetStringLength(ALLCHARACTERSFORSTRINGLENGTHTEST)/ALLCHARACTERSFORSTRINGLENGTHTEST_NCHARS; //mean of all
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"Able to check character width=%f, should have been %f\n",em, charwidthpixel/xdpi*INCHToMM);
#endif
  if (em > 0) {
    PLFLT ratio=charwidthpixel/xdpi*INCHToMM/em;
    plstream::schr(expectedheight_in_mm*ratio, 1);
   if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"got plplot character height in mm=%f (2nd pass))\n",pls->chrdef);
//#if PLPLOT_PRIVATE_NOT_HIDDEN
//    em=gdlGetStringLength(PATTERN)/PATTERN_LENGTH; //mean of all
//    ratio=charwidthpixel/xdpi*INCHToMM/em;
//    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"re-check character width=%f, ratio is %f\n",em, ratio);
//#endif
  }
 setLineSpacing(lineSpacingpixel/ydpi*INCHToMM); //this one is NOT related to characters idiosyncrasies.
  gdlDefaultCharInitialized=0; //reset Default
  CurrentCharSize(scale);
}


//This defines the character size for FIXED character devices (X, Z )
//The dimension of "average" character (given by X_CH_SIZE) is to be a number of pixels on screen
void GDLGStream::setFixedCharacterSize( PLFLT charwidthpixel, PLFLT scale , PLFLT lineSpacingpixel) {
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"setFixedCharacterSize()\n");
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"schr(width=%f, scale=%f, spacing=%f)\n",charwidthpixel,scale,lineSpacingpixel);
// GDL asks for pixels, plplot asks for mm size, but plplot's dpi is always wrong!
// to get 'charwidthpixel' with this 'wrongdpi', wee need to ask a (wrong) mm size of the character HEIGHT, 
// BUT we do not know the height/width ratio of the font used, plus the fact that some drivers make the wrong calculation!!!!
// AND!!! the expected pixel size (X_CH_SIZE) is the true pixel on screen (or paper), not the fake one
  PLFLT expectedheight=charwidthpixel/pls->xdpi*INCHToMM*DEFAULT_FONT_ASPECT_RATIO; //start with a height/width of FONT_ASPECT_RATIO (guessed) will be updated later.
 if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"current (fake?) dpi is %f : asking for a mm height size: %f)\n",pls->ydpi, expectedheight);
   plstream::schr(expectedheight, 1); 
//trick: if 'em' is not 0, we have the character real width, in mm. It is assumed that when 0, then the size is OK
//if not 0, then we know the height/width ratio and can recompute the 'good' height that will give the 'good' width (in pixels) 
   PLFLT em=0;
#if PLPLOT_PRIVATE_NOT_HIDDEN
    em=gdlGetStringLength(ALLCHARACTERSFORSTRINGLENGTHTEST)/ALLCHARACTERSFORSTRINGLENGTHTEST_NCHARS; //mean of all
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"Able to check character width=%f, should have been %f\n",em, charwidthpixel/pls->xdpi*INCHToMM);
#endif
  if (em > 0) {
    PLFLT ratio=charwidthpixel/pls->xdpi*INCHToMM/em;
    plstream::schr(expectedheight*ratio, 1);
   if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"got plplot character height in mm=%f (2nd pass))\n",pls->chrdef);
//#if PLPLOT_PRIVATE_NOT_HIDDEN
//    em=gdlGetStringLength(PATTERN)/PATTERN_LENGTH; //mean of all
//    ratio=charwidthpixel/pls->xdpi*INCHToMM/em;
//    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"re-check character width=%f, ratio is %f, in pixels:%f\n",em, ratio, em/INCHToMM*pls->xdpi);
//#endif
  }
 setLineSpacing(lineSpacingpixel/pls->ydpi*INCHToMM); //this one is NOT related to characters idiosyncrasies.
  gdlDefaultCharInitialized=0; //reset Default
  CurrentCharSize(scale);
}

void GDLGStream::sizeChar( PLFLT scale )
{
    if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"SizeChar(%f)\n",scale);
  plstream::schr(theDefaultChar.mmsx, scale); //must FORCE a new size.
  CurrentCharSize(scale);
}

bool GDLGStream::vpor(PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax )
{
  //make vpor units really min max, otherwise som problems appear spuriously
  // if (xmin > xmax || ymin > ymax) return true; // invalid: see isue #1387
  if (xmin >= xmax) {xmin=0; xmax=1;} //#1387
  if (ymin >= ymax) {ymin=0; ymax=1;} //#1387
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"vpor(): requesting x[%f:%f],y[%f:%f] (normalized, subpage)\n",xmin,xmax,ymin,ymax);
  //note that plplot apparently does not write the y=0 line of pixels (in device coords). IDL page is on the contrary limited to
  // [0..1[ in both axes (normalized coordinates)
  plstream::vpor(xmin, xmax, ymin, ymax);
  theBox.nx1=xmin;
  theBox.nx2=xmax;
  theBox.ny1=ymin;
  theBox.ny2=ymax;
  theBox.ndx1=xmin;
  theBox.ndx2=xmax;
  theBox.ndy1=ymin;
  theBox.ndy2=ymax;
  theBox.nxsize=xmax-xmin;
  theBox.nysize=ymax-ymin;
  theBox.initialized=true;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"vpor(): got x[%f:%f],x[%f:%f] (normalized, device)\n",theBox.ndx1,theBox.ndx2,theBox.ndy1,theBox.ndy2);
  syncPageInfo();
  return false;
}

//returns true if positioning problem
bool GDLGStream::isovpor(PLFLT x1, PLFLT x2, PLFLT y1,  PLFLT y2,  PLFLT aspect)
{
  if (aspect <= 0.0) {
    return vpor(x1, x2, y1, y2);
  }
  if (x2 <= x1 || y2 <= y1) return true;
  //x1 < x2 && y1 < y2 implied
  PLFLT x1mm = nd2mx(x1);
  PLFLT y1mm = nd2my(y1);
  PLFLT x2mm = nd2mx(x2);
  PLFLT y2mm = nd2my(y2);
  PLFLT ys = y2mm - y1mm; //x and y are in normalized coordinates. ISO scaling must be performed using screen (or paper) coordinates:
  PLFLT xs = x2mm - x1mm;
  PLFLT page_aspect=ys/xs;
  aspect/=page_aspect;
  if (ys >= xs * aspect) { //x ok, resize y
    y2mm = y1mm + aspect*xs;
  } else {
    x2mm = x1mm + xs / aspect;
  }
  x1 = mm2ndx(x1mm);
  x2 = mm2ndx(x2mm);
  y1 = mm2ndy(y1mm);
  y2 = mm2ndy(y2mm);
  return vpor(x1, x2, y1, y2);
}

void GDLGStream::wind( PLFLT xmin, PLFLT xmax, PLFLT ymin, PLFLT ymax )
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"wind(): setting x[%f:%f],y[%f:%f] (world) \n",xmin,xmax,ymin,ymax);
  //silly test to protect against plplot warnings ... side effects unkonwn.
  if (xmin==xmax) {xmin=0; xmax=1;}
  if (ymin==ymax) {ymin=0; ymax=1;}
  plstream::wind(xmin, xmax, ymin, ymax);
  theBox.wx1=xmin;
  theBox.wx2=xmax;
  theBox.wy1=ymin;
  theBox.wy2=ymax;
  updateBoxDeviceCoords();
  UpdateCurrentCharWorldSize();
  setSymbolSizeConversionFactors(); //because symbols are written in world coordinates. 
}

void GDLGStream::wind( PLFLT xmin, PLFLT xmax, bool xLog, PLFLT ymin, PLFLT ymax, bool yLog )
{
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"wind(): setting x[%f:%f],y[%f:%f] (world) \n",xmin,xmax,ymin,ymax);
  if (xLog) {
    xmin = log10(xmin);
    xmax = log10(xmax);
  }
  if (yLog) {
    ymin = log10(ymin);
    ymax = log10(ymax);
  }
  if (xmin==xmax) {xmin=0; xmax=1;}
  if (ymin==ymax) {ymin=0; ymax=1;}
  plstream::wind(xmin, xmax, ymin, ymax);
  theBox.wx1=xmin;
  theBox.wx2=xmax;
  theBox.wy1=ymin;
  theBox.wy2=ymax;
  updateBoxDeviceCoords();
  UpdateCurrentCharWorldSize();
  setSymbolSizeConversionFactors(); //because symbols are written in world coordinates. 
}
void GDLGStream::ssub(PLINT nx, PLINT ny, PLINT nz)
{
//  plstream::ssub( nx, ny ); // does not appear to change charsize.

  // set subpage numbers in X and Y
  thePage.nbPages=nx*ny*nz;
  thePage.nx=nx;
  thePage.ny=ny;
  thePage.nz=nz;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"ssub() %dx%dx%d pages\n",nx,ny,nz);
  thePage.curPage=1;
  syncPageInfo();
}

void GDLGStream::adv(PLINT page)
{
//  plstream::adv(page); //plstream below soes not advance pages as it has only ONE.
  if (page==0) {thePage.curPage++;} else {thePage.curPage=page;}
  if (thePage.curPage > thePage.nbPages) thePage.curPage=1;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"adv() now at page %d\n",thePage.curPage);
  PLFLT sxmin,symin,sxmax,symax,szmin,szmax;
  getSubpageRegion(sxmin,symin,sxmax,symax,&szmin,&szmax);
  //SET ALL REGION TAGS
  unsigned regionTag=SysVar::X()->Desc()->TagIndex("REGION");
  (*static_cast<DFloatGDL*>(SysVar::X()->GetTag(regionTag, 0)))[0]=sxmin;
  (*static_cast<DFloatGDL*>(SysVar::X()->GetTag(regionTag, 0)))[1]=sxmax;
  regionTag=SysVar::Y()->Desc()->TagIndex("REGION");
  (*static_cast<DFloatGDL*>(SysVar::Y()->GetTag(regionTag, 0)))[0]=symin;
  (*static_cast<DFloatGDL*>(SysVar::Y()->GetTag(regionTag, 0)))[1]=symax;
  regionTag=SysVar::Z()->Desc()->TagIndex("REGION");
  (*static_cast<DFloatGDL*>(SysVar::Z()->GetTag(regionTag, 0)))[0]=szmin;
  (*static_cast<DFloatGDL*>(SysVar::Z()->GetTag(regionTag, 0)))[1]=szmax;
}

void GDLGStream::getSubpageRegion(PLFLT &sxmin, PLFLT &symin, PLFLT &sxmax, PLFLT &symax, PLFLT *szmin, PLFLT *szmax){
  int p=thePage.curPage-1;
  PLFLT width=1.0/thePage.nx;
  PLFLT height=1.0/thePage.ny;
  PLFLT profund=1.0/thePage.nz;
 int k= p / (thePage.nx*thePage.ny);
 int l= p - k*(thePage.nx*thePage.ny);
 int j= l /thePage.nx ;
 int i= (l - j*thePage.nx); 
 sxmin=i*width;
 sxmax=sxmin+width;
 symax=1-(j*height);
 symin=symax-height;
 if (szmin != NULL) {
   *szmin=k*profund;
   *szmax=*szmin+profund;
 }
}
//get region (3BPP data)

bool GDLGStream::GetRegion(DLong& xoff, DLong& yoff, DLong& nx, DLong& ny) {
  long nxOrig,nyOrig;
  this->GetGeometry(nxOrig,nyOrig);

  DLong xmax = xoff + nx - 1;
  DLong ymax = yoff + ny - 1;
  if (yoff < 0 || yoff > nyOrig - 1) return false;
  if (xoff < 0 || xoff > nxOrig - 1) return false;
  if (xmax < 0 || xmax > nxOrig - 1) return false;
  if (ymax < 0 || ymax > nyOrig - 1) return false;

  DByteGDL *bitmap = static_cast<DByteGDL*> (this->GetBitmapData(xoff,yoff,nx,ny));
  if (bitmap == NULL) return false; //need to GDLDelete bitmap on exit after this line.
  DByte* bmp=static_cast<DByte*>(bitmap->DataAddr());

  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
  unsigned char* data = actDevice->SetCopyBuffer(nx * ny * 3);
  for (auto k=0; k< nx*ny*3; ++k) data[k] = bmp[k];
  GDLDelete(bitmap);
  return true;
}

bool GDLGStream::SetRegion(DLong& xs, DLong& ys, DLong& nx, DLong& ny){
  DLong pos[4]={xs,nx,ys,ny};
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
  return this->PaintImage(actDevice->GetCopyBuffer(), nx, ny, pos, 1, 0);  
}
//
////plplot main functions patched for 3d
//#include "plplotdriver/plutils.h"
