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
  
// if not found returns -pos -1
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
    
    if( hashCompare == -1) // key < hashKey[searchIx]
      searchIxEnd = searchIx;
    else				// key > hashKey[searchIx]
      searchIxStart = searchIx+1 ;
    if( searchIxStart == searchIxEnd)
    {
	return -(searchIx + 1);
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
  
  if( nSize == nCount)
  {
    // TODO: resize (using some special functions)
  }

  if( nCount == 0)
  {
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
    DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, hashIndex)))[0];
    DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, hashIndex)))[0];
    GDLDelete( BaseGDL::interpreter->GetHeap( vID));
    GDLDelete( BaseGDL::interpreter->GetHeap( kID));
    BaseGDL::interpreter->GetHeap( vID) = value;
    BaseGDL::interpreter->GetHeap( kID) = key;
    return;
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
    
    // TODO: handle single struct parameter
    
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
    
    for( SizeT eIx=0; eIx<nEntries; ++eIx)
    {
      BaseGDL* key = e->GetKW( 2 * eIx);
      // !NULL keys are not inserted
      if( key == NULL || key == NullGDL::GetSingleInstance())
	continue;
      
      SizeT nKey = key->N_Elements();
      
      SizeT valueIx = 2 * eIx + 1;
      BaseGDL* value = e->GetKW( valueIx);
      SizeT nValue = 0;
      if( value != NULL)
	nValue = value->N_Elements();
      
      if( nValue != 0 && nKey != 1 && nValue != nKey)
	e->Throw( "Key and Value must have the same number of elements.");
      
      if( !kwNO_COPY && value != NULL)
	value = value->Dup();
      
      key = key->Dup();
      
      if( nKey == 1)
      {
	InsertIntoHashTable( hashStruct, hashTable, key, value);
      }

      
      if( kwNO_COPY)
      {
	bool stolen = e->StealLocalPar( valueIx);
	if( !stolen) e->GetPar(valueIx) = NULL;
      }
      
    }
    
    
  } 
  
} // namespace lib