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

#include "nullgdl.hpp"

#include "datatypes.hpp"

using namespace std;

char NullGDL::buf[sizeof(NullGDL)];

NullGDL* NullGDL::instance = NULL;

NullGDL::~NullGDL()
{
  //throw GDLException("Attempt to destroy !NULL."); 
  cerr << "Internal error: !NULL destructor called. Please report at http://sourceforge.net/tracker/?group_id=97659&atid=618683" << endl;
}

bool NullGDL::IsAssoc() const { return false;}
  
BaseGDL* NullGDL::AssocVar( int, SizeT)
{
  throw GDLException("NullGDL::AssocVar(...) called.");
}

SizeT NullGDL::N_Elements() const 
{
  return 0;
}
SizeT NullGDL::Size() const { return 0;}
SizeT NullGDL::ToTransfer() const { return 0;}
SizeT NullGDL::Sizeof() const { return 0;}
SizeT NullGDL::NBytes() const { return 0;} // for assoc function

BaseGDL& NullGDL::operator=(const BaseGDL& right)
{
  return *this;
}

void  NullGDL::InitFrom(const BaseGDL& right)
{
  throw GDLException("NullGDL::InitFrom(const BaseGDL& right) called.");
}


// BaseGDL* NullGDL::Abs() const
// {
//   throw GDLException("NullGDL::Abs() called.");
// }

bool NullGDL::Greater(SizeT i1, SizeT i2) const
{
  throw GDLException("NullGDL::Greater(SizeT,SizeT) called.");
}
bool NullGDL::Equal(SizeT i1, SizeT i2) const
{
  throw GDLException("NullGDL::Equal(SizeT,SizeT) called.");
}

BaseGDL* NullGDL::CShift( DLong d) const
{
  throw GDLException("NullGDL::CShift(...) called.");
}
BaseGDL* NullGDL::CShift( DLong d[MAXRANK]) const
{
  throw GDLException("NullGDL::CShift( DLong d[MAXRANK]) called.");
}

  
BaseGDL* NullGDL::Transpose( DUInt* perm)
{
  throw GDLException("NullGDL::Transpose(...) called.");
}

void NullGDL::MinMax( DLong* minE, DLong* maxE, 
		      BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
		      SizeT start, SizeT stop, SizeT step, DLong valIx)
{
  throw GDLException("NullGDL::MinMax(...) called.");
}

void NullGDL::Clear() 
{
  throw GDLException("NullGDL::Clear() called.");
}
void NullGDL::Construct() 
{
  throw GDLException("NullGDL::Construct() called.");
}
void NullGDL::ConstructTo0() 
{
  throw GDLException("NullGDL::ConstructTo0() called.");
}
void NullGDL::Destruct() 
{
  throw GDLException("NullGDL::Destruct() called.");
}

void NullGDL::Assign( BaseGDL* src, SizeT nEl)
{
  throw GDLException("NullGDL::Assign(...) called.");
}

std::ostream& NullGDL::Write( std::ostream& os, bool swapEndian, 
			      bool compress, XDR *xdrs)
{
  throw GDLException("NullGDL::Write(...) called.");
}
std::istream& NullGDL::Read( std::istream& os, bool swapEndian, 
			     bool compress, XDR *xdrs)
{
  throw GDLException("NullGDL::Read(...) called.");
}

std::ostream& NullGDL::ToStream(std::ostream& o, SizeT width, 
				SizeT* actPosPtr )
{
  o << "!NULL";
  return o;
}
std::istream& NullGDL::FromStream(std::istream& i)
{
  i >> *this;
  return i;
}

bool NullGDL::Scalar() const { return false;}
bool NullGDL::StrictScalar() const { return false;}
DType   NullGDL::Type() const { return UNDEF;}
const std::string& NullGDL::TypeStr() const
{ static const std::string s("UNDEFINED"); return s;}
bool NullGDL::EqType( const BaseGDL*) const
{
  throw GDLException("NullGDL::EqType(...) called.");
}

void* NullGDL::DataAddr()// SizeT elem)
{
  throw GDLException("NullGDL::DataAddr(...) called.");
}
  
// make same type on the heap
BaseGDL* NullGDL::New( const dimension& dim_, InitType noZero) const
{
  return NullGDL::GetSingleInstance();
}
BaseGDL* NullGDL::NewResult() const
{
  return NullGDL::GetSingleInstance();
}

