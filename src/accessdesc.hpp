/***************************************************************************
     accessdesc.hpp  -  accessing structs, sometimes also called "extractor"
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

#ifndef ACCESSDESC_HPP_
#define ACCESSDESC_HPP_

#include <vector>

#include "typedefs.hpp"
#include "datatypes.hpp"
#include "arrayindexlistt.hpp"
#include "assocdata.hpp"
#include "initsysvar.hpp"

// #ifdef _OPENMP
// #include "objects.hpp"
// #endif

class DotAccessDescT
{
private:
//   SizeT                         targetDepth;
  bool                          propertyAccess;
  DString                       propertyName;
  BaseGDL*                      top;
  std::vector<DStructGDL*>	dStruct; // structures
  std::vector<SizeT>            tag;     // tag index
  std::vector<ArrayIndexListT*> ix;      // array index

  dimension dim;
  SizeT rStride;
  SizeT rOffset;

  bool owner; // delete dStruct[0] upon own destruction

  // does the assignment
  void DoAssign( DStructGDL* l, BaseGDL* r, SizeT depth=0)
  {
    SizeT nCp;
    SizeT actTag=tag[ depth];
    if( ix[depth] == NULL)
      { // non-indexed case
	nCp=dStruct[depth]->N_Elements();

	if( (depth+1) == tag.size())
	  { // the actual assignment
	    for( SizeT c=0; c<nCp; c++)
	      {
		BaseGDL* actTop=l->GetTag( actTag, c);

		actTop->AssignAt( r, ix[depth+1], rOffset);
		rOffset += rStride;
	      }
	  }
	else
	  {
	    for( SizeT c=0; c<nCp; c++)
	      {
		DStructGDL* nextL = static_cast
		  <DStructGDL*>( l->GetTag( actTag, c));

		DoAssign( nextL, r, depth+1);
	      }
	  }
      }
    else
      { // indexed case
	ArrayIndexListT& actIx= *ix[depth];
	nCp=actIx.N_Elements();

	AllIxBaseT* allIx = actIx.BuildIx();

	if( (depth+1) == tag.size())
	  {
		BaseGDL* actTop = l->GetTag( actTag,   allIx->InitSeqAccess());
		actTop->AssignAt( r, ix[depth+1], rOffset);
		rOffset += rStride;
	    for( SizeT c=1; c<nCp; ++c)
	      { // the actual assignment
			actTop = l->GetTag( actTag,  allIx->SeqAccess());
			//BaseGDL* actTop = l->Get( actTag,  actIx.GetIx( c));
			actTop->AssignAt( r, ix[depth+1], rOffset);
			rOffset += rStride;
	      }
	  }
	else
	  {
		DStructGDL* nextL = static_cast<DStructGDL*>( l->GetTag( actTag, allIx->InitSeqAccess()));
		DoAssign( nextL, r, depth+1);
	    for( SizeT c=1; c<nCp; ++c)
	      {
			nextL = static_cast<DStructGDL*>( l->GetTag( actTag, allIx->SeqAccess()));
			//<DStructGDL*>( l->Get( actTag, actIx.GetIx( c)));
			DoAssign( nextL, r, depth+1);
	      }
	  }
      }
  }

  // does the decrement
  void DoDec( DStructGDL* l, SizeT depth=0)
  {
    SizeT nCp;
    SizeT actTag=tag[ depth];
    if( ix[depth] == NULL)
      { // non-indexed case
	nCp=dStruct[depth]->N_Elements();

	if( (depth+1) == tag.size())
	  { // the actual decrement
	    for( SizeT c=0; c<nCp; c++)
	      {
		BaseGDL* actTop=l->GetTag( actTag, c);
		actTop->DecAt( ix[depth+1]);
	      }
	  }
	else
	  {
	    for( SizeT c=0; c<nCp; c++)
	      {
		DStructGDL* nextL = static_cast
		  <DStructGDL*>( l->GetTag( actTag, c));
		DoDec( nextL, depth+1);
	      }
	  }
      }
    else
      { // indexed case
		ArrayIndexListT& actIx= *ix[depth];
		nCp=actIx.N_Elements();

		AllIxBaseT* allIx = actIx.BuildIx();

		if( (depth+1) == tag.size())
		{
			BaseGDL* actTop = l->GetTag( actTag,  allIx->InitSeqAccess());
			actTop->DecAt( ix[depth+1]);
			for( SizeT c=1; c<nCp; ++c)
			{ // the actual decrement
				actTop = l->GetTag( actTag,  allIx->SeqAccess());
				//BaseGDL* actTop = l->Get( actTag,  actIx.GetIx( c));
				actTop->DecAt( ix[depth+1]);	
			}
	  }
	else
	  {
		DStructGDL* nextL = static_cast<DStructGDL*>( l->GetTag( actTag, allIx->InitSeqAccess()));
		DoDec( nextL, depth+1);
	    for( SizeT c=1; c<nCp; ++c)
	      {
			nextL = static_cast<DStructGDL*>( l->GetTag( actTag, allIx->SeqAccess()));
			DoDec( nextL, depth+1);
	      }
	  }
      }
  }
  // does the increment
  void DoInc( DStructGDL* l, SizeT depth=0)
  {
    SizeT nCp;
    SizeT actTag=tag[ depth];
    if( ix[depth] == NULL)
      { // non-indexed case
	nCp=dStruct[depth]->N_Elements();

	if( (depth+1) == tag.size())
	  { // the actual increment
	    for( SizeT c=0; c<nCp; c++)
	      {
		BaseGDL* actTop=l->GetTag( actTag, c);
		actTop->IncAt( ix[depth+1]);
	      }
	  }
	else
	  {
	    for( SizeT c=0; c<nCp; c++)
	      {
		DStructGDL* nextL = static_cast
		  <DStructGDL*>( l->GetTag( actTag, c));
		DoInc( nextL, depth+1);
	      }
	  }
      }
    else
      { // indexed case
	ArrayIndexListT& actIx= *ix[depth];
	nCp=actIx.N_Elements();

	AllIxBaseT* allIx = actIx.BuildIx();

	if( (depth+1) == tag.size())
	  {
		BaseGDL* actTop = l->GetTag( actTag,  allIx->InitSeqAccess());
		actTop->IncAt( ix[depth+1]);
	    for( SizeT c=1; c<nCp; ++c)
	      { // the actual increment
			actTop = l->GetTag( actTag,  allIx->SeqAccess());
			//BaseGDL* actTop = l->Get( actTag,  actIx.GetIx( c));
			actTop->IncAt( ix[depth+1]);	
	      }
	  }
	else
	  {
		DStructGDL* nextL = static_cast<DStructGDL*>( l->GetTag( actTag, allIx->InitSeqAccess()));
		DoInc( nextL, depth+1);
	    for( SizeT c=1; c<nCp; c++)
	      {
			nextL = static_cast
			<DStructGDL*>( l->GetTag( actTag, allIx->SeqAccess()));
			//<DStructGDL*>( l->Get( actTag, actIx.GetIx( c)));
			DoInc( nextL, depth+1);
	      }
	  }
      }
  }

  // does the actual resolving
  void DoResolve( BaseGDL* newVar, DStructGDL* l, SizeT depth=0)
  {
    SizeT nCp;
    SizeT actTag=tag[ depth];
    if( ix[depth] == NULL)
      { // non-indexed case
	nCp=dStruct[depth]->N_Elements();

	if( (depth+1) == tag.size())
	  { // the actual resolving
	    for( SizeT c=0; c<nCp; c++)
	      {
			BaseGDL* actTop=l->GetTag( actTag, c);

			// newVar is empty -> no deleting of old data in InsertAt
			newVar->InsertAt( rOffset, actTop, ix[depth+1]);
			rOffset += rStride;
	      }
	  }
	else
	  {
	    for( SizeT c=0; c<nCp; c++)
	      {
			DStructGDL* nextL = static_cast<DStructGDL*>( l->GetTag( actTag, c));
			DoResolve( newVar, nextL, depth+1);
	      }
	  }
      }
    else
      { // indexed case
	ArrayIndexListT& actIx= *ix[depth];
	nCp=actIx.N_Elements();

	AllIxBaseT* allIx = actIx.BuildIx();

	if( (depth+1) == tag.size())
	  {
		BaseGDL* actTop = l->GetTag( actTag,  allIx->InitSeqAccess());
		newVar->InsertAt( rOffset, actTop, ix[depth+1]);
		rOffset += rStride;
	    for( SizeT c=1; c<nCp; ++c)
	      { // the actual resolving
			actTop = l->GetTag( actTag,  allIx->SeqAccess());
			//BaseGDL* actTop = l->Get( actTag,  actIx.GetIx( c));
			newVar->InsertAt( rOffset, actTop, ix[depth+1]);	
			rOffset += rStride;
	      }
	  }
	else
	  {
		DStructGDL* nextL = static_cast
		  <DStructGDL*>( l->GetTag( actTag, allIx->InitSeqAccess()));
		DoResolve( newVar, nextL, depth+1);
	    for( SizeT c=1; c<nCp; ++c)
	      {
			nextL = static_cast
			<DStructGDL*>( l->GetTag( actTag, allIx->SeqAccess()));
			//<DStructGDL*>( l->Get( actTag, actIx.GetIx( c)));
			DoResolve( newVar, nextL, depth+1);
	      }
	  }
      }
  }

  // intializes dim (implicitely checks rank)
  void SetupDim()
  {
    // calculate dimension
    SizeT nDot=tag.size();
    SizeT d;
    for( d=0; d<nDot; ++d)
    {
      if( ix[d] == NULL)
      { // loop over all elements
	if( dStruct[d]->N_Elements() > 1)
		dim >> dStruct[d]->Dim();
      }
      else
      {
	ix[d]->SetVariable( dStruct[d]);
	if( ix[d]->N_Elements() > 1)
		dim >> ix[d]->GetDim();
      }
    }
    //     dimension topDim;
    if( ix[d] == NULL)
    { // loop over all elements
    // 	topDim=top->Dim();
    // 	dim >> topDim;
	    dim >> top->Dim();
    }
    else
    {
	    ix[d]->SetVariable( top);
    // 	topDim=ix[d]->GetDim();
    // 	dim >> topDim;
	    dim >> ix[d]->GetDim();
    }
  }
  
private:
  DotAccessDescT() {} 

public:
  DotAccessDescT( SizeT depth): 
    propertyAccess(false), top(NULL), dStruct(), tag(), ix(), dim(), owner(false)
  {  
    dStruct.reserve( depth);//-1);
    tag.reserve( depth);//-1);
    ix.reserve( depth);
  }

  ~DotAccessDescT()
  {
    // delete root struct if owner
    if( owner && dStruct.size() > 0) delete dStruct[0];
    
    // the indices have to be cleared in any case
    SizeT nIx=ix.size();
    for( SizeT i=0; i<nIx; i++) 
      if( ix[i] != NULL) ix[i]->Clear();
  }
  
  void SetOwner( bool o)
  { owner = o;}
  bool IsOwner()
  { return owner;}

  // resloves (structure hierarchy described by) this to BaseGDL
  BaseGDL* ADResolve()
  {
    SetupDim();

    BaseGDL* newData;
    // no zeroing, here the new variable is created 
    // zero only for GDL_PTR and GDL_OBJ (because of ref counting)
    if( top->Type() == GDL_PTR || top->Type() == GDL_OBJ)
      newData=top->New( dim);//, BaseGDL::NOZERO);
    else
      newData=top->New( dim, BaseGDL::NOZERO);

    rOffset=0; // crucial line, only with rOffset == 0 var is set
 
    if( ix.back() == NULL) 
      rStride=top->N_Elements();
    else
      rStride=ix.back()->N_Elements();

    DoResolve( newData, dStruct[0]);

    return newData;
  }

  // assigns r to (structure hierarchy described by) this
  void ADAssign( BaseGDL* r)
  {
    SetupDim();

    SizeT lRank=dim.Rank();
    SizeT rRank=r->Rank();

//     if( rRank > lRank)
//       throw GDLException(NULL,"Conflicting data structures (rank).",true,false);

    SizeT topRank=top->Rank();

    SizeT rElem=r->N_Elements();

    SizeT topElem;

    if( ix.back() == NULL)
      topElem=top->N_Elements();
    else
      topElem=ix.back()->N_Elements();
    
    if( rElem <= topElem)
      {
	 // IDL seems to allow a maximum of one rank more for the r-value
// 	if( rRank > (topRank+1))
// 	  throw GDLException(NULL,"Conflicting data structures (top-rank).",true,false);

	// inplace copy to every instance of top
	// just loop over all top elements and insert (at appropriate indices)
	rStride=0;
	rOffset=0;
	
	if( r->Type() != top->Type())
	  {
	    BaseGDL* rConv = r->Convert2( top->Type(), BaseGDL::COPY);
	    std::auto_ptr<BaseGDL> conv_guard( rConv);

	    DoAssign( dStruct[0], rConv);
	  }
	else
	  DoAssign( dStruct[0], r);
      }
    else
      {
	// different slices into different top element instances
	// all dimensions must match here
	for( SizeT i=0; i<rRank; i++)
	  if( dim[i] != r->Dim(i))
	    throw GDLException(NULL,"Conflicting data structures (dim).",true,false);

	// copy only topRank dimensions each time (topElem elements)
	// topRank is the dim to start the outer loop with
/*	if( ix[ tag.size()]->N_Elements() == 1 &&
	    ix[ tag.size()]->NDim() == 1)
		rStride = 1;
	else*/
		rStride=r->Stride(topRank);
	rOffset=0;

	if( r->Type() != top->Type())
	  {
	    BaseGDL* rConv = r->Convert2( top->Type(), BaseGDL::COPY);
	    std::auto_ptr<BaseGDL> conv_guard( rConv);

	    DoAssign( dStruct[0], rConv);
	  }
	else
	  DoAssign( dStruct[0], r);
      }
