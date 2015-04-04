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

#include "datatypes.hpp"
#include "dinterpreter.hpp"

namespace lib {
	BaseGDL* wxwidgets_exists(EnvT* e);
#ifdef HAVE_LIBWXWIDGETS
	BaseGDL* dialog_pickfile_wxwidgets(EnvT* e);
#endif
} // namespace
