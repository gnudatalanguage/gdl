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
#include "gdleventhandler.hpp" //for gdlwait_responsive
#include "dinterpreter.hpp"
#include "basic_pro_jmg.hpp"
#include "gdlhelp.hpp" //for dlmInfo

#if defined(_WIN32) && !defined(__CYGWIN__)
#   include "gtdhelper.hpp" // just a workaround, using QueryPerformanceCounter is better
typedef HMODULE handle_t;
#else
typedef void* handle_t;
#endif

#include "export.hpp"

static SizeT increment=0;

namespace lib {
    using namespace std;

    //search in AllDLMSymbols all functions/procs referring to this dlm name, and clears them to indicate that the "Loaded DLM" message has already been activated.
    //mimics IDL's behaviour although we do not use the same logic, the dlls are loaded when the dlm is read, not when an entry is called.
    void ClearDlmStringReference(std::string &s){
      std::string local_s=s;
      for (auto i=0; i< AllDLMSymbols.size() ;++i) if (AllDLMSymbols[i].second.size() && AllDLMSymbols[i].second == local_s) AllDLMSymbols[i].second.clear();
    }
    std::string MyFunName(size_t t){
        for (std::map<const char*, void*>::iterator it = SysFunDefinitions.begin(); it != SysFunDefinitions.end(); ++it) if ((size_t)(it->second)==t) return it->first;
        return "Unknown";
    }
    std::string MyProName(size_t t){
        for (std::map<const char*, void*>::iterator it = SysProDefinitions.begin(); it != SysProDefinitions.end(); ++it) if ((size_t)(it->second)==t) return it->first;
        return "Unknown";
    }
  
  BaseGDL* CallDllFunc(EnvT* e) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  SizeT pos=(SizeT)(static_cast<DLibFun*> (e->GetPro())->GetFunAddress());
  EXPORT_SYSRTN_FUN calldllfunc= (EXPORT_SYSRTN_FUN) AllDLMSymbols[pos].first;
    if ((AllDLMSymbols[pos].second).size()) {
      std::cout<<"Loaded DLM: "<<AllDLMSymbols[pos].second<<std::endl;
      ClearDlmStringReference(AllDLMSymbols[pos].second);
    }
    int argc = e->NParam();
    EXPORT_VPTR argv[argc];
    bool tempo[argc];
#ifdef GDL_DEBUG
    std::cerr<<"FUNCTION "<<MyFunName((size_t)calldllfunc)<<std::endl;
#endif
    for (auto i = 0; i < argc; ++i) {
      tempo[i] = (e->GetString(i).find('>') != std::string::npos);      // tells if input parameter is temporary (expression)
      argv[i] = GDL_ToVPTR(e->GetPar(i), tempo[i]);
      if (!tempo[i]) PassedVariables[argv[i]]=e->GetString(i) ; //add to list of passed NAMED variables
#ifdef GDL_DEBUG
      std::cerr << e->GetString(i) << "="; if (e->GetPar(i)) e->GetPar(i)->ToStream(std::cerr); else std::cerr<<"<Undefined>"; std::cerr<<std::endl;
#endif
    }
    char *argk = NULL;
    // keywords are passed as _REF_EXTRA struct, we just populate argk with our GDL_KEYWORDS_LIST struct
    SizeT nkw = 0;
    GDL_PASS_KEYWORDS_LIST passed;
    GDL_KEYWORDS_LIST* kws = NULL;
    DStringGDL* refextra;
    if (e->GetPro()->NKey() > 0) {
      //pass in "argk" as a_REF_EXTRA 
      //we cannot directly use the _EXTRA mechanism as some passed values should be writeable or are not defined at the time of calling this function
      if (e->KeywordPresentAndDefined(0)) {
        refextra = e->GetKWAs<DStringGDL>(0);
        nkw = refextra->N_Elements();
        kws = (GDL_KEYWORDS_LIST*) calloc(nkw,sizeof (GDL_KEYWORDS_LIST));
        for (auto i = 0; i < nkw; ++i) {
          kws[i].name = (*refextra)[i].c_str();
          BaseGDL** gvarp = e->GetRefExtraListPtr((*refextra)[i]); //check as Ptr as the variable may not exist
          if (gvarp == NULL) {
            kws[i].readonly = 1;
            //pass the variable anyway using GetRefExtraList
            BaseGDL* gvar = e->GetRefExtraList((*refextra)[i]);
            // Add var name to PassedVariables
            kws[i].varname = e->Caller()->GetString(gvar);
            kws[i].varptr = gvar;
            if (gvar == NULL) kws[i].type = GDL_TYP_UNDEF;
          } else {
            kws[i].readonly = 0;
            kws[i].varptr = *gvarp;
            if (*gvarp == NULL) kws[i].type = GDL_TYP_UNDEF;
            kws[i].varname = e->Caller()->GetString(*gvarp);
          }
#ifdef GDL_DEBUG
          std::cerr<<kws[i].name<<": "<<kws[i].varname<<"="; (kws[i].varptr)->ToStream(std::cerr);
#endif
        }
      }
      passed.npassed = nkw;
      passed.passed = kws;
      argk = (char*) (&passed);
    }
    EXPORT_VPTR ret;
    try{
      ret = calldllfunc(argc, argv, argk);
    } catch (GDLException& e) { throw e;} 
      catch (...) {e->Throw("error in DLM code / unsupported procedure, returning.");
    }
    if (ret->type == GDL_TYP_UNDEF) e->Throw("Variable is undefined: <UNDEFINED>.");
    BaseGDL* back = VPTR_ToGDL(ret, true); //protect data

