/***************************************************************************
                          typedefs.hpp  -  basic typedefs
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

#ifndef TYPEDEFS_HPP_
#define TYPEDEFS_HPP_

// check for needed libraries here as this file is included by most sources
// (via dimension.hpp via basegdl.hpp)
#ifdef HAVE_CONFIG_H

#include <config.h>

#ifndef HAVE_LIBGSL
#error "GNU Scientific Library not installed. Please see README file."
#endif

#ifndef HAVE_LIBGSLCBLAS
#error "CBLAS (part of GNU Scientific Library) not installed. Please see README file."
#endif

#ifndef HAVE_LIBPLPLOTCXXD
#error "plplot not installed. Please see README file."
#endif

#endif // HAVE_CONFIG_H

// Python.h must be included before everything else
#if defined(USE_PYTHON) || defined(PYTHON_MODULE)
//#undef _POSIX_C_SOURCE // get rid of warning
#include <Python.h>
//#ifndef _POSIX_C_SOURCE 
//#warning "_POSIX_C_SOURCE not defined in Python.h (remove #undef)"
//#endif
#endif

#include <set>
#include <string>
#include <deque>
#include <complex>
#include <vector>
#include <valarray>
#include <cassert>

#if defined(HAVE_64BIT_OS)
typedef unsigned long long int      SizeT;
#else
typedef unsigned int        	    SizeT;
#endif
//typedef size_t              SizeT;
typedef unsigned int        UInt;
typedef unsigned long       ULong;


// convenient naming
typedef unsigned char          DByte;
// typedef int                    DInt;
// typedef unsigned int           DUInt;
// typedef long int               DLong;
// typedef unsigned long int      DULong;

#ifdef _MSC_VER
typedef __int64               DLong64;
typedef unsigned __int64      DULong64;
#else
//typedef long int               DLong64;
//typedef unsigned long int      DULong64;
typedef long long int          DLong64;
typedef unsigned long long int DULong64;
#endif

typedef short                  DInt;
typedef unsigned short         DUInt;
typedef int                    DLong;
typedef unsigned int           DULong;
typedef float                  DFloat;
typedef double                 DDouble;
typedef std::string            DString;
typedef SizeT                  DPtr; // ptr to heap
typedef DPtr                   DObj; // ptr to object heap
typedef std::complex<float>    DComplex;
typedef std::complex<double>   DComplexDbl;


// list of identifiers (used in several places)
typedef std::deque<std::string>       IDList;
typedef std::deque<std::string>       StrArr;

// used by file.cpp and in other places 
typedef std::deque<DString>           FileListT;

typedef std::valarray<SizeT>          AllIxT;

typedef std::set< DPtr>               DPtrListT;

class ArrayIndexT;
typedef std::vector<ArrayIndexT*> ArrayIndexVectorT;

// searches IDList idL for std::string s, returns its position, -1 if not found
inline int FindInIDList(IDList& idL,const std::string& s)
{
  int ix=0;
  for(IDList::iterator i=idL.begin();
      i != idL.end(); ++i, ++ix) if( *i==s) {
	return ix;
      }

  return -1;
}

// like auto_ptr but for arrays (delete[] is used upon destruction)
template <class T>
class ArrayGuard
{
private:
  T*      guarded;
  
public:
  ArrayGuard(): guarded( NULL)
  {}
  ArrayGuard( T* c): guarded( c)
  {}
  
  void Reset( T* newGuarded)
  {
    delete guarded;
    guarded = newGuarded;
  }  

  ~ArrayGuard()
  {
    delete[] guarded;
  }
};

// maintains size of stack, needed for exceptions
template <class T>
class StackGuard
{
private:
  T&     container;
  SizeT  cSize;
  
public:
  StackGuard( T& c): container( c)
  {
    cSize=container.size();
  }
  
  ~StackGuard()
  {
    for( SizeT s=container.size(); s > cSize; s--)
      {
	delete container.back();
	container.pop_back();
      }
  }
};

// needed for exceptions
template <class T>
class StackSizeGuard
{
private:
  T&     container;
  SizeT  cSize;
  
public:
  StackSizeGuard( T& c): container( c)
  {
    cSize=container.size();
  }
  
  ~StackSizeGuard()
  {
    for( SizeT s=container.size(); s > cSize; s--)
      { // no deleting here
	container.pop_back();
      }
  }
};

// needed for exception savety (assures that after destruction the value
// will be the same as on instantiation)
template <class T>
class ValueGuard
{
private:
  T&     val;
  T      oldVal;

public:
  ValueGuard( T& v): val( v), oldVal( v)
  {}
  
  ~ValueGuard()
  {
    val = oldVal;
  }
};

// like stackguard, but allows releasing
template <class T>
class PtrGuard
{
private:
  T*     container;
  SizeT  cSize;
  
public:
  PtrGuard( T* c): container( c)
  {
    cSize=container->size();
  }
  
  ~PtrGuard()
  {
    if( container != NULL)
      for( SizeT s=container->size(); s > cSize; s--)
	{
	  delete container->back();
	  container->pop_back();
	}
  }

  T* Release() { T* r=container; container=NULL; return r;}
};

template <class T>
class GDLArray
{
private:
  T scalar;
  T*    buf;
  SizeT sz;

public:
  GDLArray(): buf( NULL), sz( 0) {}
  GDLArray( const GDLArray& cp)
    : buf( (cp.size() > 1)?new T[ cp.size()] : &scalar)
    , sz( cp.size())
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] = cp.buf[ i];
  }

  GDLArray( SizeT s, bool b): buf( (s>1)?new T[ s] : &scalar), sz( s)
  {}
  GDLArray( T val, SizeT s): buf((s>1)?new T[ s]: &scalar), sz( s)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] = val;
  }
  GDLArray( const T* arr, SizeT s): buf( (s>1)?new T[ s]: &scalar), sz( s)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] = arr[ i];
  }

  explicit GDLArray( const T& s): scalar( s), buf( &scalar), sz( 1)
  {}

  ~GDLArray()
  {
    if( sz > 1)
      delete[] buf;
  }

  T& operator[]( SizeT ix)
  {
    return buf[ ix];
  }
  const T& operator[]( SizeT ix) const
  {
    return buf[ ix];
  }

  GDLArray& operator=( const GDLArray& right)
  {
    assert( sz == right.size());
    //     if( &right != this)
    //       {
    if( sz == right.size())
      {
	for( SizeT i=0; i<sz; ++i)
	  buf[ i] = right.buf[ i];
      }
    else
      {
	if( sz > 1) 
	  delete[] buf;
	sz = right.size();
	buf = (sz>1) ? new T[ sz] : &scalar;
	for( SizeT i=0; i<sz; ++i)
	  buf[ i] = right.buf[ i];
      }
    //       }
    return *this;
  }
  GDLArray&operator+=( const GDLArray& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right.buf[ i];
    return *this;
  }
  GDLArray&operator-=( const GDLArray& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right.buf[ i];
    return *this;
  }
  GDLArray&operator*=( const GDLArray& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] *= right.buf[ i];
    return *this;
  }
  GDLArray&operator/=( const GDLArray& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] /= right.buf[ i];
    return *this;
  }
  GDLArray&operator+=( const T& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right;
    return *this;
  }
  GDLArray&operator-=( const T& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right;
    return *this;
  }
  GDLArray&operator*=( const T& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] *= right;
    return *this;
  }
  GDLArray&operator/=( const T& right)
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] /= right;
    return *this;
  }

  void SetBuffer( T* b)
  {
    buf = b;
  }
  T* GetBuffer()
  {
    return buf;
  }
  void SetBufferSize( SizeT s)
  {
    sz = s;
  }

  SizeT size() const
  {
    return sz;
  }

  void resize( SizeT newSz)
  {
    assert( newSz > sz);
    T* newBuf = new T[ newSz];
    for( SizeT i=0; i<sz; ++i)
      newBuf[ i] = buf[ i];
    if( sz > 1)
      delete[] buf;
    buf = newBuf;
    sz = newSz;
  }

  T min() const
  {
    T res = buf[ 0];
    for( SizeT i=1; i<sz; ++i)
      if( res < buf[ i]) res = buf[ i];
    return res;
  }
  T max() const
  {
    T res = buf[ 0];
    for( SizeT i=1; i<sz; ++i)
      if( res > buf[ i]) res = buf[ i];
    return res;
  }
};

// friend  GDLArray pow(const GDLArray& left, const GDLArray& right);

// friend  GDLArray pow(const GDLArray& left, const T& right);

// friend  GDLArray pow(const T& left, const GDLArray& right);

// };

// template<class Ty>
//     GDLArray<Ty> pow(const GDLArray<Ty>& left,
//         const GDLArray<Ty>& right)
// {
//   GDLArray<Ty> res( left.size);
  
//   for( SizeT i=0; i<left.size(); ++i)
//     res[ i] = pow(left[i],right[i]);
// }
// template<class Ty>
//     GDLArray<Ty> pow(const GDLArray<Ty> left, const Ty& right);
// template<class Ty>
//     GDLArray<Ty> pow(const Ty& left, const GDLArray<Ty>& right);

#endif
