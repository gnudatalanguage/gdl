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

  void plimage_gdl(unsigned short *idata, PLINT nx, PLINT ny)
  {
    PLINT nnx, nny, ix, iy, ixx, iyy, xm, ym;
    PLFLT dx, dy;
    short *Xf, *Yf;
    PLFLT lzmin, lzmax, tz;
 
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

    //    printf("width: %d  height: %d\n", width,height);
    //printf("xSize: %d  ySize: %d\n", xSize,ySize);

    unsigned short *img2;
    img2 = (unsigned short *) calloc(width*height, sizeof(unsigned short));
    for( SizeT i=0; i<width*height; ++i) {
      SizeT j = width * (i % height) + (i / height);
      img2[i] = (*img)[j] * 257;
    }
    memcpy(&((*img)[0]), img2, width*height*sizeof(unsigned short));
    free(img2);

    plimage_gdl(&(*img)[0], (int) width, (int) height);
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

    //    SizeT nCol = ctSize - start;

    for( SizeT i=0; i<ctSize; ++i) {
      actCT->Get( i, r[ i], g[ i], b[ i]);

      rint[i] = (PLINT) r[i];
      gint[i] = (PLINT) g[i];
      bint[i] = (PLINT) b[i];
    }

    if (actStream != NULL)
      actStream->scmap1( rint, gint, bint, 256 );

  }


} // namespace


