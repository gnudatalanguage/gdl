/***************************************************************************
                          dnode.hpp  -  the node used for the AST
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

#ifndef dnode_hpp__
#define dnode_hpp__

#include <cmath>
#include <memory>
#include <iostream>

#include "typedefs.hpp"

#include "GDLTokenTypes.hpp"

#include "antlr/CommonAST.hpp"

//ANTLR_USING_NAMESPACE(std)
//ANTLR_USING_NAMESPACE(antlr)

// used together with some defines do
// allow using non-ref nodes with ANTLR
// see gdlc.i.g
class ProgNode;
typedef ProgNode* ProgNodeP;
namespace antlr {

RefAST ConvertAST( ProgNodeP p);
}

class DInterpreter;

class DNode;
typedef antlr::ASTRefCount<DNode> RefDNode;

class DVar;
class DPro;

class DLibFun;
class DLibPro;

class BaseGDL;

class ArrayIndexListT;

class DNode : public antlr::CommonAST {

public:

  ~DNode();

  DNode(): CommonAST(), //down(), right(),
// 	keepRight( false),
	   lineNumber(0), cData(NULL), 
	   var(NULL), 
	   libFun(NULL),
	   libPro(NULL),
	   arrIxList(NULL),arrIxListNoAssoc(NULL), labelStart( -1), labelEnd( -1)
  {
  }

  DNode( const DNode& cp);

  DNode(antlr::RefToken t) : antlr::CommonAST(t) //, down(), right()
//   	, keepRight( false)
{
    //    antlr::CommonAST::setType(t->getType() );
    //    antlr::CommonAST::setText(t->getText() );
    DNode::SetLine(t->getLine() );
  }

// 	void DoKeepRight() { keepRight = true;}

  void initialize(int t, const std::string& txt)
  {
    antlr::CommonAST::setType(t);
    antlr::CommonAST::setText(txt);

// 	keepRight = false;

    lineNumber = 0;
    cData=NULL;
    libFun=NULL;
    libPro=NULL;
    labelStart = -1;
    labelEnd   = -1;
    var=NULL;
    arrIxList=NULL;
    arrIxListNoAssoc=NULL;
  }

  // used by DNodeFactory
  void initialize( RefDNode t );

  // we deal only with RefDNode here
  void initialize( antlr::RefAST t )
  {
    //    antlr::CommonAST::initialize(t);
    initialize(static_cast<RefDNode>(t));
  }

  void initialize( antlr::RefToken t )
  {
    antlr::CommonAST::initialize(t);
    
    //    DNode::SetLine( t->getLine());
    SetLine( t->getLine());
  }

  antlr::RefAST clone( void ) const
  {
    DNode *newNode = new DNode( *this);
    return antlr::RefAST( newNode);
  }

  void setText(const std::string& txt)
  {
    antlr::CommonAST::setText(txt);
  }

  void setType(int type)
  {
    antlr::CommonAST::setType(type);
  }

  void addChild( RefDNode c )
  {
    BaseAST::addChild( static_cast<antlr::RefAST>(c) );
  }

  static antlr::RefAST factory()
  {
    antlr::RefAST ret = static_cast<antlr::RefAST>(RefDNode(new DNode));
    return ret;
  }
  
  RefDNode GetFirstChild() const
  {
    return static_cast<RefDNode>(BaseAST::getFirstChild());
  }

  RefDNode GetNextSibling() const
  {
    return static_cast<RefDNode>(BaseAST::getNextSibling());
  }

  void RemoveNextSibling();

  // Extensions
  void SetLine(int l_)
  {
    lineNumber = l_;
  }

  int getLine() const
  { 
    if( lineNumber != 0 || BaseAST::getFirstChild() == NULL)
      return lineNumber;
    // this handles inserted nodes
    return static_cast<RefDNode>(BaseAST::getFirstChild())->getLine();

// This was just too clever :-)
//     if( lineNumber != 0)
//       return lineNumber;
//     else 
//       {
// 	if( BaseAST::getFirstChild() == NULL)
// 	  {
// 	    if( BaseAST::getNextSibling() == NULL)
// 	      {
// 		return lineNumber;
// 	      }
// 	    else 
// 	      {
// 		return 
// 		  static_cast<RefDNode>(BaseAST::getNextSibling())->getLine();
// 	      }
// 	    return lineNumber;
// 	  }
// 	else 
// 	  {
// 	    return 
// 	      static_cast<RefDNode>(BaseAST::getFirstChild())->getLine();
// 	  }
//       }
  }
    
  void SetVarIx(int vIx)
  {
    varIx=vIx;
  }

  void SetVar(DVar* v)
  {
    var=v;
  }

  template<typename T> bool Text2Number( T& out, int base)
  {
    bool noOverflow = true;

    T number=0;

    for(unsigned i=0; i < text.size(); ++i)
      {
	char c=text[i];
	if( c >= '0' && c <= '9')
	  {
	    c -= '0';
	  }
	else if( c >= 'a' &&  c <= 'f')
	  {
	    c -= 'a'-10;
	  }
	else 
	  {
	    c -= 'A'-10;
	  }

	T newNumber = base * number + c;

	// check for overflow
	if( newNumber < number)
	  {
	    noOverflow = false;
	  }

	number=newNumber;
      }
    out=number;

    return noOverflow;
  } 

  void Text2Byte(int base);
  // promote: use Long if number to large
  void Text2Int(int base, bool promote=false);
  void Text2UInt(int base, bool promote=false);
  void Text2Long(int base, bool promote=false);
  void Text2ULong(int base, bool promote=false); 
  void Text2Long64(int base);
  void Text2ULong64(int base);
  void Text2Float();
  void Text2Double();
  void Text2String();

  void SetNumBranch(const int nB) { numBranch=nB;} 
  void SetArrayDepth(const int aD) { arrayDepth=aD;} 

  void SetFunIx(const int ix);
  void SetProIx(const int ix);
  void SetLibFun(DLibFun* const l) { libFun=l;}
  void SetLibPro(DLibPro* const l) { libPro=l;}
  void SetNDot(const int n) { nDot=n;}
  int GetNDot() const { return nDot;}
  void SetNParam(const int n) { nParam=n;}
  int GetNParam() const { return nParam;}

  void SetCompileOpt(const int n) { compileOpt=n; }
  int GetCompileOpt() { return compileOpt; }
  
  void SetLabelRange( const int s, const int e)
  { labelStart = s; labelEnd = e;}
  
  //  bool LabelInRange( const int lIx)
  //  { return (lIx >= labelStart) && (lIx < labelEnd);}

  void DefinedStruct( const bool noTagName)
  { if( noTagName) structDefined = 1; else structDefined = 0;}

  void SetArrayIndexList( ArrayIndexListT* aL, ArrayIndexListT* aLNoAssoc)
  { 
    arrIxList = aL;
    arrIxListNoAssoc = aLNoAssoc;    
  }

  BaseGDL* CData() { return cData;}
  void     ResetCData( BaseGDL* newCData);

  DVar*    GetVar()   { return var;}
  int      GetVarIx() { return varIx;}

private:

  BaseGDL* StealCData() { BaseGDL* res = cData; cData=NULL; return res;}

  ArrayIndexListT* StealArrIxList() 
  { ArrayIndexListT* res = arrIxList; arrIxList=NULL; return res;}
  ArrayIndexListT* CloneArrIxList(); 

  ArrayIndexListT* StealArrIxNoAssocList() 
  { ArrayIndexListT* res = arrIxListNoAssoc; arrIxListNoAssoc=NULL; return res;}
  ArrayIndexListT* CloneArrIxNoAssocList(); 

// 	bool keepRight; // for passing to ProgNode, nodes here are reference counted

  //  RefDNode down;
  //  RefDNode right;

  // track line number in node
  int lineNumber;

  // void*     initPtr;    // for c-i not actaully used
  BaseGDL*   cData;        // constant data
  //  DNode*     gotoTarget;   // for goto statement
  DVar*      var;          // ptr to variable (for system variables and common blocks)

  DLibFun*   libFun;
  DLibPro*   libPro;

  ArrayIndexListT* arrIxList; // ptr to array index list
  ArrayIndexListT* arrIxListNoAssoc; // ptr to array index list
//  ArrayIndexT*     arrIx;     // ptr to array index (1-dim)

  union {
    int        initInt;    // for c-i not actually used
    
    int        numBranch;  // number of branches in switch/case statements
    int        nDot;       // nesting level for tag access
    int        arrayDepth; // dimension to cat

    int        proIx;      // Index into proList
    int        funIx;      // Index into funList
    int        varIx;      // Index into variable list
    int        targetIx;   // Index into label list

    int        structDefined; // struct contains entry with no tag name

    int        nParam;	// number of positional parameters in this parameter list,
			// stored at first parameter (keyword or positional)
    
    int        compileOpt; // for PRO and FUNCTION nodes
  };

  int labelStart; // for loops to determine if to bail out
  int labelEnd; // for loops to determine if to bail out

  friend class ProgNode;
  friend class DCompiler;
  friend class GDLTreeParser;

// private:
//   // forbid usage of these
//   DNode& operator=( const DNode& r) 
//   { return *this;} // make c++ compiler shut up
//   DNode( const DNode& cp) 
//   {} 
};

#endif

