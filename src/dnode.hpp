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
	   lineNumber(0), cData(NULL), 
	   var(NULL), 
	   libFun(NULL),
	   libPro(NULL),
	   arrIxList(NULL), labelStart( -1), labelEnd( -1)
  {
  }

  DNode( const DNode& cp);

  DNode(antlr::RefToken t) : antlr::CommonAST(t) //, down(), right()
  {
    //    antlr::CommonAST::setType(t->getType() );
    //    antlr::CommonAST::setText(t->getText() );
    DNode::SetLine(t->getLine() );
  }

  void initialize(int t, const std::string& txt)
  {
    antlr::CommonAST::setType(t);
    antlr::CommonAST::setText(txt);

    lineNumber = 0;
    cData=NULL;
    libFun=NULL;
    libPro=NULL;
    labelStart = -1;
    labelEnd   = -1;
    var=NULL;
    arrIxList=NULL;
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

  template<typename T> void Text2Number( T& out, int base)
  {
    T number=0;

    for(unsigned i=0; i < text.size(); i++)
      {
	char c=text[i];
	if( c >= '0' &&  c <= '9' )
	  {
	    c -= '0';
	  }
	else if( c >= 'a' &&  c <= 'f' )
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
	    out=number;
	    // put a notification here
	    return;
	  }

	number=newNumber;
      }
    out=number;
  } 

  void Text2Byte(int base);
  // promote: use Long if number to large
  void Text2Int(int base, bool promote=false);
  void Text2UInt(int base, bool promote=false);
  void Text2Long(int base);
  void Text2ULong(int base); 
  void Text2Long64(int base);
  void Text2ULong64(int base);
  void Text2Float();
  void Text2Double();
  void Text2String();

  void SetNumBranch(const int nB) { numBranch=nB;} 
  void SetArrayDepth(const int aD) { arrayDepth=aD;} 

  void SetFunIx(const int ix) { funIx=ix;}
  void SetProIx(const int ix) { proIx=ix;}
  void SetLibFun(DLibFun* const l) { libFun=l;}
  void SetLibPro(DLibPro* const l) { libPro=l;}
  void SetNDot(const int n) { nDot=n;}

  void SetCompileOpt(const int n) { compileOpt=n;}
  
  void SetLabelRange( const int s, const int e)
  { labelStart = s; labelEnd = e;}
  
  //  bool LabelInRange( const int lIx)
  //  { return (lIx >= labelStart) && (lIx < labelEnd);}

  void DefinedStruct( const bool noTagName)
  { if( noTagName) definedStruct = 1; else definedStruct = 0;}

  void SetArrayIndexList( ArrayIndexListT* aL)
  { arrIxList = aL;}

  BaseGDL* CData() { return cData;}
  void     ResetCData( BaseGDL* newCData);

private:

  BaseGDL* StealCData() { BaseGDL* res = cData; cData=NULL; return res;}
  ArrayIndexListT* StealArrIxList() 
  { ArrayIndexListT* res = arrIxList; arrIxList=NULL; return res;}

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

    int        definedStruct; // struct contains entry with no tag name

    int        compileOpt; // for PRO and FUNCTION nodes
  };

  int labelStart; // for loops to determine if to bail out
  int labelEnd; // for loops to determine if to bail out

  friend class ProgNode;

// private:
//   // forbid usage of these
//   DNode& operator=( const DNode& r) 
//   { return *this;} // make c++ compiler shut up
//   DNode( const DNode& cp) 
//   {} 
};

class ProgNode;
typedef ProgNode* ProgNodeP;

// the nodes the programs are made of
class ProgNode
{
protected:
  static DInterpreter* interpreter;

private:
  int ttype;
  std::string text;

protected:
  ProgNodeP down;
  ProgNodeP right;

  static void AdjustTypes(std::auto_ptr<BaseGDL>& a, 
			  std::auto_ptr<BaseGDL>& b);

  BaseGDL*   cData;           // constant data
  DVar*      var;             // ptr to variable 

  DLibFun*   libFun;
  DLibPro*   libPro;

  union {
    int        initInt;    // for c-i not actually used
    int        numBranch;  // number of branches in switch/case statements
    int        nDot;       // nesting level for tag access
    int        arrayDepth; // dimension to cat
    int        proIx;      // Index into proList
    int        funIx;      // Index into funList
    int        varIx;      // Index into variable list
    int        targetIx;   // Index into label list
    int        definedStruct; // struct contains entry with no tag name
    int        compileOpt; // for PRO and FUNCTION nodes
  };

private:
  // from DNode (see there)
  int lineNumber;
  ArrayIndexListT* arrIxList; // ptr to array index list
  int labelStart; // for loops to determine if to bail out
  int labelEnd; // for loops to determine if to bail out

public:
  ProgNode();

  ProgNode( const RefDNode& refNode);

