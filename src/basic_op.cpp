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

using namespace std;

// Not operation
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::NotOp()
{
  ULong sEl=N_Elements();
  assert( sEl != 0);
  //  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = ~dd[i];
  return this;
}
// others
template<>
Data_<SpDFloat>* Data_<SpDFloat>::NotOp()
{
  ULong sEl=N_Elements();
  assert( sEl != 0);
  //  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = (dd[i] == 0.0f)? 1.0f : 0.0f;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::NotOp()
{
  ULong sEl=N_Elements();
  assert( sEl != 0);
  //  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = (dd[i] == 0.0)? 1.0 : 0.0;
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
DStructGDL* DStructGDL::NotOp()
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::NotOp()
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}

// UMinus unary minus
// for numbers
template<class Sp>
BaseGDL* Data_<Sp>::UMinus()
{
  dd = -dd;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   //  if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] = -dd[i];
  return this;
}
template<>
BaseGDL* Data_<SpDString>::UMinus()
{
  ULong sEl=N_Elements();
  assert( sEl != 0);
  //  if( !sEl) throw GDLException("Variable is undefined.");  
  Data_<SpDFloat>* newThis=static_cast<Data_<SpDFloat>*>(this->Convert2( FLOAT));
  //  this is deleted by convert2!!! 
  return static_cast<BaseGDL*>( newThis->UMinus());
}
BaseGDL* DStructGDL::UMinus()
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
BaseGDL* Data_<SpDPtr>::UMinus()
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
BaseGDL* Data_<SpDObj>::UMinus()
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
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
  for( SizeT i=0; i < nEl; i++)
    (*res)[i] = (dd[i] == 0)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDFloat>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
    (*res)[i] = (dd[i] == 0.0f)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDDouble>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
    (*res)[i] = (dd[i] == 0.0)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDString>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
    (*res)[i] = (dd[i] == "")? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
    (*res)[i] = (dd[i].real() == 0.0 && dd[i].imag() == 0.0)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LogNeg()
{
  SizeT nEl = dd.size();
  assert( nEl);
  //  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
    (*res)[i] = (dd[i].real() == 0.0 && dd[i].imag() == 0.0)? 1 : 0;
  return res;
}

// increment decrement operators
// integers
template<class Sp>
void Data_<Sp>::Dec()
{
  ULong sEl=N_Elements();
  assert( sEl != 0);
  // if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i]--;
}
template<class Sp>
void Data_<Sp>::Inc()
{
  ULong sEl=N_Elements();
  assert( sEl != 0);
  // if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i]++;
}
// float
template<>
void Data_<SpDFloat>::Dec()
{
  dd -= 1.0f;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] -= 1.0;
}
template<>
void Data_<SpDFloat>::Inc()
{
  dd += 1.0f;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] += 1.0;
}
// double
template<>
void Data_<SpDDouble>::Dec()
{
  dd -= 1.0;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] -= 1.0;
}
template<>
void Data_<SpDDouble>::Inc()
{
  dd += 1.0;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] += 1.0;
}
// complex
template<>
void Data_<SpDComplex>::Dec()
{
  dd -= 1.0f;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] -= 1.0;
}
template<>
void Data_<SpDComplex>::Inc()
{
  dd += 1.0f;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] += 1.0;
}
template<>
void Data_<SpDComplexDbl>::Dec()
{
  dd -= 1.0;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] -= 1.0;
}
template<>
void Data_<SpDComplexDbl>::Inc()
{
  dd += 1.0;
//   ULong sEl=N_Elements();
//   assert( sEl != 0);
//   // if( !sEl) throw GDLException("Variable is undefined.");  
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] += 1.0;
}
// forbidden types
template<>
void Data_<SpDString>::Dec()
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
void Data_<SpDPtr>::Dec()
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
void Data_<SpDObj>::Dec()
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
void Data_<SpDString>::Inc()
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
void Data_<SpDPtr>::Inc()
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
void Data_<SpDObj>::Inc()
{
  throw GDLException("Object expression not allowed in this context.");
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
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->Scalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (dd[i] == s);
    }
  else if( Scalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] == s);
    }
  else if( rEl < sEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] == dd[i]);
    }
  else // ( rEl >= sEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (right->dd[i] == dd[i]);
    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::EqOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::EqOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::EqOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return NULL;
}

