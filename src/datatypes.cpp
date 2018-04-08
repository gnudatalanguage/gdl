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
#include <numpy/arrayobject.h>
#endif

#include <iomanip>

//#include "datatypes.hpp" // included from arrayindex.hpp
#include "nullgdl.hpp"
#include "dstructgdl.hpp"
#include "arrayindexlistt.hpp"
#include "assocdata.hpp"
#include "io.hpp"
#include "dinterpreter.hpp"
#include "terminfo.hpp"

// needed with gcc-3.3.2
#include <cassert>

// on OS X isnan is not defined
#if defined(__APPLE__) && defined(OLD_DARWIN) && !defined(isnan)

#ifdef __cplusplus
extern "C" {
#endif
// #define      isnan( x )         ( ( sizeof ( x ) == sizeof(double) ) ?  \
// 				  __isnand ( x ) :			\
// 				  ( sizeof ( x ) == sizeof( float) ) ?	\
// 				  __isnanf ( x ) :			\
// 				  __isnan  ( x ) )
namespace std {

  template <typename T>
  bool isnan( T x) { return ( ( sizeof ( x ) == sizeof(double) ) ?  
				  __isnand ( x ) :			
				  ( sizeof ( x ) == sizeof( float) ) ?	
				  __isnanf ( x ) :			
				  __isnan  ( x ) );}
}
#ifdef __cplusplus
}
#endif
#endif

#ifdef _MSC_VER
#define isfinite _finite
#define std__isnan isnan
#else
#define std__isnan std::isnan
#endif

//using namespace std;
//using std::isnan;

// this (ugly) including of other sourcefiles has to be done, because
// on Mac OS X a template instantiation request (see bottom of file)
// can only be done once
#define INCLUDE_GETAS_CPP 1
#include "getas.cpp"

#define INCLUDE_BASIC_OP_CPP 1
#include "basic_op.cpp"
#include "basic_op_new.cpp"

#define INCLUDE_DEFAULT_IO_CPP 1
#include "default_io.cpp"

#define INCLUDE_IFMT_CPP 1
#include "ifmt.cpp"

#define INCLUDE_OFMT_CPP 1
#include "ofmt.cpp"

#define INCLUDE_DATATYPESREF_CPP 1
#include "datatypesref.cpp"

#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

#  define INCLUDE_TOPYTHON_CPP 1
#  include "topython.cpp"

#  define INCLUDE_GDLPYTHON_CPP 1
#  include "gdlpython.cpp"

#  ifdef PYTHON_MODULE
#    define INCLUDE_PYTHONGDL_CPP 1
#    include "pythongdl.cpp"
#  endif
#endif

#ifdef _MSC_VER
#define isnan _isnan
#define isinfinite _isinfinite
#endif


#ifdef TESTTG

#include "test_template_grouping.cpp"
template<class Sp>
void Data_<Sp>::TestTemplateGrouping()              
{ 
//   Ty ty = Test1();
  bool b = Test2();
}

#endif

template<class Sp>
FreeListT Data_<Sp>::freeList;

#ifdef GDLARRAY_CACHE

#ifdef GDLARRAY_DEBUG

inline void TraceCache( SizeT& cacheSize, SizeT sz, bool cacheIsNull, SizeT smallArraySize)
{
  // 	if( cacheSize > smallArraySize && cacheSize == sz  && !cacheIsNull)
  // 			std::cout << "+++ CACHE HIT\tID: ("  << &cacheSize  << ")   sz: " << cacheSize << std::endl;
  // 	else
  if( sz > smallArraySize)
    std::cout << "+ New\t\tID: ("  << &cacheSize  << ")   sz: " << sz << "   cache size: " <<  cacheSize <<std::endl;
}
		
#else

#define TraceCache( a, b, c, d)		

#endif

template<class Sp>
SizeT GDLArray<Sp>::cacheSize = 0;

template<>
SizeT GDLArray<char>::cacheSize = 0;

template<class Sp>
typename	GDLArray<Sp>::Ty* GDLArray<Sp>::cache = NULL;

template<class Sp>
typename GDLArray<Sp>::Ty* GDLArray<Sp>::Cached( SizeT newSize)
{
  assert( newSize > smallArraySize);
  if( cache != NULL && cacheSize == newSize)
    {
#ifdef GDLARRAY_DEBUG
      std::cout << "*** CACHE HIT\tID: ("  << &cacheSize << ")   sz: " << cacheSize << "   ***" << std::endl;
#endif
      Ty* hit = cache;
      cache = NULL;
      return hit;
    }
  return new Ty[ newSize];
}

template<class Sp>GDLArray<Sp>::GDLArray( const GDLArray<Sp>& cp) : sz( cp.size())
{
  TraceCache( cacheSize, sz, cache==NULL, smallArraySize);
	  
  try {
    buf = (sz > smallArraySize) ? Cached( sz) /* new Ty[ cp.size()]*/  : scalar;
  } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
	  
  std::memcpy(buf,cp.buf,sz*sizeof(Ty));
}

template<class Sp>GDLArray<Sp>::	GDLArray( SizeT s, bool b) : sz( s)
{
  TraceCache( cacheSize, sz, cache==NULL, smallArraySize);

  try {
    buf = (sz > smallArraySize) ? Cached( sz) /* new Ty[ sz]*/  : scalar;
  } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
}

template<class Sp>GDLArray<Sp>::	GDLArray( Ty val, SizeT s) : sz( s)
{
  TraceCache( cacheSize, sz, cache==NULL, smallArraySize);

  try {
    buf = (sz > smallArraySize) ? Cached( sz) /* new Ty[ sz]*/  : scalar;
  } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
  for( SizeT i=0; i<sz; ++i)
    buf[ i] = val;
}

template<class Sp>GDLArray<Sp>::	GDLArray( const Ty* arr, SizeT s) : sz( s)
{
  TraceCache( cacheSize, sz, cache==NULL, smallArraySize);
	  
  try {
    buf = (sz > smallArraySize) ? Cached( sz) /* new Ty[ sz]*/  : scalar;
  } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
  std::memcpy(buf,arr,sz*sizeof(Ty));
}

template<class Sp>GDLArray<Sp>::~GDLArray() throw()
{
#ifdef GDLARRAY_DEBUG
  if( buf == cache)
    std::cout << "~~~ recycled cache\tID: ("  << &cacheSize << ")   sz: " << sz << "\tcacheSize: " << cacheSize << std::endl;
#endif			

  assert( buf != cache || sz == cacheSize);
	
  if ( buf != NULL && buf != scalar &&
       buf != cache // note: assumes cacheSize never changes for a given cache
       )
    {		
      assert( sz > smallArraySize);
      if ( sz <= maxCache )
	{
			
#ifdef GDLARRAY_DEBUG
	  std::cout << "--- free cache\tID: ("  << &cacheSize << ")   sz: " << cacheSize << "\tnew: " << sz << std::endl;
#endif			
	  delete cache;
	  cache = buf;
	  cacheSize = sz;
	}
      else
	{
	  delete[] buf;
	}
    }
}
// as strings may occupy arbitrary memory (regardless of the array size), better not cache them
// note: Structs are ok, since GDL cleans up the strings they may contain and uses GDLArray as raw memory
template<>
GDLArray<DString>::~GDLArray() throw()
{
  if ( buf != scalar )
    {
      delete[] buf;
    }
}


template class GDLArray<char>;
  
#endif

template<typename T>
inline bool gdlValid( const T &value )
{
    T max_value = std::numeric_limits<T>::max();
    T min_value = - max_value;
    return ( ( min_value <= value && value <= max_value ) &&  (value == value));
}
inline bool gdlValid( const DComplex &value )
{
    DFloat max_value = std::numeric_limits<DFloat>::max();
    DFloat min_value = - max_value;
    return ( ( min_value <= value.real() && value.real() <= max_value ) &&  (value.real() == value.real()))&&
            ( ( min_value <= value.imag() && value.imag() <= max_value ) && (value.imag() == value.imag()));
}
inline bool gdlValid( const DComplexDbl &value )
{
    DDouble max_value = std::numeric_limits<DDouble>::max();
    DDouble min_value = - max_value;
    return ( ( min_value <= value.real() && value.real() <= max_value ) &&  (value.real() == value.real()))&&
            ( ( min_value <= value.imag() && value.imag() <= max_value ) &&  (value.imag() == value.imag()));
}



template<class Sp> void* Data_<Sp>::operator new( size_t bytes)
{
  assert( bytes == sizeof( Data_));

  if( freeList.size() > 0)
    {
      return freeList.pop_back();
//       void* res = freeList.back();
//       freeList.pop_back();
//       return res;	
    }

  const size_t newSize = multiAlloc - 1;

  static long callCount = 0;
  ++callCount;
  
  // reserve space for all instances
  // note that reserve must do an allocation
  // this hack divides the number of those allocation
  // (for the cost of initially larger allocation - but only for pointers)
  const long allocDivider = 4;
  freeList.reserve( ((callCount/allocDivider+1)*allocDivider-1)*multiAlloc);

  // resize to what is needed now
//   freeList.resize( newSize);

#ifdef USE_EIGEN  
  // we need this allocation here as well (as in typedefs.hpp), because GDLArray needs to be aligned
  const int alignmentInBytes = 16; // set to multiple of 16 >= sizeof( char*)
  const size_t realSizeOfType = sizeof( Data_);
  const SizeT exceed = realSizeOfType % alignmentInBytes;
  const size_t sizeOfType = realSizeOfType + (alignmentInBytes - exceed);
  char* res = static_cast< char*>( Eigen::internal::aligned_malloc( sizeOfType * multiAlloc)); // one more than newSize
#else
  const size_t sizeOfType = sizeof( Data_);
  char* res = static_cast< char*>( malloc( sizeOfType * multiAlloc)); // one more than newSize
#endif
  
  res = freeList.Init( newSize, res, sizeOfType);
//   freeList[0] = NULL;
//   for( size_t i=1; i<=newSize; ++i)
//     {
//       freeList[ i] = res;
//       res += sizeOfType;
//     } 

  // the one more
  return res;
}

template<class Sp> void Data_<Sp>::operator delete( void *ptr)
{
  freeList.push_back( ptr);
}



// destructor
template<class Sp> Data_<Sp>::~Data_() {}
template<> Data_<SpDPtr>::~Data_()
{
  if( this->dd.GetBuffer() != NULL)
    GDLInterpreter::DecRef( this);
}
template<> Data_<SpDObj>::~Data_()
{
  if( this->dd.GetBuffer() != NULL)
    GDLInterpreter::DecRefObj( this);
}

// default
template<class Sp> Data_<Sp>::Data_(): Sp(), dd() {}

// scalar
template<class Sp> Data_<Sp>::Data_(const Ty& d_): Sp(), dd(d_)
{}
// template<> Data_<SpDPtr>::Data_(const Ty& d_): SpDPtr(), dd(d_)
// {GDLInterpreter::IncRef(d_);}
// template<> Data_<SpDObj>::Data_(const Ty& d_): SpDObj(), dd(d_)
// {GDLInterpreter::IncRefObj(d_);}

// new array, zero fields
template<class Sp> Data_<Sp>::Data_(const dimension& dim_): 
  Sp( dim_), dd( Sp::zero, this->dim.NDimElements())
{
  this->dim.Purge();
}

// new one-dim array from Ty*
template<class Sp> Data_<Sp>::Data_( const Ty* p, const SizeT nEl): 
  Sp( dimension( nEl)), dd( p, nEl)
{}
template<> Data_<SpDPtr>::Data_( const Ty* p, const SizeT nEl):
  SpDPtr( dimension( nEl)), dd( p, nEl)
{GDLInterpreter::IncRef(this);}
template<> Data_<SpDObj>::Data_( const Ty* p, const SizeT nEl):
  SpDObj( dimension( nEl)), dd( p, nEl)
{GDLInterpreter::IncRefObj(this);}

// c-i 
// template<class Sp> Data_<Sp>::Data_(const Data_& d_): 
// Sp(d_.dim), dd(d_.dd) {}

template<class Sp> Data_<Sp>::Data_(const dimension& dim_, BaseGDL::InitType iT):
  Sp( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{
  this->dim.Purge();

  if( iT == BaseGDL::INDGEN)
    {
      SizeT sz=dd.size();
      //       Ty val=Sp::zero;
      // #pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
      {
	// #pragma omp for
	for( SizeT i=0; i<sz; i++)
	  {
	    (*this)[i]=i;//val;
	  }
	// 	  val += 1; // no increment operator for floats
      }
  }
  if (iT == BaseGDL::ZERO) {
    SizeT sz = dd.size();
#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    {
#pragma omp for
      for (int i = 0; i < sz; ++i) {
        (*this)[i] = 0;
      }
    }
  }
}
/*// INDGEN seems to be more precise for large arrays
  template<> Data_<SpDFloat>::Data_(const dimension& dim_,
  BaseGDL::InitType iT): 
  SpDFloat( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.N_Elements(), false)
  {
  this->dim.Purge();

  if( iT == BaseGDL::INDGEN)
  {
  SizeT sz=dd.size();

  for( SizeT i=0; i<sz; ++i)
  {
  (*this)[i] = i;
  }
  }
  }
  template<> Data_<SpDComplex>::Data_(const dimension& dim_,
  BaseGDL::InitType iT): 
  SpDComplex( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.N_Elements(), false)
  {
  this->dim.Purge();

  if( iT == BaseGDL::INDGEN)
  {
  SizeT sz=dd.size();

  for( SizeT i=0; i<sz; ++i)
  {
  (*this)[i] = i;
  }
  }
  }
  template<> Data_<SpDDouble>::Data_(const dimension& dim_,
  BaseGDL::InitType iT): 
  SpDDouble( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.N_Elements(), false)
  {
  this->dim.Purge();

  if( iT == BaseGDL::INDGEN)
  {
  SizeT sz=dd.size();

  for( SizeT i=0; i<sz; ++i)
  {
  (*this)[i] = i;
  }
  }
  }
  template<> Data_<SpDComplexDbl>::Data_(const dimension& dim_,
  BaseGDL::InitType iT): 
  SpDComplexDbl( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.N_Elements(), false)
  {
  this->dim.Purge();

  if( iT == BaseGDL::INDGEN)
  {
  SizeT sz=dd.size();

  for( SizeT i=0; i<sz; ++i)
  {
  (*this)[i] = i;
  }
  }
  }*/