/*#ifdef _OPENMP
    if( dStruct[0] == SysVar::Cpu())
	{
		SysVar::CPUChanged();
	}
#endif*/
  }

  void Dec()
  {
    SetupDim();
    DoDec( dStruct[0]);
/*#ifdef _OPENMP
    if( dStruct[0] == SysVar::Cpu())
		{
		SysVar::CPUChanged();
		}
#endif*/
  }
  void Inc()
  {
    SetupDim();
    DoInc( dStruct[0]);
/*#ifdef _OPENMP
    if( dStruct[0] == SysVar::Cpu())
		{
		SysVar::CPUChanged();
		}
#endif*/
  }

  DStructGDL* TopStruct()
  {
    return dStruct.back();
  }

  void ADRoot( DStructGDL* s, ArrayIndexListT* ix_=NULL) // root
  {
//     if( s->IsAssoc())
//       throw GDLException(NULL,"File expression not allowed in this context.",true,false);
    propertyAccess = false;
    dStruct.push_back(s);
    ix.push_back(ix_); 
  }

  void ADAdd( const std::string& tagName) // tags
  {
    if( dStruct.back() == NULL)
    {
      // Note: as this is the fail condition, this handling won't slow down normal operation
//       if( top->Type() == GDL_OBJ) // we must keep "top" as the resolve will be done with this.
//       {
// 	// must only have one time property access
// 	if( propertyAccess)
// 	{
// 	  throw GDLException(NULL,"Cannot access tag: "+ tagName+" [of property: "+propertyName+"].",true,false);
// 	}
// 	propertyName = tagName;
// 	propertyAccess = true;
// 	return; // no further change
// 	// hence "Add( SizeT)" will fail next time as well -> no further action here
//       }
      assert( top != NULL);
      if( top->Type() == GDL_OBJ) 
	throw GDLException(NULL,"Nested structure references are not allowed with objects. Consider using parentheses: "+tagName);
      else 
	throw GDLException(NULL,"Left side of a tag must be a STRUCT: "+tagName);
    }
   
    int t=dStruct.back()->Desc()->TagIndex( tagName);
    if( t == -1) 
    {
      // TODO: Check for call to Get/SetProperty
      
      throw GDLException(NULL,"Tag name: "+tagName+" is undefined for STRUCT.",true,false);
    }
    // call SizeT version
    SizeT tagIx=static_cast<SizeT>(t);
    ADAdd( tagIx);
  }


  void ADAdd( SizeT tagN) // tags
  {
    DStructGDL* actTop=dStruct.back();

    if( actTop == NULL)
      throw GDLException(NULL,"Expression must be a STRUCT in this context.",true,false);
    
    if( actTop->N_Elements() == 0) // maybe not needed
      throw GDLException(NULL,"Error struct data empty.",true,false);
    
    SizeT nTags=actTop->Desc()->NTags();
    
    if( tagN >= nTags)
      throw GDLException(NULL,"Invalid tag number.",true,false);

    // TODO: Insert object struct for Get/SetProperty
    // tagN == -1 (change type to int)?
      
    top=actTop->GetTag( tagN, 0);

    // push struct onto struct stack
    if( top->Type() == GDL_STRUCT)
    {
      DStructGDL* newTop=static_cast<DStructGDL*>(top);
      dStruct.push_back( newTop);
    }
    else
    {
      dStruct.push_back( NULL);      
    }
    tag.push_back(tagN);
  }

  void ADAddIx( ArrayIndexListT* ix_) // tags
  {
    if( propertyAccess && ix_ != NULL)
	  throw GDLException(NULL,"Property must not be indexed: "+propertyName+".",true,false);      
    ix.push_back(ix_); 
  }

};

// typedef std::vector<DotAccessDescT*> AccessStackT;

#endif

