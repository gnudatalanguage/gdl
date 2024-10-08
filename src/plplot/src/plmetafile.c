// PLplot metafile I/O routines.  Originally implemented in the plmeta
// driver and plrender.  The decision was made to provide the capability
// to always read/write metafiles, thus the routines have been merged
// into the core of the library.
//
// Copyright (C) 2006  Hazen Babcock
// Copyright (C) 2015  Jim Dishaw

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

#define NEED_PLDEBUG
#include "plplotP.h"
#include "metadefs.h"
#include <stddef.h>       // For the offsetof() macro

#define MAX_BUFFER    256 // Character buffer size for reading records

#if defined ( _MSC_VER ) && _MSC_VER <= 1500
// Older versions of Visual Studio (2005 perhaps 2008) do not define uint8_t
// The newer versions of Visual Studio will not install on Vista or older
// versions of Windows.
typedef unsigned char   uint8_t;
#endif

// Not all platforms support the lround() function and the coordinate system
// representation is going to be redone, thus this is just a placeholder until
// everything is sorted out.
#define PLFLT2COORD( a )    ( (short) ceil( a ) )

// Status codes
enum _plm_status
{
    PLM_READ_ERROR       = -7,
    PLM_UNKNOWN_DATATYPE = -6,
    PLM_INVALID_STATE    = -5,
    PLM_INVALID_CMD      = -4,
    PLM_FORMAT_ERROR     = -3,
    PLM_UNKNOWN_VERSION  = -2,
    PLM_NOT_PLMETA_FILE  = -1,
    PLM_SUCCESS          = 0
};

// Portable data format types (perhaps should be defined elsewhere?)
enum _pdf_types
{
    PDF_NULL = 0, // No-data type
    PDF_UBYTE,    // one-byte unsigned integer
    PDF_USHORT,   // two-byte unsigned integer
    PDF_ULONG,    // four-byte unsigned integer
    PDF_IEEEF     // IEEE 32 bit floating point
};

// Data types used by PLplot (perhaps should be defined elsewhere?)
enum _plp_types
{
    PLP_NULL = 0, // No-data type
    PLP_UBYTE,
    PLP_UCHAR,
    PLP_USHORT,
    PLP_SHORT,
    PLP_PLINT,
    PLP_PLFLT,
    PLP_ULONG,
};

// Data format entry structure
struct _plm_format
{
    char            *name;     // Name of the field
    enum _pdf_types pdf_type;  // Field data type in metafile
    enum _plp_types plp_type;  // Field data type in PLplot
    const size_t    offset;    // Byte offset in the plmeta device
};

// Metafile index information header
static const struct _plm_format index_2005_header[] = {
    { "pages", PDF_USHORT, PLP_USHORT, offsetof( PLmIndex, pages ) },
    // Last entry, do not change
    { NULL,    PDF_NULL,   PLP_NULL, 0 }
};

// Device information header
static const struct _plm_format dev_2005_header[] = {
    { "xmin", PDF_USHORT, PLP_PLINT, offsetof( PLmDev, xmin ) },
    { "xmax", PDF_USHORT, PLP_PLINT, offsetof( PLmDev, xmax ) },
    { "ymin", PDF_USHORT, PLP_PLINT, offsetof( PLmDev, ymin ) },
    { "ymax", PDF_USHORT, PLP_PLINT, offsetof( PLmDev, ymax ) },
    { "pxlx", PDF_IEEEF,  PLP_PLFLT, offsetof( PLmDev, pxlx ) },
    { "pxly", PDF_IEEEF,  PLP_PLFLT, offsetof( PLmDev, pxly ) },
    // Last entry, do not change
    { NULL,   PDF_NULL,   PLP_NULL, 0 }
};

// Plot information header
static const struct _plm_format plot_2005_header[] = {
    { "xdpi",    PDF_IEEEF,  PLP_PLFLT, offsetof( PLStream, xdpi )    },
    { "ydpi",    PDF_IEEEF,  PLP_PLFLT, offsetof( PLStream, ydpi )    },
    { "xlength", PDF_USHORT, PLP_PLINT, offsetof( PLStream, xlength ) },
    { "ylength", PDF_USHORT, PLP_PLINT, offsetof( PLStream, ylength ) },
    { "xoffset", PDF_USHORT, PLP_PLINT, offsetof( PLStream, xoffset ) },
    { "yoffset", PDF_USHORT, PLP_PLINT, offsetof( PLStream, yoffset ) },
    { "",        PDF_NULL,   PLP_NULL,                 0 },
    // Last entry, do not change
    { NULL,               0, 0 }
};

