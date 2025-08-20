/***************************************************************************
                          includefirst.hpp  -  include this first
                             -------------------
    begin                : Wed Apr 18 16:58:14 JST 2005
    copyright            : (C) 2002-2006 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INCLUDEFIRST_HPP_
#define INCLUDEFIRST_HPP_

// define something to give the visibility of a function /class
// see https://gcc.gnu.org/wiki/Visibility for the whole story, noticeably the problem of catching C++ exceptions. 
// if we use this method along with  -fvisibility=hidden passed to the gcc compiler,
// then only DLL_PUBLIC entries will be visible outside.
#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport)
#endif
#define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
#define DLL_PUBLIC
#define DLL_LOCAL
  #endif
#endif

//prevent register to take effect when including old code.
#if __cplusplus > 199711L
#define register      // Deprecated in C++11.
#endif  // #if __cplusplus > 199711L

#ifdef __CYGWIN__
//  std::cerr is  broken in gcc/cygwin64 - for gdl, anyways.
#define cerr cout
#endif
// #undef cerr if you want to try it.

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN 1
// JP: Assume that we only use unicode on Windows.
#  define _UNICODE
#  define UNICODE
#  include <windows.h>
#  include <winsock2.h> // mandatory - at least for gethostname
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef _MSC_VER
#define NOMINMAX
#define _WINSOCKAPI_
#pragma warning( disable : 4716 )
#endif

#include <cstddef>
#include <cstdlib>
#include <algorithm>

#if defined(USE_EIGEN)
#include <Eigen/Core>
#endif

#if defined(__sun__)
// SA: CS is defined in /usr/include/sys/regset.h and used in an enum statement by ANTLR
#  include <sys/regset.h>
#  undef CS
#  undef GS
#endif

//define globally a gdl method for allocating/deallocating/freeing data compatible with alignment needed by EIGEN.
//This permits to create unallocated data variable (BaseGDL* objects) and have them point to some
// data adress that has been allocated in a c or c++ function.
//NOTE THIS RELIES ON EIGEN today so its not aligned without EIGEN. ~/nbut we could use C11 aligned_alloc 
inline void* gdlAlignedMalloc(std::size_t size) {
#if defined(USE_EIGEN)
 return Eigen::internal::aligned_malloc(size);
#else
 return std::malloc(size);
#endif
}

inline void* gdlAlignedRealloc(void *ptr, std::size_t new_size, std::size_t old_size=0) { //apparently Eigen does not use old_size anymore. Pfewh.
#if defined(USE_EIGEN)
 return Eigen::internal::aligned_realloc(ptr,new_size,old_size);
#else
 return std::realloc(ptr, new_size);
#endif
}

inline void gdlAlignedFree(void* ptr) {
#if defined(USE_EIGEN)
 Eigen::internal::aligned_free(ptr);
#else
 free(ptr);
#endif
 ptr=NULL;
}


#ifdef _OPENMP
#include <omp.h>

inline int currentNumberOfThreads() {
  return omp_get_num_threads();
}
#define DSFMT_MAX_PARALLEL_SEEDS 4
inline int maxNumberOfThreadsForDSFMT() {
  return std::min (DSFMT_MAX_PARALLEL_SEEDS,omp_get_num_procs());
}
inline int numberOfProcs() {
  return omp_get_num_procs();
}
inline int currentThreadNumber() {
return omp_get_thread_num();
}
#else
inline int currentNumberOfThreads() {
  return 1;
}

inline int maxNumberOfThreadsForDSFMT() {
  return 1;
}

inline int currentThreadNumber() {
return 0;
}
#endif

#endif
