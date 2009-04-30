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

#include <unistd.h>
#include <sys/stat.h>

// #include <wx/utils.h>
// #include <wx/file.h>
// #include <wx/dir.h>

#include <glob.h>
#include <dirent.h>
#include <fnmatch.h>

// workaround for HP-UX. A better solution is needed i think
#if defined(__hpux__) || defined(__sun__)
#define GLOB_TILDE 0
#define GLOB_BRACE 0
#define GLOB_ONLYDIR 0
#define GLOB_PERIOD 0
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
          actFile = tmp.c_str();
        } 
        else 
        {
          actFile = (*p0S)[f].c_str();
        }

	struct stat statStruct;
	int actStat = lstat( actFile, &statStruct);
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
	if( executable && access( actFile, X_OK) != 0)
	  continue;

	if( zero_length && statStruct.st_size != 0) 
	  continue;

	if( get_mode)
	  (*getMode)[ f] = statStruct.st_mode & 
	    (S_IRWXU | S_IRWXG | S_IRWXO);
	    
	if( directory && S_ISDIR(statStruct.st_mode) == 0) 
	  continue;
	if( regular && S_ISREG(statStruct.st_mode) == 0) 
	  continue;
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
	    int actStat = lstat( testFile.c_str(), &statStruct);
	    if( S_ISDIR(statStruct.st_mode) == 0)
	      { // only test non-dirs
		int match = fnmatch( pat.c_str(), entryStr.c_str(), 0);
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
	    int actStat = lstat( testDir.c_str(), &statStruct);
	    if( S_ISDIR(statStruct.st_mode) != 0)
	      {
		recurDir.push_back( testDir);
	      }
	    else if( notAdded)
	      {
		int match = fnmatch( pat.c_str(), entryStr.c_str(), 0);
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
    
    if( dirN[0] != '+')
      {
	result.push_back( dirN);
	return;
      }
    
    if( dirN.length() == 1) // dirN == "+" 
      return;

    // dirN == "+DIRNAME"

    // do first a glob because of '~'
    int flags = GLOB_TILDE | GLOB_NOSORT;
    glob_t p;
    int gRes = glob( dirN.substr(1).c_str(), flags, NULL, &p);
    if( gRes != 0 || p.gl_pathc == 0)
      {
	globfree( &p);
	return;
      }

    DString initDir = p.gl_pathv[ 0];
    globfree( &p);
     
    ExpandPathN( result, initDir, pat, all_dirs);
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

    if( !match_dot)
      fnFlags |= FNM_PERIOD;

    if( !quote)
      fnFlags |= FNM_NOESCAPE;

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
		int actStat = lstat( testDir.c_str(), &statStruct);
		if( S_ISDIR(statStruct.st_mode) != 0)
		    recurDir.push_back( testDir);
	      }

	    // dirs are also returned if they match
	    int match = fnmatch( pat.c_str(), entryStr.c_str(), fnFlags);
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

  void FileSearch( FileListT& fL, const DString& s, 
		   bool environment,
		   bool tilde,
		   bool accErr,
		   bool mark,
		   bool noSort,
		   bool quote,
		   bool dir,
		   bool period)
  {
    int flags = 0;

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

    glob_t p;
    int gRes;
    if( s == "")
      gRes = glob( "*", flags, NULL, &p);
    else
      gRes = glob( s.c_str(), flags, NULL, &p);

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

    if( s == "" && dir)
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

    // ...
    if( fold_case)
      Warning( "FILE_SEARCH: FOLD_CASE keyword ignored (not supported).");
    if( match_all_dot)
      Warning( "FILE_SEARCH: MATCH_ALL_INITIAL_DOT keyword ignored (not supported).");

    bool onlyDir = nParam > 1;

    FileListT fileList;

    if( nPath == 0)
      FileSearch( fileList, "", 
		  environment, tilde, 
		  accErr, mark, noSort, quote, onlyDir, match_dot);
    else
      FileSearch( fileList, (*pathSpec)[0],
		  environment, tilde, 
		  accErr, mark, noSort, quote, onlyDir, match_dot);
    
    for( SizeT f=1; f < nPath; ++f) 
      FileSearch( fileList, (*pathSpec)[f],
		  environment, tilde, 
		  accErr, mark, noSort, quote, onlyDir, match_dot);

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

}

//#endif
