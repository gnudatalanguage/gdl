/***************************************************************************
                          image.cpp  -  GDL image routines
                             -------------------
    begin                : Jul 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// PLplot is used for direct graphics

#include "graphics.hpp"
#include "image.hpp"

#define ToXColor(a) (((0xFF & (a)) << 8) | (a))

using namespace std;

namespace lib {

  void plimage_gdl(unsigned char *idata, PLINT nx, PLINT ny, DInt tru)
  {
    PLINT ix, iy, xm, ym;

    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL;

    if (plsc->level < 3) {
      plabort("plimage: window must be set up first");
      return;
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
	plabort("Can't get image, window partly off-screen, move to fit screen");
	return;
    }


    int ncolors;
    PLINT iclr1, ired, igrn, iblu;
    if (tru == 0) {

      ncolors = 256;
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


  void tv( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam( 1); 

    DLong xLL=0, yLL=0, pos=0;
    if (nParam >= 2) 
      e->AssureLongScalarPar( 1, pos);
    if (nParam >= 3) {
      e->AssureLongScalarPar( 1, xLL);
      e->AssureLongScalarPar( 2, yLL);
    }

    GDLGStream* actStream = actDevice->GetStream();
    if( actStream == NULL)
      {
	DString title = "GDL 0";
	bool success = actDevice->WOpen( 0, title, 640, 512, 0, 0);
	if( !success)
	  throw GDLException( e->CallingNode(), 
			      "TV: Unable to create window.");
	actStream = actDevice->GetStream();
	if( actStream == NULL)
	  {
	    cerr << "TV: Internal error: plstream not set." << endl;
	    exit( EXIT_FAILURE);
	  }
      }
    
    int xSize, ySize, xPos, yPos;
    int actWin = actDevice->ActWin();
    bool success = actDevice->WSize( actWin, &xSize, &ySize, &xPos, &yPos);

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
			    "TV: Device depth must be 24 or greater for true color display");
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


  void loadct( EnvT* e)
  {
    DLong iCT;

    DByte r[256], g[256], b[256];
    PLINT rint[256], gint[256], bint[256];

    e->AssureLongScalarPar( 0, iCT);

    if( iCT < 0 || iCT > 40)
      throw GDLException( e->CallingNode(), 
  			  "LOADCT: Table number must be from 0 to 40.");

    Graphics* actDevice = Graphics::GetDevice();
    GDLGStream* actStream = actDevice->GetStream();

    Graphics::LoadCT( iCT);

    GDLCT* actCT = Graphics::GetCT();

    for( SizeT i=0; i<ctSize; ++i) {
      actCT->Get( i, r[ i], g[ i], b[ i]);

      rint[i] = (PLINT) r[i];
      gint[i] = (PLINT) g[i];
      bint[i] = (PLINT) b[i];
    }

    if (actStream != NULL)
      actStream->scmap1( rint, gint, bint, 256 );

  }


  BaseGDL* tvrd( EnvT* e)
  {
    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL;

    Graphics* actDevice = Graphics::GetDevice();

    SizeT nParam=e->NParam(); 


    /* this variable will contain the attributes of the window. */
    //    XWindowAttributes win_attr;

    /* query the window's attributes. */
    // Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attr);
    // SizeT xSize = win_attr.width;
    // SizeT ySize = win_attr.height;

    int xSize, ySize, xPos, yPos;
    int actWin = actDevice->ActWin();
    bool success = actDevice->WSize( actWin, &xSize, &ySize, &xPos, &yPos);

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

} // namespace

