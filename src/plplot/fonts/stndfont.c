//      Utility to generate standard font set.

#include "plplotP.h"

extern short int *hersh[];
extern short int *findex[];
extern short int *buffer[];

int
compare( const void *si1, const void *si2 );
int
compare( const void *si1, const void *si2 )
{
    const short *a = (const short *) si1;
    const short *b = (const short *) si2;

    return ( *a == *b ? 0 : ( *a > *b ? 1 : -1 ) );
}

int
main( void )
{
    size_t  i, j, k, ib, nstd;
    U_SHORT nchars, nleng, htab, nindx, zero;
    U_SHORT *hrshlst, *hrshidx;
    int     ix, iy;
    long    fpos;
    PDFstrm *pdfs;

    hrshlst = (U_SHORT *) malloc( 176 * sizeof ( U_SHORT ) );
    hrshidx = (U_SHORT *) malloc( 176 * sizeof ( U_SHORT ) );

    ib = 0;
    for ( k = 0; k < 176; k++ )
        hrshlst[ib++] = (U_SHORT) *( hersh[0] + k );

// Sort list

    qsort( (char *) hrshlst, ib, sizeof ( U_SHORT ), compare );

// Remove duplicates

    k = 0;
    j = 0;
    do
    {
        if ( hrshlst[k] == hrshlst[j] )
            j++;
        else
            hrshlst[++k] = hrshlst[j];
    } while ( j < ib );

    nstd = k + 1;

// Now reindex the fonts

    for ( k = 0; k < 176; k++ )
        for ( i = 0; i < nstd; i++ )
            if ( *( hersh[0] + k ) == hrshlst[i] )
            {
                hrshidx[k] = (U_SHORT) ( i + 1 );
                break;
            }

    pdfs = pdf_fopen( PL_SFONT, "wb+" );
    if ( !pdfs )
    {
        printf( "Error opening standard font file.\n" );
        exit( 1 );
    }

    htab = 1 * 256 + 176;
    pdf_wr_2bytes( pdfs, htab );
    pdf_wr_2nbytes( pdfs, hrshidx, 176 );

    zero  = 0;
    nindx = 0;
    nleng = 1;
    fpos  = ftell( pdfs->file );

    pdf_wr_2bytes( pdfs, nindx );
    for ( j = 0; j < nstd; j++ )
    {
        ib = (size_t) *( findex[( hrshlst[j] - 1 ) / 100] + ( hrshlst[j] - 1 ) % 100 );
        if ( ib == 0 )
        {
            pdf_wr_2bytes( pdfs, zero );
            nindx++;
        }
        else
        {
            pdf_wr_2bytes( pdfs, nleng );
            nindx++;
            for (;; )
            {
                ix = *( buffer[ib / 100] + ib % 100 ) / 128 - 64;
                iy = *( buffer[ib / 100] + ib % 100 ) % 128 - 64;
                ib++;
                if ( ix == -64 )
                    ix = 64;
                if ( iy == -64 )
                    iy = 64;
                nleng++;
                if ( ix == 64 && iy == 64 )
                    break;
            }
        }
    }
    fseek( pdfs->file, fpos, 0 );
    pdf_wr_2bytes( pdfs, nindx );

    nchars = 0;
    nleng  = 1;
    fseek( pdfs->file, 0, 2 );            // Go to end of file
    fpos = ftell( pdfs->file );           // Save current position
    pdf_wr_2bytes( pdfs, nleng );

    for ( j = 0; j < nstd; j++ )
    {
        ib = (size_t) *( findex[( hrshlst[j] - 1 ) / 100] + ( hrshlst[j] - 1 ) % 100 );
        if ( ib != 0 )
        {
            for (;; )
            {
                ix = *( buffer[ib / 100] + ib % 100 ) / 128 - 64;
                iy = *( buffer[ib / 100] + ib % 100 ) % 128 - 64;
                ib++;
                if ( ix == -64 )
                    ix = 64;
                if ( iy == -64 )
                    iy = 64;
                fputc( ix, pdfs->file );
                fputc( iy, pdfs->file );
                nleng++;
                if ( ix == 64 && iy == 64 )
                    break;
            }
            nchars++;
        }
    }
    nleng--;
    fseek( pdfs->file, fpos, 0 );
    pdf_wr_2bytes( pdfs, nleng );
    pdf_close( pdfs );
    free( hrshlst );
    free( hrshidx );
    printf( "There are %d characters in standard font set.\n", nchars - 1 );
    exit( 0 );
}
