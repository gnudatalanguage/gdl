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

#include "typedefs.hpp"
#include "gdlexception.hpp"

const SizeT MAXRANK=8;         // arrays are limited to 8 dimensions
const std::string MAXRANK_STR("8");  // for use in strings (error messages)

inline SizeT RankIx( const SizeT rank)
{
  return (rank <= 1)? 0 : rank-1;
}

class dimension
{
  SizeT dim[MAXRANK];         // dimension
  SizeT rank;                 // how many dim are valid  

public:
  // structors
  dimension(): rank(0)
  {}

  // c-i
  dimension(const dimension& dim_)
  { 
    rank = dim_.rank;
    for(unsigned i=0; i<rank; ++i)
      { dim[i]=dim_.dim[i];}
  }

  // initialize from SizeT[ nD] array
  dimension(const SizeT*  d, SizeT nD=MAXRANK)
  {
    rank = nD;
    for( SizeT i=0; i<rank; ++i)
      dim[i]=d[i];
  }

  // simplified constructor for one to three dimensions
  dimension(const SizeT d0, const SizeT d1=0, const SizeT d2=0)
  {
    if( d0 == 0) 
      {
	rank = 0; return;
      }
    dim[0] = d0;
    if( d1 == 0) 
      {
	rank = 1; return;
      }
    dim[1] = d1;
    if( d2 == 0) 
      {
	rank = 2; return;
      }
    dim[2] = d2;
    rank = 3;
  }

  // operators
  // assignment
  dimension& operator=(const dimension& dim_)
  { 
    if( &dim_ == this) return *this; // self assignment
    rank = dim_.rank;
    for(unsigned i=0; i<rank; ++i) dim[i]=dim_.dim[i];
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
  }

  // cat one dim to the right
  void operator<<(const SizeT add)
  { 
    if( rank == MAXRANK)
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    dim[rank++]=add;
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
  }

//   SizeT RemoveFirst()
//   {
//     SizeT res = dim[0];
//     for( SizeT i=0; i<MAXRANK-1; ++i)
//       dim[i]=dim[i+1];
//     dim[MAXRANK-1]=0;
//     return res;
//   }

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
	o << d.N_Elements();
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
  SizeT N_Elements() const
  {
    SizeT res=1;
    for(unsigned i=0; i<rank; ++i) res *= dim[i];
    return res;
  }

  void Set(const SizeT ix, const SizeT d) 
  {
    if( ix >= rank) rank = ix+1;
    dim[ix]=d;
  }

  SizeT Stride(const SizeT i) const
  {
    SizeT ret=1;
    SizeT l = (i<rank)?i:rank;
    for(unsigned m=1; m<=l; ++m)
      ret *= dim[m-1];

    return ret;
  }

  void Stride( SizeT s[], SizeT upto) const
  {
    s[0]=1; // upto must be at least 1
    unsigned m=1;
    if( upto <= rank)
      for(; m<=upto; ++m)
	s[m] = s[m-1] * dim[m-1];
    else
      {
      for(; m<=rank; ++m)
	s[m] = s[m-1] * dim[m-1];
      for(; m<=upto; ++m)
	s[m] = s[m-1];
      }
  }

  // actual rank (0->scalar .. MAXRANK)
  // dim[rank]=0 for rank<MAXRANK
  SizeT Rank() const
  {
    return rank;
  }

  // throw away unused ranks (ie. ranks == 1)
  void Purge()
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
  }

  // multidim index to one dim index
  SizeT LongIndex(const dimension& ix) const
  {
    // SizeT s[MAXRANK+1];    // multiplier for dimension
    // Stride[ c, MAXRANK];
    SizeT s = 1; 

    SizeT res=0;
    unsigned i;
    for( i=0; i<rank; ++i)
      {
	if( ix.dim[i] >= dim[i])
	  throw GDLException("Array index out of range (1)");

	res += ix.dim[i] * s;
	s *= dim[ i];
      }
    for(; i<ix.rank; ++i)
      if( ix.dim[i] > 0)
	throw GDLException("Array index out of range (2)");
    return res;
  }
};

#endif
