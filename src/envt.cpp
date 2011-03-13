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

#include "includefirst.hpp"

#include <iomanip>

#include "objects.hpp"
#include "dinterpreter.hpp"
#include "envt.hpp"
#include "basic_pro.hpp"

#include <assert.h> // always as last

using namespace std;

// instance of static data
DInterpreter* EnvBaseT::interpreter;

EnvBaseT::ContainerT EnvBaseT::toDestroy;

EnvBaseT::EnvBaseT( ProgNodeP cN, DSub* pro_): 
  env(), 
  pro(pro_),
  extra(NULL),
  newEnv(NULL), 
  callingNode( cN),
  lineNumber( 0),
  obj(false)
, toDestroyInitialIndex( toDestroy.size())
{}

EnvUDT::EnvUDT( ProgNodeP cN, DSub* pro_, bool lF): 
  EnvBaseT( cN, pro_),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  lFun( lF),
  nJump( 0),
  lastJump( -1)
{
  DSubUD* proUD=static_cast<DSubUD*>(pro);

  forLoopInfo.resize( proUD->NForLoops());
  
  SizeT envSize;
  SizeT keySize;

  envSize=proUD->var.size();
  keySize=proUD->key.size();

  env.resize(envSize);
  parIx=keySize; // set to first parameter
}

EnvT::EnvT( ProgNodeP cN, DSub* pro_):
  EnvBaseT( cN, pro_)
{
  SizeT envSize;
  SizeT keySize;
  keySize=pro->key.size();
  if( pro->nPar > 0)
    envSize=pro->nPar+keySize;
  else
    {
      envSize=keySize;
      // performance optimization
      //env.reserve(envSize+5);
    }
  env.resize(envSize);
  parIx=keySize; // set to first parameter
}

// member pro
EnvUDT::EnvUDT( ProgNodeP cN, BaseGDL* self, 
		const string& parent): 
  EnvBaseT( cN, NULL),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  lFun( false),
  nJump( 0),
  lastJump( -1)
{
  obj = true;
  
  DStructGDL* oStructGDL = interpreter->ObjectStruct( self, cN);

  const string& mp = cN->getText();

  DStructDesc* desc=oStructGDL->Desc();

  if( parent != "")
    {
      pro=desc->GetPro( mp, parent);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   parent+"::"+mp,true,false);
    }
  else
    {
      pro=desc->GetPro( mp);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   desc->Name()+"::"+mp,true,false);
    }

  DSubUD* proUD=static_cast<DSubUD*>(pro);

  forLoopInfo.resize( proUD->NForLoops());

  SizeT envSize;
  SizeT keySize;
  envSize=proUD->var.size();
  keySize=proUD->key.size();

  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by value (self must not be changed)
  env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}

// member fun
EnvUDT::EnvUDT( BaseGDL* self, //DStructGDL* oStructGDL,  
		ProgNodeP cN, const string& parent, bool lF): 
  EnvBaseT( cN, NULL),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  lFun( lF),
  nJump( 0),
  lastJump( -1)
{
  obj = true;

  DStructGDL* oStructGDL = interpreter->ObjectStruct( self, cN);

  const string& mp = cN->getText();

  DStructDesc* desc=oStructGDL->Desc();
  
  if( parent != "")
    {
      pro=desc->GetFun( mp, parent);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   parent+"::"+mp,true,false);
    }
  else
    {
      pro=desc->GetFun( mp);
      
      if( pro == NULL)
	throw GDLException(cN,"Attempt to call undefined method: "+
			   desc->Name()+"::"+mp,true,false);
    }

  DSubUD* proUD=static_cast<DSubUD*>(pro);

  forLoopInfo.resize( proUD->NForLoops());

  SizeT envSize;
  SizeT keySize;
  envSize=proUD->var.size();
  keySize=proUD->key.size();

  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by value (self must not be changed)
  env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}

// for obj_new, obj_destroy, call_procedure and call_function
EnvT::EnvT( EnvT* pEnv, DSub* newPro, BaseGDL** self):
  EnvBaseT( pEnv->callingNode, newPro)
{
  obj = (self != NULL);

  SizeT envSize;
  SizeT keySize;
  keySize=pro->key.size();
  if( pro->nPar >= 0)
    envSize=pro->nPar+pro->key.size();
  else
    {
      envSize=keySize;
      // performance optimization
      //env.reserve(envSize+5);
    }
  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by reference (self must not be deleted)
  if( self != NULL)
    env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}
