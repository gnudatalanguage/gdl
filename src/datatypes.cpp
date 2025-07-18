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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

//#include "datatypes.hpp" // for friend declaration
#include "dinterpreter.hpp"

// needed with gcc-3.3.2
#include <cassert>

#define INCLUDE_DATATYPESREF_CPP 1
#include "datatypesref.cpp"

#ifdef USE_EIGEN  
#if EIGEN_VERSION_AT_LEAST(3,3,7) //my version atthis time, works.
#include<unsupported/Eigen/CXX11/Tensor>
#define EIGEN_HAS_TENSOR
#endif
#endif

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
  // this assert is WRONG. bytes is just for call compatibility as we alloc sizeof( Data_), and 'bytes' cans be set by an externally compiled
  // function. Example: POWELL is now a DLM, will crash if this assert is present beacuse for some reason at the moment of
  // runtime new DDoubleGDL(res) at line 2729 in brent.cpp the 'new' does not pass the true size of DDoubleGDL but the 'new' does allocate the true size.
//  assert( bytes == sizeof( Data_));

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
template<class Sp> Data_<Sp>::~Data_() { }
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
template<class Sp> Data_<Sp>::Data_(): Sp(), dd() { }

// scalar
template<class Sp> Data_<Sp>::Data_(const Ty& d_): Sp(), dd(d_)
{ }
// template<> Data_<SpDPtr>::Data_(const Ty& d_): SpDPtr(), dd(d_)
// { GDLInterpreter::IncRef(d_);}
// template<> Data_<SpDObj>::Data_(const Ty& d_): SpDObj(), dd(d_)
// { GDLInterpreter::IncRefObj(d_);}

// new array, zero fields
template<class Sp> Data_<Sp>::Data_(const dimension& dim_): 
  Sp( dim_), dd( Sp::zero, this->dim.NDimElements())
{ 
  this->dim.Purge();
}

// new one-dim array from Ty*
template<class Sp> Data_<Sp>::Data_( const Ty* p, const SizeT nEl): 
  Sp( dimension( nEl)), dd( p, nEl)
{ }
template<> Data_<SpDPtr>::Data_( const Ty* p, const SizeT nEl):
  SpDPtr( dimension( nEl)), dd( p, nEl)
{ GDLInterpreter::IncRef(this);}
template<> Data_<SpDObj>::Data_( const Ty* p, const SizeT nEl):
  SpDObj( dimension( nEl)), dd( p, nEl)
{ GDLInterpreter::IncRefObj(this);}

// c-i 
// template<class Sp> Data_<Sp>::Data_(const Data_& d_): 
// Sp(d_.dim), dd(d_.dd) { }

template<class Sp> Data_<Sp>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble off, DDouble inc):
  Sp( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{ 
  this->dim.Purge();
  
  if (iT == BaseGDL::NOZERO) return;  //very frequent
  
  if (iT == BaseGDL::ZERO) { //rather frequent
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    }
  }
  
  else if (iT == BaseGDL::INDGEN) { //less frequent
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      if (off==0 && inc==1) { 
         for (SizeT i = 0; i < sz; ++i) (*this)[i]=i;
      } else {
         for (SizeT i = 0; i < sz; ++i) (*this)[i]=off+i*inc;
      }
    } else {
      if (off==0 && inc==1) { 
	TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
         for (SizeT i = 0; i < sz; ++i) (*this)[i]=i;
      } else {
	TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
         for (SizeT i = 0; i < sz; ++i) (*this)[i]=off+i*inc;
      }
    }
  }
}

//IDL uses floats increments and offset for floats and complex (normal) .
template<> Data_<SpDFloat>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble off, DDouble inc):
  SpDFloat( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false) { 
  this->dim.Purge();

  if (iT == BaseGDL::NOZERO) return; //very frequent

  if (iT == BaseGDL::ZERO) { //rather frequent
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    }
  } else if (iT == BaseGDL::INDGEN) { //less frequent
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      if (off == 0 && inc == 1) {
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = i;
      } else {
        DFloat f_off = off;
        DFloat f_inc = inc;
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = f_off + i * f_inc;
      }
    } else {
      if (off == 0 && inc == 1) {
	TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = i;
      } else {
        DFloat f_off = off;
        DFloat f_inc = inc;
	TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = f_off + i * f_inc;
      }
    }
  }
}

template<> Data_<SpDComplex>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble off, DDouble inc):
  SpDComplex( dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false) { 
  this->dim.Purge();

  if (iT == BaseGDL::NOZERO) return; //very frequent

  if (iT == BaseGDL::ZERO) { //rather frequent
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    }
  } else if (iT == BaseGDL::INDGEN) { //less frequent
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      if (off == 0 && inc == 1) {
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = i;
      } else {
        DFloat f_off = off;
        DFloat f_inc = inc;
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = f_off + i * f_inc;
      }
    } else {
      if (off == 0 && inc == 1) {
	TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = i;
      } else {
        DFloat f_off = off;
        DFloat f_inc = inc;
	TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) (*this)[i] = f_off + i * f_inc;
      }
    }
  }
}

template<> Data_<SpDString>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble):
  SpDString(dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{ 
  dim.Purge();
  
  if( iT == BaseGDL::INDGEN) throw GDLException("DStringGDL(dim,InitType=INDGEN) called.");
}

template<> Data_<SpDPtr>::Data_(const dimension& dim_,  BaseGDL::InitType iT, DDouble, DDouble):
  SpDPtr(dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false) { 
  dim.Purge();

  if (iT == BaseGDL::INDGEN) throw GDLException("DPtrGDL(dim,InitType=INDGEN) called.");

  if (iT != BaseGDL::NOALLOC && iT != BaseGDL::NOZERO) {
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    }
  }
}
template<> Data_<SpDObj>::Data_(const dimension& dim_, BaseGDL::InitType iT, DDouble, DDouble):
  SpDObj(dim_), dd( (iT == BaseGDL::NOALLOC) ? 0 : this->dim.NDimElements(), false)
{ 
  dim.Purge();

  if( iT == BaseGDL::INDGEN) throw GDLException("DObjGDL(dim,InitType=INDGEN) called.");
  
  if (iT != BaseGDL::NOALLOC && iT != BaseGDL::NOZERO) {
    SizeT sz = dd.size();
    if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < sz; ++i) (*this)[i] = 0;
    }
  }
}

// c-i
//template<class Sp>
//Data_<Sp>::Data_(const Data_& d_): Sp(d_.dim), dd(d_.dd) {  }

//faster, C++ initializer is probably too shy. 
template<class Sp>
Data_<Sp>::Data_(const Data_& d_) : Sp(d_.dim), dd(this->dim.NDimElements(), false) { 
//  this->dim.Purge(); //very bad, see #1537 
  SizeT sz = dd.size();
  if ((GDL_NTHREADS=parallelize( sz, TP_ARRAY_INITIALISATION))==1) { //most frequent
    for (SizeT i = 0; i < sz; i++) dd[i] = d_.dd[i];
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < sz; i++) dd[i] = d_.dd[i];
  }
}

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
Data_<Sp>* Data_<Sp>::Dup() const {  return new Data_(*this);}

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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  DFloatGDL* res = static_cast<DFloatGDL*> (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->LogThis();
  return res;
}

template<>
BaseGDL* Data_<SpDFloat>::Log() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  }
  return n;
}

template<>
BaseGDL* Data_<SpDDouble>::Log()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  }
  return n;
}
template<>
BaseGDL* Data_<SpDComplex>::Log()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  }
  return n;
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Log()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*n)[ i] = log((*this)[ i]);
  }
  return n;
}

// this is actually not a "log" of "this",
// but the behaviour is fine with the usage in the library function
// the real LogThis is done in the specializations for floating and 
// complex types
template<class Sp>
BaseGDL* Data_<Sp>::LogThis()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  DFloatGDL* res = static_cast<DFloatGDL*>(this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->LogThis(); // calls correct LogThis for float
  return res;
}

template<>
BaseGDL* Data_<SpDFloat>::LogThis() { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  }
  return this;
}

template<>
BaseGDL* Data_<SpDDouble>::LogThis()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  }
  return this;
}

template<>
BaseGDL* Data_<SpDComplex>::LogThis()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  }
  return this;
}
template<>
BaseGDL* Data_<SpDComplexDbl>::LogThis()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log((*this)[ i]);
  }
  return this;
}

template<class Sp>
BaseGDL* Data_<Sp>::Log10()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  DFloatGDL* res = static_cast<DFloatGDL*> (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->Log10This();
  return res;
}

template<>
BaseGDL* Data_<SpDFloat>::Log10()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log10((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  }
  return n;
}

template<>
BaseGDL* Data_<SpDDouble>::Log10()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log10((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  }
  return n;
}

template<>
BaseGDL* Data_<SpDComplex>::Log10()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log10((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  }
  return n;
}

