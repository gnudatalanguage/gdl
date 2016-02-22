/***************************************************************************
                          dvar.hpp  -  GDL common block and system variables
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

#ifndef DVAR_HPP_
#define DVAR_HPP_

#include <string>
#include <deque>
#include <functional>

#include "basegdl.hpp"


// variable type
class DVar
{
private:
  std::string     name; // the name
  BaseGDL* d;

public:
  DVar();
  DVar(const std::string& n, BaseGDL* = 0); 
  ~DVar();

  const std::string& Name() const {return name;}

  void Delete() // for ResetObjects() to resolve COMMON/STRUCT mutual dependency
  {
    GDLDelete( d);
    d = NULL;
  }
  BaseGDL*& Data() 
  {
    return d;
  }
  BaseGDL* Data() const
  {
    return d;
  }
};

typedef std::vector<DVar*>        VarListT;

class DVar_eq: public std::unary_function<DVar,bool>
{
  std::string name;
  BaseGDL*  pp;
public:
  explicit DVar_eq(const std::string& s): name(s), pp(NULL) {}
  explicit DVar_eq( BaseGDL* p): name(), pp(p) {}
  bool operator() ( DVar* v) const 
  { 
    if( pp != NULL) return v->Data() == pp;
    return v->Name() == name;
  }
};

DVar* FindInVarList(VarListT&, const std::string& name);
DVar* FindInVarList(VarListT&, BaseGDL* p);

#endif
