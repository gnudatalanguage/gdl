/***************************************************************************
                          objects.cpp  -  global structures
                             -------------------
    begin                : November 28 2012
    copyright            : (C) 2012 by Alain Coulais
    email                : 
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

#include <limits>
//#include <ios>

#include "str.hpp"
//#include "gdlexception.hpp"
#include "initsysvar.hpp"
//#include "dnodefactory.hpp"

//using namespace std;

//StructListT prefList;

namespace Preferences
{
  
  void Init()
  {
    SpDInt    aInt;
    SpDLong   aLong;
    SpDString aString;
    SpDByte   aByte;

    // extra types we don't used now ...
    // SpDULong  aULong;
    //SpDLong64 aLong64;
    //SpDFloat  aFloat;
    //SpDDouble aDouble;
    //SpDLong   aLongArr8( dimension(8));
    
    DStructDesc* gdl_gr_x_height= new DStructDesc("GDL_GR_X_HEIGHT");
    gdl_gr_x_height->AddTag("NAME", &aString);
    gdl_gr_x_height->AddTag("DESCRIPTION", &aString);
    gdl_gr_x_height->AddTag("CURRENT_SOURCE", &aString);
    gdl_gr_x_height->AddTag("COMMIT_SEEN", &aString);
    gdl_gr_x_height->AddTag("READONLY", &aByte);
    gdl_gr_x_height->AddTag("HAS_MIN", &aByte);
    gdl_gr_x_height->AddTag("HAS_MAX", &aByte);
    gdl_gr_x_height->AddTag("HAS_DISCRETE_VALUES", &aByte);
    gdl_gr_x_height->AddTag("HAS_PENDING", &aByte);
    gdl_gr_x_height->AddTag("VALUE_EFFECTIVE", &aLong);
    gdl_gr_x_height->AddTag("VALUE_DEFAULT", &aLong);
    gdl_gr_x_height->AddTag("VALUE_PENDING", &aLong);
    gdl_gr_x_height->AddTag("VALUE_MIN", &aLong);
    
    structList.push_back(gdl_gr_x_height);
  }
}

