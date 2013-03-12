/***************************************************************************
                          basic_op_mult.cpp  -  GDL mult (*) operators
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
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mRight(&(*right)[0], nEl);
  mThis *= mRight;
  return this;
#else
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] *= (*right)[i];
    }  //C delete right;
  return this;
#endif
  
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
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mThis(&(*this)[0], nEl);
  mThis *= s;
  return this;
#else
  TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i < nEl; ++i)
	(*this)[i] *= s;
    }  //C delete right;
  return this;
#endif
  
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

#endif
