/***************************************************************************
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
#ifdef __MINGW32__
#include <unistd.h> // for close()
#endif

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <iostream>
#include <ext/stdio_filebuf.h>// TODO: is it portable across compilers?
using __gnu_cxx::stdio_filebuf;

inline stdio_filebuf<char> * fileBufFromFD(int fd, std::_Ios_Openmode mode) {
  return (new stdio_filebuf<char> (fd, mode));
}
bool GDLStream::CanOpenAsPipes(){return true;}
#else
bool GDLStream::CanOpenAsPipes(){return false;}
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#define NS_INT16SZ       2
#define NS_INADDRSZ      4
#define NS_IN6ADDRSZ    16
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 9901 
#endif

static int inet_pton4(const char *src, unsigned char *dst) {
  static const char digits[] = "0123456789";
  int saw_digit, octets, ch;
  unsigned char tmp[NS_INADDRSZ], *tp;

  saw_digit = 0;
  octets = 0;
  *(tp = tmp) = 0;
  while ((ch = *src++) != ' ') {
    const char *pch;

    if ((pch = strchr(digits, ch)) != NULL) {
      unsigned int iNew = (unsigned int) (*tp * 10 + (pch - digits));

      if (iNew > 255) return (0);
      *tp = iNew;
      if (!saw_digit) {
        if (++octets > 4) return (0);
        saw_digit = 1;
      }
    } else if (ch == '.' && saw_digit) {
      if (octets == 4) return (0);
      *++tp = 0;
      saw_digit = 0;
    } else {
      return (0);
    }
  }

  if (octets < 4) return (0);

  memcpy(dst, tmp, NS_INADDRSZ);

  return (1);
}

static int inet_pton6(const char *src, unsigned char * dst) {
  static const char xdigits_l[] = "0123456789abcdef",
    xdigits_u[] = "0123456789ABCDEF";
  unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
  const char *xdigits, *curtok;
  int ch, saw_xdigit;
  unsigned int val;

  memset((tp = tmp), ' ', NS_IN6ADDRSZ);
  endp = tp + NS_IN6ADDRSZ;
  colonp = NULL;

  // Leading :: requires some special handling.
  if (*src == ':' && *++src != ':') return (0);

  curtok = src;
  saw_xdigit = 0;
  val = 0;
  while ((ch = *src++) != ' ') {
    const char *pch;

    if ((pch = strchr((xdigits = xdigits_l), ch)) == NULL)
      pch = strchr((xdigits = xdigits_u), ch);

    if (pch != NULL) {
      val <<= 4;
      val |= (pch - xdigits);
      if (val > 0xffff) return (0);
      saw_xdigit = 1;
      continue;
    }

    if (ch == ':') {
      curtok = src;
      if (!saw_xdigit) {
        if (colonp) return (0);
        colonp = tp;
        continue;
      }

      if (tp + NS_INT16SZ > endp) return (0);

      *tp++ = (unsigned char) (val >> 8) & 0xff;
      *tp++ = (unsigned char) val & 0xff;
      saw_xdigit = 0;
      val = 0;
      continue;
    }

    if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) && inet_pton4(curtok, tp) > 0) {
      tp += NS_INADDRSZ;
      saw_xdigit = 0;
      break; /* ' ' was seen by inet_pton4(). */
    }
    return (0);
  }

  if (saw_xdigit) {
    if (tp + NS_INT16SZ > endp) return (0);

    *tp++ = (unsigned char) (val >> 8) & 0xff;
    *tp++ = (unsigned char) val & 0xff;
  }

  if (colonp != NULL) {
    // Since some memmove()'s erroneously fail to handle overlapping regions, we'll do the shift by hand.
    const int n = (int) (tp - colonp);
    int i;

    for (i = 1; i <= n; i++) {
      endp[-i] = colonp[n - i];
      colonp[n - i] = 0;
    }
    tp = endp;
  }

  if (tp != endp) return (0);

  memcpy(dst, tmp, NS_IN6ADDRSZ);
  return (1);
}

int inet_pton(int af, const char *src, void *dst) {
  switch (af) {
  case AF_INET:
    return inet_pton4(src, (unsigned char *) dst);
  case AF_INET6:
    return inet_pton6(src, (unsigned char *) dst);
  default:
    errno = EAFNOSUPPORT;
    return (-1);
  }
}
// This source code is from http://blog.naver.com/PostView.nhn?blogId=websearch&logNo=70089324416&parentCategoryNo=4&viewDate=&currentPage=1&listtype=0
#endif


using namespace std;

const string StreamInfo(ios* searchStream) {
  if (dynamic_cast<istringstream*> (searchStream) != NULL)
    return "Unit: 0, <stdin> (redirected)";

  if (searchStream == &cin) return "Unit: 0, <stdin>";
  if (searchStream == &cout) return "Unit: -1, <stdout>";
  if (searchStream == &cerr) return "Unit: -2, <stderr>";
  for (SizeT i = 0; i < fileUnits.size(); i++) {
    if (fileUnits[ i].anyStream != NULL && fileUnits[ i].anyStream->FStream() == searchStream) {
      return "Unit: " + i2s(i + 1) + ", File: " + fileUnits[ i].Name();
    }
  }
  return "Internal error: Stream not found.";
}

