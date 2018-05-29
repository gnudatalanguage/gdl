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
// #include "basic_pro.hpp"
#include "nullgdl.hpp"
#include "list.hpp"
#include "hash.hpp"

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
  else return -1;
}

// for proper error reporting we need to unwind the stack
void ThrowFromInternalUDSub( EnvUDT* e, const string& s)
  {
    ProgNodeP callingNode = e->CallingNode();
    string objectName =e->GetPro()->ObjectName();
    delete e;
    GDLException::Interpreter()->CallStack().pop_back();
    throw GDLException( callingNode, objectName+" (internal): " + s, false, false);
  }

BaseGDL* _GDL_OBJECT_OverloadIsTrue( EnvUDT* e)
{
// default behavior: Implict: Another object cannot be the null object
  return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}
BaseGDL* _GDL_OBJECT_Init( EnvUDT* e)
{	  
//	std::cout << " gdl_OBJECT_Init!" << std::endl;
	return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}
void _GDL_OBJECT_OverloadBracketsLeftSide( EnvUDT* e)
{
  //   // debug/check
  //   std::cout << "_GDL_OBJECT_OverloadBracketsLeftSide called" << std::endl;

  // this is only called on scalar object references
  // IDL's default behavior is to just replace SELF (via OBJREF) by RVALUE
  // no index checking is done.
  SizeT nParam = e->NParam();
  if( nParam < 3) // consider implicit SELF
    return; // RVALUE not given -> ignore

//  BaseGDL** objRef = e->GetKW(1);
//  BaseGDL** objRefP = e->GetPtrTo( objRef);
  if( !e->GlobalKW(1))
  {
    ThrowFromInternalUDSub( e, "Parameter 1 (OBJREF) must be a passed as reference in this context.");
  }
  BaseGDL** objRefP = &e->GetKW(1);
  BaseGDL* objRef = *objRefP;

  BaseGDL* rValue = e->GetKW(2);
  if( rValue == NULL)
  {
    ThrowFromInternalUDSub( e, "Parameter 2 (RVALUE) is undefined.");
  }
  if( rValue->Type() != GDL_OBJ)
  {
    ThrowFromInternalUDSub( e, "Parameter 2 (RVALUE) must be an OBJECT in this context.");
  }
  
  GDLDelete( *objRefP);
  *objRefP = rValue->Dup();
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
  else  {
    try{
      isRangeLong = static_cast<DLongGDL*>( isRange->Convert2( GDL_LONG, BaseGDL::COPY));
    }
		catch( GDLException& ex) {
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
			ThrowFromInternalUDSub( e,
			 "Parameter is undefined: "  + e->Caller()->GetString(e->GetKW( p + 2)));
      DLong isRangeX = (*isRangeLong)[p];
      if( isRangeX != 0 && isRangeX != 1)
			ThrowFromInternalUDSub( e,
			 "Value of parameter 1 (ISRANGE["+i2s(p)+"]) is out of allowed range.");
      if( isRangeX == 1)
      {
	if( parX->N_Elements() != 3)
			ThrowFromInternalUDSub( e, "Range vector must have 3 elements: " +
							e->Caller()->GetString(e->GetKW( p + 2)));
	Guard<DLongGDL> parXLongGuard;
	DLongGDL* parXLong;
		  if( parX->Type() == GDL_LONG)
			parXLong = static_cast<DLongGDL*>( parX);
		  else  {
	  try{
	    parXLong = static_cast<DLongGDL*>( parX->Convert2( GDL_LONG, BaseGDL::COPY));
	  }
				catch( GDLException& ex) {
	    ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage());
	  }
			parXLongGuard.Reset( parXLong);
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
  // because only they clean up ixList on destruction
  Guard< ArrayIndexListT> aLGuard( aL);

  IxExprListT ixL;
  return aL->Index( e->GetKW( 0), ixL); // index SELF
}

BaseGDL* _GDL_OBJECT_OverloadEQOp( EnvUDT* e)
{
  SizeT nParam = e->NParam(); // number of parameters actually given
//   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if( nParam < 2) // consider implicit SELF
    ThrowFromInternalUDSub( e, "2 parameters are needed: LEFT, RIGHT.");

  // default behavior: Exact like scalar indexing
  BaseGDL* l = e->GetKW(1);
  if( l->Type() != GDL_OBJ)
    ThrowFromInternalUDSub( e, "Unable to convert parameter #1 to type object reference.");

  BaseGDL* r = e->GetKW(2);
  if( r->Type() != GDL_OBJ)
    ThrowFromInternalUDSub( e, "Unable to convert parameter #2 to type object reference.");
  
  DObjGDL* left = static_cast<DObjGDL*>(l);
  DObjGDL* right = static_cast<DObjGDL*>(r);
  
  ULong rEl=right->N_Elements();
  ULong nEl=left->N_Elements();
  //   if( nEl == 0)
  // 	 nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  DObj s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( left->Dim(), BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = (s == (*left)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < nEl; ++i)
	    (*res)[i] = ((*left)[i] == s);
	}    }
  else if( left->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->Dim(), BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] == s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] == s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] == (*left)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( left->Dim(), BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] == (*left)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] == (*left)[i]);
	}    }
  return res;
}

