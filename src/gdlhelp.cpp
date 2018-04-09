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
#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"


#include "graphicsdevice.hpp"
#include "dcommon.hpp"
#include "dpro.hpp"

#include "gdlhelp.hpp"

// for sorting compiled pro/fun lists by name
struct CompFunName: public std::binary_function< DFun*, DFun*, bool>
{
  bool operator() ( DFun* f1, DFun* f2) const
	{
    return f1->ObjectName() < f2->ObjectName();
  }
};

struct CompProName: public std::binary_function< DPro*, DPro*, bool>
{
  bool operator() ( DPro* f1, DPro* f2) const
	{
    return f1->ObjectName() < f2->ObjectName();
  }
};

static bool CompareWithJokers(string names, string sourceFiles) {

#ifdef _WIN32
  WCHAR wnames[MAX_PATH];
  WCHAR wsourceFiles[MAX_PATH];

  const char* cnames = names.c_str();
  const char* csourceFiles = sourceFiles.c_str();

  MultiByteToWideChar(CP_UTF8, 0, cnames, -1,
    (LPWSTR) wnames, MAX_PATH);
  MultiByteToWideChar(CP_UTF8, 0, csourceFiles, -1,
    (LPWSTR) wsourceFiles, MAX_PATH);
  int match = 1 - PathMatchSpecW(wsourceFiles, wnames);
#else
  int match = fnmatch(names.c_str(), sourceFiles.c_str(), 0 );
#endif
  if ( match == 0) 	return true;
  else 		return false;
  }
extern GDLFileListT  fileUnits;

    using namespace std;
// (static = internal) helper routines serving the lib:: routines called out in 
// gdlhelper.hpp
static void help_files(ostream& os,
					EnvT* e) {
  for( DLong lun=maxUserLun+1; lun <= fileUnits.size(); lun++)
    if( fileUnits[ lun-1].InUse() || fileUnits[ lun-1].GetGetLunLock())
    {
		os << 
	    "	 lun "<< lun << ": "+fileUnits[lun-1].Name() << endl;
    }
     return;
}

static void help_keys(ostream& ostr)
  { 
	ostr << "GDL is using Readline to manage keys shortcuts, few useful listed below." << endl;
	ostr << "A summary can be read here : http://www.bigsmoke.us/readline/shortcuts " << endl;
	ostr << endl;
	ostr << "Moving in the command line :"<< endl;
	ostr << "  Ctrl-a          : going to the beginning of the line"<< endl;
	ostr << "  Ctrl-e          : going to the end of the line"<< endl;
	ostr << "  Ctrl-u          : removing from here to the beginning of the line"<< endl;
	ostr << "  Ctrl-k          : removing from here to the end of the line"<< endl;
	ostr << "  Ctrl-RightArrow : jumping one word on the right"<< endl;
	ostr << "  Ctrl-LeftArrow  : jumping one word on the left"<< endl;
	ostr << endl;
	ostr << "Moving in the history :"<< endl;
	ostr << "  HELP, /recall       : listing the whole history" << endl;
	ostr << "  Ctrl-p or UpArrow   : previous entry in history"  << endl;
	ostr << "  Ctrl-n or DownArrow : next entry in history"  << endl;
	ostr << endl;    
	return;
  }

static void help_info()
  { 

	  cout << "* Homepage: http://gnudatalanguage.sf.net" << endl;
	  cout << endl;
	  cout << "* #NameOfRoutine for list of params and keywords" 
	    " for a given NameOfRoutine (internal or compiled pro/func)" << endl;
	  cout << endl;
	  cout << "* ?NameOfRoutine for starting a browser to access online doc"
	    " for a given routine (if exists ! internal or compiled pro/func)" ;
	  cout << endl; cout << endl;
	  cout << "* HELP, /INTERNAL_LIB_GDL for a list of all internal library "
	    "functions/procedures." << endl;
	  cout << "* HELP, /LIB Additional subroutines are written in GDL language, "
	    "look for *.pro files (e.g. in CVS in src/pro/)." << endl;
	  cout << endl;
	  cout << "* HELP, /KEYS for useful CLI keys shortcuts." << endl;
	  cout << endl;
		return;
}
static void help_sysvar(ostream& os , bool briefKW=true)
  {
	  if (briefKW) {
	    string tmp_line="", tmp_word="";
	    for( SizeT v=0; v<sysVarList.size(); ++v)
	      {
	      DVar* var = sysVarList[ v];
	      tmp_word=" !"+var->Name();
	      if (tmp_line.length()+tmp_word.length() >= 80) {
		    os << tmp_line <<endl;
			tmp_line=tmp_word;
			}
			else {
			tmp_line=tmp_line+tmp_word;
	        }
	      }
	      if (tmp_line.length() > 0) os << tmp_line <<endl;
		}
		else {
	      for( SizeT v=0; v<sysVarList.size(); ++v) {
			DVar* var = sysVarList[ v];
			DStructGDL* tmp= tmp=static_cast<DStructGDL*>(var->Data());
			lib::help_item(os, tmp, "!"+var->Name(), false);	    
		  }
		}
	return;
  }
