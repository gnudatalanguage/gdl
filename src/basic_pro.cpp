/***************************************************************************
                          basic_pro.cpp  -  basic GDL library procedures
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net

    - UNIT keyword for SPAWN by Greg Huey
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

#include <sys/types.h>

#include <string>
#include <fstream>
#include <memory>
#include <set>
#include <iterator>

#include <sys/stat.h>

#ifndef _WIN32
//#include <regex.h> // stregex
#include <fnmatch.h>
#include <sys/wait.h>
#else
#include <shlwapi.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#include "dinterpreter.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "io.hpp"
#include "basic_pro.hpp"
#include "semshm.hpp"
#include "graphicsdevice.hpp"

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#include <ext/stdio_filebuf.h> // TODO: is it portable across compilers?
#endif
#include <signal.h>
static bool trace_me(false);

namespace lib {

  using namespace std;
	DString GetCWD(); // From file.cpp
  // control !CPU settings

  void cpu(EnvT* e) {
    static int resetIx = e->KeywordIx("RESET");
    static int restoreIx = e->KeywordIx("RESTORE");
    static int max_eltsIx = e->KeywordIx("TPOOL_MAX_ELTS");
    static int min_eltsIx = e->KeywordIx("TPOOL_MIN_ELTS");
    static int nThreadsIx = e->KeywordIx("TPOOL_NTHREADS");
    static int vectorEableIx = e->KeywordIx("VECTOR_ENABLE");

    bool reset = e->KeywordSet(resetIx);
    bool restore = e->KeywordSet(restoreIx);
    if ((reset) && (restore)) e->Throw("Conflicting keywords (/reset and /restore).");

    bool vectorEnable = e->KeywordSet(vectorEableIx);

    DLong NbCOREs = 1;
#ifdef _OPENMP
    NbCOREs = omp_get_num_procs();
#endif

    DLong locCpuTPOOL_NTHREADS = CpuTPOOL_NTHREADS;
    DLong64 locCpuTPOOL_MIN_ELTS = CpuTPOOL_MIN_ELTS;
    DLong64 locCpuTPOOL_MAX_ELTS = CpuTPOOL_MAX_ELTS;

    // reading the Tag Index of the variable parts in !CPU
    DStructGDL* cpu = SysVar::Cpu();
    static unsigned NTHREADSTag = cpu->Desc()->TagIndex("TPOOL_NTHREADS");
    static unsigned TPOOL_MIN_ELTSTag = cpu->Desc()->TagIndex("TPOOL_MIN_ELTS");
    static unsigned TPOOL_MAX_ELTSTag = cpu->Desc()->TagIndex("TPOOL_MAX_ELTS");

    if (reset) {
      locCpuTPOOL_NTHREADS = NbCOREs;
      locCpuTPOOL_MIN_ELTS = DefaultTPOOL_MIN_ELTS;
      locCpuTPOOL_MAX_ELTS = DefaultTPOOL_MAX_ELTS;
    } else if (e->KeywordPresent(restoreIx)) {
      DStructGDL* restoreCpu = e->GetKWAs<DStructGDL>(restoreIx);

      if (restoreCpu->Desc() != cpu->Desc())
        e->Throw("RESTORE must be set to an instance with the same struct layout as {!CPU}");

      locCpuTPOOL_NTHREADS = (*(static_cast<DLongGDL*> (restoreCpu->GetTag(NTHREADSTag, 0))))[0];
      locCpuTPOOL_MIN_ELTS = (*(static_cast<DLong64GDL*> (restoreCpu->GetTag(TPOOL_MIN_ELTSTag, 0))))[0];
      locCpuTPOOL_MAX_ELTS = (*(static_cast<DLong64GDL*> (restoreCpu->GetTag(TPOOL_MAX_ELTSTag, 0))))[0];
    } else {
      if (e->KeywordPresent(nThreadsIx)) {
        e->AssureLongScalarKW(nThreadsIx, locCpuTPOOL_NTHREADS);
      }
      if (e->KeywordPresent(min_eltsIx)) {
        e->AssureLongScalarKW(min_eltsIx, locCpuTPOOL_MIN_ELTS);
      }
      if (e->KeywordPresent(max_eltsIx)) {
        e->AssureLongScalarKW(max_eltsIx, locCpuTPOOL_MAX_ELTS);
      }
    }

    // update here all together in case of error

#ifdef _OPENMP
    //cout <<locCpuTPOOL_NTHREADS << " " << CpuTPOOL_NTHREADS << endl;
    if (locCpuTPOOL_NTHREADS > 0) {
      CpuTPOOL_NTHREADS = locCpuTPOOL_NTHREADS;
    } else {
      CpuTPOOL_NTHREADS = NbCOREs;
    }
    if (CpuTPOOL_NTHREADS > NbCOREs)
      Warning("CPU : Warning: Using more threads (" + i2s(CpuTPOOL_NTHREADS) + ") than the number of CPUs in the system (" + i2s(NbCOREs) + ") will degrade performance.");
#else
    CpuTPOOL_NTHREADS = 1;
#endif
    if (locCpuTPOOL_MIN_ELTS >= 0) CpuTPOOL_MIN_ELTS = locCpuTPOOL_MIN_ELTS;
    if (locCpuTPOOL_MAX_ELTS >= 0) CpuTPOOL_MAX_ELTS = locCpuTPOOL_MAX_ELTS;

    // update !CPU system variable
    (*static_cast<DLongGDL*> (cpu->GetTag(NTHREADSTag, 0)))[0] = CpuTPOOL_NTHREADS;
    (*static_cast<DLong64GDL*> (cpu->GetTag(TPOOL_MIN_ELTSTag, 0)))[0] = CpuTPOOL_MIN_ELTS;
    (*static_cast<DLong64GDL*> (cpu->GetTag(TPOOL_MAX_ELTSTag, 0)))[0] = CpuTPOOL_MAX_ELTS;

#ifdef _OPENMP
    omp_set_num_threads(CpuTPOOL_NTHREADS);
#endif
  }

  void exitgdl(EnvT* e) {

#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)

    // we manage the ASCII "history" file (located in ~/.gdl/)
    // we do not manage NOW the number of lines we save,
    // this should be limited by "history/readline" itself

    if (historyIntialized) {
      // Create eventually the ".gdl" path in user $HOME
      int result, debug = 0;
      char *homeDir = getenv("HOME");
      if (homeDir != NULL) {
        string pathToGDL_history = homeDir;
        AppendIfNeeded(pathToGDL_history, "/");
        pathToGDL_history += ".gdl";
        // Create eventially the ".gdl" path in Home
#ifdef _WIN32
        result = mkdir(pathToGDL_history.c_str());
#else
        result = mkdir(pathToGDL_history.c_str(), 0700);
#endif
        if (debug) {
          if (result == 0) cout << "Creation of ~/.gdl PATH " << endl;
          else cout << "~/.gdl PATH was still here " << endl;
        }

        // (over)write the history file in ~/.gdl PATH

        AppendIfNeeded(pathToGDL_history, "/");
        string history_filename = pathToGDL_history + "history";
        if (debug) cout << "History file name: " << history_filename << endl;
        result = write_history(history_filename.c_str());
        if (debug) {
          if (result == 0) cout << "Successfull writing of ~/.gdl/history" << endl;
          else cout << "Fail to write ~/.gdl/history" << endl;
        }
      }
    }
#endif

    sem_onexit();

    BaseGDL* status = e->GetKW(1);
    if (status == NULL) exit(EXIT_SUCCESS);

    if (!status->Scalar())
      e->Throw("Expression must be a scalar in this context: " +
      e->GetString(status));

    DLongGDL* statusL = static_cast<DLongGDL*> (status->Convert2(GDL_LONG,
      BaseGDL::COPY));

    DLong exit_status;
    statusL->Scalar(exit_status);
    exit(exit_status);
  }

  void heap_gc(EnvT* e) {
    static int objIx = e->KeywordIx("OBJ");
    static int ptrIx = e->KeywordIx("PTR");
    static int verboseIx = e->KeywordIx("VERBOSE");
    bool doObj = e->KeywordSet(objIx);
    bool doPtr = e->KeywordSet(ptrIx);
    bool verbose = e->KeywordSet(verboseIx);
    if (!doObj && !doPtr)
      doObj = doPtr = true;

    e->HeapGC(doPtr, doObj, verbose);
      if( GDLInterpreter::HeapSize() == 0 and (GDLInterpreter::ObjHeapSize() == 0)  )
				GDLInterpreter::ResetHeap();
  }

  void HeapFreeObj(EnvT* env, BaseGDL* var, bool verbose) {
    if (var == NULL)
      return;
    if (var->Type() == GDL_STRUCT) {
      DStructGDL* varStruct = static_cast<DStructGDL*> (var);
      DStructDesc* desc = varStruct->Desc();
      for (SizeT e = 0; e < varStruct->N_Elements(); ++e)
        for (SizeT t = 0; t < desc->NTags(); ++t) {
          BaseGDL* actElementTag = varStruct->GetTag(t, e);
          HeapFreeObj(env, actElementTag, verbose);
        }
    } else if (var->Type() == GDL_PTR) {
      // descent into pointer
      DPtrGDL* varPtr = static_cast<DPtrGDL*> (var);
      for (SizeT e = 0; e < varPtr->N_Elements(); ++e) {
        DPtr actPtrID = (*varPtr)[e];
		  if( !DInterpreter::PtrValid(actPtrID)) continue;

	      BaseGDL* derefPtr = DInterpreter::GetHeap(actPtrID);
        HeapFreeObj(env, derefPtr, verbose);
      }
    } else if (var->Type() == GDL_OBJ) {
      DObjGDL* varObj = static_cast<DObjGDL*> (var);
      for (SizeT e = 0; e < varObj->N_Elements(); ++e) {
        DObj actID = (*varObj)[e];
        if (actID == 0)
          continue;

        if (verbose) {
          BaseGDL* derefObj = DInterpreter::GetObjHeap(actID);
          help_item(cout,
            derefObj, DString("<ObjHeapVar") +
            i2s(actID) + ">",
            false);
        }
        // 2. free object
        env->ObjCleanup(actID);
      }
    }
  }

  void HeapFreePtr(BaseGDL* var, bool verbose) {
    if (var == NULL)
      return;
    if (var->Type() == GDL_STRUCT) {
      DStructGDL* varStruct = static_cast<DStructGDL*> (var);
      DStructDesc* desc = varStruct->Desc();
      for (SizeT e = 0; e < varStruct->N_Elements(); ++e)
        for (SizeT t = 0; t < desc->NTags(); ++t) {
          BaseGDL* actElementTag = varStruct->GetTag(t, e);
          HeapFreePtr(actElementTag, verbose); // recursive call
        }
    } else if (var->Type() == GDL_PTR) {
      // 1. descent into pointer
      DPtrGDL* varPtr = static_cast<DPtrGDL*> (var);
      for (SizeT e = 0; e < varPtr->N_Elements(); ++e) {
        DPtr actPtrID = (*varPtr)[e];


//GJ 2016.05.12 Replaced "if (actPtrID == 0)" for a more restrictive condition:
		  if( !DInterpreter::PtrValid(actPtrID)) continue;

	      BaseGDL* derefPtr = DInterpreter::GetHeap(actPtrID);
	      if (verbose)
        {
          help_item(cout,
            derefPtr, DString("<PtrHeapVar") +
            i2s(actPtrID) + ">",
            false);
        }

	      if (derefPtr == NULL)	continue;
        HeapFreePtr(derefPtr, verbose); // recursive call
      // 2. free pointer
      DInterpreter::FreeHeap(varPtr);
    }
  }
    }

  void heap_free(EnvT* e) {
    static int objIx = e->KeywordIx("OBJ");
    static int ptrIx = e->KeywordIx("PTR");
    static int verboseIx = e->KeywordIx("VERBOSE");
    bool doObj = e->KeywordSet(objIx);
    bool doPtr = e->KeywordSet(ptrIx);
    bool verbose = e->KeywordSet(verboseIx);
    if (!doObj && !doPtr)
      doObj = doPtr = true;

    e->NParam(1);
    BaseGDL* p0 = e->GetParDefined(0);

    if (doObj) // do first objects as they may in turn free some pointers
      HeapFreeObj(e, p0, verbose);
    if (doPtr)
      HeapFreePtr(p0, verbose);
  }

  void ptr_free(EnvT* e) {
    SizeT nParam = e->NParam();
    for (SizeT i = 0; i < nParam; i++) {
      BaseGDL* p = e->GetPar(i);
      if (p == NULL) {
        e->Throw("Pointer type required"
          " in this context: " + e->GetParString(i));
      }
      if (p->Type() != GDL_PTR) {
        e->Throw("Pointer type required"
          " in this context: " + e->GetParString(i));
      }
      DPtrGDL* par = static_cast<DPtrGDL*> (e->GetPar(i));
      e->FreeHeap(par);
    }
  }

  void obj_destroy(EnvT* e) {
    StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());

	int n_Param=e->NParam();
	if( n_Param == 0) return;
	BaseGDL*& par=e->GetPar( 0);
	if( par == NULL or par->Type() != GDL_OBJ) return;
	DObjGDL* op= static_cast<DObjGDL*>(par);

    SizeT nEl = op->N_Elements();
	for( SizeT i=0; i<nEl; i++)	
			e->ObjCleanup( (*op)[i]);
  }

  void call_procedure(EnvT* e) {
    int nParam = e->NParam();
    if (nParam == 0)
      e->Throw("No procedure specified.");

    DString callP;
    e->AssureScalarPar<DStringGDL>(0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase(callP);

    // first search library procedures
    int proIx = LibProIx(callP);
    if (proIx != -1) {
      // 	e->PushNewEnv( libProList[ proIx], 1);
      // make the call
      // 	EnvT* newEnv = static_cast<EnvT*>(e->Interpreter()->CallStack().back());
      EnvT* newEnv = e->NewEnv(libProList[proIx], 1);
      Guard<EnvT> guard(newEnv);
      static_cast<DLibPro*> (newEnv->GetPro())->Pro()(newEnv);
    } else {
      proIx = DInterpreter::GetProIx(callP);

      StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());

      EnvUDT* newEnv = e->PushNewEnvUD(proList[proIx], 1);

      // make the call
      // 	EnvUDT* newEnv = static_cast<EnvUDT*>(e->Interpreter()->CallStack().back());
      e->Interpreter()->call_pro(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());
    }
  }

  void call_method_procedure(EnvT* e) {
    int nParam = e->NParam();
    if (nParam < 2)
      e->Throw("Name and object reference must be specified.");

    DString callP;
    e->AssureScalarPar<DStringGDL>(0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase(callP);

    DStructGDL* oStruct = e->GetObjectPar(1);

    DPro* method = oStruct->Desc()->GetPro(callP);

    if (method == NULL)
      e->Throw("Method not found: " + callP);

    StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());

    e->PushNewEnvUD(method, 2, (DObjGDL**) & e->GetPar(1));

    // the call
    e->Interpreter()->call_pro(method->GetTree());
  }

  void get_lun(EnvT* e) {
    int nParam = e->NParam(1);

    // not using SetPar later gives a better error message
    e->AssureGlobalPar(0);

    // here lun is the GDL lun, not the internal one
    DLong lun = GetLUN();

    if (lun == 0)
      e->Throw("All available logical units are currently in use.");

    BaseGDL** retLun = &e->GetPar(0);

    GDLDelete((*retLun));
    //            if( *retLun != e->Caller()->Object()) delete (*retLun);

    *retLun = new DLongGDL(lun);
    return;
  }

  // returns true if lun points to special unit
  // lun is GDL lun (-2..128)

  bool check_lun(EnvT* e, DLong lun) {
    if (lun < -2 || lun > maxLun)
      e->Throw("File unit is not within allowed range: " +
      i2s(lun) + ".");
    return (lun <= 0);
  }

  // TODO: handle ON_ERROR, ON_IOERROR, !ERROR_STATE.MSG

  void open_lun(EnvT* e, fstream::openmode mode) {
    int nParam = e->NParam(2);

    DLong lun;
    static int getlunIx=e->KeywordIx("GET_LUN"); //works because index of GET_LUN is same for all 3 functions using it. 
    bool getlunIsSet=e->KeywordSet(getlunIx);
    if (getlunIsSet) {
      //     get_lun( e);
      // not using SetPar later gives a better error message
      e->AssureGlobalPar(0);

      // here lun is the GDL lun, not the internal one
      lun = GetLUN();

      if (lun == 0)
        e->Throw("All available logical units are currently in use.");
    } else {
      e->AssureLongScalarPar(0, lun);
    }

    bool stdLun = check_lun(e, lun);
    if (stdLun)
      e->Throw("Unit already open. Unit: " + i2s(lun));

    DString name;
    // IDL allows here also arrays of length 1
    e->AssureScalarPar<DStringGDL>(1, name);

    // Change leading "~" to home directory
    //     if (name.substr(0,2) == "~/")
    //       name = getenv("HOME") + name.substr(1,name.size()-1);

    // AC 2018-mars-22 : at the end, correcting bug 720
    WordExp(name);

    // endian
    bool swapEndian = false;
    static int swapIx = e->KeywordIx("SWAP_ENDIAN");
    static int swapIfBigIx = e->KeywordIx("SWAP_IF_BIG_ENDIAN");
    static int swapIfLittleIx = e->KeywordIx("SWAP_IF_LITTLE_ENDIAN");
    if (e->KeywordSet(swapIx))
      swapEndian = true;
    else if (BigEndian())
      swapEndian = e->KeywordSet(swapIfBigIx);
    else
      swapEndian = e->KeywordSet(swapIfLittleIx);

    // compress
    bool compress = false;
    static int compressIx = e->KeywordIx("COMPRESS");
    if (e->KeywordSet(compressIx))
      compress = true;

    // xdr
    static int xdrIx = e->KeywordIx("XDR");
    bool xdr = e->KeywordSet(xdrIx);

    static int appendIx = e->KeywordIx("APPEND");
    // if( e->KeywordSet( appendIx)) mode |= fstream::ate;// fstream::app;
    // SA: trunc flag for non-existent file is needed in order to comply with
    // IDL behaviour (creating a file even if /APPEND flag is set) (tracker bug 2103871)
    if (e->KeywordSet(appendIx)) {
      if (compress)
        e->Throw("Keywords APPEND and COMPRESS exclude each other.");
      //
      // SA: The manual says that access() "is a potential security hole and should never be used"
      // but I didn't find any better way to do it. A problem might happen when the following sequence occurs:
      // * openu/openw is called with the /append flag and the target file does not exist
      // * access() informs about non-existence -> "trunc" flag is set to be used instead of "ate"
      // * in the meantime the file is created by some other process
      // * opening the file truncates it but it shouldn't as the /append flag was used
      // However, apparently only when "trunc" is set, a previously-non-existent file gets created.
      // Therefore it seems necessary to check for file existence before, in order to choose
      // between "ate" and "trunc" flags.
      //
      // ensuring trunc when a non-existent file requested (the OPENU,/APPEND case)
      if (-1 == access(name.c_str(), F_OK)) mode |= fstream::trunc;
      else {
        // ensuring no trunc when an existent file requested (the OPENW,/APPEND case)
        mode &= ~fstream::trunc;
        // handling /APPEND (both for OPENW,/APPEND and OPENU,/APPEND)
        mode |= fstream::ate;
      }
    }
#ifdef _WIN32
      mode |= ios::binary;
#endif

    static int f77Ix = e->KeywordIx("F77_UNFORMATTED");
    bool f77 = e->KeywordSet(f77Ix);

    static int delIx = e->KeywordIx("DELETE");
    bool deleteKey = e->KeywordSet(delIx);

    static int errorIx = e->KeywordIx("ERROR");
    bool errorKeyword = e->KeywordPresent(errorIx);
    if (errorKeyword) e->AssureGlobalKW(errorIx);

    DLong width = defaultStreamWidth;
    static int widthIx = e->KeywordIx("WIDTH");
    BaseGDL* widthKeyword = e->GetKW(widthIx);
    if (widthKeyword != NULL) {
      e->AssureLongScalarKW(widthIx, width);
    }

    // Assume variable-length VMS file initially
    // fileUnits[ lun-1].PutVarLenVMS( true);

    // m_schellens: this is no good. It fails for regular files which by accident fit the
    // variable-length VMS file criteria (see bug tracker ID: 3028279)
    // we need something more sophisticated here
    fileUnits[lun - 1].PutVarLenVMS(false);

    try {
      fileUnits[lun - 1].Open(name, mode, swapEndian, deleteKey,
        xdr, width, f77, compress);

      if (getlunIsSet) {
        BaseGDL** retLun = &e->GetPar(0);
        GDLDelete((*retLun));
        *retLun = new DLongGDL(lun);
        // par 0 contains now the LUN
      }
    }    //GD: If GDLIOException is not catched here BEFORE GDLException,
    catch (GDLIOException& ex) {
      if (getlunIsSet) {
        fileUnits[lun - 1].Free();
      }

      DString errorMsg = ex.getMessage() + // getMessage gets the non-decorated error message
        " Unit: " + i2s(lun) + ", File: " + name; //+ fileUnits[lun - 1].Name();

      if (!errorKeyword) {
        throw GDLIOException(ex.ErrorCode(), e->CallingNode(), errorMsg); //go above and be catched
      }

      BaseGDL** err = &e->GetKW(errorIx);

      GDLDelete(*err);
      //    if( *err != e->Caller()->Object()) delete (*err);

      *err = new DLongGDL(ex.ErrorCode());
      return;
    } catch (GDLException& ex) {
      if (getlunIsSet) {
        fileUnits[lun - 1].Free();
      }

      DString errorMsg = ex.getMessage() + // getMessage gets the non-decorated error message
        " Unit: " + i2s(lun) + ", File: " + name; // + fileUnits[lun - 1].Name();

      if (!errorKeyword) e->Throw(errorMsg);
      //				throw GDLIOException(ex.ErrorCode(), e->CallingNode(), errorMsg);

      BaseGDL** err = &e->GetKW(errorIx);

      GDLDelete(*err);
      //    if( *err != e->Caller()->Object()) delete (*err);

      *err = new DLongGDL(ex.ErrorCode());
      return;
    }

    if (errorKeyword) {
      BaseGDL** err = &e->GetKW(errorIx);

      // 	if( *err != e->Caller()->Object()) delete (*err);
      GDLDelete((*err));

      *err = new DLongGDL(0);
    }

  }

  void openr(EnvT* e) {
    open_lun(e, fstream::in);
  }

  void openw(EnvT* e) {
    open_lun(e, fstream::in | fstream::out | fstream::trunc);
  }

  void openu(EnvT* e) {
    open_lun(e, fstream::in | fstream::out);
  }

  void socket(EnvT* e) {
    int nParam = e->NParam(3);

    static int getlunIx=e->KeywordIx("GET_LUN"); 
    bool getlunIsSet=e->KeywordSet(getlunIx);
    if (getlunIsSet) get_lun(e);
    // par 0 contains now the LUN

    DLong lun;
    e->AssureLongScalarPar(0, lun);

    bool stdLun = check_lun(e, lun);
    if (stdLun)
      e->Throw("Unit already open. Unit: " + i2s(lun));

    DString host;
    // IDL allows here also arrays of length 1
    e->AssureScalarPar<DStringGDL>(1, host);

    DUInt port;
    BaseGDL* p2 = e->GetParDefined(2);
    if (p2->Type() == GDL_STRING) {
      // look up /etc/services
    } else if (p2->Type() == GDL_UINT) {
      e->AssureScalarPar<DUIntGDL>(2, port);
    } else if (p2->Type() == GDL_INT) {
      DInt p;
      e->AssureScalarPar<DIntGDL>(2, p);
      port = p;
    } else if (p2->Type() == GDL_LONG) {
      DLong p;
      e->AssureScalarPar<DLongGDL>(2, p);
      port = p;
    } else if (p2->Type() == GDL_ULONG) {
      DULong p;
      e->AssureScalarPar<DULongGDL>(2, p);
      port = p;
    }

    // endian
    bool swapEndian = false;
    static int swapIx = e->KeywordIx("SWAP_ENDIAN");
    static int swapIfBigIx = e->KeywordIx("SWAP_IF_BIG_ENDIAN");
    static int swapIfLittleIx = e->KeywordIx("SWAP_IF_LITTLE_ENDIAN");
    if (e->KeywordSet(swapIx))
      swapEndian = true;
    else if (BigEndian())
      swapEndian = e->KeywordSet(swapIfBigIx);
    else
      swapEndian = e->KeywordSet(swapIfLittleIx);

    static int connect_timeoutIx = e->KeywordIx("CONNECT_TIMEOUT");
    DDouble c_timeout = 0.0;
    e->AssureDoubleScalarKWIfPresent(connect_timeoutIx, c_timeout);
    static int read_timeoutIx = e->KeywordIx("READ_TIMEOUT");
    DDouble r_timeout = 0.0;
    e->AssureDoubleScalarKWIfPresent(read_timeoutIx, r_timeout);
    static int write_timeoutIx = e->KeywordIx("WRITE_TIMEOUT");
    DDouble w_timeout = 0.0;
    e->AssureDoubleScalarKWIfPresent(write_timeoutIx, w_timeout);

    static int errorIx = e->KeywordIx("ERROR");
    bool errorKeyword = e->KeywordPresent(errorIx);
    if (errorKeyword) e->AssureGlobalKW(errorIx);

    DLong width = defaultStreamWidth;
    static int widthIx = e->KeywordIx("WIDTH");
    BaseGDL* widthKeyword = e->GetKW(widthIx);
    if (widthKeyword != NULL) {
      e->AssureLongScalarKW(widthIx, width);
    }

    try {
      fileUnits[lun - 1].Socket(host, port, swapEndian,
        c_timeout, r_timeout, c_timeout);
    } catch (GDLException& ex) {
      DString errorMsg = ex.toString() + " Unit: " + i2s(lun) +
        ", File: " + fileUnits[lun - 1].Name();

      if (!errorKeyword)
        e->Throw(errorMsg);

      BaseGDL** err = &e->GetKW(errorIx);

      GDLDelete((*err));
      //    if( *err != e->Caller()->Object()) delete (*err);

      *err = new DLongGDL(1);
      return;
    }

    if (errorKeyword) {
      BaseGDL** err = &e->GetKW(errorIx);

      // 	if( *err != e->Caller()->Object()) delete (*err);
      GDLDelete((*err));

      *err = new DLongGDL(0);
    }
  }

  // FLUSH based on a patch from Orion Poplawski

  void flush_lun(EnvT* e) {
    // within GDL, always lun+1 is used
    int nParam = e->NParam();
    for (int p = 0; p < nParam; p++) {
      DLong lun;
      e->AssureLongScalarPar(p, lun);
      if (lun > maxLun)
        e->Throw("File unit is not within allowed range: " + i2s(lun) + ".");
      else if (lun == -2)
        cerr << flush;
      else if (lun == -1)
        cout << flush;
      else if (lun == 0)
        ; // do nothing?
      else
        fileUnits[lun - 1].Flush();
    }
  }

//  void close_free_lun(EnvT* e, bool freeLun) {
//    DLong journalLUN = SysVar::JournalLUN();
//
//    // within GDL, always lun+1 is used
//    if (e->KeywordSet("ALL")) //necessary: ALL is not part of free_lun list.
//      for (int p = maxUserLun; p < maxLun; ++p) {
//        if ((journalLUN - 1) != p) {
//          fileUnits[p].Close();
//          //	      if( freeLun)
//          fileUnits[p].Free();
//        }
//      }
//
//    if (e->KeywordSet("FILE") || e->KeywordSet("ALL")) //necessary, not parts of free_lun list.
//      for (int p = 0; p < maxUserLun; ++p) {
//        fileUnits[p].Close();
//        // freeing not necessary as get_lun does not use them
//        //if( freeLun) fileUnits[ p].Free();
//      }
//
//    int nParam = e->NParam();
//    for (int p = 0; p < nParam; p++) {
//      DLong lun;
//      e->AssureLongScalarPar(p, lun);
//      if (lun > maxLun)
//        e->Throw("File unit is not within allowed range: " +
//        i2s(lun) + ".");
//      if (lun < 1)
//        e->Throw("File unit does not allow this operation."
//        " Unit: " + i2s(lun) + ".");
//
//      if (lun == journalLUN)
//        e->Throw("Reserved file cannot be closed in this manner. Unit: " +
//        i2s(lun));
//
//      fileUnits[lun - 1].Close();
//      if (freeLun) fileUnits[lun - 1].Free();
//    }
//  }

  void close_lun(EnvT* e) {
    DLong journalLUN = SysVar::JournalLUN();
    static int ALLIx=e->KeywordIx("ALL");
    static int FILEIx=e->KeywordIx("FILE");
    // within GDL, always lun+1 is used
    if (e->KeywordSet(ALLIx)) {
      for (int p = maxUserLun; p < maxLun; ++p) {
        if ((journalLUN - 1) != p) {
          fileUnits[p].Close();
          fileUnits[p].Free();
        }
      }
    }

    if (e->KeywordSet(FILEIx) || e->KeywordSet(ALLIx)) {
      for (int p = 0; p < maxUserLun; ++p) {
        fileUnits[p].Close();
        // freeing not necessary as get_lun does not use them
        //if( freeLun) fileUnits[ p].Free();
      }
    }
    
    int nParam = e->NParam();
    for (int p = 0; p < nParam; p++) {
      DLong lun;
      e->AssureLongScalarPar(p, lun);
      if (lun > maxLun)
        e->Throw("File unit is not within allowed range: " +
        i2s(lun) + ".");
      if (lun < 1)
        e->Throw("File unit does not allow this operation."
        " Unit: " + i2s(lun) + ".");

      if (lun == journalLUN)
        e->Throw("Reserved file cannot be closed in this manner. Unit: " +
        i2s(lun));

      fileUnits[lun - 1].Close();
    }
  }

  void free_lun(EnvT* e) {
    DLong journalLUN = SysVar::JournalLUN();

    // within GDL, always lun+1 is used

    int nParam = e->NParam();
    for (int p = 0; p < nParam; p++) {
      DLong lun;
      e->AssureLongScalarPar(p, lun);
      if (lun > maxLun)
        e->Throw("File unit is not within allowed range: " +
        i2s(lun) + ".");
      if (lun < 1)
        e->Throw("File unit does not allow this operation."
        " Unit: " + i2s(lun) + ".");

      if (lun == journalLUN)
        e->Throw("Reserved file cannot be closed in this manner. Unit: " +
        i2s(lun));

      fileUnits[lun - 1].Close();
      fileUnits[lun - 1].Free();
    }
  }

  void writeu(EnvT* e) {
    SizeT nParam = e->NParam(1);

    DLong lun;
    e->AssureLongScalarPar(0, lun);

    ostream* os = NULL;
    ogzstream* ogzs = NULL;
    bool f77 = false;
    bool swapEndian = false;
    bool compress = false;
    XDR *xdrs = NULL;

    bool stdLun = check_lun(e, lun);
    if (stdLun) {
      if (lun == 0)
        e->Throw("Cannot write to stdin. Unit: " + i2s(lun));

      os = (lun == -1) ? &cout : &cerr;
    } else {
      if (!fileUnits[lun - 1].IsOpen())
        e->Throw("File unit is not open: " + i2s(lun));

      compress = fileUnits[lun - 1].Compress();
      if (!compress)
        os = &fileUnits[lun - 1].OStream();
      else
        ogzs = &fileUnits[lun - 1].OgzStream();
      f77 = fileUnits[lun - 1].F77();
      swapEndian = fileUnits[lun - 1].SwapEndian();
      xdrs = fileUnits[lun - 1].Xdr();
    }

    if (f77) {
      if (compress)
        e->Throw("COMPRESS not supported for F77.");

      // count record length
      SizeT nBytesAll = 0;
      for (SizeT i = 1; i < nParam; i++) {
        BaseGDL* p = e->GetParDefined(i);
        if (p->Type() == GDL_STRUCT) nBytesAll += static_cast<DStructGDL*> (p)->NBytesToTransfer();
        else nBytesAll += p->NBytes();
      }

      // write record length
      fileUnits[lun - 1].F77Write(nBytesAll);

      // write data
      for (SizeT i = 1; i < nParam; i++) {
        BaseGDL* p = e->GetPar(i); // defined already checkede
        p->Write(*os, swapEndian, compress, xdrs);
      }

      // write record length
      fileUnits[lun - 1].F77Write(nBytesAll);
    } else
      if (compress) {
      for (SizeT i = 1; i < nParam; i++) {
        BaseGDL* p = e->GetParDefined(i);
        p->Write(*ogzs, swapEndian, compress, xdrs);
      }
    } else {
      for (SizeT i = 1; i < nParam; i++) {
        BaseGDL* p = e->GetParDefined(i);
        p->Write(*os, swapEndian, compress, xdrs);
      }
    }

    BaseGDL* p = e->GetParDefined(nParam - 1);
    SizeT cc = p->Dim(0);
    BaseGDL** tcKW = NULL;
    static int tcIx = e->KeywordIx("TRANSFER_COUNT");
    if (e->KeywordPresent(tcIx)) {
      BaseGDL* p = e->GetParDefined(nParam - 1);
      tcKW = &e->GetKW(tcIx);
      GDLDelete((*tcKW));
      *tcKW = new DLongGDL(p->N_Elements());
    }
  }

  void readu(EnvT* e) {
    SizeT nParam = e->NParam(1);

    DLong lun;
    e->AssureLongScalarPar(0, lun);

    istream* is = NULL;
    igzstream* igzs = NULL;
    bool f77 = false;
    bool varlenVMS = false;
    bool swapEndian = false;
    bool compress = false;
    XDR *xdrs = NULL;
    int sockNum = fileUnits[lun - 1].SockNum();

    bool stdLun = check_lun(e, lun);
    if (stdLun) {
      if (lun != 0)
        e->Throw("Cannot read from stdout and stderr."
        " Unit: " + i2s(lun));
      is = &cin;
    } else if (sockNum != -1) {
      // Socket Read
      swapEndian = fileUnits[lun - 1].SwapEndian();

      compress = fileUnits[lun - 1].Compress();

      string *recvBuf = &fileUnits[lun - 1].RecvBuf();

      // Setup recv buffer & string
      const int MAXRECV = 2048 * 8;
      char buf[MAXRECV + 1];

      // Read socket until finished & store in recv string
      while (1) {
        memset(buf, 0, MAXRECV + 1);
        int status = recv(sockNum, buf, MAXRECV, 0);
        //	  cout << "Bytes received: " << status << endl;
        if (status == 0) break;
        for (SizeT i = 0; i < status; i++)
          recvBuf->push_back(buf[i]);
      }

      // Get istringstream, write recv string, & assign to istream
      istringstream *iss = &fileUnits[lun - 1].ISocketStream();
      iss->str(*recvBuf);
      is = iss;
    } else {
      compress = fileUnits[lun - 1].Compress();
      if (!compress)
        is = &fileUnits[lun - 1].IStream();
      else
        igzs = &fileUnits[lun - 1].IgzStream();
      f77 = fileUnits[lun - 1].F77();
      varlenVMS = fileUnits[lun - 1].VarLenVMS();
      swapEndian = fileUnits[lun - 1].SwapEndian();
      xdrs = fileUnits[lun - 1].Xdr();
    }


    if (f77) {
      SizeT recordLength = fileUnits[lun - 1].F77ReadStart();

      SizeT relPos = 0;
      for (SizeT i = 1; i < nParam; i++) {
        BaseGDL* p = e->GetPar(i);
        if (p == NULL) {
          e->AssureGlobalPar(i);
          p = new DFloatGDL(0.0);
          e->SetPar(i, p);
        }

        SizeT nBytes;
        if (p->Type() == GDL_STRUCT) nBytes = static_cast<DStructGDL*> (p)->NBytesToTransfer(); //p->NBytes does not give sum of length of struct elements, due to alignment.We decompose.
        else nBytes = p->NBytes();

        if ((relPos + nBytes) > recordLength)
          e->Throw("Attempt to read past end of F77_UNFORMATTED "
          "file record.");

        p->Read(*is, swapEndian, compress, xdrs);

        relPos += nBytes;
      }

      // forward to next record if necessary
      fileUnits[lun - 1].F77ReadEnd();
    } else
      for (SizeT i = 1; i < nParam; i++) {
        BaseGDL* p = e->GetPar(i);
        //	  cout << p->Rank() << endl; // JMG
        if (p == NULL) {
          e->AssureGlobalPar(i);
          p = new DFloatGDL(0.0);
          e->SetPar(i, p);
        }

        if (compress) {
          p->Read(*igzs, swapEndian, compress, xdrs);
        } else if (varlenVMS && i == 1) {
          // Check if VMS variable-length file
          char hdr[4], tmp;

          // Read possible record header
          is->read(hdr, 4);

          DLong nRec1;
          memcpy(&nRec1, hdr, 4);

          // switch endian
          tmp = hdr[3];
          hdr[3] = hdr[0];
          hdr[0] = tmp;
          tmp = hdr[2];
          hdr[2] = hdr[1];
          hdr[1] = tmp;

          DLong nRec2;
          memcpy(&nRec2, hdr, 4);
// 2018 April 14
// G.Jung I don't think this works right for stuctures.
//   I have a method (RealBytes) that computes the actual byte count,
//  it needs entries across several different files.
          SizeT nBytes = p->NBytes();

          // In variable length VMS files, each record is prefixed
          // with a count byte that contains the number of bytes
          // in the record.  This step checks whether the length
          // of the possible header record actually corresponds
          // to the total length of the desired fields in the
          // call to READU.

          // if not VMS v.l.f then backup 4 bytes and tag files
          // as not variable-length
          if (nRec1 != nBytes && nRec2 != nBytes) {
            is->seekg(-4, ios::cur);
            fileUnits[lun - 1].PutVarLenVMS(false);
          }
          p->Read(*is, swapEndian, compress, xdrs);
        } else
          p->Read(*is, swapEndian, compress, xdrs);

        // Socket Read
        if (sockNum != -1) {
          int pos = is->tellg();
          string *recvBuf = &fileUnits[lun - 1].RecvBuf();
          //	    cout << "pos: " << pos << endl;
          recvBuf->erase(0, pos);
        }
      }

    BaseGDL* p = e->GetParDefined(nParam - 1);
    SizeT cc = p->Dim(0);
    BaseGDL** tcKW = NULL;
    static int tcIx = e->KeywordIx("TRANSFER_COUNT");
    if (e->KeywordPresent(tcIx)) {
      BaseGDL* p = e->GetParDefined(nParam - 1);
      tcKW = &e->GetKW(tcIx);
      GDLDelete((*tcKW));
      *tcKW = new DLongGDL(p->N_Elements());
    }
  }

  void on_error(EnvT* e) {
    e->OnError();
  }

  void catch_pro(EnvT* e) {
    //     static bool warned = false;
    //     if (!warned) {
    //       Warning("CATCH: feature not implemented yet (FIXME!).");
    //       warned = true;
    //     }
    e->Catch();
  }

  void strput(EnvT* e) {
    SizeT nParam = e->NParam(2);

    BaseGDL* p0 = e->GetParGlobal(0);
    if (p0->Type() != GDL_STRING)
      e->Throw("String expression required in this context: " +
      e->GetParString(0));
    DStringGDL* dest = static_cast<DStringGDL*> (p0);

    DString source;
    e->AssureStringScalarPar(1, source);

    DLong pos = 0;
    if (nParam == 3) {
      e->AssureLongScalarPar(2, pos);
      if (pos < 0) pos = 0;
    }

    SizeT nEl = dest->N_Elements();

    TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for (OMPInt i = 0; i < nEl; ++i)
        StrPut((*dest)[i], source, pos);
    }
  }

  void retall(EnvT* e) {
    e->Interpreter()->RetAll();
  }

  void stop(EnvT* e) {
    if (e->NParam() > 0) print(e);
    debugMode = DEBUG_STOP;
  }

  void defsysv(EnvT* e) {
    SizeT nParam = e->NParam(1);

    DString sysVarNameFull;
    e->AssureStringScalarPar(0, sysVarNameFull);

    static int existIx = e->KeywordIx("EXIST");
    if (e->KeywordPresent(existIx)) {
      if (sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!') {
        e->SetKW(existIx, new DLongGDL(0));
      }

      DVar* sysVar = FindInVarList(sysVarList,
        StrUpCase(sysVarNameFull.substr(1)));
      if (sysVar == NULL)
        e->SetKW(existIx, new DLongGDL(0));
      else
        e->SetKW(existIx, new DLongGDL(1));
      return;
    } else if (nParam < 2)
      e->Throw("Incorrect number of arguments.");

    // here: nParam >= 2
    DLong rdOnly = 0;
    if (nParam >= 3)
      e->AssureLongScalarPar(2, rdOnly);

    if (sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!')
      e->Throw("Illegal system variable name: " + sysVarNameFull + ".");

    // strip "!", uppercase
    DString sysVarName = StrUpCase(sysVarNameFull.substr(1));

    DVar* sysVar = FindInVarList(sysVarList, sysVarName);

    // check if the variable is defined
    BaseGDL* p1 = e->GetParDefined(1);

    if (sysVar == NULL) {
      // define new
      DVar *newSysVar = new DVar(sysVarName, p1->Dup());
      sysVarList.push_back(newSysVar);

      // rdOnly is only set at the first definition
      if (rdOnly != 0)
        sysVarRdOnlyList.push_back(newSysVar);
      return;
    }

    // re-set
    // make sure type and size are kept
    BaseGDL* oldVar = sysVar->Data();
    BaseGDL* newVar = p1;
    if (oldVar->Type() != newVar->Type() ||
      oldVar->N_Elements() != newVar->N_Elements())
      e->Throw("Conflicting definition for " + sysVarNameFull + ".");

    // if struct -> assure equal descriptors
    if (oldVar->Type() == GDL_STRUCT) {
      DStructGDL *oldStruct = static_cast<DStructGDL*> (oldVar);
      // types are same -> static cast
      DStructGDL *newStruct = static_cast<DStructGDL*> (newVar);

      // note that IDL handles different structs more relaxed
      // ie. just the structure pattern is compared.
      if (*oldStruct->Desc() != *newStruct->Desc())
        e->Throw("Conflicting definition for " + sysVarNameFull + ".");
    }

    DVar* sysVarRdOnly = FindInVarList(sysVarRdOnlyList, sysVarName);
    if (sysVarRdOnly != NULL) {
      // rdOnly set and is already rdOnly: do nothing
      if (rdOnly != 0) return;

      // else complain
      e->Throw("Attempt to write to a readonly variable: " +
        sysVarNameFull + ".");
    } else {
      // not read only
      GDLDelete(oldVar);
      sysVar->Data() = newVar->Dup();

      // only on first definition
      //	if( rdOnly != 0)
      //	  sysVarRdOnlyList.push_back( sysVar);
    }
  }

  // note: this implemetation does not honor all keywords

  void message_pro(EnvT* e) {

    DStructGDL* errorState = SysVar::Error_State();
    static unsigned nameTag = errorState->Desc()->TagIndex("NAME");
    static unsigned blockTag = errorState->Desc()->TagIndex("BLOCK");
    static unsigned codeTag = errorState->Desc()->TagIndex("CODE");
    static unsigned sys_codeTag = errorState->Desc()->TagIndex("SYS_CODE");
    static unsigned sys_code_typeTag = errorState->Desc()->TagIndex("SYS_CODE_TYPE");
    static unsigned msgTag = errorState->Desc()->TagIndex("MSG");
    static unsigned sys_msgTag = errorState->Desc()->TagIndex("SYS_MSG");
    static unsigned msg_prefixTag = errorState->Desc()->TagIndex("MSG_PREFIX");

    SizeT nParam = e->NParam();

    static int continueIx = e->KeywordIx("CONTINUE");
    static int infoIx = e->KeywordIx("INFORMATIONAL");
    static int ioerrorIx = e->KeywordIx("IOERROR");
    static int nonameIx = e->KeywordIx("NONAME");
    static int noprefixIx = e->KeywordIx("NOPREFIX");
    static int noprintIx = e->KeywordIx("NOPRINT");
    static int resetIx = e->KeywordIx("RESET");
    static int reissueIx = e->KeywordIx("REISSUE_LAST");

    bool continueKW = e->KeywordSet(continueIx);
    bool info = e->KeywordSet(infoIx);
    bool ioerror = e->KeywordSet(ioerrorIx);
    bool noname = e->KeywordSet(nonameIx);
    bool noprefix = e->KeywordSet(noprefixIx);
    bool noprint = e->KeywordSet(noprintIx);
    bool reset = e->KeywordSet(resetIx);
    bool reissue = e->KeywordSet(reissueIx);

    if (reset) {
      (*static_cast<DStringGDL*> (errorState->GetTag(nameTag)))[0] = "IDL_M_SUCCESS";
      (*static_cast<DStringGDL*> (errorState->GetTag(blockTag)))[0] = "IDL_MBLK_CORE";
      (*static_cast<DLongGDL*> (errorState->GetTag(sys_codeTag)))[0] = 0;
      (*static_cast<DLongGDL*> (errorState->GetTag(sys_codeTag)))[1] = 0;
      (*static_cast<DStringGDL*> (errorState->GetTag(sys_code_typeTag)))[0] = "";
      (*static_cast<DStringGDL*> (errorState->GetTag(msgTag)))[0] = "";
      (*static_cast<DStringGDL*> (errorState->GetTag(sys_msgTag)))[0] = "";
      (*static_cast<DStringGDL*> (errorState->GetTag(msg_prefixTag)))[0] = "% ";

      SysVar::SetErr_String("");
      SysVar::SetErrError(0);
    }

    if (reissue) {
      DStructGDL* errorState = SysVar::Error_State();
      static unsigned msgTag = errorState->Desc()->TagIndex("MSG");
      if (!info || (SysVar::Quiet() == 0)) cout << (*static_cast<DStringGDL*> (errorState->GetTag(msgTag)))[0] << endl;
      return;
    }

    if (nParam == 0) return;

    DString msg;
    e->AssureScalarPar<DStringGDL>(0, msg);

    if (!noname) {
      static int levelIx = e->KeywordIx("LEVEL");

      if (!e->KeywordPresent(levelIx)) {
        msg = e->Caller()->GetProName() + ": " + msg;
      } else {
        DLong level;
        e->AssureLongScalarKW(levelIx, level);

        if (level == 0) {
          msg = e->Caller()->GetProName() + ": " + msg;
        } else {
          // this is a code derived from SimpleDumpStack() above
          EnvStackT& callStack = e->Interpreter()->CallStack();
          long actIx = callStack.size() - 1;

          if (level > 0) {
            if (level > actIx) level = actIx;
            if (level == 0) level++;
            EnvStackT::pointer_type upEnv = callStack[level - 1];
            DString actString = upEnv->GetProName();
            msg = actString + ": " + msg;
          }
          if (level < 0) {
            DLong value = actIx + level;
            if (value < 0) value = 0;
            EnvStackT::pointer_type upEnv = callStack[value];
            DString actString = upEnv->GetProName();
            msg = actString + ": " + msg;
          }
        }
      }
    }

    if (!info) {
      (*static_cast<DStringGDL*> (errorState->GetTag(nameTag)))[0] = "IDL_M_USER_ERR"; //unfortunately will be erased by gdlexception below...
      (*static_cast<DLongGDL*> (errorState->GetTag(codeTag)))[0] = -5;
      (*static_cast<DStringGDL*> (errorState->GetTag(msgTag)))[0] = msg;

      SysVar::SetErr_String(msg);
      SysVar::SetErrError(-5); //IDL_M_USER_ERR is -5
    }

    if (noprint)
      msg = "";

    if (!continueKW && !info)
      throw GDLException(-5, msg, !noprefix, false);

    if (!noprint && !noprefix)
      msg = SysVar::MsgPrefix() + msg;

    if (!info || (SysVar::Quiet() == 0))
      cout << msg << endl;
  }

  void byteorderDo(EnvT* e, BaseGDL* pIn, SizeT swapSz, DLong p) {
    if (pIn->Type() == GDL_STRUCT) {
      DStructGDL* dS = static_cast<DStructGDL*> (pIn);
      if (dS->Desc()->ContainsStringPtrObject())
        e->Throw("Structs must not contain PTR, OBJECT or STRING tags: " + e->GetParString(p));
      for (SizeT t = 0; t < dS->NTags(); ++t) {
        BaseGDL* par = dS->GetTag(t);

        if (par->Type() == GDL_STRUCT && par->N_Elements() == 1) {
          // do tag by tag for scalar struct as memory might not be contigous (
          byteorderDo(e, par, swapSz, p);
        } else {

          SizeT nBytes = par->NBytes();
          if (nBytes % swapSz != 0)
            e->Throw("Operand's size must be a multiple of swap "
            "datum size: " + e->GetParString(p));

          SizeT nSwap = nBytes / swapSz;

          char* addr = static_cast<char*> (par->DataAddr());

          for (SizeT i = 0; i < nSwap; ++i) {
            for (SizeT s = 0; s < (swapSz / 2); ++s) {
              char tmp = *(addr + i * swapSz + s);
              *(addr + i * swapSz + s) = *(addr + i * swapSz + swapSz - 1 - s);
              *(addr + i * swapSz + swapSz - 1 - s) = tmp;
            }
          }
        }
      }
    } else {
      if (pIn->Type() == GDL_STRING)
        e->Throw("STRING type not allowed in this context: " + e->GetParString(p));
      if (pIn->Type() == GDL_OBJ)
        e->Throw("Object type not allowed in this context: " + e->GetParString(p));
      if (pIn->Type() == GDL_PTR)
        e->Throw("PTR type not allowed in this context: " + e->GetParString(p));

      BaseGDL*& par = pIn;
      SizeT nBytes = par->NBytes();
      if (nBytes % swapSz != 0)
        e->Throw("Operand's size must be a multiple of swap "
        "datum size: " + e->GetParString(p));

      SizeT nSwap = nBytes / swapSz;

      char* addr = static_cast<char*> (par->DataAddr());

      for (SizeT i = 0; i < nSwap; ++i) {
        for (SizeT s = 0; s < (swapSz / 2); ++s) {
          char tmp = *(addr + i * swapSz + s);
          *(addr + i * swapSz + s) = *(addr + i * swapSz + swapSz - 1 - s);
          *(addr + i * swapSz + swapSz - 1 - s) = tmp;
        }
      }
    }
  }

  void byteorder(EnvT* e) {
    SizeT nParam = e->NParam(1);

    //    static int sswapIx = e->KeywordIx( "SSWAP");
    static int lswapIx = e->KeywordIx("LSWAP");
    static int l64swapIx = e->KeywordIx("L64SWAP");
    static int ifBigIx = e->KeywordIx("SWAP_IF_BIG_ENDIAN");
    static int ifLittleIx = e->KeywordIx("SWAP_IF_LITTLE_ENDIAN");
    static int ntohlIx = e->KeywordIx("NTOHL");
    static int ntohsIx = e->KeywordIx("NTOHS");
    static int htonlIx = e->KeywordIx("HTONL");
    static int htonsIx = e->KeywordIx("HTONS");
    static int ftoxdrIx = e->KeywordIx("FTOXDR");
    static int dtoxdrIx = e->KeywordIx("DTOXDR");
    static int xdrtofIx = e->KeywordIx("XDRTOF");
    static int xdrtodIx = e->KeywordIx("XDRTOD");

    bool lswap = e->KeywordSet(lswapIx);
    bool l64swap = e->KeywordSet(l64swapIx);
    bool ifBig = e->KeywordSet(ifBigIx);
    bool ifLittle = e->KeywordSet(ifLittleIx);

    // to-from network conversion (big-endian)
    bool ntohl = e->KeywordSet(ntohlIx);
    bool ntohs = e->KeywordSet(ntohsIx);
    bool htonl = e->KeywordSet(htonlIx);
    bool htons = e->KeywordSet(htonsIx);

    // XDR to-from conversion
    bool ftoxdr = e->KeywordSet(ftoxdrIx);
    bool dtoxdr = e->KeywordSet(dtoxdrIx);
    bool xdrtof = e->KeywordSet(xdrtofIx);
    bool xdrtod = e->KeywordSet(xdrtodIx);

    if (ifBig && !BigEndian()) return;
    if (ifLittle && BigEndian()) return;

    if (BigEndian() && (ntohl || ntohs || htonl || htons)) return;

    for (DLong p = nParam - 1; p >= 0; --p) {
      BaseGDL* par = e->GetParDefined(p);
      if (!e->GlobalPar(p))
        e->Throw("Expression must be named variable in this context: " +
        e->GetParString(p));

      SizeT swapSz = 2;
      if (l64swap || dtoxdr || xdrtod)
        swapSz = 8;
      else if (lswap || ntohl || htonl || ftoxdr || xdrtof)
        swapSz = 4;

      byteorderDo(e, par, swapSz, p);
    }
  }

  void setenv_pro(EnvT* e) {
    SizeT nParam = e->NParam();

    DStringGDL* name = e->GetParAs<DStringGDL>(0);
    SizeT nEnv = name->N_Elements();

    for (SizeT i = 0; i < nEnv; ++i) {
      DString strEnv = (*name)[i];
      long len = strEnv.length();
      long pos = strEnv.find_first_of("=", 0);
      if (pos == string::npos) continue;
      DString strArg = strEnv.substr(pos + 1, len - pos - 1);
      strEnv = strEnv.substr(0, pos);
      // putenv() is POSIX unlike setenv()
#if defined(__hpux__) || defined(_WIN32)
      int ret = putenv((strEnv + "=" + strArg).c_str());
#else
      int ret = setenv(strEnv.c_str(), strArg.c_str(), 1);
#endif
    }
  }

  void struct_assign_pro(EnvT* e) {
    SizeT nParam = e->NParam(2);

    DStructGDL* source = e->GetParAs<DStructGDL>(0);
    DStructGDL* dest = e->GetParAs<DStructGDL>(1);

    static int nozeroIx = e->KeywordIx("NOZERO");
    bool nozero = e->KeywordSet(nozeroIx);

    static int verboseIx = e->KeywordIx("VERBOSE");
    bool verbose = e->KeywordSet(verboseIx);

    string sourceName = (*source).Desc()->Name();

    SizeT nTags = 0;

    // array of struct
    SizeT nElements = source->N_Elements();
    SizeT nDestElements = dest->N_Elements();
    if (nElements > nDestElements)
      nElements = nDestElements;

    // zero out the destination
    if (!nozero)
      (*dest).Clear();

    nTags = (*source).Desc()->NTags();

    // copy the stuff
    for (int t = 0; t < nTags; ++t) {
      string sourceTagName = (*source).Desc()->TagName(t);
      int ix = (*dest).Desc()->TagIndex(sourceTagName);
      if (ix >= 0) {
        SizeT nTagElements = source->GetTag(t)->N_Elements();
        SizeT nTagDestElements = dest->GetTag(ix)->N_Elements();

        if (verbose) {
          if (nTagElements > nTagDestElements)
            Warning("STRUCT_ASSIGN: " + sourceName +
            " tag " + sourceTagName +
            " is longer than destination. "
            "The end will be clipped.");
          else if (nTagElements < nTagDestElements)
            Warning("STRUCT_ASSIGN: " + sourceName +
            " tag " + sourceTagName +
            " is shorter than destination. "
            "The end will be zero filled.");
        }

        if (nTagElements > nTagDestElements)
          nTagElements = nTagDestElements;

        for (SizeT a = 0; a < nElements; ++a)
          dest->GetTag(ix, a)->Assign(source->GetTag(t, a), nTagElements);
      } else
        if (verbose)
        Warning("STRUCT_ASSIGN: Destination lacks " + sourceName +
        " tag " + sourceTagName + ". Not copied.");
    }
  }
#ifdef _WIN32
#define BUFSIZE 1024

  void ReadPipeToDString(HANDLE g_Rd, char *buf, int *pos, vector<DString> *s_str) {
    CHAR chbuf[BUFSIZE];
    CHAR a_chr;
    DWORD dwRead;
    BOOL bSuccess = FALSE;
    int len = 0;
    int nlines = 0;
    int debug = 0;
    int ptr = *pos;
    if (debug) std::printf(" RPTD: for(;;) { ");
    for (;;) {
      if (debug) std::printf(" ReadFile .. ");
      bSuccess = ReadFile(g_Rd, chbuf, BUFSIZE, &dwRead, NULL);
      if (!bSuccess || dwRead == 0) break;
      if (debug) std::printf(" dwRead, pos= %d %d ", dwRead, ptr);

      len = 0;
      while (dwRead > 0) {
        a_chr = chbuf[len++];
        dwRead--;
        if (a_chr == '\r') a_chr = '\0';

        if (a_chr == '\n') {
          buf[ptr] = 0;
          ptr = 0;
          nlines++;
          //			if(debug) std::printf(" %d:%s",dwRead,buf);
          s_str->push_back(DString(buf));
        } else buf[ptr++] = a_chr;
        if (ptr >= BUFSIZE - 1) {
          buf[BUFSIZE - 1] = 0;
          ptr = 0;
          s_str->push_back(DString(buf));
        }
      }
    }
    if (debug) {
      if (bSuccess) std::printf(" bSuccess=T #lines: %d ", nlines);
      else std::printf(" bSuccess=F #lines: %d ", nlines);
      std::printf(" pos= %d }\n", ptr);
    }
    *pos = ptr;
  }
static DWORD launch_cmd(BOOL hide, BOOL nowait,
                     const char * cmd, const char * title = NULL, DWORD *pid = NULL,
		     vector<DString> *ds_outs = NULL, vector<DString> *ds_errs = NULL)
    {
    DWORD status;
    CHAR outbuf[BUFSIZE];
    CHAR errbuf[BUFSIZE];

    STARTUPINFOW si = {0,};
    PROCESS_INFORMATION pi = {0,};

      WCHAR w_cmd[1000];
      MultiByteToWideChar(CP_UTF8, 0, cmd, -1, w_cmd, 1000);
      WCHAR w_title[100];
      if( title != NULL)
              MultiByteToWideChar(CP_UTF8, 0, title, -1, w_title, 100);
      WCHAR w_cwd[MAX_PATH];
	DString cwd = lib::GetCWD();
             MultiByteToWideChar(CP_UTF8, 0, cwd.c_str(), -1, w_cwd, MAX_PATH);
	
    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength = sizeof (SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;
    HANDLE g_hChildStd_ERR_Rd = NULL;
    HANDLE g_hChildStd_ERR_Wr = NULL;

    si.cb = sizeof (si);
    if (title == NULL)
        si.lpTitle = (wchar_t *) L"GDL spawned process";
    else
        si.lpTitle = w_title;
    int debug = 0;

    if (hide) {
      si.dwFlags = STARTF_USESHOWWINDOW;
      si.wShowWindow = SW_SHOWMINNOACTIVE;
    }

    if (ds_outs != NULL) {
      if (debug) std::printf(" CreatePipe stdout: ");
      CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
      si.hStdOutput = g_hChildStd_OUT_Wr;
      // Ensure the read handle to the pipe for STDOUT is not inherited.
      SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
      if (ds_errs != NULL) {
        if (debug) std::printf(" CreatePipe stderr: ");
        CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &saAttr, 0);
        si.hStdError = g_hChildStd_ERR_Wr;
        SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0);
      }
      si.dwFlags |= STARTF_USESTDHANDLES;
      if (debug) std::printf(" CreateProcess: ");
        CreateProcessW(NULL, w_cmd, NULL, NULL, TRUE,
						 0, NULL, w_cwd,	// start from wherever we happen to be 
								&si, &pi);
      if (pid != NULL) *pid = pi.dwProcessId;
      DWORD progress;

      int poserr = 0;
      int posout = 0;
      if (ds_errs != NULL) CloseHandle(g_hChildStd_ERR_Wr);
      CloseHandle(g_hChildStd_OUT_Wr);
      do {
        Sleep(10);
        //		   if (ds_errs != NULL) ReadPipeToDString(g_hChildStd_ERR_Rd,errbuf,&poserr,ds_errs);
        if (ds_outs != NULL) ReadPipeToDString(g_hChildStd_OUT_Rd, outbuf, &posout, ds_outs);
        if (debug) std::printf(" Wait 1 sec: ");
        if (debug) Sleep(1000);
        progress = WaitForSingleObject(pi.hProcess, 0);
      } while (progress == WAIT_TIMEOUT);
      GetExitCodeProcess(pi.hProcess, &status);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);

      if (progress == WAIT_OBJECT_0) {
        ReadPipeToDString(g_hChildStd_OUT_Rd, outbuf, &posout, ds_outs);
        if (posout > 0) {
          outbuf[BUFSIZE - 1] = 0;
          posout++;
          if (posout < BUFSIZE) outbuf[posout] = 0;
          ds_outs->push_back(DString(outbuf));
        }
        CloseHandle(g_hChildStd_OUT_Rd);
        if (ds_errs != NULL) {
          ReadPipeToDString(g_hChildStd_ERR_Rd, errbuf, &poserr, ds_errs);
          if (poserr > 0) {
            errbuf[BUFSIZE - 1] = 0;
            poserr++;
            if (poserr < BUFSIZE) errbuf[poserr] = 0;
            ds_errs->push_back(DString(errbuf));
          }
          CloseHandle(g_hChildStd_ERR_Rd);
        }
      } else
        std::printf(" error from CreateProcess: progress = 0x%x \n", progress);

      }
      else
	{
          CreateProcessW(NULL, w_cmd, NULL, NULL, FALSE,
                        CREATE_NEW_CONSOLE, 
                        NULL, w_cwd,	// start from wherever we happen to be 
								&si, &pi);
      if (pid != NULL) *pid = pi.dwProcessId;
      if (!nowait) WaitForSingleObject(pi.hProcess, INFINITE);
      GetExitCodeProcess(pi.hProcess, &status);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    }
    return status;
  }

  void spawn_pro(EnvT* e) {
    SizeT nParam = e->NParam();

    static int countIx = e->KeywordIx("COUNT");
    bool countKeyword = e->KeywordPresent(countIx);
    if (countKeyword) e->AssureGlobalKW(countIx);

    static int pidIx = e->KeywordIx("PID");
    bool pidKeyword = e->KeywordPresent(pidIx);
    if (pidKeyword) e->AssureGlobalKW(pidIx);

    static int waitIx = e->KeywordIx("NOWAIT");
    bool nowaitKeyword = e->KeywordPresent(waitIx);

    static int exit_statusIx = e->KeywordIx("EXIT_STATUS");
    bool exit_statusKeyword = e->KeywordPresent(exit_statusIx);
    if (exit_statusKeyword) e->AssureGlobalKW(exit_statusIx);

    static int noshellIx = e->KeywordIx("NOSHELL");
    bool noshellKeyword = e->KeywordSet(noshellIx);

    static int hideIx = e->KeywordIx("HIDE");
    bool hideKeyword = e->KeywordSet(hideIx);

    static int unitIx = e->KeywordIx("UNIT");
    bool unitKeyword = e->KeywordPresent(unitIx);
    if (unitKeyword) e->AssureGlobalKW(unitIx);

    if (unitKeyword) {
      e->Throw("UNIT keyword is not implemented yet!");
      /*
        if (exit_statusKeyword)
        {
        Warning("SPAWN: specifying EXIT_STATUS with UNIT keyword has no meaning (assigning zero)");
        e->SetKW(exit_statusIx, new DLongGDL(0));
        }
        if (countKeyword)
        {
        Warning("SPAWN: specifying COUNT with UNIT keyword has no meaning (assigning zero)");
        e->SetKW(countIx, new DLongGDL(0));
        }
        if (nParam != 1) e->Throw("Invalid use of the UNIT keyword (only one argument allowed when using UNIT).");
       */
    }

      if (nParam == 0)
	{
          DWORD status = launch_cmd(hideKeyword, nowaitKeyword,
                              "cmd", " (GDL-Spawned) Command Prompt");
      if (countKeyword)
        e->SetKW(countIx, new DLongGDL(0));
      if (exit_statusKeyword)
        e->SetKW(exit_statusIx, new DLongGDL(status));
      return;
    }

    DStringGDL* command = e->GetParAs<DStringGDL>(0);
    DString cmd = (*command)[0];

    const int bufSize = 1024;
    char buf[bufSize];

    if (nParam > 1) e->AssureGlobalPar(1);
    if (nParam > 2) e->AssureGlobalPar(2);
    DString ds_cmd;
    if (noshellKeyword)
      ds_cmd = cmd;
    else
      ds_cmd = "cmd /c " + cmd;
    vector<DString> ds_outs;
    vector<DString> ds_errs;
    int status;
    DWORD pid;
    if (nParam == 1)
        status = launch_cmd(hideKeyword, nowaitKeyword, ds_cmd.c_str(), NULL, &pid);
    else if (nParam == 2) {
        status = launch_cmd(hideKeyword, nowaitKeyword, ds_cmd.c_str(), NULL, &pid, &ds_outs);
      }
      else if (nParam == 3) {
        status = launch_cmd(hideKeyword, nowaitKeyword, ds_cmd.c_str(), NULL, &pid, &ds_outs, &ds_errs);
    }

    if (pidKeyword)
      e->SetKW(pidIx, new DLongGDL(pid));

    if (exit_statusKeyword)
      e->SetKW(exit_statusIx, new DLongGDL(status));


    SizeT nLines = 0;
    if (nParam > 1) {
      DStringGDL* result;
      nLines = ds_outs.size();
      if (nLines == 0)
        result = new DStringGDL("");
      else {
        result = new DStringGDL(dimension(nLines), BaseGDL::NOZERO);
        for (SizeT l = 0; l < nLines; ++l) (*result)[l] = ds_outs[l];
      }
      e->SetPar(1, result);
    }

    if (countKeyword) e->SetKW(countIx, new DLongGDL(nLines));

    if (nParam > 2) {
      DStringGDL* errResult;
      SizeT nErrLines = ds_errs.size();
      if (nErrLines == 0)
        errResult = new DStringGDL("");
      else {
        errResult = new DStringGDL(dimension(nErrLines), BaseGDL::NOZERO);
        for (SizeT l = 0; l < nErrLines; ++l) (*errResult)[l] = ds_errs[l];
      }
      e->SetPar(2, errResult);
    }
  }

