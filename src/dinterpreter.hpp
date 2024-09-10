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

#define GDL_INPUT_TIMEOUT 167 //16667 //microseconds -> 60 per second

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <csignal>

#if defined(_WIN32) && !defined(__CYGWIN__)
extern void gdl_ipc_ClientSignalsOperationsOK();
extern void gdl_ipc_ClientSendReturn(unsigned char status, std::string s);
extern void gdl_ipc_ClientClosesMailBox();
#else
extern void gdl_ipc_ClientSignalsOperationsOK();
extern void gdl_ipc_ClientSendReturn(unsigned char status, std::string s);
extern void gdl_ipc_ClientClosesMailBox();
#endif 

#include <cfenv>

#include "GDLLexer.hpp"
#include "GDLParser.hpp"
#include "GDLTreeParser.hpp"
#include "GDLInterpreter.hpp"

#ifdef HAVE_LIBREADLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <fstream>
#include <vector>

// enable to print out an expression entered at the comand line
#define 	AUTO_PRINT_EXPR

void ControlCHandler(int);
void ChildControlCHandler(int);

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
     CC_RETURN,
     CC_ABORT
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
#if defined(HAVE_LIBREADLINE)
    // seems to cause valgrind to complain
    if (iAmMaster) clear_history(); // for testing of memory leaks (in GDL)
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

  bool IsInBatchProcedureAtMain(){return InBatchProcedureAtMain;}
};

#endif
