/***************************************************************************
                               color.cpp  -  GDL routines for color handling
                             -------------------
    begin                : March 25 2004
    copyright            : (C) 2004 by Marc Schellens
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

#include "color.hpp"

#include <iostream>

#ifdef _MSC_VER
#define round(f) floor(f+0.5)
#endif

using namespace std;

const DFloat maxDByte = 255.0;

void RGB2HSV( const DByte r, const DByte g, const DByte b, 
	      DFloat& h, DFloat& s, DFloat& v )
{
  DByte cmin = (r < g)? ((r < b)? r : b) : ((g < b)? g : b);
  DByte cmax = (r > g)? ((r > b)? r : b) : ((g > b)? g : b);

  v = static_cast<DFloat>(cmax) / maxDByte; // v

  DFloat delta = static_cast<DFloat>(cmax - cmin);

  if( cmax != 0)
    s = delta / static_cast<DFloat>(cmax); // s
  else 
    {
      // r = g = b = 0	-> s = 0 ->  h undefined
      s = 0.0;
      h = 0.0; // undefined
      return;
    }

  if( s == 0.0)
    {
      h = 0.0; // s = 0 ->  h undefined
      return;
    }

  if( r == cmax )
    h = static_cast<DFloat>( g - b) / delta;       // between yellow & magenta
  else if( g == cmax )
    h = 2.0 + static_cast<DFloat>( b - r) / delta; // between cyan & yellow
  else // ( b == cmax)
    h = 4.0 + static_cast<DFloat>( r - g) / delta; // between magenta & cyan

  h *= 60.0; // degrees
  if( h < 0.0 )
    h += 360.0;
}

void HSV2RGB( DFloat h, const DFloat s, const DFloat v,
	      DByte& r, DByte& g, DByte& b) 
{
  DFloat maxDByteV = v * maxDByte;
  if( s == 0.0) 
    { // grey
      r = g = b = static_cast< DByte>(round( maxDByteV));
      return;
    }

  h /= 360.0;       // [0,1]*n
  h = h - floor(h); // [0,1]
  h *= 6.0;         // sector 0 to 5
  DFloat fh = floor( h);
  DFloat f = h - fh; // factorial part of h
  DByte p = static_cast< DByte>(round( ( 1.0 - s) * maxDByteV));
  DByte q = static_cast< DByte>(round( ( 1.0 - s * f) * maxDByteV));
  DByte t = static_cast< DByte>(round( ( 1.0 - s * (1.0 - f)) * maxDByteV));

  int i = static_cast< int>( fh);
  switch( i) 
    {
    case 0:
      r = static_cast< DByte>(round( v * maxDByte)); g = t; b = p; break;
    case 1:
      r = q; g = static_cast< DByte>(round( v * maxDByte)); b = p; break;
    case 2:
      r = p; g = static_cast< DByte>(round( v * maxDByte)); b = t; break;
    case 3:
      r = p; g = q; b = static_cast< DByte>(round( v * maxDByte)); break;
    case 4:
      r = t; g = p; b = static_cast< DByte>(round( v * maxDByte)); break;
    default: // case 5:
      r = static_cast< DByte>(round( v * maxDByte)); g = p; b = q; break;
    }
}

void RGB2HLS( const DByte r, const DByte g, const DByte b,
	      DFloat& h, DFloat& l, DFloat& s) 
{
  DFloat cmin = (r < g)? ((r < b)? r : b) : ((g < b)? g : b);
  DFloat cmax = (r > g)? ((r > b)? r : b) : ((g > b)? g : b);

  l = (cmax+cmin) / maxDByte / 2.0;

  if( cmax == cmin)
    {
      s = h = 0.0; // undefined
      return;
    }

  DFloat delta = cmax - cmin;

  if( l < 0.5) 
    s = delta/(cmax+cmin);
  else	 
    s = delta/(2.0 * maxDByte - cmax - cmin);

  if( r == cmax) 
    h = static_cast<DFloat>((g-b)) / delta;
  else 
    if( g == cmax) 
      h = 2.0 + static_cast<DFloat>((b - r)) / delta;
    else        
      h = 4.0 + static_cast<DFloat>((r - g)) / delta;
  
  h *= 60.0; // degrees
  if( h < 0.0 )
    h += 360.0;
}

DFloat Hue2RGB( const DFloat m1, const DFloat m2, DFloat h) 
{
  h = h - floor(h);
  if( h < 1.0/6.0) 
    return (m1+(m2-m1)*h*6.0);
  if( h < 1.0/2.0) 
    return m2;
  if( h < 2.0/3.0) 
    return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
  return m1;
}

void HLS2RGB( DFloat h, const DFloat l, const DFloat s,
	      DByte& r, DByte& g, DByte& b) 
{
  if( s == 0.0) 
    r = g = b = static_cast< DByte>(round( l * maxDByte));
  else 
    {
      h /= 360.0; // -> [0,1]
      DFloat m2;
      if (l <= 0.5) 
	m2 = l*(1.0+s);
      else         
	m2 = l+s-l*s;
      DFloat m1 = 2.0*l-m2;
      r = static_cast< DByte>(round( Hue2RGB(m1,m2,h+1.0/3.0) * maxDByte));
      g = static_cast< DByte>(round( Hue2RGB(m1,m2,h) * maxDByte));
      b = static_cast< DByte>(round( Hue2RGB(m1,m2,h-1.0/3.0) * maxDByte));
    }
}