#else
  // helper function for spawn_pro

  static void child_sighandler(int x) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0);
  }

  void spawn_pro(EnvT* e) {
    SizeT nParam = e->NParam();

    static int countIx = e->KeywordIx("COUNT");
    bool countKeyword = e->KeywordPresent(countIx);
    if (countKeyword) e->AssureGlobalKW(countIx);

    static int pidIx = e->KeywordIx("PID");
    bool pidKeyword = e->KeywordPresent(pidIx);
    if (pidKeyword) e->AssureGlobalKW(pidIx);

    static int exit_statusIx = e->KeywordIx("EXIT_STATUS");
    bool exit_statusKeyword = e->KeywordPresent(exit_statusIx);
    if (exit_statusKeyword) e->AssureGlobalKW(exit_statusIx);

    static int shIx = e->KeywordIx("SH");
    bool shKeyword = e->KeywordSet(shIx);

    static int noshellIx = e->KeywordIx("NOSHELL");
    bool noshellKeyword = e->KeywordSet(noshellIx);

    static int unitIx = e->KeywordIx("UNIT");
    bool unitKeyword = e->KeywordPresent(unitIx);
    if (unitKeyword) e->AssureGlobalKW(unitIx);

    if (unitKeyword) {
      if (exit_statusKeyword) {
        Warning("SPAWN: specifying EXIT_STATUS with UNIT keyword has no meaning (assigning zero)");
        e->SetKW(exit_statusIx, new DLongGDL(0));
      }
      if (countKeyword) {
        Warning("SPAWN: specifying COUNT with UNIT keyword has no meaning (assigning zero)");
        e->SetKW(countIx, new DLongGDL(0));
      }
      if (nParam != 1) e->Throw("Invalid use of the UNIT keyword (only one argument allowed when using UNIT).");
    }

    string shellCmd = "/bin/sh"; // must be there if POSIX
    if (!shKeyword) {
      char* shellEnv = getenv("SHELL");
      if (shellEnv == NULL) shellEnv = getenv("COMSPEC");
      if (shellEnv == NULL) e->Throw("Error spawning child process: \n"
        "Environment variable SHELL | COMSPEC not set.");
      shellCmd = shellEnv;
    }

    if (nParam == 0) {
      int status = system(shellCmd.c_str());
      status >>= 8;
      if (countKeyword)
        e->SetKW(countIx, new DLongGDL(0));
      if (exit_statusKeyword)
        e->SetKW(exit_statusIx, new DLongGDL(status));
      return;
    }

    // added on occasion of the UNIT kw patch
    if (unitKeyword) signal(SIGCHLD, child_sighandler);

    DStringGDL* command = e->GetParAs<DStringGDL>(0);
    DString cmd = (*command)[0];

    const int bufSize = 1024;
    char buf[bufSize];

    if (nParam > 1) e->AssureGlobalPar(1);
    if (nParam > 2) e->AssureGlobalPar(2);

    int coutP[2];
    if (nParam > 1 || unitKeyword) {
      if (pipe(coutP)) return;
    }

    int cerrP[2];
    if (nParam > 2 && !unitKeyword && pipe(cerrP)) return;

    pid_t pid = fork(); // *** fork
    if (pid == -1) // error in fork
    {
      close(coutP[0]);
      close(coutP[1]);
      if (nParam > 2 && !unitKeyword) {
        close(cerrP[0]);
        close(cerrP[1]);
      }
      return;
    }

    if (pid == 0) // we are child
    {
      if (unitKeyword) {
        dup2(coutP[1], 1); // cout
        dup2(coutP[1], 2); // cout
        close(coutP[0]);
        close(coutP[1]);
      } else {
        if (nParam > 1) dup2(coutP[1], 1); // cout
        if (nParam > 2) dup2(cerrP[1], 2); // cerr

        if (nParam > 1) {
          close(coutP[0]);
          close(coutP[1]);
        }
        if (nParam > 2) {
          close(cerrP[0]);
          close(cerrP[1]);
        }
      }

      if (noshellKeyword) {
        SizeT nArg = command->N_Elements();
        char** argv = new char*[nArg + 1];
        argv[nArg] = NULL;
        for (SizeT i = 0; i < nArg; ++i)
          argv[i] = const_cast<char*> ((*command)[i].c_str());

        execvp(cmd.c_str(), argv);

        delete[] argv; // only executes if exec fails
      } else
        execl(shellCmd.c_str(), shellCmd.c_str(), "-c",
        cmd.c_str(), (char *) NULL);

      Warning("SPAWN: Error managing child process.");
      _exit(1); // error in exec
    } else // we are parent
    {
      if (pidKeyword)
        e->SetKW(pidIx, new DLongGDL(pid));

      if (nParam > 1 || unitKeyword) close(coutP[1]);
      if (nParam > 2 && !unitKeyword) close(cerrP[1]);

      if (unitKeyword) {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
        // UNIT kw code based on the patch by Greg Huey:

        Warning("Warning: UNIT keyword to SPAWN may not yet be fully implemented (proceeding)");
        // This is just code stolen from void get_lun( EnvT* e)
        // here lun is the GDL lun, not the internal one
        DLong unit_lun = GetLUN();

        if (unit_lun == 0)
          e->Throw("SPAWN: Failed to get new LUN: GetLUN says: All available logical units are currently in use.");

        FILE *coutF;
        coutF = fdopen(coutP[0], "r");
        if (coutF == NULL) close(coutP[0]);

        e->SetKW(unitIx, new DLongGDL(unit_lun));
        bool stdLun = check_lun(e, unit_lun);
        if (stdLun)
          e->Throw("SPAWN: Failed to open new LUN: Unit already open. Unit: " + i2s(unit_lun));
        fileUnits[unit_lun - 1].PutVarLenVMS(false);

        // Here we invoke the black arts of converting from a C FILE*fd to an fstream object
        __gnu_cxx::stdio_filebuf<char> *frb_p;
        frb_p = new __gnu_cxx::stdio_filebuf<char>(coutF, std::ios_base::in);

        fileUnits[unit_lun - 1].Close();
        fileUnits[unit_lun - 1].Open("/dev/zero", std::ios_base::in, 0, 0, 0, 0, 0, 0);

        basic_streambuf<char> *bsrb_old_p;
        bsrb_old_p = fileUnits[unit_lun - 1].get_stream_readbuf_bsrb();
        fileUnits[unit_lun - 1].set_stream_readbuf_bsrb_from_frb(frb_p);
        fileUnits[unit_lun - 1].set_readbuf_frb_destroy_on_close(frb_p);
        fileUnits[unit_lun - 1].set_readbuf_bsrb_destroy_on_close(bsrb_old_p);
        fileUnits[unit_lun - 1].set_fd_close_on_close(coutP[0]);
#else
        e->Throw("UNIT kw. relies on GNU extensions to the std C++ library (that were not availble during compilation?)");
#endif

      } else {
        FILE *coutF, *cerrF;
        if (nParam > 1) {
          coutF = fdopen(coutP[0], "r");
          if (coutF == NULL) close(coutP[0]);
        }
        if (nParam > 2) {
          cerrF = fdopen(cerrP[0], "r");
          if (cerrF == NULL) close(cerrP[0]);
        }

        vector<DString> outStr;
        vector<DString> errStr;

        // read cout
        if (nParam > 1 && coutF != NULL) {
          while (fgets(buf, bufSize, coutF) != NULL) {
            SizeT len = strlen(buf);
            if (len != 0 && buf[len - 1] == '\n') buf[len - 1] = 0;
            outStr.push_back(DString(buf));
          }
          fclose(coutF);
        }

        // read cerr
        if (nParam > 2 && cerrF != NULL) {
          while (fgets(buf, bufSize, cerrF) != NULL) {
            SizeT len = strlen(buf);
            if (len != 0 && buf[len - 1] == '\n') buf[len - 1] = 0;
            errStr.push_back(DString(buf));
          }
          fclose(cerrF);
        }

        // wait until child terminates
        int status;
        pid_t wpid = wait(&status);

        if (exit_statusKeyword)
          e->SetKW(exit_statusIx, new DLongGDL(status >> 8));

        SizeT nLines = 0;
        if (nParam > 1) {
          DStringGDL* result;
          nLines = outStr.size();
          if (nLines == 0)
            result = new DStringGDL("");
          else {
            result = new DStringGDL(dimension(nLines), BaseGDL::NOZERO);
            for (SizeT l = 0; l < nLines; ++l) (*result)[l] = outStr[l];
          }
          e->SetPar(1, result);
        }

        if (countKeyword) e->SetKW(countIx, new DLongGDL(nLines));

        if (nParam > 2) {
          DStringGDL* errResult;
          SizeT nErrLines = errStr.size();
          if (nErrLines == 0)
            errResult = new DStringGDL("");
          else {
            errResult = new DStringGDL(dimension(nErrLines), BaseGDL::NOZERO);
            for (SizeT l = 0; l < nErrLines; ++l) (*errResult)[l] = errStr[l];
          }
          e->SetPar(2, errResult);
        }
      }
    }
  }
