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

//
//   Program for generating data structures used for translating
//   between unicode and hershey
//
//  The program is pretty dumb, because it does no command line parsing;
//  instead it assumes that argv[1] will be the input file, and argv[2]
//  the output file.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//--------------------------------------------------------------------------
//   Function-like macro definitions
//--------------------------------------------------------------------------

#define MemError1( a )    do { fprintf( stderr, "MEMORY ERROR %d\n" a "\n", __LINE__ ); exit( __LINE__ ); } while ( 0 )

const char header[] = ""                                                                                 \
                      "/*\n"                                                                             \
                      "  This file is part of PLplot.\n"                                                 \
                      "  \n"                                                                             \
                      "  PLplot is free software; you can redistribute it and/or modify\n"               \
                      "  it under the terms of the GNU Library General Public License as published\n"    \
                      "  by the Free Software Foundation; either version 2 of the License, or\n"         \
                      "  (at your option) any later version.\n"                                          \
                      "  \n"                                                                             \
                      "  PLplot is distributed in the hope that it will be useful,\n"                    \
                      "  but WITHOUT ANY WARRANTY; without even the implied warranty of\n"               \
                      "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"                \
                      "  GNU Library General Public License for more details.\n"                         \
                      "  \n"                                                                             \
                      "  You should have received a copy of the GNU Library General Public License\n"    \
                      "  along with PLplot; if not, write to the Free Software\n"                        \
                      "  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA\n" \
                      "  \n"                                                                             \
                      "  \n"                                                                             \
                      "  This header file contains the lookup tables used for converting between\n"      \
                      "  hershey and unicode. It is an automatically generated file, so please do\n"     \
                      "  not edit it directly. Make any changes to plhershey-unicode.csv, then use\n"    \
                      "  plhershey-unicode-gen.c to recreate this header file.\n"                        \
                      "  \n"                                                                             \
                      "  plhershey-unicode.csv consists of three fields: the first field is the\n"       \
                      "  hershey code, and is in decimal; the second is the unicode value, and is\n"     \
                      "  in hex; and the final field is font index. There are five possible font\n"      \
                      "  indices:\n"                                                                     \
                      "       0        undefined/unknown\n"                                              \
                      "       1        normal\n"                                                         \
                      "       2        roman\n"                                                          \
                      "       3        italic-roman\n"                                                   \
                      "       4        script\n"                                                         \
                      "  \n"                                                                             \
                      "  Font indices are used for approximating the appearence of the original\n"       \
                      "  hershey glyphs.\n"                                                              \
                      "  \n"                                                                             \
                      "  Unicode values of 0x0000 signify unknowns.\n"                                   \
                      "  \n"                                                                             \
                      "*/";



int main( int argc, char *argv[] )
{
    FILE         *fr, *fw;
    char         readbuffer[256];
    int          *Hershey        = NULL;
    unsigned int *Unicode        = NULL;
    char         *Font           = NULL;
    int          Hershey_old     = 0;
    int          i               = 0;
    int          number_of_lines = 0;

    if ( ( argc >= 2 ) && ( fr = fopen( argv[1], "r" ) ) != NULL )
    {
        //
        //   Work out how many lines we have all up
        //

        while ( ( fgets( readbuffer, 255, fr ) != NULL ) )
        {
            ++number_of_lines;
        }

        //
        //   Allocate memory to the arrays which will hold the data
        //

        if ( ( Hershey = (int *) calloc( (size_t) number_of_lines, sizeof ( int ) ) ) == NULL )
            MemError1( "Allocating memory to the hershey table" );

        if ( ( Unicode = (unsigned int *) calloc( (size_t) number_of_lines, sizeof ( unsigned int ) ) ) == NULL )
            MemError1( "Allocating memory to the unicode table" );

        if ( ( Font = (char *) calloc( (size_t) number_of_lines, sizeof ( char ) ) ) == NULL )
            MemError1( "Allocating memory to the font table" );

        rewind( fr ); // Go back to the start of the file

        //
        //    Read in line by line, and copy the numbers into our arrays
        //

        while ( ( fgets( readbuffer, 255, fr ) != NULL ) )
        {
            sscanf( readbuffer, "%x,%d,%c", (unsigned int *) &Unicode[i], (int *) &Hershey[i], (char *) &Font[i] );
            if ( Hershey[i] <= Hershey_old )
            {
                fprintf( stderr, "Error: Hershey index = %d is not ascending\n", Hershey[i] );
                return ( 1 );
            }
            Hershey_old = Hershey[i];
            i++;
        }

        fclose( fr );
    }

//
//   Write the data out to file ready to be included in our source
//


    if ( ( fw = fopen( argv[2], "w" ) ) != NULL )
    {
        fprintf( fw, "%s\n", header );

        fprintf( fw, "const int number_of_entries_in_hershey_to_unicode_table=%d;\n\n", number_of_lines );

        fprintf( fw, "typedef struct {\n\tunsigned int Hershey;\n\tPLUNICODE Unicode;\n\tchar Font;\n} Hershey_to_Unicode_table;\n\n" );
        fprintf( fw, "const Hershey_to_Unicode_table hershey_to_unicode_lookup_table[%d] = {\n", number_of_lines );

        for ( i = 0; i < number_of_lines; i++ )
        {
            if ( ( ( i % 4 ) == 0 ) && ( i > 0 ) )
                fprintf( fw, "\n" );
            fprintf( fw, "{%d,0x%04x,%c}", (int) Hershey[i], (unsigned int) Unicode[i], (char) Font[i] );
            if ( i < ( number_of_lines - 1 ) )
                fprintf( fw, ", " );
        }

        fprintf( fw, "\n};\n" );

        fclose( fw );
    }
    free( Unicode );
    free( Hershey );
    free( Font );

    return ( 0 );
}

