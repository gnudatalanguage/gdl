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
// #include <string.h> // memcopy
#include <cstring> // memcopy
#include <deque>
#include <complex>
#include <vector>
#include <valarray>
#include <cassert>
// #include <cstdio>

#undef USE_MPFR

#ifdef USE_MPFR
#include "mpreal.h"
#endif

// // undef for releases (should not give diagnostics)
// // define for the CVS (where the default sizes can easily be adjusted)
// #define GDL_CVS_VERSION
// //#undef GDL_CVS_VERSION
// // ?
// #ifdef GDL_CVS_VERSION
// #include <iostream>
// #endif

//#define TRACE_OMP_CALLS
#undef TRACE_OMP_CALLS

#if defined(_OPENMP) && defined(TRACE_OMP_CALLS)
#define TRACEOMP( file, line)   std::cout << "TRACEOMP\t" << file << "\t" << line << std::endl;
#else
#define TRACEOMP( file, line) 
#endif

// SA: fixing bug no. 3296360
typedef unsigned long long int      SizeT;
typedef long long int RangeT;
typedef long long int OMPInt;

const SizeT MAXRANK=8;         // arrays are limited to 8 dimensions
const std::string MAXRANK_STR("8");  // for use in strings (error messages)

//// SA: the version introduced by Joel in 2006:
////#if defined(HAVE_64BIT_OS)
////  typedef unsigned long long int      SizeT;
////  typedef long long int RangeT;
////#else
////  typedef unsigned int        	    SizeT;
////  typedef int                            RangeT;
////#endif

// SA: the original version from 2005:
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

#ifdef USE_MPFR

typedef __int128               DLong128;
typedef unsigned __int128      DULong128;

typedef long double            DLDouble;
typedef std::complex<DLDouble> DComplexLDbl;


typedef mpfr::mpreal           DArbitrary;
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
typedef std::complex<DFloat>   DComplex;
typedef std::complex<DDouble>  DComplexDbl;

// list of identifiers (used in several places)
typedef std::deque<std::string>       IDList;
typedef std::deque<std::string>       StrArr;

// used by file.cpp and in other places 
typedef std::deque<DString>           FileListT;

//typedef std::valarray<SizeT>          AllIxT;

typedef std::set< DPtr>               DPtrListT;

//class ArrayIndexT;
//typedef std::vector<ArrayIndexT*> ArrayIndexVectorT;

// to resolve include conflict (declared in gdlexception.hpp)
void ThrowGDLException( const std::string& str);

// for OpenMP (defined in objects.cpp - must be declared here)
extern DLong CpuHW_NCPU;
extern DLong CpuTPOOL_NTHREADS;
extern DLong CpuTPOOL_MIN_ELTS;
extern DLong CpuTPOOL_MAX_ELTS;

// convert something to string
template <typename T>
inline std::string i2s( T i, SizeT w)// = 0)      
{
  std::ostringstream os;
  os.width(w);
  os << i;
  return os.str();
}
template <typename T>
inline std::string i2s( T i)      
{
  std::ostringstream os;
  assert( os.width() == 0);
  os << i;
  return os.str();
}

// debug 
//#include <iostream>

// searches IDList idL for std::string s, returns its position, -1 if not found
inline int FindInIDList(IDList& idL,const std::string& s)
{
//   int ix=0;
  for(IDList::iterator i=idL.begin(); i != idL.end(); ++i)//, ++ix) 
    if( *i==s) 
      {
	return i - idL.begin();
      }

  return -1;
}

// as auto_ptr is obsoleted Guard offers an alternative
template <class T>
class Guard
{
private:
  T*      guarded;
  
  Guard& operator=( Guard& r)
  {
    if( &r == this) return;
    delete guarded;
    guarded = r.guarded;
    r.guarded = NULL;
    return *this;
  }
  

public:
  Guard(): guarded( NULL)
  {}
  Guard( T* c): guarded( c)
  {}
  
  void Init( T* iniGuarded) // saves a call to delete
  {
    assert( guarded == NULL);
    guarded = iniGuarded;
  }  
  void Reset( T* newGuarded)
  {
    delete guarded;
    guarded = newGuarded;
  }  
  // for compatibiltiy with replaced auto_ptr
  void reset( T* newGuarded)
  {
    delete guarded;
    guarded = newGuarded;
  }  
  void Release()
  {
    guarded = NULL;
  }  
  T* release()
  {
    T* g = guarded;
    guarded = NULL;
    return g;
  }  
  T* Get() const
  {
    return guarded;
  }  
  // for compatibiltiy with replaced auto_ptr
  T* get() const
  {
    return guarded;
  }  
  // for compatibiltiy with replaced auto_ptr
  T* operator->() const
  {
    return guarded;
  }
  bool IsNull() const
  {
    return guarded == NULL;
  }
  
