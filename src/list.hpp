/***************************************************************************
                          list.hpp  - for LIST objects
                             -------------------
    begin                : July 22 2013
    copyright            : (C) 2013 by M. Schellens et al.
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

#ifndef LIST_HPP_
#define LIST_HPP_

// #include <envt.hpp>
void LIST__ToStream( DStructGDL* oStructGDL, std::ostream& o, SizeT w, SizeT* actPosPtr);

DStructGDL*GetOBJ( BaseGDL* selfP, EnvUDT* e);
DStructGDL*GetSELF( BaseGDL* selfP, EnvUDT* e);

namespace lib {

   BaseGDL* list_fun( EnvT* e);

   void list__cleanup( EnvUDT* e);

   BaseGDL* LIST___OverloadIsTrue( EnvUDT* e);

   BaseGDL* LIST___OverloadBracketsRightSide( EnvUDT* e);
   void LIST___OverloadBracketsLeftSide( EnvUDT* e);
   BaseGDL* LIST___OverloadPlus( EnvUDT* e);
   BaseGDL* LIST___OverloadEQOp( EnvUDT* e);
   BaseGDL* LIST___OverloadNEOp( EnvUDT* e);

   void list__add( EnvUDT* e);
   
   void container__cleanup( EnvUDT* e);
   BaseGDL* container__iscontained( EnvUDT* e);
   BaseGDL* container__equals( EnvUDT* e);
   void container__remove( EnvUDT* e);
   void list__remove_pro( EnvUDT* e);

   void list__reverse( EnvUDT* e);
   void list__swap( EnvUDT* e);
   void list__move( EnvUDT* e);
   
   BaseGDL* list__remove_fun( EnvUDT* e);

   BaseGDL* list__toarray( EnvUDT* e);

   BaseGDL* list__isempty( EnvUDT* e);
   SizeT LIST_count( DStructGDL* oStructGDL);
   BaseGDL* list__count( EnvUDT* e);
   BaseGDL* list__where( EnvUDT* e);
// these added in order to accomodate being an IDL_CONTAINER:  
	BaseGDL* list__get( EnvUDT* e);
	BaseGDL* list__init( EnvUDT* e);
}

#endif
