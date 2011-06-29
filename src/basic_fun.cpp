/***************************************************************************
                          basic_fun.cpp  -  basic GDL library function
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens (exceptions see below)
    email                : m_schellens@users.sf.net

 strtok_fun, getenv_fun, tag_names_fun, stregex_fun:
 (C) 2004 by Peter Messmer    
 
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

// get_kbrd patch
// http://sourceforge.net/forum/forum.php?thread_id=3292183&forum_id=338691
#include <termios.h> 
#include <unistd.h> 
#include <limits>
#include <string>
#include <fstream>
//#include <memory>
#include <regex.h> // stregex

#ifdef __APPLE__
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
#include <unistd.h>
#endif

#if defined(__FreeBSD__) || defined(__sun__) || defined(__OpenBSD__)
extern "C" char **environ;
#endif

#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"
#include "basic_pro.hpp"
#include "terminfo.hpp"
#include "typedefs.hpp"
#include "base64.hpp"

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif
#include <sys/utsname.h>

/* max regexp error message length */
#define MAX_REGEXPERR_LENGTH 80

namespace lib {

  using namespace std;
  using namespace antlr;

  // assumes all parameters from pOffs till end are dim
  void arr( EnvT* e, dimension& dim, SizeT pOffs=0)
  {

    int nParam=e->NParam()-pOffs;

    if( nParam <= 0)
      e->Throw( "Incorrect number of arguments.");

    const string BadDims="Array dimensions must be greater than 0.";


    if( nParam == 1 ) {

      BaseGDL* par = e->GetParDefined( pOffs); 
 	
      SizeT newDim;
      int ret = par->Scalar2index( newDim);

      if (ret < 0) throw GDLException(BadDims);

      if( ret > 0) {  // single argument
	if (newDim < 1) throw GDLException(BadDims);
	dim << newDim;
	return;
      } 
      if( ret == 0) { //  array argument
	DLongGDL* ind = 
	  static_cast<DLongGDL*>(par->Convert2(LONG, BaseGDL::COPY)); 	 
	auto_ptr<DLongGDL> ind_guard( ind);
		    //e->Guard( ind);

	for(SizeT i =0; i < par->N_Elements(); ++i){
	  if  ((*ind)[i] < 1) throw GDLException(BadDims);
	  dim << (*ind)[i];
	}
	return;
      }
      e->Throw( "arr: should never arrive here.");	
      return;
    }

    // max number checked in interpreter
    SizeT endIx=nParam+pOffs;
    for( SizeT i=pOffs; i<endIx; i++)
      {
	BaseGDL* par=e->GetParDefined( i);

	SizeT newDim;
	int ret=par->Scalar2index( newDim);
	if( ret < 1 || newDim == 0) throw GDLException(BadDims);
	dim << newDim;
      }
  }

  BaseGDL* bytarr( EnvT* e)
  {
    dimension dim;
//    try{
      arr( e, dim);
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DByteGDL(dim, BaseGDL::NOZERO);
      return new DByteGDL(dim);
 //   }
 //   catch( GDLException& ex)
 //     {
//	e->Throw( ex.getMessage());
//      }
  }
  BaseGDL* intarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DIntGDL(dim, BaseGDL::NOZERO);
      return new DIntGDL(dim);
//     }
//     catch( GDLException& ex)
//       {
// 	e->Throw( "INTARR: "+ex.getMessage());
//       }
  }
  BaseGDL* uintarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DUIntGDL(dim, BaseGDL::NOZERO);
      return new DUIntGDL(dim);
//     }
//     catch( GDLException& ex)
//       {
// 	e->Throw( "UINTARR: "+ex.getMessage());
//       }
  }
  BaseGDL* lonarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DLongGDL(dim, BaseGDL::NOZERO);
      return new DLongGDL(dim);
/*    }
    catch( GDLException& ex)
      {
	e->Throw( "LONARR: "+ex.getMessage());
      }*/
  }
  BaseGDL* ulonarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DULongGDL(dim, BaseGDL::NOZERO);
      return new DULongGDL(dim);
 /*   }
    catch( GDLException& ex)
      {
	e->Throw( "ULONARR: "+ex.getMessage());
      }
 */ 
}
  BaseGDL* lon64arr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DLong64GDL(dim, BaseGDL::NOZERO);
      return new DLong64GDL(dim);
/*    }
    catch( GDLException& ex)
      {
	e->Throw( "LON64ARR: "+ex.getMessage());
      }*/
  }
  BaseGDL* ulon64arr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DULong64GDL(dim, BaseGDL::NOZERO);
      return new DULong64GDL(dim);
/*  }
    catch( GDLException& ex)
      {
	e->Throw( "ULON64ARR: "+ex.getMessage());
      }*/
  }
  BaseGDL* fltarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DFloatGDL(dim, BaseGDL::NOZERO);
      return new DFloatGDL(dim);
   /* }
    catch( GDLException& ex)
      {
	e->Throw( "FLTARR: "+ex.getMessage());
      }
  */}
  BaseGDL* dblarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DDoubleGDL(dim, BaseGDL::NOZERO);
      return new DDoubleGDL(dim);
   /* }
    catch( GDLException& ex)
      {
	e->Throw( "DBLARR: "+ex.getMessage());
      }*/
  }
  BaseGDL* strarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) 
	e->Throw( "Keyword parameters not allowed in call.");
      return new DStringGDL(dim);
 /*   }
    catch( GDLException& ex)
      {
	e->Throw( "STRARR: "+ex.getMessage());
      }
 */ }
  BaseGDL* complexarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      if( e->KeywordSet(0)) return new DComplexGDL(dim, BaseGDL::NOZERO);
      return new DComplexGDL(dim);
    /*}
    catch( GDLException& ex)
      {
	e->Throw( "COMPLEXARR: "+ex.getMessage());
      }
 */ }
  BaseGDL* dcomplexarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)

	if( e->KeywordSet(0)) return new DComplexDblGDL(dim, BaseGDL::NOZERO);
      return new DComplexDblGDL(dim);
 /*   }
    catch( GDLException& ex)
      {
	e->Throw( "DCOMPLEXARR: "+ex.getMessage());
      }
 */ }
  BaseGDL* ptrarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      DPtrGDL* ret;

//       if( e->KeywordSet(0))
// 	       ret= new DPtrGDL(dim);//, BaseGDL::NOZERO);
//       else
//     if( e->KeywordSet(1))
// 	ret= new DPtrGDL(dim, BaseGDL::NOZERO);
//       else
// 	return new DPtrGDL(dim);
    if( !e->KeywordSet(1))
		return new DPtrGDL(dim);

	ret= new DPtrGDL(dim, BaseGDL::NOZERO);

	  SizeT nEl=ret->N_Elements();
	  SizeT sIx=e->NewHeap(nEl);
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
	  for( SizeT i=0; i<nEl; i++)
		(*ret)[i]=sIx+i;
}
      return ret;
/*    }
    catch( GDLException& ex)
      {
	e->Throw( "PTRARR: "+ex.getMessage());
      }*/
  }
  BaseGDL* objarr( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

// reference counting      if( e->KeywordSet(0)) return new DObjGDL(dim, BaseGDL::NOZERO);
      return new DObjGDL(dim);
  /*  }
    catch( GDLException& ex)
      {
	e->Throw( "OBJARR: "+ex.getMessage());
      }
 */ }

  BaseGDL* ptr_new( EnvT* e)
  {
    int nParam=e->NParam();
    
    if( nParam > 0)
      {
	// new ptr from undefined variable is allowed as well
	BaseGDL* p= e->GetPar( 0);
        if( p == NULL)
	  {
	    DPtr heapID= e->NewHeap();
	    return new DPtrGDL( heapID);
	  }

	if( e->KeywordSet(0)) // NO_COPY
	  {
	    BaseGDL** p= &e->GetPar( 0);
	    // 	    if( *p == NULL)
	    // 	      e->Throw( "Parameter undefined: "+
	    // 				  e->GetParString(0));

	    DPtr heapID= e->NewHeap( 1, *p);
	    *p=NULL;
	    return new DPtrGDL( heapID);
	  }
	else
	  {
	    BaseGDL* p= e->GetParDefined( 0);

	    DPtr heapID= e->NewHeap( 1, p->Dup());
	    return new DPtrGDL( heapID);
	  }
      }
    else
      {
	if( e->KeywordSet(1)) // ALLOCATE_HEAP
	  {
	    DPtr heapID= e->NewHeap();
	    return new DPtrGDL( heapID);
	  }
	else
	  {
	    return new DPtrGDL( 0); // null ptr
	  }
      }
  }

  BaseGDL* ptr_valid( EnvT* e)
  {
    int nParam=e->NParam();
    
    if( e->KeywordPresent( 1)) // COUNT
      {
	e->SetKW( 1, new DLongGDL( e->Interpreter()->HeapSize()));
      }

    if( nParam == 0)
      {
	return e->Interpreter()->GetAllHeap();
      } 

    BaseGDL* p = e->GetPar( 0);
    if( p == NULL)
      {
	return new DByteGDL( 0);
      } 

    if( e->KeywordSet( 0)) // CAST
      {
	DLongGDL* pL = dynamic_cast<DLongGDL*>( p);
	auto_ptr<DLongGDL> pL_guard;
	if( pL == NULL)
	  {
	    pL = static_cast<DLongGDL*>(p->Convert2(LONG,BaseGDL::COPY)); 
	    pL_guard.reset( pL);
	  }
	
	SizeT nEl = pL->N_Elements();
	DPtrGDL* ret = new DPtrGDL( pL->Dim()); // zero
	GDLInterpreter* interpreter = e->Interpreter();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    if( interpreter->PtrValid( (*pL)[ i])) 
	      (*ret)[ i] = (*pL)[ i];
	  }
	return ret;
      }

    DPtrGDL* pPtr = dynamic_cast<DPtrGDL*>( p);
    if( pPtr == NULL)
      {
	return new DByteGDL( p->Dim()); // zero
      }

    SizeT nEl = pPtr->N_Elements();
    DByteGDL* ret = new DByteGDL( pPtr->Dim()); // zero
    GDLInterpreter* interpreter = e->Interpreter();
    for( SizeT i=0; i<nEl; ++i)
      {
	if( interpreter->PtrValid( (*pPtr)[ i])) 
	  (*ret)[ i] = 1;
      }
    return ret;
  }

  BaseGDL* obj_valid( EnvT* e)
  {
    int nParam=e->NParam();
    
    if( e->KeywordPresent( 1)) // COUNT
      {
	e->SetKW( 1, new DLongGDL( e->Interpreter()->ObjHeapSize()));
      }

    if( nParam == 0)
      {
	return e->Interpreter()->GetAllObjHeap();
      } 

    BaseGDL* p = e->GetPar( 0);
    if( p == NULL)
      {
	return new DByteGDL( 0);
      } 

    if( e->KeywordSet( 0)) // CAST
      {
	DLongGDL* pL = dynamic_cast<DLongGDL*>( p);
	auto_ptr<DLongGDL> pL_guard;
	if( pL == NULL)
	  {
	    pL = static_cast<DLongGDL*>(p->Convert2(LONG,BaseGDL::COPY));
	    pL_guard.reset( pL);
	    //	    e->Guard( pL);
	  }
	
	SizeT nEl = pL->N_Elements();
	DObjGDL* ret = new DObjGDL( pL->Dim()); // zero
	GDLInterpreter* interpreter = e->Interpreter();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    if( interpreter->ObjValid( (*pL)[ i])) 
	      (*ret)[ i] = (*pL)[ i];
	  }
	return ret;
      }

    DObjGDL* pObj = dynamic_cast<DObjGDL*>( p);
    if( pObj == NULL)
      {
	return new DByteGDL( p->Dim()); // zero
      }

    SizeT nEl = pObj->N_Elements();
    DByteGDL* ret = new DByteGDL( pObj->Dim()); // zero
    GDLInterpreter* interpreter = e->Interpreter();
    for( SizeT i=0; i<nEl; ++i)
      {
	if( interpreter->ObjValid( (*pObj)[ i])) 
	  (*ret)[ i] = 1;
      }
    return ret;
  }

  BaseGDL* obj_new( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());
    
    int nParam=e->NParam();
    
    if( nParam == 0)
      {
	return new DObjGDL( 0);
      }
    
    DString objName;
    e->AssureScalarPar<DStringGDL>( 0, objName);

    // this is a struct name -> convert to UPPERCASE
    objName=StrUpCase(objName);

    DStructDesc* objDesc=e->Interpreter()->GetStruct( objName, e->CallingNode());

    DStructGDL* objStruct= new DStructGDL( objDesc, dimension());

    DObj objID= e->NewObjHeap( 1, objStruct); // owns objStruct

    BaseGDL* newObj = new DObjGDL( objID); // the object

    try {
      // call INIT function
      DFun* objINIT= objDesc->GetFun( "INIT");
      if( objINIT != NULL)
	{
	  // morph to obj environment and push it onto the stack again
	  e->PushNewEnvUD( objINIT, 1, &newObj);
	
	  BaseGDL* res=e->Interpreter()->call_fun( objINIT->GetTree());
	
	  if( res == NULL || (!res->Scalar()) || res->False())
	    {
	      delete res;
	      return new DObjGDL( 0);
	    }
	  delete res;
	}
    } catch(...) {
      e->FreeObjHeap( objID); // newObj might be changed
      delete newObj;
      throw;
    }

    return newObj;
  }

  BaseGDL* bindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DByteGDL(dim, BaseGDL::INDGEN);
   /* }
    catch( GDLException& ex)
      {
	e->Throw( "BINDGEN: "+ex.getMessage());
      }
 */ }
  // keywords not supported yet
  BaseGDL* indgen( EnvT* e)
  {
    dimension dim;

    // Defaulting to INT
    DType type = INT;

    static int kwIx1 = e->KeywordIx("BYTE");
    if (e->KeywordSet(kwIx1)){ type = BYTE; }

    static int kwIx2 = e->KeywordIx("COMPLEX");
    if (e->KeywordSet(kwIx2)){ type = COMPLEX; }
    
    static int kwIx3 = e->KeywordIx("DCOMPLEX");
    if (e->KeywordSet(kwIx3)){ type = COMPLEXDBL; }

    static int kwIx4 = e->KeywordIx("DOUBLE");
    if (e->KeywordSet(kwIx4)){ type = DOUBLE; }

    static int kwIx5 = e->KeywordIx("FLOAT");
    if (e->KeywordSet(kwIx5)){ type = FLOAT; }
    
    static int kwIx6 = e->KeywordIx("L64");
    if (e->KeywordSet(kwIx6)){ type = LONG64; }

    static int kwIx7 = e->KeywordIx("LONG");
    if (e->KeywordSet(kwIx7)){ type = LONG; }

    static int kwIx8 = e->KeywordIx("STRING");
    if (e->KeywordSet(kwIx8)){ type = STRING; }

    static int kwIx9 = e->KeywordIx("UINT");
    if (e->KeywordSet(kwIx9)){ type = UINT; }

    static int kwIx10 = e->KeywordIx("UL64");
    if (e->KeywordSet(kwIx10)){ type = ULONG64; }

    static int kwIx11 = e->KeywordIx("ULONG");
    if (e->KeywordSet(kwIx11)){ type = ULONG; }
    
    /*try
      {*/
	// Seeing if the user passed in a TYPE code
	static int kwIx12 = e->KeywordIx("TYPE");
	if ( e->KeywordPresent(kwIx12)){
	  DLong temp_long;
	  e->AssureLongScalarKW(kwIx12, temp_long);
	  type = static_cast<DType>(temp_long);
	}

	arr(e, dim);
	if (dim[0] == 0)
	  throw GDLException( "Array dimensions must be greater than 0");

	switch(type)
	  {
	  case INT:        return new DIntGDL(dim, BaseGDL::INDGEN);
	  case BYTE:       return new DByteGDL(dim, BaseGDL::INDGEN);
	  case COMPLEX:    return new DComplexGDL(dim, BaseGDL::INDGEN);
	  case COMPLEXDBL: return new DComplexDblGDL(dim, BaseGDL::INDGEN);
	  case DOUBLE:     return new DDoubleGDL(dim, BaseGDL::INDGEN);
	  case FLOAT:      return new DFloatGDL(dim, BaseGDL::INDGEN);
	  case LONG64:     return new DLong64GDL(dim, BaseGDL::INDGEN);
	  case LONG:       return new DLongGDL(dim, BaseGDL::INDGEN);
	  case STRING: {
	    DULongGDL* iGen = new DULongGDL(dim, BaseGDL::INDGEN);
	    return iGen->Convert2(STRING);
	  }
	  case UINT:       return new DUIntGDL(dim, BaseGDL::INDGEN);
	  case ULONG64:    return new DULong64GDL(dim, BaseGDL::INDGEN);
	  case ULONG:      return new DULongGDL(dim, BaseGDL::INDGEN);
	  default:
	    e->Throw( "Invalid type code specified.");
	    break;
	  }
/*      }
    catch( GDLException& ex)
      {
	e->Throw( ex.getMessage());
      }*/
  }

  BaseGDL* uindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DUIntGDL(dim, BaseGDL::INDGEN);
   /* }
    catch( GDLException& ex)
      {
	e->Throw( "UINDGEN: "+ex.getMessage());
      }
 */ }
  BaseGDL* sindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      DULongGDL* iGen = new DULongGDL(dim, BaseGDL::INDGEN);
      return iGen->Convert2( STRING);
