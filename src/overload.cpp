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


#include "objects.hpp"
#include "overload.hpp"
#include "prognodeexpr.hpp"
#include "dinterpreter.hpp"
#include "list.hpp"
#include "hash.hpp"
#include "gdl2gdl.hpp"

#ifdef USE_SHAPELIB
#include "Shapefiles.hpp"
#endif

#ifdef USE_EXPAT
#include "sax.hpp"
#endif

using namespace std;

std::string overloadOperatorNames[] ={
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
};  //_OVERLOADFUNCTION, _OVERLOADIMPLIEDPRINT, _OVERLOADMETHOD, _OVERLOADMINUSMINUS, _OVERLOADPLUSPLUS, _OVERLOADPOSTINCREMENTCOPY

// except _OVERLOADBRACKETSLEFTSIDE all are functions

int OverloadOperatorIndexFun(std::string subName) {
  assert(!subName.empty());
  if (subName[0] != '_') // optimization, true in most cases
    return -1;
  for (int i = 1; i < NumberOfOverloadOperators; ++i)
    if (subName == overloadOperatorNames[ i])
      return i;
  return -1;
}

int OverloadOperatorIndexPro(std::string subName) {
  if (subName == overloadOperatorNames[ 0])
    return 0;
  else return -1;
}

// for proper error reporting we need to unwind the stack

void ThrowFromInternalUDSub(EnvUDT* e, const string& s) {
  ProgNodeP callingNode = e->CallingNode();
  string objectName = e->GetPro()->ObjectName();
  delete e;
  GDLException::Interpreter()->CallStack().pop_back();
  throw GDLException(callingNode, objectName + " (internal): " + s, false, false);
}

BaseGDL* _GDL_OBJECT_OverloadIsTrue(EnvUDT* e) {
  // default behavior: Implict: Another object cannot be the null object
  return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}

BaseGDL* _GDL_OBJECT_Init(EnvUDT* e) {
  //  std::cout << " gdl_OBJECT_Init!" << std::endl;
  return new DIntGDL(1); // if we reach here, defaul is to return 'TRUE'
}

void _GDL_OBJECT_OverloadBracketsLeftSide(EnvUDT* e) {
  //   // debug/check
  //   std::cout << "_GDL_OBJECT_OverloadBracketsLeftSide called" << std::endl;

  // this is only called on scalar object references
  // IDL's default behavior is to just replace SELF (via OBJREF) by RVALUE
  // no index checking is done.
  SizeT nParam = e->NParam();
  if (nParam < 3) // consider implicit SELF
    return; // RVALUE not given -> ignore

  //  BaseGDL** objRef = e->GetKW(1);
  //  BaseGDL** objRefP = e->GetPtrTo( objRef);
  if (!e->GlobalKW(1)) {
    ThrowFromInternalUDSub(e, "Parameter 1 (OBJREF) must be a passed as reference in this context.");
  }
  BaseGDL** objRefP = &e->GetTheKW(1); //OK global KW tested above
  BaseGDL* objRef = *objRefP;

  BaseGDL* rValue = e->GetKW(2);
  if (rValue == NULL) {
    ThrowFromInternalUDSub(e, "Parameter 2 (RVALUE) is undefined.");
  }
  if (rValue->Type() != GDL_OBJ) {
    ThrowFromInternalUDSub(e, "Parameter 2 (RVALUE) must be an OBJECT in this context.");
  }

  GDLDelete(*objRefP);
  *objRefP = rValue->Dup();
}

