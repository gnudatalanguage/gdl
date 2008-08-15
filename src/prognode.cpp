/***************************************************************************
			prognode.cpp  -  GDL's AST is made of ProgNodes
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

#include "prognodeexpr.hpp"

#include "dinterpreter.hpp"

using namespace std;

BaseGDL* ARRAYDEFNode::Eval()
{
  // GDLInterpreter::
  DType  cType=UNDEF; // conversion type
  SizeT maxRank=0;
  ExprListT            exprList;
  BaseGDL*           cTypeData;
		
  //ProgNodeP __t174 = this;
  //ProgNodeP a =  this;
  //match(antlr::RefAST(_t),ARRAYDEF);
  ProgNodeP _t =  this->getFirstChild();
  while(  _t != NULL) {

    BaseGDL* e=_t->Eval();//expr(_t);

    _t = ProgNode::interpreter->_retTree;
			
    // add first (this way it will get cleaned up anyway)
    exprList.push_back(e);
			
    DType ty=e->Type();
    if( ty == UNDEF)
      {
	throw GDLException( _t, "Variable is undefined: "+
			    ProgNode::interpreter->Name(e));
      }
    if( cType == UNDEF) 
      {
	cType=ty;
	cTypeData=e;
      }
    else 
      { 
	if( cType != ty) 
	  {
	    if( DTypeOrder[ty] > 100 || DTypeOrder[cType] > 100) // struct, ptr, object
	      {
		throw 
		  GDLException( _t, e->TypeStr()+
				" is not allowed in this context.");
	      }
			
	    // update order if larger type (or types are equal)
	    if( DTypeOrder[ty] >= DTypeOrder[cType]) 
	      {
		cType=ty;
		cTypeData=e;
	      }
	  }
	if( ty == STRUCT)
	  {
	    // check for struct compatibility
	    DStructDesc* newS=
	      static_cast<DStructGDL*>(e)->Desc();
	    DStructDesc* oldS=
	      static_cast<DStructGDL*>(cTypeData)->Desc();
			
	    // *** here (*newS) != (*oldS) must be set when
	    // unnamed structs not in struct list anymore
	    // WRONG! This speeds up things for named structs
	    // unnamed structs all have their own desc
	    // and thus the next is always true for them
	    if( newS != oldS)
	      {
			
		if( (*newS) != (*oldS))
		  throw GDLException( _t, 
				      "Conflicting data structures: "+
				      ProgNode::interpreter->Name(cTypeData)+", "+ProgNode::interpreter->Name(e));
	      }
	  }
      }
			
    // memorize maximum Rank
    SizeT rank=e->Rank();
    if( rank > maxRank) maxRank=rank;
  }
  _t = this->getNextSibling();
	
  BaseGDL* res=cTypeData->CatArray(exprList,this->arrayDepth,maxRank);
	
  ProgNode::interpreter->_retTree = _t;
  return res;
}

BaseGDL* STRUCNode::Eval()
{
  // don't forget the struct in extrat.cpp if you change something here
  // "$" as first char in the name is necessary 
  // as it defines unnnamed structs (see dstructdesc.hpp)
  DStructDesc*   nStructDesc = new DStructDesc( "$truct");
	
  // instance takes care of nStructDesc since it is unnamed
  //     DStructGDL* instance = new DStructGDL( nStructDesc, dimension(1));
  DStructGDL* instance = new DStructGDL( nStructDesc);
  auto_ptr<DStructGDL> instance_guard(instance);
	
  ProgNodeP rTree = this->getNextSibling();
  // 	match(antlr::RefAST(_t),STRUC);
  ProgNodeP _t = this->getFirstChild();
  for (; _t != NULL;) {
    ProgNodeP si = _t;
    // 			match(antlr::RefAST(_t),IDENTIFIER);
    _t = _t->getNextSibling();
    BaseGDL* e=_t->Eval();//expr(_t);
    _t = ProgNode::interpreter->_retTree;
			
    // also adds to descriptor, grabs
    instance->NewTag( si->getText(), e); 
  }

  instance_guard.release();
  BaseGDL* res=instance;
	
  ProgNode::interpreter->_retTree = rTree;
  return res;
}

BaseGDL* NSTRUCNode::Eval()
{
  ProgNodeP id = NULL;
  ProgNodeP i = NULL;
  ProgNodeP ii = NULL;
	
  DStructDesc*          nStructDesc;
  auto_ptr<DStructDesc> nStructDescGuard;
  BaseGDL* e;
  BaseGDL* ee;
	
	
  ProgNodeP n =  this;
  // 	match(antlr::RefAST(_t),NSTRUC);
  ProgNodeP _t = this->getFirstChild();
  id = _t;
  // 	match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
	
  // definedStruct: no tags present
  if( n->structDefined == 1) ProgNode::interpreter->GetStruct( id->getText(), _t);
	
  // find struct 'id' (for compatibility check)
  DStructDesc* oStructDesc=
    FindInStructList( structList, id->getText());
	
  if( oStructDesc == NULL || oStructDesc->NTags() > 0)
    {
      // not defined at all yet (-> define now)
      // or completely defined  (-> define now and check equality)
      nStructDesc= new DStructDesc( id->getText());
	
      // guard it
      nStructDescGuard.reset( nStructDesc); 
    } 
  else
    {   // NTags() == 0
	// not completely defined (only name in list)
      nStructDesc= oStructDesc;
    }
	
  // the instance variable
  //                 DStructGDL* instance= new DStructGDL( nStructDesc,
  //                                                       dimension(1)); 
  DStructGDL* instance= new DStructGDL( nStructDesc);
	
  auto_ptr<DStructGDL> instance_guard(instance);
	
  while( _t != NULL) 
    {
      switch ( _t->getType()) 
	{
	case DInterpreter::IDENTIFIER:
	  {
	    i = _t;
	    // 			match(antlr::RefAST(_t),IDENTIFIER);
	    _t = _t->getNextSibling();
	    e=_t->Eval();//expr(_t);
	    _t = ProgNode::interpreter->_retTree;
			
	    // also adds to descriptor, grabs
	    instance->NewTag( i->getText(), e); 
			
	    break;
	  }
	case DInterpreter::INHERITS:
	  {
	    //ProgNodeP tmp61_AST_in = _t;
	    // 			match(antlr::RefAST(_t),INHERITS);
	    _t = _t->getNextSibling();
	    ii = _t;
	    // 			match(antlr::RefAST(_t),IDENTIFIER);
	    _t = _t->getNextSibling();
			
	    DStructDesc* inherit=ProgNode::interpreter->GetStruct( ii->getText(), _t);
			
	    //   nStructDesc->AddParent( inherit);
	    instance->AddParent( inherit);
			
	    break;
	  }
	default:
	  /*		case ASSIGN:
			case ASSIGN_REPLACE:
			case ARRAYDEF:
			case ARRAYEXPR:
			case CONSTANT:
			case DEREF:
			case EXPR:
			case FCALL:
			case FCALL_LIB:
			case FCALL_LIB_RETNEW:
			case MFCALL:
			case MFCALL_PARENT:
			case NSTRUC:
			case NSTRUC_REF:
			case POSTDEC:
			case POSTINC:
			case STRUC:
			case SYSVAR:
			case VAR:
			case VARPTR:
			case DEC:
			case INC:
			case DOT:
			case QUESTION:*/
	  {
	    ee=_t->Eval();//expr(_t);
	    _t = ProgNode::interpreter->_retTree;
			
	    // also adds to descriptor, grabs
	    instance->NewTag( 
			     oStructDesc->TagName( nStructDesc->NTags()),
			     ee);
			
	    break;
	  }
	} // switch
    } // while
	
  // inherit refers to nStructDesc, in case of error both have to
  // be freed here
  if( oStructDesc != NULL)
    {
      if( oStructDesc != nStructDesc)
	{
	  oStructDesc->AssureIdentical(nStructDesc);
	  instance->DStructGDL::SetDesc(oStructDesc);
	  //delete nStructDesc; // auto_ptr
	}
    }
  else
    {
      // release from guard (if not NULL)
      nStructDescGuard.release();
	
      // insert into struct list 
      structList.push_back(nStructDesc);
    }
	
  instance_guard.release();
  BaseGDL* res=instance;
	
  ProgNode::interpreter->_retTree = this->getNextSibling();
  return res;
}
BaseGDL* NSTRUC_REFNode::Eval()
{
  ProgNodeP rTree = this->getNextSibling();
  // 	match(antlr::RefAST(_t),NSTRUC_REF);
  ProgNodeP _t = this->getFirstChild();
  ProgNodeP idRef = _t;
  // 	match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
		
  // find struct 'id'
  // returns it or throws an exception
  DStructDesc* dStruct=ProgNode::interpreter->GetStruct( idRef->getText(), _t);
		
  BaseGDL* res=new DStructGDL( dStruct, dimension(1));
		
  ProgNode::interpreter->_retTree = rTree;
  return res;
}

