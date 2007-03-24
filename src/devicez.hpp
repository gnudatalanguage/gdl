/* *************************************************************************
                          devicez.hpp  -  postscript device
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

#ifndef DEVICEZ_HPP_
#define DEVICEZ_HPP_

//#include "dstructgdl.hpp"
#include "gdlzstream.hpp"

class DeviceZ: public Graphics
{
  GDLZStream*     actStream;

  char*  memBuffer;
  DInt*  zBuffer;

  void plimage_gdl(unsigned char *idata, PLINT nx, PLINT ny, 
		   PLINT xLL, PLINT yLL, DLong tru, DLong chan)
  {
    DLong xsize = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
    DLong ysize = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];

    PLINT ix, iy; //, xm, ym;

    PLINT ired; //, igrn, iblu;
    for(ix = 0; ix < nx; ++ix) {
      for(iy = 0; iy < ny; ++iy) {

	  if (tru == 0) {
	    ired = idata[iy*nx+ix];
	  } else if (tru == 1) {
	    ired = idata[3*(iy*nx+ix)+0];
	    //	    igrn = idata[3*(iy*nx+ix)+1];
	    //	    iblu = idata[3*(iy*nx+ix)+2];
	    //	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  } else if (tru == 2) {
	    ired = idata[nx*(iy*3+0)+ix];
	    //	    igrn = idata[nx*(iy*3+1)+ix];
	    //	    iblu = idata[nx*(iy*3+2)+ix];
	    //	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  } else if (tru == 3) {
	    ired = idata[nx*(0*ny+iy)+ix];
	    //	    igrn = idata[nx*(1*ny+iy)+ix];
	    //	    iblu = idata[nx*(2*ny+iy)+ix];
	    //	    curcolor.pixel = ired*256*256+igrn*256+iblu;
	  }

	  SizeT baseIx = ((ysize-1-(iy+yLL))*xsize+(ix+xLL)) * 3;
	  memBuffer[ baseIx+0] = ired;
	  memBuffer[ baseIx+1] = ired;
	  memBuffer[ baseIx+2] = ired;
      }
    }
  }

  void SetZBuffer( DLong x, DLong y)
  {
    delete[] zBuffer;
    zBuffer = new DInt[ x*y];
    SizeT n = x*y;
    for( SizeT i=0; i<n; ++i)
      zBuffer[ i] = -32765;
  }

  void DeleteStream()
  {
    // note: the plplot documentation says that the user has to
    //       free this buffer, but plplot does it itself.
    //       even worse: it does it with 'free'
    delete actStream; actStream = NULL;
    // delete[] memBuffer; memBuffer =  NULL;
    memBuffer =  NULL;
  }

  void InitStream()
  {
    DeleteStream();

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    DLong& actX = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
    DLong& actY = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];

    // always allocate the buffer with creating a new stream
    actStream = new GDLZStream( nx, ny);
    // here we allocate the memory
    // plplot frees this with 'free'
    //    memBuffer = new char[ actX * actY * 3];
    //    memBuffer = (char*) malloc( sizeof( char) * actX * actY * 3);
    // plplot mem driver error, lines could be drawn upto actY (rather that
    // actY-1)

    memBuffer = (char*) calloc( sizeof( char), actX * (actY+1) * 3);

    // make it known to plplot
    plsmem( actX, actY, memBuffer);

    // no pause on destruction
    actStream->spause( 0);

    // extended fonts
    actStream->fontld( 1);

    // we want color
    actStream->scolor( 1);

    // *** esc char
    // actStream->sesc( '!');

    // set color map (always grey ramp)
    PLINT r[ctSize], g[ctSize], b[ctSize];
    for( PLINT i = 0; i<ctSize; ++i)
      { r[ i] = g[ i] = b[ i] = i;}
    actStream->scmap1( r, g, b, ctSize); 

    actStream->SetOpt( "drvopt","text=0"); // clear drvopt

    actStream->Init();
    
    // load font
    actStream->font( 1);
    actStream->DefaultCharSize();

    //    (*pMulti)[ 0] = 0;
    actStream->adv(0);
  }

public:
  DeviceZ(): Graphics(), actStream( NULL), memBuffer( NULL), zBuffer( NULL)
  {
    name = "Z";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( 640)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( 480)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( 640)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( 480)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 8)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 12)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 26.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 26.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 1)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 414908)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 
  }
  
  ~DeviceZ()
  {
    delete[] zBuffer;
    DeleteStream();
  }

  GDLGStream* GetStream( bool open=true)
  {
    if( actStream == NULL) 
      {
	if( !open) return NULL;
	InitStream();
      }
    return actStream;
  }

  bool CloseFile() // CLOSE keyword
  {
    delete[] zBuffer; zBuffer = NULL;
    DeleteStream();
    return true;
  }

  bool ZBuffering( bool yes)
  {
    if( !yes)
      {
	delete[] zBuffer; zBuffer = NULL;
      }
    else if( zBuffer == NULL)
      {
	DLong& actX = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
	DLong& actY = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];
	
	SetZBuffer( actX, actY);
      }
    return true;
  }
  
  bool SetResolution( DLong nx, DLong ny)
  {
    DLong& actX = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
    DLong& actY = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];

    if( nx == actX && ny == actY) return true;

    DeleteStream();

    if( zBuffer != NULL)
      {
	SetZBuffer( nx, ny);
      }

    actX = nx;
    actY = ny;

    DLong& actXV = (*static_cast<DLongGDL*>( dStruct->Get( xVSTag, 0)))[0];
    DLong& actYV = (*static_cast<DLongGDL*>( dStruct->Get( yVSTag, 0)))[0];

    actXV = nx;
    actYV = ny;

    return true;
  }

  void ClearStream( DLong bColor)
  {
    DLong& actX = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
    DLong& actY = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];

    for( SizeT i=0; i<actX * (actY+1) * 3; ++i)
      memBuffer[i] = bColor;
  }


  BaseGDL* TVRD( EnvT* e)
  {
    DLong xsize = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
    DLong ysize = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];

    if( memBuffer == NULL)
      {
	return new DByteGDL( dimension( xsize, ysize));
      }
      
    DByteGDL* res = new DByteGDL( dimension( xsize, ysize), 
				  BaseGDL::NOZERO);
    
    for( SizeT x=0; x<xsize; ++x)
      for( SizeT y=0; y<ysize; ++y)
	(*res)[ y*xsize+x] = memBuffer[ ((ysize-y-1)*xsize+x) * 3];
    
    return res;
  }

  void TV( EnvT* e)
  {
    //    Graphics* actDevice = Graphics::GetDevice();
    SizeT nParam=e->NParam( 1); 

    GDLGStream* actStream = /* actDevice-> */ GetStream();

    //    actStream->NextPlot( false); // JMG

    DLong xsize = (*static_cast<DLongGDL*>( dStruct->Get( xSTag, 0)))[0];
    DLong ysize = (*static_cast<DLongGDL*>( dStruct->Get( ySTag, 0)))[0];

    DLong xLL=0, yLL=0, pos=0;
    if (nParam == 2) {
      e->AssureLongScalarPar( 1, pos);
    } else if (nParam >= 3) {
      DDouble xLLf, yLLf;
      if (e->KeywordSet(1)) { // NORMAL
	e->AssureDoubleScalarPar( 1, xLLf);
	e->AssureDoubleScalarPar( 2, yLLf);
	xLL = (DLong) rint(xLLf * xsize);
	yLL = (DLong) rint(yLLf * ysize);
      } else {
	e->AssureLongScalarPar( 1, xLL);
	e->AssureLongScalarPar( 2, yLL);
      }
    }

    //    actStream->vpor( 0, 1.0, 0, 1.0);
    // actStream->wind( 1-xLL, xsize-xLL, 1-yLL, ysize-yLL);

    DByteGDL* p0B = e->GetParAs<DByteGDL>( 0);
    SizeT rank = p0B->Rank();

    int width, height;
    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);
    if (rank == 2) 
      {
	if (tru != 0)
	  e->Throw( "Array must have 3 dimensions: "+
		    e->GetParString(0));
	width  = p0B->Dim(0);
	height = p0B->Dim(1);
      } 
    else if( rank == 3) 
      {
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
	  e->Throw( "TRUE must be between 1 and 3");
	}
      } else {
	e->Throw( "Image array must have rank 2 or 3");
      }

    if( width + xLL > xsize || height + yLL > ysize)
      e->Throw( "Value of image coordinates is out of allowed range.");

    DLong chan = 0;
    plimage_gdl(&(*p0B)[0], width, height, xLL, yLL, tru, chan);
  }

};

#endif
