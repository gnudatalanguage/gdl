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

#include "objects.hpp"
#include "nullgdl.hpp"

using namespace std;

bool* GetNonCopyNodeLookupArray()
{
static bool nonCopyNodeLookupArray[ GDLTokenTypes::MAX_TOKEN_NUMBER];
for( int i=0; i<GDLTokenTypes::MAX_TOKEN_NUMBER; ++i)
	nonCopyNodeLookupArray[ i] = false;
nonCopyNodeLookupArray[ GDLTokenTypes::VAR] = true;
nonCopyNodeLookupArray[ GDLTokenTypes::VARPTR] = true;
nonCopyNodeLookupArray[ GDLTokenTypes::DEREF] = true;
nonCopyNodeLookupArray[ GDLTokenTypes::CONSTANT] = true;
nonCopyNodeLookupArray[ GDLTokenTypes::SYSVAR] = true;
return nonCopyNodeLookupArray;
}


/////////////////////////////////////////////////////////////////////////////
// c-tor d-tor
/////////////////////////////////////////////////////////////////////////////

// tanslation RefDNode -> ProgNode
ProgNode::ProgNode( const RefDNode& refNode):
	keepRight( false),
	keepDown( false),
	breakTarget( NULL),
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
  arrIxListNoAssoc( refNode->StealArrIxNoAssocList()),
//   arrIxList( refNode->CloneArrIxList()),
  labelStart( refNode->labelStart),
  labelEnd( refNode->labelEnd)
{
  initInt = refNode->initInt;
}



ProgNode::~ProgNode()
{
  // delete cData in case this node is a constant
  if( (getType() == GDLTokenTypes::CONSTANT))
     {
      GDLDelete(cData);
     }
  if( (getType() == GDLTokenTypes::ARRAYIX))
    {
      delete arrIxList;
      delete arrIxListNoAssoc;
    }
  if( !keepDown) delete down;
  if( !keepRight) delete right;
}



ProgNodeP ProgNode::GetNULLProgNodeP()
  {
	return interpreter->GetNULLProgNodeP();
  }


void FORNode::KeepRight( ProgNodeP r)
{
	throw GDLException( "Internal error: FORNode::KeepRight() called. Please report.");
}
void FOR_STEPNode::KeepRight( ProgNodeP r)
{
	throw GDLException( "Internal error: FOR_STEPNode::KeepRight() called. Please report.");
}
void FOREACHNode::KeepRight( ProgNodeP r)
{
	throw GDLException( "Internal error: FOREACHNode::KeepRight() called. Please report.");
}
void FOREACH_INDEXNode::KeepRight( ProgNodeP r)
{
	throw GDLException( "Internal error: FOREACH_INDEXNode::KeepRight() called. Please report.");
}

bool ProgNode::ConstantNode()
  {
    if( this->getType() == GDLTokenTypes::SYSVAR)
      {
	// note: this works, because system variables are never 
	//       passed by reference
       SizeT rdOnlySize = sysVarRdOnlyList.size();
         for( SizeT i=0; i<rdOnlySize; ++i)
                  if( sysVarRdOnlyList[ i] == this->var)
					return true;
      }

    return this->getType() == GDLTokenTypes::CONSTANT;
  }

/////////////////////////////////////////////////////////
// Eval 
/////////////////////////////////////////////////////////
BaseGDL* ASSIGNNode::Eval()
{
    ProgNodeP _t = this->getFirstChild();

    auto_ptr<BaseGDL> r_guard;
	BaseGDL* res;
    if( _t->getType() == GDLTokenTypes::FCALL_LIB)
        {
            res=interpreter->lib_function_call(_t);
            _t = interpreter->GetRetTree();
            if( !interpreter->CallStackBack()->Contains( res))
                r_guard.reset( res);
        }
    else
        {
            res=interpreter->tmp_expr(_t);
            _t = interpreter->GetRetTree();
            r_guard.reset( res);
        }

    BaseGDL** l=_t->LExpr( res); //l_expr(_t, res);
    //_t = _retTree;
    if( r_guard.get() == res) // owner
      r_guard.release();
    else
      res = res->Dup();
    return res;
}
BaseGDL* ASSIGN_ARRAYEXPR_MFCALLNode::Eval()
{
    ProgNodeP _t = this->getFirstChild();

    auto_ptr<BaseGDL> r_guard;

	BaseGDL* res;
    if( _t->getType() == GDLTokenTypes::FCALL_LIB)
        {
            res=interpreter->lib_function_call(_t);
            _t = interpreter->GetRetTree();
            if( !interpreter->CallStackBack()->Contains( res))
                r_guard.reset( res);
        }
    else
        {
            res=interpreter->tmp_expr(_t);
            _t = interpreter->GetRetTree();
            r_guard.reset( res);
        }

    ProgNodeP mark = _t;
    // try MFCALL
    try
	{
	  BaseGDL** l=interpreter->l_arrayexpr_mfcall_as_mfcall( mark);

	  if( res != (*l))
	    {
	      GDLDelete(*l);
	      *l = res->Dup();

	      if( r_guard.get() == res) // owner
		  {
		      r_guard.release();
		  }
	      else
			  res = res->Dup();
	    }
	}
    catch( GDLException& ex)
	{
	  // try ARRAYEXPR
	  try
	    {
		    BaseGDL** l=interpreter->l_arrayexpr_mfcall_as_arrayexpr(mark, res);
		    if( r_guard.get() == res) // owner
				r_guard.release();
		    else
			res = res->Dup();
	    }
	  catch( GDLException& ex2)
	    {
		throw GDLException(ex.toString() + " or "+ex2.toString());
	    }
	}
	return res;
}
BaseGDL* ASSIGN_REPLACENode::Eval()
{
    ProgNodeP _t = this->getFirstChild();

    auto_ptr<BaseGDL> r_guard;

	BaseGDL* res;
    if( _t->getType() == GDLTokenTypes::FCALL_LIB)
        {
            res=interpreter->lib_function_call(_t);
            _t = interpreter->GetRetTree();
            if( !interpreter->CallStackBack()->Contains( res))
                r_guard.reset( res);
        }
    else
        {
            res=interpreter->tmp_expr(_t);
            _t = interpreter->GetRetTree();
            r_guard.reset( res);
        }

    BaseGDL** l=_t->LEval();
    //_t = _t->getNextSibling();
    //_t = _retTree;

    if( res != (*l))
    {
      GDLDelete(*l);
      *l = res->Dup();

      if( r_guard.get() == res) // owner
      {
		r_guard.release();
      }
      else
	res = res->Dup();
	}
	return res;
}


