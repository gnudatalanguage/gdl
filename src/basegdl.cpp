/***************************************************************************
                          basegdl.cpp  -  base class for GDL datatypes
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "basegdl.hpp"

using namespace std;

// get the vtable for BaseGDL
BaseGDL::~BaseGDL() {}

BaseGDL::BaseGDL(): dim() {} 
BaseGDL::BaseGDL(const dimension& dim_): dim(dim_) {} 

bool BaseGDL::IsAssoc() const { return false;}
  
BaseGDL* BaseGDL::AssocVar( int, SizeT)
{
  throw GDLException("BaseGDL::AssocVar(...) called.");
}

const SizeT BaseGDL::N_Elements() const { return dim.N_Elements();}
const SizeT BaseGDL::Size() const { return 0;}
const SizeT BaseGDL::NBytes() const { return 0;} // for assoc function
const SizeT BaseGDL::ToTransfer() const { return 0;}
const SizeT BaseGDL::Sizeof() const { return 0;}

bool BaseGDL::Greater(SizeT i1, SizeT i2) const
{
  throw GDLException("BaseGDL::Greater(SizeT,SizeT) called.");
}
bool BaseGDL::Equal(SizeT i1, SizeT i2) const
{
  throw GDLException("BaseGDL::Equal(SizeT,SizeT) called.");
}

BaseGDL* BaseGDL::CShift( DLong d)
{
  throw GDLException("BaseGDL::CShift(...) called.");
}
BaseGDL* BaseGDL::CShift( DLong d[MAXRANK])
{
  throw GDLException("BaseGDL::CShift( DLong d[MAXRANK]) called.");
}

  
BaseGDL* BaseGDL::Transpose( DUInt* perm)
{
  throw GDLException("BaseGDL::Transpose(...) called.");
}

void BaseGDL::MinMax( DLong* minE, DLong* maxE, 
		      BaseGDL** minVal, BaseGDL** maxVal)
{
  throw GDLException("BaseGDL::MinMax(...) called.");
}

void BaseGDL::Clear() 
{
  throw GDLException("BaseGDL::Clear() called.");
}

void BaseGDL::Assign( BaseGDL* src, SizeT nEl)
{
  throw GDLException("BaseGDL::Assign(...) called.");
}

std::ostream& BaseGDL::Write( std::ostream& os, bool swapEndian)
{
  throw GDLException("BaseGDL::Write(...) called.");
}
std::istream& BaseGDL::Read( std::istream& os, bool swapEndian)
{
  throw GDLException("BaseGDL::Read(...) called.");
}

std::ostream& BaseGDL::ToStream(std::ostream& o, SizeT width, 
				SizeT* actPosPtr )
{
  throw GDLException("BaseGDL::ToStream(...) called.");
}
std::istream& BaseGDL::FromStream(std::istream& i)
{
  i >> *this;
  return i;
}

bool BaseGDL::Scalar() const { return false;}
const DType   BaseGDL::Type() const { return UNDEF;}
const std::string& BaseGDL::TypeStr() const
{ static const std::string s("UNDEF"); return s;}
bool BaseGDL::EqType( const BaseGDL*) const
{
  throw GDLException("BaseGDL::EqType(...) called.");
}

void* BaseGDL::DataAddr( SizeT elem)
{
  throw GDLException("BaseGDL::DataAddr(...) called.");
}
  
// make same type on the heap
BaseGDL* BaseGDL::New( const dimension& dim_, InitType noZero)
{
  throw GDLException("BaseGDL::New(...) called.");
}

BaseGDL* BaseGDL::Dup() 
{ 
  throw GDLException("BaseGDL::Dup(...) called.");
}

BaseGDL* BaseGDL::Convert2( DType destTy, Convert2Mode mode)
{
  throw GDLException("BaseGDL::Convert2(...) called.");
}

BaseGDL* BaseGDL::GetTag() const 
{
  throw GDLException("BaseGDL::GetTag(...) called.");
}

BaseGDL* BaseGDL::GetInstance() const
{
  throw GDLException("BaseGDL::GetInstance(...) called.");
}

int BaseGDL::Scalar2index(SizeT& ret) const 
{ 
  throw GDLException("Operation not defined for UNDEF 1.");
}

bool BaseGDL::BaseGDL::True()
{
  throw GDLException("Operation not defined for UNDEF 3.");
}

bool BaseGDL::BaseGDL::False()
{
  throw GDLException("Operation not defined for UNDEF 4.");
}

bool BaseGDL::LogTrue()
{
  throw GDLException("Operation not defined for UNDEF 4a.");
}

bool BaseGDL::LogTrue( SizeT ix)
{
  throw GDLException("Operation not defined for UNDEF 4b.");
}

BaseGDL* BaseGDL::LogNeg()
{
  throw GDLException("Operation not defined for UNDEF 4c.");
}

int BaseGDL::Sgn() // -1,0,1
{
  throw GDLException("Operation not defined for UNDEF 5.");
}

bool BaseGDL::Equal( BaseGDL*)
{
  throw GDLException("Operation not defined for UNDEF 6.");
}

bool BaseGDL::ArrayEqual( BaseGDL*)
{
  throw GDLException("Operation not defined for UNDEF 6a.");
}

// for statement compliance (int types , float types scalar only)
void BaseGDL::ForCheck( BaseGDL**, BaseGDL**)
{
  throw GDLException("Operation not defined for UNDEF 7.");
}

bool BaseGDL::ForCondUp( BaseGDL*)
{
  throw GDLException("Operation not defined for UNDEF 8.");
}

bool BaseGDL::ForCondDown( BaseGDL*)
{
  throw GDLException("Operation not defined for UNDEF 9.");
}

void BaseGDL::ForAdd( BaseGDL* add)
{
  throw GDLException("Operation not defined for UNDEF 10.");
}

BaseGDL* BaseGDL::CatArray( ExprListT& exprList,
			    const SizeT catRank, 
			    const SizeT rank) 
{
  throw GDLException("BaseGDL::CatArray(...) called.");
}

BaseGDL* BaseGDL::Index( ArrayIndexListT* ixList) 
{
  throw GDLException("BaseGDL::Index(...) called.");
}

// used in r_expr
BaseGDL* BaseGDL::UMinus()              
{
  throw GDLException("Operation not defined for UNDEF 11.");
}

BaseGDL* BaseGDL::NotOp()               
{
  throw GDLException("Operation not defined for UNDEF 12.");
}

BaseGDL* BaseGDL::AndOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::AndOpInv( BaseGDL* r) 
{
  throw GDLException("Operation not defined for UNDEF 14.");
}

BaseGDL* BaseGDL::OrOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::OrOpInv( BaseGDL* r) 
{
  throw GDLException("Operation not defined for UNDEF 14.");
}

BaseGDL* BaseGDL::XorOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::EqOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::NeOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::LeOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::GeOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::LtOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::GtOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::Add( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 15.");
}
BaseGDL* BaseGDL::AddInv( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 15.");
}

BaseGDL* BaseGDL::Sub( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::SubInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::LtMark( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 18.");
}

BaseGDL* BaseGDL::GtMark( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 19.");
}

BaseGDL* BaseGDL::Mult( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 19.");
}

BaseGDL* BaseGDL::Div( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::DivInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::Mod( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::ModInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::Pow( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::PowInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::MatrixOp( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 18.");
}

void BaseGDL::AssignAt( BaseGDL* srcIn, 
			ArrayIndexListT* ixList, SizeT offset)
{
  throw GDLException("BaseGDL::AssignAt(...) called.");
}

void BaseGDL::DecAt( ArrayIndexListT* ixList)
{
  throw GDLException("BaseGDL::DecAt(...) called.");
}
void BaseGDL::IncAt( ArrayIndexListT* ixList)
{
  throw GDLException("BaseGDL::IncAt(...) called.");
}
void BaseGDL::Dec()
{
  throw GDLException("BaseGDL::Dec(...) called.");
}
void BaseGDL::Inc()
{
  throw GDLException("BaseGDL::Inc(...) called.");
}

void BaseGDL::InsertAt(  SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList)
{
  throw GDLException("BaseGDL::InsertAt( SizeT, ...) called.");
}

// formatting output functions
SizeT BaseGDL::OFmtA( std::ostream* os, SizeT offset, SizeT num, int width)
{throw GDLException("BaseGDL::OFmtA(...) called.");}

SizeT BaseGDL::OFmtF( std::ostream* os, SizeT offs, SizeT num, int width,
		      int prec, IOMode oM) 
{throw GDLException("BaseGDL::OFmtF(...) called.");}

SizeT BaseGDL::OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
		      int minN, BaseGDL::IOMode oM)
{throw GDLException("BaseGDL::OFmtI(...) called.");}

SizeT BaseGDL::IFmtA( std::istream* is, SizeT offset, SizeT num, int width)
{throw GDLException("BaseGDL::IFmtA(...) called.");}

SizeT BaseGDL::IFmtF( std::istream* is, SizeT offs, SizeT num, int width)
{throw GDLException("BaseGDL::IFmtF(...) called.");}

SizeT BaseGDL::IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
		      BaseGDL::IOMode oM)
{throw GDLException("BaseGDL::IFmtI(...) called.");}

BaseGDL* BaseGDL::Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
		 bool center, int edgeMode)
{
  throw GDLException("BaseGDL::Convol(...) called.");
}
BaseGDL* BaseGDL::Rebin( const dimension& newDim, bool sample)
{
  throw GDLException("BaseGDL::Rebin(...) called.");
}

#ifdef USE_PYTHON
PyObject* BaseGDL::ToPython()
{throw GDLException("BaseGDL::ToPython(...) called.");}
#endif