// string, ptr, obj (cannot be INDGEN, 
// need not to be zeroed if all intialized later)
// struct (as a separate class) as well
template<> Data_<SpDString>::Data_(const dimension& dim_, BaseGDL::InitType iT):
  SpDString(dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{
  dim.Purge();
  
  if( iT == BaseGDL::INDGEN)
    throw GDLException("DStringGDL(dim,InitType=INDGEN) called.");
}
template<> Data_<SpDPtr>::Data_(const dimension& dim_,  BaseGDL::InitType iT):
  SpDPtr(dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{
  dim.Purge();
  
  if( iT == BaseGDL::INDGEN)
    throw GDLException("DPtrGDL(dim,InitType=INDGEN) called.");

  if( iT != BaseGDL::NOALLOC && iT != BaseGDL::NOZERO)
    {
      SizeT sz = dd.size();
      /*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
	{
	#pragma omp for*/
      for( int i=0; i<sz; ++i)
	{
	  (*this)[i]=0;
	}
      // 	  val += 1; // no increment operator for floats
      // 	}
    }
}
template<> Data_<SpDObj>::Data_(const dimension& dim_, BaseGDL::InitType iT):
  SpDObj(dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{
  dim.Purge();

  if( iT == BaseGDL::INDGEN)
    throw GDLException("DObjGDL(dim,InitType=INDGEN) called.");

  if( iT != BaseGDL::NOALLOC && iT != BaseGDL::NOZERO)
    {
      SizeT sz = dd.size();
      /*#pragma omp parallel if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
	{
	#pragma omp for*/
      for( int i=0; i<sz; i++)
	{
	  (*this)[i]=0;
	}
      // 	  val += 1; // no increment operator for floats
      // 	}
    }
}

// c-i
template<class Sp>
Data_<Sp>::Data_(const Data_& d_): Sp(d_.dim), dd(d_.dd) {}
template<>
Data_<SpDPtr>::Data_(const Data_& d_): SpDPtr(d_.dim), dd(d_.dd)
{
  GDLInterpreter::IncRef( this);
}
template<>
Data_<SpDObj>::Data_(const Data_& d_): SpDObj(d_.dim), dd(d_.dd)
{
  GDLInterpreter::IncRefObj( this);
}


template<class Sp>
Data_<Sp>* Data_<Sp>::Dup() const { return new Data_(*this);}

// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::Dup() const
//   {
//   Data_<SpDPtr>* p =new Data_(*this);
//   GDLInterpreter::IncRef( p);
//   return p;
//   }
// template<>
//   Data_<SpDObj>* Data_<SpDObj>::Dup() const
//   {
//   Data_<SpDObj>* p =new Data_(*this);
//   GDLInterpreter::IncRefObj( p);
//   return p;
//   }


  
template<class Sp>
BaseGDL* Data_<Sp>::Log()              
{ 
  DFloatGDL* res = static_cast<DFloatGDL*>
    (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->LogThis();
  return res;
}
template<>
BaseGDL* Data_<SpDFloat>::Log()              
{ 
  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i<nEl; ++i)
	(*n)[ i] = log( (*this)[ i]);
    }
  return n;
}
template<>
BaseGDL* Data_<SpDDouble>::Log()              
{ 
  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log( (*this)[ i]);
  return n;
}
template<>
BaseGDL* Data_<SpDComplex>::Log()              
{ 
  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log( (*this)[ i]);
  return n;
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Log()              
{ 
  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log( (*this)[ i]);
  return n;
}

// this is actually not a "log" of "this",
// but the behaviour is fine with the usage in the library function
// the real LogThis is done in the specializations for floating and 
// complex types
template<class Sp>
BaseGDL* Data_<Sp>::LogThis()              
{ 
  DFloatGDL* res = static_cast<DFloatGDL*>
    (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->LogThis(); // calls correct LogThis for float
  return res;
}
template<>
BaseGDL* Data_<SpDFloat>::LogThis()              
{ 
  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log( (*this)[ i]);
  return this;
}
template<>
BaseGDL* Data_<SpDDouble>::LogThis()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log( (*this)[ i]);
  /*#else
    dd = log(dd);
    #endif*/
  return this;
}
template<>
BaseGDL* Data_<SpDComplex>::LogThis()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log( (*this)[ i]);
  /*#else
    dd = log(dd);
    #endif*/
  return this;
}
template<>
BaseGDL* Data_<SpDComplexDbl>::LogThis()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log( (*this)[ i]);
  /*#else
    dd = log(dd);
    #endif*/
  return this;
}

template<class Sp>
BaseGDL* Data_<Sp>::Log10()              
{ 
  DFloatGDL* res = static_cast<DFloatGDL*>
    (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->Log10This();
  return res;
}
template<>
BaseGDL* Data_<SpDFloat>::Log10()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log10( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log10( (*this)[ i]);
  return n;
  /*#else
    return new Data_(this->dim, log10(dd));
    #endif*/
}
template<>
BaseGDL* Data_<SpDDouble>::Log10()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log10( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log10( (*this)[ i]);
  return n;
  /*#else
    return new Data_(this->dim, log10(dd));
    #endif*/
}
template<>
BaseGDL* Data_<SpDComplex>::Log10()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log10( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log10( (*this)[ i]);
  return n;
  /*#else
    return new Data_(this->dim, log10(dd));
    #endif*/
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Log10()              
{ 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  Data_* n = this->New( this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if( nEl == 1)
    {
      (*n)[ 0] = log10( (*this)[ 0]);
      return n;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*n)[ i] = log10( (*this)[ i]);
  return n;
  /*#else
    return new Data_(this->dim, log10(dd));
    #endif*/
}

// see comment at void Data_<Sp>::LogThis()              
template<class Sp>
BaseGDL* Data_<Sp>::Log10This()              
{ 
  DFloatGDL* res = static_cast<DFloatGDL*>
    (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->Log10This(); // calls correct Log10This for float
  return res;
}
template<>
BaseGDL* Data_<SpDFloat>::Log10This()              
{ 
#if 1 || (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log10( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log10( (*this)[ i]);
#else
  dd = log10(dd);
#endif
  return this;
}
template<>
BaseGDL* Data_<SpDDouble>::Log10This()              
{ 
#if 1 || (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log10( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log10( (*this)[ i]);
#else
  dd = log10(dd);
#endif
  return this;
}
template<>
BaseGDL* Data_<SpDComplex>::Log10This()              
{ 
#if 1 || (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log10( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log10( (*this)[ i]);
#else
  dd = log10(dd);
#endif
  return this;
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Log10This()              
{ 
#if 1 || (__GNUC__ == 3) && (__GNUC_MINOR__ == 2) //&& (__GNUC_PATCHLEVEL__ == 2)

  SizeT nEl = N_Elements();
  if( nEl == 1)
    {
      (*this)[ 0] = log10( (*this)[ 0]);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel for if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    for( int i=0; i<nEl; ++i)
      (*this)[ i] = log10( (*this)[ i]);
#else
  dd = log10(dd);
#endif
  return this;
}



// template<class Sp>
// BaseGDL* Data_<Sp>::Abs() const
// {
//   return new Data_( this->dim, dd.abs());
// }

template<class Sp>
inline bool Data_<Sp>::Greater(SizeT i1, SizeT i2) const
{ return ((*this)[i1] > (*this)[i2]);}

template<>
inline bool Data_<SpDComplex>::Greater(SizeT i1, SizeT i2) const
{ return (abs((*this)[i1]) > abs((*this)[i2]));}
template<>
inline bool Data_<SpDComplexDbl>::Greater(SizeT i1, SizeT i2) const
{ return (abs((*this)[i1]) > abs((*this)[i2]));}


template<class Sp>
inline bool Data_<Sp>::Equal(SizeT i1, SizeT i2) const
{ return ((*this)[i1] == (*this)[i2]);}



// calculates the shift to be applied to the destination index
inline SizeT CShiftNormalize( DLong s, SizeT this_dim)
{
  if ( s >= 0 )
    return s % this_dim;
  // s < 0
//  long dstIx = -(-s % this_dim);
  long dstIx = -s % this_dim;
  dstIx = -dstIx;
  if( dstIx == 0) // if this_dim == 1
    return 0;
  assert( dstIx + this_dim > 0);
  return dstIx + this_dim;
}

template<class Sp>
BaseGDL* Data_<Sp>::CShift( DLong d) const
{
  SizeT nEl = dd.size();
  SizeT shift = CShiftNormalize( d, nEl);

  if( shift == 0)
    return this->Dup();

  Data_* sh = new Data_( this->dim, BaseGDL::NOZERO);

  SizeT firstChunk = nEl - shift;

  memcpy( &sh->dd[ shift], &dd[0], firstChunk * sizeof(Ty));
  memcpy( &sh->dd[ 0], &dd[firstChunk], shift * sizeof(Ty));
	
  return sh;
}

template<>
BaseGDL* Data_<SpDString>::CShift( DLong d) const
{
  SizeT nEl = dd.size();
  SizeT shift = CShiftNormalize( d, nEl);

  if( shift == 0)
    return this->Dup();

  Data_* sh = new Data_( this->dim, BaseGDL::NOZERO);

  SizeT firstChunk = nEl - shift;

  SizeT i=0;
  for( ; i<firstChunk; ++i)
    sh->dd[shift++] = dd[ i];

  shift = 0;

  for( ; i<nEl; ++i)
    sh->dd[shift++] = dd[ i];

  return sh;
}
template<>
BaseGDL* Data_<SpDPtr>::CShift( DLong d) const
{
  SizeT nEl = dd.size();
  SizeT shift = CShiftNormalize( d, nEl);

  if( shift == 0)
    return this->Dup(); // does IncRef

  Data_* sh = new Data_( this->dim, BaseGDL::NOZERO);

  SizeT firstChunk = nEl - shift;

  SizeT i=0;
  for( ; i<firstChunk; ++i)
    sh->dd[shift++] = dd[ i];

  shift = 0;

  for( ; i<nEl; ++i)
    sh->dd[shift++] = dd[ i];

  GDLInterpreter::IncRef( sh);
  return sh;
}
template<>
BaseGDL* Data_<SpDObj>::CShift( DLong d) const
{
  SizeT nEl = dd.size();
  SizeT shift = CShiftNormalize( d, nEl);

  if( shift == 0)
    return this->Dup(); // does IncRefObj

  Data_* sh = new Data_( this->dim, BaseGDL::NOZERO);

  SizeT firstChunk = nEl - shift;

  SizeT i=0;
  for( ; i<firstChunk; ++i)
    sh->dd[shift++] = dd[ i];

  shift = 0;

  for( ; i<nEl; ++i)
    sh->dd[shift++] = dd[ i];

  GDLInterpreter::IncRefObj( sh);
  return sh;
}

template<typename Ty>
inline void CShift1( Ty* dst, SizeT& dstLonIx, const Ty* src, SizeT& srcLonIx,
		     SizeT stride_1, SizeT chunk0, SizeT chunk1)
{
  memcpy(  &dst[ dstLonIx], &src[ srcLonIx], chunk0 * sizeof(Ty));
  dstLonIx += chunk0;
  srcLonIx += chunk0;

  dstLonIx -= stride_1;

  memcpy( &dst[ dstLonIx], &src[ srcLonIx], chunk1 * sizeof(Ty));
  dstLonIx += chunk1 ;
  srcLonIx += chunk1;

  dstLonIx += stride_1;
}

#undef TEST_GOOD_OL_VERSION

template<class Sp>
BaseGDL* Data_<Sp>::CShift( DLong s[ MAXRANK]) const {
  Data_* sh = new Data_(this->dim, BaseGDL::NOZERO);

  SizeT nDim = this->Rank();
  SizeT nEl = N_Elements();

  SizeT stride[ MAXRANK + 1];
  this->dim.Stride(stride, nDim);

  long srcIx[ MAXRANK + 1];
  long dstIx[ MAXRANK + 1];
  SizeT this_dim[ MAXRANK];

  const Ty* ddP = &(*this)[0];
  Ty* shP = &(*sh)[0];

  if (nDim == 2) {
    this_dim[ 0] = this->dim[ 0];
    this_dim[ 1] = this->dim[ 1];
    dstIx[ 0] = CShiftNormalize(s[ 0], this_dim[ 0]);
    dstIx[ 1] = CShiftNormalize(s[ 1], this_dim[ 1]);
    SizeT dstLonIx = dstIx[ 0] + dstIx[ 1] * stride[ 1];
    SizeT freeDstIx_0 = this_dim[ 0] - dstIx[ 0];
    SizeT freeDstIx_1 = this_dim[ 1] - dstIx[ 1];

    if (Sp::t != GDL_STRING) // strings are not POD all others are
    {
      SizeT srcLonIx = 0;
      SizeT t = 0;

      for (; t < freeDstIx_1; ++t) {
        CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
      }
      dstLonIx -= stride[ 2];
      for (; t < this_dim[ 1]; ++t) {
        CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
      }
    } else // Sp::t == GDL_STRING
    {
      SizeT a = 0;
      SizeT t = 0;
      for (; t < freeDstIx_1; ++t) {
        SizeT s = 0;
        for (; s < freeDstIx_0; ++s) {
          shP[ dstLonIx++] = ddP[ a++];
        }
        dstLonIx -= stride[ 1];
        for (; s < this_dim[ 0]; ++s) {
          shP[ dstLonIx++] = ddP[ a++];
        }
        dstLonIx += stride[ 1];
      }
      dstLonIx -= stride[ 2];
      for (; t < this_dim[ 1]; ++t) {
        SizeT s = 0;
        for (; s < freeDstIx_0; ++s) {
          shP[ dstLonIx++] = ddP[ a++];
        }
        dstLonIx -= stride[ 1];
        for (; s < this_dim[ 0]; ++s) {
          shP[ dstLonIx++] = ddP[ a++];
        }
        dstLonIx += stride[ 1];
      }
      //	dstLonIx += stride[ 2];
      assert(a == nEl);
    } // if( Sp::t != GDL_STRING) else

    return sh;
  }

  assert(nDim > 2);

#ifndef TEST_GOOD_OL_VERSION

  for (SizeT aSp = 0; aSp < nDim; ++aSp) {
    this_dim[ aSp] = this->dim[ aSp];
    srcIx[ aSp] = 0;
    dstIx[ aSp] = CShiftNormalize(s[ aSp], this_dim[ aSp]);
    //       dim_stride[ aSp] = this_dim[ aSp] * stride[ aSp];
  }
  //   srcIx[ nDim] = dstIx[ nDim] = 0;
  SizeT dstLonIx = dstIx[ 0];
  for (SizeT rSp = 1; rSp < nDim; ++rSp)
    dstLonIx += dstIx[ rSp] * stride[ rSp];

  if (Sp::t != GDL_STRING) {
    if (nDim == 3) {
      SizeT freeDstIx_0 = this_dim[ 0] - dstIx[ 0];
      SizeT freeDstIx_1 = this_dim[ 1] - dstIx[ 1];
      SizeT freeDstIx_2 = this_dim[ 2] - dstIx[ 2];

      SizeT srcLonIx = 0;
      SizeT d2 = 0;
      for (; d2 < freeDstIx_2; ++d2) {
        SizeT d1 = 0;
        for (; d1 < freeDstIx_1; ++d1) {
          CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
        }
        dstLonIx -= stride[ 2];
        for (; d1 < this_dim[ 1]; ++d1) {
          CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
        }
        dstLonIx += stride[ 2];
      }
      dstLonIx -= stride[ 3];
      for (; d2 < this_dim[ 2]; ++d2) {
        SizeT d1 = 0;
        for (; d1 < freeDstIx_1; ++d1) {
          CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
        }
        dstLonIx -= stride[ 2];
        for (; d1 < this_dim[ 1]; ++d1) {
          CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
        }
        dstLonIx += stride[ 2];
      }
      assert(srcLonIx == nEl);
      return sh;
    } // nDim == 3
    if (nDim == 4) {
      SizeT freeDstIx_0 = this_dim[ 0] - dstIx[ 0];
      SizeT freeDstIx_1 = this_dim[ 1] - dstIx[ 1];
      SizeT freeDstIx_2 = this_dim[ 2] - dstIx[ 2];
      SizeT freeDstIx_3 = this_dim[ 3] - dstIx[ 3];

      SizeT srcLonIx = 0;

      SizeT d3 = 0;
      for (; d3 < freeDstIx_3; ++d3) {
        SizeT d2 = 0;
        for (; d2 < freeDstIx_2; ++d2) {
          SizeT d1 = 0;
          for (; d1 < freeDstIx_1; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx -= stride[ 2];
          for (; d1 < this_dim[ 1]; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx += stride[ 2];
        }
        dstLonIx -= stride[ 3];
        for (; d2 < this_dim[ 2]; ++d2) {
          SizeT d1 = 0;
          for (; d1 < freeDstIx_1; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx -= stride[ 2];
          for (; d1 < this_dim[ 1]; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx += stride[ 2];
        }
        dstLonIx += stride[ 3];
      }
      dstLonIx -= stride[ 4];
      for (; d3 < this_dim[ 3]; ++d3) {
        SizeT d2 = 0;
        for (; d2 < freeDstIx_2; ++d2) {
          SizeT d1 = 0;
          for (; d1 < freeDstIx_1; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx -= stride[ 2];
          for (; d1 < this_dim[ 1]; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx += stride[ 2];
        }
        dstLonIx -= stride[ 3];
        for (; d2 < this_dim[ 2]; ++d2) {
          SizeT d1 = 0;
          for (; d1 < freeDstIx_1; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx -= stride[ 2];
          for (; d1 < this_dim[ 1]; ++d1)
            CShift1<Ty>(shP, dstLonIx, ddP, srcLonIx, stride[ 1], freeDstIx_0, dstIx[0]);
          dstLonIx += stride[ 2];
        }
        dstLonIx += stride[ 3];
      }
      assert(srcLonIx == nEl);
      return sh;
    } // nDim == 4
  } // if( Sp::t != GDL_STRING)

#else

  // need to be done earlier within the TEST_GOOD_OL_VERSION section
  for (SizeT aSp = 0; aSp < nDim; ++aSp) {
    this_dim[ aSp] = this->dim[ aSp];
    srcIx[ aSp] = 0;
    dstIx[ aSp] = CShiftNormalize(s[ aSp], this_dim[ aSp]);
  }
  SizeT dstLonIx = dstIx[ 0];
  for (SizeT rSp = 1; rSp < nDim; ++rSp)
    dstLonIx += dstIx[ rSp] * stride[ rSp];

#endif // TEST_GOOD_OL_VERSION

  // good 'ol version RELOADED
  SizeT* dim_stride = &stride[1];
  SizeT freeDstIx_0 = this_dim[ 0] - dstIx[ 0]; // how many elements till array border is reached (dim 0)
  //   for( SizeT a=0; a<nEl; ++srcIx[0],++dstIx[0])
  for (SizeT a = 0; a < nEl; ++srcIx[1], ++dstIx[1]) {
    //     for( SizeT aSp=0; aSp<nDim;)
    for (SizeT aSp = 1; aSp < nDim;) {
      if (dstIx[ aSp] >= this_dim[ aSp]) {
        // dstIx[ aSp] -= dim[ aSp];
        dstIx[ aSp] = 0;
        dstLonIx -= dim_stride[ aSp];
      }
      if (srcIx[ aSp] < this_dim[ aSp]) break;

      srcIx[ aSp] = 0;
      if (++aSp >= nDim) break; // ??


      ++srcIx[ aSp];
      ++dstIx[ aSp];
      dstLonIx += stride[ aSp];
    }

    // code from new version (to avoid the worst :-)
    // copy one line
    SizeT s = 0;
    for (; s < freeDstIx_0; ++s) {
      shP[ dstLonIx++] = ddP[ a++];
    }
    dstLonIx -= stride[ 1];
    for (; s < this_dim[ 0]; ++s) {
      shP[ dstLonIx++] = ddP[ a++];
    }
    dstLonIx += stride[ 1];

    // copy one element
    //shP[ dstLonIx++] = ddP[ a++];
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
BaseGDL* Data_<Sp>::Transpose( DUInt* perm) {
  SizeT rank = this->Rank();

  if (rank == 1) // special case: vector
  {
    if (perm != NULL) // must be [0]
    {
      return Dup();
    } else {
      Data_* res = Dup();
      res->dim >> 1;
      return res;
    }
  }

  // 2 - MAXRANK
  static DUInt* permDefault = InitPermDefault();
  if (perm == NULL) {
    
// following 2D code is now slower than multi-dim multicore solution below. 
//    if (rank == 2) {
//      SizeT srcDim0 = this->dim[0];
//      SizeT srcDim1 = this->dim[1];
//      Data_* res = new Data_(dimension(srcDim1, srcDim0), BaseGDL::NOZERO);
//
//      SizeT srcIx = 0;
//      for (SizeT srcIx1 = 0; srcIx1 < srcDim1; ++srcIx1) // src dim 1
//      {
//        SizeT resIx = srcIx1;
//        SizeT srcLim = srcIx + srcDim0; // src dim 0
//        for (; srcIx < srcLim; ++srcIx) {
//          (*res)[ resIx] = (*this)[ srcIx];
//          resIx += srcDim1;
//        }
//      }
//      return res;
//    }

    // perm == NULL, rank != 2
    perm = &permDefault[ MAXRANK - rank];
  }

  //new version, parallell the job on a multithreaded machine. Gain is 3/4 number of threads.  
  SizeT resDim[ MAXRANK]; // permutated!
  for (SizeT d = 0; d < rank; ++d) {
    resDim[ d] = this->dim[ perm[ d]];
  }

  Data_* res = new Data_(dimension(resDim, rank), BaseGDL::NOZERO);

  // src stride
  SizeT srcStride[ MAXRANK+1];
  this->dim.Stride(srcStride, rank);

  SizeT nElem = dd.size();
  long chunksize = nElem;
  long nchunk = 1;
  if (nElem > CpuTPOOL_MIN_ELTS) { //no use start parallel threading for small numbers.
    chunksize = nElem / ((CpuTPOOL_NTHREADS > 32) ? 32 : CpuTPOOL_NTHREADS);
    nchunk = nElem / chunksize;
    if (chunksize * nchunk < nElem) ++nchunk;
  } else {
    nchunk = 1;
    chunksize = nElem;
  }
  //compute start parameter for each multiWalk chunks:
  // pool of accelerators
  SizeT srcDimPool[nchunk][MAXRANK];
  for (SizeT i = 0; i < rank; ++i) for (int iloop = 0; iloop < nchunk; ++iloop) srcDimPool[iloop][i] = 0;
  //template accelerator
  SizeT templateDim[MAXRANK];
  for (SizeT i = 0; i < rank; ++i) templateDim[i] = 0;

  //compute iloop's accelerator with fast direct method
  for (long iloop = 0; iloop < nchunk; ++iloop) {
    SizeT e = iloop*chunksize;
    SizeT sizeleft = e;
    for (long i = 0; i < rank; ++i) {
      DUInt pi = perm[i]; //note the transpose effect.
      sizeleft /= resDim[i];
      templateDim[pi] = e - sizeleft * resDim[i];
      e = sizeleft;
    }
    //memorize current state accelerator for chunk iloop:
    for (long j = 0; j < rank; ++j) srcDimPool[iloop][j] = templateDim[j];
  }

#pragma omp parallel num_threads(nchunk) 
  {
#pragma omp for 
    for (long iloop = 0; iloop < nchunk; ++iloop) {
      // populate src multi dim
      SizeT srcDim[MAXRANK];
      for (SizeT i = 0; i < rank; ++i) srcDim[i] = srcDimPool[iloop][i];
      //inner loop
      for (SizeT e = iloop * chunksize; (e < (iloop + 1) * chunksize && e < nElem); ++e) {
        // src multi dim to one dim src offset index
        SizeT ix = 0;
        for (SizeT i = 0; i < rank; ++i) ix += srcDim[i] * srcStride[i];
        (*res)[ e] = (*this)[ ix];
        // update src multi dim for next dest offset index: here is the transpose effect.
        for (SizeT i = 0; i < rank; ++i) {
          DUInt pi = perm[i];
          srcDim[pi]++;
          if (srcDim[pi] < resDim[i]) break;
          srcDim[pi] = 0;
        }
      }
    }
  }
  return res;
}

// used by reverse

template<class Sp>
void Data_<Sp>::Reverse(DLong dim) {
  // SA: based on total_over_dim_template()
  //   static Data_* tmp = new Data_(dimension(1), BaseGDL::NOZERO);
  //Guard<Data_> tmp_guard(tmp);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT revLimit = this->dim[dim] * revStride;
#pragma omp parallel //if ((nEl/outerStride)*revStride >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl/outerStride)*revStride))
  {
#pragma omp for
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = 0; i < revStride; ++i) {
        SizeT oi = o + i;
        SizeT last_plus_oi = revLimit + oi - revStride + oi;
        SizeT half = ((revLimit / revStride) / 2) * revStride + oi;
        for (SizeT s = oi; s < half; s += revStride) {
          SizeT opp = last_plus_oi - s;
          Ty tmp = (*this)[s];
          (*this)[s] = (*this)[opp];
          (*this)[opp] = tmp;
        }
      }
    }
  }
}

template<class Sp>
BaseGDL* Data_<Sp>::DupReverse(DLong dim) {
  // SA: based on total_over_dim_template()
  Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
  Guard<Data_> res_guard(res);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT revLimit = this->dim[dim] * revStride;
#pragma omp parallel //if ((nEl/outerStride)*revStride >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl/outerStride)*revStride))
  {
#pragma omp for
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = 0; i < revStride; ++i) {
        SizeT oi = o + i;
        SizeT last_plus_oi = revLimit + oi - revStride + oi;
        SizeT half = ((revLimit / revStride) / 2) * revStride + oi;
        for (SizeT s = oi; s < half + 1; s += revStride) {
          SizeT opp = last_plus_oi - s;
          //	cout << s <<" "<< opp << " " << (*this)[s] << " " << (*this)[opp] << endl;
          (*res)[s] = (*this)[opp];
          (*res)[opp] = (*this)[s];
        }
      }
    }
  }
  return res_guard.release();
}
template<>
BaseGDL* Data_<SpDPtr>::DupReverse( DLong dim) {
  // SA: based on total_over_dim_template()
  Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
  Guard<Data_> res_guard(res);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT revLimit = this->dim[dim] * revStride;
#pragma omp parallel //if ((nEl/outerStride)*revStride >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl/outerStride)*revStride))
  {
#pragma omp for
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = 0; i < revStride; ++i) {
        SizeT oi = o + i;
        SizeT last_plus_oi = revLimit + oi - revStride + oi;
        SizeT half = ((revLimit / revStride) / 2) * revStride + oi;
        for (SizeT s = oi; s < half + 1; s += revStride) {
          SizeT opp = last_plus_oi - s;
          (*res)[s] = (*this)[opp];
          (*res)[opp] = (*this)[s];
        }
      }
    }
  }
  GDLInterpreter::IncRef(res);
  return res_guard.release();
}
template<>
BaseGDL* Data_<SpDObj>::DupReverse(DLong dim) {
  // SA: based on total_over_dim_template()
  Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
  Guard<Data_> res_guard(res);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT revLimit = this->dim[dim] * revStride;
#pragma omp parallel //if ((nEl/outerStride)*revStride >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl/outerStride)*revStride))
  {
#pragma omp for
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = 0; i < revStride; ++i) {
        SizeT oi = o + i;
        SizeT last_plus_oi = revLimit + oi - revStride + oi;
        SizeT half = ((revLimit / revStride) / 2) * revStride + oi;
        for (SizeT s = oi; s < half + 1; s += revStride) {
          SizeT opp = last_plus_oi - s;
          (*res)[s] = (*this)[opp];
          (*res)[opp] = (*this)[s];
        }
      }
    }
  }
  GDLInterpreter::IncRefObj(res);
  return res_guard.release();
}

// rank must be 1 or 2 (already checked)
template<class Sp> 
BaseGDL* Data_<Sp>::Rotate( DLong dir) {
  dir = (dir % 8 + 8) % 8; // bring into 0..7 range

  if (dir == 0) return Dup();
  if (dir == 2) {
    Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
    SizeT nEl = N_Elements();
//no pragma: better optimized directly by the compiler!
    {
      for (SizeT i = 0; i < nEl; ++i)
        (*res)[i] = (*this)[ nEl - 1 - i];
    }
    return res;
  }

  if (this->Rank() == 1) {
    if (dir == 7) return Dup();

    if (dir == 1 || dir == 4) {
      return new Data_(dimension(1, N_Elements()), dd);
    }
    if (dir == 5) // || dir == 2
    {
      Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
      SizeT nEl = N_Elements();
//no pragma: better optimized directly by the compiler!
      {
        for (SizeT i = 0; i < nEl; ++i)
          (*res)[ i] = (*this)[ nEl - 1 - i];
      }
      return res;
    }
    // 3 || 6
    Data_* res = new Data_(dimension(1, N_Elements()), BaseGDL::NOZERO);
    SizeT nEl = N_Elements();
//no pragma: better optimized directly by the compiler!
    {
      for (SizeT i = 0; i < nEl; ++i)
        (*res)[ i] = (*this)[ nEl - 1 - i];
    }
    return res;
  }

  // rank == 2, dir == 0 and dir == 2 already handled
  bool keepDim = (dir == 5) || (dir == 7);

  Data_* res;
  if (keepDim) {
    res = new Data_(this->dim, BaseGDL::NOZERO);
  } else {
    res = new Data_(dimension(this->dim[1], this->dim[0]), BaseGDL::NOZERO);
  }

//  bool flipX = dir == 3 || dir == 5 || dir == 6;
//  bool flipY = dir == 1 || dir == 6 || dir == 7;

  SizeT xEl = this->dim[0];
  SizeT yEl = this->dim[1];
  SizeT i = 0;
  // enable fast optimzation by removing ifs outside loops.
  if (dir == 1) { // flipY
    for (SizeT y = 0; y < yEl; ++y) for (SizeT x = 0; x < xEl; ++x) (*res)[ x * yEl + yEl - 1 - y] = (*this)[ i++];
  } else if (dir == 3) { // flipX
    for (SizeT y = 0; y < yEl; ++y) for (SizeT x = 0; x < xEl; ++x) (*res)[ (xEl - 1 - x) * yEl + y] = (*this)[ i++];
  } else if (dir == 4) { 
  for (SizeT y = 0; y < yEl; ++y) for (SizeT x = 0; x < xEl; ++x) (*res)[x * yEl + y] = (*this)[ i++];
  } else if (dir == 5) { // flipX, keepDim
  for (SizeT y = 0; y < yEl; ++y) for (SizeT x = 0; x < xEl; ++x) (*res)[y * xEl + xEl - 1 - x] = (*this)[ i++];
  } else if (dir == 6) { // flipX, flipY
  for (SizeT y = 0; y < yEl; ++y) for (SizeT x = 0; x < xEl; ++x) (*res)[(xEl - 1 - x )* yEl + yEl - 1 - y] = (*this)[ i++];
  } else if (dir == 7) { // flipY, keepDim
  for (SizeT y = 0; y < yEl; ++y) for (SizeT x = 0; x < xEl; ++x) (*res)[(yEl - 1 - y) * xEl + x ] = (*this)[ i++];
  }
//was:
//  for (SizeT y = 0; y < yEl; ++y) {
//    SizeT yR = flipY ? yEl - 1 - y : y;
//    for (SizeT x = 0; x < xEl; ++x) {
//      SizeT xR = flipX ? xEl - 1 - x : x;
//
//      SizeT ix = keepDim ? yR * xEl + xR : xR * yEl + yR;
//
//      (*res)[ix] = (*this)[ i++];
//    }
//  }
  return res;
}

template<class Sp> 
typename Data_<Sp>::Ty Data_<Sp>::Sum() const 
{
  Ty s= dd[ 0];
  SizeT nEl = dd.size();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( s)
    {
#pragma omp for reduction(+:s)
      for( int i=1; i<nEl; ++i)
	{
	  s += dd[ i];
	}
    }
  return s;
}

template<> 
Data_<SpDString>::Ty Data_<SpDString>::Sum() const 
{
  Ty s= dd[ 0];
  SizeT nEl = dd.size();
  for( SizeT i=1; i<nEl; ++i)
    {
      s += dd[ i];
    }
  return s;
}
template<> 
Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::Sum() const 
{
  DDouble sr= dd[ 0].real();
  DDouble si= dd[ 0].imag();
  SizeT nEl = dd.size();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( sr,si)
    {
#pragma omp for reduction(+:si,sr)
  for( SizeT i=1; i<nEl; ++i)
    {
      sr += dd[i].real();
      si += dd[i].imag();
    }
  }
  return std::complex<double>(sr,si);
}
template<> 
Data_<SpDComplex>::Ty Data_<SpDComplex>::Sum() const 
{
  DFloat sr= dd[ 0].real();
  DFloat si= dd[ 0].imag();
  SizeT nEl = dd.size();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel //if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( sr,si)
    {
#pragma omp for reduction(+:si,sr)
  for( SizeT i=1; i<nEl; ++i)
    {
      sr += dd[i].real();
      si += dd[i].imag();
    }
  }
  return std::complex<float>(sr,si);
}

// template<class Sp> 
// typename Data_<Sp>::DataT& Data_<Sp>:: Resize( SizeT n)
// {
//   if( n > dd.size())
//     throw GDLException("Internal error: Data_::Resize(...) tried to grow.");
//   if( dd.size() != n) 
//     {
//       DataT rsArr( n);
//       std::copy( &(*this)[0], &(*this)[n], &rsArr[0]);
//       dd.resize( n); // discards data
//       std::copy( &rsArr[0], &rsArr[n], &(*this)[0]);
//     }
//   return dd;
// }

// template<class Sp> 
// typename Data_<Sp>::Ty& Data_<Sp>::operator[] (const SizeT d1) 
// { return (*this)[d1];}

// only used from DStructGDL
template<class Sp> 
Data_<Sp>&  Data_<Sp>::operator=(const BaseGDL& r)
{
  assert( r.Type() == this->Type());
  const Data_<Sp>& right = static_cast<const Data_<Sp>&>( r);
  assert( &right != this);
  //   if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  dd = right.dd;
  return *this;
}
// only used from DStructGDL
template<>
Data_<SpDPtr>& Data_<SpDPtr>::operator=(const BaseGDL& r)
{
  assert( r.Type() == this->Type());
  const Data_& right = static_cast<const Data_&>( r);
  assert( &right != this);
  //   if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  GDLInterpreter::DecRef( this);
  dd = right.dd;
  GDLInterpreter::IncRef( this);
  return *this;
}
// only used from DStructGDL
template<>
Data_<SpDObj>& Data_<SpDObj>::operator=(const BaseGDL& r)
{
  assert( r.Type() == this->Type());
  const Data_& right = static_cast<const Data_&>( r);
  assert( &right != this);
  //   if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  GDLInterpreter::DecRefObj( this);
  dd = right.dd;
  GDLInterpreter::IncRefObj( this);
  return *this;
}


template<class Sp> 
void Data_<Sp>::InitFrom(const BaseGDL& r)
{
  assert( r.Type() == this->Type());
  const Data_<Sp>& right = static_cast<const Data_<Sp>&>( r);
  assert( &right != this);
  //   if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  dd.InitFrom( right.dd);
  //   return *this;
}
// only used from DStructGDL::DStructGDL(const DStructGDL& d_)
template<>
void Data_<SpDPtr>::InitFrom(const BaseGDL& r)
{
  assert( r.Type() == this->Type());
  const Data_& right = static_cast<const Data_&>( r);
  assert( &right != this);
  //   if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  //   GDLInterpreter::DecRef( this);
  dd.InitFrom( right.dd);
  GDLInterpreter::IncRef( this);
  //   return *this;
}
// only used from DStructGDL::DStructGDL(const DStructGDL& d_)
template<>
void Data_<SpDObj>::InitFrom(const BaseGDL& r)
{
  assert( r.Type() == this->Type());
  const Data_& right = static_cast<const Data_&>( r);
  assert( &right != this);
  //   if( &right == this) return *this; // self assignment
  this->dim = right.dim;
  //   GDLInterpreter::DecRefObj( this);
  dd.InitFrom( right.dd);
  GDLInterpreter::IncRefObj( this);
  //   return *this;
}

template< class Sp>
bool Data_<Sp>::EqType( const BaseGDL* r) const 
{ return (this->Type() == r->Type());}

template< class Sp>
void* Data_<Sp>::DataAddr()// SizeT elem)
{ return &(*this)[0];}//elem];}

// template<>
// void* Data_<SpDString>::DataAddr()// SizeT elem)
// { 
//  throw GDLException("STRING not allowed in this context.");
// return NULL;
// }
// template<>
// void* Data_<SpDPtr>::DataAddr()// SizeT elem)
// { 
//  throw GDLException("PTR not allowed in this context.");
// return NULL;
// }
// template<>
// void* Data_<SpDObj>::DataAddr()// SizeT elem)
// { 
//  throw GDLException("Object expression not allowed in this context.");
// return NULL;
// }

template< class Sp>
SizeT Data_<Sp>::N_Elements() const 
{ return dd.size();}

template<>
SizeT Data_<SpDObj>::N_Elements() const 
{ 
  if( !this->StrictScalar())
    return dd.size();
  
  DObj s = dd[0]; // is StrictScalar()
  if( s == 0)  // no overloads for null object
    return 1;
  
  DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( s);
  if( oStructGDL == NULL) // if object not valid -> default behaviour
    return 1;
  
  DStructDesc* desc = oStructGDL->Desc();

  if( desc->IsParent("LIST"))
  {
      // no static here, might vary in derived object
      unsigned nListTag = desc->TagIndex( "NLIST");
      SizeT listSize = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nListTag, 0)))[0];
      return listSize;
  }
  if( desc->IsParent("HASH"))
  {
      // no static here, might vary in derived object
      unsigned nListTag = desc->TagIndex( "TABLE_COUNT");
      SizeT listSize = (*static_cast<DLongGDL*>(oStructGDL->GetTag( nListTag, 0)))[0];
      return listSize;
  }

  return 1;
}


template< class Sp>
SizeT Data_<Sp>::Size() const 
{ return dd.size();}
template< class Sp>
SizeT Data_<Sp>::Sizeof() const 
{ return sizeof(Ty);}

template< class Sp>
void Data_<Sp>::Clear() 
{ 
  SizeT nEl = dd.size(); 
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) (*this)[ i] = Sp::zero;
}//}

// first time initialization (construction)
template< class Sp>
void Data_<Sp>::Construct() 
{
  // note that this is not possible in cases where an operation 
  // (here: 'new' which is ok) isn't defined for any POD
  // (although this code never executes and should be optimized away anyway)
  const bool isPOD = Sp::IS_POD;   
  // do nothing for POD
  if( !isPOD)
  {
  SizeT nEl = dd.size(); 
  //  for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty;
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) new (&(dd[ i])) Ty;
  }
}
template<>
void Data_<SpDPtr>::Construct() 
{
  SizeT nEl = dd.size(); 
  //  for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty;
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) dd[ i] = 0;
}//}
template<>
void Data_<SpDObj>::Construct()
{
  SizeT nEl = dd.size(); 
  //  for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty;
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) dd[ i] = 0;
}//}
// // non POD - use placement new
// template<>
// void Data_< SpDString>::Construct() 
// { 
//   SizeT nEl = dd.size(); 
//   //  for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty;
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( SizeT i = 0; i<nEl; ++i) new (&(dd[ i])) Ty;
// }//}
// template<>
// void Data_< SpDComplex>::Construct() 
// { 
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty;
// }//}
// template<>
// void Data_< SpDComplexDbl>::Construct() 
// { 
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty;
// }//}

// construction and initalization to zero
template< class Sp>
void Data_<Sp>::ConstructTo0() 
{ 
  if( Sp::IS_POD)
  {
  SizeT nEl = dd.size(); 
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) (*this)[ i] = Sp::zero;
  }
  else
  {
  SizeT nEl = dd.size(); 
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty( Sp::zero);
  }
}//}
// // non POD - use placement new
// template<>
// void Data_< SpDString>::ConstructTo0() 
// { 
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( int i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty( zero);
// }//}
// template<>
// void Data_< SpDComplex>::ConstructTo0() 
// { 
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( int i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty( zero);
// }//}
// template<>
// void Data_< SpDComplexDbl>::ConstructTo0() 
// { 
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( int i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty( zero);
// }//}

template< class Sp>
void Data_<Sp>::Destruct() 
{ 
  // no destruction for POD
  if( !Sp::IS_POD)
  {
  SizeT nEl = dd.size(); 
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( SizeT i = 0; i<nEl; ++i) 
    (*this)[ i].~Ty();    
  }
}
template<>
void Data_< SpDPtr>::Destruct()
{
  GDLInterpreter::DecRef( this);
}
template<>
void Data_< SpDObj>::Destruct()
{
  GDLInterpreter::DecRefObj( this);
}
// template<>
// void Data_< SpDString>::Destruct() 
// {
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( SizeT i = 0; i<nEl; ++i) 
//     (*this)[ i].~DString();
// }//}
// template<>
// void Data_< SpDComplex>::Destruct() 
// {
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( SizeT i = 0; i<nEl; ++i) 
//     (*this)[ i].~DComplex();
// }//}
// template<>
// void Data_< SpDComplexDbl>::Destruct() 
// {
//   SizeT nEl = dd.size(); 
//   /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//     {
//     #pragma omp for*/
//   for( SizeT i = 0; i<nEl; ++i) 
//     (*this)[ i].~DComplexDbl();
// }//}

template< class Sp>
BaseGDL* Data_<Sp>::SetBuffer( const void* b)
{
  dd.SetBuffer( static_cast< Ty*>(const_cast<void*>( b)));
  return this;
}
template< class Sp>
void Data_<Sp>::SetBufferSize( SizeT s)
{
  dd.SetBufferSize( s);
}

// template< class Sp>
// Data_<Sp>* Data_<Sp>::Dup() 
// { return new Data_(*this);}


template< class Sp>
Data_<Sp>* Data_<Sp>::New( const dimension& dim_, BaseGDL::InitType noZero) const
{
  if( noZero == BaseGDL::NOZERO) return new Data_(dim_, BaseGDL::NOZERO);
  if( noZero == BaseGDL::INIT)
    {
      Data_* res =  new Data_(dim_, BaseGDL::NOZERO);
      SizeT nEl = res->dd.size();
      /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	#pragma omp for*/
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = (*this)[ 0]; // set all to scalar
      //}
      return res;
    }
  return new Data_(dim_); // zero data
}


template< class Sp>
Data_<Sp>* Data_<Sp>::NewResult() const 
{
  return new Data_(this->dim, BaseGDL::NOZERO);
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
//   s=(*this)[0];
//   return true;
// }


template<class Sp>
SizeT Data_<Sp>::NBytes() const 
{ return (dd.size() * sizeof(Ty));}

// string, ptr, obj cannot calculate their bytes
// only used by assoc function
template<> SizeT Data_<SpDString>::NBytes() const
{
  SizeT nEl = dd.size();
  SizeT nB = 0;
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for( int i=0; i<nEl; ++i)
    nB += (*this)[i].size();
  //}
  return nB;
}
// template<> SizeT Data_<SpDObj>::NBytes() const
// {
//   throw GDLException("object references");
// }
// template<> SizeT Data_<SpDPtr>::NBytes() const
// {
//   throw GDLException("pointers");
// }

template<class Sp>
SizeT Data_<Sp>::ToTransfer() const
{
  return dd.size();
}
// complex has 2 elements to transfer
template<> SizeT Data_<SpDComplex>::ToTransfer() const
{
  return N_Elements() * 2;
}
template<> SizeT Data_<SpDComplexDbl>::ToTransfer() const
{
  return N_Elements() * 2;
}

// // note that min and max are not defined in BaseGDL
// template<class Sp> 
// typename Data_<Sp>::Ty Data_<Sp>::min() const
// { return dd.min();}
// template<class Sp> 
// typename Data_<Sp>::Ty Data_<Sp>::max() const
// { return dd.max();}
// template<>
// Data_<SpDComplex>::Ty Data_<SpDComplex>::min() const
// {
//   throw GDLException("COMPLEX expression not allowed in this context.");
// }
// template<>
// Data_<SpDComplex>::Ty Data_<SpDComplex>::max() const
// {
//   throw GDLException("COMPLEX expression not allowed in this context.");
// }
// template<>
// Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::min() const
// {
//   throw GDLException("COMPLEXDBL expression not allowed in this context.");
// }
// template<>
// Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::max() const
// {
//   throw GDLException("COMPLEXDBL expression not allowed in this context.");
// }


// for HASH objects
template<class Sp> 
DDouble Data_<Sp>::HashValue() const
{
  return static_cast<DDouble>((*this)[0]);
}
template<> 
DDouble Data_<SpDComplex>::HashValue() const
{
  return real((*this)[0]);
}
template<> 
DDouble Data_<SpDComplexDbl>::HashValue() const
{
  return real((*this)[0]);
}
template<> 
DDouble Data_<SpDString>::HashValue() const
{
  throw GDLException("STRING expression not allowed as index. Please report.");
  return 0; // get rid of warning
}
template<> 
DDouble Data_<SpDPtr>::HashValue() const
{
  throw GDLException("PTR expression not allowed as index. Please report.");
  return 0; // get rid of warning
}

template<> 
DDouble Data_<SpDObj>::HashValue() const
{
  throw GDLException("Object expression not allowed as index. Please report.");
  return 0; // get rid of warning
}


// -1 -> p2 is greater
// 0  -> equal
// 1  -> this is greater

// note: this is for internal use only (for HASH objects)
// this should not be called on non-numeric types (also for p2)
template<class Sp> 
int Data_<Sp>::HashCompare( BaseGDL* p2) const
{
  assert( dd.size() == 1);
  assert( p2->N_Elements() == 1);
  if( p2->Type() == GDL_STRING)
    return 1; // strings 1st (smallest)
  
  assert( NumericType(p2->Type()));

  if( this->IS_INTEGER)
  {
    if( IntType( p2->Type())) // make full use of data type
    {
      RangeT thisValue = this->LoopIndex();
      RangeT p2Value = p2->LoopIndex();
      if( thisValue == p2Value)
	return 0;
      if( thisValue < p2Value)
	return -1;
      return 1;
    }
  }  
  DDouble thisValue = this->HashValue();
  DDouble p2Value = p2->HashValue();
  if( thisValue == p2Value)
    return 0;
  if( thisValue < p2Value)
    return -1;
  return 1;
}

template<> 
int Data_<SpDString>::HashCompare( BaseGDL* p2) const
{
  assert( dd.size() == 1);
  assert( p2->N_Elements() == 1);
  if( p2->Type() != this->Type())
    return -1; // strings 1st (smallest)
  
  Data_* p2String = static_cast<Data_*>(p2);
  if( dd[0].length() == (*p2String)[0].length())
  {
    if( dd[0] == (*p2String)[0])
      return 0;
    if( dd[0] < (*p2String)[0])
      return -1;
    return 1;
  }
  else if( dd[0].length() < (*p2String)[0].length())
  {
    return -1;
  }
  return 1;
}


// Scalar2Index
// used by the interpreter
// -2  < 0 array
// -1  < 0 scalar
// 0   empty or array
// 1   scalar
// 2   one-element array
template<class Sp> 
int Data_<Sp>::Scalar2Index( SizeT& st) const
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
  if( (*this)[0] < 0) {
    if( this->dim.Rank() != 0) 
      return -2;
    else
      return -1;
  }

  st= static_cast<SizeT>((*this)[0]);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}

template<class Sp> 
int Data_<Sp>::Scalar2RangeT( RangeT& st) const
{
  if( dd.size() != 1) return 0;

  st= static_cast<RangeT>((*this)[0]);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}

template<> 
int Data_<SpDComplex>::Scalar2Index( SizeT& st) const
{
  if( dd.size() != 1) return 0;
  float r=real((*this)[0]);
  if( r < 0.0) return -1;
  st= static_cast<SizeT>(r);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}

template<> 
int Data_<SpDComplex>::Scalar2RangeT( RangeT& st) const
{
  if( dd.size() != 1) return 0;
  float r=real((*this)[0]);
  st= static_cast<RangeT>(r);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}

template<>
int Data_<SpDComplexDbl>::Scalar2Index( SizeT& st) const
{
  if( dd.size() != 1) return 0;
  double r=real((*this)[0]);
  if( r < 0.0) return -1;
  st= static_cast<SizeT>(r);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}

template<> 
int Data_<SpDComplexDbl>::Scalar2RangeT( RangeT& st) const
{
  if( dd.size() != 1) return 0;
  double r=real((*this)[0]);
  st= static_cast<RangeT>(r);
  if( this->dim.Rank() != 0) return 2;
  return 1;
}


template<> 
int Data_<SpDString>::Scalar2Index( SizeT& st) const
{
  if( dd.size() != 1) return 0;

  SizeT tSize=(*this)[0].size();

  if( tSize == 0) 
    {
      st=0;
    }
  else 
    {
      long int number = Str2L( (*this)[0].c_str());
      if( number < 0) return -1; 
      st=number;
    }
  if( dim.Rank() != 0) return 2;
  return 1;
}

template<> 
int Data_<SpDString>::Scalar2RangeT( RangeT& st) const
{
  if( dd.size() != 1) return 0;

  SizeT tSize=(*this)[0].size();

  if( tSize == 0) 
    {
      st=0;
    }
  else 
    {
      long int number = Str2L( (*this)[0].c_str());
      st=number;
    }
  if( dim.Rank() != 0) return 2;
  return 1;
}


template<> 
int Data_<SpDPtr>::Scalar2Index( SizeT& st) const
{
  throw GDLException("PTR expression not allowed in this context.");
  return 0; // get rid of warning
}
template<> 
int Data_<SpDPtr>::Scalar2RangeT( RangeT& st) const
{
  throw GDLException("PTR expression not allowed in this context.");
  return 0; // get rid of warning
}

template<> 
int Data_<SpDObj>::Scalar2Index( SizeT& st) const
{
  throw GDLException("Object expression not allowed in this context.");
  return 0; // get rid of warning
}
template<> 
int Data_<SpDObj>::Scalar2RangeT( RangeT& st) const
{
  throw GDLException("Object expression not allowed in this context.");
  return 0; // get rid of warning
}






// for FOR loop *indices*
template<class Sp> 
RangeT Data_<Sp>::LoopIndex() const
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
  //   if( (*this)[0] < 0)
  //     throw GDLException( "Index variable <0.");

  return static_cast<RangeT>((*this)[0]);
}
template<> 
RangeT Data_<SpDFloat>::LoopIndex() const
{
  //   if( (*this)[0] < 0.0f)
  //     //if( (*this)[0] <= 1.0f)
  //       throw GDLException( "Index variable <0.");
  //else
  //  return 0;

  return static_cast<RangeT>((*this)[0]);
}
template<> 
RangeT Data_<SpDDouble>::LoopIndex() const
{
  //   if( (*this)[0] < 0.0)
  //     //if( (*this)[0] <= 1.0)
  //       throw GDLException( "Index variable <0.");
  //else
  //  return 0;

  return static_cast<RangeT>((*this)[0]);
}
template<> 
RangeT Data_<SpDComplex>::LoopIndex() const
{
  throw GDLException( "Complex expression not allowed as index.");
  return 0;
}
template<> 
RangeT Data_<SpDComplexDbl>::LoopIndex() const
{
  throw GDLException( "Complex expression not allowed as index.");
  return 0;
}
template<> 
RangeT Data_<SpDString>::LoopIndex() const
{
  if( (*this)[0] == "")
    return 0;
	
  const char* cStart=(*this)[0].c_str();
  char* cEnd;
  RangeT ix=strtol(cStart,&cEnd,10);
  if( cEnd == cStart)
    {
      Warning( "Type conversion error: "
	       "Unable to convert given STRING: '"+
	       (*this)[0]+"' to index.");
      return 0;
    }
  return ix;
}

template<> 
RangeT Data_<SpDPtr>::LoopIndex() const
{
  throw GDLException("PTR expression not allowed as index.");
  return 0; // get rid of warning
}

template<> 
RangeT Data_<SpDObj>::LoopIndex() const
{
  throw GDLException("Object expression not allowed as index.");
  return 0; // get rid of warning
}


// True
template<class Sp>
bool Data_<Sp>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s % 2);
}

