/***************************************************************************
                          hash.cpp  - for HASH objects
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
  
// if not found returns -(pos +1)
DLong HashIndex( DStructGDL* hashTable, BaseGDL* key)
{
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
  static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");

  assert( key != NULL && key != NullGDL::GetSingleInstance());
  
  DLong searchIxStart = 0;
  DLong searchIxEnd = hashTable->N_Elements();
  
  for(;;)
  {
    DLong searchIx = (searchIxStart + searchIxEnd) / 2;
    if( (*static_cast<DPtrGDL*>( hashTable->GetTag( pKeyTag, searchIx)))[0] == 0)
    {
	DLong checkIx = searchIx-1;
	while( checkIx >= searchIxStart &&
	(*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, checkIx)))[0] == 0) 
	  --checkIx;
	if( checkIx < searchIxStart)
	{
	  checkIx = searchIx+1;
	  while( checkIx < searchIxEnd &&
	  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, checkIx)))[0] == 0) 
	    ++checkIx;
	  if( checkIx == searchIxEnd)
	  {
	    // only empty elements found in interval
	    return -(searchIx + 1);
	  }
	}
	searchIx = checkIx;
    }
    DPtr kID = (*static_cast<DPtrGDL*>( hashTable->GetTag( pKeyTag, searchIx)))[0];
    assert( kID != 0);
    int hashCompare = key->HashCompare( BaseGDL::interpreter->GetHeap( kID));
    if( hashCompare == 0)
      return searchIx;
    
    if( searchIxStart == searchIxEnd)      
    {
	return -(searchIxStart + 1);
    }

    if( hashCompare == -1) // key < hashKey[searchIx]
      searchIxEnd = searchIx;
    else
    { // key > hashKey[searchIx]
      searchIxStart = searchIx+1;
      if( searchIxStart >= hashTable->N_Elements())
      {
	return -(hashTable->N_Elements() + 1);
      }
    }
    if( searchIxStart == searchIxEnd && searchIx == searchIxStart)      
    {
	return -(searchIxStart + 1);
    }
  }
}
  
DStructGDL* GrowHashTable( DStructGDL* hashStruct, DStructGDL* hashTable, DLong nSizeNew)
{
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
  static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");
  
//   DStructDesc* hashDesc = hashStruct->Desc();
//   DStructDesc* entryDesc = hashTable->Desc();
 
  DLong nSize = (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0];
   
  DStructGDL* newHashTable= new DStructGDL( hashTable->Desc(), dimension(nSize));

  // copy old table to new one, insert holes
  SizeT oldIx = 0;
  for( SizeT nAdd=0; nAdd<nCount; ++nAdd)
  {
    // find next old entry
    while( oldIx < nSize &&
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] == 0) 
      oldIx++;
    assert( oldIx < nSize);  
      
    SizeT newIx = nAdd * nSizeNew / nCount;
    assert( newIx >= nAdd);
    
    (*static_cast<DPtrGDL*>(newHashTable->GetTag( pKeyTag, newIx)))[0] =
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0];          
    // prevent ref-count cleanup
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] = 0;

    (*static_cast<DPtrGDL*>(newHashTable->GetTag( pValueTag, newIx)))[0] = 
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0];
    // prevent ref-count cleanup
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0] = 0;
    
  }
  DPtr hashTableID = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
  assert( BaseGDL::interpreter->GetHeap( hashTableID) == hashTable);
  // delete old
  delete hashTable;
  // set new
  BaseGDL::interpreter->GetHeap( hashTableID) = newHashTable;
  (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = nSizeNew;
}
  
  
void InsertIntoHashTable( DStructGDL* hashStruct, DStructGDL* hashTable, BaseGDL* key, BaseGDL* value)
{
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
  static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");
  
//   DStructDesc* hashDesc = hashStruct->Desc();
//   DStructDesc* entryDesc = hashTable->Desc();
 
  DLong nSize = (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0];

  
//   SizeT actPosPtr = 0;
//    std::cout << "inserting:  ";
//    key->ToStream( std::cout, 80, &actPosPtr);
//    std::cout << ":";
//    value->ToStream( std::cout, 80, &actPosPtr);
  
  if( nCount == 0)
  {
    assert( nSize >= 1);
    DLong insertPos = nSize / 2;
//    std::cout << "   at " <<  i2s(insertPos) << std::endl;
    DPtr pID = BaseGDL::interpreter->NewHeap(1,value);
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertPos)))[0] = pID;
    DPtr kID = BaseGDL::interpreter->NewHeap(1,key);
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertPos)))[0] = kID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = 1;
    return;
  }
  
  DLong hashIndex = HashIndex( hashTable, key);
  if( hashIndex >= 0) // hit -> overwrite
  {
//    std::cout << "  (ovwrt) at "<< i2s(hashIndex) <<std::endl;
   
    assert( hashIndex < nSize);
    DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, hashIndex)))[0];
    DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, hashIndex)))[0];
    GDLDelete( BaseGDL::interpreter->GetHeap( vID));
    GDLDelete( BaseGDL::interpreter->GetHeap( kID));
    BaseGDL::interpreter->GetHeap( vID) = value;
    BaseGDL::interpreter->GetHeap( kID) = key;
    return;
  }

  if( nSize <= ((nCount+1) * 10 / 9)) // grow on 90% occupation
  {
    DStructGDL* hashTable = GrowHashTable( hashStruct, hashTable, nSize * 2);
    nSize = (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  }

  // new key -> insert 
  DLong insertPos = -(hashIndex + 1);
   
//    std::cout << "   try "<< i2s(insertPos) << "... ";

  // make some space
  DLong nextFreeElementIx = insertPos;
  for( ; nextFreeElementIx < nSize; ++nextFreeElementIx)
  {
    if( (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, nextFreeElementIx)))[0] == 0)
    {
      // shuffle elements away to make space for new element
      // we could optimize this by using a new DStructGDL function
      for( DLong i=nextFreeElementIx; i>insertPos; --i)
      {
	(*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0] =
	(*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i-1)))[0];
	
	(*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0] =
	(*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i-1)))[0];

// 	std::cout << i2s(i-1) << " -> " << i2s(i) << std::endl;
      }
      break;  
    }
  }
  if( nextFreeElementIx >= nSize)
  {
    nextFreeElementIx = insertPos - 1;
    for( ; nextFreeElementIx >= 0; --nextFreeElementIx)
    {
      if( (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, nextFreeElementIx)))[0] == 0)
      {
	for( DLong i=nextFreeElementIx; i<insertPos; ++i)
	{
	  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0] =
	  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i+1)))[0];
	  
	  (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0] =
	  (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i+1)))[0];

// 	  std::cout << i2s(i+1) << " -> " << i2s(i) << std::endl;
	}
	break;
      }      
    }
  }

  assert( nextFreeElementIx >= 0 && nextFreeElementIx < nSize);

  // insert the element
  // overwrite, the (now overwritten) pointers are already moved or are NULL
  DPtr kID = BaseGDL::interpreter->NewHeap(1,key);
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertPos)))[0] = kID;
  DPtr pID = BaseGDL::interpreter->NewHeap(1,value);
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertPos)))[0] = pID;

//   std::cout << "   at "<< i2s(insertPos) << "(" << i2s(kID) << "," << i2s(pID) << ")" <<std::endl;

  (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = ++nCount;
}
  
  
void HASH__ToStream( DStructGDL* oStructGDL, std::ostream& o, SizeT w, SizeT* actPosPtr)
{	  
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
  static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");
  
  SizeT nCount = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nCountTag, 0)))[0];
  SizeT nSize = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nSizeTag, 0)))[0];

  DPtr pHashTable = (*static_cast<DPtrGDL*>( oStructGDL->GetTag( pDataTag, 0)))[0];
  DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

  SizeT ix = 0;
  for( SizeT i=0; i<nCount; ++i, ++ix)
  {
    while( ix < nSize &&
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, ix)))[0] == 0) 
      ++ix;
    assert( ix < nSize);  
    
    DPtr pKey = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, ix)))[0];
    DPtr pValue = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, ix)))[0];

    BaseGDL* key = BaseGDL::interpreter->GetHeap( pKey);
    assert( key != NULL);

    BaseGDL* value = BaseGDL::interpreter->GetHeap( pValue);
    if( value == NULL) value = NullGDL::GetSingleInstance();
    
//     std::cout << "("<<i2s(pKey)<<","<<i2s(pValue)<<")  ";
    key->ToStream( o, w, actPosPtr);
    o << ":";
    value->ToStream( o, w, actPosPtr);
    if( (i+1) < nCount)
      o << std::endl;
  }
}

DLong GetInitialTableSize( DLong nEntries)
{
  DLong initialTableSize = 4;
  DLong minEntries = nEntries * 2; // initial min 50% filling
  while( initialTableSize < minEntries) initialTableSize *= 2;
  return initialTableSize;
}
  
namespace lib {

  BaseGDL* HASH___OverloadBracketsRightSide( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
    static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");

    const unsigned par1Ix = 2;
    
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
    
    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
    DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));
    
    // default behavior: Exact like scalar indexing
    BaseGDL* isRange = e->GetKW(1);
    if( isRange == NULL)
      ThrowFromInternalUDSub( e, "Parameter 1 (ISRANGE) is undefined.");
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
    
    BaseGDL* index = NULL;
    BaseGDL* parX = e->GetKW( par1Ix); // implicit SELF, ISRANGE, par1..par8
    if( parX == NULL)
      ThrowFromInternalUDSub( e, "Parameter is undefined: "  + e->Caller()->GetString(e->GetKW( par1Ix)));

    DLong isRangeX = (*isRangeLong)[0];
    if( isRangeX != 0 && isRangeX != 1)
    {
      ThrowFromInternalUDSub( e, "Value of parameter 1 (ISRANGE["+i2s(0)+"]) is out of allowed range.");
    }
    if( isRangeX == 1)
    {
      if( parX->N_Elements() != 3)
      {
	ThrowFromInternalUDSub( e, "Range vector must have 3 elements: " + e->Caller()->GetString(e->GetKW( par1Ix)));
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
      if( (*parXLong)[0] != 0 || (*parXLong)[1] != -1 || (*parXLong)[2] != 1)
	ThrowFromInternalUDSub( e, "Subscript range is not allowed: [" + 
	i2s((*parXLong)[0])+","+	i2s((*parXLong)[1])+","+i2s((*parXLong)[2])+"]");
      
      // full range -> clone ===================================================
      DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];
      SizeT nEntries = nCount;
    
      DLong initialTableSize = GetInitialTableSize( nEntries);
    
      // new hash
      DStructGDL* hashStruct= new DStructGDL( self->Desc(), dimension());
      DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
      BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
      Guard<BaseGDL> newObjGuard( newObj);
      // the return hash table
      DStructGDL* hashTable= new DStructGDL( thisHashTable->Desc(), dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;
      (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = initialTableSize;
    
      SizeT sourceIx = 0;
      for( SizeT eIx=0; eIx<nEntries; ++eIx)
      {
	DPtr kID = (*static_cast<DPtrGDL*>( thisHashTable->GetTag( pKeyTag, sourceIx)))[0];
	while( kID == 0)
	  kID = (*static_cast<DPtrGDL*>( thisHashTable->GetTag( pKeyTag, ++sourceIx)))[0];

	DPtr vID = (*static_cast<DPtrGDL*>(thisHashTable->GetTag( pValueTag, sourceIx)))[0];

	BaseGDL* key = e->Interpreter()->GetHeap( kID);
	assert( key != NULL);
	BaseGDL* value = e->Interpreter()->GetHeap( vID);
	if( value != NULL)
	  value = value->Dup();
	
	InsertIntoHashTable( hashStruct, hashTable, key->Dup(), value);
      }
      newObjGuard.Release();
      return newObj;
    }

    // non-range
    index = parX;

    // one element -> return value
    if( index->N_Elements() == 1)
    {
      DLong hashIndex = HashIndex( thisHashTable, index);
      if( hashIndex < 0)
	ThrowFromInternalUDSub( e, "Key does not exist.");
      DPtr vID = (*static_cast<DPtrGDL*>(thisHashTable->GetTag( pValueTag, hashIndex)))[0];
      BaseGDL* value = e->Interpreter()->GetHeap( vID);
      if( value == NULL)
	return NullGDL::GetSingleInstance();
      return value->Dup();
    }

    // multi element -> return new hash
    SizeT nEntries = index->N_Elements();
  
    DLong initialTableSize = GetInitialTableSize( nEntries);
  
    // new hash
    DStructGDL* hashStruct= new DStructGDL( self->Desc(), dimension());
    DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
    BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
    Guard<BaseGDL> newObjGuard( newObj);
    // the return hash table
    DStructGDL* hashTable= new DStructGDL( thisHashTable->Desc(), dimension(initialTableSize));
    DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
    (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = initialTableSize;
  
    for( SizeT eIx=0; eIx<nEntries; ++eIx)
    {
      BaseGDL* actKey = index->NewIx( eIx);
      Guard<BaseGDL> actkeyGuard( actKey);
      
      // search in this hash table
      DLong hashIndex = HashIndex( thisHashTable, actKey);
      if( hashIndex < 0)
	ThrowFromInternalUDSub( e, "Key does not exist.");
      
      DPtr vID = (*static_cast<DPtrGDL*>(thisHashTable->GetTag( pValueTag, hashIndex)))[0];
      BaseGDL* value = e->Interpreter()->GetHeap( vID);
      if( value != NULL)
	value = value->Dup();
            
      actkeyGuard.Release();
      InsertIntoHashTable( hashStruct, hashTable, actKey, value);
    }
    newObjGuard.Release();
    return newObj;
  }

  
  
  BaseGDL* hash_fun( EnvT* e)
  {
    static int kwNO_COPYIx = e->KeywordIx("NO_COPY");
    bool kwNO_COPY = false;
    if (e->KeywordSet(kwNO_COPYIx)){ kwNO_COPY = true;}

    SizeT nParam = e->NParam();
    if( nParam != 1 && nParam % 2 == 1)
      e->Throw( "Wrong number of parameters.");
    
    ProgNodeP cN = e->CallingNode();
    DInterpreter* ip = e->Interpreter();
  
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = FindInStructList( structList, hashName)->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
    static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* hashDesc=FindInStructList( structList, hashName);
    DStructDesc* entryDesc=FindInStructList( structList, entryName);
    assert( hashDesc != NULL && hashDesc->NTags() > 0);
    assert( entryDesc != NULL && entryDesc->NTags() > 0);

    DStructGDL* hashStruct= new DStructGDL( hashDesc, dimension());

    DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 

    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);

    SizeT nEntries = nParam/2;
    
    DLong initialTableSize = GetInitialTableSize( nEntries);
    
    DStructGDL* hashTable= new DStructGDL( entryDesc, dimension(initialTableSize));
    DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
    
    (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = initialTableSize;
    
    for( SizeT eIx=0; eIx<nEntries; ++eIx)
    {
      SizeT keyIx = 2 * eIx;
      SizeT valueIx = 2 * eIx + 1;
      BaseGDL* key = e->GetPar( keyIx);
      // !NULL keys are not inserted
      if( key == NULL || key == NullGDL::GetSingleInstance())
	continue;
      
      SizeT nKey = key->N_Elements();

      if( key->Type() == GDL_STRUCT)
      {
	if( nParam > 1)
	  e->Throw("Only 1 argument is allowed with input of type STRUCT.");
	DStructGDL* keyStruct = static_cast<DStructGDL*>(key);
	DStructDesc* desc = keyStruct->Desc();
	for( SizeT t=0; t<desc->NTags(); ++t)
	{
	  DStringGDL *structKey = new DStringGDL( desc->TagName(t));
	  BaseGDL* structData = keyStruct->GetTag(t,0);
	  assert(structData != NULL);
	  structData = structData->Dup();
	  
	  InsertIntoHashTable( hashStruct, hashTable, structKey, structData);
	}
      }
      else 
      {
	if( nParam == 1)
	  e->Throw( "Single parameter must be a STRUCT.");
	
	BaseGDL* value = e->GetPar( valueIx);
	SizeT nValue = 0;
	if( value != NULL)
	  nValue = value->N_Elements();
	
	if( nValue != 0 && nKey != 1 && nValue != nKey)
	  e->Throw( "Key and Value must have the same number of elements.");
		
	if( nKey == 1)
	{
	  if( !kwNO_COPY && value != NULL)
	    value = value->Dup();
	  key = key->Dup();

	  InsertIntoHashTable( hashStruct, hashTable, key, value);
	}
	else // nkey > 1
	{
	  if( value == NULL /*|| value == NullGDL::GetSingleInstance()*/)
	  {
	    for( SizeT kIx=0; kIx<nKey; ++kIx)
	      InsertIntoHashTable( hashStruct, hashTable, key->NewIx(kIx), NULL);
	  }
	  else
	  {
	    for( SizeT kIx=0; kIx<nKey; ++kIx)
	      InsertIntoHashTable( hashStruct, hashTable, key->NewIx(kIx), value->NewIx(kIx));
	  }
	}
	if( kwNO_COPY)
	{
	  bool stolen = e->StealLocalPar( valueIx);
	  if( !stolen) e->GetPar(valueIx) = NULL;
	}
      }
    }
    
    newObjGuard.Release();
    return newObj;
  } 
  
} // namespace lib