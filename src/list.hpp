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

   // helper functions for LIST
   DStructGDL* GetLISTStruct( EnvUDT* e, DPtr actP);
   DPtr GetLISTNode( EnvUDT* e, DStructGDL* self, DLong targetIx);
   DStructGDL*GetSELF( BaseGDL* selfP, EnvUDT* e);

namespace lib {

   void list__add( EnvUDT* e);
   BaseGDL* list__remove( EnvUDT* e);

}

#endif