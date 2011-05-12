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

//typedef std::valarray<SizeT> AllIxT; // now in typedefs.hpp
typedef std::vector<BaseGDL*>     IxExprListT;

enum IndexType
{
ArrayIndexTID,
ArrayIndexScalarID,
ArrayIndexScalarVPID,
CArrayIndexScalarID,
ArrayIndexIndexedID,
CArrayIndexIndexedID,
ArrayIndexAllID,
ArrayIndexORangeID,
CArrayIndexORangeID,
ArrayIndexRangeID,
CArrayIndexRangeID,
ArrayIndexORangeSID,
CArrayIndexORangeSID,
ArrayIndexRangeSID,
CArrayIndexRangeSID
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
  virtual SizeT GetStride() { return 0;}
  
  virtual bool Scalar()  const         { return false;}
  virtual bool Scalar( SizeT& s_) const {  return false;}
//  virtual bool Scalar( RangeT& s_) {  return false;}
  
  virtual bool Indexed()          { return false;}  

  virtual BaseGDL* Index( BaseGDL* var, IxExprListT& ix)=0;

  virtual SizeT NIter( SizeT varDim)=0; 

  virtual SizeT NParam()=0;

  virtual ArrayIndexT* Dup() const =0;
};

// SCALAR (only for FOR loop indices)
// VAR
class ArrayIndexScalar: public ArrayIndexT
{
protected:
  SizeT varIx;

  SizeT s;

public:
 IndexType Type() { return ArrayIndexScalarID;}

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
  SizeT maxVal;

  AllIxT*    ix;
  dimension* ixDim; // keep dimension of ix

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

  dimension GetDim() { return *ixDim;}

  RangeT GetIx0()
  {
    if( ix != NULL) return (*ix)[0]; // from array
    return s;
  }

  //  SizeT* StealIx() { SizeT* ret = ix; ix = NULL; return ret;} 
  AllIxT* StealIx() { AllIxT* ret = ix; ix = NULL; return ret;} 

  ~ArrayIndexIndexed() 
  {
    delete ix;
    delete ixDim;
  }

  ArrayIndexIndexed( bool strictArrSubs_ = false): 
    strictArrSubs( strictArrSubs_),
    maxVal( 0),
    ix( NULL), ixDim( NULL)
  {}

  ArrayIndexT* Dup() const
  {
    ArrayIndexIndexed* d =  new ArrayIndexIndexed( strictArrSubs);
    
    assert( ix == NULL);
    assert( ixDim == NULL);

    d->s = s;
    d->maxVal = maxVal;
    
    return d;
  }
 
  void Clear()
  {
    maxVal = 0;
    delete ixDim;
    ixDim = NULL;
    delete ix; 
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
	
    // unreachable because using now Scala2RangeT which returns always 0 ,1 or 2  
    if( strictArrSubs || ret == -1) // scalar index < 0
      {
	throw 
	  GDLException( NULL,"Subscript range values of the"
			" form low:high must be >= 0, < size,"
			" with low <= high.",true,false);
      }
    // one element array index < 0
    if( ret == -2)
      {
	BaseGDL* res = var->NewIx( 0);
	res->SetDim( dimension( 1));
	return res;
      }	
    else
      return var->NewIx( 0);
  }

