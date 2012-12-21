/***************************************************************************
                          basic_op_new.cpp  -  GDL operators
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
// after basic_op.cpp
#ifdef INCLUDE_BASIC_OP_CPP


using namespace std;

// binary operators

// in basic_op.cpp:
// 1. operators that always return a new result (basic_op.cpp)
// 2. operators which operate on 'this' (basic_op.cpp)

// here:
// 3. same operators as under 2. that always return a new result

// AndOp &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// Ands right and itself into a new DataT_
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  assert(right->N_Elements());
  
  Data_* res = NewResult();  
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] & (*right)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] & (*right)[i]; // & Ty(1);
    }
  return res;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInvNew( BaseGDL* right)
{
  return AndOpNew( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); 
  Data_* res = NewResult();

  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      if ( (*right)[0] == zero ) (*res)[0] = zero; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for ( int i=0; i < nEl; ++i )
	if ( (*right)[i] == zero ) (*res)[i] = zero; else (*res)[i] = (*this)[i];
    }
  return res;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  assert( right->N_Elements());
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      if( (*this)[0] != zero) (*res)[0] = (*right)[0]; else (*res)[0] = zero;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] != zero) (*res)[i] = (*right)[i]; else (*res)[i] = zero; 
    }
  return res;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  assert( right->N_Elements());
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      if ( (*right)[0] == zero ) (*res)[0] = zero; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if ( (*right)[i] == zero ) (*res)[i] = zero; else (*res)[i] = (*this)[i];
    }
  return res;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  assert( right->N_Elements());
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      if( (*this)[0] != zero) (*res)[0] = (*right)[0]; else (*res)[0] = zero;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] != zero) (*res)[i] = (*right)[i]; else (*res)[i] = zero;
    }
  return res;
}
// invalid types
DStructGDL* DStructGDL::AndOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::AndOpInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::AndOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInvNew( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
//  return res;
// }
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInvNew( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
//  return res;
// }
template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInvNew( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
//  return res;
// }

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);

  Ty s = (*right)[0];
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] & s;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared(s)
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] & s;
    }
  return res;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInvSNew( BaseGDL* right)
{
  return AndOpSNew( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);
  if( (*right)[0] == zero)
    {
	return New( this->dim, BaseGDL::ZERO);
    }
  return this->Dup();
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);
  Ty s = (*right)[0];
  if( s == zero)
    {
      return New( this->dim, BaseGDL::ZERO);
    }
  else
    {
      Data_* res = NewResult();
      if( nEl == 1)
	{			
	  if( (*this)[0] != zero) (*res)[0] = s; else (*res)[0] = zero;	
	  return res;	
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*res)[i] = s; else (*res)[i] = zero;
	}
	return res;
    }
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);
  if( (*right)[0] == zero)
    {
	return New( this->dim, BaseGDL::ZERO);
    }
  return this->Dup();
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  if( s == zero)
    {
      return New( this->dim, BaseGDL::ZERO);
    }
  else
    {
      Data_* res = NewResult();
      if( nEl == 1)
	{
	  if( (*this)[0] != zero) (*res)[0] = s; else (*res)[0] = zero;
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*res)[i] = s; else (*res)[i] = zero;
	}
	return res;
    }
}
// invalid types
DStructGDL* DStructGDL::AndOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::AndOpInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
// template<>
// DStructGDL* DStructGDL::AndOpInvNew( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
//  return res;
// }
template<>
Data_<SpDString>* Data_<SpDString>::AndOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInvSNew( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
//  return res;
// }
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}



// OrOp ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// Ors right to itself returns new result
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  Data_* res = NewResult();
  // assert( rEl);
  assert( nEl);
  //if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] | (*right)[0]; // | Ty(1);
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] | (*right)[i]; // | Ty(1);
    }
  //C delete right;
  return res;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvNew( BaseGDL* right)
{
  return OrOpNew( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  Data_* res = NewResult();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  if( nEl == 1)
    {
      if( (*this)[0] == zero) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] == zero) (*res)[i] = (*right)[i]; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); Data_* res = NewResult();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*right)[0] != zero) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*right)[i] != zero) (*res)[i] = (*right)[i]; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  Data_* res = NewResult();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  if( nEl == 1)
    {
      if( (*this)[0] == zero) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] == zero) (*res)[i] = (*right)[i]; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); Data_* res = NewResult();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*right)[0] != zero) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*right)[i] != zero) (*res)[i] = (*right)[i]; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
// invalid types
DStructGDL* DStructGDL::OrOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::OrOpInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::OrOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// OrOpS
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //s &= Ty(1);
  //  dd |= s;
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] | s;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] | s;
    }  //C delete right;
  return res;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvSNew( BaseGDL* right)
{
  return OrOpSNew( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  
  Ty s = (*right)[0];
  if( s != zero)
    {
      Data_* res = NewResult();
      if( nEl == 1)
	{
	  if( (*this)[0] == zero) (*res)[0] = s; else (*res)[0] = (*this)[0];
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    if( (*this)[i] == zero) (*res)[i] = s; else (*res)[i] = (*this)[i];
	}
	return res;
      }
  else // s == zero
	{
	return this->Dup();
	}
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  Ty s = (*right)[0];
  if( s != zero)
    {
	for( SizeT i=0; i < nEl; ++i)
		(*res)[i] = s;
	return res;
    }
  else
    {
      if( nEl == 1)
	{
	  if( (*this)[0] != zero) (*res)[0] = s; else (*res)[0] = zero;
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*res)[i] = s; else (*res)[i] = zero;
	}
	return res;
    } 
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); Data_* res = NewResult();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s != zero)
    {
      if( nEl == 1)
	{
	  if( (*this)[0] == zero) (*res)[0] = s; else (*res)[0] = (*this)[0];
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    if( (*this)[i] == zero) (*res)[i] = s; else (*res)[i] = (*this)[i];
	}
	return res;
    }
    // s == zero
    return this->Dup();
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  Ty s = (*right)[0];
  if( s != zero)
    {
	for( SizeT i=0; i < nEl; ++i)
		(*res)[i] = s;
	return res;
    }
  else
    {
      if( nEl == 1)
	{
	  if( (*this)[0] != zero) (*res)[0] = s; else (*res)[0] = zero;
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*res)[i] = s; else (*res)[i] = zero;
	}
	return res;
    } 
}
// invalid types
DStructGDL* DStructGDL::OrOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::OrOpInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::OrOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}



// XorOp ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// Xors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::XorOpNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  
  if( nEl == 1)
    {
     Data_* res = NewResult();
      (*res)[0] = (*this)[0] ^ (*right)[0];
      return res;
    }
    
  Ty s;
  if( right->StrictScalar(s))
    {
	if( s == Sp::zero)
		return this->Dup();
	  
	Data_* res = NewResult();
	TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
		for( int i=0; i < nEl; ++i)
			(*res)[i] = (*this)[i] ^ s;
	}
	return res;
    }
  else
	{
 	Data_* res = NewResult();
	TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
 		(*res)[i] = (*this)[i] ^ (*right)[i];
	}
	return res;
	}
}
// invalid types
template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype FLOAT.",true,false);  
  return NULL;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype DOUBLE.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::XorOpNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::XorOpSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  if( nEl == 1)
    {
      Data_* res = NewResult();
      (*res)[0] = (*this)[0] ^  (*right)[0];
      return res;
    }
  Ty s = (*right)[0];
   if( s == this->zero)
     return this->Dup();
     
  Data_* res = NewResult();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] ^ s;
    }
  return res;
}
// different for floats
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype FLOAT.",true,false);  
  return NULL;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype DOUBLE.",true,false);  
  return NULL;
}
// invalid types
DStructGDL* DStructGDL::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::XorOpSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// Add ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Adds right to itself returns new result
// right must always have more or same number of elements
template<class Sp>
BaseGDL* Data_<Sp>::AddNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( nEl);
  assert( right->N_Elements());

  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] + (*right)[0];
      return res;
    }

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] + (*right)[i];
    }  //C delete right;
  return res;
}

template<class Sp>
BaseGDL* Data_<Sp>::AddInvNew( BaseGDL* r)
{
  return AddNew( r);
}

template<>
BaseGDL* Data_<SpDString>::AddInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*right)[0] + (*this)[0] ;
      return res;
    }

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*right)[i] + (*this)[i];
    }  //C delete right;
  return res;
}

// invalid types
DStructGDL* DStructGDL::AddNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::AddInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDPtr>::AddNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDObj>::AddNew( BaseGDL* r)
{
    return Add( r);
}
template<>
BaseGDL* Data_<SpDObj>::AddInvNew( BaseGDL* r)
{
    return AddInv( r);
}

// scalar versions
template<class Sp>
BaseGDL* Data_<Sp>::AddSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] + (*right)[0];
      return res;
    }
  Ty s = (*right)[0];
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] + s;
    }  //C delete right;
  return res;
}
template<class Sp>
BaseGDL* Data_<Sp>::AddInvSNew( BaseGDL* r)
{
  return AddSNew( r);
}
template<>
BaseGDL* Data_<SpDString>::AddInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); Data_* res = NewResult();
  assert( nEl);
  if( nEl == 1)
    {
      (*res)[0] = (*right)[0] + (*this)[0];
      return res;
    }
  Ty s = (*right)[0];
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = s + (*this)[i];
    }  //C delete right;
  return res;
}

// invalid types
DStructGDL* DStructGDL::AddSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::AddInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDPtr>::AddSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDObj>::AddSNew( BaseGDL* r)
{
  return Add( r);
}
template<>
BaseGDL* Data_<SpDObj>::AddInvSNew( BaseGDL* r)
{ 
  return AddInv( r);
}



// Sub ----------------------------------------------------------------------
// substraction: res=left-right
template<class Sp>
BaseGDL* Data_<Sp>::SubNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  
  Data_* res = NewResult();

  if( nEl == 1)// && rEl == 1)
    {
      (*res)[0] = (*this)[0] - (*right)[0];
      return res;
    }

  Ty s;
  if( right->StrictScalar(s)) 
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = (*this)[i] - s;
	}
    }
  else 
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = (*this)[i] - (*right)[i];
	}
    }
  return res;
}
// inverse substraction: left=right-left
template<class Sp>
BaseGDL* Data_<Sp>::SubInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*right)[0] - (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*right)[i] - (*this)[i];
    }  
  return res;
}
// invalid types
DStructGDL* DStructGDL::SubNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::SubInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDString>::SubNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDString>::SubInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDPtr>::SubNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDPtr>::SubInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDObj>::SubNew( BaseGDL* r)
{
  return Sub( r);
}
template<>
BaseGDL* Data_<SpDObj>::SubInvNew( BaseGDL* r)
{
  return SubInv( r);
}

// scalar versions
template<class Sp>
BaseGDL* Data_<Sp>::SubSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] - (*right)[0];
      return res;
    }
  
  Ty s = (*right)[0];
  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] - s;
    }
  return res;
}
// inverse substraction: left=right-left
template<class Sp>
BaseGDL* Data_<Sp>::SubInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);

  Data_* res = NewResult();
  if( nEl == 1)
    {
      (*res)[0] = (*right)[0] - (*this)[0];
      return res;
    }
  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //  dd = s - dd;
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = s - (*this)[i];
    }
  return res;
}
// invalid types
DStructGDL* DStructGDL::SubSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::SubInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDString>::SubSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDString>::SubInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDPtr>::SubSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDPtr>::SubInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
BaseGDL* Data_<SpDObj>::SubSNew( BaseGDL* r)
{
  return Sub( r);
}
template<>
BaseGDL* Data_<SpDObj>::SubInvSNew( BaseGDL* r)
{
  return SubInv( r);
}

// LtMark <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// LtMarks right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); Data_* res = NewResult();
  //  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*this)[0] > (*right)[0]) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] > (*right)[i]) (*res)[i] = (*right)[i]; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
// invalid types
DStructGDL* DStructGDL::LtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::LtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);
  
  ULong nEl=N_Elements();
  assert( nEl);
  Data_* res = NewResult();
  if( nEl == 1)
    {
      if( (*this)[0] > (*right)[0]) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  Ty s = (*right)[0];
  // right->Scalar(s);
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] > s) (*res)[i] = s; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
// invalid types
DStructGDL* DStructGDL::LtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::LtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// GtMark >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// GtMarks right to itself returns new result
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); Data_* res = NewResult();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*this)[0] < (*right)[0]) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] < (*right)[i]) (*res)[i] = (*right)[i]; else (*res)[i] = (*this)[i];
    }  //C delete right;
  return res;
}
// invalid types
DStructGDL* DStructGDL::GtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::GtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); Data_* res = NewResult();
  assert( nEl);
  if( nEl == 1)
    {
      if( (*this)[0] < (*right)[0]) (*res)[0] = (*right)[0]; else (*res)[0] = (*this)[0];
      return res;
    }

  Ty s = (*right)[0];
  // right->Scalar(s);
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	if( (*this)[i] < s) (*res)[i] = s; else (*res)[i] = (*this)[i];
    }  ;
  return res;
}
// invalid types
DStructGDL* DStructGDL::GtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::GtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// Mult *********************************************************************
// Mults right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::MultNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  Data_* res=NewResult();
  
  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*res)[0] = (*this)[0] * (*right)[0];
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] * (*right)[i];
    }  //C delete right;
  return res;
}
// invalid types
DStructGDL* DStructGDL::MultNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::MultNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MultNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::MultNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::MultSNew( BaseGDL* r )
{
  Data_* right=static_cast<Data_*> ( r );

  ULong nEl=N_Elements();
  assert ( nEl );
  
  Data_* res = NewResult();
  if ( nEl == 1 )
    {
      ( *res )[0] = ( *this )[0] * ( *right )[0];
      return res;
    }
  Ty s = ( *right ) [0];
  TRACEOMP ( __FILE__, __LINE__ )
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for ( int i=0; i < nEl; ++i )
	(*res ) [i] = (*this )[i] * s;
    }
  return res;
}
// invalid types
DStructGDL* DStructGDL::MultSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::MultSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MultSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::MultSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}



// Div //////////////////////////////////////////////////////////////////////
// division: left=left/right
template<class Sp>
Data_<Sp>* Data_<Sp>::DivNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( nEl);
  
  Data_* res = NewResult();

  SizeT i = 0;

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // TODO: Check if we can use OpenMP here (is longjmp allowed?)
      //             if yes: need to run the full loop after the longjmp
      for( ; i < nEl; ++i)
	(*res)[i] = (*this)[i] / (*right)[i];
      return res;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int ix=i; ix < nEl; ++ix)
	    if( (*right)[ix] != this->zero)
	    	(*res)[ix] = (*this)[ix] / (*right)[ix];
	    else	
	    	(*res)[ix] = (*this)[ix];
	}
      return res;
    }
}
// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); Data_* res = NewResult();
  //  assert( rEl);
  assert( nEl);

  SizeT i = 0;

  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*res)[i] = (*right)[i] / (*this)[i];
      return res;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int ix=i; ix < nEl; ++ix)
	    if( (*this)[ix] != this->zero)
	      (*res)[ix] = (*right)[ix] / (*this)[ix];
	    else
	      (*res)[ix] = (*right)[ix];
	}      //C delete right;
      return res;
    }
}
// invalid types
DStructGDL* DStructGDL::DivNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::DivInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::DivSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  SizeT i=0;
  Data_* res = NewResult();
  if( s != this->zero)
    {
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] / s;
      return res;
    }
 
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
	{
	 for( SizeT i=0; i < nEl; ++i)
		(*res)[i] = (*this)[i] / s;
	}
  else
	{
	for( SizeT i=0; i < nEl; ++i)
		(*res)[i] = (*this)[i];
// 	}
	}
  return res;
}

// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); Data_* res = NewResult();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  if( nEl == 1 && (*this)[0] != this->zero)
  {
    (*res)[0] = (*right)[0] / (*this)[0];
    return res;    
  }
  
  Ty s = (*right)[0];
  SizeT i=0;
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( ; i < nEl; ++i)
	(*res)[i] = s / (*this)[i];
      return res;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int ix=i; ix < nEl; ++ix)
	    if( (*this)[ix] != this->zero)
	      (*res)[ix] = s / (*this)[ix];
	    else 
	      (*res)[ix] = s;
	}
      return res;
    }
}
// invalid types
DStructGDL* DStructGDL::DivSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::DivInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}



// Mod %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// modulo division: left=left % right
template<class Sp>
Data_<Sp>* Data_<Sp>::ModNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  Data_* res = NewResult();
  //  assert( rEl);
  assert( nEl);

  SizeT i=0;
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( ; i < nEl; ++i)
	(*res)[i] = (*this)[i] % (*right)[i];
      return res;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int ix=i; ix < nEl; ++ix)
	    if( (*right)[ix] != this->zero)
	      (*res)[ix] = (*this)[ix] % (*right)[ix];
	    else
	      (*res)[ix] = this->zero;
	}
      return res;
    }
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  
  SizeT i=0;

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( ; i < nEl; ++i)
	(*res)[i] = (*right)[i] % (*this)[i];
      return res;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int ix=i; ix < nEl; ++ix)
	    if( (*this)[ix] != this->zero)
	      (*res)[ix] = (*right)[ix] % (*this)[ix];
	    else
	      (*res)[ix] = this->zero;
	}
      return res;
    }    
}
// in basic_op.cpp
// float modulo division: left=left % right
// inline DFloat Modulo( const DFloat& l, const DFloat& r)
// {
//   float t=abs(l/r);
//   if( l < 0.0) return t=(floor(t)-t)*abs(r);
//   return (t-floor(t))*abs(r);
// }
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  
  assert( nEl);
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*this)[0],(*right)[0]);
	return res;
  }
  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
   {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo((*this)[i],(*right)[i]);
  }  
  return res;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  Data_* res = NewResult();
  
  assert( nEl);
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*right)[0],(*this)[0]);
	return res;
  }
  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo((*right)[i],(*this)[i]);
    }  
  return res;
}
// double modulo division: left=left % right
inline DDouble Modulo( const DDouble& l, const DDouble& r)
{
  DDouble t=abs(l/r);
  if( l < 0.0) return t=(floor(t)-t)*abs(r);
  return (t-floor(t))*abs(r);
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*this)[0],(*right)[0]);
	return res;
  }
  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo((*this)[i],(*right)[i]);
    }
  return res;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  Data_* res = NewResult();
  
  assert( nEl);
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*right)[0],(*this)[0]);
	return res;
  }
  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo((*right)[i],(*this)[i]);
    }  
  return res;
}
// invalid types
DStructGDL* DStructGDL::ModNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::ModInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::ModSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);

  Ty s = (*right)[0];
   
  Data_* res = NewResult();
  if( s != this->zero)
    {
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] % s;
      return res;
    }

    
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = (*this)[i] % s;
      return res;
    }
  else
    {
      assert( s == this->zero);
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = this->zero;
      return res;
    }
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1 && (*this)[0] != this->zero)
  {
    (*res)[0] = (*right)[0] % (*this)[0];
    return res;
  }
  
  Ty s = (*right)[0];
  SizeT i=0;

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( /*SizeT i=0*/; i < nEl; ++i)
	{
	  (*res)[i] = s % (*this)[i];
	}
      return res;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int ix=i; ix < nEl; ++ix)
	    if( (*this)[ix] != this->zero)
	      (*res)[ix] = s % (*this)[ix];
	    else 
	      (*res)[ix] = this->zero;
	}
      return res;
    }    
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*this)[0],(*right)[0]);
	return res;
  }
    
  Ty s = (*right)[0];
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo((*this)[i],s);
    }  
  return res;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);

  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*right)[0],(*this)[0]);
	return res;
  }
  
  Ty s = (*right)[0];
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo(s,(*this)[i]);
    }
  return res;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);
  
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*this)[0],(*right)[0]);
	return res;
  }
    
  Ty s = (*right)[0];
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo((*this)[i],s);
    }  
  return res;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);

  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = Modulo((*right)[0],(*this)[0]);
	return res;
  }
  
  Ty s = (*right)[0];
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = Modulo(s,(*this)[i]);
    }
  return res;
}
// invalid types
DStructGDL* DStructGDL::ModSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::ModInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModSNew( BaseGDL* r)