BaseGDL* _GDL_OBJECT_OverloadBracketsRightSide(EnvUDT* e) {
  //   // debug/check
  //   std::cout << "_GDL_OBJECT_OverloadBracketsRightSide called" << std::endl;

  SizeT nParam = e->NParam(); // number of parameters actually given
  //   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if (nParam < 3) // consider implicit SELF
    ThrowFromInternalUDSub(e, "At least 2 parameters are needed: ISRANGE, SUB1 [, ...].");

  // default behavior: Exact like scalar indexing
  BaseGDL* isRange = e->GetKW(1);
  if (isRange == NULL)
    ThrowFromInternalUDSub(e, "Parameter 1 (ISRANGE) is undefined.");
  if (isRange->Rank() == 0)
    ThrowFromInternalUDSub(e, "Parameter 1 (ISRANGE) must be an array in this context: " + e->Caller()->GetString(e->GetTheKW(1)));
  SizeT nIsRange = isRange->N_Elements();
  if (nIsRange > (nParam - 2)) //- SELF and ISRANGE
    ThrowFromInternalUDSub(e, "Parameter 1 (ISRANGE) must have " + i2s(nParam - 2) + " elements.");
  Guard<DLongGDL> isRangeLongGuard;
  DLongGDL* isRangeLong;
  if (isRange->Type() == GDL_LONG)
    isRangeLong = static_cast<DLongGDL*> (isRange);
  else {
    try {
      isRangeLong = static_cast<DLongGDL*> (isRange->Convert2(GDL_LONG, BaseGDL::COPY));
    } catch (GDLException& ex) {
      ThrowFromInternalUDSub(e, ex.ANTLRException::getMessage());
    }
    isRangeLongGuard.Reset(isRangeLong);
  }

  ArrayIndexVectorT ixList;
  //   IxExprListT exprList;
  try {
    for (int p = 0; p < nIsRange; ++p) {
      BaseGDL* parX = e->GetKW(p + 2); // implicit SELF, ISRANGE, par1..par8
      if (parX == NULL)
        ThrowFromInternalUDSub(e,
        "Parameter is undefined: " + e->Caller()->GetString(e->GetTheKW(p + 2)));
      DLong isRangeX = (*isRangeLong)[p];
      if (isRangeX != 0 && isRangeX != 1)
        ThrowFromInternalUDSub(e,
        "Value of parameter 1 (ISRANGE[" + i2s(p) + "]) is out of allowed range.");
      if (isRangeX == 1) {
        if (parX->N_Elements() != 3)
          ThrowFromInternalUDSub(e, "Range vector must have 3 elements: " +
          e->Caller()->GetString(e->GetTheKW(p + 2)));
        Guard<DLongGDL> parXLongGuard;
        DLongGDL* parXLong;
        if (parX->Type() == GDL_LONG)
          parXLong = static_cast<DLongGDL*> (parX);
        else {
          try {
            parXLong = static_cast<DLongGDL*> (parX->Convert2(GDL_LONG, BaseGDL::COPY));
          } catch (GDLException& ex) {
            ThrowFromInternalUDSub(e, ex.ANTLRException::getMessage());
          }
          parXLongGuard.Reset(parXLong);
        }
        // negative end ix is fine -> CArrayIndexRangeS can handle [b:*:s] ([b,-1,s])
        ixList.push_back(new CArrayIndexRangeS((*parXLong)[0], (*parXLong)[1], (*parXLong)[2]));
      } else // non-range
      {
        // ATTENTION: These two grab c1 (all others don't)
        // a bit unclean, but for maximum efficiency
        if (parX->Rank() == 0)
          ixList.push_back(new CArrayIndexScalar(parX->Dup()));
        else
          ixList.push_back(new CArrayIndexIndexed(parX->Dup()));
      }
    } // for
  }  catch (GDLException& ex) {
    ixList.Destruct(); // ixList is not valid afterwards, but as we throw this is ok
    throw ex;
  }

  ArrayIndexListT* aL;
  MakeArrayIndex(&ixList, &aL, NULL); // important to get the non-NoAssoc ArrayIndexListT
  // because only they clean up ixList on destruction
  Guard< ArrayIndexListT> aLGuard(aL);

  IxExprListT ixL;
  return aL->Index(e->GetTheKW(0), ixL); // index SELF
}

BaseGDL* _GDL_OBJECT_OverloadEQOp(EnvUDT* e) {
  SizeT nParam = e->NParam(); // number of parameters actually given
  //   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if (nParam < 2) // consider implicit SELF
    ThrowFromInternalUDSub(e, "2 parameters are needed: LEFT, RIGHT.");

  // default behavior: Exact like scalar indexing
  BaseGDL* l = e->GetKW(1);
  if (l->Type() != GDL_OBJ)
    ThrowFromInternalUDSub(e, "Unable to convert parameter #1 to type object reference.");

  BaseGDL* r = e->GetKW(2);
  if (r->Type() != GDL_OBJ)
    ThrowFromInternalUDSub(e, "Unable to convert parameter #2 to type object reference.");

  DObjGDL* left = static_cast<DObjGDL*> (l);
  DObjGDL* right = static_cast<DObjGDL*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = left->N_Elements();
  //   if( nEl == 0)
  //     nEl=N_Elements();
  assert(rEl);
  assert(nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");

  Data_<SpDByte>* res;

  DObj s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(left->Dim(), BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = (s == (*left)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*left)[i] == s);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*left)[i] == s);
    }
  } else if (left->StrictScalar(s)) {
    res = new Data_<SpDByte>(right->Dim(), BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] == s);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( rEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < rEl; ++i)  (*res)[i] = ((*right)[i] == s);
    } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < rEl; ++i)  (*res)[i] = ((*right)[i] == s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->Dim(), BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( rEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == (*left)[i]);
    } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] == (*left)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(left->Dim(), BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] == (*left)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] == (*left)[i]);
    } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] == (*left)[i]);
    }
  }
  return res;
}