/*    }
    catch( GDLException& ex)
      {
	e->Throw( "SINDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* lindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      return new DLongGDL(dim, BaseGDL::INDGEN);
/*    }
    catch( GDLException& ex)
      {
	e->Throw( "LINDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* ulindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DULongGDL(dim, BaseGDL::INDGEN);
/*    }
    catch( GDLException& ex)
      {
	e->Throw( "ULINDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* l64indgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DLong64GDL(dim, BaseGDL::INDGEN);
  /*  }
    catch( GDLException& ex)
      {
	e->Throw( "L64INDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* ul64indgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DULong64GDL(dim, BaseGDL::INDGEN);
 /*   }
    catch( GDLException& ex)
      {
	e->Throw( "UL64INDGEN: "+ex.getMessage());
      }
 */ }
  BaseGDL* findgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DFloatGDL(dim, BaseGDL::INDGEN);
  /*  }
    catch( GDLException& ex)
      {
	e->Throw( "FINDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* dindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DDoubleGDL(dim, BaseGDL::INDGEN);
  /*  }
    catch( GDLException& ex)
      {
	e->Throw( "DINDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* cindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DComplexGDL(dim, BaseGDL::INDGEN);
  /*  }
    catch( GDLException& ex)
      {
	e->Throw( "CINDGEN: "+ex.getMessage());
      }*/
  }
  BaseGDL* dcindgen( EnvT* e)
  {
    dimension dim;
//     try{
      arr( e, dim); 
      if (dim[0] == 0)
	throw GDLException( "Array dimensions must be greater than 0");

      return new DComplexDblGDL(dim, BaseGDL::INDGEN);
  /*  }
    catch( GDLException& ex)
      {
	e->Throw( "DCINDGEN: "+ex.getMessage());
      }
 */ }

  BaseGDL* n_elements( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam != 1)
      e->Throw( 
			  "Incorrect number of arguments.");

    BaseGDL* p0=e->GetPar( 0);

    if( p0 == NULL) return new DLongGDL( 0);
    
    return new DLongGDL( p0->N_Elements());
  }

  template< typename ComplexGDL, typename Complex, typename Float>
  BaseGDL* complex_fun_template( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);
    if( nParam <= 2)
      {
	if( nParam == 2)
	  {
	    BaseGDL* p0=e->GetParDefined( 0);
	    BaseGDL* p1=e->GetParDefined( 1);
	    auto_ptr<Float> p0Float( static_cast<Float*>
				     (p0->Convert2( Float::t,BaseGDL::COPY)));
	    auto_ptr<Float> p1Float( static_cast<Float*>
				     (p1->Convert2( Float::t,BaseGDL::COPY)));
	    if( p0Float->Rank() == 0)
	      {
		ComplexGDL* res = new ComplexGDL( p1Float->Dim(), 
						  BaseGDL::NOZERO);
		
		SizeT nE=p1Float->N_Elements();
#pragma omp parallel if (nE >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nE))
{
#pragma omp for
		for( SizeT i=0; i<nE; i++)
		  {
		    (*res)[i]=Complex( (*p0Float)[0], (*p1Float)[i]);
		  }
}
		return res;
	      }
	    else if( p1Float->Rank() == 0)
	      {
		ComplexGDL* res = new ComplexGDL( p0Float->Dim(), 
						  BaseGDL::NOZERO);
		
		SizeT nE=p0Float->N_Elements();
#pragma omp parallel if (nE >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nE))
{
#pragma omp for
		for( SizeT i=0; i<nE; i++)
		  {
		    (*res)[i]=Complex( (*p0Float)[i], (*p1Float)[0]);
		  }
}
		return res;
	      }
	    else if( p0Float->N_Elements() >= p1Float->N_Elements())
	      {
		ComplexGDL* res = new ComplexGDL( p1Float->Dim(), 
						  BaseGDL::NOZERO);

		SizeT nE=p1Float->N_Elements();
#pragma omp parallel if (nE >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nE))
{
#pragma omp for
		for( SizeT i=0; i<nE; i++)
		  {
		    (*res)[i]=Complex( (*p0Float)[i], (*p1Float)[i]);
		  }
}
		return res;
	      }
	    else
	      {
		ComplexGDL* res = new ComplexGDL( p0Float->Dim(), 
						  BaseGDL::NOZERO);
		
		SizeT nE=p0Float->N_Elements();
#pragma omp parallel if (nE >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nE))
{
#pragma omp for
		for( SizeT i=0; i<nE; i++)
		  {
		    (*res)[i]=Complex( (*p0Float)[i], (*p1Float)[i]);
		  }
}
		return res;
	      }
	  }
	else
	  {
            // SA: see tracker item 3151760 ... but it does not work yet :(
	    BaseGDL* p0 = e->GetParDefined( 0);
            // if (ComplexGDL::t == p0->Type() && e->GlobalPar(0)) 
            //  return p0;
            //else 
	    return p0->Convert2( ComplexGDL::t, BaseGDL::COPY);
	  }
      }
    else // COMPLEX( expr, offs, dim1,..,dim8)
      {
	BaseGDL* p0 = e->GetParDefined( 0);
	// *** WRONG: with offs data is converted bytewise
	auto_ptr<Float> p0Float(static_cast<Float*>
				(p0->Convert2( Float::t,
					       BaseGDL::COPY)));
	DLong offs;
	e->AssureLongScalarPar( 1, offs);
      
	dimension dim;
	arr( e, dim, 2);

	SizeT nElCreate=dim.N_Elements();
	
	SizeT nElSource=p0->N_Elements();
      
	if( (offs+2*nElCreate) > nElSource)
	  e->Throw( "Specified offset to"
		    " array is out of range: "+e->GetParString(0));
	
	ComplexGDL* res=new ComplexGDL( dim, BaseGDL::NOZERO);

#pragma omp parallel if (nElCreate >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nElCreate))
{
#pragma omp for
	for( SizeT i=0; i<nElCreate; i++)
	  {
	    SizeT srcIx=2*i+offs;
	    (*res)[i]=Complex( (*p0Float)[srcIx], (*p0Float)[srcIx+1]);
	  }
}	
	return res;
      }
  }

  BaseGDL* complex_fun( EnvT* e)
  {
    return complex_fun_template< DComplexGDL, DComplex, DFloatGDL>( e);
  }
  BaseGDL* dcomplex_fun( EnvT* e)
  {
    return complex_fun_template< DComplexDblGDL, DComplexDbl, DDoubleGDL>( e);
  }

  template< class TargetClass>
  BaseGDL* type_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    if( nParam == 1)
      {
	BaseGDL* p0=e->GetParDefined( 0);

	assert( dynamic_cast< EnvUDT*>( e->Caller()) != NULL);

	// type_fun( expr) just convert
	if( static_cast< EnvUDT*>( e->Caller())->GetIOError() != NULL) 
	  return p0->Convert2( TargetClass::t, 
			       BaseGDL::COPY_THROWIOERROR);
        // SA: see tracker item no. 3151760 ... does not work yet
        //else if (TargetClass::t == p0->Type() && e->GlobalPar(0)) 
        //  return p0;
	else
	  return p0->Convert2( TargetClass::t, BaseGDL::COPY);
      }
    
    BaseGDL* p0=e->GetNumericParDefined( 0);

    // BYTE( expr, offs, dim1,..,dim8)
    DLong offs;
    e->AssureLongScalarPar( 1, offs);

    dimension dim;

    if( nParam > 2)
      arr( e, dim, 2);
    
    TargetClass* res=new TargetClass( dim, BaseGDL::NOZERO);

    SizeT nByteCreate=res->NBytes(); // net size of new data
      
    SizeT nByteSource=p0->NBytes(); // net size of src
      
    if( offs < 0 || (offs+nByteCreate) > nByteSource)
      {
	delete res;
	e->Throw( "Specified offset to"
		  " expression is out of range: "+e->GetParString(0));
      }

    //*** POSSIBLE ERROR because of alignment here
    void* srcAddr = static_cast<void*>( static_cast<char*>(p0->DataAddr()) + 
					offs);
    void* dstAddr = static_cast<void*>(&(*res)[0]);
    memcpy( dstAddr, srcAddr, nByteCreate);

    //     char* srcAddr = reinterpret_cast<char*>(p0->DataAddr());
    //     char* dstAddr = reinterpret_cast<char*>(&(*res)[0]);
    //     copy( srcAddr, srcAddr+nByteCreate, dstAddr);

    return res;
  }

  BaseGDL* byte_fun( EnvT* e)
  {
    return type_fun<DByteGDL>( e);
  }
  BaseGDL* uint_fun( EnvT* e)
  {
    return type_fun<DUIntGDL>( e);
  }
  BaseGDL* long_fun( EnvT* e)
  {
    return type_fun<DLongGDL>( e);
  }
  BaseGDL* ulong_fun( EnvT* e)
  {
    return type_fun<DULongGDL>( e);
  }
  BaseGDL* long64_fun( EnvT* e)
  {
    return type_fun<DLong64GDL>( e);
  }
  BaseGDL* ulong64_fun( EnvT* e)
  {
    return type_fun<DULong64GDL>( e);
  }
  BaseGDL* float_fun( EnvT* e)
  {
    return type_fun<DFloatGDL>( e);
  }
  BaseGDL* double_fun( EnvT* e)
  {
    return type_fun<DDoubleGDL>( e);
  }
  // STRING function behaves different
  BaseGDL* string_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam == 0)
      e->Throw( "Incorrect number of arguments.");

    bool printKey =  e->KeywordSet( 4);
    int parOffset = 0; 

    // SA: handling special VMS-compatibility syntax, e.g.: string(1,'$(F)')
    //     (if nor FORMAT neither PRINT defined, >1 parameter, last param is scalar string
    //     which begins with "$(" or "(" but is not "()" then last param [minus "$"] is treated as FORMAT)
    bool vmshack = false;
    if (!printKey && (e->GetKW(0) == NULL) && nParam > 1) 
    {    
      vmshack = true;
      BaseGDL* par = e->GetParDefined(nParam - 1);
      if (par->Type() == STRING && par->Scalar())
      {
        int dollar = (*static_cast<DStringGDL*>(par))[0].compare(0,2,"$(");
        if (dollar == 0 || ((*static_cast<DStringGDL*>(par))[0].compare(0,1,"(") == 0 && (*static_cast<DStringGDL*>(par))[0] != "()"))   
        {    
          e->SetKeyword("FORMAT", new DStringGDL(
            (*static_cast<DStringGDL*>(par))[0].c_str() + (dollar == 0 ? 1 : 0) 
          ));
        }
      }    
    }    

    BaseGDL* format_kw = e->GetKW( 0);
    bool formatKey = format_kw != NULL;

    if (formatKey && format_kw->Type() == STRING && (*static_cast<DStringGDL*>(format_kw))[0] == "") formatKey = false;

    if( printKey || formatKey) // PRINT or FORMAT
      {
	stringstream os;

	SizeT width = 0;
	if( printKey) // otherwise: FORMAT -> width is ignored
	  {
	    // for /PRINT always a terminal width of 80 is assumed
	    width = 80;//TermWidth();
	  }
	
        if (vmshack)
        {
          parOffset = 1; 
          e->ShiftParNumbering(1);
        }
	print_os( &os, e, parOffset, width);
        if (vmshack) 
        {
          e->ShiftParNumbering(-1);
	}

	deque<DString> buf;
	while( os.good())
	  {
	    string line;
	    getline( os, line);
	    if( os.good()) buf.push_back( line);
	  }

	SizeT bufSize = buf.size();
	if( bufSize == 0)
	  e->Throw( "Internal error: print buffer empty.");

	if( bufSize > 1) 
	  {
	    DStringGDL* retVal = 
	      new DStringGDL( dimension( bufSize), BaseGDL::NOZERO);

	    for( SizeT i=0; i<bufSize; ++i)
	      (*retVal)[ i] = buf[ i];

	    return retVal;
	  }
	else
	  return new DStringGDL( buf[0]);
      }
    else
      {
	if( nParam == 1) // nParam == 1 -> conversion
	  {
	    BaseGDL* p0 = e->GetParDefined( 0);
            // SA: see tracker item no. 3151760 ... but it does not work yet
            //if (p0->Type() == STRING && e->GlobalPar(0)) return p0;
	    return p0->Convert2( STRING, BaseGDL::COPY);
	  }
	else // concatenation
	  {
	    DString s;
	    for( SizeT i=0; i<nParam; ++i)
	      {
		BaseGDL* p = e->GetParDefined( i);
		DStringGDL* sP = static_cast<DStringGDL*>
		  ( p->Convert2(STRING,
				BaseGDL::COPY_BYTE_AS_INT));

		SizeT nEl = sP->N_Elements();
		for( SizeT e=0; e<nEl; ++e)
		  s += (*sP)[ e];
		delete sP;
	      }
	    // IDL here breaks the string into tty-width substrings
	    return new DStringGDL( s);
	  }
      }
  }

  BaseGDL* fix_fun( EnvT* e)
  {
    DIntGDL* type = e->IfDefGetKWAs<DIntGDL>( 0);
    if (type != NULL) {
      int typ = (*type)[0];
      if (typ == BYTE)
      {
        // SA: slow yet simple solution using BYTE->INT->BYTE conversion
        return (e->KeywordSet(1) && e->GetPar(0)->Type() == STRING)
          ? type_fun<DIntGDL>( e)->Convert2(BYTE, BaseGDL::CONVERT) 
          : type_fun<DByteGDL>( e);
      }
      if (typ == 0 || typ == INT) return type_fun<DIntGDL>( e);
      if (typ == UINT) return type_fun<DUIntGDL>( e);
      if (typ == LONG) return type_fun<DLongGDL>( e);
      if (typ == ULONG) return type_fun<DULongGDL>( e);
      if (typ == LONG64) return type_fun<DLong64GDL>( e);
      if (typ == ULONG64) return type_fun<DULong64GDL>( e);
      if (typ == FLOAT) return type_fun<DFloatGDL>( e);
      if (typ == DOUBLE) return type_fun<DDoubleGDL>( e);
      if (typ == COMPLEX) return type_fun<DComplexGDL>( e);
      if (typ == COMPLEXDBL) return type_fun<DComplexDblGDL>( e);
      if (typ == STRING) 
      {
        // SA: calling STRING() with correct parameters
        static int stringIx = LibFunIx("STRING");
        EnvT* newEnv= new EnvT(e, libFunList[stringIx], NULL);
        newEnv->SetNextPar(&e->GetPar(0)); // pass as global
        if (e->KeywordSet(1) && e->GetPar(0)->Type() == BYTE)
          newEnv->SetKeyword("PRINT", new DIntGDL(1));
        e->Interpreter()->CallStack().push_back( newEnv); 
        return static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
      }
      e->Throw( "Improper TYPE value.");
    }
    return type_fun<DIntGDL>( e);
  }

  BaseGDL* call_function( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam == 0)
     e->Throw( "No function specified.");
    
    DString callF;
    e->AssureScalarPar<DStringGDL>( 0, callF);

    // this is a function name -> convert to UPPERCASE
    callF = StrUpCase( callF);

    // first search library funcedures
    int funIx=LibFunIx( callF);
    if( funIx != -1)
      {
	e->PushNewEnv( libFunList[ funIx], 1);
	
	// make the call
	EnvT* newEnv = static_cast<EnvT*>(e->Interpreter()->CallStack().back());
	return static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv);
      }
    else
      {
	funIx = GDLInterpreter::GetFunIx( callF);
	
	e->PushNewEnvUD( funList[ funIx], 1);
	
	// make the call
	EnvUDT* newEnv = static_cast<EnvUDT*>(e->Interpreter()->CallStack().back());
	return e->Interpreter()->
	  call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
      }
  }

  BaseGDL* call_method_function( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam < 2)
      e->Throw(  "Name and object reference"
			  " must be specified.");
    
    DString callP;
    e->AssureScalarPar<DStringGDL>( 0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase( callP);
    
    DStructGDL* oStruct = e->GetObjectPar( 1);

    DFun* method= oStruct->Desc()->GetFun( callP);

    if( method == NULL)
      e->Throw( "Method not found: "+callP);

    e->PushNewEnv( method, 2, &e->GetPar( 1));
    
    // make the call
    return e->Interpreter()->call_fun( method->GetTree());
  }



  BaseGDL* execute( EnvT* e)
  {
    int nParam=e->NParam( 1);

    bool quietCompile = false;
    if( nParam == 2)
      {
	BaseGDL* p1 = e->GetParDefined( 1);
	
	if( !p1->Scalar())
	  e->Throw( "Expression must be scalar in this context: "+
			      e->GetParString(1));
	
	quietCompile = p1->True();
      }

    if (e->GetParDefined(0)->Rank() != 0)
      e->Throw("Expression must be scalar in this context: "+e->GetParString(0));
    
    DString line;
    e->AssureScalarPar<DStringGDL>( 0, line);

    // remove current environment (own one)
    assert( dynamic_cast<EnvUDT*>(e->Caller()) != NULL);
    EnvUDT* caller = static_cast<EnvUDT*>(e->Caller());
    e->Interpreter()->CallStack().pop_back();
    delete e;

    istringstream istr(line+"\n");

    RefDNode theAST;
    try {  
      GDLLexer   lexer(istr, "", caller->CompileOpt());
      GDLParser& parser=lexer.Parser();
    
      parser.interactive();
    
      theAST=parser.getAST();
    }
    catch( GDLException& ex)
      {
	if( !quietCompile) GDLInterpreter::ReportCompileError( ex);
	return new DIntGDL( 0);
      }
    catch( ANTLRException ex)
      {
	if( !quietCompile) cerr << "EXECUTE: Lexer/Parser exception: " <<  
			     ex.getMessage() << endl;
	return new DIntGDL( 0);
      }
    
    if( theAST == NULL) return new DIntGDL( 1);

    RefDNode trAST;
    try
      {
	GDLTreeParser treeParser( caller);
	  
	treeParser.interactive(theAST);

	trAST=treeParser.getAST();
      }
    catch( GDLException& ex)
      {
	if( !quietCompile) GDLInterpreter::ReportCompileError( ex);
	return new DIntGDL( 0);
      }

    catch( ANTLRException ex)
      {
	if( !quietCompile) cerr << "EXECUTE: Compiler exception: " <<  
			     ex.getMessage() << endl;
	return new DIntGDL( 0);
      }
      
    if( trAST == NULL) return new DIntGDL( 1);

	int nForLoopsIn = caller->NForLoops();
    try
      {
		ProgNodeP progAST = ProgNode::NewProgNode( trAST);
		auto_ptr< ProgNode> progAST_guard( progAST);

		int nForLoops = ProgNode::NumberForLoops( progAST, nForLoopsIn);
		caller->ResizeForLoops( nForLoops);

		progAST->setLine( e->GetLineNumber());

		RetCode retCode = caller->Interpreter()->execute( progAST);

		caller->ResizeForLoops( nForLoopsIn);

		if( retCode == RC_OK)
		return new DIntGDL( 1);
		else
		return new DIntGDL( 0);
      }
    catch( GDLException& ex)
      {
		caller->ResizeForLoops( nForLoopsIn);
		// are we throwing to target environment?
// 		if( ex.GetTargetEnv() == NULL)
			if( !quietCompile) cerr << "EXECUTE: " <<
					ex.getMessage() << endl;
		return new DIntGDL( 0);
      }
    catch( ANTLRException ex)
      {
		caller->ResizeForLoops( nForLoopsIn);
		
		if( !quietCompile) cerr << "EXECUTE: Interpreter exception: " <<
					ex.getMessage() << endl;
		return new DIntGDL( 0);
      }

    return new DIntGDL( 0); // control flow cannot reach here - compiler shut up
  }

  BaseGDL* assoc( EnvT* e)
  {
    SizeT nParam=e->NParam( 2);

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      e->Throw( "File unit does not allow"
		" this operation. Unit: "+i2s( lun));

    DLong offset = 0;
    if( nParam >= 3) e->AssureLongScalarPar( 2, offset);
    
    BaseGDL* arr = e->GetParDefined( 1);
    
    if( arr->StrictScalar())
      e->Throw( "Scalar variable not allowed in this"
		" context: "+e->GetParString(1));
    
    return arr->AssocVar( lun, offset);
  }

  // gdl_ naming because of weired namespace problem in MSVC
  BaseGDL* gdl_logical_and( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam != 2)
      e->Throw(
			  "Incorrect number of arguments.");

    BaseGDL* e1=e->GetParDefined( 0);//, "LOGICAL_AND");
    BaseGDL* e2=e->GetParDefined( 1);//, "LOGICAL_AND");

    ULong nEl1 = e1->N_Elements();
    ULong nEl2 = e2->N_Elements();

    Data_<SpDByte>* res;

    if( e1->Scalar()) 
      {
	if( e1->LogTrue(0)) 
	  {
	    res= new Data_<SpDByte>( e2->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl2 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl2))
{
#pragma omp for
	    for( SizeT i=0; i < nEl2; i++)
	      (*res)[i] = e2->LogTrue( i) ? 1 : 0;
}
	  }
	else
	  {
	    return new Data_<SpDByte>( e2->Dim());
	  }
      }
    else if( e2->Scalar()) 
      {
	if( e2->LogTrue(0)) 
	  {
	    res= new Data_<SpDByte>( e1->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl1 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl1))
{
#pragma omp for
	    for( SizeT i=0; i < nEl1; i++)
	      (*res)[i] = e1->LogTrue( i) ? 1 : 0;
}
	  }
	else
	  {
	    return new Data_<SpDByte>( e1->Dim());
	  }
      }
    else if( nEl2 < nEl1) 
      {
	res= new Data_<SpDByte>( e2->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl2 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl2))
{
#pragma omp for
	for( SizeT i=0; i < nEl2; i++)
	  (*res)[i] = (e1->LogTrue( i) && e2->LogTrue( i)) ? 1 : 0;
}
      }
    else // ( nEl2 >= nEl1)
      {
	res= new Data_<SpDByte>( e1->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl1 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl1))
{
#pragma omp for
	for( SizeT i=0; i < nEl1; i++)
	  (*res)[i] = (e1->LogTrue( i) && e2->LogTrue( i)) ? 1 : 0;
}
      }
    return res;
  }

  // gdl_ naming because of weired namespace problem in MSVC
  BaseGDL* gdl_logical_or( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam != 2)
      e->Throw(
			  "Incorrect number of arguments.");

    BaseGDL* e1=e->GetParDefined( 0);//, "LOGICAL_OR");
    BaseGDL* e2=e->GetParDefined( 1);//, "LOGICAL_OR");

    ULong nEl1 = e1->N_Elements();
    ULong nEl2 = e2->N_Elements();

    Data_<SpDByte>* res;

    if( e1->Scalar()) 
      {
	if( e1->LogTrue(0)) 
	  {
	    res= new Data_<SpDByte>( e2->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl2 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl2))
{
#pragma omp for
	    for( SizeT i=0; i < nEl2; i++)
	      (*res)[i] = 1;
}
	  }
	else
	  {
	    res= new Data_<SpDByte>( e2->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl2 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl2))
{
#pragma omp for
	    for( SizeT i=0; i < nEl2; i++)
	      (*res)[i] = e2->LogTrue( i) ? 1 : 0;
}
	  }
      }
    else if( e2->Scalar()) 
      {
	if( e2->LogTrue(0)) 
	  {
	    res= new Data_<SpDByte>( e1->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl1 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl1))
{
#pragma omp for
	    for( SizeT i=0; i < nEl1; i++)
	      (*res)[i] = 1;
}
	  }
	else
	  {
	    res= new Data_<SpDByte>( e1->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl1 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl1))
{
#pragma omp for
	    for( SizeT i=0; i < nEl1; i++)
	      (*res)[i] = e1->LogTrue( i) ? 1 : 0;
}
	  }
      }
    else if( nEl2 < nEl1) 
      {
	res= new Data_<SpDByte>( e2->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl2 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl2))
{
#pragma omp for
	for( SizeT i=0; i < nEl2; i++)
	  (*res)[i] = (e1->LogTrue( i) || e2->LogTrue( i)) ? 1 : 0;
}
      }
    else // ( nEl2 >= nEl1)
      {
	res= new Data_<SpDByte>( e1->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl1 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl1))
{
#pragma omp for
	for( SizeT i=0; i < nEl1; i++)
	  (*res)[i] = (e1->LogTrue( i) || e2->LogTrue( i)) ? 1 : 0;
}
      }
    return res;
  }

  BaseGDL* logical_true( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam != 1)
      e->Throw(
			  "Incorrect number of arguments.");

    BaseGDL* e1=e->GetParDefined( 0);//, "LOGICAL_TRUE");
    
    ULong nEl1 = e1->N_Elements();

    Data_<SpDByte>* res = new Data_<SpDByte>( e1->Dim(), BaseGDL::NOZERO);
#pragma omp parallel if (nEl1 >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl1))
{
#pragma omp for
    for( SizeT i=0; i < nEl1; i++)
      (*res)[i] = e1->LogTrue( i) ? 1 : 0;
}    
    return res;
  }

  BaseGDL* replicate( EnvT* e)
  {
    SizeT nParam=e->NParam();
    if( nParam < 2)
      e->Throw( "Incorrect number of arguments.");
    dimension dim;
    arr( e, dim, 1);

    BaseGDL* p0=e->GetParDefined( 0);//, "REPLICATE");
    if( !p0->Scalar())
      e->Throw(	"Expression must be a scalar in this context: "+
		e->GetParString(0));

    return p0->New( dim, BaseGDL::INIT);
  }

  BaseGDL* strtrim( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);//, "STRTRIM");

    BaseGDL* p0 = e->GetPar( 0);
    if( p0 == NULL)
      e->Throw(
			  "Variable is undefined: "+
			  e->GetParString(0));
    DStringGDL* p0S = static_cast<DStringGDL*>
      (p0->Convert2(STRING,BaseGDL::COPY));
    
    DLong mode = 0;
    if( nParam == 2)
      {
	BaseGDL* p1 = e->GetPar( 1);
	if( p1 == NULL)
	  e->Throw(
			      "Variable is undefined: "+e->GetParString(1));
	if( !p1->Scalar())
	  e->Throw(
			      "Expression must be a "
			      "scalar in this context: "+
			      e->GetParString(1));
	DLongGDL* p1L = static_cast<DLongGDL*>
	  (p1->Convert2(LONG,BaseGDL::COPY));

	mode = (*p1L)[ 0];

	delete p1L;

	if( mode < 0 || mode > 2)
	  {
	    ostringstream os;
	    p1->ToStream( os);
	    e->Throw(
				"Value of <"+ p1->TypeStr() +
				"  ("+os.str()+
				")> is out of allowed range.");
	  }
      }
    
    SizeT nEl = p0S->N_Elements();

    if( mode == 2) // both
   {
#pragma omp parallel if ((nEl*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl*10)))
{
#pragma omp for
      for( SizeT i=0; i<nEl; ++i)
	{
	  unsigned long first= (*p0S)[ i].find_first_not_of(" \t");
	  if( first == (*p0S)[ i].npos)
	    {
	      (*p0S)[ i] = "";
	    }
	  else
	    {
	      unsigned long last = (*p0S)[ i].find_last_not_of(" \t");
	      (*p0S)[ i] = (*p0S)[ i].substr(first,last-first+1);
	    }
	}
}
  }
  else if( mode == 1) // leading
     {
#pragma omp parallel if ((nEl*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl*10)))
{
#pragma omp for
	for( SizeT i=0; i<nEl; ++i)
	{
	  unsigned long first= (*p0S)[ i].find_first_not_of(" \t");
	  if( first == (*p0S)[ i].npos)
	    {
	      (*p0S)[ i] = "";
	    }
	  else
	    {
	      (*p0S)[ i] = (*p0S)[ i].substr(first);
	    }
	}
}
    }
    else // trailing
      {
#pragma omp parallel if ((nEl*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl*10)))
{
#pragma omp for
	for( SizeT i=0; i<nEl; ++i)
	{
	  unsigned long last = (*p0S)[ i].find_last_not_of(" \t");
	  if( last == (*p0S)[ i].npos)
	    {
	      (*p0S)[ i] = "";
	    }
	  else
	    {
	      (*p0S)[ i] = (*p0S)[ i].substr(0,last+1);
	    }
	}
}
      }
    return p0S;
  }

  BaseGDL* strcompress( EnvT* e)
  {
    e->NParam( 1);

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);

    bool removeAll =  e->KeywordSet(0);

    DStringGDL* res = new DStringGDL( p0S->Dim(), BaseGDL::NOZERO);

    SizeT nEl = p0S->N_Elements();
#pragma omp parallel if ((nEl*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl*10)))
{
#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = StrCompress((*p0S)[ i], removeAll);
      }
}
    return res;
  }

  BaseGDL* strpos( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);//, "STRPOS");

    bool reverseOffset =  e->KeywordSet(0); // REVERSE_OFFSET
    bool reverseSearch =  e->KeywordSet(1); // REVERSE_SEARCH

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);

    DString searchString;
    //     e->AssureScalarPar<DStringGDL>( 1, searchString);
    DStringGDL* sStr = e->GetParAs<DStringGDL>( 1);
    if( !sStr->Scalar( searchString))
      e->Throw( "Search string must be a scalar or one element array: "+
		e->GetParString( 1));

    unsigned long pos = string::npos;
    if( nParam > 2)
{
    BaseGDL* p2 = e->GetParDefined(2);
//     if( p2 != NULL) //e->AssureLongScalarPar( 2,posDLong);
//       {
	const SizeT pIx = 2;
	BaseGDL* p = e->GetParDefined( pIx);
	DLongGDL* lp = static_cast<DLongGDL*>(p->Convert2( LONG, BaseGDL::COPY));
	auto_ptr<DLongGDL> guard_lp( lp);
	DLong scalar;
	if( !lp->Scalar( scalar))
	  throw GDLException("Parameter must be a scalar in this context: "+
			     e->GetParString(pIx));
	pos = scalar;
      }

    DLongGDL* res = new DLongGDL( p0S->Dim(), BaseGDL::NOZERO);

    SizeT nSrcStr = p0S->N_Elements();
#pragma omp parallel if ((nSrcStr*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nSrcStr*10)))
{
#pragma omp for
    for( long i=0; i<nSrcStr; ++i)
      {
	(*res)[ i] = StrPos((*p0S)[ i], searchString, pos, 
			    reverseOffset, reverseSearch);
      }
}    
    return res;
  }

  BaseGDL* strmid( EnvT* e)
  {
    e->NParam( 2);//, "STRMID");

    bool reverse =  e->KeywordSet(0);

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);
    DLongGDL*   p1L = e->GetParAs<DLongGDL>( 1);

    BaseGDL*  p2  = e->GetPar( 2);
    DLongGDL* p2L = NULL;
    if( p2 != NULL) p2L = e->GetParAs<DLongGDL>( 2);

    DLong scVal1;
    bool sc1 = p1L->Scalar( scVal1);

    DLong scVal2 = numeric_limits<DLong>::max();
    bool sc2 = true;
    if( p2L != NULL) 
      {
	DLong scalar;
	sc2 = p2L->Scalar( scalar);
	scVal2 = scalar;
      }

    DLong stride;
    if( !sc1 && !sc2)
      {
	stride = p1L->Dim( 0);
	if( stride != p2L->Dim( 0))
	  e->Throw(
			      "Starting offset and length arguments "
			      "have incompatible first dimension.");	  
      }
    else
      {
	// at least one scalar, p2L possibly NULL
	if( p2L == NULL)
	  stride = p1L->Dim( 0);
	else
	  stride = max( p1L->Dim( 0), p2L->Dim( 0));
	
	stride = (stride > 0)? stride : 1;
      }

    dimension resDim( p0S->Dim());
    if( stride > 1)
      resDim >> stride;

    DStringGDL* res = new DStringGDL( resDim, BaseGDL::NOZERO);

    SizeT nEl1 = p1L->N_Elements();
    SizeT nEl2 = (sc2)? 1 : p2L->N_Elements();

    SizeT nSrcStr = p0S->N_Elements();
#pragma omp parallel if ((nSrcStr*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nSrcStr*10))) default( shared)
{
#pragma omp for
    for( long i=0; i<nSrcStr; ++i)
      {
		for( long ii=0; ii<stride; ++ii)
		{
			SizeT destIx = i * stride + ii;
			DLong actFirst = (sc1)? scVal1 : (*p1L)[ destIx % nEl1];
			DLong actLen   = (sc2)? scVal2 : (*p2L)[ destIx % nEl2];
			if( actLen <= 0)
				(*res)[ destIx] = "";//StrMid((*p0S)[ i], actFirst, actLen, reverse);
			else	
				(*res)[ destIx] = StrMid((*p0S)[ i], actFirst, actLen, reverse);
		}
      }
}    
    return res;
  }

  BaseGDL* strlowcase( EnvT* e)
  {
    e->NParam( 1);//, "STRLOWCASE");

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);

    DStringGDL* res = new DStringGDL( p0S->Dim(), BaseGDL::NOZERO);
    
    SizeT nEl = p0S->N_Elements();
#pragma omp parallel if ((nEl*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl*10)))
{
#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = StrLowCase((*p0S)[ i]);
      }
}
    return res;
  }

  BaseGDL* strupcase( EnvT* e)
  {
    e->NParam( 1);//, "STRUPCASE");

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);

    DStringGDL* res = new DStringGDL( p0S->Dim(), BaseGDL::NOZERO);
    
    SizeT nEl = p0S->N_Elements();
