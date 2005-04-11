/* *************************************************************************
                          deviceps.hpp  -  X windows device
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

#ifndef DEVICEPS_HPP_
#define DEVICEPS_HPP_

//#include "dstructgdl.hpp"
#include "gdlpsstream.hpp"

class DevicePS: public Graphics
{
  std::string      fileName;
  GDLPSStream*     actStream;

  void InitStream()
  {
    delete actStream;

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    actStream = new GDLPSStream( nx, ny);

    actStream->sfnam( fileName.c_str());

//     // set initial window size
//     PLFLT xp; PLFLT yp; 
//     PLINT xleng; PLINT yleng;
//     PLINT xoff; PLINT yoff;
//     actStream->gpage( xp, yp, xleng, yleng, xoff, yoff);

//     xleng = xSize;
//     yleng = ySize;
//     xoff  = xPos;
//     yoff  = yPos;

//     actStream->spage( xp, yp, xleng, yleng, xoff, yoff);

    // no pause on destruction
    actStream->spause( 0);

    // extended fonts
    actStream->fontld( 1);

    // we want color
    actStream->scolor( 1);

    // *** esc char
    // actStream->sesc( '!');

    // set color map
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    //    actStream->scmap0( r, g, b, ctSize); 
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
  DevicePS(): Graphics(), fileName( "gdl.ps"), actStream( NULL)
  {
    name = "PS";

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
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 0)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 0)); 
    dStruct->InitTag("ORIGIN",     DLongGDL( 0)); 
    dStruct->InitTag("ZOOM",       DLongGDL( 0)); 
  }
  
  ~DevicePS()
  {
    delete actStream;
  }

  GDLGStream* GetStream()
  {
    if( actStream == NULL) InitStream();
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
