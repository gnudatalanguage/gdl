/***************************************************************************
                          basic_op.cpp  -  GDL operators
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

// header in datatypes.hpp

//#include "datatypes.hpp"
//#include "dstructgdl.hpp"
//#include "arrayindex.hpp"

//#include <csignal>
#include "sigfpehandler.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

#include "strassenmatrix.hpp"

using namespace std;

// Not operation
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::NotOp()
{
  ULong nEl=N_Elements();
  assert( nEl != 0);

  if( nEl == 1)
    {
      (*this)[0] = ~(*this)[0];
      return this;
    }

  //  if( !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = ~(*this)[i];
    }  return this;
}
// others
template<>
Data_<SpDFloat>* Data_<SpDFloat>::NotOp()
{
  ULong nEl=N_Elements();
  assert( nEl != 0);
  //  if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] = ((*this)[0] == 0.0f)? 1.0f : 0.0f;
      return this;
    }

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = ((*this)[i] == 0.0f)? 1.0f : 0.0f;
    }  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::NotOp()
{
  ULong nEl=N_Elements();
  assert( nEl != 0);
  //  if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] = ((*this)[0] == 0.0)? 1.0 : 0.0;
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = ((*this)[i] == 0.0)? 1.0 : 0.0;
    }  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
DStructGDL* DStructGDL::NotOp()
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// UMinus unary minus
// for numbers
template<class Sp>
BaseGDL* Data_<Sp>::UMinus()
{
  //  dd = -dd;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  //  if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] = -(*this)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = -(*this)[i];
    }  return this;
}
template<>
BaseGDL* Data_<SpDString>::UMinus()
{
  ULong nEl=N_Elements();
  assert( nEl != 0);
  //  if( !nEl) throw GDLException("Variable is undefined.");  
  Data_<SpDFloat>* newThis=static_cast<Data_<SpDFloat>*>(this->Convert2( FLOAT));
  //  this is deleted by convert2!!! 
  return static_cast<BaseGDL*>( newThis->UMinus());
}
BaseGDL* DStructGDL::UMinus()
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDPtr>::UMinus()
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
BaseGDL* Data_<SpDObj>::UMinus()
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// logical negation
// integers, also ptr and object
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  DByteGDL* res = new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
  
  if( nEl == 1)
    {
      (*res)[0] = ((*this)[0] == 0)? 1 : 0;
      return res;
    }
  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = ((*this)[i] == 0)? 1 : 0;
    }  return res;
}
template<>
Data_<SpDByte>* Data_<SpDFloat>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
  if( nEl == 1)
    {
      (*res)[0] = ((*this)[0] == 0.0f)? 1 : 0;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = ((*this)[i] == 0.0f)? 1 : 0;
    }  return res;
}
template<>
Data_<SpDByte>* Data_<SpDDouble>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  if( nEl == 1)
    {
      (*res)[0] = ((*this)[0] == 0.0)? 1 : 0;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = ((*this)[i] == 0.0)? 1 : 0;
    }  return res;
}
template<>
Data_<SpDByte>* Data_<SpDString>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  if( nEl == 1)
    {
      (*res)[0] = ((*this)[0] == "")? 1 : 0;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = ((*this)[i] == "")? 1 : 0;
    }  return res;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  if( nEl == 1)
    {
      (*res)[0] = ((*this)[0].real() == 0.0 && (*this)[0].imag() == 0.0)? 1 : 0;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = ((*this)[i].real() == 0.0 && (*this)[i].imag() == 0.0)? 1 : 0;
    }  return res;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  if( nEl == 1)
    {
      (*res)[0] = ((*this)[0].real() == 0.0 && (*this)[0].imag() == 0.0)? 1 : 0;
      return res;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*res)[i] = ((*this)[i].real() == 0.0 && (*this)[i].imag() == 0.0)? 1 : 0;
    }  return res;
}

// increment decrement operators
// integers
template<class Sp>
void Data_<Sp>::Dec()
{
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0]--;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i]--;
    }}
template<class Sp>
void Data_<Sp>::Inc()
{
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0]++;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i]++;
    }}
// float
template<>
void Data_<SpDFloat>::Dec()
{
  //   dd -= 1.0f;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] -= 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] -= 1.0;
    }}
template<>
void Data_<SpDFloat>::Inc()
{
  //   dd += 1.0f;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] += 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] += 1.0;
    }}
// double
template<>
void Data_<SpDDouble>::Dec()
{
  //   dd -= 1.0;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] -= 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] -= 1.0;
    }}
template<>
void Data_<SpDDouble>::Inc()
{
  //   dd += 1.0;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] += 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] += 1.0;
    }}
// complex
template<>
void Data_<SpDComplex>::Dec()
{
  //   dd -= 1.0f;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] -= 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] -= 1.0;
    }}
template<>
void Data_<SpDComplex>::Inc()
{
  //   dd += 1.0f;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] += 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] += 1.0;
    }}
template<>
void Data_<SpDComplexDbl>::Dec()
{
  //   dd -= 1.0;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] -= 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] -= 1.0;
    }}
template<>
void Data_<SpDComplexDbl>::Inc()
{
  //   dd += 1.0;
  ULong nEl=N_Elements();
  assert( nEl != 0);
  // if( !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] += 1.0;
      return;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] += 1.0;
    }}
// forbidden types
template<>
void Data_<SpDString>::Dec()
{
  throw GDLException("String expression not allowed in this context.",true,false);
}
template<>
void Data_<SpDPtr>::Dec()
{
  throw GDLException("Pointer expression not allowed in this context.",true,false);
}
template<>
void Data_<SpDObj>::Dec()
{
  throw GDLException("Object expression not allowed in this context.",true,false);
}
template<>
void Data_<SpDString>::Inc()
{
  throw GDLException("String expression not allowed in this context.",true,false);
}
template<>
void Data_<SpDPtr>::Inc()
{
  throw GDLException("Pointer expression not allowed in this context.",true,false);
}
template<>
void Data_<SpDObj>::Inc()
{
  throw GDLException("Object expression not allowed in this context.",true,false);
}


// binary operators

// 1. operators that always return a new result
// EqOp
// returns *this eq *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::EqOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //   if( nEl == 0)
  // 	 nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = (s == (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*this)[i] == s);
	}    }
  else if( StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] == s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] == s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] == (*this)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] == (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] == (*this)[i]);
	}    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::EqOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
// template<>
// Data_<SpDByte>* Data_<SpDPtr>::EqOp( BaseGDL* r)
// {
//   throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
//   return NULL;
// }
// template<>
// Data_<SpDByte>* Data_<SpDObj>::EqOp( BaseGDL* r)
// {
//   throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
//   return NULL;
// }

// NeOp
// returns *this ne *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::NeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;
  
  Ty s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = (s != (*this)[0]);
	  return res;
	}

      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*this)[i] != s);
	}    }
  else if( StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] != s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] != s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] != (*this)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] != (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] != (*this)[i]);
	}    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::NeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
// template<>
// Data_<SpDByte>* Data_<SpDPtr>::NeOp( BaseGDL* r)
// {
//   throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
//   return NULL;
// }
// template<>
// Data_<SpDByte>* Data_<SpDObj>::NeOp( BaseGDL* r)
// {
//   throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
//   return NULL;
// }

// LeOp
// returns *this le *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = ((*this)[0] <= s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*this)[i] <= s);
	}    }
  else if( StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] >= s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] >= s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] >= (*this)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] >= (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] >= (*this)[i]);
	}    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}

// LtOp
// returns *this lt *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LtOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = ((*this)[0] < s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*this)[i] < s);
	}    }
  else if( StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] > s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] > s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] > (*this)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] > (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] > (*this)[i]);
	}    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}

// GeOp
// returns *this ge *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::GeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;
  
  Ty s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = ((*this)[0] >= s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*this)[i] >= s);
	}    }
  else if( StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] <= s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] <= s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] <= (*this)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] <= (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] <= (*this)[i]);
	}    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}

// GtOp
// returns *this gt *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::GtOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( nEl == 1)
	{
	  (*res)[0] = ((*this)[0] > s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*this)[i] > s);
	}    }
  else if( StrictScalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] < s);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] < s);
	}    }
  else if( rEl < nEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl))
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = ((*right)[i] < (*this)[i]);
	}    }
  else // ( rEl >= nEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      if( rEl == 1)
	{
	  (*res)[0] = ((*right)[0] < (*this)[0]);
	  return res;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*res)[i] = ((*right)[i] < (*this)[i]);
	}    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return NULL;
}
// MatrixOp
// returns *this # *r, //C deletes itself and right
template<class Sp>
Data_<Sp>* Data_<Sp>::MatrixOp( BaseGDL* r, bool transpose, bool transposeResult, bool strassen)
{
  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements();
  //   assert( rEl);
  //   assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  Data_* res;

  if( this->dim.Rank() <= 1 && right->dim.Rank() <= 1)
    {
      // use transposed r if rank of both is <= 1
      // result dim
      SizeT nCol=this->dim[0];
      SizeT nRow=right->dim[0]; // transpose

      if( nCol == 0) nCol=1;
      if( nRow == 0) nRow=1;

      if( nRow > 1)
	res=New(dimension(nCol,nRow)); // zero values
      else
	res=New(dimension(nCol)); // zero values
      //      res->Purge(); // in case nRow == 1
#ifdef _OPENMP 
      SizeT nOp = nRow * nCol;
#endif
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) default(shared)
	{
#pragma omp for 
	  for( SizeT colA=0; colA < nCol; colA++)   // res dim 0
	    for( SizeT rowB=0; rowB < nRow; rowB++) // res dim 1
	      (*res)[ rowB * nCol + colA] += (*this)[colA] * (*right)[rowB];
	}
    }
  else
    {
      // [n] # [1,n] -> [n,n] ([n] -> [n,1]) 
      // [n] # [n,m] -> [1,m] ([n] -> [1,n])

      // right op 1st
      SizeT nRow=transpose ? right->dim[0] : right->dim[1];
      if( nRow == 0) nRow=1;

      // loop dim
      SizeT nRowEl=transpose ? right->dim[1] : right->dim[0];
      if( nRowEl == 0) nRowEl=1;

      // result dim
      SizeT nCol, nColEl;
      if( this->dim.Rank() <= 1)
	{
	  nColEl=this->dim[0];
	  if( nColEl == 0) // scalar
	    {
	      nColEl=1;
	      nCol  =1;
	    }
	  else if( nRowEl == 1)
	    {
	      nCol   = nColEl;
	      nColEl = 1;
	    }
	  else
	    {
	      nCol = 1;
	    }
	}
      else
	{ 
	  nCol=this->dim[0];
	  nColEl=this->dim[1];
	  assert( nColEl > 0); // rank is two -> cannot be zero
	  //	  if( nColEl == 0) nColEl=1;
	}
      
      //       cout << "nColEl, nRowEl: " << nColEl << " " << nRowEl << endl;
      //       cout << "nCol, nRow:     " << nCol << " " << nRow << endl;

      //      SizeT nRowEl=right->dim[0];
      if( nColEl != nRowEl)
	throw GDLException("Operands of matrix multiply have"
			   " incompatible dimensions.",true,false);  

      if( transposeResult)
	{
	  if( nCol > 1)
	    res=New(dimension( nRow, nCol),BaseGDL::NOZERO);
	  else
	    res=New(dimension(nRow),BaseGDL::NOZERO);
	}
      else
	{
	  if( nRow > 1)
	    res=New(dimension(nCol,nRow),BaseGDL::NOZERO);
	  else
	    res=New(dimension(nCol),BaseGDL::NOZERO);
	}
     
      SizeT rIxEnd = nRow * nColEl;
      //#ifdef _OPENMP 
      SizeT nOp = rIxEnd * nCol;

#ifdef USE_STRASSEN_MATRIXMULTIPLICATION
      if( !transpose && !transposeResult && strassen)
	//if( nOp > 1000000)
	{
	  SizeT maxDim;
	  if( nCol >= nColEl && nCol >= nRow)
	    maxDim = nCol;
	  else if( nColEl >= nRow)
	    maxDim = nColEl;
	  else
	    maxDim = nRow;

	  SizeT sOp = maxDim * maxDim * maxDim;
	  //if( (sOp / nOp) < 8)
	  {
	    SizeT mSz = 2;
	    while (mSz < maxDim) mSz <<= 1;

	    // 	      Ty* buf = new Ty[ 3 * mSz * mSz];

	    SM1<Ty>( mSz, nCol, nColEl, nRow,
		     static_cast<Ty*>(right->DataAddr()),
		     static_cast<Ty*>(this->DataAddr()),
		     static_cast<Ty*>(res->DataAddr()));

	    // 		delete[] buf;

	    return res;
	  }
	}
#endif

      //  for( SizeT j=0; j < nCol; ++j) // res dim 0
      // 	for( SizeT i=0; i < rIxEnd; i++) // res dim 1
      // 	    for( SizeT k=0; k < nColEl; ++k)
      // 	       (*res)[ (i * nCol) + j] += (*right)[ (i*nColEl)+k] * (*this)[ k*nCol+j];


      //#endif

      if( !transposeResult) // normal
	{
	  if( !transpose) // normal
	    {
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) default(shared)
		{
#pragma omp for
		  for( SizeT colA=0; colA < nCol; ++colA) // res dim 0
		    for( SizeT rIx=0, rowBnCol=0; rIx < rIxEnd;
			 rIx += nColEl, rowBnCol += nCol) // res dim 1
		      {
			Ty& resEl = (*res)[ rowBnCol + colA];
			resEl = 0;//(*this)[ colA] * (*right)[ rIx]; // initialization
			for( SizeT i=0; i < nColEl; ++i)
			  resEl += (*this)[ i*nCol+colA] * (*right)[ rIx+i];
		      }
		}
	    }
	  else // transpose r
	    {
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) default(shared)
		{
#pragma omp for
		  for( SizeT colA=0; colA < nCol; ++colA) // res dim 0
		    for( SizeT rIx=0, rowBnCol=0; rIx < nRow; ++rIx, rowBnCol += nCol) // res dim 1
		      {
			Ty& resEl = (*res)[ rowBnCol + colA];
			resEl = 0;//(*this)[ colA] * (*right)[ rIx]; // initialization
			for( SizeT i=0; i < nColEl; ++i)
			  resEl += (*this)[ i*nCol+colA] * (*right)[ rIx + i * nRow];
		      }
		}
	    }
	}
      else // transposeResult
	{
	  if( !transpose) // normal
	    {
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) default(shared)
		{
#pragma omp for
		  for( SizeT colA=0; colA < nCol; ++colA) // res dim 0
		    for( SizeT rIx=0, rowBnCol=0; rIx < rIxEnd;
			 rIx += nColEl, ++rowBnCol) // res dim 1
		      {
			Ty& resEl = (*res)[ rowBnCol + colA * nRow];
			resEl = 0;//(*this)[ colA] * (*right)[ rIx]; // initialization
			for( SizeT i=0; i < nColEl; ++i)
			  resEl += (*this)[ i*nCol+colA] * (*right)[ rIx+i];
		      }
		}
	    }
	  else // transpose r
	    {
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) default(shared)
		{
#pragma omp for
		  for( SizeT colA=0; colA < nCol; ++colA) // res dim 0
		    for( SizeT rIx=0; rIx < nRow; ++rIx) // res dim 1
		      {
			Ty& resEl = (*res)[ rIx + colA * nRow];
			resEl = 0;//(*this)[ colA] * (*right)[ rIx]; // initialization
			for( SizeT i=0; i < nColEl; ++i)
			  resEl += (*this)[ i*nCol+colA] * (*right)[ rIx + i * nRow];
		      }
		}
	    }
	}


    }
  //C delete right;
  //C delete this;
  return res;
}





// invalid types
DStructGDL* DStructGDL::MatrixOp( BaseGDL* r, bool t, bool tr, bool s)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::MatrixOp( BaseGDL* r, bool t, bool tr,  bool s)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MatrixOp( BaseGDL* r, bool t, bool tr,  bool s)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::MatrixOp( BaseGDL* r, bool t, bool tr,  bool s)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return NULL;
}


// 2. operators which operate on 'this'
// AndOp
// Ands right to itself, //C deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      (*this)[0] = (*this)[0] & (*right)[0]; // & Ty(1);
      return this;
    }
  // note: we can't use valarray operation here as right->dd 
  // might be larger than this->dd
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = (*this)[i] & (*right)[i]; // & Ty(1);
    }  //C delete right;
  return this;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInv( BaseGDL* right)
{
  return AndOp( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      if( (*right)[0] == zero) (*this)[0]=zero;
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*right)[i] == zero) (*this)[i]=zero;
      //     if( (*this)[i] == zero || (*right)[i] == zero) (*this)[i]=zero;
    }  //C delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      if( (*this)[0] != zero) (*this)[0] = (*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] != zero) (*this)[i] = (*right)[i];
    }  //C delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      if( (*right)[0] == zero) (*this)[0]=zero;
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*right)[i] == zero) (*this)[i]=zero;
      //     if( (*this)[i] == zero || (*right)[i] == zero) (*this)[i]=zero;
    }  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  if( nEl == 1)
    {
      if( (*this)[0] != zero) (*this)[0] = (*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] != zero) (*this)[i] = (*right)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::AndOpInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
// template<>
// DStructGDL* DStructGDL::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
//  return this;
// }
template<>
Data_<SpDString>* Data_<SpDString>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
//  return this;
// }
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
//  return this;
// }
template<>
Data_<SpDObj>* Data_<SpDObj>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
//  return this;
// }
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);

  Ty s = (*right)[0];
  // right->Scalar(s);

  // s &= Ty(1);
  //  dd &= s;
  if( nEl == 1)
    {
      (*this)[0] &= s;
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared(s)
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] &= s;
    }
  return this;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpInvS( BaseGDL* right)
{
  return AndOpS( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s == zero)
    //   dd = zero;
    {
      // #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
	// #pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*this)[i] = zero;
      }}
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();

  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s == zero)
    //    dd = zero;
    {
      // #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
	// #pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*this)[i] = zero;
      }}
  else
    {
      if( nEl == 1)
	{
	  if( (*this)[0] != zero) (*this)[0] = s;
	  return this;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*this)[i] = s;
	}}
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();

  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  if( s == zero)
    //    dd = zero;
    // #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
      // #pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = zero;
    }
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s == zero)
    //    dd = zero;
    {
      // #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
	// #pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*this)[i] = zero;
      }}
  else
    {
      if( nEl == 1)
	{
	  if( (*this)[0] != zero) (*this)[0] = s;
	  return this;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*this)[i] = s;
	}}
  return this;
}
// invalid types
DStructGDL* DStructGDL::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::AndOpInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
// template<>
// DStructGDL* DStructGDL::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
//  return this;
// }
template<>
Data_<SpDString>* Data_<SpDString>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInvS( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
//  return this;
// }
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}


// OrOp
// Ors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] = (*this)[0] | (*right)[0]; // | Ty(1);
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = (*this)[i] | (*right)[i]; // | Ty(1);
    }
  //C delete right;
  return this;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInv( BaseGDL* right)
{
  return OrOp( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*this)[0] == zero) (*this)[0]=(*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] == zero) (*this)[i]=(*right)[i];
    }  //C delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*right)[0] != zero) (*this)[0] = (*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*right)[i] != zero) (*this)[i] = (*right)[i];
    }  //C delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*this)[0] == zero) (*this)[0]= (*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] == zero) (*this)[i]= (*right)[i];
    }  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*right)[0] != zero) (*this)[0] = (*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*right)[i] != zero) (*this)[i] = (*right)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::OrOpInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
// OrOp
// Ors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //s &= Ty(1);
  //  dd |= s;
  if( nEl == 1)
    {
      (*this)[0] = (*this)[0] | s;
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = (*this)[i] | s;
    }  //C delete right;
  return this;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvS( BaseGDL* right)
{
  return OrOpS( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  if( s != zero)
    {
      if( nEl == 1)
	{
	  if( (*this)[0] == zero) (*this)[0] = s;
	  return this;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    if( (*this)[i] == zero) (*this)[i] = s;
	}}  //C delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s != zero)
    //    dd = s;
    //C delete right;
    return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s != zero)
    {
      if( nEl == 1)
	{
	  if( (*this)[0] == zero) (*this)[0] = s;
	  return this;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    if( (*this)[i] == zero) (*this)[i] = s;
	}}  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  if( s != zero)
    //    dd = s;
    {
      // #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
	// #pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*this)[i] = s;
      }}
  else
    {
      if( nEl == 1)
	{
	  if( (*this)[0] != zero) (*this)[0] = s;
	  return this;
	}
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    if( (*this)[i] != zero) (*this)[i] = s;
	}}  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::OrOpInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// XorOp
// Xors right to itself, //C deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::XorOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");
  if( nEl == 1)
    {
      (*this)[0] ^= (*right)[0];
      return this;
    }
  Ty s = (*right)[0];
  if( right->StrictScalar(s))
    {
      if( s != Sp::zero)
	//	dd ^= s;
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i < nEl; ++i)
		(*this)[i] ^= s;
	    }}
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*this)[i] ^= (*right)[i];
	}    }
  //C delete right;
  return this;
}
// invalid types
template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype FLOAT.",true,false);  
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype DOUBLE.",true,false);  
  return this;
}
DStructGDL* DStructGDL::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::XorOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  if( nEl == 1)
    {
      (*this)[0] ^= /*(*this)[0] ^*/  (*right)[0];
      return this;
    }
  Ty s = (*right)[0];
  //  dd ^= s;
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] ^= s;
      //     (*this)[i] = (*this)[i] ^ s;
    }  //C delete right;
  return this;
}
// different for floats
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype FLOAT.",true,false);  
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype DOUBLE.",true,false);  
  return this;
}
// invalid types
DStructGDL* DStructGDL::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::XorOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// Add
// Adds right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::Add( BaseGDL* r)
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

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] += (*right)[i];
    }  //C delete right;
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::AddInv( BaseGDL* r)
{
  return Add( r);
}
template<>
Data_<SpDString>* Data_<SpDString>::AddInv( BaseGDL* r)
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
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = (*right)[i] + (*this)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::AddInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::AddS( BaseGDL* r)
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
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] += s;
    }  //C delete right;
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::AddInvS( BaseGDL* r)
{
  return AddS( r);
}
template<>
Data_<SpDString>* Data_<SpDString>::AddInvS( BaseGDL* r)
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
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = s + (*this)[i];
    }  //C delete right;
  return this;
}