BaseGDL* _GDL_OBJECT_OverloadNEOp( EnvUDT* e)
{
  SizeT nParam = e->NParam(); // number of parameters actually given
//   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if( nParam < 3) // consider implicit SELF
    ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

  // default behavior: Exact like scalar indexing
  BaseGDL* l = e->GetKW(1);
  if( l->Type() != GDL_OBJ)
    ThrowFromInternalUDSub( e, "Unable to convert parameter #1 to type object reference.");

  BaseGDL* r = e->GetKW(2);
  if( r->Type() != GDL_OBJ)
    ThrowFromInternalUDSub( e, "Unable to convert parameter #2 to type object reference.");
  
  DObjGDL* left = static_cast<DObjGDL*>(l);
  DObjGDL* right = static_cast<DObjGDL*>(r);
  
  ULong rEl=right->N_Elements();
  ULong nEl=left->N_Elements();
  //   if( nEl == 0)
  // 	 nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  DObj s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( left->Dim(), BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = (s != (*left)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < nEl; ++i)
	    (*res)[i] = ((*left)[i] != s);
	}    }
  else if( left->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->Dim(), BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] != s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] != s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] != (*left)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( left->Dim(), BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] != (*left)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] != (*left)[i]);
	}    }
  return res;
}

// for GDL_OBJECT all other overloads are illegal operations as default
// however, they need to be accessible for (nonsense) parent method calls.
// But we can use just this one function for all of them
BaseGDL* _GDL_OBJECT_OverloadReportIllegalOperation( EnvUDT* e)
{
  ThrowFromInternalUDSub( e, "Operation illegal with object reference types.");
  return 0;
}

