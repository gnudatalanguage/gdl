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

// BYTE and the 16-bit types use the same conversion word as LONG and ULONG.
template <typename IntT>
using SignedWord = typename std::conditional<(sizeof(IntT) <= sizeof(std::int32_t)),
                                             std::int32_t, std::int64_t>::type;

// Wrap a truncated value within one unsigned word in either direction.
// NaN, infinity and magnitudes at or beyond the limit share one sentinel.
template <typename IntT, typename RealT>
inline IntT convertChecked(RealT r, bool& invalid) {
  using Word = SignedWord<IntT>;
  constexpr bool fullWidthUnsigned = std::is_unsigned<IntT>::value && sizeof(IntT) == sizeof(Word);
  constexpr Word minimum = std::numeric_limits<Word>::min();
  constexpr RealT half = -static_cast<RealT>(minimum);
  constexpr RealT limit = 2 * half;
  const bool inWindow = std::isless(std::abs(r), limit);
  if (sizeof(Word) == sizeof(std::int32_t)) {
    if (!fullWidthUnsigned) {
      // DOUBLE can truncate to INT32_MIN from fractions below it.
      constexpr double lowerCutoff = static_cast<double>(minimum) - 1;
      const bool aboveLowerCutoff = std::isgreater(static_cast<double>(r), lowerCutoff);
      invalid |= !(aboveLowerCutoff & std::isless(r, half));
    } else {
      invalid |= !inWindow;
    }
    return inWindow ? static_cast<IntT>(static_cast<std::int64_t>(r))
                    : static_cast<IntT>(minimum);
  }
  if (!inWindow) {
    invalid = true;
    return static_cast<IntT>(minimum);
  }

  // In the outer half of the 64-bit window FLOAT and DOUBLE are already
  // integral. Shifting by one word is exact and brings them into int64.
  if (r >= half) {
    invalid |= !fullWidthUnsigned;
    r -= limit;
  } else if (r < -half) {
    invalid = true;
    r += limit;
  }
  return static_cast<IntT>(static_cast<std::int64_t>(r));
}

}  // namespace real2int_detail

// FE_INVALID marks NaN, infinity and magnitudes outside the wrap window.
// It also marks truncated values outside int32 for BYTE, INT, UINT and LONG,
// or outside int64 for LONG64. ULONG64 also warns below -2^63.
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