void AnyStream::Close() {
  if (fStream != NULL && fStream->is_open()) {
    fStream->flush();
    if (old_rdbuf_in != nullptr) {
      std::basic_streambuf<char> *current=fStream->std::ios::rdbuf();
      fStream->std::ios::rdbuf(old_rdbuf_in);
      old_rdbuf_in=nullptr;
      ispipe = false;
      close(pin_fd);
      pin_fd=-1;
    }
    fStream->close();
    fStream->clear();
    delete fStream;
    fStream = NULL;
  }
  if (ofStream != NULL && ofStream->is_open()) {
    ofStream->flush();
    if (old_rdbuf_out != nullptr) {
      std::basic_streambuf<char> *current=ofStream->std::ios::rdbuf();
      ofStream->std::ios::rdbuf(old_rdbuf_out);
      old_rdbuf_out=nullptr;
      ispipe = false;
      close(pout_fd);
      pout_fd=-1;
    }
    ofStream->close();
    ofStream->clear();
    delete ofStream;
    ofStream = NULL;
  }
  if (igzStream != NULL && igzStream->rdbuf()->is_open()) {
    igzStream->close();
    igzStream->clear();
  }
  if (ogzStream != NULL && ogzStream->rdbuf()->is_open()) {
    ogzStream->close();
    ogzStream->clear();
  }
}

void AnyStream::OpenAsPipes(const std::string& name_, const std::ios_base::openmode mode_, const int pipeInFd, const int pipeOutFd) {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
  delete igzStream;
  igzStream = NULL;
  delete ogzStream;
  ogzStream = NULL;
  // Here we invoke the black arts of converting from a C FILE*fd to an fstream object
  if (mode_ & std::ios_base::in) {
    pin_fd = pipeInFd;
    stdio_filebuf<char> * in = fileBufFromFD(pipeInFd, std::ios_base::in);
    if (fStream == NULL) fStream = new fstream();
    fStream->open("/dev/null", std::ios_base::in);
    if (fStream->fail()) {
      delete fStream;
      fStream = NULL;
      throw GDLIOException(-1, "Error opening special infile.");
    }
    old_rdbuf_in = fStream->std::ios::rdbuf();
    fStream->std::ios::rdbuf(in);
    ispipe=true;
  }
  if (mode_ & std::ios_base::out) {
    pout_fd=pipeOutFd;
    stdio_filebuf<char> * out = fileBufFromFD(pipeOutFd, std::ios_base::out);
    if (out != nullptr) {
      if (ofStream == NULL) ofStream = new fstream();
      ofStream->open("/dev/null", std::ios_base::out);
      if (ofStream->fail()) {
        delete ofStream;
        ofStream = NULL;
        throw GDLIOException(-1, "Error opening special outfile.");
      }
      old_rdbuf_out = ofStream->std::ios::rdbuf();
      ofStream->std::ios::rdbuf(out);
      ispipe=true;
    }
  }
#else
  throw GDLIOException(-1,"This command relies on GNU extensions to the std C++ library that were not available during compilation on your system (?)");
#endif
}
  void AnyStream::Open(const std::string& name_, ios_base::openmode mode_, bool compress_) {
  if (compress_) {

    delete fStream;
    fStream = NULL;
    delete ofStream;
    ofStream = NULL;
    
    if ((mode_ & std::ios::out)) {
      if (ogzStream == NULL)
        ogzStream = new ogzstream();

      ogzStream->open(name_.c_str(), mode_ & ~std::ios::in);

      if (ogzStream->fail()) {
        delete ogzStream;
        ogzStream = NULL;
        throw GDLIOException("Error opening compressed file for output.");
      }
    } else {
      delete ogzStream;
      ogzStream = NULL;
    }
    if ((mode_ & std::ios::in) && !(mode_ & std::ios::out)) {
      if (igzStream == NULL)
        igzStream = new igzstream();

      igzStream->open(name_.c_str(), mode_ & ~std::ios::out);

      if (igzStream->fail()) {
        delete igzStream;
        igzStream = NULL;
        throw GDLIOException("Error opening compressed file for input.");
      }
    } else {
      delete igzStream;
      igzStream = NULL;
    }
  } else {
    delete igzStream;
    igzStream = NULL;
    delete ogzStream;
    ogzStream = NULL;

  if (fStream == NULL)
    fStream = new fstream();
  if (fStream->is_open()) throw GDLIOException(-1, "File already opened.");
  fStream->open(name_.c_str(), mode_);

  if (fStream->fail()) {
    delete fStream;
    fStream = NULL;
    if (((mode_ | ios_base::in) != 0) && ((mode_ | ios_base::out) == 0))
      throw GDLIOException(-265, "Error opening file for reading.");
    throw GDLIOException(-1, "Error opening file.");
  }
}
}

void AnyStream::ClearEof() {
  if (fStream && fStream->eof()) fStream->clear();
  if (ofStream && ofStream->eof()) ofStream->clear();
  if (igzStream && igzStream->eof()) igzStream->clear();
  if (ogzStream && ogzStream->eof()) ogzStream->clear();
}

void AnyStream::Write(char* buf, std::streamsize nBytes) {
  if (ofStream != NULL) { //special case pipe: ofStream exists
//    std::cerr<<"writing to pipe"<<std::endl;
    ofStream->write(buf, nBytes);
  } else 
    if (fStream != NULL) {
    fStream->write(buf, nBytes);
  } else if (ogzStream != NULL) {
    ogzStream->write(buf, nBytes);
  } else assert(0);
}

void AnyStream::Read(char* buf, std::streamsize nBytes) {
  if (fStream != NULL) {
    fStream->read(buf, nBytes);
  } else if (igzStream != NULL) {
    igzStream->read(buf, nBytes);
  } else assert(0);
}
std::streamsize AnyStream::Gcount() {
  if (fStream != NULL) {
    return fStream->gcount();
  } else if (igzStream != NULL) {
    return igzStream->gcount();
  } else assert(0);
    throw; // getting rid of compiler warning
}