template<>
BaseGDL* Data_<SpDComplexDbl>::Log10()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  Data_* n = this->New(this->dim, BaseGDL::NOZERO);
  SizeT nEl = n->N_Elements();
  if (nEl == 1) {
    (*n)[ 0] = log10((*this)[ 0]);
    return n;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for( SizeT i=0; i<nEl; ++i) (*n)[ i] = log10( (*this)[ i]);
  }
  return n;
}

// see comment at void Data_<Sp>::LogThis()              
template<class Sp>
BaseGDL* Data_<Sp>::Log10This()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  DFloatGDL* res = static_cast<DFloatGDL*> (this->Convert2( GDL_FLOAT, BaseGDL::COPY));
  res->Log10This(); // calls correct Log10This for float
  return res;
}
template<>
BaseGDL* Data_<SpDFloat>::Log10This()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log10((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  }
  return this;
}

template<>
BaseGDL* Data_<SpDDouble>::Log10This()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log10((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  }
  return this;
}
template<>
BaseGDL* Data_<SpDComplex>::Log10This()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log10((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  }
  return this;
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Log10This()              
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  SizeT nEl = N_Elements();
  if (nEl == 1) {
    (*this)[ 0] = log10((*this)[ 0]);
    return this;
  }
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for (SizeT i = 0; i < nEl; ++i) (*this)[ i] = log10((*this)[ i]);
  }
  return this;
}



// template<class Sp>
// BaseGDL* Data_<Sp>::Abs() const
// { 
//   return new Data_( this->dim, dd.abs());
// }

template<class Sp>
inline bool Data_<Sp>::Greater(SizeT i1, SizeT i2) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return ((*this)[i1] > (*this)[i2]);}

template<>
inline bool Data_<SpDComplex>::Greater(SizeT i1, SizeT i2) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return (abs((*this)[i1]) > abs((*this)[i2]));}
template<>
inline bool Data_<SpDComplexDbl>::Greater(SizeT i1, SizeT i2) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return (abs((*this)[i1]) > abs((*this)[i2]));}


template<class Sp>
inline bool Data_<Sp>::Equal(SizeT i1, SizeT i2) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) return ((*this)[i1] == (*this)[i2]);}



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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  memcpy( (void*) &dst[ dstLonIx], (void*) &src[ srcLonIx], chunk0 * sizeof(Ty)); //explicitly cast the pointer to silence [-Wnontrivial-memcall] warning
  // as the 'string case' is specially treated in CShift() below.
  dstLonIx += chunk0;
  srcLonIx += chunk0;

  dstLonIx -= stride_1;

  memcpy( (void*) &dst[ dstLonIx], (void*) &src[ srcLonIx], chunk1 * sizeof(Ty));
  dstLonIx += chunk1 ;
  srcLonIx += chunk1;

  dstLonIx += stride_1;
}

#undef TEST_GOOD_OL_VERSION

template<class Sp>
BaseGDL* Data_<Sp>::CShift( DLong s[ MAXRANK]) const { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
BaseGDL* Data_<Sp>::Transpose(DUInt* perm) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
  SizeT srcStride[ MAXRANK + 1];
  this->dim.Stride(srcStride, rank);

// GD: Tests show that we are way faster than eigen (below) with our 'parallell' method in ALL CASES on my intel I7.  
// But this may not be true on other platforms, so keep the possibility via a -- switch.
  if (useEigenForTransposeOps) {
#ifdef USE_EIGEN
  //for some reason, this simple eigen::code dos not like dimensions == 1, so cannot be used if this is the case.
  bool try_eigen=true;
  for (auto i=0; i< MAXRANK; ++i) if (this->dim[i]==1) try_eigen=false;
  // eigen dims are long, and template deduction mechanism may throw on a narrowing of SizeT dim to long .
  long indim[ MAXRANK ];
   for (auto i=0; i< MAXRANK; ++i) indim[i]=this->dim[i];
  long outdim[ MAXRANK ];
   for (auto i=0; i< MAXRANK; ++i) outdim[i]=res->dim[i];     
  if (try_eigen && rank == 2) // special case: eigen x 2
  {
    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Aligned> mThis(&(*this)[0],  indim[0],  indim[1]);
    Eigen::Map<Eigen::Array<Ty, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Aligned> mRes(&(*res)[0],  outdim[0],  outdim[1]);
    mRes=mThis.transpose();
    return res;
  }
#endif
  
#ifdef EIGEN_HAS_TENSOR  
  else if (try_eigen && rank == 3) // special case: eigen x 3
  {
 Eigen::TensorMap<Eigen::Tensor<Ty, 3>> mThis(&(*this)[0], indim[0],  indim[1], indim[2]); 
 Eigen::TensorMap<Eigen::Tensor<Ty, 3>> mRes(&(*res)[0], outdim[0],  outdim[1], outdim[2]);
 mRes=mThis.shuffle(perm);
 return res;
  }
  
  else if (try_eigen && rank == 4) // special case: eigen x 4
  {
 Eigen::TensorMap<Eigen::Tensor<Ty, 4>> mThis(&(*this)[0], indim[0],  indim[1], indim[2], indim[3]); 
 Eigen::TensorMap<Eigen::Tensor<Ty, 4>> mRes(&(*res)[0], outdim[0],  outdim[1], outdim[2], outdim[3]);
 mRes=mThis.shuffle(perm);
 return res;
  }
  else if (try_eigen && rank == 5) // special case: eigen x 5
  {
 Eigen::TensorMap<Eigen::Tensor<Ty, 5>> mThis(&(*this)[0], indim[0],  indim[1], indim[2], indim[3], indim[4]); 
 Eigen::TensorMap<Eigen::Tensor<Ty, 5>> mRes(&(*res)[0], outdim[0],  outdim[1], outdim[2], outdim[3], outdim[4]);
 mRes=mThis.shuffle(perm);
 return res;
  }
  else if (try_eigen && rank == 6) // special case: eigen x 6
  {
 Eigen::TensorMap<Eigen::Tensor<Ty, 6>> mThis(&(*this)[0], indim[0],  indim[1], indim[2], indim[3], indim[4], indim[5]); 
 Eigen::TensorMap<Eigen::Tensor<Ty, 6>> mRes(&(*res)[0], outdim[0],  outdim[1], outdim[2], outdim[3], outdim[4], outdim[5]);
 mRes=mThis.shuffle(perm);
 return res;
  }
  else if (try_eigen && rank == 7) // special case: eigen x 7
  {
 Eigen::TensorMap<Eigen::Tensor<Ty, 7>> mThis(&(*this)[0], indim[0],  indim[1], indim[2], indim[3], indim[4], indim[5], indim[6]); 
 Eigen::TensorMap<Eigen::Tensor<Ty, 7>> mRes(&(*res)[0], outdim[0],  outdim[1], outdim[2], outdim[3], outdim[4], outdim[5], outdim[6]);
 mRes=mThis.shuffle(perm);
 return res;
  }
  else if (try_eigen && rank == 8) // special case: eigen x 8
  {
 Eigen::TensorMap<Eigen::Tensor<Ty, 8>> mThis(&(*this)[0], indim[0],  indim[1], indim[2], indim[3], indim[4], indim[5], indim[6], indim[7]); 
 Eigen::TensorMap<Eigen::Tensor<Ty, 8>> mRes(&(*res)[0], outdim[0],  outdim[1], outdim[2], outdim[3], outdim[4], outdim[5], outdim[6], outdim[7]);
 mRes=mThis.shuffle(perm);
 return res;
  }

#endif
  
  } //will have returned if eigen ops exist.
  
  SizeT nElem = dd.size();
  long chunksize = nElem;
  long nchunk = 1;
  bool do_parallel = false;
  GDL_NTHREADS=parallelize( nElem, TP_CPU_INTENSIVE);
  if (GDL_NTHREADS > 1) { //no use start parallel threading for small numbers.
    chunksize = nElem /  GDL_NTHREADS;
    nchunk = nElem / chunksize;
    if (chunksize * nchunk < nElem) ++nchunk;
    do_parallel = true;
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

  if (!do_parallel) {
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
  } else {

    TRACEOMP(__FILE__,__LINE__)
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
  }
  return res;
}

// used by reverse
// NOT A TP function
template<class Sp>
void Data_<Sp>::Reverse(DLong dim) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT halfDim=this->dim[dim]/2 * revStride;
  if (this->dim[dim]%2) halfDim++;
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT span=outerStride - revStride;
  if ((GDL_NTHREADS=parallelize(nEl, TP_CPU_INTENSIVE))==1) {  //most frequent
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = o; i < o+revStride; ++i) {
        for (SizeT s = i, opp=span+i; s < halfDim+i  ; s += revStride, opp-=revStride) {
             Ty tmp = (*this)[s];
            (*this)[s] = (*this)[opp];
            (*this)[opp] = tmp;
        }
      }
    }
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel num_threads(GDL_NTHREADS)
    {
#pragma omp for collapse(2)
      for (SizeT o = 0; o < nEl; o += outerStride) {
        for (SizeT i = 0; i < revStride; ++i) {
          for (SizeT s = i + o, opp = span + i + o; s < halfDim + i + o; s += revStride, opp -= revStride) {
            Ty tmp = (*this)[s];
            (*this)[s] = (*this)[opp];
            (*this)[opp] = tmp;
          }
        }
      }
    }
  }
}

