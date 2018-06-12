/***************************************************************************
                          dstructgdl.hpp  -  GDL struct datatype
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

#ifndef dstructgdl_hpp_
#define dstructgdl_hpp_

#include <string>
// #include <deque>

#include "typedefs.hpp"
#include "datatypes.hpp" // for friend declaration
//#include "typetraits.hpp"
#include "dstructdesc.hpp"

// NOTE: lot of the code together with Data_<...>::functions
// does not fit really into the template because of different
// constructor
// also here a vector is used instead of a valarray

class DStructGDL: public SpDStruct
{
public:
  typedef SpDStruct::Ty    Ty;
  typedef SpDStruct        Traits;

private:

  typedef SpDStruct::DataT DataT;
  
  //public:
  std::vector<BaseGDL*> typeVar;   // for accessing data
#ifdef USE_EIGEN  
  EIGEN_ALIGN16 DataT        dd; // the data
#else
  DataT                      dd; // the data
#endif
    
  void InitTypeVar( SizeT t)
  {
    typeVar[ t] = (*Desc())[ t]->GetEmptyInstance();
    typeVar[ t]->SetBufferSize( (*Desc())[ t]->N_Elements());
    // no initial SetBuffer here, done anyway in ConstructTag()
  }

  const char* Buf() const { return &dd[0];}
  char*       Buf()       { return &dd[0];}

public:

  static std::vector< void*> freeList;

  // operator new and delete
  static void* operator new( size_t bytes);
  static void operator delete( void *ptr);

  //structors
  ~DStructGDL(); 

  // default (needed for test instantiation)
  //  DStructGDL(): SpDStruct(), d() {}
  
  // new array (desc defined)
  DStructGDL( DStructDesc* desc_, const dimension& dim_)
    : SpDStruct( desc_, dim_)
    , typeVar( desc_->NTags())
    , dd( dim.NDimElements() * desc_->NBytes(), false) //,SpDStruct::zero)
  {
    dim.Purge();
    
    SizeT nTags = NTags();
    for( SizeT t=0; t < nTags; ++t)
      {
	InitTypeVar( t);
	ConstructTagTo0( t); 
      }
  }
  // new array (desc defined), don't init fields (only for New()) 
  // (non POD must be initialized (constructed) nevertheless)
  // or no memeory allocation
  DStructGDL( DStructDesc* desc_, const dimension& dim_, BaseGDL::InitType iT)
    : SpDStruct( desc_, dim_) 
    , typeVar( desc_->NTags())
    , dd( (iT == BaseGDL::NOALLOC) ? 0 : dim.NDimElements() * desc_->NBytes(),
	  false)
  {
    assert( iT == BaseGDL::NOZERO || iT == BaseGDL::NOALLOC);
    dim.Purge();

    if( iT != BaseGDL::NOALLOC)
      {
	SizeT nTags = NTags();
// 	SizeT nEl   = dim.N_Elements();
	for( SizeT t=0; t < nTags; ++t)
	  {
	    InitTypeVar( t);
	    ConstructTag( t);
	  }
      }
    else // iT == BaseGDL::NOALLOC
      {
	SizeT nTags = NTags();
	for( SizeT t=0; t < nTags; ++t)
	  {
	    InitTypeVar( t);
	  }
      }
  }

  // c-i (desc defined)
  // only called from Assoc_'s c-i
  DStructGDL(const DStructGDL& d_);

  // For creating new structs (always scalar)
  DStructGDL( DStructDesc* desc_)
    : SpDStruct(desc_, dimension(1))
    , typeVar()
    , dd()
  {
    assert( desc_->NTags() == 0);
//     SizeT nTags = NTags();
//     for( SizeT t=0; t < nTags; ++t)
//       {
// 	InitTypeVar( t);
//       }
  }
  // new struct (always scalar), creating new descriptor
  // intended for internal (C++) use to ease struct definition
  // ATTENTION: This can only be used for NAMED STRUCTS!
  // please use the normal constructor (above) for unnamed structs 
  DStructGDL( const std::string& name_);
  

  // operators

  // assignment. 
  DStructGDL& operator=(const BaseGDL& r)
  {
    assert( r.Type() == GDL_STRUCT);
    const DStructGDL& right = static_cast<const DStructGDL&>( r);

    assert( *Desc() == *right.Desc());

    assert( &right != this);
    if( &right == this) return *this; // self assignment

    this->dim = right.dim;

    SizeT nTags = NTags();
    SizeT nEl   = N_Elements();
    for( SizeT e=0; e < nEl; ++e)
    for( SizeT t=0; t < nTags; ++t)
      {
	*GetTag( t, e) = *right.GetTag( t, e);
      }

    return *this;
  }
  
  void InitFrom(const BaseGDL& r)
  {
    assert( r.Type() == GDL_STRUCT);
    const DStructGDL& right = static_cast<const DStructGDL&>( r);

    assert( *Desc() == *right.Desc());
    assert( &right != this);
    
    this->dim = right.dim;

    SizeT nTags = NTags();
    SizeT nEl   = N_Elements();
    for( SizeT e=0; e < nEl; ++e)
		for( SizeT t=0; t < nTags; ++t)
		{
			GetTag( t, e)->InitFrom( *right.GetTag( t, e));
		}
  }
 
  inline BaseGDL* operator[] (const SizeT d1) 
  { 
    return GetTag( d1 % NTags(), d1 / NTags());
  }
  inline const BaseGDL* operator[] (const SizeT d1) const
  { 
    return GetTag( d1 % NTags(), d1 / NTags());
  }

// void* DataAddr( SizeT elem) 
// { 
// return &(*this)[elem];
// }
void* DataAddr()// SizeT elem)
{ 
if( Buf() == NULL)
  throw GDLException("DStructGDL: Data not set.");
return Buf();
}//elem];}
void* DataAddr(SizeT tag)// SizeT elem)
{ 
if( dd.size() == 0) return typeVar[ t];
return Buf();
}//elem];}

  // used for named struct definition 
  // (GDLInterpreter, basic_fun (create_struct))
  void SetDesc( DStructDesc* newDesc); 

   DStructGDL* SetBuffer( const void* b);
  void SetBufferSize( SizeT s);

  DStructGDL* CShift( DLong d) const;
  DStructGDL* CShift( DLong d[MAXRANK]) const;

  // for use by MIN and MAX functions
  void MinMax( DLong* minE, DLong* maxE, 
	       BaseGDL** minVal, BaseGDL** maxVal, bool omitNaN,
           SizeT start, SizeT stop, SizeT step, DLong valIx, bool useAbs);

  //  const DType   Type()    const { return SpDStruct::t;}
  //  const std::string& TypeStr() const { return SpDStruct::str;}
  bool          EqType( const BaseGDL* r) const 
  { return (SpDStruct::t == r->Type());} 

  SizeT N_Elements() const 
  { 
    if( dd.size() == 0) return 1;
    return dd.size()/Sizeof();
  }
  SizeT Size() const { return N_Elements();}
  SizeT NBytes() const // for assoc function
  { 
    return (Sizeof() * N_Elements());
  }
  SizeT ToTransfer() const // number of elements for IO transfer  
  { 
    SizeT nB = 0;
    SizeT nTags=NTags();
    for( SizeT i=0; i < nTags; i++)
      {
	nB += GetTag( i)->ToTransfer();
      }
    return ( nB * N_Elements()); // *** error for string?
  }
SizeT NBytesToTransfer() // number of elements for IO transfer without padding 
{ 
  SizeT nB = 0;
  SizeT nTags=this->NTags();
  for( SizeT j=0; j < this->N_Elements(); j++) { //eventually with no error for variable-length strings (in output, input will never work)
    for( SizeT i=0; i < nTags; i++)
    {
      if (this->GetTag(i,j)->Type()==GDL_STRUCT) {
        DStructGDL* str= static_cast<DStructGDL*>(this->GetTag( i, j));
        nB += str->NBytesToTransfer();} else nB += this->GetTag( i, j)->NBytes();
    }
  }
  return nB ;
}
  SizeT Sizeof() const
  {
    return Desc()->NBytes();
//     SizeT nB = 0;
//     SizeT nTags=NTags();
//     for( SizeT i=0; i < nTags; i++)
//       {
// 	nB += desc->GetTag( &dd[0], i)->NBytes();
//       }
//     return nB;
  }
  SizeT SizeofTags() const
  {
     SizeT nB = 0;
     SizeT nTags=NTags();
     for( SizeT i=0; i < nTags; i++)
       {
 	       nB += this->GetTag(i)->NBytes();
       }
     return nB;
  }
private:
  void ClearTag( SizeT t)
  {
  if( dd.size() == 0)
	{
	typeVar[ t]->Clear();
	}
  else
  {
    char*    offs = Buf() + Desc()->Offset( t);
    BaseGDL* tVar  = typeVar[ t];
    SizeT step    = Desc()->NBytes();
    SizeT endIx = step * N_Elements();
    for( SizeT ix=0; ix<endIx; ix+=step)
      {
	tVar->SetBuffer( offs + ix)->Clear();
      }
  }
  }
  void ConstructTagTo0( SizeT t)
  {
    char*    offs = Buf() + Desc()->Offset( t);
    BaseGDL* tVar  = typeVar[ t];
    SizeT step = Desc()->NBytes();
    SizeT endIx = step * N_Elements();
    for( SizeT ix=0; ix<endIx; ix+=step)
      {
	tVar->SetBuffer( offs + ix)->ConstructTo0();
      }
  }
  void ConstructTag( SizeT t)
  {
    BaseGDL* tVar  = typeVar[ t];
    if( NonPODType( tVar->Type()))
      {
	char* offs = Buf() + Desc()->Offset( t);
	SizeT step = Desc()->NBytes();
	SizeT endIx = step * N_Elements();
	for( SizeT ix=0; ix<endIx; ix+=step)
	  {
	    tVar->SetBuffer( offs + ix)->Construct();
	  }
      }
    else
      tVar->SetBuffer( Buf() + Desc()->Offset( t));
      
  }
  void DestructTag( SizeT t)
  {
    BaseGDL* tVar  = typeVar[ t];
    if( NonPODType( tVar->Type()))
      {
	char* offs = Buf() + Desc()->Offset( t);
	SizeT step = Desc()->NBytes();
	SizeT endIx = step * N_Elements();
	for( SizeT ix=0; ix<endIx; ix+=step)
	  {
	    tVar->SetBuffer( offs + ix)->Destruct();
	  }
      }
  }

public:
  void Clear() 
  {
    SizeT nTags = NTags();
    for( SizeT t=0; t < nTags; t++)
      {
	ClearTag( t);
      }
  }
  void ConstructTo0() 
  {
    SizeT nTags = NTags();
    for( SizeT t=0; t < nTags; t++)
      {
	ConstructTagTo0( t);
      }
  }
  void Construct() 
  {
    SizeT nTags = NTags();
    for( SizeT t=0; t < nTags; t++)
      {
	ConstructTag( t);
      }
  }
  void Destruct()
  {
    SizeT nTags = NTags();
    for( SizeT t=0; t < nTags; t++)
      {
	DestructTag( t);
      }
  }

  // code in default_io.cpp
  BaseGDL* AssocVar( int, SizeT);

  // single element access. 
//   BaseGDL*& Get( SizeT tag, SizeT ix)
  BaseGDL* GetTag( SizeT t, SizeT ix)
  {
    if( dd.size() == 0) return typeVar[ t];
    return typeVar[ t]->SetBuffer( Buf() + Desc()->Offset( t, ix));
  }
  BaseGDL* GetTag( SizeT t)
  {
    if( dd.size() == 0) return typeVar[ t];
    return typeVar[ t]->SetBuffer( Buf() + Desc()->Offset( t));
  }
  const BaseGDL* GetTag( SizeT t, SizeT ix) const
  {
    if( dd.size() == 0) return typeVar[ t];
    return typeVar[ t]->SetBuffer( Buf() + Desc()->Offset( t, ix));
  }
  const BaseGDL* GetTag( SizeT t) const
  {
    if( dd.size() == 0) return typeVar[ t];
    return typeVar[ t]->SetBuffer( Buf() + Desc()->Offset( t));
  }

  // single tag access. 
  BaseGDL* Get( SizeT tag);

  //***
//   friend std::ostream& operator<<(std::ostream& o, DStructGDL& data_);
  friend std::istream& operator>>(std::istream& i, DStructGDL& data_);
  
  std::ostream& ToStream(std::ostream& o, SizeT width = 0, SizeT* actPosPtr = NULL);
  //  std::ostream& ToStream(std::ostream& o)
  //  { o << *this; return o;}
  std::ostream& ToStreamRaw(std::ostream& o);

  std::istream& FromStream(std::istream& i)
  { i >> *this; return i;}
  

  void AddParent( DStructDesc* p);

// private:
//   void AddTag( const BaseGDL* data);     // adds copy of data
//   void AddTagGrab(BaseGDL* data); // adds data (only used by ExtraT) 
//  friend class ExtraT;

public:
  // adds data, grabs, adds also to descriptor (for initialization only)
  // note that initialization is only for scalars
  void NewTag( const std::string& tName, BaseGDL* data); 

  // for easier internal (c++) usage
  template< class DataGDL>
  void InitTag(const std::string& tName, const DataGDL& data)
  {
    int tIx = Desc()->TagIndex( tName);
    if( tIx == -1)
      throw GDLException("Struct "+Desc()->Name()+
			 " does not contain tag "+tName+".");

    assert( GetTag( tIx)->N_Elements() == data.N_Elements());

    // SA: removed, apparently unnecessary, static_cast() which was causing 
    //     numerous compiler (e.g., i686-apple-darwin9-g++-4.0.1) warnings, e.g.:
    //
    //     warning: inline function 
    //       'Data_<SpDULong>& Data_<SpDULong>::operator=(const Data_<SpDULong>&)' 
    //       used but never defined  
    //
    //static_cast<DataGDL&>( *GetTag( tIx)) = data; // copy data
    GetTag( tIx)->InitFrom( data); // copy data
  }
  
  // members
  // used by the interpreter
  // throws (datatypes.cpp)
  int Scalar2Index( SizeT& st) const;
  int Scalar2RangeT( RangeT& st) const;
  RangeT LoopIndex() const;

  bool Scalar() const 
  { return (N_Elements() == 1);}
  
  // make a duplicate on the heap
  BaseGDL* Dup() const { return new DStructGDL(*this);}
  
DStructGDL* New ( const dimension& dim_,
                  BaseGDL::InitType noZero=BaseGDL::ZERO ) const
{
// No NOZERO for structs
	if ( noZero == BaseGDL::NOZERO )
	{
		DStructGDL* res = new DStructGDL ( Desc(), dim_, noZero );
		res->MakeOwnDesc();
		return res;
	}
	if ( noZero == BaseGDL::INIT )
	{
		DStructGDL* res =  new DStructGDL ( Desc(), dim_, BaseGDL::NOZERO );
		res->MakeOwnDesc();
		SizeT nEl = res->N_Elements();
		SizeT nTags = NTags();
		for ( SizeT t=0; t<nTags; ++t )
		{
			const BaseGDL& cpTag = *GetTag ( t );
			for ( SizeT i=0; i<nEl; ++i )
				res->GetTag ( t, i )->InitFrom( cpTag);
			// 	      res->dd[ i] = dd[ i % nTags]->Dup();
		}
		return res;
	}
	DStructGDL* res = new DStructGDL ( Desc(), dim_ );
	res->MakeOwnDesc();
	return res;
}
DStructGDL* NewResult() const
{
	DStructGDL* res = new DStructGDL ( Desc(), this->dim, BaseGDL::NOZERO);
	res->MakeOwnDesc();
	return res;
}

  // used by interpreter, calls CatInsert
  DStructGDL* CatArray( ExprListT& exprList, 
			const SizeT catRank, 
			const SizeT rank);

  template< typename To> typename Data_<To>::Ty GetAs( SizeT i);

  BaseGDL* Convert2( DType destTy, 
		     BaseGDL::Convert2Mode mode = BaseGDL::CONVERT);
  
#if (defined( TEMPLATE_FRIEND_OK_) || (__GNUC__ >= 4)) && (!__clang__)
  // make all other Convert2 functions friends
  template<class Sp2>  
  friend BaseGDL* Data_<Sp2>::Convert2( DType destTy, 
					BaseGDL::Convert2Mode);
#else
  // this explicit version should work in all cases
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

  std::ostream& Write( std::ostream& os, bool swapEndian, bool compress,
		       XDR *xdrs);
  std::istream& Read( std::istream& os, bool swapEndian, bool compress,
		      XDR *xdrs);

  bool True()
  { throw GDLException("Struct expression not allowed in this context.");}
  bool False()
  { throw GDLException("Struct expression not allowed in this context.");}
  bool LogTrue()
  { throw GDLException("Struct expression not allowed in this context.");}
  bool LogTrue( SizeT ix)
  { throw GDLException("Struct expression not allowed in this context.");}
  DLong* Where( bool, SizeT&)
  { throw GDLException("Struct expression not allowed in this context.");}
  DByte* TagWhere( SizeT&)
  { throw GDLException("Struct expression not allowed in this context.");}
  Data_<SpDByte>* LogNeg()
  { throw GDLException("Struct expression not allowed in this context.");}
  int  Sgn(); // -1,0,1
  bool Equal( BaseGDL*) const;
  bool EqualNoDelete( const BaseGDL*) const;
  bool ArrayEqual( BaseGDL*);
  bool ArrayNeverEqual( BaseGDL*);
  // 'for' statement compliance (int types , float types scalar only)
  void ForCheck( BaseGDL**, BaseGDL** =NULL);
  bool ForCondUp( BaseGDL*);
  bool ForCondDown( BaseGDL*);
  void ForAdd();
  void ForAdd( BaseGDL* add=NULL);

  BaseGDL* UMinus(); // UMinus for SpDString returns float
  DStructGDL*   NotOp();
  DStructGDL*   AndOp( BaseGDL* r);
  DStructGDL*   AndOpInv( BaseGDL* r);
  DStructGDL*   OrOp( BaseGDL* r);
  DStructGDL*   OrOpInv( BaseGDL* r);
  DStructGDL*   XorOp( BaseGDL* r);
  DStructGDL*   Add( BaseGDL* r);
  DStructGDL*   AddInv( BaseGDL* r);
  DStructGDL*   Sub( BaseGDL* r);
  DStructGDL*   SubInv( BaseGDL* r);
  DStructGDL*   GtMark( BaseGDL* r);
  DStructGDL*   LtMark( BaseGDL* r);
  DStructGDL*   Mult( BaseGDL* r);
  DStructGDL*   Div( BaseGDL* r);
  DStructGDL*   DivInv( BaseGDL* r);
  DStructGDL*   Mod( BaseGDL* r);
  DStructGDL*   ModInv( BaseGDL* r);
  DStructGDL*   Pow( BaseGDL* r);
  DStructGDL*   PowInv( BaseGDL* r);
  DStructGDL*   PowInt( BaseGDL* r);
  DStructGDL*   MatrixOp( BaseGDL* r, bool atranspose, bool btranspose);


  DStructGDL*   AndOpS( BaseGDL* r);
  DStructGDL*   AndOpInvS( BaseGDL* r);
  DStructGDL*   OrOpS( BaseGDL* r);
  DStructGDL*   OrOpInvS( BaseGDL* r);
  DStructGDL*   XorOpS( BaseGDL* r);
  DStructGDL*   AddS( BaseGDL* r);
  DStructGDL*   AddInvS( BaseGDL* r);
  DStructGDL*   SubS( BaseGDL* r);
  DStructGDL*   SubInvS( BaseGDL* r);
  DStructGDL*   GtMarkS( BaseGDL* r);
  DStructGDL*   LtMarkS( BaseGDL* r);
  DStructGDL*   MultS( BaseGDL* r);
  DStructGDL*   DivS( BaseGDL* r);
  DStructGDL*   DivInvS( BaseGDL* r);
  DStructGDL*   ModS( BaseGDL* r);
  DStructGDL*   ModInvS( BaseGDL* r);
  DStructGDL*   PowS( BaseGDL* r);
  DStructGDL*   PowInvS( BaseGDL* r);


  // operators returning a new value
  DStructGDL* AndOpNew( BaseGDL* r);
  DStructGDL* AndOpInvNew( BaseGDL* r);
  DStructGDL* OrOpNew( BaseGDL* r);
  DStructGDL* OrOpInvNew( BaseGDL* r);
  DStructGDL* XorOpNew( BaseGDL* r);
//   DStructGDL* EqOpNew( BaseGDL* r);
//   DStructGDL* NeOpNew( BaseGDL* r);
//   DStructGDL* LeOpNew( BaseGDL* r);
//   DStructGDL* GeOpNew( BaseGDL* r);
//   DStructGDL* LtOpNew( BaseGDL* r);
//   DStructGDL* GtOpNew( BaseGDL* r);
  DStructGDL* AddNew( BaseGDL* r);      // implemented
  DStructGDL* AddInvNew( BaseGDL* r);      // implemented
  DStructGDL* SubNew( BaseGDL* r);
  DStructGDL* SubInvNew( BaseGDL* r);
  DStructGDL* LtMarkNew( BaseGDL* r);
  DStructGDL* GtMarkNew( BaseGDL* r);
  DStructGDL* MultNew( BaseGDL* r);   // implemented
  DStructGDL* DivNew( BaseGDL* r);
  DStructGDL* DivInvNew( BaseGDL* r);
  DStructGDL* ModNew( BaseGDL* r);
  DStructGDL* ModInvNew( BaseGDL* r);
  DStructGDL* PowNew( BaseGDL* r);
  DStructGDL* PowInvNew( BaseGDL* r);
  DStructGDL* PowIntNew( BaseGDL* r);   // implemented

  // operators with scalar returning a new value
  DStructGDL* AndOpSNew( BaseGDL* r);
  DStructGDL* AndOpInvSNew( BaseGDL* r);
  DStructGDL* OrOpSNew( BaseGDL* r);
  DStructGDL* OrOpInvSNew( BaseGDL* r);
  DStructGDL* XorOpSNew( BaseGDL* r);
  DStructGDL* AddSNew( BaseGDL* r);         // implemented
  DStructGDL* AddInvSNew( BaseGDL* r);    // implemented
  DStructGDL* SubSNew( BaseGDL* r);
  DStructGDL* SubInvSNew( BaseGDL* r);
  DStructGDL* LtMarkSNew( BaseGDL* r);
  DStructGDL* GtMarkSNew( BaseGDL* r);
  DStructGDL* MultSNew( BaseGDL* r);      // implemented
  DStructGDL* DivSNew( BaseGDL* r);
  DStructGDL* DivInvSNew( BaseGDL* r);
  DStructGDL* ModSNew( BaseGDL* r);
  DStructGDL* ModInvSNew( BaseGDL* r);
  DStructGDL* PowSNew( BaseGDL* r);
  DStructGDL* PowInvSNew( BaseGDL* r);


  
  BaseGDL* EqOp( BaseGDL* r);
  BaseGDL* NeOp( BaseGDL* r);
  BaseGDL* GtOp( BaseGDL* r);
  BaseGDL* GeOp( BaseGDL* r);
  BaseGDL* LtOp( BaseGDL* r);
  BaseGDL* LeOp( BaseGDL* r);

  // assigns srcIn to this at ixList, if ixList is NULL does linear copy
  // assumes: ixList has this already set as variable
  // frees overwritten members
  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList, SizeT offset);
  void AssignAt( BaseGDL* srcIn, ArrayIndexListT* ixList);
  void AssignAt( BaseGDL* srcIn);
  void AssignAtIx( RangeT ixR, BaseGDL* srcIn);

  void DecAt( ArrayIndexListT* ixList)
  {
    throw GDLException("Struct expression not allowed in this context.");
  }
  void IncAt( ArrayIndexListT* ixList)
  {
    throw GDLException("Struct expression not allowed in this context.");
  }
  void Dec()
  {
    throw GDLException("Struct expression not allowed in this context.");
  }
  void Inc()
  {
    throw GDLException("Struct expression not allowed in this context.");
  }

  // used by AccessDescT for resolving, no checking is done
  // inserts srcIn[ ixList] at offset
  // here d is initially empty -> no deleting of old data in InsertAt
  void InsertAt( SizeT offset, BaseGDL* srcIn, ArrayIndexListT* ixList);
  
  // returns (*this)[ ixList]
  DStructGDL* Index( ArrayIndexListT* ixList);
  DStructGDL* NewIx( SizeT ix);

  // formatting output functions
  
  SizeT OFmtA( std::ostream* os, SizeT offset, SizeT num, int width, int code); 
  SizeT OFmtF( std::ostream* os, SizeT offs, SizeT num, int width, int prec, const int code=0, const BaseGDL::IOMode oM = FIXED);
  SizeT OFmtI( std::ostream* os, SizeT offs, SizeT num, int width, int minN, int code=0, BaseGDL::IOMode oM = DEC);
  
  SizeT IFmtA( std::istream* is, SizeT offset, SizeT num, int width); 
  SizeT IFmtF( std::istream* is, SizeT offs, SizeT num, int width); 
  SizeT IFmtI( std::istream* is, SizeT offs, SizeT num, int width, 
		BaseGDL::IOMode oM = DEC);
  SizeT IFmtCal( std::istream* is, SizeT offs, SizeT r, int width, BaseGDL::Cal_IOMode cMode);
private:
  void IFmtAll( SizeT offs, SizeT r,
		SizeT& firstOut, SizeT& firstOffs, SizeT& tCount, SizeT& tCountOut);
  
  void OFmtAll( SizeT offs, SizeT r,
		SizeT& firstOut, SizeT& firstOffs, SizeT& tCount, SizeT& tCountOut);
  SizeT OFmtCal( std::ostream* os, SizeT offs, SizeT r, int w, int d, char *f,  int code, BaseGDL::Cal_IOMode oMode);   
  // inserts srcIn at ixDim
  // respects the exact structure
  // used by Assign -> old data must be freed
  void InsAt( DStructGDL* srcIn, ArrayIndexListT* ixList);

  // used for concatenation, called from CatArray
  // assumes that everything is checked (see CatInfo)
  void CatInsert( const DStructGDL* srcArr, const SizeT atDim, SizeT& at);
};

#endif
