/***************************************************************************
                          basic_op_sub.cpp  -  GDL sub (-) operators
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include "datatypes.hpp" // for friend declaration
#include "nullgdl.hpp"
#include "dinterpreter.hpp"

// needed with gcc-3.3.2
#include <cassert>

// Sub
// substraction: left=left-right
template<class Sp>
BaseGDL* Data_<Sp>::Sub( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] -= (*right)[0];
      return this;
    }
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mRight(&(*right)[0], nEl);
  mThis -= mRight;
  return this;
#else

  if( nEl == rEl)
    dd -= right->dd;
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i < nEl; ++i)
	    (*this)[i] -= (*right)[i];
	}}  //C delete right;
  return this;
#endif
  
}
// inverse substraction: left=right-left
template<class Sp>
BaseGDL* Data_<Sp>::SubInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  /*  if( nEl == rEl)
      dd = right->dd - dd;
      else*/
  if( nEl == 1)
    {
      (*this)[0] = (*right)[0] - (*this)[0];
      return this;
    }
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mRight(&(*right)[0], nEl);
  mThis = mRight - mThis;
  return this;
#else
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] = (*right)[i] - (*this)[i];
    }  //C delete right;
  return this;
#endif
  
}
// invalid types
template<>
BaseGDL* Data_<SpDString>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDString>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDPtr>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDPtr>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDObj>::Sub( BaseGDL* r)
{
  // overload here
  Data_* self;
  DSubUD* plusOverload;
  
  ProgNodeP callingNode = interpreter->GetRetTree();

  if( !Scalar())
  {
    if( r->Type() == GDL_OBJ && r->Scalar())
    {
      self = static_cast<Data_*>( r);
      plusOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOMinus));
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
    plusOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOMinus));
    if( plusOverload == NULL)
    {
      if( r->Type() == GDL_OBJ && r->Scalar())
      {
	self = static_cast<Data_*>( r);
	plusOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOMinus));
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
template<>
BaseGDL* Data_<SpDObj>::SubInv( BaseGDL* r)
{
  if( r->Type() == GDL_OBJ && r->Scalar())
  {
    return r->Sub( this); // for right order of parameters
  }
    
  // overload here
  Data_* self;
  DSubUD* plusOverload;
  
  ProgNodeP callingNode = interpreter->GetRetTree();

  if( !Scalar())
  {
    throw GDLException( callingNode, "Cannot apply operation to non-scalar datatype OBJECT.", true, false);
  }
  else
  {
    // Scalar()
    self = static_cast<Data_*>( this);
    plusOverload = static_cast<DSubUD*>(GDLInterpreter::GetObjHeapOperator( (*self)[0], OOMinus));
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
Data_<Sp>* Data_<Sp>::SubS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  if( nEl == 1)
    {
      (*this)[0] -= (*right)[0];
      return this;
    }
  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //  dd -= s;
#ifdef USE_EIGEN

        Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
	mThis -= s;
	return this;
#else
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] -= s;
    }  //C delete right;
  return this;
#endif
  
}
// inverse substraction: left=right-left
template<class Sp>
Data_<Sp>* Data_<Sp>::SubInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);

  if( nEl == 1)
    {
      (*this)[0] = (*right)[0] - (*this)[0];
      return this;
    }
  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //  dd = s - dd;
#ifdef USE_EIGEN

        Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
	mThis = s - mThis;
	return this;
#else
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] = s - (*this)[i];
    }  //C delete right;
  return this;
#endif
  
}
// invalid types
template<>
Data_<SpDString>* Data_<SpDString>::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

#include "instantiate_templates.hpp"