template<class Sp>
BaseGDL* Data_<Sp>::DupReverse(DLong dim) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
  Guard<Data_> res_guard(res);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT halfDim=this->dim[dim]/2 * revStride;
  if (this->dim[dim]%2) halfDim++;
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT span=outerStride - revStride;
  if ((GDL_NTHREADS=parallelize(nEl, TP_CPU_INTENSIVE))==1) {  //most frequent
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = o; i < o+revStride; ++i) {
        for (SizeT s = i, opp=span+i; s < halfDim+i  ; s += revStride, opp-=revStride) {
             Ty tmp = (*this)[s];
            (*res)[s] = (*this)[opp];
            (*res)[opp] = tmp;
        }
      }
    }
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel num_threads(GDL_NTHREADS)
    {
#pragma omp for collapse(2)
      for (SizeT o = 0; o < nEl; o += outerStride) {
        for (SizeT i = 0; i < revStride; ++i) {
          for (SizeT s = i+o, opp = span + i+o; s < halfDim + i+o; s += revStride, opp -= revStride) {
             Ty tmp = (*this)[s];
            (*res)[s] = (*this)[opp];
            (*res)[opp] = tmp;
          }
        }
      }
    }
  }
  return res_guard.release();
}

template<>
BaseGDL* Data_<SpDPtr>::DupReverse(DLong dim) {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
  Guard<Data_> res_guard(res);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT halfDim = this->dim[dim] / 2 * revStride;
  if (this->dim[dim] % 2) halfDim++;
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT span = outerStride - revStride;
  if ((GDL_NTHREADS=parallelize(nEl, TP_CPU_INTENSIVE)) == 1) { //most frequent
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = o; i < o + revStride; ++i) {
        for (SizeT s = i, opp = span + i; s < halfDim + i; s += revStride, opp -= revStride) {
          Ty tmp = (*this)[s];
          (*res)[s] = (*this)[opp];
          (*res)[opp] = tmp;
        }
      }
    }
  } else {

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel num_threads(GDL_NTHREADS)
    {
#pragma omp for collapse(2)
      for (SizeT o = 0; o < nEl; o += outerStride) {
        for (SizeT i = 0; i < revStride; ++i) {
          for (SizeT s = i + o, opp = span + i + o; s < halfDim + i + o; s += revStride, opp -= revStride) {
            Ty tmp = (*this)[s];
            (*res)[s] = (*this)[opp];
            (*res)[opp] = tmp;
          }
        }
      }
    }
  }
  GDLInterpreter::IncRef(res);
  return res_guard.release();
}

template<>
BaseGDL* Data_<SpDObj>::DupReverse(DLong dim)  {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Data_* res = new Data_(this->dim, BaseGDL::NOZERO);
  Guard<Data_> res_guard(res);
  SizeT nEl = N_Elements();
  SizeT revStride = this->dim.Stride(dim);
  SizeT halfDim = this->dim[dim] / 2 * revStride;
  if (this->dim[dim] % 2) halfDim++;
  SizeT outerStride = this->dim.Stride(dim + 1);
  SizeT span = outerStride - revStride;
  if ((GDL_NTHREADS=parallelize(nEl, TP_CPU_INTENSIVE)) == 1) { //most frequent
    for (SizeT o = 0; o < nEl; o += outerStride) {
      for (SizeT i = o; i < o + revStride; ++i) {
        for (SizeT s = i, opp = span + i; s < halfDim + i; s += revStride, opp -= revStride) {
          Ty tmp = (*this)[s];
          (*res)[s] = (*this)[opp];
          (*res)[opp] = tmp;
        }
      }
    }
  } else {

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel num_threads(GDL_NTHREADS)
    {
#pragma omp for collapse(2)
      for (SizeT o = 0; o < nEl; o += outerStride) {
        for (SizeT i = 0; i < revStride; ++i) {
          for (SizeT s = i + o, opp = span + i + o; s < halfDim + i + o; s += revStride, opp -= revStride) {
            Ty tmp = (*this)[s];
            (*res)[s] = (*this)[opp];
            (*res)[opp] = tmp;
          }
        }
      }
    }
  }
  GDLInterpreter::IncRefObj(res);
  return res_guard.release();
}

// rank must be 1 or 2 (already checked)
template<class Sp> 
BaseGDL* Data_<Sp>::Rotate( DLong dir) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
typename Data_<Sp>::Ty Data_<Sp>::Sum() const {
  TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  Ty s = dd[ 0];
  SizeT nEl = dd.size();
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 1; i < nEl; ++i) s += dd[ i];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:s)
    for (SizeT i = 1; i < nEl; ++i) s += dd[ i];
  }
  return s;
}

template<> 
Data_<SpDString>::Ty Data_<SpDString>::Sum() const 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s= dd[ 0];
  SizeT nEl = dd.size();
  for( SizeT i=1; i<nEl; ++i)
    {
      s += dd[ i];
    }
  return s;
}

template<>
Data_<SpDComplexDbl>::Ty Data_<SpDComplexDbl>::Sum() const { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  DDouble sr = dd[ 0].real();
  DDouble si = dd[ 0].imag();
  SizeT nEl = dd.size();
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 1; i < nEl; ++i) {
      sr += dd[i].real();
      si += dd[i].imag();
    }
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:si,sr)
      for (SizeT i = 1; i < nEl; ++i) {
        sr += dd[i].real();
        si += dd[i].imag();
      }
  }
  return std::complex<DDouble>(sr, si);
}

template<>
Data_<SpDComplex>::Ty Data_<SpDComplex>::Sum() const { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  DFloat sr = dd[ 0].real();
  DFloat si = dd[ 0].imag();
  SizeT nEl = dd.size();
  if ((GDL_NTHREADS=parallelize( nEl))==1) { //most frequent
    for (SizeT i = 1; i < nEl; ++i) {
      sr += dd[i].real();
      si += dd[i].imag();
    }
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:si,sr)
      for (SizeT i = 1; i < nEl; ++i) {
        sr += dd[i].real();
        si += dd[i].imag();
      }
  }
  return std::complex<DFloat>(sr, si);
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
// {  return (*this)[d1];}

// only used from DStructGDL
template<class Sp> 
Data_<Sp>&  Data_<Sp>::operator=(const BaseGDL& r)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{  return (this->Type() == r->Type());}

template< class Sp>
void* Data_<Sp>::DataAddr()// SizeT elem)
{  return &(*this)[0];}//elem];}

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
{  return dd.size();}

template<>
SizeT Data_<SpDObj>::N_Elements() const 
{  
  if( !this->StrictScalar())
    return dd.size();
#if 1  // GJ change this and see how many problems it solves, creates. 2016/5/5 
  else return 1;
#elif 0
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
#endif  
}


template< class Sp>
SizeT Data_<Sp>::Size() const 
{  return dd.size();}
template< class Sp>
SizeT Data_<Sp>::Sizeof() const 
{  return sizeof(Ty);}

template< class Sp>
void Data_<Sp>::Clear() 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) (*this)[ i] = Sp::zero;
}

// first time initialization (construction)
template< class Sp>
void Data_<Sp>::Construct() 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  // note that this is not possible in cases where an operation 
  // (here: 'new' which is ok) isn't defined for any POD
  // (although this code never executes and should be optimized away anyway)
  const bool isPOD = Sp::IS_POD;   
  // do nothing for POD
  if( !isPOD)
  {
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) new (&(dd[ i])) Ty;
  }
}

template<>
void Data_<SpDPtr>::Construct() 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) dd[ i] = 0;
}

template<>
void Data_<SpDObj>::Construct()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) dd[ i] = 0;
}

// construction and initalization to zero
template< class Sp>
void Data_<Sp>::ConstructTo0() 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  if( Sp::IS_POD)
  {
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) (*this)[ i] = Sp::zero;
  }
  else
  {
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) new (&(*this)[ i]) Ty( Sp::zero);
  }
}

template< class Sp>
void Data_<Sp>::Destruct() 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__) 
  // no destruction for POD
  if( !Sp::IS_POD)
  {
  SizeT nEl = dd.size(); 
  for( SizeT i = 0; i<nEl; ++i) 
    (*this)[ i].~Ty();    
  }
}
template<>
void Data_< SpDPtr>::Destruct()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  GDLInterpreter::DecRef( this);
}
template<>
void Data_< SpDObj>::Destruct()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  GDLInterpreter::DecRefObj( this);
}

template< class Sp>
BaseGDL* Data_<Sp>::SetBuffer( const void* b)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  dd.SetBuffer( static_cast< Ty*>(const_cast<void*>( b)));
  return this;
}
template< class Sp>
void Data_<Sp>::SetBufferSize( SizeT s)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  dd.SetBufferSize( s);
}

