/* *************************************************************************
                          devicez.hpp  -  Z-buffer device
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

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

#if defined (_MSC_VER) && (_MSC_VER < 1800)
/* replacement of Unix rint() for Windows */
static int rint (double x)
{
char *buf;
int i,dec,sig;

buf = _fcvt(x, 0, &dec, &sig);
i = atoi(buf);
if(sig == 1) {
i = i * -1;
}
return(i);
}
#endif

class DeviceZ: public GraphicsDevice
{
  GDLZStream*     actStream;

  char*  memBuffer;
  DInt*  zBuffer;
  
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

    DLong& actX = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
    DLong& actY = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];

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
    actStream->spause( false);

    // extended fonts
    actStream->fontld( 1);

    // we want color
    actStream->scolor( 1);

    // set color map (always grey ramp)
    PLINT r[ctSize], g[ctSize], b[ctSize];
    for( PLINT i = 0; i<ctSize; ++i)
      { r[ i] = g[ i] = b[ i] = i;}
    actStream->SetColorMap1( r, g, b, ctSize); 

    actStream->SETOPT( "drvopt","text=0"); // clear drvopt

    actStream->Init();
   // need to be called initially. permit to fix things
    actStream->ssub(1,1);
    actStream->adv(0);
    // load font
    actStream->font( 1);
    actStream->vpor(0,1,0,1);
    actStream->wind(0,1,0,1);
    actStream->DefaultCharSize();
   //in case these are not initalized, here is a good place to do it.
    if (actStream->updatePageInfo()==true)
    {
        actStream->GetPlplotDefaultCharSize(); //initializes everything in fact..

    }
  }

public:
  DeviceZ(): GraphicsDevice(), actStream( NULL), memBuffer( NULL), zBuffer( NULL)
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
	DLong& actX = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
	DLong& actY = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];
	
	SetZBuffer( actX, actY);
      }
    return true;
  }
  
  bool SetResolution( DLong nx, DLong ny)
  {
    DLong& actX = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
    DLong& actY = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];

    if( nx == actX && ny == actY) return true;

    DeleteStream();

    if( zBuffer != NULL)
      {
	SetZBuffer( nx, ny);
      }

    actX = nx;
    actY = ny;

    DLong& actXV = (*static_cast<DLongGDL*>( dStruct->GetTag( xVSTag, 0)))[0];
    DLong& actYV = (*static_cast<DLongGDL*>( dStruct->GetTag( yVSTag, 0)))[0];

    actXV = nx;
    actYV = ny;

    return true;
  }

  void ClearStream( DLong bColor)
  {
    DLong& actX = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
    DLong& actY = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];

    for( SizeT i=0; i<actX * (actY+1) * 3; ++i)
      memBuffer[i] = bColor;
  }

  DLong GetPixelDepth() { return 24;}  
};

#endif