BaseGDL* _GDL_OBJECT_OverloadNEOp(EnvUDT* e) {
  SizeT nParam = e->NParam(); // number of parameters actually given
  //   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if (nParam < 3) // consider implicit SELF
    ThrowFromInternalUDSub(e, "Two parameters are needed: LEFT, RIGHT.");

  // default behavior: Exact like scalar indexing
  BaseGDL* l = e->GetKW(1);
  if (l->Type() != GDL_OBJ)
    ThrowFromInternalUDSub(e, "Unable to convert parameter #1 to type object reference.");

  BaseGDL* r = e->GetKW(2);
  if (r->Type() != GDL_OBJ)
    ThrowFromInternalUDSub(e, "Unable to convert parameter #2 to type object reference.");

  DObjGDL* left = static_cast<DObjGDL*> (l);
  DObjGDL* right = static_cast<DObjGDL*> (r);

  ULong rEl = right->N_Elements();
  ULong nEl = left->N_Elements();
  //   if( nEl == 0)
  //     nEl=N_Elements();
  assert(rEl);
  assert(nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");

  Data_<SpDByte>* res;

  DObj s;
  if (right->StrictScalar(s)) {
    res = new Data_<SpDByte>(left->Dim(), BaseGDL::NOZERO);
    if (nEl == 1) {
      (*res)[0] = (s != (*left)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl, TP_MEMORY_ACCESS))==1) {
       for (OMPInt i = 0; i < nEl; ++i)  (*res)[i] = ((*left)[i] != s);
   } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i)  (*res)[i] = ((*left)[i] != s);
    }
  } else if (left->StrictScalar(s)) {
    res = new Data_<SpDByte>(right->Dim(), BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] != s);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( rEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != s);
    } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != s);
    }
  } else if (rEl < nEl) {
    res = new Data_<SpDByte>(right->Dim(), BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( rEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != (*left)[i]);
    } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < rEl; ++i) (*res)[i] = ((*right)[i] != (*left)[i]);
    }
  } else // ( rEl >= nEl)
  {
    res = new Data_<SpDByte>(left->Dim(), BaseGDL::NOZERO);
    if (rEl == 1) {
      (*res)[0] = ((*right)[0] != (*left)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl, TP_MEMORY_ACCESS))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] != (*left)[i]);
    } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (OMPInt i = 0; i < nEl; ++i) (*res)[i] = ((*right)[i] != (*left)[i]);
    }
  }
  return res;
}

// for GDL_OBJECT all other overloads are illegal operations as default
// however, they need to be accessible for (nonsense) parent method calls.
// But we can use just this one function for all of them

BaseGDL* _GDL_OBJECT_OverloadReportIllegalOperation(EnvUDT* e) {
  ThrowFromInternalUDSub(e, "Operation illegal with object reference types.");
  return 0;
}

// set up the _overload... subroutines for GDL_OBJECT
//needeed here
namespace lib {
BaseGDL* class_name_to_obj_new(EnvT* e);
}

const char KLISTEND[] = "";

