/***************************************************************************
                          typetraits.cpp  -  type parameterization
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2006 by Marc Schellens
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

#include "includefirst.hpp"

#include "typetraits.hpp"
#include "datatypes.hpp"
#include "dstructgdl.hpp"

using namespace std;

const DType  SpDByte::t=GDL_BYTE; // type ID
const string SpDByte::str("BYTE"); // type string
const DByte  SpDByte::zero=0;
const bool SpDByte::IS_INTEGER=true;
const bool SpDByte::IS_SIGNED=false;
const bool SpDByte::IS_NUMERIC=true;
const bool SpDByte::IS_COMPLEX=false;
BaseGDL* SpDByte::GetTag() const { return new SpDByte(*this);}
DType   SpDByte::Type()    const { return t;}
const std::string& SpDByte::TypeStr() const { return str;}

const DType  SpDInt::t=GDL_INT; // type ID
const string SpDInt::str("INT"); // type string
const DInt   SpDInt::zero=0;
const bool SpDInt::IS_INTEGER=true;
const bool SpDInt::IS_SIGNED=true;
const bool SpDInt::IS_NUMERIC=true;
const bool SpDInt::IS_COMPLEX=false;
BaseGDL* SpDInt::GetTag() const { return new SpDInt(*this);}
DType   SpDInt::Type()    const { return t;}
const std::string& SpDInt::TypeStr() const { return str;}

const DType  SpDUInt::t=GDL_UINT; // type ID
const string SpDUInt::str("UINT"); // type string
const DUInt  SpDUInt::zero=0;
const bool SpDUInt::IS_INTEGER=true;
const bool SpDUInt::IS_SIGNED=false;
const bool SpDUInt::IS_NUMERIC=true;
const bool SpDUInt::IS_COMPLEX=false;
BaseGDL* SpDUInt::GetTag() const { return new SpDUInt(*this);}
DType   SpDUInt::Type()    const { return t;}
const std::string& SpDUInt::TypeStr() const { return str;}


const DType  SpDLong::t=GDL_LONG; // type ID
const string SpDLong::str("LONG"); // type string
const DLong  SpDLong::zero=0;
const bool SpDLong::IS_INTEGER=true;
const bool SpDLong::IS_SIGNED=true;
const bool SpDLong::IS_NUMERIC=true;
const bool SpDLong::IS_COMPLEX=false;
BaseGDL* SpDLong::GetTag() const { return new SpDLong(*this);}
DType   SpDLong::Type()    const { return t;}
const std::string& SpDLong::TypeStr() const { return str;}

const DType  SpDULong::t=GDL_ULONG; // type ID
const string SpDULong::str("ULONG"); // type string
const DULong SpDULong::zero=0;
const bool SpDULong::IS_INTEGER=true;
const bool SpDULong::IS_SIGNED=false;
const bool SpDULong::IS_NUMERIC=true;
const bool SpDULong::IS_COMPLEX=false;
BaseGDL* SpDULong::GetTag() const { return new SpDULong(*this);}
DType   SpDULong::Type()    const { return t;}
const std::string& SpDULong::TypeStr() const { return str;}

const DType  SpDLong64::t=GDL_LONG64; // type ID
const string SpDLong64::str("LONG64"); // type string
const DLong64  SpDLong64::zero=0;
const bool SpDLong64::IS_INTEGER=true;
const bool SpDLong64::IS_SIGNED=true;
const bool SpDLong64::IS_NUMERIC=true;
const bool SpDLong64::IS_COMPLEX=false;
BaseGDL* SpDLong64::GetTag() const { return new SpDLong64(*this);}
DType   SpDLong64::Type()    const { return t;}
const std::string& SpDLong64::TypeStr() const { return str;}

const DType  SpDULong64::t=GDL_ULONG64; // type ID
const string SpDULong64::str("ULONG64"); // type string
const DULong64 SpDULong64::zero=0;
const bool SpDULong64::IS_INTEGER=true;
const bool SpDULong64::IS_SIGNED=false;
const bool SpDULong64::IS_NUMERIC=true;
const bool SpDULong64::IS_COMPLEX=false;
BaseGDL* SpDULong64::GetTag() const { return new SpDULong64(*this);}
DType   SpDULong64::Type()    const { return t;}
const std::string& SpDULong64::TypeStr() const { return str;}

const DType  SpDFloat::t=GDL_FLOAT; // type ID
const string SpDFloat::str("FLOAT"); // type string
const DFloat SpDFloat::zero=0.0;
const bool SpDFloat::IS_INTEGER=false;
const bool SpDFloat::IS_SIGNED=true;
const bool SpDFloat::IS_NUMERIC=true;
const bool SpDFloat::IS_COMPLEX=false;
BaseGDL* SpDFloat::GetTag() const { return new SpDFloat(*this);}
DType   SpDFloat::Type()    const { return t;}
const std::string& SpDFloat::TypeStr() const { return str;}

const DType   SpDDouble::t=GDL_DOUBLE; // type ID
const string  SpDDouble::str("DOUBLE"); // type string
const DDouble SpDDouble::zero=0.0;
const bool SpDDouble::IS_INTEGER=false;
const bool SpDDouble::IS_SIGNED=true;
const bool SpDDouble::IS_NUMERIC=true;
const bool SpDDouble::IS_COMPLEX=false;
BaseGDL* SpDDouble::GetTag() const { return new SpDDouble(*this);}
DType   SpDDouble::Type()    const { return t;}
const std::string& SpDDouble::TypeStr() const { return str;}

const DType   SpDString::t=GDL_STRING; // type ID
const string  SpDString::str("STRING"); // type string
const DString SpDString::zero(""); // zero string
const bool SpDString::IS_INTEGER=false;
const bool SpDString::IS_SIGNED=false;
const bool SpDString::IS_NUMERIC=false;
const bool SpDString::IS_COMPLEX=false;
BaseGDL* SpDString::GetTag() const { return new SpDString(*this);}
DType   SpDString::Type()    const { return t;}
const std::string& SpDString::TypeStr() const { return str;}

const DType    SpDStruct::t=GDL_STRUCT;      // type ID
const string   SpDStruct::str("STRUCT"); // type string
const SpDStruct::Ty  SpDStruct::zero=0; // zero struct, special meaning
const bool SpDStruct::IS_INTEGER=false;
const bool SpDStruct::IS_SIGNED=false;
const bool SpDStruct::IS_NUMERIC=false;
const bool SpDStruct::IS_COMPLEX=false;
BaseGDL* SpDStruct::GetTag() const 
{ 
  SpDStruct* newTag = new SpDStruct(*this);
  newTag->MakeOwnDesc();
  return newTag;
}
DType   SpDStruct::Type()    const { return t;}
const std::string& SpDStruct::TypeStr() const { return str;}

const DType   SpDPtr::t=GDL_PTR;   // type ID
const string  SpDPtr::str("POINTER"); // type string
const DPtr    SpDPtr::zero=0;  // zero ptr
const bool SpDPtr::IS_INTEGER=false;
const bool SpDPtr::IS_SIGNED=false;
const bool SpDPtr::IS_NUMERIC=false;
const bool SpDPtr::IS_COMPLEX=false;
BaseGDL* SpDPtr::GetTag() const { return new SpDPtr(*this);}
DType   SpDPtr::Type()    const { return t;}
const std::string& SpDPtr::TypeStr() const { return str;}

const DType   SpDObj::t=GDL_OBJECT;   // type ID
const string  SpDObj::str("OBJREF"); // type string
const DObj    SpDObj::zero=0;  // zero ptr/obj
const bool SpDObj::IS_INTEGER=false;
const bool SpDObj::IS_SIGNED=false;
const bool SpDObj::IS_NUMERIC=false;
const bool SpDObj::IS_COMPLEX=false;
BaseGDL* SpDObj::GetTag() const { return new SpDObj(*this);}
DType   SpDObj::Type()    const { return t;}
const std::string& SpDObj::TypeStr() const { return str;}

const DType  SpDComplex::t=GDL_COMPLEX; // type ID
const string SpDComplex::str("COMPLEX"); // type string
const DComplex SpDComplex::zero(0.0,0.0);
const bool SpDComplex::IS_INTEGER=false;
const bool SpDComplex::IS_SIGNED=true;
const bool SpDComplex::IS_NUMERIC=true;
const bool SpDComplex::IS_COMPLEX=true;
BaseGDL* SpDComplex::GetTag() const { return new SpDComplex(*this);}
DType   SpDComplex::Type()    const { return t;}
const std::string& SpDComplex::TypeStr() const { return str;}

const DType  SpDComplexDbl::t=GDL_COMPLEXDBL; // type ID
const string SpDComplexDbl::str("DCOMPLEX"); // type string
const DComplexDbl SpDComplexDbl::zero(0.0,0.0);
const bool SpDComplexDbl::IS_INTEGER=false;
const bool SpDComplexDbl::IS_SIGNED=true;
const bool SpDComplexDbl::IS_NUMERIC=true;
const bool SpDComplexDbl::IS_COMPLEX=true;
BaseGDL* SpDComplexDbl::GetTag() const { return new SpDComplexDbl(*this);}
DType   SpDComplexDbl::Type()    const { return t;}
const std::string& SpDComplexDbl::TypeStr() const { return str;}

// for GDL structs
// returns an empty instance (for actually holding data)
BaseGDL* SpDByte::GetInstance() const   { return new Data_<SpDByte>(dim);}
BaseGDL* SpDInt::GetInstance() const    { return new Data_<SpDInt>(dim);}
BaseGDL* SpDUInt::GetInstance() const   { return new Data_<SpDUInt>(dim);}
BaseGDL* SpDLong::GetInstance() const   { return new Data_<SpDLong>(dim);}
BaseGDL* SpDULong::GetInstance() const  { return new Data_<SpDULong>(dim);}
BaseGDL* SpDLong64::GetInstance() const   { return new Data_<SpDLong64>(dim);}
BaseGDL* SpDULong64::GetInstance() const  { return new Data_<SpDULong64>(dim);}
BaseGDL* SpDFloat::GetInstance() const  { return new Data_<SpDFloat>(dim);}
BaseGDL* SpDDouble::GetInstance() const { return new Data_<SpDDouble>(dim);}
BaseGDL* SpDString::GetInstance() const { return new Data_<SpDString>(dim);}
BaseGDL* SpDPtr::GetInstance() const    { return new Data_<SpDPtr>(dim);}
BaseGDL* SpDObj::GetInstance() const    { return new Data_<SpDObj>(dim);}
BaseGDL* SpDStruct::GetInstance() const 
{ 
  DStructGDL* newInstance = new DStructGDL(desc,dim);
  newInstance->MakeOwnDesc();
  return newInstance;
}
BaseGDL* SpDComplex::GetInstance() const    { return new Data_<SpDComplex>(dim);}
BaseGDL* SpDComplexDbl::GetInstance() const { return new Data_<SpDComplexDbl>(dim);}

// returns an instance without allocating memory
BaseGDL* SpDByte::GetEmptyInstance() const   { return new Data_<SpDByte>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDInt::GetEmptyInstance() const    { return new Data_<SpDInt>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDUInt::GetEmptyInstance() const   { return new Data_<SpDUInt>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDLong::GetEmptyInstance() const   { return new Data_<SpDLong>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDULong::GetEmptyInstance() const  { return new Data_<SpDULong>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDLong64::GetEmptyInstance() const   { return new Data_<SpDLong64>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDULong64::GetEmptyInstance() const  { return new Data_<SpDULong64>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDFloat::GetEmptyInstance() const  { return new Data_<SpDFloat>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDDouble::GetEmptyInstance() const { return new Data_<SpDDouble>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDString::GetEmptyInstance() const { return new Data_<SpDString>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDPtr::GetEmptyInstance() const    { return new Data_<SpDPtr>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDObj::GetEmptyInstance() const    { return new Data_<SpDObj>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDStruct::GetEmptyInstance() const 
{ 
  DStructGDL* newInstance = new DStructGDL( desc, dim, BaseGDL::NOALLOC);
  newInstance->MakeOwnDesc();
  return newInstance;
}
BaseGDL* SpDComplex::GetEmptyInstance() const    { return new Data_<SpDComplex>( dim, BaseGDL::NOALLOC);}
BaseGDL* SpDComplexDbl::GetEmptyInstance() const { return new Data_<SpDComplexDbl>( dim, BaseGDL::NOALLOC);}

SpDByte::SpDByte(): BaseGDL() {}
SpDByte::SpDByte( const dimension& dim_): BaseGDL(dim_) {}
SpDByte::~SpDByte() {}

SpDInt::SpDInt(): BaseGDL() {}
SpDInt::SpDInt( const dimension& dim_): BaseGDL(dim_) {}
SpDInt::~SpDInt() {}

SpDUInt::SpDUInt(): BaseGDL() {}
SpDUInt::SpDUInt( const dimension& dim_): BaseGDL(dim_) {}
SpDUInt::~SpDUInt() {};

SpDLong::SpDLong(): BaseGDL() {}
SpDLong::SpDLong( const dimension& dim_): BaseGDL(dim_) {}
SpDLong::~SpDLong() {}

SpDULong::SpDULong(): BaseGDL() {}
SpDULong::SpDULong( const dimension& dim_): BaseGDL(dim_) {}
SpDULong::~SpDULong() {}

SpDLong64::SpDLong64(): BaseGDL() {}
SpDLong64::SpDLong64( const dimension& dim_): BaseGDL(dim_) {}
SpDLong64::~SpDLong64() {}

SpDULong64::SpDULong64(): BaseGDL() {}
SpDULong64::SpDULong64( const dimension& dim_): BaseGDL(dim_) {}
SpDULong64::~SpDULong64() {}

SpDFloat::SpDFloat(): BaseGDL() {}
SpDFloat::SpDFloat( const dimension& dim_): BaseGDL(dim_) {}
SpDFloat::~SpDFloat() {}

SpDDouble::SpDDouble(): BaseGDL() {}
SpDDouble::SpDDouble( const dimension& dim_): BaseGDL(dim_) {}
SpDDouble::~SpDDouble() {}

SpDString::SpDString(): BaseGDL() {}
SpDString::SpDString( const dimension& dim_): BaseGDL(dim_) {}
SpDString::~SpDString() {}

SpDStruct::SpDStruct( DStructDesc* desc_): 
  BaseGDL(),
  desc(desc_) 
{
  //  if( desc != NULL && desc->IsUnnamed()) 
  //    desc = new DStructDesc( desc);
}

SpDStruct::SpDStruct( DStructDesc* desc_, const dimension& dim_): 
  BaseGDL(dim_),
  desc(desc_) 
{
  //  if( desc == NULL) cout << "SpDStruct::SpDStruct( DStructDesc* desc_, const dimension& dim_): desc_ == NULL" << endl;
  //  if( desc_ != NULL && desc_->IsUnnamed()) 
  //    desc = new DStructDesc( desc_);
}

SpDStruct::~SpDStruct() 
{
  if( desc != NULL && desc->IsUnnamed()) desc->Delete();
}

SpDPtr::SpDPtr(): BaseGDL() {}
SpDPtr::SpDPtr( const dimension& dim_): BaseGDL(dim_) {}
SpDPtr::~SpDPtr() {}

SpDObj::SpDObj(): BaseGDL() {}
SpDObj::SpDObj( const dimension& dim_): BaseGDL(dim_) {}
SpDObj::~SpDObj() {}

SpDComplex::SpDComplex(): BaseGDL() {}
SpDComplex::SpDComplex( const dimension& dim_): BaseGDL(dim_) {}
SpDComplex::~SpDComplex() {}

SpDComplexDbl::SpDComplexDbl(): BaseGDL() {}
SpDComplexDbl::SpDComplexDbl( const dimension& dim_): BaseGDL(dim_) {}
SpDComplexDbl::~SpDComplexDbl() {}

