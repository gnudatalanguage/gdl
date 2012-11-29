/***************************************************************************
                       overload.cpp  -  GDL operator overloading for objects
                             -------------------
    begin                : November 29 2012
    copyright            : (C) 2012 by Marc Schellens
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

#include "overload.hpp"

std::string overloadOperatorNames[] = 
{
  "_OVERLOADBRACKETSLEFTSIDE"
, "_OVERLOADBRACKETSRIGHTSIDE" 
, "_OVERLOADMINUSUNARY"
, "_OVERLOADNOT"
, "_OVERLOADTILDE" 
, "_OVERLOADPLUS"
, "_OVERLOADMINUS" 
, "_OVERLOADASTERISK" 
, "_OVERLOADSLASH" 
, "_OVERLOADCARET" 
, "_OVERLOADMOD" 
, "_OVERLOADLESSTHAN" 
, "_OVERLOADGREATERTHAN" 
, "_OVERLOADAND"
, "_OVERLOADOR"
, "_OVERLOADXOR" 
, "_OVERLOADEQ"
, "_OVERLOADNE"
, "_OVERLOADGE"
, "_OVERLOADGT"
, "_OVERLOADLE"
, "_OVERLOADLT"
, "_OVERLOADPOUND" 
, "_OVERLOADPOUNDPOUND" 
, "_OVERLOADISTRUE"
, "_OVERLOADFOREACH"
, "_OVERLOADHELP"
, "_OVERLOADPRINT"
, "_OVERLOADSIZE"
};

int OverloadOperatorIndex( std::string subName)
{
  for( int i=0; i < NumberOfOverloadOperators; ++i)
    if( subName == overloadOperatorNames[ i])
      return i;
  return -1;
}
