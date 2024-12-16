#ifndef GDL_UTIL_HPP_
#define GDL_UTIL_HPP_
#include "basegdl.hpp"

//templates must be in the same namespace as where thet are used.
namespace gdl {
//in practice due to specialization this is for integer types only.  
 template <typename T1> T1 powI(const T1 x, const DLong y) {
//All x integer types
  if (y == 0) return 1;
  if (y < 0) {
    // case x=1 or -1 is special, other values give 0 ( as integer division of 1 by x^-y with y negative )
    if (x == 1 ) { return 1;}
    else if (x == -1 ) {
      if ( y & 0x1 ) return -1; else return 1;
    }
    else return 0;
  }

  const int nBits = sizeof (DLong) * 8;

  T1 arr = x;
  T1 res = 1;
  DLong mask = 1;
  for (SizeT i = 0; i < nBits; ++i) {
    if (y & mask) res *= arr;
    mask <<= 1;
    if (y < mask) return res;
    arr *= arr;
  }

  return res;
}

  //make specializations inline as they are seen by more than one .obj
template <>
inline DFloat powI(const DFloat x, const DLong yy) {
//All x integer types
  if (yy == 0) return 1;
  const int nBits = sizeof (DLong) * 8;

  DLong y = yy;
  DFloat arr = x;
  if (y < 0) {
    arr=1/arr;
    y=-y;
  }
  DFloat res = 1;
  DLong mask = 1;
  for (SizeT i = 0; i < nBits; ++i) {
    if (y & mask) res *= arr;
    mask <<= 1;
    if (y < mask) return res;
    arr *= arr;
  }

  return res;
}
template <>
inline DDouble powI(const DDouble x, const DLong yy) {
//All x integer types
  if (yy == 0) return 1;
  const int nBits = sizeof (DLong) * 8;
  


  DLong y = yy;
  DDouble arr = x;
  if (y < 0) {
    arr=1/arr;
    y=-y;
  }
  DDouble res = 1;
  DLong mask = 1;
  for (SizeT i = 0; i < nBits; ++i) {
    if (y & mask) res *= arr;
    mask <<= 1;
    if (y < mask) return res;
    arr *= arr;
  }

  return res;
}
template <>
inline DComplex powI(const DComplex x, const DLong y) {
  assert(0);
  throw GDLException("powI() not defined for DComplex");
}
template <>
inline DComplexDbl powI(const DComplexDbl x, const DLong y) {
  assert(0);
  throw GDLException("powI() not defined for DComplexDbl");
}
}

#endif
