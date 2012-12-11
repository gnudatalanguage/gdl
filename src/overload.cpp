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

void ThrowFromInternalUDSub( EnvUDT* e, const string& s)
  {
    ProgNodeP callingNode = e->CallingNode();
    string objectName =e->GetPro()->ObjectName();
    delete e;
    GDLException::Interpreter()->CallStack().pop_back();
    throw GDLException( callingNode, objectName+" (internal): " + s, false, false);
  }

void _GDL_OBJECT_OverloadBracketsLeftSide( EnvUDT* e)
{
  assert( GDLException::Interpreter()->CallStack().back() == e);
  //   // debug/check
  //   std::cout << "_GDL_OBJECT_OverloadBracketsLeftSide called" << std::endl;

  // this is only called on scalar object references
  // IDL's default behavior is to just replace SELF (via OBJREF) by RVALUE
  // no index checking is done.
  SizeT nParam = e->NParam();
//   int envSize = e->EnvSize();
  if( nParam < 3) // consider implicit SELF
    return; // RVALUE not given -> ignore

  BaseGDL* objRef = e->GetKW(1);
  BaseGDL** objRefP = e->GetPtrTo( objRef);
  if( objRefP == NULL)
  {
    ThrowFromInternalUDSub( e, "Parameter 1 (OBJREF) must be a passed as reference in this context.");
//     ProgNodeP callingNode = e->CallingNode();
//     string objectName =e->GetPro()->ObjectName();
//     delete e;
//     GDLException::Interpreter()->CallStack().pop_back();
//     throw GDLException( callingNode, objectName+
//     " (internal): Parameter 1 (OBJREF) must be a passed as reference in this context.", false, false);
  }
  BaseGDL* rValue = e->GetKW(2);
  if( rValue == NULL)
  {
    ThrowFromInternalUDSub( e, "Parameter 2 (RVALUE) is undefined.");
//     ProgNodeP callingNode = e->CallingNode();
//     string objectName =e->GetPro()->ObjectName();
//     delete e;
//     GDLException::Interpreter()->CallStack().pop_back();
//     throw GDLException( callingNode, objectName+
//     " (internal): Parameter 2 (RVALUE) is undefined.", false, false);
  }
  if( rValue->Type() != GDL_OBJ)
  {
    ThrowFromInternalUDSub( e, "Parameter 2 (RVALUE) must be an OBJECT in this context.");
//     ProgNodeP callingNode = e->CallingNode();
//     string objectName =e->GetPro()->ObjectName();
//     delete e;
//     GDLException::Interpreter()->CallStack().pop_back();
//     throw GDLException( callingNode, objectName+
//     " (internal): Parameter 2 (RVALUE) must be an OBJECT in this context.", false, false);
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
//   std::cout << "_GDL_OBJECT_OverloadBracketsRightSide called" << std::endl;

  SizeT nParam = e->NParam(); // number of parameters actually given
//   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if( nParam < 3) // consider implicit SELF
    ThrowFromInternalUDSub( e, "At least 2 parameters are needed: ISRANGE, SUB1 [, ...].");

  // default behavior: Exact like scalar indexing
  BaseGDL* isRange = e->GetKW(1);
  if( isRange == NULL)
    ThrowFromInternalUDSub( e, "Parameter 1 (ISRANGE) is undefined.");
  if( isRange->Rank() == 0)
    ThrowFromInternalUDSub( e, "Parameter 1 (ISRANGE) must be an array in this context: " + e->Caller()->GetString(e->GetKW(1)));
  SizeT nIsRange = isRange->N_Elements();
  if( nIsRange > (nParam - 2)) //- SELF and ISRANGE
    ThrowFromInternalUDSub( e, "Parameter 1 (ISRANGE) must have "+i2s(nParam-2)+" elements.");
  Guard<DLongGDL> isRangeLongGuard;
  DLongGDL* isRangeLong;
  if( isRange->Type() == GDL_LONG)
    isRangeLong = static_cast<DLongGDL*>( isRange);
  else
  {
    try{
      isRangeLong = static_cast<DLongGDL*>( isRange->Convert2( GDL_LONG, BaseGDL::COPY));
    }
    catch( GDLException& ex)
    {
      ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage());
    }
    isRangeLongGuard.Reset( isRangeLong);
  }

  ArrayIndexVectorT ixList;
//   IxExprListT exprList;
  try {
    for( int p=0; p<nIsRange; ++p)
    {
      BaseGDL* parX = e->GetKW( p + 2); // implicit SELF, ISRANGE, par1..par8
      if( parX == NULL)
	ThrowFromInternalUDSub( e, "Parameter is undefined: "  + e->Caller()->GetString(e->GetKW( p + 2)));

      DLong isRangeX = (*isRangeLong)[p];
      if( isRangeX != 0 && isRangeX != 1)
      {
	ThrowFromInternalUDSub( e, "Value of parameter 1 (ISRANGE["+i2s(p)+"]) is out of allowed range.");
      }
      if( isRangeX == 1)
      {
	if( parX->N_Elements() != 3)
	{
	  ThrowFromInternalUDSub( e, "Range vector must have 3 elements: " + e->Caller()->GetString(e->GetKW( p + 2)));
	}
	DLongGDL* parXLong;
	Guard<DLongGDL> parXLongGuard;
	if( parX->Type() != GDL_LONG)
	{
	  try{
	    parXLong = static_cast<DLongGDL*>( parX->Convert2( GDL_LONG, BaseGDL::COPY));
	    parXLongGuard.Reset( parXLong);
	  }
	  catch( GDLException& ex)
	  {
	    ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage());
	  }
	}
	else
	{
	  parXLong = static_cast<DLongGDL*>( parX);
	}
	// negative end ix is fine -> CArrayIndexRangeS can handle [b:*:s] ([b,-1,s])
	ixList.push_back(new CArrayIndexRangeS( (*parXLong)[0], (*parXLong)[1], (*parXLong)[2]));
      }
      else // non-range
      {
	// ATTENTION: These two grab c1 (all others don't)
	// a bit unclean, but for maximum efficiency
	if( parX->Rank() == 0)
	  ixList.push_back( new CArrayIndexScalar( parX->Dup()));
	else
	  ixList.push_back( new CArrayIndexIndexed( parX->Dup()));
      }
    } // for
  }
  catch( GDLException& ex)
  {
    ixList.Destruct(); // ixList is not valid afterwards, but as we throw this is ok
    throw ex;
  }
  
  ArrayIndexListT* aL;
  MakeArrayIndex( &ixList, &aL, NULL); // important to get the non-NoAssoc ArrayIndexListT
  // because only they clean up ixList
  Guard< ArrayIndexListT> aLGuard( aL);

  IxExprListT ixL;
  return aL->Index( e->GetKW( 0), ixL); // index SELF
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