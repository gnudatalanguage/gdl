/***************************************************************************
                          ofmt.hpp  -  formatted input/output
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

#ifndef OFMT_HPP_
#define OFMT_HPP_

#include <sstream>
#include <iomanip>
#include <ostream>
#include <cmath>

#include "datatypes.hpp"
#include "dstructgdl.hpp"

// for auto formatting of float types
// code in ofmt.cpp
template< typename T>
class AsComplex 
{
#ifdef _MSC_VER
public: // MSC cannot handle friend template specialization properly
#endif
  T   flt;
  int width;
  int prec;
  char fill;

public:
  AsComplex( const T f, const int w, const int p, const char fill): flt( f), width( w), prec( p), fill( fill) 
  {}

  template< typename T2> 
  friend std::ostream& operator<<(std::ostream& os, const AsComplex<T2>& a); 
};

class CheckNL 
{
#ifdef _MSC_VER
public: // MSC cannot handle friend template specialization properly
#endif
  SizeT  width;
  SizeT* actPosPtr;
  SizeT  nextW;
  
public:
  CheckNL( const SizeT w, 
      SizeT* const a,
      const SizeT n): width( w), actPosPtr( a), nextW( n) 
  {}
  
  friend std::ostream& operator<<(std::ostream& os, const CheckNL& cc); 
};

// code in default_io.cpp
std::ostream& operator<<(std::ostream& os, const CheckNL& c);

// formatted output functions
inline void OutStars( std::ostream& os, int n)
{
  for( int i=0; i<n; ++i) os << "*";
}

template <typename T>
inline void OutFixedZero( std::ostream& os, int w, int d, char f)
{
  bool plus=( f == '+' || f == '@' );
  if ( (f == '0'|| f == '@')  ) f='0';
  if ( f == '+' ) f=' ';
   
  if( w == 1) 
    os << "*";
  else if( d >= w)
    OutStars( os, w);
  else if( plus && d >= w-1)
    OutStars( os, w);
  else {
   if (f == '0' && plus ) os << "+" << std::setw( (w-1)-(d<=0?1:d)+1) << std::setfill(f) << std::right << "0."; // preventing "00+0.00"
   else if (plus)  os << std::setw( (w-1)-(d<=0?1:d)+1) << std::setfill(f) << std::right << "+0."; 
   else  os << std::setw( w-(d<=0?1:d)+1) << std::setfill(f) << std::right << "0."; 
   for( int i=1; i<d; ++i) os << "0";
  }
}
//note strange format support for Nan and Inf:
//IDL> print,!values.d_infinity,format='(F+018)'
//+000000000Infinity
//IDL>
//
template <typename T>
inline void OutFixedNan( std::ostream& os, T val, int w, char f) //d is ignored for Nan/Inf
{
  static std::string symbol="NaN";
  int l=symbol.length();
  char s=(std::signbit(val))?'-':'+';
  bool dosign=( f == '+' || f == '@' || s=='-');
  if ( (f == '0'|| f == '@')  ) f='0';
  if ( f == '+' ) f=' ';
  if( w < (dosign?l+1:l))
    OutStars( os, w);
  else {
   if (f == '0' && dosign ) os << s << std::setw( w-1 ) << std::setfill(f) << std::right << symbol;
   else if (dosign)  os << std::setw( w-l ) << std::setfill(f)<< s << symbol; 
   else  os << std::setw( w ) << std::setfill(f) << std::right << symbol; 
  }
}
template <typename T>
inline void OutFixedInf( std::ostream& os, T val, int w, char f) //d is ignored for Nan/Inf
{
  static std::string symbol="Inf";
  int l=symbol.length();
  char s=(std::signbit(val))?'-':'+';
  bool dosign=( f == '+' || f == '@' || s=='-');
  if ( (f == '0'|| f == '@')  ) f='0';
  if ( f == '+' ) f=' ';
  if( w < (dosign?l+1:l))
    OutStars( os, w);
  else {
   if (f == '0' && dosign ) os << s << std::setw( w-1 ) << std::setfill(f) << std::right << symbol;
   else if (dosign)  os << std::setw( w-l ) << std::setfill(f)<< s << symbol; 
   else  os << std::setw( w ) << std::setfill(f) << std::right << symbol; 
  }
}
template <>
inline void OutFixedInf<DDouble>( std::ostream& os, DDouble val, int w, char f) //d is ignored for Nan/Inf
{
  static std::string symbol="Infinity";
  int l=symbol.length();
  char s=(std::signbit(val))?'-':'+';
  bool dosign=( f == '+' || f == '@' || s=='-');
  if ( (f == '0'|| f == '@')  ) f='0';
  if ( f == '+' ) f=' ';
  if( w < (dosign?l+1:l))
    OutStars( os, w);
  else {
   if (f == '0' && dosign ) os << s << std::setw( w-1 ) << std::setfill(f) << std::right << symbol;
   else if (dosign)  os << std::setw( w-l ) << std::setfill(f)<< s << symbol; 
   else  os << std::setw( w ) << std::setfill(f) << std::right << symbol; 
  }
}

inline void OutFixFill(std::ostream& os, std::string str, int w, char f)
{
  if ( (f == '0'|| f == '@')  ) f='0';
  if ( f == '+' ) f=' ';
  os << std::setfill(f); //which is '0' or blank at this point.
  if (f == '0' && ( ( str.substr(0, 1) == "-" ) || ( str.substr(0, 1) == "+" ) ) ) // preventing "00-1.00" or "00+1.00"
    os << str.substr(0, 1) << std::setw(w - 1) << str.substr(1);
  else
    os << std::setw(w) << str;
}

template <typename T>
void OutFixed( std::ostream& os, T val, int w, int d, char f)
{
  if( val == T(0.0)) // handle 0.0
    {
      if( w == 0)
	os << "0.000000";
      else
      	OutFixedZero<T>( os, w, d+1, f);
      return;
    }
  if (std::isfinite(val)) {
   std::ostringstream oss;
   if (f == '+' || f == '@') oss << std::showpos ;
   oss << std::fixed << std::setprecision(d) << val;
   if( w == 0)
     os << oss.str();
   else if( oss.tellp() > w)
     OutStars( os, w);
   else
     OutFixFill(os, oss.str(), w, f);
  } else if (isnan(val))    OutFixedNan<T>( os, val, w, f);
  else OutFixedInf<T>( os, val, w, f);
}

template <>
void OutFixed<DComplex>( std::ostream& os, DComplex val, int w, int d, char f);
template <>
void OutFixed<DComplexDbl>( std::ostream& os, DComplexDbl val, int w, int d, char f);


template <typename T>
void OutScientific(std::ostream& os, T val, int w, int d, char f) {
 if (std::isfinite(val)) {
  std::ostringstream oss;
  // TODO: IDL handles both lower and upper case "E" (tracker item no. 3147155)
  if (f == '+' || f == '@') oss << std::showpos;
//  oss << std::scientific << std::uppercase << std::setprecision(d) << val << std::nouppercase;
  oss << std::scientific << std::setprecision(d) << val;
  if (w == 0)
   os << oss.str();
  else if (oss.tellp() > w)
   OutStars(os, w);
  else
   OutFixFill(os, oss.str(), w, f);
 } else if (isnan(val)) OutFixedNan<T>(os, val, w, f);
 else OutFixedInf<T>(os, val, w, f);
}

template <>
void OutScientific<DComplex>( std::ostream& os, DComplex val, int w, int d, char f);
template <>
void OutScientific<DComplexDbl>( std::ostream& os, DComplexDbl val, int w, int d, char f);

template <typename T>
void OutAuto(std::ostream& os, T val, int w, int d, char f) {
 if (val == T(0.0)) // handle 0.0
 {
  if (f == '+' || f == '@') os << std::showpos;
  if (w == 0)
   os << "0";
  else
   OutFixedZero<T>(os, w, d, f);
  return;
 }
 if (std::isfinite(val)) {
  int powTen = static_cast<int> (std::floor(std::log10(std::abs(val))));
  SizeT fixLen = powTen > 0 ? powTen + 1 : 1; // number of digits before '.'

  std::ostringstream ossF;

  // as its used now, if w == 0 -> d != 0 (SetField())
  // static cast here is needed for OS X, without we get a *linker* error
  // (but only if GDL is compiled with Magick)

  if (f == '+' || f == '@') ossF << std::showpos;

  if (w == 0 && (powTen < d && powTen > -d) && 
    (val - std::floor(val) < std::pow(10.0, static_cast<double> (-d)))) {
   ossF << std::fixed << std::setprecision(0) << val;
  } else if (powTen == 0 || (powTen < d && powTen > -d + 1)) {
   //format for values between -1 and 1 adapts to the width as to show as many digits as possible
   ossF << std::fixed << std::setprecision(d > fixLen ? d - fixLen + ((powTen < 0) ? -powTen : 0) : 0) << val;
   if (d <= fixLen) ossF << ".";
  } else {
   fixLen = 0;  // marker to force scientific output
  } 

  std::ostringstream ossS;
  if (f == '+' || f == '@') ossS << std::showpos;
//  ossS << std::scientific << std::uppercase << std::setprecision(d > 0 ? d - 1 : 0) << val << std::nouppercase;
  ossS << std::scientific << std::setprecision(d > 0 ? d - 1 : 0) << val;

  if (fixLen == 0 || ossF.tellp() > ossS.tellp()) {
   if (w == 0) os << ossS.str();
   else if (ossS.tellp() > w) OutStars(os, w);
   else OutFixFill(os, ossS.str(), w, f);
  } else {
   if (w == 0) os << ossF.str();
   else if (ossF.tellp() > w) OutStars(os, w);
   else OutFixFill(os, ossF.str(), w, f);
  }
 }
 else if (isnan(val)) OutFixedNan<T>(os, val, w, f);
 else OutFixedInf<T>(os, val, w, f);
}

template <>
void OutAuto<DComplex>( std::ostream& os, DComplex val, int w, int d, char f);
template <>
void OutAuto<DComplexDbl>( std::ostream& os, DComplexDbl val, int w, int d, char f);

template <typename T>
std::ostream& operator<<(std::ostream& os, const AsComplex<T>& a) 
{
  os << "(";
  OutAuto( os, a.flt.real(), a.width, a.prec, a.fill);
  os << ",";
  OutAuto( os, a.flt.imag(), a.width, a.prec, a.fill);
  os << ")";
  return os;
}

#endif
