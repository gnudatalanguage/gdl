/***************************************************************************
                          getas.cpp  -  get element as different type
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

// to be included from datatypes.cpp
#ifdef INCLUDE_GETAS_CPP

// #include "includefirst.hpp"
// //#include "datatypes.hpp"
// #include "dstructgdl.hpp"
// #include "real2int.hpp"
#include "ofmt.hpp" // OutAuto
// 
// #include "dinterpreter.hpp"

using namespace std;

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



// typedef Data_<SpDByte>       DByteGDL;
// typedef Data_<SpDInt>        DIntGDL;
// typedef Data_<SpDUInt>       DUIntGDL;
// typedef Data_<SpDLong>       DLongGDL;
// typedef Data_<SpDULong>      DULongGDL;
// typedef Data_<SpDLong64>     DLong64GDL;
// typedef Data_<SpDULong64>    DULong64GDL;
// typedef Data_<SpDFloat>      DFloatGDL;
// typedef Data_<SpDDouble>     DDoubleGDL;
// typedef Data_<SpDString>     DStringGDL;
// typedef Data_<SpDPtr>        DPtrGDL;
// typedef Data_<SpDObj>        DObjGDL;
// typedef Data_<SpDComplex>    DComplexGDL;
// typedef Data_<SpDComplexDbl> DComplexDblGDL;


// general case
template<class Sp>
template<class To> 
typename Data_<To>::Ty Data_<Sp>::GetAs( SizeT i)
  {
    return this->dd[ i];
  }

// non convertable types
// DPtrGDL specialization  
template<>
template<class To> 
typename Data_<To>::Ty Data_<SpDPtr>::GetAs( SizeT i)
  {
  if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) 
      BaseGDL::interpreter->CallStack().back()->Throw("Ptr expression not allowed in this context: "+BaseGDL::interpreter->Name(this));
  throw GDLException("Ptr expression not allowed in this context.");
  }
// DObjGDL specialization  
template<>
template<class To> 
typename Data_<To>::Ty Data_<SpDObj>::GetAs( SizeT i)
  {
  if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0) 
    BaseGDL::interpreter->CallStack().back()->Throw("Object expression not allowed in this context: "+BaseGDL::interpreter->Name(this));
  throw GDLException("Object expression not allowed in this context.");
  }
// DStructGDL   
template<class To> 
typename Data_<To>::Ty DStructGDL::GetAs( SizeT i)
  {
  if(BaseGDL::interpreter!=NULL&&BaseGDL::interpreter->CallStack().size()>0)
    BaseGDL::interpreter->CallStack().back()->Throw("Struct expression not allowed in this context: "+BaseGDL::interpreter->Name(this));
  throw GDLException("Struct expression not allowed in this context.");
  }

// Ptr, Obj, Struct: GetAs throws
// The operators are overloaded for these types, they throw
// hence GetAs<SpDPtr>( SizeT i) is never called and does not need to be overloaded
  
// integer types  
// DByteGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDByte>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],4);
  }
// DIntGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDInt>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],8);
  }
// DUIntGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDUInt>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],8);
  }
// DLongGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDLong>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],12);
  }
// DULongGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDULong>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],12);
  }
// DLong64GDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDLong64>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],22);
  }
// DULong64GDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDULong64>::GetAs<SpDString>( SizeT i)
  {
    return i2s<>(this->dd[ i],22);
  }

  
// DFloatGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDByte>::Ty Data_<SpDFloat>::GetAs<SpDByte>( SizeT i)
  {
    return Real2DByte<float>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDInt>::Ty Data_<SpDFloat>::GetAs<SpDInt>( SizeT i)
  {
    return Real2Int<DInt,float>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDUInt>::Ty Data_<SpDFloat>::GetAs<SpDUInt>( SizeT i)
  {
    return static_cast< DUInt>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDLong>::Ty Data_<SpDFloat>::GetAs<SpDLong>( SizeT i)
  {
    return Real2Int<DLong,float>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDLong64>::Ty Data_<SpDFloat>::GetAs<SpDLong64>( SizeT i)
  {
    return Real2Int<DLong64,float>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDULong>::Ty Data_<SpDFloat>::GetAs<SpDULong>( SizeT i)
  {
    return static_cast< DULong>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDULong64>::Ty Data_<SpDFloat>::GetAs<SpDULong64>( SizeT i)
  {
    return static_cast< DULong64>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDFloat>::GetAs<SpDString>( SizeT i)
  {
    return float2string((*this)[i]);
  }
  
  
  
// DDoubleGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDByte>::Ty Data_<SpDDouble>::GetAs<SpDByte>( SizeT i)
  {
    return Real2DByte<double>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDInt>::Ty Data_<SpDDouble>::GetAs<SpDInt>( SizeT i)
  {
    return Real2Int<DInt,double>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDUInt>::Ty Data_<SpDDouble>::GetAs<SpDUInt>( SizeT i)
  {
    return static_cast< DUInt>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDLong>::Ty Data_<SpDDouble>::GetAs<SpDLong>( SizeT i)
  {
    return Real2Int<DLong,double>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDLong64>::Ty Data_<SpDDouble>::GetAs<SpDLong64>( SizeT i)
  {
    return Real2Int<DLong64,double>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDULong>::Ty Data_<SpDDouble>::GetAs<SpDULong>( SizeT i)
  {
    return static_cast< DULong>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDULong64>::Ty Data_<SpDDouble>::GetAs<SpDULong64>( SizeT i)
  {
    return static_cast< DULong64>((*this)[i]);
  }
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDDouble>::GetAs<SpDString>( SizeT i)
  {
    return double2string((*this)[i]);
  }
  

// DStringGDL full specializations  
inline void StringConversionErrorGetAs( const string& msg)
{
//   if( (mode & BaseGDL::THROWIOERROR) != 0)
//   {
//     errorFlag = true;
//   }
//   else
    Warning( msg);
}

template<>
template<> 
/*typename*/ Data_<SpDByte>::Ty Data_<SpDString>::GetAs<SpDByte>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    long int result =strtol(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to BYTE.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDInt>::Ty Data_<SpDString>::GetAs<SpDInt>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    long int result =strtol(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to INT.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDUInt>::Ty Data_<SpDString>::GetAs<SpDUInt>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    unsigned long int result =strtoul(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to UINT.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDLong>::Ty Data_<SpDString>::GetAs<SpDLong>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    long int result =strtol(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to LONG.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDULong>::Ty Data_<SpDString>::GetAs<SpDULong>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    unsigned long int result =strtoul(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to ULONG.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDLong64>::Ty Data_<SpDString>::GetAs<SpDLong64>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    Data_<SpDLong64>::Ty result =strtol(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to LONG64.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDULong64>::Ty Data_<SpDString>::GetAs<SpDULong64>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    Data_<SpDULong64>::Ty result =strtoul(cStart,&cEnd,10);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to ULONG64.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDFloat>::Ty Data_<SpDString>::GetAs<SpDFloat>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    float result = strtod(cStart,&cEnd);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to FLOAT.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDDouble>::Ty Data_<SpDString>::GetAs<SpDDouble>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    double result = strtod(cStart,&cEnd);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to DOUBLE.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDComplex>::Ty Data_<SpDString>::GetAs<SpDComplex>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    float result = strtod(cStart,&cEnd);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to COMPLEX.");
    }
    return result;
  }
template<>
template<> 
/*typename*/ Data_<SpDComplexDbl>::Ty Data_<SpDString>::GetAs<SpDComplexDbl>( SizeT i)
  {
    const char* cStart=(*this)[i].c_str();
    char* cEnd;
    double result = strtod(cStart,&cEnd);
    if( cEnd == cStart && (*this)[i] != "")
    {
      StringConversionErrorGetAs( "Type conversion error: "
      "Unable to convert given STRING: '"+(*this)[i]+"' to DCOMPLEX.");
    }
    return result;
  }
  
  
// DComplexGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDByte>::Ty Data_<SpDComplex>::GetAs<SpDByte>( SizeT i)
  {
    return Real2DByte<float>(real((*this)[i])); 
  }
template<>
template<> 
/*typename*/ Data_<SpDInt>::Ty Data_<SpDComplex>::GetAs<SpDInt>( SizeT i)
  {
    return Real2Int<DInt,float>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDUInt>::Ty Data_<SpDComplex>::GetAs<SpDUInt>( SizeT i)
  {
    return static_cast< DUInt>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDLong>::Ty Data_<SpDComplex>::GetAs<SpDLong>( SizeT i)
  {
    return Real2Int<DLong,float>(real((*this)[i])); 
  }
template<>
template<> 
/*typename*/ Data_<SpDLong64>::Ty Data_<SpDComplex>::GetAs<SpDLong64>( SizeT i)
  {
    return Real2Int<DLong64,float>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDULong>::Ty Data_<SpDComplex>::GetAs<SpDULong>( SizeT i)
  {
    return static_cast< DULong>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDULong64>::Ty Data_<SpDComplex>::GetAs<SpDULong64>( SizeT i)
  {
    return static_cast< DULong64>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDFloat>::Ty Data_<SpDComplex>::GetAs<SpDFloat>( SizeT i)
  {
    return real((*this)[i]); 
  }
template<>
template<> 
/*typename*/ Data_<SpDDouble>::Ty Data_<SpDComplex>::GetAs<SpDDouble>( SizeT i)
  {
    return real((*this)[i]); 
  }
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDComplex>::GetAs<SpDString>( SizeT i)
  {
    return "("+i2s(real((*this)[i]))+","+i2s(imag((*this)[i]))+")";
  }
  


// DComplexDblGDL full specializations  
template<>
template<> 
/*typename*/ Data_<SpDByte>::Ty Data_<SpDComplexDbl>::GetAs<SpDByte>( SizeT i)
  {
    return Real2DByte<double>(real((*this)[i])); 
  }
template<>
template<> 
/*typename*/ Data_<SpDInt>::Ty Data_<SpDComplexDbl>::GetAs<SpDInt>( SizeT i)
  {
    return Real2Int<DInt,double>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDUInt>::Ty Data_<SpDComplexDbl>::GetAs<SpDUInt>( SizeT i)
  {
    return static_cast< DUInt>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDLong>::Ty Data_<SpDComplexDbl>::GetAs<SpDLong>( SizeT i)
  {
    return Real2Int<DLong,double>(real((*this)[i])); 
  }
template<>
template<> 
/*typename*/ Data_<SpDLong64>::Ty Data_<SpDComplexDbl>::GetAs<SpDLong64>( SizeT i)
  {
    return Real2Int<DLong64,double>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDULong>::Ty Data_<SpDComplexDbl>::GetAs<SpDULong>( SizeT i)
  {
    return static_cast< DULong>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDULong64>::Ty Data_<SpDComplexDbl>::GetAs<SpDULong64>( SizeT i)
  {
    return static_cast< DULong64>(real((*this)[i]));
  }
template<>
template<> 
/*typename*/ Data_<SpDFloat>::Ty Data_<SpDComplexDbl>::GetAs<SpDFloat>( SizeT i)
  {
    return real((*this)[i]); 
  }
template<>
template<> 
/*typename*/ Data_<SpDDouble>::Ty Data_<SpDComplexDbl>::GetAs<SpDDouble>( SizeT i)
  {
    return real((*this)[i]); 
  }
template<>
template<> 
/*typename*/ Data_<SpDString>::Ty Data_<SpDComplexDbl>::GetAs<SpDString>( SizeT i)
  {
    return "("+i2s(real((*this)[i]))+","+i2s(imag((*this)[i]))+")";
  }

#endif
