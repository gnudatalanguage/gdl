/***************************************************************************
                          dcompiler.cpp  -  compiler functions
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
//#undef GDL_DEBUG

#ifdef GDL_DEBUG
#include "print_tree.hpp"
#endif

#include <assert.h>

using namespace std;

DCompiler::DCompiler(const string& f, EnvBaseT* e, const std::string& sub)
  : actualFile(f), subRoutine( sub), env(e), pro(NULL), 
    activeProCompiled( false), tree( NULL)
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

  ClearOwnCommon();
}

void DCompiler::ClearOwnCommon()
{
  // delete common blocks which are not added to commonList
  // due to compile error
  CommonListT::iterator it;
  for( it=ownCommonList.begin(); it !=ownCommonList.end(); ++it)
    {
      delete *it;
    }
  ownCommonList.clear();
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

void DCompiler::EndFunPro()   // resolve gotos, add common blocks
{
  pro->ResolveAllLabels();

//   // fill the subroutines label list
//   // we already have all labels in labellist

//   LabelListT& ll = pro->LabelList();

//   if( labelList.size() > 0)
//     {
//       // label - target DNode* (other type than this->labelList)
//       LabelListT& ll = pro->LabelList();
      
//       for(map<string,deque<RefDNode> >::iterator i=labelList.begin(); 
// 	  i != labelList.end(); i++)
// 	{
// 	  const string& gotoLabel = (*i).first;

// 	  int proLabelIx = ll.Find( gotoLabel);
// 	  if( proLabelIx == -1)
// 	    throw( GDLException(pro->ObjectName()+
// 				": Undefined label "+gotoLabel+
// 				" referenced in GOTO statement."));
	  
// 	  // set the nodes of the actual tree to the appropiate index
// 	  deque<RefDNode>& gotoNodes = (*i).second;
// 	  for( deque<RefDNode>::iterator gN = gotoNodes.begin();
// 	       gN != gotoNodes.end(); ++gN)
// 	    {
// 	      (*gN)->SetGotoIx( proLabelIx);
// 	    }
// 	}

//       // clear for next subroutine
//       labelList.clear();
//     }

  for( CommonListT::iterator c = ownCommonList.begin();
       c != ownCommonList.end(); ++c)
    commonList.push_back( *c);
  ownCommonList.clear(); // not responsible anymore
}

void DCompiler::EndInteractiveStatement() // add common blocks
{
  for( CommonListT::iterator c = ownCommonList.begin();
       c != ownCommonList.end(); ++c)
    commonList.push_back( *c);
  ownCommonList.clear(); // not responsible anymore
}

void DCompiler::StartPro(const string& n,const string& o)
{
  ClearOwnCommon();
  if( n != "$MAIN$" || o != "")
    {
      pro = new DPro(n,o,actualFile);
    }
  else
    {
      env = GDLInterpreter::CallStack()[0];
      pro = static_cast<DSubUD*>( env->GetPro());
      pro->DelTree();
    }
}

void DCompiler::StartFun(const string& n,const string& o)
{
  ClearOwnCommon();
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

  if( name != "$MAIN$" || o != "")
    {
      
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
    }

  if( subRoutine == "" || subRoutine == pro->ObjectFileName())
    Message( "Compiled module: "+pro->ObjectName()+"."); 

  // reset pro // will be deleted otherwise
  if( env != NULL) 
    pro=dynamic_cast<DSubUD*>(env->GetPro()); 
  else 
    pro=NULL;
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

// returns common block with name n
DCommon* DCompiler::Common(const string& n) 
{
  CommonListT::iterator f=find_if(commonList.begin(),
				  commonList.end(),
				  DCommon_eq(n));
  if( f != commonList.end()) return *f;
  
  return NULL; //pro->Common( n);
}
  
// Common block (re)definition (variables provided)
DCommonBase* DCompiler::CommonDef(const string& N) 
{
  // search for common block
  DCommonBase* c=Common(N);

  // look also in actual subroutine
  if( c == NULL) 
      c = pro->Common( N);

  if( c == NULL) 
    { 
      // not there -> create new
      c=new DCommon(N);
      ownCommonList.push_back( static_cast<DCommon*>(c));
    }
  else 
    { // already there -> create reference
      c=new DCommonRef( *(static_cast<DCommon*>(c)));
    }
  // variables will be checked when parsed (in CommonVar())
  pro->AddCommon(c);
  return c;
}

void DCompiler::CommonVar(DCommonBase* c, const string& N)
{
  if( pro->Find(N))
    {
      DCommonBase* c1st = pro->FindCommon( N);
      // several definition/declaration of the same common block are ok
      if( c1st == NULL || c1st->Name() != c->Name())
	throw( GDLException("Variable: "+N+" ("+c->Name()+") already defined"
			    " with a conficting definition."));
    }
  c->AddVar(N);
}

// Common block declaration (no variables provided)
void DCompiler::CommonDecl(const string& N) 
{
  // search for common block
  DCommon* c=Common(N);
  if( c == NULL)
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
	  DCommonBase* c1st = pro->FindCommon( cVar->Name());
	  // several definition/declaration of the same common block are ok
	  if( c1st == NULL || c1st->Name() != c->Name())
	    throw( GDLException("Variable: "+cVar->Name()+
				" ("+N+") already defined"
				" with a conficting definition."));
	}
    }
  pro->AddCommon(c);
}

BaseGDL* DCompiler::Constant( RefDNode n)
{
  if( n->getType() == CONSTANT)
    return n->CData();
  return NULL;
}

RefDNode DCompiler::ByReference(RefDNode nIn)
{
  static RefDNode null = static_cast<RefDNode>(antlr::nullAST);

// #ifdef GDL_DEBUG
//   cout << "ByReference: in:" << endl;
//   antlr::print_tree pt;
//   pt.pr_tree(static_cast<antlr::RefAST>( nIn));
//   cout << endl;
// #endif

  RefDNode n = nIn;

  // APRO,++(a=2) is passed like APRO,a
  // APRO,(((a=2))=3) // forbidden in GDL
  // Makes IDL segfault: APRO,++(((a=2))=3)

  // expressions (braces) are ignored
  while( n->getType() == EXPR) n = n->getFirstChild();
  int t=n->getType();
  if( t == DEC || t == INC) // only preinc can be reference
    {
      n = n->getFirstChild();
      int t=n->getType();
    }

  // expressions (braces) are ignored
  while( n->getType() == EXPR) n = n->getFirstChild();
  t=n->getType();
  bool assignReplace = false;
  if( t == ASSIGN_REPLACE)
    {
      assignReplace = true;
      n = n->getFirstChild()->getNextSibling();
      int t=n->getType();
    }

  // expressions (braces) are ignored
  while( n->getType() == EXPR) n = n->getFirstChild();
  t=n->getType();

  // only var, common block var and deref ptr are passed by reference
  // *** see AssignReplace(...)
  if( !assignReplace && t != VAR && t != VARPTR && t != DEREF) return null; 

// #ifdef GDL_DEBUG
//   cout << "ByReference: out:" << endl;
//   pt.pr_tree(static_cast<antlr::RefAST>( n));
//   cout << endl;
// #endif

  return n;
}  

bool DCompiler::IsVar(const string& n) const
{
  // check if lib fun
  SizeT nLibF = libFunList.size();
  for( SizeT f=0; f<nLibF; ++f)
    if( libFunList[ f]->Name() == n) return false;

  if( FunIx( n) != -1) return false;

  // originally this was done later in the interpreter
  // but something like a = a(0) would not work if a is
  // a function (defined in a.pro) and a variable
  bool success = GDLInterpreter::SearchCompilePro( n);
  if( success) // even if file exists and compiles it might contain other stuff
    if( FunIx( n) != -1) return false;

  // Note: It is still possible that 'n' denotes a function:
  // !PATH might be changed untill the interpretation

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
  assert( pro != NULL);

#ifdef GDL_DEBUG
  cout << "Setting procedure/function tree:" << endl;
  antlr::print_tree pt;
  pt.pr_tree(static_cast<antlr::RefAST>( n));
  cout << endl;
#endif

  pro->SetTree( n);
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
      ll.Add( lab, NULL); // insert first without node
    }
}

// void DCompiler::Goto(RefDNode n)
// {
//   LabelListT& ll = pro->LabelList();

//   string lab=n->getText();

//   int ix = ll.Find( lab);

//   if( ix != -1)
//     {
//       n->SetGotoIx( ix);
//     }
//   else
//     {
//       deque<RefDNode>& nList=labelList[lab]; // inserts if not exist
//       nList.push_back(n); // put node in reminder list
//     }
// }

// used by treeparser for return statements
bool DCompiler::IsFun() const
{
  return (dynamic_cast<DFun*>(pro) != NULL);
}

int DCompiler::NDefLabel()
{
  return pro->NDefLabel();
}