template< class Sp>
Data_<Sp>* Data_<Sp>::New( const dimension& dim_, BaseGDL::InitType noZero) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( noZero == BaseGDL::NOZERO) return new Data_(dim_, BaseGDL::NOZERO);
  if( noZero == BaseGDL::INIT)
    {
      Data_* res =  new Data_(dim_, BaseGDL::NOZERO);
      SizeT nEl = res->dd.size();
      if ((GDL_NTHREADS=parallelize( nEl, TP_ARRAY_INITIALISATION))==1) { //most frequent
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = (*this)[ 0]; // set all to scalar
      } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for( SizeT i=0; i<nEl; ++i) (*res)[ i] = (*this)[ 0]; // set all to scalar
      }
      return res;
    }
  return new Data_(dim_); // zero data
}


template< class Sp>
Data_<Sp>* Data_<Sp>::NewResult() const 
{ 
  return new Data_(this->dim, BaseGDL::NOZERO);
}

template<class Sp>
SizeT Data_<Sp>::NBytes() const 
{  return (dd.size() * sizeof(Ty));}

// string, ptr, obj cannot calculate their bytes
// only used by assoc function
template<> SizeT Data_<SpDString>::NBytes() const
{ 
  SizeT nEl = dd.size();
  SizeT nB = 0;
  for( SizeT i=0; i<nEl; ++i)  nB += (*this)[i].size();
  return nB;
}

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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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

template<>
int Data_<SpDFloat>::Scalar2Index(SizeT& st) const {
  if (dd.size() != 1) return 0;
  if (std::isfinite((*this)[0])) {
    if ((*this)[0] < 0) {
      if (this->dim.Rank() != 0)
        return -2;
      else
        return -1;
    }

    st = static_cast<SizeT> ((*this)[0]);
    if (this->dim.Rank() != 0) return 2;
    return 1;
  } else {
    if (this->dim.Rank() != 0)
      return -2;
    else
      return -1;
  }
}
template<>
int Data_<SpDDouble>::Scalar2Index(SizeT& st) const {
  if (dd.size() != 1) return 0;
  if (std::isfinite((*this)[0])) {
    if ((*this)[0] < 0 ) {
      if (this->dim.Rank() != 0)
        return -2;
      else
        return -1;
    }

    st = static_cast<SizeT> ((*this)[0]);
    if (this->dim.Rank() != 0) return 2;
    return 1;
  } else {
    if (this->dim.Rank() != 0)
      return -2;
    else
      return -1;
  }
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
  if (std::isfinite(r)) {
    if( r < 0.0) return -1;
    st= static_cast<SizeT>(r);
    if( this->dim.Rank() != 0) return 2;
    return 1;
  } else {
    if( this->dim.Rank() != 0) return -2;
    return -1;
  }
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
int Data_<SpDComplexDbl>::Scalar2Index(SizeT& st) const
{
  if (dd.size() != 1) return 0;
  float r = real((*this)[0]);
  if (std::isfinite(r)) {
    if (r < 0.0) return -1;
    st = static_cast<SizeT> (r);
    if (this->dim.Rank() != 0) return 2;
    return 1;
  } else {
    if (this->dim.Rank() != 0) return -2;
    return -1;
  }
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
    std::ostringstream os;
    res->ToStream(os);
    throw GDLException( isTrueOverload->ObjectName() + ": Object reference expression not allowed in this context: " +
			 os.str(),true,false);
  }
  
  return res->LogTrue();
}

// False
template<class Sp>
bool Data_<Sp>::False()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return !True();
}

// Sgn
template<class Sp>
int Data_<Sp>::Sgn() // -1,0,1
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
bool Data_<Sp>::ArrayEqual( BaseGDL* rIn) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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


//Not used
//template<class Sp>
//bool Data_<Sp>::OutOfRangeOfInt() const 
//{ 
//  assert( this->StrictScalar());
//  return (*this)[0] > std::numeric_limits< DInt>::max() || (*this)[0] < std::numeric_limits< DInt>::min();
//}
//
//template<>
//bool Data_<SpDString>::OutOfRangeOfInt() const 
//{ 
//  return false;
//}
//template<>
//bool Data_<SpDByte>::OutOfRangeOfInt() const 
//{ 
//  return false;
//}
//template<>
//bool Data_<SpDComplex>::OutOfRangeOfInt() const 
//{ 
//  return false;
//}
//template<>
//bool Data_<SpDComplexDbl>::OutOfRangeOfInt() const 
//{ 
//  return false;
//}

// for statement compliance (int types , float types scalar only)
// (convert strings to floats here (not for first argument)
// GD: works together with FORNode::Run(). At exit, if bool is true, 'this' will be CONVERTED (in FORNode::Run()) TO lEnd's Type if they are not similar.
// the logic is the following:
// If 'this' is an unsigned integer or a floating-point value, lEnd is converted to this->Type() and that's all.
// Else (signed integer case), this will be promoted to lEnd's type if lEnd is larger than the maximum value available for 'this'.
// BUT there are issues with trying to get that with our logic: see bug #816

template<class Sp>
bool Data_<Sp>::ForCheck(BaseGDL** lEnd, BaseGDL** lStep) { 
  // all scalars?
  if (!StrictScalar())
    throw GDLException("Loop INIT must be a scalar in this context.");

  if (!(*lEnd)->StrictScalar())
    throw GDLException("Loop LIMIT must be a scalar in this context.");

  if (lStep != NULL && !(*lStep)->StrictScalar())
    throw GDLException("Loop INCREMENT must be a scalar in this context.");

  // only proper types?
  if (this->t == GDL_UNDEF)
    throw GDLException("Expression is undefined.");
  if (this->t == GDL_COMPLEX || this->t == GDL_COMPLEXDBL)
    throw GDLException("Complex expression not allowed in this context.");
  if (this->t == GDL_PTR)
    throw GDLException("Pointer expression not allowed in this context.");
  if (this->t == GDL_OBJ)
    throw GDLException("Object expression not allowed in this context.");
  if (this->t == GDL_STRING)
    throw GDLException("String expression not allowed in this context.");

  DType lType = (*lEnd)->Type();
  if (lType == GDL_COMPLEX || lType == GDL_COMPLEXDBL)
    throw GDLException("Complex expression not allowed in this context.");
  
  //float type are simple:
  if (FloatType(this->t) || this->t == GDL_LONG64 ) { //unsigned rule and other simple cases where 'this' rules
      *lEnd = (*lEnd)->Convert2(this->t);
      if (lStep != NULL) *lStep = (*lStep)->Convert2(this->t);
      return true;
    }
  
   //All other cases, it is lEnd type that rules if it's value is gt the max value of 'this' type:

  //ForCheck() is donce only once so the penalty is minimal for these complicated tests.
  //BYTE is quite special and (see #816) there are dubious cases. The following code
  //avoids all traps at the expense of being complicated and promote to INT in a case where IDL does not:
  //"for i=255b,255,1 do help,i"
  if (this->t == GDL_BYTE) {
    //early exit with simple check is needed for Bytes beacuse the arithmetic is not the same as for other types.
    // simple check: end vs. start
    (*lEnd) = (*lEnd)->Convert2(GDL_BYTE); //convert to byte!
    DByte* endval = static_cast<DByte*> ((*lEnd)->DataAddr());
    DByte* startval = static_cast<DByte*> (this->DataAddr());

    if (lStep != NULL) { //overflow case NOT handled in forAddCondUp
      (*lStep) = (*lStep)->Convert2(GDL_LONG); //check a few things with a long argument
      //now what if end+step > 255 ? 
      DLong* step = static_cast<DLong*> ((*lStep)->DataAddr());
      if (std::signbit(step[0])) {
        // 1) check lEnd is not already smaller than lStart
        if (endval[0] > startval[0]) return false;
        // 2) We must convert to INTs, but this may be a problem.
        (*lEnd) = (*lEnd)->Convert2(GDL_INT);
      } else { //step >= 0
        if (endval[0] < startval[0]) return false;
        int final = endval[0] + step[0];
        if (final > 255) { //whatever the start value, the end value must be 255 when the final step is reached.
          //We must convert to INTs, but this may be a problem.
          (*lEnd) = (*lEnd)->Convert2(GDL_INT);
        }
      }
      //Must have lStep always as LEnd
      (*lStep) = (*lStep)->Convert2((*lEnd)->Type());
    } else if (endval[0] < startval[0]) return false; //lStep==NULL case
    return true;
  }

  // Check for promotion as we have to be robust to lEnd==std::numeric_limits< DInt>::max() and lEnd+Step > numeric limit. (bug #816)
  // (*lend)+/-(*lstep) must not overflow an INT numeric limit, else the loop will fail.If (*lStep) is not defined, 
  // forAddCondUp is used and will save the day as the test is made in the right order. this is not the general case, where the
  //addition (or substraction) is made in the prognode Run() loop. In that case we *NEED* to promote the loop variable correctly.

  (*lEnd) = (*lEnd)->Convert2(GDL_LONG64); // upgrade to safe encoding
  DLong64* endval = static_cast<DLong64*> ((*lEnd)->DataAddr());
  
  DLong64 testVal=endval[0];
  
  if (lStep != NULL) (*lStep) = (*lStep)->Convert2(GDL_LONG64); //idem
  DLong64* step = NULL;
  if (lStep != NULL) {
    step = static_cast<DLong64*> ((*lStep)->DataAddr());
    testVal+=step[0];
  }
  
  if (this->t == GDL_INT) {
    if (testVal < std::numeric_limits< DInt>::max() && testVal > std::numeric_limits< DInt>::min()) *lEnd = (*lEnd)->Convert2(GDL_INT);
    else if (testVal < std::numeric_limits< DLong>::max() && testVal > std::numeric_limits< DLong>::min()) *lEnd = (*lEnd)->Convert2(GDL_LONG);
    //Must have lStep always as LEnd
    if (lStep != NULL) (*lStep) = (*lStep)->Convert2((*lEnd)->Type());
    return true; // finished for GDL_INT
  }
  if (this->t == GDL_LONG) {
    if (testVal < std::numeric_limits< DLong>::max() && testVal > std::numeric_limits< DLong>::min()) *lEnd = (*lEnd)->Convert2(GDL_LONG);
    //Must have lStep always as LEnd
    if (lStep != NULL) (*lStep) = (*lStep)->Convert2((*lEnd)->Type());
    return true; // finished for GDL_LONG
  }
  // other cases: no promotion
  *lEnd = (*lEnd)->Convert2(this->t);
  if (lStep != NULL) *lStep = (*lStep)->Convert2(this->t);
  return true;
}

