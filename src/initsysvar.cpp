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
#include "version.hpp"

#ifndef _WIN32
#include <sys/utsname.h>
#endif
#include <cmath>

#include <limits>

#ifdef _OPENMP
#include <omp.h> // for !CPU
#endif

//#include "nullgdl.hpp"

#include "objects.hpp"
#include "dstructgdl.hpp"
#include "graphicsdevice.hpp"

#include "file.hpp" //for ExpandPath

#ifdef _MSC_VER
#include "gtdhelper.hpp"
#else
#include <sys/time.h>
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__
#endif

namespace SysVar
{

  using namespace std;
  //a static float[4] holding !SC[1..4] values
  static DFloat sc[4];
  DFloat* GetSC()
  {
    return sc;
  }
  // the index of some system variables
  UInt nullIx, trueIx, falseIx, pathIx, dlmpathIx, helppathIx, promptIx, edit_inputIx, quietIx, moreIx,
    dIx, pIx, xIx, yIx, zIx, vIx, gdlIx, cIx, MouseIx,
    errorStateIx, errorIx, errIx, err_stringIx, valuesIx,
    journalIx, exceptIx, mapIx, cpuIx, dirIx, stimeIx,
    warnIx, usersymIx, orderIx, MakeDllIx, colorIx;
  // the index of some obsolete system variables
  UInt xdottitleIx,ydottitleIx,xdotticksIx,ydotticksIx,cxminIx,cxmaxIx,cyminIx,cymaxIx,
    xminIx,xmaxIx,yminIx,ymaxIx,pdotnoclipIx,pdotlinestyleIx,pdottitleIx,pdotnoeraseIx,pdotnsumIx,
    pdotpsymIx,errorstatedoterrorIx,errorstatedotmsgprefixIx,errorstatesyscodeIx,pdotticklenIx,
    fancyIx,gridIx,typeIx,sc1Ix,sc2Ix,sc3Ix,sc4Ix; 
  // !D structs
  const int nDevices = 5;
  DStructGDL* devices[ 5]; // NULL, PS, Z, SVG, (X or WIN or nothing)

  // !STIME
  const SizeT MAX_STIME_STRING_LENGTH=80;
  
  bool IsSTime( DVar* var)
  { 
// due to .RESET_SESSION we cannot use static here
// but the effect will be minimal anyway    
//     static DVar* varSTime = sysVarList[ stimeIx];
    return var == sysVarList[ stimeIx];
  }
   
//  bool IsD( DVar* var)
//  { 
//    return var == sysVarList[ dIx];
//  }