// Page information header
static const struct _plm_format page_2005_header[] = {
    { "", PDF_USHORT, PLP_PLINT, offsetof( PLmDev, page )      },
    { "", PDF_ULONG,  PLP_PLINT, offsetof( PLmDev, lp_offset ) },
    { "", PDF_ULONG,  PLP_NULL, 0 }
};

static struct _plm_version
{
    PLCHAR_VECTOR            identifier;
    const struct _plm_format *index_header;
    const struct _plm_format *device_header;
    const struct _plm_format *plot_header;
} metafile_format[] = {
    { PLMETA_VERSION,
      index_2005_header, dev_2005_header, plot_2005_header }
};

static
enum _plm_status set_ubyte_plp_value( uint8_t x,
                                      void *dest, enum _plp_types type )
{
    enum _plm_status rc = PLM_SUCCESS;

    switch ( type )
    {
    case PLP_NULL:
        break;
    case PLP_UBYTE:
        *(uint8_t *) dest = x;
        break;
    case PLP_UCHAR:
        *(unsigned char *) dest = x;
        break;
    case PLP_USHORT:
        *(U_SHORT *) dest = x;
        break;
    case PLP_SHORT:
        *(short *) dest = x;
        break;
    case PLP_PLINT:
        *(PLINT *) dest = x;
        break;
    case PLP_PLFLT:
        *(PLFLT *) dest = x;
        break;
    case PLP_ULONG:
        *(U_LONG *) dest = x;
        break;
    default:
        plwarn( "Unhandled datatype conversion in set_plp_value." );
        rc = PLM_UNKNOWN_DATATYPE;
        break;
    }

    return rc;
}

static
enum _plm_status set_ushort_plp_value( U_SHORT x,
                                       void *dest, enum _plp_types type )
{
    enum _plm_status rc = PLM_SUCCESS;

    switch ( type )
    {
    case PLP_NULL:
        break;
    case PLP_UBYTE:
        *(uint8_t *) dest = x;
        break;
    case PLP_UCHAR:
        *(unsigned char *) dest = x;
        break;
    case PLP_USHORT:
        *(U_SHORT *) dest = x;
        break;
    case PLP_SHORT:
        *(short *) dest = x;
        break;
    case PLP_PLINT:
        *(PLINT *) dest = x;
        break;
    case PLP_PLFLT:
        *(PLFLT *) dest = x;
        break;
    case PLP_ULONG:
        *(U_LONG *) dest = x;
        break;
    default:
        plwarn( "Unhandled datatype conversion in set_plp_value." );
        rc = PLM_UNKNOWN_DATATYPE;
        break;
    }

    return rc;
}

static
enum _plm_status set_ulong_plp_value( unsigned long x,
                                      void *dest, enum _plp_types type )
{
    enum _plm_status rc = PLM_SUCCESS;

    switch ( type )
    {
    case PLP_NULL:
        break;
    case PLP_UBYTE:
        *(uint8_t *) dest = x;
        break;
    case PLP_UCHAR:
        *(unsigned char *) dest = x;
        break;
    case PLP_USHORT:
        *(U_SHORT *) dest = x;
        break;
    case PLP_SHORT:
        *(short *) dest = x;
        break;
    case PLP_PLINT:
        *(PLINT *) dest = x;
        break;
    case PLP_PLFLT:
        *(PLFLT *) dest = x;
        break;
    case PLP_ULONG:
        *(U_LONG *) dest = x;
        break;
    default:
        plwarn( "Unhandled datatype conversion in set_plp_value." );
        rc = PLM_UNKNOWN_DATATYPE;
        break;
    }

    return rc;
}

static
enum _plm_status set_ieeef_plp_value( float x,
                                      void *dest, enum _plp_types type )
{
    enum _plm_status rc = PLM_SUCCESS;

    switch ( type )
    {
    case PLP_NULL:
        break;
    case PLP_UBYTE:
        *(uint8_t *) dest = x;
        break;
    case PLP_UCHAR:
        *(unsigned char *) dest = x;
        break;
    case PLP_USHORT:
        *(U_SHORT *) dest = x;
        break;
    case PLP_SHORT:
        *(short *) dest = x;
        break;
    case PLP_PLINT:
        *(PLINT *) dest = x;
        break;
    case PLP_PLFLT:
        *(PLFLT *) dest = x;
        break;
    case PLP_ULONG:
        *(U_LONG *) dest = x;
        break;
    default:
        plwarn( "Unhandled datatype conversion in set_plp_value." );
        rc = PLM_UNKNOWN_DATATYPE;
        break;
    }

    return rc;
}

