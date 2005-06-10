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

DVar::DVar(const string& n, BaseGDL* data) :
  name(n), d(data)
{}

DVar::DVar() : name(), d(0) 
{}

DVar::~DVar() 
{
  if( d != NULL) delete d;
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