  void SetGDLPath( DString& newPath)
  {
    FileListT sArr;
    SizeT d;
    long   sPos=0;
    do
      {
	d=newPath.find(lib::SearchPathSeparator(),sPos);
	string act = newPath.substr(sPos,d-sPos);
    
    if (newPath.find("<IDL_DEFAULT_PATH", 0) != std::string::npos) newPath = replaceAllOccurencesOfDefaultTokens(newPath, "<IDL_DEFAULT_PATH>", gdl_default_path);	
    if (newPath.find("<IDL_DEFAULT", 0) != std::string::npos) newPath = replaceAllOccurencesOfDefaultTokens(newPath, "<IDL_DEFAULT>", gdl_default_path);	
	
	lib::ExpandPath( sArr, act, "*.pro"); //indeed, !PATH contains only directories where a .pro is found
	
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
      path += lib::SearchPathSeparator() + sArr[i];
    // GJ version    path = sArr[nArr-1];
    // GJ version for( SizeT i=1; i<nArr; ++i)
    // GJ version  path += pathsep + sArr[nArr-i-1];
  }
  
  void SetDlmPath( DString& newPath)
  {
    FileListT sArr;
    SizeT d;
    long   sPos=0;
    do
      {
	d=newPath.find(lib::SearchPathSeparator(),sPos);
	string act = newPath.substr(sPos,d-sPos);

    if (newPath.find("<IDL_DEFAULT_DLM", 0) != std::string::npos) newPath = replaceAllOccurencesOfDefaultTokens(newPath, "<IDL_DEFAULT_DLM>", gdl_default_dlm);	
	lib::ExpandPath( sArr, act, "*.dlm", true); //ALL_DIRS since DLM_PATH contains all dirs, not only those with a valid .dlm
	
	sPos=d+1;
      }
    while( d != newPath.npos);

    SizeT nArr = sArr.size();
    if( nArr == 0) return;

    // get the path
    DVar& dlmpathSysVar=*sysVarList[dlmpathIx];
    DString& dlmpath=static_cast<DStringGDL&>(*dlmpathSysVar.Data())[0];

    // set the path
    dlmpath = sArr[0];
    for( SizeT i=1; i<nArr; ++i)
      dlmpath += lib::SearchPathSeparator() + sArr[i];
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
    time_t ttime = tval.tv_sec;
    tstruct= localtime(&ttime);

    char st[MAX_STIME_STRING_LENGTH];
    const char *format="%d-%b-%Y %H:%M:%S.00";// !STIME format.
    SizeT res=strftime( st, MAX_STIME_STRING_LENGTH, format, tstruct);

    stime = st;
  }

  DStructGDL* D()
  {
    DVar& var = *sysVarList[ dIx];
    return static_cast<DStructGDL*>(var.Data());
  }

//  // updates !D in all modes (should insure correct update if Win, XWidgets ...)
//  void UpdateD()
//  {
//    DStructGDL* dD = D();
//
//    DLong windowIdx=(*static_cast<DLongGDL*>(dD->GetTag(dD->Desc()->TagIndex("WINDOW"), 0)))[0];
//    if( windowIdx <0)  return;    
//    
//    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
//    GDLGStream* actStream = actDevice->GetStream();
//
//    long xSizeGG,ySizeGG;
//    actStream->GetGeometry(xSizeGG,ySizeGG);
//    int debug=0;
//    if (debug) cout << "GetX11Geo in SysVar::UpdateD : " << xSizeGG <<" "<< ySizeGG << endl;
//    
//    (*static_cast<DLongGDL*>(dD->GetTag(dD->Desc()->TagIndex("X_SIZE"), 0)))[0] = xSizeGG;
//    (*static_cast<DLongGDL*>(dD->GetTag(dD->Desc()->TagIndex("Y_SIZE"), 0)))[0] = ySizeGG;
//    (*static_cast<DLongGDL*>(dD->GetTag(dD->Desc()->TagIndex("X_VSIZE"), 0)))[0] = xSizeGG;
//    (*static_cast<DLongGDL*>(dD->GetTag(dD->Desc()->TagIndex("Y_VSIZE"), 0)))[0] = ySizeGG;	    
//  }

  // returns array of path strings

  const StrArr& GDLPath() {
    static StrArr sArr;

    // clear whatever old value is stored
    sArr.clear();

    // get the path
    DVar& pathSysVar = *sysVarList[pathIx];
    DString& path = static_cast<DStringGDL&> (*pathSysVar.Data())[0];

    if (path == "") return sArr;

    SizeT d;
    long sPos = 0;

    do {
      d = path.find(lib::SearchPathSeparator(), sPos);
      sArr.push_back(path.substr(sPos, d - sPos));
      sPos = d + 1;
    }    while (d != path.npos);

    return sArr;
  }
  const StrArr& DLMPath() {
    static StrArr sArr;

    // clear whatever old value is stored
    sArr.clear();

    // get the path
    DVar& dlmpathSysVar = *sysVarList[dlmpathIx];
    DString& dlmpath = static_cast<DStringGDL&> (*dlmpathSysVar.Data())[0];

    if (dlmpath == "") return sArr;

    SizeT d;
    long sPos = 0;

    do {
      d = dlmpath.find(lib::SearchPathSeparator(), sPos);
      sArr.push_back(dlmpath.substr(sPos, d - sPos));
      sPos = d + 1;
    }    while (d != dlmpath.npos);

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
  void Suppress_Edit_Input()
  {
    DVar& eiSysVar=*sysVarList[edit_inputIx];
    static_cast<DIntGDL&>(*eiSysVar.Data())[0]=0;
  }

  DLong Quiet()
  {
    DVar& qSysVar=*sysVarList[quietIx];
    return static_cast<DLongGDL&>(*qSysVar.Data())[0];
  }
  
  DInt More()
  {
    DVar& qSysVar=*sysVarList[moreIx];
    return static_cast<DIntGDL&>(*qSysVar.Data())[0];
  }
  
  void Make_Quiet()
  {
    DVar& qSysVar=*sysVarList[quietIx];
    static_cast<DLongGDL&>(*qSysVar.Data())[0]=1;
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
  void SetErrError( DLong eC)
  {
    DVar& errSysVar = *sysVarList[ errIx];
    DVar& errorSysVar = *sysVarList[ errorIx];
    static_cast<DLongGDL&>(*errSysVar.Data())[0] = eC;
    static_cast<DLongGDL&>(*errorSysVar.Data())[0] = eC;
  }

  DStructGDL* P()
  {
    DVar& pSysVar = *sysVarList[ pIx];
    return static_cast<DStructGDL*>(pSysVar.Data());
  }
  void PFancyCallBack(){
    //1) adjust FANCY (initial motive)
    DIntGDL* fancy=GetFancy();
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset
    DFloat charsizePos=(*static_cast<DFloatGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0];
    (*fancy)[0]=charsizePos*5-4;
    //2) call change of font for interested drivers.
    static DLong opfont=-1;
    DLong pfont=(*static_cast<DLongGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("FONT"), 0)))[0];
    if (opfont != pfont) { //optimize number of calls
      opfont=pfont;
      GraphicsDevice::GetDevice()->FontChanged();
    }
  }
  DLongGDL* GetPMulti()
  {
    DStructGDL* pStruct = SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    static int tag = pStruct->Desc()->TagIndex( "MULTI");
    return static_cast<DLongGDL*>( pStruct->GetTag( tag, 0));
  }

  DLong GetPFont()
  {
    DStructGDL* pStruct = SysVar::P();   //MUST NOT BE STATIC, due to .reset 
    static int tag = pStruct->Desc()->TagIndex( "FONT");
    return (*static_cast<DLongGDL*>( pStruct->GetTag( tag)))[0];
  }

  DStringGDL* STime()
  {
    DVar& var = *sysVarList[ stimeIx];
    return static_cast<DStringGDL*>(var.Data());
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
  void SetFakeRelease( DString release)
  {
    DVar& var = *sysVarList[ vIx];
    DStructGDL *s=static_cast<DStructGDL*>(var.Data());
    static int tag = s->Desc()->TagIndex( "RELEASE");
    (*static_cast<DStringGDL*>( s->GetTag( tag)))[0] = release;
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
  DIntGDL* GetFancy()
  {
    DVar& var = *obsoleteSysVarList[ fancyIx];
    return static_cast<DIntGDL*>(var.Data());
  }
  void CBGridToTicklen(){
    DVar& var = *obsoleteSysVarList[ gridIx];
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset
    DFloat* ticklen=&((*static_cast<DFloatGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("TICKLEN"), 0)))[0]);
    *ticklen=((*static_cast<DIntGDL*>(var.Data()))[0] > 0)?1.0:0.02;    
  }
  void CBFancyToCharsize(){  //makes connection between !FANCY and !P.CHARSIZE
    DIntGDL* fancy=GetFancy();
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset
    DFloat* charsize=&((*static_cast<DFloatGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("CHARSIZE"), 0)))[0]);
    *charsize=(*fancy)[0]*0.2+0.8;
  }
  void CBTypeToXandY(){
    DStructGDL* xStruct=SysVar::X();   //MUST NOT BE STATIC, due to .reset 
    DStructGDL* yStruct=SysVar::Y();   //MUST NOT BE STATIC, due to .reset 
    DStructGDL* pStruct=SysVar::P();   //MUST NOT BE STATIC, due to .reset
    unsigned xtypeTag=xStruct->Desc()->TagIndex("TYPE");
    unsigned ytypeTag=yStruct->Desc()->TagIndex("TYPE");
    unsigned xstyleTag=xStruct->Desc()->TagIndex("STYLE");
    unsigned ystyleTag=yStruct->Desc()->TagIndex("STYLE");
    DLong    *xtype= &(*static_cast<DLongGDL*>(xStruct->GetTag(xtypeTag, 0)))[0];
    DLong    *ytype= &(*static_cast<DLongGDL*>(yStruct->GetTag(ytypeTag, 0)))[0];
    DLong   *xstyle= &(*static_cast<DLongGDL*>(xStruct->GetTag(xstyleTag, 0)))[0];
    DLong   *ystyle= &(*static_cast<DLongGDL*>(yStruct->GetTag(ystyleTag, 0)))[0];
    DFloat* ticklen= &((*static_cast<DFloatGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("TICKLEN"), 0)))[0]);
    DVar& var = *obsoleteSysVarList[ typeIx];
    DInt value=(*static_cast<DIntGDL*>(var.Data()))[0];
    *xtype=(value & 1 );
    *ytype=(value & 2 )>>1;
    *xstyle=(value & 4 )>>2;
    *ystyle=(value & 8 )>>3;
    *ticklen=( value & 16 )>>4; 
    *xstyle|=(value & 32 )>>3;
    *ystyle|=(value & 64 )>>4;
  }
  void CBPath() { //could be interesting to know if path has changed. may be used to speedup procedures fining & compilation?
//    std::cerr<<"PATH changed.\n";
  }
   void CBDlmPath() { //could be interesting to know if path has changed. may be used to speedup procedures fining & compilation?
//    std::cerr<<"PATH changed.\n";
  }
  DStructGDL* Mouse()
  {
    DVar& var = *sysVarList[ MouseIx];
    return static_cast<DStructGDL*>(var.Data());
  }

  // returns !GDL
  DStructGDL* GDLconfig()
  {
    DVar* sysVarList_gdlIx = sysVarList[ gdlIx];
    return static_cast<DStructGDL*>(sysVarList_gdlIx->Data());    
  }

  DStructGDL* Cpu()
  {
    DVar* sysVarList_cpuIx = sysVarList[ cpuIx];
    return static_cast<DStructGDL*>(sysVarList_cpuIx->Data());
  }

  DStructGDL* Warn()
  {
    DVar* sysVarList_warnIx = sysVarList[ warnIx];
    return static_cast<DStructGDL*>(sysVarList_warnIx->Data());
  }
  DIntGDL* Except()
  {
    DVar* sysVarList_exceptIx = sysVarList[ exceptIx];
    return static_cast<DIntGDL*>(sysVarList_exceptIx->Data());
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
  
  DLong TV_ORDER()
  {
    DVar& orderVar=*sysVarList[orderIx];
    return static_cast<DLongGDL&>(*orderVar.Data())[0];
  }

  // call only once in main
  void InitSysVar()
  { 
    // for very sensitive compilers (which need a SizeT for dimension())
    const SizeT one=1;

    // !NULL
    NullGDL* nullInstance = NullGDL::GetSingleInstance();
    DVar *nullVar = new DVar( "NULL", nullInstance);
    nullIx=sysVarList.size();
    sysVarList.push_back(nullVar);
    sysVarNoSaveList.push_back(nullVar);

    // !TRUE
    DByteGDL* trueData = new DByteGDL(1);
    DVar *true_logical = new DVar( "TRUE", trueData );
    trueIx=sysVarList.size();
    sysVarList.push_back(true_logical);
    sysVarNoSaveList.push_back(true_logical);

    // !FALSE
    DByteGDL* falseData = new DByteGDL(0);
    DVar *false_logical = new DVar( "FALSE", falseData );
    falseIx=sysVarList.size();
    sysVarList.push_back(false_logical);
    sysVarNoSaveList.push_back(false_logical);
    
    // !PATH
    //    DString initPath(""); // set here the initial path
    DStringGDL* pathData=new DStringGDL( "");
    DVar *path=new DVar( "PATH", pathData);path->SetCallback(CBPath);
    pathIx=sysVarList.size();
    sysVarList.push_back(path);

    // !DLM_PATH
    DStringGDL* dlmpathData=new DStringGDL( "");
    DVar *dlmpath=new DVar( "DLM_PATH", dlmpathData);dlmpath->SetCallback(CBDlmPath);
    dlmpathIx=sysVarList.size();
    sysVarList.push_back(dlmpath);

    // !HELP_PATH
    //    DString initHelpPath(""); // set here the initial path
    DStringGDL* helppathData=new DStringGDL( "");
    DVar *helppath=new DVar( "HELP_PATH", helppathData);
    helppathIx=sysVarList.size();
    sysVarList.push_back(helppath);
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
    
    // !MORE
    DIntGDL* moreData=new DIntGDL( 1);
    DVar *more=new DVar( "MORE", moreData);
    moreIx=sysVarList.size();
    sysVarList.push_back(more);

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
    sysVarNoSaveList.push_back( d); // make it read only

    // plotting
    // !P
    SizeT clipDim = 6;
//    DLong p_clipInit[] = { 0, 0, 1024, 1024, 0, 1000};
    DLong p_clipInit[] = { 0, 0, 639, 511, 0, 0};
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
    plt->NewTag("LINESTYLE", new DLongGDL( 0)); //tag 6 for obsolete !LINETYPE
    plt->NewTag("MULTI", new DLongGDL( dimension( &multiDim, one))); 
    plt->NewTag("NOCLIP", new DLongGDL( 0));  //tag 8 for obsolete !IGNORE
    plt->NewTag("NOERASE", new DLongGDL( 0)); //tag 9 for obsolete !NOERAS
    plt->NewTag("NSUM", new DLongGDL( 0)); //tag 10 for obsolete !NSUM
    plt->NewTag("POSITION", new DFloatGDL( dimension( &positionDim, one))); 
    plt->NewTag("PSYM", new DLongGDL( 0)); //tag 12 for obsolete !PSYM
    plt->NewTag("REGION", new DFloatGDL( dimension( &regionDim, one))); 
    plt->NewTag("SUBTITLE", new DStringGDL( "")); 
    plt->NewTag("SYMSIZE", new DFloatGDL( 0.0)); 
    {
      DDoubleGDL* tmp = new DDoubleGDL( dimension( tDim, 2));
      (*tmp)[0] = (*tmp)[5] = (*tmp)[10] = (*tmp)[15] = 1;
      plt->NewTag("T", tmp); 
    }
    plt->NewTag("T3D", new DLongGDL( 0)); 
    plt->NewTag("THICK", new DFloatGDL( 0.0)); 
    plt->NewTag("TITLE", new DStringGDL( ""));  //tag 19 for obsolete !MTITLE
    plt->NewTag("TICKLEN", new DFloatGDL( 0.02)); 
    plt->NewTag("CHANNEL", new DLongGDL( 0)); 
    DVar *p=new DVar( "P", plt); p->SetCallback(PFancyCallBack);
    pIx=sysVarList.size();
    sysVarList.push_back(p);

    //very old obsolete variables
    DLongGDL* gdlpdotnoclip = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    gdlpdotnoclip->SetBuffer((void*) &((*static_cast<DLongGDL*> (plt->GetTag(8, 0)))[0]));
    gdlpdotnoclip->SetBufferSize(1);
    gdlpdotnoclip->SetDim(dimension(1));
    DVar *pdotnoclip = new DVar("IGNORE", gdlpdotnoclip);
    pdotnoclipIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(pdotnoclip);
    DLongGDL* gdlpdotlinestyle = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    gdlpdotlinestyle->SetBuffer((void*) &((*static_cast<DLongGDL*> (plt->GetTag(6, 0)))[0]));
    gdlpdotlinestyle->SetBufferSize(1);
    gdlpdotlinestyle->SetDim(dimension(1));
    DVar *pdotlinestyle = new DVar("LINETYPE", gdlpdotlinestyle);
    pdotlinestyleIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(pdotlinestyle);
    DLongGDL* gdlpdotnoerase = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    gdlpdotnoerase->SetBuffer((void*) &((*static_cast<DLongGDL*> (plt->GetTag(9, 0)))[0]));
    gdlpdotnoerase->SetBufferSize(1);
    gdlpdotnoerase->SetDim(dimension(1));
    DVar *pdotnoerase = new DVar("NOERAS", gdlpdotnoerase);
    pdotnoeraseIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(pdotnoerase);
    DLongGDL* gdlpdotnsum = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    gdlpdotnsum->SetBuffer((void*) &((*static_cast<DLongGDL*> (plt->GetTag(10, 0)))[0]));
    gdlpdotnsum->SetBufferSize(1);
    gdlpdotnsum->SetDim(dimension(1));
    DVar *pdotnsum = new DVar("NSUM", gdlpdotnsum);
    pdotnsumIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(pdotnsum);
    DLongGDL* gdlpdotpsym = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    gdlpdotpsym->SetBuffer((void*) &((*static_cast<DLongGDL*> (plt->GetTag(12, 0)))[0]));
    gdlpdotpsym->SetBufferSize(1);
    gdlpdotpsym->SetDim(dimension(1));
    DVar *pdotpsym = new DVar("PSYM", gdlpdotpsym);
    pdotpsymIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(pdotpsym);
   
    DStringGDL* gdlpdottitle = new DStringGDL(dimension(1), BaseGDL::NOALLOC);
    gdlpdottitle->SetBuffer((void*) &((*static_cast<DStringGDL*> (plt->GetTag(19, 0)))[0]));
    gdlpdottitle->SetBufferSize(1);
    gdlpdottitle->SetDim(dimension(1));
    DVar *pdottitle = new DVar("MTITLE", gdlpdottitle);
    pdottitleIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(pdottitle);
    //FANCY --> must be interpreted it looks like: !P.CHARSIZE = 0.206 * !FANCY + 0.73 (probably it's just 0.2)
    DIntGDL *fancyData = new DIntGDL( 1 );
    DVar *fancy = new DVar( "FANCY", fancyData);fancy->SetCallback(CBFancyToCharsize);
    fancyIx     = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back( fancy);
    //GRID --> must be interpreted (ticklen >0)
    DIntGDL *gridData = new DIntGDL(0);
    DVar *grid = new DVar("GRID", gridData);grid->SetCallback(CBGridToTicklen);
    gridIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(grid);
    //TYPE --> must be interpreted !X.TYPE, !X.STYLE, !Y.TYPE, !Y.STYLE, !P.TICKLEN 
    DIntGDL *typeData = new DIntGDL(0);
    DVar *type = new DVar("TYPE", typeData);type->SetCallback(CBTypeToXandY);
    typeIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(type);
    //SC1 --> must be interpreted !P.POSITION[0] * !D.X_VSIZE if !P.POSITION[2] is nonzero, or !X.WINDOW[0] * !D.X_VSIZE otherwise.
    
    DFloatGDL *sc1Data = new DFloatGDL(dimension(1), BaseGDL::NOALLOC);
    sc1Data->SetBuffer((void*) &(sc[0]));
    sc1Data->SetBufferSize(1);
    sc1Data->SetDim(dimension(1));
    DVar *sc1 = new DVar("SC1", sc1Data);
    sc1Ix = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(sc1);
    //SC2 --> must be interpreted !P.POSITION[2] * !D.X_VSIZE if !P.POSITION[2] is nonzero, or !X.WINDOW[1] * !D.X_VSIZE otherwise.  
    DFloatGDL *sc2Data = new DFloatGDL(dimension(1), BaseGDL::NOALLOC);
    sc2Data->SetBuffer((void*) &(sc[1]));
    sc2Data->SetBufferSize(1);
    sc2Data->SetDim(dimension(1));
    DVar *sc2 = new DVar("SC2", sc2Data);
    sc2Ix = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(sc2);
    //SC3 --> must be interpreted !P.POSITION[1] * !D.X_VSIZE if !P.POSITION[2] is nonzero, or !Y.WINDOW[0] * !D.X_VSIZE otherwise. 
    DFloatGDL *sc3Data = new DFloatGDL(dimension(1), BaseGDL::NOALLOC);
    sc3Data->SetBuffer((void*) &(sc[2]));
    sc3Data->SetBufferSize(1);
    sc3Data->SetDim(dimension(1));
    DVar *sc3 = new DVar("SC3", sc3Data);
    sc3Ix = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(sc3);
    //SC4 --> must be interpreted !P.POSITION[3] * !D.X_VSIZE if !P.POSITION[2] is nonzero, or !Y.WINDOW[1] * !D.X_VSIZE otherwise. 
    DFloatGDL *sc4Data = new DFloatGDL(dimension(1), BaseGDL::NOALLOC);
    sc4Data->SetBuffer((void*) &(sc[3]));
    sc4Data->SetBufferSize(1);
    sc4Data->SetDim(dimension(1));
    DVar *sc4 = new DVar("SC4", sc4Data);
    sc4Ix = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(sc4);
    // some constants

    // !ORDER
    DLongGDL *orderData = new DLongGDL( 0 );
    DVar *order = new DVar( "ORDER", orderData);
    orderIx     = sysVarList.size();
    sysVarList.push_back( order);

    // !GDL (to allow distinguish IDL/GDL with DEFSYSV, '!gdl', exists=exists )
    DStructGDL*  gdlStruct = new DStructGDL( "!GNUDATALANGUAGE");
    gdlStruct->NewTag("RELEASE", new DStringGDL( (char*)VERSION + (VERSION[0] == 'v' ? 1 : 0)));

    // creating an explicit build date in !GDL (also exist in !version)
    gdlStruct->NewTag("BUILD_DATE", new DStringGDL(BUILD_DATE));

    // creating and Epoch entry in order to have a simple incremental number 
    int CompilationMonth =0, CompilationYear=0, CompilationDay=0;
    string MyDate= BUILD_DATE;
    string SCompilationYear;
    SCompilationYear=MyDate.substr(7,4);
    CompilationYear=atoi(SCompilationYear.c_str());
    string SCompilationDay;
    SCompilationDay=MyDate.substr(4,2);
    CompilationDay=atoi(SCompilationDay.c_str());

    // for the months, it is more difficult
    if (MyDate.find("Jan")!=string::npos) CompilationMonth=1;
    if (MyDate.find("Feb")!=string::npos) CompilationMonth=2;
    if (MyDate.find("Mar")!=string::npos) CompilationMonth=3;
    if (MyDate.find("Apr")!=string::npos) CompilationMonth=4;
    if (MyDate.find("May")!=string::npos) CompilationMonth=5;
    if (MyDate.find("Jun")!=string::npos) CompilationMonth=6;
    if (MyDate.find("Jul")!=string::npos) CompilationMonth=7;
    if (MyDate.find("Aug")!=string::npos) CompilationMonth=8;
    if (MyDate.find("Sep")!=string::npos) CompilationMonth=9;
    if (MyDate.find("Oct")!=string::npos) CompilationMonth=10;
    if (MyDate.find("Nov")!=string::npos) CompilationMonth=11;
    if (MyDate.find("Dec")!=string::npos) CompilationMonth=12;
		     
    //cout << SCompilationYear << " "<< CompilationMonth <<endl;
    //cout << CompilationYear<< endl;
    struct tm t;
    time_t t_of_day;
    t.tm_year = CompilationYear -1900;
    t.tm_mon = CompilationMonth-1;           // Month, 0 - jan
    t.tm_mday = CompilationDay;          // Day of the month
    t.tm_hour = 0;     
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);

    // printing Epoch on the Command Line $   date +"%s"
    // printf("seconds since the Epoch: %ld\n", (long) t_of_day);

    gdlStruct->NewTag("EPOCH", new DLongGDL((long) t_of_day));
    gdlStruct->NewTag("GDL_USE_DSFMT", new DByteGDL(1));
    gdlStruct->NewTag("GDL_USE_WX", new DByteGDL(0));
#ifdef _WIN32
    std::string use_posix=GetEnvString("GDL_USE_POSIX");
    if( use_posix.length() > 0) lib::posixpaths = true;
    gdlStruct->NewTag("GDL_POSIX", new DByteGDL(lib::posixpaths));
#else
    gdlStruct->NewTag("GDL_POSIX", new DByteGDL(1));
#endif

    DVar *gdl        = new DVar( "GDL", gdlStruct);
    gdlIx=sysVarList.size();
    sysVarList.push_back(gdl);
    sysVarRdOnlyList.push_back( gdl); // make it read only

    // !DPI
    DDoubleGDL *dpiData = new DDoubleGDL( (double)(4*atan(1.0)) );
    DVar *dpi = new DVar( "DPI", dpiData);
    sysVarList.push_back( dpi);
    sysVarRdOnlyList.push_back( dpi); // make it read only
	sysVarNoSaveList.push_back( dpi); //Is not saveable as system-defined var

    // !PI
    DFloatGDL *piData = new DFloatGDL( (float)(4*atan(1.0)) );
    DVar *pi = new DVar( "PI", piData);
    sysVarList.push_back( pi);
    sysVarRdOnlyList.push_back( pi); // make it read only
	sysVarNoSaveList.push_back( pi); //Is not saveable as system-defined var

    // !DTOR
    DFloatGDL *dtorData = new DFloatGDL((*piData)[0] / 180.);// 0.0174533);
    DVar *dtor = new DVar( "DTOR", dtorData);
    sysVarList.push_back( dtor);
    sysVarRdOnlyList.push_back( dtor); // make it read only
	sysVarNoSaveList.push_back( dtor); //Is not saveable as system-defined var

    // !RADEG
    DFloatGDL *radegData = new DFloatGDL(180. / (*piData)[0]);// 57.2957764);
    DVar *radeg = new DVar( "RADEG", radegData);
    sysVarList.push_back( radeg);
    sysVarRdOnlyList.push_back( radeg); // make it read only
	sysVarNoSaveList.push_back( radeg); //Is not saveable as system-defined var

    // !CONST
    // source : http://physics.nist.gov/cgi-bin/cuu/Results?category=abbr_in
    DStructGDL *constantList   = new DStructGDL( "!CONST");
    // Fine structure constant
    constantList ->NewTag("ALPHA", new DDoubleGDL(7.2973525698e-3));
    // Astronomical Unit [m]
    constantList ->NewTag("AU", new DDoubleGDL(1.49597870700e11));
    // Speed of Light in Vacuum [m/s]
    constantList ->NewTag("C", new DDoubleGDL(299792458.));
    // Degrees to radians
    constantList ->NewTag("DTOR", new DDoubleGDL(0.0174532925199433));  //exact value in double. previous was wrong.
    // Elementary Charge [Coulon]
    constantList ->NewTag("E", new DDoubleGDL(1.602176565e-19));
    // Electric Vacuum Permittivity [F/m]
    constantList ->NewTag("EPS0", new DDoubleGDL(8.854187817e-12));
    // Euler's number
    constantList ->NewTag("EULER", new DDoubleGDL(2.7182818284590452));
    // Faraday constant NAe [C/mol]
    constantList ->NewTag("F", new DDoubleGDL(96485.3365));
    // Gravitation constant [m^3/kg/s^2]
    constantList ->NewTag("G", new DDoubleGDL(6.67384e-11));
    // Earth standard gravity [m/s^2]
    constantList ->NewTag("GN", new DDoubleGDL(9.80665));
    // Planck constant [Js]
    constantList ->NewTag("H", new DDoubleGDL(6.62606957e-34));
    // h_bar (h/!pi) [Js]
    constantList ->NewTag("HBAR", new DDoubleGDL(1.054571726e-34));
    //Imaginary number 
    complex<double> imaginary(0., 1.);
    constantList ->NewTag("I", new DComplexDblGDL(imaginary));
    // Boltzmann constant (R/NA) [J/K]
    constantList ->NewTag("K", new DDoubleGDL(1.3806488e-23 ));
    // Light-Year distance [m]
    constantList ->NewTag("LY", new DDoubleGDL(9.4607304725808e15));
    // Mass of the Earth [kg]
    constantList ->NewTag("M_EARTH", new DDoubleGDL(5.972186390e24));
    // Mass of the Sun [kg]
    constantList ->NewTag("M_SUN", new DDoubleGDL(1.98841586057e30));
    // electron mass [kg]
    constantList ->NewTag("ME", new DDoubleGDL(9.10938291e-31));
    // neutron mass [kg]
    constantList ->NewTag("MN", new DDoubleGDL(1.674927351e-27));
    // proton mass [kg]
    constantList ->NewTag("MP", new DDoubleGDL(1.672621777e-27));
    // magnetic vacuum permeability [N/A^2]
    constantList ->NewTag("MU0", new DDoubleGDL(12.566370614e-7));
    // Loschmidt's number NAp0/(RT0) [m-3]
    constantList ->NewTag("N0", new DDoubleGDL(2.6867805e25));
    // Avogadro constant NA [mol-1]
    constantList ->NewTag("NA", new DDoubleGDL(6.02214129e23));
    // Standard atmosphere Pression [Pa]
    constantList ->NewTag("P0", new DDoubleGDL(101325.));
    // Parsec distance [m]
    constantList ->NewTag("PARSEC", new DDoubleGDL(3.0856775814671912e16));
    //golden ratio ((1+sqrt(5))/2)
    constantList ->NewTag("PHI", new DDoubleGDL(1.6180339887498948));
    // Pi
    constantList ->NewTag("PI", new DDoubleGDL((*dpiData)[0]));
    // molar gas constant [J/mol/K]
    constantList ->NewTag("R", new DDoubleGDL(8.3144621));
    // Earth radius (assuming spherical) [m]
    constantList ->NewTag("R_EARTH", new DDoubleGDL(6378136.6));
    // Radians to degrees
    constantList ->NewTag("RTOD", new DDoubleGDL(180./(*dpiData)[0]));
    // classical electron radius [m]
    constantList ->NewTag("RE", new DDoubleGDL(2.8179403267e-15));
    // Rydberg constant R∞ [1/m]
    constantList ->NewTag("RYDBERG", new DDoubleGDL(10973731.568539));
    // Stefan-Boltzmann constant [W/m^2/K^4]
    constantList ->NewTag("SIGMA", new DDoubleGDL(5.670373e-8));
    // Standard temperature [K]
    constantList ->NewTag("T0", new DDoubleGDL(273.15));
    // unified atomic mass unit [kg]
    constantList ->NewTag("U", new DDoubleGDL(1.660538921e-27));
    // Molar volume, ideal gas at Standard temperature and Pression (STP) [m^3/mol]
    constantList ->NewTag("VM", new DDoubleGDL(22.413968e-3));

    DVar *constant = new DVar("CONST",constantList);
    sysVarList.push_back(constant);
    sysVarRdOnlyList.push_back(constant); // make it read only
	sysVarNoSaveList.push_back(constant); //Is not saveable as system-defined var


    // ![XYZ]
    SizeT dim2  = 2;
    SizeT dim60 = 60;
    SizeT dim10 = 10;
    DStructGDL*  xAxis = new DStructGDL( "!AXIS");
    xAxis->NewTag("TITLE", new DStringGDL( "")); //tag #0 for !XTITLE
    xAxis->NewTag("TYPE", new DLongGDL( 0)); 
    xAxis->NewTag("STYLE", new DLongGDL( 0)); 
    xAxis->NewTag("TICKS", new DLongGDL( 0)); //tag #3 for !XTICKS
    xAxis->NewTag("TICKLEN", new DFloatGDL( 0.0)); 
    xAxis->NewTag("THICK", new DFloatGDL( 0.0)); 
    xAxis->NewTag("RANGE", new DDoubleGDL( dimension( &dim2,one))); //tag #6 for ![X|Y][MIN|MAX]
    xAxis->NewTag("CRANGE", new DDoubleGDL( dimension( &dim2,one))); //tag #7 for !C[X|Y][MIN|MAX]
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

    //very old obsolete variables
    DStringGDL* gdlxdottitle = new DStringGDL(dimension(1), BaseGDL::NOALLOC);
    gdlxdottitle->SetBuffer((void*) &((*static_cast<DStringGDL*> (xAxis->GetTag(0, 0)))[0]));
    gdlxdottitle->SetBufferSize(1);
    gdlxdottitle->SetDim(dimension(1));
    DVar *xdottitle = new DVar("XTITLE", gdlxdottitle);
    xdottitleIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(xdottitle);

    DStringGDL* gdlydottitle = new DStringGDL(dimension(1), BaseGDL::NOALLOC);
    gdlydottitle->SetBuffer((void*) &((*static_cast<DStringGDL*> (yAxis->GetTag(0, 0)))[0]));
    gdlydottitle->SetBufferSize(1);
    gdlydottitle->SetDim(dimension(1));
    DVar *ydottitle = new DVar("YTITLE", gdlydottitle);
    ydottitleIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(ydottitle);

    DLongGDL* xdotticksgdl = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    xdotticksgdl->SetBuffer((void*) &((*static_cast<DLongGDL*> (xAxis->GetTag(3, 0)))[0]));
    xdotticksgdl->SetBufferSize(1);
    xdotticksgdl->SetDim(dimension(1));
    DVar *xdotticks = new DVar("XTICKS", xdotticksgdl);
    xdotticksIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(xdotticks);
    DLongGDL* ydotticksgdl = new DLongGDL(dimension(1), BaseGDL::NOALLOC);
    ydotticksgdl->SetBuffer((void*) &((*static_cast<DLongGDL*> (yAxis->GetTag(3, 0)))[0]));
    ydotticksgdl->SetBufferSize(1);
    ydotticksgdl->SetDim(dimension(1));
    DVar *ydotticks = new DVar("YTICKS", ydotticksgdl);
    ydotticksIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(ydotticks);

    DDoubleGDL* xmingdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    xmingdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (xAxis->GetTag(6, 0)))[0]));
    xmingdl->SetBufferSize(1);
    xmingdl->SetDim(dimension(1));
    DVar *xmin = new DVar("XMIN", xmingdl);
    xminIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(xmin);
    DDoubleGDL* xmaxgdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    xmaxgdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (xAxis->GetTag(6, 0)))[1]));
    xmaxgdl->SetBufferSize(1);
    xmaxgdl->SetDim(dimension(1));
    DVar *xmax = new DVar("XMAX", xmaxgdl);
    xmaxIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(xmax);
    DDoubleGDL* ymingdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    ymingdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (yAxis->GetTag(6, 0)))[0]));
    ymingdl->SetBufferSize(1);
    ymingdl->SetDim(dimension(1));
    DVar *ymin = new DVar("YMIN", ymingdl);
    yminIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(ymin);
    DDoubleGDL* ymaxgdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    ymaxgdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (yAxis->GetTag(6, 0)))[1]));
    ymaxgdl->SetBufferSize(1);
    ymaxgdl->SetDim(dimension(1));
    DVar *ymax = new DVar("YMAX", ymaxgdl);
    ymaxIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(ymax);    
    
    DDoubleGDL* cxmingdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    cxmingdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (xAxis->GetTag(7, 0)))[0]));
    cxmingdl->SetBufferSize(1);
    cxmingdl->SetDim(dimension(1));
    DVar *cxmin = new DVar("CXMIN", cxmingdl);
    cxminIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(cxmin);
    DDoubleGDL* cxmaxgdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    cxmaxgdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (xAxis->GetTag(7, 0)))[1]));
    cxmaxgdl->SetBufferSize(1);
    cxmaxgdl->SetDim(dimension(1));
    DVar *cxmax = new DVar("CXMAX", cxmaxgdl);
    cxmaxIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(cxmax);
    DDoubleGDL* cymingdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    cymingdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (yAxis->GetTag(7, 0)))[0]));
    cymingdl->SetBufferSize(1);
    cymingdl->SetDim(dimension(1));
    DVar *cymin = new DVar("CYMIN", cymingdl);
    cyminIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(cymin);
    DDoubleGDL* cymaxgdl = new DDoubleGDL(dimension(1), BaseGDL::NOALLOC);
    cymaxgdl->SetBuffer((void*) &((*static_cast<DDoubleGDL*> (yAxis->GetTag(7, 0)))[1]));
    cymaxgdl->SetBufferSize(1);
    cymaxgdl->SetDim(dimension(1));
    DVar *cymax = new DVar("CYMAX", cymaxgdl);
    cymaxIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(cymax);    
    // !VERSION
    DStructGDL*  ver = new DStructGDL( "!VERSION");
