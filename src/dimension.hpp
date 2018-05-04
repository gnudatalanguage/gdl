/***************************************************************************
                          dimension.hpp  -  GDL array structure info
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

#ifndef DIMENSION_HPP_
#define DIMENSION_HPP_

#include <cassert>

#include "typedefs.hpp"
#include "gdlexception.hpp"

// const SizeT MAXRANK=8;         // arrays are limited to 8 dimensions
// const std::string MAXRANK_STR("8");  // for use in strings (error messages)

typedef char RankT;

inline SizeT RankIx( const SizeT rank)
{
  return (rank <= 1)? 0 : rank-1;
}

class dimension
{
  SizeT dim[MAXRANK];         // dimension
  mutable SizeT stride[MAXRANK+1];      // stride

  char rank;                 // how many dim are valid

public:
  // structors
  dimension(): rank(0)
  {
    stride[0] = 0; // mark as not set
  }

  // c-i
  dimension(const dimension& dim_)
  { 
    rank = dim_.rank;
    for(char i=0; i<rank; ++i)
      { dim[i]=dim_.dim[i];}
    stride[0] = 0; // not set
  }

  // initialize from SizeT[ nD] array
  dimension(const SizeT*  d, SizeT nD=MAXRANK)
  {
    rank = nD;
    for( SizeT i=0; i<rank; ++i)
      dim[i]=d[i];
    stride[0] = 0; // not set
  }
  // initialize from DLong[ nD] array
  dimension(const DLong*  d, SizeT nD=MAXRANK)
  {
    rank = nD;
    for( SizeT i=0; i<rank; ++i)
      dim[i]= d[i];
    stride[0] = 0; // not set
  }

  // simplified constructors for one to three dimensions
  dimension(const SizeT d0)
  {
    assert( d0 != 0);
    //     if( d0 == 0) 
    //       {
    // 	rank = 0; return;
    //       }
    dim[0] = d0;
    rank = 1;
    stride[0] = 0; // not set
  }
  dimension(const SizeT d0, const SizeT d1)
  {
    assert( d0 != 0);
    dim[0] = d0;
    assert( d1 != 0);
    dim[1] = d1;
    rank = 2;
    stride[0] = 0; // not set
  }
  //   dimension(const SizeT d0, const SizeT d1, const SizeT d2)
  //   {
  //     assert( d0 != 0);
  //     dim[0] = d0;
  //     assert( d1 != 0);
  //     dim[1] = d1;
  //     assert( d2 != 0);
  //     dim[2] = d2;
  //     rank = 3;
  // //     stride[0] = 1; // set
  // //     stride[1] = d0; // set
  // //     stride[1] = d0 * d1; // set
  //   }

  // operators
  // assignment
  dimension& operator=(const dimension& dim_)
  { 
    if( &dim_ == this) return *this; // self assignment
    rank = dim_.rank;
    for(char i=0; i<rank; ++i) 
      dim[i]=dim_.dim[i];
    stride[0] = 0; // not set
    return *this;
  }

  // cat two dimensions
  // scalars are skipped
  // cat add to right
  void operator<<(const dimension& add)
  { 
    SizeT thisRank = rank;
     
    rank += add.rank;
    if( rank > MAXRANK)
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    for( SizeT i=thisRank; i<rank; ++i)
      dim[i]=add.dim[i-thisRank];
    
    stride[0] = 0; // not set
  }

  // cat one dim to the right
  void operator<<(const SizeT add)
  { 
    if( rank == MAXRANK)
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    dim[rank++]=add;
    
    stride[0] = 0; // not set
  }

  // cat one dim to the left
  void operator>>(const SizeT add)
  { 
    SizeT thisRank = rank++;
     
    if( rank > MAXRANK)
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    for( int i=thisRank-1; i>=0; i--)
      {
	dim[i+1]=dim[i];
      }
    
    dim[0]=add;
    
    stride[0] = 0; // not set
  }

  // cat add to left
  void operator>>(const dimension& add)
  { 
    int thisRank = rank;
    int addRank  = add.rank;
     
    rank += addRank;
    if( rank > static_cast<int>(MAXRANK))
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    // shift dim by addRank
    for( int i=thisRank-1; i>=0; i--)
      {
	dim[i+addRank]=dim[i];
      }

    // insert add on the left
    for( int i=0; i<addRank; ++i)
      dim[i]=add.dim[i];
    
    stride[0] = 0; // not set
  }

  // cat add to left
  void AddLeft(const dimension& add, int nRank = 0)
  { 
    int thisRank = rank;
    int addRank = add.rank;
    if( nRank != 0 && nRank < addRank)
    {
	addRank = nRank;
    }
     
    rank += addRank;
    if( rank > static_cast<int>(MAXRANK))
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    // shift dim by addRank
    for( int i=thisRank-1; i>=0; i--)
      {
	dim[i+addRank]=dim[i];
      }

    // insert add on the left
    for( int i=0; i<addRank; ++i)
      dim[i]=add.dim[i];
    
    stride[0] = 0; // not set
  }

  // remove dimesion ix (used by total function)
  SizeT Remove( SizeT ix)
  {
    if( rank == 0) 
      {
	assert( ix == 0);
	return 0;
      }
    SizeT res = dim[ix];
    rank--;
    for( SizeT i=ix; i<rank; ++i)
      dim[i]=dim[i+1];
    
    stride[0] = 0; // not set
    
    return res;
  }

  const std::string ToString() const
  {
    std::ostringstream os;
    os << (*this);
    return os.str();
  }

  friend std::ostream& operator<<(std::ostream& o, const dimension& d)
  {
    SizeT r=d.rank;
    if( r == 0)
      {
	o << "scalar ";
	o << d.NDimElementsConst();
      }
    else
      {
	o << "Array[";
	for( SizeT i=0; i<(r-1); ++i)
	  o << d.dim[i] << ", ";
	o << d.dim[r-1] << "]";
      }
    return o;
  }

  friend bool
  operator==(const dimension& left,
	     const dimension& right)
  {
    if( left.rank != right.rank) return false;
    for( SizeT i=0; i<left.rank; ++i)
      if( left.dim[i] != right.dim[i]) return false;
    return true;
  }

  friend bool
  operator!=(const dimension& left,
	     const dimension& right)
  {
    if( left.rank != right.rank) return true;
    for( SizeT i=0; i<left.rank; ++i)
      if( left.dim[i] != right.dim[i]) return true;
    return false;
  }

  // one dim array access (unchecked)
  SizeT operator[] (const SizeT d1) const
  {
    if( d1 >= rank) return 0;
    return dim[d1];
  }

  // members

  // number of elements
  SizeT NDimElements() 
  {
    if( stride[0] == 0) InitStride();
    return stride[ rank];
    //     SizeT res=1;
    //     for(unsigned i=0; i<rank; ++i) res *= dim[i];
    //     return res;
  }
  SizeT NDimElementsConst() const
  {
    SizeT res=1;
    for(char i=0; i<rank; ++i) res *= dim[i];
    return res;
  }

  // used by some lib routines, not time critical
  void SetOneDim(const SizeT ix, const SizeT d)
  {
    if( ix >= rank) rank = ix+1;
    dim[ix]=d;

    stride[0] = 0; // not set
  }

  SizeT Stride(const SizeT i) const
  {
    if( stride[0] == 0)
      this->InitStride();
    // 		const_cast<dimension*>(this)->InitStride();
    return stride[ (i<rank)?i:rank];

    // 	SizeT ret=1;
    //     SizeT l = (i<rank)?i:rank;
    //     for(unsigned m=1; m<=l; ++m)
    //       ret *= dim[m-1];
    //     return ret;
  }

  const SizeT* Stride() const
  {
    if( stride[0] == 0)
      this->InitStride();
    return stride;
  }
	
  void Stride( SizeT s[], SizeT upto) const
  {
    assert( upto >= 1);
    if( stride[0] == 0)
      this->InitStride();
    // copy
    for(int m=0; m<=upto; ++m)
      s[m] = stride[m];
    //     s[0]=1; // upto must be at least 1
    //     if( stride[0] == 0) // stride not set yet
    //     {
    // 		unsigned m=1;
    // 		if( upto <= rank)
    // 			for(; m<=upto; ++m)
    // 				s[m] = s[m-1] * dim[m-1];
    // 		else
    // 		{
    // 			for(; m<=rank; ++m)
    // 			s[m] = s[m-1] * dim[m-1];
    // 			for(; m<=upto; ++m)
    // 			s[m] = s[m-1];
    // 		}
    //     }
    //     else // stride already calculated -> copy
    //     {
    // 		unsigned m=1;
    // 		if( upto <= rank)
    // 			for(; m<=upto; ++m)
    // 				s[m] = stride[m];
    // 		else
    // 		{
    // 			for(; m<=rank; ++m)
    // 				s[m] = stride[m];
    // 			for(; m<=upto; ++m)
    // 				s[m] = s[m-1];
    // 		}
    //     }
  }
  
  // we must do a full stride calculation here because
  // variables might be indexed with more dimensions they actually have
  // (which indices then must all be zero)
  void InitStride() const
  {
    if( rank == 0)
      {
	for(int m=0; m<=MAXRANK; ++m)
	  stride[m] = 1;      
      }
    else
      {
	stride[0]=1; 
	stride[1]=dim[0]; 
	int m=1;
	for(; m<rank; ++m)
	  stride[m+1] = stride[m] * dim[m];
	for(; m<MAXRANK; ++m)
	  stride[m+1] = stride[rank];
      }
  }

  // actual rank (0->scalar .. MAXRANK)
  // dim[rank]=0 for rank<MAXRANK
  SizeT Rank() const
  {
    return rank;
  }
  //returns an equivalent rank (leading and ending sizes <=1 removed)
  SizeT EquivalentRank() const
  {
   char eqrank=rank;
   char i=0;
   for (; i<eqrank && dim[i]<=1; ++i);
   for (; eqrank>1 && dim[eqrank-1] <= 1; --eqrank);
   return eqrank-i;
  }
  // throw away unused ranks (ie. ranks == 1)
  inline void Purge()
  {
    for(; rank>1 && dim[rank-1] <= 1; --rank);
  }

  // set the rank to r (pads 1s) if it is smaller than r
  void MakeRank(SizeT r)
  {
    SizeT rNow=rank;
    if( rNow >= r) return;
    if( r > MAXRANK)
      throw GDLException("Maximum "+MAXRANK_STR+" dimensions are allowed.");
    for( SizeT i=rNow; i<r; ++i) dim[i]=1;
    rank = r;
    // as we always InitStride() to MAXRANK+1 stride is still valid
    //     stride[0] = 0;
  }

  void MakeArrayFromScalar()
  {
    assert( rank == 0);
    assert( stride[0] == 0);
    dim[0]=1;
    rank=1;
  }

};

#endif
