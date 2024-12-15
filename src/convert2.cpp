/***************************************************************************
                          convert2.cpp  -  convert one datatype into another
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

// TODO: adjust the with for string conversion for each type (i2s((*this)[i],X))

// Convert2( DType) functions
#include <string.h>

#include "dinterpreter.hpp"
#include "real2int.hpp"
#include "ofmt.hpp" // OutAuto

using namespace std;

//#define TRACE_CONVERT2 cout << "Convert2 " << this->TypeStr() << " -> " << destTy << "\tn " << dd.size() << "\tmode " << mode << endl;
#define TRACE_CONVERT2

#define DO_CONVERT_START(tnew)  {\
        Data_<tnew>* dest=new Data_<tnew>( dim, BaseGDL::NOZERO);\
        if( nEl == 1) { (*dest)[0]=(*this)[0]; if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest;}\
        if((GDL_NTHREADS=parallelize(nEl,  TP_ARRAY_INITIALISATION))==1) {for( SizeT i=0; i < nEl; ++i) (*dest)[i]=(*this)[i]; if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }

#define DO_CONVERT_START_FLOAT_TO_UNSIGNED_IA64(tnew, unsigned, signed) DO_CONVERT_START(tnew)

#define DO_CONVERT_START_FLOAT_TO_UNSIGNED_ARM64(tnew, unsigned, signed)  {\
        Data_<tnew>* dest=new Data_<tnew>( dim, BaseGDL::NOZERO);\
        if( nEl == 1) { (*dest)[0]=static_cast<unsigned>(static_cast<signed>((*this)[0])); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest;}\
        if((GDL_NTHREADS=parallelize(nEl,  TP_ARRAY_INITIALISATION))==1) {for( SizeT i=0; i < nEl; ++i) (*dest)[i]=static_cast<unsigned>(static_cast<signed>((*this)[i])); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }

#ifdef __aarch64__
#define DO_CONVERT_START_FLOAT_TO_UNSIGNED(tnew, unsigned, signed) DO_CONVERT_START_FLOAT_TO_UNSIGNED_ARM64(tnew, unsigned, signed)
#else
#define DO_CONVERT_START_FLOAT_TO_UNSIGNED(tnew, unsigned, signed) DO_CONVERT_START_FLOAT_TO_UNSIGNED_IA64(tnew, unsigned, signed)
#endif

#define DO_CONVERT_START_BIG_TO_SMALL(tnew, mask)  {\
        Data_<tnew>* dest=new Data_<tnew>( dim, BaseGDL::NOZERO);\
        if( nEl == 1) { (*dest)[0]=((*this)[0]&mask); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest;}\
        if((GDL_NTHREADS=parallelize(nEl,  TP_ARRAY_INITIALISATION))==1) {for( SizeT i=0; i < nEl; ++i) (*dest)[i]=((*this)[i]&mask); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }

#define DO_CONVERT_START_CPX(tnew)  {\
        Data_<tnew>* dest=new Data_<tnew>( dim, BaseGDL::NOZERO);\
        if( nEl == 1) { (*dest)[0]=(*this)[0].real(); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest;}\
        if((GDL_NTHREADS=parallelize(nEl,  TP_ARRAY_INITIALISATION))==1) {for( SizeT i=0; i < nEl; ++i) (*dest)[i]=(*this)[i].real(); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }

#define DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX_IA64(tnew, unsigned, signed) DO_CONVERT_START_CPX(tnew)

#define DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX_ARM64(tnew, unsigned, signed)  {\
        Data_<tnew>* dest=new Data_<tnew>( dim, BaseGDL::NOZERO);\
        if( nEl == 1) { (*dest)[0]=static_cast<unsigned>(static_cast<signed>((*this)[0].real())); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest;}\
        if((GDL_NTHREADS=parallelize(nEl,  TP_ARRAY_INITIALISATION))==1) {for( SizeT i=0; i < nEl; ++i) (*dest)[i]=static_cast<unsigned>(static_cast<signed>((*this)[i].real())); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }
#ifdef __aarch64__
#define DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(tnew, unsigned, signed) DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX_ARM64(tnew, unsigned, signed)
#else
#define DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(tnew, unsigned, signed) DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX_IA64(tnew, unsigned, signed)
#endif

#define DO_CONVERT_END  for( SizeT i=0; i < nEl; ++i) (*dest)[i]=(*this)[i]; if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }
#define DO_CONVERT_END_BIG_TO_SMALL(mask)  for( SizeT i=0; i < nEl; ++i) (*dest)[i]=((*this)[i]&mask); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }
#define DO_CONVERT_END_CPX  for( SizeT i=0; i < nEl; ++i) (*dest)[i]=(*this)[i].real(); if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest; }

// for double -> string

/*inline string double2string(const DDouble d) {
  std::ostringstream os;
  OutAuto(os, d, 16, 8, 0);
  return os.str();
}*/
inline string double2string(const DDouble d) {
    std::string st;
    if (std::isfinite(d)){
      char buf [17];
      snprintf (buf,17, "%#16.8g", d);
      st = std::string(buf);
    } else if(std::isnan((double)d)){
          st=(std::signbit(d))? "            -NaN" : "             NaN";
    
    } else if(std::isinf(d)){
          st=(std::signbit(d))? "       -Infinity" : "        Infinity";
    };
    return st;
}//Here we quit the use of ostringstream who would take 3 times long time; by JW Feb 2022; 


