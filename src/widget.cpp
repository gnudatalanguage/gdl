/***************************************************************************
                             widget.cpp  -  GDL WIDGET_XXX library functions
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "datatypes.hpp"
#include "envt.hpp"

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwidget.hpp"

namespace lib {
  using namespace std;

  BaseGDL* widget_base( EnvT* e)
  {
    SizeT nParam = e->NParam();
    // ...
  }

} // namespace

#endif
