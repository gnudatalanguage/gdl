/***************************************************************************
                          basegdl.hpp  -  base class for all data types
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

#ifndef BASEGDL_HPP_
#define BASEGDL_HPP_

#include "includefirst.hpp"

//use libtirpc (to be installed) for linux & mac, use portablexdr for windows, but use our custom 'xdr.h' 
#if defined(_WIN32)
#include "types.h"
#include "xdr.h"
#define u_int64_t uint64_t
#define u_int32_t uint32_t
#define xdr_int8_t xdr_char
#define xdr_u_int8_t xdr_u_char
#define xdr_uint8_t xdr_short
#define xdr_int16_t xdr_short
#define xdr_u_int16_t xdr_u_short
#define xdr_uint16_t xdr_u_short
#define xdr_int32_t xdr_int
#define xdr_u_int32_t xdr_u_int
#define xdr_uint32_t xdr_u_int
#else
#include <rpc/rpc.h>
#ifdef __APPLE__
#define xdr_uint16_t xdr_u_int16_t
#define xdr_uint32_t xdr_u_int32_t
#define xdr_uint64_t xdr_u_int64_t
#endif
#endif

#include <algorithm>
#include "dimension.hpp"
#include "gdlexception.hpp"

#ifdef HAVE_MALLOC_H
#  include <malloc.h>
#endif
#ifdef HAVE_MALLOC_MALLOC_H
#  include <malloc/malloc.h>
#endif
#if (!defined(HAVE_MALLINFO) && !defined(HAVE_MALLINFO2))
#  if (!defined(HAVE_MALLOC_ZONE_STATISTICS) || !defined(HAVE_MALLOC_MALLOC_H))
#    if defined(HAVE_SBRK)
#      include <unistd.h>
#    endif
#  endif
#endif

// GDL typecodes
enum DType {  // Object types (IDL type numbers)
  GDL_UNDEF=0,    // 0 Undefined value, the default for new symbols
  GDL_BYTE=1,     // 1 byte
  GDL_INT,	      // 2 Integer scalar 
  GDL_LONG,	      // 3 long Integer scalar
  GDL_FLOAT,      // 4 Real scalar
  GDL_DOUBLE,     // 5 Double scalar
  GDL_COMPLEX,    // 6 Complex scalar
  GDL_STRING,     // 7 String
  GDL_STRUCT,     // 8 Struct
  GDL_COMPLEXDBL, // 9 Complex double
  GDL_PTR,	      // 10 Pointer
  GDL_OBJ,     // 11 Object reference
  GDL_UINT,       // 12 unsigned int
  GDL_ULONG,      // 13 unsigned long int
  GDL_LONG64,     // 14 64 bit integer
  GDL_ULONG64     // 15 unsigned 64 bit integer

  // not yet implemented
//  , GDL_LONG128  // 128 bit integer
//  , GDL_ULONG128 // unsigned 128 bit integer
  
//  , GDL_LONGABI // arbitrary length int
  //, GDL_ULONGABI // arbitrary length unsigned int (pointless)
  
//  , GDL_LDOUBLE // long double  precision float (80 or 128bit)
//  , GDL_COMPLEXLDBL // Complex long double
//
//  , GDL_ARBITRARY // arbitrary precision float
//  , GDL_COMPLEXABI // Complex arbitrary
//
//  , GDL_RATIONAL // arbitrary length rational
//  , GDL_COMPLEXRAT // Complex arbitrary length rational
};

// order of conversion precedence if two types are the same,
// the first type is used // used also by ArrayIndexT
const int DTypeOrder[]={
  -1, 	//GDL_UNDEF
  2, 	//GDL_BYTE
  3, 	//GDL_INT
  4, 	//GDL_LONG,	
  8, 	//GDL_FLOAT,	
  9, 	//GDL_DOUBLE,	
  20, 	//GDL_COMPLEX,	
  1, 	//GDL_STRING,	
  101, 	//GDL_STRUCT,	
  21, 	//GDL_COMPLEXDBL,	
  102, 	//GDL_PTR,		
  103, 	//GDL_OBJ, // must be highest number (see AdjustTypes... functions)
  3, 	//GDL_UINT,	
  4, 	//GDL_ULONG,
  5, 	//GDL_LONG64,
  5 	//GDL_ULONG64
  
  // not yet implemented
  ,6  //   , GDL_LONG128  // 128 bit integer
  ,6  //   , GDL_ULONG128 // unsigned 128 bit integer
    //   
  ,7  //   , GDL_LONGAB // arbitrary length int
  // ,7  //   , GDL_ULONGAR // arbitrary length unsigned int (pointless)
    //   
  ,10  //   , GDL_LDOUBLE // quad precision float (80 or 128bit)
  ,22  //   , GDL_COMPLEXLDBL // Complex quad
    // 
  ,11  //   , GDL_ARBITRARY // arbitrary precision float
  ,23  //   , GDL_COMPLEXAR // Complex arbitrary
    // 
  ,12  //   , GDL_RATIONAL // arbitrary length rational
  ,24  //   , GDL_COMPLEXRAT // Complex arbitrary length rational
};	


inline DType PromoteMatrixOperands( DType aTy, DType bTy)
{
  DType maxTy=(DTypeOrder[aTy] >= DTypeOrder[bTy])? aTy: bTy;
  if( maxTy == GDL_BYTE || maxTy == GDL_INT)
    return GDL_LONG;
  else if( maxTy == GDL_UINT)
    return GDL_ULONG;
  return maxTy;
}

inline DType PromoteComplexOperand( DType aTy, DType bTy)
{
  if((aTy == GDL_COMPLEX && bTy == GDL_DOUBLE) ||
     (bTy == GDL_COMPLEX && aTy == GDL_DOUBLE) )
    return GDL_COMPLEXDBL;
  return GDL_UNDEF;
}

namespace gdl_type_lookup {

const bool IsConvertableType[]={
  false, 	//GDL_UNDEF
  true, 	//GDL_BYTE
  true, 	//GDL_INT
  true, 	//GDL_LONG,	
  true, 	//GDL_FLOAT,	
  true, 	//GDL_DOUBLE,	
  true, 	//GDL_COMPLEX,	
  true, 	//GDL_STRING,	
  false, 	//GDL_STRUCT,	
  true, 	//GDL_COMPLEXDBL,	
  false, 	//GDL_PTR,		
  false, 	//GDL_OBJ,
  true, 	//GDL_UINT,	
  true, 	//GDL_ULONG,
  true, 	//GDL_LONG64,
  true  	//GDL_ULONG64
};	
const bool IsNumericType[]={
  false, 	//GDL_UNDEF
  true, 	//GDL_BYTE
  true, 	//GDL_INT
  true, 	//GDL_LONG,	
  true, 	//GDL_FLOAT,	
  true, 	//GDL_DOUBLE,	
  true, 	//GDL_COMPLEX,	
  false, 	//GDL_STRING,	
  false, 	//GDL_STRUCT,	
  true, 	//GDL_COMPLEXDBL,	
  false, 	//GDL_PTR,		
  false, 	//GDL_OBJ,
  true, 	//GDL_UINT,	
  true, 	//GDL_ULONG,
  true, 	//GDL_LONG64,
  true  	//GDL_ULONG64
};	
const bool IsIntType[]={
  false, 	//GDL_UNDEF
  true, 	//GDL_BYTE
  true, 	//GDL_INT
  true, 	//GDL_LONG,	
  false, 	//GDL_FLOAT,	
  false, 	//GDL_DOUBLE,	
  false, 	//GDL_COMPLEX,	
  false, 	//GDL_STRING,	
  false, 	//GDL_STRUCT,	
  false, 	//GDL_COMPLEXDBL,	
  false, 	//GDL_PTR,		
  false, 	//GDL_OBJ,
  true, 	//GDL_UINT,	
  true, 	//GDL_ULONG,
  true, 	//GDL_LONG64,
  true  	//GDL_ULONG64
};	
const bool IsRealType[]={
  false, 	//GDL_UNDEF
  true, 	//GDL_BYTE
  true, 	//GDL_INT
  true, 	//GDL_LONG,	
  true, 	//GDL_FLOAT,	
  true, 	//GDL_DOUBLE,	
  false, 	//GDL_COMPLEX,	
  false, 	//GDL_STRING,	
  false, 	//GDL_STRUCT,	
  false, 	//GDL_COMPLEXDBL,	
  false, 	//GDL_PTR,		
  false, 	//GDL_OBJ,
  true, 	//GDL_UINT,	
  true, 	//GDL_ULONG,
  true, 	//GDL_LONG64,
  true  	//GDL_ULONG64
};	
const bool IsNonPODType[]={
  false, 	//GDL_UNDEF
  false, 	//GDL_BYTE
  false, 	//GDL_INT
  false, 	//GDL_LONG,	
  false, 	//GDL_FLOAT,	
  false, 	//GDL_DOUBLE,	
  true, 	//GDL_COMPLEX,	
  true, 	//GDL_STRING,	
  true, 	//GDL_STRUCT,	
  true, 	//GDL_COMPLEXDBL,	
  true, 	//GDL_PTR, nonPOD due to reference counting
  true, 	//GDL_OBJ, nonPOD due to reference counting
  false, 	//GDL_UINT,	
  false, 	//GDL_ULONG,
  false, 	//GDL_LONG64,
  false 	//GDL_ULONG64
};
 const bool IsLongLongType[] = {
  false, //GDL_UNDEF
  false, //GDL_BYTE
  false, //GDL_INT
  false, //GDL_LONG,	
  false, //GDL_FLOAT,	
  true, //GDL_DOUBLE,	
  false, //GDL_COMPLEX,	
  false, //GDL_STRING,	
  false, //GDL_STRUCT,	
  true, //GDL_COMPLEXDBL,	
  false, //GDL_PTR,		
  false, //GDL_OBJ,
  false, //GDL_UINT,	
  false, //GDL_ULONG,
  true, //GDL_LONG64,
  true //GDL_ULONG64
 };
 const bool IsUnsignedType[] = {
  false, //GDL_UNDEF
  true, //GDL_BYTE
  false, //GDL_INT
  false, //GDL_LONG,	
  false, //GDL_FLOAT,	
  false, //GDL_DOUBLE,	
  false, //GDL_COMPLEX,	
  false, //GDL_STRING,	
  false, //GDL_STRUCT,	
  false, //GDL_COMPLEXDBL,	
  false, //GDL_PTR,		
  false, //GDL_OBJ,
  true, //GDL_UINT,	
  true, //GDL_ULONG,
  false, //GDL_LONG64,
  true //GDL_ULONG64
 };  
} //namespace gdl_type_lookup 

inline bool NonPODType( DType t)
{
  return gdl_type_lookup::IsNonPODType[ t];
//   return (t == GDL_COMPLEX) || (t == GDL_COMPLEXDBL) || (t == GDL_STRING) || (t == GDL_STRUCT);
}
inline bool IntType( DType t)
{
  return gdl_type_lookup::IsIntType[ t];
//   int o = DTypeOrder[ t];
//   return (o >= 2 && o <= 5);
}
inline bool FloatType( DType t)
{
  return (t == GDL_FLOAT || t == GDL_DOUBLE);
}
inline bool RealType( DType t) // Float or Int
{
  return gdl_type_lookup::IsRealType[ t];
//   int o = DTypeOrder[ t];
//   return (o >= 2 && o <= 9);
}
inline bool ComplexType( DType t)
{
  return (t == GDL_COMPLEX || t == GDL_COMPLEXDBL);
}
inline bool NumericType( DType t) // Float or Int or Complex
{
  return gdl_type_lookup::IsNumericType[ t];
}
inline bool ConvertableType( DType t) // everything except Struct, Ptr, Obj
{
  return gdl_type_lookup::IsConvertableType[ t];
}
inline bool LongLongType( DType t) // POD but 64 bits
{
  return gdl_type_lookup::IsLongLongType[ t];
}
inline bool UnsignedType( DType t) // integer, unsigned type
{
  return gdl_type_lookup::IsUnsignedType[ t];
}
class   BaseGDL;
class   ArrayIndexListT;
//class   ExprListT;

//struct SpDULong;
//template<class> class Data_;

struct ForLoopInfoT;

void breakpoint();

// --- SA: MemStats stuff

class MemStats
{

private:

  // SizeT has architecture-dependant size (32/64 bit)
  static SizeT NumAlloc, NumFree, HighWater, Current;

#if (!defined(HAVE_MALLINFO) && !defined(HAVE_MALLINFO2))
#  if (!defined(HAVE_MALLOC_ZONE_STATISTICS) || !defined(HAVE_MALLOC_MALLOC_H))
#    if defined(HAVE_SBRK)
  static char* StartOfMemory;
#    endif
#  endif
#endif

public:

  ~MemStats() 
  { 
    NumFree++; 
#if defined(HAVE_MALLOC_ZONE_STATISTICS) && defined(HAVE_MALLOC_MALLOC_H)
// - the sbrk(0) does not give any meaningfull info for HIGHWATER
// - using mallinfo() frequently gives a large performace loss
// avoid doing this here, explains the slowness on OSX  //   UpdateCurrent(); // updates the highwater mark 
#endif
  }

  MemStats() { NumAlloc++; }

  static SizeT GetNumAlloc() { return NumAlloc; }
  static SizeT GetNumFree() { return NumFree; }

  static SizeT GetCurrent() 
  { 
    UpdateCurrent(); // updates the highwater mark
    return Current; 
  }

  // returns and resets the highwater mark (called from HELP and MEMORY())
  static SizeT GetHighWater() 
  { 
    // Current can be safely used as a temporary variable here
    Current = HighWater;
    HighWater = 0;
    return Current;
  }

  // returns current memory usage and updates the highwater mark
  static void UpdateCurrent() ;


};

// ---
class AllIxBaseT;

class BaseGDL: private MemStats
{
public:
  static DInterpreter* interpreter;

protected:
  dimension dim;
  bool shared=false;
  void (* callback_function)(unsigned char*)=NULL;

public:
  // type of initialization
  enum InitType {
    ZERO=0,
    NOZERO,
    INDGEN,
    INIT,
    NOALLOC
  };

  enum Convert2Mode {
    CONVERT=1,
    COPY=2,
    COPY_BYTE_AS_INT=(4|2),    // for GDL_STRING function
    THROWIOERROR=8,            // for DStringGDL::Convert2()
    COPY_THROWIOERROR=(8|2),   // for DStringGDL::Convert2()
    CONVERT_THROWIOERROR=(8|1) // for DStringGDL::Convert2() (SA: used in NEWTON())
  };

  enum IOMode {
    FIXED=0,
    SCIENTIFIC,
    BIN=2,
    OCT=8,
    DEC=10,
    HEX=16,
    HEXL, // lower case characters
    AUTO
  };

  enum Cal_IOMode {
      WRITE=-2,
      COMPUTE=-1,
      DEFAULT=0,
      CMOA,
      CMoA,
      CmoA,
      CMOI,
      CDI,
      CYI,
      CHI,
      ChI,
      CMI,
      CSI,
      CSF,
      CDWA,
      CDwA,
      CdwA,
      CAPA,
      CApA,
      CapA,
      STRING
  };
  // FIRST VIRTUAL FUNCTION'S GDL_OBJ FILE CONTAINS ALSO THE VTABLE
  // therefore it must be defined non-inline (g++)
  virtual ~BaseGDL(); // defined in basegdl.cpp

  BaseGDL();//: dim() {} 
  explicit BaseGDL(const dimension& dim_);//: dim(dim_) {} 

  //callback function when deleted
  void SetCallbackFunction(void (* ptr)(unsigned char*)){callback_function=ptr;}
  void (*GetCallbackFunction())(unsigned char*){return callback_function;}
  //shared
  bool IsShared() {return shared;}
  void SetShared() {shared=true;} //used only with shmvar variables, explicitely in theur creation.
  // provide access to dim member
  inline const dimension& Dim()   const      { return dim;}
  inline SizeT    Dim(SizeT d)    const      { return dim[d];}
//   inline SizeT*   Dim0Address()              { return dim.Dim0Address();}
  inline SizeT    Stride(SizeT d) const      { return dim.Stride(d);}
  inline void     Purge()                    { dim.Purge();}
  inline SizeT    Rank()          const      { return dim.Rank();}
  inline SizeT    EquivalentRank()          const      { return dim.EquivalentRank();}
  inline void     SetDim(const dimension& d) { dim=d;}
  inline void     MakeArrayFromScalar()      { dim.MakeArrayFromScalar();} 

  friend std::istream& operator>>(std::istream& i, BaseGDL& data_)
  {
    throw GDLException("Variable is of type UNDEF.");
    return i;
  }

//  private:
  virtual BaseGDL& operator=(const BaseGDL& right);
//  public:
 
  virtual void InitFrom(const BaseGDL& right); // for structs

  // virtual functions
  virtual bool IsAssoc() const;
  virtual BaseGDL* AssocVar( int, SizeT);

  virtual SizeT N_Elements() const; // number of elements
  virtual SizeT Size() const;       // size (= N_Elements, but 0 for BaseGDL)
  virtual SizeT NBytes() const;     // total bytes of data
  virtual SizeT ToTransfer() const; // elements to transfer
  virtual SizeT Sizeof() const;     // size of scalar data

  virtual int HashCompare( BaseGDL* p2) const;
  
  virtual BaseGDL* Transpose( DUInt* perm);
  virtual BaseGDL* Rotate( DLong dir);
  virtual void Reverse( DLong dim);
  virtual BaseGDL* DupReverse( DLong dim);

  virtual void MinMax( DLong* minE, DLong* maxE, 
		       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
               SizeT start = 0, SizeT stop = 0, SizeT step = 1, DLong valIx = -1, bool useAbs = false);

  virtual void Clear();
  virtual void Construct();
  virtual void ConstructTo0();
  virtual void Destruct();
  virtual std::ostream& Write( std::ostream& os, bool swapEndian, 
			       bool compress, XDR *xdrs);
  virtual std::istream& Read( std::istream& is, bool swapEndian, 
			      bool compress, XDR *xdrs);

  virtual std::ostream& ToStream(std::ostream& o, SizeT width = 0, 
			    SizeT* actPosPtr = NULL);
  virtual std::istream& FromStream(std::istream& i);

  virtual bool Greater(SizeT i1, SizeT i2) const; // comp 2 elements
  virtual bool Equal(SizeT i1, SizeT i2) const; // comp 2 elements

  virtual BaseGDL* CShift( DLong d) const; // circular shift
  virtual BaseGDL* CShift( DLong d[MAXRANK]) const; // circular shift multi dim

//  virtual bool OutOfRangeOfInt() const;

  virtual bool Scalar() const;
  virtual bool StrictScalar() const;
  virtual DType   Type() const;
  virtual const std::string& TypeStr() const;
  virtual bool          EqType( const BaseGDL*) const;
  virtual void* DataAddr();// SizeT elem=0);
  virtual BaseGDL* New( const dimension& dim_, InitType noZero=ZERO) const;
  virtual BaseGDL* NewResult() const;
  virtual BaseGDL* Dup() const;
//   virtual BaseGDL* Dup( char*) const; 
  virtual BaseGDL* Convert2( DType destTy, Convert2Mode mode=CONVERT);
  virtual BaseGDL* GetTag() const; 
  virtual BaseGDL* GetInstance() const;
  virtual BaseGDL* GetEmptyInstance() const;
  virtual BaseGDL* SetBuffer( const void* b);
  virtual void     SetBufferSize( SizeT s);
  virtual int Scalar2Index(SizeT& ret) const;
  virtual int Scalar2RangeT(RangeT& ret) const;
  virtual SizeT GetAsIndex( SizeT i) const;
  virtual SizeT GetAsIndexStrict( SizeT i) const;
  virtual RangeT LoopIndex() const;
  virtual DDouble HashValue() const;
  
  virtual bool True();
  virtual bool False();
  virtual bool LogTrue();
  virtual bool LogTrue( SizeT ix);
  virtual void Where(DLong* &ret, SizeT &passed_count, bool comp, DLong* &comp_ret) ;
  virtual void Where(DLong64* &ret, SizeT &passed_count, bool comp, DLong64* &comp_ret) ;
  virtual BaseGDL* LogNeg();
  virtual int Sgn(); // -1,0,1
  virtual bool Equal( BaseGDL*) const;
  virtual bool EqualNoDelete( const BaseGDL*) const;
  virtual bool ArrayEqual( BaseGDL*);
  virtual bool ArrayNeverEqual( BaseGDL*);
  
  // for statement compliance (int types , float types scalar only)
  virtual bool ForCheck( BaseGDL**, BaseGDL** = NULL);
  virtual bool ForCondUp( BaseGDL*);
  virtual bool ForCondDown( BaseGDL*);
  virtual bool ForAddCondUp( BaseGDL* loopInfo);
//   virtual bool ForAddCondUp( ForLoopInfoT& loopInfo);
//  virtual bool ForAddCondDown( ForLoopInfoT& loopInfo);
//   virtual bool ForCondUpDown( BaseGDL*);
  virtual void ForAdd( BaseGDL* add=NULL);
  virtual BaseGDL* CatArray( ExprListT& exprList,
			     const SizeT catRank, 
			     const SizeT rank); 
  virtual BaseGDL* Index( ArrayIndexListT* ixList); 
  //  virtual BaseGDL* Abs() const; 

  // return a new type of itself
  virtual BaseGDL* NewIx( SizeT ix);
  virtual BaseGDL* NewIx( BaseGDL* ix, bool strict);
  virtual BaseGDL* NewIx( AllIxBaseT* ix, const dimension* dIn);
  virtual BaseGDL* NewIxFrom( SizeT s);
  virtual BaseGDL* NewIxFrom( SizeT s, SizeT e);
  virtual BaseGDL* NewIxFromStride( SizeT s, SizeT stride);
  virtual BaseGDL* NewIxFromStride( SizeT s, SizeT e, SizeT stride);

  // library functions
  virtual BaseGDL* Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias,
 			   bool center, bool normalize, int edgeMode,
                                bool doNan, BaseGDL* missing, bool doMissing,
                                BaseGDL* invalid, bool doInvalid, DDouble edgeVal);
  virtual BaseGDL* Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing);
  virtual BaseGDL* Rebin( const dimension& newDim, bool sample);
  // for STRUCT_ASSIGN
  virtual void Assign( BaseGDL* src, SizeT nEl);
  virtual void AssignIndexedValue( BaseGDL* src, SizeT index);

  virtual BaseGDL* Log();              
  virtual BaseGDL* LogThis();
  virtual BaseGDL* Log10();              
  virtual BaseGDL* Log10This();

  // defined in basic_op.cpp
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
  virtual BaseGDL* PowInt( BaseGDL* r);

  virtual BaseGDL* AndOpS( BaseGDL* r);    
  virtual BaseGDL* AndOpInvS( BaseGDL* r); 
  virtual BaseGDL* OrOpS( BaseGDL* r);    
  virtual BaseGDL* OrOpInvS( BaseGDL* r); 
  virtual BaseGDL* XorOpS( BaseGDL* r);    
  virtual BaseGDL* AddS( BaseGDL* r);      
  virtual BaseGDL* AddInvS( BaseGDL* r);
  virtual BaseGDL* SubS( BaseGDL* r);
  virtual BaseGDL* SubInvS( BaseGDL* r);   
  virtual BaseGDL* LtMarkS( BaseGDL* r);   
  virtual BaseGDL* GtMarkS( BaseGDL* r);   
  virtual BaseGDL* MultS( BaseGDL* r);   
  virtual BaseGDL* DivS( BaseGDL* r);
  virtual BaseGDL* DivInvS( BaseGDL* r);   
  virtual BaseGDL* ModS( BaseGDL* r);      
  virtual BaseGDL* ModInvS( BaseGDL* r);   
  virtual BaseGDL* PowS( BaseGDL* r);      
  virtual BaseGDL* PowInvS( BaseGDL* r);   


  
  
  
// the New functions
// defined in basic_op_new.cpp
// results go into a new DataT

  virtual BaseGDL* AndOpNew( BaseGDL* r);
  virtual BaseGDL* AndOpInvNew( BaseGDL* r);
  virtual BaseGDL* OrOpNew( BaseGDL* r);
  virtual BaseGDL* OrOpInvNew( BaseGDL* r);
  virtual BaseGDL* XorOpNew( BaseGDL* r);
//   virtual BaseGDL* EqOpNew( BaseGDL* r);
//   virtual BaseGDL* NeOpNew( BaseGDL* r);
//   virtual BaseGDL* LeOpNew( BaseGDL* r);
//   virtual BaseGDL* GeOpNew( BaseGDL* r);
//   virtual BaseGDL* LtOpNew( BaseGDL* r);
//   virtual BaseGDL* GtOpNew( BaseGDL* r);
  virtual BaseGDL* AddNew( BaseGDL* r);      // implemented
  virtual BaseGDL* AddInvNew( BaseGDL* r);      // implemented
  virtual BaseGDL* SubNew( BaseGDL* r);
  virtual BaseGDL* SubInvNew( BaseGDL* r);
  virtual BaseGDL* LtMarkNew( BaseGDL* r);
  virtual BaseGDL* GtMarkNew( BaseGDL* r);
  virtual BaseGDL* MultNew( BaseGDL* r);   // implemented
  virtual BaseGDL* DivNew( BaseGDL* r);
  virtual BaseGDL* DivInvNew( BaseGDL* r);
  virtual BaseGDL* ModNew( BaseGDL* r);
  virtual BaseGDL* ModInvNew( BaseGDL* r);
  virtual BaseGDL* PowNew( BaseGDL* r);
  virtual BaseGDL* PowInvNew( BaseGDL* r);
  virtual BaseGDL* PowIntNew( BaseGDL* r);   // implemented

  virtual BaseGDL* AndOpSNew( BaseGDL* r);
  virtual BaseGDL* AndOpInvSNew( BaseGDL* r);
  virtual BaseGDL* OrOpSNew( BaseGDL* r);
  virtual BaseGDL* OrOpInvSNew( BaseGDL* r);
  virtual BaseGDL* XorOpSNew( BaseGDL* r);
  virtual BaseGDL* AddSNew( BaseGDL* r);         // implemented
  virtual BaseGDL* AddInvSNew( BaseGDL* r);    // implemented
  virtual BaseGDL* SubSNew( BaseGDL* r);
  virtual BaseGDL* SubInvSNew( BaseGDL* r);
  virtual BaseGDL* LtMarkSNew( BaseGDL* r);
  virtual BaseGDL* GtMarkSNew( BaseGDL* r);
  virtual BaseGDL* MultSNew( BaseGDL* r);      // implemented
  virtual BaseGDL* DivSNew( BaseGDL* r);
  virtual BaseGDL* DivInvSNew( BaseGDL* r);
  virtual BaseGDL* ModSNew( BaseGDL* r);
  virtual BaseGDL* ModInvSNew( BaseGDL* r);
  virtual BaseGDL* PowSNew( BaseGDL* r);
  virtual BaseGDL* PowInvSNew( BaseGDL* r);


  
  //  virtual BaseGDL* PowInvNew( BaseGDL* r);
  virtual BaseGDL* MatrixOp( BaseGDL* r, bool atranspose=false, bool btranspose=false);
  virtual void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset);
  virtual void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList);
  virtual void AssignAt( BaseGDL* srcIn);

  virtual void AssignAtIx( RangeT ix, BaseGDL* srcIn);

  virtual void DecAt( ArrayIndexListT* ixList);
  virtual void IncAt( ArrayIndexListT* ixList);
  virtual void Dec();
  virtual void Inc();
  virtual void InsertAt(  SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);

  // virtual formatting output functions
  virtual SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width, int code);
  virtual SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width, int prec, const int code=0, const BaseGDL::IOMode oM = FIXED); 
  virtual SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, int minN, int code=0, BaseGDL::IOMode oM = DEC);
  virtual SizeT OFmtCal( std::ostream* os, SizeT offs, SizeT num, int width, int minN, const std::string &s, int code=0, BaseGDL::Cal_IOMode oM = DEFAULT);
  virtual SizeT IFmtA( std::istream* is, SizeT offset, SizeT num, int width);
  virtual SizeT IFmtF( std::istream* is, SizeT offs, SizeT num, int width);
  virtual SizeT IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
			BaseGDL::IOMode oM = DEC);
  virtual SizeT IFmtCal( std::istream* is, SizeT offs, SizeT r, int width, BaseGDL::Cal_IOMode cMode);

#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

  virtual PyObject* ToPython();
#endif

  virtual bool Test2() {return false;}
  
};


// ExprListT deletes all members upon own destruction

// old slow version (up to GDL 0.9.1):
// class ExprListT: public std::vector<BaseGDL*>
// {
// public:
// 	ExprListT()
// 	{
// 	this->reserve(ExprListDefaultLength);
// 	}
// 	~ExprListT()
// 	{
// 	for( ExprListT::iterator i=this->begin(); i!=this->end(); ++i)
// 		delete *i;
// 	}
// };


struct ForLoopInfoT
{
  BaseGDL*  endLoopVar; // the source for foreach as well
  BaseGDL*  loopStepVar;
  DLong     foreachIx;
  bool (BaseGDL::*IncrOrDec)(BaseGDL*); //for FORSTEPLoop: method pointer saves if test on increment sign inside loop
//   bool      isHash; // only used in FOREACH_INDEXNode::Run() and FOREACH_INDEX_LOOPNode::Run()

  ForLoopInfoT()
  : endLoopVar(NULL)
  , loopStepVar(NULL)
  , foreachIx(-1)
  {}
  ~ForLoopInfoT()
  {
	  delete endLoopVar;
	  delete loopStepVar;
  }
  void Init()
  {
	  endLoopVar = NULL;
	  loopStepVar = NULL;
	  foreachIx = -1;
  }
  void Clear()
  {
	  delete endLoopVar;
	  delete loopStepVar;
  }
  void ClearInit()
  {
	  delete endLoopVar;
	  endLoopVar = NULL;
	  delete loopStepVar;
	  loopStepVar = NULL;
  }
};

// before NullGDL instance must not be deleted, now this is fine (overloaded operators new and delete)
// inline void GDLDelete( BaseGDL* toDelete) { delete toDelete;}
void GDLDelete( BaseGDL* toDelete);
 
enum ThreadPoolType {
    TP_DEFAULT=0, //the same as IDL, reserved for routines that use the thread pool, ideally check the special thread pool keywords.
    TP_ARRAY_INITIALISATION, // used by GDL array initialisation (new, convert, gdlarray): probably needs som special tuning
    TP_MEMORY_ACCESS, // concurrent memory access, probably needs to be capped to preserve bandwidth 
    TP_CPU_INTENSIVE  // benefit from max number of threads
  };

extern  int GDL_NTHREADS;
int parallelize(SizeT n, int modifier=TP_DEFAULT);

#endif

