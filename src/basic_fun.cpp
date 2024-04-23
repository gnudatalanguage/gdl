/***************************************************************************
                         basic_fun.cpp  -  basic GDL library function
                            -------------------
   begin                : July 22 2002
   copyright            : (C) 2002 by Marc Schellens (exceptions see below)
   email                : m_schellens@users.sf.net

strtok_fun, getenv_fun, tag_names_fun, stregex_fun:
(C) 2004 by Peter Messmer

20150506 Jacco A. de Zwart, National Institutes of Health, Bethesda, MD, USA
    Changed behavior of COMPLEX() and DCOMPLEX() called with three arguments,
    aka where type casting is the expected behavoir.

2017 September
  Greg Jung mods to unbug pointer, object treatments. Also:
    Updated with new Where(), cosmetics
    #ifndef _WIN32 replaces #if !defined(_WIN32) || defined(__CYGWIN__)
    Mods to array_equal() and array_never_equal (new)
    routine_filepath moved to here.
    command_line_args uses strings instead of char*

2017 July gilles-duvert  New version of Where() twice as fast as previous
 ***************************************************************************/
// AC 2018-feb

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

// used to defined GDL_TMPDIR: may have trouble on MSwin, help welcome
#ifndef _WIN32
#include <paths.h>
#endif

#include <limits>
#include <string>
#include <fstream>
#include <memory>
#include <regex.h> // stregex

#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

#if defined(__FreeBSD__) || defined(__sun__) || defined(__OpenBSD__)
extern "C" char **environ;
#endif

//#include "nullgdl.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"
#include "basic_pro.hpp"
#include "base64.hpp"
#include "gdlfpexceptions.hpp"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

/* max regexp error message length */
#define MAX_REGEXPERR_LENGTH 80

#ifdef _MSC_VER
#if _MSC_VER < 1800
#define std::isfinite _finite
#define isnan _isnan
#define round(f) floor(f+0.5)
#endif
#define std::isfinite(x) std::isfinite((double) x)

int strncasecmp(const char *s1, const char *s2, size_t n) {
  if (n == 0)
    return 0;
  while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
    if (n == 0 || *s1 == '\0' || *s2 == '\0')
      break;
    s1++;
    s2++;
  }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}
#endif

#ifndef _WIN32
#include <sys/utsname.h>
#endif

static DStructGDL* GetObjStruct(BaseGDL* Objptr, EnvT* e) {
  if (Objptr == 0 || Objptr->Type() != GDL_OBJ)
    e->Throw("Objptr not of type OBJECT. Please report.");
  if (!Objptr->Scalar())
    e->Throw("Objptr must be a scalar. Please report.");
  DObjGDL* Object = static_cast<DObjGDL*> (Objptr);
  DObj ID = (*Object)[0];
  try {
    return BaseGDL::interpreter->GetObjHeap(ID);
  } catch (GDLInterpreter::HeapException& hEx) {
    e->Throw("Object ID <" + i2s(ID) + "> not found.");
  }
  assert(false);
  return NULL;
}

static bool trace_me(false);

namespace lib {
  bool trace_arg();
  bool gdlarg_present(const char* s);
  SizeT HASH_count(DStructGDL* oStructGDL);
  SizeT LIST_count(DStructGDL* oStructGDL);

  // for use in COMMAND_LINE_ARGS()
  std::vector<std::string> command_line_args;


  using namespace std;
  using std::isinf;
  using std::isnan;
  using namespace antlr;

  DULong SHA256Constants[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
    , 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
    , 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
    , 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
    , 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
    , 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
    , 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
    , 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  DULong SHAH0[] = {
    0x6a09e667 // H0_0
    , 0xbb67ae85
    , 0x3c6ef372
    , 0xa54ff53a
    , 0x510e527f
    , 0x9b05688c
    , 0x1f83d9ab
    , 0x5be0cd19 // H0_7
  };



  // assumes all parameters from pOffs till end are dim

  void arr(EnvT* e, dimension& dim, SizeT pOffs = 0) {

    int nParam = e->NParam() - pOffs;

    if (nParam <= 0)
      e->Throw("Incorrect number of arguments.");

    const string BadDims = "Array dimensions must be greater than 0.";


    if (nParam == 1) {

      BaseGDL* par = e->GetParDefined(pOffs);

      SizeT newDim;
      int ret = par->Scalar2Index(newDim);

      if (ret < 0) throw GDLException(BadDims);

      if (ret > 0) { // single argument
        if (newDim < 1) throw GDLException(BadDims);
        dim << newDim;
        return;
      }
      if (ret == 0) { //  array argument
        DLongGDL* ind =
          static_cast<DLongGDL*> (par->Convert2(GDL_LONG, BaseGDL::COPY));
        Guard<DLongGDL> ind_guard(ind);
        //e->Guard( ind);

        for (SizeT i = 0; i < par->N_Elements(); ++i) {
          if ((*ind)[i] < 1) throw GDLException(BadDims);
          dim << (*ind)[i];
        }
        return;
      }
      e->Throw("arr: should never arrive here.");
      return;
    }

    // max number checked in interpreter
    SizeT endIx = nParam + pOffs;
    for (SizeT i = pOffs; i < endIx; i++) {
      BaseGDL* par = e->GetParDefined(i);

      SizeT newDim;
      int ret = par->Scalar2Index(newDim);
      if (ret < 1 || newDim == 0) throw GDLException(BadDims);
      dim << newDim;
    }
  }

  BaseGDL* bytarr(EnvT* e) {
    dimension dim;

    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DByteGDL(dim, BaseGDL::NOZERO);
    return new DByteGDL(dim);
  }

  BaseGDL* intarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DIntGDL(dim, BaseGDL::NOZERO);
    return new DIntGDL(dim);
    //     }
    //     catch( GDLException& ex)
    //       {
    //  e->Throw( "INTARR: "+ex.getMessage());
    //       }
  }

  BaseGDL* uintarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DUIntGDL(dim, BaseGDL::NOZERO);
    return new DUIntGDL(dim);
    //     }
    //     catch( GDLException& ex)
    //       {
    //  e->Throw( "UINTARR: "+ex.getMessage());
    //       }
  }

  BaseGDL* lonarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DLongGDL(dim, BaseGDL::NOZERO);
    return new DLongGDL(dim);
    /*    }
      catch( GDLException& ex)
      {
      e->Throw( "LONARR: "+ex.getMessage());
      }*/
  }

  BaseGDL* ulonarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DULongGDL(dim, BaseGDL::NOZERO);
    return new DULongGDL(dim);
    /*   }
     catch( GDLException& ex)
     {
     e->Throw( "ULONARR: "+ex.getMessage());
     }
     */
  }

  BaseGDL* lon64arr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DLong64GDL(dim, BaseGDL::NOZERO);
    return new DLong64GDL(dim);
    /*    }
      catch( GDLException& ex)
      {
      e->Throw( "LON64ARR: "+ex.getMessage());
      }*/
  }

  BaseGDL* ulon64arr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DULong64GDL(dim, BaseGDL::NOZERO);
    return new DULong64GDL(dim);
    /*  }
    catch( GDLException& ex)
    {
    e->Throw( "ULON64ARR: "+ex.getMessage());
    }*/
  }

  BaseGDL* fltarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DFloatGDL(dim, BaseGDL::NOZERO);
    return new DFloatGDL(dim);
    /* }
       catch( GDLException& ex)
       {
       e->Throw( "FLTARR: "+ex.getMessage());
       }
     */
  }

  BaseGDL* dblarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

     static int nozeroIx = e->KeywordIx("NOZERO");
   if (e->KeywordSet(nozeroIx)) return new DDoubleGDL(dim, BaseGDL::NOZERO);
    return new DDoubleGDL(dim);
    /* }
       catch( GDLException& ex)
       {
       e->Throw( "DBLARR: "+ex.getMessage());
       }*/
  }

  BaseGDL* strarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    // AC 2024/04°23 : no /NOZERO key *only* in this ARR function

    return new DStringGDL(dim);
    /*   }
     catch( GDLException& ex)
     {
     e->Throw( "STRARR: "+ex.getMessage());
     }
     */
  }

  BaseGDL* complexarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DComplexGDL(dim, BaseGDL::NOZERO);
    return new DComplexGDL(dim);
    /*}
      catch( GDLException& ex)
      {
      e->Throw( "COMPLEXARR: "+ex.getMessage());
      }
     */
  }

  BaseGDL* dcomplexarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    static int nozeroIx = e->KeywordIx("NOZERO");
    if (e->KeywordSet(nozeroIx)) return new DComplexDblGDL(dim, BaseGDL::NOZERO);
    return new DComplexDblGDL(dim);
    /*   }
     catch( GDLException& ex)
     {
     e->Throw( "DCOMPLEXARR: "+ex.getMessage());
     }
     */
  }

  BaseGDL* ptrarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    DPtrGDL* ret;
    
    static int nozeroIx = e->KeywordIx("NOZERO");    
    if (!e->KeywordSet(nozeroIx))
      return new DPtrGDL(dim);

    // ALLOCATE_HEAP
    ret = new DPtrGDL(dim, BaseGDL::NOZERO);

    SizeT nEl = ret->N_Elements();
    SizeT sIx = e->NewHeap(nEl, NullGDL::GetSingleInstance());
    for (SizeT i = 0; i < nEl; i++) (*ret)[i] = sIx + i;
    return ret;
  }

  BaseGDL* objarr(EnvT* e) {
    dimension dim;
    //     try{
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    // reference counting      if( e->KeywordSet(0)) return new DObjGDL(dim, BaseGDL::NOZERO);
    return new DObjGDL(dim);
  }

  BaseGDL* ptr_new(EnvT* e) {
    int nParam = e->NParam();
    if (nParam > 0) {
      BaseGDL* p = e->GetPar(0);

      // new ptr from undefined variable is allowed as well
      // this case was discovered by chance by Leva, July 16, 2014
      // p=ptr_new(), p=ptr_new(!null), p=ptr_new(undef_var) should work

      if ((p == NULL) || (p->Type() == GDL_UNDEF)) {
        DPtr heapID = e->NewHeap(1, NullGDL::GetSingleInstance()); //same as /ALLOCATE_HEAP
        return new DPtrGDL(heapID);
      }
      static int no_copyIx = e->KeywordIx("NO_COPY");
      if (e->KeywordSet(no_copyIx)) // NO_COPY
      {
        BaseGDL** p = &e->GetPar(0);

        DPtr heapID = e->NewHeap(1, *p);
        *p = NULL;
        return new DPtrGDL(heapID);
      } else {
        BaseGDL* p = e->GetParDefined(0);

        DPtr heapID = e->NewHeap(1, p->Dup());
        return new DPtrGDL(heapID);
      }
    } else {
      if (e->KeywordSet("ALLOCATE_HEAP"))
      {
        DPtr heapID = e->NewHeap(1, NullGDL::GetSingleInstance()); //allocate a !NULL, not a null ptr!!
        return new DPtrGDL(heapID);
      } else {
        return new DPtrGDL(0); // null ptr
      }
    }
  }

  BaseGDL* ptr_valid(EnvT* e) {
    int nParam = e->NParam();
    static int CASTIx = e->KeywordIx("CAST");
    static int COUNTIx = e->KeywordIx("COUNT");
    static int GET_HEAP_IDENTIFIERIx = e->KeywordIx("GET_HEAP_IDENTIFIER");

    if (e->KeywordPresent(COUNTIx)) {
      e->SetKW(COUNTIx, new DLongGDL(e->Interpreter()->HeapSize()));
    }

    if (nParam == 0) {
      return e->Interpreter()->GetAllHeap();
    }

    BaseGDL* p = e->GetPar(0);
    if (p == NULL) {
      return new DByteGDL(0);
    }

    DType pType = p->Type();
    bool isscalar = p->StrictScalar();


    GDLInterpreter* interpreter = e->Interpreter();

    //if called with  CAST return either a new pointer if arg is a pointer, or a pointer to the heap variable whose arg is an index to.
    if (e->KeywordSet(CASTIx)) {
      if (pType == GDL_PTR) return p->Dup();
      //else: only integer or array thereof authorized:
      DULongGDL* pL = static_cast<DULongGDL*> (p->Convert2(GDL_ULONG, BaseGDL::COPY));
      Guard<DULongGDL> pL_guard(pL);
      if (isscalar) {
        DULong p0 = (*pL)[0];
        if (interpreter->PtrValid(p0)) {
          interpreter->IncRef(p0);
          return new DPtrGDL(p0);
        } else return new DPtrGDL(0);
      } else {
        DPtrGDL* ret = new DPtrGDL(pL->Dim());
        for (SizeT i = 0; i < pL->N_Elements(); ++i)
          if (interpreter->PtrValid((*pL)[ i])) {
            interpreter->IncRef((*pL)[ i]);
            (*ret)[ i] = (*pL)[ i];
          }
        return ret;
      }
    }
    //no CAST. If PTR type, return true of false, or heap index if  GET_HEAP_IDENTIFIER is used.
    if (pType == GDL_PTR) {
      DPtrGDL* pPtr = static_cast<DPtrGDL*> (p);

      if (e->KeywordSet(GET_HEAP_IDENTIFIERIx)) {
        DULongGDL* pL = new DULongGDL(p->Dim());
        Guard<DULongGDL> pL_guard(pL);
        for (SizeT i = 0; i < pL->N_Elements(); ++i) (*pL) [i] = (*pPtr)[i]; //heap indexes
        if (isscalar) return new DULongGDL((*pL)[0]);
        else {
          pL_guard.release();
          return pL;
        }
      } else {
        if (isscalar) {
          return new DByteGDL(interpreter->PtrValid((*pPtr)[ 0]));
        } else {
          DByteGDL* ret = new DByteGDL(p->Dim());
          for (SizeT i = 0; i < ret->N_Elements(); ++i) (*ret)[i] = interpreter->PtrValid((*pPtr)[ i]);
          return ret;
        }
      }
    } else { // pType!=GDL_PTR: return false = 0 always.
      if (isscalar) {
        if (e->KeywordSet(GET_HEAP_IDENTIFIERIx)) return new DULongGDL(0);
        else return new DByteGDL(0);
      } else {
        if (e->KeywordSet(GET_HEAP_IDENTIFIERIx)) return new DULongGDL(p->Dim());
        else return new DByteGDL(p->Dim());
      }
    }
  }
  //
  // 2018 May 29 G. Jung: Note there is an inordinate separation of  scalar and non-scalar treament.
  //  This was my last line of attempt to quash an error, due to an assert
  // in gdlarray.cpp (line 210) which obj_valid() triggered in Travis tests.
  // I am now convinced that this error is due to the incorrect hack in GDL
  // that, for "SizeT nEl = p->N_Elements();" returns instead the count() of the list
  // so in fact, a list is not a true object.
  //  Merge "legacy_list" branch to remedy this.
  //

  BaseGDL* obj_valid(EnvT* e) {
    int nParam = e->NParam();
    static int CASTIx = e->KeywordIx("CAST");
    static int COUNTIx = e->KeywordIx("COUNT");
    static int GET_HEAP_IDENTIFIERIx = e->KeywordIx("GET_HEAP_IDENTIFIER");

    if (e->KeywordPresent(COUNTIx)) // COUNT
    {
      e->SetKW(COUNTIx, new DLongGDL(e->Interpreter()->ObjHeapSize()));
    }

    if (nParam == 0) {
      return e->Interpreter()->GetAllObjHeap();
    }

    BaseGDL* p = e->GetPar(0);
    if (p == NULL) {
      return new DByteGDL(0);
    }

    DType pType = p->Type();
    bool isscalar = p->StrictScalar();

    GDLInterpreter* interpreter = e->Interpreter();

    //if called with  CAST return either a new pointer if arg is a pointer, or a pointer to the ObjHeap variable whose arg is an index to.
    if (e->KeywordSet(CASTIx)) {
      if (pType == GDL_OBJ) return p->Dup();
      //else: only integer or array thereof authorized:
      DULongGDL* pL = static_cast<DULongGDL*> (p->Convert2(GDL_ULONG, BaseGDL::COPY));
      Guard<DULongGDL> pL_guard(pL);
      if (isscalar) {
        DULong p0 = (*pL)[0];
        if (interpreter->ObjValid(p0)) {
          interpreter->IncRefObj(p0);
          return new DObjGDL(p0);
        } else return new DObjGDL(0);
      } else {
        DObjGDL* ret = new DObjGDL(pL->Dim());
        for (SizeT i = 0; i < pL->N_Elements(); ++i)
          if (interpreter->ObjValid((*pL)[ i])) {
            interpreter->IncRefObj((*pL)[ i]);
            (*ret)[ i] = (*pL)[ i];
          }
        return ret;
      }
    }
    //no CAST. If OBJ type, return true of false, or ObjHeap index if  GET_HEAP_IDENTIFIER is used.
    if (pType == GDL_OBJ) {
      DObjGDL* pObj = static_cast<DObjGDL*> (p);

      if (e->KeywordSet(GET_HEAP_IDENTIFIERIx)) {
        DULongGDL* pL = new DULongGDL(p->Dim());
        Guard<DULongGDL> pL_guard(pL);
        for (SizeT i = 0; i < pL->N_Elements(); ++i) (*pL) [i] = (*pObj)[i]; //heap indexes
        if (isscalar) return new DULongGDL((*pL)[0]);
        else {
          pL_guard.release();
          return pL;
        }
      } else {
        if (isscalar) {
          return new DByteGDL(interpreter->ObjValid((*pObj)[ 0]));
        } else {
          DByteGDL* ret = new DByteGDL(p->Dim());
          for (SizeT i = 0; i < ret->N_Elements(); ++i) (*ret)[i] = interpreter->ObjValid((*pObj)[ i]);
          return ret;
        }
      }
    } else { // pType!=GDL_OBJ: return false = 0 always.
      if (isscalar) {
        if (e->KeywordSet(GET_HEAP_IDENTIFIERIx)) return new DULongGDL(0);
        else return new DByteGDL(0);
      } else {
        if (e->KeywordSet(GET_HEAP_IDENTIFIERIx)) return new DULongGDL(p->Dim());
        else return new DByteGDL(p->Dim());
      }
    }
  }


  //  {
  //    int nParam=e->NParam();
  //    static int CASTIx = e->KeywordIx("CAST");
  //    static int COUNTIx = e->KeywordIx("COUNT");
  //    static int GET_HEAP_IDENTIFIERIx = e->KeywordIx("GET_HEAP_IDENTIFIER");
  //
  //    if( e->KeywordPresent( COUNTIx)) // COUNT
  //      {
  //    e->SetKW( COUNTIx, new DLongGDL( e->Interpreter()->ObjHeapSize()));
  //      }
  //
  //    if( nParam == 0)
  //      {
  //    return e->Interpreter()->GetAllObjHeap();
  //      }
  //
  //    BaseGDL* p = e->GetPar( 0);
  //    if( p == NULL)
  //      {
  //    return new DByteGDL( 0);
  //      }
  //
  //    DType pType = p->Type();
  //    bool isscalar = p->StrictScalar();
  //    DLongGDL* pL;
  //    Guard<DLongGDL> pL_guard;
  //
  //    GDLInterpreter* interpreter = e->Interpreter();
  //    if( pType == GDL_OBJ) {
  //        DObjGDL* pObj = static_cast<DObjGDL*>( p);
  //        pL = new DLongGDL( p->Dim());
  //        for( SizeT i=0; i < pL->N_Elements(); ++i) (*pL) [i] = (*pObj)[i];
  //        if( e->KeywordSet( GET_HEAP_IDENTIFIERIx)) {
  //            if(isscalar) return new DLongGDL( (*pL)[0] );
  //                else    return pL;
  //            }
  //    }
  //    else {          // pType == GDL_OBJ
  //        pL = static_cast<DLongGDL*>(p->Convert2(GDL_LONG,BaseGDL::COPY));
  //        pL_guard.Init( pL);
  //        if( e->KeywordSet( CASTIx))  {
  //            if(isscalar) {
  //                DLong p0 = (*pL)[0];
  //                if(  interpreter->ObjValid( p0 )) {
  //                        interpreter->IncRefObj( p0);
  //                        return new DObjGDL( p0);
  //                } else  return new DObjGDL( 0);
  //            }
  //            DObjGDL* ret = new DObjGDL( pL->Dim());
  //            for( SizeT i=0; i < pL->N_Elements(); ++i)
  //              if( interpreter->ObjValid( (*pL)[ i])) {
  //                  interpreter->IncRefObj((*pL)[ i]);
  //                  (*ret)[ i] = (*pL)[ i];
  //                  }
  //          return ret;
  //          }
  //      }
  //
  //    DByteGDL* ret = new DByteGDL( pL->Dim()); // zero
  //    for( SizeT i=0; i<pL->N_Elements(); ++i)
  //      {
  //    if( interpreter->ObjValid( (*pL)[ i]))
  //      (*ret)[ i] = 1;
  //      }
  //
  //    if(isscalar) return new DByteGDL( (*ret)[0] );
  //       else return ret;
  //  }

  BaseGDL* obj_new(EnvT* e) {
    //     StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam = e->NParam();

    if (nParam == 0) {
      return new DObjGDL(0);
    }

    DString objName;
    e->AssureScalarPar<DStringGDL>(0, objName);

    // this is a struct name -> convert to UPPERCASE
    objName = StrUpCase(objName);
    if (objName == "IDL_OBJECT")
      objName = GDL_OBJECT_NAME; // replacement also done in GDLParser
    else if (objName == "IDL_CONTAINER")
      objName = GDL_CONTAINER_NAME;
    DStructDesc* objDesc = e->Interpreter()->GetStruct(objName, e->CallingNode());

    DStructGDL* objStruct = new DStructGDL(objDesc, dimension(1));

    DObj objID = e->NewObjHeap(1, objStruct); // owns objStruct

    DObjGDL* newObj = new DObjGDL(objID); // the object

    try {
      // call INIT function
      DFun* objINIT = objDesc->GetFun("INIT");
      if (objINIT != NULL) {
        StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());

        // morph to obj environment and push it onto the stack again
        e->PushNewEnvUD(objINIT, 1, &newObj);

        BaseGDL* res = e->Interpreter()->call_fun(objINIT->GetTree());

        if (res == NULL || (!res->Scalar()) || res->False()) {
          GDLDelete(res);
          return new DObjGDL(0);
        }
        GDLDelete(res);
      }
    } catch (...) {
      e->FreeObjHeap(objID); // newObj might be changed
      GDLDelete(newObj);
      throw;
    }

    return newObj;
  }

  BaseGDL* heap_refcount(EnvT* e) {
    static int DISABLEIx = e->KeywordIx("DISABLE");
    static int ENABLEIx = e->KeywordIx("ENABLE");
    static int IS_ENABLEDIx = e->KeywordIx("IS_ENABLED");
    //    trace_me = trace_arg();
    int nParam = e->NParam();

    GDLInterpreter* interpreter = e->Interpreter();

    if (nParam == 0) {
      if (e->KeywordSet(DISABLEIx)) {
        EnableGC(false);
      } else if (e->KeywordSet(ENABLEIx)) {
        EnableGC(true);
        interpreter->EnableAllGC();
      }
      if (e->KeywordPresent(IS_ENABLEDIx))
        e->SetKW(IS_ENABLEDIx, new DByteGDL(IsEnabledGC()));
      return new DIntGDL(0);
    }

    BaseGDL* p = e->GetPar(0);
    if (p == NULL) {
      return new DIntGDL(0);
    }

    DIntGDL* ret = new DIntGDL(p->Dim());
    Guard<DIntGDL> ret_guard(ret);
    DType pType = p->Type();
    SizeT nEl = p->N_Elements();
    if (pType == GDL_OBJ) {
      DObjGDL* pObj = static_cast<DObjGDL*> (p);
      for (SizeT i = 0; i < nEl; ++i)
        (*ret)[ i] = interpreter->RefCountHeapObj((*pObj)[ i]);
      if (e->KeywordSet(DISABLEIx) or
        e->KeywordSet(ENABLEIx)) {
        bool set = e->KeywordSet(ENABLEIx) ? true : false;
        interpreter->EnableGCObj(pObj, set);
      }
    } else {
      if (pType == GDL_PTR) {

        DPtrGDL* pPtr = static_cast<DPtrGDL*> (p);
        for (SizeT i = 0; i < nEl; ++i)
          (*ret)[ i] = interpreter->RefCountHeap((*pPtr)[ i]);
        if (e->KeywordSet(DISABLEIx) or
          e->KeywordSet(ENABLEIx)) {
          bool set = e->KeywordSet(ENABLEIx) ? true : false;
          //  if(trace_me) cout <<" GC set? "<<set<<endl;
          interpreter->EnableGC(pPtr, set);
        }
      } else {
        DLongGDL* pL;
        Guard<DLongGDL> pL_guard;
        if (pType != GDL_LONG) {
          pL = static_cast<DLongGDL*> (p->Convert2(GDL_LONG, BaseGDL::COPY));
          pL_guard.Init(pL);
        } else {
          pL = static_cast<DLongGDL*> (p);
        }
        for (SizeT i = 0; i < nEl; ++i)
          (*ret)[ i] = interpreter->RefCountHeap((*pL)[ i]);
      }
    }
    // #if 1
    if (e->KeywordPresent(IS_ENABLEDIx)) {
      DByteGDL* enabled;
      //      if(trace_me)
      //  cout << "  heap_refcount( prm, KeywordPresent(IS_ENABLEDIx)) "<< endl;
      if (pType == GDL_OBJ) {
        enabled = interpreter->IsEnabledGCObj(static_cast<DObjGDL*> (p));
      } else {
        if (pType == GDL_PTR) {
          enabled = interpreter->IsEnabledGC(static_cast<DPtrGDL*> (p));
        } else {
          //  if(trace_me)
          //  cout << " heap_refcount(prm=lonarr, KeywordPresent(IS_ENABLEDIx) "<< endl;
          DLongGDL* pL;
          Guard<DLongGDL> pL_guard;
          if (pType != GDL_LONG) {
            pL = static_cast<DLongGDL*> (p->Convert2(GDL_LONG, BaseGDL::COPY));
            pL_guard.Init(pL);
          } else {
            pL = static_cast<DLongGDL*> (p);
          }
          DPtrGDL* ptr = new DPtrGDL(p->Dim());
          Guard<DPtrGDL> ptr_guard(ptr);
          for (SizeT i = 0; i < nEl; ++i)
            (*ptr)[ i] = (*pL)[ i];

          enabled = interpreter->IsEnabledGC(ptr);
        }
      }
      e->SetKW(IS_ENABLEDIx, enabled);
    }
    // #endif
    return ret_guard.release();
  }
  
  BaseGDL* do_uindgen(dimension dim, DDouble off, DDouble inc) {
      //idl uses a size superior if necessary
    DLong64 max = off + dim.NDimElementsConst() * inc;
    DLong64 min = off;
    if (max < min) {
      min = max;
      max = off;
    }
    if (min < std::numeric_limits<DUInt>::min() || max > std::numeric_limits<DUInt>::max()) { 
      DULongGDL* iGen = new DULongGDL(dim, BaseGDL::INDGEN, off, inc);
      return iGen->Convert2(GDL_UINT);
    } else {
      return new DUIntGDL(dim, BaseGDL::INDGEN, off, inc);
    }
  }
  BaseGDL* do_lindgen(dimension dim, DDouble off, DDouble inc) {
    //idl uses a size superior if necessary
    DLong64 max=off+dim.NDimElementsConst()*inc;
    DLong64 min=off;
    if (max < min) { min=max; max=off;}
    if (min < std::numeric_limits<DLong>::min() || max > std::numeric_limits<DLong>::max() ) {
      DLong64GDL* iGen = new DLong64GDL(dim, BaseGDL::INDGEN, off, inc);
      return iGen->Convert2(GDL_LONG);
    } else {
      return new DLongGDL(dim, BaseGDL::INDGEN, off, inc);
    }
  }

  BaseGDL* do_indgen(dimension dim, DDouble off, DDouble inc) {
      //idl uses a size superior if necessary
    DLong64 max = off + dim.NDimElementsConst() * inc;
    DLong64 min = off;
    if (max < min) {
      min = max;
      max = off;
    }
    if (min < std::numeric_limits<DLong>::min() || max > std::numeric_limits<DLong>::max()) {   //and NOT numeric_limits<DInt> !! verified.
      DLong64GDL* iGen = new DLong64GDL(dim, BaseGDL::INDGEN, off, inc);
      return iGen->Convert2(GDL_INT);
    } else {
      return new DIntGDL(dim, BaseGDL::INDGEN, off, inc);
    }
  }  
  BaseGDL* do_bindgen(dimension dim, DDouble off, DDouble inc) {
    //idl uses a size superior if necessary
    DLong64 max = off + dim.NDimElementsConst() * inc;
    DLong64 min = off;
    if (max < min) {
      min = max;
      max = off;
    }
    if (min < std::numeric_limits<DUInt>::min() || max > std::numeric_limits<DUInt>::max()) {
      DULongGDL* iGen = new DULongGDL(dim, BaseGDL::INDGEN, off, inc);
      return iGen->Convert2(GDL_BYTE);
    } else {
      return new DByteGDL(dim, BaseGDL::INDGEN, off, inc);
    }
  }
  BaseGDL* do_sindgen(dimension dim, DDouble off, DDouble inc) {
    //idl uses a size superior if necessary
    DLong64 max = off + dim.NDimElementsConst() * inc;
    DLong64 min = off;
    if (max < min) {
      min = max;
      max = off;
    }
    if (min < std::numeric_limits<DLong>::min() || max > std::numeric_limits<DLong>::max()) {
      DLong64GDL* iGen = new DLong64GDL(dim, BaseGDL::INDGEN, off, inc);
      return iGen->Convert2(GDL_STRING);
    } else {
      DLongGDL* iGen = new DLongGDL(dim, BaseGDL::INDGEN, off, inc);
      return iGen->Convert2(GDL_STRING);
    }
  }
  

  
  BaseGDL* bindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return do_bindgen(dim, off, inc);
  }

  BaseGDL* indgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    DType type = GDL_INT;

    static int kwIx1 = e->KeywordIx("BYTE");
    if (e->KeywordSet(kwIx1)) {
      type = GDL_BYTE;
    }

    static int kwIx2 = e->KeywordIx("COMPLEX");
    if (e->KeywordSet(kwIx2)) {
      type = GDL_COMPLEX;
    }

    static int kwIx3 = e->KeywordIx("DCOMPLEX");
    if (e->KeywordSet(kwIx3)) {
      type = GDL_COMPLEXDBL;
    }

    static int kwIx4 = e->KeywordIx("DOUBLE");
    if (e->KeywordSet(kwIx4)) {
      type = GDL_DOUBLE;
    }

    static int kwIx5 = e->KeywordIx("FLOAT");
    if (e->KeywordSet(kwIx5)) {
      type = GDL_FLOAT;
    }

    static int kwIx6 = e->KeywordIx("L64");
    if (e->KeywordSet(kwIx6)) {
      type = GDL_LONG64;
    }

    static int kwIx7 = e->KeywordIx("LONG");
    if (e->KeywordSet(kwIx7)) {
      type = GDL_LONG;
    }

    static int kwIx8 = e->KeywordIx("STRING");
    if (e->KeywordSet(kwIx8)) {
      type = GDL_STRING;
    }

    static int kwIx9 = e->KeywordIx("UINT");
    if (e->KeywordSet(kwIx9)) {
      type = GDL_UINT;
    }

    static int kwIx10 = e->KeywordIx("UL64");
    if (e->KeywordSet(kwIx10)) {
      type = GDL_ULONG64;
    }

    static int kwIx11 = e->KeywordIx("ULONG");
    if (e->KeywordSet(kwIx11)) {
      type = GDL_ULONG;
    }

    // Seeing if the user passed in a TYPE code
    static int kwIx12 = e->KeywordIx("TYPE");
    if (e->KeywordPresent(kwIx12)) {
      DLong temp_long;
      e->AssureLongScalarKW(kwIx12, temp_long);
      type = static_cast<DType> (temp_long);
    }

    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);

    switch (type) {
    case GDL_INT: return do_indgen(dim, off, inc);
    case GDL_BYTE: return do_bindgen(dim,off,inc);
    case GDL_COMPLEX: return new DComplexGDL(dim, BaseGDL::INDGEN, off, inc);
    case GDL_COMPLEXDBL: return new DComplexDblGDL(dim, BaseGDL::INDGEN, off, inc);
    case GDL_DOUBLE: return new DDoubleGDL(dim, BaseGDL::INDGEN, off, inc);
    case GDL_FLOAT: return new DFloatGDL(dim, BaseGDL::INDGEN, off, inc);
    case GDL_LONG64: return new DLong64GDL(dim, BaseGDL::INDGEN, off, inc);
    case GDL_LONG: return do_lindgen(dim, off, inc);
    case GDL_STRING: return do_sindgen(dim, off, inc);
    case GDL_UINT: return do_uindgen(dim,off,inc);
    case GDL_ULONG64: return new DULong64GDL(dim, BaseGDL::INDGEN, off, inc);
    case GDL_ULONG: return new DULongGDL(dim, BaseGDL::INDGEN, off, inc);
    default:
      e->Throw("Invalid type code specified.");
      break;
    }
    assert(false);
    return NULL;
  }

  BaseGDL* uindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return do_uindgen(dim,off,inc);
  }

  BaseGDL* sindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return do_sindgen(dim, off, inc);
  }

  BaseGDL* lindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return do_lindgen(dim, off, inc);
  }

  BaseGDL* ulindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DULongGDL(dim, BaseGDL::INDGEN, off, inc);
  }

  BaseGDL* l64indgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DLong64GDL(dim, BaseGDL::INDGEN, off, inc);
  }

  BaseGDL* ul64indgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DULong64GDL(dim, BaseGDL::INDGEN, off, inc);
  }

  BaseGDL* findgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DFloatGDL(dim, BaseGDL::INDGEN, off, inc);
  }

  BaseGDL* dindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DDoubleGDL(dim, BaseGDL::INDGEN, off, inc);
  }

  BaseGDL* cindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DComplexGDL(dim, BaseGDL::INDGEN, off, inc);
  }

  BaseGDL* dcindgen(EnvT* e) {
    dimension dim;
    DDouble off = 0, inc = 1;
    arr(e, dim);
    if (dim[0] == 0)
      throw GDLException("Array dimensions must be greater than 0");

    e->AssureDoubleScalarKWIfPresent("START", off);
    e->AssureDoubleScalarKWIfPresent("INCREMENT", inc);
    return new DComplexDblGDL(dim, BaseGDL::INDGEN, off, inc);
  }

  // only called from CALL_FUNCTION
  // otherwise done directly in FCALL_LIB_N_ELEMENTSNode::Eval();
  // (but must be defined anyway for LibInit() for correct parametrization)
  // N_ELEMENTS is special because on error it just returns 0L
  // (the error is just caught and dropped)

  BaseGDL* n_elements(EnvT* e) {
    SizeT nParam = e->NParam(1);

    BaseGDL* p0 = e->GetPar(0);

    if (p0 == NULL)
      return new DLongGDL(0);
    if (p0->IsAssoc())
      return new DLongGDL(1);
    if (p0->Type() == GDL_OBJ) {
      DStructGDL* s = GetObjStruct(p0, e);
      if (s->Desc()->IsParent("LIST"))
        return new DLongGDL(LIST_count(s));
      else
        if (s->Desc()->IsParent("HASH"))
        return new DLongGDL(HASH_count(s));
    }
    if (p0->N_Elements() > 2147483647UL)
      return new DLong64GDL(p0->N_Elements());
    else
      return new DLongGDL(p0->N_Elements());
  }

  // GD Rewrote complex_fun_template_twopar for gain speed > 10.
  // compiler optimization, including openmp inner loops, depends terribly on the knowledge at the compilation time
  // of the exact nature of every value --- typenames do not help if they do not quickly resolve to known PODs.
  // the following construction has speeds on par with IDL's C . Note the test to avoid completely the openmp loop,
  // which seems to gain some time (?) and the use of auto and decltype() in std::complex.

  template< typename TypOutGDL, typename TypInGDL>
  BaseGDL* complex_fun_template_twopar(EnvT* e) {
    TypInGDL* re = e->GetParAs<TypInGDL>(0);
    TypInGDL* im = e->GetParAs<TypInGDL>(1);
    auto t = (*re)[0]; //a Float or Double
    if (re->Rank() == 0) {
      TypOutGDL* res = new TypOutGDL(im->Dim(), BaseGDL::NOZERO);
      SizeT nE = im->N_Elements();
      if ((GDL_NTHREADS=parallelize( nE))==1) {
        for (SizeT i = 0; i < nE; i++) (*res)[i] = std::complex<decltype(t)>((*re)[0], (*im)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
          for (OMPInt i = 0; i < nE; ++i) (*res)[i] = std::complex<decltype(t)>((*re)[0], (*im)[i]);
      }
      return res;
    } else if (im->Rank() == 0) {
      TypOutGDL* res = new TypOutGDL(re->Dim(), BaseGDL::NOZERO);
      SizeT nE = re->N_Elements();
      if ((GDL_NTHREADS=parallelize( nE))==1) {
        for (SizeT i = 0; i < nE; i++) (*res)[i] = std::complex<decltype(t)>((*re)[i], (*im)[0]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nE; ++i) (*res)[i] = std::complex<decltype(t)>((*re)[i], (*im)[0]);
      }
      return res;
    } else if (re->N_Elements() >= im->N_Elements()) {
      TypOutGDL* res = new TypOutGDL(im->Dim(), BaseGDL::NOZERO);
      SizeT nE = im->N_Elements();
      if ((GDL_NTHREADS=parallelize( nE))==1) {
        for (SizeT i = 0; i < nE; i++) (*res)[i] = std::complex<decltype(t)>((*re)[i], (*im)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
          for (OMPInt i = 0; i < nE; ++i) (*res)[i] = std::complex<decltype(t)>((*re)[i], (*im)[i]);
      }
      return res;
    } else {
      TypOutGDL* res = new TypOutGDL(re->Dim(), BaseGDL::NOZERO);
      SizeT nE = re->N_Elements();
      if ((GDL_NTHREADS=parallelize( nE))==1) {
        for (SizeT i = 0; i < nE; i++) (*res)[i] = std::complex<decltype(t)>((*re)[i], (*im)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nE; ++i) (*res)[i] = std::complex<decltype(t)>((*re)[i], (*im)[i]);
      }
      return res;
    }
  }

  template< class TargetClass>
  BaseGDL* type_fun(EnvT* e) { //nParam > 1
    SizeT nParam = e->NParam(1);
    BaseGDL* p0 = e->GetNumericParDefined(0);

    // GDL_BYTE( expr, offs, dim1,..,dim8)
    DLong offs;
    e->AssureLongScalarPar(1, offs);

    dimension dim;

    if (nParam > 2) arr(e, dim, 2);

    TargetClass* res = new TargetClass(dim, BaseGDL::NOZERO);

    SizeT nByteCreate = res->NBytes(); // net size of new data

    SizeT nByteSource = p0->NBytes(); // net size of src

    if (offs < 0 || (offs + nByteCreate) > nByteSource) {
      GDLDelete(res);
      e->Throw("Specified offset to"
        " expression is out of range: " + e->GetParString(0));
    }

    //*** POSSIBLE ERROR because of alignment here -- probably no?
    void* srcAddr = static_cast<void*> (static_cast<char*> (p0->DataAddr()) +
      offs);
    void* dstAddr = static_cast<void*> (&(*res)[0]);
    memcpy(dstAddr, srcAddr, nByteCreate);

    return res;
  }

  template< class TargetClass>
  BaseGDL* type_fun_single(EnvT* e) { //nParam=1
    BaseGDL* p0 = e->GetParDefined(0);

    assert(dynamic_cast<EnvUDT*> (e->Caller()) != NULL);

    // type_fun( expr) just convert
    if (static_cast<EnvUDT*> (e->Caller())->GetIOError() != NULL)
      return p0->Convert2(TargetClass::t, BaseGDL::COPY_THROWIOERROR);
      // SA: see tracker item no. 3151760
    else if (TargetClass::t == p0->Type() && e->GlobalPar(0))
      // HERE THE INPUT VARIABLE IS RETURNED
    {
      e->SetPtrToReturnValue(&e->GetPar(0));
      return p0;
    } else
      return p0->Convert2(TargetClass::t, BaseGDL::COPY);
    throw;
  }

  BaseGDL* byte_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DByteGDL>(e);
    return type_fun<DByteGDL>(e);
  }

  BaseGDL* int_fun(EnvT* e) //not registered, use FIX instead.
  {
    if (e->NParam() == 1) return type_fun_single<DIntGDL>(e);
    return type_fun<DIntGDL>(e);
  }

  BaseGDL* uint_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DUIntGDL>(e);
    return type_fun<DUIntGDL>(e);
  }

  BaseGDL* long_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DLongGDL>(e);
    return type_fun<DLongGDL>(e);
  }

  BaseGDL* ulong_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DULongGDL>(e);
    return type_fun<DULongGDL>(e);
  }

  BaseGDL* long64_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DLong64GDL>(e);
    return type_fun<DLong64GDL>(e);
  }

  BaseGDL* ulong64_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DULong64GDL>(e);
    return type_fun<DULong64GDL>(e);
  }

  BaseGDL* float_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DFloatGDL>(e);
    return type_fun<DFloatGDL>(e);
  }

  BaseGDL* double_fun(EnvT* e) {
    if (e->NParam() == 1) return type_fun_single<DDoubleGDL>(e);
    return type_fun<DDoubleGDL>(e);
  }

  BaseGDL* complex_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);
    static int doubleIx = e->KeywordIx("DOUBLE");
    bool noDouble = (!e->KeywordSet(doubleIx));
    if (noDouble) {
      if (nParam == 1) return type_fun_single<DComplexGDL>(e);
      if (nParam == 2) return complex_fun_template_twopar< DComplexGDL, DFloatGDL>(e);
      return type_fun<DComplexGDL>(e);
    } else {
      if (nParam == 1) return type_fun_single<DComplexDblGDL>(e);
      if (nParam == 2) return complex_fun_template_twopar< DComplexDblGDL, DDoubleGDL>(e);
      return type_fun<DComplexDblGDL>(e);
    }
  }

  BaseGDL* dcomplex_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);
    if (nParam == 1) return type_fun_single<DComplexDblGDL>(e);
    if (nParam == 2) return complex_fun_template_twopar< DComplexDblGDL, DDoubleGDL>(e);
    return type_fun<DComplexDblGDL>(e);
  }
  // STRING function behaves different

  BaseGDL* string_fun(EnvT* e) {
    SizeT nParam = e->NParam();

    if (nParam == 0)
      e->Throw("Incorrect number of arguments.");

    // AC 2016/02/12 we check now here if params are defined to avoid future problems
    // print, string(kk, 12, ee) said "ee" undefined because of VMS hack (should say kk undefined before !)
    // print, string(kk, 12, ee, format='()') did not complains
    //
    for (SizeT i = 0; i < nParam; ++i)
      BaseGDL * p = e->GetParDefined(i);
    static int printKeyIx = e->KeywordIx("PRINT");
    bool printKey = e->KeywordSet(printKeyIx);
    int parOffset = 0;

    // SA: handling special VMS-compatibility syntax, e.g.: string(1,'$(F)')
    //     (if nor FORMAT neither PRINT defined, >1 parameter, last param is scalar string
    //     which begins with "$(" or "(" but is not "()" then last param [minus "$"] is treated as FORMAT)
    bool vmshack = false;
    if (!printKey && (e->GetKW(0) == NULL) && nParam > 1) {
      vmshack = true;
      BaseGDL* par = e->GetParDefined(nParam - 1);
      if (par->Type() == GDL_STRING && par->Scalar()) {
        int dollar = (*static_cast<DStringGDL*> (par))[0].compare(0, 2, "$(");
        if (dollar == 0 || ((*static_cast<DStringGDL*> (par))[0].compare(0, 1, "(") == 0 && (*static_cast<DStringGDL*> (par))[0] != "()")) {
          e->SetKeyword("FORMAT", new DStringGDL(
            (*static_cast<DStringGDL*> (par))[0].c_str() + (dollar == 0 ? 1 : 0)
            ));
        }
      }
    }

    static int formatIx = e->KeywordIx("FORMAT");
    BaseGDL* format_kw = e->GetKW(formatIx);
    bool formatKey = format_kw != NULL;

    if (formatKey && format_kw->Type() == GDL_STRING && (*static_cast<DStringGDL*> (format_kw))[0] == "") formatKey = false;

    if (printKey || formatKey) // PRINT or FORMAT
    {
      stringstream os;

      SizeT width = 0;
      if (printKey) // otherwise: FORMAT -> width is ignored
      {
        // for /PRINT always a terminal width of 80 is assumed
        width = 80; //TermWidth();
      }

      if (vmshack) {
        parOffset = 1;
        e->ShiftParNumbering(1);
      }
      print_os(&os, e, parOffset, width);
      if (vmshack) {
        e->ShiftParNumbering(-1);
      }

      vector<DString> buf;
      while (os.good()) {
        string line;
        getline(os, line);
        if (!line.empty()) buf.push_back(line); //should save the day for the formats with '$' at end.
      }

      SizeT bufSize = buf.size();
      if (bufSize == 0) return new DStringGDL("");

      if (bufSize > 1) {
        DStringGDL* retVal =
          new DStringGDL(dimension(bufSize), BaseGDL::NOZERO);

        for (SizeT i = 0; i < bufSize; ++i)
          (*retVal)[ i] = buf[ i];

        return retVal;
      } else
        return new DStringGDL(buf[0]);
    } else {
      if (nParam == 1) // nParam == 1 -> conversion
      {
        BaseGDL* p0 = e->GetParDefined(0);
        // SA: see tracker item no. 3151760

        // HERE INPUT VARIABLE IS RETURNED
        if (p0->Type() == GDL_STRING && e->GlobalPar(0)) {
          e->SetPtrToReturnValue(&e->GetPar(0));
          return p0;
        }
        return p0->Convert2(GDL_STRING, BaseGDL::COPY);
      } else // concatenation
      {
        DString s;
        for (SizeT i = 0; i < nParam; ++i) {
          BaseGDL* p = e->GetParDefined(i);
          DStringGDL* sP = static_cast<DStringGDL*>
            (p->Convert2(GDL_STRING, BaseGDL::COPY_BYTE_AS_INT));

          SizeT nEl = sP->N_Elements();
          for (SizeT e = 0; e < nEl; ++e)
            s += (*sP)[ e];
          GDLDelete(sP);
        }
        // IDL here breaks the string into tty-width substrings
        return new DStringGDL(s);
      }
    }
  }

  //Simplified 'string' function to write out a string even for a structure, without the blanks and '{}' pretties usually present.
  BaseGDL* gdl_tostring_fun(EnvT* e) {
	SizeT nParam = e->NParam(1);
	static int formatIx = e->KeywordIx("FORMAT");
	BaseGDL* format_kw = e->GetKW(formatIx);
	bool formatKey = format_kw != NULL;

	if (formatKey && format_kw->Type() == GDL_STRING && (*static_cast<DStringGDL*> (format_kw))[0] == "") formatKey = false;
	stringstream os;
	print_os(&os, e, 0, 0);
	vector<DString> buf;
	while (os.good()) {
	  string line;
	  getline(os, line);
	  if (!line.empty()) buf.push_back(line); //should save the day for the formats with '$' at end.
	}

	SizeT bufSize = buf.size();
	if (bufSize == 0) return new DStringGDL("");

	if (bufSize > 1) {
	  DStringGDL* retVal =
		new DStringGDL(dimension(bufSize), BaseGDL::NOZERO);

	  for (SizeT i = 0; i < bufSize; ++i)
		(*retVal)[ i] = buf[ i];

	  return retVal;
	} else
	  return new DStringGDL(buf[0]);
  }

  BaseGDL* fix_fun(EnvT* e) {
    SizeT np = e->NParam(1);

    DIntGDL* type = e->IfDefGetKWAs<DIntGDL>(0); //"TYPE" keyword

    int typ = 0;
    if (type != NULL) { //see IDL's behaviour.
      typ = (*type)[0];
      if (typ > 15) typ = 0;
      if (typ < 0) typ = 0;
    }
    if (typ > 0) {
      if (typ == GDL_INT) return int_fun(e);
      if (typ == GDL_UINT) return uint_fun(e);
      if (typ == GDL_LONG) return long_fun(e);
      if (typ == GDL_ULONG) return ulong_fun(e);
      if (typ == GDL_LONG64) return long64_fun(e);
      if (typ == GDL_ULONG64) return ulong64_fun(e);
      if (typ == GDL_FLOAT) return float_fun(e);
      if (typ == GDL_DOUBLE) return double_fun(e);
      if (typ == GDL_COMPLEX) { //avoid problem with unexisting keyword "DOUBLE" in FIX's list of kws.
        if (np == 1) return type_fun_single<DComplexGDL>(e);
        return type_fun<DComplexGDL>(e);
      }
      if (typ == GDL_COMPLEXDBL) return dcomplex_fun(e);
      // 2 cases where PRINT has to be taken into account
      if (typ == GDL_BYTE) {
        static int printIx = e->KeywordIx("PRINT");
        if (e->KeywordSet(printIx) && e->GetPar(0)->Type() == GDL_STRING) {
          DLong64GDL* temp = static_cast<DLong64GDL*> (e->GetPar(0)->Convert2(GDL_LONG64, BaseGDL::COPY));
          SizeT nEl = temp->N_Elements();
          DByteGDL* ret = new DByteGDL(dimension(nEl),BaseGDL::NOZERO);
          for (SizeT i = 0; i < nEl; ++i) {
            (*ret)[i] = (*temp)[i];
          }
          (static_cast<BaseGDL*> (ret))->SetDim(e->GetPar(0)->Dim());
          GDLDelete(temp);
          return ret;
        } else
          return byte_fun(e);
      }
      if (typ == GDL_STRUCT) e->Throw("Unable to convert variable to type struct.");
      if (typ == GDL_PTR) e->Throw("Unable to convert variable to type pointer.");
      if (typ == GDL_OBJ) e->Throw("Unable to convert variable to type object reference.");

      if (typ == GDL_STRING) {
        // SA: calling GDL_STRING() with correct parameters
        int stringIx = LibFunIx("STRING");
        //assert(stringIx >= 0);

        EnvT* newEnv = new EnvT(e, libFunList[stringIx], NULL);

        Guard<EnvT> guard(newEnv);

        newEnv->SetNextPar(&e->GetPar(0)); // pass as global

        static int printIx = e->KeywordIx("PRINT");

        if (e->KeywordSet(printIx) && e->GetPar(0)->Type() == GDL_BYTE) {
          newEnv->SetKeyword("PRINT", new DIntGDL(1));
        }

        return static_cast<DLibFun*> (newEnv->GetPro())->Fun()(newEnv);
      }
    }
    return int_fun(e);
  }

  BaseGDL* call_function(EnvT* e) {
    int nParam = e->NParam();
    if (nParam == 0)
      e->Throw("No function specified.");

    DString callF;
    e->AssureScalarPar<DStringGDL>(0, callF);

    // this is a function name -> convert to UPPERCASE
    callF = StrUpCase(callF);

    // first search library funcedures
    int funIx = LibFunIx(callF);
    if (funIx != -1) {
      //  e->PushNewEnv( libFunList[ funIx], 1);
      // make the call
      //  EnvT* newEnv = static_cast<EnvT*>(e->Interpreter()->CallStack().back());

      // handle direct call functions
      if (libFunList[ funIx]->DirectCall()) {
        BaseGDL* directCallParameter = e->GetParDefined(1);
        BaseGDL* res =
          static_cast<DLibFunDirect*> (libFunList[ funIx])->FunDirect()(directCallParameter, true /*isReference*/);
        return res;
      } else {
        EnvT* newEnv = e->NewEnv(libFunList[ funIx], 1);
        Guard<EnvT> guard(newEnv);
        BaseGDL* res = static_cast<DLibFun*> (newEnv->GetPro())->Fun()(newEnv);
        e->SetPtrToReturnValue(newEnv->GetPtrToReturnValue());
        return res;
      }
    } else {
      // no direct call here

      funIx = GDLInterpreter::GetFunIx(callF);

      StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());

      EnvUDT* newEnv = e->PushNewEnvUD(funList[ funIx], 1);

      // make the call
      //  EnvUDT* newEnv = static_cast<EnvUDT*>(e->Interpreter()->CallStack().back());
      //GD: changed LRFUNCTION to RFUNCTION and removed e->SetPtrToReturnValue() below.
      //this solved bug #706
      newEnv->SetCallContext(EnvUDT::RFUNCTION);
      BaseGDL* res = e->Interpreter()->call_fun(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());
      //GD: removed   e->SetPtrToReturnValue( newEnv->GetPtrToReturnValue());
      //  BaseGDL* ppp = res->Dup();
      //  cout << " res = " << res << "  p to res = " << newEnv->GetPtrToReturnValue() << endl;
      return res;
    }
  }

  BaseGDL* call_method_function(EnvT* e) {
    int nParam = e->NParam();
    if (nParam < 2)
      e->Throw("Name and object reference must be specified.");

    DString callP;
    e->AssureScalarPar<DStringGDL>(0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase(callP);

    DStructGDL* oStruct = e->GetObjectPar(1);

    DFun* method = oStruct->Desc()->GetFun(callP);

    if (method == NULL)
      e->Throw("Method not found: " + callP);

    StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());

    EnvUDT* newEnv = e->PushNewEnvUD(method, 2, (DObjGDL**) & e->GetPar(1));

    // make the call
    //     return e->Interpreter()->call_fun( method->GetTree());
    newEnv->SetCallContext(EnvUDT::LRFUNCTION);
    BaseGDL* res = e->Interpreter()->call_fun(method->GetTree());
    e->SetPtrToReturnValue(newEnv->GetPtrToReturnValue());
    return res;
  }

  BaseGDL* execute_fun(EnvT* e) {
    int nParam = e->NParam(1);

    bool compileFlags = false;
    if (nParam >= 2) {
      BaseGDL* p1 = e->GetParDefined(1);

      if (!p1->Scalar())
        e->Throw("Expression must be scalar in this context: " +
        e->GetParString(1));

      // we do not enforce the case of Implied Print, then only 2 states
      compileFlags = p1->LogTrue();
    }

    bool quietExecution = false;
    if (nParam == 3) {
      BaseGDL* p2 = e->GetParDefined(2);

      if (!p2->Scalar())
        e->Throw("Expression must be scalar in this context: " +
        e->GetParString(2));

      quietExecution = p2->LogTrue();
      Warning("The QuietExecution argument of execute() is not yet supported by GDL.");
    }

    if (e->GetParDefined(0)->Rank() != 0)
      e->Throw("Expression must be scalar in this context: " + e->GetParString(0));

    DString line;
    e->AssureScalarPar<DStringGDL>(0, line);

    // remove current environment (own one)
    assert(dynamic_cast<EnvUDT*> (e->Caller()) != NULL);
    EnvUDT* caller = static_cast<EnvUDT*> (e->Caller());
    //     e->Interpreter()->CallStack().pop_back();

    // wrong: e is guarded, do not delete it here
    //  delete e;

    istringstream istr(line + "\n");

    RefDNode theAST;
    try {
      GDLLexer lexer(istr, "", caller->CompileOpt());
      GDLParser& parser = lexer.Parser();

      parser.interactive();

      theAST = parser.getAST();
    }    catch (GDLException& ex) {
      if (!compileFlags) GDLInterpreter::ReportCompileError(ex);
      return new DIntGDL(0);
    }    catch (ANTLRException& ex) {
      if (!compileFlags) cerr << "EXECUTE: Lexer/Parser exception: " <<
        ex.getMessage() << endl;
      return new DIntGDL(0);
    }

    if (theAST == NULL) return new DIntGDL(1);

    RefDNode trAST;
    try {
      GDLTreeParser treeParser(caller);

      treeParser.interactive(theAST);

      trAST = treeParser.getAST();
    }    catch (GDLException& ex) {
      if (!compileFlags) GDLInterpreter::ReportCompileError(ex);
      return new DIntGDL(0);
    } catch (ANTLRException& ex) {
      if (!compileFlags) cerr << "EXECUTE: Compiler exception: " <<
        ex.getMessage() << endl;
      return new DIntGDL(0);
    }

    if (trAST == NULL) return new DIntGDL(1);

    int nForLoopsIn = caller->NForLoops();
    try {
      ProgNodeP progAST = ProgNode::NewProgNode(trAST);
      Guard< ProgNode> progAST_guard(progAST);

      int nForLoops = ProgNode::NumberForLoops(progAST, nForLoopsIn);
      caller->ResizeForLoops(nForLoops);

      progAST->setLine(e->GetLineNumber());

      // AC 2016-02-26 : bug report #692 always verbose in EXECUTE()
      // Do we have a way not to *always* issue a message here
      // in case of problem ???
      RetCode retCode = caller->Interpreter()->execute(progAST);

      caller->ResizeForLoops(nForLoopsIn);

      if (retCode == RC_OK)
        return new DIntGDL(1);
      else
        return new DIntGDL(0);
    }    catch (GDLException& ex) {
      caller->ResizeForLoops(nForLoopsIn);
      // are we throwing to target environment?
      //      if( ex.GetTargetEnv() == NULL)
      if (!compileFlags) cerr << "EXECUTE: " <<
        ex.getMessage() << endl;
      return new DIntGDL(0);
    }    catch (ANTLRException& ex) {
      caller->ResizeForLoops(nForLoopsIn);

      if (!compileFlags) cerr << "EXECUTE: Interpreter exception: " <<
        ex.getMessage() << endl;
      return new DIntGDL(0);
    }

    return new DIntGDL(0); // control flow cannot reach here - compiler shut up
  }

  BaseGDL* assoc(EnvT* e) {
    SizeT nParam = e->NParam(2);

    DLong lun;
    e->AssureLongScalarPar(0, lun);

    bool stdLun = check_lun(e, lun);
    if (stdLun)
      e->Throw("File unit does not allow"
      " this operation. Unit: " + i2s(lun));

    DLong offset = 0;
    if (nParam >= 3) e->AssureLongScalarPar(2, offset);

    BaseGDL* arr = e->GetParDefined(1);

    if (arr->StrictScalar())
      e->Throw("Scalar variable not allowed in this"
      " context: " + e->GetParString(1));

    return arr->AssocVar(lun, offset);
  }

  // gdl_ naming because of weired namespace problem in MSVC

  BaseGDL* gdl_logical_and(EnvT* e) {
    SizeT nParam = e->NParam();
    if (nParam != 2)
      e->Throw(
      "Incorrect number of arguments.");

    BaseGDL* e1 = e->GetParDefined(0); //, "LOGICAL_AND");
    BaseGDL* e2 = e->GetParDefined(1); //, "LOGICAL_AND");

    ULong nEl1 = e1->N_Elements();
    ULong nEl2 = e2->N_Elements();

    Data_<SpDByte>* res;

    if (e1->Scalar()) {
      if (e1->LogTrue(0)) {
        res = new Data_<SpDByte>(e2->Dim(), BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( nEl2))==1) {
          for (SizeT i = 0; i < nEl2; i++) (*res)[i] = e2->LogTrue(i) ? 1 : 0;

        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl2; i++) (*res)[i] = e2->LogTrue(i) ? 1 : 0;
        }
      } else {
        return new Data_<SpDByte>(e2->Dim());
      }
    } else if (e2->Scalar()) {
      if (e2->LogTrue(0)) {
        res = new Data_<SpDByte>(e1->Dim(), BaseGDL::NOZERO);
        if ((GDL_NTHREADS=parallelize( nEl1))==1) {
          for (SizeT i = 0; i < nEl1; i++) (*res)[i] = e1->LogTrue(i) ? 1 : 0;
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl1; i++) (*res)[i] = e1->LogTrue(i) ? 1 : 0;
        }
      } else {
        return new Data_<SpDByte>(e1->Dim());
      }
    } else if (nEl2 <= nEl1) {
      res = new Data_<SpDByte>(e2->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl2))==1) {
        for (SizeT i = 0; i < nEl2; i++) (*res)[i] = (e1->LogTrue(i) && e2->LogTrue(i)) ? 1 : 0;
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl2; i++) (*res)[i] = (e1->LogTrue(i) && e2->LogTrue(i)) ? 1 : 0;
      }
    } else // ( nEl2 > nEl1)
    {
      res = new Data_<SpDByte>(e1->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl1))==1) {
        for (SizeT i = 0; i < nEl1; i++) (*res)[i] = (e1->LogTrue(i) && e2->LogTrue(i)) ? 1 : 0;
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl1; i++) (*res)[i] = (e1->LogTrue(i) && e2->LogTrue(i)) ? 1 : 0;
      }
    }
    return res;
  }

  // gdl_ naming because of weired namespace problem in MSVC

  BaseGDL* gdl_logical_or(EnvT* e) {
    SizeT nParam = e->NParam();
    if (nParam != 2)
      e->Throw(
      "Incorrect number of arguments.");

    BaseGDL* e1 = e->GetParDefined(0); //, "LOGICAL_OR");
    BaseGDL* e2 = e->GetParDefined(1); //, "LOGICAL_OR");

    ULong nEl1 = e1->N_Elements();
    ULong nEl2 = e2->N_Elements();

    Data_<SpDByte>* res;

    if (e1->Scalar()) {
      res = new Data_<SpDByte>(e2->Dim(), BaseGDL::NOZERO);
      if (e1->LogTrue(0)) {
        if ((GDL_NTHREADS=parallelize( nEl2))==1) {
          for (SizeT i = 0; i < nEl2; i++) (*res)[i] = 1;
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl2; i++) (*res)[i] = 1;
        }
      } else {
        if ((GDL_NTHREADS=parallelize( nEl2))==1) {
          for (SizeT i = 0; i < nEl2; i++) (*res)[i] = e2->LogTrue(i) ? 1 : 0;
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl2; i++) (*res)[i] = e2->LogTrue(i) ? 1 : 0;
        }
      }
    } else if (e2->Scalar()) {
      res = new Data_<SpDByte>(e1->Dim(), BaseGDL::NOZERO);
      if (e2->LogTrue(0)) {
        if ((GDL_NTHREADS=parallelize( nEl1))==1) {
          for (SizeT i = 0; i < nEl1; i++) (*res)[i] = 1;
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl1; i++) (*res)[i] = 1;
        }
      } else {
        if ((GDL_NTHREADS=parallelize( nEl1))==1) {
          for (SizeT i = 0; i < nEl1; i++) (*res)[i] = e1->LogTrue(i) ? 1 : 0;
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (OMPInt i = 0; i < nEl1; i++) (*res)[i] = e1->LogTrue(i) ? 1 : 0;
        }
      }
    } else if (nEl2 < nEl1) {
      res = new Data_<SpDByte>(e2->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl2))==1) {
        for (SizeT i = 0; i < nEl2; i++) (*res)[i] = (e1->LogTrue(i) || e2->LogTrue(i)) ? 1 : 0;
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl2; i++) (*res)[i] = (e1->LogTrue(i) || e2->LogTrue(i)) ? 1 : 0;
      }
    } else { // ( nEl2 >= nEl1)
      res = new Data_<SpDByte>(e1->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl1))==1) {
        for (SizeT i = 0; i < nEl1; i++) (*res)[i] = (e1->LogTrue(i) || e2->LogTrue(i)) ? 1 : 0;
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl1; i++) (*res)[i] = (e1->LogTrue(i) || e2->LogTrue(i)) ? 1 : 0;
      }
    }
    return res;
  }

  BaseGDL* logical_true(BaseGDL* e1, bool isReference)//( EnvT* e);
  {
    if (e1->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");

    ULong nEl1 = e1->N_Elements();

    Data_<SpDByte>* res = new Data_<SpDByte>(e1->Dim(), BaseGDL::NOZERO);
    if ((GDL_NTHREADS=parallelize( nEl1))==1) {
      for (SizeT i = 0; i < nEl1; i++) (*res)[i] = e1->LogTrue(i) ? 1 : 0;
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl1; i++) (*res)[i] = e1->LogTrue(i) ? 1 : 0;
    }
    return res;
  }

  BaseGDL* replicate(EnvT* e) {
    SizeT nParam = e->NParam(2);
    dimension dim;
    arr(e, dim, 1);

    BaseGDL* p0 = e->GetParDefined(0); //, "REPLICATE");
    if (!p0->Scalar())
      e->Throw("Expression must be a scalar in this context: " +
      e->GetParString(0));

    return p0->New(dim, BaseGDL::INIT);
  }
  static const std::string trimmable(" \t");


  //leading blanks

  inline void trim1(std::string &s) {
    std::size_t found = s.find_first_not_of(trimmable);
    if (found != std::string::npos)
      s.erase(0, found);
    else
      s.clear();
  }
  //trailing blanks

  inline void trim0(std::string &s) {
    std::size_t found = s.find_last_not_of(trimmable);
    if (found != std::string::npos)
      s.erase(found + 1);
    else
      s.clear();
  }

  inline void trim2(std::string &s) {
    trim0(s);
    trim1(s);
  }

  BaseGDL* strtrim(EnvT* e) {
    SizeT nParam = e->NParam(1); //, "STRTRIM");

    BaseGDL* p0 = e->GetPar(0);
    if (p0 == NULL) e->Throw("Variable is undefined: " + e->GetParString(0));

    DLong mode = 0;
    if (nParam == 2) {
      e->AssureLongScalarPar(1, mode);

      if (mode < 0 || mode > 2) {
        ostringstream os;
        e->GetPar(1)->ToStream(os);
        e->Throw("Value of <" + e->GetPar(1)->TypeStr() + "  (" + os.str() +
          ")> is out of allowed range.");
      }
    }

    DStringGDL* res;

    if (p0->Type() == GDL_STRING)
      res = static_cast<DStringGDL*> (p0->Dup());
    else {
      res = static_cast<DStringGDL*> (p0->Convert2(GDL_STRING, BaseGDL::COPY));
    }

    SizeT nEl = res->N_Elements();

    if (mode == 2) // both
    {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) trim2((*res)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) trim2((*res)[i]);
      }
    } else if (mode == 1) // leading
    {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) trim1((*res)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) trim1((*res)[i]);
      }
    } else // trailing
    {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) trim0((*res)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) trim0((*res)[i]);
      }
    }
    return res;
  }

  BaseGDL* strcompress(EnvT* e) {
    e->NParam(1);

    DStringGDL* p0S = e->GetParAs<DStringGDL>(0);

    bool removeAll = e->KeywordSet("REMOVE_ALL");

    DStringGDL* res = new DStringGDL(p0S->Dim(), BaseGDL::NOZERO);

    SizeT nEl = p0S->N_Elements();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = StrCompress((*p0S)[ i], removeAll);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) {
        (*res)[ i] = StrCompress((*p0S)[ i], removeAll);
      }
    }
    return res;
  }

  BaseGDL* strpos(EnvT* e) {
    SizeT nParam = e->NParam(2); //, "STRPOS");

    static int revOffsetIdx=e->KeywordIx("REVERSE_OFFSET");
    bool reverseOffset = e->KeywordSet(revOffsetIdx);

    static int revSearchIdx=e->KeywordIx("REVERSE_SEARCH");
    bool reverseSearch = e->KeywordSet(revSearchIdx);

    DStringGDL* p0S = e->GetParAs<DStringGDL>(0);

    DString searchString;
    //     e->AssureScalarPar<DStringGDL>( 1, searchString);
    DStringGDL* sStr = e->GetParAs<DStringGDL>(1);
    if (!sStr->Scalar(searchString))
      e->Throw("Search string must be a scalar or one element array: " +
      e->GetParString(1));

    long pos = -1; //string::npos
    if (nParam > 2) {
      BaseGDL* p2 = e->GetParDefined(2);
      const SizeT pIx = 2;
      BaseGDL* p = e->GetParDefined(pIx);
      DLongGDL* lp = static_cast<DLongGDL*> (p->Convert2(GDL_LONG, BaseGDL::COPY));
      Guard<DLongGDL> guard_lp(lp);
      DLong scalar;
      if (!lp->Scalar(scalar))
        throw GDLException("Parameter must be a scalar in this context: " +
        e->GetParString(pIx));
      pos = scalar;
    }

    DLongGDL* res = new DLongGDL(p0S->Dim(), BaseGDL::NOZERO);

    SizeT nEl = p0S->N_Elements();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (OMPInt i = 0; i < nEl; ++i) {
        (*res)[ i] = StrPos((*p0S)[ i], searchString, pos, reverseOffset, reverseSearch);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nEl; ++i) {
        (*res)[ i] = StrPos((*p0S)[ i], searchString, pos, reverseOffset, reverseSearch);
      }
    }
    return res;
  }

  BaseGDL* strmid(EnvT* e) {
    SizeT nParam = e->NParam(2); //, "STRMID");

    static int revOffsetIdx=e->KeywordIx("REVERSE_OFFSET");
    bool reverse = e->KeywordSet(revOffsetIdx);

    DStringGDL* p0S = e->GetParAs<DStringGDL>(0);
    DLongGDL* p1L = e->GetParAs<DLongGDL>(1);

    //     BaseGDL*  p2  = e->GetPar( 2);
    DLongGDL* p2L = NULL;
    if (nParam > 2) p2L = e->GetParAs<DLongGDL>(2);

    DLong scVal1;
    bool sc1 = p1L->Scalar(scVal1);

    DLong scVal2 = numeric_limits<DLong>::max();
    bool sc2 = true;
    if (p2L != NULL) {
      DLong scalar;
      sc2 = p2L->Scalar(scalar);
      scVal2 = scalar;
    }

    DLong stride;
    if (!sc1 && !sc2) {
      stride = p1L->Dim(0);
      if (stride != p2L->Dim(0))
        e->Throw("Starting offset and length arguments "
        "have incompatible first dimension.");
    } else {
      // at least one scalar, p2L possibly NULL
      if (p2L == NULL)
        stride = p1L->Dim(0);
      else
        stride = max(p1L->Dim(0), p2L->Dim(0));

      stride = (stride > 0) ? stride : 1;
    }

    dimension resDim(p0S->Dim());
    if (stride > 1)
      resDim >> stride;

    DStringGDL* res = new DStringGDL(resDim, BaseGDL::NOZERO);

    SizeT nEl1 = p1L->N_Elements();
    SizeT nEl2 = (sc2) ? 1 : p2L->N_Elements();

    SizeT nSrcStr = p0S->N_Elements();
    if (nSrcStr == 1) {
      // possibly this optimization is not worth the longer code (as the gain can only be a small fraction
      // of the overall time), but then this is a very common use
      for (long ii = 0; ii < stride; ++ii) {
        SizeT destIx = ii;
        DLong actFirst = (sc1) ? scVal1 : (*p1L)[ destIx % nEl1];
        DLong actLen = (sc2) ? scVal2 : (*p2L)[ destIx % nEl2];
        if (actLen <= 0)
          (*res)[ destIx] = ""; //StrMid((*p0S)[ i], actFirst, actLen, reverse);
        else
          (*res)[ destIx] = StrMid((*p0S)[ 0], actFirst, actLen, reverse);
      }
      return res;
    }
    if ((GDL_NTHREADS=parallelize(nSrcStr))==1) {
      for (OMPInt i = 0; i < nSrcStr; ++i) {
        for (long ii = 0; ii < stride; ++ii) {
          SizeT destIx = i * stride + ii;
          DLong actFirst = (sc1) ? scVal1 : (*p1L)[ destIx % nEl1];
          DLong actLen = (sc2) ? scVal2 : (*p2L)[ destIx % nEl2];
          if (actLen <= 0)
            (*res)[ destIx] = ""; //StrMid((*p0S)[ i], actFirst, actLen, reverse);
          else
            (*res)[ destIx] = StrMid((*p0S)[ i], actFirst, actLen, reverse);
        }
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) default( shared)
        for (OMPInt i = 0; i < nSrcStr; ++i) {
        for (long ii = 0; ii < stride; ++ii) {
          SizeT destIx = i * stride + ii;
          DLong actFirst = (sc1) ? scVal1 : (*p1L)[ destIx % nEl1];
          DLong actLen = (sc2) ? scVal2 : (*p2L)[ destIx % nEl2];
          if (actLen <= 0)
            (*res)[ destIx] = ""; //StrMid((*p0S)[ i], actFirst, actLen, reverse);
          else
            (*res)[ destIx] = StrMid((*p0S)[ i], actFirst, actLen, reverse);
        }
      }
    }
    return res;
  }

  BaseGDL* strlowcase(BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    DStringGDL* p0S;
    DStringGDL* res;
    Guard<DStringGDL> guard;

    if (p0->Type() == GDL_STRING)
      p0S = static_cast<DStringGDL*> (p0);
    else {
      p0S = static_cast<DStringGDL*> (p0->Convert2(GDL_STRING, BaseGDL::COPY));
      guard.Reset(p0S);
      isReference = true;
    }

    SizeT nEl = p0S->N_Elements();

    if (isReference) {
      res = new DStringGDL(p0S->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = StrLowCase((*p0S)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = StrLowCase((*p0S)[ i]);
      }
    } else {
      res = p0S;
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) StrLowCaseInplace((*p0S)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) StrLowCaseInplace((*p0S)[ i]);
      }
    }
    return res;
  }

  BaseGDL* strupcase(BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    DStringGDL* p0S;
    DStringGDL* res;
    Guard<DStringGDL> guard;

    if (p0->Type() == GDL_STRING)
      p0S = static_cast<DStringGDL*> (p0);
    else {
      p0S = static_cast<DStringGDL*> (p0->Convert2(GDL_STRING, BaseGDL::COPY));
      guard.Reset(p0S);
      isReference = true;
    }

    SizeT nEl = p0S->N_Elements();

    if (isReference) {
      res = new DStringGDL(p0S->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = StrUpCase((*p0S)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) (*res)[ i] = StrUpCase((*p0S)[ i]);
      }
    } else {
      res = p0S;
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) StrUpCaseInplace((*p0S)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) StrUpCaseInplace((*p0S)[ i]);
      }
    }
    return res;
  }

  BaseGDL* strlen(BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    DStringGDL* p0S;
    Guard<DStringGDL> guard;

    if (p0->Type() == GDL_STRING)
      p0S = static_cast<DStringGDL*> (p0);
    else {
      p0S = static_cast<DStringGDL*> (p0->Convert2(GDL_STRING, BaseGDL::COPY));
      guard.Reset(p0S);
    }

    DLongGDL* res = new DLongGDL(p0S->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0S->N_Elements();
    for (SizeT i = 0; i < nEl; ++i) {
      (*res)[ i] = (*p0S)[ i].length();
    }
    return res;
  }

  //GD: cannot be made faster if we keep using std::string . Performance is not good wrt idl.
  BaseGDL* strjoin(EnvT* e) {
    SizeT nParam = e->NParam(1);

    DStringGDL* p0S = e->GetParAs<DStringGDL>(0);
    SizeT nEl = p0S->N_Elements();

    DString delim = "";
    if (nParam > 1)
      e->AssureStringScalarPar(1, delim);

    bool single = e->KeywordSet("SINGLE");

    if (single) {
      DStringGDL* res = new DStringGDL((*p0S)[0]);
      DString& scl = (*res)[0];

      for (SizeT i = 1; i < nEl; ++i)
        scl += delim + (*p0S)[i];

      return res;
    }

    dimension resDim(p0S->Dim());
    resDim.Purge();

    SizeT stride = resDim.Stride(1);

    resDim.Remove(0);

    DStringGDL* res = new DStringGDL(resDim, BaseGDL::NOZERO);
    for (SizeT src = 0, dst = 0; src < nEl; ++dst) {
      (*res)[ dst] = (*p0S)[ src++];
      for (SizeT l = 1; l < stride; ++l)
        (*res)[ dst] += delim + (*p0S)[ src++];
    }

    return res;
  }

  BaseGDL* n_params(EnvT* e) {
    EnvUDT* caller = static_cast<EnvUDT*> (e->Caller());
    if (caller == NULL) return new DLongGDL(0);
    DLong nP = caller->NParam();
    if (caller->IsObject())
      return new DLongGDL(nP - 1); // "self" is not counted
    return new DLongGDL(nP);
  }
  //keyword_set returns 1 (true) if:
  //
  //    Expression is a scalar or 1-element array with a non-zero value.
  //    Expression is a structure or a n-element array, n>1
  //    Expression is an ASSOC file variable.  ---> not done?
  //
  //KEYWORD_SET returns 0 (false) if:
  //
  //    Expression is undefined.
  //    Expression is a scalar or 1-element array with a zero value.

  BaseGDL* keyword_set(EnvT* e) {
    e->NParam(1);

    BaseGDL* p0 = e->GetPar(0);
    if (p0 == NULL) return new DIntGDL(0);
    if (p0->Type() == GDL_UNDEF) return new DIntGDL(0);
    if (!p0->Scalar()) return new DIntGDL(1);
    if (p0->Type() == GDL_STRUCT) return new DIntGDL(1);
    if (p0->LogTrue()) return new DIntGDL(1);
    return new DIntGDL(0);
  }

  // passing 2nd argument by value is slightly better for float and double,
  // but incur some overhead for the complex class.

  template<class T> inline void AddOmitNaN(T& dest, T value) {
    if (std::isfinite(value)) {
      dest += value;
    }
  }

  template<class T> inline void AddOmitNaNCpx(T& dest, T value) {
    dest += T(std::isfinite(value.real()) ? value.real() : 0, std::isfinite(value.imag()) ? value.imag() : 0);
  }

  template<> inline void AddOmitNaN(DComplex& dest, DComplex value) {
    AddOmitNaNCpx<DComplex>(dest, value);
  }

  template<> inline void AddOmitNaN(DComplexDbl& dest, DComplexDbl value) {
    AddOmitNaNCpx<DComplexDbl>(dest, value);
  }

  template<class T> inline void NaN2Zero(T& value) {
    if (!std::isfinite(value)) value = 0;
  }

  template<class T> inline void NaN2ZeroCpx(T& value) {
    value = T(std::isfinite(value.real()) ? value.real() : 0, std::isfinite(value.imag()) ? value.imag() : 0);
  }

  template<> inline void NaN2Zero(DComplex& value) {
    NaN2ZeroCpx< DComplex>(value);
  }

  template<> inline void NaN2Zero(DComplexDbl& value) {
    NaN2ZeroCpx< DComplexDbl>(value);
  }

  // total over all elements, preserve type

  template<class T>
  BaseGDL* total_template_generic(T* src, bool omitNaN) {
    SizeT nEl = src->N_Elements();
    typename T::Ty sum = 0;
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      if (!omitNaN) for (SizeT i = 0; i < nEl; ++i) sum += (*src)[ i];
      else for (SizeT i = 0; i < nEl; ++i) if (isfinite((*src)[i])) sum += (*src)[ i];
    } else {
      if (!omitNaN) {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum) 
          for (OMPInt i = 0; i < nEl; ++i) sum += (*src)[ i];
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum)
          for (OMPInt i = 0; i < nEl; ++i) if (isfinite((*src)[i])) sum += (*src)[ i];
      }
    }
    return new T(sum);
  }

  template<>
  BaseGDL* total_template_generic(DComplexGDL* src, bool omitNaN) {
    //    std::cerr << " total_template_generic_DComplexGdl " << std::endl;
    SizeT nEl = src->N_Elements();
    DFloat sr = 0;
    DFloat si = 0;
    if (!omitNaN) {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) {
          sr += (*src)[i].real();
          si += (*src)[i].imag();
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sr,si)
          for (OMPInt i = 0; i < nEl; ++i) {
          sr += (*src)[i].real();
          si += (*src)[i].imag();
        }
      }
    } else {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) {
          if (isfinite((*src)[i].real())) sr += (*src)[i].real();
          if (isfinite((*src)[i].imag())) si += (*src)[i].imag();
        }
      } else {

        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel num_threads(GDL_NTHREADS)
        {
          DFloat lsr = 0;
          DFloat lsi = 0;
#pragma omp for nowait
          for (SizeT i = 0; i < nEl; ++i) {
            if (isfinite((*src)[i].real())) lsr += (*src)[i].real();
            if (isfinite((*src)[i].imag())) lsi += (*src)[i].imag();
          }
#pragma omp atomic
          sr += lsr;
          si += lsi;
        }
      }
    }
    return new DComplexGDL(std::complex<float>(sr, si));
  }

  template<>
  BaseGDL* total_template_generic(DComplexDblGDL* src, bool omitNaN) {
    //    std::cerr << " total_template_generic_DcomplexGDlDbl " << std::endl;
    SizeT nEl = src->N_Elements();
    DDouble sr = 0;
    DDouble si = 0;
    if (!omitNaN) {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) {
          sr += (*src)[i].real();
          si += (*src)[i].imag();
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sr,si)
          for (OMPInt i = 0; i < nEl; ++i) {
          sr += (*src)[i].real();
          si += (*src)[i].imag();
        }
      }
    } else {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) {
          if (isfinite((*src)[i].real())) sr += (*src)[i].real();
          if (isfinite((*src)[i].imag())) si += (*src)[i].imag();
        }
      } else {

        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel num_threads(GDL_NTHREADS)
        {
          DDouble lsr = 0;
          DDouble lsi = 0;
#pragma omp for nowait
          for (OMPInt i = 0; i < nEl; ++i) {
            if (isfinite((*src)[i].real())) lsr += (*src)[i].real();
            if (isfinite((*src)[i].imag())) lsi += (*src)[i].imag();
          }
#pragma omp atomic
          sr += lsr;
          si += lsi;
        }
      }
    }
    return new DComplexDblGDL(std::complex<double>(sr, si));
  }

  // total over all elements, done on Double. Avoids costly convert!

  template<class T>
  DDoubleGDL* total_template_double(T* src, bool omitNaN) {
    //   std::cerr<<" total_template_double "<<std::endl;
    SizeT nEl = src->N_Elements();
    DDouble sum = 0;
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      if (!omitNaN) {
        for (SizeT i = 0; i < nEl; ++i) sum += (*src)[ i];
      } else {
        for (SizeT i = 0; i < nEl; ++i) if (isfinite((*src)[i])) sum += (*src)[ i];
      }
    } else {
      if (!omitNaN) {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum)
          for (OMPInt i = 0; i < nEl; ++i) sum += (*src)[ i];
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum)
          for (OMPInt i = 0; i < nEl; ++i) if (isfinite((*src)[i])) sum += (*src)[ i];
      }
    }
    return new DDoubleGDL(sum);
  }

  template<class T>
  DFloatGDL* total_template_single(T* src, bool omitNaN) {
    //   std::cerr<<" total_template_single "<<std::endl;
    SizeT nEl = src->N_Elements();
    DDouble sum = 0;
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      if (!omitNaN) {
        for (SizeT i = 0; i < nEl; ++i) sum += (*src)[ i];
      } else {
        for (SizeT i = 0; i < nEl; ++i) if (isfinite((*src)[i])) sum += (*src)[ i];
      }
    } else {
      if (!omitNaN) {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum)
          for (OMPInt i = 0; i < nEl; ++i) sum += (*src)[ i];
      } else {

        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum)
          for (OMPInt i = 0; i < nEl; ++i) if (isfinite((*src)[i])) sum += (*src)[ i];
      }
    }
    return new DFloatGDL(sum);
  }

  //special case for /INT  using LONG64

  template<class T>
  DLong64GDL* total_template_integer(T* src) {
    //   std::cerr<<" total_template_integer "<<std::endl;
    SizeT nEl = src->N_Elements();
    DLong64 sum = 0;
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) sum += (*src)[ i];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:sum)
        for (OMPInt i = 0; i < nEl; ++i) sum += (*src)[ i];
    }
    return new DLong64GDL(sum);
  }

  template<typename T1, typename T2>
  BaseGDL* total_cu_template(T1* val, bool omitNaN) {
    typename T1::Ty *res;
    SizeT nEl = val->N_Elements();
    res = static_cast<T2*> (val->DataAddr());
    if (omitNaN) {
      for (SizeT i = 0; i < nEl; ++i) NaN2Zero(res[i]);
    }
    //this formulation is slightly faster on my machine
    for (SizeT i = 1, ii = 0; i < nEl; ++i, ++ii) res[i] += res[ii];
    //      for (SizeT i = 1; i < nEl; ++i) res[i] += res[i-1];
    return val;
  }

  // total over one dim
  //this version is much faster as the larger array (src) is explored linearly.
  template< typename T>
  BaseGDL* total_over_dim_template(T* src,
    const dimension& srcDim,
    SizeT sumDimIx, bool omitNaN) {
    SizeT nEl = src->N_Elements();

    // get dest dim and number of summations
    dimension destDim = srcDim;

    SizeT nSum = destDim.Remove(sumDimIx);

    T* res = new T(destDim); // zero fields

    // sumStride is also the number of linear src indexing
    SizeT sumStride = srcDim.Stride(sumDimIx);
    SizeT outerStride = srcDim.Stride(sumDimIx + 1);
	//compare sumStride and nEl/outerStride. Use best suited method, one uses expensive integer division times nEl/outerStride the other a test x sumStride.
	// estimated ratio is ~20 times faster for the test
	if (2*outerStride > 20* nEl / outerStride+sumStride ) { // outerStride tests +outerStride ops
    if (omitNaN) {
      for (SizeT o = 0, k = 0; o < nEl; o += outerStride, ++k) {
        SizeT jump = k*sumStride;
        for (SizeT i = 0, j = 0; i < outerStride; ++i, ++j) {
          if (j >= sumStride) j = 0;
          AddOmitNaN((*res)[ j + jump], (*src)[ i + o]);
        }
      }
    } else {
      for (SizeT o = 0, k = 0; o < nEl; o += outerStride, ++k) {
        SizeT jump = k*sumStride;  //there is an obvious optimisation for sumDimIx=0 where j=0 always. Strangely, it seems ineffective and has been dropped.
        for (SizeT i = 0, j = 0; i < outerStride; ++i, ++j) {
          if (j >= sumStride) j = 0; //this is so much better than slow integer divides or %
          (*res)[j + jump] += (*src)[ i + o];
        }
      }
    }
	} else { //favor integer division
	  SizeT sumLimit = nSum * sumStride;
	  if (omitNaN) {
		  for (SizeT o = 0; o < nEl; o += outerStride) { //nEl / outerStride divisions=20* nEl / outerStride tests + sumStride ops
			SizeT rIx = (o / outerStride) * sumStride; 
			for (SizeT i = 0; i < sumStride; ++i) {
			  SizeT oi = o + i;
			  SizeT oiLimit = sumLimit + oi;
			  for (SizeT s = oi; s < oiLimit; s += sumStride) AddOmitNaN((*res)[ rIx], (*src)[ s]);
			  ++rIx;
		  }
		}
	  } else {
		  for (SizeT o = 0; o < nEl; o += outerStride) {
			SizeT rIx = (o / outerStride) * sumStride;
			for (SizeT i = 0; i < sumStride; ++i) {
			  SizeT oi = o + i;
			  SizeT oiLimit = sumLimit + oi;
			  for (SizeT s = oi; s < oiLimit; s += sumStride) (*res)[ rIx] += (*src)[ s];
			  ++rIx;
		  }
		}
	  }
	}
    return res;
  }

  // cumulative over one dim

  template< typename T1, typename T2>
  BaseGDL* total_over_dim_cu_template(T1* val,
    SizeT sumDimIx,
    bool omitNaN) {
    SizeT nEl = val->N_Elements();
    typename T1::Ty *res;
    res = static_cast<T2*> (val->DataAddr());
    const dimension& valDim = val->Dim();
    if (omitNaN) {
      for (SizeT i = 0; i < nEl; ++i)
        NaN2Zero(res[i]);
    }
    SizeT cumStride = valDim.Stride(sumDimIx);
    SizeT outerStride = valDim.Stride(sumDimIx + 1);
    for (SizeT o = 0; o < nEl; o += outerStride) {
      SizeT cumLimit = o + outerStride;
      for (SizeT i = o + cumStride, ii = o; i < cumLimit; ++i, ++ii)
        res[ i] += res[ ii];
    }
    return val;
  }

  BaseGDL* total_fun(EnvT* e) {

	// TOTAL is watched for overflows
	std::unique_ptr<ReportFPExceptionsGuard> p=GDLStartAutoStopRegisteringFPExceptions(); //stops monitoring when exiting from here
	
    // Integer parts initially by Erin Sheldon

    SizeT nParam = e->NParam(1); //, "TOTAL");

    BaseGDL* p0 = e->GetParDefined(0); //, "TOTAL");

    SizeT nEl = p0->N_Elements();
    if (nEl == 0)
      e->Throw("Variable is undefined: " + e->GetParString(0));

    if (p0->Type() == GDL_STRING)
      e->Throw("String expression not allowed "
      "in this context: " + e->GetParString(0));

    static int cumIx = e->KeywordIx("CUMULATIVE");
    static int intIx = e->KeywordIx("INTEGER");
    static int doubleIx = e->KeywordIx("DOUBLE");
    static int nanIx = e->KeywordIx("NAN");
    static int preserveIx = e->KeywordIx("PRESERVE_TYPE");

    bool cumulative = e->KeywordSet(cumIx);
    bool useIntegerArithmetic = e->KeywordSet(intIx);
    // if double is set to ZERO, then DOUBLE things are converted IN THE END to SINGLE
    // if double is set to 1, SINGLE things are converted to double before summing, then total is reconverted to single:
    // DL> a=FINDGEN(10LL^7) & z=total(a) & help,z
    // Z               FLOAT     =   4.98246e+13
    // DL> a=FINDGEN(10LL^7) & z=total(a,/doub) & help,z
    // Z               DOUBLE    =    4.9999995e+13
    // DL> a=DINDGEN(10LL^7) & z=total(a,doub=0) & help,z
    // Z               FLOAT     =   5.00000e+13

    // PRESERVE takes precedence.
    // Next, INTEGER arithmetic takes precedence, and floats are individually converted to integers:
    // 1) ULONG64 are treated "as is" and result is like "preserve"
    // 2) Others are converted to LONG64 and result is LONG64. Complex values uses only real part.
    // /double converts individually values to doubles before summing and double=0 converts THE RESULT ONLY to single.
    // CUMULATIVE: If the input array is a temporary variable or an expression, and the result type matches the input type
    // the cumulative sum should be made in place to save memory.

    bool doublePrecision = false;
    bool downgradeDoubleResult = false;

    if (e->KeywordPresent(doubleIx)) {
      doublePrecision = e->KeywordSet(doubleIx);
      downgradeDoubleResult = !doublePrecision;
    }

    bool nan = e->KeywordSet(nanIx);
    bool preserve = e->KeywordSet(preserveIx);

    DLong sumDim = 0;
    if (nParam == 2)
      e->AssureLongScalarPar(1, sumDim);

    if (sumDim == 0) {
      if (preserve) {
        if (!cumulative) {
          switch (p0->Type()) {
          case GDL_BYTE: return total_template_generic<DByteGDL>(static_cast<DByteGDL*> (p0), false);
          case GDL_INT: return total_template_generic<DIntGDL>(static_cast<DIntGDL*> (p0), false);
          case GDL_UINT: return total_template_generic<DUIntGDL>(static_cast<DUIntGDL*> (p0), false);
          case GDL_LONG: return total_template_generic<DLongGDL>(static_cast<DLongGDL*> (p0), false);
          case GDL_ULONG: return total_template_generic<DULongGDL>(static_cast<DULongGDL*> (p0), false);
          case GDL_LONG64: return total_template_generic<DLong64GDL>(static_cast<DLong64GDL*> (p0), false);
          case GDL_ULONG64: return total_template_generic<DULong64GDL>(static_cast<DULong64GDL*> (p0), false);
          case GDL_FLOAT: return total_template_generic<DFloatGDL>(static_cast<DFloatGDL*> (p0), nan);
          case GDL_DOUBLE: return total_template_generic<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), nan);
          case GDL_COMPLEX: return total_template_generic<DComplexGDL>(static_cast<DComplexGDL*> (p0), nan);
          case GDL_COMPLEXDBL: return total_template_generic<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), nan);
          default: assert(false);
          }
        } else {
          switch (p0->Type()) {
          case GDL_BYTE: return total_cu_template<DByteGDL, DByte>(static_cast<DByteGDL*> (p0->Dup()), false);
          case GDL_INT: return total_cu_template<DIntGDL, DInt>(static_cast<DIntGDL*> (p0->Dup()), false);
          case GDL_UINT: return total_cu_template<DUIntGDL, DUInt>(static_cast<DUIntGDL*> (p0->Dup()), false);
          case GDL_LONG: return total_cu_template<DLongGDL, DLong>(static_cast<DLongGDL*> (p0->Dup()), false);
          case GDL_ULONG: return total_cu_template<DULongGDL, DULong>(static_cast<DULongGDL*> (p0->Dup()), false);
          case GDL_LONG64: return total_cu_template<DLong64GDL, DLong64>(static_cast<DLong64GDL*> (p0->Dup()), false);
          case GDL_ULONG64: return total_cu_template<DULong64GDL, DULong64>(static_cast<DULong64GDL*> (p0->Dup()), false);
          case GDL_FLOAT: return total_cu_template<DFloatGDL, DFloat>(static_cast<DFloatGDL*> (p0->Dup()), nan);
          case GDL_DOUBLE: return total_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), nan);
          case GDL_COMPLEX: return total_cu_template<DComplexGDL, DComplex>(static_cast<DComplexGDL*> (p0->Dup()), nan);
          case GDL_COMPLEXDBL: return total_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), nan);
          default: assert(false);
          }
        }
      }

      // Next, Integer Aritmetic
      if (useIntegerArithmetic) {
        if (!cumulative) {
          switch (p0->Type()) {
            // We use GDL_LONG64 unless the input is GDL_ULONG64
          case GDL_ULONG64: return total_template_generic<DULong64GDL>(static_cast<DULong64GDL*> (p0), false);
          case GDL_LONG64: return total_template_generic<DLong64GDL>(static_cast<DLong64GDL*> (p0), false);
          case GDL_BYTE: return total_template_integer<DByteGDL>(static_cast<DByteGDL*> (p0));
          case GDL_INT: return total_template_integer<DIntGDL>(static_cast<DIntGDL*> (p0));
          case GDL_UINT: return total_template_integer<DUIntGDL>(static_cast<DUIntGDL*> (p0));
          case GDL_LONG: return total_template_integer<DLongGDL>(static_cast<DLongGDL*> (p0));
          case GDL_ULONG: return total_template_integer<DULongGDL>(static_cast<DULongGDL*> (p0));
          case GDL_FLOAT: return total_template_integer<DFloatGDL>(static_cast<DFloatGDL*> (p0));
          case GDL_DOUBLE: return total_template_integer<DDoubleGDL>(static_cast<DDoubleGDL*> (p0));
          case GDL_COMPLEX:
          case GDL_COMPLEXDBL:
            // Convert to Long64 OR write a variant of total_template_integer...
          {
            DLong64GDL* p0L64 = static_cast<DLong64GDL*> (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
            Guard<DLong64GDL> guard(p0L64);
            return total_template_generic<DLong64GDL>(p0L64, false);
          }
          default: assert(false);
          }

        } else {
          switch (p0->Type()) {
          case GDL_ULONG64: return total_cu_template<DULong64GDL, DULong64>(static_cast<DULong64GDL*> (p0->Dup()), false);
          case GDL_LONG64: return total_cu_template<DLong64GDL, DLong64>(static_cast<DLong64GDL*> (p0->Dup()), false);
          case GDL_BYTE:
          case GDL_INT:
          case GDL_UINT:
          case GDL_LONG:
          case GDL_ULONG:
          case GDL_FLOAT:
          case GDL_DOUBLE:
          case GDL_COMPLEX:
          case GDL_COMPLEXDBL:
          {
            DLong64GDL* p0L64 = static_cast<DLong64GDL*> (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
            return total_cu_template<DLong64GDL, DLong64>(p0L64, false);
          }
          default: assert(false);
          }
        }
      } // integer result

      // Next, upgrade single values, and downgrade result if needed
      if (doublePrecision) {
        if (!cumulative) {
          switch (p0->Type()) {
          case GDL_DOUBLE: return total_template_generic<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), nan);
          case GDL_COMPLEXDBL: return total_template_generic<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), nan);
            // We use GDL_DOUBLE for others
          case GDL_ULONG64:
            return total_template_double<DULong64GDL>(static_cast<DULong64GDL*> (p0), false);
          case GDL_LONG64:
            return total_template_double<DLong64GDL>(static_cast<DLong64GDL*> (p0), false);
          case GDL_BYTE:
            return total_template_double<DByteGDL>(static_cast<DByteGDL*> (p0), false);
          case GDL_INT:
            return total_template_double<DIntGDL>(static_cast<DIntGDL*> (p0), false);
          case GDL_UINT:
            return total_template_double<DUIntGDL>(static_cast<DUIntGDL*> (p0), false);
          case GDL_LONG:
            return total_template_double<DLongGDL>(static_cast<DLongGDL*> (p0), false);
          case GDL_ULONG:
            return total_template_double<DULongGDL>(static_cast<DULongGDL*> (p0), false);
          case GDL_FLOAT:
            return total_template_double<DFloatGDL>(static_cast<DFloatGDL*> (p0), nan);
          case GDL_COMPLEX:
          {
            DComplexDblGDL* p0Double = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
            Guard<DComplexDblGDL> guard(p0Double);
            return total_template_generic<DComplexDblGDL>(p0Double, nan);
          }
          default: assert(false);
          }
        } else {
          switch (p0->Type()) {
          case GDL_DOUBLE: return total_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), nan);
          case GDL_COMPLEXDBL: return total_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), nan);
            // We use GDL_DOUBLE for others
          case GDL_FLOAT:
            // Conver to Double
          {
            DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
            return total_cu_template<DDoubleGDL, DDouble>(p0Double, nan);
          }
          case GDL_ULONG64:
          case GDL_LONG64:
          case GDL_BYTE:
          case GDL_INT:
          case GDL_UINT:
          case GDL_LONG:
          case GDL_ULONG:
            // Conver to Double
          {
            DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
            return total_cu_template<DDoubleGDL, DDouble>(p0Double, false);
          }
          case GDL_COMPLEX:
          {
            DComplexDblGDL* p0Double = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
            return total_cu_template<DComplexDblGDL, DComplexDbl>(p0Double, nan);
          }
          default: assert(false);
          }
        }
      } else { //does not promote, but eventually downgrade double results if downgradeDoubleResult is true
        // note integer conversion is in doubles, not floats (verified).
        if (!cumulative) {
          switch (p0->Type()) {
          case GDL_FLOAT: return total_template_generic<DFloatGDL>(static_cast<DFloatGDL*> (p0), nan);
          case GDL_DOUBLE:
          {
            if (downgradeDoubleResult) {
              return (total_template_generic<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), nan))->Convert2(GDL_FLOAT, BaseGDL::COPY);
            } else return total_template_generic<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), nan);
          }
          case GDL_COMPLEX: return total_template_generic<DComplexGDL>(static_cast<DComplexGDL*> (p0), nan);
          case GDL_COMPLEXDBL: if (downgradeDoubleResult) {
              return (total_template_generic<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), nan))->Convert2(GDL_COMPLEX, BaseGDL::COPY);
            } else return total_template_generic<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), nan);
            // convert to double, total then return double or float...
          case GDL_ULONG64:
          {
            DDoubleGDL* res = total_template_double<DULong64GDL>(static_cast<DULong64GDL*> (p0), false);
            if (downgradeDoubleResult) {
              return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
            } else return res;
          }
          case GDL_LONG64:
          {
            DDoubleGDL* res = total_template_double<DLong64GDL>(static_cast<DLong64GDL*> (p0), false);
            if (downgradeDoubleResult) {
              return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
            } else return res;
          }
            // We use GDL_FLOAT for others
          case GDL_BYTE:
            return total_template_single<DByteGDL>(static_cast<DByteGDL*> (p0), false);
          case GDL_INT:
            return total_template_single<DIntGDL>(static_cast<DIntGDL*> (p0), false);
          case GDL_UINT:
            return total_template_single<DUIntGDL>(static_cast<DUIntGDL*> (p0), false);
          case GDL_LONG:
            return total_template_single<DLongGDL>(static_cast<DLongGDL*> (p0), false);
          case GDL_ULONG:
            return total_template_single<DULongGDL>(static_cast<DULongGDL*> (p0), false);
          default: assert(false);
          }
        } else {
          switch (p0->Type()) {
          case GDL_FLOAT: return total_cu_template<DFloatGDL, DFloat>(static_cast<DFloatGDL*> (p0->Dup()), nan);
          case GDL_DOUBLE:
          {
            if (downgradeDoubleResult) {
              DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (total_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), nan));
              Guard<DDoubleGDL> guard(p0Double);
              return (p0Double)->Convert2(GDL_FLOAT, BaseGDL::COPY);
            } else
              return total_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), nan);
          }
          case GDL_COMPLEX: return total_cu_template<DComplexGDL, DComplex>(static_cast<DComplexGDL*> (p0->Dup()), nan);
          case GDL_COMPLEXDBL: if (downgradeDoubleResult) {
              DComplexGDL* p0Cpx = static_cast<DComplexGDL*> (total_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), nan));
              Guard<DComplexGDL> guard(p0Cpx);
              return (p0Cpx)->Convert2(GDL_COMPLEX, BaseGDL::COPY);
            } else
              return total_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), nan);
            // convert to double, total then return double or float...
          case GDL_ULONG64:
          case GDL_LONG64:
          {
            DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
            if (downgradeDoubleResult) {
              DDoubleGDL* tmp = static_cast<DDoubleGDL*> (total_cu_template<DDoubleGDL, DDouble>(p0Double, false));
              Guard<DDoubleGDL> guard(tmp);
              return (tmp)->Convert2(GDL_FLOAT, BaseGDL::COPY);
            } else
              return total_cu_template<DDoubleGDL, DDouble>(p0Double, false);
          }
            // We use GDL_FLOAT for others
          case GDL_BYTE:
          case GDL_INT:
          case GDL_UINT:
          case GDL_LONG:
          case GDL_ULONG:
          {
            DFloatGDL* p0Single = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
            return total_cu_template<DFloatGDL, DFloat>(p0Single, false);
          }
          default: assert(false);
          }
        }
      }
      assert(false);
    }

    // SUM Over a dimension.

    // total over sumDim
    dimension srcDim = p0->Dim();
    SizeT srcRank = srcDim.Rank();

    if (sumDim < 1 || sumDim > srcRank)
      e->Throw(
      "Array must have " + i2s(sumDim) +
      " dimensions: " + e->GetParString(0));

    // Preserve , fast , has preference .
    if (preserve) {
      if (!cumulative) {
        switch (p0->Type()) {
        case GDL_BYTE: return total_over_dim_template<DByteGDL>(static_cast<DByteGDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_INT: return total_over_dim_template<DIntGDL>(static_cast<DIntGDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_UINT: return total_over_dim_template<DUIntGDL>(static_cast<DUIntGDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_LONG: return total_over_dim_template<DLongGDL>(static_cast<DLongGDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_ULONG: return total_over_dim_template<DULongGDL>(static_cast<DULongGDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_LONG64: return total_over_dim_template<DLong64GDL>(static_cast<DLong64GDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_ULONG64: return total_over_dim_template<DULong64GDL>(static_cast<DULong64GDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_FLOAT: return total_over_dim_template<DFloatGDL>(static_cast<DFloatGDL*> (p0), srcDim, sumDim - 1, nan);
        case GDL_DOUBLE: return total_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), srcDim, sumDim - 1, nan);
        case GDL_COMPLEX: return total_over_dim_template<DComplexGDL>(static_cast<DComplexGDL*> (p0), srcDim, sumDim - 1, nan);
        case GDL_COMPLEXDBL: return total_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), srcDim, sumDim - 1, nan);
        default: assert(false);
        }
      } else {
        switch (p0->Type()) {
        case GDL_BYTE: return total_over_dim_cu_template<DByteGDL, DByte>(static_cast<DByteGDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_INT: return total_over_dim_cu_template<DIntGDL, DInt>(static_cast<DIntGDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_UINT: return total_over_dim_cu_template<DUIntGDL, DUInt>(static_cast<DUIntGDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_LONG: return total_over_dim_cu_template<DLongGDL, DLong>(static_cast<DLongGDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_ULONG: return total_over_dim_cu_template<DULongGDL, DULong>(static_cast<DULongGDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_LONG64: return total_over_dim_cu_template<DLong64GDL, DLong64>(static_cast<DLong64GDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_ULONG64: return total_over_dim_cu_template<DULong64GDL, DULong64>(static_cast<DULong64GDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_FLOAT: return total_over_dim_cu_template<DFloatGDL, DFloat>(static_cast<DFloatGDL*> (p0->Dup()), sumDim - 1, nan);
        case GDL_DOUBLE: return total_over_dim_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), sumDim - 1, nan);
        case GDL_COMPLEX: return total_over_dim_cu_template<DComplexGDL, DComplex>(static_cast<DComplexGDL*> (p0->Dup()), sumDim - 1, nan);
        case GDL_COMPLEXDBL: return total_over_dim_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), sumDim - 1, nan);
        default: assert(false);
        }
      }
    }
    // Next, Integer Aritmetic
    if (useIntegerArithmetic) {
      if (!cumulative) {
        switch (p0->Type()) {
        case GDL_ULONG64: return total_over_dim_template<DULong64GDL>(static_cast<DULong64GDL*> (p0), srcDim, sumDim - 1, false);
        case GDL_LONG64: return total_over_dim_template<DLong64GDL>(static_cast<DLong64GDL*> (p0), srcDim, sumDim - 1, false);
          // We use GDL_LONG64 unless the input is GDL_ULONG64
        case GDL_BYTE:
        case GDL_INT:
        case GDL_UINT:
        case GDL_LONG:
        case GDL_ULONG:
        case GDL_FLOAT:
        case GDL_DOUBLE:
        case GDL_COMPLEX:
        case GDL_COMPLEXDBL:
          // Conver to Long64
        {
          DLong64GDL* p0L64 = static_cast<DLong64GDL*> (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
          Guard<DLong64GDL> guard(p0L64);
          return total_over_dim_template<DLong64GDL>(p0L64, srcDim, sumDim - 1, false);
        }
        default: assert(false);
        }

      } else {
        switch (p0->Type()) {
        case GDL_ULONG64: return total_over_dim_cu_template<DULong64GDL,DULong64>(static_cast<DULong64GDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_LONG64: return total_over_dim_cu_template<DLong64GDL,DLong64>(static_cast<DLong64GDL*> (p0->Dup()), sumDim - 1, false);
        case GDL_BYTE:
        case GDL_INT:
        case GDL_UINT:
        case GDL_LONG:
        case GDL_ULONG:
        case GDL_FLOAT:
        case GDL_DOUBLE:
        case GDL_COMPLEX:
        case GDL_COMPLEXDBL:
        {
          DLong64GDL* p0L64 = static_cast<DLong64GDL*> (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
          return total_over_dim_cu_template<DLong64GDL, DLong64>(p0L64, sumDim - 1, false);
        }
        default: assert(false);
        }
      }
    } // integer result

    // Next, upgrade single values, and downgrade result if needed
    if (doublePrecision) {
        if (!cumulative) {
        switch (p0->Type()) {
        case GDL_DOUBLE: return total_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), srcDim, sumDim - 1, nan);
        case GDL_COMPLEXDBL: return total_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), srcDim, sumDim - 1, nan);
          // We use GDL_DOUBLE for others
        case GDL_FLOAT:
          // Conver to Double
        {
          DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          Guard<DDoubleGDL> guard(p0Double);
          return total_over_dim_template<DDoubleGDL>(p0Double, srcDim, sumDim - 1, nan);
        }
        case GDL_ULONG64:
        case GDL_LONG64:
        case GDL_BYTE:
        case GDL_INT:
        case GDL_UINT:
        case GDL_LONG:
        case GDL_ULONG:
          // Conver to Double
        {
          DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          Guard<DDoubleGDL> guard(p0Double);
          return total_over_dim_template<DDoubleGDL>(p0Double, srcDim, sumDim - 1, false);
        }
        case GDL_COMPLEX:
        {
          DComplexDblGDL* p0Double = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
          Guard<DComplexDblGDL> guard(p0Double);
          return total_over_dim_template<DComplexDblGDL>(p0Double, srcDim, sumDim - 1, nan);
        }
        default: assert(false);
        }
      } else {
        switch (p0->Type()) {
        case GDL_DOUBLE: return total_over_dim_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), sumDim - 1, nan);
        case GDL_COMPLEXDBL: return total_over_dim_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), sumDim - 1, nan);
          // We use GDL_DOUBLE for others
        case GDL_FLOAT:
          // Conver to Double
        {
          DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          return total_over_dim_cu_template<DDoubleGDL, DDouble>(p0Double, sumDim - 1, nan);
        }
        case GDL_ULONG64:
        case GDL_LONG64:
        case GDL_BYTE:
        case GDL_INT:
        case GDL_UINT:
        case GDL_LONG:
        case GDL_ULONG:
          // Conver to Double
        {
          DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          return total_over_dim_cu_template<DDoubleGDL, DDouble>(p0Double, sumDim - 1, false);
        }
        case GDL_COMPLEX:
        {
          DComplexDblGDL* p0Cpx = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
          return total_over_dim_cu_template<DComplexDblGDL, DComplexDbl>(p0Cpx, sumDim - 1, nan);
        }
        default: assert(false);
        }
      }
    }// promote to double
    else { //does not promote, but eventually downgrade double results if downgradeDoubleResult is true
      if (!cumulative) {
        switch (p0->Type()) {
        case GDL_FLOAT: return total_over_dim_template<DFloatGDL>(static_cast<DFloatGDL*> (p0), srcDim, sumDim - 1, nan);
        case GDL_DOUBLE:
        {
          if (downgradeDoubleResult) {
            return (total_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), srcDim, sumDim - 1, nan))->Convert2(GDL_FLOAT, BaseGDL::COPY);
          } else return total_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), srcDim, sumDim - 1, nan);
        }
        case GDL_COMPLEX: return total_over_dim_template<DComplexGDL>(static_cast<DComplexGDL*> (p0), srcDim, sumDim - 1, nan);
        case GDL_COMPLEXDBL: if (downgradeDoubleResult) {
            return (total_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), srcDim, sumDim - 1, nan))->Convert2(GDL_COMPLEX, BaseGDL::COPY);
          } else return total_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), srcDim, sumDim - 1, nan);
          // convert to double, total_over_dim then return double or float...
        case GDL_ULONG64:
        case GDL_LONG64:
        {
          DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          Guard<DDoubleGDL> guard(p0Double);
          if (downgradeDoubleResult) {
            return (total_over_dim_template<DDoubleGDL>(p0Double, srcDim, sumDim - 1, false))->Convert2(GDL_FLOAT, BaseGDL::COPY);
          } else return total_over_dim_template<DDoubleGDL>(p0Double, srcDim, sumDim - 1, false);
        }
          // We use GDL_FLOAT for others
        case GDL_BYTE:
        case GDL_INT:
        case GDL_UINT:
        case GDL_LONG:
        case GDL_ULONG:
        {
          DFloatGDL* p0Single = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
          Guard<DFloatGDL> guard(p0Single);
          return total_over_dim_template<DFloatGDL>(p0Single, srcDim, sumDim - 1, false);
        }
        default: assert(false);
        }
      } else {
        switch (p0->Type()) {
        case GDL_FLOAT: return total_over_dim_cu_template<DFloatGDL, DFloat>(static_cast<DFloatGDL*> (p0->Dup()), sumDim - 1, nan);
        case GDL_DOUBLE:
        {
          if (downgradeDoubleResult) {
            DDoubleGDL* tmp = static_cast<DDoubleGDL*> (total_over_dim_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), sumDim - 1, nan));
            Guard<DDoubleGDL> guard(tmp);
            return tmp->Convert2(GDL_FLOAT, BaseGDL::COPY);
          } else return total_over_dim_cu_template<DDoubleGDL, DDouble>(static_cast<DDoubleGDL*> (p0->Dup()), sumDim - 1, nan);
        }
        case GDL_COMPLEX: return total_over_dim_cu_template<DComplexGDL, DComplex>(static_cast<DComplexGDL*> (p0->Dup()), sumDim - 1, nan);
        case GDL_COMPLEXDBL: if (downgradeDoubleResult) {
            DComplexDblGDL* tmp = static_cast<DComplexDblGDL*> (total_over_dim_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), sumDim - 1, nan));
            Guard<DComplexDblGDL> guard(tmp);
            return tmp->Convert2(GDL_COMPLEX, BaseGDL::COPY);
          } else return total_over_dim_cu_template<DComplexDblGDL, DComplexDbl>(static_cast<DComplexDblGDL*> (p0->Dup()), sumDim - 1, nan);
          // convert to double, total_over_dim then return double or float...
        case GDL_ULONG64:
        case GDL_LONG64:
        {
          DDoubleGDL* p0Double = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          if (downgradeDoubleResult) {
            Guard<DDoubleGDL> guard(p0Double);
            return (total_over_dim_cu_template<DDoubleGDL, DDouble>(p0Double, sumDim - 1, false))->Convert2(GDL_FLOAT, BaseGDL::COPY);
          } else return total_over_dim_cu_template<DDoubleGDL, DDouble>(p0Double, sumDim - 1, false);
        }
          // We use GDL_FLOAT for others
        case GDL_BYTE:
        case GDL_INT:
        case GDL_UINT:
        case GDL_LONG:
        case GDL_ULONG:
        {
          DFloatGDL* p0Single = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
          return total_over_dim_cu_template<DFloatGDL, DFloat>(p0Single, sumDim - 1, false);
        }
        default: assert(false);
        }
      }
    }
    assert(false);
    return NULL;
  }


  // passing 2nd argument by value is slightly better for float and double,
  // but incur some overhead for the complex class.

  template<class T> inline void MultOmitNaN(T& dest, T value) {
    if (std::isfinite(value)) {
      dest *= value;
    }
  }

  template<class T> inline void MultOmitNaNCpx(T& dest, T value) {
    dest *= T(std::isfinite(value.real()) ? value.real() : 1,  std::isfinite(value.imag()) ? value.imag() : 1);
  }

  template<> inline void MultOmitNaN(DComplex& dest, DComplex value) {
    MultOmitNaNCpx<DComplex>(dest, value);
  }

  template<> inline void MultOmitNaN(DComplexDbl& dest, DComplexDbl value) {
    MultOmitNaNCpx<DComplexDbl>(dest, value);
  }

  template<class T> inline void Nan2One(T& value) {
    if (!std::isfinite(value)) value = 1;
  }

  template<class T> inline void Nan2OneCpx(T& value) {
    value = T(std::isfinite(value.real()) ? value.real() : 1,  std::isfinite(value.imag()) ? value.imag() : 1);
  }

  template<> inline void Nan2One(DComplex& value) {
    Nan2OneCpx< DComplex>(value);
  }

  template<> inline void Nan2One(DComplexDbl& value) {
    Nan2OneCpx< DComplexDbl>(value);
  }

  // product over all elements

  template<class T>
  BaseGDL* product_template(T* src, bool omitNaN) {
    typename T::Ty prod = 1;
    SizeT nEl = src->N_Elements();

    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      if (!omitNaN) for (OMPInt i = 0; i < nEl; ++i) prod *= (*src)[ i];
      else for (OMPInt i = 0; i < nEl; ++i) if (std::isfinite((*src)[ i])) prod *= (*src)[ i];
    } else {
      if (!omitNaN) {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(*:prod)
          for (OMPInt i = 0; i < nEl; ++i) prod *= (*src)[ i];
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(*:prod)
          for (OMPInt i = 0; i < nEl; ++i) if (std::isfinite((*src)[ i])) prod *= (*src)[ i];
      }
    }
    return new T(prod);
  }

  template<>
  BaseGDL* product_template(DComplexGDL* src, bool omitNaN) {
    SizeT nEl = src->N_Elements();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      DComplexGDL::Ty prod = 1;
      if (!omitNaN) {
        for (SizeT i = 0; i < nEl; ++i) prod *= (*src)[ i];
      } else {
        for (SizeT i = 0; i < nEl; ++i) MultOmitNaN(prod, (*src)[ i]);
      }
      return new DComplexGDL(prod);
    } else {
        DFloat prodr = 1;
        DFloat prodi = 1;
        if (!omitNaN) {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(*:prodr,prodi)
          for (OMPInt i = 0; i < nEl; ++i) {
            prodr *= (*src)[i].real();
            prodi *= (*src)[i].imag();
          }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(*:prodr,prodi)
          for (OMPInt i = 0; i < nEl; ++i) {
            prodr *= (std::isfinite((*src)[i].real())) ? (*src)[i].real() : 1;
            prodi *= (std::isfinite((*src)[i].imag())) ? (*src)[i].imag() : 1;
        }
    }
      return new DComplexGDL(std::complex<DFloat>(prodr,prodi));
    }
  }

  template<>
  BaseGDL* product_template(DComplexDblGDL* src, bool omitNaN) {
    SizeT nEl = src->N_Elements();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      DComplexDblGDL::Ty prod = 1;
      if (!omitNaN) {
        for (SizeT i = 0; i < nEl; ++i) prod *= (*src)[ i];
      } else {
        for (SizeT i = 0; i < nEl; ++i) MultOmitNaN(prod, (*src)[ i]);
      }
      return new DComplexDblGDL(prod);
    } else {
      DDouble prodr = 1;
      DDouble prodi = 1;
      if (!omitNaN) {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(*:prodr,prodi)
          for (OMPInt i = 0; i < nEl; ++i) {
          prodr *= (*src)[i].real();
          prodi *= (*src)[i].imag();
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(*:prodr,prodi)
          for (OMPInt i = 0; i < nEl; ++i) {
          prodr *= (std::isfinite((*src)[i].real())) ? (*src)[i].real() : 1;
          prodi *= (std::isfinite((*src)[i].imag())) ? (*src)[i].imag() : 1;
        }
      }
      return new DComplexDblGDL(std::complex<DDouble>(prodr, prodi));
    }
  }
  
  template<typename T>
  BaseGDL* product_cu_template(T* res, bool omitNaN) {
    SizeT nEl = res->N_Elements();
    if (omitNaN) {
      for (SizeT i = 0; i < nEl; ++i)
        Nan2One((*res)[i]);
    }
    for (SizeT i = 1, ii = 0; i < nEl; ++i, ++ii)
      (*res)[i] *= (*res)[ii];
    return res;
  }

  // product over one dim

  template< typename T>
  BaseGDL* product_over_dim_template(T* src,
    const dimension& srcDim,
    SizeT prodDimIx,
    bool omitNaN) {
    SizeT nEl = src->N_Elements();

    // get dest dim and number of products
    dimension destDim = srcDim;
    SizeT nProd = destDim.Remove(prodDimIx);

    T* res = new T(destDim, BaseGDL::INDGEN,1,0);

    // prodStride is also the number of linear src indexing
    SizeT prodStride = srcDim.Stride(prodDimIx);
    SizeT outerStride = srcDim.Stride(prodDimIx + 1);
	//compare prodStride and nEl/outerStride. Use best suited method, one uses expensive integer division times nEl/outerStride the other a test x prodStride.
	// estimated ratio is ~20 times faster for the test
	if (2 * outerStride > 20 * nEl / outerStride + prodStride) { // outerStride tests +outerStride ops
      if (omitNaN) {
        for (SizeT o = 0, k = 0; o < nEl; o += outerStride, ++k) {
          SizeT jump = k*prodStride; //there is an obvious optimisation for sumDimIx=0 where j=0 always. Strangely, it seems ineffective and has been dropped.
          for (SizeT i = 0, j = 0; i < outerStride; ++i, ++j) {
            if (j >= prodStride) j = 0; //this is so much better than slow integer divides or %
            MultOmitNaN((*res)[j + jump] , (*src)[ i + o]);
          }
        }
      } else {
        for (SizeT o = 0, k = 0; o < nEl; o += outerStride, ++k) {
          SizeT jump = k*prodStride; //there is an obvious optimisation for sumDimIx=0 where j=0 always. Strangely, it seems ineffective and has been dropped.
          for (SizeT i = 0, j = 0; i < outerStride; ++i, ++j) {
            if (j >= prodStride) j = 0; //this is so much better than slow integer divides or %
            (*res)[j + jump] *= (*src)[ i + o];
          }
        }
      }
  } else {
	SizeT prodLimit = nProd * prodStride;
	if (omitNaN) {
	  for (SizeT o = 0; o < nEl; o += outerStride) {
		SizeT rIx = (o / outerStride) * prodStride;
		for (SizeT i = 0; i < prodStride; ++i) {
		  SizeT oi = o + i;
		  SizeT oiLimit = prodLimit + oi;
		  for (SizeT s = oi; s < oiLimit; s += prodStride) MultOmitNaN((*res)[ rIx], (*src)[ s]);
		  ++rIx;
		}
	  }
	} else {
	  for (SizeT o = 0; o < nEl; o += outerStride) {
		SizeT rIx = (o / outerStride) * prodStride;
		for (SizeT i = 0; i < prodStride; ++i) {
		  SizeT oi = o + i;
		  SizeT oiLimit = prodLimit + oi;
		  for (SizeT s = oi; s < oiLimit; s += prodStride) (*res)[ rIx] *= (*src)[ s];
		  ++rIx;
		}
          }
        }
      }
    return res;
  }

  // cumulative over one dim

  template< typename T>
  BaseGDL* product_over_dim_cu_template(T* res,
    SizeT sumDimIx,
    bool omitNaN) {
    SizeT nEl = res->N_Elements();
    const dimension& resDim = res->Dim();
    if (omitNaN) {
      for (SizeT i = 0; i < nEl; ++i)
        Nan2One((*res)[i]);
    }
    SizeT cumStride = resDim.Stride(sumDimIx);
    SizeT outerStride = resDim.Stride(sumDimIx + 1);
    for (SizeT o = 0; o < nEl; o += outerStride) {
      SizeT cumLimit = o + outerStride;
      for (SizeT i = o + cumStride, ii = o; i < cumLimit; ++i, ++ii)
        (*res)[ i] *= (*res)[ ii];
    }
    return res;
  }

  BaseGDL* product_fun(EnvT* e) {
	// PRODUCT is watched for overflows
	std::unique_ptr<ReportFPExceptionsGuard> p=GDLStartAutoStopRegisteringFPExceptions(); //stops monitoring when exiting from here
	
    SizeT nParam = e->NParam(1);

    BaseGDL* p0 = e->GetParDefined(0);

    SizeT nEl = p0->N_Elements();
    if (nEl == 0)
      e->Throw("Variable is undefined: " + e->GetParString(0));

    if (p0->Type() == GDL_STRING)
      e->Throw("String expression not allowed "
      "in this context: " + e->GetParString(0));

    static int cumIx = e->KeywordIx("CUMULATIVE");
    static int nanIx = e->KeywordIx("NAN");
    static int intIx = e->KeywordIx("INTEGER");
    static int preIx = e->KeywordIx("PRESERVE_TYPE");
    bool KwCumul = e->KeywordSet(cumIx);
    bool KwNaN = e->KeywordSet(nanIx);
    bool KwInt = e->KeywordSet(intIx);
    bool KwPre = e->KeywordSet(preIx);
    bool nanInt = false;

    DLong sumDim = 0;
    if (nParam == 2)
      e->AssureLongScalarPar(1, sumDim);

    if (sumDim == 0) {
      if (!KwCumul) {
        if (KwPre) {
          switch (p0->Type()) {
          case GDL_BYTE: return product_template<DByteGDL>(static_cast<DByteGDL*> (p0), nanInt);
          case GDL_INT: return product_template<DIntGDL>(static_cast<DIntGDL*> (p0), nanInt);
          case GDL_UINT: return product_template<DUIntGDL>(static_cast<DUIntGDL*> (p0), nanInt);
          case GDL_LONG: return product_template<DLongGDL>(static_cast<DLongGDL*> (p0), nanInt);
          case GDL_ULONG: return product_template<DULongGDL>(static_cast<DULongGDL*> (p0), nanInt);
          case GDL_LONG64: return product_template<DLong64GDL>(static_cast<DLong64GDL*> (p0), nanInt);
          case GDL_ULONG64: return product_template<DULong64GDL>(static_cast<DULong64GDL*> (p0), nanInt);
          case GDL_FLOAT: return product_template<DFloatGDL>(static_cast<DFloatGDL*> (p0), KwNaN);
          case GDL_DOUBLE: return product_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), KwNaN);
          case GDL_COMPLEX: return product_template<DComplexGDL>(static_cast<DComplexGDL*> (p0), KwNaN);
          case GDL_COMPLEXDBL: return product_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), KwNaN);
          default: assert(false);
          }
        }

        // Integer parts derivated from Total code by Erin Sheldon
        // In IDL PRODUCT(), the INTEGER keyword takes precedence
        if (KwInt) {
          // We use GDL_LONG64 unless the input is GDL_ULONG64
          if ((p0->Type() == GDL_LONG64) && (!KwNaN)) {
            return product_template<DLong64GDL>
              (static_cast<DLong64GDL*> (p0), nanInt);
          }
          if ((p0->Type() == GDL_ULONG64) && (!KwNaN)) {
            return product_template<DULong64GDL>
              (static_cast<DULong64GDL*> (p0), nanInt);
          }

          // Convert to Long64
          DLong64GDL* p0L64 = static_cast<DLong64GDL*>
            (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
          Guard<DLong64GDL> guard(p0L64);
          if (KwNaN) {
            DFloatGDL* p0f = static_cast<DFloatGDL*>
              (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
            Guard<DFloatGDL> guard(p0f);
            for (SizeT i = 0; i < nEl; ++i) {
              if (!std::isfinite((*p0f)[i])) (*p0L64)[i] = 1;
            }
          }
          return product_template<DLong64GDL>(p0L64, nanInt);
        } // integer results

        if (p0->Type() == GDL_DOUBLE) {
          return product_template<DDoubleGDL>
            (static_cast<DDoubleGDL*> (p0), KwNaN);
        }
        if (p0->Type() == GDL_COMPLEXDBL) {
          return product_template<DComplexDblGDL>
            (static_cast<DComplexDblGDL*> (p0), KwNaN);
        }
        if (p0->Type() == GDL_COMPLEX) {
          DComplexDblGDL* p0D = static_cast<DComplexDblGDL*>
            (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
          Guard<DComplexDblGDL> p0D_guard(p0D);
          //p0D_guard.Reset( p0D);
          return product_template<DComplexDblGDL>(p0D, KwNaN);
        }

        DDoubleGDL* p0D = static_cast<DDoubleGDL*>
          (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        Guard<DDoubleGDL> p0D_guard(p0D);
        //      p0D_guard.Reset( p0D);
        return product_template<DDoubleGDL>(p0D, KwNaN);
      } else { // KwCumul

        if (KwPre) {
          switch (p0->Type()) {
          case GDL_BYTE: return product_cu_template<DByteGDL>(static_cast<DByteGDL*> (p0->Dup()), nanInt);
          case GDL_INT: return product_cu_template<DIntGDL>(static_cast<DIntGDL*> (p0->Dup()), nanInt);
          case GDL_UINT: return product_cu_template<DUIntGDL>(static_cast<DUIntGDL*> (p0->Dup()), nanInt);
          case GDL_LONG: return product_cu_template<DLongGDL>(static_cast<DLongGDL*> (p0->Dup()), nanInt);
          case GDL_ULONG: return product_cu_template<DULongGDL>(static_cast<DULongGDL*> (p0->Dup()), nanInt);
          case GDL_LONG64: return product_cu_template<DLong64GDL>(static_cast<DLong64GDL*> (p0->Dup()), nanInt);
          case GDL_ULONG64: return product_cu_template<DULong64GDL>(static_cast<DULong64GDL*> (p0->Dup()), nanInt);
          case GDL_FLOAT: return product_cu_template<DFloatGDL>(static_cast<DFloatGDL*> (p0->Dup()), KwNaN);
          case GDL_DOUBLE: return product_cu_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0->Dup()), KwNaN);
          case GDL_COMPLEX: return product_cu_template<DComplexGDL>(static_cast<DComplexGDL*> (p0->Dup()), KwNaN);
          case GDL_COMPLEXDBL: return product_cu_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0->Dup()), KwNaN);
          default: assert(false);
          }
        }

        // Integer parts derivated from Total code by Erin Sheldon
        // In IDL PRODUCT(), the INTEGER keyword takes precedence
        if (KwInt) {
          // We use GDL_LONG64 unless the input is GDL_ULONG64
          if ((p0->Type() == GDL_LONG64) && (!KwNaN)) {
            return product_cu_template<DLong64GDL>
              (static_cast<DLong64GDL*> (p0->Dup()), nanInt);
          }
          if ((p0->Type() == GDL_ULONG64) && (!KwNaN)) {
            return product_cu_template<DULong64GDL>
              (static_cast<DULong64GDL*> (p0->Dup()), nanInt);
          }
          // Convert to Long64
          DLong64GDL* p0L64 = static_cast<DLong64GDL*>
            (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
          Guard<DLong64GDL> guard(p0L64);
          if (KwNaN) {
            DFloatGDL* p0f = static_cast<DFloatGDL*>
              (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
            Guard<DFloatGDL> guard(p0f);
            for (SizeT i = 0; i < nEl; ++i) {
              if (!std::isfinite((*p0f)[i])) (*p0L64)[i] = 1;
            }
          }
          return product_cu_template<DLong64GDL>
            (static_cast<DLong64GDL*> (p0L64->Dup()), nanInt);
        } // integer results

        // special case as GDL_DOUBLE type overrides /GDL_DOUBLE
        if (p0->Type() == GDL_DOUBLE) {
          return product_cu_template< DDoubleGDL>
            (static_cast<DDoubleGDL*> (p0->Dup()), KwNaN);
        }
        if (p0->Type() == GDL_COMPLEXDBL) {
          return product_cu_template< DComplexDblGDL>
            (static_cast<DComplexDblGDL*> (p0->Dup()), KwNaN);
        }
        if (p0->Type() == GDL_COMPLEX) {
          return product_cu_template< DComplexDblGDL>
            (static_cast<DComplexDblGDL*>
            (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY)), KwNaN);
        }
        return product_cu_template< DDoubleGDL>
          (static_cast<DDoubleGDL*>
          (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY)), KwNaN);
      }
    }

    // product over sumDim
    dimension srcDim = p0->Dim();
    SizeT srcRank = srcDim.Rank();

    if (sumDim < 1 || sumDim > srcRank)
      e->Throw("Array must have " + i2s(sumDim) +
      " dimensions: " + e->GetParString(0));

    if (!KwCumul) {

      if (KwPre) {
        switch (p0->Type()) {
        case GDL_BYTE: return product_over_dim_template<DByteGDL>(static_cast<DByteGDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_INT: return product_over_dim_template<DIntGDL>(static_cast<DIntGDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_UINT: return product_over_dim_template<DUIntGDL>(static_cast<DUIntGDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_LONG: return product_over_dim_template<DLongGDL>(static_cast<DLongGDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_ULONG: return product_over_dim_template<DULongGDL>(static_cast<DULongGDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_LONG64: return product_over_dim_template<DLong64GDL>(static_cast<DLong64GDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_ULONG64: return product_over_dim_template<DULong64GDL>(static_cast<DULong64GDL*> (p0), srcDim, sumDim - 1, nanInt);
        case GDL_FLOAT: return product_over_dim_template<DFloatGDL>(static_cast<DFloatGDL*> (p0), srcDim, sumDim - 1, KwNaN);
        case GDL_DOUBLE: return product_over_dim_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0), srcDim, sumDim - 1, KwNaN);
        case GDL_COMPLEX: return product_over_dim_template<DComplexGDL>(static_cast<DComplexGDL*> (p0), srcDim, sumDim - 1, KwNaN);
        case GDL_COMPLEXDBL: return product_over_dim_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0), srcDim, sumDim - 1, KwNaN);
        default: assert(false);
        }
      }

      // Integer parts derivated from Total code by Erin Sheldon
      // In IDL PRODUCT(), the INTEGER keyword takes precedence
      if (KwInt) {
        // We use GDL_LONG64 unless the input is GDL_ULONG64
        if ((p0->Type() == GDL_LONG64) && (!KwNaN)) {
          return product_over_dim_template<DLong64GDL>
            (static_cast<DLong64GDL*> (p0), srcDim, sumDim - 1, nanInt);
        }
        if ((p0->Type() == GDL_ULONG64) && (!KwNaN)) {
          return product_over_dim_template<DULong64GDL>
            (static_cast<DULong64GDL*> (p0), srcDim, sumDim - 1, nanInt);
        }

        // Conver to Long64
        DLong64GDL* p0L64 = static_cast<DLong64GDL*>
          (p0->Convert2(GDL_LONG64, BaseGDL::COPY));
        Guard<DLong64GDL> guard(p0L64);
        if (KwNaN) {
          DFloatGDL* p0f = static_cast<DFloatGDL*>
            (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
          Guard<DFloatGDL> guard(p0f);
          for (SizeT i = 0; i < nEl; ++i) {
            if (!std::isfinite((*p0f)[i])) (*p0L64)[i] = 1;
          }
        }
        return product_over_dim_template<DLong64GDL>
          (p0L64, srcDim, sumDim - 1, nanInt);
      } // integer results

      if (p0->Type() == GDL_DOUBLE) {
        return product_over_dim_template< DDoubleGDL>
          (static_cast<DDoubleGDL*> (p0), srcDim, sumDim - 1, KwNaN);
      }
      if (p0->Type() == GDL_COMPLEXDBL) {
        return product_over_dim_template< DComplexDblGDL>
          (static_cast<DComplexDblGDL*> (p0), srcDim, sumDim - 1, KwNaN);
      }
      if (p0->Type() == GDL_COMPLEX) {
        DComplexDblGDL* p0D = static_cast<DComplexDblGDL*>
          (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
        Guard<DComplexDblGDL> p0D_guard(p0D);
        //      p0D_guard.Reset( p0D);
        return product_over_dim_template< DComplexDblGDL>
          (p0D, srcDim, sumDim - 1, KwNaN);
      }

      DDoubleGDL* p0D = static_cast<DDoubleGDL*>
        (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
      Guard<DDoubleGDL> p0D_guard(p0D);
      //p0D_guard.Reset( p0D);
      return product_over_dim_template< DDoubleGDL>
        (p0D, srcDim, sumDim - 1, KwNaN);
    } else { // KwCumul

      if (KwPre) {
        switch (p0->Type()) {
        case GDL_BYTE: return product_over_dim_cu_template<DByteGDL>(static_cast<DByteGDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_INT: return product_over_dim_cu_template<DIntGDL>(static_cast<DIntGDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_UINT: return product_over_dim_cu_template<DUIntGDL>(static_cast<DUIntGDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_LONG: return product_over_dim_cu_template<DLongGDL>(static_cast<DLongGDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_ULONG: return product_over_dim_cu_template<DULongGDL>(static_cast<DULongGDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_LONG64: return product_over_dim_cu_template<DLong64GDL>(static_cast<DLong64GDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_ULONG64: return product_over_dim_cu_template<DULong64GDL>(static_cast<DULong64GDL*> (p0->Dup()), sumDim - 1, nanInt);
        case GDL_FLOAT: return product_over_dim_cu_template<DFloatGDL>(static_cast<DFloatGDL*> (p0->Dup()), sumDim - 1, KwNaN);
        case GDL_DOUBLE: return product_over_dim_cu_template<DDoubleGDL>(static_cast<DDoubleGDL*> (p0->Dup()), sumDim - 1, KwNaN);
        case GDL_COMPLEX: return product_over_dim_cu_template<DComplexGDL>(static_cast<DComplexGDL*> (p0->Dup()), sumDim - 1, KwNaN);
        case GDL_COMPLEXDBL: return product_over_dim_cu_template<DComplexDblGDL>(static_cast<DComplexDblGDL*> (p0->Dup()), sumDim - 1, KwNaN);
        default: assert(false);
        }
      }

      // Integer parts derivated from Total code by Erin Sheldon
      // In IDL PRODUCT(), the INTEGER keyword takes precedence
      if (KwInt) {
        // We use GDL_LONG64 unless the input is GDL_ULONG64
        if ((p0->Type() == GDL_LONG64) && (!KwNaN)) {
          return product_over_dim_cu_template<DLong64GDL>
            (static_cast<DLong64GDL*> (p0->Dup()), sumDim - 1, nanInt);
        }
        if ((p0->Type() == GDL_ULONG64) && (!KwNaN)) {
          return product_over_dim_cu_template<DULong64GDL>
            (static_cast<DULong64GDL*> (p0->Dup()), sumDim - 1, nanInt);
        }

        // Convert to Long64
        if (KwNaN) {
          DFloatGDL* p0f = static_cast<DFloatGDL*>
            (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
          Guard<DFloatGDL> guard(p0f);
          for (SizeT i = 0; i < nEl; ++i) {
            if (!std::isfinite((*p0f)[i])) (*p0f)[i] = 1;
          }
          return product_over_dim_cu_template<DLong64GDL>
            (static_cast<DLong64GDL*>
            (p0f->Convert2(GDL_LONG64, BaseGDL::COPY)), sumDim - 1, nanInt);
        } else {
          return product_over_dim_cu_template<DLong64GDL>
            (static_cast<DLong64GDL*>
            (p0->Convert2(GDL_LONG64, BaseGDL::COPY)), sumDim - 1, nanInt);
        }
      } // integer results

      if (p0->Type() == GDL_DOUBLE) {
        return product_over_dim_cu_template< DDoubleGDL>
          (static_cast<DDoubleGDL*> (p0->Dup()), sumDim - 1, KwNaN);
      }
      if (p0->Type() == GDL_COMPLEXDBL) {
        return product_over_dim_cu_template< DComplexDblGDL>
          (static_cast<DComplexDblGDL*> (p0->Dup()), sumDim - 1, KwNaN);
      }
      if (p0->Type() == GDL_COMPLEX) {
        return product_over_dim_cu_template< DComplexDblGDL>
          (static_cast<DComplexDblGDL*>
          (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY)), sumDim - 1, KwNaN);
      }

      return product_over_dim_cu_template< DDoubleGDL>
        (static_cast<DDoubleGDL*>
        (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY)), sumDim - 1, KwNaN);
    }
  }
  //  servicing array_equal and also gdl_container::equals

  bool array_equal_bool(BaseGDL* p0, BaseGDL* p1,
    bool notypeconv = false, bool not_equal = false,
    bool quiet = true) {

    if (p0 == p1) return true;
    if (p0 == 0 or p1 == 0) return false;
    SizeT nEl0 = p0->N_Elements();
    SizeT nEl1 = p1->N_Elements();

    // first case : arrays with differents size (>1)
    if (nEl0 != nEl1 && nEl0 != 1 && nEl1 != 1)
      return false;

    // if one of input has only one element, it should NOt be an array
    // ARRAY_EQUAL(1,[1,1]) True, ARRAY_EQUAL([1],[1,1]) False !!
    if (nEl0 != nEl1) {
      if (nEl0 == 1 && nEl1 != 1) {
        if (!p0->StrictScalar()) return false;
      }
      if (nEl0 != 1 && nEl1 == 1) {
        if (!p1->StrictScalar()) return false;
      }
    }

    //cout << "pO "<< p0->Dim() << " p1 "<< p1->Dim() << endl;
    //cout << "pO "<< p0->StrictScalar() << " p1 "<< p1->StrictScalar() << endl;
    DType aTy = p0->Type();
    DType bTy = p1->Type();

    if (aTy == GDL_STRUCT or bTy == GDL_STRUCT) {
      if (quiet) return false;
      throw GDLException("array_equal: inconvertable GDL_STRUCT");
    }

    Guard<BaseGDL> p0_guard;
    Guard<BaseGDL> p1_guard;

    if ((aTy == GDL_PTR and bTy == GDL_PTR) or
      ( aTy == GDL_OBJ and bTy == GDL_OBJ)) {
      Data_<SpDULong64>* p0t =
        static_cast<Data_<SpDULong64>*> (p0);
      if (not_equal) return p0t->ArrayNeverEqual(p1);
      else return p0t->ArrayEqual(p1);
    } else if (aTy == GDL_PTR or bTy == GDL_PTR) {
      if (quiet) return false;
      throw GDLException("array_equal: GDL_PTR only with PTR");
    } else if (aTy == GDL_OBJ or bTy == GDL_OBJ) {
      if (quiet) return false;
      throw GDLException("array_equal: GDL_OBJ only with OBJ");
    } else if (aTy != bTy) {
      if (notypeconv) // NO_TYPECONV
        return false;
      else {
        if (!ConvertableType(aTy) or !ConvertableType(bTy)) {
          if (quiet) return false;
          throw GDLException("array_equal: inconvertable type");
        } else if (DTypeOrder[aTy] >= DTypeOrder[bTy]) {
          p1 = p1->Convert2(aTy, BaseGDL::COPY);
          p1_guard.Reset(p1);
        } else {
          p0 = p0->Convert2(bTy, BaseGDL::COPY);
          p0_guard.Reset(p0);
        }
      }
    }
    if (not_equal) return p0->ArrayNeverEqual(p1);
    else return p0->ArrayEqual(p1);
  }

  BaseGDL* array_equal(EnvT* e) {
    e->NParam(2);
    //   trace_me = trace_arg();
    static int notypeconvIx = e->KeywordIx("NO_TYPECONV");
    static int notequalIx = e->KeywordIx("NOT_EQUAL");
    static int quietIx = e->KeywordIx("QUIET");
    //  if(trace_me) cout << " array=? ";
    BaseGDL* p0 = e->GetParDefined(0);
    BaseGDL* p1 = e->GetParDefined(1);

    bool result = array_equal_bool(p0, p1,
      e->KeywordSet(notypeconvIx), e->KeywordSet(notequalIx),
      e->KeywordSet(quietIx));
    //   if(trace_me) cout << result<< endl;
    return new DByteGDL(result ? 1 : 0);
  }

  BaseGDL* min_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);
    BaseGDL* searchArr = e->GetParDefined(0);

    static int omitNaNIx = e->KeywordIx("NAN");
    bool omitNaN = e->KeywordSet(omitNaNIx);

    static int subIx = e->KeywordIx("SUBSCRIPT_MAX");
    bool subMax = e->WriteableKeywordPresent(subIx); //insure the output variable exist and is of 'good' type

    static int dimIx = e->KeywordIx("DIMENSION");
    bool dimSet = e->KeywordSet(dimIx);

    static int maxIx = e->KeywordIx("MAX");
    bool maxSet = e->WriteableKeywordPresent(maxIx); //insure the output variable exist and is of 'good' type

    static int absIx = e->KeywordIx("ABSOLUTE");
    bool absSet = e->KeywordSet(absIx); // not KeywordPresent as it should be ignored if not set.

    DLong searchDim;
    if (dimSet) {
      e->AssureLongScalarKW(dimIx, searchDim);
      if (searchDim < 0 || searchDim > searchArr->Rank())
        e->Throw("Illegal keyword value for DIMENSION");
    }

    if (dimSet && searchArr->Rank() > 1) {
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
      DLongGDL *minElArr = NULL, *maxElArr = NULL;

      if (maxSet) {
        maxVal = searchArr->New(destDim, BaseGDL::NOZERO);
      }

      if (subMax) {
        maxElArr = new DLongGDL(destDim);
      }

      if (nParam == 2) {
        e->AssureGlobalPar(1); // instead of using a guard pointer
        minElArr = new DLongGDL(destDim);
      }

      SizeT rIx = 0;
      for (SizeT o = 0; o < nEl; o += outerStride) {
        SizeT rIx = (o / outerStride) * searchStride;
        for (SizeT i = 0; i < searchStride; ++i) {
          searchArr->MinMax(
            (nParam == 2 ? &((*minElArr)[rIx]) : NULL),
            (subMax ? &((*maxElArr)[rIx]) : NULL),
            &resArr,
            (maxSet ? &maxVal : NULL),
            omitNaN, o + i, searchLimit + o + i, searchStride, rIx, absSet
            );
          rIx++;
        }
      }
      if (nParam == 2) e->SetPar(1, minElArr);
      if (subMax) e->SetKW(subIx, maxElArr);
      if (maxSet) e->SetKW(maxIx, maxVal);

      return resArr;
    } else {
      DLong minEl;
      BaseGDL* res;

      if (maxSet) // MAX keyword given
      {
        e->AssureGlobalKW(0);
        GDLDelete(e->GetTheKW(0)); //TheKW to keep old behaviour, OK since GlobalPar is assured.
        DLong maxEl;
        searchArr->MinMax(&minEl, &maxEl, &res, &e->GetTheKW(0), omitNaN, 0, 0, 1, -1, absSet);
        if (subMax) e->SetKW(subIx, new DLongGDL(maxEl));
      } else // no MAX keyword
      {
        if (subMax) {
          DLong maxEl;
          searchArr->MinMax(&minEl, &maxEl, &res, NULL, omitNaN, 0, 0, 1, -1, absSet);
          e->SetKW(subIx, new DLongGDL(maxEl));
        } else searchArr->MinMax(&minEl, NULL, &res, NULL, omitNaN, 0, 0, 1, -1, absSet);
      }

      // handle index
      if (nParam == 2) e->SetPar(1, new DLongGDL(minEl));
      else SysVar::SetC(minEl);
      return res;
    }
  }

  BaseGDL* max_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);
    BaseGDL* searchArr = e->GetParDefined(0);

    static int omitNaNIx = e->KeywordIx("NAN");
    bool omitNaN = e->KeywordSet(omitNaNIx);

    static int subIx = e->KeywordIx("SUBSCRIPT_MIN");
    bool subMin = e->WriteableKeywordPresent(subIx);

    static int dimIx = e->KeywordIx("DIMENSION");
    bool dimSet = e->KeywordSet(dimIx);

    static int minIx = e->KeywordIx("MIN");
    bool minSet = e->WriteableKeywordPresent(minIx);

    static int absIx = e->KeywordIx("ABSOLUTE");
    bool absSet = e->KeywordSet(absIx); // not KeywordPresent as it should be ignored if not set.

    DLong searchDim;
    if (dimSet) {
      e->AssureLongScalarKW(dimIx, searchDim);
      if (searchDim < 0 || searchDim > searchArr->Rank())
        e->Throw("Illegal keyword value for DIMENSION");
    }

    if (dimSet && searchArr->Rank() > 1) {
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
      DLongGDL *minElArr = NULL, *maxElArr = NULL;

      if (minSet) {
        minVal = searchArr->New(destDim, BaseGDL::NOZERO);
      }

      if (subMin) {
        minElArr = new DLongGDL(destDim);
      }

      if (nParam == 2) {
        e->AssureGlobalPar(1); // instead of using a guard pointer
        maxElArr = new DLongGDL(destDim);
      }
      for (SizeT o = 0; o < nEl; o += outerStride) {
        SizeT rIx = (o / outerStride) * searchStride;
        for (SizeT i = 0; i < searchStride; ++i) {
          searchArr->MinMax(
            (subMin ? &((*minElArr)[rIx]) : NULL),
            (nParam == 2 ? &((*maxElArr)[rIx]) : NULL),
            (minSet ? &minVal : NULL),
            &resArr,
            omitNaN, o + i, searchLimit + o + i, searchStride, rIx, absSet
            );
          rIx++;
        }
      }
      if (nParam == 2) e->SetPar(1, maxElArr);
      if (subMin) e->SetKW(subIx, minElArr);
      if (minSet) e->SetKW(minIx, minVal);

      return resArr;
    } else {
      DLong maxEl;
      BaseGDL* res;

      if (minSet) // MIN keyword given
      {
        e->AssureGlobalKW(0);
        GDLDelete(e->GetTheKW(0)); //TheKW to keep old behaviour, OK since GlobalPar is assured.
        DLong minEl;
        searchArr->MinMax(&minEl, &maxEl, &e->GetTheKW(0), &res, omitNaN, 0, 0, 1, -1, absSet);
        if (subMin) e->SetKW(subIx, new DLongGDL(minEl));
      } else // no MIN keyword
      {
        if (subMin) {
          DLong minEl;
          searchArr->MinMax(&minEl, &maxEl, NULL, &res, omitNaN, 0, 0, 1, -1, absSet);
          e->SetKW(subIx, new DLongGDL(minEl));
        } else searchArr->MinMax(NULL, &maxEl, NULL, &res, omitNaN, 0, 0, 1, -1, absSet);
      }

      // handle index
      if (nParam == 2) e->SetPar(1, new DLongGDL(maxEl));
      else SysVar::SetC(maxEl);
      return res;
    }
  }

  BaseGDL* transpose(EnvT* e) {
    SizeT nParam = e->NParam(1);

    BaseGDL* p0 = e->GetParDefined(0);
    if (p0->Type() == GDL_STRUCT)
      e->Throw("Struct expression not allowed in this context: " +
      e->GetParString(0));

    SizeT rank = p0->Rank();
    if (rank == 0)
      e->Throw("Expression must be an array "
      "in this context: " + e->GetParString(0));

    if (nParam == 2) {

      BaseGDL* p1 = e->GetParDefined(1);
      if (p1->N_Elements() != rank)
        e->Throw("Incorrect number of elements in permutation.");

      DUInt* perm = new DUInt[rank];
      ArrayGuard<DUInt> perm_guard(perm);

      DUIntGDL* p1L = static_cast<DUIntGDL*>
        (p1->Convert2(GDL_UINT, BaseGDL::COPY));
      for (SizeT i = 0; i < rank; ++i) perm[i] = (*p1L)[ i];
      GDLDelete(p1L);

      // check permutation vector
      for (SizeT i = 0; i < rank; ++i) {
        DUInt j;
        for (j = 0; j < rank; ++j) if (perm[j] == i) break;
        if (j == rank)
          e->Throw("Incorrect permutation vector.");
      }
      //check we transpose something
      bool identical=true;
      for (SizeT i = 0; i < rank; ++i) if (perm[i] != i) identical=false;
      if (identical) return p0->Dup();
      
      return p0->Transpose(perm);
    }

    return p0->Transpose(NULL);
  }


  template< typename IndexT>
  void MergeSortOpt(BaseGDL* p0, IndexT* hhS, IndexT* h1, IndexT* h2,
    SizeT len) {
    if (len <= 1) return;

    SizeT h1N = len / 2;
    SizeT h2N = len - h1N;

    // 1st half
    MergeSortOpt(p0, hhS, h1, h2, h1N);

    // 2nd half
    IndexT* hhM = &hhS[h1N];
    MergeSortOpt(p0, hhM, h1, h2, h2N);

    SizeT i;
    for (i = 0; i < h1N; ++i) h1[i] = hhS[ i];
    for (i = 0; i < h2N; ++i) h2[i] = hhM[ i];

    SizeT h1Ix = 0;
    SizeT h2Ix = 0;
    for (i = 0; (h1Ix < h1N) && (h2Ix < h2N); ++i) {
      // the actual comparisson
      if (p0->Greater(h1[h1Ix], h2[h2Ix]))
        hhS[ i] = h2[ h2Ix++];
      else
        hhS[ i] = h1[ h1Ix++];
    }
    for (; h1Ix < h1N; ++i) hhS[ i] = h1[ h1Ix++];
    for (; h2Ix < h2N; ++i) hhS[ i] = h2[ h2Ix++];
  }

  // start of highly-optimized median code. 1D and 2D fast medians are in medianfilter.cpp, gathered from
  // recent sources. see the include file for explanations & copyrights.
#include "medianfilter.cpp"
  /*
   *  Following routines are variants of the algorithm described in
   *  "Numerical recipes in C", Second Edition,
   *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
   *  Original code by Nicolas Devillard - 1998. Public domain.
   *  Modified by G. Duvert, 2017, for NaN/INF handling and correction of Nicolas's code
   *  which gave erroneous results when two or more elements were identical.
   */
#define ELEM_SWAP(a,b) { DDouble t=(a);(a)=(b);(b)=t; }

  DDouble quick_select_d(DDouble array[], SizeT arraySize, int even) {

    if (arraySize == 1) return array[0];

    SizeT high, low, middle;
    SizeT median = (arraySize) / 2;
    SizeT ll, hh;
    DDouble pivot;
    low = 0;
    high = arraySize - 1;
    for (;;) {
      if (high <= low + 1) {
        if (high == low + 1 && array[high] < array[low]) {
          ELEM_SWAP(array[low], array[high])
        }
        if (even) return 0.5 * (array[median] + array[median - 1]);
        else return array[median];
      } else {
        middle = (low + high) / 2;
        ELEM_SWAP(array[middle], array[low + 1])
        if (array[low] > array[high]) {
          ELEM_SWAP(array[low], array[high])
        }
        if (array[low + 1] > array[high]) {
          ELEM_SWAP(array[low + 1], array[high])
        }
        if (array[low] > array[low + 1]) {
          ELEM_SWAP(array[low], array[low + 1])
        }
        ll = low + 1;
        hh = high;
        pivot = array[low + 1];
        for (;;) {
          do ll++; while (array[ll] < pivot);
          do hh--; while (array[hh] > pivot);
          if (hh < ll) break;
          ELEM_SWAP(array[ll], array[hh])
        }
        array[low + 1] = array[hh];
        array[hh] = pivot;
        if (hh >= median) high = hh - 1;
        if (hh <= median) low = ll;
      }
    }

  }

#undef ELEM_SWAP
#define ELEM_SWAP(a,b) { DFloat t=(a);(a)=(b);(b)=t; }

  DFloat quick_select_f(DFloat array[], SizeT arraySize, int even) {

    if (arraySize == 1) return array[0];

    SizeT high, low, middle;
    SizeT median = (arraySize) / 2;
    SizeT ll, hh;
    DFloat pivot;
    low = 0;
    high = arraySize - 1;
    for (;;) {
      if (high <= low + 1) {
        if (high == low + 1 && array[high] < array[low]) {
          ELEM_SWAP(array[low], array[high])
        }
        if (even) return 0.5 * (array[median] + array[median - 1]);
        else return array[median];
      } else {
        middle = (low + high) / 2;
        ELEM_SWAP(array[middle], array[low + 1])
        if (array[low] > array[high]) {
          ELEM_SWAP(array[low], array[high])
        }
        if (array[low + 1] > array[high]) {
          ELEM_SWAP(array[low + 1], array[high])
        }
        if (array[low] > array[low + 1]) {
          ELEM_SWAP(array[low], array[low + 1])
        }
        ll = low + 1;
        hh = high;
        pivot = array[low + 1];
        for (;;) {
          do ll++; while (array[ll] < pivot);
          do hh--; while (array[hh] > pivot);
          if (hh < ll) break;
          ELEM_SWAP(array[ll], array[hh])
        }
        array[low + 1] = array[hh];
        array[hh] = pivot;
        if (hh >= median) high = hh - 1;
        if (hh <= median) low = ll;
      }
    }

  }

#undef ELEM_SWAP
  //input-protected versions of above

  DFloat quick_select_f_protect_input(const DFloat data[], SizeT arraySize, int even) {
    DFloat * array = (DFloat*) malloc(arraySize * sizeof (DFloat));
    for (SizeT i = 0; i < arraySize; ++i) array[i] = data[i];
    DFloat res = quick_select_f(array, arraySize, even);
    free(array);
    return res;
  }

  DDouble quick_select_d_protect_input(const DDouble data[], SizeT arraySize, int even) {
    DDouble * array = (DDouble*) malloc(arraySize * sizeof (DDouble));
    for (SizeT i = 0; i < arraySize; ++i) array[i] = data[i];
    DDouble res = quick_select_d(array, arraySize, even);
    free(array);
    return res;
  }

  //simple median for double arrays with no NaNs.

  inline BaseGDL* mymedian_d(EnvT* e) {
    
    DDoubleGDL* array = e->GetParAs<DDoubleGDL>(0)->Dup(); //original array is protected
    SizeT nEl = array->N_Elements();
    static int evenIx = e->KeywordIx("EVEN");
    int iseven = ((nEl % 2) == 0 && e->KeywordSet(evenIx));
    BaseGDL *res = new DDoubleGDL(quick_select_d((DDouble*) array->DataAddr(), nEl, iseven));

    delete array;

    return res;
  }

  //simple median for double arrays whith NaNs. Remove the Nans before doing the median.

  inline BaseGDL* mymedian_d_nan(EnvT* e) {
    DDoubleGDL* data = e->GetParAs<DDoubleGDL>(0); //original array is protected
    SizeT nEl = data->N_Elements();
    DLong iEl = 0;
    DDouble * array = (DDouble*) malloc(nEl * sizeof (DDouble));
    for (SizeT i = 0; i < data->N_Elements(); ++i) {
      if (!isnan((*data)[i])) {
        array[iEl] = (*data)[i];
        iEl++;
      }
    }
    if (iEl == 0) {
      free(array);
      return new DDoubleGDL(std::numeric_limits<double>::quiet_NaN());
    }
    static int evenIx = e->KeywordIx("EVEN");
    int iseven = (((iEl + 1) % 2) == 0 && e->KeywordSet(evenIx));
    BaseGDL *res = new DDoubleGDL(quick_select_d(array, iEl, iseven));
    free(array);
    return res;
  }
  //simple median for double arrays whith NaNs. Remove the Nans before doing the median.

  inline DDouble quick_select_d_filter_nan(const DDouble* arr, SizeT nEl, int even) {
    DLong iEl = 0;
    DDouble* array = (DDouble*) malloc(nEl * sizeof (DDouble));
    for (SizeT i = 0; i < nEl; ++i) {
      if (!isnan(arr[i])) {
        array[iEl] = arr[i];
        iEl++;
      }
    }
    if (iEl == 0) {
      free(array);
      return std::numeric_limits<double>::quiet_NaN();
    }
    DDouble res = quick_select_d(array, iEl, even);
    free(array);
    return res;
  }

  inline bool hasnan_d(DDouble* arr, SizeT nEl) {
    for (SizeT i = 0; i < nEl; ++i) if (isnan(arr[i])) return true;
    return false;
  }

  inline BaseGDL* mymedian_f(EnvT* e) {
    DFloatGDL* array = e->GetParAs<DFloatGDL>(0)->Dup(); //original array is protected
    SizeT nEl = array->N_Elements();

    static int evenIx = e->KeywordIx("EVEN");
    int iseven = ((nEl % 2) == 0 && e->KeywordSet(evenIx));
    BaseGDL *res = new DFloatGDL(quick_select_f((DFloat*) array->DataAddr(), nEl, iseven));

    delete array;

    return res;
  }

  inline BaseGDL* mymedian_f_nan(EnvT* e) {
    DFloatGDL* data = e->GetParAs<DFloatGDL>(0); //original array is protected
    SizeT nEl = data->N_Elements();
    DLong iEl = 0;
    DFloat * array = (DFloat*) malloc(nEl * sizeof (DFloat));
    for (SizeT i = 0; i < data->N_Elements(); ++i) {
      if (!isnan((*data)[i])) {
        array[iEl] = (*data)[i];
        iEl++;
      }
    }
    if (iEl == 0) {
      free(array);
      return new DFloatGDL(std::numeric_limits<float>::quiet_NaN());
    }
    static int evenIx = e->KeywordIx("EVEN");
    int iseven = (((iEl + 1) % 2) == 0 && e->KeywordSet(evenIx));
    BaseGDL *res = new DFloatGDL(quick_select_f(array, iEl, iseven));
    free(array);
    return res;
  }

  inline DFloat quick_select_f_filter_nan(const DFloat* arr, SizeT nEl, int even) {
    DLong iEl = 0;
    DFloat * array = (DFloat*) malloc(nEl * sizeof (DFloat));
    for (SizeT i = 0; i < nEl; ++i) {
      if (!isnan(arr[i])) {
        array[iEl] = arr[i];
        iEl++;
      }
    }
    if (iEl == 0) {
      free(array);
      return std::numeric_limits<float>::quiet_NaN();
    }
    DFloat res = quick_select_f(array, iEl, even);
    free(array);
    return res;
  }

  inline bool hasnan_f(DFloat* arr, SizeT nEl) {
    for (SizeT i = 0; i < nEl; ++i) if (isnan(arr[i])) return true;
    return false;
  }

  BaseGDL* SlowReliableMedian(EnvT* e); //see below.

  BaseGDL* median(EnvT* e) {
    BaseGDL* p0 = e->GetParDefined(0);
    SizeT nParam = e->NParam(1); //get number of parameters, must be >=1.

    if (p0->Rank() == 0)
      e->Throw("Expression must be an array in this context: " + e->GetParString(0));

    if (p0->Type() == GDL_PTR)
      e->Throw("Pointer expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_OBJ)
      e->Throw("Object expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_STRUCT)
      e->Throw("Struct expression not allowed in this context: " + e->GetParString(0));

    static int doubleIx = e->KeywordIx("DOUBLE");
    bool dbl =
      (p0->Type() == GDL_DOUBLE ||
      p0->Type() == GDL_COMPLEXDBL ||
      e->KeywordSet(doubleIx));
    //contrary to doc (?) EVEN is useable everywhere, 1D or 2D.

    static int evenIx = e->KeywordIx("EVEN");

    if (nParam == 1) {
      // Check conversion to real or double:

      // Check possibility of Nan (not useful to speed down medians on integer data which
      // will never produce NaNs).
      bool possibleNaN = (p0->Type() == GDL_DOUBLE ||
        p0->Type() == GDL_FLOAT ||
        p0->Type() == GDL_COMPLEX ||
        p0->Type() == GDL_COMPLEXDBL);

      //DIMENSION Kw
      static int dimIx = e->KeywordIx("DIMENSION");
      bool dimSet = e->KeywordSet(dimIx);

      DLong medianDim;
      if (dimSet) {
        e->AssureLongScalarKW(dimIx, medianDim);
        if (medianDim < 0 || medianDim > p0->Rank())
          e->Throw("Illegal keyword value for DIMENSION");
      }

      if (dimSet && p0->Rank() > 1) {
        medianDim -= 1; // user-supplied dimensions start with 1!


        // input/output dimensions: copy srcDim to destDim
        dimension destDim = p0->Dim();
        // make array of dims for transpose
        DUInt* perm = new DUInt[p0->Rank()];
        ArrayGuard<DUInt> perm_guard(perm);
        //useful to reorder dims for transpose to order data in continuous 'medianing' order.
        DUInt i = 0, j = 0;
        for (i = 0; i < p0->Rank(); ++i) if (i != medianDim) {
            perm[j + 1] = i;
            j++;
          }
        perm[0] = medianDim;
        // resize destDim
        destDim.Remove(medianDim);
        //compute stride and number of elements of result:
        SizeT stride = p0->Dim(medianDim);
        int iseven = ((stride % 2) == 0 && e->KeywordSet(evenIx));

        SizeT nEl = destDim.NDimElementsConst();

        //transpose p0 to arrange dimensions if medianDim is > 0. Do not forget to remove transposed array.
        bool clean_array = false;
        if (possibleNaN) {
          bool hasnan = false;
          if (dbl) {
            DDoubleGDL* input = e->GetParAs<DDoubleGDL>(0);
            if (medianDim != 0) {
              input = static_cast<DDoubleGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
              clean_array = true;
            }
            DDoubleGDL* res = new DDoubleGDL(destDim, BaseGDL::NOZERO);
            for (SizeT i = 0; i < nEl; ++i) {
              if (hasnan_d(&(*input)[i * stride], stride)) (*res)[i] = quick_select_d_filter_nan(&(*input)[i * stride], stride, iseven); //special if nan.
              else (*res)[i] = quick_select_d_protect_input(&(*input)[i * stride], stride, iseven);
            }
            if (clean_array) delete input;
            return res;
          } else {
            DFloatGDL* input = e->GetParAs<DFloatGDL>(0);
            if (medianDim != 0) {
              input = static_cast<DFloatGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
              clean_array = true;
            }
            DFloatGDL* res = new DFloatGDL(destDim, BaseGDL::NOZERO);
            for (SizeT i = 0; i < nEl; ++i) {
              if (hasnan_f(&(*input)[i * stride], stride)) (*res)[i] = quick_select_f_filter_nan(&(*input)[i * stride], stride, iseven); //special if nan.
              else (*res)[i] = quick_select_f_protect_input(&(*input)[i * stride], stride, iseven);
            }
            if (clean_array) delete input;
            return res;
          }
        } else { //faster since no NaN handling needed.
          if (dbl) {
            DDoubleGDL* input = e->GetParAs<DDoubleGDL>(0);
            if (medianDim != 0) {
              input = static_cast<DDoubleGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
              clean_array = true;
            }
            DDoubleGDL* res = new DDoubleGDL(destDim, BaseGDL::NOZERO);
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = quick_select_d_protect_input(&(*input)[i * stride], stride, iseven);
            if (clean_array) delete input;
            return res;
          } else {
            DFloatGDL* input = e->GetParAs<DFloatGDL>(0);
            if (medianDim != 0) {
              input = static_cast<DFloatGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
              clean_array = true;
            }
            DFloatGDL* res = new DFloatGDL(destDim, BaseGDL::NOZERO);
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = quick_select_f_protect_input(&(*input)[i * stride], stride, iseven);
            if (clean_array) delete input;
            return res;
          }
        }
      } else {
        if (possibleNaN) {
          if (dbl) {
            return mymedian_d_nan(e);
          } else {
            return mymedian_f_nan(e);
          }
        } else {
          if (dbl) return mymedian_d(e);
          else return mymedian_f(e);
        }
      }
    } else if (nParam == 2) {

      if (p0->Rank() > 2)
        e->Throw("Only 1 or 2 dimensions allowed: " + e->GetParString(0));
      //rank is important as fast algos are different!
      bool twoD = (p0->Rank() == 2);

      // basic checks on "width" input
      DDoubleGDL* p1d = e->GetParAs<DDoubleGDL>(1);

      if (p1d->N_Elements() > 1 || (*p1d)[0] <= 0)
        e->Throw("Width must be a positive scalar or 1 (positive) element array in this context: " + e->GetParString(0));
      DLong MaxAllowedWidth = 0;
      if (twoD) {
        MaxAllowedWidth = p0->Dim(0);
        if (p0->Dim(1) < MaxAllowedWidth) MaxAllowedWidth = p0->Dim(1);
      } else MaxAllowedWidth = p0->N_Elements();

      if (!std::isfinite((*p1d)[0]))
        e->Throw("Width must be > 1, and < dimension of array (NaN or Inf)");
      if ((*p1d)[0] < 2 || (*p1d)[0] > MaxAllowedWidth)
        e->Throw("Width must be > 1, and < dimensions: <INT (" + i2s(MaxAllowedWidth) + ")>.");
      DIntGDL* p1 = e->GetParAs<DIntGDL>(1);

      int width = p0->Dim(0);
      int height = twoD ? p0->Dim(1) : 1;
      int size = (*p1)[0];
      int radius = (size - 1) / 2;
      bool oddsize = (size % 2 == 1);

      bool iseven = ((size % 2) == 0 && e->KeywordSet(evenIx));

      if (p0->Type() == GDL_BYTE && twoD && oddsize) {
        // for this special case we apply the constant-time algorithm described in Perreault et al,
        // Published in the September 2007 issue of IEEE Transactions on Image Processing. DOI: 10.1109/TIP.2007.902329
        DByteGDL* data = e->GetParAs<DByteGDL>(0);
        BaseGDL* res = new DByteGDL(data->Dim(), BaseGDL::NOZERO);
        fastmedian::ctmf(
          (unsigned char*) data->DataAddr(), (unsigned char*) res->DataAddr(),
          width, height,
          width, width,
          radius, 1, 32 * 1024); //for a 32K cache. FIXME-> get cache size value!!!
        return res;
      } else {
        //here we adapt according to problem using various solutions found in the literature.
        if (dbl) {
          DDoubleGDL* data = e->GetParAs<DDoubleGDL>(0);
          if (twoD) {
            if (oddsize) { //2D fast routines are programmed with odd sizes (2*radius+1)
              BaseGDL* res = new DDoubleGDL(data->Dim(), BaseGDL::NOZERO);
              fastmedian::median_filter_2d(width, height, radius, radius, 0, (DDouble*) data->DataAddr(), (DDouble*) res->DataAddr());
              return res;
            } else { //for quite a large number of pixels (100=10^2), use the next ODD value. Results are compatible within 1% for random values.
              //to be tested, but should be better for natural values.
              if (size > 10) {
                radius = size / 2; //1 more
                BaseGDL* res = new DDoubleGDL(data->Dim(), BaseGDL::NOZERO);
                fastmedian::median_filter_2d(width, height, radius, radius, 0, (DDouble*) data->DataAddr(), (DDouble*) res->DataAddr());
                if (p0->Type() == GDL_BYTE) return res->Convert2(GDL_BYTE, BaseGDL::CONVERT);
                else return res;
              } else return SlowReliableMedian(e); //until we rewrite a fast non-odd 2 d filter.
            }
          } else {
            if (oddsize) {
              BaseGDL* res = new DDoubleGDL(data->Dim(), BaseGDL::NOZERO);
              fastmedian::median_filter_1d(width, radius, 0, (DDouble*) data->DataAddr(), (DDouble*) res->DataAddr());
              return res;
            } else { //this oneD fast routine accepts odd and even sizes, but is slower than Jukka's
              BaseGDL* res = data->Dup();
              fastmedian::filter((DDouble*) res->DataAddr(), width, size, iseven);
              return res;
            }
          }
        } else {
          DFloatGDL* data = e->GetParAs<DFloatGDL>(0);
          if (twoD) {
            if (oddsize) { //2D fast routines are programmed with odd sizes (2*radius+1).
              BaseGDL* res = new DFloatGDL(data->Dim(), BaseGDL::NOZERO);
              fastmedian::median_filter_2d(width, height, radius, radius, 0, (DFloat*) data->DataAddr(), (DFloat*) res->DataAddr());
              return res;
            } else { //for quite a large number of pixels (100=10^2), use the next ODD value. Results are compatible within 1% for random values.
              //to be tested, but should be better for natural values.
              if (size > 10) {
                radius = size / 2; //1 more
                BaseGDL* res = new DFloatGDL(data->Dim(), BaseGDL::NOZERO);
                fastmedian::median_filter_2d(width, height, radius, radius, 0, (DFloat*) data->DataAddr(), (DFloat*) res->DataAddr());
                if (p0->Type() == GDL_BYTE) return res->Convert2(GDL_BYTE, BaseGDL::CONVERT);
                else return res;
              } else return SlowReliableMedian(e); //until we rewrite a fast non-odd 2 d filter.
            }
          } else {
            if (oddsize) { //Jukka's version is faster.
              BaseGDL* res = new DFloatGDL(data->Dim(), BaseGDL::NOZERO);
              fastmedian::median_filter_1d(width, radius, 0, (DFloat*) data->DataAddr(), (DFloat*) res->DataAddr());
              return res;
            } else { //this oneD fast routine accepts odd an even sizes.
              BaseGDL* res = data->Dup();
              fastmedian::filter((DFloat*) res->DataAddr(), width, size, iseven);
              if (p0->Type() == GDL_BYTE) return res->Convert2(GDL_BYTE, BaseGDL::CONVERT);
              else return res;
            }
          }
        }
      }
    }
    return NULL; //pacifies dumm compilers.
  }
  // uses MergeSort
  // 2 parts in the code: without "width" or with "width" (limited to 1D and 2D)

  BaseGDL* SlowReliableMedian(EnvT* e) {

    BaseGDL* p0 = e->GetParDefined(0);

    if (p0->Type() == GDL_PTR)
      e->Throw("Pointer expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_OBJ)
      e->Throw("Object expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_STRUCT)
      e->Throw("Struct expression not allowed in this context: " + e->GetParString(0));

    if (p0->Rank() == 0)
      e->Throw("Expression must be an array in this context: " + e->GetParString(0));

    SizeT nParam = e->NParam(1);
    SizeT nEl = p0->N_Elements();

    // "f_nan" and "d_nan" used by both parts ...
    DStructGDL *Values = SysVar::Values(); //MUST NOT BE STATIC, due to .reset
    DFloat f_nan = (*static_cast<DFloatGDL*> (Values->GetTag(Values->Desc()->TagIndex("F_NAN"), 0)))[0];
    DDouble d_nan = (*static_cast<DDoubleGDL*> (Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];

    // --------------------------------------------------------
    // begin of the part 1: without "width" param
    if (nParam == 1) {

      static int evenIx = e->KeywordIx("EVEN");

      // TYPE
      static int doubleIx = e->KeywordIx("DOUBLE");
      bool dbl =
        p0->Type() == GDL_DOUBLE ||
        p0->Type() == GDL_COMPLEXDBL ||
        e->KeywordSet(doubleIx);
      DType type = dbl ? GDL_DOUBLE : GDL_FLOAT;
      bool noconv = (dbl && p0->Type() == GDL_DOUBLE) ||
        (!dbl && p0->Type() == GDL_FLOAT);

      // DIMENSION keyword
      DLong dim = 0;
      DLong nmed = 1;
      BaseGDL *res;

      static int dimensionIx = e->KeywordIx("DIMENSION");
      e->AssureLongScalarKWIfPresent(dimensionIx, dim);

      //    cout << "dim : "<< dim << endl;

      if (dim > p0->Rank())
        e->Throw("Illegal keyword value for DIMENSION.");

      if (dim > 0) {
        DLong dims[8];
        DLong k = 0;
        for (SizeT i = 0; i < p0->Rank(); ++i)
          if (i != (dim - 1)) {
            nmed *= p0->Dim(i);
            dims[k++] = p0->Dim(i);
          }
        dimension dimRes((DLong *) dims, p0->Rank() - 1);
        res = dbl
          ? static_cast<BaseGDL*> (new DDoubleGDL(dimRes, BaseGDL::NOZERO))
          : static_cast<BaseGDL*> (new DFloatGDL(dimRes, BaseGDL::NOZERO));
      } else {
        res = dbl
          ? static_cast<BaseGDL*> (new DDoubleGDL(1))
          : static_cast<BaseGDL*> (new DFloatGDL(1));
      }

      // conversion of Complex types
      if (p0->Type() == GDL_COMPLEX) p0 = p0->Convert2(GDL_FLOAT, BaseGDL::COPY);
      if (p0->Type() == GDL_COMPLEXDBL) p0 = p0->Convert2(GDL_DOUBLE, BaseGDL::COPY);

      // helper arrays
      if (nmed > 1) nEl = p0->N_Elements() / nmed;

      //    cout << "hello2" << endl;

      DLong *hh = new DLong[ nEl];
      DLong* h1 = new DLong[ nEl / 2];
      DLong* h2 = new DLong[ (nEl + 1) / 2];

      DLong accumStride = 1;
      if (nmed > 1)
        for (DLong i = 0; i < dim - 1; ++i) accumStride *= p0->Dim(i);

      BaseGDL *op1, *op2, *op3;
      if (dbl) op3 = new DDoubleGDL(2);
      else op3 = new DFloatGDL(2);

      // nEl_extern is used to store "nEl" initial value
      DLong nanIx, nEl_extern;
      nEl_extern = nEl;
      //    if (nmed > 1) nEl_extern = p0->N_Elements() / nmed;
      //else nEl_extern = p0->N_Elements();

      //    cout << "hello type" << p0->Type() << endl;

      // Loop over all subarray medians
      for (SizeT k = 0; k < nmed; ++k) {

        //    nEl=nEl_extern;

        if (nmed == 1) {
          //cout << "hello inside 1D" << endl;
          for (DLong i = 0; i < nEl; ++i) hh[i] = i;
          nanIx = nEl;

          if (p0->Type() == GDL_DOUBLE) {
            DDoubleGDL* p0F = static_cast<DDoubleGDL*> (p0);
            for (DLong i = nEl - 1; i >= 0; --i) {
              if (isnan((*p0F)[i])) {
                --nanIx;
                hh[i] = hh[nanIx];
                hh[ nanIx] = i;
              }
            }
          }

          if (p0->Type() == GDL_FLOAT) {
            DFloatGDL* p0F = static_cast<DFloatGDL*> (p0);
            for (DLong i = nEl - 1; i >= 0; --i) {
              if (isnan((*p0F)[i])) {
                --nanIx;
                hh[i] = hh[nanIx];
                hh[ nanIx] = i;
              }
            }
          }

          //cout << "nEl " << nEl << " nanIx " << nanIx << endl;
          nEl = nanIx;
        } else {
          nanIx = nEl;
          nEl = nEl_extern;

          //          DLong nanIx = nEl;
          // Starting Element
          DLong start = accumStride * p0->Dim(dim - 1) * (k / accumStride) +
            (k % accumStride);
          for (DLong i = 0; i < nEl; ++i) hh[i] = start + i * accumStride;
          DLong jj;
          nanIx = nEl;

          if (p0->Type() == GDL_FLOAT) {
            DFloatGDL* p0F = static_cast<DFloatGDL*> (p0);
            for (DLong i = nEl - 1; i >= 0; --i) {
              jj = start + i * accumStride;
              if (isnan((*p0F)[ jj])) {
                --nanIx;
                hh[i] = hh[nanIx];
                hh[ nanIx] = i;
              }
            }
            nEl = nanIx;
          }

          if (p0->Type() == GDL_DOUBLE) {
            DDoubleGDL* p0F = static_cast<DDoubleGDL*> (p0);
            for (DLong i = nEl - 1; i >= 0; --i) {
              jj = start + i * accumStride;
              if (isnan((*p0F)[ jj])) {
                --nanIx;
                hh[i] = hh[nanIx];
                hh[ nanIx] = i;
              }
            }
            //cout << "nanIx :" << nanIx << "nEl :" << nEl << endl;
            nEl = nanIx;
          }
        }
        DLong medEl, medEl_1;

        // call the sort routine
        if (nEl > 1) {
          MergeSortOpt<DLong>(p0, hh, h1, h2, nEl);
          medEl = hh[ nEl / 2];
          medEl_1 = hh[ nEl / 2 - 1];
        } else {
          if (nEl == 1) {
            medEl = hh[0];
            medEl_1 = hh[0];
          } else { // normal case, more than one element, nothing to do
            //cout << "gasp : no result ! " << endl;
          }
        }

        if (nEl <= 0) { // we have a NaN
          if (dbl) (*static_cast<DDoubleGDL*> (res))[k] = d_nan;
          else (*static_cast<DFloatGDL*> (res))[k] = f_nan;
        } else {
          //cout << k << "" << (*static_cast<DFloatGDL*>(p0))[medEl] << " "
          //     << (*static_cast<DFloatGDL*>(p0))[medEl_1] << endl;
          //cout << "k :" << k << endl;
          if ((nEl % 2) == 1 || !e->KeywordSet(evenIx)) {
            if (nmed == 1)
              res = p0->NewIx(medEl)->Convert2(type, BaseGDL::CONVERT);
            else {
              if (noconv) {
                if (dbl) (*static_cast<DDoubleGDL*> (res))[k] = (*static_cast<DDoubleGDL*> (p0))[medEl];
                else (*static_cast<DFloatGDL*> (res))[k] = (*static_cast<DFloatGDL*> (p0))[medEl];
              } else {
                op1 = p0->NewIx(medEl)->Convert2(type, BaseGDL::CONVERT);
                if (dbl) (*static_cast<DDoubleGDL*> (res))[k] = (*static_cast<DDoubleGDL*> (op1))[0];
                else (*static_cast<DFloatGDL*> (res))[k] = (*static_cast<DFloatGDL*> (op1))[0];
                delete(op1);
              }
            }
          } else {
            if (noconv) {
              if (dbl) (*static_cast<DDoubleGDL*> (res))[k] = .5 * (
                (*static_cast<DDoubleGDL*> (p0))[medEl] +
                (*static_cast<DDoubleGDL*> (p0))[medEl_1]
                );
              else (*static_cast<DFloatGDL*> (res))[k] = .5 * (
                (*static_cast<DFloatGDL*> (p0))[medEl] +
                (*static_cast<DFloatGDL*> (p0))[medEl_1]
                );
            } else {
              op1 = p0->NewIx(medEl)->Convert2(type, BaseGDL::CONVERT);
              op2 = p0->NewIx(medEl_1)->Convert2(type, BaseGDL::CONVERT);
              if (nmed == 1) res = op2->Add(op1)->Div(op3); // TODO: leak with res?
              else {
                if (dbl) (*static_cast<DDoubleGDL*> (res))[k] =
                  (*static_cast<DDoubleGDL*> ((op2->Add(op1)->Div(op3))))[0];
                else (*static_cast<DFloatGDL*> (res))[k] =
                  (*static_cast<DFloatGDL*> ((op2->Add(op1)->Div(op3))))[0];
                delete(op2);
              }
              delete(op1);
            }
          }
        }
      }
      delete(op3);
      delete[] h1;
      delete[] h2;
      delete[] hh;

      return res;
    }

    // begin of the part 2: with "width" param
    if (nParam == 2) {
      // with parameter Width : median filtering with no optimisation,
      //  such as histogram algorithms.
      // Copyright: (C) 2008 by Nicolas Galmiche

      // basic checks on "vector/array" input
      DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>(0);

      if (p0->Rank() > 2)
        e->Throw("Only 1 or 2 dimensions allowed: " + e->GetParString(0));

      // basic checks on "width" input
      DDoubleGDL* p1d = e->GetParAs<DDoubleGDL>(1);

      if (p1d->N_Elements() > 1 || (*p1d)[0] <= 0)
        e->Throw("Width must be a positive scalar or 1 (positive) element array in this context: " + e->GetParString(0));
      DLong MaxAllowedWidth = 0;
      if (p0->Rank() == 1) MaxAllowedWidth = p0->N_Elements();
      if (p0->Rank() == 2) {
        MaxAllowedWidth = p0->Dim(0);
        if (p0->Dim(1) < MaxAllowedWidth) MaxAllowedWidth = p0->Dim(1);
      }
      const int debug = 0;
      if (debug == 1) {
        cout << "X dim " << p0->Dim(0) << endl;
        cout << "y dim " << p0->Dim(1) << endl;
        cout << "MaxAllowedWidth " << MaxAllowedWidth << endl;
      }
      if (!std::isfinite((*p1d)[0]))
        e->Throw("Width must be > 1, and < dimension of array (NaN or Inf)");

      DLongGDL* p1 = e->GetParAs<DLongGDL>(1);

      DDoubleGDL *tamp = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
      DDouble min = ((*p0)[0]);
      DDouble max = min;

      for (SizeT ii = 0; ii < p0->N_Elements(); ++ii) {
        (*tamp)[ii] = (*p0)[ii];
        if ((*p0)[ii] < min) min = ((*p0)[ii]);
        if ((*p0)[ii] > max) max = ((*p0)[ii]);
      }

      //---------------------------- END d'acquisistion des parametres -------------------------------------


      static int evenIx = e->KeywordIx("EVEN");
      static int doubleIx = e->KeywordIx("DOUBLE");
      DStructGDL *Values = SysVar::Values(); //MUST NOT BE STATIC, due to .reset
      DDouble d_nan = (*static_cast<DDoubleGDL*> (Values->GetTag(Values->Desc()->TagIndex("D_NAN"), 0)))[0];
      DDouble d_infinity = (*static_cast<DDoubleGDL*> (Values->GetTag(Values->Desc()->TagIndex("D_INFINITY"), 0)))[0];

      //------------------------------ Init variables and allocation ---------------------------------------
      SizeT width = (*p1)[0];
      SizeT N_MaskElem = width*width;
      SizeT larg = p0->Stride(1);
      SizeT haut = p0->Stride(2) / larg;
      SizeT lim = static_cast<SizeT> (round(width / 2));
      SizeT init = (lim * larg + lim);

      // we don't go further if dimension(s) versus not width OK

      if (debug == 1) {
        cout << "ici" << endl;
      }

      if (p0->Rank() == 1) {
        if (larg < width || width == 1) e->Throw("Width must be > 1, and < width of vector");
      }
      if (p0->Rank() == 2) {
        if (larg < width || haut < width || width == 1) e->Throw("Width must be > 1, and < dimension of array");
      }

      // for 2D arrays, we use the algorithm described in paper
      // from T. Huang, G. Yang, and G. Tang, Fast Two-Dimensional Median Filtering Algorithm,
      // IEEE Trans. Acoust., Speech, Signal Processing,
      // vol. 27, no. 1, pp. 13--18, 1979.

      if ((e->GetParDefined(0)->Type() == GDL_BYTE ||
        e->GetParDefined(0)->Type() == GDL_INT ||
        e->GetParDefined(0)->Type() == GDL_UINT ||
        e->GetParDefined(0)->Type() == GDL_LONG ||
        e->GetParDefined(0)->Type() == GDL_ULONG ||
        e->GetParDefined(0)->Type() == GDL_LONG64 ||
        e->GetParDefined(0)->Type() == GDL_ULONG64) &&
        (haut > 1)) {
        SizeT taille = static_cast<SizeT> (abs(max) - min + 1);
        DDoubleGDL* Histo = new DDoubleGDL(taille, BaseGDL::NOZERO);
        if (width % 2 == 0) {
          for (SizeT i = 0; i < haut - 2 * lim; ++i) {
            SizeT ltmed = 0;
            SizeT med = 0;
            SizeT initial = init + i * larg - lim * larg - lim;
            for (SizeT pp = 0; pp < taille; ++pp)(*Histo)[pp] = 0;
            for (SizeT ii = initial; ii < initial + width; ++ii) {
              for (SizeT yy = 0; yy < width; yy++)
                (*Histo)[static_cast<SizeT> ((*p0)[ii + yy * larg] - min)]++;
            }

            while (ltmed + (*Histo)[med] <= (N_MaskElem / 2)) {
              ltmed += static_cast<SizeT> ((*Histo)[med]);
              ++med;
            }
            if (e->KeywordSet(evenIx)) {

              SizeT EvenMed = med;
              //if ((*Histo)[EvenMed]==1 || (ltmed!=0 && ltmed !=(N_MaskElem /2) -1))
              if ((*Histo)[EvenMed] == 1 || (ltmed != 0 && N_MaskElem / 2 - ltmed != 1)) {
                while ((*Histo)[EvenMed - 1] == 0) {
                  EvenMed--;
                }
                (*tamp)[init + i * larg] = ((med + min)+(EvenMed - 1 + min)) / 2;
              } else
                (*tamp)[init + i * larg] = med + min;
            } else {
              (*tamp)[init + i * larg] = med + min;
            }

            for (SizeT j = init + i * larg + 1; j < init + (i + 1) * larg - 2 * lim; ++j) {
              SizeT initMask = j - lim * larg - lim;
              for (SizeT k = 0; k < 2 * lim; ++k) {
                (*Histo)[static_cast<SizeT> ((*p0)[initMask - 1 + k * larg] - min)]--;
                if ((*p0)[initMask - 1 + k * larg] - min < med)ltmed--;

                (*Histo)[static_cast<SizeT> ((*p0)[initMask + k * larg + 2 * lim - 1] - min)]++;
                if ((*p0)[initMask + k * larg + 2 * lim - 1] - min < med)ltmed++;
              }
              if (ltmed > N_MaskElem / 2) {
                while (ltmed > N_MaskElem / 2) {
                  --med;
                  ltmed -= static_cast<SizeT> ((*Histo)[med]);
                }
              } else {
                while (ltmed + (*Histo)[med] <= (N_MaskElem / 2)) {
                  ltmed += static_cast<SizeT> ((*Histo)[med]);
                  ++med;
                }
              }

              if (e->KeywordSet(evenIx)) {
                SizeT EvenMed = med;
                if ((*Histo)[EvenMed] == 1 || (ltmed != 0 && N_MaskElem / 2 - ltmed != 1)) {
                  while ((*Histo)[EvenMed - 1] == 0) {
                    EvenMed--;
                  }
                  (*tamp)[j] = ((med + min)+(EvenMed - 1 + min)) / 2;
                } else {
                  (*tamp)[j] = med + min;
                }
              } else {
                (*tamp)[j] = med + min;
              }
            }
          }
        } else {
          for (SizeT i = 0; i < haut - 2 * lim; ++i) {
            SizeT ltmed = 0;
            SizeT med = 0;
            SizeT initial = init + i * larg - lim * larg - lim;
            for (SizeT pp = 0; pp < taille; ++pp)(*Histo)[pp] = 0;
            for (SizeT ii = initial; ii < initial + width; ++ii) {
              for (SizeT yy = 0; yy < width; yy++)
                (*Histo)[static_cast<SizeT> ((*p0)[ii + yy * larg] - min)]++;
            }

            while (ltmed + (*Histo)[med] <= (N_MaskElem / 2)) {
              ltmed += static_cast<SizeT> ((*Histo)[med]);
              ++med;
            }
            (*tamp)[init + i * larg] = med + min;

            for (SizeT j = init + i * larg + 1; j < init + (i + 1) * larg - 2 * lim; ++j) {

              SizeT initMask = j - lim * larg - lim;
              for (SizeT k = 0; k <= 2 * lim; ++k) {
                (*Histo)[static_cast<SizeT> ((*p0)[initMask - 1 + k * larg] - min)]--;
                if ((*p0)[initMask - 1 + k * larg] - min < med)ltmed--;

                (*Histo)[static_cast<SizeT> ((*p0)[initMask + k * larg + 2 * lim] - min)]++;
                if ((*p0)[initMask + k * larg + 2 * lim] - min < med)ltmed++;
              }
              if (ltmed > N_MaskElem / 2) {
                while (ltmed > N_MaskElem / 2) {
                  --med;
                  ltmed -= static_cast<SizeT> ((*Histo)[med]);
                }
              } else {
                while (ltmed + (*Histo)[med] <= (N_MaskElem / 2)) {
                  ltmed += static_cast<SizeT> ((*Histo)[med]);
                  ++med;
                }
              }

              (*tamp)[j] = med + min;

            }
          }
        }

      } else {
        DLong* hh;
        DLong* h1;
        DLong* h2;
        DDoubleGDL* Mask, *Mask1D;
        if (p0->Rank() != 1) {
          hh = new DLong[ N_MaskElem];
          h1 = new DLong[ N_MaskElem / 2];
          h2 = new DLong[ (N_MaskElem + 1) / 2];
          Mask = new DDoubleGDL(N_MaskElem, BaseGDL::NOZERO);

          for (DLong i = 0; i < N_MaskElem; ++i) hh[i] = i;
        } else {
          hh = new DLong[ width];
          h1 = new DLong[ width / 2];
          h2 = new DLong[(width + 1) / 2];
          Mask1D = new DDoubleGDL(width, BaseGDL::NOZERO);

          for (DLong i = 0; i < width; ++i) hh[i] = i;
        }

        //-------------------------------- END OF VARIABLES INIT ---------------------------------------------

        //------------------------------ Median Filter Algorithms ---------------------------------------

        if (width % 2 == 0) {
          if (p0->Rank() == 1)//------------------------  For a vector with even width -------------------
          {
            for (SizeT col = lim; col < larg - lim; ++col) {
              SizeT ctl_NaN = 0;
              SizeT kk = 0;
              for (SizeT ind = col - lim; ind < col + lim; ++ind) {
                if ((*p0)[ind] != d_infinity && (*p0)[ind] != -d_infinity && std::isfinite((*p0)[ind]) == 0)
                  ctl_NaN++;
                else {
                  (*Mask1D)[kk] = (*p0)[ind];
                  kk++;
                }
              }
              if (ctl_NaN != 0) {
                if (ctl_NaN == width)(*tamp)[col] = d_nan;
                else {
                  DLong* hhbis = new DLong[ width - ctl_NaN];
                  DLong* h1bis = new DLong[ width - ctl_NaN / 2];
                  DLong* h2bis = new DLong[(width - ctl_NaN + 1) / 2];
                  DDoubleGDL *Mask1Dbis = new DDoubleGDL(width - ctl_NaN, BaseGDL::NOZERO);
                  for (DLong t = 0; t < width - ctl_NaN; ++t) hhbis[t] = t;
                  for (DLong ii = 0; ii < width - ctl_NaN; ++ii)(*Mask1Dbis)[ii] = (*Mask1D)[ii];
                  BaseGDL* besort = static_cast<BaseGDL*> (Mask1Dbis);
                  MergeSortOpt<DLong>(besort, hhbis, h1bis, h2bis, (width - ctl_NaN));
                  if (e->KeywordSet(evenIx)&& (width - ctl_NaN) % 2 == 0)
                    (*tamp)[col] = ((*Mask1Dbis)[hhbis[ (width - ctl_NaN) / 2]]+(*Mask1Dbis
                    )[hhbis [ (width - ctl_NaN - 1) / 2]]) / 2;
                  else
                    (*tamp)[col] = (*Mask1Dbis)[hhbis[ (width - ctl_NaN) / 2]];
                  delete[]hhbis;
                  delete[]h2bis;
                  delete[]h1bis;
                }
              } else {
                BaseGDL* besort = static_cast<BaseGDL*> (Mask1D);
                MergeSortOpt<DLong>(besort, hh, h1, h2, width); // call the sort routine

                if (e->KeywordSet(evenIx))

                  (*tamp)[col] = ((*Mask1D)[hh[ width / 2]]+(*Mask1D)[hh[ (width - 1) / 2]]) / 2;
                else
                  (*tamp)[col] = (*Mask1D)[hh[ width / 2]]; // replace value by Mask median
              }
            }

          } else//------------------------  For an array with even width -------------------
          {
            SizeT jj;
            for (SizeT i = 0; i < haut - 2 * lim; ++i) // lines to replace
            {
              for (SizeT j = init + i * larg; j < init + (i + 1) * larg - 2 * lim; ++j)// elements to replace
              {
                SizeT initMask = j - lim * larg - lim; // left corner of mask
                SizeT kk = 0;
                SizeT ctl_NaN = 0;
                for (SizeT k = 0; k < 2 * lim; ++k) // lines of mask
                {

                  for (jj = initMask + k * larg; jj < (initMask + k * larg) + 2 * lim; ++jj) // elements of mask
                  {
                    if ((*p0)[jj] != d_infinity && (*p0)[jj] != -d_infinity && std::isfinite((*p0)[jj]) == 0)
                      ctl_NaN++;
                    else {
                      (*Mask)[kk] = (*p0)[jj];
                      kk++;
                    }
                  }
                }
                if (ctl_NaN != 0) {
                  if (ctl_NaN == N_MaskElem)(*tamp)[j] = d_nan;
                  else {
                    DLong* hhb = new DLong[ N_MaskElem - ctl_NaN];
                    DLong* h1b = new DLong[ (N_MaskElem - ctl_NaN) / 2];
                    DLong* h2b = new DLong[(N_MaskElem - ctl_NaN + 1) / 2];
                    DDoubleGDL *Maskb = new DDoubleGDL(N_MaskElem - ctl_NaN, BaseGDL::NOZERO);
                    for (DLong t = 0; t < N_MaskElem - ctl_NaN; ++t) hhb[t] = t;
                    for (DLong ii = 0; ii < N_MaskElem - ctl_NaN; ++ii)(*Maskb)[ii] = (*Mask)[ii];
                    BaseGDL* besort = static_cast<BaseGDL*> (Maskb);
                    MergeSortOpt<DLong>(besort, hhb, h1b, h2b, (N_MaskElem - ctl_NaN));
                    if ((N_MaskElem - ctl_NaN) % 2 == 0 && e->KeywordSet(evenIx))
                      (*tamp)[j] = ((*Maskb)[hhb[ (N_MaskElem - ctl_NaN) / 2]]+(*Maskb)[hhb
                      [ (N_MaskElem -
                      ctl_NaN - 1) / 2]]) / 2;
                    else
                      (*tamp)[j] = (*Maskb)[hhb[ (N_MaskElem - ctl_NaN) / 2]];
                    delete[]hhb;
                    delete[]h2b;
                    delete[]h1b;
                  }
                } else {
                  BaseGDL* besort = static_cast<BaseGDL*> (Mask);
                  MergeSortOpt<DLong>(besort, hh, h1, h2, N_MaskElem); // call the sort routine
                  if (e->KeywordSet(evenIx))
                    (*tamp)[j] = ((*Mask)[hh[ N_MaskElem / 2]]+(*Mask)[hh[ (N_MaskElem - 1) / 2]]) / 2;
                  else
                    (*tamp)[j] = (*Mask)[hh[ N_MaskElem / 2]]; // replace value by median Mask one
                }
              }
            }
          }
        } else {
          if (p0->Rank() == 1)//------------------------  For a vector with odd width -------------------
          {
            for (SizeT col = lim; col < larg - lim; ++col) {
              SizeT kk = 0;
              SizeT ctl_NaN = 0;
              for (SizeT ind = col - lim; ind <= col + lim; ++ind) {
                if ((*p0)[ind] != d_infinity && (*p0)[ind] != -d_infinity && std::isfinite((*p0)[ind]) == 0)
                  ctl_NaN++;
                else {
                  (*Mask1D)[kk] = (*p0)[ind];
                  kk++;
                }
              }
              if (ctl_NaN != 0) {
                if (ctl_NaN == width)(*tamp)[col] = d_nan;
                else {
                  DLong* hhbis = new DLong[ width - ctl_NaN];
                  DLong* h1bis = new DLong[ width - ctl_NaN / 2];
                  DLong* h2bis = new DLong[(width - ctl_NaN + 1) / 2];
                  DDoubleGDL *Mask1Dbis = new DDoubleGDL(width - ctl_NaN, BaseGDL::NOZERO);
                  for (DLong t = 0; t < width - ctl_NaN; ++t) hhbis[t] = t;
                  for (DLong ii = 0; ii < width - ctl_NaN; ++ii)(*Mask1Dbis)[ii] = (*Mask1D)[ii];
                  BaseGDL* besort = static_cast<BaseGDL*> (Mask1Dbis);
                  MergeSortOpt<DLong>(besort, hhbis, h1bis, h2bis, (width - ctl_NaN));
                  if (e->KeywordSet(evenIx)&& (width - ctl_NaN) % 2 == 0)
                    (*tamp)[col] = ((*Mask1Dbis)[hhbis[ (width - ctl_NaN) / 2]]+(*Mask1Dbis
                    )[hhbis [ (width - ctl_NaN - 1) / 2]]) / 2;
                  else(*tamp)[col] = (*Mask1Dbis)[hhbis[ (width - ctl_NaN) / 2]];
                  delete[]hhbis;
                  delete[]h2bis;
                  delete[]h1bis;
                }
              } else {
                BaseGDL* besort = static_cast<BaseGDL*> (Mask1D);
                MergeSortOpt<DLong>(besort, hh, h1, h2, width); // call the sort routine
                (*tamp)[col] = (*Mask1D)[hh[ (width) / 2]]; // replace value by Mask median
              }
            }

          } else //-----------------------------  For an array with odd width ---------------------------------
          {
            SizeT jj;
            for (SizeT i = 0; i < haut - 2 * lim; ++i) // lines to replace
            {

              SizeT initial = init + i * larg - lim * larg - lim;
              SizeT dd = 0;
              SizeT ctl_NaN_init = 0;
              for (SizeT yy = 0; yy < width; yy++) {
                for (SizeT ii = initial + yy * larg; ii < initial + yy * larg + width; ++ii) {

                  if ((*p0)[ii] != d_infinity && (*p0)[ii] != -d_infinity && std::isfinite((*p0)[ii]) == 0)
                    ctl_NaN_init++;
                  else
                    (*Mask)[dd] = (*p0)[ii];
                  dd++;
                }
              }
              SizeT kk = 0;

              for (SizeT j = init + i * larg; j < init + (i + 1) * larg - 2 * lim; ++j)// elements to replace
              {
                SizeT initMask = j - lim * larg - lim; // left corner of mask
                SizeT kk = 0;
                SizeT ctl_NaN = 0;
                for (SizeT k = 0; k <= 2 * lim; ++k) // lines of mask
                {

                  for (jj = initMask + k * larg; jj <= (initMask + k * larg) + 2 * lim; ++jj) // elements of mask
                  {
                    if ((*p0)[jj] != d_infinity && (*p0)[jj] != -d_infinity && std::isfinite((*p0)[jj]) == 0)
                      ctl_NaN++;

                    else {
                      (*Mask)[kk] = (*p0)[jj];
                      kk++;
                    }
                  }

                }

                if (ctl_NaN != 0) {
                  if (ctl_NaN == N_MaskElem)
                    (*tamp)[j] = d_nan;
                  else {
                    DLong* hhb = new DLong[ N_MaskElem - ctl_NaN];
                    DLong* h1b = new DLong[ (N_MaskElem - ctl_NaN) / 2];
                    DLong* h2b = new DLong[(N_MaskElem - ctl_NaN + 1) / 2];
                    DDoubleGDL*Maskb = new DDoubleGDL(N_MaskElem - ctl_NaN, BaseGDL::NOZERO);
                    for (DLong t = 0; t < N_MaskElem - ctl_NaN; ++t) hhb[t] = t;
                    for (DLong ii = 0; ii < N_MaskElem - ctl_NaN; ++ii)(*Maskb)[ii] = (*Mask)[ii];
                    BaseGDL* besort = static_cast<BaseGDL*> (Maskb);
                    MergeSortOpt<DLong>(besort, hhb, h1b, h2b, (N_MaskElem - ctl_NaN));
                    if ((N_MaskElem - ctl_NaN) % 2 == 0 && e->KeywordSet(evenIx))
                      (*tamp)[j] = ((*Maskb)[hhb[ (N_MaskElem - ctl_NaN) / 2]]+(*Maskb)[hhb
                      [ (N_MaskElem -
                      ctl_NaN - 1) / 2]]) / 2;
                    else(*tamp)[j] = (*Maskb)[hhb[(N_MaskElem - ctl_NaN) / 2]];
                    delete[]hhb;
                    delete[]h2b;
                    delete[]h1b;
                  }
                } else {
                  BaseGDL* besort = static_cast<BaseGDL*> (Mask);
                  MergeSortOpt<DLong>(besort, hh, h1, h2, N_MaskElem); // call the sort routine
                  (*tamp)[j] = (*Mask)[hh[ (N_MaskElem) / 2]]; // replace value by Mask median
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
      if (e->GetParDefined(0)->Type() == GDL_DOUBLE || p0->Type() == GDL_COMPLEXDBL || e->KeywordSet(doubleIx))
        return tamp;
      else if (e->GetParDefined(0)->Type() == GDL_BYTE)
        return tamp->Convert2(GDL_BYTE, BaseGDL::CONVERT);

      return tamp->Convert2(GDL_FLOAT, BaseGDL::CONVERT);

    }// end if
    e->Throw("More than 2 parameters not handled.");
    return NULL;

  }// end of median

  template <typename Ty> static inline Ty do_mean(const Ty* data, const SizeT sz) {
    Ty mean = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) mean += data[i];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:mean)
        for (OMPInt i = 0; i < sz; ++i) mean += data[i];
    }
    return mean / sz;
  }

  template <typename Ty, typename T2> static inline Ty do_mean_cpx(const Ty* data, const SizeT sz) {
    T2 meanr = 0;
    T2 meani = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        meanr += data[i].real();
        meani += data[i].imag();
      }
    } else {

      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:meanr,meani)
      for (OMPInt i = 0; i < sz; ++i) {
        meanr += data[i].real();
        meani += data[i].imag();
      }
    }
    return std::complex<T2>(meanr / sz, meani / sz);
  }

  template <typename Ty> static inline Ty do_mean_nan(const Ty* data, const SizeT sz) {
    Ty mean = 0;
    SizeT n = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty v = data[i];
        if (std::isfinite(v)) {
            n++;
            mean += v;
        }
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:mean,n)
      for (OMPInt i = 0; i < sz; ++i) {
        Ty v = data[i];
        if (std::isfinite(v)) {
            n++;
            mean += v;
        }
      }
    }
    return mean / n;
  }

  template <typename Ty, typename T2> static inline Ty do_mean_cpx_nan(const Ty* data, const SizeT sz) {
    T2 meanr = 0;
    T2 meani = 0;
    SizeT nr = 0;
    SizeT ni = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        T2 v = data[i].real();
        if (std::isfinite(v)) {
          nr++;
          meanr += v;
        }
        v = data[i].imag();
        if (std::isfinite(v)) {
          ni++;
          meani += v;
        }
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:meanr,nr,meani,ni)
        for (OMPInt i = 0; i < sz; ++i) {
        T2 v = data[i].real();
        if (std::isfinite(v)) {
          nr++;
          meanr += v;
        }
        v = data[i].imag();
        if (std::isfinite(v)) {
          ni++;
          meani += v;
        }
      }
    }
    return std::complex<T2>(meanr / nr, meani / ni);
  }

  BaseGDL* mean_fun(EnvT* e) {
    BaseGDL* p0 = e->GetParDefined(0);

    if (p0->Type() == GDL_PTR)
      e->Throw("Pointer expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_OBJ)
      e->Throw("Object expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_STRUCT)
      e->Throw("Struct expression not allowed in this context: " + e->GetParString(0));

    static int doubleIx = e->KeywordIx("DOUBLE");
    bool dbl =
      (p0->Type() == GDL_DOUBLE ||
      p0->Type() == GDL_COMPLEXDBL ||
      e->KeywordSet(doubleIx));

    static int nanIx = e->KeywordIx("NAN");
    // Check possibility of Nan (not useful to speed down mean on integer data which
    // will never produce NaNs).
    bool possibleNaN = (p0->Type() == GDL_DOUBLE ||
      p0->Type() == GDL_FLOAT ||
      p0->Type() == GDL_COMPLEX ||
      p0->Type() == GDL_COMPLEXDBL);
    bool omitNaN = (e->KeywordPresent(nanIx) && possibleNaN);

    //DIMENSION Kw
    static int dimIx = e->KeywordIx("DIMENSION");
    bool dimSet = e->KeywordSet(dimIx);

    DLong meanDim;
    if (dimSet) {
      e->AssureLongScalarKW(dimIx, meanDim);
      if (meanDim < 0 || meanDim > p0->Rank())
        e->Throw("Illegal keyword value for DIMENSION");
    }

    if (dimSet && p0->Rank() > 1) {
      meanDim -= 1; // user-supplied dimensions start with 1!

      // output dimension: copy srcDim to destDim
      dimension destDim = p0->Dim();
      // make array of dims for transpose
      DUInt* perm = new DUInt[p0->Rank()];
      ArrayGuard<DUInt> perm_guard(perm);
      //useful to reorder dims for transpose to order data in continuous order.
      DUInt i = 0, j = 0;
      for (i = 0; i < p0->Rank(); ++i) if (i != meanDim) {
          perm[j + 1] = i;
          j++;
        }
      perm[0] = meanDim;
      // resize destDim
      destDim.Remove(meanDim); //will be one dimension less
      //compute stride and number of elements of result:
      SizeT stride = p0->Dim(meanDim);

      SizeT nEl = destDim.NDimElementsConst();

      //transpose p0 to arrange dimensions if meanDim is > 0. Do not forget to remove transposed array.
      bool clean_array = false;
      if (p0->Type() == GDL_COMPLEXDBL || (p0->Type() == GDL_COMPLEX && dbl)) {
        DComplexDblGDL* input = e->GetParAs<DComplexDblGDL>(0);
        if (meanDim != 0) {
          input = static_cast<DComplexDblGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
          clean_array = true;
        }
        DComplexDblGDL* res = new DComplexDblGDL(destDim, BaseGDL::NOZERO);
        if (omitNaN) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean_cpx_nan<DComplexDbl, double>(&(*input)[i * stride], stride);
        } else {
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean_cpx<DComplexDbl, double>(&(*input)[i * stride], stride);
        }
        if (clean_array) delete input;
        return res;
      } else if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* input = e->GetParAs<DComplexGDL>(0);
        if (meanDim != 0) {
          input = static_cast<DComplexGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
          clean_array = true;
        }
        DComplexGDL* res = new DComplexGDL(destDim, BaseGDL::NOZERO);
        if (omitNaN) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean_cpx_nan<DComplex, float>(&(*input)[i * stride], stride);
        } else {
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean_cpx<DComplex, float>(&(*input)[i * stride], stride);
        }
        if (clean_array) delete input;
        return res;
      } else {
        if (dbl) {
          DDoubleGDL* input = e->GetParAs<DDoubleGDL>(0);
          if (meanDim != 0) {
            input = static_cast<DDoubleGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
            clean_array = true;
          }
          DDoubleGDL* res = new DDoubleGDL(destDim, BaseGDL::NOZERO);
          if (omitNaN) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean_nan(&(*input)[i * stride], stride);
          } else {
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean(&(*input)[i * stride], stride);
          }
          if (clean_array) delete input;
          return res;
        } else {
          DFloatGDL* input = e->GetParAs<DFloatGDL>(0);

          if (meanDim != 0) {
            input = static_cast<DFloatGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
            clean_array = true;
          }
          DFloatGDL* res = new DFloatGDL(destDim, BaseGDL::NOZERO);
          if (omitNaN) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean_nan(&(*input)[i * stride], stride);
          } else {
            for (SizeT i = 0; i < nEl; ++i) (*res)[i] = do_mean(&(*input)[i * stride], stride);
          }
          if (clean_array) delete input;
          return res;
        }
      }
    } else {
      if (p0->Type() == GDL_COMPLEXDBL || (p0->Type() == GDL_COMPLEX && dbl)) {
        DComplexDblGDL* input = e->GetParAs<DComplexDblGDL>(0);
        if (omitNaN) return new DComplexDblGDL(do_mean_cpx_nan<DComplexDbl, double>(&(*input)[0], input->N_Elements()));
        else return new DComplexDblGDL(do_mean_cpx<DComplexDbl, double>(&(*input)[0], input->N_Elements()));
      } else if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* input = e->GetParAs<DComplexGDL>(0);
        if (omitNaN) return new DComplexGDL(do_mean_cpx_nan<DComplex, float>(&(*input)[0], input->N_Elements()));
        else return new DComplexGDL(do_mean_cpx<DComplex, float>(&(*input)[0], input->N_Elements()));
      } else {
        if (dbl) {
          DDoubleGDL* input = e->GetParAs<DDoubleGDL>(0);
          if (omitNaN) return new DDoubleGDL(do_mean_nan(&(*input)[0], input->N_Elements()));
          else return new DDoubleGDL(do_mean(&(*input)[0], input->N_Elements()));
        } else {
          DFloatGDL* input = e->GetParAs<DFloatGDL>(0);
          if (omitNaN) return new DFloatGDL(do_mean_nan(&(*input)[0], input->N_Elements()));
          else return new DFloatGDL(do_mean(&(*input)[0], input->N_Elements()));
        }
      }
    }
  }

  template<typename Ty>
  static inline void do_moment(const Ty* data, const SizeT sz, Ty &mean, Ty &variance, Ty &skewness,
    Ty &kurtosis, Ty &mdev, Ty &sdev, const int maxmoment) {
    Ty meanl = do_mean(data, sz);
    mean = meanl;
    if (maxmoment == 1) {
      variance = skewness = kurtosis = mdev = sdev = std::numeric_limits<float>::quiet_NaN();
      return;
    }

    Ty var = 0;
    Ty md = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        var += cdata*cdata;
        md += fabs(cdata);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:var,md)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        var += cdata*cdata;
        md += fabs(cdata);
      }
    }
    var /= (sz - 1);
    variance = var;
    sdev = sqrt(var);
    mdev = md / sz;

    if (maxmoment == 2 || var == 0) {
      skewness = kurtosis = std::numeric_limits<float>::quiet_NaN();
      return;
    }
    Ty skew = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        skew += (cdata * cdata * cdata) / (var * sdev);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:skew)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        skew += (cdata * cdata * cdata) / (var * sdev);
      }
    }
    skewness = skew / sz;
    if (maxmoment == 3) {
      kurtosis = std::numeric_limits<float>::quiet_NaN();
      return;
    }
    Ty kurt = 0;
    if ((GDL_NTHREADS=parallelize(sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        kurt += (cdata * cdata * cdata * cdata) / (var * var);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:kurt)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        kurt += (cdata * cdata * cdata * cdata) / (var * var);
      }
    }
    kurtosis = (kurt / sz) - 3;
  }

  template<typename Ty, typename T2>
  static inline void do_moment_cpx(const Ty* data, const SizeT sz, Ty &mean, Ty &variance, Ty &skewness,
    Ty &kurtosis, T2 &mdev, Ty &sdev, const int maxmoment) {
    Ty meanl = do_mean_cpx<Ty, T2>(data, sz);
    mean = meanl;
    if (maxmoment == 1) {
      variance = skewness = kurtosis = sdev =
        std::complex<T2>(std::numeric_limits<T2>::quiet_NaN(), std::numeric_limits<T2>::quiet_NaN());
      mdev = std::numeric_limits<T2>::quiet_NaN();
      return;
    }

    T2 mdr = 0;
    T2 varr = 0;
    T2 vari = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        varr += (cdatar * cdatar)-(cdatai * cdatai);
        vari += 2 * cdatar*cdatai;
        mdr += sqrt(cdatar * cdatar + cdatai * cdatai);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:varr,vari,mdr)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        varr += (cdatar * cdatar)-(cdatai * cdatai);
        vari += 2 * cdatar*cdatai;
        mdr += sqrt(cdatar * cdatar + cdatai * cdatai);
      }
    }
    varr /= (sz - 1);
    vari /= (sz - 1);
    mdr /= sz;
    variance = std::complex<T2>(varr, vari);
    sdev = sqrt(variance);
    mdev = mdr;

    if (maxmoment == 2) {
      skewness = kurtosis =
        std::complex<T2>(std::numeric_limits<T2>::quiet_NaN(), std::numeric_limits<T2>::quiet_NaN());
      return;
    }
    T2 skewr = 0;
    T2 skewi = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        skewr += (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) *
          cos(0.15E1 * atan2(vari, varr))+(3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));

        skewi += (3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * cos(1.5 * atan2(vari, varr))-
          (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:skewr,skewi)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        skewr += (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) *
          cos(0.15E1 * atan2(vari, varr))+(3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));

        skewi += (3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * cos(1.5 * atan2(vari, varr))-
          (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));
      }
    }
    skewness = std::complex<T2>(skewr / sz, skewi / sz);
    if (maxmoment == 3) {
      kurtosis = std::complex<T2>(std::numeric_limits<T2>::quiet_NaN(), std::numeric_limits<T2>::quiet_NaN());
      return;
    }
    T2 kurtr = 0;
    T2 kurti = 0;
    if ((GDL_NTHREADS=parallelize(sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        kurtr += (cdatar * cdatar * cdatar * cdatar - 6.0 * cdatar * cdatar * cdatai * cdatai + cdatai *
          cdatai * cdatai * cdatai)*(varr * varr - vari * vari) / (pow(varr * varr - vari*vari, 2.0) +
          4.0 * varr * varr * vari * vari) + 2.0 * (4.0 * cdatar * cdatar * cdatar * cdatai -
          4.0 * cdatar * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari*vari, 2.0) +
          4.0 * varr * varr * vari * vari);
        kurti += (4.0 * cdatar * cdatar * cdatar * cdatai - 4.0 * cdatar * cdatai * cdatai * cdatai)*
          (varr * varr - vari * vari) / (pow(varr * varr - vari*vari, 2.0) + 4.0 * varr * varr * vari * vari) -
          2.0 * (cdatar * cdatar * cdatar * cdatar -
          6.0 * cdatar * cdatar * cdatai * cdatai + cdatai * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari*vari, 2.0) +
          4.0 * varr * varr * vari * vari);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:kurtr,kurti)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        kurtr += (cdatar * cdatar * cdatar * cdatar - 6.0 * cdatar * cdatar * cdatai * cdatai + cdatai *
          cdatai * cdatai * cdatai)*(varr * varr - vari * vari) / (pow(varr * varr - vari*vari, 2.0) +
          4.0 * varr * varr * vari * vari) + 2.0 * (4.0 * cdatar * cdatar * cdatar * cdatai -
          4.0 * cdatar * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari*vari, 2.0) +
          4.0 * varr * varr * vari * vari);
        kurti += (4.0 * cdatar * cdatar * cdatar * cdatai - 4.0 * cdatar * cdatai * cdatai * cdatai)*
          (varr * varr - vari * vari) / (pow(varr * varr - vari*vari, 2.0) + 4.0 * varr * varr * vari * vari) -
          2.0 * (cdatar * cdatar * cdatar * cdatar -
          6.0 * cdatar * cdatar * cdatai * cdatai + cdatai * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari*vari, 2.0) +
          4.0 * varr * varr * vari * vari);
      }
    }
    kurtosis = std::complex<T2>((kurtr / sz) - 3, (kurti / sz) - 3);
  }

  template<typename Ty>
  static inline void do_moment_nan(const Ty* data, const SizeT sz, Ty &mean, Ty &variance, Ty &skewness,
    Ty &kurtosis, Ty &mdev, Ty &sdev, const int maxmoment) {
    Ty meanl = do_mean_nan(data, sz);
    mean = meanl;
    if (maxmoment == 1 || !std::isfinite(mean)) {
      variance = skewness = kurtosis = mdev = sdev = std::numeric_limits<float>::quiet_NaN();
      return;
    }

    Ty var = 0;
    Ty md = 0;
    SizeT k = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        if (std::isfinite(cdata)) {
          var += cdata*cdata;
          md += fabs(cdata);
          k += 1;
        }
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:var,md,k)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        if (std::isfinite(cdata)) {
          var += cdata*cdata;
          md += fabs(cdata);
          k += 1;
        }
      }
    }
    if (k > 1) var /= (k - 1);
    else {
      variance = skewness = kurtosis = mdev = sdev = std::numeric_limits<float>::quiet_NaN();
      return;
    }
    variance = var;
    sdev = sqrt(var);
    mdev = md / k;
    if (maxmoment == 2 || var == 0) {
      skewness = kurtosis = std::numeric_limits<float>::quiet_NaN();
      return;
    }
    Ty skew = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        if (std::isfinite(cdata)) skew += (cdata * cdata * cdata) / (var * sdev);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:skew)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        if (std::isfinite(cdata)) skew += (cdata * cdata * cdata) / (var * sdev);
      }
    }
    skewness = skew / k;
    if (maxmoment == 3) {
      kurtosis = std::numeric_limits<float>::quiet_NaN();
      return;
    }
    Ty kurt = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        if (std::isfinite(cdata)) kurt += (cdata * cdata * cdata * cdata) / (var * var);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:kurt)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        if (std::isfinite(cdata)) kurt += (cdata * cdata * cdata * cdata) / (var * var);
      }
    }
    kurtosis = (kurt / k) - 3;
  }

  template<typename Ty, typename T2>
  static inline void do_moment_cpx_nan(const Ty* data, const SizeT sz, Ty &mean, Ty &variance, Ty &skewness,
    Ty &kurtosis, T2 &mdev, Ty &sdev, const int maxmoment) {
    Ty meanl = do_mean_cpx_nan<Ty, T2>(data, sz);
    mean = meanl;
    if (maxmoment == 1) {
      variance = skewness = kurtosis = sdev =
        std::complex<T2>(std::numeric_limits<T2>::quiet_NaN(), std::numeric_limits<T2>::quiet_NaN());
      mdev = std::numeric_limits<T2>::quiet_NaN();
      return;
    }
    SizeT kr = 0;
    SizeT ki = 0;
    T2 mdr = 0;
    T2 varr = 0;
    T2 vari = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        if (std::isfinite(cdatar)) {
          varr += cdatar*cdatar;
          kr++;
        }
        if (std::isfinite(cdatai)) {
          vari += cdatai*cdatai;
          ki++;
        }
        if (std::isfinite(cdatar)) mdr += sqrt(cdatar * cdatar + cdatai * cdatai);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:varr,vari,mdr,kr,ki)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        if (std::isfinite(cdatar)) {
          varr += cdatar*cdatar;
          kr++;
        }
        if (std::isfinite(cdatai)) {
          vari += cdatai*cdatai;
          ki++;
        }
        if (std::isfinite(cdatar)) mdr += sqrt(cdatar * cdatar + cdatai * cdatai);
      }
    }
    varr /= (kr - 1);
    vari /= (ki - 1);
    mdr /= kr;
    variance = std::complex<T2>(varr, vari);
    sdev = sqrt(variance);
    mdev = mdr;

    if (maxmoment == 2) {
      skewness = kurtosis =
        std::complex<T2>(std::numeric_limits<T2>::quiet_NaN(), std::numeric_limits<T2>::quiet_NaN());
      return;
    }
    T2 skewr = 0;
    T2 skewi = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        if (std::isfinite(cdatar)) skewr += (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) *
          cos(0.15E1 * atan2(vari, varr))+(3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));

        if (std::isfinite(cdatai)) skewi += (3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * cos(1.5 * atan2(vari, varr))-
          (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:skewr,skewi)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        if (std::isfinite(cdatar)) skewr += (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) *
          cos(0.15E1 * atan2(vari, varr))+(3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));

        if (std::isfinite(cdatai)) skewi += (3.0 * cdatar * cdatar * cdatai - cdatai * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * cos(1.5 * atan2(vari, varr))-
          (cdatar * cdatar * cdatar - 3.0 * cdatar * cdatai * cdatai) *
          exp(-0.75 * log(varr * varr + vari * vari)) * sin(1.5 * atan2(vari, varr));
      }
    }
    skewness = std::complex<T2>(skewr / kr, skewi / ki);
    if (maxmoment == 3) {
      kurtosis = std::complex<T2>(std::numeric_limits<T2>::quiet_NaN(), std::numeric_limits<T2>::quiet_NaN());
      return;
    }
    T2 kurtr = 0;
    T2 kurti = 0;
    if ((GDL_NTHREADS=parallelize( sz))==1) {
      for (SizeT i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        if (std::isfinite(cdatar)) kurtr += (cdatar * cdatar * cdatar * cdatar - 6.0 * cdatar * cdatar * cdatai * cdatai + cdatai *
          cdatai * cdatai * cdatai)*(varr * varr - vari * vari) / (pow(varr * varr - vari * vari, 2.0) +
          4.0 * varr * varr * vari * vari) + 2.0 * (4.0 * cdatar * cdatar * cdatar * cdatai -
          4.0 * cdatar * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari * vari, 2.0) +
          4.0 * varr * varr * vari * vari);
        if (std::isfinite(cdatai)) kurti += (4.0 * cdatar * cdatar * cdatar * cdatai - 4.0 * cdatar * cdatai * cdatai * cdatai)*
          (varr * varr - vari * vari) / (pow(varr * varr - vari * vari, 2.0) + 4.0 * varr * varr * vari * vari) -
          2.0 * (cdatar * cdatar * cdatar * cdatar -
          6.0 * cdatar * cdatar * cdatai * cdatai + cdatai * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari * vari, 2.0) +
          4.0 * varr * varr * vari * vari);
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) reduction(+:kurtr,kurti)
        for (OMPInt i = 0; i < sz; ++i) {
        Ty cdata = data[i] - meanl;
        T2 cdatar = cdata.real();
        T2 cdatai = cdata.imag();
        if (std::isfinite(cdatar)) kurtr += (cdatar * cdatar * cdatar * cdatar - 6.0 * cdatar * cdatar * cdatai * cdatai + cdatai *
          cdatai * cdatai * cdatai)*(varr * varr - vari * vari) / (pow(varr * varr - vari * vari, 2.0) +
          4.0 * varr * varr * vari * vari) + 2.0 * (4.0 * cdatar * cdatar * cdatar * cdatai -
          4.0 * cdatar * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari * vari, 2.0) +
          4.0 * varr * varr * vari * vari);
        if (std::isfinite(cdatai)) kurti += (4.0 * cdatar * cdatar * cdatar * cdatai - 4.0 * cdatar * cdatai * cdatai * cdatai)*
          (varr * varr - vari * vari) / (pow(varr * varr - vari * vari, 2.0) + 4.0 * varr * varr * vari * vari) -
          2.0 * (cdatar * cdatar * cdatar * cdatar -
          6.0 * cdatar * cdatar * cdatai * cdatai + cdatai * cdatai * cdatai * cdatai) * varr * vari /
          (pow(varr * varr - vari * vari, 2.0) +
          4.0 * varr * varr * vari * vari);
      }
    }
    kurtosis = std::complex<T2>((kurtr / kr) - 3, (kurti / kr) - 3);
  }

  BaseGDL* moment_fun(EnvT* e) {
    BaseGDL* p0 = e->GetParDefined(0);

    if (p0->Type() == GDL_PTR)
      e->Throw("Pointer expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_OBJ)
      e->Throw("Object expression not allowed in this context: " + e->GetParString(0));
    if (p0->Type() == GDL_STRUCT)
      e->Throw("Struct expression not allowed in this context: " + e->GetParString(0));

    static int doubleIx = e->KeywordIx("DOUBLE");
    bool dbl =
      (p0->Type() == GDL_DOUBLE ||
      p0->Type() == GDL_COMPLEXDBL ||
      e->KeywordSet(doubleIx));

    static int nanIx = e->KeywordIx("NAN");
    // Check possibility of Nan (not useful to speed down moment on integer data which
    // will never produce NaNs).
    bool possibleNaN = (p0->Type() == GDL_DOUBLE ||
      p0->Type() == GDL_FLOAT ||
      p0->Type() == GDL_COMPLEX ||
      p0->Type() == GDL_COMPLEXDBL);
    bool omitNaN = (e->KeywordPresent(nanIx) && possibleNaN);

    //DIMENSION Kw
    static int dimIx = e->KeywordIx("DIMENSION");
    bool dimSet = e->KeywordSet(dimIx);

    //MAXMOMENT Kw. It limits the computation, even if a modifying kw of higher moment, such as "kurtosis" is present

    static int maxmIx = e->KeywordIx("MAXMOMENT");
    DLong maxmoment = 4;
    if (e->KeywordPresent(maxmIx)) e->AssureLongScalarKW(maxmIx, maxmoment);
    if (maxmoment > 4) maxmoment = 4;
    if (maxmoment < 1) maxmoment = 4;

    //MEAN Kw
    static int meanIx = e->KeywordIx("MEAN");
    int domean = e->KeywordPresent(meanIx);
    //KURTOSIS Kw
    static int kurtIx = e->KeywordIx("KURTOSIS");
    int dokurt = e->KeywordPresent(kurtIx);
    //SDEV Kw
    static int sdevIx = e->KeywordIx("SDEV");
    int dosdev = e->KeywordPresent(sdevIx);
    //MDEV Kw
    static int mdevIx = e->KeywordIx("MDEV");
    int domdev = e->KeywordPresent(mdevIx);
    //VARIANCE Kw
    static int varIx = e->KeywordIx("VARIANCE");
    int dovar = e->KeywordPresent(varIx);
    //SKEWNESS Kw
    static int skewIx = e->KeywordIx("SKEWNESS");
    int doskew = e->KeywordPresent(skewIx);


    DLong momentDim;
    if (dimSet) {
      e->AssureLongScalarKW(dimIx, momentDim);
      if (momentDim < 0 || momentDim > p0->Rank())
        e->Throw("Illegal keyword value for DIMENSION");
    }

    if (dimSet && p0->Rank() > 1) {
      momentDim -= 1; // user-supplied dimensions start with 1!

      // output dimension: copy srcDim to destDim
      dimension destDim = p0->Dim();
      dimension auxiliaryDim;
      // make array of dims for transpose
      DUInt* perm = new DUInt[p0->Rank()];
      ArrayGuard<DUInt> perm_guard(perm);
      //useful to reorder dims for transpose to order data in continuous order.
      DUInt i = 0, j = 0;
      for (i = 0; i < p0->Rank(); ++i) if (i != momentDim) {
          perm[j + 1] = i;
          j++;
        }
      perm[0] = momentDim;
      // resize destDim
      destDim.Remove(momentDim); //will be one dimension less
      SizeT nEl = destDim.NDimElementsConst(); //need to compute that here, before adding last dim.
      auxiliaryDim = destDim;

      destDim << 4; //add 4 as last dim
      //compute stride and number of elements of result:
      SizeT stride = p0->Dim(momentDim);


      //transpose p0 to arrange dimensions if momentDim is > 0. Do not forget to remove transposed array.
      bool clean_array = false;
      if (p0->Type() == GDL_COMPLEXDBL || (p0->Type() == GDL_COMPLEX && dbl)) {
        DComplexDblGDL* input = e->GetParAs<DComplexDblGDL>(0);
        if (momentDim != 0) {
          input = static_cast<DComplexDblGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
          clean_array = true;
        }
        DComplexDblGDL* res = new DComplexDblGDL(destDim, BaseGDL::NOZERO);
        DComplexDblGDL* mean;
        DComplexDblGDL* var;
        DComplexDblGDL* skew;
        DComplexDblGDL* kurt;
        DComplexDblGDL* sdev;
        DDoubleGDL* mdev;
        if (domean) mean = new DComplexDblGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (dovar) var = new DComplexDblGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (doskew) skew = new DComplexDblGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (dokurt) kurt = new DComplexDblGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (dosdev) sdev = new DComplexDblGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (domdev) mdev = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (omitNaN) {
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) {
              DDouble mdevl;
              DComplexDbl sdevl;
              do_moment_cpx_nan<DComplexDbl, double>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < nEl; ++i) {
              DDouble mdevl;
              DComplexDbl sdevl;
              do_moment_cpx_nan<DComplexDbl, double>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          }
        } else {
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) {
              DDouble mdevl;
              DComplexDbl sdevl;
              do_moment_cpx<DComplexDbl, double>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < nEl; ++i) {
              DDouble mdevl;
              DComplexDbl sdevl;
              do_moment_cpx<DComplexDbl, double>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          }
        }
        if (clean_array) delete input;
        if (domean) e->SetKW(meanIx, mean);
        if (dovar) e->SetKW(varIx, var);
        if (doskew) e->SetKW(skewIx, skew);
        if (dokurt) e->SetKW(kurtIx, kurt);
        if (dosdev) e->SetKW(sdevIx, sdev);
        if (domdev) e->SetKW(mdevIx, mdev);
        return res;
      } else if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* input = e->GetParAs<DComplexGDL>(0);
        if (momentDim != 0) {
          input = static_cast<DComplexGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
          clean_array = true;
        }
        DComplexGDL* res = new DComplexGDL(destDim, BaseGDL::NOZERO);
        DComplexGDL* mean;
        DComplexGDL* var;
        DComplexGDL* skew;
        DComplexGDL* kurt;
        DComplexGDL* sdev;
        DFloatGDL* mdev;
        if (domean) mean = new DComplexGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (dovar) var = new DComplexGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (doskew) skew = new DComplexGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (dokurt) kurt = new DComplexGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (dosdev) sdev = new DComplexGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (domdev) mdev = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
        if (omitNaN) {
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) {
              DFloat mdevl;
              DComplex sdevl;
              do_moment_cpx_nan<DComplex, float>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < nEl; ++i) {
              DFloat mdevl;
              DComplex sdevl;
              do_moment_cpx_nan<DComplex, float>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          }
        } else {
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) {
              DFloat mdevl;
              DComplex sdevl;
              do_moment_cpx<DComplex, float>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (OMPInt i = 0; i < nEl; ++i) {
              DFloat mdevl;
              DComplex sdevl;
              do_moment_cpx<DComplex, float>(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl], (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
              if (domean) (*mean)[i] = (*res)[i];
              if (dovar) (*var)[i] = (*res)[i + nEl];
              if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
              if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
              if (dosdev) (*sdev)[i] = sdevl;
              if (domdev) (*mdev)[i] = mdevl;
            }
          }
        }
        if (clean_array) delete input;
        if (domean) e->SetKW(meanIx, mean);
        if (dovar) e->SetKW(varIx, var);
        if (doskew) e->SetKW(skewIx, skew);
        if (dokurt) e->SetKW(kurtIx, kurt);
        if (dosdev) e->SetKW(sdevIx, sdev);
        if (domdev) e->SetKW(mdevIx, mdev);
        return res;
      } else {
        if (dbl) {
          DDoubleGDL* input = e->GetParAs<DDoubleGDL>(0);
          if (momentDim != 0) {
            input = static_cast<DDoubleGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
            clean_array = true;
          }
          DDoubleGDL* res = new DDoubleGDL(destDim, BaseGDL::NOZERO);
          DDoubleGDL* mean;
          DDoubleGDL* var;
          DDoubleGDL* skew;
          DDoubleGDL* kurt;
          DDoubleGDL* sdev;
          DDoubleGDL* mdev;
          if (domean) mean = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (dovar) var = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (doskew) skew = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (dokurt) kurt = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (dosdev) sdev = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (domdev) mdev = new DDoubleGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (omitNaN) {
            if ((GDL_NTHREADS=parallelize( nEl))==1) {
              for (SizeT i = 0; i < nEl; ++i) {
                DDouble mdevl;
                DDouble sdevl;
                do_moment_nan(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            } else {
              TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
                for (OMPInt i = 0; i < nEl; ++i) {
                DDouble mdevl;
                DDouble sdevl;
                do_moment_nan(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            }
          } else {
            if ((GDL_NTHREADS=parallelize( nEl))==1) {
              for (SizeT i = 0; i < nEl; ++i) {
                DDouble mdevl;
                DDouble sdevl;
                do_moment(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            } else {
              TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
                for (OMPInt i = 0; i < nEl; ++i) {
                DDouble mdevl;
                DDouble sdevl;
                do_moment(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            }
          }
          if (clean_array) delete input;
          if (domean) e->SetKW(meanIx, mean);
          if (dovar) e->SetKW(varIx, var);
          if (doskew) e->SetKW(skewIx, skew);
          if (dokurt) e->SetKW(kurtIx, kurt);
          if (dosdev) e->SetKW(sdevIx, sdev);
          if (domdev) e->SetKW(mdevIx, mdev);
          return res;
        } else {
          DFloatGDL* input = e->GetParAs<DFloatGDL>(0);
          if (momentDim != 0) {
            input = static_cast<DFloatGDL*> (static_cast<BaseGDL*> (input)->Transpose(perm));
            clean_array = true;
          }
          DFloatGDL* res = new DFloatGDL(destDim, BaseGDL::NOZERO);
          DFloatGDL* mean;
          DFloatGDL* var;
          DFloatGDL* skew;
          DFloatGDL* kurt;
          DFloatGDL* sdev;
          DFloatGDL* mdev;
          if (domean) mean = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (dovar) var = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (doskew) skew = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (dokurt) kurt = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (dosdev) sdev = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (domdev) mdev = new DFloatGDL(auxiliaryDim, BaseGDL::NOZERO);
          if (omitNaN) {
            if ((GDL_NTHREADS=parallelize( nEl))==1) {
              for (SizeT i = 0; i < nEl; ++i) {
                DFloat mdevl;
                DFloat sdevl;
                do_moment_nan(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            } else {
              TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
                for (OMPInt i = 0; i < nEl; ++i) {
                DFloat mdevl;
                DFloat sdevl;
                do_moment_nan(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            }
          } else {
            if ((GDL_NTHREADS=parallelize( nEl))==1) {
              for (SizeT i = 0; i < nEl; ++i) {
                DFloat mdevl;
                DFloat sdevl;
                do_moment(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            } else {
              TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
                for (OMPInt i = 0; i < nEl; ++i) {
                DFloat mdevl;
                DFloat sdevl;
                do_moment(&(*input)[i * stride], stride, (*res)[i], (*res)[i + nEl],
                  (*res)[i + 2 * nEl], (*res)[i + 3 * nEl], mdevl, sdevl, maxmoment);
                if (domean) (*mean)[i] = (*res)[i];
                if (dovar) (*var)[i] = (*res)[i + nEl];
                if (doskew) (*skew)[i] = (*res)[i + 2 * nEl];
                if (dokurt) (*kurt)[i] = (*res)[i + 3 * nEl];
                if (dosdev) (*sdev)[i] = sdevl;
                if (domdev) (*mdev)[i] = mdevl;
              }
            }
          }
          if (clean_array) delete input;
          if (domean) e->SetKW(meanIx, mean);
          if (dovar) e->SetKW(varIx, var);
          if (doskew) e->SetKW(skewIx, skew);
          if (dokurt) e->SetKW(kurtIx, kurt);
          if (dosdev) e->SetKW(sdevIx, sdev);
          if (domdev) e->SetKW(mdevIx, mdev);
          return res;
        }
      }
    } else {
      if (p0->Type() == GDL_COMPLEXDBL || (p0->Type() == GDL_COMPLEX && dbl)) {
        DComplexDblGDL* input = e->GetParAs<DComplexDblGDL>(0);
        DComplexDbl mean;
        DComplexDbl var;
        DComplexDbl skew;
        DComplexDbl kurt;
        DComplexDbl sdev;
        DDouble mdev;
        if (omitNaN) do_moment_cpx_nan<DComplexDbl, double>(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
        else do_moment_cpx<DComplexDbl, double>(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
        if (domean) e->SetKW(meanIx, new DComplexDblGDL(mean));
        if (dovar) e->SetKW(varIx, new DComplexDblGDL(var));
        if (doskew) e->SetKW(skewIx, new DComplexDblGDL(skew));
        if (dokurt) e->SetKW(kurtIx, new DComplexDblGDL(kurt));
        if (dosdev) e->SetKW(sdevIx, new DComplexDblGDL(sdev));
        if (domdev) e->SetKW(mdevIx, new DDoubleGDL(mdev));
        DComplexDblGDL* res = new DComplexDblGDL(4, BaseGDL::NOZERO);
        (*res)[0] = mean;
        (*res)[1] = var;
        (*res)[2] = skew;
        (*res)[3] = kurt;
        return res;
      } else if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* input = e->GetParAs<DComplexGDL>(0);
        DComplex mean;
        DComplex var;
        DComplex skew;
        DComplex kurt;
        DComplex sdev;
        DFloat mdev;
        if (omitNaN) do_moment_cpx_nan<DComplex, float>(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
        else do_moment_cpx<DComplex, float>(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
        if (domean) e->SetKW(meanIx, new DComplexGDL(mean));
        if (dovar) e->SetKW(varIx, new DComplexGDL(var));
        if (doskew) e->SetKW(skewIx, new DComplexGDL(skew));
        if (dokurt) e->SetKW(kurtIx, new DComplexGDL(kurt));
        if (dosdev) e->SetKW(sdevIx, new DComplexGDL(sdev));
        if (domdev) e->SetKW(mdevIx, new DFloatGDL(mdev));
        DComplexGDL* res = new DComplexGDL(4, BaseGDL::NOZERO);
        (*res)[0] = mean;
        (*res)[1] = var;
        (*res)[2] = skew;
        (*res)[3] = kurt;
        return res;
      } else {
        if (dbl) {
          DDoubleGDL* input = e->GetParAs<DDoubleGDL>(0);
          DDouble mean;
          DDouble var;
          DDouble skew;
          DDouble kurt;
          DDouble sdev;
          DDouble mdev;
          if (omitNaN) do_moment_nan(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
          else do_moment(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
          if (domean) e->SetKW(meanIx, new DDoubleGDL(mean));
          if (dovar) e->SetKW(varIx, new DDoubleGDL(var));
          if (doskew) e->SetKW(skewIx, new DDoubleGDL(skew));
          if (dokurt) e->SetKW(kurtIx, new DDoubleGDL(kurt));
          if (dosdev) e->SetKW(sdevIx, new DDoubleGDL(sdev));
          if (domdev) e->SetKW(mdevIx, new DDoubleGDL(mdev));
          DDoubleGDL* res = new DDoubleGDL(4, BaseGDL::NOZERO);
          (*res)[0] = mean;
          (*res)[1] = var;
          (*res)[2] = skew;
          (*res)[3] = kurt;
          return res;
        } else {
          DFloatGDL* input = e->GetParAs<DFloatGDL>(0);
          DFloat mean;
          DFloat var;
          DFloat skew;
          DFloat kurt;
          DFloat sdev;
          DFloat mdev;
          if (omitNaN) do_moment_nan(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
          else do_moment(&(*input)[0], input->N_Elements(), mean, var, skew, kurt, mdev, sdev, maxmoment);
          if (domean) e->SetKW(meanIx, new DFloatGDL(mean));
          if (dovar) e->SetKW(varIx, new DFloatGDL(var));
          if (doskew) e->SetKW(skewIx, new DFloatGDL(skew));
          if (dokurt) e->SetKW(kurtIx, new DFloatGDL(kurt));
          if (dosdev) e->SetKW(sdevIx, new DFloatGDL(sdev));
          if (domdev) e->SetKW(mdevIx, new DFloatGDL(mdev));
          DFloatGDL* res = new DFloatGDL(4, BaseGDL::NOZERO);
          (*res)[0] = mean;
          (*res)[1] = var;
          (*res)[2] = skew;
          (*res)[3] = kurt;
          return res;
        }
      }
    }
  }

  template<typename T> void pos_ishft_s(T* out, const SizeT n, const char s) {
    // parallelization is marginally useful as the loop is well paralleized by compiler.
    for (SizeT i = 0; i < n; ++i) out[i] <<= s;
  }

  template<typename T> void neg_ishft_s(T* out, const SizeT n, const char s) {
    // parallelization is marginally useful as the loop is well paralleized by compiler.
    for (SizeT i = 0; i < n; ++i) out[i] >>= s;
  }

  template<typename T> void ishft_m(T* out, const SizeT n, const DLong* s) {
    if ((GDL_NTHREADS=parallelize( n))==1) {
      for (SizeT i = 0; i < n; ++i) {
        if (s[i] >= 0) out[i] <<= s[i];
        else out[i] >>= s[i];
      }
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < n; ++i) {
        if (s[i] >= 0) out[i] <<= s[i];
        else out[i] >>= s[i];
      }
    }
  }

  BaseGDL* ishft_single(BaseGDL* in, SizeT n, char s, bool pos) {
    BaseGDL* out = in->Dup();
    switch (in->Type()) {
    case GDL_BYTE:
    {
      DByte* _out = static_cast<DByte*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    case GDL_UINT:
    {
      DUInt* _out = static_cast<DUInt*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    case GDL_INT:
    {
      DInt* _out = static_cast<DInt*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    case GDL_LONG:
    {
      DLong* _out = static_cast<DLong*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    case GDL_ULONG:
    {
      DULong* _out = static_cast<DULong*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    case GDL_LONG64:
    {
      DULong64* _out = static_cast<DULong64*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    case GDL_ULONG64:
    {
      DLong64* _out = static_cast<DLong64*> (out->DataAddr());
      if (pos) pos_ishft_s(_out, n, s);
      else neg_ishft_s(_out, n, s);
    }
      break;
    default:
      throw;
    }
    return out;
  }

  BaseGDL* ishft_multiple(BaseGDL* in, DLongGDL* _s, SizeT n) {
    BaseGDL* out = in->Dup(); //New(n, BaseGDL::NOZERO);
    DLong* s = static_cast<DLong*> (_s->DataAddr());
    switch (in->Type()) {
    case GDL_BYTE:
    {
      DByte* _out = static_cast<DByte*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    case GDL_UINT:
    {
      DUInt* _out = static_cast<DUInt*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    case GDL_INT:
    {
      DInt* _out = static_cast<DInt*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    case GDL_LONG:
    {
      DLong* _out = static_cast<DLong*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    case GDL_ULONG:
    {
      DULong* _out = static_cast<DULong*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    case GDL_LONG64:
    {
      DULong64* _out = static_cast<DULong64*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    case GDL_ULONG64:
    {
      DLong64* _out = static_cast<DLong64*> (out->DataAddr());
      ishft_m(_out, n, s);
    }
      break;
    default:
      throw;
    }
    return out;
  }

  BaseGDL* ishft_fun(EnvT* e) {
    SizeT nParam=e->NParam(2);
    
    Guard<BaseGDL>guard;

    BaseGDL* in = (e->GetParDefined(0));
    DType typ = in->Type();
    //types are normally correct, so do not loose time looking for wrong types
    if (IntType(typ)) {
      dimension finalDim;
      //behaviour: minimum set of dimensions of arrays. singletons expanded to dimension,
      //keep array trace.
      SizeT nEl, maxEl = 1, minEl, finalN = 1;
      for (int i = 0; i < 2; ++i) {
        nEl = e->GetPar(i)->N_Elements();
        if ((nEl > 1) && (nEl > maxEl)) {
          maxEl = nEl;
          finalN = maxEl;
          finalDim = e->GetPar(i)->Dim();
        }
      } //first max - but we need first min:
      minEl = maxEl;
      for (int i = 0; i < 2; ++i) {
        nEl = e->GetPar(i)->N_Elements();
        if ((nEl > 1) && (nEl < minEl)) {
          minEl = nEl;
          finalN = minEl;
          finalDim = e->GetPar(i)->Dim();
        }
      }
      //note: res is always 0's type:
      //fill shift, using a large type Long as apparently IDL does
      DLongGDL* sss = e->GetParAs<DLongGDL>(1);
      //if sss is a singleton, or not:
      if (sss->N_Elements() == 1) {
        char shift;
        if ((*sss)[0] == 0) return in->Dup();
        else if ((*sss)[0] > 0) {
          if ((*sss)[0] > 254) shift = -1;
          else shift = (*sss)[0];
          return ishft_single(in, finalN, shift, true);
        } else {
          if ((*sss)[0] < -254) shift = -1;
          else shift = -(*sss)[0];
          return ishft_single(in, finalN, shift, false);
        }
      } else {
        if (in->Scalar()) {
          in = in->New(finalN, BaseGDL::INIT);
          guard.Reset(in);
        } //expand to return element size, for parallel processing
        return ishft_multiple(in, sss, finalN);
      }
    } else e->Throw("Operand must be integer:" + e->GetParString(0));
    return NULL; //pacify dumb compilers.
  }

  BaseGDL* shift_fun(EnvT* e) {
    SizeT nParam = e->NParam(2);

    BaseGDL* p0 = e->GetParDefined(0);

    SizeT nShift = nParam - 1;

    DLong sIx[ MAXRANK];

    // in fact, the second param can be a singleton or an array ...
    if (nShift == 1) {
      DLongGDL* s1v = e->GetParAs<DLongGDL>(1);

      if (s1v->N_Elements() == 1) {
        DLong s1;
        e->AssureLongScalarPar(1, s1);

        // IncRef[Obj] done for GDL_PTR and GDL_OBJ
        return p0->CShift(s1);
      }

      if (p0->Rank() != s1v->N_Elements())
        e->Throw("Incorrect number of arguments.");

      for (SizeT i = 0; i < s1v->N_Elements(); i++)
        sIx[ i] = (*s1v)[i];
    } else {

      if (p0->Rank() != nShift)
        e->Throw("Incorrect number of arguments.");

      //    DLong sIx[ MAXRANK];
      for (SizeT i = 0; i < nShift; i++)
        e->AssureLongScalarPar(i + 1, sIx[ i]);

      if (p0->Type() == GDL_OBJ)
        GDLInterpreter::IncRefObj(static_cast<DObjGDL*> (p0));
      else if (p0->Type() == GDL_PTR)
        GDLInterpreter::IncRef(static_cast<DPtrGDL*> (p0));

    }

    return p0->CShift(sIx);
  }

  BaseGDL* arg_present(EnvT* e) {
    e->NParam(1);

    if (!e->GlobalPar(0))
      return new DIntGDL(0);

    EnvBaseT* caller = e->Caller();
    if (caller == NULL)
      return new DIntGDL(0);

    BaseGDL** pp0 = &e->GetPar(0);

    int ix = caller->FindGlobalKW(pp0);
    if (ix == -1)
      return new DIntGDL(0);

    return new DIntGDL(1);
  }

  BaseGDL* eof_fun(EnvT* e) {
    e->NParam(1);

    DLong lun;
    e->AssureLongScalarPar(0, lun);
    //GD: see  #1187 discussion
    if (lun ==0) { //special stdin case
      char c=std::cin.get();
      if ( std::cin.eof() ) return new DIntGDL(1); else {std::cin.unget(); return new DIntGDL(0);}
    }
    
    bool stdLun = check_lun(e, lun);
    if (stdLun)
      return new DIntGDL(0);

    // nicer error message (Disregard if socket)
    if (fileUnits[ lun - 1].SockNum() == -1) {
      if (!fileUnits[ lun - 1].IsOpen())
        throw GDLIOException(e->CallingNode(), "File unit is not open: " + i2s(lun) + ".");

      if (fileUnits[ lun - 1].Eof())
        return new DIntGDL(1);
    } else {
      // Socket
      string *recvBuf = &fileUnits[ lun - 1].RecvBuf();
      if (recvBuf->size() == 0)
        return new DIntGDL(1);
    }
    return new DIntGDL(0);
  }

  BaseGDL* rebin_fun(EnvT* e) {
    SizeT nParam = e->NParam(2);

    BaseGDL* p0 = e->GetNumericParDefined(0);

    SizeT rank = p0->Rank();

    if (rank == 0)
      e->Throw("Expression must be an array in this context: " +
      e->GetParString(0));

    SizeT resDimInit[ MAXRANK];

    DLongGDL* p1 = e->GetParAs<DLongGDL>(1);
    if (p1->Rank() > 0 && nParam > 2)
      e->Throw("The new dimensions must either be specified as an array or as a set of scalars.");
    SizeT np = p1->Rank() == 0 ? nParam : p1->N_Elements() + 1;

    for (SizeT p = 1; p < np; ++p) {
      DLong newDim;
      if (p1->Rank() == 0) e->AssureLongScalarPar(p, newDim);
      else newDim = (*p1)[p - 1];

      if (newDim <= 0)
        e->Throw("Array dimensions must be greater than 0.");

      if (rank >= p) {
        SizeT oldDim = p0->Dim(p - 1);

        if (newDim > oldDim) {
          if ((newDim % oldDim) != 0)
            e->Throw("Result dimensions must be integer factor "
            "of original dimensions.");
        } else {
          if ((oldDim % newDim) != 0)
            e->Throw("Result dimensions must be integer factor "
            "of original dimensions.");
        }
      }

      resDimInit[ p - 1] = newDim;
    }

    dimension resDim(resDimInit, np - 1);

    static int sampleIx = e->KeywordIx("SAMPLE");
    bool sample = e->KeywordSet(sampleIx);

    return p0->Rebin(resDim, sample);
  }

  BaseGDL* obj_class(EnvT* e) {
    SizeT nParam = e->NParam();

    static int countIx = e->KeywordIx("COUNT");
    static int superIx = e->KeywordIx("SUPERCLASS");

    bool super = e->KeywordSet(superIx);

    bool count = e->KeywordPresent(countIx);
    if (count)
      e->AssureGlobalKW(countIx);

    if (nParam > 0) {
      BaseGDL* p0 = e->GetParDefined(0);

      if (p0->Type() != GDL_STRING && p0->Type() != GDL_OBJ)
        e->Throw("Argument must be a scalar object reference or string: " +
        e->GetParString(0));

      if (!p0->Scalar())
        e->Throw("Expression must be a scalar or 1 element "
        "array in this context: " + e->GetParString(0));

      DStructDesc* objDesc;

      if (p0->Type() == GDL_STRING) {
        DString objName;
        e->AssureScalarPar<DStringGDL>(0, objName);
        objName = StrUpCase(objName);

        objDesc = FindObjectInStructList(structList, objName);
        if (objDesc == NULL) {
          if (count)
            e->SetKW(countIx, new DLongGDL(0));
          return new DStringGDL("");
        }
      } else // GDL_OBJ
      {
        DObj objRef;
        e->AssureScalarPar<DObjGDL>(0, objRef);

        if (objRef == 0) {
          if (count)
            e->SetKW(countIx, new DLongGDL(0));
          return new DStringGDL("");
        }

        DStructGDL* oStruct;
        try {
          oStruct = e->GetObjHeap(objRef);
        } catch (GDLInterpreter::HeapException&) { // non valid object
          if (count)
            e->SetKW(countIx, new DLongGDL(0));
          return new DStringGDL("");
        }

        objDesc = oStruct->Desc(); // cannot be NULL
      }

      if (!super) {
        if (count)
          e->SetKW(countIx, new DLongGDL(1));
        return new DStringGDL(objDesc->Name());
      }

      vector< string> pNames;
      objDesc->GetParentNames(pNames);

      SizeT nNames = pNames.size();

      if (count)
        e->SetKW(countIx, new DLongGDL(nNames));

      if (nNames == 0) {
        return new DStringGDL("");
      }

      DStringGDL* res = new DStringGDL(dimension(nNames),BaseGDL::NOZERO);

      for (SizeT i = 0; i < nNames; ++i) {
        (*res)[i] = pNames[i];
      }

      return res;
    }

    if (super)
      e->Throw("Conflicting keywords.");

    vector< string> objNames;
    for (SizeT i = 0; i < structList.size(); ++i) {
      if ((structList[i]->FunList().size() + structList[i]->ProList().size()) == 0) continue;
      objNames.push_back(structList[i]->Name());
    }
    SizeT nObj = objNames.size();
    if (count) e->SetKW(countIx, new DLongGDL(nObj));
    if (nObj > 0) {
      DStringGDL* res = new DStringGDL(dimension(nObj), BaseGDL::NOZERO);

      for (SizeT i = 0; i < nObj; ++i) {
        (*res)[i] = objNames[i];
      }
      return res;
    } else return new DStringGDL("");

  }

  BaseGDL* obj_hasmethod(EnvT* e) {
    SizeT nParam = e->NParam(2);
    //trace_me = trace_arg();
    BaseGDL*& p0 = e->GetPar(0);
    if (p0 == NULL || p0->Type() != GDL_OBJ)
      e->Throw("Object reference type required in this context: " +
      e->GetParString(0));

    BaseGDL* p1 = e->GetParDefined(1);
    if (p1->Type() != GDL_STRING)
      e->Throw("Methods can be referenced only with names (strings)");
    DStringGDL* p1S = static_cast<DStringGDL*> (p1);
    DObjGDL* pObj = static_cast<DObjGDL*> (p0);
    SizeT nObj = p0->StrictScalar() ? 1 : p0->N_Elements();
    DByteGDL* res = new DByteGDL(dimension(nObj),BaseGDL::ZERO);
    Guard<DByteGDL> res_guard(res);
    DByteGDL* altres = new DByteGDL(dimension(nObj),BaseGDL::ZERO);
    Guard<DByteGDL> altres_guard(altres);
    GDLInterpreter* interpreter = e->Interpreter();

    for (SizeT iobj = 0; iobj < nObj; ++iobj) {
      if (((*res)[iobj] != 0) || ((*altres)[iobj] != 0)) continue;
      DObj s = (*static_cast<DObjGDL*> (p0))[iobj];
      if (s != 0) {
        //          DStructGDL* oStruct = e->GetObjHeap( (*pObj)[iobj]);
        DStructGDL* oStruct = e->GetObjHeap(s);
        //if(trace_me) std::cout << " oStruct";
        DStructDesc* odesc = oStruct->Desc();
        int passed = 1;
        for (SizeT m = 0; m < p1->N_Elements(); m++) {
          DString method = StrUpCase((*p1S)[m]);
          //          if(trace_me) std::cout << method;
          if (odesc->GetFun(method) != NULL) continue;
          if (odesc->GetPro(method) != NULL) continue;
          passed = 0;
          break;
        }
        (*res)[iobj] = passed;
        for (SizeT i = iobj + 1; i < nObj; ++i) {
          if (interpreter->ObjValid((*pObj)[ i]))
            if (e->GetObjHeap((*pObj)[i])->Desc() == odesc) {
              (*res)[i] = passed;
              (*altres)[i] = 1 - passed;
            }
        }
      } // else if(trace_me) std::cout << " 0 ";
    }
    if (p0->StrictScalar())
      return new DByteGDL((*res)[0]);
    else return res_guard.release();
  }

  BaseGDL* obj_isa(EnvT* e) {
    e->NParam(2);
    DString className;
    e->AssureScalarPar<DStringGDL>(1, className);
    className = StrUpCase(className);
    if (className == "IDL_OBJECT")
      className = GDL_OBJECT_NAME;
    else if (className == "IDL_CONTAINER")
      className = GDL_CONTAINER_NAME;
    BaseGDL* p0 = e->GetPar(0);
    //nObjects is the number of objects or strings passed in array format.
    SizeT nElem = p0->N_Elements();

    DByteGDL* res = new DByteGDL(p0->Dim()); // zero

    if (p0->Type() == GDL_OBJ) {
      DObjGDL* pObj = static_cast<DObjGDL*> (p0);
      if (pObj) { //pObj protection probably overkill.
        for (SizeT i = 0; i < nElem; ++i) {
          if (e->Interpreter()->ObjValid((*pObj)[ i])) {
            DStructGDL* oStruct = e->GetObjHeap((*pObj)[i]);
            if (oStruct->Desc()->IsParent(className))
              (*res)[i] = 1;
          }
        }
        return res;
      }
    } else if (p0->Type() == GDL_STRING) {
      std::cerr << "OBJ_ISA: not implemented for strings, only objects (FIXME)." << endl;
      for (SizeT i = 0; i < nElem; ++i) {
        (*res)[i] = 0;
      }
      return res;
    } else e->Throw("Object reference type required in this context: " + e->GetParString(0));
    return NULL;
  }

  BaseGDL* n_tags(EnvT* e) {
    e->NParam(1);

    BaseGDL* p0 = e->GetPar(0);
    if (p0 == NULL)
      return new DLongGDL(0);

    if (p0->Type() != GDL_STRUCT)
      return new DLongGDL(0);

    DStructGDL* s = static_cast<DStructGDL*> (p0);

    //static int lengthIx = e->KeywordIx( "DATA_LENGTH");
    //bool length = e->KeywordSet( lengthIx);

    // we don't know now how to distinguish the 2 following cases
    static int datalengthIx = e->KeywordIx("DATA_LENGTH");
    static int lengthIx = e->KeywordIx("LENGTH");

    if (e->KeywordSet(datalengthIx))
      return new DLongGDL(s->SizeofTags());

    if (e->KeywordSet(lengthIx))
      return new DLongGDL(s->Sizeof());

    return new DLongGDL(s->Desc()->NTags());
  }

  BaseGDL* bytscl(EnvT* e) {
    SizeT nParam = e->NParam(1);

    BaseGDL* p0 = e->GetNumericParDefined(0);

    static int minIx = e->KeywordIx("MIN");
    static int maxIx = e->KeywordIx("MAX");
    static int topIx = e->KeywordIx("TOP");
    static int nanIx = e->KeywordIx("NAN");
    bool omitNaN = e->KeywordPresent(nanIx);

    //the following is going to be wrong in cases where TOP is so negative that a Long does not suffice.
    //Besides, a template version for each different type would be faster and probably the only solution to get the
    //correct behavior in all cases.
    DLong topL = 255;
    if (e->GetKW(topIx) != NULL)  e->AssureLongScalarKW(topIx, topL);
    if (topL > 255) topL = 255; // Bug corrected: Topl cannot be > 255.
    DDouble dTop = static_cast<DDouble> (topL); //Topl can be extremely negative.

    DDouble min;
    bool minSet = false;
    // SA: handling 3 parameters to emulate undocumented IDL behaviour
    //     of translating second and third arguments to MIN and MAX, respectively
    //     (parameters have precedence over keywords)
    if (nParam >= 2) {
      e->AssureDoubleScalarPar(1, min);
      minSet = true;
    } else if (e->GetKW(minIx) != NULL) {
      e->AssureDoubleScalarKW(minIx, min);
      minSet = true;
    }

    DDouble max;
    bool maxSet = false;
    if (nParam == 3) {
      e->AssureDoubleScalarPar(2, max);
      maxSet = true;
    } else if (e->GetKW(maxIx) != NULL) {
      e->AssureDoubleScalarKW(maxIx, max);
      maxSet = true;
    }

    DDoubleGDL* dRes = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));

    DLong maxEl, minEl;
    if (!maxSet || !minSet)
      dRes->MinMax(&minEl, &maxEl, NULL, NULL, omitNaN);
    if (!minSet)
      min = (*dRes)[ minEl];
    if (!maxSet)
      max = (*dRes)[ maxEl];

    //    cout << "Min/max :" << min << " " << max << endl;

    SizeT nEl = dRes->N_Elements();
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      if (IntType(p0->Type())) {
        //Is a thread pool function
        for (SizeT i = 0; i < nEl; ++i) {
          DDouble& d = (*dRes)[ i];
          if (d <= min) (*dRes)[ i] = 0;
          else if (d >= max) (*dRes)[ i] = dTop;
          else {
            // SA: floor is used for integer types to simulate manipulation on input data types
            (*dRes)[ i] = floor(((dTop + 1.)*(d - min) - 1.) / (max - min));
          }
        }
      } else {
        for (SizeT i = 0; i < nEl; ++i) {
          DDouble& d = (*dRes)[ i];
          if (omitNaN && (isnan(d) || isinf(d))) (*dRes)[ i] = 0;
          else if (d <= min) (*dRes)[ i] = 0;
          else if (d >= max) (*dRes)[ i] = dTop;
          else {
            // SA (?): here floor is used (instead of round) to simulate IDL behaviour
            (*dRes)[ i] = floor(((dTop + .9999)*(d - min)) / (max - min));
          }
        }
      }
    } else {
      if (IntType(p0->Type())) {
        //Is a thread pool function
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) {
          DDouble& d = (*dRes)[ i];
          if (d <= min) (*dRes)[ i] = 0;
          else if (d >= max) (*dRes)[ i] = dTop;
          else {
            // SA: floor is used for integer types to simulate manipulation on input data types
            (*dRes)[ i] = floor(((dTop + 1.)*(d - min) - 1.) / (max - min));
          }
        }
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (OMPInt i = 0; i < nEl; ++i) {
          DDouble& d = (*dRes)[ i];
          if (omitNaN && (isnan(d) || isinf(d))) (*dRes)[ i] = 0;
          else if (d <= min) (*dRes)[ i] = 0;
          else if (d >= max) (*dRes)[ i] = dTop;
          else {
            // SA (?): here floor is used (instead of round) to simulate IDL behaviour
            (*dRes)[ i] = floor(((dTop + .9999)*(d - min)) / (max - min));
          }
        }
      }
    }
    return dRes->Convert2(GDL_BYTE);
  }

  BaseGDL* strtok_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);

    DString stringIn;
    e->AssureStringScalarPar(0, stringIn);

    DString pattern = " \t";
    if (nParam > 1) {
      e->AssureStringScalarPar(1, pattern);
    }

    static int extractIx = e->KeywordIx("EXTRACT");
    bool extract = e->KeywordSet(extractIx);

    static int countIx = e->KeywordIx("COUNT");
    bool countPresent = e->KeywordPresent(countIx);

    static int lengthIx = e->KeywordIx("LENGTH");
    bool lengthPresent = e->KeywordPresent(lengthIx);

    static int pre0Ix = e->KeywordIx("PRESERVE_NULL");
    bool pre0 = e->KeywordSet(pre0Ix);

    static int regexIx = e->KeywordIx("REGEX");
    bool regex = e->KeywordSet(regexIx);
    char err_msg[MAX_REGEXPERR_LENGTH];
    regex_t regexp;

    static int foldCaseIx = e->KeywordIx("FOLD_CASE");
    bool foldCaseKW = e->KeywordSet(foldCaseIx);
    //FOLD_CASE can only be specified if the REGEX keyword is set
    if (!regex && foldCaseKW) e->Throw("Conflicting keywords.");

    vector<long> tokenStart;
    vector<long> tokenLen;

    int strLen = stringIn.length();

    DString escape = "";
    static int ESCAPEIx = e->KeywordIx("ESCAPE");

    //ESCAPE cannot be specified with the FOLD_CASE or REGEX keywords.
    if (regex && e->KeywordPresent(ESCAPEIx)) e->Throw("Conflicting keywords.");
    if (foldCaseKW && e->KeywordPresent(ESCAPEIx)) e->Throw("Conflicting keywords.");

    vector<long> escList;
	if (e->KeywordSet(ESCAPEIx)) {
	  e->AssureStringScalarKWIfPresent(ESCAPEIx, escape); //must be a singleton.
	  size_t pos = 0;
	  while (pos != std::string::npos) {
		pos = stringIn.find_first_of(escape, pos);
		if (pos != string::npos) {
		  escList.push_back(pos + 1); // remember escaped char
		  pos += 2; // skip escaped char
		}
	  }
	}
    vector<long>::iterator escBeg = escList.begin();
    vector<long>::iterator escEnd = escList.end();

    long tokB = 0;
    long tokE;
    long nextE = 0;
    long actLen;
    //special case: pattern void string
    if (pattern.size() == 0) {
      if (lengthPresent) {
        e->AssureGlobalKW(lengthIx);
        e->SetKW(lengthIx, new DLongGDL(0));
      }
      if (countPresent) {
        e->AssureGlobalKW(countIx);
        e->SetKW(countIx, new DLongGDL(0));
      }
      if (!extract) return new DLongGDL(0);
      else return new DStringGDL("");
    }

    // If regex then compile regex.
    // set the compile flags to use the REG_ICASE facility in case /FOLD_CASE is given.
    int cflags = REG_EXTENDED;
    if (foldCaseKW)
      cflags |= REG_ICASE;

    if (regex) {
      if (pattern == " \t") pattern = " "; // regcomp doesn't like "\t" JMG
      int compRes = regcomp(&regexp, pattern.c_str(), cflags);
      if (compRes) {
        regerror(compRes, &regexp, err_msg, MAX_REGEXPERR_LENGTH);
        e->Throw("Error processing regular expression: " +
          pattern + "\n           " + string(err_msg) + ".");
      }
    }

    if (foldCaseKW && !regex) { //duplicate pattern with ascii chars upcased
      pattern = pattern + StrUpCase(pattern);
    }
    for (;;) {
      regmatch_t pmatch[1];
      if (regex) {
        int matchres = regexec(&regexp, stringIn.c_str() + nextE, 1, pmatch, 0);
        tokE = matchres ? -1 : pmatch[0].rm_so;
      } else {
        tokE = stringIn.find_first_of(pattern, nextE);
      }

      if (tokE == string::npos) {
        actLen = strLen - tokB;
        if (actLen > 0 || pre0) {
          tokenStart.push_back(tokB);
          tokenLen.push_back(actLen);
        }
        break;
      }

      if (find(escBeg, escEnd, tokE) == escEnd) {
        if (regex) actLen = tokE;
        else actLen = tokE - tokB;
        if (actLen > 0 || pre0) {
          tokenStart.push_back(tokB);
          tokenLen.push_back(actLen);
        }
        if (regex) tokB += pmatch[0].rm_eo;
        else tokB = tokE + 1;
      }
      if (regex) nextE += pmatch[0].rm_eo;
      else nextE = tokE + 1;
    } // for(;;)

    if (regex) regfree(&regexp);

    SizeT nTok = tokenStart.size();
    if (countPresent) {
      e->AssureGlobalKW(countIx);
      if (nTok > 0) {
        DLongGDL* count = new DLongGDL(nTok);
        e->SetKW(countIx, count);
      } else {
        e->SetKW(countIx, new DLongGDL(0));
      }
    }

    if (lengthPresent) {
      e->AssureGlobalKW(lengthIx);

      if (nTok > 0) {
        dimension dim(nTok);
        DLongGDL* len = new DLongGDL(dim,BaseGDL::NOZERO);
        for (int i = 0; i < nTok; i++)
          (*len)[i] = tokenLen[i];

        e->SetKW(lengthIx, len);
      } else {
        e->SetKW(lengthIx, new DLongGDL(0));
      }
    }

    if (!extract) {

      if (nTok == 0) return new DLongGDL(0);

      dimension dim(nTok);
      DLongGDL* d = new DLongGDL(dim,BaseGDL::NOZERO);
      for (int i = 0; i < nTok; i++)
        (*d)[i] = tokenStart[i];
      return d;
    } else {

      // EXTRACT
      if (nTok == 0) return new DStringGDL("");

      dimension dim(nTok);
      DStringGDL *d = new DStringGDL(dim,BaseGDL::NOZERO);
      for (int i = 0; i < nTok; i++) {
        (*d)[i] = stringIn.substr(tokenStart[i], tokenLen[i]);

        // remove escape
        DString& act = (*d)[i];
        long escPos = act.find_first_of(escape, 0);
        while (escPos != string::npos) {
          act = act.substr(0, escPos) + act.substr(escPos + 1);
          escPos = act.find_first_of(escape, escPos + 1);
        }
      }
      return d;
    }
  }

  BaseGDL* getenv_fun(EnvT* e) {
    SizeT nParam = e->NParam();

    static int environmentIx = e->KeywordIx("ENVIRONMENT");
    bool environment = e->KeywordSet(environmentIx);

    SizeT nEnv;
    DStringGDL* env;

    if (environment) {

      if (nParam != 0)
        e->Throw("Incorrect number of arguments.");

      // determine number of environment entries
      for (nEnv = 0; environ[nEnv] != NULL; ++nEnv);

      dimension dim(nEnv);
      env = new DStringGDL(dim,BaseGDL::NOZERO);

      // copy stuff into local string array
      for (SizeT i = 0; i < nEnv; ++i)
        (*env)[i] = environ[i];

    } else {

      if (nParam != 1)
        e->Throw("Incorrect number of arguments.");

      DStringGDL* name = e->GetParAs<DStringGDL>(0);
      nEnv = name->N_Elements();

      env = new DStringGDL(name->Dim(),BaseGDL::NOZERO);

      // copy the stuff into local string only if param found
      char *resPtr;
      for (SizeT i = 0; i < nEnv; ++i) {
        // handle special environment variables
        // GDL_TMPDIR, IDL_TMPDIR
        if ((*name)[i] == "GDL_TMPDIR" || (*name)[i] == "IDL_TMPDIR") {
          resPtr = getenv((*name)[i].c_str());

          if (resPtr != NULL) {
            (*env)[i] = resPtr;
          } else {
            //        (*env)[i] = SysVar::Dir();
#ifdef _WIN32
            WCHAR tmpBuf[MAX_PATH];
            GetTempPathW(MAX_PATH, tmpBuf);
            char c_tmpBuf[MAX_PATH];
            WideCharToMultiByte(CP_ACP, 0, tmpBuf, MAX_PATH, c_tmpBuf, MAX_PATH, NULL, NULL);
            (*env)[i] = c_tmpBuf;
#else
            // AC 2017/10/19 : why _PATH_VARTMP_, not just _PATH_TMP_
            (*env)[i] = _PATH_TMP;
#endif
          }
          AppendIfNeeded((*env)[i], lib::PathSeparator());
        } else // normal environment variables
          if ((resPtr = getenv((*name)[i].c_str())))
          (*env)[i] = resPtr;
      }
    }

    return env;
  }

  BaseGDL* tag_names_fun(EnvT* e) {
    SizeT nParam = e->NParam();
    BaseGDL* p = e->GetParDefined(0);
    DStructGDL* struc = nullptr;
    if (p->Type() == DObjGDL::t) {
      DObjGDL* obj = static_cast<DObjGDL*> (p);
      DObj objRef;
      if (obj && obj->Scalar(objRef)) {
        try {
          struc = e->GetObjHeap(objRef);
        } catch (GDLInterpreter::HeapException&) {
        }
      }
    } else if (p->Type() == DStructGDL::t) {
      struc = static_cast<DStructGDL*> (p);
    }

    if (!struc) {
      e->Throw("Error: Failed to obtain structure. Input type: " + p->TypeStr());
    }

    static int structureNameIx = e->KeywordIx("STRUCTURE_NAME");
    bool structureName = e->KeywordSet(structureNameIx);

    DStringGDL* tagNames;

    if (structureName) {

      if ((*struc).Desc()->Name() != "$truct") {
        tagNames = new DStringGDL((*struc).Desc()->Name());
      } else {
        tagNames = new DStringGDL("");
      }
    } else {
      SizeT nTags = (*struc).Desc()->NTags();
      tagNames = new DStringGDL(dimension(nTags),BaseGDL::NOZERO);
      for (int i = 0; i < nTags; ++i) {
        (*tagNames)[i] = (*struc).Desc()->TagName(i);
      }
    }

    return tagNames;

  }

  // AC 12-Oc-2011: better version for: len=len, /Extract and /Sub
  // but it is still not perfect

  BaseGDL* stregex_fun(EnvT* e) {
    SizeT nParam = e->NParam(2);

    DStringGDL* stringExpr = e->GetParAs<DStringGDL>(0);
    dimension dim = stringExpr->Dim();

    DString pattern;
    e->AssureStringScalarPar(1, pattern);
    if (pattern.size() <= 0) {
      e->Throw("Error processing regular expression: " + pattern +
        "\n           empty (sub)expression");
    }

    static int booleanIx = e->KeywordIx("BOOLEAN");
    bool booleanKW = e->KeywordSet(booleanIx);

    static int extractIx = e->KeywordIx("EXTRACT");
    bool extractKW = e->KeywordSet(extractIx);

    static int foldCaseIx = e->KeywordIx("FOLD_CASE");
    bool foldCaseKW = e->KeywordSet(foldCaseIx);

    //XXXpch: this is wrong, should check arg_present
    static int lengthIx = e->KeywordIx("LENGTH");
    bool lengthKW = e->KeywordPresent(lengthIx);

    static int subexprIx = e->KeywordIx("SUBEXPR");
    bool subexprKW = e->KeywordSet(subexprIx);

    if (booleanKW && (subexprKW || extractKW || lengthKW))
      e->Throw("Conflicting keywords.");

    char err_msg[MAX_REGEXPERR_LENGTH];

    // set the compile flags
    int cflags = REG_EXTENDED;
    if (foldCaseKW)
      cflags |= REG_ICASE;
    if (booleanKW)
      cflags |= REG_NOSUB;

    // compile the regular expression
    regex_t regexp;
    int compRes = regcomp(&regexp, pattern.c_str(), cflags);
    SizeT nSubExpr = regexp.re_nsub + 1;

    //    cout << regexp.re_nsub << endl;

    if (compRes) {
      regerror(compRes, &regexp, err_msg, MAX_REGEXPERR_LENGTH);
      e->Throw("Error processing regular expression: " +
        pattern + "\n           " + string(err_msg) + ".");
    }

    BaseGDL* result;

    if (booleanKW)
      result = new DByteGDL(dim,BaseGDL::ZERO); //must not use BaseGDL::NOZERO see below.
    else if (extractKW && !subexprKW) {
      //  cout << "my pb ! ? dim= " << dim << endl;
      result = new DStringGDL(dim,BaseGDL::ZERO);
    } else if (subexprKW) {
      //  cout << "my pb 2 ? dim= " << dim << endl;
      dimension subExprDim = dim;
      subExprDim >> nSubExpr; // m_schellens: commented in, needed
      if (extractKW)
        result = new DStringGDL(subExprDim,BaseGDL::ZERO);
      else
        result = new DLongGDL(subExprDim,BaseGDL::ZERO);
    } else
      result = new DLongGDL(dim,BaseGDL::ZERO);

    DLongGDL* len = NULL;
    if (lengthKW) {
      e->AssureGlobalKW(lengthIx);
      if (subexprKW) {
        dimension subExprDim = dim;
        subExprDim >> nSubExpr; // m_schellens: commented in, needed
        len = new DLongGDL(subExprDim,BaseGDL::NOZERO);
      } else {
        len = new DLongGDL(dim,BaseGDL::NOZERO);
      }
      for (SizeT i = 0; i < len->N_Elements(); ++i)
        (*len)[i] = -1;
    }

    int nmatch = 1;
    if (subexprKW) nmatch = nSubExpr;

    regmatch_t* pmatch = new regmatch_t[nSubExpr];
    ArrayGuard<regmatch_t> pmatchGuard(pmatch);

    //    cout << "dim " << dim.NDimElements() << endl;
    for (SizeT s = 0; s < dim.NDimElements(); ++s) {
      int eflags = 0;

      for (SizeT sE = 0; sE < nSubExpr; ++sE)
        pmatch[sE].rm_so = -1;

      // now match towards the string
      int matchres = regexec(&regexp, (*stringExpr)[s].c_str(), nmatch, pmatch, eflags);

      // subexpressions
      if (extractKW && subexprKW) {

        // Loop through subexpressions & fill output array
        for (SizeT i = 0; i < nSubExpr; ++i) {
          if (pmatch[i].rm_so != -1)
            (*static_cast<DStringGDL*> (result))[i + s * nSubExpr] =
            (*stringExpr)[s].substr(pmatch[i].rm_so, pmatch[i].rm_eo - pmatch[i].rm_so);
          //          (*stringExpr)[i+s*nSubExpr].substr( pmatch[i].rm_so,  pmatch[i].rm_eo - pmatch[i].rm_so);
          if (lengthKW)
            (*len)[i + s * nSubExpr] = pmatch[i].rm_so != -1 ? pmatch[i].rm_eo - pmatch[i].rm_so : -1;
          //            (*len)[i+s*nSubExpr] = pmatch[i].rm_eo - pmatch[i].rm_so;
        }
      } else if (subexprKW) {
        //      cout << "je ne comprends pas v2: "<< nSubExpr << endl;

        // Loop through subexpressions & fill output array
        for (SizeT i = 0; i < nSubExpr; ++i) {
          (* static_cast<DLongGDL*> (result))[i + s * nSubExpr] = pmatch[i].rm_so;
          if (lengthKW)
            (*len)[i + s * nSubExpr] = pmatch[i].rm_so != -1 ? pmatch[i].rm_eo - pmatch[i].rm_so : -1;
        }
      } else {
        if (booleanKW)
          (* static_cast<DByteGDL*> (result))[s] = (matchres == 0);
        else if (extractKW) // !subExprKW
        {
          if (matchres == 0)
            (* static_cast<DStringGDL*> (result))[s] =
            (*stringExpr)[s].substr(pmatch[0].rm_so,
            pmatch[0].rm_eo - pmatch[0].rm_so);
        } else
          (*static_cast<DLongGDL*> (result))[s] = matchres ? -1 : pmatch[0].rm_so;
      }

      if (lengthKW && !subexprKW)
        //(*len)[s] = pmatch[0].rm_eo - pmatch[0].rm_so;
        (*len)[s] = pmatch[0].rm_so != -1 ? pmatch[0].rm_eo - pmatch[0].rm_so : -1;

    }

    regfree(&regexp);

    if (lengthKW)
      e->SetKW(lengthIx, len);

    return result;
  }

  BaseGDL* routine_filepath(EnvT* e) {
    SizeT nParam = e->NParam();
    DStringGDL* p0S;
    Guard<DStringGDL> p0S_guard;
    if (nParam > 1) e->Throw("Incorrect number of arguments.");
    if (nParam > 0) {
      BaseGDL* p0 = e->GetParDefined(0);
      if (p0->Type() != GDL_STRING)
        e->Throw("String expression required in this context: " + e->GetParString(0));
      p0S = static_cast<DStringGDL*> (p0);
    } else { // routine_filepath()
      p0S = new DStringGDL(dynamic_cast<DSubUD*> ((e->Caller())->GetPro())->ObjectName());
      p0S_guard.Init(p0S);
    }

    static int is_functionIx = e->KeywordIx("IS_FUNCTION");
    bool is_functionKW = e->KeywordSet(is_functionIx);
    static int eitherIx = e->KeywordIx("EITHER");
    bool eitherKW = e->KeywordSet(eitherIx);

    SizeT nPath = p0S->N_Elements();
    DStringGDL* res = new DStringGDL(p0S->Dim(), BaseGDL::NOZERO);
    Guard<DStringGDL> res_guard(res);

    DString name;
    string FullFileName;
    for (int i = 0; i < nPath; i++) {

      name = StrUpCase((*p0S)[i]);

      bool found = false;
      FullFileName = "";

      size_t pos(0);
      if ((pos = name.find("::")) != DString::npos) {
        DString struct_tag = name.substr(0, pos);
        DString method_name = name.substr(pos + 2);
        for (auto& s : structList) {
          if (s && (s->Name() != struct_tag)) continue;
          if (eitherKW || !is_functionKW) {
            DPro* pp = s->FindInProList(method_name);
            if (pp) {
              found = true;
              FullFileName = pp->GetFilename();
              break;
            }
          }
          if (!found && (is_functionKW || eitherKW)) {
            DFun* fp = s->FindInFunList(method_name);
            if (fp) {
              found = true;
              FullFileName = fp->GetFilename();
              break;
            }
          }
        }
      } else {
        if (eitherKW || !is_functionKW) {
          for (ProListT::iterator i = proList.begin();
            i != proList.end(); ++i)
            if ((*i)->ObjectName() == name) {
              found = true;
              FullFileName = (*i)->GetFilename();
              break;
            }
        }
        if (!found && (is_functionKW || eitherKW)) {
          for (FunListT::iterator i = funList.begin();
            i != funList.end(); ++i)
            if ((*i)->ObjectName() == name) {
              found = true;
              FullFileName = (*i)->GetFilename();
              break;
            }
        }
      }

      (*res)[i] = FullFileName;
    }
    //    if(nParam == 0) return new DStringGDL(FullFileName);
    return res_guard.release();
  }

  //AC 2019 (see "routine_name.pro". Here another way to catch the name ...)

  BaseGDL* routine_name_fun(EnvT* e) {
    EnvStackT& callStack = e->Interpreter()->CallStack();
    string name = callStack.back()->GetProName();
    return new DStringGDL(name);
  }

  BaseGDL* routine_info(EnvT* e) {
    SizeT nParam = e->NParam();
    if (nParam > 1) e->Throw("Incorrect number of arguments.");

    static int functionsIx = e->KeywordIx("FUNCTIONS");
    bool functionsKW = e->KeywordSet(functionsIx);
    static int systemIx = e->KeywordIx("SYSTEM");
    bool systemKW = e->KeywordSet(systemIx);
    static int disabledIx = e->KeywordIx("DISABLED");
    bool disabledKW = e->KeywordSet(disabledIx);
    static int parametersIx = e->KeywordIx("PARAMETERS");
    bool parametersKW = e->KeywordSet(parametersIx);
    static int sourceIx = e->KeywordIx("SOURCE");
    bool sourceKW = e->KeywordSet(sourceIx);

    if (sourceKW) {

      // sanity checks
      if (systemKW) e->Throw("Conflicting keywords.");

      DString raw_name, name;
      string FullFileName;
      bool found = FALSE;
      DStructGDL* stru;
      DStructDesc* stru_desc;


      if (nParam == 1) {
        // getting the routine name from the first parameter (must be a singleton)
        e->AssureScalarPar<DStringGDL>(0, raw_name);
        name = StrUpCase(raw_name);
        if (functionsKW) {
          for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
            if ((*i)->ObjectName() == name) {
              found = true;
              FullFileName = (*i)->GetFilename();
              break;
            }
          }
          if (!found) e->Throw("% Attempt to call undefined/not compiled function: '" + raw_name + "'");
        } else {
          for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
            if ((*i)->ObjectName() == name) {
              found = true;
              FullFileName = (*i)->GetFilename();
              break;
            }
          }
	  if (name == "$MAIN$") {
	    found = true;
	    FullFileName = "";
	  }
          if (!found) e->Throw("% Attempt to call undefined/not compiled procedure: '" + raw_name + "'");
        }

        // creating the output anonymous structure
        stru_desc = new DStructDesc("$truct");
        SpDString aString;
        stru_desc->AddTag("NAME", &aString);
        stru_desc->AddTag("PATH", &aString);
        stru = new DStructGDL(stru_desc, dimension());

        // filling the structure with information about the routine
        stru->InitTag("NAME", DStringGDL(name));
        stru->InitTag("PATH", DStringGDL(FullFileName));
        return stru;

      } else {
        // creating the output anonymous structure
        stru_desc = new DStructDesc("$truct");
        SpDString aString;
        stru_desc->AddTag("NAME", &aString);
        stru_desc->AddTag("PATH", &aString);

        //always starts with $MAIN$
        SizeT N = (functionsKW) ? funList.size() + 1 : proList.size() + 1;
        stru = new DStructGDL(stru_desc, dimension(N));
        (*static_cast<DStringGDL*> (stru->GetTag((SizeT) 0, 0)))[0] = "$MAIN$";

        if (functionsKW) {
          SizeT ii = 1;
          for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
            (*static_cast<DStringGDL*> (stru->GetTag((SizeT) 0, ii)))[0] = (*i)->ObjectName();
            (*static_cast<DStringGDL*> (stru->GetTag((SizeT) 1, ii)))[0] = (*i)->GetFilename();
            ii++;
          }
        } else {
          SizeT ii = 1;
          for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
            (*static_cast<DStringGDL*> (stru->GetTag((SizeT) 0, ii)))[0] = (*i)->ObjectName();
            (*static_cast<DStringGDL*> (stru->GetTag((SizeT) 1, ii)))[0] = (*i)->GetFilename();
            ii++;
          }
        }
        return stru;
      }
    }

    if (parametersKW) {
      // sanity checks
      if (systemKW || disabledKW) e->Throw("Conflicting keywords.");

      // getting the routine name from the first parameter
      DString name;
      e->AssureScalarPar<DStringGDL>(0, name);
      name = StrUpCase(name);

      DSubUD* routine = functionsKW
        ? static_cast<DSubUD*> (funList[GDLInterpreter::GetFunIx(name)])
        : static_cast<DSubUD*> (proList[GDLInterpreter::GetProIx(name)]);
      SizeT np = routine->NPar(), nk = routine->NKey();

      // creating the output anonymous structure
      DStructDesc* stru_desc = new DStructDesc("$truct");
      SpDLong aLong;
      stru_desc->AddTag("NUM_ARGS", &aLong);
      stru_desc->AddTag("NUM_KW_ARGS", &aLong);
      if (np > 0) {
        SpDString aStringArr(dimension((int) np));
        stru_desc->AddTag("ARGS", &aStringArr);
      }
      if (nk > 0) {
        SpDString aStringArr(dimension((int) nk));
        stru_desc->AddTag("KW_ARGS", &aStringArr);
      }
      DStructGDL* stru = new DStructGDL(stru_desc, dimension());

      // filling the structure with information about the routine
      stru->InitTag("NUM_ARGS", DLongGDL(np));
      stru->InitTag("NUM_KW_ARGS", DLongGDL(nk));
      if (np > 0) {
        DStringGDL *pnames = new DStringGDL(dimension(np),BaseGDL::NOZERO);
        for (SizeT p = 0; p < np; ++p) (*pnames)[p] = routine->GetVarName(nk + p);
        stru->InitTag("ARGS", *pnames);
        GDLDelete(pnames);
      }
      if (nk > 0) {
        DStringGDL *knames = new DStringGDL(dimension(nk),BaseGDL::NOZERO);
        for (SizeT k = 0; k < nk; ++k) (*knames)[k] = routine->GetKWName(k);
        stru->InitTag("KW_ARGS", *knames);
        GDLDelete(knames);
      }

      // returning
      return stru;
    }

    // GDL does not have disabled routines
    if (disabledKW) return new DStringGDL("");

    //    if( functionsKW || systemKW || nParam == 0)
    //      {
    vector<DString> subList;

    if (functionsKW) {
      if (systemKW) {
        SizeT n = libFunList.size();
        if (n == 0) return new DStringGDL("");

        DStringGDL* res = new DStringGDL(dimension(n), BaseGDL::NOZERO);
        for (SizeT i = 0; i < n; ++i)
          (*res)[i] = libFunList[ i]->ObjectName();

        return res;
      } else {
        SizeT n = funList.size();
        if (n == 0) {
          Message("No FUNCTIONS compiled yet !");
          return new DStringGDL("");
        }
        for (SizeT i = 0; i < n; ++i)
          subList.push_back(funList[ i]->ObjectName());
      }
    } else {
      if (systemKW) {
        SizeT n = libProList.size();
        if (n == 0) return new DStringGDL("");

        DStringGDL* res = new DStringGDL(dimension(n), BaseGDL::NOZERO);
        for (SizeT i = 0; i < n; ++i)
          (*res)[i] = libProList[ i]->ObjectName();

        return res;
      } else {
        SizeT n = proList.size();
        if (n == 0) {
          Message("No PROCEDURES compiled yet !");
          DStringGDL* res = new DStringGDL(1, BaseGDL::NOZERO);
          (*res)[0] = "$MAIN$";
          return res;
        }
        subList.push_back("$MAIN$");
        for (SizeT i = 0; i < n; ++i)
          subList.push_back(proList[ i]->ObjectName());
      }
    }

    sort(subList.begin(), subList.end());
    SizeT nS = subList.size();

    DStringGDL* res = new DStringGDL(dimension(nS), BaseGDL::NOZERO);
    for (SizeT s = 0; s < nS; ++s)
      (*res)[ s] = subList[ s];

    return res;
    //      }
  }

  BaseGDL* temporary_fun(EnvT* e) {
    SizeT nParam = e->NParam(1);

    BaseGDL** p0 = &e->GetParDefined(0);

    BaseGDL* ret = *p0;

    *p0 = NULL; // make parameter undefined
    return ret;
  }

  BaseGDL* memory_fun(EnvT* e) {
    SizeT nParam = e->NParam(0);

    BaseGDL* ret;
    static int kw_l64_Ix = e->KeywordIx("L64");
    bool kw_l64 = e->KeywordSet(kw_l64_Ix);
    // TODO: IDL-doc mentions about automatically switching to L64 if needed

    static int structureIx = e->KeywordIx("STRUCTURE");
    if (e->KeywordSet(structureIx)) {
      // returning structure
      if (kw_l64) {
        ret = new DStructGDL("IDL_MEMORY64");
        DStructGDL* retStru = static_cast<DStructGDL*> (ret);
        (retStru->GetTag(retStru->Desc()->TagIndex("CURRENT")))->InitFrom(DLong64GDL(MemStats::GetCurrent()));
        (retStru->GetTag(retStru->Desc()->TagIndex("NUM_ALLOC")))->InitFrom(DLong64GDL(MemStats::GetNumAlloc()));
        (retStru->GetTag(retStru->Desc()->TagIndex("NUM_FREE")))->InitFrom(DLong64GDL(MemStats::GetNumFree()));
        (retStru->GetTag(retStru->Desc()->TagIndex("HIGHWATER")))->InitFrom(DLong64GDL(MemStats::GetHighWater()));
      } else {
        ret = new DStructGDL("IDL_MEMORY");
        DStructGDL* retStru = static_cast<DStructGDL*> (ret);
        (retStru->GetTag(retStru->Desc()->TagIndex("CURRENT")))->InitFrom(DLongGDL(MemStats::GetCurrent()));
        (retStru->GetTag(retStru->Desc()->TagIndex("NUM_ALLOC")))->InitFrom(DLongGDL(MemStats::GetNumAlloc()));
        (retStru->GetTag(retStru->Desc()->TagIndex("NUM_FREE")))->InitFrom(DLongGDL(MemStats::GetNumFree()));
        (retStru->GetTag(retStru->Desc()->TagIndex("HIGHWATER")))->InitFrom(DLongGDL(MemStats::GetHighWater()));
      }
    } else {
      static int Ix_kw_current = e->KeywordIx("CURRENT");
      static int Ix_kw_num_alloc = e->KeywordIx("NUM_ALLOC");
      static int Ix_kw_num_free = e->KeywordIx("NUM_FREE");
      static int Ix_kw_highwater = e->KeywordIx("HIGHWATER");

      bool kw_current = e->KeywordSet(Ix_kw_current);
      bool kw_num_alloc = e->KeywordSet(Ix_kw_num_alloc);
      bool kw_num_free = e->KeywordSet(Ix_kw_num_free);
      bool kw_highwater = e->KeywordSet(Ix_kw_highwater);

      // Following the IDL documentation: mutually exclusive keywords
      // IDL behaves different, incl. segfaults with selected kw combinations
      if (kw_current + kw_num_alloc + kw_num_free + kw_highwater > 1)
        e->Throw("CURRENT, NUM_ALLOC, NUM_FREE & HIGHWATER keywords"
        " are mutually exclusive");

      if (kw_current) {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetCurrent());
        else ret = new DLongGDL(MemStats::GetCurrent());
      }
      else if (kw_num_alloc) {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetNumAlloc());
        else ret = new DLongGDL(MemStats::GetNumAlloc());
      } else if (kw_num_free) {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetNumFree());
        else ret = new DLongGDL(MemStats::GetNumFree());
      } else if (kw_highwater) {
        if (kw_l64) ret = new DLong64GDL(MemStats::GetHighWater());
        else ret = new DLongGDL(MemStats::GetHighWater());
      } else {
        // returning 4-element array
        if (kw_l64) {
          ret = new DLong64GDL(dimension(4),BaseGDL::NOZERO);
          (*static_cast<DLong64GDL*> (ret))[0] = MemStats::GetCurrent();
          (*static_cast<DLong64GDL*> (ret))[1] = MemStats::GetNumAlloc();
          (*static_cast<DLong64GDL*> (ret))[2] = MemStats::GetNumFree();
          (*static_cast<DLong64GDL*> (ret))[3] = MemStats::GetHighWater();
        } else {
          ret = new DLongGDL(dimension(4),BaseGDL::NOZERO);
          (*static_cast<DLongGDL*> (ret))[0] = MemStats::GetCurrent();
          (*static_cast<DLongGDL*> (ret))[1] = MemStats::GetNumAlloc();
          (*static_cast<DLongGDL*> (ret))[2] = MemStats::GetNumFree();
          (*static_cast<DLongGDL*> (ret))[3] = MemStats::GetHighWater();
        }
      }
    }

    return ret;
  }

  inline DByte StrCmp(const string& s1, const string& s2, DLong n) {
    if (n <= 0) return 1;
    if (s1.substr(0, n) == s2.substr(0, n)) return 1;
    return 0;
  }

  inline DByte StrCmp(const string& s1, const string& s2) {
    if (s1 == s2) return 1;
    return 0;
  }

  inline DByte StrCmpFold(const string& s1, const string& s2, DLong n) {
    if (n <= 0) return 1;
    if (StrUpCase(s1.substr(0, n)) == StrUpCase(s2.substr(0, n))) return 1;
    return 0;
  }

  inline DByte StrCmpFold(const string& s1, const string& s2) {
    if (StrUpCase(s1) == StrUpCase(s2)) return 1;
    return 0;
  }

  BaseGDL* strcmp_fun(EnvT* e) {
    SizeT nParam = e->NParam(2);

    DStringGDL* s0 = static_cast<DStringGDL*> (e->GetParAs< DStringGDL>(0));
    DStringGDL* s1 = static_cast<DStringGDL*> (e->GetParAs< DStringGDL>(1));

    DLongGDL* l2 = NULL;
    if (nParam > 2) {
      l2 = static_cast<DLongGDL*> (e->GetParAs< DLongGDL>(2));
    }

    static int foldIx = e->KeywordIx("FOLD_CASE");
    bool fold = e->KeywordSet(foldIx);

    if (s0->Scalar() && s1->Scalar()) {
      if (l2 == NULL) {
        if (fold)
          return new DByteGDL(StrCmpFold((*s0)[0], (*s1)[0]));
        else
          return new DByteGDL(StrCmp((*s0)[0], (*s1)[0]));
      } else {
        DByteGDL* res = new DByteGDL(l2->Dim(), BaseGDL::NOZERO);
        SizeT nEl = l2->N_Elements();
        if (fold)
          for (SizeT i = 0; i < nEl; ++i)
            (*res)[i] = StrCmpFold((*s0)[0], (*s1)[0], (*l2)[i]);
        else
          for (SizeT i = 0; i < nEl; ++i)
            (*res)[i] = StrCmp((*s0)[0], (*s1)[0], (*l2)[i]);
        return res;
      }
    } else // at least one array
    {
      if (l2 == NULL) {
        if (s0->Scalar()) {
          DByteGDL* res = new DByteGDL(s1->Dim(), BaseGDL::NOZERO);
          SizeT nEl = s1->N_Elements();
          if (fold)
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmpFold((*s0)[0], (*s1)[i]);
          else
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmp((*s0)[0], (*s1)[i]);
          return res;
        } else if (s1->Scalar()) {
          DByteGDL* res = new DByteGDL(s0->Dim(), BaseGDL::NOZERO);
          SizeT nEl = s0->N_Elements();
          if (fold)
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmpFold((*s0)[i], (*s1)[0]);
          else
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmp((*s0)[i], (*s1)[0]);
          return res;
        } else // both arrays
        {
          DByteGDL* res;
          SizeT nEl;
          if (s0->N_Elements() <= s1->N_Elements()) {
            res = new DByteGDL(s0->Dim(), BaseGDL::NOZERO);
            nEl = s0->N_Elements();
          } else {
            res = new DByteGDL(s1->Dim(), BaseGDL::NOZERO);
            nEl = s1->N_Elements();
          }
          if (fold)
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmpFold((*s0)[i], (*s1)[i]);
          else
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmp((*s0)[i], (*s1)[i]);
          return res;
        }
      } else // l2 != NULL
      {
        DByteGDL* res;
        SizeT nEl;
        bool l2Scalar = l2->Scalar();
        if (s0->Scalar()) {
          if (l2Scalar || s1->N_Elements() <= l2->N_Elements()) {
            res = new DByteGDL(s1->Dim(), BaseGDL::NOZERO);
            nEl = s1->N_Elements();
          } else {
            res = new DByteGDL(l2->Dim(), BaseGDL::NOZERO);
            nEl = l2->N_Elements();
          }
          if (fold)
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmpFold((*s0)[0], (*s1)[i], (*l2)[l2Scalar ? 0 : i]);
          else
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmp((*s0)[0], (*s1)[i], (*l2)[l2Scalar ? 0 : i]);
          return res;
        } else if (s1->Scalar()) {
          if (l2Scalar || s0->N_Elements() <= l2->N_Elements()) {
            res = new DByteGDL(s0->Dim(), BaseGDL::NOZERO);
            nEl = s0->N_Elements();
          } else {
            res = new DByteGDL(l2->Dim(), BaseGDL::NOZERO);
            nEl = l2->N_Elements();
          }
          if (fold)
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmpFold((*s0)[i], (*s1)[0], (*l2)[l2Scalar ? 0 : i]);
          else
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmp((*s0)[i], (*s1)[0], (*l2)[l2Scalar ? 0 : i]);
          return res;
        } else // s1 and s2 are arrays
        {
          if (l2Scalar)
            if (s0->N_Elements() <= s1->N_Elements()) {
              res = new DByteGDL(s0->Dim(), BaseGDL::NOZERO);
              nEl = s0->N_Elements();
            } else {
              res = new DByteGDL(s1->Dim(), BaseGDL::NOZERO);
              nEl = s1->N_Elements();
            } else {
            if (s0->N_Elements() <= s1->N_Elements())
              if (s0->N_Elements() <= l2->N_Elements()) {
                res = new DByteGDL(s0->Dim(), BaseGDL::NOZERO);
                nEl = s0->N_Elements();
              } else {
                res = new DByteGDL(l2->Dim(), BaseGDL::NOZERO);
                nEl = l2->N_Elements();
              } else
              if (s1->N_Elements() <= l2->N_Elements()) {
              res = new DByteGDL(s1->Dim(), BaseGDL::NOZERO);
              nEl = s1->N_Elements();
            } else {
              res = new DByteGDL(l2->Dim(), BaseGDL::NOZERO);
              nEl = l2->N_Elements();
            }
          }
          if (fold)
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmpFold((*s0)[i], (*s1)[i], (*l2)[l2Scalar ? 0 : i]);
          else
            for (SizeT i = 0; i < nEl; ++i)
              (*res)[i] = StrCmp((*s0)[i], (*s1)[i], (*l2)[l2Scalar ? 0 : i]);
          return res;
        }
      }
    }
    assert(false);
  }

  string TagName(EnvT* e, const string& name) {
    string n = StrUpCase(name);
    SizeT len = n.size();
    //GD: bug (#1627) 
    if (n[0] == ' ' || n[0] == '\t' ) n[0] = '_';
    if (n[0] != '_' && n[0] != '!' && (n[0] < 'A' || n[0] > 'Z'))
      e->Throw("Illegal tag name: " + name + ".");
    for (SizeT i = 1; i < len; ++i) {
      if (n[i] == ' ')
        n[i] = '_';
      else
        if (n[i] != '_' && n[i] != '$' && //n[0] != '!' &&
        (n[i] < 'A' || n[i] > 'Z') &&
        (n[i] < '0' || n[i] > '9'))
        e->Throw("Illegal tag name: " + name + ".");
    }
    return n;
  }

  BaseGDL* create_struct(EnvT* e) {
    static int nameIx = e->KeywordIx("NAME");
    DString name = "$truct";
    if (e->KeywordPresent(nameIx)) {
      // Check if name exists, if not then treat as unnamed
      if (e->GetKW(nameIx) != NULL)
        e->AssureStringScalarKW(nameIx, name);
    }

    if (name != "$truct") // named struct
    {
      name = StrUpCase(name);

      SizeT nParam = e->NParam();

      if (nParam == 0) {
        DStructDesc* desc =
          e->Interpreter()->GetStruct(name, e->CallingNode());

        dimension dim(1);
        return new DStructGDL(desc, dim);
      }

      DStructDesc* nStructDesc;
      Guard<DStructDesc> nStructDescGuard;

      DStructDesc* oStructDesc =
        FindInStructList(structList, name);

      if (oStructDesc == NULL || oStructDesc->NTags() > 0) {
        // not defined at all yet (-> define now)
        // or completely defined  (-> define now and check equality)
        nStructDesc = new DStructDesc(name);

        // guard it
        nStructDescGuard.Reset(nStructDesc);
      } else {
        // NTags() == 0
        // not completely defined (only name in list)
        nStructDesc = oStructDesc;
      }

      // the instance variable
      //  dimension dim( 1);
      //  DStructGDL* instance = new DStructGDL( nStructDesc, dim);
      DStructGDL* instance = new DStructGDL(nStructDesc);
      Guard<DStructGDL> instance_guard(instance);

      for (SizeT p = 0; p < nParam; ++p) {
        BaseGDL* par = e->GetParDefined(p);
        if (par->Type() == GDL_STRUCT) {
          DStructGDL* parStruct = static_cast<DStructGDL*> (par);
          // add struct
          if (!parStruct->Scalar())
            e->Throw("Expression must be a scalar in this context: " +
            e->GetParString(p));

          DStructDesc* desc = parStruct->Desc();
          for (SizeT t = 0; t < desc->NTags(); ++t) {
            instance->NewTag(desc->TagName(t),
              parStruct->GetTag(t)->Dup());
          }
        } else {
          // add tag value pair
          DStringGDL* tagNames = e->GetParAs<DStringGDL>(p);
          SizeT nTags = tagNames->N_Elements();

          SizeT tagStart = p + 1;
          SizeT tagEnd = p + nTags;
          if (tagEnd >= nParam)
            e->Throw("Incorrect number of arguments.");

          do {
            ++p;
            BaseGDL* value = e->GetParDefined(p);

            // add
            instance->NewTag(TagName(e, (*tagNames)[ p - tagStart]), value->Dup());
          } while (p < tagEnd);
        }
      }

      if (oStructDesc != NULL) {
        if (oStructDesc != nStructDesc) {
          oStructDesc->AssureIdentical(nStructDesc);
          instance->DStructGDL::SetDesc(oStructDesc);
          //delete nStructDesc; // auto_ptr
        }
      } else {
        // release from guard (if not NULL)
        nStructDescGuard.release();
        // insert into struct list
        structList.push_back(nStructDesc);
      }

      instance_guard.release();
      return instance;
    } else { // unnamed struc

      // Handle case of single structure parameter
      SizeT nParam;
      nParam = e->NParam(1);
      BaseGDL* par = e->GetParDefined(0);
      //  DStructGDL* parStruct = dynamic_cast<DStructGDL*>( par);
      if (nParam != 1 || par->Type() != GDL_STRUCT)// == NULL)
        nParam = e->NParam(2);

      DStructDesc* nStructDesc = new DStructDesc("$truct");
      // instance takes care of nStructDesc since it is unnamed
      //  dimension dim( 1);
      //  DStructGDL* instance = new DStructGDL( nStructDesc, dim);
      DStructGDL* instance = new DStructGDL(nStructDesc);
      Guard<DStructGDL> instance_guard(instance);

      for (SizeT p = 0; p < nParam;) {
        BaseGDL* par = e->GetParDefined(p);
        //      DStructGDL* parStruct = dynamic_cast<DStructGDL*>( par);
        //      if( parStruct != NULL)
        if (par->Type() == GDL_STRUCT) {
          // add struct
          DStructGDL* parStruct = static_cast<DStructGDL*> (par);
          if (!parStruct->Scalar())
            e->Throw("Expression must be a scalar in this context: " +
            e->GetParString(p));

          DStructDesc* desc = parStruct->Desc();
          for (SizeT t = 0; t < desc->NTags(); ++t) {
            instance->NewTag(desc->TagName(t), parStruct->GetTag(t)->Dup());
          }
          ++p;
        } else {
          // add tag value pair
          DStringGDL* tagNames = e->GetParAs<DStringGDL>(p);
          SizeT nTags = tagNames->N_Elements();

          SizeT tagStart = p + 1;
          SizeT tagEnd = p + nTags;
          if (tagEnd >= nParam)
            e->Throw("Incorrect number of arguments.");

          for (++p; p <= tagEnd; ++p) {
            BaseGDL* value = e->GetParDefined(p);

            // add
            instance->NewTag(TagName(e, (*tagNames)[ p - tagStart]),  value->Dup());
          }
        }
      }

      instance_guard.release();
      return instance;
    }
  }

  BaseGDL* rotate(EnvT* e) {
    e->NParam(2);
    BaseGDL* p0 = e->GetParDefined(0);

    if (p0->Rank() == 0)
      e->Throw("Expression must be an array in this context: " + e->GetParString(0));

    if (p0->Rank() != 1 && p0->Rank() != 2)
      e->Throw("Only 1 or 2 dimensions allowed: " + e->GetParString(0));

    if (p0->Type() == GDL_STRUCT)
      e->Throw("STRUCT expression not allowed in this context: " +
      e->GetParString(0));

    DLong dir;
    e->AssureLongScalarPar(1, dir);

    return p0->Rotate(dir);
  }

  // SA: based on the code of rotate() (above)

  BaseGDL* reverse(EnvT* e) {
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
    static int overwriteIx = e->KeywordIx("OVERWRITE");
    if (e->KeywordSet(overwriteIx)) {
      p0->Reverse(dim - 1);
      bool stolen = e->StealLocalPar(0);
      //    if( !stolen)
      //    e->GetPar(0) = NULL;
      if (!stolen)
        e->SetPtrToReturnValue(&e->GetPar(0));
      return p0;
    } else
      ret = p0->DupReverse(dim - 1);
    return ret;
  }

  // PARSE_URL based on the IDL parse_url function behaviour and documentation

  BaseGDL* parse_url(EnvT* env) {
    // sanity check for number of parameters
    SizeT nParam = env->NParam();

    // 1-nd argument : the url string
    DString url;
    env->AssureScalarPar<DStringGDL>(0, url);

    // creating the output anonymous structure
    DStructDesc* urlstru_desc = new DStructDesc("$truct");
    SpDString aString;
    urlstru_desc->AddTag("SCHEME", &aString);
    urlstru_desc->AddTag("USERNAME", &aString);
    urlstru_desc->AddTag("PASSWORD", &aString);
    urlstru_desc->AddTag("HOST", &aString);
    urlstru_desc->AddTag("PORT", &aString);
    urlstru_desc->AddTag("PATH", &aString);
    urlstru_desc->AddTag("QUERY", &aString);
    DStructGDL* urlstru = new DStructGDL(urlstru_desc, dimension());
    Guard<DStructGDL> urlstru_guard(urlstru);

    char const *str = url.c_str();
    size_t length = url.length();
    char const*pStart, *pMid, *pEnd;

    // initialise PORT at 80
    urlstru->InitTag("PORT", DStringGDL("80"));

    //searching for the scheme and exciting if not found
    pStart = str;
    if (!(pEnd = std::strstr(str, "://"))) {
      urlstru_guard.release();
      return (urlstru);
    }
    urlstru->InitTag("SCHEME", DStringGDL(pStart < pEnd ? string(pStart, pEnd - pStart) : ""));

    // setting pStart after "://"
    pEnd += 3;
    pStart = pEnd;

    //searching for the username and password (':' & '@')
    if (std::strchr(pStart, '@')) {
      pEnd = std::strchr(pStart, '@');
      if (!(pMid = std::strchr(pStart, ':')))
        pMid = pEnd;
      if (pMid && pMid < pEnd)
        urlstru->InitTag("PASSWORD", DStringGDL(pMid + 1 < pEnd ? string(pMid + 1, pEnd - (pMid + 1)) : ""));
      urlstru->InitTag("USERNAME", DStringGDL(pStart < pMid ? string(pStart, pMid - pStart) : ""));
      pStart = pEnd + 1;
    }

    // setting pEnd at the first '/' found or at the end if not found
    if (std::strchr(pStart, '/')) {
      pEnd = std::strchr(pStart, '/');
    } else {
      pEnd = pStart + std::strlen(pStart);
    }

    // setting pMid at the first ':' found or at the end if not found
    // if found : InitTag "PORT" from pMid + 1 (after ':') to pEnd ('/' or END)
    if (std::strchr(pStart, ':')) {
      pMid = std::strchr(pStart, ':');
      urlstru->InitTag("PORT", DStringGDL(pMid + 1 < pEnd ? string(pMid + 1, pEnd - (pMid + 1)) : ""));
    } else {
      pMid = pEnd;
    }
    // InitTag "PORT" from pStart(after "://" or '@') to pMid (':' or '/' or END)
    urlstru->InitTag("HOST", DStringGDL(pStart < pMid ? string(pStart, pMid - pStart) : ""));
    pStart = pEnd + 1;
    // Searching for a query ('?')
    // if found : InitTag "QUERY" from pEnd + 1 (after '?') to the end
    if ((pEnd = strchr(pMid, '?'))) {
      urlstru->InitTag("QUERY", DStringGDL(std::strlen(pEnd + 1) > 0 ? string(pEnd + 1, std::strlen(pEnd + 1)) : ""));
    } else {
      pEnd = pMid + std::strlen(pMid);
    }
    // InitTag "PATH" from pStart (after '/') to the end
    urlstru->InitTag("PATH", DStringGDL(pStart < pEnd ? string(pStart, pEnd - pStart) : ""));
    urlstru_guard.release();
    return urlstru;
  }

  BaseGDL* locale_get(EnvT* e) {
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

  BaseGDL* command_line_args_fun(EnvT* e) {
    static int countIx = e->KeywordIx("COUNT");
    static int resetIx = e->KeywordIx("RESET");
    static int setIx = e->KeywordIx("SET");
    // resetting the command_line_args
    if (e->KeywordSet(resetIx)) command_line_args.clear();

    BaseGDL* setKW = e->GetKW(setIx);
    if (setKW != NULL) {
      if (setKW->Type() != GDL_STRING)
        e->Throw(" SET string values only allowed ");
      DString setp;
      for (SizeT i = 0; i < setKW->N_Elements(); i++) {
        setp = (*static_cast<DStringGDL*> (setKW))[i];
        command_line_args.push_back(setp);
      }
      //          printf(" SET: %s \n", (*static_cast<DStringGDL*>(setKW))[i] )
      //          command_line_args.push_back( ( (*static_cast<DStringGDL*>(setKW))[i] );

    }
    // setting the COUNT keyword value
    if (e->KeywordPresent(countIx)) {
      e->AssureGlobalKW(countIx);
      e->SetKW(countIx, new DLongGDL(command_line_args.size()));
    }

    // returning empty string or an array of arguments
    if (command_line_args.empty()) return new DStringGDL("");
    else {
      BaseGDL* ret = new DStringGDL(dimension(command_line_args.size()),BaseGDL::NOZERO);
      for (size_t i = 0; i < command_line_args.size(); i++)
        (*static_cast<DStringGDL*> (ret))[i] = command_line_args[i];
      return ret;
    }
  }

  // SA: relies in the uname() from libc (must be there if POSIX)

  BaseGDL* get_login_info(EnvT* e) {
    // getting the info
#ifdef _WIN32
#define MAX_WCHAR_BUF 256

    char login[MAX_WCHAR_BUF];
    char info[MAX_WCHAR_BUF];

    DWORD N_WCHAR = MAX_WCHAR_BUF;

    WCHAR w_buf[MAX_WCHAR_BUF];
    GetUserNameW(w_buf, &N_WCHAR);
    WideCharToMultiByte(CP_ACP, 0, w_buf, N_WCHAR, login, N_WCHAR, NULL, NULL);
    GetComputerNameW(w_buf, &N_WCHAR);
    WideCharToMultiByte(CP_ACP, 0, w_buf, N_WCHAR, info, N_WCHAR, NULL, NULL);
#else
    char* login = getlogin();
    if (login == NULL) e->Throw("Failed to get user name from the OS");
    struct utsname info;
    if (0 != uname(&info)) e->Throw("Failed to get machine name from the OS");
#endif
    // creating the output anonymous structure
    DStructDesc* stru_desc = new DStructDesc("$truct");
    SpDString aString;
    stru_desc->AddTag("MACHINE_NAME", &aString);
    stru_desc->AddTag("USER_NAME", &aString);
    DStructGDL* stru = new DStructGDL(stru_desc, dimension());

    // returning the info
    stru->InitTag("USER_NAME", DStringGDL(login));
#ifdef _WIN32
    stru->InitTag("MACHINE_NAME", DStringGDL(info));
#else
    stru->InitTag("MACHINE_NAME", DStringGDL(info.nodename));
#endif
    return stru;
  }

  // SA: base64 logic in base64.hpp, based on code by Bob Withers (consult base64.hpp)

  BaseGDL* idl_base64(EnvT* e) {
    BaseGDL* p0 = e->GetPar(0);
    if (p0 != NULL) {
      if (p0->Rank() == 0 && p0->Type() == GDL_STRING) {
        // decoding
        string* str = &((*static_cast<DStringGDL*> (p0))[0]);
        if (str->length() == 0) return new DByteGDL(0);
        if (str->length() % 4 != 0)
          e->Throw("Input string length must be a multiple of 4");
        unsigned int retlen = base64::decodeSize(*str);
        if (retlen == 0 || retlen > str->length()) e->Throw("No data in the input string");
        DByteGDL* ret = new DByteGDL(dimension(retlen),BaseGDL::NOZERO);
        if (!base64::decode(*str, (char*) &((*ret)[0]), ret->N_Elements())) {
          delete ret;
          e->Throw("Base64 decoder failed");
          return NULL;
        }
        return ret;
      }
      if (p0->Rank() >= 1 && p0->Type() == GDL_BYTE) {
        // encoding
        return new DStringGDL(
          base64::encode((char*) &(*static_cast<DByteGDL*> (p0))[0], p0->N_Elements())
          );
      }
    }
    e->Throw("Expecting string or byte array as a first parameter");
    return NULL; //pacify dumb compilers
  }

  BaseGDL* get_drive_list(EnvT* e) {
    if (e->KeywordPresent(0)) e->SetKW(0, new DLongGDL(0));
    return new DStringGDL("");
  }

  BaseGDL* scope_level(EnvT* e) {
    SizeT nParam = e->NParam();
    if (nParam > 0) e->Throw("Incorrect number of arguments.");
    EnvStackT& callStack = e->Interpreter()->CallStack();
    return new DLongGDL(callStack.size());
  }

  // based on void SimpleDumpStack(EnvT* e) used in "basic_pro.cpp"

  BaseGDL* scope_traceback(EnvT* e) {
    static int structureIx = e->KeywordIx("STRUCTURE");
    bool structureKW = e->KeywordSet(structureIx);

    static int systemIx = e->KeywordIx("SYSTEM");
    bool systemKW = e->KeywordSet(systemIx);
    if (systemKW) {
      Warning("keyword SYSTEM is not ready here, please contribute !");
    }

    int debug = 0;

    EnvStackT& callStack = e->Interpreter()->CallStack();
    long actIx = callStack.size();

    if (debug) cout << "actIx : " << actIx << endl;

    string tmp, filename;
    int lineNumber;

    if (!structureKW) {

      DStringGDL* res;
      res = new DStringGDL(dimension(actIx), BaseGDL::NOZERO);

      for (SizeT i = 0; i < actIx; ++i) {
        EnvStackT::pointer_type upEnv = callStack[i];
        tmp = upEnv->GetProName();
        filename = upEnv->GetFilename();
        if (filename != "") {
          lineNumber = upEnv->GetLineNumber();
          if (lineNumber != 0) {
            tmp = tmp + " <" + filename + "(" + i2s(lineNumber) + ")>";
          }
        }
        if (debug) cout << tmp << endl;
        (*res)[i] = tmp;
      }

      return res;
    }

    if (structureKW) {
      DStructGDL* res = new DStructGDL(
        FindInStructList(structList, "IDL_TRACEBACK"),
        dimension(actIx));

      int tRoutine, tFilename, tLine, tLevel, tFunction;
      int tMethod = 0, tRestored = 0, tSystem = 0;

      for (SizeT i = 0; i < actIx; ++i) {

        EnvStackT::pointer_type upEnv = callStack[i];
        tmp = upEnv->GetProName();
        filename = upEnv->GetFilename();
        if (filename.length() == 0) filename = " ";
        lineNumber = upEnv->GetLineNumber();

        tRoutine = res->Desc()->TagIndex("ROUTINE");
        tFilename = res->Desc()->TagIndex("FILENAME");
        tLine = res->Desc()->TagIndex("LINE");
        tLevel = res->Desc()->TagIndex("LEVEL");
        tFunction = res->Desc()->TagIndex("IS_FUNCTION");
        tMethod = res->Desc()->TagIndex("METHOD");
        tRestored = res->Desc()->TagIndex("RESTORED");
        tSystem = res->Desc()->TagIndex("SYSTEM");

        *(res->GetTag(tRoutine, i)) = DStringGDL(tmp);
        *(res->GetTag(tFilename, i)) = DStringGDL(filename);
        *(res->GetTag(tLine, i)) = DLongGDL(lineNumber);
        *(res->GetTag(tLevel, i)) = DLongGDL(i);

        // AC 2015/03/03 : HELP WELCOME
        // I don't know how to know if we use Pro or Func
        // we do have a long way in "dinterpreter.cpp" with
        // if( firstChar == "#")
        bool isFunc = false;
        for (FunListT::iterator ifunc = funList.begin(); ifunc != funList.end(); ++ifunc) {
          if (StrUpCase(tmp).find((*ifunc)->ObjectName()) != std::string::npos) {
            isFunc = true;
            break;
          }
        }
        *(res->GetTag(tFunction, i)) = (isFunc) ? DByteGDL(1) : DByteGDL(0);
        //all others 0 for the time being
        *(res->GetTag(tMethod, i)) = DByteGDL(0);
        *(res->GetTag(tRestored, i)) = DByteGDL(0);
        *(res->GetTag(tSystem, i)) = DByteGDL(0);
      }
      return res;
    }
    return NULL; //pacify, etc.
  }

  // note: changes here MUST be reflected in scope_varfetch_reference() as well
  // because DLibFun of this function is used for scope_varfetch_reference() the keyword
  // indices must match

  BaseGDL* scope_varfetch_value(EnvT* e) {
    SizeT nParam = e->NParam(1);

    EnvStackT& callStack = e->Interpreter()->CallStack();
    //     DLong curlevnum = callStack.size()-1;
    // 'e' is not on the stack
    DLong curlevnum = callStack.size();

    static int levelIx = e->KeywordIx("LEVEL");
    static int enterIx = e->KeywordIx("ENTER");
    bool acceptNew = e->KeywordSet(enterIx);
    static int refextraIx = e->KeywordIx("REF_EXTRA");
    bool doesRefExtra=e->KeywordSet(refextraIx);
    static int commonIx = e->KeywordIx("COMMON");
    bool common=e->KeywordSet(commonIx);
    
    DLongGDL* level = e->IfDefGetKWAs<DLongGDL>(levelIx);

    DLong desiredlevnum = 0;
    if (level != NULL) desiredlevnum = (*level)[0];
    if (desiredlevnum <= 0) desiredlevnum += curlevnum;
    if (desiredlevnum < 1) desiredlevnum = 1;
    else if (desiredlevnum > curlevnum) desiredlevnum = curlevnum;
    
    //a few checks:
    if ( common && (level != NULL) ) e->Throw("Conflicting keywords.");
    if ( common &&  acceptNew ) e->Throw("Conflicting keywords.");
    if ( common &&  doesRefExtra ) e->Throw("Conflicting keywords.");
    if ( acceptNew &&  doesRefExtra ) e->Throw("Conflicting keywords.");


    if (doesRefExtra) {
      DString varName="";
      e->AssureScalarPar<DStringGDL>(0, varName);
      StrUpCaseInplace(varName);
      //in this case it is not a varname passed, but a kwname. 
      //We need to find the associated varname at the point it was passed with the kwname
      // currentlevnum is this function scope_varfetch_value
      //previous is caller, probably contains _REF_EXTRA=xxx
      //GetRefExtraList(Kwname) should go back in stack to the point where kwname=varName was passed.  
      BaseGDL* var = ((EnvT*) (callStack[desiredlevnum - 1]))->GetRefExtraList(varName);
      if (var == NULL) e->Throw("Variable not found: " + varName);
      else return var->Dup();
    }
    
    if (common) {
      DString commonName = "";
      e->AssureStringScalarKW(commonIx, commonName);
      StrUpCaseInplace(commonName);
      CommonListT::iterator it;
      for (it = commonList.begin(); it != commonList.end(); ++it) {
        if ((*it)->Name() == commonName) {
          SizeT nComm = (*it)->NVar();
          BaseGDL* p0 = e->GetPar(0);
          if (p0->Type() == GDL_STRING) {
            DString s = "";
            e->AssureStringScalarPar(0, s);
            StrUpCaseInplace(s);
            for (auto i = 0; i < nComm; ++i) {
              if ((*it)->VarName(i) == s) {
                BaseGDL* ret = (*it)->Var(i)->Data();
                if (ret == NULL) e->Throw("Variable is undefined: <No name>."); //must THROW on undefined value
                return ret;
              }
            }
            e->Throw("Variable not found: " + s);
          } else { //number
            DLong ipar = 0;
            e->AssureLongScalarPar(0, ipar);
            if ((ipar >= 0) && (ipar < nComm)) {
              BaseGDL* ret = (*it)->Var(ipar)->Data();
              if (ret == NULL) e->Throw("Variable is undefined: <No name>."); //must THROW on undefined value
              return ret;
            }
            e->Throw("Attempt to extend common block: " + commonName);
          }
        }
      }
    }
    
    // not Common: normal name
    DString varName;
    e->AssureScalarPar<DStringGDL>(0, varName);
    varName = StrUpCase(varName);
    DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum - 1]->GetPro());
    SizeT nVar = pro->Size(); // # var in GDL for desired level
    int nKey = pro->NKey();
    int xI = pro->FindVar(varName);
    if (xI != -1) {
      //       BaseGDL*& par = ((EnvT*)(callStack[desiredlevnum-1]))->GetPar( xI);
      BaseGDL*& par = callStack[desiredlevnum - 1]->GetTheKW(xI);

      if (par == NULL) e->Throw("Variable is undefined: " + varName);

      return par->Dup();
    } else if (acceptNew) {
      SizeT u = pro->AddVar(varName);
      SizeT s = callStack[desiredlevnum - 1]->AddEnv();
      BaseGDL*& par = ((EnvT*) (callStack[desiredlevnum - 1]))->GetPar(s - nKey);
      return par->Dup();
    }
    e->Throw("Variable not found: " + varName);
    return new DLongGDL(0); // compiler shut-up
  }

  // this routine is special, only called as an l-function (from FCALL_LIB::LEval())
  // it MUST use an EnvT set up for scope_varfetch_value

  BaseGDL** scope_varfetch_reference(EnvT* e) {
    SizeT nParam = e->NParam();

    EnvStackT& callStack = e->Interpreter()->CallStack();
    //     DLong curlevnum = callStack.size()-1;
    // 'e' is not on the stack
    DLong curlevnum = callStack.size();

    static int levelIx = e->KeywordIx("LEVEL");
    static int enterIx = e->KeywordIx("ENTER");
    bool acceptNew = e->KeywordSet(enterIx);
    static int refextraIx = e->KeywordIx("REF_EXTRA");
    bool doesRefExtra = e->KeywordSet(refextraIx);
    static int commonIx = e->KeywordIx("COMMON");
    bool common = e->KeywordSet(commonIx);

    DLongGDL* level = e->IfDefGetKWAs<DLongGDL>(levelIx);

    DLong desiredlevnum = 0;
    if (level != NULL) desiredlevnum = (*level)[0];
    if (desiredlevnum <= 0) desiredlevnum += curlevnum;
    if (desiredlevnum < 1) desiredlevnum = 1;
    else if (desiredlevnum > curlevnum) desiredlevnum = curlevnum;

    //a few checks:
    if (common && (level != NULL)) e->Throw("Conflicting keywords.");
    if (common && acceptNew) e->Throw("Conflicting keywords.");
    if (common && doesRefExtra) e->Throw("Conflicting keywords.");
    if (acceptNew && doesRefExtra) e->Throw("Conflicting keywords.");


    if (doesRefExtra) {
      DString varName;
      e->AssureScalarPar<DStringGDL>(0, varName);
      StrUpCaseInplace(varName);
      //in this case it is not a varname passed, but a kwname. 
      //We need to find the associated varname at the point it was passed with the kwname
      // currentlevnum is this function scope_varfetch_value
      //previous is caller, probably contains _REF_EXTRA=xxx
      //GetRefExtraList(Kwname) should go back in stack to the point where kwname=varName was passed.  
      BaseGDL** var = ((EnvT*) (callStack[desiredlevnum - 1]))->GetRefExtraListPtr(varName);
      if (var == NULL) e->Throw("Variable not found: " + varName);
      else return var;
    }

    if (common) {
      DString commonName = "";
      e->AssureStringScalarKW(commonIx, commonName);
      StrUpCaseInplace(commonName);
      CommonListT::iterator it;
      for (it = commonList.begin(); it != commonList.end(); ++it) {
        if ((*it)->Name() == commonName) {
          SizeT nComm = (*it)->NVar();
          BaseGDL* p0 = e->GetPar(0);
          if (p0->Type() == GDL_STRING) {
            DString s="";
            e->AssureStringScalarPar(0,s);
            StrUpCaseInplace(s);
            for (auto i = 0; i < nComm; ++i) {
              if ((*it)->VarName(i) == s) {
                BaseGDL* ret=(*it)->Var(i)->Data();
                // if (ret == NULL) e->Throw("Variable is undefined: <No name>."); //does NOT THROW
                return &((*it)->Var(i)->Data());
              }
            }
            e->Throw("Variable not found: " + s);
          } else { //number
            DLong ipar = 0;
            e->AssureLongScalarPar(0, ipar);
            if ((ipar >= 0) && (ipar < nComm)) {
              BaseGDL* ret=(*it)->Var(ipar)->Data();
              // if (ret == NULL) e->Throw("Variable is undefined: <No name>."); //does not THROW
              return &((*it)->Var(ipar)->Data());
            }
            e->Throw("Attempt to extend common block: " + commonName);
          }
        }
      }
    }

    // not Common: normal name
    DString varName;
    e->AssureScalarPar<DStringGDL>(0, varName);
    varName = StrUpCase(varName);
    DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum - 1]->GetPro());
    SizeT nVar = pro->Size(); // # var in GDL for desired level
    int nKey = pro->NKey();
    int xI = pro->FindVar(varName);
    if (xI != -1) {
      //       BaseGDL*& par = ((EnvT*)(callStack[desiredlevnum-1]))->GetPar( xI);
      BaseGDL*& par = callStack[desiredlevnum - 1]->GetTheKW(xI);

      //       if( par == NULL)
      //    e->Throw( "Variable is undefined: " + varName);

      return &par;
    } else if (acceptNew) {
      SizeT u = pro->AddVar(varName);
      SizeT s = callStack[desiredlevnum - 1]->AddEnv();
      BaseGDL*& par = ((EnvT*) (callStack[desiredlevnum - 1]))->GetPar(s - nKey);
      return &par;
    }
    e->Throw("LVariable not found: " + varName);
    return NULL; // compiler shut-up
  }
  
  BaseGDL* scope_varname_fun(EnvT* e) {

    SizeT nParam = e->NParam();

    EnvStackT& callStack = e->Interpreter()->CallStack();

    DLong currentLvl = callStack.size();
    DLong level = currentLvl; // default to current level

    DStringGDL* retVal = nullptr;
    SizeT count(0);

    static int commonIx = e->KeywordIx("COMMON");
    static int countIx = e->KeywordIx("COUNT");
    static int levelIx = e->KeywordIx("LEVEL");

    if (e->KeywordSet(commonIx)) {
      if (e->KeywordSet(levelIx)) e->Throw("Conflicting keywords.");
      DString commonName = "";
      e->AssureStringScalarKW(commonIx, commonName);
      StrUpCaseInplace(commonName);
      CommonListT::iterator it;
      bool absent=true;
      for (it = commonList.begin(); it != commonList.end(); ++it) {
        if ((*it)->Name() == commonName ) {
          absent=false;
          bool passed_list = true;
          SizeT nComm = (*it)->NVar();
          if (nParam < 1) {
            nParam = nComm;
            passed_list = false;
          }
          count = nParam;
          retVal = new DStringGDL(dimension(count), BaseGDL::NOZERO);
          for (auto i=0; i < count; ++i) {
            DLong ipar = i;
            if (passed_list) e->AssureLongScalarPar(i, ipar);
            if ((ipar >= 0) && (ipar < nComm)) {
              (*retVal)[i] = (*it)->VarName(ipar);
            } else ( *retVal)[i] = "";
          }
        }
      }
      if (absent) e->Throw("Common block does not exist: " + commonName);
    } else {

      DLongGDL* kwLvl = e->IfDefGetKWAs<DLongGDL>(levelIx);
      if (kwLvl) {
        DLong tmp = (*kwLvl)[0];
        if (tmp > 0) level = tmp;
        else level += tmp;
        level = std::max(std::min(level, currentLvl), 1);
      }



      if (nParam == 0) { // Just list and return all defined parameters at the requested level.
        EnvT* requestedScope = (EnvT*) callStack[level - 1];
        DSubUD* scope_pro = static_cast<DSubUD*> (requestedScope->GetPro());
        SizeT scope_nVar = scope_pro->Size();
        SizeT scope_nComm = scope_pro->CommonsSize();
        count = scope_nVar + scope_nComm;
        if (!count) {
          retVal = new DStringGDL("");
        } else { // N.B. Order doesn't matter since the result is lexically sorted.
          vector<string> names(count);
          for (SizeT i(0); i < scope_nVar; ++i) {
            names[ i ] = scope_pro->GetVarName(i);
            if (names[ i ].empty()) names[ i ] = "*";
          }
          if (scope_nComm) {
            DStringGDL* list = static_cast<DStringGDL*> (scope_pro->GetCommonVarNameList());
            for (SizeT i(0); i < list->N_Elements(); ++i) {
              names[ scope_nVar + i ] = (*list)[i];
            }
          }
          std::sort(names.begin(), names.end());
          retVal = new DStringGDL(dimension(count), BaseGDL::NOZERO);
          for (SizeT i(0); i < count; ++i) {
            (*retVal)[i] = names[i];
          }
        }
      } else {
        EnvT* requestedScope = (EnvT*) callStack[level - 1];
        DSubUD* scope_pro = static_cast<DSubUD*> (requestedScope->GetPro());
        SizeT scope_nVar = scope_pro->Size();
        SizeT scope_nComm = scope_pro->CommonsSize();
        count = nParam;
        retVal = new DStringGDL(dimension(nParam), BaseGDL::NOZERO);
        //retrieve each variable at current level, fetch name at desired level
        for (SizeT i(0); i < nParam; ++i) {
          (*retVal)[i] = ""; // not found
          BaseGDL*& par = e->GetPar(i);
          std::string tmp_name;
          bool undefineOnExit = false;
          //DANGEROUS trick to get parameter name, not <undefined> : avoid to have par=0x0 = NULL
          if (par == NULL) {
            e->SetPar(i, NullGDL::GetSingleInstance()); //make it something not meaningful
            par = e->GetPar(i);
            undefineOnExit = true;
          }
          if (scope_pro->GetCommonVarName(par, tmp_name)) { // Variable found in common-block, so use that name first.
            (*retVal)[i] = tmp_name;

            if (undefineOnExit) {
              par = NULL; //PROBABLY WILL CREATE PROBLEM SOMEWHERE ELSE
            };
            continue;
          }
          // not defined in common, can only be local.
          if (level == currentLvl) { // For current level we need to resolve using e, but return empty string if not a named variable (ex: expression)
            tmp_name = e->GetParString(i);
            if (tmp_name.find('>') == std::string::npos) (*retVal)[i] = tmp_name;
            if (undefineOnExit) {
              par = NULL; //PROBABLY WILL CREATE PROBLEM SOMEWHERE ELSE
            };
          } else {
            tmp_name = requestedScope->GetString(par);
            if (tmp_name.find('>') == std::string::npos) (*retVal)[i] = tmp_name;
          }
        }
      }

    }

    // set the COUNT keyword
    if (e->KeywordPresent(countIx)) {
      e->AssureGlobalKW(countIx);
      e->SetKW(countIx, new DLongGDL(count));
    }

    return retVal;

  }

} // namespace