bool AnyStream::Good() {
  if (fStream != NULL && ofStream != NULL)
    return fStream->good() && ofStream->good();
  else 
    if (fStream != NULL)
    return fStream->good();
  else if (ofStream != NULL)
    return ofStream->good();
  else if (igzStream != NULL && ogzStream != NULL)
    return igzStream->good() && ogzStream->good();
  else if (igzStream != NULL)
    return igzStream->good();
  else if (ogzStream != NULL)
    return ogzStream->good();
  else {
    assert(false);
    throw; // getting rid of compiler warning
  }
}

bool AnyStream::EofRaw() {
  if (fStream != NULL) {
    return fStream->eof();
  }
  if (igzStream != NULL) {
    return igzStream->eof();
  }
  return true;
}

void AnyStream::SeekEof() {
  if (fStream == NULL && igzStream == NULL && ogzStream == NULL)
    throw GDLException("Inner file unit is not open.");

  if (fStream != NULL) {
    fStream->seekp(0, std::ios_base::end);
    fStream->seekg(0, std::ios_base::end);
  } else if (igzStream != NULL) {
    igzStream->seekg(0, std::ios_base::end);
  } else if (ogzStream != NULL) {
    ogzStream->seekp(0, std::ios_base::end);
  }
}

bool AnyStream::Eof() {
  if (!InUse()) throw GDLException("Inner file unit is not open.");

  if (fStream != NULL) {
    fStream->clear(); // clear old EOF	

    if (ispipe) return true;
    else {
      fStream->peek(); // trigger EOF if at EOF
      return fStream->eof();
    }
  }
  if (igzStream != NULL) {
    igzStream->clear(); // clear old EOF	

    igzStream->peek(); // trigger EOF if at EOF

    return igzStream->eof();
  }
  return true;
}

void AnyStream::Seek(std::streampos pos) {
  if (!InUse()) throw GDLException("inner file unit is not open.");

  if (fStream != NULL) {
    if (fStream->eof())
      fStream->clear();

    fStream->rdbuf()->pubseekpos(pos, std::ios_base::in | std::ios_base::out);
  }
  if (ofStream != NULL) {
    if (ofStream->eof())
      ofStream->clear();

    ofStream->rdbuf()->pubseekpos(pos, std::ios_base::out);
  }
  if (igzStream != NULL) {
    if (igzStream->eof())
      igzStream->clear();

    igzStream->seekg(pos);
    // 	igzStream->rdbuf()->pubseekpos( pos, std::ios_base::in);
  }
  if (ogzStream != NULL) {
    if (ogzStream->eof())
      ogzStream->clear();

    ogzStream->seekp(pos);
    // 	ogzStream->rdbuf()->pubseekpos( pos, std::ios_base::out);
  }
}

DLong64 AnyStream::Skip(std::streampos pos, bool doThrow) {
  if (!InUse()) throw GDLException("inner file unit is not open.");

  if (fStream != NULL) {
    if (fStream->eof())
      fStream->clear();

    std::streampos  cur=fStream->tellg();
    fStream->ignore(pos);
    if ( doThrow && fStream->eof()) throw GDLException("End of file encountered.");
    DLong64 ret=fStream->tellg()-cur;
    return ret;
  }
  if (igzStream != NULL) {
    if (igzStream->eof())
      igzStream->clear();

    std::streampos cur=igzStream->tellg();
    igzStream->ignore(pos);
    if ( doThrow && igzStream->eof()) throw GDLException("End of file encountered.");
    DLong64 ret=igzStream->tellg()-cur;
    return ret;
  }
  return 0;
}

DLong AnyStream::SkipLines(DLong nlines, bool doThrow) {
  if (!InUse()) throw GDLException("inner file unit is not open.");

  if (fStream != NULL) {
    if (fStream->eof())
      fStream->clear();
    DLong i = 0;
    for (; i < nlines; ++i) {
      fStream->ignore(std::numeric_limits<ptrdiff_t>::max(), '\n');
      if (fStream->eof()) {
        if (doThrow) throw GDLException("End of file encountered.");
        else break;
      }
    }
    return i;
  }
  if (igzStream != NULL) {
    if (igzStream->eof())
      igzStream->clear();
    DLong i = 0;
    for (; i < nlines; ++i) {
      igzStream->ignore(std::numeric_limits<ptrdiff_t>::max(), '\n');
      if (igzStream->eof()) {
        if (doThrow) throw GDLException("End of file encountered.");
        else break;
      }
    }
    return i;
  }
  return 0;

}

std::streampos AnyStream::Size() {
  if (fStream != NULL) {
    std::streampos cur = fStream->tellp(); 
    std::streampos end = fStream->rdbuf()->pubseekoff(0, std::ios_base::end);
    fStream->rdbuf()->pubseekpos(cur, std::ios_base::in | std::ios_base::out);
    return end;
  } else if (ofStream != NULL) {
    std::streampos cur = ofStream->tellp(); 
    std::streampos end = ofStream->rdbuf()->pubseekoff(0, std::ios_base::end);
    ofStream->rdbuf()->pubseekpos(cur, std::ios_base::out);
    return end;
  } else {
    if (igzStream != NULL) {
      std::streampos cur = igzStream->tellg(); //->rdbuf()->pubseekoff(0, std::ios_base::cur);
      std::streampos end = igzStream->rdbuf()->pubseekoff(0, std::ios_base::end);
      igzStream->rdbuf()->pubseekpos(cur, std::ios_base::in);
      return end;
    }
    if (ogzStream != NULL) {
      std::streampos cur = ogzStream->tellp(); //rdbuf()->pubseekoff(0, std::ios_base::cur);
      std::streampos end = ogzStream->rdbuf()->pubseekoff(0, std::ios_base::end);
      ogzStream->rdbuf()->pubseekpos(cur, std::ios_base::out);
      return end;
    } else {
      assert(false);
      throw; // getting rid of compiler warning
    }
  }
}

