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
#define INCLUDE_PYTHON 1
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

#else

// default

// note: TermHeight() not yet used

int TermWidth()
{
  return 80;
}

#endif

