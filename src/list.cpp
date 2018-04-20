/***************************************************************************
                          list.cpp  - for LIST objects
                             -------------------
    begin                : July 22 2013
    copyright            : (C) 2013 by M. Schellens et al.
    email                : m_schellens@users.sf.net
*******************************
* 2018 Apr 19: maintaining legacy LIST, introduce some of the new code
*   LIST_count - will be used externally
*   HASH_count - similarly
*  modifications particularly related to the re-definition 
*  of N_elements() function for lists and hashes (To be redefined
*  to the usual usage, as for any other object array.
*  For scalar list parameters, N_ELEMENTS( <a list>) give the number
*  of entries (LIST.count()). lib:: routine LIST_count is provided for that purpose.
*****
*  For the features it attempts, old list ("legacy list) does ok.
*  However for instance the usage
; alist[3] = fltarr(8) generates an error.  
*  
**********
*	April 4 2016: Greg Jung 
*   - fully implement TOARRAY, including DIMENSION and NO_COPY keywords.
*   -  list.MOVE, list.SWAP methods.
*   - [ and ] overloads revised to accomodate in-place array access to hashes
*     and arrays contained, including nested lists and hashes.
* 
*   Additional goodies:
*   list.help(MAXITEM) - produce help lines for items within the list.
*     MAXITEM - maximum # of items to print help on ( default=4, -1 = no max)
*    items are identified as "list item " xxx.
*    sub-lists are expanded recursively.
*  ::GET - used by both LIST and by GDL_CONTAINER
* LIST_Count(DstructGDL* Lstruct) - for use externally to get
* #items in the list.  This replaces the re-definition of N_Elements
* for lists (and hashes) which confuses the issue.
* macros used to declare static tag items
*  GDL_CONTAINER_STRUCT()
*  GDL_LIST_STRUCT()
*  GDL_CONTAINER_MODE()
* Also
* #define MAKE_LONGGDL(X, XLong) 
* Used to get a guarded long from parameter X, possibly converted.
* GDL_HASH_STRUCT() 			\
* GDL_HASHTABLEENTRY()
* For access to a hash embedded in a list.
* This is a stage1 list.cpp, so most of the new functionality is NOT
* included.
**************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// These macros are used in the new container-related routines

#define GDL_CONTAINER_STRUCT()			\
  static unsigned GDLContainerVersionTag = \
		structDesc::GDL_CONTAINER->TagIndex( "GDLCONTAINERVERSION"); \
  static unsigned pHeadTag = structDesc::GDL_CONTAINER->TagIndex( "PHEAD");	\
  static unsigned pTailTag = structDesc::GDL_CONTAINER->TagIndex( "PTAIL");	\
  static unsigned nListTag = structDesc::GDL_CONTAINER->TagIndex( "NLIST");

#define GDL_LIST_STRUCT()			\
  static unsigned GDLContainerVersionTag = \
		structDesc::GDL_CONTAINER->TagIndex( "GDLCONTAINERVERSION"); \
  static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");	\
  static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");	\
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

#define	GDL_CONTAINER_NODE()			\
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");	\
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

#define GDL_HASH_STRUCT() 			\
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS");	\
    static unsigned pTableTag = structDesc::HASH->TagIndex( "TABLE_DATA");	\
    static unsigned TableSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");	\
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");

#define GDL_HASHTABLEENTRY()			\
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY"); \
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

#define MAKE_LONGGDL(X, XLong) \
		DLongGDL* XLong=0; \
		Guard<DLongGDL> XLongGuard; \
		if( X != 0) \
		  if( X->Type() == GDL_LONG) \
			XLong = static_cast<DLongGDL*>( X); \
		  else  { \
			try{ \
			  XLong = static_cast<DLongGDL*>( X->Convert2( GDL_LONG, BaseGDL::COPY)); \
				} \
				catch( GDLException& ex) { \
				  ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage()); \
				} \
			XLongGuard.Init( XLong); \
		  }

#include "includefirst.hpp"

#include "nullgdl.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"
  
static bool trace_me(false);

namespace lib {
//	bool trace_arg();

	
  bool array_equal_bool( BaseGDL* p0, BaseGDL* p1,
			bool notypeconv=false, bool not_equal=false,
			bool quiet=true);
 
  void help_item( std::ostream& os,
		  BaseGDL* par, DString parString, bool doIndentation);
  void help_struct( std::ostream& os,  BaseGDL* par, int indent , bool debug );
}
  template< typename IndexT>
  void MergeSortDescending( IndexT* hhS, IndexT* h1, IndexT* h2, SizeT len) 
  {
    if( len <= 1) return;       

    SizeT h1N = len / 2;
    SizeT h2N = len - h1N;

    // 1st half
    MergeSortDescending(hhS, h1, h2, h1N);

    // 2nd half
    IndexT* hhM = &hhS[h1N]; 
    MergeSortDescending(hhM, h1, h2, h2N);

    SizeT i;
    for(i=0; i<h1N; ++i) h1[i] = hhS[ i];
    for(i=0; i<h2N; ++i) h2[i] = hhM[ i];

    SizeT  h1Ix = 0;
    SizeT  h2Ix = 0;
    for( i=0; (h1Ix < h1N) && (h2Ix < h2N); ++i) 
      {
	// the actual comparisson
	if( h1[h1Ix] < h2[h2Ix]) 
	  hhS[ i] = h2[ h2Ix++];
	else
	  hhS[ i] = h1[ h1Ix++];
      }
    for(; h1Ix < h1N; ++i) hhS[ i] = h1[ h1Ix++];
    for(; h2Ix < h2N; ++i) hhS[ i] = h2[ h2Ix++];
  }
  
  
  static DStructGDL* GetOBJ( BaseGDL* Objptr, EnvUDT* e)
  {
    if( Objptr == 0 || Objptr->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "Objptr not of type OBJECT. Please report.");
    if( !Objptr->Scalar())
      ThrowFromInternalUDSub( e, "Objptr must be a scalar. Please report.");
    DObjGDL* Object = static_cast<DObjGDL*>( Objptr);
    DObj ID = (*Object)[0];
    try {
      return BaseGDL::interpreter->GetObjHeap( ID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "Object ID <"+i2s(ID)+"> not found.");      
    }
  }
 
  DStructGDL* GetLISTStruct( EnvUDT* e, DPtr actP)
  {
// Get the GDL_CONTAINER_NODE struct {pNext, pData} from a list
//  From the heap pointer actP 
// the next pointer in list will be found in the struct.
    BaseGDL* actPHeap;
    try {
      actPHeap = BaseGDL::interpreter->GetHeap( actP);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      if( e == NULL)
			throw GDLException( "LIST container node ID <"
							+i2s(actP)+"> not found.");
      ThrowFromInternalUDSub( e, "LIST container node ID <"
							+i2s(actP)+"> not found.");      
    }
    if( actPHeap == NULL || actPHeap->Type() != GDL_STRUCT)
      {
	if( e == NULL)
	  throw GDLException( "LIST node must be a STRUCT.");
	else
	  ThrowFromInternalUDSub( e, "LIST node must be a STRUCT.");
      }
    DStructGDL* actPStruct = static_cast<DStructGDL*>( actPHeap);

    return actPStruct;
  }

  void FreeLISTNode( EnvUDT* e, DPtr pRemoveNode, bool deleteData = true)
  {
    static DString cNodeName("GDL_CONTAINER_NODE");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
    
    DStructGDL* removeNode = GetLISTStruct( e, pRemoveNode);
    
    DPtr pData = (*static_cast<DPtrGDL*>( removeNode->GetTag( pDataTag, 0)))[0];	      
    DPtr pNext = (*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0];	      
    
    if( deleteData)
      BaseGDL::interpreter->FreeHeap( pData); // delete
    else
      BaseGDL::interpreter->HeapErase( pData); // no delete
    
    // prevent cleanup due to ref-counting  
    (*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0] = 0;  
      
    BaseGDL::interpreter->FreeHeap( pRemoveNode); // delete
  }

  DPtr GetLISTNode( EnvUDT* e, DStructGDL* self, DLong targetIx)
  {
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
  
    DPtr actP;
    if( targetIx == -1)
    {
      actP = (*static_cast<DPtrGDL*>(self->GetTag( pHeadTag, 0)))[0];      
    }
    else
    {
      actP = (*static_cast<DPtrGDL*>(self->GetTag( pTailTag, 0)))[0];
      for( SizeT elIx = 0; elIx < targetIx; ++elIx)
      {
	DStructGDL* actPStruct = GetLISTStruct(e, actP);

	actP = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
      }
    }
    return actP;
  }
  
  DStructGDL*GetSELF( BaseGDL* selfP, EnvUDT* e)
  {
    // TODO remove this checks (SELF is set always internally)
    if( selfP == NULL || selfP->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "SELF is not of type OBJECT. Please report.");
    if( !selfP->Scalar())
      ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT. Please report.");
    DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
    DObj selfID = (*selfObj)[0];
    try {
      return BaseGDL::interpreter->GetObjHeap( selfID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "SELF object ID <"+i2s(selfID)+"> not found.");      
    }
  }

  void LIST__ToStream( DStructGDL* oStructGDL, std::ostream& o, SizeT w, SizeT* actPosPtr)
  {	  
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
    
    SizeT nList = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nListTag, 0)))[0];
    DPtr pActNode = GetLISTNode( NULL, oStructGDL, 0);
    for( SizeT i=0; i<nList; ++i)
    {
      DStructGDL* actNode = GetLISTStruct( NULL, pActNode);   
      DPtr pData = (*static_cast<DPtrGDL*>(actNode->GetTag( pDataTag, 0)))[0];
      BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);

      if( data == NULL) data = NullGDL::GetSingleInstance();
      
      data->ToStream( o, w, actPosPtr);
      if( (i+1) < nList) o << std::endl;

      pActNode = (*static_cast<DPtrGDL*>(actNode->GetTag( pNextTag, 0)))[0];
    }
  }
  
  // for HEAP_GC
  void EnvBaseT::AddLIST( DPtrListT& ptrAccessible,
			  DPtrListT& objAccessible, DStructGDL* listStruct)
  {
    DStructGDL* self = listStruct;

    // here static is fine
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
//     vector<DPtr> listElementsID;
//     listElementsID.reserve( nList);

    DPtr actP = (*static_cast<DPtrGDL*>(self->GetTag( pTailTag, 0)))[0];
    for( SizeT elIx = 0; elIx < nList; ++elIx)
      {
	// no recursion here
	// PNEXT is handled within this loop instead
        ptrAccessible.insert( actP);

	DStructGDL* actPStruct = GetLISTStruct(NULL, actP);

	DPtr actPData = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pDataTag, 0)))[0];

	// the LIST is corrupted if this check fails,
	// but we quietly ignore it, as this is only about heap consistency
	if( actPData != 0 && interpreter->PtrValid( actPData))
	{
	  ptrAccessible.insert( actPData);
	  Add( ptrAccessible, objAccessible, interpreter->GetHeap( actPData));
	}
	
	actP = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
      }    
  }

  void LISTCleanup( EnvUDT* e, DStructGDL* self)
  {
    // here static is fine
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");

    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
    DPtr actP = (*static_cast<DPtrGDL*>(self->GetTag( pTailTag, 0)))[0];
    // swipe head and tail pointer
    (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;
    (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;	      
    for( SizeT elIx = 0; elIx < nList; ++elIx)
      {
	DStructGDL* actPStruct = GetLISTStruct(e, actP);

	DPtr actPNext = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
		
	// the key here: break the chain
	(*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0] = 0;
	
	GDLInterpreter::FreeHeap( actP); // deletes also PDATA
	
	actP = actPNext;
      }    
    (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;	      
  }

  
namespace lib {


void list_insertion( BaseGDL* theref, BaseGDL* rVal,
			ArrayIndexListT* ixList)
{		// This routine is simply copied from the new list.cpp
	DType destTy = theref->Type();
    ixList->SetVariable( theref);
	dimension dim = ixList->GetDim();
	if( rVal->Type() != destTy) 
		rVal = rVal->Convert2( destTy, BaseGDL::COPY);
  switch( destTy)
    {
    case GDL_BYTE:
      { Data_<SpDByte>* dest=static_cast<Data_<SpDByte>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break;}

    case GDL_INT:
      { Data_<SpDInt>* dest=static_cast<Data_<SpDInt>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break;}

    case GDL_UINT:
      { Data_<SpDUInt>* dest=static_cast<Data_<SpDUInt>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	 break; }

    case GDL_LONG:
      { Data_<SpDLong>* dest=static_cast<Data_<SpDLong>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_ULONG:
      { Data_<SpDULong>* dest=static_cast<Data_<SpDULong>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_LONG64:
      { Data_<SpDLong64>* dest=static_cast<Data_<SpDLong64>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_ULONG64:
      { Data_<SpDULong64>* dest=static_cast<Data_<SpDULong64>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_FLOAT: 
      { Data_<SpDFloat>* dest=static_cast<Data_<SpDFloat>* >(theref);
      	dest->AssignAt( rVal, ixList);
	break; }

    case GDL_DOUBLE: 
      { Data_<SpDDouble>* dest=static_cast<Data_<SpDDouble>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_STRING: 
      { Data_<SpDString>* dest=static_cast<Data_<SpDString>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_COMPLEX: 
      { Data_<SpDComplex>* dest=static_cast<Data_<SpDComplex>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_COMPLEXDBL: 
      { Data_<SpDComplexDbl>* dest=
	  static_cast<Data_<SpDComplexDbl>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break; }

    case GDL_STRUCT:
      { DStructGDL* dest=static_cast<DStructGDL* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break;}

    case GDL_PTR:
      { Data_<SpDPtr>* dest=static_cast<Data_<SpDPtr>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break;}
    case GDL_OBJ:
      { Data_<SpDObj>* dest=static_cast<Data_<SpDObj>* >(theref);
      	dest->AssignAt( rVal, ixList);
      	break;}

    }
    return;
}

  
  void list__cleanup( EnvUDT* e)
  {
    SizeT nParam = e->NParam(1); // SELF
	
    DStructGDL* self = GetSELF( e->GetKW( 0), e);
    
    LISTCleanup( e, self);
  }

  BaseGDL* LIST___OverloadIsTrue( EnvUDT* e)
  {
    SizeT nParam = e->NParam(1); // SELF
	
    DStructGDL* self = GetSELF( e->GetKW( 0), e);

    // here static is fine
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
    if( nList == 0)
      return new DByteGDL(0);
    else
      return new DByteGDL(1);
  }

  
  
BaseGDL* LIST___OverloadEQOp( EnvUDT* e);
BaseGDL* LIST___OverloadNEOp( EnvUDT* e)
{
  DByteGDL* result = static_cast<DByteGDL*>(LIST___OverloadEQOp( e));
  for( SizeT i=0; i<result->N_Elements(); ++i)
  {
    if( (*result)[i] == 0)
      (*result)[i] = 1;
    else
      (*result)[i] = 0;
  }
  return result;
}
BaseGDL* LIST___OverloadEQOp( EnvUDT* e)
{
  SizeT nParam = e->NParam(); // number of parameters actually given
//   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if( nParam < 3) // consider implicit SELF
    ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
  static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
  static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
  
  // default behavior: Exact like scalar indexing
  DStructGDL* leftStruct = NULL;
  DStructGDL* rightStruct = NULL;
  BaseGDL* l = e->GetKW(1);
  if( l == NULL)
    l = NullGDL::GetSingleInstance();
  if( l->Type() == GDL_OBJ)
  {
    DObjGDL* left = static_cast<DObjGDL*>(l);
    DObj leftID = (*left)[0];
    try {
      leftStruct = BaseGDL::interpreter->GetObjHeap( leftID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "Left parameter object ID <"+i2s(leftID)+"> not found.");      
    }
  }
  
  BaseGDL* r = e->GetKW(2);
  if( r == NULL)
  {
    if( leftStruct == NULL)
      ThrowFromInternalUDSub( e, "At least one parameter must be a LIST.");      	
    r = NullGDL::GetSingleInstance();
  }
  if( r->Type() == GDL_OBJ)
  {
    DObjGDL* right = static_cast<DObjGDL*>(r);
    DObj rightID = (*right)[0];
    try {
      rightStruct = BaseGDL::interpreter->GetObjHeap( rightID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "Right parameter object ID <"+i2s(rightID)+"> not found.");      
    }
  }
  
  if( rightStruct == NULL && leftStruct == NULL)
  {
      ThrowFromInternalUDSub( e, "At least one parameter must be a LIST.");      	      
  }
  
  if( leftStruct == NULL)
  {
      leftStruct = rightStruct;
      rightStruct = NULL;
      r = l;
  }
  
  DStructDesc* listDesc = leftStruct->Desc();
  if( listDesc != structDesc::LIST)
    ThrowFromInternalUDSub( e, "Parameter must be a LIST.");
  if( rightStruct != NULL && rightStruct->Desc() != structDesc::LIST)
    ThrowFromInternalUDSub( e, "Right parameter must be a LIST.");

  SizeT nListLeft = (*static_cast<DLongGDL*>(leftStruct->GetTag( nListTag, 0)))[0];
  SizeT nListRight = 0; 
  if( rightStruct != NULL)
  {
      nListRight = (*static_cast<DLongGDL*>(rightStruct->GetTag( nListTag, 0)))[0];
      if( nListRight == 0)
      {
	if( nListLeft == 0)
	  return new DByteGDL(1);
	else
	  return new DByteGDL(0);
      }
  }
  assert( rightStruct == NULL || nListRight > 0);
  
  if( nListLeft == 0)
  {    
    return new DByteGDL(0);
  }

  if( rightStruct != NULL)
  {
    SizeT nEl = (nListLeft > nListRight) ? nListLeft : nListRight;
    DByteGDL* result = new DByteGDL( dimension( nEl));
    Guard<BaseGDL> resultGuard( result);
    DPtr pActLNode = GetLISTNode( e, leftStruct, 0);
    DPtr pActRNode = GetLISTNode( e, rightStruct, 0);
    for( SizeT i=0; i<nEl; ++i)
    {
      DStructGDL* actLNode = GetLISTStruct( e, pActLNode);   
      DStructGDL* actRNode = GetLISTStruct( e, pActRNode);   
      
      DPtr pDataL = (*static_cast<DPtrGDL*>(actLNode->GetTag( pDataTag, 0)))[0];
      BaseGDL* dataL = BaseGDL::interpreter->GetHeap( pDataL);
      DPtr pDataR = (*static_cast<DPtrGDL*>(actRNode->GetTag( pDataTag, 0)))[0];
      BaseGDL* dataR = BaseGDL::interpreter->GetHeap( pDataR);
      if( dataL == NULL || dataL == NullGDL::GetSingleInstance())
      {
        if( dataR == NULL || dataR == NullGDL::GetSingleInstance())
	  (*result)[ i] = 1;
      }
      else if( dataR != NULL && dataR != NullGDL::GetSingleInstance())
      {
	if( dataL->EqType(dataR))
	{
	  BaseGDL* eqRes = dataL->EqOp( dataR);
	  if( eqRes->Type() != GDL_BYTE)
	  {
	    Guard<BaseGDL> eqResGuardTmp( eqRes);
	    eqRes = eqRes->Convert2( GDL_BYTE, BaseGDL::CONVERT);
	    eqResGuardTmp.Release();
	  }
	  Guard<BaseGDL> eqResGuard( eqRes);
	  DByteGDL* eqResByte = static_cast<DByteGDL*>(eqRes);
	  SizeT c = 0;
	  for( c=0; c<eqResByte->N_Elements(); ++c)
	    if( !((*eqResByte)[ c]))
	      break;
	  if( c == eqResByte->N_Elements())
	    (*result)[ i] = 1;	  
	}
	else
	{
	  BaseGDL* rConvert = dataR->Convert2(dataL->Type(),BaseGDL::COPY);
	  Guard<BaseGDL> rCovertGuard( rConvert);
	  BaseGDL* eqRes = dataL->EqOp( rConvert);
	  if( eqRes->Type() != GDL_BYTE)
	  {
	    Guard<BaseGDL> eqResGuardTmp( eqRes);
	    eqRes = eqRes->Convert2( GDL_BYTE, BaseGDL::CONVERT);
	    eqResGuardTmp.Release();
	  }
	  Guard<BaseGDL> eqResGuard( eqRes);
	  DByteGDL* eqResByte = static_cast<DByteGDL*>(eqRes);
	  SizeT c = 0;
	  for( c=0; c<eqResByte->N_Elements(); ++c)
	    if( !((*eqResByte)[ c]))
	      break;
	  if( c == eqResByte->N_Elements())
	    (*result)[ i] = 1;	  
	}
      }
      // advance to next node
      pActLNode = (*static_cast<DPtrGDL*>(actLNode->GetTag( pNextTag, 0)))[0];
      pActRNode = (*static_cast<DPtrGDL*>(actRNode->GetTag( pNextTag, 0)))[0];
    }
    resultGuard.Release();
    return result;
  }
  else
  {
    SizeT nEl = nListLeft;
    DByteGDL* result = new DByteGDL( dimension( nEl));
    Guard<BaseGDL> resultGuard( result);
    DPtr pActLNode = GetLISTNode( e, leftStruct, 0);
    BaseGDL* dataR = r;
    for( SizeT i=0; i<nEl; ++i)
    {
      DStructGDL* actLNode = GetLISTStruct( e, pActLNode);   
      
      DPtr pDataL = (*static_cast<DPtrGDL*>(actLNode->GetTag( pDataTag, 0)))[0];
      BaseGDL* dataL = BaseGDL::interpreter->GetHeap( pDataL);
      if( dataL == NULL || dataL == NullGDL::GetSingleInstance())
      {
        if( dataR == NULL || dataR == NullGDL::GetSingleInstance())
	  (*result)[ i] = 1;
      }
      else if( dataR != NULL && dataR != NullGDL::GetSingleInstance())
      {
	if( dataL->EqType(dataR))
	{
	  BaseGDL* eqRes = dataL->EqOp( dataR);
	  if( eqRes->Type() != GDL_BYTE)
	  {
	    Guard<BaseGDL> eqResGuardTmp( eqRes);
	    eqRes = eqRes->Convert2( GDL_BYTE, BaseGDL::CONVERT);
	    eqResGuardTmp.Release();
	  }
	  Guard<BaseGDL> eqResGuard( eqRes);
	  DByteGDL* eqResByte = static_cast<DByteGDL*>(eqRes);
	  SizeT c = 0;
	  for( c=0; c<eqResByte->N_Elements(); ++c)
	    if( !((*eqResByte)[ c]))
	      break;
	  if( c == eqResByte->N_Elements())
	    (*result)[ i] = 1;	  
	}
	else
	{
	  
	  BaseGDL* eqRes = NULL;

	  DType aTy=dataL->Type();
	  DType bTy=dataR->Type();
	  if( DTypeOrder[aTy] > DTypeOrder[bTy])
	    {
	      // convert b to a
	      BaseGDL* rConvert = dataR->Convert2(dataL->Type(),BaseGDL::COPY);
	      Guard<BaseGDL> rConvertGuard(rConvert);
	      eqRes = dataL->EqOp( rConvert);
	    }
	  else
	    {
	      // convert a to b
	      BaseGDL* lConvert = dataL->Convert2(dataR->Type(),BaseGDL::COPY);
	      Guard<BaseGDL> lConvertGuard(lConvert);
	      eqRes = dataR->EqOp( lConvert);
	    }
	  	  
	  if( eqRes->Type() != GDL_BYTE)
	  {
	    Guard<BaseGDL> eqResGuardTmp( eqRes);
	    eqRes = eqRes->Convert2( GDL_BYTE, BaseGDL::CONVERT);
	    eqResGuardTmp.Release();
	  }
	  Guard<BaseGDL> eqResGuard( eqRes);
	  DByteGDL* eqResByte = static_cast<DByteGDL*>(eqRes);
	  SizeT c = 0;
	  for( c=0; c<eqResByte->N_Elements(); ++c)
	    if( !((*eqResByte)[ c]))
	      break;
	  if( c == eqResByte->N_Elements())
	    (*result)[ i] = 1;	  
	}
      }
      // advance to next node
      pActLNode = (*static_cast<DPtrGDL*>(actLNode->GetTag( pNextTag, 0)))[0];
    }
    resultGuard.Release();
    return result;    
  }
  
}
  
  
  BaseGDL* LIST___OverloadPlus( EnvUDT* e)
  {
    SizeT nParam = e->NParam(); // number of parameters actually given
    //   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
    if( nParam < 3) 
      ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
    
    // default behavior: Exact like scalar indexing
    BaseGDL* l = e->GetKW(1);
    if(l == NULL || l->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "Left parameter must be a LIST.");

    BaseGDL* r = e->GetKW(2);
    if(r == NULL || r->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "Right parameter must be a LIST.");

    DObjGDL* left = static_cast<DObjGDL*>(l);
    DObjGDL* right = static_cast<DObjGDL*>(r);

    DObj leftID = (*left)[0];
    DObj rightID = (*right)[0];
    DStructGDL* leftStruct;
    DStructGDL* rightStruct;
    try {
      leftStruct = BaseGDL::interpreter->GetObjHeap( leftID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "Left parameter object ID <"+i2s(leftID)+"> not found.");      
    }
    try {
      rightStruct = BaseGDL::interpreter->GetObjHeap( rightID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "Right parameter object ID <"+i2s(rightID)+"> not found.");      
    }

    DStructDesc* listDesc = leftStruct->Desc();
    if( listDesc != structDesc::LIST)
      ThrowFromInternalUDSub( e, "Left parameter must be a LIST.");
    if( rightStruct->Desc() != structDesc::LIST)
      ThrowFromInternalUDSub( e, "Right parameter must be a LIST.");
  
    SizeT nListLeft = (*static_cast<DLongGDL*>(leftStruct->GetTag( nListTag, 0)))[0];
    SizeT nListRight = (*static_cast<DLongGDL*>(rightStruct->GetTag( nListTag, 0)))[0];

    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    BaseGDL* newObj = new DObjGDL( objID); // the list object

    if( nListLeft == 0 && nListRight == 0)
      return newObj;
    
    Guard<BaseGDL> newObjGuard( newObj);

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;
    
    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    DPtr pActNode = GetLISTNode( e, (nListLeft > 0) ? leftStruct : rightStruct, 0);
    for( SizeT i=0; i<nListLeft+nListRight; ++i)
    {
      DStructGDL* actNode = GetLISTStruct( e, pActNode);   

      DPtr pData = (*static_cast<DPtrGDL*>(actNode->GetTag( pDataTag, 0)))[0];
      BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
      if( data != NULL) 
	data = data->Dup();

      DPtr dID = e->Interpreter()->NewHeap(1,data);
      
      cStruct = new DStructGDL( containerDesc, dimension());
      cID = e->Interpreter()->NewHeap(1,cStruct);
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = dID;
      
      if( cStructLast != NULL)
	(*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
      else
      { // 1st element
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
      }
	    
      cStructLast = cStruct;
      
      if( (i+1) == nListLeft && nListRight > 0)
	pActNode = GetLISTNode( e, rightStruct, 0);
      else
	pActNode = (*static_cast<DPtrGDL*>(actNode->GetTag( pNextTag, 0)))[0];
    }
    
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nListLeft+nListRight;      

    newObjGuard.Release();
    return newObj;
  }



BaseGDL* LIST___OverloadBracketsRightSide( EnvUDT* e)
{
  SizeT nParam = e->NParam(1); // number of parameters actually given
//   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if( nParam < 3) // consider implicit SELF
    ThrowFromInternalUDSub( e, "Two parameters are needed: ISRANGE, SUB1 [, ...].");
  if( nParam > 3) // consider implicit SELF
    ThrowFromInternalUDSub( e, "Only one dimensional access allowed.");

  BaseGDL* selfP = e->GetKW( 0);
  if( selfP->Type() != GDL_OBJ)
    ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
  if( !selfP->Scalar())
    ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");

  DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
  DObj selfID = (*selfObj)[0];
  DStructGDL* self = e->Interpreter()->GetObjHeap( selfID);
  

//   DStructDesc* listDesc= self->Desc();
// 
//   // because of .RESET_SESSION, we cannot use static here
//   DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;

  static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
  static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
  static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
  static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

  // default behavior: Exact like scalar indexing
  BaseGDL* isRange = e->GetKW(1);
  if( isRange == NULL)
    ThrowFromInternalUDSub( e, "Parameter 1 (ISRANGE) is undefined.");
//   if( isRange->Rank() == 0)
//     ThrowFromInternalUDSub( e, "Parameter 1 (ISRANGE) must be an array in this context: " + e->Caller()->GetString(e->GetKW(1)));
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
  catch( ...)
  {
    ixList.Destruct(); // ixList is not valid afterwards, but as we throw this is ok
    throw;
  }
  
  SizeT listSize = (*static_cast<DLongGDL*>(self->GetTag( nListTag, 0)))[0];
  
  ArrayIndexListT* aL;
  MakeArrayIndex( &ixList, &aL, NULL); // important to get the non-NoAssoc ArrayIndexListT
  // because only they clean up ixList on destruction
  Guard< ArrayIndexListT> aLGuard( aL);

  SpDLong t = SpDLong( dimension(listSize));
  aL->SetVariable( &t);
          
  AllIxBaseT* allIx = aL->BuildIx();
  
  // because of .RESET_SESSION, we cannot use static here
  DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;

  if( allIx->size() == 1)
  {
    DLong targetIx = allIx->operator[](0);
    if( targetIx == (listSize-1))
      targetIx = -1;
    DPtr actP = GetLISTNode( e, self, targetIx);

    DStructGDL* actPStruct = GetLISTStruct( e, actP);   

    actP = (*static_cast<DPtrGDL*>(actPStruct->GetTag( pDataTag, 0)))[0];
    
    BaseGDL* res = e->Interpreter()->GetHeap( actP);
    if( res == NULL)
      return NullGDL::GetSingleInstance();
    return res->Dup();
  }

  DStructDesc* listDesc= structDesc::LIST;
  
  DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
  DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
  BaseGDL* newObj = new DObjGDL( objID); // the list object
  Guard<BaseGDL> newObjGuard( newObj);
  // we need ref counting here as the LIST (newObj) is a regular return value
//   e->Interpreter()->IncRefObj( objID);

  DStructGDL* cStructLast = NULL;
  DStructGDL* cStruct = NULL;
  DPtr cID = 0;
  for( SizeT i=0; i<allIx->size(); ++i)
  {
    DLong actIx = allIx->operator[](i);
    if( actIx == (listSize-1))
      actIx = -1;
    DPtr pActNode = GetLISTNode( e, self, actIx);
    DStructGDL* actNode = GetLISTStruct( e, pActNode);   

    DPtr pData = (*static_cast<DPtrGDL*>(actNode->GetTag( pDataTag, 0)))[0];
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
    if( data != NULL) 
      data = data->Dup();
    DPtr dID = e->Interpreter()->NewHeap(1,data);
    
    cStruct = new DStructGDL( containerDesc, dimension());
    cID = e->Interpreter()->NewHeap(1,cStruct);
    (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = dID;
    
    if( cStructLast != NULL)
      (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
    else
    { // 1st element
      (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
    }
          
    cStructLast = cStruct;
  }
  
  (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
  (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = allIx->size();      

  newObjGuard.Release();
  return newObj;
}




void LIST___OverloadBracketsLeftSide( EnvUDT* e)
{
  // SELF
  //->AddPar("OBJREF")->AddPar("RVALUE")->AddPar("ISRANGE");
  //->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
  //->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");

  SizeT nParam = e->NParam(1); // number of parameters actually given
//   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
  if( nParam < 5) // consider implicit SELF
    ThrowFromInternalUDSub( e, "Four parameters are needed: OBJREF, RVALUE, ISRANGE, SUB1.");
  if( nParam > 5) // consider implicit SELF
    ThrowFromInternalUDSub( e, "Only one dimensional access allowed.");

  // handle DOT access
  bool dotAccess = false;

  BaseGDL** objRef = &e->GetKW(1);
  if( *objRef == NULL || *objRef == NullGDL::GetSingleInstance())
  {
    if( !e->GlobalKW(1))
       ThrowFromInternalUDSub( e, "Parameter 1 (OBJREF) is undefined.");    
    dotAccess = true;
  }

  BaseGDL* rValue = e->GetKW(2);
  if( rValue == NULL)
  {
    rValue = NullGDL::GetSingleInstance();
  }

  BaseGDL* selfP = e->GetKW( 0);
  if( selfP->Type() != GDL_OBJ)
    ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
  if( !selfP->Scalar())
    ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");

  DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
  DObj selfID = (*selfObj)[0];
  DStructGDL* self = e->Interpreter()->GetObjHeap( selfID);

  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
  static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

  // default behavior: Exact like scalar indexing
  BaseGDL* isRange = e->GetKW(3);
  if( isRange == NULL)
    ThrowFromInternalUDSub( e, "Parameter 2 (ISRANGE) is undefined.");
  SizeT nIsRange = isRange->N_Elements();
  if( nIsRange > (nParam - 4)) //- SELF and ISRANGE
    ThrowFromInternalUDSub( e, "Parameter 2 (ISRANGE) must have "+i2s(nParam-4)+" elements.");
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
      BaseGDL* parX = e->GetKW( p + 4); // implicit SELF, ISRANGE, par1..par8
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
  
  SizeT listSize = (*static_cast<DLongGDL*>(self->GetTag( nListTag, 0)))[0];
  
  ArrayIndexListT* aL;
  MakeArrayIndex( &ixList, &aL, NULL); // important to get the non-NoAssoc ArrayIndexListT
  // because only they clean up ixList on destruction
  Guard< ArrayIndexListT> aLGuard( aL);

  SpDLong t = SpDLong( dimension(listSize));
  aL->SetVariable( &t);
          
  AllIxBaseT* allIx = aL->BuildIx();
  
  SizeT allIxSize = allIx->size();
  
  if( dotAccess) // -> objRef is NULL (or !NULL)
  {
    if( rValue != NullGDL::GetSingleInstance())
    {
      ThrowFromInternalUDSub( e, "For struct access (OBJREF is !NULL), RVALUE must be !NULL as well.");      
    }
    if( allIxSize != 1)
      ThrowFromInternalUDSub( e, "Only single value struct access is allowed.");

    SizeT actIx = allIx->operator[](0);
    DPtr pActNode = GetLISTNode( e, self, actIx);
    DStructGDL* actNode = GetLISTStruct( e, pActNode);   
    *objRef = actNode->GetTag( pDataTag, 0)->Dup();
    return;
  }
  
  SizeT rValueSize= rValue->Size();
  if( rValueSize != allIxSize && rValueSize > 1)
    ThrowFromInternalUDSub( e, "Incorrect number of elements for Values ("+
    i2s(allIxSize)+" NE "+i2s(rValueSize)+").");

  if( rValueSize <= 1)
  {
    for( SizeT i=0; i<allIxSize; ++i)
    {
      SizeT actIx = allIx->operator[](i);
      DPtr pActNode = GetLISTNode( e, self, actIx);
      DStructGDL* actNode = GetLISTStruct( e, pActNode);   

      DPtr pData = (*static_cast<DPtrGDL*>(actNode->GetTag( pDataTag, 0)))[0];
      BaseGDL::interpreter->GetHeap( pData) = rValue->Dup();
    }
  }
  else
  {
    for( SizeT i=0; i<allIxSize; ++i)
    {
      SizeT actIx = allIx->operator[](i);
      DPtr pActNode = GetLISTNode( e, self, actIx);
      DStructGDL* actNode = GetLISTStruct( e, pActNode);   

      DPtr pData = (*static_cast<DPtrGDL*>(actNode->GetTag( pDataTag, 0)))[0];
      BaseGDL::interpreter->GetHeap( pData) = rValue->NewIx( i);
    }    
  }
  
}




template< typename DTypeGDL>
BaseGDL* LIST__ToArray( DLong nList, DPtr actP, BaseGDL* missingKW)
{
  static DString cNodeName("GDL_CONTAINER_NODE");
  // because of .RESET_SESSION, we cannot use static here
  static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
  static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

  DTypeGDL* missingT = NULL;
  Guard<DTypeGDL> missingTGuard;
  DTypeGDL* result = new DTypeGDL( dimension( nList), BaseGDL::NOZERO);
  Guard<DTypeGDL> resultGuard( result);
  for( SizeT i=0; i<nList; ++i)
  {

    DStructGDL* actNode = GetLISTStruct(NULL, actP);  

    DPtr pData = (*static_cast<DPtrGDL*>( actNode->GetTag( pDataTag, 0)))[0];   
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);

    if( data == NULL)
    {
      if( missingT == NULL)
      {
	if( missingKW == NULL)
	  throw GDLException( "Unable to convert to type : Element "+i2s(i));
	if( missingKW->Type() == DTypeGDL::t)
	  missingT = static_cast<DTypeGDL*>(missingKW);
	else
	{
	  missingT = static_cast<DTypeGDL*>(missingKW->Convert2(DTypeGDL::t,BaseGDL::COPY));
	  missingTGuard.Init( missingT);
	}
      }
      data = missingT;
    }
    assert( data != NULL);
    
    if( data->N_Elements() != 1)
    {
      throw GDLException( "Unable to convert to type (N_ELEMENTS > 1): Element "+i2s(i));		    
    }
    
    // we are not owner of 'data' here
    if( data->Type() == DTypeGDL::t)
      (*result)[i] = (*static_cast<DTypeGDL*>(data))[0];
    else
    {
      DTypeGDL* dataT = static_cast<DTypeGDL*>(data->Convert2(DTypeGDL::t,BaseGDL::COPY));
      (*result)[i] = (*static_cast<DTypeGDL*>(dataT))[0];
      delete dataT;
    }

    actP = (*static_cast<DPtrGDL*>( actNode->GetTag( pNextTag, 0)))[0];     		  
  }
  resultGuard.Release();
  return result;
}

BaseGDL* list__toarray( EnvUDT* e)
  {
    static int kwMISSINGIx = 0; 
    static int kwTYPEIx = 1;
    static int kwSELFIx = 2;

    SizeT nParam = e->NParam(1); // SELF
	
    DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);
    DStructDesc* listDesc= self->Desc();

    static DString listName("LIST");
    static DString cNodeName("GDL_CONTAINER_NODE");

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;

    assert( listDesc != NULL && listDesc->NTags() > 0);
    assert( containerDesc != NULL && containerDesc->NTags() > 0);

    // here static is fine
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
    if( nList == 0)
      return NullGDL::GetSingleInstance();

    BaseGDL* missingKW = e->GetKW( kwMISSINGIx);
    BaseGDL* typeKW = e->GetKW( kwTYPEIx);
    
    DType resultType = GDL_UNDEF;
    
    DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];
    DStructGDL* tailNode = GetLISTStruct(e, pTail);  
    
    DPtr pData = (*static_cast<DPtrGDL*>( tailNode->GetTag( pDataTag, 0)))[0];   
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
    
    if( typeKW == NULL)
    {
      if( data != NULL)
	resultType = data->Type();
      else if( missingKW != NULL)
	resultType = missingKW->Type();
      else
	ThrowFromInternalUDSub( e, "Unable to convert to type : Element zero");      	
    }
    else
    {
      if( typeKW->Type() == GDL_STRING)
      {
	DString typeStr = StrUpCase( (*static_cast<DStringGDL*>(typeKW))[0]);
	     if( SpDByte().TypeStr() == typeStr) resultType = GDL_BYTE;
	else if( SpDInt().TypeStr() == typeStr) resultType = GDL_INT;
	else if( SpDLong().TypeStr() == typeStr) resultType = GDL_LONG;
	else if( SpDFloat().TypeStr() == typeStr) resultType = GDL_FLOAT;
	else if( SpDDouble().TypeStr() == typeStr) resultType = GDL_DOUBLE;
	else if( SpDComplex().TypeStr() == typeStr) resultType = GDL_COMPLEX;
	else if( SpDString().TypeStr() == typeStr) resultType = GDL_STRING;
	else if( SpDComplexDbl().TypeStr() == typeStr) resultType = GDL_COMPLEXDBL;
	else if( SpDUInt().TypeStr() == typeStr) resultType = GDL_UINT;
	else if( SpDULong().TypeStr() == typeStr) resultType = GDL_ULONG;
	else if( SpDLong64().TypeStr() == typeStr) resultType = GDL_LONG64;
	else if( SpDULong64().TypeStr() == typeStr) resultType = GDL_ULONG64;
	else 
	  ThrowFromInternalUDSub( e, "Unknown or unable to convert to type " + typeStr);      	
      }
      else
      {
	DLongGDL* typeCodeKW;
	Guard<DLongGDL> typeCodeGuard;
	if( typeKW->Type() == GDL_LONG)
	{
	  typeCodeKW = static_cast<DLongGDL*>(typeKW);
	}
	else
	{
	  try{
	    typeCodeKW = static_cast<DLongGDL*>(typeKW->Convert2(GDL_LONG,BaseGDL::COPY));
	    typeCodeGuard.Init(typeCodeKW);
	  }
	  catch( GDLException& ex)
	  {
	    ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage());
	  }
	}
	DLong typeCode = (*typeCodeKW)[0];
	if( typeCode < GDL_BYTE || typeCode > GDL_ULONG64)
	  ThrowFromInternalUDSub( e, "Illegal value for TYPE: " + i2s(typeCode));      	
	resultType = static_cast<DType>(typeCode);  
      }
    }
    if( resultType == GDL_UNDEF)
	ThrowFromInternalUDSub( e, "Result type is UNDEF. Please report.");      	
      
    try{
	     if( resultType == GDL_BYTE)
	       return LIST__ToArray<DByteGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_INT)
	       return LIST__ToArray<DIntGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_LONG)
	       return LIST__ToArray<DLongGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_FLOAT)
	       return LIST__ToArray<DFloatGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_DOUBLE)
	       return LIST__ToArray<DDoubleGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_COMPLEX)
	       return LIST__ToArray<DComplexGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_STRING)
	       return LIST__ToArray<DStringGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_COMPLEXDBL)
	       return LIST__ToArray<DComplexDblGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_UINT)
	       return LIST__ToArray<DUIntGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_ULONG)
	       return LIST__ToArray<DULongGDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_LONG64)
	       return LIST__ToArray<DLong64GDL>( nList, pTail, missingKW);
	       
	else if( resultType == GDL_ULONG64)
	       return LIST__ToArray<DULong64GDL>( nList, pTail, missingKW);
	       
	else 
	  throw GDLException( "Unknown or unable to convert to type code: " + i2s(resultType));      	
      
    }
    catch( GDLException& ex)
      {
	ThrowFromInternalUDSub( e, ex.ANTLRException::getMessage());
      }
    assert(false);
    return NULL;
  }

BaseGDL* list__isempty( EnvUDT* e)
{
  static int kwSELFIx = 0;
  DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
  DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  if (nList > 0) return new DByteGDL(0); else return new DLongGDL(1);
}

SizeT LIST_count( DStructGDL* list) {// straight through, no checks
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    return (*static_cast<DLongGDL*>( list->GetTag( nListTag, 0)))[0];	      
}

BaseGDL* list__count( EnvUDT* e)
{
	static int kwSELFIx = 0;
	static int kwVALUEIx = 1;
	static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

	SizeT nParam = e->NParam(1); // minimum SELF

	DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);

	if( nParam > 1)
	{
	  BaseGDL* r = e->GetKW( kwVALUEIx);

	  DObjGDL* selfObj = static_cast<DObjGDL*>(e->GetKW( kwSELFIx));
	  
	  DByteGDL* result = static_cast<DByteGDL*>(selfObj->EqOp( r));
	  Guard<DByteGDL> newObjGuard( result);
	  
	  DLong nList = 0;
	  for( SizeT i=0; i<result->N_Elements(); ++i)
	  {
	if( (*result)[i] != 0)
	  ++nList;
	  }
	  return new DLongGDL( nList);
	}

	DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
	return new DLongGDL( nList);
}
  
void list__move( EnvUDT* e)
{
  
	  GDL_LIST_STRUCT()
	  GDL_CONTAINER_NODE()

    SizeT nParam = e->NParam(3); // minimum SELF, SOURCE, DESTINATION
	
    static int kwSELFIx = 0; // no keywords
    DStructGDL* self = GetOBJ( e->GetKW( kwSELFIx), e);
    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
    trace_me = false; // trace_arg();

    DLong index1, index2;
	// (allowing negative index references)
	e->AssureLongScalarPar( 1, index1); if(index1 < 0) index1 += nList;
	e->AssureLongScalarPar( 2, index2); if(index2 < 0) index2 += nList;
	
	if( index1 >= nList or index2 >= nList
		or  index1 < 0 or index2 < 0) 	
		ThrowFromInternalUDSub( e," index out of range - listsize=: "
				+i2s(nList)+" indeces: "+i2s(index1)+","+i2s(index2));
		
    DStructDesc* selfDesc= self->Desc();
    DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;
    assert( selfDesc != NULL && selfDesc->NTags() > 0);
    assert( containerDesc != NULL && containerDesc->NTags() > 0);

    DPtrGDL* Tail = static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0));
    DPtrGDL* Head = static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0));
    DPtr pTail = (*Tail)[0];	DPtr pHead = (*Head)[0];
	if(trace_me or ((index1 ==0) and (index2==0))) 
	{	DPtr p0 = (*Tail)[0];
		DPtr pdata;
		std::printf(" list: TAIL=%llu", p0);
		for( int i=0; i < nList ; i++) 
		{	DStructGDL* Node=GetLISTStruct(NULL, p0);
			p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			pdata = (*static_cast<DPtrGDL*>( Node->GetTag( pDataTag, 0)))[0];
			std::printf("->%llu<%llu>:", p0, pdata);}
		std::printf("->HEAD= %llu (list.move:%d,%d) \n",  pHead, index1, index2);
	}
	if(index1 == index2) return; 		// trivial case

	DPtr ptrg, predptr;
	DPtr p0 = pTail;
	DStructGDL* Node = GetLISTStruct(NULL, p0);
	DStructGDL* predNode = Node;
	if(index2 < index1)
		for( int i=0; i < index1 ; i++) 
		{	if(i == index2-1) ptrg = p0;
			predptr = p0;
			p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			predNode = Node;
			Node = GetLISTStruct(NULL, p0);
		} else
		for( int i=0; i < index1 ; i++)
		{
			predptr = p0;
			p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			predNode = Node;
			Node = GetLISTStruct(NULL, p0);
		}

	DPtr psrc = p0;
	DPtrGDL* ptrnxt = static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0));
	
	if(index1 == 0) 
		(*Tail)[0] = (*ptrnxt)[0];
	else if( index1 == nList-1)
	{
		(*Head)[0] = predptr;
		(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 0;			
	}
	else
		(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 
			(*ptrnxt)[0];
	(*ptrnxt)[0] = 0;
	
	if( index2 == 0)
	{
		(*Tail)[0] = psrc;
		(*ptrnxt)[0] = pTail;
	}
	else if( index2 == nList-1)
	{
		Node = GetLISTStruct(NULL, (*Head)[0]);
		(*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0] = psrc;
		(*Head)[0] = psrc;
	}
	else {
		if( index1 == 0) p0 =(*Tail)[0];
		else if(index2 > index1) p0 = 
			(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
		else p0 = ptrg;

		for(int i = index1; i < index2 - 1 ; i++) {
			Node = GetLISTStruct(NULL, p0);
			p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
		}
		Node = GetLISTStruct(NULL, p0);
		ptrnxt = static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0));

		p0 = (*ptrnxt)[0];
		(*ptrnxt)[0] = psrc;
		Node = GetLISTStruct(NULL, psrc);
		(*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0] = p0;
	}
	
	if(trace_me) 
	{	p0 = (*Tail)[0];
		std::printf(" from list.move   : TAIL=%llu" , p0);
		for( int i=0; i < nList ; i++) 
		{
			DStructGDL* Node=GetLISTStruct(NULL, p0);
			p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			std::printf("->%llu", p0);
		}
		p0 = (*Head)[0];
		std::printf(" : HEAD= %llu \n",  p0);
	}

	return; // victorious
}

void list__swap( EnvUDT* e)
{
  
  GDL_LIST_STRUCT()
  GDL_CONTAINER_NODE()

		trace_me = false; // lib::trace_arg();
	if(trace_me) std::printf(" list__swap ");
     SizeT nParam = e->NParam(3); // minimum SELF, INDEX1, INDEX2
    
    static int kwSELFIx = 0; // no keywords
    DStructGDL* self = GetOBJ( e->GetKW( kwSELFIx), e);
    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
    
    DLong index1, index2;
	e->AssureLongScalarPar( 1, index1);
	e->AssureLongScalarPar( 2,  index2);
	if(index1 < 0 or index1 >= nList)
		ThrowFromInternalUDSub( e," index1 out of range: "+i2s(index1));
	if(index2 < 0 or index2 >= nList)
		ThrowFromInternalUDSub( e," index2 out of range: "+i2s(index2));

	if(index1 < index2) {
		DLong swap = index1; index1 = index2; index2 = swap;
	} else if(index1 == index2) return;

// define the standard LIST struct = listDesc:
   DStructDesc* listDesc = structDesc::LIST;
    DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;

    DPtrGDL* Tail = static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0));
    DPtrGDL* Head = static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0));
	DStructGDL* Node;
	DStructGDL* predNsrc = Node;
	DStructGDL* Ntrg;
	DStructGDL* predNtrg;
	DPtr pNext, ptrg, psrc;
//--- .     ....    index2 ,,,,,               ... index1
//     tail[0]>      ptrg  ..... predsrc->pNext .. psrc
//  predtrg->pNext	                                      
	DPtr p0 = (*Tail)[0];
	Node = GetLISTStruct(NULL, p0);
	for( int i=0; i < index1 ; i++) 
	{   if(i == index2) { ptrg = p0; predNtrg=predNsrc; Ntrg=Node;}
		p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
		predNsrc = Node;
		Node = GetLISTStruct(NULL, p0);
	}
	psrc=p0;
	DStructGDL* Nsrc = Node;
	if(trace_me) {
		std::printf(" ptrg %llu -> %llu , psrc %llu-> %llu ",
			 ptrg,(*static_cast<DPtrGDL*>( Ntrg->GetTag( pNextTag, 0)))[0],
			 psrc,(*static_cast<DPtrGDL*>( Nsrc->GetTag( pNextTag, 0)))[0]);
		std::printf(" : Tail=%llu Head=%llu list.swap:%d,%d \n",
					(*Tail)[0], (*Head)[0], index1, index2);
	}

// swap contents of pNextTag
	DPtr swap = (*static_cast<DPtrGDL*>( Ntrg->GetTag( pNextTag, 0)))[0];
	if(swap == psrc) {
		swap = ptrg; // correction for swapping adjacent nodes.
		predNsrc = Nsrc;
	}

	(*static_cast<DPtrGDL*>( Ntrg->GetTag( pNextTag, 0)))[0] =
		(*static_cast<DPtrGDL*>( Nsrc->GetTag( pNextTag, 0)))[0];

	(*static_cast<DPtrGDL*>( Nsrc->GetTag( pNextTag, 0)))[0] = swap;
	
	if( index1 == nList-1)
	{
		(*Head)[0] = ptrg;
		(*static_cast<DPtrGDL*>( Ntrg->GetTag( pNextTag, 0)))[0] = 0;			
	}
	
	(*static_cast<DPtrGDL*>( predNsrc->GetTag( pNextTag, 0)))[0] = ptrg;
		
	if(index2 == 0)
		(*Tail)[0] = psrc;
	 else
	 {
		(*static_cast<DPtrGDL*>( predNtrg->GetTag( pNextTag, 0)))[0] = psrc;
		}
//--- .     ....    index2 ,,,,,               ... index1
//     tail[0]>      psrc  ..... predsrc->pNext .. ptrg
	return;
}

  BaseGDL* list__where( EnvUDT* e)
  {
    const int kwNCOMPLEMENTIx = 0; 
    const int kwCOUNTIx = 1; 
    const int kwCOMPLEMENTIx = 2; 
    const int kwSELFIx = 3;
    const int kwVALUEIx = 4;

    SizeT nParam = e->NParam(2); // SELF, VALUE

    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e); // checks

    BaseGDL* r = e->GetKW( kwVALUEIx);
  
    DObjGDL* selfObj = static_cast<DObjGDL*>(e->GetKW( kwSELFIx));

    BaseGDL* result = selfObj->EqOp( r);
    Guard<BaseGDL> resultGuard( result);

    DLong nEl = result->N_Elements();
    
    SizeT count;
    DLong* ixList = result->Where( e->KeywordPresent( kwCOMPLEMENTIx), count);
    
    if( e->KeywordPresent( kwNCOMPLEMENTIx)) // NCOMPLEMENT
    {
	e->SetKW( kwNCOMPLEMENTIx, new DLongGDL( nEl - count));
    }
    if( e->KeywordPresent( kwCOUNTIx)) // COUNT
    {
	e->SetKW( kwCOUNTIx, new DLongGDL( count));
    }
    if( e->KeywordPresent( kwCOMPLEMENTIx)) // COMPLEMENT
    {
        SizeT nCount = nEl - count;
	if( nCount == 0)
	{
	  e->SetKW( kwCOMPLEMENTIx, NullGDL::GetSingleInstance());
	}
	else
	{
	  DLongGDL* cIxList = new DLongGDL( dimension( &nCount, 1), BaseGDL::NOZERO);	  
	  SizeT cIx = nEl;
	  for( SizeT i=0; i<nCount; ++i)
	      (*cIxList)[ i] = ixList[ --cIx];
	  e->SetKW( kwCOMPLEMENTIx, cIxList);
	}
    }
    
    if( count == 0)
      return NullGDL::GetSingleInstance();
    else
      return new DLongGDL( ixList, count);
  }
  
  
  BaseGDL* list__remove( EnvUDT* e, bool asFunction);

  BaseGDL* list__remove_fun( EnvUDT* e)
  {
    return list__remove( e, true);
  }
  void list__remove_pro( EnvUDT* e)
  {
    list__remove( e, false);
  }

  BaseGDL* list__remove( EnvUDT* e, bool asFunction)
  {
  // see overload.cpp
  static int kwALLIx = 0; 
  static int kwSELFIx = 1;
  static int kwINDEXIx = 2;

  bool kwALL = false;
  if (e->KeywordSet(kwALLIx)){ kwALL = true;}

  SizeT nParam = e->NParam(1); // minimum SELF
      
  DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);
  DStructDesc* listDesc= self->Desc();
  
//   DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;

  // here static is fine
  static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
  static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
  static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
  static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

  if( kwALL)
  {
    if( asFunction)
    {
      DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
      DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct, inits ref count
      BaseGDL* newObj = new DObjGDL( objID); // the list object
      Guard<BaseGDL> newObjGuard( newObj);
      
      (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] =
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;
      
      (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] =
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;
      
      (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] =
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;
      
      newObjGuard.Release();
      return newObj;    
    }
    else
    {
      LISTCleanup( e, self);
//       DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];
// 
//       // trigger ref-count delete of all elements      
         // problem here due to recursion on large LISTs
//       BaseGDL::interpreter->FreeHeap( pTail); 
//       
//       (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;	      
//       (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;	      
//       (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;
      return NULL;      
    }
  }

  DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
  if( nList == 0)
    ThrowFromInternalUDSub( e, "LIST is empty.");

  BaseGDL* index = NULL;
  DLongGDL* indexLong = NULL;
  Guard<BaseGDL> indexLongGuard;
  if( nParam >= 2)
    index = e->GetKW(kwINDEXIx);
  if( index != NULL)
  {
    if( index->Type() != GDL_LONG)
    {
      indexLong = static_cast<DLongGDL*>(index->Convert2(GDL_LONG,BaseGDL::COPY));
      indexLongGuard.Init( indexLong);
    }
    else
      indexLong = static_cast<DLongGDL*>(index);
  }
  
  DLong removePos = -1;
  if( indexLong != NULL)
  {
    if( indexLong->N_Elements() == 1)
    {
	removePos = (*indexLong)[0];
	if( removePos < 0)
	  removePos += nList;
	if( removePos < 0)
	  ThrowFromInternalUDSub( e, "Index too small.");
	if( removePos >= nList)
	  ThrowFromInternalUDSub( e, "Index out of range.");  
    }
  }
  
  if( indexLong == NULL || removePos == nList-1) // remove head
  {

    DPtr pHead = (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];	      
    
    DStructGDL* headNode = GetLISTStruct(e, pHead);  
    
    DPtr pData = (*static_cast<DPtrGDL*>( headNode->GetTag( pDataTag, 0)))[0];   
    
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
    
    if( nList == 1)
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;    
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;
    }
    else if( nList == 2)
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 1;
    }
    else // nList > 2
    {
      DPtr pPredHead = GetLISTNode( e, self, nList-2);
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = pPredHead;    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;
    }
//     e->Interpreter()->HeapErase( pData); // no delete
//     e->Interpreter()->FreeHeap( pHead); // delete
    FreeLISTNode( e, pHead, !asFunction);
    
    if( data == NULL)
      return NullGDL::GetSingleInstance();
    return data;
  }
  if( removePos == 0) // remove tail
  {
    // implicit: nList > 1
    DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];	      
    
    DStructGDL* tailNode = GetLISTStruct(e, pTail);  
    
    DPtr pData = (*static_cast<DPtrGDL*>( tailNode->GetTag( pDataTag, 0)))[0];   
    
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
    
    if( nList == 2)
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 1;
    }
    else // nList > 2
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 
      (*static_cast<DPtrGDL*>( tailNode->GetTag( pNextTag, 0)))[0];        
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;
    }
//     e->Interpreter()->HeapErase( pData); // no delete
//     e->Interpreter()->FreeHeap( pTail); // delete
    FreeLISTNode( e, pTail, !asFunction);
    
    if( data == NULL)
      return NullGDL::GetSingleInstance();
    return data;    
  }
  if( removePos != -1) // single element
  {
    // implicit: nList > 2
    DPtr pPredNode = GetLISTNode( e, self, removePos-1);
    DStructGDL* predNode = GetLISTStruct( e, pPredNode);   

    DPtr pRemoveNode = (*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
    DStructGDL* removeNode = GetLISTStruct( e, pRemoveNode);   

    DPtr pData = (*static_cast<DPtrGDL*>( removeNode->GetTag( pDataTag, 0)))[0];   
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);

    (*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0];
    
    (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;

//     e->Interpreter()->HeapErase( pData); // no delete
//     e->Interpreter()->FreeHeap( pRemoveNode); // no delete
    FreeLISTNode( e, pRemoveNode, !asFunction);
    
    if( data == NULL)
      return NullGDL::GetSingleInstance();
    return data;    
  }
  
  // remove all indexed elements
  // 1st build return LIST
  BaseGDL* newObj = NULL; // the list object
  Guard<BaseGDL> newObjGuard;
  SizeT indexN_Elements = indexLong->N_Elements();
  if( asFunction)
  {
    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    newObj = new DObjGDL( objID); // the list object
    newObjGuard.Init( newObj);
    // we need ref counting here as the LIST (newObj) is a regular return value
  //   e->Interpreter()->IncRefObj( objID);
    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    for( SizeT i=0; i<indexN_Elements; ++i)
    {
      DLong actIx = (*indexLong)[ i];
      if( actIx < 0)
	actIx += nList;
      if( actIx < 0)
	ThrowFromInternalUDSub( e, "Index too small.");
      if( actIx >= nList)
	ThrowFromInternalUDSub( e, "Index out of range.");
	
      
      DPtr pActNode = GetLISTNode( e, self, actIx);
      DStructGDL* actNode = GetLISTStruct( e, pActNode);   

      DPtr pData = (*static_cast<DPtrGDL*>(actNode->GetTag( pDataTag, 0)))[0];
      BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
      if( data != NULL) 
	data = data->Dup();
      DPtr dID = e->Interpreter()->NewHeap(1,data);
      
      cStruct = new DStructGDL( structDesc::GDL_CONTAINER_NODE, dimension());
      cID = e->Interpreter()->NewHeap(1,cStruct);
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = dID;
      
      if( cStructLast != NULL)
	(*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
      else
      { // 1st element
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
      }
	    
      cStructLast = cStruct;
    }
    
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = indexN_Elements;      
  } // if( asFunction)
  
  // 2nd: remove the indexed elements
  if( indexLongGuard.Get() == NULL)
  {
    // we need to sort the index
    indexLong = indexLong->Dup();
    indexLongGuard.Init(indexLong);
  }
  DLong *hh = static_cast<DLong*>(indexLong->DataAddr());
  DLong* h1 = new DLong[ indexN_Elements/2];
  DLong* h2 = new DLong[ (indexN_Elements+1)/2];
  // call the sort routine
  MergeSortDescending<DLong>( hh, h1, h2, indexN_Elements);
  delete[] h1;
  delete[] h2;

  SizeT nListStart = nList;
  for( DLong i=0; i < indexN_Elements; ++i)
  {
    DLong removeIndex = hh[ i];
//     std::cout << " Removing index: " << i2s(removeIndex) << std::endl;
    
    if( removeIndex < 0)
      removeIndex += nListStart;
    if( removeIndex < 0)
      ThrowFromInternalUDSub( e, "Index too small.");
    if( removeIndex >= nList)
      ThrowFromInternalUDSub( e, "Index out of range.");
    
    if( removeIndex == nList-1) // remove head
    {
//     std::cout << " Removing index: nList-1" << std::endl;

      DPtr pHead = (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];	      
      
      DStructGDL* headNode = GetLISTStruct(e, pHead);  
      
      DPtr pData = (*static_cast<DPtrGDL*>( headNode->GetTag( pDataTag, 0)))[0];   
      
//       BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
      
      if( nList == 1)
      {
	(*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;    
	(*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;    
	(*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;
      }
      else if( nList == 2)
      {
	(*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];    
	(*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 1;
      }
      else // nList > 2
      {
	DPtr pPredHead = GetLISTNode( e, self, nList-2);
	(*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = pPredHead;    
	(*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;
      }
      
      // prevent (ref-count) cleanup of next node 
      (*static_cast<DPtrGDL*>( headNode->GetTag( pNextTag, 0)))[0] = 0;      
//       e->Interpreter()->FreeHeap( pData);
//       e->Interpreter()->FreeHeap( pHead);
      FreeLISTNode( e, pHead, true);
    }
    else if( removeIndex == 0) // remove tail
    {
//     std::cout << " Removing index: zero" << std::endl;
      // implicit: nList > 1
      DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];	      
      
      DStructGDL* tailNode = GetLISTStruct(e, pTail);  
      
      DPtr pData = (*static_cast<DPtrGDL*>( tailNode->GetTag( pDataTag, 0)))[0];   
      
//       BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
      
      if( nList == 2)
      {
	(*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];    
	(*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 1;
      }
      else // nList > 2
      {
	(*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( tailNode->GetTag( pNextTag, 0)))[0];        
	(*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;
      }

      // prevent (ref-count) cleanup of next node 
      (*static_cast<DPtrGDL*>( tailNode->GetTag( pNextTag, 0)))[0] = 0;      
//       e->Interpreter()->FreeHeap( pData);
//       e->Interpreter()->FreeHeap( pTail);
      FreeLISTNode( e, pTail, true);
    }
    else
    {
//     std::cout << "  Removing index: " << i2s(removeIndex) << std::endl;
      // implicit: nList > 2
      DPtr pPredNode = GetLISTNode( e, self, removeIndex-1);
      DStructGDL* predNode = GetLISTStruct( e, pPredNode);   

      DPtr pRemoveNode = (*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
      DStructGDL* removeNode = GetLISTStruct( e, pRemoveNode);   

      DPtr pData = (*static_cast<DPtrGDL*>( removeNode->GetTag( pDataTag, 0)))[0];   
//       BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);

      (*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 
	  (*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0];

//     std::cout << "    Next index: " << i2s((*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0]) << std::endl;

//     std::cout << "    Freeing index: " << i2s(pRemoveNode) << std::endl;

      // prevent (ref-count) cleanup of next node 
      (*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0] = 0;
//       e->Interpreter()->FreeHeap( pData);
//       e->Interpreter()->FreeHeap( pRemoveNode);
      FreeLISTNode( e, pRemoveNode, true);
    }
    assert( nList >= 1);
    // keep LIST consistent
    (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = --nList;
  }
  
  newObjGuard.Release();
  return newObj;
  }

  
  
  void list__reverse( EnvUDT* e)
  {
    // no args no kwords (but SELF parameter)
    SizeT nParam = e->NParam(1); // SELF

    DStructGDL* self = GetSELF( e->GetKW( 0), e);
  
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    
    DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      

    if( nList <= 1) // no change for empty or one-element
      return;
    
    DPtr actPrevP = 0;
    DPtr actP = (*static_cast<DPtrGDL*>(self->GetTag( pTailTag, 0)))[0];
    for( SizeT elIx = 0; elIx < nList; ++elIx)
      {
	DStructGDL* actPStruct = GetLISTStruct(e, actP);

	DPtr actPNext = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];

	(*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0] = actPrevP;
	
	actPrevP = actP;
	
	actP = actPNext;
      }

    // swap head and tail pointer
    DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];
    (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] =  
    (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];	      
    (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = pTail;	      
  }


 BaseGDL* list__init( EnvUDT* e) {

//	if( trace_me) std::cout << " List Init!" << std::endl;
	return new DByteGDL(1); // if we reach here, defaul is to return 'TRUE'
 }
  
 BaseGDL* list__get( EnvUDT* e)
  {
  // see overload.cpp
  //     DFunLIST__ADD->AddKey("ALL","ALL")->AddKey("ISA","ISA")->AddKey("NULL","NULL");
  //     DFunLIST__ADD->AddPar("VALUE")->AddPar("INDEX");
  // res=List.get([/all] [, isa=(names)] [. position=index] [, count=variable] [/null][)
  
  GDL_LIST_STRUCT()
  GDL_CONTAINER_NODE()
  
  SizeT nParam = e->NParam(1);

		trace_me = false;//trace_arg();
  if(trace_me) std::printf(" list__get -nprm= %d ", nParam);
  
	static int kwALLIx = e->GetKeywordIx("ALL");
	static int kwISAIx = e->GetKeywordIx("ISA");
	static int kwNULLIx = e->GetKeywordIx("NULL");
	static int kwPOSITIONIx = e->GetKeywordIx("POSITION");
	static int kwCOUNTIx = e->GetKeywordIx("COUNT");
	static int kwSELFIx = kwALLIx + 1;
	DStructGDL* self = GetOBJ( e->GetKW( kwSELFIx), e);

  DStructDesc* selfDesc= self->Desc();
  bool listmode = ( selfDesc == structDesc::LIST);

	DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
	bool nullKW = e->KeywordSet(kwALLIx);
	if( nList == 0)
	{	if(nullKW) return NullGDL::GetSingleInstance();
		else return new DLongGDL(-1);
	}
	
	DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];
	DStructGDL* Node = GetLISTStruct(e, pTail);
	DPtr pNext = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
	BaseGDL* NodePtr = Node->GetTag( pDataTag, 0);
	DPtr pointer = (*static_cast<DPtrGDL*>( NodePtr))[0];
// IDL_CONTAINER began as an object container and was later extended to
// include Heap variable pointers.  Both cases are handled with the same
// GDL_CONTAINER link-list, as for LIST. pDataTag is always just a pointer,
// when an object is contained it points to an object's pointer.
// When ::GET is to be used on a real LIST then pData will point to
// other data.
// If however ::GET is used on a GDL_CONTAINER then pData
// will be just pData if the container is to be holding only heapvars,
// 
// an IDL_CONTAINER is supposed to be only of one type or another:
// either it is holding objects or it is holding heapvar pointers.
// However 
	if( ! BaseGDL::interpreter->PtrValid(pointer))
		ThrowFromInternalUDSub( e,  " leading heap pointer is not valid");
	
	BaseGDL* theref = BaseGDL::interpreter->GetHeap( pointer );

	bool isPtr =  (theref->Type() != GDL_OBJ);
	bool allKW = e->KeywordSet(kwALLIx);

    BaseGDL* isaKW = NULL;
    isaKW = e->GetKW( kwISAIx);
    if( isaKW != NULL && allKW) isPtr = false;  // /all,isa='kkk' is object
    BaseGDL** countKW = NULL;
    if( e->GetKW( kwCOUNTIx) != NULL) countKW = &e->GetKW( kwCOUNTIx);

	BaseGDL* index = e->GetKW(kwPOSITIONIx);
 
	MAKE_LONGGDL(index, indexLong)

	if( indexLong == NULL) indexLong = new DLongGDL(0);

 if(trace_me) std::cout<< " isPtr? "<< isPtr;
 if(trace_me) std::printf(" Tail: %llu \n", pointer);
 

	std::vector<DPtr> pointers;
  	if(allKW)			// /ALL case here significantly easier than indexes
	{
		std::vector<DString> testisa;
		if( isaKW != NULL) 
			{
			if( isaKW->Type() != GDL_STRING)
					 ThrowFromInternalUDSub( e,
					  "Object Classes can be referenced only with names (strings)");
			for(SizeT i=0; i < isaKW->N_Elements(); i++)
				testisa.push_back(StrUpCase( (*static_cast<DStringGDL*>( isaKW))[i]));
			}
		int inlist = 0;
		do {
			if( e->Interpreter()->PtrValid( pointer) ) {
				if( isPtr) {
					pointers.push_back( pointer);
				 if(trace_me) std::printf(" ++: %d %llu",inlist,  pointer);	
				}
				else
				{
					theref = BaseGDL::interpreter->GetHeap( pointer );
					bool accept = true;
					DObj ID = (*static_cast<DObjGDL*>( theref))[0];
					if( e->Interpreter()->ObjValid( ID) ) {
						if( isaKW != NULL)
						{
							accept = false;
							DStructGDL* oStruct = e->GetObjHeap( ID);
							for(SizeT i =0; i < testisa.size(); i++)
									if( oStruct->Desc()->IsParent( testisa[i]))
									  { accept = true; break;}
						}
						if(accept) pointers.push_back( ID);
					}
				}
			} else  if(trace_me) std::printf(" XX: %llu",  pointer);
			if(trace_me) std::printf(" pNext:%llu",  pNext);
			if( pNext != 0) {
				Node = GetLISTStruct(e, pNext);
				NodePtr = Node->GetTag( pDataTag, 0);   
				pointer = (*static_cast<DPtrGDL*>( NodePtr))[0];
				pNext = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			}
		} while ( ++inlist != nList );
		if(pointers.size() == 0)
		{	if(nullKW) return NullGDL::GetSingleInstance();
			else return new DLongGDL(-1);
		}
		if(trace_me) {
			 std::cout << " <DPtr>pointers: ";
			 for( SizeT k=0; k < pointers.size(); k++) 
					std::printf("%llu :",  pointers[k]);
			 std::cout << std::endl;
		 }
	}  else 
	{
    
		if( index == NULL) {
			if(countKW != NULL) *countKW = new DLongGDL(1);
				 if(trace_me) std::printf(" 1-shot %llu\n" ,  pointer);	
			if(isPtr)	return new DPtrGDL( pointer);
				else 	return new DObjGDL( (*static_cast<DObjGDL*>( theref))[0]);
			}

		int inlist = 0;
		SizeT nEl = indexLong->N_Elements();
		do {
			if( e->Interpreter()->PtrValid( pointer) ) {
				if( isPtr) {
				  for( SizeT i=0; i < nEl; i++)
					if(inlist == (*indexLong)[i]) 
					{
						if(trace_me) std::printf(" >>: %d %llu", inlist, pointer);
						 pointers.push_back( pointer);
					}
				} else {
					theref = BaseGDL::interpreter->GetHeap( pointer );
					DObj ID = (*static_cast<DObjGDL*>( theref))[0];
					if( e->Interpreter()->ObjValid( ID) )
					{
					  for( SizeT i=0; i < nEl; i++)
						if(inlist == (*indexLong)[i]) 
						{
					if(trace_me) std::printf("-> %d %llu", inlist, ID);
							 pointers.push_back( ID);
						}
					}
				}
			} else  if(trace_me) std::printf(" XX: %llu",  pointer);
			if( pNext != 0) {
				Node = GetLISTStruct(e, pNext);
				NodePtr = Node->GetTag( pDataTag, 0);   
				pointer = (*static_cast<DPtrGDL*>( NodePtr))[0];
				pNext = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			}
			if(trace_me) std::printf(" pNext:%llu",  pNext);
		} while ( ++inlist != nList );
		if(trace_me) std::cout << std::endl;

	}
	SizeT nfetch = pointers.size();
	if(countKW != NULL) *countKW = new DLongGDL(nfetch);
    if( isPtr)	
    {
		DPtrGDL* ret;
		if( allKW) ret = new DPtrGDL( dimension(nfetch));
		else 		ret = new DPtrGDL( dimension(indexLong->Dim()));
		Guard<DPtrGDL> retGuard( ret);
		for(SizeT i=0; i < nfetch; i++) {
 			if(trace_me) std::printf(": %d %llu",i, pointers[i]);
				e->Interpreter()->IncRef(pointers[i]);
					(*ret)[i] = pointers[i];
				}
		if(trace_me) std::cout << std::endl;
		if(trace_me) {
			 std::cout << " <DPtr>pointers: ";
			 for( SizeT k=0; k < pointers.size(); k++) 
					std::printf("%llu :",  pointers[k]);
			 std::cout << std::endl;
		 }
		retGuard.Release();
		return ret;
	}
	else 
	{
		DObjGDL* ret;
		if( allKW) ret = new DObjGDL( dimension(nfetch));
		else 		ret = new DObjGDL( dimension(indexLong->Dim()));
		Guard<DObjGDL> retGuard( ret);
		for(SizeT i=0; i < nfetch; i++) {
				e->Interpreter()->IncRefObj(pointers[i]);
					(*ret)[i] = pointers[i];
				}
		retGuard.Release();
		return ret;
	}
}
  void list__add( EnvUDT* e)
  {
  // see overload.cpp
  //     DFunLIST__ADD->AddKey("EXTRACT","EXTRACT")->AddKey("NO_COPY","NO_COPY");
  //     DFunLIST__ADD->AddPar("VALUE")->AddPar("INDEX");

  static int kwNO_COPYIx = 0; // pushed front 2nd
  static int kwEXTRACTIx = 1; // pushed front 1st
  static int kwSELFIx = 2;
  static int kwVALUEIx = 3;
  static int kwINDEXIx = 4;

  bool kwEXTRACT = false;
  bool kwNO_COPY = false;
  if (e->KeywordSet(kwEXTRACTIx)){ kwEXTRACT = true;}
  if (e->KeywordSet(kwNO_COPYIx)){ kwNO_COPY = true;}

  SizeT nParam = e->NParam(1); // minimum SELF
  
  DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);
  
  static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
  static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
  static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
  static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

  // because of .RESET_SESSION, we cannot use static here
  DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;

  DStructGDL* listStruct= self;

  BaseGDL* value = NULL;
  if( nParam >= 2)
    value = e->GetKW(kwVALUEIx);

  DLong nList = (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0];	      

  BaseGDL* index = NULL;
  DLongGDL* indexLong = NULL;
  Guard<BaseGDL> indexLongGuard;
  DLong insertPos = -1;
  if( nParam >= 3)
    index = e->GetKW(kwINDEXIx);
  if( index != NULL)
  {
    if( index->Type() != GDL_LONG)
    {
      indexLong = static_cast<DLongGDL*>(index->Convert2(GDL_LONG,BaseGDL::COPY));
      indexLongGuard.Init( indexLong);
    }
    else
      indexLong = static_cast<DLongGDL*>(index);
    insertPos = (*indexLong)[0];
    if( insertPos < 0)
      ThrowFromInternalUDSub( e, "INDEX out of range ("+i2s(insertPos)+" (<0))");
    if( insertPos > nList)
      ThrowFromInternalUDSub( e, "INDEX out of range ("+i2s(insertPos)+" (>"+i2s(nList)+"))");	
  }

    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    DStructGDL* cStructLast = NULL;
    if( kwEXTRACT && value != NULL && value->N_Elements() > 1)
    {
      DPtr firstID = 0;
      DStructGDL* cStructLast = NULL;
      SizeT valueN_Elements = value->N_Elements();
      for( SizeT eIx=0; eIx<valueN_Elements; ++eIx)
      {
	DPtr pID;

	pID = e->Interpreter()->NewHeap(1,value->NewIx(eIx));
	
	cStruct= new DStructGDL( containerDesc, dimension());

	(*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = pID;
	
	cID = e->Interpreter()->NewHeap(1,cStruct);

	if( cStructLast != NULL)
	  (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
	else
	{ // 1st element
	  firstID = cID;	      
	}
	
	cStructLast = cStruct;
      }
      if( kwNO_COPY)
      {
	bool stolen = e->StealLocalKW( kwVALUEIx);
	if( !stolen) e->GetKW(kwVALUEIx) = NULL;
	GDLDelete(value);
      }

      if( nList == 0) // empty LIST
      {
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = firstID;
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      	
      }
      else if( insertPos == -1 || insertPos == nList) // head
      {
	DPtr pHead = (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0];	      
	DStructGDL* headNode = GetLISTStruct( e, pHead);  
	
	(*static_cast<DPtrGDL*>( headNode->GetTag( pNextTag, 0)))[0] = firstID;
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
      }
      else if( insertPos == 0) // tail
      {
	DPtr pTail = (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0];	      
	
	(*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = pTail;
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = firstID;	      
      }
      else
      {
	DPtr pPredNode = GetLISTNode( e, self, insertPos-1);
	DStructGDL* predNode = GetLISTStruct( e, pPredNode);   

	(*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
	(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = firstID;
      }
      
      (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nList+valueN_Elements;
    }
  else
    {
      SizeT pID;
      if( value == NULL || kwNO_COPY)
      {
	pID = e->Interpreter()->NewHeap(1,value);
	bool stolen = e->StealLocalKW( kwVALUEIx);
	if( !stolen) e->GetKW(kwVALUEIx) = NULL;
      }
      else
      {
	pID = e->Interpreter()->NewHeap(1,value->Dup());
      }
      // pID properly set (ptr to data)
      cStruct= new DStructGDL( containerDesc, dimension());
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = pID;
      cID = e->Interpreter()->NewHeap(1,cStruct);
      
      if( nList == 0) // empty LIST
      {
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      	
      }
      else if( insertPos == -1 || insertPos == nList) // head
      {
	DPtr pHead = (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0];	      
	DStructGDL* headNode = GetLISTStruct( e, pHead);  
	
	(*static_cast<DPtrGDL*>( headNode->GetTag( pNextTag, 0)))[0] = cID;
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
      }
      else if( insertPos == 0) // tail
      {
	DPtr pTail = (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0];	      
	
	(*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = pTail;
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
      }
      else
      {
	DPtr pPredNode = GetLISTNode( e, self, insertPos-1);
	DStructGDL* predNode = GetLISTStruct( e, pPredNode);   

	(*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
	(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = cID;
      }
      (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nList+1;	      
    }
  }
  
  
  
  BaseGDL* list_fun( EnvT* e)
  {
    static int kwEXTRACTIx = e->KeywordIx("EXTRACT");
    static int kwLENGTHIx = e->KeywordIx("LENGTH");
    static int kwNO_COPYIx = e->KeywordIx("NO_COPY");

    bool kwEXTRACT = false;
    bool kwNO_COPY = false;
    if (e->KeywordSet(kwEXTRACTIx)){ kwEXTRACT = true;}
    if (e->KeywordSet(kwNO_COPYIx)){ kwNO_COPY = true;}
    
    SizeT nParam = e->NParam();

    DLong listLength = 0;
    DLongGDL* lengthKW = e->IfDefGetKWAs<DLongGDL>(kwLENGTHIx);
    if( lengthKW != NULL)
    {
      listLength = (*lengthKW)[0];
      if( listLength < 0)
	listLength = 0;
    }
    
    DInterpreter* ip = e->Interpreter();
    
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
      
    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* listDesc=structDesc::LIST;
    DStructDesc* containerDesc=structDesc::GDL_CONTAINER_NODE;
    assert( listDesc != NULL && listDesc->NTags() > 0);
    assert( containerDesc != NULL && containerDesc->NTags() > 0);

    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());

    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct, sets ref count to 1 

    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);
    
    SizeT added = 0;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
    if( nParam > 0 || listLength > 0)
    {
      DStructGDL* cStructLast = NULL;
      for( SizeT pIx=0; pIx<nParam; ++pIx)
      {
	BaseGDL* p = e->GetPar(pIx);
	
	if( kwEXTRACT && p != NULL && p->N_Elements() > 1)
	{
	  for( SizeT eIx=0; eIx<p->N_Elements(); ++eIx)
	  {
	    DPtr pID;

//	    pID = ip->NewHeap(1,p->NewIx(eIx)); // sets ref count to 1
		if(p->Type() != GDL_PTR or (p->StrictScalar()) ){
			pID = ip->NewHeap(1,p->NewIx(eIx));
		} else { 				// when a ptrarr is added & extracted, make ptrarr(1)
			DPtrGDL* pHeap = new DPtrGDL( dimension(1));
			(*pHeap)[0] = (*static_cast<DPtrGDL*>(p))[eIx];
			pID = ip->NewHeap(1, // scalar PTRs are treated different in [, ].
					pHeap);
		}
	    
	    cStruct= new DStructGDL( containerDesc, dimension());
    
	    (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = pID;
	    
	    cID = ip->NewHeap(1,cStruct); // sets ref count to 1

	    if( cStructLast != NULL)
	      (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
	    else
	    { // 1st element
	      (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
	    }
	    
	    cStructLast = cStruct;

	    if( ++added == listLength)
	      break;	    
	  }
	  if( kwNO_COPY)
	  {
	    bool stolen = e->StealLocalPar( pIx);
	    if( !stolen) e->GetPar(pIx) = NULL;
	    GDLDelete(p);
	  }
	  assert( added > 0);
	  if( added == listLength)
	    break;	    
	}
	else
	{
	  SizeT pID;

	  if( p == NULL || kwNO_COPY)
	  {
	    pID = ip->NewHeap(1,p); // sets ref count
	    bool stolen = e->StealLocalPar( pIx);
	    if( !stolen) e->GetPar(pIx) = NULL;
	  }
	  else
	  {
	    pID = ip->NewHeap(1,p->Dup());
	  }
  
	  cStruct= new DStructGDL( containerDesc, dimension());
  
	  (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = pID;
	  
	  cID = ip->NewHeap(1,cStruct); // sets ref count

	  if( cStructLast != NULL)
	    (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
	  else
	  { // 1st element
	    (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
	  }
	  
	  cStructLast = cStruct;

	  if( ++added == listLength)
	    break;	    
	}
      }
      if( listLength != 0 && added < listLength)
      {
	for( ; added<listLength; ++added)
	{
	  DPtr pID;

	  pID = ip->NewHeap(1,NULL);
	  
	  cStruct= new DStructGDL( containerDesc, dimension());
  
	  (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = pID;
	  
	  cID = ip->NewHeap(1,cStruct);

	  if( cStructLast != NULL)
	    (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
	  else
	  { // 1st element
	    (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
	  }
	  
	  cStructLast = cStruct;
	}
      }
    }

//     if( cStruct != NULL)
//       (*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = 0;
	    
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = added;	      

    newObjGuard.Release();
    return newObj;
  }
  
  BaseGDL* container__iscontained( EnvUDT* e)
  {
    GDL_CONTAINER_STRUCT()
    GDL_CONTAINER_NODE()
	static int kwPOSITIONIx = e->GetKeywordIx("POSITION");
    static int kwSELFIx = kwPOSITIONIx + 1; // no keywords
    static int kwVALUEIx = kwSELFIx + 1;
// Keyword
    DStructGDL* self = GetOBJ( e->GetKW( kwSELFIx), e);
	DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];  
    if( nList == 0)
      return NullGDL::GetSingleInstance();
	SizeT nParam = e->NParam(1);
	BaseGDL* values = e->GetKW( kwVALUEIx);
	if( nParam == 0 or values== 0) return NullGDL::GetSingleInstance();
	DInt GDLContainerVersion = 
  		  (*static_cast<DIntGDL*>( self->GetTag( GDLContainerVersionTag, 0)))[0];
	if(	GDLContainerVersion == 0)
			ThrowFromInternalUDSub( e,
			 " only pointers or objects shall be placed in such containers");
	if( GDLContainerVersion == 1 and values->Type() != GDL_PTR)
			ThrowFromInternalUDSub( e,
			 " only pointers can be found in this container");
	if( GDLContainerVersion == 2 and values->Type() != GDL_OBJ)
			ThrowFromInternalUDSub( e,
			 " only objects can be found in this container");
	
    DPtr actP = (*static_cast<DPtrGDL*>(self->GetTag( pTailTag, 0)))[0];
	if( ! BaseGDL::interpreter->PtrValid(actP)) // 
				return NullGDL::GetSingleInstance();
    DLongGDL* pos = new DLongGDL( dimension(values->N_Elements()) );
    Guard<DLongGDL> posGuard( pos);
	
    DByteGDL* result = new DByteGDL( dimension(nList));
    Guard<DByteGDL> resultGuard( result);
    
    for( SizeT elIx = 0; elIx < nList; ++elIx)
      {
		DStructGDL* actPStruct = GetLISTStruct(e, actP);

		DPtr Ptr = (*static_cast<DPtrGDL*>(actPStruct->GetTag( pDataTag, 0)))[0];
		for( SizeT k=0; k < values->N_Elements(); k++ ) 
			if( Ptr == (*static_cast<DPtrGDL*>(values))[k] ) {
				(*result)[k] = 1;
				(*pos)[k] = elIx;
				break;
			}
		actP = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
      }
	for( SizeT k=0; k < values->N_Elements(); k++ ) 
			if( (*result)[k] == 0 ) (*pos)[k] = -1;
    if( e->KeywordPresent( kwPOSITIONIx)) {
		BaseGDL** posKW = &e->GetKW( kwPOSITIONIx);
		posGuard.Release();
		*posKW = pos;
		}
    resultGuard.Release();
    return result;
}
  BaseGDL* container__equals( EnvUDT* e)
  {
    GDL_CONTAINER_STRUCT()
    GDL_CONTAINER_NODE()
    static int kwSELFIx = 0; // no keywords
    static int kwVALUEIx = 1;
// no Keywords
    DStructGDL* self = GetOBJ( e->GetKW( kwSELFIx), e);
	DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];  
    if( nList == 0)
      return NullGDL::GetSingleInstance();
	SizeT nParam = e->NParam(1);
	BaseGDL* value = e->GetKW( kwVALUEIx);
	if( nParam == 0 or value== 0) return NullGDL::GetSingleInstance();
    DByteGDL* result = new DByteGDL( dimension(nList));
    Guard<DByteGDL> resultGuard( result);
	DInt GDLContainerVersion = 
  		  (*static_cast<DIntGDL*>( self->GetTag( GDLContainerVersionTag, 0)))[0];
	if(	GDLContainerVersion != 1) // should be throwing exception here
			ThrowFromInternalUDSub( e, " only containers of pointers are allowed");
    DPtr actP = (*static_cast<DPtrGDL*>(self->GetTag( pTailTag, 0)))[0];
	if( ! BaseGDL::interpreter->PtrValid(actP)) // 
				return NullGDL::GetSingleInstance();

    for( SizeT elIx = 0; elIx < nList; ++elIx)
      {
		DStructGDL* actPStruct = GetLISTStruct(e, actP);

		DPtr Ptr = (*static_cast<DPtrGDL*>(actPStruct->GetTag( pDataTag, 0)))[0];
		BaseGDL* data = BaseGDL::interpreter->GetHeapNoThrow( Ptr);
		if( data == NULL || data == NullGDL::GetSingleInstance())
			(*result)[elIx] = 0;
        else (*result)[elIx] = lib::array_equal_bool(data, value) ? 1 : 0;

		actP = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
      }    
    resultGuard.Release();
    return result;
}
  void container__remove( EnvUDT* e)
  {

  // sALL, POSITION are keyword.
	static int kwALLIx = e->GetKeywordIx("ALL");
	static int kwPOSITIONIx = e->GetKeywordIx("POSITION");


		trace_me = false; // lib::trace_arg();
	static int kwSELFIx = kwALLIx + 1;
	SizeT nParam = e->NParam(1);      

    DStructGDL* self = GetOBJ( e->GetKW( kwSELFIx), e);
	  
    GDL_CONTAINER_STRUCT()
    GDL_CONTAINER_NODE()
    bool asFunction = false;
	if(e->KeywordSet(kwALLIx)) {
	  LISTCleanup( e, self);
	  return;
	  }

  DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
  if( nList == 0)
    ThrowFromInternalUDSub( e, "LIST is empty.");

  BaseGDL* index = NULL;
  DLongGDL* indexLong = NULL;
  Guard<BaseGDL> indexLongGuard;
  if( nParam == 0)
    index = e->GetKW(kwPOSITIONIx);
  if( index != NULL)
  {
    if( index->Type() != GDL_LONG)
    {
      indexLong = static_cast<DLongGDL*>(index->Convert2(GDL_LONG,BaseGDL::COPY));
      indexLongGuard.Init( indexLong);
    }
    else
      indexLong = static_cast<DLongGDL*>(index);
	}
  
  DLong removePos = -1;
  if( indexLong != NULL)
  {
    if( indexLong->N_Elements() == 1)
    {
	removePos = (*indexLong)[0];
	if( removePos < 0)
	  removePos += nList;
	if( removePos < 0)
	  ThrowFromInternalUDSub( e, "Index too small.");
	if( removePos >= nList)
	  ThrowFromInternalUDSub( e, "Index out of range.");  
    }
  }
  
  if( removePos == nList-1) // remove head
  {

    DPtr pHead = (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];	      
    
    DStructGDL* headNode = GetLISTStruct(e, pHead);  
    
//    DPtr Ptr = (*static_cast<DPtrGDL*>( headNode->GetTag( pDataTag, 0)))[0];   
    
//    BaseGDL* data = BaseGDL::interpreter->GetHeap( Ptr);
    
    if( nList == 1)
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;    
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;
    }
    else if( nList == 2)
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 1;
    }
    else // nList > 2
    {
      DPtr pPredHead = GetLISTNode( e, self, nList-2);
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = pPredHead;    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;
    }
//     e->Interpreter()->HeapErase( pData); // no delete
//     e->Interpreter()->FreeHeap( pHead); // delete
    FreeLISTNode( e, pHead, !asFunction);
    
      return;
  }
  if( indexLong == NULL || removePos == 0) // remove tail
  {
    // implicit: nList > 1
    DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];	      
    
    DStructGDL* tailNode = GetLISTStruct(e, pTail);  
    
//    DPtr Ptr = (*static_cast<DPtrGDL*>( tailNode->GetTag( pDataTag, 0)))[0];   
    
//    BaseGDL* data = BaseGDL::interpreter->GetHeap( Ptr);
    
    if( nList == 2)
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];    
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 1;
    }
    else // nList > 2
    {
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 
      (*static_cast<DPtrGDL*>( tailNode->GetTag( pNextTag, 0)))[0];        
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;
    }
//     e->Interpreter()->HeapErase( pData); // no delete
//     e->Interpreter()->FreeHeap( pTail); // delete
    FreeLISTNode( e, pTail, !asFunction);
    
    return;    
  }
  if( removePos != -1) // single element
  {
    // implicit: nList > 2
    DPtr pPredNode = GetLISTNode( e, self, removePos-1);
    DStructGDL* predNode = GetLISTStruct( e, pPredNode);   

    DPtr pRemoveNode = (*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
    DStructGDL* removeNode = GetLISTStruct( e, pRemoveNode);   

//    DPtr Ptr = (*static_cast<DPtrGDL*>( removeNode->GetTag( pDataTag, 0)))[0];   
//    BaseGDL* data = BaseGDL::interpreter->GetHeap( Ptr);

    (*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( removeNode->GetTag( pNextTag, 0)))[0];
    
    (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = nList - 1;

//     e->Interpreter()->HeapErase( pData); // no delete
//     e->Interpreter()->FreeHeap( pRemoveNode); // no delete
    FreeLISTNode( e, pRemoveNode, !asFunction);
    
    return;
  }
  
  // remove all indexed elements
  // 1st build return LIST
//  BaseGDL* newObj = NULL; // the list object
//  Guard<BaseGDL> newObjGuard;
  SizeT indexN_Elements = indexLong->N_Elements();
 
  // 2nd: remove the indexed elements
  if( indexLongGuard.Get() == NULL)
  {
    // we need to sort the index
    indexLong = indexLong->Dup();
    indexLongGuard.Init(indexLong);
  }
  DLong *hh = static_cast<DLong*>(indexLong->DataAddr());
// massage the indeces so that < 0 are back in range
  for( DLong i=0; i < indexN_Elements; ++i)
	    if( hh[i] < 0) hh[i] += nList;
// This needed for the tail->head removall.	  
  DLong* h1 = new DLong[ indexN_Elements/2];
  DLong* h2 = new DLong[ (indexN_Elements+1)/2];
  // call the sort routine
  MergeSortDescending<DLong>( hh, h1, h2, indexN_Elements);
  delete[] h1;
  delete[] h2;
  
	DPtrGDL* Tail = static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0));
	DPtrGDL* Head = static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0));
		if(trace_me) 
		{	DPtr p0 = (*Tail)[0];
			std::printf(" tracing list.remove : TAIL=%llu", p0);
			for( int i=0; i < nList ; i++) 
			{	DStructGDL* Node=GetLISTStruct(NULL, p0);
				p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
				std::printf("->%llu", p0);}
			std::printf(" : HEAD= %llu (incoming) \n",  (*Head)[0]);
		}
	  if(trace_me) {
		  std::cout << "list.Remove() "+i2s(indexN_Elements)+" indeces ";
			for( DLong i=0; i < indexN_Elements; ++i)
				std::cout << hh[ indexN_Elements - i - 1] <<": ";
		  std::cout << std::endl;
	  }
	SizeT nListStart = nList;
	DLong prvfetch = -1; // repeated indeces need to be skipped!!
    DPtr pTail = (*Tail)[0];
	DPtr predptr;
	DPtr p0 = pTail;
	DLong inlist = 0;
	DStructGDL* Node;
	DStructGDL* predNode;
	for( DLong i=0; i < indexN_Elements; ++i)
	  { 		// get the next index targetted for removal.  
		DLong removeIndex = hh[ indexN_Elements - i - 1];
		if( removeIndex < 0)
		  ThrowFromInternalUDSub( e, "Index too small:"+i2s(removeIndex) );
		if( removeIndex >= nListStart)
		  ThrowFromInternalUDSub( e, "Index out of range: "+i2s(removeIndex) );
		if( removeIndex == prvfetch) continue; // (ignore repeats)

//	  if(trace_me)  std::cout << i2s(removeIndex) << " p0="<<p0<<" " ;
		
			assert( nList >= 1);
		Node = GetLISTStruct(NULL, p0);

		for( DLong j=0; j < removeIndex - prvfetch - 1; j++) 
		{	inlist++;
			predptr = p0;
//			if(trace_me)  std::cout <<" ^" << p0;
	  		p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
			predNode = Node;
			Node = GetLISTStruct(NULL, p0);
		}
		DPtrGDL* ptrnxt = static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0));

		if(inlist == 0) {

			(*Tail)[0] = (*ptrnxt)[0];

		} 
		else if( inlist == nList -1 )
		{   
			(*Head)[0] = predptr;
			(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 0;
		}
		else
			(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = 
				(*ptrnxt)[0];
   
//		DPtr Ptr = (*static_cast<DPtrGDL*>( Node->GetTag( pDataTag, 0)))[0];	      
//		BaseGDL::interpreter->FreeHeap( Ptr);

		DPtr pNext = (*ptrnxt)[0];  // delete p0 from heap
		(*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0] = 0;  
		BaseGDL::interpreter->FreeHeap( p0);
		p0 = pNext;			// & get ready for next item.

		(*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = --nList;
		prvfetch = removeIndex;
	}

	if(trace_me) 
			{
				std::cout<< std::endl; 
	DPtrGDL* Tail = static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0));
	DPtrGDL* Head = static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0));
					DPtr p0 = (*Tail)[0];
				std::printf(" tracing: TAIL=%llu", p0);
				for( int i=0; i < nList-1 ; i++) 
				{	DStructGDL* Node=GetLISTStruct(NULL, p0);
					p0 = (*static_cast<DPtrGDL*>( Node->GetTag( pNextTag, 0)))[0];
					std::printf("->%llu", p0);}
				std::printf(" : HEAD= %llu (outgoing) \n",  (*Head)[0]);
			}


//  newObjGuard.Release();
//  return newObj;
  }

  
} // namespace lib