BaseGDL* NullGDL::Dup() const
{ 
  return NullGDL::GetSingleInstance();
}
// BaseGDL* NullGDL::Dup( void*) const 
// { 
//   throw GDLException("NullGDL::Dup(...) called.");
// }

BaseGDL* NullGDL::Convert2( DType destTy, Convert2Mode mode)
{
  if( destTy == STRING)
    return new DStringGDL( "!NULL");
  throw GDLException("Variable is undefined: !NULL");  
}

BaseGDL* NullGDL::GetTag() const 
{
  throw GDLException("NullGDL::GetTag(...) called.");
}

BaseGDL* NullGDL::GetInstance() const
{
  throw GDLException("NullGDL::GetInstance(...) called.");
}
BaseGDL* NullGDL::GetEmptyInstance() const
{
  throw GDLException("NullGDL::GetEmptyInstance(...) called.");
}
BaseGDL* NullGDL::SetBuffer( const void* b)
{
  throw GDLException("NullGDL::SetBuffer called.");
}
void NullGDL::SetBufferSize( SizeT s)
{
  throw GDLException("NullGDL::SetBufferSize called.");
}

int NullGDL::Scalar2index(SizeT& ret) const 
{ 
  throw GDLException("Operation not defined for !NULL 1.");
}
int NullGDL::Scalar2RangeT(RangeT& ret) const
{ 
  throw GDLException("Operation not defined for !NULL 1a.");
}

SizeT NullGDL::GetAsIndex( SizeT i) const
{ 
  throw GDLException("NullGDL::GetAsIndex called.");
}
SizeT NullGDL::GetAsIndexStrict( SizeT i) const
{ 
  throw GDLException("NullGDL::GetAsIndexStrict called.");
}

bool NullGDL::NullGDL::True()
{
  throw GDLException("Operation not defined for !NULL 3.");
}

bool NullGDL::NullGDL::False()
{
  throw GDLException("Operation not defined for !NULL 4.");
}

bool NullGDL::LogTrue()
{
  throw GDLException("Operation not defined for !NULL 4a.");
}

bool NullGDL::LogTrue( SizeT ix)
{
  throw GDLException("Operation not defined for !NULL 4b.");
}

DLong* NullGDL::Where( bool, SizeT&)
{
  throw GDLException("Operation not defined for !NULL 4b.");
}

BaseGDL* NullGDL::LogNeg()
{
  throw GDLException("Operation not defined for !NULL 4c.");
}

int NullGDL::Sgn() // -1,0,1
{
  throw GDLException("Operation not defined for !NULL 5.");
}

bool NullGDL::Equal( BaseGDL*) const
{
  throw GDLException("Operation not defined for !NULL 6.");
}

bool NullGDL::EqualNoDelete( const BaseGDL*) const
{
  throw GDLException("Operation not defined for !NULL 6a.");
}

bool NullGDL::ArrayEqual( BaseGDL*)
{
  throw GDLException("Operation not defined for !NULL 6b.");
}

// for statement compliance (int types , float types scalar only)
void NullGDL::ForCheck( BaseGDL**, BaseGDL**)
{
  throw GDLException("Operation not defined for !NULL 7.");
}

bool NullGDL::ForCondUp( BaseGDL*)
{
  throw GDLException("Operation not defined for !NULL 8.");
}

bool NullGDL::ForAddCondUp( BaseGDL* loopInfo)
// bool NullGDL::ForAddCondUp( ForLoopInfoT& loopInfo)
{
  throw GDLException("Operation not defined for !NULL 8a.");
}

bool NullGDL::ForCondDown( BaseGDL*)
{
  throw GDLException("Operation not defined for !NULL 9.");
}

// bool NullGDL::ForCondUpDown( BaseGDL*)
// {
//   throw GDLException("Operation not defined for !NULL 9a.");
// }

void NullGDL::ForAdd( BaseGDL* add)
{
  throw GDLException("Operation not defined for !NULL 10.");
}

BaseGDL* NullGDL::CatArray( ExprListT& exprList,
			    const SizeT catRank, 
			    const SizeT rank) 
{
  throw GDLException("NullGDL::CatArray(...) called.");
}

BaseGDL* NullGDL::Index( ArrayIndexListT* ixList) 
{
  throw GDLException("NullGDL::Index(...) called.");
}

