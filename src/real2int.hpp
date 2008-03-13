/***************************************************************************
                       real2int.hpp  -  convert float/double to integer type
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

#ifndef REAL2INT_HPP_
#define REAL2INT_HPP_

#include <limits>

template< typename IntT, typename RealT>
IntT Real2Int( RealT r)
{
  if( std::numeric_limits< IntT>::max() < r)
    {
      Warning( "conversion overflow (+)");
      return std::numeric_limits< IntT>::min();
    }
  if( std::numeric_limits< IntT>::min() > r)
    {
      Warning( "conversion overflow (-)");
      return std::numeric_limits< IntT>::min();
    }
  return static_cast< IntT>( r);
}

template< typename RealT>
DByte Real2DByte( RealT r)
{
 return static_cast< DByte>( Real2Int<DInt, RealT>( r));
}

// //template< typename RealT>
// DByte Real2DByte(float  r)
// {
//   return static_cast< DByte>( Real2Int<DInt, float>( fmod(r,256.0)));
// }

// //template< typename RealT>
// DByte Double2DByte(double r)
// {
//   return static_cast< DByte>( Real2Int<DInt, double >( fmod(r,256.0)));
// }

#endif
