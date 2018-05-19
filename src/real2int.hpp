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
inline IntT Real2Int( const RealT r)
{
 //NOOOOOO! IDL does not test numeric limits! its is jus a cast, and for good reasons!!!
//  if( std::numeric_limits< IntT>::max() < r)
//    {
//      //      Warning( "conversion overflow (+)");
//      return std::numeric_limits< IntT>::max();
//    }
//  if( std::numeric_limits< IntT>::min() > r)
//    {
//      //Warning( "conversion overflow (-)");
//      return std::numeric_limits< IntT>::min();
//    }
  return static_cast< IntT>( r);
}


template< typename RealT>
inline DByte Real2DByte( const RealT r)
{
 return static_cast< DByte>( Real2Int<DLong, RealT>( r));
}

#endif