template<>
bool Data_<SpDFloat>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0.0f);
}

template<>
bool Data_<SpDDouble>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0.0);
}

template<>
bool Data_<SpDString>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != "");
}

template<>
bool Data_<SpDComplex>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (real(s) != 0.0 || imag(s) != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (real(s) != 0.0 || imag(s) != 0.0);
}

template<>
bool Data_<SpDPtr>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0);
}

template<>
bool Data_<SpDObj>::True()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  if( s == 0)
    return false; // on overloads for null object -> default is 'false'

//   DStructGDL* oStructGDL= GDLInterpreter::GetObjHeapNoThrow( s);
//   if( oStructGDL == NULL) // object not valid -> default behaviour
//     return true; // true is ok here: Default behaviour is to just checks for null object
// 
//   DStructDesc* desc = oStructGDL->Desc();
//   
//   DFun* isTrueOverload = static_cast<DFun*>(desc->GetOperator( OOIsTrue));

  DSubUD* isTrueOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( s, OOIsTrue));
  if( isTrueOverload == NULL) 
    return true; // not overloaded, false case for default already returned (s. a.)
  
  ProgNodeP callingNode = interpreter->GetRetTree();
    
//   BaseGDL* self = this->Dup();
//   Guard<BaseGDL> selfGuard( self);
//   EnvUDT* newEnv= new EnvUDT( callingNode, isTrueOverload, &self);
  // no parameters
  
  EnvUDT* newEnv;
  DObjGDL* self;
  Guard<BaseGDL> selfGuard;
  // Dup() here is not optimal
  // avoid at least for internal overload routines (which do/must not change SELF or r)
  bool internalDSubUD = isTrueOverload->GetTree()->IsWrappedNode();  
  if( internalDSubUD)  
  {
    self = this;
    newEnv= new EnvUDT( callingNode, isTrueOverload, &self);
  }
  else
  {
    self = this->Dup();
    selfGuard.Init( self);
    newEnv= new EnvUDT( callingNode, isTrueOverload, &self);
  }
  
  StackGuard<EnvStackT> guard(interpreter->CallStack());

  interpreter->CallStack().push_back( newEnv); 
  
  // make the call
  BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  if( !internalDSubUD && self != selfGuard.Get())
  {
    // always put out warning first, in case of a later crash
    Warning( "WARNING: " + isTrueOverload->ObjectName() + 
	  ": Assignment to SELF detected (GDL session still ok).");
    // assignment to SELF -> self was deleted and points to new variable
    // which it owns
    selfGuard.Release();
    if( (BaseGDL*)self != NullGDL::GetSingleInstance())
      selfGuard.Reset(self);
  }
  if( NullGDL::IsNULLorNullGDL( res))
  {
    throw GDLException( isTrueOverload->ObjectName() + " returned an undefined value.",true,false);
  }
  
  Guard<BaseGDL> resGuard( res);
  
  // prevent recursion
  if( res->Type() == GDL_OBJ)
  {
    ostringstream os;
    res->ToStream(os);
    throw GDLException( isTrueOverload->ObjectName() + ": Object reference expression not allowed in this context: " +
			 os.str(),true,false);
  }
  
  return res->LogTrue();
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
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
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

