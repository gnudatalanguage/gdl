/***************************************************************************
			prognode_lexpr.cpp  -  LExpr functions
			-------------------
begin                : July 22 2002
copyright            : (C) 2011 by Marc Schellens
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

// from GDLInterpreter::l_expr
#include <cassert>
#include <string>

#include "dinterpreter.hpp"
#include "prognodeexpr.hpp"
#include "basegdl.hpp"
#include "arrayindexlistt.hpp"
//#include "envt.hpp"
#include "gdlexception.hpp"
#include "nullgdl.hpp"

// illegal
BaseGDL** ProgNode::LExpr( BaseGDL* right)
//	default:
{
	//   case ARRAYDEF:
	//   case EXPR:
	//   case NSTRUC:
	//   case NSTRUC_REF:
	//   case POSTDEC:
	//   case POSTINC:
	//   case STRUC:
	//   case DEC:
	//   case INC:
	//   case CONSTANT:
	throw GDLException( this, "Expression not allowed as l-value.",
				  true,false);
	return NULL; // avoid compiler warning
}


BaseGDL** QUESTIONNode::LExpr( BaseGDL* right)
		//case QUESTION:
{
    ProgNodeP branch = this->GetBranch();
    return branch->LExpr( right);
    
//     ProgNodeP _t = this->getFirstChild();
// 
//     Guard<BaseGDL> e1_guard;
//     BaseGDL* e1;
//     if( NonCopyNode( _t->getType()))
//     {
//         e1 = _t->EvalNC();
//     }
//     else
//     {
//         BaseGDL** ref = _t->EvalRefCheck(e1);
//         if( ref == NULL)
//             e1_guard.Init(e1);
//         else
//             e1 = *ref;
//     }
// // 	BaseGDL*       e1=interpreter->expr(_t);
//     _t = _t->GetNextSibling();
// // 	Guard<BaseGDL> e1_guard(e1);
//     if( e1->True())
//     {
//         return _t->LExpr( right); //l_expr(_t, right);
//     }
//     else
//     {
//         _t=_t->GetNextSibling(); // jump over 1st expression
//         return _t->LExpr( right); //l_expr(_t, right);
//     }
//     //SetRetTree( tIn->getNextSibling());
//     //return res;
}

BaseGDL** ARRAYEXPRNode::LExpr( BaseGDL* right) // 'right' is not owned
	//case ARRAYEXPR:
  {
  //res=l_array_expr(_t, right);
  if( right == NULL)
    throw GDLException( this, "Indexed expression not allowed in this context.", true,false);

  ArrayIndexListT* aL;
  ArrayIndexListGuard guard;
  BaseGDL** res;
//   try{
// 	res=interpreter->l_indexable_expr( this->getFirstChild());
  res = this->getFirstChild()->LEval(); // throws
  if( *res == NULL) 
  { // ERROR
      // check not needed for SYSVAR 
      ProgNodeP _t = this->getFirstChild();
      assert( _t->getType() != GDLTokenTypes::SYSVAR);
      if( _t->getType() == GDLTokenTypes::VARPTR)
      {
	  GDLException ex( _t, "Common block variable is undefined: "+
			      interpreter->CallStackBack()->GetString( *res),true,false);
	  ex.SetArrayexprIndexeeFailed( true);
	  throw ex;
      }
      if( _t->getType() == GDLTokenTypes::VAR)
      {
	  GDLException ex( _t, "Variable is undefined: "+
			interpreter->CallStackBack()->GetString(_t->varIx),true,false);
	  ex.SetArrayexprIndexeeFailed( true);
	  throw ex;
      }
      GDLException ex( _t, "Heap variable is undefined: "+interpreter->Name(res),true,false);
      ex.SetArrayexprIndexeeFailed( true);
      throw ex;
  }

  if( (*res)->IsAssoc())
  {
    aL=interpreter->arrayindex_list( this->getFirstChild()->getNextSibling(), false);
  }
  else
  {
    if( (*res)->Type() == GDL_OBJ && (*res)->StrictScalar())
    {
	// check for _overloadBracketsLeftSide
	DObj s = (*static_cast<DObjGDL*>(*res))[0]; // is StrictScalar()
// 	      if( s != 0)  // no overloads for null object
// 	      {
// 		DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( s);
// 		if( oStructGDL != NULL) // if object not valid -> default behaviour
// 		  {
// 		    DStructDesc* desc = oStructGDL->Desc();
// 		    DPro* bracketsLeftSideOverload = static_cast<DPro*>(desc->GetOperator( OOBracketsLeftSide));
	DSubUD* bracketsLeftSideOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( s, OOBracketsLeftSide));
	if( bracketsLeftSideOverload != NULL)
	{
	  bool internalDSubUD = bracketsLeftSideOverload->GetTree()->IsWrappedNode();  

	    // _overloadBracketsLeftSide
	  IxExprListT indexList;
	  interpreter->arrayindex_list_overload( this->getFirstChild()->getNextSibling(), indexList);
	  ArrayIndexListGuard guard(this->getFirstChild()->getNextSibling()->arrIxListNoAssoc);
	  
	  // hidden SELF is counted as well
	  int nParSub = bracketsLeftSideOverload->NPar();
	  assert( nParSub >= 1); // SELF
//  		      int indexListSizeDebug = indexList.size();
	  // indexList.size() + OBJREF + RVALUE > regular paramters w/o SELF
	  if( (indexList.size() + 2) > nParSub - 1)
	  {
	    indexList.Cleanup();
	    throw GDLException( this, bracketsLeftSideOverload->ObjectName() +
			    ": Incorrect number of arguments.",
			    false, false);
	  }

	  DObjGDL* self;
	  Guard<BaseGDL> selfGuard;
	  if( internalDSubUD)
	  {
	    self = static_cast<DObjGDL*>(*res); // internal subroutines behave well
	  }
	  else
	  {
	    self = static_cast<DObjGDL*>(*res)->Dup(); // res should be not changeable via SELF
	    selfGuard.Reset( self);
	  }
	  
	  // adds already SELF parameter
	  EnvUDT* newEnv= new EnvUDT( this, bracketsLeftSideOverload, &self);
// 		      Guard<EnvUDT> newEnvGuard( newEnv);
	  
	  // parameters
	  newEnv->SetNextParUnchecked( res); // OBJREF  parameter
	  // Dup() here is not optimal
	  // avoid at least for internal overload routines (which do/must not change RVALUE)
	  if( internalDSubUD)  
	    newEnv->SetNextParUnchecked( &right); // RVALUE  parameter, as reference to prevent cleanup in newEnv
	  else
	    newEnv->SetNextParUnchecked( right->Dup()); // RVALUE parameter, as value
	  // pass as reference would be more efficient, but as the data might
	  // be deleted in bracketsLeftSideOverload it is not possible.
	  // BaseGDL* rightCopy = right;  
	  // newEnv->SetNextParUnchecked( &rightCopy); // RVALUE  parameter
	  for( SizeT p=0; p<indexList.size(); ++p)
	    newEnv->SetNextParUnchecked( indexList[p]); // takes ownership

	  StackGuard<EnvStackT> stackGuard(interpreter->CallStack());
	  interpreter->CallStack().push_back( newEnv); 
	  
	  // make the call
	  interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

	  if( !internalDSubUD && self != selfGuard.Get())
	  {
	    // always put out warning first, in case of a later crash
	    Warning( "WARNING: " + bracketsLeftSideOverload->ObjectName() + 
		  ": Assignment to SELF detected (GDL session still ok).");
	    // assignment to SELF -> self was deleted and points to new variable
	    // which it owns
	    selfGuard.Release();
	    if( static_cast<BaseGDL*>(self) != NullGDL::GetSingleInstance())
	      selfGuard.Reset(self);
	  }
	  
	  return res;
	}
    }
      
// aL=interpreter->arrayindex_list_noassoc( this->getFirstChild()->getNextSibling());  

//       IxExprListT      cleanupList; // for cleanup
    ProgNodeP ax = this->getFirstChild()->getNextSibling();
    aL=interpreter->arrayindex_list( ax, true);
    
  }
  guard.reset(aL);

  try {
    aL->AssignAt( *res, right);
  }
  catch( GDLException& ex)
  {
    ex.SetErrorNodeP( this);
    throw ex;
  }
  //_retTree = _t->getNextSibling();
  return res;
}
// default ...Grab version
	
BaseGDL** SYSVARNode::LExpr( BaseGDL* right)
		//case SYSVAR:
	{
	if( right == NULL)
	throw GDLException( this, "System variable not allowed in this context.",
		true,false);

	BaseGDL** res=this->LEval(); //l_sys_var(this);
	Guard<BaseGDL> conv_guard; //( rConv);
	BaseGDL* rConv = right;
	if( !(*res)->EqType( right))
	{
		rConv = right->Convert2( (*res)->Type(), BaseGDL::COPY);
		conv_guard.Reset( rConv);
	}
	if( right->N_Elements() != 1 && ((*res)->N_Elements() != right->N_Elements()))
	{
		throw GDLException( this, "Conflicting data structures: <"+
			right->TypeStr()+" "+right->Dim().ToString()+">, !"+
			this->getText(),true,false);
	}
	(*res)->AssignAt( rConv); // linear copy
	return res;
	}
// default ...Grab version

// BaseGDL** XXXNode::LExpr( BaseGDL* right)
// // 	case FCALL:
// // 	case FCALL_LIB:
// // 	case MFCALL:
// // 	case MFCALL_PARENT:
// // 	case DEREF:
// // 	case VAR:
// // 	case VARPTR:
// 	{
// 	BaseGDL** res=this->LEval(); //l_simple_var(_t);
// 	//_retTree = _t->getNextSibling();
// 	if( right != NULL && right != (*res))
// 	{
// 		delete *res;
// 		*res = right->Dup();
// 	}
// 	return res;
// 	}

// #define LEXPRGRAB \
// 	{	BaseGDL** res=this->LEval(); \
// 	if( right != NULL && right != (*res)) \
// 	{	delete *res; *res = right;} \
// 	return res;}
// 
// BaseGDL** FCALLNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// BaseGDL** MFCALLNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// BaseGDL** MFCALL_PARENTNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// BaseGDL** FCALL_LIBNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// BaseGDL** DEREFNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// BaseGDL** VARNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// BaseGDL** VARPTRNode::LExprGrab( BaseGDL* right)
// LEXPRGRAB
// #undef LEXPRGRAB

#define LEXPR \
	{	BaseGDL** res=this->LEval(); \
	if( right != NULL && right != (*res)) \
	{	GDLDelete(*res); *res = right->Dup();} \
	return res;}
	
BaseGDL** FCALLNode::LExpr( BaseGDL* right)
LEXPR
BaseGDL** MFCALLNode::LExpr( BaseGDL* right)
LEXPR
BaseGDL** MFCALL_PARENTNode::LExpr( BaseGDL* right)
LEXPR
BaseGDL** FCALL_LIBNode::LExpr( BaseGDL* right)
LEXPR
BaseGDL** DEREFNode::LExpr( BaseGDL* right)
LEXPR
BaseGDL** VARNode::LExpr( BaseGDL* right)
LEXPR
BaseGDL** VARPTRNode::LExpr( BaseGDL* right)
LEXPR
#undef LEXPR

BaseGDL** ARRAYEXPR_FCALLNode::LExpr( BaseGDL* right)
{
  if( fcallNodeFunIx >= 0)
      return fcallNode->FCALLNode::LExpr( right);
  else if( fcallNodeFunIx == -2)
  {
    return arrayExprNode->ARRAYEXPRNode::LExpr( right);
  }
    
  assert( fcallNodeFunIx == -1);
  try{
    BaseGDL** res = arrayExprNode->ARRAYEXPRNode::LExpr( right);
    fcallNodeFunIx = -2; // mark as ARRAYEXPR succeeded
    return res;
  }
  catch( GDLException& ex)
  {
    if( !ex.GetArrayexprIndexeeFailed())
    {
      fcallNodeFunIx = -2; // mark as ARRAYEXPR succeeded
      throw ex;
    }
    try{
	BaseGDL** res = fcallNode->FCALLNode::LExpr( right);
	fcallNodeFunIx = fcallNode->funIx;
	return res;
    }    // keep FCALL if already compiled (but runtime error)
    catch( GDLException& innerEx)
    {
      if(fcallNode->funIx >= 0)
      {
	fcallNodeFunIx = fcallNode->funIx;
	throw innerEx;
      }
	std::string msg = "Ambiguous: " + ex.ANTLRException::toString() +
	" or: " + innerEx.ANTLRException::toString();
	throw GDLException(this,msg,true,false);
    }
  }
}
  

BaseGDL** ARRAYEXPR_MFCALLNode::LExpr( BaseGDL* right)
{
  return interpreter->l_arrayexpr_mfcall(this, right);
}
// default ...Grab version

BaseGDL** DOTNode::LExpr( BaseGDL* right)
{
  if( right == NULL)
      throw GDLException( this, "Struct expression not allowed in this context.",
			  true,false);

  ProgNodeP _t = this->getFirstChild();

  //SizeT nDot = tIn->nDot;
  Guard<DotAccessDescT> aD( new DotAccessDescT(nDot+1));

  //interpreter->l_dot_array_expr(_t, aD.get());

  ArrayIndexListT* aL;
  BaseGDL**        rP;
  if( _t->getType() == GDLTokenTypes::ARRAYEXPR)
  {
// 	  rP=l_indexable_expr(_t->getFirstChild());
      rP = _t->getFirstChild()->LEval(); // throws
      if( *rP == NULL)
      {   // ERROR
	  BaseGDL** res = rP;
	  _t = _t->getFirstChild();
	  // check not needed for SYSVAR
	  assert( _t->getType() != GDLTokenTypes::SYSVAR);
	  if( _t->getType() == GDLTokenTypes::VARPTR)
	  {
	      GDLException ex( _t, "Common block variable is undefined: "+
				interpreter->CallStackBack()->GetString( *res),true,false);
	      ex.SetArrayexprIndexeeFailed( true);
	      throw ex;
	  }
	  if( _t->getType() == GDLTokenTypes::VAR)
	  {
	      GDLException ex( _t, "Variable is undefined: "+
				interpreter->CallStackBack()->GetString(_t->GetVarIx()),true,false);
	      ex.SetArrayexprIndexeeFailed( true);
	      throw ex;
	  }
	  GDLException ex( _t, "Variable is undefined: "+interpreter->Name(res),true,false);
	  ex.SetArrayexprIndexeeFailed( true);
	  throw ex;
      }

      // aL=arrayindex_list(_t->getFirstChild()->getNextSibling());
      bool handled = false;
      if( !(*rP)->IsAssoc() && (*rP)->Type() == GDL_OBJ && (*rP)->StrictScalar())
      {

	  // check for _overloadBracketsLeftSide
	  DObj s = (*static_cast<DObjGDL*>(*rP))[0]; // is StrictScalar()
	  DSubUD* bracketsLeftSideOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( s, OOBracketsLeftSide));
	  if( bracketsLeftSideOverload != NULL)
	  {
	      bool internalDSubUD = bracketsLeftSideOverload->GetTree()->IsWrappedNode();

	      // _overloadBracketsLeftSide
	      IxExprListT indexList;
	      interpreter->arrayindex_list_overload( _t->getFirstChild()->getNextSibling(), indexList);
	      ArrayIndexListGuard guard(_t->getFirstChild()->getNextSibling()->arrIxListNoAssoc);

	      // hidden SELF is counted as well
	      int nParSub = bracketsLeftSideOverload->NPar();
	      assert( nParSub >= 1); // SELF

	      // indexList.size() + OBJREF + RVALUE > regular paramters w/o SELF
	      if( (indexList.size() + 2) > nParSub - 1)
	      {
		  indexList.Cleanup();
		  throw GDLException( this, bracketsLeftSideOverload->ObjectName() +
				      ": Incorrect number of arguments.",
				      false, false);
	      }

	      DObjGDL* self;
	      Guard<BaseGDL> selfGuard;
	      if( internalDSubUD)
	      {
		  self = static_cast<DObjGDL*>(*rP); // internal subroutines behave well
	      }
	      else
	      {
		  self = static_cast<DObjGDL*>(*rP)->Dup(); // res should be not changeable via SELF
		  selfGuard.Reset( self);
	      }

	      // adds already SELF parameter
	      EnvUDT* newEnv= new EnvUDT( this, bracketsLeftSideOverload, &self);
	      // Guard<EnvUDT> newEnvGuard( newEnv);

	      // parameters
	      // special: we are in dot access here
	      // signal to _overloadBracketsLeftSide by setting OBJREF to NULL
	      BaseGDL* returnOBJREF = NULL;
	      newEnv->SetNextParUnchecked( &returnOBJREF); // OBJREF  parameter
	      // Dup() here is not optimal
	      // avoid at least for internal overload routines (which do/must not change RVALUE)

	      BaseGDL* rValueNull = NULL;
	      newEnv->SetNextParUnchecked( rValueNull); // RVALUE parameter NULL, as value

	      for( SizeT p=0; p<indexList.size(); ++p)
		  newEnv->SetNextParUnchecked( indexList[p]); // takes ownership

	      StackGuard<EnvStackT> stackGuard(interpreter->CallStack());
	      interpreter->CallStack().push_back( newEnv);

	      // make the call
	      interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

	      if( !internalDSubUD && self != selfGuard.Get())
	      {
		  // always put out warning first, in case of a later crash
		  Warning( "WARNING: " + bracketsLeftSideOverload->ObjectName() +
			    ": Assignment to SELF detected (GDL session still ok).");
		  // assignment to SELF -> self was deleted and points to new variable
		  // which it owns
		  selfGuard.Release();
		  if( static_cast<BaseGDL*>(self) != NullGDL::GetSingleInstance())
		      selfGuard.Reset(self);
	      }

	      if( returnOBJREF == NULL || returnOBJREF->Type() != GDL_PTR)
		  GDLException ex( _t, "OBJREF must return a PTR to the STRUCT to access.",true,false);

	      DPtr vID = (*static_cast<DPtrGDL*>(returnOBJREF))[0];
	      delete returnOBJREF;

	      BaseGDL* structToAccess = interpreter->GetHeap( vID);

	      interpreter->SetRootL( _t, aD.get(), structToAccess, NULL);
	      handled = true;
	  }
      } //  	  if( (*rP)->Type() == GDL_OBJ && (*rP)->StrictScalar())
      if( !handled)
      {
	  // regular (non-object) case
	  aL=interpreter->arrayindex_list( _t->getFirstChild()->getNextSibling(),!(*rP)->IsAssoc());
	  interpreter->SetRootL( _t, aD.get(), *rP, aL);
      }
  }
  else
      // case ARRAYEXPR_MFCALL:
      // case DEREF:
      // case EXPR:
      // case FCALL:
      // case FCALL_LIB:
      // case MFCALL:
      // case MFCALL_PARENT:
      // case SYSVAR:
      // case VAR:
      // case VARPTR:
  {
      // rP=l_indexable_expr(_t);
      rP = _t->LEval(); // throws
      if( *rP == NULL)
      {   // ERROR
	  BaseGDL** res = rP;
	  // check not needed for SYSVAR
	  assert( _t->getType() != GDLTokenTypes::SYSVAR);
	  if( _t->getType() == GDLTokenTypes::VARPTR)
	  {
	      GDLException ex( _t, "Common block variable is undefined: "+
				interpreter->CallStackBack()->GetString( *res),true,false);
	      ex.SetArrayexprIndexeeFailed( true);
	      throw ex;
	  }
	  if( _t->getType() == GDLTokenTypes::VAR)
	  {
	      GDLException ex( _t, "Variable is undefined: "+
				interpreter->CallStackBack()->GetString(_t->GetVarIx()),true,false);
	      ex.SetArrayexprIndexeeFailed( true);
	      throw ex;
	  }
	  GDLException ex( _t, "Variable is undefined: "+interpreter->Name(res),true,false);
	  ex.SetArrayexprIndexeeFailed( true);
	  throw ex;
      }
      interpreter->SetRootL( _t, aD.get(), *rP, NULL);
  }

  _t = _t->getNextSibling();
  for( int d=0; d<nDot; ++d)
  {
      // if ((_t->getType() == ARRAYEXPR || _t->getType() == EXPR ||
      // _t->getType() == IDENTIFIER)) {
      interpreter->tag_array_expr(_t, aD.get());
      _t = interpreter->GetRetTree();
      //       }
      //       else {
      // 	break;
      //       }
  }
  aD->ADAssign( right);
  //res=NULL;
  //SetRetTree( tIn->getNextSibling());
  return NULL;
}
// default ...Grab version

BaseGDL** ASSIGNNode::LExpr( BaseGDL* right)	
//case ASSIGN:
{
  ProgNodeP _t = this->getFirstChild();
  if( NonCopyNode(_t->getType()))
  {
//     BaseGDL*       e1=interpreter->indexable_expr(_t);
//     _t = interpreter->GetRetTree();
    BaseGDL* e1 = _t->EvalNC();
    _t = _t->getNextSibling();    	  
  }
//   else if( _t->getType() == GDLTokenTypes::FCALL_LIB)
//   {
// // 	  BaseGDL*       e1=interpreter->lib_function_call(_t);
// // 	  _t = interpreter->GetRetTree();
// 	  BaseGDL** retValPtr;
// 	  BaseGDL* e1 = static_cast<FCALL_LIBNode*>(_t)->EvalFCALL_LIB( retValPtr); 
// 	  _t = _t->getNextSibling();
// // 	  if( !interpreter->CallStackBack()->Contains( e1))
// 	  if( retValPtr == NULL)
// 	    GDLDelete(e1); // guard if no global data
//   }
  else
  {
    //       case ASSIGN:
    //       case ASSIGN_REPLACE:
    //       case ASSIGN_ARRAYEXPR_MFCALL:
    //       case ARRAYDEF:
    //       case ARRAYEXPR:
    //       case ARRAYEXPR_MFCALL:
    //       case EXPR:
    //       case FCALL:
    //       case FCALL_LIB_RETNEW:
    //       case MFCALL:
    //       case MFCALL_PARENT:
    //       case NSTRUC:
    //       case NSTRUC_REF:
    //       case POSTDEC:
    //       case POSTINC:
    //       case STRUC:
    //       case DEC:
    //       case INC:
    //       case DOT:
    //       case QUESTION:
//     BaseGDL*       e1=interpreter->indexable_tmp_expr(_t);
//     _t = interpreter->GetRetTree();
//     BaseGDL* e1 = _t->Eval(); 
//     //lib_function_call_retnew(_t);
//     GDLDelete(e1);
    BaseGDL* e1;
    BaseGDL** ref =_t->EvalRefCheck(e1);
    if( ref == NULL)
      GDLDelete(e1);
    _t = _t->getNextSibling();
  }
  //SetRetTree( tIn->getNextSibling());
  return _t->LExpr( right); //l_expr(_t, right);
}

// something like: (( ((aFUNorVAR(aVAR))) =e1 ))=right
BaseGDL** ASSIGN_ARRAYEXPR_MFCALLNode::LExpr( BaseGDL* right)
//case ASSIGN_ARRAYEXPR_MFCALL:
{
  ProgNodeP _t = this->getFirstChild();

  if( NonCopyNode(_t->getType()))
  {
    BaseGDL* e1 = _t->EvalNC();
    _t = _t->getNextSibling();    
  }
  else
  {
    // e1 must be calculated due to possible side effects, but the result isn't used or even accessible
    BaseGDL* e1;
    BaseGDL** ref =_t->EvalRefCheck(e1);
    if( ref == NULL)
      GDLDelete(e1);
//     BaseGDL* e1 = _t->Eval(); 
//     //lib_function_call_retnew(_t);
//     GDLDelete(e1);
    _t = _t->getNextSibling();
  }
  ProgNodeP l = _t;
  BaseGDL** res;
  // try MFCALL
  try
  {
    res=interpreter->l_arrayexpr_mfcall_as_mfcall( l);
    if( right != (*res))
    {
      GDLDelete(*res);
      *res = right->Dup();
    }
  }
  catch( GDLException& ex)
  {
    // try ARRAYEXPR
    try
    {
      res=interpreter->l_arrayexpr_mfcall_as_arrayexpr(l, right);
    }
    catch( GDLException& ex2)
    {
      throw GDLException(ex.toString() + " or "+ex2.toString());
    }
  }
  //SetRetTree( tIn->getNextSibling());
  return res;
}


BaseGDL** ASSIGN_REPLACENode::LExpr( BaseGDL* right)
//	case ASSIGN_REPLACE:
{
  ProgNodeP _t = this->getFirstChild();

  BaseGDL** res;
//   if( _t->getType() == GDLTokenTypes::FCALL_LIB)
//   {
// // 	  BaseGDL* e1=interpreter->lib_function_call(_t);
// // 	  _t = interpreter->GetRetTree();
//       BaseGDL** retValPtr;
//       BaseGDL* e1 = static_cast<FCALL_LIBNode*>(_t)->EvalFCALL_LIB( retValPtr); 
//       _t = _t->getNextSibling();
//       res =_t->LEval(); //l_function_call(_t);
// //       if( *res != e1 && !interpreter->CallStackBack()->Contains( e1))
//       if( *res != e1 && retValPtr == NULL)
// 	GDLDelete(e1);
//   }
//   else
  {
  //     case ASSIGN:
  //     case ASSIGN_REPLACE:
  //     case ASSIGN_ARRAYEXPR_MFCALL:
  //     case ARRAYDEF:
  //     case ARRAYEXPR:
  //     case ARRAYEXPR_MFCALL:
  //     case CONSTANT:
  //     case DEREF:
  //     case EXPR:
  //     case FCALL:
  //     case FCALL_LIB_RETNEW:
  //     case MFCALL:
  //     case MFCALL_PARENT:
  //     case NSTRUC:
  //     case NSTRUC_REF:
  //     case POSTDEC:
  //     case POSTINC:
  //     case STRUC:
  //     case SYSVAR:
  //     case VAR:
  //     case VARPTR:
  //     case DEC:
  //     case INC:
  //     case DOT:
  //     case QUESTION:

//   BaseGDL* e1=interpreter->tmp_expr(_t);
    BaseGDL* e1;
    BaseGDL** ref =_t->EvalRefCheck(e1);
    if( ref != NULL)
	e1 = *ref;

    _t =_t->getNextSibling();

    res =_t->LEval(); //l_function_call(_t);
    if( *res != e1 && ref == NULL) 
      GDLDelete(e1);

//     BaseGDL* e1 = _t->Eval();
//     _t =_t->getNextSibling();
//     res =_t->LEval(); //l_function_call(_t);
//     if( *res != e1)
//       GDLDelete(e1);
  }

  // switch ( _t->getType()) {
  // case DEREF:
  //     // 	  {
  //     // 		  res=_t->LEval(); //l_deref(_t);
  //     // 		  _t = _retTree;
  //     // 		  break;
  //     // 	  }
  // case VAR:
  // case VARPTR:
  //     // {
  //     //     res=_t->LEval(); //l_simple_var(_t);
  //     //     _retTree = tIn->getNextSibling();
  //     //     //_t = _retTree;
  //     //     break;
  //     // }
  // default:
  //     // 	  case FCALL:
  //     // 	  case FCALL_LIB:
  //     // 	  case MFCALL:
  //     // 	  case MFCALL_PARENT:
  //     {
//   BaseGDL** res=_t->LEval(); //l_function_call(_t);
  //_retTree = tIn->getNextSibling();
  //_t = _retTree;
  //         break;
  //     }
  // }
  if( right != (*res))
  {
    GDLDelete(*res);
    assert( right != NULL);
    *res = right->Dup();
  }
// 	SetRetTree( this->getNextSibling());
  return res;
}



// l_expr finish /////////////////////////////////////////////


