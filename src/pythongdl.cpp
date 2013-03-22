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

#include "gdleventhandler.hpp"

// SA fix based on:
// http://synopsis.fresco.org/viewsvn/Synopsis/branches/Synopsis_0_8/src/Synopsis/Python/Object.hh?r2=1792&rev=1792&r1=1657&sortdir=down
#if PY_VERSION_HEX < 0x02050000
  typedef int Py_ssize_t;
#endif

void LibInit(); // defined in libinit.cpp

using namespace std;

// everything is executed within this interpreter
// initialized in the initGDL function
DInterpreter* interpreter;
PyObject*     gdlError;

bool GetScript( PyObject *argTuple, DString& name)
{
  if( argTuple == NULL)
    {
      PyErr_SetString( gdlError, "No input.");
      return false;
    }

  int nArg = PyTuple_Size( argTuple);
  if( nArg == 0)
    {
      PyErr_SetString( gdlError, "No input.");
      return false;
    }

  PyObject* proPy = PyTuple_GetItem(argTuple, 0);
  BaseGDL* proGDL = FromPython( proPy); // throws
  if( proGDL->Type() != GDL_STRING)
    {
      PyErr_SetString( gdlError, "Script must be a tuple of strings.");
      GDLDelete(proGDL);
      return false;
    }

  name = StrUpCase((*(static_cast< DStringGDL*>( proGDL)))[ 0]);
  GDLDelete(proGDL);
  
  return true;
}

bool GetFirstString( PyObject *argTuple, DString& name)
{
  if( argTuple == NULL)
    {
      PyErr_SetString( gdlError, "No argument.");
      return false;
    }

  int nArg = PyTuple_Size( argTuple);
  if( nArg == 0)
    {
      PyErr_SetString( gdlError, "No argument.");
      return false;
    }

  PyObject* proPy = PyTuple_GetItem(argTuple, 0);
  BaseGDL* proGDL = FromPython( proPy); // throws
  if( proGDL->Type() != GDL_STRING || proGDL->N_Elements() != 1)
    {
      PyErr_SetString( gdlError, "First argument must be a scalar string");
      GDLDelete(proGDL);
      return false;
    }

  name = (*(static_cast< DStringGDL*>( proGDL)))[ 0];
  GDLDelete(proGDL);
  
  return true;
}


bool CheckSub( DSub* sub, PyObject *argTuple, PyObject *kwDict)
{
  int     nKey   = sub->NKey();
  int     nPar   = sub->NPar();

  int nArg = PyTuple_Size( argTuple);

  // check args and keywords
  if( nPar != -1 && (nArg-1) > nPar)
    {
      string errString = "Only " + i2s(nPar) + 
	" arguments are allowed in call to: " + sub->ObjectName();
      PyErr_SetString( gdlError, errString.c_str());
      return false;
    }

  if( kwDict == NULL) return true; // finish

  int nKW = PyDict_Size( kwDict);

  if( nKW > nKey) 
    {
      string errString = "Only " + i2s(nKey) + 
	" keywords are allowed in call to: " + sub->ObjectName();
      PyErr_SetString( gdlError, errString.c_str());
      return false;
    }
  return true;
}

bool CopyArgFromPython( vector<BaseGDL*>& parRef,
			vector<BaseGDL*>& kwRef,
			EnvBaseT& e,
			PyObject *argTuple, PyObject *kwDict)
{
  int nArg = PyTuple_Size( argTuple);

  if( nArg > 1)
    parRef.reserve( nArg-1);
  
  // copy arguments
  for( SizeT p=1; p<nArg; ++p)
    {
      PyObject *pyArg = PyTuple_GetItem(argTuple, p);
      if( PyTuple_Check( pyArg)) // local variable (no cpy back)
	{
	  BaseGDL* pP = FromPython( PyTuple_GetItem( pyArg, 0)); // throws
	  parRef.push_back( NULL);
	  e.SetNextPar( pP);
	}
      else
	{
	  BaseGDL* pP = FromPython( pyArg); // throws
	  parRef.push_back( pP);
	  e.SetNextPar( &(parRef.back()));

	  //	  cout << "Set arg " << p << ": "; pP->ToStream( cout); cout << endl;
	  //	  cout << pP << " " << parRef.back() << "  &" << &parRef.back() << endl;
	}
    }
  if( kwDict != NULL)
    {
      PyObject *key, *value;
      Py_ssize_t dictPos = 0;
      
      int nKW = PyDict_Size( kwDict);

      parRef.reserve( nKW);

      for( SizeT k=0; k<nKW; ++k)
	{
	  PyDict_Next( kwDict, &dictPos, &key, &value);
	  int keyIsString =  PyString_Check( key);
	  if( !keyIsString)
	    {
	      PyErr_SetString( gdlError, 
			       "Keywords must be of type string");
	      return false;
	    }
	  const char* keyChar = PyString_AsString( key);
	  string keyString = StrUpCase( keyChar);
	  int kwIx = e.GetPro()->FindKey( keyString);
	  if( kwIx == -1) 
	    {
	      string errString = "Keyword " + string(keyChar) + 
		" not allowed in call to: " + e.GetPro()->ObjectName();
	      PyErr_SetString( gdlError, errString.c_str());
	      return false;
	    }
	  
	  if( PyTuple_Check( value)) // local keyword (no cpy back)
	    {
	      BaseGDL* pP = FromPython( PyTuple_GetItem( value, 0)); // throws
	      kwRef.push_back( NULL);
	      e.SetKeyword(  keyString, pP);
	    }
	  else
	    {
	      BaseGDL* pP = FromPython( value); // throws
	      kwRef.push_back( pP);
	      e.SetKeyword(  keyString, &kwRef.back());
	    }
	}
    }
  
  e.ResolveExtra(); // expand _EXTRA

  return true;
}