// Equal (deletes r) only used in ForCheck(...)
template<class Sp>
bool Data_<Sp>::Equal( BaseGDL* r) const
{
  assert( r->StrictScalar());
  //   if( !r->Scalar())
  //     {
  //       GDLDelete(r);
  //       throw GDLException("Expression must be a scalar in this context.");
  //     }
  assert( r->Type() == this->t);
  Data_* rr=static_cast<Data_*>(r);
  //  Data_* rr=static_cast<Data_*>(r->Convert2( this->t));
  bool ret= ((*this)[0] == (*rr)[0]);
  GDLDelete(rr);
  return ret;
}
template<>
bool Data_<SpDFloat>::Equal( BaseGDL* r) const
{
  assert( r->StrictScalar());
  //   if( !r->Scalar())
  //     {
  //       GDLDelete(r);
  //       throw GDLException("Expression must be a scalar in this context.");
  //     }
  assert( r->Type() == this->t);
  Data_* rr=static_cast<Data_*>(r);
  //  Data_* rr=static_cast<Data_*>(r->Convert2( this->t));
  bool ret= fabs((*this)[0] - (*rr)[0]) < 1.0f;
  GDLDelete(rr);
  return ret;
}
template<>
bool Data_<SpDDouble>::Equal( BaseGDL* r) const
{
  assert( r->StrictScalar());
  //   if( !r->Scalar())
  //     {
  //       GDLDelete(r);
  //       throw GDLException("Expression must be a scalar in this context.");
  //     }
  assert( r->Type() == this->t);
  Data_* rr=static_cast<Data_*>(r);
  //  Data_* rr=static_cast<Data_*>(r->Convert2( this->t));
  bool ret= fabs((*this)[0] - (*rr)[0]) < 1.0;
  GDLDelete(rr);
  return ret;
}

// Equal (does not delete r)
template<class Sp>
bool Data_<Sp>::EqualNoDelete( const BaseGDL* r) const
{
  if( !r->Scalar())
    {
      throw GDLException("Expression must be a scalar in this context.");
    }
  bool ret;
  if( r->Type() != this->t)
    {
      Data_* rr=static_cast<Data_*>(const_cast<BaseGDL*>(r)->Convert2( this->t, BaseGDL::COPY));
      ret= ((*this)[0] == (*rr)[0]);
      GDLDelete(rr);
    }
  else
    {
      const Data_* rr=static_cast<const Data_*>(r);
      ret= ((*this)[0] == (*rr)[0]);
    }
  return ret;
}

bool DStructGDL::Equal( BaseGDL* r) const
{
  GDLDelete(r);
  throw GDLException("Struct expression not allowed in this context.");
  return false;
}

// For array_equal r must be of same type
template<class Sp>
bool Data_<Sp>::ArrayEqual( BaseGDL* rIn) {
  Data_<Sp>* r = static_cast<Data_<Sp>*> (rIn);
  SizeT nEl = N_Elements();
  SizeT rEl = r->N_Elements();
  if (rEl == 1) {
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i] != (*r)[0]) return false;
    return true;
  }
  if (nEl == 1) {
    for (SizeT i = 0; i < rEl; ++i)
      if ((*this)[0] != (*r)[i]) return false;
    return true;
  }
  if (nEl != rEl) return false;
  for (SizeT i = 0; i < nEl; ++i)
    if ((*this)[i] != (*r)[i]) return false;
  return true;
}

bool DStructGDL::ArrayEqual( BaseGDL* r)
{
  throw GDLException("Struct expression not allowed in this context.");
  return false;
}

// For array_equal r must be of same type
template<class Sp>
bool Data_<Sp>::ArrayNeverEqual( BaseGDL* rIn)
{
  Data_<Sp>* r = static_cast< Data_<Sp>*>( rIn);
  SizeT nEl = N_Elements();
  SizeT rEl = r->N_Elements();
  if( rEl == 1)
    {
      for( SizeT i=0; i<nEl; ++i)
	if( (*this)[i] == (*r)[0]) return false;
      return true;
    }
  if( nEl == 1)
    {
      for( SizeT i=0; i<rEl; ++i)
	if( (*this)[0] == (*r)[i]) return false;
      return true;
    }
  if( nEl != rEl) return true;
  for( SizeT i=0; i<nEl; ++i)
    if( (*this)[i] == (*r)[i]) return false;
  return true;
}

