/***************************************************************************
                          envt.hpp  -  the environment for each GDL module
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ENVT_HPP_
#define ENVT_HPP_

#include <limits>
#include <vector>
#include <cstdlib>

#include "typedefs.hpp"
#include "str.hpp"
#include "dpro.hpp"
#include "datatypes.hpp"
#include "dstructgdl.hpp"
#include "datalistt.hpp"
#include "extrat.hpp"
#include "calendar.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

class DInterpreter;

namespace lib {
  BaseGDL* obj_new( EnvT* e);
  void obj_destroy( EnvT* e);
}



class EnvBaseT
{
private:
//   typedef std::deque<BaseGDL*> ContainerT;
  typedef ExprListT ContainerT;

//   SizeT toDestroyInitialIndex;

  EnvBaseT(){}
  
protected:
  // stores all data which has to deleted upon destruction
  ContainerT toDestroy;
   //   static ContainerT toDestroy;

public:
  // needed to delete temporary ptr parameters only after subroutine completion
  // 'guards' a newly created variable which should be deleted
  // upon library routines exit (normal or on error)
  // elimates the need of auto_ptr and in some places later destruction is needed
  
  void DeleteAtExit( BaseGDL* toGuard)
    {
//	if( toDestroy == NULL)
//		toDestroy = new ContainerT();
      toDestroy.push_back( toGuard);
    }

protected:

  // for obj cleanup
  static std::set< DObj> inProgress;
  
  static DInterpreter* interpreter;
  DataListT         env;
  SizeT                parIx;     // ix of next parameter to put
  DSub*              pro;
  ProgNodeP      callingNode;
  int			          lineNumber;
  bool                 obj;       // member subroutine?
  ExtraT*            extra;

  EnvBaseT*      newEnvOff;

  // finds the local variable pp points to
//   int FindLocalKW( BaseGDL** pp) { return env.FindLocal( pp);}
private:
    BaseGDL** ptrToReturnValue;
public:

  void SetPtrToReturnValue(BaseGDL** p) { ptrToReturnValue = p;}
  BaseGDL** GetPtrToReturnValueNull() { BaseGDL** p = ptrToReturnValue;ptrToReturnValue=NULL;return p;}
  BaseGDL** GetPtrToReturnValue() const { return ptrToReturnValue;}
  BaseGDL** GetPtrToGlobalReturnValue() 
  { 
    if( ptrToReturnValue == NULL)
      return NULL;
    if( env.InLoc(ptrToReturnValue))
    {
      *ptrToReturnValue = NULL; // steal local value
      return NULL; // return as not global
    }
    return ptrToReturnValue;
    
  }
  bool InLoc( BaseGDL** pp)
  {
    return env.InLoc(pp);
  }
  
  
  void SetKW( SizeT ix, BaseGDL* newVal);

  // used by the interperter returns the keyword index, used for UD functions
  // and used by WRAPPED subroutines
  int GetKeywordIx( const std::string& k);

	
  int findvar(const std::string& s);
  int findvar(BaseGDL* delP);
  bool Remove(int* rindx);
  bool Removeall();
  
  bool StealLocalKW( SizeT ix) 
  { 
    if( LocalKW( ix))
      {
	env.Clear( ix);
	return true;
      }
    return false;
  }

//   bool StealLocalKW( BaseGDL** ref) 
//   { 
//    if( !env.InLoc( pp))
//      return false;
// 
//    *ref = NULL;
//     return true;
//   }

  bool LocalKW( SizeT ix) const
  {
    if( ix >= env.size()) return false;
    return ( env.Loc( ix) != NULL);
  }

  bool GlobalKW( SizeT ix) const
  {
    if( ix >= env.size()) return false;
    return ( env.Env( ix) != NULL);
  }

  bool KeywordSet( SizeT ix);

protected:
  // for HEAP_GC
  static void AddStruct( DPtrListT& ptrAccessible,  DPtrListT& objAccessible, 
		  DStructGDL* stru);
  static void AddPtr( DPtrListT& ptrAccessible, DPtrListT& objAccessible, 
	       DPtrGDL* ptr);
  static void AddObj( DPtrListT& ptrAccessible, DPtrListT& objAccessible, 
	       DObjGDL* obj);
  static void Add( DPtrListT& ptrAccessible, DPtrListT& objAccessible, 
	    BaseGDL* p);
  
  // definition in list.cpp
  static void AddLIST( DPtrListT& ptrAccessible,
		        DPtrListT& objAccessible, DStructGDL* listStruct);


public:
// 	void DebugInfo()
// 	{
// 		std::cout << this->pro->ObjectName() << std::endl;
// 	}

  EnvBaseT* GetNewEnv() {return newEnvOff; }
  void SetNewEnv( EnvBaseT* nE) { newEnvOff = nE;}

  virtual void ObjCleanup( DObj actID);

  // for CLEANUP calls due to reference counting
  void PushNewEmptyEnvUD(  DSubUD* newPro, DObjGDL** newObj = NULL);
//   void PushNewEmptyEnvUDWithExtra(  DSubUD* newPro, BaseGDL** newObj = NULL);
  
  void AddEnv( DPtrListT& ptrAccessible, DPtrListT& objAccessible);
  void AddToDestroy( DPtrListT& ptrAccessible, DPtrListT& objAccessible);

  virtual ~EnvBaseT()
  {
    delete extra;
// 	delete toDestroy; // cleans up its content
/*    for( SizeT i=toDestroyInitialIndex; i<toDestroy.size(); ++i)
      {
		delete toDestroy[i];
      }
    toDestroy.resize( toDestroyInitialIndex);*/