// invalid types
DStructGDL* DStructGDL::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::AddInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// Sub
// substraction: left=left-right
template<class Sp>
Data_<Sp>* Data_<Sp>::Sub( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == rEl)
    dd -= right->dd;
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*this)[i] -= (*right)[i];
	}}  //C delete right;
  return this;
}
// inverse substraction: left=right-left
template<class Sp>
Data_<Sp>* Data_<Sp>::SubInv( BaseGDL* r)
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
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = (*right)[i] - (*this)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
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
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] -= s;
    }  //C delete right;
  return this;
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
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = s - (*this)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
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

// LtMark
// LtMarks right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::LtMark( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*this)[0] > (*right)[0]) (*this)[0]=(*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] > (*right)[i]) (*this)[i]=(*right)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);
  
  ULong nEl=N_Elements();
  assert( nEl);
  if( nEl == 1)
    {
      if( (*this)[0] > (*right)[0]) (*this)[0]=(*right)[0];
      return this;
    }
  Ty s = (*right)[0];
  // right->Scalar(s);
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] > s) (*this)[i]=s;
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
// GtMark
// GtMarks right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::GtMark( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      if( (*this)[0] < (*right)[0]) (*this)[0]=(*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] < (*right)[i]) (*this)[i]=(*right)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  if( nEl == 1)
    {
      if( (*this)[0] < (*right)[0]) (*this)[0]=(*right)[0];
      return this;
    }

  Ty s = (*right)[0];
  // right->Scalar(s);
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	if( (*this)[i] < s) (*this)[i]=s;
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// Mult
// Mults right to itself, //C deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::Mult( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( nEl == 1)
    {
      (*this)[0] *= (*right)[0];
      return this;
    }
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] *= (*right)[i];
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

