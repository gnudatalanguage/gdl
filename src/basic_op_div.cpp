/***************************************************************************
                          basic_op_div.cpp  -  GDL div (/) operators
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
	  for( OMPInt ix=i; ix < nEl; ++ix)
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
	  for( OMPInt ix=i; ix < nEl; ++ix)
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



#endif
