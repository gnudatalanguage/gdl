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
    actStream->scmap1( r, g, b, ctSize); 

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
  
#define PAD 3
  BaseGDL* TVRD( EnvT* e)
  {
    DLong xMaxSize = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
    DLong yMaxSize = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];

    if (e->KeywordSet("WORDS")) e->Throw( "WORDS keyword not yet supported.");
    DLong orderVal=SysVar::TV_ORDER();
    e->AssureLongScalarKWIfPresent( "ORDER", orderVal);
    SizeT dims[3];
    
    DByteGDL* res;

    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);
    if (tru > 3 || tru < 0) e->Throw("Value of TRUE keyword is out of allowed range.");

    DLong channel=-1;

    unsigned int x_gdl=0;
    unsigned int y_gdl=0;
    unsigned int nx_gdl=xMaxSize;
    unsigned int ny_gdl=yMaxSize;

    bool error=false;
    bool hasXsize=false;
    bool hasYsize=false;
    int nParam = e->NParam();
    if (nParam >= 4) {
      DLongGDL* Ny = e->GetParAs<DLongGDL>(3);
      ny_gdl=(*Ny)[0];
      hasYsize=true;
    }
    if (nParam >= 3) {
      DLongGDL* Nx = e->GetParAs<DLongGDL>(2);
      nx_gdl=(*Nx)[0];
      hasXsize=true;
    }
    if (nParam >= 2) {
      DLongGDL* y0 = e->GetParAs<DLongGDL>(1);
      y_gdl=(*y0)[0];
    }
    if (nParam >= 1) {
      DLongGDL* x0 = e->GetParAs<DLongGDL>(0);
      x_gdl=(*x0)[0];
    }
    if (nParam == 5) {
      DLongGDL* ChannelGdl = e->GetParAs<DLongGDL>(4);
      channel=(*ChannelGdl)[0]; 
    }
    e->AssureLongScalarKWIfPresent( "CHANNEL", channel);
    if (channel > 3) e->Throw("Value of Channel is out of allowed range.");

    bool debug=false;
    if (debug) {
      cout << x_gdl <<" "<< y_gdl <<" "<< nx_gdl <<" "<< ny_gdl <<" "<< channel <<endl;
    }
    if (!(hasXsize))nx_gdl-=x_gdl; 
    if (!(hasYsize))ny_gdl-=y_gdl;
    
    DLong xref,xval,xinc,yref,yval,yinc,xmax11,ymin11;
    int x_11=0;
    int y_11=0;
    xref=0;xval=0;xinc=1;
    yref=yMaxSize-1;yval=0;yinc=-1;
    
    x_11=xval+(x_gdl-xref)*xinc;
    y_11=yval+(y_gdl-yref)*yinc;
    xmax11=xval+(x_gdl+nx_gdl-1-xref)*xinc;    
    ymin11=yval+(y_gdl+ny_gdl-1-yref)*yinc;
    if (debug) {
      cout <<"["<< x_11 <<","<< xmax11 <<"],["<< ymin11 <<","<< y_11 <<"]"<<endl;
    }   
    if (y_11 < 0 || y_11 > yMaxSize-1) error=true;
    if (x_11 < 0 || x_11 > xMaxSize-1) error=true;
    if (xmax11 < 0 || xmax11 > xMaxSize-1) error=true;
    if (ymin11 < 0 || ymin11 > yMaxSize-1) error=true;
    if (error) e->Throw("Value of Area is out of allowed range.");

    if (tru == 0) {
      dims[0] = nx_gdl;
      dims[1] = ny_gdl;
      dimension dim(dims, (SizeT) 2);
      res = new DByteGDL( dim, BaseGDL::ZERO);

      if (memBuffer == NULL) return res;

      if (channel <= 0) { //channel not given, return max of the 3 channels
	DByte mx, mx1;
	for (SizeT i = 0; i < dims[0] * dims[1]; ++i) {
	  mx = (DByte) memBuffer[PAD * i];
	  mx1 = (DByte) memBuffer[PAD * i + 1];
	  if (mx1 > mx) mx = mx1;
	  mx1 = (DByte) memBuffer[PAD * i + 2];
	  if (mx1 > mx) mx = mx1;
	  (*res)[i] = mx;
	}
      } else {
	for (SizeT i = 0; i < dims[0] * dims[1]; ++i) {
	  (*res)[i] = memBuffer[PAD * i + channel]; //0=R,1:G,2:B,3
	}
      }
      // Reflect about y-axis
      if (orderVal == 0) res->Reverse(1);
      return res;

    } else {
      dims[0] = 3;
      dims[1] = nx_gdl;
      dims[2] = ny_gdl;
      dimension dim(dims, (SizeT) 3);
      res = new DByteGDL(dim, BaseGDL::NOZERO);
      if (memBuffer == NULL) return res; //new DByteGDL(dim, BaseGDL::ZERO);

      for (SizeT i = 0, kpad=0; i < dims[1] * dims[2]; ++i)
        {
	  for(SizeT j=0; j<PAD; ++j) (*res)[(i+1)*PAD-(j+1)] = memBuffer[kpad++]; 
	  kpad++;
        } 

      // Reflect about y-axis
      if (orderVal == 0) res->Reverse(2);

      DUInt* perm = new DUInt[3];
      if (tru == 1) {
	return res;
      } else if (tru == 2) {
	perm[0] = 1;
	perm[1] = 0;
	perm[2] = 2;
	return res->Transpose(perm);
      } else if (tru == 3) {
	perm[0] = 1;
	perm[1] = 2;
	perm[2] = 0;
	return res->Transpose(perm);
      }
    }
    assert( false);
    return NULL;
  }
#undef PAD
};

#endif