bool DStructGDL::ForCheck( BaseGDL** lEnd, BaseGDL** lStep)
{
  throw GDLException("Struct expression not allowed in this context.");
  return false;
}

// ForCheck must have been called before
template<class Sp>
bool Data_<Sp>::ForAddCondUp( BaseGDL* endLoopVar)
{ 
  if( endLoopVar->Type() != this->t) throw GDLException("Type of FOR index variable changed.");
  Data_* lEnd=static_cast<Data_*>(endLoopVar);
  bool what=true;
  if ((*this)[0] == (*lEnd)[0]) //This way, loop will stop on good end value and loop index will be incremented... 
    //but only afterwards as to avoid bug #816
  {
    what=false;
  } else what=((*this)[0] < (*lEnd)[0]); 
  (*this)[0] += 1;
  return what; 
}
// ForCheck must have been called before
template<class Sp>
bool Data_<Sp>::ForCondUp( BaseGDL* lEndIn)
{ 
  if( lEndIn->Type() != this->t) throw GDLException("Type of FOR index variable changed.");
  Data_* lEnd=static_cast<Data_*>(lEndIn);
  return (*this)[0] <= (*lEnd)[0];
}
template<class Sp>
bool Data_<Sp>::ForCondDown( BaseGDL* lEndIn)
{ 
  if( lEndIn->Type() != this->t) throw GDLException("Type of FOR index variable changed.");
  Data_* lEnd=static_cast<Data_*>(lEndIn);
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


//NOT USED (GD)
//// normal (+1) version
//template<class Sp>
//void Data_<Sp>::ForAdd()
//{ 
//  (*this)[0] += 1;
//}
// cannnot be called, just to make the compiler shut-up
//void DStructGDL::ForAdd() {}

template<class Sp>
void Data_<Sp>::AssignAtIx( RangeT ixR, BaseGDL* srcIn)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
	  for( int c=0; c<nCp; ++c)
	    (*this)[ c]=scalar;
	}
      else
	{
	  SizeT nCp=ixList->N_Elements();
	  
	  AllIxBaseT* allIx = ixList->BuildIx();
	  (*this)[ allIx->InitSeqAccess()]=scalar;
	  for( SizeT c=1; c<nCp; ++c)
	    (*this)[ allIx->SeqAccess()]=scalar;
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
	  for( int c=0; c<nCp; ++c)
	    (*this)[ c]=(*src)[c+offset];
	}
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
		  (*this)[ allIx->InitSeqAccess()]=(*src)[0];
		  for( SizeT c=1; c<nCp; ++c)
		    (*this)[ allIx->SeqAccess()]=(*src)[c];
		}
	      else
		{
		  if( (srcElem-offset) < nCp)
		    throw GDLException("Array subscript must have same size as"
				       " source expression.");
		  
		  AllIxBaseT* allIx = ixList->BuildIx();
		  (*this)[ allIx->InitSeqAccess()]=(*src)[offset];
		  for( SizeT c=1; c<nCp; ++c)
		    (*this)[ allIx->SeqAccess()]=(*src)[c+offset];
		}
	    }
	}
    }
}
template<class Sp>
void Data_<Sp>::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  assert( ixList != NULL);

  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar) 
    {// src is scalar
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
	  for( int c=1; c<nCp; ++c)
	    (*this)[ allIx->SeqAccess()]=scalar;
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
	  for( int c=1; c<nCp; ++c)
	    (*this)[ allIx->SeqAccess()]=(*src)[c];
	}
    }
}
template<class Sp>
void Data_<Sp>::AssignAt( BaseGDL* srcIn)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  //  breakpoint(); // gdbg can not handle breakpoints in template functions
  Data_* src = static_cast<Data_*>(srcIn);  

  SizeT srcElem= src->N_Elements();
  bool  isScalar= (srcElem == 1);
  if( isScalar) 
    { // src is scalar
      Ty scalar=(*src)[0];

      /*      dd = scalar;*/
      SizeT nCp=Data_::N_Elements();
      for( int c=0; c<nCp; ++c)
	(*this)[ c]=scalar;
    }
  else
    {
      SizeT nCp=Data_::N_Elements();
      
      // if (non-indexed) src is smaller -> just copy its number of elements
      if( nCp > srcElem) nCp=srcElem;
      for( int c=0; c<nCp; ++c)
	(*this)[ c]=(*src)[c];
    }
}

// increment/decrement operators
// integers
template<class Sp>
void Data_<Sp>::DecAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      (*this)[ allIx->InitSeqAccess()]--;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()]--;
    }
}
template<class Sp>
void Data_<Sp>::IncAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      (*this)[ allIx->InitSeqAccess()]++;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()]++;
    }
}
// float, double
template<>
void Data_<SpDFloat>::DecAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      (*this)[ allIx->InitSeqAccess()] -= 1.0f;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0f;
    }
}
template<>
void Data_<SpDFloat>::IncAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      (*this)[ allIx->InitSeqAccess()] += 1.0f;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0f;
    }
}
template<>
void Data_<SpDDouble>::DecAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      (*this)[ allIx->InitSeqAccess()] -= 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0;
    }
}
template<>
void Data_<SpDDouble>::IncAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
      (*this)[ allIx->InitSeqAccess()] += 1.0f;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0f;
    }
}
// complex
template<>
void Data_<SpDComplex>::DecAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( ixList == NULL)
    {
      //       dd -= 1.0f;

      SizeT nCp=Data_::N_Elements();
      
      for( int c=0; c<nCp; ++c)
	(*this)[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      (*this)[ allIx->InitSeqAccess()] -= 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0;
    }
}
template<>
void Data_<SpDComplex>::IncAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( ixList == NULL)
    {
      //       dd += 1.0f;

      SizeT nCp=Data_::N_Elements();
      for( int c=0; c<nCp; ++c)
      	(*this)[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      (*this)[ allIx->InitSeqAccess()] += 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0;
    }
}
template<>
void Data_<SpDComplexDbl>::DecAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( ixList == NULL)
    {
      //       dd -= 1.0;

      SizeT nCp=Data_::N_Elements();
      for( int c=0; c<nCp; ++c)
      	(*this)[ c] -= 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      (*this)[ allIx->InitSeqAccess()] -= 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] -=1.0;
    }
}
template<>
void Data_<SpDComplexDbl>::IncAt( ArrayIndexListT* ixList) 
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( ixList == NULL)
    {
      //       dd += 1.0;

      SizeT nCp=Data_::N_Elements();
      for( int c=0; c<nCp; ++c)
      	(*this)[ c] += 1.0;
    }
  else
    {
      SizeT nCp=ixList->N_Elements();
      
      AllIxBaseT* allIx = ixList->BuildIx();
      (*this)[ allIx->InitSeqAccess()] += 1.0;
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ allIx->SeqAccess()] +=1.0;
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* src=static_cast<Data_* >(srcIn);
  if( ixList == NULL)
    {
      SizeT nCp=src->N_Elements();
      for( int c=0; c<nCp; ++c)
	(*this)[ c+offset]=(*src)[c];
    }
  else
    {
      SizeT nCp=ixList->N_Elements();

      AllIxBaseT* allIx = ixList->BuildIx();
      (*this)[ offset]=(*src)[ allIx->InitSeqAccess()];
      for( SizeT c=1; c<nCp; ++c)
	(*this)[ c+offset]=(*src)[ allIx->SeqAccess()];
    }
}


