/***************************************************************************
            gdleventhandler.hpp  -  global event handler routine 
                             -------------------
    begin                : February 23 2004
    copyright            : (C) 2004 by Marc Schellens
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

// later this should become a thread
// right now its called in DInterpreter::InterpreterLoop()

#include "gdleventhandler.hpp"
#include "graphics.hpp"

using namespace std;

int GDLEventHandler()
{
  Graphics::HandleEvents();

  return 0;
}

