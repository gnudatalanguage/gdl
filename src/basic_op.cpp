/***************************************************************************
                          basic_op.cpp  -  GDL operators
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
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

using namespace std;

// Not operation
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::NotOp()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = ~dd[i];
  return this;
}
// others
template<>
Data_<SpDFloat>* Data_<SpDFloat>::NotOp()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = (dd[i] == 0.0f)? 1.0f : 0.0f;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::NotOp()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
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

// UMinus unary minus
// for numbers
template<class Sp>
BaseGDL* Data_<Sp>::UMinus()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] = -dd[i];
  return this;
}
template<>
BaseGDL* Data_<SpDString>::UMinus()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
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

// increment decrement operators
// integers
template<class Sp>
void Data_<Sp>::Dec()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i]--;
}
template<class Sp>
void Data_<Sp>::Inc()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i]++;
}
// float
template<>
void Data_<SpDFloat>::Dec()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] -= 1.0;
}
template<>
void Data_<SpDFloat>::Inc()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] += 1.0;
}
// double
template<>
void Data_<SpDDouble>::Dec()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] -= 1.0;
}
template<>
void Data_<SpDDouble>::Inc()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] += 1.0;
}
// complex
template<>
void Data_<SpDComplex>::Dec()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] -= 1.0;
}
template<>
void Data_<SpDComplex>::Inc()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] += 1.0;
}
template<>
void Data_<SpDComplexDbl>::Dec()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] -= 1.0;
}
template<>
void Data_<SpDComplexDbl>::Inc()
{
  ULong sEl=N_Elements();
  if( !sEl) throw GDLException("Variable is undefined.");  
  for( SizeT i=0; i < sEl; i++)
    dd[i] += 1.0;
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


// AndOp
// Ands right to itself, deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::AndOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      // s &= Ty(1);
      for( SizeT i=0; i < sEl; i++)
	dd[i] = dd[i] & s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = dd[i] & right->dd[i]; // & Ty(1);
    }
  delete right;
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

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s == zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] = zero;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] == zero || right->dd[i] == zero) dd[i]=zero;
    }
  delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::AndOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s == zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] = zero;
      else
	for( SizeT i=0; i < sEl; i++)
	  if( dd[i] != zero) dd[i] = s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] != zero) dd[i] = right->dd[i];
    }
  delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s == zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] = zero;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] == zero || right->dd[i] == zero) dd[i]=zero;
    }
  delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::AndOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s == zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] = zero;
      else
	for( SizeT i=0; i < sEl; i++)
	  if( dd[i] != zero) dd[i] = s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] != zero) dd[i] = right->dd[i];
    }
  delete right;
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


// OrOp
// Ors right to itself, deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::OrOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      //s &= Ty(1);
      for( SizeT i=0; i < sEl; i++)
	dd[i] = dd[i] | s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = dd[i] | right->dd[i]; // | Ty(1);
    }
  delete right;
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

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s != zero)
	for( SizeT i=0; i < sEl; i++)
	  if( dd[i] == zero) dd[i] = s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] == zero) dd[i]=right->dd[i];
    }
  delete right;
  return this;
}
template<>
Data_<SpDFloat>* Data_<SpDFloat>::OrOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s != zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] = s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( right->dd[i] != zero) dd[i] = right->dd[i];
    }
  delete right;
  return this;
}
// for doubles
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s != zero)
	for( SizeT i=0; i < sEl; i++)
	  if( dd[i] == zero) dd[i] = s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] == zero) dd[i]= right->dd[i];
    }
  delete right;
  return this;
}
template<>
Data_<SpDDouble>* Data_<SpDDouble>::OrOpInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s != zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] = s;
      else
	for( SizeT i=0; i < sEl; i++)
	  if( dd[i] != zero) dd[i] = s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( right->dd[i] != zero) dd[i] = right->dd[i];
    }
  delete right;
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

// XorOp
// Xors right to itself, deletes right
// right must always have more or same number of elements
// for integers
template<class Sp>
Data_<Sp>* Data_<Sp>::XorOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      if( s != Sp::zero)
	for( SizeT i=0; i < sEl; i++)
	  dd[i] ^= s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] ^= right->dd[i];
    }
  delete right;
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

// EqOp
// returns *this eq *r, deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::EqOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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
  delete right;
  delete this;
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

// NeOp
// returns *this ne *r, deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::NeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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
  delete right;
  delete this;
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

// LeOp
// returns *this le *r, deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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
  delete right;
  delete this;
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
// returns *this lt *r, deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LtOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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
  delete right;
  delete this;
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
// returns *this ge *r, deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::GeOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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
  delete right;
  delete this;
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
// returns *this gt *r, deletes itself and right
template<class Sp>
Data_<SpDByte>* Data_<Sp>::GtOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

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
  delete right;
  delete this;
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


// Add
// Adds right to itself, deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::Add( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] += s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] += right->dd[i];
    }
  delete right;
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

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = s + dd[i];
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = right->dd[i] + dd[i];
    }
  delete right;
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

// Sub
// substraction: left=left-right
template<class Sp>
Data_<Sp>* Data_<Sp>::Sub( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] -= s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] -= right->dd[i];
    }
  delete right;
  return this;
}
// inverse substraction: left=right-left
template<class Sp>
Data_<Sp>* Data_<Sp>::SubInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = s - dd[i];
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = right->dd[i] - dd[i];
    }
  delete right;
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

// LtMark

// LtMarks right to itself, deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::LtMark( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] > s) dd[i]=s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] > right->dd[i]) dd[i]=right->dd[i];
    }
  delete right;
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

// GtMark
// GtMarks right to itself, deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::GtMark( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 

    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] < s) dd[i]=s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	if( dd[i] < right->dd[i]) dd[i]=right->dd[i];
    }
  delete right;
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

// Mult
// Mults right to itself, deletes right
// right must always have more or same number of elements
template<class Sp>
Data_<Sp>* Data_<Sp>::Mult( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] *= s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] *= right->dd[i];
    }
  delete right;
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

// Div
// division: left=left/right
template<class Sp>
Data_<Sp>* Data_<Sp>::Div( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] /= s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] /= right->dd[i];
    }
  delete right;
  return this;
}
// inverse division: left=right/left
template<class Sp>
Data_<Sp>* Data_<Sp>::DivInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = s / dd[i];
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = right->dd[i] / dd[i];
    }
  delete right;
  return this;
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

// Mod
// modulo division: left=left % right
template<class Sp>
Data_<Sp>* Data_<Sp>::Mod( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] %= s;
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] %= right->dd[i];
    }
  delete right;
  return this;
}
// inverse modulo division: left=right % left
template<class Sp>
Data_<Sp>* Data_<Sp>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = s % dd[i];
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = right->dd[i] % dd[i];
    }
  delete right;
  return this;
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

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(dd[i],s);
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(dd[i],right->dd[i]);
    }
  delete right;
  return this;
}
// float  inverse modulo division: left=right % left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(s,dd[i]);
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(right->dd[i],dd[i]);
    }
  delete right;
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

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(dd[i],s);
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(dd[i],right->dd[i]);
    }
  delete right;
  return this;
}
// double inverse modulo division: left=right % left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::ModInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(s,dd[i]);
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = Modulo(right->dd[i],dd[i]);
    }
  delete right;
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

// Pow
// C++ defines pow only for floats and doubles
inline DInt pow( const DByte& l, const DByte& r)
{ 
  return static_cast<DInt>(pow( static_cast<float>(l),
				static_cast<float>(r)));
}
inline DInt pow( const DInt& l, const DInt& r)
{ 
  return static_cast<DInt>(pow( static_cast<float>(l),
				static_cast<float>(r)));
}
inline DUInt pow( const DUInt& l, const DUInt& r)
{ 
  return static_cast<DUInt>(pow( static_cast<float>(l),
				 static_cast<float>(r)));
}
inline DLong pow( const DLong& l, const DLong& r)
{ 
  return static_cast<DLong>(pow( static_cast<double>(l),
				 static_cast<double>(r)));
}
inline DULong pow( const DULong& l, const DULong& r)
{ 
  return static_cast<DULong>(pow( static_cast<double>(l),
				  static_cast<double>(r)));
}
inline DLong64 pow( const DLong64& l, const DLong64& r)
{ 
  return static_cast<DLong64>(pow( static_cast<double>(l),
				   static_cast<double>(r)));
}
inline DULong64 pow( const DULong64& l, const DULong64& r)
{ 
  return static_cast<DULong64>(pow( static_cast<double>(l),
				    static_cast<double>(r)));
}
// power of value: left=left ^ right
template<class Sp>
Data_<Sp>* Data_<Sp>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = pow( dd[i], s); 
    }
  else 
    {
      for( SizeT i=0; i < sEl; i++)
	dd[i] = pow( dd[i], right->dd[i]); // valarray
    }
  delete right;
  return this;
}
// inverse power of value: left=right ^ left
template<class Sp>
Data_<Sp>* Data_<Sp>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      //      dd = pow( s, d); // valarray
      for( SizeT i=0; i < sEl; i++)
	dd[i] = pow( s, dd[i]);
    }
  else 
    {
      //      right->dd.resize(sEl);
      //      dd = pow( right->Resize(sEl), dd); // valarray
      
      for( SizeT i=0; i < sEl; i++)
 	dd[i] = pow( right->dd[i], dd[i]);
    }
  delete right;
  return this;
}
// floats power of value: left=left ^ right
template<>
Data_<SpDFloat>* Data_<SpDFloat>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      dd = pow( dd, s); // valarray
    }
  else 
    {
      //      right->dd.resize(sEl);
      //      dd = pow( dd, right->dd); // valarray
      dd = pow( dd, right->Resize(sEl)); // valarray
    }
  delete right;
  return this;
}
// floats inverse power of value: left=right ^ left
template<>
Data_<SpDFloat>* Data_<SpDFloat>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      dd = pow( s, dd); // valarray
    }
  else 
    {
      //      right->dd.resize(sEl);
      dd = pow( right->Resize(sEl), dd); // valarray
    }
  delete right;
  return this;
}
// doubles power of value: left=left ^ right
template<>
Data_<SpDDouble>* Data_<SpDDouble>::Pow( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      dd = pow( dd, s); // valarray
    }
  else 
    {
      //      right->dd.resize(sEl);
      dd = pow( dd, right->Resize(sEl)); // valarray
    }
  delete right;
  return this;
}
// doubles inverse power of value: left=right ^ left
template<>
Data_<SpDDouble>* Data_<SpDDouble>::PowInv( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  
  Ty s;
  if( right->Scalar(s)) 
    {
      dd = pow( s, dd); // valarray
    }
  else 
    {
      //      right->dd.resize(sEl);
      dd = pow( right->Resize(sEl), dd); // valarray
    }
  delete right;
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

// MatrixOp
// returns *this # *r, deletes itself and right
template<class Sp>
Data_<Sp>* Data_<Sp>::MatrixOp( BaseGDL* r)
{
  Data_* right=static_cast<Data_*>(r);

  ULong rEl=right->N_Elements();
  ULong sEl=N_Elements();
  if( !rEl || !sEl) throw GDLException("Variable is undefined.");  

  Data_* res;

  if( this->dim.Rank() <= 1 && right->dim.Rank() <= 1)
    {
      // use transposed r if rank of both is <= 1
      // result dim
      SizeT nCol=this->dim[0];
      SizeT nRow=right->dim[0]; // transpose

      if( nCol == 0) nCol=1;
      if( nRow == 0) nRow=1;

      SizeT dimArr[2]={nCol,nRow};

      res=New(dimension(dimArr,2)); // zero values
      res->Purge();

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

      res = New( dimension( nCol, nRow), BaseGDL::NOZERO);
      res->Purge();
     
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

  delete right;
  delete this;
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

// logical negation
// integers, also ptr and object
template<class Sp>
Data_<SpDByte>* Data_<Sp>::LogNeg()
{
  SizeT nEl = dd.size();
  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
	(*res)[i] = (dd[i] == 0)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDFloat>::LogNeg()
{
  SizeT nEl = dd.size();
  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( this->dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
	(*res)[i] = (dd[i] == 0.0f)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDDouble>::LogNeg()
{
  SizeT nEl = dd.size();
  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
	(*res)[i] = (dd[i] == 0.0)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDString>::LogNeg()
{
  SizeT nEl = dd.size();
  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
	(*res)[i] = (dd[i] == "")? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDComplex>::LogNeg()
{
  SizeT nEl = dd.size();
  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
	(*res)[i] = (dd[i].real() == 0.0 && dd[i].imag() == 0.0)? 1 : 0;
  return res;
}
template<>
Data_<SpDByte>* Data_<SpDComplexDbl>::LogNeg()
{
  SizeT nEl = dd.size();
  if( nEl == 0) throw GDLException("Variable is undefined.");  
  
  DByteGDL* res = new Data_<SpDByte>( dim, BaseGDL::NOZERO);
  for( SizeT i=0; i < nEl; i++)
	(*res)[i] = (dd[i].real() == 0.0 && dd[i].imag() == 0.0)? 1 : 0;
  return res;
}

//#include "instantiate_templates.hpp"

#endif
