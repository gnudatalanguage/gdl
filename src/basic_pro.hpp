/***************************************************************************
                          basic_pro.hpp  -  basic GDL library procedures
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

#ifndef BASIC_PRO_HPP_
#define BASIC_PRO_HPP_

namespace lib {


	// control !CPU settings
	void cpu(EnvT* e);

	// helper function
	bool check_lun(EnvT* e, DLong lun);

	void ptr_free(EnvT* e);
	void obj_destroy(EnvT* e);
	void call_procedure(EnvT* e);
	void call_method_procedure(EnvT* e);

	void get_lun(EnvT* e);
	void openr(EnvT* e);
	void openw(EnvT* e);
	void openu(EnvT* e);
	void socket(EnvT* e);
	void flush_lun(EnvT* e);
	void close_lun(EnvT* e);
	void free_lun(EnvT* e);
	void writeu(EnvT* e);
	void readu(EnvT* e);

  
	// help_item is used by EnvT::HeapGC
	void help_item(std::ostream& os,
		BaseGDL* par, DString parString, bool doIndentation);
  void help_struct( std::ostream& os,  BaseGDL* par, int indent , bool debug );
	void exitgdl(EnvT* e);

	// in print.cpp
	void print(EnvT* e);
	void printf(EnvT* e);
	void print_os(std::ostream* os, EnvT* e, int parOffset, SizeT width);

	// in read.cpp
	void read(EnvT* e);
	void readf(EnvT* e);
	void reads(EnvT* e);
	void read_is(std::istream* is, EnvT* e, int parOffset);

	void on_error(EnvT* e);
	void catch_pro(EnvT* e);

	void strput(EnvT* e);

	void retall(EnvT* e);

	void stop(EnvT* e);

	void heap_gc(EnvT* e);
	void heap_free(EnvT* e);

	void defsysv(EnvT* e);

	void message_pro(EnvT* e);

	void byteorder(EnvT* e);

	void spawn_pro(EnvT* e);

	// the following by Peter Messmer 
	// (messmer@users.sourceforge.net)
	void setenv_pro(EnvT* e);
	void struct_assign_pro(EnvT* e);

	// the following by Sergio Gelato
	// (Sergio.Gelato@astro.su.se)
	void replicate_inplace_pro(EnvT* e);

	// the following by Alain Coulais
	// (alaingdl @@ users.sourceforge.net)


	void resolve_routine(EnvT* e);

	// the following by Sylwester Arabas
	// (slayoo@igf.fuw.edu.pl)
	void caldat(EnvT* e);
	BaseGDL* julday(EnvT* e);
	void pm(EnvT* e);
	//dummy stub preventing !err and other !error_state to be set!
	void pref_set_pro(EnvT* e);

	void delvar_pro( EnvT* e);
	void findvar_pro( EnvT* e);
} // namespace

#endif
