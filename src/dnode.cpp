/***************************************************************************
                          dnode.cpp  -  GDL's AST is made of DNodes
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

#include "includefirst.hpp"

#include <string>
#include <limits>

#include "antlr/TreeParser.hpp"
#include "antlr/Token.hpp"

#include "dnode.hpp"
#include "datatypes.hpp"
#include "arrayindex.hpp"
#include "dinterpreter.hpp"

#include <assert.h>

using namespace std;

namespace antlr {

  RefAST ConvertAST( ProgNodeP p) 
  { 
    if( p == NULL) return TreeParser::ASTNULL;

    RefDNode refNode = RefDNode( new DNode);
    refNode->setType( p->getType());
    refNode->setText( p->getText());
    refNode->SetLine( p->getLine());
    
    return static_cast<antlr::RefAST>( refNode);
  }
}

DInterpreter* ProgNode::interpreter;

void DNode::RemoveNextSibling()
{
  right = static_cast<BaseAST*>(static_cast<AST*>(antlr::nullAST));
}

bool NonCopyNode( int type)
{
  return (type == GDLTokenTypes::DEREF) ||
    (type == GDLTokenTypes::CONSTANT) ||
    (type == GDLTokenTypes::VAR) ||
    (type == GDLTokenTypes::SYSVAR) ||
    (type == GDLTokenTypes::VARPTR);
}

BaseGDL* ProgNode::EvalNC()
{
  assert( 0); // internal error
}

BaseGDL* VARNode::EvalNC()
{
      EnvStackT& callStack=interpreter->CallStack();
      BaseGDL* res=callStack.back()->GetKW(this->varIx); 
      if( res == NULL)
	throw GDLException( this, "Variable is undefined: "+
			    callStack.back()->GetString(this->varIx));
      return res;
}

BaseGDL* VARPTRNode::EvalNC()
{
      BaseGDL* res=this->var->Data();
      if( res == NULL)
	{
	  EnvStackT& callStack=interpreter->CallStack();
	  throw GDLException( this, "Variable is undefined: "+
			      callStack.back()->GetString( res));
	}
      return res;
}

BaseGDL* CONSTANTNode::EvalNC()
{
  return this->cData;
}

BaseGDL* SYSVARNode::EvalNC()
{
  if( this->var == NULL) 
    {
      this->var=FindInVarList(sysVarList,this->getText());
      if( this->var == NULL)		    
	throw GDLException( this, "Not a legal system variable: !"+
			    this->getText());
    }
  // system variables are always defined
  return this->var->Data(); 
}

BaseGDL* DEREFNode::EvalNC()
{
  BaseGDL* e1 = this->getFirstChild()->Eval();
  auto_ptr<BaseGDL> e1_guard(e1);
  
  DPtrGDL* ptr=dynamic_cast<DPtrGDL*>(e1);
  if( ptr == NULL)
    throw GDLException( this, "Pointer type required"
			" in this context: "+interpreter->Name(e1));
  DPtr sc; 
  if( !ptr->Scalar(sc))
    throw GDLException( this, "Expression must be a "
			"scalar in this context: "+interpreter->Name(e1));
  if( sc == 0)
    throw GDLException( this, "Unable to dereference"
			" NULL pointer: "+interpreter->Name(e1));
  
  try{
    return interpreter->GetHeap(sc);
  }
  catch( GDLInterpreter::HeapException)
    {
      throw GDLException( this, "Invalid pointer: "+interpreter->Name(e1));
    }
}

BinaryExpr::BinaryExpr( const RefDNode& refNode): DefaultNode( refNode)
{
  op1 = GetFirstChild();
  op2 = GetFirstChild()->GetNextSibling();
}
BinaryExprNC::BinaryExprNC( const RefDNode& refNode): BinaryExpr( refNode)
{
  op1NC = NonCopyNode( op1->getType());
  op2NC = NonCopyNode( op2->getType());
}

ProgNode::ProgNode(): // for NULLProgNode
  ttype( antlr::Token::NULL_TREE_LOOKAHEAD),
  text( "NULLProgNode"),
  down( NULL), 
  right( NULL),
  lineNumber( 0),
  cData( NULL),
  var( NULL),
  labelStart( 0),
  labelEnd( 0)
{}

ProgNode::ProgNode( const RefDNode& refNode):
  ttype( refNode->getType()),
  text( refNode->getText()),
  down( NULL), 
  right( NULL),
  lineNumber( refNode->getLine()),
  cData( refNode->StealCData()),
  var( refNode->var),
  arrIxList( refNode->StealArrIxList()),
  labelStart( refNode->labelStart),
  labelEnd( refNode->labelEnd)
{
  initInt = refNode->initInt;
  
//   if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
//     {
//       down = NewProgNode( refNode->GetFirstChild());
//     }
//   if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
//     {
//       right = NewProgNode( refNode->GetNextSibling());
//     }
}

ProgNode::~ProgNode()
{
  // delete cData in case this node is a constant
  if( (getType() == GDLTokenTypes::CONSTANT))
    {
      delete cData;
    }
  if( (getType() == GDLTokenTypes::ARRAYIX))
    {
      delete arrIxList;
    }
  delete down;
  delete right;
}

void ProgNode::SetNodes( const ProgNodeP r, const ProgNodeP d)
{
  right = r;
  down  = d;
}

BaseGDL* ProgNode::Eval()
{ 
  return ProgNode::interpreter->expr( this);
}

DNode::~DNode()
  {
    // delete cData in case this node is a constant
    if( (getType() == GDLTokenTypes::CONSTANT) && cData != NULL)
      {
	delete cData;
      }
    if( (getType() == GDLTokenTypes::ARRAYIX))
      {
	delete arrIxList;
      }
  }

void DNode::Text2Byte(int base)
{
  // cout << "byte" << endl;
  DByte val;
  Text2Number( val, base);
  cData=new DByteGDL(val);
}

// promote: make Long (Long64) if number is to large
void DNode::Text2Int(int base, bool promote)
{
  static const DLong64 maxDInt=
    static_cast<DLong64>(numeric_limits<DInt>::max());
  static const DLong64 maxDLong=
    static_cast<DLong64>(numeric_limits<DLong>::max());
  
  if( promote)
    {
      DLong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDInt)
	{
	  DInt val = static_cast<DInt>(ll);
	  cData=new DIntGDL( val);
	}
      else if( ll <= maxDLong)
	{
	  DLong val = static_cast<DLong>(ll);
	  cData=new DLongGDL( val);
	}
      else
	{
	  cData=new DLong64GDL( ll);
	}
    }
  else
    {
      DInt val;
      Text2Number( val, base);
      cData=new DIntGDL(val);
    }
}
void DNode::Text2UInt(int base, bool promote)
{
  static const DULong64 maxDUInt=
    static_cast<DULong64>(numeric_limits<DUInt>::max());
  static const DULong64 maxDULong=
    static_cast<DULong64>(numeric_limits<DULong>::max());

  if( promote)
    {
      DULong64 ll;
      Text2Number( ll, base);
      
      if( ll <= maxDUInt)
	{
	  DUInt val = static_cast<DUInt>(ll);
	  cData=new DUIntGDL( val);
	}
      else if( ll <= maxDULong)
	{
	  DULong val = static_cast<DULong>(ll);
	  cData=new DULongGDL( val);
	}
      else
	{
	  cData=new DULong64GDL( ll);
	}
    }
  else
    {
      DUInt val;
      Text2Number( val, base);
      cData=new DUIntGDL(val);
    }
}
void DNode::Text2Long(int base)
{
  // cout << "long" << endl;
  DLong val;
  Text2Number( val, base);
  cData=new DLongGDL(val);
}
void DNode::Text2ULong(int base) 
{
  DULong val;
  Text2Number( val, base);
  cData=new DULongGDL(val);
}
void DNode::Text2Long64(int base)
{
  DLong64 val;
  Text2Number( val, base);
  cData=new DLong64GDL(val);
}
void DNode::Text2ULong64(int base)
{
  DULong64 val;
  Text2Number( val, base);
  cData=new DULong64GDL(val);
}
void DNode::Text2Float()
{
  const char* cStr=text.c_str();
  DFloat val=strtod(cStr,NULL);
  cData=new DFloatGDL(val);
}
void DNode::Text2Double()
{
  const char*  cStr=text.c_str();
  DDouble val=strtod(cStr,NULL);
  cData=new DDoubleGDL(val);
}
void DNode::Text2String()
{
  cData=new DStringGDL(text);
}

// used by DNodeFactory
void DNode::initialize( RefDNode t )
{
  CommonAST::setType( t->getType());
  CommonAST::setText( t->getText());

  DNode::SetLine(t->getLine() );

  if( t->getType() == GDLTokenTypes::CONSTANT)
    {
      if( t->cData != NULL) cData=t->cData->Dup(); else cData = NULL;
    }
  else if( t->getType() == GDLTokenTypes::GOTO)
    {
      targetIx=t->targetIx;
    }
  else if( t->getType() == GDLTokenTypes::SYSVAR || 
	   t->getType() == GDLTokenTypes::VARPTR)
    {
      var=t->var;
    }
  else
    {
      initInt=t->initInt;
    }

  labelStart = t->labelStart;
  labelEnd   = t->labelEnd;
  // copy union stuff
  //    initPtr=t->initPtr;
}

ProgNodeP ProgNode::NewProgNode( const RefDNode& refNode)
{
  bool nonCopy = false;
  if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
    {
      if( NonCopyNode( refNode->GetFirstChild()->getType()))
	nonCopy = true;
      if( refNode->GetFirstChild()->GetNextSibling() != 
	  RefDNode(antlr::nullAST))
	if( NonCopyNode( refNode->GetFirstChild()->GetNextSibling()->getType()))
	  nonCopy = true;
    }
  
  if( nonCopy)
  switch( refNode->getType())
    {
    case GDLTokenTypes::QUESTION:
      {
	return new QUESTIONNode( refNode);
      }

      // unary
    case GDLTokenTypes::UMINUS:
      {
	return new UMINUSNode( refNode);
      }
    case GDLTokenTypes::LOG_NEG:
      {
	return new LOG_NEGNode( refNode);
      }
    case GDLTokenTypes::NOT_OP:
      {
	return new NOT_OPNode( refNode);
      }

      // binary
    case GDLTokenTypes::AND_OP:
      {
	return new AND_OPNCNode( refNode);
      }
    case GDLTokenTypes::OR_OP:
      {
	return new OR_OPNCNode( refNode);
      }
    case GDLTokenTypes::XOR_OP:
      {
	return new XOR_OPNCNode( refNode);
      }
    case GDLTokenTypes::LOG_AND:
      {
	return new LOG_ANDNCNode( refNode);
      }
    case GDLTokenTypes::LOG_OR:
      {
	return new LOG_ORNCNode( refNode);
      }
    case GDLTokenTypes::EQ_OP:
      {
	return new EQ_OPNCNode( refNode);
      }
    case GDLTokenTypes::NE_OP:
      {
	return new NE_OPNCNode( refNode);
      }
    case GDLTokenTypes::LE_OP:
      {
	return new LE_OPNCNode( refNode);
      }
    case GDLTokenTypes::LT_OP:
      {
	return new LT_OPNCNode( refNode);
      }
    case GDLTokenTypes::GE_OP:
      {
	return new GE_OPNCNode( refNode);
      }
    case GDLTokenTypes::GT_OP:
      {
	return new GT_OPNCNode( refNode);
      }
    case GDLTokenTypes::PLUS:
      {
	return new PLUSNCNode( refNode);
      }
    case GDLTokenTypes::MINUS:
      {
	return new MINUSNCNode( refNode);
      }
    case GDLTokenTypes::LTMARK:
      {
	return new LTMARKNCNode( refNode);
      }
    case GDLTokenTypes::GTMARK:
      {
	return new GTMARKNCNode( refNode);
      }
    case GDLTokenTypes::ASTERIX:
      {
	return new ASTERIXNCNode( refNode);
      }
    case GDLTokenTypes::MATRIX_OP1:
      {
	return new MATRIX_OP1NCNode( refNode);
      }
    case GDLTokenTypes::MATRIX_OP2:
      {
	return new MATRIX_OP2NCNode( refNode);
      }
    case GDLTokenTypes::SLASH:
      {
	return new SLASHNCNode( refNode);
      }
    case GDLTokenTypes::MOD_OP:
      {
	return new MOD_OPNCNode( refNode);
      }
    case GDLTokenTypes::POW:
      {
	return new POWNCNode( refNode);
      }
//     case GDLTokenTypes::DEC:
//       {
// 	return new DECNCNode( refNode);
//       }
//     case GDLTokenTypes::INC:
//       {
// 	return new INCNCNode( refNode);
//       }
//     case GDLTokenTypes::POSTDEC:
//       {
// 	return new POSTDECNCNode( refNode);
//       }
//     case GDLTokenTypes::POSTINC:
//       {
// 	return new POSTINCNCNode( refNode);
//       }
//    default:
//      return new DefaultNode( refNode);
    }
  else // !nonCopy
    switch( refNode->getType())
      {
      case GDLTokenTypes::QUESTION:
	{
	  return new QUESTIONNode( refNode);
	}

	// unary
      case GDLTokenTypes::UMINUS:
	{
	  return new UMINUSNode( refNode);
	}
      case GDLTokenTypes::LOG_NEG:
	{
	  return new LOG_NEGNode( refNode);
	}
      case GDLTokenTypes::NOT_OP:
	{
	  return new NOT_OPNode( refNode);
	}

	// binary
      case GDLTokenTypes::AND_OP:
	{
	  return new AND_OPNode( refNode);
	}
      case GDLTokenTypes::OR_OP:
	{
	  return new OR_OPNode( refNode);
	}
      case GDLTokenTypes::XOR_OP:
	{
	  return new XOR_OPNode( refNode);
	}
      case GDLTokenTypes::LOG_AND:
	{
	  return new LOG_ANDNode( refNode);
	}
      case GDLTokenTypes::LOG_OR:
	{
	  return new LOG_ORNode( refNode);
	}
      case GDLTokenTypes::EQ_OP:
	{
	  return new EQ_OPNode( refNode);
	}
      case GDLTokenTypes::NE_OP:
	{
	  return new NE_OPNode( refNode);
	}
      case GDLTokenTypes::LE_OP:
	{
	  return new LE_OPNode( refNode);
	}
      case GDLTokenTypes::LT_OP:
	{
	  return new LT_OPNode( refNode);
	}
      case GDLTokenTypes::GE_OP:
	{
	  return new GE_OPNode( refNode);
	}
      case GDLTokenTypes::GT_OP:
	{
	  return new GT_OPNode( refNode);
	}
      case GDLTokenTypes::PLUS:
	{
	  return new PLUSNode( refNode);
	}
      case GDLTokenTypes::MINUS:
	{
	  return new MINUSNode( refNode);
	}
      case GDLTokenTypes::LTMARK:
	{
	  return new LTMARKNode( refNode);
	}
      case GDLTokenTypes::GTMARK:
	{
	  return new GTMARKNode( refNode);
	}
      case GDLTokenTypes::ASTERIX:
	{
	  return new ASTERIXNode( refNode);
	}
      case GDLTokenTypes::MATRIX_OP1:
	{
	  return new MATRIX_OP1Node( refNode);
	}
      case GDLTokenTypes::MATRIX_OP2:
	{
	  return new MATRIX_OP2Node( refNode);
	}
      case GDLTokenTypes::SLASH:
	{
	  return new SLASHNode( refNode);
	}
      case GDLTokenTypes::MOD_OP:
	{
	  return new MOD_OPNode( refNode);
	}
      case GDLTokenTypes::POW:
	{
	  return new POWNode( refNode);
	}
	//     case GDLTokenTypes::DEC:
	//       {
	// 	return new DECNode( refNode);
	//       }
	//     case GDLTokenTypes::INC:
	//       {
	// 	return new INCNode( refNode);
	//       }
	//     case GDLTokenTypes::POSTDEC:
	//       {
	// 	return new POSTDECNode( refNode);
	//       }
	//     case GDLTokenTypes::POSTINC:
	//       {
	// 	return new POSTINCNode( refNode);
	//       }
	//      default:
      }

  // independed of nonCopy:
  switch( refNode->getType())
    {
      case GDLTokenTypes::VAR:
	{
	  return new VARNode( refNode);
	}
      case GDLTokenTypes::VARPTR:
	{
	  return new VARPTRNode( refNode);
	}
      case GDLTokenTypes::SYSVAR:
	{
	  return new SYSVARNode( refNode);
	}
      case GDLTokenTypes::DEREF:
	{
	  return new DEREFNode( refNode);
	}
      case GDLTokenTypes::CONSTANT:
	{
	  return new CONSTANTNode( refNode);
	}
    }

  // default
  return new DefaultNode( refNode);
}

// converts inferior type to superior type
void ProgNode::AdjustTypes(auto_ptr<BaseGDL>& a, auto_ptr<BaseGDL>& b)
{
  DType aTy=a->Type();
  DType bTy=b->Type();
  if( aTy == bTy) return;
  if( aTy > 100 || bTy > 100)
    {
      //exception
      throw GDLException( "Expressions of this type cannot be converted.");
    }
  if( DTypeOrder[aTy] > DTypeOrder[bTy])
    {
      // convert b to a
      b.reset( b.release()->Convert2( aTy));
    }
  else
    {
      // convert a to b
      a.reset( a.release()->Convert2( bTy));
    }
}
void BinaryExprNC::AdjustTypesNC(auto_ptr<BaseGDL>& g1, BaseGDL*& e1,
				 auto_ptr<BaseGDL>& g2, BaseGDL*& e2)
{
  if( op1NC)
    {
      e1 = op1->EvalNC();
    }
  else
    {
      e1 = op1->Eval();
      g1.reset( e1);
    }
  if( op2NC)
    {
      e2 = op2->EvalNC();
    }
  else
    {
      e2 = op2->Eval();
      g2.reset( e2);
    }

  DType aTy=e1->Type();
  DType bTy=e2->Type();
  if( aTy == bTy) 
      return;

  if( aTy > 100 || bTy > 100)
    {
      throw GDLException( "Expressions of this type cannot be converted.");
    }

  if( DTypeOrder[aTy] > DTypeOrder[bTy])
    {
      // convert e2 to e1
      e2 = e2->Convert2( aTy, BaseGDL::COPY);
      g2.reset( e2); // delete former e2
    }
  else
    {
      // convert e1 to e2
      e1 = e1->Convert2( bTy, BaseGDL::COPY);
      g1.reset( e1); // delete former e1
    }
}

BaseGDL* QUESTIONNode::Eval()
{
  auto_ptr<BaseGDL> e1( op1->Eval());
  if( e1->True())
    {
      return op2->Eval(); // right->down
    }
  return op3->Eval(); // right->right
}

BaseGDL* UMINUSNode::Eval()
{
  BaseGDL* e1 = down->Eval();
  return e1->UMinus(); // might delete e1 (STRING)
}
BaseGDL* NOT_OPNode::Eval()
{
  BaseGDL* e1 = down->Eval();
  return e1->NotOp();
}
BaseGDL* LOG_NEGNode::Eval()
{
  auto_ptr<BaseGDL> e1( down->Eval());
  return e1->LogNeg();
}

BaseGDL* AND_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    {
    res= e2->AndOp(e1.get()); // scalar+scalar or array+scalar
    e2.release();
    }
  else
    if( e2->Scalar())
      {
      res= e1->AndOpInv(e2.get()); // array+scalar
      e1.release();
      }
    else
      if( e1->N_Elements() <= e2->N_Elements())
	{
	res = e1->AndOpInv(e2.get()); // smaller_array + larger_array or same size
	e1.release();
	}
      else
	{
	res = e2->AndOp(e1.get()); // smaller + larger
	e2.release();
	}
  return res;
}
BaseGDL* OR_OPNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->OrOp(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->OrOpInv(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->OrOpInv(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->OrOp(e1.get()); // smaller + larger
	 e2.release();
       }
 return res;
}
BaseGDL* XOR_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  if( e1->N_Elements() <= e2->N_Elements())
    {
    res= e1->XorOp(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
    }
  else
    {
    res= e2->XorOp(e1.get()); // smaller + larger
	 e2.release();
    }
  return res;
}
BaseGDL* LOG_ANDNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  if( !e1->LogTrue()) res = new DByteGDL( 0);
  else if( !e2->LogTrue()) res = new DByteGDL( 0);
  else res = new DByteGDL( 1);
  return res;
}
BaseGDL* LOG_ORNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  if( e1->LogTrue()) res = new DByteGDL( 1); 
  else if( e2->LogTrue()) res = new DByteGDL( 1);
  else res = new DByteGDL( 0);
  return res;
}

BaseGDL* EQ_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  res=e1->EqOp(e2.get());
  return res;
}
BaseGDL* NE_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  res=e1->NeOp(e2.get());
  return res;
}
BaseGDL* LE_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  res=e1->LeOp(e2.get());
  return res;
}
BaseGDL* LT_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  res=e1->LtOp(e2.get());
  return res;
}
BaseGDL* GE_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  res=e1->GeOp(e2.get());
  return res;
}
BaseGDL* GT_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  res=e1->GtOp(e2.get());
  return res;
}
BaseGDL* PLUSNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->AddInv(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->Add(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->Add(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->AddInv(e1.get()); // smaller + larger
	 e2.release();
       }
return res;
}
BaseGDL* MINUSNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->SubInv(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->Sub(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->Sub(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->SubInv(e1.get()); // smaller + larger
	 e2.release();
       }
 return res;
}
BaseGDL* LTMARKNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->LtMark(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->LtMark(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->LtMark(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->LtMark(e1.get()); // smaller + larger
	 e2.release();
       }
 return res;
}
BaseGDL* GTMARKNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->GtMark(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->GtMark(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->GtMark(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->GtMark(e1.get()); // smaller + larger
	 e2.release();
       }
 return res;
}
BaseGDL* ASTERIXNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->Mult(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->Mult(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->Mult(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->Mult(e1.get()); // smaller + larger
	 e2.release();
       }
 return res;
}

BaseGDL* MATRIX_OP1Node::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  DType aTy=e1->Type();
  DType bTy=e2->Type();
  DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;

  DType cTy=maxTy;
  if( maxTy == BYTE || maxTy == INT)
    cTy=LONG;
  else if( maxTy == UINT)
    cTy=ULONG;

  if( aTy != cTy)
    e1.reset( e1.release()->Convert2( cTy));

  AdjustTypes(e1,e2);
  res=e1->MatrixOp(e2.get());
  return res;
}
BaseGDL* MATRIX_OP2Node::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  DType aTy=e1->Type();
  DType bTy=e2->Type();
  DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;

  DType cTy=maxTy;
  if( maxTy == BYTE || maxTy == INT)
    cTy=LONG;
  else if( maxTy == UINT)
    cTy=ULONG;

  if( aTy != cTy) 
    e1.reset( e1.release()->Convert2( cTy));

  AdjustTypes(e1,e2);
  res=e2->MatrixOp(e1.get());
  return res;
}
BaseGDL* SLASHNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->DivInv(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->Div(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->Div(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->DivInv(e1.get()); // smaller + larger
	 e2.release();
       }

 return res;
}
BaseGDL* MOD_OPNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> e1( op1->Eval());
 auto_ptr<BaseGDL> e2( op2->Eval());
 AdjustTypes(e1,e2);
 if( e1->Scalar())
   {
     res= e2->ModInv(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->Scalar())
     {
       res= e1->Mod(e2.get()); // array+scalar
       e1.release();
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 res= e1->Mod(e2.get()); // smaller_array + larger_array or same size
	 e1.release();
       }
     else
       {
	 res= e2->ModInv(e1.get()); // smaller + larger
	 e2.release();
       }
 return res;
}


BaseGDL* POWNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  // special handling for complex
  DType aTy=e1->Type();
  DType bTy=e2->Type();
  if( ComplexType( aTy))
    {
      if( IntType( bTy))
	{
	  e2.reset( e2.release()->Convert2( LONG));
	  res = e1->Pow( e2.get());
	  if( res == e1.get())
	    e1.release();
	  return res;
	}
      if( aTy == COMPLEX)
	{
	  if( bTy == DOUBLE)
	    {
	      e1.reset( e1.release()->Convert2( COMPLEXDBL));
	      aTy = COMPLEXDBL;
	    }
	  else if( bTy == FLOAT)
	    {
	      res = e1->Pow( e2.get());
	      if( res == e1.get())
		e1.release();
	      return res;
	    }
	}
      if( aTy == COMPLEXDBL)
	{
	  if( bTy == FLOAT)
	    {
	      e2.reset( e2.release()->Convert2( DOUBLE));
	      bTy = DOUBLE;
	    }
	  if( bTy == DOUBLE)
	    {
	      res = e1->Pow( e2.get());
	      if( res == e1.get())
		e1.release();
	      return res;
	    }
	}
    }

  DType convertBackT; 

  // convert back
  if( IntType( bTy) && (DTypeOrder[ bTy] > DTypeOrder[ aTy]))
    convertBackT = aTy;
  else
    convertBackT = UNDEF;
  
  AdjustTypes(e2,e1); // order crucial here (for converting back)

  if( e1->Scalar())
    {
      res= e2->PowInv(e1.get()); // scalar+scalar or array+scalar
      e2.release();
    }
  else
    if( e2->Scalar())
      {
      res= e1->Pow(e2.get()); // array+scalar
      e1.release();
      }
    else
      if( e1->N_Elements() <= e2->N_Elements())
	{
	res= e1->Pow(e2.get()); // smaller_array + larger_array or same size
	e1.release();
	}
      else
	{
	  res= e2->PowInv(e1.get()); // smaller + larger
	  e2.release();
	}
  if( convertBackT != UNDEF)
    {
      res = res->Convert2( convertBackT, BaseGDL::CONVERT);
    }
 endPOW:
  return res;
}
// BaseGDL* DECNode::Eval()
// { BaseGDL* res;
//   return new DECNode( refNode);
// }
// BaseGDL* INCNode::Eval()
// { BaseGDL* res;
//   return new INCNode( refNode);
// }
// BaseGDL* POSTDECNode::Eval()
// { BaseGDL* res;
//   return new POSTDECNode( refNode);
// }
// BaseGDL* POSTINCNode::Eval()
// { BaseGDL* res;
//   return new POSTINCNode( refNode);
// }

// ***********************
// **** nonCopy nodes ****
// ***********************
BaseGDL* AND_OPNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->AndOpInv(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->AndOp(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
	 return e1->Dup()->AndOpInv(e2);
       }
   }

 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->AndOp(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       res= e1->AndOpInv(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res = e1->AndOpInv(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res = e2->AndOp(e1); // smaller + larger
	 
       }
 return res;
}
BaseGDL* OR_OPNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->OrOpInv(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->OrOp(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
	 return e1->Dup()->OrOpInv(e2);
       }
   }

 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->OrOp(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->OrOpInv(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->OrOpInv(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->OrOp(e1); // smaller + larger
	 
       }
 return res;
}
BaseGDL* XOR_OPNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 
 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->XorOp(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->XorOp(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->XorOp(e2); 
       }
   }

 if( e1->N_Elements() <= e2->N_Elements())
   {
     if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
     res= e1->XorOp(e2); // smaller_array + larger_array or same size
     
   }
 else
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->XorOp(e1); // smaller + larger
     
   }
 return res;
}
BaseGDL* LOG_ANDNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2;
 if( op1NC)
   {
     e1 = op1->EvalNC();
   }
 else
   {
     e1 = op1->Eval();
     g1.reset( e1);
   }
 if( !e1->LogTrue()) return new DByteGDL( 0);

 if( op2NC)
   {
     e2 = op2->EvalNC();
   }
 else
   {
     e2 = op2->Eval();
     g2.reset( e2);
   }
 if( !e2->LogTrue()) return new DByteGDL( 0);
 return new DByteGDL( 1);
}
BaseGDL* LOG_ORNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2;
 if( op1NC)
   {
     e1 = op1->EvalNC();
   }
 else
   {
     e1 = op1->Eval();
     g1.reset( e1);
   }
 if( e1->LogTrue()) return new DByteGDL( 1); 
 if( op2NC)
   {
     e2 = op2->EvalNC();
   }
 else
   {
     e2 = op2->Eval();
     g2.reset( e2);
   }
 if( e2->LogTrue()) return new DByteGDL( 1);
 return new DByteGDL( 0);
}


BaseGDL* EQ_OPNCNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> g1;
  auto_ptr<BaseGDL> g2;
  BaseGDL *e1, *e2;
  AdjustTypesNC( g1, e1, g2, e2);
  res=e1->EqOp(e2);
  return res;
}
BaseGDL* NE_OPNCNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> g1;
  auto_ptr<BaseGDL> g2;
  BaseGDL *e1, *e2;
  AdjustTypesNC( g1, e1, g2, e2);
  res=e1->NeOp(e2);
  return res;
}
BaseGDL* LE_OPNCNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> g1;
  auto_ptr<BaseGDL> g2;
  BaseGDL *e1, *e2;
  AdjustTypesNC( g1, e1, g2, e2);
  res=e1->LeOp(e2);
  return res;
}
BaseGDL* LT_OPNCNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> g1;
  auto_ptr<BaseGDL> g2;
  BaseGDL *e1, *e2;
  AdjustTypesNC( g1, e1, g2, e2);
  res=e1->LtOp(e2);
  return res;
}
BaseGDL* GE_OPNCNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> g1;
  auto_ptr<BaseGDL> g2;
  BaseGDL *e1, *e2;
  AdjustTypesNC( g1, e1, g2, e2);
  res=e1->GeOp(e2);
  return res;
}
BaseGDL* GT_OPNCNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> g1;
  auto_ptr<BaseGDL> g2;
  BaseGDL *e1, *e2;
  AdjustTypesNC( g1, e1, g2, e2);
  res=e1->GtOp(e2);
  return res;
}
BaseGDL* PLUSNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->Add(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->AddInv(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->Add(e2); 
       }
   }

 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->AddInv(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->Add(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->Add(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->AddInv(e1); // smaller + larger
	 
       }
 return res;
}
BaseGDL* MINUSNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->Sub(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->SubInv(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->Sub(e2); 
       }
   }


 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->SubInv(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->Sub(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->Sub(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->SubInv(e1); // smaller + larger
	 
       }
 return res;
}
BaseGDL* LTMARKNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->LtMark(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->LtMark(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->LtMark(e2); 
       }
   }


 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->LtMark(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->LtMark(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->LtMark(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->LtMark(e1); // smaller + larger
	 
       }
 return res;
}
BaseGDL* GTMARKNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->GtMark(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->GtMark(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->GtMark(e2); 
       }
   }

 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->GtMark(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->GtMark(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->GtMark(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->GtMark(e1); // smaller + larger
	 
       }
 return res;
}
BaseGDL* ASTERIXNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->Mult(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->Mult(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->Mult(e2); 
       }
   }

 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->Mult(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->Mult(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->Mult(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->Mult(e1); // smaller + larger
	 
       }
 return res;
}

BaseGDL* MATRIX_OP1NCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2;
 if( op1NC)
   {
     e1 = op1->EvalNC();
   }
 else
   {
     e1 = op1->Eval();
     g1.reset( e1);
   }
 if( op2NC)
   {
     e2 = op2->EvalNC();
   }
 else
   {
     e2 = op2->Eval();
     g2.reset( e2);
   }
 DType aTy=e1->Type();
 DType bTy=e2->Type();
 DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;
 if( maxTy > 100)
   {
     throw GDLException( "Expressions of this type cannot be converted.");
   }

 DType cTy=maxTy;
 if( maxTy == BYTE || maxTy == INT)
   cTy=LONG;
 else if( maxTy == UINT)
   cTy=ULONG;

 if( aTy != cTy)
   {
     e1 = e1->Convert2( cTy, BaseGDL::COPY);
     g1.reset( e1);
   }
 if( bTy != cTy)
   {
     e2 = e2->Convert2( cTy, BaseGDL::COPY);
     g2.reset( e2);
   }
 
 res=e1->MatrixOp(e2);
 return res;
}
BaseGDL* MATRIX_OP2NCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2;
 if( op1NC)
   {
     e1 = op1->EvalNC();
   }
 else
   {
     e1 = op1->Eval();
     g1.reset( e1);
   }
 if( op2NC)
   {
     e2 = op2->EvalNC();
   }
 else
   {
     e2 = op2->Eval();
     g2.reset( e2);
   }
 DType aTy=e1->Type();
 DType bTy=e2->Type();
 DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;
 if( maxTy > 100)
   {
     throw GDLException( "Expressions of this type cannot be converted.");
   }

 DType cTy=maxTy;
 if( maxTy == BYTE || maxTy == INT)
   cTy=LONG;
 else if( maxTy == UINT)
   cTy=ULONG;

 if( aTy != cTy)
   {
     e1 = e1->Convert2( cTy, BaseGDL::COPY);
     g1.reset( e1);
   }
 if( bTy != cTy)
   {
     e2 = e2->Convert2( cTy, BaseGDL::COPY);
     g2.reset( e2);
   }

 res=e2->MatrixOp(e1);
 return res;
}
BaseGDL* SLASHNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->Div(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->DivInv(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->Div(e2); 
       }
   }


 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->DivInv(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->Div(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->Div(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->DivInv(e1); // smaller + larger
	 
       }

 return res;
}
BaseGDL* MOD_OPNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2; AdjustTypesNC( g1, e1, g2, e2); 

 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 return e1->Mod(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->ModInv(e1);
	 res->SetDim( e1->Dim());
	 return res;
       }
     else
       {
       return e1->Dup()->Mod(e2); 
       }
   }

 if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->ModInv(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->Mod(e2); // array+scalar
       
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	 res= e1->Mod(e2); // smaller_array + larger_array or same size
	 
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->ModInv(e1); // smaller + larger
	 
       }
 return res;
}


BaseGDL* POWNCNode::Eval()
{ BaseGDL* res;
 auto_ptr<BaseGDL> g1;
 auto_ptr<BaseGDL> g2;
 BaseGDL *e1, *e2;
 if( op1NC)
   {
     e1 = op1->EvalNC();
   }
 else
   {
     e1 = op1->Eval();
     g1.reset( e1);
   }
 if( op2NC)
   {
     e2 = op2->EvalNC();
   }
 else
   {
     e2 = op2->Eval();
     g2.reset( e2);
   }

 DType aTy=e1->Type();
 DType bTy=e2->Type();
 if( ComplexType(aTy))
   {
     if( IntType( bTy))
       {
	 e2 = e2->Convert2( LONG, BaseGDL::COPY);
	 g2.reset( e2);
	 if( g1.get() == NULL) 
	   {
	     // this logic has to follow Pow(...) (basic_op.cpp)
	     //	     if( e2->Scalar() || (e1->N_Elements() < e2->N_Elements()))
	     //	       e1 = e1->Dup(); 
	     return e1->PowNew( e2);
	   }
	 else 
	   {
	     g1.release();
	     return e1->Pow( e2);
	   }
       }
     if( aTy == COMPLEX)
       {
	 if( bTy == DOUBLE)
	   {
	     e1 = e1->Convert2( COMPLEXDBL, BaseGDL::COPY);
	     g1.reset( e1); 
	     aTy = COMPLEXDBL;
	   }
	 else if( bTy == FLOAT)
	   {
	     if( g1.get() == NULL) 
	       {
		 // this logic has to follow Pow(...) (basic_op.cpp)
		 return e1->PowNew( e2);
	       }
	     else 
	       {
		 g1.release();
		 return e1->Pow( e2);
	       }
	   }
       }
     if( aTy == COMPLEXDBL)
       {
	 if( bTy == FLOAT)
	   {
	     e2 = e2->Convert2( DOUBLE, BaseGDL::COPY);
	     g2.reset( e2);
	     bTy = DOUBLE;
	   }
	 if( bTy == DOUBLE)
	   {
	     if( g1.get() == NULL) 
	       {
		 return e1->PowNew( e2);
	       }
	     else
	       {
		 g1.release();
		 return e1->Pow( e2);
	       }
	   }
       }
   }

 DType convertBackT; 
 
 // convert back
 if( IntType( bTy) && (DTypeOrder[ bTy] > DTypeOrder[ aTy]))
   convertBackT = aTy;
 else
   convertBackT = UNDEF;

 if( aTy != bTy) 
   {
     if( aTy > 100 || bTy > 100)
       {
	 throw GDLException( "Expressions of this type cannot be converted.");
       }

     if( DTypeOrder[aTy] >= DTypeOrder[bTy]) // crucial: '>' -> '>='
       {
	 // convert e2 to e1
	 e2 = e2->Convert2( aTy, BaseGDL::COPY);
	 g2.reset( e2); // delete former e2
       }
     else
       {
	 // convert e1 to e2
	 e1 = e1->Convert2( bTy, BaseGDL::COPY);
	 g1.reset( e1); // delete former e1
       }
   }

 // AdjustTypes(e2,e1); // order crucial here (for converting back)
 if( e1->N_Elements() == e2->N_Elements())
   {
     if( g1.get() != NULL)
       {
	 g1.release();
	 res = e1->Pow(e2);
       }
     if( g2.get() != NULL) 
       {
	 g2.release();
	 res = e2->PowInv(e1);
	 res->SetDim( e1->Dim());
       }
     else
       {
	 e1 = e1->Dup();
	 res = e1->Pow(e2); 
       }
   }
 else if( e1->Scalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->PowInv(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->Scalar())
     {
       if( g1.get() == NULL) 
	 {
	   e1 = e1->Dup();
	   res = e1->Pow(e2); // array+scalar
	 }
       else 
	 {
	   g1.release();
	   res= e1->Pow(e2); // array+scalar
	 }
     }
   else
     if( e1->N_Elements() <= e2->N_Elements())
       {
	 if( g1.get() == NULL) 
	   {
	     e1 = e1->Dup();
	     res= e1->Pow(e2); // smaller_array + larger_array or same size
	   }
	 else 
	   {
	     g1.release();
	     res= e1->Pow(e2); // smaller_array + larger_array or same size
	   }
       }
     else
       {
	 if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
	 res= e2->PowInv(e1); // smaller + larger
	 
       }
 if( convertBackT != UNDEF)
   {
     res = res->Convert2( convertBackT, BaseGDL::CONVERT);
   }
 return res;
}
// BaseGDL* DECNCNode::Eval()
// { BaseGDL* res;
//   return new DECNode( refNode);
// }
// BaseGDL* INCNCNode::Eval()
// { BaseGDL* res;
//   return new INCNode( refNode);
// }
// BaseGDL* POSTDECNCNode::Eval()
// { BaseGDL* res;
//   return new POSTDECNode( refNode);
// }
// BaseGDL* POSTINCNCNode::Eval()
// { BaseGDL* res;
//   return new POSTINCNode( refNode);
// }
