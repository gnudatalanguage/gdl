/***************************************************************************
  gdlwxstream.hpp  - adapted from plplot wxWidgets driver documentation
                             -------------------
    begin                : Wed Oct 16 2013
    copyright            : (C) 2013 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#ifdef FOR_IMPORT

#ifndef GDLWXSTREAM_HPP_
#define GDLWXSTREAM_HPP_

#include "gdlgstream.hpp"

// #include <plplotP.h>
// #include <plstream.h>
#include <wx/dc.h>

class GDLWXStream: public GDLGStream
{
private:
    wxDC* m_dc;   //!< Pointer to wxDC to plot into.
    int m_width;   //!< Width of dc/plot area.
    int m_height;   //!< Height of dc/plot area.

public:
    GDLWXStream( wxDC *dc, int width, int height );  //!< Constructor.
    void set_stream();   //!< Calls some code before every PLplot command.
    void SetSize( int width, int height );   //!< Set new size of plot area.
    void RenewPlot();   //!< Redo plot.
    
    void Init();

};



#endif

//#endif