void ASSIGNNode::Run()
{
  BaseGDL*  r;
  BaseGDL** l;
  auto_ptr<BaseGDL> r_guard;
	
  //     match(antlr::RefAST(_t),ASSIGN);
  ProgNodeP _t = this->getFirstChild();
  {
    switch ( _t->getType()) {
    case GDLTokenTypes::CONSTANT:
    case GDLTokenTypes::DEREF:
    case GDLTokenTypes::SYSVAR:
    case GDLTokenTypes:: VAR:
    case GDLTokenTypes::VARPTR:
      {
	r= ProgNode::interpreter->indexable_expr(_t);
	_t = ProgNode::interpreter->_retTree;
	break;
      }
    case GDLTokenTypes::FCALL_LIB:
      {
	r=ProgNode::interpreter->check_expr(_t);
	_t = ProgNode::interpreter->_retTree;
			
	if( !ProgNode::interpreter->callStack.back()->Contains( r)) 
	  r_guard.reset( r); // guard if no global data
			
	break;
      }
    default:
      {
	r=ProgNode::interpreter->indexable_tmp_expr(_t);
	_t = ProgNode::interpreter->_retTree;
	r_guard.reset( r);
	break;
      }
    }//switch
  }
  l=ProgNode::interpreter->l_expr(_t, r);

  ProgNode::interpreter->_retTree = this->getNextSibling();
}

