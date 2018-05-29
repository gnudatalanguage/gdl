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
#include "list.hpp"

std::string ValidTagName( const std::string& in) // (performance) Function parameter 'in' should be passed by reference.
{
  if( in.length() == 0)
    return "_";
  std::string result = StrUpCase( in);
  SizeT i = 0;
  if( result[0] >= '0' && result[0] <= '9')
  {
    result = "_" + result;
    ++i;
  }
  else if( result[0] == '!')
  {
    ++i;
  }
  for( ; i<result.length(); ++i)
  {
    if( !((result[i] >= 'A' && result[i] <= 'Z') ||
	(result[i] >= '0' && result[i] <= '9') ||
	result[i] == '$'))
      result[i] = '_';
  }
  return result;
}


// if not found returns -(pos +1)
DLong HashIndex( DStructGDL* hashTable, BaseGDL* key)
{
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
  static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

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
  
// copies all keys and values  
DStructGDL* CopyHashTable( DStructGDL* hashStruct, DStructGDL* hashTable, DLong nSizeNew)
{
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
  static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");
  
//   DStructDesc* hashDesc = hashStruct->Desc();
//   DStructDesc* entryDesc = hashTable->Desc();
 
  DLong nSize = hashTable->N_Elements();//(*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0];

  assert( nSizeNew >= nCount);
  
  DStructGDL* newHashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(nSizeNew));

  // copy old table to new one, insert holes
  SizeT nAdd = 0;
  for( SizeT oldIx=0; oldIx<nSize; ++oldIx)
  {
    if( (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] == 0)
      continue;

    SizeT newIx = nAdd * nSizeNew / nCount;
    assert( newIx >= nAdd);
    
    ++nAdd;
    
    DPtr keyP = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0];
    // create new heap copy
    BaseGDL* key = BaseGDL::interpreter->GetHeap( keyP);
    assert( key != NULL);
    DPtr newKeyP = BaseGDL::interpreter->NewHeap( 1, key->Dup());
    
    (*static_cast<DPtrGDL*>(newHashTable->GetTag( pKeyTag, newIx)))[0] = newKeyP;

    
    DPtr valP = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0];
    // create new heap copy
    BaseGDL* value = BaseGDL::interpreter->GetHeap( valP);
    if( value != NULL)
      value = value->Dup();
    DPtr newValP = BaseGDL::interpreter->NewHeap( 1, value);    
    (*static_cast<DPtrGDL*>(newHashTable->GetTag( pValueTag, newIx)))[0] = newValP;    
  }

//   SizeT oldIx = 0;
//   for( SizeT nAdd=0; nAdd<nCount; ++nAdd)
//   {
//     // find next old entry
//     while( oldIx < nSize &&
//     (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] == 0) 
//       ++oldIx;
//     assert( oldIx < nSize);  
//       
//     SizeT newIx = nAdd * nSizeNew / nCount;
//     assert( newIx >= nAdd);
//     
//     DPtr keyP = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0];
//     // create new heap copy
//     BaseGDL* key = BaseGDL::interpreter->GetHeap( keyP);
//     assert( key != NULL);
//     DPtr newKeyP = BaseGDL::interpreter->NewHeap( 1, key->Dup());
//     
//     (*static_cast<DPtrGDL*>(newHashTable->GetTag( pKeyTag, newIx)))[0] = newKeyP;
// 
//     
//     DPtr valP = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0];
//     // create new heap copy
//     BaseGDL* value = BaseGDL::interpreter->GetHeap( valP);
//     if( value != NULL)
//       value = value->Dup();
//     DPtr newValP = BaseGDL::interpreter->NewHeap( 1, value);    
//     (*static_cast<DPtrGDL*>(newHashTable->GetTag( pValueTag, newIx)))[0] = newValP;
//     
//   }
  return newHashTable;
}
  
  
// keeps the keys and values 
void GrowHashTable( DStructGDL* hashStruct, DStructGDL*& hashTable, DLong nSizeNew)
{
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
  static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");
  
//   DStructDesc* hashDesc = hashStruct->Desc();
//   DStructDesc* entryDesc = hashTable->Desc();
 
  DLong nSize = hashTable->N_Elements();//(*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0];
   
  DStructGDL* newHashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(nSizeNew));

  assert( nSizeNew > nCount);
  
  // copy old table to new one, insert holes
  SizeT nAdd = 0;
  for( SizeT oldIx=0; oldIx<nSize; ++oldIx)
  {
    if( (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] == 0)
      continue;
    
    SizeT newIx = nAdd * nSizeNew / nCount; // +1 : keep bottom free
    assert( newIx >= nAdd);
    
    ++nAdd;
    
    (*static_cast<DPtrGDL*>(newHashTable->GetTag( pKeyTag, newIx)))[0] =
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0];          
    // prevent ref-count cleanup
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] = 0;

    (*static_cast<DPtrGDL*>(newHashTable->GetTag( pValueTag, newIx)))[0] = 
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0];
    // prevent ref-count cleanup
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0] = 0;
  }
  
//   SizeT oldIx = 0;
//   for( SizeT nAdd=0; nAdd<nCount; ++nAdd)
//   {
//     // find next old entry
//     while( oldIx < nSize &&
//     (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] == 0) 
//       oldIx++;
//     assert( oldIx < nSize);  
//       
//     SizeT newIx = nAdd * nSizeNew / nCount; // +1 : keep bottom free
//     assert( newIx >= nAdd);
//     
//     (*static_cast<DPtrGDL*>(newHashTable->GetTag( pKeyTag, newIx)))[0] =
//     (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0];          
//     // prevent ref-count cleanup
//     (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, oldIx)))[0] = 0;
// 
//     (*static_cast<DPtrGDL*>(newHashTable->GetTag( pValueTag, newIx)))[0] = 
//     (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0];
//     // prevent ref-count cleanup
//     (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, oldIx)))[0] = 0;
//     
//   }

  DPtr hashTableID = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
  assert( BaseGDL::interpreter->GetHeap( hashTableID) == hashTable);
  // delete old
  delete hashTable;
  // set new instead
  BaseGDL::interpreter->GetHeap( hashTableID) = newHashTable;
  // update nSize
  (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = newHashTable->N_Elements();
  // return the new table
  hashTable = newHashTable;
}
  
  
  
BaseGDL* RemoveFromHashTable( EnvUDT* e, DStructGDL* hashStruct, BaseGDL* key)
{
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
  static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

  // our current table
  DPtr thisTableID = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
  DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( thisTableID));

  DLong hashIndex = -1;
  if( key == NULL) // special case - remove random
  {
    // remove last element
    for( DLong h=hashTable->N_Elements()-1; h>=0; --h)
    {
      DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, h)))[0];
      if( kID != 0)
      {
	hashIndex = h;
	break;
      }
    }
    if( hashIndex < 0) // nothing found - ok for empty table :-)
      return NullGDL::GetSingleInstance();
