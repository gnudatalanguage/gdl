/***************************************************************************
                          typetraits.cpp  -  type parameterization
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

#include "typetraits.hpp"
#include "datatypes.hpp"
#include "dstructgdl.hpp"

using namespace std;

const DType  SpDByte::t=BYTE; // type ID
const string SpDByte::str("BYTE"); // type string
const DByte  SpDByte::zero=0;
BaseGDL* SpDByte::GetTag() const { return new SpDByte(*this);}
const DType   SpDByte::Type()    const { return t;}
const std::string& SpDByte::TypeStr() const { return str;}

const DType  SpDInt::t=INT; // type ID
const string SpDInt::str("INT"); // type string
const DInt   SpDInt::zero=0;
BaseGDL* SpDInt::GetTag() const { return new SpDInt(*this);}
const DType   SpDInt::Type()    const { return t;}
const std::string& SpDInt::TypeStr() const { return str;}

const DType  SpDUInt::t=UINT; // type ID
const string SpDUInt::str("UINT"); // type string
const DUInt  SpDUInt::zero=0;
BaseGDL* SpDUInt::GetTag() const { return new SpDUInt(*this);}
const DType   SpDUInt::Type()    const { return t;}
const std::string& SpDUInt::TypeStr() const { return str;}


const DType  SpDLong::t=LONG; // type ID
const string SpDLong::str("LONG"); // type string
const DLong  SpDLong::zero=0;
BaseGDL* SpDLong::GetTag() const { return new SpDLong(*this);}
const DType   SpDLong::Type()    const { return t;}
const std::string& SpDLong::TypeStr() const { return str;}

const DType  SpDULong::t=ULONG; // type ID
const string SpDULong::str("ULONG"); // type string
const DULong SpDULong::zero=0;
BaseGDL* SpDULong::GetTag() const { return new SpDULong(*this);}
const DType   SpDULong::Type()    const { return t;}
const std::string& SpDULong::TypeStr() const { return str;}

const DType  SpDLong64::t=LONG64; // type ID
const string SpDLong64::str("LONG64"); // type string
const DLong64  SpDLong64::zero=0;
BaseGDL* SpDLong64::GetTag() const { return new SpDLong64(*this);}
const DType   SpDLong64::Type()    const { return t;}
const std::string& SpDLong64::TypeStr() const { return str;}

const DType  SpDULong64::t=ULONG64; // type ID
const string SpDULong64::str("ULONG64"); // type string
const DULong64 SpDULong64::zero=0;
BaseGDL* SpDULong64::GetTag() const { return new SpDULong64(*this);}
const DType   SpDULong64::Type()    const { return t;}
const std::string& SpDULong64::TypeStr() const { return str;}

const DType  SpDFloat::t=FLOAT; // type ID
const string SpDFloat::str("FLOAT"); // type string
const DFloat SpDFloat::zero=0.0;
BaseGDL* SpDFloat::GetTag() const { return new SpDFloat(*this);}
const DType   SpDFloat::Type()    const { return t;}
const std::string& SpDFloat::TypeStr() const { return str;}

const DType   SpDDouble::t=DOUBLE; // type ID
const string  SpDDouble::str("DOUBLE"); // type string
const DDouble SpDDouble::zero=0.0;
BaseGDL* SpDDouble::GetTag() const { return new SpDDouble(*this);}
const DType   SpDDouble::Type()    const { return t;}
const std::string& SpDDouble::TypeStr() const { return str;}

const DType   SpDString::t=STRING; // type ID
const string  SpDString::str("STRING"); // type string
const DString SpDString::zero(""); // zero string
BaseGDL* SpDString::GetTag() const { return new SpDString(*this);}
const DType   SpDString::Type()    const { return t;}
const std::string& SpDString::TypeStr() const { return str;}

const DType    SpDStruct::t=STRUCT;      // type ID
const string   SpDStruct::str("STRUCT"); // type string
const SpDStruct::Ty  SpDStruct::zero=NULL; // zero struct, special meaning
BaseGDL* SpDStruct::GetTag() const 
{ 
  SpDStruct* newTag = new SpDStruct(*this);
  newTag->MakeOwnDesc();
  return newTag;
}
const DType   SpDStruct::Type()    const { return t;}
const std::string& SpDStruct::TypeStr() const { return str;}

const DType   SpDPtr::t=PTR;   // type ID
const string  SpDPtr::str("PTR"); // type string
const DPtr    SpDPtr::zero=0;  // zero ptr
BaseGDL* SpDPtr::GetTag() const { return new SpDPtr(*this);}
const DType   SpDPtr::Type()    const { return t;}
const std::string& SpDPtr::TypeStr() const { return str;}

const DType   SpDObj::t=OBJECT;   // type ID
const string  SpDObj::str("OBJECT"); // type string
const DObj    SpDObj::zero=0;  // zero ptr/obj
BaseGDL* SpDObj::GetTag() const { return new SpDObj(*this);}
const DType   SpDObj::Type()    const { return t;}
const std::string& SpDObj::TypeStr() const { return str;}

const DType  SpDComplex::t=COMPLEX; // type ID
const string SpDComplex::str("COMPLEX"); // type string
const DComplex SpDComplex::zero(0.0,0.0);
BaseGDL* SpDComplex::GetTag() const { return new SpDComplex(*this);}
const DType   SpDComplex::Type()    const { return t;}
const std::string& SpDComplex::TypeStr() const { return str;}

const DType  SpDComplexDbl::t=COMPLEXDBL; // type ID
const string SpDComplexDbl::str("DCOMPLEX"); // type string
const DComplexDbl SpDComplexDbl::zero(0.0,0.0);
BaseGDL* SpDComplexDbl::GetTag() const { return new SpDComplexDbl(*this);}
const DType   SpDComplexDbl::Type()    const { return t;}
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
  SpDStruct* newInstance = new DStructGDL(desc,dim);
  newInstance->MakeOwnDesc();
  return newInstance;
}
BaseGDL* SpDComplex::GetInstance() const    { return new Data_<SpDComplex>(dim);}
BaseGDL* SpDComplexDbl::GetInstance() const { return new Data_<SpDComplexDbl>(dim);}

SpDByte::SpDByte(): BaseGDL() {}
SpDByte::SpDByte( const dimension& dim_): BaseGDL(dim_) {}
SpDByte::~SpDByte() {};

SpDInt::SpDInt(): BaseGDL() {}
SpDInt::SpDInt( const dimension& dim_): BaseGDL(dim_) {}
SpDInt::~SpDInt() {};

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
  if( desc != NULL && desc->IsUnnamed()) delete desc;
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

