/***************************************************************************
                          gdlhelp.cpp  - GDL help procedure
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
         
    - Numerous enhancements by Alain Coulais
    - May 2015 code cleanup by Greg Jung: 
    * gdlhelp.cpp, gdlhelp.hpp for HELP and DELVAR
    * OUTPUT= and NAMES= keywords implemented for help.
    * /COMMON to list all common blocks anywhere
    * Undefined variables normally not listed.
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

#ifndef _WIN32
//#include <regex.h> // stregex
#include <fnmatch.h>

#else
#include <shlwapi.h>
#endif

#ifndef _WIN32
#include <termios.h> 
#include <unistd.h> 
#endif

// used to defined GDL_TMPDIR: may have trouble on MSwin, help welcome
#ifndef _WIN32
#include <paths.h>
#endif


#ifndef _MSC_VER
#	include <dirent.h>
#else
// MSC workaround implementation in file.cpp
/*
  Declaration of POSIX directory browsing functions and types for Win32.

  Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
  History: Created March 1997. Updated June 2003.
  Rights:  See end of file.
*/
extern "C" {
  
  typedef struct DIR DIR;
  
  struct dirent {
    char *d_name;
  };
  
  DIR           *opendir(const char *);
  int           closedir(DIR *);
  struct dirent *readdir(DIR *);
  void          rewinddir(DIR *);
  
  /*
    Copyright Kevlin Henney, 1997, 2003. All rights reserved.
    
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.
    
    This software is supplied "as is" without express or implied warranty.
    
    But that said, if there are any problems please get in touch.
  */
  
} // extern "C"
#endif

#if 0
#ifdef __APPLE__
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
#define F_OK    0       /* Test for existence.  */
#else
#include <unistd.h>
#endif
#endif
#endif


#include "dinterpreter.hpp"
#include "graphicsdevice.hpp"
#include "dcommon.hpp"
#include "dpro.hpp"

#include "gdlhelp.hpp"
#include "terminfo.hpp"
#ifndef _WIN32
#include "shm_utils.hpp"
#endif

static std::vector<DStringGDL*> dlmInfo;            // DStringGDL* info to be used by help,/DLM, populated if the DLL is defined by a DLM related command (not directly linkimage)

// for sorting compiled pro/fun lists by name
struct CompFunName: public std::function<bool(DFun*, DFun*)>
{
  bool operator() ( DFun* f1, DFun* f2) const
	{
    return f1->ObjectName() < f2->ObjectName();
  }
};

struct CompProName: public std::function<bool(DPro*, DPro*)>
{
  bool operator() ( DPro* f1, DPro* f2) const
	{
    return f1->ObjectName() < f2->ObjectName();
  }
};

static bool CompareWithJokers(string theString, string thePattern) {

#ifdef _WIN32
  WCHAR wnames[MAX_PATH];
  WCHAR wsourceFiles[MAX_PATH];

  const char* cnames = theString.c_str();
  const char* csourceFiles = thePattern.c_str();

  MultiByteToWideChar(CP_UTF8, 0, cnames, -1,
    (LPWSTR) wnames, MAX_PATH);
  MultiByteToWideChar(CP_UTF8, 0, csourceFiles, -1,
    (LPWSTR) wsourceFiles, MAX_PATH);
  int match = 1 - PathMatchSpecW(wsourceFiles, wnames);
#else
  int flags=0;
#ifdef __GNUC__
  flags=FNM_CASEFOLD;
#endif
  int match = fnmatch(theString.c_str(), thePattern.c_str(), flags );
#endif
  if ( match == 0) 	return true;
  else 		return false;
  }
extern GDLFileListT  fileUnits;

    using namespace std;
// (static = internal) helper routines serving the lib:: routines called out in 
// gdlhelper.hpp


static void help_files(ostream& os, EnvT* e) {
  // AC 2020-04-28
  // see https://github.com/gnudatalanguage/gdl/issues/743
  // pb1 : why maxUserLun ?? pb2 : no return for -1, 0, 1
  cout << " maxUserLun : " << maxUserLun << " fileUnits.size() : " << fileUnits.size() << '\n';

  for( DLong lun=maxUserLun+1; lun <= fileUnits.size(); ++lun)
    if( fileUnits[ lun-1].InUse() || fileUnits[ lun-1].GetGetLunLock())
      {
	os << "	 lun "<< lun << ": "+fileUnits[lun-1].Name() << '\n';
      }
  return;
}

// AC 2020-04-28 derivated work from get_kbrd().
// Not sure what happen without ReadLine
char my_get_kbrd()
{
#if defined(HAVE_LIBREADLINE)
#include <readline/readline.h>
  rl_prep_terminal (0);
#endif

  char c='\0'; //initialize is never a bad idea...
  
  int fd=fileno(stdin);
#ifndef _WIN32
  struct termios orig, get; 
#endif
  // Get terminal setup to revert to it at end. 
#ifndef _WIN32
  (void)tcgetattr(fd, &orig); 
  // New terminal setup, non-canonical.
  get.c_lflag = ISIG; 
#endif
    // will wait for a character
#ifndef _WIN32
    get.c_cc[VTIME]=0;
    get.c_cc[VMIN]=1;
    (void)tcsetattr(fd, TCSANOW, &get); 
#endif
    cin.get(c);

    // Restore original terminal settings. 
#ifndef _WIN32
    (void)tcsetattr(fd, TCSANOW, &orig); 
#endif
#if defined(HAVE_LIBREADLINE)
    rl_deprep_terminal ();
#endif
    return c; 
  }

static void help_keys(ostream& ostr)
  { 
	ostr << "GDL is using Readline to manage keys shortcuts, few useful listed below." << '\n';
	ostr << "A summary can be read here : http://www.bigsmoke.us/readline/shortcuts " << '\n';
	ostr << '\n';
	ostr << "Moving in the command line :"<< '\n';
	ostr << "  Ctrl-a          : going to the beginning of the line"<< '\n';
	ostr << "  Ctrl-e          : going to the end of the line"<< '\n';
	ostr << "  Ctrl-u          : removing from here to the beginning of the line"<< '\n';
	ostr << "  Ctrl-k          : removing from here to the end of the line"<< '\n';
	ostr << "  Ctrl-RightArrow : jumping one word on the right"<< '\n';
	ostr << "  Ctrl-LeftArrow  : jumping one word on the left"<< '\n';
	ostr << '\n';
	ostr << "Moving in the history :"<< '\n';
	ostr << "  HELP, /recall       : listing the whole history" << '\n';
	ostr << "  Ctrl-p or UpArrow   : previous entry in history"  << '\n';
	ostr << "  Ctrl-n or DownArrow : next entry in history"  << '\n';
	ostr << '\n';    
	return;
  }

static void help_info()
  { 

	  cout << "* Homepage: https://gnudatalanguage.github.io/" << '\n';
	  cout << '\n';
	  cout << "* #NameOfRoutine for list of params and keywords" 
	    " for a given NameOfRoutine (internal or compiled pro/func)" << '\n';
	  cout << '\n';
	  cout << "* ?NameOfRoutine for starting a browser to access online doc"
	    " for a given routine (if exists ! internal or compiled pro/func)" ;
	  cout << '\n'; cout << '\n';
	  cout << "* HELP, /INTERNAL_LIB_GDL for a list of all internal library "
	    "functions/procedures." << '\n';
	  cout << "* HELP, /LIB Additional subroutines are written in GDL language, "
	    "look for *.pro files (e.g. in CVS in src/pro/)." << '\n';
	  cout << '\n';
	  cout << "* HELP, /KEYS for useful CLI keys shortcuts." << '\n';
	  cout << '\n';
		return;
}
static void help_sysvar(ostream& os , bool briefKW=true)
  {
  std::map<std::string, DVar*>list;
  for (SizeT v = 0; v < sysVarList.size(); ++v) list.insert(std::pair<std::string, DVar*>(" !" +sysVarList[ v]->Name(), sysVarList[ v]));
	  if (briefKW) {
    for (std::map<std::string, DVar*>::iterator it=list.begin(); it!=list.end(); ++it) { os << it->first << '\n';}
  } else {
    for (std::map<std::string, DVar*>::iterator it=list.begin(); it!=list.end(); ++it) { 
      DVar* v=it->second;
      BaseGDL* var= v->Data();
      lib::help_item(os, var , it->first, false);
		  }
		}
	return;
  }
