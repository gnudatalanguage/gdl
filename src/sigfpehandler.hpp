/***************************************************************************
                            sigfpehandler.hpp  -  handle floating exceptions
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

#ifndef SIGFPEHANDLER_HPP_
#define SIGFPEHANDLER_HPP_

#include <csetjmp>

extern sigjmp_buf sigFPEJmpBuf;

void SigFPEHandler( int signo); 

#endif
