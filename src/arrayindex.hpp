/***************************************************************************
                          arrayindex.hpp  -  array access descriptor
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#ifndef ARRAYINDEX_HPP_
#define ARRAYINDEX_HPP_

#include <valarray>
#include <vector>

#include "datatypes.hpp"
#include "real2int.hpp"

// done below
// #include "allix.hpp"



//typedef std::valarray<SizeT> AllIxT; // now in typedefs.hpp
//typedef std::vector<BaseGDL*>     IxExprListT;
class IxExprListT
{
private:
BaseGDL* eArr[3*MAXRANK];
int sz;
public:
IxExprListT(): sz(0) {}
void push_back( BaseGDL* p) { assert( sz<3*MAXRANK); eArr[ sz++] = p;}
BaseGDL* operator[]( SizeT i) const { assert( i<3*MAXRANK && i<sz); return eArr[i];}
SizeT size() const { return sz;}
void Cleanup() { for( int i=0; i<sz; ++i) delete eArr[i]; sz=0;}
IxExprListT& operator=(IxExprListT&r) { sz = r.sz; for(int i=0; i<sz; ++i) eArr[i] = r.eArr[i]; return r;}
};

enum IndexType
{
ArrayIndexTID, // abstract
 ArrayIndexScalarID,   // scalar
CArrayIndexScalarID, // with const e. g. [42]
ArrayIndexScalarVPID, // common blobck or sysvar
 ArrayIndexIndexedID,   // indexed
CArrayIndexIndexedID, // indexed with const
ArrayIndexAllID,             // [*]
 ArrayIndexORangeID,   // [3:*]
CArrayIndexORangeID,  // with const
 ArrayIndexRangeID,      // [a:b]
CArrayIndexRangeID,     // with const
 ArrayIndexORangeSID,  // [a:*:stride]
CArrayIndexORangeSID, // with const
 ArrayIndexRangeSID,     // [a:b:stride]
CArrayIndexRangeSID     // with const
};

class ArrayIndexT
{
public:
 virtual IndexType Type() { return ArrayIndexTID;}

// this may be called from ArrayIndexListT::ToAssocIndex
  virtual void Init() {} 
  
  // the following should never be called
  virtual void Init( BaseGDL*) { assert( false);}
  virtual void Init( BaseGDL*, BaseGDL*) { assert( false);}
  virtual void Init( BaseGDL*, BaseGDL*, BaseGDL*) { assert( false);}

  virtual bool IsRange() { return false;} // default for non-ranges
  virtual BaseGDL* OverloadIndexNew() { assert( false);}
  virtual BaseGDL* OverloadIndexNew( BaseGDL*) { assert( false);}
  virtual BaseGDL* OverloadIndexNew( BaseGDL*, BaseGDL*) { assert( false);}
  virtual BaseGDL* OverloadIndexNew( BaseGDL*, BaseGDL*, BaseGDL*) { assert( false);}

  virtual void Clear() {}
  virtual ~ArrayIndexT() {}

  virtual RangeT GetIx0()=0;
  virtual RangeT GetS() { return 0;}
  virtual SizeT GetStride() { return 1;} // changed from 0
  
  virtual bool Scalar()  const         { return false;}
//   virtual bool Scalar( SizeT& s_) const {  return false;}
  virtual bool Scalar( RangeT& s_) const {  return false;}
  
  virtual bool Indexed()          { return false;}  

//  virtual BaseGDL* Index( BaseGDL* var, IxExprListT& ix)=0;

  virtual SizeT NIter( SizeT varDim)=0; 

  virtual SizeT NParam()=0;

  virtual ArrayIndexT* Dup() const =0;
};

class ArrayIndexVectorT
{
private:
ArrayIndexT* arrayIxArr[ MAXRANK];
SizeT             sz;
public:
ArrayIndexVectorT(): sz(0) {}
~ArrayIndexVectorT()
{} // for( int i=0; i<sz; ++i) delete arrayIxArr[ i];}
ArrayIndexVectorT( const ArrayIndexVectorT& cp): sz( cp.sz)
{
for( SizeT i=0; i<sz;++i)
	arrayIxArr[ i] = cp.arrayIxArr[ i];
}
SizeT FrontGetS() const
{
assert( sz > 0);
return arrayIxArr[0]->GetS();
}

ArrayIndexT* operator[]( SizeT ix) const
{
assert( ix < MAXRANK);
return arrayIxArr[ ix];
}
SizeT size() const { return sz;}
void push_back( ArrayIndexT* aIx)
{
assert( sz < MAXRANK);
arrayIxArr[ sz++] = aIx;
}
void Clear()
{
for( int i=0; i<sz; ++i)
	arrayIxArr[ i]->Clear();
}
void Destruct()  // only to be used from destructor (instance is not valid anymore afterwards)
{
for( int i=0; i<sz; ++i)
	delete arrayIxArr[ i];
 //sz = 0;	
}
ArrayIndexT* back() const { return arrayIxArr[ sz-1];}
void pop_back() { --sz;}
ArrayIndexT* pop_back_get() { --sz; return arrayIxArr[ sz];}
};

#include "allix.hpp"

// SCALAR (only for FOR loop indices)
// VAR
class ArrayIndexScalar: public ArrayIndexT
{
protected:
  SizeT varIx;

  RangeT sInit;
  RangeT s;

public:
 IndexType Type() { return ArrayIndexScalarID;}
//   SizeT GetIx( SizeT i)
//   {
// 	assert( ix != NULL);
//     return (*ix)[ i];
//   }
  BaseGDL* OverloadIndexNew(); 

  SizeT GetVarIx() const { return varIx;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  RangeT GetS() { return s;}

  bool Scalar() const { return true;}
  bool Scalar( RangeT& s_) const
  { 
    s_ = s;
    return true;
  }

  RangeT GetIx0()
  {
    return s;
  }

  ~ArrayIndexScalar() 
  {}

  ArrayIndexScalar( RefDNode& dNode);

  // c-i
  ArrayIndexScalar( const ArrayIndexScalar& r):
    varIx( r.varIx), 
    sInit( r.sInit),
    s( r.s)
  {}

  ArrayIndexT* Dup() const
  {
    return new ArrayIndexScalar(*this);
  }
 
  void Init();

  void Clear()
  {}

  // if this is used, Init was NOT called before
//   BaseGDL* Index( BaseGDL* var, IxExprListT& ixL);

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim);
};
// VARPTR (common block variable)
class ArrayIndexScalarVP: public ArrayIndexT
{
protected:
  DVar* varPtr;

  RangeT sInit;
  RangeT s;

public:
 IndexType Type() { return ArrayIndexScalarVPID;}

 BaseGDL* OverloadIndexNew() 
  { 
    BaseGDL* v = varPtr->Data();
    if( v == NULL) return NULL;
    return v->Dup();
  }

  DVar* GetVarPtr() const { return varPtr;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  RangeT GetS() { return s;}

  bool Scalar() const { return true;}
  bool Scalar( RangeT& s_) const
  { 
    s_ = s;
    return true;
  }

  void Init() 
  {
    sInit = varPtr->Data()->LoopIndex();
    s = sInit;
  }

  void Clear()
  {}

  RangeT GetIx0()
  {
    return s;
  }

  ~ArrayIndexScalarVP() 
  {}

  ArrayIndexScalarVP( RefDNode& dNode);

  // c-i
  ArrayIndexScalarVP( const ArrayIndexScalarVP& r):
    varPtr( r.varPtr), 
    s( r.s)
  {}

  ArrayIndexT* Dup() const
  {
    return new ArrayIndexScalarVP(*this);
  }
 
  // if this is used, Init was NOT called before
//   BaseGDL* Index( BaseGDL* var, IxExprListT& ixL);

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim);
};





// constant SCALAR
class CArrayIndexScalar: public ArrayIndexT
{
private:
  RangeT sInit; // can be < 0 as well
  RangeT s;
  BaseGDL* rawData; // for overloaded object indexing

  CArrayIndexScalar( const CArrayIndexScalar& c): s( c.s), sInit( c.sInit) 
  {
    assert( c.rawData != NULL);
    rawData = c.rawData->Dup();
  }

public:
  IndexType Type() { return CArrayIndexScalarID;}

  BaseGDL* OverloadIndexNew() 
  { 
    assert( rawData != NULL);
    return rawData->Dup();
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  BaseGDL* StealRawData() { BaseGDL* r = rawData; rawData = NULL; return r;}
  
  bool Scalar() const { return true;}
  bool Scalar( RangeT& s_) const
  { 
    s_ = s;
    return true;
  }

  RangeT GetIx0()
  {
    return s;
  }

  RangeT GetS() { return s;}

  ~CArrayIndexScalar() 
  {
    delete rawData;
  }

  // grabs c
  CArrayIndexScalar( BaseGDL* c)
  : rawData(c)
  {
//     if( c->Type() == GDL_STRING)
//     {
//       DStringGDL* cString = static_cast<DStringGDL*>(c);
//       if( (*cString)[0] == "")
//       {
// 	s = 0;
//       }
//       else
//       {
// 	const char* cStart=(*cString)[0].c_str();
// 	char* cEnd;
// 	RangeT ix=strtol(cStart,&cEnd,10);
// 	if( cEnd == cStart)
// 	  {
// 	    // in LoopIndex() just a warning is printed
// 	    // An exception is necessary as then ArrayIndexScalar
// 	    // is used instead (see: gdlc.tree.g: arrayindex)
// 	    throw GDLException("Type conversion error: "
// 	       "Unable to convert given STRING: '"+
// 	       (*cString)[0]+"' to index.");
// 	  }
// 	s = ix;
//       }
//     }
//     else
      sInit = c->LoopIndex(); // non STRING throw if not allowed
      s = sInit;
  }

  CArrayIndexScalar( RangeT s_): sInit( s_), s( s_), rawData(NULL)  
  {}

  ArrayIndexT* Dup() const
  {
    return new CArrayIndexScalar( *this);
  }
 
  void Clear()
  {}
  void Init()
  {}

//   // if this is used, Init was NOT called before
//   BaseGDL* Index( BaseGDL* var, IxExprListT& ixL)
//   {
//     if( s >= var->Size())
//       throw GDLException(NULL,"Scalar subscript out of range [>].h1",true,false);
//     return var->NewIx( s);
//   }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim) 
  {
    if( sInit < 0)
      s = sInit + varDim;
    else
      s = sInit;

    if( s < 0)
	throw GDLException(NULL,"Constant scalar subscript out of range [-i].",true,false);
    if( s >= varDim && s > 0) // varDim == 0 && s == 0 ok
	throw GDLException(NULL,"Constant scalar out of range [i].",true,false);
    return 1;

//     if( s >= varDim && s > 0) // varDim == 0 && s == 0 ok
//       throw GDLException(NULL,"Scalar subscript out of range [>].h2",true,false);
//     return 1;
  }
}; //class CArrayIndexScalar: public ArrayIndexT





// INDEXED or ONE [v] (must handle both)
class ArrayIndexIndexed: public ArrayIndexT
{
protected:
  bool      strictArrSubs;          // for compile_opt STRICTARRSUBS
  
  RangeT sInit; // can be < 0 as well
  RangeT s;
//  SizeT maxVal;

  AllIxIndicesT*    ix;
  char ixBuf[ AllIxMaxSize];
  
  const dimension* ixDim; // keep dimension of ix

  // forbid c-i
  ArrayIndexIndexed( const ArrayIndexT& r) {}

public:
  IndexType Type() { return ArrayIndexIndexedID;}

  BaseGDL* OverloadIndexNew( BaseGDL* p1) 
  { 
    if( p1 == NULL) return NULL;
    return p1->Dup();
  }

  SizeT NParam() { return 1;} // number of parameter to Init(...)

  RangeT GetS() { return s;}

  bool Scalar() const { return (ix == NULL);}
  bool Scalar( RangeT& s_) const // changed from RangeT for proper overloading
  {
    if( ix == NULL)
    {
	    s_ = s;
	    return true;
    }
    s_ = (*ix)[0];
    return (ix->size() == 1);
}

  bool Indexed() { return (ix != NULL);}

  const dimension& GetDim() { assert(ixDim != NULL); return *ixDim;}

  RangeT GetIx0()
  {
	  if( ix != NULL) return (*ix)[0]; // from array
	  return s;
  }

  SizeT GetIx( SizeT i)
  {
	  assert( ix != NULL);
	  return (*ix)[ i];
  }

  //  SizeT* StealIx() { SizeT* ret = ix; ix = NULL; return ret;}
  //AllIxIndicesT* StealIx() { AllIxIndicesT* ret = ix; ix = NULL; return ret;}
  AllIxIndicesT* GetAllIx() const { return ix;}

  ~ArrayIndexIndexed()
  {
	  //     delete ix;
	  //     delete ixDim;
  }

  ArrayIndexIndexed( bool strictArrSubs_ = false):
  strictArrSubs( strictArrSubs_),
  //    maxVal( 0),
  ix( NULL), ixDim( NULL)
  {}

  ArrayIndexT* Dup() const
  {
    ArrayIndexIndexed* d =  new ArrayIndexIndexed( strictArrSubs);

    assert( ix == NULL);
    assert( ixDim == NULL);

    d->sInit = sInit;
    d->s = s;
    //     d->maxVal = maxVal;

    return d;
  }

  void Clear()
  {
    //     maxVal = 0;
    //     delete ixDim;
    ixDim = NULL;
    //     delete ix; 
    ix = NULL; // marker ONE or INDEXED
  }

  void Init( BaseGDL* ix_)   
  {
    if( ix_->Rank() == 0) // type ONE
    {
      ix_->Scalar2RangeT(sInit);
      s = sInit; // in case of assoc NIter is not called
      // int ret = ix_->Scalar2RangeT(s);
      // from GDL 0.9 on negative indices are fine
      // 	if( ret == -1) // index < 0
      // 	  {
      // 	    throw 
      // 	      GDLException(NULL, "Subscript range values of the"
      // 			    " form low:high must be >= 0, < size,"
      // 			    " with low <= high.",true,false);
      // 	  }
      return;
    }

    // type INDEXED
    DType dType = ix_->Type();

    assert( dType != GDL_UNDEF);
    //     assert( maxVal == 0);

    int typeCheck = DTypeOrder[ dType];
    if( typeCheck >= 100)
      throw GDLException(NULL,"Type not allowed as subscript.",true,false);

    //SizeT nElem = ix_->N_Elements();
    //    ix = new SizeT[ nElem]; // allocate array

  //DEBUG if( ix != NULL)    
    assert( ix == NULL);

    //     ix = new AllIxMultiT( nElem);
    if( strictArrSubs)
      ix = new (ixBuf) AllIxIndicesStrictT( ix_);
    else
      ix = new (ixBuf) AllIxIndicesT( ix_);

    ixDim = &ix_->Dim();
  }

  // number of iterations
  // also checks/adjusts range
  SizeT NIter( SizeT varDim)
  {
    if( ix == NULL) // ONE
    {
      if( sInit < 0)
	s = sInit + varDim;
      else
	s = sInit;

      if( s < 0)
	throw GDLException(NULL,"Subscript out of range [-i].",true,false);
      if( s >= varDim && s > 0)
	throw GDLException(NULL,"Subscript out of range [i].",true,false);
      return 1;
    }
    // INDEXED
    ix->SetUpper( varDim-1);
    return ix->size();
  }
}; // class ArrayIndexIndexed: public ArrayIndexT

// INDEXED or ONE [v] (must handle both)
class CArrayIndexIndexed: public ArrayIndexT
{
private:
  bool   strictArrSubs; // for compile_opt STRICTARRSUBS
  
  RangeT sInit; // can be < 0 as well
  RangeT s;

  AllIxIndicesT*    ix;
  char ixBuf[ AllIxMaxSize];
  
  const dimension* ixDim; // keep dimension of ix

  // forbid c-i
  CArrayIndexIndexed( const ArrayIndexT& r) { assert(false);}

  BaseGDL* rawData;
  bool     isScalar;
  
public:
  IndexType Type() { return CArrayIndexIndexedID;}

  BaseGDL* OverloadIndexNew() 
  { 
    assert( rawData != NULL);
    return rawData->Dup();
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  RangeT GetS() 
  {     
    return s;
  }

  bool Scalar() const { return isScalar;}
  bool Scalar( RangeT& s_) const // changed from RangeT for proper overloading
  {
    if( isScalar)
    {
      s_ = s;
      return true;
    }
    s_ = (*ix)[0];
    return (ix->size() == 1);
}

  bool Indexed() { return !isScalar;}

  const dimension& GetDim() { assert(ixDim != NULL); return *ixDim;}

  RangeT GetIx0()
  {
    if( !isScalar) return (*ix)[0]; // from array
    return s;
  }

  SizeT GetIx( SizeT i)
  {
    assert( ix != NULL);
    return (*ix)[ i];
  }

  //  SizeT* StealIx() { SizeT* ret = ix; ix = NULL; return ret;}
  //AllIxIndicesT* StealIx() { AllIxIndicesT* ret = ix; ix = NULL; return ret;}
  AllIxIndicesT* GetAllIx() const 
  { 
    return ix;
  }

  ~CArrayIndexIndexed()
  {
    delete rawData;
  }

  // grabs c
  CArrayIndexIndexed( BaseGDL* c, bool strictArrSubs_ = false)
  : strictArrSubs( strictArrSubs_)
  , ix( NULL), ixDim( NULL)
  , rawData( c)
  {
    assert( rawData != NULL);
    
    if( rawData->Rank() == 0) // type ONE
    {
      rawData->Scalar2RangeT(sInit);
      s = sInit; // in case of assoc NIter is not called
      isScalar = true;
      return;
    }

    // type INDEXED
    isScalar = false;;
    
    ixDim = &rawData->Dim();

    assert( rawData->Type() != GDL_UNDEF);
    DType dType = rawData->Type();
    int typeCheck = DTypeOrder[ dType];
    if( typeCheck >= 100)
      throw GDLException(NULL,"Type not allowed as subscript.",true,false);

    if( strictArrSubs)
      ix = new (ixBuf) AllIxIndicesStrictT( rawData);
    else
      ix = new (ixBuf) AllIxIndicesT( rawData);
  }

  ArrayIndexT* Dup() const
  {
    return new CArrayIndexIndexed( rawData->Dup(), strictArrSubs);
  }

  void Clear()
  {} // nothing to clear

  void Init()   
  {} // already initialized in constructor

  // number of iterations
  // also checks/adjusts range
  SizeT NIter( SizeT varDim)
  {
    if( isScalar) // ONE
    {
      if( sInit < 0)
	s = sInit + varDim;
      else
	s = sInit;

      if( s < 0)
	throw GDLException(NULL,"Subscript out of range [-i].",true,false);
      if( s >= varDim && s > 0)
	throw GDLException(NULL,"Subscript out of range [i].",true,false);
      return 1;
    }
    // INDEXED
    ix->SetUpper( varDim-1);
    return ix->size();
  }
}; //class CArrayIndexIndexed: public ArrayIndexIndexed



// [*]
class ArrayIndexAll: public ArrayIndexT
{
public:
 IndexType Type() { return ArrayIndexAllID;}

 bool IsRange() { return true;}
 
 BaseGDL* OverloadIndexNew()
  {
    const DLong arr[3] = {0,-1,1};
    return new DLongGDL( arr, 3);
  }
 
  SizeT NParam() { return 0;} // number of parameter to Init(...)

  void Init() {};

  RangeT GetIx0() { return 0;}

  ArrayIndexT* Dup() const
  {
    return new ArrayIndexAll();
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
    return varDim;
  }
};



// [s:*]
class ArrayIndexORange: public ArrayIndexT
{
protected:
  RangeT sInit;
  RangeT s;

public:
 IndexType Type() { return ArrayIndexORangeID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew( BaseGDL* s_) 
  {
    Init( s_);
    DLong arr[3] = {sInit,-1,1};
    return new DLongGDL( arr, 3);
  }
  
  SizeT NParam() { return 1;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexORange* d = new ArrayIndexORange();
    d->sInit = sInit;
    d->s = s;
    return d;
  }

   // s is always scalar here
  void Init( BaseGDL* s_)
  {
    int retMsg=s_->Scalar2RangeT(sInit);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException(NULL,"Internal error: Scalar2RangeT:"
			  " 1st index empty",true,false); 
	else
	  throw 
	    GDLException(NULL,"Expression must be a scalar"
			  " in this context.",true,false); 
      }
// not with Scalar2RangeT():
//     if( retMsg == -1) // index < 0
//       {
// 	throw 
// 	  GDLException(NULL,"Subscript range values of the"
// 			" form low:high must be >= 0, < size, "
// 			"with low <= high.",true,false);
//       }
  }

  SizeT NIter( SizeT varDim)
  {
    if( sInit >= varDim) // && s > 0)
      throw GDLException(NULL,"Subscript out of range [s:*].",true,false);
    if( sInit < 0)
    {
      s = sInit + varDim;
      if( s < 0)
	      throw GDLException(NULL,"Subscript out of range [-s:*].",true,false);
	  
      return (varDim - s);
    }
    s = sInit;
    return (varDim - s);
  }
};



class CArrayIndexORange: public ArrayIndexORange
{
public:
 IndexType Type() { return CArrayIndexORangeID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew()
  {
    DLong arr[3] = {sInit,-1,1};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexORange( BaseGDL* c): ArrayIndexORange()
  {
    ArrayIndexORange::Init( c);
  }

  CArrayIndexORange() {}

  ArrayIndexT* Dup() const
  {
    CArrayIndexORange* d = new CArrayIndexORange();
    d->sInit = sInit;
    d->s = s;
    return d;
  }

};



// [s:e]
class ArrayIndexRange: public ArrayIndexT
{
protected:
  RangeT sInit,eInit;
  RangeT s,e;

public:
 IndexType Type() { return ArrayIndexRangeID;}

  bool IsRange() { return true;}
  
  BaseGDL* OverloadIndexNew( BaseGDL* s_, BaseGDL* e_) 
  {
    Init( s_, e_);
    DLong arr[3] = {sInit,eInit,1};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 2;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexRange* d = new ArrayIndexRange();
    d->sInit = sInit;
    d->eInit = eInit;
    d->s = s;
    d->e = e;
    return d;
  }

  void Init( BaseGDL* s_, BaseGDL* e_)
  {
// 	SizeT varSize = var->Size();
    
    int retMsg=s_->Scalar2RangeT(sInit);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException(NULL,"Internal error: Scalar2RangeT: 1st index empty.",true,false);
	else
	  throw 
	    GDLException(NULL,"Expression must be a scalar in this context.",true,false); 
      }
//     if( retMsg == -1) // index < 0
//       {
// 	throw 
// 	  GDLException(NULL,"Subscript range values of the form low:high " 
// 			"must be >= 0, < size, with low <= high.",true,false);
//       }
    
    retMsg=e_->Scalar2RangeT(eInit);
    if( retMsg == 0) // index empty or array
      {
	if( e_->N_Elements() == 0)
	  throw 
	    GDLException(NULL,"Internal error: Scalar2RangeT: 2nd index empty.",true,false);
	else
	  throw 
	    GDLException(NULL,"Expression must be a scalar in this context.",true,false); 
      }
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
// 	RangeT sl,el;
    if( sInit < 0)
    {
	    s = sInit + varDim;
	    if( s < 0)
		    throw GDLException(NULL,"Subscript out of range [S:e].",true,false);
    }
    else
	    s = sInit;
    if( eInit < 0)
    {
	    e = eInit + varDim;
	    if( e < 0)
		    throw GDLException(NULL,"Subscript out of range [s:E].",true,false);
    }
    else
	    e = eInit;

    if( s > e)
      throw 
	GDLException(NULL,"Subscript range values of the form low:high "
		"must be < size, with low <= high",true,false);
    if( e >= varDim) // && e > 0)
		throw GDLException(NULL,"Subscript out of range [s:e].",true,false);
    return (e - s + 1);
  }
};



class CArrayIndexRange: public ArrayIndexRange
{
public:
 IndexType Type() { return CArrayIndexRangeID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew()
  {
    DLong arr[3] = {sInit,eInit,1};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexRange( BaseGDL* c1, BaseGDL* c2): ArrayIndexRange()
  {
    ArrayIndexRange::Init( c1, c2);
  }

  CArrayIndexRange(){}

  ArrayIndexT* Dup() const
  {
    CArrayIndexRange* d = new CArrayIndexRange();
    d->sInit = sInit;
    d->eInit = eInit;
    d->s = s;
    d->e = e;
    return d;
  }

};



// [s:*:st]
class ArrayIndexORangeS: public ArrayIndexT
{
protected:
  RangeT sInit;
  RangeT s;
  SizeT stride;

public:
 IndexType Type() { return ArrayIndexORangeSID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew( BaseGDL* s_, BaseGDL* stride_) 
  {
    Init( s_, stride_);
    DLong arr[3] = {sInit,-1,stride};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 2;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  SizeT GetStride() { return stride;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexORangeS* d = new ArrayIndexORangeS();
    d->sInit = sInit;
    d->s = s;
    d->stride = stride;
    return d;
  }

  void Init( BaseGDL* s_, BaseGDL* stride_)
  {
    int retMsg=s_->Scalar2RangeT( sInit);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	throw
		GDLException(  "Internal error: Scalar2RangeT:"
			" 1st index empty",true,false);
	else
	throw
		GDLException(  "Expression must be a scalar"
			" in this context.",true,false);
      }
//     if( retMsg == -1) // index < 0
//       {
// 	throw 
// 	  GDLException(  "Subscript range values of the"
// 			" form low:high must be >= 0, < size, with low <= high.",true,false);
//       }
    // stride
    retMsg=stride_->Scalar2index( stride);
    if( retMsg == 0) // index empty or array
      {
			if( stride_->N_Elements() == 0)
			throw
				GDLException(  "Internal error: Scalar2index:"
					" stride index empty",true,false);
			else
			throw
				GDLException(  "Expression must be a scalar"
					" in this context.",true,false);
      }
    if( retMsg == -1 || stride == 0) // stride <= 0
      {
			throw 
			GDLException(  "Range subscript stride must be >= 1.",true,false);
      }
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
    if( sInit < 0)
      {
	s = sInit + varDim;
	if( s < 0)
		throw GDLException(NULL,"Subscript out of range [-S:*:stride].",true,false);
	return (varDim - s + stride - 1)/stride;
      }
    else
      s= sInit;

    if( s >= varDim) // && s > 0)
      throw GDLException(NULL,"Subscript out of range [s:*:stride].",true,false);
    return (varDim - s + stride - 1)/stride;
  }
};

class CArrayIndexORangeS: public ArrayIndexORangeS
{
public:
 IndexType Type() { return CArrayIndexORangeSID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew()
  {
    DLong arr[3] = {sInit,-1,stride};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexORangeS( BaseGDL* c1, BaseGDL* c2): ArrayIndexORangeS()
  {
    ArrayIndexORangeS::Init( c1, c2);
  }

  CArrayIndexORangeS(){}

  ArrayIndexT* Dup() const
  {
    CArrayIndexORangeS* d = new CArrayIndexORangeS();
    d->sInit = sInit;
    d->s = s;
    d->stride = stride;
    return d;
  }

};

// [s:e:st]
class ArrayIndexRangeS: public ArrayIndexT
{
protected:
  RangeT sInit,eInit;
  RangeT s,e;
  SizeT stride;

public:
 IndexType Type() { return ArrayIndexRangeSID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew( BaseGDL* s_, BaseGDL* e_, BaseGDL* stride_)
  {
    Init( s_, e_, stride_);
    DLong arr[3] = {sInit,eInit,stride};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 3;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  SizeT GetStride() { return stride;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexRangeS* d = new ArrayIndexRangeS();
    d->sInit = sInit;
    d->eInit = eInit;
    d->s = s;
    d->e = e;
    d->stride = stride;
    return d;
  }

  void Init( BaseGDL* s_, BaseGDL* e_, BaseGDL* stride_)
  {
    int retMsg=s_->Scalar2RangeT(sInit);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2RangeT: 1st index empty.",true,false);
	else
	  throw 
	    GDLException(  "Expression must be a scalar in this context.",true,false); 
      }
//     if( retMsg == -1) // index < 0
//       {
// 	throw 
// 	  GDLException(  "Subscript range values of the form low:high " 
// 			"must be >= 0, < size, with low <= high.",true,false);
//       }
            
    retMsg=e_->Scalar2RangeT(eInit);
    if( retMsg == 0) // index empty or array
      {
	if( e_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2RangeT: 2nd index empty.",true,false);
	else
	  throw 
	    GDLException(  "Expression must be a scalar in this context.",true,false); 
      }
                            
    // stride
    retMsg=stride_->Scalar2index(stride);
    if( retMsg == 0) // index empty or array
      {
	if( stride_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2index:"
			  " stride index empty",true,false); 
	else
	  throw 
	    GDLException(  "Expression must be a scalar"
			  " in this context.",true,false); 
      }
    if( retMsg == -1 || stride == 0) // stride <= 0
      {
	throw 
	  GDLException(  "Range subscript stride must be >= 1.",true,false);
      }
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
//     RangeT sl,el;
    if( sInit < 0)
	    {
	      s = sInit + varDim;
	      if( s < 0)
		      throw GDLException(NULL,"Subscript out of range [-S:e:stride].",true,false);
	    }
    else
	    s = sInit;
    if( eInit < 0)
	    {
	      e = eInit + varDim;
	      if( e < 0)
		      throw GDLException(NULL,"Subscript out of range [s:-E:stride].",true,false);
	    }
    else
	    e = eInit;
    
    if( s > e)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    
    if( e >= varDim) // && e > 0)
      {
		throw GDLException(NULL,"Subscript out of range [s:E:st].",true,false);
      }
    return (e - s + stride)/stride;
  }
};

class CArrayIndexRangeS: public ArrayIndexRangeS
{
public:
  IndexType Type() { return CArrayIndexRangeSID;}

  bool IsRange() { return true;}

  BaseGDL* OverloadIndexNew()
  {
    DLong arr[3] = {sInit,eInit,stride};
    return new DLongGDL( arr, 3);
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexRangeS( BaseGDL* c1, BaseGDL* c2, BaseGDL* c3): 
    ArrayIndexRangeS()
  {
    ArrayIndexRangeS::Init( c1, c2, c3);
  }

  CArrayIndexRangeS(){}

  ArrayIndexT* Dup() const
  {
    CArrayIndexRangeS* d = new CArrayIndexRangeS();
    d->sInit = sInit;
    d->eInit = eInit;
    d->s = s;
    d->e = e;
    d->stride = stride;
    return d;
  }

};

#endif