static
enum _plm_status read_entry( PDFstrm *plm,
                             enum _pdf_types from_type,
                             enum _plp_types to_type,
                             void *dest )
{
    uint8_t          b;
    unsigned long    l;
    U_SHORT          x;
    float            f;
    enum _plm_status rc;
    int pdf_rc = 0;

    switch ( from_type )
    {
    case PLP_NULL:
        pdf_rc = 0;
        rc     = PLM_SUCCESS;
        break;

    case PDF_UBYTE:     // Unsigned one-byte integer
        if ( ( pdf_rc = pdf_rd_1byte( plm, &b ) ) == 0 )
            rc = set_ubyte_plp_value( b, dest, to_type );
        break;

    case PDF_USHORT:      // Unsigned two-byte integer
        if ( ( pdf_rc = pdf_rd_2bytes( plm, &x ) ) == 0 )
            rc = set_ushort_plp_value( x, dest, to_type );
        break;

    case PDF_ULONG:      // Unsigned four-byte integer
        if ( ( pdf_rc = pdf_rd_4bytes( plm, &l ) ) == 0 )
            rc = set_ulong_plp_value( l, dest, to_type );
        break;

    case PDF_IEEEF:      // IEEE 32 bit float
        if ( ( pdf_rc = pdf_rd_ieeef( plm, &f ) ) == 0 )
            rc = set_ieeef_plp_value( f, dest, to_type );
        break;

    default:
        plwarn( "Unhandled datatype conversion in read_entry." );
        rc = PLM_UNKNOWN_DATATYPE;
        break;
    }

    if ( pdf_rc == 0 )
        return rc;
    else
        return PLM_READ_ERROR;
}

static
enum _plm_status read_string( PDFstrm *plm,
                              size_t bytes,
                              char *dest )
{
    int rc;

    rc = pdf_rd_string( plm, dest, bytes );

    if ( rc == 0 )
        return PLM_SUCCESS;
    else
        return PLM_READ_ERROR;
}

//--------------------------------------------------------------------------
// read_metafile_header()
//
// Attempts to read the metafile header information in order to see if
// is a metafile and identify the version.
//--------------------------------------------------------------------------
static
enum _plm_status read_metafile_header( PDFstrm *plm, PLmDev *dev )
{
    char  buffer[MAX_BUFFER];
    PLINT version;
    PLINT num_format_entries = sizeof ( metafile_format ) / sizeof ( struct _plm_version );

    dbug_enter( "read_metafile_enter()" );

    // Attempt to identify that this is a PLplot metafile
    plm_rd( pdf_rd_header( plm, buffer ) );
    if ( strcmp( buffer, PLMETA_HEADER ) != 0 )
    {
        return PLM_NOT_PLMETA_FILE;
    }

    // Attempt to identify the version number
    plm_rd( pdf_rd_header( plm, buffer ) );
    for ( version = 0;
          version < num_format_entries
          && strcmp( metafile_format[version].identifier, buffer ) != 0;
          version++ )
        ;

    if ( version >= num_format_entries )
        return ( PLM_UNKNOWN_VERSION );

    dev->version = version;

    return PLM_SUCCESS;
}

static
void check_buffer_size( PLmDev *dev, size_t need_size )
{
    // Do we have enough space?
    if ( need_size > dev->buffer_size )
    {
        // Nope, free the current buffer if it is allocated
        if ( dev->buffer != NULL )
            free( dev->buffer );

        dev->buffer = malloc( need_size );
        if ( dev->buffer == NULL )
        {
            plexit( "plmetafie: Insufficient memory for temporary storage" );
        }
        dev->buffer_size = need_size;
    }
}

//--------------------------------------------------------------------------
// read_header()
//
// Read a header block from the plot metafile.
//
// NOTE:  Currently we enforce rigid adherence to the order
// specified by the format.  This can be changed so that the
// entries a looked up instead.
//--------------------------------------------------------------------------
static
enum _plm_status read_header( PDFstrm *plm,
                              const struct _plm_format *header,
                              uint8_t * dest )
{
    char             buffer[MAX_BUFFER];
    unsigned int     entry;
    enum _plm_status rc;

    for ( entry = 0;
          header[entry].name != NULL;
          entry++ )
    {
        // Get the name of this field and verify it is correct
        plm_rd( pdf_rd_header( plm, buffer ) );
        if ( strcmp( header[entry].name, buffer ) != 0 )
        {
            return PLM_FORMAT_ERROR;
        }

        rc = read_entry( plm,
            header[entry].pdf_type,
            header[entry].plp_type,
            dest + header[entry].offset );

        if ( rc != PLM_SUCCESS )
            return rc;
    }

    return PLM_SUCCESS;
}

