/* *************************************************************************
     graphicsdevice.cpp  -  GDL base class for all graphic devices
renamed from: graphics.cpp                          
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

#include <cstdlib>

#include "objects.hpp"
#include "graphicsdevice.hpp"
#if defined(_WIN32) && !defined(__CYGWIN__) // does cygwin have its own X11 port?
#  include "devicewin.hpp"
#else
#  include "devicex.hpp"
#endif
#include "deviceps.hpp"
#include "devicesvg.hpp"
#include "devicez.hpp"
#include "devicenull.hpp"
#include "initsysvar.hpp"
#include "color.hpp"

using namespace std;

bool GDLCT::Get( PLINT r_[], PLINT g_[], PLINT b_[], UInt nCol) const
{
  if( nCol > ctSize)
    return false;

  for(UInt i=0; i<nCol; ++i)
    {
      r_[i] = static_cast<PLINT>(r[i]);
      g_[i] = static_cast<PLINT>(g[i]);
      b_[i] = static_cast<PLINT>(b[i]);
    }

  return true;
}

bool GDLCT::Get( UInt ix, DByte& r_, DByte& g_, DByte& b_) const
{
  if( ix >= ctSize)
    return false;
  r_ = r[ix];
  g_ = g[ix];
  b_ = b[ix];
  return true;
}

bool GDLCT::Set( UInt ix, DByte r_, DByte g_, DByte b_)
{
  if( ix >= ctSize)
    return false;
  r[ix] = r_;
  g[ix] = g_;
  b[ix] = b_;
  return true;
}

bool GDLCT::SetHLS( UInt ix, DFloat h, DFloat l, DFloat s)
{
  if( ix >= ctSize)
    return false;

  DByte r_, g_, b_;
  HLS2RGB( h, l, s, r_, g_, b_);

  r[ix] = r_;
  g[ix] = g_;
  b[ix] = b_;
  return true;
}

bool GDLCT::SetHSV( UInt ix, DFloat h, DFloat s, DFloat v)
{
  if( ix >= ctSize)
    return false;

  DByte r_, g_, b_;
  HSV2RGB( h, s, v, r_, g_, b_);

  r[ix] = r_;
  g[ix] = g_;
  b[ix] = b_;
  return true;
}

std::vector<GDLCT> GraphicsDevice::CT;    // predefined colortables
GDLCT              GraphicsDevice::actCT; // actual used colortable

DeviceListT  GraphicsDevice::deviceList;
GraphicsDevice*    GraphicsDevice::actDevice   = NULL;
GraphicsDevice*    GraphicsDevice::actGUIDevice   = NULL;

int GraphicsDevice::wTag;
int GraphicsDevice::xSTag;
int GraphicsDevice::ySTag;
int GraphicsDevice::xVSTag;
int GraphicsDevice::yVSTag;
int GraphicsDevice::n_colorsTag;

GraphicsDevice::~GraphicsDevice() 
{
  // actDevice's dStruct is or will be deleted from sysVarList
  if( actDevice != this) delete dStruct;
} // v-table instatiation

GraphicsDevice::GraphicsDevice(): dStruct( NULL)
{
}

void GraphicsDevice::ListDevice()
{
  int size = deviceList.size();
  cout << "Available Graphics Devices: ";
  for( int i=0; i<size; i++) cout << deviceList[ i]->Name() << " ";
  cout << endl;
}

bool GraphicsDevice::ExistDevice( const string& device, int &index)
{
  index=-1;
  int size = deviceList.size();
  for( int i=0; i<size; i++)
    {
      if( deviceList[ i]->Name() == device)
	{
	  index=i;
	  return true;
	}
    }
  return false;
}

bool GraphicsDevice::SetDevice( const string& device)
{
  int size = deviceList.size();
  for( int i=0; i<size; i++)
    {
      if( deviceList[ i]->Name() == device)
	{
	  actDevice   = deviceList[ i];

	  // update !D
	  SysVar::SetD( actDevice->DStruct());

	  return true;
	}
    }
  return false;
}

void GraphicsDevice::Init()
{
  InitCT();

  DefineDStructDesc();

  // 4 devices types without surprise !
  deviceList.push_back( new DeviceNULL());
  deviceList.push_back( new DevicePS());
  deviceList.push_back( new DeviceSVG());
  deviceList.push_back( new DeviceZ());

#if defined(_WIN32) && !defined(__CYGWIN__)
  deviceList.push_back( new DeviceWIN());
#else
  #  ifdef HAVE_X
  deviceList.push_back( new DeviceX());
  #  endif
#endif

  // we try to set WIN or X as default 
  // (and NULL if X11 system (Linux, OSX, Sun) but without X11 at compilation)
#if defined(_WIN32) && !defined(__CYGWIN__)
  if( !SetDevice( "WIN")) 
#else
#  ifndef HAVE_X
    if( !SetDevice( "NULL")) 
#  else
    if( !SetDevice( "X")) 
#  endif
#endif
#  ifndef HAVE_X
      {}
#  else
  {
    cerr << "Error initializing graphics." << endl;
    exit( EXIT_FAILURE);
  }
#  endif

#ifdef HAVE_LIBWXWIDGETS

  // some X error message suggested this call
#ifdef HAVE_X
  XInitThreads();
#endif
#endif
  int index=0;
  // setting the GUI dev. (before, X/win was the first but X might be not defined now
  if (ExistDevice( "WIN", index)) {
    actGUIDevice = deviceList[index];
  } else {
    if (ExistDevice( "X", index)) {
      actGUIDevice = deviceList[index];
    } else {
      actGUIDevice = deviceList[0];
    }
  }
}

void GraphicsDevice::DestroyDevices()
{
  PurgeContainer( deviceList);
  actDevice = NULL;
}

void GraphicsDevice::DefineDStructDesc()
{
  DStructDesc* dSysVarDesc = FindInStructList( structList, "!DEVICE");
  if( dSysVarDesc != NULL) return; 

  dSysVarDesc = new DStructDesc( "!DEVICE");

  SpDString aString;
  SpDLong   aLong;
  SpDLong   aLongArr2( dimension(2));
  SpDFloat  aFloat;

  dSysVarDesc->AddTag("NAME",       &aString); 
  dSysVarDesc->AddTag("X_SIZE",     &aLong); 
  dSysVarDesc->AddTag("Y_SIZE",     &aLong); 
  dSysVarDesc->AddTag("X_VSIZE",    &aLong); 
  dSysVarDesc->AddTag("Y_VSIZE",    &aLong); 
  dSysVarDesc->AddTag("X_CH_SIZE",  &aLong); 
  dSysVarDesc->AddTag("Y_CH_SIZE",  &aLong); 
  dSysVarDesc->AddTag("X_PX_CM",    &aFloat); 
  dSysVarDesc->AddTag("Y_PX_CM",    &aFloat); 
  dSysVarDesc->AddTag("N_COLORS",   &aLong); 
  dSysVarDesc->AddTag("TABLE_SIZE", &aLong); 
  dSysVarDesc->AddTag("FILL_DIST",  &aLong); 
  dSysVarDesc->AddTag("WINDOW",     &aLong); 
  dSysVarDesc->AddTag("UNIT",       &aLong); 
  dSysVarDesc->AddTag("FLAGS",      &aLong); 
  dSysVarDesc->AddTag("ORIGIN",     &aLongArr2); 
  dSysVarDesc->AddTag("ZOOM",       &aLongArr2); 
  
  structList.push_back( dSysVarDesc);

  // !D tag indices
  wTag   = dSysVarDesc->TagIndex( "WINDOW");
  xSTag  = dSysVarDesc->TagIndex( "X_SIZE");
  ySTag  = dSysVarDesc->TagIndex( "Y_SIZE");
  xVSTag = dSysVarDesc->TagIndex( "X_VSIZE");
  yVSTag = dSysVarDesc->TagIndex( "Y_VSIZE");
  n_colorsTag = dSysVarDesc->TagIndex( "N_COLORS");
}

void GraphicsDevice::HandleEvents()
{
  DeviceListT::iterator i;
  for( i=deviceList.begin(); i != deviceList.end(); ++i)
    {
      (*i)->EventHandler();
    }
}


void GraphicsDevice::LoadCT( UInt iCT)
{
  actCT = CT[iCT];
}
