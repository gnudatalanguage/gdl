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

// TODO: adjust the with for string conversion for each type (i2s(dd[i],X))

// Convert2( DType) functions

#include "includefirst.hpp"

//#include "datatypes.hpp"
#include "dstructgdl.hpp"
#include "real2int.hpp"
#include "ofmt.hpp" // OutAuto

using namespace std;

// for double -> string
inline string double2string( DDouble d)      
{
  std::ostringstream os;
  OutAuto( os, d, 16, 8);
  return os.str();
}

// every type need this function which defines its conversion to all other types
// so for every new type each of this functions has to be extended
// and a new function has to be 'specialized'
// not very nice, but how else to do?
// each function creates the new type on the heap

// non convertables
BaseGDL* DStructGDL::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);
  throw GDLException("Struct expression not allowed in this context.");
  return NULL; // get rid of warning 
}  
template<> BaseGDL* Data_<SpDPtr>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);
  throw GDLException("Ptr expression not allowed in this context.");
  return NULL; // get rid of warning 
}  
template<> BaseGDL* Data_<SpDObj>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);
  throw GDLException("Object expression not allowed in this context.");
  return NULL; // get rid of warning 
}  


// the real Convert2 functions
template<> BaseGDL* Data_<SpDByte>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();
  
  switch( destTy)
    {
//    case BYTE:
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: // BYTE to STRING: remove first dim
      {
	if( mode == BaseGDL::COPY_BYTE_AS_INT)
	  {
	    Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
	    for( SizeT i=0; i < nEl; ++i)
	      (*dest)[i]=i2s(dd[i],4);
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
		  buf[b] = dd[ basePtr+b];
		
		(*dest)[i]=buf; //i2s(dd[i]);
	      }
	    
	    if( (mode & BaseGDL::CONVERT) != 0) delete this;
	    return dest;
	  }
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   return NULL; // get rid of warning
} // BYTE 

template<> BaseGDL* Data_<SpDInt>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case INT:
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
// This does not work, as the type constant is not changed 
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDUInt>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDUInt>*>(this->Dup());
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast<DLong>(dd[i]);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=static_cast<DLong64>(dd[i]);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],8);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // INT  

template<> BaseGDL* Data_<SpDUInt>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDInt>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDInt>*>(this->Dup());
      //    case UINT:
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],8);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // UINT


template<> BaseGDL* Data_<SpDLong>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case LONG:
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDULong>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDULong>*>(this->Dup());
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],8);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // LONG  

template<> BaseGDL* Data_<SpDULong>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDLong>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDLong>*>(this->Dup());
      //    case ULONG:
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],8);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
} // ULONG   

template<> BaseGDL* Data_<SpDFloat>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DByte,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DUInt,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong64,float>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case FLOAT: 
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],13);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  