template<class Sp>
Data_<Sp>* Data_<Sp>::MultS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  if( nEl == 1)
    {
      (*this)[0] *= (*right)[0];
      return this;
    }
  Ty s = (*right)[0];
  // right->Scalar(s);
  //  dd *= s;
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] *= s;
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// Div
// division: left=left/right
template<class Sp>
Data_<Sp>* Data_<Sp>::Div( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  SizeT i = 0;

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // TODO: Check if we can use OpenMP here (is longjmp allowed?)
      //             if yes: need to run the full loop after the longjmp
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] /= (*right)[i];
      //C delete right;
      return this;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	  //       bool zeroEncountered = false; // until zero operation is already done.
#pragma omp for
	  for( SizeT ix=i; ix < nEl; ++ix)
	    /*	if( !zeroEncountered)
		{
		if( (*right)[ix] == this->zero)
		zeroEncountered = true;
		}
		else*/
	    if( (*right)[ix] != this->zero) (*this)[ix] /= (*right)[ix];
	}      //C delete right;
      return this;
    }
}
// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);

  SizeT i = 0;

  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] = (*right)[i] / (*this)[i];
      //C delete right;
      return this;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	  //       bool zeroEncountered = false; // until zero operation is already done.
#pragma omp for
	  for( SizeT ix=i; ix < nEl; ++ix)
	    /*	if( !zeroEncountered)
		{
		if( (*this)[ix] == this->zero)
		{
		zeroEncountered = true;
		(*this)[ ix] = (*right)[i];
		}
		}
		else*/
	    if( (*this)[ix] != this->zero) 
	      (*this)[ix] = (*right)[ix] / (*this)[ix]; 
	    else
	      (*this)[ix] = (*right)[ix];
	}      //C delete right;
      return this;
    }
}
// invalid types
DStructGDL* DStructGDL::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::DivS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];

  // remember: this is a template (must work for several types)
  // due to error handling the actual devision by 0
  // has to be done 
  // but if not 0, we save the expensive error handling
  if( s != this->zero)
    {
      for(SizeT i=0; i < nEl; ++i)
      {
	(*this)[i] /= s;
      }
      return this;
    }
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for(SizeT i=0; i < nEl; ++i)
      {
	(*this)[i] /= s;
      }
      return this;
    }
  return this;
}

// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  // remember: this is a template (must work for several types)
  // due to error handling the actual devision by 0
  // has to be done 
  // but if not 0, we save the expensive error handling
  if( nEl == 1 && (*this)[0] != this->zero) 
  {
    (*this)[0] = (*right)[0] / (*this)[0]; 
    return this;
  }
  
  Ty s = (*right)[0];
  SizeT i=0;
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] = s / (*this)[i];
      //C delete right;
      return this;
    }
  else
    {
//      TRACEOMP( __FILE__, __LINE__)
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
// 	{
// 	  //       bool zeroEncountered = false;
// #pragma omp for
	  // right->Scalar(s); 
	  for( SizeT ix=i; ix < nEl; ++ix)
	    /*	if( !zeroEncountered)
		{
		if( (*this)[ix] == this->zero)
		{
		zeroEncountered = true;
		(*this)[ix] = s;
		}
		}
		else*/
	    if( (*this)[ix] != this->zero) 
	      (*this)[ix] = s / (*this)[ix]; 
	    else 
	      (*this)[ix] = s;
// 	}      //C delete right;
      return this;
    }
}
// invalid types
DStructGDL* DStructGDL::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// Mod
// modulo division: left=left % right
template<class Sp>
Data_<Sp>* Data_<Sp>::Mod( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);

  SizeT i=0;
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] %= (*right)[i];
      //C delete right;
      return this;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	  //       bool zeroEncountered = false;