    for (auto i = 0; i < argc; ++i) {
      if (!tempo[i]) e->SetPar(i, VPTR_ToGDL(argv[i]));
      if (!tempo[i]) PassedVariables.erase(argv[i]); //remove map entry
    }
    //check if some argk keywords have been returned too. A real variable must be associated to be replaced in return
    for (auto i = 0; i < nkw; ++i) {
        if (kws[i].out != NULL) {
        PassedVariables.erase(kws[i].out); //remove map entry
        BaseGDL** gvarp = e->GetRefExtraListPtr((*refextra)[i]); //Ptr as the variable may not exist
        if (gvarp) { //replace parameter's value
          GDLDelete(*gvarp);
          *gvarp = (BaseGDL*) VPTR_ToGDL((EXPORT_VPTR) (kws[i].out));
        } else {
          e->Throw("Unexpected error, variable not existing. Please report.");
        }
      }
    }
    GDL_FreeResources() ;
    return back;
  }

  void CallDllPro(EnvT* e) {TRACE_ROUTINE(__FUNCTION__, __FILE__, __LINE__)
  SizeT pos=(SizeT)(static_cast<DLibPro*> (e->GetPro())->GetProAddress());
    EXPORT_SYSRTN_PRO calldllpro = (EXPORT_SYSRTN_PRO) AllDLMSymbols[pos].first;
    if ((AllDLMSymbols[pos].second).size()) {
      std::cout<<"Loaded DLM: "<<AllDLMSymbols[pos].second<<std::endl;
      ClearDlmStringReference(AllDLMSymbols[pos].second);
    }
    int argc = e->NParam();
    EXPORT_VPTR argv[argc];
    bool tempo[argc];
#ifdef GDL_DEBUG
    std::cerr<<"PROCEDURE "<<MyProName((size_t)calldllpro)<<std::endl;
#endif
    for (auto i = 0; i < argc; ++i) {
      tempo[i] = (e->GetString(i).find('>') != std::string::npos);      // tells if input parameter is temporary (expression)
#ifdef GDL_DEBUG
      std::cerr << e->GetString(i) << "=";
      if (e->GetPar(i)) e->GetPar(i)->ToStream(std::cerr);
      else std::cerr << "<Undefined>";
      std::cerr << std::endl;
#endif
      argv[i] = GDL_ToVPTR(e->GetPar(i), tempo[i]);
      if (!tempo[i]) PassedVariables[argv[i]]=e->GetString(i) ; //add to list of passed NAMED variables
    }
    char *argk = NULL;
    // keywords are passed as _REF_EXTRA struct, we just populate argk with our GDL_KEYWORDS_LIST struct
    SizeT nkw = 0;
    GDL_PASS_KEYWORDS_LIST passed;
    GDL_KEYWORDS_LIST* kws = NULL;
    DStringGDL* refextra;
    if (e->GetPro()->NKey() > 0) {
      //pass in "argk" as a_REF_EXTRA 
      //we cannot directly use the _EXTRA mechanism as some passed values should be writeable or are not defined at the time of calling this function
      if (e->KeywordPresentAndDefined(0)) {
        refextra = e->GetKWAs<DStringGDL>(0);
        nkw = refextra->N_Elements();
        kws = (GDL_KEYWORDS_LIST*) calloc(nkw,sizeof (GDL_KEYWORDS_LIST));
        for (auto i = 0; i < nkw; ++i) {
          kws[i].name = (*refextra)[i].c_str();
          BaseGDL** gvarp = e->GetRefExtraListPtr((*refextra)[i]); //check as Ptr as the variable may not exist
          if (gvarp == NULL) {
            kws[i].readonly = 1;
            //pass the variable anyway using GetRefExtraList
            BaseGDL* gvar = e->GetRefExtraList((*refextra)[i]);
            // Add var name to PassedVariables
            kws[i].varname=e->Caller()->GetString(gvar);
            kws[i].varptr = gvar;
            if (gvar == NULL) kws[i].type = GDL_TYP_UNDEF;
          } else {
            kws[i].readonly = 0;
            kws[i].varptr = *gvarp;
            if (*gvarp == NULL) kws[i].type = GDL_TYP_UNDEF;
            kws[i].varname=e->Caller()->GetString(*gvarp);
          }
#ifdef GDL_DEBUG
          std::cerr<<kws[i].name<<": "<<kws[i].varname<<"="; (kws[i].varptr)->ToStream(std::cerr);
#endif
        }
      }
      passed.npassed = nkw;
      passed.passed = kws;
      argk = (char*) (&passed);
    }
    try{
      calldllpro(argc, argv, argk);
    } catch (GDLException& e) { throw e;} 
      catch (...) {e->Throw("error in DLM code / unsupported procedure, returning.");}
    for (auto i = 0; i < argc; ++i) {
      if (!tempo[i]) e->SetPar(i, VPTR_ToGDL(argv[i]));
      if (!tempo[i]) PassedVariables.erase(argv[i]); //remove map entry
    }
    //check if some argk keywords have been returned too. A real variable must be associated to be replaced in return
    for (auto i = 0; i < nkw; ++i) {
      if (kws[i].out != NULL) {
        BaseGDL** gvarp = e->GetRefExtraListPtr((*refextra)[i]); //Ptr as the variable may not exist
        if (gvarp) {
          GDLDelete(*gvarp);
          *gvarp = (BaseGDL*) VPTR_ToGDL((EXPORT_VPTR) (kws[i].out));
        } else {
          e->Throw("Unexpected error, variable not existing. Please report.");
        }
      }
    }
    GDL_FreeResources() ;
  }

