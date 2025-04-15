#include "plplotP.h"
void
plexit( PLCHAR_VECTOR errormsg ) {
  exit(0);
}
static void
pldebug( const char *label, ... )
{
  fprintf( stderr, "%s: ", label );
}
static int
pdf_wrx( const U_CHAR *x, long nitems, PDFstrm *pdfs )
{
    int i, result = 0;

    if ( pdfs->file != NULL )
    {
        result    = (int) fwrite( x, 1, (size_t) nitems, pdfs->file );
        pdfs->bp += (size_t) nitems;
    }
    else if ( pdfs->buffer != NULL )
    {
        for ( i = 0; i < nitems; i++ )
        {
            if ( pdfs->bp >= pdfs->bufmax )
            {
                pldebug( "pdf_wrx",
                    "Increasing buffer to %d bytes\n", pdfs->bufmax );
                pdfs->bufmax += 512;
                if ( ( pdfs->buffer = (U_CHAR *)
                                      realloc( (void *) ( pdfs->buffer ), pdfs->bufmax ) ) == NULL )
                {
                    plexit( "pdf_wrx: Insufficient memory" );
                }
            }
            pdfs->buffer[pdfs->bp++] = x[i];
        }
        result = i;
    }

    return result;
}

int
pdf_wr_2bytes( PDFstrm *pdfs, U_SHORT s )
{
    U_CHAR x[2];

    x[0] = (U_CHAR) ( (U_LONG) ( s & (U_LONG) 0x00FF ) );
    x[1] = (U_CHAR) ( (U_LONG) ( s & (U_LONG) 0xFF00 ) >> 8 );

    if ( pdf_wrx( x, 2, pdfs ) != 2 )
        return PDF_WRERR;

    return 0;
}
int
pdf_wr_2nbytes( PDFstrm *pdfs, U_SHORT *s, PLINT n )
{
    PLINT  i;
    U_CHAR x[2];

    for ( i = 0; i < n; i++ )
    {
        x[0] = (U_CHAR) ( (U_LONG) ( s[i] & (U_LONG) 0x00FF ) );
        x[1] = (U_CHAR) ( (U_LONG) ( s[i] & (U_LONG) 0xFF00 ) >> 8 );

        if ( pdf_wrx( x, 2, pdfs ) != 2 )
            return PDF_WRERR;
    }
    return 0;
}
PDFstrm *
pdf_fopen( PLCHAR_VECTOR filename, PLCHAR_VECTOR mode )
{
    PDFstrm *pdfs;

    dbug_enter( "pdf_fopen" );

    pdfs = (PDFstrm *) malloc( sizeof ( PDFstrm ) );

    if ( pdfs != NULL )
    {
        pdfs->buffer = NULL;
        pdfs->file   = NULL;
        pdfs->bp     = 0;
        pdfs->file = fopen( filename, mode );
        if ( pdfs->file == NULL )
        {
            pdf_close( pdfs );
            pdfs = NULL;
        }
    }

    return pdfs;
}


int
pdf_close( PDFstrm *pdfs )
{

    if ( pdfs != NULL )
    {
        if ( pdfs->file != NULL )
        {
            fclose( pdfs->file );
        }
        else if ( pdfs->buffer != NULL )
        {
            free( (void *) pdfs->buffer );
        }
        free( (void *) pdfs );
    }
    return 0;
}
