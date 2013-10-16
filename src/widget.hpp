/***************************************************************************
                          widget.hpp  -  WxWidgets GDL library function
                             -------------------
    begin                : Dec 17 2007
    copyright            : (C) 2007 by Joel Gales
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIDGET_HPP_
#define WIDGET_HPP_

namespace lib {

  BaseGDL* widget_draw( EnvT* e);
  BaseGDL* widget_base( EnvT* e);
  BaseGDL* widget_button( EnvT* e);
  BaseGDL* widget_droplist( EnvT* e);
  BaseGDL* widget_list( EnvT* e);
  BaseGDL* widget_text( EnvT* e);
  BaseGDL* widget_label( EnvT* e);
  BaseGDL* widget_info( EnvT* e);
  BaseGDL* widget_event( EnvT* e);
  BaseGDL* widget_bgroup( EnvT* e);

  void widget_control( EnvT* e);

} // namespace

BaseGDL* CallEventFunc( const std::string f, BaseGDL* ev);
void CallEventPro( const std::string p, BaseGDL* p0, BaseGDL* p1 = NULL);
DStructGDL* CallEventHandler( DLong id, DStructGDL* ev);

#endif