bool CopyArgToPython( vector<BaseGDL*>& parRef,
		      vector<BaseGDL*>& kwRef,
		      EnvBaseT& e,
		      PyObject *argTuple, PyObject *kwDict)
{
  int nArg = PyTuple_Size( argTuple);
  for( SizeT p=1; p<nArg; ++p)
    {
      BaseGDL* gdlPar = parRef[ p-1];
      if( gdlPar != NULL)
	{
	  PyObject* pyObj = gdlPar->ToPython(); // throws
	  int success0 = PyTuple_SetItem( argTuple, p, pyObj);
	  // Py_DECREF(pyObj); not needed: PyTuple_SetItem steals
	}
    }
  if( kwDict != NULL)
    {
      PyObject *key, *value;
      Py_ssize_t dictPos = 0;

      int nKW = PyDict_Size( kwDict);
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
	}
    }
  return true;
}

int (*oldInputHook)();
int GDLEventHandlerPy()
{
  GDLEventHandler();
  if( oldInputHook != NULL)
    (*oldInputHook)();
}
  
// Execute a GDL subroutine
PyObject *GDLSub( PyObject *self, PyObject *argTuple, PyObject *kwDict,
		  bool functionCall)
{
  feclearexcept(FE_ALL_EXCEPT);

  PyOS_sighandler_t oldControlCHandler = PyOS_setsig(SIGINT,ControlCHandler);
  PyOS_sighandler_t oldSigFPEHandler   = PyOS_setsig(SIGFPE,SigFPEHandler);

  PyObject *retVal = NULL; // init to error indicator

  vector<BaseGDL*> parRef;
  vector<BaseGDL*> kwRef;
  bool success;
  DString pro;
    
  // handle GDL exceptions
  try {

    success = GetFirstString( argTuple, pro);
    if( !success) goto ret;

    pro = StrUpCase( pro);

    DSub*    sub;
    bool     libCall = false;

    if( functionCall)
      {
	// search for function pro
	// first search library functions
	int proIx = LibFunIx( pro);
	if( proIx != -1)
	  {
	    // PCALL_LIB
	    sub = libFunList[ proIx];
	    libCall = true;
	  }
	else
	  {
	    // FCALL - user defined procedures
	    proIx = FunIx( pro);
	    if( proIx == -1)
	      {
		/*bool found=*/ interpreter->SearchCompilePro( pro, false);
	      
		proIx = FunIx( pro);
		if( proIx == -1)
		  {
		    string errString = "Function " + pro + " not found.";
		    PyErr_SetString( gdlError, errString.c_str());
		    goto ret;
		  }
	      }
	  
	    sub = proList[ proIx];
	  }
      }
    else
      {
	// search for procedure pro
	// first search library procedures
	int proIx = LibProIx( pro);
	if( proIx != -1)
	  {
	    // PCALL_LIB
	    sub = libProList[ proIx];
	    libCall = true;
	  }
	else
	  {
	    // PCALL - user defined procedures
	    proIx = ProIx( pro);
	    if( proIx == -1)
	      {
		/*bool found=*/ interpreter->SearchCompilePro( pro, true);
	      
		proIx = ProIx( pro);
		if( proIx == -1)
		  {
		    string errString = "Procedure " + pro + " not found.";
		    PyErr_SetString( gdlError, errString.c_str());
		    goto ret;
		  }
	      }
	  
	    sub = proList[ proIx];
	  }
      }
      
    success = CheckSub( sub, argTuple, kwDict);
    if( !success) goto ret;
      
    // build the environment
    EnvBaseT* e;

    if( libCall)
      e = new EnvT( NULL, sub);
    else
      e = new EnvUDT( NULL, sub);

    Guard< EnvBaseT> e_guard( e);

    // copy arguments
    success = CopyArgFromPython( parRef, kwRef, *e, argTuple, kwDict);
    if( !success) goto ret;

    // make the call
    StackSizeGuard<EnvStackT> guard( GDLInterpreter::CallStack());
    
    if( !libCall)
    {
      GDLInterpreter::CallStack().push_back( static_cast<EnvUDT*>(e));
      e_guard.release();
    }
    
    BaseGDL* retValGDL = NULL;
    Guard<BaseGDL> retValGDL_guard;
    if( functionCall)
      {
	if( libCall) 
	  retValGDL = static_cast<DLibFun*>(static_cast<EnvT*>(e)->
					    GetPro())->Fun()( static_cast<EnvT*>(e));
	else
	  retValGDL = interpreter->call_fun(static_cast<DSubUD*>
					    (static_cast<EnvUDT*>(e)
					     ->GetPro())->GetTree());
	retValGDL_guard.Reset( retValGDL);
      }
    else
      {
	if( libCall) 
	  static_cast<DLibPro*>(e->GetPro())->Pro()(static_cast<EnvT*>(e)); // throws
	else
	  interpreter->call_pro(static_cast<DSubUD*>
				(e->GetPro())->GetTree()); //throws
      }

    // copy back args and keywords
    success = CopyArgToPython( parRef, kwRef, *e, argTuple, kwDict);
    if( !success) goto ret;

    if( retValGDL != NULL)
      {
	retVal = retValGDL->ToPython();
      }
  }
  catch ( GDLException ex)
    {
      // ERROR GDL exception
      string errString = "Calling " + pro + ": " + ex.toString();
      PyErr_SetString( gdlError, errString.c_str());
      goto ret;
    }

  if( retVal == NULL)
    {
      // no error: return Py_None from procedure
      Py_INCREF(Py_None);
      retVal = Py_None;
    }

 ret:
  // free GDL parameters and keywords
  Purge( parRef);
  Purge( kwRef);

  // restore old signal handlers
  PyOS_setsig(SIGINT,oldControlCHandler);
  PyOS_setsig(SIGFPE,oldSigFPEHandler);
    
  return retVal;
}

