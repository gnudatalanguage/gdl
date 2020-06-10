/***************************************************************************
                         terminfo.cpp  -  get information about the terminal
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
/*
  #ifdef HAVE_CONFIG_H
  #include <config.h>
  #endif
*/

#include "includefirst.hpp"
#include "stdio.h"
#include <iostream>

#include "terminfo.hpp"

#ifndef _WIN32
#include <termios.h> 
#include <unistd.h> 
#endif

#if defined(HAVE_LIBREADLINE)
#include <readline/readline.h>
#endif

/* AC 2020 mai : it is useful ?!
// used to defined GDL_TMPDIR: may have trouble on MSwin, help welcome
#ifndef _WIN32
#include <paths.h>
#endif
*/

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h>

int TermWidth()
{
  HANDLE consh = GetStdHandle(STD_OUTPUT_HANDLE);
  
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  GetConsoleScreenBufferInfo(consh, &bufinfo);
  return bufinfo.srWindow.Right + 1;
}
int TermHeight()
{
  HANDLE consh = GetStdHandle(STD_OUTPUT_HANDLE);

  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  GetConsoleScreenBufferInfo(consh, &bufinfo);
  return bufinfo.srWindow.Bottom + 1;
}

#elif defined(HAVE_LIBREADLINE) && defined(RL_GET_SCREEN_SIZE)

int TermWidth()
{
  int cols;
  int rows;
  rl_get_screen_size(&rows, &cols);
  return cols;
}

int TermHeight()
{
  int cols;
  int rows;
  rl_get_screen_size(&rows, &cols);
  return rows;
}

#elif defined(HAVE_LIBNCURSES) || defined(HAVE_LIBCURSES)

#ifdef HAVE_LIBNCURSES 
#include <ncurses.h>
#elif defined(HAVE_LIBCURSES) 
#include <curses.h>
#endif

int TermWidth()
{
  int cols = 0;
  SCREEN *screen;

  if( cols != 0) return cols;

  screen = newterm((char *) NULL, stdout, stdin);
  if((void *)screen == NULL)
    cols = 80;
  else
    cols = COLS;

  endwin();

  return cols;
}

int TermHeight()
{
  int lines = 0;
  SCREEN *screen;

  if( lines != 0) return lines;

  // original line follows:
  // initscr();

  screen = newterm((char *) NULL, stdout, stdin);
  if((void *)screen == NULL)
    lines = 24;
  else
    lines = LINES;

  endwin();

  return lines;
}

#else

// default

int TermWidth()
{
  return 80;
}

int TermHeight()
{
  return 24;
}

#endif

// AC 2020-05-05 : <<found on the Internet>> Unclear for me :((
#if defined(HAVE_LIBREADLINE) && defined(RL_GET_SCREEN_SIZE)

void SetTermSize(int rows, int cols)
{
  rl_set_screen_size (rows, cols);
#if defined(RL_ISSTATE) && defined(RL_INITIALIZED)
  if (RL_ISSTATE(RL_INITIALIZED)) {
    rl_resize_terminal();
  } else {
    std::cout << "Please report" << std::endl;
  }
#else
  std::cout << "Not ready due to RL_ISSTATE/RL_INITIALIZED (please report)" << std::endl;
#endif
}

#endif

namespace lib {
  using namespace std;

#ifdef _WIN32
  BaseGDL* get_kbrd( EnvT* e)
  {
      
    SizeT nParam=e->NParam();

    bool doWait = true;
    if( nParam > 0) {
      doWait = false;
      DLong waitArg = 0;
      e->AssureLongScalarPar( 0, waitArg);
      if( waitArg != 0)
	doWait = true;
    }
    
    char c='\0'; 
    
    if (doWait) {
      cin.get(c);
    }
    else {
      c=std::fgetc(stdin);
      if(c==EOF) c='\0';
    }

    DStringGDL* res = new DStringGDL( DString( i2s( c)));
    return res;
  }

#else
  // get_kbrd patch
  // http://sourceforge.net/forum/forum.php?thread_id=3292183&forum_id=338691
  BaseGDL* get_kbrd( EnvT* e)
  {
#if defined(HAVE_LIBREADLINE)
    rl_prep_terminal (0);
#endif
    
    SizeT nParam=e->NParam();
  
    bool doWait = true;
    if (nParam > 0)
      {
	doWait = false;
	DLong waitArg = 0;
	e->AssureLongScalarPar( 0, waitArg);
	if (waitArg != 0)
	  {
	    doWait = true;
	  }
      }

    // https://sourceforge.net/forum/forum.php?thread_id=3292183&forum_id=338691
    // DONE: Implement proper SCALAR parameter handling (doWait variable)
    // which is/was not blocking in the original program. 
    // note: multi-byte input is not supported here.
    
    char c='\0'; //initialize is never a bad idea...

    int fd=fileno(stdin);

    struct termios orig, get; 

    // Get terminal setup to revert to it at end. 

    (void)tcgetattr(fd, &orig); 
    // New terminal setup, non-canonical.
    get.c_lflag = ISIG; 

    if (doWait)
      {
	// will wait for a character
	get.c_cc[VTIME]=0;
	get.c_cc[VMIN]=1;
	(void)tcsetattr(fd, TCSANOW, &get); 

	cin.get(c);
      }
    else 
      {
	// will not wait, but return EOF or next character in terminal buffer if present

	get.c_cc[VTIME]=0;
	get.c_cc[VMIN]=0;
	(void)tcsetattr(fd, TCSANOW, &get); 

	//the trick is *not to use C++ functions here. cin.get would wait.*
	c=std::fgetc(stdin);
	//and to convert EOF to null (otherwise GDL may exit if not compiled with
	//[lib][n]curses)
	if(c==EOF) c='\0';
      }
    
    // Restore original terminal settings. 

    (void)tcsetattr(fd, TCSANOW, &orig); 

#if defined(HAVE_LIBREADLINE)
    rl_deprep_terminal ();
#endif

    DStringGDL* res = new DStringGDL( DString( i2s( c))); 
    return res;
  }
#endif
  
  BaseGDL* terminal_size_fun( EnvT* e ) {

    SizeT nParam = e->NParam();
    //    cout << nParam << endl;

    // Just returning the size of the Terminal
    if (nParam == 0) {
      DLongGDL* ret = new DLongGDL(dimension(2));
      (*ret)[0] = TermWidth();
      (*ret)[1] = TermHeight();
      return ret;
    }

    DLong nb_lines = -1, nb_cols = -1;

    if (nParam == 1) {
      e->AssureLongScalarPar( 0, nb_cols);
    }
    if (nParam == 2) {
      e->AssureLongScalarPar( 0, nb_cols);
      e->AssureLongScalarPar( 1, nb_lines);
    }
    if (nb_lines <= 0) nb_lines = TermHeight();
    if (nb_cols <= 0) nb_cols = TermWidth();

    //    cout << nb_lines << " "<< nb_cols << endl;

#if defined(HAVE_LIBREADLINE) && defined(RL_GET_SCREEN_SIZE)
    SetTermSize(nb_lines, nb_cols);
#else 
    Message("Setting Terminal Size not ready (OK only with recent Readline (5.2+))");
#endif

    // reading again the new size 
    DLongGDL* ret = new DLongGDL( dimension(2) );
    (*ret)[0] = TermWidth();
    (*ret)[1] = TermHeight();
    return ret;
  }

} // namespace

