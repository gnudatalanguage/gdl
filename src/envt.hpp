/***************************************************************************
                          envt.hpp  -  the environment for each GDL module
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
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

#include <vector>
#include <cstdlib>

#include "str.hpp"
#include "dpro.hpp"
#include "datatypes.hpp"
#include "dstructgdl.hpp"
#include "datalistt.hpp"
#include "extrat.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

class GDLInterpreter;

namespace lib {
  BaseGDL* obj_new( EnvT* e);
  void obj_destroy( EnvT* e);
}

class EnvT
{
  // Please use non library API (see below) function with caution
  // (most of them can be ignored by library function authors)
  GDLInterpreter*   interpreter;
  DataListT         env;
  DSub*             pro;
  SizeT             parIx;   // ix of next parameter to put
  RefDNode          ioError; // pointer to an DNode 
  DLong             onError; // on_error setting
  BaseGDL**         catchVar;
  RefDNode          catchNode; // pointer to an DNode 
  bool              obj;       // member subroutine?
  ExtraT            extra;
  RefDNode          callingNode;
  bool              lFun; // assignment paramter for functions as l_value
  SizeT             nJump; // number of jumps in current environment
  int               lastJump; // to which label last jump went

  // stores all data which has to deleted upon destruction
  std::vector<BaseGDL*>  toDestroy;

  // finds the local variable pp points to
  int FindLocalKW( BaseGDL** pp) { return env.FindLocalKW( pp);}
  // used by the interperter returns the keyword index, used for UD functions
  int GetKeywordIx( const std::string& k);
  
public:
  // UD pro/fun
  EnvT( GDLInterpreter*, RefDNode idN, DSub* pro_, bool lF = false);
  // member procedure
  EnvT( GDLInterpreter*, RefDNode idN, BaseGDL* self, 
	const std::string& parent="");
  // member function
  EnvT( GDLInterpreter*, BaseGDL* self, 
	RefDNode idN, 
	const std::string& parent="", bool lF = false);
  // for obj_new and obj_destroy
  EnvT( EnvT* pEnv, DSub* newPro, BaseGDL** self); 

  ~EnvT()
  {
    for( std::vector<BaseGDL*>::iterator i=toDestroy.begin();
	 i != toDestroy.end(); ++i) delete *i;
  }

  SizeT NJump() { return nJump;}
  int    LastJump() { return lastJump;}
  DNode* GotoTarget( int ix)
  { 
    lastJump = ix; //static_cast<DSubUD*>( pro)->LabelOrd( ix);
    ++nJump;
    return static_cast<DSubUD*>( pro)->GotoTarget( ix);
  }
  bool LFun() const { return lFun;} // left-function

  // finds the global variable pp (used by arg_present function)
  int FindGlobalKW( BaseGDL** pp) { return env.FindGlobalKW( pp);}

  // checks if pp points to a local variable
  bool IsLocalKW( BaseGDL** pp) const { return env.InLoc( pp);}
  SizeT NewObjHeap( SizeT n=1, DStructGDL* v=NULL);
  SizeT NewHeap( SizeT n=1, BaseGDL* v=NULL);
  void FreeObjHeap( DObj id);
  void FreeHeap( DPtrGDL* p);
  DStructGDL* GetObjHeap( DObj ID);
  BaseGDL* GetHeap( DPtr ID);
  GDLInterpreter* Interpreter() const { return interpreter;}
  bool IsObject() const { return obj;}
  DSub* GetPro() const { return pro;}
  void SetIOError( int targetIx) 
  { // this isn't a jump
    if( targetIx != -1)
      ioError = static_cast<DSubUD*>( pro)->GotoTarget( targetIx)->
	getNextSibling(); 
    else
      ioError = NULL;
  }
  RefDNode GetIOError() { return ioError;}
  // the upper (calling) environment
  EnvT* Caller();
  RefDNode CallingNode() { return callingNode;}

  // called after parameter definition
  void Extra();
  friend class ExtraT;

  // used by compiler and from EnvT (for variable number of paramters)
  SizeT AddEnv()
  {
    SizeT s = env.size();
    env.resize(s+1);
    return s;
  }

  // next four are used by interpreter
  void SetNextPar( BaseGDL* const nextP); // by value (reset loc)
  void SetNextPar( BaseGDL** const nextP); // by reference (reset env)
  void SetKeyword( const std::string& k, BaseGDL* const val);  // value
  void SetKeyword( const std::string& k, BaseGDL** const val); // reference
  // used by obj_new (basic_fun.cpp)
  void PushNewEnv(  DSub* newPro, SizeT skipP, BaseGDL** newObj=NULL);
  // for exclusive use by lib::on_error
  void OnError();
  // for exclusive use by lib::catch_pro
  void Catch();

  const std::string GetFilename() const
  {
    static const std::string internal("<INTERNAL_LIB>");
    DSubUD* subUD=dynamic_cast<DSubUD*>( pro);
    if( subUD == NULL) return internal;
    return subUD->GetFilename();
  }

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
  const std::string GetString( BaseGDL*& p);



  // *************************
  // API for library functions
  // *************************

  // raise an exception from within a library function
  // automatically cares for adding line/column info and the
  // function name. 's' should be set to the 'raw' error message
  // saves some typing :-)
  void Throw( const std::string& s)
  { throw GDLException( CallingNode(), pro->ObjectName()+": "+s);}

  // 'guards' a newly created variable which should be deleted
  // upon library routines exit (normal or on error)
  // elimates the need of auto_ptr
  void Guard( BaseGDL* toGuard)
  { toDestroy.push_back( toGuard);}

  // returns environment data, by value (but that by C++ reference)
  BaseGDL*& GetKW(SizeT ix) { return env[ix];}

  // returns the ix'th parameter (NULL if not defined)
  BaseGDL*& GetPar(SizeT i);

  // get i'th parameter
  // throws if not defined (ie. never returns NULL)
  BaseGDL*& GetParDefined(SizeT i); //, const std::string& subName = "");

  // throw for STRING, STRUCT, PTR and OBJECT
  BaseGDL*& GetNumericParDefined( SizeT ix)
  {
    BaseGDL*& p0 = GetParDefined( ix);
    if( p0->Type() == STRING)
      Throw( "String expression not allowed in this context: "+GetParString(0));
    if( p0->Type() == STRUCT)
      Throw( "Struct expression not allowed in this context: "+GetParString(0));
    if( p0->Type() == PTR)
      Throw( "Pointer expression not allowed in this context: "+GetParString(0));
    if( p0->Type() == OBJECT)
      Throw( "Object reference not allowed in this context: "+GetParString(0));
    return p0;
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
    T* res = dynamic_cast<T*>( p);
    if( res != NULL) return res;
    res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    toDestroy.push_back( res);
    return res;
  }
  // same as before for keywords
  template <typename T> 
  T* GetKWAs( SizeT ix)
  {
    BaseGDL* p = GetKW( ix);
    if( p == NULL)
      Throw( "Keyword is undefined: "+GetString( ix));
    T* res = dynamic_cast<T*>( p);
    if( res != NULL) return res;
    res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    toDestroy.push_back( res);
    return res;
  }

  // next two same as last two, but return NULL if parameter/keyword is not defined
  template <typename T> 
  T* IfDefGetParAs( SizeT pIx)
  {
    BaseGDL* p = GetPar( pIx);
    if( p == NULL) return NULL;
    T* res = dynamic_cast<T*>( p);
    if( res != NULL) return res;
    res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    toDestroy.push_back( res);
    return res;
  }
  // same as before for keywords
  template <typename T> 
  T* IfDefGetKWAs( SizeT ix)
  {
    BaseGDL* p = GetKW( ix);
    if( p == NULL) return NULL;
    T* res = dynamic_cast<T*>( p);
    if( res != NULL) return res;
    res = static_cast<T*>( p->Convert2( T::t, BaseGDL::COPY));
    toDestroy.push_back( res);
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

  bool KeywordPresent( SizeT ix)
  { return(env.Loc(ix)!=NULL)||(env.Env(ix)!=NULL);}

  // local/global keyword/paramter
  bool LocalKW( SizeT ix) 
  {
    if( ix >= env.size()) return false;
    return ( env.Loc( ix) != NULL);
  }
  bool GlobalKW( SizeT ix) 
  {
    if( ix >= env.size()) return false;
    return ( env.Env( ix) != NULL);
  }
  bool LocalPar( SizeT ix) { return LocalKW( ix + pro->key.size());}
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
  void SetKW( SizeT ix, BaseGDL* newVal);
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
    T* tp= dynamic_cast<T*>(p);
    if( tp == NULL)
      Throw( "Variable must be a "+T::str+" in this context: "+
	     GetParString(pIx));
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
    T* tp= dynamic_cast<T*>(p);
    if( tp == NULL)
      Throw("Keyword must be a "+T::str+" in this context: "+
	    GetString(ix));
    if( !tp->Scalar( scalar))
      Throw("Keyword must be a scalar in this context: "+
	    GetString(ix));
  }

  void AssureGlobalPar( SizeT pIx);
  void AssureGlobalKW( SizeT ix);

  // if keyword 'kw' is not set, 'scalar' is left unchanged
  void AssureLongScalarKWIfPresent( const std::string& kw, DLong& scalar);
  // converts keyword 'kw' if necessary and sets 'scalar' 
  void AssureLongScalarKW( const std::string& kw, DLong& scalar);
  // converts ix'th keyword if necessary and sets 'scalar' 
  void AssureLongScalarKW( SizeT ix, DLong& scalar);
  // converts parameter 'ix' if necessary and sets 'scalar' 
  void AssureLongScalarPar( SizeT ix, DLong& scalar);

  // same as for Long
  void AssureDoubleScalarKWIfPresent( const std::string& kw, DDouble& scalar);
  void AssureDoubleScalarKW( const std::string& kw, DDouble& scalar);
  void AssureDoubleScalarKW( SizeT ix, DDouble& scalar);
  void AssureDoubleScalarPar( SizeT ix, DDouble& scalar);

  // same as for Long
  void AssureStringScalarKWIfPresent( const std::string& kw, DString& scalar);
  void AssureStringScalarKW( const std::string& kw, DString& scalar);
  void AssureStringScalarKW( SizeT ix, DString& scalar);
  void AssureStringScalarPar( SizeT ix, DString& scalar);

  // to be extended on demand for other data types  
};

typedef std::deque<EnvT*> EnvStackT;

#endif