EnvUDT::EnvUDT( EnvBaseT* pEnv, DSub* newPro, BaseGDL** self):
  EnvBaseT( pEnv->CallingNode(), newPro),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  lFun( false),
  nJump( 0),
  lastJump( -1)
{
  obj = (self != NULL);

  DSubUD* proUD=static_cast<DSubUD*>(pro);
  
  forLoopInfo.resize( proUD->NForLoops());

  SizeT envSize;
  SizeT keySize;
  envSize=proUD->var.size();
  keySize=proUD->key.size();
  env.resize(envSize);
  parIx=keySize; // set to first parameter
  // pass by reference (self must not be deleted)
  if( self != NULL)
    env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}


void EnvBaseT::AddStruct( DPtrListT& ptrAccessible,
			  DPtrListT& objAccessible, DStructGDL* stru)
{
  if( stru == NULL) return;

  SizeT nEl = stru->N_Elements();

  const DStructDesc* desc = stru->Desc();

  SizeT nTags = desc->NTags();
  for( SizeT t=0; t<nTags; ++t)
    {
      if( (*desc)[ t]->Type() == PTR)
	{
	  for( SizeT e = 0; e<nEl; ++e)
	    {
	      DPtrGDL* ptr = static_cast< DPtrGDL*>( stru->GetTag( t, e));
	      AddPtr( ptrAccessible, objAccessible, ptr);
	    }
	}
      else if( (*desc)[ t]->Type() == STRUCT)
	{
	  for( SizeT e = 0; e<nEl; ++e)
	    {
	      DStructGDL* ptr = static_cast< DStructGDL*>( stru->GetTag( t, e));
	      AddStruct( ptrAccessible, objAccessible, ptr);
	    }
	}
     else if( (*desc)[ t]->Type() == OBJECT)
	{
	  for( SizeT e = 0; e<nEl; ++e)
	    {
	      DObjGDL* obj = static_cast< DObjGDL*>( stru->GetTag( t, e));
	      AddObj( ptrAccessible, objAccessible, obj);
	    }
	}

    }
}
void EnvBaseT::AddPtr( DPtrListT& ptrAccessible, DPtrListT& objAccessible,
		       DPtrGDL* ptr)
{
  if( ptr == NULL) return;

  SizeT nEl = ptr->N_Elements();
  for( SizeT e = 0; e<nEl; ++e)
    {
      DPtr p = (*ptr)[ e];
      if( p != 0 && interpreter->PtrValid( p))
	{
	  if( ptrAccessible.find( p) == ptrAccessible.end())
	    {
	      ptrAccessible.insert( p);
	      Add( ptrAccessible, objAccessible, interpreter->GetHeap( p));
	    }
	}
    }
}
void EnvBaseT::AddObj( DPtrListT& ptrAccessible, DPtrListT& objAccessible,
		       DObjGDL* ptr)
{
  if( ptr == NULL) return;

  SizeT nEl = ptr->N_Elements();
  for( SizeT e = 0; e<nEl; ++e)
    {
      DObj p = (*ptr)[ e];
      if( p != 0 && interpreter->ObjValid( p))
	{
	  if( objAccessible.find( p) == objAccessible.end())
	    {
	      objAccessible.insert( p);
	      AddStruct( ptrAccessible, objAccessible, 
			 interpreter->GetObjHeap( p));
	    }
	}
    }
}
void EnvBaseT::Add( DPtrListT& ptrAccessible, DPtrListT& objAccessible,
		    BaseGDL* p)
{
  DPtrGDL* ptr = dynamic_cast< DPtrGDL*>( p);
  AddPtr( ptrAccessible,  objAccessible, ptr);
  DStructGDL* stru = dynamic_cast< DStructGDL*>( p);
  AddStruct( ptrAccessible, objAccessible, stru);
  DObjGDL* obj = dynamic_cast< DObjGDL*>( p);
  AddObj( ptrAccessible, objAccessible, obj);
}
void EnvBaseT::AddEnv( DPtrListT& ptrAccessible, DPtrListT& objAccessible)
{
  for( SizeT e=0; e<env.size(); ++e)
    {
      Add( ptrAccessible, objAccessible, env[ e]);
    }
}
void EnvT::HeapGC( bool doPtr, bool doObj, bool verbose)
{
  // within CLEANUP method HEAP_GC could be called again
  // within CLEANUP common block or global variables may be freed
  // thus HEAP_GC has to be called again if called (but only once)
  static SizeT inProgress = 0;
  if( inProgress > 0) 
    {
      inProgress = 2;
      return;
    }

 startGC:
  inProgress = 1;

  try {
    DPtrListT ptrAccessible;
    DPtrListT objAccessible;
  
    // search common blocks
    for( CommonListT::iterator c = commonList.begin();
	 c != commonList.end(); ++c)
      {
	DCommon* common = *c;
	SizeT nVar = common->NVar();
	for( SizeT v = 0; v < nVar; ++v)
	  {
	    DVar* var = common->Var( v);
	    if( var != NULL)
	      {
			Add( ptrAccessible, objAccessible, var->Data());
	      }
	  }
      }

    SizeT nVar = sysVarList.size();
    for( SizeT v=0; v<nVar; ++v)
      {
	DVar* var = sysVarList[ v];
	if( var != NULL)
	  {
	    Add( ptrAccessible, objAccessible, var->Data());
	  }
      }

    EnvStackT& cS=interpreter->CallStack();
    for( EnvStackT::reverse_iterator r = cS.rbegin(); r != cS.rend(); ++r) 
      {
	(*r)->AddEnv( ptrAccessible, objAccessible);
      }

    // do OBJ first as the cleanup might need the PTR be valid
    if( doObj)
      {
	std::vector<DObj>* heap = interpreter->GetAllObjHeapSTL();
	auto_ptr< std::vector<DObj> > heap_guard( heap);
	if( heap->size() > 0 && (*heap)[0] != 0)
	  {
	    SizeT nH = heap->size();//N_Elements();
	    for( SizeT h=0; h<nH; ++h)
	      {
		DObj p = (*heap)[ h];
		if( interpreter->ObjValid( p))
		  if( objAccessible.find( p) == objAccessible.end())
		    {
		      if( verbose)
			{
			  BaseGDL* hV = GetObjHeap( p);		  
			  lib::help_item( cout, 
					  hV, DString( "<ObjHeapVar")+
					  i2s(p)+">",
					  false);
			}
		      ObjCleanup( p);
		    }
		// 	    else
		// 	      objAccessible.erase( p);
	      }
	  }
      }
    if( doPtr)
      {
	std::vector<DPtr>* heap = interpreter->GetAllHeapSTL();
	auto_ptr< std::vector<DPtr> > heap_guard( heap);
	if( heap->size() > 0 && (*heap)[0] != 0)
	  {
	    SizeT nH = heap->size();
	    for( SizeT h=0; h<nH; ++h)
	      {
		DPtr p = (*heap)[ h];
		if( interpreter->PtrValid( p))
		  if( ptrAccessible.find( p) == ptrAccessible.end())
		    {
		      if( verbose)
			{
			  BaseGDL* hV = GetHeap( p);
			  lib::help_item( cout, 
					  hV, DString( "<PtrHeapVar")+
					  i2s(p)+">",
					  false);
			}
		      interpreter->FreeHeap( p);
		    }
		// 	    else
		// 	      ptrAccessible.erase( p);
	      }
	  }
      }
  }
  catch( ...)
    {
      // make sure HEAP_GC stays not disabled in case of unhandled error
      inProgress = 0;
      throw;
    }

  if( inProgress == 2)
    {
      inProgress = 1;
      goto startGC;
    }
  inProgress = 0;
}