//--------------------------------------------------------------------------
// read_line()
//
// Process a LINE command from the metafile
//--------------------------------------------------------------------------
static
enum _plm_status read_line( PDFstrm *plm, PLmDev *dev, PLStream *pls )
{
    PLFLT            x1, y1, x2, y2;
    short            x[2], y[2];
    enum _plm_status rc;

    // Read the start and end points
    // The metafile stores the points as x,y pairs
    rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &x1 );
    if ( rc != PLM_SUCCESS )
        return rc;
    rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &y1 );
    if ( rc != PLM_SUCCESS )
        return rc;
    rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &x2 );
    if ( rc != PLM_SUCCESS )
        return rc;
    rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &y2 );
    if ( rc != PLM_SUCCESS )
        return rc;

    // Transform the coordinates from the meta device to the current
    // device coordinate system
    x[0] = PLFLT2COORD( dev->mfpcxa * x1 + dev->mfpcxb );
    y[0] = PLFLT2COORD( dev->mfpcya * y1 + dev->mfpcyb );
    x[1] = PLFLT2COORD( dev->mfpcxa * x2 + dev->mfpcxb );
    y[1] = PLFLT2COORD( dev->mfpcya * y2 + dev->mfpcyb );

    // Draw the line
    plP_line( x, y );

    // Preserve the last XY coords for the LINETO command
    dev->xold = (short) x[1];
    dev->yold = (short) y[1];

    return PLM_SUCCESS;
}

//--------------------------------------------------------------------------
// read_lineto()
//
// Process a LINE command from the metafile
//--------------------------------------------------------------------------
static
enum _plm_status read_lineto( PDFstrm *plm, PLmDev *dev, PLStream *pls )
{
    PLFLT            x2, y2;
    short            x[2], y[2];
    int              i;
    enum _plm_status rc;

    // Set the start to the last known position
    x[0] = (PLFLT) dev->xold;
    y[0] = (PLFLT) dev->yold;

    // Read the end point
    rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &x2 );
    if ( rc != PLM_SUCCESS )
        return rc;
    rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &y2 );
    if ( rc != PLM_SUCCESS )
        return rc;

    // Transform the coordinates from the meta device to the current
    // device coordinate system
    x[1] = PLFLT2COORD( dev->mfpcxa * x2 + dev->mfpcxb );
    y[1] = PLFLT2COORD( dev->mfpcya * y2 + dev->mfpcyb );

    // Draw the line
    plP_line( x, y );

    // Preserve the last XY coords for the LINETO command
    dev->xold = (short) x[1];
    dev->yold = (short) y[1];

    return PLM_SUCCESS;
}

//--------------------------------------------------------------------------
// read_polyline()
//
// Process a POLYLINE command from the metafile
//--------------------------------------------------------------------------
static
enum _plm_status read_polyline( PDFstrm *plm, PLmDev *dev, PLStream *pls )
{
    PLINT            i, npts;
    PLFLT            x, y;
    short            *xd, *yd;
    enum _plm_status rc;

    // Read the number of control points and put into the plot buffer
    rc = read_entry( plm, PDF_USHORT, PLP_PLINT, &npts );
    if ( rc != PLM_SUCCESS )
        return rc;

    // Setup temporary storage.  We need 2 * npts to store the X,Y pairs
    check_buffer_size( dev, sizeof ( short ) * npts * 2 );
    // Setup storage for the x values and y values
    xd = (short *) ( dev->buffer );
    yd = ( (short *) ( dev->buffer ) ) + npts;

    // Read the points and insert into the plot buffer
    // The x values
    for ( i = 0; i < npts; i++ )
    {
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &x );
        if ( rc != PLM_SUCCESS )
            return rc;

        // Transform the coordinates from the meta device to the current
        // device coordinate system
        xd[i] = PLFLT2COORD( dev->mfpcxa * x + dev->mfpcxb );
    }
    // Preserve the last X value for the LINETO command
    dev->xold = xd[npts - 1];

    // The y values
    for ( i = 0; i < npts; i++ )
    {
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &y );
        if ( rc != PLM_SUCCESS )
            return rc;

        // Transform the coordinates from the meta device to the current
        // device coordinate system
        yd[i] = PLFLT2COORD( dev->mfpcya * y + dev->mfpcyb );
    }
    // Preserve the last Y value for the LINETO command
    dev->yold = yd[npts - 1];

    // Draw the line
    plP_polyline( xd, yd, npts );

    return PLM_SUCCESS;
}

//--------------------------------------------------------------------------
// read_escape()
//
// Process a escape command from the metafile
//--------------------------------------------------------------------------
static
enum _plm_status read_escape( PDFstrm *plm, PLmDev *dev, PLStream *pls )
{
    uint8_t          op;
    enum _plm_status rc = PLM_SUCCESS;