static void help_dlm(ostream &os, bool briefKW=false) {
  std::vector<DStringGDL*>::iterator it;
  if (briefKW) {
    for (it = dlmInfo.begin(); it != dlmInfo.end(); ++it) {
    if ( (*it)->N_Elements() != 4) continue; 
    os << (*(*it))[0] <<" ";
    } 
    os << "\n";
  } else {
    for (it = dlmInfo.begin(); it != dlmInfo.end(); ++it) {
      if ( (*it)->N_Elements() != 4) continue; 
      os << "** "<<(*(*it))[0]<<" - "<<(*(*it))[1]<<"\n";
      os << "   "<<(*(*it))[2]<<"\n";
      os << "   "<<(*(*it))[3]<<"\n";
   }
  }
}
static void help_Output(BaseGDL** outputKW, ostringstream& ostr, SizeT &nlines, bool doOutput=true)
  {
	// Setup output return variable ostream& os, int &lines_count

	std::string s = ostr.rdbuf()->str().erase(ostr.rdbuf()->str().length(),1);
	char delimiter = '\n';
	SizeT nOut = 0;
	size_t found=0;
	if(doOutput) {

	  do {
		++nOut;
		found=s.find( delimiter,found);
	     }   while( (found++ != std::string::npos) );

	  nlines = --nOut;	if(nlines == 0) return;
	  if (*outputKW!=NULL) GDLDelete((*outputKW));
	  dimension dim(&nlines, (size_t) 1);
	  *outputKW = new DStringGDL(dim, BaseGDL::NOZERO);
	}
	size_t pos = 0;
	nOut = 0;
	std::string token;
	while ((found = s.find(delimiter,pos)) != std::string::npos) {
		token = s.substr(pos, found-pos);
		if( doOutput and (nOut not_eq nlines)) (*(DStringGDL *) *outputKW)[nOut] = token;
		else cout << token << '\n';
		++nOut;
		pos = found+1;
		}
	    ostr.str("");
//	    if( nOut  not_eq nlines and debug) cout << 
//			" help_Output: Error counting lines -" <<
//			" nOut: "<<nOut<<" OutputLines:"<<nlines<<'\n';
	}

    // showing HELP, /path_cache
  void help_path_cached(ostream& ostr, SizeT &lines_count) {

    struct dirent *dp;
    const char *ProSuffix = ".pro";
    int ProSuffixLen = strlen(ProSuffix);

    string tmp_fname;
    size_t found;

    StrArr path = SysVar::GDLPath();

    std::sort(path.begin(),path.end());
    ostr << "!PATH Cache (Enabled, "<< path.size() <<" directories)" << '\n';
    lines_count = 1;

    for (StrArr::iterator CurrentDir = path.begin(); CurrentDir != path.end(); ++CurrentDir) {
      //	  cout << "1>>" << (*CurrentDir).c_str() << "<<" <<'\n';
      DIR* dirp = opendir((*CurrentDir).c_str());
      //cout << "2>>" << dirp << "<<" <<'\n';
      if (dirp != NULL) {
		int NbProFilesInCurrentDir = 0;
        while ((dp = readdir(dirp)) != NULL) {
          tmp_fname = dp->d_name;
          found = tmp_fname.rfind(ProSuffix);
          if (found != std::string::npos) {
            if ((found + ProSuffixLen) == tmp_fname.length())
              ++NbProFilesInCurrentDir;
          }
        }
        closedir(dirp);
        ++lines_count;
        ostr << *CurrentDir << " (" << NbProFilesInCurrentDir << " files)" << '\n';
      }
    }
  }

  // a simplification of codes (DebugMsg & DumpStack) in GDLInterpreter.cpp
  // ProgNodeP cN = e->CallingNode();
  // DInterpreter::DebugMsg(cN, "At ");
  // DInterpreter::DumpStack(3);

  static void SimpleDumpStack(EnvT* e, ostream& ost=std::cerr) {

    EnvStackT& callStack = e->Interpreter()->CallStack();

    // simple way to manage the first line : exception
    SizeT w = 0;
    string msgPrefix = "% At ";

    long actIx = callStack.size() - 1;
    for (; actIx >= 0; --actIx) {
      EnvStackT::pointer_type upEnv = callStack[actIx];

      ost << msgPrefix << std::right << std::setw(w) << "";
      // simple way to manage the first line : exception
      msgPrefix = "";
      w = 5;
      ost << std::left << std::setw(16) << upEnv->GetProName();

      std::string file = upEnv->GetFilename();
      if (file != "") {
        int lineNumber = upEnv->GetLineNumber();
        if (lineNumber != 0) {
          ost << std::right << std::setw(6) << lineNumber;
        } else {
          ost << std::right << std::setw(6) << "";
        }
        ost << std::left << " " << file;
      }
      ost << '\n';
    }
	  return;
  }
static void help_object(std::ostream* ostrp, DStructDesc* objDesc, bool verbose = false)
{
  FunListT& funlist = objDesc->FunList();
  ProListT& prolist = objDesc->ProList();
  //if object is purely GDL internal , do not display it if not verbose:
  if (!verbose) {
  bool internal=true;
	for (auto i=0; i< funlist.size(); ++i) if (funlist[i]->GetSCC()!=NULL) {internal=false; break;}
	if (internal)  for (auto i=0; i< prolist.size(); ++i) if (prolist[i]->GetSCC()!=NULL) {internal=false; break;}
	if (internal) return;
  }
  int num_methods = funlist.size() + prolist.size();
  int numpar = objDesc->GetNumberOfParents();
  if (numpar==1) *ostrp << "** Object class " << objDesc->Name() << ", " << numpar << " direct superclass, " << num_methods << " known methods" << '\n';
  else *ostrp << "** Object class " << objDesc->Name() << ", " << numpar << " direct superclasses, " << num_methods << " known methods" << '\n';
  if (numpar > 0) {
    *ostrp << "   Superclasses:\n";
    std::set< std::string> pNames;
    objDesc->GetParentNames(pNames);
    for (std::set<string>::iterator j = pNames.begin(); j != pNames.end(); ++j) *ostrp << "      " << (*j) << "  <Direct>\n";
    //find all ancestors
    pNames.clear();
    objDesc->GetAncestorsNames(pNames);
    for (std::set<string>::iterator j = pNames.begin(); j != pNames.end(); ++j) *ostrp << "      " << (*j) << "\n";
		}
  if (num_methods > 0) {
    if (funlist.size() > 0) {
      *ostrp << "   Known Function Methods:\n";
      for (int j = 0; j < funlist.size(); ++j) *ostrp << "      " << objDesc->Name() << "::" << funlist[j]->Name() << "\n";
		}
    if (prolist.size() > 0) {
      *ostrp << "   Known Procedure Methods:\n";
      for (int j = 0; j < prolist.size(); ++j) *ostrp << "      " << objDesc->Name() << "::" << prolist[j]->Name() << "\n";
}
//
//    if (!verbose) return;
//    DStructGDL* dumm = new DStructGDL(objDesc, dimension());
//      Guard<DStructGDL> guard(dumm);
//    lib::help_struct(*ostrp, dumm, 0, false);
    }
	}