// set up the _overload... subroutines for GDL_OBJECT
void SetupOverloadSubroutines()
{
//   // The call
//   BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
//   in call_fun eventually (in GDLInterpreter::statement) tree->Run() is called
  DStructDesc* gdlObjectDesc = FindInStructList(structList, GDL_OBJECT_NAME);
  assert( gdlObjectDesc != NULL);
  DStructDesc* gdlContainerDesc = FindInStructList(structList, "GDL_CONTAINER");
  assert( gdlObjectDesc != NULL);
  DStructDesc* listDesc = FindInStructList(structList, "LIST");
  assert( listDesc != NULL);
  DStructDesc* hashDesc = FindInStructList(structList, "HASH");
  assert( hashDesc != NULL);
  
  WRAPPED_FUNNode *tree;
  WRAPPED_FUNNode *treeFun;
  WRAPPED_PRONode *treePro;
  
  // automatically adds "SELF" parameter (object name is != "")
  DFun *_overloadIsTrue = new DFun("_OVERLOADISTRUE",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadIsTrue);
  _overloadIsTrue->SetTree( treeFun);
// we are NOT setting the operator to have (faster) default behavior
// the functions must be there nevertheless for expicit callingNode
// that's why we add them to the functions list  
  gdlObjectDesc->FunList().push_back(_overloadIsTrue);
//   gdlObjectDesc->SetOperator(OOIsTrue,_overloadIsTrue);
// GDL_OBJECT:: [
  DPro *_overloadBracketsLeftSide = new DPro("_OVERLOADBRACKETSLEFTSIDE",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  _overloadBracketsLeftSide->AddPar("OBJREF")->AddPar("RVALUE")->AddPar("ISRANGE");
  _overloadBracketsLeftSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  _overloadBracketsLeftSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  treePro = new WRAPPED_PRONode(_GDL_OBJECT_OverloadBracketsLeftSide);
  _overloadBracketsLeftSide->SetTree( treePro); 
  gdlObjectDesc->ProList().push_back(_overloadBracketsLeftSide);
//   gdlObjectDesc->SetOperator(OOBracketsLeftSide,_overloadBracketsLeftSide);
// GDL_OBJECT::INIT()
  DFun *_init = new DFun("INIT",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_Init);
  _init->SetTree( treeFun);
  gdlObjectDesc->FunList().push_back(_init);
// GDL_OBJECT:: ]
  DFun *_overloadBracketsRightSide = new DFun("_OVERLOADBRACKETSRIGHTSIDE",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  _overloadBracketsRightSide->AddPar("ISRANGE");
  _overloadBracketsRightSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  _overloadBracketsRightSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadBracketsRightSide);
  _overloadBracketsRightSide->SetTree( treeFun);
  gdlObjectDesc->FunList().push_back(_overloadBracketsRightSide);
//   gdlObjectDesc->SetOperator(OOBracketsRightSide,_overloadBracketsRightSide);
// GDL_OBJECT:: =
   DFun *_overloadEQ = new DFun("_OVERLOADEQ",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  _overloadEQ->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadEQOp);
  _overloadEQ->SetTree( treeFun);
  gdlObjectDesc->FunList().push_back(_overloadEQ);
//   gdlObjectDesc->SetOperator(OOEQ,_overloadEQ);
// GDL_OBJECT:: !=
  DFun *_overloadNE = new DFun("_OVERLOADNE",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  _overloadNE->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadNEOp);
  _overloadNE->SetTree( treeFun);
  gdlObjectDesc->FunList().push_back(_overloadNE);
//   gdlObjectDesc->SetOperator(OONE,_overloadNE);
// GDL_OBJECT:: +
  DFun *_overloadPlus = new DFun("_OVERLOADPLUS",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  _overloadPlus->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadReportIllegalOperation);
  _overloadPlus->SetTree( treeFun);
  gdlObjectDesc->FunList().push_back(_overloadPlus);
//   gdlObjectDesc->SetOperator(OOPlus,_overloadPlus);
// GDL_OBJECT:: -
  DFun *_overloadMinus = new DFun("_OVERLOADMINUS",GDL_OBJECT_NAME,INTERNAL_LIBRARY_STR);
  _overloadMinus->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(_GDL_OBJECT_OverloadReportIllegalOperation);
  _overloadMinus->SetTree( treeFun);
  gdlObjectDesc->FunList().push_back(_overloadMinus);
//   gdlObjectDesc->SetOperator(OOMINUS,_overloadMinus);

// LIST:: ]  
  DFun *DFunLIST__overloadBracketsRightSide = new DFun("_OVERLOADBRACKETSRIGHTSIDE","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__overloadBracketsRightSide->AddPar("ISRANGE");
  DFunLIST__overloadBracketsRightSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  DFunLIST__overloadBracketsRightSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  treeFun = new WRAPPED_FUNNode( lib::LIST___OverloadBracketsRightSide);
  DFunLIST__overloadBracketsRightSide->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__overloadBracketsRightSide);
  listDesc->SetOperator(OOBracketsRightSide,DFunLIST__overloadBracketsRightSide);
// LIST:: [
  DPro *DFunPro_overloadBracketsLeftSide = new DPro("_OVERLOADBRACKETSLEFTSIDE","LIST",INTERNAL_LIBRARY_STR);
  DFunPro_overloadBracketsLeftSide->AddPar("OBJREF")->AddPar("RVALUE")->AddPar("ISRANGE");
  DFunPro_overloadBracketsLeftSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  DFunPro_overloadBracketsLeftSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  treePro = new WRAPPED_PRONode(lib::LIST___OverloadBracketsLeftSide);
  DFunPro_overloadBracketsLeftSide->SetTree( treePro); 
  listDesc->ProList().push_back(DFunPro_overloadBracketsLeftSide);
  listDesc->SetOperator(OOBracketsLeftSide,DFunPro_overloadBracketsLeftSide);
// LIST:: +
  DFun *LIST_overloadPlus = new DFun("_OVERLOADPLUS","LIST",INTERNAL_LIBRARY_STR);
  LIST_overloadPlus->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(lib::LIST___OverloadPlus);
  LIST_overloadPlus->SetTree( treeFun);
  listDesc->FunList().push_back(LIST_overloadPlus);
  listDesc->SetOperator(OOPlus,LIST_overloadPlus);
// LIST:: =
  DFun *LIST_overloadEQ = new DFun("_OVERLOADEQ","LIST",INTERNAL_LIBRARY_STR);
  LIST_overloadEQ->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(lib::LIST___OverloadEQOp);
  LIST_overloadEQ->SetTree( treeFun);
  listDesc->FunList().push_back(LIST_overloadEQ);
  listDesc->SetOperator(OOEQ,LIST_overloadEQ);
// LIST:: !=
  DFun *LIST_overloadNE = new DFun("_OVERLOADNE","LIST",INTERNAL_LIBRARY_STR);
  LIST_overloadNE->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(lib::LIST___OverloadNEOp);
  LIST_overloadNE->SetTree( treeFun);
  listDesc->FunList().push_back(LIST_overloadNE);
  listDesc->SetOperator(OONE,LIST_overloadNE);
// LIST::
  DFun *LIST_overloadIsTrue = new DFun("_OVERLOADISTRUE","LIST",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode(lib::LIST___OverloadIsTrue);
  LIST_overloadIsTrue->SetTree( treeFun);
  listDesc->FunList().push_back(LIST_overloadIsTrue);
  listDesc->SetOperator(OOIsTrue,LIST_overloadIsTrue);
 
// LIST::ADD
  DPro *DProLIST__ADD = new DPro("ADD","LIST",INTERNAL_LIBRARY_STR);
  DProLIST__ADD->AddKey("EXTRACT","EXTRACT")->AddKey("NO_COPY","NO_COPY");
  DProLIST__ADD->AddKey("POSITION","POSITION");
  DProLIST__ADD->AddPar("VALUE")->AddPar("INDEX");
  treePro = new WRAPPED_PRONode( lib::list__add);
  DProLIST__ADD->SetTree( treePro);
  listDesc->ProList().push_back(DProLIST__ADD);
// LIST::REMOVE()
  DFun *DFunLIST__REMOVE = new DFun("REMOVE","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__REMOVE->AddKey("ALL","ALL");
  DFunLIST__REMOVE->AddPar("INDEX");
  treeFun = new WRAPPED_FUNNode( lib::list__remove_fun);
  DFunLIST__REMOVE->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__REMOVE);
// LIST::REMOVE PRO
  DPro *DProLIST__REMOVE = new DPro("REMOVE","LIST",INTERNAL_LIBRARY_STR);
  DProLIST__REMOVE->AddKey("ALL","ALL");
  DProLIST__REMOVE->AddPar("INDEX");
  treePro = new WRAPPED_PRONode( lib::list__remove_pro);
  DProLIST__REMOVE->SetTree( treePro);
  listDesc->ProList().push_back(DProLIST__REMOVE);
// LIST::REVERSE PRO
  DPro *DProLIST__REVERSE = new DPro("REVERSE","LIST",INTERNAL_LIBRARY_STR);
  treePro = new WRAPPED_PRONode( lib::list__reverse);
  DProLIST__REVERSE->SetTree( treePro);
  listDesc->ProList().push_back(DProLIST__REVERSE);
// LIST::ToArray()
  DFun *DFunLIST__TOARRAY = new DFun("TOARRAY","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__TOARRAY->AddKey("TYPE","TYPE")->AddKey("MISSING","MISSING");
  DFunLIST__TOARRAY->AddKey("DIMENSION","DIMENSION")->AddKey("NO_COPY","NO_COPY");
  DFunLIST__TOARRAY->AddKey("PROMOTE_TYPE","PROMOTE_TYPE")->AddKey("TRANSPOSE","TRANSPOSE");
  
  treeFun = new WRAPPED_FUNNode( lib::list__toarray);
  DFunLIST__TOARRAY->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__TOARRAY);
  #if 0
// LIST::HELP()
  DFun *DFunLIST__HELP = new DFun("HELP","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__TOHELP->AddKey("MAXITEM","MAXITEM");
  treeFun = new WRAPPED_FUNNode( lib::list__help);
  DFunLIST__TOHELP->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__TOHELP);
  #endif
  #if 0
// LIST::HELP
  DPro *DProLIST__HELP = new DPro("HELP","LIST",INTERNAL_LIBRARY_STR);
  DProLIST__HELP->AddKey("MAXITEM","MAXITEM");
  treePro = new WRAPPED_PRONode( lib::list__help);
  DProLIST__HELP->SetTree( treePro);
  listDesc->ProList().push_back(DProLIST__HELP);
  #endif
// LIST::CLEANUP
  DPro *DProLIST__CLEANUP = new DPro("CLEANUP","LIST",INTERNAL_LIBRARY_STR);
  treePro = new WRAPPED_PRONode( lib::list__cleanup);
  DProLIST__CLEANUP->SetTree( treePro);
	  listDesc->ProList().push_back(DProLIST__CLEANUP);
// LIST::MOVE
  DPro *DProLIST__MOVE = new DPro("MOVE","LIST",INTERNAL_LIBRARY_STR);
  DProLIST__MOVE->AddPar("SOURCE")->AddPar("DESTINATION");
  treePro = new WRAPPED_PRONode( lib::list__move);
  DProLIST__MOVE->SetTree( treePro);
  listDesc->ProList().push_back(DProLIST__MOVE);
// LIST::SWAP
  DPro *DProLIST__SWAP = new DPro("SWAP","LIST",INTERNAL_LIBRARY_STR);
  DProLIST__SWAP->AddPar("INDEX1")->AddPar("INDEX2");
  treePro = new WRAPPED_PRONode( lib::list__swap);
  DProLIST__SWAP->SetTree( treePro);
  listDesc->ProList().push_back(DProLIST__SWAP);
// LIST::COUNT()
  DFun *DFunLIST__COUNT = new DFun("COUNT","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__COUNT->AddPar("VALUE");
  treeFun = new WRAPPED_FUNNode( lib::list__count);
  DFunLIST__COUNT->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__COUNT);
// LIST::ISEMPTY()
  DFun *DFunLIST__ISEMPTY = new DFun("ISEMPTY","LIST",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::list__isempty);
  DFunLIST__ISEMPTY->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__ISEMPTY);
// LIST::WHERE()
  DFun *DFunLIST__WHERE = new DFun("WHERE","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__WHERE->AddKey("COMPLEMENT","COMPLEMENT");
  DFunLIST__WHERE->AddKey("COUNT","COUNT");
  DFunLIST__WHERE->AddKey("NCOMPLEMENT","NCOMPLEMENT");
  DFunLIST__WHERE->AddPar("VALUE");
  treeFun = new WRAPPED_FUNNode( lib::list__where);
  DFunLIST__WHERE->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__WHERE);
// LIST::GET()  // here to make up for IDL_CONTAINER lack.
  // res=List.get([/all] [, isa=(names)] [. position=index] [, count=variable] [/null][)
  DFun *DFunLIST__GET = new DFun("GET","LIST",INTERNAL_LIBRARY_STR);
  DFunLIST__GET->AddKey("ALL","ALL")->AddKey("ISA","ISA")->AddKey("NULL","NULL");
  DFunLIST__GET->AddKey("COUNT","COUNT");
  DFunLIST__GET->AddKey("POSITION","POSITION");

  treeFun = new WRAPPED_FUNNode( lib::list__get);
  DFunLIST__GET->SetTree( treeFun);
  listDesc->FunList().push_back(DFunLIST__GET);

// LIST::INIT()  // here to make up for IDL_CONTAINER lack.
  // list is parented by GDL_OBJECT which can handled INIT:
 // DFun *DFunLIST__INIT = new DFun("INIT","LIST",INTERNAL_LIBRARY_STR);
 // treeFun = new WRAPPED_FUNNode( lib::list__init);
 // DFunLIST__INIT->SetTree( treeFun);
 // listDesc->FunList().push_back(DFunLIST__INIT);
  
  
// HASH  
  DFun *DFunHASH__overloadBracketsRightSide = new DFun("_OVERLOADBRACKETSRIGHTSIDE","HASH",INTERNAL_LIBRARY_STR);
  DFunHASH__overloadBracketsRightSide->AddPar("ISRANGE");
  DFunHASH__overloadBracketsRightSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  DFunHASH__overloadBracketsRightSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  treeFun = new WRAPPED_FUNNode( lib::HASH___OverloadBracketsRightSide);
  DFunHASH__overloadBracketsRightSide->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__overloadBracketsRightSide);
  hashDesc->SetOperator(OOBracketsRightSide,DFunHASH__overloadBracketsRightSide);

  DPro *DProHASH_overloadBracketsLeftSide = new DPro("_OVERLOADBRACKETSLEFTSIDE","HASH",INTERNAL_LIBRARY_STR);
  DProHASH_overloadBracketsLeftSide->AddPar("OBJREF")->AddPar("RVALUE")->AddPar("ISRANGE");
  DProHASH_overloadBracketsLeftSide->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  DProHASH_overloadBracketsLeftSide->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
  treePro = new WRAPPED_PRONode(lib::HASH___OverloadBracketsLeftSide);
  DProHASH_overloadBracketsLeftSide->SetTree( treePro); 
  hashDesc->ProList().push_back(DProHASH_overloadBracketsLeftSide);
  hashDesc->SetOperator(OOBracketsLeftSide,DProHASH_overloadBracketsLeftSide);

  DFun *HASH_overloadPlus = new DFun("_OVERLOADPLUS","HASH",INTERNAL_LIBRARY_STR);
  HASH_overloadPlus->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(lib::HASH___OverloadPlus);
  HASH_overloadPlus->SetTree( treeFun);
  hashDesc->FunList().push_back(HASH_overloadPlus);
  hashDesc->SetOperator(OOPlus,HASH_overloadPlus);
 
  DFun *HASH_overloadEQ = new DFun("_OVERLOADEQ","HASH",INTERNAL_LIBRARY_STR);
  HASH_overloadEQ->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(lib::HASH___OverloadEQOp);
  HASH_overloadEQ->SetTree( treeFun);
  hashDesc->FunList().push_back(HASH_overloadEQ);
  hashDesc->SetOperator(OOEQ,HASH_overloadEQ);

  DFun *HASH_overloadNE = new DFun("_OVERLOADNE","HASH",INTERNAL_LIBRARY_STR);
  HASH_overloadNE->AddPar("LEFT")->AddPar("RIGHT");
  treeFun = new WRAPPED_FUNNode(lib::HASH___OverloadNEOp);
  HASH_overloadNE->SetTree( treeFun);
  hashDesc->FunList().push_back(HASH_overloadNE);
  hashDesc->SetOperator(OONE,HASH_overloadNE);
 
  DFun *HASH_overloadIsTrue = new DFun("_OVERLOADISTRUE","HASH",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode(lib::HASH___OverloadIsTrue);
  HASH_overloadIsTrue->SetTree( treeFun);
  hashDesc->FunList().push_back(HASH_overloadIsTrue);
  hashDesc->SetOperator(OOIsTrue,HASH_overloadIsTrue);
 
// HASH::REMOVE()
  DFun *DFunHASH__REMOVE = new DFun("REMOVE","HASH",INTERNAL_LIBRARY_STR);
  DFunHASH__REMOVE->AddKey("ALL","ALL");
  DFunHASH__REMOVE->AddPar("INDEX");
  treeFun = new WRAPPED_FUNNode( lib::hash__remove_fun);
  DFunHASH__REMOVE->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__REMOVE);
// HASH::REMOVE PRO
  DPro *DProHASH__REMOVE = new DPro("REMOVE","HASH",INTERNAL_LIBRARY_STR);
  DProHASH__REMOVE->AddKey("ALL","ALL");
  DProHASH__REMOVE->AddPar("INDEX");
  treePro = new WRAPPED_PRONode( lib::hash__remove_pro);
  DProHASH__REMOVE->SetTree( treePro);
  hashDesc->ProList().push_back(DProHASH__REMOVE);
// HASH::HASKEY()
  DFun *DFunHASH__HASKEY = new DFun("HASKEY","HASH",INTERNAL_LIBRARY_STR);
  DFunHASH__HASKEY->AddPar("KEYLIST");
  treeFun = new WRAPPED_FUNNode( lib::hash__haskey);
  DFunHASH__HASKEY->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__HASKEY);
