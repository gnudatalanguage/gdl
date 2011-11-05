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


// undef for releases (should not give diagnostics)
// define for the CVS (where the default sizes can easily be adjusted)
#define GDL_CVS_VERSION
// #undef GDL_CVS_VERSION

#ifdef GDL_CVS_VERSION
#include <iostream>
#endif

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
inline std::string i2s( T i, SizeT w = 0)      
{
  std::ostringstream os;
  os.width(w);
  os << i;
  return os.str();
}

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

// #define GDLARRAY_CACHE
#undef GDLARRAY_CACHE

#define GDLARRAY_DEBUG
// #undef GDLARRAY_DEBUG

// const SizeT smallArraySize = 27;
// const SizeT maxArrayCache = 1000 * 1000; // ComplexDbl is 16 bytes

template <class T>
class GDLArray
{
private:
	enum GDLArrayConstants
	{
		smallArraySize = 27,
		maxCache = 1000 * 1000 // ComplexDbl is 16 bytes
	};
		
	typedef T Ty;

#ifdef GDLARRAY_CACHE
		
	static SizeT cacheSize;
	static T* cache;
	static T* Cached( SizeT newSize);
#endif
		
	T scalar[ smallArraySize];
	T*    buf;
	SizeT sz;

public:
  	GDLArray() throw() : buf( NULL), sz( 0) {}
  
#ifndef GDLARRAY_CACHE

