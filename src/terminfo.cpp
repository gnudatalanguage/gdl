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

#include "terminfo.hpp"

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

#include <readline/readline.h>
#include <readline/history.h>

int TermWidth()
{
  int cols;
  int rows;

  rl_get_screen_size(&rows, &cols);
  return cols;
}

int TermHeight()
{

  //  std::cout << "hello" << std::endl;
  int cols;
  int rows;
  rl_reset_screen_size ();
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

  // original line follows:
  // initscr();

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

#include <readline/readline.h>
#include <readline/history.h>

void SetTermSize(int rows, int cols)
{
  //  std::cout << "hello" << std::endl;
  rl_set_screen_size (rows, cols);
#if defined(HAVE_READLINE) && defined(RL_ISSTATE) && defined(RL_INITIALIZED)
  if (RL_ISSTATE(RL_INITIALIZED)) {
    rl_resize_terminal();
  }else {  std::cout << "Please report" << std::endl;
  }
#else
std::cout << "Not ready for no Readline libs." << std::endl;
#endif
}
#endif
namespace lib {
    using namespace std;
	
  BaseGDL* get_kbrd( EnvT* e)
  {
      
    SizeT nParam=e->NParam();

    bool doWait = true;
    if( nParam > 0)      {
		doWait = false;
		DLong waitArg = 0;
		e->AssureLongScalarPar( 0, waitArg);
		if( waitArg != 0)
			doWait = true;
      }
    
    char c='\0'; 

    if (doWait)
      {    cin.get(c);      }
    else 
      {    c=std::fgetc(stdin);
		      if(c==EOF) c='\0';
      }

    DStringGDL* res = new DStringGDL( DString( i2s( c))); 

    return res;
 
  }

  
 /*   BaseGDL* terminal_size_fun( EnvT* e ) {
        // TODO: Also allow setting width/height
        SizeT nParam = e->NParam(0);
        DLongGDL* ret = new DLongGDL( dimension(2) );
        (*ret)[0] = TermWidth();
        (*ret)[1] = TermHeight();
        return ret;
    }*/
 
   // get or change Terminal Size   
  BaseGDL* terminal_size_fun( EnvT* e ) {

    SizeT nParam = e->NParam();
    cout << nParam << endl;

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

#if defined(HAVE_LIBREADLINE)
    SetTermSize(nb_lines, nb_cols);
#else 
    Message("Setting Terminal Size not ready (OK only with Readline)");
#endif
    // reading again the new size 
    DLongGDL* ret = new DLongGDL( dimension(2) );
    (*ret)[0] = TermWidth();
    (*ret)[1] = TermHeight();
    return ret;
  }
} // namespace
