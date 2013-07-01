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
#include <dirent.h>

#include <string>
#include <fstream>
#include <memory>
#include <set>
#include <iterator>

#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <sys/wait.h>
#endif

#ifdef __APPLE__
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
#ifdef _MSC_VER
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
#define F_OK    0       /* Test for existence.  */
#else
#include <unistd.h>
#endif
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

#ifdef HAVE_EXT_STDIO_FILEBUF_H
#  include <ext/stdio_filebuf.h> // TODO: is it portable across compilers?
#endif
#include <signal.h>

#ifdef HAVE_LIBWXWIDGETS
#include <wx/wx.h>
#endif

namespace lib {
 
  using namespace std;

  // control !CPU settings
  void cpu( EnvT* e)
  {
    static int resetIx = e->KeywordIx( "RESET");
    static int restoreIx = e->KeywordIx( "RESTORE");
    static int max_eltsIx = e->KeywordIx( "TPOOL_MAX_ELTS");
    static int min_eltsIx = e->KeywordIx( "TPOOL_MIN_ELTS");
    static int nThreadsIx = e->KeywordIx( "TPOOL_NTHREADS");
    static int vectorEableIx = e->KeywordIx( "VECTOR_ENABLE");

    bool reset = e->KeywordSet( resetIx);
    bool restore = e->KeywordSet( restoreIx);
    if ((reset) && (restore)) e->Throw("Conflicting keywords.");

    bool vectorEnable = e->KeywordSet( vectorEableIx);

    DLong NbCOREs=1;
#ifdef _OPENMP
    NbCOREs=omp_get_num_procs();
#endif

    DLong locCpuTPOOL_NTHREADS=CpuTPOOL_NTHREADS;
    DLong locCpuTPOOL_MIN_ELTS=CpuTPOOL_MIN_ELTS;
    DLong locCpuTPOOL_MAX_ELTS=CpuTPOOL_MAX_ELTS;

    // reading the Tag Index of the variable parts in !CPU
    DStructGDL* cpu = SysVar::Cpu();
    static unsigned NTHREADSTag = cpu->Desc()->TagIndex( "TPOOL_NTHREADS");
    static unsigned TPOOL_MIN_ELTSTag = cpu->Desc()->TagIndex( "TPOOL_MIN_ELTS");
    static unsigned TPOOL_MAX_ELTSTag = cpu->Desc()->TagIndex( "TPOOL_MAX_ELTS");

    if( reset)
      {
	locCpuTPOOL_NTHREADS = NbCOREs;
        locCpuTPOOL_MIN_ELTS = DefaultTPOOL_MIN_ELTS;
        locCpuTPOOL_MAX_ELTS = DefaultTPOOL_MAX_ELTS;
      }
    else if( e->KeywordPresent( restoreIx))
      {
	DStructGDL* restoreCpu = e->GetKWAs<DStructGDL>( restoreIx);
	
	if( restoreCpu->Desc() != cpu->Desc())
	  e->Throw("RESTORE must be set to an instance with the same struct layout as {!CPU}");
	
        locCpuTPOOL_NTHREADS = (*(static_cast<DLongGDL*>( restoreCpu->GetTag( NTHREADSTag, 0))))[0];
        locCpuTPOOL_MIN_ELTS = (*(static_cast<DLongGDL*>( restoreCpu->GetTag( TPOOL_MIN_ELTSTag, 0))))[0];
        locCpuTPOOL_MAX_ELTS= (*(static_cast<DLongGDL*>( restoreCpu->GetTag( TPOOL_MAX_ELTSTag, 0))))[0];
      }
    else
      {
	if( e->KeywordPresent(nThreadsIx))
	  {
	    e->AssureLongScalarKW(nThreadsIx, locCpuTPOOL_NTHREADS);
	  }
	if( e->KeywordPresent(min_eltsIx))
	  {
	    e->AssureLongScalarKW(min_eltsIx, locCpuTPOOL_MIN_ELTS);
	  }
	if( e->KeywordPresent(max_eltsIx))
	  {
	    e->AssureLongScalarKW(max_eltsIx, locCpuTPOOL_MAX_ELTS);
	  }
      }

    // update here all together in case of error
    
#ifdef _OPENMP
    //cout <<locCpuTPOOL_NTHREADS << " " << CpuTPOOL_NTHREADS << endl;
    if (locCpuTPOOL_NTHREADS > 0) {
      CpuTPOOL_NTHREADS=locCpuTPOOL_NTHREADS;
    } else {
      CpuTPOOL_NTHREADS=NbCOREs;
    }
    if (CpuTPOOL_NTHREADS > NbCOREs)
      Warning("CPU : Warning: Using more threads ("+i2s(CpuTPOOL_NTHREADS)+") than the number of CPUs in the system ("+i2s(NbCOREs)+") will degrade performance.");
#else
    CpuTPOOL_NTHREADS=1;
#endif
    CpuTPOOL_MIN_ELTS=locCpuTPOOL_MIN_ELTS;
    CpuTPOOL_MAX_ELTS=locCpuTPOOL_MAX_ELTS;

    // update !CPU system variable
    (*static_cast<DLongGDL*>( cpu->GetTag( NTHREADSTag, 0)))[0] = CpuTPOOL_NTHREADS;
    (*static_cast<DLongGDL*>( cpu->GetTag( TPOOL_MIN_ELTSTag, 0)))[0] = CpuTPOOL_MIN_ELTS;
    (*static_cast<DLongGDL*>( cpu->GetTag( TPOOL_MAX_ELTSTag, 0)))[0] = CpuTPOOL_MAX_ELTS;

#ifdef _OPENMP
    omp_set_num_threads(CpuTPOOL_NTHREADS);
#endif
  }

  // display help for one variable or one structure tag
  void help_item( ostream& os,
		  BaseGDL* par, DString parString, bool doIndentation)
  {
    if( doIndentation) os << "   ";

    // Name display
    os.width(16);
    os << left << parString;
    if( parString.length() >= 16)
      {
        os << " " << endl; // for cmsv compatible output (uses help,OUTPUT)
        os.width(doIndentation? 19:16);
        os << "";
      }

    // Type display
    if( !par)
      {
        os << "UNDEFINED = !NULL" << endl;
        return;
      }
    os.width(10);
    os << par->TypeStr() << right;

    if( !doIndentation) os << "= ";

    // Data display
    if( par->Type() == GDL_STRUCT)
      {
        DStructGDL* s = static_cast<DStructGDL*>( par);
        os << "-> ";
        os << (s->Desc()->IsUnnamed()? "<Anonymous>" : s->Desc()->Name());
	os << " ";
      }
    else if( par->Dim( 0) == 0)
      {
        if (par->Type() == GDL_STRING)
	  {
            // trim string larger than 45 characters
            DString dataString = (*static_cast<DStringGDL*>(par))[0];
            os << "'" << StrMid( dataString,0,45,0) << "'";
	    if( dataString.length() > 45) os << "...";
	  }
	else
	  {
            par->ToStream( os);
	  }
      }

    // Dimension display
    if( par->Dim( 0) != 0) os << par->Dim();

    // End of line
    os << endl;
  }


  DStringGDL* recall_commands_internal()//EnvT* e)
  {
    //int status=0;
    DStringGDL* retVal;
    retVal = new DStringGDL(1, BaseGDL::NOZERO);
    (*retVal)[ 0] ="";

#if defined(HAVE_LIBREADLINE) && !defined(__APPLE__)
    //status=1;
    // http://cnswww.cns.cwru.edu/php/chet/readline/history.html#IDX14
    HIST_ENTRY **the_list;
    //    cout << "history_length" << history_length << endl;
    the_list = history_list ();

    if (the_list) {
      retVal = new DStringGDL( history_length-1, BaseGDL::NOZERO);
      for (SizeT i = 0; i<history_length-1 ; i++)
	(*retVal)[ i] = the_list[i]->line;
    }
#else
//     if (status == 0) {
      Message("RECALL_COMMANDS: nothing done, because compiled without READLINE");
//     }
#endif
    return retVal;
  }
 
  BaseGDL* recall_commands( EnvT* e)
  {
    return recall_commands_internal();
  }

  void help_path_cached()  // showing HELP, /path_cache
  {
    DIR *dirp;
    struct dirent *dp;
    const char *ProSuffix=".pro";
    int ProSuffixLen = strlen(ProSuffix);
    int NbProFilesInCurrentDir;
    string tmp_fname;
    size_t found;

    StrArr path=SysVar::GDLPath();

    cout << "!PATH (no cache managment in GDL, "<< path.size()  << " directories)" << endl;

    for( StrArr::iterator CurrentDir=path.begin(); CurrentDir != path.end(); CurrentDir++)
      {
	NbProFilesInCurrentDir=0;
	dirp = opendir((*CurrentDir).c_str());
	while ((dp = readdir(dirp)) != NULL){
	  tmp_fname=dp->d_name;
	  found = tmp_fname.rfind(ProSuffix);
	  if (found!=std::string::npos) {
	    if ((found+ProSuffixLen) == tmp_fname.length()) NbProFilesInCurrentDir++;
	  }
	}
	cout << *CurrentDir << " (" << NbProFilesInCurrentDir << " files)" << endl;
      }
  }

