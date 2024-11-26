//--------------------------------------------------------------------------
//
// File:           nan.h
//
// Created:        18/10/2001
//
// Author:         Pavel Sakov
//                 CSIRO Marine Research
//
// Purpose:        NaN definition
//
// Description:    Should cover machines with 64 bit doubles or other machines
//                 with GCC
//
// Revisions:      None
//
//--------------------------------------------------------------------------

#if !defined ( _NAN_H )
#define _NAN_H

#if ( defined ( __GNUC__ ) && !defined ( __ICC ) ) || defined ( __BORLANDC__ )

static const double NaN = 0.0 / 0.0;

#ifdef __BORLANDC__
  #define isnan       _isnan
  #define copysign    _copysign
#endif

#elif defined ( _WIN32 )

#if !defined ( _MSC_VER )

static unsigned _int64 lNaN = ( (unsigned _int64) 1 << 63 ) - 1;

#define NaN    ( *(double *) &lNaN )

#else

// MSVC/C++ 6.0 reports a version 1200,
// MSVC/C++ 2003 reports a version 1300
//
#include <float.h>
#include <ymath.h>
#undef NaN
#if _MSC_VER < 1300
#define NaN         _Nan._D
#else
#define NaN         _Nan._Double
#endif

#define isnan       _isnan
#define copysign    _copysign

#endif

#else

static const long long lNaN = ( (unsigned long long) 1 << 63 ) - 1;

#define NaN    ( *(double *) &lNaN )

#endif

#endif
