/***************************************************************************
                          gdl.cpp  -  main program
                             -------------------
    begin                : Wed Apr 18 16:58:14 JST 2001
    copyright            : (C) 2002-2004 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define VERSION "0.8"
#endif

#include <string>
#include <csignal>
#include <cstdlib>

//#include <fenv.h>

#include "str.hpp"
#include "dinterpreter.hpp"
#include "terminfo.hpp"
#include "sigfpehandler.hpp"

using namespace std;

static void StartupMessage()
{
  cout << "GDL - GNU Data Language, Version " << VERSION << endl;
}

void LibInit(); // defined in libinit.cpp

void AtExit()
{
  cerr << flush; cout << flush; clog << flush;
  // clean up everything
  // (for debugging memory leaks)
  ResetObjects();
}

int main(int argc, char *argv[])
{
  if( atexit( AtExit) != 0) cerr << "atexit registration failed." << endl;

  // ncurses blurs the output, initialize TermWidth here
  TermWidth();

  // initializations
  InitObjects();

  LibInit(); // init library functions
    
  // turn on all floating point exceptions
  //  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW );

  signal(SIGINT,ControlCHandler);
  signal(SIGFPE,SigFPEHandler);
    
  StartupMessage();

  DInterpreter interpreter;

  string gdlPath=GetEnvString("GDL_PATH");
  if( gdlPath == "") gdlPath=GetEnvString("IDL_PATH");
  if( gdlPath != "")
    {
      SysVar::SetGDLPath( gdlPath);
    }

  string startup=GetEnvString("GDL_STARTUP");
  if( startup == "") startup=GetEnvString("IDL_STARTUP");
  if( startup == "")
    {
      cout << "'GDL_STARTUP'/'IDL_STARTUP' environment "
	"variables both not set.\n"
	"No startup file read." << endl;
    }
  else
    {
      // if path not given, add users home
      if( !PathGiven(startup))
	{
	  string home=GetEnvString("HOME");
	  if( home != "") 
	    {
	      AppendIfNeeded(home,"/");
	      startup=home+startup;
	    }
	}
      
      ifstream in(startup.c_str());
      while( in.good())
	{
	  interpreter.ExecuteLine( &in);
	}
    }

  interpreter.InterpreterLoop();

  return 0;
}
