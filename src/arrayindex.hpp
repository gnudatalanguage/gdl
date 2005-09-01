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

typedef std::valarray<SizeT> AllIxT;
typedef std::vector<BaseGDL*> IxExprListT;

class ArrayIndexT
{
public:
  virtual void Init( BaseGDL*) { assert( false);}
  virtual void Init( BaseGDL*, BaseGDL*) { assert( false);}
  virtual void Init( BaseGDL*, BaseGDL*, BaseGDL*) { assert( false);}

  virtual void Clear() {}
  virtual ~ArrayIndexT() {}

  virtual SizeT GetIx0()=0;
  virtual SizeT GetS() { return 0;}
  virtual SizeT GetStride() { return 0;}
  
  virtual bool Scalar()           { return false;}
  virtual bool Scalar( SizeT& s_) { return false;}
  virtual bool Indexed()          { return false;}  

  virtual SizeT NIter( SizeT varDim)=0; 

  virtual SizeT NParam()=0;
};

// INDEXED or ONE [v]
class ArrayIndexIndexed: public ArrayIndexT
{
protected:
  bool      strictArrSubs;          // for compile_opt STRICTARRSUBS
  
  SizeT s;
  SizeT maxVal;

  AllIxT*    ix;
  dimension* ixDim; // keep dimension of ix

  // forbid c-i
  ArrayIndexIndexed( const ArrayIndexT& r) {}

public:
  SizeT NParam() { return 1;} // number of parameter to Init(...)

  SizeT GetS() { return s;}

  bool Scalar() { return (ix == NULL);}
  bool Scalar( SizeT& s_)
  { 
    s_ = s;
    return (ix == NULL);
  }

  bool Indexed() { return (ix != NULL);}

  dimension GetDim() { return *ixDim;}

  SizeT GetIx0()
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

  void Clear()
  {
    maxVal = 0;
    delete ixDim;
    ixDim = NULL;
    delete ix; 
    ix = NULL; // marker ONE or INDEXED
  }