#ifdef _WIN32
#ifdef __MINGW32__
    typedef void (WINAPI *GetNativeSystemInfoFunc)(LPSYSTEM_INFO);
    HMODULE hModule = LoadLibraryW(L"kernel32.dll");
    GetNativeSystemInfoFunc GetNativeSystemInfo =(GetNativeSystemInfoFunc) 
      GetProcAddress(hModule, "GetNativeSystemInfo");
#endif
    const char* SysName = "Windows";
    SYSTEM_INFO stInfo;
    GetNativeSystemInfo( &stInfo );
    DStringGDL *arch;
    switch(stInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
      arch = new DStringGDL("x64");
      break;
    case PROCESSOR_ARCHITECTURE_INTEL:
      arch = new DStringGDL("x86");
      break;
    case PROCESSOR_ARCHITECTURE_ARM:
      arch = new DStringGDL("ARM");
      break;
    default:
      arch = new DStringGDL("unknown");
    }
    ver->NewTag("ARCH", arch); 
    ver->NewTag("OS", new DStringGDL(SysName));
    ver->NewTag("OS_FAMILY", new DStringGDL(SysName));
    ver->NewTag("OS_NAME", new DStringGDL(SysName));
#else
    struct utsname uts;
    uname(&uts);
    ver->NewTag("ARCH", new DStringGDL( uts.machine));
    const char *SysName=uts.sysname;
    if (strcmp(SysName,"Linux") ==0) SysName="linux";
    if (strcmp(SysName,"Darwin") ==0) SysName="darwin";
    ver->NewTag("OS", new DStringGDL(SysName));    //correct IDL order
    ver->NewTag("OS_FAMILY", new DStringGDL( "unix"));
    // AC 2018-sep-07
    if (strcmp(SysName,"darwin") ==0) SysName="Mac OS X";
    ver->NewTag("OS_NAME", new DStringGDL(SysName));
