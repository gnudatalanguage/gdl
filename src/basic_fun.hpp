/***************************************************************************
                          basic_fun.hpp  -  basic GDL library functions
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASIC_FUN_HPP_
#define BASIC_FUN_HPP_

namespace lib {

  // also used from basic_fun_jmg.cpp
  void arr( EnvT* e, dimension& dim, SizeT pOffs=0);

  BaseGDL* bytarr( EnvT* e);
  BaseGDL* intarr( EnvT* e);
  BaseGDL* uintarr( EnvT* e);
  BaseGDL* lonarr( EnvT* e);
  BaseGDL* ulonarr( EnvT* e);
  BaseGDL* lon64arr( EnvT* e);
  BaseGDL* ulon64arr( EnvT* e);
  BaseGDL* fltarr( EnvT* e);
  BaseGDL* dblarr( EnvT* e);
  BaseGDL* strarr( EnvT* e);
  BaseGDL* complexarr( EnvT* e);
  BaseGDL* dcomplexarr( EnvT* e);

  BaseGDL* ptrarr( EnvT* e);
  BaseGDL* objarr( EnvT* e);

  BaseGDL* ptr_valid( EnvT* e);
  BaseGDL* obj_valid( EnvT* e);

  BaseGDL* ptr_new( EnvT* e);
  BaseGDL* obj_new( EnvT* e);
  BaseGDL* call_function( EnvT* e);
  BaseGDL* call_method_function( EnvT* e);
  
  BaseGDL* bindgen( EnvT* e);
  BaseGDL* indgen( EnvT* e);
  BaseGDL* uindgen( EnvT* e);
  BaseGDL* sindgen( EnvT* e);
  BaseGDL* lindgen( EnvT* e);
  BaseGDL* ulindgen( EnvT* e);
  BaseGDL* l64indgen( EnvT* e);
  BaseGDL* ul64indgen( EnvT* e);
  BaseGDL* findgen( EnvT* e);
  BaseGDL* dindgen( EnvT* e);
  BaseGDL* cindgen( EnvT* e);
  BaseGDL* dcindgen( EnvT* e);

  BaseGDL* n_elements( EnvT* e);

  BaseGDL* execute( EnvT* e);

  BaseGDL* assoc( EnvT* e);
  
  BaseGDL* byte_fun( EnvT* e);
  BaseGDL* fix_fun( EnvT* e);
  BaseGDL* uint_fun( EnvT* e);
  BaseGDL* long_fun( EnvT* e);
  BaseGDL* ulong_fun( EnvT* e);
  BaseGDL* long64_fun( EnvT* e);
  BaseGDL* ulong64_fun( EnvT* e);
  BaseGDL* float_fun( EnvT* e);
  BaseGDL* double_fun( EnvT* e);
  BaseGDL* string_fun( EnvT* e);
  BaseGDL* complex_fun( EnvT* e);
  BaseGDL* dcomplex_fun( EnvT* e);

  BaseGDL* gdl_logical_and( EnvT* e);
  BaseGDL* gdl_logical_or( EnvT* e);
  BaseGDL* logical_true( EnvT* e);

  BaseGDL* replicate( EnvT* e);

  BaseGDL* strcompress( EnvT* e);
  BaseGDL* strlowcase( EnvT* e);
  BaseGDL* strupcase( EnvT* e);
  BaseGDL* strlen( EnvT* e);
  BaseGDL* strmid( EnvT* e);
  BaseGDL* strpos( EnvT* e);
  BaseGDL* strtrim( EnvT* e);

  BaseGDL* where( EnvT* e);
  BaseGDL* total( EnvT* e);

  BaseGDL* n_params( EnvT* e);
  BaseGDL* keyword_set( EnvT* e);

  BaseGDL* array_equal( EnvT* e);

  BaseGDL* min_fun( EnvT* e);
  BaseGDL* max_fun( EnvT* e);

  BaseGDL* transpose( EnvT* e);

  BaseGDL* strtok_fun( EnvT* e); // strsplit

  BaseGDL* sort_fun( EnvT* e);

  BaseGDL* shift_fun( EnvT* e);

  BaseGDL* arg_present( EnvT* e);

  BaseGDL* eof_fun( EnvT* e);

  BaseGDL* strjoin( EnvT* e);

  BaseGDL* convol( EnvT* e);

  BaseGDL* rebin_fun( EnvT* e);

  BaseGDL* obj_class( EnvT* e);
  BaseGDL* obj_isa( EnvT* e);

} // namespace

#endif
