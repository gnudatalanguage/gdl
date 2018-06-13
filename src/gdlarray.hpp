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
      for( int i = 0; i<sz; ++i) 
	new (&(b[ i])) Ty();
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
// better align all data, also POD    
// as compound types might benefit from it as well
#ifdef USE_EIGEN 
    return Eigen::internal::aligned_new<Ty>( s);
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
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) 
	Eigen::internal::aligned_delete( buf, sz);
#else
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) 
	delete[] buf; // buf == NULL also possible
#endif
    // no cleanup of "buf" here
    }
  else
    {
#ifdef USE_EIGEN  
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) 
	Eigen::internal::aligned_delete( buf, sz);
    else
      for( int i = 0; i<sz; ++i) 
	buf[i].~Ty();
#else
    if( buf != reinterpret_cast<Ty*>(scalarBuf)) 
	delete[] buf; // buf == NULL also possible
    else
      for( int i = 0; i<sz; ++i) 
	buf[i].~Ty();
#endif
    }
  }

  GDLArray( const GDLArray& cp) : sz( cp.size())
  {
    if( IsPOD)
    {
      try {
	  buf = (cp.size() > smallArraySize) ? New(cp.size()) /*New T[ cp.size()]*/ : InitScalar();
      } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }

      std::memcpy(buf,cp.buf,sz*sizeof(T));
    }
    else
    {
      try {
	buf = (cp.size() > smallArraySize) ? New(cp.size()) /*new Ty[ cp.size()]*/ : InitScalar();
      } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
      for( SizeT i=0; i<sz; ++i)
	buf[ i] = cp.buf[ i];
    }
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

    for( SizeT i=0; i<sz; ++i)
      buf[ i] = val;
  }
  
  GDLArray( const T* arr, SizeT s) : sz( s)
  {
    if( IsPOD)
    {
      try
      {
	      buf = ( s > smallArraySize ) ? New(s) /*T[ s]*/: InitScalar();
      }
      catch ( std::bad_alloc& ) { ThrowGDLException ( "Array requires more memory than available" ); }

      std::memcpy(buf,arr,sz*sizeof(T));
    }
    else
    {    
      try {
	buf = (s > smallArraySize) ? New(s) /*new Ty[ s]*/: InitScalar();
      } catch (std::bad_alloc&) { ThrowGDLException("Array requires more memory than available"); }
      for( SizeT i=0; i<sz; ++i)
	buf[ i] = arr[ i];
      }
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
      if( ix >= sz) std::cout << "GDLArray line 210 ix=" << ix
				<<", sz = " << sz << " indexing overflow" << std::endl;
    assert( ix < sz);  // see note in basic_fun.cpp at obj_valid()
    return buf[ ix];
  }
  const T& operator[]( SizeT ix) const throw()
  {
//     if( ix >= sz) // debug 
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
    for ( SizeT i=0; i<sz; ++i )
      buf[ i] = right.buf[ i];
  }
  return *this;
}

  GDLArray& operator+=( const GDLArray& right) throw()
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right.buf[ i];
    return *this;
  }
  GDLArray& operator-=( const GDLArray& right) throw()
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] -= right.buf[ i];
    return *this;
  }

  GDLArray& operator+=( const T& right) throw()
  {
    for( SizeT i=0; i<sz; ++i)
      buf[ i] += right;
    return *this;
  }
  GDLArray& operator-=( const T& right) throw()
  {
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
