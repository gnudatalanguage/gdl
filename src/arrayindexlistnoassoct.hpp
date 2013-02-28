/***************************************************************************
                          arrayindexlistnoassoct.hpp  -  array access descriptor,
			  optimization for non assoc index
                          -------------------
    begin                : July 22 2005
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

#ifndef ARRAYINDEXLISTNOASSOCT_HPP_
#define ARRAYINDEXLISTNOASSOCT_HPP_

#include "arrayindex.hpp"

// this is the main file for non assco arrayindices, 
// this classes here must only be used with non-assoc variables
// the classes in arrayindexlistt.hpp can handle both, because 
// as ASSOC variables are connected to files, a few more checks
// do not hurt, other than here: These are the core routines for indexing

// only one index [ix],[s:e],...
// NEVER ArrayIndexScalar types (they have their own ArrayIndexListT)
class ArrayIndexListOneNoAssocT: public ArrayIndexListT
{
private:
  IxExprListT cleanupIx;
	
  ArrayIndexT* ix;

  SizeT    nIx;                  // number of indexed elements

  AllIxBaseT* allIx;

  char allIxInstance[ AllIxMaxSize];
  
public:    
  
  ~ArrayIndexListOneNoAssocT()
  {
//     delete allIx;
    delete ix;
    cleanupIx.Cleanup(); // must be explicitely cleaned up
  }

  // constructor
  ArrayIndexListOneNoAssocT()
	: cleanupIx()
    , ix( NULL)
    , allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneNoAssocT( const ArrayIndexListOneNoAssocT& cp):
	cleanupIx(),
    ArrayIndexListT( cp),
    ix( cp.ix->Dup()),
    allIx( NULL)
  {
    assert( cp.allIx == NULL);
	assert( cp.cleanupIx.size() == 0);
  }

  // called after structure is fixed
  ArrayIndexListOneNoAssocT( ArrayIndexVectorT* aIV):
    allIx( NULL)
  {
    ix = (*aIV)[0]->Dup(); // ArrayIndexListOneT will grab here
    nParam = ix->NParam();
  }    
  
  void Clear()
  {
  //     delete allIx;
    allIx = NULL;
  //     allIxMulti.Clear();
    ix->Clear();
    cleanupIx.Cleanup();
  }

  ArrayIndexListT* Clone() { return new ArrayIndexListOneNoAssocT( *this);}

  void InitAsOverloadIndex( IxExprListT& ix_, IxExprListT* cleanupIxIn, IxExprListT& ixOut) 
  { 
    assert( allIx == NULL);
    assert( ix_.size() == nParam);

    if( cleanupIxIn != NULL)
      cleanupIx = *cleanupIxIn;

    DLongGDL* isRange = new DLongGDL( dimension(1, BaseGDL::NOZERO));
    ixOut.push_back(isRange);
    
    (*isRange)[ 0] = (ix->IsRange()) ? 1 : 0;
    if( nParam == 0)    
      {
	BaseGDL* oIx = ix->OverloadIndexNew();
	ixOut.push_back(oIx);
	return;
      }
    if( nParam == 1) 
      {
	BaseGDL* oIx = ix->OverloadIndexNew( ix_[ 0]);
	ixOut.push_back(oIx);
	return;
      }
    if( nParam == 2) 
      {
	BaseGDL* oIx = ix->OverloadIndexNew( ix_[ 0], ix_[ 1]);
	ixOut.push_back(oIx);
	return;
      }
    if( nParam == 3) 
      {
	BaseGDL* oIx = ix->OverloadIndexNew( ix_[ 0], ix_[ 1], ix_[ 2]);
	ixOut.push_back(oIx);
	return;
      }
  }
  
  void Init( IxExprListT& ix_, IxExprListT* cleanupIxIn)
  {
    assert( allIx == NULL);
    assert( ix_.size() == nParam);

    if( cleanupIxIn != NULL)
      cleanupIx = *cleanupIxIn;
    
    if( nParam == 0) //return;
      {
	ix->Init();
	return;
      }
    if( nParam == 1) 
      {
	ix->Init( ix_[ 0]);
	return;
      }
    if( nParam == 2) 
      {
	ix->Init( ix_[ 0], ix_[ 1]);
	return;
      }
    else // nParam == 3
      {
	assert( nParam == 3);
	ix->Init( ix_[ 0], ix_[ 1], ix_[ 2]);
	return;
      }
  }

  void Init() {} // eg. a[*]

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
//   bool ToAssocIndex( SizeT& lastIx)
  bool ToAssocIndex( SizeT& lastIx)
  {
    assert( 0);
    throw GDLException(-1,NULL,"Internal error: ArrayIndexListOneNoAssocT::ToAssocIndex(...) called.",true,false);
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // for assoc variables last index is the record
//     if( var->IsAssoc()) return;

    // ArrayIndexScalar[VP] are not initialized
    // they need the NIter call, but
    // for only one index they have their own ArrayIndexListT
    nIx=ix->NIter( var->Size());
  }
  
  // structure of indexed expression
  const dimension GetDim()
  {
    if( ix->Scalar())
      {
		return dimension();
      }
    else if( ix->Indexed())
      {
	return static_cast<ArrayIndexIndexed*>(ix)->GetDim();
	// gets structure of indexing array
      }
    else
      {
	return dimension( nIx); // one dimensional if not indexed
      }
  }

  SizeT N_Elements()
  {
    return nIx;
  }

  // returns 1-dim index for all elements
  AllIxBaseT* BuildIx()
  {
    if( allIx != NULL) // can happen if called from DotAccessDescT::DoAssign()
      return allIx;

    assert( allIx == NULL);
//     if( allIx != NULL)
// 		return allIx;

    if( ix->Indexed())
      {
//	allIx = static_cast< ArrayIndexIndexed*>(ix)->StealIx();
	allIx = static_cast< ArrayIndexIndexed*>(ix)->GetAllIx();
	return allIx;
      }
		
    if( nIx == 1)
    {
	    allIx = new (allIxInstance) AllIxT( ix->GetS());
	    return allIx;
    }

// 	allIx = new AllIxMulAllIxRangeStrideTtiT( nIx);
    SizeT s = ix->GetS();
    SizeT ixStride = ix->GetStride();
    if( ixStride <= 1) 
      if( s != 0)
	    {
	    allIx = new (allIxInstance) AllIxRangeT( nIx, s);
// 	    for( SizeT i=0; i<nIx; ++i)
// 			static_cast<AllIxMultiT*>(allIx)->SetIx( i, i + s);
// 	      (*allIx)[i] = i + s;
	    }
      else
	    {
	    allIx = new (allIxInstance) AllIxRange0T( nIx);
// 	    for( SizeT i=0; i<nIx; ++i)
// 			static_cast<AllIxMultiT*>(allIx)->SetIx( i, i );
// 	      (*allIx)[i] = i;
	    }
    else
      if( s != 0) 
	{
	    allIx = new (allIxInstance) AllIxRangeStrideT( nIx, s, ixStride);
// 	    for( SizeT i=0; i<nIx; ++i)
// 			static_cast<AllIxMultiT*>(allIx)->SetIx( i, i * ixStride + s);
// 	      (*allIx)[i] = i * ixStride + s;
	    }
      else
	    {
	    allIx = new (allIxInstance) AllIxRange0StrideT( nIx, ixStride);
// 	    for( SizeT i=0; i<nIx; ++i)
// 			static_cast<AllIxMultiT*>(allIx)->SetIx( i, i * ixStride);
// 	      (*allIx)[i] = i * ixStride;
	    }
    return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx() const
  {
    return ix->GetIx0();
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // scalar case
    if( right->N_Elements() == 1 && //!var->IsAssoc() &&
	ix->NIter( var->Size()) == 1)// && var->Type() != GDL_STRUCT) 
      {
	var->AssignAtIx( ix->GetIx0(), right);
	return;
      }
    
    SetVariable( var);
    
    if( var->EqType( right))
      {
	var->AssignAt( right, this); // assigns inplace
      }
    else
      {
	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
	std::auto_ptr<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_)
  {
    Init( ix_, NULL);
    if( ix->Scalar())// && ix->NIter( var->Size()) == 1)// && var->Type() != GDL_STRUCT) 
//     if( !var->IsAssoc() && ix->NIter( var->Size()) == 1)// && var->Type() != GDL_STRUCT) 
      {
	SizeT assertValue = ix->NIter( var->Size());
	assert( assertValue == 1);

	return var->NewIx( ix->GetIx0());
      }
    // normal case
    SetVariable( var);
    return var->Index( this);
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  const dimension GetDimIx0( SizeT& destStart)
  {
    destStart = ix->GetIx0();

    return dimension( destStart);
  }

  SizeT NDim()
  { 
    return 1;
  }
}; //class ArrayIndexListOneT: public ArrayIndexListT





// loop index
class ArrayIndexListOneScalarNoAssocT: public ArrayIndexListT
{
protected:
  SizeT varIx;
  RangeT sInit;
  RangeT s;

  AllIxT allIx;

public:    
  
  ~ArrayIndexListOneScalarNoAssocT() {}

  // constructor
  ArrayIndexListOneScalarNoAssocT()
    : varIx( 0)
//     , allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneScalarNoAssocT( const ArrayIndexListOneScalarNoAssocT& cp)
    : ArrayIndexListT( cp)
    , varIx( cp.varIx)
    , s( cp.s)
//     , allIx( NULL)
  {
//     assert( cp.allIx == NULL);
  }

  // called after structure is fixed
  ArrayIndexListOneScalarNoAssocT( ArrayIndexVectorT* aIV)
//     : allIx( NULL)
  {
    nParam = 0;
    
    varIx = static_cast<ArrayIndexScalar*>((*aIV)[0])->GetVarIx();

    // ArrayIndexListOneScalarT will do the cleanup
//     delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  ArrayIndexListT* Clone() { return new ArrayIndexListOneScalarNoAssocT( *this);}

  void InitAsOverloadIndex( IxExprListT& ix_, IxExprListT* cleanupIxIn, IxExprListT& ixOut); 
  
  void Init() {}

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx) { assert( false); return false;}

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var);
  
  // structure of indexed expression
  const dimension GetDim()
  {
    return dimension();
  }

  SizeT N_Elements()
  {
    return 1;
  }

  // returns 1-dim index for all elements
  AllIxT* BuildIx()
  {
	allIx.Set( s);
	return &allIx;
  
//     if( allIx != NULL)
//       {
// 		allIx->Set( s);
// 	return allIx;
//       }
// 
//     allIx = new AllIxT( s);
//     return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx() const
  {
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right);

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_);

  // returns multi-dim index for 1st element
  // used by InsAt functions
  const dimension GetDimIx0( SizeT& destStart)
  {
    destStart = s;
    return dimension( destStart);
  }

  SizeT NDim()
  { 
    return 1;
  }

}; // class ArrayIndexListOneScalarT: public ArrayIndexListT





class ArrayIndexListOneScalarVPNoAssocT: public ArrayIndexListT
{
protected:
  DVar* varPtr;

  RangeT sInit;
  RangeT s;

  AllIxT allIx;
//   AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneScalarVPNoAssocT()
  {
//     delete allIx;
  }

  // constructor
  ArrayIndexListOneScalarVPNoAssocT()
    : varPtr( NULL)
//     , allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneScalarVPNoAssocT( const ArrayIndexListOneScalarVPNoAssocT& cp)
    : ArrayIndexListT( cp)
    , varPtr( cp.varPtr)
    , sInit( cp.sInit)
    , s( cp.s)
//     , allIx( NULL)
  {
//     assert( cp.allIx == NULL);
  }

  // called after structure is fixed
  ArrayIndexListOneScalarVPNoAssocT( ArrayIndexVectorT* aIV)
//     : allIx( NULL)
  {
    nParam = 0;
    
    varPtr = static_cast<ArrayIndexScalarVP*>((*aIV)[0])->GetVarPtr();

        // ArrayIndexListOneScalarVPT will do the cleanup
//     delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  ArrayIndexListT* Clone() { return new ArrayIndexListOneScalarVPNoAssocT( *this);}

  void InitAsOverloadIndex( IxExprListT& ix_, IxExprListT* cleanupIxIn, IxExprListT& ixOut); 

  void Init() {}

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    sInit = varPtr->Data()->LoopIndex();
    if( sInit < 0)
      throw GDLException(-1,NULL,"Record number must be a scalar > 0 in this context.",true,false);
    s = sInit;
    lastIx = s;
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    sInit = varPtr->Data()->LoopIndex();
    if( sInit < 0)
      s = sInit + var->N_Elements();
    else
      s = sInit;
    
    // for assoc variables last index is the record
//     if( var->IsAssoc()) return;
    if( s >= var->Size())
      throw GDLException(-1,NULL,"Scalar subscript out of range [>].1",true,false);
    if( s < 0)
      throw GDLException(-1,NULL,"Scalar subscript out of range [<].1",true,false);
  }
  
  // structure of indexed expression
  const dimension GetDim()
  {
    return dimension();
  }

  SizeT N_Elements()
  {
    return 1;
  }

  // returns 1-dim index for all elements
  AllIxT* BuildIx()
  {
	allIx.Set( s);
	return &allIx;
    
//     if( allIx != NULL)
//       {
// 		allIx->Set( s);
// 		return allIx;
//       }
// 
//     allIx = new AllIxT( s);
//     return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx() const
  {
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1) // && !var->IsAssoc()) // && var->Type() != GDL_STRUCT) 
      {
	s = varPtr->Data()->LoopIndex();
	if( s >= var->Size())
	  throw GDLException(-1,NULL,"Scalar subscript out of range [>].2",true,false);
	var->AssignAtIx( s, right);
	return;
      }
    
    SetVariable( var);
    if( var->EqType( right))
      {
	var->AssignAt( right, this); // assigns inplace
      }
    else
      {
	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
	std::auto_ptr<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_);

  // returns multi-dim index for 1st element
  // used by InsAt functions
  const dimension GetDimIx0( SizeT& destStart)
  {
    destStart = s;
    return dimension( destStart);
  }
  SizeT NDim()
  { 
    return 1;
  }

}; // class ArrayIndexListOneScalarVPT: public ArrayIndexListT





class ArrayIndexListOneConstScalarNoAssocT: public ArrayIndexListT
{
  RangeT sInit;
  RangeT s;
  AllIxT allIx;
  BaseGDL* rawData;

public:    
  
  ~ArrayIndexListOneConstScalarNoAssocT() 
  {
    delete rawData;
  }

  // constructor
  ArrayIndexListOneConstScalarNoAssocT()
  : rawData( NULL)
// 	: allIx( NULL)
  {
    nParam = 0;
  }

  ArrayIndexListOneConstScalarNoAssocT( const ArrayIndexListOneConstScalarNoAssocT& cp)
    : ArrayIndexListT( cp)
    , sInit( cp.sInit)
    , s( cp.s)
//     , allIx( NULL)
  {
    assert( cp.rawData != NULL);
    rawData = cp.rawData->Dup();
//     assert( cp.allIx == NULL); // all copying should be done before using.
  }

  // called after structure is fixed
  ArrayIndexListOneConstScalarNoAssocT( ArrayIndexVectorT* aIV)
//     : allIx( NULL)
  {
    assert( CArrayIndexScalarID == (*aIV)[0]->Type()); // see MakeArrayIndex (arrayindex.cpp)
    CArrayIndexScalar* arrayIndex = static_cast<CArrayIndexScalar*>( (*aIV)[0]);
    rawData = arrayIndex->StealRawData();
    sInit = arrayIndex->GetS();
    if( sInit >= 0)
      s = sInit;
    nParam = 0;

   // ArrayIndexListOneConstScalarT will do the cleanup
//     delete (*aIV)[0];
  }    

  void InitAsOverloadIndex( IxExprListT& ix_, IxExprListT* cleanupIxIn, IxExprListT& ixOut) 
  { 
    assert( 0 == nParam);
    assert( rawData != NULL);
    
    DLongGDL* isRange = new DLongGDL( 0);
    ixOut.push_back(isRange);

    ixOut.push_back(rawData->Dup());
  }

  
  void Clear()
  {}

  ArrayIndexListT* Clone() { return new ArrayIndexListOneConstScalarNoAssocT( *this);}

  void Init()
  {}

  SizeT N_Elements()
  {
    return 1;
  }

  // returns 1-dim index for all elements
  AllIxT* BuildIx()
  {
	allIx.Set( s);
	return &allIx;
  
//     if( allIx != NULL)
//       return allIx;
// 
//     allIx = new AllIxT( s);
//     return allIx;
  }

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    if( sInit < 0)
      throw GDLException(-1,NULL,"Record number must be a scalar > 0 in this context.",true,false);      
    lastIx = sInit;
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
//     if( var->IsAssoc()) return;
    if( sInit < 0)
      s = sInit + var->Size();
    // for assoc variables last index is the record
    if( s < 0)
      throw GDLException(-1,NULL,"Scalar subscript out of range [<].1",true,false);
    if( s >= var->Size())
      throw GDLException(-1,NULL,"Scalar subscript out of range [>].1",true,false);
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx() const
  {
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // one index and assoc -> no index left -> no AssignAt
    // wrong! This is the entry function
    // assert( !var->IsAssoc());

    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1)// && !var->IsAssoc())// && var->Type() != GDL_STRUCT) 
      {
	if( sInit < 0)
	  s = sInit + var->Size();
	if( s < 0)
	  throw GDLException(-1,NULL,"Scalar subscript out of range [<].4",true,false);
	if( s >= var->Size())
	  throw GDLException(-1,NULL,"Scalar subscript out of range [>].4",true,false);
	var->AssignAtIx( s, right); // must use COPY_BYTE_AS_INT
	return;
      }
    
    SetVariable( var);
    if( var->EqType( right))
      {
		var->AssignAt( right, this); // assigns inplace
      }
    else
      {
		BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
		std::auto_ptr<BaseGDL> conv_guard( rConv);
	
		var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
//     if( !var->IsAssoc())// && var->Type() != GDL_STRUCT)
      {
	if( sInit < 0)
	  s = sInit + var->Size();
	if( s < 0)
		throw GDLException(-1,NULL,"Scalar subscript out of range [<].5",true,false);
	if( s >= var->Size())
	{
// 	    std::cout << s << " var->Size():" << var->Size() << std::endl;
		throw GDLException(-1,NULL,"Scalar subscript out of range [>].5",true,false);
	}
	
	return var->NewIx( s);
      }
    
    // normal case
    //Init();// ix_);
    //SetVariable( var);
    return var->Index( this);
  }

  const dimension GetDim()
  {
    return dimension();
  }

  const dimension GetDimIx0( SizeT& destStart)
  {
    destStart = s;
    return dimension( destStart);
  }

  SizeT NDim()
  { 
    return 1;
  }

}; // class ArrayIndexListOneConstScalarT: public ArrayIndexListT





// all scalar elements (multi-dim)
class ArrayIndexListScalarNoAssocT: public ArrayIndexListT
{
private:
  ArrayIndexVectorT ixList;
//   std::vector<SizeT> paramPresent;

  SizeT    acRank;               // rank upto which indexing is done
  const SizeT*    varStride; // variables stride
//   SizeT    varStride[MAXRANK+1]; // variables stride
//  SizeT    nIx;                  // number of indexed elements

  AllIxT allIx;
//   AllIxT allIxInstance;

//   ArrayIndexT* ixListEnd; // for assoc index

  // constructor
  ArrayIndexListScalarNoAssocT():
    acRank(0)
  { nParam = 0;}

public:    
  
  ~ArrayIndexListScalarNoAssocT() // cleanup done by related ArrayIndexListScalarT
  {
//     delete allIx;
// 	ixList.Destruct();
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin();
// 	 i != ixList.end(); ++i)
//       {	delete *i;}
  }

//   // constructor
//   ArrayIndexListScalarNoAssocT():
//     acRank(0)
// //     allIx( NULL),
// //     ixListEnd( NULL)
//   { nParam = 0;}

  ArrayIndexListScalarNoAssocT( const ArrayIndexListScalarNoAssocT& cp):
    ArrayIndexListT( cp),
//     paramPresent( cp.paramPresent),
    acRank(cp.acRank)
//     allIx( NULL),
//     ixListEnd( NULL)
  {
    //    ixList.reserve(MAXRANK); 
//     assert( cp.allIx == NULL);
//     assert( cp.ixListEnd == NULL);

    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());
  }

  // called once after structure is fixed
  ArrayIndexListScalarNoAssocT( ArrayIndexVectorT* ix):
    ixList( *ix)
//     allIx( NULL),
//     ixListEnd( NULL)
  {
    if( ix->size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    assert( ixList.size() > 1); // must be, from compiler
    
    // set acRank
    acRank = ixList.size();

    nParam = 0;
  }    

  void InitAsOverloadIndex( IxExprListT& ix, IxExprListT* cleanupIxIn, IxExprListT& ixOut) 
  { 
    assert( ix.size() == 0);

    DLongGDL* isRange = new DLongGDL( dimension(ixList.size(), BaseGDL::ZERO));
    ixOut.push_back(isRange);
    
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	assert( ixList[ i]->NParam() == 0);
	BaseGDL* oIx = ixList[ i]->OverloadIndexNew();
	ixOut.push_back(oIx);
      }
  }
  
  void Clear()
  {
  }

  ArrayIndexListT* Clone() { return new ArrayIndexListScalarNoAssocT( *this);}

  void Init()
  {}
  
  // set the root variable which is indexed by this ArrayIndexListScalarT
  void SetVariable( BaseGDL* var) 
  {
//     // set acRank
//     acRank = ixList.size();

    // ArrayIndexScalar[VP] need this call to read their actual data
    // as their are not initalized (nParam == 0)
    for( SizeT i=0; i<acRank; ++i)
      ixList[i]->NIter( var->Dim(i)); // check boundary

    varStride = var->Dim().Stride();
  }

  // structure of indexed expression
  const dimension GetDim()
  {
    return dimension(); // -> results in scalar
  }

  SizeT N_Elements()
  {
    return 1;
  }

  // returns 1-dim index for all elements
  AllIxT* BuildIx()
  {
//     if( allIx != NULL)
// 		return allIx;

    SizeT s = ixList.FrontGetS(); //[0]->GetS();
    for( SizeT l=1; l < acRank; ++l)
      {
	s += ixList[l]->GetS() * varStride[l]; 
      }
//     allIx = &allIxInstance; //new AllIxT(s);
//     allIx->Set( s);
    allIx.Set( s);
//     allIx = new AllIxT(s);
//     (*allIx)[0] = s;

    return &allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx() const
  {
//     if( acRank == 1)
//       return ixList.FrontGetS(); //ixList[0]->GetS();    
    SizeT dStart = ixList.FrontGetS(); //[0]->GetS();
    for( SizeT i=1; i < acRank; ++i)
      dStart += ixList[i]->GetS() * varStride[ i];    
    return dStart;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // right, not var
    if( right->N_Elements() == 1)// && !var->IsAssoc())
    {
      // SetVariable( var);
//       // set acRank
//       acRank = ixList.size();

      varStride = var->Dim().Stride();
      // ArrayIndexScalar[VP] need this call to read their actual data
      // as their are not initalized (nParam == 0)
      ixList[0]->NIter( var->Dim(0)); // check boundary
      SizeT dStart = ixList.FrontGetS(); //[0]->GetS();
      for( SizeT i=1; i < acRank; ++i)
      {
	ixList[i]->NIter( var->Dim(i)); // check boundary
	dStart += ixList[i]->GetS() * varStride[ i];    
      }
      
      var->AssignAtIx( dStart, right); // assigns inplace
      return;
    }
    // var->N_Elements() > 1
    SetVariable( var);
//     assert( nIx == 1);    
    if( var->EqType( right))
      {
	var->AssignAt( right, this); // assigns inplace (not only scalar)
      }
    else
      {
	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
	std::auto_ptr<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace (not only scalar)
      }
  }

  
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    varStride = var->Dim().Stride();
    // ArrayIndexScalar[VP] need this call to read their actual data
    // as their are not initalized (nParam == 0)
    ixList[0]->NIter( var->Dim(0)); // check boundary
    SizeT dStart = ixList.FrontGetS(); //[0]->GetS();
    for( SizeT i=1; i < acRank; ++i)
    {
      ixList[i]->NIter( var->Dim(i)); // check boundary
      dStart += ixList[i]->GetS() * varStride[ i];    
    }
    return var->NewIx( dStart); //this->LongIx());
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  const dimension GetDimIx0( SizeT& destStart)
  {
    SizeT dStart = 0;

    SizeT actIx[ MAXRANK];
    for( SizeT i=0; i < acRank; ++i)
      {
		actIx[ i] = ixList[i]->GetS();
		dStart += actIx[ i] * varStride[ i];
      }

    destStart = dStart;
    return dimension( actIx, acRank);
  }
  
  SizeT NDim()
  { 
    return acRank;
  }
}; // class ArrayIndexListScalarT: public ArrayIndexListT


// all scalar elements (multi-dim)
const int acRank2D = 2;
class ArrayIndexListScalarNoAssoc2DT: public ArrayIndexListT
{
private:
  ArrayIndexVectorT ixList;
//   std::vector<SizeT> paramPresent;

//   enum AcRankEnum { acRank = 2; };
  
  SizeT    varStride; // variables stride
//   SizeT    varStride[MAXRANK+1]; // variables stride
//  SizeT    nIx;                  // number of indexed elements

  AllIxT allIx;
//   AllIxT allIxInstance;

//   ArrayIndexT* ixListEnd; // for assoc index

  // constructor
  ArrayIndexListScalarNoAssoc2DT() { nParam = 0;}

public:    
  
  ~ArrayIndexListScalarNoAssoc2DT() // cleanup done by related ArrayIndexListScalarT
  {
  }

  ArrayIndexListScalarNoAssoc2DT( const ArrayIndexListScalarNoAssoc2DT& cp):
    ArrayIndexListT( cp)
  {
    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());

    assert( ixList.size() == 2); // must be, from compiler
  }

  // called once after structure is fixed
  ArrayIndexListScalarNoAssoc2DT( ArrayIndexVectorT* ix):
    ixList( *ix)
  {
    if( ix->size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    assert( ixList.size() == 2); // must be, from compiler
    nParam = 0;
  }    
  
  void InitAsOverloadIndex( IxExprListT& ix, IxExprListT* cleanupIxIn, IxExprListT& ixOut) 
  { 
    assert( ix.size() == 0);

    DLongGDL* isRange = new DLongGDL( dimension(ixList.size(), BaseGDL::ZERO));
    ixOut.push_back(isRange);
    
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	assert( ixList[ i]->NParam() == 0);
	BaseGDL* oIx = ixList[ i]->OverloadIndexNew();
	ixOut.push_back(oIx);
      }
  }
  
  void Clear()
  {
  }

  ArrayIndexListT* Clone() { return new ArrayIndexListScalarNoAssoc2DT( *this);}

  void Init()
  {}
  
  // set the root variable which is indexed by this ArrayIndexListScalarT
  void SetVariable( BaseGDL* var) 
  {
    varStride = var->Dim(0);
    ixList[0]->NIter( varStride); // check boundary
    ixList[1]->NIter( var->Dim(1)); // check boundary
  }

  // structure of indexed expression
  const dimension GetDim()
  {
    return dimension(); // -> results in scalar
  }

  SizeT N_Elements()
  {
    return 1;
  }

  // returns 1-dim index for all elements
  AllIxT* BuildIx()
  {
    SizeT s = ixList.FrontGetS() + ixList[1]->GetS() * varStride; 
    allIx.Set( s);
    return &allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx() const
  {
    SizeT s = ixList.FrontGetS() + ixList[1]->GetS() * varStride; 
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // right, not var
    if( right->N_Elements() == 1)// && !var->IsAssoc())
    {
      varStride = var->Dim(0);
      // ArrayIndexScalar[VP] need this call to read their actual data
      // as their are not initalized (nParam == 0)
      ixList[0]->NIter( varStride); // check boundary
      ixList[1]->NIter( var->Dim(1)); // check boundary
      SizeT s = ixList.FrontGetS() + ixList[1]->GetS() * varStride;     
      var->AssignAtIx( s, right); // assigns inplace
      return;
    }
    // right->N_Elements() > 1
    SetVariable( var);
    if( var->EqType( right))
      {
	var->AssignAt( right, this); // assigns inplace (not only scalar)
      }
    else
      {
	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
	std::auto_ptr<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace (not only scalar)
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    varStride = var->Dim(0);
    // ArrayIndexScalar[VP] need this call to read their actual data
    // as their are not initalized (nParam == 0)
    ixList[0]->NIter( varStride); // check boundary
    ixList[1]->NIter( var->Dim(1)); // check boundary
    SizeT dStart = ixList.FrontGetS() + ixList[1]->GetS() * varStride; 
    return var->NewIx( dStart); //this->LongIx());
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  const dimension GetDimIx0( SizeT& destStart)
  {
    SizeT actIx[ MAXRANK];

    actIx[ 0] = ixList[0]->GetS();
    actIx[ 1] = ixList[1]->GetS();

    SizeT dStart = actIx[ 0] + actIx[ 1] * varStride;

    destStart = dStart;
    return dimension( actIx, acRank2D);
  }
  
  SizeT NDim()
  { 
    return acRank2D;
  }
}; // class ArrayIndexListScalar2DT: public ArrayIndexListT






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// general case (mixed, multi-dim) at least one indexed
// but note: last index can be assoc index
class ArrayIndexListMultiNoAssocT: public ArrayIndexListT
{
private:
	IxExprListT cleanupIx;

protected:
  ArrayIndexVectorT ixList;

  enum AccessType
  {
	  GDL_UNDEF=0,      // for init access type
	  INDEXED_ONE,  // all indexed OR one
	  NORMAL,       // mixed
	  ALLINDEXED,
	  ALLONE        // all ONE
  };

  AccessType accessType;         // actual access type
  AccessType accessTypeInit;     // possible access type non assoc
  AccessType accessTypeAssocInit;// possible access type for assoc
  SizeT    acRank;               // rank upto which indexing is done
  SizeT    nIterLimit[MAXRANK];  // for each dimension, how many iterations
  SizeT    stride[MAXRANK+1];
  const SizeT*  varStride; // variables stride
// 	SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

  AllIxBaseT*      allIx;
  char allIxInstance[ AllIxMaxSize];

//   ArrayIndexT* ixListEnd; // for assoc index

  // for access with only a single variable index (column/row-extractor)
  SizeT nIterLimitGt1; // how many dimensions > 1
  RankT gt1Rank; // which rank is the variable rank
  SizeT baseIx; // offset to add for all other constant dims
  bool indexed; // is the variable index indexed?

public:    
  
  ~ArrayIndexListMultiNoAssocT()
  {
//     delete allIx;
//    ixList.Destruct(); // done in ArrayIndexListMultiT
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin();
// 	 i != ixList.end(); ++i)
//       {	delete *i;}
	cleanupIx.Cleanup();
  }

  // constructor
  ArrayIndexListMultiNoAssocT():
    accessType(NORMAL),
    acRank(0),
    allIx( NULL)
//     ixListEnd( NULL)
  { nParam = 0;}
	
  ArrayIndexListMultiNoAssocT( const ArrayIndexListMultiNoAssocT& cp):
    ArrayIndexListT( cp),
    accessType(cp.accessType),
    accessTypeInit(cp.accessTypeInit),
    accessTypeAssocInit(cp.accessTypeAssocInit),
    acRank(cp.acRank),
    allIx( NULL)
//     ixListEnd( NULL)
  {
    assert( cp.allIx == NULL);
//     assert( cp.ixListEnd == NULL);
	assert( cp.cleanupIx.size() == 0);
	
    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());
  }

  // called once after structure is fixed at (GDL-)compile time
  ArrayIndexListMultiNoAssocT( ArrayIndexVectorT* ix):
    ixList( *ix),
    allIx( NULL)
//     ixListEnd( NULL)
  {
    assert( ix->size() != 0); // must be, from compiler

    if( ixList.size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    nParam = 0;
    for( SizeT i=0; i<ix->size(); ++i)
	nParam += (*ix)[i]->NParam();

    // determine type of index
    SizeT nIndexed = 0;
    SizeT nScalar  = 0;
    for( SizeT i=0; (i+1)<ixList.size(); ++i)
      {
	// note: here we examine the actual type
	if( ArrayIndexScalarID == ixList[i]->Type() ||
		ArrayIndexScalarVPID == ixList[i]->Type() || // ? (from MakeArrayIndex)
		CArrayIndexScalarID == ixList[i]->Type() ) nScalar++;
	else if( ArrayIndexIndexedID == ixList[i]->Type() ||
		CArrayIndexIndexedID == ixList[i]->Type()) nIndexed++;
      }
    if( nScalar == ixList.size()-1)
      accessTypeAssocInit = ALLONE;
    else if( nIndexed == ixList.size()-1)
      accessTypeAssocInit = ALLINDEXED; // might be ALLONE as well
    else if( nScalar + nIndexed < ixList.size()-1)
      accessTypeAssocInit = NORMAL;
    else
      accessTypeAssocInit = INDEXED_ONE; // save some checking later
    
    if( ArrayIndexScalarID == ixList[ixList.size()-1]->Type() ||
	    ArrayIndexScalarVPID == ixList[ixList.size()-1]->Type() || // ? (from MakeArrayIndex)
	    CArrayIndexScalarID == ixList[ixList.size()-1]->Type()) nScalar++;
    else if( ArrayIndexIndexedID == ixList[ixList.size()-1]->Type() ||
	    CArrayIndexIndexedID == ixList[ixList.size()-1]->Type()) nIndexed++;
    
    if( nScalar == ixList.size())
      accessTypeInit = ALLONE;
    else if( nIndexed == ixList.size())
      // actually cannot happen (ArrayIndexListMultiAllIndexedT)
      // but better keep it for stability
      accessTypeInit = ALLINDEXED; // might be ALLONE as well
    else if( nScalar + nIndexed < ixList.size())
      accessTypeInit = NORMAL;
    else
      accessTypeInit = INDEXED_ONE; // save some checking later

//     std::cout << "accessTypeInit: " << accessTypeInit << std::endl;
  }
  
  void Clear()
  {
//     delete allIx;
    allIx = NULL;
    
    ixList.Clear();
//     for( ArrayIndexVectorT::iterator i=ixList.begin(); i != ixList.end(); ++i)
//       {	(*i)->Clear();}
    cleanupIx.Cleanup();
  }

  ArrayIndexListT* Clone() { return new ArrayIndexListMultiNoAssocT( *this);}

  void InitAsOverloadIndex( IxExprListT& ix, IxExprListT* cleanupIxIn, IxExprListT& ixOut) 
  { 
    assert( allIx == NULL);
    assert( ix.size() == nParam);

    if( cleanupIxIn != NULL)
      cleanupIx = *cleanupIxIn;

    DLongGDL* isRange = new DLongGDL( dimension(ixList.size(), BaseGDL::NOZERO));
    ixOut.push_back(isRange);
    
    SizeT pIX = 0;
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	SizeT ixNParam = ixList[ i]->NParam();
	(*isRange)[ i] = (ixList[ i]->IsRange()) ? 1 : 0;
	if( ixNParam == 0)    
	  {
	    BaseGDL* oIx = ixList[ i]->OverloadIndexNew();
	    ixOut.push_back(oIx);
	    continue;
	  }
	if( ixNParam == 1) 
	  {
	    BaseGDL* oIx = ixList[ i]->OverloadIndexNew( ix[ pIX]);
	    ixOut.push_back(oIx);
	    pIX += 1;
	    continue;
	  }
	if( ixNParam == 2) 
	  {
	    BaseGDL* oIx = ixList[ i]->OverloadIndexNew( ix[ pIX], ix[ pIX+1]);
	    ixOut.push_back(oIx);
	    pIX += 2;
	    continue;
	  }
	if( ixNParam == 3) 
	  {
	    BaseGDL* oIx = ixList[ i]->OverloadIndexNew( ix[ pIX], ix[ pIX+1], ix[ pIX+2]);
	    ixOut.push_back(oIx);
	    pIX += 3;
	    continue;
	  }
      }
  }

  void Init( IxExprListT& ix, IxExprListT* cleanupIxIn)
  {
    assert( allIx == NULL);
    assert( ix.size() == nParam);

    if( cleanupIxIn != NULL)
      cleanupIx = *cleanupIxIn;

    SizeT pIX = 0;
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	SizeT ixNParam = ixList[ i]->NParam();
	if( ixNParam == 0) 	    
	  {
	    ixList[ i]->Init();
	    continue;
	  }
	if( ixNParam == 1) 
	  {
	    ixList[ i]->Init( ix[ pIX]);
	    pIX += 1;
	    continue;
	  }
	if( ixNParam == 2) 
	  {
	    ixList[ i]->Init( ix[ pIX], ix[ pIX+1]);
	    pIX += 2;
	    continue;
	  }
	if( ixNParam == 3) 
	  {
	    ixList[ i]->Init( ix[ pIX], ix[ pIX+1], ix[ pIX+2]);
	    pIX += 3;
	    continue;
	  }
      }
  }
  
  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    assert( false);
    return FALSE;
  }

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // set acRank
    acRank = ixList.size();

    // for assoc variables last index is the record
//     if( var->IsAssoc()) 
//       {
// 	acRank--;
// 	accessType = accessTypeAssocInit;
//       }
//     else
      accessType = accessTypeInit;

    // can happen due to assoc variables
    if( accessType == ALLONE) // implied none INDEXED
      {
	const dimension& varDim  = var->Dim();
	SizeT            varRank = varDim.Rank();

	varStride = varDim.Stride();
	nIterLimitGt1 = 0; // marker for BuildIx

	ixList[0]->NIter( (0<varRank)?varDim[0]:1);
	assert( varStride[0] == 1);
	baseIx = ixList.FrontGetS(); //  * varStride[0]; // GetS() ok because of none INDEXED

	// check boundary
	for(SizeT i=1; i<acRank; ++i)
	{
		ixList[i]->NIter( (i<varRank)?varDim[i]:1);
		baseIx += ixList[i]->GetS() * varStride[i]; // GetS() ok because of none INDEXED
	}

	nIx = 1;
	return;
// 	varStride = var->Dim().Stride();
// 	// check boundary
// 	const dimension& varDim  = var->Dim();
// 	SizeT            varRank = varDim.Rank();
// 	for(SizeT i=0; i<acRank; ++i)
// 	ixList[i]->NIter( (i<varRank)?varDim[i]:1);
// 	nIx = 1;
// 	return;
      }
    if( accessType == ALLINDEXED || accessType == INDEXED_ONE)
      {
	SizeT i=0;
	for(; i<acRank; ++i)
		if( !ixList[i]->Scalar())
			break;

	if( i == acRank) // counted up to acRank -> all scalar
	{
		accessType = ALLONE; // needed for GetDim()
		const dimension& varDim  = var->Dim();
		SizeT            varRank = varDim.Rank();

		varStride = varDim.Stride();
		nIterLimitGt1 = 0; // marker for BuildIx

		ixList[0]->NIter( (0<varRank)?varDim[0]:1);
		assert( varStride[0] == 1);
		baseIx = ixList[0]->GetIx0(); //  * varStride[0]; // GetS() not ok because INDEXED

		// check boundary
		for(SizeT i=1; i<acRank; ++i)
		{
			ixList[i]->NIter( (i<varRank)?varDim[i]:1);
			baseIx += ixList[i]->GetIx0() * varStride[i]; // GetS() not ok because INDEXED
		}

		nIx = 1;
		return;			
// 			accessType = ALLONE;
// 			varStride = var->Dim().Stride();
// 			// check boundary
// 			const dimension& varDim  = var->Dim();
// 			SizeT            varRank = varDim.Rank();
// 			for(SizeT i=0; i<acRank; ++i)
// 			ixList[i]->NIter( (i<varRank)?varDim[i]:1);
// 			nIx = 1;
// 			return;
	}
	// after break
	if( i > 0 || accessType == INDEXED_ONE)
	{
	    accessType = NORMAL; // there was a scalar (and break because of non-scalar)
	}
	else // i == 0 -> first was (because of ALLINDEXED) indexed
	{
	    ++i; // first was already non-scalar -> indexed
	    for(; i<acRank; ++i)
		if( !ixList[i]->Indexed())
		{
		  accessType = NORMAL;
		  break;
		}
	    // else
	    //	accessType = ALLINDEXED; // is already
	}
    }

    // accessType can be at this point:
    // NORMAL
    // ALLINDEXED
    // both are the definite types here
    assert( accessType == NORMAL || accessType == ALLINDEXED);

    // set varDim from variable
    const dimension& varDim  = var->Dim();
    SizeT            varRank = varDim.Rank();

    varStride = var->Dim().Stride();

    if( accessType == ALLINDEXED)
    {
      baseIx = 0;
      
      nIx=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
      assert( nIx > 1);

      for( SizeT i=1; i<acRank; ++i)
      {
	  SizeT nIter = ixList[i]->NIter( (i<varRank)?varDim[i]:1);
	  if( nIter != nIx)
		  throw GDLException(-1,NULL, "All array subscripts must be of same size.", true, false);	
      }
      // in this case, having more index dimensions does not matter
      // indices are used only upto variables rank
      // ok as we set acRank here
      if( varRank < acRank) 
	acRank = varRank;
      //varDim.Stride( varStride,acRank); // copy variables stride into varStride
      return;
    }
    
    // NORMAL
    //     varDim.Stride( varStride,acRank); // copy variables stride into varStride
    assert( varStride[0] == 1);

    nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
    //nIx = nIterLimit[0]; // calc number of assignments
    stride[0]=1;

    if( nIterLimit[0] > 1)
    {
	    nIterLimitGt1 = 1; // important for BuildIx
	    gt1Rank = 0;
	    if( ixList[0]->Indexed())
	    {
		    baseIx = 0;
		    indexed = true;
	    }
	    else
	    {
		    baseIx = ixList[0]->GetS();// * varStride[0];
		    indexed = false;
	    }
    }
    else
    {
	    nIterLimitGt1 = 0; // important for BuildIx
	    if( ixList[0]->Indexed())
	    {
		    baseIx = static_cast< ArrayIndexIndexed*>( ixList[0])->GetIx0();// * varStride[0];
	    }
	    else
	    {
		    baseIx = ixList[0]->GetS();//  * varStride[0];
	    }
    }
    for( SizeT i=1; i<acRank; ++i)
    {
	    nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1);
	    //nIx *= nIterLimit[i]; // calc number of assignments
	    stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride

	    if( nIterLimit[i] > 1)
	    {
		    ++nIterLimitGt1;
		    gt1Rank = i;
		    if( ixList[i]->Indexed())
		    {
			    indexed = true;
		    }
		    else
		    {
			    baseIx += ixList[i]->GetS() * varStride[i];
			    indexed = false;
		    }
	    }
	    else
	    {
		    if( ixList[i]->Indexed())
		    {
			    baseIx += static_cast< ArrayIndexIndexed*>( ixList[i])->GetIx0() * varStride[i];
		    }
		    else
		    {
			    baseIx += ixList[i]->GetS()  * varStride[i];
		    }
	    }
    }
    stride[acRank]=stride[acRank-1]*nIterLimit[acRank-1]; // index stride
    nIx = stride[acRank];
  }

  // structure of indexed expression
  const dimension GetDim()
  {
    // should be changed to ALLINDEXED or ALLONE by now
    assert( accessType != INDEXED_ONE); 

    if( accessType == ALLONE) return dimension(); // -> results in scalar
    if( accessType == ALLINDEXED)
      { // always indexed
		return static_cast<ArrayIndexIndexed*>(ixList[0])->GetDim();
      }
    // accessType == NORMAL -> structure from indices
    return dimension( nIterLimit, acRank);
  }

  SizeT N_Elements()
  {
    return nIx;
  }

  // returns 1-dim index for all elements
  AllIxBaseT* BuildIx() // ArrayIndexListMultiT
  {
    if( allIx != NULL) // can happen if called from DotAccessDescT::DoAssign()
      return allIx;

    assert( allIx == NULL);

    if( accessType == ALLINDEXED) //  nIterLimitGt1 is not properly set
    {
	    // note that this indexer cannot live without this ArrayIndexListMultiT
	    allIx = new (allIxInstance) AllIxAllIndexedT( &ixList, acRank, nIx, varStride);
	    return allIx;
    }
    
    // can happen
    // this must be done only here as oterwise nIterLimitGt1 is not properly set
    if( nIterLimitGt1 == 0) // only one single index
    {
	    allIx = new (allIxInstance) AllIxT( baseIx);
	    return allIx;
    }
    assert( acRank > 1);

    // NORMAL
    // loop only over specified indices
    // higher indices of variable are implicitely zero,
    // therefore they are not checked in 'SetRoot'

    if( nIterLimitGt1 == 1) // only one variable dimension
    {
      if( indexed)
	      allIx = new (allIxInstance) AllIxNewMultiOneVariableIndexIndexedT( gt1Rank, baseIx, &ixList, acRank, nIx, varStride, nIterLimit, stride);
      else
	      allIx = new (allIxInstance) AllIxNewMultiOneVariableIndexNoIndexT( gt1Rank, baseIx, &ixList, acRank, nIx, varStride, nIterLimit, stride);
	    return allIx;
    }
    if( acRank == 2)
    {
      allIx = new (allIxInstance) AllIxNewMulti2DT( &ixList, nIx, varStride, nIterLimit, stride);
      return allIx;
    }
    allIx = new (allIxInstance) AllIxNewMultiT( &ixList, acRank, nIx, varStride, nIterLimit, stride);
    return allIx;
}

  // returns one dim long ix in case of one element array index
  // used by AssignAt and Index functions
  SizeT LongIx() const
  {
    SizeT dStart = ixList[0]->GetIx0();
    for( SizeT i=1; i < acRank; ++i)
		dStart += ixList[i]->GetIx0() * varStride[ i];
    return dStart;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    SetVariable( var);
    
    if( var->EqType( right))
      {
	var->AssignAt( right, this); // assigns inplace
      }
    else
      {
	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
	std::auto_ptr<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    // normal case
    Init( ix, NULL);
    SetVariable( var);
    if( nIx == 1)// && !var->IsAssoc())
    {
      BaseGDL* res = var->NewIx( baseIx);
      if( accessType != ALLONE)
	res->MakeArrayFromScalar();
      return res;
    }
    return var->Index( this);
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  const dimension GetDimIx0( SizeT& destStart)
  {
    SizeT dStart = 0;

    SizeT actIx[ MAXRANK];
    for( SizeT i=0; i < acRank; ++i)
      {
	actIx[ i] = ixList[i]->GetIx0();

	dStart += actIx[ i] * varStride[ i];
      }

    destStart = dStart;
    return dimension( actIx, acRank);
  }

  SizeT NDim()
  { return acRank;}
}; //class ArrayIndexListMultiT: public ArrayIndexListT



// some checks are not needed here
class ArrayIndexListMultiNoneIndexedNoAssocT: public ArrayIndexListMultiNoAssocT
{
	public:	
  // constructor
//   ArrayIndexListMultiNoneIndexedT
//   : ArrayIndexListMultiT()
//   {}

//   ArrayIndexListMultiNoneIndexedT( const ArrayIndexListMultiNoneIndexedT& cp):
//   ArrayIndexListMultiT( cp)
//   {}
// called after structure is fixed
  ArrayIndexListMultiNoneIndexedNoAssocT( ArrayIndexVectorT* ix)
// 	: ixList( *ix),
//     allIx( NULL),
//     ixListEnd( NULL)
  {
    ixList = *ix;
    allIx = NULL;
//     ixListEnd = NULL;
	  
    assert( ix->size() != 0); // must be, from compiler

    if( ixList.size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    nParam = 0;
    for( SizeT i=0; i<ix->size(); ++i)
	nParam += (*ix)[i]->NParam();

    // determine type of index
//     SizeT nIndexed = 0;
    SizeT nScalar  = 0;
    for( SizeT i=0; (i+1)<ixList.size(); ++i)
      {
		// note: here we examine the actual type
		if( ArrayIndexScalarID == ixList[i]->Type() ||
			ArrayIndexScalarVPID == ixList[i]->Type() || // ? (from MakeArrayIndex)
			CArrayIndexScalarID == ixList[i]->Type() ) nScalar++;
// 		else if( ArrayIndexIndexedID == ixList[i]->Type() ||
// 			CArrayIndexIndexedID == ixList[i]->Type()) nIndexed++;
      }
    if( nScalar == ixList.size()-1)
    	accessTypeAssocInit = ALLONE;
//     else if( nIndexed == ixList.size()-1)
//       accessTypeAssocInit = ALLINDEXED; // might be ALLONE as well
//     else if( nScalar + nIndexed < ixList.size()-1)
    else  
		accessTypeAssocInit = NORMAL;
//     else
//       accessTypeAssocInit = INDEXED_ONE;
    
	if( ArrayIndexScalarID == ixList[ixList.size()-1]->Type() ||
		ArrayIndexScalarVPID == ixList[ixList.size()-1]->Type() || // ? (from MakeArrayIndex)
		CArrayIndexScalarID == ixList[ixList.size()-1]->Type()) nScalar++;
// 	else if( ArrayIndexIndexedID == ixList[ixList.size()-1]->Type() ||
// 		CArrayIndexIndexedID == ixList[ixList.size()-1]->Type()) nIndexed++;
/*    if( dynamic_cast< ArrayIndexScalar*>(ixList[ixList.size()-1]) ||
	dynamic_cast< CArrayIndexScalar*>(ixList[ixList.size()-1])) nScalar++;
    if( dynamic_cast<ArrayIndexIndexed*>(ixList[ixList.size()-1]) ||
	dynamic_cast<CArrayIndexIndexed*>(ixList[ixList.size()-1]) ) nIndexed++;*/

	assert( nScalar <= ixList.size()); // from MakeArrayIndex
