/***************************************************************************
                          hash.hpp  - for HASH objects
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

#ifndef HASH_HPP_
#define HASH_HPP_

void HASH__ToStream( DStructGDL* oStructGDL, std::ostream& o, SizeT w, SizeT* actPosPtr);

  DLong HashIndex( DStructGDL* hashTable, BaseGDL* key);

namespace lib {

  BaseGDL* HASH___OverloadIsTrue( EnvUDT* e);

  BaseGDL* HASH___OverloadNEOp( EnvUDT* e);
  BaseGDL* HASH___OverloadEQOp( EnvUDT* e);

  BaseGDL* HASH___OverloadPlus( EnvUDT* e);

  BaseGDL* HASH___OverloadBracketsRightSide( EnvUDT* e);

  void HASH___OverloadBracketsLeftSide( EnvUDT* e);

  BaseGDL* hash__haskey( EnvUDT* e);

  BaseGDL* hash__remove_fun( EnvUDT* e);
  void hash__remove_pro( EnvUDT* e);

  BaseGDL* hash__values( EnvUDT* e);
  BaseGDL* hash__keys( EnvUDT* e);

  BaseGDL* hash__tostruct( EnvUDT* e);

  BaseGDL* hash__count( EnvUDT* e);
  SizeT HASH_count( DStructGDL* oStructGDL);

  BaseGDL* hash__isempty( EnvUDT* e);
  BaseGDL* hash__isordered( EnvUDT* e);
  BaseGDL* hash__isfoldcase( EnvUDT* e);
  BaseGDL* hash__where( EnvUDT* e);

  BaseGDL* hash_fun( EnvT* e);
  BaseGDL* orderedhash_fun( EnvT* e);
}

#endif
