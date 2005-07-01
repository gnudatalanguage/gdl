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

//#include <valarray>
#include <vector>

#include "datatypes.hpp"
#include "real2int.hpp"

typedef std::vector<SizeT> AllIxT;

class ArrayIndexT
{
public:
  enum IxType {
    INDEXED=0, // explicit indices
    ALL,       // *
    ONE,       // scalar
    ORANGE,    // open range s:*
    RANGE,      // s:e
    ORANGE_S,    // open range with stride s:*:stride
    RANGE_S      // s:e:stride
  };

private:
  IxType     t;
  SizeT      s,e,stride;
  AllIxT*    ix;
  //  SizeT*     ix;
  //  SizeT      nElem; // for ix
  dimension* ixDim; // keep dimension of ix

  // forbid c-i
  ArrayIndexT( const ArrayIndexT& r) {}

  // get nth index
  SizeT GetIx( SizeT nth)
  {
    if( t == 0) return (*ix)[nth]; // from array
    //if( t == 2) return s;        // scalar (nth == 0)
    return nth + s;
  }

public:

  //  SizeT* StealIx() { SizeT* ret = ix; ix = NULL; return ret;} 
  AllIxT* StealIx() { AllIxT* ret = ix; ix = NULL; return ret;} 
  
  ~ArrayIndexT() 
  {
    delete ix;
    //    delete[] ix;
    delete   ixDim;
  }

  // [[ix]] (type 0)
  ArrayIndexT( BaseGDL* ix_): 
    t(INDEXED), 
    s(0), e(0), stride(0), 
    ix( NULL), ixDim( NULL) 
  {
    DType dType = ix_->Type();

    assert( dType != UNDEF);

    int typeCheck = DTypeOrder[ dType];
    if( typeCheck >= 100)
      throw GDLException("Type not allowed as subscript.");
    
    SizeT nElem = ix_->N_Elements();
    //    ix = new SizeT[ nElem]; // allocate array
    ix = new AllIxT( nElem);

    ixDim = new dimension( ix_->Dim());

    switch( dType)
      {
      case BYTE:
	{
	  DByteGDL* src = static_cast<DByteGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= (*src)[i]; 
	  return;
	}
      case INT:
	{
	  DIntGDL* src = static_cast<DIntGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= static_cast<DLong>((*src)[i]); 
	  return;
	}
      case UINT:
	{
	  DUIntGDL* src = static_cast<DUIntGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= (*src)[i]; 
	  return;
	}
      case LONG:
	{
	  DLongGDL* src = static_cast<DLongGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= (*src)[i]; 
	  return;
	}
      case ULONG:
	{
	  DULongGDL* src = static_cast<DULongGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= (*src)[i]; 
	  return;
	}
      case LONG64:
	{
	  DLong64GDL* src = static_cast<DLong64GDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= (*src)[i]; 
	  return;
      }
      case ULONG64:
	{
	  DULong64GDL* src = static_cast<DULong64GDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= (*src)[i]; 
	  return;
	}
      case FLOAT: 
	{
	  DFloatGDL* src = static_cast<DFloatGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= Real2Int<DLong,float>((*src)[i]); 
	  return;
	}
      case DOUBLE: 
	{
	  DDoubleGDL* src = static_cast<DDoubleGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= Real2Int<DLong,double>((*src)[i]); 
	  return;
	}
      case STRING: 
	{
	  DStringGDL* src = static_cast<DStringGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    {
	      const char* cStart=(*src)[i].c_str();
	      char* cEnd;
	      (*ix)[i]=strtol(cStart,&cEnd,10);
	      if( cEnd == cStart)
		{
		  Warning("Type conversion error: "
			  "Unable to convert given STRING to LONG.");
		}
	    }
	  return;
	}
      case COMPLEX: 
	{
	  DComplexGDL* src = static_cast<DComplexGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= Real2Int<DLong,float>(real((*src)[i])); 
	  return;
	}
      case COMPLEXDBL: 
	{
	  DComplexDblGDL* src = static_cast<DComplexDblGDL*>( ix_);
	  for( SizeT i=0; i < nElem; ++i)
	    (*ix)[i]= Real2Int<DLong,double>(real((*src)[i])); 
	  return;
	}
      }
  } 
  
  // [*] (type 1)
  ArrayIndexT(): t(ALL), s(0), e(0), stride(0), ix(NULL), ixDim( NULL)  
  {} 

