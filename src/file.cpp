/***************************************************************************
                                 file.cpp  -  file related library functions 
                             -------------------
    begin                : July 22 2004
    copyright            : (C) 2004 by Marc Schellens
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

#ifndef _MSC_VER
#	include <libgen.h>
#	include <sys/types.h>
#endif

#include <sys/stat.h>

#ifndef _MSC_VER
#	include <unistd.h> 
#endif

#include "basegdl.hpp"
#include "str.hpp"


#include "envt.hpp"
#include "file.hpp"
#include "objects.hpp"

#include <climits> // PATH_MAX

//#ifndef _MSC_VER
#ifndef _WIN32
#	include <fnmatch.h>
#	include <glob.h> // glob in MinGW ok for mingw >=3.21 11/2014
#else
#	include <shlwapi.h>
#       include <windows.h>
#    if !defined(S_IFLNK)
#	define S_IFLNK 0xA000
#	define S_ISLNK(mode) (((mode) & S_IFLNK) == S_IFLNK)
#    endif
#endif

#ifndef _MSC_VER
#	include <dirent.h>
#else
#	include <io.h>

#	define access _access

#	define R_OK    4       /* Test for read permission.  */
#	define W_OK    2       /* Test for write permission.  */
//#	define   X_OK    1       /* execute permission - unsupported in windows*/
#	define F_OK    0       /* Test for existence.  */

#	define PATH_MAX 255  //should be _MAX_PATH no? (as in str.cpp) (GD)

#	include <direct.h>


#	if !defined(S_ISDIR)

#	define __S_ISTYPE(mode, mask)	(((mode) & S_IFMT) == (mask))
#	define S_ISDIR(mode)	 __S_ISTYPE((mode), S_IFDIR)
#	define S_ISREG(mode)    __S_ISTYPE((mode), S_IFREG)

#endif


#endif

// workaround for HP-UX. A better solution is needed i think
//#if defined(__hpux__) || defined(__sun__)
#if !defined(GLOB_TILDE)
#  define GLOB_TILDE 0
#endif
#if !defined(GLOB_BRACE)
#  define GLOB_BRACE 0
#endif
//#if defined(__hpux__) || defined(__sun__) || defined(__CYGWIN__) || defined(__OpenBSD__)
#if !defined(GLOB_ONLYDIR)
#  define GLOB_ONLYDIR 0
#endif
#if !defined(GLOB_PERIOD)
#  define GLOB_PERIOD 0
#endif

#ifdef _MSC_VER

/*
  Implementation of POSIX directory browsing functions and types for Win32.

  Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
  History: Created March 1997. Updated June 2003 and July 2012.
  Rights:  See end of file.
*/

#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno iff they return -1 */
#include <stdlib.h>
#include <string.h>

struct dirent
{
  char *d_name;
};

