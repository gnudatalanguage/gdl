/***************************************************************************
                          basegdl.cpp  -  base class for GDL datatypes
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

#include "includefirst.hpp"

#include "basegdl.hpp"

using namespace std;

DInterpreter* BaseGDL::interpreter = NULL;

// get the vtable for BaseGDL
BaseGDL::~BaseGDL() {}

BaseGDL::BaseGDL(): dim() {} 
BaseGDL::BaseGDL(const dimension& dim_): dim(dim_) {} 

bool BaseGDL::IsAssoc() const { return false;}
  
BaseGDL* BaseGDL::AssocVar( int, SizeT)
{
  throw GDLException("BaseGDL::AssocVar(...) called.");
}

SizeT BaseGDL::N_Elements() const { return dim.N_Elements();}
SizeT BaseGDL::Size() const { return 0;}
SizeT BaseGDL::ToTransfer() const { return 0;}
SizeT BaseGDL::Sizeof() const { return 0;}
SizeT BaseGDL::NBytes() const { return 0;} // for assoc function

BaseGDL& BaseGDL::operator=(const BaseGDL& right)
{
  if( &right == this) return *this;
  this->dim = right.dim;
  return *this;
}


// BaseGDL* BaseGDL::Abs() const
// {
//   throw GDLException("BaseGDL::Abs() called.");
// }

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
		      BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
		      SizeT start, SizeT stop, SizeT step, DLong valIx)
{
  throw GDLException("BaseGDL::MinMax(...) called.");
}

void BaseGDL::Clear() 
{
  throw GDLException("BaseGDL::Clear() called.");
}
void BaseGDL::Construct() 
{
  throw GDLException("BaseGDL::Construct() called.");
}
void BaseGDL::ConstructTo0() 
{
  throw GDLException("BaseGDL::ConstructTo0() called.");
}
void BaseGDL::Destruct() 
{
  throw GDLException("BaseGDL::Destruct() called.");
}

void BaseGDL::Assign( BaseGDL* src, SizeT nEl)
{
  throw GDLException("BaseGDL::Assign(...) called.");
}

std::ostream& BaseGDL::Write( std::ostream& os, bool swapEndian, 
			      bool compress, XDR *xdrs)
{
  throw GDLException("BaseGDL::Write(...) called.");
}
std::istream& BaseGDL::Read( std::istream& os, bool swapEndian, 
			     bool compress, XDR *xdrs)
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
bool BaseGDL::StrictScalar() const { return false;}
DType   BaseGDL::Type() const { return UNDEF;}
const std::string& BaseGDL::TypeStr() const
{ static const std::string s("UNDEFINED"); return s;}
bool BaseGDL::EqType( const BaseGDL*) const
{
  throw GDLException("BaseGDL::EqType(...) called.");
}

void* BaseGDL::DataAddr()// SizeT elem)
{
  throw GDLException("BaseGDL::DataAddr(...) called.");
}
  
// make same type on the heap
BaseGDL* BaseGDL::New( const dimension& dim_, InitType noZero)
{
  throw GDLException("BaseGDL::New(...) called.");
}

BaseGDL* BaseGDL::Dup() const
{ 
  throw GDLException("BaseGDL::Dup() called.");
}
// BaseGDL* BaseGDL::Dup( void*) const 
// { 
//   throw GDLException("BaseGDL::Dup(...) called.");
// }

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
BaseGDL* BaseGDL::GetEmptyInstance() const
{
  throw GDLException("BaseGDL::GetEmptyInstance(...) called.");
}
BaseGDL* BaseGDL::SetBuffer( const void* b)
{
  throw GDLException("BaseGDL::SetBuffer called.");
}
void BaseGDL::SetBufferSize( SizeT s)
{
  throw GDLException("BaseGDL::SetBufferSize called.");
}

int BaseGDL::Scalar2index(SizeT& ret) const 
{ 
  throw GDLException("Operation not defined for UNDEF 1.");
}
int BaseGDL::Scalar2RangeT(RangeT& ret) const
{ 
  throw GDLException("Operation not defined for UNDEF 1a.");
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

DLong* BaseGDL::Where( bool, SizeT&)
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

bool BaseGDL::Equal( BaseGDL*) const
{
  throw GDLException("Operation not defined for UNDEF 6.");
}

bool BaseGDL::EqualNoDelete( const BaseGDL*) const
{
  throw GDLException("Operation not defined for UNDEF 6a.");
}

bool BaseGDL::ArrayEqual( BaseGDL*)
{
  throw GDLException("Operation not defined for UNDEF 6b.");
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

bool BaseGDL::ForAddCondUp( BaseGDL* loopInfo)
// bool BaseGDL::ForAddCondUp( ForLoopInfoT& loopInfo)
{
  throw GDLException("Operation not defined for UNDEF 8a.");
}

bool BaseGDL::ForCondDown( BaseGDL*)
{
  throw GDLException("Operation not defined for UNDEF 9.");
}

// bool BaseGDL::ForCondUpDown( BaseGDL*)
// {
//   throw GDLException("Operation not defined for UNDEF 9a.");
// }

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

BaseGDL* BaseGDL::XorOpS( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13a.");
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
BaseGDL* BaseGDL::PowInt( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 170.");
}
BaseGDL* BaseGDL::PowIntNew( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 171.");
}

BaseGDL* BaseGDL::MatrixOp( BaseGDL* r, bool rtranspose, bool transposeResult, bool strassen)
{
  throw GDLException("Operation not defined for UNDEF 18.");
}

void BaseGDL::AssignAt( BaseGDL* srcIn,	ArrayIndexListT* ixList, SizeT offset)
{
  throw GDLException("BaseGDL::AssignAt(...) called.");
}
void BaseGDL::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList)
{
  throw GDLException("BaseGDL::AssignAt(..) called.");
}
void BaseGDL::AssignAt( BaseGDL* srcIn)
{
  throw GDLException("BaseGDL::AssignAt(.) called.");
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



BaseGDL* BaseGDL::AndOpNew( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

// BaseGDL* BaseGDL::AndOpInvNew( BaseGDL* r) 
// {
//   throw GDLException("Operation not defined for UNDEF 14.");
// }

BaseGDL* BaseGDL::OrOpNew( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

// BaseGDL* BaseGDL::OrOpInvNew( BaseGDL* r) 
// {
//   throw GDLException("Operation not defined for UNDEF 14.");
// }

BaseGDL* BaseGDL::XorOpNew( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}
BaseGDL* BaseGDL::AddNew( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 15.");
}
// BaseGDL* BaseGDL::AddInvNew( BaseGDL* r)      
// {
//   throw GDLException("Operation not defined for UNDEF 15.");
// }

BaseGDL* BaseGDL::SubNew( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
// BaseGDL* BaseGDL::SubInvNew( BaseGDL* r)   
// {
//   throw GDLException("Operation not defined for UNDEF 17.");
// }
BaseGDL* BaseGDL::MultNew( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 19.");
}

BaseGDL* BaseGDL::DivNew( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
// BaseGDL* BaseGDL::DivInvNew( BaseGDL* r)   
// {
//   throw GDLException("Operation not defined for UNDEF 17.");
// }

BaseGDL* BaseGDL::ModNew( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
// BaseGDL* BaseGDL::ModInvNew( BaseGDL* r)   
// {
//   throw GDLException("Operation not defined for UNDEF 17.");
// }

BaseGDL* BaseGDL::PowNew( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
// BaseGDL* BaseGDL::PowInvNew( BaseGDL* r)   
// {
//   throw GDLException("Operation not defined for UNDEF 17.");
// }




BaseGDL* BaseGDL::AndOpS( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::AndOpInvS( BaseGDL* r) 
{
  throw GDLException("Operation not defined for UNDEF 14.");
}

BaseGDL* BaseGDL::OrOpS( BaseGDL* r)    
{
  throw GDLException("Operation not defined for UNDEF 13.");
}

BaseGDL* BaseGDL::OrOpInvS( BaseGDL* r) 
{
  throw GDLException("Operation not defined for UNDEF 14.");
}

BaseGDL* BaseGDL::AddS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 15.");
}
BaseGDL* BaseGDL::AddInvS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 15.");
}

BaseGDL* BaseGDL::SubS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::SubInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::LtMarkS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 18.");
}

BaseGDL* BaseGDL::GtMarkS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 19.");
}

BaseGDL* BaseGDL::MultS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 19.");
}

BaseGDL* BaseGDL::DivS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::DivInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::ModS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::ModInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::PowS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for UNDEF 16.");
}
BaseGDL* BaseGDL::PowInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for UNDEF 17.");
}

BaseGDL* BaseGDL::NewIx( SizeT ix)
{
  throw GDLException("Operation not defined for UNDEF 20.");
}
BaseGDL* BaseGDL::NewIx( BaseGDL* ix, bool strict)
{
  throw GDLException("Operation not defined for UNDEF 21.");
}
BaseGDL* BaseGDL::NewIx( AllIxT* ix, dimension* dIn)
{
  throw GDLException("Operation not defined for UNDEF 22.");
}
BaseGDL* BaseGDL::NewIxFrom( SizeT s)
{
  throw GDLException("Operation not defined for UNDEF 23.");
}
BaseGDL* BaseGDL::NewIxFrom( SizeT s, SizeT e)
{
  throw GDLException("Operation not defined for UNDEF 24.");
}
BaseGDL* BaseGDL::NewIxFromStride( SizeT s, SizeT stride)
{
  throw GDLException("Operation not defined for UNDEF 25.");
}
BaseGDL* BaseGDL::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{
  throw GDLException("Operation not defined for UNDEF 26.");
}
BaseGDL* BaseGDL::Log()              
{ 
  throw GDLException("Operation not defined for UNDEF 27a.");
}
void BaseGDL::LogThis()              
{ 
  throw GDLException("Operation not defined for UNDEF 27b.");
}
BaseGDL* BaseGDL::Log10()              
{ 
  throw GDLException("Operation not defined for UNDEF 27c.");
}
void BaseGDL::Log10This()              
{ 
  throw GDLException("Operation not defined for UNDEF 27d.");
}

void BaseGDL::AssignAtIx( RangeT ix, BaseGDL* srcIn)
{ 
  throw GDLException("Operation not defined for UNDEF 28.");
}

SizeT BaseGDL::LoopIndex() const
{ 
  throw GDLException("Operation not defined for UNDEF 29.");
}

BaseGDL* BaseGDL::Rotate( DLong dir)
{ 
  throw GDLException("Operation not defined for UNDEF 30.");
}

void BaseGDL::Reverse( DLong dim)
{ 
  throw GDLException("Operation not defined for UNDEF 31.");
}

BaseGDL* BaseGDL::DupReverse( DLong dim)
{ 
  throw GDLException("Operation not defined for UNDEF 32.");
}

// --- SA: MemStats stuff

SizeT MemStats::NumAlloc = 0;
SizeT MemStats::NumFree = 0;
SizeT MemStats::HighWater = 0;
SizeT MemStats::Current = 0;

#if !defined(HAVE_MALLINFO) 
#  if (!defined(HAVE_MALLOC_ZONE_STATISTICS) || !defined(HAVE_MALLOC_MALLOC_H))
#    if defined(HAVE_SBRK)
char* MemStats::StartOfMemory = reinterpret_cast<char*>(::sbrk(0));
#    endif
#  endif
#endif

// ---
