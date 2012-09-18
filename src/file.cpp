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

#include "basegdl.hpp"
#include "str.hpp"

//#ifdef HAVE_LIBWXWIDGETS

#include "envt.hpp"
#include "file.hpp"
#include "objects.hpp"

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <climits> // PATH_MAX

// #include <wx/utils.h>
// #include <wx/file.h>
// #include <wx/dir.h>

#ifndef _MSC_VER
#include <glob.h>
#include <fnmatch.h>
#include <dirent.h>
#else
#include <io.h>
#define access _access
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */
#define PATH_MAX 255

#include <direct.h>

#if !defined(S_ISDIR)
#define __S_ISTYPE(mode, mask)	(((mode) & S_IFMT) == (mask))
#define S_ISDIR(mode)	 __S_ISTYPE((mode), S_IFDIR)
#define S_ISREG(mode)    __S_ISTYPE((mode), S_IFREG)
#endif
#include <shlwapi.h>
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

namespace lib {

  using namespace std;

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
	const char* actFile;
        string tmp;

        if (!noexpand_path) 
        {
          tmp = (*p0S)[f];
          WordExp(tmp);
	  if( tmp.length() > 1 && tmp[ tmp.length()-1] == '/')
          actFile = tmp.substr(0,tmp.length()-1).c_str();
	else
          actFile = tmp.c_str();
        } 
        else 
        {
          tmp = (*p0S)[f];
	  if( tmp.length() > 1 && tmp[ tmp.length()-1] == '/')
          actFile = tmp.substr(0,tmp.length()-1).c_str();
	else
          actFile = tmp.c_str();
        }

	

	struct stat statStruct;
#ifdef _MSC_VER
	int actStat = stat( actFile, &statStruct);
#else
	int actStat = lstat( actFile, &statStruct);
#endif
	if( actStat != 0) 
	  continue;

	// 	if( !wxFileExists( actFile) && !wxDirExists( actFile))
	// 	  continue;

	// 	if( directory && !wxDirExists( actFile))
	// 	  continue;
	// 	if( read && !wxFile::Access( actFile, wxFile::read))
	// 	  continue;
	// 	if( write && !wxFile::Access( actFile, wxFile::write))
	// 	  continue;

	if( read && access( actFile, R_OK) != 0)
	  continue;
	if( write && access( actFile, W_OK) != 0)
	  continue;

	if( zero_length && statStruct.st_size != 0) 
	  continue;
#ifndef _MSC_VER
	if( executable && access( actFile, X_OK) != 0)
	  continue;

	if( get_mode)
	  (*getMode)[ f] = statStruct.st_mode & 
	    (S_IRWXU | S_IRWXG | S_IRWXO);
	if( block_special && S_ISBLK(statStruct.st_mode) == 0) 
	  continue;
	if( character_special && S_ISCHR(statStruct.st_mode) == 0) 
	  continue;
	if( named_pipe && S_ISFIFO(statStruct.st_mode) == 0) 
	  continue;
	if( socket && S_ISSOCK(statStruct.st_mode) == 0) 
	  continue;
	if( symlink && S_ISLNK(statStruct.st_mode) == 0) 
	  continue;
#endif
	if( directory && S_ISDIR(statStruct.st_mode) == 0) 
	  continue;
	if( regular && S_ISREG(statStruct.st_mode) == 0) 
	  continue;
	
	(*res)[ f] = 1;
      }

    return res;
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
//     if( dir[0] == '~')
//       {
// 	char* homeDir = getenv( "HOME");
// 	if( homeDir != NULL)
// 	  {
// 	    dir = string( homeDir) + dir.substr(1);
// 	  }
//       }

    int success = chdir( dir.c_str());
    if( success != 0)
      e->Throw( "Unable to change current directory to: "+dir+".");
  }