// used in r_expr
BaseGDL* NullGDL::UMinus()              
{
  throw GDLException("Operation not defined for !NULL 11.");
}

BaseGDL* NullGDL::NotOp()               
{
  throw GDLException("Operation not defined for !NULL 12.");
}

BaseGDL* NullGDL::AndOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for !NULL 13.");
}

BaseGDL* NullGDL::AndOpInv( BaseGDL* r) 
{
  throw GDLException("Operation not defined for !NULL 14.");
}

BaseGDL* NullGDL::OrOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for !NULL 13.");
}

BaseGDL* NullGDL::OrOpInv( BaseGDL* r) 
{
  throw GDLException("Operation not defined for !NULL 14.");
}

BaseGDL* NullGDL::XorOp( BaseGDL* r)    
{
  throw GDLException("Operation not defined for !NULL 13.");
}

BaseGDL* NullGDL::XorOpS( BaseGDL* r)    
{
  throw GDLException("XOR: Operation not defined for !NULL.");
}

BaseGDL* NullGDL::EqOp( BaseGDL* r)    
{
  if( (r == NULL) || (r == NullGDL::GetSingleInstance()))
  {
    return new Data_<SpDByte>( 1);
  }
  // check for null opject and pointer
  DType rTy= r->Type();
  if( rTy == PTR)
  {
    DPtrGDL* rP = static_cast<DPtrGDL*>(r);
    DPtr pVal;
    if( rP->Scalar( pVal) && pVal == 0)
      return new Data_<SpDByte>( 1);
  }
  else if( rTy == OBJECT)
  {
    DObjGDL* rP = static_cast<DObjGDL*>(r);
    DObj pVal;
    if( rP->Scalar( pVal) && pVal == 0)
      return new Data_<SpDByte>( 1);
  }
  return new Data_<SpDByte>( 0);
}

BaseGDL* NullGDL::NeOp( BaseGDL* r)    
{
  if( (r == NULL) || (r == NullGDL::GetSingleInstance()))
  {
    return new Data_<SpDByte>( 0);
  }
  // check for null opject and pointer
  DType rTy= r->Type();
  if( rTy == PTR)
  {
    DPtrGDL* rP = static_cast<DPtrGDL*>(r);
    DPtr pVal;
    if( rP->Scalar( pVal) && pVal == 0)
      return new Data_<SpDByte>( 0);
  }
  else if( rTy == OBJECT)
  {
    DObjGDL* rP = static_cast<DObjGDL*>(r);
    DObj pVal;
    if( rP->Scalar( pVal) && pVal == 0)
      return new Data_<SpDByte>( 0);
  }
  return new Data_<SpDByte>( 1);
}

BaseGDL* NullGDL::LeOp( BaseGDL* r)    
{
  throw GDLException("LE: Operation not defined for !NULL.");
}

BaseGDL* NullGDL::GeOp( BaseGDL* r)    
{
  throw GDLException("GE: Operation not defined for !NULL.");
}

BaseGDL* NullGDL::LtOp( BaseGDL* r)    
{
  throw GDLException("LT: Operation not defined for !NULL.");
}

BaseGDL* NullGDL::GtOp( BaseGDL* r)    
{
  throw GDLException("GT: Operation not defined for !NULL.");
}

BaseGDL* NullGDL::Add( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 15.");
}
BaseGDL* NullGDL::AddInv( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 15.");
}

