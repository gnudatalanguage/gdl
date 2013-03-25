/***************************************************************************
                          assocdata.hpp  -  GDL assoc variables
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

#ifndef ASSOCDATA_HPP_
#define ASSOCDATA_HPP_

#include "typetraits.hpp"
#include "datatypes.hpp"
#include "dstructgdl.hpp"

template<class Parent_>
class Assoc_: public Parent_
{
  int    lun;       // internal lun (0..127)
  SizeT fileOffset;
  SizeT sliceSize; // size of one slice

public:
	// memory management optimization
static std::deque< void*> freeList;

	// operator new and delete
static 	void* operator new( size_t bytes);
static	void operator delete( void *ptr);


  // external lun (1..128)
  Assoc_( int lun_, Parent_* assoc_, SizeT fileOffset_);
  
  ~Assoc_() {}
  
  // c-i
  Assoc_( const Assoc_& cp): Parent_(cp),
			     lun(cp.lun),
			     fileOffset( cp.fileOffset),
			     sliceSize( cp.sliceSize)
  {}

 Parent_* Dup() { return new Assoc_(*this);}

  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset);
  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList);
  void AssignAt( BaseGDL* srcIn);

  // used by AccessDescT for resolving, no checking is done
  // inserts srcIn[ ixList] at offset
  void InsertAt( SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);

  // returns (*this)[ ixList]
  Parent_* Index( ArrayIndexListT* ixList);

  SizeT N_Elements() const { return 1;}

  bool IsAssoc() const { return true;}

  // ******************************************
  // all the following operations are illegal with file types
  // one dim array access (unchecked)
  typename Parent_::Ty& operator[] (const SizeT d1)
  {
    throw GDLException("Assoc_::Operator[](...) called.");
  }

  SizeT NBytes() const 
  {
    throw GDLException("Assoc_::NBytes() called.");
  }
  
  std::ostream& Write( std::ostream& os, bool swapEndian)
  {
    throw GDLException("Assoc_::Write(...) called.");
  }
  std::istream& Read( std::istream& os, bool swapEndian)
  {
    throw GDLException("Assoc_::Read(...) called.");
  }

  //  std::ostream& ToStream(std::ostream& o)
  std::ostream& ToStream(std::ostream& o, SizeT width = 0, SizeT* actPosPtr = NULL)
  {
    throw GDLException("File expression not allowed in this context.");
  }
  std::istream& FromStream(std::istream& i)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool Scalar() const { return false;}
  //  const DType   Type() const;
  //  const string& TypeStr() const; 

  //Don't overwrite New method
  Parent_* New( const dimension& dim_, BaseGDL::InitType noZero=BaseGDL::ZERO) const
  {
    throw GDLException("Assoc_::New(...) called.");
  }
  Parent_* NewResult() const
  {
    throw GDLException("Assoc_::NewResult() called.");
  }
  
  // cannot convert Assoc_ data
  // Parent_::CatArray throws because of Parent_::Convert2(...)
  // if type is Assoc_<>
  Parent_* Convert2( DType destTy, 
		     BaseGDL::Convert2Mode=BaseGDL::CONVERT)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  // DStructDesc::AddTag() (which uses GetTag()) must refuse Assoc_
  Parent_* GetTag() const 
  {
    throw GDLException("File expression not allowed in this context.");
  }

  // because of AddTag(), this function should never be called
  Parent_* GetInstance() const
  {
    throw GDLException("Assoc_::GetInstance(...) called.");
  }

  int Scalar2Index(SizeT& ret) const 
  { 
    throw GDLException("File expression not allowed in this context.");
  }

  bool True()
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool LogTrue()
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool LogTrue( SizeT ix)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Data_<SpDByte>* LogNeg()
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool False()
  {
    throw GDLException("File expression not allowed in this context.");
  }

  int Sgn() // -1,0,1
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool Equal( BaseGDL*)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool EqualNoDelete( const BaseGDL*)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  void ForCheck( BaseGDL**, BaseGDL** = NULL)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool ForAddCondUp( BaseGDL* loopInfo)
//   bool ForAddCondUp( ForLoopInfoT& loopInfo)
  {
    throw GDLException("File expression not allowed in this context.");
  }
  
  bool ForCondUp( BaseGDL*)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  bool ForCondDown( BaseGDL*)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  void ForAdd( BaseGDL* add=NULL)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* CatArray( ExprListT& exprList, 
		     const SizeT catRank, 
		     const SizeT rank) 
  {
    throw GDLException("File expression not allowed in this context.");
  }

  // used in r_expr
  BaseGDL* UMinus()              
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* NotOp()               
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* AndOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* AndOpInv( BaseGDL* r) 
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* OrOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* OrOpInv( BaseGDL* r) 
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* XorOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  BaseGDL* EqOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  BaseGDL* NeOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  BaseGDL* LeOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  BaseGDL* GeOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  BaseGDL* LtOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  BaseGDL* GtOp( BaseGDL* r)    
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* Add( BaseGDL* r)      
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* AddInv( BaseGDL* r)      
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* Sub( BaseGDL* r)      
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* SubInv( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* LtMark( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* GtMark( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* Mult( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* Div( BaseGDL* r)      
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* DivInv( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* Mod( BaseGDL* r)      
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* ModInv( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* Pow( BaseGDL* r)      
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* PowInv( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* PowInt( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }
  Parent_* PowIntNew( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* MatrixOp( BaseGDL* r)   
  {
    throw GDLException("File expression not allowed in this context.");
  }
 
  void DecAt( ArrayIndexListT* ixList)
  {
    throw GDLException("File expression not allowed in this context.");
  }
 
  void IncAt( ArrayIndexListT* ixList)
  {
    throw GDLException("File expression not allowed in this context.");
  }
 
  void Dec()
  {
    throw GDLException("File expression not allowed in this context.");
  }
 
  void Inc()
  {
    throw GDLException("File expression not allowed in this context.");
  }
 
  SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width)
  {
    throw GDLException("File expression not allowed in this context.");
  }
  SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width, 
		int prec, char fill, BaseGDL::IOMode oM = BaseGDL::FIXED)
  {
    throw GDLException("File expression not allowed in this context.");
  }
  SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
		int minN, char fill, BaseGDL::IOMode oM = BaseGDL::DEC)
  {
    throw GDLException("File expression not allowed in this context.");
  }
  SizeT IFmtA( std::istream* os, SizeT offset, SizeT num, int width)
  {
    throw GDLException("File expression not allowed in this context.");
  }
  SizeT IFmtF( std::istream* os, SizeT offs, SizeT num, int width) 
  {
    throw GDLException("File expression not allowed in this context.");
  }
  SizeT IFmtI( std::istream* os, SizeT offs, SizeT num, int width, 
		BaseGDL::IOMode oM = BaseGDL::DEC)
  {
    throw GDLException("File expression not allowed in this context.");
  }

  Parent_* AssocVar( int, SizeT)
  {
    throw GDLException("File expression not allowed in this context.");
  }
};

template<>
Assoc_< DStructGDL>::Assoc_( int lun_, DStructGDL* assoc_, SizeT fileOffset_);

#endif
