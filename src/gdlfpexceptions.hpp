/***************************************************************************
                          gdleventhandler.hpp  -  description
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
#ifndef GDLFPEXCEPTIONS_HPP_
#define GDLFPEXCEPTIONS_HPP_

void gdlReportFPExceptions();
//void GDLInitFPExceptionCatching();
void GDLStopRegisteringFPExceptions();
void GDLStartRegisteringFPExceptions();
void GDLCheckFPExceptionsAtLineLevel();
//Clled from innards
void GDLCheckFPExceptionsAtEndBlockLevel();

#endif
