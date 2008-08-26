/***************************************************************************
                          prognodeexpr.cpp  -  GDL's AST is made of DNodes
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

#include <memory>

#include <antlr/ASTFactory.hpp>

#include "dinterpreter.hpp"
#include "prognodeexpr.hpp"
#include "basegdl.hpp"
#include "arrayindexlistt.hpp"
#include "envt.hpp"
#include "gdlexception.hpp"

using namespace std;

bool NonCopyNode( int type)
{
  return (type == GDLTokenTypes::DEREF) ||
    (type == GDLTokenTypes::CONSTANT) ||
    (type == GDLTokenTypes::VAR) ||
    (type == GDLTokenTypes::VARPTR);
}
// are always copy nodes
//     (type == GDLTokenTypes::ARRAYDEF) 
//     (type == GDLTokenTypes::STRUC) 
//     (type == GDLTokenTypes::NSTRUC) 
//     (type == GDLTokenTypes::NSTRUC_REF) 

BinaryExpr::BinaryExpr( const RefDNode& refNode): DefaultNode( refNode)
{
  op1 = GetFirstChild();
  op2 = GetFirstChild()->GetNextSibling();
  setType( GDLTokenTypes::EXPR);
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
  libPro( NULL),
  libFun( NULL),
  var( NULL),
  labelStart( 0),
  labelEnd( 0)
{}

// tanslation RefDNode -> ProgNode
ProgNode::ProgNode( const RefDNode& refNode):
  ttype( refNode->getType()),
  text( refNode->getText()),
  down( NULL), 
  right( NULL),
  lineNumber( refNode->getLine()),
  cData( refNode->StealCData()),
  libPro( refNode->libPro),
  libFun( refNode->libFun),
  var( refNode->var),
  arrIxList( refNode->StealArrIxList()),
//   arrIxList( refNode->CloneArrIxList()),
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

BaseGDL* ProgNode::EvalNC()
{
  throw GDLException( this,
		      "Internal error. "
		      "ProgNode::EvalNC() called.");
}

void  ProgNode::Run()
{ 
  throw GDLException( this,
		      "Internal error. "
		      "ProgNode::Run() called.");
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

// checks if parameterlist is constant
bool ConstantPar( ProgNodeP _t)
{

  while(_t != NULL) {
    switch ( _t->getType()) {
    case GDLTokenTypes::KEYDEF_REF:
      {
	return false;
      }
    case GDLTokenTypes::KEYDEF_REF_EXPR:
      {
	return false;
      }
    case GDLTokenTypes::KEYDEF:
      {
// 	ProgNodeP __t162 = _t;

	// 			match(antlr::RefAST(_t),KEYDEF);
// 	_t = _t->getFirstChild();
	// 			match(antlr::RefAST(_t),IDENTIFIER);
// 	_t = _t->getNextSibling();

	if( !_t->getFirstChild()->getNextSibling()->ConstantNode())
	  return false;
			
// 	_t = __t162;
	_t = _t->getNextSibling();
	break;
      }
    case GDLTokenTypes::REF:
      {
	return false;
      }
    case GDLTokenTypes::REF_EXPR:
      {
	return false;
      }
    case GDLTokenTypes::KEYDEF_REF_CHECK:
      {
	return false;
      }
    case GDLTokenTypes::REF_CHECK:
      {
	return false;
      }
    default:
      {
	// expr
	if( !_t->ConstantNode())
	  return false;

	_t = _t->getNextSibling();
	break;
      }
    } // switch
  } // while

  return true;
}





ProgNodeP ProgNode::NewProgNode( const RefDNode& refNode)
{
  // this can happen in case of a subroutine with only common blocks
  if( refNode == RefDNode(antlr::nullAST)) return NULL;

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
  
  // note: constant expressions are always nonCopy
  // but expressions of constant expressions are not
  if( nonCopy) // VAR, VARPTR,...
    {
      UnaryExpr* newUnary = NULL;
      BinaryExpr* newNode = NULL;
      switch( refNode->getType())
	{
	case GDLTokenTypes::QUESTION:
	  {
	    return new QUESTIONNode( refNode);
	  }

	  // unary
	case GDLTokenTypes::UMINUS:
	  {
	    // 	// optimize constant unary minus away
	    // 	// CONSTANT is a non-copy node
	    // 	if( refNode->GetFirstChild()->getType() == GDLTokenTypes::CONSTANT)
	    // 	  {
	    // 	    const RefDNode& child = refNode->GetFirstChild();
	    // 	    child->ResetCData( child->CData()->UMinus());
	    // 	    return NewProgNode( child);
	    // 	  }
	    // 	else
	    newUnary = new UMINUSNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_NEG:
	  {
	    newUnary = new LOG_NEGNode( refNode);
	    break;
	  }
	case GDLTokenTypes::NOT_OP:
	  {
	    newUnary = new NOT_OPNode( refNode);
	    break;
	  }

	  // binary
	case GDLTokenTypes::AND_OP:
	  {
	    newNode = new AND_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::OR_OP:
	  {
	    newNode = new OR_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::XOR_OP:
	  {
	    newNode = new XOR_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_AND:
	  {
	    newNode = new LOG_ANDNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_OR:
	  {
	    newNode = new LOG_ORNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::EQ_OP:
	  {
	    newNode = new EQ_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::NE_OP:
	  {
	    newNode = new NE_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LE_OP:
	  {
	    newNode = new LE_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LT_OP:
	  {
	    newNode = new LT_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GE_OP:
	  {
	    newNode = new GE_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GT_OP:
	  {
	    newNode = new GT_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::PLUS:
	  {
	    newNode = new PLUSNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MINUS:
	  {
	    newNode = new MINUSNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LTMARK:
	  {
	    newNode = new LTMARKNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GTMARK:
	  {
	    newNode = new GTMARKNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::ASTERIX:
	  {
	    newNode = new ASTERIXNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MATRIX_OP1:
	  {
	    newNode = new MATRIX_OP1NCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MATRIX_OP2:
	  {
	    newNode = new MATRIX_OP2NCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::SLASH:
	  {
	    newNode = new SLASHNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MOD_OP:
	  {
	    newNode = new MOD_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::POW:
	  {
	    newNode = new POWNCNode( refNode);
	    break;
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

      if( newNode != NULL)
	{
	  if( !newNode->ConstantExpr()) return newNode;

	  auto_ptr<ProgNode> guard( newNode);

	  BaseGDL* cData = newNode->Eval();

	  ProgNodeP cN = new CONSTANTNode( newNode->StealNextSibling(), cData);
	  cN->lineNumber = refNode->getLine();
	  cN->setText( "C" + refNode->getText());

	  return cN;
	}
      else if( newUnary != NULL)
	{
	  if( !newUnary->ConstantExpr()) return newUnary;

	  auto_ptr<ProgNode> guard( newUnary);

	  BaseGDL* cData = newUnary->Eval();

	  ProgNodeP cN = new CONSTANTNode( newUnary->StealNextSibling(), cData);
	  cN->lineNumber = refNode->getLine();
	  cN->setText( "C" + refNode->getText());

	  return cN;
	}
    }
  else // !nonCopy
    {
      UnaryExpr* newUnary = NULL;
      BinaryExpr* newNode = NULL;
      switch( refNode->getType())
	{
	case GDLTokenTypes::QUESTION:
	  {
	    return new QUESTIONNode( refNode);
	  }

	  // unary
	case GDLTokenTypes::UMINUS:
	  {
	    newUnary = new UMINUSNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_NEG:
	  {
	    newUnary = new LOG_NEGNode( refNode);
	    break;
	  }
	case GDLTokenTypes::NOT_OP:
	  {
	    newUnary = new NOT_OPNode( refNode);
	    break;
	  }

	  // binary
	case GDLTokenTypes::AND_OP:
	  {
	    newNode = new AND_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::OR_OP:
	  {
	    newNode = new OR_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::XOR_OP:
	  {
	    newNode = new XOR_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_AND:
	  {
	    newNode = new LOG_ANDNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_OR:
	  {
	    newNode = new LOG_ORNode( refNode);
	    break;
	  }
	case GDLTokenTypes::EQ_OP:
	  {
	    newNode = new EQ_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::NE_OP:
	  {
	    newNode = new NE_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LE_OP:
	  {
	    newNode = new LE_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LT_OP:
	  {
	    newNode = new LT_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GE_OP:
	  {
	    newNode = new GE_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GT_OP:
	  {
	    newNode = new GT_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::PLUS:
	  {
	    newNode = new PLUSNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MINUS:
	  {
	    newNode = new MINUSNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LTMARK:
	  {
	    newNode = new LTMARKNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GTMARK:
	  {
	    newNode = new GTMARKNode( refNode);
	    break;
	  }
	case GDLTokenTypes::ASTERIX:
	  {
	    newNode = new ASTERIXNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MATRIX_OP1:
	  {
	    newNode = new MATRIX_OP1Node( refNode);
	    break;
	  }
	case GDLTokenTypes::MATRIX_OP2:
	  {
	    newNode = new MATRIX_OP2Node( refNode);
	    break;
	  }
	case GDLTokenTypes::SLASH:
	  {
	    newNode = new SLASHNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MOD_OP:
	  {
	    newNode = new MOD_OPNode( refNode);
	    break;
	  }
	case GDLTokenTypes::POW:
	  {
	    newNode = new POWNode( refNode);
	    break;
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
      if( newNode != NULL)
	{
	  if( !newNode->ConstantExpr()) return newNode;

	  auto_ptr<ProgNode> guard( newNode);

	  BaseGDL* cData = newNode->Eval();

	  ProgNodeP cN = new CONSTANTNode( newNode->StealNextSibling(), cData);
	  cN->lineNumber = refNode->getLine();
	  cN->setText( "C" + refNode->getText());

	  return cN;
	}
      else if( newUnary != NULL)
	{
	  if( !newUnary->ConstantExpr()) return newUnary;

	  auto_ptr<ProgNode> guard( newUnary);

	  BaseGDL* cData = newUnary->Eval();

	  ProgNodeP cN = new CONSTANTNode( newUnary->StealNextSibling(), cData);
	  cN->lineNumber = refNode->getLine();
	  cN->setText( "C" + refNode->getText());

	  return cN;
	}
    }

  // independed of nonCopy:
  switch( refNode->getType())
    {
    case GDLTokenTypes::FCALL_LIB_RETNEW:
      {
	ProgNodeP c = new DefaultNode( refNode);

	if( !static_cast<DLibFunRetNew*>(c->libFun)->RetConstant()
	    || !ConstantPar( c->getFirstChild())) return c;
	
	auto_ptr< ProgNode> guard( c);
	
	BaseGDL* cData = c->Eval();
	
	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
	cN->setText( "C()");
	
	return cN;
      }
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
    case GDLTokenTypes::ARRAYDEF:
      {
	ARRAYDEFNode* c = new ARRAYDEFNode( refNode);
	if( !c->ConstantArray()) return c;

	auto_ptr< ARRAYDEFNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
        cN->setText( "[c]");

	return cN;

      }
    case GDLTokenTypes::ARRAYDEF_CONST:
      {

	ProgNodeP c = new ARRAYDEFNode( refNode);
	auto_ptr< ProgNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
        cN->setText( "[C]");

	return cN;

// 	DNode* cN = new DNode();
//         cN->setType(GDLTokenTypes::CONSTANT);
// 	cN->setText("[CONSTANT]");
// 	cN->setNextSibling( refNode->getNextSibling());

// 	refNode->setNextSibling(antlr::nullAST);

// 	ProgNodeP c = new ARRAYDEFNode( refNode);
// 	auto_ptr< ProgNode> guard( c);
// 	//c->setType(  GDLTokenTypes::ARRAYDEF);

// 	// evaluate constant
// 	BaseGDL* res = c->Eval();

// 	cN->ResetCData( res);

// 	return new CONSTANTNode( RefDNode( cN));
      }
    case GDLTokenTypes::STRUC:
      {
	return new STRUCNode( refNode);
      }
    case GDLTokenTypes::NSTRUC:
      {
	return new NSTRUCNode( refNode);
      }
    case GDLTokenTypes::NSTRUC_REF:
      {
 	return new NSTRUC_REFNode( refNode);
      }
    case GDLTokenTypes::ASSIGN:         
      {
	return new ASSIGNNode( refNode);
      }
    case GDLTokenTypes::ASSIGN_REPLACE:
      {
	return new ASSIGN_REPLACENode( refNode);
      }
    case GDLTokenTypes::PCALL_LIB:
      {
	return new  PCALL_LIBNode( refNode);
      }
    case GDLTokenTypes::MPCALL:
      {
	return new  MPCALLNode( refNode);
      }
    case GDLTokenTypes::MPCALL_PARENT:
      {
	return new  MPCALL_PARENTNode( refNode);
      }
    case GDLTokenTypes::PCALL:
      {
	return new  PCALLNode( refNode);
      }
    case GDLTokenTypes::DEC:
      {
	return new  DECNode( refNode);
      }
    case GDLTokenTypes::INC:
      {
	return new  INCNode( refNode);
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

  // Will be checked by Convert2() function
//   if( DTypeOrder[aTy] > 100 || DTypeOrder[bTy] > 100) // STRUCT, PTR, OBJ
//     {
//       //exception
//       throw GDLException( "Expressions of this type cannot be converted.");
//     }
  
  // COMPLEX op DOUBLE = COMPLEXDBL
  if( (aTy == COMPLEX && bTy == DOUBLE) ||
      (bTy == COMPLEX && aTy == DOUBLE))
    {
      a.reset( a.release()->Convert2( COMPLEXDBL));
      b.reset( b.release()->Convert2( COMPLEXDBL));
      return;
    }

  // Change > to >= JMG
  if( DTypeOrder[aTy] >= DTypeOrder[bTy])
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
  if( aTy == bTy) return;

  // Will be checked by Convert2() function
//   if( DTypeOrder[aTy] > 100 || DTypeOrder[bTy] > 100) // STRUCT, PTR, OBJ
//     {
//       throw GDLException( "Expressions of this type cannot be converted.");
//     }

  // COMPLEX op DOUBLE = COMPLEXDBL
  if( (aTy == COMPLEX && bTy == DOUBLE) ||
      (bTy == COMPLEX && aTy == DOUBLE))
    {
      e2 = e2->Convert2( COMPLEXDBL, BaseGDL::COPY);
      g2.reset( e2); // delete former e2
      e1 = e1->Convert2( COMPLEXDBL, BaseGDL::COPY);
      g1.reset( e1); // delete former e1
      return;
    }

  // Change > to >= JMG
  if( DTypeOrder[aTy] >= DTypeOrder[bTy])
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

BaseGDL* VARNode::EvalNC()
{
      EnvStackT& callStack=interpreter->CallStack();
      BaseGDL* res=static_cast<EnvUDT*>(callStack.back())->GetKW(this->varIx); 
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

// trinary operator
BaseGDL* QUESTIONNode::Eval()
{
  auto_ptr<BaseGDL> e1( op1->Eval());
  if( e1->True())
    {
      return op2->Eval(); // right->down
    }
  return op3->Eval(); // right->right
}

// unary operators
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

// binary operators
BaseGDL* AND_OPNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  auto_ptr<BaseGDL> e2( op2->Eval());
  AdjustTypes(e1,e2);
  if( e1->StrictScalar()) 
    {
    res= e2->AndOpS(e1.get()); // scalar+scalar or array+scalar
    e2.release();
    }
  else
    if( e2->StrictScalar())
      {
      res= e1->AndOpInvS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->OrOpS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->OrOpInvS(e2.get()); // array+scalar
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
  if( !e1->LogTrue()) return new DByteGDL( 0);
  auto_ptr<BaseGDL> e2( op2->Eval());
  if( !e2->LogTrue()) return new DByteGDL( 0);
  return new DByteGDL( 1);
}
BaseGDL* LOG_ORNode::Eval()
{ BaseGDL* res;
  auto_ptr<BaseGDL> e1( op1->Eval());
  if( e1->LogTrue()) return new DByteGDL( 1); 
  auto_ptr<BaseGDL> e2( op2->Eval());
  if( e2->LogTrue()) return new DByteGDL( 1);
  return new DByteGDL( 0);
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
 if( e1->StrictScalar())
   {
     res= e2->AddInvS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->AddS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->SubInvS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->SubS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->LtMarkS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->LtMarkS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->GtMarkS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->GtMarkS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->MultS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->MultS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->DivInvS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->DivS(e2.get()); // array+scalar
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
 if( e1->StrictScalar())
   {
     res= e2->ModInvS(e1.get()); // scalar+scalar or array+scalar
     e2.release();
   }
 else
   if( e2->StrictScalar())
     {
       res= e1->ModS(e2.get()); // array+scalar
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
  // special handling for aTy == complex && bTy != complex
  DType aTy=e1->Type();
  DType bTy=e2->Type();
 if( aTy == STRING)
   {
     e1.reset( e1->Convert2( FLOAT, BaseGDL::COPY));
     aTy = FLOAT;
   }
 if( bTy == STRING)
   {
     e2.reset( e2->Convert2( FLOAT, BaseGDL::COPY));
     bTy = FLOAT;
   }
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

  if( IntType( bTy) && FloatType( aTy))
    {
      e2.reset( e2.release()->Convert2( LONG));

      res = e1->PowInt( e2.get());
      if( res == e1.get())
	e1.release();

      return res;
    }
  
  DType convertBackT; 

  // convert back
  if( IntType( bTy) && (DTypeOrder[ bTy] > DTypeOrder[ aTy]))
//   if( IntType( bTy) && (DTypeOrder[ bTy] > DTypeOrder[ aTy]))
    convertBackT = aTy;
  else
    convertBackT = UNDEF;
  
  //  first operand determines type JMG
  //  AdjustTypes(e2,e1); // order crucial here (for converting back)
  AdjustTypes(e1,e2); // order crucial here (for converting back)

  if( e1->StrictScalar())
    {
      res= e2->PowInvS(e1.get()); // scalar+scalar or array+scalar
      e2.release();
    }
  else
    if( e2->StrictScalar())
      {
      res= e1->PowS(e2.get()); // array+scalar
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->AndOpS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->AndOpInvS(e2); // array+scalar
       
     }
   else
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
     else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->OrOpS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->OrOpInvS(e2); // array+scalar
       
     }
   else
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
else if( e1->N_Elements() < e2->N_Elements())
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

  if( e1->StrictScalar())
    {
      if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
      res= e2->XorOpS(e1); // scalar+scalar or array+scalar
     
    }
  else if( e2->StrictScalar())
      {
	if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
	res= e1->XorOpS(e2); // array+scalar
       
      }
    else if( e1->N_Elements() == e2->N_Elements())
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
    else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->AddInvS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->AddS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->SubInvS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->SubS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
       return e1->SubNew(e2); 
       }
   }
else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->LtMarkS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->LtMarkS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
   else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->GtMarkS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->GtMarkS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->MultS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->MultS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->DivInvS(e1); // scalar+scalar or array+scalar
     
   }
 else
   if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->DivS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
else if( e1->N_Elements() < e2->N_Elements())
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

 if( e1->StrictScalar())
   {
     if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
     res= e2->ModInvS(e1); // scalar+scalar or array+scalar
     
   }
 else if( e2->StrictScalar())
     {
       if( g1.get() == NULL) e1 = e1->Dup(); else g1.release();
       res= e1->ModS(e2); // array+scalar
       
     }
   else if( e1->N_Elements() == e2->N_Elements())
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
else if( e1->N_Elements() < e2->N_Elements())
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
{
  BaseGDL* res;
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

  if( aTy == STRING)
    {
      e1 = e1->Convert2( FLOAT, BaseGDL::COPY);
      g1.reset( e1);
      aTy = FLOAT;
    }
  if( bTy == STRING)
    {
      e2 = e2->Convert2( FLOAT, BaseGDL::COPY);
      g2.reset( e2);
      bTy = FLOAT;
    }

  if( ComplexType(aTy))
    {
      if( IntType( bTy))
	{
	  if( bTy != LONG)
	    {
	      e2 = e2->Convert2( LONG, BaseGDL::COPY);
	      g2.reset( e2);
	    }
	  if( g1.get() == NULL) 
	    {
	      return e1->PowNew( e2);
	    }
	  else 
	    {
	      res = g1->Pow( e2);
	      if( res == g1.get())
		g1.release();
	      return res;
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
		  return e1->PowNew( e2);
		}
	      else 
		{
		  res = g1->Pow( e2);
		  if( res == g1.get())
		    g1.release();
		  return res;
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
		  res = g1->Pow( e2);
		  if( res == g1.get())
		    g1.release();
		  return res;
		}
	    }
	}
    }

  if( IntType( bTy) && FloatType( aTy))
    {
      if( bTy != LONG)
	{
	  e2 = e2->Convert2( LONG, BaseGDL::COPY);
	  g2.reset( e2);
	}
      if( g1.get() == NULL)
	res = e1->PowIntNew( e2);
      else 
	{
	  res = g1->PowInt( e2);
	  if( res == g1.get())
	    g1.release();
	}
      return res;
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
if( e1->StrictScalar())
    {
      if( g2.get() == NULL) e2 = e2->Dup(); else g2.release();
      res= e2->PowInvS(e1); // scalar+scalar or array+scalar
     
    }
  else if( e2->StrictScalar())
    {
      if( g1.get() == NULL) 
	{
	  e1 = e1->Dup();
	  res = e1->PowS(e2); // array+scalar
	}
      else 
	{
	  g1.release();
	  res= e1->PowS(e2); // array+scalar
	}
    }
  else if( e1->N_Elements() == e2->N_Elements())
    {
      if( g1.get() != NULL)
	{
	  g1.release();
	  res = e1->Pow(e2);
	}
      else if( g2.get() != NULL) 
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
  else if( e1->N_Elements() < e2->N_Elements())
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
