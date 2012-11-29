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

#include "includefirst.hpp"

//#include "datatypes.hpp"
#include "dstructgdl.hpp"
#include "real2int.hpp"
#include "ofmt.hpp" // OutAuto

#include "dinterpreter.hpp"

using namespace std;

//#define TRACE_CONVERT2 cout << "Convert2 " << this->TypeStr() << " -> " << destTy << "\tn " << dd.size() << "\tmode " << mode << endl;
#define TRACE_CONVERT2

#define SINGLE_ELEMENT_OPTIMIZATION                   if( nEl == 1) \
{ (*dest)[0]=(*this)[0]; if( (mode & BaseGDL::CONVERT) != 0) delete this; return dest;}


// for double -> string
inline string double2string( DDouble d)      
{
  std::ostringstream os;
  OutAuto( os, d, 16, 8, ' ');
  return os.str();
}

// for float -> string
inline string float2string( DFloat f)      
{
  std::ostringstream os;
  OutAuto( os, f, 13, 6, ' ');
  return os.str();
}

// for string -> float/double
template <typename real_t>
inline real_t string2real_helper(const char* cStart, char** cEnd);

template <>
inline float string2real_helper<float>(const char* cStart, char** cEnd)
{
#ifdef _MSC_VER
  return strtod(cStart, cEnd);
#else
  return strtof(cStart, cEnd);
#endif
}

template <>
inline double string2real_helper<double>(const char* cStart, char** cEnd)
{
  return strtod(cStart, cEnd);
}

template <typename real_t>
inline real_t string2real(const char* cStart, char** cEnd)
{
  const char* d = strpbrk(cStart, "Dd");
  if (d == NULL) 
    return string2real_helper<real_t>(cStart, cEnd);
  else
  {
    string tmps(cStart);
    tmps[d - cStart] = 'E';
    char* tmpcp;
    real_t tmpf = string2real_helper<real_t>(tmps.c_str(), &tmpcp);
    *cEnd = const_cast<char*>(cStart) + (tmpcp - tmps.c_str());
    return tmpf;
  }
}

// every type need this function which defines its conversion to all other types
// so for every new type each of this functions has to be extended
// and a new function has to be 'specialized'
// not very nice, but how else to do?
// each function creates the new type on the heap

// non convertables
BaseGDL* DStructGDL::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);
//DInterpreter* testDbg = BaseGDL::interpreter;
//int szDbg = BaseGDL::interpreter->CallStack().size();
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Struct expression not allowed in this context: "+BaseGDL::interpreter->Name(this));
  throw GDLException("Struct expression not allowed in this context.");
  return NULL; // get rid of warning 
}  
template<> BaseGDL* Data_<SpDPtr>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Ptr expression not allowed in this context: "+BaseGDL::interpreter->Name(this));
  throw GDLException("Ptr expression not allowed in this context.");
  return NULL; // get rid of warning 
}  
template<> BaseGDL* Data_<SpDObj>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Object expression not allowed in this context: "+BaseGDL::interpreter->Name(this));
  throw GDLException("Object expression not allowed in this context.");
  return NULL; // get rid of warning 
}  