template<> BaseGDL* Data_<SpDDouble>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DByte,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DUInt,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong64,double>(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case DOUBLE: 
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=double2string(dd[i]);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  

template<> BaseGDL* Data_<SpDString>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();
  
  switch( destTy)
    {
    case BYTE: // STRING to BYTE: add first dim
      {
	SizeT maxLen = 1; // empty string is converted to 0b
      	for( SizeT i=0; i < nEl; ++i)
	  if( dd[i].length() > maxLen) maxLen = dd[i].length();

	dimension bytDim( dim);
	bytDim >> maxLen;

      	Data_<SpDByte>* dest=new Data_<SpDByte>( bytDim); // zero fields
      	for( SizeT i=0; i < nEl; ++i)
	  {
	    SizeT basePtr = i*maxLen;

	    SizeT strLen = dd[ i].length();
	    for( SizeT b=0; b<strLen; b++)
	      (*dest)[basePtr + b] = dd[ i][ b];
 	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtol(cStart,&cEnd,10);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to INT.");
      	      }
	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case UINT: 
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtoul(cStart,&cEnd,10);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to UINT.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG: 
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtol(cStart,&cEnd,10);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to LONG.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG: 
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtoul(cStart,&cEnd,10);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to ULONG.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64: 
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtol(cStart,&cEnd,10);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to LONG64.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64: 
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtoul(cStart,&cEnd,10);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to ULONG64.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i] = strtod(cStart,&cEnd);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to FLOAT.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
	    char* cEnd;
	    (*dest)[i] = strtod( cStart, &cEnd);
	    if( cEnd == cStart && dd[i] != "")
	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to DOUBLE.");
	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case STRING: 
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtod(cStart,&cEnd);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to COMPLEX.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  {
      	    const char* cStart=dd[i].c_str();
      	    char* cEnd;
      	    (*dest)[i]=strtod(cStart,&cEnd);
      	    if( cEnd == cStart && dd[i] != "")
      	      {
		Warning("Type conversion error: "
			"Unable to convert given STRING: '"+dd[i]+"' to DCOMPLEX.");
      	      }
      	  }
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  

template<> BaseGDL* Data_<SpDComplex>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DByte,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DUInt,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong64,float>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	for( SizeT i=0; i < nEl; ++i)
	  (*dest)[i]=real(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=real(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]="("+i2s(real(dd[i]))+","+i2s(imag(dd[i]))+")";
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case COMPLEX: 
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=new Data_<SpDComplexDbl>( dim, 
							     BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

  return NULL; // get rid of warning
}  

template<> BaseGDL* Data_<SpDComplexDbl>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DByte,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DInt,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DUInt,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DLong64,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=Real2Int<DULong64,double>(real(dd[i])); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case FLOAT: 
      {
	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
	for( SizeT i=0; i < nEl; ++i)
	  (*dest)[i]=real(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=real(dd[i]); 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]="("+i2s(real(dd[i]))+","+i2s(imag(dd[i]))+")";
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
	  (*dest)[i] = DComplex( static_cast<float>(dd[i].real()), 
				 static_cast<float>(dd[i].imag()) );
	//      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case COMPLEXDBL: 
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

  return NULL; // get rid of warning
}  

// 64 bit integers
template<> BaseGDL* Data_<SpDLong64>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
      //    case LONG64:
    case ULONG64:
      {
      	Data_<SpDULong64>* dest=new Data_<SpDULong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDULong64>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDULong64>*>(this->Dup());
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],22);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  

template<> BaseGDL* Data_<SpDULong64>::Convert2( DType destTy, BaseGDL::Convert2Mode mode)
{
  if( destTy == t) return (((mode & BaseGDL::COPY) != 0)?Dup():this);

  SizeT nEl=dd.size();

  switch( destTy)
    {
    case BYTE:
      {
      	Data_<SpDByte>* dest=new Data_<SpDByte>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case INT:
      {
      	Data_<SpDInt>* dest=new Data_<SpDInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case UINT:
      {
      	Data_<SpDUInt>* dest=new Data_<SpDUInt>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG:
      {
      	Data_<SpDLong>* dest=new Data_<SpDLong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case ULONG:
      {
      	Data_<SpDULong>* dest=new Data_<SpDULong>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case LONG64:
      {
      	Data_<SpDLong64>* dest=new Data_<SpDLong64>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
//       if( (mode & BaseGDL::CONVERT) != 0)
// 	return reinterpret_cast<Data_<SpDLong64>*>(this);
//       else
// 	return reinterpret_cast<Data_<SpDLong64>*>(this->Dup());
//  case ULONG64:
    case FLOAT: 
      {
      	Data_<SpDFloat>* dest=new Data_<SpDFloat>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case DOUBLE: 
      {
      	Data_<SpDDouble>* dest=new Data_<SpDDouble>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i]; 
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case STRING: 
      {
      	Data_<SpDString>* dest=new Data_<SpDString>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=i2s(dd[i],22);
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEX: 
      {
      	Data_<SpDComplex>* dest=new Data_<SpDComplex>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case COMPLEXDBL: 
      {
      	Data_<SpDComplexDbl>* dest=
	  new Data_<SpDComplexDbl>( dim, BaseGDL::NOZERO);
      	for( SizeT i=0; i < nEl; ++i)
      	  (*dest)[i]=dd[i];
	if( (mode & BaseGDL::CONVERT) != 0) delete this;
      	return dest;
      }
    case PTR:
    case OBJECT:
    case STRUCT:
    case UNDEF: throw GDLException("Cannot convert to this type.");
    }

   // get rid of warning
  return NULL; 
}  
