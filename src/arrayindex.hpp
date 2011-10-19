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
IxExprListT& operator=( const IxExprListT&r) { sz = r.sz; for( int i=0; i<sz; ++i) eArr[i] = r.eArr[i];}
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

  virtual void Clear() {}
  virtual ~ArrayIndexT() {}

  virtual RangeT GetIx0()=0;
  virtual RangeT GetS() { return 0;}
  virtual SizeT GetStride() { return 1;} // changed from 0
  
  virtual bool Scalar()  const         { return false;}
  virtual bool Scalar( SizeT& s_) const {  return false;}
//  virtual bool Scalar( RangeT& s_) {  return false;}
  
  virtual bool Indexed()          { return false;}  

  virtual BaseGDL* Index( BaseGDL* var, IxExprListT& ix)=0;

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

  SizeT s;

public:
 IndexType Type() { return ArrayIndexScalarID;}
//   SizeT GetIx( SizeT i)
//   {
// 	assert( ix != NULL);
//     return (*ix)[ i];
//   }

  SizeT GetVarIx() const { return varIx;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  RangeT GetS() { return s;}

  bool Scalar() const { return true;}
  bool Scalar( SizeT& s_) const
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
  BaseGDL* Index( BaseGDL* var, IxExprListT& ixL);

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim);
};
// VARPTR (common block variable)
class ArrayIndexScalarVP: public ArrayIndexT
{
protected:
  DVar* varPtr;

  SizeT s;

public:
 IndexType Type() { return ArrayIndexScalarVPID;}

  DVar* GetVarPtr() const { return varPtr;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  RangeT GetS() { return s;}

  bool Scalar() const { return true;}
  bool Scalar( SizeT& s_) const
  { 
    s_ = s;
    return true;
  }

  void Init() 
  {
    s = varPtr->Data()->LoopIndex();
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
  BaseGDL* Index( BaseGDL* var, IxExprListT& ixL);

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim);
};





// constant SCALAR
class CArrayIndexScalar: public ArrayIndexT
{
private:
  SizeT s;

public:
 IndexType Type() { return CArrayIndexScalarID;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  bool Scalar() const { return true;}
  bool Scalar( SizeT& s_) const
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
  {}

  CArrayIndexScalar( BaseGDL* c)
  {
    s = c->LoopIndex();
  }

  CArrayIndexScalar( SizeT s_): s( s_) 
  {}

  ArrayIndexT* Dup() const
  {
    return new CArrayIndexScalar( s);
  }
 
  void Clear()
  {}

  // if this is used, Init was NOT called before
  BaseGDL* Index( BaseGDL* var, IxExprListT& ixL)
  {
    if( s >= var->Size())
      throw GDLException(NULL,"Scalar subscript out of range [>].h1",true,false);
    return var->NewIx( s);
  }

  void Init( BaseGDL* ix_) 
  {
    assert( 0);
  } 

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim) 
  {
    if( s > 0 && s >= varDim)
      throw GDLException(NULL,"Scalar subscript out of range [>].h2",true,false);
    return 1;
  }
};





// INDEXED or ONE [v] (must handle both)
class ArrayIndexIndexed: public ArrayIndexT
{
protected:
  bool      strictArrSubs;          // for compile_opt STRICTARRSUBS
  
  RangeT s;
//  SizeT maxVal;

  AllIxIndicesT*    ix;
  char ixBuf[ AllIxMaxSize];
  
  const dimension* ixDim; // keep dimension of ix

  // forbid c-i
  ArrayIndexIndexed( const ArrayIndexT& r) {}

public:
 IndexType Type() { return ArrayIndexIndexedID;}

  SizeT NParam() { return 1;} // number of parameter to Init(...)

  RangeT GetS() { return s;}

  bool Scalar() const { return (ix == NULL);}
  bool Scalar( SizeT& s_) const // changed from RangeT for proper overloading
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

  const dimension& GetDim() { return *ixDim;}

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