set< DObj> EnvBaseT::inProgress;

// for CLEANUP calls due to reference counting
// note: refcount is already zero for actID
void EnvBaseT::ObjCleanup( DObj actID)
{
  if( actID != 0 && (inProgress.find( actID) == inProgress.end()))
    {
      DStructGDL* actObj;
      try{
// 		actObj=GetObjHeap( actID);
 		GDLInterpreter::ObjHeapT::iterator it;
		actObj=GDLInterpreter::GetObjHeap( actID, it);
      }
      catch( GDLInterpreter::HeapException){
		actObj=NULL;
      }
	    
    if( actObj != NULL)
		{
		try{
				// call CLEANUP function
				DPro* objCLEANUP= actObj->Desc()->GetPro( "CLEANUP");
			
				if( objCLEANUP != NULL)
				{
					BaseGDL* actObjGDL = new DObjGDL( actID);
					auto_ptr<BaseGDL> actObjGDL_guard( actObjGDL);
					GDLInterpreter::IncRefObj( actID); // set refcount to 1
				
					PushNewEmptyEnvUD( objCLEANUP, &actObjGDL);
				
					inProgress.insert( actID);
				
					interpreter->call_pro( objCLEANUP->GetTree());
				
					inProgress.erase( actID);

					EnvBaseT* callStackBack =  interpreter->CallStack().back();
					interpreter->CallStack().pop_back();
					delete callStackBack;

					FreeObjHeap( actID); // make sure actObj is freed
					// actObjGDL goes out of scope -> refcount is (would be) decreased
				}
			}
		catch( ...)
			{
				FreeObjHeap( actID); // make sure actObj is freed
				throw; // rethrow
			}		
		}
	else		
		FreeObjHeap( actID); // the actual freeing
    }
}



