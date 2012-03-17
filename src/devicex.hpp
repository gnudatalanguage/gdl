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

#ifndef HAVE_X
#else

#include <algorithm>
#include <vector>
#include <cstring>

//#include <plplot/plstream.h>
#include <plplot/plplotP.h>
#include <plplot/drivers.h>

#include "gdlxstream.hpp"
#include "initsysvar.hpp"

#define ToXColor(a) (((0xFF & (a)) << 8) | (a))
#ifndef free_mem
#define free_mem(a) \
    if (a != NULL) { free((void *) a); a = NULL; }
#endif

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

const int maxWin=32;  

class DeviceX: public Graphics
{
  std::vector<GDLGStream*> winList;
  std::vector<long>        oList;
  long oIx;
  int  actWin;
  int decomposed; // false -> use color table


  void plimage_gdl(unsigned char *idata, PLINT nx, PLINT ny, 
		   DLong tru, DLong chan)
  {
    PLINT ix, iy, xm, ym;

    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL, *ximg_pixmap = NULL;

    int x, y;

    int (*oldErrorHandler)(Display*, XErrorEvent*);

    if (plsc->level < 3) {
      plabort("plimage: window must be set up first");
      return ;
    }

    if (nx <= 0 || ny <= 0) {
      plabort("plimage: nx and ny must be positive");
      return;
    }

    oldErrorHandler = XSetErrorHandler(GetImageErrorHandler);

    XFlush(xwd->display);
    if (dev->write_to_pixmap) {
      ximg = XGetImage( xwd->display, dev->pixmap, 0, 0, 
			dev->width, dev->height,
			AllPlanes, ZPixmap);
      ximg_pixmap = ximg;
    }

    if (dev->write_to_window)
	ximg = XGetImage( xwd->display, dev->window, 0, 0, 
			  dev->width, dev->height,
			  AllPlanes, ZPixmap);

    XSetErrorHandler(oldErrorHandler);

    if (ximg == NULL) {
      XSync(xwd->display, 0);
      x = 0;
      y = 0;
      if (dev->write_to_pixmap) {
	XCopyArea(xwd->display, dev->pixmap, dev->window, dev->gc,
		  x, y, dev->width, dev->height, x, y);
	XSync(xwd->display, 0);
	ximg = ximg_pixmap;
      }
    }


    int ncolors;
    PLINT iclr1, ired, igrn, iblu;
    if (tru == 0 && chan == 0) {

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

    PLINT kxLimit = dev->width - xoff;
    PLINT kyLimit = dev->height - yoff;

    if( nx < kxLimit) kxLimit = nx;
    if( ny < kyLimit) kyLimit = ny;

/*#ifdef _OPENMP
SizeT nOp = kxLimit * kyLimit;
#endif
#pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) private(ired,igrn,iblu,kx,ky,iclr1,curcolor)
{
#pragma omp for*/
    for(ix = 0; ix < kxLimit; ++ix) {
      for(iy = 0; iy < kyLimit; ++iy) {

	kx = xoff + ix;
	ky = yoff + iy;

	if (tru == 0  && chan == 0) {
	  iclr1 = idata[iy*nx+ix];

	  if (xwd->color)
	    curcolor = xwd->cmap1[iclr1];
	  else
	    curcolor = xwd->fgcolor;

	  //	  printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

	} else {
	  if (chan == 0) {
	    if (tru == 1) {
	      ired = idata[3*(iy*nx+ix)+0];
	      igrn = idata[3*(iy*nx+ix)+1];
	      iblu = idata[3*(iy*nx+ix)+2];
	    } else if (tru == 2) {
	      ired = idata[nx*(iy*3+0)+ix];
	      igrn = idata[nx*(iy*3+1)+ix];
	      iblu = idata[nx*(iy*3+2)+ix];
	    } else if (tru == 3) {
	      ired = idata[nx*(0*ny+iy)+ix];
	      igrn = idata[nx*(1*ny+iy)+ix];
	      iblu = idata[nx*(2*ny+iy)+ix];
	    }
	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  } else if (chan == 1) {
	    unsigned long pixel = 
	      XGetPixel(ximg, ix, dev->height-1-ky) & 0x00ffff;
	    ired = idata[1*(iy*nx+ix)+0];
	    curcolor.pixel = ired*256*256 + pixel;
	  } else if (chan == 2) {
	    unsigned long pixel = 
	      XGetPixel(ximg, ix, dev->height-1-ky) & 0xff00ff;
	    igrn = idata[1*(iy*nx+ix)+1];
	    curcolor.pixel = igrn*256 + pixel;
	  } else if (chan == 3) {
	    unsigned long pixel = 
	      XGetPixel(ximg, ix, dev->height-1-ky) & 0xffff00;
	    iblu = idata[1*(iy*nx+ix)+2];
	    curcolor.pixel = iblu + pixel;
	  }
	}

	//std::cout << "XPutPixel: "<<kx<<"  "<< dev->height-ky-1 << std::endl;
	// TODO check if XPutPixel() and XGetPixel() are thread save
	if( ky < dev->height && kx < dev->width)
	XPutPixel(ximg, kx, dev->height-1-ky, curcolor.pixel);
      }
    }
//}

    if (dev->write_to_pixmap)
	XPutImage( xwd->display, dev->pixmap, dev->gc, ximg, 0, 0, 
		   0, 0, dev->width, dev->height);

    if (dev->write_to_window)
	XPutImage( xwd->display, dev->window, dev->gc, ximg, 0, 0,
		   0, 0, dev->width, dev->height);

    if (ximg != ximg_pixmap) {
      XDestroyImage(ximg);
      XDestroyImage(ximg_pixmap);
    } else {
      XDestroyImage(ximg);
    }
    return;
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
	(*static_cast<DLongGDL*>( dStruct->GetTag( xSTag)))[0] = xsize;
	(*static_cast<DLongGDL*>( dStruct->GetTag( ySTag)))[0] = ysize;
	(*static_cast<DLongGDL*>( dStruct->GetTag( xVSTag)))[0] = xsize;
	(*static_cast<DLongGDL*>( dStruct->GetTag( yVSTag)))[0] = ysize;

        // number of colors (based on the color depth from PLPlot)
        (*static_cast<DLongGDL*>( dStruct->GetTag( n_colorsTag)))[0] = 
          1 << (((static_cast<XwDisplay*>((static_cast<XwDev*>(plsc->dev))->xwd))->depth));
      }	

    // window number
    (*static_cast<DLongGDL*>( dStruct->GetTag( wTag)))[0] = wIx;

    actWin = wIx;
  }

