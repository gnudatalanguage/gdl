// Standardized I/O handler for PLplot.
//
// Copyright (C) 2006  Jim Dishaw
// Copyright (C) 2006  Hazen Babcock
// Copyright (C) 2016  Alan W. Irwin
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#define DEBUG
#define NEED_PLDEBUG
#include "plplotP.h"

// These three are needed for open.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// For Visual C++ 2005 and later mktemp() and open() are deprecated (see
// http://msdn.microsoft.com/en-us/library/ms235413.aspx and
// http://msdn.microsoft.com/en-us/library/ms235491.aspx). mktemp()
// is redefined to _mktemp() as well as open() to _open(). In addition
// we need to include io.h.
//
#if defined ( _MSC_VER ) && _MSC_VER >= 1400
#include <io.h>
#define mktemp    _mktemp
#define open      _open
#define fdopen    _fdopen
#endif

// AM: getcwd has a somewhat strange status on Windows, its proper
// name is _getcwd, this is a problem in the case of DLLs, like with
// the Java bindings. The function _getcwd() is
// declared in direct.h for Visual C++.
#if defined ( _MSC_VER )
#  include <direct.h>
#  define getcwd    _getcwd
#endif

// Static function declaration
static PLINT
get_tmpdir_list( PLINT max_ntmpdir_list, char ** tmpdir_list, PLINT maxcurrdir, char * currdir );

//
// plio_write()
//
// Writes the contents of buf to stream.  Handles any I/O error conditions
// so that the caller can "fire and forget."
//

void
plio_fwrite( void *buf, size_t size, size_t nmemb, FILE *stream )
{
    dbug_enter( "plio_fwrite" );

    // Exit if there is nothing to write
    if ( size == 0 || nmemb == 0 )
        return;

    // Clear the error flag for this steam
    clearerr( stream );

    fwrite( buf, size, nmemb, stream );

    if ( ferror( stream ) )
    {
        // Perhaps we can add a flag (global or per output stream)
        // in order to decide if we should abort or warn.  I think
        // I/O errors should generate an abort
        plabort( "Error writing to file" );
    }
}

//
// plio_fread()
//
// Read from stream into buf.  Like plio_write(), this function will
// handle any I/O error conditions.
//

void
plio_fread( void *buf, size_t size, size_t nmemb, FILE *stream )
{
    size_t bytes;

    dbug_enter( "plio_fread" );

    // If the buffer has a size of zero, we should complain
    if ( size == 0 || nmemb == 0 )
    {
        plwarn( "Zero length buffer size in plio_fread, returning" );
        return;
    }

    // Clear the error flag for this steam
    clearerr( stream );

    bytes = fread( buf, size, nmemb, stream );

    if ( ( bytes < nmemb ) && ferror( stream ) )
    {
        // The read resulted in an error
        plabort( "Error reading from file" );
    }
}

//
// plio_fgets()
//
// Read from stream into buf.  This version of fgets is designed for the occasions
// where the caller wants to ignore the return value.
//
// NOTE: If one is reading from a file until an EOF condition, fgets() is better suited
// than this function, i.e.
//
//     while(fgets(buf, size, fp) != NULL) { ... do some stuff ... }
//
// rather than
//
//     while(!feof(fp)) { plio_fgets(buf, size, fp);  ... do some stuff ... }
//
// which would require checking for an empty buffer.
//

void
plio_fgets( char *buf, int size, FILE *stream )
{
    char *s;

    dbug_enter( "plio_fgets" );

    // If the buffer has a size of zero, we should complain
    if ( size == 0 )
    {
        plwarn( "Zero length buffer size in plio_fgets, returning" );
        return;
    }

    // Clear the error flag for this steam
    clearerr( stream );

    s = fgets( buf, size, stream );

    if ( s == NULL && ferror( stream ) )
    {
        // The read resulted in an error
        plabort( "Error reading from file" );
    }
}

// Get list of possible directories for creating temporary files.
// This list of directories is as follows:
// The directory names in the environment variables:
// "TMP" [Windows only]
// "TEMP" [Windows only]
// "TMPDIR" [Unix only]
// The value of the P_tmpdir macro as defined in <stdio.h> [Unix only]
// The current working directory
// Some specific hard-coded locations that are guessed for the
// Unix and Windows cases.

