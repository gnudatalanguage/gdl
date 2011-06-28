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

    private: void handle_args(EnvT* e) // {{{
    {
      if (nParam() > 1)
        e->Throw( "Incorrect number of arguments.");
    } // }}}

    void old_body( EnvT* e, GDLGStream* actStream) // {{{
    {
    } // }}}

    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      if (nParam() == 0)
      {
        actStream->Clear();
      }
      else 
      {
        DLong bColor;
        e->AssureLongScalarPar( 0, bColor);
        if (bColor > 255) bColor = 255;
        if (bColor < 0)   bColor = 0;
        actStream->Clear( bColor);
      }
    } // }}}

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