//      for( ContainerT::iterator i=toDestroy.begin();
//  	 i != toDestroy.end(); ++i) 
//        delete *i;
  }

  EnvBaseT( ProgNodeP cN, DSub* pro_);

  static SizeT NewObjHeap( SizeT n=1, DStructGDL* v=NULL);
  static SizeT NewHeap( SizeT n=1, BaseGDL* v=NULL);
  static void FreeObjHeap( DObj id);
  static void FreeHeap( DPtr id);
  static void FreeHeap( DPtrGDL* p);
  static DStructGDL* GetObjHeap( DObj ID);
  static BaseGDL* GetHeap( DPtr ID);

  int GetLineNumber()
  {
  return lineNumber;
  }
  void SetLineNumber(int l)
  {
  lineNumber = l;
  }
  unsigned int CompileOpt()
  {
    DSubUD* proUD = dynamic_cast<DSubUD*>(pro);
    if( proUD == NULL)
	    throw GDLException("Intenal error: CompileOpt called non DSub object.");
    return proUD->GetCompileOpt();
  }
  void SetCompileOpt( unsigned int cOpt)
  {
    DSubUD* proUD = dynamic_cast<DSubUD*>(pro);
    if( proUD == NULL)
	    throw GDLException("Intenal error: CompileOpt called non DSub object.");
    proUD->SetCompileOpt( cOpt);
  }

  // raise an exception from within a library function
  // automatically cares for adding line/column info and the
  // function name. 's' should be set to the 'raw' error message
  // saves some typing :-)
  void Throw( const std::string& s)
  { throw GDLException( CallingNode(), pro->ObjectName()+": "+s, false, false);}

  // finds the global variable pp (used by arg_present function)
  int FindGlobalKW( BaseGDL** pp) { return env.FindGlobal( pp);}

  // checks if pp points to a local variable
  bool IsLocalKW( BaseGDL** pp) const { return env.InLoc( pp);}

