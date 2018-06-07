/***************************************************************************
 prognodeexpr.hpp  -  the node used for expressions in the running program
                             -------------------
    begin                : July 22 2008
    copyright          : (C) 2002-2008 by Marc Schellens
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

#ifndef prognodeexpr_hpp__
#define prognodeexpr_hpp__

#include "prognode.hpp"
#include "dpro.hpp"


class UnaryExpr: public DefaultNode
{
public:
  UnaryExpr( const RefDNode& refNode): DefaultNode( refNode) 
  { setType( GDLTokenTypes::EXPR);}

  bool ConstantExpr()
  {
    return down->ConstantNode();
  }
  //  int getType() { return GDLTokenTypes::EXPR;}
};

class BinaryExpr: public DefaultNode
{
protected:
  ProgNodeP op1, op2;

public:
  BinaryExpr( const RefDNode& refNode);

  bool ConstantExpr()
  {
    return op1->ConstantNode() &&
      op2->ConstantNode();
  }
  //  int getType() { return GDLTokenTypes::EXPR;}
};
// binary expression with at least one operand non-copy
class BinaryExprNC: public BinaryExpr
{
protected:
  bool op1NC, op2NC;

public:
  BinaryExprNC( const RefDNode& refNode);

  void AdjustTypesNC( Guard<BaseGDL>& g1, BaseGDL*& e1, 
		      Guard<BaseGDL>& g2, BaseGDL*& e2);
  // for overloaded operators 
  void SetupGuards( Guard<BaseGDL>& g1, BaseGDL*& e1,
		    Guard<BaseGDL>& g2, BaseGDL*& e2);
  void AdjustTypesNCNull( Guard<BaseGDL>& g1, BaseGDL*& e1, 
		      Guard<BaseGDL>& g2, BaseGDL*& e2);

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
    //    setType( GDLTokenTypes::EXPR); Can be l_expr!!!
  }
  //  int getType() { return GDLTokenTypes::EXPR;}
};

// VAR, SYSVAR, ...
class LeafNode: public DefaultNode
{
public:
  LeafNode(): DefaultNode() 
  {}
  LeafNode( const RefDNode& refNode): DefaultNode( refNode)
  {}
};





class FCALL_LIB_RETNEWNode: public LeafNode
{
public:
  FCALL_LIB_RETNEWNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** LEval();
  BaseGDL* Eval();
};

class FCALL_LIB_DIRECTNode: public LeafNode
{
  LibFunDirect libFunDirectFun;
public:
  FCALL_LIB_DIRECTNode( const RefDNode& refNode): LeafNode( refNode)
  {
    assert( this->libFun != NULL);
    libFunDirectFun = static_cast<DLibFunDirect*>(this->libFun)->FunDirect();
  }
  BaseGDL** LEval();
  BaseGDL* Eval();
};

class FCALL_LIB_N_ELEMENTSNode: public LeafNode
{
public:
  FCALL_LIB_N_ELEMENTSNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** LEval();
  BaseGDL* Eval();
};



class FCALL_LIBNode: public LeafNode
{
public:
  FCALL_LIBNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* Eval();
  BaseGDL* EvalFCALL_LIB( BaseGDL**& retValPtr); // might return a non new value
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);  
};



class MFCALLNode: public LeafNode
{
public:
  MFCALLNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* Eval();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);  
};



class MFCALL_PARENTNode: public LeafNode
{
public:
  MFCALL_PARENTNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* Eval();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);  
};



class FCALLNode: public LeafNode
{
public:
  FCALLNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LExpr( BaseGDL* right);
  BaseGDL** LEval();
  BaseGDL* Eval();
//   BaseGDL** LExprGrab( BaseGDL* right);  
};

class ARRAYEXPR_FCALLNode: public LeafNode
{
private:
  int fcallNodeFunIx;
  FCALLNode* fcallNode;
  ARRAYEXPRNode* arrayExprNode;
  
  ARRAYEXPR_FCALLNode(){} // disable

public:
  ARRAYEXPR_FCALLNode( const RefDNode& refNode): LeafNode( refNode)
  {
    assert( dynamic_cast<FCALLNode*>(this->getFirstChild()->getNextSibling()));
    assert( dynamic_cast<ARRAYEXPRNode*>(this->getFirstChild()));
    arrayExprNode = static_cast<ARRAYEXPRNode*>(this->getFirstChild());
    fcallNode = dynamic_cast<FCALLNode*>(arrayExprNode->getNextSibling());
    fcallNodeFunIx = fcallNode->funIx;
  }
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LExpr( BaseGDL* right);
  BaseGDL** LEval();
  BaseGDL* Eval();
};



class ARRAYEXPR_MFCALLNode: public LeafNode
{
public:
  ARRAYEXPR_MFCALLNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval(); //{ ThrowGDLException( "Internal errorr: ARRAYEXPR_MFCALLNode called.");}
  BaseGDL* Eval();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);  
};



class VARNode: public LeafNode
{
public:
  VARNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* EvalNC();
  BaseGDL* EvalNCNull();
  BaseGDL* Eval();
  BaseGDL** LExpr( BaseGDL* right);  
//   BaseGDL** LExprGrab( BaseGDL* right);  
};
class VARPTRNode: public LeafNode
{
public:
  VARPTRNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* EvalNC();
  BaseGDL* EvalNCNull();
  BaseGDL* Eval();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);  
};
class SYSVARNode: public LeafNode
{
public:
  SYSVARNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* EvalNC();
  BaseGDL* Eval();
  BaseGDL** LExpr(BaseGDL* r);
  //BaseGDL** LExprGrab( BaseGDL* right);    
};
class DEREFNode: public LeafNode
{
public:
  DEREFNode( const RefDNode& refNode): LeafNode( refNode)
  {}
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
  BaseGDL** LEval();
  BaseGDL* EvalNC();
  BaseGDL* Eval();
  BaseGDL** LExpr( BaseGDL* right);
//   BaseGDL** LExprGrab( BaseGDL* right);  
};
class CONSTANTNode: public LeafNode
{
public:
  CONSTANTNode( ProgNodeP r, BaseGDL* c): LeafNode()
  { 
if( c == NULL)
    assert( c != NULL);
    setType( GDLTokenTypes::CONSTANT);
    cData = c;
    right = r;
  }
  CONSTANTNode( const RefDNode& refNode): LeafNode( refNode)
  {
if( cData == NULL)
    assert( cData != NULL);
  }
  BaseGDL* EvalNC();
  BaseGDL* Eval();
};
class ARRAYDEFNode: public DefaultNode
{ public:
  ARRAYDEFNode( const RefDNode& refNode): DefaultNode( refNode)
  {}
  
  BaseGDL* Eval();
  bool ConstantArray()
  {
    ProgNodeP _t =  this->getFirstChild();
    while(  _t != NULL) {

      if( _t->getType() != GDLTokenTypes::CONSTANT)
		return false;

      _t = _t->getNextSibling();
    }
    return true;
  }

};
class ARRAYDEF_GENERALIZED_INDGENNode: public ARRAYDEFNode
{
public:
  ARRAYDEF_GENERALIZED_INDGENNode( const RefDNode& refNode): ARRAYDEFNode( refNode)
  {}
  
  BaseGDL* Eval();
};

class STRUCNode: public DefaultNode
{ public:
  STRUCNode( const RefDNode& refNode): DefaultNode( refNode){}
  BaseGDL* Eval();
};
class NSTRUCNode: public DefaultNode
{ public:
  NSTRUCNode( const RefDNode& refNode): DefaultNode( refNode){}
  BaseGDL* Eval();
};
class DStructDesc;
class NSTRUC_REFNode: public DefaultNode
{ public:
  NSTRUC_REFNode( const RefDNode& refNode): DefaultNode( refNode), dStruct(NULL) {}
  BaseGDL* Eval();
  private:
    DStructDesc* dStruct;
};

// expression nodes
class QUESTIONNode: public TrinaryExpr
{ public:
  QUESTIONNode( const RefDNode& refNode): TrinaryExpr( refNode){}
  ProgNodeP GetThisBranch(); // as parameter or keyword
  BaseGDL* Eval();
  BaseGDL** LEval();
  BaseGDL** LExpr(BaseGDL* right);
  BaseGDL** EvalRefCheck( BaseGDL*& rEval);
//   ProgNodeP GetBranch() 
//   {
//     ProgNodeP branch = this->GetThisBranch();
//     while( branch->getType() == GDLTokenTypes::QUESTION)
//     {
//       QUESTIONNode* qRecursive = static_cast<QUESTIONNode*>( branch);
//       branch = qRecursive->GetThisBranch();
//     }
//     return branch;
//   }
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
class PLUSNC12Node: public BinaryExprNC // both are NC
{ public:
  PLUSNC12Node( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class MINUSNCNode: public BinaryExprNC
{ public:
  MINUSNCNode( const RefDNode& refNode): BinaryExprNC( refNode){}
  BaseGDL* Eval();
};
class MINUSNC12Node: public BinaryExprNC
{ public:
  MINUSNC12Node( const RefDNode& refNode): BinaryExprNC( refNode){}
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
class ASTERIXNC12Node: public BinaryExprNC // both are NC
{ public:
  ASTERIXNC12Node( const RefDNode& refNode): BinaryExprNC( refNode){}
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
class SLASHNC12Node: public BinaryExprNC
{ public:
  SLASHNC12Node( const RefDNode& refNode): BinaryExprNC( refNode){}
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

