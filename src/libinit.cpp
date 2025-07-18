/***************************************************************************
                     libinit.cpp  -  initialization of GDL library routines
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

#include <string>
#include <fstream>

#include "envt.hpp"
#include "dpro.hpp"
#include "objects.hpp"

#include "basic_fun.hpp"
#include "basic_pro.hpp"
#include "gdlhelp.hpp"

#include "list.hpp"
#include "hash.hpp"

#include "math_fun.hpp"
#include "math_fun_ac.hpp"
#include "math_fun_gm.hpp"
#include "math_fun_ng.hpp"
#include "plotting.hpp"
#include "sorting.hpp"

#include "file.hpp"

#include "gsl_fun.hpp"

#include "where.hpp"
#include "convol.hpp"
#include "smooth.hpp"
//#include "brent.hpp" //now a dlm
#include "linearprogramming.hpp"
#include "saverestore.hpp"

#include "terminfo.hpp"

#ifdef USE_PYTHON
#  include "gdlpython.hpp"
#endif

#include "grib.hpp"
#include "semshm.hpp"
#ifndef _WIN32
#include "shm.hpp"
#endif
#include "labelregion.hpp"
#include "value_locate.hpp"
#include "gdl2gdl.hpp"

using namespace std;

// declare here other library init functions
void LibInit_jmg(); // libinit_jmg.cpp
void LibInit_cl(); // libinit_cl.cpp
void LibInit_mes(); // libinit_mes.cpp
void LibInit_ac(); // libinit_ac.cpp
void LibInit_gm(); // libinit_gm.cpp
void LibInit_ng(); // libinit_ng.cpp
void LibInit_jp(); // libinit_jp.cpp
void LibInit_exists(); // libinit_exists.cpp

void LibInit()
{
  // call other library init functions
  LibInit_jmg();
  LibInit_cl();
  LibInit_mes();
  LibInit_ac();
  LibInit_gm();
  LibInit_ng(); 
  LibInit_jp(); //absence of wxWidgets will trigger a message for each finction using widgets.
  LibInit_exists();

  const char KLISTEND[] = "";

  const string restoreKey[]={ "FILENAME","DESCRIPTION","VERBOSE", "SKIP_EXISTING", KLISTEND};
  const string restoreWarnKey[]={"NO_COMPILE", "RELAXED_STRUCTURE_ASSIGNMENT", "RESTORED_OBJECTS" , KLISTEND};
  new DLibPro(lib::gdl_restore,string("RESTORE"),1,restoreKey,restoreWarnKey);
//  new DLibPro(lib::gdl_savetest,string("SAVETEST"),1); //,restoreKey,restoreWarnKey);
  
  const string saveKey[]={ "FILENAME","DESCRIPTION","VERBOSE","VARIABLES", "ALL", "COMM", "COMPRESS", "SYSTEM_VARIABLES", "ROUTINES", "IGNORE_NOSAVE"
    ,"XDR" //obsolete
    ,KLISTEND};
  const string saveWarnKey[]={"BUFFER", "EMBEDDED", KLISTEND};
  new DLibPro(lib::gdl_save,string("SAVE"),-1,saveKey,saveWarnKey);


  const string listKey[]={"EXTRACT", "LENGTH", "NO_COPY", KLISTEND};
  new DLibFunRetNew(lib::list_fun,string("LIST"),-1,listKey);

  const string hashKey[]={"EXTRACT", "LOWERCASE", "NO_COPY", "FOLD_CASE", KLISTEND};
  new DLibFunRetNew(lib::hash_fun,string("HASH"),-1,hashKey);
  new DLibFunRetNew(lib::orderedhash_fun,string("ORDEREDHASH"),-1,hashKey);

  new DLibFun(lib::scope_level,string("SCOPE_LEVEL"),0);

  //SCOPE_VARFETCH WARNING: changes in lib::scope_varfetch_value must be reported also in lib::scope_varfetch_reference
  const string scope_varfetchKey[]={"LEVEL","ENTER", "REF_EXTRA", "COMMON", KLISTEND};
  new DLibFun(lib::scope_varfetch_value,string("SCOPE_VARFETCH"),-1,scope_varfetchKey);
 // END OF WARNING 
  
  const string scope_tracebackKey[]={"STRUCTURE","SYSTEM", KLISTEND};
  new DLibFunRetNew(lib::scope_traceback,string("SCOPE_TRACEBACK"),0,scope_tracebackKey);
  const string scope_varnameKey[] = {"COMMON", "COUNT", "LEVEL", KLISTEND};
  new DLibFun (lib::scope_varname_fun, string ("SCOPE_VARNAME"), -1, scope_varnameKey);


  const string cpuKey[]={ "RESET","RESTORE","TPOOL_MAX_ELTS", "TPOOL_MIN_ELTS",
					"TPOOL_NTHREADS","VECTOR_ENABLE",KLISTEND};
  new DLibPro(lib::cpu_pro,string("CPU"),0,cpuKey);
  
// removed, see comments in basic_pro.cpp 
//  const string gdlconfigKey[]={"MAP_QUALITY","GDL_NO_DSFMT","GDL_USE_WX",KLISTEND};
//  new DLibPro(lib::gdl_config_pro,string("GDL_CONFIG"),0,gdlconfigKey);

  const string get_kbrdKey[]={"ESCAPE","KEY_NAME",KLISTEND};
  new DLibFunRetNew(lib::get_kbrd,string("GET_KBRD"),1,NULL,get_kbrdKey);

  const string svdcKey[]={"COLUMN","ITMAX","DOUBLE",KLISTEND};
  new DLibPro(lib::svdc,string("SVDC"),4,svdcKey);

  new DLibFunRetNew(lib::temporary_fun,string("TEMPORARY"),1);
  
  new DLibFunRetNew(lib::terminal_size_fun,string("TERMINAL_SIZE"),2);

  const string routine_infoKey[]={"FUNCTIONS","SYSTEM","DISABLED","ENABLED",
				  "PARAMETERS","SOURCE","UNRESOLVED","VARIABLES", KLISTEND};
  new DLibFunRetNew(lib::routine_info,string("ROUTINE_INFO"),1,routine_infoKey);

  new DLibFunRetNew(lib::routine_name_fun,string("ROUTINE_NAME_INTERNALGDL"),1);
  new DLibFunRetNew(lib::routine_dir_fun,string("ROUTINE_DIR"),1);


  const string spawnKey[]={ "COUNT","EXIT_STATUS","NOSHELL","NULL_STDIN","PID","STDERR","UNIT", //All platforms
  "FORCE", "MACCREATOR", "NOCLISYM", "NOLOGNAME", "NOTIFY", //obsoleted and silently ignored.  
#ifdef _WIN32
	  "HIDE", "LOG_OUTPUT", "NOWAIT",  KLISTEND};

#else
  "NOTTYRESET","SH",  KLISTEND};
#endif
 
  new DLibPro(lib::spawn_pro,string("SPAWN"),3,spawnKey);

  const string bytsclKey[]={"MIN","MAX","TOP","NAN",KLISTEND};
  new DLibFunRetNewTP(lib::bytscl,string("BYTSCL"),3,bytsclKey);  //UsesThreadPOOL 

  const string n_tagsKey[]={"DATA_LENGTH","LENGTH",KLISTEND};
  new DLibFunRetNew(lib::n_tags,string("N_TAGS"),1,n_tagsKey);
  
  const string byteorderKey[]={"SSWAP","LSWAP","L64SWAP",
			       "SWAP_IF_BIG_ENDIAN",
			       "SWAP_IF_LITTLE_ENDIAN",
			       "NTOHL","NTOHS","HTONL","HTONS",
			       "FTOXDR","DTOXDR","XDRTOF","XDRTOD",
             "DTOGFLOAT","GFLOATTOD", // obsoleted 2  keywords only on the VMS platform
			       KLISTEND};
  new DLibPro(lib::byteorder,string("BYTEORDER"),-1,byteorderKey,NULL,0,true);  //UsesThreadPOOL 

  const string obj_classKey[]={"COUNT","SUPERCLASS",KLISTEND};
  new DLibFunRetNew(lib::obj_class,string("OBJ_CLASS"),1,obj_classKey);

  new DLibFunRetNew(lib::obj_isa,string("OBJ_ISA"),2,NULL,NULL,false,2);
  new DLibFunRetNew(lib::obj_hasmethod,string("OBJ_HASMETHOD"),2);

  const string rebinKey[]={"SAMPLE",KLISTEND};
  new DLibFunRetNew(lib::rebin_fun,string("REBIN"),9,rebinKey);

  const string convolKey[]={"CENTER","EDGE_TRUNCATE","EDGE_WRAP","EDGE_ZERO", "EDGE_MIRROR","EDGE_CONSTANT",
			    "BIAS","NORMALIZE","NAN", "INVALID", "MISSING",KLISTEND};
  new DLibFunRetNewTP(lib::convol_fun,string("CONVOL"),3,convolKey);  //UsesThreadPOOL 

  const string smoothKey[]={"NAN", "EDGE_MIRROR", "EDGE_WRAP","EDGE_TRUNCATE", "EDGE_ZERO", "MISSING", KLISTEND};
  new DLibFunRetNew(lib::smooth_fun,string("SMOOTH"),2,smoothKey);

  const string file_searchKey[]={"COUNT","EXPAND_ENVIRONMENT","EXPAND_TILDE",
				 "FOLD_CASE","ISSUE_ACCESS_ERROR",
				 "MARK_DIRECTORY","NOSORT","QUOTE",
				 "MATCH_INITIAL_DOT",
                                 "MATCH_ALL_INITIAL_DOT","FULLY_QUALIFY_PATH",
                                "TEST_DIRECTORY", "TEST_EXECUTABLE", "TEST_READ",
                                 "TEST_REGULAR", "TEST_WRITE", "TEST_ZERO_LENGTH",
                                 "TEST_SYMLINK",
                                 "DIRECTORY","SYMLINK","REGULAR","ZERO_LENGTH",
                                 KLISTEND};
  new DLibFunRetNew(lib::file_search,string("FILE_SEARCH"),2,file_searchKey); //was 3: is NOT IDL, check this special GDL possibilty ? still useful?
  const string findfileKey[]={"COUNT",KLISTEND}; //Obsolete but very much used version of file_search! Use C++ code ABSOLUTELY!!
  new DLibFunRetNew(lib::file_search,string("FINDFILE"),1,findfileKey);
  
  const string file_expand_pathKey[]={KLISTEND};
  new DLibFunRetNew(lib::file_expand_path,string("FILE_EXPAND_PATH"),1,file_expand_pathKey);

  const string file_readlinkKey[]={"ALLOW_NONEXISTENT","ALLOW_NONSYMLINK","NOEXPAND_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_readlink,string("FILE_READLINK"),1,file_readlinkKey);

  const string expand_pathKey[]={"ARRAY","ALL_DIRS","COUNT","DLM","HELP",KLISTEND}; // ,"PATTERN" not in IDL doc
  new DLibFunRetNew(lib::expand_path,string("EXPAND_PATH"),1,expand_pathKey);
  
  const string strjoinKey[]={"SINGLE",KLISTEND};
  new DLibFunRetNew(lib::strjoin,string("STRJOIN"),2,strjoinKey,NULL,true/*retConstant*/);

  const string strcmpKey[]={"FOLD_CASE",KLISTEND};
  new DLibFunRetNew(lib::strcmp_fun,string("STRCMP"),3,strcmpKey,NULL,true);

  new DLibFunRetNew(lib::eof_fun,string("EOF"),1);

  new DLibFunRetNew(lib::arg_present,string("ARG_PRESENT"),1);

  const string messageKey[]={"CONTINUE","INFORMATIONAL","IOERROR","LEVEL",
			     "NONAME","NOPREFIX","NOPRINT",
			     "RESET","REISSUE_LAST","TRACEBACK", KLISTEND}; 
  //TRACEBACK is in MESSAGE but obsolete since 5.0. it is used widely in CMSVlib !
  const string messageWarnKey[]={"NAME", "BLOCK",KLISTEND};
  new DLibPro(lib::message_pro,string("MESSAGE"),-1,messageKey,messageWarnKey);
  
  const string cdKey[]={"CURRENT",KLISTEND};
  new DLibPro(lib::cd_pro,string("CD"),1,cdKey);

  const string file_testKey[]={"DIRECTORY","EXECUTABLE","READ",
			       "REGULAR","WRITE","ZERO_LENGTH",
			       "GET_MODE",
			       "BLOCK_SPECIAL","CHARACTER_SPECIAL",
			       "NAMED_PIPE","SOCKET","SYMLINK","NOEXPAND_PATH","DANGLING_SYMLINK",KLISTEND};
  new DLibFunRetNew(lib::file_test,string("FILE_TEST"),1,file_testKey);

  const string path_sepKey[]={"PARENT_DIRECTORY","SEARCH_PATH",KLISTEND};
  new DLibFunRetNew(lib::path_sep,string("PATH_SEP"),0,path_sepKey);
  
  const string file_basenameKey[]={"FOLD_CASE",KLISTEND};
  new DLibFunRetNew(lib::file_basename,string("FILE_BASENAME"),2,file_basenameKey);

  const string file_dirnameKey[]={"MARK_DIRECTORY",KLISTEND};
  new DLibFunRetNew(lib::file_dirname,string("FILE_DIRNAME"),1,file_dirnameKey);


  const string file_moveKey[]={"ALLOW_SAME", "OVERWRITE", "REQUIRE_DIRECTORY", 
			"VERBOSE", "NOEXPAND_PATH",KLISTEND};
  new DLibPro(lib::file_move,string("FILE_MOVE"),2,file_moveKey);
  
  //exists as stub procedure for _WIN32