//       ThrowFromInternalUDSub( e, "Internal error. Please report. Random hash index not found.");
  }
  else
  {
    hashIndex = HashIndex( hashTable, key);
    if( hashIndex < 0)
      ThrowFromInternalUDSub( e, "Key does not exist.");
  }
  
  DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, hashIndex)))[0];
  DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, hashIndex)))[0];

  BaseGDL* retValue = BaseGDL::interpreter->GetHeap( vID);
  BaseGDL::interpreter->GetHeap( vID) = NULL;
  
  BaseGDL::interpreter->FreeHeap( kID);
  BaseGDL::interpreter->FreeHeap( vID);

  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, hashIndex)))[0] = 0;
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, hashIndex)))[0] = 0;

  --((*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0]);
  return retValue;
}
  
  
// must pass hashTable as reference as it might be changed (GrowHashTable)
void InsertIntoHashTable( DStructGDL* hashStruct, DStructGDL*& hashTable, BaseGDL* key, BaseGDL* value)
{
  static DString hashName("HASH");
  static DString entryName("GDL_HASHTABLEENTRY");
  static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
  static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");
  
//   DStructDesc* hashDesc = hashStruct->Desc();
//   DStructDesc* entryDesc = hashTable->Desc();
 
//   DLong nSize = (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  DLong nSize = hashTable->N_Elements();//(*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
  assert( nSize == (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0]);
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
  
  // must be done here, otherwise hashIndex will be not in sync
  if( nSize <= (nCount * 10 / 8)) // grow on 80% occupation. TODO: find optimal value
  {
//     std::cout << "   grow table "<< i2s(nSize) << " -> " << i2s(nSize * 2) << std::endl;

    // deletes hashTable, replaces it by new one, updates nSize
    GrowHashTable( hashStruct, hashTable, nSize * 2); // grow to 50% occupation.  TODO: find optimal value
//     nSize = (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0];
    nSize = hashTable->N_Elements();
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

//   std::cout << "  nSize = "<< i2s(nSize) <<std::endl;

  // new key -> insert 
  DLong insertPos = -(hashIndex + 1);
   
//   std::cout << "   try "<< i2s(insertPos) << "... ";

  // make some space
  DLong nextFreeElementIx = insertPos;
  for( ; nextFreeElementIx < nSize; ++nextFreeElementIx)
  {
    // shuffle against top
    // insert at insertPos as old insertPos is shuffled up
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
    // shuffle against bottom
    // insert at insertPos-1 as old insertPos stays at insertPos
    --insertPos;
    nextFreeElementIx = insertPos;
    for( ; nextFreeElementIx >= 0; --nextFreeElementIx)
    {
      if( (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, nextFreeElementIx)))[0] == 0)
      {
	for( DLong i=nextFreeElementIx; i<insertPos; ++i)
	{
// 	  std::cout << i2s(i+1) << " -> " << i2s(i) << "   kID:";//std::endl;
// 	  std::cout << (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i+1)))[0]; 
// 	  std::cout << "   vID:" << (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i+1)))[0];
// 	  std::cout << std::endl;

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
  static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
  static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
  static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
  static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
  static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");
  
  SizeT nCount = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nCountTag, 0)))[0];
//   SizeT nSize = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nSizeTag, 0)))[0];

  DPtr pHashTable = (*static_cast<DPtrGDL*>( oStructGDL->GetTag( pDataTag, 0)))[0];
  DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

  DLong nSize = hashTable->N_Elements();

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
  DLong minEntries = nEntries * 2; // initial min 50% filling. TODO: find optimal value
  while( initialTableSize < minEntries) initialTableSize <<= 1; //*= 2;
  return initialTableSize;
}
 
// checks wether referenced values are equal (recursively) 
bool PtrDerefEqual( DPtrGDL* l, DPtrGDL* r)
{
  SizeT nEl = l->N_Elements();
  if( nEl != r->N_Elements())
    return false;
  for( SizeT i=0; i<nEl; ++i)
  {
    DPtr pL = (*l)[i];
    DPtr pR = (*r)[i];
    if( pL == 0 && pR == 0)
      continue;
    if( pL == 0 || pR == 0)
      return false;
    BaseGDL* derefL = BaseGDL::interpreter->GetHeap( pL);
    BaseGDL* derefR = BaseGDL::interpreter->GetHeap( pR);
    if( derefL == NullGDL::GetSingleInstance())
      derefL = NULL;
    if( derefR == NullGDL::GetSingleInstance())
      derefR = NULL;
    if( derefL == NULL && derefR == NULL)
      continue;
    if( derefL == NULL || derefR == NULL)
      return false;
    if( derefL->Type() != derefR->Type())
      return false;
    if( derefL->Type() == GDL_PTR)
    {
      // recursion here
      if( !PtrDerefEqual( static_cast<DPtrGDL*>( derefL), static_cast<DPtrGDL*>( derefR)))
	return false;
    }
    else  if( !derefL->ArrayEqual( derefR))
      return false;
  }
  return true;
}





namespace lib {
  
  
  
  BaseGDL* HASH___OverloadIsTrue( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    const int kwSELFIx = 0;

    SizeT nParam = e->NParam(1); // SELF
    
    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e);

    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];
    
    if( nCount == 0)
      return new DByteGDL(0);
    else
      return new DByteGDL(1);
  }
  
  
  
  BaseGDL* HASH___OverloadNEOp( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    SizeT nParam = e->NParam(); // number of parameters actually given
    // more precise error message
    if( nParam < 3) // consider SELF
      ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

    BaseGDL* l = e->GetKW(1);
    if( l == NullGDL::GetSingleInstance())
      l = NULL;
    
    BaseGDL* r = e->GetKW(2);
    if( r == NullGDL::GetSingleInstance())
      r = NULL;

    if( (l == NULL && r == NULL))
      ThrowFromInternalUDSub( e, "At least one parameter must be defined and a HASH.");
      
    DStructGDL* leftStruct = NULL;
    DObj leftID = 0;
    if( l != NULL && l->Type() == GDL_OBJ)
    {
      DObjGDL* left = static_cast<DObjGDL*>(l);
      leftID = (*left)[0];
      if( leftID == 0)
      { // null object -> compare to !NULL
	l = NULL;
      }
      else
      {
	try {
	  leftStruct = BaseGDL::interpreter->GetObjHeap( leftID);
	}
	catch( GDLInterpreter::HeapException& hEx)
	{
	  ThrowFromInternalUDSub( e, "Left parameter object ID <"+i2s(leftID)+"> not found.");      
	}
	if( !leftStruct->Desc()->IsParent("HASH"))	
	  leftStruct = NULL;
      }
    }
    DStructGDL* rightStruct = NULL;
    DObj rightID = 0;
    if( r != NULL && r->Type() == GDL_OBJ)
    {
      DObjGDL* right = static_cast<DObjGDL*>(r);
      rightID = (*right)[0];
      if( rightID == 0)
      { // null object -> compare to !NULL
	r = NULL;
      }
      else
      {
	try {
	  rightStruct = BaseGDL::interpreter->GetObjHeap( rightID);
	}
	catch( GDLInterpreter::HeapException& hEx)
	{
	  ThrowFromInternalUDSub( e, "Right parameter object ID <"+i2s(rightID)+"> not found.");      
	}
	if( !rightStruct->Desc()->IsParent("HASH"))	
	  rightStruct = NULL;
      }
    }

    DStructGDL* hashStruct = NULL;
    DStructGDL* hashTable = NULL;
    DStructGDL* compareStruct = NULL;
    DStructGDL* compareTable = NULL;
    BaseGDL* compare = NULL;
    if( leftStruct != NULL)
    {
      hashStruct = leftStruct;
      DPtr pHashTable = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

      if( rightStruct != NULL)
      {
	compareStruct = rightStruct;
	DPtr pHashTable = (*static_cast<DPtrGDL*>( compareStruct->GetTag( pDataTag, 0)))[0];
	compareTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));
      }
      else
      {
	compare = r;	
      }
    } 
    else if( rightStruct != NULL)
    {
      hashStruct = rightStruct;
      DPtr pHashTable = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

      compare = l;
    }
    else
      ThrowFromInternalUDSub( e, "At least one parameter must be a HASH.");

    DLong nSize = hashTable->N_Elements();