void RegisterFun(DStructDesc* o, const std::string &n,  BaseGDL* (*fun_)( EnvUDT*) , int npar=0, const string *keylist=NULL) {
  assert (o->FindInFunList(n) ==NULL);//no because .RESET
  DFun *f = new DFun(n, o->Name(), INTERNAL_LIBRARY_STR);
  if (keylist) {
    int ikey=0;
    while (keylist[ikey] != "") {
      f->AddKey(keylist[ikey],keylist[ikey]);
      ikey++;
    }
  }
  if (npar > 0) {
    for (int ipar = 0; ipar < npar; ipar++) f->AddPar("PAR" + i2s(ipar));
  }
  WRAPPED_FUNNode* treeFun = new WRAPPED_FUNNode(fun_);
  f->SetTree(treeFun);
  funMap[o->Name()+"::"+n]=o->FunList().size();
  o->FunList().push_back(f);
}
void RegisterOperatorFun(int op, DStructDesc* o, const std::string &n,  BaseGDL* (*fun_)( EnvUDT*) , int npar=0, const string *keylist=NULL) {
  assert (o->FindInFunList(n) ==NULL);//no because .RESET
  DFun *f = new DFun(n, o->Name(), INTERNAL_LIBRARY_STR);
  if (keylist) {
    int ikey=0;
    while (keylist[ikey] != "") {
      f->AddKey(keylist[ikey],keylist[ikey]);
      ikey++;
    }
  }
  if (npar > 0) {
    for (int ipar = 0; ipar < npar; ipar++) f->AddPar("PAR" + i2s(ipar));
  }
  WRAPPED_FUNNode* treeFun = new WRAPPED_FUNNode(fun_);
  f->SetTree(treeFun);
  funMap[o->Name()+"::"+n]=o->FunList().size();
  o->FunList().push_back(f);
  o->SetOperator(op,f);
}
void RegisterPro(DStructDesc* o, const std::string n, void (*pro_)(EnvUDT*), int npar=0, const string *keylist = NULL) {
//  assert (o->FindInProList(n) ==NULL);//no because .RESET
  DPro *p = new DPro(n, o->Name(), INTERNAL_LIBRARY_STR);
  if (keylist) {
    int ikey = 0;
    while (keylist[ikey] != "") {
      p->AddKey(keylist[ikey], keylist[ikey]);
      ikey++;
    }
  }
  if (npar > 0) {
    for (int ipar = 0; ipar < npar; ipar++) p->AddPar("PAR" + i2s(ipar));
  }
  WRAPPED_PRONode* treePro = new WRAPPED_PRONode(pro_);
  p->SetTree(treePro);
  proMap[o->Name() + "::" + n] = o->ProList().size();
  o->ProList().push_back(p);
}