  void Init( BaseGDL* ix_) 
  {
    if( ix_->Rank() == 0) // type ONE
      {
	int ret = ix_->Scalar2RangeT(s);
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
    assert( maxVal == 0);

    int typeCheck = DTypeOrder[ dType];
    if( typeCheck >= 100)
      throw GDLException(NULL,"Type not allowed as subscript.",true,false);
    
    SizeT nElem = ix_->N_Elements();
    //    ix = new SizeT[ nElem]; // allocate array

    assert( ix == NULL);

    ix = new AllIxT( nElem);

    ixDim = new dimension( ix_->Dim());

    switch( dType)
      {
      case BYTE:
	{
	  DByteGDL* src = static_cast<DByteGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case INT:
	{
	  DIntGDL* src = static_cast<DIntGDL*>( ix_);
	  SizeT i = 0;
	  for(; i < nElem; ++i)
	    if( (*src)[i] < 0)
	      {
		if( strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
		(*ix)[i++] = 0;
		break;
	      }
	    else
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  
	  for(; i < nElem; ++i)
	    if( (*src)[i] < 0)
	      (*ix)[i] = 0;
	    else
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;

	  // older version
/*	  DInt minVal = src->min();
	  if( minVal < 0)
	    {
	      //	      negative = true;
	      for( SizeT i=0; i < nElem; ++i)
		if( (*src)[i] < 0)
		  (*ix)[i] = 0;
		else
		  (*ix)[i]= (*src)[i]; 
	    }
	  else
	    for( SizeT i=0; i < nElem; ++i)
	      (*ix)[i]= (*src)[i]; 
	  break;*/
	}
      case UINT:
	{
	  DUIntGDL* src = static_cast<DUIntGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case LONG: // typical type (returned from WHERE)
	{
	  DLongGDL* src = static_cast<DLongGDL*>( ix_);
	  SizeT i = 0;
	  for(; i < nElem; ++i)
	    if( (*src)[i] < 0)
	      {
		if( strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
		(*ix)[i++] = 0;
		break;
	      }
	    else
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  
// 	  for(; i < nElem; ++i)
// 	    if( (*src)[i] < 0)
// 	      (*ix)[i] = 0;
// 	    else
// 	      {
// 		(*ix)[i]= (*src)[i]; 
// 		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
// 	      }
// 	  break;

// 	  // older version
// 	  DLong minVal = src->min();
// 	  if( minVal < 0)
// 	    {
// 	      for( SizeT i=0; i < nElem; ++i)
// 		if( (*src)[i] < 0)
// 		  (*ix)[i] = 0;
// 		else
// 		  (*ix)[i]= (*src)[i]; 
// 	      //	      negative = true;
// 	      for( SizeT i=0; i < nElem; ++i)
// 		if( (*src)[i] < 0)
// 		  (*ix)[i] = 0;
// 		else
// 		  (*ix)[i]= (*src)[i]; 
// 	    }
// 	  else
// 	    for( SizeT i=0; i < nElem; ++i)
// 	      (*ix)[i]= (*src)[i]; 
// 	  break;
	  for(; i < nElem; ++i)
	    if( (*src)[i] < 0)
	      (*ix)[i] = 0;
	    else
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;

/*	  // older version
	  DLong minVal = src->min();
	  if( minVal < 0)
	    {
	      for( SizeT i=0; i < nElem; ++i)
		if( (*src)[i] < 0)
		  (*ix)[i] = 0;
		else
		  (*ix)[i]= (*src)[i]; 
	    }
	  else
	    for( SizeT i=0; i < nElem; ++i)
	      (*ix)[i]= (*src)[i]; 
	  break;*/
	}
      case ULONG:
	{
	  DULongGDL* src = static_cast<DULongGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case LONG64:
	{
	  DLong64GDL* src = static_cast<DLong64GDL*>( ix_);
	  SizeT i = 0;
	  for(; i < nElem; ++i)
	    if( (*src)[i] < 0)
	      {
		if( strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
		(*ix)[i++] = 0;
		break;
	      }
	    else
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  
	  for(; i < nElem; ++i)
	    if( (*src)[i] < 0)
	      (*ix)[i] = 0;
	    else
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;

/*	  // older version
	  DLong64 minVal = src->min();
	  if( minVal < 0)
	    {
	      //	      negative = true;
	      for( SizeT i=0; i < nElem; ++i)
		if( (*src)[i] < 0)
		  (*ix)[i] = 0;
		else
		  (*ix)[i]= (*src)[i]; 
	    }
	  else
	    for( SizeT i=0; i < nElem; ++i)
	      (*ix)[i]= (*src)[i]; 
	  break;*/
      }
      case ULONG64:
	{
	  DULong64GDL* src = static_cast<DULong64GDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	      {
		(*ix)[i]= (*src)[i]; 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case FLOAT: 
	{
	  DFloatGDL* src = static_cast<DFloatGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    if( (*src)[i] <= 0.0)
	      {
		(*ix)[i] = 0;
		if( (*src)[i] <= -1.0 && strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
	      }
	    else
	      {
		(*ix)[i]= Real2Int<SizeT,float>((*src)[i]); 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case DOUBLE: 
	{
	  DDoubleGDL* src = static_cast<DDoubleGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    if( (*src)[i] <= 0.0)
	      {
		(*ix)[i] = 0;
		if( (*src)[i] <= -1.0 && strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
	      }
	    else
	      {
		(*ix)[i]= Real2Int<SizeT,double>((*src)[i]); 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case STRING: 
	{
	  DStringGDL* src = static_cast<DStringGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    {
	      const char* cStart=(*src)[i].c_str();
	      char* cEnd;
	      long l=strtol(cStart,&cEnd,10);
	      if( cEnd == cStart)
		{
		  Warning("Type conversion error: "
			  "Unable to convert given STRING to LONG.");
		  (*ix)[i] = 0;
		}
	      else if( l < 0)
		{
		  if( strictArrSubs)
		    throw GDLException(NULL,"Array used to subscript array "
				       "contains out of range (<0) subscript.",true,false);
		  (*ix)[i] = 0;
		}
	      else
		{
		  (*ix)[i] = l;
		  if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
		}
	    }
	  break;
	}
      case COMPLEX: 
	{
	  DComplexGDL* src = static_cast<DComplexGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    if( real((*src)[i]) <= 0.0)
	      {
		if( real((*src)[i]) <= -1.0 && strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
		(*ix)[i] = 0;
	      }
	    else
	      {
		(*ix)[i]= Real2Int<DLong,float>(real((*src)[i])); 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      case COMPLEXDBL: 
	{
	  DComplexDblGDL* src = static_cast<DComplexDblGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    if( real((*src)[i]) <= 0.0)
	      {
		if( real((*src)[i]) <= -1.0 && strictArrSubs)
		  throw GDLException(NULL,"Array used to subscript array "
				     "contains out of range (<0) subscript.",true,false);
		(*ix)[i] = 0;
	      }
	    else
	      {
		(*ix)[i]= Real2Int<DLong,double>(real((*src)[i])); 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      default: // (UNDEF)
        {
          assert(false);
          throw; // getting rid of compiler warning
        }
      }
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
    if( maxVal < varDim)
      return ix->size();

    if( strictArrSubs)
      { // strictArrSubs -> exception if out of bounds
	throw GDLException(NULL,"Array used to subscript array "
			   "contains out of range subscript.",true,false);
      }
    else
      {
	SizeT upper = varDim-1;
	SizeT ix_size = ix->size();
	for( SizeT i=0; i < ix_size; ++i)
	  {
	    if( (*ix)[i] > upper) (*ix)[i]=upper;
	  }
	return ix_size; 
      }
  }

};



// constant version
class CArrayIndexIndexed: public ArrayIndexIndexed
{
protected:
  AllIxT*    ixOri;
  SizeT      maxIx;

public:
 IndexType Type() { return CArrayIndexIndexedID;}

  ~CArrayIndexIndexed() { delete ixOri;}

  CArrayIndexIndexed( BaseGDL* c, bool strictArrSubs_ = false): 
    ArrayIndexIndexed( strictArrSubs_), ixOri( NULL), maxIx( 0)
  {
    ArrayIndexIndexed::Init( c);
    ixOri = ix;
    ix = NULL;
    if( ixOri != NULL) maxIx = ixOri->max();
  }

  CArrayIndexIndexed( const CArrayIndexIndexed& cp):
    ArrayIndexIndexed( cp.strictArrSubs), ixOri( NULL), maxIx( cp.maxIx)
  {
    assert( cp.ix == NULL);

    s = cp.s;
    maxVal = cp.maxVal;
    maxIx  = cp.maxIx;

    if( cp.ixOri != NULL)
      ixOri = new AllIxT( *cp.ixOri);
  }

  ArrayIndexT* Dup() const
  {
    return new CArrayIndexIndexed( *this);
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)
  void Clear() { delete ix; ix=NULL;} // note that ixDim is untouched

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

    if( maxIx >= var->Size())
      {
	if( strictArrSubs)
	  { // strictArrSubs -> exception if out of bounds
	    throw GDLException(NULL,"Array used to subscript array "
			       "contains out of range subscript.",true,false);
	  }

	SizeT ix_size = ix->size();
	ix = new AllIxT( ixOri->size()); // make copy as changed (see below)
	SizeT upper = var->Size()-1;
	for( SizeT i=0; i < ix_size; ++i)
	  {
	    if( (*ixOri)[i] > upper) 
	      (*ix)[i] = upper;
	    else
	      (*ix)[i] = (*ixOri)[i];
	  }
	return var->NewIx( ix, ixDim);
      }
    else
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
    ix = new AllIxT( ixOri->size()); // make copy as changed (see below)
    SizeT ix_size = ix->size();
    if( maxIx >= varDim)
      {
	if( strictArrSubs)
	  { // strictArrSubs -> exception if out of bounds
	    throw GDLException(NULL,"Array used to subscript array "
			       "contains out of range subscript.",true,false);
	  }

	SizeT upper = varDim-1;
	for( SizeT i=0; i < ix_size; ++i)
	  {
	    if( (*ixOri)[i] > upper) 
	      (*ix)[i] = upper;
	    else
	      (*ix)[i] = (*ixOri)[i];
	  }
      }
    else
      for( SizeT i=0; i < ix_size; ++i)
	(*ix)[i] = (*ixOri)[i];
    
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

// class ArrayIndexListT
// {
// private:
//   ArrayIndexVectorT ixList;

//   enum AccessType {
//     UNDEF=0,      // for init access type
//     INDEXED_ONE,  // all indexed OR one
//     NORMAL,       // mixed
//     ONEDIM,
//     ALLINDEXED,
//     ALLONE        // all ONE
//   };

//   AccessType accessType;         // actual access type
//   AccessType accessTypeInit;     // possible access type non assoc
//   AccessType accessTypeAssocInit;// possible access type for assoc
//   SizeT    acRank;               // rank upto which indexing is done
//   SizeT    nIterLimit[MAXRANK];  // for each dimension, how many iterations
//   SizeT    stride[MAXRANK+1];    // for each dimension, how many iterations
//   SizeT    varStride[MAXRANK+1]; // variables stride
//   SizeT    nIx;                  // number of indexed elements

//   AllIxT* allIx;

//   ArrayIndexT* ixListEnd; // for assoc index

//   SizeT nParam; // number of (BaseGDL*) parameters
// public:    
  
//   SizeT NParam() { return nParam;}

// //   ArrayIndexT* StealIxList0() 
// //   {
// //     ArrayIndexT* res = ixList[0];
// //     ixList[0] = NULL;
// //     return res;
// //   }

//   ~ArrayIndexListT()
//   {
//     delete allIx;
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
// 	 i != ixList.end(); ++i)
//       {	delete *i;}
//   }

//   // constructor
//   ArrayIndexListT():
//     accessType(NORMAL),
//     acRank(0),
//     allIx( NULL),
//     ixListEnd( NULL),
//     nParam( 0)
//   {
//     //    ixList.reserve(MAXRANK); 
//   }

//   ArrayIndexListT( const ArrayIndexListT& cp):
//     accessType(cp.accessType),
//     accessTypeInit(cp.accessTypeInit),
//     accessTypeAssocInit(cp.accessTypeAssocInit),
//     acRank(cp.acRank),
//     allIx( NULL),
//     ixListEnd( NULL),
//     nParam( cp.nParam)
//   {
//     //    ixList.reserve(MAXRANK); 
//     assert( cp.allIx == NULL);
//     assert( cp.ixListEnd == NULL);

//     for( SizeT i=0; i<cp.ixList.size(); ++i)
//       ixList.push_back( cp.ixList[i]->Dup());
//   }

//   // called after structure is fixed
//   ArrayIndexListT( ArrayIndexVectorT* ix): nParam(0)
//   {
//     assert( ix->size() != 0); // must be, from compiler
    
//     if( ix->size() > MAXRANK)
//       throw GDLException(NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

//     for( SizeT i=0; i<ix->size(); ++i)
//       {
// 	ixList.push_back( (*ix)[i]);
// 	nParam += (*ix)[i]->NParam();
//       }

//     if( ix->size() == 1)
//       {
// 	accessTypeInit = ONEDIM; // ok also for assoc
// 	accessTypeAssocInit = ONEDIM; // ok also for assoc
//       }
//     else
//       {
// 	SizeT nIndexed = 0;
// 	SizeT nScalar  = 0;
//  	for( SizeT i=0; (i+1)<ixList.size(); ++i)
// 	  {
// 	    // note: here we examine the actual type
// 	    if( dynamic_cast< ArrayIndexScalar*>(ixList[i]))  nScalar++;
// 	    if( dynamic_cast<ArrayIndexIndexed*>(ixList[i])) nIndexed++;
// 	  }
// 	if( nScalar == ixList.size()-1)
// 	  accessTypeAssocInit = ALLONE;
// 	else if( nIndexed == ixList.size()-1)
// 	  accessTypeAssocInit = ALLINDEXED; // might be ALLONE as well
// 	else if( nScalar + nIndexed < ixList.size()-1)
// 	  accessTypeAssocInit = NORMAL;
// 	else
// 	  accessTypeAssocInit = INDEXED_ONE;

// 	if( dynamic_cast< ArrayIndexScalar*>(ixList[ixList.size()-1]))  nScalar++;
// 	if( dynamic_cast<ArrayIndexIndexed*>(ixList[ixList.size()-1])) nIndexed++;

// 	if( nScalar == ixList.size())
// 	  accessTypeInit = ALLONE;
// 	else if( nIndexed == ixList.size())
// 	  accessTypeInit = ALLINDEXED; // might be ALLONE as well
// 	else if( nScalar + nIndexed < ixList.size())
// 	  accessTypeInit = NORMAL;
// 	else
// 	  accessTypeInit = INDEXED_ONE;
//       }
//   }    
  
//   void Clear()
//   {
//     delete allIx;
//     allIx = NULL;
    
//     if( ixListEnd != NULL) // revert assoc indexing
//       {
// 	ixList.push_back( ixListEnd);
// 	ixListEnd = NULL;
//       }
    
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
// 	 i != ixList.end(); ++i)
//       {	(*i)->Clear();}
//   }

//   void Init( IxExprListT& ix)
//   {
//     assert( allIx == NULL);
//     assert( ix.size() == nParam);
    
//     SizeT pIX = 0;
//     for( SizeT i=0; i<ixList.size(); ++i)
//       {
// 	SizeT ixNParam = ixList[ i]->NParam();
// 	if( ixNParam == 0) continue;
// 	if( ixNParam == 1) 
// 	  {
// 	    ixList[ i]->Init( ix[ pIX]);
// 	    pIX += 1;
// 	    continue;
// 	  }
// 	if( ixNParam == 2) 
// 	  {
// 	    ixList[ i]->Init( ix[ pIX], ix[ pIX+1]);
// 	    pIX += 2;
// 	    continue;
// 	  }
// 	if( ixNParam == 3) 
// 	  {
// 	    ixList[ i]->Init( ix[ pIX], ix[ pIX+1], ix[ pIX+2]);
// 	    pIX += 3;
// 	    continue;
// 	  }
//       }
//   }
  

//   // requires special handling
//   // used by Assoc_<> returns last index in lastIx, removes it
//   // and returns true is the list is empty
//   bool ToAssocIndex( SizeT& lastIx)
//   {
//     assert( ixListEnd == NULL);
    
//     ArrayIndexT* ixListEndTmp = ixList[ ixList.size()-1];

//     if( !ixListEndTmp->Scalar( lastIx))
//       throw GDLException(NULL,"Record number must be a scalar in this context.",true,false);

//     ixListEnd = ixListEndTmp;
//     ixList.pop_back();
    
//     return ixList.empty();
//   }

//   // set the root variable which is indexed by this ArrayIndexListT
//   void SetVariable( BaseGDL* var) 
//   {
//     assert( allIx == NULL);

//     // set acRank
//     acRank = ixList.size();

//     // for assoc variables last index is the record
//     if( var->IsAssoc()) 
//       {
// 	acRank--;
// 	accessType = accessTypeAssocInit;
// 	if( acRank == 0) return;
//       }
//     else
//       accessType = accessTypeInit;

//     // init access parameters
//     // special case: 1-dim access
//     if( acRank == 1)
//       {
// 	// need varDim here instead of var because of Assoc_<>
// 	nIterLimit[0]=ixList[0]->NIter( var->Size());
// 	nIx=nIterLimit[0];
	
// 	assert( accessType == ONEDIM);
// 	return;
//       }
    
//     if( accessType == ALLONE)
//       {
// 	var->Dim().Stride( varStride,acRank); // copy variables stride into varStride
// 	nIx = 1;
// 	return;
//       }

//     if( accessType == ALLINDEXED || accessType == INDEXED_ONE)
//       {
// 	SizeT i=0;
// 	for(; i<acRank; ++i)
// 	  if( !ixList[i]->Scalar())
// 	      break;
// 	if( i == acRank) // counted up to acRank
// 	  {
// 	    accessType = ALLONE;
// 	    var->Dim().Stride( varStride,acRank); // copy variables stride into varStride
// 	    nIx = 1;
// 	    return;
// 	  }
// 	// after break
// 	if( i > 0 || accessType == INDEXED_ONE)
// 	  accessType = NORMAL; // there was a scalar (and break because of non-scalar)
// 	else // i == 0 -> first was (actually) indexed 
// 	  {
// 	    ++i; // first was already non-scalar -> indexed
// 	    for(; i<acRank; ++i)
// 	      if( !ixList[i]->Indexed())
// 		break;
// 	    if( i < acRank)
// 	      accessType = NORMAL;
// 	  }
//       }
//     // accessType can be at this point:
//     // NORMAL
//     // ALLINDEXED
//     // both are the definite types here
//     assert( accessType == NORMAL || accessType == ALLINDEXED);
    
//     // set varDim from variable
//     const dimension& varDim  = var->Dim();
//     SizeT            varRank = varDim.Rank();

//     if( accessType == ALLINDEXED)
//       {
// 	nIx=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
// 	for( SizeT i=1; i<acRank; ++i)
// 	  {
// 	    SizeT nIter = ixList[i]->NIter( (i<varRank)?varDim[i]:1);
// 	    if( nIter != nIx)
// 	      throw GDLException(NULL,"All array subscripts must be of same size.",true,false);
// 	  }
	
// 	// in this case, having more index dimensions does not matter
// 	// indices are used only upto variables rank
// 	if( varRank < acRank) acRank = varRank;

// 	varDim.Stride( varStride,acRank); // copy variables stride into varStride

// 	return;
//       }

//     // NORMAL
//     nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
//     nIx = nIterLimit[0]; // calc number of assignments
//     stride[0]=1;
//     for( SizeT i=1; i<acRank; ++i)
//       {
// 	nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1);
// 	nIx *= nIterLimit[i]; // calc number of assignments

// 	stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride 
//       }
//     stride[acRank]=stride[acRank-1]*nIterLimit[acRank-1]; // index stride 

//     varDim.Stride( varStride,acRank); // copy variables stride into varStride
//   }

//   // structure of indexed expression
//   dimension GetDim()
//   {
//     if( accessType == ALLONE) return dimension(); // -> results in scalar
//     if( accessType == ONEDIM)
//       {
//  	if( ixList[0]->Scalar())
//  	  {
//  	    return dimension();
//  	  }
// 	else if( ixList[0]->Indexed())
// 	  {
// 	    return static_cast<ArrayIndexIndexed*>(ixList[0])->GetDim(); 
// 	    // gets structure of indexing array
// 	  }
// 	else
// 	  {
// 	    return dimension( nIterLimit, 1); // one dimensional if not indexed
// 	  }
//       }
//     if( accessType == ALLINDEXED)
//       { // always indexed
// 	return static_cast<ArrayIndexIndexed*>(ixList[0])->GetDim();
//       }
//     // accessType == NORMAL -> structure from indices
//     return dimension( nIterLimit, acRank);
//   }

//   SizeT N_Elements()
//   {
//     return nIx;
//   }

//   // returns 1-dim index for all elements
//   AllIxT* BuildIx()
//   {
//     if( allIx != NULL) return allIx;

//     if( accessType == ONEDIM)
//       {
// 	if( ixList[0]->Indexed())
// 	  {
// 	    allIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();
// 	    return allIx;
// 	  }
// 	else
// 	  {
// 	    if( nIx == 1)
// 	      {
// 		allIx = new AllIxT( ixList[0]->GetS(), 1);
// 		return allIx;
// 	      }

// 	    //	    allIx = new SizeT[ nIx];
// 	    allIx = new AllIxT( nIx);
// 	    SizeT s = ixList[0]->GetS();
// 	    SizeT ixStride = ixList[0]->GetStride();
// 	    if( ixStride <= 1) 
// 	      if( s != 0) 
// 		for( SizeT i=0; i<nIx; ++i)
// 		  (*allIx)[i] = i + s;
// 	      else
// 		for( SizeT i=0; i<nIx; ++i)
// 		  (*allIx)[i] = i;
// 	    else
// 	      if( s != 0) 
// 		for( SizeT i=0; i<nIx; ++i)
// 		  (*allIx)[i] = i * ixStride + s;
// 	      else
// 		for( SizeT i=0; i<nIx; ++i)
// 		  (*allIx)[i] = i * ixStride;
// 	    return allIx;
// 	  }
//       }

//     if( accessType == ALLONE)
//       {
// 	SizeT s = ixList[0]->GetS();
// 	for( SizeT l=1; l < acRank; ++l)
// 	  {
// 	    s += ixList[l]->GetS() * varStride[l]; 
// 	  }
// 	allIx = new AllIxT( 1);
// 	(*allIx)[0] = s;

// 	return allIx;
//       }

//     if( accessType == ALLINDEXED)
//       {
// 	// ALLINDEXED -> all ArrayIndexT::INDEXED
// 	allIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();
	
// 	for( SizeT l=1; l < acRank; ++l)
// 	  {
// 	    AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[l])->StealIx();
	    
// 	    for( SizeT i=0; i<nIx; ++i)
// 	      (*allIx)[i] += (*tmpIx)[i] * varStride[l];
	    
// 	    delete tmpIx;
// 	  }
// 	return allIx;
//       }

//     // NORMAL
//     // loop only over specified indices
//     // higher indices of variable are implicitely zero,
//     // therefore they are not checked in 'SetRoot'

//     allIx = new AllIxT( nIx);

//     // init allIx from first index
//     if( ixList[0]->Indexed())
//       {
// 	AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();

// 	for( SizeT i=0; i<nIx; ++i)
// 	  {
// 	    (*allIx)[ i] = (*tmpIx)[ i %  nIterLimit[0]];
// 	  }

// 	delete tmpIx;
//       }
//     else
//       {
// 	SizeT s = ixList[0]->GetS();
// 	SizeT ixStride = ixList[0]->GetStride();
	
// 	if( ixStride <= 1)
// 	  if( s != 0) 
// 	    for( SizeT i=0; i<nIx; ++i)
// 	      {
// 		(*allIx)[i] = (i %  nIterLimit[0]) + s; // stride[0], varStride[0] == 1
// 	      }
// 	  else
// 	    for( SizeT i=0; i<nIx; ++i)
// 	      {
// 		(*allIx)[i] = (i %  nIterLimit[0]); // stride[0], varStride[0] == 1
// 	      }
// 	else
// 	  if( s != 0) 
// 	    for( SizeT i=0; i<nIx; ++i)
// 	      {
// 		(*allIx)[i] = (i %  nIterLimit[0]) * ixStride + s; // stride[0], varStride[0] == 1
// 	      }
// 	  else
// 	    for( SizeT i=0; i<nIx; ++i)
// 	      {
// 		(*allIx)[i] = (i %  nIterLimit[0]) * ixStride; // stride[0], varStride[0] == 1
// 	      }
//       }

//     for( SizeT l=1; l < acRank; ++l)
//       {

// 	if( ixList[l]->Indexed())
// 	  {
// 	    AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[l])->StealIx();
// 	    //	    SizeT* tmpIx = ixList[l]->StealIx();
	    
// 	    for( SizeT i=0; i<nIx; ++i)
// 	      {
// 		(*allIx)[ i] += (*tmpIx)[ (i / stride[l]) %  nIterLimit[l]] * 
// 		  varStride[l];
// 	      }
	    
// 	    delete tmpIx;
// 	  }
// 	else
// 	  {
// 	    SizeT s = ixList[l]->GetS();
// 	    SizeT ixStride = ixList[l]->GetStride();
	    
// 	    if( ixStride <= 1)
// 	    if( s != 0) 
// 	      for( SizeT i=0; i<nIx; ++i)
// 		{
// 		  (*allIx)[i] += ((i / stride[l]) %  nIterLimit[l] + s) * 
// 		    varStride[l]; 
// 		}
// 	    else
// 	      for( SizeT i=0; i<nIx; ++i)
// 		{
// 		  (*allIx)[i] += ((i / stride[l]) %  nIterLimit[l]) * 
// 		    varStride[l]; 
// 		}
// 	    else // ixStride > 1 
// 	    if( s != 0) 
// 	      for( SizeT i=0; i<nIx; ++i)
// 		{
// 		  (*allIx)[i] += (((i / stride[l]) %  nIterLimit[l]) 
// 				  * ixStride + s) * varStride[l]; 
// 		}
// 	    else
// 	      for( SizeT i=0; i<nIx; ++i)
// 		{
// 		  (*allIx)[i] += ((i * ixStride / stride[l]) %  nIterLimit[l]) 
// 		    * ixStride * varStride[l]; 
// 		}
// 	  }
//       }
    
//     return allIx;
//   }

//   // returns one dim long ix in case of one element array index
//   // used by AssignAt functions
//   SizeT LongIx()
//   {
//     if( accessType == ONEDIM || acRank == 1)
// 	return ixList[0]->GetIx0();
    
//     SizeT dStart = ixList[0]->GetIx0();
//     for( SizeT i=1; i < acRank; ++i)
// 	dStart += ixList[i]->GetIx0() * varStride[ i];

//     return dStart;
//   }

//   void AssignAt( BaseGDL* var, BaseGDL* right)
//   {
//     // scalar case
//     if( ixList.size() == 1 && right->N_Elements() == 1 && !var->IsAssoc() &&
// 	ixList[0]->NIter( var->Size()) == 1 && var->Type() != STRUCT) 
//       {
// 	var->AssignAtIx( ixList[0]->GetIx0(), right);
// 	return;
//       }
    
//     SetVariable( var);
    
//     if( var->EqType( right))
//       {
// 	var->AssignAt( right, this); // assigns inplace
//       }
//     else
//       {
// 	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
// 	std::auto_ptr<BaseGDL> conv_guard( rConv);
	
// 	var->AssignAt( rConv, this); // assigns inplace
//       }
//   }

//   // optimized for one dimensional access
//   BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
//   {
//     if( ixList.size() == 1 && !var->IsAssoc() && var->Type() != STRUCT)
//       return ixList[0]->Index( var, ix);
    
//     // normal case
//     Init( ix);
//     SetVariable( var);
//     return var->Index( this);
//   }

//   // returns multi-dim index for 1st element
//   // used by InsAt functions
//   dimension GetDimIx0( SizeT& rank, SizeT& destStart)
//   {
//     if( accessType == ONEDIM)
//       {
// 	rank = 1;

// 	destStart = ixList[0]->GetIx0();

// 	return dimension( destStart);
//       }
    
//     SizeT dStart = 0;

//     SizeT actIx[ MAXRANK];
//     for( SizeT i=0; i < acRank; ++i)
//       {
// 	actIx[ i] = ixList[i]->GetIx0();

// 	dStart += actIx[ i] * varStride[ i];
//       }

//     destStart = dStart;
//     rank = acRank;
//     return dimension( actIx, acRank);
//   }
  

//   //  SizeT NDim() { return ixList.size();}
// };


#endif