//     DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0];

    static DString listName("LIST");
    static DString cNodeName("GDL_CONTAINER_NODE");
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pListDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

    // the result list
    DStructDesc* listDesc= structDesc::LIST;
    DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;
    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);

    
    DLong nCountList = 0;
    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    for( SizeT i=0; i<nSize; ++i)
    {
      DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0];
      if( kID == 0)
	continue;

      BaseGDL* key = BaseGDL::interpreter->GetHeap( kID);
      assert( key != NULL);

      if( compareStruct == NULL) // against value
      {
	DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0];
	BaseGDL* v = BaseGDL::interpreter->GetHeap( vID);

	BaseGDL* vCmp = compare;
	
	if( v == NULL || v == NullGDL::GetSingleInstance())
	{
	  if( vCmp == NULL /*&& vCmp == NullGDL::GetSingleInstance()*/)
	    continue;
	}
	if( vCmp == NULL /*|| vCmp == NullGDL::GetSingleInstance()*/)
	{
	  if( v == NULL && v == NullGDL::GetSingleInstance())
	    continue;
	}
	if( v != NULL && vCmp != NULL)
	{
	  if( v->Type() == vCmp->Type())
	  {
	  
	    if( v->Type() == GDL_PTR)
	    {
	      if( PtrDerefEqual( static_cast<DPtrGDL*>(v), static_cast<DPtrGDL*>(vCmp)))
		continue;
	    }
	    else if( v->ArrayEqual( vCmp))
	      continue;
	  }
	}
      }
      else // against other HASH
      {
	DLong insertIx = HashIndex(compareTable, key);
	if( insertIx >= 0) // found
	{

	  DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0];
	  BaseGDL* v = BaseGDL::interpreter->GetHeap( vID);

	  DPtr vCmpID = (*static_cast<DPtrGDL*>(compareTable->GetTag( pValueTag, insertIx)))[0];
	  BaseGDL* vCmp = BaseGDL::interpreter->GetHeap( vCmpID);
	  
	  if( v == NULL || v == NullGDL::GetSingleInstance())
	  {
	    if( vCmp == NULL || vCmp == NullGDL::GetSingleInstance())
	      continue;
	  }
	  if( vCmp == NULL || vCmp == NullGDL::GetSingleInstance())
	  {
	    if( v == NULL || v == NullGDL::GetSingleInstance())
	      continue;
	  }
	  if( v != NULL && vCmp != NULL)
	  {
	    if( v->Type() == vCmp->Type())
	    {
	    
	      if( v->Type() == GDL_PTR)
	      {
		if( PtrDerefEqual( static_cast<DPtrGDL*>(v), static_cast<DPtrGDL*>(vCmp)))
		  continue;
	      }
	      else if( v->ArrayEqual( vCmp))
		continue;
	    }
	  }
	}
      }

      // not equal or not found -> insert into LIST
      DPtr dID = e->Interpreter()->NewHeap(1,key->Dup());

      cStruct = new DStructGDL( containerDesc, dimension());
      cID = e->Interpreter()->NewHeap(1,cStruct);
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pListDataTag, 0)))[0] = dID;
      
      if( cStructLast != NULL)
	(*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
      else
      { // 1st element
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
      }
	    
      cStructLast = cStruct;
      ++nCountList;
    } // for
    
    // now check other HASH (if it is a HASH)
    // add all keys not in first HASH
    if( compareStruct != NULL)
    {
      DLong nSizeCmp = compareTable->N_Elements();       
      for( SizeT i=0; i<nSizeCmp; ++i)
      {
	DPtr kID = (*static_cast<DPtrGDL*>(compareTable->GetTag( pKeyTag, i)))[0];
	if( kID == 0)
	  continue;

	BaseGDL* key = BaseGDL::interpreter->GetHeap( kID);
	assert( key != NULL);

	DLong insertIx = HashIndex(hashTable, key);
	if( insertIx >= 0) // found
	{
	  // this key was already handled (inserted or not) during the first compare
	  continue;
	}

	// not equal -> insert into LIST
	DPtr dID = e->Interpreter()->NewHeap(1,key->Dup());

	cStruct = new DStructGDL( containerDesc, dimension());
	cID = e->Interpreter()->NewHeap(1,cStruct);
	(*static_cast<DPtrGDL*>( cStruct->GetTag( pListDataTag, 0)))[0] = dID;
	
	if( cStructLast != NULL)
	  (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
	else
	{ // 1st element
	  (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
	}
	      
	cStructLast = cStruct;
	++nCountList;
      } // for
    }

    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nCountList;      

    newObjGuard.Release();
    return newObj;
  } // HASH___OverloadNEOp
  
  
  
  BaseGDL* HASH___OverloadEQOp( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    SizeT nParam = e->NParam(); // number of parameters actually given
    // more precise error message
    if( nParam < 3) // consider SELF
      ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

    BaseGDL* l = e->GetKW(1);
    if( l == NullGDL::GetSingleInstance())
      l = NULL;
    
    BaseGDL* r = e->GetKW(2);
    if( r == NullGDL::GetSingleInstance())
      r = NULL;

    if( (l == NULL && r == NULL))
      ThrowFromInternalUDSub( e, "At least one parameter must be defined and a HASH.");
      
    DStructGDL* leftStruct = NULL;
    DObj leftID = 0;
    if( l != NULL && l->Type() == GDL_OBJ)
    {
      DObjGDL* left = static_cast<DObjGDL*>(l);
      leftID = (*left)[0];
      if( leftID == 0)
      { // null object -> compare to !NULL
	l = NULL;
      }
      else
      {
	try {
	  leftStruct = BaseGDL::interpreter->GetObjHeap( leftID);
	}
	catch( GDLInterpreter::HeapException& hEx)
	{
	  ThrowFromInternalUDSub( e, "Left parameter object ID <"+i2s(leftID)+"> not found.");      
	}
	if( !leftStruct->Desc()->IsParent("HASH"))	
	  leftStruct = NULL;
      }
    }
    DStructGDL* rightStruct = NULL;
    DObj rightID = 0;
    if( r != NULL && r->Type() == GDL_OBJ)
    {
      DObjGDL* right = static_cast<DObjGDL*>(r);
      rightID = (*right)[0];
      if( rightID == 0)
      { // null object -> compare to !NULL
	r = NULL;
      }
      else
      {
	try {
	  rightStruct = BaseGDL::interpreter->GetObjHeap( rightID);
	}
	catch( GDLInterpreter::HeapException& hEx)
	{
	  ThrowFromInternalUDSub( e, "Right parameter object ID <"+i2s(rightID)+"> not found.");      
	}
	if( !rightStruct->Desc()->IsParent("HASH"))	
	  rightStruct = NULL;
      }
    }

    DStructGDL* hashStruct = NULL;
    DStructGDL* hashTable = NULL;
    DStructGDL* compareStruct = NULL;
    DStructGDL* compareTable = NULL;
    BaseGDL* compare = NULL;
    if( leftStruct != NULL)
    {
      hashStruct = leftStruct;
      DPtr pHashTable = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

      if( rightStruct != NULL)
      {
	compareStruct = rightStruct;
	DPtr pHashTable = (*static_cast<DPtrGDL*>( compareStruct->GetTag( pDataTag, 0)))[0];
	compareTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));
      }
      else
      {
	compare = r;	
      }
    } 
    else if( rightStruct != NULL)
    {
      hashStruct = rightStruct;
      DPtr pHashTable = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

      compare = l;
    }
    else
      ThrowFromInternalUDSub( e, "At least one parameter must be a HASH.");

    DLong nSize = hashTable->N_Elements();
    DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0];

    static DString listName("LIST");
    static DString cNodeName("GDL_CONTAINER_NODE");
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pListDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

    // the result list
    DStructDesc* listDesc= structDesc::LIST;
    DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;
    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);

    
    DLong nCountList = 0;
    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    for( SizeT i=0; i<nSize; ++i)
    {
      DPtr kID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0];
      if( kID == 0)
	continue;

      BaseGDL* key = BaseGDL::interpreter->GetHeap( kID);
      assert( key != NULL);

      if( compareStruct == NULL) // against value
      {
	DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0];
	BaseGDL* v = BaseGDL::interpreter->GetHeap( vID);

	BaseGDL* vCmp = compare;
	
	if( v == NULL || v == NullGDL::GetSingleInstance())
	{
	  if( vCmp != NULL /*&& vCmp != NullGDL::GetSingleInstance()*/)
	    continue;
	}
	if( vCmp == NULL /*|| vCmp == NullGDL::GetSingleInstance()*/)
	{
	  if( v != NULL && v != NullGDL::GetSingleInstance())
	    continue;
	}
	if( v != NULL)
	{
	  if( v->Type() != vCmp->Type())
	    continue;
	  
	  if( v->Type() == GDL_PTR)
	  {
	    if( !PtrDerefEqual( static_cast<DPtrGDL*>(v), static_cast<DPtrGDL*>(vCmp)))
	      continue;
	  }
	  else if( !v->ArrayEqual( vCmp))
	    continue;
	}
      }
      else // against other HASH
      {
	DLong insertIx = HashIndex(compareTable, key);
	if( insertIx < 0) // not found
	  continue;

	DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0];
	BaseGDL* v = BaseGDL::interpreter->GetHeap( vID);

	DPtr vCmpID = (*static_cast<DPtrGDL*>(compareTable->GetTag( pValueTag, insertIx)))[0];
	BaseGDL* vCmp = BaseGDL::interpreter->GetHeap( vCmpID);
	
	if( v == NULL || v == NullGDL::GetSingleInstance())
	{
	  if( vCmp != NULL && vCmp != NullGDL::GetSingleInstance())
	    continue;
	}
	if( vCmp == NULL || vCmp == NullGDL::GetSingleInstance())
	{
	  if( v != NULL && v != NullGDL::GetSingleInstance())
	    continue;
	}
	if( v != NULL)
	{
	  if( v->Type() != vCmp->Type())
	    continue;
	  
	  if( v->Type() == GDL_PTR)
	  {
	    if( !PtrDerefEqual( static_cast<DPtrGDL*>(v), static_cast<DPtrGDL*>(vCmp)))
	      continue;
	  }
	  else if( !v->ArrayEqual( vCmp))
	    continue;
	}
      }

      // equal -> insert into LIST
      DPtr dID = e->Interpreter()->NewHeap(1,key->Dup());

      cStruct = new DStructGDL( containerDesc, dimension());
      cID = e->Interpreter()->NewHeap(1,cStruct);
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pListDataTag, 0)))[0] = dID;
      
      if( cStructLast != NULL)
	(*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
      else
      { // 1st element
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
      }
	    
      cStructLast = cStruct;
      ++nCountList;
    } // for
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nCountList;      

    newObjGuard.Release();
    return newObj;
  }
  
  
  
  BaseGDL* HASH___OverloadPlus( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    SizeT nParam = e->NParam(); // number of parameters actually given
    // more precise error message
    if( nParam < 3) 
      ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

    // default behavior: Exact like scalar indexing
    BaseGDL* l = e->GetKW(1);
    if(l == NULL || (l->Type() != GDL_OBJ && l->Type() != GDL_STRUCT))
      ThrowFromInternalUDSub( e, "Left parameter must be a HASH or STRUCT.");

    BaseGDL* r = e->GetKW(2);
    if(r == NULL || (r->Type() != GDL_OBJ && r->Type() != GDL_STRUCT))
      ThrowFromInternalUDSub( e, "Right parameter must be a HASH or STRUCT.");

    // new hash
    DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
    DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
    BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
    Guard<BaseGDL> newObjGuard( newObj);

    DStructGDL* leftStruct = NULL;
    DObj leftID = 0;
    if( l->Type() == GDL_OBJ)
    {
      DObjGDL* left = static_cast<DObjGDL*>(l);
      leftID = (*left)[0];
      try {
	leftStruct = BaseGDL::interpreter->GetObjHeap( leftID);
      }
      catch( GDLInterpreter::HeapException& hEx)
      {
	ThrowFromInternalUDSub( e, "Left parameter object ID <"+i2s(leftID)+"> not found.");      
      }
      if( !leftStruct->Desc()->IsParent("HASH"))
	ThrowFromInternalUDSub( e, "Left parameter object ("+leftStruct->Desc()->Name()+") must be a HASH.");      	
    }
    else
    {
      leftStruct = static_cast<DStructGDL*>(l);
    }
    
    DStructGDL* rightStruct = NULL;
    DObj rightID = 0;
    if( r->Type() == GDL_OBJ)
    {
      DObjGDL* right = static_cast<DObjGDL*>(r);
      rightID = (*right)[0];
      try {
	rightStruct = BaseGDL::interpreter->GetObjHeap( rightID);
      }
      catch( GDLInterpreter::HeapException& hEx)
      {
	ThrowFromInternalUDSub( e, "Right parameter object ID <"+i2s(rightID)+"> not found.");      
      }
      if( !rightStruct->Desc()->IsParent("HASH"))
	ThrowFromInternalUDSub( e, "Right parameter object ("+rightStruct->Desc()->Name()+") must be a HASH.");      	
    }
    else
    {
      rightStruct = static_cast<DStructGDL*>(r);
    }
    
    if( leftID != 0 && rightID != 0)
    {
      // merge sort them together
      DLong nCountL = (*static_cast<DLongGDL*>(leftStruct->GetTag( nCountTag, 0)))[0];
      DLong nCountR = (*static_cast<DLongGDL*>(rightStruct->GetTag( nCountTag, 0)))[0];

      DPtr pHashTableL = (*static_cast<DPtrGDL*>( leftStruct->GetTag( pDataTag, 0)))[0];
      DStructGDL* hashTableL = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTableL));
      DPtr pHashTableR = (*static_cast<DPtrGDL*>( rightStruct->GetTag( pDataTag, 0)))[0];
      DStructGDL* hashTableR = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTableR));

      DLong nSizeL = hashTableL->N_Elements();
      DLong nSizeR = hashTableR->N_Elements();
      
      DLong nCountMax = nCountL + nCountR;
      
      // new hash table
      DLong initialTableSize = GetInitialTableSize( nCountMax);
      DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;

      DLong nSize = hashTable->N_Elements();

      assert( nSize >= nCountMax);

      if( nCountMax == 0) // two empty HASH
      {
	(*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = nSize;
	// nCount was set to zero at init
	newObjGuard.Release();
	return newObj;
      }
      
      DLong leftIx = -1;
      DLong rightIx = -1;
      // advance both to 1st
      if( nCountL > 0)
	while( (*static_cast<DPtrGDL*>(hashTableL->GetTag( pKeyTag, ++leftIx)))[0] == 0);
      if( leftIx == -1)
	leftIx = nSizeL;
      if( nCountR > 0)
	while( (*static_cast<DPtrGDL*>(hashTableR->GetTag( pKeyTag, ++rightIx)))[0] == 0);
      if( rightIx == -1)
	rightIx = nSizeR;
      
      DLong nCount = nCountMax;
      for( SizeT el=0; el<nCount; ++el)
      {
	int hashCompare;
	BaseGDL* keyL;
	BaseGDL* keyR;
	if( leftIx < nSizeL)
	{
	  DPtr kIDL = (*static_cast<DPtrGDL*>(hashTableL->GetTag( pKeyTag, leftIx)))[0];
	  keyL = BaseGDL::interpreter->GetHeap( kIDL);
	  assert( keyL != NULL);

	  if( rightIx < nSizeR)
	  {
	    DPtr kIDR = (*static_cast<DPtrGDL*>(hashTableR->GetTag( pKeyTag, rightIx)))[0];
	    keyR = BaseGDL::interpreter->GetHeap( kIDR);
	    assert( keyR != NULL);

	    // both valid -> compare
	    hashCompare = keyL->HashCompare( keyR);
	    if( hashCompare == 0)
	      --nCount;
	  }
	  else
	  {
	    // right finish -> use left
	    hashCompare = -1;
	  }
	}
	else
	{
	  // left finish -> use right
	  if( rightIx >= nSizeR)
	    assert( rightIx < nSizeR);

	  DPtr kIDR = (*static_cast<DPtrGDL*>(hashTableR->GetTag( pKeyTag, rightIx)))[0];
	  keyR = BaseGDL::interpreter->GetHeap( kIDR);
	  assert( keyR != NULL);

	  hashCompare = 1;
	}

	DLong insertIx = el * nSize / nCountMax;	
	if( hashCompare == -1) // keyL smaller -> use left
	{
	    DPtr kID = BaseGDL::interpreter->NewHeap(1,keyL->Dup());
	    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertIx)))[0] = kID;

	    DPtr vSrcID = (*static_cast<DPtrGDL*>(hashTableL->GetTag( pValueTag, leftIx)))[0];
	    BaseGDL* value = BaseGDL::interpreter->GetHeap( vSrcID);
	    if( value != NULL) 
	      value = value->Dup();
	    
	    DPtr vID = BaseGDL::interpreter->NewHeap(1,value);
	    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertIx)))[0] = vID;

	    // advance l
	    while( (++leftIx < nSizeL) && (*static_cast<DPtrGDL*>(hashTableL->GetTag( pKeyTag, leftIx)))[0] == 0);
	}
	else // keyL larger or equal -> use right
	{
	    DPtr kID = BaseGDL::interpreter->NewHeap(1,keyR->Dup());
	    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertIx)))[0] = kID;

	    DPtr vSrcID = (*static_cast<DPtrGDL*>(hashTableR->GetTag( pValueTag, rightIx)))[0];
	    BaseGDL* value = BaseGDL::interpreter->GetHeap( vSrcID);
	    if( value != NULL) 
	      value = value->Dup();
	    
	    DPtr vID = BaseGDL::interpreter->NewHeap(1,value);
	    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertIx)))[0] = vID;

	    // advance r
	    while( (++rightIx < nSizeR) && (*static_cast<DPtrGDL*>(hashTableR->GetTag( pKeyTag, rightIx)))[0] == 0);
	    
	    if( hashCompare == 0) // advance r also if equal keys
	      while( (++leftIx < nSizeL) && (*static_cast<DPtrGDL*>(hashTableL->GetTag( pKeyTag, leftIx)))[0] == 0);
	}
      }

      (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = nSize;     
      (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = nCount;     
      
      newObjGuard.Release();
      return newObj;
    }

    DStructGDL* newHashTable;

      // at least one is struct
    if( leftID != 0) // left is HASH
    {
      DLong nCountL = (*static_cast<DLongGDL*>(leftStruct->GetTag( nCountTag, 0)))[0];

      DPtr pHashTableL = (*static_cast<DPtrGDL*>( leftStruct->GetTag( pDataTag, 0)))[0];
      DStructGDL* hashTableL = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTableL));

      // right must be struct
      DLong nCountR = rightStruct->NTags();
      
      DLong nCount = nCountL + nCountR;
      
      DLong initialTableSize = GetInitialTableSize( nCount);
      newHashTable = CopyHashTable( leftStruct, hashTableL, initialTableSize);
      DPtr hashTableID= e->NewHeap( 1, newHashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;
      (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = nCountL;
    }
    else if( rightID != 0) // right is HASH
    {
      DLong nCountR = (*static_cast<DLongGDL*>(rightStruct->GetTag( nCountTag, 0)))[0];

      DPtr pHashTableR = (*static_cast<DPtrGDL*>( rightStruct->GetTag( pDataTag, 0)))[0];
      DStructGDL* hashTableR = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTableR));

      // right must be struct
      DLong nCountL = leftStruct->NTags();
      
      DLong nCount = nCountL + nCountR;
      
      DLong initialTableSize = GetInitialTableSize( nCount);
      newHashTable = CopyHashTable( rightStruct, hashTableR, initialTableSize);
      DPtr hashTableID= e->NewHeap( 1, newHashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;
      (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = nCountR;
    }
    else // both are struct
    {
      DLong nCountL = leftStruct->NTags();
      DLong nCountR = rightStruct->NTags();
      DLong nCount = nCountL + nCountR;
      DLong initialTableSize = GetInitialTableSize( nCount);
      newHashTable = new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, newHashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;
      // set to zero at init //(*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = 0;      
    }
    (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = newHashTable->N_Elements();

    if( leftID == 0) // add left struct
    {
	DStructDesc* desc = leftStruct->Desc();
	for( SizeT t=0; t<desc->NTags(); ++t)
	{
	  DStringGDL *structKey = new DStringGDL( desc->TagName(t));
	  BaseGDL* structData = leftStruct->GetTag(t,0);
	  assert(structData != NULL);
	  structData = structData->Dup();
	  
	  InsertIntoHashTable( hashStruct, newHashTable, structKey, structData);
	}

    }
    if( rightID == 0) // add right struct
    {
	DStructDesc* desc = rightStruct->Desc();
	for( SizeT t=0; t<desc->NTags(); ++t)
	{
	  DStringGDL *structKey = new DStringGDL( desc->TagName(t));
	  BaseGDL* structData = rightStruct->GetTag(t,0);
	  assert(structData != NULL);
	  structData = structData->Dup();
	  
	  InsertIntoHashTable( hashStruct, newHashTable, structKey, structData);
	}

    }
    
    newObjGuard.Release();
    return newObj;
  }
  
  
  BaseGDL* hash__tostruct( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    const int kwMISSINGIx = 0; // pushed 2nd
    const int kwSKIPPEDIx = 1;
    const int kwSELFIx = 2;

    SizeT nParam = e->NParam(1); // SELF
    
    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e);

    DPtr pHashTable = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
    DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

//     DLong nSize = (*static_cast<DLongGDL*>( self->GetTag( nSizeTag, 0)))[0];
    DLong nSize = hashTable->N_Elements();
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];

    BaseGDL* missing = e->GetKW( kwMISSINGIx);
    BaseGDL** skipped = NULL;
    if( e->GlobalKW( kwSKIPPEDIx))
      skipped = &e->GetKW(kwSKIPPEDIx);
    
    static DString listName("LIST");
    static DString cNodeName("GDL_CONTAINER_NODE");
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pListDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
   
    DStructDesc* listDesc= structDesc::LIST;
    DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;

    DStructGDL* listStruct = NULL;
    BaseGDL* newObj = NULL;
    Guard<BaseGDL> newListObjGuard;
    if( skipped != NULL)
    {
      listStruct= new DStructGDL( listDesc, dimension());
      DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
      newObj = new DObjGDL( objID); // the list object
      newListObjGuard.Init( newObj);
    }
    
    DStructDesc* nStructDesc = new DStructDesc( "$truct");
    // instance takes care of nStructDesc since it is unnamed
    // 	dimension dim( 1);
    // 	DStructGDL* instance = new DStructGDL( nStructDesc, dim);
    DStructGDL* instance = new DStructGDL( nStructDesc);
    Guard<DStructGDL> instance_guard(instance);

    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    
    for( SizeT el=0; el<nSize; ++el)
    {
      DPtr pKey = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, el)))[0];
      if( pKey == 0)
	continue;

      DPtr pValue = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, el)))[0];
      assert( pValue != 0);
      
      BaseGDL* key = BaseGDL::interpreter->GetHeap( pKey);
      assert( key != NULL);

      BaseGDL* value = BaseGDL::interpreter->GetHeap( pValue);
      if( value == NULL || value == NullGDL::GetSingleInstance())
      {
	value = missing;
      }
      // we are not owner of value here
      
      bool added = false;
      if( key->Type() == GDL_STRING && value != NULL)
      {
	assert( key->N_Elements() == 1);
	DString keyString = (*static_cast<DStringGDL*>(key))[0];
	
	DString tagString = ValidTagName( keyString);
	
	if( nStructDesc->TagIndex( tagString) == -1)
	{
          instance->NewTag( tagString, value->Dup());
	  added = true;
	}	
      }
      
      if( !added && listStruct != NULL) // add key to skipped
      {
	key = key->Dup();
	DPtr dID = e->Interpreter()->NewHeap(1,key);

	cStruct = new DStructGDL( containerDesc, dimension());
	cID = e->Interpreter()->NewHeap(1,cStruct);
	(*static_cast<DPtrGDL*>( cStruct->GetTag( pListDataTag, 0)))[0] = dID;
      
	if( cStructLast != NULL)
	  (*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
	else
	{ // 1st element
	  (*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
	}
	    
      cStructLast = cStruct;
      }
    }

    if( skipped != NULL)
    {
      GDLDelete( *skipped);
      newListObjGuard.Release();
      *skipped = newObj;
    }

    if( instance->NTags() == 0)
      return NullGDL::GetSingleInstance();
    
    instance_guard.Release();
    return instance;
  }

  BaseGDL* hash__isempty( EnvUDT* e)
  {
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    const int kwSELFIx = 0;
    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e); // checks
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];
    if (nCount > 0) return new DByteGDL( 0); else return new DByteGDL(1);
  }


  SizeT HASH_count( DStructGDL* hash)
  {
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    return (*static_cast<DLongGDL*>( hash->GetTag( TableCountTag, 0)))[0];	      
  }
  BaseGDL* hash__count( EnvUDT* e)
  {
    const int kwSELFIx = 0;
    const int kwVALUEIx = 1;
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

    SizeT nParam = e->NParam(1); // SELF

    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e); // checks

    DObjGDL* selfObj = static_cast<DObjGDL*>(selfP);
    
    if( nParam > 1)
    {
      BaseGDL* r = e->GetKW( kwVALUEIx);
    
      DObjGDL* listObj = static_cast<DObjGDL*>( selfObj->EqOp( r));
      Guard<DObjGDL> listObjGuard( listObj);
    
      DStructGDL* selfLIST = GetSELF( listObj, e);

      DLong nList = (*static_cast<DLongGDL*>( selfLIST->GetTag( nListTag, 0)))[0];	      

      return new DLongGDL( nList);
    }
    
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];	      
    return new DLongGDL( nCount);
  }
  
  
  BaseGDL* hash__where( EnvUDT* e)
  {
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

    const int kwNCOMPLEMENTIx = 0; 
    const int kwCOUNTIx = 1; 
    const int kwCOMPLEMENTIx = 2; 
    const int kwSELFIx = 3;
    const int kwVALUEIx = 4;

    SizeT nParam = e->NParam(2); // SELF, VALUE

    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e); // checks

    DObjGDL* selfObj = static_cast<DObjGDL*>(selfP);
    
    BaseGDL* r = e->GetKW( kwVALUEIx);
    
    DObjGDL* listObj = static_cast<DObjGDL*>( selfObj->EqOp( r));
    Guard<DObjGDL> listObjGuard( listObj);

    DStructGDL* selfLIST = GetSELF( listObj, e);
    DLong nList = (*static_cast<DLongGDL*>( selfLIST->GetTag( nListTag, 0)))[0];	      

    if( e->KeywordPresent( kwNCOMPLEMENTIx)) // NCOMPLEMENT
    {
	DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];
	e->SetKW( kwNCOMPLEMENTIx, new DLongGDL( nCount - nList));
    }
    if( e->KeywordPresent( kwCOUNTIx)) // COUNT
    {
	e->SetKW( kwCOUNTIx, new DLongGDL( nList));
    }
    if( e->KeywordPresent( kwCOMPLEMENTIx)) // COMPLEMENT
    {
	DObjGDL* compObj = static_cast<DObjGDL*>( selfObj->NeOp( r));
	e->SetKW( kwCOMPLEMENTIx, compObj);
    }
    
    listObjGuard.Release();
    return listObj;
  }
  
  
  BaseGDL* hash__keysvalues( EnvUDT* e, bool keys);

  BaseGDL* hash__values( EnvUDT* e)
  {
    return hash__keysvalues( e, false);
  }
  BaseGDL* hash__keys( EnvUDT* e)
  {
    return hash__keysvalues( e, true);
  }
  BaseGDL* hash__keysvalues( EnvUDT* e, bool doKeys)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    const int kwSELFIx = 0;

    SizeT nParam = e->NParam(1); // SELF

    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e);

