/***************************************************************************
                       initsysvar.cpp  -  definition of GDL system variables
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

#include <sys/utsname.h>
#include <cmath>
#include <sys/time.h>

#include <limits>

#include "objects.hpp"
#include "dstructgdl.hpp"
#include "graphics.hpp"

#include "file.hpp"

namespace SysVar
{

  using namespace std;

  // the index of some system variables
  UInt pathIx, promptIx, edit_inputIx, quietIx, 
    dIx, pIx, xIx, yIx, zIx, vIx, cIx, MouseIx,
    errorStateIx, errorIx, errIx, err_stringIx, valuesIx,
    journalIx, exceptIx, mapIx, cpuIx, dirIx, stimeIx, warnIx;

  // !D structs
  const int nDevices = 2;
  DStructGDL* devices[ 2]; // X, PS

  // !STIME
  const SizeT MAX_STIME_STRING_LENGTH=80;

  void SetGDLPath( const DString& newPath)
  {
    FileListT sArr;

    SizeT d;
    long   sPos=0;
    do
      {
	d=newPath.find(':',sPos);
	string act = newPath.substr(sPos,d-sPos);
	
	lib::ExpandPath( sArr, act, "*.pro");
	
	sPos=d+1;
      }
    while( d != newPath.npos);

    SizeT nArr = sArr.size();
    if( nArr == 0) return;

    // get the path
    DVar& pathSysVar=*sysVarList[pathIx];
    DString& path=static_cast<DStringGDL&>(*pathSysVar.Data())[0];

    // set the path
    path = sArr[0];
    for( SizeT i=1; i<nArr; ++i)
      path += ":" + sArr[i];
  }

  // returns !DIR (as a plain DString)
  const DString& Dir()
  {
    DVar& dirSysVar = *sysVarList[ dirIx];
    return static_cast<DStringGDL&>( *dirSysVar.Data())[0];
  }

  // updates !STIME (as a plain DString)
  void UpdateSTime()
  {
    DVar& stimeSysVar = *sysVarList[ stimeIx];
    DString& stime=static_cast<DStringGDL&>(*stimeSysVar.Data())[0];

    struct timeval tval;
    struct timezone tzone;
    struct tm *tstruct;

    gettimeofday(&tval,&tzone);
    tstruct= localtime((time_t *)&tval.tv_sec);

    char st[MAX_STIME_STRING_LENGTH];
    const char *format="%d-%h-%Y %T.00";// !STIME format.
    SizeT res=strftime( st, MAX_STIME_STRING_LENGTH, format, tstruct);

    stime = st;
  }

  // returns array of path strings
  const StrArr& GDLPath()
  {
    static StrArr sArr;
  
    // clear whatever old value is stored
    sArr.clear();
  
    // get the path
    DVar& pathSysVar=*sysVarList[pathIx];
    DString& path=static_cast<DStringGDL&>(*pathSysVar.Data())[0];
    
    if( path == "") return sArr;
  
    SizeT d;
    long   sPos=0;

    do
      {
	d=path.find(':',sPos);
	sArr.push_back(path.substr(sPos,d-sPos));
	sPos=d+1;
      }
    while( d != path.npos);

    return sArr;
  }

  const string& Prompt()
  {
    DVar& promptSysVar=*sysVarList[promptIx];
    return static_cast<DStringGDL&>(*promptSysVar.Data())[0];
  }

  int Edit_Input()
  {
    DVar& eiSysVar=*sysVarList[edit_inputIx];
    return static_cast<DIntGDL&>(*eiSysVar.Data())[0];
  }

  DLong Quiet()
  {
    DVar& qSysVar=*sysVarList[quietIx];
    return static_cast<DLongGDL&>(*qSysVar.Data())[0];
  }

  void SetC( DLong cVal)
  {
    DVar& cSysVar=*sysVarList[cIx];
    static_cast<DLongGDL&>(*cSysVar.Data())[0] = cVal;
  }

  void SetD( BaseGDL* newD)
  {
    DVar& dSysVar = *sysVarList[ dIx];
    dSysVar.Data() = newD;
  }

  void SetErr_String( const DString& eS)
  {
    DVar& eSSysVar = *sysVarList[ err_stringIx];
    static_cast<DStringGDL&>(*eSSysVar.Data())[0] = eS;
  }

  DStructGDL* P()
  {
    DVar& pSysVar = *sysVarList[ pIx];
    return static_cast<DStructGDL*>(pSysVar.Data());
  }

  DLongGDL* GetPMulti()
  {
    static DStructGDL* pStruct = SysVar::P();
    static int tag = pStruct->Desc()->TagIndex( "MULTI");
    return static_cast<DLongGDL*>( pStruct->GetTag( tag, 0));
  }

  DStructGDL* X()
  {
    DVar& var = *sysVarList[ xIx];
    return static_cast<DStructGDL*>(var.Data());
  }
  DStructGDL* Y()
  {
    DVar& var = *sysVarList[ yIx];
    return static_cast<DStructGDL*>(var.Data());
  }
  DStructGDL* Z()
  {
    DVar& var = *sysVarList[ zIx];
    return static_cast<DStructGDL*>(var.Data());
  }
  DStructGDL* Version()
  {
    DVar& var = *sysVarList[ vIx];
    return static_cast<DStructGDL*>(var.Data());
  }
  DStructGDL* Values()
  {
    DVar& var = *sysVarList[ valuesIx];
    return static_cast<DStructGDL*>(var.Data());
  }
  
  DStructGDL* Error_State()
  {
    DVar& var = *sysVarList[ errorStateIx];
    return static_cast<DStructGDL*>(var.Data());
  }

  DStructGDL* Map()
  {
    DVar& var = *sysVarList[ mapIx];
    return static_cast<DStructGDL*>(var.Data());
  }
  
  DStructGDL* Mouse()
  {
    DVar& var = *sysVarList[ MouseIx];
    return static_cast<DStructGDL*>(var.Data());
  }

  DStructGDL* Cpu()
  {
    DVar& var = *sysVarList[ cpuIx];
    return static_cast<DStructGDL*>(var.Data());
  }

  DStructGDL* D()
  {
    DVar& var = *sysVarList[ dIx];
    return static_cast<DStructGDL*>(var.Data());
  }

  DString MsgPrefix()
  {
    DStructGDL* errorState = Error_State();
    static unsigned msgTag = errorState->Desc()->TagIndex( "MSG_PREFIX");
    return (*static_cast<DStringGDL*>( errorState->GetTag( msgTag, 0)))[0];
  }

  DLong JournalLUN()
  {
    DVar& jSysVar=*sysVarList[journalIx];
    return static_cast<DLongGDL&>(*jSysVar.Data())[0];
  }
  void JournalLUN( DLong jLUN)
  {
    DVar& jSysVar=*sysVarList[journalIx];
    static_cast<DLongGDL&>(*jSysVar.Data())[0] = jLUN;
  }

  // call only once in main
  void InitSysVar()
  { 
    // for very sensitive compilers (which need a SizeT for dimension())
    const SizeT one=1;

    // !PATH
    //    DString initPath(""); // set here the initial path
    DStringGDL* pathData=new DStringGDL( "");
    DVar *path=new DVar( "PATH", pathData);
    pathIx=sysVarList.size();
    sysVarList.push_back(path);

    // !PROMPT
    DStringGDL* promptData=new DStringGDL( "GDL> ");
    DVar *prompt=new DVar( "PROMPT", promptData);
    promptIx=sysVarList.size();
    sysVarList.push_back(prompt);

    // !EDIT_INPUT
    DIntGDL* edit_inputData=new DIntGDL( 1);
    DVar *edit_input=new DVar( "EDIT_INPUT", edit_inputData);
    edit_inputIx=sysVarList.size();
    sysVarList.push_back(edit_input);

    // !QUIET
    DLongGDL* quietData=new DLongGDL( 0);
    DVar *quiet=new DVar( "QUIET", quietData);
    quietIx=sysVarList.size();
    sysVarList.push_back(quiet);

    // !C
    DLongGDL* cData=new DLongGDL( 0);
    DVar *c=new DVar( "C", cData);
    cIx=sysVarList.size();
    sysVarList.push_back(c);

    // !D defined in Graphics
    DVar *d=new DVar( "D", NULL);
    dIx=sysVarList.size();
    sysVarList.push_back(d);
    sysVarRdOnlyList.push_back( d); // make it read only

    // plotting
    // !P
    SizeT clipDim = 6;
    DLong p_clipInit[] = { 60, 40, 622, 492, 0, 1000};
    DLongGDL* p_clip = new DLongGDL( dimension( &clipDim, one));
    for( UInt i=0; i<clipDim; i++) (*p_clip)[ i] = p_clipInit[ i];
    SizeT multiDim = 5;
    SizeT positionDim = 4;
    SizeT regionDim = 4;
    SizeT tDim[] = { 4, 4};
    DStructGDL*  plt = new DStructGDL( "!PLT");
    plt->NewTag("BACKGROUND", new DLongGDL( 0)); 
    plt->NewTag("CHARSIZE", new DFloatGDL( 0.0)); 
    plt->NewTag("CHARTHICK", new DFloatGDL( 0.0)); 
    plt->NewTag("CLIP", p_clip); 
    plt->NewTag("COLOR", new DLongGDL( 255)); 
    plt->NewTag("FONT", new DLongGDL( -1)); 
    plt->NewTag("LINESTYLE", new DLongGDL( 0)); 
    plt->NewTag("MULTI", new DLongGDL( dimension( &multiDim, one))); 
    plt->NewTag("NOCLIP", new DLongGDL( 0)); 
    plt->NewTag("NOERASE", new DLongGDL( 0)); 
    plt->NewTag("NSUM", new DLongGDL( 0)); 
    plt->NewTag("POSITION", new DFloatGDL( dimension( &positionDim, one))); 
    plt->NewTag("PSYM", new DLongGDL( 0)); 
    plt->NewTag("REGION", new DFloatGDL( dimension( &regionDim, one))); 
    plt->NewTag("SUBTITLE", new DStringGDL( "")); 
    plt->NewTag("SYMSIZE", new DFloatGDL( 0.0)); 
    plt->NewTag("T", new DDoubleGDL( dimension( tDim, 2))); 
    plt->NewTag("T3D", new DLongGDL( 0)); 
    plt->NewTag("THICK", new DFloatGDL( 0.0)); 
    plt->NewTag("TITLE", new DStringGDL( "")); 
    plt->NewTag("TICKLEN", new DFloatGDL( 0.02)); 
    plt->NewTag("CHANNEL", new DLongGDL( 0)); 
    DVar *p=new DVar( "P", plt);
    pIx=sysVarList.size();
    sysVarList.push_back(p);

    // some constants

    // !GDL (to allow distinguish IDL/GDL with DEFSYSV, '!gdl', exists=exists )
    DStringGDL *gdlData = new DStringGDL( "GDL");
    DVar *gdl = new DVar( "GDL", gdlData);
    sysVarList.push_back( gdl);
    sysVarRdOnlyList.push_back( gdl);

    // !DPI
    DDoubleGDL *dpiData = new DDoubleGDL( (double)(4*atan(1.0)) );
    DVar *dpi = new DVar( "DPI", dpiData);
    sysVarList.push_back( dpi);
    sysVarRdOnlyList.push_back( dpi); // make it read only

    // !PI
    DFloatGDL *piData = new DFloatGDL( (float)(4*atan(1.0)) );
    DVar *pi = new DVar( "PI", piData);
    sysVarList.push_back( pi);
    sysVarRdOnlyList.push_back( pi);

    // !DTOR
    DFloatGDL *dtorData = new DFloatGDL( 0.0174533);
    DVar *dtor = new DVar( "DTOR", dtorData);
    sysVarList.push_back( dtor);
    sysVarRdOnlyList.push_back( dtor);

    // !RADEG
    DFloatGDL *radegData = new DFloatGDL( 57.2957764);
    DVar *radeg = new DVar( "RADEG", radegData);
    sysVarList.push_back( radeg);
    sysVarRdOnlyList.push_back( radeg);

    // ![XYZ]
    SizeT dim2  = 2;
    SizeT dim60 = 60;
    SizeT dim10 = 10;
    DStructGDL*  xAxis = new DStructGDL( "!AXIS");
    xAxis->NewTag("TITLE", new DStringGDL( "")); 
    xAxis->NewTag("TYPE", new DLongGDL( 0)); 
    xAxis->NewTag("STYLE", new DLongGDL( 0)); 
    xAxis->NewTag("TICKS", new DLongGDL( 0)); 
    xAxis->NewTag("TICKLEN", new DFloatGDL( 0.0)); 
    xAxis->NewTag("THICK", new DFloatGDL( 0.0)); 
    xAxis->NewTag("RANGE", new DDoubleGDL( dimension( &dim2,one))); 
    xAxis->NewTag("CRANGE", new DDoubleGDL( dimension( &dim2,one))); 
    xAxis->NewTag("S", new DDoubleGDL( dimension( &dim2,one))); 
    xAxis->NewTag("MARGIN", new DFloatGDL( dimension( &dim2,one))); 
    xAxis->NewTag("OMARGIN", new DFloatGDL( dimension( &dim2,one))); 
    xAxis->NewTag("WINDOW", new DFloatGDL( dimension( &dim2,one))); 
    xAxis->NewTag("REGION", new DFloatGDL( dimension( &dim2,one))); 
    xAxis->NewTag("CHARSIZE", new DFloatGDL( 0.0)); 
    xAxis->NewTag("MINOR", new DLongGDL( 0)); 
    xAxis->NewTag("TICKV", new DDoubleGDL( dimension( &dim60,one))); 
    xAxis->NewTag("TICKNAME", new DStringGDL( dimension( &dim60,one))); 
    xAxis->NewTag("GRIDSTYLE", new DLongGDL( 0)); 
    xAxis->NewTag("TICKFORMAT", new DStringGDL( dimension( &dim10,one))); 
    xAxis->NewTag("TICKINTERVAL", new DDoubleGDL( 0)); 
    xAxis->NewTag("TICKLAYOUT", new DLongGDL( 0)); 
    xAxis->NewTag("TICKUNITS", new DStringGDL( dimension( &dim10,one))); 
    (*static_cast<DDoubleGDL*>( xAxis->GetTag( 8, 0)))[1] = 1.0;
    (*static_cast<DFloatGDL*>(  xAxis->GetTag( 9, 0)))[0] = 10.0;
    (*static_cast<DFloatGDL*>(  xAxis->GetTag( 9, 0)))[1] = 3.0;
    DVar *x            = new DVar( "X", xAxis);
    xIx                = sysVarList.size();
    sysVarList.push_back(x);
    DStructGDL*  yAxis = new DStructGDL( "!AXIS");
    (*static_cast<DDoubleGDL*>( yAxis->GetTag( 8, 0)))[1] = 1.0;
    (*static_cast<DFloatGDL*>(  yAxis->GetTag( 9, 0)))[0] = 4.0;
    (*static_cast<DFloatGDL*>(  yAxis->GetTag( 9, 0)))[1] = 2.0;
    DVar*        y     = new DVar( "Y", yAxis);
    yIx                = sysVarList.size();
    sysVarList.push_back(y);
    DStructGDL*  zAxis = new DStructGDL( "!AXIS");
    (*static_cast<DDoubleGDL*>( zAxis->GetTag( 8, 0)))[1] = 1.0;
    DVar*        z     = new DVar( "Z", zAxis);
    zIx                = sysVarList.size();
    sysVarList.push_back(z);

    // !VERSION
    DStructGDL*  ver = new DStructGDL( "!VERSION");
    struct utsname uts;
    uname(&uts);
    ver->NewTag("ARCH", new DStringGDL( uts.machine)); 
    ver->NewTag("OS", new DStringGDL( uts.sysname)); 
    ver->NewTag("OS_FAMILY", new DStringGDL( "unix")); 
    ver->NewTag("OS_NAME", new DStringGDL( uts.sysname)); 
    ver->NewTag("RELEASE", new DStringGDL( "6.0")); 
    ver->NewTag("BUILD_DATE", new DStringGDL( "Jul 07 2005")); 
    ver->NewTag("MEMORY_BITS", new DIntGDL( 32)); 
    ver->NewTag("FILE_OFFSET_BITS", new DIntGDL( 64)); 
    DVar *v            = new DVar( "VERSION", ver);
    vIx                = sysVarList.size();
    sysVarList.push_back(v);
    sysVarRdOnlyList.push_back(v);

    // !Mouse
    DStructGDL*  MouseData = new DStructGDL( "!MOUSE");
    MouseData->NewTag("X", new DLongGDL( 0));
    MouseData->NewTag("Y", new DLongGDL( 0));
    MouseData->NewTag("BUTTON", new DLongGDL( 0));
    MouseData->NewTag("TIME", new DLongGDL( 0));
    DVar *Mouse      = new DVar( "MOUSE", MouseData);
    MouseIx          = sysVarList.size();
    sysVarList.push_back(Mouse);


    // !ERROR_STATE
    DStructGDL*  eStateData = new DStructGDL( "!ERROR_STATE");
    eStateData->NewTag("NAME", new DStringGDL( "IDL_M_SUCCESS")); 
    eStateData->NewTag("BLOCK", new DStringGDL( "IDL_MBLK_CORE")); 
    eStateData->NewTag("CODE", new DLongGDL( 0)); 
    eStateData->NewTag("RANGE", new DLongGDL( dimension( &dim2,one))); 
    eStateData->NewTag("SYS_CODE_TYPE", new DStringGDL( "")); 
    eStateData->NewTag("MSG", new DStringGDL( "")); 
    eStateData->NewTag("SYS_MSG", new DStringGDL( "")); 
    eStateData->NewTag("MSG_PREFIX", new DStringGDL( "% ")); 
    DVar *eState       = new DVar( "ERROR_STATE", eStateData);
    errorStateIx       = sysVarList.size();
    sysVarList.push_back(eState);
    //    sysVarRdOnlyList.push_back(eState);

    // !ERROR
    DLongGDL *errorData = new DLongGDL( 0 );
    DVar *errorVar = new DVar( "ERROR", errorData );
    errorIx            = sysVarList.size();
    sysVarList.push_back( errorVar);
    sysVarRdOnlyList.push_back( errorVar);

    // !ERR
    DLongGDL *errData = new DLongGDL( 0 );
    DVar *errVar = new DVar( "ERR", errData );
    errIx              = sysVarList.size();
    sysVarList.push_back( errVar );
    //    sysVarRdOnlyList.push_back( errVar);

    // !ERR_STRING
    DStringGDL *err_stringData = new DStringGDL( "");
    DVar *err_stringVar = new DVar( "ERR_STRING", err_stringData );
    err_stringIx        = sysVarList.size();
    sysVarList.push_back( err_stringVar );
    sysVarRdOnlyList.push_back( err_stringVar);

    // !VALUES
    DStructGDL*  valuesData = new DStructGDL( "!VALUES");
    if( std::numeric_limits< DFloat>::has_infinity)
      {
	valuesData->NewTag("F_INFINITY", 
			   new DFloatGDL( std::numeric_limits< DFloat>::infinity())); 
      }
    else
      {
	valuesData->NewTag("F_INFINITY", new DFloatGDL((float)1.0/0.0)); 
      }

    valuesData->NewTag("F_NAN", new DFloatGDL(-sqrt((float) -1.0))); 

    if( std::numeric_limits< DDouble>::has_infinity)
      {
	valuesData->NewTag("D_INFINITY", 
			   new DDoubleGDL( std::numeric_limits< DDouble>::infinity())); 
      }
    else
      {
	valuesData->NewTag("D_INFINITY", new DDoubleGDL( (double)1.0/0.0)); 
      }

    valuesData->NewTag("D_NAN", new DDoubleGDL(-sqrt((double) -1.0)));
    DVar *values       = new DVar( "VALUES", valuesData);
    valuesIx           = sysVarList.size();
    sysVarList.push_back(values);
    sysVarRdOnlyList.push_back( values);

    // !JOURNAL hold journal file lun
    DLongGDL *journalData = new DLongGDL( 0);
    DVar *journal = new DVar( "JOURNAL", journalData);
    journalIx     = sysVarList.size();
    sysVarList.push_back( journal);
    sysVarRdOnlyList.push_back( journal);

    // !EXCEPT
    DIntGDL *exceptData = new DIntGDL( 1);
    DVar *except = new DVar( "EXCEPT", exceptData);
    exceptIx=sysVarList.size();
    sysVarList.push_back( except);

    // !MAP
    DStructGDL* mapData = new DStructGDL( "!MAP");
    mapData->NewTag("PROJECTION", new DLongGDL( 0)); 
    mapData->NewTag("SIMPLE", new DLongGDL( 0)); 
    mapData->NewTag("FILL_METHOD", new DLongGDL( 0)); 
    mapData->NewTag("UP_FLAGS", new DLongGDL( 0)); 
    mapData->NewTag("UP_NAME", new DStringGDL( "")); 
    mapData->NewTag("P0LON", new DDoubleGDL( 0.0));  
    mapData->NewTag("P0LAT", new DDoubleGDL( 0.0));  
    mapData->NewTag("U0", new DDoubleGDL( 0.0));  
    mapData->NewTag("V0", new DDoubleGDL( 0.0));  
    mapData->NewTag("SINO", new DDoubleGDL( 0.0));  
    mapData->NewTag("COSO", new DDoubleGDL( 0.0));  
    mapData->NewTag("ROTATION", new DDoubleGDL( 0.0));
    mapData->NewTag("SINR", new DDoubleGDL( 0.0));  
    mapData->NewTag("COSR", new DDoubleGDL( 0.0));    
    mapData->NewTag("A", new DDoubleGDL( 0.0));    
    mapData->NewTag("E2", new DDoubleGDL( 0.0));    
    mapData->NewTag("UV", new DDoubleGDL( dimension( 2)));
    mapData->NewTag("POLE", new DDoubleGDL( dimension( 7)));
    mapData->NewTag("UV_BOX", new DDoubleGDL( dimension( 4)));
    mapData->NewTag("LL_BOX", new DDoubleGDL( dimension( 4)));
    mapData->NewTag("SEGMENT_LENGTH", new DDoubleGDL( 0.0));  
    mapData->NewTag("P", new DDoubleGDL( dimension( 16)));  
    mapData->NewTag("PIPELINE", new DDoubleGDL( dimension( 8, 12)));  

    DVar *map=new DVar( "MAP", mapData);
    mapIx=sysVarList.size();
    sysVarList.push_back( map);

    // !CPU
    DStructGDL* cpuData = new DStructGDL( "!CPU");
    cpuData->NewTag("HW_VECTOR", new DLongGDL( 0)); 
    cpuData->NewTag("VECTOR_ENABLE", new DLongGDL( 0)); 
    cpuData->NewTag("HW_NCPU", new DLongGDL( 0)); 
    cpuData->NewTag("TPOOL_NTHREADS", new DLongGDL( 0)); 
    cpuData->NewTag("TPOOL_MIN_ELTS", new DLongGDL( 0)); 
    cpuData->NewTag("TPOOL_MAX_ELTS", new DLongGDL( 0)); 

    DVar *cpu=new DVar( "CPU", cpuData);
    cpuIx=sysVarList.size();
    sysVarList.push_back( cpu);

    // !DIR
#ifndef EXEC_PREFIX
#define EXEC_PREFIX ""
#endif
    DStringGDL *dirData = new DStringGDL( EXEC_PREFIX);
    string gdlDir=GetEnvString("GDL_DIR");
    if( gdlDir == "") gdlDir=GetEnvString("IDL_DIR");
    if( gdlDir != "") dirData = new DStringGDL( gdlDir);
    DVar *dir = new DVar( "DIR", dirData);
    dirIx=sysVarList.size();
    sysVarList.push_back( dir);

    // !STIME
    DStringGDL *stimeData = new DStringGDL( "");
    DVar *stime = new DVar( "STIME", stimeData);
    stimeIx=sysVarList.size();
    sysVarList.push_back( stime);
    sysVarRdOnlyList.push_back( stime); // make it read only

    // !WARN
    DStructGDL*  warnData = new DStructGDL( "!WARN");
    warnData->NewTag("OBS_ROUTINES", new DByteGDL( 0)); 
    warnData->NewTag("OBS_SYSVARS", new DByteGDL( 0)); 
    warnData->NewTag("PARENS", new DByteGDL( 0)); 
    DVar *warn = new DVar( "WARN", warnData);
    warnIx     = sysVarList.size();
    sysVarList.push_back(warn);

  }

}