// HASH::KEYS()
  DFun *DFunHASH__KEYS = new DFun("KEYS","HASH",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::hash__keys);
  DFunHASH__KEYS->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__KEYS);
// HASH::VALUES()
  DFun *DFunHASH__VALUES = new DFun("VALUES","HASH",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::hash__values);
  DFunHASH__VALUES->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__VALUES);
// HASH::TOSTRUCT()
  DFun *DFunHASH__TOSTRUCT = new DFun("TOSTRUCT","HASH",INTERNAL_LIBRARY_STR);

  DFunHASH__TOSTRUCT->AddKey("SKIPPED","SKIPPED")->AddKey("MISSING","MISSING");
  DFunHASH__TOSTRUCT->AddKey("NO_COPY","NO_COPY")->AddKey("RECURSIVE","RECURSIVE");

  treeFun = new WRAPPED_FUNNode( lib::hash__tostruct);
  DFunHASH__TOSTRUCT->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__TOSTRUCT);
// HASH::COUNT()
  DFun *DFunHASH__COUNT = new DFun("COUNT","HASH",INTERNAL_LIBRARY_STR);
  DFunHASH__COUNT->AddPar("VALUE");
  treeFun = new WRAPPED_FUNNode( lib::hash__count);
  DFunHASH__COUNT->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__COUNT);