//   void FileSearchWE( FileListT& fL, const DString& s)
//   {
//     wordexp_t p;
//     int wERes;
//     if( s == "")
//       wERes = wordexp( "*", &p, WRDE_NOCMD);
//     else
//       wERes = wordexp( s.c_str(), &p, WRDE_NOCMD);
//     if( wERes == 0)
//       for( SizeT f=0; f<p.we_wordc; ++f)
// 	fL.push_back( p.we_wordv[ f]);
//     wordfree( &p);
//   }

  bool FindInDir( const DString& dirN, const DString& pat)
  {
    DString root = dirN;
    AppendIfNeeded( root, "/");

    DIR* dir = opendir( dirN.c_str());
    if( dir == NULL) return false;

    struct stat    statStruct;

    for(;;)
      {
	struct dirent* entry = readdir( dir);
	if( entry == NULL) break;
	
	DString entryStr( entry->d_name);
	if( entryStr != "." && entryStr != "..")
	  {
	    DString testFile = root + entryStr;
#ifdef _MSC_VER
	    int actStat = stat( testFile.c_str(), &statStruct);
#else
	    int actStat = lstat( testFile.c_str(), &statStruct);
#endif
	    if( S_ISDIR(statStruct.st_mode) == 0)
	      { // only test non-dirs
#ifdef _MSC_VER
		int match = PathMatchSpecEx(entryStr.c_str(), pat.c_str(), 0);
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
		    bool all_dirs)
  {
    // expand "+"
	    
    int fnFlags = 0;

    //    fnFlags |= FNM_PERIOD;
    //    fnFlags |= FNM_NOESCAPE;

    DString root = dirN;
    AppendIfNeeded( root, "/");
    
    struct stat    statStruct;

    FileListT recurDir;
    
    bool notAdded = true;

    DIR* dir = opendir( dirN.c_str());
    if( dir == NULL) return;

    if( all_dirs)
      notAdded = false;
    
    for(;;)
      {
	struct dirent* entry = readdir( dir);
	if( entry == NULL) break;

	DString entryStr( entry->d_name);
	if( entryStr != "." && entryStr != "..")
	  {
	    DString testDir = root + entryStr;
#ifdef _MSC_VER
	    int actStat = stat( testDir.c_str(), &statStruct);
#else
	    int actStat = lstat( testDir.c_str(), &statStruct);
#endif
	    if( S_ISDIR(statStruct.st_mode) != 0)
	      {
		recurDir.push_back( testDir);
	      }
	    else if( notAdded)
	      {
#ifdef _MSC_VER
		int match = PathMatchSpec(entryStr.c_str(), pat.c_str());
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
    for( SizeT d=0; d<nRecur; ++d)
      {
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

#ifdef _MSC_VER
	// Windows does not use '~' as a home directory alias
	DString initDir = dirN;
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

    SizeT d;
    long   sPos=0;
    do
      {
	d=s.find(':',sPos);
	string act = s.substr(sPos,d-sPos);
	
	ExpandPath( sArr, act, "*.pro", all_dirs);
	
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
      cat += ":" + sArr[i];
    
    return new DStringGDL( cat);
  }


  void PatternSearch( FileListT& fL, const DString& dirN, const DString& pat,
		      bool accErr,
		      bool quote,
		      bool match_dot,
		      const DString& prefixIn)
  {
    int fnFlags = 0;

#ifndef _MSC_VER
    if( !match_dot)
      fnFlags |= FNM_PERIOD;

    if( !quote)
      fnFlags |= FNM_NOESCAPE;
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
      }

     DString prefix = root;
//     DString prefix = prefixIn;
//     if( prefix != "")
//       {
// 	 long endR; 
// 	 for( endR = prefix.length()-1; endR >= 0; --endR)
// 	   {
// 	     if( prefix[ endR] != '/')
// 	       break;
// 	   }
// 	 if( endR >= 0)
// 	   prefix = prefix.substr( 0, endR+1) + "/";
// 	 else
// 	   prefix = "/";
//       }

    struct stat    statStruct;

    FileListT recurDir;
    
    DIR* dir;
    if( root != "")
      dir = opendir( dirN.c_str());
    else
      dir = opendir( ".");
    if( dir == NULL)
      if( accErr)
	throw GDLException( "FILE_SEARCH: Error opening dir: "+dirN);
      else
	return;

    for(;;)
      {
	struct dirent* entry = readdir( dir);
	if( entry == NULL)
	  break;

	DString entryStr( entry->d_name);
	if( entryStr != "." && entryStr != "..")
	  {
	    if( root != "") // dirs for current ("") already included
	      {
		DString testDir = root + entryStr;
#ifdef _MSC_VER
		int actStat = stat( testDir.c_str(), &statStruct);
#else
		int actStat = lstat( testDir.c_str(), &statStruct);
#endif
		if( S_ISDIR(statStruct.st_mode) != 0)
		    recurDir.push_back( testDir);
	      }

	    // dirs are also returned if they match
#ifdef _MSC_VER
	    int match = PathMatchSpec(entryStr.c_str(), pat.c_str());
#else
	    int match = fnmatch( pat.c_str(), entryStr.c_str(), fnFlags);
#endif
	    if( match == 0)
	      fL.push_back( prefix + entryStr);
	  }
      }

    int c = closedir( dir);
    if( c == -1)
      if( accErr)
	throw GDLException( "FILE_SEARCH: Error closing dir: "+dirN);
      else
	return;

    // recursive search
    SizeT nRecur = recurDir.size();
    for( SizeT d=0; d<nRecur; ++d)
      {
	//	cout << "Recursive looking in: " << recurDir[d] << endl;
	PatternSearch( fL, recurDir[d], pat, accErr, quote, 
		       match_dot,
		       /*prefix +*/ recurDir[d]);
      }
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
				for(size_t ii=i;ii<s.size();ii++) // Looking for matching right bracket
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

#ifndef _MSC_VER
  void FileSearch( FileListT& fL, const DString& s, 
		   bool environment,
		   bool tilde,
		   bool accErr,
		   bool mark,
		   bool noSort,
		   bool quote,
		   bool dir,
		   bool period,
                   bool forceAbsPath,
		   bool fold_case)
  {
    int flags = 0;
    DString st;

    if( environment)
      flags |= GLOB_BRACE;
    
    if( tilde)
      flags |= GLOB_TILDE;

    if( accErr)
      flags |= GLOB_ERR;
    
    if( mark && !dir) // only mark directory if not in dir mode
      flags |= GLOB_MARK;

    if( noSort)
      flags |= GLOB_NOSORT;

#if !defined(__APPLE__) && !defined(__FreeBSD__)
    if( !quote) // n/a on OS X
      flags |= GLOB_NOESCAPE;

    if( dir) // simulate with lstat()
      flags |= GLOB_ONLYDIR;

    if( period) // n/a on OS X
      flags |= GLOB_PERIOD;
#else
    struct stat    statStruct;
#endif
    if( fold_case)
	st=makeInsensitive(s);
   else
	st=s;

    glob_t p;
    int gRes;
    if (!forceAbsPath)
    {
      if (st != "") gRes = glob(st.c_str(), flags, NULL, &p);
      else gRes = glob("*", flags, NULL, &p);
    }
    else 
    {
      string pattern;
      if 
      (
        st.at(0) != '/' && 
        !(tilde && st.at(0) == '~') && 
        !(environment && st.at(0) == '$')
      ) 
      { 
        pattern = GetCWD();
        pattern.append("/");
        pattern.append(s);
	if(!( st.size() ==1 && st.at(0) == '.')) pattern.append(st);
        gRes = glob(pattern.c_str(), flags, NULL, &p);
      }
      else 
      {
        gRes = glob(st.c_str(), flags, NULL, &p);
      }
    }

#ifndef __APPLE__
    if( accErr && (gRes == GLOB_ABORTED || gRes == GLOB_NOSPACE))
      throw GDLException( "FILE_SEARCH: Read error: "+s);
#else
    if( accErr && (gRes != 0 && p.gl_pathc > 0)) // NOMATCH is no error
      throw GDLException( "FILE_SEARCH: Read error: "+s);
#endif      

    if( gRes == 0)
      for( SizeT f=0; f<p.gl_pathc; ++f)
	{
#ifndef __APPLE__
	  fL.push_back( p.gl_pathv[ f]);
#else
	  if( !dir)
	    fL.push_back( p.gl_pathv[ f]);
	  else
	    { // push only if dir
	      int actStat = lstat( p.gl_pathv[ f], &statStruct);
	      if( S_ISDIR(statStruct.st_mode) != 0)
		fL.push_back( p.gl_pathv[ f]);
	    }
#endif      
	}
    globfree( &p);

    if( st == "" && dir)
      fL.push_back( "");
  }

  // not finished yet
  BaseGDL* file_search( EnvT* e)
  {
    SizeT nParam=e->NParam(); // 0 -> "*"
    
    DStringGDL* pathSpec;
    DString     recurPattern;

    SizeT nPath = 0;

    if( nParam > 0)
      {
	BaseGDL* p0 = e->GetParDefined( 0);
	pathSpec = dynamic_cast<DStringGDL*>( p0);
	if( pathSpec == NULL)
	  e->Throw( "String expression required in this context.");

	nPath = pathSpec->N_Elements();

	if( nParam > 1)
	  e->AssureScalarPar< DStringGDL>( 1, recurPattern);
      }

    // unix defaults
    bool tilde = true;
    bool environment = true;
    bool fold_case = false;

    // keywords
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

    if( nPath == 0)
      FileSearch( fileList, "", 
		  environment, tilde, 
		  accErr, mark, noSort, quote, onlyDir, match_dot, forceAbsPath, fold_case);
    else
      FileSearch( fileList, (*pathSpec)[0],
		  environment, tilde, 
		  accErr, mark, noSort, quote, onlyDir, match_dot, forceAbsPath, fold_case);
    
    for( SizeT f=1; f < nPath; ++f) 
      FileSearch( fileList, (*pathSpec)[f],
		  environment, tilde, 
		  accErr, mark, noSort, quote, onlyDir, match_dot, forceAbsPath, fold_case);

    DLong count = fileList.size();

    if( onlyDir)
      { // recursive search for recurPattern
	FileListT fileOut;
	
	for( SizeT f=0; f<count; ++f) // ok for count == 0
	  {
	    //	    cout << "Looking in: " << fileList[f] << endl;
	    PatternSearch( fileOut, fileList[f], recurPattern, accErr, quote,
			   match_dot,
			   fileList[f]);
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
#endif

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
      if (!e->KeywordSet(e->KeywordIx("NOEXPAND_PATH"))) 
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
      struct stat statStruct;
      dev_t file0dev;
      ino_t file0ino;    
      int ret = stat(file0, &statStruct);
      if (ret != 0) continue;
      file0dev = statStruct.st_dev;
      file0ino = statStruct.st_ino;
      ret = stat(file1, &statStruct);
      if (ret != 0) continue;
      (*res)[i] = (file0dev == statStruct.st_dev && file0ino == statStruct.st_ino);

    }

    return res;

  }

  BaseGDL* file_info( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    DStringGDL* p0S = dynamic_cast<DStringGDL*>(e->GetParDefined(0));
    if( p0S == NULL)
      e->Throw( "String expression required in this context: "+
		e->GetParString(0));

    bool noexpand_path = e->KeywordSet(e->KeywordIx( "NOEXPAND_PATH"));

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
          actFile = tmp.c_str();
        } 
        else actFile = (*p0S)[f].c_str();
	*(res->GetTag(tName, f)) = DStringGDL(actFile);

        // stating the file (and moving on to the next file if failed)
	struct stat statStruct;
#ifdef _MSC_VER
	if (stat(actFile, &statStruct) != 0) continue;
#else
	if (lstat(actFile, &statStruct) != 0) continue;
#endif
        // checking struct tag indices (once)
        if (!indices_known) 
        {
          tExists =           res->Desc()->TagIndex("EXISTS"); 
          tRead =             res->Desc()->TagIndex("READ"); 
          tWrite =            res->Desc()->TagIndex("WRITE"); 
          tRegular =          res->Desc()->TagIndex("REGULAR"); 
          tDirectory =        res->Desc()->TagIndex("DIRECTORY");
#ifndef _MSC_VER
          tBlockSpecial =     res->Desc()->TagIndex("BLOCK_SPECIAL");
          tCharacterSpecial = res->Desc()->TagIndex("CHARACTER_SPECIAL");
          tNamedPipe =        res->Desc()->TagIndex("NAMED_PIPE");
          tExecute =          res->Desc()->TagIndex("EXECUTE"); 
          tSetuid =           res->Desc()->TagIndex("SETUID");
          tSetgid =           res->Desc()->TagIndex("SETGID");
		  tSocket =           res->Desc()->TagIndex("SOCKET");
          tStickyBit =        res->Desc()->TagIndex("STICKY_BIT");
          tSymlink =          res->Desc()->TagIndex("SYMLINK");
          tDanglingSymlink =  res->Desc()->TagIndex("DANGLING_SYMLINK");
          tMode =             res->Desc()->TagIndex("MODE");
#endif
          tAtime =            res->Desc()->TagIndex("ATIME");
          tCtime =            res->Desc()->TagIndex("CTIME");
          tMtime =            res->Desc()->TagIndex("MTIME");
          tSize =             res->Desc()->TagIndex("SIZE");
          indices_known = true;
        }

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
        *(res->GetTag(tDirectory, f)) =        DByteGDL(S_ISDIR( statStruct.st_mode) != 0);
#ifndef _MSC_VER
        *(res->GetTag(tBlockSpecial, f)) =     DByteGDL(S_ISBLK( statStruct.st_mode) != 0);
        *(res->GetTag(tCharacterSpecial, f)) = DByteGDL(S_ISCHR( statStruct.st_mode) != 0);
        *(res->GetTag(tNamedPipe, f)) =        DByteGDL(S_ISFIFO(statStruct.st_mode) != 0);
        *(res->GetTag(tSocket, f)) =           DByteGDL(S_ISSOCK(statStruct.st_mode) != 0);
#endif  
        // SETUID, SETGID, STICKY_BIT
#ifndef _MSC_VER
        *(res->GetTag(tSetuid, f)) =           DByteGDL((S_ISUID & statStruct.st_mode) != 0);
        *(res->GetTag(tSetgid, f)) =           DByteGDL((S_ISGID & statStruct.st_mode) != 0);
        *(res->GetTag(tStickyBit, f)) =        DByteGDL((S_ISVTX & statStruct.st_mode) != 0);
        // MODE
        *(res->GetTag(tMode, f)) = DLongGDL(
          statStruct.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX)
        );
#endif

        // ATIME, CTIME, MTIME
        *(res->GetTag(tAtime, f)) = DLong64GDL(statStruct.st_atime);
        *(res->GetTag(tCtime, f)) = DLong64GDL(statStruct.st_ctime);
        *(res->GetTag(tMtime, f)) = DLong64GDL(statStruct.st_mtime);

        // SIZE
	*(res->GetTag(tSize, f)) = DLong64GDL(statStruct.st_size);

        // SYMLINK, DANLING_SYMLINK
#ifndef _MSC_VER // No symlinks in windows
        if (S_ISLNK(statStruct.st_mode) != 0)
        {
          *(res->GetTag(tSymlink, f)) = DByteGDL(1);
          // warning: statStruct now describes the linked file
          *(res->GetTag(tDanglingSymlink, f)) = DByteGDL(stat(actFile, &statStruct) != 0);
        }
#endif
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
#ifdef _MSC_VER
    string cmd = "md"; // windows always creates all of the non-existing directories
#else
    string cmd = "mkdir -p";
#endif
    for (int i=0; i<nParam; i++)
    {
      DStringGDL* pi = dynamic_cast<DStringGDL*>(e->GetParDefined(i));
      for (int j=0; j<pi->N_Elements(); j++)
      {
        string tmp = (*pi)[j];
        if (!noexpand_path) WordExp(tmp);
        cmd.append(" " + tmp);
      }
    }
#ifndef _MSC_VER
    cmd.append(" 2>&1 | awk '{print \"% FILE_MKDIR: \" $0; exit 1}'");
#endif
    // SA: calling system(), mkdir and awk is surely not the most efficient way, 
    //     but copying a bunch of code from coreutils does not seem elegant either
    if (system(cmd.c_str()) != 0) e->Throw("failed to create a directory (or execute mkdir).");
  }

}

//#endif
