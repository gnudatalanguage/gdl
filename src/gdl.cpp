/***************************************************************************
                          gdl.cpp  -  main program
                             -------------------
    begin                : Wed Apr 18 16:58:14 JST 2001
    copyright            : (C) 2002-2006 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
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

// #ifndef VERSION
// #define VERSION "0.9"
// #endif

#include <string>
#include <csignal>
#include <cstdlib>
#if defined(_MSC_VER) || defined (_WIN32)
#include <io.h>
#define isatty _isatty
#else
#include <unistd.h> // isatty
#endif
#include <climits> // PATH_MAX
//patch #90
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifndef _WIN32
#include <sys/resource.h> //rlimits to augment stack size (needed fot DICOM objects)
#endif

#include "str.hpp"
#include "dinterpreter.hpp"
#include "terminfo.hpp"
#include "gdleventhandler.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef USE_MPI
#  include "mpi.h"
#endif

#ifdef HAVE_LOCALE_H
#  include <locale.h>
#endif

// GDLDATADIR
#include "config.h"

//we use gdlgstream in gdl.cpp
#include "gdlgstream.hpp"

//initialize wxWidgets system:  create an instance of wxAppGDL
#ifdef HAVE_LIBWXWIDGETS
#include "gdlwidget.hpp"
//displaced in gdlwidget.cpp to make wxGetApp() available under Python (in GDL.so)
//#ifndef __WXMAC__ 
//wxIMPLEMENT_APP_NO_MAIN( wxAppGDL);
//#else
//wxIMPLEMENT_APP_NO_MAIN( wxApp);
//#endif
#endif

#include "version.hpp"

using namespace std;

static void StartupMessage()
{
  cerr << "  GDL - GNU Data Language, Version " << VERSION << endl;
  cerr << "- For basic information type HELP,/INFO" << endl;
}

void LibInit(); // defined in libinit.cpp

namespace lib {
void SetGDLGenericGSLErrorHandler(); // defined in gsl_fun.cpp
}

// Nodar and Alain, May 2013: we try to optimize the value for CpuTPOOL_NTHREADS
// if *valid* external value for OMP_NUM_THREADS (0 < OMP_NUM_THREADS < nb_cores) we used it
// if not provided, we try to estimate a value looking at the average load

void InitOpenMP() {
#ifdef _OPENMP
  int suggested_num_threads, omp_num_core;  
  suggested_num_threads=get_suggested_omp_num_threads();
  omp_num_core=omp_get_num_procs();

  //  cerr << "estimated Threads :" << suggested_num_threads << endl;

  // we update iff needed (by default, "omp_num_threads" is initialiazed to "omp_num_core"
  if ((suggested_num_threads > 0) && (suggested_num_threads < omp_num_core)) {

    // update of !cpu.TPOOL_NTHREADS
    DStructGDL* cpu = SysVar::Cpu();
    static unsigned NTHREADSTag = cpu->Desc()->TagIndex( "TPOOL_NTHREADS");
    (*static_cast<DLongGDL*>( cpu->GetTag( NTHREADSTag, 0)))[0] =suggested_num_threads;

    // effective global change of num of treads using omp_set_num_threads()
    CpuTPOOL_NTHREADS=suggested_num_threads;
    omp_set_num_threads(suggested_num_threads);
  } else {
    CpuTPOOL_NTHREADS=omp_get_num_procs();
    omp_set_num_threads(CpuTPOOL_NTHREADS);
  }
  //  cout << CpuTPOOL_NTHREADS <<endl;
#endif
}

void AtExit()
{
  //this function probably cleans otherwise cleaned objets and should be called only for debugging purposes.
  cerr << "Using AtExit() for debugging" << endl;
  cerr << flush; cout << flush; clog << flush;
  // clean up everything
  // (for debugging memory leaks)
  ResetObjects();
  PurgeContainer(libFunList);
  PurgeContainer(libProList);
}

#ifndef _WIN32
void GDLSetLimits()
{
#define GDL_PREFERED_STACKSIZE  1024000000 //1000000*1024 like IDL
struct rlimit* gdlstack=new struct rlimit;
  int r=getrlimit(RLIMIT_STACK,gdlstack); 
//  cerr <<"Current rlimit = "<<gdlstack->rlim_cur<<endl;
//  cerr<<"Max rlimit = "<<  gdlstack->rlim_max<<endl;
  if (gdlstack->rlim_cur >= GDL_PREFERED_STACKSIZE ) return; //the bigger the better.
  if (gdlstack->rlim_max > GDL_PREFERED_STACKSIZE ) gdlstack->rlim_cur=GDL_PREFERED_STACKSIZE; //not completely satisfactory.
  r=setrlimit(RLIMIT_STACK,gdlstack);
}
#endif

void InitGDL()
{
#ifndef _WIN32
  GDLSetLimits();
#endif

//rl_event_hook (defined below) uses a wxwidgets event loop, so wxWidgets must be started
#ifdef HAVE_LIBWXWIDGETS
    if (useWxWidgets) GDLWidget::Init();
#endif

#if defined(HAVE_LIBREADLINE)
  // initialize readline (own version - not pythons one)
  // in includefirst.hpp readline is disabled for python_module
  rl_initialize();
  char rlName[] = "GDL";
  rl_readline_name = rlName;
  //Our handler takes too long
  //when editing the command line with ARROW keys. (bug 562). (used also in dinterpreted.cpp )
  //but... without it we have no graphics event handler! FIXME!!! 
  rl_event_hook = GDLEventHandler;
#endif

  // ncurses blurs the output, initialize TermWidth here
  TermWidth();

  // initializations
  InitObjects();

  // init library functions
  LibInit(); 
    
  // ensuring we work in the C locale (needs to be called after InitObjects and LibInit!!! 
  // as some code there calls setlocale as well, e.g. MagickInit)
#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL, "C");
#endif

  // for debug one could turn on all floating point exceptions, it will stop at first one.
  //  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW );

  signal(SIGINT,ControlCHandler);

  lib::SetGDLGenericGSLErrorHandler();
}

static bool trace_me;

// SA: for use in COMMAND_LINE_ARGS()
namespace lib {
  extern std::vector<std::string> command_line_args;
#ifdef _WIN32
  bool posixpaths;
#endif
  bool gdlarg_present(const char* s)
  {
		for (size_t i = 0; i < command_line_args.size(); i++)
			  if( command_line_args[i] == s )  return true;
		return false;
		  }
  bool trace_arg()
  {
		for (size_t i = 0; i < command_line_args.size(); i++) 
			  if( command_line_args[i] == "trace" )  return true;
		return false;
   }

}

#include <whereami.h>

namespace MyPaths {
  std::string getExecutablePath(){
  char* path = NULL;
  
  int length, dirname_length;
  int i;
  length = wai_getExecutablePath(NULL, 0, &dirname_length);
  if (length > 0)
  {
    path = (char*)malloc(length + 1);
    if (!path) return std::string(".");
    wai_getExecutablePath(path, length, &dirname_length);
    path[dirname_length] = '\0';
//    printf("  dirname: %s\n", path);
    std::string pathstring(path);
    free(path);
    return pathstring;
  }
  return std::string(".");
}
}


int main(int argc, char *argv[])
{
#ifdef GDL_DEBUG
  if( atexit( AtExit) != 0) cerr << "atexit registration failed." << endl;
#endif
  // indicates if the user wants to see the welcome message
  bool quiet = false;
  bool gdlde = false;

//The default installation location --- will not always be there.  
  gdlDataDir = std::string(GDLDATADIR);
  gdlLibDir = std::string(GDLLIBDIR);
#ifdef _WIN32
  std::replace(gdlDataDir.begin(), gdlDataDir.end(), '/', '\\');
  std::replace(gdlLibDir.begin(), gdlLibDir.end(), '/', '\\');
#endif 

//check where is the executable being run
 std::string whereami=MyPaths::getExecutablePath();
// if I am at a 'bin' location, then there are chances that I've bee INSTALLED, so all the resources I need can be accessed relatively to this 'bin' directory.
// if not, then I'm probably just a 'build' gdl and my ressources may (should?) be in the default location GDLDATADIR
  std::size_t pos=whereami.rfind("bin");
  if (pos == whereami.size()-3) { //we are the installed gdl!
    gdlDataDir.assign( whereami+ lib::PathSeparator() + ".." + lib::PathSeparator() + "share" + lib::PathSeparator() + "gnudatalanguage") ;
//    std::cerr<<"installed at: "<<gdlDataDir<<std::endl;
  }

//PATH. This one is often modified by people before starting GDL.
  string gdlPath=GetEnvPathString("GDL_PATH"); //warning: is a Path, use system separator.
  if( gdlPath == "") gdlPath=GetEnvString("IDL_PATH"); //warning: is a Path, use system separator.
  if( gdlPath == "") gdlPath = gdlDataDir + lib::PathSeparator() + "lib";

//LIBDIR. Can be '' in which case the location of drivers is deduced from the location of
//the executable (OSX, Windows, unix in user-installed mode).
  string driversPath = GetEnvPathString("GDL_DRV_DIR");
  if (driversPath == "") { //NOT enforced by GDL_DRV_DIR
    driversPath = gdlLibDir; //e.g. Fedora
    if (driversPath == "") { //NOT enforced by GDLLIBDIR at build : not a distro
      driversPath = gdlDataDir + lib::PathSeparator() + "drivers"; //deduced from the location of the executable 
    }
  }
  //drivers if local
  useLocalDrivers=false;
  bool driversNotFound=false;

  //The current value for PLPLOT_DRV_DIR.
  //To find our drivers, the plplot library needs to have PLPLOT_DRV_DIR set to the good path, i.e., driversPath.
  const char* DrvEnvName = "PLPLOT_DRV_DIR";
  //In a startup message (below), the value of $PLPLOT_DRV_DIR appears.
  //It will be the value set inside the program (just below) to find the relevant drivers.

#ifdef INSTALL_LOCAL_DRIVERS
  useLocalDrivers=true;
  //For WIN32 the drivers dlls are copied along with the gdl.exe and plplot does not use PLPLOT_DRV_DIR to find them.
#ifndef _WIN32
  char* oldDriverEnv=getenv(DrvEnvName);
  // We must declare here (and not later) where our local copy of (customized?) drivers is to be found.
  char s[256];
  strcpy(s,DrvEnvName);
  strcat(s,"=");
  strcat(s,driversPath.c_str());
      //set nex drvPath as PLPLOT_DRV_DIR
  putenv(s);
  //Now, it is possible that GDL WAS compiled with INSTALL_LOCAL_DRIVERS, but the plplot installation is NOT compiled with DYNAMIC DRIVERS.
  //So I check here the plplot driver list to check if wxwidgets is present. If not, useLocalDriver=false
  bool driversOK=GDLGStream::checkPlplotDriver("ps"); //ps because xwin and wxwidgets may be absent. ps is always present.
  if (!driversOK) {
    driversNotFound=true; 
    useLocalDrivers=false;
    unsetenv(DrvEnvName); //unknown on windows
    //eventually restore previous value
    if (oldDriverEnv) {
      strcpy(s,DrvEnvName);
      strcat(s,"=");
      strcat(s,oldDriverEnv);
      putenv(s);
    }
    plend(); //this is necessary to reset PLPLOT to a state that will read again the driver configuration at PLPLOT_DRV_DIR
             // otherwise the next call to checkPlplotDriver() in GDLWxStream will fail.
  }
#endif
#endif
  // keeps a list of files to be executed after the startup file
  // and before entering the interactive mode
  vector<string> batch_files;
  string statement;
  string pretendRelease;
  bool strict_syntax=false;
  bool syntaxOptionSet=false;

  bool force_no_wxgraphics = false;
  usePlatformDeviceName=false;
  tryToMimicOriginalWidgets = false;
  useDSFMTAcceleration = true;
  iAmANotebook=false; //option --notebook
 #ifdef HAVE_LIBWXWIDGETS 

    #if defined (__WXMAC__) 
      useWxWidgets=true;
    #elif defined (__WXMSW__)
      useWxWidgets=true;
    #else  
      if (GetEnvString("DISPLAY").length() > 0) useWxWidgets=true; else useWxWidgets=false;
    #endif
  
#else
  useWxWidgets=false;
#endif
#ifdef _WIN32
  lib::posixpaths = false;
#endif
  for( SizeT a=1; a< argc; ++a)
    {
      if( string( argv[a]) == "--help" | string( argv[a]) == "-h") {
      cerr << "Usage: gdl [ OPTIONS ] [ batch_file ... ]" << endl;
      cerr << "Start the GDL interpreter (incremental compiler)" << endl;
      cerr << endl;
      cerr << "GDL options:" << endl;
      cerr << "  --help (-h)        display this message" << endl;
      cerr << "  --version (-V, -v) show version information" << endl;
      cerr << "  --fakerelease X.y  pretend that !VERSION.RELEASE is X.y" << endl;
      cerr << "  --fussy            implies that procedures adhere with modern IDL, where \"()\" are for functions and \"[]\" are for arrays." << endl;
      cerr << "                     This speeds up (sometimes terribly) compilation but choke on every use of \"()\" with arrays." << endl;
      cerr << "                     Conversion of procedures to modern IDL can be done with D. Landsman's idlv4_to_v5 procedure." << endl;
      cerr << "                     Use enviromnment variable \"GDL_IS_FUSSY\" to set up permanently this feature." << endl;
      cerr << "  --sloppy           Sets the traditional (default) compiling option where \"()\"  can be used both with functions and arrays." << endl;
      cerr << "                     Needed to counteract temporarily the effect of the enviromnment variable \"GDL_IS_FUSSY\"." << endl;
      cerr << "  --MAC              Graphic device will be called 'MAC' on MacOSX. (default: 'X')" << endl;
      cerr << "  [--no-use-wx | -X] Tells GDL not to use WxWidgets graphics and resort to X11 (if available)." << endl;
      cerr << "                     Also enabled by setting the environment variable GDL_DISABLE_WX_PLOTS to a non-null value." << endl;
      cerr << "  --notebook         Force SVG-only device, used only when GDL is a Python Notebook Kernel." << endl;
      cerr << "  --widget-compat    Tells GDL to use a default (rather ugly) fixed pitch font for compatiblity with IDL widgets." << endl;
      cerr << "                     Also enabled by setting the environment variable GDL_WIDGET_COMPAT to a non-null value." << endl;
      cerr << "                     Using this option may render some historical widgets more readable (as they are based on fixed sizes)." << endl;
      cerr << "  --no-dSFMT         Tells GDL not to use double precision SIMD oriented Fast Mersenne Twister(dSFMT) for random doubles." << endl;
      cerr << "                     Also disable by setting the environment variable GDL_NO_DSFMT to a non-null value." << endl;
      cerr << "  --with-eigen-transpose lets GDL use Eigen::transpose and related functions instead of our accelerated transpose function. Normally slower." <<endl;
      cerr << "  --smart-tpool      switch to a mode where the number of threads is adaptive (experimental). Should enable better perfs on many core machines." <<endl;
#ifdef _WIN32
      cerr << "  --posix (Windows only): paths will be posix paths (experimental)." << endl;
#endif
      cerr << endl;
      cerr << "IDL-compatible options:" << endl;
      cerr << "  -arg value tells COMMAND_LINE_ARGS() to report" << endl;
      cerr << "             the following argument (may be specified more than once)" << endl;
      cerr << "  -args ...  tells COMMAND_LINE_ARGS() to report " << endl;
      cerr << "             all following arguments" << endl;
      cerr << "  -e value   execute given statement and exit (last occurrence taken into account only," << endl;
      cerr << "             executed after startup file, may not be specified together with batch files)" << endl;
      cerr << "  -pref=/path/to/params_file  loads the specified preference file" << endl;
      cerr << "  -quiet (--quiet, -q) suppress welcome messages" << endl;
      cerr << endl;
      cerr << "Homepage: https://gnudatalanguage.github.io" << endl;
      return 0;
    }
      else if (string(argv[a])=="--version" | string(argv[a])=="-v" | string(argv[a])=="-V")
	{
	  cerr << "GDL - GNU Data Language, Version " << VERSION << endl;
	  return 0;
	}
      else if( string( argv[a]) == "-arg")
      {
        if (a == argc - 1)
        {
          cerr << "gdl: -arg must be followed by a user argument." << endl;
          return 0;
        } 
        lib::command_line_args.push_back(string(argv[++a]));
      }
      else if( string( argv[a]) == "-args")
      {
        for (int i = a + 1; i < argc; i++) lib::command_line_args.push_back(string(argv[i]));
        break;
      }
      else if (string(argv[a])=="-quiet" | string(argv[a])=="--quiet" | string(argv[a])=="-q") 
	{
	  quiet = true;
	}
      else if (string(argv[a]).find("-pref=") ==0)
	{
	  cerr << "This option is not operational now" << endl;
	  string tmp;
	  tmp=string(argv[a]);
	  string params_file(tmp.begin()+6,tmp.end());
	  //cerr << "(not ready) to be processed file >>" << params_file << "<<" << endl;
	  WordExp(params_file);
	  ifstream file_params;
	  file_params.open(params_file.c_str());
	  if (!file_params.is_open()) {
	    cerr << "Error opening file. File: "<< params_file << endl;
	    cerr << "No such file or directory"<< endl;
	    return 0;
	  }
	  file_params.close();
	}
      else if (string(argv[a]) == "-e")
	{
	  if (a == argc - 1)
	    {
	      cerr << "gdl: -e must be followed by a user argument." << endl;
	      return 0;
	    }
	  statement = string(argv[++a]);
	  statement.append("\n"); // apparently not needed but this way the empty-string case is covered
	  // (e.g. $ gdl -e "")
	}
      else if (
	    string(argv[a]) == "-demo" || 
        string(argv[a]) == "-em" || 
        string(argv[a]) == "-novm" ||
        string(argv[a]) == "-queue" ||
        string(argv[a]) == "-rt" ||
        string(argv[a]) == "-ulicense" ||
        string(argv[a]) == "-vm" 
      )
        cerr << argv[0] << ": " << argv[a] << " option ignored." << endl;
      else if (string(argv[a]) == "-gdlde")
      {
          gdlde = true;
      }
      else if (string(argv[a]) == "--fussy")
      {
          strict_syntax = true;
          syntaxOptionSet = true;
      }
      else if (string(argv[a]) == "--sloppy")
      {
          strict_syntax = false;
          syntaxOptionSet = true;
      }
      else if (string(argv[a]) == "--no-dSFMT")
      {
           useDSFMTAcceleration = false;
      }
      else if (string(argv[a]) == "--widget-compat")
      {
          tryToMimicOriginalWidgets = true;
      }      
#ifdef _WIN32
      else if (string(argv[a]) == "--posix") lib::posixpaths=true;
#endif
      else if (string(argv[a]) == "--MAC")
      {
         usePlatformDeviceName = true;
      }
      else if (string(argv[a]) == "--no-use-wx" |  string(argv[a]) == "-X")
      {
         force_no_wxgraphics = true;
      }
      else if (string(argv[a]) == "--with-eigen-transpose")
      {
         useEigenForTransposeOps = true;
      }
      else if (string(argv[a]) == "--smart-tpool")
      {
         useSmartTpool = true;
      }
      else if (string(argv[a]) == "--notebook")
      {
         iAmANotebook = true;
      }
      else if (string(argv[a]) == "--fakerelease")
      {
        if (a == argc - 1)
          {
            cerr << "gdl: --fakerelease must be followed by a string argument like \"6.4\"" << endl;
            return 0;
          }
        pretendRelease = string(argv[++a]);
      }
      else if (*argv[a] == '-')
      {
        cerr << argv[0] << ": " << argv[a] << " option not recognized." << endl;
        return 0;
      }
      else
      {
        batch_files.push_back(argv[a]);
      }
    }

  if (0&&statement.length() > 0 && batch_files.size() > 0) 
  {
    cerr << argv[0] << ": " << "-e option cannot be specified with batch files" << endl;
    return 0;
  }
  
  //before InitGDL() as InitGDL() starts graphic!
  
#ifdef HAVE_LIBWXWIDGETS
  //tells if wxWidgets is working (may not be the case if DISPLAY is not set) by setting useWxWidgets to false
  if (useWxWidgets) useWxWidgets=GDLWidget::InitWx();
  // default is wx Graphics...
  useWxWidgetsForGraphics=useWxWidgets;
#else
  useWxWidgetsForGraphics=false;
#endif
#ifdef HAVE_X
  // unless we have X and want to see it for plots
  std::string disableWXPlots=GetEnvString("GDL_DISABLE_WX_PLOTS");
  if ( disableWXPlots.length() > 0) useWxWidgetsForGraphics=false; //not necessary "YES".
  if (force_no_wxgraphics) useWxWidgetsForGraphics=false; //this has the last answer, whatever the setup.
#endif  
  std::string doUseUglyFonts=GetEnvString("GDL_WIDGETS_COMPAT");
  if ( doUseUglyFonts.length() > 0) tryToMimicOriginalWidgets=true; 
  
  InitGDL(); 

  // must be after !cpu initialisation
  InitOpenMP(); //will supersede values for CpuTPOOL_NTHREADS

  // instantiate the interpreter
  DInterpreter interpreter;

  if (gdlde || (isatty(0) && !quiet)) {
    StartupMessage();
    cerr << "- Default library routine search path used (GDL_PATH/IDL_PATH env. var. not set): " << gdlPath << endl;
    if (useWxWidgetsForGraphics) cerr << "- Using WxWidgets as graphics library (windows and widgets)." << endl;
    if (useLocalDrivers || driversNotFound) {
      if (driversNotFound) cerr << "- Local drivers not found --- using default ones. " << endl;
      else if (getenv(DrvEnvName)) cerr << "- Using local drivers in " << getenv(DrvEnvName) << endl; //protect against NULL.
    }
  }
  if (useDSFMTAcceleration && (GetEnvString("GDL_NO_DSFMT").length() > 0)) useDSFMTAcceleration=false;
  
  //report in !GDL status struct
  DStructGDL* gdlconfig = SysVar::GDLconfig();
  unsigned  DSFMTTag= gdlconfig->Desc()->TagIndex("GDL_USE_DSFMT");
  (*static_cast<DByteGDL*> (gdlconfig->GetTag(DSFMTTag, 0)))[0]=useDSFMTAcceleration;
  
  //same for use of wxwidgets
  unsigned  useWXTAG= gdlconfig->Desc()->TagIndex("GDL_USE_WX");
  (*static_cast<DByteGDL*> (gdlconfig->GetTag(useWXTAG, 0)))[0]=useWxWidgetsForGraphics;
  
  if (!pretendRelease.empty()) SysVar::SetFakeRelease(pretendRelease);
  //fussyness setup and change if switch at start
  if (syntaxOptionSet) { //take it no matters any env. var.
    if (strict_syntax == true) SetStrict(true);
  } else {
    if (GetEnvString("GDL_IS_FUSSY").size()> 0) SetStrict(true);
  }
  
  
  string startup=GetEnvPathString("GDL_STARTUP");
  if( startup == "") startup=GetEnvPathString("IDL_STARTUP");
  if( startup == "")
    {
      if (gdlde || (isatty(0) && !quiet)) cerr << 
        "- No startup file read (GDL_STARTUP/IDL_STARTUP env. var. not set). " << endl;
    }

  if (gdlde || (isatty(0) && !quiet))
  {
    cerr << "- Please report bugs, feature or help requests and patches at: https://github.com/gnudatalanguage/gdl" << endl << endl;
  }
//   else
//     {
//       // if path not given, add users home
//       if( !PathGiven(startup))
// 	{
// 	  string home=GetEnvString("HOME");
// 	  if( home != "") 
// 	    {
// 	      AppendIfNeeded(home,"/");
// 	      startup=home+startup;
// 	    }
// 	}
//     }

#ifdef USE_MPI
  {
    // warning the user if MPI changes the working directory of GDL
    char wd1[PATH_MAX], wd2[PATH_MAX];
    char *wd1p, *wd2p;
    wd1p = getcwd(wd1, PATH_MAX);
    MPI_Init(&argc, &argv);
    wd2p = getcwd(wd2, PATH_MAX);
    if (strcmp(wd1, wd2) != 0)
      cerr << "Warning: MPI has changed the working directory of GDL!" << endl;
  }
  int myrank = 0;
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
  int size; 
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int tag = 0;
  char* mpi_procedure = getenv("GDL_MPI");
  if (myrank == 0 && mpi_procedure != NULL){
    for( SizeT i = 0; i < size; i++)
      MPI_Send(mpi_procedure, strlen(mpi_procedure)+1, MPI_CHAR, i, 
	       tag, MPI_COMM_WORLD);
  }
#endif // USE_MPI

  interpreter.InterpreterLoop( startup, batch_files, statement);

  return 0;
}