// used for array concatenation
template<class Sp>
Data_<Sp>* Data_<Sp>::CatArray( ExprListT& exprList,
				const SizeT catRankIx, 
				const SizeT rank)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  //  breakpoint();
  SizeT rankIx = RankIx( rank);
  SizeT maxIx = (catRankIx > rankIx)? catRankIx : rankIx;

  dimension     catArrDim(this->dim); // list contains at least one element

  catArrDim.InsureRankAtLeast( maxIx+1);
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
  
// special case: identity (c=[a]) equivalent to c=a, e.g. Dup(). Speedup
  if (exprList.size()==1 && (this->dim==catArrDim)) return this->Dup(); 

  // the concatenated array
  Data_<Sp>* catArr=New(catArrDim, BaseGDL::NOZERO);
  
  SizeT at=0;

// accelerated in CatInsert. Note that "at" is computed serially in CatInsert but can be computed externally (if one wanted to precompute it for parallel calls to CatInsert:
//  SizeT add=static_cast<Data_<Sp>*>( (*i))->dim[catRankIx]; // update 'at'
//  for( i=exprList.begin(); i != exprList.end(); ++i) at += (add > 1)? add : 1;

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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
  (*res)[0]=(*this)[ allIx->InitSeqAccess()];
  for( SizeT c=1; c<nCp; ++c)
    (*res)[c]=(*this)[ allIx->SeqAccess()];
  return res;
}

// inserts srcIn at index ixDim
// respects the exact structure of srcIn
template<class Sp>
void Data_<Sp>::InsAt( Data_* srcIn, ArrayIndexListT* ixList, SizeT offset)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
void Data_<Sp>::CatInsert (const Data_* srcArr, const SizeT atDim, SizeT& at)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  // length of one segment to copy
  SizeT len = srcArr->dim.Stride (atDim + 1); // src array

  SizeT nEl = srcArr->N_Elements ();
  // number of copy actions
  SizeT nCp = nEl / len;

  // initial offset
  SizeT destStart = this->dim.Stride (atDim) * at; // dest array
  SizeT destEnd = destStart + len;

  // number of elements to skip
  SizeT gap = this->dim.Stride (atDim + 1); // dest array
  
//GD: speed up by using indexing that permit parallel and collapse.
  if ((GDL_NTHREADS=parallelize( len*nCp, TP_CPU_INTENSIVE))==1) { //most frequent
    for (OMPInt c = 0; c < nCp; ++c) {
      for (SizeT destIx = 0; destIx < len; destIx++) (*this)[destIx + destStart + c * gap] = (*srcArr)[ destIx + c * len];
    }
  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) collapse(2)
    for (OMPInt c = 0; c < nCp; ++c)
      {
        //            // copy one segment
        //            SizeT destStartLoop = destStart + c * gap;
        //            SizeT destEndLoop   = destStartLoop + len;
        //            SizeT srcIxLoop     = c * len;
        //            std::cerr<<"ncp="<<nCp<<",c="<<c<<": start: "<<destStart+c*gap<<" end: "<<destStart + c * gap + len<<" srcIxLoop="<<c * len<<std::endl;
        for (SizeT destIx = 0; destIx < len; destIx++) (*this)[destIx + destStart + c * gap] = (*srcArr)[ destIx + c * len];
      }
  }

  SizeT add = srcArr->dim[atDim]; // update 'at'
  at += (add > 1) ? add : 1;
}

// Logical True
// integers
template<class Sp>
bool Data_<Sp>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0);
}
template<>
bool Data_<SpDFloat>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0.0f);
}
template<>
bool Data_<SpDDouble>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0.0);
}
template<>
bool Data_<SpDString>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != "");
}
template<>
bool Data_<SpDComplex>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (real(s) != 0.0 || imag(s) != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (real(s) != 0.0 || imag(s) != 0.0);
}
template<>
bool Data_<SpDPtr>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Ty s;
  if( !Scalar( s))
    throw GDLException("Expression must be a scalar or 1 element array in this context.",true,false);
  return (s != 0);
}
template<>
bool Data_<SpDObj>::LogTrue()
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  // ::_overloadIsTrue is handled in True()
  
  return this->True();
}
// structs are not allowed

// indexed version
// integers
template<class Sp>
bool Data_<Sp>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return ((*this)[i] != 0);
}
template<>
bool Data_<SpDFloat>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return ((*this)[i] != 0.0f);
}
template<>
bool Data_<SpDDouble>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return ((*this)[i] != 0.0);
}
template<>
bool Data_<SpDString>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return ((*this)[i] != "");
}
template<>
bool Data_<SpDComplex>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0);
}
template<>
bool Data_<SpDComplexDbl>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return ((*this)[i].real() != 0.0 || (*this)[i].imag() != 0.0);
}
template<>
bool Data_<SpDPtr>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return (*this)[i] != 0;
}
template<>
bool Data_<SpDObj>::LogTrue(SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return (*this)[i] != 0;
}