//   void RemoveLoc( BaseGDL* p) { env.RemoveLoc( p);}

  // called after parameter definition
  void ResolveExtra();
  friend class ExtraT;

  // used by compiler and from EnvT (for variable number of paramters)
  SizeT AddEnv()
  {
    SizeT s = env.size();
    env.AddOne();
    return s;
  }

   void DelEnv()
   {
     env.pop_back();
   }

  // the upper (calling) environment
  // a EnvT must have always a /*EnvUDT*/ caller
  // i.e. library functions never call each other
  // with a new EnvT environment
  // for library subroutines, get the EnvUDT from which they are called
  EnvBaseT* Caller();

  // returns environment data, by value (but that by C++ reference)
  BaseGDL*& GetKW(SizeT ix) { return env[ix];}

  // used by HELP, SetNextPar(...)
  SizeT EnvSize() const { return env.size();}

  // next four are used by Parameter...(...) functions
  void SetNextParUnchecked( BaseGDL* const nextP); // by value (reset loc)
  void SetNextParUnchecked( BaseGDL** const nextP); // by reference (reset env)
  void SetNextParUncheckedVarNum( BaseGDL* const nextP); // by value (reset loc)
  void SetNextParUncheckedVarNum( BaseGDL** const nextP); // by reference (reset env)
  // these are used outside Parameter functions
  void SetNextPar( BaseGDL* const nextP); // by value (reset loc)
  void SetNextPar( BaseGDL** const nextP); // by reference (reset env)
  void SetKeyword( const std::string& k, BaseGDL* const val);  // value
  void SetKeyword( const std::string& k, BaseGDL** const val); // reference

  // to check if a lib function returned a variable of this env
//   bool Contains( BaseGDL* p) const;

//   BaseGDL** GetPtrTo( BaseGDL* p);

  DInterpreter* Interpreter() const { return interpreter;}

  bool  IsObject() const { return obj;}
  DSub* GetPro()   const { return pro;}

  ProgNodeP CallingNode() { return callingNode;}

  SizeT NParam( SizeT minPar = 0); //, const std::string& subName = "");

  const std::string GetProName() const
  {
    if( pro == NULL) return "";
    return pro->ObjectName();
  }
  
  // get the name of 'i'th parameter
  const std::string GetParString( SizeT i)
  {
    SizeT ix= i + pro->key.size();
    return GetString( ix);
  }

  // get the name of 'ix'th environment entry (asks 'pro')
  const std::string GetString( SizeT ix);
  
  // get name of 'p'
  const std::string GetString( BaseGDL*& p, bool calledFromHELP=false);

//   // get name of 'p'
//   const std::string GetString( BaseGDL* p);

  virtual const std::string GetFilename() const=0;

  void AssureGlobalKW( SizeT ix);

  // converts parameter 'ix' if necessary and sets 'scalar' 
  void AssureLongScalarPar( SizeT ix, DLong& scalar);
  void AssureLongScalarPar( SizeT ix, DLong64& scalar);
  // get i'th parameter
  // throws if not defined (ie. never returns NULL)
  BaseGDL*& GetParDefined(SizeT i); //, const std::string& subName = "");
  bool KeywordPresent( SizeT ix)
  { return (env.Loc(ix)!=NULL)||(env.Env(ix)!=NULL);}
  void SetNextParUnckeckedVarNum(BaseGDL** arg1);

  friend class DInterpreter; // gcc 4.4 compatibility
  friend class InProgressGuard;
};


