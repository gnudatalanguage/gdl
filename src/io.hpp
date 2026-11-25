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

#include "includefirst.hpp"

#include <fstream>
#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <cassert>

#include <sys/types.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <unistd.h>
#endif

#include <string.h>  // for memcpy
#include "gzstream.hpp"

#include "gdlexception.hpp"


// the file IO system consists of 128 GDLStream objects

const int maxLun=128;    // within GDL, internal max LUN is 127
const int maxUserLun=99; // within GDL, internal always lun-1 is used

const std::string StreamInfo( std::ios* searchStream); 
 
const SizeT defaultStreamWidth = 80; // used by open_lun

class AnyStream
{
public:
  enum class FileAccess { Untracked, Initial, Read, Write };
private:
  FileAccess lastFileAccess = FileAccess::Untracked;
  // The fstreams borrow these buffers when connected to a child process.
  std::unique_ptr<std::streambuf> pipeInput, pipeOutput;
  std::unique_ptr<std::fstream> fStream, ofStream;
  std::unique_ptr<igzstream> igzStream;
  std::unique_ptr<ogzstream> ogzStream;

public:
  void Flush() ;
  void Close();
  void OpenAsPipes(const std::string& name_, const std::ios_base::openmode mode_, const int pipeInFd, const int pipeOutFd); 
  void Open(const std::string& name_, std::ios_base::openmode mode_ , bool compress_);
  std::fstream* oFStream(){return ofStream.get();}
  std::fstream* FStream(){return fStream.get();}
  std::fstream& FileFor(FileAccess access);
  igzstream* IgzStream(){return igzStream.get();} // for gzip compressed input
  ogzstream* OgzStream(){return ogzStream.get();} // for gzip compressed output

  void ClearEof();

  void Write( char* buf, std::streamsize nBytes);


  void Read( char* buf, std::streamsize nBytes);
  std::streamsize Gcount();
  bool Good();

  bool EofRaw();
  

 void SeekEof();
  
  bool Eof();
 
  void Seek( std::streampos pos);
  DLong64 Skip( std::streampos pos, bool doThrow=true);
  DLong SkipLines( DLong nlines, bool doThrow);
  std::streampos Size();

  std::streampos Tell();

 void SeekPad(std::streampos pos);

  bool InUse() { return (fStream != NULL || ofStream != NULL || igzStream != NULL || ogzStream != NULL);}
  bool IsOpen()
  { return (fStream != NULL && fStream->is_open()) || (ofStream != NULL && ofStream->is_open()) || (igzStream != NULL && igzStream->rdbuf()->is_open()) || (ogzStream != NULL && ogzStream->rdbuf()->is_open());} 

  void Pad( std::streamsize nBytes);

  void Clear()
  {
  }

};



class GDLStream
{
  bool getLunLock;
   
  std::string name;
  std::ios_base::openmode mode;

  std::unique_ptr<AnyStream> anyStream;

  bool f77; // FORTRAN unformatted
  bool swapEndian;
  bool deleteOnClose;
  bool varlenVMS;
  bool compress;
  std::unique_ptr<XDR> xdrs;

  std::unique_ptr<std::istringstream> iSocketStream;
  int sockNum;
  std::unique_ptr<std::string> recvBuf;
  DDouble c_timeout;
  DDouble r_timeout;
  DDouble w_timeout;

  SizeT width;
  std::streampos lastSeekPos;

  // for F77
  std::streampos lastRecord;
  std::streampos lastRecordStart;
  
  void Pad( std::streamsize nBytes); // puts out nBytes zero bytes

public:
  GDLStream(): 
    getLunLock(false),
    name(), 
    mode(), 
    f77(false),
    swapEndian(false),
    deleteOnClose(false),
    varlenVMS(false),
    compress(false),

    sockNum( -1),
    c_timeout(0.0),
    r_timeout(0.0),
    w_timeout(0.0),

    width( defaultStreamWidth),
    lastSeekPos( 0),
    lastRecord( 0),
    lastRecordStart( 0)
    {
    }
 
  GDLStream(const GDLStream&) = delete;
  GDLStream& operator=(const GDLStream&) = delete;
  ~GDLStream() { Close(); }

  bool CanOpenAsPipes();  
  void OpenAsPipes(const std::string& name, const std::ios_base::openmode mode_, const int pipeInFd, const int pipeOutFd );
  void Open( const std::string& name_,
	     std::ios_base::openmode,
	     bool swapEndian_, bool deleteOnClose_, bool xdr_, 
	     SizeT width, bool f77, bool compress);
  
  void Socket( const std::string& host,
	       DUInt port, bool swapEndian_,
	       DDouble c_timeout, DDouble r_timeout, DDouble w_timeout, SizeT width);

  void Flush(); 

  void Close(); 
  
  bool Eof();
  
  void SeekEof();
  void Seek( std::streampos pos);
  DLong64 Skip( std::streampos pos, bool doThrow=true);
  void Truncate();
  
  DLong SkipLines( DLong nlines, bool doThrow=true);
  DLong64 CopySomeTo( DLong64 pos, GDLStream& to, bool doThrow=true);
  DLong CopySomeLinesTo( DLong nlines, GDLStream& to, bool doThrow=true);
  
  std::streampos Size()
  {
    return anyStream->Size();
  }

  std::streampos Tell()
  {
    return anyStream->Tell();
  }

  SizeT Width()
  {
    return width;
  }

  void SeekPad( std::streampos pos);
  
  bool InUse() { return sockNum != -1 || (anyStream != NULL && anyStream->InUse()); }

  bool IsOpen()
  { return (anyStream != NULL && anyStream->IsOpen());} 
  bool IsReadable()
  { return (IsOpen() && (mode & std::ios::in));} 
  bool IsWriteable()
  { return (IsOpen() && (mode & std::ios::out));} 
  
  void Free(); 

  void SetGetLunLock( bool b) { getLunLock = b;}
  bool GetGetLunLock() { return getLunLock;}

  const std::string& Name() { return name;}
  
  bool SwapEndian() { return swapEndian;}

  bool VarLenVMS() { return varlenVMS;}
  void PutVarLenVMS( bool varlenVMS_) { varlenVMS = varlenVMS_;}

  bool Compress() { return compress;}
  /*  void PutCompress( bool compress_) { compress = compress_;}*/
  igzstream& IgzStream(); 
  ogzstream& OgzStream(); 

  XDR *Xdr() { return xdrs.get();}

  std::fstream& IStream(); 
  std::fstream& OStream(); 

  int SockNum() { return sockNum;}
  std::istringstream& ISocketStream(); 
  std::string& RecvBuf() { return *recvBuf;}
  DDouble cTimeout() { return c_timeout;}
  DDouble rTimeout() { return r_timeout;}
  DDouble wTimeout() { return w_timeout;}

//   friend const std::string StreamInfo( AnyStream* searchStream);
  friend const std::string StreamInfo( std::ios* searchStream);

  // F77_UNFORMATTED stuff
  bool F77() { return f77;}
  void F77Write( DULong tCount);

  DULong F77ReadStart();
  void   F77ReadEnd();
};


using GDLFileListT = std::vector<GDLStream>;

#endif