static void help_ListLib(const DString &names, ostream& ostr, bool internal=true)
  {
	bool searchbyname;
	  searchbyname = (names != "");
	  vector<DString> subList;
//	  for( libProListT::iterator i=libProList.begin(); i != libProList.end(); i++)
	  for( SizeT i = 0; i<libProList.size(); ++i)  {
	      if( internal == libProList[ i]->GetHideHelp()) {
	         if(searchbyname and not 
				CompareWithJokers(names, libProList[i]->Name())) continue;
		      subList.push_back(libProList[ i]->ToString());
			}
	    }
	  sort( subList.begin(), subList.end());
	  if(internal) ostr << "Internal l";
	  else ostr << "L";
	  ostr << "ibrary procedures (" 
				<< subList.size() <<"):" << '\n';
	  for( SizeT i = 0; i<subList.size(); ++i)
			ostr << subList[ i] << '\n';

	  subList.clear();

//	  for( libFunListT::iterator i=libFunList.begin(); i != libFunList.end(); i++)
	  for( SizeT i = 0; i<libFunList.size(); ++i)
	    {
	      if(  internal == libFunList[ i]->GetHideHelp()) {
	         if(searchbyname and not 
				CompareWithJokers(names, libFunList[i]->Name())) continue;
			  subList.push_back(libFunList[ i]->ToString());
			}
	    }
	  sort( subList.begin(), subList.end());

	  if(internal) ostr << "Internal l";
	  else ostr << "L";
	  ostr << "ibrary functions (" 
				<< subList.size() <<"):" << '\n';
	  for( SizeT i = 0; i<subList.size(); ++i)
	    ostr << subList[ i] << '\n';
	  subList.clear();

}

static void help_heap_obj_ptr_head(EnvT* e, ostream& ostr)
{
  SizeT numPtr = e->Interpreter()->HeapSize();
  SizeT numObj = e->Interpreter()->ObjHeapSize();
  ostr << "Heap Variables:" << '\n';
  ostr << "    # Pointer: " << numPtr << '\n';
  ostr << "    # Object : " << numObj << '\n'<<'\n';
}
      
      
static void help_mix_heap_and_obj(EnvT* e, ostream& ostr)
{
  std::vector<DObj>* objheap = e->Interpreter()->GetAllObjHeapSTL();
  Guard< std::vector<DObj> > heap_objguard(objheap);
  SizeT nobjH = objheap->size();
  std::vector<DPtr>* heap = e->Interpreter()->GetAllHeapSTL();
  Guard< std::vector<DPtr> > heap_guard(heap);
  SizeT nH = heap->size();
  SizeT tot=nH+nobjH;
  if (tot <= 0) return;
  // objHeap and heap contain different, globally increasing, integers.
  // to show them in incresing order like in IDL one needs to put them in an ordered fashion:
  std::set<DPtr> myHeapIndex;
  SizeT k=0;
  for (SizeT i=0; i<nobjH; ++i) myHeapIndex.insert((*objheap)[i]);
  for (SizeT i=0; i<nH;    ++i) myHeapIndex.insert((*heap)[i]);
  
  std::set<DPtr>::iterator it;
  for (it = myHeapIndex.begin(); it!=myHeapIndex.end(); ++it) {
    DPtr h=(*it); 
    if (e->Interpreter()->ObjValid(h))
    {
      BaseGDL* hV = BaseGDL::interpreter->GetObjHeap(h);
      SizeT refc = BaseGDL::interpreter->RefCountHeapObj(h);
      lib::help_item(ostr, hV, DString("<ObjHeapVar") + i2s(h) + ">  refcount=" + i2s(refc), false);
    } else {
      if (e->Interpreter()->PtrValid(h))
      {
        BaseGDL* hV = BaseGDL::interpreter->GetHeap(h);
        SizeT refc = BaseGDL::interpreter->RefCountHeap(h);
        lib::help_item(ostr, hV, DString("<PtrHeapVar") + i2s(h) + ">  refcount=" + i2s(refc), false);
      }
    }
  }
  return;
}


static void help_lastmsg(EnvT* e)
  {
      // if LAST_MESSAGE is present, it is the only output.
      // All other kw are ignored *EXCEPT 'output'*.
    BaseGDL** outputKW = NULL;
    
    DStructGDL* errorState = SysVar::Error_State();
    static unsigned msgTag = errorState->Desc()->TagIndex( "MSG");
    
    static int outputIx = e->KeywordIx( "OUTPUT");
    if (e->WriteableKeywordPresent( outputIx)) 
      {    // Setup output return variable
	outputKW = &e->GetTheKW( outputIx);
	GDLDelete((*outputKW));
	*outputKW = static_cast<DStringGDL*>((errorState->GetTag( msgTag))
					     ->Convert2( GDL_STRING, BaseGDL::COPY));
	return;
      }
    else {
      cout << (*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0]<< '\n';
      return;
    }
  }
    
  static DStringGDL* recall_commands_internal() {

#if defined(HAVE_LIBREADLINE)
  if (iAmMaster) {
	// http://cnswww.cns.cwru.edu/php/chet/readline/history.html#IDX14
	HIST_ENTRY **the_list;
	//    cout << "history_length" << history_length << '\n';
	the_list = history_list();

	if (the_list) {
	  DStringGDL* retVal = new DStringGDL(history_length, BaseGDL::NOZERO);
	  for (SizeT i = 0; i < history_length; ++i)
		(*retVal)[history_length - i - 1] = the_list[i]->line;
	  return retVal;
	}  else return new DStringGDL("");
  } else return new DStringGDL(""); //pacify compiler
#else
  Message("RECALL_COMMANDS: nothing done, because compiled without READLINE");
  return new DStringGDL("");
#endif
}

namespace lib {

  using namespace std;


BaseGDL* recall_commands( EnvT* e)
  {
      return recall_commands_internal();
  }


  // display help for one variable or one structure tag