#pragma omp parallel if ((nEl*10) >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= (nEl*10)))
{
#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = StrUpCase((*p0S)[ i]);
      }
}
    return res;
  }

  BaseGDL* strlen( EnvT* e)
  {
    e->NParam( 1);//, "STRLEN");

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);

    DLongGDL* res = new DLongGDL( p0S->Dim(), BaseGDL::NOZERO);

    SizeT nEl = p0S->N_Elements();
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = (*p0S)[ i].length();
      }
}
    return res;
  }

  BaseGDL* strjoin( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);

    DStringGDL* p0S = e->GetParAs<DStringGDL>( 0);
    SizeT nEl = p0S->N_Elements();

    DString delim = "";
    if( nParam > 1)
      e->AssureStringScalarPar( 1, delim);
    
    bool single = e->KeywordSet( 0); // SINGLE

    if( single)
      {
	DStringGDL* res = new DStringGDL( (*p0S)[0]);
	DString&    scl = (*res)[0];

	for( SizeT i=1; i<nEl; ++i)
	  scl += delim + (*p0S)[i];

	return res;
      }

    dimension resDim( p0S->Dim());
    resDim.Purge();
    
    SizeT stride = resDim.Stride( 1);

    resDim.Remove( 0);

    DStringGDL* res = new DStringGDL( resDim, BaseGDL::NOZERO);
    for( SizeT src=0, dst=0; src<nEl; ++dst)
      {
	(*res)[ dst] = (*p0S)[ src++];
	for(SizeT l=1; l<stride; ++l)
	  (*res)[ dst] += delim + (*p0S)[ src++];
      }
    
    return res;
  }

  BaseGDL* where( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);//, "WHERE");

    BaseGDL* p0 = e->GetParDefined( 0);//, "WHERE");

    SizeT nEl = p0->N_Elements();

    SizeT count;

    DLong* ixList = p0->Where( e->KeywordPresent( 0), count);
    ArrayGuard<DLong> guard( ixList);
    SizeT nCount = nEl - count;

    if( e->KeywordPresent( 0)) // COMPLEMENT
      {
	if( nCount == 0)
	  {
	    e->SetKW( 0, new DLongGDL( -1));
	  }
	else
	  {
	    DLongGDL* cIxList = new DLongGDL( dimension( &nCount, 1), 
					      BaseGDL::NOZERO);
	    
	    SizeT cIx = nEl - 1;
#pragma omp parallel if (nCount >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nCount))
{
#pragma omp for
	    for( SizeT i=0; i<nCount; ++i)
	      (*cIxList)[ i] = ixList[ cIx - i];
// 	      (*cIxList)[ i] = ixList[ --cIx];
}
	    e->SetKW( 0, cIxList);
	  }
      }

    if( e->KeywordPresent( 1)) // NCOMPLEMENT
      {
	e->SetKW( 1, new DLongGDL( nCount));
      }

    if( nParam == 2)
      {
	e->SetPar( 1, new DLongGDL( count));
      }

    if( count == 0) return new DLongGDL( -1);

    return new DLongGDL( ixList, count);

    //     DLongGDL* res = new DLongGDL( dimension( &count, 1), 
    // 				  BaseGDL::NOZERO);
    //     for( SizeT i=0; i<count; ++i)
    //       (*res)[ i] = ixList[ i];

    //     return res;
  }

  BaseGDL* n_params( EnvT* e) 
  {
    EnvUDT* caller = static_cast<EnvUDT*>(e->Caller());
    if( caller == NULL) return new DLongGDL( 0);
    DLong nP = caller->NParam();
    if( caller->IsObject()) 
      return new DLongGDL( nP-1); // "self" is not counted
    return new DLongGDL( nP);
  }

  BaseGDL* keyword_set( EnvT* e)
  {
    e->NParam( 1);//, "KEYWORD_SET");

    BaseGDL* p0 = e->GetPar( 0);
    if( p0 == NULL) return new DIntGDL( 0);
    if( !p0->Scalar()) return new DIntGDL( 1);
    if( p0->Type() == STRUCT) return new DIntGDL( 1);
    if( p0->LogTrue()) return new DIntGDL( 1);
    return new DIntGDL( 0);
  }

  // passing 2nd argument by value is slightly better for float and double, 
  // but incur some overhead for the complex class.
  template<class T> inline void AddOmitNaN(T& dest, T value)
{
 if (isfinite(value)) 
{
// #pragma omp atomic
	dest += value; 
}
}
  template<class T> inline void AddOmitNaNCpx(T& dest, T value)
  {
// #pragma omp atomic
    dest += T(isfinite(value.real())? value.real() : 0,
	      isfinite(value.imag())? value.imag() : 0);
  }
  template<> inline void AddOmitNaN(DComplex& dest, DComplex value)
  { AddOmitNaNCpx<DComplex>(dest, value); }
  template<> inline void AddOmitNaN(DComplexDbl& dest, DComplexDbl value)
  { AddOmitNaNCpx<DComplexDbl>(dest, value); }

  template<class T> inline void NaN2Zero(T& value)
  { if (!isfinite(value)) value = 0; }
  template<class T> inline void NaN2ZeroCpx(T& value)
  {
    value = T(isfinite(value.real())? value.real() : 0, 
              isfinite(value.imag())? value.imag() : 0);
  }
  template<> inline void NaN2Zero(DComplex& value)
  { NaN2ZeroCpx< DComplex>(value); }
  template<> inline void NaN2Zero(DComplexDbl& value)
  { NaN2ZeroCpx< DComplexDbl>(value); }

  // total over all elements
  template<class T>
  BaseGDL* total_template( T* src, bool omitNaN)
  {
    if (!omitNaN) return new T(src->Sum());
    typename T::Ty sum = 0;
    SizeT nEl = src->N_Elements();
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared(sum)
{
#pragma omp for
    for ( SizeT i=0; i<nEl; ++i)
      {
	AddOmitNaN(sum, (*src)[ i]);
      }
}
    return new T(sum);
  }
  
  // cumulative over all dims
  template<typename T>
  BaseGDL* total_cu_template( T* res, bool omitNaN)
  {
    SizeT nEl = res->N_Elements();
    if (omitNaN)
      {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
        for( SizeT i=0; i<nEl; ++i)
          NaN2Zero((*res)[i]);
}
      }
    for( SizeT i=1,ii=0; i<nEl; ++i,++ii)
      (*res)[i] += (*res)[ii];
    return res;
  }

  // total over one dim
  template< typename T>
  BaseGDL* total_over_dim_template( T* src, 
				    const dimension& srcDim, 
				    SizeT sumDimIx,
                                    bool omitNaN)
  {
    SizeT nEl = src->N_Elements();
    
    // get dest dim and number of summations
    dimension destDim = srcDim;
    SizeT nSum = destDim.Remove( sumDimIx);

    T* res = new T( destDim); // zero fields

    // sumStride is also the number of linear src indexing
    SizeT sumStride = srcDim.Stride( sumDimIx); 
    SizeT outerStride = srcDim.Stride( sumDimIx + 1);
    SizeT sumLimit = nSum * sumStride;
    SizeT rIx=0;
    for( SizeT o=0; o < nEl; o += outerStride)
      for( SizeT i=0; i < sumStride; ++i)
	{
	  SizeT oi = o+i;
	  SizeT oiLimit = sumLimit + oi;
          if( omitNaN)
            {
              for( SizeT s=oi; s<oiLimit; s += sumStride)
                AddOmitNaN((*res)[ rIx], (*src)[ s]);
	    }
          else
            {
  	      for( SizeT s=oi; s<oiLimit; s += sumStride)
	        (*res)[ rIx] += (*src)[ s];
            }
	  ++rIx;
	}
    return res;
  }

  // cumulative over one dim
  template< typename T>
  BaseGDL* total_over_dim_cu_template( T* res, 
				       SizeT sumDimIx,
                                       bool omitNaN)
  {
    SizeT nEl = res->N_Elements();
    const dimension& resDim = res->Dim();
    if (omitNaN)
      {
        for( SizeT i=0; i<nEl; ++i)
          NaN2Zero((*res)[i]);
      }
    SizeT cumStride = resDim.Stride( sumDimIx); 
    SizeT outerStride = resDim.Stride( sumDimIx + 1);
    for( SizeT o=0; o < nEl; o += outerStride)
      {
	SizeT cumLimit = o+outerStride;
	for( SizeT i=o+cumStride, ii=o; i<cumLimit; ++i, ++ii)
	  (*res)[ i] += (*res)[ ii];
      }
    return res;
  }


  BaseGDL* total( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);//, "TOTAL");

    BaseGDL* p0 = e->GetParDefined( 0);//, "TOTAL");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));

    if( p0->Type() == STRING)
      e->Throw( "String expression not allowed "
		"in this context: "+e->GetParString(0));
    
    static int cumIx = e->KeywordIx( "CUMULATIVE");
    static int intIx = e->KeywordIx("INTEGER");
    static int doubleIx = e->KeywordIx( "DOUBLE");
    static int nanIx = e->KeywordIx( "NAN");
    static int preserveIx = e->KeywordIx( "PRESERVE_TYPE");

    bool cumulative = e->KeywordSet( cumIx);
    bool intRes  = e->KeywordSet( intIx);
    bool doubleRes  = e->KeywordSet( doubleIx);
    bool nan        = e->KeywordSet( nanIx);
    bool preserve   = e->KeywordSet( preserveIx);

    DLong sumDim = 0;
    if( nParam == 2)
      e->AssureLongScalarPar( 1, sumDim);

    if( sumDim == 0)
      {
	if( !cumulative)
	  {
            if (preserve) 
            {
              switch (p0->Type())
              {
                case BYTE: return total_template<DByteGDL>(static_cast<DByteGDL*>(p0), false);
                case INT: return total_template<DIntGDL>(static_cast<DIntGDL*>(p0), false);
                case UINT: return total_template<DUIntGDL>(static_cast<DUIntGDL*>(p0), false);
                case LONG: return total_template<DLongGDL>(static_cast<DLongGDL*>(p0), false);
                case ULONG: return total_template<DULongGDL>(static_cast<DULongGDL*>(p0), false);
                case LONG64: return total_template<DLong64GDL>(static_cast<DLong64GDL*>(p0), false);
                case ULONG64: return total_template<DULong64GDL>(static_cast<DULong64GDL*>(p0), false);
                case FLOAT: return total_template<DFloatGDL>(static_cast<DFloatGDL*>(p0), nan);
                case DOUBLE: return total_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0), nan);
                case COMPLEX: return total_template<DComplexGDL>(static_cast<DComplexGDL*>(p0), nan);
                case COMPLEXDBL: return total_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0), nan);
                default: assert(false);
              }
            }

	    // Integer parts by Erin Sheldon
	    // In IDL total(), the INTEGER keyword takes precedence 
	    if( intRes )
	      {
		// We use LONG64 unless the input is ULONG64
		if ( p0->Type() == LONG64 )
		  {
		    return total_template<DLong64GDL>
		      ( static_cast<DLong64GDL*>(p0), nan );
		  }
		if ( p0->Type() == ULONG64 )
		  {
		    return total_template<DULong64GDL>
		      ( static_cast<DULong64GDL*>(p0), nan );
		  }

		// Conver to Long64
		DLong64GDL* p0L64 = static_cast<DLong64GDL*>
		  (p0->Convert2( LONG64, BaseGDL::COPY));
		auto_ptr<DLong64GDL> guard( p0L64);
		return total_template<DLong64GDL>( p0L64, nan);

	      } // integer results


	    if( p0->Type() == DOUBLE)
	      {
		return total_template<DDoubleGDL>
                  ( static_cast<DDoubleGDL*>(p0), nan); 
	      }
	    if( p0->Type() == COMPLEXDBL)
	      {
		return total_template<DComplexDblGDL>
                  ( static_cast<DComplexDblGDL*>(p0), nan); 
	      }

	    if( !doubleRes)
	      {
		if( p0->Type() == FLOAT)
		  {
		    return total_template<DFloatGDL>
		      ( static_cast<DFloatGDL*>(p0), nan); 
		  }
		if( p0->Type() == COMPLEX)
		  {
		    return total_template<DComplexGDL>
		      ( static_cast<DComplexGDL*>(p0), nan); 
		  }
 		DFloatGDL* p0F = static_cast<DFloatGDL*>
 		  (p0->Convert2( FLOAT,BaseGDL::COPY));
 		auto_ptr<DFloatGDL> guard( p0F);
		return total_template<DFloatGDL>( p0F, false);
	      }
	    if( p0->Type() == COMPLEX)
	      {
		DComplexDblGDL* p0D = static_cast<DComplexDblGDL*>
		  (p0->Convert2( COMPLEXDBL,BaseGDL::COPY));
		auto_ptr<DComplexDblGDL> p0D_guard( p0D);
		return total_template<DComplexDblGDL>( p0D, nan); 
	      }
	    
	    DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	      (p0->Convert2( DOUBLE, BaseGDL::COPY));
	    auto_ptr<DDoubleGDL> p0D_guard( p0D);
	    return total_template<DDoubleGDL>( p0D, nan);
	  }
	else // cumulative
	  {
            if (preserve) 
            {
              switch (p0->Type())
              {
                case BYTE: return total_cu_template<DByteGDL>(static_cast<DByteGDL*>(p0)->Dup(), false);
                case INT: return total_cu_template<DIntGDL>(static_cast<DIntGDL*>(p0)->Dup(), false);
                case UINT: return total_cu_template<DUIntGDL>(static_cast<DUIntGDL*>(p0)->Dup(), false);
                case LONG: return total_cu_template<DLongGDL>(static_cast<DLongGDL*>(p0)->Dup(), false);
                case ULONG: return total_cu_template<DULongGDL>(static_cast<DULongGDL*>(p0)->Dup(), false);
                case LONG64: return total_cu_template<DLong64GDL>(static_cast<DLong64GDL*>(p0)->Dup(), false);
                case ULONG64: return total_cu_template<DULong64GDL>(static_cast<DULong64GDL*>(p0)->Dup(), false);
                case FLOAT: return total_cu_template<DFloatGDL>(static_cast<DFloatGDL*>(p0)->Dup(), nan);
                case DOUBLE: return total_cu_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0)->Dup(), nan);
                case COMPLEX: return total_cu_template<DComplexGDL>(static_cast<DComplexGDL*>(p0)->Dup(), nan);
                case COMPLEXDBL: return total_cu_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0)->Dup(), nan);
                default: assert(false);
              }
            }

	    // INTEGER keyword takes precedence
	    if( intRes )
	      {
		// We use LONG64 unless the input is ULONG64
		if ( p0->Type() == LONG64 )
		  {
		    return total_cu_template<DLong64GDL>
		      ( static_cast<DLong64GDL*>(p0)->Dup(), nan );
		  }
		if ( p0->Type() == ULONG64 )
		  {
		    return total_cu_template<DULong64GDL>
		      ( static_cast<DULong64GDL*>(p0)->Dup(), nan );
		  }

		// Convert to Long64
		return total_cu_template<DLong64GDL>
		  ( static_cast<DLong64GDL*>
		    (p0->Convert2( LONG64, BaseGDL::COPY)), nan);
						     
	      } // integer results


	    // special case as DOUBLE type overrides /DOUBLE
	    if( p0->Type() == DOUBLE)
	      {
  	        return total_cu_template< DDoubleGDL>
		  ( static_cast<DDoubleGDL*>(p0)->Dup(), nan);
	      }
	    if( p0->Type() == COMPLEXDBL)
	      {
  	        return total_cu_template< DComplexDblGDL>
		  ( static_cast<DComplexDblGDL*>(p0)->Dup(), nan);
	      }



	    if( !doubleRes)
	      {
		// special case for FLOAT has no advantage here
		if( p0->Type() == COMPLEX)
		  {
		    return total_cu_template< DComplexGDL>
                      ( static_cast<DComplexGDL*>(p0)->Dup(), nan);
		  }
    	        return total_cu_template< DFloatGDL>
		  ( static_cast<DFloatGDL*>( p0->Convert2(FLOAT, 
							  BaseGDL::COPY)), nan);
	      }
	    if( p0->Type() == COMPLEX)
	      {
		return total_cu_template< DComplexDblGDL>
		  ( static_cast<DComplexDblGDL*>(p0->Convert2( COMPLEXDBL, 
							       BaseGDL::COPY)), nan);
	      }
    	    return total_cu_template< DDoubleGDL>
	      ( static_cast<DDoubleGDL*>(p0->Convert2( DOUBLE, 
						       BaseGDL::COPY)), nan);
	  }
      }

    // total over sumDim
    dimension srcDim = p0->Dim();
    SizeT srcRank = srcDim.Rank();

    if( sumDim < 1 || sumDim > srcRank)
      e->Throw( 
			  "Array must have "+i2s(sumDim)+
			  " dimensions: "+e->GetParString(0));

    if( !cumulative)
      {
        if (preserve) 
        {
          switch (p0->Type())
          {
            case BYTE: return total_over_dim_template<DByteGDL>(static_cast<DByteGDL*>(p0), srcDim, sumDim-1, false);
            case INT: return total_over_dim_template<DIntGDL>(static_cast<DIntGDL*>(p0), srcDim, sumDim-1, false);
            case UINT: return total_over_dim_template<DUIntGDL>(static_cast<DUIntGDL*>(p0), srcDim, sumDim-1, false);
            case LONG: return total_over_dim_template<DLongGDL>(static_cast<DLongGDL*>(p0), srcDim, sumDim-1, false);
            case ULONG: return total_over_dim_template<DULongGDL>(static_cast<DULongGDL*>(p0), srcDim, sumDim-1, false);
            case LONG64: return total_over_dim_template<DLong64GDL>(static_cast<DLong64GDL*>(p0), srcDim, sumDim-1, false);
            case ULONG64: return total_over_dim_template<DULong64GDL>(static_cast<DULong64GDL*>(p0), srcDim, sumDim-1, false);
            case FLOAT: return total_over_dim_template<DFloatGDL>(static_cast<DFloatGDL*>(p0), srcDim, sumDim-1, nan);
            case DOUBLE: return total_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0), srcDim, sumDim-1, nan);
            case COMPLEX: return total_over_dim_template<DComplexGDL>(static_cast<DComplexGDL*>(p0), srcDim, sumDim-1, nan);
            case COMPLEXDBL: return total_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0), srcDim, sumDim-1, nan);
            default: assert(false);
          }
        }

	// INTEGER keyword takes precedence 
	if( intRes )
	  {
	    // We use LONG64 unless the input is ULONG64
	    if ( p0->Type() == LONG64 )
	      {
		return total_over_dim_template<DLong64GDL>
		  ( static_cast<DLong64GDL*>(p0), srcDim, sumDim-1, nan );
	      }
	    if ( p0->Type() == ULONG64 )
	      {
		return total_over_dim_template<DULong64GDL>
		  ( static_cast<DULong64GDL*>(p0), srcDim, sumDim-1, nan );
	      }
	    
	    // Conver to Long64
	    DLong64GDL* p0L64 = static_cast<DLong64GDL*>
	      (p0->Convert2( LONG64, BaseGDL::COPY));

	    auto_ptr<DLong64GDL> p0L64_guard( p0L64);
	    return total_over_dim_template<DLong64GDL>
	      ( p0L64, srcDim, sumDim-1, nan);
	    
	  } // integer results


	if( p0->Type() == DOUBLE)
	  {
	    return total_over_dim_template< DDoubleGDL>
	      ( static_cast<DDoubleGDL*>(p0), srcDim, sumDim-1, nan);
	  }
	if( p0->Type() == COMPLEXDBL)
	  {
	    return total_over_dim_template< DComplexDblGDL>
	      ( static_cast<DComplexDblGDL*>(p0), srcDim, sumDim-1, nan);
	  }
	if( !doubleRes)
	  {
	    if( p0->Type() == FLOAT)
	      {
		return total_over_dim_template< DFloatGDL>
		  ( static_cast<DFloatGDL*>(p0), srcDim, sumDim-1, nan);
	      }
	    if( p0->Type() == COMPLEX)
	      {
		return total_over_dim_template< DComplexGDL>
		  ( static_cast<DComplexGDL*>(p0), srcDim, sumDim-1, nan);
	      }
	    // default for NOT /DOUBLE
	    DFloatGDL* p0F = static_cast<DFloatGDL*>
	      (p0->Convert2( FLOAT,BaseGDL::COPY));
	    auto_ptr<DFloatGDL> p0F_guard( p0F);
	    //	    p0F_guard.reset( p0F);
	    return total_over_dim_template< DFloatGDL>
	      ( p0F, srcDim, sumDim-1, false);
	  }
	if( p0->Type() == COMPLEX)
	  {
	    DComplexDblGDL* p0D = static_cast<DComplexDblGDL*>
	      (p0->Convert2( COMPLEXDBL,BaseGDL::COPY));
	    auto_ptr<DComplexDblGDL> p0D_guard( p0D);
	    // 	    p0D_guard.reset( p0D);
	    return total_over_dim_template< DComplexDblGDL>
	      ( p0D, srcDim, sumDim-1, nan);
	  }
	// default for /DOUBLE
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	  (p0->Convert2( DOUBLE,BaseGDL::COPY));
	auto_ptr<DDoubleGDL> p0D_guard( p0D);
	//p0D_guard.reset( p0D);
	return total_over_dim_template< DDoubleGDL>( p0D, srcDim, sumDim-1,nan);
      }
    else // cumulative
      {
        if (preserve) 
        {
          switch (p0->Type())
          {
            case BYTE: return total_over_dim_cu_template<DByteGDL>(static_cast<DByteGDL*>(p0)->Dup(), sumDim-1, false);
            case INT: return total_over_dim_cu_template<DIntGDL>(static_cast<DIntGDL*>(p0)->Dup(), sumDim-1, false);
            case UINT: return total_over_dim_cu_template<DUIntGDL>(static_cast<DUIntGDL*>(p0)->Dup(), sumDim-1, false);
            case LONG: return total_over_dim_cu_template<DLongGDL>(static_cast<DLongGDL*>(p0)->Dup(), sumDim-1, false);
            case ULONG: return total_over_dim_cu_template<DULongGDL>(static_cast<DULongGDL*>(p0)->Dup(), sumDim-1, false);
            case LONG64: return total_over_dim_cu_template<DLong64GDL>(static_cast<DLong64GDL*>(p0)->Dup(), sumDim-1, false);
            case ULONG64: return total_over_dim_cu_template<DULong64GDL>(static_cast<DULong64GDL*>(p0)->Dup(), sumDim-1, false);
            case FLOAT: return total_over_dim_cu_template<DFloatGDL>(static_cast<DFloatGDL*>(p0)->Dup(), sumDim-1, nan);
            case DOUBLE: return total_over_dim_cu_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0)->Dup(), sumDim-1, nan);
            case COMPLEX: return total_over_dim_cu_template<DComplexGDL>(static_cast<DComplexGDL*>(p0)->Dup(), sumDim-1, nan);
            case COMPLEXDBL: return total_over_dim_cu_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0)->Dup(), sumDim-1, nan);
            default: assert(false);
          }
        }

	// INTEGER keyword takes precedence
	if( intRes )
	  {
	    // We use LONG64 unless the input is ULONG64
	    if ( p0->Type() == LONG64 )
	      {
		return total_over_dim_cu_template<DLong64GDL>
		  ( static_cast<DLong64GDL*>(p0)->Dup(), sumDim-1, nan );
	      }
	    if ( p0->Type() == ULONG64 )
	      {
		return total_over_dim_cu_template<DULong64GDL>
		  ( static_cast<DULong64GDL*>(p0)->Dup(), sumDim-1, nan );
	      }
	    
	    // Convert to Long64
	    return total_over_dim_cu_template<DLong64GDL>
	      ( static_cast<DLong64GDL*>
		(p0->Convert2( LONG64, BaseGDL::COPY)), sumDim-1, nan);
	    
	  } // integer results


	if( p0->Type() == DOUBLE)
	  {
	    return total_over_dim_cu_template< DDoubleGDL>
	      ( static_cast<DDoubleGDL*>(p0)->Dup(), sumDim-1, nan);
	  }
	if( p0->Type() == COMPLEXDBL)
	  {
	    return total_over_dim_cu_template< DComplexDblGDL>
	      ( static_cast<DComplexDblGDL*>(p0)->Dup(), sumDim-1, nan);
	  }
	if( !doubleRes)
	  {
	    // special case for FLOAT has no advantage here
	    if( p0->Type() == COMPLEX)
	      {
		return total_over_dim_cu_template< DComplexGDL>
		  ( static_cast<DComplexGDL*>(p0)->Dup(), sumDim-1, nan);
	      }
	    // default for NOT /DOUBLE
	    return total_over_dim_cu_template< DFloatGDL>
	      ( static_cast<DFloatGDL*>( p0->Convert2( FLOAT, 
						       BaseGDL::COPY)), sumDim-1, nan);
	  }
	if( p0->Type() == COMPLEX)
	  {
	    return total_over_dim_cu_template< DComplexDblGDL>
	      ( static_cast<DComplexDblGDL*>(p0->Convert2( COMPLEXDBL,
							   BaseGDL::COPY)), sumDim-1, nan);
	  }
	// default for /DOUBLE
	return total_over_dim_cu_template< DDoubleGDL>
	  ( static_cast<DDoubleGDL*>(p0->Convert2( DOUBLE,
						   BaseGDL::COPY)), sumDim-1, nan);
      }
  }


  // passing 2nd argument by value is slightly better for float and double, 
  // but incur some overhead for the complex class.
  template<class T> inline void MultOmitNaN(T& dest, T value)
  { 
	if (isfinite(value)) 
	{
// #pragma omp atomic
		dest *= value; 
	}
  }
  template<class T> inline void MultOmitNaNCpx(T& dest, T value)
  {
    dest *= T(isfinite(value.real())? value.real() : 1,
	      isfinite(value.imag())? value.imag() : 1);
  }
  template<> inline void MultOmitNaN(DComplex& dest, DComplex value)
  { MultOmitNaNCpx<DComplex>(dest, value); }
  template<> inline void MultOmitNaN(DComplexDbl& dest, DComplexDbl value)
  { MultOmitNaNCpx<DComplexDbl>(dest, value); }

  template<class T> inline void Nan2One(T& value)
  { if (!isfinite(value)) value = 1; }
  template<class T> inline void Nan2OneCpx(T& value)
  {
    value = T(isfinite(value.real())? value.real() : 1, 
              isfinite(value.imag())? value.imag() : 1);
  }
  template<> inline void Nan2One(DComplex& value)
  { Nan2OneCpx< DComplex>(value); }
  template<> inline void Nan2One(DComplexDbl& value)
  { Nan2OneCpx< DComplexDbl>(value); }

  // product over all elements
  template<class T>
  BaseGDL* product_template( T* src, bool omitNaN)
  {
    typename T::Ty sum = 1;
    SizeT nEl = src->N_Elements();
    if( !omitNaN) 
	{
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared(sum)
{
#pragma omp for reduction(*:sum)
	for ( SizeT i=0; i<nEl; ++i)
		{
		sum *= (*src)[ i];
		}
}
	}
    else
	{
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl)) shared(sum)
{
#pragma omp for reduction(*:sum)
	for ( SizeT i=0; i<nEl; ++i)
		{
		MultOmitNaN( sum, (*src)[ i]);
		}
}
	}
    return new T( sum);
  }

  template<>
  BaseGDL* product_template( DComplexGDL* src, bool omitNaN)
  {
    DComplexGDL::Ty sum = 1;
    SizeT nEl = src->N_Elements();
    if( !omitNaN) 
	{
	for ( SizeT i=0; i<nEl; ++i)
		{
		sum *= (*src)[ i];
		}
	}
    else
	{
	for ( SizeT i=0; i<nEl; ++i)
		{
		MultOmitNaN( sum, (*src)[ i]);
		}
	}
    return new DComplexGDL( sum);
  }
  
  template<>
  BaseGDL* product_template( DComplexDblGDL* src, bool omitNaN)
  {
    DComplexDblGDL::Ty sum = 1;
    SizeT nEl = src->N_Elements();
    if( !omitNaN) 
	{
	for ( SizeT i=0; i<nEl; ++i)
		{
		sum *= (*src)[ i];
		}
	}
    else
	{
	for ( SizeT i=0; i<nEl; ++i)
		{
		MultOmitNaN( sum, (*src)[ i]);
		}
	}
    return new DComplexDblGDL( sum);
  }
  
  // cumulative over all dims
  template<typename T>
  BaseGDL* product_cu_template( T* res, bool omitNaN)
  {
    SizeT nEl = res->N_Elements();
    if( omitNaN)
      {
        for( SizeT i=0; i<nEl; ++i)
          Nan2One( (*res)[i]);
      }
    for( SizeT i=1,ii=0; i<nEl; ++i,++ii)
      (*res)[i] *= (*res)[ii];
    return res;
  }

  // product over one dim
  template< typename T>
  BaseGDL* product_over_dim_template( T* src, 
				      const dimension& srcDim, 
				      SizeT sumDimIx,
				      bool omitNaN)
  {
    SizeT nEl = src->N_Elements();
    
    // get dest dim and number of summations
    dimension destDim = srcDim;
    SizeT nSum = destDim.Remove( sumDimIx);

    T* res = new T( destDim, BaseGDL::NOZERO);

    // sumStride is also the number of linear src indexing
    SizeT sumStride = srcDim.Stride( sumDimIx); 
    SizeT outerStride = srcDim.Stride( sumDimIx + 1);
    SizeT sumLimit = nSum * sumStride;
    SizeT rIx=0;
    for( SizeT o=0; o < nEl; o += outerStride)
      for( SizeT i=0; i < sumStride; ++i)
	{
	  (*res)[ rIx] = 1;
	  SizeT oi = o+i;
	  SizeT oiLimit = sumLimit + oi;
          if( omitNaN)
            {
              for( SizeT s=oi; s<oiLimit; s += sumStride)
                MultOmitNaN((*res)[ rIx], (*src)[ s]);
	    }
          else
            {
  	      for( SizeT s=oi; s<oiLimit; s += sumStride)
	        (*res)[ rIx] *= (*src)[ s];
            }
	  ++rIx;
	}
    return res;
  }

  // cumulative over one dim
  template< typename T>
  BaseGDL* product_over_dim_cu_template( T* res, 
					 SizeT sumDimIx,
					 bool omitNaN)
  {
    SizeT nEl = res->N_Elements();
    const dimension& resDim = res->Dim();
    if (omitNaN)
      {
        for( SizeT i=0; i<nEl; ++i)
          Nan2One((*res)[i]);
      }
    SizeT cumStride = resDim.Stride( sumDimIx); 
    SizeT outerStride = resDim.Stride( sumDimIx + 1);
    for( SizeT o=0; o < nEl; o += outerStride)
      {
	SizeT cumLimit = o+outerStride;
	for( SizeT i=o+cumStride, ii=o; i<cumLimit; ++i, ++ii)
	  (*res)[ i] *= (*res)[ ii];
      }
    return res;
  }

  BaseGDL* product( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);
    
    BaseGDL* p0 = e->GetParDefined( 0);
    
    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == STRING)
      e->Throw( "String expression not allowed "
		"in this context: "+e->GetParString(0));
    
    static int cumIx = e->KeywordIx( "CUMULATIVE");
    static int nanIx = e->KeywordIx( "NAN");
    static int intIx = e->KeywordIx("INTEGER");
    static int preIx = e->KeywordIx("PRESERVE_TYPE");
    bool KwCumul     = e->KeywordSet( cumIx);
    bool KwNaN       = e->KeywordSet( nanIx);
    bool KwInt       = e->KeywordSet( intIx);
    bool KwPre       = e->KeywordSet( preIx);
    bool nanInt=false;
    
    DLong sumDim = 0;
    if( nParam == 2)
      e->AssureLongScalarPar( 1, sumDim);
    
    if( sumDim == 0) {
	if( !KwCumul) {
          if (KwPre) 
          {
            switch (p0->Type())
            {
              case BYTE: return product_template<DByteGDL>(static_cast<DByteGDL*>(p0), nanInt);
              case INT: return product_template<DIntGDL>(static_cast<DIntGDL*>(p0), nanInt);
              case UINT: return product_template<DUIntGDL>(static_cast<DUIntGDL*>(p0), nanInt);
              case LONG: return product_template<DLongGDL>(static_cast<DLongGDL*>(p0), nanInt);
              case ULONG: return product_template<DULongGDL>(static_cast<DULongGDL*>(p0), nanInt);
              case LONG64: return product_template<DLong64GDL>(static_cast<DLong64GDL*>(p0), nanInt);
              case ULONG64: return product_template<DULong64GDL>(static_cast<DULong64GDL*>(p0), nanInt);
              case FLOAT: return product_template<DFloatGDL>(static_cast<DFloatGDL*>(p0), KwNaN);
              case DOUBLE: return product_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0), KwNaN);
              case COMPLEX: return product_template<DComplexGDL>(static_cast<DComplexGDL*>(p0), KwNaN);
              case COMPLEXDBL: return product_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0), KwNaN);
              default: assert(false);
            }
          }

	  // Integer parts derivated from Total code by Erin Sheldon
	  // In IDL PRODUCT(), the INTEGER keyword takes precedence 
	  if (KwInt) {
	    // We use LONG64 unless the input is ULONG64
	    if ((p0->Type() == LONG64) && (!KwNaN)) {
	      return product_template<DLong64GDL>
		( static_cast<DLong64GDL*>(p0), nanInt );
	    }
	    if ((p0->Type() == ULONG64) && (!KwNaN)) {
	      return product_template<DULong64GDL>
		(static_cast<DULong64GDL*>(p0), nanInt );
	    }
	    
	    // Convert to Long64
	    DLong64GDL* p0L64 = static_cast<DLong64GDL*>
	      (p0->Convert2( LONG64, BaseGDL::COPY));
	    auto_ptr<DLong64GDL> guard( p0L64);
	    if (KwNaN) {
	      DFloatGDL* p0f = static_cast<DFloatGDL*>
		(p0->Convert2( FLOAT, BaseGDL::COPY));
	      auto_ptr<DFloatGDL> guard( p0f);
	      for( SizeT i=0; i<nEl; ++i) {
		if (!isfinite((*p0f)[i])) (*p0L64)[i]=1;
	      }
	    }
	    return product_template<DLong64GDL>( p0L64, nanInt);	      
	  } // integer results
	  
	  if( p0->Type() == DOUBLE) {
	    return product_template<DDoubleGDL>
	      ( static_cast<DDoubleGDL*>(p0), KwNaN); 
	  }
	  if( p0->Type() == COMPLEXDBL) {
	    return product_template<DComplexDblGDL>
	      ( static_cast<DComplexDblGDL*>(p0), KwNaN); 
	  }
	  if( p0->Type() == COMPLEX) {
	    DComplexDblGDL* p0D = static_cast<DComplexDblGDL*>
	      (p0->Convert2( COMPLEXDBL,BaseGDL::COPY));
	    auto_ptr<DComplexDblGDL> p0D_guard( p0D);
	    //p0D_guard.reset( p0D);
	    return product_template<DComplexDblGDL>( p0D, KwNaN); 
	  }
	  
	  DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	    (p0->Convert2( DOUBLE, BaseGDL::COPY));
	  auto_ptr<DDoubleGDL> p0D_guard( p0D);
	  //	    p0D_guard.reset( p0D);
	  return product_template<DDoubleGDL>( p0D, KwNaN);
	} 
	else
	  { // KwCumul

            if (KwPre) 
            {
              switch (p0->Type())
              {
                case BYTE: return product_cu_template<DByteGDL>(static_cast<DByteGDL*>(p0)->Dup(), nanInt);
                case INT: return product_cu_template<DIntGDL>(static_cast<DIntGDL*>(p0)->Dup(), nanInt);
                case UINT: return product_cu_template<DUIntGDL>(static_cast<DUIntGDL*>(p0)->Dup(), nanInt);
                case LONG: return product_cu_template<DLongGDL>(static_cast<DLongGDL*>(p0)->Dup(), nanInt);
                case ULONG: return product_cu_template<DULongGDL>(static_cast<DULongGDL*>(p0)->Dup(), nanInt);
                case LONG64: return product_cu_template<DLong64GDL>(static_cast<DLong64GDL*>(p0)->Dup(), nanInt);
                case ULONG64: return product_cu_template<DULong64GDL>(static_cast<DULong64GDL*>(p0)->Dup(), nanInt);
                case FLOAT: return product_cu_template<DFloatGDL>(static_cast<DFloatGDL*>(p0)->Dup(), KwNaN);
                case DOUBLE: return product_cu_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0)->Dup(), KwNaN);
                case COMPLEX: return product_cu_template<DComplexGDL>(static_cast<DComplexGDL*>(p0)->Dup(), KwNaN);
                case COMPLEXDBL: return product_cu_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0)->Dup(), KwNaN);
                default: assert(false);
              }
            }

	    // Integer parts derivated from Total code by Erin Sheldon
	    // In IDL PRODUCT(), the INTEGER keyword takes precedence 
	    if (KwInt) {
	      // We use LONG64 unless the input is ULONG64
	      if ((p0->Type() == LONG64) && (!KwNaN)) {
		return product_cu_template<DLong64GDL>
		  ( static_cast<DLong64GDL*>(p0)->Dup(), nanInt);
	      }
	      if ((p0->Type() == ULONG64) && (!KwNaN)) {
		return product_cu_template<DULong64GDL>
		  ( static_cast<DULong64GDL*>(p0)->Dup(), nanInt);
	      }
	      // Convert to Long64
	      DLong64GDL* p0L64 = static_cast<DLong64GDL*>
		(p0->Convert2( LONG64, BaseGDL::COPY));
	      auto_ptr<DLong64GDL> guard( p0L64);
	      if (KwNaN) {
		DFloatGDL* p0f = static_cast<DFloatGDL*>
		  (p0->Convert2( FLOAT, BaseGDL::COPY));
		auto_ptr<DFloatGDL> guard( p0f);
		for( SizeT i=0; i<nEl; ++i) {
		  if (!isfinite((*p0f)[i])) (*p0L64)[i]=1;
		}
	      }
	      return product_cu_template<DLong64GDL>
		((p0L64)->Dup(), nanInt);	      
	    } // integer results
	      
	      // special case as DOUBLE type overrides /DOUBLE
	    if (p0->Type() == DOUBLE) {
	      return product_cu_template< DDoubleGDL>
		( static_cast<DDoubleGDL*>(p0)->Dup(), KwNaN);
	    }
	    if (p0->Type() == COMPLEXDBL) {
	      return product_cu_template< DComplexDblGDL>
		( static_cast<DComplexDblGDL*>(p0)->Dup(), KwNaN);
	    }
	    if (p0->Type() == COMPLEX) {
	      return product_cu_template< DComplexDblGDL>
		( static_cast<DComplexDblGDL*>
		  (p0->Convert2( COMPLEXDBL, BaseGDL::COPY)), KwNaN);
	    }
	    return product_cu_template< DDoubleGDL>
	      ( static_cast<DDoubleGDL*>
		(p0->Convert2( DOUBLE, BaseGDL::COPY)), KwNaN);
	  }
    }
    
    // product over sumDim
    dimension srcDim = p0->Dim();
    SizeT srcRank = srcDim.Rank();
    
    if( sumDim < 1 || sumDim > srcRank)
      e->Throw( "Array must have "+i2s(sumDim)+
		" dimensions: "+e->GetParString(0));
    
    if (!KwCumul) {

      if (KwPre) 
      {
        switch (p0->Type())
        {
          case BYTE: return product_over_dim_template<DByteGDL>(static_cast<DByteGDL*>(p0), srcDim, sumDim-1, nanInt);
          case INT: return product_over_dim_template<DIntGDL>(static_cast<DIntGDL*>(p0), srcDim, sumDim-1, nanInt);
          case UINT: return product_over_dim_template<DUIntGDL>(static_cast<DUIntGDL*>(p0), srcDim, sumDim-1, nanInt);
          case LONG: return product_over_dim_template<DLongGDL>(static_cast<DLongGDL*>(p0), srcDim, sumDim-1, nanInt);
          case ULONG: return product_over_dim_template<DULongGDL>(static_cast<DULongGDL*>(p0), srcDim, sumDim-1, nanInt);
          case LONG64: return product_over_dim_template<DLong64GDL>(static_cast<DLong64GDL*>(p0), srcDim, sumDim-1, nanInt);
          case ULONG64: return product_over_dim_template<DULong64GDL>(static_cast<DULong64GDL*>(p0), srcDim, sumDim-1, nanInt);
          case FLOAT: return product_over_dim_template<DFloatGDL>(static_cast<DFloatGDL*>(p0), srcDim, sumDim-1, KwNaN);
          case DOUBLE: return product_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0), srcDim, sumDim-1, KwNaN);
          case COMPLEX: return product_over_dim_template<DComplexGDL>(static_cast<DComplexGDL*>(p0), srcDim, sumDim-1, KwNaN);
          case COMPLEXDBL: return product_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0), srcDim, sumDim-1, KwNaN);
          default: assert(false);
        }
      }

      // Integer parts derivated from Total code by Erin Sheldon
      // In IDL PRODUCT(), the INTEGER keyword takes precedence 
      if (KwInt) {	  
	// We use LONG64 unless the input is ULONG64
	if ((p0->Type() == LONG64 ) && (!KwNaN)) {
	  return product_over_dim_template<DLong64GDL>
	    ( static_cast<DLong64GDL*>(p0), srcDim, sumDim-1, nanInt);
	}
	if ((p0->Type() == ULONG64) && (!KwNaN)) {
	  return product_over_dim_template<DULong64GDL>
	    ( static_cast<DULong64GDL*>(p0), srcDim, sumDim-1, nanInt);
	}
	
	// Conver to Long64
	DLong64GDL* p0L64 = static_cast<DLong64GDL*>
	  (p0->Convert2( LONG64, BaseGDL::COPY));
	auto_ptr<DLong64GDL> guard( p0L64);
	if (KwNaN) {
	  DFloatGDL* p0f = static_cast<DFloatGDL*>
	    (p0->Convert2( FLOAT, BaseGDL::COPY));
	  auto_ptr<DFloatGDL> guard( p0f);
	  for( SizeT i=0; i<nEl; ++i) {
	    if (!isfinite((*p0f)[i])) (*p0L64)[i]=1;
	  }
	}
	return product_over_dim_template<DLong64GDL>
	  ( p0L64, srcDim, sumDim-1, nanInt);
      } // integer results
      
      if( p0->Type() == DOUBLE) {
	return product_over_dim_template< DDoubleGDL>
	  ( static_cast<DDoubleGDL*>(p0), srcDim, sumDim-1, KwNaN);
      }
      if( p0->Type() == COMPLEXDBL) {
	return product_over_dim_template< DComplexDblGDL>
	  ( static_cast<DComplexDblGDL*>(p0), srcDim, sumDim-1, KwNaN);
      }
      if( p0->Type() == COMPLEX) {
	DComplexDblGDL* p0D = static_cast<DComplexDblGDL*>
	  (p0->Convert2( COMPLEXDBL,BaseGDL::COPY));
	auto_ptr<DComplexDblGDL> p0D_guard( p0D);
	//	    p0D_guard.reset( p0D);
	return product_over_dim_template< DComplexDblGDL>
	  ( p0D, srcDim, sumDim-1, KwNaN);
      }
	
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	(p0->Convert2( DOUBLE,BaseGDL::COPY));
      auto_ptr<DDoubleGDL> p0D_guard( p0D);
      //p0D_guard.reset( p0D);
      return product_over_dim_template< DDoubleGDL>
	( p0D, srcDim, sumDim-1,KwNaN);
    } 
    else
      { // KwCumul

        if (KwPre) 
        {
          switch (p0->Type())
          {
            case BYTE: return product_over_dim_cu_template<DByteGDL>(static_cast<DByteGDL*>(p0)->Dup(), sumDim-1, nanInt);
            case INT: return product_over_dim_cu_template<DIntGDL>(static_cast<DIntGDL*>(p0)->Dup(), sumDim-1, nanInt);
            case UINT: return product_over_dim_cu_template<DUIntGDL>(static_cast<DUIntGDL*>(p0)->Dup(), sumDim-1, nanInt);
            case LONG: return product_over_dim_cu_template<DLongGDL>(static_cast<DLongGDL*>(p0)->Dup(), sumDim-1, nanInt);
            case ULONG: return product_over_dim_cu_template<DULongGDL>(static_cast<DULongGDL*>(p0)->Dup(), sumDim-1, nanInt);
            case LONG64: return product_over_dim_cu_template<DLong64GDL>(static_cast<DLong64GDL*>(p0)->Dup(), sumDim-1, nanInt);
            case ULONG64: return product_over_dim_cu_template<DULong64GDL>(static_cast<DULong64GDL*>(p0)->Dup(), sumDim-1, nanInt);
            case FLOAT: return product_over_dim_cu_template<DFloatGDL>(static_cast<DFloatGDL*>(p0)->Dup(), sumDim-1, KwNaN);
            case DOUBLE: return product_over_dim_cu_template<DDoubleGDL>(static_cast<DDoubleGDL*>(p0)->Dup(), sumDim-1, KwNaN);
            case COMPLEX: return product_over_dim_cu_template<DComplexGDL>(static_cast<DComplexGDL*>(p0)->Dup(), sumDim-1, KwNaN);
            case COMPLEXDBL: return product_over_dim_cu_template<DComplexDblGDL>(static_cast<DComplexDblGDL*>(p0)->Dup(), sumDim-1, KwNaN);
            default: assert(false);
          }
        }

	// Integer parts derivated from Total code by Erin Sheldon
	// In IDL PRODUCT(), the INTEGER keyword takes precedence 
	if (KwInt) {
	  // We use LONG64 unless the input is ULONG64
	  if ((p0->Type() == LONG64) && (!KwNaN)) {
	  return product_over_dim_cu_template<DLong64GDL>
	    ( static_cast<DLong64GDL*>(p0)->Dup(), sumDim-1, nanInt);
	}
	if ((p0->Type() == ULONG64 ) && (!KwNaN)) {
	  return product_over_dim_cu_template<DULong64GDL>
	    ( static_cast<DULong64GDL*>(p0)->Dup(), sumDim-1, nanInt);
	}
	  
	// Convert to Long64
	if (KwNaN) {
	  DFloatGDL* p0f = static_cast<DFloatGDL*>
	    (p0->Convert2( FLOAT, BaseGDL::COPY));
	  auto_ptr<DFloatGDL> guard( p0f);
	  for( SizeT i=0; i<nEl; ++i) {
	    if (!isfinite((*p0f)[i])) (*p0f)[i]=1;
	  }
	  return product_over_dim_cu_template<DLong64GDL>
	    ( static_cast<DLong64GDL*>
	      (p0f->Convert2( LONG64, BaseGDL::COPY)), sumDim-1, nanInt);  
	} else {
	  return product_over_dim_cu_template<DLong64GDL>
	    ( static_cast<DLong64GDL*>
	      (p0->Convert2( LONG64, BaseGDL::COPY)), sumDim-1, nanInt);
	}
	} // integer results
	
	if( p0->Type() == DOUBLE) {
	  return product_over_dim_cu_template< DDoubleGDL>
	    ( static_cast<DDoubleGDL*>(p0)->Dup(), sumDim-1, KwNaN);
	}
	if( p0->Type() == COMPLEXDBL) {
	  return product_over_dim_cu_template< DComplexDblGDL>
	    ( static_cast<DComplexDblGDL*>(p0)->Dup(), sumDim-1, KwNaN);
	}
	if( p0->Type() == COMPLEX) {
	  return product_over_dim_cu_template< DComplexDblGDL>
	    ( static_cast<DComplexDblGDL*>
	      (p0->Convert2( COMPLEXDBL, BaseGDL::COPY)), sumDim-1, KwNaN);
	}
      
	return product_over_dim_cu_template< DDoubleGDL>
	  ( static_cast<DDoubleGDL*>
	    (p0->Convert2( DOUBLE, BaseGDL::COPY)), sumDim-1, KwNaN);
      }
  }

  BaseGDL* array_equal( EnvT* e)
  {
    e->NParam( 2);//, "ARRAY_EQUAL");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ARRAY_EQUAL");
    BaseGDL* p1 = e->GetParDefined( 1);//, "ARRAY_EQUAL");

    if( p0 == p1) return new DByteGDL( 1);

    SizeT nEl0 = p0->N_Elements();
    SizeT nEl1 = p1->N_Elements();
    if( nEl0 != nEl1 && nEl0 != 1 && nEl1 != 1)
      return new DByteGDL( 0);
    
    auto_ptr<BaseGDL> p0_guard;
    auto_ptr<BaseGDL> p1_guard;
    if( p0->Type() != p1->Type())
      {
	if( e->KeywordSet( 0)) // NO_TYPECONV
	  return new DByteGDL( 0);
	else
	  {
	    DType aTy=p0->Type();
	    DType bTy=p1->Type();
	    if( DTypeOrder[aTy] >= DTypeOrder[bTy])
	      {
		p1 = p1->Convert2( aTy, BaseGDL::COPY);
		p1_guard.reset( p1);
	      }
	    else
	      {
		p0 = p0->Convert2( bTy, BaseGDL::COPY);
		p0_guard.reset( p0);
	      }
	  }
      }
    
    if( p0->ArrayEqual( p1)) return new DByteGDL( 1);

    return new DByteGDL( 0);
  }

  BaseGDL* min_fun( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);
    BaseGDL* searchArr = e->GetParDefined( 0);

    bool omitNaN = e->KeywordSet( "NAN");

    static int subIx = e->KeywordIx("SUBSCRIPT_MAX");
    bool subMax = e->KeywordPresent(subIx);  
    
    static int dimIx = e->KeywordIx("DIMENSION");
    bool dimSet = e->KeywordSet(dimIx);

    static int maxIx = e->KeywordIx("MAX");
    bool maxSet = e->KeywordPresent(maxIx);

    DLong searchDim; 
    if (dimSet) {
      e->AssureLongScalarKW(dimIx, searchDim);
      if (searchDim < 0 || searchDim > searchArr->Rank())
        e->Throw("Illegal keyword value for DIMENSION");
    }

    if (dimSet && searchArr->Rank() > 1) 
    {
      searchDim -= 1; // user-supplied dimensions start with 1!

      // here destDim is in fact the srcDim...
      dimension destDim = searchArr->Dim();
      SizeT searchStride = destDim.Stride(searchDim);
      SizeT outerStride = destDim.Stride(searchDim + 1);
      // ... and now becomes the destDim
      SizeT nSearch = destDim.Remove(searchDim);
      SizeT searchLimit = nSearch * searchStride;
      SizeT nEl = searchArr->N_Elements();

      // memory allocation
      BaseGDL *maxVal, *resArr = searchArr->New(destDim, BaseGDL::NOZERO);
      DLongGDL *minElArr, *maxElArr;

      if (maxSet) 
      {
        e->AssureGlobalKW(maxIx); // instead of using a guard pointer
        maxVal = searchArr->New(destDim, BaseGDL::NOZERO);
      }

      if (subMax) 
      { 
        e->AssureGlobalKW(subIx); // instead of using a guard pointer
        maxElArr = new DLongGDL(destDim);
      }

      if (nParam == 2) 
      {
        e->AssureGlobalPar(1);    // instead of using a guard pointer
        minElArr = new DLongGDL(destDim);
      }

      SizeT rIx = 0;
      for (SizeT o = 0; o < nEl; o += outerStride) for (SizeT i = 0; i < searchStride; ++i)
      {
        searchArr->MinMax(
          (nParam == 2 ? &((*minElArr)[rIx]) : NULL), 
          (subMax      ? &((*maxElArr)[rIx]) : NULL), 
          &resArr, 
          (maxSet      ? &maxVal             : NULL), 
          omitNaN, o + i, searchLimit + o + i, searchStride, rIx
        );
        rIx++;
      }

      if (nParam == 2) e->SetPar(1, minElArr);
      if (subMax) e->SetKW(subIx, maxElArr);
      if (maxSet) e->SetKW(maxIx, maxVal);

      return resArr;
    } 
    else 
    {
      DLong minEl;
      BaseGDL* res;

      if (maxSet) // MAX keyword given
      {
        e->AssureGlobalKW( 0);
        delete e->GetKW( 0);
        DLong maxEl;
        searchArr->MinMax( &minEl, &maxEl, &res, &e->GetKW( 0), omitNaN);
        if (subMax) e->SetKW(subIx, new DLongGDL(maxEl));
      }
      else // no MAX keyword
      {
        if (subMax)
        {
          DLong maxEl;
          searchArr->MinMax( &minEl, &maxEl, &res, NULL, omitNaN);
          e->SetKW(subIx, new DLongGDL(maxEl));
        }
        else searchArr->MinMax(&minEl, NULL, &res, NULL, omitNaN);
      }
    
      // handle index
      if (nParam == 2) e->SetPar(1, new DLongGDL( minEl));
      else SysVar::SetC( minEl);
      return res;
    }
  }

  BaseGDL* max_fun( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);
    BaseGDL* searchArr = e->GetParDefined( 0);

    bool omitNaN = e->KeywordSet( "NAN");

    static int subIx = e->KeywordIx("SUBSCRIPT_MIN");
    bool subMin = e->KeywordPresent(subIx);  

    static int dimIx = e->KeywordIx("DIMENSION");
    bool dimSet = e->KeywordSet(dimIx);

    static int minIx = e->KeywordIx("MIN");
    bool minSet = e->KeywordPresent(minIx);

    DLong searchDim; 
    if (dimSet) 
    {
      e->AssureLongScalarKW(dimIx, searchDim);
      if (searchDim < 0 || searchDim > searchArr->Rank())
        e->Throw("Illegal keyword value for DIMENSION");
    }

    if (dimSet && searchArr->Rank() > 1) 
    {
      searchDim -= 1; // user-supplied dimensions start with 1!

      // here destDim is in fact the srcDim...
      dimension destDim = searchArr->Dim();
      SizeT searchStride = destDim.Stride(searchDim);
      SizeT outerStride = destDim.Stride(searchDim + 1);
      // ... and now becomes the destDim
      SizeT nSearch = destDim.Remove(searchDim);
      SizeT searchLimit = nSearch * searchStride;
      SizeT nEl = searchArr->N_Elements();

      // memory allocation
      BaseGDL *minVal, *resArr = searchArr->New(destDim, BaseGDL::NOZERO);
      DLongGDL *minElArr, *maxElArr;

      if (minSet) 
      {    
        e->AssureGlobalKW(minIx); // instead of using a guard pointer
        minVal = searchArr->New(destDim, BaseGDL::NOZERO);
      }    

      if (subMin) 
      {    
        e->AssureGlobalKW(subIx); // instead of using a guard pointer
        minElArr = new DLongGDL(destDim);
      }    

      if (nParam == 2) 
      {    
        e->AssureGlobalPar(1);    // instead of using a guard pointer
        maxElArr = new DLongGDL(destDim);
      }

      SizeT rIx = 0;
      for (SizeT o = 0; o < nEl; o += outerStride) for (SizeT i = 0; i < searchStride; ++i)
      {
        searchArr->MinMax(
          (subMin      ? &((*minElArr)[rIx]) : NULL),
          (nParam == 2 ? &((*maxElArr)[rIx]) : NULL),
          (minSet      ? &minVal             : NULL),
          &resArr,
          omitNaN, o + i, searchLimit + o + i, searchStride, rIx
        );
        rIx++;
      }

      if (nParam == 2) e->SetPar(1, maxElArr);
      if (subMin) e->SetKW(subIx, minElArr);
      if (minSet) e->SetKW(minIx, minVal);

      return resArr;
    }
    else 
    {
      DLong maxEl;
      BaseGDL* res;

      if (minSet) // MIN keyword given
      {
        e->AssureGlobalKW( 0);
        delete e->GetKW( 0);
        DLong minEl;
        searchArr->MinMax( &minEl, &maxEl, &e->GetKW( 0), &res, omitNaN);
	if (subMin) e->SetKW(subIx, new DLongGDL(minEl));
      }
      else // no MIN keyword
      {
	if (subMin)
        {
          DLong minEl;
          searchArr->MinMax( &minEl, &maxEl, NULL, &res, omitNaN);
          e->SetKW(subIx, new DLongGDL(minEl));
        }
	else searchArr->MinMax(NULL, &maxEl, NULL, &res, omitNaN);
      }

      // handle index
      if (nParam == 2) e->SetPar(1, new DLongGDL( maxEl));
      else SysVar::SetC(maxEl);
      return res;
    }
  }
 
