/***************************************************************************
                            sigfpehandler.cpp  -  handle floating exceptions
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

#include <csetjmp>

//#include "sigfpehandler.hpp"

sigjmp_buf sigFPEJmpBuf;

// this is only called from integer division by zero
void SigFPEHandler( int signo) 
{
  siglongjmp( sigFPEJmpBuf,-1);
} 