//     DLong nSize = (*static_cast<DLongGDL*>( self->GetTag( nSizeTag, 0)))[0];
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];

    static DString listName("LIST");
    static DString cNodeName("GDL_CONTAINER_NODE");
    static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");
    static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");
    static unsigned pListDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");
   
    DStructDesc* listDesc= structDesc::LIST;
    DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;
  
    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);

    DPtr pHashTable = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
    DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( pHashTable));

    DLong nSize = hashTable->N_Elements();

    unsigned sourceTag;
    if( doKeys)
    {
      sourceTag = pKeyTag;
    }
    else // do values
    {
      sourceTag = pValueTag;
    }

    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    SizeT ix = 0;
    for( SizeT i=0; i<nCount; ++i, ++ix)
    {
      while( ix < nSize &&
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, ix)))[0] == 0) 
	++ix;
      assert( ix < nSize);  
      
      DPtr pSource = (*static_cast<DPtrGDL*>(hashTable->GetTag( sourceTag, ix)))[0];
      BaseGDL* source = BaseGDL::interpreter->GetHeap( pSource);
      assert( !doKeys || source != NULL);    
      
      if( source != NULL) // NULL is ok for values
	source = source->Dup();
      DPtr dID = e->Interpreter()->NewHeap(1,source);

      cStruct = new DStructGDL( containerDesc, dimension());
      cID = e->Interpreter()->NewHeap(1,cStruct);
      (*static_cast<DPtrGDL*>( cStruct->GetTag( pListDataTag, 0)))[0] = dID;
      
      if( cStructLast != NULL)
	(*static_cast<DPtrGDL*>( cStructLast->GetTag( pNextTag, 0)))[0] = cID;
      else
      { // 1st element
	(*static_cast<DPtrGDL*>( listStruct->GetTag( pTailTag, 0)))[0] = cID;	      
      }
	    
      cStructLast = cStruct;
    }
  
    (*static_cast<DPtrGDL*>( listStruct->GetTag( pHeadTag, 0)))[0] = cID;	      
    (*static_cast<DLongGDL*>( listStruct->GetTag( nListTag, 0)))[0] = nCount;      

    newObjGuard.Release();
    return newObj;
  }  
    
  BaseGDL* hash__haskey( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    // see overload.cpp
    const int kwSELFIx = 0;
    const int kwKEYIx = 1;

    SizeT nParam = e->NParam(2); // SELF, KEYLIST

    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e);
   
    BaseGDL* keyList = e->GetKW( kwKEYIx);
    if( keyList == NULL || keyList == NullGDL::GetSingleInstance())
      ThrowFromInternalUDSub( e, "Key must be a scalar string or number.");
    if( keyList->Type() != GDL_STRING && !NumericType(keyList->Type()))
      ThrowFromInternalUDSub( e, "Key must be a scalar string or number.");

    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
    DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));

    if( keyList->N_Elements() == 1)
    {
      DLong hashIndex = HashIndex( thisHashTable, keyList);
      if( hashIndex >= 0)
	return new DLongGDL( 1);
      return new DLongGDL( 0);
    }
    
    SizeT keyListN_Elements = keyList->N_Elements();
    DIntGDL* result = new DIntGDL( dimension(keyListN_Elements)); // zero
    Guard<BaseGDL> resultGuard( result);
    for( SizeT i=0; i<keyListN_Elements; ++i)
    {
	BaseGDL* key = keyList->NewIx( i);
	Guard<BaseGDL> keyGuard( key);
	DLong hashIndex = HashIndex( thisHashTable, key);
	if( hashIndex >= 0)
	  (*result)[ i] = 1;
    }
    resultGuard.Release();
    return result;
  }

  
  BaseGDL* hash__remove( EnvUDT* e, bool asFunction);

  BaseGDL* hash__remove_fun( EnvUDT* e)
  {
    return hash__remove( e, true);
  }
  void hash__remove_pro( EnvUDT* e)
  {
    hash__remove( e, false);
  }

  BaseGDL* hash__remove( EnvUDT* e, bool asFunction)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    // see overload.cpp
    const int kwALLIx = 0; 
    const int kwSELFIx = 1;
    const int kwINDEXIx = 2;

    bool kwALL = false;
    if (e->KeywordSet(kwALLIx)){ kwALL = true;}

    SizeT nParam = e->NParam(1); // minimum SELF

    BaseGDL* selfP = e->GetKW( kwSELFIx);
    DStructGDL* self = GetSELF( selfP, e);

    if( kwALL)
    {
      if( asFunction)
      {
// 	DLong nSize = (*static_cast<DLongGDL*>( self->GetTag( nSizeTag, 0)))[0];
	DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];

	DLong initialTableSize = GetInitialTableSize( 0);

	// our current table
	DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
  	DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));

	DLong nSize = thisHashTable->N_Elements();

	// new hash
	DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
	DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
	BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
	Guard<BaseGDL> newObjGuard( newObj);

	// our new hash table
	DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
	DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 

	// set our table to new empty table
	(*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0] = hashTableID;
	(*static_cast<DLongGDL*>( self->GetTag( nSizeTag, 0)))[0] = initialTableSize;
	(*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0] = 0;
	
	// set our old table to new HASH
	(*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = thisTableID;
	(*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = nSize;
	(*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = nCount;
	
	newObjGuard.Release();
	return newObj;    
      }
      else
      {
	DLong initialTableSize = GetInitialTableSize( 0);

	// our current table
	DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
  	DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));

	// our new hash table
	DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
	DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 

	// set our table to new empty table
	(*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0] = hashTableID;
	(*static_cast<DLongGDL*>( self->GetTag( nSizeTag, 0)))[0] = initialTableSize;
	(*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0] = 0;
	
	// trigger ref-count delete of all elements      
	BaseGDL::interpreter->FreeHeap( thisTableID); 
	
	return NULL;      
      }
    }

    BaseGDL* index = NULL;
    if( nParam >= 2)
      index = e->GetKW(kwINDEXIx);
    
    if( index == NULL)
    {
      BaseGDL* removedElement = RemoveFromHashTable( e, self, NULL);
      if( !asFunction)
      {
	  GDLDelete( removedElement);
	  removedElement = NULL;
      }
      return removedElement;
    }
    
    if( index->N_Elements() == 1)
    {
      BaseGDL* removedElement = RemoveFromHashTable( e, self, index);
      if( !asFunction)
      {
	  GDLDelete( removedElement);
	  removedElement = NULL;
      }
      return removedElement;
    }
   
    if( asFunction)
    {
      // new hash
      DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
      DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
      BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
      Guard<BaseGDL> newObjGuard( newObj);

      SizeT nRemove = index->N_Elements();

      DLong initialTableSize = GetInitialTableSize( nRemove);

      // new hash table
      // our current table (for the descriptor)
//       DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
//       DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));
      DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pDataTag, 0)))[0] = hashTableID;

      (*static_cast<DLongGDL*>( hashStruct->GetTag( nSizeTag, 0)))[0] = initialTableSize;