BaseGDL* NullGDL::Sub( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::SubInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::LtMark( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 18.");
}

BaseGDL* NullGDL::GtMark( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 19.");
}

BaseGDL* NullGDL::Mult( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 19a.");
}

BaseGDL* NullGDL::Div( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::DivInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::Mod( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::ModInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::Pow( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::PowInv( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}
BaseGDL* NullGDL::PowInt( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 170.");
}

BaseGDL* NullGDL::MatrixOp( BaseGDL* r, bool rtranspose, bool transposeResult, bool strassen)
{
  throw GDLException("Operation not defined for !NULL 18.");
}

void NullGDL::AssignAt( BaseGDL* srcIn,	ArrayIndexListT* ixList, SizeT offset)
{
  throw GDLException("NullGDL::AssignAt(...) called.");
}
void NullGDL::AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList)
{
  throw GDLException("NullGDL::AssignAt(..) called.");
}
void NullGDL::AssignAt( BaseGDL* srcIn)
{
  throw GDLException("NullGDL::AssignAt(.) called.");
}

void NullGDL::DecAt( ArrayIndexListT* ixList)
{
  throw GDLException("NullGDL::DecAt(...) called.");
}
void NullGDL::IncAt( ArrayIndexListT* ixList)
{
  throw GDLException("NullGDL::IncAt(...) called.");
}
void NullGDL::Dec()
{
  throw GDLException("NullGDL::Dec(...) called.");
}
void NullGDL::Inc()
{
  throw GDLException("NullGDL::Inc(...) called.");
}

void NullGDL::InsertAt(  SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList)
{
  throw GDLException("NullGDL::InsertAt( SizeT, ...) called.");
}

// formatting output functions
SizeT NullGDL::OFmtA( std::ostream* os, SizeT offset, SizeT num, int width)
{throw GDLException("NullGDL::OFmtA(...) called.");}

SizeT NullGDL::OFmtF( std::ostream* os, SizeT offs, SizeT num, int width,
		      int prec, char fill, IOMode oM) 
{throw GDLException("NullGDL::OFmtF(...) called.");}

SizeT NullGDL::OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
		      int minN, char fill, NullGDL::IOMode oM)
{throw GDLException("NullGDL::OFmtI(...) called.");}

SizeT NullGDL::IFmtA( std::istream* is, SizeT offset, SizeT num, int width)
{throw GDLException("NullGDL::IFmtA(...) called.");}

SizeT NullGDL::IFmtF( std::istream* is, SizeT offs, SizeT num, int width)
{throw GDLException("NullGDL::IFmtF(...) called.");}

SizeT NullGDL::IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
		      NullGDL::IOMode oM)
{throw GDLException("NullGDL::IFmtI(...) called.");}

BaseGDL* NullGDL::Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
		 bool center, int edgeMode)
{
  throw GDLException("NullGDL::Convol(...) called.");
}
BaseGDL* NullGDL::Rebin( const dimension& newDim, bool sample)
{
  throw GDLException("NullGDL::Rebin(...) called.");
}

#ifdef USE_PYTHON
PyObject* NullGDL::ToPython()
{throw GDLException("NullGDL::ToPython(...) called.");}
#endif





BaseGDL* NullGDL::AndOpS( BaseGDL* r)    
{
  throw GDLException("Operation not defined for !NULL 13.");
}

BaseGDL* NullGDL::AndOpInvS( BaseGDL* r) 
{
  throw GDLException("Operation not defined for !NULL 14.");
}

BaseGDL* NullGDL::OrOpS( BaseGDL* r)    
{
  throw GDLException("Operation not defined for !NULL 13.");
}

BaseGDL* NullGDL::OrOpInvS( BaseGDL* r) 
{
  throw GDLException("Operation not defined for !NULL 14.");
}

BaseGDL* NullGDL::AddS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 15.");
}
BaseGDL* NullGDL::AddInvS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 15.");
}

BaseGDL* NullGDL::SubS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::SubInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::LtMarkS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 18.");
}

BaseGDL* NullGDL::GtMarkS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 19.");
}

BaseGDL* NullGDL::MultS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 19a.");
}

BaseGDL* NullGDL::DivS( BaseGDL* r)
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::DivInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::ModS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::ModInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::PowS( BaseGDL* r)      
{
  throw GDLException("Operation not defined for !NULL 16.");
}
BaseGDL* NullGDL::PowInvS( BaseGDL* r)   
{
  throw GDLException("Operation not defined for !NULL 17.");
}

BaseGDL* NullGDL::NewIx( SizeT ix)
{
  throw GDLException("Operation not defined for !NULL 20.");
}
BaseGDL* NullGDL::NewIx( BaseGDL* ix, bool strict)
{
  throw GDLException("Operation not defined for !NULL 21.");
}
BaseGDL* NullGDL::NewIx( AllIxBaseT* ix, const dimension* dIn)
{
  throw GDLException("Operation not defined for !NULL 22.");
}
BaseGDL* NullGDL::NewIxFrom( SizeT s)
{
  throw GDLException("Operation not defined for !NULL 23.");
}
BaseGDL* NullGDL::NewIxFrom( SizeT s, SizeT e)
{
  throw GDLException("Operation not defined for !NULL 24.");
}
BaseGDL* NullGDL::NewIxFromStride( SizeT s, SizeT stride)
{
  throw GDLException("Operation not defined for !NULL 25.");
}
BaseGDL* NullGDL::NewIxFromStride( SizeT s, SizeT e, SizeT stride)
{
  throw GDLException("Operation not defined for !NULL 26.");
}
BaseGDL* NullGDL::Log()              
{ 
  throw GDLException("Operation not defined for !NULL 27a.");
}
BaseGDL* NullGDL::LogThis()              
{ 
  throw GDLException("Operation not defined for !NULL 27b.");
}
BaseGDL* NullGDL::Log10()              
{ 
  throw GDLException("Operation not defined for !NULL 27c.");
}
BaseGDL* NullGDL::Log10This()              
{ 
  throw GDLException("Operation not defined for !NULL 27d.");
}