#pragma omp for
	  for( SizeT ix=i; ix < nEl; ++ix)
	    /*	if( !zeroEncountered)
		{
		if( (*right)[i] == this->zero)
		{
		zeroEncountered = true;
		(*this)[i] = this->zero;
		}
		}
		else*/
	    if( (*right)[ix] != this->zero) 
	      (*this)[ix] %= (*right)[ix];
	    else
	      (*this)[ix] = this->zero;
	}    //C delete right;
      return this;
    }
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  SizeT i=0;

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] = (*right)[i] % (*this)[i];
      //C delete right;
      return this;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
	  //       bool zeroEncountered = false;
#pragma omp for
	  for( SizeT ix=i; ix < nEl; ++ix)
	    /*	if( !zeroEncountered)
		{
		if( (*this)[ix] == this->zero)
		{
		zeroEncountered = true;
		(*this)[ ix] = this->zero;
		}
		}
		else*/
	    if( (*this)[ix] != this->zero) 
	      (*this)[ix] = (*right)[ix] % (*this)[ix]; 
	    else
	      (*this)[ix] = this->zero;
	}      //C delete right;
      return this;
    }    
}
// float modulo division: left=left % right
inline DFloat Modulo( const DFloat& l, const DFloat& r)
{
  float t=abs(l/r);
  if( l < 0.0) return t=(floor(t)-t)*abs(r);
  return (t-floor(t))*abs(r);
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::Mod( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo((*this)[i],(*right)[i]);
    }  //C delete right;
  return this;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo((*right)[i],(*this)[i]);
    }  //C delete right;
  return this;
}
// in basic_op.cpp
// double modulo division: left=left % right
// inline DDouble Modulo( const DDouble& l, const DDouble& r)
// {
//   DDouble t=abs(l/r);
//   if( l < 0.0) return t=(floor(t)-t)*abs(r);
//   return (t-floor(t))*abs(r);
// }
template<>
Data_<SpDDouble>* Data_<SpDDouble>::Mod( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo((*this)[i],(*right)[i]);
    }  //C delete right;
  return this;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  // assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo((*right)[i],(*this)[i]);
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::ModS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  
  Ty s = (*right)[0];
  SizeT i=0;

  // remember: this is a template (must work for several types)
  // due to error handling the actual devision by 0
  // has to be done 
  // but if not 0, we save the expensive error handling
  if( s != this->zero)
    {
      // right->Scalar(s); 
      //     dd %= s;
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] %= s;
      //C delete right;
      return this;
    }
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      //     dd %= s;
      for( /*SizeT i=0*/; i < nEl; ++i)
	(*this)[i] %= s;
      //C delete right;
      return this;
    }
  else
    {
      //       bool zeroEncountered = false; // until zero operation is already done.
      
      // right->Scalar(s); 
      assert( s == this->zero);
      // #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS)// && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
	// #pragma omp for
	for( SizeT ix=i; ix < nEl; ++ix)
	  (*this)[ix] = 0;
      }      //C delete right;
      return this;
    }
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  

  // remember: this is a template (must work for several types)
  // due to error handling the actual devision by 0
  // has to be done 
  // but if not 0, we save the expensive error handling
  if( nEl == 1 && (*this)[0] != this->zero) 
  {
    (*this)[0] = (*right)[0] % (*this)[0]; 
    return this;
  }

  Ty s = (*right)[0];
  SizeT i=0;
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      for( /*SizeT i=0*/; i < nEl; ++i)
	{
	  (*this)[i] = s % (*this)[i];
	}
      //C delete right;
      return this;
    }
  else
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  //       bool zeroEncountered = false;
	  // right->Scalar(s); 
	  for( SizeT ix=i; ix < nEl; ++ix)
	    /*	if( !zeroEncountered)
		{
		if( (*this)[ix] == this->zero)
		{
		zeroEncountered = true;
		(*this)[ix] = this->zero;
		}
		}
		else*/
	    if( (*this)[ix] != this->zero) 
	      (*this)[ix] = s % (*this)[ix]; 
	    else 
	      (*this)[ix] = this->zero;
	}      //C delete right;
      return this;
    }    
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo((*this)[i],s);
    }  //C delete right;
  return this;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo(s,(*this)[i]);
    }  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s);
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo((*this)[i],s);
    }  //C delete right;
  return this;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = Modulo(s,(*this)[i]);
    }  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModS( BaseGDL* r)

