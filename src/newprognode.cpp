/***************************************************************************
                          newprognode.cpp  -  translation from DNodes to ProgNode (our final 'code')
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

// print out AST tree
#define GDL_DEBUG
//#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include "print_tree.hpp"
#endif


using namespace std;

// helper function for NewProgNode( const RefDNode& refNode)
// checks if parameterlist is all constant
// used for constant evaluation at compile time
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



// here the transcription from DNode to ProgNode takes place
// references are resolved
ProgNodeP ProgNode::NewProgNode( const RefDNode& refNode)
{
  // this can happen in case of a subroutine with only common blocks
  if( refNode == RefDNode(antlr::nullAST)) return NULL;

  bool nonCopy = false;
  bool nonCopy12 = false;
  if( refNode->GetFirstChild() != RefDNode(antlr::nullAST))
    {
      if( NonCopyNode( refNode->GetFirstChild()->getType()))
	nonCopy = true;
      if( refNode->GetFirstChild()->GetNextSibling() != RefDNode(antlr::nullAST))
		if( NonCopyNode( refNode->GetFirstChild()->GetNextSibling()->getType()))
		{
		  if( nonCopy)
		    nonCopy12 = true;
		  else  
		    nonCopy = true;
		}
    }
  
  // note: constant expressions are always nonCopy
  // but expressions of constant expressions are not
  if( nonCopy) // VAR, VARPTR,...
    {
      UnaryExpr* newUnary = NULL;
      BinaryExpr* newNode = NULL;
      switch( refNode->getType())
	{
// 	case GDLTokenTypes::QUESTION:
// 	  {
// 	    return new QUESTIONNode( refNode);
// 	  }
// 
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
	    if( nonCopy12)
	      newNode = new AND_OPNCNode( refNode);
	    else
	      newNode = new AND_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::OR_OP:
	  {
	    if( nonCopy12)
	    newNode = new OR_OPNCNode( refNode);
	    else
	    newNode = new OR_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::XOR_OP:
	  {
	    if( nonCopy12)
	    newNode = new XOR_OPNCNode( refNode);
	    else
	    newNode = new XOR_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_AND:
	  {
	    if( nonCopy12)
	    newNode = new LOG_ANDNCNode( refNode);
	    else
	    newNode = new LOG_ANDNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LOG_OR:
	  {
	    if( nonCopy12)
	    newNode = new LOG_ORNCNode( refNode);
	    else
	    newNode = new LOG_ORNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::EQ_OP:
	  {
	    if( nonCopy12)
	    newNode = new EQ_OPNCNode( refNode);
	    else
	    newNode = new EQ_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::NE_OP:
	  {
	    if( nonCopy12)
	    newNode = new NE_OPNCNode( refNode);
	    else
	    newNode = new NE_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LE_OP:
	  {
	    if( nonCopy12)
	    newNode = new LE_OPNCNode( refNode);
	    else
	    newNode = new LE_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LT_OP:
	  {
	    if( nonCopy12)
	    newNode = new LT_OPNCNode( refNode);
	    else
	    newNode = new LT_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GE_OP:
	  {
	    if( nonCopy12)
	    newNode = new GE_OPNCNode( refNode);
	    else
	    newNode = new GE_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GT_OP:
	  {
	    if( nonCopy12)
	    newNode = new GT_OPNCNode( refNode);
	    else
	    newNode = new GT_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::PLUS:
	  {
	    if( nonCopy12)
	    newNode = new PLUSNC12Node( refNode);
	    else
	    newNode = new PLUSNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MINUS:
	  {
	    if( nonCopy12)
	    newNode = new MINUSNC12Node( refNode);
	    else
	    newNode = new MINUSNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::LTMARK:
	  {
	    if( nonCopy12)
	    newNode = new LTMARKNCNode( refNode);
	    else
	    newNode = new LTMARKNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::GTMARK:
	  {
	    if( nonCopy12)
	    newNode = new GTMARKNCNode( refNode);
	    else
	    newNode = new GTMARKNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::ASTERIX:
	  {
	    if( nonCopy12)
	    newNode = new ASTERIXNC12Node( refNode);
	    else
	    newNode = new ASTERIXNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MATRIX_OP1:
	  {
	    if( nonCopy12)
	    newNode = new MATRIX_OP1NCNode( refNode);
	    else
	    newNode = new MATRIX_OP1NCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MATRIX_OP2:
	  {
	    if( nonCopy12)
	    newNode = new MATRIX_OP2NCNode( refNode);
	    else
	    newNode = new MATRIX_OP2NCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::SLASH:
	  {
	    if( nonCopy12)
	    newNode = new SLASHNC12Node( refNode);
	    else
	    newNode = new SLASHNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::MOD_OP:
	  {
	    if( nonCopy12)
	    newNode = new MOD_OPNCNode( refNode);
	    else
	    newNode = new MOD_OPNCNode( refNode);
	    break;
	  }
	case GDLTokenTypes::POW:
	  {
	    if( nonCopy12)
	    newNode = new POWNCNode( refNode);
	    else
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

	  Guard<ProgNode> guard( newNode);

	  BaseGDL* cData = newNode->Eval();

	  ProgNodeP cN = new CONSTANTNode( newNode->StealNextSibling(), cData);
	  cN->lineNumber = refNode->getLine();
	  cN->setText( "C" + refNode->getText());

	  return cN;
	}
      else if( newUnary != NULL)
	{
	  if( !newUnary->ConstantExpr()) return newUnary;

	  Guard<ProgNode> guard( newUnary);

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
// 	case GDLTokenTypes::QUESTION:
// 	  {
// 	    return new QUESTIONNode( refNode);
// 	  }
// 
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

	  Guard<ProgNode> guard( newNode);

	  BaseGDL* cData = newNode->Eval();

	  ProgNodeP cN = new CONSTANTNode( newNode->StealNextSibling(), cData);
	  cN->lineNumber = refNode->getLine();
	  cN->setText( "C" + refNode->getText());

	  return cN;
	}
      else if( newUnary != NULL)
	{
	  if( !newUnary->ConstantExpr()) return newUnary;

	  Guard<ProgNode> guard( newUnary);

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
    case GDLTokenTypes::QUESTION:
      {
	return new QUESTIONNode( refNode);
      }

    case GDLTokenTypes::FCALL_LIB_RETNEW:
      {
	ProgNodeP c = new FCALL_LIB_RETNEWNode( refNode);

	if( !static_cast<DLibFunRetNew*>(c->libFun)->RetConstant()
	    || !ConstantPar( c->getFirstChild())) return c;

	Guard< ProgNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
	cN->setText( "C()");

	return cN;
      }
    case GDLTokenTypes::FCALL_LIB_DIRECT:
      {
	ProgNodeP c = new FCALL_LIB_DIRECTNode( refNode);

	if( !static_cast<DLibFunDirect*>(c->libFun)->RetConstant()
	    || !ConstantPar( c->getFirstChild())) return c;

	Guard< ProgNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
	cN->setText( "C()");

	return cN;
      }
    case GDLTokenTypes::FCALL_LIB_N_ELEMENTS:
      {
	ProgNodeP c = new FCALL_LIB_N_ELEMENTSNode( refNode);

	if( !static_cast<DLibFunRetNew*>(c->libFun)->RetConstant()
	    || !ConstantPar( c->getFirstChild())) return c;

	Guard< ProgNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
	cN->setText( "C()");

	return cN;
      }

    case GDLTokenTypes::FCALL_LIB:
      {
		return new FCALL_LIBNode( refNode);
      }
    case GDLTokenTypes::MFCALL:
      {
		return new MFCALLNode( refNode);
      }
    case GDLTokenTypes::MFCALL_PARENT:
      {
		return new MFCALL_PARENTNode( refNode);
      }
    case GDLTokenTypes::FCALL:
      {
		return new FCALLNode( refNode);
      }
    case GDLTokenTypes::ARRAYEXPR_FCALL:
      {
		return new ARRAYEXPR_FCALLNode( refNode);
      }
    case GDLTokenTypes::ARRAYEXPR_MFCALL:
      {
		return new ARRAYEXPR_MFCALLNode( refNode);
      }

    case GDLTokenTypes::ARRAYEXPR:
      {
	return new ARRAYEXPRNode( refNode);
      }
    case GDLTokenTypes::EXPR:
      {
	return new EXPRNode( refNode);
      }
    case GDLTokenTypes::DOT:
      {
	return new DOTNode( refNode);
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
//   cout << "CONSTANT RefDNode:" << endl;
//   antlr::print_tree pt;
//   pt.pr_tree(static_cast<antlr::RefAST>( refNode));
//   cout << "1st cData: " << refNode->getType() << " "<< refNode->getText() << " " << refNode->CData() << endl;
//   cout << endl;
// 	  
//       ProgNodeP c = new CONSTANTNode( refNode);
//       
//   cout << "CONSTANT ProgNodeP:" << endl;
//   pt.pr_tree(c);
//   cout << endl;
//       
//       return c;
  
	return new CONSTANTNode( refNode);
      }
    case GDLTokenTypes::ARRAYDEF_GENERALIZED_INDGEN:
      {
	ARRAYDEF_GENERALIZED_INDGENNode* c = new ARRAYDEF_GENERALIZED_INDGENNode( refNode);
	if( !c->ConstantArray()) return c;

	Guard< ARRAYDEF_GENERALIZED_INDGENNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
        cN->setText( "[c1:c2:c3]");

	return cN;

      }
  case GDLTokenTypes::ARRAYDEF:
      {
	ARRAYDEFNode* c = new ARRAYDEFNode( refNode);
	if( !c->ConstantArray()) return c;

	Guard< ARRAYDEFNode> guard( c);

	BaseGDL* cData = c->Eval();

	ProgNodeP cN = new CONSTANTNode( c->StealNextSibling(), cData);
	cN->lineNumber = refNode->getLine();
        cN->setText( "[c]");

	return cN;

      }
    case GDLTokenTypes::ARRAYDEF_CONST:
      {
//   cout << "ARRAYDEF_CONST RefDNode:" << endl;
//   antlr::print_tree pt;
//   pt.pr_tree(static_cast<antlr::RefAST>( refNode));
//   cout << endl;

	ARRAYDEFNode* c = new ARRAYDEFNode( refNode);

//   cout << "ARRAYDEF_CONST ProgNodeP:" << endl;
//   pt.pr_tree(c);
//   cout << endl;
	assert( c->ConstantArray());

	Guard< ProgNode> guard( c);

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
// 	Guard< ProgNode> guard( c);
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
    case GDLTokenTypes::ASSIGN_ARRAYEXPR_MFCALL:
      {
	return new ASSIGN_ARRAYEXPR_MFCALLNode( refNode);
      }
    case GDLTokenTypes::ASSIGN_REPLACE:
      {
	return new ASSIGN_REPLACENode( refNode);
      }
    case GDLTokenTypes::GOTO:
      {
	return new  GOTONode( refNode);
      }
    case GDLTokenTypes::BREAK:
      {
	return new  BREAKNode( refNode);
      }
    case GDLTokenTypes::CONTINUE:
      {
	return new  CONTINUENode( refNode);
      }
    case GDLTokenTypes::LABEL:
      {
	return new  LABELNode( refNode);
      }
    case GDLTokenTypes::ON_IOERROR:
      {
	return new  ON_IOERRORNode( refNode);
      }
    case GDLTokenTypes::ON_IOERROR_NULL:
      {
	return new  ON_IOERROR_NULLNode( refNode);
      }
    case GDLTokenTypes::RETF:
      {
	return new  RETFNode( refNode);
      }
    case GDLTokenTypes::RETP:
      {
	return new  RETPNode( refNode);
      }
    case GDLTokenTypes::FOR:
      {
	return new  FORNode( refNode);
      }
    case GDLTokenTypes::FOR_STEP:
      {
	return new  FOR_STEPNode( refNode);
      }
    case GDLTokenTypes::FOREACH:
      {
	return new  FOREACHNode( refNode);
      }
    case GDLTokenTypes::FOREACH_INDEX:
      {
	return new  FOREACH_INDEXNode( refNode);
      }
    case GDLTokenTypes::WHILE:
      {
	return new  WHILENode( refNode);
      }
    case GDLTokenTypes::REPEAT:
      {
	return new  REPEATNode( refNode);
      }
    case GDLTokenTypes::CASE:
      {
	return new  CASENode( refNode);
      }
    case GDLTokenTypes::SWITCH:
      {
	return new  SWITCHNode( refNode);
      }
    case GDLTokenTypes::BLOCK:
      {
	return new  BLOCKNode( refNode);
      }
    case GDLTokenTypes::IF:
      {
	return new  IFNode( refNode);
      }
    case GDLTokenTypes::IF_ELSE:
      {
	return new  IF_ELSENode( refNode);
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
    case GDLTokenTypes::POSTDEC:
      {
	return new  POSTDECNode( refNode);
      }
    case GDLTokenTypes::POSTINC:
      {
	return new  POSTINCNode( refNode);
      }
    case GDLTokenTypes::DEC:
      {
	return new  DECNode( refNode);
      }
    case GDLTokenTypes::INC:
      {
	return new  INCNode( refNode);
      }
      // the following must change their type if precalculation strikes
    case GDLTokenTypes::KEYDEF_REF:
    {
      ProgNodeP nn = new KEYDEF_REFNode( refNode);

      if( !nn->getFirstChild()->getNextSibling()->ConstantNode())
	return nn;

// 		  nn->setType( GDLTokenTypes::KEYDEF);
      delete nn;
      nn = new KEYDEFNode( refNode);
      return nn;
    }
    case GDLTokenTypes::KEYDEF_REF_EXPR:
    {
      ProgNodeP nn = new KEYDEF_REF_EXPRNode( refNode);

      if( !nn->getFirstChild()->getNextSibling()->ConstantNode())
	return nn;

      delete nn;
      nn = new KEYDEFNode( refNode);
// 		  nn->setType( GDLTokenTypes::KEYDEF);
      return nn;
    }
    case  GDLTokenTypes::PARAEXPR:
      {
      ProgNodeP firstChild = 
	new ParameterNode( refNode);
      return firstChild;
      }
    case  GDLTokenTypes::PARAEXPR_VN:
      {
      ProgNodeP firstChild = 
	new ParameterVNNode( refNode);
      return firstChild;
      }
    case  GDLTokenTypes::KEYDEF:
      {
      ProgNodeP nn = new KEYDEFNode( refNode);
      return nn;  
      }
    case GDLTokenTypes::REF:
    {
      ProgNodeP nn = new REFNode( refNode);

      if( !nn->getFirstChild()->ConstantNode())
	return nn;
      
      delete nn;
      
      ProgNodeP firstChild = 
	new ParameterNode( refNode);//->GetFirstChild());
      return firstChild;
    }
    case GDLTokenTypes::REF_VN:
    {
      ProgNodeP nn = new REFVNNode( refNode);

      if( !nn->getFirstChild()->ConstantNode())
	return nn;
      
      delete nn;
      
      ProgNodeP firstChild = 
	new ParameterNode( refNode);//->GetFirstChild());
      return firstChild;
    }
    case GDLTokenTypes::REF_EXPR:
    {
      ProgNodeP nn = new REF_EXPRNode( refNode);

      if( !nn->getFirstChild()->ConstantNode())
	return nn;
      
      delete nn;
      ProgNodeP firstChild = 
	new ParameterNode( refNode);//->GetFirstChild());
      return firstChild;

// 		  Guard<ProgNode> guard(nn);
// 		  return nn->StealFirstChild();
    }
    case GDLTokenTypes::REF_EXPR_VN:
    {
      ProgNodeP nn = new REF_EXPRVNNode( refNode);

      if( !nn->getFirstChild()->ConstantNode())
	return nn;
      
      delete nn;
      ProgNodeP firstChild = 
	new ParameterNode( refNode);//->GetFirstChild());
      return firstChild;

// 		  Guard<ProgNode> guard(nn);
// 		  return nn->StealFirstChild();
    }
    case GDLTokenTypes::KEYDEF_REF_CHECK:
    {
      ProgNodeP nn = new KEYDEF_REF_CHECKNode( refNode);

      if( nn->getFirstChild()->getNextSibling()->getType() != 
	  GDLTokenTypes::CONSTANT) return nn;

      delete nn;
      nn = new KEYDEFNode( refNode);
      return nn;
    }
    case GDLTokenTypes::REF_CHECK:
    {
      ProgNodeP nn = new REF_CHECKNode( refNode);

      if( !nn->getFirstChild()->ConstantNode())
	return nn;
      
      delete nn;
      ProgNodeP firstChild = 
	new ParameterNode( refNode);
      return firstChild;

// 		  Guard<ProgNode> guard(nn);
// 		  ProgNodeP firstChild = nn->StealFirstChild();
// 		  firstChild->SetNextSibling( nn->StealNextSibling());
// 		  return firstChild;
    }
    case GDLTokenTypes::REF_CHECK_VN:
    {
      ProgNodeP nn = new REF_CHECKVNNode( refNode);

      if( !nn->getFirstChild()->ConstantNode())
	return nn;
      
      delete nn;
      ProgNodeP firstChild = 
	new ParameterNode( refNode);
      return firstChild;

// 		  Guard<ProgNode> guard(nn);
// 		  ProgNodeP firstChild = nn->StealFirstChild();
// 		  firstChild->SetNextSibling( nn->StealNextSibling());
// 		  return firstChild;
    }
    }

  // default
  return new DefaultNode( refNode);
}
