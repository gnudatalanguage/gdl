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
//   EnvType( BaseGDL* pIn): p(pIn), pp(NULL) {}
//   EnvType( BaseGDL** ppIn): p(NULL), pp(ppIn) {}
  EnvType() {}
//   ~EnvType() { delete p;}
  void Null() { p = NULL; pp = NULL;}
  void NullP() { p = NULL;}
  void NullPP() { pp = NULL;}
  
  bool IsSet() { return (p != NULL || pp != NULL);}

  bool IsP() const { return p != NULL;}
  bool IsPP() const { return pp != NULL;}

  void SetPNullPP( BaseGDL* pIn) {p = pIn; pp = NULL;}
  void SetPPNullP( BaseGDL** ppIn) {pp = ppIn; p = NULL;}
  void SetP( BaseGDL* pIn) {p = pIn;}
  void SetPP( BaseGDL** ppIn) {pp = ppIn;}
  
  BaseGDL* P() const { return p;}
  BaseGDL** PP() const { return pp;}
  BaseGDL*& PRef() { return p;}
  BaseGDL**& PPRef() { return pp;}
  BaseGDL* const& PRefConst() const { return p;}
  BaseGDL** const& PPRefConst() const { return pp;}

private:
  BaseGDL* p;
  BaseGDL** pp;
};

const SizeT DataListDefaultLength = 64;

// this data structure is optimized for list sizes < defaultLength
// DataListDefaultLength should be set such that it will probably never exceed
// note: it will work for larger lists as well, but then copy operations are performed
template< typename T, SizeT defaultLength> class EnvTypePreAllocListT
{
public:
typedef T* iterator;

private:
T* eArr;
T buf[defaultLength];
SizeT sz;
SizeT actLen;

public:
EnvTypePreAllocListT(): eArr(buf), sz(0), actLen(defaultLength) {}
~EnvTypePreAllocListT()
{
	if( eArr != buf)
		delete[] eArr;
}
void push_back( BaseGDL* p)
{
	if( sz >= actLen)
	{
		actLen *= 4; // should only happen rarely
		T* newArr = new T[ actLen];
		for( SizeT i=0; i<sz; ++i)
			newArr[i] = eArr[i];
		if( eArr != buf)
			delete[] eArr;
		eArr = newArr;
	}
	eArr[ sz++].SetPNullPP( p);
}
void push_back( BaseGDL** pp)
{
	if( sz >= actLen)
	{
		actLen *= 4; // should only happen rarely
		T* newArr = new T[ actLen];
		for( SizeT i=0; i<sz; ++i)
			newArr[i] = eArr[i];
		if( eArr != buf)
			delete[] eArr;
		eArr = newArr;
	}
	eArr[ sz++].SetPPNullP( pp);
}
void push_back()
{
	if( sz >= actLen)
	{
		actLen *= 4; // should only happen rarely
		T* newArr = new T[ actLen];
		for( SizeT i=0; i<sz; ++i)
			newArr[i] = eArr[i];
		if( eArr != buf)
			delete[] eArr;
		eArr = newArr;
	}
	eArr[ sz++].Null();
}
const T& operator[]( SizeT i) const { assert( i<sz);  return eArr[i];}
T& operator[]( SizeT i)
{
assert( i<sz);
return eArr[i];
}
bool Contains( BaseGDL* p) const
{
  for( SizeT i=0; i<sz; ++i)
    {
      if( eArr[i].P() == p) return true;
      if( eArr[i].IsPP() && *(eArr[i].PP()) == p) return true;
    }
  return false;
}
void RemoveLoc( BaseGDL* p) 
  {
    for( SizeT i=0; i<sz; ++i)
      if( eArr[i].P() == p) 
	{
	  eArr[i].NullP(); 
	  return;
	}
  }
SizeT size() const { return sz;}
iterator begin()  { return &eArr[0];}
iterator end()  { return &eArr[sz];}
bool empty() const { return sz == 0;}
T& front() { return eArr[0];}
const T& front() const { return eArr[0];}
T& back() { return eArr[sz-1];}
const T& back() const { return eArr[sz-1];}
void pop_back() { --sz;}
void resize( SizeT newSz)
{
  assert( newSz >= sz);
  if( newSz > actLen) // should only happen rarely
    {
      actLen = newSz; 
      T* newArr = new T[ actLen];
      SizeT i=0;
      for( ; i<sz; ++i)
	      newArr[i] = eArr[i];
      for( ; i<newSz; ++i)
	      {
		      newArr[i].Null();
	      }
      if( eArr != buf)
	      delete[] eArr;
      eArr = newArr;
      sz = newSz;
      return;
    }
  for( ; sz<newSz; ++sz)
    {
      eArr[sz].Null();
    }
// 	for( SizeT i=sz; i<newSz; ++i)
// 		{
// 			eArr[i].Null();
// 		}
// 	sz = newSz;
}

};

