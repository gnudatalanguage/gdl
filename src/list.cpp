/***************************************************************************
                          list.cpp  - for LIST objects
                             -------------------
    begin                : July 22 2013
    copyright            : (C) 2013 by M. Schellens et al.
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

#include "nullgdl.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"
  
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
  
  DStructGDL* GetLISTStruct( EnvUDT* e, DPtr actP)
  {
    static DString cNodeName("GDL_CONTAINER_NODE");
    DStructDesc* containerDesc=FindInStructList( structList, cNodeName);
    BaseGDL* actPHeap;
    try {
      actPHeap = BaseGDL::interpreter->GetHeap( actP);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
      ThrowFromInternalUDSub( e, "LIST container node ID <"+i2s(actP)+"> not found.");      
    }
    if( actPHeap == NULL || actPHeap->Type() != GDL_STRUCT)
      ThrowFromInternalUDSub( e, "LIST node must be a STRUCT.");
//       throw GDLException( "LIST node must be a STRUCT.");	
    DStructGDL* actPStruct = static_cast<DStructGDL*>( actPHeap);
    if( actPStruct->Desc() != containerDesc)
      ThrowFromInternalUDSub( e, "LIST node must be a GDL_CONTAINER_NODE STRUCT.");
//       throw GDLException( "LIST node must be a GDL_CONTAINER_NODE STRUCT.");	
    return actPStruct;
  }

  void FreeLISTNode( EnvUDT* e, DPtr pRemoveNode, bool deleteData = true)
  {
    static DString cNodeName("GDL_CONTAINER_NODE");
    DStructDesc* containerDesc=FindInStructList( structList, cNodeName);
    static unsigned pNextTag = containerDesc->TagIndex( "PNEXT");
    static unsigned pDataTag = containerDesc->TagIndex( "PDATA");
    
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
    static DString cNodeName("GDL_CONTAINER_NODE");
    DStructDesc* containerDesc=FindInStructList( structList, cNodeName);

    DStructDesc* listDesc=self->Desc();

    // here static is fine
    static unsigned pHeadTag = listDesc->TagIndex( "PHEAD");
    static unsigned pTailTag = listDesc->TagIndex( "PTAIL");
    static unsigned nListTag = listDesc->TagIndex( "NLIST");

    static unsigned pNextTag = containerDesc->TagIndex( "PNEXT");
    static unsigned pDataTag = containerDesc->TagIndex( "PDATA");
  
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
    
//     DStructGDL* actPStruct = GetLISTStruct(actP);   
//     return actPStruct;
  }
  
  DStructGDL*GetSELF( BaseGDL* selfP, EnvUDT* e)
  {
    if( selfP == NULL || selfP->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
    if( !selfP->Scalar())
      ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");
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
  
namespace lib {
 
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
  //     DFunLIST__ADD->AddKey("EXTRACT","EXTRACT")->AddKey("NO_COPY","NO_COPY");
  //     DFunLIST__ADD->AddPar("VALUE")->AddPar("INDEX");

  static int kwALLIx = 0; 
  static int kwSELFIx = 1;
  static int kwINDEXIx = 2;

  bool kwALL = false;
  if (e->KeywordSet(kwALLIx)){ kwALL = true;}

  SizeT nParam = e->NParam(1); // minimum SELF
      
//   BaseGDL* selfP = e->GetKW( kwSELFIx);    
//   if( selfP == NULL || selfP->Type() != GDL_OBJ)
//     ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
//   if( !selfP->Scalar())
//     ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");
//   DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
//   DObj selfID = (*selfObj)[0];
//   DStructGDL* self = e->Interpreter()->GetObjHeap( selfID);
  DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);
  DStructDesc* listDesc= self->Desc();

  static DString cNodeName("GDL_CONTAINER_NODE");

  // because of .RESET_SESSION, we cannot use static here
  DStructDesc* containerDesc=FindInStructList( structList, cNodeName);

  assert( listDesc != NULL && listDesc->NTags() > 0);
  assert( containerDesc != NULL && containerDesc->NTags() > 0);

  // here static is fine
  static unsigned pHeadTag = listDesc->TagIndex( "PHEAD");
  static unsigned pTailTag = listDesc->TagIndex( "PTAIL");
  static unsigned nListTag = listDesc->TagIndex( "NLIST");
  static unsigned pNextTag = containerDesc->TagIndex( "PNEXT");
  static unsigned pDataTag = containerDesc->TagIndex( "PDATA");

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
      DPtr pTail = (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0];
      // trigger ref-count delete of all elements      
      BaseGDL::interpreter->FreeHeap( pTail); 
      
      (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0] = 0;	      
      (*static_cast<DPtrGDL*>( self->GetTag( pTailTag, 0)))[0] = 0;	      
      (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0] = 0;
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
    if( removeIndex == 0) // remove tail
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
    // no args no kwords
    SizeT nParam = e->NParam(1); // minimum SELF

    DStructGDL* self = GetSELF( e->GetKW( 0), e);
  
    static DString cNodeName("GDL_CONTAINER_NODE");
    
    DStructDesc* listDesc= self->Desc();

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* containerDesc=FindInStructList( structList, cNodeName);
    assert( listDesc != NULL && listDesc->NTags() > 0);
    assert( containerDesc != NULL && containerDesc->NTags() > 0);

    // here static is fine
    static unsigned pHeadTag = listDesc->TagIndex( "PHEAD");
    static unsigned pTailTag = listDesc->TagIndex( "PTAIL");
    static unsigned nListTag = listDesc->TagIndex( "NLIST");
    static unsigned pNextTag = containerDesc->TagIndex( "PNEXT");
    static unsigned pDataTag = containerDesc->TagIndex( "PDATA");
    
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
      
//   BaseGDL* selfP = e->GetKW( kwSELFIx);    
//   if( selfP == NULL || selfP->Type() != GDL_OBJ)
//     ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
//   if( !selfP->Scalar())
//     ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");
//   DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
//   DObj selfID = (*selfObj)[0];
//   DStructGDL* self = e->Interpreter()->GetObjHeap( selfID);
  
  DStructGDL* self = GetSELF( e->GetKW( kwSELFIx), e);
  
  
  static DString cNodeName("GDL_CONTAINER_NODE");

  DStructDesc* listDesc= self->Desc();

  // because of .RESET_SESSION, we cannot use static here
  DStructDesc* containerDesc=FindInStructList( structList, cNodeName);

  assert( listDesc != NULL && listDesc->NTags() > 0);
  assert( containerDesc != NULL && containerDesc->NTags() > 0);

  // here static is fine
  static unsigned pHeadTag = listDesc->TagIndex( "PHEAD");
  static unsigned pTailTag = listDesc->TagIndex( "PTAIL");
  static unsigned nListTag = listDesc->TagIndex( "NLIST");
  static unsigned pNextTag = containerDesc->TagIndex( "PNEXT");
  static unsigned pDataTag = containerDesc->TagIndex( "PDATA");

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

      if( insertPos == -1 || insertPos == nList) // head
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
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTail, 0)))[0] = firstID;	      
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
      
      if( insertPos == -1 || insertPos == nList) // head
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
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTail, 0)))[0] = cID;	      
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

    SizeT listLength = 0;
    DLongGDL* lengthKW = e->IfDefGetKWAs<DLongGDL>(kwLENGTHIx);
    if( lengthKW != NULL)
    {
      listLength = (*lengthKW)[0];
      if( listLength < 0)
	listLength = 0;
    }
    
    ProgNodeP cN = e->CallingNode();
    DInterpreter* ip = e->Interpreter();
    
    static DString listName("LIST");
    static DString cNodeName("GDL_CONTAINER_NODE");
    
    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* listDesc=FindInStructList( structList, listName);
    DStructDesc* containerDesc=FindInStructList( structList, cNodeName);

    // here static is fine
    static unsigned pHeadTag = listDesc->TagIndex( "PHEAD");
    static unsigned pTailTag = listDesc->TagIndex( "PTAIL");
    static unsigned nListTag = listDesc->TagIndex( "NLIST");

    static unsigned pNextTag = containerDesc->TagIndex( "PNEXT");
    static unsigned pDataTag = containerDesc->TagIndex( "PDATA");
    
    
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

	    pID = ip->NewHeap(1,p->NewIx(eIx)); // sets ref count to 1
	    
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
  
  
} // namespace lib