{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}

// Pow
// C++ defines pow only for floats and doubles
//template <typename T, typename TT> T pow( const T r, const TT l)
template <typename T> T pow( const T r, const T l)
{
  typedef T TT;

  if( l == 0) return 1;
  if( l < 0)  return 0;

  const int nBits = sizeof(TT) * 8;

  T arr = r;
  T res = 1;
  TT mask = 1;
  for( SizeT i=0; i<nBits; ++i)
    {
      if( l & mask) res *= arr;
      mask <<= 1;
      if( l < mask) return res;
      arr *= arr;
    }

  return res;
}

// power of value: left=left ^ right
// integral types
template<class Sp>
Data_<Sp>* Data_<Sp>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*this)[i], (*right)[i]); // valarray
    }  //C delete right;
  return this;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  //  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(nEl);
  //      dd = pow( right->Resize(nEl), dd); // valarray
      
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*right)[i], (*this)[i]);
    }  //C delete right;
  return this;
}
// floats power of value: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  /*  if( rEl == nEl)
      {
      for( SizeT i=0; i < nEl; ++i)
      dd[ i] = pow( dd[ i], right->dd[ i]); // valarray
      }
      else*/
  {
    TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( SizeT i=0; i < nEl; ++i)
	  (*this)[i] = pow( (*this)[i], (*right)[i]);
      }    }
  return this;
}

