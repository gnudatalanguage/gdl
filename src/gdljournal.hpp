/***************************************************************************
                          gdljournal.cpp  -  exception handling
                             -------------------
    begin                : September 26 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef GDL_JOURNAL_H
#define GDL_JOURNAL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "datatypes.hpp"
#include "envt.hpp"
#include "io.hpp"

namespace lib
{
	
  extern const char*  JOURNALCOMMENT;

  void journal(EnvT *e);
  void write_journal(std::string str);
  void write_journal_comment(std::string str);
  void write_journal_comment(EnvT *e, int offset, SizeT width);
  GDLStream* get_journal();
  
}
#endif