  BaseGDL* Index( BaseGDL* var, IxExprListT& ixL)
  {
    int ret = ixL[0]->Scalar2RangeT(s);

    if( ret == 0) // more than one element
      return var->NewIx( ixL[0], strictArrSubs);

    // scalar (-1,1) or one-element array (-2,2)
    //if( ret >= 1) // type ONE
    if( ret == 1) // type ONE
	{
		if( s < 0) //&& s >= var->Size())
		{
			RangeT ix = var->Size() + s;
			if( ix < 0)
				throw GDLException(NULL,"Subscript out of range [-i].",true,false);
			//if( strictArrSubs || ret == 1)
			//return var->NewIx( var->Size()-1);
			return var->NewIx( ix);
		}
		if( s >= var->Size())
		{
			throw GDLException(NULL,"Subscript out of range [i].",true,false);
		}
		return var->NewIx( s);
	}

	// ret == 2 (one dim array)
	if( s < 0) //&& s >= var->Size())
	{
		if( strictArrSubs)
			throw GDLException(NULL,"Subscript out of range [-i].",true,false);
		BaseGDL* res = var->NewIx( 0);
		res->SetDim( dimension( 1));
		return res;
	}
	if( s >= var->Size())
	{
		if( strictArrSubs)
			throw GDLException(NULL,"Subscript out of range [i].",true,false);
		BaseGDL* res = var->NewIx( var->Size()-1);
		res->SetDim( dimension( 1));
		return res;
	}

	BaseGDL* res = var->NewIx( s);
	res->SetDim( dimension( 1));
	return res;
	
// //     // unreachable because using now Scala2RangeT which returns always 0 ,1 or 2
// //     if( strictArrSubs || ret == -1) // scalar index < 0
// //       {
// // 	throw
// // 	  GDLException( NULL,"Subscript range values of the"
// // 			" form low:high must be >= 0, < size,"
// // 			" with low <= high.",true,false);
// //       }
// //     one element array index < 0
// //     if( ret == -2)
// //       {
// // 	BaseGDL* res = var->NewIx( 0);
// // 	res->SetDim( dimension( 1));
// // 	return res;
// //       }
// //     else
// //       return var->NewIx( 0);
   }

  void Init( BaseGDL* ix_) 
  {
    if( ix_->Rank() == 0) // type ONE
      {
		ix_->Scalar2RangeT(s);
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

    assert( dType != UNDEF);
//     assert( maxVal == 0);

    int typeCheck = DTypeOrder[ dType];
    if( typeCheck >= 100)
      throw GDLException(NULL,"Type not allowed as subscript.",true,false);
    
    //SizeT nElem = ix_->N_Elements();
    //    ix = new SizeT[ nElem]; // allocate array

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
		if( s > 0 && s >= varDim)
		throw GDLException(NULL,"Subscript out of range [i].",true,false);
		if( s < 0 && -s > varDim)
		throw GDLException(NULL,"Subscript out of range [-i].",true,false);
		return 1;
      }

    // INDEXED
    // note: this should be faster as most arrays are within bounds
    // (like returned from WHERE function)
    //    SizeT maxIx = ix->max();
//     if( maxVal < varDim)
// 		  return ix->size();

	ix->SetUpper( varDim-1);

    return ix->size();

//     if( strictArrSubs)
//       { // strictArrSubs -> exception if out of bounds
//       }
//     else
//       {
// 	SizeT upper = varDim-1;
// 	SizeT ix_size = ix->size();
// 	for( SizeT i=0; i < ix_size; ++i)
// 	  {
// 	    if( (*ix)[i] > upper)
//     		ix->SetIx( i, upper);
// 			(*ix)[i]=upper;
// 	  }
// 	return ix_size;
//       }
  }
};



// constant version
class CArrayIndexIndexed: public ArrayIndexIndexed
{
protected:
  AllIxIndicesT*    ixOri;
  char ixOriBuf[ sizeof( AllIxIndicesStrictT)];

//  SizeT      maxIx;

public:
 IndexType Type() { return CArrayIndexIndexedID;}

