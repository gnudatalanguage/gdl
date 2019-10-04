/***************************************************************************
                          gdlarray.hpp  -  basic typedefs
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

#ifndef GDLARRAY_HPP_
#define GDLARRAY_HPP_

// #define GDLARRAY_CACHE
#undef GDLARRAY_CACHE

//#define GDLARRAY_DEBUG
#undef GDLARRAY_DEBUG

// for complex (of POD)
const bool TreatPODComplexAsPOD = true;

template <typename T, bool IsPOD>
class GDLArray
{
private:
	enum GDLArrayConstants
	{
		smallArraySize = 27,
		maxCache = 1000 * 1000 // ComplexDbl is 16 bytes
	};
		
	typedef T Ty;

#ifdef USE_EIGEN  
  EIGEN_ALIGN16 char scalarBuf[ smallArraySize * sizeof(Ty)];
#else
  char scalarBuf[ smallArraySize * sizeof(Ty)];
#endif
  
  Ty* InitScalar()
  {
    assert( sz <= smallArraySize);
    if( IsPOD)
    {
      return reinterpret_cast<Ty*>(scalarBuf);
    }
    else
    {
      Ty* b = reinterpret_cast<Ty*>(scalarBuf);
#pragma omp parallel for if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))      
      for( int i = 0; i<sz; ++i) new (&(b[ i])) Ty();
      return b;
    }
  }

#ifdef GDLARRAY_CACHE
#error "GDLARRAY_CACHE defined"
  static SizeT cacheSize;
  static Ty* cache;
  static Ty* Cached( SizeT newSize);
#endif
  
  Ty*   buf;
  SizeT sz;

  Ty* New( SizeT s)
  {
// We should align all our arrays on the boundary that will be beneficial for the acceleration of the machine GDL is built,
// as sse and other avx need 32,64..512 alignment. Not necessary on the EIGEN_ALIGN_16, and not only if we use Eigen:: as some code (median filter, random)
// uses hardware acceleration independently of whatever speedup Eigen:: may propose. Note that according to http://eigen.tuxfamily.org/dox/group__CoeffwiseMathFunctions.html
// Eigen:: may eventually use sse2 or avx on reals but not doubles, etc.
// As Eigen::internal::aligned_new is SLOW FOR NON-PODS and sdt::complex is a NON-POD for the compiler (but not for us), we use gdlAlignedMalloc for all PODS.
// Normally, Everything should be allocated using gdlAlignedMalloc with the 'good' alignment, not only in the USE_EIGEN case.
// Unfortunately gdlAlignedMalloc uses Eigen::internal::alogned_malloc at the moment. Todo Next.
#ifdef USE_EIGEN
   if (IsPOD) return (Ty*) gdlAlignedMalloc(s*sizeof(Ty)); else return Eigen::internal::aligned_new<Ty>( s);
#else
    return new Ty[ s];
#endif
  }
    
public:
  GDLArray() throw() : buf( NULL), sz( 0) {}
  
#ifndef GDLARRAY_CACHE

  ~GDLArray() throw()
  {
  if( IsPOD)
    {
#ifdef USE_EIGEN  
    if ( buf != reinterpret_cast<Ty*>(scalarBuf)) gdlAlignedFree(buf);
//	Eigen::internal::aligned_delete( buf, sz);
#else
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) 
	delete[] buf; // buf == NULL also possible
#endif
    // no cleanup of "buf" here
    }
  else
    {
#ifdef USE_EIGEN  
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) Eigen::internal::aligned_delete( buf, sz);
    else
      for( int i = 0; i<sz; ++i) buf[i].~Ty();
#else
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) delete[] buf; // buf == NULL also possible
    else
      for( int i = 0; i<sz; ++i) buf[i].~Ty();
#endif
    }
  }

  GDLArray( const GDLArray& cp) : sz( cp.size())
  {
      try {
	buf = (cp.size() > smallArraySize) ? New(cp.size()) /*new Ty[ cp.size()]*/ : InitScalar();
      } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
#pragma omp parallel for if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
      for( SizeT i=0; i<sz; ++i)	buf[ i] = cp.buf[ i];
  }

  GDLArray( SizeT s, bool dummy) : sz( s)
  {
    try {
      buf = (s > smallArraySize) ? New(s) /*T[ s]*/ : InitScalar();
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
  }
  
  GDLArray( T val, SizeT s) : sz( s)
  {
    try {
	    buf = (s > smallArraySize) ? New(s) /*T[ s]*/ : InitScalar();
    } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
#pragma omp parallel for if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for( SizeT i=0; i<sz; ++i) buf[ i] = val;
  }
  
  GDLArray( const T* arr, SizeT s) : sz( s)
  {   
      try {
	buf = (s > smallArraySize) ? New(s) /*new Ty[ s]*/: InitScalar();
      } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
#pragma omp parallel for if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz)) 
      for( SizeT i=0; i<sz; ++i)	buf[ i] = arr[ i];
  }

#else // GDLARRAY_CACHE

  // use definition in datatypes.cpp
  GDLArray( const GDLArray& cp) ;
  GDLArray( SizeT s, bool b) ;
  GDLArray( T val, SizeT s) ;
  GDLArray( const T* arr, SizeT s) ;
  ~GDLArray() throw();

#endif // GDLARRAY_CACHE
  
  // scalar
  explicit GDLArray( const T& s) throw() : sz( 1)
  { 
    if( IsPOD)
    {
      buf = reinterpret_cast<Ty*>(scalarBuf);
      buf[0] = s;    
    }
    else
    {
      Ty* b = reinterpret_cast<Ty*>(scalarBuf); 
      new (&(b[ 0])) Ty( s);
      buf = b;
    }
  }

  T& operator[]( SizeT ix) throw()
  {
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
  assert( &right != this);
  assert ( sz == right.size() );
  if( IsPOD)
  {
    std::memcpy(buf,right.buf,sz*sizeof(Ty));
  }
  else
  {
#pragma omp parallel for   if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for ( SizeT i=0; i<sz; ++i )
	buf[ i] = right.buf[ i];
  }    
}

GDLArray& operator= ( const GDLArray& right )
{
  assert( this != &right);
  assert( sz == right.size());
  if( IsPOD)
  {
    std::memcpy(buf,right.buf,sz*sizeof(Ty));
  }
  else
  {
#pragma omp parallel for   if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for ( SizeT i=0; i<sz; ++i )
      buf[ i] = right.buf[ i];
  }
  return *this;
}

  GDLArray& operator+=( const GDLArray& right) throw()
  {
#pragma omp parallel for   if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right.buf[ i];
    return *this;
  }
  GDLArray& operator-=( const GDLArray& right) throw()
  {
#pragma omp parallel for   if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right.buf[ i];
    return *this;
  }

  GDLArray& operator+=( const T& right) throw()
  {
#pragma omp parallel for   if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right;
    return *this;
  }
  GDLArray& operator-=( const T& right) throw()
  {
#pragma omp parallel for   if (sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= sz))
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right;
    return *this;
  }

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
    sz = newSz;
    if ( sz > smallArraySize )
    {
      try
      {
	buf = New(sz) /*new T[ newSz]*/;
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
      buf = InitScalar();
    }
  }
  
// protected:
//     void assert(ix<sz arg1);
// protected:
//     void assert(ix<sz arg1);
}; // GDLArray

#endif
