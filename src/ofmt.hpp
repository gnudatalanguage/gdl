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
#include <bitset> // for binary output
//using namespace std;

#include "datatypes.hpp"
#include "dstructgdl.hpp"

#ifdef _MSC_VER
#define finite _finite
#define isnan _isnan
#endif

typedef enum codeFlags_
  {
    fmtALIGN_LEFT = 1
   ,fmtSHOWPOS = 2
   ,fmtPAD = 4
   ,fmtUPPER = 8
  } codeFlags;
  


  
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
  int code;
public:
  AsComplex( const T val, const int w, const int p, const int c=0): flt( val), width( w), prec( p), code( c)
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
inline void OutStars( std::ostream& os, const int n)
{
  for( int i=0; i<n; ++i) os << "*";
}

inline void OutFixedStringVal( std::ostream& os, const std::string &symbol, const char s, int w, const int code) {
  int l=symbol.length();
  bool dosign=( code & fmtSHOWPOS || s=='-');
  bool dofill=( code & fmtPAD );
  if (w <=0) w=(dosign?l+1:l);
  if( w < (dosign?l+1:l)) { OutStars( os, w); return;}
  if (code & fmtALIGN_LEFT) 
  {
   os << std::left;
   if (dosign) {os << s << std::setw( w-1 ) << symbol ; return;}
   os << std::setw( w ) << symbol;  return;
  }
  if (dofill && dosign ) { os << s << std::setw( w-1 ) << std::setfill('0') << std::right << symbol << std::setfill(' ') ; return;}
  if (dofill) { os << std::setw( w ) << std::setfill('0') << std::right << symbol << std::setfill(' ') ; return;}
  if (dosign)  { os << std::setw( w-l ) << std::right << s << symbol; return;}
  os << std::setw( w ) << std::right << symbol; return;
}

template <typename T>
inline void OutFixedNan( std::ostream& os, const T val, const int w, const int code) //d is ignored for Nan/Inf
{
  static std::string symbol="NaN";
  char s=(std::signbit(val))?'-':'+';
  OutFixedStringVal(os, symbol, s, w, code);
}

template <typename T>
inline void OutFixedInf( std::ostream& os, const T val, const int w, const int code) //d is ignored for Nan/Inf
{
  static std::string symbol="Inf";
  char s=(std::signbit(val))?'-':'+';
  OutFixedStringVal(os, symbol, s, w, code);
}
template <>
inline void OutFixedInf<DDouble>( std::ostream& os, const DDouble val, const int w, const int code) //d is ignored for Nan/Inf
{
  static std::string symbol="Infinity";
  char s=(std::signbit(val))?'-':'+';
  OutFixedStringVal(os, symbol,s, w, code);
}

inline void OutFixFill(std::ostream& os, const std::string &s, const int w, const int code)
{
   if ( code & fmtPAD ) os << std::setfill('0'); 
   if ( ( code & fmtPAD ) && ((s.substr( 0, 1) == "-" ) || ( s.substr(0, 1) == "+" )) ) {  // preventing "00-1.00" or "00+1.00"
     os << s.substr(0, 1) << std::right << std::setw(w - 1) << s.substr(1);
   }
   else  
    os << std::setw(w) << std::right << s;
   if ( code & fmtPAD ) os << std::setfill(' '); //which is '0' or blank at this point.
}

inline void OutAdjustFill(std::ostream& os, const std::string &s, const int w, const int code)
{
   if ( code & fmtPAD ) os << std::setfill('0'); 
   if ( ( code & fmtPAD ) && ((s.substr( 0, 1) == "-" ) || ( s.substr(0, 1) == "+" )) ) {  // preventing "00-1.00" or "00+1.00"
     os << s.substr(0, 1) << std::right << std::setw(w) << s.substr(1);
   }
   else  
    os << std::setw(w) << std::right << s;
   if ( code & fmtPAD ) os << std::setfill(' '); //which is '0' or blank at this point.
}

template <typename T>
void OutFixed(std::ostream& os, const T &val, const int w, const int d, const int code)
{
  if (std::isfinite(val)) {
   std::ostringstream oss;
  if ( code & fmtSHOWPOS ) oss << std::showpos;
   oss << std::fixed << std::setprecision(d) << val;
   if (d==0) oss << ".";
   if( w <= 0)
     os << oss.str();
   else if( oss.tellp() > w)
     OutStars( os, w);
   else if (code & fmtALIGN_LEFT) 
   {
    os << std::left;
    os << std::setw(w);
    os << oss.str();
    os << std::right;
   }
   else
     OutFixFill(os, oss.str(), w, code);
  } else if (std::isnan(val))    OutFixedNan<T>( os, val, w, code);
  else OutFixedInf<T>( os, val, w, code);
}

template <>
void OutFixed<DComplex>(std::ostream& os, const DComplex &val, const int w, const int d, const int code);
template <>
void OutFixed<DComplexDbl>(std::ostream& os, const DComplexDbl &val, const int w, const int d, const int code);


