// Copyright (C) 2015-2017 Phil Rosenberg
// Copyright (C) 2017 Alan W. Irwin
// Copyright (C) 2008 Werner Smekal

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

#ifndef __PL_WXWIDGETS_COMMS__
#define __PL_WXWIDGETS_COMMS__

#include "plplotP.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#endif

#include <wx/font.h>
#include "wxPLplot_nanosec.h"

//data transmission codes
const unsigned char transmissionRegular          = 0;
const unsigned char transmissionSkipFileEnd      = 1;
const unsigned char transmissionEndOfPage        = 2;
const unsigned char transmissionBeginPage        = 3;
const unsigned char transmissionLocate           = 4;
const unsigned char transmissionPartial          = 5;
const unsigned char transmissionComplete         = 6;
const unsigned char transmissionRequestTextSize  = 7;
const unsigned char transmissionEndOfPageNoPause = 8;
const unsigned char transmissionClose            = 9;
#ifdef PL_WXWIDGETS_IPC3
const unsigned char transmissionFlush = 10;
#endif

#define TEXTSIZEINFO_MAX_LENGTH    500

struct TextSizeInfo
{
    long         width;
    long         height;
    long         depth;
    long         leading;
    wxFontFamily family;
    int          style;
    int          weight;
    int          pt;
    bool         underlined;
    wchar_t      text[TEXTSIZEINFO_MAX_LENGTH + 1];
    bool         written;
};

struct MemoryMapHeader
{
    size_t        viewerOpenFlag;
    size_t        locateModeFlag;
    size_t        completeFlag;
#ifdef PL_WXWIDGETS_IPC3
    size_t        plbufAmountToTransmit;
    unsigned char transmissionType;
#else
    size_t        readLocation;
    size_t        writeLocation;
#endif
    PLGraphicsIn  graphicsIn;
    TextSizeInfo  textSizeInfo;
};

#ifdef PL_WXWIDGETS_IPC3
// Tuned such that total length of time to do all 35 C examples
// (except for 17 and 20) with PLPLOT_WX_NOPLOT #defined in
// utils/wxplframe.cpp is not increased significantly beyond the
// measurement noise of ~2 per cent between identical runs.  In fact
// sizes of 1024*1024 and 1024 were not different by more than the ~2
// per cent measurement noise and a size of 128 was only ~4 per cent
// larger than 1024.  So 10 * 1024 should already be gross overkill.
#define PL_SHARED_ARRAY_SIZE    10 * 1024

// In the three-semaphores method of IPC, the shared memory area must
// correspond to this shmbuf struct which contains some control data
// explicitly used for the communication, e.g., at least the total
// number of bytes of data to be transferred, and limited size
// data areas to be transferred under three-semaphore control.
struct shmbuf
{
    size_t          nbytes;           // Total number of data bytes to be transferred
    // header data to be transferred under three-semaphore control.
    MemoryMapHeader header;
    // plbuf data to be transferred under three-semaphore control.
    char            data[PL_SHARED_ARRAY_SIZE];
};

