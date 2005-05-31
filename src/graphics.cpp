/* *************************************************************************
                          graphics.cpp  -  GDL base class for all 
			  graphic devices
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

#include <cstdlib>

#include "objects.hpp"
#include "graphics.hpp"
#ifdef _MSC_VER
#include "devicewin.hpp"
#else
#include "devicex.hpp"
#endif
#include "deviceps.hpp"
#include "devicez.hpp"
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

std::vector<GDLCT> Graphics::CT; // predefined colortables
GDLCT              Graphics::actCT; // actual used colortable

DeviceListT  Graphics::deviceList;
Graphics*    Graphics::actDevice   = NULL;
int Graphics::wTag;
int Graphics::xSTag;
int Graphics::ySTag;
int Graphics::xVSTag;
int Graphics::yVSTag;

Graphics::~Graphics() 
{
  // actDevice's dStruct is or will be deleted from sysVarList
  if( actDevice != this) delete dStruct;
} // v-table instatiation

Graphics::Graphics(): dStruct( NULL)
{
}

bool Graphics::SetDevice( const string& device)
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

void Graphics::Init()
{
  InitCT();

  DefineDStructDesc();

#ifdef _MSC_VER
  deviceList.push_back( new DeviceWIN());
#else
  deviceList.push_back( new DeviceX());
#endif
  deviceList.push_back( new DevicePS());
  deviceList.push_back( new DeviceZ());

#ifdef _MSC_VER
  if( !SetDevice( "WIN")) 
#else
  if( !SetDevice( "X")) 
#endif
    {
    cerr << "Error initializing graphics." << endl;
    exit( EXIT_FAILURE);
    }
}

void Graphics::DestroyDevices()
{
  Purge( deviceList);
  actDevice = NULL;
}

void Graphics::DefineDStructDesc()
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
}

void Graphics::HandleEvents()
{
  DeviceListT::iterator i;
  for( i=deviceList.begin(); i != deviceList.end(); ++i)
    {
      (*i)->EventHandler();
    }
}


void Graphics::LoadCT( UInt iCT)
{

  actCT = CT[iCT];
}