{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}



// Pow pow pow pow pow pow pow pow pow pow pow pow pow pow pow pow pow pow
// C++ defines pow only for floats and doubles
// in basic_op.cpp:
// template <typename T> T pow( const T r, const T l)
// {
//   typedef T TT;
// 
//   if( l == 0) return 1;
//   if( l < 0)  return 0;
// 
//   const int nBits = sizeof(TT) * 8;
// 
//   T arr = r;
//   T res = 1;
//   TT mask = 1;
//   for( SizeT i=0; i<nBits; ++i)
//     {
//       if( l & mask) res *= arr;
//       mask <<= 1;
//       if( l < mask) return res;
//       arr *= arr;
//     }
// 
//   return res;
// }

// power of value: left=left ^ right
// integral types
template<class Sp>
Data_<Sp>* Data_<Sp>::PowNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  if( nEl == 1)
  {
	(*res)[0] = pow( (*this)[0], (*right)[0]);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*this)[i], (*right)[i]); // valarray
    }  //C delete right;
  return res;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = pow( (*right)[0], (*this)[0]);
	return res;
  }
      
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*right)[i], (*this)[i]);
    }
  return res;
}

// PowInt and PowIntNew can only be called for GDL_FLOAT and GDL_DOUBLE
template<class Sp>
Data_<Sp>* Data_<Sp>::PowIntNew( BaseGDL* r)
{
  assert( 0);
  throw GDLException("Internal error: Data_::PowIntNew called.",true,false);
  return NULL;
}
DStructGDL* DStructGDL::PowIntNew( BaseGDL* r)
{
  assert( 0);
  throw GDLException("Internal error: DStructGDL::PowIntNew called.",true,false);  
  return NULL;
}
// floats power of value with GDL_LONG: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowIntNew( BaseGDL* r)
{
  DLongGDL* right=static_cast<DLongGDL*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  if( r->StrictScalar())
    {
      Data_* res = new Data_( Dim(), BaseGDL::NOZERO);
      DLong r0 = (*right)[0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = pow( (*this)[i], r0);
	}      return res;
    }
  if( StrictScalar())
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      Ty s0 = (*this)[ 0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( int i=0; i < rEl; ++i)
	    (*res)[ i] = pow( s0, (*right)[ i]);
	}      return res;
    }
  if( nEl <= rEl)
    {
      Data_* res = new Data_( Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}      return res;
    }
  else
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( int i=0; i < rEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}      return res;
    }
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowIntNew( BaseGDL* r)
{
  DLongGDL* right=static_cast<DLongGDL*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); Data_* res = NewResult();
  assert( rEl);
  assert( nEl);
  if( r->StrictScalar())
    {
      Data_* res = new Data_( Dim(), BaseGDL::NOZERO);
      DLong r0 = (*right)[0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = pow( (*this)[i], r0);
	}      return res;
    }
  if( StrictScalar())
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      Ty s0 = (*this)[ 0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( int i=0; i < rEl; ++i)
	    (*res)[ i] = pow( s0, (*right)[ i]);
	}      return res;
    }
  if( nEl <= rEl)
    {
      Data_* res = new Data_( Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}      return res;
    }
  else
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( int i=0; i < rEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}      return res;
    }
}

