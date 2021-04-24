
/***************************************************************************
                          str.cpp  -  basic string manipulation functions
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include <cstdlib>

// that's enough with Cygwin >= 1.7.1 
//#ifdef __CYGWIN__
//extern "C" {
//#endif
//
// GJ 2018 April: I believe accomodating __CYGWIN__ is no longer required.
// although __OpenBSD__ may be.
  // quoting http://permalink.gmane.org/gmane.os.openbsd.tech/19860 :
  // 'wordexp() will never be in OpenBSD's libc' :)
  // (TODO: perhaps better to implement it using HAVE_WORDEXP_H? + once more below in WordExp())
#if (!defined(__OpenBSD__) && !defined(_WIN32)) //|| defined(__CYGWIN__)
#  include <wordexp.h>
#endif

//#ifdef __CYGWIN__
//}
//#endif

#include "str.hpp"
#include "gdlexception.hpp"
#include "initsysvar.hpp" // GDLPath();
namespace lib {
  std::string PathSeparator()
  {
#ifdef _WIN32
    if (lib::posixpaths) return std::string ("/");
    return std::string ("\\");
#else
    return std::string ("/");
#endif
  }
  
  std::string SearchPathSeparator()
  {
#ifdef _WIN32
    return std::string (";");
#else
    return std::string (":");
#endif
  }
  
  std::string ParentDirectoryIndicator()
  {
    return std::string ("..");
  }
}
//using namespace std;
//namespace lib {
//bool trace_arg();
//}
std::string GetEnvString(const char* env)
{
  char* c=getenv(env);
  if( !c) return std::string("");
  return std::string(c);
}

DLong StrPos(const std::string& s, const std::string& searchStr, long pos, 
	     bool reverseOffset, bool reverseSearch)
{
  if( s == "") return -1;

  long strLen = s.length();
  if( pos == std::string::npos)
    {
      if( reverseSearch || reverseOffset)
	pos = strLen - 1;
      else
	pos = 0;
    }
  else
    {
      if( pos < 0) pos = 0;
      if( reverseOffset)
	pos = strLen - pos - 1;
    }

  if( searchStr == "")
    {
      if( pos >= strLen)
	return strLen - 1; 
      else 
	{
	  if( reverseSearch && pos < 0) return 0;
	  return pos;
	}
    }
  if( pos < 0) return -1;

  std::string::size_type res;
  if( reverseSearch)
    {
      res = s.rfind( searchStr, pos);
      if( res == std::string::npos) return -1;
    }
  else
    {
      res = s.find( searchStr, pos);
      if( res == std::string::npos) return -1;
    }

  return res;
}
//#define STRMID_DEBUG 
std::string StrMid(const std::string& s, long first, long len, bool reverse)
{
#ifdef STRMID_DEBUG
  std::cout << "DebugInfo: StrMid(\"" << s << "\"," << first <<","<<len<<","<<reverse<<") = ";//<<std::endl
#endif

  // (long)std::string::npos == -1
  if( len != std::string::npos && len <= 0)
    {
#ifdef STRMID_DEBUG
      std::cout << "." << std::endl;
#endif
      return std::string("");
    }
  long strLen = s.length();
  if( reverse)
    {
      if( first < 0) return std::string("");
      first = strLen - first -1;
    }

  if( first >= strLen) return std::string("");
  if( first < 0) first = 0; 

#ifdef STRMID_DEBUG
  std::cout << s.substr( first, len)<<"." << std::endl;
#endif
  return s.substr( first, len);
}

std::string StrCompress(const std::string& s, bool removeAll)
{
  SizeT strLen = s.length();
  if( strLen == 0) return std::string("");

  std::string res;

  if( removeAll)
    {
      for( SizeT i=0; i<strLen; ++i)
	if( s[i] != ' ' && s[i] != '\t') res += s[i];
      return res;
    }

  SizeT actPos = 0;
  while( actPos < strLen)
    {
      SizeT first = s.find_first_not_of(" \t",actPos);
      if( first >= strLen)
	{
	  res += " ";
	  break;
	}
      if( first != actPos) res += " ";

      SizeT last  = s.find_first_of(" \t",first);
      if( last >= strLen) last = strLen;
      actPos = last;

      res += s.substr( first, last-first);
    }

  return res;
}

void StrPut(std::string& s1, const std::string& s2, DLong pos)
{
  unsigned len1=s1.length();
  unsigned len2=s2.length();
  if( pos >= len1) return;
  unsigned n = (len1 > pos+len2) ? len2 : len1-pos;
  s1.replace( pos, n, s2, 0, n);
}

std::string StrUpCase(const std::string& s)
{
  unsigned len=s.length();
  char const *sCStr=s.c_str();
  char* r = new char[len+1];
  ArrayGuard<char> guard( r);
  r[len]=0;
  for(unsigned i=0;i<len;i++)
  r[i]=std::toupper(sCStr[i]);
  return std::string(r);
}
void StrUpCaseInplace( std::string& s)
{
  unsigned len=s.length();
  //   char const *sCStr=s.c_str();
  //   char* r = new char[len+1];
  //   ArrayGuard<char> guard( r);
  //   r[len]=0;
  for(unsigned i=0;i<len;i++)
  s[i]=std::toupper(s[i]);
}

std::string StrLowCase(const std::string& s)
{
  unsigned len=s.length();
  char const *sCStr=s.c_str();
  char* r = new char[len+1];
  ArrayGuard<char> guard( r);
  r[len]=0;
  for(unsigned i=0;i<len;i++)
  r[i]=std::tolower(sCStr[i]);
  return std::string(r);
}
void StrLowCaseInplace(std::string& s)
{
  unsigned len=s.length();
  //   char const *sCStr=s.c_str();
  for(unsigned i=0;i<len;i++)
  s[i]=std::tolower(s[i]);
}

// replacement for library routine 
// double strtod( const char* cStart, char** cEnd);
// to hanlde d/D instead of e/E (e. g. 1.2D5)
// this is done very slow by copying the string and replacing the d/D with e/E
// however, it is done only, if strtod stops at a 'd' or 'D' character 
double StrToD( const char* cStart, char** cEnd)
{
  double ret = strtod( cStart, cEnd);
  if( cEnd != NULL && (**cEnd == 'd' || **cEnd == 'D'))
    {
      int dPos = *cEnd - cStart;      

      // copy the string here. This is very slow.
      // but the glibc implementation is hidden
      // I have not investigated further, but I assume this is because processor specific
      // optimizations are used. So it might be ok to copy the string here as in the regular
      // case the optimzed strtod function will make up for the loss.
      std::string cStr( cStart);

      // replace d by e and D by E
      cStr[dPos] = (**cEnd == 'd')? 'e':'E';

      char* cEndD;
      const char* cStrc_str = cStr.c_str();
      
      double retD = strtod( cStrc_str, &cEndD);      
      
      // set end as if orignal string had the d/D replaced
      *cEnd = const_cast<char*>(cStart) + (cEndD - cStrc_str);      
      // return replaced result
      return retD;
    }
  return ret;
}
double Str2D( const char* cStart)
{
  char* cEnd;
  double ret = strtod( cStart, &cEnd);
  if( cEnd == cStart)
    {
      Warning("Type conversion error: "
	      "Unable to convert given STRING: '"+std::string(cStart)+"' to DOUBLE.");
    }
  return ret;
}
long int Str2L( const char* cStart, int base)
{
  char* cEnd;
  long int ret = strtol( cStart, &cEnd, base);
  if( cEnd == cStart)
    {
      Warning("Type conversion error: "
	      "Unable to convert given STRING: '"+std::string(cStart)+"' to LONG.");
    }
  return ret;
}
unsigned long int Str2UL( const char* cStart, int base)
{
  char* cEnd;
  unsigned long int ret = strtoul( cStart, &cEnd, base);
  if( cEnd == cStart)
    {
      Warning("Type conversion error: "
	      "Unable to convert given STRING: '"+std::string(cStart)+"' to ULONG.");
    }
  return ret;
}

double Str2D( const std::string& s)
{
  const char* cStart = s.c_str();
  return Str2D( cStart);
}
long int Str2L( const std::string& s, int base)
{
  const char* cStart = s.c_str();
  return Str2L( cStart, base);
}
unsigned long int Str2UL( const std::string& s, int base)
{
  const char* cStart = s.c_str();
  return Str2UL( cStart, base);
}
#ifdef _WIN32

void WordExp(std::string& s)
{
  if (s.length() == 0) return;
  bool trace_me = false;

  std::string sEsc = "";
  int ipos = 0;
  if (s[ipos] == '~') {
    char* homeDir = getenv("HOME");
    ipos++;
    if (homeDir == NULL) homeDir = getenv("HOMEPATH");

    if (homeDir != NULL)
      sEsc = std::string(homeDir) + "/";
  }
  for (int i = ipos; i < s.length(); ++i) {
    char achar = s[i];
    if (achar != '$') sEsc.push_back(achar);
    else { // $
      std::string name = "";
      for (int ind = i + 1; i < s.length(); ++ind) {
        char tchar = s[ind];
        if (tchar == ' ' or tchar == '/' or
          tchar == '\\' or tchar == ':') break;
        name.push_back(tchar);
      }
      char* subst = getenv(name.c_str());
      if (subst != NULL) {
        sEsc += std::string(subst);
        i += name.length();
      } else sEsc.push_back(achar);
    }
  }
  if (trace_me) std::cout << "WordExp  in: " << s << " -(modified original)- WordExp esc: " << sEsc << std::endl;
  s = sEsc;
}
#endif

#ifndef _WIN32

void WordExp(std::string& s)
{
  //AC 2018-04-25 : because crash of :  // openr, unit, '', ERROR=error,/get_lun
  if (s.length() == 0) return;
  bool trace_me = false; //lib::trace_arg();

  std::string sEsc = "";
  int ipos = 0;
  // escape blanks
  for (int i = ipos; i < s.length(); ++i) {
    char achar = s[i];
    if (achar == ' ') sEsc += std::string("\\ ");
    else if (achar == '\\') {
      if ((i + 1) < s.length()) {
        if (s[i + 1] == ' ') {
          sEsc += std::string("\\ ");
          ++i;
        }
      }
    }
#ifdef __OpenBSD__
      //in the case of OpenBSD, try to expand at least simple things like $HOME as we do not have wordexp() available.
    else if (achar != '$') sEsc.push_back(achar);
    else { // $
      std::string name = "";
      for (int ind = i + 1; i < s.length(); ++ind) {
        char tchar = s[ind];
        if (tchar == ' ' or tchar == '/' or tchar == '\\' or tchar == ':') break;
        name.push_back(tchar);
      }
      char* subst = getenv(name.c_str());
      if (subst != NULL) {
        sEsc += std::string(subst);
        i += name.length();
      } else sEsc.push_back(achar);
    }
  }
#else 
    else sEsc.push_back(achar);
  }
  //after blank escaping we refer to wordexp to manage the (eventually complicated) expansions.    
  wordexp_t p;
// std::cerr<<sEsc<<std::endl;
  int ok0 = wordexp(sEsc.c_str(), &p, WRDE_NOCMD);
  if (ok0 == 0) {
    if (p.we_wordv[0] !=NULL) s=std::string(p.we_wordv[0]);
#if defined(__APPLE__)
    p.we_offs = 0;
#endif
    wordfree(&p);
  }
#endif
  if (trace_me) std::cout << "WordExp  in: " << s << " -(modified original)- WordExp esc: " << sEsc << std::endl;
}

#endif //not def WIN32

#if defined (_WIN32)
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH) 
// ref:http://sourceforge.net/p/mingw/patches/256/ Keith Marshall 2005-12-02
#endif

#include <climits> // PATH_MAX

#ifdef _MSC_VER
#define PATH_MAX _MAX_PATH
#endif
//patch #90
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
std::string FullPathFileName(std::string in_file)
{
  
  std::string AbsolutePath;

  char *symlinkpath =const_cast<char*> (in_file.c_str());
  char actualpath [PATH_MAX+1];
  char *ptr;

  ptr = realpath(symlinkpath, actualpath);
  if( ptr != NULL ){
    AbsolutePath =std::string(ptr);
  }else {
    AbsolutePath = in_file;
  }
 
#ifdef GDL_DEBUG
    std::cout << in_file << std::endl;
    std::cout << AbsolutePath << std::endl;
#endif
  return AbsolutePath;

}

// Tries to find file "fn" along GDLPATH.
// If found, sets fn to the full pathname.
// and returns true, else false
// 
// this line is no more true : "If fn starts with '/' or ".." or "./", just checks if it is readable."
// new since AC 11-Sept-2014: we return the absolute path, this is needed
// for outputs in various procedures:
// GDL> HELP, /source  ou HELP, /traceback
// GDL> print, ROUTINE_INFO('dist',/function,/source)

bool CompleteFileName(std::string& fn)
{
  WordExp( fn);

  // try actual directory (or given path)
  FILE *fp = fopen(fn.c_str(),"r");
  if(fp)
    {
      fclose(fp);
      fn=FullPathFileName(fn);
      return true;
    }

  if( PathGiven(fn)) return false;

  StrArr path=SysVar::GDLPath();
  if( path.size() == 0) 
  {
    std::string act = "./pro/"; // default path if no path is given

#ifdef GDL_DEBUG
    std::cout << "Looking in:" << std::endl;
    std::cout << act << std::endl;
#endif

    act = act + fn;
    fp = fopen(act.c_str(), "r");
    if (fp) {
      fclose(fp);
      fn = act;
      fn = FullPathFileName(fn);
      return true;
    }
  }
  else
    for(unsigned p=0; p<path.size(); p++)
      {
	std::string act=path[p];
	
	AppendIfNeeded(act,lib::PathSeparator());
	
#ifdef GDL_DEBUG
	if( p == 0) std::cout << "Looking in:" << std::endl;
	std::cout << act << std::endl;
#endif

	act=act+fn;
	fp = fopen(act.c_str(),"r");
	if(fp) {
	  fclose(fp);
	  fn=act;
	  fn=FullPathFileName(fn);
	  return true;
	}
      }
  return false;
}