// NeOp
// returns *this ne *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::NeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->Scalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (dd[i] != s);
    }
  else if( Scalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] != s);
    }
  else if( rEl < sEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] != dd[i]);
    }
  else // ( rEl >= sEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (right->dd[i] != dd[i]);
    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::NeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::NeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::NeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return NULL;
}

// LeOp
// returns *this le *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->Scalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (dd[i] <= s);
    }
  else if( Scalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] >= s);
    }
  else if( rEl < sEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] >= dd[i]);
    }
  else // ( rEl >= sEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (right->dd[i] >= dd[i]);
    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}

// LtOp
// returns *this lt *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LtOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->Scalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (dd[i] < s);
    }
  else if( Scalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] > s);
    }
  else if( rEl < sEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] > dd[i]);
    }
  else // ( rEl >= sEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (right->dd[i] > dd[i]);
    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}

// GeOp
// returns *this ge *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::GeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->Scalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (dd[i] >= s);
    }
  else if( Scalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] <= s);
    }
  else if( rEl < sEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] <= dd[i]);
    }
  else // ( rEl >= sEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (right->dd[i] <= dd[i]);
    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::GeOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}

// GtOp
// returns *this gt *r, //C deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::GtOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_<SpDByte>* res;

  Ty s;
  if( right->Scalar(s)) 
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (dd[i] > s);
    }
  else if( Scalar(s)) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] < s);
    }
  else if( rEl < sEl) 
    {
      res= new Data_<SpDByte>( right->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < rEl; i++)
	(*res)[i] = (right->dd[i] < dd[i]);
    }
  else // ( rEl >= sEl)
    {
      res= new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
      for( SizeT i=0; i < sEl; i++)
	(*res)[i] = (right->dd[i] < dd[i]);
    }
  //C delete right;
  //C delete this;
  return res;
}
// invalid types
Data_<SpDByte>* DStructGDL::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDPtr>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDObj>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::GtOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return NULL;
}
// MatrixOp
// returns *this # *r, //C deletes itself and right
template<class Sp>
Data_<Sp>* Data_<Sp>::MatrixOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

//   ULong rEl=right->N_Elements();
//   ULong sEl=N_Elements();
//   assert( rEl);
//   assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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

      for( SizeT colA=0; colA < nCol; colA++)   // res dim 0
	for( SizeT rowB=0; rowB < nRow; rowB++) // res dim 1
	  res->dd[ rowB * nCol + colA] += dd[colA] * right->dd[rowB];
    }
  else
    {
      // result dim
      SizeT nCol, nColEl;
      if( this->dim.Rank() <= 1)
	{
	  nCol=1;
	  nColEl=this->dim[0];
	}
      else
	{
	  nCol=this->dim[0];
	  nColEl=this->dim[1];
	}
      if( nColEl == 0) nColEl=1;
      
      SizeT nRow=right->dim[1];
      if( nRow == 0) nRow=1;

      // loop dim
      SizeT nRowEl=right->dim[0];

      if( nRowEl == 0) nRowEl=1;

      if( nColEl != nRowEl)
	throw GDLException("Operands of matrix multiply have"
			   " incompatible dimensions.");  

      if( nRow > 1)
	res=New(dimension(nCol,nRow)); // zero values
      else
	res=New(dimension(nCol)); // zero values
//       res = New( dimension( nCol, nRow), BaseGDL::NOZERO);
//       res->Purge();
     
      SizeT rIxEnd = nRow * nColEl;
      
      for( SizeT colA=0; colA < nCol; ++colA) // res dim 0
	for( SizeT rIx=0, rowBnCol=0; rIx < rIxEnd; 
	     rIx += nColEl, rowBnCol += nCol) // res dim 1
	  {
	    Ty& resEl = res->dd[ rowBnCol + colA];
	    resEl = dd[ colA] * right->dd[ rIx];
	    for( SizeT i=1; i < nColEl; ++i)
	      resEl += dd[ i*nCol+colA] * right->dd[ rIx+i];
	  }
    }

  //C delete right;
  //C delete this;
  return res;
}
// invalid types
DStructGDL* DStructGDL::MatrixOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return NULL;
}
template<>
Data_<SpDString>* Data_<SpDString>::MatrixOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MatrixOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return NULL;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::MatrixOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  // note: we can't use valarray operation here as right->dd 
  // might be larger than this->dd
  for( SizeT i=0; i < sEl; i++)
    dd[i] = dd[i] & right->dd[i]; // & Ty(1);
  //C delete right;
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] == zero || right->dd[i] == zero) dd[i]=zero;
  //C delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] != zero) dd[i] = right->dd[i];
  //C delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] == zero || right->dd[i] == zero) dd[i]=zero;
  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] != zero) dd[i] = right->dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::AndOpInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