// floats power of value: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = pow( (*this)[0], (*right)[0]);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	for( int i=0; i < nEl; ++i)
	  (*res)[i] = pow( (*this)[i], (*right)[i]);
	}
  return res;
}
// floats inverse power of value: left=right ^ left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = pow( (*right)[0], (*this)[0]);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*right)[i], (*this)[i]);
    }  //C delete right;
  return res;
}
// doubles power of value: left=left ^ right
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements(); 
  assert( nEl);
  Data_* res = NewResult();
  if( nEl == 1)
  {
	(*res)[0] = pow( (*this)[0], (*right)[0]);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	for( int i=0; i < nEl; ++i)
	  (*res)[i] = pow( (*this)[i], (*right)[i]);
	}
  return res;
}
// doubles inverse power of value: left=right ^ left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  if( nEl == 1)
  {
	(*res)[0] = pow( (*right)[0], (*this)[0]);
	return res;
  }

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*right)[i], (*this)[i]);
    }  //C delete right;
  return res;
}
// complex power of value: left=left ^ right
// complex is special here
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowNew( BaseGDL* r)
{
SizeT nEl = N_Elements();

  assert( nEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == GDL_FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  DComplexGDL* res = new DComplexGDL( this->Dim(), 
					      BaseGDL::NOZERO);
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplex s;
	      if( StrictScalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      DComplexGDL* res = new DComplexGDL( this->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }
  if( r->Type() == GDL_LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      DLong s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  DComplexGDL* res = new DComplexGDL( this->Dim(), 
					      BaseGDL::NOZERO);
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplex s;
	      if( StrictScalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      DComplexGDL* res = new DComplexGDL( this->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements(); Data_* res = NewResult();
  //   if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->StrictScalar(s)) 
    {
      DComplexGDL* res = new DComplexGDL( this->Dim(), 
					  BaseGDL::NOZERO);
      //#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i<nEl; ++i)
	    (*res)[ i] = pow( (*this)[ i], s);
	}
	return res;
    }
  else 
    {
      DComplexGDL* res = new DComplexGDL( this->Dim(), 
					  BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}
	return res;
    }
}
// complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
    
  Data_* res = NewResult();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*right)[i], (*this)[i]);
    }
  return res;
}
// double complex power of value: left=left ^ right
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowNew( BaseGDL* r)
{
  SizeT nEl = N_Elements();

  assert( nEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == GDL_DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      DDouble s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						    BaseGDL::NOZERO);
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[ i] = pow( (*this)[ i], s);
	    }
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplexDbl s;
	      if( StrictScalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }
		  return res;
		}

	      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}
	      return res;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}
	      return res;
	    }
	}
    }
  if( r->Type() == GDL_LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      DLong s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						    BaseGDL::NOZERO);
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[ i] = pow( (*this)[ i], s);
	    }
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplexDbl s;
	      if( StrictScalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }
		  return res;
		}

	      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  Ty s;
  if( right->StrictScalar(s)) 
    {
      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i<nEl; ++i)
	    (*res)[ i] = pow( (*this)[ i], s);
	}
	return res;
    }
  else 
    {
      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( int i=0; i < nEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}
	return res;
    }
}
// double complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements(); 
  assert( rEl);
  assert( nEl);
  Data_* res = NewResult();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*right)[i], (*this)[i]);
    }
  return res;
}
// invalid types
DStructGDL* DStructGDL::PowNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::PowInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

