/***************************************************************************
                       dinterpreter.hpp  -  main class which controls it all
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
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

#include <fenv.h>

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
  
  DInterpreter(): GDLInterpreter()
  {
    
#ifdef HAVE_LIBREADLINE
    // initialize readline
    char rlName[] = "GDL";
    rl_readline_name = rlName;
    rl_event_hook = GDLEventHandler;
    stifle_history( 20);
#endif
    
    //    heap.push_back(NULL); // init heap index 0 (used as NULL ptr)
    //    objHeap.push_back(NULL); // init heap index 0 (used as NULL ptr)
    interruptEnable = true;
    objHeapIx=1; // map version (0 is NULL ptr)
    heapIx=1;    // map version (0 is NULL ptr)
    returnValue  = NULL;
    returnValueL = NULL;
    
    // setup main level environment
    DPro* mainPro=new DPro();        // $MAIN$  NOT inserted into proList
    EnvT* mainEnv=new EnvT(this, static_cast<RefDNode>(antlr::nullAST), mainPro);
    callStack.push_back(mainEnv);   // push main environment (necessary)
  }
  
  // execute one line of code
  CommandCode ExecuteLine( std::ifstream* in = NULL);

  // the main program for interactive mode
  GDLInterpreter::RetCode InterpreterLoop();  
};

#endif
