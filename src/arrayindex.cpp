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
      throw GDLException("Scalar subscript out of range [>].");
    }
  
  return var->NewIx( s);
}
BaseGDL* ArrayIndexScalarVP::Index( BaseGDL* var, IxExprListT& ixL)
{
  s = varPtr->Data()->LoopIndex();

  if( s >= var->Size())
    {
      throw GDLException("Scalar subscript out of range [>].");
    }
  
  return var->NewIx( s);
}

SizeT ArrayIndexScalar::NIter( SizeT varDim) 
{
  s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();

  if( s >= varDim)
    throw GDLException("Scalar subscript out of range [>].");
  return 1;
}
SizeT ArrayIndexScalarVP::NIter( SizeT varDim) 
{
  s = varPtr->Data()->LoopIndex();

  if( s >= varDim)
    throw GDLException("Scalar subscript out of range [>].");
  return 1;
}

// void ArrayIndexListOneScalarT::Init( IxExprListT& ix_)
// {
//   s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
// }
// void ArrayIndexListOneScalarVPT::Init( IxExprListT& ix_)
// {
//   s = varPtr->Data()->LoopIndex();
// }
void ArrayIndexListOneScalarT::Init()
{
  s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
}
void ArrayIndexListOneScalarVPT::Init()
{
  s = varPtr->Data()->LoopIndex();
}

// optimized for one dimensional access
BaseGDL* ArrayIndexListOneScalarT::Index( BaseGDL* var, IxExprListT& ix_)
  {
    // Init() not called
    if( !var->IsAssoc() && var->Type() != STRUCT)
      {
	s = GDLInterpreter::CallStackBack()->GetKW( varIx)->LoopIndex();
	  
	if( s >= var->Size())
	  {
	    throw GDLException("Scalar subscript out of range [>].");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    Init();
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
	    throw GDLException("Scalar subscript out of range [>].");
	  }
	    
	return var->NewIx( s);
      }
    
    // normal case
    Init();
    SetVariable( var);
    return var->Index( this);
  }

// vtable
ArrayIndexListT::~ArrayIndexListT() {}

AllIxT* ArrayIndexListT::BuildIx() {}