    // Read the state operation, return if an error
    if ( pdf_rd_1byte( plm, &op ) != 0 )
        return PLM_FORMAT_ERROR;

    switch ( op )
    {
    case PLESC_FILL:
    {
        PLINT i, npts;
        PLFLT x, y;
        short *xd, *yd;

        // Get the number of control points for the fill
        rc = read_entry( plm, PDF_USHORT, PLP_PLINT, &npts );
        if ( rc != PLM_SUCCESS )
            return rc;

        // Setup temporary storage.  We need 2 * npts to store the X,Y pairs
        check_buffer_size( dev, sizeof ( short ) * npts * 2 );
        // Setup storage for the x values and y values
        xd = (short *) ( dev->buffer );
        yd = ( (short *) ( dev->buffer ) ) + npts;

        for ( i = 0; i < npts; i++ )
        {
            rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &x );
            if ( rc != PLM_SUCCESS )
                return rc;

            rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &y );
            if ( rc != PLM_SUCCESS )
                return rc;

            // Transform the coordinates from the meta device to the current
            // device coordinate system
            xd[i] = PLFLT2COORD( dev->mfpcxa * x + dev->mfpcxb );
            yd[i] = PLFLT2COORD( dev->mfpcya * y + dev->mfpcyb );
        }

        plP_fill( xd, yd, npts );
    }
    break;

    case PLESC_SWIN:
        rc = PLM_SUCCESS;
        break;


    // Text handling.  The metafile contains unprocessed string
    // data
    case PLESC_HAS_TEXT:
    {
        EscText   text;
        PLFLT     xform[4];
        PLUNICODE fci, ch;
        PLINT     i;
        PLFLT     xmin, xmax, ymin, ymax;
        PLFLT     x, y, refx, refy;
        size_t    len;

        // Setup storage for the transformation matrix
        text.xform = xform;

        // Read the information from the metafile
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &pls->chrht );
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &pls->diorot );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &xmin );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &xmax );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &ymin );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &ymax );

        rc = read_entry( plm, PDF_USHORT, PLP_PLINT, &text.base );
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &text.just );
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &text.xform[0] );
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &text.xform[1] );
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &text.xform[2] );
        rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &text.xform[3] );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &x );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &y );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &refx );
        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &refy );
        rc = read_entry( plm, PDF_UBYTE, PLP_UCHAR, &text.font_face );

        // Check for a size mismatch that indicates a problem in the
        // library that the developers need to fix
        if ( sizeof ( text.text_type ) != sizeof ( U_LONG ) )
        {
            plwarn( "plmetafile:  Serious library error! text_type != U_LONG" );
            return PLM_FORMAT_ERROR;
        }
        rc = read_entry( plm, PDF_ULONG, PLP_ULONG, &text.text_type );

        // Translate coordinates to the device coordinate system
        pls->clpxmi = PLFLT2COORD( dev->mfpcxa * xmin + dev->mfpcxb );
        pls->clpxma = PLFLT2COORD( dev->mfpcxa * xmax + dev->mfpcxb );
        pls->clpymi = PLFLT2COORD( dev->mfpcxa * ymin + dev->mfpcxb );
        pls->clpyma = PLFLT2COORD( dev->mfpcxa * ymax + dev->mfpcxb );

        text.x    = PLFLT2COORD( dev->mfpcxa * x + dev->mfpcxb );
        text.y    = PLFLT2COORD( dev->mfpcya * y + dev->mfpcyb );
        text.refx = PLFLT2COORD( dev->mfpcxa * refx + dev->mfpcxb );
        text.refy = PLFLT2COORD( dev->mfpcya * refy + dev->mfpcyb );

        if ( text.text_type == PL_STRING_TEXT )
        {
            // Retrieve the text string
            rc = read_entry( plm, PDF_USHORT, PLP_ULONG, &len );

            // Add one to the length for the NUL character.  The metafile
            // format stores the NUL, so we must read it.
            len++;

            // Check that we have enough storage for the string
            check_buffer_size( dev, len * sizeof ( char ) );

            // Read the string from the metafile
            rc = read_string( plm, len, dev->buffer );

            text.string = (char *) dev->buffer;

            // Call the text rendering routine
            plP_text(
                text.base, text.just, text.xform,
                text.x, text.y,
                text.refx, text.refy,
                text.string );
        }
        else
        {
            rc = read_entry( plm, PDF_USHORT, PLP_PLINT, &text.symbol );
            plhrsh( text.symbol, text.x, text.y );
        }
    }
        rc = PLM_SUCCESS;
        break;

    // Alternate unicode text handling
    case PLESC_BEGIN_TEXT:
    case PLESC_TEXT_CHAR:
    case PLESC_CONTROL_CHAR:
    case PLESC_END_TEXT:
        // NOP these for now until a decision is made
        // which method should be implemented for metafiles
        plwarn( "plmetafile: Alternate Unicode text handling is not implemented" );
        rc = PLM_INVALID_CMD;
        break;

    default:
        break;
    }

    return rc;
}