// the real Convert2 functions
template<> BaseGDL* Data_<SpDByte>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();
  
  switch( destTy)
    {
//    case GDL_BYTE:
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: // GDL_BYTE to GDL_STRING: remove first dim
      {
	if( mode == BaseGDL::COPY_BYTE_AS_INT)
	  {
	    Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
// TRACEOMP( __FILE__, __LINE__)
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
// {
// #pragma omp for
	    for( SizeT i=0; i < nEl; ++i)
	      (*dest)[i]=i2s(static_cast<int>((*this)[i]),4);
// }
	    if( (mode & BaseGDL::CONVERT) != 0) delete this;
	    return dest;
	  }
	else
	  {
	    dimension strDim( dim);
	    SizeT strLen = strDim.Remove( 0);

	    if (strLen == 0) strLen = 1;

	    nEl /= strLen;

	    char *buf = new char[ strLen+1];
	    ArrayGuard<char> guard( buf);

	    buf[ strLen] = 0;
	    Data_<SpDString>* dest = 
	      new Data_<SpDString>( strDim, BaseGDL::NOZERO);
	    for( SizeT i=0; i < nEl; ++i)
	      {
		SizeT basePtr = i*strLen;
		for( SizeT b=0; b<strLen; b++)
		  buf[b] = (*this)[ basePtr+b];
		
		(*dest)[i]=buf; //i2s((*this)[i]);
	      }
	    
	    if( (mode & BaseGDL::CONVERT) != 0) delete this;
	    return dest;
	  }
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   return NULL; // get rid of warning
} // GDL_BYTE 

template<> BaseGDL* Data_<SpDInt>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_INT:
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
// This does not work, as the type constant is not changed 
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDUInt>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDUInt>*>(this->Dup());
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast<DLong>((*this)[i]);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
//       	  (*dest)[i]=static_cast<DLong64>((*this)[i]);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s((*this)[i],8);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // GDL_INT  

template<> BaseGDL* Data_<SpDUInt>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDInt>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDInt>*>(this->Dup());
      //    case GDL_UINT:
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s((*this)[i],8);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // GDL_UINT




template<> BaseGDL* Data_<SpDLong>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_LONG:
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDULong>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDULong>*>(this->Dup());
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s((*this)[i],12);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // GDL_LONG  




template<> BaseGDL* Data_<SpDULong>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDLong>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDLong>*>(this->Dup());
      //    case GDL_ULONG:
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s((*this)[i],12);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
//   #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
    	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // GDL_ULONG   



template<> BaseGDL* Data_<SpDFloat>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		(*dest)[0]=Real2DByte<float>((*this)[0]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
	  (*dest)[i]=Real2DByte<float>((*this)[i]); 
}	//(*dest)[i]=Real2DByte((*this)[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		(*dest)[0]=Real2Int<DInt,float>((*this)[0]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,float>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		(*dest)[0]= static_cast< DUInt>( ((*this)[0]));//Real2Int<DUInt,float>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]= static_cast< DUInt>( ((*this)[i]));//Real2Int<DUInt,float>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		(*dest)[0]=Real2Int<DLong,float>((*this)[0]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,float>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong>( ((*this)[i]));//Real2Int<DULong,float>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong>( ((*this)[i]));//Real2Int<DULong,float>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong64,float>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,float>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) 
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong64>( ((*this)[i]));//Real2Int<DULong64,float>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong64>( ((*this)[i]));//Real2Int<DULong64,float>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_FLOAT: 
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=float2string((*this)[i]);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
    	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  




template<> BaseGDL* Data_<SpDDouble>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2DByte<double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
	  (*dest)[i]=Real2DByte<double>((*this)[i]); 
	  //(*dest)[i]=Double2DByte((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DInt,double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,double>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DUInt>((*this)[i]);//Real2Int<DUInt,double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DUInt>((*this)[i]);//Real2Int<DUInt,double>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong,double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,double>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong>((*this)[i]);//Real2Int<DULong,double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong>((*this)[i]);//Real2Int<DULong,double>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong64,double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,double>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong64>((*this)[i]);//Real2Int<DULong64,double>((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong64>((*this)[i]);//Real2Int<DULong64,double>((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_DOUBLE: 
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=double2string((*this)[i]);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) 
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  





inline void StringConversionError( bool& errorFlag, BaseGDL::Convert2Mode mode, const string& msg)
{
  if( (mode & BaseGDL::THROWIOERROR) != 0)
 {
	errorFlag = true;
/*    delete dest;
    throw GDLIOException( msg);*/
  }
else
	Warning( msg);
}

template<> BaseGDL* Data_<SpDString>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();
  
  bool errorFlag = false;

  switch( destTy)
    {
    case GDL_BYTE: // GDL_STRING to GDL_BYTE: add first dim
      {
	SizeT maxLen = 1; // empty string is converted to 0b
      	for( SizeT i=0; i < nEl; ++i)
	  if( (*this)[i].length() > maxLen) maxLen = (*this)[i].length();

	dimension bytDim( dim);
	bytDim >> maxLen;

      	Data_<SpDByte>* dest=new Data_<SpDByte>( bytDim); // zero fields
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
	  {
	    SizeT basePtr = i*maxLen;

	    SizeT strLen = (*this)[ i].length();
	    for( SizeT b=0; b<strLen; b++)
	      (*dest)[basePtr + b] = (*this)[ i][ b];
 	  }
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtol(cStart,&cEnd,10);
      	    if( cEnd == cStart && (*this)[i] != "")
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to INT.");
      	      }
	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to INT.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case GDL_UINT: 
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtoul(cStart,&cEnd,10);
      	    if( cEnd == cStart && (*this)[i] != "")
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to UINT.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to UINT.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG: 
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtol(cStart,&cEnd,10);
      	    if( cEnd == cStart && (*this)[i] != "")
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to LONG.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to LONG.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG: 
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtoul(cStart,&cEnd,10);
      	    if( cEnd == cStart && (*this)[i] != "")
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to ULONG.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to ULONG.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64: 
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtol(cStart,&cEnd,10);
      	    if( cEnd == cStart && (*this)[i] != "")
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to LONG64.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to LONG64.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64: 
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtoul(cStart,&cEnd,10);
      	    if( cEnd == cStart && (*this)[i] != "")
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to ULONG64.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to ULONG64.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i] = string2real<float>(cStart, &cEnd);
      	    if((cEnd == cStart && (*this)[i] != "")) //  || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to FLOAT.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to FLOAT.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
	    char* cEnd;
	    (*dest)[i] = string2real<double>( cStart, &cEnd);
	    if( (cEnd == cStart && (*this)[i] != "")) // || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to DOUBLE.");
	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to DOUBLE.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_STRING: 
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=string2real<float>(cStart,&cEnd);
      	    if((cEnd == cStart && (*this)[i] != "")) // || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to COMPLEX.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to COMPLEX.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared( errorFlag, mode)
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=(*this)[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=string2real<double>(cStart,&cEnd);
      	    if((cEnd == cStart && (*this)[i] != "")) // || (cEnd - cStart) != strlen(cStart)) // reports error for "16 "
      	      {
StringConversionError( errorFlag, mode, "Type conversion error: "
				       "Unable to convert given STRING: '"+
				       (*this)[i]+"' to DCOMPLEX.");
      	      }
      	  }
}
	if( errorFlag)
	{
		delete dest;
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		throw GDLIOException( "Type conversion error: Unable to convert given STRING to DCOMPLEX.");
	}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

  // get rid of warning
  return NULL; 
}  




