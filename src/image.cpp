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

#include "graphics.hpp"
#include "image.hpp"

// #define ToXColor(a) (((0xFF & (a)) << 8) | (a))

// #define free_mem(a) \
//     if (a != NULL) { free((void *) a); a = NULL; }

using namespace std;

namespace lib {


  void tv( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    actDevice->TV( e);
  }

  BaseGDL* tvrd( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    return actDevice->TVRD( e);
  }

  void loadct( EnvT* e) // LOADCT_INTERNALGDL for exclusive use by LOADCT
  {
    SizeT nCT = Graphics::N_CT();

    static int get_namesIx = e->KeywordIx( "GET_NAMES"); 
    bool get_names = e->KeywordPresent( get_namesIx);
    if( get_names)
      {
	e->AssureGlobalKW( get_namesIx);
	
	DStringGDL* names = new DStringGDL( nCT, BaseGDL::NOZERO);
	for( SizeT i=0; i<nCT; ++i)
	  (*names)[ i] = Graphics::GetCT( i)->Name();

	e->SetKW( get_namesIx, names);
      }

    if( e->NParam() == 0) return;

    DLong iCT;

    DByte r[256], g[256], b[256];
    PLINT rint[256], gint[256], bint[256];

    e->AssureLongScalarPar( 0, iCT);
    if( iCT < 0 || iCT >= nCT)
      e->Throw( "Table number must be from 0 to "+i2s(nCT-1));
    
    Graphics* actDevice = Graphics::GetDevice();
    GDLGStream* actStream = actDevice->GetStream( false); // no open

    Graphics::LoadCT( iCT);
    GDLCT* actCT = Graphics::GetCT();

    for( SizeT i=0; i<ctSize; ++i) {
      actCT->Get( i, r[ i], g[ i], b[ i]);

      rint[i] = (PLINT) r[i];
      gint[i] = (PLINT) g[i];
      bint[i] = (PLINT) b[i];
    }

    if (actStream != NULL)
      actStream->scmap1( rint, gint, bint, ctSize);
  }

} // namespace

