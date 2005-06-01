/***************************************************************************
                          envt.cpp  -  description
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

#include <iomanip>

#include "objects.hpp"
#include "GDLInterpreter.hpp"
#include "envt.hpp"

#include <assert.h> // always as last

using namespace std;

EnvT::EnvT( GDLInterpreter* ipr, RefDNode cN, DSub* pro_, bool lF): 
  interpreter(ipr), env(), pro(pro_),
  ioError(NULL), onError( -1), catchVar(NULL), catchNode(NULL), 
  obj(false), 
  extra(this), callingNode( cN),
  lFun( lF),
  nJump( 0),
  lastJump( -1)
{
  DSubUD* proUD=dynamic_cast<DSubUD*>(pro); // ???
  SizeT envSize;
  SizeT keySize;
  if( proUD != NULL)
    { // UD fun/pro
      envSize=proUD->var.size();
      keySize=proUD->key.size();
    }
  else
    { // Lib fun/pro
      keySize=pro->key.size();
      if( pro->nPar >= 0)
	envSize=pro->nPar+pro->key.size();
      else
	{
	envSize=keySize;
	// performance optimization
	env.reserve(envSize+5);
	}
    }
  env.resize(envSize);
  parIx=keySize; // set to first parameter
}

// member pro
EnvT::EnvT( GDLInterpreter* ipr,  RefDNode cN, BaseGDL* self, 
	    const string& parent): 
  interpreter(ipr), env(), pro(NULL), 
  ioError(NULL), onError( -1), catchVar(NULL), catchNode(NULL), 
  obj(true),
  extra(this), callingNode( cN),
  lFun( false),
  nJump( 0),
  lastJump( -1)
{
  DStructGDL* oStructGDL = interpreter->ObjectStruct( self, cN);

  const string& mp = cN->getText();

  DStructDesc* desc=oStructGDL->Desc();

  if( parent != "")
    {
      pro=desc->GetPro( mp, parent);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   parent+"::"+mp);
    }
  else
    {
      pro=desc->GetPro( mp);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   desc->Name()+"::"+mp);
    }

  DSubUD* proUD=dynamic_cast<DSubUD*>(pro); // ???
  SizeT envSize;
  SizeT keySize;
  if( proUD != NULL)
    { // UD fun/pro
      envSize=proUD->var.size();
      keySize=proUD->key.size();
    }
  else
    { // Lib fun/pro
      keySize=pro-> key.size();
      if( pro->nPar >= 0)
	envSize=pro->nPar+pro->key.size();
      else
	{
	envSize=keySize;
	// performance optimization
	env.reserve(envSize+5);
	}
    }

  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by value (self must not be changed)
  env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}

// member fun
EnvT::EnvT( GDLInterpreter* ipr, BaseGDL* self, //DStructGDL* oStructGDL,  
	    RefDNode cN, const string& parent, bool lF): 
  interpreter(ipr), env(), pro(NULL), 
  ioError(NULL), onError( -1), catchVar(NULL), catchNode(NULL), 
  obj(true),
  extra(this), callingNode( cN),
  lFun( lF),
  nJump( 0),
  lastJump( -1)
{
  DStructGDL* oStructGDL = interpreter->ObjectStruct( self, cN);

  const string& mp = cN->getText();

  DStructDesc* desc=oStructGDL->Desc();
  
  if( parent != "")
    {
      pro=desc->GetFun( mp, parent);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   parent+"::"+mp);
    }
  else
    {
      pro=desc->GetFun( mp);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   desc->Name()+"::"+mp);
    }

  DSubUD* proUD=dynamic_cast<DSubUD*>(pro); // ???
  SizeT envSize;
  SizeT keySize;
  if( proUD != NULL)
    { // UD fun/pro
      envSize=proUD->var.size();
      keySize=proUD->key.size();
    }
  else
    { // Lib fun/pro
      keySize=pro->key.size();
      if( pro->nPar >= 0)
	envSize=pro->nPar+pro->key.size();
      else
	{
	envSize=keySize;
	// performance optimization
	env.reserve(envSize+5);
	}
    }

  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by value (self must not be changed)
  env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}

// for obj_new, obj_destroy, call_procedure and call_function
EnvT::EnvT( EnvT* pEnv, DSub* newPro, BaseGDL** self):
  interpreter( pEnv->interpreter), env(), pro(newPro), 
  ioError(NULL), onError( -1), catchVar(NULL), catchNode(NULL),
  obj( (self != NULL)),
  extra(this), callingNode( pEnv->callingNode),
  lFun( false),
  nJump( 0),
  lastJump( -1)
{
  DSubUD* proUD=dynamic_cast<DSubUD*>(pro); // ???
  SizeT envSize;
  SizeT keySize;
  if( proUD != NULL)
    { // UD fun/pro
      envSize=proUD->var.size();
      keySize=proUD->key.size();
    }
  else
    { // Lib fun/pro
      keySize=pro->key.size();
      if( pro->nPar >= 0)
	envSize=pro->nPar+pro->key.size();
      else
	{
	envSize=keySize;
	// performance optimization
	//env.reserve(envSize+5);
	}
    }
  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by reference (self must not be deleted)
  if( self != NULL)
    env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}

// these two functions should be inlined
SizeT EnvT::NewObjHeap( SizeT n, DStructGDL* v) 
{
  return interpreter->NewObjHeap(n,v);
}
SizeT EnvT::NewHeap( SizeT n, BaseGDL* v) 
{ 
  return interpreter->NewHeap(n,v);
}
void EnvT::FreeObjHeap( DObj id)
{
  interpreter->FreeObjHeap( id);
}
void EnvT::FreeHeap( DPtrGDL* p)
{
  interpreter->FreeHeap(p);
}
DStructGDL* EnvT::GetObjHeap( DObj ID)
{
  return interpreter->GetObjHeap( ID);
}
BaseGDL* EnvT::GetHeap( DPtr ID)
{
  return interpreter->GetHeap( ID);
}

// returns name of BaseGDL*
const string EnvT::GetString( BaseGDL*& p)
{
  DSubUD* subUD=dynamic_cast<DSubUD*>(pro);
  
  SizeT nVar=env.size();
  for( SizeT ix=0; ix<nVar; ix++)
    {
      if( (env.Env(ix) != NULL && p != NULL && *env.Env(ix) == p) ||
	  (&env[ ix] == &p) || 
	  (p != NULL && env.Loc( ix) == p) 
	  )
	{
	  if( subUD != NULL) return subUD->GetVarName(ix);
	  if( ix < pro->key.size()) return "KEYWORD_"+i2s(ix);
	  return "PAR_VAR_"+i2s(ix - pro->key.size());
	}
    }
  
  // search system variables
  // note: system variables are never passed by reference
  // ie. always a copy is passed.
  // therefore the help function never returns the sys var's name here
//   DVar* sysVar=FindInVarList( sysVarList, p);
//   if( sysVar != NULL) return sysVar->Name();

  // search common blocks
  if( subUD != NULL)
    {
      string varName;
      if( subUD->GetCommonVarName( p, varName)) return varName;
    }

  return string("<Expression>");
}

void EnvT::SetKeyword( const string& k, BaseGDL* const val) // value
{
  int varIx=GetKeywordIx( k);

  // -4 means ignore (warn keyword)
  if( varIx == -4) return;

  // -2 means _EXTRA keyword
  // -3 means _STRICT_EXTRA keyword
  if( varIx <= -2)
    {
      extra.Set(val);
      extra.SetStrict( varIx == -3);
      return;
    }

  // -1 means an extra (additional) keyword
  if( varIx == -1)
    {
      extra.Add(k,val);
      return;
    }

  env.Set( varIx,val);
}

void EnvT::SetKeyword( const string& k, BaseGDL** const val) // reference
{
  int varIx=GetKeywordIx( k);

  // -4 means ignore (warn keyword)
  if( varIx == -4) return;

  // -2 means _EXTRA keyword
  // -3 means _STRICT_EXTRA keyword
  if( varIx <= -2)
    {
      extra.Set(val);
      extra.SetStrict( varIx == -3);
      return;
    }

  // -1 means an extra (additional) keyword
  if( varIx == -1)
    {
      extra.Add(k,val);
      return;
    }

  env.Set( varIx,val);
}

// called after parameter definition
void EnvT::Extra()
{
  extra.Resolve();
}

EnvT* EnvT::Caller()
{
  EnvStackT& callStack=interpreter->CallStack();

  if( callStack.size() <= 1) return NULL;

  if( callStack.back() != this) return callStack.back();

  return callStack[ callStack.size()-2];
}

// used by obj_new (basic_fun.cpp)
// and obj_destroy (basic_pro.cpp)
void EnvT::PushNewEnv(  DSub* newPro, SizeT skipP, BaseGDL** newObj)
{
  EnvT* newEnv= new EnvT( this, newPro, newObj);

  // pass the parameters, skip the first 'skipP'
  SizeT nParam = NParam();
  for( SizeT p=skipP; p<nParam; p++)
    {
      newEnv->SetNextPar( &GetPar( p)); // pass as global
    }

  interpreter->CallStack().push_back( newEnv); 

  // _REF_EXTRA is set to the keyword string array
  newEnv->extra.Set( &env[0]);

  newEnv->extra.Resolve();
}

void EnvT::AssureGlobalPar( SizeT pIx)
{
  SizeT ix= pIx + pro->key.size();
  AssureGlobalKW( ix);
}
  
void EnvT::AssureGlobalKW( SizeT ix)
{
  if( env.Env( ix) == NULL)
    if( env.Loc( ix) != NULL)
      Throw( "Attempt to store into an expression.");
    else
      Throw( "Parameter must be a named variable.");
}

DStructGDL* EnvT::GetObjectPar( SizeT pIx)
{
  BaseGDL* p1= GetParDefined( pIx);
  
  DObjGDL* oRef = dynamic_cast<DObjGDL*>(p1);
  if( oRef == NULL)
    {
      Throw( "Parameter must be an object reference"
	     " in this context: "+
	     GetParString(pIx));
    }
  else
    {
      DObj objIx;
      if( !oRef->Scalar( objIx))
	Throw( "Parameter must be a scalar in this context: "+
	       GetParString(pIx));
      if( objIx == 0)
	Throw( "Unable to invoke method"
	       " on NULL object reference: "+GetParString(pIx));
      
      try {
	return GetObjHeap( objIx);
      }
      catch ( GDLInterpreter::HeapException)
	{
	  Throw( "Object not valid: "+GetParString(pIx));
	}
    }
}

// for exclusive use by lib::catch_pro
void EnvT::Catch()
  {
    EnvT* caller = Caller();
    if( caller == NULL) return;
    SizeT nParam = NParam();
    if( nParam == 0)
      {
	if( KeywordSet( 0)) // CANCEL
	  {
	  caller->catchVar = NULL;
	  caller->catchNode = NULL;
	  }
	return;
      }
    if( !GlobalPar( 0))
      throw GDLException( callingNode,
			  "CATCH: Expression must be named variable "
			  "in this context.");
    caller->catchNode = callingNode->getNextSibling();
    caller->catchVar = &GetPar( 0);
    delete *caller->catchVar;
    *caller->catchVar = new DLongGDL( 0);
  }

// for exclusive use by lib::on_error
void EnvT::OnError()
{
  SizeT nParam = NParam();
  DLong onE = 0;
  if( nParam > 0)
    AssureLongScalarPar( 0, onE);
  if( onE < 0 || onE > 3)
    throw GDLException( callingNode,
			"ON_ERROR: Value out of allowed range.");
  EnvT* caller = Caller();
  if( caller == NULL) return;
  caller->onError = onE;
}

int EnvT::KeywordIx( const std::string& k)
{
  //  cout << pro->ObjectName() << "  Key: " << k << endl;
  assert( pro != NULL);
  assert( pro->FindKey( k) != -1);
  return pro->FindKey( k);
}

const string EnvT::GetString( SizeT ix)
  {
    const string unnamed("<INTERNAL_VAR>");
    DSubUD* subUD=dynamic_cast<DSubUD*>(pro);
    if( subUD == NULL) 
      { // internal subroutine
	DLib* subLib=dynamic_cast<DLib*>(pro);
	if( subLib != NULL)
	  {
	    EnvT* caller = Caller();
	    if( caller != NULL) return caller->GetString( env[ ix]);
	  }
	return unnamed;
      }
    // UD subroutine
    return subUD->GetVarName( ix);
  }

BaseGDL*& EnvT::GetParDefined(SizeT i)
{
  SizeT ix = i + pro->key.size();
  if( ix >= env.size() || env[ ix] == NULL) 
    throw GDLException( callingNode, pro->ObjectName()+
			": Variable is undefined: "+
			GetString( ix));
  return env[ ix];
}

BaseGDL*& EnvT::GetParGlobal(SizeT pIx)
{
  AssureGlobalPar( pIx);
  return GetPar( pIx);
}

  // get i'th parameter, subName is used for error reporting
  // throws if not present (ie. not global)
  // paramter might be NULL (but &paramter is a valid BaseGDL** to store into) 
// BaseGDL*& EnvT::GetParPresent(SizeT i, const std::string& subName = "")
// {
//   SizeT ix = i + pro->key.size();
//   if( ix >= env.size() || env.Env( ix) == NULL) 
//     if( subName != "")
//       throw GDLException( callingNode, subName+": Paramter must be a "
// 			  "named variable in this context: "+
// 			  GetString( ix));
//     else
//       throw GDLException( callingNode, "Paramter must be a "
// 			  "named variable in this context: "+
// 			  GetString( ix));
//   return env[ ix];
// }

SizeT EnvT::NParam( SizeT minPar)
{
  assert( pro != NULL);

  SizeT nPar = parIx - pro->key.size();
  
  if( nPar < minPar)
    throw GDLException( callingNode, pro->ObjectName()+
			": Incorrect number of arguments.");
  return nPar;
}

void EnvT::SetNextPar( BaseGDL* const nextP) // by value (reset loc)
{
  if( pro->nPar >= 0)
    {
      if( static_cast<int>(parIx - pro->key.size()) >= pro->nPar)
	{
	  throw GDLException(callingNode,
			     pro->Name()+": Incorrect number of arguments.");
	}
    }
  else
    { // variable number of parameters (only lib functions)
      AddEnv();
    }
  env.Set(parIx++,nextP);
}

void EnvT::SetNextPar( BaseGDL** const nextP) // by reference (reset env)
{
  if( pro->nPar >= 0)
    {
      if( static_cast<int>(parIx - pro->key.size()) >= pro->nPar)
	{
	  throw GDLException(callingNode,
			     pro->Name()+": Incorrect number of arguments.");
	}
    }
  else
    {  // variable number of parameters (only lib functions)
      AddEnv();
    }
  env.Set(parIx++,nextP);
}

// returns the keyword index, used for UD functions
int EnvT::GetKeywordIx( const std::string& k)
{
  String_abbref_eq strAbbrefEq_k(k);

  // if there are no keywords, even _EXTRA isn't allowed
  if( pro->key.size() == 0)
    {
      if( pro->warnKey.size() == 0)
	throw GDLException( callingNode,
			    "Keyword parameters not allowed in call.");

      // look if warnKeyword
      IDList::iterator wf=std::find_if(pro->warnKey.begin(),
				       pro->warnKey.end(),
				       strAbbrefEq_k);
      if( wf == pro->warnKey.end()) 
	throw GDLException(callingNode,
			   "Keyword parameter "+k+" not allowed in call "
			   "to: "+pro->Name());

      Warning("Warning: Keyword parameter "+k+" not supported in call "
	      "to: "+pro->Name() + ". Ignored.");
      
      return -4;
    }
  
  // search keyword
  IDList::iterator f=std::find_if(pro->key.begin(),
				  pro->key.end(),
				  strAbbrefEq_k);
  if( f == pro->key.end()) 
    {
      // every routine (which accepts keywords), also accepts (_STRICT)_EXTRA
      if( strAbbrefEq_k("_EXTRA")) return -2;
      if( strAbbrefEq_k("_STRICT_EXTRA")) return -3;
      
      if( pro->Extra() == DSub::NONE)
	{
	  // look if warnKeyword
	  IDList::iterator wf=std::find_if(pro->warnKey.begin(),
					   pro->warnKey.end(),
					   strAbbrefEq_k);
	  if( wf == pro->warnKey.end()) 
	    throw GDLException(callingNode,
			       "Keyword parameter "+k+" not allowed in call "
			       "to: "+pro->Name());
	  
	  Warning("Warning: Keyword parameter "+k+" not supported in call "
		  "to: "+pro->Name() + ". Ignored.");
	  
	  return -4;
	}
      
      // extra keyword
      return -1;
    }
  // continue search (for ambiguity)
  IDList::iterator ff=std::find_if(f+1,
				   pro->key.end(),
				   strAbbrefEq_k);
  if( ff != pro->key.end())
    {
      throw GDLException(callingNode,"Ambiguous keyword abbreviation: "+k);
    }
    
  // every routine (which accepts keywords), also accepts (_STRICT)_EXTRA
  if( strAbbrefEq_k("_EXTRA")) return -2;
  if( strAbbrefEq_k("_STRICT_EXTRA")) return -3;
    
  SizeT varIx=std::distance(pro->key.begin(),f);

  // already set? -> Warning
  if( KeywordPresent(varIx)) // just a message in the original
    {
      Warning( "Duplicate keyword "+k+" in call to: "+pro->Name());
    }

  return varIx;
}
  
// for use within library functions
bool EnvT::KeywordSet( const std::string& kw)
{
  assert( pro != NULL);

  int ix=pro->FindKey( kw);
  if( ix == -1) return false;
  return KeywordSet( static_cast<SizeT>(ix));
}

bool EnvT::KeywordSet( SizeT ix)
{
  BaseGDL* keyword=env[ix];
  if( keyword == NULL) return false;
  if( !keyword->Scalar()) return false;
  return keyword->LogTrue();
}

// returns the ix'th parameter
BaseGDL*& EnvT::GetPar(SizeT i)
{
  static BaseGDL* null=NULL;
  SizeT ix= i + pro->key.size();
  if( ix >= env.size()) return null;
  return env[ ix];
}

void EnvT::AssureLongScalarPar( SizeT pIx, DLong& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DLongGDL* lp = static_cast<DLongGDL*>(p->Convert2( LONG, BaseGDL::COPY));
  auto_ptr<DLongGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    throw GDLException("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
// if keyword 'kw' is not set, 'scalar' is left unchanged
void EnvT::AssureLongScalarKWIfPresent( const std::string& kw, DLong& scalar)
{
  int ix = KeywordIx( kw);
  if( !KeywordPresent( ix)) return;
  AssureLongScalarKW( ix, scalar);
}
// converts keyword 'kw' if necessary and sets 'scalar' 
void EnvT::AssureLongScalarKW( const std::string& kw, DLong& scalar)
{
  AssureLongScalarKW( KeywordIx( kw), scalar);
}
void EnvT::AssureLongScalarKW( SizeT eIx, DLong& scalar)
{
  BaseGDL* p = GetKW( eIx);
  
  if( p == NULL)
    throw GDLException("Expression undefined: "+GetString(eIx));
  
  DLongGDL* lp= static_cast<DLongGDL*>(p->Convert2( LONG, BaseGDL::COPY));
  
  auto_ptr<DLongGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    throw GDLException("Expression must be a scalar in this context: "+
		       GetString(eIx));
}

void EnvT::AssureDoubleScalarPar( SizeT pIx, DDouble& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DDoubleGDL* lp = static_cast<DDoubleGDL*>(p->Convert2( DOUBLE, BaseGDL::COPY));
  auto_ptr<DDoubleGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    throw GDLException("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureDoubleScalarKWIfPresent( const std::string& kw, DDouble& scalar)
{
  int ix = KeywordIx( kw);
  if( !KeywordPresent( ix)) return;
  AssureDoubleScalarKW( ix, scalar);
}
void EnvT::AssureDoubleScalarKW( const std::string& kw, DDouble& scalar)
{
  AssureDoubleScalarKW( KeywordIx( kw), scalar);
}
void EnvT::AssureDoubleScalarKW( SizeT eIx, DDouble& scalar)
{
  BaseGDL* p = GetKW( eIx);
  
  if( p == NULL)
    throw GDLException("Expression undefined: "+GetString(eIx));
  
  DDoubleGDL* lp= static_cast<DDoubleGDL*>(p->Convert2( DOUBLE, BaseGDL::COPY));
  
  auto_ptr<DDoubleGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    throw GDLException("Expression must be a scalar in this context: "+
		       GetString(eIx));
}


void EnvT::AssureFloatScalarPar( SizeT pIx, DFloat& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DFloatGDL* lp = static_cast<DFloatGDL*>(p->Convert2( FLOAT, BaseGDL::COPY));
  auto_ptr<DFloatGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    throw GDLException("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureFloatScalarKWIfPresent( const std::string& kw, DFloat& scalar)
{
  int ix = KeywordIx( kw);
  if( !KeywordPresent( ix)) return;
  AssureFloatScalarKW( ix, scalar);
}
void EnvT::AssureFloatScalarKW( const std::string& kw, DFloat& scalar)
{
  AssureFloatScalarKW( KeywordIx( kw), scalar);
}
void EnvT::AssureFloatScalarKW( SizeT eIx, DFloat& scalar)
{
  BaseGDL* p = GetKW( eIx);
  
  if( p == NULL)
    throw GDLException("Expression undefined: "+GetString(eIx));
  
  DFloatGDL* lp= static_cast<DFloatGDL*>(p->Convert2( FLOAT, BaseGDL::COPY));
  
  auto_ptr<DFloatGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    throw GDLException("Expression must be a scalar in this context: "+
		       GetString(eIx));
}


void EnvT::AssureStringScalarPar( SizeT pIx, DString& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DStringGDL* lp = static_cast<DStringGDL*>(p->Convert2( STRING, BaseGDL::COPY));
  auto_ptr<DStringGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    throw GDLException("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureStringScalarKWIfPresent( const std::string& kw, DString& scalar)
{
  int ix = KeywordIx( kw);
  if( !KeywordPresent( ix)) return;
  AssureStringScalarKW( ix, scalar);
}
void EnvT::AssureStringScalarKW( const std::string& kw, DString& scalar)
{
  AssureStringScalarKW( KeywordIx( kw), scalar);
}
void EnvT::AssureStringScalarKW( SizeT eIx, DString& scalar)
{
  BaseGDL* p = GetKW( eIx);
  if( p == NULL)
    throw GDLException("Expression undefined: "+GetString(eIx));
  
  DStringGDL* lp= static_cast<DStringGDL*>(p->Convert2( STRING, BaseGDL::COPY));
  auto_ptr<DStringGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    throw GDLException("Expression must be a scalar in this context: "+
		       GetString(eIx));
}

void EnvT::SetKW( SizeT ix, BaseGDL* newVal)
{
  // can't use Guard here as data has to be released
  auto_ptr<BaseGDL> guard( newVal);
  AssureGlobalKW( ix);
  delete GetKW( ix);
  GetKW( ix) = guard.release();
}
void EnvT::SetPar( SizeT ix, BaseGDL* newVal)
{
  // can't use Guard here as data has to be released
  auto_ptr<BaseGDL> guard( newVal);
  AssureGlobalPar( ix);
  delete GetPar( ix);
  GetPar( ix) = guard.release();
}

bool EnvT::Contains( BaseGDL* p) const 
{ 
  if( env.Contains( p)) return true;
  return (static_cast<DSubUD*>(pro)->GetCommonVarPtr( p) != NULL);
}

BaseGDL** EnvT::GetPtrTo( BaseGDL* p) 
{ 
  BaseGDL** pp = env.GetPtrTo( p);
  if( pp != NULL) return pp;
  return static_cast<DSubUD*>(pro)->GetCommonVarPtr( p);
}
