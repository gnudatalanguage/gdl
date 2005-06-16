/***************************************************************************
                          topython.cpp  -  ToPython functions
                             -------------------
    begin                : Apr 19 2004
    copyright            : (C) 2004 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
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
#ifdef INCLUDE_TOPYTHON_CPP

//#include <Python.h>
//#include <numarray/libnumarray.h>
//#include <numarray/numarray.h>

//#include "datatypes.hpp"
//#include "objects.hpp"

using namespace std;

// from numarray.h
// struct s_PyArrayObject {
// 	/* Numeric compatible stuff */

// 	PyObject_HEAD
// 	char *data;
// 	int nd;
// 	maybelong dimensions[MAXDIM];
// 	maybelong strides[MAXDIM];
// 	PyObject *base;
// 	PyArray_Descr *descr;
// 	int flags;

// 	/* numarray extras */

// 	PyObject *_data;       /* object must meet buffer API */
// 	PyObject *_shadows;    /* ill-behaved original array. */
// 	int      nstrides;     /* elements in strides array */
// 	long     byteoffset;   /* offset into buffer where array data begins */
// 	long     bytestride;   /* basic seperation of elements in bytes */
// 	long     itemsize;     /* length of 1 element in bytes */

// 	char      byteorder;   /* NUM_BIG_ENDIAN, NUM_LITTLE_ENDIAN */

// 	char      _aligned;    /* test override flag */      
// 	char      _contiguous; /* test override flag */

// 	Complex64      temp;   /* temporary for gettitem/setitem MACROS */
// 	char *         wptr;   /* working pointer for getitem/setitem MACROS */
// };       

const NumarrayType pyType[] = {
  tAny,     //UNDEF***
  tUInt8,   //BYTE
  tInt16,   //INT
  tInt32,   //LONG,	
  tFloat32, //FLOAT,	
  tFloat64, //DOUBLE,	
  tComplex32,  //COMPLEX,	
  tAny,        //STRING***	
  tAny,        //STRUCT***	
  tComplex64,  //COMPLEXDBL,	
  tAny,        //PTR***		
  tAny,        //OBJECT***
  tUInt16,     //UINT*
  tUInt32,     //ULONG*
  tInt64,      //LONG64*
  tUInt64      //ULONG64*
};	

template < typename Sp>
PyObject* Data_<Sp>::ToPython()
{
  SizeT nEl = dd.size();
  
  if( nEl == 1)
    { // return simple python object
      return ToPythonScalar();
    }

  const NumarrayType item_type = pyType[ Sp::t];
  if( item_type == tAny)
    throw GDLException("Cannot convert "+this->TypeStr()+" array to python.");

  int n_dim = this->Rank();
  maybelong dimArr[ MAXRANK];
  for( int i=0; i<n_dim; ++i) dimArr[i]=this->dim[i];

  return reinterpret_cast< PyObject*>
    (NA_vNewArray( DataAddr(), item_type, n_dim, dimArr));
}

template < typename Sp>
PyObject* Data_<Sp>::ToPythonScalar()
{
  throw GDLException("Cannot convert scalar "+this->TypeStr()+" to python.");
  return NULL;
}
template<>
PyObject* Data_<SpDByte>::ToPythonScalar()
{
  return Py_BuildValue("B", dd[0]);
}
template<>
PyObject* Data_<SpDInt>::ToPythonScalar()
{
  return Py_BuildValue("h", dd[0]);
}
template<>
PyObject* Data_<SpDUInt>::ToPythonScalar()
{
  return Py_BuildValue("H", dd[0]);
}
template<>
PyObject* Data_<SpDLong>::ToPythonScalar()
{
  return Py_BuildValue("i", dd[0]);
}
template<>
PyObject* Data_<SpDULong>::ToPythonScalar()
{
  return Py_BuildValue("I", dd[0]);
}
// template<>
// PyObject* Data_<SpDLong64>::ToPythonScalar()
// {
//   return Py_BuildValue("L", dd[0]);
// }
// template<>
// PyObject* Data_<SpDULong64>::ToPythonScalar()
// {
//   return Py_BuildValue("K", dd[0]);
// }
template<>
PyObject* Data_<SpDFloat>::ToPythonScalar()
{
  return Py_BuildValue("f", dd[0]);
}
template<>
PyObject* Data_<SpDDouble>::ToPythonScalar()
{
  return Py_BuildValue("d", dd[0]);
}
template<>
PyObject* Data_<SpDComplex>::ToPythonScalar()
{
  Py_complex c;
  c.real = dd[0].real();
  c.imag = dd[0].imag();
  return Py_BuildValue("D", c);
}
template<>
PyObject* Data_<SpDComplexDbl>::ToPythonScalar()
{
  Py_complex c;
  c.real = dd[0].real();
  c.imag = dd[0].imag();
  return Py_BuildValue("D", c);
}
template<>
PyObject* Data_<SpDString>::ToPythonScalar()
{
  return Py_BuildValue("s", dd[0].c_str());
}

//#include "instantiate_templates.hpp"

#endif