class DataListT
{
// typedef std::vector<EnvType> ListT;
typedef EnvTypePreAllocListT<EnvType, DataListDefaultLength> ListT;

ListT env; // holds the variables

public:
  DataListT(): env() {}
  ~DataListT()
  {
	ListT::iterator pEnd = env.end();
	for( ListT::iterator p = env.begin(); p!=pEnd;++p)
		delete p->P();
//     for( SizeT i=0; i<env.size(); i++)
//       delete env[i];
//       delete env[i].p;
  }

  bool InLoc( BaseGDL** p) const
  {
    return ( !env.empty() && p >= &env.front().PRefConst() && p <= &env.back().PRefConst());
  }

  bool Contains( BaseGDL* p) const
  {
    return env.Contains( p);
  }

  void RemoveLoc( BaseGDL* p) 
  {
    env.RemoveLoc( p);
  }

  void push_back( BaseGDL* p)
  {
    env.push_back(p);
  }
  void push_back( BaseGDL** pp)
  {
    env.push_back(pp);
  }

  void AddOne()
  {
    env.push_back();
  }

  void pop_back()
  {
    env.pop_back();
  }

  SizeT size() const { return env.size();}
//    void reserve( SizeT s)
//    {
//      env.reserve( s);
//    }
   void resize( SizeT s)
   {
     env.resize( s); //, EnvType(NULL,NULL));
   }

  BaseGDL*& operator[]( const SizeT ix)
  {
    if( env[ ix].IsPP() )
		return *env[ ix].PP();
    return env[ ix].PRef(); // ok, IsP
  }
  void Clear( SizeT ix)
  {
    env[ ix].Null();
  }
  void Reset( SizeT ix, BaseGDL* p)
  {
    if( env[ ix].IsP()) // implies pp == NULL
    {
		delete env[ ix].P();
		env[ ix].SetP( p);
		return;
    }
    // !IsP -> p == NULL
	env[ ix].SetPNullPP( p);
  }
  void Reset( SizeT ix, BaseGDL** pp)
  {
    if( env[ ix].IsP())
    {
		delete env[ ix].P();
 		env[ ix].NullP();
    }
    // !IsP -> p == NULL
    env[ ix].SetPP( pp);
  }
  void Set( SizeT ix, BaseGDL* p)
  {
    env[ ix].SetPNullPP( p);
  }
  void Set( SizeT ix, BaseGDL** pp)
  {
    env[ ix].SetPPNullP( pp);
  }
  bool IsSet( SizeT ix)
  {
    return env[ix].IsSet(); //(env[ ix].p != NULL || env[ ix].pp != NULL);
  }
  BaseGDL* Grab( SizeT ix)
  {
    BaseGDL* ret;
    if( env[ ix].IsP())
      {
		ret=env[ ix].P();
		env[ ix].NullP();
		return ret;
      }
    if( env[ ix].IsPP())
		return (*env[ ix].PP())->Dup();
    return NULL;
 }

  // finds the local variable pp points to
  int FindLocal( BaseGDL** pp)
  {
    for( SizeT i=0; i<env.size(); i++)
      if( &env[i].PRef() == pp) return static_cast<int>(i);
    return -1;
  }

  // finds the gloabl variable pp
  int FindGlobal( BaseGDL** pp)
  {
    for( SizeT i=0; i<env.size(); i++)
      if( env[i].PP() == pp) return static_cast<int>(i);
    return -1;
  }

  BaseGDL** GetPtrTo( BaseGDL* p)
  {
	assert( p != NULL);
    for( SizeT i=0; i<env.size(); i++)
      {
		if( env[i].P() == p) return &env[i].PRef();
		if( env[i].IsPP() && *env[i].PP() == p) return env[i].PP();
      }
    return NULL;
  }

  BaseGDL* Loc( SizeT ix)
  {
    return env[ ix].P();
  }
  BaseGDL** Env( SizeT ix)
  {
    return env[ ix].PP();
  }
};

#endif
