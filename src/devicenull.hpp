/* *************************************************************************
                          devicenull.hpp  -  NULL device
                             -------------------
    begin                : 20 February 2014
    copyright            : (C) 2012 by Alain Coulais
    email                : alaingdl@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEVICENULL_HPP_
#define DEVICENULL_HPP_
#include "gdlnullstream.hpp"
class DeviceNULL : public GraphicsDevice
{
   GDLNULLStream*   actStream;
  void DeleteStream()
  {
    delete actStream; actStream = NULL;
  }

  void InitStream()
  {
    DeleteStream();

    // always allocate the buffer with creating a new stream
    actStream = new GDLNULLStream( 100, 100);
    actStream->Init();
    // need to be called initially. permit to fix things
    actStream->plstream::ssub(1, 1); // plstream below stays with ONLY ONE page
    actStream->plstream::adv(0); //-->this one is the 1st and only pladv
    // load font
    actStream->plstream::font(1);
    actStream->plstream::vpor(0, 1, 0, 1);
    actStream->plstream::wind(0, 1, 0, 1);

    actStream->ssub(1, 1);
    actStream->SetPageDPMM();
    actStream->DefaultCharSize();
    actStream->adv(0); //this is for us (counters) //needs DefaultCharSize
  }
  
  GDLGStream* GetStream( bool open=true)
  {
    if( actStream == NULL) 
      {
	InitStream();
      }
    return actStream;
  }
  
public:
  //  DeviceNULL(): GraphicsDevice(), fileName( "gdl.null"), actStream( NULL)
  DeviceNULL(): GraphicsDevice(), actStream( NULL)
  {
    name = "NULL";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( 1000)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( 1000)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( 1000)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( 1000)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 8)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 13)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 13.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 13.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 1)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 16)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 
  }
  ~DeviceNULL()
  {
    DeleteStream();
  }

 
};

#endif
