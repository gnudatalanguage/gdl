/***************************************************************************
                               color.hpp  -  GDL routines for color handling
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

#ifndef COLOR_HPP_
#define COLOR_HPP_

#include "basegdl.hpp"

// h = [0,360], s = [0,1], v = [0,1], l = [0,1]

void RGB2HSV( const DByte r, const DByte g, const DByte b, 
	      DFloat& h, DFloat& s, DFloat& v );
void HSV2RGB( DFloat h, const DFloat s, const DFloat v,
	      DByte& r, DByte& g, DByte& b); 
void RGB2HLS( const DByte r, const DByte g, const DByte b,
	      DFloat& h, DFloat& l, DFloat& s); 
void HLS2RGB( const DFloat h, const DFloat l, const DFloat s,
	      DByte& r, DByte& g, DByte& b); 

#endif