// preallocates a buffer which should never be exceeded
// but will work with any number of elements
template< typename T, SizeT defaultLength> class ForInfoListT
{
public:
typedef T* iterator;

private:
T* eArr;
char buf[defaultLength * sizeof(T)]; // prevent constructor calls
SizeT sz;

public:

ForInfoListT(): eArr( reinterpret_cast<T*>(buf)), sz( 0)
{
}

~ForInfoListT()
{
	if( eArr != reinterpret_cast<T*>(buf))
		delete[] eArr;
	else
	{
		T* pEnd = &eArr[sz];
		for( T* p = &eArr[0]; p!=pEnd; ++p)
			p->Clear();
	}
}

// must be called before access
void InitSize( SizeT s)
{
    assert( sz == 0);
    if( s == 0)
	  return;
    sz = s;
    if( s < defaultLength)
    {
	    for( SizeT i=0; i<s; ++i)
		    eArr[ i].Init();
	    return;
    }
    eArr = new T[ s]; // constructor called
}
// only needed for EXECUTE
void resize( SizeT s)
{
    if( s == sz)
      return;
    if( s < sz) // shrink
    {
	for( SizeT i=s; i<sz; ++i)
		eArr[ i].ClearInit(); // in case eArr was allocated
	sz = s;
	return;
    }
    // s > sz -> grow
    if( s <= defaultLength && eArr == reinterpret_cast<T*>(buf))
    {
		for( SizeT i=sz; i<s; ++i)
			eArr[ i].Init();
		sz = s;
		return;
    }
    // this should never happen (or only in extreme rarely cases)
    // the performance will go down
    // s > defaultLength
    T* newArr = new T[ s]; // ctor called
	if( eArr != reinterpret_cast<T*>(buf))
	{
		for( SizeT i=0; i<sz; ++i)
			{
				newArr[i] = eArr[ i];
				eArr[ i].Init(); // prevent dtor from freeing
			}
		delete[] eArr;
	}
    else
    {
		for( SizeT i=0; i<s; ++i)
			{
				newArr[i] = eArr[ i];
			}
    }
    eArr = newArr;
    sz = s;
}
// T operator[]( SizeT i) const { assert( i<sz);  return eArr[i];}
T& operator[]( SizeT i) { assert( i<sz);  return eArr[i];}
SizeT size() const { return sz;}
iterator begin() const { return &eArr[0];}
iterator end() const { return &eArr[sz];}
bool empty() const { return sz == 0;}
T& front() { return eArr[0];}
const T& front() const { return eArr[0];}
T& back() { return eArr[sz-1];}
const T& back() const { return eArr[sz-1];}
};



// for UD subroutines (written in GDL) ********************************
class EnvUDT: public EnvBaseT
{
// static std::deque< void*> freeList;
static FreeListT freeList;

public:
static 	void* operator new( size_t bytes);
static	void operator delete( void *ptr);

enum CallContext {
   RFUNCTION = 0
  ,LFUNCTION
  ,LRFUNCTION
};

private:
ForInfoListT<ForLoopInfoT, 32> forLoopInfo;
// std::vector<ForLoopInfoT> forLoopInfo;

  ProgNodeP         ioError; 
  DLong             onError; // on_error setting
  BaseGDL**         catchVar;
  ProgNodeP         catchNode; 
  CallContext       callContext; // assignment paramter for functions as l_value
  SizeT             nJump; // number of jumps in current environment
  int               lastJump; // to which label last jump went
  
public:
   ForLoopInfoT& GetForLoopInfo( int forIx) { return forLoopInfo[forIx];}
   
  int NForLoops() const { return forLoopInfo.size();}
  void ResizeForLoops( int newSize) { forLoopInfo.resize(newSize);}

  // UD pro/fun
  EnvUDT( ProgNodeP idN, DSubUD* pro_, CallContext lF = RFUNCTION);

  // member procedure
  EnvUDT( ProgNodeP idN, BaseGDL* self, 
	const std::string& parent="");
  // member function
  EnvUDT( BaseGDL* self, ProgNodeP idN, 
	const std::string& parent="", CallContext lF = RFUNCTION);


  // for obj_new and obj_destroy
  //EnvUDT( EnvBaseT* pEnv, DSub* newPro, BaseGDL** self); 
  EnvUDT( ProgNodeP callindNode_, DSubUD* newPro, DObjGDL** self); 

  DLong GetOnError() const { return onError;}

  ProgNodeP GetCatchNode() const { return catchNode;} 
  BaseGDL** GetCatchVar() const { return catchVar;} 