  ~CArrayIndexIndexed() { /*delete ixOri;*/}

  CArrayIndexIndexed( BaseGDL* c, bool strictArrSubs_ = false): 
    ArrayIndexIndexed( strictArrSubs_)//, ixOri( NULL) //, maxIx( 0)
  {
    ArrayIndexIndexed::Init( c);
    ixOri = ix;
    ix = NULL;
//     if( ixOri != NULL) maxIx = ixOri->max();
  }

  CArrayIndexIndexed( const CArrayIndexIndexed& cp):
    ArrayIndexIndexed( cp.strictArrSubs), ixOri( NULL) //, maxIx( cp.maxIx)
  {
    assert( cp.ix == NULL);

    s = cp.s;
//    maxVal = cp.maxVal;
//     maxIx  = cp.maxIx;

    if( cp.ixOri != NULL)
      ixOri = cp.ixOri->CloneAt( ixOriBuf);
//       ixOri = new AllIxT( *cp.ixOri);
  }

  ArrayIndexT* Dup() const
  {
    return new CArrayIndexIndexed( *this);
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)
  void Clear()
  {
//   delete ix;
  ix=NULL;
  } // note that ixDim is untouched

  // make the following work even before call to NIter(...)
  bool Scalar() const { return (ixOri == NULL);}
  bool Scalar( SizeT& s_) const
  { 
    if( ixOri == NULL)
      {
		s_ = s;
		return true;
      }
    s_ = (*ixOri)[0];
    return (ixOri->size() == 1);
  }
  bool Indexed() { return (ixOri != NULL);}

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_)
  {
    if( ixOri == NULL) // ONE
      {
		if( s >= var->Size())
			throw GDLException(NULL,"Subscript out of range [i].",true,false);
		return var->NewIx( s);
      }

	ixOri->SetUpper(var->Size()-1);

//     if( maxIx >= var->Size())
//       {
// 		if( strictArrSubs)
// 		{ // strictArrSubs -> exception if out of bounds
// 			throw GDLException(NULL,"Array used to subscript array "
// 					"contains out of range subscript.",true,false);
// 		}
// 
// 		SizeT ix_size = ix->size();
// 		ix = new AllIxMultiT( ixOri->size()); // make copy as changed (see below)
// 		SizeT upper = var->Size()-1;
// 		for( SizeT i=0; i < ix_size; ++i)
// 		{
// 			if( (*ixOri)[i] > upper)
// 					static_cast<AllIxMultiT*>(ix)->SetIx( i, upper);
// 		      (*ix)[i] = upper;
// 			else
// 					static_cast<AllIxMultiT*>(ix)->SetIx( i, (*ixOri)[i]);
// 		      (*ix)[i] = (*ixOri)[i];
// 		}
// 		return var->NewIx( ix, ixDim);
//       }
//     else
      return var->NewIx( ixOri, ixDim);
  }

  // old (before ixOri): special here no stealing is allowed
  //  AllIxT* StealIx() { return new AllIxT( *ix);} 

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim) 
  {
    if( ixOri == NULL) // ONE
      {
		if( s >= varDim)
		throw GDLException(NULL,"Subscript out of range [i].",true,false);
		return 1;
      }

    // INDEXED
    assert( ix == NULL);
    ix = ixOri->CloneAt(ixBuf); //new AllIxMultiT( ixOri->size()); // make copy as changed (see below)
    SizeT ix_size = ix->size();

	ix->SetUpper( varDim-1);
    
//     if( maxIx >= varDim)
//       {
// 		if( strictArrSubs)
// 		{ // strictArrSubs -> exception if out of bounds
// 			throw GDLException(NULL,"Array used to subscript array "
// 					"contains out of range subscript.",true,false);
// 		}
// 
// 		SizeT upper = varDim-1;
// 		for( SizeT i=0; i < ix_size; ++i)
// 		{
// 			if( (*ixOri)[i] > upper)
// 				static_cast<AllIxMultiT*>(ix)->SetIx( i, upper);
// 		      (*ix)[i] = upper;
// 			else
// 				static_cast<AllIxMultiT*>(ix)->SetIx( i, (*ixOri)[i]);
// 		      (*ix)[i] = (*ixOri)[i];
// 		}
//       }
//     else
//       for( SizeT i=0; i < ix_size; ++i)
// 	   		static_cast<AllIxMultiT*>(ix)->SetIx( i, (*ixOri)[i]);
// 	(*ix)[i] = (*ixOri)[i];
    
    return ix_size; 
  }
};