// PowInt and PowIntNew can only be called for FLOAT and DOUBLE
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInt( BaseGDL* r)
{
  assert( 0);
  return this;
}
DStructGDL* DStructGDL::PowInt( BaseGDL* r)
{
  assert( 0);
  return this;
}
// floats power of value with LONG: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInt( BaseGDL* r)
{
  DLongGDL* right=static_cast<DLongGDL*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  if( r->StrictScalar())
    {
      DLong r0 = (*right)[0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*this)[i] = pow( (*this)[i], r0);
	}      return this;
    }
  if( StrictScalar())
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      Ty s0 = (*this)[ 0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[ i] = pow( s0, (*right)[ i]);
	}      return res;
    }
  if( nEl <= rEl)
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*this)[i] = pow( (*this)[i], (*right)[i]);
	}      return this;
    }
  else
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}      return res;
    }
}

template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInt( BaseGDL* r)
{
  DLongGDL* right=static_cast<DLongGDL*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  if( r->StrictScalar())
    {
      DLong r0 = (*right)[0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*this)[i] = pow( (*this)[i], r0);
	}      return this;
    }
  if( StrictScalar())
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      Ty s0 = (*this)[ 0];  
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[ i] = pow( s0, (*right)[ i]);
	}      return res;
    }
  if( nEl <= rEl)
    {
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( SizeT i=0; i < nEl; ++i)
	    (*this)[i] = pow( (*this)[i], (*right)[i]);
	}      return this;
    }
  else
    {
      Data_* res = new Data_( right->Dim(), BaseGDL::NOZERO);
      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
	{
#pragma omp for
	  for( SizeT i=0; i < rEl; ++i)
	    (*res)[i] = pow( (*this)[i], (*right)[i]);
	}      return res;
    }
}