// GDL is a C++ program
extern "C" {

  // Execute a GDL procedure
  PyObject *GDL_script(PyObject *self, PyObject *argTuple, PyObject *kwDict)
  {
    PyOS_sighandler_t oldControlCHandler = PyOS_setsig(SIGINT,ControlCHandler);
    PyOS_sighandler_t oldSigFPEHandler   = PyOS_setsig(SIGFPE,SigFPEHandler);

    PyObject *retVal = NULL; // init to error indicator

    bool success;
    DString file;
    
    success = GetFirstString( argTuple, file);
    if( !success) goto ret;

    {
      ifstream in(file.c_str());
      if( !in.good())
	{
	  string errString = "Error opening file: "+file;
	  PyErr_SetString( gdlError, errString.c_str());
	  goto ret;
	}
      
      success = interpreter->RunBatch( &in);
      if( !success)
	{
	  string errString = "Error in batch file: "+file;
	  PyErr_SetString( gdlError, errString.c_str());
	  goto ret;
	}
    }
    
    Py_INCREF(Py_None);
    retVal = Py_None;

    ret:
      // restore old signal handlers
      PyOS_setsig(SIGINT,oldControlCHandler);
      PyOS_setsig(SIGFPE,oldSigFPEHandler);
      
      return retVal;
  }

  // Execute a GDL procedure
  PyObject *GDL_function(PyObject *self, PyObject *argTuple, PyObject *kwDict)
  {
    return GDLSub( self, argTuple, kwDict, true);
  }

  // Execute a GDL procedure
  PyObject *GDL_pro(PyObject *self, PyObject *argTuple, PyObject *kwDict)
  {
    return GDLSub( self, argTuple, kwDict, false);
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
    // http://docs.scipy.org/doc/numpy/reference/c-api.array.html#miscellaneous
    import_array();

    // note: we don't use atexit here
    // ncurses blurs the output, initialize TermWidth here
    TermWidth();

    // initializations
    InitObjects();

    // init library functions
    LibInit(); 

    // instantiate the interpreter (creates $MAIN$ environment)
    interpreter = new DInterpreter();

    PyObject* m = Py_InitModule("GDL", GDLMethods);

    gdlError = PyErr_NewException((char*)"GDL.error", NULL, NULL);
    Py_INCREF(gdlError);
    PyModule_AddObject(m, "error", gdlError);

    // GDL event handling
    oldInputHook = PyOS_InputHook;
    PyOS_InputHook = GDLEventHandlerPy;
  }
  
} // extern "C" 

//#endif
#endif // INCLUDE_PYTHONGDL_CPP