/*
    // AC 14-08-11 : I don't know how to copy the list in a String Array
    // then sorting is ... SysVar at not ordered ...
    // help,/sys  and help,/brief,/sys are ok
    if (sysvarKW) {
      SizeT nVar = sysVarList.size();
      if (briefKW) {
        for (SizeT v = 0; v < nVar; ++v) {
          DVar* var = sysVarList[v];
          ostr.width(12);
          ostr.flags(std::ios::left);
          ostr << "!"+var->Name() << std::endl;
        }
      } else {
        for (SizeT v = 0; v < nVar; ++v) {
          DVar* var = sysVarList[v];
          DStructGDL* tmp = static_cast<DStructGDL*> (var->Data());
          help_item(ostr, tmp, "!" + var->Name(), false);
        }
      }
       if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  SortAndPrintStream(ostr);
      return;
    }
*/

static void help_Output(BaseGDL** outputKW, ostringstream& ostr, SizeT &nlines, bool doOutput=true)
  {      static volatile bool debug(false);
	// Setup output return variable ostream& os, int &lines_count

	std::string s = ostr.rdbuf()->str().erase(ostr.rdbuf()->str().length(),1);
	char delimiter = '\n';
	SizeT nOut = 0;
	size_t found=0;
	if(doOutput) {

	  do {
		nOut++;
		found=s.find( delimiter,found);
	     }   while( (found++ != std::string::npos) );

	  nlines = --nOut;	if(nlines == 0) return;
	  GDLDelete((*outputKW));
	  dimension dim(&nlines, (size_t) 1);
	  *outputKW = new DStringGDL(dim, BaseGDL::NOZERO);
	}
	size_t pos = 0;
	nOut = 0;
	std::string token;
	while ((found = s.find(delimiter,pos)) != std::string::npos) {
		token = s.substr(pos, found-pos);
		if( doOutput and (nOut not_eq nlines)) (*(DStringGDL *) *outputKW)[nOut] = token;
		else cout << token << endl;
		nOut++;
		pos = found+1;
		}
	    ostr.str("");
	    if( nOut  not_eq nlines and debug) cout << 
			" help_Output: Error counting lines -" <<
			" nOut: "<<nOut<<" OutputLines:"<<nlines<<endl;
	}

    // showing HELP, /path_cache
  void help_path_cached(ostream& os, SizeT &lines_count) {
    DIR *dirp;
    struct dirent *dp;
    const char *ProSuffix = ".pro";
    int ProSuffixLen = strlen(ProSuffix);
    int NbProFilesInCurrentDir;
    string tmp_fname;
    size_t found;

    StrArr path = SysVar::GDLPath();

    os << "!PATH (no cache management --now-- in GDL, ";
    os << path.size() << " directories)" << endl;
    lines_count = 1;

    for (StrArr::iterator CurrentDir = path.begin(); CurrentDir != path.end(); ++CurrentDir) {
      //	  cout << "1>>" << (*CurrentDir).c_str() << "<<" <<endl;
      NbProFilesInCurrentDir = 0;
      dirp = opendir((*CurrentDir).c_str());
      //cout << "2>>" << dirp << "<<" <<endl;
      if (dirp != NULL) {
        while ((dp = readdir(dirp)) != NULL) {
          tmp_fname = dp->d_name;
          found = tmp_fname.rfind(ProSuffix);
          if (found != std::string::npos) {
            if ((found + ProSuffixLen) == tmp_fname.length())
              NbProFilesInCurrentDir++;
          }
        }
        closedir(dirp);
        lines_count++;
        os << *CurrentDir << " (" << NbProFilesInCurrentDir << " files)" << endl;
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
      ost << std::endl;
    }
	  return;
  }
static void help_ListMethods(DString names, ostream& ostr, FunListT& funlist, ProListT& prolist)
{
	bool searchbyname;
	searchbyname = (names != "");
	vector<DString> subList;
	DString proname;
	for( SizeT i = 0; i<prolist.size(); ++i)  {
	  proname = prolist[i]->Name();
		 if(searchbyname and not 
			CompareWithJokers(names, proname)) continue;
		  subList.push_back(proname);
		}
	sort( subList.begin(), subList.end());
	ostr << "Method procedures (" 
			<< subList.size() <<"):" << endl;
	for( SizeT i = 0; i<subList.size(); ++i)
		ostr << " "<< subList[ i] ;
	ostr << endl;

	subList.clear();

	for( SizeT i = 0; i<funlist.size(); ++i)  {
	  proname = funlist[i]->Name();
		 if(searchbyname and not 
			CompareWithJokers(names, proname)) continue;
		  subList.push_back(proname);
		}
	sort( subList.begin(), subList.end());
	ostr << "Method functions (" 
			<< subList.size() <<"):" << endl;
	for( SizeT i = 0; i<subList.size(); ++i)
		ostr << " "<< subList[ i] ;
	ostr << endl;
	  subList.clear();
}

static void help_object( ostream& os, DString parString, bool verbose=true )
  {
}
static void help_ListLib(DString names, ostream& os, bool internal=true)
  {

  }

static void help_heap(EnvT* e, ostream& os, bool verbose=true)
  {
// GJ 16-04-10
	SizeT numPtr = e->Interpreter()->HeapSize();
	SizeT numObj = e->Interpreter()->ObjHeapSize();
	os << "Heap Variables:" << std::endl;
	os << "    # Pointer: " << numPtr	 << std::endl;
	os << "    # Object : " << numObj	 << std::endl<< std::endl;
	if( ! verbose) return;
	std::vector<DPtr>* heap = e->Interpreter()->GetAllHeapSTL();
	Guard< std::vector<DPtr> > heap_guard( heap);

	SizeT nH = heap->size();
	if( nH <= 0 || (*heap)[0] == 0) return;
	SizeT nDisp = (nH < 50) ? nH : 15;
	for( SizeT h=0; h<nDisp; ++h)
	  {
	DPtr p = (*heap)[ h];
	if( e->Interpreter()->PtrValid( p))
//	  if( ptrAccessible.find( p) == ptrAccessible.end())
		{
		  BaseGDL* hV = BaseGDL::interpreter->GetHeap( p);
		  lib::help_item( os, 
				  hV, DString( "<PtrHeapVar")+
				  i2s(p)+">",
				  false);
		}
	}
	if(nH > nDisp) os << nH << " pointers in total"<< endl;
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
	  if (e->KeywordPresent( outputIx)) 
	    {    // Setup output return variable
	      outputKW = &e->GetKW( outputIx);
	      GDLDelete((*outputKW));
	      *outputKW = static_cast<DStringGDL*>((errorState->GetTag( msgTag))
	                                             ->Convert2( GDL_STRING, BaseGDL::COPY));
	      return;
			}
			else {
	    cout << (*static_cast<DStringGDL*>( errorState->GetTag( msgTag)))[0]<< endl;
	    return;
	  }
  }
    

  static DStringGDL* recall_commands_internal()
  {
    // maybe obsolete ??? to be check 
    // AC 24 Oct 2016 if defined(HAVE_LIBREADLINE) && !defined(__APPLE__)
      //int status=0;
      DStringGDL* retVal;
      retVal = new DStringGDL(1, BaseGDL::NOZERO);
      (*retVal)[ 0] ="";
#if defined(HAVE_LIBREADLINE) || defined(HAVE_LIBEDITLINE)
    //status=1;
    // http://cnswww.cns.cwru.edu/php/chet/readline/history.html#IDX14
    HIST_ENTRY **the_list;
    //    cout << "history_length" << history_length << endl;
    the_list = history_list();

    if (the_list) {
      DStringGDL* retVal = new DStringGDL(history_length - 1, BaseGDL::NOZERO);
      for (SizeT i = 0; i < history_length - 1; i++)
        (*retVal)[i] = the_list[i]->line;
      return retVal;
    } else return new DStringGDL("");
#else
    Message("RECALL_COMMANDS: nothing done, because compiled without READLINE");
    return new DStringGDL("");
#endif
      return retVal;
  }

namespace lib {

  using namespace std;


BaseGDL* recall_commands( EnvT* e)
  {
      return recall_commands_internal();
  }


  // display help for one variable or one structure tag

  void help_item(ostream& os,
    BaseGDL* par, DString parString, bool doIndentation) {
    int debug = 0;
    if (debug) {
      cout << par->Type() << endl;
      cout << par->TypeStr() << endl;
      cout << &par->TypeStr() << endl;
      //	cout << par->Name() << endl;
    }

    if (doIndentation) os << "   ";

    // Name display
    os.width(16);
    os << left << parString;
    if (parString.length() >= 16) {
      os << " " << endl; // for cmsv compatible output (uses help,OUTPUT)
      os.width(doIndentation ? 19 : 16);
      os << "";
    }

    // Type display (we have two "null" : defined !null and undefined variables ...
    if (par == NULL) {
      os << "UNDEFINED = <Undefined>" << endl;
      return;
    }
    if (!par) {
      os << "UNDEFINED = !NULL" << endl;
      return;
    }
    os.width(10);
    bool doTypeString = true;

    // Data display
    if (par->Type() == GDL_STRUCT) {
      os << par->TypeStr() << right;
      if (!doIndentation) os << "= ";
      doTypeString = false;

      DStructGDL* s = static_cast<DStructGDL*> (par);
      os << "-> ";
      os << (s->Desc()->IsUnnamed() ? "<Anonymous>" : s->Desc()->Name());
      os << " ";
	}
      else if( par->Dim( 0) == 0)
	{
      if (par->Type() == GDL_STRING) {
        os << par->TypeStr() << right;
        if (!doIndentation) os << "= ";
        doTypeString = false;

        // trim string larger than 45 characters
        DString dataString = (*static_cast<DStringGDL*> (par))[0];
        os << "'" << StrMid(dataString, 0, 45, 0) << "'";
        if (dataString.length() > 45) os << "...";
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
              os << desc->Name();

              unsigned nListTag = desc->TagIndex("NLIST");
              DLong nList = (*static_cast<DLongGDL*> (oStructGDL->GetTag(nListTag, 0)))[0];
              os << left;
              os << "<ID=";
              os << i2s(s) << "  N_ELEMENTS=" << i2s(nList) << ">";

              doTypeString = false;
            }
            static DString hashName("HASH");
            if (desc->IsParent(hashName)) {
              os << desc->Name();

              unsigned nListTag = desc->TagIndex("TABLE_COUNT");
              DLong nList = (*static_cast<DLongGDL*> (oStructGDL->GetTag(nListTag, 0)))[0];
              os << left;
              os << "<ID=";
              os << i2s(s) << "  N_ELEMENTS=" << i2s(nList) << ">";

              doTypeString = false;
            }
          }
        }
      }
      if (doTypeString) {
        os << par->TypeStr() << right;
        if (!doIndentation) os << "= ";
        doTypeString = false;

        par->ToStream(os);
      }
    }

    if (doTypeString) {
      os << par->TypeStr() << right;
      if (!doIndentation) os << "= ";
      if (par->IsAssoc())
        par->ToStream(os);
    }

    // Dimension display
    if (par->Dim(0) != 0) os << par->Dim();

    // End of line
    os << endl;
  }

