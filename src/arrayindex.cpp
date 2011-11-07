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

// // if this is used, Init was NOT called before
// // only called if !var->IsAssoc()
// BaseGDL* ArrayIndexScalar::Index( BaseGDL* var, IxExprListT& ixL)
// {
//   s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
// 
//   if( s >= var->Size())
//     {
//       throw GDLException("Scalar subscript out of range [>].a");
//     }
//   return var->NewIx( s);
// }
// BaseGDL* ArrayIndexScalarVP::Index( BaseGDL* var, IxExprListT& ixL)
// {
//   s = varPtr->Data()->LoopIndex();
// 
//   if( s >= var->Size())
//     {
//       throw GDLException("Scalar subscript out of range [>].b");
//     }
//   return var->NewIx( s);
// }

SizeT ArrayIndexScalar::NIter( SizeT varDim) 
{
  sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
  if( sInit < 0)
    s = sInit + varDim;
  else
    s = sInit;
  
  if( s < 0)
    throw GDLException("Scalar subscript out of range [<].c");
  if( s >= varDim)
    throw GDLException("Scalar subscript out of range [>].c");
  return 1;
}
SizeT ArrayIndexScalarVP::NIter( SizeT varDim) 
{
  s = varPtr->Data()->LoopIndex();
  if( sInit < 0)
    s = sInit + varDim;
  else
    s = sInit;
  
  if( s < 0)
    throw GDLException("Scalar subscript out of range [<].d");
  if( s >= varDim)
    throw GDLException("Scalar subscript out of range [>].d");
  return 1;
}

void ArrayIndexScalar::Init() 
{
  sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
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

  // vtable
  ArrayIndexListOneScalarNoAssocT::~ArrayIndexListOneScalarNoAssocT()
  {}

// optimized for one dimensional access
BaseGDL* ArrayIndexListOneScalarT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc())// && var->Type() != STRUCT)
      {
	sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	  
	if( sInit < 0)
	  s = sInit + var->Size();
	else
	  s = sInit;
	
	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].e");
	  }
	if( s < 0)
	  {
	    throw GDLException("Scalar subscript out of range [<].e");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    //    Init();
    SetVariable( var);
    return var->Index( this);
  }
BaseGDL* ArrayIndexListOneScalarNoAssocT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
//     if( !var->IsAssoc())// && var->Type() != STRUCT)
//       {
	sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	  
	if( sInit < 0)
	  s = sInit + var->Size();
	else
	  s = sInit;
	
	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].e");
	  }
	if( s < 0)
	  {
	    throw GDLException("Scalar subscript out of range [<].e");
	  }
	    
	return var->NewIx( s);
//       }
//     
//     // normal case
//     //    Init();
//     SetVariable( var);
//     return var->Index( this);
  }
BaseGDL* ArrayIndexListOneScalarVPT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc())// && var->Type() != STRUCT)
      {
	sInit = varPtr->Data()->LoopIndex();
	if( sInit < 0)
	  s = sInit + var->Size();
	else
	  s = sInit;

	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].f");
	  }
	if( s < 0)
	  {
	    throw GDLException("Scalar subscript out of range [<].f");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    //    Init();
    SetVariable( var);
    return var->Index( this);
  }
BaseGDL* ArrayIndexListOneScalarVPNoAssocT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
//     if( !var->IsAssoc())// && var->Type() != STRUCT)
//       {
	sInit = varPtr->Data()->LoopIndex();
	if( sInit < 0)
	  s = sInit + var->Size();
	else
	  s = sInit;

	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].f");
	  }
	if( s < 0)
	  {
	    throw GDLException("Scalar subscript out of range [<].f");
	  }
	    
	return var->NewIx( s);
//       }
//     
//     // normal case
//     //    Init();
//     SetVariable( var);
//     return var->Index( this);
  }

bool ArrayIndexListOneScalarT::ToAssocIndex( SizeT& lastIx)
  {
    sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
    if( sInit < 0)
      throw GDLException( NULL,"Record number must be a scalar > 0 in this context.",true,false);      
    lastIx = sInit;
    return true;
  }
void ArrayIndexListOneScalarT::SetVariable( BaseGDL* var) 
  {
    sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
    if( var->IsAssoc()) return;
    if( sInit < 0)
      s = sInit + var->Size();
    else
      s = sInit;
    if( s < 0)
      throw GDLException("Scalar subscript out of range [<].h");
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].h");

    // for assoc variables last index is the record
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].g");
  }
