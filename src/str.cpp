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

#include <wordexp.h>

#include "str.hpp"
#include "gdlexception.hpp"
#include "initsysvar.hpp" // GDLPath();

using namespace std;

string GetEnvString(const char* env)
{
    char* c=getenv(env);
    if( !c) return string("");
    return string(c);
}

DLong StrPos(const string& s, const string& searchStr, long pos, 
	      bool reverseOffset, bool reverseSearch)
{
  if( s == "") return -1;

  long strLen = s.length();
  if( pos == string::npos)
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
    if( pos >= strLen) 
      return strLen - 1; 
    else 
      {
	if( reverseSearch && pos < 0) return 0;
	return pos;
      }
  
  if( pos < 0) return -1;

  string::size_type res;
  if( reverseSearch)
    {
      res = s.rfind( searchStr, pos);
      if( res == string::npos) return -1;
    }
  else
    {
      res = s.find( searchStr, pos);
      if( res == string::npos) return -1;
    }

  return res;
}

string StrMid(const string& s, long first, long len, bool reverse)
{
  if( len != string::npos && len <= 0) return string("");

  long strLen = s.length();
  if( reverse)
    {
      if( first < 0) return string("");
      first = strLen - first -1;
    }

  if( first >= strLen) return string("");
  if( first < 0) first = 0; 

  return s.substr( first, len);
}

string StrCompress(const string& s, bool removeAll)
{
  SizeT strLen = s.length();
  if( strLen == 0) return string("");

  string res;

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

string StrUpCase(const string& s)
{
  unsigned len=s.length();
  char const *sCStr=s.c_str();
  char* r = new char[len+1];
  ArrayGuard<char> guard( r);
  r[len]=0;
  for(unsigned i=0;i<len;i++)
    r[i]=toupper(sCStr[i]);
  return string(r);
}

string StrLowCase(const string& s)
{
  unsigned len=s.length();
  char const *sCStr=s.c_str();
  char* r = new char[len+1];
  ArrayGuard<char> guard( r);
  r[len]=0;
  for(unsigned i=0;i<len;i++)
    r[i]=tolower(sCStr[i]);
  return string(r);
}

double Str2D( const char* cStart)
{
  char* cEnd;
  double ret = strtod( cStart, &cEnd);
  if( cEnd == cStart)
    {
      Warning("Type conversion error: "
	      "Unable to convert given STRING to DOUBLE.");
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
	      "Unable to convert given STRING to LONG.");
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
	      "Unable to convert given STRING to ULONG.");
    }
  return ret;
}

double Str2D( const string& s)
{
  const char* cStart = s.c_str();
  return Str2D( cStart);
}
long int Str2L( const string& s, int base)
{
  const char* cStart = s.c_str();
  return Str2L( cStart, base);
}
unsigned long int Str2UL( const string& s, int base)
{
  const char* cStart = s.c_str();
  return Str2UL( cStart, base);
}

void WordExp( string& s)
{
  wordexp_t p;
  int ok0 = wordexp( s.c_str(), &p, 0);
  if( ok0 == 0) 
    {
      if( p.we_wordc > 0)
	s = p.we_wordv[0];
      wordfree( &p);
    }
}

// Tries to find file "fn" along GDLPATH.
// If found, sets fn to the full pathname.
// and returns true, else false
// If fn starts with '/' or ".." or "./", just checks it is readable.
bool CompleteFileName(string& fn)
{
  WordExp( fn);

  // try actual directory (or given path)
  FILE *fp = fopen(fn.c_str(),"r");
  if(fp)
    {
      fclose(fp);
      return true;
    }

  if( PathGiven(fn)) return false;

  StrArr path=SysVar::GDLPath();
  if( path.size() == 0)
    {
      string act="./pro/"; // default path if no path is given
	
#ifdef GDL_DEBUG
      cout << "Looking in:" << endl;
      cout << act << endl;
#endif

      act=act+fn;
      fp = fopen(act.c_str(),"r");
      if(fp) {fclose(fp); fn=act; return true;}
    }
  else
    for(unsigned p=0; p<path.size(); p++)
      {
	string act=path[p];
	
	AppendIfNeeded(act,"/");
	
#ifdef GDL_DEBUG
	if( p == 0) cout << "Looking in:" << endl;
	cout << act << endl;
#endif

	act=act+fn;
	fp = fopen(act.c_str(),"r");
	if(fp) {fclose(fp); fn=act; return true;}
      }
  return false;
}
