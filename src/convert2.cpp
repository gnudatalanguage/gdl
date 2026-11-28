/***************************************************************************
                          convert2.cpp  -  convert one datatype into another
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

// Revision history
// 2026-11-23  Dmitri A. Sergatskov <dasergatskov@gmail.com>
// Use C++14 RAII and lambdas, fix an OpenMP race in string formatting,
// fix UB in floating-point to integer conversions.

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <cfenv>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>

#include "dinterpreter.hpp"
#include "gdlarray.hpp"
#include "real2int.hpp"

namespace {

bool consumesSource(BaseGDL::Convert2Mode mode) {
  return (mode & BaseGDL::CONVERT) != 0;
}

template <typename Sp>
std::unique_ptr<Data_<Sp>> sourceOwner(Data_<Sp>* source,
                                       BaseGDL::Convert2Mode mode) {
  return std::unique_ptr<Data_<Sp>>{consumesSource(mode) ? source : nullptr};
}

template <typename Sp>
BaseGDL* copyOrReuse(Data_<Sp>* source, BaseGDL::Convert2Mode mode) {
  return (mode & BaseGDL::COPY) != 0 ? source->Dup() : source;
}

template <typename Operation>
void forEachElement(SizeT nEl, Operation operation) try {
  const int threadCount =
      nEl > 1 ? parallelize(nEl, TP_ARRAY_INITIALISATION) : 1;

  if (threadCount == 1) {
    for (SizeT i = 0; i < nEl; ++i) {
      operation(i);
    }
    return;
  }

  std::exception_ptr error;
  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(threadCount)
  for (OMPInt i = 0; i < static_cast<OMPInt>(nEl); ++i) {
    try {
      operation(static_cast<SizeT>(i));
    } catch (...) {
#pragma omp critical(gdl_convert2_exception)
      {
        if (!error) {
          error = std::current_exception();
        }
      }
    }
  }
  if (error) {
    std::rethrow_exception(error);
  }
} catch (const std::bad_alloc&) {
  ThrowGDLException("Array requires more memory than available");
}

template <typename DestSp, typename SourceSp, typename Converter>
Data_<DestSp>* convertArray(Data_<SourceSp>* source, BaseGDL::Convert2Mode mode,
                            Converter converter) {
  auto owner = sourceOwner(source, mode);
  auto dest = std::make_unique<Data_<DestSp>>(source->Dim(), BaseGDL::NOZERO);

  forEachElement(source->N_Elements(),
                 [source, dest = dest.get(), converter](SizeT i) {
                   (*dest)[i] = converter((*source)[i]);
                 });

  return dest.release();
}

template <typename T>
T realPart(const T& value) {
  return value;
}

template <typename T>
T realPart(const std::complex<T>& value) {
  return value.real();
}

template <typename DestSp, typename SourceSp>
typename std::enable_if<DestSp::IS_FLOAT && SourceSp::IS_COMPLEX,
                        typename DestSp::Ty>::type
convertScalar(const typename SourceSp::Ty& value) {
  return static_cast<typename DestSp::Ty>(value.real());
}

// GDL relies on its compilers retaining the low bits on integer narrowing.
template <typename DestSp, typename SourceSp>
typename std::enable_if<!(DestSp::IS_FLOAT && SourceSp::IS_COMPLEX),
                        typename DestSp::Ty>::type
convertScalar(const typename SourceSp::Ty& value) {
  return static_cast<typename DestSp::Ty>(value);
}

// A power-of-two cutoff has zero fraction bits, so a strict magnitude
// comparison needs only the upper word. NaNs and infinities sort above it.
template <typename RealT>
std::uint32_t magnitudeWord(RealT value) {
  using Bits = typename std::conditional<sizeof(RealT) == sizeof(std::uint32_t),
                                         std::uint32_t, std::uint64_t>::type;
  Bits bits;
  std::memcpy(&bits, &value, sizeof(bits));
  return static_cast<std::uint32_t>(bits >> (std::numeric_limits<Bits>::digits - 32))
         & 0x7fffffffU;
}

template <SizeT groupSize, typename SourceT, typename RealT>
bool groupFits(const SourceT* in, RealT limit) {
  // The encoding of 1 also checks that floating-point and integer words have
  // compatible byte order. Compilers fold the layout check and the limit.
  using Limits = std::numeric_limits<RealT>;
  if (Limits::is_iec559 &&
      ((sizeof(RealT) == sizeof(std::uint32_t) && Limits::digits == 24 &&
        magnitudeWord(RealT{1}) == 0x3f800000U) ||
       (sizeof(RealT) == sizeof(std::uint64_t) && Limits::digits == 53 &&
        magnitudeWord(RealT{1}) == 0x3ff00000U))) {
    std::uint32_t largest = 0;
    for (SizeT j = 0; j < groupSize; ++j) {
      largest = std::max(largest, magnitudeWord(realPart(in[j])));
    }
    return largest < magnitudeWord(limit);
  }

  for (SizeT j = 0; j < groupSize; ++j) {
    if (!std::isless(std::abs(realPart(in[j])), limit)) return false;
  }
  return true;
}

template <typename DestT, typename SourceT>
bool convertIntegerBlock(const SourceT* in, DestT* out, SizeT nEl) {
  using RealT = decltype(realPart(*in));
  using Intermediate = real2int_detail::SignedWord<DestT>;
  constexpr RealT limit = -static_cast<RealT>(std::numeric_limits<Intermediate>::min());
  // Longer groups help with interleaved COMPLEX real lanes and amortize
  // the range reduction when both the input and the result are 64 bits.
  // For BYTE results, a larger group amortizes the range check over more
  // values. An outlier sends the whole group through the checked path.
  constexpr bool wideConversion = sizeof(RealT) == sizeof(std::uint64_t) &&
                                  sizeof(DestT) == sizeof(std::uint64_t);
  constexpr SizeT groupSize = (std::is_same<SourceT, DComplex>::value ||
                               sizeof(DestT) == 1) ? 64 :
                              (wideConversion ? 32 : 16);
  const SizeT grouped = nEl - nEl % groupSize;
  bool invalid = false;
  SizeT i = 0;
  for (; i < grouped; i += groupSize) {
    if (groupFits<groupSize>(in + i, limit)) {
      for (SizeT j = 0; j < groupSize; ++j) {
        out[i + j] = static_cast<DestT>(static_cast<Intermediate>(realPart(in[i + j])));
      }
      continue;
    }

    // The rest of the ULONG wrap window fits in int64, including negative values.
    if (std::is_same<DestT, DULong>::value && groupFits<groupSize>(in + i, 2 * limit)) {
      for (SizeT j = 0; j < groupSize; ++j) {
        out[i + j] = static_cast<DestT>(static_cast<std::int64_t>(realPart(in[i + j])));
      }
      continue;
    }

    for (SizeT j = 0; j < groupSize; ++j) {
      out[i + j] = real2int_detail::convertChecked<DestT>(realPart(in[i + j]), invalid);
    }
  }
  for (; i < nEl; ++i) {
    out[i] = real2int_detail::convertChecked<DestT>(realPart(in[i]), invalid);
  }
  return invalid;
}

template <typename DestSp, typename SourceSp>
typename std::enable_if<!DestSp::IS_INTEGER || SourceSp::IS_INTEGER,
                        Data_<DestSp>*>::type
convertNumeric(Data_<SourceSp>* source, BaseGDL::Convert2Mode mode) {
  return convertArray<DestSp>(source, mode,
                              [](const typename SourceSp::Ty& value) {
                                return convertScalar<DestSp, SourceSp>(value);
                              });
}

template <typename DestSp, typename SourceSp>
typename std::enable_if<DestSp::IS_INTEGER && !SourceSp::IS_INTEGER,
                        Data_<DestSp>*>::type
convertNumeric(Data_<SourceSp>* source, BaseGDL::Convert2Mode mode) {
  auto owner = sourceOwner(source, mode);
  auto dest = std::make_unique<Data_<DestSp>>(source->Dim(), BaseGDL::NOZERO);
  static constexpr SizeT blockSize = 1024;
  const SizeT nEl = source->N_Elements();
  const SizeT nBlocks = nEl / blockSize + (nEl % blockSize != 0);
  const auto convertBlock = [source, dest = dest.get(), nEl](SizeT b) {
    const SizeT begin = b * blockSize;
    return convertIntegerBlock(&(*source)[begin], &(*dest)[begin],
                               std::min(blockSize, nEl - begin));
  };

  const int threadCount =
      nBlocks > 1 ? static_cast<int>(std::min<SizeT>(
                        nBlocks, parallelize(nEl, TP_ARRAY_INITIALISATION)))
                  : 1;
  bool invalid = false;
  if (threadCount == 1) {
    for (SizeT b = 0; b < nBlocks; ++b) {
      invalid |= convertBlock(b);
    }
  } else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(threadCount) reduction(|| : invalid)
    for (OMPInt b = 0; b < static_cast<OMPInt>(nBlocks); ++b) {
      invalid |= convertBlock(static_cast<SizeT>(b));
    }
  }
  // CHECK_MATH reads the floating-point environment of the calling thread.
  if (invalid) {
    std::feraiseexcept(FE_INVALID);
  }
  return dest.release();
}

[[noreturn]] void cannotConvert() {
  if (BaseGDL::interpreter != nullptr &&
      !BaseGDL::interpreter->CallStack().empty()) {
    BaseGDL::interpreter->CallStack().back()->Throw(
        "Cannot convert to this type.");
  }
  throw GDLException("Cannot convert to this type.");
}

template <typename SourceSp>
BaseGDL* convertNumericArray(Data_<SourceSp>* source, DType destTy,
                             BaseGDL::Convert2Mode mode) {
  if (destTy == SourceSp::t) {
    return copyOrReuse(source, mode);
  }

  switch (destTy) {
    case GDL_BYTE:
      return convertNumeric<SpDByte>(source, mode);
    case GDL_INT:
      return convertNumeric<SpDInt>(source, mode);
    case GDL_UINT:
      return convertNumeric<SpDUInt>(source, mode);
    case GDL_LONG:
      return convertNumeric<SpDLong>(source, mode);
    case GDL_ULONG:
      return convertNumeric<SpDULong>(source, mode);
    case GDL_LONG64:
      return convertNumeric<SpDLong64>(source, mode);
    case GDL_ULONG64:
      return convertNumeric<SpDULong64>(source, mode);
    case GDL_FLOAT:
      return convertNumeric<SpDFloat>(source, mode);
    case GDL_DOUBLE:
      return convertNumeric<SpDDouble>(source, mode);
    case GDL_COMPLEX:
      return convertNumeric<SpDComplex>(source, mode);
    case GDL_COMPLEXDBL:
      return convertNumeric<SpDComplexDbl>(source, mode);
    default:
      cannotConvert();
  }
}

[[noreturn]] void expressionNotAllowed(BaseGDL* value, const char* typeName) {
  const std::string message =
      std::string{typeName} + " expression not allowed in this context";
  if (BaseGDL::interpreter != nullptr &&
      !BaseGDL::interpreter->CallStack().empty()) {
    BaseGDL::interpreter->CallStack().back()->Throw(
        message + ": " + BaseGDL::interpreter->Name(value));
  }
  throw GDLException(message + ".");
}

std::string formattedFloat(DFloat value) {
  if (std::isfinite(value)) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%#13.6g", value);
    return buf;
  }
  if (std::isnan(value)) {
    return std::signbit(value) ? "         -NaN" : "          NaN";
  }
  return std::signbit(value) ? "         -Inf" : "          Inf";
}

std::string formattedDouble(DDouble value) {
  if (std::isfinite(value)) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%#16.8g", value);
    return buf;
  }
  if (std::isnan(value)) {
    return std::signbit(value) ? "            -NaN" : "             NaN";
  }
  return std::signbit(value) ? "       -Infinity" : "        Infinity";
}

std::string formattedComplex(const DComplex& value) {
  if (std::isfinite(value.real()) && std::isfinite(value.imag())) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "(%#13.6g,%#13.6g)", value.real(),
                  value.imag());
    return buf;
  }
  return "(" + formattedFloat(value.real()) + "," +
         formattedFloat(value.imag()) + ")";
}

std::string formattedDoubleComplex(const DComplexDbl& value) {
  if (std::isfinite(value.real()) && std::isfinite(value.imag())) {
    char buf[40];
    std::snprintf(buf, sizeof(buf), "(%#16.8g,%#16.8g)", value.real(),
                  value.imag());
    return buf;
  }
  return "(" + formattedDouble(value.real()) + "," +
         formattedDouble(value.imag()) + ")";
}

template <typename SourceSp, typename Formatter>
Data_<SpDString>* convertToStrings(Data_<SourceSp>* source,
                                   BaseGDL::Convert2Mode mode,
                                   Formatter formatter) {
  return convertArray<SpDString>(source, mode, formatter);
}

template <typename RealT>
RealT parseRealImpl(const char* start, char** end);

template <>
float parseRealImpl<float>(const char* start, char** end) {
#ifdef _MSC_VER
  return StrToD(start, end);
#else
  return std::strtof(start, end);
#endif
}

template <>
double parseRealImpl<double>(const char* start, char** end) {
  return StrToD(start, end);
}

// IDL accepts Fortran-style D exponents for both FLOAT and DOUBLE input.
template <typename RealT>
RealT parseReal(const char* start, char** end) {
  const char* exponent = std::strpbrk(start, "Dd");
  if (exponent == nullptr) {
    return parseRealImpl<RealT>(start, end);
  }

  std::string normalized(start);
  normalized[exponent - start] = 'E';
  char* normalizedEnd = nullptr;
  const RealT result = parseRealImpl<RealT>(normalized.c_str(), &normalizedEnd);
  *end = const_cast<char*>(start) + (normalizedEnd - normalized.c_str());
  return result;
}

template <typename DestSp, typename Parser>
Data_<DestSp>* parseStrings(Data_<SpDString>* source,
                            BaseGDL::Convert2Mode mode,
                            const char* destinationName, Parser parser) {
  auto owner = sourceOwner(source, mode);
  auto dest = std::make_unique<Data_<DestSp>>(source->Dim(), BaseGDL::NOZERO);
  const SizeT nEl = source->N_Elements();
  // GDLArray stores short arrays inline. The loop writes every flag.
  GDLArray<DByte, true> flags(nEl, false);
  DByte* const invalid = flags.GetBuffer();

  forEachElement(nEl, [source, dest = dest.get(), invalid, parser](SizeT i) {
    const std::string& text = (*source)[i];
    const char* const start = text.c_str();
    char* end = nullptr;
    (*dest)[i] = parser(start, &end);
    invalid[i] = end == start && !text.empty();
  });

  if (std::find(invalid, invalid + nEl, 1) == invalid + nEl) {
    return dest.release();
  }

  const std::string summary =
      "Type conversion error: Unable to convert "
      "given STRING to " +
      std::string{destinationName} + ".";
  if ((mode & BaseGDL::THROWIOERROR) != 0) {
    throw GDLIOException(summary);
  }

  for (SizeT i = 0; i < nEl; ++i) {
    if (invalid[i] != 0) {
      Warning("Type conversion error: Unable to convert given STRING: '" +
              (*source)[i] + "' to " + destinationName + ".");
    }
  }
  return dest.release();
}

Data_<SpDByte>* stringsToBytes(Data_<SpDString>* source,
                               BaseGDL::Convert2Mode mode) {
  auto owner = sourceOwner(source, mode);
  const SizeT nEl = source->N_Elements();
  SizeT maxLen = 1;  // An empty string converts to one zero byte.
  for (SizeT i = 0; i < nEl; ++i) {
    maxLen = std::max(maxLen, static_cast<SizeT>((*source)[i].length()));
  }

  dimension bytDim(source->Dim());
  bytDim >> maxLen;
  auto dest = std::make_unique<Data_<SpDByte>>(bytDim);

  forEachElement(nEl, [source, dest = dest.get(), maxLen](SizeT i) {
    const std::string& text = (*source)[i];
    std::memcpy(&(*dest)[i * maxLen], text.data(), text.size());
  });
  return dest.release();
}

Data_<SpDString>* bytesToStrings(Data_<SpDByte>* source,
                                 BaseGDL::Convert2Mode mode) {
  if (mode == BaseGDL::COPY_BYTE_AS_INT) {
    return convertToStrings(source, mode, [](DByte value) {
      return i2s(static_cast<int>(value), 4);
    });
  }

  auto owner = sourceOwner(source, mode);
  dimension strDim(source->Dim());
  SizeT strLen = strDim.Remove(0);
  if (strLen == 0) {
    strLen = 1;
  }
  const SizeT nEl = source->N_Elements() / strLen;
  auto dest = std::make_unique<Data_<SpDString>>(strDim, BaseGDL::NOZERO);

  forEachElement(nEl, [source, dest = dest.get(), strLen](SizeT i) {
    // Each string ends at its first NUL byte, as before.
    const char* first = reinterpret_cast<const char*>(&(*source)[i * strLen]);
    const char* nul = std::char_traits<char>::find(first, strLen, '\0');
    (*dest)[i].assign(first, nul ? static_cast<SizeT>(nul - first) : strLen);
  });
  return dest.release();
}

}  // namespace

BaseGDL* DStructGDL::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == t) {
    return (mode & COPY) != 0 ? Dup() : this;
  }
  expressionNotAllowed(this, "Struct");
}

template <>
BaseGDL* Data_<SpDPtr>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == t) {
    return copyOrReuse(this, mode);
  }
  expressionNotAllowed(this, "Ptr");
}

template <>
BaseGDL* Data_<SpDObj>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == t) {
    return copyOrReuse(this, mode);
  }
  expressionNotAllowed(this, "Object");
}

template <>
BaseGDL* Data_<SpDByte>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return bytesToStrings(this, mode);
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDInt>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, [](DInt value) {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%8i", static_cast<int>(value));
      return std::string{buf};
    });
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDUInt>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, [](DUInt value) {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%8u", static_cast<unsigned int>(value));
      return std::string{buf};
    });
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDLong>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, [](DLong value) {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%12i", value);
      return std::string{buf};
    });
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDULong>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, [](DULong value) {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%12u", value);
      return std::string{buf};
    });
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDLong64>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, [](DLong64 value) {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%22lli", value);
      return std::string{buf};
    });
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDULong64>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, [](DULong64 value) {
      char buf[32];
      std::snprintf(buf, sizeof(buf), "%22llu", value);
      return std::string{buf};
    });
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDFloat>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, formattedFloat);
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDDouble>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, formattedDouble);
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDComplex>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, formattedComplex);
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDComplexDbl>::Convert2(DType destTy,
                                        BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return convertToStrings(this, mode, formattedDoubleComplex);
  }
  return convertNumericArray(this, destTy, mode);
}

template <>
BaseGDL* Data_<SpDString>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  if (destTy == GDL_STRING) {
    return copyOrReuse(this, mode);
  }

  switch (destTy) {
    case GDL_BYTE:
      return stringsToBytes(this, mode);
    case GDL_INT:
      return parseStrings<SpDInt>(this, mode, "INT",
                                  [](const char* start, char** end) {
                                    return std::strtol(start, end, 10);
                                  });
    case GDL_UINT:
      return parseStrings<SpDUInt>(this, mode, "UINT",
                                   [](const char* start, char** end) {
                                     return std::strtoul(start, end, 10);
                                   });
    case GDL_LONG:
      return parseStrings<SpDLong>(this, mode, "LONG",
                                   [](const char* start, char** end) {
                                     return std::strtol(start, end, 10);
                                   });
    case GDL_ULONG:
      return parseStrings<SpDULong>(this, mode, "ULONG",
                                    [](const char* start, char** end) {
                                      return std::strtoul(start, end, 10);
                                    });
    case GDL_LONG64:
      return parseStrings<SpDLong64>(this, mode, "LONG64",
                                     [](const char* start, char** end) {
                                       return std::strtoll(start, end, 10);
                                     });
    case GDL_ULONG64:
      return parseStrings<SpDULong64>(this, mode, "ULONG64",
                                      [](const char* start, char** end) {
                                        return std::strtoull(start, end, 10);
                                      });
    case GDL_FLOAT:
      return parseStrings<SpDFloat>(this, mode, "FLOAT",
                                    [](const char* start, char** end) {
                                      return parseReal<float>(start, end);
                                    });
    case GDL_DOUBLE:
      return parseStrings<SpDDouble>(this, mode, "DOUBLE",
                                     [](const char* start, char** end) {
                                       return parseReal<double>(start, end);
                                     });
    case GDL_COMPLEX:
      return parseStrings<SpDComplex>(this, mode, "COMPLEX",
                                      [](const char* start, char** end) {
                                        return parseReal<float>(start, end);
                                      });
    case GDL_COMPLEXDBL:
      return parseStrings<SpDComplexDbl>(this, mode, "DCOMPLEX",
                                         [](const char* start, char** end) {
                                           return parseReal<double>(start, end);
                                         });
    default:
      cannotConvert();
  }
}