template <typename T>
void OutScientific(std::ostream& os, const T &val, const int w, const int d, const int code) {
 if (std::isfinite(val)) {
  std::ostringstream oss;
  // TODO: IDL handles both lower and upper case "E" (tracker item no. 3147155)
  if ( code & fmtSHOWPOS ) oss << std::showpos;
  if ( code & fmtUPPER ) oss << std::uppercase ;
  oss << std::scientific << std::setprecision(d) << val;
  if (w == 0 )
   os << oss.str();
  else if (oss.tellp() > w)
   OutStars(os, w);
  else if (code & fmtALIGN_LEFT) 
  {
   os << std::left;
   os << std::setw(w);
   os << oss.str();
   os << std::right;
  }
  else OutFixFill(os, oss.str(), w, code);
 } else if (std::isnan(val)) OutFixedNan<T>(os, val, w, code);
 else OutFixedInf<T>(os, val, w, code);
}

template <>
void OutScientific<DComplex>( std::ostream& os, const DComplex &val, const int w, const int d, const int code);
template <>
void OutScientific<DComplexDbl>( std::ostream& os, const DComplexDbl &val, const int w, const int d, const int code);

template <typename T>
void OutAuto(std::ostream& os, const T &val, const int w, const int d, const int code=0) {

 if (std::isfinite(val)) {
  std::ostringstream ossF;
  int fixLen=1;
  
  if (val == T(0.0)) // handle 0.0
  {
   if (w <= 0) {
    if ( code & fmtSHOWPOS ) os<<"+0"; else os<<"0";
    return;
   } //0 is FIXED
   if (code & fmtSHOWPOS) ossF << std::showpos ;
   if (code & fmtALIGN_LEFT) ossF << std::left; else ossF << std::right;
   ossF << std::fixed << std::setprecision(d-1) << val;
  } else {
   int   powTen = static_cast<int>(std::floor( std::log10( std::abs( val))));
   fixLen = powTen > 0 ? powTen+1 : 1; // number of digits before '.'


   // as its used now, if w == 0 -> d != 0 (SetField())
   // static cast here is needed for OS X, without we get a *linker* error
   // (but only if GDL is compiled with Magick)

   if (code & fmtSHOWPOS) ossF << std::showpos ;
   if (code & fmtALIGN_LEFT) ossF << std::left; else ossF << std::right;
   if( w == 0 && (powTen < d && powTen > -d) && (val - std::floor( val) < std::pow( 10.0, static_cast<double>(-d))))
     ossF << std::fixed << std::setprecision(0) << val; 
   else if( powTen == 0 || (powTen < d && powTen > -d+1)) //just like that.
     {
       //format for values between -1 and 1 adapts to the width as to show as many digits as possible
       ossF << std::fixed << std::setprecision(d>fixLen?d-fixLen+((powTen<0)?-powTen:0):0) << val;
       if( d <= fixLen && w>0) ossF << ".";
     }
   else
     fixLen = 0; // marker to force scientific output
 }
  //get the scientific string
  std::ostringstream ossS;
  if (code & fmtSHOWPOS) ossS << std::showpos ;
  if (code & fmtALIGN_LEFT) ossS << std::left; else ossS << std::right;
  if ( code & fmtUPPER ) ossS << std::uppercase ;
  if (w==0) ossS << std::setprecision(d>6?d:6) << val; //Auto w=0 special format: does not use "scientific" but default field-point notation. 
  else ossS << std::scientific << std::setprecision(d>0?d-1:0) << val;
  // compare merits
  if( fixLen == 0 || ossF.tellp() > ossS.tellp()) {
    if( w == 0)
      os << ossS.str();
    else if( ossS.tellp() > w)
      OutStars( os, w);
    else if (code & fmtALIGN_LEFT) 
    {
     os << std::left;
     os << std::setw(w);
     os << ossS.str();
     os << std::right;
    }    
    else
      OutFixFill(os, ossS.str(), w, code);
  }
  else
  {
    if( w == 0)
      os << ossF.str();
    else if( ossF.tellp() > w)
      OutStars( os, w);
    else if (code & fmtALIGN_LEFT) 
    {
     os << std::left;
     os << std::setw(w);
     os << ossF.str();
     os << std::right;
    }
    else
      OutFixFill(os, ossF.str(), w, code);
  }
 }
 else if (std::isnan(val)) OutFixedNan<T>(os, val, w, code);
 else OutFixedInf<T>(os, val, w, code);
}

template <>
void OutAuto<DComplex>( std::ostream& os, const DComplex &val, const int w, const int d, const int code);
template <>
void OutAuto<DComplexDbl>( std::ostream& os, const DComplexDbl &val, const int w, const int d, const int code);

template <typename T>
std::ostream& operator<<(std::ostream& os, const AsComplex<T>& a) 
{
  os << "(";
  OutAuto( os, a.flt.real(), a.width, a.prec, a.code);
  os << ",";
  OutAuto( os, a.flt.imag(), a.width, a.prec, a.code);
  os << ")";
  return os;
}

#endif