bool DStructGDL::ArrayNeverEqual( BaseGDL* r)
{
  throw GDLException("Struct expression not allowed in this context.");
  return false;
}



template<class Sp>
bool Data_<Sp>::OutOfRangeOfInt() const 
{
  assert( this->StrictScalar());
  return (*this)[0] > std::numeric_limits< DInt>::max() || (*this)[0] < std::numeric_limits< DInt>::min();
}

template<>
bool Data_<SpDString>::OutOfRangeOfInt() const 
{
  return false;
}
template<>
bool Data_<SpDByte>::OutOfRangeOfInt() const 
{
  return false;
}
template<>
bool Data_<SpDComplex>::OutOfRangeOfInt() const 
{
  return false;
}
template<>
bool Data_<SpDComplexDbl>::OutOfRangeOfInt() const 
{
  return false;
}

// for statement compliance (int types , float types scalar only)
// (convert strings to floats here (not for first argument)
template<class Sp>
void Data_<Sp>::ForCheck( BaseGDL** lEnd, BaseGDL** lStep)
{
  // all scalars?
  if( !StrictScalar())
    throw GDLException("Loop INIT must be a scalar in this context.");

  if( !(*lEnd)->StrictScalar())
    throw GDLException("Loop LIMIT must be a scalar in this context.");

  if( lStep != NULL && !(*lStep)->StrictScalar())
    throw GDLException("Loop INCREMENT must be a scalar in this context.");
  
  // only proper types?
  if( this->t== GDL_UNDEF)
    throw GDLException("Expression is undefined.");
  if( this->t== GDL_COMPLEX || this->t == GDL_COMPLEXDBL)
    throw GDLException("Complex expression not allowed in this context.");
  if( this->t== GDL_PTR)
    throw GDLException("Pointer expression not allowed in this context.");
  if( this->t== GDL_OBJ)
    throw GDLException("Object expression not allowed in this context.");
  if( this->t== GDL_STRING)
    throw GDLException("String expression not allowed in this context.");

  // check for promotion of this (only GDL_INT) // and GDL_LONG ???
  DType lType = (*lEnd)->Type();
  if( this->t == GDL_INT && lType != GDL_INT)
    {
      if( lType == GDL_COMPLEX || lType == GDL_COMPLEXDBL)
	throw GDLException("Complex expression not allowed in this context.");  
    
      if( lType == GDL_STRING)
	{
	  *lEnd=(*lEnd)->Convert2( GDL_LONG);  // try with long
	  if( !(*lEnd)->OutOfRangeOfInt())
	    {
	      *lEnd=(*lEnd)->Convert2( GDL_INT); // back to GDL_INT if within range     
	    }
	}
      else if( !(*lEnd)->OutOfRangeOfInt())
	{
	  *lEnd=(*lEnd)->Convert2( GDL_INT);  // regular conversion    
	}  

      // if the GDL_INT range is exceeded, lEnd is NOT changed
      
      if( lStep != NULL) *lStep=(*lStep)->Convert2( (*lEnd)->Type());
      return; // finished for GDL_INT
    }
  
  if( this->t == GDL_LONG)
    {
      if( lType == GDL_COMPLEX || lType == GDL_COMPLEXDBL)
	throw GDLException("Complex expression not allowed in this context.");        
    }
  
  // no promotion happened
  *lEnd=(*lEnd)->Convert2( this->t);
  if( lStep != NULL) *lStep=(*lStep)->Convert2( this->t);
}

void DStructGDL::ForCheck( BaseGDL** lEnd, BaseGDL** lStep)
{
  throw GDLException("Struct expression not allowed in this context.");
}

// ForCheck must have been called before
template<class Sp>
bool Data_<Sp>::ForAddCondUp( BaseGDL* endLoopVar)
// bool Data_<Sp>::ForAddCondUp( ForLoopInfoT& loopInfo)
{
  (*this)[0] += 1;
  //   Data_* lEnd=static_cast<Data_*>(lEndIn);
  if( endLoopVar->Type() != this->t)
    throw GDLException("Type of FOR index variable changed.");
  Data_* lEnd=static_cast<Data_*>(endLoopVar);
  /*  Data_* lEnd=dynamic_cast<Data_*>(endLoopVar);
      if( lEnd == NULL)
      throw GDLException("Type of FOR index variable changed.");*/
  return (*this)[0] <= (*lEnd)[0]; 
}
// ForCheck must have been called before
template<class Sp>
bool Data_<Sp>::ForCondUp( BaseGDL* lEndIn)
{
  //   Data_* lEnd=static_cast<Data_*>(lEndIn);
  if( lEndIn->Type() != this->t)
    throw GDLException("Type of FOR index variable changed.");
  Data_* lEnd=static_cast<Data_*>(lEndIn);
  /*  Data_* lEnd=dynamic_cast<Data_*>(lEndIn);
      if( lEnd == NULL)
      throw GDLException("Type of FOR index variable changed.");*/
  return (*this)[0] <= (*lEnd)[0];
}
template<class Sp>
bool Data_<Sp>::ForCondDown( BaseGDL* lEndIn)
{
  //   Data_* lEnd=static_cast<Data_*>(lEndIn);
  if( lEndIn->Type() != this->t)
    throw GDLException("Type of FOR index variable changed.");
  Data_* lEnd=static_cast<Data_*>(lEndIn);
  /*  Data_* lEnd=dynamic_cast<Data_*>(lEndIn);
      if( lEnd == NULL)
      throw GDLException("Type of FOR index variable changed.");*/
  return (*this)[0] >= (*lEnd)[0];
}

// error if the type of the loop variable changed
bool DStructGDL::ForCondUp( BaseGDL*)
{ 
  throw GDLException("Type of FOR index variable changed to STRUCT.");
  return false; 
}
bool DStructGDL::ForCondDown( BaseGDL*)
{ 
  throw GDLException("Type of FOR index variable changed to STRUCT.");
  return false; 
}
template<>
bool Data_<SpDComplex>::ForCondUp( BaseGDL*)
{ 
  throw GDLException("Type of FOR index variable changed to COMPLEX.");
  return false; 
}

template<>
bool Data_<SpDComplex>::ForAddCondUp( BaseGDL* loopInfo)
// bool Data_<SpDComplex>::ForAddCondUp( ForLoopInfoT& loopInfo)
{ 
  throw GDLException("Type of FOR index variable changed to COMPLEX.");
  return false; 
}
template<>
bool Data_<SpDComplex>::ForCondDown( BaseGDL*)
{ 
  throw GDLException("Type of FOR index variable changed to COMPLEX.");
  return false; 
}

template<>
bool Data_<SpDComplexDbl>::ForAddCondUp( BaseGDL* loopInfo)
// bool Data_<SpDComplexDbl>::ForAddCondUp( ForLoopInfoT& loopInfo)
{ 
  throw GDLException("Type of FOR index variable changed to DCOMPLEX.");
  return false; 
}
template<>
bool Data_<SpDComplexDbl>::ForCondUp( BaseGDL*)
{ 
  throw GDLException("Type of FOR index variable changed to DCOMPLEX.");
  return false; 
}
template<>
bool Data_<SpDComplexDbl>::ForCondDown( BaseGDL*)
{ 
  throw GDLException("Type of FOR index variable changed to DCOMPLEX.");
  return false; 
}

// ForCheck must have been called before
// general version
template<class Sp>
void Data_<Sp>::ForAdd( BaseGDL* addIn)
{
  if( addIn == NULL)
    {
      (*this)[0] += 1;
      return;
    }
  Data_* add=static_cast<Data_*>(addIn);
  (*this)[0] += (*add)[0];
}
// cannnot be called, just to make the compiler shut-up
void DStructGDL::ForAdd( BaseGDL* addIn) {}

// normal (+1) version
template<class Sp>
void Data_<Sp>::ForAdd()
{
  (*this)[0] += 1;
}
// cannnot be called, just to make the compiler shut-up
void DStructGDL::ForAdd() {}

template<class Sp>
void Data_<Sp>::AssignAtIx( RangeT ixR, BaseGDL* srcIn)
{
  if( ixR < 0)
    {
      SizeT nEl = this->N_Elements();
	
      if( -ixR > nEl)
	throw GDLException("Subscript out of range: " + i2s(ixR));

      SizeT ix = nEl + ixR;
  
      if( srcIn->Type() != this->Type())
	{
	  Data_* rConv = static_cast<Data_*>(srcIn->Convert2( this->Type(), BaseGDL::COPY_BYTE_AS_INT));
	  //      Data_* rConv = static_cast<Data_*>(srcIn->Convert2( this->Type(), BaseGDL::COPY));
	  Guard<Data_> conv_guard( rConv);
	  (*this)[ix] = (*rConv)[0];
	}
      else
	(*this)[ix] = (*static_cast<Data_*>(srcIn))[0];

      return;
    } // ixR >= 0
  if( srcIn->Type() != this->Type())
    {
      Data_* rConv = static_cast<Data_*>(srcIn->Convert2( this->Type(), BaseGDL::COPY_BYTE_AS_INT));
      //       Data_* rConv = static_cast<Data_*>(srcIn->Convert2( this->Type(), BaseGDL::COPY));
      Guard<Data_> conv_guard( rConv);
      (*this)[ixR] = (*rConv)[0];
    }
  else
    (*this)[ixR] = (*static_cast<Data_*>(srcIn))[0];
}

// assigns srcIn to this at ixList, if ixList is NULL does linear copy
// assumes: ixList has this already set as variable
// used by DotAccessDescT::DoAssign
//         GDLInterpreter::l_array_expr
template<class Sp>
void Data_<Sp>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset)
{
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  //  bool  isScalar= (srcElem == 1);
  bool  isScalar= (srcElem == 1) && (src->Rank() == 0);
  if( isScalar) 
    { // src is scalar
      Ty scalar=(*src)[0];
      
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();

	  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	    {
	    #pragma omp for*/
	  for( int c=0; c<nCp; ++c)
	    (*this)[ c]=scalar;
	  // }
	}
      else
	{
	  SizeT nCp=ixList->N_Elements();
	  
	  AllIxBaseT* allIx = ixList->BuildIx();
	  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	    {
	    #pragma omp for*/
	  (*this)[ allIx->InitSeqAccess()]=scalar;
	  for( SizeT c=1; c<nCp; ++c)
	    (*this)[ allIx->SeqAccess()]=scalar;
	  //}	  //	    (*this)[ ixList->GetIx( c)]=scalar;
 	}
    }
  else
    {
      if( ixList == NULL)
	{
	  SizeT nCp=Data_::N_Elements();
	
	  // if (non-indexed) src is smaller -> just copy its number of elements
	  if( nCp > (srcElem-offset)) {
	    if( offset == 0)
	      nCp=srcElem;
	    else
	      throw GDLException("Source expression contains not enough elements.");
	  }
	  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	    {
	    #pragma omp for*/
	  for( int c=0; c<nCp; ++c)
	    (*this)[ c]=(*src)[c+offset];
	}//}
      else
	{
 	  // crucial part
	  SizeT nCp=ixList->N_Elements();

	  if( nCp == 1)
	    {
	      SizeT destStart = ixList->LongIx();
	      //  len = 1;
	      SizeT rStride = srcIn->Stride(this->Rank());
	      (*this)[ destStart] = (*src)[ offset/rStride];

	      //	      InsAt( src, ixList, offset);
	    }
	  else
	    {
	      if( offset == 0)
		{
		  if( srcElem < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
		    {
		    #pragma omp for*/
		  (*this)[ allIx->InitSeqAccess()]=(*src)[0];
		  for( SizeT c=1; c<nCp; ++c)
		    (*this)[ allIx->SeqAccess()]=(*src)[c];
		  // }		  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
		}
	      else
		{
		  if( (srcElem-offset) < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
		    {
		    #pragma omp for*/
		  (*this)[ allIx->InitSeqAccess()]=(*src)[offset];
		  for( SizeT c=1; c<nCp; ++c)
		    (*this)[ allIx->SeqAccess()]=(*src)[c+offset];
		  // }		  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
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
	  (*this)[ ixList->LongIx()] = (*src)[0];
	}
      else
	{
	  Ty scalar=(*src)[0];
	  AllIxBaseT* allIx = ixList->BuildIx();
	  (*this)[ allIx->InitSeqAccess()]=scalar;
	  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	    {
	    #pragma omp for*/
	  for( int c=1; c<nCp; ++c)
	    (*this)[ allIx->SeqAccess()]=scalar;
	  // 	    (*this)[ (*allIx)[ c]]=scalar;


	  // }	  //	    (*this)[ ixList->GetIx( c)]=scalar;
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
	  
	  AllIxBaseT* allIx = ixList->BuildIx();
	  (*this)[ allIx->InitSeqAccess()]=(*src)[0];
	  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	    {
	    #pragma omp for*/
	  for( int c=1; c<nCp; ++c)
	    (*this)[ allIx->SeqAccess()]=(*src)[c];
	  // 	    (*this)[ (*allIx)[ c]]=(*src)[c];
	  // }	  //		(*this)[ ixList->GetIx( c)]=(*src)[c+offset];
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

      /*      dd = scalar;*/
      SizeT nCp=Data_::N_Elements();
      
      
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
	(*this)[ c]=scalar;
      // }      
      //       SizeT nCp=Data_::N_Elements();

      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c]=scalar;
    }
  else
    {
      SizeT nCp=Data_::N_Elements();
      
      // if (non-indexed) src is smaller -> just copy its number of elements
      if( nCp > srcElem) nCp=srcElem;
      
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
	(*this)[ c]=(*src)[c];
      // }
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
      // 	(*this)[ c]--;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()]--;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()]--;
    }//    }
}
template<class Sp>
void Data_<Sp>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1;
      //       SizeT nCp=Data_::N_Elements();
      
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c]++;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()]++;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()]++;
    }//    }
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
      // 	(*this)[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] -= 1.0f;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0f;
    }//    }
}
template<>
void Data_<SpDFloat>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1.0f;

      //       SizeT nCp=Data_::N_Elements();
      
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] += 1.0f;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0f;
    }//    }
}
template<>
void Data_<SpDDouble>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd -= 1.0;

      //       SizeT nCp=Data_::N_Elements();
      
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] -= 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0;
    }//    }
}
template<>
void Data_<SpDDouble>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      dd += 1.0;

      //       SizeT nCp=Data_::N_Elements();
      
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] += 1.0f;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0f;
    }//    }
}
// complex
template<>
void Data_<SpDComplex>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      //       dd -= 1.0f;

      SizeT nCp=Data_::N_Elements();
      
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
	(*this)[ c] -= 1.0;
    }//    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] -= 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0;
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ (*allIx)[ c]] -= 1.0;
    }//    }
}
template<>
void Data_<SpDComplex>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      //       dd += 1.0f;

      SizeT nCp=Data_::N_Elements();
      
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
      	(*this)[ c] += 1.0;
    }//    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] += 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0;
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ (*allIx)[ c]] += 1.0;
    }//    }
}
template<>
void Data_<SpDComplexDbl>::DecAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      //       dd -= 1.0;

      SizeT nCp=Data_::N_Elements();
      
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
      	(*this)[ c] -= 1.0;
    }//    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] -= 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0;
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ (*allIx)[ c]] -= 1.0;
    } //   }
}
template<>
void Data_<SpDComplexDbl>::IncAt( ArrayIndexListT* ixList) 
{
  if( ixList == NULL)
    {
      //       dd += 1.0;

      SizeT nCp=Data_::N_Elements();
      
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
      	(*this)[ c] += 1.0;
    }//    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ allIx->InitSeqAccess()] += 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0;
      //       for( SizeT c=0; c<nCp; ++c)
      // 	(*this)[ (*allIx)[ c]] += 1.0;
    }//    }
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

      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      for( int c=0; c<nCp; ++c)
	(*this)[ c+offset]=(*src)[c];
    }//    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxBaseT* allIx = ixList->BuildIx();
      /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
	{
	#pragma omp for*/
      (*this)[ offset]=(*src)[ allIx->InitSeqAccess()];
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ c+offset]=(*src)[ allIx->SeqAccess()];
      //}      //	(*this)[ c+offset]=(*src)[ ixList->GetIx( c)];
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
  catArrDim.SetOneDim(catRankIx,0);     // clear rank which is added up

  SizeT dimSum=0;
  ExprListIterT i=exprList.begin();
  for(; i != exprList.end(); ++i)
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
  
  catArrDim.SetOneDim(catRankIx,dimSum);
  
  // the concatenated array
  Data_<Sp>* catArr=New(catArrDim, BaseGDL::NOZERO);
  
  SizeT at=0;
  for( i=exprList.begin(); i != exprList.end(); ++i)
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
  AllIxBaseT* allIx = ixList->BuildIx();

  if( nCp == 1)
    {
      (*res)[0]=(*this)[ (*allIx)[ 0]];
      return res;
    }
  //   else
  //   {
  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
    {
    #pragma omp for*/
  (*res)[0]=(*this)[ allIx->InitSeqAccess()];
  for( SizeT c=1; c<nCp; ++c)
    (*res)[c]=(*this)[ allIx->SeqAccess()];
  //}  //    res_(*this)[c]=(*this)[ (*allIx)[ c]];
  //    (*res)[c]=(*this)[ ixList->GetIx(c)];
  //   }
  return res;
}

