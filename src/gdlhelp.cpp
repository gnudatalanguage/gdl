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
  for( DLong lun=maxUserLun+1; lun <= fileUnits.size(); ++lun)
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
		++nOut;
		pos = found+1;
		}
	    ostr.str("");
//	    if( nOut  not_eq nlines and debug) cout << 
//			" help_Output: Error counting lines -" <<
//			" nOut: "<<nOut<<" OutputLines:"<<nlines<<endl;
	}

    // showing HELP, /path_cache
  void help_path_cached(ostream& ostr, SizeT &lines_count) {

    struct dirent *dp;
    const char *ProSuffix = ".pro";
    int ProSuffixLen = strlen(ProSuffix);

    string tmp_fname;
    size_t found;

    StrArr path = SysVar::GDLPath();

    ostr << "!PATH (no cache management --now-- in GDL, ";
    ostr << path.size() << " directories)" << endl;
    lines_count = 1;

    for (StrArr::iterator CurrentDir = path.begin(); CurrentDir != path.end(); ++CurrentDir) {
      //	  cout << "1>>" << (*CurrentDir).c_str() << "<<" <<endl;
      DIR* dirp = opendir((*CurrentDir).c_str());
      //cout << "2>>" << dirp << "<<" <<endl;
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
        ostr << *CurrentDir << " (" << NbProFilesInCurrentDir << " files)" << endl;
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

static void help_object( ostream& ostr, DString parString, bool verbose=true )
  {
	const string objectnotfound = ": Object not found";
	const string objectdefined = ": Object defined";
	StrUpCaseInplace( parString);
	DStructDesc* desc = FindInStructList(structList, parString);
	ostr.width(20);
	ostr << right << parString;
	if(desc == NULL) {
		ostr << objectnotfound << endl;
	} else {
		ostr << objectdefined  << endl;
		if(not verbose) return;
      DStructGDL* dumm = new DStructGDL( desc, dimension());
	Guard<DStructGDL> guard(dumm);
	  lib::help_struct(ostr, dumm , 0 , false);
	  FunListT& funlist = desc->FunList();
	  ProListT& prolist = desc->ProList();
	  help_ListMethods("", ostr, funlist, prolist);
	}
}
static void help_ListLib(DString names, ostream& ostr, bool internal=true)
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
				<< subList.size() <<"):" << endl;
	  for( SizeT i = 0; i<subList.size(); ++i)
			ostr << subList[ i] << endl;

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
				<< subList.size() <<"):" << endl;
	  for( SizeT i = 0; i<subList.size(); ++i)
	    ostr << subList[ i] << endl;
	  subList.clear();

  }

static void help_heap(EnvT* e, ostream& ostr, bool verbose=true)
  {
// GJ 16-04-10
	SizeT numPtr = e->Interpreter()->HeapSize();
	SizeT numObj = e->Interpreter()->ObjHeapSize();
	ostr << "Heap Variables:" << std::endl;
	ostr << "    # Pointer: " << numPtr	 << std::endl;
	ostr << "    # Object : " << numObj	 << std::endl<< std::endl;
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
		  lib::help_item( ostr, 
				  hV, DString( "<PtrHeapVar")+
				  i2s(p)+">",
				  false);
		}
	}
	if(nH > nDisp) ostr << nH << " pointers in total"<< endl;
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
      for (SizeT i = 0; i < history_length - 1; ++i)
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

  void help_item(ostream& ostr,
		    BaseGDL* par, DString parString,
		     bool doIndentation = false)
    {
      static volatile bool debug(false);
      if (debug and (par not_eq NULL)) {
		cout << par->Type() << " :"
		<< par->TypeStr() << " :"
//		<< &par->TypeStr() << ": "
		<< parString << endl;
    }

    if (doIndentation) ostr << "   ";

    // Name display
    ostr.width(16);
    ostr << left << parString;
    if (parString.length() >= 16) {
      ostr << " " << endl; // for cmsv compatible output (uses help,OUTPUT)
      ostr.width(doIndentation ? 19 : 16);
      ostr << "";
    }

    // Type display (we have two "null" : defined !null and undefined variables ...
    if (par == NULL) {
      ostr << "UNDEFINED = <Undefined>" << endl;
      return;
    }
    if (!par) {
      ostr << "UNDEFINED = !NULL" << endl;
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
	}
      else if( par->Dim( 0) == 0)
	{
      if (par->Type() == GDL_STRING) {
        ostr << par->TypeStr() << right;
        if (!doIndentation) ostr << "= ";
        doTypeString = false;

        // trim string larger than 45 characters
        DString dataString = (*static_cast<DStringGDL*> (par))[0];
        ostr << "'" << StrMid(dataString, 0, 45, 0) << "'";
        if (dataString.length() > 45) ostr << "...";
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
    }

    // Dimension display
    if (par->Dim(0) != 0) ostr << par->Dim();

    // End of line
    ostr << endl;
  }

void help_struct(ostream& ostr,  BaseGDL* par, int indent=0, bool debug=false)
  {
	   // STRUCTURES
	   DStructGDL* s = static_cast<DStructGDL*>( par);
	   SizeT nTags = s->Desc()->NTags();

		for(int i=0; i < indent; ++i) ostr <<"   ";
		ostr << "** Structure ";
		ostr << (s->Desc()->IsUnnamed() ? "<Anonymous>" : s->Desc()->Name());
		ostr << ", " << nTags << " tags";
		if(indent == 0) {
			ostr << ",memsize =" << s->Sizeof();
			ostr << ", data length=" << s->NBytesToTransfer()
//			<< "/" << s->RealBytes() ; GJ has this but only applied here.
			<< "/" << s->SizeofTags() ;
		}
		ostr << ":" << endl;

		for (SizeT t=0; t < nTags; ++t) {
			for(int i=0; i < indent; ++i) ostr <<"   ";
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
    // in order of priority
    bool kw = false;
    static int lastmKWIx = e->KeywordIx("LAST_MESSAGE");
    bool lastmKW = e->KeywordPresent(lastmKWIx);
	if( lastmKW) {
		help_lastmsg(e); return;}
	SizeT nParam=e->NParam();

    BaseGDL** outputKW = NULL;
    static int outputIx = e->KeywordIx("OUTPUT");
    bool doOutput = e->KeywordPresent( outputIx);
    
    if (doOutput) { // Setup output return variable
      outputKW = &e->GetKW(outputIx);
      GDLDelete((*outputKW));
    }
	static SizeT OutputLines;
	OutputLines = 0;

    std::ostringstream ostr;
// Use mostly ostrp* << from here on and then push onto outputKW if need be.
      std::ostream* ostrp = (doOutput) ? &ostr : &cout;

	static int helpIx = e->KeywordIx( "HELP");
	if( e->KeywordSet( helpIx)) {help_help(e); return;}

    static int allkeysIx=e->KeywordIx("ALL_KEYS");
    static int keysIx=e->KeywordIx("KEYS");
    if (e->KeywordSet(allkeysIx) || e->KeywordSet(keysIx)) // ALL_KEYS is an obsolete keyword
			{help_keys(ostr);
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout << ostr.str();
			 return;}

	static volatile bool debugKW(false);
    static int debugIx = e->KeywordIx( "DEBUG");     
    if(e->KeywordSet(debugIx)) {
		debugKW = !debugKW;
		cout << " Help debug option set/reset: "<< debugKW << endl;
		return;
		}

	static int pathIx = e->KeywordIx( "PATH_CACHE");
	if( e->KeywordSet( pathIx)) {   // exercising two methods
	    help_path_cached( ostr, OutputLines);
	    if (debugKW) {
	      cout << OutputLines << endl;
	      cout << "begin" << ostr.rdbuf()->str() << "end"<<endl;
		  }
		help_Output(outputKW, ostr, OutputLines, doOutput);
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
    // briefKw should be default usage with DLM HEAP_VARIABLES
	// also MESSAGES OBJECTS ROUTINES SOURCE_FILES STRUCTURES SYSTEM_VARIABLES
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
       if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  SortAndPrintStream(ostr);
		return;}

	static int namesIx = e->KeywordIx( "NAMES");
	bool isKWSetNames= e->KeywordPresent( namesIx);

	static int sysvarIx = e->KeywordIx( "SYSTEM_VARIABLES");
	if( e->KeywordSet( sysvarIx)) { 
		help_sysvar(*ostrp, briefKW);
       if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  SortAndPrintStream(ostr);
		 return;}
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


    if (sourceFilesKW) {
	  if (!isKWSetFunctions) {
	// AC 2018-01-09 : Duplicating the pro list to avoid messing up the order of "proList"
	// otherwise, a call to HELP,/source created in future calls
	// e.g. of crashing sequence : TEST_TV & HELP, /source & TEST_TV
	//
	ProListT proList_tmp;
	proList_tmp=proList;
        sort(proList_tmp.begin(), proList_tmp.end(), CompProName());
	    *ostrp << "Compiled Procedures:" << endl;
	    *ostrp << "$MAIN$" << endl;	  OutputLines += 2;
		for( SizeT i = 0; i < proList_tmp.size(); ++i)
		  {
		  if( proList_tmp[i]->isHidden()  and !fullKW  )  continue;
		  if (isKWSetNames and 
				!(CompareWithJokers(names,proList_tmp[i]->ObjectName()))) continue;
			*ostrp << setw(25) << left << proList_tmp[i]->ObjectName() << setw(0);
			*ostrp << proList_tmp[i]->GetFilename() << endl;
        }
      }

	  if (!isKWSetProcedures) {
	    if (!isKWSetFunctions) *ostrp << endl;
	// AC 2018-01-09 : Duplicating the fun list to avoid messing up the order of "funList"
	// see above in (do_pro).
	FunListT funList_tmp;
	funList_tmp=funList;
        sort(funList_tmp.begin(), funList_tmp.end(), CompFunName());
	    *ostrp << "Compiled Functions:" << endl; ++OutputLines;
		for( SizeT i = 0; i < funList_tmp.size(); ++i)
		  {
		  if( funList_tmp[i]->isHidden()  and !fullKW  )  continue;
		  if (isKWSetNames and 
				!(CompareWithJokers(names,funList_tmp[i]->ObjectName()))) continue;
			*ostrp << setw(25) << left << funList_tmp[i]->ObjectName() << setw(0);
			*ostrp << funList_tmp[i]->GetFilename() << endl;  ++OutputLines;
          }
            }
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  cout<<ostr.str();
	   return;
          }

// Compiled Procedures & Functions

	vector<DString> fList;
	static  volatile int npro = 0, nfun =0;
	for( FunListT::iterator i=funList.begin(); i != funList.end(); ++i)
		if( fullKW  or  !((*i)->isHidden() ) ) {
		  	  fList.push_back((*i)->ObjectName()); ++nfun; }
	sort( fList.begin(), fList.end());
	  if(debugKW) cout << " #functions=" << nfun; 

	vector<DString> pList;
	pList.push_back("$MAIN$");
	for( ProListT::iterator i=proList.begin(); i != proList.end(); ++i)
		if( fullKW  or  !((*i)->isHidden() ) ) {
		  	  pList.push_back((*i)->ObjectName()); ++npro; }
	sort( pList.begin(), pList.end());
	  if(debugKW) cout << " #procedures=" << npro;
	  if(debugKW) cout << " #env:" << e->Caller()->EnvSize() << endl;
	  
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
	if(e->KeywordPresent( filesIx)) {
		help_files( ostr, e); 
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr,true); else  cout<<ostr.str();
	   return;
	}

	static int infoIx = e->KeywordIx("INFO");
	if( e->KeywordSet( infoIx)){  kw = true; help_info(); }

	static int libIx = e->KeywordIx("LIB");
    // internal library functions
    static int INTERNAL_LIB_GDLIx=e->KeywordIx("INTERNAL_LIB_GDL");
    bool kwLibInternal = e->KeywordSet(INTERNAL_LIB_GDLIx);
    if (kwLibInternal) {
      cout << "NOTE: Internal subroutines are subject to change without notice." << endl;
      cout << "They should never be called directly from a GDL program." << endl;
    }
	if( e->KeywordSet( libIx)) { kw = true;  help_ListLib(names, *ostrp, kwLibInternal);	}

    static int STRUCTURESIx = e->KeywordIx("STRUCTURES");
    bool isKWSetStructures = e->KeywordSet(STRUCTURESIx);
    static int RECALL_COMMANDSIx = e->KeywordIx("RECALL_COMMANDS");
    bool isKWSetRecall = e->KeywordSet(RECALL_COMMANDSIx);
    static int MEMORYIx  = e->KeywordIx("MEMORY");
    bool isKWSetMemory = e->KeywordSet(MEMORYIx);
    static int PREFERENCESIx = e->KeywordIx("PREFERENCES");
	bool isKWSetPreferences  = e->KeywordSet( "PREFERENCES");
    if (isKWSetStructures) kw = true;
    
	if ( (isKWSetStructures or isKWSetRecall or isKWSetMemory or 
			isKWSetPreferences) and (isKWSetProcedures or isKWSetFunctions))
      e->Throw("Conflicting keywords.");

    if (isKWSetRecall) {
      DStringGDL *previous_commands;
      previous_commands = recall_commands_internal();
      SizeT nEl2 = previous_commands->N_Elements();
      cout << "Recall buffer length: " << nEl2 << endl;
      for (SizeT i = 0; i < nEl2; ++i) {
			if (isKWSetNames and 
				!CompareWithJokers(names, (*previous_commands)[i])) continue;
	       *ostrp << i+1 << "  " <<(*previous_commands)[i] << endl;
			}
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
	if (isKWSetPreferences)  {	
	  //cout << "ici 1 " << isKWSetPreferences << endl;
	  *ostrp << "Preferences: this is not ready ..."<<endl;
	  //*ostrp << GDL_GR_X_QSCREEN::GetValue();
	  if(doOutput) help_Output(outputKW, ostr, OutputLines);

	  return;
	  }
      // switch to dec output (might be changed from formatted output)

	if(!doOutput)			cout << dec;
    
	if ((nParam == 0 and !isKWSetMemory) or
			isKWSetFunctions or isKWSetProcedures) {
    
	    if (nParam == 0 and !isKWSetFunctions and
		    !isKWSetProcedures and !routinesKW) 
		    		SimpleDumpStack(e, *ostrp);	

	    if (isKWSetProcedures or routinesKW) {
			*ostrp << "Compiled Procedures:" << endl << "$MAIN$" << endl;
			OutputLines += 2;
			for( SizeT i=1; i<pList.size(); ++i) {

        // Find DPro pointer for pList[i]
		        ProListT::iterator p=std::find_if(proList.begin(),proList.end(),
					      Is_eq<DPro>(pList[i]));
		        if( p == proList.end()) continue;
          DPro *pro = *p;

		        if (isKWSetNames and 
				!CompareWithJokers(names, pro->ObjectName())) continue;
		        *ostrp << setw(25) << left << pro->ObjectName() << setw(0);
	   
		        int nPar = pro->NPar();
		        int nKey = pro->NKey();
// Loop through parameters and keywords
		        for( SizeT j=0; j<pro->NPar(); ++j)
				    *ostrp << StrLowCase(pro->GetVarName(nKey+j)) << " ";
				for( SizeT j=0; j<pro->NKey(); ++j)
					*ostrp << StrUpCase(pro->GetVarName(j)) << " ";
				*ostrp << endl; ++OutputLines;
				}
          }

	    if (isKWSetFunctions or routinesKW) {	
		    *ostrp << "Compiled Functions:" << endl;
			++OutputLines;
	  // Loop through functions
			for( SizeT i=0; i<fList.size(); ++i) {	  
	    // Find DFun pointer for fList[i]
				FunListT::iterator p=std::find_if(funList.begin(),funList.end(),
					      Is_eq<DFun>(fList[i]));
				if( p == funList.end()) continue;
				DFun *pro = *p;
				if (isKWSetNames and
					!CompareWithJokers(names, pro->ObjectName())) continue;
            int nPar = pro->NPar();
            int nKey = pro->NKey();
		    *ostrp << setw(25) << left << pro->ObjectName() << setw(0);

              for (SizeT j = 0; j < nPar; ++j)
		      *ostrp << StrLowCase(pro->GetVarName(nKey+j)) << " ";
              for (SizeT j = 0; j < nKey; ++j)
		      *ostrp << StrUpCase(pro->GetVarName(j)) << " ";
		    *ostrp << endl; OutputLines++;
            }
          }
	    if( isKWSetProcedures or isKWSetFunctions)  {
		if(doOutput) help_Output(outputKW, ostr, OutputLines);
		return;
      }
    }

      // Excluding keywords which are exclusive is not finished ...

	if (isKWSetPreferences)  {	
	  //cout << "ici 1 " << isKWSetPreferences << endl;
	  *ostrp << "Preferences: this is not ready ..."<<endl;
	  //*ostrp << GDL_GR_X_QSCREEN::GetValue();
	  if(doOutput) help_Output(outputKW, ostr, OutputLines);

	  return;
	  }
    static int commonIx = e->KeywordIx( "COMMON");
  	if(e->KeywordSet( commonIx)) {  // list in internal order
		CommonListT::iterator it;
		for( it=commonList.begin(); it != commonList.end(); ++it)
		{
		    SizeT nVar = (*it)->NVar(); if(nVar == 0) continue;
		    *ostrp << " Common block (" << (*it)->Name() << 
			") listed in internal order:" <<endl;
		    for( SizeT vIx = 0; vIx < nVar; ++vIx)	{
			    DString parString = (*it)->VarName( vIx) +" ("+(*it)->Name()+')';
			    DVar* var = (*it)->Var( vIx);
			    if( var->Data() not_eq NULL || fullKW)  
				help_item( *ostrp , var->Data(), parString, false);
			}
		    }
     if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout << ostr.str();
		if(nParam == 0) return;
 	    }

	if(debugKW) std::cout << " help_pro: nParam=" << nParam ;
	const string objectnotfound = ": Object not found";
	bool objectsKW = e->KeywordSet(objectsIx);
	for( SizeT i=0; i<nParam; ++i)  {
	  BaseGDL*& par=e->GetPar( i);
		DString parString = e->Caller()->GetString( par, true);//= string(" ??? ");
	if(debugKW) std::cout << ". " ;
 	  if(par == NULL) {
			if(debugKW) std::cout << " par ==(NULL)" << endl;
		  if( objectsKW) {
			help_object( ostr, parString, !briefKW);
				 continue;
			  }
			if(!briefKW) help_item( ostr, par, parString, false);
			continue;
          }
		if(objectsKW && par->Type() != GDL_OBJ) {
			if(par->Type() == GDL_STRING) {
				for( SizeT kobj=0; kobj < par->N_Elements(); ++kobj) 
					help_object( ostr,
						(*static_cast<DStringGDL*>(par))[kobj], !briefKW);
				continue;}
			else {
				cout << parString+
				":error (help,prm,/OBJECTS - prm is not object nor string) " << endl;
				continue;
			;}
            }
	 help_item( ostr, par, parString, false);
// NON-STRUCTURES except if one and only one param is a Struct.
	  if(par->Type() == GDL_STRUCT and (nParam == 1 or isKWSetStructures )
			and !briefKW)
			help_struct(ostr, par, 0, debugKW);
	  else if ( nParam != 1) continue;
	  else if (par->Type() == GDL_OBJ and par->StrictScalar())
	  {
		  bool isKWSetMethods = e->KeywordSet( "METHODS");
		  DObj s = (*static_cast<DObjGDL*>(par))[0];
		  DStructGDL* oStructGDL;
		  if(s != 0) oStructGDL =  GDLInterpreter::GetObjHeapNoThrow( s);
		  if( s!= 0  and (isKWSetStructures or fullKW))
			  help_struct(ostr, oStructGDL, 0, debugKW);
		  if( s!= 0  and (isKWSetMethods or fullKW)){
		      DStructDesc* desc = oStructGDL->Desc();
		      FunListT& funlist = desc->FunList();
		      ProListT& prolist = desc->ProList();
		      help_ListMethods(names, ostr, funlist, prolist);
          }
        }
		else if (par->Type() == GDL_OBJ) 
		{
			SizeT nObj = par->N_Elements();
			if(debugKW) std::cout << "nObj="<<nObj;
			for( SizeT iobj=0; iobj<nObj; ++iobj)
			  {
				if(debugKW) std::cout << "-";
				DObj s = (*static_cast<DObjGDL*>( par))[iobj];
//				object_help( ostr, s, false);
		  DStructGDL* oStructGDL;
		  DString classname;
		  DString nameobj="  ["+i2s(iobj)+"] ";
		  if(s != 0) {
			  // Name display
				if(debugKW) std::cout << "-";
				std::vector< std::string> pNames;
				ostr.width(16);
				ostr << left << nameobj;
				oStructGDL =  GDLInterpreter::GetObjHeapNoThrow( s);
				if(oStructGDL == NULL) continue;
				if(debugKW) std::cout << ".";
				classname  = oStructGDL->Desc()->Name() ;
				oStructGDL->Desc()->GetParentNames(pNames);
				ostr.width(10);
				ostr << classname << right;
				ostr << left << "<ID=" << i2s(s) << "> " << right;
				DString parents;
				if(pNames.size() == 0) parents = "";
				else if( pNames.size() == 1) parents = "  parent:";
				else  						 parents = " parents:";
				ostr << left << parents;
				for (SizeT i=0; i < pNames.size(); ++i)  ostr << " " << pNames[i];
				ostr << endl;
			  }  else 
					help_item( ostr,  NULL, nameobj, false);  
      }
    }
    
	}
	if(debugKW) std::cout << " :=:" << std::endl;
	if(nParam > 0) {
      if (doOutput)  (*outputKW)=StreamToGDLString(ostr); else  cout<<ostr.str();
      return;
    }
 

    static int levelIx = e->KeywordIx("LEVEL");
	if(e->KeywordPresent(levelIx) ) {
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
		  
		DSubUD* pro = static_cast<DSubUD*> (callStack[desiredlevnum-1]->GetPro());

		SizeT nVar = pro->Size(); // # var in GDL for desired level 
		SizeT nComm = pro->CommonsSize(); // # has commons?
		SizeT nTotVar = nVar + nComm; //All the variables availables at that lev.
		if(debugKW)	cout << " Level section: nTotVar=" << nTotVar << endl;
		if (nTotVar > 0) {
		  set<string> helpStr;  // "Sorted List" 
		  if (nVar > 0) {
			for (SizeT i = 0; i < nVar; ++i) {
			  BaseGDL*& par = (static_cast<EnvUDT*> (callStack[desiredlevnum - 1]))->GetKW(i);
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
	}
//					 if /brief and /routines then no var. to be shown
	if( routinesKW and briefKW) kw = true;

	if( nParam == 0  and !kw)
	    {
	    routinesKW = true;
	    briefKW = true;

	  // list all variables of caller
	    EnvBaseT* caller = e->Caller();
      
	    SizeT nEnv = caller->EnvSize();

	  //cout << "EnvSize() " << nEnv << endl;

      set<string> helpStr;  // "Sorted List" 
            stringstream ss;

	    for ( int i = 0; i < nEnv; ++i )  {
		    BaseGDL*& par=caller->GetKW( i);
		    
		if(par == NULL && !fullKW) continue;
	      // if( par == NULL) continue;

		    DString parString = caller->GetString( par,true);
		if(debugKW ) cout << " GetKW(i)->GetString(par) "<<parString ;
		    if (isKWSetNames and
				!CompareWithJokers(names, parString)) continue;
	 	help_item( ss, par, parString, false);	    
            helpStr.insert(ss.str());
		ss.str("");
          }
		if (debugKW) cout << endl;
	  std::vector<DCommonBase*> cptr;
	  DSubUD* proUD = dynamic_cast<DSubUD*>(caller->GetPro());
	  proUD->commonPtrs(cptr);
	  std::vector<DCommonBase*>::iterator ic;
	  for( ic=cptr.begin(); ic != cptr.end(); ++ic)
		{
		    SizeT nVar = (*ic)->NVar();
		    for( SizeT vIx = 0; vIx < nVar; ++vIx)	{
				DString parString = (*ic)->VarName( vIx) +" ("+(*ic)->Name()+')';
				if (isKWSetNames and
					!CompareWithJokers(names, parString)) continue;
				DVar* var = (*ic)->Var( vIx);
				if( (var->Data() not_eq NULL) || fullKW)  {
					help_item( ss, var->Data(), parString, false);	    
          helpStr.insert(ss.str());
					ss.str("");
        }
      }
    }
      
	    copy( helpStr.begin(), helpStr.end(),
			ostream_iterator<string>( *ostrp) );
		OutputLines += helpStr.size();

	} // if( nParam == 0  and !kw)
	if( routinesKW and briefKW) {
		// Display compiled procedures & functions
	    if (!isKWSetProcedures and !isKWSetFunctions) {

		*ostrp << "Compiled Procedures:" << endl;
		for( SizeT i=0; i<pList.size(); ++i) *ostrp << pList[i] << " ";
		*ostrp << endl << endl; 
		*ostrp << "Compiled Functions:" << endl;
		for( SizeT i=0; i<fList.size(); ++i) *ostrp << fList[i] << " ";
		*ostrp << endl;  
		OutputLines += 4;
        }
      }
	if(doOutput) help_Output(outputKW, ostr, OutputLines);
	return;
  }

  } // namespace
