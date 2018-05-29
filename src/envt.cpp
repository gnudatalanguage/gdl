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

#include "envt.hpp"
#include "objects.hpp"
#include "dinterpreter.hpp"
#include "basic_pro.hpp"
#include "nullgdl.hpp"

#include <cassert> // always as last

using namespace std;

// instance of static data
DInterpreter* EnvBaseT::interpreter;

// EnvBaseT::ContainerT EnvBaseT::toDestroy;

// EnvT::new & delete 
vector< void*> EnvT::freeList;
const int multiAllocEnvT = 4;
void* EnvT::operator new( size_t bytes)
{
  assert( bytes == sizeof( EnvT));
  if( freeList.size() > 0)
    {
      void* res = freeList.back();
      freeList.pop_back();
      return res;	
    }
  //   cout << "*** Resize EnvT " << endl;
  const size_t newSize = multiAllocEnvT - 1;
  freeList.resize( newSize);
  char* res = static_cast< char*>( malloc( sizeof( EnvT) * multiAllocEnvT)); // one more than newSize
  for( size_t i=0; i<newSize; ++i)
    {
      freeList[ i] = res;
      res += sizeof( EnvT);
    } 
  // the one more
  return res;
}
void EnvT::operator delete( void *ptr)
{
  freeList.push_back( ptr);
}

// EnvUDT::new & delete 
// deque< void*> EnvUDT::freeList;
FreeListT EnvUDT::freeList;
const int multiAllocEnvUDT = 16;
void* EnvUDT::operator new( size_t bytes)
{
  assert( bytes == sizeof( EnvUDT));
  if( freeList.size() > 0)
    {
      return freeList.pop_back();  
      //     void* res = freeList.back();
      //     freeList.pop_back();
      //     return res;	
    }
  //   cout << "*** Resize EnvUDT " << endl;
  const size_t newSize = multiAllocEnvUDT - 1;

  static long callCount = 0;
  ++callCount;
  
  freeList.reserve( multiAllocEnvUDT * callCount);
  //   char* res = static_cast< char*>( malloc( sizeof( EnvUDT) * multiAllocEnvUDT)); // one more than newSize
  //   for( size_t i=0; i<newSize; ++i)
  //   {
  //     freeList[ i] = res;
  //     res += sizeof( EnvUDT);
  //   } 
  const size_t sizeOfType = sizeof( EnvUDT);
  char* res = static_cast< char*>( malloc( sizeOfType * multiAllocEnvUDT)); // one more than newSize
  
  res = freeList.Init( newSize, res, sizeOfType);
  // the one more
  return res;
}
void EnvUDT::operator delete( void *ptr)
{
  freeList.push_back( ptr);
}





EnvBaseT::EnvBaseT( ProgNodeP cN, DSub* pro_): 
  toDestroy()
  ,env()
  ,pro(pro_)
  ,callingNode( cN)
  ,lineNumber( 0)
  ,obj(false)
  ,extra(NULL)
  ,newEnvOff(NULL)
  ,ptrToReturnValue(NULL)
		  //, toDestroyInitialIndex( toDestroy.size())
{}

EnvUDT::EnvUDT( ProgNodeP cN, DSubUD* pro_, CallContext lF): 
  EnvBaseT( cN, pro_),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  callContext( lF),
  //   callContext( RFUNCTION),
  nJump( 0),
  lastJump( -1)
{
  DSubUD* proUD=static_cast<DSubUD*>(pro);

  forLoopInfo.InitSize( proUD->NForLoops());
  
  SizeT envSize;
  SizeT keySize;

  envSize=proUD->var.size();
  keySize=proUD->key.size();

  env.resize(envSize);
  parIx=keySize; // set to first parameter
}

EnvT::EnvT ( ProgNodeP cN, DSub* pro_):
  EnvBaseT ( cN, pro_)
{
  // 	SizeT envSize;
  // 	SizeT keySize;
  parIx=pro->key.size();
  if ( pro->nPar > 0 )
    {
      env.resize ( pro->nPar + parIx);
      // 		envSize=pro->nPar+parIx;
    }
  else
    {
      env.resize ( parIx);
      // 		envSize=parIx;
      // performance optimization
      //env.reserve(envSize+5);
    }
  // 	env.resize ( envSize);
  //   parIx=keySize; // set to first parameter
}

