/***************************************************************************
                          objects.hpp  -  global structures
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

#ifndef OBJECTS_HPP_
#define OBJECTS_HPP_

//#include<deque>
#include<string>

#include "datatypes.hpp"

#include "dvar.hpp"
#include "dpro.hpp"
#include "dcommon.hpp"
#include "envt.hpp"
#include "io.hpp"

#ifdef USE_PYTHON
#include "gdlpython.hpp"
#endif

// class DInterpreter;
// extern DInterpreter* interpreter;

const std::string GDL_OBJECT_NAME("GDL_OBJECT");
const std::string GDL_CONTAINER_NAME("GDL_CONTAINER");

extern VarListT      sysVarList;
extern VarListT      sysVarRdOnlyList;

extern FunListT      funList;
extern ProListT      proList;
extern LibFunListT   libFunList;
extern LibProListT   libProList;

extern CommonListT   commonList;   // common blocks
extern StructListT   structList;

extern GDLFileListT  fileUnits;

// for OpenMP
const SizeT DefaultTPOOL_MIN_ELTS = 100000;
const SizeT DefaultTPOOL_MAX_ELTS = 0;
extern DLong CpuTPOOL_NTHREADS;
extern DLong64 CpuTPOOL_MIN_ELTS;
extern DLong64 CpuTPOOL_MAX_ELTS;

//extern DeviceListT   deviceList;
//extern Graphics*     actDevice;

// signals if control-c was pressed
extern volatile bool sigControlC;
extern          int  debugMode;

enum DebugCode {
  DEBUG_CLEAR=0,
  DEBUG_STOP = 1,
  DEBUG_PROCESS_STOP = 2,
  DEBUG_STEP = 3,
  DEBUG_STEPOVER= 4
};

template< class Container> void PurgeContainer( Container& s) 
{
  typename Container::iterator i;
  for(i = s.begin(); i != s.end(); ++i) 
    { delete *i;}// *i = NULL;}
  s.clear();  
}

void InitGDL(); // defined in gdl.cpp

void InitObjects();
void ResetObjects();

DLong GetLUN();

int ProIx(const std::string& n);
int FunIx(const std::string& n);

int LibProIx(const std::string& n);
int LibFunIx(const std::string& n);

bool IsFun(antlr::RefToken); // used by Lexer and Parser
bool IsRelaxed(); //tells if syntax is not strict (i.e. parenthesis for array indexes).
void SetStrict(bool value);

bool BigEndian();

int get_suggested_omp_num_threads();

template <typename T> class RefHeap {
  private:
    T* ptr;
    SizeT count;
    // 2016.05.13 GJ:
    bool doSave; // IDL 6.1 flag whether to include in a SAVE operation (HEAP_SAVE)
    // access via HEAP_SAVE( Heapvars) and HEAP_NOSAVE( Heapvars [,SET=[0/1]])
    bool enableGC;   // IDL 8.0 flag whether to perform automatic garbage collection 
    // access via HEAP_REFCOUNT([,/ENABLE] [,/DISABLE] [,IS_ENABLED=])
    // prevent usage
    RefHeap<T>& operator=(const RefHeap<T>& other) {	return *this;}
    template<class newType> operator RefHeap<newType>() {return RefHeap<newType>(ptr);}

      
  public:

    SizeT Count() const { return count;}

	bool IsEnabledGC() const {	return enableGC; }
	void EnableGC( bool set=true) { enableGC = set; }
	
	bool IsEnabledSave() const { return doSave; }
	void EnableSave( bool set=true) { doSave = set; }
	
    void Inc() {++count;}
    void Add( SizeT add) {count += add;}
    bool Dec() {assert(count > 0); return (--count==0);}

    RefHeap(T* p = 0)
    : ptr(p), count(1), doSave(false), enableGC(true)
    {}

    RefHeap( const RefHeap<T>& other)
    : ptr( other.ptr), count( other.count),  doSave( other.doSave), enableGC( other.enableGC)
    {}

    ~RefHeap()
    {}

    operator T* () const
    {
      return ptr;
    }

    T* operator->() const
    {
      return ptr;
    }

    T*& get()
    {
      return ptr;
    }
};

namespace structDesc {
 
  // these are used mainly in list.cpp and hash.cpp
  // as for .RESET_SESSION the pointers change
  // one can still use these as they get updated on every new creation in InitStructs()
  extern DStructDesc* LIST;
  extern DStructDesc* HASH;
  extern DStructDesc* GDL_CONTAINER;
  extern DStructDesc* GDL_CONTAINER_NODE;
  extern DStructDesc* GDL_HASHTABLEENTRY;
  
}

#endif