void EnvT::ObjCleanup( DObj actID)
{
  if( actID != 0 && (inProgress.find( actID) == inProgress.end()))
    {
      DStructGDL* actObj;
      try{
// 		actObj=GetObjHeap( actID);
 		GDLInterpreter::ObjHeapT::iterator it;
		actObj=GDLInterpreter::GetObjHeap( actID, it);
      }
      catch( GDLInterpreter::HeapException){
		actObj=NULL;
      }
	    
      if( actObj != NULL)
		{
			// call CLEANUP function
			DPro* objCLEANUP= actObj->Desc()->GetPro( "CLEANUP");
		
			if( objCLEANUP != NULL)
				{
				BaseGDL* actObjGDL = new DObjGDL( actID);
				auto_ptr<BaseGDL> actObjGDL_guard( actObjGDL);
				GDLInterpreter::IncRefObj( actID);
			
				PushNewEnvUD( objCLEANUP, 1, &actObjGDL);
			
				inProgress.insert( actID);
			
				interpreter->call_pro( objCLEANUP->GetTree());
			
				inProgress.erase( actID);

				delete interpreter->CallStack().back();
				interpreter->CallStack().pop_back();
			}
 	
		FreeObjHeap( actID); // the actual freeing
		}
    }
}



// these two functions should be inlined
SizeT EnvBaseT::NewObjHeap( SizeT n, DStructGDL* v) 
{
  return interpreter->NewObjHeap(n,v);
}
SizeT EnvBaseT::NewHeap( SizeT n, BaseGDL* v)
{ 
  return interpreter->NewHeap(n,v);
}
void EnvBaseT::FreeObjHeap( DObj id)
{
  interpreter->FreeObjHeap( id);
}
void EnvBaseT::FreeHeap( DPtrGDL* p)
{
  interpreter->FreeHeap(p);
}
// // DStructGDL* EnvT::GetObjHeap( DObj ID, ObjHeapT::iterator& it)
// // {
// //   return interpreter->GetObjHeap( ID, it);
// // }
DStructGDL* EnvBaseT::GetObjHeap( DObj ID)
{
  return interpreter->GetObjHeap( ID);
}
BaseGDL* EnvBaseT::GetHeap( DPtr ID)
{
  return interpreter->GetHeap( ID);
}

