/***************************************************************************
                          dpro.hpp  -  GDL procedure/function
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

#ifndef DPRO_HPP_
#define DPRO_HPP_

// #include <deque>
#include <string>
#include <algorithm>
#include <vector>
//#include <stack>

#include "basegdl.hpp"
#include "dcommon.hpp"
#include "dvar.hpp"
#include "prognode.hpp"
#include "str.hpp"

#include <antlr/Token.hpp>

template<typename T>  class Is_eq: public std::unary_function<T,bool>
{
  std::string name;
public:
  explicit Is_eq(const std::string& n): name(n) {}
  bool operator() (const T* p) const 
  { return (p->Name() == name);}
};

// for jumps (goto, on_ioerror)
struct LabelT
{
  std::string label;
  ProgNodeP target;

  LabelT( const std::string& l, ProgNodeP t): label( l), target( t) {}
};

class LabelListT
{
  std::vector<LabelT> list;

public:
  LabelListT(): list() {}
  ~LabelListT() {}

  void Add( const std::string& l, ProgNodeP t)
  {
    list.push_back( LabelT( l, t));
  }
  SizeT Size() { return list.size();}
  int Find( const std::string& s)
  {
    SizeT nEl = list.size();
    for( SizeT i=0; i<nEl; i++)
      if( list[ i].label == s) return static_cast<int>(i);
    return -1;
  }
  ProgNodeP Get( SizeT ix)
  {
    return list[ ix].target;
  }
  void SetLabelNode( ProgNodeP t)
  {
    int ix = Find( t->getText()); // already checked for presence
    list[ ix].target = t;
  }
  const std::string& GetLabel( SizeT ix)
  {
    return list[ ix].label;
  }
  void Clear()
  {
    list.clear();
  }
};

// Fun/Pro classes *****************************************************
// Base class **********************************************************
class DSub
{
public:
  enum ExtraType {
    NONE=0,
    EXTRA,
    REFEXTRA
  };

protected:
  std::string name;         // name (of procedure/function)
  std::string object;       // name of class for methods (might not be defined
                            // when method is compiled)
  
  // keywords are pushed in front so they are first
  // followed by the parameters 
  // (which are pushed back,
  // but due to the syntax are first,
  // so the layout is:
  // keyVar_N (ID in key[0]),..,keyVar_1 (key[n-1]),
  // par_1,..,par_nPar, var1,..,varK
  // N=size(key)
  // K=size(var)-nPar-N
  KeyVarListT 	      key;    // keyword names (IDList: typedefs.hpp)
			      // (KEYWORD_NAME=keyword_value)
  int                 nPar;   // number of parameters (-1 = infinite)
  int                 nParMin;  // minimum number of parameters (-1 = infinite)

  ExtraType           extra;
  int                 extraIx; // index of extra keyword

  IDList              warnKey;    // keyword names to accept but warn
  // (IDList: typedefs.hpp)

public:
  DSub( const std::string& n, const std::string& o=""): 
    name(n), object(o), key(), nPar(0), nParMin(0), extra(NONE), extraIx(-1), warnKey() 
  {}

  virtual ~DSub(); // polymorphism

  const std::string& Name()   const { return name;}
  const std::string& Object() const { return object;}
  std::string  ObjectName() const
  { 
    if( object == "") return name;
    return object+"::"+name;
  }
  std::string ObjectFileName() const
  { 
    if( object == "") return name;
    return object+"__"+name;
  }

  ExtraType Extra() 
  {
    return extra;
  }

  int ExtraIx() 
  {
    return extraIx;
  }

  // returns the (abbreviated) keyword value index 
  int FindKey(const std::string& s)
  {
    String_abbref_eq searchKey(s);
    int ix=0;
    for(KeyVarListT::iterator i=key.begin();
	i != key.end(); ++i, ++ix) if( searchKey(*i)) {
	  return ix;
	}
    return -1;
  }

  int   NKey() const { return key.size();}
  int   NPar() const { return nPar;}
  int   NParMin() const { return nParMin;}

  //  bool AKey() { return aKey;} // additional keywords allowed

  friend class EnvBaseT;
  friend class EnvT;
  friend class ExtraT;
  friend void SetupOverloadSubroutines(); // overload base class methods
};

// Lib pro/fun ********************************************************
// moved to prognode.hpp
// class EnvT;
// 
// typedef void     (*LibPro)(EnvT*);
// typedef BaseGDL* (*LibFun)(EnvT*);
// typedef BaseGDL* (*LibFunDirect)(BaseGDL* param,bool canGrab);

// library procedure/function (in cases both are handled the same way)
class DLib: public DSub
{
  bool hideHelp; // if set HELP,/LIB will not list this subroutine
  
public:
  DLib( const std::string& n, const std::string& o, const int nPar_,
	const std::string keyNames[],
	const std::string warnKeyNames[], const int nParMin_);

  virtual const std::string ToString() = 0;
  
  bool GetHideHelp() const { return hideHelp;}
  void SetHideHelp( bool v) { hideHelp = v;}

  // for sorting lists by name. Not used (lists too short to make a time gain. Long lists would, if existing,
  // benefit from sorting by hash number in a std::map instead of a std::list.
  struct CompLibFunName: public std::binary_function< DLib*, DLib*, bool>
  {
    bool operator() ( DLib* f1, DLib* f2) const
    { return f1->ObjectName() < f2->ObjectName();}
  };
};

// library procedure
class DLibPro: public DLib
{
  LibPro pro;
public:
  // warnKeyNames are keywords wich are not supported, but which
  // should not make the compilation fail.
  // A warning will be issued.
  // use this for keywords which are truly optional and don't
  // change the results.
  // Note that due to their nature, there should never be keywords
  // on which a value is returned.
  DLibPro( LibPro p, const std::string& n, const int nPar_=0, 
	   const std::string keyNames[]=NULL,
	   const std::string warnKeyNames[]=NULL, const int nParMin_=0);

  DLibPro( LibPro p, const std::string& n, const std::string& o, 
	   const int nPar_=0, 
	   const std::string keyNames[]=NULL,
	   const std::string warnKeyNames[]=NULL, const int nParMin_=0);

  LibPro Pro() { return pro;}

  const std::string ToString();
};

// library function
class DLibFun: public DLib
{
  LibFun fun;
public:
  DLibFun( LibFun f, const std::string& n, const int nPar_=0, 
	   const std::string keyNames[]=NULL,
	   const std::string warnKeyNames[]=NULL, const int nParMin_=0);

  DLibFun( LibFun f, const std::string& n, const std::string& o, 
	   const int nPar_=0, 
	   const std::string keyNames[]=NULL,
	   const std::string warnKeyNames[]=NULL, const int nParMin_=0);

  LibFun Fun() { return fun;}

  const std::string ToString();

  virtual bool RetNew() { return false;}
  virtual bool DirectCall() { return false;}
};

// library function which ALWAYS return a new value
// (as opposite to returning an input value)
class DLibFunRetNew: public DLibFun
{
  bool   retConstant; // means: can be pre-evaluated with constant input 
public:
  DLibFunRetNew( LibFun f, const std::string& n, const int nPar_=0, 
		 const std::string keyNames[]=NULL,
		 const std::string warnKeyNames[]=NULL, bool rConstant=false, const int nParMin_=0);


  DLibFunRetNew( LibFun f, const std::string& n, const std::string& o, 
		 const int nPar_=0, 
		 const std::string keyNames[]=NULL,
		 const std::string warnKeyNames[]=NULL, const int nParMin_=0);

  bool RetNew() { return true;}
  bool RetConstant() { return this->retConstant;}
};

// direct call functions must have:
// ony one parameter, no keywords
// these functions are called "direct", no environment is created
class DLibFunDirect: public DLibFunRetNew
{
  LibFunDirect funDirect;
public:
  DLibFunDirect( LibFunDirect f, const std::string& n, bool retConstant_=true);

  LibFunDirect FunDirect() { return funDirect;}

//   bool RetNew() { return true;}
  bool DirectCall() { return true;}
};

// UD pro/fun ********************************************************
// function/procedure (differ because they are in different lists)
// User Defined
class DSubUD: public DSub 
{
  std::string         file;        // filename were procedure is defined in

  KeyVarListT         var;         // keyword values, parameters, local variables

  CommonBaseListT     common;      // common blocks or references 
  ProgNodeP           tree;        // the 'code'
  unsigned int                 compileOpt;  // e.g. hidden or obsolete

  LabelListT          labelList;

  void ResolveLabel( ProgNodeP);

protected:
  int nForLoops;

public:
  DSubUD(const std::string&,const std::string& o="",const std::string& f="");
  ~DSubUD(); 

  void Reset();
  void DelTree();
  void SetTree( ProgNodeP t) { tree = t;}

  void AddCommon(DCommonBase* c) { common.push_back(c);}
  void DeleteLastAddedCommon(bool kill=true)
  {
    if(kill) delete common.back();
    common.pop_back();
  }
    void commonPtrs( std::vector<DCommonBase *>& cptr)
{
	cptr.clear();
    CommonBaseListT::iterator c = common.begin();
    for(; c != common.end(); ++c)
       cptr.push_back((*c));
    return;
	}
  
  void ResolveAllLabels();
  LabelListT& LabelList() { return labelList;}
  ProgNodeP GotoTarget( int ix) 
  { 
    if( ix >= labelList.Size())
      throw GDLException( "Undefined label.");
    return labelList.Get( ix);
  } 
//   int LabelOrd( int ix) { return labelList.GetOrd( ix);} 
  int NDefLabel() { return labelList.Size();} 

  std::string GetFilename()
  {
    return file;
  }

  // add variables
  DSubUD*  AddPar(const std::string&); // add paramter
  unsigned AddVar(const std::string&); // add local variable
  DSubUD*  AddKey(const std::string&, const std::string&); // add keyword=value

  void     DelVar(const int ix) {var.erase(var.begin() + ix);}

   void Resize( SizeT s) { var.resize( s);}
  SizeT Size() {return var.size();}
  SizeT CommonsSize() {
   SizeT commonsize=0;
   CommonBaseListT::iterator c = common.begin();
   for(; c != common.end(); ++c) commonsize+=(*c)->NVar();
   return commonsize;
   }
  
  int NForLoops() const { return nForLoops;}
  
  // search for variable returns true if its found in var or common blocks
  bool Find(const std::string& n)
  {
    KeyVarListT::iterator f=std::find(var.begin(),var.end(),n);
    if( f != var.end()) return true;

    CommonBaseListT::iterator c=
      std::find_if(common.begin(),common.end(),DCommon_contains_var(n));

    return (c != common.end());
  }

  // returns common block with name n
  DCommon* Common(const std::string& n)
  {
    CommonBaseListT::iterator c = common.begin();
    for(; c != common.end(); ++c)
      if( dynamic_cast< DCommon*>( *c) != NULL && (*c)->Name() == n)
	return static_cast< DCommon*>( *c);
    return NULL;
  }

  // returns common block which holds variable n
  DCommonBase* FindCommon(const std::string& n)
  {
    CommonBaseListT::iterator c=
      std::find_if(common.begin(),common.end(),DCommon_contains_var(n));

    return (c != common.end())? *c : NULL;
  }

void ReName( SizeT ix, const std::string& s)
  {
    var[ix] = s;
    return;
  }
  const std::string& GetVarName( SizeT ix)
  {
    return var[ix];
  }

  const std::string& GetKWName( SizeT ix)
  {
    return key[ix];
  }

  BaseGDL* GetCommonVarNameList();
  bool GetCommonVarName(const BaseGDL* p, std::string& varName);
  bool GetCommonVarName4Help(const BaseGDL* p, std::string& varName);

  BaseGDL** GetCommonVarPtr(const BaseGDL* p)
  {
    for( CommonBaseListT::iterator c=common.begin();
	 c != common.end(); ++c)
      {
	int vIx = (*c)->Find( p);
	if( vIx >= 0) 
	  {
	    DVar* var = (*c)->Var( vIx);
	    return &(var->Data());
	  }
      }
    return NULL;
  }
  
  BaseGDL** GetCommonVarPtr(std::string& s)
  {
    for(CommonBaseListT::iterator c=common.begin();
   	c != common.end(); ++c)
      {
       	DVar* v=(*c)->Find(s);
       	if (v) return &(v->Data());
      }
    return NULL;
  }
  
  bool ReplaceExistingCommonVar(std::string& s, BaseGDL* val)
  {
    for(CommonBaseListT::iterator c=common.begin();
   	c != common.end(); ++c)
      {
       	DVar* v=(*c)->Find(s);
       	if (v) { 
         delete (v)->Data();
         (v)->SetData(val);
         return true;
        }
      }
    return false;
  }  
  // returns the variable index (-1 if not found)
  int FindVar(const std::string& s)
  {
    return FindInKeyVarListT(var,s);
  }

  // returns ptr to common variable (NULL if not found)
  DVar* FindCommonVar(const std::string& s) 
  { 
    for(CommonBaseListT::iterator c=common.begin();
   	c != common.end(); ++c)
      {
       	DVar* v=(*c)->Find(s);
       	if( v) return v;
      }
    return NULL;
  }

  // the final "compilation" takes part here 
  void SetTree( RefDNode n);
//   {
//     //
//     // Converter Translation Transcription Rewrite RefDNode ProgNode ProgNodeP
//     //
//     // here the conversion RefDNode -> ProgNode is done
//     //
//     tree = ProgNode::NewProgNode( n);
//   }
  
  ProgNodeP GetTree()
  {
    return tree;
  }

  unsigned int GetCompileOpt() { return compileOpt; }
  void SetCompileOpt(const unsigned int n) { compileOpt = n; }
  bool isObsolete();
  bool isHidden();
  bool isStatic();

  friend class EnvUDT;
};

class DPro: public DSubUD
{
public:
  // for main function, not inserted into proList
  // should be fine (way too much): 32 NESTED loops in $MAIN$ (elswhere: unlimited)
  DPro(): DSubUD("$MAIN$","","") { this->nForLoops = 32;}

  DPro(const std::string& n,const std::string& o="",const std::string& f=""): 
    DSubUD(n,o,f)
  {}
  
  const std::string ToString();


  ~DPro() {}; 
};

class DFun: public DSubUD
{
public:
  DFun(const std::string& n,const std::string& o="",const std::string& f=""): 
    DSubUD(n,o,f)
  {} 

  const std::string ToString();

  ~DFun() {};
};


typedef std::vector<DFun*> FunListT;
typedef std::vector<DPro*> ProListT;

typedef std::vector<DLibFun*> LibFunListT;
typedef std::vector<DLibPro*> LibProListT;

#endif
