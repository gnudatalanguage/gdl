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

void DStructFactory::AddTag(const char* name, const BaseGDL& prototype, std::unique_ptr<BaseGDL> value)
{
    if (!desc_) desc_ = std::make_unique<DStructDesc>("$truct");
    desc_->AddTag(name, &prototype);
    vals_[name] = std::move(value);
}

DStructGDL* DStructFactory::Create()
{
    if(vals_.empty())
        return nullptr;

    // DStructGDL adopts the initial reference to the anonymous descriptor.
    auto res = std::make_unique<DStructGDL>(std::move(desc_), dimension());
    for(auto& pair : vals_) {
        res->InitTag(pair.first, *pair.second);
    }
    vals_.clear();
    return res.release();
}