BaseGDL* ARRAYDEFNode::Eval()
{
  // GDLInterpreter::
  DType  cType=GDL_UNDEF; // conversion type
  SizeT maxRank=0;
  ExprListT            exprList;
  BaseGDL*           cTypeData;
		
  //ProgNodeP __t174 = this;
  //ProgNodeP a =  this;
  //match(antlr::RefAST(_t),ARRAYDEF);
  ProgNodeP _t =  this->getFirstChild();
  while(  _t != NULL) {

    BaseGDL* e=_t->Eval();//expr(_t);
    _t = _t->getNextSibling();
    //WRONG    _t = ProgNode::interpreter->_retTree;
			
    // add first (this way it will get cleaned up anyway)
    if( e == NullGDL::GetSingleInstance())
      continue;
      
    exprList.push_back(e);
			
    DType ty=e->Type();
    if( ty == GDL_UNDEF)
      {
	throw GDLException( _t, "Variable is undefined: "+
			    ProgNode::interpreter->Name(e),true,false);
      }
    if( cType == GDL_UNDEF) 
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
				" is not allowed in this context.",true,false);
	      }
			
	    // update order if larger type (or types are equal)
	    if( DTypeOrder[ty] >= DTypeOrder[cType]) 
	      {
		cType=ty;
		cTypeData=e;
	      }
	  }
	if( ty == GDL_STRUCT)
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
				      ProgNode::interpreter->Name(cTypeData)+", "+ProgNode::interpreter->Name(e),true,false);
	      }
	  }
      }
			
    // memorize maximum Rank
    SizeT rank=e->Rank();
    if( rank > maxRank) maxRank=rank;
  }
  _t = this->getNextSibling();
	
  if( exprList.empty())
    return NullGDL::GetSingleInstance();
  
  BaseGDL* res=cTypeData->CatArray(exprList,this->arrayDepth,maxRank);
	
//   ProgNode::interpreter->_retTree = _t;
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
    BaseGDL* e=_t->Eval(); //interpreter->expr(_t);
    _t = _t->getNextSibling();
    //WRONG    _t = ProgNode::interpreter->_retTree;
			
    // also adds to descriptor, grabs
    instance->NewTag( si->getText(), e); 
  }

  instance_guard.release();
  BaseGDL* res=instance;
	
//   ProgNode::interpreter->_retTree = rTree;
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
	// not completely defined yet (only name in list)
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
	    _t = _t->getNextSibling();
	    //WRONG  _t = ProgNode::interpreter->_retTree;
			
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
	    instance->AddParent( inherit); // adds operatorList if GDL_OBJECT
	    
	    // set up operators form member functions
	    // they were set when the struct was not derived from GDL_OBJECT and hence ignored
	    if( nStructDesc == oStructDesc && inherit->Name() == "GDL_OBJECT")
	    {
	      // operatorList should have been set in instance->AddParent( inherit); 
	      assert( nStructDesc->GetOperatorList() != NULL);
	      // find operators in member subroutines
	      nStructDesc->SetupOperators();
	    }
			
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
	    _t = _t->getNextSibling();
	    //WRONG _t = ProgNode::interpreter->_retTree;
			
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
	  //GDLDelete(nStructDesc); // auto_ptr
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
	
//   ProgNode::interpreter->_retTree = this->getNextSibling();
  return res;
}
BaseGDL* NSTRUC_REFNode::Eval()
{
   if( this->dStruct == NULL)
     {
      //   ProgNodeP rTree = this->getNextSibling();
      // 	match(antlr::RefAST(_t),NSTRUC_REF);
      ProgNodeP _t = this->getFirstChild();
      ProgNodeP idRef = _t;
      // 	match(antlr::RefAST(_t),IDENTIFIER);
      
      // find struct 'id'
      // returns it or throws an exception
      /* DStructDesc* */ dStruct=ProgNode::interpreter->GetStruct( idRef->getText(), _t);
      
      _t = _t->getNextSibling();
     }		

  BaseGDL* res = new DStructGDL( dStruct, dimension(1));
	
//   ProgNode::interpreter->_retTree = this->getNextSibling();
  return res;
}


// parameter nodes ////////////////////////////////////////////////
// 1. keywords ////
void KEYDEF_REFNode::Parameter( EnvBaseT* actEnv)
{
  ProgNodeP _t = this->getFirstChild();
//   ProgNodeP knameR = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
//   _t = _t->getNextSibling();
  
  BaseGDL** kvalRef=_t->getNextSibling()->LEval();
  //ProgNode::interpreter->ref_parameter(_t->getNextSibling(), actEnv);

  // pass reference
  actEnv->SetKeyword( _t->getText(), kvalRef); 
			
  ProgNode::interpreter->_retTree = this->getNextSibling();
}

void KEYDEF_REF_EXPRNode::Parameter( EnvBaseT* actEnv)
{
  ProgNodeP _t = this->getFirstChild();
//   ProgNodeP knameE = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
//   _t = _t->getNextSibling();
  BaseGDL* kval= _t->getNextSibling()->Eval();//expr(_t);
  GDLDelete(kval);
//   _t = ProgNode::interpreter->_retTree;
  BaseGDL** kvalRef=_t->getNextSibling()->getNextSibling()->LEval();
//   BaseGDL** kvalRef=ProgNode::interpreter->_retTree->LEval();
//   ProgNode::interpreter->
//     ref_parameter(ProgNode::interpreter->_retTree, actEnv);

  // pass reference
  actEnv->SetKeyword( _t->getText(), kvalRef); 
			
  ProgNode::interpreter->_retTree = this->getNextSibling();
}

void KEYDEFNode::Parameter( EnvBaseT* actEnv)
{
   ProgNodeP _t = this->getFirstChild();
  // 			match(antlr::RefAST(_t),IDENTIFIER);
//   _t = _t->getNextSibling();
  BaseGDL* kval= _t->getNextSibling()->Eval();//expr(_t);
//   _t = ProgNode::interpreter->_retTree;
  // pass value
  actEnv->SetKeyword( _t->getText(), kval);
			
  ProgNode::interpreter->_retTree = this->getNextSibling();
}

void KEYDEF_REF_CHECKNode::Parameter( EnvBaseT* actEnv)
{
//   ProgNodeP _t = this->getFirstChild();
//   ProgNodeP knameCk = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
//   _t = _t->getNextSibling();
  ProgNodeP p = this->getFirstChild()->getNextSibling();

  if( p->getType() == GDLTokenTypes::QUESTION)
  {
    QUESTIONNode* q = static_cast<QUESTIONNode*>( p);
    ProgNodeP branch = q->AsParameter();
    
    while( branch->getType() == GDLTokenTypes::QUESTION)
    {
      QUESTIONNode* qRecursive = static_cast<QUESTIONNode*>( branch);
      branch = qRecursive->AsParameter();
    }
    
    BaseGDL* rVal;
    BaseGDL** lVal = branch->EvalRefCheck( rVal);
    if( lVal != NULL)
    {   // pass reference
      actEnv->SetKeyword(this->getFirstChild()->getText(), lVal); 
    }
    else
    {   // pass value
      actEnv->SetKeyword(this->getFirstChild()->getText(), rVal); 
    }
  }
  else
  {
  BaseGDL* kval=ProgNode::interpreter->
    lib_function_call(this->getFirstChild()->getNextSibling());
			
  BaseGDL** kvalRef = ProgNode::interpreter->callStack.back()->GetPtrTo( kval);
  if( kvalRef != NULL)
    {   // pass reference
      actEnv->SetKeyword(this->getFirstChild()->getText(), kvalRef); 
    }
  else 
    {   // pass value
      actEnv->SetKeyword(this->getFirstChild()->getText(), kval); 
    }
  }
  ProgNode::interpreter->_retTree = this->getNextSibling();
}


