/***************************************************************************
                        datalistt.hpp  -  holds (local and global) variables
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

#ifndef DATALISTT_HPP_
#define DATALISTT_HPP_

#include "basegdl.hpp"

class DataListT
{
  std::vector<BaseGDL**> env; // holds the variables
  std::vector<BaseGDL*>  loc; // for local data

public:
  DataListT(): env(), loc() {}
  ~DataListT()
  {
    for( SizeT i=0; i<loc.size(); i++)
      delete loc[i];
  }

  bool InLoc( BaseGDL** p) const
  {
    return ( !loc.empty() && p >= &loc.front() && p <= &loc.back());
  }

  bool Contains( BaseGDL* p) const
  {
    for( SizeT i=0; i<loc.size(); i++)
      {
	if( loc[i] == p) return true;
	if( env[i] != NULL && *env[i] == p) return true;
      }
    return false;
  }

  void push_back( BaseGDL* p)
  {
    loc.push_back(p);
    env.push_back(NULL);
  }
  void push_back( BaseGDL** pp)
  {
    loc.push_back(NULL);
    env.push_back(pp);
  }
  SizeT size() { return loc.size();}
  void reserve( SizeT s)
  {
    env.reserve( s);
    loc.reserve( s);
  }
  void resize( SizeT s)
  {
    env.resize( s, NULL);
    loc.resize( s, NULL);
  }

  BaseGDL*& operator[]( const SizeT ix)
  {
    if( env[ ix] != NULL) return *env[ ix];
    return loc[ ix];
  }
  void Clear( SizeT ix)
  {
    loc[ ix]=NULL;
    env[ ix]=NULL;
  }
  void Reset( SizeT ix, BaseGDL* p)
  {
    if( loc[ ix] != NULL) delete loc[ ix];
    loc[ ix]=p;
    env[ ix]=NULL;
  }
  void Reset( SizeT ix, BaseGDL** pp)
  {
    if( loc[ ix] != NULL) delete loc[ ix];
    loc[ ix]=NULL;
    env[ ix]=pp;
  }
  void Set( SizeT ix, BaseGDL* p)
  {
    loc[ ix]=p;
    env[ ix]=NULL;
  }
  void Set( SizeT ix, BaseGDL** pp)
  {
    loc[ ix]=NULL;
    env[ ix]=pp;
  }
  bool IsSet( SizeT ix)
  {
    return (loc[ ix] != NULL || env[ ix] != NULL);
  }
  BaseGDL* Grab( SizeT ix)
  {
    BaseGDL* ret;
    if( loc[ ix] != NULL) 
      {
	ret=loc[ ix];
	loc[ ix]=NULL;
	return ret;
      }
    if( env[ ix] != NULL) return (*env[ ix])->Dup();
    return NULL;
 }

  // finds the local variable pp points to
  int FindLocalKW( BaseGDL** pp)
  {
    for( SizeT i=0; i<loc.size(); i++)
      if( &loc[i] == pp) return static_cast<int>(i);
    return -1;
  }

  // finds the gloabl variable pp
  int FindGlobalKW( BaseGDL** pp)
  {
    for( SizeT i=0; i<env.size(); i++)
      if( env[i] == pp) return static_cast<int>(i);
    return -1;
  }

  BaseGDL* Loc( SizeT ix)
  {
    return loc[ ix];
  }
  BaseGDL** Env( SizeT ix)
  {
    return env[ ix];
  }
};

#endif