  // [s] (type 2) || [s:*] (type 3)  || [s:e] (type 4) || 
  // [s:*:stride] (type 5)  || [s:e:stride] (type 6) 
  ArrayIndexT( IxType t_, SizeT s_, SizeT e_=0, SizeT stride_=0): 
    t(t_), s(s_), e(e_), stride( stride_), 
    ix(NULL), ixDim( NULL)  
  {}
  
  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim, bool strictArrSubs) 
  {
    if( t == INDEXED) 
      {
	//	SizeT nElem=ix->N_Elements();
	
	SizeT upper=varDim-1;
	if( strictArrSubs)
	  { // strictArrSubs -> exception if out of bounds
	    for( SizeT i=0; i < ix->size(); ++i)
	      if( ((*ix)[i] < 0) || ((*ix)[i] > upper))
		throw GDLException("Array used to subscript array "
				   "contains out of range subscript.");
	  }
	else
	  {
	    for( SizeT i=0; i < ix->size(); ++i)
	      {
		if( (*ix)[i] < 0) (*ix)[i]=0; 
		else if( (*ix)[i] > upper) (*ix)[i]=upper;
		//else if( (ix)[i] > static_cast<DLong>(upper)) (ix)[i]=upper;
	      }
	  }
	return ix->size(); 
      }
    if( t == ALL) 
      {
	return varDim;
      }
    if( t == ONE) 
      {
	if( s >= varDim)
	  throw GDLException("Subscript out of range [i].");
	return 1;
      }
    if( t == ORANGE) 
      {
	if( s >= varDim)
	  throw GDLException("Subscript out of range [s:*].");
	return (varDim - s);
      }
    if( t == RANGE)
      {
	if( e >= varDim)
	  throw GDLException("Subscript out of range [s:e].");
	return (e - s + 1);
      }
    if( t == ORANGE_S) 
      {
	if( s >= varDim)
	  throw GDLException("Subscript out of range [s:*].");
	return (varDim - s + stride - 1)/stride;
      }
    // if( t == RANGE_S)
      {
	if( e >= varDim)
	  throw GDLException("Subscript out of range [s:e].");
	return (e - s + stride)/stride;
      }
  }

  bool Scalar()
  {
    return (t == ONE);
  }

  bool Scalar( SizeT& s_)
  {
    s_=s;
    return (t == ONE);
  }

  bool Indexed()
  {
    return (t == INDEXED);
  }

  dimension GetDim()
  {
    return *ixDim;
    //    return ix->Dim();
  }
  
  friend class ArrayIndexListT;
};

class ArrayIndexListT
{
private:
  std::vector<ArrayIndexT*> ixList;

  bool      strictArrSubs;          // for compile_opt STRICTARRSUBS
  
  enum AccessType {
    NORMAL=0, // mixed
    ONEDIM,
    ALLSAME,
    ALLONE    // all ONE
  };

  AccessType accessType;
  SizeT    acRank;               // rank upto which indexing is done
  SizeT    nIterLimit[MAXRANK];  // for each dimension, how many iterations
  SizeT    stride[MAXRANK+1];    // for each dimension, how many iterations
  SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

  //  SizeT    *allIx;               // index list 
  AllIxT* allIx;

public:    
  ~ArrayIndexListT()
  {
    //    delete[] allIx;
    delete allIx;
    for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
	 i != ixList.end(); ++i)
      {	delete *i;}
  }

  // constructor
  ArrayIndexListT( bool strictArrSubs_ = false):
    strictArrSubs( strictArrSubs_),
    accessType(NORMAL),
    acRank(0),
    allIx( NULL)
  {
    ixList.reserve(MAXRANK); 
  }

  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    ArrayIndexT* ixListEnd = ixList[ ixList.size()-1];

    if( !ixListEnd->Scalar( lastIx))
      throw GDLException( "Record number must be a scalar in this context.");

    delete ixListEnd;
    ixList.pop_back();

    return ixList.empty();
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    //    std::cout << "SetVariable: "; var->ToStream( std::cout); std::cout << std::endl;
    assert( allIx == NULL);

    // set acRank
    acRank = ixList.size();

    // for assoc variables last index is the record
    if( var->IsAssoc()) acRank--;
    if( acRank == 0) return;

    // set varDim from variable
    const dimension& varDim  = var->Dim();
    SizeT            varRank = varDim.Rank();

    // init access parameters
    // special case: 1-dim access
    if( acRank == 1)
      {
	// need varDim here instead of var because of Assoc_<>
	nIterLimit[0]=ixList[0]->NIter( var->Size(), strictArrSubs); 
	nIx=nIterLimit[0];
	
	accessType = ONEDIM;
      }
    else
      { // normal (multi-dim) access
	accessType = ALLSAME;
	
	nIx=1;
	for( SizeT i=0; i<acRank; ++i)
	  {
	    if( !ixList[i]->Indexed()) accessType = NORMAL;
	    
	    nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1, 
					   strictArrSubs); 
	    nIx *= nIterLimit[i]; // calc number of assignments
	  }
	
	stride[0]=1;
	for( SizeT i=1; i<=acRank; ++i)
	  {
	    stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride 
	  }
	varDim.Stride( varStride,acRank); // copy variables stride into varStride
	
	if( accessType == ALLSAME) 
	  {
	    // check if all indexing arrays have the same size
	    for( SizeT i=1; i<acRank; ++i)
	      if( nIterLimit[i] != nIterLimit[0])
		throw GDLException( "All array subscripts must be same size.");
	    
	    // number of elements accessed is size of one index
	    nIx = nIterLimit[0];
	    // in this case, having more index dimensions does not matter
	    // indices are used only upto variables rank
	    if( varRank < acRank) acRank = varRank;
	  }
	else // might be ALLONE
	  {
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
	    return ixList[0]->GetDim(); // gets structure of indexing array
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
	return ixList[0]->GetDim();
      }
    // accessType == NORMAL -> structure from indices
    return dimension( nIterLimit, acRank);
  }

  SizeT N_Elements()
  {
    return nIx;
  }

