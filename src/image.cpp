/***************************************************************************
                          image.cpp  -  GDL image routines
                             -------------------
    begin                : Jul 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// PLplot is used for direct graphics

#include "includefirst.hpp"

#include "graphicsdevice.hpp"
#include "image.hpp"

// #define ToXColor(a) (((0xFF & (a)) << 8) | (a))

// #define free_mem(a) \
//     if (a != NULL) { free((void *) a); a = NULL; }

using namespace std;

namespace lib {


  void tv( EnvT* e)
  {
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    actDevice->TV( e);
  }

  BaseGDL* tvrd( EnvT* e)
  {
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    return actDevice->TVRD( e);
  }
#define MAX_COLORS 256

  void loadct( EnvT* e) // = LOADCT_INTERNALGDL for exclusive use by LOADCT
  {
    SizeT nCT = GraphicsDevice::N_CT();

    static int get_namesIx = e->KeywordIx( "GET_NAMES"); 
    bool get_names = e->KeywordPresent( get_namesIx);
    if( get_names)
    {
      e->AssureGlobalKW( get_namesIx);

      DStringGDL* names = new DStringGDL( nCT, BaseGDL::NOZERO);
      for( SizeT i=0; i<nCT; ++i)
        (*names)[ i] = GraphicsDevice::GetCT( i)->Name();

      e->SetKW( get_namesIx, names);
      return; //correct behaviour.
    }

    if( e->NParam() == 0) return; //FIXME should list tables names, promt for number and load it!

    DLong iCT;

    DByte r[MAX_COLORS], g[MAX_COLORS], b[MAX_COLORS];
    PLINT rint[MAX_COLORS], gint[MAX_COLORS], bint[MAX_COLORS];
    //load original table
    GDLCT* actCT = GraphicsDevice::GetCT();
    actCT->Get(rint,gint,bint,MAX_COLORS);


    e->AssureLongScalarPar( 0, iCT);
    if( iCT < 0 || iCT >= nCT)
      e->Throw( "Table number must be from 0 to "+i2s(nCT-1));
    
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    GDLGStream* actStream = actDevice->GetStream( false); // no open

    GraphicsDevice::LoadCT( iCT);
    //new table is:
    actCT = GraphicsDevice::GetCT();
    DLong bottom=0;
    DLong ncolors=MAX_COLORS;
    if ( e->KeywordSet ( "BOTTOM" ) ) e->AssureLongScalarKWIfPresent ( "BOTTOM", bottom );
    if ( e->KeywordSet ( "NCOLORS" ) ) e->AssureLongScalarKWIfPresent ( "NCOLORS", ncolors );
    if (bottom < 0) bottom=0;
    if (bottom > MAX_COLORS-1) bottom=MAX_COLORS-1;
    if (ncolors < 1) ncolors=1;
    if (ncolors > MAX_COLORS) ncolors=MAX_COLORS;
    if (bottom+ncolors > MAX_COLORS) ncolors=MAX_COLORS-bottom;
    for( SizeT i=0, j=bottom ; j<bottom+ncolors; ++i, ++j) {
      actCT->Get( i, r[ i], g[ i], b[ i]);
      //update section of colors
      rint[j] = (PLINT) r[i];
      gint[j] = (PLINT) g[i];
      bint[j] = (PLINT) b[i];
    }

    static int rgbtableIx = e->KeywordIx( "RGB_TABLE");
    if( e->KeywordPresent( rgbtableIx) )
    {
      e->AssureGlobalKW( rgbtableIx);
      DByteGDL* rgbtable = new DByteGDL( dimension(ncolors, 3), BaseGDL::NOZERO);
      for( SizeT i=0, j=bottom ; j<bottom+ncolors; ++i, ++j)
      {
        (*rgbtable)[i] = rint[j];
        (*rgbtable)[i+ncolors] = gint[j];
        (*rgbtable)[i+2*ncolors] = bint[j];
      }
      e->SetKW( rgbtableIx, rgbtable);
      return; //correct behaviour.
    }

    if (actStream != NULL)
      actStream->scmap0( rint, gint, bint, MAX_COLORS);
  }
#undef MAX_COLORS

} // namespace