//     if( nScalar == ixList.size())
//       accessTypeInit = ALLONE;
// //     else if( nIndexed == ixList.size())
// //       accessTypeInit = ALLINDEXED; // might be ALLONE as well
// //     else if( nScalar + nIndexed < ixList.size())
//     else
		accessTypeInit = NORMAL;

//     std::cout << "accessTypeInit: " << accessTypeInit << std::endl;
  }
  
  ArrayIndexListT* Clone() { return new ArrayIndexListMultiNoneIndexedNoAssocT( *this);}

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // set acRank
    acRank = ixList.size();

    // for assoc variables last index is the record
//     if( var->IsAssoc()) 
//       {
// 		acRank--;
// 		accessType = accessTypeAssocInit;
//       }
//     else
      accessType = accessTypeInit;

    // can happen due to assoc variables
    if( accessType == ALLONE)
      {
	const dimension& varDim  = var->Dim();
	SizeT            varRank = varDim.Rank();

	varStride = varDim.Stride();
	nIterLimitGt1 = 0; // marker for BuildIx

	ixList[0]->NIter( (0<varRank)?varDim[0]:1);
	assert( varStride[0] == 1);
	baseIx = ixList.FrontGetS(); //  * varStride[0];

	// check boundary
	for(SizeT i=1; i<acRank; ++i)
	{
		ixList[i]->NIter( (i<varRank)?varDim[i]:1);
		baseIx += ixList[i]->GetS() * varStride[i];
	}

	nIx = 1;
	return;
      }

	// accessType can be at this point:
	// NORMAL
	// now the definite types here
	assert( accessType == NORMAL);
    
	// set varDim from variable
	const dimension& varDim  = var->Dim();
	SizeT                    varRank = varDim.Rank();
	
	varStride = varDim.Stride();
	//     varDim.Stride( varStride,acRank); // copy variables stride into varStride
	
	nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