// member pro
EnvUDT::EnvUDT( ProgNodeP cN, BaseGDL* self, 
		const string& parent): 
  EnvBaseT( cN, NULL),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  callContext( RFUNCTION),
  nJump( 0),
  lastJump( -1)
{
  obj = true;

  DType selfType = self->Type();
  if( selfType != GDL_OBJ) 
    throw GDLException( cN, "Object reference type"
			" required in this context: "+interpreter->Name(self));
  
  DStructGDL* oStructGDL = interpreter->ObjectStruct( static_cast<DObjGDL*>(self), cN);

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

  forLoopInfo.InitSize( proUD->NForLoops());

  SizeT envSize;
  //   SizeT keySize;
  envSize=proUD->var.size();
  parIx=proUD->key.size();

  env.resize(envSize);
  //   parIx=keySize; // set to first parameter
  // pass by value (self must not be changed)
  env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}

// member fun
EnvUDT::EnvUDT( BaseGDL* self, ProgNodeP cN, const string& parent, CallContext lF): 
  EnvBaseT( cN, NULL),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  callContext( lF),
  nJump( 0),
  lastJump( -1)
{
  obj = true;

  DType selfType = self->Type();
  if( selfType != GDL_OBJ) 
    throw GDLException( cN, "Object reference type"
			" required in this context: "+interpreter->Name(self));
  
  DStructGDL* oStructGDL = interpreter->ObjectStruct( static_cast<DObjGDL*>(self), cN);

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

  forLoopInfo.InitSize( proUD->NForLoops());

  SizeT envSize=proUD->var.size();
  parIx=proUD->key.size();

  env.resize(envSize);
  //   parIx=keySize; // set to first parameter
  // pass by value (self must not be changed)
  env.Set( parIx++, self); //static_cast<BaseGDL*>(oStructGDL));
}



// for obj_new, obj_destroy, call_procedure and call_function
EnvT::EnvT( EnvT* pEnv, DSub* newPro, DObjGDL** self):
  EnvBaseT( pEnv->callingNode, newPro)
{
  obj = (self != NULL);

  SizeT envSize;
  parIx=pro->key.size();
  if( pro->nPar > 0)
    {
      envSize=pro->nPar+parIx;
    }
  else
    {
      envSize=parIx;
    }
  env.resize(envSize);
  //   parIx=keySize; // set to first parameter
  // pass by reference (self must not be deleted)
  if( self != NULL)
    env.Set( parIx++, (BaseGDL**)self); //static_cast<BaseGDL*>(oStructGDL));
}



//EnvUDT::EnvUDT( EnvBaseT* pEnv, DSub* newPro, BaseGDL** self):
EnvUDT::EnvUDT( ProgNodeP callingNode_, DSubUD* newPro, DObjGDL** self):
  //   EnvBaseT( pEnv->CallingNode(), newPro),
  EnvBaseT( callingNode_, newPro),
  ioError(NULL), 
  onError( -1), 
  catchVar(NULL), 
  catchNode(NULL), 
  callContext( RFUNCTION),
  nJump( 0),
  lastJump( -1)
{
  obj = (self != NULL);

  DSubUD* proUD= newPro; //static_cast<DSubUD*>(pro);
  
  forLoopInfo.InitSize( proUD->NForLoops());

  SizeT envSize;
  //   SizeT keySize;
  envSize=proUD->var.size();
  parIx=proUD->key.size();
  env.resize(envSize);
  //   parIx=keySize; // set to first parameter
  // pass by reference (self must not be deleted)
  if( self != NULL)
    env.Set( parIx++, (BaseGDL**)self); //static_cast<BaseGDL*>(oStructGDL));
}




