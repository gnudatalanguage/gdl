/***************************************************************************
                          libinit_errorfunctions.cpp  -  a math GDL library function
                             -------------------
    begin                : 
    copyright            : 
    email                : 
    website              : 

****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#include "errorfunctions.hpp"

using namespace std;

void LibInit_errorfunctions()
{
  new DLibFunRetNewTP(lib::erf_fun,string("ERF"),1);  //UsesThreadPOOL 
  new DLibFunRetNew(lib::errorf_fun,string("ERRORF"),1);
  new DLibFunRetNewTP(lib::erfc_fun,string("ERFC"),1);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::erfi_fun,string("ERFI"),1);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::erfcx_fun,string("ERFCX"),1);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::dawson_fun,string("DAWSON"),1);  //UsesThreadPOOL 
  new DLibFunRetNewTP(lib::faddeeva_fun,string("FADDEEVA"),1);  //UsesThreadPOOL
}

