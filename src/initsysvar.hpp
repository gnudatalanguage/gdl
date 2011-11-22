/***************************************************************************
                          initsysvar.hpp  -  system variable access
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

#ifndef INITSYSVAR_HPP_
#define INITSYSVAR_HPP_

#include "objects.hpp"
namespace SysVar
{
  //  extern unsigned int pathIx, dIx;

  // returns a StrArr with the path to search
  const StrArr& GDLPath();
  void SetGDLPath( const DString& newPath);

  // returns !P
  DStructGDL* P();
  DLongGDL*   GetPMulti();
  DLong       GetPFont();

  // returns ![XYZ]
  DStructGDL* X();
  DStructGDL* Y();
  DStructGDL* Z();

  // returns !DIR
  const DString& Dir();

  // updates !STIME
  void UpdateSTime();

  DStructGDL* Version();

  DStructGDL* Values();

  // return !Mouse
  DStructGDL* Mouse();

  DStructGDL* Error_State();
  DString MsgPrefix();

  void SetErr_String( const DString& eS);

  // set !C
  void SetC( DLong newC);

  // set !D
  void SetD( BaseGDL* newD);

  // returns !MAP
  DStructGDL* Map();

  // returns !CPU
  DStructGDL* Cpu();
//  void CPUChanged();

  // returns !D
  DStructGDL* D();

  // returns !WARN
  DStructGDL* Warn();

  const std::string& Prompt();
  int   Edit_Input();
  DLong Quiet();

  // get and set !JOURNAL (journal file LUN)
  DLong JournalLUN();
  void JournalLUN( DLong jLUN);

  // get and set USERSYM
  DStructGDL* USYM();
  
  // initialize the predefined system variables (!IDNAME)
  void InitSysVar();
}

#endif
