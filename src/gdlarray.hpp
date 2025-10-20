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

//#define TRACE_GDLARRAY_OMP_CALLS
#undef TRACE_GDLARRAY_OMP_CALLS
#if defined(_OPENMP) && defined(TRACE_GDLARRAY_OMP_CALLS)
#define GDLARRAY_TRACEOMP( file, line)  TRACEOMP( file, line) 
#else
#define GDLARRAY_TRACEOMP( file, line) 
#endif
// for complex (of POD)
const bool TreatPODComplexAsPOD = true;

#define BUFFERSIZE 256 // Provision for std::string, size=32 .

template <typename T> static const int FixedBufferSize(){
	static const int sz=BUFFERSIZE/sizeof(T);
	assert (sz != 0);
	return sz;
}

template <typename T, bool IsPOD>
class GDLArray {
private:

  enum GDLArrayConstants {
    maxCache = 1000 * 1000 // ComplexDbl is 16 bytes
  };

  typedef T Ty;

#ifdef USE_EIGEN  
  EIGEN_ALIGN16 char scalarBuf[ BUFFERSIZE ];
#else
  char scalarBuf[ BUFFERSIZE ];
#endif

  Ty* InitScalar();

#ifdef GDLARRAY_CACHE
#error "GDLARRAY_CACHE defined"
  static SizeT cacheSize;
  static Ty* cache;
  static Ty* Cached(SizeT newSize);
#endif

  Ty* buf;
  SizeT sz;

  Ty* New(SizeT s);

public:

  GDLArray() throw () : buf(NULL), sz(0) {
  }

#ifndef GDLARRAY_CACHE

  ~GDLArray() throw();

  GDLArray(const GDLArray& cp);
  GDLArray(SizeT s, bool dummy);
  GDLArray(T val, SizeT s);
  GDLArray(const T* arr, SizeT s);

#else // GDLARRAY_CACHE

  // use definition in datatypes.cpp
  GDLArray(const GDLArray& cp);
  GDLArray(SizeT s, bool b);
  GDLArray(T val, SizeT s);
  GDLArray(const T* arr, SizeT s);
  ~GDLArray() throw ();

#endif // GDLARRAY_CACHE

  // scalar

  explicit GDLArray(const T& s) throw () ;

  T& operator[](SizeT ix){
    assert(ix < sz);
    return buf[ ix];
  }

  const T& operator[](SizeT ix) const{
    assert(ix < sz);
    return buf[ ix];
  }

  // private: // disable
  // only used (indirect) by DStructGDL::DStructGDL(const DStructGDL& d_)

  void InitFrom(const GDLArray& right);

  GDLArray& operator=(const GDLArray& right);

  GDLArray& operator+=(const GDLArray& right) throw();

  GDLArray& operator-=(const GDLArray& right) throw();

  GDLArray& operator+=(const T& right) throw();

  GDLArray& operator-=(const T& right) throw() ;

  void SetBuffer(T* b) throw () {
    buf = b;
  }

  T* GetBuffer() throw () {
    return buf;
  }

  void SetBufferSize(SizeT s) throw () {
    sz = s;
  }

  SizeT size() const throw () {
    return sz;
  }

  void SetSize(SizeT newSz);
}; // GDLArray

#endif
