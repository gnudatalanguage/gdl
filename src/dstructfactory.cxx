/***************************************************************************
                  dstructfactory.cxx  -  DStructGDL creation helper
                             -------------------
    begin                : May 28 2018
    copyright            : (C) 2018 by Remi A. Solås
    email                : remi.solaas (at) edinsights.no
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dstructfactory.hxx"

DStructFactory::DStructFactory() :
    desc_(new DStructDesc("$truct"))
{
}

DStructFactory::~DStructFactory()
{
    for(auto& pair : vals_)
        delete pair.second;
}

DStructGDL* DStructFactory::Create()
{
    auto res = new DStructGDL(desc_, dimension());
    for(auto& pair : vals_) {
        res->InitTag(pair.first, *pair.second);
        delete pair.second;
    }
    vals_.clear();
    return res;
}

