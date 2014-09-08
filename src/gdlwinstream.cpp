/* *************************************************************************
						  gdlwinstream.cpp  -  graphic stream M$ windows
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
using namespace std;
#include "gdlwinstream.hpp"
#include "devicewin.hpp"

void GDLWINStream::Init()
{
	this->plstream::init();
	plgpls(&pls);
	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
	dev->waiting = 1;
}

void GDLWINStream::EventHandler()
{
	MSG Message;
	if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

bool GDLWINStream::PaintImage( unsigned char *idata, PLINT nx, PLINT ny, DLong *pos, DLong tru, DLong chan ) {
  plP_esc( PLESC_FLUSH, NULL );
  
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  HDC hdc = dev->hdc;

  PLINT xoff = (PLINT) pos[0];
  PLINT yoff = (PLINT) pos[2];

  PLINT xsize = pls->phyxma;
  PLINT ysize = pls->phyyma;

  PLINT kxLimit = xsize - xoff;
  PLINT kyLimit = ysize - yoff;

  if ( nx < kxLimit ) kxLimit = nx;
  if ( ny < kyLimit ) kyLimit = ny;

  if ( nx > 0 && ny > 0 ) {
    char iclr1, ired, igrn, iblu;
    long curcolor;

    for ( ix = 0; ix < kxLimit; ++ix ) {
      for ( iy = 0; iy < kyLimit; ++iy ) {

        kx = xoff + ix;
        ky = yoff + iy;

        if ( tru == 0 && chan == 0 ) {
          iclr1 = idata[iy * nx + ix];

          curcolor = RGB( pls->cmap1[iclr1].r,
          pls->cmap1[iclr1].g, pls->cmap1[iclr1].b );

          //	 			 printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

        } else {
          if ( chan == 0 ) {
            if ( tru == 1 ) {
              ired = idata[3 * (iy * nx + ix) + 0];
              igrn = idata[3 * (iy * nx + ix) + 1];
              iblu = idata[3 * (iy * nx + ix) + 2];
            } else if ( tru == 2 ) {
              ired = idata[nx * (iy * 3 + 0) + ix];
              igrn = idata[nx * (iy * 3 + 1) + ix];
              iblu = idata[nx * (iy * 3 + 2) + ix];
            } else if ( tru == 3 ) {
              ired = idata[nx * (0 * ny + iy) + ix];
              igrn = idata[nx * (1 * ny + iy) + ix];
              iblu = idata[nx * (2 * ny + iy) + ix];
            }
            curcolor = RGB( ired, igrn, iblu ); // It is reverse!
          } else if ( chan == 1 ) {
            unsigned long pixel =
            GetPixel( hdc, ix, dev->height - 1 - ky ) & 0xffff00;
            ired = idata[1 * (iy * nx + ix) + 0];
            curcolor = RGB( ired, 0, 0 ) + pixel; // =ired*256*256 + pixel
          } else if ( chan == 2 ) {
            unsigned long pixel =
            GetPixel( hdc, ix, dev->height - 1 - ky ) & 0xff00ff;
            igrn = idata[1 * (iy * nx + ix) + 1];
            curcolor = RGB( 0, igrn, 0 ) + pixel; // = igrn * 256 + pixel
          } else if ( chan == 3 ) {
            unsigned long pixel =
            GetPixel( hdc, ix, dev->height - 1 - ky ) & 0x00ffff;
            iblu = idata[1 * (iy * nx + ix) + 2];
            curcolor = RGB( 0, 0, iblu ) + pixel; // = iblu + pixel
          } // if (chan == 0) else
        } // if (tru == 0  && chan == 0) else

        if ( ky < dev->height && kx < dev->width )
          SetPixel( hdc, kx, dev->height - 1 - ky, curcolor );
      } // for() inner (indent error)
    } // for() outer
  }
  return true;
}

  void DeviceWIN::EventHandler()
  {
    int wLSize = winList.size();
    for( int i=0; i<wLSize; i++) {
      if( winList[ i] != NULL)	winList[ i]->EventHandler();
    }
    TidyWindowsList();
  }

  bool DeviceWIN::WDelete( int wIx)
  {
	TidyWindowsList();

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

  bool DeviceWIN::WOpen( int wIx, const std::string& title, 
	      int xSize, int ySize, int xPos, int yPos)
  {

    int debug=0;
#ifdef GDL_DEBUG
     debug=1;
#endif
    if(debug) cout << "WOpen : " << xSize <<" "<< ySize<<" "<< xPos<<" "<< yPos;
    TidyWindowsList();

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

    winList[ wIx] = new GDLWINStream( nx, ny);
    
    // as wxwidgets never set this, they can be intermixed
    oList[ wIx]   = oIx++;

    // set initial window size
    PLFLT xp; PLFLT yp; 
    PLINT xleng; PLINT yleng;
    PLINT xoff; PLINT yoff;
    if(debug) cout << "WOpen: ->plstream:gpage(";
    winList[ wIx]->plstream::gpage( xp, yp, xleng, yleng, xoff, yoff);
// <<<< alternate block in devicex.hpp
		xleng = xSize;
		yleng = ySize;
		xoff = xPos;
		yoff = yPos;
// >>>>
    if(debug) cout << " )WOpen: ->spage(";
    winList[ wIx]->spage( xp, yp, xleng, yleng, xoff, yoff);

    if(debug) cout << " )\n WOpen: ->spause(";

    // no pause on win destruction
    winList[ wIx]->spause( false);
    if(debug) cout << " ) WOpen: ->fontld(";

    // extended fonts
    winList[ wIx]->fontld( 1);
    if(debug) cout << " ) WOpen: ->scolor(";


    // we want color
    winList[ wIx]->scolor( 1);

    // window title
    static char buf[ 256];
    strncpy( buf, title.c_str(), 255);
    buf[ 255] = 0;
   if(debug) cout << " )\n WOpen: ->SETOPT(plwindow,buf) \n";
    winList[ wIx]->SETOPT( "plwindow", buf);

		// we want color (and the driver options need to be overwritten)
		// winList[ wIx]->SETOPT( "drvopt","color=1");
// ---- alternate SETOPT in devicex.hpp
		// set color map
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    //    winList[ wIx]->scmap0( r, g, b, ctSize);
    winList[ wIx]->scmap1( r, g, b, ctSize);

   if(debug) cout << " WOpen:winList[ wIx]->Init(";

    winList[ wIx]->Init();
if(debug) cout << " )\n WOpen:winList[ wIx]->ssub(1,1)" 
               << "adv(0) font(1) vpor(0,1,0,1) wind(0,1,0,1";
			   //----------------------
				//----------------------
   // need to be called initially. permit to fix things
    winList[ wIx]->ssub(1,1);
    winList[ wIx]->adv(0);
    // load font
    winList[ wIx]->font( 1);
    winList[ wIx]->vpor(0,1,0,1);
    winList[ wIx]->wind(0,1,0,1);
   if(debug) cout << " )\n WOpen:winList[ wIx]->DefaultCharsize(";

    winList[ wIx]->DefaultCharSize();
   if(debug) cout << " )\n WOpen:winList[ wIx]->updatePageInfo(";
    //in case these are not initalized, here is a good place to do it.
    if (winList[ wIx]->updatePageInfo()==true)
      {
        winList[ wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

      }
    // sets actWin and updates !D
    SetActWin( wIx);
   if(debug) cout << " WOpen return true;} \n ";

    return true; //winList[ wIx]->Valid(); // Valid() need to called once
  }
//* Missing: WState( int wIx), WSize( int wIx, ...), WShow
  bool DeviceWIN::WState( int wIx)
  { 
    return wIx >= 0 && wIx < oList.size() && oList[ wIx] != 0;
  }

	bool DeviceWIN::WSet(int wIx)
	{
		TidyWindowsList();

		int wLSize = winList.size();
		if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
			return false;

		SetActWin(wIx);
		return true;
	}

  int DeviceWIN::WAdd()
  {
    TidyWindowsList();

    int wLSize = winList.size();
    for( int i=0; i<wLSize; i++)
      if( winList[ i] == NULL) return i;

    // plplot allows only 101 windows
    if( wLSize == 101) return -1;

    winList.push_back( NULL);
    oList.push_back( 0);
    return wLSize;
  }
