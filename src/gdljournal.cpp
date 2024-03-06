/***************************************************************************
                          gdljournal.cpp  -  exception handling
                             -------------------
    begin                : September 26 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef GDL_JOURNAL_C
#define GDL_JOURNAL_C

#include "includefirst.hpp"
#include "version.hpp"

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include <fstream>
#include <sys/stat.h>

#include "envt.hpp"
#include "dinterpreter.hpp"
#include "gdljournal.hpp"
#include "io.hpp"
#include "str.hpp"
#include "initsysvar.hpp"
#include "basic_pro.hpp"
#include "file.hpp"

namespace lib
{
  using namespace std;

  const int    MAX_DATE_STRING_LENGTH = 80;
  const char*  JOURNALCOMMENT = ";";

  static GDLStream *gdljournal;

  void journal(EnvT *e) {
    /*case 1:
      journal closed
      1a:no argument
      open journal with default filename 'gdljournal.pro'
      1b:1 argument
      open journal with filename from argument
		
      case 2:
      journal open
      2a:no argument
      close journal
      2b:1 argument
      print argument to journal
     */

    //setup
    SizeT nParam = e->NParam();

    if (SysVar::JournalLUN() == 0) {

      string gdljournal_filename;
      //case 1
      if (nParam == 0) {
        //case 1a = default open
        gdljournal_filename = "gdljournal.pro";
      } else if (nParam == 1) {
        //case 1b: open with filename
        e->AssureStringScalarPar(0, gdljournal_filename);
      }

      DLong jLUN = GetLUN();
      if (jLUN == 0)
        e->Throw("All available logical units are currently in use.");

      SysVar::JournalLUN(jLUN);

      gdljournal = &fileUnits[ jLUN - 1];
      // insure gdljournal_filename ican be opened
      try {
        gdljournal->Open(gdljournal_filename, fstream::out, false, false, false,
          defaultStreamWidth, false, false);
      } catch (...) {
        if (gdljournal != NULL) {
          gdljournal->Close();
          gdljournal->Free();
          gdljournal = NULL;
          SysVar::JournalLUN(0);
          throw GDLException("JOURNAL: Error opening file. Unit:"+i2s(jLUN)+", File: "+gdljournal_filename+"\n  No such file or directory.");
        }
      }
      //message  
      DStructGDL* version = SysVar::Version();

      /*      static unsigned releaseTag = version->Desc()->TagIndex( "RELEASE");*/
      static unsigned osTag = version->Desc()->TagIndex("OS");
      static unsigned archTag = version->Desc()->TagIndex("ARCH");
      static unsigned mTag = version->Desc()->TagIndex("MEMORY_BITS");
      /*      DString release = 
        (*static_cast<DStringGDL*>( version->GetTag( releaseTag, 0)))[0];*/
      DString os =
        (*static_cast<DStringGDL*> (version->GetTag(osTag, 0)))[0];
      DString arch =
        (*static_cast<DStringGDL*> (version->GetTag(archTag, 0)))[0];
      DInt m =
        (*static_cast<DIntGDL*> (version->GetTag(mTag, 0)))[0];

      write_journal_comment(string("GDL Version ") + VERSION/*release*/ + " (" + os + " " + arch + " m" +
        i2s(m) + ")");

      string user = GetEnvString("USER");

      // depending the system, HOST variable is not always set up.
      int debug = 0;
      string host = GetEnvString("HOST");
      if (debug) cout << "HOST: " << host << ".." << endl;

      if (host == "") {
        host = GetEnvString("HOSTNAME");
        if (debug) cout << "HOSTNAME: " << host << endl;
      }
      if (host == "") {
        //char *gethost;
#define GDL_HOST_NAME_MAX 255
        char gethost[GDL_HOST_NAME_MAX];
        size_t lgethost = GDL_HOST_NAME_MAX;
        // don't know if this primitive is available on Mac OS X
        int success = gethostname(gethost, lgethost);
        if (success != 0)
          // we are here only if all 3 methods failed
        {
          e->Throw("Unknown hostname !");
        } else {
          host = string(gethost);
          //if (debug) cout << "lgethost: [" << lgethost << "]"<< endl;
          //if (debug) cout << "1GETHOSTNAME: [" << gethost << "]"<< endl;
          if (debug) cout << "GETHOSTNAME: [" << host.c_str() << "]" << endl;
        }
      }

      write_journal_comment("Journal File for " + user + "@" + host);

      DString cur = GetCWD();

      write_journal_comment("Working directory: " + cur);

      time_t t = time(0);
      struct tm * tstruct;
      tstruct = localtime(&t);
      char *st = new char[MAX_DATE_STRING_LENGTH];
      const char *format = "%a %h %d %T %Y"; // day,month,day number,time,year
      SizeT res = strftime(st, MAX_DATE_STRING_LENGTH, format, tstruct);

      if (res != 0)
        write_journal_comment(string("Date: ") + st + string("\n"));
      delete[] st;

      // clear input line (should not be put into the journal file
      GDLInterpreter* ip = e->Interpreter();
      ip->GetClearActualLine();

    } else {
      //case 2
      if (nParam == 0) {
        //case 2a = close file
        if (gdljournal != NULL) {
          gdljournal->Close();
          gdljournal = NULL;
          SysVar::JournalLUN(0);
        }
      } else if (nParam == 1) {
        //case 2b = print arg to journal

        // tricky because WITHOUT ';' this time
        // solution: fake writing to normal file

        BaseGDL* outPar = e->GetParDefined(0);

        GDLStream* gdljournalSav = gdljournal;
        gdljournal = NULL;

        try {
          SizeT actPos = 0;
          outPar->ToStream(gdljournalSav->OStream(),
            gdljournalSav->Width(),
            &actPos);
          gdljournal = gdljournalSav;
        } catch (...) {
          gdljournal = gdljournalSav;
          throw;
        }
      }
    }
  }
  
  GDLStream* get_journal()
  {
    return gdljournal;
  }
	
  // used only with GetClearActualLine, which already contains a trailinf '\n'
  void write_journal(const std::string &str)
  {
    if(gdljournal == NULL) return;
    gdljournal->OStream() << str;
  }
	
  void write_journal_comment(const std::string &str)
  {
    if(gdljournal == NULL) return;
    gdljournal->OStream()  << JOURNALCOMMENT << " " << str << "\n";
  }
	
  void write_journal_comment(EnvT *e, int offset, SizeT width)
  {
    if(gdljournal == NULL) return;
    if(e->NParam() == 0) return;
    print_os( &gdljournal->OStream(), e, offset, width);
  }	
}
#endif