  void help_item(ostream& ostr,
    BaseGDL* par, DString parString,
    bool doIndentation = false)
  {
    static volatile bool debug(false);
    if (debug and (par not_eq NULL)) {
      cout << par->Type() << " :"
        << par->TypeStr() << " :"
        //		<< &par->TypeStr() << ": "
        << parString << '\n';
    }

    if (doIndentation) ostr << "   ";

    // Name display
    ostr.width(16);
    ostr << left << parString;
    if (parString.length() >= 16) {
      ostr << '\n'; // for cmsv compatible output (uses help,OUTPUT)
      ostr.width(doIndentation ? 19 : 16);
      ostr << "";
    }

    // Type display (we have two "null" : defined !null and undefined variables ...
    if (par == NULL) {
      ostr << "UNDEFINED = <Undefined>" << '\n';
      return;
    }
    if (par == NullGDL::GetSingleInstance()) {
      ostr << "UNDEFINED = !NULL" << '\n';
      return;
    }
    ostr.width(10);
    bool doTypeString = true;

    // Data display
    if (par->Type() == GDL_STRUCT) {
      ostr << par->TypeStr() << right;
      if (!doIndentation) ostr << "= ";
      doTypeString = false;

      DStructGDL* s = static_cast<DStructGDL*> (par);
      ostr << "-> ";
      ostr << (s->Desc()->IsUnnamed() ? "<Anonymous>" : s->Desc()->Name());
      ostr << " ";
    } else if (par->Dim(0) == 0) {
      if (par->Type() == GDL_STRING) {
        ostr << par->TypeStr() << right;
        if (!doIndentation) ostr << "= ";
        doTypeString = false;

        // trim string larger than $COLUMNS- characters
        DString dataString = (*static_cast<DStringGDL*> (par))[0];
        int ncols=max(39,TermWidth()-12-29); //29 as this is the position where the string is writte, 11 for '... plus blank on rght
        ostr << "'" << StrMid(dataString, 0,ncols, 0) << "'";
        if (dataString.length() > ncols) ostr << "...";
      } else if (par->Type() == GDL_OBJ && par->StrictScalar()) {
        DObj s = (*static_cast<DObjGDL*> (par))[0]; // is StrictScalar()
        if (s != 0) // no overloads for null object
        {
          DStructGDL* oStructGDL = GDLInterpreter::GetObjHeapNoThrow(s);
          if (oStructGDL != NULL) // if object not valid -> default behaviour
          {
            DStructDesc* desc = oStructGDL->Desc();
            static DString listName("LIST");
            if (desc->IsParent(listName)) {
              ostr << desc->Name();

              unsigned nListTag = desc->TagIndex("NLIST");
              DLong nList = (*static_cast<DLongGDL*> (oStructGDL->GetTag(nListTag, 0)))[0];
              ostr << left;
              ostr << "<ID=";
              ostr << i2s(s) << "  N_ELEMENTS=" << i2s(nList) << ">";

              doTypeString = false;
            }
            static DString hashName("HASH");
            if (desc->IsParent(hashName)) {
              ostr << desc->Name();

              unsigned nListTag = desc->TagIndex("TABLE_COUNT");
              DLong nList = (*static_cast<DLongGDL*> (oStructGDL->GetTag(nListTag, 0)))[0];
              ostr << left;
              ostr << "<ID=";
              ostr << i2s(s) << "  N_ELEMENTS=" << i2s(nList) << ">";

              doTypeString = false;
            }
          }
        }
      }
      if (doTypeString) {
        ostr << par->TypeStr() << right;
        if (!doIndentation) ostr << "= ";
        doTypeString = false;

        par->ToStream(ostr);
      }
    }

    if (doTypeString) {
      ostr << par->TypeStr() << right;
      if (!doIndentation) ostr << "= ";
      if (par->IsAssoc())
        par->ToStream(ostr);
#ifndef _WIN32
	  else if (par->IsShared()) help_par_shared(par, ostr);
#endif
    }

    // Dimension display
    if (par->Dim(0) != 0) ostr << par->Dim();

    // End of line
    ostr << '\n';
  }

  void help_struct(ostream& ostr, BaseGDL* par, int indent = 0, bool debug = false)
  {
    // STRUCTURES
    DStructGDL* s = static_cast<DStructGDL*> (par);
    SizeT nTags = s->Desc()->NTags();

    for (int i = 0; i < indent; ++i) ostr << "   ";
    ostr << "** Structure ";
    ostr << (s->Desc()->IsUnnamed() ? "<Anonymous>" : s->Desc()->Name());
    ostr << ", " << nTags << " tags";
    if (indent == 0) {
      ostr << ",memsize =" << s->Sizeof();
      ostr << ", data length=" << s->NBytesToTransfer()
        //			<< "/" << s->RealBytes() ; GJ has this but only applied here.
        << "/" << s->SizeofTags();
    }
    ostr << ":" << '\n';

    for (SizeT t = 0; t < nTags; ++t) {
      for (int i = 0; i < indent; ++i) ostr << "   ";
      if (debug) ostr.width(18);
      if (debug) ostr << "dbg: OFFSET=" << s->Desc()->Offset(t);
      help_item(ostr, s->GetTag(t), s->Desc()->TagName(t), true);
      // only one level visible in "help".
      //		    if(s->GetTag(t)->Type() == GDL_STRUCT) help_struct(ostr, s->GetTag(t), indent+1);
    }
    //		lines_count += nTags;
  }
#if 0
void help_struct(ostream& ostr,  DStructDesc* dsc)
  {

	   SizeT nTags = dsc->NTags();
		ostr << "** Structure ";
		ostr << (dsc->IsUnnamed() ? "<Anonymous>" : dsc->Name());
		ostr << ", " << nTags << " tags";
		ostr << ",length =" << dsc->NBytes();
		for (SizeT t=0; t < nTags; ++t) {
		    help_item( ostr, dsc->GetTag(t), dsc->TagName(t), true);
		    if(dsc->GetTag(t)->Type() == GDL_STRUCT) 
				help_struct(ostr, dsc->GetTag(t)->Desc());
			}
		ostr << ":" << '\n';
}
#endif

void help_help(EnvT* e)
  {
	string inline_help[]={"Usage: "+e->GetProName()+", expr1, ..., exprN,", 
			      "          /ALL_KEYS, /BRIEF, /CALLS, /FUNCTIONS, /HELP, /INFO,",
			      "          /INTERNAL_LIB_GDL, /KEYS, /LAST_MESSAGE, /LIB, /MEMORY,",
			      "          NAMES=string_filter, OUTPUT=res, /PATH_CACHE, /FILES, ",
			      "          /PREFERENCES, /PROCEDURES, /RECALL_COMMANDS, /ROUTINES,",
			      "          /SOURCE_FILES, /STRUCTURES, /SYSTEM_VARIABLES, /TRACEBACK"};
	int size_of_s = sizeof(inline_help) / sizeof(inline_help[0]);
	e->Help(inline_help, size_of_s);
	return;
  }

  void SortAndPrintStream(ostringstream& oss) {
    std::string delimiter = "\n";
    std::string s = oss.rdbuf()->str().erase(oss.rdbuf()->str().length(), 1);
    size_t pos = 0;
    vector<std::string> stringList;

    while ((pos = s.find(delimiter)) != std::string::npos) {
      stringList.push_back(s.substr(0, pos));
      s.erase(0, pos + delimiter.length());
    }
    oss.str("");
    sort(stringList.begin(), stringList.end());
    vector<std::string>::iterator it = stringList.begin();
	while (it != stringList.end()) std::cout << *it++;
    std::cout << '\n';
}
  
  DStringGDL* StreamToGDLString(ostringstream& oss, bool sorted=false) {

    std::string delimiter = "\n";
    int nlines = 0;
    size_t pos = 0;
    while ((pos = oss.str().find(delimiter, pos + 1)) != std::string::npos) {
      ++nlines;
    }
    if (!nlines) return new DStringGDL("");

    dimension dim(nlines, (size_t) 1);
    DStringGDL* out = new DStringGDL(dim, BaseGDL::NOZERO);

    std::string s = oss.rdbuf()->str().erase(oss.rdbuf()->str().length(), 1);
    pos = 0;
    vector<std::string> stringList;
    SizeT nOut = 0;

    while ((pos = s.find(delimiter)) != std::string::npos) {
      stringList.push_back(s.substr(0, pos));
      s.erase(0, pos + delimiter.length());
    }
    oss.str("");
    
    if (sorted) sort(stringList.begin(), stringList.end());
    vector<std::string>::iterator it = stringList.begin();
	while (it != stringList.end()) (*out)[nOut++] = *it++;

    return out;
  }

