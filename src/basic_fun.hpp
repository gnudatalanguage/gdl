/***************************************************************************
                          basic_fun.hpp  -  basic GDL library functions
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

#ifndef BASIC_FUN_HPP_
#define BASIC_FUN_HPP_

namespace lib {

  // also used from basic_fun_jmg.cpp
  void arr( EnvT* e, dimension& dim, SizeT pOffs=0);
  SizeT HASH_count( DStructGDL* oStructGDL);
  SizeT LIST_count( DStructGDL* oStructGDL);

  BaseGDL* get_kbrd( EnvT* e);

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

  BaseGDL* heap_refcount( EnvT* e);

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

  BaseGDL* execute_fun( EnvT* e);

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
  BaseGDL* logical_true( BaseGDL* p0, bool isReference);//( EnvT* e);

  BaseGDL* replicate( EnvT* e);

  BaseGDL* strcompress( EnvT* e);
  BaseGDL* strlowcase( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* strupcase( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* strlen( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* strmid( EnvT* e);
  BaseGDL* strpos( EnvT* e);
  BaseGDL* strtrim( EnvT* e);

  BaseGDL* total_fun( EnvT* e);
  BaseGDL* product_fun( EnvT* e);

  BaseGDL* n_params( EnvT* e);
  BaseGDL* keyword_set( EnvT* e);

  bool array_equal_bool( BaseGDL* p0, BaseGDL* p1,
			bool notypeconv=false, bool not_equal=false,
			bool quiet=true);
  BaseGDL* array_equal( EnvT* e);

  BaseGDL* min_fun( EnvT* e);
  BaseGDL* max_fun( EnvT* e);

  BaseGDL* transpose( EnvT* e);

  BaseGDL* sort_fun( EnvT* e);

  BaseGDL* SlowReliableMedian( EnvT* e);
  BaseGDL* median( EnvT* e);

  BaseGDL* shift_fun( EnvT* e);

  BaseGDL* ishft_fun( EnvT* e);

  BaseGDL* arg_present( EnvT* e);

  BaseGDL* eof_fun( EnvT* e);

  BaseGDL* strjoin( EnvT* e);

  BaseGDL* rebin_fun( EnvT* e);

  BaseGDL* obj_class( EnvT* e);

  BaseGDL* obj_hasmethod( EnvT* e);

  BaseGDL* obj_isa( EnvT* e);

  BaseGDL* n_tags( EnvT* e);

  BaseGDL* bytscl( EnvT* e);

  BaseGDL* routine_info( EnvT* e);

  BaseGDL* temporary( EnvT* e);

  BaseGDL* memory(EnvT* e);

  // the following by Peter Messmer 
  // (messmer@users.sourceforge.net)
  BaseGDL* strtok_fun( EnvT* e); // strsplit
  BaseGDL* getenv_fun( EnvT* e);
  BaseGDL* tag_names_fun( EnvT* e);
  BaseGDL* stregex_fun( EnvT* e);

  BaseGDL* strcmp_fun( EnvT* e);

  BaseGDL* create_struct( EnvT* e);

  BaseGDL* rotate( EnvT* e);

  BaseGDL* routine_filepath( EnvT* e);

  // the following by Sylwester Arabas 
  // (slayoo@users.sourceforge.net)
  BaseGDL* reverse( EnvT* e);
  BaseGDL* parse_url( EnvT* e);
  BaseGDL* locale_get( EnvT* e);
  BaseGDL* get_login_info( EnvT* e);
  BaseGDL* command_line_args_fun(EnvT* e);
  BaseGDL* idl_base64(EnvT* e);
  BaseGDL* get_drive_list(EnvT* e);

  BaseGDL* scope_level( EnvT* e);
  BaseGDL* scope_traceback( EnvT* e);
  BaseGDL* scope_varfetch_value( EnvT* e); // regular library function
  BaseGDL** scope_varfetch_reference( EnvT* e); // special version for LEval()
  BaseGDL* mean_fun(EnvT* e); 
  BaseGDL* moment_fun(EnvT* e); 
} // namespace

#endif
