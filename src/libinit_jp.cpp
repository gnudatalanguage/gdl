/***************************************************************************
libinit_jp.cpp  -  initialization of GDL library routines
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


#include "includefirst.hpp"
#include "dialog.hpp"

using namespace std;

void LibInit_jp()
{
	const char KLISTEND[] = "";

	new DLibFunRetNew(lib::wxwidgets_exists, string("WXWIDGETS_EXISTS"));

#ifdef HAVE_LIBWXWIDGETS
	const string dialog_pickFile_wxwidgetsKey[] = { "DEFAULT_EXTENSION", "DIRECTORY", "DIALOG_PARENT",
		"DISPLAY_NAME", "FILE", "FILTER", "FIX_FILTER",
		"GET_PATH", "GROUP", "MULTIPLE_FILES", "MUST_EXIST",
		"OVERWRITE_PROMPT", "PATH", "READ", "WRITE", "RESOURCE_NAME",
		"TITLE", KLISTEND };
	new DLibFunRetNew(lib::dialog_pickfile_wxwidgets, string("DIALOG_PICKFILE_WXWIDGETS"), 0, dialog_pickFile_wxwidgetsKey);
#endif
}