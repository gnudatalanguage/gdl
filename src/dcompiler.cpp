/***************************************************************************
                          dcompiler.cpp  -  compiler functions
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

#include <iostream>
#include <stdio.h>

#include "str.hpp"
#include "gdlexception.hpp"
#include "objects.hpp"
#include "dcompiler.hpp"
#include "str.hpp"
#include "dcommon.hpp"
#include "GDLInterpreter.hpp"

// print out AST tree
//#define GDL_DEBUG
#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include "print_tree.hpp"
#endif

using namespace std;

DCompiler::DCompiler(const string& f, EnvT* e, const std::string& sub)
  : actualFile(f), subRoutine( sub), env(e), pro(NULL), 
    activeProCompiled( false)
{
  // interactive mode?
  if( env != NULL) pro=dynamic_cast<DSubUD*>(env->GetPro());
}

DCompiler::~DCompiler() 
{ 
  if( env != NULL) 
    {
      if( pro != static_cast<DSubUD*>(env->GetPro()))
	delete pro; // delete if not env's pro
    }
  else
    delete pro; // NULL or failed to compile function/procedure
}

void DCompiler::ForwardFunction(const string& s) // add to function list
{
  new DFun(s);
}

void DCompiler::AddPar(const string& P)          // add parameter
{
  if( pro->Find(P))
    throw(GDLException(P+" is already defined with a conflicting definition."));
  pro->AddPar(P);
}

void DCompiler::AddKey(const string& K,const string& V)   // add keyword,valName
{
  if( pro->FindKey(K) != -1) 
    throw(GDLException("Ambiguous keyword: "+K));
  if( pro->Find(V))
    throw(GDLException(V+" is already defined with a conflicting definition."));
  pro->AddKey(K,V);
}

void DCompiler::EndFunPro()   // resolve gotos
{
  const string& name=pro->ObjectName();

  if( labelList.size() > 0)
    {
      LabelListT& ll = pro->LabelList();
      
      for(map<string,deque<RefDNode> >::iterator i=labelList.begin(); 
	  i != labelList.end(); i++)
	{
	  const string& gotoLabel = (*i).first;

	  int proLabelIx = ll.Find( gotoLabel);
	  if( proLabelIx == -1)
	    throw( GDLException(name+": Undefined label "+gotoLabel+
				" referenced in GOTO statement."));
	  
	  deque<RefDNode>& gotoNodes = (*i).second;
	  for( deque<RefDNode>::iterator gN = gotoNodes.begin();
	       gN != gotoNodes.end(); ++gN)
	    {
	      (*gN)->SetGotoIx( proLabelIx);
	    }
	}

      // clear for next subroutine
      labelList.clear();
    }
}

void DCompiler::StartPro(const string& n,const string& o)
{
  pro = new DPro(n,o,actualFile);
}

void DCompiler::StartFun(const string& n,const string& o)
{
  pro = new DFun(n,o,actualFile);
}

bool DCompiler::IsActivePro( DSub* p)
{
  EnvStackT& cS = GDLInterpreter::CallStack();
  SizeT stSz = cS.size();
  for( SizeT i=1; i<stSz; ++i) // i=1: skip $MAIN$
    if( cS[ i]->GetPro() == p) return true;
  return false;
}

void DCompiler::EndPro() // inserts in proList
{
  EndFunPro();

  const string& name=pro->Name();
  const string& o=pro->Object();

  ProListT *searchList;
  if( o == "")
    searchList= &proList;
  else
    {
      // find struct 'id'
      DStructDesc* dStruct=FindInStructList( structList, o);
      if( dStruct == NULL) 
	{
	  dStruct=new DStructDesc( o);
	  structList.push_back( dStruct);
	}
      searchList = &dStruct->ProList();
    }

  // search/replace in proList
  ProListT::iterator p=find_if((*searchList).begin(),(*searchList).end(),
			       Is_eq<DPro>(name));
  if( p != (*searchList).end()) 
    {
      if( *p != NULL)
	{
	  delete *p;
	  if( IsActivePro( *p))
	    {
	      Warning( "Procedure was compiled while active: "+
		       pro->ObjectName()+". Returning.");
	      activeProCompiled = true;
	    }
	}
      *p=static_cast<DPro*>(pro);
    }
  else
    (*searchList).push_back(static_cast<DPro*>(pro));
  
  if( subRoutine == "" || subRoutine == pro->ObjectFileName())
    Message( "Compiled module: "+pro->ObjectName()+"."); 

  // reset pro // will be deleted otherwise
  if( env != NULL) pro=dynamic_cast<DSubUD*>(env->GetPro()); else pro=NULL;
}

void DCompiler::EndFun() // inserts in funList
{
  EndFunPro();

  const string& name=pro->Name();
  const string& o=pro->Object();

  FunListT *searchList;
  if( o == "")
    searchList= &funList;
  else
    {
      // find struct 'id'
      DStructDesc* dStruct=FindInStructList( structList, o);
      if( dStruct == NULL) 
	{
	  dStruct=new DStructDesc( o); 
	  structList.push_back( dStruct);
	}
      searchList = &dStruct->FunList();
    }

  // search/replace in funList
  FunListT::iterator p=find_if((*searchList).begin(),(*searchList).end(),
			       Is_eq<DFun>(name));
  if( p != (*searchList).end()) 
    {
      if( *p != NULL)
	{
	  delete *p;
	  if( IsActivePro( *p))
	    {
	      Warning( "Procedure was compiled while active: "+
		       pro->ObjectName()+". Returning.");
	      activeProCompiled = true;
	    }
	}
      *p=static_cast<DFun*>(pro);
    }
  else
    (*searchList).push_back(static_cast<DFun*>(pro));

  if( subRoutine == "" || subRoutine == pro->ObjectFileName())
    Message( "Compiled module: "+pro->ObjectName()+"."); 

  // reset pro // will be deleted otherwise
  if( env != NULL) pro=dynamic_cast<DSubUD*>(env->GetPro()); else pro=NULL;
}

DCommon* DCompiler::Common(const string& n) // returns common block with name n
{
  CommonListT::iterator f=find_if(commonList.begin(),
				  commonList.end(),
				  DCommon_eq(n));
  if( f != commonList.end()) return *f;
  return NULL;
}
  
// Common block (re)definition (variables provided)
DCommonBase* DCompiler::CommonDef(const string& N) 
{
  // search for common block
  DCommonBase* c=Common(N);
  if( !c) 
    { // not there -> create new
      c=new DCommon(N);
    }
  else 
    { // already there -> create reference
      c=new DCommonRef( *(static_cast<DCommon*>(c)));
    }
  // variables will be checked when parsed (in CommonVar())
  pro->AddCommon(c);
  return c;
}

// Common block declaration (no variables provided)
void DCompiler::CommonDecl(const string& N) 
{
  // search for common block
  DCommon* c=Common(N);
  if( !c)
    {
      throw( GDLException("Common block: "+N+" must contain variables."));
    }
  // check variables
  unsigned nVar=c->NVar();
  for(unsigned u=0; u < nVar; u++)
    {
      DVar* cVar=c->Var(u); // variable from common block
      if( pro->Find(cVar->Name()))
	{
	  throw( GDLException("Variable: "+cVar->Name()+
			      " ("+N+") already defined"
			      " with a conficting definition."));
	}
    }
  pro->AddCommon(c);
}

void DCompiler::CommonVar(DCommonBase* c, const string& N)
{
  bool v=pro->Find(N);
  if( v) 
    {
      throw( GDLException("Variable: "+N+" ("+c->Name()+") already defined"
			  " with a conficting definition."));
    }
  c->AddVar(N);
}

RefDNode DCompiler::ByReference(RefDNode n)
{
  static RefDNode null = static_cast<RefDNode>(antlr::nullAST);

  // expressions (braces) are ignored
  while( n->getType() == EXPR) n = n->getFirstChild();
  int t=n->getType();

  // only var, common block var and deref ptr are passed by reference
  if( t != VAR && t != VARPTR && t != DEREF) return null; 

  return n;
}  

bool DCompiler::IsVar(const string& n) const
{
  // check if lib fun
  SizeT nLibF = libFunList.size();
  for( SizeT f=0; f<nLibF; ++f)
    if( libFunList[ f]->Name() == n) return false;

  return pro->Find(n);
}

// variable (parameter, keyword-value) reference
void DCompiler::Var(RefDNode n)
{
    string V=n->getText();
    int i=pro->FindVar(V);
    if( i != -1) 
	{
	    n->SetVarIx(i);
	    return;
	}
    DVar *c=pro->FindCommonVar(V);
    if( c != NULL) 
	{
	    n->setType(VARPTR);
	    n->SetVar(c);
	    return;
	}
    SizeT u=pro->AddVar(V);
    if( env != NULL)
      {
	SizeT e=env->AddEnv();
	if( u != e)
	  throw GDLException("env and pro out of sync."); 
      }
    n->SetVarIx(static_cast<int>(u));
}

void DCompiler::SysVar(RefDNode n)
{
  n->SetVar(NULL);
}

void DCompiler::SetTree(RefDNode n)
{
#ifdef GDL_DEBUG
  cout << "Setting procedure/function tree:" << endl;
  if( pro != NULL)
    {
      cout << "Subroutine: " << pro->Name() << endl;
    }
  else cout << "pro == NULL" << endl;

  antlr::print_tree pt;
  pt.pr_tree(static_cast<antlr::RefAST>( n));
  cout << endl;
#endif

  pro->SetTree(n);
}

void DCompiler::Label(RefDNode n)
{
  LabelListT& ll = pro->LabelList();

  string lab=n->getText();

  int ix = ll.Find( lab);

  if( ix != -1)
    {
      throw( GDLException(n, "Label "+lab+" defined more than once."));
    }
  else
    {
      ll.Add( lab, n);
    }
}

void DCompiler::Goto(RefDNode n)
{
  LabelListT& ll = pro->LabelList();

  string lab=n->getText();

  int ix = ll.Find( lab);

  if( ix != -1)
    {
      n->SetGotoIx( ix);
    }
  else
    {
      deque<RefDNode>& nList=labelList[lab]; // inserts if not exist
      nList.push_back(n); // put node in reminder list
    }
}

// used by treeparser for return statements
bool DCompiler::IsFun() const
{
  return (dynamic_cast<DFun*>(pro) != NULL);
}

int DCompiler::NDefLabel()
{
  return pro->NDefLabel();
}