// 2. Parameters ////
bool REFNode::ParameterDirect( BaseGDL*& ref)
{
  BaseGDL** pvalRef=this->getFirstChild()->LEval();
  ref = *pvalRef;
  return true;
}
void REFNode::Parameter( EnvBaseT* actEnv)
{
//   ProgNodeP _t = this->getFirstChild();
  BaseGDL** pvalRef=this->getFirstChild()->LEval();
//   ProgNode::interpreter->ref_parameter(this->getFirstChild(), actEnv);
//   _t = ProgNode::interpreter->_retTree;
  // pass reference
  actEnv->SetNextParUnchecked(pvalRef); 
			
  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void REFVNNode::Parameter( EnvBaseT* actEnv)
{
  BaseGDL** pvalRef=this->getFirstChild()->LEval();
  actEnv->SetNextParUncheckedVarNum(pvalRef); 
  ProgNode::interpreter->_retTree = this->getNextSibling();
}


bool REF_EXPRNode::ParameterDirect( BaseGDL*& ref)
{
  BaseGDL* pval= this->getFirstChild()->Eval();//expr(_t);
  GDLDelete(pval);
  BaseGDL** pvalRef=this->getFirstChild()->getNextSibling()->LEval();
  ref = *pvalRef;
  return true;
}
void REF_EXPRNode::Parameter( EnvBaseT* actEnv)
{
  // 			match(antlr::RefAST(_t),REF_EXPR);
//   ProgNodeP _t = this->getFirstChild();
  BaseGDL* pval= this->getFirstChild()->Eval();//expr(_t);
  GDLDelete(pval);
//   _t = ProgNode::interpreter->_retTree;
  BaseGDL** pvalRef=this->getFirstChild()->getNextSibling()->LEval();
//   ProgNode::interpreter->
//     ref_parameter( ProgNode::interpreter->_retTree, actEnv);

  // pass reference
  actEnv->SetNextParUnchecked(pvalRef); 
			
  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void REF_EXPRVNNode::Parameter( EnvBaseT* actEnv)
{
  BaseGDL* pval= this->getFirstChild()->Eval();//expr(_t);
  GDLDelete(pval);
  BaseGDL** pvalRef=this->getFirstChild()->getNextSibling()->LEval();
  actEnv->SetNextParUncheckedVarNum(pvalRef); 
  ProgNode::interpreter->_retTree = this->getNextSibling();
}

// returns true if reference, false else
bool REF_CHECKNode::ParameterDirect( BaseGDL*& pval)
{
  ProgNodeP p = this->getFirstChild();
  if( p->getType() == GDLTokenTypes::QUESTION)
  {
    // for the trinary operator we just use pass by value which should be ok, 
    // as direct functions cannot modify their (single) parameter anyway.
    // LEval might save a copy operation here, but considering that direct functions are
    // probably very seldom called with the trinary operator we leave it like this for now. 
    pval = p->Eval();
    return false; // pass value
  }
  pval=ProgNode::interpreter->lib_function_call(p);
  BaseGDL** pvalRef = ProgNode::interpreter->callStack.back()->GetPtrTo( pval);
  return (pvalRef != NULL);
//   if( pvalRef != NULL)
//     {   // pass reference
//       return true;
//     }
//   else 
//     {   // pass value
//       return false;
//     }
}
void REF_CHECKNode::Parameter( EnvBaseT* actEnv)
{
  ProgNodeP p = this->getFirstChild();

  if( p->getType() == GDLTokenTypes::QUESTION)
  {
    QUESTIONNode* q = static_cast<QUESTIONNode*>( p);
    ProgNodeP branch = q->AsParameter();
    
    while( branch->getType() == GDLTokenTypes::QUESTION)
    {
      QUESTIONNode* qRecursive = static_cast<QUESTIONNode*>( branch);
      branch = qRecursive->AsParameter();
    }
    
    BaseGDL* rVal;
    BaseGDL** lVal = branch->EvalRefCheck( rVal);
    if( lVal != NULL)
    {   // pass reference
      actEnv->SetNextParUnchecked( lVal); 
    }
    else
    {   // pass value
      actEnv->SetNextParUnchecked( rVal); 
    }
  }
  else
  {  
    BaseGDL* pval=ProgNode::interpreter->lib_function_call(this->getFirstChild());
			  
    BaseGDL** pvalRef = ProgNode::interpreter->callStack.back()->GetPtrTo( pval);
    if( pvalRef != NULL)
      {   // pass reference
	actEnv->SetNextParUnchecked( pvalRef); 
      }
    else 
      {   // pass value
	actEnv->SetNextParUnchecked( pval); 
      }
  }
  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void REF_CHECKVNNode::Parameter( EnvBaseT* actEnv)
{
  ProgNodeP p = this->getFirstChild();

  if( p->getType() == GDLTokenTypes::QUESTION)
  {
    QUESTIONNode* q = static_cast<QUESTIONNode*>( p);
    ProgNodeP branch = q->AsParameter();
    
    while( branch->getType() == GDLTokenTypes::QUESTION)
    {
      QUESTIONNode* qRecursive = static_cast<QUESTIONNode*>( branch);
      branch = qRecursive->AsParameter();
    }
    
    BaseGDL* rVal;
    BaseGDL** lVal = branch->EvalRefCheck( rVal);
    if( lVal != NULL)
    {   // pass reference
      actEnv->SetNextParUncheckedVarNum( lVal); 
    }
    else
    {   // pass value
      actEnv->SetNextParUncheckedVarNum( rVal); 
    }
  }
  else
  {
    BaseGDL* pval=ProgNode::interpreter->lib_function_call(this->getFirstChild());
    BaseGDL** pvalRef = ProgNode::interpreter->callStack.back()->GetPtrTo( pval);
    if( pvalRef != NULL)
      {   // pass reference
	actEnv->SetNextParUncheckedVarNum( pvalRef); 
      }
    else 
      {   // pass value
	actEnv->SetNextParUncheckedVarNum( pval); 
      }
  }
  ProgNode::interpreter->_retTree = this->getNextSibling();
}

bool ParameterNode::ParameterDirect( BaseGDL*& pval)
{
  pval = this->getFirstChild()->Eval(); 
  return false;			
}
void ParameterNode::Parameter( EnvBaseT* actEnv)
{
//   BaseGDL* pval=this->Eval();//expr(this);
			
  // pass value
  actEnv->SetNextParUnchecked(this->getFirstChild()->Eval()); 
			
  ProgNode::interpreter->_retTree = this->getNextSibling();
}
void ParameterVNNode::Parameter( EnvBaseT* actEnv)
{
  actEnv->SetNextParUncheckedVarNum(this->getFirstChild()->Eval()); 
  ProgNode::interpreter->_retTree = this->getNextSibling();
}



RetCode  ASSIGNNode::Run()
{
  BaseGDL*  r;
  BaseGDL** l;
  auto_ptr<BaseGDL> r_guard;
	
  //     match(antlr::RefAST(_t),ASSIGN);
  ProgNodeP _t = this->getFirstChild();
  if( NonCopyNode(_t->getType()))
  {
      r= _t->EvalNC(); //ProgNode::interpreter->indexable_expr(_t);
      _t = _t->getNextSibling();
      l=_t->LExpr( r); //ProgNode::interpreter->l_expr(_t, r);
  }
  else if( _t->getType() == GDLTokenTypes::FCALL_LIB)
  {
      r=ProgNode::interpreter->lib_function_call(_t);
      _t = ProgNode::interpreter->_retTree;		      
      if( !ProgNode::interpreter->callStack.back()->Contains( r)) 
 	r_guard.reset( r); // guard if no global data
      l=_t->LExpr( r); //ProgNode::interpreter->l_expr(_t, r);
  }
  else
  {
      r=_t->Eval(); //ProgNode::interpreter->indexable_tmp_expr(_t);
      _t = _t->getNextSibling();
//       _t = ProgNode::interpreter->_retTree;
      r_guard.reset( r);
      l=_t->LExpr( r); //ProgNode::interpreter->l_expr(_t, r);
  }
//     switch ( _t->getType()) {
//     case GDLTokenTypes::CONSTANT:
//     case GDLTokenTypes::DEREF:
//     case GDLTokenTypes::SYSVAR:
//     case GDLTokenTypes::VAR:
//     case GDLTokenTypes::VARPTR:
//       {
// 	r= ProgNode::interpreter->indexable_expr(_t);
// 	_t = ProgNode::interpreter->_retTree;
// 	break;
//       }
//     case GDLTokenTypes::FCALL_LIB:
//       {
// 	r=ProgNode::interpreter->lib_function_call(_t);
// 	_t = ProgNode::interpreter->_retTree;
// 			
// 	if( !ProgNode::interpreter->callStack.back()->Contains( r)) 
// 	  r_guard.reset( r); // guard if no global data
// 			
// 	break;
//       }
//     default:
//       {
// 	r=ProgNode::interpreter->indexable_tmp_expr(_t);
// 	_t = ProgNode::interpreter->_retTree;
// 	r_guard.reset( r);
// 	break;
//       }
//     }//switch
//   l=_t->LExpr( r); //ProgNode::interpreter->l_expr(_t, r);

  ProgNode::interpreter->_retTree = this->getNextSibling();
  return RC_OK;
}



RetCode  ASSIGN_ARRAYEXPR_MFCALLNode::Run()
{
  BaseGDL*  r;
  BaseGDL** l;
  auto_ptr<BaseGDL> r_guard;

  //match(antlr::RefAST(_t),ASSIGN_REPLACE);
  ProgNodeP _t = this->getFirstChild();
  {
    // BOTH
    if( _t->getType() ==  GDLTokenTypes::FCALL_LIB)
      {
	r=ProgNode::interpreter->lib_function_call(_t);

	if( r == NULL) // ROUTINE_NAMES
		ProgNode::interpreter->callStack.back()->Throw( "Undefined return value");

	_t = ProgNode::interpreter->_retTree;
	
	if( !ProgNode::interpreter->callStack.back()->Contains( r)) 
		r_guard.reset( r);
			
      }
    else
      {
	// ASSIGN
	if( NonCopyNode(_t->getType()))
	{
	  r= _t->EvalNC(); //ProgNode::interpreter->indexable_expr(_t);
	  _t = _t->getNextSibling(); // ProgNode::interpreter->_retTree;
	}
	else
	{
	  r= _t->Eval(); //ProgNode::interpreter->indexable_tmp_expr(_t);
	  _t = _t->getNextSibling();
// 	  _t = ProgNode::interpreter->_retTree;
	  r_guard.reset( r);
	}

// 	switch ( _t->getType()) {
// 	  case GDLTokenTypes::CONSTANT:
// 	  case GDLTokenTypes::DEREF:
// 	  case GDLTokenTypes::SYSVAR:
// 	  case GDLTokenTypes:: VAR:
// 	  case GDLTokenTypes::VARPTR:
// 	  {
// 	    r= ProgNode::interpreter->indexable_expr(_t);
// 	    _t = ProgNode::interpreter->_retTree;
// 	    break;
// 	  }
// 	  default:
// 	  {
// 	    r=ProgNode::interpreter->indexable_tmp_expr(_t);
// 	    _t = ProgNode::interpreter->_retTree;
// 	    r_guard.reset( r);
// 	    break;
// 	  }
// 	}//switch
    }
  }

//     ProgNodeP lExpr = _t;
    
    // try MFCALL
    try
    {
      l=ProgNode::interpreter->l_arrayexpr_mfcall_as_mfcall(_t);
  
      if( r != (*l))
	{
	  GDLDelete(*l);

	  if( r_guard.get() == r)
	    *l = r_guard.release();
	  else
	    *l = r->Dup();
	}
    }
    catch( GDLException& e)
    {
      // try ARRAYEXPR
      try
      {
	l=ProgNode::interpreter->l_arrayexpr_mfcall_as_arrayexpr(_t/*lExpr*/, r);
      }
      catch( GDLException& e2)
      {
	throw GDLException(e.toString() + " or "+e2.toString());
      }
    }

  ProgNode::interpreter->_retTree = this->getNextSibling();
  return RC_OK;
}



RetCode  ASSIGN_REPLACENode::Run()
{
  BaseGDL*  r;
  auto_ptr<BaseGDL> r_guard;

  //match(antlr::RefAST(_t),ASSIGN_REPLACE);
  ProgNodeP _t = this->getFirstChild();
  {
//     if( _t->getType() ==  GDLTokenTypes::FCALL_LIB)
//       {
// 	r=_t->Eval();//different: ProgNode::interpreter->lib_function_call(_t);
// 	_t = _t->getNextSibling(); //ProgNode::interpreter->_retTree;
// 	assert(_t != NULL);
// 	r_guard.reset( r);
// // 	if( !ProgNode::interpreter->callStack.back()->Contains( r))
// // 		r_guard.reset( r);
// // 	else
// // 		r_guard.reset( r->Dup());
//       }
//     else
      {
	//r=ProgNode::interpreter->tmp_expr(_t);
 	r = _t->Eval();
	r_guard.reset( r);
 	_t = _t->getNextSibling();
	assert(_t != NULL);
      }
  }
//   switch ( _t->getType()) {
//   case GDLTokenTypes::VAR:
//   case GDLTokenTypes::VARPTR:
//   case GDLTokenTypes::DEREF:
//     {
//       l=_t->LEval(); //ProgNode::interpreter->l_simple_var(_t);
// //       _t = ProgNode::interpreter->_retTree;
//       break;
//     }
//   default:
// //   case GDLTokenTypes::FCALL:
// //   case GDLTokenTypes::FCALL_LIB:
// //   case GDLTokenTypes::MFCALL:
// //   case GDLTokenTypes::MFCALL_PARENT:
//     {
//       l=ProgNode::interpreter->l_function_call(_t);
// //       _t = ProgNode::interpreter->_retTree;
//       break;
//     }
//   } // switch
  
  BaseGDL** l=_t->LEval();

  if( r != (*l)) // && (*l) != NullGDL::GetSingleInstance())
    GDLDelete(*l);
  
  *l = r_guard.release();

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  return RC_OK;
}



RetCode  PCALL_LIBNode::Run()
{
//   // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
//   StackGuard<EnvStackT> guard( ProgNode::interpreter->CallStack());
  BaseGDL *self;
	
  // 		match(antlr::RefAST(_t),PCALL_LIB);
  ProgNodeP _t = this->getFirstChild();
  ProgNodeP pl = _t;
  // 		match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
		
  EnvT* newEnv=new EnvT( pl, pl->libPro);//libProList[pl->proIx]);
		
  ProgNode::interpreter->parameter_def_nocheck(_t, newEnv);
  auto_ptr<EnvT> guardEnv( newEnv);

  //   _t = _retTree;
  //if( this->getLine() != 0) ProgNode::interpreter->callStack.back()->SetLineNumber( this->getLine());
		
//   // push environment onto call stack
//   ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  static_cast<DLibPro*>(newEnv->GetPro())->Pro()(newEnv);

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  //  ProgNode::interpreter->_retTree = this->getNextSibling();
  return RC_OK;
}



RetCode  MPCALLNode::Run()
{
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(ProgNode::interpreter->CallStack());
  BaseGDL *self;
  EnvUDT*   newEnv;
	
  // 			match(antlr::RefAST(_t),MPCALL);
  ProgNodeP _t = this->getFirstChild();
  self=_t->Eval(); //ProgNode::interpreter->expr(_t);
  _t = _t->getNextSibling(); // ProgNode::interpreter->_retTree;
  ProgNodeP mp = _t;
  // 			match(antlr::RefAST(_t),IDENTIFIER);
  _t = _t->getNextSibling();
			
  auto_ptr<BaseGDL> self_guard(self);
			
  newEnv=new EnvUDT( mp, self);
			
  self_guard.release();
			
  ProgNode::interpreter->parameter_def(_t, newEnv);

  //if( this->getLine() != 0) ProgNode::interpreter->callStack.back()->SetLineNumber( this->getLine());

  // push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  ProgNode::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  return RC_OK;
}



RetCode  MPCALL_PARENTNode::Run()
{
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(ProgNode::interpreter->callStack);
  BaseGDL *self;
  EnvUDT*   newEnv;

  // 			match(antlr::RefAST(_t),MPCALL_PARENT);
  ProgNodeP _t = this->getFirstChild();
  self=_t->Eval(); //ProgNode::interpreter->expr(_t);
  _t = _t->getNextSibling(); // ProgNode::interpreter->_retTree;
//   self=ProgNode::interpreter->expr(_t);
//   _t = ProgNode::interpreter->_retTree;
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

  //if( this->getLine() != 0) ProgNode::interpreter->callStack.back()->SetLineNumber( this->getLine());

  // push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  ProgNode::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  //  ProgNode::interpreter->_retTree = this->getNextSibling();
  return RC_OK;
}

RetCode  PCALLNode::Run()
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

//     if( _t->getLine() != 0) 
  //if( this->getLine() != 0) ProgNode::interpreter->callStack.back()->SetLineNumber( this->getLine());
// push environment onto call stack
  ProgNode::interpreter->callStack.push_back(newEnv);
		
  // make the call
  ProgNode::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  //  ProgNode::interpreter->_retTree = this->getNextSibling();
  return RC_OK;
}

BaseGDL* POSTDECNode::Eval()
{
  BaseGDL* res=interpreter->l_decinc_expr( this->getFirstChild(), GDLTokenTypes::POSTDEC);
//   interpreter->SetRetTree(this->getNextSibling());
  return res;
}

BaseGDL* DECNode::Eval()
{
  BaseGDL* res=interpreter->l_decinc_expr( this->getFirstChild(), GDLTokenTypes::DEC);
//   interpreter->SetRetTree(this->getNextSibling());
  return res;
}

RetCode  DECNode::Run()
{
  //		match(antlr::RefAST(_t),DEC);
  ProgNodeP _t = this->getFirstChild();
  ProgNode::interpreter->l_decinc_expr(_t, GDLTokenTypes::DECSTATEMENT);
  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  return RC_OK;
}


BaseGDL* POSTINCNode::Eval()
{
  BaseGDL* res=interpreter->l_decinc_expr( this->getFirstChild(), GDLTokenTypes::POSTINC);
//   interpreter->SetRetTree(this->getNextSibling());
  return res;
}

BaseGDL* INCNode::Eval()
{
  BaseGDL* res=interpreter->l_decinc_expr( this->getFirstChild(), GDLTokenTypes::INC);
//   interpreter->SetRetTree(this->getNextSibling());
  return res;
}

RetCode  INCNode::Run()
{
  //		match(antlr::RefAST(_t),INC);
  ProgNodeP _t = this->getFirstChild();
  ProgNode::interpreter->l_decinc_expr(_t, GDLTokenTypes::INCSTATEMENT);
  ProgNode::interpreter->SetRetTree( this->getNextSibling());
  return RC_OK;
}

RetCode   FORNode::Run()//for_statement(ProgNodeP _t) {
{
  EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
  
  ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);

  ProgNodeP vP = this->GetNextSibling()->GetFirstChild();
  
  BaseGDL** v=vP->LEval(); //ProgNode::interpreter->l_simple_var(vP);
  
  BaseGDL* s=this->GetFirstChild()->Eval(); 
//   BaseGDL* s=ProgNode::interpreter->expr( this->GetFirstChild());
  auto_ptr<BaseGDL> s_guard(s);
  
  GDLDelete(loopInfo.endLoopVar);
  loopInfo.endLoopVar=this->GetFirstChild()->GetNextSibling()->Eval();
//   loopInfo.endLoopVar=ProgNode::interpreter->expr(this->GetFirstChild()->GetNextSibling());
  
  s->ForCheck( &loopInfo.endLoopVar);
  
  if( loopInfo.endLoopVar->Type() != s->Type()) // promote s
    {
      BaseGDL* sPromote = s->Convert2(loopInfo.endLoopVar->Type(), BaseGDL::COPY);
      s_guard.reset( sPromote);
    }
  
  // ASSIGNMENT used here also
  GDLDelete((*v));
  (*v)= s_guard.release(); // s held in *v after this
  
  if( (*v)->ForCondUp( loopInfo.endLoopVar))
  {
	  ProgNode::interpreter->_retTree = vP->GetNextSibling();
	  return RC_OK;
  }
  else
  {
	  // skip if initial test fails
	  ProgNode::interpreter->_retTree = this->GetNextSibling()->GetNextSibling();
	  return RC_OK;
  }
}


RetCode   FOR_LOOPNode::Run()
{
	EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
	ForLoopInfoT& loopInfo = 	callStack_back->GetForLoopInfo( this->forLoopIx);
// 		BaseGDL* endLoopVar = 	loopInfo.endLoopVar;
	if( loopInfo.endLoopVar == NULL)
	{
		// non-initialized loop (GOTO)
		ProgNode::interpreter->_retTree = this->GetNextSibling();
		return RC_OK;
	}

	// // problem:
	// // EXECUTE may call DataListT.loc.resize(), as v points to the
	// // old sequence v might be invalidated -> segfault
	// // note that the value (*v) is preserved by resize()
	
	BaseGDL** v=this->getFirstChild()->LEval();//ProgNode::interpreter->l_simple_var(this->getFirstChild());

// shortCut:;
	
	//(*v)->ForAdd();
	if( (*v)->ForAddCondUp( loopInfo.endLoopVar))
	{
		ProgNode::interpreter->_retTree = this->statementList; //GetFirstChild()->GetNextSibling();
// 			if( ProgNode::interpreter->_retTree == this) goto shortCut;
	}
	else
	{
		GDLDelete(loopInfo.endLoopVar);
		loopInfo.endLoopVar = NULL;
		ProgNode::interpreter->_retTree = this->GetNextSibling();
	}
	return RC_OK;
}

	
RetCode   FOR_STEPNode::Run()//for_statement(ProgNodeP _t) {
{
  EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());

  ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);

  ProgNodeP vP = this->GetNextSibling()->GetFirstChild();

  BaseGDL** v=vP->LEval(); //ProgNode::interpreter->l_simple_var(vP);

  BaseGDL* s=this->GetFirstChild()->Eval(); 