// template<>
// DStructGDL* DStructGDL::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRUCT.");  
//  return this;
// }
template<>
Data_<SpDString>* Data_<SpDString>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRING.");  
//  return this;
// }
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype PTR.");  
//  return this;
// }
template<>
Data_<SpDObj>* Data_<SpDObj>::AndOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
// template<>
// Data_<SpDPtr>* Data_<SpDPtr>::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype PTR.");  
//  return this;
// }
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);

  Ty s = right->dd[0];
  // right->Scalar(s);

  // s &= Ty(1);
  dd &= s;
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] = dd[i] & s;

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

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  if( s == zero)
    dd = zero;
//     for( SizeT i=0; i < sEl; i++)
//       dd[i] = zero;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();

  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s);
  if( s == zero)
    dd = zero;
//     for( SizeT i=0; i < sEl; i++)
//       dd[i] = zero;
  else
    for( SizeT i=0; i < sEl; i++)
      if( dd[i] != zero) dd[i] = s;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();

  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
  if( s == zero)
    dd = zero;
//     for( SizeT i=0; i < sEl; i++)
//       dd[i] = zero;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  if( s == zero)
    dd = zero;
//     for( SizeT i=0; i < sEl; i++)
//       dd[i] = zero;
  else
    for( SizeT i=0; i < sEl; i++)
      if( dd[i] != zero) dd[i] = s;
  return this;
}
// invalid types
DStructGDL* DStructGDL::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::AndOpInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
// template<>
// DStructGDL* DStructGDL::AndOpInv( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRUCT.");  
//  return this;
// }
template<>
Data_<SpDString>* Data_<SpDString>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
// template<>
// Data_<SpDString>* Data_<SpDString>::AndOpInvS( BaseGDL* r)
// {
//  throw GDLException("Cannot apply operation to datatype STRING.");  
//  return this;
// }
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::AndOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = dd[i] | right->dd[i]; // | Ty(1);

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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] == zero) dd[i]=right->dd[i];
  //C delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    if( right->dd[i] != zero) dd[i] = right->dd[i];
  //C delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] == zero) dd[i]= right->dd[i];
  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    if( right->dd[i] != zero) dd[i] = right->dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::OrOpInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::OrOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
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

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  //s &= Ty(1);
  dd |= s;
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] = dd[i] | s;
  //C delete right;
  return this;
}
// different for floats
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOpInvS( BaseGDL* right)
{
  return OrOp( right);
}
// for floats
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
  if( s != zero)
    for( SizeT i=0; i < sEl; i++)
      if( dd[i] == zero) dd[i] = s;
  //C delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  if( s != zero)
    dd = s;
//     for( SizeT i=0; i < sEl; i++)
//       dd[i] = s;
  //C delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  if( s != zero)
    for( SizeT i=0; i < sEl; i++)
      if( dd[i] == zero) dd[i] = s;
  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  if( s != zero)
    dd = s;