void RegisterOperatorPro(int op, DStructDesc* o, const std::string n, void (*pro_)(EnvUDT*), int npar=0, const string *keylist = NULL) {
//  assert (o->FindInProList(n) ==NULL);//no because .RESET
  DPro *p = new DPro(n, o->Name(), INTERNAL_LIBRARY_STR);
  if (keylist) {
    int ikey = 0;
    while (keylist[ikey] != "") {
      p->AddKey(keylist[ikey], keylist[ikey]);
      ikey++;
    }
  }
  if (npar > 0) {
    for (int ipar = 0; ipar < npar; ipar++) p->AddPar("PAR" + i2s(ipar));
  }
  WRAPPED_PRONode* treePro = new WRAPPED_PRONode(pro_);
  p->SetTree(treePro);
  proMap[o->Name() + "::" + n] = o->ProList().size();
  o->ProList().push_back(p);
  o->SetOperator(op,p);
}
void SetupOverloadSubroutines() {
  //   // The call
  //   BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
  //   in call_fun eventually (in GDLInterpreter::statement) tree->Run() is called
  DStructDesc* gdlObjectDesc = FindInStructList(structList, GDL_OBJECT_NAME);
  assert(gdlObjectDesc != NULL);
  DStructDesc* gdlContainerDesc = FindInStructList(structList, "GDL_CONTAINER");
  assert(gdlContainerDesc != NULL);
  DStructDesc* listDesc = FindInStructList(structList, "LIST");
  assert(listDesc != NULL);
  DStructDesc* hashDesc = FindInStructList(structList, "HASH");
  assert(hashDesc != NULL);

  DStructDesc* gdl2gdlBridgeDesc = FindInStructList(structList, "IDL_IDLBRIDGE");
  assert(gdl2gdlBridgeDesc != NULL);
  
#ifdef USE_SHAPELIB
  DStructDesc* GDLffShapeDesc = FindInStructList(structList, "IDLFFSHAPE");
  assert(GDLffShapeDesc != NULL);
#endif

#ifdef USE_EXPAT
  DStructDesc* GDLffXmlSaxDesc = FindInStructList(structList, "IDLFFXMLSAX");
  assert(GDLffXmlSaxDesc != NULL);
#endif

  WRAPPED_FUNNode *treeFun;
  WRAPPED_PRONode *treePro;
  //Need to define a LibFunRetNew() for "GDL_OBJECT_NAME" and "GDL_CONTAINER_NAME" as "gdl_object__define.pro" does not exist, so the
  //general mechanism to use equivalently x=class(params...) or x=obj_new("class", params...) does not work if a class__define.pro is not present in !PATH
  const string keyNames[] = {"_EXTRA", ""};
  new DLibFunRetNew(lib::class_name_to_obj_new, GDL_OBJECT_NAME, 100, keyNames);
  new DLibFunRetNew(lib::class_name_to_obj_new, GDL_CONTAINER_NAME, 100, keyNames);

  // we are NOT setting the operator to have (faster) default behavior
  // the functions must be there nevertheless for explicit callingNode
  // that's why we add them to the functions list
  RegisterOperatorFun(OOIsTrue, gdlObjectDesc, "_OVERLOADISTRUE", _GDL_OBJECT_OverloadIsTrue);
  // GDL_OBJECT:: [
  RegisterOperatorPro(OOBracketsLeftSide, gdlObjectDesc, "_OVERLOADBRACKETSLEFTSIDE", _GDL_OBJECT_OverloadBracketsLeftSide,11);
  // GDL_OBJECT::INIT()
  RegisterFun(gdlObjectDesc, "INIT", _GDL_OBJECT_Init);
  // GDL_OBJECT:: ]
  RegisterOperatorFun(OOBracketsRightSide, gdlObjectDesc, "_OVERLOADBRACKETSRIGHTSIDE", _GDL_OBJECT_OverloadBracketsRightSide,9);
  // GDL_OBJECT:: =
  RegisterOperatorFun(OOEQ, gdlObjectDesc, "_OVERLOADEQ", _GDL_OBJECT_OverloadEQOp,2);
  // GDL_OBJECT:: !=
  RegisterOperatorFun(OONE, gdlObjectDesc, "_OVERLOADNE", _GDL_OBJECT_OverloadNEOp,2);
  // GDL_OBJECT:: +
  RegisterOperatorFun(OOPlus, gdlObjectDesc, "_OVERLOADPLUS", _GDL_OBJECT_OverloadReportIllegalOperation,2);
  // GDL_OBJECT:: -
  RegisterOperatorFun(OOMinus,gdlObjectDesc, "_OVERLOADMINUS", _GDL_OBJECT_OverloadReportIllegalOperation,2);
  
  
  // GDL_CONTAINER 
  // - uses corresponding list:: procedures because we are lazy and because LIST was written before GDL_CONTAINER (!).
  // Some keywords/parameters thus do not exist in the GDL_CONTAINER:: object but this should not be muech of a problem, eh?
  // GDL_CONTAINER::ADD
  const string listaddk[] = {"EXTRACT", "POSITION", "NO_COPY", KLISTEND}; //this because we use list_add. GDL_CONTAINER::ADD has less parameters and keywords.
  RegisterPro(gdlContainerDesc, "ADD", lib::list__add, 1, listaddk);
  // GDL_CONTAINER::CLEANUP
  RegisterPro(gdlContainerDesc, "CLEANUP", lib::container__cleanup);
  // GDL_CONTAINER::COUNT()
  RegisterFun(gdlContainerDesc, "COUNT", lib::list__count); //no "VALUE" contrary to LIST::
  // GDL_CONTAINER::EQUALS()
  RegisterFun(gdlContainerDesc, "EQUALS", lib::container__equals, 1);
  // GDL_CONTAINER::GET
  const string contgetk[] = {"ALL", "ISA", "NULL", "COUNT", "POSITION", KLISTEND};
  RegisterFun(gdlContainerDesc, "GET", lib::container__get, 0, contgetk);  //inherited by LIST
  // GDL_CONTAINER::INIT
  RegisterFun(gdlContainerDesc, "INIT", lib::container__init);
  // GDL_CONTAINER::ISCONTAINED()
  const string contiscontainedk[] = {"POSITION", KLISTEND};
  RegisterFun(gdlContainerDesc, "ISCONTAINED", lib::container__iscontained, 1, contiscontainedk);
  // GDL_CONTAINER::MOVE
  RegisterPro(gdlContainerDesc, "MOVE", lib::list__move, 2);
  // GDL_CONTAINER::REMOVE()
  const string contremovek[] = {"ALL", "POSITION", KLISTEND};
  RegisterPro(gdlContainerDesc, "REMOVE", lib::container__remove, 1, contremovek);

  // LIST:: ]
  RegisterOperatorFun(OOBracketsRightSide, listDesc, "_OVERLOADBRACKETSRIGHTSIDE", lib::LIST___OverloadBracketsRightSide,9);
  // LIST:: [
  RegisterOperatorPro(OOBracketsLeftSide,listDesc, "_OVERLOADBRACKETSLEFTSIDE", lib::LIST___OverloadBracketsLeftSide,11);
  // LIST:: +
  RegisterOperatorFun(OOPlus,listDesc, "_OVERLOADPLUS", lib::LIST___OverloadPlus,2);
  // LIST:: =
  RegisterOperatorFun(OOEQ,listDesc, "_OVERLOADEQ", lib::LIST___OverloadEQOp,2);
  // LIST:: !=
  RegisterOperatorFun(OONE,listDesc, "_OVERLOADNE", lib::LIST___OverloadNEOp,2);
  // LIST::
  RegisterOperatorFun(OOIsTrue,listDesc, "_OVERLOADISTRUE", lib::LIST___OverloadIsTrue);
  // LIST::ADD
  RegisterPro(listDesc, "ADD", lib::list__add, 2, listaddk);
  // LIST::REMOVE()
  const string listremovek[]={ "ALL", KLISTEND};
  RegisterFun(listDesc,"REMOVE",lib::list__remove_fun,1,listremovek);
  // LIST::REMOVE PRO
  RegisterPro(listDesc,"REMOVE",lib::list__remove_pro,1,listremovek);
  // LIST::REVERSE PRO
  RegisterPro(listDesc,"REVERSE",lib::list__reverse);
  // LIST::ToArray()
  const string listtoarrayk[]={ "TYPE","MISSING","DIMENSION", "NO_COPY", "PROMOTE_TYPE", "TRANSPOSE", KLISTEND};
  RegisterFun(listDesc,"TOARRAY",lib::list__toarray,0,listtoarrayk);
  // LIST::CLEANUP
  RegisterPro(listDesc,"CLEANUP",lib::list__cleanup);
  // LIST::MOVE
  RegisterPro(listDesc,"MOVE",lib::list__move,2);
  // LIST::SWAP
  RegisterPro(listDesc,"SWAP",lib::list__swap,2);
  // LIST::COUNT()
  RegisterFun(listDesc,"COUNT",lib::list__count,1);
  // LIST::ISEMPTY()
  RegisterFun(listDesc,"ISEMPTY",lib::list__isempty);
  // LIST::WHERE()
  const string listwherek[]={ "COMPLEMENT", "COUNT","NCOMPLEMENT", KLISTEND};
  RegisterFun(listDesc,"WHERE",lib::list__where,1,listwherek);

  // HASH
  // HASH:: ]
  RegisterOperatorFun(OOBracketsRightSide, hashDesc, "_OVERLOADBRACKETSRIGHTSIDE", lib::HASH___OverloadBracketsRightSide,9);
  // HASH:: [
  RegisterOperatorPro(OOBracketsLeftSide, hashDesc, "_OVERLOADBRACKETSLEFTSIDE", lib::HASH___OverloadBracketsLeftSide,11);
  // HASH:: +
  RegisterOperatorFun(OOPlus, hashDesc, "_OVERLOADPLUS", lib::HASH___OverloadPlus,2);
  // HASH:: =
  RegisterOperatorFun(OOEQ, hashDesc, "_OVERLOADEQ", lib::HASH___OverloadEQOp,2);
  // HASH:: !=
  RegisterOperatorFun(OONE, hashDesc, "_OVERLOADNE", lib::HASH___OverloadNEOp,2);
  // HASH::
  RegisterOperatorFun(OOIsTrue, hashDesc, "_OVERLOADISTRUE", lib::HASH___OverloadIsTrue);

 // HASH::REMOVE()
  RegisterFun(hashDesc,"REMOVE",lib::hash__remove_fun,1,listremovek);
  // LIST::REMOVE PRO
  RegisterPro(hashDesc,"REMOVE",lib::hash__remove_pro,1,listremovek);
  // HASH::HASKEY()
  RegisterFun(hashDesc,"HASKEY",lib::hash__haskey,1);
  // HASH::KEYS()
  RegisterFun(hashDesc,"KEYS",lib::hash__keys);
  // HASH::VALUES()
  RegisterFun(hashDesc,"VALUES",lib::hash__values);
  // HASH::TOSTRUCT()
  const string hastostructk[]={ "SKIPPED", "MISSING", "NO_COPY", "RECURSIVE", KLISTEND};
  RegisterFun(hashDesc,"TOSTRUCT",lib::hash__tostruct,0,hastostructk);
  // HASH::COUNT()
  RegisterFun(hashDesc,"COUNT",lib::hash__count,1);
  // HASH::ISEMPTY()
  RegisterFun(hashDesc,"ISEMPTY",lib::hash__isempty,1);
  // HASH::ISORDERED()
  RegisterFun(hashDesc,"ISORDERED",lib::hash__isordered,1);
  // HASH::ISFOLDCASE()
  RegisterFun(hashDesc,"ISFOLDCASE",lib::hash__isfoldcase,1);
  // HASH::WHERE()
  const string hashwherek[]={"COMPLEMENT", "COUNT","NCOMPLEMENT", KLISTEND};
  RegisterFun(hashDesc,"WHERE",lib::hash__where,1,hashwherek);
// HASH::SET PRO
  RegisterPro(hashDesc,"SET",lib::HASH__Set,2);

#ifdef USE_SHAPELIB
  //=============GDLffShape========================
  //IDLFFSHAPE::GETATTRIBUTES
  const string ffshpattributestructuresk[]={ "ATTRIBUTE_STRUCTURE", "ALL", KLISTEND};
  RegisterFun(GDLffShapeDesc,"GETATTRIBUTES",lib::GDLffShape___GetAttributes,1,ffshpattributestructuresk);
  //IDLFFSHAPE::GETENTITY
  const string ffshpattributesk[]={ "ATTRIBUTES", "ALL", KLISTEND};
  RegisterFun(GDLffShapeDesc,"GETENTITY",lib::GDLffShape___GetEntity,1,ffshpattributesk);
  //IDLFFSHAPE::INIT
  const string ffshpinitk[]={ "DBF_ONLY", "ENTITY_TYPE", "UPDATE", KLISTEND};
  RegisterFun(GDLffShapeDesc,"INIT",lib::GDLffShape___Init,1,ffshpinitk);
  //IDLFFSHAPE::OPEN
  RegisterFun(GDLffShapeDesc,"OPEN",lib::GDLffShape___Open,1,ffshpinitk);
  //IDLFFSHAPE::ADDATTRIBUTE
  const string ffshpaddattrk[]={ "PRECISION", KLISTEND};
  RegisterPro(GDLffShapeDesc,"ADDATTRIBUTE",lib::GDLffShape___AddAttribute,3,ffshpaddattrk);
  //IDLFFSHAPE::CLEANUP
  RegisterPro(GDLffShapeDesc,"CLEANUP",lib::GDLffShape___Cleanup);
  //IDLFFSHAPE::CLOSE
  RegisterPro(GDLffShapeDesc,"CLOSE",lib::GDLffShape___Close);
  //IDLFFSHAPE::DESTROYENTITY
  RegisterPro(GDLffShapeDesc,"DESTROYENTITY",lib::GDLffShape___DestroyEntity,1);
  //IDLFFSHAPE::GETPROPERTY
  const string ffshpgetpropk[]={"ATTRIBUTE_INFO","ATTRIBUTE_NAMES","ENTITY_TYPE","FILENAME","IS_OPEN","N_ATTRIBUTES","N_ENTITIES", "N_RECORDS", KLISTEND};
  RegisterPro(GDLffShapeDesc,"GETPROPERTY",lib::GDLffShape___GetProperty,0,ffshpgetpropk);
  //IDLFFSHAPE::PUTENTITY
  RegisterPro(GDLffShapeDesc,"PUTENTITY",lib::GDLffShape___PutEntity,1);
  //IDLFFSHAPE::SETATTRIBUTES
  RegisterPro(GDLffShapeDesc,"SETATTRIBUTES",lib::GDLffShape___SetAttributes,3);
#endif
DFun* DFunlist;
DPro* DProlist;
#ifdef USE_EXPAT

  //IDLFFXMLSAX::INIT
  RegisterFun(GDLffXmlSaxDesc,"INIT",lib::GDLffXmlSax___Init);
  //IDLFFXMLSAX::CLEANUP
  RegisterPro(GDLffXmlSaxDesc,"CLEANUP",lib::GDLffXmlSax___Cleanup);
  //IDLFFXMLSAX::PARSEFILE
  const string saxparsefk[] = {"URL", "XML_STRING", KLISTEND};
  RegisterPro(GDLffXmlSaxDesc,"PARSEFILE",lib::GDLffXmlSax__ParseFile,1,saxparsefk);
  RegisterPro(GDLffXmlSaxDesc,"ATTRIBUTEDECL",lib::GDLffXmlSax__AttributeDecl,5);
  RegisterPro(GDLffXmlSaxDesc,"CHARACTERS",lib::GDLffXmlSax__Characters,1);
  RegisterPro(GDLffXmlSaxDesc,"COMMENT",lib::GDLffXmlSax__Comment,1);
  RegisterPro(GDLffXmlSaxDesc,"ELEMENTDECL",lib::GDLffXmlSax__ElementDecl,2);
  RegisterPro(GDLffXmlSaxDesc,"ENDCDATA",lib::GDLffXmlSax__EndCDATA);
  RegisterPro(GDLffXmlSaxDesc,"ENDDOCUMENT",lib::GDLffXmlSax__EndDocument);
  RegisterPro(GDLffXmlSaxDesc,"ENDDTD",lib::GDLffXmlSax__EndDTD);
  RegisterPro(GDLffXmlSaxDesc,"ENDELEMENT",lib::GDLffXmlSax__EndElement,3);
  RegisterPro(GDLffXmlSaxDesc,"ENDENTITY",lib::GDLffXmlSax__EndEntity,1);
  RegisterPro(GDLffXmlSaxDesc,"ENDPREFIXMAPPING",lib::GDLffXmlSax__EndPrefixMapping,1);
  RegisterPro(GDLffXmlSaxDesc,"ERROR",lib::GDLffXmlSax__Error,4);
  RegisterPro(GDLffXmlSaxDesc,"EXTERNALENTITYDECL",lib::GDLffXmlSax__ExternalEntityDecl,3);
  RegisterPro(GDLffXmlSaxDesc,"FATALERROR",lib::GDLffXmlSax__FatalError,4);
  RegisterPro(GDLffXmlSaxDesc,"IGNORABLEWHITESPACE",lib::GDLffXmlSax__IgnorableWhitespace,1);
  RegisterPro(GDLffXmlSaxDesc,"INTERNALENTITYDECL",lib::GDLffXmlSax__InternalEntityDecl,2);
  RegisterPro(GDLffXmlSaxDesc,"NOTATIONDECL",lib::GDLffXmlSax__NotationDecl,3);
  RegisterPro(GDLffXmlSaxDesc,"PROCESSINGINSTRUCTION",lib::GDLffXmlSax__ProcessingInstruction,2);
  RegisterPro(GDLffXmlSaxDesc,"SKIPPEDENTITY",lib::GDLffXmlSax__SkippedEntity,1);
  RegisterPro(GDLffXmlSaxDesc,"STARTCDATA",lib::GDLffXmlSax__StartCDATA);
  RegisterPro(GDLffXmlSaxDesc,"STARTDOCUMENT",lib::GDLffXmlSax__StartDocument);
  RegisterPro(GDLffXmlSaxDesc,"STARTDTD",lib::GDLffXmlSax__StartDTD,3);
  RegisterPro(GDLffXmlSaxDesc,"STARTELEMENT",lib::GDLffXmlSax__StartElement,5);
  RegisterPro(GDLffXmlSaxDesc,"STARTENTITY",lib::GDLffXmlSax__StartEntity,1);
  RegisterPro(GDLffXmlSaxDesc,"STARTPREFIXMAPPING",lib::GDLffXmlSax__StartPrefixmapping,2);
  RegisterPro(GDLffXmlSaxDesc,"STOPPARSING",lib::GDLffXmlSax__StopParsing);
  RegisterPro(GDLffXmlSaxDesc,"UNPARSEDENTITYDECL",lib::GDLffXmlSax__UnparsedEntityDecl,2);
  RegisterPro(GDLffXmlSaxDesc,"WARNING",lib::GDLffXmlSax__Warning,4);
  const string saxgetpropk[] = {"VALIDATION_MODE", "SCHEMA_CHECKING","PARSER_URI","PARSER_PUBLICID","PARSER_LOCATION","NAMESPACE_PREFIXES","FILENAME", KLISTEND};
  RegisterPro(GDLffXmlSaxDesc,"GETPROPERTY",lib::GDLffXmlSax__GetProperty,0,saxgetpropk);
  const string saxsetpropk[] = {"NAMESPACE_PREFIXES","SCHEMA_CHECKING","VALIDATION_MODE", KLISTEND};
  RegisterPro(GDLffXmlSaxDesc,"SETPROPERTY",lib::GDLffXmlSax__SetProperty,0,saxsetpropk);
#endif
}
