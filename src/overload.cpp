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
#include "dinterpreter.hpp"

using namespace std;

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

// #include <iostream>
BaseGDL* _GDL_OBJECT_OverloadIsTrue( EnvUDT* e)
{
//   // debug/check
//   std::cout << "_GDL_OBJECT_OverloadIsTrue called" << std::endl;

// default behavior: Implict: Another object cannot be the null object
  return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}

void _GDL_OBJECT_OverloadBracketsLeftSide( EnvUDT* e)
{
  assert( GDLException::Interpreter()->CallStack().back() == e);
  //   // debug/check
  //   std::cout << "_GDL_OBJECT_OverloadBracketsLeftSide called" << std::endl;

  // this is only called on scalar object references
  // IDL's default behavior is to just replace SELF (via OBJREF) by RVALUE
  // no index checking is done.
  int envSize = e->EnvSize();
  if( envSize < 3) // consider implicit SELF
    return; // RVALUE not given -> ignore

  BaseGDL* objRef = e->GetKW(1);
  BaseGDL** objRefP = e->GetPtrTo( objRef);
  if( objRefP == NULL)
  {
    ProgNodeP callingNode = e->CallingNode();
    string objectName =e->GetPro()->ObjectName();
    delete e;
    GDLException::Interpreter()->CallStack().pop_back();
    throw GDLException( callingNode, objectName+
    " (internal): Parameter 1 (OBJREF) must be a passed as reference in this context.", false, false);
  }
  BaseGDL* rValue = e->GetKW(2);
  if( rValue == NULL)
  {
    ProgNodeP callingNode = e->CallingNode();
    string objectName =e->GetPro()->ObjectName();
    delete e;
    GDLException::Interpreter()->CallStack().pop_back();
    throw GDLException( callingNode, objectName+
    " (internal): Parameter 2 (RVALUE) is undefined.", false, false);
  }
  if( rValue->Type() != GDL_OBJ)
  {
    ProgNodeP callingNode = e->CallingNode();
    string objectName =e->GetPro()->ObjectName();
    delete e;
    GDLException::Interpreter()->CallStack().pop_back();
    throw GDLException( callingNode, objectName+
    " (internal): Parameter 2 (RVALUE) must be an OBJECT in this context.", false, false);
  }
  
  GDLDelete( *objRefP);
  *objRefP = rValue->Dup();
  
//   int envSize = e->EnvSize();
//   if( envSize < 5) // consider implicit SELF
//     e->Throw("At least 4 parameters are needed: OBJREF, RVALUE, ISRANGE, SUB1 [, ...].");
//   BaseGDL* objRef = e->GetKW(1);
//   BaseGDL** objRefP = e->GetPtrTo( objRef);
//   if( objRefP == NULL)
//     e->Throw("Parameter 1 (OBJREF) must be a passed as reference in this context.");
//   BaseGDL* rValue = e->GetKW(2);
//   if( rValue == NULL)
//     e->Throw("Parameter 2 (RVALUE) is undefined.");
//   if( rValue->Type() != GDL_OBJ)
//     e->Throw("Parameter 2 (RVALUE) must be an OBJECT in this context.");
//   BaseGDL* isRange = e->GetKW(3);
//   if( isRange == NULL)
//     e->Throw("Parameter 3 (ISRANGE) is undefined.");
//   Guard<DLongGDL> isRangeLongGuard;
//   DLongGDL* isRangeLong;
//   if( isRange->Type() == GDL_LONG)
//     isRangeLong = static_cast<DLongGDL*>( isRange);
//   else
//   {
//     isRangeLong = isRange->Convert2( GDL_LONG, BaseGDL::COPY);
//     isRangeLongGuard.Reset( isRangeLong);
//   }
//   SizeT nIsRange = isRangeLong->N_Elements();
//   SizeT nSub = envSize - 4; // >= 1 s. a.
//   if( nIsRange != nSub)
//     e->Throw("Parameter 3 (ISRANGE) must have one element for each index parameter (#4 - #11).");
}

BaseGDL* _GDL_OBJECT_OverloadBracketsRightSide( EnvUDT* e)
{
//   // debug/check
//   std::cout << "_GDL_OBJECT_OverloadIsTrue called" << std::endl;
// TODO
// default behavior: Implict: Another object cannot be the null object
  return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}

// set up the _overload... subroutines for GDL_OBJECT
void SetupOverloadSubroutines()
{
//   // The call
//   BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
//   in call_fun eventually (in GDLInterpreter::statement) tree->Run() is called
  DStructDesc* gdlObjectDesc = FindInStructList(structList, GDL_OBJECT_NAME);
  assert( gdlObjectDesc != NULL);
  
  // automatically adds "SELF" parameter (object name is != "")
  DFun *_overloadIsTrue = new DFun("_OVERLOADISTRUE",GDL_OBJECT_NAME,"*INTERNAL*");
  WRAPPED_FUNNode *tree1 = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadIsTrue);
  _overloadIsTrue->SetTree( tree1);
  gdlObjectDesc->FunList().push_back(_overloadIsTrue);
  gdlObjectDesc->SetOperator(OOIsTrue,_overloadIsTrue);

  DPro *_overloadBracketsLeftSide = new DPro("_OVERLOADBRACKETSLEFTSIDE",GDL_OBJECT_NAME,"*INTERNAL*");
  _overloadBracketsLeftSide->AddPar("OBJREF")->AddPar("RVALUE")->AddPar("ISRANGE");
  _overloadBracketsLeftSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  _overloadBracketsLeftSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  WRAPPED_PRONode *tree2 = new WRAPPED_PRONode(_GDL_OBJECT_OverloadBracketsLeftSide);
  _overloadBracketsLeftSide->SetTree( tree2); 
  gdlObjectDesc->ProList().push_back(_overloadBracketsLeftSide);
  gdlObjectDesc->SetOperator(OOBracketsLeftSide,_overloadBracketsLeftSide);

  DFun *_overloadBracketsRightSide = new DFun("_OVERLOADBRACKETSRIGHTSIDE",GDL_OBJECT_NAME,"*INTERNAL*");
  _overloadBracketsRightSide->AddPar("ISRANGE");
  _overloadBracketsRightSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  _overloadBracketsRightSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  WRAPPED_FUNNode *tree3 = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadBracketsRightSide);
  _overloadBracketsRightSide->SetTree( tree3);
  gdlObjectDesc->FunList().push_back(_overloadBracketsRightSide);
  gdlObjectDesc->SetOperator(OOBracketsRightSide,_overloadBracketsRightSide);

}