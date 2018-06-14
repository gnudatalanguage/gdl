/***************************************************************************
                          objects.cpp  -  global structures
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

#include "includefirst.hpp"

#include <limits>
#include <ios>

#include "str.hpp"
#include "gdlexception.hpp"
#include "initsysvar.hpp"
#include "dnodefactory.hpp"

#include "objects.hpp"
#include "graphicsdevice.hpp"
#include "overload.hpp"

//#include "dinterpreter.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef USE_PYTHON
#include "gdlpython.hpp"
#endif

using namespace std;

// DInterpreter* interpreter = NULL;

// instantiate the global lists
VarListT      sysVarList;
VarListT      sysVarRdOnlyList;

FunListT      funList;
ProListT      proList;

LibFunListT   libFunList;
LibProListT   libProList;

CommonListT   commonList;

StructListT   structList;

GDLFileListT  fileUnits;

// flag for control-c
volatile bool sigControlC;
int           debugMode;
bool  strictInterpreter;
//	global garbage collection flag in support of HEAP_REFCOUNT:
static bool enabled_GC=true;
	bool IsEnabledGC()  // Referenced from GDLInterpreter.hpp
	 { return enabled_GC; }
	void EnableGC( bool set=true) // same names, many contexts.
	 { enabled_GC = set; }

namespace structDesc {
  // set in InitStructs()
  DStructDesc* LIST = NULL;
  DStructDesc* HASH = NULL;
  DStructDesc* GDL_CONTAINER = NULL;
  DStructDesc* GDL_CONTAINER_NODE = NULL;
  DStructDesc* GDL_HASHTABLEENTRY = NULL;
}

// for OpenMP
DLong CpuTPOOL_NTHREADS;
DLong64 CpuTPOOL_MIN_ELTS;
DLong64 CpuTPOOL_MAX_ELTS;
const DLong64 CpuTPOOL_MAX_ELTS_max = numeric_limits<DLong64>::max();

// instantiate own AST factory
//_DNodeFactory DNodeFactory;
antlr::ASTFactory DNodeFactory("DNode",DNode::factory);

void ResetObjects()
{
  
  GraphicsDevice::DestroyDevices();

  fileUnits.clear();
  cerr << flush; cout << flush; clog << flush;

  PurgeContainer(sysVarList);
//   sysVarRdOnlyList.clear(); // data is owned by sysVarList
  PurgeContainer(funList);
  PurgeContainer(proList);

  // delete common block data (which might be of type STRUCT)
  CommonListT::iterator i;
  for(i = commonList.begin(); i != commonList.end(); ++i) 
    { (*i)->DeleteData();}

  PurgeContainer(structList); // now deletes member subroutines (and they in turn common block references)
  // hence delete common blocks after structList
 
  // should be ok now as data is already deleted //avoid purging commonlist-->crash (probably some COMMON structures already destroyed)
  PurgeContainer(commonList);
  
  // don't purge library here
//   PurgeContainer(libFunList);
//   PurgeContainer(libProList);
  
#ifdef USE_PYTHON
  PythonEnd();
#endif
}

// initialize struct descriptors which are not system variables
void InitStructs()
{
  SpDInt    aInt;
  SpDLong   aLong;
  SpDString aString;
  SpDByte   aByte;
  SpDULong  aULong;
  SpDLong64 aLong64;
  SpDFloat  aFloat;
  SpDDouble aDouble;
  SpDComplex aComplex;
  SpDComplexDbl aComplexDbl;
//  SpDStruct  aStruct;  //protected, we cannot use?
  SpDInt   aColor( dimension(3));
  SpDLong   aLongArr8( dimension(8));
  SpDLong64   aLong64Arr8( dimension(8));
  SpDPtr    aPtrRef;
  SpDObj    aObjRef;
  SpDUInt   auInt;
  SpDULong  auLong;
  SpDULong64 auLong64;

  // OBJECTS =================================================

//A structure where objects are in the order of GDL Typecodes. Useful.

  DStructDesc* gdltypecodes = new DStructDesc("GDL_TYPECODES_AS_STRUCT");
   gdltypecodes->AddTag("GDL_UNDEF",   &aInt            );  //Danger! // 0 Undefined value, the default for new symbols
   gdltypecodes->AddTag("GDL_BYTE",    &aByte           );  // 1 byte
   gdltypecodes->AddTag("GDL_INT",	   &aInt            );  // 2 Integer scalar 
   gdltypecodes->AddTag("GDL_LONG",	   &aLong           );  // 3 long Integer scalar
   gdltypecodes->AddTag("GDL_FLOAT",   &aFloat          );  // 4 Real scalar
   gdltypecodes->AddTag("GDL_DOUBLE",  &aDouble         );  // 5 Double scalar
   gdltypecodes->AddTag("GDL_COMPLEX", &aComplex        );  // 6 Complex scalar
   gdltypecodes->AddTag("GDL_STRING",  &aString         );  // 7 String
   gdltypecodes->AddTag("GDL_STRUCT",  &aInt            );  //Danger // 8 Struct
   gdltypecodes->AddTag("GDL_COMPLEXDBL",  &aComplexDbl );  // 9 Complex double
   gdltypecodes->AddTag("GDL_PTR",	   &aPtrRef         );  // 10 Pointer
   gdltypecodes->AddTag("GDL_OBJ",     &aObjRef         );  // 11 Object reference
   gdltypecodes->AddTag("GDL_UINT",    &auInt           );  // 12 unsigned int
   gdltypecodes->AddTag("GDL_ULONG",   &auLong          );  // 13 unsigned long int
   gdltypecodes->AddTag("GDL_LONG64",  &aLong64         );  // 14 64 bit integer
   gdltypecodes->AddTag("GDL_ULONG64", &auLong64        );  // 15 unsigned 64 bit integer
  // insert into structList
  structList.push_back(gdltypecodes);


  DStructDesc* gdl_object = new DStructDesc( GDL_OBJECT_NAME);
  gdl_object->AddTag("GDL_OBJ_TOP", &aLong64);
  gdl_object->AddTag("__OBJ__", &aObjRef);
  gdl_object->AddTag("GDL_OBJ_BOTTOM", &aLong64);
  // special for GDL_OBJECT ony
  gdl_object->InitOperatorList();
  // insert into structList
  structList.push_back(gdl_object);
  
  DStructDesc* gdlList = new DStructDesc( "LIST");
  gdlList->AddTag("GDL_CONTAINER_TOP", &aLong64);
  gdlList->AddTag("GDLCONTAINERVERSION", &aInt);
  gdlList->AddTag("PHEAD", &aPtrRef);
  gdlList->AddTag("PTAIL", &aPtrRef);
  gdlList->AddTag("NLIST", &aLong);
  gdlList->AddTag("GDL_CONTAINER_BOTTOM", &aLong64);
  // use operator overloading (note: gdl_object's operators are not set yet)
  gdlList->AddParent(gdl_object);
  // insert into structList
  structList.push_back(gdlList);
  structDesc::LIST = gdlList;
  
  DStructDesc* gdlContainerNode = new DStructDesc( "GDL_CONTAINER_NODE");
  gdlContainerNode->AddTag("PNEXT", &aPtrRef);
  gdlContainerNode->AddTag("PDATA", &aPtrRef);
//   gdlContainerNode->AddTag("OOBJ", &aObjRef);
//   gdlContainerNode->AddTag("FLAGS", &aLong);
  // insert into structList
  structList.push_back(gdlContainerNode);
  structDesc::GDL_CONTAINER_NODE = gdlContainerNode;

  DStructDesc* gdlContainer = new DStructDesc( GDL_CONTAINER_NAME);
  gdlContainer->AddTag("GDL_CONTAINER_TOP", &aLong64);
  gdlContainer->AddTag("GDLCONTAINERVERSION", &aInt);
  gdlContainer->AddTag("PHEAD", &aPtrRef);
  gdlContainer->AddTag("PTAIL", &aPtrRef);
  gdlContainer->AddTag("NLIST", &aLong);
  gdlContainer->AddTag("GDL_CONTAINER_BOTTOM", &aLong64);
//  gdlContainer->AddParent(gdl_object);// no operator overloading
  structList.push_back(gdlContainer);
  structDesc::GDL_CONTAINER = gdlContainer;
  DStructDesc* gdlHash = new DStructDesc( "HASH");
  gdlHash->AddTag("TABLE_BITS", &aULong);
  gdlHash->AddTag("TABLE_SIZE", &aULong);
  gdlHash->AddTag("TABLE_COUNT", &aULong);
  gdlHash->AddTag("TABLE_REMOVE", &aULong);
  gdlHash->AddTag("TABLE_FOREACH", &aULong);
  gdlHash->AddTag("TABLE_DATA", &aPtrRef);
  // use operator overloading (note: gdl_object's operators are not set yet)
  gdlHash->AddParent(gdl_object);
  // insert into structList
  structList.push_back(gdlHash);
  structDesc::HASH = gdlHash;

  DStructDesc* gdlHashTE = new DStructDesc( "GDL_HASHTABLEENTRY");
  gdlHashTE->AddTag("PKEY", &aPtrRef);
  gdlHashTE->AddTag("PVALUE", &aPtrRef);
  // insert into structList
  structList.push_back(gdlHashTE);
  structDesc::GDL_HASHTABLEENTRY = gdlHashTE;
  
  // OBJECTS END =======================================================
   
  for (int big = 1; big >= 0; --big) 
  {
    DStructDesc* gdl_size = new DStructDesc( big ? "IDL_SIZE64" : "IDL_SIZE");
    gdl_size->AddTag("TYPE_NAME", &aString);
    gdl_size->AddTag("STRUCTURE_NAME", &aString);
    gdl_size->AddTag("TYPE", &aInt);
    gdl_size->AddTag("FILE_LUN", &aInt);
    if (big) {
      gdl_size->AddTag("FILE_OFFSET",  &aLong64);
      gdl_size->AddTag("N_ELEMENTS",  &aLong64);
    } else {
      gdl_size->AddTag("FILE_OFFSET",  &aLong);
      gdl_size->AddTag("N_ELEMENTS",  &aLong);
    }
    gdl_size->AddTag("N_DIMENSIONS",  &aLong);
    if (big) {
      gdl_size->AddTag("DIMENSIONS",  &aLong64Arr8);
    } else {
      gdl_size->AddTag("DIMENSIONS",  &aLongArr8);
    }
    // insert into structList
    structList.push_back(gdl_size);
  }

  for (int big = 1; big >= 0; --big) 
  {
    DStructDesc* fstat = new DStructDesc( big ? "FSTAT64" : "FSTAT");
    fstat->AddTag("UNIT", &aLong);
    fstat->AddTag("NAME", &aString);
    fstat->AddTag("OPEN", &aByte);
    fstat->AddTag("ISATTY", &aByte);
    fstat->AddTag("ISAGUI", &aByte);
    fstat->AddTag("INTERACTIVE", &aByte);
    fstat->AddTag("XDR", &aByte);
    fstat->AddTag("COMPRESS", &aByte);
    fstat->AddTag("READ", &aByte);
    fstat->AddTag("WRITE", &aByte);
    fstat->AddTag("ATIME", &aLong64);
    fstat->AddTag("CTIME", &aLong64);
    fstat->AddTag("MTIME", &aLong64);
    if (big)
    {
      fstat->AddTag("TRANSFER_COUNT", &aLong64);
      fstat->AddTag("CUR_PTR", &aLong64);
      fstat->AddTag("SIZE", &aLong64);
      fstat->AddTag("REC_LEN", &aLong64);
    }
    else
    {
      fstat->AddTag("TRANSFER_COUNT", &aLong);
      fstat->AddTag("CUR_PTR", &aLong);
      fstat->AddTag("SIZE", &aLong);
      fstat->AddTag("REC_LEN", &aLong);
    }
    // insert into structList
    structList.push_back( fstat);
  }

  DStructDesc* finfo = new DStructDesc("FILE_INFO");
  finfo->AddTag("NAME", &aString);
  finfo->AddTag("EXISTS", &aByte);
  finfo->AddTag("READ", &aByte);
  finfo->AddTag("WRITE", &aByte);
  finfo->AddTag("EXECUTE", &aByte);
  finfo->AddTag("REGULAR", &aByte);
  finfo->AddTag("DIRECTORY", &aByte);
  finfo->AddTag("BLOCK_SPECIAL", &aByte);
  finfo->AddTag("CHARACTER_SPECIAL", &aByte);
  finfo->AddTag("NAMED_PIPE", &aByte);
  finfo->AddTag("SETUID", &aByte);
  finfo->AddTag("SETGID", &aByte);
  finfo->AddTag("SOCKET", &aByte);
  finfo->AddTag("STICKY_BIT", &aByte);
  finfo->AddTag("SYMLINK", &aByte);
  finfo->AddTag("DANGLING_SYMLINK", &aByte);
  finfo->AddTag("MODE", &aLong);
  finfo->AddTag("ATIME", &aLong64);
  finfo->AddTag("CTIME", &aLong64);
  finfo->AddTag("MTIME", &aLong64);
  finfo->AddTag("SIZE", &aLong64);
  // insert into structList
  structList.push_back( finfo);

  DStructDesc* memory = new DStructDesc("IDL_MEMORY");
  memory->AddTag("CURRENT", &aLong);
  memory->AddTag("NUM_ALLOC", &aLong);
  memory->AddTag("NUM_FREE", &aLong);
  memory->AddTag("HIGHWATER", &aLong);
  // insert into structList
  structList.push_back(memory);

  DStructDesc* memory64 = new DStructDesc("IDL_MEMORY64");
  memory64->AddTag("CURRENT", &aLong64);
  memory64->AddTag("NUM_ALLOC", &aLong64);
  memory64->AddTag("NUM_FREE", &aLong64);
  memory64->AddTag("HIGHWATER", &aLong64);
  // insert into structList
  structList.push_back(memory64);

  DStructDesc* machar = new DStructDesc( "MACHAR");
  machar->AddTag("IBETA", &aLong);
  machar->AddTag("IT", &aLong);
  machar->AddTag("IRND", &aLong);
  machar->AddTag("NGRD", &aLong);
  machar->AddTag("MACHEP", &aLong);
  machar->AddTag("NEGEP", &aLong);
  machar->AddTag("IEXP", &aLong);
  machar->AddTag("MINEXP", &aLong);
  machar->AddTag("MAXEXP", &aLong);
  machar->AddTag("EPS", &aFloat);
  machar->AddTag("EPSNEG", &aFloat);
  machar->AddTag("XMIN", &aFloat);
  machar->AddTag("XMAX", &aFloat);
  // insert into structList
  structList.push_back( machar);

  DStructDesc* dmachar = new DStructDesc( "DMACHAR");
  dmachar->AddTag("IBETA", &aLong);
  dmachar->AddTag("IT", &aLong);
  dmachar->AddTag("IRND", &aLong);
  dmachar->AddTag("NGRD", &aLong);
  dmachar->AddTag("MACHEP", &aLong);
  dmachar->AddTag("NEGEP", &aLong);
  dmachar->AddTag("IEXP", &aLong);
  dmachar->AddTag("MINEXP", &aLong);
  dmachar->AddTag("MAXEXP", &aLong);
  dmachar->AddTag("EPS", &aDouble);
  dmachar->AddTag("EPSNEG", &aDouble);
  dmachar->AddTag("XMIN", &aDouble);
  dmachar->AddTag("XMAX", &aDouble);
  // insert into structList
  structList.push_back( dmachar);

  // for internal usage. make event handler destroy the TLB widget
  // attention: $WIDGET_DESTROY would identify this as an unnamed struct
  // see DStructDesc constructor
  DStructDesc* widgdestroy = new DStructDesc( "*WIDGET_DESTROY*");
  widgdestroy->AddTag("ID", &aLong);
  widgdestroy->AddTag("TOP", &aLong);
  widgdestroy->AddTag("HANDLER", &aLong);
  widgdestroy->AddTag("MESSAGE", &aLong);
  // insert into structList
  structList.push_back( widgdestroy);
  
  // for internal usage. make event handler exit form event loop on TLB destruction without destroying (again) the TLB widget
  DStructDesc* toplevelISdestroyed = new DStructDesc( "*TOPLEVEL_DESTROYED*");
  toplevelISdestroyed->AddTag("ID", &aLong);
  toplevelISdestroyed->AddTag("TOP", &aLong);
  toplevelISdestroyed->AddTag("HANDLER", &aLong);
  toplevelISdestroyed->AddTag("MESSAGE", &aLong);
  // insert into structList
  structList.push_back( toplevelISdestroyed);
  
  DStructDesc* widgbut = new DStructDesc( "WIDGET_BUTTON");
  widgbut->AddTag("ID", &aLong);
  widgbut->AddTag("TOP", &aLong);
  widgbut->AddTag("HANDLER", &aLong);
  widgbut->AddTag("SELECT", &aLong);
  // insert into structList
  structList.push_back( widgbut);

  DStructDesc* widgdlist = new DStructDesc( "WIDGET_DROPLIST");
  widgdlist->AddTag("ID", &aLong);
  widgdlist->AddTag("TOP", &aLong);
  widgdlist->AddTag("HANDLER", &aLong);
  widgdlist->AddTag("INDEX", &aLong);
  // insert into structList
  structList.push_back( widgdlist);

  DStructDesc* widgcbox = new DStructDesc( "WIDGET_COMBOBOX");
  widgcbox->AddTag("ID", &aLong);
  widgcbox->AddTag("TOP", &aLong);
  widgcbox->AddTag("HANDLER", &aLong);
  widgcbox->AddTag("INDEX", &aLong);
  widgcbox->AddTag("STR", &aString);
  // insert into structList
  structList.push_back( widgcbox);

  DStructDesc* widglist = new DStructDesc( "WIDGET_LIST");
  widglist->AddTag("ID", &aLong);
  widglist->AddTag("TOP", &aLong);
  widglist->AddTag("HANDLER", &aLong);
  widglist->AddTag("INDEX", &aLong);
  widglist->AddTag("CLICKS", &aLong);
  // insert into structList
  structList.push_back( widglist);

  DStructDesc* widgdtab = new DStructDesc( "WIDGET_TAB");
  widgdtab->AddTag("ID", &aLong);
  widgdtab->AddTag("TOP", &aLong);
  widgdtab->AddTag("HANDLER", &aLong);
  widgdtab->AddTag("TAB", &aLong);
  // insert into structList
  structList.push_back( widgdtab);

  DStructDesc* widgdsl = new DStructDesc( "WIDGET_SLIDER");
  widgdsl->AddTag("ID", &aLong);
  widgdsl->AddTag("TOP", &aLong);
  widgdsl->AddTag("HANDLER", &aLong);
  widgdsl->AddTag("VALUE", &aLong);
  widgdsl->AddTag("DRAG", &aInt);
  // insert into structList
  structList.push_back( widgdsl);

  DStructDesc* widgbgroup =  new DStructDesc( "WIDGET_BGROUP");
  widgbgroup->AddTag("ID", &aLong);
  widgbgroup->AddTag("TOP", &aLong);
  widgbgroup->AddTag("HANDLER", &aLong);
  widgbgroup->AddTag("SELECT", &aLong);
  widgbgroup->AddTag("VALUE", &aLong);
  // insert into structList
  structList.push_back(widgbgroup);

  DStructDesc* widgtxtc = new DStructDesc( "WIDGET_TEXT_CH");
  widgtxtc->AddTag("ID", &aLong);
  widgtxtc->AddTag("TOP", &aLong);
  widgtxtc->AddTag("HANDLER", &aLong);
  widgtxtc->AddTag("TYPE", &aInt); // 0
  widgtxtc->AddTag("OFFSET", &aLong);
  widgtxtc->AddTag("CH", &aByte);
  // insert into structList
  structList.push_back( widgtxtc);

  DStructDesc* widgtablec = new DStructDesc( "WIDGET_TABLE_CH");
  widgtablec->AddTag("ID", &aLong);
  widgtablec->AddTag("TOP", &aLong);
  widgtablec->AddTag("HANDLER", &aLong);
  widgtablec->AddTag("TYPE", &aInt); // 0
  widgtablec->AddTag("OFFSET", &aLong);
  widgtablec->AddTag("CH", &aByte);
  widgtablec->AddTag("X", &aLong);
  widgtablec->AddTag("Y", &aLong);
  // insert into structList
  structList.push_back( widgtablec);

  DStructDesc* widgtxtst = new DStructDesc( "WIDGET_TEXT_STR");
  widgtxtst->AddTag("ID", &aLong);
  widgtxtst->AddTag("TOP", &aLong);
  widgtxtst->AddTag("HANDLER", &aLong);
  widgtxtst->AddTag("TYPE", &aInt); // 1
  widgtxtst->AddTag("OFFSET", &aLong);
  widgtxtst->AddTag("STR", &aString);
  // insert into structList
  structList.push_back( widgtxtst);

  DStructDesc* widgtablest = new DStructDesc( "WIDGET_TABLE_STR");
  widgtablest->AddTag("ID", &aLong);
  widgtablest->AddTag("TOP", &aLong);
  widgtablest->AddTag("HANDLER", &aLong);
  widgtablest->AddTag("TYPE", &aInt); // 1
  widgtablest->AddTag("OFFSET", &aLong);
  widgtablest->AddTag("STR", &aString);
  widgtablest->AddTag("X", &aLong);
  widgtablest->AddTag("Y", &aLong);
   // insert into structList
  structList.push_back( widgtablest);

  DStructDesc* widgtxts = new DStructDesc( "WIDGET_TEXT_SEL");
  widgtxts->AddTag("ID", &aLong);
  widgtxts->AddTag("TOP", &aLong);
  widgtxts->AddTag("HANDLER", &aLong);
  widgtxts->AddTag("TYPE", &aInt); // 3
  widgtxts->AddTag("OFFSET", &aLong);
  widgtxts->AddTag("LENGTH", &aLong);
  // insert into structList
  structList.push_back( widgtxts);

  DStructDesc* widgtablesel = new DStructDesc( "WIDGET_TABLE_TEXT_SEL");
  widgtablesel->AddTag("ID", &aLong);
  widgtablesel->AddTag("TOP", &aLong);
  widgtablesel->AddTag("HANDLER", &aLong);
  widgtablesel->AddTag("TYPE", &aInt); // 3
  widgtablesel->AddTag("OFFSET", &aLong);
  widgtablesel->AddTag("LENGTH", &aLong);
  widgtablesel->AddTag("X", &aLong);
  widgtablesel->AddTag("Y", &aLong);
  // insert into structList
  structList.push_back( widgtablesel);
  
  DStructDesc* widgtablecelsel = new DStructDesc( "WIDGET_TABLE_CELL_SEL");
  widgtablecelsel->AddTag("ID", &aLong);
  widgtablecelsel->AddTag("TOP", &aLong);
  widgtablecelsel->AddTag("HANDLER", &aLong);
  widgtablecelsel->AddTag("TYPE", &aInt); // 4
  widgtablecelsel->AddTag("SEL_LEFT", &aLong);
  widgtablecelsel->AddTag("SEL_TOP", &aLong);
  widgtablecelsel->AddTag("SEL_RIGHT", &aLong);
  widgtablecelsel->AddTag("SEL_BOTTOM", &aLong);
  // insert into structList
  structList.push_back( widgtablecelsel);

  DStructDesc* widgtableceldesel = new DStructDesc( "WIDGET_TABLE_CELL_DESEL");
  widgtableceldesel->AddTag("ID", &aLong);
  widgtableceldesel->AddTag("TOP", &aLong);
  widgtableceldesel->AddTag("HANDLER", &aLong);
  widgtableceldesel->AddTag("TYPE", &aInt); // 9
  widgtableceldesel->AddTag("SEL_LEFT", &aLong);
  widgtableceldesel->AddTag("SEL_TOP", &aLong);
  widgtableceldesel->AddTag("SEL_RIGHT", &aLong);
  widgtableceldesel->AddTag("SEL_BOTTOM", &aLong);
  // insert into structList
  structList.push_back( widgtableceldesel);
  
  DStructDesc* widgtxtd = new DStructDesc( "WIDGET_TEXT_DEL");
  widgtxtd->AddTag("ID", &aLong);
  widgtxtd->AddTag("TOP", &aLong);
  widgtxtd->AddTag("HANDLER", &aLong);
  widgtxtd->AddTag("TYPE", &aInt); // 2
  widgtxtd->AddTag("OFFSET", &aLong);
  widgtxtd->AddTag("LENGTH", &aLong);
  // insert into structList
  structList.push_back( widgtxtd);
  
  DStructDesc* widgtabled = new DStructDesc( "WIDGET_TABLE_DEL");
  widgtabled->AddTag("ID", &aLong);
  widgtabled->AddTag("TOP", &aLong);
  widgtabled->AddTag("HANDLER", &aLong);
  widgtabled->AddTag("TYPE", &aInt); // 2
  widgtabled->AddTag("OFFSET", &aLong);
  widgtabled->AddTag("LENGTH", &aLong);
  widgtabled->AddTag("X", &aLong);
  widgtabled->AddTag("Y", &aLong);
   // insert into structList
  structList.push_back( widgtabled);
  
  DStructDesc* widgtablerowheight = new DStructDesc( "WIDGET_TABLE_ROW_HEIGHT");
  widgtablerowheight->AddTag("ID", &aLong);
  widgtablerowheight->AddTag("TOP", &aLong);
  widgtablerowheight->AddTag("HANDLER", &aLong);
  widgtablerowheight->AddTag("TYPE", &aInt); // 6
  widgtablerowheight->AddTag("ROW", &aLong);
  widgtablerowheight->AddTag("HEIGHT", &aLong);
  // insert into structList
  structList.push_back( widgtablerowheight);
  
  DStructDesc* widgtablecolwidth = new DStructDesc( "WIDGET_TABLE_COL_WIDTH");
  widgtablecolwidth->AddTag("ID", &aLong);
  widgtablecolwidth->AddTag("TOP", &aLong);
  widgtablecolwidth->AddTag("HANDLER", &aLong);
  widgtablecolwidth->AddTag("TYPE", &aInt); // 7
  widgtablecolwidth->AddTag("COL", &aLong);
  widgtablecolwidth->AddTag("WIDTH", &aLong);
  // insert into structList
  structList.push_back( widgtablecolwidth);
  
  DStructDesc* widgtableinvalidentry = new DStructDesc( "WIDGET_TABLE_INVALID_ENTRY");
  widgtableinvalidentry->AddTag("ID", &aLong);
  widgtableinvalidentry->AddTag("TOP", &aLong);
  widgtableinvalidentry->AddTag("HANDLER", &aLong);
  widgtableinvalidentry->AddTag("TYPE", &aInt); // 8
  widgtableinvalidentry->AddTag("STR", &aString);
  widgtableinvalidentry->AddTag("X", &aLong);
  widgtableinvalidentry->AddTag("Y", &aLong);
  // insert into structList
  structList.push_back( widgtableinvalidentry);
  
  DStructDesc* widgnoevent = new DStructDesc( "WIDGET_NOEVENT");
  widgnoevent->AddTag("ID", &aLong);
  widgnoevent->AddTag("TOP", &aLong);
  widgnoevent->AddTag("HANDLER", &aLong);  
  // insert into structList
  structList.push_back( widgnoevent);
  
  DStructDesc* widgver = new DStructDesc( "WIDGET_VERSION");
  widgver->AddTag("STYLE", &aString);
  widgver->AddTag("TOOLKIT", &aString);
  widgver->AddTag("RELEASE", &aString);
  // insert into structList
  structList.push_back( widgver);
  
  DStructDesc* widggeom = new DStructDesc( "WIDGET_GEOMETRY");
  widggeom->AddTag("XOFFSET",&aFloat);
  widggeom->AddTag("YOFFSET",&aFloat);
  widggeom->AddTag("XSIZE",&aFloat);
  widggeom->AddTag("YSIZE",&aFloat);
  widggeom->AddTag("SCR_XSIZE",&aFloat);
  widggeom->AddTag("SCR_YSIZE",&aFloat);
  widggeom->AddTag("DRAW_XSIZE",&aFloat);
  widggeom->AddTag("DRAW_YSIZE",&aFloat);
  widggeom->AddTag("MARGIN",&aFloat);
  widggeom->AddTag("XPAD",&aFloat);
  widggeom->AddTag("YPAD",&aFloat);
  widggeom->AddTag("SPACE",&aFloat);
  // insert into structList
  structList.push_back( widggeom);
  
  DStructDesc* widgdraw = new DStructDesc( "WIDGET_DRAW");
  widgdraw->AddTag("ID", &aLong);
  widgdraw->AddTag("TOP", &aLong);
  widgdraw->AddTag("HANDLER", &aLong);
  widgdraw->AddTag("TYPE", &aInt);
  widgdraw->AddTag("X", &aLong);
  widgdraw->AddTag("Y", &aLong);
  widgdraw->AddTag("PRESS", &aByte);
  widgdraw->AddTag("RELEASE", &aByte);
  widgdraw->AddTag("CLICKS", &aLong);
  widgdraw->AddTag("MODIFIERS", &aLong);
  widgdraw->AddTag("CH", &aByte);
  widgdraw->AddTag("KEY", &aLong);
  // insert into structList
  structList.push_back( widgdraw);
  
  DStructDesc* widgkbrdfocus = new DStructDesc( "WIDGET_KBRD_FOCUS");  
  widgkbrdfocus->AddTag("ID", &aLong);
  widgkbrdfocus->AddTag("TOP", &aLong);
  widgkbrdfocus->AddTag("HANDLER", &aLong);
  widgkbrdfocus->AddTag("ENTER", &aInt);
  structList.push_back( widgkbrdfocus);

  DStructDesc* widgcontext = new DStructDesc( "WIDGET_CONTEXT");
  widgcontext->AddTag("ID", &aLong);
  widgcontext->AddTag("TOP", &aLong);
  widgcontext->AddTag("HANDLER", &aLong);
  widgcontext->AddTag("X", &aLong);
  widgcontext->AddTag("Y", &aLong);
  widgcontext->AddTag("ROW", &aLong);
  widgcontext->AddTag("COL", &aLong);
  // insert into structList
  structList.push_back( widgcontext);
  
  DStructDesc* widgtlb_size_events = new DStructDesc( "WIDGET_BASE");
  widgtlb_size_events->AddTag("ID", &aLong);
  widgtlb_size_events->AddTag("TOP", &aLong);
  widgtlb_size_events->AddTag("HANDLER", &aLong);
  widgtlb_size_events->AddTag("X", &aLong);
  widgtlb_size_events->AddTag("Y", &aLong);
  // insert into structList
  structList.push_back( widgtlb_size_events);
  
  DStructDesc* widgtlb_move_events = new DStructDesc( "WIDGET_TLB_MOVE");
  widgtlb_move_events->AddTag("ID", &aLong);
  widgtlb_move_events->AddTag("TOP", &aLong);
  widgtlb_move_events->AddTag("HANDLER", &aLong);
  widgtlb_move_events->AddTag("X", &aLong);
  widgtlb_move_events->AddTag("Y", &aLong);
  // insert into structList
  structList.push_back( widgtlb_move_events);

  DStructDesc* widgtlb_iconify_events = new DStructDesc( "WIDGET_TLB_ICONIFY");
  widgtlb_iconify_events->AddTag("ID", &aLong);
  widgtlb_iconify_events->AddTag("TOP", &aLong);
  widgtlb_iconify_events->AddTag("HANDLER", &aLong);
  widgtlb_iconify_events->AddTag("ICONIFIED", &aInt);
  // insert into structList
  structList.push_back( widgtlb_iconify_events);

  DStructDesc* widgtlb_kill_request_events = new DStructDesc( "WIDGET_KILL_REQUEST");
  widgtlb_kill_request_events->AddTag("ID", &aLong);
  widgtlb_kill_request_events->AddTag("TOP", &aLong);
  widgtlb_kill_request_events->AddTag("HANDLER", &aLong);
  // insert into structList
  structList.push_back( widgtlb_kill_request_events);
  
  DStructDesc* widgtracking = new DStructDesc( "WIDGET_TRACKING");
  widgtracking->AddTag("ID", &aLong);
  widgtracking->AddTag("TOP", &aLong);
  widgtracking->AddTag("HANDLER", &aLong);
  widgtracking->AddTag("ENTER", &aInt);
  // insert into structList
  structList.push_back( widgtracking);

  DStructDesc* widgtimer = new DStructDesc( "WIDGET_TIMER");
  widgtimer->AddTag("ID", &aLong);
  widgtimer->AddTag("TOP", &aLong);
  widgtimer->AddTag("HANDLER", &aLong);
  // insert into structList
  structList.push_back( widgtimer);
  
  DStructDesc* colo = new DStructDesc( "WIDGET_SYSTEM_COLORS");
  colo->AddTag("DARK_SHADOW_3D", &aColor);
  colo->AddTag("FACE_3D", &aColor);
  colo->AddTag("LIGHT_EDGE_3D", &aColor);
  colo->AddTag("LIGHT_3D", &aColor);
  colo->AddTag("SHADOW_3D", &aColor);
  colo->AddTag("ACTIVE_BORDER", &aColor);
  colo->AddTag("ACTIVE_CAPTION", &aColor);
  colo->AddTag("APP_WORKSPACE", &aColor);
  colo->AddTag("DESKTOP", &aColor);
  colo->AddTag("BUTTON_TEXT", &aColor);
  colo->AddTag("CAPTION_TEXT", &aColor);
  colo->AddTag("GRAY_TEXT", &aColor);
  colo->AddTag("HIGHLIGHT", &aColor);
  colo->AddTag("HIGHLIGHT_TEXT", &aColor);
  colo->AddTag("INACTIVE_BORDER", &aColor);
  colo->AddTag("INACTIVE_CAPTION", &aColor);
  colo->AddTag("INACTIVE_CAPTION_TEXT", &aColor);
  colo->AddTag("TOOLTIP_BK", &aColor);
  colo->AddTag("TOOLTIP_TEXT", &aColor);
  colo->AddTag("MENU", &aColor);
  colo->AddTag("MENU_TEXT", &aColor);
  colo->AddTag("SCROLLBAR", &aColor);
  colo->AddTag("WINDOW_BK", &aColor);
  colo->AddTag("WINDOW_FRAME", &aColor);
  colo->AddTag("WINDOW_TEXT", &aColor);
  // insert into structList
  structList.push_back( colo);
  
 DStructDesc* dropstruct = new DStructDesc( "WIDGET_DROP");
  dropstruct->AddTag("ID", &aLong);
  dropstruct->AddTag("TOP", &aLong);
  dropstruct->AddTag("HANDLER", &aLong);
  dropstruct->AddTag("DRAG_ID", &aLong);
  dropstruct->AddTag("POSITION", &aInt);
  dropstruct->AddTag("X", &aLong);
  dropstruct->AddTag("Y", &aLong);
  dropstruct->AddTag("MODIFIERS", &aInt);
  // insert into structList
  structList.push_back( dropstruct); 
  
 DStructDesc* treeselstruct = new DStructDesc( "WIDGET_TREE_SEL");
  treeselstruct->AddTag("ID", &aLong);
  treeselstruct->AddTag("TOP", &aLong);
  treeselstruct->AddTag("HANDLER", &aLong);
  treeselstruct->AddTag("TYPE", &aInt);
  treeselstruct->AddTag("CLICKS", &aLong);
  // insert into structList
  structList.push_back( treeselstruct);
  
 DStructDesc* treeexpandstruct = new DStructDesc( "WIDGET_TREE_EXPAND");
  treeexpandstruct->AddTag("ID", &aLong);
  treeexpandstruct->AddTag("TOP", &aLong);
  treeexpandstruct->AddTag("HANDLER", &aLong);
  treeexpandstruct->AddTag("TYPE", &aInt);
  treeexpandstruct->AddTag("EXPAND", &aLong);
  // insert into structList
  structList.push_back( treeexpandstruct); 
  
 DStructDesc* idltracebackstruct = new DStructDesc( "IDL_TRACEBACK");
  idltracebackstruct->AddTag("ROUTINE", &aString);
  idltracebackstruct->AddTag("FILENAME", &aString);
  idltracebackstruct->AddTag("LINE", &aLong);
  idltracebackstruct->AddTag("LEVEL", &aLong);
  idltracebackstruct->AddTag("IS_FUNCTION", &aByte);
  idltracebackstruct->AddTag("METHOD", &aByte);
  idltracebackstruct->AddTag("RESTORED", &aByte);
  idltracebackstruct->AddTag("SYSTEM", &aByte);
  // insert into structList
  structList.push_back( idltracebackstruct); 

//template for future uses:
// DStructDesc* struct = new DStructDesc( "WIDGET_DROP");
//  struct->AddTag("ID", &aLong);
//  struct->AddTag("TOP", &aLong);
//  struct->AddTag("HANDLER", &aLong);
//  // insert into structList
//  structList.push_back( struct); 
}

void InitObjects()
{

  sigControlC = false;
  debugMode   = 0;

  fileUnits.resize( maxLun); // 0-127 -> 1-128 within GDL for files

  // initialize GDL system variables
  SysVar::InitSysVar();

  // initialize struct descriptors which are not system variables
  InitStructs();
  // add internal memeber subroutines
  SetupOverloadSubroutines();
  
  // graphic devices must be initialized after system variables.
  // !D must already exist
  // We need to initialize the multi-device object that inherits from the single-device object.
  GraphicsMultiDevice::Init();

  string gdlPath=GetEnvString("GDL_PATH");
  if( gdlPath == "") gdlPath=GetEnvString("IDL_PATH");
  if( gdlPath == "") gdlPath = "+" GDLDATADIR "/lib";
  SysVar::SetGDLPath( gdlPath);
}

// returns GDL lun, 0 on failure
DLong GetLUN()
{
  for( DLong lun=maxUserLun+1; lun <= fileUnits.size(); lun++)
    if( !fileUnits[ lun-1].InUse() && !fileUnits[ lun-1].GetGetLunLock())
    {
      fileUnits[ lun-1].SetGetLunLock( true);
      return lun;
    }
  
  return 0;
}
bool IsRelaxed(){return !strictInterpreter;}
void SetStrict(bool value){strictInterpreter=value;}

// for semantic predicate
bool IsFun(antlr::RefToken rT1)
{
  antlr::Token& T1=*rT1;

  // search for T1.getText() in function table and path
  string searchName=StrUpCase(T1.getText());

//  cout << "IsFun: Searching for: " << searchName << endl;

// Speeds up the process of finding (in gdlc.g) if a syntax like foo(bar) is a call to the function 'foo'
// or the 'bar' element of array 'foo'.
  LibFunListT::iterator p=find_if(libFunList.begin(),libFunList.end(),
			       Is_eq<DLibFun>(searchName));
  if( p != libFunList.end()) if( *p != NULL) return true;

  FunListT::iterator q=find_if(funList.begin(),funList.end(),
			       Is_eq<DFun>(searchName));
  if( q != funList.end()) if( *q != NULL) return true;

  //  cout << "Not found: " << searchName << endl;

  return false;
}

int ProIx(const string& n)
{
SizeT nF=proList.size();
for( SizeT i=0; i<nF; i++) if( Is_eq<DPro>(n)(proList[i])) 
  return (int)i;
return -1;
}

int FunIx(const string& n)
{
SizeT nF=funList.size();
for( SizeT i=0; i<nF; i++) if( Is_eq<DFun>(n)(funList[i]))
  return (int)i;
return -1;
}

int LibProIx(const string& n)
{
  SizeT nF=libProList.size();
  for( SizeT i=0; i<nF; i++) 
    {
      if( Is_eq<DLibPro>(n)(libProList[i])) return (int)i;
    }
  return -1;
}

int LibFunIx(const string& n)
{
  SizeT nF=libFunList.size();
  
  for( SizeT i=0; i<nF; i++) 
    {
      if( Is_eq<DLibFun>(n)(libFunList[i])) return (int)i;
    }
  return -1;
}

// returns the endian of the current machine
bool BigEndian()
{
  // a long should at least have two bytes
  // big endian -> msb first (msb is 0 here)
  static const unsigned long int s = 0x0001;
  static const bool bigEndian = !(*reinterpret_cast<const unsigned char*>( &s));
  return bigEndian;
}

// test---------------

void breakpoint()
{
  static SizeT num=1;
  cout << "objects.cpp: at breakpoint(): " << num << endl;
  num++;
}


#ifndef _OPENMP
int get_suggested_omp_num_threads() {
  return 1;
}
#endif

#if defined _OPENMP
int get_suggested_omp_num_threads() {

  int default_num_threads=1, suggested_num_threads=1;
  
  char* env_var_c;
  env_var_c = getenv ("OMP_NUM_THREADS");
  if(env_var_c) 
    {
      return atoi(env_var_c);
    }
  //    cout<<"OMP_NUM_THREADS is not defined"<<endl;
  
  //set number of threads for appropriate OS
  int avload = 0;
  int nbofproc = omp_get_num_procs();
  FILE *iff;
    
#if defined(__APPLE__) || defined(__MACH__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
  //  cout<<"is MAC/*BSD"<<endl;
  iff= popen("echo $(sysctl -n vm.loadavg|cut -d\" \" -f 3)", "r");
  if (!iff)
    {
      return default_num_threads;
    }
      
#elif defined(__linux__) || defined(__gnu_linux__) || defined(linux)
  //cout<<"is linux"<<endl;
  iff= popen("cat /proc/loadavg |cut -d\" \" -f3", "r");
  if (!iff)
    {
      return default_num_threads;
    }
   
#elif defined (__unix) || defined(__unix__)
  iff=freopen("/proc/loadavg","r",stderr);
  fclose(stderr);
  if(!iff)
    {
      cout<<"your OS is not supported"<<endl;
      return default_num_threads;
    }
  iff= popen("cat /proc/loadavg 2>/dev/null|cut -d\" \" -f3", "r");
  if (!iff)
    {
      return default_num_threads;
    }


#elif defined(_WIN32)
#if 0
  //cout<<"get_suggested_omp_num_threads(): is windows"<<endl;
  iff= _popen("wmic cpu get loadpercentage|more +1", "r");
  if (!iff)
    {
      return default_num_threads;
    }
  char buffer[4];
  char* c;
  c=fgets(buffer, sizeof(buffer), iff);
  _pclose(iff);
  if(!c)
    {
      return default_num_threads;
    }
  int count=0;
  while(count < sizeof(buffer) && buffer[count]!='\0' && buffer[count]!=' ')count++;
  for(int i=1,j=1;i<=count;i++,j*=10)
  {
    if( buffer[count-i] != ' ' && buffer[count-i] != '\t')
      avload+=(buffer[count-i]-'0')*j;
  }
  suggested_num_threads=nbofproc-(int)(avload*((float)nbofproc/100)+0.5);
  return suggested_num_threads;
#elif 1
   return nbofproc+2;
#endif
#else 
  cout<<"Can't define your OS"<<endl;
  return default_num_threads;
#endif

  //  cout << "Nb Procs.: " << nbofproc <<  endl;
  // cout << "nb Thead computed: " << nbofproc-(int)(avload+0.5) << endl;

  // if the following is commented out, there is no return statement
  // this lead to FILE_INFO in TEST_FILE_COPY fail
#if !defined(_WIN32)
  
  char buffer[4];
  char* c;
  c=fgets(buffer, sizeof(buffer), iff);
  pclose(iff);
  if(!c)
    {
      return default_num_threads;
    }
  //   cout<<buffer[0]<<" "<<buffer[1]<<endl;
  avload=(buffer[0]-'0')+((buffer[2]-'0')>5?1:0);

  suggested_num_threads=nbofproc-avload;
#endif  
  return suggested_num_threads;
}
#endif


