/***************************************************************************
                          dimension.hpp  -  GDL array structure info
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

public:
  // structors
  dimension()
  {
    for(unsigned i=0; i<MAXRANK; i++) { dim[i]=0;}
  }

  // c-i
  dimension(const dimension& dim_)
  { 
    for(unsigned i=0; i<MAXRANK; i++)
      { dim[i]=dim_.dim[i];}
  }

  // initialize from SizeT[ nD] array
  dimension(const SizeT*  d, SizeT nD=MAXRANK)
  {
    SizeT i;
    for( i=0; i<nD; i++)
      dim[i]=d[i];
    for(; i<MAXRANK; i++)
      dim[i]=0;
  }

  // simplified constructor for one to three dimensions
  dimension(const SizeT d0, const SizeT d1=0, const SizeT d2=0)
  {
    dim[0] = d0;
    dim[1] = d1;
    dim[2] = d2;
    for( SizeT i=3; i<MAXRANK; i++)
      dim[i]=0;
  }

  // operators
  // assignment
  dimension& operator=(const dimension& dim_)
  { 
    if( &dim_ == this) return *this; // self assignment
    for(unsigned i=0; i<MAXRANK; i++) dim[i]=dim_.dim[i];
    return *this;
  }

  // cat two dimensions
  // scalars are skipped
  // cat add to right
  void operator<<(const dimension& add)
  { 
    SizeT thisRank = Rank();
    SizeT addRank  = add.Rank();
     
    SizeT newRank= thisRank+addRank;
    if( newRank > MAXRANK)
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    for( SizeT i=thisRank; i<newRank; i++)
      dim[i]=add.dim[i-thisRank];
  }

  // cat one dim to the right
  void operator<<(const SizeT add)
  { 
    SizeT thisRank = Rank();
     
    SizeT newRank= thisRank+1;
    if( newRank > MAXRANK)
      throw GDLException("Only "+MAXRANK_STR+" dimensions allowed.");

    dim[thisRank]=add;
  }

  // cat one dim to the left
  void operator>>(const SizeT add)
  { 
    SizeT thisRank = Rank();
     
    SizeT newRank= thisRank+1;
    if( newRank > MAXRANK)
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
    int thisRank = Rank();
    int addRank  = add.Rank();
     
    int newRank= thisRank+addRank;
    if( newRank > static_cast<int>(MAXRANK))
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
    SizeT res = dim[ix];
    for( SizeT i=ix; i<MAXRANK-1; ++i)
      dim[i]=dim[i+1];
    dim[MAXRANK-1]=0;
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
    SizeT r=d.Rank();
    if( r == 0)
      {
	o << "scalar ";
	o << d.N_Elements();
      }
    else
      {
	o << " Array[";
	for( SizeT i=0; i<(r-1); i++)
	  o << d.dim[i] << ",";
	o << d.dim[r-1] << "]";
      }
    return o;
  }

  friend bool
    operator==(const dimension& left,
               const dimension& right)
  {
    for( SizeT i=0; i<MAXRANK; i++)
      if( left.dim[i] != right.dim[i]) return false;
    return true;
  }

  friend bool
    operator!=(const dimension& left,
               const dimension& right)
  {
    for( SizeT i=0; i<MAXRANK; i++)
      if( left.dim[i] != right.dim[i]) return true;
    return false;
  }

  // one dim array access (unchecked)
  SizeT operator[] (const SizeT d1) const
  {
    return dim[d1];
  }

  // members
  // number of elements
  SizeT N_Elements() const
  {
    SizeT res=1;
    for(unsigned i=0; i<MAXRANK && dim[i] != 0; i++) res *= dim[i];
    return res;
  }

  void Set(const SizeT ix, const SizeT d) 
  {
    dim[ix]=d;
  }

  SizeT Stride(const SizeT i) const
  {
    SizeT ret=1;
    for(unsigned m=1; m<=i && (dim[m-1] != 0); m++)
      ret *= dim[m-1];

    return ret;
  }

  void Stride( SizeT s[], SizeT upto) const
  {
    s[0]=1; // upto must be at least 1
    for(unsigned m=1; m<=upto; m++)
      s[m] = s[m-1] * (dim[m-1]?dim[m-1]:1);
  }

  // actual rank (0->scalar .. MAXRANK)
  // dim[rank]=0 for rank<MAXRANK
  SizeT Rank() const
  {
    SizeT i;
    for(i=0; i<MAXRANK && dim[i]; i++); 
    return i;
  }

  // throw away unused ranks (ie. ranks == 1)
  void Purge()
  {
    for(SizeT i=MAXRANK-1; i>0 && dim[i] <= 1; i--) dim[i]=0;
  }

  // set the rank to r (pads 1s) if it is smaller than r
  void MakeRank(SizeT r)
  {
    SizeT rNow=Rank();
    if( rNow >= r) return;
    if( r > MAXRANK)
      throw GDLException("Maximum "+MAXRANK_STR+" dimensions are allowed.");
    for( SizeT i=rNow; i<r; i++) dim[i]=1;
  }

  // multidim index to one dim index
  SizeT LongIndex(const dimension& ix) const
  {
    SizeT s[MAXRANK+1];    // multiplier for dimension
    Stride( s, MAXRANK);

    SizeT res=0;
    unsigned i;
    for( i=0; dim[i] && i<MAXRANK; ++i)
      {
      	if( ix.dim[i] >= dim[i])
      	  throw GDLException("Array index out of range (1)");
      	res += ix.dim[i] * s[i];
      }
    for(; i<MAXRANK; ++i)
      if( ix.dim[i] > 0)
	throw GDLException("Array index out of range (2)");

    return res;
  }
};

#endif
