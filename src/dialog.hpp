/***************************************************************************
             dialog.hpp  -  GDL library function for dialogs
                             -------------------
    begin                : 04 Apr 2015
    copyright            : (C) 2015 by Jeongbin Park
    email                : pjb7687@gmail.com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "envt.hpp"

//#ifndef _U
//// only in wxWidget 3.0 and after : 
//#define _U(x) wxString(x, wxConvUTF8)
//#endif
#ifndef _D
#define _D(x) DString((x).mb_str(wxConvUTF8))
#endif

namespace lib {
	BaseGDL* wxwidgets_exists(EnvT* e);
	BaseGDL* dialog_pickfile_wxwidgets(EnvT* e);
	BaseGDL* dialog_message_wxwidgets(EnvT* e);
} // namespace
