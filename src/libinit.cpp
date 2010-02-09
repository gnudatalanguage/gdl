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

#include "math_fun.hpp"
#include "math_fun_ac.hpp"
#include "math_fun_gm.hpp"
#include "math_fun_ng.hpp"
#include "plotting.hpp"

#include "file.hpp"

#include "gsl_fun.hpp"

#ifdef USE_PYTHON
#include "gdlpython.hpp"
#endif

#include "grib.hpp"

// for extensions
#include "new.hpp"

// for sorting lists by name
struct CompLibFunName: public std::binary_function< DLibFun*, DLibFun*, bool>
{
  bool operator() ( DLibFun* f1, DLibFun* f2) const
  { return f1->ObjectName() < f2->ObjectName();}
};

struct CompLibProName: public std::binary_function< DLibPro*, DLibPro*, bool>
{
  bool operator() ( DLibPro* f1, DLibPro* f2) const
  { return f1->ObjectName() < f2->ObjectName();}
};

using namespace std;

// declare here other library init functions
void LibInit_jmg(); // libinit_jmg.cpp
void LibInit_cl(); // libinit_cl.cpp
void LibInit_mes(); // libinit_mes.cpp
void LibInit_ac(); // libinit_ac.cpp
void LibInit_gm(); // libinit_gm.cpp
void LibInit_ng(); // libinit_ng.cpp