void NullGDL::AssignAtIx( RangeT ix, BaseGDL* srcIn)
{ 
  throw GDLException("Operation not defined for !NULL 28.");
}

RangeT NullGDL::LoopIndex() const
{ 
  throw GDLException("Operation not defined for !NULL 29.");
}

BaseGDL* NullGDL::Rotate( DLong dir)
{ 
  throw GDLException("Operation not defined for !NULL 30.");
}

void NullGDL::Reverse( DLong dim)
{ 
  throw GDLException("Operation not defined for !NULL 31.");
}

BaseGDL* NullGDL::DupReverse( DLong dim)
{ 
  throw GDLException("Operation not defined for !NULL 32.");
}

  BaseGDL* NullGDL::AndOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 40.");}
  BaseGDL* NullGDL::AndOpInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 41.");}
  BaseGDL* NullGDL::OrOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 42.");}
  BaseGDL* NullGDL::OrOpInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 43.");}
  BaseGDL* NullGDL::XorOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 44.");}
//   BaseGDL* NullGDL::EqOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 45.");}
//   BaseGDL* NullGDL::NeOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 46.");}
//   BaseGDL* NullGDL::LeOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 47.");}
//   BaseGDL* NullGDL::GeOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 48.");}
//   BaseGDL* NullGDL::LtOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 49.");}
//   BaseGDL* NullGDL::GtOpNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 50.");}
  BaseGDL* NullGDL::AddNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 51.");}       // implemented
  BaseGDL* NullGDL::AddInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 52.");}       // implemented
  BaseGDL* NullGDL::SubNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 53.");}
  BaseGDL* NullGDL::SubInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 54.");}
  BaseGDL* NullGDL::LtMarkNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 55.");}
  BaseGDL* NullGDL::GtMarkNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 56.");}
  BaseGDL* NullGDL::MultNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 57.");}    // implemented
  BaseGDL* NullGDL::DivNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 58.");}
  BaseGDL* NullGDL::DivInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 59.");}
  BaseGDL* NullGDL::ModNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 60.");}
  BaseGDL* NullGDL::ModInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 61.");}
  BaseGDL* NullGDL::PowNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 62.");}
  BaseGDL* NullGDL::PowInvNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 63.");}
  BaseGDL* NullGDL::PowIntNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 64.");}    // implemented

  BaseGDL* NullGDL::AndOpSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 65.");}
  BaseGDL* NullGDL::AndOpInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 66.");}
  BaseGDL* NullGDL::OrOpSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 67.");}
  BaseGDL* NullGDL::OrOpInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 68.");}
  BaseGDL* NullGDL::XorOpSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 69.");}
  BaseGDL* NullGDL::AddSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 70.");}          // implemented
  BaseGDL* NullGDL::AddInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 71.");}     // implemented
  BaseGDL* NullGDL::SubSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 73.");}
  BaseGDL* NullGDL::SubInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 74.");}
  BaseGDL* NullGDL::LtMarkSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 75.");}
  BaseGDL* NullGDL::GtMarkSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 76.");}
  BaseGDL* NullGDL::MultSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 77.");}       // implemented
  BaseGDL* NullGDL::DivSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 78.");}
  BaseGDL* NullGDL::DivInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 79.");}
  BaseGDL* NullGDL::ModSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 80.");}
  BaseGDL* NullGDL::ModInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 81.");}
  BaseGDL* NullGDL::PowSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 82.");}
  BaseGDL* NullGDL::PowInvSNew( BaseGDL* r) { throw GDLException("Operation not defined for !NULL 83.");}