  void help( EnvT* e)
  {    
    bool kw = false;
    //if LAST_MESSAGE is present, it is the only otput. All other kw are ignored.
    static int lastmKWIx = e->KeywordIx("LAST_MESSAGE");
    bool lastmKW = e->KeywordPresent( lastmKWIx);
    if( lastmKW)
    {
      DStructGDL* errorState = SysVar::Error_State();
      static unsigned msgTag = errorState->Desc()->TagIndex( "MSG");
      cout << (*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0]<< endl;
      return;
    }

    static int helpKWIx = e->KeywordIx("HELP");
    bool helpKW= e->KeywordPresent(helpKWIx);
    if( helpKW) {
      string inline_help[]={"Usage: "+e->GetProName()+", expr1, ..., exprN,", 
			    "          /BRIEF, /CALLS, /FUNCTIONS, /HELP, /INFO,",
			    "          /INTERNAL_LIB_GDL, /LAST_MESSAGE, /LIB, /MEMORY,",
			    "          /OUTPUT, /PATH_CACHE, /PREFERENCES, /PROCEDURES,",
			    "          /RECALL_COMMANDS, /ROUTINES, /SOURCE_FILES, /STRUCTURES,"};
      int size_of_s = sizeof(inline_help) / sizeof(inline_help[0]);	
      e->Help(inline_help, size_of_s);
    }
    
    static int pathKWIx = e->KeywordIx("PATH_CACHE");
    bool pathKW= e->KeywordPresent(pathKWIx);
    if( pathKW) {
      help_path_cached();
      return;
    }
    
    static int sourceFilesKWIx = e->KeywordIx("SOURCE_FILES");
    bool sourceFilesKW = e->KeywordPresent( sourceFilesKWIx);
    if( sourceFilesKW)
      {
	deque<string> sourceFiles;
	
	for(FunListT::iterator i=funList.begin(); i != funList.end(); ++i)
	  {
	    string funFile = (*i)->GetFilename();
	    bool alreadyInList = false;
	    for(deque<string>::iterator i2=sourceFiles.begin(); i2 != sourceFiles.end(); ++i2)
	      {
		if( funFile == *i2)
		{
		  alreadyInList = true;
		  break;
		}
	    }
	    if( !alreadyInList)
	      sourceFiles.push_back(funFile);
	}
	for(ProListT::iterator i=proList.begin(); i != proList.end(); ++i)
	{
	    string proFile = (*i)->GetFilename();
	    bool alreadyInList = false;
	    for(deque<string>::iterator i2=sourceFiles.begin(); i2 != sourceFiles.end(); ++i2)
	    {
		if( proFile == *i2)
		{
		  alreadyInList = true;
		  break;
		}
	    }
	    if( !alreadyInList)
	      sourceFiles.push_back(proFile);
	}
	// sourceFiles now contains a uniqe list of all file names.
	sort( sourceFiles.begin(), sourceFiles.end());

      	SizeT nSourceFiles = sourceFiles.size();
	cout << "Source files (" << nSourceFiles <<"):" << endl;
	for( SizeT i = 0; i<nSourceFiles; ++i)
	  cout << sourceFiles[ i] << endl;
    }
    
    static int callsKWIx = e->KeywordIx("CALLS");
    bool callsKW = e->KeywordPresent( callsKWIx);
    if( callsKW)
      {
	EnvStackT& cS = e->Interpreter()->CallStack();

	SizeT level = cS.size();

	assert( level > 1); // HELP, $MAIN$

	DStringGDL* retVal = new DStringGDL( dimension( level-1), BaseGDL::NOZERO);
	SizeT rIx = 0;
	for( EnvStackT::reverse_iterator r = cS.rbegin()+1; r != cS.rend(); ++r)
	  {
	    EnvBaseT* actEnv = *r;
	    assert( actEnv != NULL);

	    DString actString = actEnv->GetProName();
	    DSubUD* actSub = dynamic_cast<DSubUD*>(actEnv->GetPro());
	    if( (r+1) != cS.rend() && actSub != NULL)
	      {
		actString += " <"+actSub->GetFilename() + "(";
		if( (*(r-1))->CallingNode() != NULL)
		  actString += i2s( (*(r-1))->CallingNode()->getLine(), 4);
		else
		  actString += "   ?";
		actString += ")>";
	      }
	    
	    (*retVal)[ rIx++] = actString;
	  }

	e->SetKW( callsKWIx, retVal);
	return;
      }

    if( e->KeywordSet( "INFO"))
      {
	kw = true;

	cout << "Homepage: http://gnudatalanguage.sf.net" << endl;
	cout << "HELP,/LIB for a list of all internal library "
	  "functions/procedures." << endl;
	cout << "Additional subroutines are written in GDL language, "
	  "look for *.pro files." << endl;
	cout << endl;
      }

    bool kwLib = e->KeywordSet( "LIB"); 
    if( kwLib)
      {
	kw = true;

	deque<DString> subList;
	SizeT nPro = libProList.size();
	for( SizeT i = 0; i<nPro; ++i)
	{
	  if( !libProList[ i]->GetHideHelp())
	    subList.push_back(libProList[ i]->ToString());
	}
	sort( subList.begin(), subList.end());

	SizeT nProList = subList.size();
	cout << "Library procedures (" << nProList <<"):" << endl;
	for( SizeT i = 0; i<nProList; ++i)
	  cout << subList[ i] << endl;

	subList.clear();

	SizeT nFun = libFunList.size();
	for( SizeT i = 0; i<nFun; ++i)
	{
	  if( !libFunList[ i]->GetHideHelp())
	    subList.push_back(libFunList[ i]->ToString());
	}
	sort( subList.begin(), subList.end());

	SizeT nFunList = subList.size();
	cout << "Library functions (" << nFunList <<"):" << endl;
	for( SizeT i = 0; i<nFunList; ++i)
	  cout << subList[ i] << endl;
      }

    // internal library functions
    bool kwLibInternal = e->KeywordSet( "INTERNAL_LIB_GDL"); 
    if( kwLibInternal)
      {
	kw = true;

	deque<DString> subList;
	SizeT nPro = libProList.size();
	for( SizeT i = 0; i<nPro; ++i)
	{
	  if( libProList[ i]->GetHideHelp()) // difference here
	    subList.push_back(libProList[ i]->ToString());
	}
	sort( subList.begin(), subList.end());

	SizeT nProList = subList.size();
	cout << "NOTE: Internal subroutines are subject to change without notice." << endl;
	cout << "They should never be called directly from a GDL program." << endl;
	cout << "Internal library procedures (" << nProList <<"):" << endl;
	for( SizeT i = 0; i<nProList; ++i)
	  cout << subList[ i] << endl;

	subList.clear();

	SizeT nFun = libFunList.size();
	for( SizeT i = 0; i<nFun; ++i)
	{
	  if( libFunList[ i]->GetHideHelp()) // difference here
	    subList.push_back(libFunList[ i]->ToString());
	}
	sort( subList.begin(), subList.end());

	SizeT nFunList = subList.size();
	cout << "Internal library functions (" << nFunList <<"):" << endl;
	for( SizeT i = 0; i<nFunList; ++i)
	  cout << subList[ i] << endl;
      }

    bool isKWSetStructures = e->KeywordSet( "STRUCTURES");
    if( isKWSetStructures) kw = true;

    bool isKWSetProcedures = e->KeywordSet( "PROCEDURES");
    bool isKWSetFunctions  = e->KeywordSet( "FUNCTIONS");
   
    if (isKWSetStructures && (isKWSetProcedures || isKWSetFunctions))
      e->Throw( "Conflicting keywords.");	
    
    bool isKWSetRecall  = e->KeywordSet( "RECALL_COMMANDS");
    if (isKWSetRecall && (isKWSetProcedures || isKWSetFunctions))
      e->Throw( "Conflicting keywords.");
    
    bool isKWSetMemory  = e->KeywordSet( "MEMORY");
    if (isKWSetMemory && (isKWSetProcedures || isKWSetFunctions))
      e->Throw( "Conflicting keywords.");

    bool isKWSetPreferences  = e->KeywordSet( "PREFERENCES");
    if (isKWSetPreferences && (isKWSetProcedures || isKWSetFunctions))
      e->Throw( "Conflicting keywords.");

    // using this way, we don't need to manage HAVE_READLINE at this level ...
    if (isKWSetRecall) {
      DStringGDL *previous_commands;
      previous_commands=recall_commands_internal();
      SizeT nEl2 = previous_commands->N_Elements();
      cout << "Recall buffer length: " << nEl2 << endl;
	for( SizeT i=0; i<nEl2; ++i)
	  cout << i+1 << "  " <<(*previous_commands)[i] << endl;
      return;
    }

    SizeT nParam=e->NParam();
    std::ostringstream ostr;

    // Compiled Procedures & Functions
    DLong np = proList.size() + 1;
    DLong nf = funList.size();
    deque<DString> pList;
    deque<DString> fList;

    // If OUTPUT keyword set then set up output string array (outputKW)
    BaseGDL** outputKW = NULL;
    static int outputIx = e->KeywordIx( "OUTPUT");
    if( e->KeywordPresent( outputIx)) {
      SizeT nlines = 0;
      if (isKWSetProcedures) {
	nlines = np + 1;
      } else if (isKWSetFunctions) {
	nlines = nf + 1;
      } else {
	// Determine the number of entries in the output array
	for( SizeT i=0; i<nParam; i++)
	  {
	    BaseGDL*& par=e->GetPar( i);
	    DString parString = e->Caller()->GetString( par, true);
	    if( !par || !isKWSetStructures || par->Type() != GDL_STRUCT) {
	      nlines++;
	    } else {
	      DStructGDL* s = static_cast<DStructGDL*>( par);
	      SizeT nTags = s->Desc()->NTags();
	      nlines++;
	      nlines += nTags;
	    }
	  }

	// Add space for compiled procedures & functions
	if (nParam == 0) {
	  // list all variables of caller
	  EnvBaseT* caller = e->Caller();
	  SizeT nEnv = caller->EnvSize();
	  nlines = nEnv + 5;
	}
      }

      // Setup output return variable
      outputKW = &e->GetKW( outputIx);
      GDLDelete((*outputKW));
      dimension dim(&nlines, (size_t) 1);
      *outputKW = new DStringGDL(dim, BaseGDL::NOZERO);
    }

    // switch to dec output (might be changed from formatted output)
    if( outputKW == NULL)
      cout << dec;

    static int routinesKWIx = e->KeywordIx("ROUTINES");
    static int briefKWIx = e->KeywordIx("BRIEF");
    bool routinesKW = e->KeywordSet( routinesKWIx);
    bool briefKW = e->KeywordSet( briefKWIx);
    SizeT nOut = 0;
    
    if ((nParam == 0 && !isKWSetMemory) || isKWSetFunctions || isKWSetProcedures) {

      if (nParam == 0 && !isKWSetFunctions && !isKWSetProcedures) {
	// Tell where we are
	DSubUD* pro = static_cast<DSubUD*>( e->Caller()->GetPro());
	if (outputKW == NULL) {
	  cout << "% At " << pro->ObjectName() << endl;
	} else {
	  ostr << "% At " << pro->ObjectName();
	  (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	  ostr.str("");
	}
      }

      // Get list of user procedures
      pList.push_back("$MAIN$");
      for( ProListT::iterator i=proList.begin(); i != proList.end(); i++)
	pList.push_back((*i)->ObjectName());
      sort( pList.begin(), pList.end());

      // Get list of user functions
      for( FunListT::iterator i=funList.begin(); i != funList.end(); i++)
	fList.push_back((*i)->ObjectName());
      sort( fList.begin(), fList.end());

      // PROCEDURES keyword
      if (isKWSetProcedures) {
	if (outputKW == NULL) {
	    cout << "Compiled Procedures:" << endl;
	} else {
	  ostr << "Compiled Procedures:";
	  (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	  ostr.str("");
	}

	// Loop through procedures
	for( SizeT i=0; i<np; i++) {
	  // Add $MAIN$
	  if (i == 0) {
	    if (outputKW == NULL) {
	      cout << "$MAIN$";
	    } else {
	      ostr << "$MAIN$";
	      (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	      ostr.str("");
	    }
	  }

	  // Find DPro pointer for pList[i]
	  ProListT::iterator p=std::find_if(proList.begin(),proList.end(),
					    Is_eq<DPro>(pList[i]));
	  if( p != proList.end()) {
	    DPro *pro = *p;
	    int nPar = pro->NPar();
	    int nKey = pro->NKey();

	    // Loop through parameters
	    if (outputKW == NULL) {
	      cout << setw(25) << left << pro->ObjectName() << setw(0);
	      for( SizeT j=0; j<nPar; j++)
		cout << StrLowCase(pro->GetVarName(nKey+j)) << " ";
	    } else {
	      ostr << setw(25) << left << pro->ObjectName() << setw(0);
	      for( SizeT j=0; j<nPar; j++)
		ostr << StrLowCase(pro->GetVarName(nKey+j)) << " ";
	      (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	      ostr.str("");
	    }
	  }
	  if (outputKW == NULL) cout << endl;
	}
	// FUNCTIONS keyword
      } else if (isKWSetFunctions) {

	if (outputKW == NULL) {
	  cout << "Compiled Functions:" << endl;
	} else {
	  ostr << "Compiled Functions:";
	  (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	  ostr.str("");
	}

	// Loop through functions
	for( SizeT i=0; i<nf; i++) {

	  // Find DFun pointer for fList[i]
	  FunListT::iterator p=std::find_if(funList.begin(),funList.end(),
					    Is_eq<DFun>(fList[i]));
	  if( p != funList.end()) {
	    DFun *pro = *p;
	    int nPar = pro->NPar();
	    int nKey = pro->NKey();

	    // Loop through parameters
	    if (outputKW == NULL) {
	      cout << setw(25) << left << pro->ObjectName() << setw(0);
	      for( SizeT j=0; j<nPar; j++)
		cout << StrLowCase(pro->GetVarName(nKey+j)) << " ";
	    } else {
	      ostr << setw(25) << left << pro->ObjectName() << setw(0);
	      for( SizeT j=0; j<nPar; j++)
		ostr << StrLowCase(pro->GetVarName(nKey+j)) << " ";
	      (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	      ostr.str("");
	    }
	  }
	  if (outputKW == NULL) cout << endl;
	}
      }
      if( isKWSetProcedures) return;
      if( isKWSetFunctions)  return;
    } 
    else if (isKWSetMemory)
    {
      std::ostream* ostrp = outputKW == NULL ? &cout : &ostr;
      *ostrp << "heap memory used: ";
      *ostrp << MemStats::GetCurrent();
      *ostrp << ", max: ";
      *ostrp << MemStats::GetHighWater();
      *ostrp << ", gets: ";
      *ostrp << MemStats::GetNumAlloc();
      *ostrp << ", frees: ";
      *ostrp << MemStats::GetNumFree();
      if (outputKW == NULL) cout << endl;
      else
      {
        (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
        ostr.str("");
      }
      return;
    } 

    // Excluding keywords which are exclusive is not finished ...
    if (isKWSetPreferences)
      {	
	//cout << "ici 1 " << isKWSetPreferences << endl;
	std::ostream* ostrp = outputKW == NULL ? &cout : &ostr;
	*ostrp << "Preferences";
	// this is not ready ...
	//*ostrp << GDL_GR_X_QSCREEN::GetValue();
	if (outputKW == NULL) cout << endl;
	else
	  {
	    (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	    ostr.str("");
	  }
	return;	
      }
    
    //    cout << "ici 2" << isKWSetPreferences << endl;

    for( SizeT i=0; i<nParam; i++)
      {
	BaseGDL*& par=e->GetPar( i);
	DString parString = e->Caller()->GetString( par, true);
	// NON-STRUCTURES
	if( !par || !isKWSetStructures || par->Type() != GDL_STRUCT)
          {
	    // If no OUTPUT keyword send to stdout
	    if (outputKW == NULL) {
	      help_item( cout, par, parString, false);
	    } else {
	      // else send to string stream & store in outputKW (remove CR)
	      help_item( ostr, par, parString, false);
	      (*(DStringGDL *) *outputKW)[nOut++] = 
		ostr.rdbuf()->str().erase(ostr.rdbuf()->str().length()-1,1); 
	      ostr.str("");
	    }
          }
        else
	  {
	    // STRUCTURES
            DStructGDL* s = static_cast<DStructGDL*>( par);
	    SizeT nTags = s->Desc()->NTags();
	    if (outputKW == NULL) {
	      cout << "** Structure ";
	      cout << (s->Desc()->IsUnnamed() ? "<Anonymous>" : 
		       s->Desc()->Name());
	      cout << ", " << nTags << " tags";
	      cout << ", data length=" << s->Sizeof();
              cout << ":" << endl;
	      for (SizeT t=0; t < nTags; ++t)
		{    
		  DString tagString = s->Desc()->TagName(t);
		  help_item( cout, s->GetTag(t), tagString, true);
		}
	    } else {
	      // OUTPUT KEYWORD SET
	      ostr << "** Structure ";
	      ostr << (s->Desc()->IsUnnamed() ? "<Anonymous>" : 
		       s->Desc()->Name());
	      ostr << ", " << nTags << " tags";
	      ostr << ", data length=" << s->Sizeof();
              ostr << ":";
	      (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	      ostr.str("");

	      for (SizeT t=0; t < nTags; ++t)
		{    
		  DString tagString = s->Desc()->TagName(t);
		  help_item( ostr, s->GetTag(t), tagString, true);
		  (*(DStringGDL *) *outputKW)[nOut++] = 
		    ostr.rdbuf()->str().erase(ostr.rdbuf()->str().
					      length()-1,1);
		  ostr.str("");
		}
	    }
	  }
      }
    if( routinesKW || briefKW) kw = true;

    if( nParam == 0 && !kw)
      {
	routinesKW = true;
	briefKW = true;

	// list all variables of caller
	EnvBaseT* caller = e->Caller();

	SizeT nEnv = caller->EnvSize();

	set<string> helpStr;  // "Sorted List" 
	for ( int i = 0; i < nEnv; ++i ) 
	  {
	    BaseGDL*& par=caller->GetKW( i);
	    if( par == NULL) 
	      continue;

	    DString parString = caller->GetString( par,true);
	    
	    stringstream ss;
	    help_item( ss, par, parString, false);
	    
	    helpStr.insert( ss.str() );
	  }

	  if (outputKW == NULL) {
	    copy( helpStr.begin(), helpStr.end(),
		  ostream_iterator<string>( cout) );
	  }


	// Display compiled procedures & functions
	if (!isKWSetProcedures && !isKWSetFunctions) {
	  // StdOut
	  if (outputKW == NULL) {
	    cout << "Compiled Procedures:" << endl;
	    for( SizeT i=0; i<np; i++) cout << pList[i] << " ";
	    cout << endl << endl;

	    cout << "Compiled Functions:" << endl;
	    for( SizeT i=0; i<nf; i++) cout << fList[i] << " ";
	    cout << endl;
	  } else {
	    // Keyword Output

	    // Output variables
	    set<string>::iterator it = helpStr.begin(); 
	    while(it != helpStr.end()) { 
	      ostr << *it;

	      (*(DStringGDL *) *outputKW)[nOut++] = 
		ostr.rdbuf()->str().erase(ostr.rdbuf()->str().length()-1,1); 

	      ++it;
	      ostr.str("");
	    } 

	    // Output procedures & functions
	    ostr << "Compiled Procedures:";
	    (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	    ostr.str("");

	    for( SizeT i=0; i<np; i++) ostr << pList[i] << " ";
	    (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	    ostr.str("");

	    ostr << "Compiled Functions:";
	    (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	    ostr.str("");

	    for( SizeT i=0; i<nf; i++) ostr << fList[i] << " ";
	    (*(DStringGDL *) *outputKW)[nOut++] = ostr.rdbuf()->str();
	    ostr.str("");
	  }

	}
    }

// 	stringstream strS;
// 	for( SizeT i=0; i<nEnv; ++i)
// 	  {
// 	    BaseGDL*& par=caller->GetKW( i);
// 	    if( par == NULL) 
// 	      continue;
	    
// 	    DString parString = caller->GetString( par);

// 	    help_item(  &strS, par, parString, false);
// 	  }

// 	deque<DString> toSort;
// 	for( SizeT i=0; i<nEnv; ++i)
// 	  {
// 	    char buf[ 256];
// 	    strS.getline( buf, 256);
// 	    toSort.push_back( buf);
// 	  }
// 	sort( toSort.begin(), toSort.end());
// 	for( SizeT i=0; i<nEnv; ++i)
// 	  {
// 	    cout << toSort[ i] << endl;
// 	  }
  }

  void exitgdl( EnvT* e)
  {

#ifdef HAVE_LIBREADLINE

    // we manage the ASCII "history" file (located in ~/.gdl/)
    // we do not manage NOW the number of lines we save,
    // this should be limited by "history/readline" itself

    if( historyIntialized)
    {
		// Create eventually the ".gdl" path in user $HOME
		int result, debug=0;
		char *homeDir = getenv( "HOME");
		if (homeDir != NULL)
		{
			string pathToGDL_history = homeDir;
			AppendIfNeeded(pathToGDL_history, "/");
			pathToGDL_history += ".gdl";
			// Create eventially the ".gdl" path in Home
#ifdef _MSC_VER
			result = mkdir(pathToGDL_history.c_str());
#else
			result = mkdir(pathToGDL_history.c_str(), 0700);
#endif
			if (debug)
			{
				if (result == 0) cout << "Creation of ~/.gdl PATH "<< endl;
				else cout << "~/.gdl PATH was still here "<< endl;
			}
			
			// (over)write the history file in ~/.gdl PATH
		
			AppendIfNeeded(pathToGDL_history, "/");
			string history_filename = pathToGDL_history + "history";
			if (debug) cout << "History file name: " << history_filename << endl;
			result = write_history(history_filename.c_str());
			if (debug)
			{
				if (result == 0) cout<<"Successfull writing of ~/.gdl/history"<<endl;
				else cout <<"Fail to write ~/.gdl/history"<<endl;
			}
		}
    }
#endif

#ifdef HAVE_LIBWXWIDGETS
    // wxTheApp may be a null pointer (tracker item no. 2946058)
    if (wxTheApp) wxTheApp->OnExit(); // Defined in GDLApp::OnExit() in gdlwidget.cpp

    // SA: gives the following error message with no connection to X-server:
    //   GDL> exit
    //   Error: Unable to initialize gtk, is DISPLAY set properly?
    //wxUninitialize();
#endif

    sem_onexit();

    BaseGDL* status=e->GetKW( 1);
    if( status == NULL) exit( EXIT_SUCCESS);
    
    if( !status->Scalar())
      e->Throw( "Expression must be a scalar in this context: "+
		e->GetString( status));

    DLongGDL* statusL=static_cast<DLongGDL*>(status->Convert2( GDL_LONG, 
							       BaseGDL::COPY));
    
    DLong exit_status;
    statusL->Scalar( exit_status);
    exit( exit_status);
  }

  void heap_gc( EnvT* e)
  {
    static SizeT objIx = e->KeywordIx( "OBJ");
    static SizeT ptrIx = e->KeywordIx( "PTR");
    static SizeT verboseIx = e->KeywordIx( "VERBOSE");
    bool doObj = e->KeywordSet( objIx);
    bool doPtr = e->KeywordSet( ptrIx);
    bool verbose =  e->KeywordSet( verboseIx);
    if( !doObj && !doPtr)
      doObj = doPtr = true;

    e->HeapGC( doPtr, doObj, verbose);
  }
  
  void HeapFreeObj( EnvT* env, BaseGDL* var, bool verbose)
  {
    if( var == NULL)
      return;
    if( var->Type() == GDL_STRUCT)
    {
      DStructGDL* varStruct = static_cast<DStructGDL*>( var);
      DStructDesc* desc = varStruct->Desc();
      for( SizeT e=0; e<varStruct->N_Elements(); ++e)
	for( SizeT t=0; t<desc->NTags(); ++t)
	{
	    BaseGDL* actElementTag = varStruct->GetTag( t, e);
	    HeapFreeObj( env, actElementTag, verbose);
	}
    }
    else if( var->Type() == GDL_PTR)
    {
      // descent into pointer
      DPtrGDL* varPtr = static_cast<DPtrGDL*>( var);
      for( SizeT e=0; e<varPtr->N_Elements(); ++e)
      {
	DPtr actPtrID = (*varPtr)[e];
	if( actPtrID == 0)
	  continue;
	
	BaseGDL* derefPtr = DInterpreter::GetHeap( actPtrID);
	HeapFreeObj( env, derefPtr, verbose);
      }
    }
    else if( var->Type() == GDL_OBJ)
    {
      DObjGDL* varObj = static_cast<DObjGDL*>( var);
      for( SizeT e=0; e<varObj->N_Elements(); ++e)
      {
	DObj actID = (*varObj)[e];
	if( actID == 0)
	  continue;
	
	if( verbose)
	{
	  BaseGDL* derefObj = DInterpreter::GetObjHeap( actID);
	  help_item( cout, 
		      derefObj, DString( "<ObjHeapVar")+
		      i2s(actID)+">",
		      false);
	}
	// 2. free object
	env->ObjCleanup( actID);
      }
    }
  }
  
  void HeapFreePtr( BaseGDL* var, bool verbose)
  {
    if( var == NULL)
      return;
    if( var->Type() == GDL_STRUCT)
    {
      DStructGDL* varStruct = static_cast<DStructGDL*>( var);
      DStructDesc* desc = varStruct->Desc();
      for( SizeT e=0; e<varStruct->N_Elements(); ++e)
	for( SizeT t=0; t<desc->NTags(); ++t)
	{
	    BaseGDL* actElementTag = varStruct->GetTag( t, e);
	    HeapFreePtr( actElementTag, verbose);  // recursive call
	}
    }
    else if( var->Type() == GDL_PTR)
    {
      // 1. descent into pointer
      DPtrGDL* varPtr = static_cast<DPtrGDL*>( var);
      for( SizeT e=0; e<varPtr->N_Elements(); ++e)
      {
	DPtr actPtrID = (*varPtr)[e];
	if( actPtrID == 0)
	  continue;
	
	BaseGDL* derefPtr = DInterpreter::GetHeap( actPtrID);
	if( verbose)
	{
	  help_item( cout, 
		      derefPtr, DString( "<PtrHeapVar")+
		      i2s(actPtrID)+">",
		      false);
	}
	HeapFreePtr( derefPtr, verbose); // recursive call
      }
      // 2. free pointer
      DInterpreter::FreeHeap( varPtr);
    }
  }
  
  void heap_free( EnvT* e)
  {
    static SizeT objIx = e->KeywordIx( "OBJ");
    static SizeT ptrIx = e->KeywordIx( "PTR");
    static SizeT verboseIx = e->KeywordIx( "VERBOSE");
    bool doObj = e->KeywordSet( objIx);
    bool doPtr = e->KeywordSet( ptrIx);
    bool verbose =  e->KeywordSet( verboseIx);
    if( !doObj && !doPtr)
      doObj = doPtr = true;

    e->NParam( 1);
    BaseGDL* p0 = e->GetParDefined(0);
   
    if( doObj) // do first objects as they may in turn free some pointers
      HeapFreeObj( e, p0, verbose);
    if( doPtr)
      HeapFreePtr( p0, verbose);
  }

  void ptr_free( EnvT* e)
  {
    SizeT nParam=e->NParam();
    for( SizeT i=0; i<nParam; i++)
      {
	BaseGDL* p = e->GetPar( i);
	if( p == NULL)
	{
	  e->Throw( "Pointer type required"
		    " in this context: "+e->GetParString(i));	  
	}
	if( p->Type() != GDL_PTR)
	{
	  e->Throw( "Pointer type required"
		    " in this context: "+e->GetParString(i));	  
	}
	DPtrGDL* par=static_cast<DPtrGDL*>(e->GetPar( i));
	e->FreeHeap( par);
      }
  }

  void obj_destroy( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam == 0) return;
    
    BaseGDL* p= e->GetParDefined( 0);

    if( p->Type() != GDL_OBJ)
      e->Throw( "Parameter must be an object in"
		" this context: "+
		e->GetParString(0));
    DObjGDL* op= static_cast<DObjGDL*>(p);

    SizeT nEl=op->N_Elements();
    for( SizeT i=0; i<nEl; i++)
      {
	DObj actID=(*op)[i];
	e->ObjCleanup( actID);
      }
  }
  
  void call_procedure( EnvT* e)
  {
    int nParam=e->NParam();
    if( nParam == 0)
      e->Throw( "No procedure specified.");
    
    DString callP;
    e->AssureScalarPar<DStringGDL>( 0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase( callP);

    // first search library procedures
    int proIx=LibProIx( callP);
    if( proIx != -1)
      {
// 	e->PushNewEnv( libProList[ proIx], 1);
	// make the call
// 	EnvT* newEnv = static_cast<EnvT*>(e->Interpreter()->CallStack().back());
	EnvT* newEnv = e->NewEnv( libProList[ proIx], 1);
	Guard<EnvT> guard( newEnv);
	static_cast<DLibPro*>(newEnv->GetPro())->Pro()(newEnv);
      }
    else
      {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

	proIx = DInterpreter::GetProIx( callP);
	
	e->PushNewEnvUD( proList[ proIx], 1);
	
	// make the call
	EnvUDT* newEnv = static_cast<EnvUDT*>(e->Interpreter()->CallStack().back());
	e->Interpreter()->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->
				   GetTree());
      }
  }

  void call_method_procedure( EnvT* e)
  {
    StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

    int nParam=e->NParam();
    if( nParam < 2)
      e->Throw( "Name and object reference must be specified.");
    
    DString callP;
    e->AssureScalarPar<DStringGDL>( 0, callP);

    // this is a procedure name -> convert to UPPERCASE
    callP = StrUpCase( callP);

    DStructGDL* oStruct = e->GetObjectPar( 1);
    
    DPro* method= oStruct->Desc()->GetPro( callP);

    if( method == NULL)
      e->Throw( "Method not found: "+callP);

    e->PushNewEnvUD( method, 2, &e->GetPar( 1));
    
    // the call
    e->Interpreter()->call_pro( method->GetTree());
  }

  void get_lun( EnvT* e)
  {
    int nParam=e->NParam( 1);
    
    // not using SetPar later gives a better error message
    e->AssureGlobalPar( 0);
    
    // here lun is the GDL lun, not the internal one
    DLong lun = GetLUN();

    if( lun == 0)
      e->Throw( "All available logical units are currently in use.");

    BaseGDL** retLun = &e->GetPar( 0);
    
    GDLDelete((*retLun)); 
    //            if( *retLun != e->Caller()->Object()) delete (*retLun); 
    
    *retLun = new DLongGDL( lun);
    return;
  }

  // returns true if lun points to special unit
  // lun is GDL lun (-2..128)
  bool check_lun( EnvT* e, DLong lun)
  {
    if( lun < -2 || lun > maxLun)
      e->Throw( "File unit is not within allowed range: "+
		i2s(lun)+".");
    return (lun <= 0);
  }
  
  // TODO: handle ON_ERROR, ON_IOERROR, !ERROR_STATE.MSG
  void open_lun( EnvT* e, fstream::openmode mode)
  {
    int nParam=e->NParam( 2);
    
    if( e->KeywordSet( "GET_LUN")) get_lun( e);
    // par 0 contains now the LUN

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      e->Throw( "Unit already open. Unit: "+i2s( lun));
    
    DString name;
    // IDL allows here also arrays of length 1
    e->AssureScalarPar<DStringGDL>( 1, name); 

    // Change leading "~" to home directory
//     if (name.substr(0,2) == "~/")
//       name = getenv("HOME") + name.substr(1,name.size()-1);

    // endian
    bool swapEndian=false;
    static int swapIx = e->KeywordIx( "SWAP_ENDIAN");
    static int swapIfBigIx = e->KeywordIx( "SWAP_IF_BIG_ENDIAN");
    static int swapIfLittleIx = e->KeywordIx( "SWAP_IF_LITTLE_ENDIAN");
    if( e->KeywordSet( swapIx))
      swapEndian = true;
    else if( BigEndian())
      swapEndian = e->KeywordSet( swapIfBigIx);
    else
      swapEndian = e->KeywordSet( swapIfLittleIx);
    
    // compress
    bool compress=false;
    static int compressIx = e->KeywordIx( "COMPRESS");
    if( e->KeywordSet( compressIx))
      compress = true;

    // xdr
    static int xdrIx = e->KeywordIx( "XDR");
    bool xdr = e->KeywordSet( xdrIx);

    static int appendIx = e->KeywordIx( "APPEND");
    // if( e->KeywordSet( appendIx)) mode |= fstream::ate;// fstream::app;
    // SA: trunc flag for non-existent file is needed in order to comply with
    // IDL behaviour (creating a file even if /APPEND flag is set) (tracker bug 2103871)
    if (e->KeywordSet(appendIx))
    { 
	if( compress)
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

    static int f77Ix = e->KeywordIx( "F77_UNFORMATTED");
    bool f77 = e->KeywordSet( f77Ix);

    static int delIx = e->KeywordIx( "DELETE");
    bool deleteKey = e->KeywordSet( delIx);
    
    static int errorIx = e->KeywordIx( "ERROR");
    bool errorKeyword = e->KeywordPresent( errorIx);
    if( errorKeyword) e->AssureGlobalKW( errorIx);

    DLong width = defaultStreamWidth;
    static int widthIx = e->KeywordIx( "WIDTH");
    BaseGDL* widthKeyword = e->GetKW( widthIx);
    if( widthKeyword != NULL)
      {
	e->AssureLongScalarKW( widthIx, width);
      }

    // Assume variable-length VMS file initially
    // fileUnits[ lun-1].PutVarLenVMS( true);

   // m_schellens: this is no good. It fails for regular files which by accident fit the 
   // variable-length VMS file criteria (see bug tracker ID: 3028279)
   // we need something more sophisticated here
   fileUnits[ lun-1].PutVarLenVMS( false);

    try{
      fileUnits[ lun-1].Open( name, mode, swapEndian, deleteKey, 
			      xdr, width, f77, compress);
    } 
    catch( GDLException& ex) {
      DString errorMsg = ex.getMessage()+ // getMessage gets the non-decorated error message
	" Unit: "+i2s( lun)+", File: "+fileUnits[ lun-1].Name();
      
      if( !errorKeyword)
	throw GDLIOException( ex.ErrorCode(), e->CallingNode(), errorMsg);
      
      BaseGDL** err = &e->GetKW( errorIx);
      
      GDLDelete(*err); 
//    if( *err != e->Caller()->Object()) delete (*err); 
      
      *err = new DLongGDL( ex.ErrorCode());
      return;
    }

    if( errorKeyword)
      {
	BaseGDL** err = &e->GetKW( errorIx);
      
// 	if( *err != e->Caller()->Object()) delete (*err); 
	GDLDelete((*err)); 
      
	*err = new DLongGDL( 0);
      }

  }
  
  void openr( EnvT* e)
  {
    open_lun( e, fstream::in);
  }

  void openw( EnvT* e)
  {
    open_lun( e, fstream::in | fstream::out | fstream::trunc);
  }

  void openu( EnvT* e)
  {
    open_lun( e, fstream::in | fstream::out);
  }
  
  void socket( EnvT* e)
  {
    int nParam=e->NParam( 3);
    
    if( e->KeywordSet( "GET_LUN")) get_lun( e);
    // par 0 contains now the LUN

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      e->Throw( "Unit already open. Unit: "+i2s( lun));
    
    DString host;
    // IDL allows here also arrays of length 1
    e->AssureScalarPar<DStringGDL>( 1, host); 

    DUInt port;
    BaseGDL* p2 = e->GetParDefined( 2);
    if (p2->Type() == GDL_STRING) {
      // look up /etc/services
    } else if (p2->Type() == GDL_UINT) {
      e->AssureScalarPar<DUIntGDL>( 2, port);
    } else if (p2->Type() == GDL_INT) {
      DInt p;
      e->AssureScalarPar<DIntGDL>( 2, p);
      port = p;
    } else if (p2->Type() == GDL_LONG) {
      DLong p;
      e->AssureScalarPar<DLongGDL>( 2, p);
      port = p;
    } else if (p2->Type() == GDL_ULONG) {
      DULong p;
      e->AssureScalarPar<DULongGDL>( 2, p);
      port = p;
    }

    // endian
    bool swapEndian=false;
    if( e->KeywordSet( "SWAP_ENDIAN"))
      swapEndian = true;
    else if( BigEndian())
      swapEndian = e->KeywordSet( "SWAP_IF_BIG_ENDIAN");
    else
      swapEndian = e->KeywordSet( "SWAP_IF_LITTLE_ENDIAN");

    DDouble c_timeout=0.0;
    e->AssureDoubleScalarKWIfPresent( "CONNECT_TIMEOUT", c_timeout);
    DDouble r_timeout=0.0;
    e->AssureDoubleScalarKWIfPresent( "READ_TIMEOUT",    r_timeout);
    DDouble w_timeout=0.0;
    e->AssureDoubleScalarKWIfPresent( "WRITE_TIMEOUT",   w_timeout);
   
    static int errorIx = e->KeywordIx( "ERROR");
    bool errorKeyword = e->KeywordPresent( errorIx);
    if( errorKeyword) e->AssureGlobalKW( errorIx);

    DLong width = defaultStreamWidth;
    static int widthIx = e->KeywordIx( "WIDTH");
    BaseGDL* widthKeyword = e->GetKW( widthIx);
    if( widthKeyword != NULL)
      {
	e->AssureLongScalarKW( widthIx, width);
      }

    try{
      fileUnits[ lun-1].Socket( host, port, swapEndian,
				c_timeout, r_timeout, c_timeout);
    } 
    catch( GDLException& ex) {
      DString errorMsg = ex.toString()+" Unit: "+i2s( lun)+
	", File: "+fileUnits[ lun-1].Name();
      
      if( !errorKeyword)
	e->Throw( errorMsg);
      
      BaseGDL** err = &e->GetKW( errorIx);
      
      GDLDelete((*err)); 
//    if( *err != e->Caller()->Object()) delete (*err); 
      
      *err = new DLongGDL( 1);
      return;
    }

    if( errorKeyword)
      {
	BaseGDL** err = &e->GetKW( errorIx);
      
// 	if( *err != e->Caller()->Object()) delete (*err); 
	GDLDelete((*err)); 
      
	*err = new DLongGDL( 0);
      }
  }

  // FLUSH based on a patch from Orion Poplawski
  void flush_lun( EnvT* e)
  {
    // within GDL, always lun+1 is used
    int nParam = e->NParam();
    for (int p = 0; p < nParam; p++)
    {
      DLong lun;
      e->AssureLongScalarPar(p, lun);
      if (lun > maxLun) 
        e->Throw("File unit is not within allowed range: " + i2s(lun) + ".");
      else if (lun == -2) 
        cerr << flush;
      else if (lun == -1)
        cout << flush; 
      else if (lun == 0)
        ;// do nothing?
      else 
        fileUnits[lun - 1].Flush();
    }
  }

  void close_free_lun( EnvT* e, bool freeLun)
  {
    DLong journalLUN = SysVar::JournalLUN();

    // within GDL, always lun+1 is used
    if( e->KeywordSet("ALL"))
      for( int p=maxUserLun; p<maxLun; ++p)
	{
	  if( (journalLUN-1) != p)
	    {
	      fileUnits[ p].Close();
	      //	      if( freeLun) 
	      fileUnits[ p].Free();
	    }
	}
    
    if( e->KeywordSet("FILE") || e->KeywordSet("ALL"))
      for( int p=0; p<maxUserLun; ++p)
	{
	  fileUnits[ p].Close();
	  // freeing not necessary as get_lun does not use them
	  //if( freeLun) fileUnits[ p].Free();
	}
    
    int nParam=e->NParam();
    for( int p=0; p<nParam; p++)
      {
	DLong lun;
	e->AssureLongScalarPar( p, lun);
	if( lun > maxLun)
	  e->Throw( "File unit is not within allowed range: "+
		    i2s(lun)+".");
	if( lun < 1)
	  e->Throw( "File unit does not allow this operation."
		    " Unit: "+i2s(lun)+".");

	if( lun == journalLUN)
	  e->Throw(  "Reserved file cannot be closed in this manner. Unit: "+
		     i2s( lun));
	
	fileUnits[ lun-1].Close();
	if( freeLun) fileUnits[ lun-1].Free();
      }
  }

  void close_lun( EnvT* e)
  {
    close_free_lun( e, false);
  }
  
  void free_lun( EnvT* e)
  {
    close_free_lun( e, true);
  }

  void writeu( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    ostream* os=NULL;
    ogzstream* ogzs=NULL;
    bool f77 = false;
    bool swapEndian = false;
    bool compress = false;
    XDR *xdrs = NULL;

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      {
	if( lun == 0)
	  e->Throw( "Cannot write to stdin. Unit: "+i2s( lun));

	os = (lun == -1)? &cout : &cerr;
      }
    else
      {
        if (!fileUnits[lun-1].IsOpen())
          e->Throw("File unit is not open: " + i2s(lun));

	compress = fileUnits[ lun-1].Compress();
	if( !compress)
		os = &fileUnits[ lun-1].OStream();
	else
		ogzs = &fileUnits[ lun-1].OgzStream();
	f77 = fileUnits[ lun-1].F77();
	swapEndian = fileUnits[ lun-1].SwapEndian();
	xdrs = fileUnits[ lun-1].Xdr();
      }

    if( f77)
      {
	if(compress)
		e->Throw("COMPRESS not supported for F77.");

	// count record length
	SizeT nBytesAll = 0;
	for( SizeT i=1; i<nParam; i++)
	  {
	    BaseGDL* p = e->GetParDefined( i);
	    nBytesAll += p->NBytes();
	  }
	
	// write record length
	fileUnits[ lun-1].F77Write( nBytesAll);

	// write data
	for( SizeT i=1; i<nParam; i++)
	  {
	    BaseGDL* p = e->GetPar( i); // defined already checkede
	    p->Write( *os, swapEndian, compress, xdrs);
	  }

	// write record length
		fileUnits[ lun-1].F77Write( nBytesAll);
	}
  else
	if( compress)
	{
		for( SizeT i=1; i<nParam; i++)
			{
			BaseGDL* p = e->GetParDefined( i);
			p->Write( *ogzs, swapEndian, compress, xdrs);
			}
	}
	else
	{
		for( SizeT i=1; i<nParam; i++)
			{
			BaseGDL* p = e->GetParDefined( i);
			p->Write( *os, swapEndian, compress, xdrs);
			}
	}
}

  void readu( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    istream* is = NULL;
    igzstream* igzs = NULL;
    bool f77 = false;
    bool varlenVMS = false;
    bool swapEndian = false;
    bool compress = false;
    XDR *xdrs = NULL;
    int sockNum = fileUnits[ lun-1].SockNum();

    bool stdLun = check_lun( e, lun);
    if( stdLun)
      {
	if( lun != 0)
	  e->Throw( "Cannot read from stdout and stderr."
		    " Unit: "+i2s( lun));
	is = &cin;
      }
    else if (sockNum != -1)
      {
	// Socket Read
	swapEndian = fileUnits[ lun-1].SwapEndian();

	compress = fileUnits[ lun-1].Compress();

	string *recvBuf = &fileUnits[ lun-1].RecvBuf();

	// Setup recv buffer & string
	const int MAXRECV = 2048*8;
	char buf[MAXRECV+1];

	// Read socket until finished & store in recv string
	while (1) {
	  memset (buf, 0, MAXRECV+1);
	  int status = recv(sockNum, buf, MAXRECV, 0);
	  //	  cout << "Bytes received: " << status << endl;
	  if (status == 0) break;
	  for( SizeT i=0; i<status; i++) 
	    recvBuf->push_back(buf[i]);
	}

	// Get istringstream, write recv string, & assign to istream
	istringstream *iss = &fileUnits[ lun-1].ISocketStream();
	iss->str(*recvBuf);
	is = iss;
      }
    else
      {
	compress = fileUnits[ lun-1].Compress();
	if( !compress)
		is = &fileUnits[ lun-1].IStream();
	else
		igzs = &fileUnits[ lun-1].IgzStream();
	f77 = fileUnits[ lun-1].F77();
	varlenVMS = fileUnits[ lun-1].VarLenVMS();
	swapEndian = fileUnits[ lun-1].SwapEndian();
	xdrs = fileUnits[ lun-1].Xdr();
      }


    if( f77)
      {
	SizeT recordLength = fileUnits[ lun-1].F77ReadStart();

	SizeT relPos = 0;
	for( SizeT i=1; i<nParam; i++)
	  {
	    BaseGDL* p = e->GetPar( i);
	    if( p == NULL)
	      {
		e->AssureGlobalPar( i);
		p = new DFloatGDL( 0.0);
		e->SetPar( i, p);
	      }

	    SizeT nBytes = p->NBytes();

	    if( (relPos + nBytes) > recordLength)
	      e->Throw( "Attempt to read past end of F77_UNFORMATTED "
			"file record.");

	    p->Read( *is, swapEndian, compress, xdrs);

	    relPos += nBytes;
	  }

	// forward to next record if necessary
	fileUnits[ lun-1].F77ReadEnd();
      }
    else
      for( SizeT i=1; i<nParam; i++)
	{
	  BaseGDL* p = e->GetPar( i);
	  //	  cout << p->Rank() << endl; // JMG
	  if( p == NULL)
	    {
	      e->AssureGlobalPar( i);
	      p = new DFloatGDL( 0.0);
	      e->SetPar( i, p);
	    }

	  if (compress) 
	  {
	    p->Read( *igzs, swapEndian, compress, xdrs);
	  } 
	  else if (varlenVMS && i == 1) 
	  {
	    // Check if VMS variable-length file
	    char hdr[4], tmp;

	    // Read possible record header
	    is->read(hdr, 4);

	    DLong nRec1;
	    memcpy(&nRec1, hdr, 4);

	    // switch endian
	    tmp = hdr[3]; hdr[3] = hdr[0]; hdr[0] = tmp;
	    tmp = hdr[2]; hdr[2] = hdr[1]; hdr[1] = tmp;

	    DLong nRec2;
	    memcpy(&nRec2, hdr, 4);
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
	      fileUnits[ lun-1].PutVarLenVMS( false);
	    }
	    p->Read( *is, swapEndian, compress, xdrs);
	  }
	  else 
		p->Read( *is, swapEndian, compress, xdrs);

	  // Socket Read
	  if (sockNum != -1) {
	    int pos = is->tellg();
	    string *recvBuf = &fileUnits[ lun-1].RecvBuf();
	    //	    cout << "pos: " << pos << endl;
	    recvBuf->erase(0, pos);
	  }
	}
  }

  void on_error( EnvT* e)
  {
    e->OnError();
  }

  void catch_pro( EnvT* e)
  {
//     static bool warned = false;
//     if (!warned) {
//       Warning("CATCH: feature not implemented yet (FIXME!).");
//       warned = true;
//     }
    e->Catch();
  }

  void strput( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);
    
    BaseGDL* p0 = e->GetParGlobal( 0);
    if( p0->Type() != GDL_STRING)
      e->Throw( "String expression required in this context: "+
		e->GetParString(0));
    DStringGDL* dest = static_cast<DStringGDL*>( p0);
      
    DString source;
    e->AssureStringScalarPar( 1, source);
    
    DLong pos = 0;
    if (nParam == 3)
      {
	e->AssureLongScalarPar( 2, pos);
	if (pos < 0) pos = 0;
      }

    SizeT nEl = dest->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
    for( OMPInt i=0; i<nEl; ++i)
	StrPut((*dest)[ i], source, pos);
}
  }

  void retall( EnvT* e)
  {
    e->Interpreter()->RetAll();
  }

  void stop( EnvT* e)
  {
    if( e->NParam() > 0) print( e);
    debugMode = DEBUG_STOP;
  }

  void defsysv( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);

    DString sysVarNameFull;
    e->AssureStringScalarPar( 0, sysVarNameFull);
    
    static int existIx = e->KeywordIx( "EXIST");
    if( e->KeywordPresent( existIx))
      {
	if( sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!')
	  {
	    e->SetKW( existIx, new DLongGDL( 0));
	  }
	
	DVar* sysVar = FindInVarList( sysVarList,
				      StrUpCase( sysVarNameFull.substr(1)));
	if( sysVar == NULL)
	  e->SetKW( existIx, new DLongGDL( 0));
	else
	  e->SetKW( existIx, new DLongGDL( 1));
	return;
      }
    else if( nParam < 2)
      e->Throw( "Incorrect number of arguments.");
    
    // here: nParam >= 2
    DLong rdOnly = 0;
    if( nParam >= 3)
      e->AssureLongScalarPar( 2, rdOnly);

    if( sysVarNameFull.length() < 2 || sysVarNameFull[0] != '!')
      e->Throw( "Illegal system variable name: "+sysVarNameFull+".");
    
    // strip "!", uppercase
    DString sysVarName = StrUpCase( sysVarNameFull.substr(1)); 
    
    DVar* sysVar = FindInVarList( sysVarList, sysVarName);

    // check if the variable is defined
    BaseGDL* p1 = e->GetParDefined( 1);

    if( sysVar == NULL)
      {
	// define new
	DVar *newSysVar = new DVar( sysVarName, p1->Dup());
	sysVarList.push_back( newSysVar);

	// rdOnly is only set at the first definition
	if( rdOnly != 0)
	  sysVarRdOnlyList.push_back( newSysVar);
	return;
      }

    // re-set
    // make sure type and size are kept
    BaseGDL* oldVar = sysVar->Data();
    BaseGDL* newVar = p1;
    if( oldVar->Type()       != newVar->Type() ||
	oldVar->N_Elements() != newVar->N_Elements())
      e->Throw( "Conflicting definition for "+sysVarNameFull+".");

    // if struct -> assure equal descriptors
    if( oldVar->Type() == GDL_STRUCT)
      {
	DStructGDL *oldStruct =  static_cast<DStructGDL*>( oldVar);
	// types are same -> static cast
	DStructGDL *newStruct =  static_cast<DStructGDL*>( newVar);

	// note that IDL handles different structs more relaxed
	// ie. just the structure pattern is compared.
	if( *oldStruct->Desc() != *newStruct->Desc())
	  e->Throw( "Conflicting definition for "+sysVarNameFull+".");
      }
	
    DVar* sysVarRdOnly = FindInVarList( sysVarRdOnlyList, sysVarName);
    if( sysVarRdOnly != NULL)
      {
	// rdOnly set and is already rdOnly: do nothing
	if( rdOnly != 0) return; 

	// else complain
	e->Throw( "Attempt to write to a readonly variable: "+
		  sysVarNameFull+".");
      }
    else
      {
	// not read only
	GDLDelete(oldVar);
	sysVar->Data() = newVar->Dup();

	// only on first definition
	//	if( rdOnly != 0)
	//	  sysVarRdOnlyList.push_back( sysVar);
      }
  }

  // note: this implemetation does not honor all keywords
  void message( EnvT* e)
  {
    SizeT nParam = e->NParam();

    static int continueIx = e->KeywordIx( "CONTINUE");
    static int infoIx = e->KeywordIx( "INFORMATIONAL");
    static int ioerrorIx = e->KeywordIx( "IOERROR");
    static int nonameIx = e->KeywordIx( "NONAME");
    static int noprefixIx = e->KeywordIx( "NOPREFIX");
    static int noprintIx = e->KeywordIx( "NOPRINT");
    static int resetIx = e->KeywordIx( "RESET");
    static int reissueIx = e->KeywordIx( "REISSUE_LAST");

    bool continueKW = e->KeywordSet( continueIx);
    bool info = e->KeywordSet( infoIx);
    bool ioerror = e->KeywordSet( ioerrorIx);
    bool noname = e->KeywordSet( nonameIx);
    bool noprefix = e->KeywordSet( noprefixIx);
    bool noprint = e->KeywordSet( noprintIx);
    bool reset = e->KeywordSet( resetIx);
    bool reissue = e->KeywordSet( reissueIx);

    if( reset)
    {
      DStructGDL* errorState = SysVar::Error_State();
      static unsigned nameTag = errorState->Desc()->TagIndex( "NAME");
      static unsigned blockTag = errorState->Desc()->TagIndex( "BLOCK");
      static unsigned codeTag = errorState->Desc()->TagIndex( "CODE");
      static unsigned rangeTag = errorState->Desc()->TagIndex( "RANGE");
      static unsigned sys_code_typeTag = errorState->Desc()->TagIndex( "SYS_CODE_TYPE");
      static unsigned msgTag = errorState->Desc()->TagIndex( "MSG");
      static unsigned sys_msgTag = errorState->Desc()->TagIndex( "SYS_MSG");
      static unsigned msg_prefixTag = errorState->Desc()->TagIndex( "MSG_PREFIX");

      (*static_cast<DStringGDL*>( errorState->GetTag( nameTag)))[0] = "IDL_M_SUCCESS";
      (*static_cast<DStringGDL*>( errorState->GetTag( blockTag)))[0] = "IDL_MBLK_CORE";
      (*static_cast<DLongGDL*>( errorState->GetTag( codeTag)))[0] = 0;
      (*static_cast<DLongGDL*>( errorState->GetTag( rangeTag)))[0] = 0;
      (*static_cast<DLongGDL*>( errorState->GetTag( rangeTag)))[1] = 0;
      (*static_cast<DStringGDL*>( errorState->GetTag( sys_code_typeTag)))[0] = "";
      (*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0] = "";
      (*static_cast<DStringGDL*>( errorState->GetTag( sys_msgTag)))[0] = "";
      (*static_cast<DStringGDL*>( errorState->GetTag( msg_prefixTag)))[0] = "% ";
      
      SysVar::SetErr_String( "");     
      SysVar::SetErrError( 0);
    }
    
    if( reissue )
    {
      DStructGDL* errorState = SysVar::Error_State();
      static unsigned msgTag = errorState->Desc()->TagIndex( "MSG");
      if( !info || (SysVar::Quiet() == 0)) cout << (*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0]<< endl;
      return;
    }

    if( nParam == 0) return;

    DString msg;
    e->AssureScalarPar<DStringGDL>( 0, msg);

    if( !noname)
      msg = e->Caller()->GetProName() + ": " + msg;

    if( !info)
    {
      DStructGDL* errorState = SysVar::Error_State();
      static unsigned codeTag = errorState->Desc()->TagIndex( "CODE");
      (*static_cast<DLongGDL*>( errorState->GetTag( codeTag)))[0] = 0;
      static unsigned msgTag = errorState->Desc()->TagIndex( "MSG");
      (*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0] = msg;

      SysVar::SetErr_String( msg);
      SysVar::SetErrError( -1);
    }

    if( noprint)
      msg = "";
    
    if( !continueKW && !info)
      throw GDLException( msg, !noprefix, false);
    
    if( !noprint && !noprefix)
      msg = SysVar::MsgPrefix() + msg;

    if( !info || (SysVar::Quiet() == 0))
      cout << msg << endl;
  }

  void byteorderDo( EnvT* e, BaseGDL* pIn, SizeT swapSz, DLong p)
{
	if( pIn->Type() == GDL_STRUCT)
	{
		DStructGDL* dS=static_cast<DStructGDL*>( pIn);
		if( dS->Desc()->ContainsStringPtrObject())
			e->Throw( "Structs must not contain PTR, OBJECT or STRING tags: "+e->GetParString(p));
		for( SizeT t=0; t<dS->NTags(); ++t)
		{
			BaseGDL* par = dS->GetTag( t);
			
			if( par->Type() == GDL_STRUCT && par->N_Elements() == 1)
			{
				// do tag by tag for scalar struct as memory might not be contigous (
				byteorderDo( e, par, swapSz, p);
			}
			else
			{
				
				SizeT nBytes = par->NBytes();
				if( nBytes % swapSz != 0)
				e->Throw( "Operand's size must be a multiple of swap "
					"datum size: " + e->GetParString(p));		    
				
				SizeT nSwap = nBytes / swapSz;
			
				char* addr = static_cast<char*>(par->DataAddr());
			
				for( SizeT i=0; i<nSwap; ++i)
				{
					for( SizeT s=0; s < (swapSz/2); ++s)
					{
						char tmp = *(addr+i*swapSz+s);
						*(addr+i*swapSz+s) = *(addr+i*swapSz+swapSz-1-s);
						*(addr+i*swapSz+swapSz-1-s) = tmp;
					}
				}
			}
		}
	}
	else
	{
		if( pIn->Type() == GDL_STRING)
		e->Throw( "STRING type not allowed in this context: "+e->GetParString(p));		    
		if( pIn->Type() == GDL_OBJ)
		e->Throw( "Object type not allowed in this context: "+e->GetParString(p));		    
		if( pIn->Type() == GDL_PTR)
		e->Throw( "PTR type not allowed in this context: "+e->GetParString(p));		    
	
		BaseGDL*& par = pIn;
		SizeT nBytes = par->NBytes();
		if( nBytes % swapSz != 0)
		e->Throw( "Operand's size must be a multiple of swap "
			"datum size: " + e->GetParString(p));		    
		
		SizeT nSwap = nBytes / swapSz;
	
		char* addr = static_cast<char*>(par->DataAddr());
	
		for( SizeT i=0; i<nSwap; ++i)
		{
			for( SizeT s=0; s < (swapSz/2); ++s)
			{
				char tmp = *(addr+i*swapSz+s);
				*(addr+i*swapSz+s) = *(addr+i*swapSz+swapSz-1-s);
				*(addr+i*swapSz+swapSz-1-s) = tmp;
			}
		}
	}
}

  void byteorder( EnvT* e)
  {
    SizeT nParam = e->NParam( 1);

    //    static int sswapIx = e->KeywordIx( "SSWAP");
    static int lswapIx = e->KeywordIx( "LSWAP");
    static int l64swapIx = e->KeywordIx( "L64SWAP");
    static int ifBigIx = e->KeywordIx( "SWAP_IF_BIG_ENDIAN");
    static int ifLittleIx = e->KeywordIx( "SWAP_IF_LITTLE_ENDIAN");
    static int ntohlIx = e->KeywordIx( "NTOHL");
    static int ntohsIx = e->KeywordIx( "NTOHS");
    static int htonlIx = e->KeywordIx( "HTONL");
    static int htonsIx = e->KeywordIx( "HTONS");
    static int ftoxdrIx = e->KeywordIx( "FTOXDR");
    static int dtoxdrIx = e->KeywordIx( "DTOXDR");
    static int xdrtofIx = e->KeywordIx( "XDRTOF");
    static int xdrtodIx = e->KeywordIx( "XDRTOD");

    bool lswap = e->KeywordSet( lswapIx);
    bool l64swap = e->KeywordSet( l64swapIx);
    bool ifBig = e->KeywordSet( ifBigIx);
    bool ifLittle = e->KeywordSet( ifLittleIx);

    // to-from network conversion (big-endian)
    bool ntohl = e->KeywordSet( ntohlIx);
    bool ntohs = e->KeywordSet( ntohsIx);
    bool htonl = e->KeywordSet( htonlIx);
    bool htons = e->KeywordSet( htonsIx);

    // XDR to-from conversion
    bool ftoxdr = e->KeywordSet( ftoxdrIx);
    bool dtoxdr = e->KeywordSet( dtoxdrIx);
    bool xdrtof = e->KeywordSet( xdrtofIx);
    bool xdrtod = e->KeywordSet( xdrtodIx);

    if( ifBig && !BigEndian()) return;
    if( ifLittle && BigEndian()) return;

    if( BigEndian() && (ntohl || ntohs || htonl || htons)) return;

    for( DLong p=nParam-1; p>=0; --p)
      {
	BaseGDL* par = e->GetParDefined( p);
	if( !e->GlobalPar( p))
	  e->Throw( "Expression must be named variable in this context: "+
		    e->GetParString(p));		    

	SizeT swapSz = 2; 
	if( l64swap || dtoxdr || xdrtod)
	  swapSz = 8;
	else if( lswap || ntohl || htonl || ftoxdr || xdrtof)
	  swapSz = 4;

 	byteorderDo( e, par, swapSz, p);
      }
  }

  void setenv_pro( EnvT* e)
  {
    SizeT nParam = e->NParam();

    DStringGDL* name = e->GetParAs<DStringGDL>(0);
    SizeT nEnv = name->N_Elements();

    for(SizeT i=0; i < nEnv; ++i){
      DString strEnv = (*name)[i];
      long len = strEnv.length();
      long pos = strEnv.find_first_of("=", 0); 
      if( pos == string::npos) continue;   
      DString strArg = strEnv.substr(pos+1, len - pos - 1);
      strEnv = strEnv.substr(0, pos);
      // putenv() is POSIX unlike setenv()
      #if defined(__hpux__) || defined(_MSC_VER)
      int ret = putenv((strEnv+"="+strArg).c_str());
      #else
      int ret = setenv(strEnv.c_str(), strArg.c_str(), 1);
      #endif
    }
  } 

  void struct_assign_pro( EnvT* e)
  {
    SizeT nParam=e->NParam( 2);
 
    DStructGDL* source = e->GetParAs<DStructGDL>(0);
    DStructGDL* dest   = e->GetParAs<DStructGDL>(1);
  
    static int nozeroIx = e->KeywordIx("NOZERO");
    bool nozero = e->KeywordSet( nozeroIx); 

    static int verboseIx = e->KeywordIx("VERBOSE");
    bool verbose = e->KeywordSet( verboseIx);

    string sourceName = (*source).Desc()->Name();

    SizeT nTags = 0;

    // array of struct
    SizeT nElements = source->N_Elements();
    SizeT nDestElements = dest->N_Elements();
    if( nElements > nDestElements)
      nElements = nDestElements;

    // zero out the destination
    if( !nozero)
       (*dest).Clear();

    nTags = (*source).Desc()->NTags();

    // copy the stuff
    for(int t=0; t < nTags; ++t)
      {    
	string sourceTagName = (*source).Desc()->TagName(t);
	int ix = (*dest).Desc()->TagIndex( sourceTagName );
	if( ix >= 0)
	  {
	    SizeT nTagElements = source->GetTag( t)->N_Elements();
	    SizeT nTagDestElements = dest->GetTag( ix)->N_Elements();

	    if( verbose) 
	      {
		if( nTagElements > nTagDestElements)
		  Warning( "STRUCT_ASSIGN: " + sourceName + 
			   " tag " + sourceTagName + 
			   " is longer than destination. "
			   "The end will be clipped.");
		else if( nTagElements < nTagDestElements)
		  Warning( "STRUCT_ASSIGN: " + sourceName + 
			   " tag " + sourceTagName + 
			   " is shorter than destination. "
			   "The end will be zero filled.");
	      }

	    if( nTagElements > nTagDestElements)
		nTagElements = nTagDestElements;

	    for( SizeT a=0; a< nElements; ++a)
	      dest->GetTag( ix, a)->Assign( source->GetTag( t, a), nTagElements);
	  }
	else 
	  if(verbose)
	    Warning( "STRUCT_ASSIGN: Destination lacks " + sourceName +
		     " tag " + sourceTagName + ". Not copied.");
      }
  }

#ifndef _MSC_VER
 
  // helper function for spawn_pro
  static void child_sighandler(int x){
    pid_t pid;
    while((pid=waitpid(-1,NULL,WNOHANG)) > 0);
  }

  void spawn_pro( EnvT* e)
  {
    SizeT nParam = e->NParam();

    static int countIx = e->KeywordIx( "COUNT");
    bool countKeyword = e->KeywordPresent( countIx);
    if( countKeyword) e->AssureGlobalKW( countIx);

    static int pidIx = e->KeywordIx( "PID");
    bool pidKeyword = e->KeywordPresent( pidIx);
    if( pidKeyword) e->AssureGlobalKW( pidIx);
    
    static int exit_statusIx = e->KeywordIx( "EXIT_STATUS");
    bool exit_statusKeyword = e->KeywordPresent( exit_statusIx);
    if( exit_statusKeyword) e->AssureGlobalKW( exit_statusIx);

    static int shIx = e->KeywordIx( "SH");
    bool shKeyword = e->KeywordSet( shIx);
    
    static int noshellIx = e->KeywordIx( "NOSHELL");
    bool noshellKeyword = e->KeywordSet( noshellIx);
    
    static int unitIx = e->KeywordIx( "UNIT");
    bool unitKeyword = e->KeywordPresent( unitIx);
    if( unitKeyword) e->AssureGlobalKW( unitIx);

    if (unitKeyword)
    {
      if (exit_statusKeyword) 
      {
        Warning("SPAWN: specifying EXIT_STATUS with UNIT keyword has no meaning (assigning zero)");
        e->SetKW( exit_statusIx, new DLongGDL( 0));
      }
      if (countKeyword)
      {
        Warning("SPAWN: specifying COUNT with UNIT keyword has no meaning (assigning zero)");
        e->SetKW( countIx, new DLongGDL( 0)); 
      }
      if (nParam != 1) e->Throw("Invalid use of the UNIT keyword (only one argument allowed when using UNIT).");
    }

    string shellCmd;
    if( shKeyword) 
      shellCmd = "/bin/sh"; // must be there if POSIX
    else
      {
	char* shellEnv = getenv("SHELL");
	if (shellEnv == NULL)
	  e->Throw( "Error managing child process. "
		    "Environment variable SHELL not set.");
        shellCmd = shellEnv;
      }

    if( nParam == 0)
      { 
        int status = system( shellCmd.c_str());
        status >>= 8; 
	if( countKeyword)
	  e->SetKW( countIx, new DLongGDL( 0));
        if( exit_statusKeyword)
          e->SetKW( exit_statusIx, new DLongGDL( status));
	return;
      }

    // added on occasion of the UNIT kw patch
    if (unitKeyword) signal(SIGCHLD,child_sighandler);

    DStringGDL* command = e->GetParAs<DStringGDL>( 0);
    DString cmd = (*command)[0];

    const int bufSize = 1024;
    char buf[ bufSize];

    if( nParam > 1) e->AssureGlobalPar( 1);
    if( nParam > 2) e->AssureGlobalPar( 2);

    int coutP[2];
    if (nParam > 1 || unitKeyword)  
    {
      if (pipe(coutP)) return;
    }

    int cerrP[2];
    if( nParam > 2 && !unitKeyword && pipe(cerrP)) return;
	
    pid_t pid = fork(); // *** fork
    if( pid == -1) // error in fork
      {
	close( coutP[0]); close( coutP[1]);
        if( nParam > 2 && !unitKeyword) { close( cerrP[0]); close( cerrP[1]);}
	return;
      }

    if( pid == 0) // we are child
      {
        if (unitKeyword) 
        {
          dup2(coutP[1], 1); // cout
          dup2(coutP[1], 2); // cout
          close( coutP[0]); 
          close( coutP[1]);
        }
        else
        {
	  if( nParam > 1) dup2(coutP[1], 1); // cout
	  if( nParam > 2) dup2(cerrP[1], 2); // cerr

	  if( nParam > 1) 
          { 
            close( coutP[0]); 
            close( coutP[1]);
          }
	  if( nParam > 2) 
          { 
            close( cerrP[0]); 
            close( cerrP[1]);
          }
        }

	if( noshellKeyword)
	  {
	    SizeT nArg = command->N_Elements();
	    char** argv = new char*[ nArg+1];
	    argv[ nArg] = NULL;
	    for( SizeT i=0; i<nArg; ++i)
	      argv[i] = const_cast<char*>((*command)[i].c_str());
		
	    execvp( cmd.c_str(), argv);

	    delete[] argv; // only executes if exec fails
	  }
	else
	  execl( shellCmd.c_str(), shellCmd.c_str(), "-c", 
		 cmd.c_str(), (char *) NULL);

	Warning( "SPAWN: Error managing child process.");
	_exit(1); // error in exec
      }
    else // we are parent
      {
	if( pidKeyword)
	  e->SetKW( pidIx, new DLongGDL( pid));

        if( nParam > 1 || unitKeyword) close( coutP[1]);
        if( nParam > 2 && !unitKeyword) close( cerrP[1]);

        if (unitKeyword)
        {
#ifdef HAVE_EXT_STDIO_FILEBUF_H
          // UNIT kw code based on the patch by Greg Huey:
 
          Warning("Warning: UNIT keyword to SPAWN may not yet be fully implemented (proceeding)");
          // This is just code stolen from void get_lun( EnvT* e)
          // here lun is the GDL lun, not the internal one
          DLong unit_lun = GetLUN();

          if( unit_lun == 0)
            e->Throw( "SPAWN: Failed to get new LUN: GetLUN says: All available logical units are currently in use.");
 
           FILE *coutF;
           coutF = fdopen( coutP[0], "r");
           if( coutF == NULL) close( coutP[0]);

           e->SetKW( unitIx, new DLongGDL( unit_lun ));
           bool stdLun = check_lun( e, unit_lun);
           if( stdLun)
             e->Throw( "SPAWN: Failed to open new LUN: Unit already open. Unit: "+i2s( unit_lun));
           fileUnits[ unit_lun-1].PutVarLenVMS( false);
 
           // Here we invoke the black arts of converting from a C FILE*fd to an fstream object
           __gnu_cxx::stdio_filebuf<char> *frb_p;
           frb_p = new __gnu_cxx::stdio_filebuf<char>(coutF, std::ios_base::in);
 
           fileUnits[ unit_lun-1].Close();
           fileUnits[ unit_lun-1].Open("/dev/zero", std::ios_base::in, 0, 0, 0, 0, 0, 0);

           basic_streambuf<char> *bsrb_old_p;
           bsrb_old_p = fileUnits[ unit_lun-1].get_stream_readbuf_bsrb();
           fileUnits[ unit_lun-1].set_stream_readbuf_bsrb_from_frb(frb_p);
           fileUnits[ unit_lun-1].set_readbuf_frb_destroy_on_close(frb_p);
           fileUnits[ unit_lun-1].set_readbuf_bsrb_destroy_on_close(bsrb_old_p);
           fileUnits[ unit_lun-1].set_fd_close_on_close(coutP[0]);
#else
           e->Throw("UNIT kw. relies on GNU extensions to the std C++ library (that were not availble during compilation?)");
#endif

        }
        else
        {
	  FILE *coutF, *cerrF;
	  if( nParam > 1) 
	  {
	    coutF = fdopen( coutP[0], "r");
	    if( coutF == NULL) close( coutP[0]);
	  }
	  if( nParam > 2) 
	  {
	    cerrF = fdopen( cerrP[0], "r");
	    if( cerrF == NULL) close( cerrP[0]);
	  }

          vector<DString> outStr;
          vector<DString> errStr;
	    
          // read cout
          if( nParam > 1 && coutF != NULL)
	  {
            while( fgets(buf, bufSize, coutF) != NULL)
            {
              SizeT len = strlen( buf);
              if( len != 0 && buf[ len-1] == '\n') buf[ len-1] = 0;
              outStr.push_back( DString( buf));
            }
            fclose( coutF);
          }

          // read cerr
          if( nParam > 2 && cerrF != NULL) 
          {
            while( fgets(buf, bufSize, cerrF) != NULL)
            {
              SizeT len = strlen( buf);
              if( len != 0 && buf[ len-1] == '\n') buf[ len-1] = 0;
              errStr.push_back( DString( buf));
            }
            fclose( cerrF);
          }

          // wait until child terminates
          int status;
          pid_t wpid  = wait( &status);
	
          if( exit_statusKeyword)
            e->SetKW( exit_statusIx, new DLongGDL( status >> 8));
	    
          SizeT nLines = 0;
          if( nParam > 1)
          {
            DStringGDL* result;
            nLines = outStr.size();
            if( nLines == 0)
              result = new DStringGDL("");
            else 
            {
              result = new DStringGDL( dimension( nLines), BaseGDL::NOZERO);
              for( SizeT l=0; l<nLines; ++l) (*result)[ l] = outStr[ l];
            }
            e->SetPar( 1, result);
          }

          if( countKeyword) e->SetKW( countIx, new DLongGDL( nLines));
	    
          if( nParam > 2)
          {
            DStringGDL* errResult;
            SizeT nErrLines = errStr.size();
            if( nErrLines == 0)
              errResult = new DStringGDL("");
            else 
            {
              errResult = new DStringGDL( dimension( nErrLines), BaseGDL::NOZERO);
              for( SizeT l=0; l<nErrLines; ++l) (*errResult)[ l] = errStr[ l];
            }
            e->SetPar( 2, errResult);
          }
        }
      }
  }
#endif

  void replicate_inplace_pro( EnvT* e)
  {
    SizeT nParam = e->NParam( 2);

    if( nParam % 2)
      e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);
    if( !e->GlobalPar( 0))
      e->Throw( "Expression must be named variable in this context: "+
		e->GetParString(0));
    
    BaseGDL* p1 = e->GetParDefined( 1);
    if( !p1->Scalar())
      e->Throw( "Expression must be a scalar in this context: "+
		e->GetParString(1));
    p1 = p1->Convert2( p0->Type(), BaseGDL::COPY);
    
    if (nParam == 2)
      {
        p0->AssignAt( p1);
	return;
      } 
    else 
      {
 	BaseGDL* p2 = e->GetNumericParDefined( 2);
	if( !p2->StrictScalar())
	  e->Throw( "Expression must be a scalar in this context: "+
		    e->GetParString(2));
	
	SizeT d1;
	int ret = p2->Scalar2Index( d1);
	if( d1 < 1 || d1 > p0->Rank())
	  e->Throw( "D1 (3rd) argument is out of range: "+
		    e->GetParString(2));
	
// 	BaseGDL* p3 = e->GetNumericParDefined( 3);
	DLongGDL* p3 = e->GetParAs< DLongGDL>( 3);
	if( p3->N_Elements() != p0->Rank())
	  e->Throw( "Loc1 (4th) argument must have the same number of "
		    "elements as the dimensions of the X (1st) argument: "+
		    e->GetParString(3));

	SizeT d2 = 0;
	BaseGDL* p4;
	BaseGDL* p5;
	if (nParam > 4) 
	  {
	    p4 = e->GetNumericParDefined( 4);
	    if( !p4->StrictScalar())
	      e->Throw( "Expression must be a scalar in this context: "+
		        e->GetParString(4));
	    ret = p4->Scalar2Index( d2);
	    if( d2 < 1 || d2 > p0->Rank())
	      e->Throw( "D5 (5th) argument is out of range: "+
		        e->GetParString(4));

	    p5 = e->GetNumericParDefined( 5);
	  }

// 	ArrayIndexVectorT* ixList = new ArrayIndexVectorT();
// 	Guard< ArrayIndexVectorT> ixList_guard( ixList);
	ArrayIndexVectorT ixList; 
// 	BaseGDL* loc1 = p3->Dup();
// 	loc1->SetDim (dimension( loc1->N_Elements()));
//	ixList->reserve( p3->N_Elements());
	for (size_t i=0; i<p3->N_Elements(); i++)
	  if( (i+1) == d1)
	    ixList.push_back( new ArrayIndexAll());
	  else if( (i+1) == d2)
	    ixList.push_back( new CArrayIndexIndexed( p5, true));
	  else
	    ixList.push_back( new CArrayIndexScalar( (*p3)[ i]));//p3->NewIx(i)));
	ArrayIndexListT* ixL;
	MakeArrayIndex( &ixList, &ixL);
	Guard< ArrayIndexListT> ixL_guard( ixL);
	ixL->AssignAt( p0, p1);
	return;
      }
  }

  void resolve_routine( EnvT* e)
  {
    SizeT nParam=e->NParam(1); 
    
//     static int eitherIx = e->KeywordIx( "EITHER");
//     static int is_functionIx = e->KeywordIx( "IS_FUNCTION");
//     static int no_recompileIx = e->KeywordIx( "NO_RECOMPILE");

	BaseGDL* p0 = e->GetParDefined( 0);
	if( p0->Type() != GDL_STRING)
	      e->Throw( "Expression must be a string in this context: "+
		        e->GetParString(0));
	DStringGDL* p0S = static_cast<DStringGDL*>( p0);

	static StrArr openFiles;
	
	SizeT nEl = p0S->N_Elements();
	for( int i=0; i<nEl; ++i)
	{
		DString pro = (*p0S)[ i];

		string proFile=StrLowCase(pro);
		AppendIfNeeded( proFile, ".pro");

		bool found=CompleteFileName(proFile);
		if( !found)
			e->Throw("Not found: " + proFile);
	
		// file already opened?
		bool open = false;
		for( StrArr::iterator i=openFiles.begin(); i != openFiles.end(); i++)
		{
			if( proFile == *i)
			{
				open = true;
				break;
			}
		}
		if( open)
			continue;

		StackSizeGuard<StrArr> guard( openFiles);

	    // append file to list
		openFiles.push_back(proFile);

		bool success =  GDLInterpreter::CompileFile( proFile); // this might trigger recursion
			
		if( success)
			Message("RESOLVE_ROUTINE: Compiled file: " + proFile);
		else
			e->Throw("Failed to compiled file: " + proFile);
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
    SizeT nParam=e->NParam(1); 
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
    ret = (BaseGDL***)malloc((nParam-1)*sizeof(BaseGDL**));
    GDLGuard<BaseGDL**,void,void> retGuard( ret, free);
    
    for (int i = nParam - 2; i >= 0; i--) if (global[i]) 
    {
      ret[i] = &e->GetPar(i + 1);
      // global parameter: undefined or different type/size -> creating
      if 
      (
        *ret[i] == NULL || 
        (*ret[i])->Type() != (i < 5 ? GDL_LONG : GDL_DOUBLE) || 
        (*ret[i])->N_Elements() != nEl 
      )
      {
        // not catching exceptions from SetPar as globality was ensured before
        if (i < 5) 
        {
          // handling repeated parameters case
          if (nParam == 7 && *ret[i] == *ret[5]) global[i] = false;
          else e->SetPar(i + 1, new DLongGDL(p0->Dim()));
        }
        else e->SetPar(i + 1, new DDoubleGDL(p0->Dim()));
      } 
      // global parameter that has correct size but different shape -> reforming
      else if ((*ret[i])->Rank() != p0->Rank()) (*ret[i])->SetDim(p0->Dim());
    }

    int A, a, B, C, D, E, hours, minutes, months;
    double JD, F, Z;
    // loop over input elements
    for (SizeT i = 0; i < nEl; i++)
    {
      JD = (*p0)[i] + 0.5;
      Z = floor(JD);
      F = JD - Z;

      if (Z < 2299161) A = (int) Z;
      else {
        a = (int) ((Z - 1867216.25) / 36524.25);
        A = (int) (Z + 1 + a - (int)(a / 4));
      }

      B = A + 1524;
      C = (int) ((B - 122.1) / 365.25);
      D = (int) (365.25 * C);
      E = (int) ((B - D) / 30.6001);

      // months
      months = E < 14 ? E - 1 : E - 13;
      if (global[1 - 1]) 
        (*static_cast<DLongGDL*>(*ret[1 - 1]))[i] = months;

      // days
      if (global[2 - 1]) 
        (*static_cast<DLongGDL*>(*ret[2 - 1]))[i] = B - D - (int)(30.6001 * E);

      // years
      if (global[3 - 1])
      {
        (*static_cast<DLongGDL*>(*ret[3 - 1]))[i] = months > 2 ? C - 4716 : C - 4715;
        if ((*static_cast<DLongGDL*>(*ret[3 - 1]))[i] < 0) 
          (*static_cast<DLongGDL*>(*ret[3 - 1]))[i] -= 1;
      }

      if (!(global[4 - 1] || global[5 - 1] || global[6 - 1])) continue;

      // hours
      hours = (int) (F * 24);
      F -= (double)hours / 24;
      if (global[4 - 1]) 
        (*static_cast<DLongGDL*>(*ret[4 - 1]))[i] = hours;

      // minutes
      minutes = (int) (F * 1440);
      F -= (double)minutes / 1440;
      if (global[5 - 1]) 
        (*static_cast<DLongGDL*>(*ret[5 - 1]))[i] = minutes;

      // seconds
      if (global[6 - 1]) 
        (*static_cast<DDoubleGDL*>(*ret[6 - 1]))[i] = F * 86400;
    }
    // now guarded. s. a.
//     free((void *)ret);
  }

  bool dateToJD(DDouble &jd, DLong &day, DLong &month, DLong &year, DLong &hour, DLong &minute, DDouble &second)
  {   
    if (year < -4716 || year > 5000000 || year==0 ) return false;
    if (month < 1 || month > 12) return false;
    if (day < 0 || day > 31) return false;

    // the following tests seem to be NOT active ...

    // if (hour < 0 || hour > 24) return false;
    // if (minute < 0 || minute > 60) return false;
    // if (second < 0 || second > 60) return false;

//    fprintf(stderr,"Day %d, Month %d Year %d, Hour %d Minute %d Second %f\n",
//            day, month, year, hour, minute, second);
    DDouble a,y,b,c;
    DLong m;
    y=(year>0)?year:year+1; //formula below is for *astronomical calendar* where year 0 exists.
    // but it appears that we use here a calendar with no year 0
    m=month;
    b=0.0;
    c=0.0;
    if (month <= 2)
    {
      y=y-1.0;
      m=m+12;
    }
    if (y < 0)
    {
      c=-0.75;
    } else {
       if (year > 1582  ||  (year == 1582 &&  (month > 10  ||
               (month == 10 && day > 14)))) {
          a=floor(y/100.0);
          b=2.0-a+floor(a/4.0);
       } else if (year == 1582 && month == 10 && day >= 5 && day <= 14) {
          jd= 2299161; //date does not move 
          return true;
       }
    }
    jd=ceil(365.25*y+c)+floor(30.6001*(m+1))+day+(hour*1.0)/24.0+(minute*1.0)/1440.0+
    (second*1.0)/86400.0+1720994.50+b;

    //    cout << "jd :" << jd << endl;
    return true;
  }
  
  BaseGDL* julday(EnvT* e)
  {
    if ((e->NParam() < 3 || e->NParam() > 6)) {e->Throw("Incorrect number of arguments.");}

    DLongGDL *Month, *Day, *Year, *Hour, *Minute;
    DDoubleGDL* Second;
    DDouble jd;
    DLong h=12;
    DLong m=0;
    DDouble s=0.0;
    SizeT nM,nD,nY,nH,nMi,nS,finalN=1,minsizePar;
    dimension finalDim;
    //behaviour: minimum set of dimensions of arrays. singletons expanded to dimension,
    //keep array trace.
    SizeT nEl,maxEl=1,minEl;
    for (int i=0; i<e->NParam() ; ++i) {
      nEl = e->GetPar(i)->N_Elements() ;
      if (nEl > 1 && nEl > maxEl) {
        maxEl=nEl;
        finalN = maxEl;
        finalDim = e->GetPar(i)->Dim();
      }
    } //first max - but we need first min:
    minEl=maxEl;
    for (int i=0; i<e->NParam() ; ++i) {
      nEl = e->GetPar(i)->N_Elements() ;
      if ( (nEl > 1) && (nEl < minEl)) {
        minEl=nEl; 
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

    if (e->NParam() == 3 ) {
      DLongGDL *ret = new DLongGDL(finalDim, BaseGDL::NOZERO);
      for (SizeT i=0; i< finalN; ++i) {
        if (dateToJD(jd,(*Day)[i%nD],(*Month)[i%nM],(*Year)[i%nY],h,m,s)) { (*ret)[i]=(long)jd;}
	else e->Throw("Invalid Calendar Date input.");
      }
      return ret;
    }
    
    DDoubleGDL *ret = new DDoubleGDL(finalDim, BaseGDL::NOZERO);
    
    if (e->NParam() >= 4) {
      Hour = e->GetParAs<DLongGDL>(3);
      nH = Hour->N_Elements();
    }
    if (e->NParam() == 4) {
      for (SizeT i=0; i< finalN; ++i) {
        if (dateToJD(jd,(*Day)[i%nD],(*Month)[i%nM],(*Year)[i%nY],(*Hour)[i%nH], m, s)) {(*ret)[i]=jd;}
	else e->Throw("Invalid Calendar Date input.");	
	return ret;
      }
    }

    if (e->NParam() >= 5) {
      Minute = e->GetParAs<DLongGDL>(4);
      nMi = Minute->N_Elements();
    }
    if (e->NParam() == 5) {
      for (SizeT i=0; i< finalN; ++i) {
        if (dateToJD(jd,(*Day)[i%nD],(*Month)[i%nM],(*Year)[i%nY],(*Hour)[i%nH], (*Minute)[i%nMi], s)) (*ret)[i]=jd;
	else e->Throw("Invalid Calendar Date input.");
	return ret;
      }
    }
    
    if (e->NParam() == 6) {
      Second = e->GetParAs<DDoubleGDL>(5);
      nS = Second->N_Elements();
      for (SizeT i=0; i< finalN; ++i) {
        if (dateToJD(jd,(*Day)[i%nD],(*Month)[i%nM],(*Year)[i%nY],(*Hour)[i%nH],(*Minute)[i%nMi],(*Second)[i%nS])) {(*ret)[i]=jd;}
        else e->Throw("Invalid Calendar Date input.");
      }
      return ret;
    }
  }
} // namespace
