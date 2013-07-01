/***************************************************************************
                                 file.hpp  -  file related library functions 
                             -------------------
    begin                : July 22 2004
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

#ifndef FILE_HPP_
#define FILE_HPP_

namespace lib {

  // library functions
  BaseGDL* file_test( EnvT* e);

  void cd_pro( EnvT* e);

  BaseGDL* expand_path( EnvT* e);

#ifndef _MSC_VER
  BaseGDL* file_search( EnvT* e);
#endif

  BaseGDL* file_same( EnvT* e);

  BaseGDL* file_info( EnvT* e);

  BaseGDL* file_basename( EnvT* e);
  BaseGDL* file_dirname( EnvT* e);

  // helper functions
  void ExpandPath( FileListT& result,
		   const DString& dirN, 
		   const DString& pat,
		   bool all_dirs = false);

  DString GetCWD(); // also used by gdljournal.cpp

  // SA:
  void file_mkdir( EnvT* e);

} // namespace

#endif

