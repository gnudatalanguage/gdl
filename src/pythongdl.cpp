/***************************************************************************
                          pythongdl.cpp  -  GDL embedded in python
                                            too be included by gdlpython
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

// this has to be included from gdlpython because python
// (here numarray) duplicates the C API for each compilation unit module
// which includes it and hence numarray does not feel initialized in 
// gdlpython.cpp
// and as topython.cpp has to be included from datatypes,
// they all have to be included from datatypes
#ifdef INCLUDE_PYTHONGDL_CPP

//#include "includefirst.hpp"

// if build as a python module
//#ifdef PYTHON_MODULE

// already included from includefirst
//#include <Python.h>

//#include <memory> // auto_ptr
//#include <vector> 

//#include "datatypes.hpp"
//#include "envt.hpp"
//#include "sigfpehandler.hpp"
//#include "terminfo.hpp"
//#include "dinterpreter.hpp"

//#include <numarray/numarray.h>

void LibInit(); // defined in libinit.cpp

using namespace std;

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
    PyOS_sighandler_t oldControlCHandler = PyOS_setsig(SIGINT,ControlCHandler);
    PyOS_sighandler_t oldSigFPEHandler   = PyOS_setsig(SIGFPE,SigFPEHandler);

    //    feclearexcept(FE_ALL_EXCEPT); // new round new luck
    GDLEventHandler(); // we don't have a command line here

    PyObject *retVal = NULL; // init to error indicator

    vector<BaseGDL*> parRef;
    vector<BaseGDL*> kwRef;

    // avoid crossing intialization error
    {

    if( argTuple == NULL)
      {
	PyErr_SetString( gdlError, "No procedure name.");
	goto ret;
      }

    int nArg = PyTuple_Size( argTuple);
    if( nArg == 0)
      {
	PyErr_SetString( gdlError, "No procedure name.");
	goto ret;
      }
    
    cout << "nArg: " << nArg << endl;

    DString pro;

    // handle GDL exceptions
    try {

      // reference count???
      PyObject* proPy = PyTuple_GetItem(argTuple, 0);
      BaseGDL* proGDL = FromPython( proPy); // throws
      auto_ptr< BaseGDL> proGDL_guard( proGDL);

      cout << "proGDL->TypeStr(): " << proGDL->TypeStr() << endl;

      if( proGDL->Type() != STRING || proGDL->N_Elements() != 1)
	{
	  PyErr_SetString( gdlError, "Procedure name must be a scalar string");
	  goto ret;
	}

      pro = StrUpCase((*(static_cast< DStringGDL*>( proGDL)))[ 0]);

      cout << "pro: " << pro << endl;

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
		  string errString = "Procedure: " + pro + " not found.";
		  PyErr_SetString( gdlError, errString.c_str());
		  goto ret;
		}
	    }
	
	  sub = proList[ proIx];
	}

      cout << "sub->ObjectName(): " << sub->ObjectName() << endl;

      int     nKey   = sub->NKey();
      int     nPar   = sub->NPar();

      cout << "nPar: " << nPar << endl;

      // check args and keywords
      if( nPar != -1 && (nArg-1) > nPar)
	{
	  string errString = "Only " + i2s(nPar) + 
	    " arguments are allowed in call to: " + pro;
	  PyErr_SetString( gdlError, errString.c_str());
	  goto ret;
	}

      int nKW = 0;
      if( kwDict != NULL)
	nKW =  PyDict_Size( kwDict);

      if( nKW > nKey) 
	{
	  string errString = "Only " + i2s(nKey) + 
	    " keywords are allowed in call to: " + pro;
	  PyErr_SetString( gdlError, errString.c_str());
	  goto ret;
	}

      cout << "nKW: " << nKW << endl;

      // build the environment
      RefDNode dummyNode; 
      EnvT  e( interpreter, dummyNode, sub);

      // copy arguments
      for( SizeT p=1; p<nArg; ++p)
	{
	  BaseGDL* pP = FromPython( PyTuple_GetItem(argTuple, p)); // throws
	  parRef.push_back( pP);
	  e.SetNextPar( &parRef.back());
	}
      if( kwDict != NULL)
	{
	  PyObject *key, *value;
	  int dictPos = 0;

	  for( SizeT k=0; k<nKW; ++k)
	    {
	      PyDict_Next( kwDict, &dictPos, &key, &value);
	      int keyIsString =  PyString_Check( key);
	      if( !keyIsString)
		{
		  PyErr_SetString( gdlError, 
				   "Keywords must be of type string");
		  goto ret;
		}
	      const char* keyChar = PyString_AsString( key);
	      string keyString = StrUpCase( keyChar);
	      int kwIx = sub->FindKey( keyString);
	      if( kwIx == -1) 
		{
		  string errString = "Keyword " + string(keyChar) + 
		    " not allowed in call to: " + pro;
		  PyErr_SetString( gdlError, errString.c_str());
		  goto ret;
		}

	      BaseGDL* pP = FromPython( value); // throws
	      kwRef.push_back( pP);

	      e.SetKeyword(  keyString, &kwRef.back());
	    }
	}   

      // make the call
      if( libPro) 
	static_cast<DLibPro*>(e.GetPro())->Pro()(&e); // throws
      else
	interpreter->call_pro(static_cast<DSubUD*>
			      (e.GetPro())->GetTree()); //throws

      // copy back args and keywords
      for( SizeT p=1; p<nArg; ++p)
	{
	  BaseGDL* gdlPar = parRef[ p-1];
	  if( gdlPar != NULL)
	    {
	      PyObject* pyObj = gdlPar->ToPython(); // throws
	      int success0 = PyTuple_SetItem( argTuple, p, pyObj);
	      // Py_DECREF(pyObj); not needed: PyTuple_SetItem steals
	    }
	  else
	    {
	      int success0 = PyTuple_SetItem( argTuple, p, Py_None);
	      // Py_DECREF(Py_None); not needed: PyTuple_SetItem steals
	    }
	}
      if( kwDict != NULL)
	{
	  PyObject *key, *value;
	  int dictPos = 0;
	  for( SizeT k=0; k<nKW; ++k)
	    {
	      BaseGDL* gdlKW = kwRef[ k];
	      PyDict_Next( kwDict, &dictPos, &key, &value);
	      if( gdlKW != NULL)
		{
		  PyObject* pyObj = gdlKW->ToPython(); // throws
		  int success0 = PyDict_SetItem( kwDict, key, pyObj);
		  Py_DECREF( pyObj);
		}
	      else
		{
		  int success0 = PyDict_SetItem( kwDict, key, Py_None);
		}
	    }
	}
    }
    catch ( GDLException ex)
      {
	// ERROR GDL exception
	string errString = "Calling " + pro + ": " + ex.toString();
	PyErr_SetString( gdlError, errString.c_str());
	goto ret;
      }

    // no error: return Py_None from procedure
    Py_INCREF(Py_None);
    retVal = Py_None;

    }
    // avoid crossing intialization error

  ret:
    // free GDL parameters and keywords
    Purge( parRef);
    Purge( kwRef);

    // restore old signal handlers
    PyOS_setsig(SIGINT,oldControlCHandler);
    PyOS_setsig(SIGFPE,oldSigFPEHandler);
    
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

    PyObject* m = Py_InitModule("GDL", GDLMethods);

    gdlError = PyErr_NewException("GDL.error", NULL, NULL);
    Py_INCREF(gdlError);
    PyModule_AddObject(m, "error", gdlError);

    import_libnumarray(); // obligatory with GDL
  }
  
} // extern "C" 

//#endif
#endif // INCLUDE_PYTHONGDL_CPP