std::streampos AnyStream::Tell() {
  if (fStream != NULL) return fStream->tellp(); //openr
  else if (ofStream != NULL) return ofStream->tellp(); //openr
  else if (igzStream != NULL)
    return igzStream->rdbuf()->getPosition(); 
  else if (ogzStream != NULL)
    return ogzStream->tellp();
  else {
    assert(false);
    throw; // getting rid of compiler warning
  }
}

void AnyStream::SeekPad(std::streampos pos) {
  if (fStream == NULL && ofStream == NULL && ogzStream == NULL)  throw GDLException("File unit is not open.");
  if (ofStream != NULL) {
    if (ofStream->eof())
      ofStream->clear();

    std::streampos fSize = Size();
    if (pos > fSize) Pad(pos - fSize);

    ofStream->rdbuf()->pubseekpos(pos, std::ios_base::out);
  } else 
    if (fStream != NULL) {
    if (fStream->eof())
      fStream->clear();

    std::streampos fSize = Size();
    if (pos > fSize) Pad(pos - fSize);

    fStream->rdbuf()->pubseekpos(pos, std::ios_base::in | std::ios_base::out);
  } else  {
    if (ogzStream->eof())
      ogzStream->clear();

    std::streampos fSize = Size();
    if (pos > fSize) Pad(pos - fSize);

    ogzStream->rdbuf()->pubseekpos(pos, std::ios_base::in | std::ios_base::out);
  }
}

void GDLStream::OpenAsPipes(const string& name_, const std::ios_base::openmode mode_, const int pipeInFd, const int pipeOutFd ) {
  if (anyStream != NULL && anyStream->IsOpen())  throw GDLIOException("File unit is already open.");

  if (anyStream == NULL) anyStream = new AnyStream();

  name = name_;
  mode = mode_;
  compress = false;
  f77 = false;
  varlenVMS = false;
  anyStream->OpenAsPipes(name, mode_, pipeInFd, pipeOutFd );

  swapEndian = false;
  deleteOnClose = true;

  lastSeekPos = 0;
  lastRecord = 0;
  lastRecordStart = 0;
  width = 0;
}


void GDLStream::Open(const string& name_,
    ios_base::openmode mode_,
    bool swapEndian_, bool dOC, bool xdr_,
    SizeT width_,
    bool f77_, bool compress_) {
  string expName = name_;
  WordExp(expName);

  f77 = f77_;

  //   if( f77_)
  //     throw GDLException("F77_UNFORMATTED format not supported.");

  //  if( (fStream != NULL && fStream->is_open()) || (igzStream != NULL && igzStream->rdbuf()->is_open()) || (ogzStream != NULL && ogzStream->rdbuf()->is_open()))
  if (anyStream != NULL && anyStream->IsOpen())
    throw GDLIOException("File unit is already open.");

  if (anyStream == NULL)
    anyStream = new AnyStream();

  name = expName;
  mode = mode_;
  compress = compress_;

  anyStream->Open(expName, mode_, compress_);

  swapEndian = swapEndian_;
  deleteOnClose = dOC;

  if (xdr_)
    xdrs = new XDR;

  lastSeekPos = 0;
  lastRecord = 0;
  lastRecordStart = 0;
  width = width_;
}

void GDLStream::Socket(const string& host,
  DUInt port, bool swapEndian_,
  DDouble c_timeout_, DDouble r_timeout_,
  DDouble w_timeout_) {
  if (iSocketStream == NULL)
    iSocketStream = new istringstream;

  if (recvBuf == NULL)
    recvBuf = new string;

  name = host;

  sockNum = socket(AF_INET, SOCK_STREAM, 0);

  c_timeout = c_timeout_;
  r_timeout = r_timeout_;
  w_timeout = w_timeout_;

  int on = 1;
  if (setsockopt(sockNum, SOL_SOCKET, SO_REUSEADDR,
    (const char*) &on, sizeof (on)) == -1) {
    throw GDLIOException("Error opening file.");
  }

  sockaddr_in m_addr;
  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons(port);

  // Convert host to IPv4 format
  struct hostent *h;
  if ((h = gethostbyname(host.c_str())) == NULL) { // get the host info
    throw GDLIOException("Unable to lookup host.");
  }

  //  cout << inet_ntoa(*((struct in_addr *)h->h_addr)) << endl;

  int status = inet_pton(AF_INET, inet_ntoa(*((struct in_addr *) h->h_addr)),
    &m_addr.sin_addr);

  status = connect(sockNum, (sockaddr *) & m_addr, sizeof (m_addr));

  swapEndian = swapEndian_;

  // BIG limit on socket send width to avoid leading \n in CheckNL
  width = 32768;
}

void AnyStream::Flush() {
  if (fStream != NULL) {
    fStream->flush();
  }
   if (ofStream != NULL) {
    ofStream->flush();
  }
  if (ogzStream != NULL) {
    ogzStream->flush();
  }
}

