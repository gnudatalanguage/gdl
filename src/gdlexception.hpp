/***************************************************************************
                              gdlexception.hpp
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

#ifndef GDLEXCEPTION_HPP_
#define GDLEXCEPTION_HPP_

#include <string>
#include <iostream>

#include "dnode.hpp"

#include "antlr/ANTLRException.hpp"

//using namespace std;

class GDLException: public antlr::ANTLRException
{
  RefDNode errorNode;
  SizeT line;
  SizeT col;
  bool prefix;

public:
  GDLException(): ANTLRException(), 
    errorNode(static_cast<RefDNode>(antlr::nullAST)),
		  line( 0), col( 0), prefix( true)
  {}
  GDLException(const std::string& s, bool pre = true);
  GDLException(const RefDNode eN, const std::string& s);
  GDLException(SizeT l, SizeT c, const std::string& s);

  ~GDLException() throw() {}

  SizeT getLine() const 
  { 
    if( errorNode != static_cast<RefDNode>(antlr::nullAST))
      return errorNode->getLine();
    return line;
  }

  void SetLine( SizeT l) { line = l;}

  SizeT getColumn() const 
  { 
    //    if( errorNode != static_cast<RefDNode>(antlr::nullAST))
    //      return errorNode->getColumn();
    return col;
  }

  bool Prefix() const 
  { 
    return prefix;
  }
};

// warnings ignore !QUIET
void Warning(const std::string& s);

// messages honor !QUIET
void Message(const std::string& s);

#endif