// inserts srcIn at index ixDim
// respects the exact structure of srcIn
template<class Sp>
void Data_<Sp>::InsAt( Data_* srcIn, ArrayIndexListT* ixList, SizeT offset)
{
  // max. number of dimensions to copy
  SizeT nDim = ixList->NDim();
 
  if( nDim == 1)
    //     {
    //       SizeT destStart = ixList->LongIx();

    //       SizeT len = srcIn->Dim( 0); // length of segment to copy
    //       // check if in bounds of a
    //       if( (destStart+len) > this->N_Elements()) //dim[0])
    // 	throw GDLException("Out of range subscript encountered (1).");
  
    //       DataT& srcIn_dd = srcIn->dd; 
    //       SizeT srcIx = 0; // this one simply runs from 0 to N_Elements(srcIn)

    //       SizeT destEnd = destStart + len;
    //       for( SizeT destIx = destStart; destIx < destEnd; ++destIx)
    // 	(*this)[ destIx] = (*srcIn)[ srcIx++];

    //       return;
    //     }
    {
      SizeT destStart = ixList->LongIx();

      //SizeT len;
      if( this->N_Elements() == 1)
	{
	  //	  len = 1;
	  SizeT rStride = srcIn->Stride(this->Rank());
	  (*this)[ destStart] = (*srcIn)[ offset/rStride];
	}
      else 
	{
	  SizeT len = srcIn->Dim( 0); // length of segment to copy
          // TODO: IDL reports here (and probably in the insert-dimension case below as well) 
          //       the name of a variable, e.g.:
          //       IDL> a=[0,0,0] & a[2]=[2,2,2]
          //       % Out of range subscript encountered: A.
	  if( (destStart+len) > this->N_Elements()) //dim[0])
	    throw GDLException("Out of range subscript encountered (length of insert exceeds array boundaries).");

	  // DataT& srcIn_dd = srcIn->dd; 
	  SizeT srcIx = 0; // this one simply runs from 0 to N_Elements(srcIn)

	  SizeT destEnd = destStart + len;
	  for( SizeT destIx = destStart; destIx < destEnd; ++destIx)
	    (*this)[ destIx] = (*srcIn)[ srcIx++];
	}

      return;
    }

  SizeT destStart; // 1-dim starting index 
  // ATTENTION: dimension is used as an index here
  dimension ixDim = ixList->GetDimIx0( destStart);
  nDim--;

  dimension srcDim=srcIn->Dim();
  srcDim.Purge(); // newly inserted: ticket #675
  SizeT len=srcDim[0]; // length of one segment to copy (one line of srcIn)

  //  SizeT nDim   =RankIx(ixDim.Rank());  
  SizeT srcNDim=RankIx(srcDim.Rank()); // number of source dimensions
  if( srcNDim < nDim) nDim=srcNDim;

  // check limits (up to Rank to consider)
  for( SizeT dIx=0; dIx <= nDim; ++dIx)
    // check if in bounds of a
    if( (ixDim[dIx]+srcDim[dIx]) > this->dim[dIx])
      throw GDLException("Out of range subscript encountered (dimension of insert exceeds array boundaries for dimension " + i2s(dIx +1) + ").");

  SizeT nCp=srcIn->Stride(nDim+1)/len; // number of OVERALL copy actions

  // as lines are copied, we need the stride from 2nd dim on
  SizeT retStride[MAXRANK+1];
  for( SizeT a=0; a <= nDim; ++a) retStride[a]=srcDim.Stride(a+1)/len;
    
  // a magic number, to reset destStart for this dimension
  SizeT resetStep[MAXRANK+1];
  for( SizeT a=1; a <= nDim; ++a) 
    resetStep[a]=(retStride[a]-1)/retStride[a-1]*this->dim.Stride(a);
	
  //  SizeT destStart=this->dim.LongIndex(ixDim); // starting pos

  // DataT& srcIn_dd = srcIn->dd; 

  SizeT srcIx=0; // this one simply runs from 0 to N_Elements(srcIn)
  for( SizeT c=1; c<=nCp; ++c) // linearized verison of nested loops
    {
      // copy one segment
      SizeT destEnd=destStart+len;
      for( SizeT destIx=destStart; destIx<destEnd; ++destIx)
	(*this)[destIx] = (*srcIn)[ srcIx++];

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

  SizeT nEl=srcArr->N_Elements();
  // number of copy actions
  SizeT nCp=nEl/len;

  // initial offset
  SizeT destStart= this->dim.Stride(atDim) * at; // dest array
  SizeT destEnd  = destStart + len;

  // number of elements to skip
  SizeT gap=this->dim.Stride(atDim+1);    // dest array

#ifdef _OPENMP
    #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
        #pragma omp for
        for( OMPInt c=0; c<nCp; ++c)
        {
            // copy one segment
            SizeT destStartLoop = destStart + c * gap;
            SizeT destEndLoop   = destStartLoop + len;
            SizeT srcIxLoop     = c * len;
            for( SizeT destIx=destStartLoop; destIx< destEndLoop; destIx++)
                (*this)[destIx] = (*srcArr)[ srcIxLoop++];

        }
    } // OMP
#else // #ifdef _OPENMP
    SizeT srcIx=0;
    for( SizeT c=0; c<nCp; ++c)
    {
        // copy one segment
        for( SizeT destIx=destStart; destIx< destEnd; destIx++)
            (*this)[destIx] = (*srcArr)[ srcIx++];

        // set new destination pointer
        destStart += gap;
        destEnd   += gap;
    }
#endif

  SizeT add=srcArr->dim[atDim]; // update 'at'
  at += (add > 1)? add : 1;
}

// Logical True
// integers
template<class Sp>
bool Data_<Sp>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0);
}
template<>
bool Data_<SpDFloat>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0.0f);
}
template<>
bool Data_<SpDDouble>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0.0);
}
template<>
bool Data_<SpDString>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != "");
}
template<>
bool Data_<SpDComplex>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (real(s) != 0.0 || imag(s) != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (real(s) != 0.0 || imag(s) != 0.0);
}
template<>
bool Data_<SpDPtr>::LogTrue()
{
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0);
}
template<>
bool Data_<SpDObj>::LogTrue()
{
  // ::_overloadIsTrue is handled in True()
  
  return this->True();
}
// structs are not allowed

// indexed version
// integers
template<class Sp>
bool Data_<Sp>::LogTrue(SizeT i)
{
  return ((*this)[i] != 0);
}
template<>
bool Data_<SpDFloat>::LogTrue(SizeT i)
{
  return ((*this)[i] != 0.0f);
}
template<>
bool Data_<SpDDouble>::LogTrue(SizeT i)
{
  return ((*this)[i] != 0.0);
}
template<>
bool Data_<SpDString>::LogTrue(SizeT i)
{
  return ((*this)[i] != "");
}
template<>
bool Data_<SpDComplex>::LogTrue(SizeT i)
{
  return ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::LogTrue(SizeT i)
{
  return ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0);
}
template<>
bool Data_<SpDPtr>::LogTrue(SizeT i)
{
  return (*this)[i] != 0;
}
template<>
bool Data_<SpDObj>::LogTrue(SizeT i)
{
  return (*this)[i] != 0;
}
// structs are not allowed
template<class Sp>
DByte* Data_<Sp>::TagWhere(SizeT& n)
{
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i) {
      DByte tmp=(dd[i]!=0);
      ixList[i]=tmp;
      count +=tmp;
    }
    }
  n = count;
  return ixList;    
}

template<>
DByte* Data_<SpDFloat>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i) {
      DByte tmp = (dd[i] != 0.0f);
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}

template<>
DByte* Data_<SpDDouble>::TagWhere(SizeT& n) {
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i) {
      DByte tmp = (dd[i] != 0.0);
      ixList[i] = tmp;
      count += tmp;
    }
  }
  n = count;
  return ixList;
}
template<>
DByte* Data_<SpDString>::TagWhere(SizeT& n)
{
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i) {
      DByte tmp=((*this)[i]!="");
      ixList[i]=tmp;
      count +=tmp;
    }
    }
  n = count;
  return ixList;    
}
template<>
DByte* Data_<SpDComplex>::TagWhere(SizeT& n)
{
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i) {
      DFloat re=(*this)[i].real();
      DFloat im=(*this)[i].imag();
      DByte tmp=(re!=0.0f || im!=0.0f);
      ixList[i]=tmp;
      count +=tmp;
    }
    }
  n = count;
  return ixList;    
}
template<>
DByte* Data_<SpDComplexDbl>::TagWhere(SizeT& n)
{
  SizeT nEl = N_Elements();
  DByte* ixList = new DByte[ nEl];
  SizeT count = 0;
#pragma omp parallel reduction(+:count) if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for 
    for (SizeT i = 0; i < nEl; ++i) {
      DDouble re=(*this)[i].real();
      DDouble im=(*this)[i].imag();
      DByte tmp=(re!=0.0 || im!=0.0);
      ixList[i]=tmp;
      count +=tmp;
    }
    }
  n = count;
  return ixList;    
}

// AC: see bug report #592 : it was found on 2014 March, 18,
// that WHERE was buggy due to OpenMP option
// when n_elements of input bigger than !cpu.TPOOL_MIN_ELTS
//
// for WHERE, integers, also ptr and object
template<class Sp>
DLong* Data_<Sp>::Where( bool comp, SizeT& n)
{
  SizeT nEl = N_Elements();
  DLong* ixList = new DLong[ nEl];
  SizeT count = 0;
  if( comp)
    {
      SizeT nIx = nEl;
      //  TRACEOMP( __FILE__, __LINE__)
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i] != 0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i] != 0)
	{
	  ixList[ count++] = i;
	}
    } // omp
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
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i] != 0.0f)
	    {
          ixList[ count++] = i;
        }
	  else
	    {
          ixList[ --nIx] = i;
        }
      }
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
//#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i] != 0.0f)
      {
	  ixList[ count++] = i;
      }
    } // omp
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
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i] != 0.0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i] != 0.0)
	{
	  ixList[ count++] = i;
	}
    } // omp
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
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i] != "")
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i] != "")
	{
	  ixList[ count++] = i;
	}
    } // omp
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
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
	{
	  ixList[ count++] = i;
	}
    } // omp
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
      //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  if( (*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
	    {
	      ixList[ count++] = i;
	    }
	  else
	    {
	      ixList[ --nIx] = i;
	    }
	}
    } // omp
    }
  else
    //#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      //#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      if( (*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0)
	{
	  ixList[ count++] = i;
	}
    } // omp
  n = count;
  return ixList;
}
// structs are not allowed

// SA: the start, stop, step & valIx arguments to MinMax are used when
//     user specifies the DIMENSION keyword to MIN/MAX; they all have
//     default values (defined in basegdl.hpp & datatypes.hpp) thus
//     their introduction should not affect previous calls to MinMax

// for use by MIN and MAX functions
// integer (NaN not an issue)
template<class Sp>
void Data_<Sp>::MinMax( DLong* minE, DLong* maxE, 
			BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
            SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();

  if (minE == NULL && minVal == NULL)
  {
    DLong maxEl = start;
    Ty maxV = (*this)[maxEl];
    if (useAbs) maxV = llabs(maxV);
    for (DLong i = start + step; i < stop; i += step)
    {
      if (useAbs)
      {
        if (llabs((*this)[i]) > maxV) maxV = llabs((*this)[maxEl = i]);
      } else
      {
        if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
      }
    }
    if (useAbs) maxV = (*this)[maxEl];
    if (maxE != NULL) *maxE = maxEl;
    if (maxVal != NULL)
    {
      if (valIx == -1) *maxVal = new Data_(maxV);
      else (*static_cast<Data_*> (*maxVal))[valIx] = maxV;
    }
    return;
  }
  if (maxE == NULL && maxVal == NULL)
  {
    DLong minEl = start;
    Ty minV = (*this)[minEl];
    if (useAbs) minV = llabs(minV);
    for (DLong i = start + step; i < stop; i += step)
    {
      if (useAbs)
      {
        if (llabs((*this)[i]) < minV) minV = llabs((*this)[minEl = i]);
      } else
      {
        if ((*this)[i] < minV) minV = (*this)[minEl = i];
      }
    }
    if (useAbs) minV = (*this)[minEl];
    if (minE != NULL) *minE = minEl;
    if (minVal != NULL)
    {
      if (valIx == -1) *minVal = new Data_(minV);
      else (*static_cast<Data_*> (*minVal))[valIx] = minV;
    }
    return;
  }

  DLong maxEl, minEl;
  Ty maxV, minV;
  maxV = minV = (*this)[maxEl = minEl = start];
  if (useAbs)
  {
    maxV = llabs(maxV);
    minV = llabs(maxV);
  }

  for (DLong i = start + step; i < stop; i += step)
  {
    if (useAbs)
    {
      if (llabs((*this)[i]) > maxV)
        maxV = llabs((*this)[maxEl = i]);
      if (llabs((*this)[i] < minV))
        minV = llabs((*this)[minEl = i]);
    } else
    {
      if ((*this)[i] > maxV)
        maxV = (*this)[maxEl = i];
      else if ((*this)[i] < minV)
        minV = (*this)[minEl = i];
    }
  }
  if (useAbs)
  {
    maxV = (*this)[maxEl];
    minV = (*this)[minEl];
  }
  if (maxE != NULL) *maxE = maxEl;
  if (maxVal != NULL)
  {
    if (valIx == -1) *maxVal = new Data_(maxV);
    else (*static_cast<Data_*> (*maxVal))[valIx] = maxV;
  }

  if (minE != NULL) *minE = minEl;
  if (minVal != NULL)
  {
    if (valIx == -1) *minVal = new Data_(minV);
    else (*static_cast<Data_*> (*minVal))[valIx] = minV;
  }
}

// the code for <SpDFloat>::MinMax is a "template" for Double, Complex and DoubleComplex ...
// Please note that IDL does not take care of order for {-Inf, Nan, Inf}
// when NaN is present with Inf and -Inf ONLY. We follow this rule ...
// (we took the last one)

template<>
void Data_<SpDFloat>::MinMax( DLong* minE, DLong* maxE, 
			      BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                  SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs) {
  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();

  if (minE == NULL && minVal == NULL)
  {
    DLong maxEl = start;
    Ty maxV = (*this)[maxEl];
    if (useAbs) maxV = fabsf(maxV);
    DLong i, i_min = start + step;

    if (omitNaN)
    {
      i = start;
      int flag = 1;
      while (flag == 1)
      {
        if (!std__isnan((*this)[i]) && isfinite((*this)[i])) flag = 0;
        if (i + step >= stop) flag = 0;
        i += step;
      }
      maxV = (*this)[maxEl = i - step];
      if (useAbs) maxV = fabsf(maxV);
      i_min = i;
    }

    for (i = i_min; i < stop; i += step)
    {
      if (omitNaN)
      {
        if (std__isnan((*this)[i]) || !isfinite((*this)[i])) continue;
      }
      if (useAbs)
      {
        if (fabsf((*this)[i]) > maxV) maxV = fabsf((*this)[maxEl = i]);
      } else
      {
        if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
      }
    }
    if (useAbs)
      maxV = (*this)[maxEl];
    if (maxE != NULL) *maxE = maxEl;
    if (maxVal != NULL)
    {
      if (valIx == -1) *maxVal = new Data_(maxV);
      else (*static_cast<Data_*> (*maxVal))[valIx] = maxV;
    }
    return;
  }
  if (maxE == NULL && maxVal == NULL)
  {
    DLong minEl = start;
    Ty minV = (*this)[minEl];
    if (useAbs) minV = fabsf(minV);
    DLong i, i_min = start + step;

    if (omitNaN)
    {
      i = start;
      int flag = 1;
      while (flag == 1)
      {
        if (!std__isnan((*this)[i]) && isfinite((*this)[i])) flag = 0;
        if (i + step >= stop) flag = 0;
        i += step;
      }
      minV = (*this)[minEl = i - step];
      if (useAbs) minV = fabsf(minV);
      i_min = i;
    }

    for (i = i_min; i < stop; i += step)
    {
      if (omitNaN)
      {
        if (std__isnan((*this)[i]) || !isfinite((*this)[i])) continue;
      }
      if (useAbs)
      {
        if (fabsf((*this)[i] < minV)) minV = fabsf((*this)[minEl = i]);
      } else
      {
        if ((*this)[i] < minV) minV = (*this)[minEl = i];
      }
    }
    if (useAbs) minV = (*this)[minEl];
    if (minE != NULL) *minE = minEl;
    if (minVal != NULL)
    {
      if (valIx == -1) *minVal = new Data_(minV);
      else (*static_cast<Data_*> (*minVal))[valIx] = minV;
    }
    return;
  }

  DLong maxEl, minEl;
  Ty maxV, minV;
  maxV = minV = (*this)[maxEl = minEl = start];
  if (useAbs)
  {
    maxV = fabsf(maxV);
    minV = fabsf(minV);
  }
  DLong i, i_min = start + step;

  if (omitNaN)
  {
    i = start;
    int flag = 1;
    while (flag == 1)
    {
      if (!std__isnan((*this)[i]) && isfinite((*this)[i])) flag = 0;
      if (i + step >= stop) flag = 0;
      i += step;
    }
    maxV = minV = (*this)[maxEl = minEl = i - step];
    if (useAbs)
    {
      maxV = fabsf(maxV);
      minV = fabsf(minV);
    }
    i_min = i;
  }

  for (i = i_min; i < stop; i += step)
  {
    if (omitNaN)
    {
      if (std__isnan((*this)[i]) || !isfinite((*this)[i])) continue;
    }
    if (useAbs)
    {
      if (fabsf((*this)[i]) > maxV) maxV = fabsf((*this)[maxEl = i]);
      if (fabsf((*this)[i]) < minV) minV = fabsf((*this)[minEl = i]);
    } else
    {
      if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
      else if ((*this)[i] < minV) minV = (*this)[minEl = i];
    }
  }
  if (useAbs)
  {
    maxV = (*this)[maxEl];
    minV = (*this)[minEl];
  }
  if (maxE != NULL) *maxE = maxEl;
  if (maxVal != NULL)
  {
    if (valIx == -1) *maxVal = new Data_(maxV);
    else (*static_cast<Data_*> (*maxVal))[valIx] = maxV;
  }

  if (minE != NULL) *minE = minEl;
  if (minVal != NULL)
  {
    if (valIx == -1) *minVal = new Data_(minV);
    else (*static_cast<Data_*> (*minVal))[valIx] = minV;
  }
}