//  BaseGDL* s=ProgNode::interpreter->expr( this->GetFirstChild());
  auto_ptr<BaseGDL> s_guard(s);

  GDLDelete(loopInfo.endLoopVar);
  loopInfo.endLoopVar=this->GetFirstChild()->GetNextSibling()->Eval();
//  loopInfo.endLoopVar=ProgNode::interpreter->expr(this->GetFirstChild()->GetNextSibling());

  GDLDelete(loopInfo.loopStepVar);
  loopInfo.loopStepVar=this->GetFirstChild()->GetNextSibling()->GetNextSibling()->Eval();
//   loopInfo.loopStepVar=ProgNode::interpreter->expr(this->GetFirstChild()->GetNextSibling()->GetNextSibling());

  s->ForCheck( &loopInfo.endLoopVar, &loopInfo.loopStepVar);

  if( loopInfo.endLoopVar->Type() != s->Type()) // promote s
    {
      BaseGDL* sPromote = s->Convert2(loopInfo.endLoopVar->Type(), BaseGDL::COPY);
      s_guard.reset( sPromote);
      assert( loopInfo.loopStepVar->Type() == s_guard.get()->Type());
    }

  // ASSIGNMENT used here also
  GDLDelete((*v));
  (*v)= s_guard.release(); // s held in *v after this

  if( loopInfo.loopStepVar->Sgn() == -1)
  {
	  if( (*v)->ForCondDown( loopInfo.endLoopVar))
	  {
		  ProgNode::interpreter->_retTree = vP->GetNextSibling();
		  return RC_OK;

	  }
  }
  else
  {
	  if( (*v)->ForCondUp( loopInfo.endLoopVar))
	  {
		  ProgNode::interpreter->_retTree = vP->GetNextSibling();
		  return RC_OK;

	  }
  }
  // skip if initial test fails
  ProgNode::interpreter->_retTree = this->GetNextSibling()->GetNextSibling();
  return RC_OK;
}
	