//   // returns 1-dim index for nTh element
//   SizeT GetIx( SizeT nTh)
//   {
//     if( accessType == ONEDIM)
//       {
// 	return ixList[0]->GetIx( nTh);
//       }

//     if( accessType == ALLSAME)
//       {
// 	SizeT actIx=0;

// 	for( SizeT i=0; i < acRank; ++i)
// 	  {
// 	    actIx += ixList[i]->GetIx( nTh) * varStride[i];
// 	  }
// 	return actIx;
//       }

//     // NORMAL or ALLONE
//     // loop only over specified indices
//     // higher indices of variable are implicitely zero,
//     // therefore they are not checked in 'SetRoot'
//     SizeT actIx=0;

//     for( SizeT i=0; i < acRank; ++i)
//       {
// 	actIx += ixList[i]->GetIx( (nTh / stride[i]) % nIterLimit[i]) * 
// 	  varStride[i];
//       }

//     return actIx;
//   }

  // returns 1-dim index for all nTh elements
  AllIxT* BuildIx()
  {
    if( allIx != NULL) return allIx;

    if( accessType == ONEDIM)
      {
	if( ixList[0]->t == ArrayIndexT::INDEXED)
	  allIx = ixList[0]->StealIx();
	else
	  {
	    //	    allIx = new SizeT[ nIx];
	    allIx = new AllIxT( nIx);
	    SizeT s = ixList[0]->s;
	    SizeT ixStride = ixList[0]->stride;
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
	  }
	return allIx;
      }

    if( accessType == ALLSAME)
      {
	// ALLSAME -> all ArrayIndexT::INDEXED
	allIx = ixList[0]->StealIx();

// 	if( varStride[0] != 1) // always 1
// 	  for( SizeT i=0; i<nIx; ++i)
// 	    (*allIx)[i] *= varStride[0];
	
	for( SizeT l=1; l < acRank; ++l)
	  {
	    AllIxT* tmpIx = ixList[ l]->StealIx();
	    
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] += (*tmpIx)[i] * varStride[l];
	    
	    //	    delete[] tmpIx;
	    delete tmpIx;
	  }
	return allIx;
      }

    // NORMAL or ALLONE
    // loop only over specified indices
    // higher indices of variable are implicitely zero,
    // therefore they are not checked in 'SetRoot'
    allIx = new AllIxT( nIx);
    //    allIx = new SizeT[ nIx];
    
    // init allIx from first index
    if( ixList[0]->t == ArrayIndexT::INDEXED)
      {
	AllIxT* tmpIx = ixList[ 0]->StealIx();
	//SizeT* tmpIx = ixList[0]->StealIx();

	for( SizeT i=0; i<nIx; ++i)
	  {
	    (*allIx)[ i] = (*tmpIx)[ i %  nIterLimit[0]];
	  }

	//	delete[] tmpIx;
	delete tmpIx;
      }
    else
      {
	SizeT s = ixList[0]->s;
	SizeT ixStride = ixList[0]->stride;
	
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

	if( ixList[l]->t == ArrayIndexT::INDEXED)
	  {
	    AllIxT* tmpIx = ixList[ l]->StealIx();
	    //	    SizeT* tmpIx = ixList[l]->StealIx();
	    
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[ i] += (*tmpIx)[ (i / stride[l]) %  nIterLimit[l]] * 
		  varStride[l];
	      }
	    
	    //	    delete[] tmpIx;
	    delete tmpIx;
	  }
	else
	  {
	    SizeT s = ixList[l]->s;
	    SizeT ixStride = ixList[l]->stride;
	    
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

  // returns multi-dim index for nTh element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& rank)
  {
    const SizeT nTh = 0;
    if( accessType == ONEDIM)
      {
	rank = 1;
	return dimension( ixList[0]->GetIx( nTh));
      }
    
    SizeT actIx[ MAXRANK];
    for( SizeT i=0; i < acRank; ++i)
      {
	actIx[ i] = ixList[i]->GetIx( nTh);
      }

    rank = acRank;
    return dimension( actIx, acRank);
  }
  
  void push_back( ArrayIndexT* pb)
  {
    if( ixList.size() >= MAXRANK)
      throw GDLException("Maximum of "+MAXRANK_STR+" dimensions allowed.");
    ixList.push_back( pb);
  }
};

#endif