  SizeT NJump() const { return nJump;}
  int   LastJump() const { return lastJump;}
  ProgNodeP GotoTarget( int ix)
  { 
    lastJump = ix; //static_cast<DSubUD*>( pro)->LabelOrd( ix);
    ++nJump;
    return static_cast<DSubUD*>( pro)->GotoTarget( ix);
  }
  CallContext GetCallContext() const { return callContext;} // left-function
  void SetCallContext(CallContext cC) { callContext = cC;} // left-function

  void SetIOError( int targetIx) 
  { // this isn't a jump
    if( targetIx != -1)
      ioError = static_cast<DSubUD*>( pro)->GotoTarget( targetIx)->
	getNextSibling(); 
    else
      ioError = NULL;
  }
  ProgNodeP GetIOError() { return ioError;}

  const std::string GetFilename() const
  {
    DSubUD* subUD=static_cast<DSubUD*>( pro);
    return subUD->GetFilename();
  }

//   // for internal non-library routines (e.g. operator overloads) ('this' is on the stack)
//   EnvUDT* CallingEnv();

  friend class DInterpreter;
  friend class EnvT;
};


// for library subroutines **************************************
// this contains the library function API ***********************
class EnvT: public EnvBaseT
{
static std::vector< void*> freeList;
  
public:
static 	void* operator new( size_t bytes);
static	void operator delete( void *ptr);

  // Please use non library API (see below) function with caution
  // (most of them can be ignored by library function authors)
public:
  ~EnvT()
  {
  }

  EnvT( ProgNodeP cN, DSub* pro_);

  // for obj_new and obj_destroy
  EnvT( EnvT* pEnv, DSub* newPro, DObjGDL** self); 

  void HeapGC( bool doPtr, bool doObj, bool verbose);
  void ObjCleanup( DObj actID);

  // used by obj_new (basic_fun.cpp)
  EnvT* NewEnv(  DSub* newPro, SizeT skipP, DObjGDL** newObj=NULL);
  EnvUDT* PushNewEnvUD(  DSubUD* newPro, SizeT skipP, DObjGDL** newObj=NULL);
  // for exclusive use by lib::on_error
  void OnError();
  // for exclusive use by lib::catch_pro
  void Catch();

  const std::string GetFilename() const
  {
    static const std::string internal(INTERNAL_LIBRARY_STR);
    return internal;
  }

  // *************************
  // API for library functions
  // *************************

//   // raise an exception from within a library function
//   // automatically cares for adding line/column info and the
//   // function name. 's' should be set to the 'raw' error message
//   // saves some typing :-)
//   void Throw( const std::string& s)
//   { throw GDLException( CallingNode(), pro->ObjectName()+": "+s);}

  // From now on all library functions which return a l-value must
  // call SetPtrToReturnValue with the ptr to the returned value
  void SetPtrToReturnValue(BaseGDL** p) { EnvBaseT::SetPtrToReturnValue(p);}

  // will print the message (can be multiline) and exit
  // first usage in "math_fun_ac.cpp"
  void Help(const std::string s_help[], int size_of_s);
 
  // returns environment data, by value (but that by C++ reference)
  // in EnvBaseT
  //   BaseGDL*& GetKW(SizeT ix) { return env[ix];}

  // it is now possible to define a niminum number of parameters for library subroutines
  // if this is done the next function can be used
  //   BaseGDL*& GetParUnchecked(SizeT i);

  // returns the ix'th parameter (NULL if not defined)
  BaseGDL*& GetPar( SizeT i);  

  // get i'th parameter
  // throws if not defined (ie. never returns NULL)
  BaseGDL*& GetParDefined(SizeT i); //, const std::string& subName = "");