template<>
BaseGDL* Data_<SpDString>::Rebin( const dimension& newDim, bool sample)
{std::string t = BaseGDL::interpreter->Name(this);
if ( t.substr(0,12) != "<Expression>" ){
  throw GDLException("REBIN: String expression not allowed in this context: "+ t +".");
  
}else {
  t = t.erase(0,13);
  throw GDLException("REBIN: String expression not allowed in this context: "+ t +"."); 
}
}
template<>
BaseGDL* Data_<SpDComplex>::Rebin( const dimension& newDim, bool sample)
{std::string t = BaseGDL::interpreter->Name(this);
if ( t.substr(0,12) != "<Expression>" ){
  throw GDLException("REBIN: Complex expression not allowed in this context: "+ t +".");
  
}else {
  t = t.erase(0,13);
  throw GDLException("REBIN: Complex expression not allowed in this context: "+ t +"."); 
}
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Rebin( const dimension& newDim, bool sample)
{std::string t = BaseGDL::interpreter->Name(this);
if ( t.substr(0,12) != "<Expression>" ){
  throw GDLException("REBIN: Complex expression not allowed in this context: "+ t +".");
  
}else {
  t = t.erase(0,13);
  throw GDLException("REBIN: Complex expression not allowed in this context: "+ t +"."); 
}
}


// rebin over dimIx, new value: newDim
// newDim != srcDim[ dimIx] -> compress or expand
template< typename T>
T* Rebin1(T* src,
  const dimension& srcDim,
	SizeT dimIx, SizeT newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = src->N_Elements();
  if (newDim == 0) newDim = 1;

  // get dest dim and number of summations
  dimension destDim = srcDim;

  destDim.InsureRankAtLeast(dimIx + 1);

  SizeT srcDimIx = destDim[ dimIx];

  destDim.SetOneDim(dimIx, newDim); //reset stride

  SizeT dstStride = destDim.Stride(dimIx); //computes new strides for destdim
  SizeT dstOuterStride = destDim.Stride(dimIx + 1);

  // dimStride is also the number of linear src indexing
  SizeT srcStride = srcDim.Stride(dimIx);
  SizeT srcOuterStride = srcDim.Stride(dimIx + 1);
  SizeT rebinLimit = srcDimIx * srcStride;
  
  if (newDim < srcDimIx) // compress
  {  

    SizeT ratio = srcDimIx / newDim;
    if (sample) {
      
      T* res = new T(destDim, BaseGDL::NOZERO);
      SizeT dstnEl = res->N_Elements();
      for (SizeT o = 0, i=0; o < dstnEl; o += dstOuterStride, i+= srcOuterStride) {
      SizeT oiLimit = o+dstOuterStride;
        for (SizeT os = 0, is= 0; os < dstStride; ++os, ++is) // src element offset (lower dim)
        {
          SizeT oi = o + os;
          SizeT ii = i + is;
          for (SizeT s = oi, p=ii; s < oiLimit; s += dstStride , p+=srcStride*ratio) // run over dim
          {
           (*res)[ s] = (*src)[ p ];
          }
        }   
      }  
      return res;
    } else {
      T* res = new T(destDim, BaseGDL::NOZERO);
      SizeT dstnEl = res->N_Elements();
      for (SizeT o = 0, i=0; o < dstnEl; o += dstOuterStride, i+= srcOuterStride) {
      SizeT oiLimit = o+dstOuterStride;
        for (SizeT os = 0, is = 0; os < dstStride; ++os, ++is) // src element offset (lower dim)
        {
          SizeT oi = o + os;
          SizeT ii = i + is;
          for (SizeT s = oi, p = ii; s < oiLimit; s += dstStride) // run over dim
          {
            typename T::Ty val=0;
            for (SizeT j = 0; j < ratio; ++j, p += srcStride) {
              val += (*src)[ p ];
            }
            (*res)[ s] = val/ratio;
          }
        }
      }   
      return res;
    }
    
  } else // expand
  {
    
    T* res = new T(destDim, BaseGDL::NOZERO);
      SizeT ratio = newDim / srcDimIx; // make sure 32 bit integers are working also
      SizeT fact = ratio * srcStride;
    
    if (sample) {
      for (SizeT o = 0; o < nEl; o += srcOuterStride) { // outer dim
              for (SizeT i = 0; i < srcStride; ++i) // src element offset (lower dim)
        {
          SizeT oi = o + i;
          SizeT oiLimit = oi + rebinLimit;

          for (SizeT s = oi; s < oiLimit; s += srcStride) // run over dim
          {
            SizeT s_dimStride_ratio_dimStride_i =   (s / srcStride) * fact + i;
            typename T::Ty *me = &((*res)[ s_dimStride_ratio_dimStride_i ]);
            for (SizeT r = 0; r < ratio*srcStride; r+=srcStride) {
              me[r] = (*src)[s];
            }
          }
        }
      }
    } else {
      for (SizeT o = 0; o < nEl; o += srcOuterStride) // outer dim
        for (SizeT i = 0; i < srcStride; ++i) // src element offset (lower dim)
        {
          SizeT oi = o + i;
          SizeT oiLimit = oi + rebinLimit;

          for (SizeT s = oi; s < oiLimit; s += srcStride) // run over dim
          {
            typename T::Ty  first = (*src)[ s];
            typename T::Ty  incr = ( (*src)[ (s + srcStride) < oiLimit ? s + srcStride : s]-first)/ratio;
            SizeT s_dimStride_ratio_dimStride_i =  (s / srcStride) * fact + i; //integer division: slow.
            typename T::Ty *me = &((*res)[ s_dimStride_ratio_dimStride_i ]);
            for (SizeT r = 0, p=0; r < ratio*srcStride; r+=srcStride, ++p) {
              me[r] = first + incr * p; // will NOT give exactly IDL results in some cases: odd expansions for example.
            }
          }
        }
    }
    
    return res;
  }
}

// for integer

template< typename T, typename TNext>
T* Rebin1Int(T* src,
  const dimension& srcDim,
	     SizeT dimIx, SizeT newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nEl = src->N_Elements();
  if (newDim == 0) newDim = 1;

  // get dest dim and number of summations
  dimension destDim = srcDim;

  destDim.InsureRankAtLeast(dimIx + 1);

  SizeT srcDimIx = destDim[ dimIx];

  destDim.SetOneDim(dimIx, newDim); //reset stride

  SizeT dstStride = destDim.Stride(dimIx); //computes new strides for destdim
  SizeT dstOuterStride = destDim.Stride(dimIx + 1);

  // dimStride is also the number of linear src indexing
  SizeT srcStride = srcDim.Stride(dimIx);
  SizeT srcOuterStride = srcDim.Stride(dimIx + 1);
  SizeT rebinLimit = srcDimIx * srcStride;
  
  if (newDim < srcDimIx) // compress
  {

    SizeT ratio = srcDimIx / newDim;
    if (sample) {
      T* res = new T(destDim, BaseGDL::NOZERO);
      SizeT dstnEl = res->N_Elements();
      for (SizeT o = 0, i=0; o < dstnEl; o += dstOuterStride, i+= srcOuterStride) {
      SizeT oiLimit = o+dstOuterStride;

        for (SizeT os = 0, is= 0; os < dstStride; ++os, ++is) // src element offset (lower dim)
        {
          SizeT oi = o + os;
          SizeT ii = i + is;
          for (SizeT s = oi, p=ii; s < oiLimit; s += dstStride , p+=srcStride*ratio) // run over dim
          {
           (*res)[ s] = (*src)[ p ];
          }
        }        
      }
      return res;
    } else {
      T* res = new T(destDim);// MUST zero fields
      SizeT dstnEl = res->N_Elements();
      for (SizeT o = 0, i=0; o < dstnEl; o += dstOuterStride, i+= srcOuterStride) {
      SizeT oiLimit = o+dstOuterStride;
        for (SizeT os = 0, is = 0; os < dstStride; ++os, ++is) // src element offset (lower dim)
        {
          SizeT oi = o + os;
          SizeT ii = i + is;
          for (SizeT s = oi, p = ii; s < oiLimit; s += dstStride) // run over dim
          {
            TNext tmp = 0;
            for (SizeT j = 0; j < ratio; ++j, p += srcStride) {
              tmp += (*src)[ p ];
            }
            (*res)[ s] = tmp/ratio;
          }
        }
      }
      return res;    
    }
  
  } else // expand
  {
    T* res = new T(destDim, BaseGDL::NOZERO);
      DLong64 ratio = newDim / srcDimIx; // make sure 32 bit integers are working also
      SizeT fact = ratio * srcStride;

    if (sample) {
      for (SizeT o = 0; o < nEl; o += srcOuterStride) { // outer dim
              for (SizeT i = 0; i < srcStride; ++i) // src element offset (lower dim)
        {
          SizeT oi = o + i;
          SizeT oiLimit = oi + rebinLimit;

          for (SizeT s = oi; s < oiLimit; s += srcStride) // run over dim
          {
            SizeT s_dimStride_ratio_dimStride_i =   (s / srcStride) * fact + i;
            typename T::Ty *me = &((*res)[ s_dimStride_ratio_dimStride_i ]);
            for (SizeT r = 0; r < ratio*srcStride; r+=srcStride) {
              me[r] = (*src)[s];
            }
          }
        }
      }
    } else {
      for (SizeT o = 0; o < nEl; o += srcOuterStride) // outer dim
        for (SizeT i = 0; i < srcStride; ++i) // src element offset (lower dim)
        {
          SizeT oi = o + i;
          SizeT oiLimit = oi + rebinLimit;

          for (SizeT s = oi; s < oiLimit; s += srcStride) // run over dim
          {
            TNext first = (*src)[ s];
            TNext next = (*src)[ (s + srcStride) < oiLimit ? s + srcStride : s];
            SizeT s_dimStride_ratio_dimStride_i = (s / srcStride) * fact + i; //integer division: slow.
            typename T::Ty *me = &((*res)[ s_dimStride_ratio_dimStride_i ]);
            for (DLong64 r = 0, p=0; r < ratio*srcStride; r+=srcStride, ++p) {
//              me[r] = first + incr * p / ratio; //wrong : does not work with bytes
              me[r] = (first * (ratio - p) + next * p) / ratio; // idem IDL only if divide by integer is done this way. Difficult to optimize.
            }
          }
        }
    }

    return res;
  }
}

template<class Sp>
BaseGDL* Data_<Sp>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();
  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;

  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot sore 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
  
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protection  
    if (olddim == 0) olddim = 1; //protection  
    if (newdim < olddim ) {
      Data_* act = Rebin1(actIn, actDim, d, newdim, sample);
      actDim = act->Dim();
      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT,float>(d,key));
    }
    }
    if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = Rebin1(actIn, actDim, rit->first, newDim[rit->first], sample);
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }}
  if (actIn == this) return actIn->Dup();
  return actIn;
}

#define DO_COMPRESS_INT(X,Y) Rebin1Int< X, Y>(actIn, actDim, it->first, newDim[it->first], sample);
#define DO_EXPAND_INT(X,Y) Rebin1Int< X, Y>(actIn, actDim, rit->first, newDim[rit->first], sample);