// returns name of BaseGDL*&
const string EnvBaseT::GetString( BaseGDL*& p, bool calledFromHELP)
{
  DSubUD* subUD=dynamic_cast<DSubUD*>(pro);
  
  SizeT nVar=env.size();
  const string Default = "<Expression>";
  string name = Default;
  for( SizeT ix=0; ix<nVar; ix++)
    {
      if( (env.Env(ix) != NULL && p != NULL && *env.Env(ix) == p) ||
	  (&env[ ix] == &p) || 
	  (p != NULL && env.Loc( ix) == p) 
	  )
	{
	  if( subUD != NULL) {return subUD->GetVarName(ix);break;}

	string callerName = Default;
 	if( this->Caller() != NULL)
 		callerName = this->Caller()->GetString( p, calledFromHELP);
 
	if( callerName.length() < Default.length() || callerName.substr(0,Default.length()) != Default)
		return callerName;

	  if( ix < pro->key.size()) {name="<KEY_"+i2s(ix)+">";break;}
	  name="<PAR_"+i2s(ix - pro->key.size())+">";
	  break;
	}
    }
  
  // search system variables
  // note: system variables are never passed by reference
  // ie. always a copy is passed.
  // therefore the help function never returns the sys var's name here
//   DVar* sysVar=FindInVarList( sysVarList, p);
//   if( sysVar != NULL) return sysVar->Name();

  // search common blocks
  if( name == Default && subUD != NULL)
    {
      string varName;
      if( subUD->GetCommonVarName( p, varName)) 
	return varName;
    }

    if( !p)
      {
        return  "<Undefined>";
      }

    if( !calledFromHELP)
	{
	ostringstream os;
	os << '<' << left;
	os.width(10);
	os << p->TypeStr() << right;
	
	// Data display
	if( p->Type() == STRUCT)
	{
/*		DStructGDL* s = static_cast<DStructGDL*>( p);
		os << "-> ";
		os << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());
		os << " ";*/
	}
	else if( p->Dim( 0) == 0)
	{
		os << "(";
		if (p->Type() == STRING)
		{
		// trim string larger than 45 characters
		DString dataString = (*static_cast<DStringGDL*>(p))[0];
		os << "'" << StrMid( dataString,0,45,0) << "'";
		if( dataString.length() > 45) os << "...";
		}
		else
		{
		p->ToStream( os);
		}
		os << ")";
	}
	
	// Dimension display
	if( p->Dim( 0) != 0) os << p->Dim();
	
	os << ">";
	
	name += " " + os.str();
// 	return os.str();
	}

  return name; //string("<Expression>");
}

// // returns name of BaseGDL*
// const string EnvBaseT::GetString( BaseGDL* p)
// {
//   DSubUD* subUD=dynamic_cast<DSubUD*>(pro);
//   
//   SizeT nVar=env.size();
//   for( SizeT ix=0; ix<nVar; ix++)
//     {
//       if( (env.Env(ix) != NULL && p != NULL && *env.Env(ix) == p) ||
// // 	  (&env[ ix] == &p) || 
// 	  (p != NULL && env.Loc( ix) == p) 
// 	  )
// 	{
// 	  if( subUD != NULL) return subUD->GetVarName(ix);
// 	  if( ix < pro->key.size()) return "KEYWORD_"+i2s(ix);
// 	  return "PAR_VAR_"+i2s(ix - pro->key.size());
// 	}
//     }
//   
//   // search system variables
//   // note: system variables are never passed by reference
//   // ie. always a copy is passed.
//   // therefore the help function never returns the sys var's name here
// //   DVar* sysVar=FindInVarList( sysVarList, p);
// //   if( sysVar != NULL) return sysVar->Name();
// 
//   // search common blocks
//   if( subUD != NULL)
//     {
//       string varName;
//       if( subUD->GetCommonVarName( p, varName)) return varName;
//     }
// 
//     if( !p)
//       {
//         return  "<Undefined>";
//       }
// 
// //     if( !calledFromHELP)
// 	{
// 	ostringstream os;
// 	os << '<' << left;
// 	os.width(10);
// 	os << p->TypeStr() << right;
// 	
// 	// Data display
// 	if( p->Type() == STRUCT)
// 	{
// 		DStructGDL* s = static_cast<DStructGDL*>( p);
// 		os << "-> ";
// 		os << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());
// 		os << " ";
// 	}
// 	else if( p->Dim( 0) == 0)
// 	{
// 		os << "(";
// 		if (p->Type() == STRING)
// 		{
// 		// trim string larger than 45 characters
// 		DString dataString = (*static_cast<DStringGDL*>(p))[0];
// 		os << "'" << StrMid( dataString,0,45,0) << "'";
// 		if( dataString.length() > 45) os << "...";
// 		}
// 		else
// 		{
// 		p->ToStream( os);
// 		}
// 		os << ")";
// 	}
// 	
// 	// Dimension display
// 	if( p->Dim( 0) != 0) os << p->Dim();
// 	
// 	os << ">";
// 	
// 	return os.str();
// 	}
// 
//   return string("<Expression>");
// }