static PLINT
get_tmpdir_list( PLINT max_ntmpdir_list, char ** tmpdir_list, PLINT maxcurrdir, char * currdir )
{
    PLINT ntmpdir_list = 0;
#if defined ( MSDOS ) || defined ( _WIN32 )
    tmpdir_list[ntmpdir_list] = getenv( "TMP" );
    if ( tmpdir_list[ntmpdir_list] != NULL && ntmpdir_list < max_ntmpdir_list )
        ntmpdir_list++;

    tmpdir_list[ntmpdir_list] = getenv( "TEMP" );
    if ( tmpdir_list[ntmpdir_list] != NULL && ntmpdir_list < max_ntmpdir_list )
        ntmpdir_list++;
#else
    tmpdir_list[ntmpdir_list] = getenv( "TMPDIR" );
    if ( tmpdir_list[ntmpdir_list] != NULL && ntmpdir_list < max_ntmpdir_list )
        ntmpdir_list++;

// The P_tmpdir macro is defined in stdio.h on many UNIX systems - try that
#ifdef P_tmpdir
    tmpdir_list[ntmpdir_list] = P_tmpdir;
    if ( tmpdir_list[ntmpdir_list] != NULL && ntmpdir_list < max_ntmpdir_list )
        ntmpdir_list++;
#endif
#endif //#if defined ( MSDOS ) || defined ( _WIN32 )

// Add current working directory to list where the currdir char array
// space should be provided by the calling routine with size of maxcurrdir.
    if ( getcwd( currdir, maxcurrdir ) == NULL )
        plexit( "get_tmpdir_list: getcwd error" );
    tmpdir_list[ntmpdir_list] = currdir;
    if ( ntmpdir_list < max_ntmpdir_list )
        ntmpdir_list++;

#if defined ( MSDOS ) || defined ( _WIN32 )
    tmpdir_list[ntmpdir_list] = "c:\\windows\\Temp";
#else
    tmpdir_list[ntmpdir_list] = "/tmp";
#endif
    if ( ntmpdir_list < max_ntmpdir_list )
        ntmpdir_list++;
    return ntmpdir_list;
}

//
// pl_create_tempfile()
//
// Securely create a temporary file and return a file handle to it.
// This provides cross-platform compatibility and also adds some
// additional functionality over mkstemp in that it uses the
// potential temporary directory locations specified in the
// get_tmpdir_list function.
//
// The function returns the file handle.
//
// If the fname variable is not NULL, then on return it will contain
// a pointer to the full temporary file name. This will be allocated
// with malloc. It is the caller's responsibility to ensure this
// memory is free'd and to ensure the file is deleted after use.
// If fname is NULL then the file will be automatically deleted
// when it is closed.
//
FILE *
pl_create_tempfile( char **fname )
{
    FILE          *fd;
    char          *tmpdir;
    char          *template = NULL;
    PLCHAR_VECTOR tmpname   = "plplot_XXXXXX";
#ifndef PL_HAVE_MKSTEMP
    int           flags;
#endif
    PLINT         ntmpdir_list;
#define PL_MAXNTMPDIR_LIST    5
    char          * tmpdir_list[PL_MAXNTMPDIR_LIST];
    char          currdir[PLPLOT_MAX_PATH];
    int           itmpdir;

    ntmpdir_list = get_tmpdir_list( PL_MAXNTMPDIR_LIST, tmpdir_list, PLPLOT_MAX_PATH, currdir );
    for ( itmpdir = 0; itmpdir < ntmpdir_list; itmpdir++ )
    {
        // Only guarantee is that tmpdir is not NULL and points to a NULL-terminated string.
        tmpdir = tmpdir_list[itmpdir];
        pldebug( "pl_create_tempfile", "Attempting to create temporary file in %s directory\n", tmpdir );

        // N.B. realloc ensures template is long enough so strcpy and strcat
        // are safe here.
        template = (char *) realloc( template, sizeof ( char ) * ( strlen( tmpdir ) + strlen( tmpname ) + 2 ) );
        strcpy( template, tmpdir );
#if defined ( MSDOS ) || defined ( _WIN32 )
        strcat( template, "\\" );
#else
        strcat( template, "/" );
#endif
        strcat( template, tmpname );

#ifdef PL_HAVE_MKSTEMP
        fd = fdopen( mkstemp( template ), "wb+" );
#else   //#ifdef PL_HAVE_MKSTEMP
        // This conditionally compiled code branch is only an insecure last resort
        // if mkstemp is not available.
#if !defined ( _S_IREAD )
#define _S_IREAD     256
#endif
#if !defined ( _S_IWRITE )
#define _S_IWRITE    128
#endif
        fd    = NULL;
        flags = O_RDWR | O_CREAT | O_EXCL;
#if defined ( MSDOS ) || defined ( _WIN32 )
        // These are flags that are only relevant to Windows open.
        flags = flags | O_BINARY | _O_SHORT_LIVED;
        // If we are not returning the file name then add (Windows) flag to automatically
        // delete file once all file handles are closed.
        if ( fname == NULL )
            flags = flags | _O_TEMPORARY;
#endif
        mktemp( template );
        fd = fdopen( open( template, flags, _S_IREAD | _S_IWRITE ), "wb+" );
#endif  // #ifdef PL_HAVE_MKSTEMP
        if ( fd == NULL )
            continue;
        else
            break;
    }
    if ( fd == NULL )
    {
        plwarn( "pl_create_tempfile: Unable to open temporary file - returning" );
        if ( fname != NULL )
            *fname = NULL;
        free( template );
        return NULL;
    }
#if defined ( PL_HAVE_MKSTEMP ) && defined ( PL_HAVE_UNLINK )
    // If we are not returning the file name then unlink the file so it is
    // automatically deleted.
    if ( fname == NULL )
        unlink( template );
#endif
    if ( fname != NULL )
    {
        *fname = template;
    }
    else
    {
        free( template );
    }

    return fd;
}

