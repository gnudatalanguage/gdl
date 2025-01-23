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

// GGH ggh hack to implement SPAWN keyword UNIT
#ifdef HAVE_EXT_STDIO_FILEBUF_H
#  include <ext/stdio_filebuf.h>
#endif
//#include <bits/basic_ios.h>


// the file IO system consists of 128 GDLStream objects

const int maxLun=128;    // within GDL, internal max LUN is 127
const int maxUserLun=99; // within GDL, internal always lun-1 is used

const std::string StreamInfo( std::ios* searchStream); 
 
const SizeT defaultStreamWidth = 80; // used by open_lun

class AnyStream
{
// GGH ggh made all these public
public:
  std::fstream* ifStream;
  std::fstream* ofStream;
  igzstream* igzStream; // for gzip compressed input
  ogzstream* ogzStream; // for gzip compressed output
  std::basic_streambuf<char> *old_rdbuf_in;
  std::basic_streambuf<char> *old_rdbuf_out;
 //public:
  AnyStream()
    : ifStream(NULL) 
    , ofStream(NULL) 
    , igzStream(NULL) 
    , ogzStream(NULL)
    , old_rdbuf_in(NULL)
    , old_rdbuf_out(NULL)
  {}

  void Flush() ;
  void Close();
#ifdef HAVE_EXT_STDIO_FILEBUF_H
  void Open(const std::string& name_,
	    std::ios_base::openmode mode_ , bool compress_, __gnu_cxx::stdio_filebuf<char> *in=NULL, __gnu_cxx::stdio_filebuf<char> *out=NULL);
#else
void Open(const std::string& name_,
	    std::ios_base::openmode mode_ , bool compress_);
#endif  
  std::fstream* iFStream(){return ifStream;}
  std::fstream* oFStream(){return ofStream;}
  std::fstream* FStream(){return ifStream;}
  igzstream* IgzStream(){return igzStream;} // for gzip compressed input
  ogzstream* OgzStream(){return ogzStream;} // for gzip compressed output

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

  bool InUse() { return (ifStream != NULL || ofStream != NULL || igzStream != NULL || ogzStream != NULL);}
  bool IsOpen()
  { return (ifStream != NULL && ifStream->is_open()) || (ofStream != NULL && ofStream->is_open()) || (igzStream != NULL && igzStream->rdbuf()->is_open()) || (ogzStream != NULL && ogzStream->rdbuf()->is_open());} 

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

  AnyStream* anyStream;

  //    std::fstream* fStream;
  //    igzstream* igzStream; // for gzip compressed input
  //    ogzstream* ogzStream; // for gzip compressed output


  bool f77; // FORTRAN unformatted
  bool swapEndian;
  bool deleteOnClose;
  bool varlenVMS;
  bool compress;
  XDR *xdrs;

  std::istringstream* iSocketStream;
  int sockNum;
  std::string* recvBuf;
  DDouble c_timeout;
  DDouble r_timeout;
  DDouble w_timeout;

#ifdef HAVE_EXT_STDIO_FILEBUF_H
// GGH ggh hack to implement SPAWN keyword UNIT
  __gnu_cxx::stdio_filebuf<char> *readbuf_frb_destroy_on_close_p;
  std::basic_streambuf<char> *readbuf_bsrb_destroy_on_close_p;
  int fd_close_on_close;
#endif

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
    anyStream(NULL), 
    /*    fStream(NULL), 
	  igzStream(NULL), 
	  ogzStream(NULL), */
    f77(false),
    swapEndian(false),
    deleteOnClose(false),
    varlenVMS(false),
    compress(false),
    xdrs(NULL),

    iSocketStream(NULL), 
    sockNum( -1),
    recvBuf(NULL),
    c_timeout(0.0),
    r_timeout(0.0),
    w_timeout(0.0),

    width( defaultStreamWidth),
    lastSeekPos( 0),
    lastRecord( 0),
    lastRecordStart( 0)
    {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
      readbuf_frb_destroy_on_close_p = NULL;
      readbuf_bsrb_destroy_on_close_p = NULL;
      fd_close_on_close = -1;
#endif
    }

  ~GDLStream() 
  {
    delete xdrs;

    delete anyStream;
    /*    delete fStream;
	  delete igzStream;
	  delete ogzStream;*/
    delete iSocketStream;
  }  

 #ifdef HAVE_EXT_STDIO_FILEBUF_H
  void Open( const std::string& name_,
	     std::ios_base::openmode,
	     bool swapEndian_, bool deleteOnClose_, bool xdr_, 
	     SizeT width, bool f77, bool compress,__gnu_cxx::stdio_filebuf<char> *in=NULL, __gnu_cxx::stdio_filebuf<char> *out=NULL);
#else
  void Open( const std::string& name_,
	     std::ios_base::openmode,
	     bool swapEndian_, bool deleteOnClose_, bool xdr_, 
	     SizeT width, bool f77, bool compress);
#endif  
  void Socket( const std::string& host,
	       DUInt port, bool swapEndian_,
	       DDouble c_timeout, DDouble r_timeout, DDouble w_timeout);

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
  
  bool InUse() { return (anyStream != NULL && anyStream->InUse());}

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

  XDR *Xdr() { return xdrs;}

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


#ifdef HAVE_EXT_STDIO_FILEBUF_H
// GGH ggh hack to implement SPAWN keyword UNIT
  std::basic_streambuf<char> *get_stream_readbuf_bsrb();
  int set_stream_readbuf_bsrb_from_frb(__gnu_cxx::stdio_filebuf<char> *frb_p, std::string newname="");
  int set_readbuf_frb_destroy_on_close(__gnu_cxx::stdio_filebuf<char> *frb_p);
  int set_readbuf_bsrb_destroy_on_close(std::basic_streambuf<char> *bsrb_p);
  int set_fd_close_on_close(int fd);
#endif

};


typedef std::vector<GDLStream> GDLFileListT;

#endif