// [*]
class ArrayIndexAll: public ArrayIndexT
{
public:
 IndexType Type() { return ArrayIndexAllID;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  void Init() {};

  RangeT GetIx0() { return 0;}

  ArrayIndexT* Dup() const
  {
    return new ArrayIndexAll();
  }

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    // make index 1-dim
    BaseGDL* clone =  var->Dup();
    clone->SetDim( dimension( clone->N_Elements()));
    return clone;
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
  RangeT s;

public:
 IndexType Type() { return ArrayIndexORangeID;}

  SizeT NParam() { return 1;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexORange* d = new ArrayIndexORange();
    d->s = s;
    return d;
  }

   // s is always scalar here
  void Init( BaseGDL* s_)
  {
    int retMsg=s_->Scalar2RangeT(s);
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

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    Init( ix[0]);

	SizeT varSize = var->Size();
    
	if( s < 0)
		{
			RangeT i = s + varSize;
			if( i < 0)
				throw GDLException(NULL,"Subscript out of range [-s:*].",true,false);
    
			return var->NewIxFrom( i);
		}
	
    if( s >= varSize)
      throw GDLException(NULL,"Subscript out of range [s:*].",true,false);

    return var->NewIxFrom( s);
  }

  SizeT NIter( SizeT varDim)
  {
    if( s >= varDim)
      throw GDLException(NULL,"Subscript out of range [s:*].",true,false);
    if( s < 0)
    {
		RangeT i = s + varDim;
		if( i < 0)
			throw GDLException(NULL,"Subscript out of range [-s:*].",true,false);
		
		return (varDim - i);
    }
    return (varDim - s);
  }
};



class CArrayIndexORange: public ArrayIndexORange
{
public:
 IndexType Type() { return CArrayIndexORangeID;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexORange( BaseGDL* c): ArrayIndexORange()
  {
    ArrayIndexORange::Init( c);
  }

  CArrayIndexORange() {}

  ArrayIndexT* Dup() const
  {
    CArrayIndexORange* d = new CArrayIndexORange();
    d->s = s;
    return d;
  }

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
	SizeT varSize = var->Size();
    
	if( s < 0)
		{
			RangeT i = s + varSize;
			if( i < 0)
				throw GDLException(NULL,"Subscript out of range [-s:*].",true,false);
    
			return var->NewIxFrom( i);
		}
    
    if( s >= varSize)
      throw GDLException(NULL,"Subscript out of range [s:*].",true,false);

    return var->NewIxFrom( s);
  }
};



// [s:e]
class ArrayIndexRange: public ArrayIndexT
{
protected:
  RangeT s,e;

public:
 IndexType Type() { return ArrayIndexRangeID;}

  SizeT NParam() { return 2;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  RangeT GetIx0() { return s;}

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    Init( ix[0], ix[1]);
    
	SizeT varSize = var->Size();
	
    RangeT sl,el;
	if( s < 0)
		{
			sl = s + varSize;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:e].",true,false);
		}
	else
		sl = s;
	if( e < 0)
		{
			el = e + varSize;
			if( el < 0)
				throw GDLException(NULL,"Subscript out of range [s:-E].",true,false);
		}
	else
		el = e;
    
    if( sl > el)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    if( el >= var->Size())
      throw GDLException(NULL,"Subscript out of range [s:e].",true,false);

    return var->NewIxFrom( sl, el);
  }

  ArrayIndexT* Dup() const
  {
    ArrayIndexRange* d = new ArrayIndexRange();
    d->s = s;
    d->e = e;
    return d;
  }

