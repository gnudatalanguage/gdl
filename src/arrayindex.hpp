/***************************************************************************
                          arrayindex.hpp  -  array access descriptor
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
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

#include <vector>

#include "datatypes.hpp"

class ArrayIndexT
{
public:
  enum IxType {
    INDEXED=0, // explicit indices
    ALL,       // *
    ONE,       // scalar
    ORANGE,    // open range s:*
    RANGE      // s:e
  };

private:
  IxType      t;
  SizeT    s,e;
  DLongGDL* ix;
  
public:

  ~ArrayIndexT() {} // never delete ix -> done by ArrayIndexListT

  // [[ix]] (type 0)
  ArrayIndexT( DLongGDL* ix_): t(INDEXED), s(0), e(0), ix( ix_) 
  {} 

  // [*] (type 1)
  ArrayIndexT(): t(ALL), s(0), e(0), ix(NULL) 
  {} 

  // [s] (type 2) || [s:*] (type 3)  || [s:e] (type 4) 
  ArrayIndexT( IxType t_, SizeT s_, SizeT e_=0): t(t_), s(s_), e(e_), ix(NULL) 
  {}
  
  // get nth index
  SizeT GetIx( SizeT nth)
  {
    if( t == 0) return (*ix)[nth]; // from array
    //if( t == 2) return s;        // scalar (nth == 0)
    return nth + s;
  }

  // number of iterations
  // also checks/adjusts range 
  SizeT NIter( SizeT varDim, bool strictArrSubs) 
  {
    if( t == INDEXED) 
      {
	SizeT nElem=ix->N_Elements();

	if( strictArrSubs)
	  { // strictArrSubs -> exception if out of bounds
	    for( SizeT i=0; i < nElem; ++i)
	      if( ((*ix)[i] < 0) || ((*ix)[i] >= static_cast<DLong>(varDim)))
		throw GDLException("Array used to subscript array "
				   "contains out of range subscript.");
	  }
	else
	  {
	    SizeT upper=varDim-1;
	    for( SizeT i=0; i < nElem; ++i)
	      {
		if( (*ix)[i] < 0) (*ix)[i]=0; 
		else if( (*ix)[i] > static_cast<DLong>(upper)) (*ix)[i]=upper;
	      }
	  }
	return nElem; 
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
    // t == RANGE
    if( e >= varDim)
      throw GDLException("Subscript out of range [s:e].");
    return (e - s + 1);
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
    return ix->Dim();
  }
  
  friend class ArrayIndexListT;
};

class ArrayIndexListT
{
private:
  std::vector<ArrayIndexT> ixList;

  bool      strictArrSubs;          // for compile_opt STRICTARRSUBS
  
  enum AccessType {
    NORMAL=0,
    ONEDIM,
    ALLSAME,
    ALLONE  // all ONE
  };

  AccessType accessType;
  SizeT    acRank;               // rank upto which indexing is done
  SizeT    nIterLimit[MAXRANK];  // for each dimension, how many iterations
  SizeT    stride[MAXRANK+1];    // for each dimension, how many iterations
  SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

public:    
  ~ArrayIndexListT()
  {
    for( std::vector<ArrayIndexT>::iterator i=ixList.begin(); 
	 i != ixList.end(); ++i)
      if( i->ix != NULL) delete i->ix;
  }

  // constructor
  ArrayIndexListT( bool strictArrSubs_ = false):
    strictArrSubs( strictArrSubs_),
    accessType(NORMAL),
    acRank(0) 
  {
    ixList.reserve(MAXRANK); 
  }

  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    ArrayIndexT& ixListEnd = ixList[ ixList.size()-1];

    if( !ixListEnd.Scalar( lastIx))
      throw GDLException( "Record number must be a scalar in this context.");

    ixList.pop_back();

    return ixList.empty();
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
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
	nIterLimit[0]=ixList[0].NIter( var->Size(), strictArrSubs); 
	nIx=nIterLimit[0];
	
	accessType = ONEDIM;
      }
    else
      { // normal (multi-dim) access
	accessType = ALLSAME;
	
	nIx=1;
	for( SizeT i=0; i<acRank; ++i)
	  {
	    if( !ixList[i].Indexed()) accessType = NORMAL;
	    
	    nIterLimit[i]=ixList[i].NIter( (i<varRank)?varDim[i]:1, 
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
		    if( !ixList[i].Scalar())
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
	if( ixList[0].Indexed())
	  {
	    return ixList[0].GetDim(); // gets structure of indexing array
	  }
 	else if( ixList[0].Scalar())
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
	return ixList[0].GetDim();
      }
    // accessType == NORMAL -> structure from indices
    return dimension( nIterLimit, acRank);
  }

  SizeT N_Elements()
  {
    return nIx;
  }

  // returns 1-dim index for nTh element
  SizeT GetIx( SizeT nTh)
  {
    if( accessType == ONEDIM)
      {
	return ixList[0].GetIx( nTh);
      }

    if( accessType == ALLSAME)
      {
	SizeT actIx=0;

	for( SizeT i=0; i < acRank; ++i)
	  {
	    actIx += ixList[i].GetIx( nTh) * varStride[i];
	  }
	return actIx;
      }

    // NORMAL or ALLONE
    // loop only over specified indices
    // higher indices of variable are implicitely zero,
    // therefore they are not checked in 'SetRoot'
    SizeT actIx=0;

    for( SizeT i=0; i < acRank; ++i)
      {
	actIx += ixList[i].GetIx( (nTh / stride[i]) % nIterLimit[i]) * 
	  varStride[i];
      }

    return actIx;
  }

  // returns multi-dim index for nTh element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& rank)
  {
    const SizeT nTh = 0;
    if( accessType == ONEDIM)
      {
	rank = 1;
	return dimension( ixList[0].GetIx( nTh));
      }
    
    SizeT actIx[ MAXRANK];
    
    for( SizeT i=0; i < acRank; ++i)
      {
	actIx[ i] = ixList[i].GetIx( nTh);
      }

    rank = acRank;
    return dimension( actIx, acRank);
  }
  
  void push_back( ArrayIndexT& pb)
  {
    if( ixList.size() >= MAXRANK)
      throw GDLException("Maximum of "+MAXRANK_STR+" dimensions allowed.");
    ixList.push_back(pb);
  }
};

#endif
