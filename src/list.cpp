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
#include "basic_pro.hpp"
#include "terminfo.hpp"


namespace lib {

  
  DStructGDL* GetLISTStruct( DPtr actP)
  {
    static DString cNodeName("GDL_CONTAINER_NODE");
    DStructDesc* containerDesc=FindInStructList( structList, cNodeName);
    BaseGDL* actPHeap = BaseGDL::interpreter->GetHeap( actP);
    if( actPHeap->Type() != GDL_STRUCT)
      throw GDLException( "LIST node must be a STRUCT.");	
    DStructGDL* actPStruct = static_cast<DStructGDL*>( actPHeap);
    if( actPStruct->Desc() != containerDesc)
      throw GDLException( "LIST node must be a GDL_CONTAINER_NODE STRUCT.");	
    return actPStruct;
  }

  DStructGDL* GetLISTNode( DStructGDL* self, DLong targetIx)
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
	DStructGDL* actPStruct = GetLISTStruct(actP);

	actP = (*static_cast<DPtrGDL*>( actPStruct->GetTag( pNextTag, 0)))[0];
      }
    }
    
    DStructGDL* actPStruct = GetLISTStruct(actP);
   
    return actPStruct;
  }
  
  DStructGDL*GetSELF( BaseGDL* selfP, EnvUDT* e)
  {
    if( selfP == NULL || selfP->Type() != GDL_OBJ)
      ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
    if( !selfP->Scalar())
      ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");
    DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
    DObj selfID = (*selfObj)[0];
    return BaseGDL::interpreter->GetObjHeap( selfID);
  }
  
  BaseGDL* list__remove( EnvUDT* e)
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
    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);
    // we need ref counting here as the LIST (newObj) is a regular return value
    e->Interpreter()->IncRefObj( objID);
    
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
  

  DStructGDL* listStruct= self;
  DLong nList = (*static_cast<DLongGDL*>( self->GetTag( nListTag, 0)))[0];	      
  
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
  
  if( index == NULL) // remove head
  {
    DPtr pHead = (*static_cast<DPtrGDL*>( self->GetTag( pHeadTag, 0)))[0];	      
    DStructGDL* headNode = GetLISTStruct(pHead);  
    
    DPtr pData = (*static_cast<DPtrGDL*>( headNode->GetTag( pDataTag, 0)))[0];
    
    BaseGDL* data = BaseGDL::interpreter->GetHeap( pData);
  }
  
  
  try{
    
  }
  catch( GDLException& ex)
  {
    ThrowFromInternalUDSub( e, ex.getMessage());
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

  try{
    
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
	DStructGDL* headNode = GetLISTStruct(pHead);  
	
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
	DStructGDL* predNode = GetLISTNode( self, insertPos-1);
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
	DStructGDL* headNode = GetLISTStruct(pHead);  
	
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
	DStructGDL* predNode = GetLISTNode( self, insertPos-1);
	(*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = 
	(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0];
	(*static_cast<DPtrGDL*>( predNode->GetTag( pNextTag, 0)))[0] = cID;
      }
    }
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nList+1;	      
  }
  catch( GDLException& ex)
  {
    ThrowFromInternalUDSub( e, ex.getMessage());
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

    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct

    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);
    // we need ref counting here as the LIST (newObj) is a regular return value
    e->Interpreter()->IncRefObj( objID);
    
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

	    pID = ip->NewHeap(1,p->NewIx(eIx));
	    
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
	    pID = ip->NewHeap(1,p);
	    bool stolen = e->StealLocalPar( pIx);
	    if( !stolen) e->GetPar(pIx) = NULL;
	  }
	  else
	  {
	    pID = ip->NewHeap(1,p->Dup());
	  }
  
	  cStruct= new DStructGDL( containerDesc, dimension());
  
	  (*static_cast<DPtrGDL*>( cStruct->GetTag( pDataTag, 0)))[0] = pID;
	  // no ref counting here (and below) as heap data cannot be accessed outside object
	  
	  cID = ip->NewHeap(1,cStruct);

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

    if( cStruct != NULL)
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pNextTag, 0)))[0] = 0;
	    
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = added;	      

    newObjGuard.Release();
    return newObj;
  }
  
  
} // namespace lib