// 	nIx = nIterLimit[0]; // calc number of assignments
	stride[0]=1;

	nIterLimitGt1 = (nIterLimit[0] > 1)? 1 : 0;
	gt1Rank = 0;
	assert( varStride[0] == 1);
	baseIx = ixList[0]->GetS(); //  * varStride[0];

	for( SizeT i=1; i<acRank; ++i)
      {
		nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1);
// 		nIx *= nIterLimit[i]; // calc number of assignments
		stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride
		
		if( nIterLimit[i] > 1)
		{
			++nIterLimitGt1;
			gt1Rank = i;
		}
		baseIx += ixList[i]->GetS()  * varStride[i];
      }
    stride[acRank]= stride[acRank-1]*nIterLimit[acRank-1]; // index stride 
    nIx = stride[acRank];
  }

  // returns 1-dim index for all elements
  AllIxBaseT* BuildIx()
  {
    if( allIx != NULL) // can happen if called from DotAccessDescT::DoAssign()
      return allIx;

	assert( allIx == NULL);  
// 	if( allIx != NULL)
// 		return allIx;

// 	if( accessType == ALLONE)
// 	{
// 		SizeT s = ixList.FrontGetS(); //ixList[0]->GetS();
// 		for( SizeT l=1; l < acRank; ++l)
// 		{
// 			s += ixList[l]->GetS() * varStride[l];
// 		}
// 		allIx = new (allIxInstance) AllIxT(s);
// 		return allIx;
// 	}
	
	// ALLONE or all nIterLimit == 1
	if( nIterLimitGt1 == 0) // only one single index
	{
		allIx = new (allIxInstance) AllIxT( baseIx);
		return allIx;
	}
	  
	assert( acRank > 1);  

	
	// NORMAL
	// loop only over specified indices
	// higher indices of variable are implicitely zero,
	// therefore they are not checked in 'SetRoot'
	if( nIterLimitGt1 == 1) // only one variable dimension
	{
		allIx = new (allIxInstance) AllIxNewMultiOneVariableIndexNoIndexT( gt1Rank, baseIx, &ixList, acRank, nIx, varStride, nIterLimit, stride);
		return allIx;
	}
	
	if( acRank == 2) // assoc already recognized
	{
		allIx = new (allIxInstance) AllIxNewMultiNoneIndexed2DT( &ixList, nIx, varStride, nIterLimit, stride);
		return allIx;
	}
	
	allIx = new (allIxInstance) AllIxNewMultiNoneIndexedT( &ixList, acRank, nIx, varStride, nIterLimit, stride);
	return allIx;
  }
}; // ArrayIndexListMultiNoneIndexedT



