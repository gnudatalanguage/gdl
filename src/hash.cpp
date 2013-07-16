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
DLong HashIndex( DStructGDL* hashTable, DLong nCount, BaseGDL* key)
{
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pKeyTag = FindInStructList( structList, entryName)->TagIndex( "PKEY");
  static unsigned pValueTag = FindInStructList( structList, entryName)->TagIndex( "PVALUE");

  assert( nCount != 0);
  
  DLong searchIxStart = 0;
  DLong searchIxEnd = nCount;
  
  for(;;)
  {
    DLong searchIx = (searchIxStart + searchIxEnd) / 2;
    DPtr kID = (*static_cast<DPtrGDL*>( hashTable->GetTag( pKeyTag, searchIx)))[0];
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
      searchIxStart = searchIx+1 ;
    }
    if( searchIxStart == searchIxEnd && searchIx == searchIxStart)      
    {
	return -(searchIxStart + 1);
    }
  }
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
  
  if( nCount == 0)
  {
    assert( nSize >= 1);
    DPtr pID = BaseGDL::interpreter->NewHeap(1,value);
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, 0)))[0] = pID;
    DPtr kID = BaseGDL::interpreter->NewHeap(1,key);
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, 0)))[0] = kID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = 1;
    return;
  }
  
  DLong hashIndex = HashIndex( hashTable, nCount, key);
  if( hashIndex >= 0) // hit -> overwrite
  {
    assert( hashIndex < nSize);
    DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, hashIndex)))[0];
    DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, hashIndex)))[0];
    GDLDelete( BaseGDL::interpreter->GetHeap( vID));
    GDLDelete( BaseGDL::interpreter->GetHeap( kID));
    BaseGDL::interpreter->GetHeap( vID) = value;
    BaseGDL::interpreter->GetHeap( kID) = key;
    return;
  }
  
  if( nSize == nCount)
  {
    // TODO: introduce DStructGDL memeber function for this job
    nSize *= 2;
    DStructGDL* newHashTable= new DStructGDL( hashTable->Desc(), dimension(nSize));
    // copy old table to new one
    for( SizeT i=0; i<nCount; ++i)
    {
      (*static_cast<DPtrGDL*>(newHashTable->GetTag( pValueTag, i)))[0] = 
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0];
      // prevent ref-count cleanup
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0] = 0;
      (*static_cast<DPtrGDL*>(newHashTable->GetTag( pKeyTag, i)))[0] =
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0];          
      // prevent ref-count cleanup
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0] = 0;
    }
    DPtr hashTableID = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
    assert( BaseGDL::interpreter->GetHeap( hashTableID) == hashTable);
    // delete old
    delete hashTable;
    // set new
    hashTable = newHashTable;
    BaseGDL::interpreter->GetHeap( hashTableID) = hashTable;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = nSize;
  }

  // new key -> insert 
  DLong insertPos = -(hashIndex + 1);
   
  // make space
  for( RangeT i = nCount; i >= (insertPos+1); --i)
  {
    // we could optimize this by using a new DStructGDL function
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0] = 
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i-1)))[0];
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0] = 
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i-1)))[0];    
  }
  
  DPtr pID = BaseGDL::interpreter->NewHeap(1,value);
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertPos)))[0] = pID;
  DPtr kID = BaseGDL::interpreter->NewHeap(1,key);
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertPos)))[0] = kID;
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

  DPtr pHashTable = (*static_cast<DPtrGDL*>( oStructGDL->GetTag( pDataTag, 0)))[0];
  DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

  for( SizeT i=0; i<nCount; ++i)
  {
    DPtr pKey = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0];
    DPtr pValue = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0];

    BaseGDL* key = BaseGDL::interpreter->GetHeap( pKey);
    assert( key != NULL);

    BaseGDL* value = BaseGDL::interpreter->GetHeap( pValue);
    if( value == NULL) value = NullGDL::GetSingleInstance();
    
    key->ToStream( o, w, actPosPtr);
    o << ":";
    value->ToStream( o, w, actPosPtr);
    if( (i+1) < nCount)
      o << std::endl;
  }
}

  
namespace lib {

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
    
    DLong initialTableSize = 4;
    while( initialTableSize < nEntries) initialTableSize *= 2;
    
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