// scalar versions
template<class Sp>
Data_<Sp>* Data_<Sp>::PowSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  Data_* res = NewResult();
  assert( nEl);
  Ty s = (*right)[0];
  if( nEl == 1)
  {
  	(*res)[0] = pow( (*this)[0], s);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( (*this)[i], s);
    }
  //C delete right;
  return res;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  Data_* res = NewResult();
  if( nEl == 1)
  {
  	(*res)[0] = pow( s, (*this)[0]);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i < nEl; ++i)
	(*res)[i] = pow( s, (*this)[i]);
    }  //C delete right;
  return res;
}
// complex power of value: left=left ^ right
// complex is special here
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowSNew( BaseGDL* r)
{
  SizeT nEl = N_Elements();
  assert( nEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == GDL_FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  Data_* res = NewResult();
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[i] =  pow( (*this)[ i], s);
	    }
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplex s;
	      if( StrictScalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }
		  return res;
		}

	      Data_* res = NewResult();
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[i] =  pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }
  if( r->Type() == GDL_LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      DLong s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  Data_* res = NewResult();
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[i] =  pow( (*this)[ i], s);
	    }	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplex s;
	      if( StrictScalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      Data_* res = NewResult();
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[i] =  pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}
	      return res;
	    }
	}
    }

  // r->Type() == GDL_COMPLEX
  Data_* right=static_cast<Data_*>(r);

  Ty s = (*right)[0];
  Data_* res = NewResult();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i<nEl; ++i)
	(*res)[i] =  pow( (*this)[ i], s);
    }

  return res;
}
// complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);
  ULong nEl=N_Elements();
  assert( nEl);
  assert( right->N_Elements());
  Ty s = (*right)[0];
  Data_* res = NewResult();
  if( nEl == 1)
  {
  	(*res)[0] = pow( s, (*this)[0]);
	return res;
  }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i<nEl; ++i)
	(*res)[i] =  pow( s, (*this)[ i]);
    }
  return res;
}
// double complex power of value: left=left ^ right
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowSNew( BaseGDL* r)
{
  SizeT nEl = N_Elements();

  assert( nEl > 0);

  if( r->Type() == GDL_DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      assert( right->N_Elements() > 0);

      DDouble s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->StrictScalar(s)) 
	{
	  Data_* res = NewResult();
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[i] =  pow( (*this)[ i], s);
	    }	  
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplexDbl s;
	      if( StrictScalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		
		  return res;
		}

	      Data_* res = NewResult();
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[i] =  pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	     
	      return res;
	    }
	}
    }
  if( r->Type() == GDL_LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      assert( right->N_Elements() > 0);

      DLong s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->StrictScalar(s)) 
	{
	  Data_* res = NewResult();
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( int i=0; i<nEl; ++i)
		(*res)[i] =  pow( (*this)[ i], s);
	    }	  
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( nEl < rEl)
	    {
	      DComplexDbl s;
	      if( StrictScalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		    {
#pragma omp for
		      for( int i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  
		  return res;
		}

	      Data_* res = NewResult();
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( int i=0; i<nEl; ++i)
		    (*res)[i] =  pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( int i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);
  const Ty s = (*right)[0];
  Data_* res = NewResult();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i<nEl; ++i)
	(*res)[i] =  pow( (*this)[ i], s);
    }
  return res;
}
// double complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvSNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  Data_* res = NewResult();
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( int i=0; i<nEl; ++i)
	(*res)[i] =  pow( s, (*this)[ i]);
    }
  return res;
}
// invalid types
DStructGDL* DStructGDL::PowSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
DStructGDL* DStructGDL::PowInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvSNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}

//#include "instantiate_templates.hpp"

#endif