void EnvBaseT::AddStruct( DPtrListT& ptrAccessible,
			  DPtrListT& objAccessible, DStructGDL* stru)
{
  if( stru == NULL) return;

  SizeT nEl = stru->N_Elements();

  const DStructDesc* desc = stru->Desc();
  
  // avoid recursion on LIST (for > 100000 list elements a segfault is generated otherwise)
  if( desc->IsParent("LIST"))
    {
      AddLIST(ptrAccessible, objAccessible, stru);
      return;
    }

  SizeT nTags = desc->NTags();
  for( SizeT t=0; t<nTags; ++t)
    {
      if( (*desc)[ t]->Type() == GDL_PTR)
	{
	  for( SizeT e = 0; e<nEl; ++e)
	    {
	      DPtrGDL* ptr = static_cast< DPtrGDL*>( stru->GetTag( t, e));
	      AddPtr( ptrAccessible, objAccessible, ptr);
	    }
	}
      else if( (*desc)[ t]->Type() == GDL_STRUCT)
	{
	  for( SizeT e = 0; e<nEl; ++e)
	    {
	      DStructGDL* ptr = static_cast< DStructGDL*>( stru->GetTag( t, e));
	      AddStruct( ptrAccessible, objAccessible, ptr);
	    }
	}
      else if( (*desc)[ t]->Type() == GDL_OBJ)
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

  SizeT nEl = ptr->Size();//  N_Elements();
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
  if( p == NULL)
    return;
  DType pType = p->Type();
  if( pType == GDL_PTR)
    AddPtr( ptrAccessible,  objAccessible, static_cast< DPtrGDL*>( p));
  else if( pType == GDL_STRUCT)
    AddStruct( ptrAccessible, objAccessible, static_cast< DStructGDL*>( p));
  else if( pType == GDL_OBJ)
    AddObj( ptrAccessible, objAccessible, static_cast< DObjGDL*>( p));
}
void EnvBaseT::AddEnv( DPtrListT& ptrAccessible, DPtrListT& objAccessible)
{
  for( SizeT e=0; e<env.size(); ++e)
    {
      Add( ptrAccessible, objAccessible, env[ e]);
    }
}
void EnvBaseT::AddToDestroy( DPtrListT& ptrAccessible, DPtrListT& objAccessible)
{
  for( SizeT i=0; i<toDestroy.size(); ++i)
    {
      Add( ptrAccessible, objAccessible, toDestroy[i]);
    }
}

typedef std::vector<DObj> VectorDObj;
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
    //     for( EnvStackT::reverse_iterator r = cS.rbegin(); r != cS.rend(); ++r) 
    for( long ix = cS.size()-1; ix >= 0; --ix) 
      {
	cS[ix]->AddEnv( ptrAccessible, objAccessible);
      }

    // add all data already set for destruction (not to be deleted now)  
    AddToDestroy( ptrAccessible, objAccessible);  

    // do OBJ first as the cleanup might need the GDL_PTR be valid
    if( doObj)
      {
	std::vector<DObj>* heap = interpreter->GetAllObjHeapSTL();
	Guard< std::vector<DObj> > heap_guard( heap);
	SizeT nH = heap->size();//N_Elements();
	if( nH > 0 && (*heap)[0] != 0)
	  {
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
	Guard< std::vector<DPtr> > heap_guard( heap);
	SizeT nH = heap->size();
	if( nH > 0 && (*heap)[0] != 0)
	  {
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

class InProgressGuard
{
private:
  DObj actID;
public:
  InProgressGuard( DObj id): actID( id) 
  {
    EnvBaseT::inProgress.insert( actID);    
  }
  ~InProgressGuard()
  {
    EnvBaseT::inProgress.erase( actID);
  }
};

// for CLEANUP calls due to reference counting
// note: refcount is already zero for actID
void EnvBaseT::ObjCleanup( DObj actID)
{
  if( actID == 0 || (inProgress.find( actID) != inProgress.end()))
    return;

  DStructGDL* actObj;
  try{
    actObj=GetObjHeap( actID);
  }
  catch( GDLInterpreter::HeapException&){
    // not found
    return;
  }
	 
  // found actID  
  if( actObj != NULL)
    {
      InProgressGuard inProgressGuard( actID); // exception save
    
      Guard<BaseGDL> actObjGDL_guard;
      try{
	// call CLEANUP function
	DPro* objCLEANUP= actObj->Desc()->GetPro( "CLEANUP");

	if( objCLEANUP != NULL)
	  {
	    DObjGDL* actObjGDL = new DObjGDL( actID);
	    actObjGDL_guard.Init( actObjGDL);
	    GDLInterpreter::IncRefObj( actID); // set refcount to 1
  
	    PushNewEmptyEnvUD( objCLEANUP, &actObjGDL);
  
	    interpreter->call_pro( objCLEANUP->GetTree());
  
	    EnvBaseT* callStackBack =  interpreter->CallStack().back();
	    interpreter->CallStack().pop_back();
	    delete callStackBack;
	  }
      }
      catch( ...)
	{
	  FreeObjHeap( actID); // make sure actObj is freed
	  throw; // rethrow
	}		
      // actObjGDL_guard goes out of scope -> refcount is (would be) decreased
      FreeObjHeap( actID); 
    }
  else // actObj == NULL
    {
      Warning("Cleaning up invalid (NULL) OBJECT ID <"+i2s(actID)+">.");
      FreeObjHeap( actID); // make sure actObj is freed
    }
}



void EnvT::ObjCleanup( DObj actID)
{
  if( actID != 0 && (inProgress.find( actID) == inProgress.end()))
    {
      DStructGDL* actObj;
      try {
	actObj=GetObjHeap( actID);
	//  		GDLInterpreter::ObjHeapT::iterator it;
	// 		actObj=GDLInterpreter::GetObjHeap( actID, it);
      }
      catch( GDLInterpreter::HeapException) {
	actObj=NULL;
      }

      if( actObj != NULL)
        {
	  // call CLEANUP function
	  DPro* objCLEANUP= actObj->Desc()->GetPro( "CLEANUP");

	  if( objCLEANUP != NULL)
            {
	      DObjGDL* actObjGDL = new DObjGDL( actID);
	      Guard<BaseGDL> actObjGDL_guard( actObjGDL);
	      GDLInterpreter::IncRefObj( actID);

	      StackGuard<EnvStackT> guard( interpreter->CallStack());
	      EnvUDT* newEnv = PushNewEnvUD( objCLEANUP, 1, &actObjGDL);

	      inProgress.insert( actID);

	      interpreter->call_pro( objCLEANUP->GetTree());

	      inProgress.erase( actID);
	      //                 delete newEnv;
	      //                 interpreter->CallStack().pop_back();
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
      if( subUD->GetCommonVarName4Help( p, varName)) 
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
      if( p->Type() == GDL_STRUCT)
	{
	  /*		DStructGDL* s = static_cast<DStructGDL*>( p);
	    os << "-> ";
	    os << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());
	    os << " ";*/
	}
      else if( p->Dim( 0) == 0)
	{
	  os << "(";
	  if (p->Type() == GDL_STRING)
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
// 	if( p->Type() == GDL_STRUCT)
// 	{
// 		DStructGDL* s = static_cast<DStructGDL*>( p);
// 		os << "-> ";
// 		os << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());
// 		os << " ";
// 	}
// 	else if( p->Dim( 0) == 0)
// 	{
// 		os << "(";
// 		if (p->Type() == GDL_STRING)
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


void EnvT::Help(const std::string s_help[], int size_of_s)
{
  if (size_of_s == 0) 
    throw GDLException( CallingNode(), pro->ObjectName()+": no inline doc ready");
  else {
    int i;
    for (i = 0; i < size_of_s; i++)
      Message(pro->ObjectName()+": "+s_help[i]);
    throw GDLException( CallingNode(), pro->ObjectName()+": call to inline help");
  }
}
//TODO: variant enabling static ints in lieu of const string& (speedup!)
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
//TODO: variant enabling static ints in lieu of const string& (speedup!)

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
void EnvBaseT::ResolveExtra()
{
  if( extra != NULL) extra->ResolveExtra( NULL);
}

// // for internal non-library routines (e.g. operator overloads) ('this' is on the stack)
// EnvUDT* EnvUDT::CallingEnv()
// {
//   EnvStackT& callStack=interpreter->CallStack();
//   assert( callStack.size() >= 2); // must be: "$MAIN$" and the EnvUDT of the internal routine
//   return callStack[callStack.size()-2];
// }

// for library subroutines, get the EnvUDT from which they are called
EnvBaseT* EnvBaseT::Caller()
{
  EnvStackT& callStack=interpreter->CallStack();

  //if( callStack.size() <= 1) return NULL;
  // library environments are no longer on the call stack
  // but since we have WRAPPED_FUNNode it is convenient 
  //   assert( callStack.back() != this);
  if( callStack.back() == this)
    {
      assert( callStack.size() >= 2);
      return callStack[ callStack.size() - 2];
    }

  return callStack.back();
  
  //   if( callStack.back() != this) 
  //     return callStack.back();
  // //     return static_cast< EnvUDT*>( callStack.back());
  // 
  //   return callStack[ callStack.size()-2];
  // //   return static_cast< EnvUDT*>( callStack[ callStack.size()-2]);
}

// used by obj_new (basic_fun.cpp)
// and obj_destroy (basic_pro.cpp)
void EnvBaseT::PushNewEmptyEnvUD(  DSubUD* newPro, DObjGDL** newObj)
{
  EnvUDT* newEnv= new EnvUDT( this->CallingNode(), newPro, newObj);

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
// and call_function (basic_fun.cpp)
// and call_procedure (basic_pro.cpp)
EnvUDT* EnvT::PushNewEnvUD(  DSubUD* newPro, SizeT skipP, DObjGDL** newObj)
{
  EnvUDT* newEnv= new EnvUDT( this->CallingNode(), newPro, newObj);

  // pass the parameters, skip the first 'skipP'
  SizeT nParam = NParam();
  for( SizeT p=skipP; p<nParam; p++)
    {
      newEnv->SetNextPar( &GetPar( p)); // pass as global
    }

  //   interpreter->CallStack().push_back( newEnv); // problem with call_function if done here s. b.

  // _REF_EXTRA is set to the keyword string array
  newEnv->extra = new ExtraT( newEnv);
  newEnv->extra->Set( &env[0]);
  newEnv->extra->ResolveExtra( this); // s. a. problem caused here due to a call to EnvBaseT::Caller() in Resolve()

  interpreter->CallStack().push_back( newEnv); 
  return newEnv;
}
// used by obj_new (basic_fun.cpp)
// and obj_destroy (basic_pro.cpp)
// and call_function (basic_fun.cpp)
// and call_procedure (basic_pro.cpp)
EnvT* EnvT::NewEnv(  DSub* newPro, SizeT skipP, DObjGDL** newObj)
{
  EnvT* newEnv= new EnvT( this, newPro, newObj);

  // pass the parameters, skip the first 'skipP'
  SizeT nParam = NParam();
  for( SizeT p=skipP; p<nParam; p++)
    {
      newEnv->SetNextPar( &GetPar( p)); // pass as global
    }

  //   interpreter->CallStack().push_back( newEnv); 

  // _REF_EXTRA is set to the keyword string array
  newEnv->extra = new ExtraT( newEnv);
  newEnv->extra->Set( &env[0]);
  newEnv->extra->ResolveExtra( this);

  return newEnv;
}

void EnvT::AssureGlobalPar( SizeT pIx)
{
  SizeT ix= pIx + pro->key.size();
  AssureGlobalKW( ix);
}
  
void EnvBaseT::AssureGlobalKW( SizeT ix)
{
  if( env.Env( ix) == NULL) {
    if( env.Loc( ix) != NULL)
      Throw( "Attempt to store into an expression.");
    else
      Throw( "Parameter must be a named variable.");
  }
}

DStructGDL* EnvT::GetObjectPar( SizeT pIx)
{
  BaseGDL* p1 = GetParDefined(pIx);

  if( p1->Type() != GDL_OBJ)
    {
      Throw( "Parameter must be an object reference"
	     " in this context: "+
      GetParString(pIx));
    }
  else
    {
    DObjGDL* oRef = static_cast<DObjGDL*> (p1);
    DObj objIx;
    if (!oRef->Scalar(objIx))
      Throw("Parameter must be a scalar or 1 element array in this context: " +
      GetParString(pIx));
    if (objIx == 0)
      Throw("Unable to invoke method"
      " on NULL object reference: " + GetParString(pIx));

    try {
      return GetObjHeap(objIx);
      }
      catch ( GDLInterpreter::HeapException)
	{
      Throw("Object not valid: " + GetParString(pIx));
    }
  }
  return NULL; //keep clang happy.
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
  GDLDelete(*caller->catchVar);
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
  int val=pro->FindKey( k);
  if( val == -1) {		//  assert( val != -1);

    cout << "Invalid Keyword lookup (EnvT::KeywordIx) ! "
				" from "+pro->ObjectName() + "  Key: " + k << endl;
//    cout << pro->ObjectName() << "  Key: " << k << endl;
//		<< " Returning the wrong (but a valid) key index of zero" << endl;
//		val = 0; // too lax - may allow most tests to pass
  	assert( val != -1);
	}
  return val;
}

bool EnvT::KeywordPresent( const std::string& kw)
{
  int ix = KeywordIx( kw);
  return EnvBaseT::KeywordPresent( ix);
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

// SA: used by GDL_STRING() for VMS-compat hack
void EnvT::ShiftParNumbering(int n)
{
  assert(abs(n) == 1); // currently the code below works for n = +/- 1 only

  SizeT nParam = NParam();
  SizeT oParam = pro->key.size();

  if (n == 1)
    {
      BaseGDL* tmp = env[oParam + nParam - 1];
      for (int i = nParam - 1; i > 0; --i) 
	{
	  env[oParam + i] = env[oParam + i - 1];
	}
      env[oParam] = tmp;
    }
  else if (n == -1)
    {
      BaseGDL* tmp = env[oParam];
      for (int i = 0; i < nParam - 1; ++i) 
	{
	  env[oParam + i] = env[oParam + i + 1];
	}
      env[oParam + nParam - 1] = tmp;
    }
}

BaseGDL*& EnvBaseT::GetParDefined(SizeT i)
{
  SizeT ix = i + pro->key.size();

  // cout << i << " -> " << ix << "  " << env.size() << "  env[ix] " << env[ix] << endl;
  if( ix >= env.size())
    Throw("Incorrect number of arguments.");
  if( env[ ix] == NULL || env[ ix] == NullGDL::GetSingleInstance())
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
    Throw( "Incorrect number of arguments.");
  return nPar;
}
SizeT EnvT::NParam( SizeT minPar)
{
  return EnvBaseT::NParam( minPar);
}

bool EnvBaseT::Removeall() 
{
	DSubUD* proD=dynamic_cast<DSubUD*>(pro);
	int osz = env.size();
	for( SizeT ix=0; ix < osz; ix++) {
		if( env[ix] != NULL) GDLDelete( env[ix]);
		env.pop_back();
	}
	proD->Resize(0);
	return true;
}
	
bool EnvBaseT::Remove(int* rindx)
{
	DSubUD* proD=dynamic_cast<DSubUD*>(pro);

	static volatile bool debug( false);  // switch off/on
	static int ix, osz, inrem;

	osz = env.size();
	inrem = 0;
	int itrg = rindx[0];
	ix=itrg;
		if(debug)	printf(" env.size() = %d", osz);	
	while( ix >= 0)
	{
		inrem++;
		if(debug)	printf(" env.now.size() = %d  env[%d] = 0x%x ",
				osz - inrem,
				ix,static_cast <const void *>(env[ix]) );
		if ( env[ix] != NULL) GDLDelete( env[ix]);
		int esrc = rindx[inrem];
		if(esrc < 0) esrc = osz;
		if(debug) cout << " limit:"<< esrc ;
		while( ++ix < esrc) {
			if(debug) cout << ", @:"<<itrg<<"<"<<ix;
				env.Set( itrg, env.Loc(ix));
				proD->ReName(itrg++, proD->GetVarName(ix));
			}
		ix=rindx[inrem];
		if(debug) cout << " inrem:"<<inrem <<" ix:" << ix << endl; 
		}				 // zero all with GDLDelete
	if(inrem <= 0) return false;
	
	osz = osz - inrem;
    while(inrem-- > 0) env.pop_back();

	env.resize(osz);
	proD->Resize(osz);
	return true;
 }
 
int EnvBaseT::findvar(const std::string& s)
{
	DSubUD* proD=dynamic_cast<DSubUD*>(pro);
	int kIx = proD->FindVar(s);
	return kIx;
}

int EnvBaseT::findvar(BaseGDL* delP)
{
//  static BaseGDL* null=NULL;
	for(int Ix=0; Ix < env.size(); Ix++) {
		if(delP != env[ Ix] ) continue;
	return Ix;
	}
	return -1;
}

void EnvBaseT::SetNextParUnchecked( BaseGDL* const nextP) // by value (reset loc)
{
  //   if(!( static_cast<int>(parIx - pro->key.size()) < pro->nPar))
  assert( static_cast<int>(parIx - pro->key.size()) < pro->nPar);
  env.Set(parIx++,nextP); // check done in parameter_def
}
void EnvBaseT::SetNextParUncheckedVarNum( BaseGDL* const nextP) // by reference (reset env)
{
  AddEnv();
  env.Set(parIx++,nextP);
}

void EnvBaseT::SetNextParUnchecked( BaseGDL** const nextP) // by reference (reset env)
{
  assert( static_cast<int>(parIx - pro->key.size()) < pro->nPar);
  env.Set(parIx++,nextP);
}
void EnvBaseT::SetNextParUncheckedVarNum( BaseGDL** const nextP) // by reference (reset env)
{
  AddEnv();
  env.Set(parIx++,nextP);
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
	Throw(  "Keyword parameter -"+k+"- not allowed in call "
		"to: "+pro->Name());
      // 	throw GDLException(callingNode,
      // 			   "Keyword parameter "+k+" not allowed in call "
      // 			   "to: "+pro->Name());

      Warning("Warning: Keyword parameter "+k+" not supported in call "
	      "to: "+pro->Name() + ". Ignored.");
      
      return -4;
    }
  
  // search keyword
  KeyVarListT::iterator f=std::find_if(pro->key.begin(),
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
	    Throw( "Keyword parameter <"+k+"> not allowed in call "
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
  KeyVarListT::iterator ff=std::find_if(f+1,
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
  // (move to Throw by AC on June 25, 2014, bug found by Levan.)
// Removed G. Jung 2016:
// mungs things up.  Could not determine 2014 bug.
//  if( KeywordPresent(varIx)) // just a message in the original
//    {
//      Throw( "Duplicate keyword "+k+" in call to: "+pro->Name());
//    }

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
  return EnvBaseT::KeywordSet( ix);
}
bool EnvBaseT::KeywordSet( SizeT ix)
{
  BaseGDL* keyword=env[ix];
  if( keyword == NULL) return false;
  if( !keyword->Scalar()) return true;
  if( keyword->Type() == GDL_STRUCT) return true;
  return keyword->LogTrue();
}

// returns the ix'th parameter (for library function API only)
BaseGDL*& EnvT::GetPar(SizeT i)
{
  static BaseGDL* null=NULL;
  SizeT ix= i + pro->key.size();
  if( ix >= env.size()) 
    {
      //       Warning( "EnvT::GetPar(): Index out of env size ("+i2s(env.size())+"): " + i2s(i) +" (+ "+i2s(pro->key.size())+" KW)");
      return null;
    }
  return env[ ix];
}
// BaseGDL*& EnvT::GetParUnchecked(SizeT i)
// {
//   SizeT ix= i + pro->key.size();
//   return env[ ix];
// }

void EnvBaseT::AssureLongScalarPar( SizeT pIx, DLong64& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DLong64GDL* lp = static_cast<DLong64GDL*>(p->Convert2( GDL_LONG64, BaseGDL::COPY));
  Guard<DLong64GDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar or 1 element array in this context: "+
	  GetParString(pIx));
}
void EnvBaseT::AssureLongScalarPar( SizeT pIx, DLong& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DLongGDL* lp = static_cast<DLongGDL*>(p->Convert2( GDL_LONG, BaseGDL::COPY));
  Guard<DLongGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar or 1 element array in this context: "+
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
void EnvT::AssureLongScalarKW( const std::string& kw, DLong64& scalar)
{
  AssureLongScalarKW( KeywordIx( kw), scalar);
}

void EnvT::AssureLongScalarKW( SizeT eIx, DLong64& scalar)
{
  BaseGDL* p = GetKW( eIx);
  
  if( p == NULL)
    Throw("Expression undefined: "+GetString(eIx));
  
  DLong64GDL* lp= static_cast<DLong64GDL*>(p->Convert2( GDL_LONG64, BaseGDL::COPY));
  
  Guard<DLong64GDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar or 1 element array in this context: "+
	  GetString(eIx));
}
void EnvT::AssureLongScalarKW( SizeT eIx, DLong& scalar)
{
  BaseGDL* p = GetKW( eIx);
  
  if( p == NULL)
    Throw("Expression undefined: "+GetString(eIx));
  
  DLongGDL* lp= static_cast<DLongGDL*>(p->Convert2( GDL_LONG, BaseGDL::COPY));
  
  Guard<DLongGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar or 1 element array in this context: "+
	  GetString(eIx));
}

void EnvT::AssureDoubleScalarPar( SizeT pIx, DDouble& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DDoubleGDL* lp = static_cast<DDoubleGDL*>(p->Convert2( GDL_DOUBLE, BaseGDL::COPY));
  Guard<DDoubleGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar or 1 element array in this context: "+
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
  
  DDoubleGDL* lp= static_cast<DDoubleGDL*>(p->Convert2( GDL_DOUBLE, BaseGDL::COPY));
  
  Guard<DDoubleGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar or 1 element array in this context: "+
	  GetString(eIx));
}


void EnvT::AssureFloatScalarPar( SizeT pIx, DFloat& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DFloatGDL* lp = static_cast<DFloatGDL*>(p->Convert2( GDL_FLOAT, BaseGDL::COPY));
  Guard<DFloatGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar or 1 element array in this context: "+
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
  
  DFloatGDL* lp= static_cast<DFloatGDL*>(p->Convert2( GDL_FLOAT, BaseGDL::COPY));
  
  Guard<DFloatGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar or 1 element array in this context: "+
	  GetString(eIx));
}


void EnvT::AssureStringScalarPar( SizeT pIx, DString& scalar)
{
  BaseGDL* p = GetParDefined( pIx);
  DStringGDL* lp = static_cast<DStringGDL*>(p->Convert2( GDL_STRING, BaseGDL::COPY));
  Guard<DStringGDL> guard_lp( lp);
  if( !lp->Scalar( scalar))
    Throw("Parameter must be a scalar or 1 element array in this context: "+
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
  
  DStringGDL* lp= static_cast<DStringGDL*>(p->Convert2( GDL_STRING, BaseGDL::COPY));
  Guard<DStringGDL> guard_lp( lp);

  if( !lp->Scalar( scalar))
    Throw("Expression must be a scalar or 1 element array in this context: "+
	  GetString(eIx));
}

void EnvBaseT::SetKW( SizeT ix, BaseGDL* newVal)
{
  // can't use Guard here as data has to be released
  Guard<BaseGDL> guard( newVal);
  AssureGlobalKW( ix);
  GDLDelete(GetKW( ix));
  GetKW( ix) = guard.release();
}
void EnvT::SetPar( SizeT ix, BaseGDL* newVal)
{
  // can't use Guard here as data has to be released
  Guard<BaseGDL> guard( newVal);
  AssureGlobalPar( ix);
  GDLDelete(GetPar( ix));
  GetPar( ix) = guard.release();
}

// bool EnvBaseT::Contains( BaseGDL* p) const 
// { 
//   if( env.Contains( p)) return true;
//   if (static_cast<DSubUD*>(pro)->GetCommonVarPtr( p) != NULL) return true;
//   // horrible slow... but correct
//   return Interpreter()->GetPtrToHeap( p) != NULL;
// }

// BaseGDL** EnvBaseT::GetPtrTo( BaseGDL* p) 
// { 
//   BaseGDL** pp = env.GetPtrTo( p);
//   if( pp != NULL) return pp;
//   pp = static_cast<DSubUD*>(pro)->GetCommonVarPtr( p);
//   if( pp != NULL) return pp;
//   return GDLInterpreter::GetPtrToHeap( p);
// }