RetCode   FOR_STEP_LOOPNode::Run()
{
  EnvUDT* callStack_back = static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
  
  ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);
  if( loopInfo.endLoopVar == NULL)
  {
    // non-initialized loop (GOTO)
    ProgNode::interpreter->_retTree = this->GetNextSibling();
    return RC_OK;
  }

  // // problem:
  // // EXECUTE may call DataListT.loc.resize(), as v points to the
  // // old sequence v might be invalidated -> segfault
  // // note that the value (*v) is preserved by resize()

  BaseGDL** v=this->GetFirstChild()->LEval(); //ProgNode::interpreter->l_simple_var(this->GetFirstChild());

  (*v)->ForAdd(loopInfo.loopStepVar);
  if( loopInfo.loopStepVar->Sgn() == -1)
  {
    if( (*v)->ForCondDown( loopInfo.endLoopVar))
    {
	    ProgNode::interpreter->_retTree = this->GetFirstChild()->GetNextSibling();
	    return RC_OK;
    }
  }
  else
  {
    if( (*v)->ForCondUp( loopInfo.endLoopVar))
    {
	    ProgNode::interpreter->_retTree = this->GetFirstChild()->GetNextSibling();
	    return RC_OK;
    }
  }
  
  GDLDelete(loopInfo.endLoopVar);
  loopInfo.endLoopVar = NULL;
  GDLDelete(loopInfo.loopStepVar);
  loopInfo.loopStepVar = NULL;
  ProgNode::interpreter->_retTree = this->GetNextSibling();
  return RC_OK;
}