  void RaiseWin( int wIx)
  {
    if (wIx >= 0 && wIx < winList.size()) winList[ wIx]->Raise();
  }

  void LowerWin( int wIx)
  {
    if (wIx >= 0 && wIx < winList.size()) winList[ wIx]->Lower();
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


    // set new actWin IF NOT VALID ANY MORE
    if( actWin < 0 || actWin >= wLSize || 
	winList[ actWin] == NULL || !winList[ actWin]->Valid())
      {
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
  }

public:
  DeviceX(): Graphics(), oIx( 1), actWin( -1), decomposed( -1)
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
    dStruct->InitTag("TABLE_SIZE", DLongGDL( ctSize)); 
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

//     GDLGStream::SetErrorHandlers();
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
		{
		delete winList[ wIx];
		winList[ wIx] = NULL;
		}

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

    int debug=0;
    if (debug) cout <<xp<<" "<<yp<<" "<<xleng<<" "<<yleng<<" "<<xoff<<" "<<yoff<<endl;

    DLong xMaxSize, yMaxSize;
    DeviceX::MaxXYSize(&xMaxSize, &yMaxSize);

    xleng = xSize;
    yleng = ySize;
    xoff  = xPos;
    yoff  = yMaxSize-(yPos+ySize);
    if (yoff <= 0) yoff=1;
    
    if (debug) cout <<xp<<" "<<yp<<" "<<xleng<<" "<<yleng<<" "<<xoff<<" "<<yoff<<endl;

    winList[ wIx]->spage( xp, yp, xleng, yleng, xoff, yoff);

    // no pause on win destruction
    winList[ wIx]->spause( false);

    // extended fonts
    winList[ wIx]->fontld( 1);

    // we want color
    winList[ wIx]->scolor( 1);

    // window title
    static char buf[ 256];
    strncpy( buf, title.c_str(), 255);
    buf[ 255] = 0;
    winList[ wIx]->SETOPT( "plwindow", buf);

    // we use our own window handling
    winList[ wIx]->SETOPT( "drvopt","usepth=0");

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

  bool WState( int wIx)
  { 
    return wIx >= 0 && wIx < oList.size() && oList[ wIx] != 0;
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

  bool WShow( int ix, bool show, bool iconic)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if (ix >= wLSize || ix < 0 || winList[ ix] == NULL) return false;
 
    if (show) RaiseWin(ix);
    else LowerWin(ix);

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
  GDLGStream* GetStream( bool open=true)
  {
    ProcessDeleted();
    if( actWin == -1)
      {
	if( !open) return NULL;

	DString title = "GDL 0";
        DLong xSize, ySize;
        DefaultXYSize(&xSize, &ySize);
	bool success = WOpen( 0, title, xSize, ySize, 0, 0);
	if( !success)
	  return NULL;
	if( actWin == -1)
	  {
	    std::cerr << "Internal error: plstream not set." << std::endl;
	    exit( EXIT_FAILURE);
	  }
      }
    return winList[ actWin];
  }

  bool Decomposed( bool value)                
  { 
    decomposed = value;
    return true;
  }

  DLong GetDecomposed()                
  { 
    // initial setting (information from the X-server needed)
    if( decomposed == -1)
    {
      Display* display = XOpenDisplay(NULL);
      if (display == NULL) ThrowGDLException("Cannot connect to X server");

      int Depth;
      Depth=DefaultDepth(display, DefaultScreen(display));      
      decomposed = (Depth >= 15 ? true : false);
      DLong toto=16777216;
      if (Depth == 24) 
	(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = toto;
      int debug=0;
      if (debug) {
	cout << "Display Depth " << Depth << endl;
	cout << "n_colors " << toto << endl;
      }
      XCloseDisplay(display);
    }
    if( decomposed) return 1;
    return 0;
  }

  int MaxWin() { ProcessDeleted(); return winList.size();}
  int ActWin() { ProcessDeleted(); return actWin;}

  BaseGDL* TVRD( EnvT* e)
  {
    // AC 17 march 2012: needed to catch the rigth current window (wset ...)
    DLong wIx = -1;
    Graphics* actDevice = Graphics::GetDevice();
    wIx = actDevice->ActWin();
    bool success = actDevice->WSet( wIx);
    cout << "wIx :" << wIx << " " << success << endl;

    //everywhere we use XGetImage we need to set an error handler, since GTK crashes on every puny
    //BadMatch error, and if you read the XGetImage doc you'll see that such errors are prone to happen
    //as soon as part of the window is obscured.
    int (*oldErrorHandler)(Display*, XErrorEvent*);

    XwDev *dev = (XwDev *) plsc->dev;
    if( dev == NULL || dev->xwd == NULL)
    {
      GDLGStream* newStream = actDevice->GetStream();
      //already done: newStream->Init();
      dev = (XwDev *) plsc->dev;
      if( dev == NULL) e->Throw( "Device not open.");
    }

    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL;

    if (e->KeywordSet("ORDER")) e->Throw( "ORDER keyword not yet supported.");
    if (e->KeywordSet("WORDS")) e->Throw( "WORDS keyword not yet supported.");

    /* this variable will contain the attributes of the window. */
      XWindowAttributes win_attr;

    /* query the window's attributes. */
     Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attr);
    unsigned int xSize = win_attr.width;
    unsigned int ySize = win_attr.height;
    //cout<<xSize<<" "<<ySize<<endl;
    //int xPos, yPos;
    int actWin = /* actDevice-> */ ActWin();
   // bool success = /* actDevice-> */ WSize( actWin, &xSize, &ySize, &xPos, &yPos);
    oldErrorHandler = XSetErrorHandler(GetImageErrorHandler);
    ximg = XGetImage( xwd->display, dev->window, (int) 0, (int) 0,
		      xSize, ySize, AllPlanes, ZPixmap);
    XSetErrorHandler(oldErrorHandler);

    SizeT dims[3];
    
    DByteGDL* res;

    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);

    DLong channel=-1;
    e->AssureLongScalarKWIfPresent( "CHANNEL", channel);
    if (channel > 3) e->Throw("Value of Channel is out of allowed range.");

    unsigned int x0u=0;
    unsigned int y0u=0;
    unsigned int Nxu=xSize-1;
    unsigned int Nyu=ySize-1;
    unsigned int Channelu=0;

    int nParam = e->NParam();
    if (nParam >= 1) {
      DLongGDL* x0 = e->GetParAs<DLongGDL>(0);
      x0u=(*x0)[0];
    }
    if (nParam >= 2) {
      DLongGDL* y0 = e->GetParAs<DLongGDL>(1);
      y0u=(*y0)[0];
    }
    if (nParam >= 3) {
      DLongGDL* Nx = e->GetParAs<DLongGDL>(2);
      Nxu=(*Nx)[0];
    }
    if (nParam >= 4) {
      DLongGDL* Ny = e->GetParAs<DLongGDL>(3);
      Nyu=(*Ny)[0];
    }
    if (nParam == 5) {
      DLongGDL* Channel = e->GetParAs<DLongGDL>(4);
      Channelu=(*Channel)[0];
    }

    int debug =1;
    if (debug) {
      cout << "hello"<<endl;
      //     cout << (*x0)[0] <<" "<< (*x1)[0] <<" "<< (*Nx)[0] <<" "<< (*Ny)[0] <<" "<< (*Channel)[0] <<endl;
      cout << x0u <<" "<< y0u <<" "<< Nxu <<" "<< Nyu <<" "<< Channelu <<endl;
    }

    if (tru == 0 || channel != -1) {
      dims[0] = xSize;
      dims[1] = ySize;
      dimension dim(dims, (SizeT) 2);
      res = new DByteGDL( dim, BaseGDL::ZERO);

      if (ximg == NULL) return res;

      if (channel <= 0) {
        DByte mx, mx1;
        for (SizeT i = 0; i < xSize * ySize; ++i) {
          mx = (DByte) ximg->data[4 * i];
          for (SizeT j = 1; j <= 2; ++j) {
            mx1 = (DByte) ximg->data[4 * i + j];
            if (mx1 > mx) mx = mx1;
          }
          (*res)[ xSize * ySize - 1 - i] = mx;
        }
      } else {
        for (SizeT i = 0; i < xSize * ySize; ++i) {
          (*res)[ xSize * ySize - 1 - i] = ximg->data[4 * i + (3 - channel)];
        }
      }

      // Reflect about y-axis
      for (SizeT i = 0; i < ySize; ++i) {
        for (SizeT j = 0; j < xSize / 2; ++j) {
          DByte c = (*res)[ i * xSize + (xSize - 1 - j)];
          (*res)[ i * xSize + (xSize - 1 - j)] = (*res)[ i * xSize + j];
          (*res)[ i * xSize + j] = c;
        }
      }
      return res;

    } else {
      if (tru > 3) e->Throw("Value of TRUE keyword is out of allowed range.");

      // AC 17 march 2012
      // below something is wrong
      // window, 3,xsize=10, ysize=7
      // plot, findgen(10), col='ff'x, back='ff'x ;; all in red
      // uu=tvrd(/tru)
      // plot, uu(0,*,*) ; should be at 255, but not all :(
      // It seems to be related to refreshment of the screen !

      if (nParam ==0)
	{
	  // AC 17 march 2012 : we may have a mirror effect here
	  dims[0] = 3;
	  dims[1] = xSize;
	  dims[2] = ySize;
	  dimension dim(dims, (SizeT) 3);
	  res = new DByteGDL(dim, BaseGDL::NOZERO);
	  if (ximg == NULL) return res;
	  
	  SizeT jj=0;
	  for (SizeT i = 0; i < xSize; ++i) {
	    for (SizeT j = 0; j < ySize; ++j) {
	      for (SizeT k = 0; k < 4; ++k) {
		if (k < 3) {
		  (*res)[xSize*j * 3 + i * 3 + 2 - k] =
		        ximg->data[j * xSize * 4 + i * 4 + k];
		}
	      }
	    }
	  }
	}
      else
	{
	  // AC 17 march 2012 : we may have a mirror effect here
	  dims[0] = 3;
	  dims[1] = Nxu;
	  dims[2] = Nyu;
	  cout << dims[1] << " " << dims[2] <<endl;
	  dimension dim(dims, (SizeT) 3);
	  res = new DByteGDL(dim, BaseGDL::NOZERO);
	  if (ximg == NULL) return res;
	  
	  for (SizeT i = x0u; i < x0u+Nxu; ++i) {
	    for (SizeT j = y0u; j < y0u+Nyu; ++j) {
	      for (SizeT k = 0; k < 4; ++k) {
		if (k < 3) {		  
		  (*res)[dims[1]*(j-y0u) * 3 + (i-x0u) * 3 + 2 - k] =
		    ximg->data[j * xSize * 4 + i * 4 + k];
		}
	      }
	    }
	  }
	}
    }

    XDestroyImage(ximg);

    DUInt* perm = new DUInt[3];
    if (tru == 1) {
      return res;
    } else if (tru == 2) {
      perm[0] = 1;
      perm[1] = 0;
      perm[2] = 2;
      return res->Transpose( perm);
    } else if (tru = 3) {
      perm[0] = 1;
      perm[1] = 2;
      perm[2] = 0;
      return res->Transpose( perm);
    }
  }

  void TV( EnvT* e)
  {
    //    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam( 1); 

    GDLGStream* actStream = GetStream();
    if( actStream == NULL)
      {
		std::cerr << "TV: Internal error: plstream not set." << std::endl;
		exit( EXIT_FAILURE);
      }

    //    actStream->NextPlot( false);
    actStream->NoSub();

    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;

    int xSize, ySize, xPos, yPos;
    int actWin = ActWin();
    bool success = WSize( actWin, &xSize, &ySize, &xPos, &yPos);

    BaseGDL* p0=e->GetParDefined( 0);
    SizeT rank = p0->Rank();

    if (rank < 2 || rank > 3) e->Throw("Image array must have rank 2 or 3");

    DLong orderVal=0;
    e->AssureLongScalarKWIfPresent( "ORDER", orderVal);
    //std::cout << "OrderVal "<< orderVal << std::endl;      
    if (orderVal != 0) p0=p0->Rotate(7);

    //DByteGDL* p0B = e->GetParAs<DByteGDL>( 0);
    DByteGDL* p0B;
    p0B =static_cast<DByteGDL*>(p0->Convert2(BYTE,BaseGDL::COPY));
    e->Guard( p0B);
    
    int width, height;
    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);

    if (rank == 2) {
      if (tru != 0) e->Throw("Array must have 3 dimensions: " +e->GetParString(0));

      width = p0B->Dim(0);
      height = p0B->Dim(1);

    } 
    if (rank == 3) {
      if (tru < 0 || tru > 3) e->Throw("Value of TRUE keyword is out of allowed range.");
      if (tru == 1 && xwd->depth < 24) e->Throw("Device depth must be 24 or greater for true color display");

      int DimProblem=0;
      if (tru == 1) {
	if (p0B->Dim(1) < 3) DimProblem=1;
	width = p0B->Dim(1);
	height = p0B->Dim(2);
      }
      if (tru == 2) {
	if (p0B->Dim(1) < 3) DimProblem=1;
	width = p0B->Dim(0);
	height = p0B->Dim(2);
      } 
      if (tru == 3) {
	if (p0B->Dim(2) < 3) DimProblem=1;
	width = p0B->Dim(0);
	height = p0B->Dim(1);
      }
      if (tru == 0) {  // here we have a rank =3
	width = p0B->Dim(0);
	height = p0B->Dim(1);
	if (p0B->Dim(0) == 1) {
	  width = p0B->Dim(1);
	  height = p0B->Dim(2);
	} 
	if (p0B->Dim(1) == 1) {
	  width = p0B->Dim(0);
	  height = p0B->Dim(2);
	}
      }
      if (DimProblem == 1) {
        e->Throw("Array <BYTE     Array[" +i2s(p0B->Dim(0))+","+
			    i2s(p0B->Dim(1))+","+i2s(p0B->Dim(2))
			    +"]> does not have enough elements.");
      }
    }
    int debug=0;
    if (debug == 1) {
      std::cout << "==================== " << std::endl;
      std::cout << "true " << tru <<std::endl;
      std::cout << "Rank " << rank <<std::endl;
      std::cout << "width " << width <<std::endl;
      std::cout << "height " << height <<std::endl;
      std::cout << "nParam " << nParam <<std::endl;
    }

    DLong xLL=0, yLL=0, pos=0;
    if (nParam == 2) {
      int nx, ny, ix, iy;
      e->AssureLongScalarPar( 1, pos);
      nx = xSize/width;
      ny = ySize/height;
      // AC 2011/11/06, bug 3433502
      if (nx > 0) { ix = pos % nx;} else ix=0;
      if (ny > 0) { iy = (pos / nx) % ny;} else iy=0;
      xLL= width*ix;
      yLL= ySize - height*(iy+1);
    }
    else if (nParam >= 3) {
      if (e->KeywordSet(1)) { // NORMAL
	DDouble xLLf, yLLf;
	e->AssureDoubleScalarPar( 1, xLLf);
	e->AssureDoubleScalarPar( 2, yLLf);
	xLL = (DLong) rint(xLLf * xSize);
	yLL = (DLong) rint(yLLf * ySize);
      } else {
	e->AssureLongScalarPar( 1, xLL);
	e->AssureLongScalarPar( 2, yLL);
      }
    }

    actStream->vpor( 0, 1.0, 0, 1.0);
    actStream->wind( 1-xLL, xSize-xLL, 1-yLL, ySize-yLL);
    
    if (debug == 1) {
      std::cout << "xLL :" << xLL << std::endl;
      std::cout << "yLL :" << yLL << std::endl;
      std::cout << "xSize :" << xSize << std::endl;
      std::cout << "ySize :" << ySize << std::endl;
    }
    DLong channel=0;
    e->AssureLongScalarKWIfPresent( "CHANNEL", channel);
    if (channel < 0 || channel > 3)
      e->Throw("Value of Channel is out of allowed range.");

    std::auto_ptr<BaseGDL> chan_guard;
    if (channel == 0) {
      plimage_gdl(&(*p0B)[0], width, height, tru, channel);
    } else if (rank == 3) {
      // Rank == 3 w/channel
      SizeT dims[2];
      dims[0] = width;
      dims[1] = height;
      dimension dim( dims, 2); 
      DByteGDL* p0B_chan = new DByteGDL( dim, BaseGDL::ZERO);
      for( SizeT i=(channel-1); i<p0B->N_Elements(); i+=3) {
	(*p0B_chan)[i/3] = (*p0B)[i];
      }
      // Send just single channel
      plimage_gdl(&(*p0B_chan)[0], width, height, tru, channel);
      chan_guard.reset( p0B_chan); // delete upon exit
    } else if (rank == 2) {
      // Rank = 2 w/channel
      plimage_gdl(&(*p0B)[0], width, height, tru, channel);
    }
  }

