/***************************************************************************
                 libinit_ng.cpp  -  initialization of GDL library routines
                             -------------------
    begin                : 26 May 2008
    copyright            : (C) 2008 by Nicolas Galmiche
    email                : n.galmiche@gmail.com
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
#include "math_fun_ng.hpp"

using namespace std;

void LibInit_ng()
{
  const char KLISTEND[] = "";

  const string rk4Key[]={"DOUBLE","ITER",KLISTEND};
  new DLibFunRetNew(lib::rk4_fun,string("RK4"),5,rk4Key);

  const string voigtKey[]={"DOUBLE","ITER",KLISTEND};
  new DLibFunRetNew(lib::voigt_fun,string("VOIGT"),2,voigtKey);
 
}

