/* *************************************************************************
                          devicex.hpp  -  X windows device
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

#ifndef DEVICEX_HPP_
#define DEVICEX_HPP_

#include <algorithm>
#include <vector>
#include <cstring>

//#include <plplot/plstream.h>
#include <plplot/plplotP.h>

#include "gdlxstream.hpp"
#include "initsysvar.hpp"

#define ToXColor(a) (((0xFF & (a)) << 8) | (a))
#define free_mem(a) \
    if (a != NULL) { free((void *) a); a = NULL; }


const int maxWin=32;  

class DeviceX: public Graphics
{
  std::vector<GDLGStream*> winList;
  std::vector<long>        oList;
  long oIx;
  int  actWin;
  bool decomposed; // false -> use color table

  void plimage_gdl(unsigned char *idata, PLINT nx, PLINT ny, DInt tru)
  {
    PLINT ix, iy, xm, ym;

    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL;

    if (plsc->level < 3) {
      plabort("plimage: window must be set up first");
      return ;
    }

    if (nx <= 0 || ny <= 0) {
      plabort("plimage: nx and ny must be positive");
      return;
    }

    XFlush(xwd->display);
    if (dev->write_to_pixmap)
	ximg = XGetImage( xwd->display, dev->pixmap, 0, 0, 
			  dev->width, dev->height,
			  AllPlanes, ZPixmap);

    if (dev->write_to_window)
	ximg = XGetImage( xwd->display, dev->window, 0, 0, 
			  dev->width, dev->height,
			  AllPlanes, ZPixmap);

    if (ximg == NULL) {
	plabort("Can't get image, window partly off-screen,"
		" move to fit screen");
	return;
    }


    int ncolors;
    PLINT iclr1, ired, igrn, iblu;
    if (tru == 0) {

      ncolors = 256;

#if PL_RGB_COLOR == -1
      free_mem(xwd->cmap1);
      xwd->cmap1 = (XColor *) calloc(ncolors, (size_t) sizeof(XColor));
#endif

      for( SizeT i = 0; i < ncolors; i++ ) {

	xwd->cmap1[i].red   = ToXColor(plsc->cmap1[i].r);
	xwd->cmap1[i].green = ToXColor(plsc->cmap1[i].g);
	xwd->cmap1[i].blue  = ToXColor(plsc->cmap1[i].b);
	xwd->cmap1[i].flags = DoRed | DoGreen | DoBlue;
	
	if ( XAllocColor( xwd->display, xwd->map, &xwd->cmap1[i]) == 0)
	  break;
      }
      xwd->ncol1 = ncolors;
    }

    PLINT xoff = (PLINT) (plsc->wpxoff/32767 * dev->width  + 1);
    PLINT yoff = (PLINT) (plsc->wpyoff/24575 * dev->height + 1);
    PLINT kx, ky;

    XColor curcolor;
    for(ix = 0; ix < nx; ix++) {
      for(iy = 0; iy < ny; iy++) {

	kx = xoff + ix;
	ky = yoff + iy;

	if (tru == 0) {
	  iclr1 = idata[iy*nx+ix];

	  if (xwd->color)
	    curcolor = xwd->cmap1[iclr1];
	  else
	    curcolor = xwd->fgcolor;

	  //	  printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

	} else {

	  if (tru == 1) {
	    ired = idata[3*(iy*nx+ix)+0];
	    igrn = idata[3*(iy*nx+ix)+1];
	    iblu = idata[3*(iy*nx+ix)+2];
	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  } else if (tru == 2) {
	    ired = idata[nx*(iy*3+0)+ix];
	    igrn = idata[nx*(iy*3+1)+ix];
	    iblu = idata[nx*(iy*3+2)+ix];
	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  } else if (tru == 3) {
	    ired = idata[nx*(0*ny+iy)+ix];
	    igrn = idata[nx*(1*ny+iy)+ix];
	    iblu = idata[nx*(2*ny+iy)+ix];
	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  }

	}

	//	cout << "XPutPixel:\n"<<kx<<"  "<< dev->height-1-ky << endl;
	XPutPixel(ximg, kx, dev->height-1-ky, curcolor.pixel);
      }
    }

    if (dev->write_to_pixmap)
	XPutImage( xwd->display, dev->pixmap, dev->gc, ximg, 0, 0, 
		   0, 0, dev->width, dev->height);

    if (dev->write_to_window)
	XPutImage( xwd->display, dev->window, dev->gc, ximg, 0, 0,
		   0, 0, dev->width, dev->height);

    XDestroyImage(ximg);
  }


  void SetActWin( int wIx)
  {
    // update !D
    if( wIx >= 0 && wIx < winList.size()) 
      {
	long xsize,ysize,xoff,yoff;
	winList[ wIx]->GetGeometry( xsize, ysize, xoff, yoff);

	// window size and pos
// 	PLFLT xp; PLFLT yp; 
// 	PLINT xleng; PLINT yleng;
// 	PLINT xoff; PLINT yoff;
// 	winList[ wIx]->gpage( xp, yp, xleng, yleng, xoff, yoff);
	(*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0] = xsize;
	(*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0] = ysize;
	(*static_cast<DLongGDL*>( dStruct->Get( xVSTag, 0)))[0] = xsize;
	(*static_cast<DLongGDL*>( dStruct->Get( yVSTag, 0)))[0] = ysize;
      }	

    // window number
    (*static_cast<DLongGDL*>( dStruct->Get( wTag, 0)))[0] = wIx;

    actWin = wIx;
  }

  // process user deleted windows
  // should be done in a thread
  void ProcessDeleted()
  {
    int wLSize = winList.size();

//     bool redo;
//     do { // it seems that the event queue is only searched a few events deep
//       redo = false;
      for( int i=0; i<wLSize; i++)
	if( winList[ i] != NULL && !winList[ i]->Valid()) 
	  {
	    delete winList[ i];
	    winList[ i] = NULL;
	    oList[ i] = 0;
// 	    redo = true;
	  }
//     } while( redo);

    // set to most recently created
    std::vector< long>::iterator mEl = 
      std::max_element( oList.begin(), oList.end());
    
    // no window open
    if( *mEl == 0) 
      {
	SetActWin( -1);
	oIx = 1;
      }
    else
      SetActWin( std::distance( oList.begin(), mEl)); 
  }

public:
  DeviceX(): Graphics(), oIx( 1), actWin( -1), decomposed( false)
  {
    name = "X";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;


    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( 640)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( 512)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( 640)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( 512)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 6)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 9)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 40.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 40.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 0)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 328124)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 

    winList.resize( maxWin);
    for( int i=0; i < maxWin; i++) winList[ i] = NULL;
    oList.resize( maxWin);
    for( int i=0; i < maxWin; i++) oList[ i] = 0;

    GDLGStream::SetErrorHandlers();
  }
  
  ~DeviceX()
  {
    std::vector<GDLGStream*>::iterator i;
    for(i = winList.begin(); i != winList.end(); ++i) 
      { delete *i; *i = NULL;}
  }

  void EventHandler()
  {
    int wLSize = winList.size();
    for( int i=0; i<wLSize; i++)
      if( winList[ i] != NULL)
	winList[ i]->EventHandler();

    ProcessDeleted();
  }

  bool WDelete( int wIx)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0 || winList[ wIx] == NULL)
      return false;

    delete winList[ wIx];
    winList[ wIx] = NULL;
    oList[ wIx] = 0;

    // set to most recently created
    std::vector< long>::iterator mEl = 
      std::max_element( oList.begin(), oList.end());
    
    // no window open
    if( *mEl == 0) 
      {
	SetActWin( -1);
	oIx = 1;
      }
    else
      SetActWin( std::distance( oList.begin(), mEl)); 

    return true;
  }

  bool WOpen( int wIx, const std::string& title, 
	      int xSize, int ySize, int xPos, int yPos)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0)
      return false;

    if( winList[ wIx] != NULL)
      delete winList[ wIx];

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    winList[ wIx] = new GDLXStream( nx, ny);
    oList[ wIx]   = oIx++;

    // set initial window size
    PLFLT xp; PLFLT yp; 
    PLINT xleng; PLINT yleng;
    PLINT xoff; PLINT yoff;
    winList[ wIx]->gpage( xp, yp, xleng, yleng, xoff, yoff);

    xleng = xSize;
    yleng = ySize;
    xoff  = xPos;
    yoff  = yPos;

    winList[ wIx]->spage( xp, yp, xleng, yleng, xoff, yoff);

    // no pause on win destruction
    winList[ wIx]->spause( 0);

    // extended fonts
    winList[ wIx]->fontld( 1);

    // we want color
    winList[ wIx]->scolor( 1);

    // *** esc char
    // winList[ wIx]->sesc( '!');

    // window title
    static char buf[ 256];
    strncpy( buf, title.c_str(), 255);
    buf[ 255] = 0;
    winList[ wIx]->SetOpt( "plwindow", buf);

    // we use our own window handling
    winList[ wIx]->SetOpt( "drvopt","usepth=0");

    // set color map
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    //    winList[ wIx]->scmap0( r, g, b, ctSize); 
    winList[ wIx]->scmap1( r, g, b, ctSize); 

    winList[ wIx]->Init();
    
    // load font
    winList[ wIx]->font( 1);
    winList[ wIx]->DefaultCharSize();

    //    (*pMulti)[ 0] = nx*ny;

    // need to be called initially
    winList[ wIx]->adv(0);

    // sets actWin and updates !D
    SetActWin( wIx);

    return true; //winList[ wIx]->Valid(); // Valid() need to called once
  }

  bool WSize( int wIx, int *xSize, int *ySize, int *xPos, int *yPos)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if( wIx > wLSize || wIx < 0)
      return false;

    long xleng, yleng;
    long xoff, yoff;
    winList[ wIx]->GetGeometry( xleng, yleng, xoff, yoff);

    *xSize = xleng;
    *ySize = yleng;
    *xPos = xoff;
    *yPos = yoff;

    return true;
  }

  bool WSet( int wIx)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0 || winList[ wIx] == NULL)
      return false;

    SetActWin( wIx);
    return true;
  }

  int WAdd()
  {
    ProcessDeleted();

    int wLSize = winList.size();
    for( int i=maxWin; i<wLSize; i++)
      if( winList[ i] == NULL) return i;

    // plplot allows only 101 windows
    if( wLSize == 101) return -1;

    winList.push_back( NULL);
    oList.push_back( 0);
    return wLSize;
  }

  // should check for valid streams
  GDLGStream* GetStream()
  {
    ProcessDeleted();
    if( actWin == -1) return NULL;
    return winList[ actWin];
  }

  bool Decomposed( bool value)                
  { 
    decomposed = value;
    return true;
  }

  int MaxWin() { ProcessDeleted(); return maxWin;}
  int ActWin() { ProcessDeleted(); return actWin;}

  BaseGDL* TVRD( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL;

    /* this variable will contain the attributes of the window. */
    //    XWindowAttributes win_attr;

    /* query the window's attributes. */
    // Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attr);
    // SizeT xSize = win_attr.width;
    // SizeT ySize = win_attr.height;

    int xSize, ySize, xPos, yPos;
    int actWin = /* actDevice-> */ ActWin();
    bool success = /* actDevice-> */ WSize( actWin, &xSize, &ySize, &xPos, &yPos);

    ximg = XGetImage( xwd->display, dev->window, 0, 0, 
		      xSize, ySize, AllPlanes, ZPixmap);

    SizeT dims[2];
    dims[0] = xSize;
    dims[1] = ySize;
    dimension dim(dims, (SizeT) 2);
    DByteGDL* res = new DByteGDL( dim, BaseGDL::NOZERO);

    DByte mx;
    for( SizeT i=0; i<xSize*ySize; ++i) {
      mx = ximg->data[4*i];
      for( SizeT j=1; j<=2; ++j) {
	if (ximg->data[4*i+j] > mx) mx = ximg->data[4*i+j];
      }
      (*res)[ xSize*ySize-1-i] = mx;
    }
    
    // Reflect about y-axis
    for( SizeT i=0; i<ySize; ++i) {
      for( SizeT j=0; j<xSize/2; ++j) {
	DByte c = (*res)[ i*xSize+(xSize-1-j)];
	(*res)[ i*xSize+(xSize-1-j)] = (*res)[ i*xSize+j];
	(*res)[ i*xSize+j] = c;
      }
    }
    
    XDestroyImage(ximg);
    
    return res; 
  }


  void TV( EnvT* e)
  {
    //    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam( 1); 

    DLong xLL=0, yLL=0, pos=0;
    if (nParam >= 2) 
      e->AssureLongScalarPar( 1, pos);
    if (nParam >= 3) {
      e->AssureLongScalarPar( 1, xLL);
      e->AssureLongScalarPar( 2, yLL);
    }

    GDLGStream* actStream = /* actDevice-> */ GetStream();
    if( actStream == NULL)
      {
	DString title = "GDL 0";
	bool success = /* actDevice-> */ WOpen( 0, title, 640, 512, 0, 0);
	if( !success)
	  throw GDLException( e->CallingNode(), 
			      "TV: Unable to create window.");
	actStream = /* actDevice-> */ GetStream();
	if( actStream == NULL)
	  {
	    std::cerr << "TV: Internal error: plstream not set." << std::endl;
	    exit( EXIT_FAILURE);
	  }
      }

    actStream->NextPlot( false);

    int xSize, ySize, xPos, yPos;
    int actWin = /* actDevice-> */ ActWin();
    bool success = /* actDevice-> */ 
      WSize( actWin, &xSize, &ySize, &xPos, &yPos);

    actStream->vpor( 0, 1.0, 0, 1.0);
    actStream->wind( 1-xLL, xSize-xLL, 1-yLL, ySize-yLL);

    DByteGDL* p0B = e->GetParAs<DByteGDL>( 0);
    SizeT rank = p0B->Rank();
    int width, height;
    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);
    if (rank == 2) {
      if (tru != 0)
	throw GDLException( e->CallingNode(),
			    "TV: Array must have 3 dimensions: "+
			    e->GetParString(0));
      width = p0B->Dim(0);
      height = p0B->Dim(1);

    } else if (rank == 3) {
      XwDev *dev = (XwDev *) plsc->dev;
      XwDisplay *xwd = (XwDisplay *) dev->xwd;
      if (tru == 1 && xwd->depth < 24) {
	throw GDLException( e->CallingNode(), 
			    "TV: Device depth must be 24 or greater "
			    "for true color display");
      return;
    }
      if (tru == 1) {
	width = p0B->Dim(1);
	height = p0B->Dim(2);
      } else if (tru == 2) {
	width = p0B->Dim(0);
	height = p0B->Dim(2);
      } else if (tru == 3) {
	width = p0B->Dim(0);
	height = p0B->Dim(1);
      } else {
	throw GDLException( e->CallingNode(), 
			    "TV: TRUE must be between 1 and 3");
      }
    } else {
      throw GDLException( e->CallingNode(), 
			  "Image array must have rank 2 or 3");
    }
    plimage_gdl(&(*p0B)[0], width, height, tru);
  }
};


#endif