template<> BaseGDL* Data_<SpDComplex>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2DByte<float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2DByte<float>(real((*this)[i])); 
}	//(*dest)[i]=Real2DByte(real((*this)[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DInt,float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
    	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,float>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DUInt>(real((*this)[i]));//Real2Int<DUInt,float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DUInt>(real((*this)[i]));//Real2Int<DUInt,float>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong,float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,float>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong>(real((*this)[i]));//Real2Int<DULong,float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong>(real((*this)[i]));//Real2Int<DULong,float>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong64,float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,float>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong64>(real((*this)[i]));//Real2Int<DULong64,float>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong64>(real((*this)[i]));//Real2Int<DULong64,float>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=real((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*dest)[i]=real((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=real((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=real((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]="("+i2s(real((*this)[i]))+","+i2s(imag((*this)[i]))+")";
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_COMPLEX: 
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=new Data_<SpDComplexDbl>( dim, 
							     BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

  return NULL; // get rid of warning
}  




template<> BaseGDL* Data_<SpDComplexDbl>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2DByte<double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2DByte<double>(real((*this)[i])); 
}      	  //(*dest)[i]=Double2DByte(real((*this)[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DInt,double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,double>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DUInt>(real((*this)[i]));//Real2Int<DUInt,double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DUInt>(real((*this)[i]));//Real2Int<DUInt,double>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong,double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,double>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong>(real((*this)[i]));//Real2Int<DULong,double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong>(real((*this)[i]));//Real2Int<DULong,double>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=Real2Int<DLong64,double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,double>(real((*this)[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
}
      	return dest;
      }
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=static_cast< DULong64>(real((*this)[i]));//Real2Int<DULong64,double>(real((*this)[i]));
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast< DULong64>(real((*this)[i]));//Real2Int<DULong64,double>(real((*this)[i])); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_FLOAT: 
      {
	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=real((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*dest)[i]=real((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i]=real((*this)[i]);
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=real((*this)[i]); 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]="("+i2s(real((*this)[i]))+","+i2s(imag((*this)[i]))+")";
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
#ifdef SINGLE_ELEMENT_OPTIMIZATION
	if( nEl == 1) \
	{
		const SizeT i = 0;
		(*dest)[i] = DComplex( static_cast<float>((*this)[i].real()),
					static_cast<float>((*this)[i].imag()) );
		if( (mode & BaseGDL::CONVERT) != 0) delete this;
		return dest;
	}
#endif
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
//{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
// 	  (*dest)[i] = DComplex( static_cast<float>((*this)[i].real()), 
// 				 static_cast<float>((*this)[i].imag()) );
//}      	  
	(*dest)[i]=(*this)[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_COMPLEXDBL: 
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

  return NULL; // get rid of warning
}  




// 64 bit integers
template<> BaseGDL* Data_<SpDLong64>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case GDL_LONG64:
    case GDL_ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDULong64>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDULong64>*>(this->Dup());
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s((*this)[i],22);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  



template<> BaseGDL* Data_<SpDULong64>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
TRACE_CONVERT2
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case GDL_BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDLong64>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDLong64>*>(this->Dup());
//  case GDL_ULONG64:
    case GDL_FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i]; 
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
      	for( int i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s((*this)[i],22);
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
	SINGLE_ELEMENT_OPTIMIZATION
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
// #pragma omp for
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=(*this)[i];
}
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case GDL_PTR:
    case GDL_OBJ:
    case GDL_STRUCT:
    case GDL_UNDEF: 
if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) BaseGDL::interpreter->CallStack().back()->Throw("Cannot convert to this type.");
throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  
