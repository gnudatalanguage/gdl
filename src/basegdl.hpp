/***************************************************************************
                          basegdl.hpp  -  base class for all data types
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

#ifndef BASEGDL_HPP_
#define BASEGDL_HPP_

#include <list>

#include "dimension.hpp"

#ifdef USE_PYTHON
#include <Python.h>
#endif

// GDL typecodes
enum DType {  // Object types (IDL type numbers)
  UNDEF=0,    // Undefined value, the default for new symbols
  BYTE=1,     // byte
  INT,	      // Integer scalar
  LONG,	      // long Integer scalar
  FLOAT,      // Real scalar
  DOUBLE,     // Integer scalar
  COMPLEX,    // Complex scalar
  STRING,     // String
  STRUCT,     // Struct
  COMPLEXDBL, // Complex double
  PTR,	      // Pointer
  OBJECT,     // Object reference
  UINT,       // unsigned int
  ULONG,      // unsigned long int
  LONG64,      // 64 bit integer
  ULONG64      // unsigned 64 bit integer
};

// order of conversion precedence if two types are the same,
// the first type is used
const int DTypeOrder[]={
  -1, 	//UNDEF
  2, 	//BYTE
  3, 	//INT
  4, 	//LONG,	
  8, 	//FLOAT,	
  9, 	//DOUBLE,	
  10, 	//COMPLEX,	
  1, 	//STRING,	
  101, 	//STRUCT,	
  11, 	//COMPLEXDBL,	
  102, 	//PTR,		
  103, 	//OBJECT,
  3, 	//UINT,	
  4, 	//ULONG,
  5, 	//LONG64,
  5 	//ULONG64
};	

class   BaseGDL;
class   ArrayIndexListT;
class   ExprListT;

struct SpDULong;
template<class> class Data_;

void breakpoint();

class BaseGDL
{
protected:
  dimension dim;
  
public:
  // type of initalization
  enum InitType {
    ZERO=0,
    NOZERO,
    INDGEN,
    INIT
  };

  enum Convert2Mode {
    CONVERT=1,
    COPY=2,
    COPY_BYTE_AS_INT= (4|2) // for STRING function
  };

  enum IOMode {
    FIXED=0,
    SCIENTIFIC,
    AUTO,
    OCT=8,
    DEC=10,
    HEX=16,
    HEXL // lower case characters
  };
  
  // FIRST VIRTUAL FUNCTION'S OBJECT FILE CONTAINS ALSO THE VTABLE
  // therefore it must be defined non-inline (g++)
  virtual ~BaseGDL(); // defined in datatypes.cpp

  BaseGDL();//: dim() {} 
  BaseGDL(const dimension& dim_);//: dim(dim_) {} 


  // provide access to dim member
  inline const dimension& Dim()   const      { return dim;}
  inline SizeT    Dim(SizeT d)    const      { return dim[d];}
  inline SizeT    Stride(SizeT d) const      { return dim.Stride(d);}
  inline void     Purge()                    { dim.Purge();}
  inline SizeT    Rank()          const      { return dim.Rank();}
  inline void     SetDim(const dimension& d) { dim=d;}


  friend std::istream& operator>>(std::istream& i, BaseGDL& data_)
  {
    throw GDLException("Variable is of type UNDEF.");
    return i;
  }


  // virtual functions
  virtual bool IsAssoc() const;
  virtual BaseGDL* AssocVar( int, SizeT);

  virtual const SizeT N_Elements() const;
  virtual const SizeT Size() const;
  virtual const SizeT NBytes() const;
  virtual const SizeT ToTransfer() const;
  virtual const SizeT Sizeof() const;
  
  virtual BaseGDL* Transpose( DUInt* perm);

  virtual void MinMax( DLong* minE, DLong* maxE, 
		       BaseGDL** minVal, BaseGDL** maxVal);

  virtual void Clear();
  virtual std::ostream& Write( std::ostream& os, bool swapEndian);
  virtual std::istream& Read( std::istream& os, bool swapEndian);

  virtual std::ostream& ToStream(std::ostream& o, SizeT width = 0, 
			    SizeT* actPosPtr = NULL);
  virtual std::istream& FromStream(std::istream& i);

  virtual bool Greater(SizeT i1, SizeT i2) const; // comp 2 elements
  virtual bool Equal(SizeT i1, SizeT i2) const; // comp 2 elements

  virtual BaseGDL* CShift( DLong d); // circular shift
  virtual BaseGDL* CShift( DLong d[MAXRANK]); // circular shift multi dim

  virtual bool Scalar() const;
  virtual const DType   Type() const;
  virtual const std::string& TypeStr() const;
  virtual bool          EqType( const BaseGDL*) const;
  virtual void* DataAddr( SizeT elem=0);
  virtual BaseGDL* New( const dimension& dim_, InitType noZero=ZERO);
  virtual BaseGDL* Dup(); 
  virtual BaseGDL* Convert2( DType destTy, Convert2Mode mode=CONVERT);
  virtual BaseGDL* GetTag() const; 
  virtual BaseGDL* GetInstance() const;
  virtual int Scalar2index(SizeT& ret) const; 
  virtual bool True();
  virtual bool False();
  virtual bool LogTrue();
  virtual bool LogTrue( SizeT ix);
  virtual BaseGDL* LogNeg();
  virtual int Sgn(); // -1,0,1
  virtual bool Equal( BaseGDL*);
  virtual bool ArrayEqual( BaseGDL*);
  // for statement compliance (int types , float types scalar only)
  virtual void ForCheck( BaseGDL**, BaseGDL** = NULL);
  virtual bool ForCondUp( BaseGDL*);
  virtual bool ForCondDown( BaseGDL*);
  virtual void ForAdd( BaseGDL* add=NULL);
  virtual BaseGDL* CatArray( ExprListT& exprList,
			     const SizeT catRank, 
			     const SizeT rank); 
  virtual BaseGDL* Index( ArrayIndexListT* ixList); 

  // library functions
  virtual BaseGDL* Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
			   bool center, int edgeMode);
  virtual BaseGDL* Rebin( const dimension& newDim, bool sample);

  // used in r_expr
  virtual BaseGDL* UMinus();              
  virtual BaseGDL* NotOp();               
  virtual BaseGDL* AndOp( BaseGDL* r);    
  virtual BaseGDL* AndOpInv( BaseGDL* r); 
  virtual BaseGDL* OrOp( BaseGDL* r);    
  virtual BaseGDL* OrOpInv( BaseGDL* r); 
  virtual BaseGDL* XorOp( BaseGDL* r);    
  virtual BaseGDL* EqOp( BaseGDL* r);    
  virtual BaseGDL* NeOp( BaseGDL* r);    
  virtual BaseGDL* LeOp( BaseGDL* r);    
  virtual BaseGDL* GeOp( BaseGDL* r);    
  virtual BaseGDL* LtOp( BaseGDL* r);    
  virtual BaseGDL* GtOp( BaseGDL* r);    
  virtual BaseGDL* Add( BaseGDL* r);      
  virtual BaseGDL* AddInv( BaseGDL* r);      
  virtual BaseGDL* Sub( BaseGDL* r);      
  virtual BaseGDL* SubInv( BaseGDL* r);   
  virtual BaseGDL* LtMark( BaseGDL* r);   
  virtual BaseGDL* GtMark( BaseGDL* r);   
  virtual BaseGDL* Mult( BaseGDL* r);   
  virtual BaseGDL* Div( BaseGDL* r);      
  virtual BaseGDL* DivInv( BaseGDL* r);   
  virtual BaseGDL* Mod( BaseGDL* r);      
  virtual BaseGDL* ModInv( BaseGDL* r);   
  virtual BaseGDL* Pow( BaseGDL* r);      
  virtual BaseGDL* PowInv( BaseGDL* r);   
  virtual BaseGDL* MatrixOp( BaseGDL* r);   
  virtual void AssignAt( BaseGDL* srcIn, 
			 ArrayIndexListT* ixList=NULL, SizeT offset=0);
  virtual void DecAt( ArrayIndexListT* ixList);
  virtual void IncAt( ArrayIndexListT* ixList);
  virtual void Dec();
  virtual void Inc();
  virtual void InsertAt(  SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);

  // virtual formatting output functions
  virtual SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width);
  virtual SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width,
			int prec, IOMode oM = FIXED); 
  virtual SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
			int minN, BaseGDL::IOMode oM = DEC);
  virtual SizeT IFmtA( std::istream* is, SizeT offset, SizeT num, int width);
  virtual SizeT IFmtF( std::istream* is, SizeT offs, SizeT num, int width);
  virtual SizeT IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
			BaseGDL::IOMode oM = DEC);

#ifdef USE_PYTHON
  virtual PyObject* ToPython();
#endif
};

// ExprListT deletes all members upon own destruction
class ExprListT: public std::list<BaseGDL*> 
{
public:
  ~ExprListT()
  {
    for( ExprListT::iterator i=this->begin(); i!=this->end(); ++i)
      delete *i;
  }
};

typedef ExprListT::iterator ExprListIterT;

#endif

