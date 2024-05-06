/***************************************************************************
                          hash.cpp  - for HASH objects
                             -------------------
    begin                : July 22 2013
    copyright            : (C) 2013 by M. Schellens et al.
    email                : m_schellens@users.sf.net
* 
*   April 11 2016: Greg Jung 
*   - implemented EXTRACT, FOLD_CASE. 
*   - hash__tostruct set up for RECURSIVE, NO_COPY implementations.
*   -   ORDEREDHASH placeholder routines, flags only.
*   - [ and ] overloads revision to accomodate in-place array access
*  is implemented in list.cpp - same logic overhaul can be applied here.
* 
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#define GDL_LIST_STRUCT()           \
  static unsigned pHeadTag = structDesc::LIST->TagIndex( "PHEAD");  \
  static unsigned pTailTag = structDesc::LIST->TagIndex( "PTAIL");  \
  static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

#define GDL_CONTAINER_NODE()            \
    static unsigned pNextTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PNEXT");  \
    static unsigned pListDataTag = structDesc::GDL_CONTAINER_NODE->TagIndex( "PDATA");

#define GDL_HASH_STRUCT()           \
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS"); \
    static unsigned pTableTag = structDesc::HASH->TagIndex( "TABLE_DATA"); \
    static unsigned TableSizeTag = structDesc::HASH->TagIndex( "TABLE_SIZE"); \
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");

#define GDL_HASHTABLEENTRY()            \
    static unsigned pKeyTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PKEY"); \
    static unsigned pValueTag = structDesc::GDL_HASHTABLEENTRY->TagIndex( "PVALUE");

#define MAKE_LONGGDL(X, XLong) \
        DLongGDL* XLong=0; \
        Guard<DLongGDL> XLongGuard; \
        if( X != 0) { \
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
          } \
        }
    
#include "dinterpreter.hpp"


static bool trace_me(false);
 
namespace lib {
//  bool trace_arg();
 
  void help_item( std::ostream& os,
          BaseGDL* par, DString parString, bool doIndentation);
//  void help_struct( std::ostream& os,  BaseGDL* par, int indent , bool debug );
  SizeT HASH_count( DStructGDL* hash)
  {
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    return (*static_cast<DLongGDL*>( hash->GetTag( TableCountTag, 0)))[0];        
  }
} 

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
static  DStructGDL* GetLISTStruct( EnvUDT* e, DPtr actP) // (copied from list.cpp)
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

static DPtr GetLISTNode( EnvUDT* e, DStructGDL* self, DLong targetIx)
  {
      
    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()
      
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

static BaseGDL* GetNodeData(DPtr &Node) 
{
    GDL_CONTAINER_NODE()

    DStructGDL* act = GetLISTStruct( NULL, Node);
    DPtr ptrX = (*static_cast<DPtrGDL*>(act->GetTag( pListDataTag, 0)))[0];
    Node = (*static_cast<DPtrGDL*>(act->GetTag( pNextTag, 0)))[0];
    BaseGDL* result = new BaseGDL( );
    Guard<BaseGDL> resultGuard( result);
    result =  BaseGDL::interpreter->GetHeap( ptrX);
    if( result == NULL) result = NullGDL::GetSingleInstance();
//  if(trace_me) lib::help_item(std::cout, result, " from GetNodeData", false);
    resultGuard.Release();
    return result;
}

static  BaseGDL* hash_create( EnvT* e, bool isordered );
static BaseGDL* structP_tohash( EnvT* e,BaseGDL* par,
                        bool foldcasekw, bool extractkw, bool isordered );

static DStructGDL* GetOBJ( BaseGDL* Objptr, EnvUDT* e)
  {
    if( Objptr == NULL || Objptr->Type() != GDL_OBJ) {
        if( e == NULL)
              throw GDLException( "Objptr not of type OBJECT. Please report.");
        else ThrowFromInternalUDSub( e, "Objptr not of type OBJECT. Please report.");
        }
    if( !Objptr->Scalar()) {
        if( e == NULL)
              throw GDLException(  "Objptr must be a scalar. Please report.");
        else ThrowFromInternalUDSub( e, "Objptr must be a scalar. Please report.");
    }
    DObjGDL* Object = static_cast<DObjGDL*>( Objptr);
    DObj ID = (*Object)[0];
    try {
      return BaseGDL::interpreter->GetObjHeap( ID);
    }
    catch( GDLInterpreter::HeapException& hEx)
    {
        if( e == NULL)
              throw GDLException(  "Object ID <"+i2s(ID)+"> not found.");
        else ThrowFromInternalUDSub( e, "Object ID <"+i2s(ID)+"> not found.");      
    }

    assert(false);
    return NULL;
  }

// if not found returns -(pos +1)
DLong HashIndex( DStructGDL* hashTable, BaseGDL* key, bool isfoldcase=false)
{
    GDL_HASHTABLEENTRY()
  assert( key != NULL && key != NullGDL::GetSingleInstance());
  
  DLong searchIxStart = 0;
  DLong searchIxEnd = hashTable->N_Elements();
  bool dofoldcase = isfoldcase;
  if( key->Type() != GDL_STRING) dofoldcase = false;
    if(trace_me) std::cout << ". ";

  BaseGDL* keyfind = key;
  if(dofoldcase) 
    {       // this code bombs if key is not a string.
    DString keyval = (*static_cast<DStringGDL*>(key))[0];
    std::transform(keyval.begin(), keyval.end(),
                            keyval.begin(), ::tolower);
    keyfind = static_cast<BaseGDL*>(new DStringGDL(keyval));
      }
    if(trace_me) std::cout << ". ";
  
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
    BaseGDL* candidate = BaseGDL::interpreter->GetHeap( kID);
    if( dofoldcase  and  candidate->Type() == GDL_STRING) 
      {
          DString keyval = (*static_cast<DStringGDL*>(candidate))[0];
            std::transform(keyval.begin(), keyval.end(),
                            keyval.begin(), ::tolower);
        candidate = static_cast<BaseGDL*>(new DStringGDL(keyval));
          }

    int hashCompare = keyfind->HashCompare( candidate);
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
  

bool Hashisfoldcase( DStructGDL* hashStruct)
{
    static unsigned fold_case_mask = 0x00000001;
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS");
    if(hashStruct == NULL) return false;
    DLong bits = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableBitsTag , 0)))[0];
//    if(trace_me) std::cout << "isfold: bits=" << bits << std::endl;
    if ( (bits & fold_case_mask) == 0) return false; else return true;
    }  
static bool Hashisordered( DStructGDL* hashStruct)
{
    static unsigned ordmask = 0x00000010;
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS");
    if(hashStruct == NULL) return false;
    DLong bits = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableBitsTag , 0)))[0];
    if ( (bits & ordmask) == 0) return false; else return true;
    }  
// copies all keys and values  
DStructGDL* CopyHashTable( DStructGDL* hashStruct, DStructGDL* hashTable, DLong nSizeNew)
{
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()
  
    if(hashStruct == NULL) return NULL;
  DLong nSize = hashTable->N_Elements();
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0];

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
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()
  
    if(hashStruct == NULL) return;
  DLong nSize = hashTable->N_Elements();
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0];
   
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

  DPtr hashTableID = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0];
  assert( BaseGDL::interpreter->GetHeap( hashTableID) == hashTable);
  // delete old
  delete hashTable;
  // set new instead
  BaseGDL::interpreter->GetHeap( hashTableID) = newHashTable;
  // update nSize
  (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = newHashTable->N_Elements();
  // return the new table
  hashTable = newHashTable;
}
  
  
  
BaseGDL* RemoveFromHashTable( EnvUDT* e, DStructGDL* hashStruct, BaseGDL* key)
{
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    if(hashStruct == NULL) return NULL;
  bool isfoldcase = Hashisfoldcase( hashStruct);
  // our current table
  DPtr thisTableID = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0];
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
    hashIndex = HashIndex( hashTable, key, isfoldcase);
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

  --((*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0]);
  return retValue;
}
  
  
// must pass hashTable as reference as it might be changed (GrowHashTable)
void InsertIntoHashTable( DStructGDL* hashStruct, DStructGDL*& hashTable, BaseGDL* key, BaseGDL* value)
{
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()
// key is always copied into heap, value is taken as is.
    if(hashStruct == NULL) return;
  bool isfoldcase = Hashisfoldcase( hashStruct);
  DLong nSize = hashTable->N_Elements();
  assert( nSize == (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0]);
  DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0];
  
  if( nCount == 0)
  {
    assert( nSize >= 1);
    DLong insertPos = nSize / 2;
//    std::cout << "   at " <<  i2s(insertPos) << std::endl;
    DPtr pID = BaseGDL::interpreter->NewHeap(1,value);
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertPos)))[0] = pID;
    DPtr kID = BaseGDL::interpreter->NewHeap(1,key->Dup());
    (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertPos)))[0] = kID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = 1;
    return;
  }
  
  // must be done here, otherwise hashIndex will be not in sync
  if( nSize <= (nCount * 10 / 8)) // grow on 80% occupation. TODO: find optimal value
  {
//     std::cout << "   grow table "<< i2s(nSize) << " -> " << i2s(nSize * 2) << std::endl;

    // deletes hashTable, replaces it by new one, updates nSize
    GrowHashTable( hashStruct, hashTable, nSize * 2); // grow to 50% occupation.  TODO: find optimal value
//     nSize = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0];
    nSize = hashTable->N_Elements();
  }
  
  DLong hashIndex = HashIndex( hashTable, key, isfoldcase);
  if( hashIndex >= 0) // hit -> overwrite
  {
//    std::cout << "  (ovwrt) at "<< i2s(hashIndex) <<std::endl;
   
    assert( hashIndex < nSize);
    DPtr vID = (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, hashIndex)))[0];
    GDLDelete( BaseGDL::interpreter->GetHeap( vID));
    BaseGDL::interpreter->GetHeap( vID) = value;
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

//  std::cout << i2s(i-1) << " -> " << i2s(i) << std::endl;
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
//    std::cout << i2s(i+1) << " -> " << i2s(i) << "   kID:";//std::endl;
//    std::cout << (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i+1)))[0]; 
//    std::cout << "   vID:" << (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i+1)))[0];
//    std::cout << std::endl;

      (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i)))[0] =
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, i+1)))[0];
      
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i)))[0] =
      (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, i+1)))[0];

//    std::cout << i2s(i+1) << " -> " << i2s(i) << std::endl;
    }
    break;
      }      
    }
  }

  assert( nextFreeElementIx >= 0 && nextFreeElementIx < nSize);
  
  // insert the element
  // overwrite, the (now overwritten) pointers are already moved or are NULL
  DPtr kID = BaseGDL::interpreter->NewHeap(1,key->Dup());
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, insertPos)))[0] = kID;
  DPtr pID = BaseGDL::interpreter->NewHeap(1,value);
  (*static_cast<DPtrGDL*>(hashTable->GetTag( pValueTag, insertPos)))[0] = pID;

//   std::cout << "   at "<< i2s(insertPos) << "(" << i2s(kID) << "," << i2s(pID) << ")" <<std::endl;

  (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = ++nCount;
}
  
  
  
void HASH__ToStream( DStructGDL* oStructGDL, std::ostream& o, SizeT w, SizeT* actPosPtr)
{     
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()
  
  SizeT nCount = (*static_cast<DLongGDL*>(oStructGDL->GetTag( TableCountTag, 0)))[0];

  DPtr Ptr = (*static_cast<DPtrGDL*>( oStructGDL->GetTag( pTableTag, 0)))[0];
  DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

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
      o << '\n';
  }
}

DLong GetInitialTableSize( DLong nEntries)
{
  DLong initialTableSize = 4;
  DLong minEntries = nEntries * 2; // initial min 50% filling. TODO: find optimal value
  while( initialTableSize < minEntries) initialTableSize <<= 1; //*= 2;
  return initialTableSize;
}
 
DObj new_hashStruct(  DLong initialTableSize, DStructGDL*& hashTable,
                        bool foldcasekw=false, bool isordered=false)
{
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()
    static unsigned fold_case_mask = 0x00000001;
    static unsigned orderedmask = 0x00000010;

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* hashDesc=structDesc::HASH;
    DStructDesc* entryDesc=structDesc::GDL_HASHTABLEENTRY;
    assert( hashDesc != NULL && hashDesc->NTags() > 0);
    assert( entryDesc != NULL && entryDesc->NTags() > 0);

    DStructGDL* hashStruct= new DStructGDL( hashDesc, dimension());
    DObj objID= BaseGDL::interpreter->NewObjHeap( 1, hashStruct);

    hashTable= new DStructGDL( entryDesc, dimension(initialTableSize));
    DPtr hashTableID= BaseGDL::interpreter->NewHeap( 1, hashTable);

    (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = initialTableSize;
    unsigned long bitmask=0;
        // apply specialty bits to TABLE_BITS of hashStruct
        if( foldcasekw) bitmask = fold_case_mask;
        if( isordered) bitmask |= orderedmask;
        (*static_cast<DLongGDL*>( hashStruct->GetTag( TableBitsTag, 0)))[0] = bitmask;
    return objID;
}  


static BaseGDL* struct_tohash( EnvT* e,DStructGDL* parStruct,
                        bool foldcasekw, bool extractkw, bool isordered=false)
{

    static int kwLOWERCASEIx = e->KeywordIx("LOWERCASE");

    bool keytolower = e->KeywordSet(kwLOWERCASEIx);
    DStructDesc* desc = parStruct->Desc();
    DStructGDL* hashTable;
    DLong initialTableSize = GetInitialTableSize( desc->NTags());
    DObj objID= 
        new_hashStruct( initialTableSize, hashTable, foldcasekw, isordered);
    BaseGDL* newObj = new DObjGDL( objID); // the hash object
    Guard<BaseGDL> newObjGuard( newObj);
    DStructGDL* hashStruct = GetOBJ( newObj, 0);
    for( SizeT t=0; t<desc->NTags(); ++t)
      {
          DString validName = ValidTagName( desc->TagName(t));
    // http://blog.fourthwoods.com/2013/12/10/convert-c-string-to-lower-case-or-upper-case/
          if(keytolower) 
            std::transform(validName.begin(), validName.end(),
                            validName.begin(), ::tolower);
          DStringGDL *structKey = new DStringGDL( validName);
          BaseGDL* structData;
          BaseGDL* par = parStruct->GetTag(t,0);
          assert(par != NULL);
          if( extractkw and par->Type() == GDL_STRUCT and (par->N_Elements()==1))
                    structData = structP_tohash( e, par, foldcasekw, extractkw, isordered);
          else structData = par->Dup();
          
          InsertIntoHashTable( hashStruct, hashTable, structKey, structData);
      }
    
    newObjGuard.Release();
    return newObj;
}

static BaseGDL* structP_tohash( EnvT* e,BaseGDL* par, bool foldcasekw, bool extractkw, bool isordered=false)
{
    if(par->N_Elements() != 1)
            e->Throw(" only a single struct may be hashed");
    DStructGDL* parStruct = static_cast<DStructGDL*>(par);
    return struct_tohash( e, parStruct, foldcasekw, extractkw, isordered);
}

  BaseGDL* hash_tostruct( DStructGDL* self ,
    BaseGDL* missing, BaseGDL** skipped, bool recursive, bool no_copy)
  {

    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    DPtr Ptr = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
    DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

    DLong nSize = hashTable->N_Elements();
  
    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()
 
    DStructDesc* listDesc= structDesc::LIST;
    DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;

    DStructGDL* listStruct = NULL;
    BaseGDL* newObj = NULL;
    Guard<BaseGDL> newListObjGuard;
    if( skipped != NULL)
    {
      listStruct= new DStructGDL( listDesc, dimension());
      DObj objID= BaseGDL::interpreter->NewObjHeap( 1, listStruct); // owns objStruct
      newObj = new DObjGDL( objID); // the list object
      newListObjGuard.Init( newObj);
    }
    
    DStructDesc* nStructDesc = new DStructDesc( "$truct");
    // instance takes care of nStructDesc since it is unnamed
    //  dimension dim( 1);
    //  DStructGDL* instance = new DStructGDL( nStructDesc, dim);
    DStructGDL* instance = new DStructGDL( nStructDesc);
    Guard<DStructGDL> instance_guard(instance);

    DStructGDL* cStructLast = NULL;
    DStructGDL* cStruct = NULL;
    DPtr cID = 0;
    
    for( SizeT el=0; el<nSize; ++el)
    {
      DPtr pKey = (*static_cast<DPtrGDL*>(hashTable->GetTag( pKeyTag, el)))[0];
      if( pKey == 0)     continue;

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
      if( key->Type() == GDL_STRING && value != NULL) {
    assert( key->N_Elements() == 1);
    DString keyString = (*static_cast<DStringGDL*>(key))[0];
    
    DString tagString = ValidTagName( keyString);
        if( nStructDesc->TagIndex( tagString) == -1) {
            if(value->Type() == GDL_OBJ and value->StrictScalar()) {
                if(recursive) {
                    DStructGDL* theStruct = GetOBJ( value, NULL);
                    if( (theStruct->Desc())->IsParent("HASH") ) {
                        SizeT nhash = lib::HASH_count(theStruct);
                        if( nhash > 0) {
                             value = hash_tostruct( theStruct ,
                                    missing, skipped, recursive, no_copy);
                            }
                        }
            }
            } //else if(trace_me) std::cout<<" value->Type() or StrictScalar()" << std::endl;
            //if(trace_me) lib::help_item(std::cout,value,tagString, false);

          instance->NewTag( tagString, value->Dup());
      added = true;

         } //else if(trace_me) std::cout<<" TagIndex != -1" << std::endl;
      }
      
      if( !added && listStruct != NULL) // add key to skipped
      {
    key = key->Dup();
    DPtr dID = BaseGDL::interpreter->NewHeap(1,key);

    cStruct = new DStructGDL( containerDesc, dimension());
    cID = BaseGDL::interpreter->NewHeap(1,cStruct);
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

    instance_guard.Release();
    if( instance->NTags() == 0)
      return NullGDL::GetSingleInstance();
    return instance;
  }

  
// checks wether referenced values are equal (recursively). This is IDL behaviour.
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
    SizeT LIST_count( DStructGDL* list);
BaseGDL* list_rightextraction( EnvUDT* e, BaseGDL* theref, int iprm  );
void list_leftinsertion( EnvUDT* e, BaseGDL* theref, int iprm  );
  BaseGDL* HASH___OverloadIsTrue( EnvUDT* e)
  {
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    SizeT nParam = e->NParam(1);

    static int kwSELFIx = 0;
    DStructGDL* self = GetOBJ( e->GetTheKW( kwSELFIx), e); //by ref faster

    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];
    
    if( nCount == 0)
      return new DByteGDL(0);
    else
      return new DByteGDL(1);
  }
  
  
  
  BaseGDL* HASH___OverloadNEOp( EnvUDT* e)
  {
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    SizeT nParam = e->NParam(); // number of parameters actually given
    // more precise error message
    if( nParam < 3) // consider SELF
      ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

    BaseGDL* l = e->GetKW(1);
    BaseGDL* r = e->GetKW(2);

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
      DPtr Ptr = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

      if( rightStruct != NULL)
      {
    compareStruct = rightStruct;
    DPtr Ptr = (*static_cast<DPtrGDL*>( compareStruct->GetTag( pTableTag, 0)))[0];
    compareTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));
      }
      else
      {
    compare = r;    
      }
    } 
    else if( rightStruct != NULL)
    {
      hashStruct = rightStruct;
      DPtr Ptr = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

      compare = l;
    }
    else
      ThrowFromInternalUDSub( e, "At least one parameter must be a HASH.");
  bool isfoldcase = Hashisfoldcase( hashStruct);

    DLong nSize = hashTable->N_Elements();
//     DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0];

    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()

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
    DLong insertIx = HashIndex(compareTable, key, isfoldcase);
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

    DLong insertIx = HashIndex(hashTable, key, isfoldcase);
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
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    SizeT nParam = e->NParam(); // number of parameters actually given
    // more precise error message
    if( nParam < 3) // consider SELF
      ThrowFromInternalUDSub( e, "Two parameters are needed: LEFT, RIGHT.");

    BaseGDL* l = e->GetKW(1);
    BaseGDL* r = e->GetKW(2);
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

    bool isfoldcase = Hashisfoldcase(leftStruct) or Hashisfoldcase(rightStruct);

    DStructGDL* hashStruct = NULL;
    DStructGDL* hashTable = NULL;
    DStructGDL* compareStruct = NULL;
    DStructGDL* compareTable = NULL;
    BaseGDL* compare = NULL;
    if( leftStruct != NULL)
    {
      hashStruct = leftStruct;
      DPtr Ptr = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

      if( rightStruct != NULL)
      {
    compareStruct = rightStruct;
    DPtr Ptr = (*static_cast<DPtrGDL*>( compareStruct->GetTag( pTableTag, 0)))[0];
    compareTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));
      }
      else
      {
    compare = r;    
      }
    } 
    else if( rightStruct != NULL)
    {
      hashStruct = rightStruct;
      DPtr Ptr = (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0];
      hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

      compare = l;
    }
    else
      ThrowFromInternalUDSub( e, "At least one parameter must be a HASH.");

    DLong nSize = hashTable->N_Elements();
    DLong nCount = (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0];

  GDL_LIST_STRUCT()
  GDL_CONTAINER_NODE()

    // making the result list:
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
    DLong insertIx = HashIndex(compareTable, key, isfoldcase);
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
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

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
      DLong nCountL = (*static_cast<DLongGDL*>(leftStruct->GetTag( TableCountTag, 0)))[0];
      DLong nCountR = (*static_cast<DLongGDL*>(rightStruct->GetTag( TableCountTag, 0)))[0];

      DPtr PtrL = (*static_cast<DPtrGDL*>( leftStruct->GetTag( pTableTag, 0)))[0];
      DStructGDL* hashTableL = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( PtrL));
      DPtr PtrR = (*static_cast<DPtrGDL*>( rightStruct->GetTag( pTableTag, 0)))[0];
      DStructGDL* hashTableR = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( PtrR));

      DLong nSizeL = hashTableL->N_Elements();
      DLong nSizeR = hashTableR->N_Elements();
      
      DLong nCountMax = nCountL + nCountR;
      
      // new hash table
      DLong initialTableSize = GetInitialTableSize( nCountMax);
      DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;

      DLong nSize = hashTable->N_Elements();

      assert( nSize >= nCountMax);

      if( nCountMax == 0) // two empty HASH
      {
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = nSize;
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

      (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = nSize;     
      (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = nCount;     
      
      newObjGuard.Release();
      return newObj;
    }

    DStructGDL* newHashTable;

      // at least one is struct
    if( leftID != 0) // left is HASH
    {
      DLong nCountL = (*static_cast<DLongGDL*>(leftStruct->GetTag( TableCountTag, 0)))[0];

      DPtr PtrL = (*static_cast<DPtrGDL*>( leftStruct->GetTag( pTableTag, 0)))[0];
      DStructGDL* hashTableL = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( PtrL));

      // right must be struct
      DLong nCountR = rightStruct->NTags();
      
      DLong nCount = nCountL + nCountR;
      
      DLong initialTableSize = GetInitialTableSize( nCount);
      newHashTable = CopyHashTable( leftStruct, hashTableL, initialTableSize);
      DPtr hashTableID= e->NewHeap( 1, newHashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
      (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = nCountL;
    }
    else if( rightID != 0) // right is HASH
    {
      DLong nCountR = (*static_cast<DLongGDL*>(rightStruct->GetTag( TableCountTag, 0)))[0];

      DPtr PtrR = (*static_cast<DPtrGDL*>( rightStruct->GetTag( pTableTag, 0)))[0];
      DStructGDL* hashTableR = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( PtrR));

      // right must be struct
      DLong nCountL = leftStruct->NTags();
      
      DLong nCount = nCountL + nCountR;
      
      DLong initialTableSize = GetInitialTableSize( nCount);
      newHashTable = CopyHashTable( rightStruct, hashTableR, initialTableSize);
      DPtr hashTableID= e->NewHeap( 1, newHashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
      (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = nCountR;
    }
    else // both are struct
    {
      DLong nCountL = leftStruct->NTags();
      DLong nCountR = rightStruct->NTags();
      DLong nCount = nCountL + nCountR;
      DLong initialTableSize = GetInitialTableSize( nCount);
      newHashTable = new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, newHashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
      // set to zero at init //(*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = 0;      
    }
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = newHashTable->N_Elements();

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
    static int kwSKIPPEDIx = e->GetKeywordIx("SKIPPED");
    static int kwMISSINGIx = e->GetKeywordIx("MISSING"); 
    static int kwRECURSIVEIx = e->GetKeywordIx("RECURSIVE");
    static int kwNO_COPYIx = e->GetKeywordIx("NO_COPY");
    
    BaseGDL* missing = e->GetKW( kwMISSINGIx);
    BaseGDL** skipped = NULL;
    if( e->GlobalKW( kwSKIPPEDIx)) skipped = &e->GetTheKW(kwSKIPPEDIx);
    bool recursive = e->KeywordSet(kwRECURSIVEIx);
    bool no_copy = e->KeywordSet( kwNO_COPYIx);
    
    
    static int kwSELFIx = kwSKIPPEDIx + 1;

    SizeT nParam = e->NParam(1); 
    DStructGDL* self = GetOBJ( e->GetTheKW( kwSELFIx), e);
    return hash_tostruct( self, missing, skipped, recursive, no_copy);
}
  BaseGDL* hash__isempty( EnvUDT* e)
  {
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static int kwSELFIx = 0;
    BaseGDL* selfP = e->GetTheKW( kwSELFIx);
    DStructGDL* self = GetOBJ( selfP, e);
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];
    if (nCount > 0) return new DByteGDL( 0); else return new DByteGDL(1);
  }

  BaseGDL* hash__isordered( EnvUDT* e)
  {
    static unsigned ordmask = 0x00000010;
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS");
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static int kwSELFIx = 0;
    BaseGDL* selfP = e->GetTheKW( kwSELFIx);
    DStructGDL* self = GetOBJ( selfP, e);
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];
    if(trace_me) std::cout << "isordered: nCount=" << nCount << std::endl;
    DLong bits = (*static_cast<DLongGDL*>( self->GetTag( TableBitsTag , 0)))[0];
    if(trace_me) std::cout << "isordered: bits=" << bits << std::endl;
    if ( (bits & ordmask) == 0) return new DByteGDL( 0); else return new DByteGDL(1);
  }
  BaseGDL* hash__isfoldcase( EnvUDT* e)
  {
    static unsigned fold_case_mask = 0x00000001;
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS");
    
    if ( Hashisfoldcase( GetOBJ( e->GetTheKW( 0), e)) ) return new DByteGDL(1);
     else return new DByteGDL(0);
  }

  BaseGDL* hash__count( EnvUDT* e)
  {
    static int kwSELFIx = 0;
    static int kwVALUEIx = 1;
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

    BaseGDL* selfP = e->GetTheKW( kwSELFIx);

    SizeT nParam = e->NParam(1);
    if( nParam == 1) return new DLongGDL( HASH_count( GetOBJ( selfP, e)));

    DObjGDL* selfObj = static_cast<DObjGDL*>(selfP);
    // nParam > 1:
      BaseGDL* r = e->GetTheKW( kwVALUEIx);
    
      DObjGDL* listObj = static_cast<DObjGDL*>( selfObj->EqOp( r));
      Guard<DObjGDL> listObjGuard( listObj);
    
      DStructGDL* selfLIST = GetOBJ( listObj, e);

      DLong nList = (*static_cast<DLongGDL*>( selfLIST->GetTag( nListTag, 0)))[0];        

      return new DLongGDL( nList);
    }
    
  
  BaseGDL* hash__where( EnvUDT* e)
  {
    static unsigned TableCountTag = structDesc::HASH->TagIndex( "TABLE_COUNT");
    static unsigned nListTag = structDesc::LIST->TagIndex( "NLIST");

    static int kwNCOMPLEMENTIx = e->GetKeywordIx("NCOMPLEMENT"); 
    static int kwCOUNTIx =  e->GetKeywordIx("COUNT"); 
    static int kwCOMPLEMENTIx =  e->GetKeywordIx("COMPLEMENT"); 
    static int kwSELFIx = 3;
    static int kwVALUEIx = kwSELFIx + 1;

    SizeT nParam = e->NParam(2); // SELF, VALUE

    BaseGDL* selfP = e->GetTheKW( kwSELFIx);
    DStructGDL* self = GetOBJ( selfP, e);

    DObjGDL* selfObj = static_cast<DObjGDL*>(selfP);
    
    BaseGDL* r = e->GetTheKW( kwVALUEIx);
    
    DObjGDL* listObj = static_cast<DObjGDL*>( selfObj->EqOp( r));
    Guard<DObjGDL> listObjGuard( listObj);

    DStructGDL* selfLIST = GetOBJ( listObj, e);
    DLong nList = (*static_cast<DLongGDL*>( selfLIST->GetTag( nListTag, 0)))[0];          

    if( e->KeywordPresent( kwNCOMPLEMENTIx)) // NCOMPLEMENT
    {
        DLong nCount = 
            (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];
    e->SetKW( kwNCOMPLEMENTIx, new DLongGDL( nCount - nList));
    }
    if( e->KeywordPresent( kwCOUNTIx)) // COUNT
    {
    e->SetKW( kwCOUNTIx, new DLongGDL( nList));
    }
    if( e->KeywordPresent( kwCOMPLEMENTIx)) // COMPLEMENT
    {
        DObjGDL* compObj = 
            static_cast<DObjGDL*>( selfObj->NeOp( r));
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
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    static int kwSELFIx = 0;

    SizeT nParam = e->NParam(1); // SELF

    BaseGDL* selfP = e->GetTheKW( kwSELFIx);
    DStructGDL* self = GetOBJ( selfP, e);

    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];

    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()
   
    DStructDesc* listDesc= structDesc::LIST;
    DStructDesc* containerDesc= structDesc::GDL_CONTAINER_NODE;
  
    DStructGDL* listStruct= new DStructGDL( listDesc, dimension());
    DObj objID= e->NewObjHeap( 1, listStruct); // owns objStruct
    BaseGDL* newObj = new DObjGDL( objID); // the list object
    Guard<BaseGDL> newObjGuard( newObj);

    DPtr Ptr = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
    DStructGDL* hashTable = static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( Ptr));

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
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    // see overload.cpp
    static int kwSELFIx = 0;
    static int kwKEYIx = 1;

    SizeT nParam = e->NParam(2); // SELF, KEYLIST

    BaseGDL* selfP = e->GetTheKW( kwSELFIx);
    DStructGDL* self = GetOBJ( selfP, e);

    bool isfoldcase = Hashisfoldcase( self);
   
    BaseGDL* keyList = e->GetKW( kwKEYIx);
    if( keyList == NULL || keyList == NullGDL::GetSingleInstance())
      ThrowFromInternalUDSub( e, "Key must be a scalar string or number.");
    if( keyList->Type() != GDL_STRING && !NumericType(keyList->Type()))
      ThrowFromInternalUDSub( e, "Key must be a scalar string or number.");

    DPtr Ptr = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
    DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( Ptr));

    if( keyList->N_Elements() == 1)
    {
      DLong hashIndex = HashIndex( thisHashTable, keyList, isfoldcase);
      if( hashIndex >= 0)
    return new DIntGDL( 1);
      return new DIntGDL( 0);
    }
    
    SizeT keyListN_Elements = keyList->N_Elements();
    DIntGDL* result = new DIntGDL( dimension(keyListN_Elements),BaseGDL::ZERO); // zero
    Guard<BaseGDL> resultGuard( result);
    for( SizeT i=0; i<keyListN_Elements; ++i)
    {
    BaseGDL* key = keyList->NewIx( i);
//  Guard<BaseGDL> keyGuard( key);
    DLong hashIndex = HashIndex( thisHashTable, key, isfoldcase);
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
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    // see overload.cpp
    static int kwALLIx = 0; 
    static int kwSELFIx = 1;
    static int kwINDEXIx = 2;

    bool kwALL = false;
    if (e->KeywordSet(kwALLIx)){ kwALL = true;}

    SizeT nParam = e->NParam(1);

    DStructGDL* self = GetOBJ( e->GetTheKW( kwSELFIx), e);

    if( kwALL)
    {
      if( asFunction)
      {
    DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];

    DLong initialTableSize = GetInitialTableSize( 0);

    // our current table
    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
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
    (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( self->GetTag( TableSizeTag, 0)))[0] = initialTableSize;
    (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0] = 0;
    
    // set our old table to new HASH
    (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = thisTableID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = nSize;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableCountTag, 0)))[0] = nCount;
    
    newObjGuard.Release();
    return newObj;    
      }
      else
      {
    DLong initialTableSize = GetInitialTableSize( 0);

    // our current table
    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
//      DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));

    // our new hash table
    DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
    DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 

    // set our table to new empty table
    (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( self->GetTag( TableSizeTag, 0)))[0] = initialTableSize;
    (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0] = 0;
    
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

      DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= e->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;

      (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = initialTableSize;

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
BaseGDL* hash_subset(DStructGDL* thisTable, BaseGDL* index, bool isfoldcase);
BaseGDL* hash_newhash(SizeT nEntries = 0, bool isfoldcase = false) {
    // new hash
    GDL_HASHTABLEENTRY()
    GDL_HASH_STRUCT()
    static unsigned fold_case_mask = 0x00000001;

    DLong initialTableSize = GetInitialTableSize(nEntries); 
    DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
    DObj objID= BaseGDL::interpreter->NewObjHeap( 1, hashStruct);

    if( isfoldcase) (*static_cast<DLongGDL*>
        ( hashStruct->GetTag( TableBitsTag, 0)))[0]
                            = fold_case_mask;
    BaseGDL* newObj = new DObjGDL( objID);
    Guard<BaseGDL> newObjGuard( newObj);

    DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
    DPtr hashTableID= BaseGDL::interpreter->NewHeap( 1, hashTable);
    (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = initialTableSize;

    newObjGuard.Release();
    return newObj;

}

    void hash_leftinsertion( EnvUDT* e, DStructGDL* theStruct, int iprm  )
{
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()
    static unsigned isRangeIx = 3;
    static unsigned prmbeg = isRangeIx+1;

    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()
    
    // handle DOT access
    bool dotAccess = false;
    
    BaseGDL** objRef = &e->GetTheKW(1);
    if( *objRef == NULL || *objRef == NullGDL::GetSingleInstance())
    {
      if( !e->GlobalKW(1))
    ThrowFromInternalUDSub( e, "Parameter 1 (OBJREF) is undefined.");    
      dotAccess = true;
    }
    BaseGDL* rValue = e->GetTheKW(2);
    if( rValue == NULL)  rValue = NullGDL::GetSingleInstance();

    BaseGDL* parX = e->GetKW(iprm + prmbeg);
    if( parX == NULL)  ThrowFromInternalUDSub( e, "Parameter is undefined: "
                            + e->Caller()->GetString(e->GetTheKW( iprm + prmbeg )));
    bool stolen = e->StealLocalKW(iprm + prmbeg);
    if( !stolen) parX = parX->Dup(); // if called explicitely
    DType theType = parX->Type();
    if(trace_me) {
        std::cout << " parX->Type() " << theType ;
        help_item(std::cout, parX, " hash-leftinsert ",false);
    }
    DStructGDL* ListHead;
    SizeT listSize = 0;
    DPtr keyNode;
    bool iskeylist = false;;
    if(theType == GDL_OBJ && parX->Rank() == 0) {
          DObj p=(*static_cast<DObjGDL*>( parX))[0];
          if(p != 0) {
          ListHead = GetOBJ( parX, e);
          DStructDesc* desc = ListHead->Desc();
          iskeylist = desc->IsParent("LIST");
            }
    if(iskeylist) {
        listSize = (*static_cast<DLongGDL*>(ListHead->GetTag( nListTag, 0)))[0];
        keyNode = GetLISTNode(e, ListHead, 0);
        if( listSize == 1) { 
// accidentally this will pass through an array embedded in a single-element list.
            parX = GetNodeData( keyNode);
            iskeylist = false;
                }
//
                }
      }

    DPtr thisTableID = (*static_cast<DPtrGDL*>( theStruct->GetTag( pTableTag, 0)))[0];
    DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->
                GetHeap( thisTableID));
    bool isfoldcase = Hashisfoldcase( theStruct);

    // non-range (keyed)
    SizeT nKey = parX->N_Elements();
    if(iskeylist) nKey = listSize;

    if( nKey == 1) // single key
    { 
    if( dotAccess) // -> objRef is NULL (or !NULL)
    {
      if( rValue != NullGDL::GetSingleInstance())
      {
        ThrowFromInternalUDSub( e, "For struct access (OBJREF is !NULL), RVALUE must be !NULL as well.");      
      }

      DLong hashIndex = HashIndex( thisHashTable, parX, isfoldcase);
      if( hashIndex < 0)
        ThrowFromInternalUDSub( e, "Key not found.");

      *objRef = thisHashTable->GetTag( pValueTag, hashIndex)->Dup();
      return;
    }    
    
    InsertIntoHashTable( theStruct, thisHashTable, parX, rValue->Dup());
    return;
    }
    if( dotAccess)
        ThrowFromInternalUDSub( e, "Only single value struct access is allowed.");
    
    bool isvalscalar = false;
    bool isvallist = false;
    if( rValue == NULL || rValue == NullGDL::GetSingleInstance())
        isvalscalar = true;
    else if( rValue->StrictScalar() ) isvalscalar = true;
    if(trace_me) std::cout << " isvalscalar/ iskeylist? nKey:"<<
                isvalscalar<< iskeylist << nKey << std::endl;
    if( !isvalscalar &&
             rValue->N_Elements() != nKey )
    ThrowFromInternalUDSub( e, "Key and Value must have the same number of elements.");
    
    if( isvalscalar){
        if( rValue->Type() == GDL_OBJ) {
          DObj p=(*static_cast<DObjGDL*>( rValue))[0];
          if(p != 0) {
          ListHead = GetOBJ( rValue, e);
          DStructDesc* desc = ListHead->Desc();
          isvallist = desc->IsParent("LIST");
            }
        }
        if(isvallist) {
            listSize = (*static_cast<DLongGDL*>(ListHead->GetTag( nListTag, 0)))[0];
            if(listSize != nKey)
    ThrowFromInternalUDSub( e, "Key and Value must have the same number of elements.");
            DPtr valNode = GetLISTNode(e, ListHead, 0);
            if(iskeylist) 
                for( SizeT kIx=0; kIx<nKey; ++kIx)
              InsertIntoHashTable( theStruct, thisHashTable,
                    GetNodeData(keyNode), (GetNodeData(valNode))->Dup());
            else     
                for( SizeT kIx=0; kIx<nKey; ++kIx)
              InsertIntoHashTable( theStruct, thisHashTable,
                        parX->NewIx(kIx), (GetNodeData(valNode))->Dup());
        } else {
        if(iskeylist) 
            for( SizeT kIx=0; kIx<nKey; ++kIx)
          InsertIntoHashTable( theStruct, thisHashTable, 
                        GetNodeData(keyNode), rValue->Dup());
        else     
            for( SizeT kIx=0; kIx<nKey; ++kIx)
          InsertIntoHashTable( theStruct, thisHashTable,
                        parX->NewIx(kIx), rValue->Dup());
      }
    } else  {           // rValue not a scalar.
        if(iskeylist) {
            for( SizeT kIx=0; kIx<nKey; ++kIx)
            InsertIntoHashTable( theStruct, thisHashTable, 
                    GetNodeData(keyNode), rValue->NewIx(kIx));
        } else {
            for( SizeT kIx=0; kIx<nKey; ++kIx)
            InsertIntoHashTable( theStruct, thisHashTable,
                    parX->NewIx(kIx), rValue->NewIx(kIx));
            }
        }
    }
 
  void HASH___OverloadBracketsLeftSide( EnvUDT* e)
  {
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

//    static unsigned par1Ix = 4;
    static unsigned isRangeIx = 3;
    static unsigned prmbeg = isRangeIx+1;
//    trace_me = lib::trace_arg();
      std::string trcn = trace_me? "\n;" :";";
    
    SizeT nParam = e->NParam(1);
    if( nParam < 5)      ThrowFromInternalUDSub( e, 
        "Four parameters are needed: OBJREF, RVALUE, ISRANGE, SUB1.");

    // handle DOT access
    bool dotAccess = false;
    
    BaseGDL** objRef = &e->GetTheKW(1);
    if( *objRef == NULL || *objRef == NullGDL::GetSingleInstance())
    {
      if( !e->GlobalKW(1))
    ThrowFromInternalUDSub( e, "Parameter 1 (OBJREF) is undefined.");    
      dotAccess = true;
    }
    BaseGDL* rValue = e->GetTheKW(2);
    if( rValue == NULL)  rValue = NullGDL::GetSingleInstance();
    
   DLongGDL* isRange = static_cast<DLongGDL*>( e->GetKW(isRangeIx)); //insure is defined!
   SizeT nIsRange = isRange->N_Elements();

   if(nIsRange == 0)  {
      nIsRange = nParam - isRangeIx - 1;
       for (int i=0; i < nIsRange; i++) (*isRange)[i] = 0;
        if(trace_me) std::cout << " nIsRange was 0!" ;
      }
// isRange is a series of 0 or 1 indicating if a range is present.
// more comments in list__]/
  if(trace_me) {
    std::cout << "Hash-OL left:nIsRange " << nIsRange << ",nParam " << nParam
        << " ,isRange:";
      for (int i=0; i < nIsRange; i++)
        std::cout << " "<<(*isRange)[i];
          std::cout << std::endl;
        }
// hash[*] = ... (Decline the attempt.)
    BaseGDL* parX = e->GetKW( prmbeg); // implicit SELF, ISRANGE, par1..par8
    if( parX == NULL) ThrowFromInternalUDSub( e,
     "Parameter is undefined: "  + e->Caller()->GetString(e->GetTheKW( prmbeg)));

    if( (*isRange)[0]== 1) {  
      if( parX->N_Elements() != 3)
            ThrowFromInternalUDSub( e,
            "Range vector must have 3 elements: " + 
            e->Caller()->GetString(e->GetTheKW(prmbeg)));
      
        MAKE_LONGGDL(parX, parXLong)

        if( (*parXLong)[0] != 0 || (*parXLong)[1] != -1 || 
            (*parXLong)[2] != 1) ThrowFromInternalUDSub( e,
             "Subscript range is not allowed: [" + i2s((*parXLong)[0])+
             ":"+   i2s((*parXLong)[1])+":"+i2s((*parXLong)[2])+"]");

//          (Dead End)
        ThrowFromInternalUDSub( e, 
        "Setting all [*] for hash is not allowed in GDL. \n"
        " (in IDL, kl=hash.keys(). for i=0,n_elements(kl)-1 do hash[kl[i]]= ...)\n"
      "Please report if you would appreciate this functionality.");
    }
// Generalize from  self to theStruct
    DStructGDL* self = GetOBJ( e->GetTheKW( 0), e);
    DStructGDL* theStruct = self;       

    DPtr Ptr = (*static_cast<DPtrGDL*>( 
            theStruct->GetTag( pTableTag, 0)    ))[0];
    DStructGDL* hashTable = static_cast<DStructGDL*>(
            BaseGDL::interpreter->GetHeap( Ptr));
    bool isfoldcase = Hashisfoldcase( theStruct);

    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()
// This section copied and slightly adapted from LIST_OVERLOADLEFT
    BaseGDL* theref = NULL; 
    int iprm = 0;

    SizeT listSize = 0;

    bool islist= false;
    bool ishash = true;
    bool isstruct = false;
    bool finalprm = false;

    while(  (*isRange)[iprm] == 0) { //  && (iprm+3 < nParam)
 
        finalprm = (iprm+prmbeg+1 == nParam);
        BaseGDL* XX = e->GetKW( iprm + prmbeg);
        if( XX == NULL) 
            ThrowFromInternalUDSub( e,       "Parameter is undefined " );
        if( XX->Rank() != 0 ) break; // must be a scalar

        if( islist) {
            MAKE_LONGGDL( XX, XXLong) 
            DLong Ixref = (*XXLong)[0];
            listSize = (*static_cast<DLongGDL*>(theStruct->GetTag( nListTag, 0)))[0];
            if(Ixref >= listSize) // this is certainly common.
                ThrowFromInternalUDSub( e, "direct access index is too high"
                + i2s(Ixref) + " >= nList:"+i2s(listSize));
            DStructGDL* Node = GetLISTStruct( e, 
                                 GetLISTNode( e, theStruct, Ixref));
            DPtr Ptr = 
                (*static_cast<DPtrGDL*>( Node->GetTag( pListDataTag, 0)))[0];
            if(trace_me) std::cout << " islist iprm="<<iprm;
            if( finalprm ) { // last parameter, in a list.
// list[ix]=<expression>    or list[ix,iy] = <expression>
                if(trace_me) std::cout<<" - immediate Dup()"<<std::endl;
                BaseGDL::interpreter->GetHeap( Ptr) = rValue->Dup();
                return;
      }
            theref = BaseGDL::interpreter->GetHeap( Ptr);
            if( theref == 0)
                ThrowFromInternalUDSub( e, " No data in list at index "+i2s(Ixref));
        } else if( ishash ) {
            DPtr Ptr = (*static_cast<DPtrGDL*>( 
                    theStruct->GetTag( pTableTag, 0)    ))[0];

            hashTable = static_cast<DStructGDL*>(
                    BaseGDL::interpreter->GetHeap( Ptr));
            if(trace_me) std::cout<< iprm << " =iprm, " ;
            isfoldcase = Hashisfoldcase( theStruct);
            DLong hashIndex = HashIndex( hashTable , XX, isfoldcase);
            if(trace_me) std::cout << " hashindex= "<<hashIndex;
            if(hashIndex >= 0) {
                DPtr pValue = (*static_cast<DPtrGDL*>( 
                        hashTable->GetTag( pValueTag, hashIndex)    ))[0];
                if( finalprm ) {
                    if(trace_me) std::cout<<" - immediate Dup()"<<std::endl;
                    BaseGDL::interpreter->GetHeap( pValue) = rValue->Dup();
      return;
                    } else {
                    theref = BaseGDL::interpreter->GetHeap( pValue);
    }    
            } else { // hashIndex >= 0
                if( finalprm ) {
                    if(trace_me) std::cout<<" - insert rVal->Dup()";
                    hash_leftinsertion( e,  theStruct, iprm);
    return;
    }
                 else{
                    if(trace_me) std::cout<<" newhash(0)";
                    theref = 
                        hash_subset( hashTable, 0, isfoldcase);
//                  theref = hash_newhash( 0, isfoldcase);
// Now we must insert this new hash before proceeding down the nest.                    
                    bool stolen = e->StealLocalKW(iprm + prmbeg);
                    if( !stolen) XX = XX->Dup();
                    InsertIntoHashTable( theStruct, hashTable, XX, theref->Dup());
    }
      }
        } else if( isstruct ) {
// use XX to access struct. GDL extension, IDL doesn't do this.
            int ptagindex = 0;
            if(XX->Type() != GDL_STRING) {
                MAKE_LONGGDL( XX, XXLong)
                ptagindex = (*XXLong)[0];
                if(ptagindex >= theStruct->Desc()->NTags() ) 
                    ptagindex = -1;
                }
            else {
                DString tag = (*static_cast<DStringGDL*>( XX))[0];
                StrUpCaseInplace(tag);
                ptagindex = theStruct->Desc()->TagIndex(tag);
            }
            if(ptagindex < 0) 
                ThrowFromInternalUDSub( e, " struct tag not found ");
            theref = theStruct->Get( ptagindex);    
        } else {break;}
        DType theType = theref->Type();
// Enhance (?) it by allowing pointers to lists to represent lists.
// in order to turn this off, a ptr scalar must go in as a ptrarr[1];
// This is probably not IDL behavior.
        if( theType == GDL_PTR && theref->Rank() == 0) {
              theref = BaseGDL::interpreter->GetHeap(
                (*static_cast<DPtrGDL*>( theref))[0] );
              theType = theref->Type();
              // this is probably so odd itshould be advertised when it happens:
              std::cout<<" **p= "<<std::endl;
          }
//      if( iprm + prmbeg  == nParam) break;//( no prms left)
        if(trace_me) help_item(std::cout, theref, trcn+"theref ",false);
        islist = false;
        ishash = false;
        isstruct = false;
        if(theType == GDL_OBJ && theref->Rank() == 0) {
              theStruct = GetOBJ( theref, e);
              DStructDesc* desc = theStruct->Desc();
              islist = desc->IsParent("LIST");
              ishash = desc->IsParent("HASH");
              isstruct = !(islist or ishash);
            }
        else if( theType == GDL_STRUCT and theref->N_Elements() == 1) {
            theStruct = static_cast<DStructGDL*>( theref);
             isstruct = true;
         }
        iprm++;
        if( gdl_type_lookup::IsConvertableType[theType] ) break;
         } // while
    if(trace_me) {
        std::cout << " HASH_OL[L]: iprm="<< i2s(iprm);
//
        if(theref == NULL || theref == NullGDL::GetSingleInstance() )
                std::cout <<" (theref == 0) "<< std::endl;
                else lib::help_item(std::cout, theref," theref",false);//
    }
    
    if(ishash) { // if we finish on a hash, make insertion and leave.
//      BaseGDL* parX = e->GetKW( iprm + prmbeg );
//      if( parX == 0)  ThrowFromInternalUDSub( e, "Parameter is undefined: "
//                          + e->Caller()->GetString(e->GetKW( iprm + prmbeg )));
        hash_leftinsertion( e, theStruct, iprm);
        return;
    } else if(islist || gdl_type_lookup::IsConvertableType[theref->Type()] ) {
        list_leftinsertion( e, theref, iprm);
        return;
    } else 
        ThrowFromInternalUDSub( e, 
        "Logic error in hash_overloadleft."
        "Please report this Occurence.");
    
  }

BaseGDL* hash_subset(DStructGDL* thisTable, BaseGDL* index, bool isfoldcase)
{   

    GDL_LIST_STRUCT()

    DStructGDL* ListHead;
    SizeT listSize = 0;
    DPtr keyNode;
    bool iskeylist = false;;
    SizeT nEntries = 0;
    DType theType;
    if( index != NULL) {  // use index==NULL to simply make a blank hash.
        nEntries = index->N_Elements();
        theType = index->Type();
    if(trace_me) std::cout << " hash_subset: index->Type() " << theType << std::endl;

    if(theType == GDL_OBJ && index->Rank() == 0) {
          DObj p=(*static_cast<DObjGDL*>( index))[0];
          if(p != 0) {
          ListHead = GetOBJ( index, 0);
          DStructDesc* desc = ListHead->Desc();
          iskeylist = desc->IsParent("LIST");
            }
    if(iskeylist) {
        listSize = (*static_cast<DLongGDL*>(ListHead->GetTag( nListTag, 0)))[0];
        keyNode = GetLISTNode( 0, ListHead, 0);
        if( listSize == 1) { 
            index = GetNodeData( keyNode);
            iskeylist = false;
                }
        nEntries = listSize;
                }
      }
    }

    GDL_HASHTABLEENTRY()
    GDL_HASH_STRUCT()
    static unsigned fold_case_mask = 0x00000001;

    DLong initialTableSize = GetInitialTableSize( nEntries);

    // new hash
    DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
    DObj objID= BaseGDL::interpreter->NewObjHeap( 1, hashStruct);
    if( isfoldcase) (*static_cast<DLongGDL*>
        ( hashStruct->GetTag( TableBitsTag, 0)))[0]
                            = fold_case_mask;
    BaseGDL* newObj = new DObjGDL( objID);
    Guard<BaseGDL> newObjGuard( newObj);

    DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
    DPtr hashTableID= BaseGDL::interpreter->NewHeap( 1, hashTable);
    (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
    (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = initialTableSize;

    BaseGDL* actKey;
    for( SizeT eIx=0; eIx<nEntries; ++eIx)
    {
        if(iskeylist)   actKey = GetNodeData(keyNode);
        else            actKey = index->NewIx( eIx);
      
      DLong hashIndex = HashIndex( thisTable, actKey, isfoldcase);
      if( hashIndex < 0) continue;
      DPtr vID = (*static_cast<DPtrGDL*>(thisTable->GetTag( pValueTag, hashIndex)))[0];
      BaseGDL* value = BaseGDL::interpreter->GetHeap( vID);

      if( value != NULL)    value = value->Dup();          

      InsertIntoHashTable( hashStruct, hashTable, actKey->Dup(), value);
    }
    newObjGuard.Release();
    return newObj;
  }
  

BaseGDL* hash_duplicate(DStructGDL* self) {
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
    DStructGDL* thisHashTable = 
        static_cast<DStructGDL*>(BaseGDL::interpreter->GetHeap( thisTableID));

      DLong nCount = (*static_cast<DLongGDL*>( self->GetTag( TableCountTag, 0)))[0];
      SizeT nEntries = nCount;

      DLong initialTableSize = GetInitialTableSize( nEntries);
    
      // new hash
      DStructGDL* hashStruct= new DStructGDL( structDesc::HASH, dimension());
//    unsigned long bitmask;
            (*static_cast<DLongGDL*>( hashStruct->GetTag( TableBitsTag, 0)))[0] = 
            (*static_cast<DLongGDL*>( self->GetTag( TableBitsTag , 0)))[0];

      DObj objID=BaseGDL::interpreter->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
      BaseGDL* newObj = new DObjGDL( objID); // the return HASH object
      Guard<BaseGDL> newObjGuard( newObj);
      // the return hash table
      DStructGDL* hashTable= new DStructGDL( structDesc::GDL_HASHTABLEENTRY, dimension(initialTableSize));
      DPtr hashTableID= BaseGDL::interpreter->NewHeap( 1, hashTable); // owns hashTable, sets ref count to 1 
      (*static_cast<DPtrGDL*>( hashStruct->GetTag( pTableTag, 0)))[0] = hashTableID;
      (*static_cast<DLongGDL*>( hashStruct->GetTag( TableSizeTag, 0)))[0] = initialTableSize;
      if(trace_me) 
            std::cout << "hash_duplicate:  nCount= " << nCount <<
            " nEntries= " << nEntries ;    
      SizeT sourceIx = 0;
      for( SizeT eIx=0; eIx<nEntries; ++eIx)
      {
        DPtr kID = (*static_cast<DPtrGDL*>( thisHashTable->GetTag( pKeyTag, sourceIx)))[0];
        while( kID == 0)
          kID = (*static_cast<DPtrGDL*>( thisHashTable->GetTag( pKeyTag, ++sourceIx)))[0];

        DPtr vID = (*static_cast<DPtrGDL*>(thisHashTable->GetTag( pValueTag, sourceIx++)))[0];

        BaseGDL* key = BaseGDL::interpreter->GetHeap( kID);
        assert( key != NULL);
        BaseGDL* value = BaseGDL::interpreter->GetHeap( vID);
        if( value != NULL)   {
            if( value->Type() == GDL_OBJ && value->Rank() == 0) {
                  DObj p=(*static_cast<DObjGDL*>( value))[0];
                  if(p != 0) {
                      DStructGDL* theStruct = GetOBJ( value, 0);
                      DStructDesc* desc = theStruct->Desc();
                      if( desc->IsParent("HASH")) value = hash_duplicate(theStruct);
                  }
               }
           }
        InsertIntoHashTable( hashStruct, hashTable, key->Dup(), value->Dup());

      }
        
       newObjGuard.Release();
      return newObj;
}
  
  BaseGDL* HASH___OverloadBracketsRightSide( EnvUDT* e)
  {
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    static unsigned fold_case_mask = 0x00000001;
    static unsigned isRangeIx = 1;
    static unsigned prmbeg = isRangeIx+1;
//    trace_me = lib::trace_arg();
    
    SizeT nParam = e->NParam(1); 
    
    if( nParam < 3) ThrowFromInternalUDSub( e,
        "Two parameters are needed: ISRANGE, SUB1 [, ...].");

    DStructGDL* self = GetOBJ( e->GetTheKW( 0), e);

    DLongGDL* isRange = static_cast<DLongGDL*>( e->GetKW(isRangeIx));
    SizeT nIsRange = isRange->N_Elements();

    if(nIsRange == 0)  { // sometimes this comes in wrong - fix it.
       nIsRange = nParam - isRangeIx - 1;
       for (int i=0; i < nIsRange; i++) (*isRange)[i] = 0;
     }
// isRange is a series of 0 or 1 indicating if a range is present.
// more comments in list__]/
      if(trace_me) {
        std::cout << "Hash-OL right:nIsRange " << nIsRange << ",nParam " << nParam
            << " ,isRange:";
          for (int i=0; i < nIsRange; i++)
            std::cout << " "<<(*isRange)[i];
              std::cout << std::endl;
            }
    bool isfoldcase = Hashisfoldcase( self);
    DPtr thisTableID = (*static_cast<DPtrGDL*>( self->GetTag( pTableTag, 0)))[0];
    DStructGDL* thisHashTable = static_cast<DStructGDL*>(e->Interpreter()->GetHeap( thisTableID));
    
    BaseGDL* parX = e->GetKW( prmbeg); // implicit SELF, ISRANGE, par1..par8
    if( parX == NULL) ThrowFromInternalUDSub( e,
     "Parameter is undefined: "  + e->Caller()->GetString(e->GetTheKW( prmbeg)));

    if( (*isRange)[0] == 1)  // r = hash[*]
      {
      if( parX->N_Elements() != 3) ThrowFromInternalUDSub( e, 
        "Range vector must have 3 elements: " + e->Caller()->GetString(e->GetTheKW( prmbeg)));
    
        MAKE_LONGGDL(parX, parXLong)

      if( (*parXLong)[0] != 0 || (*parXLong)[1] != -1 || (*parXLong)[2] != 1)
    ThrowFromInternalUDSub( e, "Subscript range is not allowed: [" + 
    i2s((*parXLong)[0])+":"+i2s((*parXLong)[1])+":"+i2s((*parXLong)[2])+"]");
        BaseGDL* cloned = hash_duplicate(self); 
        if(trace_me) std::cout << " cloned " <<std::endl;
            return cloned;
    } // (*isRange)[0] == 1
      
    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()
    
// Generalize from  self to theStruct
    BaseGDL* theref = parX;
    DStructGDL* theStruct = self;       
    int iprm = 0;
  
    SizeT listSize = 0;

    bool islist=false;
    bool ishash = true;
    bool isstruct = false;
    bool finalprm = false;
    
    while(  (*isRange)[iprm] == 0) { //  && (iprm+3 < nParam)
    
        finalprm = (iprm+prmbeg+1 == nParam);
        BaseGDL* XX = e->GetKW( iprm + prmbeg);
        if( XX == NULL) 
            ThrowFromInternalUDSub( e,       "Parameter is undefined " );
        if( XX->Rank() != 0 ) break; // must be a scalar

        if( islist) {
            MAKE_LONGGDL( XX, XXLong) 
            DLong Ixref = (*XXLong)[0];
            listSize = (*static_cast<DLongGDL*>(theStruct->GetTag( nListTag, 0)))[0];
            if(Ixref >= listSize) // this is certainly common.
                ThrowFromInternalUDSub( e, "direct access index is too high"
                + i2s(Ixref) + " >= nList:"+i2s(listSize));
            DStructGDL* Node = GetLISTStruct( e, GetLISTNode( e, theStruct, Ixref));
            DPtr Ptr = 
                (*static_cast<DPtrGDL*>( Node->GetTag( pListDataTag, 0)))[0];
            theref = BaseGDL::interpreter->GetHeap( Ptr);
            if(trace_me) std::cout << " islist iprm="<<iprm;
            if( theref == 0){
                if(trace_me)  std::cout << " theref==0 ";
                 return NullGDL::GetSingleInstance();
                }// return NullGDL::GetSingleInstance();
            // ok to return a null ptr
        } else if( ishash ) {
                if(trace_me) std::cout << " ishash iprm="<<iprm;
            DPtr Ptr = (*static_cast<DPtrGDL*>( 
                    theStruct->GetTag( pTableTag, 0)    ))[0];
            DStructGDL* hashTable = static_cast<DStructGDL*>(
                    BaseGDL::interpreter->GetHeap( Ptr));
                if(trace_me) std::cout << " . ";
            bool isfoldcase = Hashisfoldcase( theStruct); 
            if(XX->Type() == GDL_OBJ) {
                DObj p=(*static_cast<DObjGDL*>( XX))[0];
                if(p != 0) {
                    DStructGDL* ListHead = GetOBJ( XX, e);
    //                      DStructDesc* desc = ListHead->Desc();
                    if(ListHead->Desc()->IsParent("LIST")) {
                        theref = hash_subset(hashTable, XX, isfoldcase);
                        if( finalprm ) return theref;
                        if(trace_me) std::cout << " ...[XX...]" ;
                        }
                    else
                    ThrowFromInternalUDSub( e, " -XX- hash key an object not LIST");
                } else
                    ThrowFromInternalUDSub( e, " -XX- hash key an invalid object");
            } else {
                DLong hashIndex = HashIndex( hashTable , XX, isfoldcase);

                if(trace_me) std::cout <<" pData:"<<i2s(Ptr) ;
            if(trace_me) help_item(std::cout,XX, "XX",false);
                if(hashIndex < 0) 
                    ThrowFromInternalUDSub( e, " -XX- hash key not found ");
                DPtr pValue = (*static_cast<DPtrGDL*>( 
                        hashTable->GetTag( pValueTag, hashIndex)    ))[0];
                theref = BaseGDL::interpreter->GetHeap( pValue);
            }
        } else if( isstruct ) {
// use XX to access struct by naming the tag, or by its numerical index.
            int ptagindex = 0;
            if(XX->Type() != GDL_STRING) {
                MAKE_LONGGDL( XX, XXLong)
                ptagindex = (*XXLong)[0];
                if(ptagindex >= theStruct->Desc()->NTags() ) 
                    ptagindex = -1;
                }
            else {
                DString tag = (*static_cast<DStringGDL*>( XX))[0];
                StrUpCaseInplace(tag);
                ptagindex = theStruct->Desc()->TagIndex(tag);
            }
            if(ptagindex < 0) 
                ThrowFromInternalUDSub( e, " struct tag not found ");
            theref = theStruct->Get( ptagindex);
    
        } else {
            if(trace_me) std::cout << "isstruct || ishash || islist "<< iprm <<std::endl;
            break;} // isstruct || ishash || islist
// We've gotten past any list, hash,or struct/
        DType theType = theref->Type();
            if(trace_me) std::cout << " theType="<<theType;
// DO NOT EVER de-reference any pointers we find. This is *not* IDL behavior.
// in order to turn this off, a ptr scalar must go in as a ptrarr[1];
//
//        if( theType == GDL_PTR && theref->Rank() == 0) {
//            DPtr p=(*static_cast<DPtrGDL*>( theref))[0];
//              if( p != 0) theref = BaseGDL::interpreter->GetHeap( p);
//              theType = theref->Type();
//          }
// right bracket, we are done if only 3 parameters left (?).
        if(finalprm) {
            if(trace_me) help_item(std::cout, theref, " ->Dup():",false);
            return theref->Dup();
      }
        if(trace_me) help_item(std::cout, theref, " \n theref ",false);
// otherwise, another dimension is involved and we need to process again.
        islist = false;
        ishash = false;
        isstruct = false;
        if(theType == GDL_OBJ && theref->Rank() == 0) {
              DObj p=(*static_cast<DObjGDL*>( theref))[0];
              if(p == 0) break;
              theStruct = GetOBJ( theref, e);
              DStructDesc* desc = theStruct->Desc();
              islist = desc->IsParent("LIST");
              ishash = desc->IsParent("HASH");
              isstruct = !(islist or ishash);
            }
        else if( theType == GDL_STRUCT and
                theref->N_Elements() == 1) {
            theStruct = static_cast<DStructGDL*>( theref);
            isstruct = true;
    }
        iprm++;
        if( gdl_type_lookup::IsConvertableType[theType] ) break;
         } // while
  /*
 
 */
    if(trace_me) std::cout << " HASH_OL[R]: iprm="<< i2s(iprm);
    if(trace_me) lib::help_item(std::cout, theref," theref",false);

    if(!ishash) return list_rightextraction( e, theref, iprm);