void EnvBaseT::SetKeyword( const string& k, BaseGDL* const val) // value
{
  int varIx=GetKeywordIx( k);

  // -4 means ignore (warn keyword)
  if( varIx == -4) return;

  // -2 means _EXTRA keyword
  // -3 means _STRICT_EXTRA keyword
  if( varIx <= -2)
    {
      if( extra == NULL) extra = new ExtraT( this);
      extra->Set(val);
      extra->SetStrict( varIx == -3);
      return;
    }

  // -1 means an extra (additional) keyword
  if( varIx == -1)
    {
      if( extra == NULL) extra = new ExtraT( this);
      extra->Add(k,val);
      return;
    }

  env.Set( varIx,val);
}

void EnvBaseT::SetKeyword( const string& k, BaseGDL** const val) // reference
{
  int varIx=GetKeywordIx( k);

  // -4 means ignore (warn keyword)
  if( varIx == -4) return;

  // -2 means _EXTRA keyword
  // -3 means _STRICT_EXTRA keyword
  if( varIx <= -2)
    {
      if( extra == NULL) extra = new ExtraT( this);
      extra->Set(val);
      extra->SetStrict( varIx == -3);
      return;
    }

  // -1 means an extra (additional) keyword
  if( varIx == -1)
    {
      if( extra == NULL) extra = new ExtraT( this);
      extra->Add(k,val);
      return;
    }

  env.Set( varIx,val);
}

// called after parameter definition
void EnvBaseT::Extra()
{
  if( extra != NULL) extra->Resolve();
}

EnvBaseT* EnvBaseT::Caller()
{
  EnvStackT& callStack=interpreter->CallStack();

  if( callStack.size() <= 1) return NULL;

  if( callStack.back() != this) 
    return callStack.back();
//     return static_cast< EnvUDT*>( callStack.back());

  return callStack[ callStack.size()-2];
//   return static_cast< EnvUDT*>( callStack[ callStack.size()-2]);
}

// used by obj_new (basic_fun.cpp)
// and obj_destroy (basic_pro.cpp)
void EnvBaseT::PushNewEmptyEnvUD(  DSub* newPro, BaseGDL** newObj)
{
  EnvUDT* newEnv= new EnvUDT( this, newPro, newObj);

  // pass the parameters, skip the first 'skipP'
//   SizeT nParam = NParam();
//   for( SizeT p=skipP; p<nParam; p++)
//     {
//       newEnv->SetNextPar( &GetPar( p)); // pass as global
//     }

  interpreter->CallStack().push_back( newEnv); 

  // _REF_EXTRA is set to the keyword string array
//   newEnv->extra = new ExtraT( newEnv);
//   newEnv->extra->Set( &env[0]);
//   newEnv->extra->Resolve();
}

// used by obj_new (basic_fun.cpp)
// and obj_destroy (basic_pro.cpp)
void EnvT::PushNewEnvUD(  DSub* newPro, SizeT skipP, BaseGDL** newObj)
{
  EnvUDT* newEnv= new EnvUDT( this, newPro, newObj);

  // pass the parameters, skip the first 'skipP'
  SizeT nParam = NParam();
  for( SizeT p=skipP; p<nParam; p++)
    {
      newEnv->SetNextPar( &GetPar( p)); // pass as global
    }

  interpreter->CallStack().push_back( newEnv); 

  // _REF_EXTRA is set to the keyword string array
  newEnv->extra = new ExtraT( newEnv);
  newEnv->extra->Set( &env[0]);
  newEnv->extra->Resolve();
}
// used by obj_new (basic_fun.cpp)
// and obj_destroy (basic_pro.cpp)
// and call_function (basic_fun.cpp)
// and call_procedure (basic_pro.cpp)
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
  newEnv->extra = new ExtraT( newEnv);
  newEnv->extra->Set( &env[0]);
  newEnv->extra->Resolve();
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
    EnvUDT* caller = static_cast<EnvUDT*>(Caller());
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
      Throw( "Expression must be named variable "
			  "in this context: " + GetParString(0));
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
    Throw(	"Value out of allowed range: " + GetParString(0));
  EnvUDT* caller = static_cast<EnvUDT*>(Caller());
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

