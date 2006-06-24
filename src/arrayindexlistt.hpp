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

  virtual void Init( IxExprListT& ix) { assert( 0);}
  virtual void Init() {}
  
  virtual bool ToAssocIndex( SizeT& lastIx) {}

  // set the root variable which is indexed by this ArrayIndexListT
  virtual void SetVariable( BaseGDL* var) {}

  // structure of indexed expression
  virtual dimension GetDim() {}

  virtual SizeT N_Elements() {}

  // returns 1-dim index for all elements
  virtual AllIxT* BuildIx();

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  virtual SizeT LongIx() {}

  virtual void AssignAt( BaseGDL* var, BaseGDL* right) {}

  // optimized for one dimensional access
  virtual BaseGDL* Index( BaseGDL* var, IxExprListT& ix) {}

  // returns multi-dim index for 1st element
  // used by InsAt functions
  virtual dimension GetDimIx0( SizeT& destStart) {}
  virtual SizeT NDim() {}
};

// only one index [ix],[s:e],...
// NEVER ArrayIndexScalar types (they have their own ArrayIndexListT)
class ArrayIndexListOneT: public ArrayIndexListT
{
private:
  ArrayIndexT* ix;

  SizeT    nIx;                  // number of indexed elements

  AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneT()
  {
    delete allIx;
    delete ix;
  }

  // constructor
  ArrayIndexListOneT():
    ix( NULL),
    allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneT( const ArrayIndexListOneT& cp):
    ArrayIndexListT( cp),
    ix( cp.ix->Dup()),
    allIx( NULL)
  {
    assert( cp.allIx == NULL);
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
    delete allIx;
    allIx = NULL;
    
    ix->Clear();
  }

  void Init( IxExprListT& ix_)
  {
    assert( allIx == NULL);
    assert( ix_.size() == nParam);
    
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
    if( !ix->Scalar( lastIx))
      throw GDLException( "Record number must be a scalar in this context.");
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);

    // for assoc variables last index is the record
    if( var->IsAssoc()) return;