  void Init( BaseGDL* s_, BaseGDL* e_)
  {
// 	SizeT varSize = var->Size();
    
    int retMsg=s_->Scalar2RangeT(s);
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
    
    retMsg=e_->Scalar2RangeT(e);
    if( retMsg == 0) // index empty or array
      {
	if( e_->N_Elements() == 0)
	  throw 
	    GDLException(NULL,"Internal error: Scalar2RangeT: 2nd index empty.",true,false);
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
    
    if( e>=0 && s>=0 && e < s)
      {
	throw 
	  GDLException(NULL,"Subscript range values of the form low:high "
			"must be >= 0, < size, with low <= high",true,false);
      }
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
	RangeT sl,el;
	if( s < 0)
	{
		sl = s + varDim;
		if( sl < 0)
			throw GDLException(NULL,"Subscript out of range [S:e].",true,false);
	}
	else
		sl = s;
	if( e < 0)
	{
		el = e + varDim;
		if( el < 0)
			throw GDLException(NULL,"Subscript out of range [s:E].",true,false);
	}
	else
		el = e;
    if( sl > el)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    if( el >= varDim)
		throw GDLException(NULL,"Subscript out of range [s:e].",true,false);
    return (el - sl + 1);
  }
};



class CArrayIndexRange: public ArrayIndexRange
{
public:
 IndexType Type() { return CArrayIndexRangeID;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexRange( BaseGDL* c1, BaseGDL* c2): ArrayIndexRange()
  {
    ArrayIndexRange::Init( c1, c2);
  }

  CArrayIndexRange(){}

  ArrayIndexT* Dup() const
  {
    CArrayIndexRange* d = new CArrayIndexRange();
    d->s = s;
    d->e = e;
    return d;
  }

   BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
   {
	SizeT varSize = var->Size();
	
    RangeT sl,el;
	if( s < 0)
		{
			sl = s + varSize;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:e].",true,false);
		}
	else
		sl = s;
	if( e < 0)
		{
			el = e + varSize;
			if( el < 0)
				throw GDLException(NULL,"Subscript out of range [s:-E].",true,false);
		}
	else
		el = e;
    
    if( sl > el)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    if( el >= var->Size())
      throw GDLException(NULL,"Subscript out of range [s:e].",true,false);

    return var->NewIxFrom( sl, el);
   }
};



// [s:*:st]
class ArrayIndexORangeS: public ArrayIndexT
{
protected:
  RangeT s;
  SizeT stride;

public:
 IndexType Type() { return ArrayIndexORangeSID;}

  SizeT NParam() { return 2;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  SizeT GetStride() { return stride;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexORangeS* d = new ArrayIndexORangeS();
    d->s = s;
    d->stride = stride;
    return d;
  }

  void Init( BaseGDL* s_, BaseGDL* stride_)
  {
    int retMsg=s_->Scalar2RangeT( s);
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

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    Init( ix[0], ix[1]);
	
	SizeT varSize = var->Size();

	if( s < 0)
		{
			RangeT sl = s + varSize;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:*:stride].",true,false);

			return var->NewIxFromStride( sl, stride);
		}

    if( s >= var->Size())
      throw GDLException(NULL,"Subscript out of range [s:*:stride].",true,false);

    return var->NewIxFromStride( s, stride);
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
	if( s < 0)
		{
			RangeT sl = s + varDim;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:*:stride].",true,false);
			return (varDim - sl + stride - 1)/stride;
		}

    if( s >= varDim)
      throw GDLException(NULL,"Subscript out of range [s:*:stride].",true,false);
    return (varDim - s + stride - 1)/stride;
  }
};

class CArrayIndexORangeS: public ArrayIndexORangeS
{
public:
 IndexType Type() { return CArrayIndexORangeSID;}

  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexORangeS( BaseGDL* c1, BaseGDL* c2): ArrayIndexORangeS()
  {
    ArrayIndexORangeS::Init( c1, c2);
  }

CArrayIndexORangeS(){}

