/***************************************************************************
                          pythongdl.cpp  -  GDL embedded in python
                                            see gdlpython for vice versa
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

#include "includefirst.hpp"

// if build as a python module
#ifdef PYTHON_MODULE

// already included from basegdl.hpp
#include <Python.h>

#include "datatypes.hpp"
#include "envt.hpp"
#include "sigfpehandler.hpp"
#include "terminfo.hpp"
#include "dinterpreter.hpp"

#include <numarray/libnumarray.h>
//#include <numarray/numarray.h>

// everything is executed within this interpreter
// initialized in the initGDL function
DInterpreter* interpreter;
PyObject*     gdlError;

// GDL is a C++ program
extern "C" {

  // Execute a GDL procedure
  PyObject *GDL_script(PyObject *self, PyObject *argTuple, PyObject *kwDict)
  {}

  // Execute a GDL procedure
  PyObject *GDL_function(PyObject *self, PyObject *argTuple, PyObject *kwDict)
  {}

  // Execute a GDL procedure
  PyObject *GDL_pro(PyObject *self, PyObject *argTuple, PyObject *kwDict)
  {
    sighandler_t oldControlCHandler = signal(SIGINT,ControlCHandler);
    sighandler_t oldSigFPEHandler   = signal(SIGFPE,SigFPEHandler);

    PyObject *retVal = NULL; // init to error indicator

    // avoid crossing intialization error
    {

    if( argTuple == NULL)
      {
	// ERROR we need at least the procedure name
	goto ret;
      }

    int nArg = PyTuple_Size( argTuple);
    if( nArg == 0)
      {
	// ERROR we need at least the procedure name
	goto ret;
      }
    
    // reference count???
    PyObject* proPy = PyTuple_GetItem(argTuple, 0);

    BaseGDL* proGDL = FromPython( proPy);
    auto_ptr< BaseGDL> proGDL_guard( proGDL);

    if( proGDL->Type() != STRING || proGDL->N_Elements() != 0)
      {
	// ERROR pro argument must be a scalar string
	goto ret;
      }

    DString pro = (*(static_cast< DStringGDL*>( proGDL)))[ 0];

    DSub*    sub  = NULL;
    bool     libPro = false;
    // search for procedure pro
    // first search library procedures
    int proIx = LibProIx( pro);
    if( proIx != -1)
      {
	// PCALL_LIB
	sub = libProList[ proIx];
	libPro = true;
	//newEnv=new EnvT( interpreter, dummyNode, libProList[pl->proIx]);
      }
    else
      {
	// PCALL - user defined procedures
	proIx = ProIx( pro);
	if( proIx == -1)
	  {
	    /*bool found=*/ interpreter->SearchCompilePro( pro);
	  
	    proIx = ProIx( pro);
	    if( proIx == -1)
	      {
		// ERROR procedure not found
		goto ret;
	      }
	  }
	
	sub = proList[ proIx];
	//newEnv = new EnvT( interpreter, dummyNode, proList[p->proIx]);
      }

    int     nKey   = sub->NKey();
    int     nPar   = sub->NPar();

    // check args and keywords
    if( (nArg-1) > nPar)
      {
	// ERROR too many args
	goto ret;
      }

    int nKW = 0;
    if( kwDict != NULL)
      nKW =  PyDict_Size( kwDict);

    if( nKW > nKey) 
      {
	// ERROR too many keywords
	goto ret;
      }

    // check keywords
    // PyDictObject kwDict;
    vector<int> gdlKWIx;
    PyObject *key, *value;
    int dictPos = 0;
    while( PyDict_Next( kwDict, &dictPos, &key, &value)) 
      {
	// check key name and value
	int keyIsString =  PyString_Check( key);
	if( !keyIsString)
	  {
	    // ERROR too many args
	    goto ret;
	  }
	const char* keyChar = PyString_AsString( key);
	int kwIx = sub->FindKey( StrUpCase( keyChar));
	if( kwIx == -1) 
	  {
	    // ERROR keyword not found
	    goto ret;
	  }
	
	gdlKWIx.push_back( kwIx);
      }

    // build the environment
    RefDNode dummyNode; 
    EnvT  e( interpreter, dummyNode, sub);

    vector<BaseGDL*> parRef;
    for( SizeT p=0; p<nArg; ++p)
      {
	BaseGDL* pP = FromPython( PyTuple_GetItem(argTuple, p+1)); // throws
	parRef.push_back( pP);
	e.SetNextPar( &parRef.back());
      }
    dictPos = 0;
    vector<BaseGDL*> kwRef;
    for( SizeT k=0; k<nKW; ++k)
      {
	PyDict_Next( kwDict, &dictPos, &key, &value);
	BaseGDL* pP = FromPython( value); // throws
	kwRef.push_back( pP);

	const char* keyChar = PyString_AsString( key);
	e.SetKeyword(  StrUpCase( keyChar), &kwRef.back());
      }
   
  // make the call
  if( libPro) 
    static_cast<DLibPro*>(e.GetPro())->Pro()(&e); // throws
  else
    interpreter->call_pro(static_cast<DSubUD*>
			  (e.GetPro())->GetTree()); //throws

  // copy back args and keywords
  for( SizeT p=0; p<nArg; ++p)
    {
      BaseGDL* gdlPar = e.GetPar( p);
      if( gdlPar != NULL)
	{
	  PyObject* pyObj = gdlPar->ToPython(); // throws
	  delete gdlPar;
	  int success0 = PyTuple_SetItem( argTuple, p, pyObj);
	  Py_INCREF(pyObj);
	}
      else
	{
	  int success0 = PyTuple_SetItem( argTuple, p, Py_None);
	  Py_INCREF(Py_None);
	}
    }
  dictPos = 0;
  for( SizeT k=0; k<nKW; ++k)
    {
      BaseGDL* gdlKW = e.GetKW( gdlKWIx[ k]);
      PyDict_Next( kwDict, &dictPos, &key, &value);
      if( gdlKW != NULL)
	{
	  PyObject* pyObj = gdlKW->ToPython(); // throws
	  delete gdlKW;
	  int success0 = PyDict_SetItem( kwDict, key, pyObj);
	  Py_INCREF(pyObj);
	}
      else
	{
	  int success0 = PyDict_SetItem( kwDict, key, Py_None);
	  Py_INCREF(Py_None);
	}
    }
  
    // no error: return Py_None from procedure
    Py_INCREF(Py_None);
    retVal = Py_None;

    }
    // avoid crossing intialization error

  ret:
    // restore old signal handlers
    signal(SIGINT,oldControlCHandler);
    signal(SIGFPE,oldSigFPEHandler);
    
    return retVal;
  }

  // python GDL module method table
  PyMethodDef GDLMethods[] = {
    {"pro",      (PyCFunction) GDL_pro,      METH_VARARGS | METH_KEYWORDS,
     "Execute a GDL procedure."},
    {"function", (PyCFunction) GDL_function, METH_VARARGS | METH_KEYWORDS,
     "Execute a GDL function."},
    {"script",   (PyCFunction) GDL_script,   METH_VARARGS | METH_KEYWORDS,
     "Run a GDL script (sequence of commands)."},
    {NULL, NULL, 0, NULL}        // Sentinel
  };
  
  // python GDL module init function
  PyMODINIT_FUNC initGDL()
  {  
    // ncurses blurs the output, initialize TermWidth here
    TermWidth();

    // initializations
    InitObjects();

    // init library functions
    LibInit(); 

    // instantiate the interpreter
    interpreter = new DInterpreter();

    import_libnumarray(); // obligatory with GDL

    PyObject* m = Py_InitModule("GDL", GDLMethods);

    gdlError = PyErr_NewException("GDL.error", NULL, NULL);
    Py_INCREF(gdlError);
    PyModule_AddObject(m, "error", gdlError);
  }
  
} // extern "C" 

#endif