#endif

    ver->NewTag("RELEASE", new DStringGDL( "8.4")); //Reasonable value to attain some day (for example, we do not have the new graphic functions).
    ver->NewTag("BUILD_DATE", new DStringGDL(BUILD_DATE));
    ver->NewTag("MEMORY_BITS", new DIntGDL( sizeof(BaseGDL*)*8));
    ver->NewTag("FILE_OFFSET_BITS", new DIntGDL( sizeof(SizeT)*8));
    DVar *v            = new DVar( "VERSION", ver);
    vIx                = sysVarList.size();
    sysVarList.push_back(v);
    sysVarRdOnlyList.push_back(v);
	sysVarNoSaveList.push_back(v); //Is not saveable as system-defined var

    // !Mouse
    DStructGDL*  MouseData = new DStructGDL( "!MOUSE");
    MouseData->NewTag("X", new DLongGDL( 0));
    MouseData->NewTag("Y", new DLongGDL( 0));
    MouseData->NewTag("BUTTON", new DLongGDL( 0));
    MouseData->NewTag("TIME", new DLongGDL( 0));
    DVar *Mouse      = new DVar( "MOUSE", MouseData);
    MouseIx          = sysVarList.size();
    sysVarList.push_back(Mouse);

    // !Make_dll
    DStructGDL*  MakeDllData = new DStructGDL( "!MAKE_DLL");
    MakeDllData->NewTag("COMPILE_DIRECTORY", new DStringGDL("/tmp/"));
    MakeDllData->NewTag("COMPILER_NAME", new DStringGDL("GCC"));
    MakeDllData->NewTag("CC", new DStringGDL("gcc %X -fPIC -I%Z -c -D_REENTRANT %C -o %O"));
    MakeDllData->NewTag("LD", new DStringGDL("ld -shared -o %L %O %X"));
    DVar *MakeDll      = new DVar( "MAKE_DLL", MakeDllData);
    MakeDllIx          = sysVarList.size();
    sysVarList.push_back(MakeDll);
    sysVarNoSaveList.push_back(MakeDll);

    // !ERROR_STATE
    DStructGDL*  eStateData = new DStructGDL( "!ERROR_STATE");
    eStateData->NewTag("NAME", new DStringGDL( "IDL_M_SUCCESS")); //0 see !ERROR_NAME
    eStateData->NewTag("BLOCK", new DStringGDL( "IDL_MBLK_CORE"));
    eStateData->NewTag("CODE", new DLongGDL( 0));
    eStateData->NewTag("SYS_CODE", new DLongGDL( dimension( &dim2,one))); //idl 8 //3 see SYSERROR
    eStateData->NewTag("SYS_CODE_TYPE", new DStringGDL( ""));
    eStateData->NewTag("MSG", new DStringGDL( ""));
    eStateData->NewTag("SYS_MSG", new DStringGDL( ""));
    eStateData->NewTag("MSG_PREFIX", new DStringGDL( "% "));//7 see !MSG_PREFIX
    DVar *eState       = new DVar( "ERROR_STATE", eStateData);
    errorStateIx       = sysVarList.size();
    sysVarList.push_back(eState);
    //    sysVarRdOnlyList.push_back(eState);
    //very old obsolete variables
    DStringGDL* gdlerrorstatedoterror = new DStringGDL(dimension(1), BaseGDL::NOALLOC);
    gdlerrorstatedoterror->SetBuffer((void*) &((*static_cast<DStringGDL*> (eStateData->GetTag(0, 0)))[0]));
    gdlerrorstatedoterror->SetBufferSize(1);
    gdlerrorstatedoterror->SetDim(dimension(1));
    DVar *errorstatedoterror = new DVar("ERROR_NAME", gdlerrorstatedoterror);
    errorstatedoterrorIx = obsoleteSysVarList.size(); //do not remove this!
    obsoleteSysVarList.push_back(errorstatedoterror); //must be in obsolete and cannot be Save/Restored.
    DStringGDL* gdlerrorstatedotmsgprefix = new DStringGDL(dimension(1), BaseGDL::NOALLOC);
    gdlerrorstatedotmsgprefix->SetBuffer((void*) &((*static_cast<DStringGDL*> (eStateData->GetTag(7, 0)))[0]));
    gdlerrorstatedotmsgprefix->SetBufferSize(1);
    gdlerrorstatedotmsgprefix->SetDim(dimension(1));
    DVar *errorstatedotmsgprefix = new DVar("MSG_PREFIX", gdlerrorstatedotmsgprefix);
    errorstatedotmsgprefixIx = obsoleteSysVarList.size(); //do not remove this!
    obsoleteSysVarList.push_back(errorstatedotmsgprefix); //must be in obsolete and cannot be Save/Restored.
    DLongGDL* errorstatesyscodegdl = new DLongGDL(dimension(2), BaseGDL::NOALLOC);
    errorstatesyscodegdl->SetBuffer((void*) &((*static_cast<DLongGDL*> (eStateData->GetTag(3, 0)))[0]));
    errorstatesyscodegdl->SetBufferSize(2);
    errorstatesyscodegdl->SetDim(dimension(2));
    DVar *errorstatesyscode = new DVar("SYSERROR", errorstatesyscodegdl);
    errorstatesyscodeIx = obsoleteSysVarList.size();
    obsoleteSysVarList.push_back(errorstatesyscode);

    // !ERROR
    DLongGDL *errorData = new DLongGDL( 0 );
    DVar *errorVar = new DVar( "ERROR", errorData );
    errorIx            = sysVarList.size();
    sysVarList.push_back( errorVar);
    //sysVarRdOnlyList.push_back( errorVar); !error is (no more?) a readonly variable.

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
    sysVarRdOnlyList.push_back( err_stringVar); //!err_string IS a readonly variable!
	sysVarNoSaveList.push_back( err_stringVar); //Is not saveable as system-defined var

    // !VALUES
    DStructGDL*  valuesData = new DStructGDL( "!VALUES");
    if( std::numeric_limits< DFloat>::has_infinity)
      {
	valuesData->NewTag("F_INFINITY", 
			   new DFloatGDL( std::numeric_limits< DFloat>::infinity())); 
      }
    else
      {
#ifndef _MSC_VER // Can be ignored, because the windows version of limit has infinity()
	valuesData->NewTag("F_INFINITY", new DFloatGDL((float)1.0/0.0)); 
#endif
      }
