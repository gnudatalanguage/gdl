/***************************************************************************
                          gdlarray.cpp  -  basic typedefs
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

#include "basegdl.hpp"
#include "dstructdesc.hpp"
#include "gdlarray.hpp"
template<class T, bool IsPOD>
T* GDLArray<T,IsPOD>::InitScalar() {
    assert(sz <= smallArraySize);
    if (IsPOD) {
      return reinterpret_cast<T*> (scalarBuf);
    } else {
      T* b = reinterpret_cast<T*> (scalarBuf);
      for (int i = 0; i < sz; ++i) new (&(b[ i])) T(); //not parallel as ALWAYS SMALL NUMBER!
      return b;
    }
  }

template<class T, bool IsPOD>
  T* GDLArray<T,IsPOD>::New(SizeT s) {
    // We should align all our arrays on the boundary that will be beneficial for the acceleration of the machine GDL is built,
    // as sse and other avx need 32,64..512 alignment. Not necessary on the EIGEN_ALIGN_16, and not only if we use Eigen:: as some code (median filter, random)
    // uses hardware acceleration independently of whatever speedup Eigen:: may propose. Note that according to http://eigen.tuxfamily.org/dox/group__CoeffwiseMathFunctions.html
    // Eigen:: may eventually use sse2 or avx on reals but not doubles, etc.
    // As Eigen::internal::aligned_new is SLOW FOR NON-PODS and sdt::complex is a NON-POD for the compiler (but not for us), we use gdlAlignedMalloc for all PODS.
    // Normally, Everything should be allocated using gdlAlignedMalloc with the 'good' alignment, not only in the USE_EIGEN case.
    // Unfortunately gdlAlignedMalloc uses Eigen::internal::alogned_malloc at the moment. Todo Next.
#ifdef USE_EIGEN
    if (IsPOD) return (T*) gdlAlignedMalloc(s * sizeof (T));
    else return Eigen::internal::aligned_new<T>(s);
#else
    return new T[ s];
#endif
  }

#ifndef GDLARRAY_CACHE

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>::~GDLArray() throw () {
    if (IsPOD) {
#ifdef USE_EIGEN  
      if (buf != reinterpret_cast<T*> (scalarBuf)) gdlAlignedFree(buf);
      //	Eigen::internal::aligned_delete( buf, sz);
#else
      if (buf != reinterpret_cast<T*> (scalarBuf))
        delete[] buf; // buf == NULL also possible
#endif
      // no cleanup of "buf" here
    } else {
#ifdef USE_EIGEN  
      if (buf != reinterpret_cast<T*> (scalarBuf)) Eigen::internal::aligned_delete(buf, sz);
      else
        for (int i = 0; i < sz; ++i) buf[i].~T();
#else
      if (buf != reinterpret_cast<T*> (scalarBuf)) delete[] buf; // buf == NULL also possible
      else
        for (int i = 0; i < sz; ++i) buf[i].~T();
#endif
    }
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>::GDLArray(const GDLArray& cp) : sz(cp.size()) {
    try {
      buf = (cp.size() > smallArraySize) ? New(cp.size()) /*new T[ cp.size()]*/ : InitScalar();
    } catch (std::bad_alloc&) {
      ThrowGDLException("Array requires more memory than available");
    }
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] = cp.buf[ i];
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] = cp.buf[ i];
    }
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>::GDLArray(SizeT s, bool dummy) : sz(s) {
    try {
      buf = (s > smallArraySize) ? New(s) /*T[ s]*/ : InitScalar();
    } catch (std::bad_alloc&) {
      ThrowGDLException("Array requires more memory than available");
    }
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>::GDLArray(T val, SizeT s) : sz(s) {
    try {
      buf = (s > smallArraySize) ? New(s) /*T[ s]*/ : InitScalar();
    } catch (std::bad_alloc&) {
      ThrowGDLException("Array requires more memory than available");
    }
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] = val;
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] = val;
    }
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>::GDLArray(const T* arr, SizeT s) : sz(s) {
    try {
      buf = (s > smallArraySize) ? New(s) /*new T[ s]*/ : InitScalar();
    } catch (std::bad_alloc&) {
      ThrowGDLException("Array requires more memory than available");
    }
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] = arr[ i];
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] = arr[ i];
    }
  }


