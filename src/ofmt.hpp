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

public:
  AsComplex( const T f, const int w, const int p): flt( f), width( w), prec( p) 
  {}

  template< typename T2> 
  friend std::ostream& operator<<(std::ostream& os, const AsComplex<T2>& a); 
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const AsComplex<T>& a) 
{
  os << "(";
  OutAuto( os, a.flt.real(), a.width, a.prec);
  os << ",";
  OutAuto( os, a.flt.imag(), a.width, a.prec);
  os << ")";
  return os;
}

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
inline void OutFixedZero( std::ostream& os, int w, int d)
{
  if( w == 1) 
    os << "*";
  else if( d >= w)
    OutStars( os, w);
  else if( d == 1) 
    os << std::setw( w) << "0.";
  else 
    {
      os << std::setw( w-(d<=0?1:d)+1) << "0.";
      for( int i=1; i<d; ++i) os << "0";
    }
}

template <typename T>
void OutFixed( std::ostream& os, T val, int w, int d)
{
  if( val == T(0.0)) // handle 0.0
    {
      if( w == 0)
	os << "0.000000";
      else
	OutFixedZero<T>( os, w, d);
    return;
    }
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(d) << val;
  if( w == 0)
    os << oss.str();
  else if( oss.tellp() > w)
    OutStars( os, w);
  else
    os << std::setw( w) << oss.str();
}

template <>
void OutFixed<DComplex>( std::ostream& os, DComplex val, int w, int d);
template <>
void OutFixed<DComplexDbl>( std::ostream& os, DComplexDbl val, int w, int d);


template <typename T>
void OutScientific( std::ostream& os, T val, int w, int d)
{
  std::ostringstream oss;
  oss << std::scientific << std::setprecision(d) << val;
  if( w == 0)
    os << oss.str();
  else if( oss.tellp() > w)
    OutStars( os, w);
  else
    os << std::setw( w) << oss.str();
}

template <>
void OutScientific<DComplex>( std::ostream& os, DComplex val, int w, int d);
template <>
void OutScientific<DComplexDbl>( std::ostream& os, DComplexDbl val, int w, int d);

template <typename T>
void OutAuto( std::ostream& os, T val, int w, int d)
{
  if( val == T(0.0)) // handle 0.0
    {
      if( w == 0)
	os << "0";
      else
	OutFixedZero<T>( os, w, d);
    return;
    }

  int   powTen = static_cast<int>(std::floor( std::log10( std::abs( val))));
  SizeT fixLen = powTen > 0 ? powTen+1 : 1; // number of digits before '.'

  std::ostringstream ossF;

  // as its used now, if w == 0 -> d != 0 (SetField())
  // static cast here is needed for OS X, without we get a *linker* error
  // (but only if GDL is compiled with Magick)
  if( w == 0 && (powTen < d && powTen > -d) &&
      (val - std::floor( val) < std::pow( 10.0, static_cast<double>(-d))))
    ossF << std::fixed << std::setprecision(0) << val; 
  else if( powTen == 0 || (powTen < d && powTen > -d))
    {
      ossF << std::fixed << std::setprecision(d>fixLen?d-fixLen:0) << val;
      if( d <= fixLen) ossF << ".";
    }
  else
    fixLen = 0; // marker to force scientific output
  
  std::ostringstream ossS;
  ossS << std::scientific << std::setprecision(d>0?d-1:0) << val;
  
  if( fixLen == 0 || ossF.tellp() > ossS.tellp())
    if( w == 0)
      os << ossS.str();
    else if( ossS.tellp() > w)
      OutStars( os, w);
    else
      os << std::setw( w) << ossS.str();
  else
    if( w == 0)
      os << ossF.str();
    else if( ossF.tellp() > w)
      OutStars( os, w);
    else
      os << std::setw( w) << ossF.str();
}

template <>
void OutAuto<DComplex>( std::ostream& os, DComplex val, int w, int d);
template <>
void OutAuto<DComplexDbl>( std::ostream& os, DComplexDbl val, int w, int d);

#endif