template<>
BaseGDL* Data_<SpDByte>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();
  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;

  std::map<float, SizeT>compress;
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protetion  
    if (olddim == 0) olddim = 1; //protetion  
    if (newdim < olddim) {
      float key=float(olddim / newdim)+0.01*d;
      compress.insert(std::pair<SizeT, float>(d, key));
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT, float>(d, key));
    }
  }
  if (compress.size() > 0) {
    for (std::map<float, SizeT>::iterator it = compress.begin(); it != compress.end(); ++it) {
      Data_* act = DO_COMPRESS_INT(DByteGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = DO_EXPAND_INT(DByteGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot sore 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
 
  if (actIn == this) return actIn->Dup();
  return actIn;
}

template<>
BaseGDL* Data_<SpDInt>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();
  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;

  std::map<float, SizeT>compress;
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protetion  
    if (olddim == 0) olddim = 1; //protetion  
    if (newdim < olddim) {
      float key=float(olddim / newdim)+0.01*d;
      compress.insert(std::pair<SizeT, float>(d, key));
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT, float>(d, key));
    }
  }
  if (compress.size() > 0) {
    for (std::map<float, SizeT>::iterator it = compress.begin(); it != compress.end(); ++it) {
      Data_* act = DO_COMPRESS_INT(DIntGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = DO_EXPAND_INT(DIntGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot store 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
  
  if (actIn == this) return actIn->Dup();
  return actIn;
}

template<>
BaseGDL* Data_<SpDUInt>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();
  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;
  std::map<float, SizeT>compress;
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protetion  
    if (olddim == 0) olddim = 1; //protetion  
    if (newdim < olddim) {
      float key=float(olddim / newdim)+0.01*d;
      compress.insert(std::pair<SizeT, float>(d, key));
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT, float>(d, key));
    }
  }
  if (compress.size() > 0) {
    for (std::map<float, SizeT>::iterator it = compress.begin(); it != compress.end(); ++it) {
      Data_* act = DO_COMPRESS_INT(DUIntGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = DO_EXPAND_INT(DUIntGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot sore 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
  
  if (actIn == this) return actIn->Dup();
  return actIn;
}

template<>
BaseGDL* Data_<SpDLong>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();
  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;
  
  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot sore 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
  std::map<float, SizeT>compress;
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protetion  
    if (olddim == 0) olddim = 1; //protetion  
    if (newdim < olddim) {
      float key=float(olddim / newdim)+0.01*d;
      compress.insert(std::pair<SizeT, float>(d, key));
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT, float>(d, key));
    }
  }
  if (compress.size() > 0) {
    for (std::map<float, SizeT>::iterator it = compress.begin(); it != compress.end(); ++it) {
      Data_* act = DO_COMPRESS_INT(DLongGDL, DLong64);
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = DO_EXPAND_INT(DLongGDL, DLong64);
      actDim = act->Dim();
      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }

  if (actIn == this) return actIn->Dup();
  return actIn;
}

template<>
BaseGDL* Data_<SpDULong>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();

  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;

  std::map<float, SizeT>compress;
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protetion  
    if (olddim == 0) olddim = 1; //protetion  
    if (newdim < olddim) {
      float key=float(olddim / newdim)+0.01*d;
      compress.insert(std::pair<SizeT, float>(d, key));
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT, float>(d, key));
    }
  }
  if (compress.size() > 0) {
    for (std::map<float, SizeT>::iterator it = compress.begin(); it != compress.end(); ++it) {
      Data_* act = DO_COMPRESS_INT(DULongGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = DO_EXPAND_INT(DULongGDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot sore 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
  
  if (actIn == this) return actIn->Dup();
  return actIn;
}
template<>
BaseGDL* Data_<SpDLong64>::Rebin(const dimension& newDim, bool sample) { TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT resRank = newDim.Rank();
  SizeT srcRank = this->Rank();

  SizeT nDim;
  if (resRank < srcRank)
    nDim = srcRank;
  else
    nDim = resRank;

  dimension actDim(this->dim);
  Data_* actIn = this;
 
  std::map<float, SizeT>compress;
  std::map<float, SizeT>expand;
  for (SizeT d = 0; d < nDim; ++d) {
    SizeT newdim = newDim[d];
    SizeT olddim = this->dim[d];
    if (newdim == 0) newdim = 1; //protetion  
    if (olddim == 0) olddim = 1; //protetion  
    if (newdim < olddim) {
      float key=float(olddim / newdim)+0.01*d;
      compress.insert(std::pair<SizeT, float>(d, key));
    }
    if (newdim > olddim) {
      float key=float(newdim / olddim)+0.01*d;
      expand.insert(std::pair<SizeT, float>(d, key));
    }
  }
  if (compress.size() > 0) {
    for (std::map<float, SizeT>::iterator it = compress.begin(); it != compress.end(); ++it) {
      Data_* act = DO_COMPRESS_INT(DLong64GDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  if (expand.size() > 0) {
    for (std::map<float, SizeT>::reverse_iterator rit = expand.rbegin(); rit != expand.rend(); ++rit) {
      Data_* act = DO_EXPAND_INT(DLong64GDL, DLong64)
      actDim = act->Dim();

      if (actIn != this) GDLDelete(actIn);
      actIn = act;
    }
  }
  // 1st compress
  //optimization: start by compressing the dimension that will decrease total size the most.
  //the key is used to keep the map always in increasing order of key, so it's the compression factor,
  //but we cannot sore 2 identical compression factors, on 2 difeerent dimensions, in the map since the key must be unique.
  // so the key is a real, composed by the compression factor+dim/100. Sufficient to have 2 keys different and still order OK.
  
  if (actIn == this) return actIn->Dup();
  return actIn;
}
// plain copy of nEl from src
// no checking
template<class Sp>
void Data_<Sp>::Assign( BaseGDL* src, SizeT nEl)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
  for(long k=0; k < nEl; ++k)
    {
      (*this)[ k] = (*srcT)[ k];
    }
}
// plain copy of element at index i from src in scalar 'this'. To be used in loops to change the value of the loop variable.
// absolutely no checking for speed so BE AWARE to check equality of Type() before use.
template<class Sp>
void Data_<Sp>::AssignIndexedValue( BaseGDL* src, SizeT i)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  Data_* srcT = dynamic_cast<Data_*>( src);
  (*this)[ 0] = (*srcT)[ i];
}




// return a new type of itself (only for one dimensional case)
template<class Sp>
BaseGDL* Data_<Sp>::NewIx( SizeT ix)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return new Data_( (*this)[ ix]);
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( AllIxBaseT* ix, const dimension* dIn)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nCp = ix->size();
  Data_* res=Data_::New( *dIn, BaseGDL::NOZERO);
  if ((GDL_NTHREADS=parallelize( nCp, TP_MEMORY_ACCESS))==1) {
    for( int c=0; c<nCp; ++c)  (*res)[c]=(*this)[ (*ix)[ c]];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for( int c=0; c<nCp; ++c)  (*res)[c]=(*this)[ (*ix)[ c]];
  }
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFrom( SizeT s)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nCp = dd.size() - s;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  if ((GDL_NTHREADS=parallelize( nCp, TP_MEMORY_ACCESS))==1) {
   for( int c=0; c<nCp; ++c)  (*res)[c]=(*this)[ s+c];
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
   for( int c=0; c<nCp; ++c)  (*res)[c]=(*this)[ s+c];
  }
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFrom( SizeT s, SizeT e)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nCp = e - s + 1;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  if ((GDL_NTHREADS=parallelize( nCp, TP_MEMORY_ACCESS))==1) {
  for( int c=0; c<nCp; ++c)  (*res)[c]=(*this)[ s+c];
  }else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
  for( int c=0; c<nCp; ++c)  (*res)[c]=(*this)[ s+c];
  }
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFromStride( SizeT s, SizeT stride)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nCp = (dd.size() - s + stride - 1)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride) (*res)[c]=(*this)[ s];
  return res;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  SizeT nCp = (e - s + stride)/stride;
  Data_* res=Data_::New( dimension( nCp), BaseGDL::NOZERO);
  for( SizeT c=0; c<nCp; ++c, s += stride)  (*res)[c]=(*this)[ s];
  return res;
}



template<class Sp>
Data_<Sp>* Data_<Sp>::NewIx( BaseGDL* ix, bool strict)
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return (*this)[ i];
}
template<class Sp> SizeT Data_<Sp>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  return (*this)[ i]; // good for unsigned types
}

template<>
SizeT Data_<SpDInt>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] < 0)
    return 0;
  return (*this)[i];
}	
template<>
SizeT Data_<SpDInt>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] < 0)
    return 0;
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong64>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] < 0)
    return 0;
  return (*this)[i];
}	
template<>
SizeT Data_<SpDLong64>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] < 0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  return (*this)[i];
}	
template<>
SizeT Data_<SpDFloat>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,float>((*this)[i]);
}	
template<>
SizeT Data_<SpDFloat>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,float>((*this)[i]);
}	
template<>
SizeT Data_<SpDDouble>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,double>((*this)[i]);
}	
template<>
SizeT Data_<SpDDouble>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( (*this)[i] <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( (*this)[i] <= 0.0)
    return 0;
  return Real2Int<SizeT,double>((*this)[i]);
}	
template<>
SizeT Data_<SpDString>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
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
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,float>(real((*this)[i]));
}	
template<>
SizeT Data_<SpDComplex>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( real((*this)[i]) <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,float>(real((*this)[i]));
}	
template<>
SizeT Data_<SpDComplexDbl>::GetAsIndex( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,double>(real((*this)[i]));
}	
template<>
SizeT Data_<SpDComplexDbl>::GetAsIndexStrict( SizeT i) const
{ TRACE_ROUTINE(__FUNCTION__,__FILE__,__LINE__)
  if( real((*this)[i]) <= -1.0)
    throw GDLException(-1,NULL,"Array used to subscript array "
		       "contains out of range (<0) subscript (at index: " + i2s(i) + ").",true,false);
  if( real((*this)[i]) <= 0.0)
    return 0;
  return Real2Int<SizeT,double>(real((*this)[i]));
}
#ifdef ONE_DATAFILE
//#warning "using a big datatype.cpp file, compilation slow"
#include "basic_op_sub.cpp"
#include "basic_op_add.cpp"
#include "basic_op.cpp"
#include "basic_op_new.cpp"
#include "basic_op_div.cpp"
#include "basic_op_mult.cpp"
#include "default_io.cpp"
#include "ifmt.cpp"
#include "ofmt.cpp"
#endif
#include "instantiate_templates.hpp"