#ifdef NAN
    valuesData->NewTag("F_NAN", new DFloatGDL(NAN));
#else
    valuesData->NewTag("F_NAN", new DFloatGDL(sqrt((float) -1.0))); //sign depends on the architecture, dangerous way to define a +Nan!
#endif

    if( std::numeric_limits< DDouble>::has_infinity)
      {
	valuesData->NewTag("D_INFINITY", 
			   new DDoubleGDL( std::numeric_limits< DDouble>::infinity())); 
      }
    else
      {
#ifndef _MSC_VER // Can be ignored, because the windows version of limit has infinity()
	valuesData->NewTag("D_INFINITY", new DDoubleGDL( (double)1.0/0.0)); 
#endif
      }

#ifdef NAN
    valuesData->NewTag("D_NAN", new DDoubleGDL(NAN));
#else
    valuesData->NewTag("D_NAN", new DDoubleGDL(-sqrt((double) -1.0))); //sign depends on the architecture, dangerous way to define a +Nan!
#endif
    DVar *values       = new DVar( "VALUES", valuesData);
    valuesIx           = sysVarList.size();
    sysVarList.push_back(values);
    sysVarRdOnlyList.push_back( values);
	sysVarNoSaveList.push_back( values); //Is not saveable as system-defined var

    // !JOURNAL hold journal file lun
    DLongGDL *journalData = new DLongGDL( 0);
    DVar *journal = new DVar( "JOURNAL", journalData);
    journalIx     = sysVarList.size();
    sysVarList.push_back( journal);
    sysVarRdOnlyList.push_back( journal);
	sysVarNoSaveList.push_back( journal); //Is not saveable as system-defined var

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
    // init independent of OpenMP usage
