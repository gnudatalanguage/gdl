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


using namespace std;

namespace lib {

  //void plimage_gdl_old(unsigned short *, PLINT, PLINT);


  /*------------------------------------------------------------------------*\
   * PLColor_to_XColor()
   *
   * Copies the supplied PLColor to an XColor, padding with bits as necessary
   * (a PLColor uses 8 bits for color storage, while an XColor uses 16 bits).
   * The argument types follow the same order as in the function name.
   *
   * Taken from Plplot
   \*-----------------------------------------------------------------------*/

#define ToXColor(a) (((0xFF & (a)) << 8) | (a))

  static void
  PLColor_to_XColor(PLColor *plcolor, XColor *xcolor)
  {
    xcolor->red   = ToXColor(plcolor->r);
    xcolor->green = ToXColor(plcolor->g);
    xcolor->blue  = ToXColor(plcolor->b);
    xcolor->flags = DoRed | DoGreen | DoBlue;
  }


  void plimage_gdl(unsigned short *idata, PLINT nx, PLINT ny)
  {
    PLINT ix, iy, xm, ym;

    XwDev *dev = (XwDev *) plsc->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XImage *ximg = NULL;
    XColor curcolor;
    PLINT icol1;

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

    int i, r, ncolors;
    PLColor cmap1color;
    XColor xcol;

    ncolors = 200;
    ncolors = 256;

    for( i = 0; i < ncolors; i++ ) {
      plcol_interp( plsc, &cmap1color, i, ncolors );
      PLColor_to_XColor( &cmap1color, &xcol );

      r = XAllocColor( xwd->display, xwd->map, &xcol );

      if ( r )
	xwd->cmap1[i] = xcol;
      else
	break;
    }
    xwd->ncol1 = ncolors;

    PLINT xoff = (PLINT) (plsc->wpxoff/32767 * dev->width  + 1);
    PLINT yoff = (PLINT) (plsc->wpyoff/24575 * dev->height + 1);
    PLINT kx, ky;

    //    cout << "xoff: "; cout << xoff << endl;
    //cout << "yoff: "; cout << yoff << endl;


    for(ix = 0; ix < nx; ix++) {
      for(iy = 0; iy < ny; iy++) {

	icol1 = idata[ix*(ny)+iy];
	icol1 = (PLINT) (icol1/(float)USHRT_MAX * (xwd->ncol1-1));

	//	/* only plot points within zmin/zmax range */
	//if (icol1 < pls->dev_zmin || icol1 > pls->dev_zmax)
	//  continue;

	if (xwd->color)
	  curcolor = xwd->cmap1[icol1];
	else
	  curcolor = xwd->fgcolor;
	
	//	printf("%d %d %X %X %X %X\n", ix,iy, curcolor.pixel,
	//       curcolor.red,curcolor.green,curcolor.blue);

	kx = xoff + ix;
	ky = yoff + iy;

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

    BaseGDL* p0 = e->GetParDefined( 0);

    DLong xLL=0, yLL=0;
    if (nParam >= 2) e->AssureLongScalarPar( 1, xLL);
    if (nParam >= 3) e->AssureLongScalarPar( 2, yLL);

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

    DUIntGDL* img = 
      static_cast<DUIntGDL*>(p0->Convert2( UINT, BaseGDL::COPY));

    int width = p0->Dim(0);
    int height = p0->Dim(1);

    unsigned short *img2;
    img2 = (unsigned short *) calloc(width*height, sizeof(unsigned short));
    for( SizeT i=0; i<width*height; ++i) {
      SizeT j = width * (i % height) + (i / height);
      img2[i] = (*img)[j] * 257;
    }
    memcpy(&((*img)[0]), img2, width*height*sizeof(unsigned short));
    free(img2);

    plimage_gdl(&(*img)[0], (int) width, (int) height);
    //plimage_gdl_old(&(*img)[0], (int) width, (int) height);
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
    XWindowAttributes win_attr;

    /* query the window's attributes. */
    Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attr);
    SizeT xSize = win_attr.width;
    SizeT ySize = win_attr.height;

    //    int xSize, ySize, xPos, yPos;
    //int actWin = actDevice->ActWin();
    //bool success = actDevice->WSize( actWin, &xSize, &ySize, &xPos, &yPos);

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
      for( SizeT j=1; j<=3; ++j) {
	if (ximg->data[4*i+j] > mx) mx = ximg->data[4*i+j];
      }
      (*res)[ i] = mx;
    }


    XDestroyImage(ximg);

    return res; 
  }

#if 0
  void plimage_gdl_old(unsigned short *idata, PLINT nx, PLINT ny)
  {
    PLINT nnx, nny, ix, iy, ixx, iyy, xm, ym;
    PLFLT dx, dy;
    short *Xf, *Yf;
    PLFLT lzmin, lzmax, tz;
 
    cout << "in old" << endl;

    if (plsc->level < 3) {
      plabort("plimage: window must be set up first");
      return;
    }

    if (nx <= 0 || ny <= 0) {
      plabort("plimage: nx and ny must be positive");
      return;
    }

    dx = 1.0;
    dy = 1.0;
    nnx = nx;
    nny = ny;

    /* The X and Y arrays has size nnx*nny */
    nnx++; nny++;

    Xf = (short *) malloc(nny*nnx*sizeof(short));
    Yf = (short *) malloc(nny*nnx*sizeof(short));

    /* adjust the step for the X/Y arrays */
    dx = dx*(nx-1)/nx;
    dy = dy*(ny-1)/ny;

    for (ix = 0; ix < nnx; ix++)
      for (iy = 0; iy < nny; iy++) {
	Xf[ix*nny+iy] =  plP_wcpcx(1 + ix*dx);
	Yf[ix*nny+iy] =  plP_wcpcy(1 + iy*dy);
    }

    //    plP_image(Xf, Yf, idata, nnx, nny, 1, 1, dx, dy, 0, USHRT_MAX);

    plsc->dev_ix = Xf;
    plsc->dev_iy = Yf;
    plsc->dev_z = idata;
    plsc->dev_nptsX = nnx;
    plsc->dev_nptsY = nny;
    plsc->dev_zmin = 0;
    plsc->dev_zmax = USHRT_MAX;

    plsc->imclxmin = plsc->phyxmi;
    plsc->imclymin = plsc->phyymi;
    plsc->imclxmax = plsc->phyxma;
    plsc->imclymax = plsc->phyyma;

    plsc->plbuf_write = 0;

    plP_esc(PLESC_IMAGE, NULL);
   
    free(Xf);
    free(Yf);
  }
#endif


} // namespace


