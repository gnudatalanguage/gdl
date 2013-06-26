/***************************************************************************
                          nullgdl.hpp  -  class for !NULL object
                             -------------------
    begin                : July 22 2012
    copyright            : (C) 2012 by Marc Schellens
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

#ifndef NULLGDL_HPP_
#define NULLGDL_HPP_

#include "basegdl.hpp"

// !NULL is a singleton
class NullGDL: public BaseGDL
{ 
  private:
    static NullGDL* instance;
    static char buf[];
    
    NullGDL(): BaseGDL() {} 

    ~NullGDL(); // virtual due to base class -> can be called nevertheless (but this would be an error here)

  public:

    void* operator new( size_t bytes, char* cP)
    {
	assert( bytes == sizeof( NullGDL));
	return NullGDL::buf;
    }
    void* operator new( size_t bytes)
    {
	assert( bytes == sizeof( NullGDL));
	return NullGDL::buf;
    }

    void operator delete( void *ptr)
    {
	// do nothing
    }
    
    static NullGDL* GetSingleInstance()
    {
      if( instance == NULL)
	instance = new (NullGDL::buf) NullGDL();
      return instance;
    }
    
    static bool IsNULLorNullGDL( BaseGDL* p)
    {
      // works for uninitialized !NULL as well
      // if not instantiated yet, instance is NULL
      // if p is !NULL, instance must be initialized
      // even then we save one comparison (for instance == NULL)
      return (p == NULL) || (p == instance);
    }
//  private:
  /*virtual*/ BaseGDL& operator=(const BaseGDL& right);
//  public:
 
  /*virtual*/ void InitFrom(const BaseGDL& right); // for structs

  // /*virtual*/ functions
  /*virtual*/ bool IsAssoc() const;
  /*virtual*/ BaseGDL* AssocVar( int, SizeT);

  /*virtual*/ SizeT N_Elements() const; // number of elements
  /*virtual*/ SizeT Size() const;       // size (= N_Elements, but 0 for BaseGDL)
  /*virtual*/ SizeT NBytes() const;     // total bytes of data
  /*virtual*/ SizeT ToTransfer() const; // elements to transfer
  /*virtual*/ SizeT Sizeof() const;     // size of scalar data
  
  /*virtual*/ BaseGDL* Transpose( DUInt* perm);
  /*virtual*/ BaseGDL* Rotate( DLong dir);
  /*virtual*/ void Reverse( DLong dim);
  /*virtual*/ BaseGDL* DupReverse( DLong dim);

  /*virtual*/ void MinMax( DLong* minE, DLong* maxE, 
		       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
		       SizeT start = 0, SizeT stop = 0, SizeT step = 1, DLong valIx = -1);

  /*virtual*/ void Clear();
  /*virtual*/ void Construct();
  /*virtual*/ void ConstructTo0();
  /*virtual*/ void Destruct();
  /*virtual*/ std::ostream& Write( std::ostream& os, bool swapEndian, 
			       bool compress, XDR *xdrs);
  /*virtual*/ std::istream& Read( std::istream& os, bool swapEndian, 
			      bool compress, XDR *xdrs);

  /*virtual*/ std::ostream& ToStream(std::ostream& o, SizeT width = 0, 
			    SizeT* actPosPtr = NULL);
  /*virtual*/ std::istream& FromStream(std::istream& i);

  /*virtual*/ bool Greater(SizeT i1, SizeT i2) const; // comp 2 elements
  /*virtual*/ bool Equal(SizeT i1, SizeT i2) const; // comp 2 elements

  /*virtual*/ BaseGDL* CShift( DLong d) const; // circular shift
  /*virtual*/ BaseGDL* CShift( DLong d[MAXRANK]) const; // circular shift multi dim

  /*virtual*/ bool Scalar() const;
  /*virtual*/ bool StrictScalar() const;
  /*virtual*/ DType   Type() const;
  /*virtual*/ const std::string& TypeStr() const;
  /*virtual*/ bool          EqType( const BaseGDL*) const;
  /*virtual*/ void* DataAddr();// SizeT elem=0);
  /*virtual*/ BaseGDL* New( const dimension& dim_, InitType noZero=ZERO) const;
  /*virtual*/ BaseGDL* NewResult() const;
  /*virtual*/ BaseGDL* Dup() const;