class ArrayIndexListMultiNoneIndexedNoAssoc2DT: public ArrayIndexListMultiNoAssocT
{
public:	
// called after structure is fixed
  ArrayIndexListMultiNoneIndexedNoAssoc2DT( ArrayIndexVectorT* ix)
  {
    ixList = *ix;
    allIx = NULL;
	  
    assert( ix->size() == 2); // must be, from compiler
    assert( ixList.size() == 2);

    acRank = acRank2D;

    nParam = 0;
    for( SizeT i=0; i<ix->size(); ++i)
	nParam += (*ix)[i]->NParam();

    // determine type of index
//     SizeT nIndexed = 0;
    SizeT nScalar  = 0;
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	// note: here we examine the actual type
	if( ArrayIndexScalarID == ixList[i]->Type() ||
		ArrayIndexScalarVPID == ixList[i]->Type() || // ? (from MakeArrayIndex)
		CArrayIndexScalarID == ixList[i]->Type() ) nScalar++;
      }
    assert( nScalar < ixList.size()); // from MakeArrayIndex
    accessTypeInit = NORMAL;
  }
  
  ArrayIndexListT* Clone() { return new ArrayIndexListMultiNoneIndexedNoAssoc2DT( *this);}

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
    // accessType must be at this point:
    // NORMAL
    // now the definite types here

    // set varDim from variable
    const dimension& varDim  = var->Dim();
    SizeT            varRank = varDim.Rank();
    
    varStride = varDim.Stride();
    //     varDim.Stride( varStride,acRank); // copy variables stride into varStride
    
    nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
    stride[0]=1;

    nIterLimitGt1 = (nIterLimit[0] > 1)? 1 : 0;
    gt1Rank = 0;
    assert( varStride[0] == 1);

    nIterLimit[1]=ixList[1]->NIter( (1<varRank)?varDim[1]:1);
    nIx = nIterLimit[0] * nIterLimit[1]; // calc number of assignments
    stride[1]=nIterLimit[0]; // index stride
		
    if( nIterLimit[1] > 1)
      {
	++nIterLimitGt1;
	gt1Rank = 1;
      }
    baseIx = ixList[0]->GetS() + ixList[1]->GetS() * varStride[1];

    stride[2]=nIx; // index stride 
  }

  // returns 1-dim index for all elements
  AllIxBaseT* BuildIx()
  {
    if( allIx != NULL) // can happen if called from DotAccessDescT::DoAssign()
      return allIx;

    assert( allIx == NULL);  
    
    // all nIterLimit == 1
    if( nIterLimitGt1 == 0) // only one single index
    {
      allIx = new (allIxInstance) AllIxT( baseIx);
      return allIx;
    }	  
    // NORMAL
    // loop only over specified indices
    // higher indices of variable are implicitely zero,
    // therefore they are not checked in 'SetRoot'
    if( nIterLimitGt1 == 1) // only one variable dimension
    {
	    allIx = new (allIxInstance) AllIxNewMultiOneVariableIndexNoIndexT( gt1Rank, baseIx, &ixList, acRank2D, nIx, varStride, nIterLimit, stride);
	    return allIx;
    }
    
    allIx = new (allIxInstance) AllIxNewMultiNoneIndexed2DT( &ixList, nIx, varStride, nIterLimit, stride);
    return allIx;
  }
}; // ArrayIndexListMultiNoneIndexed2DT