void GDLStream::Flush() {
  if (anyStream != NULL) {
    anyStream->Flush();
  }
}

void GDLStream::Close() {
  if (anyStream != NULL) {
    anyStream->Close();
    if (deleteOnClose)
      std::remove(name.c_str());
  }
  name = "";
  f77 = false;
  swapEndian = false;
  compress = false;
  deleteOnClose = false;

  delete xdrs;
  xdrs = NULL;

  width = defaultStreamWidth;

  sockNum = -1;
  c_timeout = 0.0;
  r_timeout = 0.0;
  w_timeout = 0.0;
}

void GDLStream::Free() {
  Close();

  delete anyStream;
  anyStream = NULL;

  delete iSocketStream;
  iSocketStream = NULL;

  delete recvBuf;
  recvBuf = NULL;

  getLunLock = false;
}

igzstream& GDLStream::IgzStream() {
  if (anyStream == NULL || anyStream->IgzStream() == NULL || !anyStream->IsOpen())
    throw GDLIOException("File unit is not open for compressed reading or writing.");
  if (!(mode & ios::in))
    throw GDLIOException("File unit is not open for reading.");
  return *anyStream->IgzStream();
}

ogzstream& GDLStream::OgzStream() {
  if (anyStream == NULL || anyStream->OgzStream() == NULL || !anyStream->IsOpen())
    throw GDLIOException("File unit is not open for compressed reading or writing.");
  if (!(mode & ios::out))
    throw GDLIOException("File unit is not open for compressed writing.");
  return *anyStream->OgzStream();
}

fstream& GDLStream::IStream() { //IStream is always FStream (normal file)
  if (anyStream == NULL || anyStream->FStream() == NULL || !anyStream->IsOpen())
    throw GDLIOException("File unit is not open.");
  if (!(mode & ios::in))
    throw GDLIOException("File unit is not open for reading.");
  return *anyStream->FStream();
}

fstream& GDLStream::OStream() { //OStream may be oFStream if oFStream is not NULL (different pipe)
  if (anyStream->oFStream() != NULL) { //process special pipe
    return *anyStream->oFStream();
  }
  // old behaviour for normal files
  if (anyStream == NULL || anyStream->FStream() == NULL || !anyStream->IsOpen())
    throw GDLIOException("File unit is not open.");
  if (!(mode & ios::out))
    throw GDLIOException("File unit is not open for writing.");
  return *anyStream->FStream();
}

istringstream& GDLStream::ISocketStream() {
  if (iSocketStream == NULL)
    throw GDLIOException("Socket unit is not open.");
  return *iSocketStream;
}

void GDLStream::Pad(std::streamsize nBytes) {
  if (anyStream != NULL)
    anyStream->Pad(nBytes);
}

void AnyStream::Pad(std::streamsize nBytes) {
  const std::streamsize bufSize = 1024;
  static char buf[ bufSize];
  SizeT nBuf = nBytes / bufSize;
  std::streamsize lastBytes = nBytes % bufSize;
  if (ofStream != NULL) {
    for (SizeT i = 0; i < nBuf; ++i) fStream->write(buf, bufSize);
    if (lastBytes > 0) ofStream->write(buf, lastBytes);
  } else 
    if (fStream != NULL) {
    for (SizeT i = 0; i < nBuf; ++i) fStream->write(buf, bufSize);
    if (lastBytes > 0) fStream->write(buf, lastBytes);
  } else if (ogzStream != NULL) {
    for (SizeT i = 0; i < nBuf; ++i) ogzStream->write(buf, bufSize);
    if (lastBytes > 0) ogzStream->write(buf, lastBytes);
  }
}

void GDLStream::F77Write(DULong tCount) {
  anyStream->ClearEof();

  assert(sizeof ( DULong) == 4);
  if (swapEndian) {
    char swapTCount[ sizeof ( DULong)];
    for (SizeT i = 0; i<sizeof ( DULong); ++i)
      swapTCount[i] =
      reinterpret_cast<char*> (&tCount)[ sizeof ( DULong) - 1 - i];
    anyStream->Write(swapTCount, sizeof ( DULong));
  } else {
    anyStream->Write(reinterpret_cast<char*> (&tCount), sizeof ( DULong));
  }

  if (!anyStream->Good()) {
    throw GDLIOException("Error writing F77_UNFORMATTED record data.");
  }
}

DULong GDLStream::F77ReadStart() {
  if (anyStream->EofRaw())
    throw GDLIOException("End of file encountered.");

  assert(sizeof ( DULong) == 4);
  DULong tCountRd;
  if (swapEndian) {
    char swapTCount[ sizeof ( DULong)];
    anyStream->Read(swapTCount, sizeof ( DULong));
    for (SizeT i = 0; i<sizeof ( DULong); ++i)
      reinterpret_cast<char*> (&tCountRd)[ sizeof ( DULong) - 1 - i] =
      swapTCount[i];
  } else {
    anyStream->Read(reinterpret_cast<char*> (&tCountRd), sizeof ( DULong));
  }

  if (anyStream->EofRaw())
    throw GDLIOException("End of file encountered.");

  if (!anyStream->Good()) {
    throw GDLIOException("Error reading F77_UNFORMATTED record data.");
  }

  lastRecord = tCountRd;
  lastRecordStart = Tell();
  return tCountRd;
}

