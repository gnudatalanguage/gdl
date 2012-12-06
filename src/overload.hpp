/***************************************************************************
                       overload.hpp  -  GDL operator overloading for objects
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

#ifndef OVERLOAD_HPP_
#define OVERLOAD_HPP_

#include <string>

#include "dpro.hpp"

// changes here must be reflected in std::string overloadOperatorNames[] (overload.cpp)
enum OverloadOperators
{
  OOBracketsLeftSide
, OOBracketsRightSide 
, OOMinusUnary 
, OONOT 
, OOTilde 
, OOPlus 
, OOMinus 
, OOAsterisk 
, OOSlash 
, OOCaret 
, OOMOD 
, OOLessThan 
, OOGreaterThan 
, OOAND 
, OOOR 
, OOXOR 
, OOEQ 
, OONE 
, OOGE 
, OOGT 
, OOLE 
, OOLT 
, OOPound 
, OOPoundPound 
, OOIsTrue
, OOForeach
, OOHelp
, OOPrint
, OOSize
, NumberOfOverloadOperators
};

int OverloadOperatorIndexFun( std::string subName);
int OverloadOperatorIndexPro( std::string subName);

void SetupOverloadSubroutines();

class OperatorList
{
private:  
  DSubUD* operators[ NumberOfOverloadOperators];  
  
public:
  OperatorList()
  {
    for( int i=0; i < NumberOfOverloadOperators; ++i)
      operators[ i] = NULL;
  }  
  OperatorList( const OperatorList& cp)
  {
    for( int i=0; i < NumberOfOverloadOperators; ++i)
      operators[ i] = cp[ i];
  }  
  ~OperatorList()
  {
    // as operators are added to funList/proList as well, they are not owned by OperatorList
//     for( int i=0; i < NumberOfOverloadOperators; ++i)
//       delete operators[ i];
  }  
  
  DSubUD* operator[]( SizeT i) const { return operators[i];}
  void SetOperator( SizeT op, DSubUD* opSub) { operators[op] = opSub;}
};



#endif