  // throw for GDL_STRING, GDL_STRUCT, GDL_PTR and GDL_OBJ
  BaseGDL*& GetNumericParDefined( SizeT ix)
  {
    BaseGDL*& p0 = GetParDefined( ix);
    if( NumericType( p0->Type()))
	return p0;

    // AC 2014-08-14 : in fact, in most case, a tentative of String to Numeric
    // convertion is done. E.g. invert(['1']) is OK !
    //    if( p0->Type() == GDL_STRING)
    //  Throw( "String expression not allowed in this context: "+GetParString(ix));
    if( p0->Type() == GDL_STRUCT)
      Throw( "Struct expression not allowed in this context: "+GetParString(ix));
    if( p0->Type() == GDL_PTR)
      Throw( "Pointer expression not allowed in this context: "+GetParString(ix));
    if( p0->Type() == GDL_OBJ)
      Throw( "Object reference not allowed in this context: "+GetParString(ix));

    assert( false);
    return p0;
  }

  // throw for non-Arrays
  BaseGDL*& GetNumericArrayParDefined( SizeT ix)
  {
    BaseGDL*& p0 = GetNumericParDefined( ix);
    if (p0->Rank() != 0) return p0;
    Throw("Expression must be an array in this context: "+GetParString(ix));
    assert(false);
    throw;
  }

  // get i'th parameter
  // throws if not global (might be NULL), for assigning a new variable to
  // (write only)
  BaseGDL*& GetParGlobal(SizeT i); 

  // get the pIx'th paramter and converts it to T if necessary
  // implies that the parameter must be defined
  // if it converts it cares for the destruction of the copy
  // CAUTION: this is for *read only* data, as the returned data might
  // be a copy or not
  template <typename T> 
  T* GetParAs( SizeT pIx)
  {
    BaseGDL* p = GetParDefined( pIx);
	if( p->Type() == T::t)
		return static_cast<T*>( p);
//     T* res = dynamic_cast<T*>( p);
//     if( res != NULL) return res;
    T* res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    this->DeleteAtExit( res);
    return res;
  }
  // same as before for keywords
  template <typename T> 
  T* GetKWAs( SizeT ix)
  {
    BaseGDL* p = GetKW( ix);
    if( p == NULL)
      Throw( "Keyword is undefined: "+GetString( ix));
    if( p->Type() == T::t)
      return static_cast<T*>( p);
//     T* res = dynamic_cast<T*>( p);
//     if( res != NULL) return res;
    T* res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    this->DeleteAtExit( res);
    return res;
  }

  // next two same as last two, but return NULL if parameter/keyword is not defined
  template <typename T> 
  T* IfDefGetParAs( SizeT pIx)
  {
    BaseGDL* p = GetPar( pIx);
    if( p == NULL) return NULL;
    if( p->Type() == T::t)
	return static_cast<T*>( p);
//     T* res = dynamic_cast<T*>( p);
//     if( res != NULL) return res;
    T* res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    this->DeleteAtExit( res);
    return res;
  }
  // same as before for keywords
  template <typename T> 
  T* IfDefGetKWAs( SizeT ix)
  {
    BaseGDL* p = GetKW( ix);
    if( p == NULL) return NULL;
	if( p->Type() == T::t)
		return static_cast<T*>( p);
//     T* res = dynamic_cast<T*>( p);
//     if( res != NULL) return res;
    T* res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    this->DeleteAtExit( res);
    return res;
  }

  // returns the struct of a valid object reference or throws
  DStructGDL* GetObjectPar( SizeT pIx);

  // returns the actual number of paramters passed to a library function
  // minPar is the minimal number of paramters the function needs
  // (if less it throws), subName is used for error reporting
  SizeT NParam( SizeT minPar = 0); //, const std::string& subName = "");

  // for library functions (keyword must be exact)
  // returns the index of keyword k
  int KeywordIx( const std::string& k);

