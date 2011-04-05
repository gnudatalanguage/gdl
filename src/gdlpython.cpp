/***************************************************************************
                          gdlpython.cpp  -  python embedded in GDL
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#ifdef INCLUDE_GDLPYTHON_CPP

//#include "includefirst.hpp"

// this part contains variable conversion stuff
// used by both GDL embedded in python and python embedded in GDL
//#if defined(USE_PYTHON) || defined(PYTHON_MODULE)

//#include <deque>
//#include <iterator>

//#include "datatypes.hpp"
//#include "envt.hpp"
//#include "objects.hpp"

using namespace std;

void PythonInit()
{
  if( Py_IsInitialized()) return;
  Py_Initialize(); // signal handlers?
  
  static int argc = 1;
  static char* arg0 = (char*)"./py/python.exe";
  static char* argv[] = {arg0};
  PySys_SetArgv(argc, argv);

  // http://docs.scipy.org/doc/numpy/reference/c-api.array.html#miscellaneous
  import_array();
}

void PythonEnd()
{
  // was not possible with numarray (numarray cannot be restarted)
  // but now we use Numpy??? (TODO/FIXME)

  //if( !Py_IsInitialized()) return;
  //Py_Finalize();
}

template< typename T>
T* NewFromPyArrayObject( const dimension& dim, PyArrayObject *array)
{
  T* res = new T( dim, BaseGDL::NOZERO);
  SizeT nEl = res->N_Elements();
  typename T::Ty* dPtr = 
    reinterpret_cast<typename T::Ty*>( PyArray_DATA(array));
  for( SizeT i=0; i<nEl; ++i) (*res)[i] = dPtr[i];
  Py_DECREF(array); // must be decremented
  return res;
}

BaseGDL* FromPython( PyObject* pyObj)
{
  if( !PyArray_Check( pyObj))
    {
      if( PyString_Check( pyObj))
	{
	  return new DStringGDL( PyString_AsString( pyObj));
	}
      if( PyInt_Check( pyObj))
	{
	  return new DLongGDL( PyInt_AsLong( pyObj));
	}
      if( PyLong_Check( pyObj))
	{
	  return new DLongGDL( PyLong_AsLong( pyObj));
	}
      if( PyFloat_Check( pyObj))
	{
	  return new DDoubleGDL( PyFloat_AsDouble( pyObj));
	}
      if( PyComplex_Check( pyObj))
	{
	  DDouble re,im;
	  re = PyComplex_RealAsDouble( pyObj);
	  im = PyComplex_ImagAsDouble( pyObj);
	  return new DComplexDblGDL( DComplexDbl( re, im));
	}
      throw GDLException( "Cannot convert python scalar.") ;
    }

  // make array contiguous
  PyArrayObject* array = PyArray_GETCONTIGUOUS( reinterpret_cast< PyArrayObject *>( pyObj));

  if( array == NULL)
    throw GDLException( "Error getting python array.") ;
  
  int nDim = array->nd;
  SizeT dimArr[ MAXRANK];
  if( nDim > MAXRANK)
    {
    Warning( "Python array has more than "+MAXRANK_STR+
	     " dimensions. Extending last one."); 
    SizeT lastDim = array->dimensions[ MAXRANK-1];
    for( SizeT i=MAXRANK; i<nDim; ++i) lastDim *= array->dimensions[ i];
    for( SizeT i=0; i<MAXRANK-1; ++i)
      dimArr[ i] = array->dimensions[ i];
    dimArr[ MAXRANK-1] = lastDim;
    nDim = MAXRANK;
    }
  else
    {
      for( SizeT i=0; i<nDim; ++i)
	dimArr[ i] = array->dimensions[ i];
    }
  dimension dim( dimArr, nDim);

  switch( array->descr->type_num) 
    {
    case NPY_UINT8:   //BYTE
      return NewFromPyArrayObject< DByteGDL>( dim, array);
    case NPY_INT16:   //INT
      return NewFromPyArrayObject< DIntGDL>( dim, array);
    case NPY_INT32:     //LONG	
      return NewFromPyArrayObject< DLongGDL>( dim, array);
    case NPY_FLOAT32:   //FLOAT	
      return NewFromPyArrayObject< DFloatGDL>( dim, array);
    case NPY_FLOAT64:  //DOUBLE	
      return NewFromPyArrayObject< DDoubleGDL>( dim, array);
    case NPY_COMPLEX64:  //COMPLEX	
      return NewFromPyArrayObject< DComplexGDL>( dim, array);
    case NPY_COMPLEX128: //COMPLEXDBL	
      return NewFromPyArrayObject< DComplexDblGDL>( dim, array);
    case NPY_UINT16:         //UINT*
      return NewFromPyArrayObject< DUIntGDL>( dim, array);
    case NPY_UINT32:         //ULONG*
      return NewFromPyArrayObject< DULongGDL>( dim, array);
    default:
      Py_DECREF(array); // must be decremented
      throw GDLException( "FromPython: Unknown array type.") ;
    }
  
  return NULL; // compiler shut-up
}


namespace lib {

  using namespace std;

  BaseGDL* gdlpython( EnvT* e, int kIx)
  {
// you can compile GDL as a python module without supporting
// python *within* GDL
#ifdef USE_PYTHON

    PythonInit();

    SizeT nParam = e->NParam();

    static int argvIx =  e->KeywordIx( "ARGV");
    BaseGDL* argv1 = e->GetKW( argvIx);
    if( argv1 != NULL)
      {
	DStringGDL* argvS = dynamic_cast<DStringGDL*>( argv1);
	if( argvS == NULL)
	  e->Throw( "ARGV keyword must be of type STRING.");
	
	int argc = argvS->N_Elements();
	char** argv = new char*[ argc];

	// pyhton copies the value -> threats it as const
	for( int i=0; i<argc; ++i)
	  argv[i] = const_cast<char*>((*argvS)[ i].c_str()); 

	PySys_SetArgv(argc, argv);
	delete[] argv;
      }

    if( nParam < 2 && kIx != -1)
      e->Throw( "Function must have at least 2 parameters.");
    
    if( nParam == 0)
      return NULL; // ok, just keywords

    DString module;
    e->AssureScalarPar<DStringGDL>( 0, module);
    
//     PyObject* pName = PyString_FromString( module.c_str());
//     // Error checking of pName left out
//     PyObject* pModule = PyImport_Import(pName);
//     Py_DECREF(pName);
    PyObject* pModule = 
      PyImport_ImportModule(const_cast<char*>(module.c_str()));
    if (pModule == NULL)
      {
	PyErr_Print();
	e->Throw( "Failed to load module: "+module);
      }

    if( nParam == 1)
      {
	Py_DECREF(pModule);
	return NULL; // ok, only load module
      }
    
    DString function;
    e->AssureScalarPar<DStringGDL>( 1, function);

    PyObject* pDict = PyModule_GetDict(pModule);
    /* pDict is a borrowed reference */

    PyObject* pFunc = PyDict_GetItemString(pDict, function.c_str());
    /* pFunc: Borrowed reference */

    if( !(pFunc && PyCallable_Check(pFunc)))
      {
	if (PyErr_Occurred()) PyErr_Print();
	e->Throw( "Cannot find function: "+function);
      }

    PyObject* pArgs = PyTuple_New( nParam-2);
    for( SizeT i = 2; i<nParam; ++i) 
      {
	BaseGDL* actPar = e->GetParDefined( i);
	PyObject* pValue = actPar->ToPython();
	if (!pValue) 
	  {
	    Py_DECREF(pArgs);
	    Py_DECREF(pModule);
	    
	    e->Throw( "Cannot convert value: "+
		      e->GetParString( i));
	  }
	/* pValue reference stolen here: */
	PyTuple_SetItem(pArgs, i-2, pValue);
      }

    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);
    Py_DECREF(pModule);
    // pDict and pFunc are borrowed and must not be Py_DECREF-ed
    if( pResult == NULL) 
      {
	PyErr_Print();
	e->Throw( "Call failed: "+module+"."+function);
      }

    if( kIx == -1) // called as procedures
      {
	Py_DECREF(pResult);
	return NULL;
      }

    // as function
    BaseGDL* res;
    if( pResult == Py_None)
      {
	Py_DECREF(pResult);
	BaseGDL* defRet = e->GetKW( kIx);
	if( defRet == NULL)
	  e->Throw( "Function returned 'None' "
		    "and DEFAULTRETURN not defined.");
	res = defRet->Dup();
      }
    else
      {
	try {
	  res = FromPython( pResult);
	  Py_DECREF(pResult);
	}
	catch(...)
	  {
	    Py_DECREF(pResult);
	    throw;
	  }
      }
    
    return res;
#endif // #ifdef USE_PYTHON
    e->Throw("GDL was compiled without support for Python");
  }

  // GDL PYTHON procedure
  // PYTHON,module,function[,args]
  void gdlpython_pro( EnvT* e)
  {
    gdlpython( e, -1);
  }

  // GDL PYTHON function
  BaseGDL* gdlpython_fun( EnvT* e)
  {
    static int kIx = e->KeywordIx( "DEFAULTRETURNVALUE");
    return gdlpython( e, kIx);
  }


} // namespace



//#endif // #if defined(USE_PYTHON) || defined(PYTHON_MODULE)

#endif // #ifdef INCLUDE_GDLPYTHON_CPP
