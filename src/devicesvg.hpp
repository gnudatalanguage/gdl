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

class DeviceSVG : public Graphics
{
  std::string      fileName;
  GDLSVGStream*     actStream;

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

    // set color map
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    //    actStream->scmap0( r, g, b, ctSize); 
    actStream->scmap1( r, g, b, ctSize); 

    actStream->SETOPT( "drvopt","text_clipping=1"); // clear drvopt

    actStream->Init();
    
    // load font
    actStream->font( 1);
    actStream->DefaultCharSize();

    //    (*pMulti)[ 0] = 0;
    actStream->adv(0);
  }

public:
  DeviceSVG(): Graphics(), fileName( "gdl.svg"), actStream( NULL)
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
};

#endif
