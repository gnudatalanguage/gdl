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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//#else
//#error "config.h required. Compile with -DHAVE_CONFIG_H"
//#endif
#ifdef _MSC_VER
#define NOMINMAX
#define _WINSOCKAPI_
#pragma warning( disable : 4716 )
#endif

// Python.h must be included before everything else
#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

// save HAVE_LIBREADLINE status (Python.h defines HAVE_LIBREADLINE)
#ifndef HAVE_LIBREADLINE
#define GDL_NOT_HAVE_READLINE
#endif

//#undef _POSIX_C_SOURCE // get rid of warning
#include <Python.h>
//#ifndef _POSIX_C_SOURCE 
//#warning "_POSIX_C_SOURCE not defined in Python.h (remove #undef)"
//#endif

// for the python module pyhton's readline is used
// this is ok as long as there is no GDL prompt within python
#ifdef PYTHON_MODULE
#undef HAVE_LIBREADLINE
#endif

#ifdef GDL_NOT_HAVE_READLINE
#undef HAVE_LIBREADLINE
#endif

#undef GDL_NOT_HAVE_READLINE

//#if defined(USE_PYTHON) || defined(PYTHON_MODULE)
#endif

#if defined(USE_EIGEN)
#include <Eigen/Core>
#endif

#if defined(__sun__)
// SA: CS is defined in /usr/include/sys/regset.h and used in an enum statement by ANTLR
#  include <sys/regset.h>
#  undef CS
#  undef GS
#endif

#endif
