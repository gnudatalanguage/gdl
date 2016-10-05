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

using namespace std;

const int pyType[] = {
  NPY_NOTYPE,     //GDL_UNDEF***
  NPY_UINT8,   //GDL_BYTE
  NPY_INT16,   //GDL_INT
  NPY_INT32,   //GDL_LONG,	
  NPY_FLOAT32, //GDL_FLOAT,	
  NPY_FLOAT64, //GDL_DOUBLE,	
  NPY_COMPLEX64,  //GDL_COMPLEX,	
  NPY_NOTYPE,        //GDL_STRING***	
  NPY_NOTYPE,        //GDL_STRUCT***	
  NPY_COMPLEX128,  //GDL_COMPLEXDBL,	
  NPY_NOTYPE,        //GDL_PTR***		
  NPY_NOTYPE,        //GDL_OBJ***
  NPY_UINT32,     //GDL_UINT*
  NPY_UINT32,     //GDL_ULONG*
  NPY_INT64,      //GDL_LONG64*
  NPY_UINT64      //GDL_ULONG64*
};	

template < typename Sp>
PyObject* Data_<Sp>::ToPython()
{
  SizeT nEl = dd.size();
  
  if( nEl == 1)
    { // return simple python object
      return ToPythonScalar();
    }

  const int item_type = pyType[ Sp::t];
  if( item_type == NPY_NOTYPE)
    throw GDLException("Cannot convert "+this->TypeStr()+" array to python.");

  int n_dim = this->Rank();
  npy_intp dimArr[MAXRANK];
  for( int i=0; i<n_dim; ++i) dimArr[i]=this->dim[i];

  // SA: this does not copy the data (see tracker item no. 3148396)
  // return //reinterpret_cast< PyObject*>
  //  (PyArray_SimpleNewFromData( n_dim, dimArr, item_type, DataAddr()));

  PyObject* ret = PyArray_SimpleNew(n_dim, dimArr, item_type);
  if (!PyArray_CHKFLAGS(ret, NPY_C_CONTIGUOUS))
  {
    // TODO: free the memory:  PyArray_Free(PyObject* op, void* ptr) ?
    throw GDLException("Failed to convert array to python.");
  }
  memcpy(PyArray_DATA(ret), DataAddr(), this->N_Elements() * Data_<Sp>::Sizeof());
  return ret;
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
  return Py_BuildValue("B", (*this)[0]);
}
template<>
PyObject* Data_<SpDInt>::ToPythonScalar()
{
  return Py_BuildValue("h", (*this)[0]);
}
template<>
PyObject* Data_<SpDUInt>::ToPythonScalar()
{
  return Py_BuildValue("H", (*this)[0]);
}
template<>
PyObject* Data_<SpDLong>::ToPythonScalar()
{
  return Py_BuildValue("i", (*this)[0]);
}
template<>
PyObject* Data_<SpDULong>::ToPythonScalar()
{
  return Py_BuildValue("I", (*this)[0]);
}
// template<>
// PyObject* Data_<SpDLong64>::ToPythonScalar()
// {
//   return Py_BuildValue("L", (*this)[0]);
// }
// template<>
// PyObject* Data_<SpDULong64>::ToPythonScalar()
// {
//   return Py_BuildValue("K", (*this)[0]);
// }
template<>
PyObject* Data_<SpDFloat>::ToPythonScalar()
{
  return Py_BuildValue("f", (*this)[0]);
}
template<>
PyObject* Data_<SpDDouble>::ToPythonScalar()
{
  return Py_BuildValue("d", (*this)[0]);
}
template<>
PyObject* Data_<SpDComplex>::ToPythonScalar()
{
  Py_complex c;
  c.real = (*this)[0].real();
  c.imag = (*this)[0].imag();
  return Py_BuildValue("D", c);
}
template<>
PyObject* Data_<SpDComplexDbl>::ToPythonScalar()
{
  Py_complex c;
  c.real = (*this)[0].real();
  c.imag = (*this)[0].imag();
  return Py_BuildValue("D", c);
}
template<>
PyObject* Data_<SpDString>::ToPythonScalar()
{
  return Py_BuildValue("s", (*this)[0].c_str());
}

//#include "instantiate_templates.hpp"

#endif