// HASH::ISEMPTY()
  DFun *DFunHASH__ISEMPTY = new DFun("ISEMPTY","HASH",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::hash__isempty);
  DFunHASH__ISEMPTY->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__ISEMPTY);
// HASH::ISORDERED()
  DFun *DFunHASH__ISORDERED = new DFun("ISORDERED","HASH",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::hash__isordered);
  DFunHASH__ISORDERED->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__ISORDERED);
// HASH::ISFOLDCASE()
  DFun *DFunHASH__ISFOLDCASE = new DFun("ISFOLDCASE","HASH",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::hash__isfoldcase);
  DFunHASH__ISFOLDCASE->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__ISFOLDCASE);
// HASH::WHERE()
  DFun *DFunHASH__WHERE = new DFun("WHERE","HASH",INTERNAL_LIBRARY_STR);
  DFunHASH__WHERE->AddKey("COMPLEMENT","COMPLEMENT");
  DFunHASH__WHERE->AddKey("COUNT","COUNT");
  DFunHASH__WHERE->AddKey("NCOMPLEMENT","NCOMPLEMENT");
  DFunHASH__WHERE->AddPar("VALUE");
  treeFun = new WRAPPED_FUNNode( lib::hash__where);
  DFunHASH__WHERE->SetTree( treeFun);
  hashDesc->FunList().push_back(DFunHASH__WHERE);
 
 // GDL_CONTAINER - references list procedures because, we can.