  void Init( BaseGDL* ix_) 
  {
    if( ix_->Rank() == 0) // type ONE
      {
	int ret = ix_->Scalar2index(s);
	if( ret == -1) // index < 0
	  {
	    throw 
	      GDLException( "Subscript range values of the"
			    " form low:high must be >= 0, < size,"
			    " with low <= high.");
	  }
	return;
      }

    // type INDEXED
    DType dType = ix_->Type();

    assert( dType != UNDEF);
    assert( maxVal == 0);

    int typeCheck = DTypeOrder[ dType];
    if( typeCheck >= 100)
      throw GDLException("Type not allowed as subscript.");
    
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
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
	  DInt minVal = src->min();
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
	  break;
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
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
	  break;
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
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
	  break;
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
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
		}
	      if( l < 0)
		{
		  if( strictArrSubs)
		    throw GDLException("Array used to subscript array "
				       "contains out of range (<0) subscript.");
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
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
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
		(*ix)[i] = 0;
	      }
	    else
	      {
		(*ix)[i]= Real2Int<DLong,double>(real((*src)[i])); 
		if( (*ix)[i] > maxVal) maxVal = (*ix)[i];
	      }
	  break;
	}
      }
  } 

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim) 
  {
    if( ix == NULL) // ONE
      {
	if( s >= varDim)
	  throw GDLException("Subscript out of range [i].");
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
	throw GDLException("Array used to subscript array "
			   "contains out of range subscript.");
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
private:
  AllIxT*    ixOri;
  SizeT      maxIx;

public:
  ~CArrayIndexIndexed() { delete ixOri;}

  CArrayIndexIndexed( BaseGDL* c, bool strictArrSubs_ = false): 
    ArrayIndexIndexed( strictArrSubs_), ixOri( NULL), maxIx( 0)
  {
    ArrayIndexIndexed::Init( c);
    ixOri = ix;
    ix = NULL;
    if( ixOri != NULL) maxIx = ixOri->max();
  }

  SizeT NParam() { return 0;} // number of parameter to Init(...)
  void Clear() { delete ix; ix=NULL;} // note that ixDim is untouched

  // make the following work even before call to NIter(...)
  bool Scalar() { return (ixOri == NULL);}
  bool Scalar( SizeT& s_)
  { 
    s_ = s;
    return (ixOri == NULL);
  }
  bool Indexed() { return (ixOri != NULL);}

  // old (before ixOri): special here no stealing is allowed
  //  AllIxT* StealIx() { return new AllIxT( *ix);} 

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim) 
  {
    if( ixOri == NULL) // ONE
      {
	if( s >= varDim)
	  throw GDLException("Subscript out of range [i].");
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
	    throw GDLException("Array used to subscript array "
			       "contains out of range subscript.");
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
  SizeT NParam() { return 0;} // number of parameter to Init(...)

  void Init() {};

  SizeT GetIx0() { return 0;}

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
private:
  SizeT s;

public:
  SizeT NParam() { return 1;} // number of parameter to Init(...)

  SizeT GetS() { return s;}
  SizeT GetIx0() { return s;}

  void Init( BaseGDL* s_)
  {
    int retMsg=s_->Scalar2index(s);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException( "Internal error: Scalar2index:"
			  " 1st index empty"); 
	else
	  throw 
	    GDLException( "Expression must be a scalar"
			  " in this context."); 
      }
    if( retMsg == -1) // index < 0
      {
	throw 
	  GDLException( "Subscript range values of the"
			" form low:high must be >= 0, < size, "
			"with low <= high.");
      }
  }

  SizeT NIter( SizeT varDim)
  {
    if( s >= varDim)
      throw GDLException("Subscript out of range [s:*].");
    return (varDim - s);
  }
};

class CArrayIndexORange: public ArrayIndexORange
{
public:
  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexORange( BaseGDL* c): ArrayIndexORange()
  {
    ArrayIndexORange::Init( c);
  }
};

// [s:e]
class ArrayIndexRange: public ArrayIndexT
{
private:
  SizeT s,e;

public:
  SizeT NParam() { return 2;} // number of parameter to Init(...)

  SizeT GetS() { return s;}
  SizeT GetIx0() { return s;}

  void Init( BaseGDL* s_, BaseGDL* e_)
  {
    SizeT retMsg=s_->Scalar2index(s);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException( "Internal error: Scalar2index: 1st index empty."); 
	else
	  throw 
	    GDLException( "Expression must be a scalar in this context."); 
      }
    if( retMsg == -1) // index < 0
      {
	throw 
	  GDLException( "Subscript range values of the form low:high " 
			"must be >= 0, < size, with low <= high.");
      }
            
    retMsg=e_->Scalar2index(e);
    if( retMsg == 0) // index empty or array
      {
	if( e_->N_Elements() == 0)
	  throw 
	    GDLException( "Internal error: Scalar2index: 2nd index empty."); 
	else
	  throw 
	    GDLException( "Expression must be a scalar in this context."); 
      }
    if( retMsg == -1) // index < 0
      {
	throw 
	  GDLException( "Subscript range values of the form low:high " 
			"must be >= 0, < size, with low <= high.");
      }
    
    if( e < s)
      {
	throw 
	  GDLException( " Subscript range values of the form low:high " 
			"must be >= 0, < size, with low <= high");
      }
  }


  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
    if( e >= varDim)
      throw GDLException("Subscript out of range [s:e].");
    return (e - s + 1);
  }
};

class CArrayIndexRange: public ArrayIndexRange
{
public:
  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexRange( BaseGDL* c1, BaseGDL* c2): ArrayIndexRange()
  {
    ArrayIndexRange::Init( c1, c2);
  }
};

// [s:*:st]
class ArrayIndexORangeS: public ArrayIndexT
{
private:
  SizeT s,stride;

public:
  SizeT NParam() { return 2;} // number of parameter to Init(...)

