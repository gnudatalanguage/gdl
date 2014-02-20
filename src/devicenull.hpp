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

class DeviceNULL : public GraphicsDevice
{
  //  std::string      fileName;
  //  GDLNULLStream*     actStream;
  // void InitStream()  {  }

public:
  //  DeviceNULL(): GraphicsDevice(), fileName( "gdl.null"), actStream( NULL)
  DeviceNULL(): GraphicsDevice()
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
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 0)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 0)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 1000.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 1000.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 266807)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 
  }
  
  ~DeviceNULL() {}
 
};

#endif
