/***************************************************************************
                          dcommon.hpp  -  GDL common blocks
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

#ifndef DCOMMON_HPP_
#define DCOMMON_HPP_

#include "gdlexception.hpp"
#include "dvar.hpp"

class DCommonBase 
{
public:
  virtual ~DCommonBase() {}              
  virtual const std::string& Name() const =0; 
  virtual unsigned NVar() const =0;      
  virtual void AddVar(const std::string&)=0; 
  virtual DVar* Find(const std::string&)=0;
  virtual int Find(const BaseGDL*)=0;
  virtual const std::string& VarName(const unsigned i)=0;
  virtual DVar* Var(unsigned ix)=0;
};

// common block *******************************************************
class DCommon: public DCommonBase
{
private: 
  std::string     name;	// common block name
  VarListT        var;       // the data
public:
  DCommon(const std::string&);
  ~DCommon();

  const std::string& VarName(const unsigned i)
  {
    if( i >= var.size())
      throw(GDLException("Common block index out of range"));
    return var[i]->Name();
  }
  unsigned NVar() const { return var.size();}
  void     AddVar(const std::string&);
  const std::string& Name() const;
  DVar* Find(const std::string&);
  int Find(const BaseGDL*);
  DVar* Var(unsigned ix) { return var[ix];}
};

class DCommon_eq: public std::unary_function<DCommon,bool>
{
  std::string name;
public:
  explicit DCommon_eq(const std::string& s): name(s) {}
  bool operator() (const DCommon* v) const { return v->Name() == name;}
};

// ref to common block (variable names (and number) might differ) *****
class DCommonRef: public DCommonBase
{
private:
  IDList  varNames;     // overridden variable names 
  DCommon *cRef;        // ptr to common blk
public:
  DCommonRef(DCommon&);
  ~DCommonRef();

  const std::string& VarName(const unsigned i)
  {
    if( i >= varNames.size())
      throw(GDLException("CommonRef block index out of range"));
    return varNames[i];
  }
  unsigned NVar() const { return varNames.size();}
  void   AddVar(const std::string&);
  const std::string& Name() const;
  DVar* Find(const std::string&);
  int Find(const BaseGDL*);
  DVar* Var(unsigned ix);
};

typedef std::vector<DCommonBase*> CommonBaseListT;
typedef std::vector<DCommon*>     CommonListT;

class DCommon_contains_var: public std::unary_function<DCommonBase,bool>
{
  std::string name;
public:
  explicit DCommon_contains_var(const std::string& s): name(s) {}
  bool operator() (DCommonBase* c) const { return (c->Find(name) != NULL);}
};

#endif