//     for( SizeT i=0; i < sEl; i++)
//       dd[i] = s;
  else
    for( SizeT i=0; i < sEl; i++)
      if( dd[i] != zero) dd[i] = s;
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::OrOpInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::OrOpS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  if( right->Scalar(s))
    {
      if( s != Sp::zero)
	dd ^= s;
// 	for( SizeT i=0; i < sEl; i++)
// 	  dd[i] ^= s;
    }
  else
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] ^= right->dd[i];
    }
  //C delete right;
  return this;
}
// invalid types
template<>
Data_<SpDFloat>* Data_<SpDFloat>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype FLOAT.");  
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype DOUBLE.");  
  return this;
}
DStructGDL* DStructGDL::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::XorOp( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  for( SizeT i=0; i < sEl; i++)
    dd[i] += right->dd[i];
  //C delete right;
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = right->dd[i] + dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::AddInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Add( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::AddS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s);
  dd += s;
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] += s;
  //C delete right;
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

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s);
  for( SizeT i=0; i < sEl; i++)
    dd[i] = s + dd[i];
  //C delete right;
  return this;
}

// invalid types
DStructGDL* DStructGDL::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::AddInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::AddS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}

// Sub
// substraction: left=left-right
template<class Sp>
Data_<Sp>* Data_<Sp>::Sub( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  if( sEl == rEl)
    dd -= right->dd;
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] -= right->dd[i];
  //C delete right;
  return this;
}
// inverse substraction: left=right-left
template<class Sp>
Data_<Sp>* Data_<Sp>::SubInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  if( sEl == rEl)
    dd = right->dd - dd;
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = right->dd[i] - dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Sub( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::SubS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  dd -= s;
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] -= s;
  //C delete right;
  return this;
}
// inverse substraction: left=right-left
template<class Sp>
Data_<Sp>* Data_<Sp>::SubInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  dd = s - dd;
//   for( SizeT i=0; i < sEl; ++i)
//     dd[i] = s - dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
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
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] > right->dd[i]) dd[i]=right->dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::LtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::LtMarkS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);
  
  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] > s) dd[i]=s;
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::LtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] < right->dd[i]) dd[i]=right->dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::GtMark( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::GtMarkS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  for( SizeT i=0; i < sEl; i++)
    if( dd[i] < s) dd[i]=s;
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::GtMarkS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] *= right->dd[i];
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Mult( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::MultS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  dd *= s;
//   for( SizeT i=0; i < sEl; i++)
//     dd[i] *= s;
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::MultS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}

// Div
// division: left=left/right
template<class Sp>
Data_<Sp>* Data_<Sp>::Div( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] /= right->dd[i];
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false; // until zero operation is already done.
      for( SizeT i=0; i < sEl; i++)
	if( !zeroEncountered)
	  {
	    if( right->dd[i] == this->zero)
	      zeroEncountered = true;
	  }
	else
	  if( right->dd[i] != this->zero) dd[i] /= right->dd[i];
      //C delete right;
      return this;
    }
}
// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = right->dd[i] / dd[i];
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false;
      for( SizeT i=0; i < sEl; i++)
	if( !zeroEncountered)
	  {
	    if( dd[i] == this->zero)
	      {
		zeroEncountered = true;
		dd[ i] = right->dd[i];
	      }
	  }
	else
	  if( dd[i] != this->zero) 
	    dd[i] = right->dd[i] / dd[i]; 
	  else
	    dd[i] = right->dd[i];
      //C delete right;
      return this;
    }
}
// invalid types
DStructGDL* DStructGDL::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Div( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::DivS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      dd /= s;
//       for( SizeT i=0; i < sEl; i++)
// 	dd[i] /= s;
      //C delete right;
      return this;
    }
  return this;
}

// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      for( SizeT i=0; i < sEl; i++)
	dd[i] = s / dd[i];
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false;
      // right->Scalar(s); 
      for( SizeT i=0; i < sEl; i++)
	if( !zeroEncountered)
	  {
	    if( dd[i] == this->zero)
	      {
		zeroEncountered = true;
		dd[i] = s;
	      }
	  }
	else
	  if( dd[i] != this->zero) 
	    dd[i] = s / dd[i]; 
	  else 
	    dd[i] = s;
      //C delete right;
      return this;
    }
}
// invalid types
DStructGDL* DStructGDL::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::DivInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}

