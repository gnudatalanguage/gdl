/***************************************************************************
                          gdlhelp.hpp  -  basic GDL library procedures
                             -------------------
    begin                : May 01 2015
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

#ifndef GDLHELP_HPP_
#define GDLHELP_HPP_

namespace lib {

  
  // help_item is used by EnvT::HeapGC
  void help_item( std::ostream& os,
		  BaseGDL* par, DString parString, bool doIndentation);
  void help_help( EnvT* e); // This may be useful and easy to call
						// directly from interpreter
  void help_struct( std::ostream& os,  BaseGDL* par, int indent , bool debug );
  
   // the following by Alain Coulais
  // (alaingdl @@ users.sourceforge.net)
  BaseGDL* recall_commands(EnvT* e);

// routine_filepath is housed in basic_fun.spp
//  BaseGDL* routine_filepath( EnvT* e);

  void help_pro( EnvT* e);


  } // namespace

#endif