  // for use within library functions
  // consider to use (note: 'static' is the point here):
  // static int kwIx = env->KeywordIx( "KEYWORD");
  // bool kwSet = env->KeywordSet( kwIx);
  //
  // instead of:
  // bool kwSet = env->KeywordSet( "KEYWORD");
  //
  // this one adds some overhead, but is easy to use
  bool KeywordSet( const std::string& kw);
  // this one together with a static int holding the index is faster
  // (after the first call)
  bool KeywordSet( SizeT ix);
  // GD added -- possibly very wrong?
  bool KeywordPresent( const std::string& kw);
  bool KeywordPresent( SizeT ix)
  { return EnvBaseT::KeywordPresent( ix);}

  // local/global keyword/paramter
  bool LocalKW( SizeT ix) const
  {
    return EnvBaseT::LocalKW( ix);
//     if( ix >= env.size()) return false;
//     return ( env.Loc( ix) != NULL);
  }
  bool GlobalKW( SizeT ix) const
  {
    return EnvBaseT::GlobalKW( ix);
  }
  bool LocalPar( SizeT ix) { return LocalKW( ix + pro->key.size());}
  bool StealLocalPar( SizeT ix) 
  { 
    if( LocalKW( ix + pro->key.size()))
      {
	env.Clear( ix + pro->key.size());
	return true;
      }
    return false;
  }
// removed: IDL does not undefine the global parameter with OVERWRITE
//   void StealLocalParUndefGlobal( SizeT ix)
//   {
//     if( LocalKW( ix + pro->key.size()))
//       {
// 	env.Clear( ix + pro->key.size());
//       }
//       else
//       {
//       env[ ix + pro->key.size()] = NULL;
//       }
//   }
  bool GlobalPar( SizeT ix) { return GlobalKW( ix + pro->key.size());}

  // next two to set keywords/paramters
  // note that the value MUST be created in the library function
  // with operator new
  // Before it must be tested with KeywordPresent() or NParam() if
  // the keyword/paramter is present 
  // this is not done automatically because its more effective, to 
  // create the data (newVal) only if its necessary
  // if the functions throw, they delete newVal before -> no
  // guarding of newVal is needed
//   void SetKW( SizeT ix, BaseGDL* newVal);
  void SetPar( SizeT ix, BaseGDL* newVal);

  // Assure functions:
  // if name contains "Par" they must be used for paramters, else for keywords
  // (the error messages are defined for this usage and the indexing is 
  // done respectively)

  // next two: NO CONVERSION (throw if wrong type)
  // NOTE: only few functions need to be so restrictive
  // converts parameter to scalar, throws if parameter is of different type,
  // non-scalar or not defined
  template <typename T> 
  void AssureScalarPar( SizeT pIx, typename T::Ty& scalar)
  {
    BaseGDL* p = GetParDefined( pIx);
    if( p->Type() != T::t)
      Throw( "Variable must be a "+T::str+" in this context: "+
	     GetParString(pIx));
    T* tp= static_cast<T*>(p);
    if( !tp->Scalar( scalar))
      Throw("Variable must be a scalar in this context: "+
	    GetParString(pIx));
  }
  // same as before for keywords
  template <typename T> 
  void AssureScalarKW( SizeT ix, typename T::Ty& scalar)
  {
    BaseGDL* p = GetKW( ix);
    if( p == NULL)
      Throw("Keyword undefined: "+GetString(ix));
    if( p->Type() != T::t)
      Throw("Keyword must be a "+T::str+" in this context: "+
	    GetString(ix));
    T* tp= static_cast<T*>(p);
    if( !tp->Scalar( scalar))
      Throw("Keyword must be a scalar in this context: "+
	    GetString(ix));
  }

  void AssureGlobalPar( SizeT pIx);
//   void AssureGlobalKW( SizeT ix);