// Mod
// modulo division: left=left % right
template<class Sp>
Data_<Sp>* Data_<Sp>::Mod( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] %= right->dd[i];
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false; // until zero operation is already done.
      
      for( SizeT i=0; i < sEl; i++)
	if( !zeroEncountered)
	  {
	    if( right->dd[i] == this->zero)
	      {
		zeroEncountered = true;
		dd[i] = this->zero;
	      }
	  }
	else
	  if( right->dd[i] != this->zero) 
	    dd[i] %= right->dd[i];
	  else
	    dd[i] = this->zero;
      //C delete right;
      return this;
    }
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      for( SizeT i=0; i < sEl; ++i)
	dd[i] = right->dd[i] % dd[i];
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false;
      for( SizeT i=0; i < sEl; i++)
	if( !zeroEncountered)
	  {
	    if( dd[i] == this->zero)
	      {
		zeroEncountered = true;
		dd[ i] = this->zero;
	      }
	  }
	else
	  if( dd[i] != this->zero) 
	    dd[i] = right->dd[i] % dd[i]; 
	  else
	    dd[i] = this->zero;
      //C delete right;
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
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(dd[i],right->dd[i]);
  //C delete right;
  return this;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(right->dd[i],dd[i]);
  //C delete right;
  return this;
}
// double modulo division: left=left % right
inline DDouble Modulo( const DDouble& l, const DDouble& r)
{
  DDouble t=abs(l/r);
  if( l < 0.0) return t=(floor(t)-t)*abs(r);
  return (t-floor(t))*abs(r);
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::Mod( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(dd[i],right->dd[i]);
  //C delete right;
  return this;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  // ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  // assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(right->dd[i],dd[i]);
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Mod( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::ModS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      dd %= s;
//       for( SizeT i=0; i < sEl; i++)
// 	dd[i] %= s;
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false; // until zero operation is already done.
      
      // right->Scalar(s); 
      assert( s == this->zero);
      for( SizeT i=0; i < sEl; i++)
	dd[i] = 0;
      //C delete right;
      return this;
    }
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];

  if( sigsetjmp( sigFPEJmpBuf, 1) == 0)
    {
      // right->Scalar(s); 
      for( SizeT i=0; i < sEl; ++i)
	{
	  dd[i] = s % dd[i];
	}
      //C delete right;
      return this;
    }
  else
    {
      bool zeroEncountered = false;
      // right->Scalar(s); 
      for( SizeT i=0; i < sEl; i++)
	if( !zeroEncountered)
	  {
	    if( dd[i] == this->zero)
	      {
		zeroEncountered = true;
		dd[i] = this->zero;
	      }
	  }
	else
	  if( dd[i] != this->zero) 
	    dd[i] = s % dd[i]; 
	  else 
	    dd[i] = this->zero;
      //C delete right;
      return this;
    }    
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(dd[i],s);
  //C delete right;
  return this;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(s,dd[i]);
  //C delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s);
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(dd[i],s);
  //C delete right;
  return this;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  for( SizeT i=0; i < sEl; i++)
    dd[i] = Modulo(s,dd[i]);
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModS( BaseGDL* r)

{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype "+str+".");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::ModInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}

// Pow
// C++ defines pow only for floats and doubles
template <typename T> T pow( const T r, const T l)
{
  if( r == 0) return 0;
  if( r == 1) return 1;

  const int nBits = sizeof(T) * 8;

  T arr = r;
  T res = 1;
  T mask = 1;
  for( SizeT i=0; i<nBits; ++i, mask <<= 1)
    {
      if( l & mask) res *= arr;
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
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = pow( dd[i], right->dd[i]); // valarray
  //C delete right;
  return this;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  //  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  //  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  //      dd = pow( right->Resize(sEl), dd); // valarray
      
  for( SizeT i=0; i < sEl; i++)
    dd[i] = pow( right->dd[i], dd[i]);
  //C delete right;
  return this;
}
// floats power of value: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  //      dd = pow( dd, right->dd); // valarray
  //      slice sl( 0, sEl, 1);
  //      dd = pow( dd, right->dd[ sl]); // valarray
  if( rEl == sEl)
    dd = pow( dd, right->dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( dd[i], right->dd[i]);
  //C delete right;
  return this;
}
// floats inverse power of value: left=right ^ left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  if( rEl == sEl)
    dd = pow( right->dd, dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( right->dd[i], dd[i]);
  //C delete right;
  return this;
}
// doubles power of value: left=left ^ right
template<>
Data_<SpDDouble>* Data_<SpDDouble>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  //      dd = pow( dd, right->Resize(sEl)); // valarray
  if( rEl == sEl)
    dd = pow( dd, right->dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( dd[i], right->dd[i]);
  //C delete right;
  return this;
}
// doubles inverse power of value: left=right ^ left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  // dd = pow( right->Resize(sEl), dd); // valarray
  if( rEl == sEl)
    dd = pow( right->dd, dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( right->dd[i], dd[i]);
  //C delete right;
  return this;
}
// complex power of value: left=left ^ right
// complex is special here
template<>
Data_<SpDComplex>* Data_<SpDComplex>::Pow( BaseGDL* r)
{
  SizeT sEl = N_Elements();

  assert( sEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplex s;
	      if( Scalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		(*res)[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
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
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplex s;
	      if( Scalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		(*res)[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong sEl=N_Elements();
  //   if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  //      dd = pow( dd, right->dd); // valarray
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( dd[ i], (*right)[ i]);
#else
  //      dd = pow( dd, right->Resize(sEl)); // valarray
  if( r->N_Elements() == sEl)
    dd = pow( dd, right->dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( dd[i], right->dd[i]);
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
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( (*right)[ i], dd[i]);
#else
  //      right->dd.resize(sEl);
  //      dd = pow( right->Resize(sEl), dd); // valarray
  if( rEl == sEl)
    dd = pow( right->dd, dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( right->dd[i], dd[i]);
#endif
  //C delete right;
  return this;
}
// double complex power of value: left=left ^ right
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::Pow( BaseGDL* r)
{
  SizeT sEl = N_Elements();

  assert( sEl > 0);

  if( r->Type() == DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      assert( right->N_Elements() > 0);

      DDouble s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplexDbl s;
	      if( Scalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
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
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplexDbl s;
	      if( Scalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong sEl=N_Elements();
  //   if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  //      right->dd.resize(sEl);
  //      dd = pow( dd, right->dd); // valarray
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( dd[ i], (*right)[ i]);
#else
  //      dd = pow( dd, right->Resize(sEl)); // valarray
  if( r->N_Elements() == sEl)
    dd = pow( dd, right->dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( dd[i], right->dd[i]);
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
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( (*right)[ i], dd[i]);
#else
  //      right->dd.resize(sEl);
  //      dd = pow( right->Resize(sEl), dd); // valarray
  if( rEl == sEl)
    dd = pow( right->dd, dd); // valarray
  else
    for( SizeT i=0; i < sEl; i++)
      dd[i] = pow( right->dd[i], dd[i]);
#endif
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::Pow( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowInv( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<class Sp>
Data_<Sp>* Data_<Sp>::PowS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
  for( SizeT i=0; i < sEl; i++)
    dd[i] = pow( dd[i], s); 
  //C delete right;
  return this;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
  //      dd = pow( s, d); // valarray
  for( SizeT i=0; i < sEl; i++)
    dd[i] = pow( s, dd[i]);
  //C delete right;
  return this;
}
// floats power of value: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  dd = pow( dd, s); // valarray
  //C delete right;
  return this;
}
// floats inverse power of value: left=right ^ left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  dd = pow( s, dd); // valarray
  //C delete right;
  return this;
}
// doubles power of value: left=left ^ right
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  dd = pow( dd, s); // valarray
  //C delete right;
  return this;
}
// doubles inverse power of value: left=right ^ left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  Ty s = right->dd[0];
  // right->Scalar(s); 
  dd = pow( s, dd); // valarray
  //C delete right;
  return this;
}
// complex power of value: left=left ^ right
// complex is special here
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowS( BaseGDL* r)
{
  SizeT sEl = N_Elements();

  assert( sEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplex s;
	      if( Scalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		(*res)[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
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
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplex s;
	      if( Scalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		(*res)[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong sEl=N_Elements();
  //   if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( dd[ i], s);
#else
  dd = pow( dd, s); // valarray
#endif
  //C delete right;

  return this;
}
// complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( s, dd[ i]);
#else
  dd = pow( s, dd); // valarray
#endif
  //C delete right;
  return this;
}
// double complex power of value: left=left ^ right
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowS( BaseGDL* r)
{
  SizeT sEl = N_Elements();

  assert( sEl > 0);

  if( r->Type() == DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      assert( right->N_Elements() > 0);

      DDouble s;

      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplexDbl s;
	      if( Scalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
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
      if( right->Scalar(s)) 
	{
	  for( SizeT i=0; i<sEl; ++i)
	    dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return this;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplexDbl s;
	      if( Scalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      for( SizeT i=0; i<sEl; ++i)
		dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return this;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong sEl=N_Elements();
  //   if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( dd[ i], s);
#else
  dd = pow( dd, s); // valarray
#endif
  //C delete right;
  return this;
}
// double complex inverse power of value: left=right ^ left
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowInvS( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong sEl=N_Elements();
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s = right->dd[0];
  // right->Scalar(s); 
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
  for( SizeT i=0; i<sEl; ++i)
    dd[ i] = pow( s, dd[ i]);
#else
  dd = pow( s, dd); // valarray
#endif
  //C delete right;
  return this;
}
// invalid types
DStructGDL* DStructGDL::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
DStructGDL* DStructGDL::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRUCT.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDString>* Data_<SpDString>::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::PowInvS( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}

template<class Sp> Data_<Sp>* Data_<Sp>::AndOpNew( BaseGDL* r) {}
template<class Sp> Data_<Sp>* Data_<Sp>::OrOpNew( BaseGDL* r) {}    
template<class Sp> Data_<Sp>* Data_<Sp>::XorOpNew( BaseGDL* r) {}    
template<class Sp> Data_<Sp>* Data_<Sp>::AddNew( BaseGDL* r) {}      
template<class Sp> Data_<Sp>* Data_<Sp>::MultNew( BaseGDL* r) {}   
template<class Sp> Data_<Sp>* Data_<Sp>::DivNew( BaseGDL* r) {}      
template<class Sp> Data_<Sp>* Data_<Sp>::ModNew( BaseGDL* r) {}      
template<class Sp> Data_<Sp>* Data_<Sp>::PowNew( BaseGDL* r) {}     

template<class Sp>
Data_<Sp>* Data_<Sp>::SubNew( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  assert( rEl);
  assert( sEl);
  //  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  
  Data_* res = New( this->Dim(), BaseGDL::NOZERO);

  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	res->dd[i] = dd[i] - s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	res->dd[i] = dd[i] - right->dd[i];
    }
  //C delete right;
  return res;
}
// invalid types
template<>
Data_<SpDString>* Data_<SpDString>::SubNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype STRING.");  
  return this;
}
template<>
Data_<SpDPtr>* Data_<SpDPtr>::SubNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype PTR.");  
  return this;
}
template<>
Data_<SpDObj>* Data_<SpDObj>::SubNew( BaseGDL* r)
{
  throw GDLException("Cannot apply operation to datatype OBJECT.");  
  return this;
}
template<>
Data_<SpDComplex>* Data_<SpDComplex>::PowNew( BaseGDL* r)
{
  SizeT sEl = N_Elements();

  assert( sEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == FLOAT)
    {
      Data_<SpDFloat>* right=static_cast<Data_<SpDFloat>* >(r);

      DFloat s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->Scalar(s)) 
	{
	  DComplexGDL* res = new DComplexGDL( this->Dim(), 
					      BaseGDL::NOZERO);
	  for( SizeT i=0; i<sEl; ++i)
	    res->dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplex s;
	      if( Scalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      DComplexGDL* res = new DComplexGDL( this->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<sEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
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
      if( right->Scalar(s)) 
	{
	  DComplexGDL* res = new DComplexGDL( this->Dim(), 
					      BaseGDL::NOZERO);
	  for( SizeT i=0; i<sEl; ++i)
	    res->dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplex s;
	      if( Scalar(s)) 
		{
		  DComplexGDL* res = new DComplexGDL( right->Dim(), 
						      BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      DComplexGDL* res = new DComplexGDL( this->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<sEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexGDL* res = new DComplexGDL( right->Dim(), 
						  BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong sEl=N_Elements();
  //   if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      DComplexGDL* res = new DComplexGDL( this->Dim(), 
					  BaseGDL::NOZERO);
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
      for( SizeT i=0; i<sEl; ++i)
	res->dd[ i] = pow( dd[ i], s);
#else
      res->dd = pow( dd, s); // valarray
#endif
    }
  else 
    {
      DComplexGDL* res = new DComplexGDL( this->Dim(), 
					  BaseGDL::NOZERO);
      //      right->dd.resize(sEl);
      //      dd = pow( dd, right->dd); // valarray
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
      for( SizeT i=0; i<sEl; ++i)
	res->dd[ i] = pow( dd[ i], (*right)[ i]);
#else
      //      dd = pow( dd, right->Resize(sEl)); // valarray
      if( r->N_Elements() == sEl)
	res->dd = pow( dd, right->dd); // valarray
      else
	for( SizeT i=0; i < sEl; i++)
	  res->dd[i] = pow( dd[i], right->dd[i]);
#endif
    }
  //C delete right;

  return this;
}
template<>
Data_<SpDComplexDbl>* Data_<SpDComplexDbl>::PowNew( BaseGDL* r)
{
  SizeT sEl = N_Elements();

  assert( sEl > 0);
  assert( r->N_Elements() > 0);

  if( r->Type() == DOUBLE)
    {
      Data_<SpDDouble>* right=static_cast<Data_<SpDDouble>* >(r);

      DDouble s;
      // note: changes here have to be reflected in POWNCNode::Eval() (dnode.cpp)
      // (concerning when a new variable is created vs. using this)
      // (must also be consistent with ComplexDbl)
      if( right->Scalar(s)) 
	{
	  DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						    BaseGDL::NOZERO);
	  for( SizeT i=0; i<sEl; ++i)
	    res->dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplexDbl s;
	      if( Scalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<sEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
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
      if( right->Scalar(s)) 
	{
	  DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						    BaseGDL::NOZERO);
	  for( SizeT i=0; i<sEl; ++i)
	    res->dd[ i] = pow( dd[ i], s);
	  //C delete right;
	  return res;
	}
      else 
	{
	  SizeT rEl = right->N_Elements();
	  if( sEl < rEl)
	    {
	      DComplexDbl s;
	      if( Scalar(s)) 
		{
		  DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							    BaseGDL::NOZERO);
		  for( SizeT i=0; i<rEl; ++i)
		    res->dd[ i] = pow( s, (*right)[ i]);
		  //C delete right;
		  return res;
		}

	      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<sEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      return res;
	    }
	  else
	    {
	      DComplexDblGDL* res = new DComplexDblGDL( right->Dim(), 
							BaseGDL::NOZERO);
	      for( SizeT i=0; i<rEl; ++i)
		res->dd[ i] = pow( dd[ i], (*right)[ i]);
	      //C delete right;
	      //C delete this;
	      return res;
	    }
	}
    }

  Data_* right=static_cast<Data_*>(r);

  //   ULong rEl=right->N_Elements();
  //   ULong sEl=N_Elements();
  //   if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						BaseGDL::NOZERO);
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
      for( SizeT i=0; i<sEl; ++i)
	res->dd[ i] = pow( dd[ i], s);
#else
      res->dd = pow( dd, s); // valarray
#endif
    }
  else 
    {
      DComplexDblGDL* res = new DComplexDblGDL( this->Dim(), 
						BaseGDL::NOZERO);
      //      right->dd.resize(sEl);
      //      dd = pow( dd, right->dd); // valarray
#if (__GNUC__ == 3) && (__GNUC_MINOR__ <= 2)
      for( SizeT i=0; i<sEl; ++i)
	res->dd[ i] = pow( dd[ i], (*right)[ i]);
#else
      //      dd = pow( dd, right->Resize(sEl)); // valarray
      if( r->N_Elements() == sEl)
	res->dd = pow( dd, right->dd); // valarray
      else
	for( SizeT i=0; i < sEl; i++)
	  res->dd[i] = pow( dd[i], right->dd[i]);
#endif
    }
  //C delete right;

  return this;
}


//#include "instantiate_templates.hpp"

#endif