#endif

  void replicate_inplace_pro(EnvT* e) {
    SizeT nParam = e->NParam(2);

    if (nParam % 2)
      e->Throw("Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined(0);
    if (!e->GlobalPar(0))
      e->Throw("Expression must be named variable in this context: " +
      e->GetParString(0));

    BaseGDL* p1 = e->GetParDefined(1);
    if (!p1->Scalar())
      e->Throw("Expression must be a scalar in this context: " +
      e->GetParString(1));
    p1 = p1->Convert2(p0->Type(), BaseGDL::COPY);

    if (nParam == 2) {
      p0->AssignAt(p1);
      return;
    } else {
      BaseGDL* p2 = e->GetNumericParDefined(2);
      if (!p2->StrictScalar())
        e->Throw("Expression must be a scalar in this context: " +
        e->GetParString(2));

      SizeT d1;
      int ret = p2->Scalar2Index(d1);
      if (d1 < 1 || d1 > p0->Rank())
        e->Throw("D1 (3rd) argument is out of range: " +
        e->GetParString(2));

      // 	BaseGDL* p3 = e->GetNumericParDefined( 3);
      DLongGDL* p3 = e->GetParAs< DLongGDL>(3);
      if (p3->N_Elements() != p0->Rank())
        e->Throw("Loc1 (4th) argument must have the same number of "
        "elements as the dimensions of the X (1st) argument: " +
        e->GetParString(3));

      SizeT d2 = 0;
      BaseGDL* p4;
      BaseGDL* p5;
      if (nParam > 4) {
        p4 = e->GetNumericParDefined(4);
        if (!p4->StrictScalar())
          e->Throw("Expression must be a scalar in this context: " +
          e->GetParString(4));
        ret = p4->Scalar2Index(d2);
        if (d2 < 1 || d2 > p0->Rank())
          e->Throw("D5 (5th) argument is out of range: " +
          e->GetParString(4));

        p5 = e->GetNumericParDefined(5);
      }

      // 	ArrayIndexVectorT* ixList = new ArrayIndexVectorT();
      // 	Guard< ArrayIndexVectorT> ixList_guard( ixList);
      ArrayIndexVectorT ixList;
      // 	BaseGDL* loc1 = p3->Dup();
      // 	loc1->SetDim (dimension( loc1->N_Elements()));
      //	ixList->reserve( p3->N_Elements());
      for (size_t i = 0; i < p3->N_Elements(); i++)
        if ((i + 1) == d1)
          ixList.push_back(new ArrayIndexAll());
        else if ((i + 1) == d2)
          ixList.push_back(new CArrayIndexIndexed(p5, true));
        else
          ixList.push_back(new CArrayIndexScalar((*p3)[i])); //p3->NewIx(i)));
      ArrayIndexListT* ixL;
      MakeArrayIndex(&ixList, &ixL);
      Guard< ArrayIndexListT> ixL_guard(ixL);
      ixL->AssignAt(p0, p1);
      return;
    }
  }

  void resolve_routine(EnvT* e) {

    // AC 11 Sept. 2014
    //Warning("This code is not doing what it should.");
    //Warning("and keywords are not managed ...");
    //Warning("If you need this code, please ask or contribute !");

    static int eitherIx = e->KeywordIx("EITHER");
    bool eitherKeyword = e->KeywordSet(eitherIx);

    static int is_functionIx = e->KeywordIx("IS_FUNCTION");
    bool isfunctionKeyword = e->KeywordSet(is_functionIx);

    static int no_recompileIx = e->KeywordIx("NO_RECOMPILE");
    bool norecompileKeyword = e->KeywordSet(no_recompileIx);

    static int quietIx = e->KeywordIx("QUIET");
    bool quiet = e->KeywordSet(quietIx);
    
    static int cffIx = e->KeywordIx("COMPILE_FULL_FILE");
    bool cff = e->KeywordSet(cffIx);
    
    BaseGDL* p0 = e->GetParDefined(0);
    if (p0->Type() != GDL_STRING)
      e->Throw("Expression must be a string in this context: " +
      e->GetParString(0));
    DStringGDL* p0S = static_cast<DStringGDL*> (p0);

    static StrArr openFiles;

    SizeT nEl = p0S->N_Elements();
    for (int i = 0; i < nEl; ++i) {
      DString pro = (*p0S)[i];

      string proFile = StrLowCase(pro);
      AppendIfNeeded(proFile, ".pro");

      bool found = CompleteFileName(proFile);
      if (!found )
        if (!quiet) e->Throw("Not found: " + proFile); else return;

      // file already opened?
      bool open = false;
      for (StrArr::iterator j = openFiles.begin(); j != openFiles.end(); ++j) {
        if (proFile == *j) {
          open = true;
          break;
        }
      }
      if (open)
        continue;
      //routine already compiled? NATCHKEBIA Ilia 24.06.2015
      bool exists = false;
      for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i) {
        if (StrUpCase(proFile).find((*i)->ObjectName()) != std::string::npos) {
          //cout << "Routine is compiled,so won't recompile " << (*i)->ObjectName() <<endl;
          exists = true;
          break;
        }
      }
      if (exists && norecompileKeyword)
        continue;

      StackSizeGuard<StrArr> guard(openFiles);

      // append file to list
      openFiles.push_back(proFile);
      bool success = GDLInterpreter::CompileFile(proFile,cff?StrUpCase(pro):""); // this might trigger recursion

      //is func NATCHKEBIA Ilia 25.06.2015
      bool isFunc = false;
      for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i) {
        if (StrUpCase(proFile).find((*i)->ObjectName()) != std::string::npos) {
          //cout << "exists function " << (*i)->ObjectName() <<endl;
          isFunc = true;
          break;
        }
      }
      if ((!isFunc && isfunctionKeyword && !eitherKeyword) ||
        (isFunc && !isfunctionKeyword && !eitherKeyword && !exists))
        if (!quiet) e->Throw("Attempt to call undefined : " + proFile);

      if (success) {
        // Message("RESOLVE_ROUTINE: Compiled file: " + proFile);
      } else
        if (!quiet) e->Throw("Failed to compiled file: " + proFile); //please check this is the good behaviour
    }
  }

  void caldat(EnvT* e) {
    /*
     * SA: based on the following codes:
     * - cal_date() function from the NOVAS-C library (novas.c)
     *   (U.S. Naval Observatory Vector Astrometry Subroutines)
     *   http://aa.usno.navy.mil/software/novas/novas_c/novasc_info.php
     * - ln_get_date() function from the libnova library (src/julian_day.c)
     *   (by Liam Girdwood and Petr Kubanek) http://libnova.sourceforge.net/
     */

    // checking input; exiting if nothing to do
    SizeT nParam = e->NParam(1);
    if (nParam == 1) return;
    DDoubleGDL* p0 = e->GetParAs<DDoubleGDL>(0);

    // checking output (if present and global); exiting if nothing to do
    bool global[6];
    {
      short int sum = 0;
      for (SizeT i = 0; i < 6; i++) sum += global[i] = e->GlobalPar(i + 1);
      if (sum == 0) return;
    }

    // checking if all Julian values fall within the accepted range
    SizeT nEl = p0->N_Elements();
    for (SizeT i = 0; i < nEl; i++) if ((*p0)[i] < -1095 || (*p0)[i] > 1827933925)
        e->Throw("Value of Julian date (" + i2s((*p0)[i]) + ") is out of allowed range.");

    // preparing output (loop order important when all parameters point the same variable)
    //BaseGDL** ret[nParam - 1];
    BaseGDL*** ret;
    ret = (BaseGDL***) malloc((nParam - 1) * sizeof (BaseGDL**));
    GDLGuard<BaseGDL**, void, void> retGuard(ret, free);

    for (int i = nParam - 2; i >= 0; i--) {
      if (global[i]) {
        ret[i] = &e->GetPar(i + 1);
        // global parameter: undefined or different type/size -> creating
        if (*ret[i] == NULL ||
          (*ret[i])->Type() != (i < 5 ? GDL_LONG : GDL_DOUBLE) ||
          (*ret[i])->N_Elements() != nEl) { // gratutitous braces
          // not catching exceptions from SetPar as globality was ensured before
          if (i < 5) {
            // handling repeated parameters case
            if (nParam == 7 && *ret[i] == *ret[5]) global[i] = false;
            else e->SetPar(i + 1, new DLongGDL(p0->Dim()));
          } else e->SetPar(i + 1, new DDoubleGDL(p0->Dim()));
        }          // global parameter that has correct size but different shape -> reforming
        else if ((*ret[i])->Rank() != p0->Rank()) (*ret[i])->SetDim(p0->Dim());
      }
    }
    // loop over input elements
    for (SizeT i = 0; i < nEl; i++) {
      DLong iMonth, iDay, iYear, iHour, iMinute, dow, icap;
      DDouble Second;
      if (!j2ymdhms( (*p0)[i], iMonth, iDay, iYear, iHour, iMinute, Second, dow, icap )) throw GDLException("Value of Julian date is out of allowed range.");     

      if (global[1 - 1]) (*static_cast<DLongGDL*> (*ret[1 - 1]))[i] = iMonth+1;

      // days
      if (global[2 - 1]) (*static_cast<DLongGDL*> (*ret[2 - 1]))[i] = iDay;

      // years
      if (global[3 - 1]) (*static_cast<DLongGDL*> (*ret[3 - 1]))[i] = iYear;

      if (!(global[4 - 1] || global[5 - 1] || global[6 - 1])) continue;

      // hours
      if (global[4 - 1]) (*static_cast<DLongGDL*> (*ret[4 - 1]))[i] = iHour;

      // minutes
      if (global[5 - 1]) (*static_cast<DLongGDL*> (*ret[5 - 1]))[i] = iMinute;

      // seconds
      if (global[6 - 1]) (*static_cast<DDoubleGDL*> (*ret[6 - 1]))[i] = Second;
    }
    // now guarded. s. a.
    //     free((void *)ret);
  }

  BaseGDL* julday(EnvT* e) {
    if ((e->NParam() < 3 || e->NParam() > 6)) {
      e->Throw("Incorrect number of arguments.");
    }

    DLongGDL *Month, *Day, *Year, *Hour, *Minute;
    DDoubleGDL* Second;
    DDouble jd;
    DLong h = 12;
    DLong m = 0;
    DDouble s = 0.0;
    SizeT nM, nD, nY, nH, nMi, nS, finalN = 1;
    dimension finalDim;
    //behaviour: minimum set of dimensions of arrays. singletons expanded to dimension,
    //keep array trace.
    SizeT nEl, maxEl = 1, minEl;
    for (int i = 0; i < e->NParam(); ++i) {
      nEl = e->GetPar(i)->N_Elements();
      if ((nEl > 1) && (nEl > maxEl)) {
        maxEl = nEl;
        finalN = maxEl;
        finalDim = e->GetPar(i)->Dim();
      }
    } //first max - but we need first min:
    minEl = maxEl;
    for (int i = 0; i < e->NParam(); ++i) {
      nEl = e->GetPar(i)->N_Elements();
      if ((nEl > 1) && (nEl < minEl)) {
        minEl = nEl;
        finalN = minEl;
        finalDim = e->GetPar(i)->Dim();
      }
    } //min not singleton
    Month = e->GetParAs<DLongGDL>(0);
    nM = Month->N_Elements();
    Day = e->GetParAs<DLongGDL>(1);
    nD = Day->N_Elements();
    Year = e->GetParAs<DLongGDL>(2);
    nY = Year->N_Elements();

    if (e->NParam() == 3) {
      DLongGDL *ret = new DLongGDL(finalDim, BaseGDL::NOZERO);
      for (SizeT i = 0; i < finalN; ++i) {
        if (dateToJD(jd, (*Day)[i % nD], (*Month)[i % nM], (*Year)[i % nY], h, m, s)) {
          (*ret)[i] = (long) jd;
        } else e->Throw("Invalid Calendar Date input.");
      }
      return ret;
    }

    DDoubleGDL *ret = new DDoubleGDL(finalDim, BaseGDL::NOZERO);

    if (e->NParam() >= 4) {
      Hour = e->GetParAs<DLongGDL>(3);
      nH = Hour->N_Elements();
    }
    if (e->NParam() == 4) {
      for (SizeT i = 0; i < finalN; ++i) {
        if (dateToJD(jd, (*Day)[i % nD], (*Month)[i % nM], (*Year)[i % nY], (*Hour)[i % nH],
          m, s)) {
          (*ret)[i] = jd;
        } else e->Throw("Invalid Calendar Date input.");
      }
      return ret;
    }

    if (e->NParam() >= 5) {
      Minute = e->GetParAs<DLongGDL>(4);
      nMi = Minute->N_Elements();
    }
    if (e->NParam() == 5) {
      for (SizeT i = 0; i < finalN; ++i) {
        if (dateToJD(jd, (*Day)[i % nD], (*Month)[i % nM], (*Year)[i % nY], (*Hour)[i % nH],
          (*Minute)[i % nMi], s)) {
          (*ret)[i] = jd;
        } else e->Throw("Invalid Calendar Date input.");
      }
      return ret;
    }

    if (e->NParam() == 6) {
      Second = e->GetParAs<DDoubleGDL>(5);
      nS = Second->N_Elements();
      for (SizeT i = 0; i < finalN; ++i) {
        if (dateToJD(jd, (*Day)[i % nD], (*Month)[i % nM], (*Year)[i % nY], (*Hour)[i % nH],
          (*Minute)[i % nMi], (*Second)[i % nS])) {
          (*ret)[i] = jd;
        } else e->Throw("Invalid Calendar Date input.");
      }
      return ret;
    }
    assert(false);
    return NULL;
  }
  //dummy stub preventing !err and other !errore_state to be set!

  void pref_set_pro(EnvT* e) {
    SizeT nParam = e->NParam(1);
    if (nParam == 0) return;
    DStringGDL* p0 = e->GetParAs<DStringGDL>(0);
    cerr << "% PREF_SET: Unknown preference: " + (*p0)[0] << endl;
  }