  void help_pro(EnvT* e) {
    //unsupported: just return
    //BREAKPOINTS","MESSAGES", "LAMBDA",
	static int BREAKPOINTS = e->KeywordIx("BREAKPOINTS"); if (e->KeywordPresent(BREAKPOINTS)) { Message("Unsupported Keyword."); return;}

	static int MESSAGES = e->KeywordIx("MESSAGES"); if (e->KeywordPresent(MESSAGES)) { Message("Unsupported Keyword."); return;}
	static int LAMBDA = e->KeywordIx("LAMBDA"); if (e->KeywordPresent(LAMBDA)) { Message("Unsupported Keyword."); return;}

	// in order of priority
	bool kw = false;
	static int lastmKWIx = e->KeywordIx("LAST_MESSAGE");
	bool lastmKW = e->KeywordPresent(lastmKWIx);
	if (lastmKW) {
	  help_lastmsg(e);
	  return;
	}
	SizeT nParam = e->NParam();

	BaseGDL** outputKW = NULL;
	static int outputIx = e->KeywordIx("OUTPUT");
	bool doOutput = e->WriteableKeywordPresent(outputIx);

	if (doOutput) { // Setup output return variable
	  outputKW = &e->GetTheKW(outputIx);
	  GDLDelete((*outputKW));
	}
	static SizeT OutputLines;
	OutputLines = 0;

	std::ostringstream ostr;
	// Use mostly ostrp* << from here on and then push onto outputKW if need be.
	std::ostream* ostrp = (doOutput) ? &ostr : &cout;

	static int helpIx = e->KeywordIx("HELP");
	if (e->KeywordSet(helpIx)) {
	  help_help(e);
	  return;
    }

	static int namesIx = e->KeywordIx("NAMES");
	bool isKWSetNames = e->KeywordPresent(namesIx);
	DString names = "";
	if (isKWSetNames) {
	  e->AssureStringScalarKWIfPresent(namesIx, names);
	  // since routines and var. are stored in Maj, we convert ...
	  names = StrUpCase(names);
	}

	static int allkeysIx = e->KeywordIx("ALL_KEYS");
	static int keysIx = e->KeywordIx("KEYS");
	if (e->KeywordSet(allkeysIx) || e->KeywordSet(keysIx)) // ALL_KEYS is an obsolete keyword
	{
	  help_keys(ostr);
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr);
	  else cout << ostr.str();
	  return;
	}

	static volatile bool debugKW(false);
	static int debugIx = e->KeywordIx("DEBUG");
	if (e->KeywordSet(debugIx)) {
	  debugKW = !debugKW;
	  cout << " Help debug option set/reset: " << debugKW << '\n';
	  return;
	}

	static int pathIx = e->KeywordIx("PATH_CACHE");
	if (e->KeywordSet(pathIx)) { // exercising two methods
	  help_path_cached(ostr, OutputLines);
	  if (debugKW) {
		cout << OutputLines << '\n';
		cout << "begin" << ostr.rdbuf()->str() << "end" << '\n';
	  }
	  help_Output(outputKW, ostr, OutputLines, doOutput);
	  return;
	}
	// if keyword /TraceBack then we return
	static int tracebackKWIx = e->KeywordIx("TRACEBACK");
	bool tracebackKW = e->KeywordSet(tracebackKWIx);

