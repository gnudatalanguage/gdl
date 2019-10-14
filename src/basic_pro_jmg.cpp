/***************************************************************************
                          basic_fun_jmg.cpp  -  basic GDL library function
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
                         : (C) 2010 by Christoph Fuchs (CALL_EXTERNAL)
    email                : jomoga@users.sourceforge.net
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

#include <string>
#include <fstream>
#include <limits>

#include "envt.hpp"
#include "dinterpreter.hpp"
#include "prognode.hpp"
#include "basic_pro_jmg.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

#if defined(_WIN32) && !defined(__CYGWIN__)
#   include "gtdhelper.hpp" // just a workaround, using QueryPerformanceCounter is better
    typedef HMODULE handle_t;
#else
    typedef void* handle_t;
#endif

namespace lib {

    using namespace std;

    void CleanupProc( DLibPro* proc ) {
        auto it = libProList.begin();
        auto itE = libProList.end();
        for( ; it != itE; it++ ) {
            if( *it == proc ) break;
        }
        if( it < itE ) {
            delete *it;
            libProList.erase( it );
        }
    }

    void CleanupFunc( DLibFun* func ) {
        auto it = libFunList.begin();
        auto itE = libFunList.end();
        for( ; it != itE; it++ ) {
            if( *it == func ) break;
        }
        if( it < itE ) {
            delete *it;
            libFunList.erase( it );
        }
    }

    struct DllContainer {
        DllContainer( const DllContainer& ) = delete;
        DllContainer( DllContainer&& rhs ) : handle(nullptr) { std::swap( handle, rhs.handle ); };
        DllContainer( const string& fn ) : handle(nullptr) { load(fn); }
        ~DllContainer() { unload( true ); }
        void load( const string& fn ) {
            if( handle ) return;     // already loaded.
            string msg;
#if defined(_WIN32) && !defined(__CYGWIN__)
            WCHAR u_shrdimgName[255];
            MultiByteToWideChar(CP_ACP, 0, fn.c_str(), fn.length(), u_shrdimgName, 255);
            handle = LoadLibraryW(u_shrdimgName);
            if( !handle ) {
                msg = "Couldn't open " + fn;
            }
#else
            handle = dlopen(fn.c_str(), RTLD_LAZY);
            if( !handle ) {
                msg = "Couldn't open " + fn;
                char* error = dlerror();
                if( error ) {
                    msg += string(": ") + error;
                }
            }
#endif
            if( !handle ) {
                throw runtime_error( msg );
            }
        }
        void unload( bool force=false ) {
            if( !force && !(my_procs.empty() && my_funcs.empty()) ) {
                return;
            }
            
            for( auto&p: my_procs ) {
                all_procs.erase(p);
            }
            for( auto&f: my_funcs ) {
                all_funcs.erase(f);
            }
            
#if defined(_WIN32) && !defined(__CYGWIN__)
            if( handle ) FreeLibrary(handle);
#else
            if( handle ) dlclose(handle);
#endif
            handle = nullptr;
        }
        static DllContainer& get( const string& fn ) {
            auto res = libs.emplace( std::pair<string,DllContainer>(fn, DllContainer(fn)) );
            DllContainer& l = res.first->second;
            if( ! l.isLoaded() ) {
                l.load(fn);
            }
            return l;
        }
        static void unload( const string& fn, bool force=false ) {
            if( libs.count(fn) ) {
                DllContainer& l = libs.at( fn );
                l.unload( force );
            }
        }
        static void clear( void ) {
            libs.clear();
            all_procs.clear();
            all_funcs.clear();
        }
        void RegsisterSymbol( const string& lib_symbol, const string& proc_name, DLong funcType, DLong max_args=16, DLong min_args=0, const string keyNames[]=NULL ) {
            if( !handle ) {
                throw runtime_error( "Library not loaded!" );
            } else if( funcType < 0 || funcType>1 ) {
                throw runtime_error( "Improper function type: "+to_string(funcType) );
            }
            if( funcType == 0 ) {
                RegsisterProc( lib_symbol, proc_name, max_args, min_args, keyNames );
            } else {
                RegsisterFunc( lib_symbol, proc_name, max_args, min_args, keyNames );
            }
        }
        void UnregsisterSymbol( const string& proc_name, DLong funcType ) {
            if( !handle ) {
                throw runtime_error( "Library not loaded!" );
            } else if( funcType < 0 || funcType>1 ) {
                throw runtime_error( "Improper function type: "+to_string(funcType) );
            }
            if( (funcType == 0) && my_procs.count(proc_name) ) {
                all_procs.erase( proc_name );
                my_procs.erase( proc_name );
            } else if( my_funcs.count(proc_name) ) {
                all_funcs.erase( proc_name );
                my_funcs.erase( proc_name );
            }
        }
        template <typename T>
        T LinkAs( const string& lib_symbol, const string& proc_name ) { 
            T fPtr = nullptr;
            char* error = nullptr;
#if defined(_WIN32) && !defined(__CYGWIN__)
            fPtr = (T) GetProcAddress( handle, lib_symbol.c_str() );
#else
            error = dlerror();  // clear error
            fPtr = (T) dlsym( handle, lib_symbol.c_str() );
            error = dlerror();
#endif
            if( error ) {
                throw runtime_error( "Failed to register DLL-routine: " + proc_name + string(" -> ") + lib_symbol + string(" : ") + error );
            }
            return fPtr;
        }
        void RegsisterProc( const string& lib_symbol, const string& proc_name, DLong max_args, DLong min_args, const string keyNames[] ) { 
            if( all_procs.count( proc_name ) ) return;
            all_procs[proc_name].reset(
                new DLibPro( LinkAs<LibPro>( lib_symbol, proc_name ), proc_name.c_str(), max_args, keyNames, NULL, min_args),
                CleanupProc
            );
            my_procs.insert(proc_name);
        }
        void RegsisterFunc( const string& lib_symbol, const string& func_name, DLong max_args, DLong min_args, const string keyNames[] ) { 
            if( all_funcs.count( func_name ) ) return;
            all_funcs[func_name].reset(
                new DLibFun( LinkAs<LibFun>( lib_symbol, func_name ), func_name.c_str(), max_args, keyNames, NULL, min_args),
                CleanupFunc
            );
            my_funcs.insert(func_name);
        }
        bool isLoaded( void ) { return handle; };
        
        handle_t handle;                                    // Handle to the linked DLL
        
        set<string> my_procs;                               // list of procedures linked within this DLL
        set<string> my_funcs;                               // list of functions linked within this DLL
        
        static map<string,shared_ptr<DLibPro>> all_procs;   // list of ALL procedures linked (using linkimage)
        static map<string,shared_ptr<DLibFun>> all_funcs;   // list of ALL functions linked (using linkimage)
        static map<string,DllContainer> libs;               // list of ALL DLLs linked (using linkimage)
        
    };

    // Instantiate static members
    map<string,shared_ptr<DLibPro>> DllContainer::all_procs;
    map<string,shared_ptr<DLibFun>> DllContainer::all_funcs;
    map<string,DllContainer> DllContainer::libs;


  void point_lun( EnvT* e) 
  { 
    e->NParam( 1);//, "POINT_LUN");

    DLong lun;
    e->AssureLongScalarPar( 0, lun);

    if( lun == 0 || abs(lun) > maxLun)
      throw GDLException( e->CallingNode(), 
  			  "POINT_LUN:  File unit is not within allowed range.");

    GDLStream& actUnit = fileUnits[ abs(lun)-1];

    if( !actUnit.IsOpen()) 
      throw GDLException( e->CallingNode(), 
  			  "POINT_LUN:  File unit is not open: " +i2s(abs(lun)));

    if (lun < 0) {
      BaseGDL** retPos = &e->GetPar( 1);

      GDLDelete(*retPos);

      // see https://github.com/gnudatalanguage/gdl/issues/441
      DLong64 pos = actUnit.Tell();
      if (pos > std::numeric_limits<DLong>::max()) {
        *retPos = new DLong64GDL( pos);
      } else {
        *retPos = new DLongGDL( pos);
      }
      return;

    } else {
      DLong64 pos;
      e->AssureLongScalarPar( 1, pos);
      actUnit.Seek( pos);
    }
  }

  void linkimage( EnvT* e ) {

    SizeT nP = e->NParam(2);
 
    DString funcName;
    e->AssureScalarPar<DStringGDL>( 0, funcName );
    DString upCasefuncName = StrUpCase( funcName );

    DString shrdimgName;
    e->AssureScalarPar<DStringGDL>( 1, shrdimgName );

    DLong funcType = 0;
    if( nP > 2 ) {
        e->AssureLongScalarPar( 2, funcType );
    }
    
    DString entryName;
    if( nP > 3 ) {
        e->AssureScalarPar<DStringGDL>( 3, entryName );
    }
    
    static int functIx = e->KeywordIx("FUNCT");
    static int keywordsIx = e->KeywordIx("KEYWORDS");
    static int maxargsIx = e->KeywordIx("MAX_ARGS");
    static int minargsIx = e->KeywordIx("MIN_ARGS");
    
    if( e->KeywordPresent( functIx ) ) funcType = 1;
    
    DLong max_args = 16;
    e->AssureLongScalarKWIfPresent( maxargsIx, max_args );
    DLong min_args = 0;
    e->AssureLongScalarKWIfPresent( minargsIx, min_args );

    vector<string> keywords;
    string* kw_ptr = nullptr;
    if( e->KeywordSet( keywordsIx ) ) {
        DStringGDL* kws = e->GetKWAs<DStringGDL>( keywordsIx );
        if( kws ) {
            SizeT nEL = kws->N_Elements();
            for( SizeT i=0; i<nEL; ++i ) {
                string tmpS = StrUpCase((*kws)[i]);
                if( !tmpS.empty() ) keywords.push_back( tmpS );
            }
        }
    }

    if( !keywords.empty() ) {
        std::sort( keywords.begin(), keywords.end() ); 
        keywords.push_back("");
        kw_ptr = keywords.data();
    }

    if( entryName.empty() ) {
        entryName = funcName;
    }

    try {
        DllContainer& lib = DllContainer::get( shrdimgName );
        lib.RegsisterSymbol( entryName, upCasefuncName, funcType, max_args, min_args, kw_ptr );
    } catch ( const std::exception& ex ) {
        e->Throw("Error linking procedure/DLL: " + funcName + " -> " + entryName + "  (" + shrdimgName + ") : " + ex.what() );
    }
    
  }
  
  void unlinkimage( EnvT* e ) {

    e->NParam(1);
    
    DString shrdimgName;
    e->AssureScalarPar<DStringGDL>( 0, shrdimgName );
    
    bool force = false;
    static int forceIx = e->KeywordIx("FORCE");
    if( e->KeywordPresent( forceIx ) ) force = true;

    try {
        DllContainer::unload( shrdimgName, force );
    } catch ( const std::exception& ex ) {
        e->Throw("Error unlinkimage:  (" + shrdimgName + ") : " + ex.what() );
    }
    
  }

  
  void unlinksymbol( EnvT* e ) {

    SizeT nP = e->NParam(2);
 
    DString funcName;
    e->AssureScalarPar<DStringGDL>( 0, funcName );
    DString upCasefuncName = StrUpCase( funcName );

    DString shrdimgName;
    e->AssureScalarPar<DStringGDL>( 1, shrdimgName );
    
    DLong funcType = 0;
    if( nP > 2 ) {
        e->AssureLongScalarPar( 2, funcType );
    }
    
    static int functIx = e->KeywordIx("FUNCT");
    if( e->KeywordPresent( functIx ) ) funcType = 1;

    try {
        DllContainer& lib = DllContainer::get( shrdimgName );
        lib.UnregsisterSymbol( upCasefuncName, funcType );
        lib.unload();       // will only unload if all symbols have been unregistered.
    } catch ( const std::exception& ex ) {
        e->Throw("Error unlinksymbol: " + funcName + "  (" + shrdimgName + ") : " + ex.what() );
    }
    
  }

  
  void ResetDLLs( void ) {
      DllContainer::clear();
  }
  
  void wait_pro( EnvT* e) 
  { 
    e->NParam( 1);//, "WAIT");

    DDouble waittime;
    e->AssureDoubleScalarPar( 0, waittime);

    if( waittime < 0)
      throw GDLException( e->CallingNode(), 
  			  "WAIT: Argument must be non-negative"
			  +e->GetParString( 0));

#ifdef _WIN32
	LARGE_INTEGER Frequency;
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER Endtime;
	LARGE_INTEGER elapsed;
	LARGE_INTEGER waittime_us;
	waittime_us.QuadPart = waittime * 1e6;

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	while (1) {
		QueryPerformanceCounter(&Endtime);
		elapsed.QuadPart = (Endtime.QuadPart - BeginTime.QuadPart)/(Frequency.QuadPart/1000000);
		if (elapsed.QuadPart >= waittime_us.QuadPart) break;
		else if (elapsed.QuadPart > 100) Sleep(80);
	}
#else
    int old_version=0;

    if (waittime <= 0.005) old_version=1;

    // AC 2010-09-16
    // this version is OK and very accurate for small durations
    // but used 100% of one CPU :((
    if (old_version == 1) {
	  struct timeval tval;
      struct timezone tzone;
      
      // derivated from the current version of SYSTIME()
      gettimeofday(&tval,&tzone);
      double t_start = tval.tv_sec+tval.tv_usec/1e+6; // time in UTC seconds
      double t_current=0.0;
      
	  double diff = 0.0;
      while (diff < waittime ) {      

		  gettimeofday(&tval,&tzone);
	t_current= tval.tv_sec+tval.tv_usec/1e+6;
	diff=t_current - t_start;
	  }
	}

    // AC 2010-09-16 this version should used much less CPU !
    if (old_version == 0) {
      //cout << floor(waittime) << " " <<  waittime-floor(waittime) << endl;
      struct timespec tv;
      tv.tv_sec = floor(waittime);
      tv.tv_nsec = (waittime-floor(waittime))*1e9;
      int retval;
      retval=nanosleep(&tv,NULL);
    }
#endif
  }


//   void kwtest( EnvT* e)
//   {
//     StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());
// 
//     // here first parameter is the function name
//     // set callF to the function you want to call
//     int nParam=e->NParam();
//     if( nParam == 0)
//      e->Throw( "No function specified.");
//     DString callF;
//     e->AssureScalarPar<DStringGDL>( 0, callF);
//  
//     // this is a function name -> convert to UPPERCASE
//     callF = StrUpCase( callF);
// 
// 	SizeT funIx = GDLInterpreter::GetFunIx( callF);
// 	
// 	EnvUDT* newEnv= new EnvUDT( e->CallingNode(), funList[ funIx], (BaseGDL**)NULL);
// 
// 	// add parameter
// 	SizeT widgetID = 999;
//       
//     newEnv->SetNextPar( new DLongGDL(widgetID)); // pass as local
// 
// 	e->Interpreter()->CallStack().push_back( newEnv);
// 	
// 	// make the call
// 	BaseGDL* res = e->Interpreter()->
// 	  call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
// 
// 	// set the keyword to the function's return value
//     static int testIx = e->KeywordIx( "TEST");
//     e->SetKW( testIx, res);
//   }

  // CALL_EXTERNAL by Christoph Fuchs
  //AC #ifdef USE_EIGEN
  //AC SizeT defaultAlign = 16;
  //AC #else  
  typedef struct {
    char      c;
    long long l;
  } testAlign;
  SizeT defaultAlign = (SizeT)( sizeof(testAlign)-sizeof(long long) );
  //AC #endif
  
  BaseGDL* call_external( EnvT* e)
  {
    DString image, entry;
    static std::string s;
    SizeT myAlign      = defaultAlign;
    DType myReturnType = GDL_UNDEF;

    SizeT nParam=e->NParam(2);

    // Interpret keywords

    // Return-Type

    static int kwIxReturnType = e->KeywordIx("RETURN_TYPE");
    DIntGDL* KWReturnType     = e->IfDefGetKWAs<DIntGDL>(kwIxReturnType);
    if (KWReturnType != NULL && (*KWReturnType)[0] != 0) {
	myReturnType = (DType) (*KWReturnType)[0];
    }
    
    static int B_VALUEIx = e->KeywordIx("B_VALUE");
    static int I_VALUEIx = e->KeywordIx("I_VALUE");
    static int L_VALUEIx = e->KeywordIx("L_VALUE");
    static int F_VALUEIx = e->KeywordIx("F_VALUE");
    static int D_VALUEIx = e->KeywordIx("D_VALUE");
	static int S_VALUEIx = e->KeywordIx("S_VALUE");
	static int UI_VALUEIx = e->KeywordIx("UI_VALUE");
    static int UL_VALUEIx = e->KeywordIx("UL_VALUE");
    static int L64_VALUEIx = e->KeywordIx("L64_VALUE");
    static int UL64_VALUEIx = e->KeywordIx("UL64_VALUE");
      
    static string TypeKW[16] = {
	"", "B_VALUE", "I_VALUE", "L_VALUE", "F_VALUE", "D_VALUE",
	"", "S_VALUE", "", "", "", "",
	"UI_VALUE", "UL_VALUE", "L64_VALUE", "UL64_VALUE"
    };
    static int TypeKWIx[16] = {
	-1, B_VALUEIx, I_VALUEIx, L_VALUEIx, F_VALUEIx, D_VALUEIx,
	-1, S_VALUEIx, -1 , -1, -1, -1,
	UI_VALUEIx, UL_VALUEIx, L64_VALUEIx, UL64_VALUEIx
    };
    for (int i=0; i<16; i++) {
//	if (strcmp(TypeKW[i].c_str(), "") == 0) {continue;}
	if (TypeKWIx[i] == -1) {continue;}
//	int kwIxType = e->KeywordIx(TypeKW[i]); //should be made static to speed up
//	if (e->KeywordSet(kwIxType)) {
	if (e->KeywordSet(TypeKWIx[i])) {
	    if (myReturnType != GDL_UNDEF) {
		e->Throw("Multiple requests of return type not allowed: "
			 + TypeKW[i] );
	    }
	    myReturnType = (DType)i;
	}
    }

    if (myReturnType == GDL_UNDEF) {
	myReturnType = GDL_LONG;
    }

    // STRUCT_ALIGN_BYTES

    static int kwIxStructAlign = e->KeywordIx("STRUCT_ALIGN_BYTES");
    DIntGDL* KWStructAlign     = e->IfDefGetKWAs<DIntGDL>(kwIxStructAlign);
    if (KWStructAlign != NULL && (*KWStructAlign)[0] != 0) {
	myAlign = (SizeT) (*KWStructAlign)[0];
    }

    // UNLOAD

    static int kwIxUnload  = e->KeywordIx("UNLOAD");
    bool flagUnload = e->KeywordSet(kwIxUnload);

    // parameters by value/reference/GDL reference

    static int kwIxAllValue  = e->KeywordIx("ALL_VALUE");
    bool flagAllValue = e->KeywordSet(kwIxAllValue);

    static int kwIxAllGdl  = e->KeywordIx("ALL_GDL");
    bool flagAllGdl = e->KeywordSet(kwIxAllGdl);
    if (flagAllValue && flagAllGdl) {
	e->Throw("Conflicting keywords ALL_VALUE and ALL_GDL");
    }

    vector<short> byValue(nParam-2,0);

    for (SizeT i=0; i<nParam-2;i++) {
	byValue[i] = flagAllValue ? 1 : flagAllGdl ? -1 : 0;
    }

    static int kwIxValue = e->KeywordIx("VALUE");
    DIntGDL* KWValue     = e->IfDefGetKWAs<DIntGDL>(kwIxValue);
    if (KWValue != NULL) {
	if (flagAllValue || flagAllGdl) {
	    e->Throw("Only one of VALUE,  ALL_VALUE, or ALL_GDL allowed");
	}
	if (KWValue->N_Elements() < nParam - 2) {
	    e->Throw("VALUE must have an entry for every parameter");
	}
	for (SizeT i=0; i<nParam-2;i++) {
	    byValue[i] = (short)(*KWValue)[i];
	}
    }

    e->AssureStringScalarPar( (SizeT)0, image);
    e->AssureStringScalarPar( (SizeT)1, entry);

    int argc      = nParam-2;
    // must be void** for dl... stuff
    void **argv   = (void**)malloc((nParam-2) * sizeof(void*) );
    if (argv == NULL) {
	e->Throw("Internal error allocating memory for argv");
    }
    GDLGuard<void*,void,void> argvGuard(argv, free);
    
    // test if we pass sufficently wide values in case some INT values are shorter than this machine's C "int".
    bool upgrade[nParam-2];
    bool small_int= (sizeof(DInt) < sizeof(int));
    bool small_uint= (sizeof(DUInt) < sizeof(int)); //expendable, UInt and Int have same size!
    for(SizeT i =2; i < nParam; i++){
      BaseGDL* par = e->GetParDefined(i);
      DType    pType  = par->Type();
      upgrade[i-2]=false;
      if ( pType==GDL_INT && small_int) upgrade[i-2]=true; //Warning(e->GetParString(i)+" is of type INT, too short to be passed to the called function, use LONG");}
      if ( pType==GDL_UINT && small_uint) upgrade[i-2]=true; //Warning(e->GetParString(i)+" is of type UINT, too short to be passed to the called function, use ULONG");}
    }
    // Fill argv with the parameters

    for(SizeT i =2; i < nParam; i++){
	BaseGDL* par = e->GetParDefined(i);
	DType    pType  = par->Type();

	if (byValue[i-2] > 0 ) {	// By Value
	    if (! par->StrictScalar() ) {
		e->Throw("Must be a scalar here: "
			 + e->GetParString(i)
		);
	    }

	    if (NumericType(pType)) { //contains INT and UINT
		if (par->Sizeof() > sizeof(void*)) {
		    e->Throw("Parameter is larger than pointer: "
			     + e->GetParString(i)
		    );
		}
        if (upgrade[i-2]) {
            DLong intpar=0; e->AssureLongScalarPar(i,intpar); //normally should be specialzed for UInts.
            memcpy(&argv[i-2], (void*) &intpar, sizeof(DLong));
        }
        else
		memcpy(&argv[i-2], (void*) par->DataAddr(), par->Sizeof());
	    }
	    else if (pType == GDL_STRING) {
		argv[i-2] = (void*) (*(DStringGDL*)(par))[0].c_str();
	    }
	    else {
		e->Throw("Unsupported type for call by value: "
			 + e->GetParString(i)
		);
	    }
	}
	else if (byValue[i-2] < 0) {		// by GDL pointer
	    argv[i-2] = (void*) par;
	}
	else {					// By reference (default)
        if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ)
        {
          argv[i - 2] = (void*) par->DataAddr();
        }
	    else if (pType == GDL_STRING) {
		argv[i-2] = (void*) ce_StringGDLtoIDL(e, par);
	    }
	    else if (pType == GDL_STRUCT) {
		argv[i-2] = ce_StructGDLtoIDL(e, par, NULL, myAlign);
	    }
	    else {
		e->Throw("Objects and Pointers not yet supported");
	    }
	}
    }

    // Load shared object, call function

#if defined(_WIN32) && !defined(__CYGWIN__)
	LPWSTR wchr = new WCHAR[image.size() + 1];
    wchr[image.size()] = 0;
    std::copy(image.begin(), image.end(), wchr);
    HMODULE handle = LoadLibraryW(wchr);
    delete(wchr);
#else
    // note following code line is correct for gcc and linux.
    // in case of trouble on some architecture, find the correct option and make an #ifdef.
    // Do *not* modify the following line. GD.
    void* handle =  dlopen(image.c_str(),  RTLD_NOW | RTLD_GLOBAL);
#endif
    if (handle == NULL) {
#if !defined(_WIN32) || defined(__CYGWIN__)
	cout << dlerror() << endl;
#endif
	e->Throw("Error opening shared object: " + image);
    }

#if defined(_WIN32) && !defined(__CYGWIN__)
    void* func = (void *)GetProcAddress(handle,entry.c_str());
#else
    void* func = dlsym(handle,entry.c_str());
#endif
    if (func == NULL) {
	e->Throw("Entry not found: " + entry);
    }

    union {
	DByte		d_byte;
	DInt		d_int;
	DLong		d_long;
	DFloat		d_float;
	DDouble		d_double;
	DUInt		d_uint;
	DULong		d_ulong;
	DLong64		d_long64;
	DULong64	d_ulong64;
	char*		d_string;
    } ret;

    switch (myReturnType) {
	case GDL_BYTE:    ret.d_byte    = ((DByte(*)   (int, void**))func)(argc, argv);
		      break;
	case GDL_INT:     ret.d_int     = ((DInt(*)    (int, void**))func)(argc, argv);
		      break;
	case GDL_LONG:    ret.d_long    = ((DLong(*)   (int, void**))func)(argc, argv);
		      break;
	case GDL_FLOAT:   ret.d_float   = ((DFloat(*)  (int, void**))func)(argc, argv);
		      break;
	case GDL_DOUBLE:  ret.d_double  = ((DDouble(*) (int, void**))func)(argc, argv);
		      break;
	case GDL_UINT:    ret.d_uint    = ((DUInt(*)   (int, void**))func)(argc, argv);
		      break;
	case GDL_ULONG:   ret.d_ulong   = ((DULong(*)  (int, void**))func)(argc, argv);
		      break;
	case GDL_LONG64:  ret.d_long64  = ((DLong64(*) (int, void**))func)(argc, argv);
		      break;
	case GDL_ULONG64: ret.d_ulong64 = ((DULong64(*)(int, void**))func)(argc, argv);
		      break;
	case GDL_STRING:  ret.d_string  = ((char*(*)   (int, void**))func)(argc, argv); 
		      break;
	default:      e->Throw("Return type not supported: " + i2s(myReturnType) );
		      break;
    }

    if (flagUnload) {
#if defined(_WIN32) && !defined(__CYGWIN__)
	while (! FreeLibrary(handle) ) {}
#else
	while (! dlclose(handle) ) {}
#endif
    }
// necessary since struct is freed below, i do not see how??? (FIXME)
    if (myReturnType == GDL_STRING) {s=ret.d_string;}
    // Copy strings and structures back to GDL, free memory

    for (SizeT i = nParam - 1; i >= 2; i--) {
	if (byValue[i-2] != 0) {continue;}
      BaseGDL* par = e->GetParDefined(i);
      SizeT pType = par->Type();
      if (pType == GDL_STRING) {
        ce_StringIDLtoGDL((EXTERN_STRING*) argv[i - 2], par, 1);
      }
      else if (pType == GDL_STRUCT) {
        ce_StructIDLtoGDL(e, argv[i - 2], par, 1, myAlign);
      }
    }

    // Return the return value

    switch (myReturnType) {
        case GDL_BYTE:      return new DByteGDL(ret.d_byte);
			break;
        case GDL_INT:
          if (small_int)    return new DIntGDL(ret.d_int);
			break;
        case GDL_LONG:      return new DLongGDL(ret.d_long);
			break;
        case GDL_FLOAT:     return new DFloatGDL(ret.d_float);
			break;
        case GDL_DOUBLE:    return new DDoubleGDL(ret.d_double);
			break;
        case GDL_UINT:
          if (small_uint)   return new DUIntGDL(ret.d_uint);
			break;
        case GDL_ULONG:     return new DULongGDL(ret.d_ulong);
			break;
        case GDL_LONG64:    return new DLong64GDL(ret.d_long64);
			break;
        case GDL_ULONG64:   return new DULong64GDL(ret.d_ulong64);
			break;
        case GDL_STRING:    return new DStringGDL(s);
			break;
        default:
        break;  //already exited due to throw above.
    }
    assert( false);
    return NULL; //no -Wreturn-type warnings.
  }


    // ce_StringGDLtoIDL: Convert a GDL string (array) to the IDL string structure
    //                   used for call_external
    //                   Allocates memory, which should be freed later with
    //                   ce_StringIDLtoGDL

    EXTERN_STRING* ce_StringGDLtoIDL(EnvT* e, const BaseGDL* par) 
    {
	SizeT NEl = par->N_Elements();
	EXTERN_STRING* extstring = (EXTERN_STRING*) malloc(NEl * sizeof(EXTERN_STRING));
	if (extstring == NULL) {
	    e->Throw("Internal error allocating memory for extstring");
	}
	for (SizeT iEl=0; iEl < NEl; iEl++) {
	    // DString     parstring = (*static_cast<DStringGDL*>(par))[iEl];
	    DString   parstring = (*(DStringGDL*)(par))[iEl];
	    extstring[iEl].slen = parstring.length();
	    extstring[iEl].s    = (char*) malloc( (extstring[iEl].slen+1) * sizeof(char) );
	    if (extstring[iEl].s == NULL) {
		e->Throw("Internal error allocating memory for extstring[iEl].s");
	    }
	    strcpy(extstring[iEl].s, parstring.c_str());
	}
	return extstring;
    }

    // ce_StringIDLtoGDL: copy any changes made in the IDL string stucture
    //                   back to an GDL string 
    //                   possibly free any memory allocated by ce_StringGDLtoIDL

    void ce_StringIDLtoGDL( EXTERN_STRING* extstring, BaseGDL* par, int freeMemory)
    {
	SizeT NEl = par->N_Elements();
	for (SizeT iEl=0; iEl < NEl; iEl++) {
	    DString parstring = (*static_cast<DStringGDL*>(par))[iEl];
	    if (strcmp( extstring[iEl].s, parstring.c_str()) ) {
		((*static_cast<DStringGDL*>(par))[iEl]).assign(extstring[iEl].s);
	    }
	    if (freeMemory) {free(extstring[iEl].s); }
	}
	if (freeMemory) {free(extstring); }
    }
    

    // ce_StructGDLtoIDL: Convert a GDL structure (array) to the IDL (C) structure
    //                   used for call_external
    //                   Allocates memory, which should be freed later with
    //                   ce_StructIDLtoGDL

    void* ce_StructGDLtoIDL( EnvT* e, const BaseGDL* par, SizeT* length, SizeT myAlign) 
    {
	SizeT totalSize = ce_LengthOfIDLStruct( e, par, myAlign);
	if (length != NULL) {*length = totalSize;}

	SizeT nEl       = par->N_Elements();
	void* IDLStruct = malloc(nEl * totalSize);
	if (IDLStruct == NULL) {
	    e->Throw("Internal error allocating memory for IDLStruct");
	}

	DStructGDL* s = (DStructGDL*) par;
	SizeT nTags   = s->Desc()->NTags();

	for (SizeT iEl=0; iEl < nEl; iEl++) {
	    char* p = (char*) IDLStruct + iEl*totalSize;
	    for (SizeT iTag=0; iTag < nTags; iTag++) {
		BaseGDL* member = s->GetTag(iTag, iEl);
		DType    pType  = member->Type();
		SizeT length;
		SizeT sizeOf;
		void* source;
		int   doFree = 0;
		if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ) {
		    source = (void*) member->DataAddr();
		    length = member->NBytes();
		    sizeOf = member->Sizeof();
		}
		else if (pType == GDL_STRING) {
		    source = (void*) ce_StringGDLtoIDL(e, member);
		    length = member->N_Elements() * sizeof(EXTERN_STRING);
		    sizeOf = 8;
		    doFree = 1;
		}
		else if (pType == GDL_STRUCT) {
		    source = ce_StructGDLtoIDL( e, member, &length, myAlign );
		    length *= member->N_Elements();
		    sizeOf = 8;
		}
		else {
		    e->Throw("Unsupported type in structure: " + i2s(pType) );
		}
		SizeT align = sizeOf < myAlign ? sizeOf : myAlign;
		if ((SizeT)p % align) {
		    SizeT space = align - (SizeT)p % align;
		    p += space;
		}
		memcpy(p, source, length);
		p+=length;
		if (doFree) {free(source);}
		    
	    }
	}

	return IDLStruct;
    }

    // ce_StructIDLtoGDL: copy any changes made in the IDL stucture
    //                   back to an GDL structure
    //                   possibly free any memory allocated bi ce_StructGDLtoIDL

    void ce_StructIDLtoGDL( EnvT* e,  void* IDLStruct, BaseGDL* par, int freeMemory, SizeT myAlign)
    {
	SizeT nEl     = par->N_Elements();
	DStructGDL* s = (DStructGDL*) par;
	SizeT nTags   = s->Desc()->NTags();

	char* p = (char*) IDLStruct;
	for (SizeT iEl=0; iEl < nEl; iEl++) {
	    for (SizeT iTag=0; iTag < nTags; iTag++) {
		BaseGDL* member = s->GetTag(iTag, iEl);
		DType    pType  = member->Type();
		SizeT length;
		SizeT sizeOf;
		void* dest;
		if (NumericType(pType)) {
		    sizeOf = member->Sizeof();
		}
		else {
		    sizeOf = 8;
		}
		SizeT align = sizeOf < myAlign ? sizeOf : myAlign;
		if ((SizeT)p % align) {
		    SizeT space = align - (SizeT)p % align;
		    p += space;
		}

		if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ) {
		    length = member->NBytes();
		    dest   = (void*) member->DataAddr();
		    memcpy(dest, p, length);
		}
		else if (pType == GDL_STRING) {
		    ce_StringIDLtoGDL((EXTERN_STRING*) p, member, 0);
		    length = member->N_Elements() * sizeof(EXTERN_STRING);
		}
		else if (pType == GDL_STRUCT) {
		    ce_StructIDLtoGDL( e, (void*)p, member, 0, myAlign );
		    length = member->N_Elements() * ce_LengthOfIDLStruct(e, member, myAlign);
		}
		else {
		    e->Throw("Unsupported type");
		}

		p += length;
	    }
	    if ((SizeT)p%myAlign) {
		p+= (myAlign - (SizeT)p%myAlign);
	    }
	}

	if (freeMemory) {
	    free(IDLStruct);
	}
    }

    SizeT ce_LengthOfIDLStruct( EnvT* e, const BaseGDL* par, SizeT myAlign) 
    {
	DStructGDL* s = (DStructGDL*) par;
	SizeT nTags   = s->Desc()->NTags();

	SizeT totalSize = 0;
	SizeT sizeOf;
	for (SizeT iTag=0; iTag < nTags; iTag++) {
	    BaseGDL* member = s->GetTag(iTag);
	    DType    pType  = member->Type();
	    // there is probably no point transfering PTR and OBJ
	    // but if it fits why restrict it? They should never be used though
	    if (NumericType(pType) || pType == GDL_PTR || pType == GDL_OBJ) {
		totalSize += member->NBytes();
		sizeOf    =  member->Sizeof();
	    }
	    else if (pType == GDL_STRING) {
		totalSize += member->N_Elements() * sizeof(EXTERN_STRING);
		sizeOf    =  8;
	    }
	    else if (pType == GDL_STRUCT) {
		totalSize += member->N_Elements() * ce_LengthOfIDLStruct( e, member, myAlign);
		sizeOf    =  8;
	    }
	    else {
		e->Throw("Unsupported type");
	    }

	    SizeT align = sizeOf < myAlign ? sizeOf : myAlign;
	    if (totalSize%align) {
		totalSize+= (align - totalSize%align);
	    }
	}
	if (totalSize%myAlign) {
	    totalSize+= (myAlign - totalSize%myAlign);
	}
	return totalSize;
    }

} // namespace
