/***************************************************************************
                       dinterpreter.hpp  -  main class which controls it all
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DINTERPRETER_HPP_
#define DINTERPRETER_HPP_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <csignal>

#ifdef __cplusplus
  extern "C" {
#if defined(__OpenBSD__)
// SA: based on http://ftp2.cz.freesbie.org/pub/FreeBSD-cvs/gnats/i386/75862
#  include <ieeefp.h>
#  define feclearexcept(e)	(void)fpsetsticky(~((fp_except)(e)))
#  define fetestexcept(e)	((int)(fpgetsticky() & (fp_except)(e)))
#  define FE_ALL_EXCEPT 	(FP_X_INV | FP_X_DNML | FP_X_DZ | FP_X_OFL | FP_X_UFL | FP_X_IMP)
#  define FE_DIVBYZERO		FP_X_DZ
#  define FE_INEXACT		FP_X_IMP
#  define FE_INVALID		FP_X_INV
#  define FE_OVERFLOW		FP_X_OFL
#  define FE_UNDERFLOW		FP_X_UFL
#elif defined(_MSC_VER) && _MSC_VER < 1800
#  include <float.h>
#  pragma fenv_access(on)
#else
#  ifdef __MINGW32__ // hack for MINGW
#    define FE_INVALID		0x01
#    define FE_DENORMAL		0x02
#    define FE_DIVBYZERO	0x04
#    define FE_OVERFLOW		0x08
#    define FE_UNDERFLOW	0x10
#    define FE_INEXACT		0x20
#    define FE_ALL_EXCEPT (FE_INVALID | FE_DENORMAL | FE_DIVBYZERO \
		           | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)
extern int __cdecl __MINGW_NOTHROW feclearexcept (int);	  	
extern int __cdecl __MINGW_NOTHROW fetestexcept (int excepts);
#  endif
#  include <fenv.h>
#endif
#  if defined(__FreeBSD__)
#    pragma STDC FENV_ACCESS ON
#  endif
}
#endif

//#include "initsysvar.hpp"
//#include "objects.hpp"
#include "GDLLexer.hpp"
#include "GDLParser.hpp"
#include "GDLTreeParser.hpp"
#include "GDLInterpreter.hpp"
//#include "gdleventhandler.hpp"

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <fstream>
#include <vector>


void ControlCHandler(int);

extern bool lineEdit; // = false;
extern bool historyIntialized; 
extern std::string actualPrompt;

class DInterpreter: public GDLInterpreter
{
public:
   enum CommandCode {
     CC_OK=0,
     CC_CONTINUE,
     CC_STEP,
     CC_SKIP,
     CC_RETURN
   };

  char* NoReadline(const std::string&);

private:

  // execute GDL command (.run, .step, ...)
  CommandCode ExecuteCommand(const std::string& command);
  CommandCode CmdCompile(const std::string& command);
  CommandCode CmdRun(const std::string& command);
  CommandCode CmdReset();
  CommandCode CmdFullReset();

  // execute OS shell command (interactive shell if command == "") 
  static void ExecuteShellCommand(const std::string& command);
  
  std::string GetLine(); // get one line of input, trims it
  
  void RunDelTree();

public:
  ~DInterpreter() 
  {
#ifdef HAVE_LIBREADLINE
    // seems to cause valgrind to complain
    clear_history(); // for testing of memory leaks (in GDL)
#endif
  }
  
  // this is executed at the beginning (gdl.cpp)
  DInterpreter();
  
  // execute one line of code
  CommandCode ExecuteLine( std::istream* in = NULL, SizeT lineOffset = 0);

  // execute a whole file (used by @ and for batch files specified as arguments to gdl)
  void        ExecuteFile( const std::string& file);  

  // run a list of commands from 'in'. Used by python module. Returns success
  bool RunBatch( std::istream* in);

  // the main program for interactive mode
  RetCode InterpreterLoop( const std::string& startup,
    std::vector<std::string>& batch_files, const std::string& statement);  

  // called within InterpreterLoop()
  RetCode InnerInterpreterLoop(SizeT lineOffset);

};

#endif