RetCode   FOREACHNode::Run()
{
	EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
	ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);

	ProgNodeP vP = this->GetNextSibling()->GetFirstChild();

	BaseGDL** v=vP->LEval(); // ProgNode::interpreter->l_simple_var(vP);

	GDLDelete(loopInfo.endLoopVar);
	loopInfo.endLoopVar=this->GetFirstChild()->Eval();
//	loopInfo.endLoopVar=ProgNode::interpreter->expr(this->GetFirstChild());

	loopInfo.foreachIx = 0;

	// currently there are no empty arrays
	//SizeT nEl = loopInfo.endLoopVar->N_Elements();

	// ASSIGNMENT used here also
	GDLDelete((*v));
	(*v) = loopInfo.endLoopVar->NewIx( 0);

	ProgNode::interpreter->_retTree = vP->GetNextSibling();
	return RC_OK;
}
	
RetCode   FOREACH_LOOPNode::Run()
{
	EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
	ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);

	if( loopInfo.endLoopVar == NULL)
	{
	// non-initialized loop (GOTO)
	ProgNode::interpreter->_retTree = this->GetNextSibling();
	return RC_OK;
	}

	BaseGDL** v=this->GetFirstChild()->LEval(); //ProgNode::interpreter->l_simple_var(this->GetFirstChild());
	
	++loopInfo.foreachIx;

	SizeT nEl = loopInfo.endLoopVar->N_Elements();

	if( loopInfo.foreachIx < nEl)
	{
		// ASSIGNMENT used here also
		GDLDelete((*v));
		(*v) = loopInfo.endLoopVar->NewIx( loopInfo.foreachIx);

		ProgNode::interpreter->_retTree = this->GetFirstChild()->GetNextSibling();
		return RC_OK;
	}

	GDLDelete(loopInfo.endLoopVar);
	loopInfo.endLoopVar = NULL;
	// 	loopInfo.foreachIx = -1;
	ProgNode::interpreter->SetRetTree( this->GetNextSibling());
	return RC_OK;
}



