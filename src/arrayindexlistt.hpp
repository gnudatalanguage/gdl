/***************************************************************************
                          arrayindexlistt.hpp  -  array access descriptor
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

#ifndef ARRAYINDEXLISTT_HPP_
#define ARRAYINDEXLISTT_HPP_

#include "arrayindex.hpp"

class ArrayIndexListT
{
protected:

  SizeT nParam; // number of (BaseGDL*) parameters

public:    
  
  SizeT NParam() { return nParam;}

  virtual ~ArrayIndexListT();

  // constructor
  ArrayIndexListT() {}

  ArrayIndexListT( const ArrayIndexListT& cp):
    nParam( cp.nParam)
  {}
  
  virtual void Clear() {}

  virtual ArrayIndexListT* Clone() { assert( 0); return NULL;}

  virtual void InitAsOverloadIndex( IxExprListT& ixIn, /*IxExprListT* cleanupIx,*/ IxExprListT& ixOut) 
  { 
    throw GDLException( -1, NULL,"Internal error: ArrayIndexListT::InitAsOverladIndex( IxExprListT& ixInOut) called.",true,false);    
  }
  virtual void Init( IxExprListT& ix)//, IxExprListT* cleanupIx) 
  { 
    assert( 0);
    throw GDLException( -1, NULL,"Internal error: ArrayIndexListT::Init( IxExprListT& ix, IxExprListT* cleanupIx) called.",true,false);    
  }
  virtual IxExprListT* GetCleanupIx()
  { 
    assert( 0);
    throw GDLException( -1, NULL,"Internal error: ArrayIndexListT::GetCleanupIx() called.",true,false);    
  }

  virtual void Init() {}
  
   virtual bool ToAssocIndex( SizeT& lastIx) 
   { 
    assert( 0);       
    throw GDLException( -1, NULL,"Internal error: ArrayIndexListT::ToAssocIndex( SizeT& lastIx) called.",true,false);
   }
//  virtual bool ToAssocIndex( RangeT& lastIx) = 0;

  // set the root variable which is indexed by this ArrayIndexListT
  virtual void SetVariable( BaseGDL* var) {}

  // structure of indexed expression
   virtual const dimension GetDim() = 0;

  virtual SizeT N_Elements() = 0;

  // returns 1-dim index for all elements
  virtual AllIxBaseT* BuildIx();

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  virtual SizeT LongIx() const = 0;

  virtual void AssignAt( BaseGDL* var, BaseGDL* right) {}

  // optimized for one dimensional access
  // this is called from the interpreter and ARRAYEXPRNode::Eval()
  virtual BaseGDL* Index( BaseGDL* var, IxExprListT& ix) = 0;

  // returns multi-dim index for 1st element
  // used by InsAt functions
  virtual const dimension GetDimIx0( SizeT& destStart) = 0;
  virtual SizeT NDim() = 0;
};


#include "arrayindexlistnoassoct.hpp"

// only one index [ix],[s:e],...
// NEVER ArrayIndexScalar types (they have their own ArrayIndexListT)
class ArrayIndexListOneT: public ArrayIndexListT
{
private:
  IxExprListT cleanupIx;
	
  ArrayIndexT* ix;

  SizeT    nIx;                  // number of indexed elements

  AllIxBaseT* allIx;

  char allIxInstance[ AllIxMaxSize];
  
public:    
  IxExprListT* GetCleanupIx() { return &cleanupIx;}
    
  ~ArrayIndexListOneT()
  {
//     delete allIx;
    delete ix;
    cleanupIx.Cleanup(); // must be explicitely cleaned up
  }

  // constructor
  ArrayIndexListOneT()
	: cleanupIx()
    , ix( NULL)
    , allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneT( const ArrayIndexListOneT& cp):
    ArrayIndexListT( cp),
	cleanupIx(),
    ix( cp.ix->Dup()),
    allIx( NULL)
  {
    assert( cp.allIx == NULL);
    assert( cp.cleanupIx.size() == 0);
  }