//--------------------------------------------------------------------------
// read_state()
//
// Process a state command from the metafile
//--------------------------------------------------------------------------
static
enum _plm_status read_state( PDFstrm *plm, PLmDev *dev, PLStream *pls )
{
    uint8_t          op;
    enum _plm_status rc = PLM_SUCCESS;

    // Read the state operation, return if an error
    if ( pdf_rd_1byte( plm, &op ) != 0 )
        return PLM_FORMAT_ERROR;

    switch ( op )
    {
    case PLSTATE_WIDTH:
        pldebug( "state: WIDTH" );

        rc = read_entry( plm, PDF_USHORT, PLP_PLFLT, &( pls->width ) );
        if ( rc != PLM_SUCCESS )
            return rc;

        break;

    case PLSTATE_COLOR0:
    case PLSTATE_COLOR1:
        pldebug( "state: COLOR0/COLOR1" );

        {
            PLINT icol;

            // Read the color index number
            rc = read_entry( plm, PDF_USHORT, PLP_PLINT, &icol );
            if ( rc != PLM_SUCCESS )
                return rc;

            // Was pen 0 set to an RGB value rather than color index?
            if ( op == PLSTATE_COLOR0 && icol != PL_RGB_COLOR )
            {
                pls->icol0      = icol;
                pls->curcolor.r = pls->cmap0[icol].r;
                pls->curcolor.g = pls->cmap0[icol].g;
                pls->curcolor.b = pls->cmap0[icol].b;
                pls->curcolor.a = pls->cmap0[icol].a;
            }
            else if ( op == PLSTATE_COLOR1 )
            {
                pls->icol1      = icol;
                pls->curcolor.r = pls->cmap1[icol].r;
                pls->curcolor.g = pls->cmap1[icol].g;
                pls->curcolor.b = pls->cmap1[icol].b;
                pls->curcolor.a = pls->cmap1[icol].a;
            }
            else
            {
                // Get the RGB value and copy to the plot buffer
                PLColor color;

                rc = read_entry( plm, PDF_UBYTE, PLP_UCHAR, &color.r );
                if ( rc != PLM_SUCCESS )
                    return rc;

                rc = read_entry( plm, PDF_UBYTE, PLP_UCHAR, &color.g );
                if ( rc != PLM_SUCCESS )
                    return rc;

                rc = read_entry( plm, PDF_UBYTE, PLP_UCHAR, &color.b );
                if ( rc != PLM_SUCCESS )
                    return rc;

                pls->icol0      = icol;
                pls->curcolor.r = color.r;
                pls->curcolor.g = color.g;
                pls->curcolor.b = color.b;
                pls->curcolor.a = 1.0;
            }
        }
        break;

    case PLSTATE_FILL:
        pldebug( "state: FILL" );

        // Read the pattern and put into the plot buffer
        rc = read_entry( plm, PDF_USHORT, PLP_UCHAR, &( pls->patt ) );
        if ( rc != PLM_SUCCESS )
            return rc;

        break;

    case PLSTATE_CMAP0:
    case PLSTATE_CMAP1:
        pldebug( "state: CMAP0/CMAP1" );

        {
            PLINT i, ncol;
            PLINT *r, *g, *b;
            PLFLT *alpha;
            void  *ptr;

            // Read the number of colors
            rc = read_entry( plm, PDF_USHORT, PLP_PLINT, &ncol );
            if ( rc != PLM_SUCCESS )
                return rc;

            // Check that temporary storage is sized correctly
            check_buffer_size(
                dev,
                sizeof ( PLINT ) * ncol * 3  // R, G, B values
                + sizeof ( PLFLT ) * ncol ); // alpha channel values
            ptr   = dev->buffer;
            r     = (PLINT *) ptr;
            ptr   = ( (PLINT *) ptr ) + ncol;
            g     = (PLINT *) ptr;
            ptr   = ( (PLINT *) ptr ) + ncol;
            b     = (PLINT *) ptr;
            ptr   = ( (PLINT *) ptr ) + ncol;
            alpha = (PLFLT *) ptr;

            // Read the colormap
            for ( i = 0; i < ncol; i++ )
            {
                rc = read_entry( plm, PDF_UBYTE, PLP_PLINT, &( r[i] ) );
                if ( rc != PLM_SUCCESS )
                    return rc;

                rc = read_entry( plm, PDF_UBYTE, PLP_PLINT, &( g[i] ) );
                if ( rc != PLM_SUCCESS )
                    return rc;

                rc = read_entry( plm, PDF_UBYTE, PLP_PLINT, &( b[i] ) );
                if ( rc != PLM_SUCCESS )
                    return rc;

                alpha[i] = 1.0;
            }

            // Call the colormap API so that memory is correctly allocated
            // instead of plP_state( PLSTATE_CMAP0 );
            if ( op == PLSTATE_CMAP0 )
                plscmap0a( r, g, b, alpha, ncol );
            else
                plscmap1a( r, g, b, alpha, ncol );

            // Return here because plscmap0a and plscmap1a call
            // plP_state( PLSTATE_CMAP0 or PLSTATE_CMAP1 )
            return PLM_SUCCESS;
        }
        break;

    case PLSTATE_CHR:
        pldebug( "state: CHR" );

        // The 2005 version and earlier do not support this operation
        if ( 1 )
        {
            rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &( pls->chrdef ) );
            if ( rc != PLM_SUCCESS )
                return rc;

            rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &( pls->chrht ) );
            if ( rc != PLM_SUCCESS )
                return rc;
        }
        break;

    case PLSTATE_SYM:
        pldebug( "state: SYM" );

        // The 2005 version and earlier do not support this operation
        if ( 1 )
        {
            rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &( pls->symdef ) );
            if ( rc != PLM_SUCCESS )
                return rc;

            rc = read_entry( plm, PDF_IEEEF, PLP_PLFLT, &( pls->symht ) );
            if ( rc != PLM_SUCCESS )
                return rc;
        }
        break;

    default:
        pldebug( "state: INVALID STATE" );
        return PLM_INVALID_STATE;
    }

    plP_state( op );

    return PLM_SUCCESS;
}