class PLThreeSemaphores
{
public:
    // Default constructor: Initialize m_wsem, m_rsem, and m_tsem to
    // NULL to mark those as invalid semaphore locations.
    PLThreeSemaphores();
    // Named semaphores.
    // Create three semaphore names from basename, and open and (only
    // on creation which happens automatically for both the Windows
    // and POSIX API cases) initialize the corresponding named
    // semaphores with the read and write semaphores initially blocked
    // and the transmit semaphore initially unblocked.
    void initializeToValid( const char * baseName );
    // call initializeToInvalid.
    ~PLThreeSemaphores();
    // If the m_wsem, m_rsem, and m_tsem locations are non-NULL
    // destroy those semaphores.  Also, unconditionally set
    // m_wsem, m_rsem, and m_tsem to NULL to mark those as invalid
    // semaphore locations.
    void initializeToInvalid();
    bool isWriteSemaphoreValid();
    bool isReadSemaphoreValid();
    bool isTransmitSemaphoreValid();
    // Return true if all semaphores are valid.
    // Return false if all semaphores are invalid.
    // Throw an exception otherwise.
    bool areSemaphoresValid();
    // Check whether write and read semaphores are valid and blocked.
    bool areWriteReadSemaphoresBlocked();
#ifndef _WIN32
    // Get value of Write semaphore.
    int getValueWriteSemaphore();
    // Get value of Read semaphore.
    int getValueReadSemaphore();
#endif // #ifndef _WIN32
    void postWriteSemaphore();
    void postReadSemaphore();
    void postTransmitSemaphore();
    void waitWriteSemaphore();
    void waitReadSemaphore();
    void waitTransmitSemaphore();
private:
    // Attempts to test semaphore initialization validity using
    // sem_getvalue on Linux proved fruitless since as far as I can tell
    // with gdb that function always returns zero, i.e., always signals
    // success, and returns a 0 or 1 value argument ___so long as its
    // sem_t * argument points to _any_ accessible memory area that is
    // cast to sem_t *__!  So here is the alternative plan we are using:
    // m_wsem, m_rsem, and m_tsem should always be NULL unless the non-NULL
    // locations they point to are properly initialized semaphores.
#define PL_SEMAPHORE_NAME_LENGTH    250 // Maximum length excluding terminating NULL byte
    char m_wsemName[PL_SEMAPHORE_NAME_LENGTH + 1];
    char m_rsemName[PL_SEMAPHORE_NAME_LENGTH + 1];
    char m_tsemName[PL_SEMAPHORE_NAME_LENGTH + 1];
#ifdef _WIN32
    // Windows named semaphores.
    HANDLE m_wsem;
    HANDLE m_rsem;
    HANDLE m_tsem;
#else // #ifdef _WIN32
      // POSIX named semaphores.
    sem_t *m_wsem;
    sem_t *m_rsem;
    sem_t *m_tsem;
#endif // #ifdef _WIN32
};

#endif //#ifdef PL_WXWIDGETS_IPC3

const PLINT plMemoryMapReservedSpace = sizeof ( MemoryMapHeader );

class PLMemoryMap
{
public:
    PLMemoryMap();
    PLMemoryMap( const char *name, PLINT size, bool mustExist, bool mustNotExist );
    void create( const char *name, PLINT size, bool mustExist, bool mustNotExist );
    void close();
    ~PLMemoryMap();
    bool isValid() { return m_buffer != NULL; }
#ifdef PL_WXWIDGETS_IPC3
    char *getBuffer() { return ( (shmbuf *) m_buffer )->data; }
    MemoryMapHeader *getHeader() { return &( ( (shmbuf *) m_buffer )->header ); }
    void initializeSemaphoresToValid( const char *baseName ) { m_threeSemaphores.initializeToValid( baseName ); }
    size_t *getTotalDataBytes() { return &( ( (shmbuf *) m_buffer )->nbytes ); }
    size_t getSize() { return PL_SHARED_ARRAY_SIZE; }
    void transmitBytes( bool ifHeader, const void *src, size_t n );
    void receiveBytes( bool ifHeader, void *dest, size_t n );
#else // #ifdef PL_WXWIDGETS_IPC3
    char *getBuffer() { return (char *) m_buffer; }
    size_t getSize() { return m_size; }
#endif // #ifdef PL_WXWIDGETS_IPC3
private:
#ifdef _WIN32
    HANDLE m_mapFile;
#else
    int m_mapFile;
    char * m_name;
#endif
#ifdef PL_WXWIDGETS_IPC3
    // instantiate m_threeSemaphores private object (with default
    // constructor) when PLMemoryMap is instantiated.
    PLThreeSemaphores m_threeSemaphores;
#endif
    // Size of shared memory buffer
    size_t m_size;
    void *m_buffer;
};

#ifndef PL_WXWIDGETS_IPC3

class PLNamedMutex
{
public:
    PLNamedMutex();
    ~PLNamedMutex();
    PLNamedMutex( const char *name, bool aquireOnCreate = false );
    void create( const char *name, bool aquireOnCreate  = false );
    void clear();
    void aquire();
    bool aquire( unsigned long millisecs );
    bool aquireNoWait();
    void release();
    bool isValid();
private:
    bool m_haveLock;
#ifdef _WIN32
    HANDLE m_mutex;
#else
    sem_t * m_mutex;
    char m_mutexName[251];
#endif
};

class PLNamedMutexLocker
{
public:
    PLNamedMutexLocker( PLNamedMutex *mutex );
    ~PLNamedMutexLocker();
private:
    PLNamedMutex *m_mutex;
    //remove default constructors
    PLNamedMutexLocker();
    PLNamedMutexLocker( const PLNamedMutexLocker & );
    PLNamedMutexLocker & operator =( const PLNamedMutex & );
};
#endif //ifndef PL_WXWIDGETS_IPC3

#endif
