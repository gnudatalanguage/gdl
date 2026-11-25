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

#include <algorithm>
#include <cstdio> // std::remove(...)
#include <limits>
#include "objects.hpp"
#include "io.hpp"
#ifdef __MINGW32__
#include <unistd.h> // for close()
#endif

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <iostream>
#include <ext/stdio_filebuf.h>// TODO: is it portable across compilers?
using __gnu_cxx::stdio_filebuf;

namespace {
// Keep the logical output offset even though the child process pipe cannot seek.
class PipeOutputBuffer final : public std::streambuf {
  stdio_filebuf<char> pipe;
  std::streamoff written = 0;

public:
  explicit PipeOutputBuffer(int fd) : pipe(fd, std::ios::out) {}

protected:
  std::streamsize xsputn(const char* data, std::streamsize size) override {
    const auto count = pipe.sputn(data, size);
    written += count;
    return count;
  }

  int_type overflow(int_type c) override {
    if (traits_type::eq_int_type(c, traits_type::eof())) return traits_type::not_eof(c);
    const auto result = pipe.sputc(traits_type::to_char_type(c));
    if (!traits_type::eq_int_type(result, traits_type::eof())) ++written;
    return result;
  }

  int sync() override { return pipe.pubsync(); }

  pos_type seekoff(off_type offset, std::ios::seekdir dir, std::ios::openmode which) override {
    if ((which & std::ios::out) && dir == std::ios::cur && offset == 0) return written;
    return pos_type(-1);
  }
};
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
  lastFileAccess = FileAccess::Untracked;
  fStream.reset();
  ofStream.reset();
  pipeInput.reset();
  pipeOutput.reset();
  igzStream.reset();
  ogzStream.reset();
}

void AnyStream::OpenAsPipes(const std::string& name_, std::ios_base::openmode mode_,
                            int pipeInFd, int pipeOutFd) {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
  // GNU stdio_filebuf owns the descriptor passed to its constructor.
  auto input = (mode_ & ios::in) ? std::make_unique<stdio_filebuf<char>>(pipeInFd, ios::in) : nullptr;
  auto output = (mode_ & ios::out) ? std::make_unique<PipeOutputBuffer>(pipeOutFd) : nullptr;
  auto openPipe = [](std::streambuf* buffer, ios::openmode mode) {
    std::unique_ptr<fstream> stream;
    if (buffer) {
      stream = std::make_unique<fstream>("/dev/null", mode);
      if (!*stream) throw GDLIOException(-1, "Error opening pipe stream.");
      stream->std::ios::rdbuf(buffer);
    }
    return stream;
  };
  auto in = openPipe(input.get(), ios::in);
  auto out = openPipe(output.get(), ios::out);
  pipeInput = std::move(input);
  pipeOutput = std::move(output);
  fStream = std::move(in);
  ofStream = std::move(out);
#else
  throw GDLException("This command relies on GNU extensions to the std C++ library that were not available during compilation on your system (?)");
#endif
}

void AnyStream::Open(const std::string& name_, ios_base::openmode mode_, bool compress_) {
  if (compress_) {
    if (mode_ & ios::out) {
      auto stream = std::make_unique<ogzstream>();
      stream->open(name_.c_str(), mode_ & ~ios::in);
      if (!*stream) throw GDLIOException("Error opening compressed file for output.");
      ogzStream = std::move(stream);
    } else {
      auto stream = std::make_unique<igzstream>();
      stream->open(name_.c_str(), mode_);
      if (!*stream) throw GDLIOException("Error opening compressed file for input.");
      igzStream = std::move(stream);
    }
    return;
  }

  auto stream = std::make_unique<fstream>(name_.c_str(), mode_);
  if (!*stream) throw GDLIOException(-1, "Error opening file.");
  if ((mode_ & ios::in) && (mode_ & ios::out) &&
      stream->rdbuf()->pubseekoff(0, ios::cur) != std::streampos(-1))
    lastFileAccess = FileAccess::Initial;
  fStream = std::move(stream);
}

std::fstream& AnyStream::FileFor(FileAccess access) {
  if (lastFileAccess != FileAccess::Untracked) {
    // An update stream needs a positioning operation when changing direction.
    // Seek at the logical position so buffered read-ahead is accounted for.
    if (lastFileAccess != FileAccess::Initial && lastFileAccess != access &&
        fStream->rdbuf()->pubseekoff(0, ios::cur) == std::streampos(-1))
      throw GDLIOException("Error positioning file when switching between reading and writing.");
    lastFileAccess = access;
  }
  return *fStream;
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
    FileFor(FileAccess::Write).write(buf, nBytes);
  } else if (ogzStream != NULL) {
    ogzStream->write(buf, nBytes);
  } else assert(0);
}