  SizeT GetS() { return s;}
  SizeT GetStride() { return stride;}
  SizeT GetIx0() { return s;}

  void Init( BaseGDL* s_, BaseGDL* stride_)
  {
    int retMsg=s_->Scalar2index( s);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2index:"
			  " 1st index empty"); 
	else
	  throw 
	    GDLException(  "Expression must be a scalar"
			  " in this context."); 
      }
    if( retMsg == -1) // index < 0
      {
	throw 
	  GDLException(  "Subscript range values of the"
			" form low:high must be >= 0, < size, with low <= high.");
      }
    // stride
    retMsg=stride_->Scalar2index( stride);
    if( retMsg == 0) // index empty or array
      {
	if( stride_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2index:"
			  " stride index empty"); 
	else
	  throw 
	    GDLException(  "Expression must be a scalar"
			  " in this context."); 
      }
    if( retMsg == -1 || stride == 0) // stride <= 0
      {
	throw 
	  GDLException(  "Range subscript stride must be >= 1.");
      }
  }


  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
    if( s >= varDim)
      throw GDLException("Subscript out of range [s:*].");
    return (varDim - s + stride - 1)/stride;
  }
};

class CArrayIndexORangeS: public ArrayIndexORangeS
{
public:
  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexORangeS( BaseGDL* c1, BaseGDL* c2): ArrayIndexORangeS()
  {
    ArrayIndexORangeS::Init( c1, c2);
  }
};

// [s:e:st]
class ArrayIndexRangeS: public ArrayIndexT
{
private:
  SizeT s,e,stride;

public:
  SizeT NParam() { return 3;} // number of parameter to Init(...)

  SizeT GetS() { return s;}
  SizeT GetStride() { return stride;}
  SizeT GetIx0() { return s;}

  void Init( BaseGDL* s_, BaseGDL* e_, BaseGDL* stride_)
  {
    int retMsg=s_->Scalar2index(s);
    if( retMsg == 0) // index empty or array
      {
	if( s_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2index: 1st index empty."); 
	else
	  throw 
	    GDLException(  "Expression must be a scalar in this context."); 
      }
    if( retMsg == -1) // index < 0
      {
	throw 
	  GDLException(  "Subscript range values of the form low:high " 
			"must be >= 0, < size, with low <= high.");
      }
            
    retMsg=e_->Scalar2index(e);
    if( retMsg == 0) // index empty or array
      {
	if( e_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2index: 2nd index empty."); 
	else
	  throw 
	    GDLException(  "Expression must be a scalar in this context."); 
      }
    if( retMsg == -1) // index < 0
      {
	throw 
	  GDLException(  "Subscript range values of the form low:high " 
			"must be >= 0, < size, with low <= high.");
      }
            
    if( e < s)
      {
	throw 
	  GDLException(  "Subscript range values of the form low:high " 
			"must be >= 0, < size, with low <= high");
      }
                            
    // stride
    retMsg=stride_->Scalar2index(stride);
    if( retMsg == 0) // index empty or array
      {
	if( stride_->N_Elements() == 0)
	  throw 
	    GDLException(  "Internal error: Scalar2index:"
			  " stride index empty"); 
	else
	  throw 
	    GDLException(  "Expression must be a scalar"
			  " in this context."); 
      }
    if( retMsg == -1 || stride == 0) // stride <= 0
      {
	throw 
	  GDLException(  "Range subscript stride must be >= 1.");
      }
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim)
  {
    if( e >= varDim)
      {
	throw GDLException("Subscript out of range [s:e:st].");
      }
    return (e - s + stride)/stride;
  }
};

class CArrayIndexRangeS: public ArrayIndexRangeS
{
public:
  SizeT NParam() { return 0;} // number of parameter to Init(...)

  CArrayIndexRangeS( BaseGDL* c1, BaseGDL* c2, BaseGDL* c3): 
    ArrayIndexRangeS()
  {
    ArrayIndexRangeS::Init( c1, c2, c3);
  }
};

class ArrayIndexListT
{
private:
  std::vector<ArrayIndexT*> ixList;

  enum AccessType {
    NORMAL=0, // mixed
    ONEDIM,
    ALLSAME,
    ALLONE    // all ONE
  };

  AccessType accessTypeInit;     // possible access type non assoc
  AccessType accessTypeAssocInit;// possible access type for assoc
  AccessType accessType;         // actual access type
  SizeT    acRank;               // rank upto which indexing is done
  SizeT    nIterLimit[MAXRANK];  // for each dimension, how many iterations
  SizeT    stride[MAXRANK+1];    // for each dimension, how many iterations
  SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

  AllIxT* allIx;

  ArrayIndexT* ixListEnd; // for assoc index

  SizeT nParam; // number of (BaseGDL*) parameters
public:    
  
  SizeT NParam() { return nParam;}

  ~ArrayIndexListT()
  {
    delete allIx;
    for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
	 i != ixList.end(); ++i)
      {	delete *i;}
  }

  // constructor
  ArrayIndexListT():
    accessType(NORMAL),
    acRank(0),
    allIx( NULL),
    ixListEnd( NULL),
    nParam( 0)
  {
    //    ixList.reserve(MAXRANK); 
  }
  
  void Clear()
  {
    delete allIx;
    allIx = NULL;
    
    if( ixListEnd != NULL) // revert assoc indexing
      {
	ixList.push_back( ixListEnd);
	ixListEnd = NULL;
      }
    
    for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
	 i != ixList.end(); ++i)
      {	(*i)->Clear();}
  }