#ifdef _OPENMP
    CpuTPOOL_NTHREADS = omp_get_num_procs();
    omp_set_num_threads(CpuTPOOL_NTHREADS);
#else
    CpuTPOOL_NTHREADS = 1;
#endif
    CpuTPOOL_MIN_ELTS = DefaultTPOOL_MIN_ELTS;
    CpuTPOOL_MAX_ELTS = DefaultTPOOL_MAX_ELTS;

    DStructGDL* cpuData = new DStructGDL( "!CPU");
    cpuData->NewTag("HW_VECTOR", new DLongGDL( 0)); 
    cpuData->NewTag("VECTOR_ENABLE", new DLongGDL( 0)); 
#ifdef _OPENMP
    cpuData->NewTag("HW_NCPU", new DLongGDL( omp_get_num_procs())); 
#else
    cpuData->NewTag("HW_NCPU", new DLongGDL( 1)); 
#endif
    cpuData->NewTag("TPOOL_NTHREADS", new DLongGDL( CpuTPOOL_NTHREADS));

    //if use DLong64 below, please update basic_pro.cpp (function cpu()) and
    //add an 'assureLong64Kw()' function in envt.cpp. Otherwise the program will
    //crash in cpu(). (should have been done on 2014 March 18 by AC (tested).)
    cpuData->NewTag("TPOOL_MIN_ELTS", new DLong64GDL( CpuTPOOL_MIN_ELTS)); 
    cpuData->NewTag("TPOOL_MAX_ELTS", new DLong64GDL( CpuTPOOL_MAX_ELTS)); 

    DVar *cpu=new DVar( "CPU", cpuData);
    cpuIx=sysVarList.size();
    sysVarList.push_back( cpu);
    sysVarRdOnlyList.push_back( cpu);
	sysVarNoSaveList.push_back( cpu); //Is not saveable as system-defined var

