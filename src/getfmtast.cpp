/***************************************************************************
                          getfmtast.cpp  -  returns the AST for formatted IO
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

//#define FMT_DEBUG
#undef  FMT_DEBUG

#include "basegdl.hpp"
#include "fmtnode.hpp"
#include "print_tree.hpp"

#include "FMTLexer.hpp"
#include "FMTParser.hpp"

#include "antlr/ASTFactory.hpp"

using namespace std;

antlr::ASTFactory FMTNodeFactory("FMTNode",FMTNode::factory);

RefFMTNode GetFMTAST( DString fmtString)
{
  istringstream istr(fmtString); //+"\n");

  RefFMTNode fmtAST;
  try {  
    antlr::TokenStreamSelector selector;
    
    FMTLexer   lexer( istr);
    lexer.SetSelector( selector);
    
    CFMTLexer  cLexer( lexer.getInputState());
    cLexer.SetSelector( selector);

    lexer.SetCLexer( cLexer);

    selector.select( &lexer);

    FMTParser  parser( selector);

    // because we use the standard (ANTLR generated) constructor here
    // we cannot do it in the constructor
    parser.initializeASTFactory( FMTNodeFactory);
    parser.setASTFactory( &FMTNodeFactory );
   
    parser.format( 1);
    
    fmtAST=parser.getAST();

#ifdef FMT_DEBUG
    antlr::print_tree pt;
    pt.pr_tree(static_cast<antlr::RefAST>(fmtAST));
    cout << endl;
#endif
  }
  catch( GDLException ex)
    {
      throw GDLException("Format: "+ex.getMessage());
    }
  catch( antlr::ANTLRException ex)
    {
      throw GDLException("Format parser: "+ex.getMessage());
    }
  catch( exception ex)
    {
      throw GDLException("Format exception: "+string(ex.what()));
    }
  catch(...)
    {
      throw GDLException("Format: general exception.");
    }

  return fmtAST;
}
