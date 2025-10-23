/***************************************************************************
                          dvar.cpp  -  GDL system and common block variables
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

#include <algorithm>

#include "dvar.hpp"

using namespace std;

DVar::DVar(const string& n, BaseGDL* data, bool isacopy) : //isacopy to tell if this DVAR just points to another (used in some cases. These DVars MUST NOT be destroyed!)
  name(n), d(data),callback(defaultDVarCallback),isAClone(isacopy)
{}

DVar::DVar() : name(), d(0) ,callback(defaultDVarCallback)
{}

DVar::~DVar() 
{
  // Note: !NULL would be naturally destroyed from here at program end
  // we explicitely preventing the deletion to be able to flag possible
  // other destructions of !NULL (which are bugs)
  //if( d != NullGDL::GetSingleInstance()) 
  if (!isAClone) GDLDelete(d);
  name.clear();
  d=NULL;
  }

void DVar::Delete() // for ResetObjects() to resolve COMMON/STRUCT mutual dependency
{
if (!isAClone)  GDLDelete( d);
  name.clear();
  d = NULL;
}

DVar* FindInVarList(VarListT& v, const string& s)
{
  VarListT::iterator f=find_if(v.begin(),v.end(),DVar_eq(s));
  if( f == v.end()) return NULL;
  return *f;
}

DVar* FindInVarList(VarListT& v, BaseGDL* p)
{
  VarListT::iterator f=find_if(v.begin(),v.end(),DVar_eq(p));
  if( f == v.end()) return NULL;
  return *f;
}


