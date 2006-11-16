/* *************************************************************************
                          io.cpp  -  GDL classes for file io
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
		      bool swapEndian_, bool dOC, bool xdr_, 
		      SizeT width_,
		      bool f77_)
{ 
  f77 = f77_;

//   if( f77_)
//     throw GDLException("F77_UNFORMATTED format not supported.");

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
      throw GDLIOException("Error opening file.");
    }
    
  mode=mode_;
  swapEndian = swapEndian_;
  deleteOnClose = dOC;

  if( xdr_)
    xdrs = new XDR;

  lastSeekPos = 0;
  lastRecord = 0;
  lastRecordStart = 0;
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
  f77=false;
  swapEndian=false;
  deleteOnClose=false;

  delete xdrs;
  xdrs = NULL;

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

void GDLStream::F77Write( DULong tCount)
{
  if( fStream->eof()) fStream->clear();

  assert( sizeof( DULong) == 4);
  if( swapEndian)
    {
      char swapTCount[ sizeof( DULong)];
      for( SizeT i=0; i<sizeof( DULong); ++i)
	swapTCount[i] = 
	  reinterpret_cast<char*>(&tCount)[ sizeof( DULong)-1-i];
      fStream->write(swapTCount,sizeof( DULong));
    }
  else
    {
      fStream->write(reinterpret_cast<char*>(&tCount),sizeof( DULong));
    }

  if( !fStream->good())
    {
      throw GDLIOException("Error writing F77_UNFORMATTED record data.");
    }  
}

DULong GDLStream::F77ReadStart()
{
  if( fStream->eof())
    throw GDLIOException("End of file encountered.");

  assert( sizeof( DULong) == 4);
  DULong tCountRd;
  if( swapEndian)
    {
      char swapTCount[ sizeof( DULong)];
      fStream->read( swapTCount, sizeof( DULong));
      for( SizeT i=0; i<sizeof( DULong); ++i)
	reinterpret_cast<char*>(&tCountRd)[ sizeof( DULong)-1-i] = 
	  swapTCount[i];
    }
  else
    {
      fStream->read(reinterpret_cast<char*>(&tCountRd),sizeof( DULong));
    }

  if( fStream->eof())
    throw GDLIOException("End of file encountered.");

  if( !fStream->good())
    {
      throw GDLIOException("Error reading F77_UNFORMATTED record data.");
    }  

  lastRecord = tCountRd;
  lastRecordStart = Tell();
  return tCountRd;
}

void GDLStream::F77ReadEnd()
{
  if( fStream->eof())
    throw GDLIOException("End of file encountered.");

  SizeT actPos = Tell();
  if( actPos > (lastRecordStart+lastRecord))
    throw GDLIOException( "Read past end of Record of F77_UNFORAMTTED file.");

  if( actPos < (lastRecordStart+lastRecord))
    Seek( lastRecordStart+lastRecord);
  
  DULong tCountRd;
  if( swapEndian)
    {
      char swapTCount[ sizeof( DULong)];
      fStream->read( swapTCount, sizeof( DULong));
      for( SizeT i=0; i<sizeof( DULong); ++i)
	reinterpret_cast<char*>(&tCountRd)[ sizeof( DULong)-1-i] = 
	  swapTCount[i];
    }
  else
    {
      fStream->read(reinterpret_cast<char*>(&tCountRd),sizeof( DULong));
    }

  if( fStream->eof())
    throw GDLIOException("End of file encountered.");

  if( !fStream->good())
    {
      throw GDLIOException("Error reading F77_UNFORMATTED record data.");
    }  

  if( lastRecord != tCountRd)
    throw GDLIOException( "Logical error in F77_UNFORAMTTED file.");
}

