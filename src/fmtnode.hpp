/***************************************************************************
                          FMTNode.hpp  -  node for formatted io processing
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

#ifndef FMTNode_hpp__
#define FMTNode_hpp__

#include "typedefs.hpp"

#include "FMTTokenTypes.hpp"

#include <antlr/CommonAST.hpp>

//ANTLR_USING_NAMESPACE(std)
//ANTLR_USING_NAMESPACE(antlr)
  
class FMTNode;
typedef antlr::ASTRefCount<FMTNode> RefFMTNode;

class FMTNode : public antlr::CommonAST {

public:

  ~FMTNode();

  FMTNode(): CommonAST(), down(), right(), w(-1), d(-1), rep(1), fill(' ')
  {
  }

  FMTNode( antlr::RefToken t) : 
    CommonAST(t), down(), right(), w(-1), d(-1), rep(1), fill(' ')
  {
  }

  void initialize(int t, const std::string& txt)
  {
    CommonAST::setType(t);
    CommonAST::setText(txt);
  }

  // used by FMTNodeFactory
  void initialize( RefFMTNode t );

  // we deal only with RefFMTNode here
  void initialize( antlr::RefAST t )
  {
    //    CommonAST::initialize(t);
    initialize(static_cast<RefFMTNode>(t));
  }

  void initialize( antlr::RefToken t )
  {
    CommonAST::initialize(t);
  }

  void setText(const std::string& txt)
  {
    CommonAST::setText(txt);
  }

  void setType(int type)
  {
    CommonAST::setType(type);
  }

  void addChild( RefFMTNode c )
  {
    BaseAST::addChild( static_cast<antlr::RefAST>(c) );
  }

  static antlr::RefAST factory()
  {
    antlr::RefAST ret = static_cast<antlr::RefAST>(RefFMTNode(new FMTNode));
    return ret;
  }
  
//   RefFMTNode getFirstChild() const
//   {
//     return static_cast<RefFMTNode>(BaseAST::getFirstChild());
//   }

//   RefFMTNode getNextSibling() const
//   {
//     return static_cast<RefFMTNode>(BaseAST::getNextSibling());
//   }

  void setW( const int w_)
  {
    w=w_;
  }
  int getW()
  {
    return w;
  }

  void setD( const int d_)
  {
    d=d_;
  }
  int getD()
  {
    return d;
  }

  void setRep( const int rep_)
  {
    rep=rep_;
  }
  int getRep()
  {
    return rep;
  }
  void setFill( const char fill_)
  {
    fill=fill_;
  }
  char getFill()
  {
    return fill;
  }
  
private:
  RefFMTNode down;
  RefFMTNode right;

  int w;
  int d;

  int rep;
  char fill; // fill (for zero padding)
  
// private:

//   // forbid usage of these
//   FMTNode& operator=( const FMTNode& r) 
//   { return *this;} // make compiler shut up
//   FMTNode( const FMTNode& cp) 
//   {} 
};

#endif

