/***************************************************************************
                          fmtnode.cpp  -  instantiate the vtable
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

#include "fmtnode.hpp"

FMTNode::~FMTNode() {}

// used by FMTNodeFactory
void FMTNode::initialize( RefFMTNode t )
{
  CommonAST::setType( t->getType());
  CommonAST::setText( t->getText());

  w = t->w;
  d = t->d;
  rep = t->rep;
}