void ArrayIndexListOneScalarNoAssocT::SetVariable( BaseGDL* var) 
  {
    sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
//     if( var->IsAssoc()) return;
    if( sInit < 0)
      s = sInit + var->Size();
    else
      s = sInit;
    if( s < 0)
      throw GDLException("Scalar subscript out of range [<].h");
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].h");

    // for assoc variables last index is the record
    if( s >= var->Size())
      throw GDLException("Scalar subscript out of range [>].g");
  }
void ArrayIndexListOneScalarT::AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1 && !var->IsAssoc())// && var->Type() != STRUCT) 
      {
	sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	if( sInit < 0)
	  s = sInit + var->Size();
	else
	  s = sInit;
	if( s < 0)
	  throw GDLException("Scalar subscript out of range [<].h");
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
void ArrayIndexListOneScalarNoAssocT::AssignAt( BaseGDL* var, BaseGDL* right)
  {
    // Init() was already called
    // scalar case
    if( right->N_Elements() == 1)// && !var->IsAssoc())// && var->Type() != STRUCT) 
      {
	sInit = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	if( sInit < 0)
	  s = sInit + var->Size();
	else
	  s = sInit;
	if( s < 0)
	  throw GDLException("Scalar subscript out of range [<].h");
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

AllIxBaseT* ArrayIndexListT::BuildIx() {}

// called from compiler after structure is fixed
// the ArrayIndexListT factory
void MakeArrayIndex( ArrayIndexVectorT* ixList, 
				  ArrayIndexListT** arrayIndexOut, 
				  ArrayIndexListT** arrayIndexNoAssocOut)
{
  assert( ixList->size() != 0); // must be, from compiler

  if( ixList->size() == 1)
    {
      if( CArrayIndexScalarID == (*ixList)[0]->Type())
      {
	if( arrayIndexNoAssocOut != NULL)
		*arrayIndexNoAssocOut = new ArrayIndexListOneConstScalarNoAssocT( ixList);
	*arrayIndexOut = new ArrayIndexListOneConstScalarT( ixList);
	return;
      }
      if( ArrayIndexScalarID == (*ixList)[0]->Type())
      {
	if( arrayIndexNoAssocOut != NULL)
		*arrayIndexNoAssocOut = new ArrayIndexListOneScalarNoAssocT( ixList);
		*arrayIndexOut = new ArrayIndexListOneScalarT( ixList);
		return;
      }
      
      if( ArrayIndexScalarVPID == (*ixList)[0]->Type())
      {
	if( arrayIndexNoAssocOut != NULL)
		*arrayIndexNoAssocOut = new ArrayIndexListOneScalarVPNoAssocT( ixList);
		*arrayIndexOut = new ArrayIndexListOneScalarVPT( ixList);
		return;
      }
      
	if( arrayIndexNoAssocOut != NULL)
    *arrayIndexNoAssocOut = new ArrayIndexListOneNoAssocT( ixList);
    *arrayIndexOut = new ArrayIndexListOneT( ixList);
    return;
    }
  
  SizeT nScalar  = 0;
  SizeT nIndexed = 0;
  for( SizeT i=0; i<ixList->size(); ++i)
    {
      if( ArrayIndexScalarID == (*ixList)[i]->Type() ||
	  ArrayIndexScalarVPID == (*ixList)[i]->Type() ||
	  CArrayIndexScalarID == (*ixList)[i]->Type() ) ++nScalar;
	else if( ArrayIndexIndexedID == (*ixList)[i]->Type() ||
		CArrayIndexIndexedID == (*ixList)[i]->Type()) ++nIndexed;
    }
  if( nScalar == ixList->size())
  {
	if( arrayIndexNoAssocOut != NULL)
    *arrayIndexNoAssocOut = new ArrayIndexListScalarNoAssocT( ixList);
    *arrayIndexOut = new ArrayIndexListScalarT( ixList);
    return;
  }	
  // Note that each index can be a assoc index anytime
  // filter this special case out should save complexity in
  // ArrayIndexListMultiT
  if( nIndexed == 0)
  {
	if( arrayIndexNoAssocOut != NULL)
    *arrayIndexNoAssocOut = new ArrayIndexListMultiNoneIndexedNoAssocT( ixList);
    *arrayIndexOut = new ArrayIndexListMultiNoneIndexedT( ixList);
    return;
  }	
  if( nIndexed == ixList->size())
  {
	if( arrayIndexNoAssocOut != NULL)
    *arrayIndexNoAssocOut = new ArrayIndexListMultiAllIndexedNoAssocT( ixList);
    *arrayIndexOut = new ArrayIndexListMultiAllIndexedT( ixList);
    return;
  }	

	if( arrayIndexNoAssocOut != NULL)
  *arrayIndexNoAssocOut = new ArrayIndexListMultiNoAssocT( ixList);
  *arrayIndexOut = new ArrayIndexListMultiT( ixList);
}
