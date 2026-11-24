/***************************************************************************
                       real2int.hpp  -  convert float/double to integer type
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

#ifndef REAL2INT_HPP_
#define REAL2INT_HPP_

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace real2int_detail {

// Every destination value of BYTE, INT, UINT and LONG fits in int32.
template <typename IntT>
constexpr bool fitsInt32() {
  return std::numeric_limits<IntT>::digits <=
         std::numeric_limits<std::int32_t>::digits;
}

// At this magnitude finite operands both signal invalid and have zero low
// destination bits. Binary floating-point spacing is at least 2^bits.
template <typename IntT, typename RealT>
constexpr RealT zeroWithInvalidThreshold() {
  using UIntT = typename std::make_unsigned<IntT>::type;
  constexpr int zeroExponent =
      std::numeric_limits<UIntT>::digits + std::numeric_limits<RealT>::digits - 1;
  constexpr int invalidExponent = fitsInt32<IntT>() ? 32 : 64;
  constexpr int exponent = zeroExponent > invalidExponent ? zeroExponent : invalidExponent;
  RealT threshold = 1;
  for (int i = 0; i < exponent; ++i) threshold *= 2;
  return threshold;
}

// The lower limit applies after truncation. FLOAT has no representable
// values between -2147483649 and -2147483648, but DOUBLE does.
// Evaluate both comparisons so the array loop can vectorize.
inline bool truncatesIntoInt32(float r) {
  const int above = std::isgreaterequal(r, -2147483648.0f);
  return above & std::isless(r, 2147483648.0f);
}

inline bool truncatesIntoInt32(double r) {
  const int above = std::isgreater(r, -2147483649.0);
  return above & std::isless(r, 2147483648.0);
}

// Truncate finite values and retain the low destination bits. Accumulate
// invalid operands so a parallel conversion can report them on its caller.
template <typename IntT, typename RealT>
inline IntT convertWide(const RealT r, bool& invalid) {
  // Both bounds are exact powers of two.
  constexpr RealT two63 =
      -static_cast<RealT>(std::numeric_limits<std::int64_t>::min());
  if (std::isgreaterequal(r, -two63) && std::isless(r, two63)) {
    return static_cast<IntT>(static_cast<std::int64_t>(r));
  }
  // [2^63, 2^64): r - 2^63 is exact and fits int64, so nothing signals.
  if (std::isgreaterequal(r, two63) && std::isless(r, 2 * two63)) {
    const auto low =
        static_cast<std::uint64_t>(static_cast<std::int64_t>(r - two63));
    return static_cast<IntT>(low + (std::uint64_t{1} << 63));
  }

  invalid = true;
  using UIntT = typename std::make_unsigned<IntT>::type;
  constexpr int bits = std::numeric_limits<UIntT>::digits;
  constexpr UIntT signBit = std::numeric_limits<UIntT>::max() / 2 + 1;
  if (!std::isfinite(r)) {
    if (std::is_signed<IntT>::value) {
      return static_cast<IntT>(bits < 32 ? 0
                                         : std::numeric_limits<IntT>::min());
    }
    return static_cast<IntT>(
        bits == 64 && (std::isnan(r) || std::signbit(r)) ? signBit : 0);
  }
  // At this magnitude every FLOAT and DOUBLE is already a whole number.
  const RealT magnitude = std::abs(r);
  constexpr RealT zeroThreshold = zeroWithInvalidThreshold<IntT, RealT>();
  if (magnitude >= zeroThreshold) {
    return IntT{0};
  }
  UIntT lowBits = static_cast<UIntT>(
      std::fmod(magnitude, std::ldexp(static_cast<RealT>(1), bits)));
  if (r < 0) {
    lowBits = static_cast<UIntT>(0) - lowBits;
  }
  return static_cast<IntT>(lowBits);
}

// Checked conversion shared by scalar callers and exceptional array elements.
template <typename IntT, typename RealT>
inline IntT convertChecked(RealT r, bool& invalid) {
  if (fitsInt32<IntT>()) {
    if (truncatesIntoInt32(r)) {
      return static_cast<IntT>(static_cast<std::int32_t>(r));
    }
    invalid = true;
  }
  return convertWide<IntT>(r, invalid);
}

}  // namespace real2int_detail

// CHECK_MATH reports NaN, infinity and values whose truncated integer falls
// outside int32 (BYTE, INT, UINT, LONG) or [-2^63, 2^64) (other integers).
template <typename IntT, typename RealT>
inline IntT Real2Int(const RealT r) {
  static_assert(std::is_integral<IntT>::value,
                "Real2Int requires an integer destination");
  bool invalid = false;
  const IntT result = real2int_detail::convertChecked<IntT>(r, invalid);
  if (invalid) {
    std::feraiseexcept(FE_INVALID);
  }
  return result;
}

template <typename RealT>
inline DByte Real2DByte(const RealT r) {
  return Real2Int<DByte>(r);
}

#endif