void GDLStream::F77ReadEnd() {
  if (anyStream->EofRaw())
    throw GDLIOException("End of file encountered.");

  std::streampos actPos = Tell();
  if (actPos > (lastRecordStart + lastRecord))
    throw GDLIOException("Read past end of Record of F77_UNFORMATTED file.");

  if (actPos < (lastRecordStart + lastRecord))
    Seek(lastRecordStart + lastRecord);

  DULong tCountRd;
  if (swapEndian) {
    char swapTCount[ sizeof ( DULong)];
    anyStream->Read(swapTCount, sizeof ( DULong));
    for (SizeT i = 0; i<sizeof ( DULong); ++i)
      reinterpret_cast<char*> (&tCountRd)[ sizeof ( DULong) - 1 - i] = swapTCount[i];
  } else {
    anyStream->Read(reinterpret_cast<char*> (&tCountRd), sizeof ( DULong));
  }

  if (anyStream->EofRaw())
    throw GDLIOException("End of file encountered.");

  if (!anyStream->Good()) {
    throw GDLIOException("Error reading F77_UNFORMATTED record data.");
  }

  if (lastRecord != static_cast<std::streampos> (tCountRd))
    throw GDLIOException("Logical error in F77_UNFORMATTED file.");

}

bool GDLStream::Eof() {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");

  return anyStream->Eof();
}

void GDLStream::SeekEof() {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");

  anyStream->SeekEof();
  lastSeekPos = anyStream->Tell();
}

void GDLStream::Seek(std::streampos pos) {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");
  anyStream->Seek(pos);
  lastSeekPos = pos;
}

void GDLStream::Truncate() {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");
  std::streampos currentPos = anyStream->Tell();
  char* buffer = (char*) malloc(currentPos);
  anyStream->Seek(0);
  anyStream->Read(buffer, currentPos);
  anyStream->Close();
  anyStream->Open(name, fstream::out | fstream::trunc, compress);
  lastSeekPos = 0;
  lastRecord = 0;
  lastRecordStart = 0;
  anyStream->Write(buffer, currentPos);
  anyStream->Close();
  anyStream->Open(name, mode & ~fstream::trunc, compress); //write again but not trunc!
  anyStream->Seek(currentPos);
  lastSeekPos = anyStream->Tell();
  free(buffer);
}

//should work with compressed files too
DLong64 GDLStream::CopySomeTo(DLong64 nbytes, GDLStream& to, bool doThrow) {
  DLong64 chunksize = 1000000; //1 megabyte
  DLong64 nchunk = nbytes / chunksize;
  ssize_t remainder = nbytes - (nchunk * chunksize);
  char* buffer = (char*) malloc(chunksize);
  bool earlyEnd = false;
  std::streampos size = this->Size();
  DLong64 ret = 0;
  DLong64 n = 0;  
  //the first nchunk chunks...
  for (int i = 0; i < nchunk; ++i) {
    //current position in 'in'
    std::streampos startpos = anyStream->Tell();
    anyStream->Read(buffer, chunksize);
    //perhaps we encountered end of file. The principle is that all the valid data has already been copied BEFORE end-of-file is reported
    if (!anyStream->Good()) {
      n = size - startpos; //error eof
      earlyEnd = true;
      to.anyStream->Write(buffer, n);
      ret += n;
      break; //no input bytes anymore!
    }
    to.anyStream->Write(buffer, chunksize);
    ret += chunksize;
  }
  //remainder: idem except if early end
  if (!earlyEnd) {
    //current position in 'in'
    std::streampos startpos = anyStream->Tell();
    anyStream->Read(buffer, remainder);
    if (!anyStream->Good()) {
      n = size - startpos; //error eof
    } else n = remainder;
    to.anyStream->Write(buffer, n);
    ret += n;
  }
  free(buffer);
  if (doThrow && !anyStream->Good()) throw GDLException("End of file encountered. File: " + name);
  return ret;
}
//Slow as does the copy byte per byte. Could be improved for normal files.
DLong GDLStream::CopySomeLinesTo(DLong nlines, GDLStream& to, bool doThrow) {
  DLong i=0;
  for (; i < nlines; ++i) {
    //current position in 'in'
    char buf[32];
    do {
      anyStream->Read(buf,1);
      if (!anyStream->Good()) {
        if (doThrow) throw GDLException("End of file encountered. File: " + name);
        return i;
      }
      to.anyStream->Write(buf, 1); 
    } while(buf[0]!='\n');
  }
  return i;
}

DLong64  GDLStream::Skip(std::streampos pos, bool doThrow) {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");
  DLong64 ret = anyStream->Skip(pos, doThrow);
  lastSeekPos = anyStream->Tell();
  return ret;
}
DLong GDLStream::SkipLines(DLong nlines, bool doThrow) {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");
  DLong n = anyStream->SkipLines(nlines, doThrow);
  lastSeekPos = anyStream->Tell();
  return n;
}

void GDLStream::SeekPad(std::streampos pos) {
  if (anyStream == NULL)
    throw GDLException("File unit is not open.");
  anyStream->SeekPad(pos);
  lastSeekPos = pos;
}

// ============================================================================
// gzstream, C++ iostream classes wrapping the zlib compression library.
// Copyright (C) 2001  Deepak Bandyopadhyay, Lutz Kettner
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// ============================================================================
//
// File          : gzstream.C
// Author(s)     : Deepak Bandyopadhyay, Lutz Kettner
// 
// Standard streambuf implementation following Nicolai Josuttis, "The 
// Standard C++ Library".
// ============================================================================

#ifdef GZSTREAM_NAMESPACE
namespace GZSTREAM_NAMESPACE {
#endif

