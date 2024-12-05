/***************************************************************************
            gdlfpexceptions.cpp  -  global FP exception reporting
                             -------------------
    begin                : February 23 2004
    copyright            : (C) 2023 by Gilles Duvert
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

// modified by GD 2023
// the exception test should probably be 
// the system continously sets exception flags, independently of us, because of problems in 1) our code and, alas, 2) the libraries we call.
// so just testing the exceptions will report exceptions whatever the command.
// We can cure all of them, but new will reappear? -- Best to register only the exceptions we want to report only inside some parts of our code:
// - functions that can create such math errors: all divisions, mod, and sqrt() at the moment.

#include <iostream>
#include "gdlfpexceptions.hpp"
#include "typedefs.hpp"
#include "initsysvar.hpp"
#include <cfenv>
static fexcept_t gdlExceptFlags;

// reports exceptions that occured and clear gdlExceptFlags.
void gdlReportFPExceptions() {
    std::feclearexcept(FE_ALL_EXCEPT);
    if (gdlExceptFlags&FE_INVALID) std::cout << "% Program caused arithmetic error: Floating illegal operand" << std::endl;
    if (gdlExceptFlags&FE_DIVBYZERO) std::cout << "% Program caused arithmetic error: Floating divide by zero" << std::endl;
    if (gdlExceptFlags&FE_UNDERFLOW) std::cout << "% Program caused arithmetic error: Floating underflow" << std::endl;
    if (gdlExceptFlags&FE_OVERFLOW) std::cout << "% Program caused arithmetic error: Floating overflow" << std::endl;
	gdlExceptFlags=0; //cleared
}
void GDLRegisterADivByZeroException() {
// intentionnally inform of a divbyzero to gdl's current exception status
  gdlExceptFlags|=FE_DIVBYZERO;
}

//stop monitoring and save currently registered exceptions in gdlExceptFlags.
void GDLStopRegisteringFPExceptions() {
// copy existing fpe exceptions (if any) to gdl's current exception status
  if (std::fetestexcept(FE_INVALID)) gdlExceptFlags|=FE_INVALID;
  if (std::fetestexcept(FE_DIVBYZERO)) gdlExceptFlags|=FE_DIVBYZERO;
  if (std::fetestexcept(FE_UNDERFLOW)) gdlExceptFlags|=FE_UNDERFLOW;
  if (std::fetestexcept(FE_OVERFLOW)) gdlExceptFlags|=FE_OVERFLOW;
  std::feclearexcept(FE_ALL_EXCEPT);
// if !EXCEPT is 2, report immediately:
static DInt* except = static_cast<DInt*>(SysVar::Except()->DataAddr());
  if (*except == 2) gdlReportFPExceptions();
}
//start monitoring fp exceptions
void GDLStartRegisteringFPExceptions() {
  // just clear existing fpe exceptions
    std::feclearexcept(FE_ALL_EXCEPT);
}
//start monitoring fp exceptions and will automatically call  GDLStopRegisteringFPExceptions() when exiting
std::unique_ptr<ReportFPExceptionsGuard> GDLStartAutoStopRegisteringFPExceptions() {
  // just clear existing fpe exceptions
    std::feclearexcept(FE_ALL_EXCEPT);
	std::unique_ptr<ReportFPExceptionsGuard> p(new ReportFPExceptionsGuard()) ;
	std::unique_ptr<ReportFPExceptionsGuard> pp=std::move( p );
	return pp;
}
//normal !EXCEPT=1 reporting
void GDLCheckFPExceptionsAtLineLevel() {
static DInt* except = static_cast<DInt*>(SysVar::Except()->DataAddr());
  if (*except == 0) return;
  gdlReportFPExceptions();
}
//Not used --- there is no place in the Nodes to put this, marker for Blocks have disappeared in the final compiled code.
//this is why GDLStopRegisteringFPExceptions() tests id !EXCEPT==2 and reports then. Works approximately the same.
void GDLCheckFPExceptionsAtEndBlockLevel() {
static DInt* except = static_cast<DInt*>(SysVar::Except()->DataAddr());
  if (*except < 2 ) return;
  gdlReportFPExceptions();
}