void AnyStream::Read(char* buf, std::streamsize nBytes) {
  if (fStream != NULL) {
    FileFor(FileAccess::Read).read(buf, nBytes);
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

    if (pipeInput) return true;
    else {
      std::istream::int_type c = FileFor(FileAccess::Read).peek();  // peek character, test EOF

  if ( c == std::char_traits<char>::eof() )  {fStream->clear(); return true;}
  else return false;
    }
  }

  if (igzStream != NULL) {
    igzStream->clear(); // clear old EOF	

    std::istream::int_type c = igzStream->peek(); // peek character, test EOF

    if (c == std::char_traits<char>::eof())   {igzStream->clear(); return true;}
    else return false;
  }
  return true;
}

void AnyStream::Seek(std::streampos pos) {
  if (!InUse()) throw GDLException("inner file unit is not open.");

  if (fStream != NULL) {
    if (fStream->eof())
      fStream->clear();
    // AC related to bug #1474
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
    FileFor(FileAccess::Read).ignore(pos);
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
    std::fstream& input = FileFor(FileAccess::Read);
    for (; i < nlines; ++i) {
      input.ignore(std::numeric_limits<ptrdiff_t>::max(), '\n');
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
    std::streampos cur = ofStream->tellg(); 
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
  if (fStream != NULL) return fStream->tellp();
  else if (ofStream != NULL) return ofStream->tellg();
  else if (igzStream != NULL)
    return igzStream->tellg();
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

    const auto current = ofStream->tellp();
    if (current == std::streampos(-1) || pos < current)
      throw GDLIOException("Cannot seek backwards on an output pipe.");
    if (pos > current) Pad(pos - current);
    if (!*ofStream) throw GDLIOException("Error padding an output pipe.");
  } else 
    if (fStream != NULL) {
    if (fStream->eof())
      fStream->clear();

    // Append the gap after the existing records.
    auto& output = FileFor(FileAccess::Write);
    output.seekp(0, std::ios::end);
    const auto end = output.tellp();
    if (end == std::streampos(-1)) throw GDLIOException("Cannot find the end of the file.");
    if (pos > end) Pad(pos - end);
    else output.seekp(pos);
    if (!output) throw GDLIOException("Error positioning file for record write.");
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

  if (anyStream == NULL) anyStream = std::make_unique<AnyStream>();

  name = name_;
  mode = mode_;
  compress = false;
  f77 = false;
  varlenVMS = false;
  anyStream->OpenAsPipes(name, mode_, pipeInFd, pipeOutFd );

  swapEndian = false;
  deleteOnClose = false; // A pipe's display name is not a file to unlink.

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

  if (anyStream != NULL && anyStream->IsOpen())
    throw GDLIOException("File unit is already open.");

  if (anyStream == NULL)
    anyStream = std::make_unique<AnyStream>();

  auto xdr = xdr_ ? std::make_unique<XDR>() : nullptr;
  anyStream->Open(expName, mode_, compress_);

  name = std::move(expName);
  mode = mode_;
  compress = compress_;
  f77 = f77_;
  xdrs = std::move(xdr);

  swapEndian = swapEndian_;
  deleteOnClose = dOC;


  lastSeekPos = 0;
  lastRecord = 0;
  lastRecordStart = 0;
  width = width_;
}

#ifdef __MINGW32__

// Use obsolete code, as I've no time to adapt new code to Windows.

void GDLStream::Socket(const string& host,
    DUInt port, bool swapEndian_,
    DDouble c_timeout_, DDouble r_timeout_,
    DDouble w_timeout_, SizeT width) {
  if (iSocketStream == NULL)
    iSocketStream = std::make_unique<istringstream>();

  if (recvBuf == NULL)
    recvBuf = std::make_unique<string>();

  name = host + "." + i2s(port);;

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

  memcpy(&m_addr.sin_addr, h->h_addr_list[0], sizeof(m_addr.sin_addr));
  if (connect(sockNum, (sockaddr *) &m_addr, sizeof(m_addr)) != 0)
    throw GDLIOException("Unable to connect to host.");

  swapEndian = swapEndian_;

  // GD: ?????? // BIG limit on socket send width to avoid leading \n in CheckNL
  //width = 32768;
}
#else
//New code suppressing call to obsolete functions
#include <fcntl.h>
void GDLStream::Socket(const string& host,
  DUInt port, bool swapEndian_,
  DDouble c_timeout_, DDouble r_timeout_,
  DDouble w_timeout_, SizeT width) {
  if (iSocketStream == NULL)
    iSocketStream = std::make_unique<istringstream>();

  if (recvBuf == NULL)
    recvBuf = std::make_unique<string>();

  const std::string service = i2s(port);
  name = host + "." + service;

  addrinfo hints{};
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  addrinfo* result = nullptr;
  const int lookupError = getaddrinfo(host.c_str(), service.c_str(), &hints, &result);
  if (lookupError != 0) {
    std::cerr << gai_strerror(lookupError) << std::endl;
    throw GDLIOException("Unable to lookup host.");
  }
  std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> addresses(result, &freeaddrinfo);

  sockNum = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

  c_timeout = c_timeout_;
  r_timeout = r_timeout_;
  w_timeout = w_timeout_;

  int on = 1;
  if (setsockopt(sockNum, SOL_SOCKET, SO_REUSEADDR,
    (const char*) &on, sizeof (on)) == -1) {
    throw GDLIOException("Error opening file.");
  }

  if (connect(sockNum, addresses->ai_addr, addresses->ai_addrlen) != 0)
    throw GDLIOException("Unable to connect to host.");
  int flags = fcntl(sockNum, F_GETFL, 0); 
  fcntl(sockNum,F_SETFD, flags|FD_CLOEXEC);
  swapEndian = swapEndian_;

// GD ?????  // BIG limit on socket send width to avoid leading \n in CheckNL
//  width = 32768;
}
#endif

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
  f77 = false;
  swapEndian = false;
  compress = false;
  deleteOnClose = false;

  xdrs.reset();

  width = defaultStreamWidth;
  //Do not forget sockets!
  if (sockNum != -1) {
#ifdef __MINGW32__
    closesocket(sockNum);
#else
    close(sockNum);
#endif
  }
  sockNum = -1;
  c_timeout = 0.0;
  r_timeout = 0.0;
  w_timeout = 0.0;
}

void GDLStream::Free() {
  Close();

  anyStream.reset();

  iSocketStream.reset();

  recvBuf.reset();

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
  return anyStream->FileFor(AnyStream::FileAccess::Read);
}

fstream& GDLStream::OStream() { //OStream may be oFStream if oFStream is not NULL (different pipe)
  if (anyStream == NULL || !anyStream->IsOpen()) throw GDLIOException("File unit is not open.");
  if (anyStream->oFStream() != NULL) { //process special pipe
    return *anyStream->oFStream();
  }
  // old behaviour for normal files
  if (anyStream->FStream() == NULL) throw GDLIOException("File unit is not open.");
  if (!(mode & ios::out))  throw GDLIOException("File unit is not open for writing.");
  return anyStream->FileFor(AnyStream::FileAccess::Write);
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
    for (SizeT i = 0; i < nBuf && *ofStream; ++i) ofStream->write(buf, bufSize);
    if (lastBytes > 0) ofStream->write(buf, lastBytes);
  } else 
    if (fStream != NULL) {
    std::fstream& output = FileFor(FileAccess::Write);
    for (SizeT i = 0; i < nBuf && output; ++i) output.write(buf, bufSize);
    if (lastBytes > 0) output.write(buf, lastBytes);
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
  const std::streamoff length = currentPos;
  if (length < 0 || static_cast<DULong64>(length) > std::numeric_limits<size_t>::max())
    throw GDLIOException("Invalid position for truncating file: " + name);
  // Zero-initialized: POINT_LUN may have positioned the unit beyond the end of the file.
  auto storage = std::make_unique<char[]>(static_cast<size_t>(length));
  char* buffer = storage.get();
  anyStream->Seek(0);
  if (length > 0) {
    anyStream->Read(buffer, length);
    if (!anyStream->Good() && !anyStream->EofRaw())
      throw GDLIOException("Error reading file before truncation: " + name);
  }
  anyStream->Close();
  anyStream->Open(name, fstream::out | fstream::trunc, compress);
  lastSeekPos = 0;
  lastRecord = 0;
  lastRecordStart = 0;
  if (length > 0) {
    anyStream->Write(buffer, length);
    anyStream->Flush();
    if (!anyStream->Good()) throw GDLIOException("Error writing truncated file: " + name);
  }
  anyStream->Close();
  anyStream->Open(name, mode & ~fstream::trunc, compress); //write again but not trunc!
  anyStream->Seek(currentPos);
  lastSeekPos = anyStream->Tell();
}

//should work with compressed files too
DLong64 GDLStream::CopySomeTo(DLong64 nbytes, GDLStream& to, bool doThrow) {
  if (nbytes < 0) throw GDLIOException("Invalid byte count for COPY_LUN.");
  const DLong64 chunkSize = 1000000;
  auto storage = std::make_unique<char[]>(static_cast<size_t>(std::min(nbytes, chunkSize)));
  char* buffer = storage.get();
  DLong64 copied = 0;
  while (copied < nbytes) {
    const auto requested = std::min(nbytes - copied, chunkSize);
    anyStream->Read(buffer, requested);
    const auto count = anyStream->Gcount();
    if (count > 0) {
      to.anyStream->Write(buffer, count);
      if (!to.anyStream->Good()) throw GDLIOException("Error writing file: " + to.name);
      copied += count;
    }
    if (count < requested) {
      if (!anyStream->EofRaw()) throw GDLIOException("Error reading file: " + name);
      if (doThrow) throw GDLException("End of file encountered. File: " + name);
      break;
    }
  }
  return copied;
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
    inputBufferEnd = 0;
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
    memmove(buffer + (buf4 - n_putback), gptr() - n_putback, n_putback);

    int num = gzread(file, buffer + buf4, bufferSize - buf4);
    if (num <= 0) {
      // Only the saved putback bytes remain available after this refill.
      setg(buffer + buf4 - n_putback, buffer + buf4, buffer + buf4);
      return EOF;
    }

    inputBufferEnd += num;
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

  std::streampos gzstreambuf::seekpos(std::streampos sp, std::ios_base::openmode which) {
    if (!is_open()) return -1;
    const bool reading = (which & std::ios::in) && (mode & std::ios::in);
    // READF backs up one character after a numeric field, often once per line.
    if (reading && sp >= std::streampos(inputBufferEnd - (egptr() - eback())) && sp <= std::streampos(inputBufferEnd)) {
      gbump(static_cast<int>(sp - inputPosition()));
      return sp;
    }
    if (sp < std::streampos(0) || sp > std::streampos(std::numeric_limits<z_off_t>::max())) return -1;
    if (reading) {
      const auto off = gzseek(file, static_cast<z_off_t>(sp), SEEK_SET);
      if (off >= 0) {
        setg(buffer + buf4, buffer + buf4, buffer + buf4);
        inputBufferEnd = off;
      }
      return off;
    }
    if (!(which & std::ios::out) || !(mode & std::ios::out)) return -1;
    const auto current = outputPosition();
    if (sp <= current) return current; // Output streams only seek forward.
    if (sync() != 0) return -1;
    return gzseek(file, static_cast<z_off_t>(sp), SEEK_SET);
  }

  std::streampos gzstreambuf::seekoff(std::streamoff offset, std::ios_base::seekdir way, std::ios_base::openmode which) {
    if (!is_open()) return -1;
    if ((which & std::ios::in) && (mode & std::ios::in)) {
      if (way == std::ios::beg) return pubseekpos(offset, which);
      if (way == std::ios::end) {
        // zlib cannot seek relative to the end of the uncompressed data.
        while (sgetc() != traits_type::eof()) gbump(static_cast<int>(egptr() - gptr()));
        if (!gzeof(file)) return -1;
      }
      const auto current = inputPosition();
      return offset == 0 ? current : pubseekpos(current + offset, which);
    }
    if (!(which & std::ios::out) || !(mode & std::ios::out)) return -1;
    if (way == std::ios::beg) return pubseekpos(offset, which);
    const auto current = outputPosition(); // The writable gzip stream is always at its end.
    return offset == 0 ? current : pubseekpos(current + offset, which);
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

  std::streampos igzstream::tellg() {
    return rdbuf()->inputPosition();
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
    return rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out);
  }  
#ifdef GZSTREAM_NAMESPACE
} // namespace GZSTREAM_NAMESPACE
#endif

// ============================================================================
// EOF //