void ASSIGN_REPLACENode::Run()
{
  BaseGDL*  r;
  BaseGDL** l;
  auto_ptr<BaseGDL> r_guard;

  //match(antlr::RefAST(_t),ASSIGN_REPLACE);
  ProgNodeP _t = this->getFirstChild();
  {
    if( _t->getType() ==  GDLTokenTypes::FCALL_LIB)
      {
	r=ProgNode::interpreter->check_expr(_t);
	_t = ProgNode::interpreter->_retTree;
			
			
	if( !ProgNode::interpreter->callStack.back()->Contains( r)) 
	  r_guard.reset( r);
			
      }
    else
      {
	r=ProgNode::interpreter->tmp_expr(_t);
	_t = ProgNode::interpreter->_retTree;
			
	r_guard.reset( r);
			
      }
  }

  switch ( _t->getType()) {
  case GDLTokenTypes::VAR:
  case GDLTokenTypes::VARPTR:
    {
      l=ProgNode::interpreter->l_simple_var(_t);
//       _t = ProgNode::interpreter->_retTree;
      break;
    }
  case GDLTokenTypes::DEREF:
    {
      l=ProgNode::interpreter->l_deref(_t);
//       _t = ProgNode::interpreter->_retTree;
      break;
    }
  default:
//   case GDLTokenTypes::FCALL:
//   case GDLTokenTypes::FCALL_LIB:
//   case GDLTokenTypes::MFCALL:
//   case GDLTokenTypes::MFCALL_PARENT:
    {
      l=ProgNode::interpreter->l_function_call(_t);
//       _t = ProgNode::interpreter->_retTree;
      break;
    }
  }
		
  if( r != (*l))
    {
      delete *l;
		
      if( r_guard.get() == r)
	*l = r_guard.release();
      else  
	*l = r->Dup();
    }

  ProgNode::interpreter->_retTree = this->getNextSibling();
}