class ArrayIndexListMultiAllIndexedNoAssocT: public ArrayIndexListMultiNoAssocT
{
public:
  
  // called once after structure is fixed at (GDL-)compile time
  ArrayIndexListMultiAllIndexedNoAssocT( ArrayIndexVectorT* ix)
// 	: ixList( *ix),
//     allIx( NULL),
//     ixListEnd( NULL)
  {
    ixList = *ix;
    allIx = NULL;
//     ixListEnd = NULL;

    assert( ix->size() != 0); // must be, from compiler

    if( ixList.size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    nParam = 0;
    for( SizeT i=0; i<ix->size(); ++i)
	nParam += (*ix)[i]->NParam();

    accessTypeInit = ALLINDEXED; // might be ALLONE as well
  }
  

  ArrayIndexListT* Clone() { return new ArrayIndexListMultiAllIndexedNoAssocT( *this);}

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
	assert( allIx == NULL);

	// set acRank
	acRank = ixList.size();

	// for assoc variables last index is the record
// 	if( var->IsAssoc())
// 	{
// 		acRank--;
// 		accessType = accessTypeAssocInit;
// 	}
// 	else
		accessType = accessTypeInit;

	SizeT i=0;
	for(; i<acRank; ++i)
		if( !ixList[i]->Scalar())
			break;
	
	if( i == acRank) // counted up to acRank -> all scalar
	{
		accessType = ALLONE; // needed for GetDim() and Index (to return scalar instead of array)
		const dimension& varDim  = var->Dim();
		SizeT            varRank = varDim.Rank();

		varStride = varDim.Stride();
		nIterLimitGt1 = 0; // marker for BuildIx

		ixList[0]->NIter( (0<varRank)?varDim[0]:1);
		assert( varStride[0] == 1);
		baseIx = ixList[0]->GetIx0(); //  * varStride[0]; // GetS() not ok because INDEXED

		// check boundary
		for(SizeT i=1; i<acRank; ++i)
		{
			ixList[i]->NIter( (i<varRank)?varDim[i]:1);
			baseIx += ixList[i]->GetIx0() * varStride[i]; // GetS() not ok because INDEXED
		}

		nIx = 1;
		return;			
	}
	if( i > 0)
	{
		accessType = NORMAL; // there was a scalar (and break because of non-scalar)
	}
	else // i == 0 -> first was (because of ALLINDEXED) indexed
	{
		++i; // first was already non-scalar -> indexed
		for(; i<acRank; ++i)
			if( !ixList[i]->Indexed())
			{
				accessType = NORMAL;
				break;
			}
	}

	// accessType can be at this point:
	// NORMAL
	// ALLINDEXED
	// both are the definite types here
	assert( accessType == NORMAL || accessType == ALLINDEXED);

	// set varDim from variable
	const dimension& varDim  = var->Dim();
	SizeT            varRank = varDim.Rank();
    
	if( accessType == ALLINDEXED)
	{
		nIx=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
		for( SizeT i=1; i<acRank; ++i)
			{
				SizeT nIter = ixList[i]->NIter( (i<varRank)?varDim[i]:1);
				if( nIter != nIx)
					throw GDLException(-1,NULL, "All array subscripts must be of same size.", true, false);
			}

		// in this case, having more index dimensions does not matter
		// indices are used only upto variables rank
		// ok as we set acRank here
		if( varRank < acRank) 
		  acRank = varRank;

		varStride = var->Dim().Stride();
		return;
	}
    
	// NORMAL
	varStride = var->Dim().Stride();
	assert( varStride[0] == 1);

	nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
	nIx = nIterLimit[0]; // calc number of assignments
	stride[0]=1;

	if( nIterLimit[0] > 1)
	{
		nIterLimitGt1 = 1; // important for BuildIx
		gt1Rank = 0;
		if( ixList[0]->Indexed())
		{
			baseIx = 0;
			indexed = true;
		}
		else
		{
			baseIx = ixList[0]->GetS();// * varStride[0];
			indexed = false;
		}
	}
	else
	{
		nIterLimitGt1 = 0; // important for BuildIx
		if( ixList[0]->Indexed())
		{
			baseIx = static_cast< ArrayIndexIndexed*>( ixList[0])->GetIx0();// * varStride[0];
		}
		else
		{
			baseIx = ixList[0]->GetS();//  * varStride[0];
		}
	}
	for( SizeT i=1; i<acRank; ++i)
	{
		nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1);
		nIx *= nIterLimit[i]; // calc number of assignments
		stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride

		if( nIterLimit[i] > 1)
		{
			++nIterLimitGt1;
			gt1Rank = i;
			if( ixList[i]->Indexed())
			{
				indexed = true;
			}
			else
			{
				baseIx += ixList[i]->GetS() * varStride[i];
				indexed = false;
			}
		}
		else
		{
			if( ixList[i]->Indexed())
			{
				baseIx += static_cast< ArrayIndexIndexed*>( ixList[i])->GetIx0() * varStride[i];
			}
			else
			{
				baseIx += ixList[i]->GetS()  * varStride[i];
			}
		}
	}
	stride[acRank]=stride[acRank-1]*nIterLimit[acRank-1]; // index stride
  }
}; //class ArrayIndexListMultiAllIndexedT: public ArrayIndexListMultiT


#endif
