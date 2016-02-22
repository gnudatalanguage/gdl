/***************************************************************************
                          dcommon.cpp  -  GDL common blocks
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

#include "gdlexception.hpp"
#include "dcommon.hpp"
#include "str.hpp"
#include "objects.hpp"

// common block ********************************************

using namespace std;

// init with common block name
DCommon::DCommon(const string& n) : name(n)
{
  // auto insert self into list of common blocks
  // now done when subroutine compiles ok // commonList.push_back(this);
}

DCommon::~DCommon()
{
  PurgeContainer( var);
}

void DCommon::DeleteData()
{
  typename VarListT::iterator i;
  for(i = var.begin(); i != var.end(); ++i) 
    (*i)->Delete();
}

void DCommon::AddVar(const string& v)
{
  var.push_back(new DVar(v));
}

const string& DCommon::Name() const
{
  return name;
}

DVar* DCommon::Find(const string& n)
{
  return FindInVarList(var,n); 
}

int DCommon::Find(const BaseGDL* data_)
{
  int vSize=var.size();
  for( int i=0; i<vSize; i++)
    {
      if( var[i]->Data() == data_) return i;
    }
  return -1;
}

// common block reference **********************************
// init with common block
DCommonRef::DCommonRef(DCommon& c) : cRef(&c)
{
}

DCommonRef::~DCommonRef()
{
}

void DCommonRef::AddVar(const string& v)
{
  // check size
  if( cRef->NVar() == NVar()) 
    throw( GDLException("Attempt to extent common block: "+Name()));
  // add name
  varNames.push_back(v);
}

const string& DCommonRef::Name() const
{
  return cRef->Name();
}

DVar* DCommonRef::Find(const string& n)
{
  int ix=FindInIDList(varNames,n);
  if( ix == -1) return NULL;
  return cRef->Var(ix);
}

int DCommonRef::Find(const BaseGDL* data_)
{
  int ix=cRef->Find( data_);
  if( ix >= static_cast<int>(varNames.size())) return -1;
  return ix;
}

DVar* DCommonRef::Var(unsigned ix)
{
  return cRef->Var( ix);
}