BaseGDL* transpose( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 

    BaseGDL* p0 = e->GetParDefined( 0);
    if( p0->Type() == STRUCT)
      e->Throw("Struct expression not allowed in this context: "+
	       e->GetParString(0));
    
    SizeT rank = p0->Rank();
    if( rank == 0)
      e->Throw( "Expression must be an array "
		"in this context: "+ e->GetParString(0));
    
    if( nParam == 2) 
      {
 
	BaseGDL* p1 = e->GetParDefined( 1);
	if( p1->N_Elements() != rank)
	  e->Throw("Incorrect number of elements in permutation.");

	DUInt* perm = new DUInt[rank];
	auto_ptr<DUInt> perm_guard( perm);

	DUIntGDL* p1L = static_cast<DUIntGDL*>
	  (p1->Convert2( UINT, BaseGDL::COPY));
	for( SizeT i=0; i<rank; ++i) perm[i] = (*p1L)[ i];
	delete p1L;

	// check permutaion vector
	for( SizeT i=0; i<rank; ++i) 
	  {
	    DUInt j;
	    for( j=0; j<rank; ++j) if( perm[j] == i) break;
	    if (j == rank)
	      e->Throw( "Incorrect permutation vector.");
	  }
	return p0->Transpose( perm);
      }

    return p0->Transpose( NULL);
  }


