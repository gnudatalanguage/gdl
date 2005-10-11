/***************************************************************************
                          datatypes.cpp  -  GDL datatypes
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

#include "includefirst.hpp"

#if defined(USE_PYTHON) || defined(PYTHON_MODULE)
#include <numarray/numarray.h>
#endif

#include <iomanip>

//#include "datatypes.hpp" // included from arrayindex.hpp
#include "dstructgdl.hpp"
#include "arrayindexlistt.hpp"
#include "assocdata.hpp"
#include "io.hpp"
#include "dinterpreter.hpp"
#include "terminfo.hpp"

// needed with gcc-3.3.2
#include <assert.h>

// on OS X isnan is not defined
#if defined(__APPLE__) && !defined(isnan)

#ifdef __cplusplus
extern "C" {
#endif
#define      isnan( x )         ( ( sizeof ( x ) == sizeof(double) ) ?  \
                              __isnand ( x ) :                          \
                                ( sizeof ( x ) == sizeof( float) ) ?    \
                              __isnanf ( x ) :                          \
                              __isnan  ( x ) )
#ifdef __cplusplus
}
#endif
#endif

using namespace std;

// this (ugly) including of other sourcefiles has to be done, because
// on Mac OS X a template instantiation request (see bottom of file)
// can only be done once
#define INCLUDE_BASIC_OP_CPP 1
#include "basic_op.cpp"

#define INCLUDE_DEFAULT_IO_CPP 1
#include "default_io.cpp"

#define INCLUDE_IFMT_CPP 1
#include "ifmt.cpp"

#define INCLUDE_OFMT_CPP 1
#include "ofmt.cpp"

#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

#define INCLUDE_TOPYTHON_CPP 1
#include "topython.cpp"

#define INCLUDE_GDLPYTHON_CPP 1
#include "gdlpython.cpp"

#ifdef PYTHON_MODULE
#define INCLUDE_PYTHONGDL_CPP 1
#include "pythongdl.cpp"
#endif
#endif


// destructor
template<class Sp> Data_<Sp>::~Data_() {};

// default
template<class Sp> Data_<Sp>::Data_(): Sp(), dd() {}

// scalar
template<class Sp> Data_<Sp>::Data_(const Ty& d_): Sp(), dd(d_,1)
{}

// new array, zero fields
template<class Sp> Data_<Sp>::Data_(const dimension& dim_): 
  Sp( dim_), dd( Sp::zero, this->dim.N_Elements())
{
  this->dim.Purge();
}

// new one-dim array fromTy*
template<class Sp> Data_<Sp>::Data_(const Ty* p, const SizeT nEl): 
  Sp( dimension( nEl)), dd( p, nEl)
{}

// c-i 
// template<class Sp> Data_<Sp>::Data_(const Data_& d_): 
// Sp(d_.dim), dd(d_.dd) {}

template<class Sp> Data_<Sp>::Data_(const dimension& dim_,
				    BaseGDL::InitType iT): 
  Sp( dim_), dd( this->dim.N_Elements())
{
  this->dim.Purge();

  if( iT == BaseGDL::INDGEN)
    {
      SizeT sz=dd.size();
      Ty val=Sp::zero;
      for( SizeT i=0; i<sz; i++)
	{
	  dd[i]=val;
	  val += 1; // no increment operator for floats
	}
    }
}

template<class Sp>
Data_<Sp>* Data_<Sp>::Log()              
{ 
  assert( 0);
  return NULL;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::Log()              
{ 
  return new Data_(this->dim, log(dd));
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::Log()              
{ 
  return new Data_(this->dim, log(dd));
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::Log()              
{ 
  return new Data_(this->dim, log(dd));
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Log()              
{ 
  return new Data_(this->dim, log(dd));
}

template<class Sp>
void Data_<Sp>::LogThis()              
{ 
  assert( 0);
}
template<>
void Data_<SpDFloat>::LogThis()              
{ 
  dd = log(dd);
}
template<>
void Data_<SpDDouble>::LogThis()              
{ 
  dd = log(dd);
}
template<>
void Data_<SpDComplex>::LogThis()              
{ 
  dd = log(dd);
}
template<>
void Data_<SpDComplexDbl>::LogThis()              
{ 
  dd = log(dd);
}

// template<class Sp>
// BaseGDL* Data_<Sp>::Abs() const
// {
//   return new Data_( this->dim, dd.abs());
// }

template<class Sp>
bool Data_<Sp>::Greater(SizeT i1, SizeT i2) const
{ return (dd[i1] > dd[i2]);}

template<>
bool Data_<SpDComplex>::Greater(SizeT i1, SizeT i2) const
{ return (abs(dd[i1]) > abs(dd[i2]));}
template<>
bool Data_<SpDComplexDbl>::Greater(SizeT i1, SizeT i2) const
{ return (abs(dd[i1]) > abs(dd[i2]));}


template<class Sp>
bool Data_<Sp>::Equal(SizeT i1, SizeT i2) const
{ return (dd[i1] == dd[i2]);}


template<class Sp>
BaseGDL* Data_<Sp>::CShift( DLong d)
{
  Data_* sh = new Data_( this->dim, BaseGDL::NOZERO); 

  if( d >= 0)
    d %= dd.size();
  else
    d = -(-d % dd.size());

  sh->dd = dd.cshift( -d);
  return sh;
}

template<class Sp>
BaseGDL* Data_<Sp>::CShift( DLong s[ MAXRANK])
{
  Data_* sh = new Data_( this->dim, BaseGDL::NOZERO); 

  SizeT nDim = this->Rank();
  SizeT nEl = N_Elements();

  SizeT  stride[ MAXRANK + 1];
  this->dim.Stride( stride, nDim);

  long  srcIx[ MAXRANK+1];
  long  dstIx[ MAXRANK+1];
  SizeT dim_stride[ MAXRANK];
  SizeT this_dim[ MAXRANK];
  for( SizeT aSp=0; aSp<nDim; ++aSp) 
    {
      this_dim[ aSp] = this->dim[ aSp];

      srcIx[ aSp] = 0;
      if( s[ aSp] >= 0)
	dstIx[ aSp] = s[ aSp] % this_dim[ aSp];
      else
	dstIx[ aSp] = -(-s[aSp] % this_dim[ aSp]);
      if( dstIx[ aSp] < 0) dstIx[ aSp] += this_dim[ aSp];

      dim_stride[ aSp] = this_dim[ aSp] * stride[ aSp];
    }
  srcIx[ nDim] = dstIx[ nDim] = 0;

  SizeT dstLonIx = dstIx[ 0];
  for( SizeT rSp=1; rSp<nDim; ++rSp)
    dstLonIx += dstIx[ rSp] * stride[ rSp];

  Ty* ddP = &dd[0];
  Ty* shP = &sh->dd[0];
  
  for( SizeT a=0; a<nEl; ++srcIx[0],++dstIx[0])
    {
      for( SizeT aSp=0; aSp<nDim;)
	{
	  if( dstIx[ aSp] >= this_dim[ aSp]) 
	    {
	      // dstIx[ aSp] -= dim[ aSp];
	      dstIx[ aSp] = 0;
	      dstLonIx -= dim_stride[ aSp];
	    }
	  if( srcIx[ aSp] < this_dim[ aSp]) break;

	  srcIx[ aSp] = 0;
	  ++srcIx[ ++aSp];
	  ++dstIx[ aSp];
	  dstLonIx += stride[ aSp];
	}

      shP[ dstLonIx++] = ddP[ a++];
    }
  
  return sh;
}



// assumes *perm is already checked according to uniqness and length
// dim[i]_out = dim[perm[i]]_in
// helper function for Transpose()
DUInt* InitPermDefault()
{
  static DUInt res[ MAXRANK];
  for( SizeT i=MAXRANK-1, ii=0; ii<MAXRANK; ++ii, --i)
    res[ ii] = i;
  return res;
}
template<class Sp> 
BaseGDL* Data_<Sp>::Transpose( DUInt* perm)
{
  SizeT rank = this->Rank();

  if( rank == 1) // special case: vector
    {
      if( perm != NULL) // must be [0]
	{
	  return Dup();
	}
      else
	{
	  Data_* res = Dup();
	  res->dim >> 1;
	  return res;
	}
    }

  // 2 - MAXRANK
  static DUInt* permDefault = InitPermDefault(); 
  if( perm == NULL)
      perm = &permDefault[ MAXRANK - rank];

  dimension newDim;
  SizeT this_dim[ MAXRANK]; // permutated!
  for( SizeT d=0; d<rank; ++d)
    {
      this_dim[ d] = this->dim[ perm[ d]];
      newDim.Set( d, this_dim[ d]);
    }

  Data_* res = new Data_( newDim, BaseGDL::NOZERO);

  // src stride
  SizeT srcStride[ MAXRANK];
  this->dim.Stride( srcStride, rank);

  // src multi dim
  SizeT srcDim[MAXRANK];
  for( SizeT i=0; i<rank; ++i) srcDim[i]=0;

  SizeT nElem = dd.size();
  for( SizeT e=0; e<nElem; ++e)
    {
      // multi src dim to one dim index
      SizeT ix = 0;
      for( SizeT i=0; i < rank; ++i)
	ix += srcDim[i] * srcStride[i];
      
      res->dd[ e] = dd[ ix];

      // update dest multi dim
      for( SizeT i=0; i < rank; ++i)
	{
	  DUInt pi = perm[i];
	  srcDim[pi]++;
	  if( srcDim[pi] < this_dim[i]) break;
	  srcDim[pi]=0;
	}
    }
  
  return res;
}

// rank must be 1 or 2 (already checked)
template<class Sp> 
BaseGDL* Data_<Sp>::Rotate( DLong dir)
{
  dir = (dir%8+8)%8; // bring into 0..7 range

  if( dir == 0) return Dup();
  if( dir == 2)
    {
      Data_* res = new Data_( this->dim, BaseGDL::NOZERO);
      SizeT nEl = N_Elements();
      
      for( SizeT i=0; i<nEl; ++i)
	  res->dd[i] = dd[ nEl-1-i];
      return res;
    }

  if( this->Rank() == 1)
    {
      if( dir == 7) return Dup();
      
      if( dir == 1 || dir == 4)
	{
	  return new Data_( dimension( 1, N_Elements()), dd);
	}
      if( dir == 5) // || dir == 2
	{
	  Data_* res = new Data_( this->dim, BaseGDL::NOZERO);
	  SizeT nEl = N_Elements();
	  for( SizeT i=0; i<nEl; ++i)
	    res->dd[ i] = dd[ nEl-1-i];
	  return res;
	}
      // 3 || 6
      Data_* res = new Data_( dimension( 1, N_Elements()), BaseGDL::NOZERO);
      SizeT nEl = N_Elements();
      for( SizeT i=0; i<nEl; ++i)
	res->dd[ i] = dd[ nEl-1-i];
      return res;
    }

  // rank == 2, dir == 0 and dir == 2 already handled
  bool keepDim = (dir == 5) || (dir == 7);

  Data_* res;
  if( keepDim)
    {
      res = new Data_( this->dim, BaseGDL::NOZERO);
    }
  else
    {
      res = new Data_( dimension( this->dim[1], this->dim[0]), BaseGDL::NOZERO);
    }

  bool flipX = dir == 3 || dir == 5 || dir == 6;
  bool flipY = dir == 1 || dir == 6 || dir == 7;

  SizeT xEl = this->dim[0];
  SizeT yEl = this->dim[1];
  SizeT i = 0;
  for( SizeT y=0; y<yEl; ++y)
    {
      SizeT yR = flipY?yEl-1-y:y;
      for( SizeT x=0; x<xEl; ++x)
	{
	  SizeT xR = flipX?xEl-1-x:x;
	  
	  SizeT ix = keepDim? yR * xEl + xR: xR * yEl + yR;
	  
	  res->dd[ix] = dd[ i++];
	}
    }
  return res;
}

template<class Sp> 
typename Data_<Sp>::Ty Data_<Sp>::Sum() const 
{ return dd.sum();}

// template<class Sp> 
// typename Data_<Sp>::DataT& Data_<Sp>:: Resize( SizeT n)
// {
//   if( n > dd.size())
//     throw GDLException("Internal error: Data_::Resize(...) tried to grow.");
//   if( dd.size() != n) 
//     {
//       DataT rsArr( n);
//       std::copy( &dd[0], &dd[n], &rsArr[0]);
//       dd.resize( n); // discards data
//       std::copy( &rsArr[0], &rsArr[n], &dd[0]);
//     }
//   return dd;
// }

// template<class Sp> 
// typename Data_<Sp>::Ty& Data_<Sp>::operator[] (const SizeT d1) 
// { return dd[d1];}

template<class Sp> 
//typename Data_<Sp>::Data_& Data_<Sp>::operator=(const Data_& right)
Data_<Sp>& Data_<Sp>::operator=(const Data_& right)
{
  if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  dd = right.dd;
  return *this;
}

template< class Sp>
bool Data_<Sp>::EqType( const BaseGDL* r) const 
{ return (Sp::t == r->Type());}

template< class Sp>
void* Data_<Sp>::DataAddr( SizeT elem)
{ return &dd[elem];}

template< class Sp>
const SizeT Data_<Sp>::N_Elements() const 
{ return dd.size();}
template< class Sp>
const SizeT Data_<Sp>::Size() const 
{ return dd.size();}
template< class Sp>
const SizeT Data_<Sp>::Sizeof() const 
{ return sizeof(Ty);}

template< class Sp>
void Data_<Sp>::Clear() 
{ dd = Sp::zero;
  //SizeT nEl = dd.size(); for( SizeT i = 0; i<nEl; ++i) dd[ i] = Sp::zero;
}

// template< class Sp>
// Data_<Sp>* Data_<Sp>::Dup() 
// { return new Data_(*this);}

template< class Sp>
Data_<Sp>* Data_<Sp>::New( const dimension& dim_, BaseGDL::InitType noZero)
{
  if( noZero == BaseGDL::NOZERO) return new Data_(dim_, BaseGDL::NOZERO);
  if( noZero == BaseGDL::INIT)
    {
      Data_* res =  new Data_(dim_, BaseGDL::NOZERO);
      SizeT nEl = res->dd.size();
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = dd[ 0]; // set all to scalar
      return res;
    }
  return new Data_(dim_);
}

// template< class Sp>
// bool Data_<Sp>::Scalar() const
// {
//   return (dd.size() == 1);
// }

// template< class Sp>
// bool Data_<Sp>::Scalar(Ty& s) const
// {
//   if( dd.size() != 1) return false;
//   s=dd[0];
//   return true;
// }

// string, ptr, obj (cannot be INDGEN, 
// need not to be zeroed if all intialized later)
// struct (as a separate class) as well
template<> Data_<SpDString>::Data_(const dimension& dim_,  
				   BaseGDL::InitType iT): 
  SpDString(dim_), dd(dim.N_Elements())
{
  dim.Purge();
  
  if( iT == BaseGDL::INDGEN)
    throw GDLException("DStringGDL(dim,InitType=INDGEN) called.");
}
template<> Data_<SpDPtr>::Data_(const dimension& dim_,  
				BaseGDL::InitType iT): 
  SpDPtr(dim_), dd(dim.N_Elements())
{
  dim.Purge();
  
  if( iT == BaseGDL::INDGEN)
    throw GDLException("DPtrGDL(dim,InitType=INDGEN) called.");
}
template<> Data_<SpDObj>::Data_(const dimension& dim_,  
				BaseGDL::InitType iT): 
  SpDObj(dim_), dd(dim.N_Elements())
{
  dim.Purge();

  if( iT == BaseGDL::INDGEN)
    throw GDLException("DObjGDL(dim,InitType=INDGEN) called.");
}

template<class Sp>
const SizeT Data_<Sp>::NBytes() const 
{ return (dd.size() * sizeof(Ty));}
// string, ptr, obj cannot calculate their bytes
// only used by assoc function
template<> const SizeT Data_<SpDString>::NBytes() const
{
  throw GDLException("string data");
}
template<> const SizeT Data_<SpDObj>::NBytes() const
{
  throw GDLException("object references");
}
template<> const SizeT Data_<SpDPtr>::NBytes() const
{
  throw GDLException("pointers");
}

template<class Sp>
const SizeT Data_<Sp>::ToTransfer() const
{
  return dd.size();
}
// complex has 2 elements to transfer
template<> const SizeT Data_<SpDComplex>::ToTransfer() const
{
  return N_Elements() * 2;
}
template<> const SizeT Data_<SpDComplexDbl>::ToTransfer() const
{
  return N_Elements() * 2;
}

// note that min and max are not defined in BaseGDL
template<class Sp> 
typename Data_<Sp>::Ty Data_<Sp>::min() const
{ return dd.min();}
template<class Sp> 
typename Data_<Sp>::Ty Data_<Sp>::max() const
{ return dd.max();}
template<>
Data_<SpDComplex>::Ty Data_<SpDComplex>::min() const
{
  throw GDLException("COMPLEX expression not allowed in this context.");
}
template<>
Data_<SpDComplex>::Ty Data_<SpDComplex>::max() const
{
  throw GDLException("COMPLEX expression not allowed in this context.");
}
template<>
Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::min() const
{
  throw GDLException("COMPLEXDBL expression not allowed in this context.");
}
template<>
Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::max() const
{
  throw GDLException("COMPLEXDBL expression not allowed in this context.");
}


// Scalar2index
// used by the interpreter
// -2  < 0 array
// -1  < 0 scalar
// 0   empty or array
// 1   scalar
// 2   one-element array
template<class Sp> 
int Data_<Sp>::Scalar2index( SizeT& st) const
{
  if( dd.size() != 1) return 0;

  // the next statement gives a warning for unsigned integer types:
  // "comparison is always false due to limited range of data type"
  // This is because the same template is used here for signed and 
  // unsigned data. A template specialization for the unsigned integer
  // types would result in three identical specializations, which is bad
  // for maintainability. And as any modern C++ compiler will optimize
  // away the superflous (for unsigned data) statement anyway, it is 
  // better to keep the code this way here.
  if( dd[0] < 0)
    if( this->dim.Rank() != 0) 
      return -2;
    else
      return -1;

  st= static_cast<SizeT>(dd[0]);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}

template<> 
int Data_<SpDComplex>::Scalar2index( SizeT& st) const
{
  if( dd.size() != 1) return 0;
  float r=real(dd[0]);
  if( r < 0.0) return -1;
  st= static_cast<SizeT>(r);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}
template<> 
int Data_<SpDComplexDbl>::Scalar2index( SizeT& st) const
{
  if( dd.size() != 1) return 0;
  double r=real(dd[0]);
  if( r < 0.0) return -1;
  st= static_cast<SizeT>(r);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}
template<> 
int Data_<SpDString>::Scalar2index( SizeT& st) const
{
  if( dd.size() != 1) return 0;

  SizeT tSize=dd[0].size();

  if( tSize == 0) 
    {
      st=0;
    }
  else 
    {
      long int number = Str2L( dd[0].c_str());
      if( number < 0) return -1; 
      st=number;
    }
  if( dim.Rank() != 0) return 2;
  return 1;
}

int DStructGDL::Scalar2index( SizeT& st) const
{
  throw GDLException("STRUCT expression not allowed in this context.");
  return 0; // get rid of warning
}

template<> 
int Data_<SpDPtr>::Scalar2index( SizeT& st) const
{
  throw GDLException("PTR expression not allowed in this context.");
  return 0; // get rid of warning
}

template<> 
int Data_<SpDObj>::Scalar2index( SizeT& st) const
{
  throw GDLException("Object expression not allowed in this context.");
  return 0; // get rid of warning
}

// for FOR loop *indices*
template<class Sp> 
SizeT Data_<Sp>::LoopIndex() const
{
  //  if( dd.size() != 1) return 0;

  // the next statement gives a warning for unsigned integer types:
  // "comparison is always false due to limited range of data type"
  // This is because the same template is used here for signed and 
  // unsigned data. A template specialization for the unsigned integer
  // types would result in three identical specializations, which is bad
  // for maintainability. And as any modern C++ compiler will optimize
  // away the superflous (for unsigned data) statement anyway, it is 
  // better to keep the code this way here.
  if( dd[0] < 0)
    throw GDLException( "Loop index variable <0.");

  return static_cast<SizeT>(dd[0]);
}
template<> 
SizeT Data_<SpDFloat>::LoopIndex() const
{
  if( dd[0] < 0.0f)
    if( dd[0] <= 1.0f)
      throw GDLException( "Loop index variable <0.");
    else
      return 0;

  return static_cast<SizeT>(dd[0]);
}
template<> 
SizeT Data_<SpDDouble>::LoopIndex() const
{
  if( dd[0] < 0.0)
    if( dd[0] <= 1.0)
      throw GDLException( "Loop index variable <0.");
    else
      return 0;

  return static_cast<SizeT>(dd[0]);
}
template<> 
SizeT Data_<SpDComplex>::LoopIndex() const
{
  throw GDLException( "Complex expression not allowed as FOR loop index.");
  return 0;
}
template<> 
SizeT Data_<SpDComplexDbl>::LoopIndex() const
{
  throw GDLException( "Complex expression not allowed as FOR loop index.");
  return 0;
}
template<> 
SizeT Data_<SpDString>::LoopIndex() const
{
  throw GDLException( "String expression not allowed as FOR loop index.");
  return 0;
}
// SizeT DStructGDL::LoopIndex() const
// {
//   throw GDLException("STRUCT expression not allowed in this context.");
//   return 0; // get rid of warning
// }

template<> 
SizeT Data_<SpDPtr>::LoopIndex() const
{
  throw GDLException("PTR expression not allowed in this context.");
  return 0; // get rid of warning
}

template<> 
SizeT Data_<SpDObj>::LoopIndex() const
{
  throw GDLException("Object expression not allowed in this context.");
  return 0; // get rid of warning
}


// True
template<class Sp>
bool Data_<Sp>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s % 2);
}

template<>
bool Data_<SpDFloat>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != 0.0f);
}

template<>
bool Data_<SpDDouble>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != 0.0);
}

template<>
bool Data_<SpDString>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != "");
}

template<>
bool Data_<SpDComplex>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (real(s) != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (real(s) != 0.0);
}

template<>
bool Data_<SpDPtr>::True()
{
  throw GDLException("Ptr expression not allowed in this context.");
}

template<>
bool Data_<SpDObj>::True()
{
  throw GDLException("Object expression not allowed in this context.");
}

// False
template<class Sp>
bool Data_<Sp>::False()
{
  return !True();
}

// Sgn
template<class Sp>
int Data_<Sp>::Sgn() // -1,0,1
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  if( s > 0) return 1;
  if( s == 0) return 0;
  return -1;
} 

template<>
int Data_<SpDString>::Sgn() // -1,0,1
{
  throw GDLException("String expression not allowed in this context.");
  return 0;
} 
template<>
int Data_<SpDComplex>::Sgn() // -1,0,1
{
  throw GDLException("Complex expression not allowed in this context.");
  return 0;
} 
template<>
int Data_<SpDComplexDbl>::Sgn() // -1,0,1
{
  throw GDLException("Complex expression not allowed in this context.");
  return 0;
} 

int DStructGDL::Sgn() // -1,0,1
{
  throw GDLException("Struct expression not allowed in this context.");
  return 0;
} 

template<>
int Data_<SpDPtr>::Sgn() // -1,0,1
{
  throw GDLException("Ptr expression not allowed in this context.");
  return 0;
} 

template<>
int Data_<SpDObj>::Sgn() // -1,0,1
{
  throw GDLException("Object expression not allowed in this context.");
  return 0;
} 

// Equal (deletes r)
template<class Sp>
bool Data_<Sp>::Equal( BaseGDL* r)
{
  if( !r->Scalar())
    {
    delete r;
    throw GDLException("Expression must be a scalar in this context.");
    }
  Data_* rr=static_cast<Data_*>(r->Convert2( this->t));
  bool ret= (dd[0] == rr->dd[0]);
  delete rr;
  return ret;
}

bool DStructGDL::Equal( BaseGDL* r)
{
  delete r;
  throw GDLException("Struct expression not allowed in this context.");
  return false;
}

// For array_equal r must be of same type
template<class Sp>
bool Data_<Sp>::ArrayEqual( BaseGDL* rIn)
{
  Data_<Sp>* r = static_cast< Data_<Sp>*>( rIn);
  SizeT nEl = N_Elements();
  SizeT rEl = r->N_Elements();
  if( rEl == 1)
    {
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i] != (*r)[0]) return false;
    return true;
    }
  if( nEl == 1)
    {
    for( SizeT i=0; i<rEl; ++i)
      if( dd[0] != (*r)[i]) return false;
    return true;
    }
  if( nEl != rEl) return false;
  for( SizeT i=0; i<nEl; ++i)
    if( dd[i] != (*r)[i]) return false;
  return true;
}

bool DStructGDL::ArrayEqual( BaseGDL* r)
{
  throw GDLException("Struct expression not allowed in this context.");
  return false;
}

// for statement compliance (int types , float types scalar only)
// (convert strings to floats here (not for first argument)
template<class Sp>
void Data_<Sp>::ForCheck( BaseGDL** lEnd, BaseGDL** lStep)
{
  // all scalars?
  if( !Scalar())
    throw GDLException("Loop INIT must be a scalar in this context.");

  if( !(*lEnd)->Scalar())
    throw GDLException("Loop LIMIT must be a scalar in this context.");

  if( lStep != NULL && !(*lStep)->Scalar())
    throw GDLException("Loop INCREMENT must be a scalar in this context.");
  
  // only proper types?
  if( this->t== UNDEF)
    throw GDLException("Expression is undefined.");
  if( this->t== COMPLEX || this->t == COMPLEXDBL)
    throw GDLException("Complex expression not allowed in this context.");
  if( this->t== PTR)
    throw GDLException("Pointer expression not allowed in this context.");
  if( this->t== OBJECT)
    throw GDLException("Object expression not allowed in this context.");
  if( this->t== STRING)
    throw GDLException("String expression not allowed in this context.");

  // check here if loop limit is COMPLEX, but *only* if loop init is INT

  *lEnd=(*lEnd)->Convert2( this->t);
  if( lStep != NULL) *lStep=(*lStep)->Convert2( this->t);
}

void DStructGDL::ForCheck( BaseGDL** lEnd, BaseGDL** lStep)
{
  throw GDLException("Struct expression not allowed in this context.");
}

// ForCheck must have been called before
template<class Sp>
bool Data_<Sp>::ForCondUp( BaseGDL* lEndIn)
{
  Data_* lEnd=static_cast<Data_*>(lEndIn);
  return dd[0] <= lEnd->dd[0]; 
}
template<class Sp>
bool Data_<Sp>::ForCondDown( BaseGDL* lEndIn)
{
  Data_* lEnd=static_cast<Data_*>(lEndIn);
  return dd[0] >= lEnd->dd[0]; 
}

// cannnot be called, just to make the compiler shut-up
bool DStructGDL::ForCondUp( BaseGDL*)
{ return false; }
bool DStructGDL::ForCondDown( BaseGDL*)
{ return false; }
template<>
bool Data_<SpDComplex>::ForCondUp( BaseGDL*)
{ return false; }
template<>
bool Data_<SpDComplex>::ForCondDown( BaseGDL*)
{ return false; }
template<>
bool Data_<SpDComplexDbl>::ForCondUp( BaseGDL*)
{ return false; }
template<>
bool Data_<SpDComplexDbl>::ForCondDown( BaseGDL*)
{ return false; }

// ForCheck must have been called before
// general version
template<class Sp>
void Data_<Sp>::ForAdd( BaseGDL* addIn)
{
  if( addIn == NULL)
    {
      dd[0] += 1;
      return;
    }
  Data_* add=static_cast<Data_*>(addIn);
  dd[0] += add->dd[0];
}
// cannnot be called, just to make the compiler shut-up
void DStructGDL::ForAdd( BaseGDL* addIn) {}
// normal (+1) version
template<class Sp>
void Data_<Sp>::ForAdd()
{
  dd[0] += 1;
}
// cannnot be called, just to make the compiler shut-up
void DStructGDL::ForAdd() {}

template<class Sp>
void Data_<Sp>::AssignAtIx( SizeT ix, BaseGDL* srcIn)
{
  if( srcIn->Type() != this->Type())
    {
      Data_* rConv = static_cast<Data_*>(srcIn->Convert2( this->Type(), BaseGDL::COPY));
      auto_ptr<Data_> conv_guard( rConv);
      dd[ix] = rConv->dd[0];
    }
  else
    dd[ix] = static_cast<Data_*>(srcIn)->dd[0];
}

// assigns srcIn to this at ixList, if ixList is NULL does linear copy
// assumes: ixList has this already set as variable
// used by DotAccessDescT::DoAssign
//         GDLInterpreter::l_array_expr
template<class Sp>
void Data_<Sp>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, 
			  SizeT offset)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar) 
    { // src is scalar
      Ty scalar=(*src)[0];
      
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();

	  for( SizeT c=0; c<nCp; ++c)
	    dd[ c]=scalar;
	}
      else
	{
	  SizeT nCp=ixList->N_Elements();
	  
	  AllIxT* allIx = ixList->BuildIx();
	  for( SizeT c=0; c<nCp; ++c)
	    dd[ (*allIx)[ c]]=scalar;
	  //	    dd[ ixList->GetIx( c)]=scalar;
	}
    }
  else
    {
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();
	
	  // if (non-indexed) src is smaller -> just copy its number of elements
	  if( nCp > (srcElem-offset))
	    if( offset == 0)
	      nCp=srcElem;
	    else
	      throw GDLException("Source expression contains not enough elements.");

	  for( SizeT c=0; c<nCp; ++c)
	    dd[ c]=(*src)[c+offset];
	}
      else
	{
 	  // crucial part
	  SizeT nCp=ixList->N_Elements();

	  if( nCp == 1)
	    {
	      InsAt( src, ixList);
	    }
	  else
	    {
	      if( offset == 0)
		{
		  if( srcElem < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxT* allIx = ixList->BuildIx();
		  for( SizeT c=0; c<nCp; ++c)
		    dd[ (*allIx)[ c]]=(*src)[c];
		  //		dd[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	      else
		{
		  if( (srcElem-offset) < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxT* allIx = ixList->BuildIx();
		  for( SizeT c=0; c<nCp; ++c)
		    dd[ (*allIx)[ c]]=(*src)[c+offset];
		  //		dd[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	    }
	}
    }
}
template<class Sp>
void Data_<Sp>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList) 
{
  assert( ixList != NULL);

  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar) 
    { // src is scalar
      SizeT nCp=ixList->N_Elements();

      if( nCp == 1)
	{
	  dd[ ixList->LongIx()] = (*src)[0];
	}
      else
	{
	  Ty scalar=(*src)[0];
	  AllIxT* allIx = ixList->BuildIx();
	  for( SizeT c=0; c<nCp; ++c)
	    dd[ (*allIx)[ c]]=scalar;
	  //	    dd[ ixList->GetIx( c)]=scalar;
	}
    }
  else
    {
      // crucial part
      SizeT nCp=ixList->N_Elements();
      
      if( nCp == 1)
	{
	  InsAt( src, ixList);
	}
      else
	{
	  if( srcElem < nCp)
	    throw GDLException("Array subscript must have same size as"
			       " source expression.");
	  
	  AllIxT* allIx = ixList->BuildIx();
	  for( SizeT c=0; c<nCp; ++c)
	    dd[ (*allIx)[ c]]=(*src)[c];
	  //		dd[ ixList->GetIx( c)]=(*src)[c+offset];
	}
    }
}
template<class Sp>
void Data_<Sp>::AssignAt( BaseGDL* srcIn)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar) 
    { // src is scalar
      Ty scalar=(*src)[0];

      dd = scalar;
      
//       SizeT nCp=Data_::N_Elements();

//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c]=scalar;
    }
  else
    {
      SizeT nCp=Data_::N_Elements();
      
      // if (non-indexed) src is smaller -> just copy its number of elements
      if( nCp > srcElem) nCp=srcElem;
      
      for( SizeT c=0; c<nCp; ++c)
	dd[ c]=(*src)[c];
    }
}

// increment/decrement operators
// integers
template<class Sp>
void Data_<Sp>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd -= 1;
//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c]--;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]]--;
    }
}
template<class Sp>
void Data_<Sp>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1;
//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c]++;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]]++;
    }
}
// float, double
template<>
void Data_<SpDFloat>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd -= 1.0f;
      
//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] -= 1.0;
    }
}
template<>
void Data_<SpDFloat>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1.0f;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] += 1.0;
    }
}
template<>
void Data_<SpDDouble>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd -= 1.0;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] -= 1.0;
    }
}
template<>
void Data_<SpDDouble>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1.0;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] += 1.0;
    }
}
// complex
template<>
void Data_<SpDComplex>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd -= 1.0f;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] -= 1.0;
    }
}
template<>
void Data_<SpDComplex>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1.0f;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] += 1.0;
    }
}
template<>
void Data_<SpDComplexDbl>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd -= 1.0;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] -= 1.0;
    }
}
template<>
void Data_<SpDComplexDbl>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1.0;

//       SizeT nCp=Data_::N_Elements();
      
//       for( SizeT c=0; c<nCp; ++c)
// 	dd[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ (*allIx)[ c]] += 1.0;
    }
}
// forbidden types
template<>
void Data_<SpDString>::DecAt( ArrayIndexListT* ixList) 
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
void Data_<SpDPtr>::DecAt( ArrayIndexListT* ixList) 
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
void Data_<SpDObj>::DecAt( ArrayIndexListT* ixList) 
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
void Data_<SpDString>::IncAt( ArrayIndexListT* ixList) 
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
void Data_<SpDPtr>::IncAt( ArrayIndexListT* ixList) 
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
void Data_<SpDObj>::IncAt( ArrayIndexListT* ixList) 
{
  throw GDLException("Object expression not allowed in this context.");
}


// used by AccessDescT for resolving, no checking is done
// inserts srcIn[ ixList] at offset
// used by DotAccessDescT::DoResolve
template<class Sp>
void Data_<Sp>::InsertAt( SizeT offset, BaseGDL* srcIn, 
			  ArrayIndexListT* ixList)
{
  Data_* src=static_cast<Data_* >(srcIn);
  if( ixList == NULL)
    {
      SizeT nCp=src->N_Elements();

      for( SizeT c=0; c<nCp; ++c)
	dd[ c+offset]=(*src)[c];
    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxT* allIx = ixList->BuildIx();
      for( SizeT c=0; c<nCp; ++c)
	dd[ c+offset]=(*src)[ (*allIx)[ c]];
      //	dd[ c+offset]=(*src)[ ixList->GetIx( c)];
    }
}


// used for array concatenation
template<class Sp>
Data_<Sp>* Data_<Sp>::CatArray( ExprListT& exprList,
				const SizeT catRankIx, 
				const SizeT rank)
{
  //  breakpoint();
  SizeT rankIx = RankIx( rank);
  SizeT maxIx = (catRankIx > rankIx)? catRankIx : rankIx;

  dimension     catArrDim(this->dim); // list contains at least one element

  catArrDim.MakeRank( maxIx+1);
  catArrDim.Set(catRankIx,0);     // clear rank which is added up

  SizeT dimSum=0;
  ExprListIterT i=exprList.begin();
  for(; i != exprList.end(); i++)
    {
      // conversion done already here to throw if type is Assoc_<>
      (*i)=(*i)->Convert2( this->t);

      for( SizeT dIx=0; dIx<=maxIx; dIx++)
	{
	  if( dIx != catRankIx) 
	    {
	      if( catArrDim[dIx] == (*i)->Dim(dIx)) continue;
	      if( (catArrDim[dIx] > 1) || ((*i)->Dim(dIx) > 1)) 
                throw  GDLException("Unable to concatenate variables "
                                    "because the dimensions do not agree");
	    }
	  else
	    {
	      SizeT add=(*i)->Dim(dIx);
	      dimSum+=(add)?add:1;
	    }
	}
    }
  
  catArrDim.Set(catRankIx,dimSum);
  
  // the concatenated array
  Data_<Sp>* catArr=New(catArrDim, BaseGDL::NOZERO);
  
  SizeT at=0;
  for( i=exprList.begin(); i != exprList.end(); i++)
    {
      catArr->CatInsert(static_cast<Data_<Sp>*>( (*i)),
			catRankIx,at); // advances 'at'
    }
  
  return catArr;
}

// returns (*this)[ ixList]
template<class Sp>
Data_<Sp>* Data_<Sp>::Index( ArrayIndexListT* ixList)
{
  //  ixList->SetVariable( this);

  Data_* res=Data_::New( ixList->GetDim(), BaseGDL::NOZERO);

  SizeT nCp=ixList->N_Elements();

  //  cout << "nCP = " << nCp << endl;
  //  cout << "dim = " << this->dim << endl;
  
  //  DataT& res_dd = res->dd; 
  AllIxT* allIx = ixList->BuildIx();
  for( SizeT c=0; c<nCp; ++c)
    (*res)[c]=dd[ (*allIx)[ c]];
  //    res_dd[c]=dd[ (*allIx)[ c]];
  //    (*res)[c]=dd[ ixList->GetIx(c)];
  
  return res;
}

// inserts srcIn at index ixDim
// respects the exact structure of srcIn
template<class Sp>
void Data_<Sp>::InsAt( Data_* srcIn, ArrayIndexListT* ixList)
{
  // max. number of dimensions to copy
  SizeT nDim = ixList->NDim();
 
  if( nDim == 1)
    {
      SizeT destStart = ixList->LongIx();

      SizeT len = srcIn->Dim( 0); // length of segment to copy
      // check if in bounds of a
      if( (destStart+len) > this->dim[0])
	throw GDLException("Out of range subscript encountered.");
  
      DataT& srcIn_dd = srcIn->dd; 
      SizeT srcIx = 0; // this one simply runs from 0 to N_Elements(srcIn)

      SizeT destEnd = destStart + len;
      for( SizeT destIx = destStart; destIx < destEnd; ++destIx)
	dd[ destIx] = srcIn_dd[ srcIx++];

      return;
    }

  // ATTENTION: dimension is used as an index here
  SizeT destStart; // 1-dim starting index 
  dimension ixDim = ixList->GetDimIx0( destStart);
  nDim--;

  dimension srcDim=srcIn->Dim();
  SizeT len=srcDim[0]; // length of one segment to copy (one line of srcIn)

  //  SizeT nDim   =RankIx(ixDim.Rank());  
  SizeT srcNDim=RankIx(srcDim.Rank()); // number of source dimensions
  if( srcNDim < nDim) nDim=srcNDim;

  // check limits (up to Rank to consider)
  for( SizeT dIx=0; dIx <= nDim; ++dIx)
    // check if in bounds of a
    if( (ixDim[dIx]+srcDim[dIx]) > this->dim[dIx])
      throw GDLException("Out of range subscript encountered.");

  SizeT nCp=srcIn->Stride(nDim+1)/len; // number of OVERALL copy actions

  // as lines are copied, we need the stride from 2nd dim on
  SizeT retStride[MAXRANK];
  for( SizeT a=0; a <= nDim; ++a) retStride[a]=srcDim.Stride(a+1)/len;
    
  // a magic number, to reset destStart for this dimension
  SizeT resetStep[MAXRANK];
  for( SizeT a=1; a <= nDim; ++a) 
    resetStep[a]=(retStride[a]-1)/retStride[a-1]*this->dim.Stride(a);
	
  //  SizeT destStart=this->dim.LongIndex(ixDim); // starting pos

  DataT& srcIn_dd = srcIn->dd; 

  SizeT srcIx=0; // this one simply runs from 0 to N_Elements(srcIn)
  for( SizeT c=1; c<=nCp; ++c) // linearized verison of nested loops
    {
      // copy one segment
      SizeT destEnd=destStart+len;
      for( SizeT destIx=destStart; destIx<destEnd; ++destIx)
	dd[destIx] = srcIn_dd[ srcIx++];

      // update destStart for all dimensions
      if( c < nCp)
	for( SizeT a=1; a<=nDim; ++a)
	  {
	    if( c % retStride[a])
	      {
		// advance to next
		destStart += this->dim.Stride(a);
		break;
	      }
	    else
	      {
		// reset
		destStart -= resetStep[a];
	      }
	  }
    }
}
  
// used for concatenation, called from CatArray
// assumes that everything is checked (see CatInfo)
template<class Sp>
void Data_<Sp>::CatInsert( const Data_* srcArr, const SizeT atDim, SizeT& at)
{
  // length of one segment to copy
  SizeT len=srcArr->dim.Stride(atDim+1); // src array

  // number of copy actions
  SizeT nCp=srcArr->N_Elements()/len;

  // initial offset
  SizeT destStart= this->dim.Stride(atDim) * at; // dest array
  SizeT destEnd  = destStart + len;

  // number of elements to skip
  SizeT gap=this->dim.Stride(atDim+1);    // dest array

  SizeT srcIx=0;
  for( SizeT c=0; c<nCp; ++c)
    {
      // copy one segment
      for( SizeT destIx=destStart; destIx< destEnd; destIx++)
	dd[destIx] = srcArr->dd[ srcIx++];

      // set new destination pointer
      destStart += gap;
      destEnd   += gap;
    }
      
  SizeT add=srcArr->dim[atDim]; // update 'at'
  at += (add > 1)? add : 1;
}

// Logical True
// integers, also ptr and object
template<class Sp>
bool Data_<Sp>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != 0);
}
template<>
bool Data_<SpDFloat>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != 0.0f);
}
template<>
bool Data_<SpDDouble>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != 0.0);
}
template<>
bool Data_<SpDString>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (s != "");
}
template<>
bool Data_<SpDComplex>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (real(s) != 0.0 || imag(s) != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.");
  return (real(s) != 0.0 || imag(s) != 0.0);
}
// structs are not allowed

// indexed version
// integers, also ptr and object
template<class Sp>
bool Data_<Sp>::LogTrue(SizeT i)
{
  return (dd[i] != 0);
}
template<>
bool Data_<SpDFloat>::LogTrue(SizeT i)
{
  return (dd[i] != 0.0f);
}
template<>
bool Data_<SpDDouble>::LogTrue(SizeT i)
{
  return (dd[i] != 0.0);
}
template<>
bool Data_<SpDString>::LogTrue(SizeT i)
{
  return (dd[i] != "");
}
template<>
bool Data_<SpDComplex>::LogTrue(SizeT i)
{
  return (dd[i].real() != 0.0 || dd[i].imag() != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::LogTrue(SizeT i)
{
  return (dd[i].real() != 0.0 || dd[i].imag() != 0.0);
}
// structs are not allowed

// for WHERE
// integers, also ptr and object
template<class Sp>
DLong* Data_<Sp>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( dd[i] != 0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    }
  else
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i] != 0)
	{
	  ixList[ count++] = i;
	}
  n = count;
  return ixList;
}
template<>
DLong* Data_<SpDFloat>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( dd[i] != 0.0f)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    }
  else
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i] != 0.0f)
	{
	  ixList[ count++] = i;
	}
  n = count;
  return ixList;
}
template<>
DLong* Data_<SpDDouble>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( dd[i] != 0.0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    }
  else
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i] != 0.0)
	{
	  ixList[ count++] = i;
	}
  n = count;
  return ixList;
}
template<>
DLong* Data_<SpDString>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( dd[i] != "")
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    }
  else
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i] != "")
	{
	  ixList[ count++] = i;
	}
  n = count;
  return ixList;
}
template<>
DLong* Data_<SpDComplex>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( dd[i].real() != 0.0 || dd[i].imag() != 0.0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    }
  else
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i].real() != 0.0 || dd[i].imag() != 0.0)
	{
	  ixList[ count++] = i;
	}
  n = count;
  return ixList;
}
template<>
DLong* Data_<SpDComplexDbl>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( dd[i].real() != 0.0 || dd[i].imag() != 0.0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    }
  else
    for( SizeT i=0; i<nEl; ++i)
      if( dd[i].real() != 0.0 || dd[i].imag() != 0.0)
	{
	  ixList[ count++] = i;
	}
  n = count;
  return ixList;
}
// structs are not allowed

// for use by MIN and MAX functions
// integer (NaN not an issue)
template<class Sp>
void Data_<Sp>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  if( minE == NULL)
    {
      DLong maxEl  = 0;
      Ty    maxV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if( dd[i] > maxV)
	    {
	      maxV = dd[i];
	      maxEl  = i;
	    }
	}
      *maxE = maxEl;
      if( maxVal != NULL) *maxVal = new Data_( maxV);
      return;
    }
  if( maxE == NULL)
    {
      DLong minEl  = 0;
      Ty    minV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if( dd[i] < minV)
	    {
	      minV = dd[i];
	      minEl  = i;
	    }
	}
      *minE = minEl;
      if( minVal != NULL) *minVal = new Data_( minV);
      return;
    }

  DLong maxEl  = 0;
  Ty    maxV = dd[0];

  DLong minEl  = 0;
  Ty    minV = dd[0];

  DLong nEl = dd.size();
  for( DLong i=1; i<nEl; ++i)
    {
      if( dd[i] > maxV)
	{
	  maxV = dd[i];
	  maxEl  = i;
	}
      else if( dd[i] < minV)
	{
	  minV = dd[i];
	  minEl  = i;
	}
    }
  *maxE = maxEl;
  if( maxVal != NULL) *maxVal = new Data_( maxV);

  *minE = minEl;
  if( minVal != NULL) *minVal = new Data_( minV);
}
template<>
void Data_<SpDFloat>::MinMax( DLong* minE, DLong* maxE, 
			      BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  if( minE == NULL)
    {
      DLong maxEl  = 0;
      Ty    maxV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i])) continue;
	  if( dd[i] > maxV)
	    {
	      maxV = dd[i];
	      maxEl  = i;
	    }
	  else if (omitNaN && isnan(maxV))
	    {
	      maxV = dd[i];
	      maxEl  = i;
	    }
	}
      *maxE = maxEl;
      if( maxVal != NULL) *maxVal = new Data_( maxV);
      return;
    }
  if( maxE == NULL)
    {
      DLong minEl  = 0;
      Ty    minV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i])) continue;
	  if( dd[i] < minV)
	    {
	      minV = dd[i];
	      minEl  = i;
	    }
	  else if (omitNaN && isnan(minV))
	    {
	      minV = dd[i];
	      minEl  = i;
	    }
	}
      *minE = minEl;
      if( minVal != NULL) *minVal = new Data_( minV);
      return;
    }

  DLong maxEl  = 0;
  Ty    maxV = dd[0];

  DLong minEl  = 0;
  Ty    minV = dd[0];

  DLong nEl = dd.size();
  for( DLong i=1; i<nEl; ++i)
    {
      if (omitNaN && isnan(dd[i])) continue;
      if( dd[i] > maxV)
	{
	  maxV = dd[i];
	  maxEl  = i;
	}
      else if( dd[i] < minV)
	{
	  minV = dd[i];
	  minEl  = i;
	}
      else if (omitNaN && isnan(minV)) // Doesn't matter which one we check here.
	{
	  minV = dd[i];
	  minEl  = i;
	  maxV = dd[i];
	  maxEl  = i;
	}
    }
  *maxE = maxEl;
  if( maxVal != NULL) *maxVal = new Data_( maxV);

  *minE = minEl;
  if( minVal != NULL) *minVal = new Data_( minV);
}
template<>
void Data_<SpDDouble>::MinMax( DLong* minE, DLong* maxE, 
			       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  if( minE == NULL)
    {
      DLong maxEl  = 0;
      Ty    maxV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i])) continue;
	  if( dd[i] > maxV)
	    {
	      maxV = dd[i];
	      maxEl  = i;
	    }
	  else if (omitNaN && isnan(maxV))
	    {
	      maxV = dd[i];
	      maxEl  = i;
	    }
	}
      *maxE = maxEl;
      if( maxVal != NULL) *maxVal = new Data_( maxV);
      return;
    }
  if( maxE == NULL)
    {
      DLong minEl  = 0;
      Ty    minV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i])) continue;
	  if( dd[i] < minV)
	    {
	      minV = dd[i];
	      minEl  = i;
	    }
	  else if (omitNaN && isnan(minV))
	    {
	      minV = dd[i];
	      minEl  = i;
	    }
	}
      *minE = minEl;
      if( minVal != NULL) *minVal = new Data_( minV);
      return;
    }

  DLong maxEl  = 0;
  Ty    maxV = dd[0];

  DLong minEl  = 0;
  Ty    minV = dd[0];

  DLong nEl = dd.size();
  for( DLong i=1; i<nEl; ++i)
    {
      if (omitNaN && isnan(dd[i])) continue;
      if( dd[i] > maxV)
	{
	  maxV = dd[i];
	  maxEl  = i;
	}
      else if( dd[i] < minV)
	{
	  minV = dd[i];
	  minEl  = i;
	}
      else if (omitNaN && isnan(minV)) // Doesn't matter which one we check here.
	{
	  minV = dd[i];
	  minEl  = i;
	  maxV = dd[i];
	  maxEl  = i;
	}
    }
  *maxE = maxEl;
  if( maxVal != NULL) *maxVal = new Data_( maxV);

  *minE = minEl;
  if( minVal != NULL) *minVal = new Data_( minV);
}
template<>
void Data_<SpDString>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  if( minE == NULL)
    {
      DLong maxEl  = 0;
      Ty    maxV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if( dd[i] > maxV)
	    {
	      maxV = dd[i];
	      maxEl  = i;
	    }
	}
      *maxE = maxEl;
      if( maxVal != NULL) *maxVal = new Data_( maxV);
      return;
    }
  if( maxE == NULL)
    {
      DLong minEl  = 0;
      Ty    minV = dd[0];
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if( dd[i] < minV)
	    {
	      minV = dd[i];
	      minEl  = i;
	    }
	}
      *minE = minEl;
      if( minVal != NULL) *minVal = new Data_( minV);
      return;
    }

  DLong maxEl  = 0;
  Ty    maxV = dd[0];

  DLong minEl  = 0;
  Ty    minV = dd[0];

  DLong nEl = dd.size();
  for( DLong i=1; i<nEl; ++i)
    {
      if( dd[i] > maxV)
	{
	  maxV = dd[i];
	  maxEl  = i;
	}
      else if( dd[i] < minV)
	{
	  minV = dd[i];
	  minEl  = i;
	}
    }
  *maxE = maxEl;
  if( maxVal != NULL) *maxVal = new Data_( maxV);

  *minE = minEl;
  if( minVal != NULL) *minVal = new Data_( minV);
}
template<>
void Data_<SpDComplex>::MinMax( DLong* minE, DLong* maxE, 
				BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  if( minE == NULL)
    {
      DLong maxEl  = 0;
      float maxV = dd[0].real();
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i].real())) continue;
	  if( dd[i].real() > maxV)
	    {
	      maxV = dd[i].real();
	      maxEl  = i;
	    }
	  else if (omitNaN && isnan(maxV))
	    {
	      maxV = dd[i].real();
	      maxEl  = i;
	    }
	}
      *maxE = maxEl;
      if( maxVal != NULL) *maxVal = new Data_( dd[ maxEl]);
      return;
    }
  if( maxE == NULL)
    {
      DLong minEl  = 0;
      float minV = dd[0].real();
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i].real())) continue;
	  if( dd[i].real() < minV)
	    {
	      minV = dd[i].real();
	      minEl  = i;
	    }
	  else if (omitNaN && isnan(minV))
	    {
	      minV = dd[i].real();
	      minEl  = i;
	    }
	}
      *minE = minEl;
      if( minVal != NULL) *minVal = new Data_( dd[ minEl]);
      return;
    }

  DLong maxEl  = 0;
  float maxV = dd[0].real();

  DLong minEl  = 0;
  float minV = dd[0].real();

  DLong nEl = dd.size();
  for( DLong i=1; i<nEl; ++i)
    {
      if (omitNaN && isnan(dd[i].real())) continue;
      if( dd[i].real() > maxV)
	{
	  maxV = dd[i].real();
	  maxEl  = i;
	}
      else if( dd[i].real() < minV)
	{
	  minV = dd[i].real();
	  minEl  = i;
	}
      else if (omitNaN && isnan(minV)) // Doesn't matter which one we check here.
	{
	  minV = dd[i].real();
	  minEl  = i;
	  maxV = dd[i].real();
	  maxEl  = i;
	}
    }
  *maxE = maxEl;
  if( maxVal != NULL) *maxVal = new Data_( dd[ maxEl]);

  *minE = minEl;
  if( minVal != NULL) *minVal = new Data_( dd[ minEl]);
}
template<>
void Data_<SpDComplexDbl>::MinMax( DLong* minE, DLong* maxE, 
				BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  if( minE == NULL)
    {
      DLong maxEl  = 0;
      double maxV = dd[0].real();
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i].real())) continue;
	  if( dd[i].real() > maxV)
	    {
	      maxV = dd[i].real();
	      maxEl  = i;
	    }
	  else if (omitNaN && isnan(maxV))
	    {
	      maxV = dd[i].real();
	      maxEl  = i;
	    }
	}
      *maxE = maxEl;
      if( maxVal != NULL) *maxVal = new Data_( dd[ maxEl]);
      return;
    }
  if( maxE == NULL)
    {
      DLong minEl  = 0;
      double minV = dd[0].real();
      DLong nEl = dd.size();
      for( DLong i=1; i<nEl; ++i)
	{
	  if (omitNaN && isnan(dd[i].real())) continue;
	  if( dd[i].real() < minV)
	    {
	      minV = dd[i].real();
	      minEl  = i;
	    }
	  else if (omitNaN && isnan(minV))
	    {
	      minV = dd[i].real();
	      minEl  = i;
	    }
	}
      *minE = minEl;
      if( minVal != NULL) *minVal = new Data_( dd[ minEl]);
      return;
    }

  DLong maxEl  = 0;
  double maxV = dd[0].real();

  DLong minEl  = 0;
  double minV = dd[0].real();

  DLong nEl = dd.size();
  for( DLong i=1; i<nEl; ++i)
    {
      if (omitNaN && isnan(dd[i].real())) continue;
      if( dd[i].real() > maxV)
	{
	  maxV = dd[i].real();
	  maxEl  = i;
	}
      else if( dd[i].real() < minV)
	{
	  minV = dd[i].real();
	  minEl  = i;
	}
      else if (omitNaN && isnan(minV)) // Doesn't matter which one we check here.
	{
	  minV = dd[i].real();
	  minEl  = i;
	  maxV = dd[i].real();
	  maxEl  = i;
	}
    }
  *maxE = maxEl;
  if( maxVal != NULL) *maxVal = new Data_( dd[ maxEl]);

  *minE = minEl;
  if( minVal != NULL) *minVal = new Data_( dd[ minEl]);
}
void DStructGDL::MinMax( DLong* minE, DLong* maxE, 
			 BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN)
{
  throw GDLException("Struct expression not allowed in this context.");
}

template<>
BaseGDL* Data_<SpDString>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
				   bool center, int edgeMode)
{
  throw GDLException("String expression not allowed in this context.");
}

#define INCLUDE_CONVOL_CPP 1
#define CONVOL_BYTE__

#include "convol.cpp"

#undef CONVOL_BYTE__

#include "convol.cpp"

template<>
BaseGDL* Data_<SpDString>::Rebin( const dimension& newDim, bool sample)
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDComplex>::Rebin( const dimension& newDim, bool sample)
{
  throw GDLException("Complex expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Rebin( const dimension& newDim, bool sample)
{
  throw GDLException("Double complex expression not allowed in this context.");
}


// rebin over dimIx, new value: newDim
// newDim != srcDim[ dimIx] -> compress or expand
template< typename T>
T* Rebin1( T* src, 
	   const dimension& srcDim, 
	   SizeT dimIx, SizeT newDim, bool sample)
{
  SizeT nEl = src->N_Elements();
  
  if( newDim == 0) newDim = 1;

  // get dest dim and number of summations
  dimension destDim = srcDim;

  destDim.MakeRank( dimIx + 1);

  SizeT srcDimIx = destDim[ dimIx];

  destDim.Set( dimIx, newDim);

  SizeT resStride   = destDim.Stride( dimIx); 

  // dimStride is also the number of linear src indexing
  SizeT dimStride   = srcDim.Stride( dimIx); 
  SizeT outerStride = srcDim.Stride( dimIx + 1);

  SizeT rebinLimit = srcDimIx * dimStride;
    
  if( newDim < srcDimIx) // compress
    {
    
      SizeT ratio = srcDimIx / newDim;
 
      if( sample)
	{
	  T* res = new T( destDim, BaseGDL::NOZERO);
    
	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride*ratio) // run over dim
		  {
		    (*res)[ (s / dimStride / ratio) * dimStride + i] = (*src)[ s];
		  }
	      }
      
	  return res;
	}
      else
	{
	  T* res = new T( destDim); // zero fields

	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride) // run over dim
		  {
		    // the way s indexes:
		    // assume over b (compress index)
		    // src[ a, b, c]
		    // [ 0, 0, 0] [ 0, 1, 0] [ 0, 2, 0] ...
		    // [ 1, 0, 0] [ 1, 1, 0] [ 1, 2, 0] ...
		    // [ 2, 0, 0] [ 2, 1, 0] [ 2, 2, 0] ...

		    (*res)[ (s / dimStride / ratio) * dimStride + i] += (*src)[ s];
		  }
	      }
      
	  SizeT resEl = res->N_Elements();
	  for( SizeT r=0; r < resEl; ++r)
	    (*res)[ r] /= ratio;

	  return res;
	}
    }
  else // expand
    {
      T* res = new T( destDim, BaseGDL::NOZERO);

      if( sample)
	{
	  SizeT ratio = newDim / srcDimIx;
 
	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride) // run over dim
		  {
		    typename T::Ty src_s = (*src)[ s];

		    SizeT s_dimStride_ratio_dimStride_i = 
		      (s / dimStride) * ratio * dimStride + i;

		    for( SizeT r=0; r<ratio; ++r)
		      {
			(*res)[ s_dimStride_ratio_dimStride_i + r * dimStride] = 
			  src_s;
		      }
		  }
	      }
	}
      else
	{
	  DLong64 ratio = newDim / srcDimIx; // make sure 32 bit integers are working also

	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride) // run over dim
		  {
		    typename T::Ty first = (*src)[ s];
		    typename T::Ty next = 
		      (*src)[ (s+dimStride)<oiLimit?s+dimStride:s];

		    SizeT s_dimStride_ratio_dimStride_i = 
		      (s / dimStride) * ratio * dimStride + i;
		    for( DLong64 r=0; r<ratio; ++r)
		      {
			(*res)[ s_dimStride_ratio_dimStride_i + r * dimStride] = 
			  (first * (ratio - r) + next * r) / ratio; // 64 bit temporary
		      }
		  }
	      }
	}

      return res;
    }
}

// for integer
template< typename T, typename TNext>
T* Rebin1Int( T* src, 
	      const dimension& srcDim, 
	      SizeT dimIx, SizeT newDim, bool sample)
{
  SizeT nEl = src->N_Elements();
  
  if( newDim == 0) newDim = 1;

  // get dest dim and number of summations
  dimension destDim = srcDim;

  destDim.MakeRank( dimIx + 1);

  SizeT srcDimIx = destDim[ dimIx];

  destDim.Set( dimIx, newDim);

  SizeT resStride   = destDim.Stride( dimIx); 

  // dimStride is also the number of linear src indexing
  SizeT dimStride   = srcDim.Stride( dimIx); 
  SizeT outerStride = srcDim.Stride( dimIx + 1);

  SizeT rebinLimit = srcDimIx * dimStride;
    
  if( newDim < srcDimIx) // compress
    {
    
      SizeT ratio = srcDimIx / newDim;
 
      if( sample)
	{
	  T* res = new T( destDim, BaseGDL::NOZERO);
    
	  SizeT ratio = srcDimIx / newDim;
 
	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride*ratio) // run over dim
		  {
		    (*res)[ (s / dimStride / ratio) * dimStride + i] = (*src)[ s];
		  }
	      }
      
	  return res;
	}
      else
	{
	  T* res = new T( destDim); // zero fields

	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		TNext tmp = 0;
		for( SizeT s=oi; s<oiLimit; s += dimStride) // run over dim
		  {
		    tmp += (*src)[ s];
		    
		    if( (s / dimStride) % ratio == (ratio - 1))
		      {
			(*res)[ (s / dimStride / ratio) * dimStride + i] = tmp / ratio;
			tmp = 0;
		      }
		  }
	      }
      
	  return res;
	}
    }
  else // expand
    {
      T* res = new T( destDim, BaseGDL::NOZERO);

      if( sample)
	{
	  SizeT ratio = newDim / srcDimIx;
 
	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride) // run over dim
		  {
		    typename T::Ty src_s = (*src)[ s];

		    SizeT s_dimStride_ratio_dimStride_i = 
		      (s / dimStride) * ratio * dimStride + i;

		    for( SizeT r=0; r<ratio; ++r)
		      {
			(*res)[ s_dimStride_ratio_dimStride_i + r * dimStride] = 
			  src_s;
		      }
		  }
	      }
	}
      else
	{
	  DLong64 ratio = newDim / srcDimIx; // make sure 32 bit integers are working also

	  for( SizeT o=0; o < nEl; o += outerStride) // outer dim
	    for( SizeT i=0; i < dimStride; ++i) // src element offset (lower dim)
	      {
		SizeT oi = o+i;
		SizeT oiLimit = oi + rebinLimit;

		for( SizeT s=oi; s<oiLimit; s += dimStride) // run over dim
		  {
		    typename T::Ty first = (*src)[ s];
		    typename T::Ty next = 
		      (*src)[ (s+dimStride)<oiLimit?s+dimStride:s];

		    SizeT s_dimStride_ratio_dimStride_i = 
		      (s / dimStride) * ratio * dimStride + i;
		    for( DLong64 r=0; r<ratio; ++r)
		      {
			(*res)[ s_dimStride_ratio_dimStride_i + r * dimStride] = 
			  (first * (ratio - r) + next * r) / ratio; // 64 bit temporary
		      }
		  }
	      }
	}

      return res;
    }
}

// for float, double
template<class Sp>
BaseGDL* Data_<Sp>::Rebin( const dimension& newDim, bool sample)
{
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();

  SizeT nDim;
  if( resRank < srcRank) 
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim( this->dim);
  Data_* actIn = this;

  // 1st compress
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[d] <  this->dim[d])
      { // compress
	
	Data_* act = Rebin1( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  this->dim[d])
      { // expand
	
	Data_* act = Rebin1( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }
  
  if( actIn == this) return actIn->Dup();
  return actIn;
}

// integer types
template<>
BaseGDL* Data_<SpDByte>::Rebin( const dimension& newDim, bool sample)
{
  SizeT resRank = newDim.Rank();
  SizeT srcRank = Rank();

  SizeT nDim;
  if( resRank < srcRank) 
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim( dim);
  Data_* actIn = this;

  // 1st compress
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[d] <  dim[d])
      { // compress
	
	Data_* act = Rebin1Int<DByteGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DByteGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }
  
  if( actIn == this) return actIn->Dup();
  return actIn;
}
template<>
BaseGDL* Data_<SpDInt>::Rebin( const dimension& newDim, bool sample)
{
  SizeT resRank = newDim.Rank();
  SizeT srcRank = Rank();

  SizeT nDim;
  if( resRank < srcRank) 
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim( dim);
  Data_* actIn = this;

  // 1st compress
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[d] <  dim[d])
      { // compress
	
	Data_* act = Rebin1Int<DIntGDL, DLong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DIntGDL, DLong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }
  
  if( actIn == this) return actIn->Dup();
  return actIn;
}
template<>
BaseGDL* Data_<SpDUInt>::Rebin( const dimension& newDim, bool sample)
{
  SizeT resRank = newDim.Rank();
  SizeT srcRank = Rank();

  SizeT nDim;
  if( resRank < srcRank) 
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim( dim);
  Data_* actIn = this;

  // 1st compress
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[d] <  dim[d])
      { // compress
	
	Data_* act = Rebin1Int<DUIntGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DUIntGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }
  
  if( actIn == this) return actIn->Dup();
  return actIn;
}
template<>
BaseGDL* Data_<SpDLong>::Rebin( const dimension& newDim, bool sample)
{
  SizeT resRank = newDim.Rank();
  SizeT srcRank = Rank();

  SizeT nDim;
  if( resRank < srcRank) 
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim( dim);
  Data_* actIn = this;

  // 1st compress
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[d] <  dim[d])
      { // compress
	
	Data_* act = Rebin1Int<DLongGDL, DLong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DLongGDL, DLong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }
  
  if( actIn == this) return actIn->Dup();
  return actIn;
}
template<>
BaseGDL* Data_<SpDULong>::Rebin( const dimension& newDim, bool sample)
{
  SizeT resRank = newDim.Rank();
  SizeT srcRank = Rank();

  SizeT nDim;
  if( resRank < srcRank) 
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim( dim);
  Data_* actIn = this;

  // 1st compress
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[d] <  dim[d])
      { // compress
	
	Data_* act = Rebin1Int<DULongGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DULongGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) delete actIn;
	actIn = act;
      }
  
  if( actIn == this) return actIn->Dup();
  return actIn;
}

// plain copy of nEl from src
// no checking
template<class Sp>
void Data_<Sp>::Assign( BaseGDL* src, SizeT nEl)
{
  Data_* srcT = dynamic_cast<Data_*>( src);
  auto_ptr< Data_> srcTGuard;
  if( srcT == NULL) 
    {
      srcT = static_cast<Data_*>( src->Convert2( Data_::t, BaseGDL::COPY));
      srcTGuard.reset( srcT);
    }

  for(long k=0; k < nEl; ++k)
    {
      dd[ k] = srcT->dd[ k];
    }
}

// return a new type of itself (only for one dimensional case)
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( SizeT ix)
{
  return new Data_( dd[ ix]);
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( AllIxT* ix, dimension* dIn)
{
  SizeT nCp = ix->size();
  Data_* res=Data_::New( *dIn, BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c)
    (*res)[c]=dd[ (*ix)[ c]];
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFrom( SizeT s)
{
  SizeT nCp = dd.size() - s;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c)
    (*res)[c]=dd[ s++];
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFrom( SizeT s, SizeT e)
{
  SizeT nCp = e - s + 1;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c)
    (*res)[c]=dd[ s++];
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFromStride( SizeT s, SizeT stride)
{
  SizeT nCp = (dd.size() - s + stride - 1)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    (*res)[c]=dd[ s];
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{
  SizeT nCp = (e - s + stride)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    (*res)[c]=dd[ s];
  return res;
}

#define NEWIX_UNSIGNEDINT \
SizeT i = 0;\
for( ; i < nElem; ++i)\
  if( (*src)[i] > upper)\
    {\
      if( strict)\
	throw GDLException("Array used to subscript array "\
			   "contains out of range (>) subscript.");\
      (*res)[i++]= upperVal;\
      break;\
    }\
  else\
    (*res)[i]= dd[ (*src)[i]];\
for(; i < nElem; ++i)\
  if( (*src)[i] > upper)\
    (*res)[i] = upperVal;\
  else\
    (*res)[i]= dd[ (*src)[i]];\
return guard.release();

#define NEWIX_SIGNEDINT \
  SizeT i = 0;\
for(; i < nElem; ++i)\
	  if( (*src)[i] < 0)\
	    {\
	      if( strict)\
		throw GDLException("Array used to subscript array "\
				   "contains out of range (<0) subscript.");\
	      (*res)[i++]= zeroVal;\
	      break;\
	    }\
	  else if( (*src)[i] > upper)\
	    {\
	      if( strict)\
		throw GDLException("Array used to subscript array "\
				   "contains out of range (>) subscript.");\
	      (*res)[i++]= upperVal;\
	      break;\
	    }\
	  else\
	    (*res)[ i] = dd[ (*src)[ i]];\
	for(; i < nElem; ++i)\
	  if( (*src)[i] < 0)\
	    (*res)[i]= zeroVal;\
	  else if( (*src)[i] > upper)\
	    (*res)[i]= upperVal;\
	  else\
	    (*res)[ i] = dd[ (*src)[ i]];\
	return guard.release();

template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( BaseGDL* ix, bool strict)
{
  SizeT nElem = ix->N_Elements();

  Data_* res = New( ix->Dim(), BaseGDL::NOZERO);
  auto_ptr<Data_> guard( res);

  SizeT upper = dd.size() - 1;
  Ty    upperVal = dd[ upper];
  Ty    zeroVal  = dd[ 0];
  switch( ix->Type())
    {
    case BYTE:
      {
	DByteGDL* src = static_cast<DByteGDL*>( ix);

	NEWIX_UNSIGNEDINT
	
// 	SizeT i = 0;
// 	for( ; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]];

// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    (*res)[i] = upperVal;
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]]; 
	
// 	return guard.release();
      }
    case INT:
      {
	DIntGDL* src = static_cast<DIntGDL*>( ix);

	NEWIX_SIGNEDINT

// 	SizeT i = 0;
// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] < 0)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (<0) subscript.");
// 	      (*res)[i++]= zeroVal;
// 	      break;
// 	    }
// 	  else if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[ i] = dd[ (*src)[ i]];
	
// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] < 0)
// 	    (*res)[i]= zeroVal;
// 	  else if( (*src)[i] > upper)
// 	    (*res)[i]= upperVal;
// 	  else
// 	    (*res)[ i] = dd[ (*src)[ i]];
	
// 	return guard.release();
      }
    case UINT:
      {
	DUIntGDL* src = static_cast<DUIntGDL*>( ix);

	NEWIX_UNSIGNEDINT

// 	SizeT i = 0;
// 	for( ; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]];

// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] >= upper)
// 	    (*res)[i] = upperVal;
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]]; 
	
// 	return guard.release();
      }
    case LONG: // typical type (returned from WHERE)
      {
	DLongGDL* src = static_cast<DLongGDL*>( ix);

	NEWIX_SIGNEDINT

// 	SizeT i = 0;
// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] < 0)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (<0) subscript.");
// 	      (*res)[i++]= zeroVal;
// 	      break;
// 	    }
// 	  else if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[ i] = dd[ (*src)[ i]];
	
// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] <= 0)
// 	    (*res)[i]= zeroVal;
// 	  else if( (*src)[i] >= upper)
// 	    (*res)[i]= upperVal;
// 	  else
// 	    (*res)[ i] = dd[ (*src)[ i]];
	
// 	return guard.release();
      }
    case ULONG:
      {
	DULongGDL* src = static_cast<DULongGDL*>( ix);

	NEWIX_UNSIGNEDINT

// 	SizeT i = 0;
// 	for( ; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]];

// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    (*res)[i] = upperVal;
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]]; 
	
// 	return guard.release();
      }
    case LONG64:
      {
	DLong64GDL* src = static_cast<DLong64GDL*>( ix);

	NEWIX_SIGNEDINT

// 	SizeT i = 0;
// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] < 0)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (<0) subscript.");
// 	      (*res)[i++]= zeroVal;
// 	      break;
// 	    }
// 	  else if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[ i] = dd[ (*src)[ i]];
	
// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] <= 0)
// 	    (*res)[i]= zeroVal;
// 	  else if( (*src)[i] >= upper)
// 	    (*res)[i]= upperVal;
// 	  else
// 	    (*res)[i] = dd[ (*src)[i]];
	
// 	return guard.release();
      }
    case ULONG64:
      {
	DULong64GDL* src = static_cast<DULong64GDL*>( ix);

	NEWIX_UNSIGNEDINT

// 	SizeT i = 0;
// 	for( ; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    {
// 	      if( strict)
// 		throw GDLException("Array used to subscript array "
// 				   "contains out of range (>) subscript.");
// 	      (*res)[i++]= upperVal;
// 	      break;
// 	    }
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]];

// 	for(; i < nElem; ++i)
// 	  if( (*src)[i] > upper)
// 	    (*res)[i] = upperVal;
// 	  else
// 	    (*res)[i]= dd[ (*src)[i]]; 
	
// 	return guard.release();
      }
    case FLOAT: 
      {
	DFloat maxF = upper; 
	DFloatGDL* src = static_cast<DFloatGDL*>( ix);
	for( SizeT i=0; i < nElem; ++i)
	  if( (*src)[i] <= 0.0)
	    {
	      (*res)[i] = zeroVal;
	      if( (*src)[i] <= -1.0 && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (<0) subscript.");
	    }
	  else if( (*src)[i] > maxF)
	    {
	      (*res)[i] = upperVal;
	      if( (*src)[i] >= (maxF + 1.0) && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (>) subscript.");
	    }
	  else
	    {
	      (*res)[i]= dd[ Real2Int<SizeT,float>((*src)[i])]; 
	    }
	return guard.release();
      }
    case DOUBLE: 
      {
	DDouble maxF = upper; 
	DDoubleGDL* src = static_cast<DDoubleGDL*>( ix);
	for( SizeT i=0; i < nElem; ++i)
	  if( (*src)[i] <= 0.0)
	    {
	      (*res)[i] = zeroVal;
	      if( (*src)[i] <= -1.0 && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (<0) subscript.");
	    }
	  else if( (*src)[i] > maxF)
	    {
	      (*res)[i] = upperVal;
	      if( (*src)[i] >= (maxF + 1.0) && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (>) subscript.");
	    }
	  else
	    {
	      (*res)[i]= dd[ Real2Int<SizeT,double>((*src)[i])]; 
	    }
	return guard.release();
      }
    case STRING: 
      {
	DStringGDL* src = static_cast<DStringGDL*>( ix);
	for( SizeT i=0; i < nElem; ++i)
	  {
	    const char* cStart=(*src)[i].c_str();
	    char* cEnd;
	    long l=strtol(cStart,&cEnd,10);
	    if( cEnd == cStart)
	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING to LONG.");
		(*res)[i] = zeroVal;
	      }
	    else if( l < 0)
	      {
		if( strict)
		  throw GDLException("Array used to subscript array "
				     "contains out of range (<0) subscript.");
		(*res)[i] = zeroVal;
	      }
	    else if( l > upper)
	      {
		if( strict)
		  throw GDLException("Array used to subscript array "
				     "contains out of range (>) subscript.");
		(*res)[i] = upperVal;
	      }
	    else
	      {
		(*res)[i] = dd[ l];
	      }
	  }
	return guard.release();
      }
    case COMPLEX: 
      {
	DFloat maxF = upper; 
	DComplexGDL* src = static_cast<DComplexGDL*>( ix);
	for( SizeT i=0; i < nElem; ++i)
	  if( real((*src)[i]) <= 0.0)
	    {
	      if( real((*src)[i]) <= -1.0 && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (<0) subscript.");
	      (*res)[i] = zeroVal;
	    }
	  else if( real((*src)[i]) > upper)
	    {
	      if( real((*src)[i]) >= upper+1.0 && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (>) subscript.");
	      (*res)[i] = upperVal;
	    }
	  else
	    {
	      (*res)[i]= dd[ Real2Int<DLong,float>(real((*src)[i]))]; 
	    }
	return guard.release();
      }
    case COMPLEXDBL: 
      {
	DDouble maxF = upper; 
	DComplexDblGDL* src = static_cast<DComplexDblGDL*>( ix);
	for( SizeT i=0; i < nElem; ++i)
	  if( real((*src)[i]) <= 0.0)
	    {
	      if( real((*src)[i]) <= -1.0 && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (<0) subscript.");
	      (*res)[i] = zeroVal;
	    }
	  else if( real((*src)[i]) > upper)
	    {
	      if( real((*src)[i]) >= upper+1.0 && strict)
		throw GDLException("Array used to subscript array "
				   "contains out of range (>) subscript.");
	      (*res)[i] = upperVal;
	    }
	  else
	    {
	      (*res)[i]= dd[ Real2Int<DLong,float>(real((*src)[i]))]; 
	    }
	return guard.release();
      }
    default:
      {
	DType dType = ix->Type();
	assert( dType != UNDEF);
    
	int typeCheck = DTypeOrder[ dType];
	if( typeCheck >= 100)
	  throw GDLException("Type not allowed as subscript.");

	assert( 0);
      }
    }
  assert( 0);
}

#undef NEWIX_SIGNEDINT
#undef NEWIX_UNSIGNEDINT

//#include "instantiate_templates.hpp"

template class Data_< SpDByte>;
template class Data_< SpDInt>;
template class Data_< SpDUInt>;
template class Data_< SpDLong>;
template class Data_< SpDULong>;
template class Data_< SpDLong64>;
template class Data_< SpDULong64>;
template class Data_< SpDPtr>;
template class Data_< SpDFloat>;
template class Data_< SpDDouble>;
template class Data_< SpDString>;
template class Data_< SpDObj>;
template class Data_< SpDComplex>;
template class Data_< SpDComplexDbl>;