  void Init( IxExprListT& ix)
  {
    assert( allIx == NULL);
    assert( ix.size() == nParam);
    
    SizeT pIX = 0;
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	SizeT ixNParam = ixList[ i]->NParam();
	if( ixNParam == 0) continue;
	if( ixNParam == 1) 
	  {
	    ixList[ i]->Init( ix[ pIX]);
	    pIX += ixNParam;
	    continue;
	  }
	if( ixNParam == 2) 
	  {
	    ixList[ i]->Init( ix[ pIX], ix[ pIX+1]);
	    pIX += ixNParam;
	    continue;
	  }
	if( ixNParam == 3) 
	  {
	    ixList[ i]->Init( ix[ pIX], ix[ pIX+1], ix[ pIX+2]);
	    pIX += ixNParam;
	    continue;
	  }
      }
  }
  
  // called after structure is fixed
  void Freeze()
  {
    assert( ixList.size() != 0); // must be, from compiler
    
    if( ixList.size() == 1)
      {
	accessTypeInit = ONEDIM; // ok also for assoc
	accessTypeAssocInit = ONEDIM; // ok also for assoc
      }
    else
      {
	accessTypeInit = ALLSAME;
	accessTypeAssocInit = ALLSAME;
	for( SizeT i=0; i<ixList.size(); ++i)
	  if( !ixList[i]->Indexed())
	    {
	      accessTypeInit = NORMAL;
	      if( i < (ixList.size()-1))
		accessTypeAssocInit = NORMAL;
	      break;
	    }
      }
    // NORMAL can also be ALLONE
  }

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    assert( ixListEnd == NULL);
    
    ArrayIndexT* ixListEndTmp = ixList[ ixList.size()-1];

    if( !ixListEndTmp->Scalar( lastIx))
      throw GDLException( "Record number must be a scalar in this context.");

    ixListEnd = ixListEndTmp;
    ixList.pop_back();
    
    return ixList.empty();
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // set acRank
    acRank = ixList.size();

    // for assoc variables last index is the record
    if( var->IsAssoc())
      {
	--acRank;
	if( acRank == 0) return;
	accessType = accessTypeAssocInit;
      }
    else
      accessType = accessTypeInit;
    
    // init access parameters
    // special case: 1-dim access
    if( acRank == 1)
      {
	// need varDim here instead of var because of Assoc_<>
	nIterLimit[0]=ixList[0]->NIter( var->Size()); 
	nIx=nIterLimit[0];

	assert(	accessType == ONEDIM);
      }
    else
      { // normal (multi-dim) access
	// set varDim from variable
	const dimension& varDim  = var->Dim();
	SizeT            varRank = varDim.Rank();

	if( accessType == ALLSAME) 
	  {
	    nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1); 
	    nIx = nIterLimit[0]; // calc number of assignments
	    stride[0]=1;
	    for( SizeT i=1; i<acRank; ++i)
	      {
		nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1); 
		if( nIterLimit[i] != nIterLimit[0])
		  throw GDLException( "All array subscripts "
				      "must be same size.");
		stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride 
	      }
	    // index stride 
	    stride[acRank]=stride[acRank-1]*nIterLimit[acRank-1]; 

	    // copy variables stride into varStride
	    varDim.Stride( varStride,acRank); 

	    // in this case, having more index dimensions does not matter
	    // indices are used only upto variables rank
	    if( varRank < acRank) acRank = varRank;
	  }
	else
	  {
	    //	accessType == NORMAL;
	    nIx=1;
	    for( SizeT i=0; i<acRank; ++i)
	      {
		//  if( !ixList[i]->Indexed()) accessType = NORMAL;
		nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1); 
		nIx *= nIterLimit[i]; // calc number of assignments
	      }
	
	    stride[0]=1;
	    for( SizeT i=1; i<=acRank; ++i)
	      {
		stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride 
	      }
	    // copy variables stride into varStride
	    varDim.Stride( varStride,acRank);

	    // ALLONE?
	    if( nIx == 1)
	      {
		accessType = ALLONE;
		for( SizeT i=0; i<acRank; ++i)
		  {
		    if( !ixList[i]->Scalar())
		      {
			accessType = NORMAL;
			break;
		      }
		  }
	      }
	  }
      }
  }

  // structure of indexed expression
  dimension GetDim()
  {
    if( accessType == ALLONE) return dimension(); // -> results in scalar
    if( accessType == ONEDIM)
      {
	if( ixList[0]->Indexed())
	  {
	    return static_cast<ArrayIndexIndexed*>(ixList[0])->GetDim(); 
	    // gets structure of indexing array
	  }
 	else if( ixList[0]->Scalar())
 	  {
 	    return dimension();
 	  }
	else
	  {
	    return dimension( nIterLimit, 1); // one dimensional if not indexed
	  }
      }
    if( accessType == ALLSAME)
      { // always indexed
	return static_cast<ArrayIndexIndexed*>(ixList[0])->GetDim();
      }
    // accessType == NORMAL -> structure from indices
    return dimension( nIterLimit, acRank);
  }

  SizeT N_Elements()
  {
    return nIx;
  }

  // returns 1-dim index for all nTh elements
  AllIxT* BuildIx()
  {
    if( allIx != NULL) return allIx;

    if( accessType == ONEDIM)
      {
	if( ixList[0]->Indexed())
	  {
	    allIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();
	    return allIx;
	  }
	else
	  {
	    if( nIx == 1)
	      {
		allIx = new AllIxT( ixList[0]->GetS(), 1);
		return allIx;
	      }

	    //	    allIx = new SizeT[ nIx];
	    allIx = new AllIxT( nIx);
	    SizeT s = ixList[0]->GetS();
	    SizeT ixStride = ixList[0]->GetStride();
	    if( ixStride <= 1) 
	      if( s != 0) 
		for( SizeT i=0; i<nIx; ++i)
		  (*allIx)[i] = i + s;
	      else
		for( SizeT i=0; i<nIx; ++i)
		  (*allIx)[i] = i;
	    else
	      if( s != 0) 
		for( SizeT i=0; i<nIx; ++i)
		  (*allIx)[i] = i * ixStride + s;
	      else
		for( SizeT i=0; i<nIx; ++i)
		  (*allIx)[i] = i * ixStride;
	    return allIx;
	  }
      }

    if( accessType == ALLSAME)
      {
	// ALLSAME -> all ArrayIndexT::INDEXED
	allIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();
	
	for( SizeT l=1; l < acRank; ++l)
	  {
	    AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[l])->StealIx();
	    
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] += (*tmpIx)[i] * varStride[l];
	    
	    delete tmpIx;
	  }
	return allIx;
      }

    // NORMAL or ALLONE
    // loop only over specified indices
    // higher indices of variable are implicitely zero,
    // therefore they are not checked in 'SetRoot'
    allIx = new AllIxT( nIx);

    // init allIx from first index
    if( ixList[0]->Indexed())
      {
	AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();

	for( SizeT i=0; i<nIx; ++i)
	  {
	    (*allIx)[ i] = (*tmpIx)[ i %  nIterLimit[0]];
	  }

	delete tmpIx;
      }
    else
      {
	SizeT s = ixList[0]->GetS();
	SizeT ixStride = ixList[0]->GetStride();
	
	if( ixStride <= 1)
	  if( s != 0) 
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]) + s; // stride[0], varStride[0] == 1
	      }
	  else
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]); // stride[0], varStride[0] == 1
	      }
	else
	  if( s != 0) 
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]) * ixStride + s; // stride[0], varStride[0] == 1
	      }
	  else
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]) * ixStride; // stride[0], varStride[0] == 1
	      }
      }

    for( SizeT l=1; l < acRank; ++l)
      {

	if( ixList[l]->Indexed())
	  {
	    AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[l])->StealIx();
	    //	    SizeT* tmpIx = ixList[l]->StealIx();
	    
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[ i] += (*tmpIx)[ (i / stride[l]) %  nIterLimit[l]] * 
		  varStride[l];
	      }
	    
	    delete tmpIx;
	  }
	else
	  {
	    SizeT s = ixList[l]->GetS();
	    SizeT ixStride = ixList[l]->GetStride();
	    
	    if( ixStride <= 1)
	    if( s != 0) 
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += ((i / stride[l]) %  nIterLimit[l] + s) * 
		    varStride[l]; 
		}
	    else
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += ((i / stride[l]) %  nIterLimit[l]) * 
		    varStride[l]; 
		}
	    else // ixStride > 1 
	    if( s != 0) 
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += (((i / stride[l]) %  nIterLimit[l]) 
				  * ixStride + s) * varStride[l]; 
		}
	    else
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += ((i * ixStride / stride[l]) %  nIterLimit[l]) 
		    * ixStride * varStride[l]; 
		}
	  }
      }
    
    return allIx;
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& rank, SizeT& destStart)
  {
    if( accessType == ONEDIM)
      {
	rank = 1;

	destStart = ixList[0]->GetIx0();

	return dimension( destStart);
      }
    
    SizeT dStart = 0;

    SizeT actIx[ MAXRANK];
    for( SizeT i=0; i < acRank; ++i)
      {
	actIx[ i] = ixList[i]->GetIx0();

	dStart += actIx[ i] * varStride[ i];
      }

    destStart = dStart;
    rank = acRank;
    return dimension( actIx, acRank);
  }
  
  void push_back( ArrayIndexT* pb)
  {
    ixList.push_back( pb);

    if( ixList.size() > MAXRANK)
      throw GDLException("Maximum of "+MAXRANK_STR+" dimensions allowed.");

    nParam += pb->NParam();
  }
};

class ArrayIndexListGuard
{
private:
  ArrayIndexListT* aL;
public:
  ArrayIndexListGuard(): aL( NULL) {}
  ~ArrayIndexListGuard() 
  {
    if( aL != NULL)
      aL->Clear();
  }
  void reset( ArrayIndexListT* aL_) { aL = aL_;}
  ArrayIndexListT* release() { ArrayIndexListT* res = aL; aL = NULL; return res;}
};

#endif