const string EnvBaseT::GetString( SizeT ix)
  {
    const string unnamed("<INTERNAL_VAR>");
    DSubUD* subUD=dynamic_cast<DSubUD*>(pro);
    if( subUD == NULL) 
      { // internal subroutine
	DLib* subLib=dynamic_cast<DLib*>(pro);
	if( subLib != NULL)
	  {
	    EnvBaseT* caller = Caller();
	    if( caller != NULL) return caller->GetString( env[ ix]);
	  }
	return unnamed;
      }
    // UD subroutine
    return subUD->GetVarName( ix);
  }

BaseGDL*& EnvBaseT::GetParDefined(SizeT i)
{
  SizeT ix = i + pro->key.size();

  //  cout << i << " -> " << ix << "  " << env.size() << "  env[ix] " << env[ix] << endl;
  if( ix >= env.size() || env[ ix] == NULL) 
    Throw("Variable is undefined: "+GetString( ix));
  return env[ ix];
}
BaseGDL*& EnvT::GetParDefined(SizeT i)
{
  return EnvBaseT::GetParDefined( i);
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

SizeT EnvBaseT::NParam( SizeT minPar)
{
  assert( pro != NULL);

  SizeT nPar = parIx - pro->key.size();
  
  if( nPar < minPar)
    Throw(	"Incorrect number of arguments.");
  return nPar;
}
SizeT EnvT::NParam( SizeT minPar)
{
  return EnvBaseT::NParam( minPar);
}

void EnvBaseT::SetNextPar( BaseGDL* const nextP) // by value (reset loc)
{
  if( pro->nPar >= 0)
    {
      if( static_cast<int>(parIx - pro->key.size()) >= pro->nPar)
	{
	  throw GDLException(callingNode,
			     pro->Name()+": Incorrect number of arguments.",false,false);
	}
    }
  else
    { // variable number of parameters (only lib functions)
      AddEnv();
    }
  env.Set(parIx++,nextP);
}

void EnvBaseT::SetNextPar( BaseGDL** const nextP) // by reference (reset env)
{
  if( pro->nPar >= 0)
    {
      if( static_cast<int>(parIx - pro->key.size()) >= pro->nPar)
	{
	  throw GDLException(callingNode,
			     pro->Name()+": Incorrect number of arguments.",false,false);
	}
    }
  else
    {  // variable number of parameters (only lib functions)
      AddEnv();
    }
  env.Set(parIx++,nextP);

  //  cout << "set: " << parIx-1 << " to "; (*nextP)->ToStream( cout); 
  //  cout << "  " << nextP << endl;
}

// returns the keyword index, used for UD functions
int EnvBaseT::GetKeywordIx( const std::string& k)
{
  String_abbref_eq strAbbrefEq_k(k);

  // if there are no keywords, even _EXTRA isn't allowed
  if( pro->key.size() == 0)
    {
      if( pro->warnKey.size() == 0)
	Throw( "Keyword parameters not allowed in call.");

      // look if warnKeyword
      IDList::iterator wf=std::find_if(pro->warnKey.begin(),
				       pro->warnKey.end(),
				       strAbbrefEq_k);
      if( wf == pro->warnKey.end()) 
	Throw(  "Keyword parameter "+k+" not allowed in call "
			   "to: "+pro->Name());
// 	throw GDLException(callingNode,
// 			   "Keyword parameter "+k+" not allowed in call "
// 			   "to: "+pro->Name());

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
	    Throw( "Keyword parameter "+k+" not allowed in call "
			       "to: "+pro->Name());
/*	    throw GDLException(callingNode,
			       "Keyword parameter "+k+" not allowed in call "
			       "to: "+pro->Name());*/
	  
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
      Throw("Ambiguous keyword abbreviation: "+k);
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
  assert( null == NULL);
  SizeT ix= i + pro->key.size();
  if( ix >= env.size()) 
    {
//       Warning( "EnvT::GetPar(): Index out of env size ("+i2s(env.size())+"): " + i2s(i) +" (+ "+i2s(pro->key.size())+" KW)");
      return null;
    }
  return env[ ix];
}

void EnvBaseT::AssureLongScalarPar( SizeT pIx, DLong64& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DLong64GDL* lp = static_cast<DLong64GDL*>(p->Convert2( LONG64, BaseGDL::COPY));
  auto_ptr<DLong64GDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvBaseT::AssureLongScalarPar( SizeT pIx, DLong& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DLongGDL* lp = static_cast<DLongGDL*>(p->Convert2( LONG, BaseGDL::COPY));
  auto_ptr<DLongGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureLongScalarPar( SizeT pIx, DLong64& scalar)
{
  EnvBaseT::AssureLongScalarPar( pIx, scalar);
}
void EnvT::AssureLongScalarPar( SizeT pIx, DLong& scalar)
{
  EnvBaseT::AssureLongScalarPar( pIx, scalar);
}
// if keyword 'kw' is not set, 'scalar' is left unchanged
void EnvT::AssureLongScalarKWIfPresent( const std::string& kw, DLong& scalar)
{
  int ix = KeywordIx( kw);
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
  AssureLongScalarKW( ix, scalar);
}
void EnvT::AssureLongScalarKWIfPresent( SizeT ix, DLong& scalar)
{
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
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
    Throw("Expression undefined: "+GetString(eIx));
  
  DLongGDL* lp= static_cast<DLongGDL*>(p->Convert2( LONG, BaseGDL::COPY));
  
  auto_ptr<DLongGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar in this context: "+
		       GetString(eIx));
}

void EnvT::AssureDoubleScalarPar( SizeT pIx, DDouble& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DDoubleGDL* lp = static_cast<DDoubleGDL*>(p->Convert2( DOUBLE, BaseGDL::COPY));
  auto_ptr<DDoubleGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureDoubleScalarKWIfPresent( const std::string& kw, DDouble& scalar)
{
  int ix = KeywordIx( kw);
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
  AssureDoubleScalarKW( ix, scalar);
}
void EnvT::AssureDoubleScalarKWIfPresent( SizeT ix, DDouble& scalar)
{
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
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
    Throw("Expression undefined: "+GetString(eIx));
  
  DDoubleGDL* lp= static_cast<DDoubleGDL*>(p->Convert2( DOUBLE, BaseGDL::COPY));
  
  auto_ptr<DDoubleGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar in this context: "+
		       GetString(eIx));
}


void EnvT::AssureFloatScalarPar( SizeT pIx, DFloat& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DFloatGDL* lp = static_cast<DFloatGDL*>(p->Convert2( FLOAT, BaseGDL::COPY));
  auto_ptr<DFloatGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureFloatScalarKWIfPresent( const std::string& kw, DFloat& scalar)
{
  int ix = KeywordIx( kw);
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
  AssureFloatScalarKW( ix, scalar);
}
void EnvT::AssureFloatScalarKWIfPresent( SizeT ix, DFloat& scalar)
{
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
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
    Throw("Expression undefined: "+GetString(eIx));
  
  DFloatGDL* lp= static_cast<DFloatGDL*>(p->Convert2( FLOAT, BaseGDL::COPY));
  
  auto_ptr<DFloatGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar in this context: "+
		       GetString(eIx));
}


void EnvT::AssureStringScalarPar( SizeT pIx, DString& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DStringGDL* lp = static_cast<DStringGDL*>(p->Convert2( STRING, BaseGDL::COPY));
  auto_ptr<DStringGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar in this context: "+
		       GetParString(pIx));
}
void EnvT::AssureStringScalarKWIfPresent( const std::string& kw, DString& scalar)
{
  int ix = KeywordIx( kw);
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
  AssureStringScalarKW( ix, scalar);
}
void EnvT::AssureStringScalarKWIfPresent( SizeT ix, DString& scalar)
{
  if( env[ix] == NULL) return;
  //  if( !KeywordPresent( ix)) return;
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
    Throw("Expression undefined: "+GetString(eIx));
  
  DStringGDL* lp= static_cast<DStringGDL*>(p->Convert2( STRING, BaseGDL::COPY));
  auto_ptr<DStringGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar in this context: "+
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

bool EnvBaseT::Contains( BaseGDL* p) const 
{ 
  if( env.Contains( p)) return true;
  return (static_cast<DSubUD*>(pro)->GetCommonVarPtr( p) != NULL);
}

BaseGDL** EnvBaseT::GetPtrTo( BaseGDL* p) 
{ 
  BaseGDL** pp = env.GetPtrTo( p);
  if( pp != NULL) return pp;
  pp = static_cast<DSubUD*>(pro)->GetCommonVarPtr( p);
  if( pp != NULL) return pp;
  return GDLInterpreter::GetPtrToHeap( p);
}