// floats inverse power of value: left=right ^ left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(nEl);
  /*  if( rEl == nEl)
      dd = pow( right->dd, dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*right)[i], (*this)[i]);
    }  //C delete right;
  return this;
}
// doubles power of value: left=left ^ right
template<>
Data_<SpDDouble>* Data_<SpDDouble>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(nEl);
  //      dd = pow( dd, right->Resize(nEl)); // valarray
  /*  if( rEl == nEl)
      dd = pow( dd, right->dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*this)[i], (*right)[i]);
    }  //C delete right;
  return this;
}
// doubles inverse power of value: left=right ^ left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(nEl);
  // dd = pow( right->Resize(nEl), dd); // valarray
  /*  if( rEl == nEl)
      dd = pow( right->dd, dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*right)[i], (*this)[i]);
    }  //C delete right;
  return this;
}
// complex power of value: left=left ^ right
// complex is special here
template<>
Data_<SpDComplex>* Data_<SpDComplex>::Pow( BaseGDL* r)
{
  SizeT nEl = N_Elements();

  assert( nEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }
  if( r->Type() == LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      DLong s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements();
  //   if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(nEl);
  //      dd = pow( dd, right->dd); // valarray
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<nEl; ++i)
    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
#else
  //      dd = pow( dd, right->Resize(nEl)); // valarray
  /*  if( r->N_Elements() == nEl)
      dd = pow( dd, right->dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*this)[i], (*right)[i]);
    }
#endif
  //C delete right;
  return this;
}
// complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<nEl; ++i)
    (*this)[ i] = pow( (*right)[ i], (*this)[i]);
#else
  //      right->dd.resize(nEl);
  //      dd = pow( right->Resize(nEl), dd); // valarray
  /*  if( rEl == nEl)
      dd = pow( right->dd, dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*right)[i], (*this)[i]);
#endif
    }  //C delete right;
  return this;
}
// double complex power of value: left=left ^ right
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Pow( BaseGDL* r)
{
  SizeT nEl = N_Elements();

  assert( nEl > 0);

  if( r->Type() == DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      assert( right->N_Elements() > 0);

      DDouble s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }
  if( r->Type() == LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      assert( right->N_Elements() > 0);

      DLong s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements();
  //   if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(nEl);
  //      dd = pow( dd, right->dd); // valarray
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<nEl; ++i)
    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
#else
  //      dd = pow( dd, right->Resize(nEl)); // valarray
  /*  if( r->N_Elements() == nEl)
      dd = pow( dd, right->dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*this)[i], (*right)[i]);
    }
#endif
  //C delete right;
  return this;
}
// double complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<nEl; ++i)
    (*this)[ i] = pow( (*right)[ i], (*this)[i]);
#else
  //      right->dd.resize(nEl);
  //      dd = pow( right->Resize(nEl), dd); // valarray
  /*  if( rEl == nEl)
      dd = pow( right->dd, dd); // valarray
      else*/
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*right)[i], (*this)[i]);
    }
#endif
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::PowS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 

  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( (*this)[i], s); 
    }
  //C delete right;
  return this;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //      dd = pow( s, d); // valarray
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i < nEl; ++i)
	(*this)[i] = pow( s, (*this)[i]);
    }  //C delete right;
  return this;
}
// floats power of value: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)	
	dd[ i] = pow( dd[ i], s); // valarray
    }  //C delete right;
  return this;
}
// floats inverse power of value: left=right ^ left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)	
	dd[ i] = pow( s, dd[ i]); // valarray
    }  //C delete right;
  return this;
}
// doubles power of value: left=left ^ right
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)	
	dd[ i] = pow( dd[ i], s); // valarray
    }  //C delete right;
  return this;
}
// doubles inverse power of value: left=right ^ left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  Ty s = (*right)[0];
  // right->Scalar(s); 
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)	
	dd[ i] = pow( s, dd[ i]); // valarray
    }  //C delete right;
  return this;
}
// complex power of value: left=left ^ right
// complex is special here
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowS( BaseGDL* r)
{
  SizeT nEl = N_Elements();

  assert( nEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }
  if( r->Type() == LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      DLong s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements();
  //   if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	(*this)[ i] = pow( (*this)[ i], s);
    }
  //#else
  //  dd = pow( dd, s); // valarray
  //#endif
  //C delete right;

  return this;
}
// complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong nEl=N_Elements();
  assert( rEl);
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	(*this)[ i] = pow( s, (*this)[ i]);
    }
  //#else
  //  dd = pow( s, dd); // valarray
  //#endif
  //C delete right;
  return this;
}
// double complex power of value: left=left ^ right
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowS( BaseGDL* r)
{
  SizeT nEl = N_Elements();

  assert( nEl > 0);

  if( r->Type() == DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      assert( right->N_Elements() > 0);

      DDouble s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }
  if( r->Type() == LONG)
    {
      Data_<SpDLong>* right=static_cast<Data_<SpDLong>* >(r);

      assert( right->N_Elements() > 0);

      DLong s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->StrictScalar(s)) 
	{
	  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( SizeT i=0; i<nEl; ++i)
		(*this)[ i] = pow( (*this)[ i], s);
	    }	  //C delete right;
	  return this;
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
		      for( SizeT i=0; i<rEl; ++i)
			(*res)[ i] = pow( s, (*right)[ i]);
		    }		  //C delete right;
		  return res;
		}

	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
		{
#pragma omp for
		  for( SizeT i=0; i<nEl; ++i)
		    (*this)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (rEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= rEl)) 
		{
#pragma omp for
		  for( SizeT i=0; i<rEl; ++i)
		    (*res)[ i] = pow( (*this)[ i], (*right)[ i]);
		}	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong nEl=N_Elements();
  //   if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	(*this)[ i] = pow( (*this)[ i], s);
    }
  //#else
  //  dd = pow( dd, s); // valarray
  //#endif
  //C delete right;
  return this;
}
// double complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong nEl=N_Elements();
  assert( nEl);
  //  if( !rEl || !nEl) throw GDLException("Variable is undefined.");  
  Ty s = (*right)[0];
  // right->Scalar(s); 
  //#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	(*this)[ i] = pow( s, (*this)[ i]);
    }
  //#else
  //  dd = pow( s, dd); // valarray
  //#endif
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
DStructGDL* DStructGDL::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.",true,false);  
  return this;
}


//#include "instantiate_templates.hpp"

#endif
