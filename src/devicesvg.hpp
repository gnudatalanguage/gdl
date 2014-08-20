/* *************************************************************************
                          devicesvg.hpp  -  SVG device
                             -------------------
    begin                : December 26 2008
    copyright            : (C) 2008 by Sylwester Arabas
    email                : slayoo@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEVICESVG_HPP_
#define DEVICESVG_HPP_

#include "gdlsvgstream.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

class DeviceSVG : public GraphicsDevice
{
  std::string      fileName;
  GDLSVGStream*     actStream;
  int              decomposed; // false -> use color table

  void InitStream()
  {
    delete actStream;

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    actStream = new GDLSVGStream( nx, ny);

    actStream->sfnam( fileName.c_str());

    // no pause on destruction
    actStream->spause( false);

    // extended fonts
    actStream->fontld( 1);

    // we want color
    actStream->scolor( 1);

    // avoid to set color map 0 -- makes plplot very slow (?)
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
//    actStream->scmap0( r, g, b, ctSize);
    actStream->scmap1( r, g, b, ctSize);
    actStream->scolbg(255,255,255); // start with a white background

    actStream->SETOPT( "drvopt","text_clipping=1"); // clear drvopt

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
  DeviceSVG(): GraphicsDevice(), fileName( "gdl.svg"), actStream( NULL), decomposed(0)
  {
    name = "SVG";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( 17780)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( 12700)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( 640)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( 512)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 0)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 0)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 1000.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 1000.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 16777216)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 266807)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 
  }
  
  ~DeviceSVG()
  {
    delete actStream;
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

  bool SetFileName( const std::string& f)
  {
    fileName = f;
    return true;
  }

  bool CloseFile()
  {
    delete actStream;
    actStream = NULL;
    return true;
  }
  
  bool Decomposed( bool value)           
  {   
    decomposed = value;
    if (decomposed) (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256*256*256;
    else (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256;
    return true;
  }

  DLong GetDecomposed()        
  {
    return decomposed;  
  }

};

#endif