//   /*virtual*/ BaseGDL* Dup( char*) const; 
  /*virtual*/ BaseGDL* Convert2( DType destTy, Convert2Mode mode=CONVERT);
  /*virtual*/ BaseGDL* GetTag() const; 
  /*virtual*/ BaseGDL* GetInstance() const;
  /*virtual*/ BaseGDL* GetEmptyInstance() const;
  /*virtual*/ BaseGDL* SetBuffer( const void* b);
  /*virtual*/ void     SetBufferSize( SizeT s);
  /*virtual*/ int Scalar2Index(SizeT& ret) const;
  /*virtual*/ int Scalar2RangeT(RangeT& ret) const;
  /*virtual*/ SizeT GetAsIndex( SizeT i) const;
  /*virtual*/ SizeT GetAsIndexStrict( SizeT i) const;
  /*virtual*/ RangeT LoopIndex() const;
  /*virtual*/ bool True();
  /*virtual*/ bool False();
  /*virtual*/ bool LogTrue();
  /*virtual*/ bool LogTrue( SizeT ix);
  /*virtual*/ DLong* Where( bool comp, SizeT& count);
  /*virtual*/ BaseGDL* LogNeg();
  /*virtual*/ int Sgn(); // -1,0,1
  /*virtual*/ bool Equal( BaseGDL*) const;
  /*virtual*/ bool EqualNoDelete( const BaseGDL*) const;
  /*virtual*/ bool ArrayEqual( BaseGDL*);
  // for statement compliance (int types , float types scalar only)
  /*virtual*/ void ForCheck( BaseGDL**, BaseGDL** = NULL);
  /*virtual*/ bool ForCondUp( BaseGDL*);
  /*virtual*/ bool ForCondDown( BaseGDL*);
  /*virtual*/ bool ForAddCondUp( BaseGDL* loopInfo);
//   /*virtual*/ bool ForAddCondUp( ForLoopInfoT& loopInfo);
//  /*virtual*/ bool ForAddCondDown( ForLoopInfoT& loopInfo);
//   /*virtual*/ bool ForCondUpDown( BaseGDL*);
  /*virtual*/ void ForAdd( BaseGDL* add=NULL);
  /*virtual*/ BaseGDL* CatArray( ExprListT& exprList,
			     const SizeT catRank, 
			     const SizeT rank); 
  /*virtual*/ BaseGDL* Index( ArrayIndexListT* ixList); 
  //  /*virtual*/ BaseGDL* Abs() const; 

  // return a new type of itself
  /*virtual*/ BaseGDL* NewIx( SizeT ix);
  /*virtual*/ BaseGDL* NewIx( BaseGDL* ix, bool strict);
  /*virtual*/ BaseGDL* NewIx( AllIxBaseT* ix, const dimension* dIn);
  /*virtual*/ BaseGDL* NewIxFrom( SizeT s);
  /*virtual*/ BaseGDL* NewIxFrom( SizeT s, SizeT e);
  /*virtual*/ BaseGDL* NewIxFromStride( SizeT s, SizeT stride);
  /*virtual*/ BaseGDL* NewIxFromStride( SizeT s, SizeT e, SizeT stride);

  // library functions
  /*virtual*/ BaseGDL* Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
			   bool center, int edgeMode);
  /*virtual*/ BaseGDL* Rebin( const dimension& newDim, bool sample);
  // for STRUCT_ASSIGN
  /*virtual*/ void Assign( BaseGDL* src, SizeT nEl);

  /*virtual*/ BaseGDL* Log();              
  /*virtual*/ BaseGDL* LogThis();
  /*virtual*/ BaseGDL* Log10();              
  /*virtual*/ BaseGDL* Log10This();

  // defined in basic_op.cpp
  // used in r_expr
  /*virtual*/ BaseGDL* UMinus();              
  /*virtual*/ BaseGDL* NotOp();
             
  /*virtual*/ BaseGDL* AndOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* AndOpInv( BaseGDL* r); 
  /*virtual*/ BaseGDL* OrOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* OrOpInv( BaseGDL* r); 
  /*virtual*/ BaseGDL* XorOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* EqOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* NeOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* LeOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* GeOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* LtOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* GtOp( BaseGDL* r);    
  /*virtual*/ BaseGDL* Add( BaseGDL* r);      
  /*virtual*/ BaseGDL* AddInv( BaseGDL* r);      
  /*virtual*/ BaseGDL* Sub( BaseGDL* r);
  /*virtual*/ BaseGDL* SubInv( BaseGDL* r);   
  /*virtual*/ BaseGDL* LtMark( BaseGDL* r);   
  /*virtual*/ BaseGDL* GtMark( BaseGDL* r);   
  /*virtual*/ BaseGDL* Mult( BaseGDL* r);   
  /*virtual*/ BaseGDL* Div( BaseGDL* r);
  /*virtual*/ BaseGDL* DivInv( BaseGDL* r);   
  /*virtual*/ BaseGDL* Mod( BaseGDL* r);      
  /*virtual*/ BaseGDL* ModInv( BaseGDL* r);   
  /*virtual*/ BaseGDL* Pow( BaseGDL* r);      
  /*virtual*/ BaseGDL* PowInv( BaseGDL* r);   
  /*virtual*/ BaseGDL* PowInt( BaseGDL* r);

  /*virtual*/ BaseGDL* AndOpS( BaseGDL* r);    
  /*virtual*/ BaseGDL* AndOpInvS( BaseGDL* r); 
  /*virtual*/ BaseGDL* OrOpS( BaseGDL* r);    
  /*virtual*/ BaseGDL* OrOpInvS( BaseGDL* r); 
  /*virtual*/ BaseGDL* XorOpS( BaseGDL* r);    
  /*virtual*/ BaseGDL* AddS( BaseGDL* r);      
  /*virtual*/ BaseGDL* AddInvS( BaseGDL* r);
  /*virtual*/ BaseGDL* SubS( BaseGDL* r);
  /*virtual*/ BaseGDL* SubInvS( BaseGDL* r);   
  /*virtual*/ BaseGDL* LtMarkS( BaseGDL* r);   
  /*virtual*/ BaseGDL* GtMarkS( BaseGDL* r);   
  /*virtual*/ BaseGDL* MultS( BaseGDL* r);   
  /*virtual*/ BaseGDL* DivS( BaseGDL* r);
  /*virtual*/ BaseGDL* DivInvS( BaseGDL* r);   
  /*virtual*/ BaseGDL* ModS( BaseGDL* r);      
  /*virtual*/ BaseGDL* ModInvS( BaseGDL* r);   
  /*virtual*/ BaseGDL* PowS( BaseGDL* r);      
  /*virtual*/ BaseGDL* PowInvS( BaseGDL* r);   


  
  
  