  // ----------------------------------------------------------------------------
  // Internal classes to implement gzstream. See header file for user classes.
  // ----------------------------------------------------------------------------

  // --------------------------------------
  // class gzstreambuf:
  // --------------------------------------

  gzstreambuf* gzstreambuf::open(const char* name, int open_mode) {
    if (is_open())
      return (gzstreambuf*) 0;
    position = 0;
    mode = open_mode;
    // no append nor read/write mode
    if ((mode & std::ios::ate) || (mode & std::ios::app)
      || ((mode & std::ios::in) && (mode & std::ios::out)))
      return (gzstreambuf*) 0;
    char fmode[10];
    char* fmodeptr = fmode;
    if (mode & std::ios::in)
      *fmodeptr++ = 'r';
    else if (mode & std::ios::out)
      *fmodeptr++ = 'w';
    *fmodeptr++ = 'b';
    *fmodeptr = '\0';
    file = gzopen(name, fmode);
    if (file == 0)
      return (gzstreambuf*) 0;
    opened = 1;
    return this;
  }

  gzstreambuf * gzstreambuf::close() {
    if (is_open()) { //reset buf to 0 position: solves bug #724
      setg(buffer + buf4, // beginning of putback area
        buffer + buf4, // read position
        buffer + buf4); // end position      
      sync();
      opened = 0;
      position = 0;
      if (gzclose(file) == Z_OK)
        return this;
    }
    return (gzstreambuf*) 0;
  }

  int gzstreambuf::underflow() { // used for input buffer only
    if (gptr() && (gptr() < egptr()))
      return * reinterpret_cast<unsigned char *> (gptr());

    if (!(mode & std::ios::in) || !opened)
      return EOF;
    // Josuttis' implementation of inbuf
    int n_putback = gptr() - eback();
    if (n_putback > buf4)
      n_putback = buf4;
    memcpy(buffer + (buf4 - n_putback), gptr() - n_putback, n_putback);

    int num = gzread(file, buffer + buf4, bufferSize - buf4);
    if (num <= 0) // ERROR or EOF
      return EOF;

    // reset buffer pointers
    setg(buffer + (buf4 - n_putback), // beginning of putback area
      buffer + buf4, // read position
      buffer + buf4 + num); // end of buffer

    // return next character
    return * reinterpret_cast<unsigned char *> (gptr());
  }

  int gzstreambuf::flush_buffer() {
    // Separate the writing of the buffer from overflow() and
    // sync() operation.
    int w = pptr() - pbase();
    if (gzwrite(file, pbase(), w) != w)
      return EOF;
    pbump(-w);
    return w;
  }

  int gzstreambuf::overflow(int c) { // used for output buffer only
    if (!(mode & std::ios::out) || !opened)
      return EOF;
    if (c != EOF) {
      *pptr() = c;
      pbump(1);
    }
    if (flush_buffer() == EOF)
      return EOF;
    return c;
  }

  int gzstreambuf::sync() {
    // Changed to use flush_buffer() instead of overflow( EOF)
    // which caused improper behavior with std::endl and flush(),
    // bug reported by Vincent Ricard.
    if (pptr() && pptr() > pbase()) {
      if (flush_buffer() == EOF)
        return -1;
    }
    return 0;
  }

  std::streampos gzstreambuf::pubseekpos(std::streampos sp, std::ios_base::openmode which) {
    if (is_open()) {
//            cerr<<"seeking "<<sp<<" when we are at "<<gztell(this->file)<<endl;
      if ((which & std::ios_base::in && this->mode & std::ios::in) || /* read mode : ok */
        (which & std::ios_base::out && this->mode & std::ios::out &&
        static_cast<z_off_t> (sp) >= gztell(this->file))) /* write mode : seek forward only */ {
        z_off_t off = gzseek(this->file, static_cast<z_off_t> (0), SEEK_SET); //absolutely necessary to rewind!!!
        position = 0;
        setg(buffer + buf4, buffer + buf4, buffer + buf4);
        if (sp != 0) off = gzseek(this->file, static_cast<z_off_t> (sp), SEEK_SET);
//                fprintf(stderr, "Seek: %d=pubseekpos(sp=%d)\n", off, static_cast<z_off_t> (sp));
        // GD. seems reset of buffer is needed only when rewinded at 0.
        if (which & std::ios_base::in && off == 0) setg(buffer + buf4, buffer + buf4, buffer + buf4);
        position = off;
        return off;
      } else {
        z_off_t off = static_cast<std::streampos> (gztell(this->file)); /* Just don't Seek, no error */
//                fprintf(stderr, "Tell: %d=pubseekpos(sp=%d)\n", off, static_cast<z_off_t> (sp));
        position = off;
        return off;
      }
    }
    //    fprintf(stderr, "EOF: -1=pubseekpos(sp=%d)\n", static_cast<z_off_t> (sp));
    return -1;
  }