RetCode   FOREACH_INDEXNode::Run()
{
	EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
	ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);

	ProgNodeP vP = this->GetNextSibling()->GetFirstChild();
	ProgNodeP indexP = vP->GetNextSibling();

	BaseGDL** v=vP->LEval(); //ProgNode::interpreter->l_simple_var(vP);
	BaseGDL** index=indexP->LEval(); //ProgNode::interpreter->l_simple_var(indexP);

	GDLDelete(loopInfo.endLoopVar);
	loopInfo.endLoopVar=this->GetFirstChild()->Eval(); 
// 	loopInfo.endLoopVar=ProgNode::interpreter->expr(this->GetFirstChild());

	loopInfo.foreachIx = 0;

	// currently there are no empty arrays
	//SizeT nEl = loopInfo.endLoopVar->N_Elements();

	// ASSIGNMENT used here also
	GDLDelete((*v));
	(*v) = loopInfo.endLoopVar->NewIx( 0);
	
	// ASSIGNMENT used here also
	GDLDelete((*index));
	(*index) = new DLongGDL( 0);

	ProgNode::interpreter->_retTree = indexP->GetNextSibling();
	return RC_OK;
}

RetCode   FOREACH_INDEX_LOOPNode::Run()
{
	EnvUDT* callStack_back = 	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back());
	ForLoopInfoT& loopInfo = callStack_back->GetForLoopInfo( this->forLoopIx);

	if( loopInfo.endLoopVar == NULL)
	{
	// non-initialized loop (GOTO)
	ProgNode::interpreter->_retTree = this->GetNextSibling();
	return RC_OK;
	}

	BaseGDL** v=this->GetFirstChild()->LEval(); //ProgNode::interpreter->l_simple_var(this->GetFirstChild());
	BaseGDL** index=this->GetFirstChild()->GetNextSibling()->LEval(); //ProgNode::interpreter->l_simple_var(this->GetFirstChild()->GetNextSibling());
	
	++loopInfo.foreachIx;

	SizeT nEl = loopInfo.endLoopVar->N_Elements();

	if( loopInfo.foreachIx < nEl)
	{
		// ASSIGNMENT used here also
		GDLDelete((*v));
		(*v) = loopInfo.endLoopVar->NewIx( loopInfo.foreachIx);

		// ASSIGNMENT used here also
		GDLDelete((*index));
		(*index) = new DLongGDL( loopInfo.foreachIx);
		
		ProgNode::interpreter->_retTree = this->GetFirstChild()->GetNextSibling()->GetNextSibling();
		return RC_OK;
	}

	GDLDelete(loopInfo.endLoopVar);
	loopInfo.endLoopVar = NULL;
	// 	loopInfo.foreachIx = -1;
	ProgNode::interpreter->SetRetTree( this->GetNextSibling());
  return RC_OK;
}



RetCode   REPEATNode::Run()
{
  // _t is REPEAT_LOOP, GetFirstChild() is expr, GetNextSibling is first loop statement
  if( this->GetFirstChild()->GetFirstChild()->GetNextSibling() == NULL)
	  ProgNode::interpreter->SetRetTree( this->GetFirstChild());
  else	
	  ProgNode::interpreter->SetRetTree( this->GetFirstChild()->GetFirstChild()->GetNextSibling());     // statement
  return RC_OK;
}



RetCode   REPEAT_LOOPNode::Run()
{
  auto_ptr<BaseGDL> eVal( this->GetFirstChild()->Eval());
//  auto_ptr<BaseGDL> eVal( ProgNode::interpreter->expr(this->GetFirstChild()));
  if( eVal.get()->False())
  {
  ProgNode::interpreter->SetRetTree( this->GetFirstChild()->GetNextSibling());     // 1st loop statement
  if(  this->GetFirstChild()->GetNextSibling() == NULL)
	  throw GDLException(this,	"Empty REPEAT loop entered (infinite loop).",true,false);
  return RC_OK;
  }
  
  ProgNode::interpreter->SetRetTree( this->GetNextSibling());     // statement
  return RC_OK;
}



RetCode   WHILENode::Run()
{
  auto_ptr<BaseGDL> e1_guard;
  BaseGDL* e1;
  ProgNodeP evalExpr = this->getFirstChild();
  if( NonCopyNode( evalExpr->getType()))
  {
    e1 = evalExpr->EvalNC();
  }
  else
  {
    e1 = evalExpr->Eval();
    e1_guard.reset(e1);
  }
// 	auto_ptr<BaseGDL> eVal( ProgNode::interpreter->expr( this->GetFirstChild()));
  if( e1->True()) 
  {
    ProgNode::interpreter->SetRetTree( this->GetFirstChild()->GetNextSibling());
    if( this->GetFirstChild()->GetNextSibling() == NULL)
	    throw GDLException(this,"Empty WHILE loop entered (infinite loop).",true,false);
  }
  else
  {
    ProgNode::interpreter->SetRetTree( this->GetNextSibling());
  }
  return RC_OK;
}



RetCode   IFNode::Run()
 {
  auto_ptr<BaseGDL> e1_guard;
  BaseGDL* e1;
  ProgNodeP evalExpr = this->getFirstChild();
  if( NonCopyNode( evalExpr->getType()))
  {
	e1 = evalExpr->EvalNC();
  }
  else
  {
	e1 = evalExpr->Eval();
    e1_guard.reset(e1);
  }
//	auto_ptr<BaseGDL> eVal( ProgNode::interpreter->expr( this->GetFirstChild()));
	if( e1->True()) 
	{
		ProgNode::interpreter->SetRetTree( this->GetFirstChild()->GetNextSibling());
	}
	else
	{
		ProgNode::interpreter->SetRetTree( this->GetNextSibling());
	}
  return RC_OK;
}

RetCode   IF_ELSENode::Run()
{	
  auto_ptr<BaseGDL> e1_guard;
  BaseGDL* e1;
  ProgNodeP evalExpr = this->getFirstChild();
  if( NonCopyNode( evalExpr->getType()))
  {
	e1 = evalExpr->EvalNC();
  }
  else
  {
	e1 = evalExpr->Eval();
	e1_guard.reset(e1);
  }
// 	auto_ptr<BaseGDL> eVal( ProgNode::interpreter->expr( this->GetFirstChild()));
  if( e1->True()) 
  {
	  ProgNode::interpreter->SetRetTree( this->GetFirstChild()->GetNextSibling()->GetFirstChild());
  }
  else
  {
	  ProgNode::interpreter->SetRetTree( this->GetFirstChild()->GetNextSibling()->GetNextSibling());
  }
  return RC_OK;
}



