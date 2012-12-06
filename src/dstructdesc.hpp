/***************************************************************************
                          dstructdesc.hpp  -  GDL struct descriptor
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

#ifndef DSTRUCTDESC_HPP_
#define DSTRUCTDESC_HPP_

#include <vector>
#include <deque>
#include <string>
#include <functional>

#include "basegdl.hpp"
#include "dpro.hpp"
#include "typedefs.hpp"
#include "overload.hpp"

class DStructBase
{
private:
  std::deque<SizeT>    tagOffset; // data offset of tags

protected:
  std::deque<BaseGDL*> tags; // Data_<Sp> for data, 'Sp' for structList elements
  void Add( BaseGDL* t)
  {
  tags.push_back(t); // grabs
  // attention: there is a subtle difference for NBytes()
  // for SpDString (considers sizeof( DString)), used here 
  // and DStringGDL (considers actual string sizes)
  SizeT nBytes = tags.back()->NBytes();

  // alignment
  const int sizeOfPtr = sizeof( char*);
  SizeT exceed = nBytes % sizeOfPtr;
  if( exceed > 0)
	nBytes += sizeOfPtr - exceed;

  // valid tagOffset (used by NBytes())
  tagOffset.push_back( tagOffset.back() + nBytes);
  }

public:
  DStructBase(): tagOffset( 1, 0)
  {}
  
  DStructBase( const DStructBase* d_): tags( d_->NTags())
  {
    SizeT nTags = d_->NTags();
    for( SizeT t=0; t<nTags; ++t)
	{
	  tags[ t]    = d_->tags[ t]->GetTag();
	}
    tagOffset = d_->tagOffset;
  }
  
  virtual ~DStructBase();

  BaseGDL*& operator[] (const SizeT d1)
  { return tags[d1];}
  const BaseGDL* operator[] (const SizeT d1) const
  { return tags[d1];}

  SizeT Offset( SizeT t, SizeT ix) const
  {
    return (tagOffset[ t] + ix * NBytes());
  }
  SizeT Offset( SizeT t) const
  {
    return tagOffset[ t];
  }

  SizeT NTags() const  { return tags.size();}
  SizeT NBytes() const { return tagOffset.back();}

  bool ContainsStringPtrObject();
};


class DStructDesc;
typedef std::deque<DStructDesc*> StructListT;

// descriptor of structs layout ************************************************
// unnamed struct
// this is never used directly, only DStructDesc (see below)
class DUStructDesc: public DStructBase
{
private:
  std::deque<std::string>  tNames;  // tag names
  
public:
  DUStructDesc(): DStructBase()
  {}

  DUStructDesc( const DUStructDesc* d_): 
    DStructBase( d_), 
    tNames( d_->tNames)
  {}
  
  //  ~DUStructDesc();

  void AddTag( const std::string& tagName, const BaseGDL* data);
  
  int TagIndex( const std::string& tN) const
  {
    for( SizeT i=0; i< tNames.size(); i++)
      if( tNames[i] == tN) return static_cast<int>(i);
    return -1;
  }
  const std::string& TagName( const SizeT ix) const
  {
    return tNames[ ix];
  }
};

// named struct
class DStructDesc: public DUStructDesc
{
private:

  SizeT refCount;

  bool isUnnamed;
  
  // operatorList != NULL means inherited from GDL_OBJECT
  OperatorList* operatorList;
  // avoid extra allocation
//   char operatorListBuffer[ sizeof(operatorList)];
  // ONLY FOR GDL_OBJECT
  void InitOperatorList() { assert( operatorList == NULL); operatorList = new OperatorList();}
  friend void InitStructs(); // restrict usage to definition of GDL_OBECT
  
private:

  std::string              name;
  StructListT              parent;  // parent classes 
  IDList                   noDirectMembers;
  
  FunListT                 fun; // member functions
  ProListT                 pro; // member procedures

  DStructDesc( const DStructDesc&) {} // disabeld

public:
  DStructDesc( const std::string& n): DUStructDesc(), refCount( 1), operatorList( NULL), name(n)
  {
//     name=n;
    // if this is to be changed, see also:
    // DStructGDL::DStructGDL( const string& name_) // (dstructgdl.cpp)
    isUnnamed = (name[0] == '$');
  }

// private:
//   // this is only used for unnamed structs -> only copy name from 'this'
//   DStructDesc( const DStructDesc* d_): 
//     DUStructDesc( d_), 
//     name( d_->name) // must be "$..."
//   {}
// public:
  ~DStructDesc();

  friend bool operator==(const DStructDesc& left, const DStructDesc& right);
  friend bool operator!=(const DStructDesc& left, const DStructDesc& right);

  const std::string& Name() const { return name;}

  FunListT& FunList()
  {
    return fun;
  }

  DFun* FindInFunList( const std::string& n)
  {
    FunListT::iterator p=std::find_if(fun.begin(),fun.end(),Is_eq<DFun>(n));
    if( p != fun.end()) return *p;
    return NULL;
  }
  
  ProListT& ProList()
  {
    return pro;
  }

  DPro* FindInProList( const std::string& n)
  {
    ProListT::iterator p=std::find_if(pro.begin(),pro.end(),Is_eq<DPro>(n));
    if( p != pro.end()) return *p;
    return NULL;
  }

  DPro* GetPro( const std::string& pName);
  DPro* GetPro( const std::string& pName, const std::string& parentName);
  DFun* GetFun( const std::string& pName);
  DFun* GetFun( const std::string& pName, const std::string& parentName);

  // if this is to be changed, see also:
  // DStructGDL::DStructGDL( const string& name_) // (dstructgdl.cpp)
  bool IsUnnamed() const { return isUnnamed;}

//   bool InheritsGDL_OBJECT() const { return (operatorList != NULL);}
  OperatorList* GetOperatorList() const { return operatorList;}
  
  DSubUD* GetOperator( SizeT i) const
  { if( operatorList == NULL) return NULL; return (*operatorList)[ i];}
  
  void SetOperator( SizeT i, DSubUD* op)
  { if( operatorList == NULL) return; operatorList->SetOperator( i, op);}
  
  // copy appropiate member subroutines from fun and pro lists
  void SetupOperators();
  
  bool IsParent( const std::string& p)
  {
    if( p == name) return true;

    SizeT nParents=parent.size();
    for( SizeT i=0; i<nParents; i++)
      {
	if( parent[i]->IsParent( p)) return true;
      }
    return false;
  }

  void AddParent( DStructDesc*);

  void GetParentNames( std::deque< std::string>& pNames) const
  {
    SizeT nParents=parent.size();
    for( SizeT i=0; i<nParents; ++i)
      {
	pNames.push_back( parent[i]->Name());
      }
  }

  void AssureIdentical( DStructDesc* d);
  //  DStructDesc* FindEqual( const StructListT& sL);

  // for unnamed structs (typetraits.cpp)
  void AddRef()
  {
    ++refCount;
  }
  void Delete()
  {
    if( --refCount) return;
    delete this;
  }
};


class DStruct_eq: public std::unary_function<DStructDesc,bool>
{
  std::string name;

public:
  explicit DStruct_eq(const std::string& s): name(s) {}
  bool operator() (const DStructDesc* v) const { return v->Name() == name;}
};

DStructDesc* FindInStructList(StructListT, const std::string& name);

// actually holding the data **********************************************
// class DStructGDL see dstructgdl.hpp

#endif