template<>
void Data_<SpDDouble>::MinMax( DLong* minE, DLong* maxE, 
			       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                   SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();

  if (minE == NULL && minVal == NULL)
    {
      DLong maxEl  = start;
      Ty    maxV = (*this)[maxEl];
      if (useAbs) maxV = fabs(maxV);
      DLong i, i_min = start + step;

      if (omitNaN) {
	i = start;
	int flag = 1;
	while (flag == 1) {
	  if (!std__isnan((*this)[i]) && isfinite((*this)[i])) flag = 0;
	  if (i + step >= stop) flag = 0;
	  i += step;
	}
	maxV = (*this)[maxEl = i - step];
    if (useAbs) maxV = fabs(maxV);
	i_min = i;
      }
        
      for (i = i_min; i < stop; i += step) {
	if (omitNaN) {
	  if (std__isnan((*this)[i]) || !isfinite((*this)[i])) continue;
	}
    if (useAbs) {
      if (fabs((*this)[i]) > maxV) maxV = fabs((*this)[maxEl = i]);
    } else {
      if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
    }
      }
      if (useAbs) maxV = (*this)[maxEl];
      if (maxE != NULL) *maxE = maxEl;
      if (maxVal != NULL) 
	{
	  if (valIx == -1) *maxVal = new Data_( maxV);
	  else (*static_cast<Data_*>(*maxVal))[valIx] = maxV;
	}
      return;
    }
  if (maxE == NULL && maxVal == NULL)
    {
      DLong minEl = start;
      Ty    minV = (*this)[minEl];
      if (useAbs) minV = fabs(minV);
      DLong i, i_min = start + step;

      if (omitNaN) {
	i = start;
	int flag = 1;
	while (flag == 1) {
	  if (!std__isnan((*this)[i]) && isfinite((*this)[i])) flag = 0;
	  if (i + step >= stop) flag = 0;
	  i += step;
	}
	minV = (*this)[minEl = i - step];
    if (useAbs) minV = fabs(minV);
	i_min = i;
      }
   
      for (i = i_min; i < stop; i += step) {
	if (omitNaN) {
	  if (std__isnan((*this)[i]) || !isfinite((*this)[i])) continue;
	} 
    if (useAbs) {
      if (fabs((*this)[i]) < minV) minV = fabs((*this)[minEl = i]);
    } else {
      if ((*this)[i] < minV) minV = (*this)[minEl = i];
    }
      }
      if (useAbs) minV = (*this)[minEl];
      if (minE != NULL) *minE = minEl;
      if (minVal != NULL) 
	{
	  if (valIx == -1) *minVal = new Data_( minV);
	  else (*static_cast<Data_*>(*minVal))[valIx] = minV;
	}
      return;
    }
  
  DLong maxEl, minEl;
  Ty maxV, minV;
  maxV = minV = (*this)[maxEl = minEl = start];
  if (useAbs) {
    maxV = fabs(maxV);
    minV = fabs(minV);
  }
  DLong i, i_min = start + step;
  
  if (omitNaN) {
    i = start;
    int flag = 1;
    while (flag == 1) {
      if (!std__isnan((*this)[i]) && isfinite((*this)[i])) flag = 0;
      if (i + step >= stop) flag = 0;
      i += step;
    }
    minV = maxV = (*this)[minEl = maxEl = i - step];
    if (useAbs) {
      maxV = fabs(maxV);
      minV = fabs(minV);
    }
    i_min = i;
  }

  for (i = i_min; i < stop; i+= step) {
    if (omitNaN){
      if (std__isnan((*this)[i]) || !isfinite((*this)[i])) continue;
    }
    if (useAbs) {
      if (fabs((*this)[i]) > maxV) maxV = fabs((*this)[maxEl = i]);
      if( fabs((*this)[i]) < minV) minV = fabs((*this)[minEl = i]);
    } else {
      if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
      else if( (*this)[i] < minV) minV = (*this)[minEl = i];
    }
  }
  if (useAbs) {
     maxV = (*this)[maxEl];
     minV = (*this)[minEl];
  }
  if (maxE != NULL) *maxE = maxEl;
  if( maxVal != NULL) 
    {
      if (valIx == -1) *maxVal = new Data_( maxV);
      else (*static_cast<Data_*>(*maxVal))[valIx] = maxV;
    }
  
  if (minE != NULL) *minE = minEl;
  if (minVal != NULL) 
    {
      if (valIx == -1) *minVal = new Data_( minV);
      else (*static_cast<Data_*>(*minVal))[valIx] = minV;
    }
}

template<>
void Data_<SpDString>::MinMax( DLong* minE, DLong* maxE, 
			       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                   SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();

  if (minE == NULL && minVal == NULL)
    {
      DLong maxEl = start;
      Ty    maxV = (*this)[maxEl];
      for (DLong i = start + step; i < stop; i += step)
        if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
      if (maxE != NULL) *maxE = maxEl;
      if (maxVal != NULL) 
	{
	  if (valIx == -1) *maxVal = new Data_( maxV);
	  else (*static_cast<Data_*>(*maxVal))[valIx] = maxV;
	}
      return;
    }
  if( maxE == NULL && maxVal == NULL)
    {
      DLong minEl = start;
      Ty    minV = (*this)[minEl];
      for (DLong i = start + step; i < stop; i += step)
        if ((*this)[i] < minV) minV = (*this)[minEl = i];
      if (minE != NULL) *minE = minEl;
      if (minVal != NULL) 
	{
	  if (valIx == -1) *minVal = new Data_( minV);
	  else (*static_cast<Data_*>(*minVal))[valIx] = minV;
	}
      return;
    }

  DLong maxEl, minEl;
  Ty maxV, minV;
  maxV = minV = (*this)[maxEl = minEl = start];

  for (DLong i = start + step; i < stop; i += step)
    {
      if ((*this)[i] > maxV) maxV = (*this)[maxEl = i];
      else if((*this)[i] < minV) minV = (*this)[minEl = i];
    }
  if (maxE != NULL) *maxE = maxEl;
  if (maxVal != NULL) 
    {
      if (valIx == -1) *maxVal = new Data_( maxV);
      else (*static_cast<Data_*>(*maxVal))[valIx] = maxV;
    }

  if (minE != NULL) *minE = minEl;
  if (minVal != NULL) 
    {
      if (valIx == -1) *minVal = new Data_( minV);
      else (*static_cast<Data_*>(*minVal))[valIx] = minV;
    }
}