// the New functions
// defined in basic_op_new.cpp
// results go into a new DataT

  /*virtual*/ BaseGDL* AndOpNew( BaseGDL* r);
  /*virtual*/ BaseGDL* AndOpInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* OrOpNew( BaseGDL* r);
  /*virtual*/ BaseGDL* OrOpInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* XorOpNew( BaseGDL* r);
//   /*virtual*/ BaseGDL* EqOpNew( BaseGDL* r);
//   /*virtual*/ BaseGDL* NeOpNew( BaseGDL* r);
//   /*virtual*/ BaseGDL* LeOpNew( BaseGDL* r);
//   /*virtual*/ BaseGDL* GeOpNew( BaseGDL* r);
//   /*virtual*/ BaseGDL* LtOpNew( BaseGDL* r);
//   /*virtual*/ BaseGDL* GtOpNew( BaseGDL* r);
  /*virtual*/ BaseGDL* AddNew( BaseGDL* r);      // implemented
  /*virtual*/ BaseGDL* AddInvNew( BaseGDL* r);      // implemented
  /*virtual*/ BaseGDL* SubNew( BaseGDL* r);
  /*virtual*/ BaseGDL* SubInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* LtMarkNew( BaseGDL* r);
  /*virtual*/ BaseGDL* GtMarkNew( BaseGDL* r);
  /*virtual*/ BaseGDL* MultNew( BaseGDL* r);   // implemented
  /*virtual*/ BaseGDL* DivNew( BaseGDL* r);
  /*virtual*/ BaseGDL* DivInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* ModNew( BaseGDL* r);
  /*virtual*/ BaseGDL* ModInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* PowNew( BaseGDL* r);
  /*virtual*/ BaseGDL* PowInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* PowIntNew( BaseGDL* r);   // implemented

  /*virtual*/ BaseGDL* AndOpSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* AndOpInvSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* OrOpSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* OrOpInvSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* XorOpSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* AddSNew( BaseGDL* r);         // implemented
  /*virtual*/ BaseGDL* AddInvSNew( BaseGDL* r);    // implemented
  /*virtual*/ BaseGDL* SubSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* SubInvSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* LtMarkSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* GtMarkSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* MultSNew( BaseGDL* r);      // implemented
  /*virtual*/ BaseGDL* DivSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* DivInvSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* ModSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* ModInvSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* PowSNew( BaseGDL* r);
  /*virtual*/ BaseGDL* PowInvSNew( BaseGDL* r);


  
  //  /*virtual*/ BaseGDL* PowInvNew( BaseGDL* r);
  /*virtual*/ BaseGDL* MatrixOp( BaseGDL* r, bool atranspose, bool btranspose);
  /*virtual*/ void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset);
  /*virtual*/ void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList);
  /*virtual*/ void AssignAt( BaseGDL* srcIn);

  /*virtual*/ void AssignAtIx( RangeT ix, BaseGDL* srcIn);

  /*virtual*/ void DecAt( ArrayIndexListT* ixList);
  /*virtual*/ void IncAt( ArrayIndexListT* ixList);
  /*virtual*/ void Dec();
  /*virtual*/ void Inc();
  /*virtual*/ void InsertAt(  SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);

  // /*virtual*/ formatting output functions
  /*virtual*/ SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width);
  /*virtual*/ SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width,
			int prec, char fill, IOMode oM = FIXED); 
  /*virtual*/ SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
			int minN, char fill, BaseGDL::IOMode oM = DEC);
  /*virtual*/ SizeT OFmtCal( std::ostream* os, SizeT offs, SizeT num, int width, 
			int minN, char fill, BaseGDL::Cal_IOMode oM = BaseGDL::DEFAULT);
  /*virtual*/ SizeT IFmtA( std::istream* is, SizeT offset, SizeT num, int width);
  /*virtual*/ SizeT IFmtF( std::istream* is, SizeT offs, SizeT num, int width);
  /*virtual*/ SizeT IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
			BaseGDL::IOMode oM = DEC);

#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

  /*virtual*/ PyObject* ToPython();
#endif
}; // class NullGDL


#endif

