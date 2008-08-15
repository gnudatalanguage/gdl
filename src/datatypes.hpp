/***************************************************************************
           datatypes.hpp  -  defines all data types of GDL except DStructGDL
                             -------------------
    begin                : Tue Feb 13 2002
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


#ifndef DATATYPES_HPP_
#define DATATYPES_HPP_

#include <iomanip>
#include <sstream>
//#include <vector>
#include <valarray>
#include <string>

//#include <complex>
#include <deque>

#include "typedefs.hpp"
#include "basegdl.hpp"
#include "typetraits.hpp"

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
// by default intel C++ defines __GNUC__
#pragma interface
#endif

const size_t multiAlloc = 100;

template<class Sp>
class Data_: public Sp
{
public:
  typedef typename Sp::Ty    Ty;
  typedef Sp                 Traits;

#ifdef _MSC_VER
public: // MSC cannot handle friend template specialization properly
#else
private:
#endif

  typedef typename Sp::DataT DataT;
  DataT                      dd; // the data

public:
	// memory management optimization
static std::deque< void*> freeList;

	// operator new and delete
static 	void* operator new( size_t bytes);
static	void operator delete( void *ptr);

  //structors
  ~Data_();

  // default
  Data_();

  // scalar
  Data_(const Ty& d_);

  // new array, no zero or indgen
  Data_(const dimension& dim_,  BaseGDL::InitType iT);
  
  // new array, zero fields
  Data_(const dimension& dim_);

  // new array from Ty[], one dimensional
  Data_( const Ty* p, SizeT nEl);

  // new array from DataT
  Data_(const dimension& dim_, const DataT& dd_):
    Sp( dim_), dd( dd_) {}

  // c-i 
  Data_(const Data_& d_): Sp(d_.dim), dd(d_.dd) {}

  // operators
  // assignment. 
  Data_& operator=(const BaseGDL& right);

  // one dim array access (unchecked)
  inline Ty& operator[] (const SizeT d1) { return dd[d1];}
  inline const Ty& operator[] (const SizeT d1) const { return dd[d1];}
  //  Ty& operator[] (const SizeT d1) { return dd[d1];}

  template<class Sp2> 
  friend std::istream& operator>>(std::istream& i, Data_<Sp2>& data_); 
  
  // valarrays cannot be resized (without loosing data)
  //  inline DataT& Resize( SizeT n);

//   // note that min and max these are not defined in BaseGDL
//   Ty min() const;// { return dd.min();}
//   Ty max() const;// { return dd.max();}

  bool Greater(SizeT i1, SizeT i2) const; // comp 2 elements
  bool Equal(SizeT i1, SizeT i2) const; // comp 2 elements

  BaseGDL* CShift( DLong d); 
  BaseGDL* CShift( DLong d[MAXRANK]); 

  BaseGDL* Transpose( DUInt* perm);
  BaseGDL* Rotate( DLong dir);

  // for use by MIN and MAX functions
  void MinMax( DLong* minE, DLong* maxE, BaseGDL** minVal, BaseGDL** maxVal,
	       bool omitNaN);
  
  bool EqType( const BaseGDL* r) const;

  void* DataAddr( SizeT elem=0);

  Ty Sum() const;

  SizeT N_Elements() const;
  SizeT Size() const;
  SizeT NBytes() const;
  SizeT ToTransfer() const; // IO transfer count
  SizeT Sizeof() const;

  void Clear();
  void Construct();     // construction (for DStructGDL)
  void ConstructTo0();  // construction (for DStructGDL)
  void Destruct(); // destruction (for DStructGDL)

  BaseGDL* SetBuffer( const void* b);
  void SetBufferSize( SizeT s);

  BaseGDL* AssocVar( int, SizeT);

  std::ostream& ToStream(std::ostream& o, SizeT width = 0, 
			 SizeT* actPosPtr = NULL);
  std::istream& FromStream(std::istream& i);
 
  // used by the interpreter
  int Scalar2index( SizeT& st) const;
  SizeT LoopIndex() const; 
  
  // make a duplicate on the heap
  Data_* Dup() const { return new Data_(*this);}
//   // make a duplicate at loc
//   Data_* Dup( void* loc) const { return ::new ( loc) Data_(*this);}

  bool Scalar() const { return (dd.size() == 1);}
  bool StrictScalar() const { return (this->Rank() == 0);}

  bool Scalar(Ty& s) const {
    if( dd.size() != 1) return false;
    s=dd[0];
    return true; }
  bool StrictScalar(Ty& s) const {
    if( this->Rank() != 0) return false;
    s=dd[0];
    return true; }

  Data_* New( const dimension& dim_, BaseGDL::InitType noZero=BaseGDL::ZERO);

  // convert *this to other 'destTy'
  BaseGDL* Convert2( DType destTy, 
		     BaseGDL::Convert2Mode=BaseGDL::CONVERT);

  // not all compilers can handle template friend member functions
#if defined( TEMPLATE_FRIEND_OK_) || (__GNUC__ >= 4)
  // make all other Convert2 functions friends
  template<class Sp2>  
  friend BaseGDL* Data_<Sp2>::Convert2( DType destTy, 
					BaseGDL::Convert2Mode);
#else
  // this explicit version does not work with GCC 4.0
  friend BaseGDL* Data_<SpDByte>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDInt>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDUInt>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDLong>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDULong>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDLong64>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDULong64>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDFloat>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDDouble>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDString>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDPtr>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDObj>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDComplex>::Convert2( DType destTy, BaseGDL::Convert2Mode);
  friend BaseGDL* Data_<SpDComplexDbl>::Convert2( DType destTy, BaseGDL::Convert2Mode);

#endif

  bool True();
  bool False();
  bool LogTrue();
  bool LogTrue( SizeT ix);
  DLong* Where( bool comp, SizeT& count);
  Data_<SpDByte>* LogNeg();
  int  Sgn(); // returns -1,0,1
  bool Equal( BaseGDL*);
  bool ArrayEqual( BaseGDL*);
  void ForCheck( BaseGDL**, BaseGDL** =NULL);
  bool ForCondUp( BaseGDL*);
  bool ForCondDown( BaseGDL*);
  //  bool ForCondUpDown( BaseGDL*);
  void ForAdd();
  void ForAdd( BaseGDL* add);

  BaseGDL* Abs() const;  

  BaseGDL* Convol( BaseGDL* kIn, BaseGDL* scaleIn, 
		   bool center, int edgeMode);
  BaseGDL* Rebin( const dimension& newDim, bool sample);

  void Assign( BaseGDL* src, SizeT nEl);

  Data_*   Log();
  void     LogThis();

  Data_*   Log10();
  void     Log10This();

  BaseGDL* UMinus(); // UMinus for SpDString returns float
  Data_*   NotOp();
  Data_*   AndOp( BaseGDL* r);
  Data_*   AndOpInv( BaseGDL* r);
  Data_*   OrOp( BaseGDL* r);
  Data_*   OrOpInv( BaseGDL* r);
  Data_*   XorOp( BaseGDL* r);
  Data_*   Add( BaseGDL* r);
  Data_*   AddInv( BaseGDL* r);
  Data_*   Sub( BaseGDL* r);
  Data_*   SubInv( BaseGDL* r);
  Data_*   GtMark( BaseGDL* r);
  Data_*   LtMark( BaseGDL* r);
  Data_*   Mult( BaseGDL* r);
  Data_*   Div( BaseGDL* r);
  Data_*   DivInv( BaseGDL* r);
  Data_*   Mod( BaseGDL* r);
  Data_*   ModInv( BaseGDL* r);
  Data_*   Pow( BaseGDL* r);
  Data_*   PowInv( BaseGDL* r);
  Data_*   PowInt( BaseGDL* r);      
  Data_*   PowIntNew( BaseGDL* r);   
  Data_*   MatrixOp( BaseGDL* r);

  Data_*   AndOpS( BaseGDL* r);
  Data_*   AndOpInvS( BaseGDL* r);
  Data_*   OrOpS( BaseGDL* r);
  Data_*   OrOpInvS( BaseGDL* r);
  Data_*   XorOpS( BaseGDL* r);
  Data_*   AddS( BaseGDL* r);
  Data_*   AddInvS( BaseGDL* r);
  Data_*   SubS( BaseGDL* r);
  Data_*   SubInvS( BaseGDL* r);
  Data_*   GtMarkS( BaseGDL* r);
  Data_*   LtMarkS( BaseGDL* r);
  Data_*   MultS( BaseGDL* r);
  Data_*   DivS( BaseGDL* r);
  Data_*   DivInvS( BaseGDL* r);
  Data_*   ModS( BaseGDL* r);
  Data_*   ModInvS( BaseGDL* r);
  Data_*   PowS( BaseGDL* r);
  Data_*   PowInvS( BaseGDL* r);
  
  Data_* AndOpNew( BaseGDL* r);    // create new result var      
//   Data_* AndOpInvNew( BaseGDL* r); // create new result var      
  Data_* OrOpNew( BaseGDL* r);    
//   Data_* OrOpInvNew( BaseGDL* r); 
  Data_* XorOpNew( BaseGDL* r);    
  Data_* AddNew( BaseGDL* r);      
//   Data_* AddInvNew( BaseGDL* r);      
  Data_* SubNew( BaseGDL* r);      
//   Data_* SubInvNew( BaseGDL* r);   
  Data_* MultNew( BaseGDL* r);   
  Data_* DivNew( BaseGDL* r);      
//   Data_* DivInvNew( BaseGDL* r);   
  Data_* ModNew( BaseGDL* r);      
//   Data_* ModInvNew( BaseGDL* r);   
  Data_* PowNew( BaseGDL* r);     
//   Data_* PowInvNew( BaseGDL* r);  

  Data_<SpDByte>* EqOp( BaseGDL* r);
  Data_<SpDByte>* NeOp( BaseGDL* r);
  Data_<SpDByte>* GtOp( BaseGDL* r);
  Data_<SpDByte>* GeOp( BaseGDL* r);
  Data_<SpDByte>* LtOp( BaseGDL* r);
  Data_<SpDByte>* LeOp( BaseGDL* r);

  // used by interpreter, calls CatInsert
  Data_* CatArray( ExprListT& exprList, const SizeT catRank, 
		   const SizeT rank);

  // assigns srcIn to this at ixList, if ixList is NULL does linear copy
  // assumes: ixList has this already set as variable
  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset);
  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList);
  void AssignAt( BaseGDL* srcIn);

  void AssignAtIx( SizeT ix, BaseGDL* srcIn);

  // decrement (--) and increment (++) operators
  void DecAt( ArrayIndexListT* ixList);
  void IncAt( ArrayIndexListT* ixList);
  void Dec();
  void Inc();

  // used by AccessDescT for resolving, no checking is done
  // inserts srcIn[ ixList] at offset
  void InsertAt( SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);

  // returns (*this)[ ixList]
  Data_* Index( ArrayIndexListT* ixList);

  // return a new type of itself
  Data_* NewIx( SizeT ix);
  Data_* NewIx( BaseGDL* ix, bool strict);
  Data_* NewIx( AllIxT* ix, dimension* dIn);
  Data_* NewIxFrom( SizeT s);
  Data_* NewIxFrom( SizeT s, SizeT e);
  Data_* NewIxFromStride( SizeT s, SizeT stride);
  Data_* NewIxFromStride( SizeT s, SizeT e, SizeT stride);

  // binary input/output
  std::ostream& Write( std::ostream& os, bool swapEndian, bool compress,
		       XDR *xdrs);
  std::istream& Read( std::istream& os, bool swapEndian, bool compress,
		      XDR *xdrs);
  
  SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width);
  SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width, 
		int prec, BaseGDL::IOMode oM = BaseGDL::FIXED);
  SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, 
		int minN, BaseGDL::IOMode oM = BaseGDL::DEC);

  // formatting input functions
  SizeT IFmtA( std::istream* is, SizeT offset, SizeT num, int width); 
  SizeT IFmtF( std::istream* is, SizeT offs, SizeT num, int width); 
  SizeT IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
		BaseGDL::IOMode oM = BaseGDL::DEC);

#ifdef USE_PYTHON
public:
  PyObject* ToPython();
private:
  PyObject* ToPythonScalar();
#endif

private:

  // inserts srcIn at ixDim, used by AssignAt(...)
  // respects the exact structure
  void InsAt( Data_* srcIn, ArrayIndexListT* ixList, SizeT offset = 0);
  // used for concatenation, called from CatArray
  // assumes that everything is checked (see CatInfo)
  void CatInsert( const Data_* srcArr, const SizeT atDim, SizeT& at);
};

#include "specializations.hpp"

typedef Data_<SpDByte>       DByteGDL;
typedef Data_<SpDInt>        DIntGDL;
typedef Data_<SpDUInt>       DUIntGDL;
typedef Data_<SpDLong>       DLongGDL;
typedef Data_<SpDULong>      DULongGDL;
typedef Data_<SpDLong64>     DLong64GDL;
typedef Data_<SpDULong64>    DULong64GDL;
typedef Data_<SpDFloat>      DFloatGDL;
typedef Data_<SpDDouble>     DDoubleGDL;
typedef Data_<SpDString>     DStringGDL;
typedef Data_<SpDPtr>        DPtrGDL;
typedef Data_<SpDObj>        DObjGDL;
typedef Data_<SpDComplex>    DComplexGDL;
typedef Data_<SpDComplexDbl> DComplexDblGDL;

// DStructGDL defined separately

// on OS X isfinite is not defined
#if defined(__APPLE__) && defined(OLD_DARWIN) && !defined(isfinite)

#ifdef __cplusplus
extern "C" {
#endif
#define      isfinite( x )      ( ( sizeof ( x ) == sizeof(double) ) ?       \
                              __isfinited ( x ) :                            \
                                ( sizeof ( x ) == sizeof( float) ) ?         \
                              __isfinitef ( x ) :                            \
                              __isfinite  ( x ) )
#ifdef __cplusplus
}
#endif
#endif


#endif
