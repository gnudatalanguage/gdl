/***************************************************************************
                          dcompiler.hpp  -  GDL compiler
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

#ifndef compiler_hpp_
#define compiler_hpp_

#include <fstream>
#include <string>
#include <deque>
#include <map>

#include "GDLLexer.hpp"
#include "GDLParser.hpp"

#include "gdlexception.hpp"
#include "objects.hpp"
#include "dcommon.hpp"
#include "dnode.hpp"

class GDLInterpreter;

class DCompiler: public GDLTokenTypes
{
  std::string actualFile;         // actual compiled file (dbg info)
  std::string subRoutine;         // pro/fun to compile (used for messages only)

  EnvBaseT   *env;                // intial processed environment (interactive)
  DSubUD *pro;                    // actual processed pro/function

  bool            activeProCompiled;
  int             nCompileErrors; // number of compilation errors

  //  std::map<std::string,std::deque<RefDNode> > labelList; // resolving goto/label
  CommonListT   ownCommonList; // common blocks not yet added to commonList

  DCommon* Common(const std::string& n); // returns common block with name n

  void ClearOwnCommon();

  ProgNodeP tree;

public:
  // should be private, but GDLTreeParser needs it to be public
  DCompiler() 
  { std::cerr<<"DCompiler() called. Abort."<<std::endl; exit( EXIT_FAILURE);}

public:
  DCompiler(const std::string& f, EnvBaseT* e, const std::string& sub);
  ~DCompiler();

  // mainly, every language element should have its corresponding function
  void ForwardFunction(const std::string&);        // add to function list
  void AddPar(const std::string&);                 // add parameter
  void AddKey(const std::string&,const std::string&);   // add keyword,varName
  void StartPro(const std::string&, const int compileOpt = 0, const std::string& o = "", const RefDNode semiCompiledTree=NULL);     
  void ContinueMainPro();
  void EndFunPro();       
  void EndInteractiveStatement();
  void EndPro();       
  void StartFun(const std::string&, const int compileOpt, const std::string& o = "", const RefDNode semiCompiledTree=NULL);
  void EndFun();  
  void SetClass(const std::string&);      // set procedure to member (also add 'self')
  DCommonBase* CommonDef(const std::string&); // Common block (re)definition
  void CommonDecl(const std::string&);        // Common block declaration
  void CommonVar(DCommonBase*, const std::string&); // Common block variable
  BaseGDL* ConstantIndex( RefDNode n);
  RefDNode ByReference(RefDNode n_);  // for parameter passing 
  //  bool ByReferenceMaybe(RefDNode n_); // for parameter passing 
  bool IsVar(const std::string&); // variable already defined in actual context?
  void Var(RefDNode);    // sets var in node
  void SysVar(RefDNode); // sets var in node
  void SetTree(RefDNode);
  void Label(RefDNode);
  void Goto(RefDNode);
  bool IsFun() const;
  int  NDefLabel();
  bool IsActivePro( DSub* p);
  bool ActiveProCompiled() const { return activeProCompiled;}
  int NCompileErrors() const { return nCompileErrors;}
  EnvBaseT* GetEnv() const { return env;}
  void SetEnv( EnvBaseT* e) { env = e;}
  // for sorting lists by name
  struct CompLibFunName: public std::function<bool(DLibFun*, DLibFun*)>
  {
    bool operator() ( DLibFun* f1, DLibFun* f2) const
    { return f1->ObjectName() < f2->ObjectName();}
  };
};

#endif