void LibInit()
{
  // call other library init functions
  LibInit_jmg();
  LibInit_cl();
  LibInit_mes();
  LibInit_ac();
  LibInit_gm();
  LibInit_ng(); 
  const char KLISTEND[] = "";

  const string cpuKey[]={ "RESET","RESTORE","TPOOL_MAX_ELTS", "TPOOL_MIN_ELTS",
					"TPOOL_NTHREADS","VECTOR_ENABLE",KLISTEND};
  new DLibPro(lib::cpu,string("CPU"),0,cpuKey);

  const string get_kbrdKey[]={"ESCAPE","KEY_NAME",KLISTEND};
  new DLibFunRetNew(lib::get_kbrd,string("GET_KBRD"),1,NULL,get_kbrdKey);

  const string svdcKey[]={"COLUMN","ITMAX","DOUBLE",KLISTEND};
  new DLibPro(lib::svdc,string("SVDC"),4,svdcKey);

  new DLibFun(lib::temporary,string("TEMPORARY"),1);

  const string routine_infoKey[]={"FUNCTIONS","SYSTEM","DISABLED","ENABLED",
				  "PARAMETERS", KLISTEND};
  new DLibFun(lib::routine_info,string("ROUTINE_INFO"),1,routine_infoKey);

  const string spawnKey[]={ "COUNT","EXIT_STATUS","PID",
			    "SH","NOSHELL",KLISTEND};
  new DLibPro(lib::spawn_pro,string("SPAWN"),3,spawnKey);

  const string bytsclKey[]={"MIN","MAX","TOP","NAN",KLISTEND};
  new DLibFun(lib::bytscl,string("BYTSCL"),3,bytsclKey);

  const string n_tagsKey[]={"DATA_LENGTH","LENGTH",KLISTEND};
  new DLibFunRetNew(lib::n_tags,string("N_TAGS"),1,n_tagsKey);
  
  const string byteorderKey[]={"SSWAP","LSWAP","L64SWAP",
			       "SWAP_IF_BIG_ENDIAN",
			       "SWAP_IF_LITTLE_ENDIAN",
			       "NTOHL","NTOHS","HTONL","HTONS",
			       "FTOXDR","DTOXDR","XDRTOF","XDRTOD",
			       KLISTEND};
  new DLibPro(lib::byteorder,string("BYTEORDER"),-1,byteorderKey);

  const string obj_classKey[]={"COUNT","SUPERCLASS",KLISTEND};
  new DLibFunRetNew(lib::obj_class,string("OBJ_CLASS"),1,obj_classKey);

  new DLibFunRetNew(lib::obj_isa,string("OBJ_ISA"),2);

  const string rebinKey[]={"SAMPLE",KLISTEND};
  new DLibFunRetNew(lib::rebin_fun,string("REBIN"),9,rebinKey);

  const string convolKey[]={"CENTER","EDGE_TRUNCATE","EDGE_WRAP",KLISTEND};
  new DLibFunRetNew(lib::convol,string("CONVOL"),3,convolKey);

  const string file_searchKey[]={"COUNT","EXPAND_ENVIRONMENT","EXPAND_TILDE",
				 "FOLD_CASE","ISSUE_ACCESS_ERROR",
				 "MARK_DIRECTORY","NOSORT","QUOTE",
				 "MATCH_INITIAL_DOT",
				 "MATCH_ALL_INITIAL_DOT","FULLY_QUALIFY_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_search,string("FILE_SEARCH"),2,file_searchKey);

  const string expand_pathKey[]={"ARRAY","ALL_DIRS","COUNT",KLISTEND};
  new DLibFunRetNew(lib::expand_path,string("EXPAND_PATH"),1,expand_pathKey);
  
  const string strjoinKey[]={"SINGLE",KLISTEND};
  new DLibFunRetNew(lib::strjoin,string("STRJOIN"),2,strjoinKey,NULL,true/*retConstant*/);

  const string strcmpKey[]={"FOLD_CASE",KLISTEND};
  new DLibFunRetNew(lib::strcmp_fun,string("STRCMP"),3,strcmpKey,NULL,true);

  new DLibFunRetNew(lib::eof_fun,string("EOF"),1);

  new DLibFunRetNew(lib::arg_present,string("ARG_PRESENT"),1);

  const string messageKey[]={"CONTINUE","INFORMATIONAL","IOERROR",
			     "NONAME","NOPREFIX","NOPRINT",
			     "RESET","TRACEBACK",KLISTEND};
  new DLibPro(lib::message,string("MESSAGE"),1,messageKey);
  
  const string cdKey[]={"CURRENT",KLISTEND};
  new DLibPro(lib::cd_pro,string("CD"),1,cdKey);

  const string file_testKey[]={"DIRECTORY","EXECUTABLE","READ",
			       "REGULAR","WRITE","ZERO_LENGTH",
			       "GET_MODE",
			       "BLOCK_SPECIAL","CHARACTER_SPECIAL",
			       "NAMED_PIPE","SOCKET","SYMLINK","NOEXPAND_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_test,string("FILE_TEST"),1,file_testKey);

  const string file_sameKey[]={"NOEXPAND_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_same,string("FILE_SAME"),2,file_sameKey);

  const string file_infoKey[]={"NOEXPAND_PATH",KLISTEND};
  new DLibFunRetNew(lib::file_info,string("FILE_INFO"),2,file_infoKey);

  new DLibFunRetNew(lib::shift_fun,string("SHIFT"),9,NULL,NULL,true);

  const string sortKey[]={"L64",KLISTEND};
  new DLibFunRetNew(lib::sort_fun,string("SORT"),1,sortKey,NULL,true);

  const string medianKey[]={"EVEN","DOUBLE","DIMENSION",KLISTEND};
  new DLibFunRetNew(lib::median,string("MEDIAN"),2,medianKey);
  
  new DLibFunRetNew(lib::transpose,string("TRANSPOSE"),2,NULL,NULL,true);

  new DLibPro(lib::retall,string("RETALL"));

  const string catchKey[]={"CANCEL",KLISTEND};
  new DLibPro(lib::catch_pro,string("CATCH"),1,catchKey);
  new DLibPro(lib::on_error,string("ON_ERROR"),1);

  new DLibFunRetNew(lib::recall_commands,string("RECALL_COMMANDS"),0);

  const string exitKey[]={"NO_CONFIRM","STATUS",KLISTEND};
  new DLibPro(lib::exitgdl,string("EXIT"),0,exitKey);
  
  const string helpKey[]={"STRUCTURES","ROUTINES","BRIEF",
			  "OUTPUT","PROCEDURES","FUNCTIONS",
			  "INFO","LIB","CALLS","RECALL_COMMANDS",
                          "MEMORY", KLISTEND};
  new DLibPro(lib::help,string("HELP"),-1,helpKey);

  const string memoryKey[]={"CURRENT","HIGHWATER","NUM_ALLOC",
    "NUM_FREE","STRUCTURE","L64",KLISTEND};
  new DLibFunRetNew(lib::memory, string("MEMORY"), 1, memoryKey, NULL);

  // printKey, readKey and stringKey are closely associated
  // as the same functions are called "FORMAT" till "MONTH"
  // must be the first four keywords
  const string printKey[]={"FORMAT","AM_PM","DAYS_OF_WEEK","MONTH",
			   "STDIO_NON_FINITE",KLISTEND};
  new DLibPro(lib::print,string("PRINT"),-1,printKey);
  new DLibPro(lib::printf,string("PRINTF"),-1,printKey);

  const string readKey[]={"FORMAT","AM_PM","DAYS_OF_WEEK","MONTH",
			  "PROMPT",KLISTEND};
  new DLibPro(lib::read,string("READ"),-1,readKey);
  new DLibPro(lib::readf,string("READF"),-1,readKey);

  const string readsKey[]={"FORMAT","AM_PM","DAYS_OF_WEEK","MONTH",
			   KLISTEND}; // no PROMPT
  new DLibPro(lib::reads,string("READS"),-1,readsKey);

  // allow printing (of expressions) with all keywords 
  // (easier to implement this way)
  new DLibPro(lib::stop,string("STOP"),-1,printKey); 

  const string defsysvKey[]={"EXISTS",KLISTEND};
  new DLibPro(lib::defsysv,string("DEFSYSV"),3,defsysvKey); 

  const string heap_gcKey[]={"PTR","OBJ","VERBOSE",KLISTEND};
  new DLibPro(lib::heap_gc,string("HEAP_GC"),0,heap_gcKey); 


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
  new DLibFunRetNew(lib::strarr,string("STRARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::complexarr,string("COMPLEXARR"),MAXRANK,arrKey,NULL,true);
  new DLibFunRetNew(lib::dcomplexarr,string("DCOMPLEXARR"),MAXRANK,arrKey,NULL,true);

  const string ptrArrKey[]={"NOZERO","ALLOCATE_HEAP",KLISTEND};
  new DLibFunRetNew(lib::ptrarr,string("PTRARR"),MAXRANK,ptrArrKey);
  new DLibFunRetNew(lib::objarr,string("OBJARR"),MAXRANK,arrKey);

  const string ptr_newKey[]={"NO_COPY","ALLOCATE_HEAP",KLISTEND};
  new DLibFunRetNew(lib::ptr_new,string("PTR_NEW"),1,ptr_newKey);

  const string obj_validKey[]={"CAST","COUNT",KLISTEND};
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
			 "STRING","UINT","UL64","ULONG",KLISTEND};
  new DLibFunRetNew(lib::bindgen,string("BINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::indgen,string("INDGEN"),MAXRANK,indKey,NULL,true);
  new DLibFunRetNew(lib::uindgen,string("UINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::sindgen,string("SINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::lindgen,string("LINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::ulindgen,string("ULINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::l64indgen,string("L64INDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::ul64indgen,string("UL64INDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::findgen,string("FINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::dindgen,string("DINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::cindgen,string("CINDGEN"),MAXRANK,NULL,NULL,true);
  new DLibFunRetNew(lib::dcindgen,string("DCINDGEN"),MAXRANK,NULL,NULL,true);

  new DLibFunRetNew(lib::n_elements,string("N_ELEMENTS"),1,NULL,NULL,true);

  new DLibFun(lib::execute,string("EXECUTE"),2);

  const string openKey[]={"APPEND","COMPRESS","BUFSIZE",
			  "DELETE","ERROR","F77_UNFORMATTED",
			  "GET_LUN" /*7*/,"MORE","STDIO",
			  "SWAP_ENDIAN","SWAP_IF_BIG_ENDIAN",
			  "SWAP_IF_LITTLE_ENDIAN" /*12*/,
			  "VAX_FLOAT","WIDTH","XDR", "BLOCK",
			  "NOAUTOMODE","BINARY","STREAM",
			  KLISTEND};
  const string openWarnKey[]={"INITIALSIZE","EXTENDSIZE",KLISTEND}; // VAX only
  new DLibPro(lib::openr,string("OPENR"),3,openKey,openWarnKey);
  new DLibPro(lib::openw,string("OPENW"),3,openKey,openWarnKey);
  new DLibPro(lib::openu,string("OPENU"),3,openKey,openWarnKey);
  new DLibPro(lib::get_lun,string("GET_LUN"),1);

  const string socketKey[]={"ERROR","GET_LUN","STDIO",
			    "SWAP_ENDIAN","SWAP_IF_BIG_ENDIAN",
			    "SWAP_IF_LITTLE_ENDIAN","WIDTH",
			    "CONNECT_TIMEOUT","READ_TIMEOUT",
			    "WRITE_TIMEOUT",
			    KLISTEND};
  new DLibPro(lib::socket,string("SOCKET"),3,socketKey);

  new DLibPro(lib::flush_lun,string("FLUSH"),-1);

  const string closeKey[]={"EXIT_STATUS","FORCE","FILE","ALL",KLISTEND};
  new DLibPro(lib::close_lun,string("CLOSE"),-1,closeKey);
  
  const string free_lunKey[]={"EXIT_STATUS","FORCE",KLISTEND};
  new DLibPro(lib::free_lun,string("FREE_LUN"),-1,free_lunKey);

  const string writeuKey[]={"TRANSFER_COUNT",KLISTEND};
  new DLibPro(lib::writeu,string("WRITEU"),-1,writeuKey);
  new DLibPro(lib::readu,string("READU"),-1,writeuKey);

  const string resolve_routineKey[]={"EITHER","IS_FUNCTION",KLISTEND};
  new DLibPro(lib::resolve_routine,string("RESOLVE_ROUTINE"),1,
	      resolve_routineKey);

  const string assocKey[]={"PACKED",KLISTEND};
  new DLibFunRetNew(lib::assoc,string("ASSOC"),3,assocKey);
  
  const string stringKey[]={"FORMAT","AM_PM","DAYS_OF_WEEK","MONTH",
			    "PRINT",KLISTEND};
  new DLibFunRetNew(lib::string_fun,string("STRING"),-1,stringKey,NULL,true);
  new DLibFunRetNew(lib::byte_fun,string("BYTE"),10,NULL,NULL,true);
  const string fixKey[]={"TYPE",KLISTEND};
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

  new DLibFunRetNew(lib::gdl_logical_and,string("LOGICAL_AND"),2,NULL,NULL,true);
  new DLibFunRetNew(lib::gdl_logical_or,string("LOGICAL_OR"),2,NULL,NULL,true);
  new DLibFunRetNew(lib::logical_true,string("LOGICAL_TRUE"),1,NULL,NULL,true);

  new DLibFunRetNew(lib::replicate,string("REPLICATE"),9,NULL,NULL,true);
  new DLibPro(lib::replicate_inplace_pro,string("REPLICATE_INPLACE"),6);

  new DLibFunRetNew(lib::sin_fun,string("SIN"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::cos_fun,string("COS"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::tan_fun,string("TAN"),1,NULL,NULL,true);

  new DLibFunRetNew(lib::sinh_fun,string("SINH"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::cosh_fun,string("COSH"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::tanh_fun,string("TANH"),1,NULL,NULL,true);

  new DLibFunRetNew(lib::asin_fun,string("ASIN"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::acos_fun,string("ACOS"),1,NULL,NULL,true);
  const string atanKey[] = {"PHASE", KLISTEND};
  new DLibFunRetNew(lib::atan_fun,string("ATAN"),2,atanKey,NULL,true);

  new DLibFunRetNew(lib::alog_fun,string("ALOG"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::alog10_fun,string("ALOG10"),1,NULL,NULL,true);

  new DLibFunRetNew(lib::sqrt_fun,string("SQRT"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::abs_fun,string("ABS"),1,NULL,NULL,true);

  new DLibFunRetNew(lib::exp_fun,string("EXP"),1,NULL,NULL,true);

  const string roundKey[]={"L64",KLISTEND};
  // retConstant: check definition of the rounding functions if they depend 
  // from some sys var (defining a round mode) 
  // (probably nobody rounds a constant anyway)
  new DLibFunRetNew(lib::round_fun,string("ROUND"),1,roundKey);
  const string ceilfloorKey[]={"L64",KLISTEND};
  new DLibFunRetNew(lib::ceil_fun,string("CEIL"),1,ceilfloorKey);
  new DLibFunRetNew(lib::floor_fun,string("FLOOR"),1,ceilfloorKey);

  new DLibFunRetNew(lib::conj_fun,string("CONJ"),1);
  new DLibFunRetNew(lib::imaginary_fun,string("IMAGINARY"),1);

  const string strcompressKey[]={"REMOVE_ALL",KLISTEND};
  new DLibFunRetNew(lib::strcompress,string("STRCOMPRESS"),1,strcompressKey,NULL,true);
  new DLibFunRetNew(lib::strlowcase,string("STRLOWCASE"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::strupcase,string("STRUPCASE"),1,NULL,NULL,true);
  new DLibFunRetNew(lib::strlen,string("STRLEN"),1,NULL,NULL,true);
  const string strmidKey[]={"REVERSE_OFFSET",KLISTEND};
  new DLibFunRetNew(lib::strmid,string("STRMID"),3,strmidKey,NULL,true);
  new DLibFunRetNew(lib::strtrim,string("STRTRIM"),2,NULL,NULL,true);
  const string strposKey[]={"REVERSE_OFFSET","REVERSE_SEARCH",KLISTEND};
  new DLibFunRetNew(lib::strpos,string("STRPOS"),3,strposKey,NULL,true);
  new DLibPro(lib::strput,string("STRPUT"),3);
  
  const string whereKey[]={"COMPLEMENT","NCOMPLEMENT",KLISTEND};
  new DLibFunRetNew(lib::where,string("WHERE"),2,whereKey);

  const string totalKey[]={"CUMULATIVE","DOUBLE","NAN","INTEGER",KLISTEND};
  new DLibFunRetNew(lib::total,string("TOTAL"),2,totalKey,NULL,true);

  const string productKey[]={"CUMULATIVE","NAN","INTEGER",KLISTEND};
  new DLibFunRetNew(lib::product,string("PRODUCT"),2,productKey,NULL,true);

  new DLibFunRetNew(lib::n_params,string("N_PARAMS"),1); // IDL allows one parameter
  new DLibFunRetNew(lib::keyword_set,string("KEYWORD_SET"),1);

  const string array_equalKey[]={"NO_TYPECONV",KLISTEND};
  new DLibFunRetNew(lib::array_equal,string("ARRAY_EQUAL"),2,array_equalKey,NULL,true);
  
  const string minKey[]={"MAX","NAN","SUBSCRIPT_MAX","DIMENSION",KLISTEND};
  new DLibFunRetNew(lib::min_fun,string("MIN"),2,minKey,NULL,true);
  const string maxKey[]={"MIN","NAN","SUBSCRIPT_MIN","DIMENSION",KLISTEND};
  new DLibFunRetNew(lib::max_fun,string("MAX"),2,maxKey,NULL,true);

  // retConstant: structs are tricky: struct resolution depends from !PATH
  // and this might change during runtime, but if treated as retConstant
  // the struct would be already defined at compile time.
  const string create_structKey[]={"NAME",KLISTEND};
  new DLibFunRetNew(lib::create_struct,string("CREATE_STRUCT"),-1,create_structKey/*,true*/);

  new DLibFunRetNew(lib::rotate,string("ROTATE"),2,NULL,NULL,true);

  const string reverseKey[] = {"OVERWRITE", KLISTEND};
  new DLibFunRetNew(lib::reverse, string("REVERSE"), 2, reverseKey, NULL, true);

//   const string minKey[]={"MAX",KLISTEND};
//   new DLibFun(lib::min_fun,string("MIN"),2,minKey);
//   const string maxKey[]={"MIN",KLISTEND};
//   new DLibFun(lib::max_fun,string("MAX"),2,maxKey);

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
  new DLibPro(lib::wshow,string("WSHOW"),2);

  const string cursorKey[]={"CHANGE","DOWN","NOWAIT","UP","WAIT",
				"DATA","DEVICE","NORMAL",KLISTEND};
  new DLibPro(lib::cursor,string("CURSOR"),3,cursorKey);

  const string set_plotKey[]={"COPY","INTERPOLATE",KLISTEND};
  new DLibPro(lib::set_plot,string("SET_PLOT"),1,set_plotKey);
  
  const string get_screen_sizeKey[]={"RESOLUTION",KLISTEND};
  new DLibFun(lib::get_screen_size,string("GET_SCREEN_SIZE"),1,get_screen_sizeKey);

  const string tvlctKey[]={"GET","HLS","HSV",KLISTEND};
  new DLibPro(lib::tvlct,string("TVLCT"),4,tvlctKey);

  const string deviceKey[]=
    {
      "CLOSE_FILE","FILENAME","LANDSCAPE","PORTRAIT",
      "DECOMPOSED","GET_DECOMPOSED","Z_BUFFERING","SET_RESOLUTION",
      "SET_CHARACTER_SIZE","GET_VISUAL_DEPTH","XSIZE","YSIZE","COLOR",KLISTEND
    };
  const string deviceWarnKey[] = {"RETAIN", KLISTEND};
  new DLibPro(lib::device,string("DEVICE"),0, deviceKey, deviceWarnKey);

  const string plotKey[]=
    {
     // GRAPHIC KEYWORDS
     // 0
     "BACKGROUND","CHARSIZE","CHARTHICK","CLIP",
     // 4
     "COLOR",     "DATA",    "DEVICE",   "FONT",
     // 8
     "LINESTYLE", "NOCLIP",  "NODATA",   "NOERASE", 
     // 12
     "NORMAL",    "POSITION","PSYM",     "SUBTITLE",
     // 16
     "SYMSIZE",   "T3D",     "THICK",    "TICKLEN", 
     // 20
     "TITLE",
     // 21
     "XCHARSIZE",    "XGRIDSTYLE", "XMARGIN", "XMINOR",
     // 25
     "XRANGE",       "XSTYLE",     "XTHICK",  "XTICKFORMAT",
     // 29
     "XTICKINTERVAL","XTICKLAYOUT","XTICKLEN","XTICKNAME",
     // 33
     "XTICKS",       "XTICKUNITS", "XTICKV",  "XTICK_GET",
     // 37
     "XTITLE",
     // 38
     "YCHARSIZE",    "YGRIDSTYLE", "YMARGIN", "YMINOR",
     // 42
     "YRANGE",       "YSTYLE",     "YTHICK",  "YTICKFORMAT",
     // 46
     "YTICKINTERVAL","YTICKLAYOUT","YTICKLEN","YTICKNAME",
     // 50
     "YTICKS",       "YTICKUNITS", "YTICKV",  "YTICK_GET",
     // 51
     "YTITLE",
     // 55
     "ZCHARSIZE",    "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
     // 59
     "ZRANGE",       "ZSTYLE",     "ZTHICK",  "ZTICKFORMAT",
     // 63
     "ZTICKINTERVAL","ZTICKLAYOUT","ZTICKLEN","ZTICKNAME",
     // 67
     "ZTICKS",       "ZTICKUNITS", "ZTICKV",  "ZTICK_GET",
     // 71
     "ZTITLE",
     // 72
     "ZVALUE", 

     // PLOT keywords
     // 73
     "ISOTROPIC",
     "MAX_VALUE", "MIN_VALUE","NSUM",     "POLAR",
     "XLOG",     "YLOG",     "YNOZERO", "XTYPE", "YTYPE", 
     KLISTEND
    };
  new DLibPro(lib::plot,string("PLOT"),2,plotKey);

  const string axisKey[]=
    {
     // GRAPHIC KEYWORDS
     // 0
     "CHARSIZE","CHARTHICK",
     // 2
     "COLOR",     "DATA",    "DEVICE",   "FONT",
     // 6
     "NODATA",    "NOERASE", 
     // 8
     "NORMAL",    "SUBTITLE",
     // 10
     "T3D",       "TICKLEN", 
     // 12
     "XCHARSIZE", "XGRIDSTYLE", "XMARGIN", "XMINOR",
     // 16
     "XRANGE",    "XSTYLE",     "XTHICK",  "XTICKFORMAT",
     // 20
     "XTICKLEN",  "XTICKNAME",
     // 22
     "XTICKS",    
     // 23
     "XTITLE",
     // 24
     "YCHARSIZE",  "YGRIDSTYLE", "YMARGIN", "YMINOR",
     // 28
     "YRANGE",     "YSTYLE",     "YTHICK",  "YTICKFORMAT",
     // 30
     "YTICKLEN",   "YTICKNAME",
     // 32
     "YTICKS",
     // 33
     "YTITLE",
     // 34
     "ZCHARSIZE",  "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
     // 38
     "ZRANGE",     "ZSTYLE",     "ZTHICK",  "ZTICKFORMAT",
     // 42
     "ZTICKLEN",   "ZTICKNAME",
     // 44
     "ZTICKS",
     // 45
     "ZTITLE",
     // 46
     "ZVALUE", 

     // AXIS keywords
     // 47
     "SAVE", "XAXIS", "YAXIS", "XLOG", "YLOG", "XTYPE", "YTYPE", "YNOZERO", "THICK",
     KLISTEND
    };
  new DLibPro(lib::axis,string("AXIS"),3,axisKey);

  const string oplotKey[]=
    {
      // GRAPHIC KEYWORDS
      // 0
      "CLIP","COLOR", "LINESTYLE", "NOCLIP",
      // 4
      "PSYM", "SYMSIZE",  "T3D",  "THICK",
      // 8
      "MAX_VALUE", "MIN_VALUE","NSUM",  "POLAR",
      KLISTEND
    };
  new DLibPro(lib::oplot, string("OPLOT"),2,oplotKey);

  const string plotsKey[]=
    {
      // GRAPHIC KEYWORDS
      // 0
      "CLIP","COLOR", "LINESTYLE", "NOCLIP",
      // 4
      "PSYM", "SYMSIZE",  "T3D",  "THICK",
      // 8
      "DATA","DEVICE","NORMAL",
      KLISTEND
    };
  new DLibPro(lib::plots, string("PLOTS"), 3, plotsKey);

  const string surfaceKey[]=
    {
     // GRAPHIC KEYWORDS
     // 0
     "BACKGROUND","CHARSIZE","CHARTHICK","CLIP",
     // 4
     "COLOR",     "DATA",    "DEVICE",   "FONT",
     // 8
     "LINESTYLE", "NOCLIP",  "NODATA",   "NOERASE", 
     // 12
     "NORMAL",    "POSITION",     "SUBTITLE",
     // 15
     "T3D",     "THICK",    "TICKLEN", 
     // 18
     "TITLE",
     // 19
     "XCHARSIZE",    "XGRIDSTYLE", "XMARGIN", "XMINOR",
     // 23
     "XRANGE",       "XSTYLE",     "XTHICK",  "XTICKFORMAT",
     // 27
     "XTICKINTERVAL","XTICKLAYOUT","XTICKLEN","XTICKNAME",
     // 31
     "XTICKS",       "XTICKUNITS", "XTICKV",  "XTICK_GET",
     // 35
     "XTITLE",
     // 36
     "YCHARSIZE",    "YGRIDSTYLE", "YMARGIN", "YMINOR",
     // 40
     "YRANGE",       "YSTYLE",     "YTHICK",  "YTICKFORMAT",
     // 44
     "YTICKINTERVAL","YTICKLAYOUT","YTICKLEN","YTICKNAME",
     // 48
     "YTICKS",       "YTICKUNITS", "YTICKV",  "YTICK_GET",
     // 52
     "YTITLE",
     // 53
     "ZCHARSIZE",    "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
     // 57
     "ZRANGE",       "ZSTYLE",     "ZTHICK",  "ZTICKFORMAT",
     // 61
     "ZTICKINTERVAL","ZTICKLAYOUT","ZTICKLEN","ZTICKNAME",
     // 65
     "ZTICKS",       "ZTICKUNITS", "ZTICKV",  "ZTICK_GET",
     // 69
     "ZTITLE",
     // 70
     "ZVALUE", 

     // SURFACE keywords
     // 73
     "MAX_VALUE", "MIN_VALUE","AX", "AZ",
     "XLOG", "XTYPE", "YLOG", "YTYPE", "ZLOG", "ZTYPE",
     KLISTEND
    };
  new DLibPro(lib::surface,string("SURFACE"),3,surfaceKey);

  const string contourKey[]=
    {
     // GRAPHIC KEYWORDS
     // 0
     "BACKGROUND","CHARSIZE","CHARTHICK","CLIP",
     // 4
     "COLOR",     "DATA",    "DEVICE",   "FONT",
     // 8
     "NOCLIP",  "NODATA",   "NOERASE", 
     // 11
     "NORMAL",    "POSITION",     "SUBTITLE",
     // 14
     "T3D",     "THICK",    "TICKLEN", 
     // 17
     "TITLE",
     // 18
     "XCHARSIZE",    "XGRIDSTYLE", "XMARGIN", "XMINOR",
     // 22
     "XRANGE",       "XSTYLE",     "XTHICK",  "XTICKFORMAT",
     // 26
     "XTICKLEN","XTICKNAME",
     // 28
     "XTICKS",       "XTICKV",  "XTICK_GET",
     // 31
     "XTITLE",
     // 32
     "YCHARSIZE",    "YGRIDSTYLE", "YMARGIN", "YMINOR",
     // 36
     "YRANGE",       "YSTYLE",     "YTHICK",  "YTICKFORMAT",
     // 40
     "YTICKLEN","YTICKNAME",
     // 42
     "YTICKS",       "YTICKV",  "YTICK_GET",
     // 45
     "YTITLE",
     // 46
     "ZCHARSIZE",    "ZGRIDSTYLE", "ZMARGIN", "ZMINOR",
     // 50
     "ZRANGE",       "ZSTYLE",     "ZTHICK",  "ZTICKFORMAT",
     // 54
     "ZTICKLEN","ZTICKNAME",
     // 56
     "ZTICKS",       "ZTICKV",  "ZTICK_GET",
     // 59
     "ZTITLE",
     // 60
     "ZVALUE", 
     // CONTOUR keywords
     // 73
     "LEVELS", "NLEVELS",
     "MAX_VALUE", "MIN_VALUE","AX", "AZ",
     "XLOG", "XTYPE", "YLOG", "YTYPE", "ZLOG", "ZTYPE", "FILL",  "ISOTROPIC",
     "FOLLOW", "C_CHARSIZE","OVERPLOT",
     KLISTEND
    };
  new DLibPro(lib::contour,string("CONTOUR"),3,contourKey);

  const string xyoutsKey[]=
    {
      "COLOR","DATA","NORMAL","DEVICE","CLIP",
      "ORIENTATION","ALIGNMENT","CHARSIZE","CHARTHICK",
      "NOCLIP","Z",KLISTEND
    };
  new DLibPro(lib::xyouts, string("XYOUTS"), 3, xyoutsKey);

  new DLibPro(lib::erase,string("ERASE"),1);
  
  const string laguerreKey[]={"DOUBLE","COEFFICIENTS",KLISTEND};
  new DLibFun(lib::laguerre,string("LAGUERRE"),3,laguerreKey);

  new DLibFun(lib::gauss_pdf,string("GAUSS_PDF"),1);
  new DLibFun(lib::gauss_cvf,string("GAUSS_CVF"),1);

  new DLibFun(lib::t_pdf,string("T_PDF"),2);

  new DLibPro(lib::caldat, string("CALDAT"), 7);

  // SA: the HYBRID key is used in imsl_zerosys.pro to switch to the modif. Powell algo. 
  const string newtonKey[] = {"DOUBLE", "ITMAX", "TOLF", "TOLX", "HYBRID", KLISTEND };
  const string newtonWarnKey[] = {"CHECK", "STEPMAX", "TOLMIN", KLISTEND};
  new DLibFun(lib::newton_broyden, string("NEWTON"), 2, newtonKey, newtonWarnKey);

  const string broydenKey[] = {"DOUBLE", "ITMAX", "TOLF", "TOLX", KLISTEND };
  const string broydenWarnKey[] = {"CHECK", "EPS", "STEPMAX", "TOLMIN", KLISTEND};
  new DLibFun(lib::newton_broyden, string("BROYDEN"), 2, broydenKey, broydenWarnKey);

  new DLibFun(lib::parse_url, string("PARSE_URL"), 1);
  new DLibFun(lib::locale_get, string("LOCALE_GET"), 0);
  new DLibFun(lib::get_login_info, string("GET_LOGIN_INFO"), 0);
  new DLibFun(lib::idl_base64, string("IDL_BASE64"), 1);

  const string ll_arc_distanceKey[] = {"DEGREES", KLISTEND };
  new DLibFun(lib::ll_arc_distance, string("LL_ARC_DISTANCE"), 3, ll_arc_distanceKey);

  const string command_line_argsKey[] = {"COUNT", KLISTEND };
  new DLibFun(lib::command_line_args_fun, string("COMMAND_LINE_ARGS"), 0, command_line_argsKey);

  const string pmKey[] = {"FORMAT", "TITLE", KLISTEND };
  new DLibPro(lib::pm, string("PM"), -1, pmKey);

  const string constantKey[] = {"DOUBLE", KLISTEND };
  new DLibFun(lib::constant, string("IMSL_CONSTANT"), 2, constantKey);

  const string get_drive_listKey[] = {"COUNT", KLISTEND };
  new DLibFun(lib::get_drive_list, string("GET_DRIVE_LIST"), 0, get_drive_listKey);

  const string binomialcoefKey[] = {"DOUBLE", KLISTEND };
  new DLibFun(lib::binomialcoef, string("IMSL_BINOMIALCOEF"), 2, binomialcoefKey);

  // SA: GRIB format support based on the ECMWF GRIB_API package (IDL does not support it yet)
  // GRIBAPI_ prefix is used in order to (hopefully) prevent future incompatibilities with IDL
  // -----------------------------------------------------------------------------------------
  // GRIB: file related
  new DLibFun(lib::grib_open_file_fun, string("GRIBAPI_OPEN_FILE"), 1); 
  new DLibFun(lib::grib_count_in_file_fun, string("GRIBAPI_COUNT_IN_FILE"), 1); 
  new DLibPro(lib::grib_close_file_pro, string("GRIBAPI_CLOSE_FILE"), 1); 
  // GRIB: message related
  new DLibFun(lib::grib_new_from_file_fun, string("GRIBAPI_NEW_FROM_FILE"), 1); 
  new DLibPro(lib::grib_release_pro, string("GRIBAPI_RELEASE"), 1); 
  // see comment in grib.cpp
  //new DLibFun(lib::grib_get_message_size_fun, string("GRIBAPI_GET_MESSAGE_SIZE"), 1);
  new DLibFun(lib::grib_clone_fun, string("GRIBAPI_CLONE"), 1); 
  // GRIB: data related
  new DLibFun(lib::grib_get_size_fun, string("GRIBAPI_GET_SIZE"), 2); 
  new DLibPro(lib::grib_get_pro, string("GRIBAPI_GET"), 3); 
  new DLibPro(lib::grib_get_data_pro, string("GRIBAPI_GET_DATA"), 4); 
  // -----------------------------------------------------------------------------------------

  new DLibFun(lib::crossp, string("CROSSP"), 2);
  //const string hanningKey[] = {"DOUBLE", "ALPHA", KLISTEND };
  //new DLibFun(lib::hanning, string("HANNING"), 2, hanningKey);
  const string wtnKey[] = {"COLUMN", "DOUBLE", "INVERSE", "OVERWRITE", KLISTEND };
  new DLibFun(lib::wtn, string("WTN"), 2, wtnKey);
  const string zeropolyKey[] = {"DOUBLE", "COMPANION", "JENKINS_TRAUB", KLISTEND };
  new DLibFun(lib::zeropoly, string("IMSL_ZEROPOLY"), 1, zeropolyKey);

  // sort lists
  sort( libFunList.begin(), libFunList.end(), CompLibFunName());
  sort( libProList.begin(), libProList.end(), CompLibProName());
}