    // ArrayIndexScalar[VP] are not initialized
    // they need the NIter call, but
    // for only one index they have their own ArrayIndexListT
    nIx=ix->NIter( var->Size());
  }
  
  // structure of indexed expression
  dimension GetDim()
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
  AllIxT* BuildIx()
  {
    if( allIx != NULL) return allIx;

    if( ix->Indexed())
      {
	allIx = static_cast< ArrayIndexIndexed*>(ix)->StealIx();
	return allIx;
      }
    else
      {
	if( nIx == 1)
	  {
	    allIx = new AllIxT( ix->GetS(), 1);
	    return allIx;
	  }

	allIx = new AllIxT( nIx);
	SizeT s = ix->GetS();
	SizeT ixStride = ix->GetStride();
	if( ixStride <= 1) 
	  if( s != 0) 
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] = i + s;
	  else
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] = i;
	else
	  if( s != 0) 
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] = i * ixStride + s;
	  else
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] = i * ixStride;
	return allIx;
      }
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx()
  {
    return ix->GetIx0();
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // scalar case
    if( right->N_Elements() == 1 && !var->IsAssoc() &&
	ix->NIter( var->Size()) == 1 && var->Type() != STRUCT) 
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
    if( !var->IsAssoc() && var->Type() != STRUCT)
      return ix->Index( var, ix_);
    
    // normal case
    Init( ix_);
    SetVariable( var);
    return var->Index( this);
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& destStart)
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
  SizeT s;

  AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneScalarT()
  {
    delete allIx;
  }

  // constructor
  ArrayIndexListOneScalarT():
    varIx( 0),
    allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneScalarT( const ArrayIndexListOneScalarT& cp):
    ArrayIndexListT( cp),
    varIx( cp.varIx),
    s( cp.s),
    allIx( NULL)
  {
    assert( cp.allIx == NULL);
  }

  // called after structure is fixed
  ArrayIndexListOneScalarT( ArrayIndexVectorT* aIV):
    allIx( NULL)
  {
    nParam = 0;
    
    varIx = static_cast<ArrayIndexScalar*>((*aIV)[0])->GetVarIx();

    delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  void Init() {}

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx);

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var);
  
  // structure of indexed expression
  dimension GetDim()
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
    if( allIx != NULL)
      {
	allIx[0] = s;
	return allIx;
      }

    allIx = new AllIxT( s, 1);
    return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx()
  {
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right);

  // optimized for one dimensional access
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_);

  // returns multi-dim index for 1st element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& destStart)
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

  SizeT s;

  AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneScalarVPT()
  {
    delete allIx;
  }

  // constructor
  ArrayIndexListOneScalarVPT():
    varPtr( NULL),
    allIx( NULL)
  { nParam = 0;}

  ArrayIndexListOneScalarVPT( const ArrayIndexListOneScalarVPT& cp):
    ArrayIndexListT( cp),
    varPtr( cp.varPtr),
    s( cp.s),
    allIx( NULL)
  {
    assert( cp.allIx == NULL);
  }

  // called after structure is fixed
  ArrayIndexListOneScalarVPT( ArrayIndexVectorT* aIV):
    allIx( NULL)
  {
    nParam = 0;
    
    varPtr = static_cast<ArrayIndexScalarVP*>((*aIV)[0])->GetVarPtr();

    delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  void Init() {}

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    s = varPtr->Data()->LoopIndex();
    lastIx = s;
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    s = varPtr->Data()->LoopIndex();

    // for assoc variables last index is the record
    if( var->IsAssoc()) return;
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].1");
  }
  
  // structure of indexed expression
  dimension GetDim()
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
    if( allIx != NULL)
      {
	allIx[0] = s;
	return allIx;
      }

    allIx = new AllIxT( s, 1);
    return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx()
  {
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1 && !var->IsAssoc() && var->Type() != STRUCT) 
      {
	s = varPtr->Data()->LoopIndex();
	if( s >= var->Size())
	  throw GDLException("Scalar subscript out of range [>].2");
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
  dimension GetDimIx0( SizeT& destStart)
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
  SizeT s;
  AllIxT* allIx;

public:    
  
  ~ArrayIndexListOneConstScalarT() 
  {
    delete allIx;
  }

  // constructor
  ArrayIndexListOneConstScalarT(): allIx( NULL)
  {
    nParam = 0;
  }

  ArrayIndexListOneConstScalarT( const ArrayIndexListOneConstScalarT& cp):
    ArrayIndexListT( cp),
    s( cp.s),
    allIx( NULL)
  {
    assert( cp.allIx == NULL); // all copying should be done before using.
  }

  // called after structure is fixed
  ArrayIndexListOneConstScalarT( ArrayIndexVectorT* aIV):
    allIx( NULL)
  {
    s = (*aIV)[0]->GetS();
    nParam = 0;

    delete (*aIV)[0];
  }    
  
  void Clear()
  {}

  void Init()
  {}

  SizeT N_Elements()
  {
    return 1;
  }

  // returns 1-dim index for all elements
  AllIxT* BuildIx()
  {
    if( allIx != NULL)
      return allIx;

    allIx = new AllIxT( s, 1);
    return allIx;
  }

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    lastIx = s;
    return true;
  }

  // set the root variable which is indexed by this ArrayIndexListT
  void SetVariable( BaseGDL* var) 
  {
    // for assoc variables last index is the record
    if( var->IsAssoc()) return;
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].1");
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx()
  {
    return s;
  }

  void AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1 && !var->IsAssoc() && var->Type() != STRUCT) 
      {
	if( s >= var->Size())
	  throw GDLException("Scalar subscript out of range [>].2");
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
  BaseGDL* Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc() && var->Type() != STRUCT)
      {
	if( s >= var->Size())
	  {
	    std::cout << s << " var->Size():" << var->Size() << std::endl;
	    throw GDLException("Scalar subscript out of range [>].3");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    Init();// ix_);
    SetVariable( var);
    return var->Index( this);
  }

  dimension GetDim()
  {
    return dimension();
  }

  dimension GetDimIx0( SizeT& destStart)
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
  std::vector<SizeT> paramPresent;

  SizeT    acRank;               // rank upto which indexing is done
  SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

  AllIxT* allIx;

  ArrayIndexT* ixListEnd; // for assoc index

public:    
  
  ~ArrayIndexListScalarT()
  {
    delete allIx;
    for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
	 i != ixList.end(); ++i)
      {	delete *i;}
  }

  // constructor
  ArrayIndexListScalarT():
    acRank(0),
    allIx( NULL),
    ixListEnd( NULL)
  { nParam = 0;}

  ArrayIndexListScalarT( const ArrayIndexListScalarT& cp):
    ArrayIndexListT( cp),
    paramPresent( cp.paramPresent),
    acRank(cp.acRank),
    allIx( NULL),
    ixListEnd( NULL)
  {
    //    ixList.reserve(MAXRANK); 
    assert( cp.allIx == NULL);
    assert( cp.ixListEnd == NULL);

    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());
  }

  // called after structure is fixed
  ArrayIndexListScalarT( ArrayIndexVectorT* ix):
    ixList( *ix),
    allIx( NULL),
    ixListEnd( NULL)
  {
    assert( ixList.size() > 1); // must be, from compiler
    
    if( ix->size() > MAXRANK)
      throw GDLException("Maximum of "+MAXRANK_STR+" dimensions allowed.");

    nParam = 0;
    for( SizeT i=0; i<ixList.size(); ++i)
      {
	SizeT actNParam = ixList[i]->NParam();
	if( actNParam == 1) 
	  {
	    paramPresent.push_back( i);
	    nParam++;
	  }
      }
  }    
  
  void Clear()
  {
    delete allIx;
    allIx = NULL;
    
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
// 	ixList[ paramPresent[i]]->Init( ix[ i]);
//       }
//   }

  void Init()
  {}
  

  // requires special handling
  // used by Assoc_<> returns last index in lastIx, removes it
  // and returns true is the list is empty
  bool ToAssocIndex( SizeT& lastIx)
  {
    assert( ixListEnd == NULL);
    
    ixListEnd = ixList.back();
    ixList.pop_back();
    
    // init in case of ixListEnd->NParam == 0
    ixListEnd->Init();

    ixListEnd->Scalar( lastIx);

    return false; // multi dim
  }

  // set the root variable which is indexed by this ArrayIndexListScalarT
  void SetVariable( BaseGDL* var) 
  {
    assert( allIx == NULL);
    
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

    var->Dim().Stride( varStride,acRank); // copy variables stride into varStride
    nIx = 1;
  }

  // structure of indexed expression
  dimension GetDim()
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
    if( allIx != NULL) return allIx;

    SizeT s = ixList[0]->GetS();
    for( SizeT l=1; l < acRank; ++l)
      {
	s += ixList[l]->GetS() * varStride[l]; 
      }
    allIx = new AllIxT( 1);
    (*allIx)[0] = s;

    return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx()
  {
    if( acRank == 1)
      return ixList[0]->GetS();
    
    SizeT dStart = ixList[0]->GetS();
    for( SizeT i=1; i < acRank; ++i)
      dStart += ixList[i]->GetS() * varStride[ i];
    
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
    //    Init();
    SetVariable( var);
    return var->Index( this);
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& destStart)
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

// general case (mixed, multi-dim)
class ArrayIndexListMultiT: public ArrayIndexListT
{
private:
  ArrayIndexVectorT ixList;

  enum AccessType {
    UNDEF=0,      // for init access type
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
  SizeT    stride[MAXRANK+1];    // for each dimension, how many iterations
  SizeT    varStride[MAXRANK+1]; // variables stride
  SizeT    nIx;                  // number of indexed elements

  AllIxT*      allIx;

  ArrayIndexT* ixListEnd; // for assoc index

public:    
  
  ~ArrayIndexListMultiT()
  {
    delete allIx;
    for( std::vector<ArrayIndexT*>::iterator i=ixList.begin(); 
	 i != ixList.end(); ++i)
      {	delete *i;}
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

    for( SizeT i=0; i<cp.ixList.size(); ++i)
      ixList.push_back( cp.ixList[i]->Dup());
  }

  // called after structure is fixed
  ArrayIndexListMultiT( ArrayIndexVectorT* ix):
    ixList( *ix),
    allIx( NULL),
    ixListEnd( NULL)
  {
    assert( ix->size() != 0); // must be, from compiler

    if( ixList.size() > MAXRANK)
      throw GDLException("Maximum of "+MAXRANK_STR+" dimensions allowed.");

    nParam = 0;
    for( SizeT i=0; i<ix->size(); ++i)
	nParam += (*ix)[i]->NParam();

    // determine type of index
    SizeT nIndexed = 0;
    SizeT nScalar  = 0;
    for( SizeT i=0; (i+1)<ixList.size(); ++i)
      {
	// note: here we examine the actual type
	if( dynamic_cast< ArrayIndexScalar*>(ixList[i]) ||
	    dynamic_cast< CArrayIndexScalar*>(ixList[i])) nScalar++;
	if( dynamic_cast< ArrayIndexIndexed*>(ixList[i]) ||
	    dynamic_cast< CArrayIndexIndexed*>(ixList[i])) nIndexed++;
      }
    if( nScalar == ixList.size()-1)
      accessTypeAssocInit = ALLONE;
    else if( nIndexed == ixList.size()-1)
      accessTypeAssocInit = ALLINDEXED; // might be ALLONE as well
    else if( nScalar + nIndexed < ixList.size()-1)
      accessTypeAssocInit = NORMAL;
    else
      accessTypeAssocInit = INDEXED_ONE;
    
    if( dynamic_cast< ArrayIndexScalar*>(ixList[ixList.size()-1]) ||
	dynamic_cast< CArrayIndexScalar*>(ixList[ixList.size()-1])) nScalar++;
    if( dynamic_cast<ArrayIndexIndexed*>(ixList[ixList.size()-1]) ||
	dynamic_cast<CArrayIndexIndexed*>(ixList[ixList.size()-1]) ) nIndexed++;
    
    if( nScalar == ixList.size())
      accessTypeInit = ALLONE;
    else if( nIndexed == ixList.size())
      accessTypeInit = ALLINDEXED; // might be ALLONE as well
    else if( nScalar + nIndexed < ixList.size())
      accessTypeInit = NORMAL;
    else
      accessTypeInit = INDEXED_ONE;
  }
  
  void Clear()
  {
    delete allIx;
    allIx = NULL;
    
    if( ixListEnd != NULL) // revert assoc indexing
      {
	ixList.push_back( ixListEnd);
	ixListEnd = NULL;
      }
    
    for( ArrayIndexVectorT::iterator i=ixList.begin(); i != ixList.end(); ++i)
      {	(*i)->Clear();}
  }

  void Init( IxExprListT& ix)
  {
    assert( allIx == NULL);
    assert( ix.size() == nParam);
    
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
    if( !ixListEndTmp->Scalar( lastIx))
      throw GDLException( "Record number must be a scalar in this context.");

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
    if( accessType == ALLONE)
      {
	var->Dim().Stride( varStride,acRank); // copy variables stride into varStride
	nIx = 1;
	return;
      }

    if( accessType == ALLINDEXED || accessType == INDEXED_ONE)
      {
	SizeT i=0;
	for(; i<acRank; ++i)
	  if( !ixList[i]->Scalar())
	      break;

	if( i == acRank) // counted up to acRank
	  {
	    accessType = ALLONE;
	    var->Dim().Stride( varStride,acRank); // copy variables stride into varStride
	    nIx = 1;
	    return; 
	  }
	// after break
	if( i > 0 || accessType == INDEXED_ONE)
	  accessType = NORMAL; // there was a scalar (and break because of non-scalar)
	else // i == 0 -> first was (actually) indexed 
	  {
	    ++i; // first was already non-scalar -> indexed
	    for(; i<acRank; ++i)
	      if( !ixList[i]->Indexed())
		break;
	    if( i < acRank)
	      accessType = NORMAL;
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
	      throw GDLException( "All array subscripts must be of same size.");
	  }
	
	// in this case, having more index dimensions does not matter
	// indices are used only upto variables rank
	if( varRank < acRank) acRank = varRank;
	
	varDim.Stride( varStride,acRank); // copy variables stride into varStride
	
	return;
      }
    
    // NORMAL
    nIterLimit[0]=ixList[0]->NIter( (0<varRank)?varDim[0]:1);
    nIx = nIterLimit[0]; // calc number of assignments
    stride[0]=1;
    for( SizeT i=1; i<acRank; ++i)
      {
	nIterLimit[i]=ixList[i]->NIter( (i<varRank)?varDim[i]:1);
	nIx *= nIterLimit[i]; // calc number of assignments

	stride[i]=stride[i-1]*nIterLimit[i-1]; // index stride 
      }
    stride[acRank]=stride[acRank-1]*nIterLimit[acRank-1]; // index stride 
    
    varDim.Stride( varStride,acRank); // copy variables stride into varStride
  }

  // structure of indexed expression
  dimension GetDim()
  {
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
  AllIxT* BuildIx()
  {
    if( allIx != NULL) return allIx;

    if( accessType == ALLONE)
      {
	SizeT s = ixList[0]->GetS();
	for( SizeT l=1; l < acRank; ++l)
	  {
	    s += ixList[l]->GetS() * varStride[l]; 
	  }
	allIx = new AllIxT( 1);
	(*allIx)[0] = s;

	return allIx;
      }

    if( accessType == ALLINDEXED)
      {
	// ALLINDEXED -> all ArrayIndexT::INDEXED
	allIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();
	
	for( SizeT l=1; l < acRank; ++l)
	  {
	    AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[l])->StealIx();
	    
	    for( SizeT i=0; i<nIx; ++i)
	      (*allIx)[i] += (*tmpIx)[i] * varStride[l];
	    
	    delete tmpIx;
	  }
	return allIx;
      }

    // NORMAL
    // loop only over specified indices
    // higher indices of variable are implicitely zero,
    // therefore they are not checked in 'SetRoot'

    allIx = new AllIxT( nIx);

    // init allIx from first index
    if( ixList[0]->Indexed())
      {
	AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[0])->StealIx();

	for( SizeT i=0; i<nIx; ++i)
	  {
	    (*allIx)[ i] = (*tmpIx)[ i %  nIterLimit[0]];
	  }

	delete tmpIx;
      }
    else
      {
	SizeT s = ixList[0]->GetS();
	SizeT ixStride = ixList[0]->GetStride();
	
	if( ixStride <= 1)
	  if( s != 0) 
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]) + s; // stride[0], varStride[0] == 1
	      }
	  else
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]); // stride[0], varStride[0] == 1
	      }
	else
	  if( s != 0) 
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]) * ixStride + s; // stride[0], varStride[0] == 1
	      }
	  else
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[i] = (i %  nIterLimit[0]) * ixStride; // stride[0], varStride[0] == 1
	      }
      }

    for( SizeT l=1; l < acRank; ++l)
      {

	if( ixList[l]->Indexed())
	  {
	    AllIxT* tmpIx = static_cast< ArrayIndexIndexed*>(ixList[l])->StealIx();
	    //	    SizeT* tmpIx = ixList[l]->StealIx();
	    
	    for( SizeT i=0; i<nIx; ++i)
	      {
		(*allIx)[ i] += (*tmpIx)[ (i / stride[l]) %  nIterLimit[l]] * 
		  varStride[l];
	      }
	    
	    delete tmpIx;
	  }
	else
	  {
	    SizeT s = ixList[l]->GetS();
	    SizeT ixStride = ixList[l]->GetStride();
	    
	    if( ixStride <= 1)
	    if( s != 0) 
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += ((i / stride[l]) %  nIterLimit[l] + s) * 
		    varStride[l]; 
		}
	    else
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += ((i / stride[l]) %  nIterLimit[l]) * 
		    varStride[l]; 
		}
	    else // ixStride > 1 
	    if( s != 0) 
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += (((i / stride[l]) %  nIterLimit[l]) 
				  * ixStride + s) * varStride[l]; 
		}
	    else
	      for( SizeT i=0; i<nIx; ++i)
		{
		  (*allIx)[i] += ((i * ixStride / stride[l]) %  nIterLimit[l]) 
		    * ixStride * varStride[l]; 
		}
	  }
      }
    
    return allIx;
  }

  // returns one dim long ix in case of one element array index
  // used by AssignAt functions
  SizeT LongIx()
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
    Init( ix);
    SetVariable( var);
    return var->Index( this);
  }

  // returns multi-dim index for 1st element
  // used by InsAt functions
  dimension GetDimIx0( SizeT& destStart)
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

class ArrayIndexListGuard
{
private:
  ArrayIndexListT* aL;
public:
  ArrayIndexListGuard(): aL( NULL) {}
  ~ArrayIndexListGuard() 
  {
    if( aL != NULL)
      aL->Clear();
  }
  void reset( ArrayIndexListT* aL_) { aL = aL_;}
  ArrayIndexListT* release() { ArrayIndexListT* res = aL; aL = NULL; return res;}
};

#endif
