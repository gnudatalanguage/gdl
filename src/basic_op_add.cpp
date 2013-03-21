/***************************************************************************
                          basic_op_add.cpp  -  GDL add (+) operators
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

// to be included from datatypes.cpp
#ifdef INCLUDE_BASIC_OP_CPP

// // header in datatypes.hpp
// 
// //#include "datatypes.hpp"
// //#include "dstructgdl.hpp"
// //#include "arrayindex.hpp"
// 
// //#include <csignal>
// #include "sigfpehandler.hpp"
// 
// #ifdef _OPENMP
// #include <omp.h>
// #endif
// 
// #include "typetraits.hpp"
// 
// using namespace std;


// ************************
// Add
// ************************

// also see Add...New operators (in basic_op_new.cpp)

// Adds right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
BaseGDL* Data_<Sp>::Add( BaseGDL* r)
{
  
  
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      (*this)[0] += (*right)[0];
      return this;
    }
#ifdef USE_EIGEN

        Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
        Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mRight(&(*right)[0], nEl);
	mThis += mRight;
	return this;
#else

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] += (*right)[i];
    }  //C delete right;
  return this;
#endif
  
}
template<class Sp>
BaseGDL* Data_<Sp>::AddInv( BaseGDL* r)
{
  assert( this->Type() != GDL_OBJ); // should never be called via this
  return Add( r); // this needs to be modified
}
template<>
BaseGDL* Data_<SpDString>::AddInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      (*this)[0] = (*right)[0] + (*this)[0];
      return this;
    }
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] = (*right)[i] + (*this)[i];
    }  //C delete right;
  return this;
}
// invalid types
template<>
BaseGDL* Data_<SpDPtr>::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDObj>::Add( BaseGDL* r)
{
  // overload here
  Data_* self;
  DFun* plusOverload;
  
  ProgNodeP callingNode = interpreter->GetRetTree();

  if( !Scalar())
  {
    if( r->Type() == GDL_OBJ && r->Scalar())
    {
      self = static_cast<Data_*>( r);
      plusOverload = static_cast<DFun*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOPlus));
      if( plusOverload == NULL)
      {
	throw GDLException( callingNode, "Cannot apply not overloaded operator to datatype OBJECT.", true, false);
      }
    }
    else
      {
	throw GDLException( callingNode, "Cannot apply operation to non-scalar datatype OBJECT.", true, false);
      }
  }
  else
  {
    // Scalar()
    self = static_cast<Data_*>( this);
    plusOverload = static_cast<DFun*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOPlus));
    if( plusOverload == NULL)
    {
      if( r->Type() == GDL_OBJ && r->Scalar())
      {
	self = static_cast<Data_*>( r);
	plusOverload = static_cast<DFun*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOPlus));
	if( plusOverload == NULL)
	{
	  throw GDLException(callingNode,"Cannot apply not overloaded operator to datatype OBJECT.",true, false);  
	} 
      }
      else
      {
	throw GDLException( callingNode, "Cannot apply not overloaded operator to datatype OBJECT.", true, false);
      }
    }
  }

  assert( self->Scalar());
  assert( plusOverload != NULL);

  // hidden SELF is counted as well
  int nParSub = plusOverload->NPar();
  assert( nParSub >= 1); // SELF
  if( nParSub < 3) // (SELF), LEFT, RIGHT
  {
    throw GDLException( callingNode, plusOverload->ObjectName() +
		    ": Incorrect number of arguments.",
		    false, false);
  }
  EnvUDT* newEnv;
  Guard<BaseGDL> selfGuard;
  BaseGDL* thisP;
  // Dup() here is not optimal
  // avoid at least for internal overload routines (which do/must not change SELF or r)
  bool internalDSubUD = plusOverload->GetTree()->IsWrappedNode();  
  if( internalDSubUD)  
  {
    thisP = this;
    newEnv= new EnvUDT( callingNode, plusOverload, &self);
    newEnv->SetNextParUnchecked( &thisP); // LEFT  parameter, as reference to prevent cleanup in newEnv
    newEnv->SetNextParUnchecked( &r); // RVALUE  parameter, as reference to prevent cleanup in newEnv
  }
  else
  {
    self = self->Dup();
    selfGuard.Init( self);
    newEnv= new EnvUDT( callingNode, plusOverload, &self);
    newEnv->SetNextParUnchecked( this->Dup()); // LEFT  parameter, as value
    newEnv->SetNextParUnchecked( r->Dup()); // RIGHT parameter, as value
  }

  
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(interpreter->CallStack());

  interpreter->CallStack().push_back( newEnv); 
  
  // make the call
  BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  if( !internalDSubUD && self != selfGuard.Get())
  {
    // always put out warning first, in case of a later crash
    Warning( "WARNING: " + plusOverload->ObjectName() + 
	  ": Assignment to SELF detected (GDL session still ok).");
    // assignment to SELF -> self was deleted and points to new variable
    // which it owns
    selfGuard.Release();
    if( static_cast<BaseGDL*>(self) != NullGDL::GetSingleInstance())
      selfGuard.Reset(self);
  }
  return res;
}
// difference from above: Order of parameters in call
template<>
BaseGDL* Data_<SpDObj>::AddInv( BaseGDL* r)
{
  if( r->Type() == GDL_OBJ && r->Scalar())
  {
    return r->Add( this); // for right order of parameters
  }
    
  // overload here
  Data_* self;
  DFun* plusOverload;
  
  ProgNodeP callingNode = interpreter->GetRetTree();

  if( !Scalar())
  {
    throw GDLException( callingNode, "Cannot apply operation to non-scalar datatype OBJECT.", true, false);
  }
  else
  {
    // Scalar()
    self = static_cast<Data_*>( this);
    plusOverload = static_cast<DFun*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOPlus));
    if( plusOverload == NULL)
    {
	throw GDLException( callingNode, "Cannot apply not overloaded operator to datatype OBJECT.", true, false);
    }
  }

  assert( self->Scalar());
  assert( plusOverload != NULL);

  // hidden SELF is counted as well
  int nParSub = plusOverload->NPar();
  assert( nParSub >= 1); // SELF
  if( nParSub < 3) // (SELF), LEFT, RIGHT
  {
    throw GDLException( callingNode, plusOverload->ObjectName() +
		    ": Incorrect number of arguments.",
		    false, false);
  }
  EnvUDT* newEnv;
  Guard<BaseGDL> selfGuard;
  BaseGDL* thisP;
  // Dup() here is not optimal
  // avoid at least for internal overload routines (which do/must not change SELF or r)
  bool internalDSubUD = plusOverload->GetTree()->IsWrappedNode();  
  if( internalDSubUD)  
  {
    thisP = this;
    newEnv= new EnvUDT( callingNode, plusOverload, &self);
    // order different to Add
    newEnv->SetNextParUnchecked( &r); // RVALUE  parameter, as reference to prevent cleanup in newEnv
    newEnv->SetNextParUnchecked( &thisP); // LEFT  parameter, as reference to prevent cleanup in newEnv
  }
  else
  {
    self = self->Dup();
    selfGuard.Init( self);
    newEnv= new EnvUDT( callingNode, plusOverload, &self);
    // order different to Add
    newEnv->SetNextParUnchecked( r->Dup()); // RIGHT parameter, as value
    newEnv->SetNextParUnchecked( this->Dup()); // LEFT  parameter, as value
  }

  
  // better than auto_ptr: auto_ptr wouldn't remove newEnv from the stack
  StackGuard<EnvStackT> guard(interpreter->CallStack());

  interpreter->CallStack().push_back( newEnv); 
  
  // make the call
  BaseGDL* res=interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

  if( !internalDSubUD && self != selfGuard.Get())
  {
    // always put out warning first, in case of a later crash
    Warning( "WARNING: " + plusOverload->ObjectName() + 
	  ": Assignment to SELF detected (GDL session still ok).");
    // assignment to SELF -> self was deleted and points to new variable
    // which it owns
    selfGuard.Release();
    if( static_cast<BaseGDL*>(self) != NullGDL::GetSingleInstance())
      selfGuard.Reset(self);
  }
  return res;
}

template<class Sp>
BaseGDL* Data_<Sp>::AddS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] += (*right)[0];
      return this;
    }
  Ty s = (*right)[0];
  // right->Scalar(s);
  //  dd += s;
#ifdef USE_EIGEN

        Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
	mThis += s;
	return this;
#else
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] += s;
    }  //C delete right;
  return this;
#endif
  
}
template<>
BaseGDL* Data_<SpDString>::AddS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] += (*right)[0];
      return this;
    }
  Ty s = (*right)[0];
  // right->Scalar(s);
  //  dd += s;
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] += s;
    }  //C delete right;
  return this;
}

template<class Sp>
BaseGDL* Data_<Sp>::AddInvS( BaseGDL* r)
{
  return AddS( r);
}
template<>
BaseGDL* Data_<SpDString>::AddInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] = (*right)[0] + (*this)[0] ;
      return this;
    }
  Ty s = (*right)[0];
  // right->Scalar(s);
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] = s + (*this)[i];
    }  //C delete right;
  return this;
  
}

// invalid types
template<>
BaseGDL* Data_<SpDPtr>::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDObj>::AddS( BaseGDL* r)
{
  return Add( r);
}
template<>
BaseGDL* Data_<SpDObj>::AddInvS( BaseGDL* r)
{
  return AddInv( r);
}


//#include "instantiate_templates.hpp"

#endif