// rewind to a simpler time ... hashnew=hashold[ keys ]
     if( nParam > 3)
      ThrowFromInternalUDSub( e, "Only one dimensional access allowed.");

    // non-range
        BaseGDL* index = parX;

    // one element -> return value
    if( index->N_Elements() == 1)
    {
      DLong hashIndex = HashIndex( thisHashTable, index, isfoldcase);
//      if( hashIndex < 0) ThrowFromInternalUDSub( e, "Key is not present.");
     if( hashIndex < 0) return NullGDL::GetSingleInstance();
      DPtr vID = (*static_cast<DPtrGDL*>(thisHashTable->GetTag( pValueTag, hashIndex)))[0];
      BaseGDL* value = e->Interpreter()->GetHeap( vID);
    if(trace_me) lib::help_item(std::cout, value," value",false);
      if( value == NULL)
    return NullGDL::GetSingleInstance();
      else  return value->Dup();
    } else return hash_subset(thisHashTable, index, isfoldcase);
    }

  BaseGDL* hash_fun( EnvT* e)
  {
    return hash_create( e , false);
  }

  BaseGDL* orderedhash_fun( EnvT* e)
  {
    BaseGDL* par = hash_create( e , true);
    static unsigned ordmask = 0x00000010;
    static unsigned TableBitsTag = structDesc::HASH->TagIndex( "TABLE_BITS");
    
    DObj s = (*static_cast<DObjGDL*>(par))[0]; // is StrictScalar()
    if( s == 0) e->Throw(" fail ( s == 0) in ordered hash! ");
    DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( s);
    if( oStructGDL == NULL) e->Throw(" fail ( struct == NULL) in ordered hash! ");
    DStructDesc* desc = oStructGDL->Desc();
    
    static unsigned TableBitsIx = desc->TagIndex("TABLE_BITS");
    (*static_cast<DLongGDL*>(oStructGDL->GetTag( TableBitsIx, 0)))[0] = ordmask;
    return par;
  }
} // namespace lib
  