//
// pl_create_tempfifo()
//
// Securely create a temporary fifo and return the file name.
// This only works on POSIX compliant platforms at the moment.
// It creates a secure directory first using mkdtemp, then
// creates the named fifo in this directory. The combination of
// a private directory and mkfifo failing if the file name already exists
// makes this secure against race conditions / DoS attacks. This function
// includes additional functionality over mkdtemp in that it uses the
// potential temporary directory locations specified in the
// get_tmpdir_list function.
//
// The function returns the file name of the fifo.
//
char *
pl_create_tempfifo( const char **p_fifoname, const char **p_dirname )
{
#if !defined PL_HAVE_MKDTEMP || !defined PL_HAVE_MKFIFO
    plwarn( "Creating fifos not supported on this platform" );
    return NULL;
#else
    char          *tmpdir;
    char          *template = NULL;
    char          *dirname  = NULL;
    PLCHAR_VECTOR tmpname   = "plplot_dir_XXXXXX";
    PLCHAR_VECTOR fifoname  = "plplot_fifo";

    PLINT         ntmpdir_list;
#define PL_MAXNTMPDIR_LIST    5
    char          * tmpdir_list[PL_MAXNTMPDIR_LIST];
    char          currdir[PLPLOT_MAX_PATH];
    int           itmpdir;
    int           mkfifo_rc;

    ntmpdir_list = get_tmpdir_list( PL_MAXNTMPDIR_LIST, tmpdir_list, PLPLOT_MAX_PATH, currdir );
    for ( itmpdir = 0; itmpdir < ntmpdir_list; itmpdir++ )
    {
        // Only guarantee is that tmpdir is not NULL and points to a NULL-terminated string.
        tmpdir = tmpdir_list[itmpdir];
        pldebug( "pl_create_tempfifo", "Attempting to create temporary fifo in %s directory\n", tmpdir );

        // N.B. realloc ensures template is long enough so strcpy and strcat
        // are safe here.
        dirname = (char *) realloc( dirname, sizeof ( char ) * ( strlen( tmpdir ) + strlen( tmpname ) + 2 ) );
        strcpy( dirname, tmpdir );
#if defined ( MSDOS ) || defined ( _WIN32 )
        strcat( dirname, "\\" );
#else
        strcat( dirname, "/" );
#endif
        strcat( dirname, tmpname );
        // Create the temporary directory
        dirname = mkdtemp( dirname );
        if ( dirname == NULL )
        {
            // Mark this attempt as a failure.
            mkfifo_rc = -1;
            // This is a failed attempt so try other possibilities.
            continue;
        }
        *p_dirname = dirname;

        // Now create the fifo in the directory
        template = (char *) realloc( template, sizeof ( char ) * ( strlen( tmpdir ) + strlen( tmpname ) + strlen( fifoname ) + 4 ) );
        strcpy( template, dirname );
#if defined ( MSDOS ) || defined ( _WIN32 )
        strcat( template, "\\" );
#else
        strcat( template, "/" );
#endif
        strcat( template, fifoname );
        *p_fifoname = template;

        // Check that mkfifo succeeds safely
        mkfifo_rc = mkfifo( template, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        if ( mkfifo_rc < 0 )
            // This is a failed attempt so try other possibilities.
            continue;
        else
            break;
    }
    // Check for failure of all attempts in above loop.
    if ( mkfifo_rc < 0 )
    {
        plwarn( "mkfifo error" );
        free( template );
        *p_fifoname = NULL;
        free( dirname );
        *p_dirname = NULL;
        return NULL;
    }

    return template;
#endif
}