void CleanupProc( DLibPro* proc ) {
// this does not work (double deletion at exit due to ~DllContainer() { unload( true ); } that already uses unlinksymbol.
//    auto it = libProList.begin();
//    auto itE = libProList.end();
//    for( ; it != itE; it++ ) {
//      if( *it == proc ) break;
//    }
//    if( it < itE ) {
//        delete *it;
//      libProList.erase( it );
//      }
    }

  void CleanupFunc( DLibFun* func ) {
// this does not work (double deletion at exit due to ~DllContainer() { unload( true ); } that already uses unlinksymbol.
//    auto it = libFunList.begin();
//    auto itE = libFunList.end();
//    for( ; it != itE; it++ ) {
//      if( *it == func ) break;
//    }
//    if( it < itE ) {
//      delete *it;
//      libFunList.erase( it );
//    }
  }

  struct DllContainer {
    handle_t handle; // Handle to the linked DLL

    set<string> my_procs; // list of procedures linked within this DLL
    set<string> my_funcs; // list of functions linked within this DLL
    set<handle_t> my_handles; // memorize handles to avoid calling IDL_Load() many times

    static map<string, shared_ptr<DLibPro>> all_procs; // list of ALL procedures linked (using linkimage)
    static map<string, shared_ptr<DLibFun>> all_funcs; // list of ALL functions linked (using linkimage)
    static map<string, DllContainer> libs; // list of ALL DLLs linked (using linkimage)

    bool isok=false;
    std::string DllLoadErrorMessage;
    bool addedToHelpDll=false;
    bool IsOK(){return isok;}
    bool IsHelpAdded(){return addedToHelpDll;}
    void HelpAdded(bool b){addedToHelpDll=b;}
    DllContainer( const DllContainer& ) = delete;
    DllContainer( DllContainer&& rhs ) : handle(nullptr) { std::swap( handle, rhs.handle ); };
    DllContainer( const string& fn ) : handle(nullptr) { };//load(fn); }/// do not load at DllContainer definition! will be done better after.
    ~DllContainer() { unload( true ); isok=false;}
    
    void load( const string& fn ) {
      if( handle ) return;     // already loaded.
#if defined(_WIN32) && !defined(__CYGWIN__)
      WCHAR u_shrdimgName[255];
      MultiByteToWideChar(CP_ACP, 0, fn.c_str(), fn.length(), u_shrdimgName, 255);
      handle = LoadLibraryW(u_shrdimgName);
      if( !handle ) DllLoadErrorMessage = fn; else isok=true;
#else
      handle = dlopen(fn.c_str(), RTLD_LAZY|RTLD_GLOBAL);
      if( !handle ) DllLoadErrorMessage = string(dlerror()); else isok=true;
#endif
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
      isok=false;
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

    void RegisterSymbolDefinedByIDL_Load(const string& dlm_name, const string& proc_name, DLong funcType, DLong max_args = 16, DLong min_args = 0, bool has_keys = false) {
      if (!handle) {
        throw runtime_error("Library not loaded!");
      } else if (funcType < 0 || funcType > 1) {
        throw runtime_error("Improper function type: " + to_string(funcType));
      }
      const char * name = proc_name.c_str();
      if (funcType == 0) {
        bool found = false;
        for (std::map<const char*, void*>::iterator it = SysProDefinitions.begin(); it != SysProDefinitions.end(); ++it) {
          if (strcmp(it->first, name) == 0) {
            //find lib_symbol in SysPropDefinitions
            RegisterMediatizedProc(dlm_name, it->second, proc_name, max_args, min_args, has_keys);
            found = true;
            break;
          }
        }
        if (!found) throw runtime_error("DLM's IDL_Load() does not define procedure " + proc_name);
        } else {
        bool found = false;
        for (std::map<const char*, void*>::iterator it = SysFunDefinitions.begin(); it != SysFunDefinitions.end(); ++it) {
          if (strcmp(it->first, name) == 0) {
            //find lib_symbol in SysFunDefinitions
            RegisterMediatizedFunc(dlm_name, it->second, proc_name, max_args, min_args, has_keys);
            found = true;
            break;
          }
        }
        if (!found) throw runtime_error("DLM's IDL_Load()  does not define function " + proc_name);
        }
      }
    
    void RegisterSymbol(const string & dlm_name, const string& entry_name, const string& proc_name, DLong funcType, DLong max_args=16, DLong min_args=0, bool has_keys=false) {
      if( !handle ) {
	throw runtime_error( "Library not loaded!" );
      } else if( funcType < 0 || funcType>1 ) {
	throw runtime_error( "Improper function type: "+to_string(funcType) );
      }
      if( funcType == 0 ) {
	RegisterMediatizedProc( dlm_name, MakeTarget(entry_name,proc_name), proc_name, max_args, min_args, has_keys);
      } else {
	RegisterMediatizedFunc( dlm_name, MakeTarget(entry_name,proc_name), proc_name, max_args, min_args, has_keys);
      }
    }
 
    void RegisterNativeSymbol( const string& entry_name, const string& proc_name, DLong funcType, DLong max_args=16, DLong min_args=0, string keyNames[]=NULL) {
      if( !handle ) {
	throw runtime_error( "Library not loaded!" );
      } else if( funcType < 0 || funcType>1 ) {
	throw runtime_error( "Improper function type: "+to_string(funcType) );
      }
      if( funcType == 0 ) {
	RegisterNativeProc( entry_name, proc_name, max_args, min_args, keyNames);
      } else {
	RegisterNativeFunc( entry_name, proc_name, max_args, min_args, keyNames);
      }
    }
    void UnregisterSymbol( const string& proc_name, DLong funcType ) {
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
    //for IDL-compatible library, try IDL_Load, that may define useful variables or structures before the use of the othe functions/procedures inside.
    int CallLoadToDefineEntryLocations(){
      if (my_handles.count(handle) > 0) return 1; //already known
      void* fPtr=NULL;
      char* error = nullptr;
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (void*) GetProcAddress( handle, "IDL_Load" );
#else
      error = dlerror();  // clear error
      fPtr = dlsym( handle, "IDL_Load" );
      error = dlerror();
#endif
      if(fPtr) { //IDL_Load() may be absent, no probs for us.
        // if it is present, we call it. If it contains calls to IDL_SysRtnAdd(), then IDL_SysRtnAdd() will to load all the definitions, they will not be defined again.
        int (*Call_IDL_Load)(void) = ( int (*)(void)) fPtr;
        int ret ;
        try{
          ret = Call_IDL_Load();
        } catch (GDLException ex) { throw GDLException("Error while calling IDL_Load(): "+ex.toString());}
          catch (...) { throw GDLException("Error while calling IDL_Load()");}
        if (ret) my_handles.insert(handle); // IDL_Load has been called
        return ret;
      }
      return 0;
    }
   template <typename T>
    T LinkAs(const string& lib_symbol, const string& proc_name) {
      T fPtr = nullptr;
      char* error = nullptr;
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (T) GetProcAddress(handle, lib_symbol.c_str());
#else
      error = dlerror(); // clear error
      fPtr = (T) dlsym(handle, lib_symbol.c_str());
      error = dlerror();
#endif
      if (error) { //try lowercase (DLMs are written in UPPERCASE)
        std::string s=StrLowCase(lib_symbol);
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (T) GetProcAddress( handle, s.c_str() );
#else
      error = dlerror();  // clear error
      fPtr = (T) dlsym( handle, s.c_str() );
      error = dlerror();
#endif
      }
      if (error) { //try uppercase?
        std::string s=StrUpCase(lib_symbol);
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (T) GetProcAddress( handle, s.c_str() );
#else
      error = dlerror();  // clear error
      fPtr = (T)  dlsym( handle, s.c_str() );
      error = dlerror();
#endif
      }
      if (error) {
        throw runtime_error("Failed to register DLL-routine: " + proc_name + string(" -> ") + lib_symbol + string(" : ") + error);
      }
      return fPtr;
    }
    
    void* MakeTarget( const string& lib_symbol, const string& proc_name ) { 
      void* fPtr = nullptr;
      char* error = nullptr;
      // if 'lib_symbol' does not work, try both upcase and lowcase versions
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (void*) GetProcAddress( handle, lib_symbol.c_str() );
#else
      error = dlerror();  // clear error
      fPtr = dlsym( handle, lib_symbol.c_str() );
      error = dlerror();
#endif
      // if 'lib_symbol' does not work, try both upcase and lowcase versions
      if (error) { //try lowercase (DLMs are written in UPPERCASE)
        std::string s=StrLowCase(lib_symbol);
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (void*) GetProcAddress( handle, s.c_str() );
#else
      error = dlerror();  // clear error
      fPtr = dlsym( handle, s.c_str() );
      error = dlerror();
#endif
      }
      if (error) { //try uppercase?
        std::string s=StrUpCase(lib_symbol);
#if defined(_WIN32) && !defined(__CYGWIN__)
      fPtr = (void*) GetProcAddress( handle, s.c_str() );
#else
      error = dlerror();  // clear error
      fPtr = dlsym( handle, s.c_str() );
      error = dlerror();
#endif
      }
      if( error ) {
	throw runtime_error( "Failed to register DLL-routine: " + proc_name + string(" -> ") + lib_symbol + string(" : ") + error );
      }
      return fPtr;
    }
#define ADD_PROC_TO_INFOLIST_AND_INCREMENT_COUNTER std::pair <void*, std::string> newpair;\
      newpair.first=target;\
      newpair.second=dlm_name;\
      AllDLMSymbols.push_back(newpair);

    void RegisterMediatizedProc(const string& dlm_name, void* target, const string& proc_name, DLong max_args, DLong min_args, bool has_keys) {
      if (all_procs.count(proc_name)) return;
      // this method of 'cleaning' is excellent but show a problem when exiting, seen only with valgrind.
      // Not clear why, as calling .FULL_RESET prior exiting does not show this error. 
      all_procs[proc_name].reset(
          new DLibPro((void (*)(EnvT * e)) (increment++), (void*) CallDllPro,  proc_name, max_args, min_args, has_keys),
          CleanupProc
          );
      my_procs.insert(proc_name);
      ADD_PROC_TO_INFOLIST_AND_INCREMENT_COUNTER
    }
    
    void RegisterNativeProc( const string& lib_symbol, const string& proc_name, DLong max_args, DLong min_args, const string keyNames[]) { 
      if( all_procs.count( proc_name ) ) return;
      all_procs[proc_name].reset(
				 new DLibPro( LinkAs<LibPro>( lib_symbol, proc_name ), proc_name.c_str(), max_args, keyNames, NULL, min_args),
				 CleanupProc
				 );
      my_procs.insert(proc_name);
    }

    void RegisterMediatizedFunc(const string& dlm_name, void* target, const string& func_name, DLong max_args, DLong min_args, bool has_keys) { 
      if( all_funcs.count( func_name ) ) return;
      // this method of 'cleaning' is excellent but show a problem when exiting, seen only with valgrind.
      all_funcs[func_name].reset(
				 new DLibFun((BaseGDL* (*)(EnvT* e)) (increment++), (void*)CallDllFunc , func_name, max_args,  min_args, has_keys),
				 CleanupFunc
				 );
      my_funcs.insert(func_name);
      ADD_PROC_TO_INFOLIST_AND_INCREMENT_COUNTER
     }

     void RegisterNativeFunc( const string& lib_symbol, const string& func_name, DLong max_args, DLong min_args,  string keyNames[]) { 
      if( all_funcs.count( func_name ) ) return;
      all_funcs[func_name].reset(
				 new DLibFun( LinkAs<LibFun>( lib_symbol, func_name ), func_name.c_str(), max_args, keyNames, NULL, min_args),
				 CleanupFunc
				 );
      my_funcs.insert(func_name);
      }
  
    bool isLoaded( void ) { return handle; };        
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
  void truncate_lun( EnvT* e) 
  {
    SizeT nPar=e->NParam();
    
    DLong lun;
    for (SizeT ix=0; ix < nPar; ++ix) {
      e->AssureLongScalarPar( ix, lun);

      if( lun > maxLun || lun < 0)
        throw GDLException( e->CallingNode(), 
			    "TRUNCATE_LUN:  File unit is not within allowed range.");
      if( lun == 0)
        throw GDLException( e->CallingNode(), 
			    "TRUNCATE_LUN: Operation is invalid on a terminal. Unit: 0, File: <stdin>");
      GDLStream& actUnit = fileUnits[ abs(lun)-1];
      if( !actUnit.IsWriteable()) throw GDLException( e->CallingNode(), 
						      "TRUNCATE_LUN:  File unit is not open for output. Unit: " +i2s(abs(lun)));
      if( !actUnit.IsOpen()) 
        throw GDLException( e->CallingNode(), 
			    "TRUNCATE_LUN:  File unit is not open. Unit: " +i2s(abs(lun)));
      if ( actUnit.Compress()) throw GDLException( e->CallingNode(), 
						   "TRUNCATE_LUN:  Not available for Compressed files, please complain!");
      
      actUnit.Truncate();
    }
  }

  void copy_lun(EnvT* e) {
    SizeT nPar = e->NParam(2);

    DLong lun1;
    e->AssureLongScalarPar(0, lun1);

    if (lun1 > maxLun || lun1 < 0) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not within allowed range.");
    if (lun1 == 0) throw GDLException(e->CallingNode(), "COPY_LUN: Operation is invalid on a terminal. Unit: 0, File: <stdin>");

    GDLStream& actUnit1 = fileUnits[ abs(lun1) - 1];
    if (!actUnit1.IsOpen()) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not open. Unit: " + i2s(abs(lun1)));

    DLong lun2;
    e->AssureLongScalarPar(1, lun2);

    if (lun2 > maxLun || lun2 < 0) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not within allowed range.");
    if (lun2 == 0) throw GDLException(e->CallingNode(), "COPY_LUN: Operation is invalid on a terminal. Unit: 0, File: <stdin>");
    GDLStream& actUnit2 = fileUnits[ abs(lun2) - 1];

    if (!actUnit2.IsOpen()) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not open. Unit: " + i2s(abs(lun2)));
    if (!actUnit2.IsWriteable()) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not open for Write. Unit: " + i2s(abs(lun2)));

    static int tcIx = e->KeywordIx("TRANSFER_COUNT");
    bool docount = e->KeywordPresent(tcIx);
    static int ixLINES = e->KeywordIx("LINES");
    bool doline = e->KeywordSet(ixLINES);
    static int ixEOF = e->KeywordIx("EOF");
    bool doeof = e->KeywordSet(ixEOF);
    if (nPar == 2) doeof = true;
    // if docount, set param to zero beforehand in case of early return on error.
    DLong64GDL* returnedCount;
    std::streamoff currentPos = actUnit1.Tell();
    if (docount) {
      returnedCount = new DLong64GDL(0);
      e->SetKW(tcIx, returnedCount);
    }
    bool doThrow = !doeof;
    if (doline) {
      DLong nlines;
      if (doeof) nlines = std::numeric_limits<DLong>::max(); //copy until the end: read a big number, but do not throw on EOF
      else e->AssureLongScalarPar(2, nlines);
      DLong ret = actUnit1.CopySomeLinesTo(nlines, actUnit2, doThrow);
      if (docount) (*returnedCount)[0] = ret;
    } else {
      DLong64 pos;
      if (doeof) pos = std::numeric_limits<DLong64>::max(); //copy until the end: read a big number, but do not throw on EOF 
      else e->AssureLongScalarPar(2, pos);
      DLong64 ret = actUnit1.CopySomeTo(pos, actUnit2, doThrow);
      if (docount) (*returnedCount)[0] = ret;
    }
  }

  void skip_lun(EnvT* e) {
    SizeT nPar = e->NParam(1);

    DLong lun;
    e->AssureLongScalarPar(0, lun);

    if (lun > maxLun || lun < 0) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not within allowed range.");
    if (lun == 0) throw GDLException(e->CallingNode(), "COPY_LUN: Operation is invalid on a terminal. Unit: 0, File: <stdin>");

    GDLStream& actUnit = fileUnits[ abs(lun) - 1];
    if (!actUnit.IsOpen()) throw GDLException(e->CallingNode(), "COPY_LUN:  File unit is not open. Unit: " + i2s(abs(lun)));

    static int tcIx = e->KeywordIx("TRANSFER_COUNT");
    bool docount = e->KeywordPresent(tcIx);
    static int ixLINES = e->KeywordIx("LINES");
    bool doline = e->KeywordSet(ixLINES);
    static int ixEOF = e->KeywordIx("EOF");
    bool doeof = e->KeywordSet(ixEOF);
    if (nPar == 1) doeof = true;
    // if docount, set param to zero beforehand in case of early return on error.
    DLong64GDL* returnedCount;
    std::streamoff currentPos = actUnit.Tell();
    if (docount) {
      returnedCount = new DLong64GDL(0);
      e->SetKW(tcIx, returnedCount);
    }
    bool doThrow = !doeof;
    if (doline) {
      DLong nlines;
      if (doeof) nlines = std::numeric_limits<DLong>::max(); //copy until the end: read a big number, but do not throw on EOF
      else e->AssureLongScalarPar(1, nlines);
      DLong ret = actUnit.SkipLines(nlines, doThrow);
      if (docount) (*returnedCount)[0] = ret;
    } else {
      DLong64 pos;
      if (doeof) pos = std::numeric_limits<DLong64>::max(); //copy until the end: read a big number, but do not throw on EOF 
      else e->AssureLongScalarPar(1, pos);
      DLong64 ret = actUnit.Skip(pos,  doThrow);
      if (docount) (*returnedCount)[0] = ret;
    }
  }
  void dlm_load( EnvT* e ) {
    // do nothing as long we do load the .so segment when opening a dlm.
    // if not, this would be a good place to call DllContainer::get( shrdimgName ); (see below)
  }
  
  //linkimage is used by all DLM-related stuff, but should behave differently when called 'a la DLM'.
  //Hence the use of DLM_INFO string array.
  
#define COPYSTR(what)      IDL_SysvVersion.what.slen=what.length();\
  IDL_SysvVersion.what.s=(char*)malloc(what.length());\
  strncpy(IDL_SysvVersion.what.s,what.c_str(),what.length());
 
  void linkimage( EnvT* e ) {
    
    static bool IdlStaticsUninitialized=true;
    if (IdlStaticsUninitialized) {
      IdlStaticsUninitialized=false;
      //set up values for some EXPORT_XXX info structures
      //EXPORT_SysvVersion:
      DStructGDL* version = SysVar::Version();
      static unsigned releaseTag = version->Desc()->TagIndex( "RELEASE");
      static unsigned osTag = version->Desc()->TagIndex("OS");
      static unsigned osFamilyTag = version->Desc()->TagIndex("OS_FAMILY");
      static unsigned osNameTag = version->Desc()->TagIndex("OS_NAME");
      static unsigned osBuildTag = version->Desc()->TagIndex("BUILD_DATE");
      static unsigned archTag = version->Desc()->TagIndex("ARCH");
      static unsigned mTag = version->Desc()->TagIndex("MEMORY_BITS");
      static unsigned foTag = version->Desc()->TagIndex("FILE_OFFSET_BITS");
      DString arch =     (*static_cast<DStringGDL*> (version->GetTag(archTag, 0)))[0];
      COPYSTR(arch)
      DString os =       (*static_cast<DStringGDL*> (version->GetTag(osTag, 0)))[0];
      COPYSTR(os)
      DString os_family =       (*static_cast<DStringGDL*> (version->GetTag(osFamilyTag, 0)))[0];
      COPYSTR(os_family)
      DString os_name =       (*static_cast<DStringGDL*> (version->GetTag(osNameTag, 0)))[0];
      COPYSTR(os_name)
      DString release =  (*static_cast<DStringGDL*>( version->GetTag( releaseTag, 0)))[0];
      COPYSTR(release)
      DString build_date =       (*static_cast<DStringGDL*> (version->GetTag(osBuildTag, 0)))[0];
      COPYSTR(build_date)
      DInt memory_bits =       (*static_cast<DIntGDL*> (version->GetTag(mTag, 0)))[0]; 
      IDL_SysvVersion.memory_bits=memory_bits;
      DInt file_offset_bits =       (*static_cast<DIntGDL*> (version->GetTag(foTag, 0)))[0]; 
      IDL_SysvVersion.file_offset_bits=file_offset_bits;
      //IDL_SysvDir:
      DString dir=SysVar::Dir();
      IDL_SysvDir.slen=dir.length();
      IDL_SysvDir.s=(char*)malloc(dir.length());
      strncpy(IDL_SysvDir.s,dir.c_str(),dir.length());
    }
#undef COPYSTR
    
    SizeT nP = e->NParam(2);
 
    DString funcName;
    e->AssureScalarPar<DStringGDL>( 0, funcName );

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
    static int nativeIx = e->KeywordIx("NATIVE");
    bool isNativeGdl=e->KeywordSet(nativeIx); //this is a GDL-native .so 
    static int dlminfoIx = e->KeywordIx("DLM_INFO");
    bool isDlm=e->KeywordSet(dlminfoIx);

    if( e->KeywordSet( functIx ) ) funcType = 1;
    
    DLong max_args = 16;
    e->AssureLongScalarKWIfPresent( maxargsIx, max_args );
    DLong min_args = 0;
    e->AssureLongScalarKWIfPresent( minargsIx, min_args );

    bool hasKeywords=( e->KeywordSet( keywordsIx ) );
    //if 'native' (gdl) then if keywords, these are a DStringGDL
    
    if( entryName.empty() ) {
      entryName = funcName;
    }

//    try {
      DStringGDL* info = NULL;
      if (isDlm) info = e->GetKWAs<DStringGDL>(dlminfoIx);
      DllContainer& lib = DllContainer::get(shrdimgName);
      if (lib.IsOK()) {
        if (isNativeGdl) {
          //if 'native' (gdl) then if keywords, these are a DStringGDL
          if (hasKeywords) {
            BaseGDL* p = e->GetKW(keywordsIx);
            if (p->Type() != GDL_STRING) e->Throw("KEYWORDS: Expecting STRING keywords.");
            // GD: attention Keyword list must be null terminated!
            DStringGDL* crude = e->GetKWAs<DStringGDL>(keywordsIx);
            DStringGDL* null_terminated = new DStringGDL(dimension(crude->N_Elements() + 1));
            for (auto i = 0; i < p->N_Elements(); ++i) (*null_terminated)[i] = (*crude)[i];
            lib.RegisterNativeSymbol(entryName, funcName, funcType, max_args, min_args, &((*null_terminated)[0]));
          } else lib.RegisterNativeSymbol(entryName, funcName, funcType, max_args, min_args);
        } else {
          int ret = 0;
          if (isDlm) {
            // test if "IDL_Load() can be called to give, noot symbol names, but symbols addresses
            try {
              ret = lib.CallLoadToDefineEntryLocations();
            } catch (GDLException ex) { Warning((*info)[0]+": Recovering from error: "+ex.toString()); e->Interpreter()->RetAll();}
            if (ret) { //if we got directly symbol addresses, use a special function
              lib.RegisterSymbolDefinedByIDL_Load((*info)[0],funcName, funcType, max_args, min_args, hasKeywords);
              if (!lib.IsHelpAdded()) { //can still be unloaded due to problems.
                help_AddDlmInfo(info->Dup()); //only once
                lib.HelpAdded(true);
              }
              if (funcType) {//insert DLM name in lib/pro structure to enable the "Loaded XXXX" message at first use.
                int funIx=LibFunIx(funcName);
                } else {
                int proIx=LibProIx(funcName);
                }
            } else lib.RegisterSymbol((*info)[0], entryName, funcName, funcType, max_args, min_args, hasKeywords); //not a dlm, or dlm does not have ILD_Load()
          }
        }
      } else {
        e->Throw("Error linking procedure/DLL: " + funcName + " -> " + entryName + "  (" + shrdimgName + ") : " +lib.DllLoadErrorMessage); e->Interpreter()->RetAll();}
//    } catch (GDLException ex) {
//      e->Throw(ex.toString());
//    } catch (const std::exception& ex) {
//      e->Throw("Error linking procedure/DLL: " + funcName + " -> " + entryName + "  (" + shrdimgName + ") : " + ex.what());
//    } catch (...) { e->Throw("Unexpected exception.");}
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
      lib.UnregisterSymbol( upCasefuncName, funcType );
      lib.unload();       // will only unload if all symbols have been unregistered.
    } catch ( const std::exception& ex ) {
      e->Throw("Error unlinksymbol: " + funcName + "  (" + shrdimgName + ") : " + ex.what() );
    }
    
  }

  
  void ResetDLLs( void ) {
    DllContainer::clear();
  }
  
  //idem WAIT_PRO, but needed in idlneturl as we wait while some widgets events must happen.
  //should NEVER be used elsewhere (the idlneturl.pro procedure should disappear in favor of a pure C/C++ use of the CURL library)
  //The real WAIT blocks absolutely anything --- as it should.
  void gdlwait_responsive(EnvT* e) {
    e->NParam(1); //, "WAIT");

    DDouble waittime;
    e->AssureDoubleScalarPar(0, waittime);

    if (waittime < 0)
      throw GDLException(e->CallingNode(),
        "WAIT: Argument must be non-negative"
        + e->GetParString(0));

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
      elapsed.QuadPart = (Endtime.QuadPart - BeginTime.QuadPart) / (Frequency.QuadPart / 1000000);
      if (elapsed.QuadPart >= waittime_us.QuadPart) break;
      else if (elapsed.QuadPart > 100) Sleep(80);
    }
#else
    int old_version = 0;

    if (waittime <= 0.005) old_version = 1;

    // AC 2010-09-16
    // this version is OK and very accurate for small durations
    // but used 100% of one CPU :((
    if (old_version == 1) {
      struct timeval tval;
      struct timezone tzone;

      // derivated from the current version of SYSTIME()
      gettimeofday(&tval, &tzone);
      double t_start = tval.tv_sec + tval.tv_usec / 1e+6; // time in UTC seconds
      double t_current = 0.0;

      double diff = 0.0;
      while (diff < waittime) {

        gettimeofday(&tval, &tzone);
        t_current = tval.tv_sec + tval.tv_usec / 1e+6;
        diff = t_current - t_start;
      }
    }

    // AC 2010-09-16 this version should used much less CPU !
    if (old_version == 0) {
      //cout << floor(waittime) << " " <<  waittime-floor(waittime) << endl;
      struct timespec tv;
      tv.tv_sec = floor(waittime);
      tv.tv_nsec = (waittime - floor(waittime))*1e9;
      int retval;
      retval = nanosleep(&tv, NULL);
    }
#endif
   GDLEventHandler(); //this is probably not OK, but is at the moment needed to permit delivery of callback functions in idlneturl__define.pro where a waiting loop uses teh WAIT command.  
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

    //AC24 see discussions/1708 (#1708)
    // /CDECL is silently ignore on Linux/OSX 
    static int cdeclIx = e->KeywordIx("CDECL");
    #if defined(_WIN32)
    Warning("/CDECL on MSwin is not tested !");
    #endif

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
    void* handle =  dlopen(image.c_str(), RTLD_LAZY);
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
