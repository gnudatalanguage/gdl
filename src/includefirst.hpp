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

#ifdef __CYGWIN__
//  std::cerr is  broken in gcc/cygwin64 - for gdl, anyways.
#define cerr cout
#endif
// #undef cerr if you want to try it.

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN 1
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef _MSC_VER
#define NOMINMAX
#define _WINSOCKAPI_
#pragma warning( disable : 4716 )
#endif

// Python.h must be included before everything else
#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

#ifndef HAVE_LIBREADLINE
#define GDL_NOT_HAVE_READLINE
#endif

#include <Python.h>

#ifdef GDL_NOT_HAVE_READLINE
#undef HAVE_LIBREADLINE
#endif

#undef GDL_NOT_HAVE_READLINE

#endif

#include <cstddef>
#include <cstdlib>
#include <cstring>

#if defined(USE_EIGEN)
#include <Eigen/Core>
#endif

#if defined(__sun__)
// SA: CS is defined in /usr/include/sys/regset.h and used in an enum statement by ANTLR
#  include <sys/regset.h>
#  undef CS
#  undef GS
#endif

// This will be the boundaries on which DATA (i.e., something that is accessed through the DataAddr() pointer of
// any GDL variables (ex: DFloatGDL*)... is ALIGNED.
// Must be in accordance with Eigen::'s requirements. AND all the other alignment requirements of various SIMD-oriented
// code such as dSFMT etc.
#define GDL_GLOBAL_ALIGNMENT 16


//define globally a gdl method for allocating/deallocating/freeing data compatible with alignment needed by some
// libraries (Eigen, but also all accelerated code)
//This permits to create unallocated data variables (BaseGDL* objects) and have them point to some
// data adress that has been allocated in another c or c++ function. This makes, when correctly used, the economy of
// time in copying data computed by (external) library code into an otherwise initialized BaseGDLsomething variable.
inline void* gdlAlignedMalloc(std::size_t size) {
#if defined(USE_EIGEN)
 return Eigen::internal::aligned_malloc(size); //the simplest way to stay Eigen compatible!
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

// note: clang does know std::free. replace by free?
inline void gdlAlignedFree(void* ptr) {
#if defined(USE_EIGEN)
 if (ptr) return Eigen::internal::aligned_free(ptr);
#else
 if (ptr) free(*(reinterpret_cast<void**>(ptr) - 1));
#endif
}
#endif
