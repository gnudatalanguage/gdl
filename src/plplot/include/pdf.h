//  Copyright (C) 1992 by Maurice J. LeBrun
//
//  Macros and prototypes for the PDF package.
//
//  This software may be freely copied, modified and redistributed without
//  fee provided that this copyright notice is preserved intact on all
//  copies and modified copies.
//
//  There is no warranty or other guarantee of fitness of this software.
//  It is provided solely "as is". The author(s) disclaim(s) all
//  responsibility and liability with respect to this software's usage or
//  its effect upon hardware or computer systems.
//

#ifndef __PDF_H__
#define __PDF_H__

//--------------------------------------------------------------------------
// dll functions
//--------------------------------------------------------------------------
#include "pldll.h"

// Some unsigned types

#ifndef U_CHAR
#define U_CHAR    unsigned char
#endif

#ifndef U_SHORT
#define U_SHORT    unsigned short
#endif

#ifndef U_INT
#define U_INT    unsigned int
#endif

#ifndef U_LONG
#define U_LONG    unsigned long
#endif

#ifdef PLPLOT_USE_TCL_CHANNELS
#include <tcl.h>
#endif

// PDFstrm definition
// The low level PDF i/o routines use the transfer method appropriate for
// the first non-null type below

typedef struct
{
    FILE          *file;                // Filesystem
    unsigned char *buffer;              // Memory buffer
#ifdef PLPLOT_USE_TCL_CHANNELS
    Tcl_Channel   tclChan;              // Tcl channel
#endif
    size_t        bp, bufmax;           // Buffer pointer and max size
} PDFstrm;

// Info for the i/o device.  Only used by Tcl/TK/dp drivers for now

typedef struct
{
    int        fd;                      // I/O device file descriptor
    FILE       *file;                   // File handle
    const char *fileName;               // Fifo or socket name (if needed)
    const char *fileHandle;             // Handle for use from interpreter
    int        type;                    // Communication channel type
    const char *typeName;               // As above, but in string form
} PLiodev;

// Error numbers

#define PDF_ERROR       1               // Unknown error
#define PDF_FNOPEN      2               // File not open
#define PDF_FAOPEN      3               // File already open
#define PDF_BADUN       4               // Bad unit number
#define PDF_BADNBITS    5               // Invalid # of bits
#define PDF_RDERR       6               // Read error
#define PDF_WRERR       7               // Write error
#define PDF_NOTPDF      8               // Not a valid PDF file

// Prototypes
// Use a wrapper for the prototypes for use from K&R C

void pdf_set( char *option, int value );
PLDLLIMPEXP PDFstrm *pdf_fopen( const char *fileName, const char *mode );
PLDLLIMPEXP PDFstrm *pdf_bopen( U_CHAR * buffer, size_t bufmax );
PLDLLIMPEXP PDFstrm *pdf_finit( FILE * file );
PDFstrm *plLibOpenPdfstrm( const char * fn );
PLDLLIMPEXP int pdf_close( PDFstrm * pdfs );

int pdf_putc( int c, PDFstrm * pdfs );
PLDLLIMPEXP int pdf_getc( PDFstrm * pdfs );
PLDLLIMPEXP int pdf_ungetc( int c, PDFstrm * pdfs );
int pdf_rdx( U_CHAR * x, long nitems, PDFstrm * pdfs );

PLDLLIMPEXP int pdf_rd_header( PDFstrm * pdfs, char *header );
PLDLLIMPEXP int pdf_wr_header( PDFstrm * pdfs, const char *header );
int pdf_wr_string( PDFstrm * pdfs, const char *string );
int pdf_rd_string( PDFstrm * pdfs, char *string, int nmax );
PLDLLIMPEXP int pdf_wr_1byte( PDFstrm * pdfs, U_CHAR s );
PLDLLIMPEXP int pdf_rd_1byte( PDFstrm * pdfs, U_CHAR * ps );
PLDLLIMPEXP int pdf_wr_2bytes( PDFstrm * pdfs, U_SHORT s );
PLDLLIMPEXP int pdf_rd_2bytes( PDFstrm * pdfs, U_SHORT * ps );
PLDLLIMPEXP int pdf_wr_2nbytes( PDFstrm * pdfs, U_SHORT * s, PLINT n );
PLDLLIMPEXP int pdf_rd_2nbytes( PDFstrm * pdfs, U_SHORT * s, PLINT n );
PLDLLIMPEXP int pdf_wr_4bytes( PDFstrm * pdfs, U_LONG s );
PLDLLIMPEXP int pdf_rd_4bytes( PDFstrm * pdfs, U_LONG * ps );
PLDLLIMPEXP int pdf_wr_ieeef( PDFstrm * pdfs, float f );
PLDLLIMPEXP int pdf_rd_ieeef( PDFstrm * pdfs, float *pf );

#endif  // __PDF_H__
