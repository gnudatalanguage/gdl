/***************************************************************************
                       plotting_erase.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
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
#include "plotting.hpp"

namespace lib {

  using namespace std;

  class erase_call : public plotting_routine_call
  {

    private: bool handle_args(EnvT* e) // {{{
    {
      if (nParam() > 1)
        e->Throw( "Incorrect number of arguments.");
      return false;
    } // }}}

    void old_body( EnvT* e, GDLGStream* actStream) // {{{
    {
    } // }}}

    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      DLong chan = 0;
      static int chanIx = e->KeywordIx( "CHANNEL" );
      if ( e->KeywordPresent( chanIx ) ) {
        e->AssureLongScalarKWIfPresent( chanIx, chan );
        if ( (chan > 3) || (chan < 0) ) e->Throw( "Value of Channel is out of allowed range." );
      } else { //get !P.CHANNEL value
          static DStructGDL* pStruct = SysVar::P( );
          chan = (*static_cast<DLongGDL*>
          (pStruct->GetTag( pStruct->Desc( )->TagIndex( "CHANNEL" ), 0 )))[0];
      }

      static DStructGDL* dStruct = SysVar::D( );
      DLong MaxColorIdx;
      MaxColorIdx = (*static_cast<DLongGDL*>
      (dStruct->GetTag( dStruct->Desc( )->TagIndex( "N_COLORS" ), 0 )))[0];

      DLong bColor = -1;
      static int bColorIx = e->KeywordIx( "COLOR" );

      if ( nParam( ) == 0 ) {
        if ( e->KeywordPresent( e->KeywordIx( "COLOR" ) ) ) {
          e->AssureLongScalarKWIfPresent( bColorIx, bColor );
        } else
          // we have to read back !p.background value
        {
          static DStructGDL* pStruct = SysVar::P( );
          bColor = (*static_cast<DLongGDL*>
          (pStruct->GetTag( pStruct->Desc( )->TagIndex( "BACKGROUND" ), 0 )))[0];
        }
      } else {
        e->AssureLongScalarPar( 0, bColor );
      }
      if ( bColor > MaxColorIdx ) bColor = MaxColorIdx;
      if ( bColor < 0 ) bColor = 0;
      DLong decomposed = GraphicsDevice::GetDevice( )->GetDecomposed( );
      actStream->Background( bColor, decomposed );
      if ( chan > 0 ) actStream->Clear( chan - 1 ); else actStream->Clear( );      
    }

    private: virtual void post_call(EnvT*, GDLGStream*) // {{{
    {
    } // }}}

  };

  void erase(EnvT* e)
  {
    erase_call erase;
    erase.call(e, 0); 
  }

} // namespace