  static ProgNodeP NewProgNode( const RefDNode& refNode);

  virtual ~ProgNode();
  
  void SetNodes( const ProgNodeP right, const ProgNodeP down);

  virtual BaseGDL* Eval();
  virtual BaseGDL* EvalNC(); // non-copy

  ProgNodeP getFirstChild() const
  {
    return down;
  }
  ProgNodeP GetFirstChild() const
  {
    return getFirstChild();
  }
  ProgNodeP getNextSibling() const
  {
    return right;
  }
  ProgNodeP GetNextSibling() const
  {
    return getNextSibling();
  }
  
  int getType() { return ttype;}
  void setType( int t) { ttype=t;}
  std::string getText() { return text;}
  int getLine() const { return lineNumber;}
  void SetGotoIx( int ix) { targetIx=ix;}
  
  bool LabelInRange( const int lIx)
  { return (lIx >= labelStart) && (lIx < labelEnd);}
  
  friend class GDLInterpreter;
  friend class DInterpreter;
};

class DefaultNode: public ProgNode
{
public:
  DefaultNode( const RefDNode& refNode): ProgNode( refNode) 
  {
    if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
      {
	down = NewProgNode( refNode->GetFirstChild());
      }
    if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
      {
	right = NewProgNode( refNode->GetNextSibling());
      }
  }
};
class UnaryExpr: public DefaultNode
{
public:
  UnaryExpr( const RefDNode& refNode): DefaultNode( refNode) 
  { setType( GDLTokenTypes::EXPR);}

  //  int getType() { return GDLTokenTypes::EXPR;}
};
class BinaryExpr: public DefaultNode
{
protected:
  ProgNodeP op1, op2;

public:
  BinaryExpr( const RefDNode& refNode);

  //  int getType() { return GDLTokenTypes::EXPR;}
};
// binary expression with at least one operand non-copy
class BinaryExprNC: public BinaryExpr
{
protected:
  bool op1NC, op2NC;

public:
  BinaryExprNC( const RefDNode& refNode);

  void AdjustTypesNC( std::auto_ptr<BaseGDL>& g1, BaseGDL*& e1, 
		      std::auto_ptr<BaseGDL>& g2, BaseGDL*& e2);

  //  int getType() { return GDLTokenTypes::EXPR;}
};
class TrinaryExpr: public DefaultNode
{
protected:
  ProgNodeP op1, op2, op3;

public:
  TrinaryExpr( const RefDNode& refNode): DefaultNode( refNode) 
  {
    op1 = GetFirstChild();
    op2 = GetFirstChild()->GetNextSibling();
    op3 = GetFirstChild()->GetNextSibling()->GetNextSibling();
    setType( GDLTokenTypes::EXPR);
  }
  //  int getType() { return GDLTokenTypes::EXPR;}
};

// VAR, SYSVAR, ...
class LeafNode: public DefaultNode
{
public:
  LeafNode( const RefDNode& refNode): DefaultNode( refNode)
  {}
};


// used together with some defines do
// allow using non-ref nodes with ANTLR
// see gdlc.i.g
namespace antlr {

  RefAST ConvertAST( ProgNodeP p);
}

class VARNode: public LeafNode
{
public:
  VARNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL* EvalNC();
};
class VARPTRNode: public LeafNode
{
public:
  VARPTRNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL* EvalNC();
};
class SYSVARNode: public LeafNode
{
public:
  SYSVARNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL* EvalNC();
};
class DEREFNode: public LeafNode
{
public:
  DEREFNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL* EvalNC();
};
class CONSTANTNode: public LeafNode
{
public:
  CONSTANTNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL* EvalNC();
};