  // if keyword 'kw' is not set, 'scalar' is left unchanged
  void AssureLongScalarKWIfPresent( const std::string& kw, DLong& scalar);
  void AssureLongScalarKWIfPresent( SizeT ix, DLong& scalar);
  // converts keyword 'kw' if necessary and sets 'scalar' 
  void AssureLongScalarKW( const std::string& kw, DLong& scalar);
  void AssureLongScalarKW( const std::string& kw, DLong64& scalar);
  // converts ix'th keyword if necessary and sets 'scalar' 
  void AssureLongScalarKW( SizeT ix, DLong& scalar);
  void AssureLongScalarKW( SizeT ix, DLong64& scalar);
  // converts parameter 'ix' if necessary and sets 'scalar' 
  void AssureLongScalarPar( SizeT ix, DLong& scalar);
  void AssureLongScalarPar( SizeT ix, DLong64& scalar);

  // same as for Long
  void AssureDoubleScalarKWIfPresent( const std::string& kw, DDouble& scalar);
  void AssureDoubleScalarKWIfPresent( SizeT ix, DDouble& scalar);
  void AssureDoubleScalarKW( const std::string& kw, DDouble& scalar);
  void AssureDoubleScalarKW( SizeT ix, DDouble& scalar);
  void AssureDoubleScalarPar( SizeT ix, DDouble& scalar);

  // same as for Long
  void AssureFloatScalarKWIfPresent( const std::string& kw, DFloat& scalar);
  void AssureFloatScalarKWIfPresent( SizeT ix, DFloat& scalar);
  void AssureFloatScalarKW( const std::string& kw, DFloat& scalar);
  void AssureFloatScalarKW( SizeT ix, DFloat& scalar);
  void AssureFloatScalarPar( SizeT ix, DFloat& scalar);

  // same as for Long
  void AssureStringScalarKWIfPresent( const std::string& kw, DString& scalar);
  void AssureStringScalarKWIfPresent( SizeT ix, DString& scalar);
  void AssureStringScalarKW( const std::string& kw, DString& scalar);
  void AssureStringScalarKW( SizeT ix, DString& scalar);
  void AssureStringScalarPar( SizeT ix, DString& scalar);

  // to be extended on demand for other data types  

  // SA: used by GDL_STRING() for VMS-compat hack
  void ShiftParNumbering(int n);
};

const int defaultStackDepth = 64;
class EnvStackT
{
  EnvUDT** envStackFrame;
  EnvUDT** envStack;

  SizeT top;
  SizeT sz;

public:
  typedef SizeT size_type;
  typedef EnvUDT* pointer_type;
  
  EnvStackT(): top(0), sz(defaultStackDepth) 
  {
    envStackFrame = new EnvUDT* [ sz+1];
    envStack = envStackFrame + 1;
  }
  ~EnvStackT() { delete[] envStackFrame;}
  
  bool empty() const { return top == 0;}
  
  void push_back( EnvUDT* b) 
  {
    if( top >= sz)
    {
      if( sz >= 32768)
	    throw GDLException("Recursion limit reached ("+i2s(sz)+").");

      EnvUDT** newEnvStackFrame = new EnvUDT* [ sz + sz + 1];
      EnvUDT** newEnvStack = newEnvStackFrame + 1;

      for( SizeT i=0; i<sz; ++i)
	newEnvStack[ i] = envStack[ i];

      delete[] envStackFrame;
      envStackFrame = newEnvStackFrame;
      envStack = newEnvStack;
      sz += sz;
    }
    envStackFrame[ ++top] = b; 
  }
  void pop_back() { assert(top>0); --top;}
  EnvUDT* back() const { assert(top>0); return envStackFrame[ top];}
  SizeT size() const { return top;}
  EnvUDT* operator[]( SizeT ix) const { return envStack[ ix];}
  EnvUDT*& operator[]( SizeT ix) { return envStack[ ix];}
//   EnvUDT** begin() const { return &envStack[0];}
//   EnvUDT** end() const { return &envStack[sz];}
};

// typedef std::deque<EnvBaseT*> EnvStackT;
// typedef std::deque<EnvUDT*> EnvStackT;

#endif


