/***************************************************************************
                        datalistt.hpp  -  holds (local and global) variables
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

#ifndef DATALISTT_HPP_
#define DATALISTT_HPP_

#include "basegdl.hpp"

struct EnvType
{
  EnvType( BaseGDL* pIn, BaseGDL** ppIn): p(pIn), pp(ppIn) {}
  bool IsSet() { return (p != NULL || pp != NULL);}
  BaseGDL* p;
  BaseGDL** pp;
};

class DataListT
{
  std::vector<EnvType> env; // holds the variables

public:
  DataListT(): env() {}
  ~DataListT()
  {
    for( SizeT i=0; i<env.size(); i++)
      delete env[i].p;
  }

  bool InLoc( BaseGDL** p) const
  {
    return ( !env.empty() && p >= &env.front().p && p <= &env.back().p);
  }

  bool Contains( BaseGDL* p) const
  {
    for( SizeT i=0; i<env.size(); i++)
      {
	if( env[i].p == p) return true;
	if( env[i].pp != NULL && *(env[i].pp) == p) return true;
      }
    return false;
  }

  void RemoveLoc( BaseGDL* p) 
  {
    for( SizeT i=0; i<env.size(); i++)
      if( env[i].p == p)
	{
	  env[i].p = NULL;
	  return;
	}
  }

  void push_back( BaseGDL* p)
  {
    env.push_back(EnvType(p,NULL));
  }
  void push_back( BaseGDL** pp)
  {
    env.push_back(EnvType(NULL,pp));
  }

  void AddOne()
  {
    env.push_back(EnvType(NULL,NULL));
  }

  void pop_back()
  {
    env.pop_back();
  }

  SizeT size() { return env.size();}
  void reserve( SizeT s)
  {
    env.reserve( s);
  }
  void resize( SizeT s)
  {
    env.resize( s, EnvType(NULL,NULL));
  }

  BaseGDL*& operator[]( const SizeT ix)
  {
    if( env[ ix].pp != NULL) return *env[ ix].pp;
    return env[ ix].p;
  }
  void Clear( SizeT ix)
  {
    env[ ix]=EnvType(NULL,NULL);
  }
  void Reset( SizeT ix, BaseGDL* p)
  {
    if( env[ ix].p != NULL) delete env[ ix].p;
    env[ ix]=EnvType(p,NULL);
  }
  void Reset( SizeT ix, BaseGDL** pp)
  {
    if( env[ ix].p != NULL) delete env[ ix].p;
    env[ ix]=EnvType(NULL,pp);
  }
  void Set( SizeT ix, BaseGDL* p)
  {
    env[ ix]=EnvType(p,NULL);
  }
  void Set( SizeT ix, BaseGDL** pp)
  {
    env[ ix]=EnvType(NULL,pp);
  }
  bool IsSet( SizeT ix)
  {
    return env[ix].IsSet(); //(env[ ix].p != NULL || env[ ix].pp != NULL);
  }
  BaseGDL* Grab( SizeT ix)
  {
    BaseGDL* ret;
    if( env[ ix].p != NULL) 
      {
	ret=env[ ix].p;
	env[ ix].p=NULL;
	return ret;
      }
    if( env[ ix].pp != NULL) return (*env[ ix].pp)->Dup();
    return NULL;
 }

  // finds the local variable pp points to
  int FindLocal( BaseGDL** pp)
  {
    for( SizeT i=0; i<env.size(); i++)
      if( &env[i].p == pp) return static_cast<int>(i);
    return -1;
  }

  // finds the gloabl variable pp
  int FindGlobal( BaseGDL** pp)
  {
    for( SizeT i=0; i<env.size(); i++)
      if( env[i].pp == pp) return static_cast<int>(i);
    return -1;
  }

  BaseGDL** GetPtrTo( BaseGDL* p)
  {
    for( SizeT i=0; i<env.size(); i++)
      {
	if( env[i].p == p) return &env[i].p;
	if( env[i].pp != NULL && *env[i].pp == p) return env[i].pp;
      }
    return NULL;
  }

  BaseGDL* Loc( SizeT ix)
  {
    return env[ ix].p;
  }
  BaseGDL** Env( SizeT ix)
  {
    return env[ ix].pp;
  }
};

#endif