#ifdef __cplusplus
extern "C"
{
#endif

  typedef ptrdiff_t handle_type; /* C99's intptr_t not sufficiently portable */

  struct DIR
  {
    handle_type         handle; /* -1 for failed rewind */
    struct _finddata_t  info;
    struct dirent       result; /* d_name null iff first time */
    char                *name;  /* null-terminated char string */
  };


  DIR *opendir(const char *name)
  {
    DIR *dir = 0;

    if(name && name[0])
      {
        size_t base_length = strlen(name);
        const char *all = /* search pattern must end with suitable wildcard */
	  strchr("/\\", name[base_length - 1]) ? "*" : "/*";

        if((dir = (DIR *) malloc(sizeof *dir)) != 0 &&
           (dir->name = (char *) malloc(base_length + strlen(all) + 1)) != 0)
	  {
            strcat(strcpy(dir->name, name), all);

            if((dir->handle =
                (handle_type) _findfirst(dir->name, &dir->info)) != -1)
	      {
                dir->result.d_name = 0;
	      }
            else /* rollback */
	      {
                free(dir->name);
                free(dir);
                dir = 0;
	      }
	  }
        else /* rollback */
	  {
            free(dir);
            dir   = 0;
            errno = ENOMEM;
	  }
      }
    else
      {
        errno = EINVAL;
      }

    return dir;
  }

  int closedir(DIR *dir)
  {
    int result = -1;

    if(dir)
      {
        if(dir->handle != -1)
	  {
            result = _findclose(dir->handle);
	  }
        free(dir->name);
        free(dir);
      }
    if(result == -1) /* map all errors to EBADF */
      {
        errno = EBADF;
      }
    return result;
  }

  struct dirent *readdir(DIR *dir)
  {
    struct dirent *result = 0;

    if(dir && dir->handle != -1)
      {
        if(!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
	  {
            result         = &dir->result;
            result->d_name = dir->info.name;
	  }
      }
    else
      {
        errno = EBADF;
      }
    return result;
  }

  void rewinddir(DIR *dir)
  {
    if(dir && dir->handle != -1)
      {
        _findclose(dir->handle);
        dir->handle = (handle_type) _findfirst(dir->name, &dir->info);
        dir->result.d_name = 0;
      }
    else
      {
        errno = EBADF;
      }
  }

#ifdef __cplusplus
}
#endif
/*
  Copyright Kevlin Henney, 1997, 2003, 2012. All rights reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose is hereby granted without fee, provided
  that this copyright and permissions notice appear in all copies and
  derivatives.
  This software is supplied "as is" without express or implied warranty.
  But that said, if there are any problems please get in touch.
*/
#endif
#define NTEST_SEARCH 7
#if defined(__CYGWIN__) || defined(__FreeBSD__)
#define stat64 stat
#define lstat64 lstat
// for religious reasons, CYGWIN doesn't do lstat64
// FreeBSD doesn't do lstat64 because there is no need for it
#endif

namespace lib {

  using namespace std;
#ifdef _WIN32
  //
#ifdef _MSC_VER // MSVC uses 64bit internally
#   define stat64 stat
#   define lstat64(x,y) stat(x,y) 
#else
      // Patch by Greg Jung: Using _stati64 is acceptable down to winXP version and will
      // result in a 64-bit st_size for both mingw-org and for mingw-w64.
      // The times st_atime, st_mtime, etc. will be 32-bit in mingw-org.
      #ifndef stat64 /* case of mingw-org .vs. mingw-w64 */
      # define stat64 _stati64
      #endif
#    define lstat64(x,y) stat64(x,y)
#endif

  // fstat_win32 used for symlink treatment
  //
  void fstat_win32(const DString& DSpath, int& st_mode, DWORD &dwattrib)
  {
    DWORD      reparsetag;
    WCHAR	filepath[MAX_PATH+1];
    HANDLE	hFind;
    BOOL	foundnext;
    WIN32_FIND_DATAW FindFileData;
    // Native NTFS symlinks (not Junctions) are found
    // also cygwin symlinks are sniffed out.

    MultiByteToWideChar(CP_UTF8, 0,
			(LPCSTR) DSpath.c_str(), -1,
			filepath, MAX_PATH+1);
    hFind = FindFirstFileExW( filepath,
			      FindExInfoStandard,
			      &FindFileData,
			      FindExSearchNameMatch, NULL, 0);
    if (hFind == INVALID_HANDLE_VALUE) {
      FindClose(hFind);
      return;
    }
    dwattrib = FindFileData.dwFileAttributes;
    if( (dwattrib & FILE_ATTRIBUTE_REPARSE_POINT) != 0 )  {
      reparsetag = FindFileData.dwReserved0;
      if(reparsetag ==  IO_REPARSE_TAG_SYMLINK)
	st_mode |= S_IFLNK;
    } else
      if ( (dwattrib & FILE_ATTRIBUTE_SYSTEM) != 0 ) {
	// This is the first step to a cygwin symlink.
	// next, '!<symlink>' is first 10 bytes.
	// following this, 0xFF 0xFE <target name in UTF-8>
	// for now, just assume it is a link.

        HANDLE hFile = CreateFileW( filepath,
				    FILE_GENERIC_READ,
				    FILE_SHARE_READ | FILE_SHARE_WRITE,         0,
				    OPEN_EXISTING,
				    FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_RANDOM_ACCESS,
				    0);

        if (hFile == INVALID_HANDLE_VALUE) {
	  CloseHandle(hFile);
	  // happens for "hidden" files or protected files:
	  //              cout << " stat_win32: could not open " + DSpath << endl;
	  return;
	}
        char header[12];
        DWORD nbytesread;

        BOOL result = ReadFile(hFile, header, 12,
                               &nbytesread, 0 );
        if (strncmp( header, "!<symlink>",10) == 0 ) st_mode |= S_IFLNK;
	CloseHandle(hFile);
      }
    FindClose(hFind);
    return;
  }

#endif

  string PathSeparator()
  {
#ifdef _WIN32
    return string ("\\");
#else
    return string ("/");
#endif

  }

  DString GetCWD()
  {
    SizeT bufSize = PATH_MAX;
    char *buf = new char[ bufSize];
    for(;;)
      {
	char* value = getcwd( buf, bufSize);
	if( value != NULL)
	  break;
	delete[] buf;
	if( bufSize > 32000) 
	  throw GDLException("Cannot get CWD.");
	bufSize += PATH_MAX;
	buf = new char[ bufSize];
      }

    DString cur( buf);
    delete[] buf;
    
    return cur;
  }

  void cd_pro( EnvT* e)
  {
    if( e->KeywordPresent( 0)) // CURRENT
      {
	DString cur = GetCWD();
	e->SetKW( 0, new DStringGDL( cur));
      }

    SizeT nParam=e->NParam(); 
    if( nParam == 0) return;
    
    DString dir;
    e->AssureScalarPar<DStringGDL>( 0, dir);
   
    WordExp( dir);

     
    //     // expand tilde
#ifdef _WIN32
    if( dir[0] == '~')
      {
 	char* homeDir = getenv( "HOME");

 	if( homeDir != NULL)
	  dir = string( homeDir) + "/" + dir.substr(1);
      }
#endif

    int success = chdir( dir.c_str());
 
    if( success != 0)
      e->Throw( "Unable to change current directory to: "+dir+".");
  }

  bool FindInDir( const DString& dirN, const DString& pat)
  {
    DString root = dirN;
    AppendIfNeeded( root, "/");

    DIR* dir = opendir( dirN.c_str());
    if( dir == NULL) return false;

    struct stat64    statStruct;
#ifdef _WIN32

    // JP Mar 2015: Below code block is inspired by Greg's code to improve speed

    wchar_t entryWStr[PATH_MAX+1] = {0,};
    wchar_t patW[PATH_MAX+1] = {0,};
    MultiByteToWideChar(CP_UTF8, 0, pat.c_str(), -1, patW, MAX_PATH+1);
#endif

    for(;;)
      {
	struct dirent* entry = readdir( dir);
	if( entry == NULL) break;
	
	DString entryStr( entry->d_name);
	if( entryStr != "." && entryStr != "..")
	  {
	    DString testFile = root + entryStr;

	    int actStat = lstat64( testFile.c_str(), &statStruct);

	    if( S_ISDIR(statStruct.st_mode) == 0)

	      { // only test non-dirs

#ifdef _WIN32
		MultiByteToWideChar(CP_UTF8, 0, entryStr.c_str(), -1, entryWStr, MAX_PATH+1);
		int match = !PathMatchSpecW( entryWStr, patW );
#else
		int match = fnmatch( pat.c_str(), entryStr.c_str(), 0);
#endif
		if( match == 0)
		  {
		    closedir( dir);
		    return true;
		  }
	      }
	  }
      }

    closedir( dir);
    return false;
  }
  
  void ExpandPathN( FileListT& result,
		    const DString& dirN, 
		    const DString& pat,
		    bool all_dirs ) {
    // expand "+"
 
    int fnFlags = 0;

    //    fnFlags |= FNM_PERIOD;
    //    fnFlags |= FNM_NOESCAPE;

    DString root = dirN;
    AppendIfNeeded( root, "/");
    
    struct stat64    statStruct;

    FileListT recurDir;
    
    bool notAdded = !all_dirs;

    DIR* dir = opendir( dirN.c_str());
 
    if( dir == NULL) return;
    int debug=0;
    if ( debug ) cout << "ExpandPathN: " << dirN << endl;

    // JP Mar 2015: Below code block is inspired by Greg's code to improve speed
#if defined (_WIN32)
    wchar_t entryWStr[PATH_MAX+1] = {0,};
    wchar_t patW[PATH_MAX+1] = {0,};

    MultiByteToWideChar(CP_UTF8, 0, pat.c_str(), -1, patW, PATH_MAX+1);
#endif
    for (;; ) {
      struct dirent* entry = readdir( dir);
      if( entry == NULL) break;

      DString entryStr( entry->d_name);
      if ( entryStr != "." && entryStr != ".." ) {
	DString testDir = root + entryStr;

        int actStat = lstat64(testDir.c_str(), &statStruct);
#ifdef _WIN32
	DWORD dwattrib;
        int addlink = 0;
        fstat_win32(testDir, addlink, dwattrib);
        statStruct.st_mode |= addlink;
#endif
        bool isASymLink = S_ISLNK(statStruct.st_mode);
	//
	//   follow for expand_path, as per documented behavior of IDL
	//
	if(isASymLink) actStat = stat64(testDir.c_str(), &statStruct);
        if( S_ISDIR(statStruct.st_mode) != 0) {
	  if( debug && isASymLink ) cout << " following a symlink directory: " << testDir << endl;
          recurDir.push_back( testDir);
	}     else if( notAdded)
	  {
#ifdef _WIN32
            MultiByteToWideChar(CP_UTF8, 0, entryStr.c_str(),
				-1, entryWStr, PATH_MAX+1);
            int match = !PathMatchSpecW(entryWStr, patW);

#else
	    int match = fnmatch( pat.c_str(), entryStr.c_str(), 0);
#endif
	    if( match == 0)
	      notAdded = false;
	  }
      }
    }

    int c = closedir( dir);
    if( c == -1) return;

    // recursive search
    SizeT nRecur = recurDir.size();
    for ( SizeT d = 0; d < nRecur; ++d ) {
      ExpandPathN( result, recurDir[d], pat, all_dirs); 
    }

    if( !notAdded)
      result.push_back( dirN);
  }

  void ExpandPath( FileListT& result,
		   const DString& dirN, 
		   const DString& pat,
		   bool all_dirs)
  {
    int debug=0;
    if(debug) 	cout << " ExpandPath(,dirN.pat,bool) " << dirN << endl;
    if( dirN == "") 
      return;

    if( StrUpCase( dirN) == "<GDL_DEFAULT>" ||
	StrUpCase( dirN) == "<IDL_DEFAULT>")
      {
	// result.push_back( the default path here);
	return;
      }
    
    if( dirN[0] != '+' && dirN[0] != '~')
      {
	result.push_back( dirN);
	return;
      }
    
    if( dirN.length() == 1) {
      // dirN == "+" 
      if (dirN[0] == '+') return;
    }

    // dirN == "+DIRNAME"

#ifdef _WIN32
    DString initDir = dirN;
    if(dirN[0] == '+') 
      initDir = dirN.substr(1);
        
#else

    // do first a glob because of '~'

    int flags = GLOB_TILDE | GLOB_NOSORT;

    glob_t p;

    int offset_tilde=0;
    if (dirN[0] == '+') offset_tilde=1;
    int gRes = glob( dirN.substr(offset_tilde).c_str(), flags, NULL, &p);
    if( gRes != 0 || p.gl_pathc == 0)
      {
	globfree( &p);
	return;
      }



    DString initDir = p.gl_pathv[ 0];
    globfree( &p);

#endif
    //   cout << "ExpandPath: initDir:" << initDir << dirN << endl;
    if (dirN[0] == '+')
      { 
	ExpandPathN( result, initDir, pat, all_dirs);
      } 
    else
      {
	result.push_back(initDir);
      }

  }

  BaseGDL* expand_path( EnvT* e)
  {
    e->NParam( 1);

    DString s;
    e->AssureStringScalarPar( 0, s);

    FileListT sArr;
    

    static int all_dirsIx = e->KeywordIx( "ALL_DIRS");
    bool all_dirs = e->KeywordSet( all_dirsIx);

    static int arrayIx = e->KeywordIx( "ARRAY");
    bool array = e->KeywordSet( arrayIx);

    static int countIx = e->KeywordIx( "COUNT");

    DString pattern;
    static int typeIx = e->KeywordIx( "PATTERN");
    if(e->KeywordPresent(typeIx)) {
      e->AssureStringScalarKWIfPresent( typeIx, pattern);
    }
    else      pattern = "*.pro";

    SizeT d;
    long   sPos=0;
#ifdef _WIN32
    char pathsep[]=";";
#else
    char pathsep[]=":";
#endif
    do
      {
	d=s.find(pathsep[0],sPos);
	string act = s.substr(sPos,d-sPos);
	
	ExpandPath( sArr, act, pattern, all_dirs);
	
	sPos=d+1;
      }
    while( d != s.npos);

    SizeT nArr = sArr.size();

    if( e->KeywordPresent( countIx)) 
      {
	e->SetKW( countIx, new DLongGDL( nArr));
      }

    if( nArr == 0)
      return new DStringGDL( "");

    if( array)
      {
	DStringGDL* res = new DStringGDL( dimension( nArr), BaseGDL::NOZERO);
	for( SizeT i=0; i<nArr; ++i)
	  (*res)[ i] = sArr[i];
	return res;
      }

    // set the path
    DString cat = sArr[0];
    for( SizeT i=1; i<nArr; ++i)
      cat += pathsep + sArr[i];
    return new DStringGDL( cat);
  }

#ifdef _WIN32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH) 
  // ref:http://sourceforge.net/p/mingw/patches/256/ Keith Marshall 2005-12-02
  // http://msdn.microsoft.com/en-us/library/506720ff.aspx
#endif

  void PatternSearch( FileListT& fL, const DString& dirN, const DString& pat,
		      bool accErr,
		      bool quote,      bool match_dot, bool  forceAbsPath, bool fold_case,
		      bool onlyDir,
                      bool *tests,  bool recursive)
  {
    enum { testregular=3, testdir, testzero, testsymlink };
    bool dotest = false;
    for( SizeT i=0; i < NTEST_SEARCH; i++) dotest |= tests[i];
    int fnFlags = 0;

#ifndef _WIN32

    if( !match_dot)
      fnFlags |= FNM_PERIOD;

    if( !quote)
      fnFlags |= FNM_NOESCAPE;

    if(fold_case) fnFlags |= FNM_CASEFOLD;

#endif


    DString root = dirN;
    if( root != "")
      {
	long endR; 
	for( endR = root.length()-1; endR >= 0; --endR)
	  {
	    if( root[ endR] != '/')
	      break;
	  }
	if( endR >= 0)
	  root = root.substr( 0, endR+1) + "/";
	else
	  root = "/";
	// Include a provision for %HOME%:
#ifdef _WIN32 
	if( root[0] == '~') { 
	  char* homeDir = getenv( "HOME");
	  if( homeDir != NULL)
 	    root = string( homeDir) + "/" + root.substr(1);
	}
#endif

      }
    DString prefix="";
    if( root== "") prefix=GetCWD()+"/";
    int debug=0;
    if(debug) {
      cout << " PatternSearch: DirN='"<<dirN<<"', root='"<<root<<"', :"<<pat<<" onlyDir?"<<onlyDir<<endl;
    }
    FileListT recurDir;
    
    DIR* dir;
    if( root != "")
      dir = opendir( dirN.c_str());
    else
      dir = opendir( ".");
    if( dir == NULL) {
      if( accErr)
	throw GDLException( "FILE_SEARCH: Error opening dir: "+dirN);
      else
	return;
    }


    int accessmode = 0;
    if( tests[0]) accessmode = R_OK;
    if( tests[1]) accessmode |= W_OK;
#ifndef _WIN32
    if( tests[2]) accessmode |= X_OK;
#else

    wchar_t patW[MAX_PATH+1];
    wchar_t entryWstr[MAX_PATH+1];
    patW[1]=0;
    if(pat == "") patW[0] = '*'; else
      MultiByteToWideChar(CP_UTF8, 0,
			  (LPCSTR)pat.c_str(), -1,
			  patW, MAX_PATH+1);
#endif

    struct stat64    statStruct, statlink;
    for(;;)
      {
	struct dirent* entry = readdir( dir);
	if( entry == NULL)
	  break;

	DString entryStr( entry->d_name);
	if( entryStr != "." && entryStr != "..")
	  {

	    DString testDir = root + entryStr;

	    int actStat = lstat64( testDir.c_str(), &statStruct);
	    if(onlyDir && (S_ISDIR(statStruct.st_mode) == 0) ) continue;

	    //#if 0 block that I don't know if it can't go ...
	    if( root != "") // dirs for current ("") already included
	      {
		if( S_ISDIR(statStruct.st_mode) != 0) {		      
		  // (symlinked directories are not followed)
#ifdef _WIN32
		  //  This is probably superfluous.  A directory wont show as symlink
		  DWORD dwattrib;
		  int addlink = 0;
		  fstat_win32(testDir, addlink, dwattrib);
		  statStruct.st_mode |= addlink;
#endif
		  // again, probably superfluous
		  if( S_ISLNK(statStruct.st_mode) == 0) recurDir.push_back( testDir);
		}
	      }
	    // dirs are also returned if they match
	    //#endif block that I don't know if it can't go ...



#ifdef _WIN32
	    MultiByteToWideChar(CP_UTF8, 0,
                                (LPCSTR)entryStr.c_str(), -1,
                                entryWstr, MAX_PATH+1);
	    int match = !PathMatchSpecW(entryWstr, patW);
#else
	    int match = fnmatch( pat.c_str(), entryStr.c_str(), fnFlags);
#endif
 
            if( match == 0) {
	      if(dotest != 0) {
		if( tests[testregular] &&  // only take regulars (excludes dirs, sym)
		    (S_ISREG( statStruct.st_mode) == 0)) continue;
#ifdef _WIN32
		DWORD dwattrib;
		int addlink = 0;
		fstat_win32(testDir, addlink, dwattrib);
		statStruct.st_mode |= addlink;
#endif

		bool isaDir = (S_ISDIR(statStruct.st_mode) != 0);
		bool isASymLink = (S_ISLNK(statStruct.st_mode) != 0);
		if(isASymLink) {
		  actStat = stat64( testDir.c_str(), &statlink);
		  statStruct.st_mode |= statlink.st_mode;
		  isaDir = (S_ISDIR(statlink.st_mode) != 0);
		}   
		if( tests[testdir] && !isaDir) continue;
		if( tests[testsymlink] && !isASymLink) continue;


		if( tests[testzero] &&
		    (statStruct.st_size != 0)) continue;
		// now read, write, execute:
		if(accessmode != 0)
		  if(access(entryStr.c_str(), accessmode) != 0 ) continue;
	      }
	      if(forceAbsPath && !onlyDir) {
		char *symlinkpath =const_cast<char*> (testDir.c_str());
		char actualpath [PATH_MAX+1];
		char *ptr;
		ptr = realpath(symlinkpath, actualpath);
		if( ptr != NULL ) fL.push_back( string(ptr));
		else cout << " Failed to convert "+testDir+" to actualpath!!"<< endl;
	      }
	      else
		fL.push_back( root + entryStr);
	      if(debug && onlyDir) cout << " onlyDir=T:"<<testDir;
            }
	  }
      }
    if(debug && onlyDir) cout << " ###\\n"<<endl;
    int c = closedir( dir);
    if( c == -1) {
      if( accErr)
	throw GDLException( "FILE_SEARCH: Error closing dir: "+dirN);
      else
	return;
    }
    // recursive search
    if( !recursive ) return;
    SizeT nRecur = recurDir.size();
    if(debug) if (nRecur > 0) {
	cout << " Pdebug mode, #recursive="<<nRecur<<" see only first 2:"<<endl;
      }
    for( SizeT d=0; d<nRecur; ++d)
      {
	if(debug && (d<=2)) cout << " do now "+recurDir[d]<<endl;
	PatternSearch( fL, recurDir[d], pat, accErr, quote, 
		       match_dot,  forceAbsPath, fold_case,
		       onlyDir,     tests, true);
      }
    if(debug && (nRecur > 0) ) cout <<"End PatternSearch recursion section"<< endl;
  }

  // Make s string case-insensitive for glob()
  DString makeInsensitive(const DString &s)
  {
    DString insen="";
    char coupleBracket[5]={'[',0,0,']',0};
    char couple[3]={0};
    bool bracket=false;
	
    for(size_t i=0;i<s.size();i++) 
      if((s[i]>='A' && s[i]<='Z') || (s[i]>='a' && s[i]<='z'))
	{
	  char m,M;
	  if(s[i]>='a' && s[i]<='z')
	    m=s[i],M=m+'A'-'a';
	  else
	    M=s[i],m=M-'A'+'a';

	  if(bracket) // If bracket is open, then don't add bracket
	    couple[0]=m,couple[1]=M,insen+=couple;
	  else // else [aA]
	    coupleBracket[1]=m,coupleBracket[2]=M,insen+=coupleBracket;
	}
      else
	{
	  if(s[i]=='[')
	    {
	      bracket=false;
	      for( size_t ii=i;ii<s.size();ii++) // Looking for matching right bracket
		if(s[ii]==']') { bracket=true; break; }

	      if(bracket) insen+=s[i];
	      else insen+="[[]";
	    }
	  else if(s[i]==']' && s[(!i?0:i-1)]!='[')
	    bracket=false, insen+=s[i];
	  else
	    insen+=s[i];
	}
    return insen;
  }


#ifndef _WIN32
  std::string BeautifyPath(std::string st, bool removeMark=true)
  {
    //removes series of "//", "/./" and "/.." and adjust path accordingly.
     if ( st.length( ) > 0 ) {
      size_t pp;
      pp=0;
      do {
        pp=st.find( "/./");
        if (pp!=string::npos) { st.erase(pp, 2);}
      } while (pp!=string::npos);
      pp=0;
      do {
        pp=st.find( "//");
        if (pp!=string::npos) { st.erase(pp, 1);}
      } while (pp!=string::npos);
      //Last "/.."
      pp=st.rfind( "/.."); //remove and back if last
      if (pp!=string::npos && pp==st.size()-3) {
        //erase from previous "/" to pp+3. Unless there is no previous "/"!
        size_t prevdir = st.rfind("/",pp-1);
        if (prevdir != string::npos) {st.erase(prevdir, pp+3-prevdir);}
      }
      //Last "/."
      pp=st.rfind( "/."); //remove if last
      if (pp!=string::npos && pp==st.size()-2) st.erase(pp);
      //Last "/" if removeMark is true
      if (removeMark) {
        pp=st.rfind( "/"); //remove and back if last
        if (pp!=string::npos && pp==st.size()-1) st.erase(pp);
      }
      // other places for "/..": between directories
      pp=0;
      do {
        pp=st.find( "/../");
        if (pp!=string::npos) {
          //erase from previous "/" to pp+3. Unless there is no previous "/"!
          size_t prevdir = st.rfind("/",pp-1);
          if (prevdir != string::npos) {st.erase(prevdir, pp+3-prevdir);}
          else break; //what should I do?
        }
      } while (pp!=string::npos);
      //First "./" 
      pp=st.find( "./"); //remove if first
      if (pp==0) st.erase(pp,2);
    }
  return st;
  }

#include <stdlib.h> 
void FileSearch( FileListT& fileList, const DString& pathSpec,
bool environment,
bool tilde,
bool accErr,
bool mark,
bool noSort,
bool quote,
bool dir,
bool period,
bool forceAbsPath,
bool fold_case,
bool *tests )
 {

  enum {
    testregular = 3, testdir, testzero, testsymlink
  };
  bool dotest = false;
  for ( SizeT i = 0; i < NTEST_SEARCH; i++ ) dotest |= tests[i];
  int globflags = 0;
  DString st;

  if ( environment )
    globflags |= GLOB_BRACE;

  if ( tilde )
    globflags |= GLOB_TILDE;

  if ( accErr )
    globflags |= GLOB_ERR;

  if ( mark && !dir ) // only mark directory if not in dir mode
    globflags |= GLOB_MARK;

  if ( noSort )
    globflags |= GLOB_NOSORT;

#if !defined(__APPLE__) && !defined(__FreeBSD__)
  if ( !quote ) // n/a on OS X
    globflags |= GLOB_NOESCAPE;

  if ( dir ) // simulate with lstat()
    globflags |= GLOB_ONLYDIR;

  if ( period ) // n/a on OS X
    globflags |= GLOB_PERIOD;
#endif
   if( fold_case)
      st=makeInsensitive(pathSpec);
    else
      st=pathSpec;  
  glob_t p;
  int gRes;
  if ( !forceAbsPath ) {
    if ( st != "" ) gRes = glob( st.c_str( ), globflags, NULL, &p );
    else gRes = glob( "*", globflags, NULL, &p );
  } else {
    int debug = 0;

    string pattern;
    if ( st == "" ) {
      pattern = GetCWD( );
      pattern.append( "/*" );
      gRes = glob( pattern.c_str( ), globflags, NULL, &p );
    } else {
      if (
      st.at( 0 ) != '/' &&
      !(tilde && st.at( 0 ) == '~') &&
      !(environment && st.at( 0 ) == '$')
      ) {
        pattern = GetCWD( );
        pattern.append( "/" );
        if ( !(st.size( ) == 1 && st.at( 0 ) == '.') ) pattern.append( st );

        if ( debug ) cout << "pattern : " << pattern << endl;

        gRes = glob( pattern.c_str( ), globflags, NULL, &p );
      } else {
        gRes = glob( st.c_str( ), globflags, NULL, &p );
      }
    }
    if ( debug ) {
      cout << "gRes : " << gRes << endl;
      cout << "st out : " << st << endl;
    }
  }

#ifndef __APPLE__
  if ( accErr && (gRes == GLOB_ABORTED || gRes == GLOB_NOSPACE) )
    throw GDLException( "FILE_SEARCH: Read error: " + pathSpec );
#else
  if ( accErr && (gRes != 0 && p.gl_pathc > 0) ) // NOMATCH is no error
    throw GDLException( "FILE_SEARCH: Read error: " + pathSpec );
#endif      

  struct stat64 statStruct, statlink;
  int accessmode = 0;
  if ( tests[0] ) accessmode = R_OK;
  if ( tests[1] ) accessmode |= W_OK;
  if ( tests[2] ) accessmode |= X_OK;
  int debug = 0;

  if ( gRes == 0 )
    for ( SizeT f = 0; f < p.gl_pathc; ++f ) {
      int actStat;
      std::string actFile = p.gl_pathv[ f];
      if ( dotest != 0 ) {

        actStat = lstat64( actFile.c_str(), &statStruct );
        if ( tests[testregular] && // (excludes dirs, sym)
        (S_ISREG( statStruct.st_mode ) == 0) ) continue;

        bool isaDir = (S_ISDIR( statStruct.st_mode ) != 0);
        bool isASymLink = (S_ISLNK( statStruct.st_mode ) != 0);
        if ( isASymLink ) {
          actStat = stat64( actFile.c_str(), &statlink );
          bool isaDir = (S_ISDIR( statlink.st_mode ) != 0);
          statStruct.st_mode |= statlink.st_mode;
        }
        if ( debug ) cout << isASymLink << isaDir << actFile << endl;
        if ( tests[testdir] && !isaDir ) continue;
        if ( tests[testsymlink] && !isASymLink ) continue;

        if ( tests[testzero] &&
        (statStruct.st_size != 0) ) continue;
        // now read, write, execute:
        if ( accessmode != 0 )
          if ( access( actFile.c_str(), accessmode ) != 0 ) continue;
      }


#ifndef __APPLE__
      fileList.push_back( BeautifyPath(actFile, !mark) );
#else
      if ( !dir )
        fileList.push_back(  BeautifyPath(actFile, !mark) );
      else { // push only if dir
        actStat = lstat64( actFile.c_str(), &statStruct );
        if ( S_ISDIR( statStruct.st_mode ) != 0 )
          fileList.push_back(  BeautifyPath(actFile, !mark) );
      }
#endif      
    }
  globfree( &p );

  if ( st == "" && dir )
    fileList.push_back( "" );
}
#endif // !def_WIN32
  // ** out of _WIN32 block-off until it fails.

  // AC 16 May 2014 : preliminary (and no MSwin support !)
  // revised by AC on June 28 
  // PRINT, FILE_expand_path([['','.'],['$PWD','src/']])
  // when the path is wrong, wrong output ...
  BaseGDL* file_expand_path( EnvT* e)
  {
    // always 1
    SizeT nParam=e->NParam(1);

    // accepting only strings as parameters
    BaseGDL* p0 = e->GetParDefined(0);
    if( p0->Type() != GDL_STRING)
      e->Throw("String expression required in this context: " + e->GetParString(0));
    DStringGDL* p0S = static_cast<DStringGDL*>(p0);

    SizeT nPath = p0S->N_Elements();

    DStringGDL* res = new DStringGDL(p0S->Dim(), BaseGDL::NOZERO);
    for( SizeT r=0; r<nPath ; ++r) {
      string tmp=(*p0S)[r];

      if (tmp.length() == 0) {
	char* cwd;
	char buff[PATH_MAX + 1];
	cwd = getcwd( buff, PATH_MAX + 1 );
	if( cwd != NULL ){
	  (*res)[r]= string(cwd);
	} 
	else {
	  (*res)[r]=""; //( errors are not managed ...)
	}
      } else {
	WordExp(tmp);
	char *symlinkpath =const_cast<char*> (tmp.c_str());
	char actualpath [PATH_MAX+1];
	char *ptr;
	ptr = realpath(symlinkpath, actualpath);
	if( ptr != NULL ){
	  (*res)[r] =string(ptr);
	} else {
	  //( errors are not managed ...)
	  (*res)[r] = tmp ;
	}
      }
    }
    return res;
  }
  /*
    Result = FILE_SEARCH(Path_Specification) (Standard)
    or for recursive searching,
    Result = FILE_SEARCH(Dir_Specification, Recur_Pattern)
    Standard: When called with a single Path_Specification argument, FILE_SEARCH returns 
    all files that match that specification. This is the same operation, sometimes 
    referred to as file globbing, performed by most operating system command interpreters
    when wildcard characters are used in file specifications.
    Recursive: When called with two arguments, FILE_SEARCH performs recursive searching 
    of directory hierarchies. In a recursive search, FILE_SEARCH looks recursively for 
    any and all subdirectories in the file hierarchy rooted at the Dir_Specification argument.
    Within each of these subdirectories, it returns the names of all files that match the 
    pattern in the Recur_Pattern argument. 
    This operation is similar to that performed by the UNIX find(1) command.
    NOTE: in order to avoid infinite reference loops,
    IDL policy states that symlinks are not followed.
    symlnk references should therefore be returned as found, without resolution, and
    can be processed by the FILE_READLINK function.
    NOTE: Contrary to above documented intention, acutal IDL behavior is, that
    when called with two arguments the Dir_specification argument could itself be a pattern-search;
    hence the dance below where, for Nparam > 1, first duty is to search on the 1st parameter for directories.
    Result = FILE_READLINK(Path [, /ALLOW_NONEXISTENT] [, /ALLOW_NONSYMLINK] [, /NOEXPAND_PATH] )
    // not finished yet
    */
  BaseGDL* file_search( EnvT* e)
  {
    SizeT nParam=e->NParam(); // 0 -> "*"
    
    DStringGDL* pathSpec;
    DString     Pattern = "";
    SizeT nPath = 0;

    if( nParam > 0)
      {
	BaseGDL* p0 = e->GetParDefined( 0);
	pathSpec = dynamic_cast<DStringGDL*>( p0);
	if( pathSpec == NULL)
	  e->Throw( "String expression required in this context.");

	nPath = pathSpec->N_Elements();

	if( nParam > 1)   e->AssureScalarPar< DStringGDL>( 1, Pattern);
       
      } 
      static int TEST_READIx = e->KeywordIx("TEST_READ");
      static int TEST_WRITEIx = e->KeywordIx("TEST_WRITE");
      static int TEST_EXECUTABLEIx = e->KeywordIx("TEST_EXECUTABLE");
      static int TEST_REGULARIx = e->KeywordIx("TEST_REGULAR");
      static int TEST_DIRECTORYIx = e->KeywordIx("TEST_DIRECTORY");
      static int TEST_ZERO_LENGTHIx = e->KeywordIx("TEST_ZERO_LENGTH");
      static int TEST_SYMLINKIx = e->KeywordIx("TEST_SYMLINK");
    const int test_kwIx[]={
      TEST_READIx, TEST_WRITEIx, TEST_EXECUTABLEIx,
      TEST_REGULARIx, TEST_DIRECTORYIx, TEST_ZERO_LENGTHIx,
      TEST_SYMLINKIx};
    
//    const string test_kw[]={
//      "TEST_READ", "TEST_WRITE",  "TEST_EXECUTABLE",
//      "TEST_REGULAR", "TEST_DIRECTORY", "TEST_ZERO_LENGTH",
//      "TEST_SYMLINK"};
    bool tests[NTEST_SEARCH];
    static int keyindex;
    for( SizeT i=0; i < NTEST_SEARCH; i++) {
      tests[i] = false;
//      keyindex = e->KeywordIx(test_kw[i]); //TODO: check following (static int vs. multiple choices)
//      if (e->KeywordPresent(keyindex)) tests[i] = e->KeywordSet(keyindex);
      if (e->KeywordPresent(test_kwIx[i])) tests[i] = e->KeywordSet(test_kwIx[i]);
    }
    // keywords
    bool tilde = true;
    bool environment = true;
    bool fold_case = false;
    // next three have default behaviour
    static int tildeIx = e->KeywordIx( "EXPAND_TILDE");
    bool tildeKW = e->KeywordPresent( tildeIx);
    if( tildeKW) tilde = e->KeywordSet( tildeIx);

    static int environmentIx = e->KeywordIx( "EXPAND_ENVIRONMENT");
    bool environmentKW = e->KeywordPresent( environmentIx);
    if( environmentKW) 
      {
	environment = e->KeywordSet( environmentIx);
	if( environment) // only warn when expiclitely set
	  Warning( "FILE_SEARCH: EXPAND_ENVIRONMENT not supported.");
      }

    static int fold_caseIx = e->KeywordIx( "FOLD_CASE");
    bool fold_caseKW = e->KeywordPresent( fold_caseIx);
    if( fold_caseKW) fold_case = e->KeywordSet( fold_caseIx);

    // 
    static int countIx = e->KeywordIx( "COUNT");
    bool countKW = e->KeywordPresent( countIx);

    static int accerrIx = e->KeywordIx( "ISSUE_ACCESS_ERROR");
    bool accErr = e->KeywordSet( accerrIx);

    static int markIx = e->KeywordIx( "MARK_DIRECTORY");
    bool mark = e->KeywordSet( markIx);

    static int nosortIx = e->KeywordIx( "NOSORT");
    bool noSort = e->KeywordSet( nosortIx);

    static int quoteIx = e->KeywordIx( "QUOTE");
    bool quote = e->KeywordSet( quoteIx);

    static int match_dotIx = e->KeywordIx( "MATCH_INITIAL_DOT");
    bool match_dot = e->KeywordSet( match_dotIx);

    static int match_all_dotIx = e->KeywordIx( "MATCH_ALL_INITIAL_DOT");
    bool match_all_dot = e->KeywordSet( match_all_dotIx);

    static int fully_qualified_pathIx = e->KeywordIx( "FULLY_QUALIFY_PATH");
    bool forceAbsPath = e->KeywordSet( fully_qualified_pathIx);

    if( match_all_dot)
      Warning( "FILE_SEARCH: MATCH_ALL_INITIAL_DOT keyword ignored (not supported).");

    bool onlyDir = nParam > 1;

    FileListT fileList;

    int debug=0;
    DLong count;
#ifndef _WIN32
    if( nPath == 0)
      FileSearch( fileList, "",
		  environment, tilde, 
                  accErr, mark, noSort, quote, onlyDir,
		  match_dot, forceAbsPath, fold_case,
		  tests);
    else
      for( SizeT f=0; f < nPath; ++f) 
	FileSearch( fileList, (*pathSpec)[f],  environment, tilde, 
		    accErr, mark, noSort, quote, onlyDir,
		    match_dot, forceAbsPath, fold_case,
		    tests);
#else
    if(nPath == 0)
      PatternSearch( fileList, "", Pattern, accErr, quote,
		     match_dot, forceAbsPath, fold_case,
		     onlyDir,   tests, false);
    else

      for( SizeT f=0; f < nPath; ++f) {
	DString DirSpec;
	DString dirsearch;
	int dirsep=-1;
	int ii=0;
	DirSpec = (*pathSpec)[f];
	int lenpath = DirSpec.length();
	do
	  if((DirSpec[ii] == '/') || (DirSpec[ii] == '\\')) dirsep=ii;
	while( (DirSpec[ii++] != 0) && (ii < lenpath) );
	if(debug) if( f==0)
		    cout <<nPath<<"=#paths, 1st call to PatternSearch. onlyDir?"<<onlyDir<<DirSpec<<endl;
	dirsearch = DirSpec.substr(dirsep+1);
	DirSpec.resize(dirsep+1);
	if(debug) cout << " pathSpec[f]:"<<DirSpec<<"<dirspec pattern>"<<Pattern<<endl;
	PatternSearch( fileList, DirSpec, dirsearch, accErr, quote,
		       match_dot, forceAbsPath, fold_case,
		       onlyDir,   tests, false);
      }
#endif
    onlyDir = false; // retire this variable here in place of ( nParam > 1)
    count = fileList.size();
    if( nParam > 1)
      { // recursive search for recurPattern
	FileListT fileOut;
	for( SizeT f=0; f<count; ++f) 
	  {
	    //    cout <<count<< Pattern <<"Looking in: " << fileList[f] << endl;
	    PatternSearch( fileOut, fileList[f], Pattern, accErr,
			   quote,   match_dot, forceAbsPath, fold_case,
			   onlyDir ,   tests, true);
	  }	

	DLong pCount = fileOut.size();
	
	if( countKW)
	  e->SetKW( countIx, new DLongGDL( pCount));

	if( pCount == 0)
	  return new DStringGDL("");

	if( !noSort)
	  sort( fileOut.begin(), fileOut.end());
    
	// fileOut -> res
	DStringGDL* res = new DStringGDL( dimension( pCount), BaseGDL::NOZERO);
	for( SizeT r=0; r<pCount; ++r)
	  (*res)[r] = fileOut[ r];

	return res;
      }

    if( countKW)
      e->SetKW( countIx, new DLongGDL( count));

    if( count == 0)
      return new DStringGDL("");
    
    if( !noSort)
      sort( fileList.begin(), fileList.end());

    // fileList -> res
    DStringGDL* res = new DStringGDL( dimension( count), BaseGDL::NOZERO);
    for( SizeT r=0; r<count; ++r)
      (*res)[r] = fileList[ r];

    return res;
  }



  BaseGDL* file_basename( EnvT* e)
  {

    SizeT nParams=e->NParam( 1);

    // accepting only strings as parameters
    BaseGDL* p0 = e->GetParDefined(0);
    if( p0->Type() != GDL_STRING)
      e->Throw("String expression required in this context: " + e->GetParString(0));
    DStringGDL* p0S = static_cast<DStringGDL*>(p0);

    BaseGDL* p1;
    DStringGDL* p1S;
    bool DoRemoveSuffix = false;

    if (nParams == 2) {
      // shall we remove a suffix ?
      p1 = e->GetPar(1);
      if( p1 == NULL || p1->Type() != GDL_STRING)
	e->Throw("String expression required in this context: " + e->GetParString(1));
      p1S = static_cast<DStringGDL*>(p1);
      if (p1S->N_Elements() == 1) {
	if ((*p1S)[0].length() >0) DoRemoveSuffix=true;
      }
      if (p1S->N_Elements() > 1) 
	e->Throw(" Expression must be a scalar or 1 element array in this context: " + e->GetParString(1));
    }
    
    dimension resDim;
    resDim=p0S->Dim();
    DStringGDL* res = new DStringGDL(resDim, BaseGDL::NOZERO);

    for (SizeT i = 0; i < p0S->N_Elements(); i++) {

      //tmp=strdup((*p0S)[i].c_str());
      const string& tmp=(*p0S)[i];

      //      cout << ">>"<<(*p0S)[i].c_str() << "<<" << endl;
      if (tmp.length() > 0) {

#ifdef _WIN32
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( tmp.c_str(),drive,dir,fname,ext);
	string bname = string(fname)+ext;
#else
	char buf[ PATH_MAX+1];
	strncpy(buf, tmp.c_str(), PATH_MAX+1);
	string bname = basename(buf);
#endif

	(*res)[i] = bname;
      } 
      else
	{
	  (*res)[i]="";
	}
    }

    // managing suffixe
    if (DoRemoveSuffix) {
      
      string suffixe=(*p1S)[0];
      int suffLength=(*p1S)[0].length();
      
      static int fold_caseIx = e->KeywordIx( "FOLD_CASE");
      bool fold_case = e->KeywordSet( fold_caseIx);
      
      if (fold_case) suffixe=StrUpCase(suffixe);

      //cout << "suffixe :"<< suffixe << endl;

      
      string tmp1, fin_tmp;
      for (SizeT i = 0; i < p0S->N_Elements(); i++) {
	tmp1=(*res)[i];
	
	// Strickly greater : if equal, we keep it !
	if (tmp1.length() > suffLength) {
	  fin_tmp=tmp1.substr(tmp1.length()-suffLength);
	  
	  if (fold_case) fin_tmp=StrUpCase(fin_tmp);
	  
	  if (fin_tmp.compare(suffixe) == 0) {
	    (*res)[i]=tmp1.substr(0,tmp1.length()-suffLength);
	  }	 	  
	}
      }
      
    }

    return res;
  }


  BaseGDL* file_dirname( EnvT* e)
  {
    // accepting only strings as parameters
    BaseGDL* p0 = e->GetParDefined(0);
    if( p0->Type() != GDL_STRING)
      e->Throw("String expression required in this context: " + e->GetParString(0));
    DStringGDL* p0S = static_cast<DStringGDL*>(p0);

    dimension resDim;
    resDim=p0S->Dim();
    DStringGDL* res = new DStringGDL(resDim, BaseGDL::NOZERO);

    for (SizeT i = 0; i < p0S->N_Elements(); i++) {
      //tmp=strdup((*p0S)[i].c_str());
      const string& tmp = (*p0S)[i];
      // if 0/ if 1: preference.
#ifdef _WIN32
#	if 1
      char path_buffer[_MAX_PATH];
      char drive[_MAX_DRIVE];
      char dir[_MAX_DIR];
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];

      DString::size_type pos = 0, offset = 0;
      DString tmp2(tmp);
      while ((pos = tmp2.find("/", offset)) != string::npos)
	{
	  tmp2[pos] = '\\';
	  offset = pos + 1;
	}
      //   while (tmp2[tmp2.size() - 1] == '\\') tmp2.pop_back();
      int size=tmp2.size();
      if(tmp2[size--] == '\\') 
	do  	tmp2.resize(size);
	while((size != 0) && tmp2[size--] =='\\');

      _splitpath( tmp2.c_str(),drive,dir,fname,ext);
      dir[strlen(dir) - 1] = 0; // Remove seperator
      DString dname = DString(drive)+dir;

      //   while (dname[dname.size() - 1] == '\\') dname.pop_back();
      size = dname.size();
      if(dname[size--] == '\\')
	do      dname.resize(size);
	while((size != 0) && dname[size--] =='\\');
#	elif 0
      char buf[ PATH_MAX+1];
      strncpy(buf, tmp.c_str(), PATH_MAX+1);
      string dname = dirname(buf);
#   endif
#endif
#ifndef _WIN32
      char buf[ PATH_MAX+1];
      strncpy(buf, tmp.c_str(), PATH_MAX+1);
      string dname = dirname(buf);
#endif
      (*res)[i] = dname;

    }
    static int file_dirnameIx=e->KeywordIx("MARK_DIRECTORY");
    if (e->KeywordSet(file_dirnameIx)) {
      for (SizeT i = 0; i < p0S->N_Elements(); i++) {
	(*res)[i]=(*res)[i] + PathSeparator();
      }
    }
    
    return res;

  }

  BaseGDL* file_same( EnvT* e)
  {
    // assuring right number of parameters
    SizeT nParam=e->NParam(2); 

    // accepting only strings as parameters
    BaseGDL* p0 = e->GetParDefined(0);
    DStringGDL* p0S = dynamic_cast<DStringGDL*>(p0);
    if (p0S == NULL) e->Throw("String expression required in this context: " + e->GetParString(0));

    BaseGDL* p1 = e->GetParDefined(1);
    DStringGDL* p1S = dynamic_cast<DStringGDL*>(p1);
    if (p1S == NULL) e->Throw("String expression required in this context: " + e->GetParString(1));

    // no empty strings accepted
    {
      int empty = 0;
      for (SizeT i = 0; i < p0S->N_Elements(); i++) empty += (*p0S)[i].empty();
      for (SizeT i = 0; i < p1S->N_Elements(); i++) empty += (*p1S)[i].empty();
      if (empty != 0) e->Throw("Null filename not allowed.");
    }

    // allocating memory for the comparison result
    DByteGDL* res;
    {
      dimension resDim;
      if (p0S->Rank() == 0 || p1S->Rank() == 0) {
        resDim = (p0S->N_Elements() > p1S->N_Elements() ? p0S : p1S)->Dim(); 
      } else {
        resDim = (p0S->N_Elements() < p1S->N_Elements() ? p0S : p1S)->Dim(); 
      }
      res = new DByteGDL(resDim); // zero
    }

    // comparison loop
    for (SizeT i = 0; i < res->N_Elements(); i++)
      {
	// deciding which filename to compare
	SizeT p0idx = p0S->Rank() == 0 ? 0 : i;
	SizeT p1idx = p1S->Rank() == 0 ? 0 : i;

	// checking for lexically identical paths
	if ((*p0S)[p0idx].compare((*p1S)[p1idx]) == 0)
	  {
	    (*res)[i] = 1;
	    continue;
	  }

	// expanding if needed (tilde, shell variables, etc)
	const char *file0, *file1;
	string tmp0, tmp1;
    static int noexpoand_pathIx=e->KeywordIx("NOEXPAND_PATH");
	if (!e->KeywordSet(noexpoand_pathIx)) 
	  {
	    tmp0 = (*p0S)[p0idx];
	    WordExp(tmp0);
	    tmp1 = (*p1S)[p1idx];
	    WordExp(tmp1);
	    // checking again for lexically identical paths (e.g. ~/ and $HOME)
	    if (tmp0.compare(tmp1) == 0)
	      {
		(*res)[i] = 1;
		continue;
	      }
	    file0 = tmp0.c_str();
	    file1 = tmp1.c_str();
	  }
	else
	  {
	    file0 = (*p0S)[p0idx].c_str();
	    file1 = (*p1S)[p1idx].c_str();
	  }

	// checking for the same inode/device numbers
	struct stat64 statStruct;
	dev_t file0dev;
	ino_t file0ino;    
	int ret = stat64(file0, &statStruct);
	if (ret != 0) continue;
	file0dev = statStruct.st_dev;
	file0ino = statStruct.st_ino;
	ret = stat64(file1, &statStruct);
	if (ret != 0) continue;
	(*res)[i] = (file0dev == statStruct.st_dev && file0ino == statStruct.st_ino);

      }

    return res;

  }
  BaseGDL* file_test( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    
    BaseGDL* p0 = e->GetParDefined( 0);

    DStringGDL* p0S = dynamic_cast<DStringGDL*>( p0);
    if( p0S == NULL)
      e->Throw( "String expression required in this context: "+
		e->GetParString(0));

    static int directoryIx = e->KeywordIx( "DIRECTORY");
    bool directory = e->KeywordSet( directoryIx);

    static int executableIx = e->KeywordIx( "EXECUTABLE");
    bool executable = e->KeywordSet( executableIx);

    static int readIx = e->KeywordIx( "READ");
    bool read = e->KeywordSet( readIx);

    static int writeIx = e->KeywordIx( "WRITE");
    bool write = e->KeywordSet( writeIx);

    static int zero_lengthIx = e->KeywordIx( "ZERO_LENGTH");
    bool zero_length = e->KeywordSet( zero_lengthIx);

    static int get_modeIx = e->KeywordIx( "GET_MODE");
    bool get_mode = e->KeywordPresent( get_modeIx);

    static int regularIx = e->KeywordIx( "REGULAR");
    bool regular = e->KeywordSet( regularIx);

    static int block_specialIx = e->KeywordIx( "BLOCK_SPECIAL");
    bool block_special = e->KeywordSet( block_specialIx);

    static int character_specialIx = e->KeywordIx( "CHARACTER_SPECIAL");
    bool character_special = e->KeywordSet( character_specialIx);

    static int named_pipeIx = e->KeywordIx( "NAMED_PIPE");
    bool named_pipe = e->KeywordSet( named_pipeIx);

    static int socketIx = e->KeywordIx( "SOCKET");
    bool socket = e->KeywordSet( socketIx);

    static int symlinkIx = e->KeywordIx( "SYMLINK");
    bool symlink = e->KeywordSet( symlinkIx);

    static int dSymlinkIx = e->KeywordIx( "DANGLING_SYMLINK");
    bool dsymlink = e->KeywordSet( dSymlinkIx);

    static int noexpand_pathIx = e->KeywordIx( "NOEXPAND_PATH");
    bool noexpand_path = e->KeywordSet( noexpand_pathIx);

    DLongGDL* getMode = NULL; 
    if( get_mode)
      {
	getMode = new DLongGDL( p0S->Dim()); // zero
	e->SetKW( get_modeIx, getMode);
      }
    
    DLongGDL* res = new DLongGDL( p0S->Dim()); // zero

    //     bool doStat = 
    //       zero_length || get_mode || directory || 
    //       regular || block_special || character_special || 
    //       named_pipe || socket || symlink;

    SizeT nEl = p0S->N_Elements();

    for( SizeT f=0; f<nEl; ++f)
      {
	string actFile;

	if ( !noexpand_path ) {
	  string tmp = (*p0S)[f];
          WordExp(tmp);

	  // Ilia2015 : about "|" : see 2 places (file_test() and file_info()) in "file.cpp",
	  // and one place in "str.cpp" same label
	  tmp=tmp.substr(0, tmp.find("|", 0));

	  if( tmp.length() > 1 && tmp[ tmp.length()-1] == '/')
	    actFile = tmp.substr(0,tmp.length()-1);
	  else
	    actFile = tmp;
        } 
        else 
	  {
	    const string& tmp = (*p0S)[f];
	    if( tmp.length() > 1 && tmp[ tmp.length()-1] == '/')
	      actFile = tmp.substr(0,tmp.length()-1);
	    else
	      actFile = tmp;
	  }

        struct stat64 statStruct, statlink;

        int actStat = lstat64(actFile.c_str(), &statStruct);
#ifdef _WIN32
	DWORD dwattrib;
        int addlink = 0;
        fstat_win32(actFile, addlink, dwattrib);
        statStruct.st_mode |= addlink;
#endif
        bool isASymLink = (S_ISLNK(statStruct.st_mode) != 0);
        if (isASymLink ) actStat = stat64(actFile.c_str(), &statlink);
	//
	//be more precise in case of symlinks --- use stat
	// to check if target exists or is a dangling symlink!
	//
        bool isADanglingSymLink = (actStat != 0 && isASymLink);

	if( actStat != 0) 	  continue;

	if( read && access( actFile.c_str(), R_OK) != 0)  continue;
	if( write && access( actFile.c_str(), W_OK) != 0)  continue;
	if( zero_length && statStruct.st_size != 0) 	  continue;


#ifndef _WIN32

	if( executable && access( actFile.c_str(), X_OK) != 0)	  continue;

	if( get_mode)
	  (*getMode)[ f] = statStruct.st_mode & 
	    (S_IRWXU | S_IRWXG | S_IRWXO);

	if( block_special && S_ISBLK(statStruct.st_mode) == 0) 	  continue;

	if( character_special && S_ISCHR(statStruct.st_mode) == 0) continue;

	if( named_pipe && S_ISFIFO(statStruct.st_mode) == 0)       continue;

	if( socket && S_ISSOCK(statStruct.st_mode) == 0) 	  continue;

	if( dsymlink && !isADanglingSymLink ) 	  continue;

#endif
        if( symlink && !isASymLink ) 	  continue;

	if( directory && (S_ISDIR(statStruct.st_mode) || (S_ISDIR(statlink.st_mode) && isASymLink)) == 0) 
	  continue;

	if( regular && S_ISREG(statStruct.st_mode) == 0) 
	  continue;

	(*res)[ f] = 1;

      }
    return res;
  }
  // Result = FILE_READLINK(Path [, /ALLOW_NONEXISTENT] [, /ALLOW_NONSYMLINK] [, /NOEXPAND_PATH] )
  BaseGDL* file_readlink( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    DStringGDL* p0S = dynamic_cast<DStringGDL*>(e->GetParDefined(0));
    if( p0S == NULL)
      e->Throw( "String expression required in this context: "+e->GetParString(0));
	  
    static int noexpand_pathIx = e->KeywordIx( "NOEXPAND_PATH");
    bool noexpand_path = e->KeywordSet(noexpand_pathIx);
    static int allow_nonexistIx = e->KeywordIx( "ALLOW_NONEXISTENT");
    bool allow_nonexist = e->KeywordSet(allow_nonexistIx);
    static int allow_nonsymlinkIx = e->KeywordIx( "ALLOW_NONSYMLINK");
    bool allow_nonsymlink = e->KeywordSet(allow_nonsymlinkIx);
    
    SizeT nPath = p0S->N_Elements();

    DStringGDL* res = new DStringGDL(p0S->Dim(), BaseGDL::NOZERO);

#if 0
    for (SizeT f = 0; f < nPath; f++)
      {
	// This is a random code block thrown in to start programming the routine
	// #elif 1 below begins the actually functional code.
	// I don't know why thbis #if/ifdef/endif/elif  sequennce works the way it was intended!
	const char* actFile;
        string tmp;
        if (!noexpand_path) 
	  {
	    tmp = (*p0S)[f];
	    WordExp(tmp);
	    actFile = tmp.c_str();
	  } 
        else actFile = (*p0S)[f].c_str();

	struct stat64 statStruct, statlink;
	int actStat = lstat64(actFile, &statStruct);

	//#ifdef _WIN32 commented out just because it looks disturbing.
	DWORD dwattrib;
	int addlink = 0;
	fstat_win32(actFile, addlink, dwattrib);
	statStruct.st_mode |= addlink;
	//#endif

	bool isASymLink = S_ISLNK(statStruct.st_mode);
	if (isASymLink ) actStat = stat64(actFile, &statlink);
	// Here begins the (quicky) real code. Looks like it will also double as 
	// a GDL call to realpath() for non-symlinked files, also.
	//
#elif 1
	{

	  for( SizeT r=0; r<nPath ; ++r) {
	    string tmp=(*p0S)[r];

	    if (tmp.length() == 0) {
	      (*res)[r]=""; //( errors are not managed ...)
	    } else {
	      WordExp(tmp);
	      char *symlinkpath =const_cast<char*> (tmp.c_str());
	      char actualpath [PATH_MAX+1];
	      char *ptr;
	      ptr = realpath(symlinkpath, actualpath);
	      if( ptr != NULL ){
		(*res)[r] =string(ptr);
	      } else {
		(*res)[r] = tmp ;
	      }
	    }
	  }
	  return res;

	}
#endif	
      }
	
	
    BaseGDL* file_info( EnvT* e)
    {
      SizeT nParam=e->NParam( 1); 
      DStringGDL* p0S = dynamic_cast<DStringGDL*>(e->GetParDefined(0));
      if( p0S == NULL)
	e->Throw( "String expression required in this context: "+
		  e->GetParString(0));

      static int noexpand_pathIx = e->KeywordIx( "NOEXPAND_PATH");
      bool noexpand_path = e->KeywordSet(noexpand_pathIx);

      DStructGDL* res = new DStructGDL(
				       FindInStructList(structList, "FILE_INFO"), 
				       p0S->Rank() == 0 ? dimension(1) : p0S->Dim()
				       ); 

      int tName = tName = res->Desc()->TagIndex("NAME");
      int tExists, tRead, tWrite, tExecute, tRegular, tDirectory, tBlockSpecial, 
	tCharacterSpecial, tNamedPipe, tSetuid, tSetgid, tSocket, tStickyBit, 
	tSymlink, tDanglingSymlink, tMode, tAtime, tCtime, tMtime, tSize;
      int indices_known = false;

      SizeT nEl = p0S->N_Elements();

      for (SizeT f = 0; f < nEl; f++)
	{
	  // NAME
	  const char* actFile;
	  string tmp;
	  if (!noexpand_path) 
	    {
	      tmp = (*p0S)[f];
	      WordExp(tmp);
	  
	      // Ilia2015 : about "|" : see 2 places (file_test() and file_info()) in "file.cpp",
	      // and one place in "str.cpp" same label
	      tmp=tmp.substr(0, tmp.find("|", 0)); //take the first file that corresponds the pattern.
	      if( tmp.length() > 1 && tmp[ tmp.length()-1] == '/')
		actFile = tmp.substr(0,tmp.length()-1).c_str();
	      else
		actFile = tmp.c_str();
	    }
	  else actFile = (*p0S)[f].c_str();
	  *(res->GetTag(tName, f)) = DStringGDL(actFile);

	  // stating the file (and moving on to the next file if failed)
	  struct stat64 statStruct, statlink;

	  int actStat = lstat64(actFile, &statStruct);

#ifdef _WIN32
	  DWORD dwattrib;
	  int addlink = 0;
	  fstat_win32(actFile, addlink, dwattrib);
	  statStruct.st_mode |= addlink;
#endif

	  bool isaDir = (S_ISDIR(statStruct.st_mode) != 0);
	  bool isASymLink = S_ISLNK(statStruct.st_mode);

#ifdef _WIN32
	  if(isASymLink && (statStruct.st_size > 0))
	    statStruct.st_size = (statStruct.st_size -12)/2 - 1;
#else
	  if (isASymLink ) {
	    actStat = stat64(actFile, &statlink);
	    isaDir = (S_ISDIR(statlink.st_mode) != 0);
	    // This works ok here but fails in FileSearch
	  }
#endif

	  //
	  //be more precise in case of symlinks --- use stat
	  // to check if target exists or is a dangling symlink!
	  //
	  bool isADanglingSymLink = (actStat != 0 && isASymLink); 


	  // checking struct tag indices (once)

	  if (!indices_known) 

	    {

	      tExists =           res->Desc()->TagIndex("EXISTS"); 
	      tRead =             res->Desc()->TagIndex("READ"); 
	      tWrite =            res->Desc()->TagIndex("WRITE"); 
	      tRegular =          res->Desc()->TagIndex("REGULAR"); 
	      tDirectory =        res->Desc()->TagIndex("DIRECTORY");

	      //#ifndef _MSC_VER

	      tBlockSpecial =     res->Desc()->TagIndex("BLOCK_SPECIAL");
	      tCharacterSpecial = res->Desc()->TagIndex("CHARACTER_SPECIAL");
	      tNamedPipe =        res->Desc()->TagIndex("NAMED_PIPE");
	      tExecute =          res->Desc()->TagIndex("EXECUTE"); 
	      //#ifndef _WIN32
	      tSetuid =           res->Desc()->TagIndex("SETUID");
	      tSetgid =           res->Desc()->TagIndex("SETGID");
	      //#else
	      //          tSetuid =           res->Desc()->TagIndex("SYSTEM");
	      //          tSetgid =           res->Desc()->TagIndex("HIDDEN");
	      //#endif
	      tSocket =           res->Desc()->TagIndex("SOCKET");
	      tStickyBit =        res->Desc()->TagIndex("STICKY_BIT");
	      tSymlink =          res->Desc()->TagIndex("SYMLINK");
	      tDanglingSymlink =  res->Desc()->TagIndex("DANGLING_SYMLINK");
	      tMode =             res->Desc()->TagIndex("MODE");

	      //#endif

	      tAtime =            res->Desc()->TagIndex("ATIME");
	      tCtime =            res->Desc()->TagIndex("CTIME");
	      tMtime =            res->Desc()->TagIndex("MTIME");
	      tSize =             res->Desc()->TagIndex("SIZE");

	      indices_known = true;

	    }
	  // DANGLING_SYMLINK good place
	  // SYMLINK

	  if (isASymLink)
	    {
	      *(res->GetTag(tSymlink, f)) = DByteGDL(1);
	      if( actStat != 0 )
		*(res->GetTag(tDanglingSymlink, f)) = DByteGDL(1);
	    }

	  if( actStat != 0 ) continue;

	  // EXISTS (would not reach here if stat failed)
	  *(res->GetTag(tExists, f)) = DByteGDL(1);
        
	  // READ, WRITE, EXECUTE

	  *(res->GetTag(tRead, f)) =    DByteGDL(access(actFile, R_OK) == 0);

	  *(res->GetTag(tWrite, f)) =   DByteGDL(access(actFile, W_OK) == 0);

#ifndef _MSC_VER

	  *(res->GetTag(tExecute, f)) = DByteGDL(access(actFile, X_OK) == 0);

#endif



	  // REGULAR, DIRECTORY, BLOCK_SPECIAL, CHARACTER_SPECIAL, NAMED_PIPE, SOCKET

	  *(res->GetTag(tRegular, f)) =          DByteGDL(S_ISREG( statStruct.st_mode) != 0);

	  *(res->GetTag(tDirectory, f)) =        DByteGDL(isaDir);

#ifndef _WIN32

	  *(res->GetTag(tBlockSpecial, f)) =     DByteGDL(S_ISBLK( statStruct.st_mode) != 0);

	  *(res->GetTag(tCharacterSpecial, f)) = DByteGDL(S_ISCHR( statStruct.st_mode) != 0);

	  *(res->GetTag(tNamedPipe, f)) =        DByteGDL(S_ISFIFO(statStruct.st_mode) != 0);
#ifndef __MINGW32__
	  *(res->GetTag(tSocket, f)) =           DByteGDL(S_ISSOCK(statStruct.st_mode) != 0);
#endif

#endif  

	  // SETUID, SETGID, STICKY_BIT

#ifndef _WIN32

	  *(res->GetTag(tSetuid, f)) =           DByteGDL((S_ISUID & statStruct.st_mode) != 0);
	  *(res->GetTag(tSetgid, f)) =           DByteGDL((S_ISGID & statStruct.st_mode) != 0);
	  *(res->GetTag(tStickyBit, f)) =        DByteGDL((S_ISVTX & statStruct.st_mode) != 0);

	  // MODE

	  *(res->GetTag(tMode, f)) = DLongGDL(
					      statStruct.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX)
					      );
#else
	  *(res->GetTag(tSetuid, f)) = DByteGDL(0);
	  *(res->GetTag(tSetgid, f)) = DByteGDL(0);
	  //*(res->GetTag(tSetuid, f)) =           DByteGDL(
	  //	(FILE_ATTRIBUTE_SYSTEM & dwattrib) != 0);
	  //*(res->GetTag(tSetgid, f)) =           DByteGDL(
	  //	(FILE_ATTRIBUTE_HIDDEN & dwattrib) != 0);
	  *(res->GetTag(tMode, f)) = DLongGDL(dwattrib);
#endif

	  // ATIME, CTIME, MTIME
	  *(res->GetTag(tAtime, f)) = DLong64GDL(statStruct.st_atime);
	  *(res->GetTag(tCtime, f)) = DLong64GDL(statStruct.st_ctime);
	  *(res->GetTag(tMtime, f)) = DLong64GDL(statStruct.st_mtime);

	  // SIZE
	  *(res->GetTag(tSize, f)) = DLong64GDL(statStruct.st_size);
	}

      return res;

    }

    void file_mkdir( EnvT* e)
    {
      // sanity checks
      SizeT nParam=e->NParam( 1);
      for (int i=0; i<nParam; i++)
	{
	  if (dynamic_cast<DStringGDL*>(e->GetParDefined(i)) == NULL) 
	    e->Throw( "All arguments must be string scalars/arrays, argument " + i2s(i+1) + " is: " + e->GetParString(i));
	}

      static int noexpand_pathIx = e->KeywordIx( "NOEXPAND_PATH");
      bool noexpand_path = e->KeywordSet( noexpand_pathIx);
#ifndef _WIN32
      string cmd = "mkdir -p";
#else
      string cmd = "mkdir";
#endif
      for (int i=0; i<nParam; i++)
	{
	  DStringGDL* pi = dynamic_cast<DStringGDL*>(e->GetParDefined(i));
	  for (int j=0; j<pi->N_Elements(); j++)
	    {
	      string tmp = (*pi)[j];
	      //	cout<<tmp<<"--tmp\n";
	      if (!noexpand_path) WordExp(tmp);
	      tmp="'"+tmp+"'";
	      cmd.append(" " + tmp);
	    }
	}
#ifndef _WIN32
      cmd.append(" 2>&1 | awk '{print \"% FILE_MKDIR: \" $0; exit 1}'");
#endif
      // SA: calling system(), mkdir and awk is surely not the most efficient way, 
      //     but copying a bunch of code from coreutils does not seem elegant either
      //    system("echo 'hello world'");
      if (system(cmd.c_str()) != 0) e->Throw("failed to create a directory (or execute mkdir).");
    }

  }

  //#endif