  std::streampos gzstreambuf::pubseekoff(std::streamoff offIn, std::ios_base::seekdir way, std::ios_base::openmode which) {
    // debug aid
//        string str;
//        switch(way)
//        {
//            case std::ios_base::beg:
//                str="beg"; break;
//            case std::ios_base::cur:
//                str="cur"; break;
//            case std::ios_base::end:
//                str="end"; break;
//        }
//        string strw;
//        switch(which)
//        {
//            case std::ios_base::in:
//                strw="in"; break;
//            case std::ios_base::out:
//                strw="out"; break;
//            case std::ios_base::out|std::ios_base::in:
//                strw="in|out"; break;
//        }
//                fprintf(stderr, "offIn: %d, seekdir: %s, openmode %s\n" , offIn, str.c_str(), strw.c_str());
        
        if (is_open() && way != std::ios_base::end) /* No seek with SEEK_END */
    {
      if ((which & std::ios_base::in && this->mode & std::ios::in) || /* read mode : ok */
        (which & std::ios_base::out && this->mode & std::ios::out && /* write mode : ok if */
        ((way == std::ios_base::cur && offIn >= 0) || /* SEEK_CUR with positive offset */
        (way == std::ios_base::beg && static_cast<z_off_t> (offIn) >= gztell(this->file))))) /* or SEEK_SET which go forward */ {
        z_off_t off = gzseek(this->file, static_cast<z_off_t> (offIn), (way == std::ios_base::beg ? SEEK_SET : SEEK_CUR));
//                fprintf(stderr, "Seek: %d=pubseekoff(offIn=%d,way=%s)\n", off, static_cast<z_off_t> (offIn), str.c_str());
        // GD. seems reset of buffer is needed only when rewinded at 0.
        if (which & std::ios_base::in && off == 0) setg(buffer + buf4, buffer + buf4, buffer + buf4);
        position = off;
        return off;
      } else {
        z_off_t off = static_cast<std::streampos> (gztell(this->file)); /* Just don't Seek, no error */
//                fprintf(stderr, "Just Tell: %d=pubseekoff(offIn=%d,way=%s)\n", off, static_cast<z_off_t> (offIn), str.c_str());
        position = off;
        return off;
      }
    } else if (is_open()) { //decompress, nothing else possible:
      z_off_t off=gztell(this->file);
      static char buf[32];
      int i=0;
      do {
        i=gzread(this->file,buf,32);
        if (i>0) off+=i;
      }
      while (i > 0);
      return off-1; //apparently reads a spurious EOF , maybe different on differents Oses?
    } 
//    fprintf(stderr, "EOF: -1=pubseekoff(offIn=%d,way=%s)\n", static_cast<z_off_t> (offIn), str.c_str());
    return -1;
  }
  
  std::streampos gzstreambuf::seeknext(int_type __delim) {
    if (is_open()) { //decompress and find char, nothing else possible:
      z_off_t off=gztell(this->file);
      static char buf[1];
      int i=0;
      do {
        i=gzread(this->file,buf,1);
        if (i==1 && buf[0]==__delim) break;
        if (i>0) off+=i;
      } while (i > 0);
      return off-1; // was 1 too much.
    } 
    return -1;
  }

  // --------------------------------------
  // class gzstreambase:
  // --------------------------------------

  gzstreambase::gzstreambase(const char* name, int mode) {
    init(&buf);
    open(name, mode);
  }

  gzstreambase::~gzstreambase() {
    buf.close();
  }

  void gzstreambase::open(const char* name, int open_mode) {
    if (!buf.open(name, open_mode))
      clear(rdstate() | std::ios::badbit);
  }

  void gzstreambase::close() {
    if (buf.is_open())
      if (!buf.close())
        clear(rdstate() | std::ios::badbit);
  }

  // --------------------------------------
  // class igzstream:
  // --------------------------------------

  igzstream& igzstream::seekg(std::streampos pos) {
    if (rdbuf()->pubseekpos(pos, ios_base::in) == std::streampos(-1))
      this->setstate(std::ios_base::badbit);
    else this->setstate(std::ios_base::goodbit);
    return *this;
  }

  igzstream& igzstream::seekg(std::streamoff off, std::ios_base::seekdir dir) {
    if (rdbuf()->pubseekoff(off, dir, ios_base::in) == std::streampos(-1))
      this->setstate(std::ios_base::badbit);
    else this->setstate(std::ios_base::goodbit);
    return *this;
  }

  igzstream& igzstream::ignore(std::streamsize __n) {
    if (rdbuf()->pubseekoff(__n, ios_base::cur, ios_base::in) == std::streampos(-1))
      this->setstate(std::ios_base::badbit);
    else this->setstate(std::ios_base::goodbit);
    return *this;
  }
  igzstream& igzstream::ignore(std::streamsize __n, int_type __delim) {
    if (rdbuf()->seeknext(__delim) == std::streampos(-1))
      this->setstate(std::ios_base::badbit);
    else this->setstate(std::ios_base::goodbit);
    return *this;
  }
  std::streampos igzstream::tellg(){
    return rdbuf()->pubseekoff( 0, std::ios_base::cur, std::ios_base::in);
  }

  // --------------------------------------
  // class ogzstream:
  // --------------------------------------

  ogzstream& ogzstream::seekp(std::streampos pos) {
    if (rdbuf()->pubseekpos(pos, ios_base::out) == std::streampos(-1))
      this->setstate(std::ios_base::badbit);
    else this->setstate(std::ios_base::goodbit);
    return *this;
  }

  ogzstream& ogzstream::seekp(std::streamoff off, std::ios_base::seekdir dir) {
    if (rdbuf()->pubseekoff(off, dir, ios_base::out) == std::streampos(-1))
      this->setstate(std::ios_base::badbit);
    else this->setstate(std::ios_base::goodbit);
    return *this;
  }
  std::streampos ogzstream::tellp(){
    return rdbuf()->pubseekoff( 0, std::ios_base::cur, std::ios_base::in);
  }  
#ifdef GZSTREAM_NAMESPACE
} // namespace GZSTREAM_NAMESPACE
#endif

// ============================================================================
// EOF //