// BaseGDL* matrix_multiply( EnvT* e)
//   {
//     SizeT nParam=e->NParam( 2); 
// 
//     BaseGDL* a = e->GetNumericArrayParDefined( 0);
//     BaseGDL* b = e->GetNumericArrayParDefined( 1);
//     
//     static int aTIx = e->KeywordIx("ATRANSPOSE");
//     bool aT = e->KeywordPresent(aTIx);
//     static int bTIx = e->KeywordIx("BTRANSPOSE");
//     bool bT = e->KeywordPresent(bTIx);
//     
//     static int strassenIx = e->KeywordIx("STRASSEN_ALGORITHM");
//     bool strassen = e->KeywordPresent(strassenIx);
// 
//     
//     if( p1->N_Elements() != rank)
// 	  e->Throw("Incorrect number of elements in permutation.");
// 
// 	DUInt* perm = new DUInt[rank];
// 	auto_ptr<DUInt> perm_guard( perm);
// 
// 	DUIntGDL* p1L = static_cast<DUIntGDL*>
// 	  (p1->Convert2( UINT, BaseGDL::COPY));
// 	for( SizeT i=0; i<rank; ++i) perm[i] = (*p1L)[ i];
// 	delete p1L;
// 
// 	// check permutaion vector
// 	for( SizeT i=0; i<rank; ++i) 
// 	  {
// 	    DUInt j;
// 	    for( j=0; j<rank; ++j) if( perm[j] == i) break;
// 	    if (j == rank)
// 	      e->Throw( "Incorrect permutation vector.");
// 	  }
// 	return p0->Transpose( perm);
//       }
// 
//     return a->Transpose( NULL);
//   }

  // helper function for sort_fun, recursive
  // optimized version
  template< typename IndexT>
  void MergeSortOpt( BaseGDL* p0, IndexT* hhS, IndexT* h1, IndexT* h2,
		     SizeT len) 
  {
    if( len <= 1) return;       

    SizeT h1N = len / 2;
    SizeT h2N = len - h1N;

    // 1st half
    MergeSortOpt(p0, hhS, h1, h2, h1N);

    // 2nd half
    IndexT* hhM = &hhS[h1N]; 
    MergeSortOpt(p0, hhM, h1, h2, h2N);

    SizeT i;
    for(i=0; i<h1N; ++i) h1[i] = hhS[ i];
    for(i=0; i<h2N; ++i) h2[i] = hhM[ i];

    SizeT  h1Ix = 0;
    SizeT  h2Ix = 0;
    for( i=0; (h1Ix < h1N) && (h2Ix < h2N); ++i) 
      {
	// the actual comparisson
	if( p0->Greater( h1[h1Ix], h2[h2Ix])) 
	  hhS[ i] = h2[ h2Ix++];
	else
	  hhS[ i] = h1[ h1Ix++];
      }
    for(; h1Ix < h1N; ++i) hhS[ i] = h1[ h1Ix++];
    for(; h2Ix < h2N; ++i) hhS[ i] = h2[ h2Ix++];
  }

  // helper function for sort_fun, recursive
  void MergeSort( BaseGDL* p0, SizeT* hh, SizeT* h1, SizeT* h2,
		  SizeT start, SizeT end) 
  {
    if( start+1 >= end) return;       

    SizeT middle = (start+end) / 2;

    MergeSort(p0, hh, h1, h2, start, middle);
    MergeSort(p0, hh, h1, h2, middle, end);

    SizeT h1N = middle - start;
    SizeT h2N = end - middle;

    SizeT* hhS = &hh[start];

    SizeT i;
    for(i=0; i<h1N; ++i) h1[i] = hhS[ i];
    for(i=0; i<h2N; ++i) h2[i] = hh[middle + i];

    SizeT  h1Ix = 0;
    SizeT  h2Ix = 0;
    for( i=0; (h1Ix < h1N) && (h2Ix < h2N); ++i) 
      {
	// the actual comparisson
	if( p0->Greater( h1[h1Ix], h2[h2Ix])) 
	  hhS[ i] = h2[ h2Ix++];
	else
	  hhS[ i] = h1[ h1Ix++];
      }
    for(; h1Ix < h1N; ++i) hhS[ i] = h1[ h1Ix++];
    for(; h2Ix < h2N; ++i) hhS[ i] = h2[ h2Ix++];
  }

  // sort function uses MergeSort
  BaseGDL* sort_fun( EnvT* e)
  {
    e->NParam( 1);
    
    BaseGDL* p0 = e->GetParDefined( 0);

    if( p0->Type() == STRUCT)
      e->Throw( "Struct expression not allowed in this context: "+
		e->GetParString(0));
    
    static int l64Ix = e->KeywordIx( "L64");
    bool l64 = e->KeywordSet( l64Ix);
    
    SizeT nEl = p0->N_Elements();
    
    // helper arrays
    DLongGDL* res = new DLongGDL( dimension( nEl), BaseGDL::INDGEN);

	DLong nanIx = nEl;
    if( p0->Type() == FLOAT)
    {
		DFloatGDL* p0F = static_cast<DFloatGDL*>(p0);
		for( DLong i=nEl-1; i >= 0; --i)
		{
			if( isnan((*p0F)[ i]) )//|| !isfinite((*p0F)[ i]))
				{
					--nanIx;
					(*res)[i] = (*res)[nanIx];
					(*res)[ nanIx] = i;

// cout << "swap " << i << " with " << nanIx << endl;
// cout << "now:     ";
// 		for( DLong ii=0; ii < nEl; ++ii)
// 		{
// 		cout << (*res)[ii] << " ";		
// 		}
// cout  << endl;
				}
		}
    }
    else if( p0->Type() == DOUBLE)
    {
		DDoubleGDL* p0F = static_cast<DDoubleGDL*>(p0);
		for( DLong i=nEl-1; i >= 0; --i)
		{
			if( isnan((*p0F)[ i]))// || !isfinite((*p0F)[ i]))
				{
					--nanIx;
					(*res)[i] = (*res)[nanIx];
					(*res)[ nanIx] = i;
				}
		}
    }
    else if( p0->Type() == COMPLEX)
    {
		DComplexGDL* p0F = static_cast<DComplexGDL*>(p0);
		for( DLong i=nEl-1; i >= 0; --i)
		{
			if( isnan((*p0F)[ i].real()) || //!isfinite((*p0F)[ i].real()) ||
			     isnan((*p0F)[ i].imag()))// || !isfinite((*p0F)[ i].imag()) )
				{
					--nanIx;
					(*res)[i] = (*res)[nanIx];
					(*res)[ nanIx] = i;
				}
		}
    }
    else if( p0->Type() == COMPLEXDBL)
    {
		DComplexDblGDL* p0F = static_cast<DComplexDblGDL*>(p0);
		for( DLong i=nEl-1; i >= 0; --i)
		{
			if( isnan((*p0F)[ i].real()) || //!isfinite((*p0F)[ i].real()) ||
			     isnan((*p0F)[ i].imag()))// || !isfinite((*p0F)[ i].imag()) )
				{
					--nanIx;
					(*res)[i] = (*res)[nanIx];
					(*res)[ nanIx] = i;
				}
		}
    }

// cout << "nEl " << nEl << " nanIx " << nanIx << endl;
	nEl = nanIx;
// cout << "sorting:  ";
// 		for( DLong ii=0; ii < nEl; ++ii)
// 		{
// 		cout << (*res)[ii] << " ";		
// 		}
// cout  << endl;

    DLong *hh = static_cast<DLong*>(res->DataAddr());

    DLong* h1 = new DLong[ nEl/2];
    DLong* h2 = new DLong[ (nEl+1)/2];
    // call the sort routine
    MergeSortOpt<DLong>( p0, hh, h1, h2, nEl);
    delete[] h1;
    delete[] h2;

    if( l64) 
      {
	// leave it this way, as sorting of more than 2^31
	// items seems not feasible in the future we might 
	// use MergeSortOpt<DLong64>(...) for this 
	return res->Convert2( LONG64);
      }

    return res;
  }

  // uses MergeSort
  BaseGDL* median( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);
    
    if( !(nParam > 1))
      {
	BaseGDL* p0 = e->GetParDefined( 0);

	if( p0->Type() == PTR)
	  e->Throw( "Pointer expression not allowed in this context: "+
		    e->GetParString(0));
	if( p0->Type() == OBJECT)
	  e->Throw( "Object expression not allowed in this context: "+
		    e->GetParString(0));
	if( p0->Type() == STRUCT)
	  e->Throw( "Struct expression not allowed in this context: "+
		    e->GetParString(0));

	if( p0->Rank() == 0)
	  e->Throw( "Expression must be an array in this context: "+
		    e->GetParString(0));
    
	SizeT nEl = p0->N_Elements();
	
	static int evenIx = e->KeywordIx( "EVEN");
	bool dbl = 
          p0->Type() == DOUBLE || 
          p0->Type() == COMPLEXDBL || 
          e->KeywordSet(e->KeywordIx("DOUBLE"));
    DType type = dbl ? DOUBLE : FLOAT;
    bool noconv = (dbl && p0->Type() == DOUBLE) || (!dbl && p0->Type() == FLOAT);

	// DIMENSION keyword
	DLong dim = 0;
	DLong nmed = 1;
	BaseGDL *res;
	e->AssureLongScalarKWIfPresent( "DIMENSION", dim);
	
	if (dim > p0->Rank())
	  e->Throw( "Illegal keyword value for DIMENSION.");
	
	if (dim > 0) {
	  DLong dims[8];
	  DLong k = 0;
	  for (SizeT i=0; i<p0->Rank(); ++i)
	    if (i != (dim-1)) {
	      nmed *= p0->Dim(i);
	      dims[k++] = p0->Dim(i);
	    }
	  dimension dimRes((DLong *) dims, p0->Rank()-1);
	  res = dbl 
            ? static_cast<BaseGDL*>(new DDoubleGDL(dimRes, BaseGDL::NOZERO)) 
            : static_cast<BaseGDL*>(new DFloatGDL(dimRes, BaseGDL::NOZERO));
	} else {
	  res = dbl 
            ? static_cast<BaseGDL*>(new DDoubleGDL(1)) 
            : static_cast<BaseGDL*>(new DFloatGDL(1));
	}
	
	// helper arrays
	if (nmed > 1) nEl = p0->N_Elements() / nmed;
	
	DLong *hh = new DLong[ nEl];
	DLong* h1 = new DLong[ nEl/2];
	DLong* h2 = new DLong[ (nEl+1)/2];

	DLong accumStride = 1;
	if (nmed > 1)
	  for( DLong i=0; i<dim-1; ++i) accumStride *= p0->Dim(i);
	
	BaseGDL *op1, *op2, *op3;
        if (dbl) op3 = new DDoubleGDL(2);
        else op3 = new DFloatGDL(2);

	// Loop over all subarray medians
	for (SizeT k=0; k<nmed; ++k) {
	  
	  if (nmed == 1) { 
	    for( DLong i=0; i<nEl; ++i) hh[i] = i;

		DLong nanIx = nEl;
		if( p0->Type() == FLOAT)
		{
			DFloatGDL* p0F = static_cast<DFloatGDL*>(p0);
			for( DLong i=nEl-1; i >= 0; --i)
			{
				if( isnan((*p0F)[ i]) )//|| !isfinite((*p0F)[ i]))
					{
						--nanIx;
						hh[i] = hh[nanIx];
						hh[ nanIx] = i;

	// cout << "swap " << i << " with " << nanIx << endl;
	// cout << "now:     ";
	// 		for( DLong ii=0; ii < nEl; ++ii)
	// 		{
	// 		cout << hh[ii] << " ";
	// 		}
	// cout  << endl;
					}
			}
		}
		else if( p0->Type() == DOUBLE)
		{
			DDoubleGDL* p0F = static_cast<DDoubleGDL*>(p0);
			for( DLong i=nEl-1; i >= 0; --i)
			{
				if( isnan((*p0F)[ i]))// || !isfinite((*p0F)[ i]))
					{
						--nanIx;
						hh[i] = hh[nanIx];
						hh[ nanIx] = i;
					}
			}
		}
		else if( p0->Type() == COMPLEX)
		{
			DComplexGDL* p0F = static_cast<DComplexGDL*>(p0);
			for( DLong i=nEl-1; i >= 0; --i)
			{
				if( isnan((*p0F)[ i].real()) || //!isfinite((*p0F)[ i].real()) ||
					isnan((*p0F)[ i].imag()))// || !isfinite((*p0F)[ i].imag()) )
					{
						--nanIx;
						hh[i] = hh[nanIx];
						hh[ nanIx] = i;
					}
			}
		}
		else if( p0->Type() == COMPLEXDBL)
		{
			DComplexDblGDL* p0F = static_cast<DComplexDblGDL*>(p0);
			for( DLong i=nEl-1; i >= 0; --i)
			{
				if( isnan((*p0F)[ i].real()) || //!isfinite((*p0F)[ i].real()) ||
					isnan((*p0F)[ i].imag()))// || !isfinite((*p0F)[ i].imag()) )
					{
						--nanIx;
						hh[i] = hh[nanIx];
						hh[ nanIx] = i;
					}
			}
		}

		// cout << "nEl " << nEl << " nanIx " << nanIx << endl;
		nEl = nanIx;

	  } else {
	    // Starting Element
	    DLong start = accumStride * p0->Dim(dim-1) * (k / accumStride) + 
	      (k % accumStride);
	    for( DLong i=0; i<nEl; ++i) hh[i] = start + i * accumStride;
	  }

	  // call the sort routine
	  MergeSortOpt<DLong>( p0, hh, h1, h2, nEl);
	  DLong medEl = hh[ nEl/2];
	  DLong medEl_1 = hh[ nEl/2 - 1];
	  
	  if( (nEl % 2) == 1 || !e->KeywordSet( evenIx)) {
	    if (nmed == 1)
	      res = p0->NewIx(medEl)->Convert2(type, BaseGDL::CONVERT); 
	    else {
              if (noconv) 
              {
                if (dbl) (*static_cast<DDoubleGDL*>(res))[k] = (*static_cast<DDoubleGDL*>(p0))[medEl];
                else (*static_cast<DFloatGDL*>(res))[k] = (*static_cast<DFloatGDL*>(p0))[medEl];
              }
              else 
              {
	        op1 = p0->NewIx(medEl)->Convert2(type, BaseGDL::CONVERT);
	        if (dbl) (*static_cast<DDoubleGDL*>(res))[k] = (*static_cast<DDoubleGDL*>(op1))[0];
                else (*static_cast<DFloatGDL*>(res))[k] = (*static_cast<DFloatGDL*>(op1))[0];
                delete(op1);
              }
	    }
	  } else {
            if (noconv) 
            {
              if (dbl) (*static_cast<DDoubleGDL*>(res))[k] = .5 * (
                (*static_cast<DDoubleGDL*>(p0))[medEl] + 
                (*static_cast<DDoubleGDL*>(p0))[medEl_1]
              );
              else (*static_cast<DFloatGDL*>(res))[k] = .5 * (
                (*static_cast<DFloatGDL*>(p0))[medEl] +
                (*static_cast<DFloatGDL*>(p0))[medEl_1]
              );
            }
            else 
            {
	      op1 = p0->NewIx(medEl)->Convert2(type, BaseGDL::CONVERT); 
	      op2 = p0->NewIx(medEl_1)->Convert2(type, BaseGDL::CONVERT);
	      if (nmed == 1) res = op2->Add(op1)->Div(op3); // TODO: leak with res?
	      else 
              {
                if (dbl) (*static_cast<DDoubleGDL*>(res))[k] = 
                  (*static_cast<DDoubleGDL*>((op2->Add(op1)->Div(op3))))[0];
                else (*static_cast<DFloatGDL*>(res))[k] =
                  (*static_cast<DFloatGDL*>((op2->Add(op1)->Div(op3))))[0];
	        delete(op2);
              }
	      delete(op1);
            }
	  }
	}
        delete(op3);
	delete[] h1;
	delete[] h2;
	delete[] hh;
	
	return res;
      }
    else 
      // with parameter Width : median filtering with no optimisation,
      //  such as histogram algorithms.
      // Copyright: (C) 2008 by Nicolas Galmiche
      {
	// basic checks on "vector/array" input	
	DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>( 0);	

    	if( p0->Type() == STRUCT)
	  e->Throw( "Struct expression not allowed in this context: "+ e->GetParString(0));	
   	if( p0->Rank() == 0)
	  e->Throw( "Expression must be an array in this context: "+ e->GetParString(0));
   	
	if( p0->Rank() > 2)
	  e->Throw( "Only 1 or 2 dimensions allowed: "+ e->GetParString(0));
   	
	// basic checks on "width" input		
 	DDoubleGDL* p1d = e->GetParAs<DDoubleGDL>(1);
 	
	if (p1d->N_Elements() > 1 || (*p1d)[0] <=0 ) 
	  e->Throw( "Width must be a positive scalar or 1 (positive) element array in this context: "+ e->GetParString(0));
	DLong MaxAllowedWidth=0;
	if (p0->Rank() == 1) MaxAllowedWidth=p0->N_Elements();
	if (p0->Rank() == 2) {
	  MaxAllowedWidth=p0->Dim(0);
	  if (p0->Dim(1) < MaxAllowedWidth) MaxAllowedWidth=p0->Dim(1);	   
	}
	const int debug =0;
	if (debug == 1) {
	  cout << "X dim " << p0->Dim(0) <<endl;
	  cout << "y dim " << p0->Dim(1) <<endl;	  
	  cout << "MaxAllowedWidth " << MaxAllowedWidth <<endl;
	}
	if (!isfinite( (*p1d)[0]))
	  e->Throw("Width must be > 1, and < dimension of array (NaN or Inf)");
	
 	DLongGDL* p1 = e->GetParAs<DLongGDL>(1);	

	DDoubleGDL *tamp = new DDoubleGDL(p0->Dim(),BaseGDL::NOZERO);
	DDouble min=((*p0)[0]);
     	DDouble max=min;
    	 
     	for (SizeT ii=0 ; ii<p0->N_Elements() ; ++ii)
	  {(*tamp)[ii]=(*p0)[ii];
	    if ( (*p0)[ii] < min ) min = ((*p0)[ii]);
	    if ( (*p0)[ii] > max ) max = ((*p0)[ii]);
	  }	
		
   	//---------------------------- END d'acquisistion des paramètres -------------------------------------	

	
	static int evenIx = e->KeywordIx( "EVEN");
    static int doubleIx = e->KeywordIx( "DOUBLE");
	static DStructGDL *Values =  SysVar::Values();                                                
    DDouble d_nan=(*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
	DDouble d_infinity= (*static_cast<DDoubleGDL*>(Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0]; 
 
	//------------------------------ Init variables and allocation ---------------------------------------
	SizeT width=(*p1)[0];
	SizeT N_MaskElem= width*width;
	SizeT larg = p0->Stride(1);
	SizeT haut = p0->Stride(2)/larg;
	SizeT lim= static_cast<SizeT>(round(width/2));
	SizeT init=(lim*larg+lim);
	
	// we don't go further if dimension(s) versus not width OK

	if (debug == 1) {cout << "ici" <<endl;}
	
	if ( p0->Rank() == 1) {
	  if (larg < width || width==1 ) e->Throw( "Width must be > 1, and < width of vector");
	} 
	if ( p0->Rank() == 2) {	
	  if (larg < width || haut < width || width==1) e->Throw("Width must be > 1, and < dimension of array");
	}

	// for 2D arrays, we use the algorithm described in paper
	// from T. Huang, G. Yang, and G. Tang, “A Fast Two-Dimensional Median
	// Filtering Algorithm,” IEEE Trans. Acoust., Speech, Signal Processing,
	// vol. 27, no. 1, pp. 13–18, 1979.

	if ( (e->GetParDefined( 0)->Type() == BYTE ||
	      e->GetParDefined( 0)->Type() == INT  || 
	      e->GetParDefined( 0)->Type() == UINT ||
	      e->GetParDefined( 0)->Type() == LONG ||
	      e->GetParDefined( 0)->Type() == ULONG ||
	      e->GetParDefined( 0)->Type() == LONG64 ||
	      e->GetParDefined( 0)->Type() == ULONG64) &&
	     (haut>1))
	  {
	    SizeT taille=static_cast<SizeT>(abs(max)-min+1);		
	    DDoubleGDL* Histo = new DDoubleGDL(taille,BaseGDL::NOZERO);
	    if (width % 2 ==0)
	      {
		for(SizeT i=0 ; i<haut-2*lim ; ++i)				
		  {
		    SizeT ltmed=0;
		    SizeT med=0;
		    SizeT initial=init+i*larg-lim*larg-lim;
		    for(SizeT pp=0 ; pp<taille;++pp)(*Histo)[pp]=0;	
		    for (SizeT ii=initial ; ii <initial+ width ; ++ii)
		      {	
			for(SizeT yy=0;yy<width;yy++)
			  (*Histo)[static_cast<SizeT>((*p0)[ii+yy*larg]-min)]++;
		      }
		    
		    while (ltmed+(*Histo)[med]<=(N_MaskElem /2))
		      {
			ltmed+= static_cast<SizeT>((*Histo)[med]);
			++med;
		      }
		    if (e->KeywordSet( evenIx))
		      {
			
			SizeT EvenMed=med;
			//if ((*Histo)[EvenMed]==1 || (ltmed!=0 && ltmed !=(N_MaskElem /2) -1))
			if ((*Histo)[EvenMed]==1 || (ltmed!=0 && N_MaskElem /2- ltmed!=1) )
			  {
			    while ((*Histo)[EvenMed-1]==0)
			      {  EvenMed--;}
			    (*tamp)[init+i*larg]=((med+min)+(EvenMed-1+min))/2;
			  }
			else
			  (*tamp)[init+i*larg]=med+min;
		      }
		    else
		      {(*tamp)[init+i*larg]=med+min; }
		    
		    for(SizeT j=init+i*larg +1; j<init+(i+1)*larg-2*lim ;++ j)	
		      {				
			SizeT initMask=j-lim*larg-lim;			
			for(SizeT k=0;k<2*lim;++k)			
			  {	
			    (*Histo)[static_cast<SizeT>((*p0)[initMask-1+k*larg]-min)]--;
			    if ((*p0)[initMask-1+k*larg]-min<med)ltmed--;
				 						
			    (*Histo)[static_cast<SizeT>((*p0)[initMask+k*larg+2*lim-1]-min)]++;
			    if ((*p0)[initMask+k*larg+2*lim-1]-min<med)ltmed++;
			  }
			if (ltmed>N_MaskElem /2)
			  {
			    while(ltmed>N_MaskElem /2)
			      {
				--med;
				ltmed-=static_cast<SizeT>((*Histo)[med]);
			      }
			  }
			else
			  {
			    while (ltmed+(*Histo)[med]<=(N_MaskElem /2))
			      {
				ltmed+= static_cast<SizeT>((*Histo)[med]);
				++med;
			      }	
			  }
			
			if (e->KeywordSet( evenIx))
			  {
			    SizeT EvenMed=med;
			    if ((*Histo)[EvenMed]==1 || (ltmed!=0 &&N_MaskElem /2- ltmed!=1 ))
			      {
				while ((*Histo)[EvenMed-1]==0)
				  {  EvenMed--;}
				(*tamp)[j]=((med+min)+(EvenMed-1+min))/2;
			      }
			    else
			      {(*tamp)[j]=med+min; }
			  }
			else
			  {(*tamp)[j]=med+min; }
		      }
		  } 
	      }
	    else
	      {
		for(SizeT i=0 ; i<haut-2*lim ; ++i)				
		  {
		    SizeT ltmed=0;
		    SizeT med=0;
		    SizeT initial=init+i*larg-lim*larg-lim;
		    for(SizeT pp=0 ; pp<taille;++pp)(*Histo)[pp]=0;	
		    for (SizeT ii=initial ; ii <initial+ width ; ++ii)
		      {	
			for(SizeT yy=0;yy<width;yy++)
			  (*Histo)[static_cast<SizeT>((*p0)[ii+yy*larg]-min)]++;
		      }

		    while (ltmed+(*Histo)[med]<=(N_MaskElem /2))
		      {
			ltmed+= static_cast<SizeT>((*Histo)[med]);
			++med;
		      }
		    (*tamp)[init+i*larg]=med+min;
	
		    for(SizeT j=init+i*larg +1; j<init+(i+1)*larg-2*lim ;++ j)	
		      {	
			
			SizeT initMask=j-lim*larg-lim;			
			for(SizeT k=0;k<=2*lim;++k)			
			  {	
			    (*Histo)[static_cast<SizeT>((*p0)[initMask-1+k*larg]-min)]--;
			    if ((*p0)[initMask-1+k*larg]-min<med)ltmed--;
				 						 						 		
			    (*Histo)[static_cast<SizeT>((*p0)[initMask+k*larg+2*lim]-min)]++;
			    if ((*p0)[initMask+k*larg+2*lim]-min<med)ltmed++;
			  }
			if (ltmed>N_MaskElem /2)
			  {
			    while(ltmed>N_MaskElem /2)
			      {
				--med;
				ltmed-=static_cast<SizeT>((*Histo)[med]);
			      }
			  }
			else
			  {
			    while (ltmed+(*Histo)[med]<=(N_MaskElem /2))
			      {
				ltmed+= static_cast<SizeT>((*Histo)[med]);
				++med;
			      }	
			  }
			
			(*tamp)[j]=med+min;
			
		      }
		  } 
	      }
	
	  }
	else
	  {	
	    DLong* hh; 
	    DLong* h1;
	    DLong* h2;
	    DDoubleGDL* Mask,*Mask1D;
	    if ( p0->Rank() != 1 )
	      {
		hh = new DLong[ N_MaskElem];
    		h1 = new DLong[ N_MaskElem/2];
		h2= new DLong[ (N_MaskElem+1)/2];
		Mask = new DDoubleGDL(N_MaskElem,BaseGDL::NOZERO);
		
		for( DLong i=0; i<N_MaskElem; ++i) hh[i] = i;
	      }
	    else
	      {
		hh = new DLong[ width];
		h1 = new DLong[ width/2];
		h2= new DLong[(width+1)/2];
		Mask1D = new DDoubleGDL(width,BaseGDL::NOZERO);
		
		for( DLong i=0; i<width; ++i) hh[i] = i;
	      }
	
	    //-------------------------------- END OF VARIABLES INIT ---------------------------------------------

	    //------------------------------ Median Filter Algorithms ---------------------------------------
	
	    if ( width % 2 ==0)
	      {
		if ( p0->Rank() == 1 )//------------------------  For a vector with even width -------------------
		  {	
		    for (SizeT col= lim ; col<larg-lim ; ++col)
		      {	
			SizeT ctl_NaN=0;
			SizeT kk=0;
			for (SizeT ind=col-lim ; ind<col+lim ; ++ind)
			  {
			    if( (*p0)[ind]!=d_infinity && (*p0)[ind]!=-d_infinity && isfinite((*p0)[ind])==0)
			      ctl_NaN++;
			    else
			      {	
				(*Mask1D)[kk]=(*p0)[ind];
				kk++;
			      }
			  }
			if (ctl_NaN!=0)
			  {
			    if(ctl_NaN==width)(*tamp)[col]= d_nan;
			    else 
			      {
					DLong*	hhbis = new DLong[ width-ctl_NaN];
					DLong*	h1bis = new DLong[ width-ctl_NaN/2];
					DLong*	h2bis= new DLong[(width-ctl_NaN+1)/2];
					DDoubleGDL *Mask1Dbis = new DDoubleGDL(width-ctl_NaN,BaseGDL::NOZERO);
					for( DLong t=0; t<width-ctl_NaN; ++t) hhbis[t] = t;
					for( DLong ii=0; ii<width-ctl_NaN; ++ii)(*Mask1Dbis)[ii]=(*Mask1D)[ii];
					BaseGDL* besort=static_cast<BaseGDL*>(Mask1Dbis);	
					MergeSortOpt<DLong>( besort, hhbis, h1bis, h2bis,(width - ctl_NaN));
					if (e->KeywordSet( evenIx)&& (width - ctl_NaN) % 2 == 0)
					(*tamp)[col]=((*Mask1Dbis)[hhbis[ (width-ctl_NaN)/2]]+(*Mask1Dbis
												)[hhbis	[ (width - ctl_NaN-1)/2]])/2;
					else
					(*tamp)[col]=(*Mask1Dbis)[hhbis[ (width- ctl_NaN)/2]];
					delete[]hhbis;
					delete[]h2bis;
					delete[]h1bis;
					}
			  }	
			else
			  {
			    BaseGDL* besort=static_cast<BaseGDL*>(Mask1D);	
			    MergeSortOpt<DLong>( besort, hh, h1, h2,width ); // call the sort routine

			    if (e->KeywordSet( evenIx))

			      (*tamp)[col]=((*Mask1D)[hh[ width/2]]+(*Mask1D)[hh[ (width-1)/2]])/2;
			    else
			      (*tamp)[col]=(*Mask1D)[hh[ width/2]];// replace value by Mask median 
			  }
		      }
			
		  }
		else//------------------------  For an array with even width -------------------
		  {
		    SizeT jj;
		    for(SizeT i=0 ; i<haut-2*lim ; ++i)		// lines to replace
		      {
		 	for(SizeT j=init+i*larg ; j<init+(i+1)*larg-2*lim ; ++j)// elements to replace
			  {
			    SizeT initMask=j-lim*larg-lim;	// left corner of mask
			    SizeT kk=0;
			    SizeT ctl_NaN=0;
			    for(SizeT k=0;k<2*lim;++k)		// lines of mask
			      {	
								
				for(jj=initMask+k*larg ; jj<(initMask+k*larg)+2*lim ; ++jj) // elements of mask
				  {
				    if( (*p0)[jj]!=d_infinity && (*p0)[jj]!=-d_infinity && isfinite((*p0)[jj])==0)
				      ctl_NaN++;
				    else
				      {
					(*Mask)[kk]=(*p0)[jj];
					kk++;
				      }
				  }
			      }
			    if (ctl_NaN!=0)
			      {
				if(ctl_NaN==N_MaskElem)(*tamp)[j]= d_nan;
				else {
				  DLong*	hhb = new DLong[ N_MaskElem-ctl_NaN];
				  DLong*	h1b = new DLong[ (N_MaskElem-ctl_NaN)/2];
				  DLong*	h2b = new DLong[(N_MaskElem-ctl_NaN+1)/2];
				  DDoubleGDL *Maskb = new DDoubleGDL(N_MaskElem-ctl_NaN,BaseGDL::NOZERO);
				  for( DLong t=0; t<N_MaskElem-ctl_NaN; ++t) hhb[t] = t;
				  for( DLong ii=0; ii<N_MaskElem-ctl_NaN; ++ii)(*Maskb)[ii]=(*Mask)[ii];
				  BaseGDL* besort=static_cast<BaseGDL*>(Maskb);	
				  MergeSortOpt<DLong>( besort, hhb, h1b, h2b,(N_MaskElem - ctl_NaN)); 
				  if ((N_MaskElem - ctl_NaN) % 2 == 0 && e->KeywordSet( evenIx))
				    (*tamp)[j]=((*Maskb)[hhb[ (N_MaskElem-ctl_NaN)/2]]+(*Maskb)[hhb 
												[ (N_MaskElem - 
												   ctl_NaN-1)/2]])/2;
				  else
				    (*tamp)[j]=(*Maskb)[hhb[ (N_MaskElem- ctl_NaN)/2]];
				  delete[]hhb;
				  delete[]h2b;
				  delete[]h1b;
				}
			      }	
			    else
			      {
			 	BaseGDL* besort=static_cast<BaseGDL*>(Mask);	
			 	MergeSortOpt<DLong>( besort, hh, h1, h2, N_MaskElem); // call the sort routine
				if (e->KeywordSet( evenIx))
				  (*tamp)[j]=((*Mask)[hh[ N_MaskElem/2]]+(*Mask)[hh[ (N_MaskElem-1)/2]])/2;
				else
				  (*tamp)[j]=(*Mask)[hh[ N_MaskElem/2]];// replace value by median Mask one
			      }
			  }
		      }
		  }
	      }

	    else
	      {
		if ( p0->Rank() == 1 )//------------------------  For a vector with odd width -------------------
	
		  {	
		    for (SizeT col= lim ; col<larg-lim ; ++col)
		      {	
			SizeT kk=0;
			SizeT ctl_NaN=0;
			for (SizeT ind=col-lim ; ind<=col+lim ; ++ind)
			  {if( (*p0)[ind]!=d_infinity && (*p0)[ind]!=-d_infinity && isfinite((*p0)[ind])==0)
			      ctl_NaN++;
			    else{
			      (*Mask1D)[kk]=(*p0)[ind];				
			      kk++;
			    }
			  }
			if (ctl_NaN!=0)
			  {
			    if(ctl_NaN==width)(*tamp)[col]= d_nan;
			    else {
			      DLong*	hhbis = new DLong[ width-ctl_NaN];
			      DLong*	h1bis = new DLong[ width-ctl_NaN/2];
			      DLong*	h2bis= new DLong[(width-ctl_NaN+1)/2];
			      DDoubleGDL *Mask1Dbis = new DDoubleGDL(width-ctl_NaN,BaseGDL::NOZERO);
			      for( DLong t=0; t<width-ctl_NaN; ++t) hhbis[t] = t;
			      for( DLong ii=0; ii<width-ctl_NaN; ++ii)(*Mask1Dbis)[ii]=(*Mask1D)[ii];
			      BaseGDL* besort=static_cast<BaseGDL*>(Mask1Dbis);	
			      MergeSortOpt<DLong>( besort, hhbis, h1bis, h2bis,(width - ctl_NaN)); 
			      if (e->KeywordSet( evenIx)&& (width - ctl_NaN) % 2 == 0)
				(*tamp)[col]=((*Mask1Dbis)[hhbis[ (width-ctl_NaN)/2]]+(*Mask1Dbis
										       )[hhbis	[ (width - ctl_NaN-1)/2]])/2;	
			      else(*tamp)[col]=(*Mask1Dbis)[hhbis[ (width- ctl_NaN)/2]];
			      delete[]hhbis;
			      delete[]h2bis;
			      delete[]h1bis;
			    }
			  }	
			else
			  {
			    BaseGDL* besort=static_cast<BaseGDL*>(Mask1D);	
			    MergeSortOpt<DLong>( besort, hh, h1, h2,width ); // call the sort routine
			    (*tamp)[col]=(*Mask1D)[hh[ (width)/2]];	// replace value by Mask median 
			  }
		      }
		
		  }
	
		else //-----------------------------  For an array with odd width ---------------------------------
		  {
		    SizeT jj;
		    for(SizeT i=0 ; i<haut-2*lim ; ++i)				// lines to replace
		      {
		
		 	SizeT initial=init+i*larg-lim*larg-lim;
			SizeT dd=0;SizeT ctl_NaN_init=0;
			for(SizeT yy=0;yy<width;yy++)
			  {	
			    for (SizeT ii=initial+yy*larg ; ii <initial+ yy*larg+ width; ++ii)
			      {
					
				if( (*p0)[ii]!=d_infinity && (*p0)[ii]!=-d_infinity && isfinite((*p0)[ii])==0)
				  ctl_NaN_init++;
				else
				  (*Mask)[dd]=(*p0)[ii];
				dd++;
			      }
			  }
			SizeT kk=0;

		 	for(SizeT j=init+i*larg ; j<init+(i+1)*larg-2*lim ; ++j)// elements to replace
			  {
			    SizeT initMask=j-lim*larg-lim;			// left corner of mask
			    SizeT kk=0;
			    SizeT ctl_NaN=0;
			    for(SizeT k=0;k<=2*lim;++k)			// lines of mask
			      {	
								
				for(jj=initMask+k*larg ; jj<=(initMask+k*larg)+2*lim ; ++jj) // elements of mask
				  {
				    if( (*p0)[jj]!=d_infinity && (*p0)[jj]!=-d_infinity && isfinite((*p0)[jj])==0)
				      ctl_NaN++;
						
				    else
				      {
					(*Mask)[kk]=(*p0)[jj];
					kk++;
				      }
				  }
				
			      }
			 
			    if (ctl_NaN!=0)
			      {	
					if(ctl_NaN==N_MaskElem)
						(*tamp)[j]= d_nan;
					else {
						DLong*	hhb = new DLong[ N_MaskElem-ctl_NaN];
						DLong*	h1b = new DLong[ (N_MaskElem-ctl_NaN)/2];
						DLong*	h2b= new DLong[(N_MaskElem-ctl_NaN+1)/2];
						DDoubleGDL*Maskb = new DDoubleGDL(N_MaskElem-ctl_NaN,BaseGDL::NOZERO);
						for( DLong t=0; t<N_MaskElem-ctl_NaN; ++t) hhb[t] = t;
						for( DLong ii=0; ii<N_MaskElem-ctl_NaN; ++ii)(*Maskb)[ii]=(*Mask)[ii];
						BaseGDL* besort=static_cast<BaseGDL*>(Maskb);
						MergeSortOpt<DLong>( besort, hhb, h1b, h2b,(N_MaskElem - ctl_NaN));
						if ((N_MaskElem - ctl_NaN) % 2 == 0 && e->KeywordSet( evenIx))
							(*tamp)[j]=((*Maskb)[hhb[ (N_MaskElem-ctl_NaN)/2]]+(*Maskb)[hhb
														[ (N_MaskElem - 
														ctl_NaN-1)/2]])/2;
						else(*tamp)[j]=(*Maskb)[hhb[(N_MaskElem- ctl_NaN)/2]];
						delete[]hhb;
						delete[]h2b;
						delete[]h1b;
					}
			      }	
			    else
			      {
				BaseGDL* besort=static_cast<BaseGDL*>(Mask);	
			 	MergeSortOpt<DLong>( besort, hh, h1, h2, N_MaskElem); // call the sort routine
				(*tamp)[j]=(*Mask)[hh[ (N_MaskElem)/2]];	// replace value by Mask median 
			      }
			  }
		      }
		  }
	      }
	    
	    //--------------------------- END OF MEDIAN FILTER ALOGORITHMS -----------------------------------

	    delete[] h1;
	    delete[] h2;
	    delete[] hh;   	
	  }
       	if ( e->GetParDefined( 0)->Type() == DOUBLE || p0->Type() == COMPLEXDBL ||e->KeywordSet( doubleIx) )
	  return tamp;
	else if (e->GetParDefined( 0)->Type() == BYTE) 
	  return tamp->Convert2(BYTE,BaseGDL::CONVERT);
	
	return tamp->Convert2(FLOAT,BaseGDL::CONVERT);
	
      }// end if

  }// end of median

  BaseGDL* shift_fun( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);

    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nShift = nParam - 1;
    if( nShift == 1)
      {
	DLong s1;
	e->AssureLongScalarPar( 1, s1);

	return p0->CShift( s1);
      }
    
    if( p0->Rank() != nShift)
      e->Throw( "Incorrect number of arguments.");

    DLong sIx[ MAXRANK];
    for( SizeT i=0; i< nShift; i++)
      e->AssureLongScalarPar( i+1, sIx[ i]);

    return p0->CShift( sIx);
  }

  BaseGDL* arg_present( EnvT* e)
  {
    e->NParam( 1);
    
    if( !e->GlobalPar( 0))
      return new DIntGDL( 0);

    EnvBaseT* caller = e->Caller();
    if( caller == NULL)
      return new DIntGDL( 0);

    BaseGDL** pp0 = &e->GetPar( 0);
    
    int ix = caller->FindGlobalKW( pp0);
    if( ix == -1)
      return new DIntGDL( 0);

    return new DIntGDL( 1);
  }

  BaseGDL* eof_fun( EnvT* e)
  {
    e->NParam( 1);

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      return new DIntGDL( 0);

    // nicer error message (Disregard if socket)
    if ( fileUnits[ lun-1].SockNum() == -1) {
      if( !fileUnits[ lun-1].IsOpen())
	throw GDLIOException( e->CallingNode(), "File unit is not open: "+i2s( lun)+".");

      if( fileUnits[ lun-1].Eof())
	return new DIntGDL( 1);
    } else {
      // Socket
      string *recvBuf = &fileUnits[ lun-1].RecvBuf();
      if (recvBuf->size() == 0)
	return new DIntGDL( 1);
    }
    return new DIntGDL( 0);
  }

  BaseGDL* convol( EnvT* e)
  {
    SizeT nParam=e->NParam( 2); 

    BaseGDL* p0 = e->GetNumericParDefined( 0);
    if( p0->Rank() == 0) 
      e->Throw( "Expression must be an array in this context: "+
		e->GetParString(0));
    
    BaseGDL* p1 = e->GetNumericParDefined( 1);
    if( p1->Rank() == 0) 
      e->Throw( "Expression must be an array in this context: "+
		e->GetParString(1));
    
    if( p0->N_Elements() <= p1->N_Elements())
      e->Throw( "Incompatible dimensions for Array and Kernel.");

    // rank 1 for kernel works always
    if( p1->Rank() != 1)
      {
	SizeT rank = p0->Rank();
	if( rank != p1->Rank())
	  e->Throw( "Incompatible dimensions for Array and Kernel.");

	for( SizeT r=0; r<rank; ++r)
	  if( p0->Dim( r) <= p1->Dim( r))
	    e->Throw( "Incompatible dimensions for Array and Kernel.");
      }

    // convert kernel to array type
    auto_ptr<BaseGDL> p1Guard;
    if( p0->Type() == BYTE)
      {
	if( p1->Type() != INT)
	  {
	    p1 = p1->Convert2( INT, BaseGDL::COPY); 
	    p1Guard.reset( p1);
	  }
      }
    else if( p0->Type() != p1->Type())
      {
	p1 = p1->Convert2( p0->Type(), BaseGDL::COPY); 
	p1Guard.reset( p1);
      }

    BaseGDL* scale;
    auto_ptr<BaseGDL> scaleGuard;
    if( nParam > 2)
      {
	scale = e->GetParDefined( 2);
	if( scale->Rank() > 0)
	  e->Throw( "Expression must be a scalar in this context: "+
		    e->GetParString(2));

	// p1 here handles BYTE case also
	if( p1->Type() != scale->Type())
	  {
	    scale = scale->Convert2( p1->Type(),BaseGDL::COPY); 
	    scaleGuard.reset( scale);
	  }
      }
    else
      {
	scale = p1->New( dimension(), BaseGDL::ZERO);
      }

    bool center = true;
    static int centerIx = e->KeywordIx( "CENTER");
    if( e->KeywordPresent( centerIx))
      {
	DLong c;
	e->AssureLongScalarKW( centerIx, c);
	center = (c != 0);
      }

    // overrides EDGE_TRUNCATE
    static int edge_wrapIx = e->KeywordIx( "EDGE_WRAP");
    bool edge_wrap = e->KeywordSet( edge_wrapIx);
    static int edge_truncateIx = e->KeywordIx( "EDGE_TRUNCATE");
    bool edge_truncate = e->KeywordSet( edge_truncateIx);

    int edgeMode = 0; 
    if( edge_wrap)
      edgeMode = 1;
    else if( edge_truncate)
      edgeMode = 2;

    // p0, p1 and scale have same type
    // p1 has rank of 1 or same rank as p0 with each dimension smaller than p0
    // scale is a scalar
    return p0->Convol( p1, scale, center, edgeMode);
  }

  BaseGDL* rebin_fun( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);

    BaseGDL* p0 = e->GetNumericParDefined( 0);

    SizeT rank = p0->Rank();

    if( rank == 0) 
      e->Throw( "Expression must be an array in this context: "+
		e->GetParString(0));
    
    SizeT resDimInit[ MAXRANK];

    DLongGDL* p1 = e->GetParAs<DLongGDL>(1);
    if (p1->Rank() > 0 && nParam > 2) 
      e->Throw("The new dimensions must either be specified as an array or as a set of scalars.");
    SizeT np = p1->Rank() == 0 ? nParam : p1->N_Elements() + 1;

    for( SizeT p=1; p<np; ++p)
      {
	DLong newDim;
	if (p1->Rank() == 0) e->AssureLongScalarPar( p, newDim);
        else newDim = (*p1)[p - 1];

	if( newDim <= 0)
	  e->Throw( "Array dimensions must be greater than 0.");
	
	if( rank >= p)
	  {
	    SizeT oldDim = p0->Dim( p-1);

	    if( newDim > oldDim)
	      {
		if( (newDim % oldDim) != 0)
		  e->Throw( "Result dimensions must be integer factor "
			    "of original dimensions.");
	      }
	    else
	      {
		if( (oldDim % newDim) != 0)
		  e->Throw( "Result dimensions must be integer factor "
			    "of original dimensions.");
	      }
	  }
	
	resDimInit[ p-1] = newDim; 
      }

    dimension resDim( resDimInit, np-1);

    static int sampleIx = e->KeywordIx( "SAMPLE");
    bool sample = e->KeywordSet( sampleIx);
    
    return p0->Rebin( resDim, sample);
  }

  BaseGDL* obj_class( EnvT* e)
  {
    SizeT nParam = e->NParam();

    static int countIx = e->KeywordIx( "COUNT");
    static int superIx = e->KeywordIx( "SUPERCLASS");

    bool super = e->KeywordSet( superIx);

    bool count = e->KeywordPresent( countIx);
    if( count)
      e->AssureGlobalKW( countIx);

    if( nParam > 0)
      {
	BaseGDL* p0 = e->GetParDefined( 0);

	if( p0->Type() != STRING && p0->Type() != OBJECT)
	  e->Throw( "Argument must be a scalar object reference or string: "+
		    e->GetParString(0));

	if( !p0->Scalar())
	  e->Throw( "Expression must be a scalar or 1 element "
		    "array in this context: "+e->GetParString(0));

	DStructDesc* objDesc;

	if( p0->Type() == STRING)
	  {
	    DString objName;
	    e->AssureScalarPar<DStringGDL>( 0, objName);
	    objName = StrUpCase( objName);

	    objDesc = FindInStructList( structList, objName);
	    if( objDesc == NULL)
	      {
		if( count)
		  e->SetKW( countIx, new DLongGDL( 0));
		return new DStringGDL( "");
	      }
	  }
	else // OBJECT
	  {
	    DObj objRef;
	    e->AssureScalarPar<DObjGDL>( 0, objRef);

	    if( objRef == 0)
	      {
		if( count)
		  e->SetKW( countIx, new DLongGDL( 0));
		return new DStringGDL( "");
	      }

	    DStructGDL* oStruct;
	    try {
	      oStruct = e->GetObjHeap( objRef);
	    }
	    catch ( GDLInterpreter::HeapException)
	      { // non valid object
		if( count)
		  e->SetKW( countIx, new DLongGDL( 0));
		return new DStringGDL( "");
	      }

	    objDesc = oStruct->Desc(); // cannot be NULL
	  }

	if( !super)
	  {
	    if( count)
	      e->SetKW( countIx, new DLongGDL( 1));
	    return new DStringGDL( objDesc->Name());
	  }
	
	deque< string> pNames;
	objDesc->GetParentNames( pNames);

	SizeT nNames = pNames.size();
	    
	if( count)
	  e->SetKW( countIx, new DLongGDL( nNames));

	if( nNames == 0)
	  {
	    return new DStringGDL( "");
	  }

	DStringGDL* res = new DStringGDL( dimension( nNames), 
					  BaseGDL::NOZERO);

	for( SizeT i=0; i<nNames; ++i)
	  {
	    (*res)[i] = pNames[i];
	  }
	
	return res;
      }

    if( super)
      e->Throw( "Conflicting keywords.");

    SizeT nObj = structList.size();

    DStringGDL* res = new DStringGDL( dimension( nObj), 
				      BaseGDL::NOZERO);

    for( SizeT i=0; i<nObj; ++i)
      {
	(*res)[i] = structList[i]->Name();
      }
	
    return res;
  }

  BaseGDL* obj_isa( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);

    BaseGDL* p0 = e->GetPar( 0);
    if( p0 == NULL || p0->Type() != OBJECT)
      e->Throw( "Object reference type required in this context: "+
		e->GetParString(0));

    DString className;
    e->AssureScalarPar<DStringGDL>( 1, className);
    className = StrUpCase( className);

    DObjGDL* pObj = static_cast<DObjGDL*>( p0);

    DByteGDL* res = new DByteGDL( pObj->Dim()); // zero 

    GDLInterpreter* interpreter = e->Interpreter();

    SizeT nElem = pObj->N_Elements();
    for( SizeT i=0; i<nElem; ++i)
      {
	if( interpreter->ObjValid( (*pObj)[ i])) 
	  {
	    DStructGDL* oStruct = e->GetObjHeap( (*pObj)[i]);
	    if( oStruct->Desc()->IsParent( className))
	      (*res)[i] = 1;
	  }
      }
    
    return res;
  }

  BaseGDL* n_tags( EnvT* e)
  {
    e->NParam( 1);

    BaseGDL* p0 = e->GetPar( 0);
    if( p0 == NULL)
      return new DLongGDL( 0);
    
    if( p0->Type() != STRUCT)
      return new DLongGDL( 0);
    
    DStructGDL* s = static_cast<DStructGDL*>( p0);

    //static int lengthIx = e->KeywordIx( "DATA_LENGTH");
    //bool length = e->KeywordSet( lengthIx);
    
    // we don't know now how to distinghuis the 2 following cases
    if(e->KeywordSet("DATA_LENGTH"))
      return new DLongGDL( s->Sizeof());
    
    if(e->KeywordSet("LENGTH"))
      return new DLongGDL( s->Sizeof());

    return new DLongGDL( s->Desc()->NTags());
  }

  BaseGDL* bytscl( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);

    BaseGDL* p0=e->GetNumericParDefined( 0);

    static int minIx = e->KeywordIx( "MIN");
    static int maxIx = e->KeywordIx( "MAX");
    static int topIx = e->KeywordIx( "TOP");
    bool omitNaN = e->KeywordPresent( 3);

    DLong topL=255;
    if( e->GetKW( topIx) != NULL)
      e->AssureLongScalarKW( topIx, topL);
    DByte top = static_cast<DByte>(topL);
    DDouble dTop = static_cast<DDouble>(top);

    DDouble min;
    bool minSet = false;
    // SA: handling 3 parameters to emulate undocumented IDL behaviour 
    //     of translating second and third arguments to MIN and MAX, respectively
    //     (parameters have precedence over keywords)
    if (nParam >= 2)
    {
      e->AssureDoubleScalarPar(1, min);
      minSet = true;
    } 
    else if (e->GetKW(minIx) != NULL)
    {
      e->AssureDoubleScalarKW(minIx, min);
      minSet = true;
    }

    DDouble max;
    bool maxSet = false;
    if (nParam == 3)
    {
      e->AssureDoubleScalarPar(2, max);
      maxSet = true;
    }
    else if (e->GetKW(maxIx) != NULL)
    {
      e->AssureDoubleScalarKW(maxIx, max);
      maxSet = true;
    }

    DDoubleGDL* dRes = 
      static_cast<DDoubleGDL*>(p0->Convert2( DOUBLE, BaseGDL::COPY));

    DLong maxEl, minEl;
    if( !maxSet || !minSet)
      dRes->MinMax( &minEl, &maxEl, NULL, NULL, omitNaN);
    if( !minSet)
      min = (*dRes)[ minEl];
    if( !maxSet)
      max = (*dRes)[ maxEl];

    SizeT nEl = dRes->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	DDouble& d = (*dRes)[ i];
	if( d <= min) (*dRes)[ i] = 0;
	else if( d >= max) (*dRes)[ i] = dTop;
	else
        {
          // SA: floor is used for integer types to simulate manipulation on input data types
          if (IntType(p0->Type())) (*dRes)[ i] = floor(((dTop + 1.)*(d - min) - 1.) / (max-min));
          // SA (?): here floor is used (instead of round) to simulate IDL behaviour
          else (*dRes)[ i] = floor((d - min) / (max-min) * (dTop + .9999));
        }
      }

    return dRes->Convert2( BYTE);
  } 

  BaseGDL* strtok_fun( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);
    
    DString stringIn;
    e->AssureStringScalarPar( 0, stringIn);

    DString pattern = " \t";
    if(nParam > 1) {
      e->AssureStringScalarPar( 1, pattern);
    }
    
    static int extractIx = e->KeywordIx( "EXTRACT");
    bool extract = e->KeywordSet( extractIx);

    static int lengthIx = e->KeywordIx( "LENGTH");
    bool lengthPresent = e->KeywordPresent( lengthIx);

    if( extract && lengthPresent)
      e->Throw( "Conflicting keywords.");
    
    static int pre0Ix = e->KeywordIx( "PRESERVE_NULL");
    bool pre0 = e->KeywordSet( pre0Ix);

    static int regexIx = e->KeywordIx( "REGEX");
    bool regex = e->KeywordPresent( regexIx);
    char err_msg[MAX_REGEXPERR_LENGTH];
    regex_t regexp;
    
    deque<long> tokenStart;
    deque<long> tokenLen;
 
    int strLen = stringIn.length();

    DString escape = "";
    e->AssureStringScalarKWIfPresent( "ESCAPE", escape);
    deque<long> escList;
    long pos = 0;
    while(pos != string::npos)
      {
	pos = stringIn.find_first_of( escape, pos);
	if( pos != string::npos)
	  {
	    escList.push_back( pos+1); // remember escaped char
	    pos += 2; // skip escaped char
	  }
      }
    deque<long>::iterator escBeg = escList.begin();
    deque<long>::iterator escEnd = escList.end();

    long tokB = 0;
    long tokE;
    long nextE = 0;
    long actLen;

    // If regex then compile regex
    if( regex) {
      if (pattern == " \t") pattern = " "; // regcomp doesn't like "\t" JMG
      int compRes = regcomp( &regexp, pattern.c_str(), REG_EXTENDED);
      if (compRes) {
	regerror(compRes, &regexp, err_msg, MAX_REGEXPERR_LENGTH);
	e->Throw(  "Error processing regular expression: "+
			   pattern+"\n           "+string(err_msg)+".");
      }
    }

    for(;;)
      {
	regmatch_t pmatch[1];
	if( regex) {
	  int matchres = regexec( &regexp, stringIn.c_str()+nextE, 1, pmatch, 0);
	  tokE = matchres? -1:pmatch[0].rm_so;
	} else { 
	  tokE = stringIn.find_first_of( pattern, nextE);
	}

	if( tokE == string::npos)
	  {
	    actLen = strLen - tokB;
	    if( actLen > 0 || pre0)
	      {
		tokenStart.push_back( tokB);
		tokenLen.push_back( actLen);
	      }
	    break;
	  }

	if( find( escBeg, escEnd, tokE) == escEnd) 
	  {
	    if (regex) actLen = tokE; else actLen = tokE - tokB;
	    if( actLen > 0 || pre0)
	      {
		tokenStart.push_back( tokB);
		tokenLen.push_back( actLen);
	      }
	    if (regex) tokB += pmatch[0].rm_eo; else tokB = tokE + 1;
	  }
	if (regex) nextE += pmatch[0].rm_eo; else nextE = tokE + 1;
      } // for(;;)

    if (regex) regfree( &regexp);

    SizeT nTok = tokenStart.size();

    if( !extract)
      {    
	if( lengthPresent) 
	  {
	    e->AssureGlobalKW( lengthIx);
	    
	    if( nTok > 0)
	      {
		dimension dim(nTok);
		DLongGDL* len = new DLongGDL(dim);
		for(int i=0; i < nTok; i++)
		  (*len)[i] = tokenLen[i];

		e->SetKW( lengthIx, len);
	      }
	    else
	      {
		e->SetKW( lengthIx, new DLongGDL( 0));
	      }
	  }
	
	if( nTok == 0) return new DLongGDL( 0);
    
	dimension dim(nTok);
	DLongGDL* d = new DLongGDL(dim);
	for(int i=0; i < nTok; i++)
	  (*d)[i] = tokenStart[i];
	return d; 
      } 

    // EXTRACT
    if( nTok == 0) return new DStringGDL( "");

    dimension dim(nTok);
    DStringGDL *d = new DStringGDL(dim);
    for(int i=0; i < nTok; i++) 
      {
	(*d)[i] = stringIn.substr(tokenStart[i], tokenLen[i]);	

	// remove escape
	DString& act = (*d)[i];
	long escPos = act.find_first_of( escape, 0);
	while( escPos != string::npos)
	  {
	    act = act.substr( 0, escPos)+act.substr( escPos+1);
	    escPos = act.find_first_of( escape, escPos+1);
	  }
      }
    return d;
  }

  BaseGDL* getenv_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    static int environmentIx = e->KeywordIx( "ENVIRONMENT" );
    bool environment = e->KeywordSet( environmentIx );
  
    SizeT nEnv; 
    DStringGDL* env;

    if( environment) {

      if(nParam != 0) 
        e->Throw( "Incorrect number of arguments.");

      // determine number of environment entries
      for(nEnv = 0; environ[nEnv] != NULL  ; ++nEnv);

      dimension dim( nEnv );
      env = new DStringGDL(dim);

      // copy stuff into local string array
      for(SizeT i=0; i < nEnv ; ++i)
        (*env)[i] = environ[i];

    } else {

      if(nParam != 1) 
        e->Throw( "Incorrect number of arguments.");

      DStringGDL* name = e->GetParAs<DStringGDL>(0);
      nEnv = name->N_Elements();

      env = new DStringGDL( name->Dim());
 
      // copy the stuff into local string only if param found
      char *resPtr;
      for(SizeT i=0; i < nEnv ; ++i)
	{
	  // handle special environment variables
	  // GDL_TMPDIR, IDL_TMPDIR
	  if( (*name)[i] == "GDL_TMPDIR" || (*name)[i] == "IDL_TMPDIR")
	    {
	      resPtr = getenv((*name)[i].c_str());

	      if( resPtr != NULL)
		(*env)[i] = resPtr;
	      else
		(*env)[i] = SysVar::Dir();

	      AppendIfNeeded( (*env)[i], "/");
	    }
	  else // normal environment variables
	    if( (resPtr = getenv((*name)[i].c_str())) ) 
	      (*env)[i] = resPtr;
	}
    }
    
    return env;
  }

  BaseGDL* tag_names_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();
    DStructGDL* struc= e->GetParAs<DStructGDL>(0);

    static int structureNameIx = e->KeywordIx( "STRUCTURE_NAME" );
    bool structureName = e->KeywordSet( structureNameIx );
    
    DStringGDL* tagNames;

    if(structureName){
        
      if ((*struc).Desc()->Name() != "$truct")
	tagNames =  new DStringGDL((*struc).Desc()->Name());
      else
	tagNames =  new DStringGDL("");

    } else {
      SizeT nTags = (*struc).Desc()->NTags();
    
      tagNames = new DStringGDL(dimension(nTags));
      for(int i=0; i < nTags; ++i)
        (*tagNames)[i] = (*struc).Desc()->TagName(i);
    }

    return tagNames;
  }

  BaseGDL* stregex_fun( EnvT* e)
  {
    SizeT nParam=e->NParam( 2);
    
    DStringGDL* stringExpr= e->GetParAs<DStringGDL>(0);
    DString pattern;
    e->AssureStringScalarPar(1, pattern);

    static int booleanIx = e->KeywordIx( "BOOLEAN" );
    bool booleanKW = e->KeywordSet( booleanIx );

    static int extractIx = e->KeywordIx( "EXTRACT" );
    bool extractKW = e->KeywordSet( extractIx );

    static int foldCaseIx = e->KeywordIx( "FOLD_CASE" );
    bool foldCaseKW = e->KeywordSet( foldCaseIx );

    //XXXpch: this is wrong, should check arg_present
    static int lengthIx = e->KeywordIx( "LENGTH" );
    bool lengthKW = e->KeywordPresent( lengthIx );
//     bool lengthKW = e->KeywordSet( lengthIx );
   
    static int subexprIx = e->KeywordIx( "SUBEXPR" );
    bool subexprKW = e->KeywordSet( subexprIx );
 
    if( booleanKW && (subexprKW || extractKW || lengthKW))
      e->Throw( "Conflicting keywords.");
    //    if( subexprKW) 
    // e->Throw( "Subexpression not yet implemented.");
  
    int nStr = stringExpr->N_Elements();
    dimension dim = stringExpr->Dim();

    BaseGDL* result;

    char err_msg[MAX_REGEXPERR_LENGTH];

    // set the compile flags 
    int cflags = REG_EXTENDED;
    if (foldCaseKW)
      cflags |= REG_ICASE;
    if (booleanKW)
      cflags |= REG_NOSUB;

    //    cout << pattern.c_str() << endl;

    // compile the regular expression
    regex_t regexp;
    int compRes = regcomp( &regexp, pattern.c_str(), cflags);
    SizeT nSubExpr = regexp.re_nsub + 1;
    
    if (compRes) {
      regerror(compRes, &regexp, err_msg, MAX_REGEXPERR_LENGTH);
      e->Throw( "Error processing regular expression: "+
                         pattern+"\n           "+string(err_msg)+".");
    }

    if( booleanKW) 
      result = new DByteGDL(dim);
    else if( extractKW && !subexprKW)
      result = new DStringGDL(dim);
    else if( subexprKW)
      {
	dimension subExprDim = dim;
	subExprDim >> nSubExpr;
	if( extractKW)
	  result = new DStringGDL(subExprDim);
	else
	  result = new DLongGDL(subExprDim);
      }
    else 
      result = new DLongGDL(dim); 

    DLongGDL* len = NULL;
    if( lengthKW) {
      e->AssureGlobalKW( lengthIx);
      if( subexprKW)
	{
	  dimension subExprDim = dim;
	  subExprDim >> nSubExpr;
	  len = new DLongGDL(subExprDim);
	}
      else
	{
	  len = new DLongGDL(dim);
	}
    } 
    
    int nmatch = 1;
    if( subexprKW) nmatch = nSubExpr;

    regmatch_t* pmatch = new regmatch_t[nSubExpr];
    ArrayGuard<regmatch_t> pmatchGuard( pmatch);
 
    for( SizeT s=0; s<dim.N_Elements(); ++s)
      {
	int eflags = 0; 

	for( SizeT sE=0; sE<nSubExpr; ++sE)
	  pmatch[sE].rm_so = -1;

	// now match towards the string
	int matchres = regexec( &regexp, (*stringExpr)[s].c_str(),  nmatch, pmatch, eflags);

	// subexpressions
	if ( extractKW && subexprKW) {

	  // Find number of subexpressions
// 	  SizeT nOut = 0;
// 	  for( SizeT i = 0; i<nmatch; ++i) {
// 	    if ( pmatch[i].rm_so == -1) break;
// 	    nOut++;
// 	  }

	  // Loop through subexpressions & fill output array
	  for( SizeT i = 0; i<nSubExpr; ++i) {
// 	  for( SizeT i = 0; i<=nOut; ++i) {
	    if (pmatch[i].rm_so != -1) (* static_cast<DStringGDL*>(result))[i+s*nSubExpr] = 
	      (*stringExpr)[s].substr( pmatch[i].rm_so, 
				       pmatch[i].rm_eo - pmatch[i].rm_so);
	    if( lengthKW)
	      (*len)[i+s*nSubExpr] = pmatch[i].rm_so != -1 ? pmatch[i].rm_eo - pmatch[i].rm_so : -1;
//  	      (*len)[i+s*nSubExpr] = pmatch[i].rm_eo - pmatch[i].rm_so;
	  }
	}
	else  if ( subexprKW) 
	  {
	    // Find number of subexpressions
//  	    SizeT nOut = 0;
// 	    for( SizeT i = 0; i<nmatch; ++i) {
// 	      if ( pmatch[i].rm_so == -1) break;
// 	      nOut++;
// 	    }

	    // Loop through subexpressions & fill output array
	    for( SizeT i = 0; i<nSubExpr; ++i) {
	      (* static_cast<DLongGDL*>(result))[i+s*nSubExpr] =  pmatch[i].rm_so;
	      if( lengthKW)
		(*len)[i+s*nSubExpr] = pmatch[i].rm_so != -1 ? pmatch[i].rm_eo - pmatch[i].rm_so : -1;
	    }
	  }
	else
	  {
	    if( booleanKW)
	      (* static_cast<DByteGDL*>(result))[s] = (matchres == 0);
	    else if ( extractKW) // !subExprKW
	      (* static_cast<DStringGDL*>(result))[s] = 
		(*stringExpr)[s].substr( pmatch[0].rm_so, 
					 pmatch[0].rm_eo - pmatch[0].rm_so);
	    else
	      (* static_cast<DLongGDL*>(result))[s] = matchres? -1:pmatch[0].rm_so;
	  }

	if( lengthKW && !subexprKW)
	  (*len)[s] = pmatch[0].rm_eo - pmatch[0].rm_so;
      }

    regfree( &regexp);

    if( lengthKW)
      e->SetKW( lengthIx, len);    

    return result;
  }

  BaseGDL* routine_info( EnvT* e)
  {
    SizeT nParam=e->NParam();

    static int functionsIx = e->KeywordIx( "FUNCTIONS" );
    bool functionsKW = e->KeywordSet( functionsIx );
    static int systemIx = e->KeywordIx( "SYSTEM" );
    bool systemKW = e->KeywordSet( systemIx );
    static int disabledIx = e->KeywordIx( "DISABLED" );
    bool disabledKW = e->KeywordSet( disabledIx );
    static int parametersIx = e->KeywordIx( "PARAMETERS" );
    bool parametersKW = e->KeywordSet( parametersIx );

    if (parametersKW)
    {
      // sanity checks
      if (systemKW || disabledKW) e->Throw("Conflicting keywords.");
      if (nParam != 1) e->Throw("Incorrect number of arguments.");

      // getting the routine name from the first parameter
      DString name;
      e->AssureScalarPar<DStringGDL>(0, name);
      name = StrUpCase(name);
        
      DSubUD* routine = functionsKW 
        ? static_cast<DSubUD*>(funList[GDLInterpreter::GetFunIx(name)])
        : static_cast<DSubUD*>(proList[GDLInterpreter::GetProIx(name)]);
      SizeT np = routine->NPar(), nk = routine->NKey();

      // creating the output anonymous structure
      DStructDesc* stru_desc = new DStructDesc("$truct");
      SpDLong aLong;
      stru_desc->AddTag("NUM_ARGS", &aLong);
      stru_desc->AddTag("NUM_KW_ARGS", &aLong);
      if (np > 0) 
      {
        SpDString aStringArr(dimension((int)np));
        stru_desc->AddTag("ARGS", &aStringArr);
      }
      if (nk > 0) 
      {
        SpDString aStringArr(dimension((int)nk));
        stru_desc->AddTag("KW_ARGS", &aStringArr);
      }
      DStructGDL* stru = new DStructGDL(stru_desc, dimension());

      // filling the structure with information about the routine 
      stru->InitTag("NUM_ARGS", DLongGDL(np));
      stru->InitTag("NUM_KW_ARGS", DLongGDL(nk));
      if (np > 0)
      {
        DStringGDL *pnames = new DStringGDL(dimension(np));
        for (SizeT p = 0; p < np; ++p) (*pnames)[p] = routine->GetVarName(nk + p); 
        stru->InitTag("ARGS", *pnames);
        delete pnames;
      }
      if (nk > 0)
      {
        DStringGDL *knames = new DStringGDL(dimension(nk));
        for (SizeT k = 0; k < nk; ++k) (*knames)[k] = routine->GetKWName(k); 
        stru->InitTag("KW_ARGS", *knames);
        delete knames;
      }

      // returning
      return stru;
    }

    // GDL does not have disabled routines
    if( disabledKW) return new DStringGDL("");

    //    if( functionsKW || systemKW || nParam == 0)
    //      {
    deque<DString> subList;
	    
    if( functionsKW)
      {
	if( systemKW)
	  {
	    SizeT n = libFunList.size();
	    if( n == 0) return new DStringGDL("");

	    DStringGDL* res = new DStringGDL( dimension( n), BaseGDL::NOZERO);
	    for( SizeT i = 0; i<n; ++i)
	      (*res)[i] = libFunList[ i]->ObjectName();

	    return res;
	  }
	else
	  {
	    SizeT n = funList.size();
	    if( n == 0) return new DStringGDL("");
	    subList.resize( n);
		
	    for( SizeT i = 0; i<n; ++i)
	      subList.push_back( funList[ i]->ObjectName());
	  }
      }
    else
      {
	if( systemKW)
	  {
	    SizeT n = libProList.size();
	    if( n == 0) return new DStringGDL("");

	    DStringGDL* res = new DStringGDL( dimension( n), BaseGDL::NOZERO);
	    for( SizeT i = 0; i<n; ++i)
	      (*res)[i] = libProList[ i]->ObjectName();

	    return res;
	  }
	else
	  {
	    SizeT n = proList.size();
	    if( n == 0) return new DStringGDL("");
	    subList.resize( n);
		
	    for( SizeT i = 0; i<n; ++i)
	      subList.push_back( proList[ i]->ObjectName());
	  }
      }
	
    sort( subList.begin(), subList.end());
    SizeT nS = subList.size();

    DStringGDL* res = new DStringGDL( dimension( nS), BaseGDL::NOZERO);
    for( SizeT s=0; s<nS; ++s)
      (*res)[ s] = subList[ s];

    return res;
    //      }
  }

  BaseGDL* get_kbrd( EnvT* e)
  {
    SizeT nParam=e->NParam();

    bool doWait = true;
    if( nParam > 0)
      {
	doWait = false;
	DLong waitArg = 0;
	e->AssureLongScalarPar( 0, waitArg);
	if( waitArg != 0)
	  {
	    doWait = true;
	  }
      }

//     if( doWait)
//       {
 
// 	char c = cin.get();
// 	DStringGDL* res = new DStringGDL( DString( i2s( c)));
// 	return res;
//       }
//     else
//       {
// 	char c = cin.get();
// 	DStringGDL* res = new DStringGDL( DString( i2s( c)));
// 	return res;
//       }

    // https://sourceforge.net/forum/forum.php?thread_id=3292183&forum_id=338691
    // TODO Implement proper SCALAR parameter handling (doWait variable?). 
 
    struct termios orig, get; 
    (void)tcgetattr(fileno(stdin), &orig); 
    get = orig; 
 
    // Disable terminal echoing and set it to non-canonical mode. 
    get.c_lflag &= ~(ECHO|ICANON); 
 
    (void)tcsetattr(fileno(stdin), TCSANOW, &get); 
 
    char c = cin.get(); 
 
    // Restore original terminal settings. 
    (void)tcsetattr(fileno(stdin), TCSANOW, &orig); 
 
    DStringGDL* res = new DStringGDL( DString( i2s( c))); 
    return res; 
  }


  BaseGDL* temporary( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    BaseGDL** p0 = &e->GetParDefined( 0);

    BaseGDL* ret = *p0;

    *p0 = NULL; // make parameter undefined
    return ret;
  }

  BaseGDL* memory( EnvT* e)
  {
    SizeT nParam=e->NParam( 0); 

    BaseGDL* ret;
    bool kw_l64 = e->KeywordSet(e->KeywordIx("L64"));
    // TODO: IDL-doc mentions about automatically switching to L64 if needed

    if (e->KeywordSet(e->KeywordIx("STRUCTURE")))
    {
      // returning structure
      if (kw_l64) 
      {
        ret = new DStructGDL("IDL_MEMORY64");
        DStructGDL* retStru = static_cast<DStructGDL*>(ret);
        *(retStru->GetTag(retStru->Desc()->TagIndex("CURRENT"))) = DLong64GDL(MemStats::GetCurrent());
        *(retStru->GetTag(retStru->Desc()->TagIndex("NUM_ALLOC"))) = DLong64GDL(MemStats::GetNumAlloc());
        *(retStru->GetTag(retStru->Desc()->TagIndex("NUM_FREE"))) = DLong64GDL(MemStats::GetNumFree());
        *(retStru->GetTag(retStru->Desc()->TagIndex("HIGHWATER"))) = DLong64GDL(MemStats::GetHighWater());
      }
      else 
      {
        ret = new DStructGDL("IDL_MEMORY");
        DStructGDL* retStru = static_cast<DStructGDL*>(ret);
        *(retStru->GetTag(retStru->Desc()->TagIndex("CURRENT"))) = DLongGDL(MemStats::GetCurrent());
        *(retStru->GetTag(retStru->Desc()->TagIndex("NUM_ALLOC"))) = DLongGDL(MemStats::GetNumAlloc());
        *(retStru->GetTag(retStru->Desc()->TagIndex("NUM_FREE"))) = DLongGDL(MemStats::GetNumFree());
        *(retStru->GetTag(retStru->Desc()->TagIndex("HIGHWATER"))) = DLongGDL(MemStats::GetHighWater());
      }
    }
    else 
    {
      bool kw_current = e->KeywordSet(e->KeywordIx("CURRENT"));
      bool kw_num_alloc = e->KeywordSet(e->KeywordIx("NUM_ALLOC"));
      bool kw_num_free = e->KeywordSet(e->KeywordIx("NUM_FREE"));
      bool kw_highwater = e->KeywordSet(e->KeywordIx("HIGHWATER"));

      // Following the IDL documentation: mutually exclusive keywords
      // IDL behaves different, incl. segfaults with selected kw combinations
      if (kw_current + kw_num_alloc + kw_num_free + kw_highwater > 1) 
        e->Throw("CURRENT, NUM_ALLOC, NUM_FREE & HIGHWATER keywords"
          " are mutually exclusive");

      if (kw_current)
      {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetCurrent());
        else ret = new DLongGDL(MemStats::GetCurrent());
      } 
      else if (kw_num_alloc)
      {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetNumAlloc());
        else ret = new DLongGDL(MemStats::GetNumAlloc());
      }
      else if (kw_num_free)
      {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetNumFree());
        else ret = new DLongGDL(MemStats::GetNumFree());
      }
      else if (kw_highwater)
      {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetHighWater());
        else ret = new DLongGDL(MemStats::GetHighWater());
      }
      else 
      {
        // returning 4-element array 
        if (kw_l64) 
        {
          ret = new DLong64GDL(dimension(4));
          (*dynamic_cast<DLong64GDL*>(ret))[0] = MemStats::GetCurrent();
          (*dynamic_cast<DLong64GDL*>(ret))[1] = MemStats::GetNumAlloc();
          (*dynamic_cast<DLong64GDL*>(ret))[2] = MemStats::GetNumFree();
          (*dynamic_cast<DLong64GDL*>(ret))[3] = MemStats::GetHighWater();
        }
        else 
        {
          ret = new DLongGDL(dimension(4));
          (*dynamic_cast<DLongGDL*>(ret))[0] = MemStats::GetCurrent();
          (*dynamic_cast<DLongGDL*>(ret))[1] = MemStats::GetNumAlloc();
          (*dynamic_cast<DLongGDL*>(ret))[2] = MemStats::GetNumFree();
          (*dynamic_cast<DLongGDL*>(ret))[3] = MemStats::GetHighWater();
        }
      }
    }

    return ret;
  }

  inline DByte StrCmp( const string& s1, const string& s2, DLong n)
  {
    if( n <= 0) return 1;
    if( s1.substr(0,n) == s2.substr(0,n)) return 1;
    return 0;
  }
  inline DByte StrCmp( const string& s1, const string& s2)
  {
    if( s1 == s2) return 1;
    return 0;
  }
  inline DByte StrCmpFold( const string& s1, const string& s2, DLong n)
  {
    if( n <= 0) return 1;
    if( StrUpCase( s1.substr(0,n)) == StrUpCase(s2.substr(0,n))) return 1;
    return 0;
  }
  inline DByte StrCmpFold( const string& s1, const string& s2)
  {
    if( StrUpCase( s1) == StrUpCase(s2)) return 1;
    return 0;
  }

  BaseGDL* strcmp_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    DStringGDL* s0 = static_cast<DStringGDL*>( e->GetParAs< DStringGDL>( 0));
    DStringGDL* s1 = static_cast<DStringGDL*>( e->GetParAs< DStringGDL>( 1));

    DLongGDL* l2 = NULL;
    if( nParam > 2)
      {
	l2 = static_cast<DLongGDL*>( e->GetParAs< DLongGDL>( 2));
      }

    static int foldIx = e->KeywordIx( "FOLD_CASE");
    bool fold = e->KeywordSet( foldIx );
    
    if( s0->Scalar() && s1->Scalar())
      {
	if( l2 == NULL)
	  {
	    if( fold)
	      return new DByteGDL( StrCmpFold( (*s0)[0], (*s1)[0]));
	    else
	      return new DByteGDL( StrCmp( (*s0)[0], (*s1)[0]));
	  }
	else
	  {
	    DByteGDL* res = new DByteGDL( l2->Dim(), BaseGDL::NOZERO);
	    SizeT nEl = l2->N_Elements();
	    if( fold)
	      for( SizeT i=0; i<nEl; ++i)
		(*res)[i] = StrCmpFold( (*s0)[0], (*s1)[0], (*l2)[i]);
	    else
	      for( SizeT i=0; i<nEl; ++i)
		(*res)[i] = StrCmp( (*s0)[0], (*s1)[0], (*l2)[i]);
	    return res;
	  }
      }
    else // at least one array
      {
	if( l2 == NULL)
	  {
	    if( s0->Scalar())
	      {
		DByteGDL* res = new DByteGDL( s1->Dim(), BaseGDL::NOZERO);
		SizeT nEl = s1->N_Elements();
		if( fold)
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmpFold( (*s0)[0], (*s1)[i]);
		else
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmp( (*s0)[0], (*s1)[i]);
		return res;
	      }
	    else if( s1->Scalar())
	      {
		DByteGDL* res = new DByteGDL( s0->Dim(), BaseGDL::NOZERO);
		SizeT nEl = s0->N_Elements();
		if( fold)
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmpFold( (*s0)[i], (*s1)[0]);
		else
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmp( (*s0)[i], (*s1)[0]);
		return res;
	      }
	    else // both arrays
	      {
		DByteGDL* res;
		SizeT    nEl;
		if( s0->N_Elements() <= s1->N_Elements())
		  {
		    res = new DByteGDL( s0->Dim(), BaseGDL::NOZERO);
		    nEl = s0->N_Elements();
		  }
		else		      
		  {
		    res = new DByteGDL( s1->Dim(), BaseGDL::NOZERO);
		    nEl = s1->N_Elements();
		  }
		if( fold)
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmpFold( (*s0)[i], (*s1)[0]);
		else
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmp( (*s0)[i], (*s1)[0]);
		return res;
	      }
	  }
	else // l2 != NULL
	  {
	    DByteGDL* res;
	    SizeT    nEl;
	    bool l2Scalar = l2->Scalar();
	    if( s0->Scalar())
	      {
		if( l2Scalar || s1->N_Elements() <= l2->N_Elements())
		  {
		    res = new DByteGDL( s1->Dim(), BaseGDL::NOZERO);
		    nEl = s1->N_Elements();
		  }
		else
		  {
		    res = new DByteGDL( l2->Dim(), BaseGDL::NOZERO);
		    nEl = l2->N_Elements();
		  }
		if( fold)
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmpFold( (*s0)[0], (*s1)[i], (*l2)[l2Scalar?0:i]);
		else
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmp( (*s0)[0], (*s1)[i], (*l2)[l2Scalar?0:i]);
		return res;
	      }
	    else if( s1->Scalar())
	      {
		if( l2Scalar || s0->N_Elements() <= l2->N_Elements())
		  {
		    res = new DByteGDL( s0->Dim(), BaseGDL::NOZERO);
		    nEl = s0->N_Elements();
		  }
		else
		  {
		    res = new DByteGDL( l2->Dim(), BaseGDL::NOZERO);
		    nEl = l2->N_Elements();
		  }
		if( fold)
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmpFold( (*s0)[i], (*s1)[0], (*l2)[l2Scalar?0:i]);
		else
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmp( (*s0)[i], (*s1)[0], (*l2)[l2Scalar?0:i]);
		return res;
	      }
	    else // s1 and s2 are arrays
	      {
		if( l2Scalar)
		  if( s0->N_Elements() <= s1->N_Elements())
		    {
		      res = new DByteGDL( s0->Dim(), BaseGDL::NOZERO);
		      nEl = s0->N_Elements();
		    }
		  else 
		    {
		      res = new DByteGDL( s1->Dim(), BaseGDL::NOZERO);
		      nEl = s1->N_Elements();
		    }
		else 
		  {
		    if( s0->N_Elements() <= s1->N_Elements())
		      if( s0->N_Elements() <= l2->N_Elements())
			{
			  res = new DByteGDL( s0->Dim(), BaseGDL::NOZERO);
			  nEl = s0->N_Elements();
			}
		      else
			{
			  res = new DByteGDL( l2->Dim(), BaseGDL::NOZERO);
			  nEl = l2->N_Elements();
			}
		    else
		      if( s1->N_Elements() <= l2->N_Elements())
			{
			  res = new DByteGDL( s1->Dim(), BaseGDL::NOZERO);
			  nEl = s1->N_Elements();
			}
		      else
			{
			  res = new DByteGDL( l2->Dim(), BaseGDL::NOZERO);
			  nEl = l2->N_Elements();
			}
		  }
		if( fold)
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmpFold( (*s0)[i], (*s1)[i], (*l2)[l2Scalar?0:i]);
		else
		  for( SizeT i=0; i<nEl; ++i)
		    (*res)[i] = StrCmp( (*s0)[i], (*s1)[i], (*l2)[l2Scalar?0:i]);
		return res;
	      }
	  }
      }
  }

  string TagName( EnvT* e, const string& name)
  {
    string n = StrUpCase( name);
    SizeT len = n.size();
    if( n[0] != '_' && n[0] != '!' && (n[0] < 'A' || n[0] > 'Z'))
      e->Throw( "Illegal tag name: "+name+".");
    for( SizeT i=1; i<len; ++i)
      {
	if( n[i] == ' ')
	  n[i] = '_';
	else 
	  if( n[i] != '_' && n[i] != '$' && //n[0] != '!' &&
	      (n[i] < 'A' || n[i] > 'Z') &&
	      (n[i] < '0' || n[i] > '9'))
	    e->Throw( "Illegal tag name: "+name+".");
      }
    return n;
  }

  BaseGDL* create_struct( EnvT* e)
  {
    static int nameIx = e->KeywordIx( "NAME" );
    DString name = "$truct";
    if( e->KeywordPresent( nameIx)) {
      // Check if name exists, if not then treat as unnamed
      if (e->GetKW( nameIx) != NULL)
	e->AssureStringScalarKW( nameIx, name);
    }

    if( name != "$truct") // named struct
      {
	name = StrUpCase( name);
	
	SizeT nParam=e->NParam();

	if( nParam == 0)
	  {
	    DStructDesc* desc = 
	      e->Interpreter()->GetStruct( name, e->CallingNode());
	   
	    dimension dim( 1);
	    return new DStructGDL( desc, dim);
	  }

	DStructDesc*          nStructDesc;
	auto_ptr<DStructDesc> nStructDescGuard;
	
	DStructDesc* oStructDesc=
	  FindInStructList( structList, name);
	
	if( oStructDesc == NULL || oStructDesc->NTags() > 0)
	  {
	    // not defined at all yet (-> define now)
	    // or completely defined  (-> define now and check equality)
	    nStructDesc= new DStructDesc( name);
                    
	    // guard it
	    nStructDescGuard.reset( nStructDesc); 
	  }
	else
	  {   
	    // NTags() == 0
	    // not completely defined (only name in list)
	    nStructDesc= oStructDesc;
	  }
                
	// the instance variable
	// 	dimension dim( 1);
	// 	DStructGDL* instance = new DStructGDL( nStructDesc, dim);
	DStructGDL* instance = new DStructGDL( nStructDesc);
	auto_ptr<DStructGDL> instance_guard(instance);

	for( SizeT p=0; p<nParam; ++p)
	  {
	    BaseGDL* par = e->GetParDefined( p);
	    DStructGDL* parStruct = dynamic_cast<DStructGDL*>( par);
	    if( parStruct != NULL)
	      {
		// add struct
		if( !parStruct->Scalar())
		  e->Throw("Expression must be a scalar in this context: "+
			   e->GetParString( p));
		
		DStructDesc* desc = parStruct->Desc();
		for( SizeT t=0; t< desc->NTags(); ++t)
		  {
		    instance->NewTag( desc->TagName( t), 
				      parStruct->GetTag( t)->Dup());
		  }
	      }
	    else
	      {
		// add tag value pair
		DStringGDL* tagNames = e->GetParAs<DStringGDL>( p);
		SizeT nTags = tagNames->N_Elements();

		SizeT tagStart = p+1;
		SizeT tagEnd   = p+nTags;
		if( tagEnd >= nParam)
		  e->Throw( "Incorrect number of arguments.");

		do{
		  ++p;
		  BaseGDL* value = e->GetParDefined( p);
		    
		  // add 
		  instance->NewTag( TagName( e, (*tagNames)[ p-tagStart]),
				    value->Dup());
		} 
		while( p<tagEnd);
	      }
	  }

	if( oStructDesc != NULL)
	  {
	    if( oStructDesc != nStructDesc)
	      {
		oStructDesc->AssureIdentical(nStructDesc);
		instance->DStructGDL::SetDesc(oStructDesc);
		//delete nStructDesc; // auto_ptr
	      }
	  }
	else
	  {
	    // release from guard (if not NULL)
	    nStructDescGuard.release();
	    // insert into struct list 
	    structList.push_back(nStructDesc);
	  }
	
	instance_guard.release();
	return instance;
      }
    else 
      { // unnamed struc

	// Handle case of single structure parameter
	SizeT nParam;
	nParam = e->NParam(1);
	BaseGDL* par = e->GetParDefined( 0);
	DStructGDL* parStruct = dynamic_cast<DStructGDL*>( par);
	if (nParam != 1 || parStruct == NULL)
	  nParam=e->NParam(2);

	DStructDesc*          nStructDesc = new DStructDesc( "$truct");
	// instance takes care of nStructDesc since it is unnamed
	// 	dimension dim( 1);
	// 	DStructGDL* instance = new DStructGDL( nStructDesc, dim);
	DStructGDL* instance = new DStructGDL( nStructDesc);
	auto_ptr<DStructGDL> instance_guard(instance);

	for( SizeT p=0; p<nParam;)
	  {
	    BaseGDL* par = e->GetParDefined( p);
	    DStructGDL* parStruct = dynamic_cast<DStructGDL*>( par);
	    if( parStruct != NULL)
	      {
		// add struct
		if( !parStruct->Scalar())
		  e->Throw("Expression must be a scalar in this context: "+
			   e->GetParString( p));
		
		DStructDesc* desc = parStruct->Desc();
		for( SizeT t=0; t< desc->NTags(); ++t)
		  {
		    instance->NewTag( desc->TagName( t), 
				      parStruct->GetTag( t)->Dup());
		  }
		++p;
	      }
	    else
	      {
		// add tag value pair
		DStringGDL* tagNames = e->GetParAs<DStringGDL>( p);
		SizeT nTags = tagNames->N_Elements();

		SizeT tagStart = p+1;
		SizeT tagEnd   = p+nTags;
		if( tagEnd >= nParam)
		  e->Throw( "Incorrect number of arguments.");

		for(++p; p<=tagEnd; ++p)
		  {
		    BaseGDL* value = e->GetParDefined( p);

		    // add 
		    instance->NewTag( TagName( e, (*tagNames)[ p-tagStart]),
				      value->Dup());
		  }
	      }
	  }
	
	instance_guard.release();
	return instance;
      }
  }

  BaseGDL* rotate( EnvT* e)
  {
    e->NParam(2);
    BaseGDL* p0 = e->GetParDefined( 0);

    if( p0->Rank() == 0)
      e->Throw( "Expression must be an array in this context: " + e->GetParString( 0));

    if( p0->Rank() != 1 && p0->Rank() != 2)
      e->Throw( "Only 1 or 2 dimensions allowed: " + e->GetParString( 0));

    if( p0->Type() == STRUCT)
      e->Throw( "STRUCT expression not allowed in this context: "+
		e->GetParString( 0));
    
    DLong dir;
    e->AssureLongScalarPar( 1, dir);

    return p0->Rotate( dir);
  }

  // SA: based on the code of rotate() (above)
  BaseGDL* reverse( EnvT* e)
  {
    e->NParam(1);
    BaseGDL* p0 = e->GetParDefined(0);
    if (p0->Rank() == 0) return p0->Dup();

    DLong dim = 1;
    if (e->GetPar(1) != NULL) 
      e->AssureLongScalarPar(1, dim);
    if (p0->Rank() != 0 && (dim > p0->Rank() || dim < 1))
      e->Throw("Subscript_index must be positive and less than or equal to number of dimensions.");

    BaseGDL* ret;
    // IDL doc states that OVERWRITE is ignored for one- or two-dim. arrays 
    // but it seems to behave differently
    // if (p0->Rank() > 2 && e->KeywordSet("OVERWRITE") && e->GlobalPar(0))
    if (e->KeywordSet("OVERWRITE"))
    {
      p0->Reverse(dim-1);
      bool stolen = e->StealLocalPar( 0);
      if( !stolen) e->GetPar(0) = NULL;
      return p0;
    }
    else ret = p0->DupReverse(dim - 1);
    return ret;
  }

  // SA: parse_url based on the PHP parse_url() function code
  //     by Jim Winstead / The PHP Group (PHP license v. 3.01)
  //     (http://svn.php.net/viewvc/php/php-src/trunk/ext/standard/url.c)
  //     PHP is free software available at http://www.php.net/software/
  //
  //     notes: 
  //     - IDL does not support IPv6 URLs, GDL does 
  //     - IDL includes characters after '#' in the QUERY part, GDL
  //       just skips them and issues a warning (perhaps not needed)
  //     - IDL preserves controll characters in URLs, GDL preserves
  //       them as well but a warning is issued
  //     - IDL sets 80 as a default value for PORT, even if the url has 
  //       an ftp:// schema indicated - GDL does not have any default value
  //     - IDL excludes the leading "/" from the path, GDL preserves it
  //     ... these differences seem just rational for me but please do change
  //         it if IDL-compatibility would be beneficial for any reason here

  BaseGDL* parse_url(EnvT* env)
  {
    // sanity check for number of parameters
    SizeT nParam = env->NParam();

    // 1-nd argument : the url string
    DString url; 
    env->AssureScalarPar<DStringGDL>(0, url); 

    // sanity check for controll characters
    string::iterator it;
    for (it = url.begin(); it < url.end(); it++) if (iscntrl(*it))
    {
      Warning("PARSE_URL: URL contains a control character");
      break;
    }

    // creating the output anonymous structure
    DStructDesc* urlstru_desc = new DStructDesc("$truct");
    SpDString aString;
    urlstru_desc->AddTag("SCHEME",   &aString);
    static size_t ixSCHEME = 0;
    urlstru_desc->AddTag("USERNAME", &aString);
    urlstru_desc->AddTag("PASSWORD", &aString);
    urlstru_desc->AddTag("HOST",     &aString);
    urlstru_desc->AddTag("PORT",     &aString);
    static size_t ixPORT = 4;
    urlstru_desc->AddTag("PATH",     &aString);
    urlstru_desc->AddTag("QUERY",    &aString);
    DStructGDL* urlstru = new DStructGDL(urlstru_desc, dimension());
    auto_ptr<DStructGDL> urlstru_guard(urlstru);
          
    // parsing the URL
    char const *str = url.c_str();
    size_t length = url.length();
    char port_buf[6];
    char const *s, *e, *p, *pp, *ue;
		
    s = str;
    ue = s + length;

    // parsing scheme 
    if ((e = (const char*)memchr(s, ':', length)) && (e - s)) 
    {
      // validating scheme 
      p = s;
      while (p < e) 
      {
        // scheme = 1*[ lowalpha | digit | "+" | "-" | "." ]
        if (!isalpha(*p) && !isdigit(*p) && *p != '+' && *p != '.' && *p != '-') 
        {
          if (e + 1 < ue) goto parse_port;
          else goto just_path;
        }
        p++;
      }
      if (*(e + 1) == '\0') 
      { 
        // only scheme is available 
        urlstru->InitTag("SCHEME", DStringGDL(string(s, (e - s))));
        goto end;
      }
      // schemas without '/' (like mailto: and zlib:) 
      if (*(e+1) != '/') 
      {
        // check if the data we get is a port this allows us to correctly parse things like a.com:80
        p = e + 1;
        while (isdigit(*p)) p++;
	if ((*p == '\0' || *p == '/') && (p - e) < 7) goto parse_port;
        urlstru->InitTag("SCHEME", DStringGDL(string(s, (e - s))));
        length -= ++e - s;
        s = e;
        goto just_path;
      } 
      else 
      {
        urlstru->InitTag("SCHEME", DStringGDL(string(s, (e - s))));
        if (*(e+2) == '/') 
        {
          s = e + 3;
          if (!strncasecmp("file", 
            (*static_cast<DStringGDL*>(urlstru->GetTag(ixSCHEME)))[0].c_str(), 
            sizeof("file")
          )) 
          {
            if (*(e + 3) == '/') 
            {
              // support windows drive letters as in: file:///c:/somedir/file.txt
              if (*(e + 5) == ':') s = e + 4;
              goto nohost;
            }
          }
        } 
        else 
        {
          if (!strncasecmp("file", 
            (*static_cast<DStringGDL*>(urlstru->GetTag(ixSCHEME)))[0].c_str(), 
            sizeof("file"))
          ) 
          {
            s = e + 1;
            goto nohost;
          } 
          else 
          {
            length -= ++e - s;
            s = e;
            goto just_path;
          }	
        }
      }	
    } 
    else if (e) 
    { 
      // no scheme, look for port 
      parse_port:
      p = e + 1;
      pp = p;
      while (pp-p < 6 && isdigit(*pp)) pp++;
      if (pp-p < 6 && (*pp == '/' || *pp == '\0')) 
      {
        memcpy(port_buf, p, (pp-p));
        port_buf[pp-p] = '\0';
        urlstru->InitTag("PORT", DStringGDL(port_buf));
      } 
      else goto just_path;
    } 
    else 
    {
      just_path:
      ue = s + length;
      goto nohost;
    }
    e = ue;
    if (!(p = (const char*)memchr(s, '/', (ue - s)))) 
    {
      if ((p = (const char*)memchr(s, '?', (ue - s)))) e = p;
      else if ((p = (const char*)memchr(s, '#', (ue - s)))) e = p;
    } 
    else e = p;
    // check for login and password 
    {
      size_t pos;
      if ((pos = string(s, e - s).find_last_of("@")) != string::npos)
      {
        p = s + pos;
        if ((pp = (const char*)memchr(s, ':', (p-s)))) 
        {
          if ((pp-s) > 0) urlstru->InitTag("USERNAME", DStringGDL(string(s, (pp - s))));
          pp++;
          if (p-pp > 0) urlstru->InitTag("PASSWORD", DStringGDL(string(pp, (p - pp))));
        } 
        else urlstru->InitTag("USERNAME", DStringGDL(string(s, (p - s))));
        s = p + 1;
      }
    }
    // check for port 
    if (*s == '[' && *(e-1) == ']') p = s;     // IPv6 embedded address 
    else for(p = e; *p != ':' && p >= s; p--); // memrchr is a GNU extension 
    if (p >= s && *p == ':') 
    {
      if ((*static_cast<DStringGDL*>(urlstru->GetTag(ixPORT)))[0].length() == 0) 
      {
        p++;
        if (e-p > 5) env->Throw("port cannot be longer then 5 characters");
	else if (e - p > 0) 
        {
          memcpy(port_buf, p, (e-p));
          port_buf[e-p] = '\0';
          urlstru->InitTag("PORT", DStringGDL(port_buf));
        }
        p--;
      }	
    } 
    else p = e;
    // check if we have a valid host, if we don't reject the string as url 
    if ((p-s) < 1) env->Throw("invalid host");
    urlstru->InitTag("HOST", DStringGDL(string(s, (p - s))));
    if (e == ue) goto end;
    s = e;
    nohost:
    if ((p = (const char*)memchr(s, '?', (ue - s)))) 
    {
      pp = strchr(s, '#');
      if (pp && pp < p) 
      {
        p = pp;
        pp = strchr(pp+2, '#');
      }
      if (p - s) urlstru->InitTag("PATH", DStringGDL(string(s, (p - s))));
      if (pp) 
      {
        if (pp - ++p) urlstru->InitTag("QUERY", DStringGDL(string(p, (pp - p))));
        p = pp;
        goto label_parse;
      } 
      else if (++p - ue) urlstru->InitTag("QUERY", DStringGDL(string(p, (ue - p))));
    } 
    else if ((p = (const char*)memchr(s, '#', (ue - s)))) 
    {
      if (p - s) urlstru->InitTag("PATH", DStringGDL(string(s, (p - s))));
      label_parse:
      p++;
      if (ue - p) Warning("PARSE_URL: URL fragment left out: #" + string(p, (ue-p)));
    } 
    else urlstru->InitTag("PATH", DStringGDL(string(s, (ue - s))));
    end:

    // returning the result
    urlstru_guard.release();
    return urlstru;
  }

  BaseGDL* locale_get(EnvT* e)
  {
#ifdef HAVE_LOCALE_H

    // make GDL inherit the calling process locale
    setlocale(LC_ALL, "");
    // note doen the inherited locale
    DStringGDL *locale = new DStringGDL(setlocale(LC_CTYPE, NULL));
    // return to the C locale
    setlocale(LC_ALL, "C");

    return locale;
#else
    e->Throw("OS does not provide locale information");
#endif
  }

  // SA: relies on the contents of the lib::command_line_args vector
  //     defined and filled with data (pointers) in gdl.cpp
  BaseGDL* command_line_args_fun(EnvT* e)
  {
#ifdef PYTHON_MODULE
    e->Throw("no command line arguments available (GDL built as a Python module)");
#else
    static int countIx = e->KeywordIx("COUNT");
    extern std::vector<char*> command_line_args; 

    // setting the COUNT keyword value
    if (e->KeywordPresent(countIx))
    {
      e->AssureGlobalKW(countIx);
      e->SetKW(countIx, new DLongGDL(command_line_args.size()));
    }

    // returning empty string or an array of arguments
    if (command_line_args.empty()) return new DStringGDL("");
    else
    {
      BaseGDL* ret = new DStringGDL(dimension(command_line_args.size()));   
      for (size_t i = 0; i < command_line_args.size(); i++)
        (*static_cast<DStringGDL*>(ret))[i] = command_line_args[i];
      return ret;
    }
#endif
  }

  // SA: relies in the uname() from libc (must be there if POSIX)
  BaseGDL* get_login_info( EnvT* e)
  {
    // getting the info 
    char* login = getlogin();
    if (login == NULL) e->Throw("Failed to get user name from the OS"); 
    struct utsname info;
    if (0 != uname(&info)) e->Throw("Failed to get machine name from the OS");

    // creating the output anonymous structure
    DStructDesc* stru_desc = new DStructDesc("$truct");
    SpDString aString;
    stru_desc->AddTag("MACHINE_NAME", &aString);
    stru_desc->AddTag("USER_NAME", &aString);
    DStructGDL* stru = new DStructGDL(stru_desc, dimension());

    // returning the info 
    stru->InitTag("USER_NAME", DStringGDL(login));
    stru->InitTag("MACHINE_NAME", DStringGDL(info.nodename));
    return stru;
  }

  // SA: base64 logic in base64.hpp, based on code by Bob Withers (consult base64.hpp)
  BaseGDL* idl_base64(EnvT* e)
  {
    BaseGDL* p0 = e->GetPar(0);    
    if (p0 != NULL)
    { 
      if (p0->Rank() == 0 && p0->Type() == STRING)
      {
        // decoding
        string* str = &((*static_cast<DStringGDL*>(p0))[0]);
        if (str->length() == 0) return new DByteGDL(0);
        if (str->length() % 4 != 0) 
          e->Throw("Input string length must be a multiple of 4");
        unsigned int retlen = base64::decodeSize(*str);
        if (retlen == 0 || retlen > str->length()) e->Throw("No data in the input string");
        DByteGDL* ret = new DByteGDL(dimension(retlen));
        if (!base64::decode(*str, (char*)&((*ret)[0]), ret->N_Elements()))
          e->Throw("Base64 decoder failed"); 
        return ret;
      }
      if (p0->Rank() >= 1 && p0->Type() == BYTE)
      {
        // encoding
        return new DStringGDL(
          base64::encode((char*)&(*static_cast<DByteGDL*>(p0))[0], p0->N_Elements())
        );
      } 
    }
    e->Throw("Expecting string or byte array as a first parameter");
  }

  BaseGDL* get_drive_list(EnvT* e)
  {
    if (e->KeywordPresent(0)) e->SetKW(0, new DLongGDL(0));
    return new DStringGDL("");
  }

} // namespace