//--------------------------------------------------------------------------
// read_plot_commands()
//
// Reads the plot commands from the metafile and places them into the
// plot buffer
//--------------------------------------------------------------------------
static
enum _plm_status read_plot_commands( PDFstrm *plm, PLmDev *dev, PLStream *pls )
{
    uint8_t          cmd;
    enum _plm_status rc = PLM_SUCCESS;

    dbug_enter( "read_plot_commands()" );

    // Read the metafile until a non-zero result occurs, which typically
    // indicates an end-of-file condition
    while ( rc == PLM_SUCCESS && pdf_rd_1byte( plm, &cmd ) == 0 )
    {
        switch ( cmd )
        {
        case INITIALIZE:
            pldebug( "cmd: INITIALIZE" );
            // No action needed
            break;

        case CLOSE:
            pldebug( "cmd: CLOSE" );
            // No action needed
            break;

        case EOP:
            pldebug( "cmd: EOP" );

            plP_eop();
            break;

        case BOP:
        case BOP0:                   // First BOP in a file
            pldebug( "cmd: BOP/BOP0" );

            // Read the metadata for this page
            rc = read_entry( plm,
                page_2005_header[0].pdf_type,
                page_2005_header[0].plp_type,
                (uint8_t *) dev + page_2005_header[0].offset );
            if ( rc != PLM_SUCCESS )
                break;

            rc = read_entry( plm,
                page_2005_header[1].pdf_type,
                page_2005_header[1].plp_type,
                (uint8_t *) dev + page_2005_header[1].offset );
            if ( rc != PLM_SUCCESS )
                break;

            rc = read_entry( plm,
                page_2005_header[2].pdf_type,
                page_2005_header[2].plp_type,
                (uint8_t *) dev + page_2005_header[2].offset );
            if ( rc != PLM_SUCCESS )
                break;

            plP_bop();

            break;

        case LINE:
            pldebug( "cmd: LINE" );

            rc = read_line( plm, dev, pls );
            break;

        case LINETO:
            pldebug( "cmd: LINETO" );

            rc = read_lineto( plm, dev, pls );
            break;

        case ESCAPE:
            pldebug( "cmd: ESCAPE" );

            rc = read_escape( plm, dev, pls );
            break;

        case POLYLINE:
            pldebug( "cmd: POLYLINE" );

            rc = read_polyline( plm, dev, pls );
            break;

        case CHANGE_STATE:
            pldebug( "cmd: CHANGE_STATE" );

            rc = read_state( plm, dev, pls );
            break;

        case END_OF_FIELD:
            pldebug( "cmd: EOF" );

            // No action needed

            break;

        case SWITCH_TO_TEXT:        // Obsolete, replaced by ESCAPE
        case SWITCH_TO_GRAPH:       // Obsolete, replaced by ESCAPE
        case NEW_COLOR:             // Obsolete, replaced by CHANGE_STATE
        case NEW_WIDTH:             // Obsolete, replaced by CHANGE_STATE
        case ADVANCE:               // Obsolete, BOP/EOP used instead
        case NEW_COLOR1:
            pldebug( "cmd: OBSOLETE CMD" );
            plabort( "OBSOLETE CMD" );

            break;

        default:
            pldebug( "cmd: INVALID CMD" );
            plabort( "INVALID CMD" );

            return PLM_INVALID_CMD;
        }
    }