// delvar_pro used to live in gdlhelp.cpp

void delvar_pro( EnvT* e)
    {
      StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());
	static volatile bool debug(false);
	static bool message_needed(false);
	static volatile SizeT numEnv;
	std::vector<int>  delvar, delcommon;
//	  trace_me = trace_arg(); 
	SizeT n_Param = e->NParam();
	set<string> showStr;  // "Sorted List" 
// 2016.05.16: message is taken down since errors have been attributed to 
// other aspects, not delvar specifically.
	if(message_needed) {
		std::cout << " ** DELVAR - Warning -- experimental routine !! " << std::endl
		<< "  o- Not 100 % robust: should NOT be used for objects"   << std::endl;
		message_needed = false;
	}
	EnvBaseT* caller = e->Caller();
	if(caller->CallingNode() != 0) return;
//			e->Throw(" DELVAR may only  be called from $MAIN$");
			
    SizeT pIndex;
	int todel[33];
	static int itest,ndel;
    ndel=0;
	delvar.clear();  delcommon.clear();
	for( SizeT ip=0; ip<n_Param; ip++)
		{
		BaseGDL*& par=e->GetPar( ip);
		if(par == 0) continue;
		DString parString = caller->GetString( par,true);
		itest= caller->findvar(parString);
		if ( itest < 0) {delcommon.push_back(ip); continue;}
		if( par->Type() == GDL_OBJ) {
/*			if(trace_me) // deliberately skpping code unless in debug mode.
				std::cout << " ** delvar on object "<< parString<< std::endl;
			else { */
				std::cout << " ** DELVAR - object " + parString + 
				"	skipped !! use obj_destroy " << std::endl;
				continue;
				}
/*	GDLDelete may not be adequate for cleanup.  Problems calling this way.
// ObjCleanup wants to reference _EXTRA keyword but DELVAR does not support that.
	 		DObjGDL* op= static_cast<DObjGDL*>(par);
			SizeT nEl=op->N_Elements();
			for( SizeT i=0; i<nEl; i++)	
				e->ObjCleanup( (*op)[i]);

 			}*/
		delvar.push_back(itest); ndel++;
		if (ndel == 32) break;
		pIndex = caller->findvar(par);
		if(debug) cout << " delvar_pro: "+parString+" pro.var, .env= "
		  <<itest << " : "<< pIndex <<endl;
			  
		}
	if(!delvar.empty()) {
			sort(delvar.begin(), delvar.end());
        	int ndel=0;
			for (std::vector<int>::iterator ix=delvar.begin();
			                           ix< delvar.end(); ix++) todel[ndel++] = (*ix);
			if(trace_me) std::cout << " ** delvar x-"<< ndel;
			todel[ndel] = -1;
			caller->Remove(todel);
			if(trace_me) std::cout << std::endl;
		}
	if(delcommon.empty()) return;
	unsigned cIx;
	int ncmnfound=0;
	std::vector<DCommonBase*> c;
	DSubUD* proUD   = dynamic_cast<DSubUD*>(caller->GetPro());
	proUD->commonPtrs(c);
	for (std::vector<int>::iterator ix=delcommon.begin();
			                           ix< delcommon.end(); ix++) {
		int i;
		BaseGDL*& par=e->GetPar(*ix);
		for( i=0; i < c.size(); i++) {
			cIx = c[i]->Find(par);
			if(cIx >= 0) break;
		  }
		if(cIx >= 0) {
		    DString parString = c[i]->VarName( cIx) +" ("+c[i]->Name()+')';
		    help_item( cout , par, parString, false);
 		    c[i]->Var(cIx)->Delete(); // this would delete the variable. 
		    ncmnfound++;			// but 
			} else {
				cout << " lost variable" << endl;
			}
		}
