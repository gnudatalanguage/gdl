/***************************************************************************
                          extrat.hpp  -  (_REF)_EXTRA keyword handling
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

// used by EnvT

#ifndef EXTRA_HPP_
#define EXTRA_HPP_

#include <string>
#include <vector>

#include "datatypes.hpp"
#include "datalistt.hpp"

class EnvBaseT;

class ExtraT
{
  EnvBaseT* thisEnv;

  // extra keywords
  IDList            listName;
  DataListT         listEnv; // for (_REF)_EXTRA

  // the _EXTRA keyword
  BaseGDL*          loc; // extra keywords value
  BaseGDL**         env; // extra keywords value

  bool              strict; // _STRICT_EXTRA

  // disable use of default constructor
  ExtraT() {}

public:
  ExtraT( EnvBaseT* e): thisEnv(e), loc(NULL), env(NULL), strict(false) {}

  ~ExtraT() 
  {
    GDLDelete(loc);
  }

  void SetStrict( bool s)
  {
    strict = s;
  }

  void Set( BaseGDL* const val)
  {
    if( val->Type() != GDL_STRUCT && 
	val->Type() != GDL_STRING)
      throw GDLException("Invalid value for _EXTRA keyword.");
    
    GDLDelete(loc);
    
    loc=val;
  }
  void Set( BaseGDL** const val)
  {
    if( *val != NULL && 
	(*val)->Type() != GDL_STRUCT && 
	(*val)->Type() != GDL_STRING)
      throw GDLException("Invalid value for _EXTRA keyword.");
    
    env=val;
  }
  void Add( const std::string& k, BaseGDL* const val)
  {
    listName.push_back(k);
    listEnv.push_back(val);
  }
  void Add( const std::string& k, BaseGDL** const val)
  {
    listName.push_back(k);
    listEnv.push_back(val);
  }

  int Find( const std::string& name)
  {
    String_abbref_eq strAbbrefEq_name(name);

    // search keyword
    IDList::iterator f=std::find_if(listName.begin(),
			       listName.end(),
			       strAbbrefEq_name);
    if( f == listName.end()) return -1;

    // Note: there might be duplicate extra keyword names, return first one
//     // continue search (for ambiguity)
//     IDList::iterator ff=std::find_if(f+1,
// 				listName.end(),
// 				strAbbrefEq_name);
//     if( ff != listName.end())
//       {
// 	throw GDLException("Ambiguous keyword abbreviation in _EXTRA: "+name);
//       }
    return std::distance( listName.begin(),f);
  }

  // 1. extract own keywords from _EXTRA data (override explicit ones)
  // 2. if pro has (_REF)_EXTRA:
  // combine additional keywords and the (remaining) _EXTRA data to pro's 
  // (_REF)_EXTRA value
  void ResolveExtra(EnvBaseT* caller);
};

#endif