static  BaseGDL* hash_create( EnvT* e, bool isordered=false)
  {
    static int kwNO_COPYIx = e->KeywordIx("NO_COPY");
    bool kwNO_COPY = false;
    if (e->KeywordSet(kwNO_COPYIx)){ kwNO_COPY = true;}
    static int kwFOLD_CASEIx = e->KeywordIx("FOLD_CASE");
    static unsigned fold_case_mask = 0x00000001;
    static int kwEXTRACTIx = e->KeywordIx("EXTRACT");   // still new
    trace_me = false; // lib::trace_arg();

    SizeT nParam = e->NParam();   

    ProgNodeP cN = e->CallingNode();
    DInterpreter* ip = e->Interpreter();
  
    GDL_HASH_STRUCT()
    GDL_HASHTABLEENTRY()

    // because of .RESET_SESSION, we cannot use static here
    DStructDesc* hashDesc=structDesc::HASH;
    DStructDesc* entryDesc=structDesc::GDL_HASHTABLEENTRY;
    assert( hashDesc != NULL && hashDesc->NTags() > 0);
    assert( entryDesc != NULL && entryDesc->NTags() > 0);
    
     bool foldcasekw = e->KeywordSet( kwFOLD_CASEIx);
     bool extractkw = e->KeywordSet( kwEXTRACTIx);

    GDL_LIST_STRUCT()
    GDL_CONTAINER_NODE()

    BaseGDL* key;
    SizeT nEntries = 0;
    if( nParam == 1) {
      key = e->GetPar( 0);
      if( key == NULL || key == NullGDL::GetSingleInstance())
                e->Throw(" invalid 1st parameter ");
      if(trace_me) std::cout << " hash(nParam=1) ";
      if(  key->Type() == GDL_STRUCT ) {
                if(trace_me) std::cout << " . " ;
            return structP_tohash( e, key, foldcasekw, extractkw, isordered);
          }// direct return for the case of struct --> hash
      else if ( key->Type() == GDL_OBJ and key->StrictScalar())
      {    // GDL extension: put object structure directly into a hash.
            DStructGDL* oStructGDL= GetOBJ( key, NULL);
            return struct_tohash( e, oStructGDL, foldcasekw, extractkw, isordered);
        }   else {  // key->Type() == GDL_STRUCT || key->Type() == GDL_OBJ
         // 1-element hash table, value=null);
         nEntries = key->N_Elements();
        if(trace_me) std::cout << " nEntries= " << nEntries;
        }
      } else if( nParam == 2) { //ckeck key/value similar number of elements
		if ((e->GetPar(0))->N_Elements() != (e->GetPar(1))->N_Elements()) e->Throw("Key and Value must have the same number of elements.");
	  } else { 
	  if (nParam % 2 != 0) e->Throw("Incorrect number of arguments.");
      }
// Count the possible entries, no check for total correctness.
    for( SizeT eIx=0; eIx < nParam/2; ++eIx) {
        BaseGDL* key = e->GetPar(  2 * eIx);
        if( key == NULL || key == NullGDL::GetSingleInstance())
            continue;
        if ( key->Type() == GDL_OBJ and key->StrictScalar()) {
            DStructGDL* theList = GetOBJ( key, NULL);
            DStructDesc* desc = theList->Desc();
            if(desc->IsParent("LIST")) nEntries += 
                (*static_cast<DLongGDL*>( theList->GetTag( nListTag, 0)))[0];
            }
        else if( key->Type() == GDL_STRUCT )
                e->Throw("Only 1 argument is allowed with input of type STRUCT.");
        else nEntries += key->N_Elements();
        }
    if(trace_me) std::cout << " hash_create: counted nEntries=" << nEntries << std::endl;
  
    DStructGDL* hashTable;
    DLong initialTableSize = GetInitialTableSize( nEntries);
//    DStructGDL* hashStruct = 
//      create_hashStruct( initialTableSize, hashTable, foldcasekw, isordered);   
//  DObj objID= e->NewObjHeap( 1, hashStruct); // owns hashStruct, sets ref count to 1 
    DObj objID= 
        new_hashStruct( initialTableSize, hashTable, foldcasekw, isordered);
    BaseGDL* newObj = new DObjGDL( objID); // the hash object
    Guard<BaseGDL> newObjGuard( newObj);
    DStructGDL* hashStruct = GetOBJ( newObj, 0);    
    if( nParam == 1) {
        if( key->N_Elements() == 1) {
            InsertIntoHashTable( hashStruct, hashTable, key, NullGDL::GetSingleInstance());
            } else {
            for( SizeT kIx=0; kIx < nEntries; ++kIx)
                InsertIntoHashTable( hashStruct, hashTable, key->NewIx(kIx), NullGDL::GetSingleInstance());
            }
        newObjGuard.Release();
        return newObj;
        }

    for( SizeT eIx=0; eIx < nParam/2; ++eIx)
    {
        SizeT keyIx = 2 * eIx;
        SizeT valueIx = 2 * eIx + 1;
        BaseGDL* key = e->GetPar( keyIx);
        bool iskeylist=false;
        // !NULL keys are not inserted
        if( key == NULL || key == NullGDL::GetSingleInstance())
                                continue;       // --- (to change)
        if(trace_me ) {
            std::cout << i2s(eIx) << " key:";
            lib::help_item(std::cout , key, " key for hash", false);
        }

        SizeT nKey = key->N_Elements();
        DPtr keyNode = 0;
        DPtr valNode = 0;
        if ( key->Type() == GDL_OBJ and key->StrictScalar()) {
            DStructGDL* keyList = GetOBJ( key, NULL);
            DStructDesc* desc = keyList->Desc();
            iskeylist = desc->IsParent("LIST");
            if(iskeylist) {
                nKey = 
                (*static_cast<DLongGDL*>( keyList->GetTag( nListTag, 0)))[0];
                keyNode = GetLISTNode(NULL, keyList, 0);
                }
            else
                e->Throw("only objects allowed for keys or values are LISTs");
            }
        else if( key->Type() == GDL_STRUCT )
                e->Throw("Only 1 argument is allowed with input of type STRUCT.");

        BaseGDL* value = e->GetPar( valueIx);
        SizeT nValue = 1;
        bool isvallist = false;
        bool isvalscalar = false;
        if( value == NULL || value == NullGDL::GetSingleInstance())
            isvalscalar = true;
        else
            nValue = value->N_Elements();
            if ( value->Type() == GDL_OBJ and value->StrictScalar()) {
                isvalscalar = true;
                DStructGDL* valList = GetOBJ( value, NULL);
                DStructDesc* desc = valList->Desc();
                isvallist = desc->IsParent("LIST");
                if(isvallist) {
                    nValue = 
                    (*static_cast<DLongGDL*>( valList->GetTag( nListTag, 0)))[0];
                    valNode = GetLISTNode(NULL, valList, 0);
                    isvalscalar = false;
                if(trace_me) 
                std::cout <<"HASH: isvallist=true, nvalue="<<nValue<<std::endl;
                    }
                }  // GDL_OBJ and value->StrictScalar
            else if( value->Type() == GDL_STRUCT )
                    e->Throw("Only 1 argument is allowed with input of type STRUCT.");
            else if( value->StrictScalar() )
                    isvalscalar = true;;
    if(trace_me) std::cout << " isvalscalar/ iskeylist? nkey:"<<
                isvalscalar<< iskeylist << nKey << std::endl;
        if( ( !isvalscalar) &&
            ( nValue != 0 && nKey != 1 && nValue != nKey) )
          e->Throw( "Key and Value must have the same number of elements.");
// Allowing a list of key and/or a list of values
// leaves an ambiguity for the case of a single addition        
        if( nKey == 1) {
                if(isvallist && nValue == 1) value = GetNodeData(valNode);
                if(iskeylist) key   = GetNodeData(keyNode);
              if( !kwNO_COPY && value != NULL)
                            value = value->Dup();
              InsertIntoHashTable( hashStruct, hashTable, key, value);
            }
         else // nkey > 1
            {
              if( isvalscalar)
              {
                if(iskeylist) 
                    for( SizeT kIx=0; kIx<nKey; ++kIx)
                  InsertIntoHashTable( hashStruct, hashTable,
                            GetNodeData(keyNode), value->Dup());
                else     
                    for( SizeT kIx=0; kIx<nKey; ++kIx)
                  InsertIntoHashTable( hashStruct, hashTable,
                                key->NewIx(kIx), value->Dup());
              }
              else
              {         // value not a scalar.
                if(iskeylist) {
                    if(isvallist) {
                        for( SizeT kIx=0; kIx<nKey; ++kIx)
                        InsertIntoHashTable( hashStruct, hashTable, 
                            GetNodeData(keyNode), (GetNodeData(valNode))->Dup());
                    } else {
                        for( SizeT kIx=0; kIx<nKey; ++kIx)
                        InsertIntoHashTable( hashStruct, hashTable, 
                            GetNodeData(keyNode), value->NewIx(kIx));
                    }
                } else {
                    if(isvallist) {
                        for( SizeT kIx=0; kIx<nKey; ++kIx)
                        InsertIntoHashTable( hashStruct, hashTable, 
                            key->NewIx(kIx), (GetNodeData(valNode))->Dup());
                    } else {
                        for( SizeT kIx=0; kIx<nKey; ++kIx)
                        InsertIntoHashTable( hashStruct, hashTable,
                            key->NewIx(kIx), value->NewIx(kIx));
                        }
                    }
              }
            }
        if( kwNO_COPY)
            {
              bool stolen = e->StealLocalPar( valueIx);
              if( !stolen) e->GetPar(valueIx) = NULL;
            }

    }
  
    newObjGuard.Release();
    return newObj;
  } 