#ifndef _WIN32
  const string file_linkKey[]={"ALLOW_SAME", "HARDLINK", 
			"VERBOSE", "NOEXPAND_PATH",KLISTEND};
  new DLibPro(lib::file_link,string("FILE_LINK"),2,file_linkKey);
#endif  
  const string file_copyKey[]={"ALLOW_SAME", "OVERWRITE","FORCE", "REQUIRE_DIRECTORY", 
			"VERBOSE", "NOEXPAND_PATH","RECURSIVE","COPY_SYMLINK",KLISTEND};
  new DLibPro(lib::file_copy,string("FILE_COPY"),2,file_copyKey);

  const string file_deleteKey[]={"ALLOW_NONEXISTENT","NOEXPAND_PATH","RECURSIVE",
			"QUIET","VERBOSE",KLISTEND};
  new DLibPro(lib::file_delete,string("FILE_DELETE"),-1,file_deleteKey);


  const string file_sameKey[]={"NOEXPAND_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_same,string("FILE_SAME"),2,file_sameKey);

  const string file_infoKey[]={"NOEXPAND_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_info,string("FILE_INFO"),2,file_infoKey);

  const string file_linesKey[]={"NOEXPAND_PATH","COMPRESS",KLISTEND};
  new DLibFunRetNew(lib::file_lines,string("FILE_LINES"),1,file_linesKey);

  const string file_mkdirKey[]={"NOEXPAND_PATH",KLISTEND};
  new DLibPro(lib::file_mkdir,string("FILE_MKDIR"),-1,file_mkdirKey);

  new DLibFunRetNew(lib::shift_fun,string("SHIFT"),9,NULL,NULL,true);
  new DLibFunRetNewTP(lib::ishft_fun,string("ISHFT"),2,NULL,NULL,true);  //UsesThreadPOOL 

  const string sortKey[]={"L64",KLISTEND};
  new DLibFunRetNew(lib::sort_fun,string("SORT"),1,sortKey,NULL,true);
  const string gdlsortKey[]={"L64","QUICK","MERGE","RADIX","INSERT","AUTO",KLISTEND}; //,"CHECK"
  new DLibFunRetNew(lib::gdl_sort_fun,string("GDL_SORT"),1,gdlsortKey,NULL,true);

  const string medianKey[]={"EVEN","DOUBLE","DIMENSION",KLISTEND};
  new DLibFunRetNew(lib::median,string("MEDIAN"),2,medianKey);

  const string meanKey[]={"DOUBLE","DIMENSION","NAN",KLISTEND};
  new DLibFunRetNew(lib::mean_fun,string("MEAN"),1,meanKey);
  
  const string momentKey[]={"DOUBLE","DIMENSION","NAN","KURTOSIS","MAXMOMENT","MDEV","MEAN","SDEV","SKEWNESS","VARIANCE",KLISTEND};
  new DLibFunRetNew(lib::moment_fun,string("MOMENT"),1,momentKey);

  new DLibFunRetNew(lib::transpose,string("TRANSPOSE"),2,NULL,NULL,true);

  new DLibPro(lib::retall,string("RETALL"));

  const string catchKey[]={"CANCEL",KLISTEND};
  new DLibPro(lib::catch_pro,string("CATCH"),1,catchKey);
  new DLibPro(lib::on_error,string("ON_ERROR"),1);

  new DLibFunRetNew(lib::recall_commands,string("RECALL_COMMANDS"),0);

  const string exitKey[]={"NO_CONFIRM","STATUS",KLISTEND};
  new DLibPro(lib::exitgdl,string("EXIT"),0,exitKey);
  
  const string helpKey[]={"ALL_KEYS","BRIEF","FULL","CALLS",
	           "DEBUG","DEVICE","FUNCTIONS","HEAP_VARIABLES","HELP","INFO","FILES",
			  "INTERNAL_LIB_GDL","KEYS","LAST_MESSAGE","LIB","MEMORY","NAMES",
		"OBJECTS","OUTPUT","PATH_CACHE","PREFERENCES","PROCEDURES",
			  "RECALL_COMMANDS","ROUTINES","SOURCE_FILES","STRUCTURES",
              "SYSTEM_VARIABLES","TRACEBACK", "COMMON","LEVEL", "SHARED_MEMORY", "BREAKPOINTS","DLM", "MESSAGES", "LAMBDA", KLISTEND};
  new DLibPro(lib::help_pro,string("HELP"),-1,helpKey);

  new DLibPro(lib::delvar_pro,string("DELVAR"),-1,NULL,NULL);
  DLibPro* hide = new DLibPro(lib::findvar_pro,string("FINDVAR"),-1,NULL,NULL);
  hide->SetHideHelp(true);
  
  //stub to avoid setting errors on pref_set. One may want to really write pref_set,
  // but this function is just here to prevent setting !ERR=-1 when stumbling on a pref_set command,
  // since !ERR=-1 is frequently checked by legacy procedures.
  const string pref_setKey[] = {"FILENAME","COMMIT","DEFAULT", KLISTEND };
  new DLibPro(lib::pref_set_pro, string("PREF_SET"), -1, pref_setKey);
  //Same for (related) pref_commit  
  const string pref_commitKey[] = {"RESET","RESIGNAL", KLISTEND };
  new DLibPro(lib::pref_commit_pro, string("PREF_COMMIT"), 1, pref_commitKey);
  
  const string memoryKey[]={"CURRENT","HIGHWATER","NUM_ALLOC",
    "NUM_FREE","STRUCTURE","L64",KLISTEND};
  new DLibFunRetNew(lib::memory_fun, string("MEMORY"), 1, memoryKey, NULL);

  // printKey, readKey and stringKey are closely associated
  // as the same functions are called "FORMAT" till "MONTH"
  // must be the first four keywords. The inner print_os function is BASED on this ORDER!
  //NOTE THAT AM_PM, DAYS_OF_WEEK and MONTHS are silently ignored!!!
  #define COMMONKEYWORDSFORSTRINGFORMATTING "FORMAT","AM_PM","DAYS_OF_WEEK","MONTHS"
  //implied print is a feature introduced in idl8.3 and shared by print/printf, string and fix(type=7) and used in print_os()
  #define COMMONKEYWORDSFORSTRINGFORMATTINGWITHIMPLIEDPRINT COMMONKEYWORDSFORSTRINGFORMATTING,"IMPLIED_PRINT"
  const string printOsKey[]={COMMONKEYWORDSFORSTRINGFORMATTINGWITHIMPLIEDPRINT, "STDIO_NON_FINITE",KLISTEND};
  const string printKey[]={COMMONKEYWORDSFORSTRINGFORMATTINGWITHIMPLIEDPRINT, "STDIO_NON_FINITE","REWRITE"/*obsoleted in 5.3*/,KLISTEND};
  new DLibPro(lib::print,string("PRINT"),-1,printKey);
  new DLibPro(lib::printf,string("PRINTF"),-1,printOsKey);
  // allow printing (of expressions) with all keywords 
  // (easier to implement this way)
  new DLibPro(lib::stop,string("STOP"),-1,printOsKey); 

  const string readKey[]={COMMONKEYWORDSFORSTRINGFORMATTING,"PROMPT"
    ,"KEY_ID","KEY_MATCH","KEY_VALUE" //obsoleted in 5.3
    ,KLISTEND};
  new DLibPro(lib::read_pro,string("READ"),-1,readKey);
  new DLibPro(lib::readf_pro,string("READF"),-1,readKey);

  const string readsKey[]={COMMONKEYWORDSFORSTRINGFORMATTING,	   KLISTEND}; // no PROMPT
  new DLibPro(lib::reads,string("READS"),-1,readsKey);

  const string stringKey[]={COMMONKEYWORDSFORSTRINGFORMATTINGWITHIMPLIEDPRINT,"PRINT",KLISTEND};
  new DLibFun(lib::string_fun,string("STRING"),-1,stringKey);
  //idem but ony with "FORMAT" option, for internal use
  const string gdl_stringKey[]={"FORMAT",KLISTEND};
  new DLibFun(lib::gdl_tostring_fun,string("GDL_TOSTRING"),1,gdl_stringKey);
  
  const string defsysvKey[]={"EXISTS","TEST",KLISTEND};
  new DLibPro(lib::defsysv,string("DEFSYSV"),4,defsysvKey); 

  const string heap_gcKey[]={"PTR","OBJ","VERBOSE",KLISTEND};
  new DLibPro(lib::heap_gc,string("HEAP_GC"),0,heap_gcKey); 
  new DLibPro(lib::heap_free,string("HEAP_FREE"),1,heap_gcKey);

  const string heap_refcount[]={"DISABLE","ENABLE","IS_ENABLED",KLISTEND};
  new DLibFunRetNew(lib::heap_refcount,string("HEAP_REFCOUNT"),1,heap_refcount);


  new DLibPro(lib::ptr_free,string("PTR_FREE"),-1);

  const string arrKey[]={"NOZERO",KLISTEND};
  new DLibFunRetNew(lib::bytarr,string("BYTARR"),MAXRANK,arrKey,NULL,true/*retConstant*/);
  new DLibFunRetNew(lib::intarr,string("INTARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::uintarr,string("UINTARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::lonarr,string("LONARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::ulonarr,string("ULONARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::lon64arr,string("LON64ARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::ulon64arr,string("ULON64ARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::fltarr,string("FLTARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::dblarr,string("DBLARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::complexarr,string("COMPLEXARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::dcomplexarr,string("DCOMPLEXARR"),MAXRANK,arrKey,NULL,true);

  new DLibFunRetNew(lib::strarr,string("STRARR"),MAXRANK,NULL,NULL,true);

  const string ptrArrKey[]={"ALLOCATE_HEAP","NOZERO",KLISTEND};
  new DLibFunRetNew(lib::ptrarr,string("PTRARR"),MAXRANK,ptrArrKey,NULL,true);
  new DLibFunRetNew(lib::objarr,string("OBJARR"),MAXRANK,arrKey,NULL,true);

  const string ptr_newKey[]={"NO_COPY","ALLOCATE_HEAP",KLISTEND};
  new DLibFunRetNew(lib::ptr_new,string("PTR_NEW"),1,ptr_newKey);

  const string obj_validKey[]={"CAST","COUNT","GET_HEAP_IDENTIFIER",KLISTEND};
  new DLibFunRetNew(lib::ptr_valid,string("PTR_VALID"),1,obj_validKey);
  new DLibFunRetNew(lib::obj_valid,string("OBJ_VALID"),1,obj_validKey);
  
  const string obj_newKey[]={"_REF_EXTRA",KLISTEND};
  new DLibFunRetNew(lib::obj_new,string("OBJ_NEW"),-1,obj_newKey);
  new DLibPro(lib::obj_destroy,string("OBJ_DESTROY"),-1,obj_newKey);

  // call... fun/pro
  new DLibFun(lib::call_function,string("CALL_FUNCTION"),-1,obj_newKey);
  new DLibPro(lib::call_procedure,string("CALL_PROCEDURE"),-1,obj_newKey);
  new DLibFun(lib::call_method_function, string("CALL_METHOD"),-1,obj_newKey);
  new DLibPro(lib::call_method_procedure,string("CALL_METHOD"),-1,obj_newKey);
  
  const string indKey[]={"TYPE","BYTE","COMPLEX","DCOMPLEX",
			 "DOUBLE","FLOAT","L64","LONG",
			 "STRING","UINT","UL64","ULONG",
			 "START", "INCREMENT", KLISTEND};
  const string xindKey[]={"START", "INCREMENT", KLISTEND};
  new DLibFunRetNewTP(lib::bindgen,string("BINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::indgen,string("INDGEN"),MAXRANK,indKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::uindgen,string("UINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::sindgen,string("SINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::lindgen,string("LINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::ulindgen,string("ULINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::l64indgen,string("L64INDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::ul64indgen,string("UL64INDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::findgen,string("FINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::dindgen,string("DINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::cindgen,string("CINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::dcindgen,string("DCINDGEN"),MAXRANK,xindKey,NULL,true);  //UsesThreadPOOL 

  new DLibFunRetNew(lib::n_elements,string("N_ELEMENTS"),1,NULL,NULL,true,1);

  new DLibFun(lib::execute_fun,string("EXECUTE"),3);

  const string openKey[]={"APPEND","COMPRESS","BUFSIZE",
			  "DELETE","ERROR","F77_UNFORMATTED",
			  "GET_LUN" /*7*/,"MORE","STDIO",
			  "SWAP_ENDIAN","SWAP_IF_BIG_ENDIAN",
			  "SWAP_IF_LITTLE_ENDIAN" /*12*/,
			  "VAX_FLOAT","WIDTH","XDR", "BLOCK",
			  "NOAUTOMODE","BINARY"
       ,"STREAM"// obsolete VMS only
       ,"DEFAULT","INITIALSIZE","EXTENDSIZE","FIXED","FORTRAN"// obsolete VMS only
       ,"KEYED","LIST","NONE","PRINT","SEGMENTED","SHARED","SUBMIT","SUPERSED"// obsolete VMS only
       ,"TRUNCATE_ON_CLOSE","UDF_BLOCK","VARIABLE"// obsolete VMS only
       ,KLISTEND};
  new DLibPro(lib::openr,string("OPENR"),3,openKey); //3 as a third arg, "record_length", VMS only, was once possible.
  new DLibPro(lib::openw,string("OPENW"),3,openKey);
  new DLibPro(lib::openu,string("OPENU"),3,openKey);
  new DLibPro(lib::get_lun,string("GET_LUN"),1);

  const string socketKey[]={"ERROR","GET_LUN","STDIO",
			    "SWAP_ENDIAN","SWAP_IF_BIG_ENDIAN",
			    "SWAP_IF_LITTLE_ENDIAN","WIDTH",
			    "CONNECT_TIMEOUT","READ_TIMEOUT",
			    "WRITE_TIMEOUT",
			    KLISTEND};
  new DLibPro(lib::socket,string("SOCKET"),3,socketKey);

  new DLibPro(lib::flush_lun,string("FLUSH"),-1);

  const string close_lunKey[]={"FORCE","FILE","ALL",KLISTEND};
  const string close_lunWarnKey[]={"EXIT_STATUS",KLISTEND};
  new DLibPro(lib::close_lun,string("CLOSE"),-1,close_lunKey,close_lunWarnKey);
  
  const string free_lunKey[]={"FORCE",KLISTEND};
  const string free_lunWarnKey[]={"EXIT_STATUS",KLISTEND};
  new DLibPro(lib::free_lun,string("FREE_LUN"),-1,free_lunKey,free_lunWarnKey);

  const string writeuKey[]={"TRANSFER_COUNT","REWRITE"/*obsolete*/,KLISTEND};
  const string readuKey[]={"TRANSFER_COUNT"
    ,"KEY_ID", "KAY_MATCH", "KEY_VALUE" // obsoleted in 5.3
    ,KLISTEND};
  new DLibPro(lib::writeu,string("WRITEU"),-1,writeuKey);
  new DLibPro(lib::readu,string("READU"),-1,readuKey);

  const string resolve_routineWarnKey[]={"SKIP_EXISTING",KLISTEND};
  const string resolve_routineKey[]={"NO_RECOMPILE","IS_FUNCTION","EITHER","COMPILE_FULL_FILE","QUIET",KLISTEND};
  new DLibPro(lib::resolve_routine,string("RESOLVE_ROUTINE"),1,
	      resolve_routineKey,resolve_routineWarnKey);

  const string routine_filepathKey[]={"EITHER","IS_FUNCTION", KLISTEND};
  new DLibFunRetNew(lib::routine_filepath,string("ROUTINE_FILEPATH"),1,
				routine_filepathKey);

  const string assocKey[]={"PACKED",KLISTEND};
  new DLibFunRetNew(lib::assoc,string("ASSOC"),3,assocKey);

  new DLibFun(lib::byte_fun,string("BYTE"),10,NULL,NULL,0,true);  //UsesThreadPOOL 

/*
  new DLibFunRetNew(lib::fix_fun,string("FIX"),10,fixKey,NULL,true);
  new DLibFunRetNew(lib::uint_fun,string("UINT"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::long_fun,string("LONG"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::ulong_fun,string("ULONG"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::long64_fun,string("LONG64"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::ulong64_fun,string("ULONG64"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::float_fun,string("FLOAT"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::double_fun,string("DOUBLE"),10,NULL,NULL,true);
  new DLibFunRetNew(lib::complex_fun,string("COMPLEX"),MAXRANK+2,NULL,NULL,true);
  new DLibFunRetNew(lib::dcomplex_fun,string("DCOMPLEX"),MAXRANK+2,NULL,NULL,true);
*/
// that's apparently the desired bahaviour, see bug no. 3151760
  const string fixKey[]={"TYPE","PRINT","IMPLIED_PRINT",KLISTEND}; //Fix never uses IMPLIED_PRINT, but called print_os does.
  new DLibFun(lib::fix_fun,string("FIX"),10,fixKey,NULL,0,true);  //UsesThreadPOOL 

  new DLibFun(lib::uint_fun,string("UINT"),10,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::long_fun,string("LONG"),10,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::ulong_fun,string("ULONG"),10,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::long64_fun,string("LONG64"),10,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::ulong64_fun,string("ULONG64"),10,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::float_fun,string("FLOAT"),10,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::double_fun,string("DOUBLE"),10,NULL,NULL,0,true);  //UsesThreadPOOL 

  const string complexKey[]={"DOUBLE",KLISTEND};
  new DLibFun(lib::complex_fun,string("COMPLEX"),MAXRANK+2,complexKey,NULL,0,true);  //UsesThreadPOOL 
  new DLibFun(lib::dcomplex_fun,string("DCOMPLEX"),MAXRANK+2,NULL,NULL,0,true);  //UsesThreadPOOL 

  new DLibFunRetNewTP(lib::gdl_logical_and,string("LOGICAL_AND"),2,NULL,NULL,true);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::gdl_logical_or,string("LOGICAL_OR"),2,NULL,NULL,true);  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::logical_true,string("LOGICAL_TRUE"));  //UsesThreadPOOL 

  new DLibFunRetNew(lib::replicate,string("REPLICATE"),9,NULL,NULL,true);
  new DLibPro(lib::replicate_inplace_pro,string("REPLICATE_INPLACE"),6,NULL,NULL,0,true);  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::signum_fun,string("SIGNUM"));  //UsesThreadPOOL 
  
  new DLibFunDirectTP(lib::sin_fun,string("SIN"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::cos_fun,string("COS"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::tan_fun,string("TAN"));  //UsesThreadPOOL 

  new DLibFunDirectTP(lib::sinh_fun,string("SINH"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::cosh_fun,string("COSH"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::tanh_fun,string("TANH"));  //UsesThreadPOOL 

  new DLibFunDirectTP(lib::asin_fun,string("ASIN"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::acos_fun,string("ACOS"));  //UsesThreadPOOL 
  const string atanKey[] = {"PHASE", KLISTEND};
  new DLibFunRetNewTP(lib::atan_fun,string("ATAN"),2,atanKey,NULL,true);  //UsesThreadPOOL 

  new DLibFunDirectTP(lib::alog_fun,string("ALOG"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::alog2_fun,string("ALOG2"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::alog10_fun,string("ALOG10"));  //UsesThreadPOOL 

  new DLibFunDirectTP(lib::sqrt_fun,string("SQRT"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::abs_fun,string("ABS"));  //UsesThreadPOOL 

  new DLibFunDirectTP(lib::exp_fun,string("EXP"));  //UsesThreadPOOL 

  const string roundKey[]={"L64",KLISTEND};
  // retConstant: check definition of the rounding functions if they depend 
  // from some sys var (defining a round mode) 
  // (probably nobody rounds a constant anyway)
  const string roundceilfloorKey[]={"L64",KLISTEND};
  new DLibFunRetNewTP(lib::round_fun,string("ROUND"),1,roundceilfloorKey);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::ceil_fun,string("CEIL"),1,roundceilfloorKey);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::floor_fun,string("FLOOR"),1,roundceilfloorKey);  //UsesThreadPOOL 

  new DLibFunDirectTP(lib::conj_fun,string("CONJ"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::imaginary_fun,string("IMAGINARY"));  //UsesThreadPOOL 
  new DLibFunDirectTP(lib::real_part_fun,string("REAL_PART"));

  const string strcompressKey[]={"REMOVE_ALL",KLISTEND};
  new DLibFunRetNew(lib::strcompress,string("STRCOMPRESS"),1,strcompressKey,NULL,true);
  
  new DLibFunDirectTP(lib::strlowcase,string("STRLOWCASE"));
  new DLibFunDirectTP(lib::strupcase,string("STRUPCASE"));
  new DLibFunDirectTP(lib::strlen,string("STRLEN"));

  const string strmidKey[]={"REVERSE_OFFSET",KLISTEND};
  new DLibFunRetNew(lib::strmid,string("STRMID"),3,strmidKey,NULL,true);
  new DLibFunRetNew(lib::strtrim,string("STRTRIM"),2,NULL,NULL,true);
  const string strposKey[]={"REVERSE_OFFSET","REVERSE_SEARCH",KLISTEND};
  new DLibFunRetNew(lib::strpos,string("STRPOS"),3,strposKey,NULL,true,2);
  new DLibPro(lib::strput,string("STRPUT"),3,NULL,NULL,2);
  
  const string whereKey[]={"COMPLEMENT","NCOMPLEMENT","NULL","L64",KLISTEND};
  new DLibFunRetNewTP(lib::where_fun,string("WHERE"),2,whereKey);  //UsesThreadPOOL 

  const string totalKey[]={"CUMULATIVE","DOUBLE","NAN","INTEGER","PRESERVE_TYPE",KLISTEND};
  new DLibFunRetNewTP(lib::total_fun,string("TOTAL"),2,totalKey,NULL,true);  //UsesThreadPOOL 

  const string productKey[]={"CUMULATIVE","NAN","INTEGER","PRESERVE_TYPE",KLISTEND};
  new DLibFunRetNewTP(lib::product_fun,string("PRODUCT"),2,productKey,NULL,true);  //UsesThreadPOOL 

  new DLibFunRetNew(lib::n_params,string("N_PARAMS"),1); // IDL allows one parameter
  new DLibFunRetNew(lib::keyword_set,string("KEYWORD_SET"),1);

  const string array_equalKey[]={"NO_TYPECONV","NOT_EQUAL","QUIET",KLISTEND};
  new DLibFunRetNew(lib::array_equal,string("ARRAY_EQUAL"),2,array_equalKey,NULL,true);
  
  const string minKey[]={"MAX","NAN","SUBSCRIPT_MAX","DIMENSION","ABSOLUTE",KLISTEND};
  new DLibFunRetNewTP(lib::min_fun,string("MIN"),2,minKey,NULL,true);  //UsesThreadPOOL 
  const string maxKey[]={"MIN","NAN","SUBSCRIPT_MIN","DIMENSION","ABSOLUTE",KLISTEND};
  new DLibFunRetNewTP(lib::max_fun,string("MAX"),2,maxKey,NULL,true);  //UsesThreadPOOL 

  // retConstant: structs are tricky: struct resolution depends from !PATH
  // and this might change during runtime, but if treated as retConstant
  // the struct would be already defined at compile time.
  const string create_structKey[]={"NAME",KLISTEND};
  new DLibFunRetNew(lib::create_struct,string("CREATE_STRUCT"),-1,create_structKey/*,true*/);

  new DLibFunRetNew(lib::rotate,string("ROTATE"),2,NULL,NULL,true);

  const string reverseKey[] = {"OVERWRITE", KLISTEND};
  new DLibFun(lib::reverse, string("REVERSE"), 2, reverseKey, NULL, true);


#ifdef USE_PYTHON
  const string python_funKey[]={"ARGV","DEFAULTRETURNVALUE",KLISTEND};
  new DLibFun(lib::gdlpython_fun,string("PYTHON"),-1,python_funKey);

  const string python_proKey[]={"ARGV",KLISTEND};
  new DLibPro(lib::gdlpython_pro,string("PYTHON"),-1,python_proKey);
#endif

  // graphics *******************************************************
  const string windowKey[]={"COLORS","FREE","PIXMAP","RETAIN","TITLE",
			    "XPOS","YPOS","XSIZE","YSIZE",KLISTEND};
  new DLibPro(lib::window,string("WINDOW"),1,windowKey);
  new DLibPro(lib::wdelete,string("WDELETE"),-1);
  new DLibPro(lib::wset,string("WSET"),1);

  const string wshowKey[]={"ICONIC", KLISTEND};
  new DLibPro(lib::wshow,string("WSHOW"),2,wshowKey);

  const string cursorKey[]={"CHANGE","DOWN","NOWAIT","UP","WAIT",
				"DATA","DEVICE","NORMAL",KLISTEND};
  new DLibPro(lib::cursor,string("CURSOR"),3,cursorKey);

  const string set_plotKey[]={"COPY","INTERPOLATE",KLISTEND};
  new DLibPro(lib::set_plot,string("SET_PLOT"),1,set_plotKey);

  const string get_screen_sizeKey[]={"RESOLUTION","DISPLAY_NAME",KLISTEND};
  new DLibFunRetNew(lib::get_screen_size,string("GET_SCREEN_SIZE"),1,get_screen_sizeKey);

  const string tvlctKey[]={"GET","HLS","HSV",KLISTEND};
  new DLibPro(lib::tvlct,string("TVLCT"),4,tvlctKey);

  const string tvcrsKey[]={"DATA","DEVICE","NORMAL",KLISTEND};
  const string tvcrsWarnKey[]={"CENTIMETERS","INCHES","HIDE_CURSOR","T3D","Z",KLISTEND};
  new DLibPro(lib::tvcrs,string("TVCRS"),2,tvcrsKey,tvcrsWarnKey);

  new DLibPro(lib::empty,string("EMPTY"),-1);
  
  const string deviceKey[]=
    {
      "CLOSE_FILE", "FILENAME", "LANDSCAPE", "PORTRAIT",  //there is a "CLOSE" for device Z defined... and a CLOSE_DOCUMENT for printer 
      "DECOMPOSED","GET_DECOMPOSED","Z_BUFFERING","SET_RESOLUTION",
      "XSIZE","YSIZE",
      "COLOR","GET_PAGE_SIZE","GET_SCREEN_SIZE","INCHES","WINDOW_STATE","SCALE_FACTOR", 
      "XOFFSET", "YOFFSET", "ENCAPSULATED", "GET_GRAPHICS_FUNCTION", 
      "SET_GRAPHICS_FUNCTION", "CURSOR_STANDARD", "CURSOR_ORIGINAL",
      "CURSOR_CROSSHAIR",
      "CURSOR_IMAGE", //WIN,X
      "CURSOR_MASK",  //WIN,X
      "CURSOR_XY",  //WIN,X
      "RETAIN", "BITS_PER_PIXEL", 
      "GET_WINDOW_POSITION","GET_PIXEL_DEPTH","GET_VISUAL_DEPTH","GET_VISUAL_NAME",
      "GET_WRITE_MASK", "COPY","GET_FONTNAMES","SET_FONT","FONT","GET_CURRENT_FONT","GET_FONTNUM", //"FONT" is idem SET_FONT before IDL5.3
      "SET_PIXEL_DEPTH", //Z
      "SET_CHARACTER_SIZE", //(all)
      KLISTEND
    };
  const string deviceWarnKey[] = {
    "AVANTGARDE","BKMAN","COURIER","HELVETICA","ISOLATIN","PALATINO","SCHOOLBOOK","SYMBOL","TIMES","ZAPFCHANCERY","ZAPFDINGBATS", //PS
  //"AVERAGE_LINES", (REGIS)
  //"BINARY","NCAR","TEXT", (CGM)
  "BOLD", //PS
  "BOOK", //PS
  "BYPASS_TRANSLATION", //(WIN,X)
  "CMYK", //PS
  "DIRECT_COLOR", //X
  "DEPTH", //VMS OBSOLETE
  //"EJECT", (HP)
  //"ENCODING", (CGM)
  "FLOYD", //(PCL,X)
  "FONT_INDEX", //PS
  "FONT_SIZE", //PS
  //"GIN_CHARS", (TEX)
  "GLYPH_CACHE", //PRINTER,PS,WIN,Z,METAFILE
  //"INDEX_COLOR", (METAFILE, PRINTER)
  "ITALIC", //PS
  "LANGUAGE_LEVEL" //PS,
  "DEMI","LIGHT","MEDIUM","NARROW","OBLIQUE", //PS
  //"OPTIMIZE", (PCL)
  "ORDERED", //(PCL,X)
  "OUTPUT", //(HP,PS)
  //"PIXELS", (PCL)
  //"PLOT_TO", (REGIS,TEK)
  //"PLOTTER_ON_OFF", "POLYFILL", (HP)
  "PRE_DEPTH","PRE_XSIZE","PRE_YSIZE","PREVIEW", //PS
  "PRINT_FILE",// WIN
  "PSEUDO_COLOR",//X
  //"RESET_STRING", (TEK)
  //"RESOLUTION", (PCL)
  //"SET_COLORMAP",(PCL)
  "SET_COLORS", //Z
  //"SET_STRING", (TEK)
  "SET_TRANSLATION", //X
  "SET_WRITE_MASK", //(X,Z)
  "STATIC_COLOR",//X
  "STATIC_GRAY",//X
  //"TEK4014","TEK4100", (TEK)
  "THRESHOLD", //X
  "TRANSLATION", //(WIN,X)
  "TRUE_COLOR", //(METAFILE, PRINTER, X)
  "TT_FONT", //(METAFILE, PRINTER, X, PS, WIN, Z)
  //"TTY", (REGIS, TEK)
  //"VT240","VT241","VT340","VT341",(REGIS)
  //"XON_XOFF" (HP)
   KLISTEND};
  new DLibPro(lib::device,string("DEVICE"),0, deviceKey, deviceWarnKey);

  const string usersymKey[]= 
  {
    "FILL", "COLOR","THICK", KLISTEND
   };
  new DLibPro(lib::usersym,string("USERSYM"),2, usersymKey);

  const string plotKey[]=
    {
     "BACKGROUND","CHARSIZE","CHARTHICK","CLIP",
     "COLOR",     "DATA",    "DEVICE", "ISOTROPIC",
     "LINESTYLE", "NOCLIP",  "NODATA",   "NOERASE", 
     "NORMAL",    "POSITION","PSYM",     "SUBTITLE",
     "SYMSIZE",   "THICK",    "TICKLEN", "TITLE",
     "MAX_VALUE", "MIN_VALUE",
     "XLOG",      "YLOG", "ZLOG", "YNOZERO",   "XTYPE",    "YTYPE",  //XTYPE and YTYPE are oldies, equivalent to XLOG when value is odd. ZTYPE NOT ALLOWED
        "POLAR", "NSUM", "XCHARSIZE", "YCHARSIZE",
      "XGRIDSTYLE", "YGRIDSTYLE",
      "XMARGIN", "YMARGIN",
      "XMINOR", "YMINOR",
      "XRANGE", "YRANGE",
      "XSTYLE", "YSTYLE",
      "XTHICK", "YTHICK",
      "XTICK_GET", "YTICK_GET",
      "XTICKFORMAT", "YTICKFORMAT",
      "XTICKINTERVAL", "YTICKINTERVAL",
      "XTICKLAYOUT", "YTICKLAYOUT",
      "XTICKLEN", "YTICKLEN",
      "XTICKNAME", "YTICKNAME",
      "XTICKS", "YTICKS",
      "XTICKUNITS", "YTICKUNITS",
      "XTICKV", "YTICKV",
      "XTITLE", "YTITLE",
      "ZCHARSIZE", "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
      "ZRANGE", "ZSTYLE", "ZTHICK", "ZTICK_GET", "ZTICKFORMAT", "ZTICKINTERVAL",
      "ZTICKLAYOUT", "ZTICKLEN", "ZTICKNAME", "ZTICKS", "ZTICKUNITS", "ZTICKV",
      "ZTITLE", "T3D", "ZVALUE", "FONT", "CHANNEL", KLISTEND
    };
  //
//  const string plotWarnKey[]= { "FONT","CHANNEL", KLISTEND };
  new DLibPro(lib::plot,string("PLOT"),2,plotKey);//,plotWarnKey);
  new DLibPro(lib::plot_io,string("PLOT_IO"),2,plotKey);//,plotWarnKey);
  new DLibPro(lib::plot_oo,string("PLOT_OO"),2,plotKey);//,plotWarnKey);
  new DLibPro(lib::plot_oi,string("PLOT_OI"),2,plotKey);//,plotWarnKey);

  const string axisKey[]=
  {
    "CHARSIZE",  "CHARTHICK",
    "COLOR",     "DATA",    "DEVICE",   "FONT",
    "NODATA",    "NOERASE", 
    "NORMAL",    "SUBTITLE",
    "T3D",       "TICKLEN", 
    "SAVE", "XAXIS", "YAXIS", "XLOG", "YLOG", "ZLOG", "XTYPE", "YTYPE", "ZTYPE", //XTYPE and YTYPE are oldies, equivalent to XLOG when value is odd.
    "YNOZERO", "THICK",
    "XCHARSIZE", "YCHARSIZE",
    "XGRIDSTYLE", "YGRIDSTYLE",
    "XMARGIN", "YMARGIN",
    "XMINOR", "YMINOR",
    "XRANGE", "YRANGE",
    "XSTYLE", "YSTYLE",
    "XTHICK", "YTHICK",
    "XTICK_GET", "YTICK_GET",
    "XTICKFORMAT", "YTICKFORMAT",
    "XTICKINTERVAL", "YTICKINTERVAL",
    "XTICKLAYOUT", "YTICKLAYOUT",
    "XTICKLEN", "YTICKLEN",
    "XTICKNAME", "YTICKNAME",
    "XTICKS", "YTICKS",
    "XTICKUNITS", "YTICKUNITS",
    "XTICKV", "YTICKV",
    "XTITLE", "YTITLE",
    "ZAXIS",  "ZVALUE", "ZCHARSIZE", "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
    "ZRANGE", "ZSTYLE", "ZTHICK", "ZTICK_GET", "ZTICKFORMAT", "ZTICKINTERVAL",
    "ZTICKLAYOUT", "ZTICKLEN", "ZTICKNAME", "ZTICKS", "ZTICKUNITS", "ZTICKV",
    "ZTITLE", "CHANNEL",
    KLISTEND
  };
  new DLibPro(lib::axis,string("AXIS"),3,axisKey);

  const string oplotKey[]=
    {
      // GRAPHIC KEYWORDS
      // 0
      "CLIP","COLOR", "LINESTYLE", "NOCLIP",
      // 4
      "PSYM", "SYMSIZE",  "T3D",  "ZVALUE", "THICK",
      // 8
      "MAX_VALUE", "MIN_VALUE", "NSUM", "POLAR",
      "CHANNEL", KLISTEND
    };
  new DLibPro(lib::oplot, string("OPLOT"),2,oplotKey);

  const string plotsKey[]=
    {
      "CLIP","COLOR", "LINESTYLE", "NOCLIP",
      "PSYM", "SYMSIZE",  "T3D",  "Z", "THICK",
      "DATA","DEVICE","NORMAL","CONTINUE",
      KLISTEND
    };

  new DLibPro(lib::plots, string("PLOTS"), 3, plotsKey);

  const string set_shadingKey[]=
  {
    "LIGHT", KLISTEND
  };
  
  const string set_shadingWarnKey[]=
  {
     "GOURAUD","REJECT","VALUES",KLISTEND
  };
  new DLibPro(lib::set_shading, string("SET_SHADING"), 0, set_shadingKey, set_shadingWarnKey);

  const string shade_surfKey[]=
    {
      "AX", "AZ",  "MAX_VALUE", "MIN_VALUE", 
      "HORIZONTAL", "LOWER_ONLY", "UPPER_ONLY", "BOTTOM", 
      "XLOG", "YLOG", "ZLOG", "XTYPE", "YTYPE", "ZTYPE",  //XTYPE and YTYPE are oldies, equivalent to XLOG when value is odd.
      //General Graphics KW
      "BACKGROUND", "NOERASE", "CLIP", "NOCLIP",
      "CHARSIZE", "CHARTHICK", "COLOR", "DATA", "DEVICE", "NORMAL", "FONT",
      "NODATA", "POSITION", "SUBTITLE", "THICK", "TICKLEN", "TITLE",
      //Axis KW
      "ZCHARSIZE", "YCHARSIZE", "XCHARSIZE",
      "ZGRIDSTYLE", "YGRIDSTYLE", "XGRIDSTYLE",
      "ZMARGIN", "YMARGIN", "XMARGIN",
      "ZMINOR", "YMINOR", "XMINOR",
      "ZRANGE", "YRANGE", "XRANGE",
      "ZSTYLE", "YSTYLE", "XSTYLE",
      "ZTHICK", "YTHICK", "XTHICK",
      "ZTICKFORMAT", "YTICKFORMAT", "XTICKFORMAT",
      "ZTICKINTERVAL", "YTICKINTERVAL", "XTICKINTERVAL",
      "ZTICKLAYOUT", "YTICKLAYOUT", "XTICKLAYOUT",
      "ZTICKLEN", "YTICKLEN", "XTICKLEN",
      "ZTICKNAME", "YTICKNAME", "XTICKNAME",
      "ZTICKS", "YTICKS", "XTICKS",
      "ZTICKUNITS", "YTICKUNITS", "XTICKUNITS",
      "ZTICKV", "YTICKV", "XTICKV",
      "ZTICK_GET", "YTICK_GET", "XTICK_GET",
      "ZTITLE", "YTITLE", "XTITLE",
      //3D KW
      "ZVALUE","T3D", "SAVE", "SKIRT", "ZAXIS", "CHANNEL",  "SHADES", KLISTEND
    };
  const string shade_surfWarnKey[]=
  {
      "IMAGE", "PIXELS", KLISTEND 
  };
  new DLibPro(lib::shade_surf,string("SHADE_SURF"),3,shade_surfKey, shade_surfWarnKey);

  const string surfaceKey[]=
    {
     // GRAPHIC KEYWORDS
     "BACKGROUND","CHARSIZE","CHARTHICK","CLIP",
     "COLOR",     "DATA",    "DEVICE",   "FONT",
     "LINESTYLE", "NOCLIP",  "NODATA",   "NOERASE", 
     "NORMAL",    "POSITION",     "SUBTITLE",
     "THICK",    "TICKLEN", "TITLE",
     "XCHARSIZE",    "XGRIDSTYLE", "XMARGIN", "XMINOR",
     "XRANGE",       "XSTYLE",     "XTHICK",  "XTICKFORMAT",
     "XTICKINTERVAL","XTICKLAYOUT","XTICKLEN","XTICKNAME",
     "XTICKS",       "XTICKUNITS", "XTICKV",  "XTICK_GET", "XTITLE",
     "YCHARSIZE",    "YGRIDSTYLE", "YMARGIN", "YMINOR",
     "YRANGE",       "YSTYLE",     "YTHICK",  "YTICKFORMAT",
     "YTICKINTERVAL","YTICKLAYOUT","YTICKLEN","YTICKNAME",
     "YTICKS",       "YTICKUNITS", "YTICKV",  "YTICK_GET", "YTITLE",
     "ZCHARSIZE",    "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
     "ZRANGE",       "ZSTYLE",     "ZTHICK",  "ZTICKFORMAT",
     "ZTICKINTERVAL","ZTICKLAYOUT","ZTICKLEN","ZTICKNAME",
     "ZTICKS",       "ZTICKUNITS", "ZTICKV",  "ZTICK_GET", "ZTITLE",
     // SURFACE keywords
     "MAX_VALUE",  "MIN_VALUE", "AX", "AZ", 
     "XLOG", "YLOG", "ZLOG", "XTYPE", "YTYPE", "ZTYPE",      // ([xyz]type undocumented but still existing in SURFACE ...)
     "HORIZONTAL", "LOWER_ONLY", "UPPER_ONLY",  "ZAXIS",  "BOTTOM", 
     "SKIRT", "SAVE", "T3D",  "ZVALUE", "CHANNEL", "SHADES", KLISTEND
    };
  const string surfaceWarnKey[]=
  {
      "LEGO", KLISTEND 
  };
  new DLibPro(lib::surface,string("SURFACE"),3,surfaceKey, surfaceWarnKey);

  const string contourKey[]=
    {
      "BACKGROUND","CHARSIZE","CHARTHICK","CLIP",
      "COLOR",     "DATA",    "DEVICE",
      "NOCLIP",  "NODATA",   "NOERASE",
      "NORMAL",    "POSITION", "SUBTITLE",
      "T3D",     "THICK",    "TICKLEN",
      "TITLE" ,    "LEVELS", "NLEVELS",
      "MAX_VALUE", "MIN_VALUE",
      "XLOG", "YLOG", "ZLOG", "XTYPE", "YTYPE", // ([xy]type undocumented but still existing in CONTOUR ... ZTYPE NOT ALLOWED)
      "FILL", "ISOTROPIC", "FOLLOW",
      "XCHARSIZE", "YCHARSIZE", "ZCHARSIZE",
      "XGRIDSTYLE", "YGRIDSTYLE", "ZGRIDSTYLE",
      "XMARGIN", "YMARGIN", "ZMARGIN",
      "XMINOR", "YMINOR", "ZMINOR",
      "XRANGE", "YRANGE", "ZRANGE",
      "XSTYLE", "YSTYLE", "ZSTYLE",
      "XTHICK", "YTHICK", "ZTHICK",
      "XTICK_GET", "YTICK_GET", "ZTICK_GET",
      "XTICKFORMAT", "YTICKFORMAT", "ZTICKFORMAT",
      "XTICKINTERVAL", "YTICKINTERVAL", "ZTICKINTERVAL",
      "XTICKLAYOUT", "YTICKLAYOUT", "ZTICKLAYOUT",
      "XTICKLEN", "YTICKLEN", "ZTICKLEN",
      "XTICKNAME", "YTICKNAME", "ZTICKNAME",
      "XTICKS", "YTICKS",  "ZTICKS",
      "XTICKUNITS", "YTICKUNITS",  "ZTICKUNITS",
      "XTICKV", "YTICKV","ZTICKV",
      "XTITLE", "YTITLE", "ZTITLE",
      "ZVALUE", "ZAXIS",
      "C_CHARSIZE","OVERPLOT","C_COLORS","C_LINESTYLE", "C_ANNOTATIONS",
      "C_LABELS", "C_CHARTHICK", "C_ORIENTATION", "C_SPACING", "C_THICK",
      "PATH_INFO","PATH_XY","CELL_FILL",
      "IRREGULAR", "CHANNEL", "FONT", // FONT is not permitted ???
      KLISTEND
    };
   // NO SUPPORT AT ALL for:,"CLOSED","DOWNHILL","IRREGULAR","PATH_DATA_COORDS","PATH_FILENAME",
   // "PATH_INFO","PATH_XY","TRIANGULATION","PATH_DOUBLE","ZAXIS"
  // "CHANNEL" is supposed to be passed from CONTOUR, PLOT, OPLOT, SHADE_SURF etc to ERASE
   const string contourWarnKey[]=
    {
      "TRIANGULATION", "CLOSED","DOWNHILL",
      "PATH_DATA_COORDS","PATH_FILENAME","PATH_INFO","PATH_XY","PATH_DOUBLE",KLISTEND
    };
   new DLibPro(lib::contour,string("CONTOUR"),3,contourKey,contourWarnKey);

  // note that although  SIZE is not in IDL6+ documentation, it is widely used
  // in place of CHARSIZE....
  const string xyoutsKey[]=
    {
      "COLOR","DATA","NORMAL","DEVICE","CLIP",
      "ORIENTATION","ALIGNMENT","CHARSIZE","SIZE","CHARTHICK", //note SIZE is not in doc but in XYOUTS demo!
      "NOCLIP","T3D","Z","TEXT_AXES","WIDTH","FONT",KLISTEND
    };
   new DLibPro(lib::xyouts, string("XYOUTS"), 3, xyoutsKey);//, xyoutsWarnKey);
   
   const string polyfillKey[]=
    {
      "CLIP", "COLOR","DATA","DEVICE","NORMAL","LINESTYLE","NOCLIP", //supported graphic KW
      "ORIENTATION","THICK", //supported graphic KW
      "LINE_FILL", "SPACING", "T3D","Z", //polyfill specifics
      KLISTEND
    };
   const string polyfillWarnKey[]=
    {
     "IMAGE_COORD","IMAGE_INTERP", "PATTERN", "FILL_PATTERN", "TRANSPARENT",KLISTEND
    };
  new DLibPro(lib::polyfill, string("POLYFILL"), 3, polyfillKey,polyfillWarnKey);

  const string scale3Key[]=
  {
    "AX", "AZ", "XRANGE", "YRANGE", "ZRANGE", KLISTEND
  };
  new DLibPro(lib::scale3_pro, string("SCALE3"), 0, scale3Key); //in plotting_convert_coord.cpp

  const string t3dKey[]=
  {
     "RESET", "MATRIX", "OBLIQUE", "PERSPECTIVE", "ROTATE", "SCALE", "TRANSLATE",
     "XYEXCH", "XZEXCH", "YZEXCH", KLISTEND
  };
  new DLibPro(lib::t3d_pro, string("T3D"), 1, t3dKey); //in plotting_convert_coord.cpp

  const string eraseKey[]= {"CHANNEL","COLOR",KLISTEND};
  new DLibPro(lib::erase,string("ERASE"),1, eraseKey);
  
  new DLibFunRetNew(lib::format_axis_values,string("FORMAT_AXIS_VALUES"),1);
  
  const string laguerreKey[]={"DOUBLE","COEFFICIENTS",KLISTEND};
  new DLibFunRetNew(lib::laguerre,string("LAGUERRE"),3,laguerreKey);

  new DLibFunRetNew(lib::gauss_pdf,string("GAUSS_PDF"),1);
  new DLibFunRetNew(lib::gauss_cvf,string("GAUSS_CVF"),1);

  new DLibFunRetNew(lib::t_pdf,string("T_PDF"),2);

  const string caldatKey[]={"GDL_DOW","GDL_ICAP",KLISTEND};
  new DLibPro(lib::caldat, string("CALDAT"), 7, caldatKey);
  new DLibFunRetNew(lib::julday, string("JULDAY"), 6);

  // SA: the HYBRID key is used in imsl_zerosys.pro to switch to the modif. brent algo. 
  const string newtonKey[] = {"DOUBLE", "ITMAX", "TOLF", "TOLX", "HYBRID", KLISTEND };
  const string newtonWarnKey[] = {"CHECK", "STEPMAX", "TOLMIN", KLISTEND};
  new DLibFunRetNew(lib::newton_broyden, string("NEWTON"), 2, newtonKey, newtonWarnKey);
  //WARNING: THESE FUNCTIONS MUST HAVE THE SAME OPTION LIST. NOTE THAT /HYBRID is not part of either functions,
  //it is used by imsl_zerosys.pro
  const string broydenKey[] = {"DOUBLE", "ITMAX", "TOLF", "TOLX", "HYBRID", KLISTEND };
  const string broydenWarnKey[] = {"CHECK", "STEPMAX", "TOLMIN",  "EPS",  KLISTEND};
  new DLibFunRetNew(lib::newton_broyden, string("BROYDEN"), 2, broydenKey, broydenWarnKey);


  const string amoebaKey[] = {"FUNCTION_NAME", "FUNCTION_VALUE", "NCALLS", "NMAX", "P0", "SCALE" , "SIMPLEX", KLISTEND };
  new DLibFunRetNew(lib::amoeba, string("AMOEBA"), 1, amoebaKey);
  const string dfpminKey[] = { "DOUBLE", "EPS", "ITER", "ITMAX", "STEPMAX", "TOLX", KLISTEND };
  new DLibPro(lib::dfpmin, string("DFPMIN"), 5, dfpminKey);

  // now used as and example for DLM - found in src/dlm
  //  const string brentKey[] = { "DOUBLE", "ITER", "ITMAX", KLISTEND };
//  new DLibPro(lib::brent, string("POWELL"), 5, brentKey);

  //#ifdef USE_GLPK
  const string simplexKey[] = { "DOUBLE", "EPS", "STATUS", KLISTEND };
  new DLibFunRetNew(lib::simplex, string("SIMPLEX"), 8, simplexKey);
  //#endif

  new DLibFunRetNew(lib::parse_url, string("PARSE_URL"), 1);
  new DLibFunRetNew(lib::locale_get, string("LOCALE_GET"), 0);
  new DLibFunRetNew(lib::get_login_info, string("GET_LOGIN_INFO"), 0);
  new DLibFunRetNew(lib::idl_base64, string("IDL_BASE64"), 1);

  const string ll_arc_distanceKey[] = {"DEGREES", KLISTEND };
  new DLibFunRetNew(lib::ll_arc_distance, string("LL_ARC_DISTANCE"), 3, ll_arc_distanceKey);
  const string command_line_argsKey[] = {"COUNT","RESET","SET", KLISTEND }; //note: reset and set are unsupported extensions.
// original  const string command_line_argsKey[] = {"COUNT", KLISTEND };
new DLibFunRetNew(lib::command_line_args_fun, string("COMMAND_LINE_ARGS"), 0, command_line_argsKey);

const string pmKey[] = {"FORMAT", "TITLE", KLISTEND };
new DLibPro(lib::pm_pro, string("PM"), -1, pmKey);

  const string constantKey[] = {"DOUBLE", KLISTEND };
  new DLibFunRetNew(lib::constant, string("IMSL_CONSTANT"), 2, constantKey);

  const string get_drive_listKey[] = {"COUNT", KLISTEND };
  new DLibFunRetNew(lib::get_drive_list, string("GET_DRIVE_LIST"), 0, get_drive_listKey);

  const string binomialcoefKey[] = {"DOUBLE", KLISTEND };
  new DLibFunRetNew(lib::binomialcoef, string("IMSL_BINOMIALCOEF"), 2, binomialcoefKey);

  // SA: GRIB format support using ECMWF ECCODES package (developed prior to IDL GRIB support)
  // -----------------------------------------------------------------------------------------
  // GRIB: file related
  new DLibFunRetNew(lib::grib_open_file_fun, string("GRIBAPI_OPEN_FILE"), 1); 
  new DLibFunRetNew(lib::grib_count_in_file_fun, string("GRIBAPI_COUNT_IN_FILE"), 1); 
  new DLibPro(lib::grib_close_file_pro, string("GRIBAPI_CLOSE_FILE"), 1); 
  // GRIB: message related
  new DLibFunRetNew(lib::grib_new_from_file_fun, string("GRIBAPI_NEW_FROM_FILE"), 1); 
  new DLibPro(lib::grib_release_pro, string("GRIBAPI_RELEASE"), 1); 
  // see comment in grib.cpp
  //new DLibFunRetNew(lib::grib_get_message_size_fun, string("GRIBAPI_GET_MESSAGE_SIZE"), 1);
  new DLibFunRetNew(lib::grib_clone_fun, string("GRIBAPI_CLONE"), 1); 
  // GRIB: data related
  new DLibFunRetNew(lib::grib_get_size_fun, string("GRIBAPI_GET_SIZE"), 2); 
  new DLibPro(lib::grib_get_pro, string("GRIBAPI_GET"), 3); 
  new DLibPro(lib::grib_get_data_pro, string("GRIBAPI_GET_DATA"), 4); 
  // -----------------------------------------------------------------------------------------

  new DLibFunRetNew(lib::crossp, string("CROSSP"), 2);
  //const string hanningKey[] = {"DOUBLE", "ALPHA", KLISTEND };
  //new DLibFunRetNew(lib::hanning, string("HANNING"), 2, hanningKey);
  const string wtnKey[] = {"COLUMN", "DOUBLE", "INVERSE", "OVERWRITE", KLISTEND };
  new DLibFun(lib::wtn, string("WTN"), 2, wtnKey);
  const string zeropolyKey[] = {"DOUBLE", "COMPANION", "JENKINS_TRAUB", KLISTEND };
  new DLibFunRetNew(lib::zeropoly, string("IMSL_ZEROPOLY"), 1, zeropolyKey);
  const string spher_harmKey[] = {"DOUBLE", KLISTEND };
  new DLibFunRetNew(lib::spher_harm, string("SPHER_HARM"), 4, spher_harmKey);
  const string gdl_erfinvKey[] = {"DOUBLE", KLISTEND };
  new DLibFunRetNew(lib::gdl_erfinv_fun, string("GDL_ERFINV"), 1, gdl_erfinvKey);

  // by Mateusz Turcza
  const string sem_createKey[] = {"DESTROY_SEMAPHORE", KLISTEND };
  new DLibFunRetNew(lib::sem_create, string("SEM_CREATE"), 1, sem_createKey);
  new DLibPro(lib::sem_delete, string("SEM_DELETE"), 1);
  new DLibFunRetNew(lib::sem_lock, string("SEM_LOCK"), 1);
  new DLibPro(lib::sem_release, string("SEM_RELEASE"), 1);
  // -----------------------------------------------------------------------------------------
  const string labelregionKey[] = {"ALL_NEIGHBORS", "ULONG", KLISTEND };
  new DLibFunRetNew(lib::labelregion, string("LABEL_REGION"),1,labelregionKey);

  
  const string valuelocateKey[] = {"L64",KLISTEND };
  new DLibFunRetNew(lib::value_locate, string("VALUE_LOCATE"),2,valuelocateKey);

  const string gaussfitKey[] = {"CHISQ","ESTIMATES", "MEASURE_ERRORS", "NTERMS", "SIGMA", "YERROR",KLISTEND};
  new DLibFunRetNew(lib::gaussfit, string("GAUSSFIT"), 3, gaussfitKey);
  #ifndef _WIN32
  //SHM --------------------------------------------------------------------------------------
  // Note: counted values, do not change order:
  const string shmmapKey[] = {
	"BYTE","COMPLEX","DCOMPLEX","DOUBLE","FLOAT","INTEGER","L64","LONG","UINT","UL64","ULONG", "DIMENSION","SIZE", "TEMPLATE", "TYPE", //"TYPE": position 14
	//the rest free
             "DESTROY_SEGMENT","FILENAME","GET_NAME", "GET_OS_HANDLE", "OFFSET", 
              "OS_HANDLE", "PRIVATE", "SYSV", KLISTEND};
  new DLibPro(lib::shmmap_pro,string("SHMMAP"),9,shmmapKey);
  new DLibPro(lib::shmunmap_pro,string("SHMUNMAP"),1);
  // Note: counted values, do not change order:
  const string shmvarKey[] = {"BYTE","COMPLEX","DCOMPLEX","DOUBLE","FLOAT","INTEGER","L64","LONG","UINT","UL64","ULONG","DIMENSION","SIZE", "TEMPLATE", "TYPE",KLISTEND};
  new DLibFunRetNew(lib::shmvar_fun,string("SHMVAR"),9,shmvarKey);
  new DLibFunRetNew(lib::shmdebug_fun,string("SHMDEBUG"),1);
  #endif
#if !defined(_WIN32) || defined(__CYGWIN__)
  if (iAmMaster) {
	new DLibPro(lib::gmem_setvar,string("GMEM_SETVAR"),3);
	new DLibFunRetNew(lib::gmem_getvar,string("GMEM_GETVAR"),2);
	new DLibPro(lib::gmem_abort, string("GMEM_ABORT"), 1);
	new DLibPro(lib::gmem_exit, string("GMEM_EXIT"), 1);
	new DLibFunRetNew(lib::gmem_fork, string("GMEM_FORK"), 1);
	const string gmem_sendKey[] = {"NOWAIT", KLISTEND};
	new DLibPro(lib::gmem_send, string("GMEM_SEND"), 2, gmem_sendKey);
	const string gmem_statusKey[] = {"ERROR", KLISTEND};
	new DLibFunRetNew(lib::gmem_status, string("GMEM_STATUS"), 1, gmem_statusKey);
  }
  new DLibPro(lib::gmem_subprocess_givevar,string("GMEM_SUBPROCESS_GIVEVAR"),2);
  new DLibFunRetNew(lib::gmem_subprocess_receivevar,string("GMEM_SUBPROCESS_RECEIVEVAR"),1);
#endif
}

