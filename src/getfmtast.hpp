/***************************************************************************
                          getfmtast.hpp  -  converts string to format AST
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GETFMTAST_HPP_
#define GETFMTAST_HPP_

#include "fmtnode.hpp"

#include "basegdl.hpp"

#include "antlr/ASTFactory.hpp"

extern antlr::ASTFactory FMTNodeFactory;

RefFMTNode GetFMTAST( DString fmtString);

#endif
