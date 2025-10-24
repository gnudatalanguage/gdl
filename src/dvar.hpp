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
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

static void defaultDVarCallback(){} //nothing
// variable type
class DVar
{
private:
  std::string     name; // the name
  BaseGDL* d;
  bool isAClone=false;
  void (*callback)();
public:
  DVar();
  DVar(const std::string& n, BaseGDL* = 0, bool isacopy=false); 
  ~DVar();
  void Delete(); // for ResetObjects() to resolve COMMON/STRUCT mutual dependency
  
  const std::string& Name() const {return name;}

  BaseGDL*& Data() 
  {
    return d;
  }
  BaseGDL* Data() const
  {
    return d;
  }
  void SetData(BaseGDL* val){d=val;}
  void SetCallback(void (*func)()){callback=func;}
  void DoCallback(){(*callback)();}
};

typedef std::vector<DVar*>        VarListT;

class DVar_eq: public std::function<bool(DVar)>
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
  
//// assignment. 
//  DVar& operator=(const BaseGDL& right)
//  {
//   std::cerr<<mtime<<std::endl;
//    assert( &right != this);
//    if( &right == this) return *this; // self assignment
//    d=right;
//    return *this;
//  }
#endif