void help_struct(ostream& ostr,  BaseGDL* par, int indent=0, bool debug=false)
  {
	   // STRUCTURES
	   DStructGDL* s = static_cast<DStructGDL*>( par);
	   SizeT nTags = s->Desc()->NTags();

		for(int i=0; i < indent; i++) ostr <<"   ";
		ostr << "** Structure ";
		ostr << (s->Desc()->IsUnnamed() ? "<Anonymous>" : s->Desc()->Name());
		ostr << ", " << nTags << " tags";
		if(indent == 0) {
			ostr << ",memsize =" << s->Sizeof();
			ostr << ", data length=" << s->NBytesToTransfer()
			<< "/" << s->SizeofTags() ;
		}
		ostr << ":" << endl;

		for (SizeT t=0; t < nTags; ++t) {
			for(int i=0; i < indent; i++) ostr <<"   ";
			if(debug) ostr.width(18);
			if(debug) ostr <<"dbg: OFFSET="<<s->Desc()->Offset(t);
		    help_item( ostr, s->GetTag(t), s->Desc()->TagName(t), true);
		    if(s->GetTag(t)->Type() == GDL_STRUCT) 
				help_struct(ostr, s->GetTag(t), indent+1);
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
		ostr << ":" << endl;
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
#ifdef _WIN32
// according to doc, this is what std::endl should look like on windows. Not tested.
    std::string delimiter = "\r\n";
#else
    std::string delimiter = "\n";
#endif
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
    std::cout << std::endl;
}
  
  DStringGDL* StreamToGDLString(ostringstream& oss, bool sorted=false) {
#ifdef _WIN32
// according to doc, this is what std::endl should look like on windows. Not tested.
    std::string delimiter = "\r\n";
#else
    std::string delimiter = "\n";
#endif
    int nlines = 0;
    size_t pos = 0;
    while ((pos = oss.str().find(delimiter, pos + 1)) != std::string::npos) {
      nlines++;
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
    // in order of priority
    bool kw = false;
    static int lastmKWIx = e->KeywordIx("LAST_MESSAGE");
    bool lastmKW = e->KeywordPresent(lastmKWIx);
	SizeT nParam=e->NParam();

    BaseGDL** outputKW = NULL;
    static int outputIx = e->KeywordIx("OUTPUT");
    bool doOutput = (e->KeywordPresent(outputIx));
    
    if (doOutput) { // Setup output return variable
      outputKW = &e->GetKW(outputIx);
      GDLDelete((*outputKW));
    }
	static SizeT OutputLines;
	OutputLines = 0;

    std::ostringstream ostr;
// Use mostly ostrp* << from here on and then push onto outputKW if need be.
      std::ostream* ostrp = (doOutput) ? &ostr : &cout;

    // if LAST_MESSAGE is present, it is the only output.
    // All other kw are ignored *EXCEPT 'output'*.
    if (lastmKW) {
      DStructGDL* errorState = SysVar::Error_State();
      static unsigned msgTag = errorState->Desc()->TagIndex("MSG");
      ostr << (*static_cast<DStringGDL*> (errorState->GetTag(msgTag)))[0] << endl;
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else std::cout << ostr.str();
      return;
    }

    static int helpKWIx = e->KeywordIx("HELP");
    bool helpKW = e->KeywordPresent(helpKWIx);
    if (helpKW) {
      string inline_help[] = {"Usage: " + e->GetProName() + ", expr1, ..., exprN,",
        "          /ALL_KEYS, /BRIEF, /CALLS, /COMMON, /FUNCTIONS, /HELP, /INFO,",
        "          /INTERNAL_LIB_GDL, /KEYS, /LAST_MESSAGE, /LIB, /MEMORY,",
        "          NAMES=string_filter, OUTPUT=res, /PATH_CACHE,",
        "          /PREFERENCES, /PROCEDURES, /RECALL_COMMANDS, /ROUTINES,",
        "          /SOURCE_FILES, /STRUCTURES, /SYSTEM_VARIABLES, /TRACEBACK"};
      int size_of_s = sizeof (inline_help) / sizeof (inline_help[0]);
      e->Help(inline_help, size_of_s);
      return;
    }

    static int allkeysIx=e->KeywordIx("ALL_KEYS");
    static int keysIx=e->KeywordIx("KEYS");
    if (e->KeywordSet(allkeysIx) || e->KeywordSet(keysIx)) // ALL_KEYS is an obsolete keyword
			{help_keys(*ostrp); return;}

    static int pathKWIx = e->KeywordIx("PATH_CACHE");
    bool pathKW = e->KeywordPresent(pathKWIx);
    if (pathKW) {
      int debug = 0;
      SizeT lines_count = 0;
      help_path_cached(ostr, lines_count);
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout << ostr.str();
      return;
    }

    // if keyword /TraceBack then we return
    static int tracebackKWIx = e->KeywordIx("TRACEBACK");
    bool tracebackKW = e->KeywordSet(tracebackKWIx);

    if (tracebackKW) {
      SimpleDumpStack(e, ostr);
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout << ostr.str();
      return;
    }

    static int briefKWIx = e->KeywordIx("BRIEF");
    bool briefKW = e->KeywordSet(briefKWIx);
    // briefKw should be used with DLM HEAP_VARIABLES MESSAGES OBJECTS ROUTINES SOURCE_FILES STRUCTURES SYSTEM_VARIABLES
    static int fullKWIx = e->KeywordIx("FULL");
    bool fullKW = e->KeywordSet(fullKWIx);

    // AC 14-08-11 : detailed info (display size, deep ...) are missing
    static int deviceKWIx = e->KeywordIx("DEVICE");
    bool deviceKW = e->KeywordPresent(deviceKWIx);
    if (deviceKW) {
      GraphicsDevice::ListDevice(ostr);
      DString name = (*static_cast<DStringGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("NAME"), 0)))[0];
      ostr << "Current graphics device: " << name << endl;
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  cout<<ostr.str();
      return;
    }

	static int heapIx = e->KeywordIx( "HEAP");
	if(  e->KeywordSet( heapIx)) {
		help_heap(e, *ostrp, fullKW);
//		  if(doOutput) help_Output(outputKW, ostr, OutputLines );
		return;}

	static int namesIx = e->KeywordIx( "NAMES");
	bool isKWSetNames= e->KeywordPresent( namesIx);

	static int sysvarIx = e->KeywordIx( "SYSTEM_VARIABLES");
	if( e->KeywordSet( sysvarIx)) { help_sysvar(ostr, briefKW); return;}
/*
OBJECTS
Set this keyword to display information on defined object classes.
*  If no arguments are provided, all currently-defined object classes are shown. 
*  If arguments are provided, the definition of the object class
*  for the heap variables referred to is displayed.
	
 */
	static int objectsIx = e->KeywordIx( "OBJECTS");
	if( e->KeywordSet( objectsIx) and (nParam ==0)) {
		SizeT nObj = structList.size();
		for( SizeT i=0; i<nObj; ++i)
		  {
			int num_methods = structList[i]->FunList().size() +
								structList[i]->ProList().size();
			if( num_methods == 0) continue;
			ostrp->width(20);
			*ostrp << right << structList[i]->Name() << endl;
		  }
		help_Output(outputKW, ostr, OutputLines);
		return;
	  	}
	
    DString names = "";
    if (isKWSetNames) {
      e->AssureStringScalarKWIfPresent(namesIx, names);
      // since routines and var. are stored in Maj, we convert ...
      names = StrUpCase(names);
    }

    static int sourceFilesKWIx = e->KeywordIx("SOURCE_FILES");
    bool sourceFilesKW = e->KeywordPresent(sourceFilesKWIx);

    static int routinesKWIx = e->KeywordIx("ROUTINES");
    bool routinesKW = e->KeywordSet(routinesKWIx);

    static int ProceduresIx = e->KeywordIx("PROCEDURES");
    bool isKWSetProcedures = e->KeywordSet(ProceduresIx);
    
    static int FunctionsIx = e->KeywordIx("FUNCTIONS");
    bool isKWSetFunctions = e->KeywordSet(FunctionsIx);

    bool do_pro = isKWSetProcedures;
    bool do_fun = isKWSetFunctions;
    
    if (sourceFilesKW && routinesKW) e->Throw("Conflicting keywords.");
    
    if (sourceFilesKW) {
      do_pro = true;
      do_fun = true;
      if (isKWSetProcedures && !isKWSetFunctions) do_fun = false;
      if (!isKWSetProcedures && isKWSetFunctions) do_pro = false;
    } else if (routinesKW) {
      do_pro = true;
      do_fun = true;
      if (isKWSetProcedures && !isKWSetFunctions) do_fun = false;
      if (!isKWSetProcedures && isKWSetFunctions) do_pro = false;
    }

    if (sourceFilesKW) {
      if (do_pro) {
	// AC 2018-01-09 : Duplicating the pro list to avoid messing up the order of "proList"
	// otherwise, a call to HELP,/source created in future calls
	// e.g. of crashing sequence : TEST_TV & HELP, /source & TEST_TV
	//
	ProListT proList_tmp;
	proList_tmp=proList;
        sort(proList_tmp.begin(), proList_tmp.end(), CompProName());

        ostr << "Compiled Procedures:" << endl;
        if (briefKW) {
          ostr << "$MAIN$" << endl;
          for (ProListT::iterator i = proList_tmp.begin(); i != proList_tmp.end(); ++i) {
            ostr << setw(25) << left << (*i)->ObjectName() << setw(0) <<endl;
          }
        } else if (isKWSetNames) {
          for (ProListT::iterator i = proList_tmp.begin(); i != proList_tmp.end(); ++i) {
            if (CompareWithJokers(names, (*i)->ObjectName())) {
              ostr << setw(25) << left << (*i)->ObjectName() << setw(0);
              ostr << (*i)->GetFilename() << endl;
            }
          }
        } else {
          ostr << "$MAIN$" << endl;
          for (ProListT::iterator i = proList_tmp.begin(); i != proList_tmp.end(); ++i) {
            ostr << setw(25) << left << (*i)->ObjectName() << setw(0);
            ostr << (*i)->GetFilename() << endl;
          }
        }
      }

      if (do_fun) {
	// AC 2018-01-09 : Duplicating the fun list to avoid messing up the order of "funList"
	// see above in (do_pro).
	FunListT funList_tmp;
	funList_tmp=funList;
        sort(funList_tmp.begin(), funList_tmp.end(), CompFunName());

	if (do_pro) ostr << endl;
	ostr << "Compiled Functions:" << endl;
        if (briefKW) {
          for (FunListT::iterator i = funList_tmp.begin(); i != funList_tmp.end(); ++i) {
            ostr << setw(25) << left << (*i)->ObjectName() << setw(0) <<endl;
          }
        } else if (isKWSetNames) {
          for (FunListT::iterator i = funList_tmp.begin(); i != funList_tmp.end(); ++i)
            if (CompareWithJokers(names, (*i)->ObjectName())) {
              ostr << setw(25) << left << (*i)->ObjectName() << setw(0);
              ostr << (*i)->GetFilename() << endl;
            }
        } else {
          for (FunListT::iterator i = funList_tmp.begin(); i != funList_tmp.end(); ++i) {
            ostr << setw(25) << left << (*i)->ObjectName() << setw(0);
            ostr << (*i)->GetFilename() << endl;
          }
        }
      }
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  cout<<ostr.str();
      return;
    }

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

	static int infoIx = e->KeywordIx("INFO");
	if( e->KeywordSet( infoIx)){  kw = true; help_info(); }

    static int kwlibIx=e->KeywordIx("LIB");
    bool kwLib = e->KeywordSet(kwlibIx);
    if (kwLib) {

      vector<DString> subList;
      SizeT nPro = libProList.size();
      for (SizeT i = 0; i < nPro; ++i) {
        if (!libProList[i]->GetHideHelp())
          subList.push_back(libProList[i]->ToString());
      }
      sort(subList.begin(), subList.end());

      SizeT nProList = subList.size();
      cout << "Library procedures (" << nProList << "):" << endl;
      for (SizeT i = 0; i < nProList; ++i)
        cout << subList[i] << endl;

      subList.clear();

      SizeT nFun = libFunList.size();
      for (SizeT i = 0; i < nFun; ++i) {
        if (!libFunList[i]->GetHideHelp())
          subList.push_back(libFunList[i]->ToString());
      }
      sort(subList.begin(), subList.end());

      SizeT nFunList = subList.size();
      cout << "Library functions (" << nFunList << "):" << endl;
      for (SizeT i = 0; i < nFunList; ++i)
        cout << subList[i] << endl;
      
      return;

    }

    // internal library functions
    static int INTERNAL_LIB_GDLIx=e->KeywordIx("INTERNAL_LIB_GDL");
    bool kwLibInternal = e->KeywordSet(INTERNAL_LIB_GDLIx);
    if (kwLibInternal) {

      vector<DString> subList;
      SizeT nPro = libProList.size();
      for (SizeT i = 0; i < nPro; ++i) {
        if (libProList[i]->GetHideHelp()) // difference here
          subList.push_back(libProList[i]->ToString());
      }
      sort(subList.begin(), subList.end());

      SizeT nProList = subList.size();
      cout << "NOTE: Internal subroutines are subject to change without notice." << endl;
      cout << "They should never be called directly from a GDL program." << endl;
      cout << "Internal library procedures (" << nProList << "):" << endl;
      for (SizeT i = 0; i < nProList; ++i)
        cout << subList[i] << endl;

      subList.clear();

      SizeT nFun = libFunList.size();
      for (SizeT i = 0; i < nFun; ++i) {
        if (libFunList[i]->GetHideHelp()) // difference here
          subList.push_back(libFunList[i]->ToString());
      }
      sort(subList.begin(), subList.end());

      SizeT nFunList = subList.size();
      cout << "Internal library functions (" << nFunList << "):" << endl;
      for (SizeT i = 0; i < nFunList; ++i) cout << subList[i] << endl;
      
      return;
    }

    static int MEMORYIx  = e->KeywordIx("MEMORY");
    bool isKWSetMemory = e->KeywordSet(MEMORYIx);
    static int RECALL_COMMANDSIx = e->KeywordIx("RECALL_COMMANDS");
    bool isKWSetRecall = e->KeywordSet(RECALL_COMMANDSIx);

    static int STRUCTURESIx = e->KeywordIx("STRUCTURES");
    bool isKWSetStructures = e->KeywordSet(STRUCTURESIx);
    if (isKWSetStructures) kw = true;
    
    if (isKWSetStructures && isKWSetMemory) isKWSetMemory=false; //just like that
    if (isKWSetStructures && isKWSetRecall) isKWSetRecall=false; //actually prints sysvars!
      
    if (isKWSetMemory && (isKWSetProcedures || isKWSetFunctions))
      e->Throw("Conflicting keywords.");

    if (isKWSetStructures && (isKWSetProcedures || isKWSetFunctions))
      e->Throw("Conflicting keywords.");

    if (isKWSetRecall && (isKWSetProcedures || isKWSetFunctions))
      e->Throw("Conflicting keywords.");

    static int PREFERENCESIx = e->KeywordIx("PREFERENCES");
    bool isKWSetPreferences = e->KeywordSet(PREFERENCESIx);
    if (isKWSetPreferences && (isKWSetProcedures || isKWSetFunctions))
      e->Throw("Conflicting keywords.");

    // using this way, we don't need to manage HAVE_READLINE at this level ...
    if (isKWSetRecall) {
      DStringGDL *previous_commands;
      previous_commands = recall_commands_internal();
      SizeT nEl2 = previous_commands->N_Elements();
      cout << "Recall buffer length: " << nEl2 << endl;
      for (SizeT i = 0; i < nEl2; ++i)
        ostr << i + 1 << "  " << (*previous_commands)[i] << endl;
      GDLDelete(previous_commands);
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout<<ostr.str();
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
      ostr << MemStats::GetNumFree() << endl;
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout<<ostr.str();
      return;
    }

    // Excluding keywords which are exclusive is not finished ...
    if (isKWSetPreferences) {
      ostr << "Preferences" <<endl;
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  cout<<ostr.str();
      return;
    }
    
    
    bool allOutputs=false;
    if (nParam==0 && !isKWSetProcedures && !isKWSetFunctions  && !routinesKW) {allOutputs=true; briefKW=true;} //see below

    // Compiled Procedures & Functions
    vector<DString> pList;
    vector<DString> fList;

    int npro = 0, nfun = 0;
    pList.push_back("$MAIN$");
    for (ProListT::iterator i = proList.begin(); i != proList.end(); ++i)
      if (fullKW || !((*i)->isHidden())) {
        pList.push_back((*i)->ObjectName());
        npro++;
      }
    sort(pList.begin(), pList.end());

    // Get list of user functions
    for (FunListT::iterator i = funList.begin(); i != funList.end(); ++i)
      if (fullKW || !((*i)->isHidden())) {
        fList.push_back((*i)->ObjectName());
        nfun++;
      }
    sort(fList.begin(), fList.end());


      // PROCEDURES keyword
    if (isKWSetProcedures || routinesKW || allOutputs) {
      ostr << "Compiled Procedures:" << endl;
      // Loop through procedures
      for (SizeT i = 0; i < pList.size(); i++) {
        // Add $MAIN$
        if (i == 0) ostr << "$MAIN$ ";
        bool do_output = true;

        // Find DPro pointer for pList[i]
        ProListT::iterator p = std::find_if(proList.begin(), proList.end(), Is_eq<DPro>(pList[i]));
        if (p != proList.end()) {
          DPro *pro = *p;

          if (isKWSetNames) {
            if (!CompareWithJokers(names, pro->ObjectName())) do_output = false;
          }

          if (do_output) {
            int nPar = pro->NPar();
            int nKey = pro->NKey();

            // Loop through parameters
            ostr << setw(briefKW?12:24) << left << pro->ObjectName() << " " << setw(0);
            if (!briefKW) {
              for (SizeT j = 0; j < nPar; j++)
                ostr << StrLowCase(pro->GetVarName(nKey + j)) << " ";
              for (SizeT j = 0; j < nKey; j++)
                ostr << StrUpCase(pro->GetVarName(j)) << " ";
            }
          }
        }
        if (do_output && !briefKW) ostr << endl;
      }
    }

    if (isKWSetFunctions || routinesKW || allOutputs ) {
        if (isKWSetProcedures || routinesKW || allOutputs ) ostr << endl;
        ostr << "Compiled Functions:" << endl;

      // Loop through functions
      for (SizeT i = 0; i < fList.size(); i++) {
        bool do_output = true;

        // Find DFun pointer for fList[i]
        FunListT::iterator p = std::find_if(funList.begin(), funList.end(), Is_eq<DFun>(fList[i]));
        if (p != funList.end()) {
          DFun *pro = *p;

          if (isKWSetNames) {
            if (!CompareWithJokers(names, pro->ObjectName())) do_output = false;
          }

          if (do_output) {
            int nPar = pro->NPar();
            int nKey = pro->NKey();

            // Loop through parameters
              ostr << setw(briefKW?12:24) << left << pro->ObjectName() << " " << setw(0);
            if (!briefKW) {
              for (SizeT j = 0; j < nPar; j++)
                ostr << StrLowCase(pro->GetVarName(nKey + j)) << " ";
              for (SizeT j = 0; j < nKey; j++)
                ostr << StrUpCase(pro->GetVarName(j)) << " ";
            }
          }
        }
        if (do_output && !briefKW) ostr << endl;
      }
    }
    if (briefKW) ostr <<endl;
    
    std::ostringstream routines_ostringstream;
    if ( isKWSetProcedures || isKWSetFunctions || routinesKW ) {
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,false); else  cout<<ostr.str();
      return;
    } else { //save ostr into end_of_ostr to add at end of below output if needed:
      routines_ostringstream << ostr.str();
      ostr.str("");
    }

    std::ostringstream parameters_ostringstream;
    static int levelIx = e->KeywordIx("LEVEL");
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
      
    DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum-1]->GetPro());

    SizeT nVar = pro->Size(); // # var in GDL for desired level 
    SizeT nComm = pro->CommonsSize(); // # has commons?
    SizeT nTotVar = nVar + nComm; //All the variables availables at that lev.

    if (nTotVar > 0) {
      set<string> helpStr;  // "Sorted List" 
      if (nVar > 0) {
        for (SizeT i = 0; i < nVar; ++i) {
          BaseGDL*& par = ((EnvT*) (callStack[desiredlevnum - 1]))->GetKW(i);
          if (par != NULL) {
            stringstream ss;
            string parName = pro->GetVarName(i);
            help_item(ss, par, parName, false);
            helpStr.insert(ss.str());
          }
        }
      }
      if (nComm > 0) {
        DStringGDL* list=static_cast<DStringGDL*>(pro->GetCommonVarNameList());
        for (SizeT i = 0; i < list->N_Elements(); ++i) {
          BaseGDL** par =  pro->GetCommonVarPtr((*list)[i]);
          DCommonBase* common = pro->FindCommon((*list)[i]);
          DString parName =  (*list)[i] + " (" + common->Name() + ')';
          stringstream ss;
          help_item(ss, (*par), parName, false);
          helpStr.insert(ss.str());
        }
      }
      copy(helpStr.begin(), helpStr.end(),ostream_iterator<string>(parameters_ostringstream));
    }
      
    if ( nParam == 0) {
      SimpleDumpStack(e, ostr);
      // list all variables at level 'lev'
      ostr << parameters_ostringstream.str();
      ostr << routines_ostringstream.str();
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,false); else  cout<<ostr.str();
      return;
    } else {
      for (SizeT i = 0; i < nParam; i++) {
        BaseGDL*& par = e->GetPar(i);
        DString parString = e->Caller()->GetString(par, true);
        // NON-STRUCTURES except if one and only one param is a Struct of 1 element. (and /struct was not given)
        if (!isKWSetStructures && nParam == 1 && par && par->Type() == GDL_STRUCT) {
            isKWSetStructures = (par->N_Elements() == 1);
        }

        if (!par || !isKWSetStructures || par->Type() != GDL_STRUCT) {
          // If no OUTPUT keyword send to stdout
          help_item(ostr, par, parString, false);
        } else {
          // STRUCTURES
          DStructGDL* s = static_cast<DStructGDL*> (par);
          SizeT nTags = s->Desc()->NTags();
          ostr << "** Structure ";
          ostr << (s->Desc()->IsUnnamed() ? "<Anonymous>" :
            s->Desc()->Name());
          ostr << ", " << nTags << " tags";
          ostr << ", length=" << s->Sizeof();
          ostr << ", data length=" << s->SizeofTags();
          ostr << ":" << endl;
          for (SizeT t = 0; t < nTags; ++t) {
            DString tagString = s->Desc()->TagName(t);
            help_item(ostr, s->GetTag(t), tagString, true);
          }
        }
      }
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,false); else  cout<<ostr.str();
    }
  }

  } // namespace
