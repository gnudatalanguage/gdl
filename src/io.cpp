/* *************************************************************************
                          io.cpp  -  GDL classes for file io
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cstdio> // std::remove(...)

#include "objects.hpp"
#include "io.hpp"

using namespace std;

const string StreamInfo( ios* searchStream)
{
  if( searchStream == &cin) return "Unit: 0, <stdin>";
  if( searchStream == &cout) return "Unit: -1, <stdout>";
  if( searchStream == &cerr) return "Unit: -2, <stderr>";
  for( SizeT i=0; i<fileUnits.size(); i++)
    {
      if( fileUnits[ i].fStream == searchStream)
	{
	  return "Unit: "+i2s(i+1)+", File: "+fileUnits[ i].Name();
	}
    }
  return "Internal error: Stream not found.";
}

void GDLStream::Open( const string& name_,
		     ios_base::openmode mode_,
		     bool swapEndian_, bool dOC, SizeT width_)
{ 
  if( fStream == NULL)
    fStream = new fstream();
  else if( fStream->is_open())
    throw GDLException("File unit is already open.");
    
  name=name_;

  fStream->open( name_.c_str(), mode_);

  if( fStream->fail())
    {
      delete fStream;
      fStream = NULL;
      throw GDLException("Error opening file.");
    }
    
  mode=mode_;
  swapEndian = swapEndian_;
  deleteOnClose = dOC;
  lastSeekPos = 0;
  width = width_;
}

void GDLStream::Close() 
{ 
  if( fStream != NULL)
    {
      fStream->close();
      fStream->clear();
      if( deleteOnClose) 
	std::remove(name.c_str());
    }
  name="";
  swapEndian=false;
  deleteOnClose=false;
  width = defaultStreamWidth;
}

void GDLStream::Free()
{ 
  Close();
  if( fStream != NULL) 
    {
      delete fStream;
      fStream = NULL;
    }
}
 
fstream& GDLStream::IStream()
{
  if( fStream == NULL || !fStream->is_open()) 
    throw GDLException("File unit is not open.");
  if( !(mode & ios::in))
    throw GDLException("File unit is not open for reading.");
  return *fStream;
}

fstream& GDLStream::OStream()
{
  if( fStream == NULL || !fStream->is_open()) 
    throw GDLException("File unit is not open.");
  if( !(mode & ios::out))
    throw GDLException("File unit is not open for writing.");
  return *fStream;
}

void GDLStream::Pad( SizeT nBytes)
{
  const SizeT bufSize = 1024;
  static char buf[ bufSize];
  SizeT nBuf = nBytes / bufSize;
  SizeT lastBytes = nBytes % bufSize;
  for( SizeT i=0; i<nBuf; i++) fStream->write( buf, bufSize);
  if( lastBytes > 0) fStream->write( buf, lastBytes);
}