// expression nodes
class QUESTIONNode: public TrinaryExpr
{ public:
  QUESTIONNode( const RefDNode& refNode): TrinaryExpr( refNode){}
  BaseGDL* Eval();
};
class UMINUSNode: public UnaryExpr
{ public:
  UMINUSNode( const RefDNode& refNode): UnaryExpr( refNode){}
  BaseGDL* Eval();
};
class LOG_NEGNode: public UnaryExpr
{ public:
  LOG_NEGNode( const RefDNode& refNode): UnaryExpr( refNode){}
  BaseGDL* Eval();
};
class NOT_OPNode: public UnaryExpr
{ public:
  NOT_OPNode( const RefDNode& refNode): UnaryExpr( refNode){}
  BaseGDL* Eval();
};
class AND_OPNode: public BinaryExpr
{ public:
  AND_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class OR_OPNode: public BinaryExpr
{ public:
  OR_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class XOR_OPNode: public BinaryExpr
{ public:
  XOR_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class LOG_ANDNode: public BinaryExpr
{ public:
  LOG_ANDNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class LOG_ORNode: public BinaryExpr
{ public:
  LOG_ORNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};


class EQ_OPNode: public BinaryExpr
{ public:
  EQ_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class NE_OPNode: public BinaryExpr
{ public:
  NE_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class LE_OPNode: public BinaryExpr
{ public:
  LE_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class LT_OPNode: public BinaryExpr
{ public:
  LT_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class GE_OPNode: public BinaryExpr
{ public:
  GE_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class GT_OPNode: public BinaryExpr
{ public:
  GT_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class PLUSNode: public BinaryExpr
{ public:
  PLUSNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class MINUSNode: public BinaryExpr
{ public:
  MINUSNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class LTMARKNode: public BinaryExpr
{ public:
  LTMARKNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class GTMARKNode: public BinaryExpr
{ public:
  GTMARKNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class ASTERIXNode: public BinaryExpr
{ public:
  ASTERIXNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class MATRIX_OP1Node: public BinaryExpr
{ public:
  MATRIX_OP1Node( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class MATRIX_OP2Node: public BinaryExpr
{ public:
  MATRIX_OP2Node( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class SLASHNode: public BinaryExpr
{ public:
  SLASHNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class MOD_OPNode: public BinaryExpr
{ public:
  MOD_OPNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
class POWNode: public BinaryExpr
{ public:
  POWNode( const RefDNode& refNode): BinaryExpr( refNode){}
  BaseGDL* Eval();
};
// class DECNode: public BinaryExpr
// { public:
//   DECNode( const RefDNode& refNode): BinaryExpr( refNode){}
//   BaseGDL* Eval();
// };
// class INCNode: public BinaryExpr
// { public:
//   INCNode( const RefDNode& refNode): BinaryExpr( refNode){}
//   BaseGDL* Eval();
// };
// class POSTDECNode: public BinaryExpr
// { public:
//   POSTDECNode( const RefDNode& refNode): BinaryExpr( refNode){}
//   BaseGDL* Eval();
// };
// class POSTINCNode: public BinaryExpr
// { public:
//   POSTINCNode( const RefDNode& refNode): BinaryExpr( refNode){}
//   BaseGDL* Eval();
// };

// non-copy expression nodes
// only minimal gain for trinary and unary operators
// class QUESTIONNCNode: public TrinaryExprNC
// { public:
//   QUESTIONNCNode( const RefDNode& refNode): TrinaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
// class UMINUSNCNode: public UnaryExprNC
// { public:
//   UMINUSNCNode( const RefDNode& refNode): UnaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
// class LOG_NEGNCNode: public UnaryExprNC
// { public:
//   LOG_NEGNCNode( const RefDNode& refNode): UnaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
// class NOT_OPNCNode: public UnaryExprNC
// { public:
//   NOT_OPNCNode( const RefDNode& refNode): UnaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
class AND_OPNCNode: public BinaryExprNC
{ public:
  AND_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class OR_OPNCNode: public BinaryExprNC
{ public:
  OR_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class XOR_OPNCNode: public BinaryExprNC
{ public:
  XOR_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class LOG_ANDNCNode: public BinaryExprNC
{ public:
  LOG_ANDNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class LOG_ORNCNode: public BinaryExprNC
{ public:
  LOG_ORNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class EQ_OPNCNode: public BinaryExprNC
{ public:
  EQ_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class NE_OPNCNode: public BinaryExprNC
{ public:
  NE_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class LE_OPNCNode: public BinaryExprNC
{ public:
  LE_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class LT_OPNCNode: public BinaryExprNC
{ public:
  LT_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class GE_OPNCNode: public BinaryExprNC
{ public:
  GE_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class GT_OPNCNode: public BinaryExprNC
{ public:
  GT_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class PLUSNCNode: public BinaryExprNC
{ public:
  PLUSNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class MINUSNCNode: public BinaryExprNC
{ public:
  MINUSNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class LTMARKNCNode: public BinaryExprNC
{ public:
  LTMARKNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class GTMARKNCNode: public BinaryExprNC
{ public:
  GTMARKNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class ASTERIXNCNode: public BinaryExprNC
{ public:
  ASTERIXNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class MATRIX_OP1NCNode: public BinaryExprNC
{ public:
  MATRIX_OP1NCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class MATRIX_OP2NCNode: public BinaryExprNC
{ public:
  MATRIX_OP2NCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class SLASHNCNode: public BinaryExprNC
{ public:
  SLASHNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class MOD_OPNCNode: public BinaryExprNC
{ public:
  MOD_OPNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class POWNCNode: public BinaryExprNC
{ public:
  POWNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
// class DECNCNode: public BinaryExprNC
// { public:
//   DECNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
// class INCNode: public BinaryExprNC
// { public:
//   INCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
// class POSTDECNCNode: public BinaryExprNC
// { public:
//   POSTDECNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
//   BaseGDL* Eval();
// };
// class POSTINCNode: public BinaryExprNC
// { public:
//   POSTINCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
//   BaseGDL* Eval();
// };



#endif