  // called after structure is fixed
  ArrayIndexListOneT( ArrayIndexVectorT* aIV):
    allIx( NULL)
  {
    ix = (*aIV)[0];
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

  ArrayIndexListT* Clone() { return new ArrayIndexListOneT( *this);}

  void Init( IxExprListT& ix_)//, IxExprListT* cleanupIxIn)
  {
    assert( allIx == NULL);
    assert( ix_.size() == nParam);

//     if( cleanupIxIn != NULL)
//       cleanupIx = *cleanupIxIn;
    if( nParam == 0) return;
    if( nParam == 1)
    {
	ix->Init( ix_[ 0]);
    }
    else if( nParam == 2)
    {
	ix->Init( ix_[ 0], ix_[ 1]);
	return;
    }
    else // nParam == 3
    {
	ix->Init( ix_[ 0], ix_[ 1], ix_[ 2]);
	return;
    }
  }

  void Init() {} // eg. a[*]

// requires special handling
// used by Assoc_<> returns last index in lastIx, removes it
// and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    // cannot be ArrayIndexScalar[VP] ix->Init();
    RangeT lastValIx;
    if( !ix->Scalar( lastValIx))
      throw GDLException( -1, NULL,"Record number must be a scalar in this context.",true,false);

    if( lastValIx < 0)
      throw GDLException( -1, NULL,"Record number must be a scalar > 0 in this context.",true,false);

    lastIx = lastValIx;
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // for assoc variables last index is the record
    if( var->IsAssoc())
    {
	// note: s is copied from sIter in ArrayIndex::Init
       return;
    }
    // ArrayIndexScalar[VP] are not initialized
    // they need the NIter call, but
    // for only one index they have their own ArrayIndexListT
    nIx=ix->NIter( var->N_Elements()/*var->Size()*/);
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
    if( right->N_Elements() == 1 && !var->IsAssoc() &&
	ix->NIter( var->N_Elements()/*var->Size()*/) == 1)// && var->Type() != GDL_STRUCT) 
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
	Guard<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_)
  {
    Init( ix_);//, NULL);

    if( var->IsAssoc()) // deault case
      return var->Index( this);
    
    if( /*!var->IsAssoc() &&*/ ix->Scalar()) //ix->NIter( var->N_Elements()/*var->Size()*/) == 1)// && var->Type() != GDL_STRUCT) 
      {
	SizeT assertValue = ix->NIter( var->N_Elements()/*var->Size()*/);
	assert( assertValue == 1);

	return var->NewIx( ix->GetIx0());
      }
    // normal case, no assoc
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
class ArrayIndexListOneScalarT: public ArrayIndexListT
{
protected:
  SizeT varIx;
  RangeT sInit;
  RangeT s;

  AllIxT allIx;

public:    
  
  ~ArrayIndexListOneScalarT()
  {
//     delete allIx;
  }

  // constructor
  ArrayIndexListOneScalarT()
    : varIx( 0)
//     , allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneScalarT( const ArrayIndexListOneScalarT& cp)
    : ArrayIndexListT( cp)
    , varIx( cp.varIx)
    , s( cp.s)
//     , allIx( NULL)
  {
//     assert( cp.allIx == NULL);
  }

  // called after structure is fixed
  ArrayIndexListOneScalarT( ArrayIndexVectorT* aIV)
//     : allIx( NULL)
  : varIx(static_cast<ArrayIndexScalar*>((*aIV)[0])->GetVarIx())
  {
    nParam = 0;
//    
//    varIx = static_cast<ArrayIndexScalar*>((*aIV)[0])->GetVarIx();

    delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  ArrayIndexListT* Clone() { return new ArrayIndexListOneScalarT( *this);}

  void Init() {}

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx);

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





class ArrayIndexListOneScalarVPT: public ArrayIndexListT
{
protected:
  DVar* varPtr;

  RangeT sInit;
  RangeT s;

  AllIxT allIx;
//   AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneScalarVPT()
  {
//     delete allIx;
  }

  // constructor
  ArrayIndexListOneScalarVPT()
    : varPtr( NULL)
//     , allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneScalarVPT( const ArrayIndexListOneScalarVPT& cp)
    : ArrayIndexListT( cp)
    , varPtr( cp.varPtr)
    , sInit( cp.sInit)
    , s( cp.s)
//     , allIx( NULL)
  {
//     assert( cp.allIx == NULL);
  }

  // called after structure is fixed
  ArrayIndexListOneScalarVPT( ArrayIndexVectorT* aIV)
//     : allIx( NULL)
  {
    nParam = 0;
    
    varPtr = static_cast<ArrayIndexScalarVP*>((*aIV)[0])->GetVarPtr();

    delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  ArrayIndexListT* Clone() { return new ArrayIndexListOneScalarVPT( *this);}

  void Init() {}

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    sInit = varPtr->Data()->LoopIndex();
    if( sInit < 0)
      throw GDLException( -1, NULL,"Record number must be a scalar > 0 in this context.",true,false);
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
    if( var->IsAssoc()) return;
    if( s >= var->N_Elements()/*var->Size()*/)
      throw GDLException(-1, NULL,"Scalar subscript out of range (>).",true,false);
    if( s < 0)
      throw GDLException(-1,NULL,"Scalar subscript out of range (<-1).",true,false);
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
    if( right->N_Elements() == 1 && !var->IsAssoc()) // && var->Type() != GDL_STRUCT) 
      {
	s = varPtr->Data()->LoopIndex();
	if( s >= var->N_Elements()/*var->Size()*/)
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
	Guard<BaseGDL> conv_guard( rConv);
	
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





class ArrayIndexListOneConstScalarT: public ArrayIndexListT
{
  RangeT sInit;
  RangeT s;
  AllIxT allIx;
//   AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneConstScalarT() 
  {
//     delete allIx;
  }

  // constructor
  ArrayIndexListOneConstScalarT()
// 	: allIx( NULL)
  {
    nParam = 0;
  }

  ArrayIndexListOneConstScalarT( const ArrayIndexListOneConstScalarT& cp)
    : ArrayIndexListT( cp)
    , sInit( cp.sInit)
    , s( cp.s)
//     , allIx( NULL)
  {
//     assert( cp.allIx == NULL); // all copying should be done before using.
  }

  // called after structure is fixed
  ArrayIndexListOneConstScalarT( ArrayIndexVectorT* aIV)
  :sInit((*aIV)[0]->GetS())
//     : allIx( NULL)
  {
//    sInit = (*aIV)[0]->GetS();
    if( sInit >= 0) s = sInit;
    nParam = 0;

    delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  ArrayIndexListT* Clone() { return new ArrayIndexListOneConstScalarT( *this);}

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
    if( var->IsAssoc()) return;
    if( sInit < 0)
      s = sInit + var->N_Elements()/*var->Size()*/;
    // for assoc variables last index is the record
    if( s < 0)
      throw GDLException(-1,NULL,"Scalar subscript out of range [<].1",true,false);
    if( s >= var->N_Elements()/*var->Size()*/)
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
    if( right->N_Elements() == 1 && !var->IsAssoc())// && var->Type() != GDL_STRUCT) 
      {
	if( sInit < 0)
	  s = sInit + var->N_Elements()/*var->Size()*/;
	if( s < 0)
	  throw GDLException(-1,NULL,"Scalar subscript out of range [<].2",true,false);
	if( s >= var->N_Elements()/*var->Size()*/)
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
		Guard<BaseGDL> conv_guard( rConv);
	
		var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc())// && var->Type() != GDL_STRUCT)
      {
	if( sInit < 0)
	  s = sInit + var->N_Elements()/*var->Size()*/;
	if( s < 0)
		throw GDLException(-1,NULL,"Scalar subscript out of range [<].3",true,false);
	if( s >= var->N_Elements()/*var->Size()*/)
	{
// 	    std::cout << s << " var->N_Elements()/*var->Size()*/:" << var->N_Elements()/*var->Size()*/ << std::endl;
		throw GDLException(-1,NULL,"Scalar subscript out of range [>].3",true,false);
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
class ArrayIndexListScalarT: public ArrayIndexListT
{
private:
  ArrayIndexVectorT ixList;
//   std::vector<SizeT> paramPresent;

  SizeT    acRank;               // rank upto which indexing is done
  const SizeT*    varStride; // variables stride
//   SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

  AllIxT allIx;
//   AllIxT allIxInstance;

  ArrayIndexT* ixListEnd; // for assoc index

public:    
  
  ~ArrayIndexListScalarT()
  {
//     delete allIx;
	ixList.Destruct();
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin();
// 	 i != ixList.end(); ++i)
//       {	delete *i;}
  }

  // constructor
  ArrayIndexListScalarT():
    acRank(0),
//     allIx( NULL),
    ixListEnd( NULL)
  { nParam = 0;}

  ArrayIndexListScalarT( const ArrayIndexListScalarT& cp):
    ArrayIndexListT( cp),
//     paramPresent( cp.paramPresent),
    acRank(cp.acRank),
//     allIx( NULL),
    ixListEnd( NULL)
  {
    //    ixList.reserve(MAXRANK); 
//     assert( cp.allIx == NULL);
    assert( cp.ixListEnd == NULL);

    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());
  }

  // called after structure is fixed
  ArrayIndexListScalarT( ArrayIndexVectorT* ix):
    ixList( *ix),
//     allIx( NULL),
    ixListEnd( NULL)
  {
    assert( ixList.size() > 1); // must be, from compiler
    
    if( ix->size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    nParam = 0;
    for( SizeT i=0; i<ixList.size(); ++i)
      {
		SizeT actNParam = ixList[i]->NParam();
		if( actNParam == 1) 
		{
// 			paramPresent.push_back( i);
			nParam++;
		}
      }
  }    
  
  void Clear()
  {
//     delete allIx;
//     allIx = NULL;
    
    if( ixListEnd != NULL) // revert assoc indexing
      {
		ixList.push_back( ixListEnd);
		ixListEnd = NULL;
      }
//     // no clearing of scalar indices
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
// 	 i != ixList.end(); ++i)
//       {	(*i)->Clear();}
  }

//   void Init( IxExprListT& ix)
//   {
//     assert( allIx == NULL);
//     assert( ix.size() == nParam);
    
//     for( SizeT i=0; i<nParam; ++i)
//       {
// 	ixList[ /*paramPresent*/[i]]->Init( ix[ i]);
//       }
//   }

  ArrayIndexListT* Clone() { return new ArrayIndexListScalarT( *this);}

  void Init()
  {}
  
  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true if the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    assert( ixListEnd == NULL);
    
    ixListEnd = ixList.pop_back_get();
    
    // init in case of ixListEnd->NParam == 0
    ixListEnd->Init();

    RangeT lastIxVal;
    ixListEnd->Scalar( lastIxVal); // always scalar

    if( lastIxVal < 0)
      throw GDLException(-1,NULL,"Record number must be a scalar > 0 in this context.",true,false);
    
    lastIx = lastIxVal;
    return false; // multi dim
  }

  // set the root variable which is indexed by this ArrayIndexListScalarT
  void SetVariable( BaseGDL* var) 
  {
//     assert( allIx == NULL);
    
    // set acRank
    acRank = ixList.size();
    
    // for assoc variables last index is the record
    if( var->IsAssoc()) 
      {
	acRank--;
	// if( acRank == 0) return; // multi dim
      }

    // ArrayIndexScalar[VP] need this call to read their actual data
    // as their are not initalized (nParam == 0)
    for( SizeT i=0; i<acRank; ++i)
      ixList[i]->NIter( var->Dim(i)); // check boundary

    varStride = var->Dim().Stride();
//     var->Dim().Stride( varStride, acRank); // copy variables stride into varStride
    nIx = 1;
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
    if( var->N_Elements() == 1 && !var->IsAssoc())
    {
      // SetVariable( var);
      // set acRank
      acRank = ixList.size();

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
    assert( nIx == 1);    
    if( var->EqType( right))
      {
	var->AssignAt( right, this); // assigns inplace (not only scalar)
      }
    else
      {
	BaseGDL* rConv = right->Convert2( var->Type(), BaseGDL::COPY);
	Guard<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace (not only scalar)
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    //    Init();
    // SetVariable( var);
    // set acRank
    acRank = ixList.size();
    // for assoc variables last index is the record
    if( var->IsAssoc()) 
      {
	acRank--;
	varStride = var->Dim().Stride();
	// ArrayIndexScalar[VP] need this call to read their actual data
	// as their are not initalized (nParam == 0)
	ixList[0]->NIter( var->Dim(0)); // check boundary
	for( SizeT i=1; i < acRank; ++i)
	{
	  ixList[i]->NIter( var->Dim(i)); // check boundary
	}
//     return dStart;
//     for( SizeT i=0; i<acRank; ++i)
//       ixList[i]->NIter( var->Dim(i)); // check boundary
//    	return var->NewIx( dStart); //this->LongIx());
	return var->Index( this);
      }

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
//     return dStart;
//     for( SizeT i=0; i<acRank; ++i)
//       ixList[i]->NIter( var->Dim(i)); // check boundary
    return var->NewIx( dStart); //this->LongIx());
//    return var->Index( this);
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




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// general case (mixed, multi-dim) at least one indexed
// but note: last index can be assoc index
class ArrayIndexListMultiT: public ArrayIndexListT
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

  ArrayIndexT* ixListEnd; // for assoc index

  // for access with only a single variable index (column/row-extractor)
  SizeT nIterLimitGt1; // how many dimensions > 1
  RankT gt1Rank; // which rank is the variable rank
  SizeT baseIx; // offset to add for all other constant dims
  bool indexed; // is the variable index indexed?

public:    
    IxExprListT* GetCleanupIx() { return &cleanupIx;}

  ~ArrayIndexListMultiT()
  {
//     delete allIx;
    ixList.Destruct();
//     for( std::vector<ArrayIndexT*>::iterator i=ixList.begin();
// 	 i != ixList.end(); ++i)
//       {	delete *i;}
	cleanupIx.Cleanup();
  }

  // constructor
  ArrayIndexListMultiT():
    accessType(NORMAL),
    acRank(0),
    allIx( NULL),
    ixListEnd( NULL)
  { nParam = 0;}
	
  ArrayIndexListMultiT( const ArrayIndexListMultiT& cp):
    ArrayIndexListT( cp),
    accessType(cp.accessType),
    accessTypeInit(cp.accessTypeInit),
    accessTypeAssocInit(cp.accessTypeAssocInit),
    acRank(cp.acRank),
    allIx( NULL),
    ixListEnd( NULL)
  {
    assert( cp.allIx == NULL);
    assert( cp.ixListEnd == NULL);
	assert( cp.cleanupIx.size() == 0);
	
    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());
  }

  // called once after structure is fixed at (GDL-)compile time
  ArrayIndexListMultiT( ArrayIndexVectorT* ix):
    ixList( *ix),
    allIx( NULL),
    ixListEnd( NULL)
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
    
    if( ixListEnd != NULL) // revert assoc indexing
      {
	ixList.push_back( ixListEnd);
	ixListEnd = NULL;
      }

    ixList.Clear();
//     for( ArrayIndexVectorT::iterator i=ixList.begin(); i != ixList.end(); ++i)
//       {	(*i)->Clear();}
    cleanupIx.Cleanup();
  }

  ArrayIndexListT* Clone() { return new ArrayIndexListMultiT( *this);}


  void Init( IxExprListT& ix)//, IxExprListT* cleanupIxIn)
  {
    assert( allIx == NULL);
    assert( ix.size() == nParam);
    	
// 	if( cleanupIxIn != NULL)
// 		cleanupIx = *cleanupIxIn;

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
    assert( ixListEnd == NULL);
    
    ArrayIndexT* ixListEndTmp = ixList.back();
    ixListEndTmp->Init();
    
    RangeT lastValIx;
    if( !ixListEndTmp->Scalar( lastValIx))
      throw GDLException(-1, NULL ,"Record number must be a scalar in this context.",true,false);

    if( lastValIx < 0)
      throw GDLException(-1, NULL,"Record number must be a scalar > 0 in this context.",true,false);

    lastIx = lastValIx;
   
    ixListEnd = ixListEndTmp;
    ixList.pop_back();
    
    return false; // multi dim ixList.empty();
  }

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // set acRank
    acRank = ixList.size();

    // for assoc variables last index is the record
    if( var->IsAssoc()) 
      {
	acRank--;
	accessType = accessTypeAssocInit;
      }
    else
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

    if( accessType == ALLINDEXED)
    {
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
      if( varRank < acRank) acRank = varRank;

      varStride = var->Dim().Stride();
// 		varDim.Stride( varStride,acRank); // copy variables stride into varStride
      return;
    }
    
    // NORMAL
    varStride = var->Dim().Stride();
    //     varDim.Stride( varStride,acRank); // copy variables stride into varStride
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
    
    if( acRank == 1) // assoc already recognized
    {
	ArrayIndexT* ix = ixList[0];
	if(  ix->Indexed())
	{
	    allIx = static_cast< ArrayIndexIndexed*>(ix)->GetAllIx();
	    return allIx;
	}
	if( nIx == 1)
	{
	    allIx = new (allIxInstance) AllIxT( ix->GetS());
	    return allIx;
	}
	SizeT s = ix->GetS();
	SizeT ixStride = ix->GetStride();
	if( ixStride <= 1) 
	if( s != 0)
	    {
	    allIx = new (allIxInstance) AllIxRangeT( nIx, s);
	    }
	else
	    {
	    allIx = new (allIxInstance) AllIxRange0T( nIx);
	    }
	else
	if( s != 0)
	    {
	    allIx = new (allIxInstance) AllIxRangeStrideT( nIx, s, ixStride);
	    }
	else
	    {
	    allIx = new (allIxInstance) AllIxRange0StrideT( nIx, ixStride);
	    }
	return allIx;
    }

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
	Guard<BaseGDL> conv_guard( rConv);
	
	var->AssignAt( rConv, this); // assigns inplace
      }
  }

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix)
  {
    // normal case
    Init( ix);//, NULL);
    SetVariable( var);
    if( nIx == 1 && !var->IsAssoc())
    {
      BaseGDL* res = var->NewIx( baseIx);
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
class ArrayIndexListMultiNoneIndexedT: public ArrayIndexListMultiT
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
  ArrayIndexListMultiNoneIndexedT( ArrayIndexVectorT* ix)
// 	: ixList( *ix),
//     allIx( NULL),
//     ixListEnd( NULL)
  {
    ixList = *ix;
    allIx = NULL;
    ixListEnd = NULL;
	  
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
  
  ArrayIndexListT* Clone() { return new ArrayIndexListMultiNoneIndexedT( *this);}

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // set acRank
    acRank = ixList.size();

    // for assoc variables last index is the record
    if( var->IsAssoc()) 
      {
		acRank--;
		accessType = accessTypeAssocInit;
      }
    else
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
	nIx = nIterLimit[0]; // calc number of assignments
	stride[0]=1;

	nIterLimitGt1 = (nIterLimit[0] > 1)? 1 : 0;
	gt1Rank = 0;
	assert( varStride[0] == 1);
	baseIx = ixList[0]->GetS(); //  * varStride[0];

	for( SizeT i=1; i<acRank; ++i)
      {
		nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1);
		nIx *= nIterLimit[i]; // calc number of assignments
		stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride
		
		if( nIterLimit[i] > 1)
		{
			++nIterLimitGt1;
			gt1Rank = i;
		}
		baseIx += ixList[i]->GetS()  * varStride[i];
      }
    stride[acRank]=stride[acRank-1]*nIterLimit[acRank-1]; // index stride 
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
	  
	if( acRank == 1) // assoc already recognized
	{
		ArrayIndexT* ix = ixList[0];
		if( nIx == 1)
		{
			allIx = new (allIxInstance) AllIxT( ix->GetS());
			return allIx;
		}
		SizeT s = ix->GetS();
		SizeT ixStride = ix->GetStride();
		if( ixStride <= 1) 
		if( s != 0)
			{
			allIx = new (allIxInstance) AllIxRangeT( nIx, s);
			}
		else
			{
			allIx = new (allIxInstance) AllIxRange0T( nIx);
			}
		else
		if( s != 0)
			{
			allIx = new (allIxInstance) AllIxRangeStrideT( nIx, s, ixStride);
			}
		else
			{
			allIx = new (allIxInstance) AllIxRange0StrideT( nIx, ixStride);
			}
		return allIx;
 	}

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












class ArrayIndexListMultiAllIndexedT: public ArrayIndexListMultiT
{
public:
  
  // called once after structure is fixed at (GDL-)compile time
  ArrayIndexListMultiAllIndexedT( ArrayIndexVectorT* ix)
// 	: ixList( *ix),
//     allIx( NULL),
//     ixListEnd( NULL)
  {
    ixList = *ix;
    allIx = NULL;
    ixListEnd = NULL;

    assert( ix->size() != 0); // must be, from compiler

    if( ixList.size() > MAXRANK)
      throw GDLException(-1,NULL,"Maximum of "+MAXRANK_STR+" dimensions allowed.",true,false);

    nParam = 0;
    for( SizeT i=0; i<ix->size(); ++i)
	nParam += (*ix)[i]->NParam();

    accessTypeAssocInit = ALLINDEXED; // might be ALLONE as well
    accessTypeInit = ALLINDEXED; // might be ALLONE as well
  }
  

  ArrayIndexListT* Clone() { return new ArrayIndexListMultiAllIndexedT( *this);}

  // set the root variable which is indexed by this ArrayIndexListMultiT
  void SetVariable( BaseGDL* var) 
  {
	assert( allIx == NULL);

	// set acRank
	acRank = ixList.size();

	// for assoc variables last index is the record
	if( var->IsAssoc())
	{
		acRank--;
		accessType = accessTypeAssocInit;
	}
	else
		accessType = accessTypeInit;

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
		if( varRank < acRank) acRank = varRank;

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









class ArrayIndexListGuard
{
private:
  ArrayIndexListT* aL;
public:
  ArrayIndexListGuard(): aL( NULL) {}
  ArrayIndexListGuard(ArrayIndexListT* aLToGuard): aL( aLToGuard) {}
  ~ArrayIndexListGuard() 
  {
    if( aL != NULL)
      aL->Clear();
  }
  void reset( ArrayIndexListT* aL_) { aL = aL_;}
  ArrayIndexListT* release() { ArrayIndexListT* res = aL; aL = NULL; return res;}
  void Release() { aL = NULL;}
};



// called after structure is fixed (code in arrayindex.cpp)
//ArrayIndexListT* MakeArrayIndex( ArrayIndexVectorT* ixList);
void MakeArrayIndex( ArrayIndexVectorT* ixList, 
				  ArrayIndexListT** arrayIndexOut, 
				  ArrayIndexListT** arrayIndexNoAssocOut = NULL);



#endif
