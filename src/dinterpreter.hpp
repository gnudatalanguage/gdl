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
#  include <fenv.h>
    //#  if defined(__FreeBSD__)
#    pragma STDC FENV_ACCESS ON
    //#  endif
}
#endif

#include "initsysvar.hpp"
#include "objects.hpp"
#include "GDLLexer.hpp"
#include "GDLParser.hpp"
#include "GDLTreeParser.hpp"
#include "GDLInterpreter.hpp"
#include "gdleventhandler.hpp"

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <fstream>

void ControlCHandler(int);

class DInterpreter: public GDLInterpreter
{
public:
   enum CommandCode {
     CC_OK=0,
     CC_CONTINUE,
     CC_RETURN
   };

private:

  char* NoReadline(const std::string&);

  // execute GDL command (.run, .step, ...)
  CommandCode ExecuteCommand(const std::string& command);
  CommandCode CmdCompile(const std::string& command);
  CommandCode CmdRun(const std::string& command);

  // execute OS shell command (interactive shell if command == "") 
  static void ExecuteShellCommand(const std::string& command);
  
  std::string GetLine(); // get one line of input, trims it
  
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
  CommandCode ExecuteLine( std::istream* in = NULL);

  // run a list of commands from 'in'. Used by python module. Returns success
  bool RunBatch( std::istream* in);
  // the main program for interactive mode
  GDLInterpreter::RetCode InterpreterLoop( const std::string& startup);  
  // called within InterpreterLoop()
  GDLInterpreter::RetCode InnerInterpreterLoop();  
};

#endif