    return PLM_SUCCESS;
}

static
void setup_page( PLmDev *mf_dev, PLStream *pls )
{
    PLmDev *dev = pls->dev;

    mf_dev->mfpcxa = (PLFLT) dev->xlen
                     / (PLFLT) ( mf_dev->xmax - mf_dev->xmin );
    mf_dev->mfpcxb = (PLFLT) dev->xmin;
    mf_dev->mfpcya = (PLFLT) dev->ylen
                     / (PLFLT) ( mf_dev->ymax - mf_dev->ymin );
    mf_dev->mfpcyb = (PLFLT) dev->ymin;
}

//--------------------------------------------------------------------------
// plreadmetafile()
//
//! Reads a PLplot metafile and uses the current plot stream to display
//! the contents.  If the plot stream has not been initialized, this
//! routine will attempt to intialize the plot stream via a plinit()
//! call.  For an initialized plot stream, the metafile will start at the
//! current page/subpage.
//!
//! @param infile  Input PLplot metafile name.
//!
//! Pass NULL for infile to use the filename passed from the command line
//! option -mfi.
//!
//! Returns void
//--------------------------------------------------------------------------
void plreadmetafile( char *infile )
{
    PDFstrm          *plm = NULL;
    PLStream         mf_pls;
    PLmDev           mf_dev;
    PLmIndex         index;
    enum _plm_status rc;

    if ( plsc->mf_infile == NULL && infile == NULL )
    {
        plexit( "No PLplot metafile set for input" );
    }
    else if ( infile != NULL )
    {
        plm = pdf_fopen( infile, "rb" );
    }
    else
    {
        plm = pdf_fopen( plsc->mf_infile, "rb" );
    }
    if ( plm == NULL )
    {
        plexit( "Unable to open PLplot metafile for input" );
    }

    // Intialize the metafile device
    mf_dev.buffer      = NULL;
    mf_dev.buffer_size = 0;

    // Read the file header
    if ( ( rc = read_metafile_header( plm, &mf_dev ) ) != PLM_SUCCESS )
    {
        pdf_close( plm );
        plwarn( "Failed to parse PLplot metafile, ignoring file." );
        return;
    }

    // Read the index header
    rc = read_header( plm,
        metafile_format[mf_dev.version].index_header,
        (U_CHAR *) &index );
    if ( rc != PLM_SUCCESS )
    {
        pdf_close( plm );
        plwarn( "Corrupted index in metafile, ignoring file." );
        return;
    }

    // Read the device header
    rc = read_header( plm,
        metafile_format[mf_dev.version].device_header,
        (U_CHAR *) &mf_dev );
    if ( rc != PLM_SUCCESS )
    {
        pdf_close( plm );
        plwarn( "Corrupted device information in metafile, ignoring file." );
        return;
    }

    // Read the plot header into a local version of a plot stream.
    // We do this because some of the parameters from the metafile may
    // be invalid or inappropriate for the current plot device
    // (e.g. xlength = 0).  The plspage() call should be smart enough
    // to setup the page correctly.
    rc = read_header( plm,
        metafile_format[mf_dev.version].plot_header,
        (U_CHAR *) &mf_pls );
    if ( rc != PLM_SUCCESS )
    {
        pdf_close( plm );
        plwarn( "Corrupted device information in metafile, ignoring file." );
        return;
    }

    // Is the plot stream initialized?
    if ( plsc->level == 0 )
    {
        // No, we must intialize it in order to get the
        // device configuation set
        plinit();
    }
    setup_page( &mf_dev, plsc );

    // At this point we should be in the plot commands
    rc = read_plot_commands( plm, &mf_dev, plsc );
    if ( rc != PLM_SUCCESS )
    {
        pdf_close( plm );
        plwarn( "Corrupted plot information in metafile, ignoring file." );
        return;
    }

    pdf_close( plm );

    // Free the temporary storage
    if ( mf_dev.buffer != NULL )
        free( mf_dev.buffer );
}