// GDL_CONTAINER::GET() - Not present in list.
  // res=GDL_CONTAINER.get([/all] [, isa=(names)] [. position=index] [, count=variable] [/null][)
  DFun* DFunlist = new DFun("GET","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DFunlist->AddKey("ALL","ALL")->AddKey("ISA","ISA")->AddKey("NULL","NULL");
  DFunlist->AddKey("COUNT","COUNT");
  DFunlist->AddKey("POSITION","POSITION");

  treeFun = new WRAPPED_FUNNode( lib::list__get);
  DFunlist->SetTree( treeFun);
  gdlContainerDesc->FunList().push_back(DFunlist);

// GDL_CONTAINER::INIT()
//  DFunlist = new DFun("INIT","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
//  treeFun = new WRAPPED_FUNNode( lib::list__init);
//  DFunlist->SetTree( treeFun);
//  gdlContainerDesc->FunList().push_back(DFunlist);
  
// GDL_CONTAINER::COUNT()
  DFunlist = new DFun("COUNT","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DFunlist->AddPar("VALUE");
  treeFun = new WRAPPED_FUNNode( lib::list__count);
  DFunlist->SetTree( treeFun);
  gdlContainerDesc->FunList().push_back(DFunlist);
// GDL_CONTAINER::ADD
  DPro* DProlist = new DPro("ADD","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DProlist->AddKey("EXTRACT","EXTRACT")->AddKey("NO_COPY","NO_COPY");
  DProlist->AddKey("POSITION","POSITION");
  DProlist->AddPar("VALUE")->AddPar("INDEX");
  treePro = new WRAPPED_PRONode( lib::list__add);
  DProlist->SetTree( treePro);
  gdlContainerDesc->ProList().push_back(DProlist);
  #if 0
// GDL_CONTAINER::HELP
  DProlist = new DPro("HELP","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DProlist->AddKey("MAXITEM","MAXITEM");
  treePro = new WRAPPED_PRONode( lib::list__help);
  DProlist->SetTree( treePro);
  gdlContainerDesc->ProList().push_back(DProlist);
  #endif
// GDL_CONTAINER::CLEANUP
// 
//*2017-Dec-23 GVJ Omit this routine and see if GDL takes care of things.
// 2018-May revert
  DProlist = new DPro("CLEANUP","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  treePro = new WRAPPED_PRONode( lib::container__cleanup);
  DProlist->SetTree( treePro);
  gdlContainerDesc->ProList().push_back(DProlist);//*/
// GDL_CONTAINER::MOVE
  DProlist = new DPro("MOVE","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DProlist->AddPar("SOURCE")->AddPar("DESTINATION");
  treePro = new WRAPPED_PRONode( lib::list__move);
  DProlist->SetTree( treePro);
  gdlContainerDesc->ProList().push_back(DProlist);
// GDL_CONTAINER::REMOVE()
  DProlist = new DPro("REMOVE","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DProlist->AddKey("ALL","ALL");
  DProlist->AddKey("POSITION","POSITION");
  DProlist->AddPar("HEAPVAR");
  treePro = new WRAPPED_PRONode( lib::container__remove);
  DProlist->SetTree( treePro);
  gdlContainerDesc->ProList().push_back(DProlist);
// GDL_CONTAINER::EQUALS()
  DFunlist = new DFun("EQUALS","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  treeFun = new WRAPPED_FUNNode( lib::container__equals);
  DFunlist->SetTree( treeFun);
  listDesc->FunList().push_back( DFunlist);
// GDL_CONTAINER::ISCONTAINED()
  DFunlist = new DFun("ISCONTAINED","GDL_CONTAINER",INTERNAL_LIBRARY_STR);
  DFunlist->AddKey("POSITION","POSITION");
  DFunlist->AddPar("VALUE");
  treeFun = new WRAPPED_FUNNode( lib::container__iscontained);
  DFunlist->SetTree( treeFun);
  listDesc->FunList().push_back( DFunlist);
  
}
