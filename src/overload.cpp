/***************************************************************************
                       overload.cpp  -  GDL operator overloading for objects
                             -------------------
    begin                : November 29 2012
    copyright            : (C) 2012 by Marc Schellens
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

#include "objects.hpp"
#include "overload.hpp"
#include "prognodeexpr.hpp"

std::string overloadOperatorNames[] = 
{
  "_OVERLOADBRACKETSLEFTSIDE"
, "_OVERLOADBRACKETSRIGHTSIDE" 
, "_OVERLOADMINUSUNARY"
, "_OVERLOADNOT"
, "_OVERLOADTILDE" 
, "_OVERLOADPLUS"
, "_OVERLOADMINUS" 
, "_OVERLOADASTERISK" 
, "_OVERLOADSLASH" 
, "_OVERLOADCARET" 
, "_OVERLOADMOD" 
, "_OVERLOADLESSTHAN" 
, "_OVERLOADGREATERTHAN" 
, "_OVERLOADAND"
, "_OVERLOADOR"
, "_OVERLOADXOR" 
, "_OVERLOADEQ"
, "_OVERLOADNE"
, "_OVERLOADGE"
, "_OVERLOADGT"
, "_OVERLOADLE"
, "_OVERLOADLT"
, "_OVERLOADPOUND" 
, "_OVERLOADPOUNDPOUND" 
, "_OVERLOADISTRUE"
, "_OVERLOADFOREACH"
, "_OVERLOADHELP"
, "_OVERLOADPRINT"
, "_OVERLOADSIZE"
};

// except _OVERLOADBRACKETSLEFTSIDE all are functions
int OverloadOperatorIndexFun( std::string subName)
{
  assert( !subName.empty());
  if( subName[0] != '_') // optimization, true in most cases
    return -1;
  for( int i=1; i < NumberOfOverloadOperators; ++i)
    if( subName == overloadOperatorNames[ i])
      return i;
  return -1;
}
int OverloadOperatorIndexPro( std::string subName)
{
  if( subName == overloadOperatorNames[ 0])
    return 0;
  return -1;
}

#include <iostream>
BaseGDL* _GDL_OBJECT_OverloadIsTrue( EnvUDT* e)
{
//   // debug/check
//   std::cout << "_GDL_OBJECT_OverloadIsTrue called" << std::endl;
  // default behavior: Implict: Another object cannot be the null object
  return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}

// set up the _overload... subroutines for GDL_OBJECT
void SetupOverloadSubroutines()
{
//   // The call
//   BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
//   in call_fun eventually (in GDLInterpreter::statement) tree->Run() is called

  // _overloadIsTrue
  // automatically adds "SELF" parameter (obejct name is != "")
  DFun *_overloadIsTrue = new DFun("_OVERLOADISTRUE",GDL_OBJECT_NAME,"*INTERNAL*");
  WRAPPED_FUNNode *tree = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadIsTrue);
  _overloadIsTrue->SetTree( tree);
 
  DStructDesc* gdlObjectDesc = FindInStructList(structList, GDL_OBJECT_NAME);
  assert( gdlObjectDesc != NULL);
  
  gdlObjectDesc->SetOperator(OOIsTrue,_overloadIsTrue);
  gdlObjectDesc->FunList().push_back(_overloadIsTrue);

}