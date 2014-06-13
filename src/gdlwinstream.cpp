/* *************************************************************************
						  gdlwinstream.cpp  -  graphic stream M$ windows
						  -------------------
						  begin                : July 22 2002
						  copyright            : (C) 2002 by Marc Schellens
						  email                : m_schellens@users.sf.net
						  ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include <iostream>

#include "graphicsdevice.hpp"
#include "gdlwinstream.hpp"

using namespace std;

void GDLWINStream::Init()
{
	plstream::init();
	plgpls(&pls);
	//pls->debug = true;

	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
	dev->waiting = 1;
}

void GDLWINStream::EventHandler()
{
	MSG Message;
	if (PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&Message, NULL, 0, 0);
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}