  /*------------------------------------------------------------------------*\
   * GetImageErrorHandler()
   *
   * Error handler used in XGetImage() to catch errors when pixmap or window
   * are not completely viewable.
   \*-----------------------------------------------------------------------*/

  static int
  GetImageErrorHandler(Display *display, XErrorEvent *error)
  {
    if (error->error_code != BadMatch) {
      char buffer[256];
      XGetErrorText(display, error->error_code, buffer, 256);
      std::cerr << "xwin: Error in XGetImage: " << buffer << std::endl;
    }
    return 1;
  }

  static void DefaultXYSize(DLong *xSize, DLong *ySize)
  {
    *ySize = 640;
    *ySize = 512;
#ifdef HAVE_X
    Display* display = XOpenDisplay(NULL);
    if (display != NULL)
    {   
      *xSize = DisplayWidth(display, DefaultScreen(display)) / 2; 
      *ySize = DisplayHeight(display, DefaultScreen(display)) / 2;
      XCloseDisplay(display);
    }   
#endif
  }
  
  static void MaxXYSize(DLong *xSize, DLong *ySize)
  {
    *ySize = 640;
    *ySize = 512;
#ifdef HAVE_X
    Display* display = XOpenDisplay(NULL);
    if (display != NULL)
    {
      *xSize = DisplayWidth(display, DefaultScreen(display));
      *ySize = DisplayHeight(display, DefaultScreen(display));
      XCloseDisplay(display);
    }
#endif
  }

};


#endif

#endif