#endif // GDLARRAY_CACHE

  // scalar

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>::GDLArray(const T& s) throw () : sz(1) {
    if (IsPOD) {
      buf = reinterpret_cast<T*> (scalarBuf);
      buf[0] = s;
    } else {
      T* b = reinterpret_cast<T*> (scalarBuf);
      new (&(b[ 0])) T(s);
      buf = b;
    }
  }

  // private: // disable
  // only used (indirect) by DStructGDL::DStructGDL(const DStructGDL& d_)

template<class T, bool IsPOD>
  void GDLArray<T,IsPOD>::InitFrom(const GDLArray& right) {
    assert(&right != this);
    assert(sz == right.size());
    if (IsPOD) {
      std::memcpy(buf, right.buf, sz * sizeof (T));
    } else {
      bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
      if (!parallelize) {
        for (SizeT i = 0; i < sz; ++i) buf[ i] = right.buf[ i];
      } else {
        GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
          for (SizeT i = 0; i < sz; ++i) buf[ i] = right.buf[ i];
      }
    }
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>& GDLArray<T,IsPOD>::operator=(const GDLArray<T,IsPOD>& right) {
    assert(this != &right);
    assert(sz == right.size());
    if (IsPOD) {
      std::memcpy(buf, right.buf, sz * sizeof (T));
    } else {
      bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
      if (!parallelize) {
        for (SizeT i = 0; i < sz; ++i) buf[ i] = right.buf[ i];
      } else {
        GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
          for (SizeT i = 0; i < sz; ++i) buf[ i] = right.buf[ i];
      }
    }
    return *this;
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>& GDLArray<T,IsPOD>::operator+=(const GDLArray<T,IsPOD>& right) throw () {
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] += right.buf[ i];
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] += right.buf[ i];
    }
    return *this;
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>& GDLArray<T,IsPOD>::operator-=(const GDLArray<T,IsPOD>& right) throw () {
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] -= right.buf[ i];
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] -= right.buf[ i];
    }
    return *this;
  }
template<>
  GDLArray<DString,true>& GDLArray<DString,true>::operator-=(const GDLArray<DString,true>& right) throw () {
   assert(false);
   return *this;
  }
template<>
  GDLArray<DString,false>& GDLArray<DString,false>::operator-=(const GDLArray<DString,false>& right) throw () {
   assert(false);
   return *this;
  }
template<class T, bool IsPOD>
  GDLArray<T,IsPOD>& GDLArray<T,IsPOD>::operator+=(const T& right) throw () {
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] += right;
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] += right;
    }
    return *this;
  }

template<class T, bool IsPOD>
  GDLArray<T,IsPOD>& GDLArray<T,IsPOD>::operator-=(const T& right) throw () {
    bool parallelize = (CpuTPOOL_NTHREADS > 1 && sz >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS >= sz));
    if (!parallelize) {
      for (SizeT i = 0; i < sz; ++i) buf[ i] -= right;
    } else {
      GDLARRAY_TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(CpuTPOOL_NTHREADS)
        for (SizeT i = 0; i < sz; ++i) buf[ i] -= right;
    }
    return *this;
  }
template<>
  GDLArray<DString,true>& GDLArray<DString,true>::operator-=(const DString& right) throw () {
   assert(false);
    return *this;
  }
template<>
  GDLArray<DString,false>& GDLArray<DString,false>::operator-=(const DString& right) throw () {
   assert(false);
    return *this;
  }
template<class T, bool IsPOD>
  void GDLArray<T,IsPOD>::SetSize(SizeT newSz) // only used in DStructGDL::DStructGDL( const string& name_) (dstructgdl.cpp)
  {
    assert(sz == 0);
    sz = newSz;
    if (sz > smallArraySize) {
      try {
        buf = New(sz) /*new T[ newSz]*/;
      }      catch (std::bad_alloc&) {
        ThrowGDLException("Array requires more memory than available");
      }
    } else {
      // default constructed instances have buf == NULL and size == 0
      // make sure buf is set corectly if such instances are resized
      buf = InitScalar();
    }
  }

template class GDLArray< DByte,true>;
template class GDLArray< DInt,true>;
template class GDLArray< DUInt,true>;
template class GDLArray< DLong,true>;
template class GDLArray< DULong,true>;
template class GDLArray< DLong64,true>;
template class GDLArray< DULong64,true>;
template class GDLArray< DFloat,true>;
template class GDLArray< DDouble,true>;
template class GDLArray< DString,true>;
template class GDLArray< DString,false>;
template class GDLArray< DComplex,true>;
template class GDLArray< DComplexDbl,true>;
template class GDLArray< char,true>;
template class GDLArray< char,false>;