  GDLArray( const GDLArray& cp) : sz( cp.size())
  {
    try {
		buf = (cp.size() > smallArraySize) ? new T[ cp.size()] : scalar;
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    std::memcpy(buf,cp.buf,sz*sizeof(T));
   
//     for( SizeT i=0; i<sz; ++i)
//       buf[ i] = cp.buf[ i];
// }
  }

  GDLArray( SizeT s, bool b) : sz( s)
  {
	  try {
		buf = (s > smallArraySize) ? new T[ s] : scalar;
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
  }
  
  GDLArray( T val, SizeT s) : sz( s)
  {
	  try {
	    buf = (s > smallArraySize) ? new T[ s] : scalar;
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    for( SizeT i=0; i<sz; ++i)
      buf[ i] = val;
// }
  }
  
  GDLArray( const T* arr, SizeT s) : sz( s)
  {
	try
	{
		buf = ( s > smallArraySize ) ? new T[ s]: scalar;
	}
	catch ( std::bad_alloc& ) { ThrowGDLException ( "Array requires more memory than available" ); }
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    
    std::memcpy(buf,arr,sz*sizeof(T));
//     for( SizeT i=0; i<sz; ++i)
//       buf[ i] = arr[ i];

// }
  }

  ~GDLArray() throw()
  {
  if( buf != scalar) 
      	delete[] buf; // buf == NULL also possible
  }
#else

  // use definition in datatypes.cpp
  GDLArray( const GDLArray& cp) ;
  GDLArray( SizeT s, bool b) ;
  GDLArray( T val, SizeT s) ;
  GDLArray( const T* arr, SizeT s) ;
  ~GDLArray() throw();

#endif // GDLARRAY_DEBUG
  


  explicit GDLArray( const T& s) throw() : /*scalar( s),*/ buf( scalar), sz( 1)
  { scalar[0] = s;}

  T& operator[]( SizeT ix) throw()
  {
   if( ix >= sz)
    assert( ix < sz);
    return buf[ ix];
  }
  const T& operator[]( SizeT ix) const throw()
  {
	assert( ix < sz);
    return buf[ ix];
  }

// private: // disable
// only used (indirect) by DStructGDL::DStructGDL(const DStructGDL& d_)
void InitFrom( const GDLArray& right )
{
// 	// assert( sz == right.size());
// 	if ( sz != right.size() )
// 		ThrowGDLException ( "GDLArray::operator= operands have not same size (this: " + i2s ( sz ) +", right: " + i2s ( right.size() ) + ")");;
	assert( &right != this);
 	assert ( sz == right.size() );
    std::memcpy(buf,right.buf,sz*sizeof(T));
}

GDLArray& operator= ( const GDLArray& right )
{
// 	if ( sz != right.size() )
// 		ThrowGDLException ( "GDLArray::operator= operands have not same size (this: " + i2s ( sz ) +", right: " + i2s ( right.size() ) + ")");;

    assert( this != &right);
	assert( sz == right.size());
//   	if ( &right != this )
       {
// 		if ( sz == right.size() )
		{
			/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
			{
			#pragma omp for*/
			for ( SizeT i=0; i<sz; ++i )
				buf[ i] = right.buf[ i];
			return *this;
// }
		}
// 		else
		{
			if ( buf != scalar )
				delete[] buf;
			sz = right.size();
			buf = ( sz>smallArraySize ) ? new T[ sz] : scalar;
			/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
			{
			#pragma omp for*/
			for ( SizeT i=0; i<sz; ++i )
				buf[ i] = right.buf[ i];
			return *this;
		}
		}
	//       }
}

  GDLArray& operator+=( const GDLArray& right) throw()
  {
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right.buf[ i];
// }
    return *this;
  }
  GDLArray& operator-=( const GDLArray& right) throw()
  {
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right.buf[ i];
// }
    return *this;
  }
//   GDLArray&operator*=( const GDLArray& right) throw()
//   {
//     for( SizeT i=0; i<sz; ++i)
//       buf[ i] *= right.buf[ i];
//     return *this;
//   }
//   GDLArray&operator/=( const GDLArray& right)
//   {
//     for( SizeT i=0; i<sz; ++i)
//       buf[ i] /= right.buf[ i]; // can be 0
//     return *this;
//   }

// private: // disable
  GDLArray& operator+=( const T& right) throw()
  {
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right;
// }
    return *this;
  }
  GDLArray& operator-=( const T& right) throw()
  {
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right;
// }
    return *this;
  }
//   GDLArray&operator*=( const T& right) throw()
//   {
//     for( SizeT i=0; i<sz; ++i)
//       buf[ i] *= right;
//     return *this;
//   }
//   GDLArray&operator/=( const T& right)
//   {
//     for( SizeT i=0; i<sz; ++i)
//       buf[ i] /= right; // can be 0
//     return *this;
//   }

  void SetBuffer( T* b) throw()
  {
    buf = b;
  }
  T* GetBuffer() throw()
  {
    return buf;
  }
  void SetBufferSize( SizeT s) throw()
  {
    sz = s;
  }

  SizeT size() const throw()
  {
    return sz;
  }

void SetSize( SizeT newSz ) // only used in DStructGDL::DStructGDL( const string& name_) (dstructgdl.cpp)
{
	assert ( sz == 0);
	if ( newSz > smallArraySize )
	{
		try
		{
			buf = new T[ newSz];
		}
		catch ( std::bad_alloc& )
		{
			ThrowGDLException ( "Array requires more memory than available" );
		}
	}
	else
	{
// default constructed instances have buf == NULL and size == 0
// make sure buf is set corectly if such instances are resized
		buf = scalar;
	}
	sz = newSz;
// 	assert ( newSz > sz );
// 	if ( newSz > smallArraySize )
// 	{
// 		try
// 		{
// 			T* newBuf = new T[ newSz];
// 			/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
// 			{
// 			#pragma omp for*/
// 			for ( SizeT i=0; i<sz; ++i )
// 				newBuf[ i] = buf[ i];
// // }
// 			if ( buf != scalar )
// 				delete[] buf;
// 			buf = newBuf;
// 		}
// 		catch ( std::bad_alloc& )
// 		{
// 			ThrowGDLException ( "Array requires more memory than available" );
// 		}
// 	}
// 	else
// 	{
// // default constructed instances have buf == NULL and size == 0
// // make sure buf is set corectly if such instances are resized
// 		buf = scalar;
// 	}
// 	sz = newSz;
}

//   T min() const
//   {
//     T res = buf[ 0];
//     for( SizeT i=1; i<sz; ++i)
//       if( res > buf[ i]) res = buf[ i];
//     return res;
//   }
//   T max() const
//   {
//     T res = buf[ 0];
//     for( SizeT i=1; i<sz; ++i)
//       if( res < buf[ i]) res = buf[ i];
//     return res;
//   }
}; // GDLArray

template<>
inline void GDLArray<DString>::InitFrom( const GDLArray& right )
{
	assert( &right != this);
 	assert ( sz == right.size() );
	for ( SizeT i=0; i<sz; ++i )
		buf[ i] = right.buf[ i];
}

template<>
inline GDLArray<DString>::GDLArray( const GDLArray& cp) : sz( cp.size())
  {
    try {
		buf = (cp.size() > smallArraySize) ? new Ty[ cp.size()] : scalar;
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
     for( SizeT i=0; i<sz; ++i)
       buf[ i] = cp.buf[ i];
// }
  }

template<>
inline GDLArray<DString>::GDLArray( const Ty* arr, SizeT s) : sz( s)
  {
    try {
    buf = (s > smallArraySize) ? new Ty[ s]: scalar;
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
/*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
{
#pragma omp for*/
     for( SizeT i=0; i<sz; ++i)
       buf[ i] = arr[ i];
// }
  }

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


#endif
