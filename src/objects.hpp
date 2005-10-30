/***************************************************************************
                          objects.hpp  -  global structures
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

#ifndef OBJECTS_HPP_
#define OBJECTS_HPP_

//#include<deque>
#include<string>

#include "datatypes.hpp"

#include "dvar.hpp"
#include "dpro.hpp"
#include "dcommon.hpp"
#include "envt.hpp"
#include "io.hpp"

#ifdef USE_PYTHON
#include "gdlpython.hpp"
#endif

extern VarListT      sysVarList;
extern VarListT      sysVarRdOnlyList;

extern FunListT      funList;
extern ProListT      proList;
extern LibFunListT   libFunList;
extern LibProListT   libProList;

extern CommonListT   commonList;   // common blocks
extern StructListT   structList;

extern GDLFileListT  fileUnits;

//extern DeviceListT   deviceList;
//extern Graphics*     actDevice;

// signals if control-c was pressed
extern volatile bool sigControlC;
extern          int  debugMode;

enum DebugCode {
  DEBUG_CLEAR=0,
  DEBUG_STOP,
  DEBUG_PROCESS_STOP
};  

template< class Container> void Purge( Container& s) 
{
  typename Container::iterator i;
  for(i = s.begin(); i != s.end(); ++i) 
    { delete *i; *i = NULL;}
}

void InitObjects();
void ResetObjects();

DLong GetLUN();

int ProIx(const std::string& n);
int FunIx(const std::string& n);

int LibProIx(const std::string& n);
int LibFunIx(const std::string& n);

bool IsFun(antlr::RefToken); // used by Lexer and Parser

bool BigEndian();

#endif
