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

//#include <fenv.h>

#include "str.hpp"
#include "dinterpreter.hpp"
#include "terminfo.hpp"
#include "sigfpehandler.hpp"
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

//initialize wxWidgets system
#ifdef HAVE_LIBWXWIDGETS
#include "gdlwidget.hpp"
#ifndef __WXMAC__ 
wxIMPLEMENT_APP_NO_MAIN( wxAppGDL);
#else
wxIMPLEMENT_APP_NO_MAIN( wxApp);
#endif
#endif

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
#define GDL_PREFERED_STACKSIZE 20480000 //20000*1024 OK for the time being
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

  // turn on all floating point exceptions
  //  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW );

  signal(SIGINT,ControlCHandler);
  signal(SIGFPE,SigFPEHandler);
  
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

int main(int argc, char *argv[])
{
#if GDL_DEBUG
  if( atexit( AtExit) != 0) cerr << "atexit registration failed." << endl;
#endif
  // indicates if the user wants to see the welcome message
  bool quiet = false;
  bool gdlde = false;
  
#ifdef INSTALL_LOCAL_DRIVERS
  // We must declare here (and not later) where our local copy of (customized?) drivers is to be found.
  std::string drvPath="PLPLOT_DRV_DIR=" GDLDATADIR "/drivers";
  char s[256];
  strcpy(s,drvPath.c_str());
  putenv(s);
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
  forceWxWidgetsUglyFonts = false;
  useDSFMTAcceleration = true;
  iAmANotebook=false; //option --notebook
 #ifdef HAVE_LIBWXWIDGETS 
  useWxWidgets=true;
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
      cerr << "  --no-use-wx        Tells GDL not to use WxWidgets graphics." << endl;
      cerr << "                     Also enabled by setting the environment variable GDL_DISABLE_WX_PLOTS to a non-null value." << endl;
      cerr << "  --notebook         Force SVG-only device, used only when GDL is a Python Notebook Kernel." << endl;
      cerr << "  --widget-compat    Tells GDL to use a default (rather ugly) fixed pitch font for compatiblity with IDL widgets." << endl;
      cerr << "                     Also enabled by setting the environment variable GDL_WIDGET_COMPAT to a non-null value." << endl;
      cerr << "                     Using this option may render some historical widgets unworkable (as they are based on fixed sizes)." << endl;
      cerr << "  --no-dSFMT         Tells GDL not to use double precision SIMD oriented Fast Mersenne Twister(dSFMT) for random doubles." << endl;
      cerr << "                     Also disable by setting the environment variable GDL_NO_DSFMT to a non-null value." << endl;
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
          forceWxWidgetsUglyFonts = true;
      }      
#ifdef _WIN32
      else if (string(argv[a]) == "--posix") lib::posixpaths=true;
#endif
      else if (string(argv[a]) == "--MAC")
      {
         usePlatformDeviceName = true;
      }
      else if (string(argv[a]) == "--no-use-wx")
      {
         force_no_wxgraphics = true;
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
  useWxWidgets=GDLWidget::InitWx();
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
  if ( doUseUglyFonts.length() > 0) forceWxWidgetsUglyFonts=true; 
  
  InitGDL(); 

  // must be after !cpu initialisation
  InitOpenMP(); //will supersede values for CpuTPOOL_NTHREADS

  if (gdlde || (isatty(0) && !quiet)) StartupMessage();

  // instantiate the interpreter
  DInterpreter interpreter;

  string gdlPath=GetEnvString("GDL_PATH");
  if( gdlPath == "") gdlPath=GetEnvString("IDL_PATH");
  if( gdlPath == "")
    {
      gdlPath = "+" GDLDATADIR "/lib";
      if (gdlde || (isatty(0) && !quiet)) cerr <<
        "- Default library routine search path used (GDL_PATH/IDL_PATH env. var. not set): " GDLDATADIR "/lib" << endl;
    }
  if (useWxWidgetsForGraphics) {
      if (gdlde || (isatty(0) && !quiet)) cerr << "- Using WxWidgets as graphics library (windows and widgets)." << endl;
    }

  
  if (useDSFMTAcceleration && (GetEnvString("GDL_NO_DSFMT").length() > 0)) useDSFMTAcceleration=false;
  
  //report in !GDL status struct
  DStructGDL* gdlconfig = SysVar::GDLconfig();
  unsigned  DSFMTTag= gdlconfig->Desc()->TagIndex("GDL_USE_DSFMT");
  (*static_cast<DByteGDL*> (gdlconfig->GetTag(DSFMTTag, 0)))[0]=useDSFMTAcceleration;
  
  //same for use of wxwidgets
  unsigned  useWXTAG= gdlconfig->Desc()->TagIndex("GDL_USE_WX");
  (*static_cast<DByteGDL*> (gdlconfig->GetTag(useWXTAG, 0)))[0]=useWxWidgetsForGraphics;
  
  SysVar::SetGDLPath( gdlPath); //probably duplicate with the one in initobjects!
  
  if (!pretendRelease.empty()) SysVar::SetFakeRelease(pretendRelease);
  //fussyness setup and change if switch at start
  if (syntaxOptionSet) { //take it no matters any env. var.
    if (strict_syntax == true) SetStrict(true);
  } else {
    if (GetEnvString("GDL_IS_FUSSY").size()> 0) SetStrict(true);
  }
  
  
  string startup=GetEnvString("GDL_STARTUP");
  if( startup == "") startup=GetEnvString("IDL_STARTUP");
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
