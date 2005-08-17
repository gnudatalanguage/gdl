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
  
  if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
    {
      down = new ProgNode( refNode->GetFirstChild());
    }
  if( refNode->GetNextSibling() != RefDNode(antlr::nullAST))
    {
      right = new ProgNode( refNode->GetNextSibling());
    }
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
  switch( refNode->getType())
    {
	case QUESTION:
	{
	  return new QUESTIONNode( refNode);
	}
	case UMINUS:
	{
		return new UMINUSNode( refNode);
	}
	case LOG_NEG:
	{
		return new LOG_NEGNode( refNode);
	}
	case NOT_OP:
	{
		return new NOT_OPNode( refNode);
	}
	case AND_OP:
	{
		return new AND_OPNode( refNode);
	}
	case OR_OP:
	{
		return new OR__OPNode( refNode);
	}
	case XOR_OP:
	{
		return new XOR_OPNode( refNode);
	}
	case LOG_AND:
	{
		return new LOG_ANDNode( refNode);
	}
	case LOG_OR:
	{
		return new LOG_ORNode( refNode);
	}
	case EQ_OP:
	{
		return new EQ_OPNode( refNode);
	}
	case NE_OP:
	{
		return new NE_OPNode( refNode);
	}
	case LE_OP:
	{
		return new LE_OPNode( refNode);
	}
	case LT_OP:
	{
		return new LT_OPNode( refNode);
	}
	case GE_OP:
	{
		return new GE_OPNode( refNode);
	}
	case GT_OP:
	{
		return new GT_OPNode( refNode);
	}
	case PLUS:
	{
		return new PLUSNode( refNode);
	}
	case MINUS:
	{
		return new MINUSNode( refNode);
	}
	case LTMARK:
	{
		return new LTMARKNode( refNode);
	}
	case GTMARK:
	{
		return new GTMARKNode( refNode);
	}
	case ASTERIX:
	{
		return new ASTERIXNode( refNode);
	}
	case MATRIX_OP1:
	{
		return new MATRIX_OP1Node( refNode);
	}
	case MATRIX_OP2:
	{
		return new MATRIX_OP2Node( refNode);
	}
	case SLASH:
	{
		return new SLASHNode( refNode);
	}
	case MOD_OP:
	{
		return new MOD_OPNode( refNode);
	}
	case POW:
	{
		return new POWNode( refNode);
	}
	case DEC:
	{
		return new DECNode( refNode);
	}
	case INC:
	{
		return new INCNode( refNode);
	}
	case POSTDEC:
	{
		return new POSTDECNode( refNode);
	}
	case POSTINC:
	{
		return new POSTINCNode( refNode);
	}
    }
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
      b.reset(b->Convert2( aTy));
    }
  else
    {
      // convert a to b
      a.reset(a->Convert2( bTy));
    }
}

BaseGDL* QUESTIONNode::Eval()
{
  BaseGDL* e1 = right->Eval();
  auto_ptr<BaseGDL> e1_guard(e1);
  if( e1->True())
    {
      return down->right->Eval(); 
    }
  return down->down->Eval(); 
}

