/***************************************************************************
                          terminfo.hpp  -  get the width of the terminal
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

#include "includefirst.hpp"

#ifndef TERMINFO_HPP_
#define TERMINFO_HPP_
#include "envt.hpp"

#if defined(HAVE_LIBREADLINE)
void SetTermSize(int rows, int cols);
#endif

#include "envt.hpp"

#if defined(HAVE_LIBREADLINE)
void SetTermSize(int rows, int cols);
#endif

int TermWidth();
int TermHeight();
namespace lib {
  using namespace std;

  BaseGDL* terminal_size_fun( EnvT* e );

  BaseGDL* get_kbrd( EnvT* e);
}

#endif