#ifdef _OPENMP
    if( omp_get_dynamic())
	omp_set_dynamic( 1);
#endif
#if defined (_WIN32)

#define realpath(N,R) _fullpath((R),(N),_MAX_PATH) 
// ref:http://sourceforge.net/p/mingw/patches/256/ Keith Marshall 2005-12-02
#endif
    DStringGDL *dirData = new DStringGDL(gdlDataDir) ;
    std::string gdlDir=GetEnvPathString("GDL_DIR");
    if( gdlDir == "") gdlDir=GetEnvPathString("IDL_DIR");
    if (gdlDir != "") {
      delete dirData;
      dirData = new DStringGDL(gdlDir);
    }
    DVar *dir = new DVar( "DIR", dirData);
    dirIx=sysVarList.size();
    sysVarList.push_back( dir);

    // !GDL_MAPS_DIR cannot be found in $IDL_DIR natively. It is only where we put it.
    string gdlmapsdir=GetEnvPathString("GDL_MAPS_DIR");
    if( gdlmapsdir == "") gdlmapsdir = gdlDataDir +lib::PathSeparator() +"resource"+lib::PathSeparator()+"maps";

    DStringGDL *GdlMapsDataDir =  new DStringGDL( gdlmapsdir);
    DVar *GdlMapsDir = new DVar("GDL_MAPS_DIR", GdlMapsDataDir);
    sysVarList.push_back(GdlMapsDir);
   
    // !STIME
    DStringGDL *stimeData = new DStringGDL( "");
    DVar *stime = new DVar( "STIME", stimeData);
    stimeIx=sysVarList.size();
    sysVarList.push_back( stime);
    sysVarRdOnlyList.push_back( stime); // make it read only
	sysVarNoSaveList.push_back( stime); //Is not saveable as system-defined var

    // !WARN
    DStructGDL*  warnData = new DStructGDL( "!WARN");
    warnData->NewTag("OBS_ROUTINES", new DByteGDL( 0)); 
    warnData->NewTag("OBS_SYSVARS", new DByteGDL( 0)); 
    warnData->NewTag("PARENS", new DByteGDL( 0)); 
    DVar *warn = new DVar( "WARN", warnData);
    warnIx     = sysVarList.size();
    sysVarList.push_back(warn);

    //!COLOR
    static const int col[]={240,248,255,250,235,215,0,255,255,127,255,212,240,255,255,245,245,220,255,228,196,0,0,0,255,235,205,0,0,255,138,43,
226,165,42,42,222,184,135,95,158,160,127,255,0,210,105,30,255,127,80,100,149,237,255,248,220,220,20,60,0,255,255,0,
0,139,0,139,139,184,134,11,169,169,169,0,100,0,169,169,169,189,183,107,139,0,139,85,107,47,255,140,0,153,50,204,
139,0,0,233,150,122,143,188,143,72,61,139,47,79,79,47,79,79,0,206,209,148,0,211,255,20,147,0,191,255,105,105,
105,105,105,105,30,144,255,178,34,34,255,250,240,34,139,34,255,0,255,220,220,220,248,248,255,255,215,0,218,165,32,127,
127,127,0,127,0,173,255,47,127,127,127,240,255,240,255,105,180,205,92,92,75,0,130,255,255,240,240,230,140,230,230,250,
255,240,245,124,252,0,255,250,205,173,216,230,240,128,128,224,255,255,250,250,210,144,238,144,211,211,211,211,211,211,255,182,
193,255,160,122,32,178,170,135,206,250,119,136,153,119,136,153,176,196,222,255,255,224,0,255,0,50,205,50,250,240,230,255,
0,255,127,0,0,102,205,170,0,0,205,186,85,211,147,112,219,60,179,113,123,104,238,0,250,154,72,209,204,199,21,133,
025,25,112,245,255,250,255,228,225,255,228,181,255,222,173,0,0,128,253,245,230,128,128,0,107,142,35,255,165,0,255,69,
0,218,112,214,238,232,170,152,251,152,175,238,238,219,112,147,255,239,213,255,218,185,205,133,63,255,192,203,221,160,221,176,
224,230,127,0,127,255,0,0,188,143,143,65,105,225,139,69,19,250,128,114,244,164,96,46,139,87,255,245,238,160,82,45,
192,192,192,135,206,235,106,90,205,112,128,144,112,128,144,255,250,250,0,255,127,70,130,180,210,180,140,0,128,128,216,191,
216,255,99,71,64,224,208,238,130,238,245,222,179,255,255,255,245,245,245,255,255,0,154,205,50};
    static const string coln[]={"ALICE_BLUE","ANTIQUE_WHITE","AQUA","AQUAMARINE","AZURE","BEIGE","BISQUE","BLACK","BLANCHED_ALMOND",
    "BLUE","BLUE_VIOLET","BROWN","BURLYWOOD","CADET_BLUE","CHARTREUSE","CHOCOLATE","CORAL","CORNFLOWER","CORNSILK",
    "CRIMSON","CYAN","DARK_BLUE","DARK_CYAN","DARK_GOLDENROD","DARK_GRAY","DARK_GREEN","DARK_GREY","DARK_KHAKI",
    "DARK_MAGENTA","DARK_OLIVE_GREEN","DARK_ORANGE","DARK_ORCHID","DARK_RED","DARK_SALMON","DARK_SEA_GREEN",
    "DARK_SLATE_BLUE","DARK_SLATE_GRAY","DARK_SLATE_GREY","DARK_TURQUOISE","DARK_VIOLET","DEEP_PINK","DEEP_SKY_BLUE",
    "DIM_GRAY","DIM_GREY","DODGER_BLUE","FIREBRICK","FLORAL_WHITE","FOREST_GREEN","FUCHSIA","GAINSBORO","GHOST_WHITE",
    "GOLD","GOLDENROD","GRAY","GREEN","GREEN_YELLOW","GREY","HONEYDEW","HOT_PINK","INDIAN_RED","INDIGO","IVORY","KHAKI",
    "LAVENDER","LAVENDER_BLUSH","LAWN_GREEN","LEMON_CHIFFON","LIGHT_BLUE","LIGHT_CORAL","LIGHT_CYAN","LIGHT_GOLDENROD",
    "LIGHT_GREEN","LIGHT_GRAY","LIGHT_GREY","LIGHT_PINK","LIGHT_SALMON","LIGHT_SEA_GREEN","LIGHT_SKY_BLUE","LIGHT_SLATE_GRAY",
    "LIGHT_SLATE_GREY","LIGHT_STEEL_BLUE","LIGHT_YELLOW","LIME","LIME_GREEN","LINEN","MAGENTA","MAROON","MEDIUM_AQUAMARINE",
    "MEDIUM_BLUE","MEDIUM_ORCHID","MEDIUM_PURPLE","MEDIUM_SEA_GREEN","MEDIUM_SLATE_BLUE","MEDIUM_SPRING_GREEN","MEDIUM_TURQUOISE",
    "MEDIUM_VIOLET_RED","MIDNIGHT_BLUE","MINT_CREAM","MISTY_ROSE","MOCCASIN","NAVAJO_WHITE","NAVY","OLD_LACE","OLIVE",
    "OLIVE_DRAB","ORANGE","ORANGE_RED","ORCHID","PALE_GOLDENROD","PALE_GREEN","PALE_TURQUOISE","PALE_VIOLET_RED","PAPAYA_WHIP",
    "PEACH_PUFF","PERU","PINK","PLUM","POWDER_BLUE","PURPLE","RED","ROSY_BROWN","ROYAL_BLUE","SADDLE_BROWN","SALMON",
    "SANDY_BROWN","SEA_GREEN","SEASHELL","SIENNA","SILVER","SKY_BLUE","SLATE_BLUE","SLATE_GRAY","SLATE_GREY","SNOW","SPRING_GREEN",
    "STEEL_BLUE","TAN","TEAL","THISTLE","TOMATO","TURQUOISE","VIOLET","WHEAT","WHITE","WHITE_SMOKE","YELLOW","YELLOW_GREEN"};
    int ncol=147;
    int i,k;
    DStructGDL*  colorData = new DStructGDL( "!COLOR");
    for (i=0, k=0; i<ncol; ++i){
     colorData->NewTag(coln[i], new DByteGDL( dimension(3)));
     for (int j=0; j<3; ++j) (*static_cast<DByteGDL*>( colorData->GetTag( i, 0)))[j] = col[k++];
    }
    DVar *color = new DVar( "COLOR", colorData);
    colorIx     = sysVarList.size();
    sysVarList.push_back(color);
    sysVarRdOnlyList.push_back( color); //Is Readonly. 
	sysVarNoSaveList.push_back( color); //Is not saveable as system-defined var
  }

}
