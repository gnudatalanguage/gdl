/***************************************************************************
                          str.hpp  -  basic string manipulation functions
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

#ifndef STR_HPP_
#define STR_HPP_

#include <string>
#include <sstream>
#include <functional>

#include "basegdl.hpp" // DLong

//#define GDL_DEBUG
#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include <iostream>
#endif

std::string GetEnvString(const char* env);

// convert something to string
template <typename T>
inline std::string i2s( T i, SizeT w = 0)      
{
  std::ostringstream os;
  os.width(w);
  os << i;
  return os.str();
}

// check if path is given in filename f
inline bool PathGiven(const std::string& f)
{
  if(f.substr(0,1)=="/" || f.substr(0,3)=="../" || f.substr(0,2)=="./")
    return true;
  return false;		
}

// append a if not already present
inline void AppendIfNeeded(std::string &s, const std::string& a)
{
  if( a.size() == 0) return;
  if( s.size() < a.size()) { s += a; return;}
  if( s.substr(s.size()-a.size(),a.size()) != a) s += a;
}

// Tries to find file "fn" along GDLPATH.
// If found, sets fn to the full pathname.
// and returns true, else false
// If fn starts with '/' or ".." or "./", just checks it is readable.
bool CompleteFileName(std::string& fn);

// Strip whitespace from the start and end of a string.
inline void StrTrim(std::string& s)
{
  unsigned long first=s.find_first_not_of(" \t");
  if( first == s.npos)
    {
      s="";
      return;
    }
  long last=s.find_last_not_of(" \t");
  s=s.substr(first,last-first+1);
}

DLong StrPos(const std::string& s, const std::string& searchStr, long pos, 
	     bool reverseOffset, bool reverseSearch);
std::string StrMid(const std::string& s, long first, long len, bool reverse);
std::string StrUpCase(const std::string&);
std::string StrLowCase(const std::string&);
std::string StrCompress(const std::string&,bool removeAll);
void StrPut(std::string& s1, const std::string& s2, DLong pos);

class String_abbref_eq: public std::unary_function< std::string, bool>
{
  std::string s;
public:
  explicit String_abbref_eq(const std::string& s_): s(s_) {}

  bool operator() (const std::string& p) const 
  {
    return p.substr(SizeT(0),s.size()) == s;
  }
};


double Str2D( const char* c);
double Str2D( const std::string& s);
long int Str2L( const char* c, int base=10);
long int Str2L( const std::string& s, int base=10);
unsigned long int Str2UL( const char* c, int base=10);
unsigned long int Str2UL( const std::string& s, int base=10);


#endif