template<>
void Data_<SpDComplex>::MinMax( DLong* minE, DLong* maxE, 
				BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN, 
                SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();

  if (minE == NULL && minVal == NULL)
    {
      DLong maxEl = start;
      float maxV = (*this)[maxEl].real();
      if (useAbs) maxV = std::abs((*this)[maxEl]);
      DLong i, i_min = start + step;

      if (omitNaN) {
	i = start;
	int flag = 1;
	while (flag == 1) {
      if (useAbs) {
        if (!isnan((*this)[i].imag()) && isfinite((*this)[i].imag()) && !isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      } else {
        if (!isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      }
	  if (i + step >= stop) flag = 0;
	  i += step;
	}
	maxV = (*this)[maxEl = i - step].real();
    if (useAbs) maxV = std::abs((*this)[maxEl]);
	i_min = i;
      }
        
      for (i = i_min; i < stop; i += step) {
	if (omitNaN) {
	  if (isnan((*this)[i].real()) || !isfinite((*this)[i].real())) continue;
      if (useAbs) if (isnan((*this)[i].imag()) || !isfinite((*this)[i].imag())) continue;
    }
    if (useAbs) {
      if (std::abs((*this)[i]) > maxV) maxV = std::abs((*this)[maxEl = i]);
    } else {
      if ((*this)[i].real() > maxV) maxV = (*this)[maxEl = i].real();
    }
      }
      if (maxE != NULL) *maxE = maxEl;
      if (maxVal != NULL) 
	{
	  if (valIx == -1) *maxVal = new Data_( (*this)[ maxEl]);
      else (*static_cast<Data_*>(*maxVal))[valIx] = (*this)[ maxEl]; //maxV;
	}
      return;
    }
  if (maxE == NULL && maxVal == NULL)
    {
      DLong minEl  = start;
      float minV = (*this)[minEl].real();
      if (useAbs) minV = std::abs((*this)[minEl]);
      DLong i, i_min = start + step;

      if (omitNaN) {
	i = start;
	int flag = 1;
	while (flag == 1) {
      if (useAbs) {
        if (!isnan((*this)[i].imag()) && isfinite((*this)[i].imag()) && !isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      } else {
        if (!isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      }
      if (i + step >= stop) flag = 0;
	  i += step;
	}
	minV = (*this)[minEl = i - step].real();
    if (useAbs) minV = std::abs((*this)[minEl]);
	i_min = i;
      }
   
      for (i = i_min; i < stop; i += step) {
	if (omitNaN) {
	  if (isnan((*this)[i].real()) || !isfinite((*this)[i].real())) continue;
      if (useAbs) if (isnan((*this)[i].imag()) || !isfinite((*this)[i].imag())) continue;
    }
    if (useAbs) {
      if (std::abs((*this)[i]) < minV) minV = std::abs((*this)[minEl = i]);
    } else {
      if ((*this)[i].real() < minV) minV = (*this)[minEl = i].real();
    }
      }
      if (minE != NULL) *minE = minEl;
      if (minVal != NULL) 
	{
	  if (valIx == -1) *minVal = new Data_( (*this)[ minEl]);
      else (*static_cast<Data_*>(*minVal))[valIx] = (*this)[ minEl]; //minV;
	}
      return;
    }
  
  DLong maxEl, minEl;
  float maxV, minV;
  maxV = minV = (*this)[maxEl = minEl = start].real();
  if (useAbs) {
    maxV = minV = std::abs((*this)[maxEl]);
  }
  DLong i, i_min = start + step;
  
  if (omitNaN) {
    i = start;
    int flag = 1;
    while (flag == 1) {
      if (useAbs) {
        if (!isnan((*this)[i].imag()) && isfinite((*this)[i].imag()) && !isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      } else {
        if (!isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      }
      if (i + step >= stop) flag = 0;
      i += step;
    }
    minV = maxV = (*this)[minEl = maxEl = i - step].real();
    if (useAbs) {
      maxV = minV = std::abs((*this)[maxEl]);
    }
    i_min = i;
  }

  for (i = i_min; i < stop; i += step) {
    if (omitNaN){
      if (isnan((*this)[i].real()) || !isfinite((*this)[i].real())) continue;
      if (useAbs) if (isnan((*this)[i].imag()) || !isfinite((*this)[i].imag())) continue;
    }
    if (useAbs) {
      if (std::abs((*this)[i]) > maxV) maxV = std::abs((*this)[maxEl = i]);
       if( std::abs((*this)[i]) < minV) minV = std::abs((*this)[minEl = i]);
    } else {
      if ((*this)[i].real() > maxV) maxV = (*this)[maxEl = i].real();
      else if( (*this)[i].real() < minV) minV = (*this)[minEl = i].real();
    }
  }
  if (maxE != NULL) *maxE = maxEl;
  if (maxVal != NULL) 
    {
      if (valIx == -1) *maxVal = new Data_( (*this)[ maxEl]);
      else (*static_cast<Data_*>(*maxVal))[valIx] = (*this)[maxEl]; //maxV;
    }
  
  if (minE != NULL) *minE = minEl;
  if (minVal != NULL)
    {
      if (valIx == -1) *minVal = new Data_( (*this)[ minEl]);
      else (*static_cast<Data_*>(*minVal))[valIx] = (*this)[minEl]; //minV;
    }

}

template<>
void Data_<SpDComplexDbl>::MinMax( DLong* minE, DLong* maxE, 
				   BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
                   SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs)
{
  // default: start = 0, stop = 0, step = 1, valIx = -1
  if (stop == 0) stop = dd.size();

  if (minE == NULL && minVal == NULL)
    {
      DLong maxEl = start;
      double maxV = (*this)[maxEl].real();
      if (useAbs) maxV = std::abs((*this)[maxEl]);
      DLong i, i_min = start + step;

      if (omitNaN) {
	i = start;
	int flag = 1;
	while (flag == 1) {
      if (useAbs) {
        if (!std__isnan((*this)[i].imag()) && isfinite((*this)[i].imag()) && !std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      } else {
        if (!std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      }
//	  if (!std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
	  if (i + step >= stop) flag = 0;
	  i += step;
	}
	maxV = (*this)[maxEl = i - step].real();
    if (useAbs) maxV = std::abs((*this)[maxEl]);
	i_min = i;
      }
        
      for (i = i_min; i < stop; i += step) {
	if (omitNaN) {
	  if (std__isnan((*this)[i].real()) || !isfinite((*this)[i].real())) continue;
      if (useAbs) if (std__isnan((*this)[i].imag()) || !isfinite((*this)[i].imag())) continue;
	}
    if (useAbs) {
      if (std::abs((*this)[i]) > maxV) maxV = std::abs((*this)[maxEl = i]);
    } else {
      if ((*this)[i].real() > maxV) maxV = (*this)[maxEl = i].real();
    }
      }
      if (maxE != NULL) *maxE = maxEl;
      if (maxVal != NULL) 
	{
	  if (valIx == -1) *maxVal = new Data_( (*this)[ maxEl]);
      else (*static_cast<Data_*>(*maxVal))[valIx] = (*this)[ maxEl]; //maxV;
	}
      return;
    }
  if( maxE == NULL && maxVal == NULL)
    {
      DLong minEl = start;
      double minV = (*this)[minEl].real();
      if (useAbs) minV = std::abs((*this)[minEl]);
      DLong i, i_min = start + step;

      if (omitNaN) {
	i = start;
	int flag = 1;
	while (flag == 1) {
      if (useAbs) {
        if (!std__isnan((*this)[i].imag()) && isfinite((*this)[i].imag()) && !std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      } else {
        if (!std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      }
//	  if (!std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
	  if (i + step >= stop) flag = 0;
	  i += step;
	}
	minV = (*this)[minEl = i - step].real();
    if (useAbs) minV = std::abs((*this)[minEl]);
	i_min = i;
      }
   
      for (i = i_min; i < stop; i += step) {
	if (omitNaN) {
	  if (std__isnan((*this)[i].real()) || !isfinite((*this)[i].real())) continue;
      if (useAbs) if (std__isnan((*this)[i].imag()) || !isfinite((*this)[i].imag())) continue;
	} 
    if (useAbs) {
       if (std::abs((*this)[i]) < minV) minV = std::abs((*this)[minEl = i]);
    } else {
      if ((*this)[i].real() < minV) minV = (*this)[minEl = i].real();
    }
      }
      if (minE != NULL) *minE = minEl;
      if (minVal != NULL) 
	{
	  if (valIx == -1) *minVal = new Data_( (*this)[ minEl]);
      else (*static_cast<Data_*>(*minVal))[valIx] = (*this)[ minEl]; //minV;
	}
      return;
    }
  
  DLong maxEl, minEl;
  double maxV, minV;
  minV = maxV = (*this)[minEl = maxEl = start].real();
  if (useAbs) {
    maxV = minV = std::abs((*this)[maxEl]);
  }
  DLong i, i_min = start + step;
  
  if (omitNaN) {
    i = start;
    int flag = 1;
    while (flag == 1) {
      if (useAbs) {
        if (!std__isnan((*this)[i].imag()) && isfinite((*this)[i].imag()) && !std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      } else {
        if (!std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      }
//      if (!std__isnan((*this)[i].real()) && isfinite((*this)[i].real())) flag = 0;
      if (i + step >= stop) flag = 0;
      i += step;
    }
    minV = maxV = (*this)[minEl = maxEl = i - step].real();
    if (useAbs) {
      maxV = minV = std::abs((*this)[maxEl]);
    }
    i_min = i;
  }

  for (i = i_min; i < stop; i += step) {
    if (omitNaN){
      if (std__isnan((*this)[i].real()) || !isfinite((*this)[i].real())) continue;
      if (useAbs) if (std__isnan((*this)[i].imag()) || !isfinite((*this)[i].imag())) continue;
    }
    if (useAbs) {
      if (std::abs((*this)[i]) > maxV) maxV = std::abs((*this)[maxEl = i]);
      if( std::abs((*this)[i]) < minV) minV = std::abs((*this)[minEl = i]);
    } else {
      if ((*this)[i].real() > maxV) maxV = (*this)[maxEl = i].real();
      else if( (*this)[i].real() < minV) minV = (*this)[minEl = i].real();
    }
  }
  if (maxE != NULL) *maxE = maxEl;
  if (maxVal != NULL) 
    {
      if (valIx == -1) *maxVal = new Data_( (*this)[ maxEl]);
      else (*static_cast<Data_*>(*maxVal))[valIx] = (*this)[maxEl]; //maxV;
    }
  
  if (minE != NULL) *minE = minEl;
  if (minVal != NULL) 
    {
      if (valIx == -1) *minVal = new Data_( (*this)[ minEl]);
      else (*static_cast<Data_*>(*minVal))[valIx] = (*this)[minEl]; //minV;
    }

}


template<>
BaseGDL* Data_<SpDString>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias,
 				   bool center, bool normalize, int edgeMode,
                                   bool doNan, BaseGDL* missing, bool doMissing,
                                   BaseGDL* invalid, bool doInvalid)
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDObj>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias,
 				bool center, bool normalize, int edgeMode,
                                bool doNan, BaseGDL* missing, bool doMissing,
                                BaseGDL* invalid, bool doInvalid)
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDPtr>::Convol( BaseGDL* kIn, BaseGDL* scaleIn,BaseGDL* bias,
 				bool center, bool normalize, int edgeMode,
                                bool doNan, BaseGDL* missing, bool doMissing,
                                BaseGDL* invalid, bool doInvalid)
{
  throw GDLException("Pointer expression not allowed in this context.");
}

#define INCLUDE_CONVOL_CPP 1
#define CONVOL_BYTE__

#include "convol.cpp"

#undef CONVOL_BYTE__
#define CONVOL_UINT__

#include "convol.cpp"

#undef CONVOL_UINT__
#define CONVOL_INT__

#include "convol.cpp"

#undef CONVOL_INT__
#define CONVOL_ULONG__

#include "convol.cpp"

#undef CONVOL_ULONG__
#define CONVOL_ULONG64__

#include "convol.cpp"

#undef CONVOL_ULONG64__

#include "convol.cpp"

//compute index when srcDim of rank rank is tranposed by [1,2,3...,0]. destStride must have been computed externally (accelerator).
inline static SizeT transposed1Index(const SizeT inputindex, const SizeT* srcDim, const SizeT* destStride, const long rank)
{
  SizeT dim[MAXRANK];
  SizeT index=inputindex;
  SizeT sizeleft = index;
  SizeT dim0;
  for (SizeT i = 0; i < rank; ++i) {
    dim0= srcDim[i];
    sizeleft /= dim0;
    dim[i] = index - sizeleft * dim0; //the corresponding index in src data.
    index = sizeleft;
  }
  SizeT ix = 0;
  for (SizeT i = 0; i < rank-1; ++i) ix += dim[i+1] * destStride[i]; ix+=dim[0]*destStride[rank-1]; //note dim has been rotated by 1

  return ix;
}
#define INCLUDE_SMOOTH_POLYD
template<typename T>
void SmoothPolyD(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
 #include "smoothPolyD.hpp" 
}
//subset having edges
#define USE_EDGE
template<typename T>
void SmoothPolyDWrap(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_WRAP
#include "smoothPolyD.hpp" 
#undef EDGE_WRAP
}
template<typename T>
void SmoothPolyDTruncate(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_TRUNCATE
#include "smoothPolyD.hpp" 
#undef EDGE_TRUNCATE
}
template<typename T>
void SmoothPolyDZero(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_ZERO
#include "smoothPolyD.hpp" 
#undef EDGE_ZERO
}
template<typename T>
void SmoothPolyDMirror(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_MIRROR
#include "smoothPolyD.hpp" 
#undef EDGE_MIRROR
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_POLYD

#define INCLUDE_SMOOTH_POLYD_NAN
template<typename T>
void SmoothPolyDNan(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
 #include "smoothPolyDnans.hpp" 
}
//subset having edges
#define USE_EDGE
template<typename T>
void SmoothPolyDWrapNan(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_WRAP
#include "smoothPolyDnans.hpp" 
#undef EDGE_WRAP
}
template<typename T>
void SmoothPolyDTruncateNan(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_TRUNCATE
#include "smoothPolyDnans.hpp" 
#undef EDGE_TRUNCATE
}
template<typename T>
void SmoothPolyDZeroNan(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_ZERO
#include "smoothPolyDnans.hpp" 
#undef EDGE_ZERO
}
template<typename T>
void SmoothPolyDMirrorNan(T* srcIn, T* destIn, const SizeT* datainDim, const int rank, const DLong* width) {
#define EDGE_MIRROR
#include "smoothPolyDnans.hpp" 
#undef EDGE_MIRROR
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_POLYD_NAN


#define INCLUDE_SMOOTH_1D
template<typename T>
void Smooth1D(T* data, T* res, SizeT dimx, SizeT w) {
#include "smooth1d.hpp"
}
//subset having edges
#define USE_EDGE
template<typename T>
void Smooth1DWrap(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_WRAP
#include "smooth1d.hpp"
#undef EDGE_WRAP
}
template<typename T>
void Smooth1DTruncate(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_TRUNCATE
#include "smooth1d.hpp"
#undef EDGE_TRUNCATE
}
template<typename T>
void Smooth1DMirror(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_MIRROR
#include "smooth1d.hpp"
#undef EDGE_MIRROR
}
template<typename T>
void Smooth1DZero(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_ZERO
#include "smooth1d.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_1D

//smooth 1d functions for Nans.
#define INCLUDE_SMOOTH_1D_NAN
template<typename T>
void Smooth1DNan(T* data, T* res, SizeT dimx, SizeT w) {
#include "smooth1dnans.hpp"
}
//subset having edges
#define USE_EDGE
template<typename T>
void Smooth1DWrapNan(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_WRAP
#include "smooth1dnans.hpp"
#undef EDGE_WRAP
}
template<typename T>
void Smooth1DTruncateNan(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_TRUNCATE
#include "smooth1dnans.hpp"
#undef EDGE_TRUNCATE
}
template<typename T>
void Smooth1DMirrorNan(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_MIRROR
#include "smooth1dnans.hpp"
#undef EDGE_MIRROR
}
template<typename T>
void Smooth1DZeroNan(T* data, T* res, SizeT dimx, SizeT w) {
#define EDGE_ZERO
#include "smooth1dnans.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_1D_NAN

//smooth 2d functions.
#define INCLUDE_SMOOTH_2D
template<typename T>
void Smooth2D(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#include "smooth2d.hpp"
}
//subset having edges
#define USE_EDGE
template<typename T>
void Smooth2DWrap(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_WRAP
#include "smooth2d.hpp"
#undef EDGE_WRAP
}
template<typename T>
void Smooth2DTruncate(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_TRUNCATE
#include "smooth2d.hpp"
#undef EDGE_TRUNCATE
}
template<typename T>
void Smooth2DMirror(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_MIRROR
#include "smooth2d.hpp"
#undef EDGE_MIRROR
}
template<typename T>
void Smooth2DZero(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_ZERO
#include "smooth2d.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_2D

//smooth 2d functions for Nans.
#define INCLUDE_SMOOTH_2D_NAN
template<typename T>
void Smooth2DNan(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#include "smooth2dnans.hpp"
}
//subset having edges
#define USE_EDGE
template<typename T>
void Smooth2DWrapNan(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_WRAP
#include "smooth2dnans.hpp"
#undef EDGE_WRAP
}
template<typename T>
void Smooth2DTruncateNan(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_TRUNCATE
#include "smooth2dnans.hpp"
#undef EDGE_TRUNCATE
}
template<typename T>
void Smooth2DMirrorNan(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_MIRROR
#include "smooth2dnans.hpp"
#undef EDGE_MIRROR
}
template<typename T>
void Smooth2DZeroNan(const T* src, T* dest, const SizeT dimx, const SizeT dimy, const DLong* width) {
#define EDGE_ZERO
#include "smooth2dnans.hpp"
#undef EDGE_ZERO
}
#undef USE_EDGE
#undef INCLUDE_SMOOTH_2D_NAN

//Note: Values for ULong types return differently as IDL, but it should be proven that IDL is right...
template<class Sp>
BaseGDL* Data_<Sp>::Smooth(DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  SizeT nA = N_Elements();
  
  SizeT srcRank = this->Rank();
  
  BaseGDL* data = this;
  BaseGDL* res = this->Dup();
  
  SizeT sum = 0;
  SizeT mydims[srcRank]; //memory of dimensions, done one after the other
  for (int i = 0; i < srcRank; ++i) 
  {
    mydims[i] = this->Dim(i);
    sum += width[i];
  }
  if (sum == srcRank) return res;
  
  DUInt rotate1[srcRank]; //turntable transposition index list
  
  //doNan only meaningful for real and double (complex are real and double here)
  doNan=(doNan && (this->Type()==GDL_FLOAT ||this->Type()==GDL_DOUBLE));

  for (int i = 0; i < srcRank-1; ++i) rotate1[i] = i+1; rotate1[srcRank-1]=0; //[1,2,...,0] 
 
  if (doNan) {
    if (srcRank==1) {
      SizeT dimx = nA;
      SizeT w = width[0] / 2;
      if (edgeMode==0){
        Smooth1DNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==1) {
        Smooth1DWrapNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==2) {
        Smooth1DTruncateNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==3) {
        Smooth1DZeroNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==4) {
        Smooth1DMirrorNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      }
    } else if (srcRank==2) {
      SizeT dimx = data->Dim(0);
      SizeT dimy = data->Dim(1);
      if (edgeMode==0){
        Smooth2DNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==1) {
        Smooth2DWrapNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==2) {
        Smooth2DTruncateNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==3) {
        Smooth2DZeroNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==4) {
        Smooth2DMirrorNan((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      }
    } else {
      long rank = data->Rank();
      SizeT srcDim[MAXRANK];
      for (int i = 0; i < rank; ++i) srcDim[i] = data->Dim()[i];
      BaseGDL* dataw = this->Dup();
      if (edgeMode==0){
        SmoothPolyDNan((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==1) {
        SmoothPolyDWrapNan((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==2) {
        SmoothPolyDTruncateNan((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==3) {
        SmoothPolyDZeroNan((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==4) {
        SmoothPolyDMirrorNan((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      }
      GDLDelete(dataw);
    }
   //additional loop to replace left Nans by missing, if nans are left anyway
    if (gdlValid(missingValue))  {
      Ty* resty=(Ty*)res->DataAddr();
#pragma omp for 
      for (SizeT i=0; i<nA; ++i) if (!gdlValid(resty[i])) resty[i]=missingValue;
      }
  } else {
    if (srcRank==1) {
      SizeT dimx = nA;
      SizeT w = width[0] / 2;
      if (edgeMode==0){
        Smooth1D((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==1) {
        Smooth1DWrap((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==2) {
        Smooth1DTruncate((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==3) {
        Smooth1DZero((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      } else if (edgeMode==4) {
        Smooth1DMirror((Ty*)data->DataAddr(), (Ty*)res->DataAddr(),  dimx, w);
      }
    } else if (srcRank==2) {
      SizeT dimx = data->Dim(0);
      SizeT dimy = data->Dim(1);
      if (edgeMode==0){
        Smooth2D((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==1) {
        Smooth2DWrap((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==2) {
        Smooth2DTruncate((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==3) {
        Smooth2DZero((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      } else if (edgeMode==4) {
        Smooth2DMirror((Ty*)data->DataAddr(), (Ty*)res->DataAddr(), dimx, dimy,  width);
      }
    } else  {
      long rank = data->Rank();
      SizeT srcDim[MAXRANK];
      for (int i = 0; i < rank; ++i) srcDim[i] = data->Dim()[i];
      BaseGDL* dataw = this->Dup();
      if (edgeMode==0){
        SmoothPolyD((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==1) {
        SmoothPolyDWrap((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==2) {
        SmoothPolyDTruncate((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==3) {
        SmoothPolyDZero((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      } else if (edgeMode==4) {
        SmoothPolyDMirror((Ty*)dataw->DataAddr(), (Ty*)res->DataAddr(), srcDim, rank, width);
      }
      GDLDelete(dataw);
    }
  }
  return res;
}

template<>
BaseGDL* Data_<SpDString>::Smooth( DLong* width, int edgeMode,
                                   bool doNan, BaseGDL* missing)
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDObj>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDPtr>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  DDoubleGDL* missr=new DDoubleGDL(missingValue.real());
  DDoubleGDL* missi=new DDoubleGDL(missingValue.imag());
  Data_* res = this->Dup();
  DDoubleGDL* re=new DDoubleGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*re)[i]=(*this)[i].real();
  BaseGDL* resr=re->Smooth(width, edgeMode, doNan, missr);
  DDoubleGDL* im=new DDoubleGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*im)[i]=(*this)[i].imag();
  BaseGDL* resi=im->Smooth(width, edgeMode, doNan, missi);
  DDouble* dresi=(DDouble*)resi->DataAddr();  
  DDouble* dresr=(DDouble*)resr->DataAddr();  
  for (SizeT i=0; i< this->N_Elements(); ++i) (*res)[i]=std::complex<DDouble>(dresr[i], dresr[i]);
  GDLDelete (resr);
  GDLDelete (re);
  GDLDelete (missr);
  GDLDelete (resi);
  GDLDelete (im);
  GDLDelete (missi);
  return res;
}
template<>
BaseGDL* Data_<SpDComplex>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  DFloatGDL* missr=new DFloatGDL(missingValue.real());
  DFloatGDL* missi=new DFloatGDL(missingValue.imag());
  Data_* res = this->Dup();
  DFloatGDL* re=new DFloatGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*re)[i]=(*this)[i].real();
  BaseGDL* resr=re->Smooth(width, edgeMode, doNan, missr);
  DFloatGDL* im=new DFloatGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*im)[i]=(*this)[i].imag();
  BaseGDL* resi=im->Smooth(width, edgeMode, doNan, missi);
  DFloat* fresi=(DFloat*)resi->DataAddr();  
  DFloat* fresr=(DFloat*)resr->DataAddr();  
  for (SizeT i=0; i< this->N_Elements(); ++i) (*res)[i]=std::complex<DFloat>(fresr[i], fresr[i]);
  GDLDelete (resr);
  GDLDelete (re);
  GDLDelete (missr);
  GDLDelete (resi);
  GDLDelete (im);
  GDLDelete (missi);
  return res;
}

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

  destDim.SetOneDim( dimIx, newDim);

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
	  /*#pragma omp parallel if (resEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= resEl))
	    {
	    #pragma omp for*/
	  for( int r=0; r < resEl; ++r)
	    (*res)[ r] /= ratio;
	  // }
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

  destDim.SetOneDim( dimIx, newDim);

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
	
	if( actIn != this) GDLDelete(actIn);
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  this->dim[d])
      { // expand
	
	Data_* act = Rebin1( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) GDLDelete(actIn);
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
	
	if( actIn != this) GDLDelete(actIn);
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DByteGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) GDLDelete(actIn);
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
	
	if( actIn != this) GDLDelete(actIn);
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DIntGDL, DLong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) GDLDelete(actIn);
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
	
	if( actIn != this) GDLDelete(actIn);
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DUIntGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) GDLDelete(actIn);
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
	
	if( actIn != this) GDLDelete(actIn);
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DLongGDL, DLong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) GDLDelete(actIn);
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
	
	if( actIn != this) GDLDelete(actIn);
	actIn = act;
      }

  // 2nd expand
  for( SizeT d=0; d<nDim; ++d)
    if( newDim[ d] >  dim[d])
      { // expand
	
	Data_* act = Rebin1Int<DULongGDL, DULong64>( actIn, actDim, d, newDim[d], sample);
	actDim = act->Dim();
	
	if( actIn != this) GDLDelete(actIn);
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
  Data_* srcT; // = dynamic_cast<Data_*>( src);

  Guard< Data_> srcTGuard;
  if( src->Type() != Data_::t) 
    {
      srcT = static_cast<Data_*>( src->Convert2( Data_::t, BaseGDL::COPY));
      srcTGuard.Init( srcT);
    }
  else
  {
    srcT = static_cast<Data_*>( src);
  }
  /*#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
    #pragma omp for*/
  for(long k=0; k < nEl; ++k)
    {
      (*this)[ k] = (*srcT)[ k];
    }//    }
}





// return a new type of itself (only for one dimensional case)
template<class Sp>
BaseGDL* Data_<Sp>::NewIx( SizeT ix)
{
  return new Data_( (*this)[ ix]);
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( AllIxBaseT* ix, const dimension* dIn)
{
  SizeT nCp = ix->size();
  Data_* res=Data_::New( *dIn, BaseGDL::NOZERO);
  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
    {
    #pragma omp for*/
  for( int c=0; c<nCp; ++c)
    (*res)[c]=(*this)[ (*ix)[ c]];
  // }
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFrom( SizeT s)
{
  SizeT nCp = dd.size() - s;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
    {
    #pragma omp for*/
  for( int c=0; c<nCp; ++c)
    (*res)[c]=(*this)[ s+c];
  // }
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFrom( SizeT s, SizeT e)
{
  SizeT nCp = e - s + 1;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  /*#pragma omp parallel if (nCp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCp))
    {
    #pragma omp for*/
  for( int c=0; c<nCp; ++c)
    (*res)[c]=(*this)[ s+c];
  // }
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFromStride( SizeT s, SizeT stride)
{
  SizeT nCp = (dd.size() - s + stride - 1)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    (*res)[c]=(*this)[ s];
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{
  SizeT nCp = (e - s + stride)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)
    (*res)[c]=(*this)[ s];
  return res;
}



template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( BaseGDL* ix, bool strict)
{
  assert( ix->Type() != GDL_UNDEF);

  // no type checking needed here: GetAsIndex() will fail with grace
  //     int typeCheck = DTypeOrder[ dType];
  // 	if( typeCheck >= 100)
  // 	  throw GDLException("Type "+ix->TypeStr()+" not allowed as subscript.");
  
  SizeT nElem = ix->N_Elements();

  Data_* res = New( ix->Dim(), BaseGDL::NOZERO);
  Guard<Data_> guard( res);

  SizeT upper = dd.size() - 1;
  Ty    upperVal = (*this)[ upper];
  if( strict)
    {
      for(SizeT i = 0 ; i < nElem; ++i)
	{
	  SizeT actIx = ix->GetAsIndexStrict( i);
	  if( actIx > upper)
	    throw GDLException("Array used to subscript array "
			       "contains out of range (>) subscript (at index: "+i2s(i)+").");
	  (*res)[i]= (*this)[ actIx];
	}
    }
  else // not strict
    {
      for(SizeT i = 0 ; i < nElem; ++i)
	{
	  SizeT actIx = ix->GetAsIndex( i);
	  if( actIx >= upper)
	    (*res)[i] = upperVal;
	  else
	    (*res)[i]= (*this)[ actIx];
	}
    }
  return guard.release();
}


// unsigned types
template<class Sp> SizeT Data_<Sp>::GetAsIndex( SizeT i) const
{
  return (*this)[ i];
}
template<class Sp> SizeT Data_<Sp>::GetAsIndexStrict( SizeT i) const
{
  return (*this)[ i]; // good for unsigned types
}

template<>
SizeT Data_<SpDInt>::GetAsIndex( SizeT i) const
{
  if( (*this)[i] < 0)
    return 0;
  return (*this)[i];
}	
template<>
SizeT Data_<SpDInt>::GetAsIndexStrict( SizeT i) const
{
  if( (*this)[i] < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong>::GetAsIndex( SizeT i) const
{
  if( (*this)[i] < 0)
    return 0;
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong>::GetAsIndexStrict( SizeT i) const
{
  if( (*this)[i] < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong64>::GetAsIndex( SizeT i) const
{
  if( (*this)[i] < 0)
    return 0;
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong64>::GetAsIndexStrict( SizeT i) const
{
  if( (*this)[i] < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  return (*this)[i];
}	
template<>
SizeT Data_<SpDFloat>::GetAsIndex( SizeT i) const
{
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,float>((*this)[i]);
}	
template<>
SizeT Data_<SpDFloat>::GetAsIndexStrict( SizeT i) const
{
  if( (*this)[i] <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,float>((*this)[i]);
}	
template<>
SizeT Data_<SpDDouble>::GetAsIndex( SizeT i) const
{
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,double>((*this)[i]);
}	
template<>
SizeT Data_<SpDDouble>::GetAsIndexStrict( SizeT i) const
{
  if( (*this)[i] <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,double>((*this)[i]);
}	
template<>
SizeT Data_<SpDString>::GetAsIndex( SizeT i) const
{
  const char* cStart=(*this)[i].c_str();
  char* cEnd;
  long l=strtol(cStart,&cEnd,10);
  if( cEnd == cStart)
    {
      Warning("Type conversion error: Unable to convert given STRING to LONG (at index: " + i2s(i) + ")");
      return 0;
    }
  if( l < 0)
    return 0;
  return l;
}	
template<>
SizeT Data_<SpDString>::GetAsIndexStrict( SizeT i) const
{
  const char* cStart=(*this)[i].c_str();
  char* cEnd;
  long l=strtol(cStart,&cEnd,10);
  if( cEnd == cStart)
    {
      Warning("Type conversion error: Unable to convert given STRING to LONG (at index: " + i2s(i) + ")");
      return 0;
    }
  if( l < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript.",true,false);
  return l;
}	

template<>
SizeT Data_<SpDComplex>::GetAsIndex( SizeT i) const
{
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,float>(real((*this)[i]));
}	
template<>
SizeT Data_<SpDComplex>::GetAsIndexStrict( SizeT i) const
{
  if( real((*this)[i]) <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,float>(real((*this)[i]));
}	
template<>
SizeT Data_<SpDComplexDbl>::GetAsIndex( SizeT i) const
{
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,double>(real((*this)[i]));
}	
template<>
SizeT Data_<SpDComplexDbl>::GetAsIndexStrict( SizeT i) const
{
  if( real((*this)[i]) <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,double>(real((*this)[i]));
}	

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