// for float -> string

inline string float2string(const DFloat f){
    std::string st;
    if (isfinite(f)){
      char buf [14];
      snprintf (buf,14, "%#13.6g", f);
      st = std::string(buf);
    } else if(isnan(f)){
          st=(std::signbit(f))? "         -NaN" : "          NaN";
    } else if(isinf(f)){
          st=(std::signbit(f))? "         -Inf" : "          Inf";
    };
    return st;
} //by JW Feb 2022; 


// for float -> string

inline string complex2string(const DComplex f){
    std::string st;
    if (std::isfinite(std::abs(f))){
      char buf [30];
      snprintf (buf,30, "(%#13.6g,%#13.6g)", f.real(),f.imag());
      st = std::string(buf);
    } else {
      st="(" + float2string(f.real()) + "," + float2string(f.imag()) + ")";
    }
    return st;
} //by JW Feb 2022; 

inline string dcomplex2string(const DComplexDbl f){
    std::string st;
    if (std::isfinite(std::abs(f))){
      char buf [36];
      snprintf (buf,36, "(%#16.8g,%#16.8g)", f.real(),f.imag());
      st = std::string(buf);
    } else {
      st="(" + double2string(f.real()) + "," + double2string(f.imag()) + ")";
    }
    return st;
} //by JW Feb 2022; 

// for string -> float/double
template <typename real_t>
inline real_t string2real_helper(const char* cStart, char** cEnd);

template <>
inline float string2real_helper<float>(const char* cStart, char** cEnd) {
#ifdef _MSC_VER
  return StrToD(cStart, cEnd);
#else
  return strtof(cStart, cEnd);
#endif
}

template <>
inline double string2real_helper<double>(const char* cStart, char** cEnd) {
  return StrToD(cStart, cEnd);
}

template <typename real_t>
inline real_t string2real(const char* cStart, char** cEnd) {
  const char* d = strpbrk(cStart, "Dd");
  if (d == NULL)
    return string2real_helper<real_t>(cStart, cEnd);
  else {
    string tmps(cStart);
    tmps[d - cStart] = 'E';
    char* tmpcp;
    real_t tmpf = string2real_helper<real_t>(tmps.c_str(), &tmpcp);
    *cEnd = const_cast<char*> (cStart) + (tmpcp - tmps.c_str());
    return tmpf;
  }
}

// every type need this function which defines its conversion to all other types
// so for every new type each of this functions has to be extended
// and a new function has to be 'specialized'
// not very nice, but how else to do?
// each function creates the new type on the heap

// non convertables

BaseGDL* DStructGDL::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);
  //DInterpreter* testDbg = BaseGDL::interpreter;
  //int szDbg = BaseGDL::interpreter->CallStack().size();
  if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Struct expression not allowed in this context: " + BaseGDL::interpreter->Name(this));
  throw GDLException("Struct expression not allowed in this context.");
  return NULL; // get rid of warning 
}

