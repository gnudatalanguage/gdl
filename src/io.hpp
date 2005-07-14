/* *************************************************************************
                          io.hpp  -  GDL classes for file io
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

#ifndef IO_HPP_
#define IO_HPP_


#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

#include "gdlexception.hpp"

// the file IO system consists of 128 GDLStream objects

//using namespace std;

const int maxLun=128;    // within GDL, internal max LUN is 127
const int maxUserLun=99; // within GDL, internal always lun-1 is used

const std::string StreamInfo( std::ios* searchStream); 
 
const SizeT defaultStreamWidth = 80; // used by open_lun

class GDLStream
{
  std::string name;
  std::ios_base::openmode mode;

  std::fstream* fStream;

  bool swapEndian;
  bool deleteOnClose;

  SizeT width;

  SizeT lastSeekPos;

  SizeT lastRecord;
  
  void Pad( SizeT nBytes); // puts out nBytes zero bytes

public:
  GDLStream(): 
    name(), 
    mode(), 
    fStream(NULL), 
    swapEndian(false),
    deleteOnClose(false),
    width( defaultStreamWidth),
    lastSeekPos( 0)
  {}

  ~GDLStream() 
  {
    delete fStream;
  }  

  void Open( const std::string& name_,
	     std::ios_base::openmode,
	     bool swapEndian_, bool deleteOnClose_, SizeT width);
  
  void Close(); 

  bool Eof()
  {
    if( fStream == NULL)
      throw GDLException("File unit is not open.");
    
    fStream->clear(); // clear old EOF	

    fStream->peek(); // trigger EOF if at EOF

    return fStream->eof();
  }

  void Seek( SizeT pos)
  {
    if( fStream == NULL)
      throw GDLException("File unit is not open.");

    fStream->rdbuf()->pubseekpos( pos, std::ios_base::in | std::ios_base::out);
  
    lastSeekPos = pos;
  }

  SizeT Size()
  {
    SizeT cur = Tell();
    SizeT end = fStream->rdbuf()->pubseekoff( 0, std::ios_base::end);
    fStream->rdbuf()->pubseekpos( cur, std::ios_base::in | std::ios_base::out);
    return end;
  }

  SizeT Tell()
  {
    return( fStream->tellg());
  }

  SizeT Width()
  {
    return width;
  }

  void SeekPad( SizeT pos)
  {
    if( fStream == NULL)
      throw GDLException("File unit is not open.");

    SizeT fSize = Size();
    if( pos > fSize) Pad( pos - fSize);

    fStream->rdbuf()->pubseekpos( pos, std::ios_base::in | std::ios_base::out);
  
    lastSeekPos = pos;
  }

  bool InUse() { return (fStream != NULL);}

  bool IsOpen()
  { return (fStream != NULL && fStream->is_open());} 
  bool IsReadable()
  { return (IsOpen() && (mode & std::ios::in));} 
  bool IsWriteable()
  { return (IsOpen() && (mode & std::ios::out));} 
  
  void Free(); 

  const std::string& Name() { return name;}
  
  bool SwapEndian() { return swapEndian;}

  std::fstream& IStream(); 
  std::fstream& OStream(); 

  friend const std::string StreamInfo( std::ios* searchStream);
};


typedef std::vector<GDLStream> GDLFileListT;

#endif