//	if(ncmnfound > 0) 
//		    cout << "( delvar will not handle common-block variables)"<< endl;
	if(ncmnfound != delcommon.size())
			cout << "( delvar did not find common block variables that it should have!"<<endl;
    }
#if 1
void findvar_pro( EnvT* e) { cout << " debugger routine: edit/recompile to use" << endl; return;}
#elif 0   
void findvar_pro( EnvT* e)
    {
      static volatile bool debug(true);
      static volatile SizeT numEnv;
      
	  SizeT n_Param = e->NParam();
	  //bool isKWSetStructures = e->KeywordSet( "STRUCTURES");
	bool isKWSetStructures = (n_Param == 0);
	  static EnvBaseT* caller = e->Caller();

	  bool foundit=false;
	  int sIndex,pIndex;
	  for( SizeT i=0; i<n_Param; i++)
	    {
	      BaseGDL*& par=e->GetPar( i);
		DString parString = caller->GetString( par,true);
	      if(debug) {
			help_item(cout, par, parString, false);
			}
		sIndex = caller->findvar(parString);
		pIndex = caller->findvar(par);
		cout << " findvar: "+parString+" pro.var, .env= "
			  <<sIndex << " : "<< pIndex <<endl;
		
		foundit = (sIndex >= 0);
	  
//	  if(foundit) continue;
		  
	    }
	if(!isKWSetStructures) return;
	std::string name;
	bool started = false;
	for(StructListT::iterator is = structList.begin(); 
		is != structList.end(); is++) {
		name = (*is)->Name();
		if(started) {
			if((*is)->IsUnnamed()) name ="Anonymous";
			cout << "{"+name+"} (" << (*is)->NTags() <<" tags) " ;
			}
			else started = (name.compare("!DEVICE") == 0);
			}
		cout << endl;

    }
#endif

} // namespace
