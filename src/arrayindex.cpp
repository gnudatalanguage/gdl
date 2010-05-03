/***************************************************************************
                          arrayindex.cpp  -  array access descriptor
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

#include "includefirst.hpp"

#include "GDLInterpreter.hpp"
#include "arrayindex.hpp"

using namespace std;

ArrayIndexScalar::ArrayIndexScalar( RefDNode& dNode)
{
  assert( dNode->getType() == GDLTokenTypes::VAR);
  varIx = dNode->GetVarIx();
}
ArrayIndexScalarVP::ArrayIndexScalarVP( RefDNode& dNode)
{
  assert( dNode->getType() == GDLTokenTypes::VARPTR); 
  varPtr = dNode->GetVar();
}

// if this is used, Init was NOT called before
BaseGDL* ArrayIndexScalar::Index( BaseGDL* var, IxExprListT& ixL)
{
  s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();

  if( s >= var->Size())
    {
      throw GDLException("Scalar subscript out of range [>].a");
    }
  
  return var->NewIx( s);
}
BaseGDL* ArrayIndexScalarVP::Index( BaseGDL* var, IxExprListT& ixL)
{
  s = varPtr->Data()->LoopIndex();

  if( s >= var->Size())
    {
      throw GDLException("Scalar subscript out of range [>].b");
    }
  
  return var->NewIx( s);
}

SizeT ArrayIndexScalar::NIter( SizeT varDim) 
{
  s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
  
  if( s>0 && s >= varDim)
    throw GDLException("Scalar subscript out of range [>].c");
  return 1;
}
SizeT ArrayIndexScalarVP::NIter( SizeT varDim) 
{
  s = varPtr->Data()->LoopIndex();

  if( s>0 && s >= varDim)
    throw GDLException("Scalar subscript out of range [>].d");
  return 1;
}

void ArrayIndexScalar::Init() 
{
  s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
}


// // void ArrayIndexListOneScalarT::Init( IxExprListT& ix_)
// // {
// //   s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
// // }
// // void ArrayIndexListOneScalarVPT::Init( IxExprListT& ix_)
// // {
// //   s = varPtr->Data()->LoopIndex();
// // }
// void ArrayIndexListOneScalarT::Init()
// {
//   //  assert( 0);
//   //  s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
// }
// void ArrayIndexListOneScalarVPT::Init()
// {
//   //  assert( 0);
//   //  s = varPtr->Data()->LoopIndex();
// }

// optimized for one dimensional access
BaseGDL* ArrayIndexListOneScalarT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc() && var->Type() != STRUCT)
      {
	s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	  
	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].e");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    //    Init();
    SetVariable( var);
    return var->Index( this);
  }
BaseGDL* ArrayIndexListOneScalarVPT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc() && var->Type() != STRUCT)
      {
	s = varPtr->Data()->LoopIndex();

	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].f");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    //    Init();
    SetVariable( var);
    return var->Index( this);
  }

bool ArrayIndexListOneScalarT::ToAssocIndex( SizeT& lastIx)
  {
    s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
    lastIx = s;
    return true;
  }
void ArrayIndexListOneScalarT::SetVariable( BaseGDL* var) 
  {
    s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();

    // for assoc variables last index is the record
    if( var->IsAssoc()) return;
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].g");
  }
void ArrayIndexListOneScalarT::AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1 && !var->IsAssoc() && var->Type() != STRUCT) 
      {
	s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	if( s >= var->Size())
	  throw GDLException("Scalar subscript out of range [>].h");
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


// vtable
ArrayIndexListT::~ArrayIndexListT() {}

AllIxT* ArrayIndexListT::BuildIx() {}


// called after structure is fixed
ArrayIndexListT* MakeArrayIndex( ArrayIndexVectorT* ixList)
{
  assert( ixList->size() != 0); // must be, from compiler
  
  if( ixList->size() == 1)
    {
      if( CArrayIndexScalarID == (*ixList)[0]->Type())
	return new ArrayIndexListOneConstScalarT( ixList);
      
      if( ArrayIndexScalarID == (*ixList)[0]->Type())
	return new ArrayIndexListOneScalarT( ixList);
      
      if( ArrayIndexScalarVPID == (*ixList)[0]->Type())
	return new ArrayIndexListOneScalarVPT( ixList);
      
      return new ArrayIndexListOneT( ixList);
    }
  
  SizeT nScalar  = 0;
  for( SizeT i=0; i<ixList->size(); ++i)
    {
      if( ArrayIndexScalarID == (*ixList)[i]->Type() ||
	  ArrayIndexScalarVPID == (*ixList)[i]->Type() ||
	  CArrayIndexScalarID == (*ixList)[i]->Type() ) ++nScalar;
    }
  if( nScalar == ixList->size())
    return new ArrayIndexListScalarT( ixList);
  
  return new ArrayIndexListMultiT( ixList);
}