  ArrayIndexT* Dup() const
  {
    CArrayIndexORangeS* d = new CArrayIndexORangeS();
    d->s = s;
    d->stride = stride;
    return d;
  }

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
	SizeT varSize = var->Size();

	if( s < 0)
		{
			RangeT sl = s + varSize;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:*:stride].",true,false);

			return var->NewIxFromStride( sl, stride);
		}

    if( s >= var->Size())
      throw GDLException(NULL,"Subscript out of range [s:*:stride].",true,false);

    return var->NewIxFromStride( s, stride);
  }
};

// [s:e:st]
class ArrayIndexRangeS: public ArrayIndexT
{
protected:
  RangeT s,e;
  SizeT stride;

public:
 IndexType Type() { return ArrayIndexRangeSID;}

  SizeT NParam() { return 3;} // number of parameter to Init(...)

  RangeT GetS() { return s;}
  SizeT GetStride() { return stride;}
  RangeT GetIx0() { return s;}

  ArrayIndexT* Dup() const
  {
    ArrayIndexRangeS* d = new ArrayIndexRangeS();
    d->s = s;
    d->e = e;
    d->stride = stride;
    return d;
  }

  void Init( BaseGDL* s_, BaseGDL* e_, BaseGDL* stride_)
  {
    int retMsg=s_->Scalar2RangeT(s);
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
            
    retMsg=e_->Scalar2RangeT(e);
    if( retMsg == 0) // index empty or array
      {
	if( e_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2RangeT: 2nd index empty.",true,false);
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
            
    if( e >= 0 && s >= 0 && e < s)
      {
		throw 
		GDLException(  "Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
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

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    Init( ix[0], ix[1], ix[2]);

	SizeT varSize = var->Size();
	
    RangeT sl,el;
	if( s < 0)
		{
			sl = s + varSize;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:e:stride].",true,false);
		}
	else
		sl = s;
	if( e < 0)
		{
			el = e + varSize;
			if( el < 0)
				throw GDLException(NULL,"Subscript out of range [s:-E:stride].",true,false);
		}
	else
		el = e;
    
    if( sl > el)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    
    if( el >= var->Size())
      {
	throw GDLException(NULL,"Subscript out of range [s:e:st].",true,false);
      }

    return var->NewIxFromStride( sl, el, stride);
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
    RangeT sl,el;
	if( s < 0)
		{
			sl = s + varDim;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:e:stride].",true,false);
		}
	else
		sl = s;
	if( e < 0)
		{
			el = e + varDim;
			if( el < 0)
				throw GDLException(NULL,"Subscript out of range [s:-E:stride].",true,false);
		}
	else
		el = e;
    
    if( sl > el)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    
    if( el >= varDim)
      {
		throw GDLException(NULL,"Subscript out of range [s:e:st].",true,false);
      }
    return (el - sl + stride)/stride;
  }
};

class CArrayIndexRangeS: public ArrayIndexRangeS
{
public:
 IndexType Type() { return CArrayIndexRangeSID;}

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
    d->s = s;
    d->e = e;
    d->stride = stride;
    return d;
  }

  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
	SizeT varSize = var->Size();
	
    RangeT sl,el;
	if( s < 0)
		{
			sl = s + varSize;
			if( sl < 0)
				throw GDLException(NULL,"Subscript out of range [-S:e:stride].",true,false);
		}
	else
		sl = s;
	if( e < 0)
		{
			el = e + varSize;
			if( el < 0)
				throw GDLException(NULL,"Subscript out of range [s:-E:stride].",true,false);
		}
	else
		el = e;
    
    if( sl > el)
		throw 
			GDLException(NULL,"Subscript range values of the form low:high "
				"must be < size, with low <= high",true,false);
    
    if( el >= var->Size())
      {
		throw GDLException(NULL,"Subscript out of range [s:e:st].",true,false);
      }

    return var->NewIxFromStride( sl, el, stride);
  }
};

#endif