//       (*static_cast<DLongGDL*>( hashStruct->GetTag( nCountTag, 0)))[0] = 0; // already init to zero

      for( SizeT r=0; r<nRemove; ++r)
      {
	BaseGDL* removeKey = index->NewIx( r);
	Guard<BaseGDL> removeKeyGuard( removeKey);
	BaseGDL* removedElement = RemoveFromHashTable( e, self, removeKey);      
	InsertIntoHashTable( hashStruct, hashTable, removeKeyGuard.release(), removedElement);    
      }

      newObjGuard.Release();
      return newObj;
    }    
    else
    {    
      SizeT nRemove = index->N_Elements();
      for( SizeT r=0; r<nRemove; ++r)
      {
	BaseGDL* removeKey = index->NewIx( r);
	Guard<BaseGDL> removeKeyGuard( removeKey);
	BaseGDL* removedElement = RemoveFromHashTable( e, self, removeKey);      
	GDLDelete( removedElement);
      }
      return NULL;
    }
  }
  
  void HASH___OverloadBracketsLeftSide( EnvUDT* e)
  {
    // SELF
    //->AddPar("OBJREF")->AddPar("RVALUE")->AddPar("ISRANGE");
    //->AddPar("SUB1")->AddPar("SUB2")->AddPar("SUB3")->AddPar("SUB4");
    //->AddPar("SUB5")->AddPar("SUB6")->AddPar("SUB7")->AddPar("SUB8");
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    const unsigned par1Ix = 4;
    
    
    SizeT nParam = e->NParam(1); // number of parameters actually given
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
    DStructGDL* self = GetSELF( selfP, e);

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
    
    BaseGDL* parX = e->GetKW( par1Ix); // implicit SELF, ISRANGE, par1..par8
    if( parX == NULL)
      ThrowFromInternalUDSub( e, "Parameter is undefined: "  + e->Caller()->GetString(e->GetKW( par1Ix)));

    DLong isRangeX = (*isRangeLong)[0];
    if( isRangeX != 0 && isRangeX != 1)
    {
//       if( (isRangeX == 2 || isRangeX == 3) && rValue == NullGDL::GetSingleInstance())
//       {
// 	dotAccess = true;
// 	isRangeX -= 2;
//       }
//       else
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
	i2s((*parXLong)[0])+":"+	i2s((*parXLong)[1])+":"+i2s((*parXLong)[2])+"]");
	
      ThrowFromInternalUDSub( e, "Due to compatibility, setting all [*] to one value is not allowed. "
      "Please report if you would appreciate this functionality.");

      // full access [*]
    }

    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pDataTag, 0)))[0];
    DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));

    // non-range (keyed)
    SizeT par1N_Elements = parX->N_Elements();

    if( par1N_Elements == 1) // single key
    {
	if( dotAccess) // -> objRef is NULL (or !NULL)
	{
	  if( rValue != NullGDL::GetSingleInstance())
	  {
	    ThrowFromInternalUDSub( e, "For struct access (OBJREF is !NULL), RVALUE must be !NULL as well.");      
	  }

	  DLong hashIndex = HashIndex( thisHashTable, parX);
	  if( hashIndex < 0)
	    ThrowFromInternalUDSub( e, "Key not found.");

	  *objRef = thisHashTable->GetTag( pValueTag, hashIndex)->Dup();
	  return;
	}    
	
	bool stolen = e->StealLocalKW( par1Ix);
	if( !stolen) parX = parX->Dup(); // if called explicitely
	InsertIntoHashTable( self, thisHashTable, parX, rValue->Dup());
	return;
    }
    if( dotAccess)
    {
      ThrowFromInternalUDSub( e, "Only single value struct access is allowed.");
    }

    if( rValue != NULL && rValue != NullGDL::GetSingleInstance())
    {
      if( rValue->N_Elements() != par1N_Elements)
      {
	ThrowFromInternalUDSub( e, "Key and Value must have the same number of elements.");
      }
    }
    
    if( rValue != NULL && rValue != NullGDL::GetSingleInstance())
    {
      for( SizeT k=0; k<par1N_Elements; ++k)
      {    
	InsertIntoHashTable( self, thisHashTable, parX->NewIx(k), rValue->NewIx(k));    
      }
    }
    else
    {
      for( SizeT k=0; k<par1N_Elements; ++k)
      {    
	InsertIntoHashTable( self, thisHashTable, parX->NewIx(k), NULL);    
      }    
    }
  }

  
  
  BaseGDL* HASH___OverloadBracketsRightSide( EnvUDT* e)
  {
    static DString hashName("HASH");
    static DString entryName("GDL_HASHTABLEENTRY");
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    const unsigned par1Ix = 2;
    
    SizeT nParam = e->NParam(1); // number of parameters actually given
  //   int envSize = e->EnvSize(); // number of parameters + keywords 'e' (pro) has defined
    if( nParam < 3) // consider implicit SELF
      ThrowFromInternalUDSub( e, "Two parameters are needed: ISRANGE, SUB1 [, ...].");
    if( nParam > 3) // consider implicit SELF
      ThrowFromInternalUDSub( e, "Only one dimensional access allowed.");

    BaseGDL* selfP = e->GetKW( 0);
//     if( selfP->Type() != GDL_OBJ)
//       ThrowFromInternalUDSub( e, "SELF is not of type OBJECT.");
//     if( !selfP->Scalar())
//       ThrowFromInternalUDSub( e, "SELF must be a scalar OBJECT.");
// 
//     DObjGDL* selfObj = static_cast<DObjGDL*>( selfP);
//     DObj selfID = (*selfObj)[0];
//     DStructGDL* self = e->Interpreter()->GetObjHeap( selfID);
    DStructGDL* self = GetSELF( selfP, e);
    
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
	i2s((*parXLong)[0])+":"+i2s((*parXLong)[1])+":"+i2s((*parXLong)[2])+"]");
      
      // full range -> clone ===================================================
      DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( nCountTag, 0)))[0];
      SizeT nEntries = nCount;
    
      DLong initialTableSize = GetInitialTableSize( nEntries);
    
      // new hash
      DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
      DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
      BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
      Guard<BaseGDL> newObjGuard( newObj);
      // the return hash table
      DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
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
    DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
    DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
    BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
    Guard<BaseGDL> newObjGuard( newObj);
    // the return hash table
    DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
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
    static unsigned pDataTag = structDesc::HASH->TagIndex( "TABLE_DATA");
    static unsigned nSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE");
    static unsigned nCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY");
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* hashDesc=structDesc::HASH;
    DStructDesc* entryDesc=structDesc::GDL_HASHTABLEENTRY;
    assert( hashDesc != NULL && hashDesc->NTags() > 0);
    assert( entryDesc != NULL && entryDesc->NTags() > 0);

    DStructGDL* hashStruct= new DStructGDL( hashDesc, dimension());

    DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 

    BaseGDL* newObj = new DObjGDL( objID); // the hash object
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
// 	  DString validName = ValidTagName( desc->TagName(t));
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
