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

static const PLFLT Z_DPI = 96 ; //in dpi;

//#include "dstructgdl.hpp"
#include "gdlzstream.hpp"

class DeviceZ: public GraphicsDevice
{
  GDLZStream*     actStream;

  char*  memBuffer;
  DInt*  zBuffer;
  int    decomposed;
  DString fontname;
    
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
    actStream->SetColorMap0( r, g, b, ctSize); 
    actStream->SetColorMap1( r, g, b, ctSize); 
    
  short myfont = ((int) SysVar::GetPFont()>-1) ? 1 : 0;
  std::string what = "hrshsym=0,text=" + i2s(myfont);
  actStream->setopt("drvopt", what.c_str());
  
   actStream->spage(Z_DPI, Z_DPI, nx, ny, 0, 0 );

   actStream->Init();
    // need to be called initially. permit to fix things
    actStream->plstream::ssub(1, 1); // plstream below stays with ONLY ONE page
    actStream->plstream::adv(0); //-->this one is the 1st and only pladv
    // load font
    actStream->plstream::font(1);
    actStream->plstream::vpor(0, 1, 0, 1);
    actStream->plstream::wind(0, 1, 0, 1);

    actStream->ssub(1, 1);
    actStream->adv(0); //this is for us (counters)
    actStream->DefaultCharSize();
  }

public:
  DeviceZ(): GraphicsDevice(), actStream( NULL), memBuffer( NULL), zBuffer( NULL), decomposed(0)
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
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); //our default is 24bpp 
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
  
  bool SetPixelDepth(DInt value) {
   static int displayed=0;
   if (!displayed) {displayed=1; cerr<<"Pixel Depth changes ignored in GDL, stays at 24."<<endl;}
   //this command should nevertheless reset Zbuffer:
   DLong& actX = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
   DLong& actY = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];
	
   SetZBuffer( actX, actY);  
   return true;
  }
  
  bool Decomposed( bool value)           
  {   
    decomposed = value;
//    if (decomposed==1) (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256*256*256;
//    else (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256;
    return true;
  }
  
    DLong GetDecomposed()        
  {
    return decomposed;  
  }
    
    virtual bool SetFont(DString &f) final {fontname=f; return true;}

};

#endif