BaseGDL* UMINUSNode::Eval()
{
  return right->UMinus();
}
BaseGDL* LOG_NEGNode::Eval()
{
  return right->LogNeg();
}
BaseGDL* NOT_OPNode::Eval()
{
  return right->NotOp();
}
BaseGDL* AND_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->AndOp(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->AndOpInv(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->AndOpInv(e2); // smaller_array + larger_array or same size
      else
	res= e2->AndOp(e1); // smaller + larger
  }
  e1->release();
  e2->release();
  return res;
}
BaseGDL* OR_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->OrOp(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->OrOpInv(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->OrOpInv(e2); // smaller_array + larger_array or same size
      else
	res= e2->OrOp(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* XOR_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->N_Elements() <= e2->N_Elements())
    res= e1->XorOp(e2); // smaller_array + larger_array or same size
  else
    res= e2->XorOp(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* LOG_ANDNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  if( !e1->LogTrue()) {res = new DByteGDL( 0); break;}
  if( !e2->LogTrue()) {res = new DByteGDL( 0); break;}
  res = new DByteGDL( 1);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* LOG_ORNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  if( !e1->LogTrue()) {res = new DByteGDL( 1); break;}
  if( !e2->LogTrue()) {res = new DByteGDL( 1); break;}
  res = new DByteGDL( 0);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* EQ_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  res=e1->EqOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* NE_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  res=e1->NeOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* LE_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  res=e1->LeOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* LT_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  res=e1->LtOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* GE_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  res=e1->GeOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* GT_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  res=e1->GtOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* PLUSNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->AddInv(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->Add(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->Add(e2); // smaller_array + larger_array or same size
      else
  e1->release();
  e2->release();
  return res;
	res= e2->AddInv(e1); // smaller + larger
}
BaseGDL* MINUSNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->SubInv(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->Sub(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->Sub(e2); // smaller_array + larger_array or same size
      else
	res= e2->SubInv(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* LTMARKNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->LtMark(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->LtMark(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->LtMark(e2); // smaller_array + larger_array or same size
      else
	res= e2->LtMark(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* GTMARKNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->GtMark(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->GtMark(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->GtMark(e2); // smaller_array + larger_array or same size
      else
	res= e2->GtMark(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* ASTERIXNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->Mult(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->Mult(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->Mult(e2); // smaller_array + larger_array or same size
      else
	res= e2->Mult(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* MATRIX_OP1Node::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  DType aTy=e1->Type();
  DType bTy=e2->Type();
  DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;

  DType cTy=maxTy;
  if( maxTy == BYTE || maxTy == INT)
    cTy=LONG;
  else if( maxTy == UINT)
    cTy=ULONG;

  if( aTy != cTy) e1=e1->Convert2( cTy);

  AdjustTypes(e1,e2);
  res=e1->MatrixOp(e2);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* MATRIX_OP2Node::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  DType aTy=e1->Type();
  DType bTy=e2->Type();
  DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;

  DType cTy=maxTy;
  if( maxTy == BYTE || maxTy == INT)
    cTy=LONG;
  else if( maxTy == UINT)
    cTy=ULONG;

  if( aTy != cTy) e1=e1->Convert2( cTy);

  AdjustTypes(e1,e2);
  res=e2->MatrixOp(e1);
  e1->release();
  e2->release();
  return res;
}
BaseGDL* SLASHNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->DivInv(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->Div(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->Div(e2); // smaller_array + larger_array or same size
      else
	res= e2->DivInv(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* MOD_OPNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  AdjustTypes(e1,e2);
  if( e1->Scalar())
    res= e2->ModInv(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->Mod(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->Mod(e2); // smaller_array + larger_array or same size
      else
	res= e2->ModInv(e1); // smaller + larger
  e1->release();
  e2->release();
  return res;
}
BaseGDL* POWNode::Eval()
{
  auto_ptr<BaseGDL> e1 = right->Eval();
  auto_ptr<BaseGDL> e2 = down->Eval();
  // special handling for complex
  DType aTy=e1->Type();
  if( aTy == COMPLEX)
    {
      DType bTy=e2->Type();
      if( IntType( bTy))
	{
	  e2 = e2->Convert2( FLOAT);
	  res = e1->Pow( e2);
	  goto endPOW;
	}
      else if( bTy == FLOAT)
	{
	  res = e1->Pow( e2);
	  goto endPOW;
	}
    }
  else if( aTy == COMPLEXDBL)
    {
      DType bTy=e2->Type();
      if( IntType( bTy))
	{
	  e2 = e2->Convert2( DOUBLE);
	  res = e1->Pow( e2);
	  goto endPOW;
	}
      else if( bTy == DOUBLE)
	{
	  res = e1->Pow( e2);
	  goto endPOW;
	}
    }

  DType convertBackT; 

  // convert back
  if( IntType( e2->Type()) && 
      DTypeOrder[e2->Type()] > DTypeOrder[e1->Type()])
    convertBackT = e1->Type();
  else
    convertBackT = UNDEF;

  AdjustTypes(e2,e1); // order crucial here (for converting back)

  if( e1->Scalar())
    res= e2->PowInv(e1); // scalar+scalar or array+scalar
  else
    if( e2->Scalar())
      res= e1->Pow(e2); // array+scalar
    else
      if( e1->N_Elements() <= e2->N_Elements())
	res= e1->Pow(e2); // smaller_array + larger_array or same size
      else
	res= e2->PowInv(e1); // smaller + larger
  if( convertBackT != UNDEF)
    {
      res = res->Convert2( convertBackT, BaseGDL::CONVERT);
    }
 endPOW:
  e1->release();
  e2->release();
  return res;
}
BaseGDL* DECNode::Eval()
{
  return new DECNode( refNode);
}
BaseGDL* INCNode::Eval()
{
  return new INCNode( refNode);
}
BaseGDL* POSTDECNode::Eval()
{
  return new POSTDECNode( refNode);
}
BaseGDL* POSTINCNode::Eval()
{
  return new POSTINCNode( refNode);
}