  ~Guard()
  {
    delete guarded;
  }
};

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
  void Release()
  {
    guarded = NULL;
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
  T& container;
  typename T::size_type cSize;
  
public:
  StackGuard( T& c): container( c)
  {
    cSize=container.size();
  }
  
  ~StackGuard()
  {
    for( typename T::size_type s=container.size(); s > cSize; s--)
      {
	delete container.back();
	container.pop_back();
      }
  }
};

// needed for exceptions
// does not delete elements 
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


// this data structure is optimized for list sizes < ExprListDefaultLength
// ExprListDefaultLength should be set such that it will probably never exceed
// note: it will work for larger lists as well, but then copy operations are performed
// The effect for indexed access was significant (>40%).
template< typename T, SizeT defaultLength> class PreAllocPListT
{
public:
typedef T* iterator;

private:
T* eArr;
T buf[defaultLength];
SizeT sz;
SizeT actLen;

public:
PreAllocPListT(): eArr(buf), sz(0) {}
~PreAllocPListT()
{
	T* pEnd = &eArr[sz];
	for( T* p = &eArr[0]; p!=pEnd;++p)
		delete *p;
	if( eArr != buf)
		delete[] eArr;
}
void push_back( T p)
{
	if( sz < defaultLength)
	{
		eArr[ sz++] = p;
		return;
	}
	if( sz == defaultLength)
		actLen =defaultLength; // only init here
	if( sz == actLen)
	{
// /* #ifdef CVS_VERSION
//  		only for CVS version
//   		std::cerr << "PreAllocPListT: Resize triggered ("+i2s(sz)+"). All Ok! But please report at: http://sourceforge.net/tracker/?group_id=97659&atid=618683" << std::endl;
// #endif*/
		actLen *= 2;
		T* newArr = new T[ actLen];
		for( SizeT i=0; i<sz; ++i)
			newArr[i] = eArr[i];
		if( eArr != buf)
			delete[] eArr;
		eArr = newArr;
	}
	eArr[ sz++] = p;
}
T operator[]( SizeT i) const { assert( i<sz);  return eArr[i];}
T& operator[]( SizeT i) { assert( i<sz);  return eArr[i];}
SizeT size() const { return sz;}
iterator begin()  { return &eArr[0];}
iterator end()  { return &eArr[sz];}

bool empty() const { return sz == 0;}
T& front() { return eArr[0];}
const T& front() const { return eArr[0];}
T& back() { return eArr[sz-1];}
const T& back() const { return eArr[sz-1];}
};

class BaseGDL;
const int ExprListDefaultLength = 64;
typedef PreAllocPListT<BaseGDL*, ExprListDefaultLength> ExprListT;
typedef ExprListT::iterator ExprListIterT;

// exception save usage of GSL types
// you need to pass the gsl-object to guard and the gsl-clenaup (free) function
// example usage (for gsl_matrix):
//
// gsl_matrix *matrix = gsl_matrix_alloc(p0->Dim(0), p0->Dim(0));
//
// GDLGuard< gsl_matrix> gsl_matrix_guard( matrix, gsl_matrix_free);
// (of course no explicit call to the gsl-cleanup function must be done anymore)
template< typename GSLType, typename cleanupReturnType=void>
class GDLGuard
{
  GSLType* gslObject;
  
  cleanupReturnType (*gslDestructor)(GSLType*);
  
  GDLGuard() {}
  
public:
  GDLGuard( void (*d)(GSLType*)): gslObject( NULL), gslDestructor(d) {}
  GDLGuard( GSLType* o, cleanupReturnType (*d)(GSLType*)): gslObject( o), gslDestructor(d) {}
  ~GDLGuard()
  {
    (*gslDestructor)( gslObject);
  }
  void Set( GSLType* o)
  {
    assert( gslObject == NULL);
    gslObject = o;
  }
};

// int fclose(...);
typedef GDLGuard< FILE, int> FILEGuard;

// class FILEGuard
// {
//   FILE* fp;
//   
//   FILEGuard() {}
//   
// public:
//   FILEGuard( FILE* f): fp( f) {}
//   ~FILEGuard()
//   {
//     fclose( fp);
//   }
// };

#endif