RetCode   CASENode::Run()
{
  auto_ptr<BaseGDL> e1_guard;
  BaseGDL* e1;
  ProgNodeP evalExpr = this->getFirstChild();
  if( NonCopyNode( evalExpr->getType()))
  {
	e1 = evalExpr->EvalNC();
  }
  else
  {
	e1 = evalExpr->Eval();
    e1_guard.reset(e1);
  }

// 	auto_ptr<BaseGDL> eVal( ProgNode::interpreter->expr( this->GetFirstChild()));
  	if( !e1->Scalar())
		throw GDLException( this->GetFirstChild(), "Expression must be a"
			" scalar in this context: "+ProgNode::interpreter->Name(e1),true,false);

	ProgNodeP b=this->GetFirstChild()->GetNextSibling(); // remeber block begin
 
	for( int i=0; i<this->numBranch; ++i)
	{
		if( b->getType() == GDLTokenTypes::ELSEBLK)
		{
			ProgNodeP sL = b->GetFirstChild(); // statement_list
		
			if(sL != NULL )
			{
				ProgNode::interpreter->SetRetTree( sL);
			}
			else
			{
				ProgNode::interpreter->SetRetTree( this->GetNextSibling());
			}
			return RC_OK;
		}
		else
		{
			ProgNodeP ex = b->GetFirstChild();  // EXPR
							
			auto_ptr<BaseGDL> ee_guard;
			BaseGDL* ee;
			if( NonCopyNode( ex->getType()))
				{
					ee = ex->EvalNC();
				}
			else
				{
					ee = ex->Eval();
					ee_guard.reset(ee);
				}
// 			BaseGDL* ee=ProgNode::interpreter->expr(ex);
			// auto_ptr<BaseGDL> ee_guard(ee);
			bool equalexpr=e1->EqualNoDelete(ee); // Equal deletes ee
		
			if( equalexpr)
			{
				ProgNodeP bb = ex->GetNextSibling(); // statement_list
				if(bb != NULL )
				{
					ProgNode::interpreter->SetRetTree( bb);
					return RC_OK;
				}
				else
				{
					ProgNode::interpreter->SetRetTree( this->GetNextSibling());
					return RC_OK;
				}
			}
		}
		b=b->GetNextSibling(); // next block
	} // for
	
	throw GDLException( this, "CASE statement found no match.",true,false);
	return RC_OK;
}



RetCode   SWITCHNode::Run()
{
  auto_ptr<BaseGDL> e1_guard;
  BaseGDL* e1;
  ProgNodeP evalExpr = this->getFirstChild();
  if( NonCopyNode( evalExpr->getType()))
  {
	e1 = evalExpr->EvalNC();
  }
  else
  {
	e1 = evalExpr->Eval();
    e1_guard.reset(e1);
  }

// 	auto_ptr<BaseGDL> eVal( ProgNode::interpreter->expr( this->GetFirstChild()));
 	if( !e1->Scalar())
	throw GDLException( this->GetFirstChild(), "Expression must be a"
	" scalar in this context: "+ProgNode::interpreter->Name(e1),true,false);

	ProgNodeP b=this->GetFirstChild()->GetNextSibling(); // remeber block begin
	
	bool hook=false; // switch executes everything after 1st match
	for( int i=0; i<this->numBranch; i++)
	{
		if( b->getType() == GDLTokenTypes::ELSEBLK)
			{
				hook=true;
				
				ProgNodeP sL = b->GetFirstChild(); // statement_list
				
				if(sL != NULL )
				{
					ProgNode::interpreter->SetRetTree( sL);
					return RC_OK;
				}
			}
		else
			{
				ProgNodeP ex = b->GetFirstChild();  // EXPR
				
				if( !hook)
				{
					auto_ptr<BaseGDL> ee_guard;
					BaseGDL* ee;
					if( NonCopyNode( ex->getType()))
					{
						ee = ex->EvalNC();
					}
					else
					{
						ee = ex->Eval();
						ee_guard.reset(ee);
					}
// 					BaseGDL* ee=ProgNode::interpreter->expr(ex);
					// auto_ptr<BaseGDL> ee_guard(ee);
					hook=e1->EqualNoDelete(ee); // Equal deletes ee
				}
				
				if( hook)
				{
					ProgNodeP bb = ex->GetNextSibling(); // statement_list
					// statement there
					if(bb != NULL )
					{
						ProgNode::interpreter->SetRetTree( bb);
						return RC_OK;
					}
				}
			}
		b=b->GetNextSibling(); // next block
	} // for
	ProgNode::interpreter->SetRetTree( this->GetNextSibling());
	return RC_OK;
}

RetCode   BLOCKNode::Run()
{
	ProgNode::interpreter->SetRetTree( this->getFirstChild());
  return RC_OK;
}

RetCode      GOTONode::Run()
	{
		ProgNode::interpreter->SetRetTree( static_cast<EnvUDT*>(GDLInterpreter::CallStack().back())->
			GotoTarget( targetIx)->GetNextSibling());
  return RC_OK;
	}
RetCode      CONTINUENode::Run()
{
  if( this->breakTarget == NULL)
	{
	assert( interpreter != NULL);
	EnvBaseT* e = interpreter->CallStack().back();
	throw GDLException( this, "CONTINUE must be enclosed within a FOR, WHILE, or REPEAT loop.", true, false);
	}
  ProgNode::interpreter->SetRetTree( this->breakTarget);
  return RC_OK;
}
RetCode      BREAKNode::Run()
{
  if( !this->breakTargetSet)
	{
	assert( interpreter != NULL);
	EnvBaseT* e = interpreter->CallStack().back();
	throw GDLException( this, "BREAK must be enclosed within a loop (FOR, WHILE, REPEAT), CASE or SWITCH statement.", true, false);
	}
  ProgNode::interpreter->SetRetTree( this->breakTarget);
  return RC_OK;
}
RetCode      LABELNode::Run()
{
 ProgNode::interpreter->SetRetTree( this->GetNextSibling());
   return RC_OK;
}
RetCode      ON_IOERROR_NULLNode::Run()
{
	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back())->SetIOError( -1);
	ProgNode::interpreter->SetRetTree( this->GetNextSibling());
  return RC_OK;
}
RetCode      ON_IOERRORNode::Run()
{
	static_cast<EnvUDT*>(GDLInterpreter::CallStack().back())->SetIOError( this->targetIx);
	ProgNode::interpreter->SetRetTree( this->GetNextSibling());
  return RC_OK;
}


RetCode   RETFNode::Run()
{
	ProgNodeP _t = this->getFirstChild();
	assert( _t != NULL);
	if ( !static_cast<EnvUDT*>(GDLInterpreter::CallStack().back())->LFun())
		{
			BaseGDL* e=_t->Eval(); //ProgNode::interpreter->expr(_t);
			interpreter->SetRetTree( _t->getNextSibling()); // ???
			GDLDelete(ProgNode::interpreter->returnValue);
			ProgNode::interpreter->returnValue=e;

			GDLInterpreter::CallStack().back()->RemoveLoc( e); // steal e from local list
		}
	else
		{
			BaseGDL** eL=ProgNode::interpreter->l_ret_expr(_t);
			// returnValueL is otherwise owned
			ProgNode::interpreter->returnValueL=eL;
		}
	//if( !(interruptEnable && sigControlC) && ( debugMode == DEBUG_CLEAR))
	//return RC_RETURN;
	return RC_RETURN;
}

RetCode   RETPNode::Run()
{
	return RC_RETURN;
}