void PCALL_LIBNode::Run()
{
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard( ProgNode::interpreter->CallStack());
  BaseGDL *self;
	
  // 		match(antlr::RefAST(_t),PCALL_LIB);
  ProgNodeP _t = this->getFirstChild();
  ProgNodeP pl = _t;
  // 		match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
		
  EnvT* newEnv=new EnvT( pl, pl->libPro);//libProList[pl->proIx]);
		
  ProgNode::interpreter->parameter_def(_t, newEnv);
  //   _t = _retTree;
		
  // push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  static_cast<DLibPro*>(newEnv->GetPro())->Pro()(newEnv);

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  //  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void MPCALLNode::Run()
{
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(ProgNode::interpreter->CallStack());
  BaseGDL *self;
  EnvUDT*   newEnv;
	
  // 			match(antlr::RefAST(_t),MPCALL);
  ProgNodeP _t = this->getFirstChild();
  self=ProgNode::interpreter->expr(_t);
  _t = ProgNode::interpreter->_retTree;
  ProgNodeP mp = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
			
  auto_ptr<BaseGDL> self_guard(self);
			
  newEnv=new EnvUDT( mp, self);
			
  self_guard.release();
			
  ProgNode::interpreter->parameter_def(_t, newEnv);

  // push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  ProgNode::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
}

void MPCALL_PARENTNode::Run()
{
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(ProgNode::interpreter->callStack);
  BaseGDL *self;
  EnvUDT*   newEnv;
	

  // 			match(antlr::RefAST(_t),MPCALL_PARENT);
  ProgNodeP _t = this->getFirstChild();
  self=ProgNode::interpreter->expr(_t);
  _t = ProgNode::interpreter->_retTree;
  ProgNodeP parent = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
  ProgNodeP pp = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
			
  auto_ptr<BaseGDL> self_guard(self);
			
  newEnv = new EnvUDT( pp, self, parent->getText());
			
  self_guard.release();
			
  ProgNode::interpreter->parameter_def(_t, newEnv);


  // push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  ProgNode::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  //  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void PCALLNode::Run()
{
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(ProgNode::interpreter->callStack);
  EnvUDT*   newEnv;
	

  // 			match(antlr::RefAST(_t),PCALL);
  ProgNodeP _t = this->getFirstChild();
  ProgNodeP p = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
			
  ProgNode::interpreter->SetProIx( p);
			
  newEnv = new EnvUDT( p, proList[p->proIx]);
			
  ProgNode::interpreter->parameter_def(_t, newEnv);


  // push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  ProgNode::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  //  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void DECNode::Run()
{
  //		match(antlr::RefAST(_t),DEC);
  ProgNodeP _t = this->getFirstChild();
  ProgNode::interpreter->l_decinc_expr(_t, GDLTokenTypes::DECSTATEMENT);

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
}
void INCNode::Run()
{
  //		match(antlr::RefAST(_t),INC);
  ProgNodeP _t = this->getFirstChild();
  ProgNode::interpreter->l_decinc_expr(_t, GDLTokenTypes::INCSTATEMENT);

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
}


// void FOR_INIT_Node::Run()
// {
//   BaseGDL* s;
//   BaseGDL* e;
//   BaseGDL* st;
//   GDLInterpreter::RetCode retCode = RC_OK;
	
//   //match(antlr::RefAST(_t),FOR);
//   ProgNodeP _t = this->getFirstChild();
		
//   ProgNodeP sv = _t;
		
//   BaseGDL** v=l_simple_var(_t);
//   _t = _retTree;
//   s=expr(_t);
//   _t = _retTree;
//   e=expr(_t);
//   _t = _retTree;
		
//   auto_ptr<BaseGDL> s_guard(s);
//   auto_ptr<BaseGDL> e_guard(e);
		
//   s->ForCheck( &e);
//   e_guard.release();
//   e_guard.reset(e);
		
//   ProgNodeP b= _t;
		
//   // ASSIGNMENT used here also
//   delete (*v);
		
//   // problem:
//   // EXECUTE may call DataListT.loc.resize(), as v points to the
//   // old sequence v might be invalidated -> segfault
//   // note that the value (*v) is preserved by resize()
//   s_guard.release(); // s held in *v after this


//   (*v)=s;  
//   SetReturnCode( RC_OK);
// }

// void ENDFORNode::Run()
// {
//   v=l_simple_var( sv);
//   (*v)->ForAdd();
//   ProgNode::interpreter->SetRetTree( target);
// }
// void ENDFOR_STEPNode::Run()
// {
//   v=l_simple_var( sv); 
//   (*v)->ForAdd(st))
//   ProgNode::interpreter->SetRetTree( target);
// }

// void FORNode::Run()
// {
//   if( ProgNode::interpreter->returnCode != RC_OK)
//     {
//     if( retCode == RC_CONTINUE) continue;  
//     if( retCode == RC_BREAK) 
//       {
// 	retCode = RC_OK;
// 	break;        
//       }
//     if( retCode >= RC_RETURN) break;
//     }

//  if( (*v)->ForCondUp( e))
//     {
//       ProgNode::interpreter->SetRetTree( b);
//     }
//  else
//     {
//       ProgNode::interpreter->SetRetTree( this->getNextSibling());
//     }
// }

// void FOR_STEP_INITNode::Run()
// {
//   BaseGDL* s;
//   BaseGDL* e;
//   BaseGDL* st;
// //   match(antlr::RefAST(_t),FOR_STEP);
//   ProgNodeP _t = this->getFirstChild();
		
//   ProgNodeP sv = _t;
		
//   v=l_simple_var(_t);
//   _t = _retTree;
//   s=expr(_t);
//   _t = _retTree;
//   e=expr(_t);
//   _t = _retTree;
//   st=expr(_t);
//   _t = _retTree;
		
//   auto_ptr<BaseGDL> s_guard(s);
//   auto_ptr<BaseGDL> e_guard(e);
//   auto_ptr<BaseGDL> st_guard(st);
		
//   SizeT nJump = static_cast<EnvUDT*>(callStack.back())->NJump();
		
//   s->ForCheck( &e, &st);
//   e_guard.release();
//   e_guard.reset(e);
//   st_guard.release();
//   st_guard.reset(st);
		
//   ProgNodeP bs=_t;
		
//   // ASSIGNMENT used here also
//   delete (*v);
// }
// void FOR_STEPNode::Run()
// {
//   if( ProgNode::interpreter->returnCode != RC_OK)
//     {
//     if( retCode == RC_CONTINUE) continue;  
//     if( retCode == RC_BREAK) 
//       {
// 	retCode = RC_OK;
// 	break;        
//       }
//     if( retCode >= RC_RETURN) break;
//     }

//  if( st->Sgn() == -1) 
//    {
//  if( (*v)->ForCondDown( e))
//     {
//       ProgNode::interpreter->SetRetTree( bs);
//     }
//  else
//     {
//       ProgNode::interpreter->SetRetTree( this->getNextSibling());
//     }
//    }
//  else
//    {
//  if( (*v)->ForCondUp( e))
//     {
//       ProgNode::interpreter->SetRetTree( bs);
//     }
//  else
//     {
//       ProgNode::interpreter->SetRetTree( this->getNextSibling());
//     }
//    }
// }
