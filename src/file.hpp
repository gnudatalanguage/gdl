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
#include "dinterpreter.hpp"
namespace lib {
  // library functions
  BaseGDL* file_test( EnvT* e);
  BaseGDL* file_lines( EnvT* e);

  BaseGDL* routine_dir_fun( EnvT* e);

  void cd_pro( EnvT* e);

  BaseGDL* expand_path( EnvT* e);

  BaseGDL* file_expand_path( EnvT* e);
  BaseGDL* file_search( EnvT* e);

  BaseGDL* file_same( EnvT* e);

  BaseGDL* file_info( EnvT* e);

  BaseGDL* path_sep( EnvT* e);
  BaseGDL* file_basename( EnvT* e);
  BaseGDL* file_dirname( EnvT* e);
  BaseGDL* file_readlink( EnvT* e);
  void file_copy( EnvT* e);
  void file_move( EnvT* e);
  void file_link( EnvT* e);
  
  // helper functions
  void ExpandPath( FileListT& result,
		   const DString& dirN, 
		   const DString& pat,
		   bool all_dirs = false);

  DString GetCWD(); // also used by gdljournal.cpp

  // SA:
  void file_mkdir( EnvT* e);
  void file_delete( EnvT* e);
  bool gdlarg_present(const char *s);

} // namespace

#endif