	if (tracebackKW) {
	  SimpleDumpStack(e, ostr);
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr);
	  else cout << ostr.str();
	  return;
	}

	static int briefKWIx = e->KeywordIx("BRIEF");
	bool briefKW = e->KeywordSet(briefKWIx);
	// briefKw should be default usage with DLM HEAP_VARIABLES
	// also MESSAGES OBJECTS ROUTINES SOURCE_FILES STRUCTURES SYSTEM_VARIABLES
	static int fullKWIx = e->KeywordIx("FULL");
	bool fullKW = e->KeywordSet(fullKWIx);

    static int DLM = e->KeywordIx("DLM");
    bool wantsDlm = (e->KeywordPresent(DLM));
    if (wantsDlm) {
      help_dlm(ostr,briefKW);
      if (doOutput) (*outputKW) = StreamToGDLString(ostr);
      else cout << ostr.str();
      return;
    }

	// AC 14-08-11 : detailed info (display size, deep ...) are missing
	static int deviceKWIx = e->KeywordIx("DEVICE");
	bool deviceKW = e->KeywordPresent(deviceKWIx);
	if (deviceKW) {
	  GraphicsDevice::ListDevice(ostr);
	  DString name = (*static_cast<DStringGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("NAME"), 0)))[0];
	  ostr << "Current graphics device: " << name << '\n';
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  else cout << ostr.str();
	  return;
	}

	static int heapIx = e->KeywordIx("HEAP_VARIABLES");
	if (e->KeywordSet(heapIx)) {
	  help_heap_obj_ptr_head(e, *ostrp);
	  if (briefKW) return;
	  help_mix_heap_and_obj(e, *ostrp);
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  else SortAndPrintStream(ostr);
	  return;
	}
    static int sharedIx = e->KeywordIx("SHARED_MEMORY");
	if (e->KeywordSet(sharedIx)) {
#ifndef _WIN32
	  help_shared(e, *ostrp);
	  if (briefKW) return;
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  else SortAndPrintStream(ostr);
#endif	  
	  return;
	}
    
	static int sysvarIx = e->KeywordIx("SYSTEM_VARIABLES");
	if (e->KeywordSet(sysvarIx)) {
	  help_sysvar(*ostrp, briefKW);
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  else SortAndPrintStream(ostr);
	  return;
	}
	/*
	OBJECTS
	Set this keyword to display information on defined object classes.
	 *  If no arguments are provided, all currently-defined object classes are shown. 
	 *  If arguments are provided, the definition of the object class
	 *  for the heap variables referred to is displayed.
	
	 */
	static int objectsIx = e->KeywordIx("OBJECTS");
	if (e->KeywordSet(objectsIx)) {
	  if (nParam == 0) {
		//sort alphabetically object names...
		std::set< std::string> objNames;
		for (SizeT i = 0; i < structList.size(); ++i) {
		  if ((structList[i]->FunList().size() + structList[i]->ProList().size()) == 0) continue;
		  objNames.insert(structList[i]->Name());
		}
		SizeT nObj = objNames.size();
		if (nObj < 1) return;

		//these are objects that have at least one method.
		//sort alphabetically

		for (std::set<string>::iterator iobj = objNames.begin(); iobj != objNames.end(); ++iobj) {
		  DStructDesc* objDesc = FindObjectInStructList(structList, *iobj);
		  if (objDesc != NULL) help_object(ostrp, objDesc, fullKW);
		}
	  } else {
		for (SizeT i = 0; i < nParam; ++i) {
		  BaseGDL*& par = e->GetPar(i);
		  if (par != NULL && e->GetPar(i)->Type() == GDL_OBJ) {
			DObjGDL* myObj = static_cast<DObjGDL*> (e->GetParDefined(i));
			DStructDesc* objDesc = (BaseGDL::interpreter->GetObjHeap((*myObj)[0]))->Desc();
			if (objDesc != NULL) help_object(ostrp, objDesc, fullKW);
		  }
		}
	  }
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  return;
	}


	static int sourceFilesKWIx = e->KeywordIx("SOURCE_FILES");
	bool sourceFilesKW = e->KeywordPresent(sourceFilesKWIx);

	static int routinesKWIx = e->KeywordIx("ROUTINES");
	bool routinesKW = e->KeywordSet(routinesKWIx);

	static int ProceduresIx = e->KeywordIx("PROCEDURES");
	bool isKWSetProcedures = e->KeywordSet(ProceduresIx);

	static int FunctionsIx = e->KeywordIx("FUNCTIONS");
	bool isKWSetFunctions = e->KeywordSet(FunctionsIx);


	if (sourceFilesKW) {
	  if (!isKWSetFunctions) {
		// AC 2018-01-09 : Duplicating the pro list to avoid messing up the order of "proList"
		// otherwise, a call to HELP,/source created in future calls
		// e.g. of crashing sequence : TEST_TV & HELP, /source & TEST_TV
		//
		ProListT proList_tmp;
		proList_tmp = proList;
		sort(proList_tmp.begin(), proList_tmp.end(), CompProName());
		*ostrp << "Compiled Procedures:" << '\n';
		*ostrp << "$MAIN$\n";
		OutputLines += 2;
		for (SizeT i = 0; i < proList_tmp.size(); ++i) {
		  if (proList_tmp[i]->isHidden() and !fullKW) continue;
		  if (isKWSetNames and
			!(CompareWithJokers(names, proList_tmp[i]->ObjectName()))) continue;
		  *ostrp << setw(25) << left << proList_tmp[i]->ObjectName() << setw(0);
		  *ostrp << proList_tmp[i]->GetFilename() << '\n';
		}
	  }

	  if (!isKWSetProcedures) {
		if (!isKWSetFunctions) *ostrp << '\n';
		// AC 2018-01-09 : Duplicating the fun list to avoid messing up the order of "funList"
		// see above in (do_pro).
		FunListT funList_tmp;
		funList_tmp = funList;
		sort(funList_tmp.begin(), funList_tmp.end(), CompFunName());
		*ostrp << "Compiled Functions:" << '\n';
		++OutputLines;
		for (SizeT i = 0; i < funList_tmp.size(); ++i) {
		  if (funList_tmp[i]->isHidden() and !fullKW) continue;
		  if (isKWSetNames and
			!(CompareWithJokers(names, funList_tmp[i]->ObjectName()))) continue;
		  *ostrp << setw(25) << left << funList_tmp[i]->ObjectName() << setw(0);
		  *ostrp << funList_tmp[i]->GetFilename() << '\n';
		  ++OutputLines;
		}
	  }
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  else cout << ostr.str();
	  return;
	}

	// Compiled Procedures & Functions

	vector<DString> fList;
	static volatile int npro = 0, nfun = 0;
	for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i)
	  if (fullKW or !((*i)->isHidden())) {
		fList.push_back((*i)->ObjectName());
		++nfun;
	  }
	for (SizeT i = 0; i < structList.size(); ++i) {
	  DStructDesc* s = structList[i];
	  for (auto j = 0; j < s->FunList().size(); ++j) {
		DFun* fun = (s->FunList())[j];
		if (fun->GetSCC() != NULL) { //compiled objects, not native ones
		  if (fullKW or !fun->isHidden()) {
			fList.push_back(fun->ObjectName());
			++nfun;
		  }
		}
	  }
	}
	sort(fList.begin(), fList.end());
	if (debugKW) cout << " #functions=" << nfun;

	vector<DString> pList;
	pList.push_back("$MAIN$");
	for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i)
	  if (fullKW or !((*i)->isHidden())) {
		pList.push_back((*i)->ObjectName());
		++npro;
	  }
	for (SizeT i = 0; i < structList.size(); ++i) {
	  DStructDesc* s = structList[i];
	  for (auto j = 0; j < s->ProList().size(); ++j) {
		DPro * pro = (s->ProList())[j];
		if (pro->GetSCC() != NULL) { //compiled objects, not native ones
		  if (fullKW or !pro->isHidden()) {
			pList.push_back(pro->ObjectName());
			++npro;
		  }
		}
	  }
	}
	sort(pList.begin(), pList.end());
	if (debugKW) cout << " #procedures=" << npro;
	if (debugKW) cout << " #env:" << e->Caller()->EnvSize() << '\n';

	static int callsKWIx = e->KeywordIx("CALLS");
	bool callsKW = e->KeywordPresent(callsKWIx);

	if (callsKW) {

	  // this is a code derived from SimpleDumpStack() above
	  EnvStackT& callStack = e->Interpreter()->CallStack();
	  long actIx = callStack.size() - 1;
	  DStringGDL* retVal = new DStringGDL(dimension(actIx + 1), BaseGDL::NOZERO);
	  SizeT rIx = 0;
	  for (; actIx >= 0; --actIx) {
		EnvStackT::pointer_type upEnv = callStack[actIx];
		DString actString = upEnv->GetProName();
		std::string file = upEnv->GetFilename();
		if (file != "") {
		  actString += " <" + file + "(";
		  actString += i2s(upEnv->GetLineNumber(), 4);
		  actString += ")>";
		}
		(*retVal)[rIx++] = actString;
	  }
	  e->SetKW(callsKWIx, retVal);
	  return;
	}
	static int filesIx = e->KeywordIx("FILES");
	if (e->KeywordPresent(filesIx)) {
	  help_files(ostr, e);
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr, true);
	  else cout << ostr.str();
	  return;
	}

	static int infoIx = e->KeywordIx("INFO");
	if (e->KeywordSet(infoIx)) {
	  kw = true;
	  help_info();
	}

	static int libIx = e->KeywordIx("LIB");
	// internal library functions
	static int INTERNAL_LIB_GDLIx = e->KeywordIx("INTERNAL_LIB_GDL");
	bool kwLibInternal = e->KeywordSet(INTERNAL_LIB_GDLIx);
	if (kwLibInternal) {
	  cout << "NOTE: Internal subroutines are subject to change without notice." << '\n';
	  cout << "They should never be called directly from a GDL program." << '\n';
	}
	if (e->KeywordSet(libIx)) {
	  kw = true;
	  help_ListLib(names, *ostrp, kwLibInternal);
	}

	static int STRUCTURESIx = e->KeywordIx("STRUCTURES");
	bool isKWSetStructures = e->KeywordSet(STRUCTURESIx);
	static int RECALL_COMMANDSIx = e->KeywordIx("RECALL_COMMANDS");
	bool isKWSetRecall = e->KeywordSet(RECALL_COMMANDSIx);
	static int MEMORYIx = e->KeywordIx("MEMORY");
	bool isKWSetMemory = e->KeywordSet(MEMORYIx);
	static int PREFERENCESIx = e->KeywordIx("PREFERENCES");
	bool isKWSetPreferences = e->KeywordSet(PREFERENCESIx);
	if (isKWSetStructures) kw = true;

	if ((isKWSetStructures or isKWSetRecall or isKWSetMemory or
	  isKWSetPreferences) and (isKWSetProcedures or isKWSetFunctions))
	  e->Throw("Conflicting keywords.");

	if (isKWSetRecall) {
	  DStringGDL *previous_commands;
	  previous_commands = recall_commands_internal();
	  SizeT nEl2 = previous_commands->N_Elements();


	  if (doOutput) { //bug 1628
		*ostrp << "Recall buffer length: " << nEl2 << '\n';
		for (SizeT i = 0; i < nEl2; ++i) {
		  if (isKWSetNames and !CompareWithJokers(names, (*previous_commands)[i])) continue;
		  *ostrp << i + 1 << "  " << (*previous_commands)[i] << '\n';
		}
	  } else { //bug 1628
		char ctmp;
		int nb_lines = TermHeight();
        if (SysVar::More() == 0) nb_lines=10000; 
		*ostrp << "Recall buffer length: " << nEl2 << '\n';
		for (SizeT i = 0; i < nEl2; ++i) {
		  if (isKWSetNames and !CompareWithJokers(names, (*previous_commands)[i])) continue;
		  *ostrp << i + 1 << "  " << (*previous_commands)[i] << '\n';
		  if ((i > 0) && (i % (nb_lines - 4)) == 0) {
			*ostrp << "      < Press q or Q to quit, any key to continue, ? for help >" << '\n';
			ctmp = my_get_kbrd();
			nb_lines = TermHeight();
			if ((tolower(ctmp) == 'h') || (ctmp == '?')) {
			  *ostrp << "---------------------------------------------------    " << '\n';
			  *ostrp << "<space>		Display next page of text." << '\n';
			  *ostrp << "<return>	Display next line of text. (TBD)" << '\n';
			  *ostrp << "q or Q		Quit" << '\n';
			  *ostrp << "h, H, or ?	Display this message." << '\n';
			  *ostrp << "---------------------------------------------------" << '\n';
			  ctmp = my_get_kbrd();
			  nb_lines = TermHeight();
			}
			if (tolower(ctmp) == 'q') break;
		  }
		}
	  }
	  GDLDelete(previous_commands);
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr);
	  else cout << ostr.str();
	  return;
	}
	if (isKWSetMemory) {
	  ostr << "heap memory used: ";
	  ostr << MemStats::GetCurrent();
	  ostr << ", max: ";
	  ostr << MemStats::GetHighWater();
	  ostr << ", gets: ";
	  ostr << MemStats::GetNumAlloc();
	  ostr << ", frees: ";
	  ostr << MemStats::GetNumFree() << '\n';
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr);
	  else cout << ostr.str();
	  return;
	}
	if (isKWSetPreferences) {
	  //cout << "ici 1 " << isKWSetPreferences << '\n';
	  *ostrp << "Preferences: this is not ready ..." << '\n';
	  //*ostrp << GDL_GR_X_QSCREEN::GetValue();
	  if (doOutput) help_Output(outputKW, ostr, OutputLines);

	  return;
	}
	// switch to dec output (might be changed from formatted output)

	if (!doOutput) cout << dec;

	if ((nParam == 0 and !isKWSetMemory) or isKWSetFunctions or isKWSetProcedures) {

	  if (nParam == 0 and !isKWSetFunctions and !isKWSetProcedures and !routinesKW and !isKWSetNames)	SimpleDumpStack(e, *ostrp);

	  if (isKWSetProcedures or routinesKW) {
		*ostrp << "Compiled Procedures:\n$MAIN$\n";
		OutputLines += 2;
		DPro *pro = NULL;
		for (SizeT i = 1; i < pList.size(); ++i) {

		  // Find DPro pointer for pList[i]
		  ProListT::iterator p = std::find_if(proList.begin(), proList.end(), Is_eq<DPro>(pList[i]));
		  if (p != proList.end()) {
			pro = *p;
		  } else {
			for (auto j = 0; j < structList.size(); ++j) {
			  DStructDesc* s = structList[j];
			  for (auto k = 0; k < s->ProList().size(); ++k) {
				if ((s->ProList())[k]->ObjectName() == pList[i]) {
				  pro = (s->ProList())[k];
				  break;
				}
			  }
			}
		  }
		  if (pro == NULL) continue;
		  if (isKWSetNames and
			!CompareWithJokers(names, pro->ObjectName())) continue;
		  *ostrp << setw(25) << left << pro->ObjectName() << setw(0);
		  if (!briefKW) {
		  int nPar = pro->NPar();
		  int nKey = pro->NKey();
		  // Loop through parameters and keywords, sort keywords by alphabetic order
		  for (SizeT j = 0; j < pro->NPar(); ++j) *ostrp << StrLowCase(pro->GetVarName(nKey + j)) << " ";
		  std::vector<std::string>kwdList;
		  for (SizeT j = 0; j < pro->NKey(); ++j) kwdList.push_back(StrUpCase(pro->GetVarName(j)));
		  std::sort(kwdList.begin(), kwdList.end());
		  //sorted list can only be explored via pointer:
		  for (std::vector<std::string>::iterator j = kwdList.begin(); j != kwdList.end(); ++j) *ostrp << *j << " ";
			*ostrp << '\n';
			++OutputLines;
		  }
		}
		if (briefKW) {
		  *ostrp << '\n';
		  ++OutputLines;
		}
	  }

	  if (isKWSetFunctions or routinesKW) {
		*ostrp << "Compiled Functions:" << '\n';
		++OutputLines;
		// Loop through functions
		DFun *fun = NULL;
		for (SizeT i = 0; i < fList.size(); ++i) {
		  // Find DFun pointer for fList[i]
		  FunListT::iterator p;
		  p = std::find_if(funList.begin(), funList.end(), Is_eq<DFun>(fList[i]));
		  if (p != funList.end()) {
			fun = *p;
		  } else {
			for (auto j = 0; j < structList.size(); ++j) {
			  DStructDesc* s = structList[j];
			  for (auto k = 0; k < s->FunList().size(); ++k) {
				if ((s->FunList())[k]->ObjectName() == fList[i]) {
				  fun = (s->FunList())[k];
				  break;
				}
			  }
			}
		  }
		  if (fun == NULL) continue;
		  if (isKWSetNames and !CompareWithJokers(names, fun->ObjectName())) continue;
		  *ostrp << setw(25) << left << fun->ObjectName() << setw(0);
		  if (!briefKW) {
			int nPar = fun->NPar();
			int nKey = fun->NKey();
			for (SizeT j = 0; j < nPar; ++j)  *ostrp << StrLowCase(fun->GetVarName(nKey + j)) << " ";
			std::vector<std::string>kwdList;
			for (SizeT j = 0; j < fun->NKey(); ++j) kwdList.push_back(StrUpCase(fun->GetVarName(j)));
			std::sort(kwdList.begin(), kwdList.end());
		    //sorted list can only be explored via pointer:
		    for (std::vector<std::string>::iterator j = kwdList.begin(); j != kwdList.end(); ++j) *ostrp << *j << " ";
			*ostrp << '\n';
			OutputLines++;
		  }
		}
		if (briefKW) {
		  *ostrp << '\n';
		  ++OutputLines;
		}
	  }
	  if (doOutput) help_Output(outputKW, ostr, OutputLines);
	  if (isKWSetProcedures or isKWSetFunctions or routinesKW) return;
	}

	// Excluding keywords which are exclusive is not finished ...

	if (isKWSetPreferences) {
	  //cout << "ici 1 " << isKWSetPreferences << '\n';
	  *ostrp << "Preferences: this is not ready ..." << '\n';
	  //*ostrp << GDL_GR_X_QSCREEN::GetValue();
	  if (doOutput) help_Output(outputKW, ostr, OutputLines);

	  return;
	}
	static int commonIx = e->KeywordIx("COMMON");
	if (e->KeywordSet(commonIx)) { // list in internal order
	  CommonListT::iterator it;
	  for (it = commonList.begin(); it != commonList.end(); ++it) {
		SizeT nVar = (*it)->NVar();
		if (nVar == 0) continue;
		*ostrp << " Common block (" << (*it)->Name() <<
		  ") listed in internal order:" << '\n';
		for (SizeT vIx = 0; vIx < nVar; ++vIx) {
		  DString parString = (*it)->VarName(vIx) + " (" + (*it)->Name() + ')';
		  DVar* var = (*it)->Var(vIx);
		  if (var->Data() not_eq NULL || fullKW)
			help_item(*ostrp, var->Data(), parString, false);
		}
	  }
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr);
	  else cout << ostr.str();
	  if (nParam == 0) return;
	}

	if (debugKW) std::cout << " help_pro: nParam=" << nParam;
	for (SizeT i = 0; i < nParam; ++i) {
	  BaseGDL*& par = e->GetPar(i);
	  DString parString = e->Caller()->GetString(par, true); //= string(" ??? ");
	  if (debugKW) std::cout << ". ";
	  if (par == NULL) {
		if (debugKW) std::cout << " par ==(NULL)" << '\n';
		if (!briefKW) help_item(ostr, par, parString, false);
		continue;
	  }

	  // NON-STRUCTURES except if one and only one param is a Struct.
	  if (par->Type() == GDL_STRUCT && !briefKW) {
		if (((par->N_Elements() == 1) && nParam == 1) || isKWSetStructures) {
		  help_struct(ostr, par, 0, debugKW);
		  continue;
		}
	  }
	  help_item(ostr, par, parString, false);
	  if (nParam != 1) continue;
	  else if (par->Type() == GDL_OBJ and par->StrictScalar()) {
		DObj s = (*static_cast<DObjGDL*> (par))[0];
		DStructGDL* oStructGDL;
		if (s != 0) oStructGDL = GDLInterpreter::GetObjHeapNoThrow(s);
		if (s != 0 and (isKWSetStructures or fullKW))
		  help_struct(ostr, oStructGDL, 0, debugKW);
	  } else if (par->Type() == GDL_OBJ) {
		SizeT nObj = par->N_Elements();
		if (debugKW) std::cout << "nObj=" << nObj;
		for (SizeT iobj = 0; iobj < nObj; ++iobj) {
		  if (debugKW) std::cout << "-";
		  DObj s = (*static_cast<DObjGDL*> (par))[iobj];
		  //				object_help( ostr, s, false);
		  DStructGDL* oStructGDL;
		  DString classname;
		  DString nameobj = "  [" + i2s(iobj) + "] ";
		  if (s != 0) {
			// Name display
			if (debugKW) std::cout << "-";
			std::vector< std::string> pNames;
			ostr.width(16);
			ostr << left << nameobj;
			oStructGDL = GDLInterpreter::GetObjHeapNoThrow(s);
			if (oStructGDL == NULL) continue;
			if (debugKW) std::cout << ".";
			classname = oStructGDL->Desc()->Name();
			oStructGDL->Desc()->GetParentNames(pNames);
			ostr.width(10);
			ostr << classname << right;
			ostr << left << "<ID=" << i2s(s) << "> " << right;
			DString parents;
			if (pNames.size() == 0) parents = "";
			else if (pNames.size() == 1) parents = "  parent:";
			else parents = " parents:";
			ostr << left << parents;
			for (SizeT i = 0; i < pNames.size(); ++i) ostr << " " << pNames[i];
			ostr << '\n';
		  } else
			help_item(ostr, NULL, nameobj, false);
		}
	  }

	}
	if (debugKW) std::cout << " :=:" << '\n';
	if (nParam > 0) {
	  if (doOutput) (*outputKW) = StreamToGDLString(ostr);
	  else cout << ostr.str();
	  return;
	}


	static int levelIx = e->KeywordIx("LEVEL");
	if (e->KeywordPresent(levelIx)) {
	  std::ostringstream parameters_ostringstream;
	  DLongGDL* level = e->IfDefGetKWAs<DLongGDL>(levelIx);

	  //will list (all) variables, incl. common defined, at desired level.
	  EnvStackT& callStack = e->Interpreter()->CallStack();
	  DLong curlevnum = callStack.size();
	  DLong desiredlevnum = curlevnum;
	  if (level != NULL) {
		desiredlevnum = (*level)[0];
		if (desiredlevnum <= 0) desiredlevnum += curlevnum;
		if (desiredlevnum < 1) desiredlevnum = 0;
		if (desiredlevnum > curlevnum) desiredlevnum = curlevnum;
	  }

	  DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum - 1]->GetPro());

	  SizeT nVar = pro->Size(); // # var in GDL for desired level 
	  SizeT nComm = pro->CommonsSize(); // # has commons?
	  SizeT nTotVar = nVar + nComm; //All the variables availables at that lev.
	  if (debugKW) cout << " Level section: nTotVar=" << nTotVar << '\n';
	  if (nTotVar > 0) {
		set<string> helpStr; // "Sorted List" 
		if (nVar > 0) {
		  for (SizeT i = 0; i < nVar; ++i) {
			BaseGDL*& par = (static_cast<EnvUDT*> (callStack[desiredlevnum - 1]))->GetTheKW(i);
			if (par != NULL) {
			  stringstream ss;
			  string parName = pro->GetVarName(i);
			  help_item(ss, par, parName, false);
			  helpStr.insert(ss.str());
			}
		  }
		}

		if (nComm > 0) {
		  DStringGDL* list = static_cast<DStringGDL*> (pro->GetCommonVarNameList());
		  for (SizeT i = 0; i < list->N_Elements(); ++i) {
			BaseGDL** par = pro->GetCommonVarPtr((*list)[i]);
			DCommonBase* common = pro->FindCommon((*list)[i]);
			DString parName = (*list)[i] + " (" + common->Name() + ')';
			stringstream ss;
			help_item(ss, (*par), parName, false);
			helpStr.insert(ss.str());
		  }
		}
		copy(helpStr.begin(), helpStr.end(), ostream_iterator<string>(parameters_ostringstream));
	  }
	}
	//					 if /brief and /routines then no var. to be shown
	if (routinesKW and briefKW) kw = true;

	if (nParam == 0 and !kw) {
	  routinesKW = !isKWSetNames; //when "NAMES" is present, ROUTINES ARE NOT GIVEN.
	  briefKW = true;

	  // list all variables of caller
	  EnvBaseT* caller = e->Caller();

	  SizeT nEnv = caller->EnvSize();

	  //cout << "EnvSize() " << nEnv << '\n';

	  set<string> helpStr; // "Sorted List" 
	  stringstream ss;

	  for (int i = 0; i < nEnv; ++i) {
		BaseGDL*& par = caller->GetTheKW(i);

		if (par == NULL && !fullKW) continue;
		// if( par == NULL) continue;

		DString parString = caller->GetString(par, true);
		if (debugKW) cout << " GetKW(i)->GetString(par) " << parString;
		if (isKWSetNames and
		  !CompareWithJokers(names, parString)) continue;
		help_item(ss, par, parString, false);
		helpStr.insert(ss.str());
		ss.str("");
	  }
	  if (debugKW) cout << '\n';
	  std::vector<DCommonBase*> cptr;
	  DSubUD* proUD = dynamic_cast<DSubUD*> (caller->GetPro());
	  proUD->commonPtrs(cptr);
	  std::vector<DCommonBase*>::iterator ic;
	  for (ic = cptr.begin(); ic != cptr.end(); ++ic) {
		SizeT nVar = (*ic)->NVar();
		for (SizeT vIx = 0; vIx < nVar; ++vIx) {
		  DString parString = (*ic)->VarName(vIx) + " (" + (*ic)->Name() + ')';
		  if (isKWSetNames and
			!CompareWithJokers(names, parString)) continue;
		  DVar* var = (*ic)->Var(vIx);
		  if ((var->Data() not_eq NULL) || fullKW) {
			help_item(ss, var->Data(), parString, false);
			helpStr.insert(ss.str());
			ss.str("");
		  }
		}
	  }

	  copy(helpStr.begin(), helpStr.end(),
		ostream_iterator<string>(*ostrp));
	  OutputLines += helpStr.size();

	} // if( nParam == 0  and !kw)
	if (routinesKW and briefKW) {
	  // Display compiled procedures & functions
	  if (!isKWSetProcedures and !isKWSetFunctions) {

		*ostrp << "Compiled Procedures:" << '\n';
		for (SizeT i = 0; i < pList.size(); ++i) *ostrp << pList[i] << " ";
		*ostrp << '\n' << '\n';
		*ostrp << "Compiled Functions:" << '\n';
		for (SizeT i = 0; i < fList.size(); ++i) *ostrp << fList[i] << " ";
		*ostrp << '\n';
		OutputLines += 4;
	  }
	}
	if (doOutput) help_Output(outputKW, ostr, OutputLines);
	return;
  }
  

  } // namespace

  void help_AddDlmInfo(DStringGDL* s){
  dlmInfo.push_back(s);
  }