template<> BaseGDL* Data_<SpDPtr>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);
  if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Ptr expression not allowed in this context: " + BaseGDL::interpreter->Name(this));
  throw GDLException("Ptr expression not allowed in this context.");
  return NULL; // get rid of warning 
}

template<> BaseGDL* Data_<SpDObj>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);
  if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Object expression not allowed in this context: " + BaseGDL::interpreter->Name(this));
  throw GDLException("Object expression not allowed in this context.");
  return NULL; // get rid of warning 
}

// the real Convert2 functions

template<> BaseGDL* Data_<SpDByte>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();
  switch (destTy) {
  case GDL_BYTE: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_INT:
    DO_CONVERT_START(SpDInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_UINT:
    DO_CONVERT_START(SpDUInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG:
    DO_CONVERT_START(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG:
    DO_CONVERT_START(SpDULong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
    DO_CONVERT_START(SpDULong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {   if (mode == BaseGDL::COPY_BYTE_AS_INT) {
      Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 4);
	  if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
		for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = i2s(static_cast<int> ((*this)[i]), 4);
	  } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = i2s(static_cast<int> ((*this)[i]), 4);
	  }
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      return dest;
    } else  { //NOTE: This should be only for BYTES according to the doc, but IDL in fact does it for all integer types below l64 apparently
      dimension strDim(dim);
      SizeT strLen = strDim.Remove(0);

      if (strLen == 0) strLen = 1;

      nEl /= strLen;

      char *buf = new char[ strLen + 1];
      ArrayGuard<char> guard(buf);

      buf[ strLen] = 0;
      Data_<SpDString>* dest =
        new Data_<SpDString>(strDim, BaseGDL::NOZERO);
      for (SizeT i = 0; i < nEl; ++i) {
        SizeT basePtr = i*strLen;
        for (SizeT b = 0; b < strLen; b++)
          buf[b] = (*this)[ basePtr + b];

        (*dest)[i] = buf;
	  }
	  if ((mode & BaseGDL::CONVERT) != 0) delete this;
	  return dest;
	}
  }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  return NULL; // get rid of warning
} // GDL_BYTE 

template<> BaseGDL* Data_<SpDInt>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_BIG_TO_SMALL(SpDByte, 0xFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFF)
  case GDL_INT: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_UINT:
    DO_CONVERT_START(SpDUInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG:
    DO_CONVERT_START(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG:
    DO_CONVERT_START(SpDULong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
    DO_CONVERT_START(SpDULong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING:
  {
    char buf[32];
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 8);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%8i", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 8);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%8i", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 8);
      }
	}
	if ((mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
  }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  return NULL; // get rid of warning
} // GDL_INT  

template<> BaseGDL* Data_<SpDUInt>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_BIG_TO_SMALL(SpDByte, 0xFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFF)
  case GDL_INT:
    DO_CONVERT_START(SpDInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_UINT: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_LONG:
    DO_CONVERT_START(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG:
    DO_CONVERT_START(SpDULong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
    DO_CONVERT_START(SpDULong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    char buf[32];
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 8);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%8u", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 8);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%8u", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 8);
      }
	}
	if ((mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
  }

  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  return NULL; // get rid of warning
} // GDL_UINT

template<> BaseGDL* Data_<SpDLong>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_BIG_TO_SMALL(SpDByte, 0xFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL( 0xFF)
  case GDL_INT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_UINT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDUInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL( 0xFFFF)
  case GDL_LONG: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_ULONG:
    DO_CONVERT_START(SpDULong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
    DO_CONVERT_START(SpDULong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    char buf[32];
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 12);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%12i", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 12);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%12i", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 12);
      }
	}
	if ((mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
  }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
} // GDL_LONG  

template<> BaseGDL* Data_<SpDULong>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_BIG_TO_SMALL(SpDByte, 0xFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFF)
  case GDL_INT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_UINT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDUInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_LONG:
    DO_CONVERT_START(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
    DO_CONVERT_START(SpDULong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING:
  {
    char buf[32];
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 12);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%12u", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 12);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%12u", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 12);
      }
	}
	if ((mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
 }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
} // GDL_ULONG   

template<> BaseGDL* Data_<SpDFloat>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START(SpDByte)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_INT:
    DO_CONVERT_START(SpDInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_UINT:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED(SpDUInt, DUInt, DInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG:
    DO_CONVERT_START(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG:
 DO_CONVERT_START_FLOAT_TO_UNSIGNED(SpDULong, DULong, DLong) 
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
 DO_CONVERT_START_FLOAT_TO_UNSIGNED(SpDULong64, DULong64, DLong64) 
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);

  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING:
  {
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = float2string((*this)[i]);
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = float2string((*this)[i]);
	}
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }

  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
}

template<> BaseGDL* Data_<SpDDouble>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START(SpDByte)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_INT:
    DO_CONVERT_START(SpDInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_UINT:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED(SpDUInt, DUInt, DInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG:
    DO_CONVERT_START(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG:
 DO_CONVERT_START_FLOAT_TO_UNSIGNED(SpDULong, DULong, DLong) 
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64:
 DO_CONVERT_START_FLOAT_TO_UNSIGNED(SpDULong64, DULong64, DLong64) 
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = double2string((*this)[i]);
	} else {
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = double2string((*this)[i]);
	}
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }

  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
}

inline void StringConversionError(bool& errorFlag, BaseGDL::Convert2Mode mode, const string& msg) {
  if ((mode & BaseGDL::THROWIOERROR) != 0) {
    errorFlag = true;
    /*    delete dest;
        throw GDLIOException( msg);*/
  } else
    Warning(msg);
}

// The OpenMP below is always faster than IDL, as what is in the loop is quite complicated.

template<> BaseGDL* Data_<SpDString>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  bool errorFlag = false;

  switch (destTy) {
  case GDL_BYTE: // GDL_STRING to GDL_BYTE: add first dim
  {
    SizeT maxLen = 1; // empty string is converted to 0b
    for (SizeT i = 0; i < nEl; ++i)
      if ((*this)[i].length() > maxLen) maxLen = (*this)[i].length();

    dimension bytDim(dim);
    bytDim >> maxLen;

    Data_<SpDByte>* dest = new Data_<SpDByte>(bytDim); // zero fields

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      SizeT basePtr = i*maxLen;

      SizeT strLen = (*this)[ i].length();
      for (SizeT b = 0; b < strLen; b++)
        (*dest)[basePtr + b] = (*this)[ i][ b];
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_INT:
  {
    Data_<SpDInt>* dest = new Data_<SpDInt>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode)  num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = strtol(cStart, &cEnd, 10);
      if (cEnd == cStart && (*this)[i] != "") {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to INT.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to INT.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_UINT:
  {
    Data_<SpDUInt>* dest = new Data_<SpDUInt>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = strtoul(cStart, &cEnd, 10);
      if (cEnd == cStart && (*this)[i] != "") {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to UINT.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to UINT.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_LONG:
  {
    Data_<SpDLong>* dest = new Data_<SpDLong>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = strtol(cStart, &cEnd, 10);
      if (cEnd == cStart && (*this)[i] != "") {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to LONG.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to LONG.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_ULONG:
  {
    Data_<SpDULong>* dest = new Data_<SpDULong>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = strtoul(cStart, &cEnd, 10);
      if (cEnd == cStart && (*this)[i] != "") {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to ULONG.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to ULONG.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_LONG64:
  {
    Data_<SpDLong64>* dest = new Data_<SpDLong64>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = strtoll(cStart, &cEnd, 10);
      if (cEnd == cStart && (*this)[i] != "") {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to LONG64.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to LONG64.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_ULONG64:
  {
    Data_<SpDULong64>* dest = new Data_<SpDULong64>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = strtoull(cStart, &cEnd, 10);
      if (cEnd == cStart && (*this)[i] != "") {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to ULONG64.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to ULONG64.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_FLOAT:
  {
    Data_<SpDFloat>* dest = new Data_<SpDFloat>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = string2real<float>(cStart, &cEnd);
      if ((cEnd == cStart && (*this)[i] != "")) //  || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to FLOAT.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to FLOAT.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_DOUBLE:
  {
    Data_<SpDDouble>* dest = new Data_<SpDDouble>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = string2real<double>(cStart, &cEnd);
      if ((cEnd == cStart && (*this)[i] != "")) // || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to DOUBLE.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to DOUBLE.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
    //    case GDL_STRING: 
  case GDL_COMPLEX:
  {
    Data_<SpDComplex>* dest = new Data_<SpDComplex>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = string2real<float>(cStart, &cEnd);
      if ((cEnd == cStart && (*this)[i] != "")) // || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to COMPLEX.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to COMPLEX.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_COMPLEXDBL:
  {
    Data_<SpDComplexDbl>* dest =
      new Data_<SpDComplexDbl>(dim, BaseGDL::NOZERO);

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for shared( errorFlag, mode) num_threads(GDL_NTHREADS) if (GDL_NTHREADS > 1)
      for (OMPInt i = 0; i < nEl; ++i) {
      const char* cStart = (*this)[i].c_str();
      char* cEnd;
      (*dest)[i] = string2real<double>(cStart, &cEnd);
      if ((cEnd == cStart && (*this)[i] != "")) // || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      {
        StringConversionError(errorFlag, mode, "Type conversion error: "
          "Unable to convert given STRING: '" +
          (*this)[i] + "' to DCOMPLEX.");
      }
    }
    if (errorFlag) {
      delete dest;
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      throw GDLIOException("Type conversion error: Unable to convert given STRING to DCOMPLEX.");
    }
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
}

template<> BaseGDL* Data_<SpDComplex>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_CPX(SpDByte)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_INT:
    DO_CONVERT_START_CPX(SpDInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_UINT:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(SpDUInt, DUInt, DInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_LONG:
    DO_CONVERT_START_CPX(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_ULONG:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(SpDULong, DULong, DLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_LONG64:
    DO_CONVERT_START_CPX(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_ULONG64:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(SpDULong64, DULong64, DLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_FLOAT:
    DO_CONVERT_START_CPX(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_DOUBLE:
    DO_CONVERT_START_CPX(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_COMPLEX: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  /*case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO);
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)

      //FUTURE WORK: the version below is incredibely fast
      // using 'code=[1,2,3,4,5,6,7,9,12,13,14,15] & tic & for i=6,6 do begin a=cindgen(10LL^8) & b=fix(a,type=code[i]) & help,b & print,b[-1] & end & toc'
      // I get 18 seconds instead of 71 seconds on IDL. However the adaptive formatting of IDL must be respected, so this has to be completed. Probably in io.cpp
      //      for (SizeT i = 0; i < nEl; ++i) {
      //        (*dest)[i].resize(32);
      //        snprintf(&((*dest)[i])[0],32,"(%13g,%13g)",(*this)[i].real(),(*this)[i].imag());
      //      } 
    for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = "(" + i2s(real((*this)[i])) + "," + i2s(imag((*this)[i])) + ")";
      // The using of i2s needs iostream which takes long time; by JW, Feb 2022
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }*/

  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO);
	if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
	  for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = complex2string((*this)[i]);
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      //  JW
      //  for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = "(" + float2string(real((*this)[i])) + "," + float2string(imag((*this)[i])) + ")";
      //AC
            for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = complex2string((*this)[i]);

      /*
      for (SizeT i = 0; i < nEl; ++i) {
        (*dest)[i].resize(32);
        snprintf(&((*dest)[i])[0],32,"(%#13.6g,%#13.6g)",(*this)[i].real(),(*this)[i].imag());
	} */
	}
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  }//JW, Feb 2022

  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  return NULL; // get rid of warning
}

template<> BaseGDL* Data_<SpDComplexDbl>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();

  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_CPX(SpDByte)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_INT:
    DO_CONVERT_START_CPX(SpDInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_UINT:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(SpDUInt, DUInt, DInt)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_LONG:
    DO_CONVERT_START_CPX(SpDLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_ULONG:
    DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(SpDULong, DULong, DLong)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_LONG64:
    DO_CONVERT_START_CPX(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_ULONG64:
     DO_CONVERT_START_FLOAT_TO_UNSIGNED_CPX(SpDULong64, DULong64, DLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_FLOAT:
    DO_CONVERT_START_CPX(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_DOUBLE:
    DO_CONVERT_START_CPX(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_CPX
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  /*case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO);
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = "(" + i2s(real((*this)[i])) + "," + i2s(imag((*this)[i])) + ")";
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    //the using of i2s needs iostreal, which takes long time; by JW Feb 2022; 
    return dest;
  }*/
 
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO);
	if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
	  for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = dcomplex2string((*this)[i]);
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      //JW    for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = "(" + double2string(real((*this)[i])) + "," + double2string(imag((*this)[i])) + ")";

      //AC
      for (SizeT i = 0; i < nEl; ++i) (*dest)[i] = dcomplex2string((*this)[i]);
	}
    if ((mode & BaseGDL::CONVERT) != 0) delete this;
    return dest;
  } //JW, Feb 2022
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  return NULL; // get rid of warning
}




// 64 bit integers

template<> BaseGDL* Data_<SpDLong64>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();


  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_BIG_TO_SMALL(SpDByte, 0xFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL( 0xFF)
  case GDL_INT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_UINT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDUInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_LONG:
    DO_CONVERT_START_BIG_TO_SMALL(SpDLong, 0xFFFFFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFFFFFF)
  case GDL_ULONG:
    DO_CONVERT_START_BIG_TO_SMALL(SpDULong, 0xFFFFFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL( 0xFFFFFFFF)
  case GDL_LONG64: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_ULONG64:
    DO_CONVERT_START(SpDULong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
    char buf[32];
    Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 22);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%22lli", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 22);
      }
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      return dest;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%22lli", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 22);
      }
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      return dest;
    }
  }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
}

template<> BaseGDL* Data_<SpDULong64>::Convert2(DType destTy, BaseGDL::Convert2Mode mode) {
  TRACE_CONVERT2
  if (destTy == t) return (((mode & BaseGDL::COPY) != 0) ? Dup() : this);

  SizeT nEl = dd.size();


  switch (destTy) {
  case GDL_BYTE:
    DO_CONVERT_START_BIG_TO_SMALL(SpDByte, 0xFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFF)
  case GDL_INT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_UINT:
    DO_CONVERT_START_BIG_TO_SMALL(SpDUInt, 0xFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFF)
  case GDL_LONG:
    DO_CONVERT_START_BIG_TO_SMALL(SpDLong, 0xFFFFFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFFFFFF)
  case GDL_ULONG:
    DO_CONVERT_START_BIG_TO_SMALL(SpDULong, 0xFFFFFFFF)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END_BIG_TO_SMALL(0xFFFFFFFF)
  case GDL_LONG64:
    DO_CONVERT_START(SpDLong64)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_ULONG64: return (((mode & BaseGDL::COPY) != 0) ? Dup():this);
  case GDL_FLOAT:
    DO_CONVERT_START(SpDFloat)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_DOUBLE:
    DO_CONVERT_START(SpDDouble)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEX:
    DO_CONVERT_START(SpDComplex)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_COMPLEXDBL:
    DO_CONVERT_START(SpDComplexDbl)
    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
      DO_CONVERT_END
  case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
  {
   char buf[32];
   Data_<SpDString>* dest = new Data_<SpDString>(dim, BaseGDL::NOZERO, 22);
    if ((GDL_NTHREADS = parallelize(nEl, TP_ARRAY_INITIALISATION)) == 1) {
      for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%22llu", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 22);
      }
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      return dest;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) {
        snprintf(buf,32, "%22llu", (*this)[i]);
        (*dest)[i] = std::string(buf);
        //        (*dest)[i] = i2s((*this)[i], 22);
      }
      if ((mode & BaseGDL::CONVERT) != 0) delete this;
      return dest;
    }
  }
  case GDL_PTR:
  case GDL_OBJ:
  case GDL_STRUCT:
  case GDL_UNDEF:
  default:
    if (BaseGDL::interpreter != NULL && BaseGDL::interpreter->CallStack().size() > 0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
    throw GDLException("Cannot convert to this type.");
  }

  // get rid of warning
  return NULL;
}
