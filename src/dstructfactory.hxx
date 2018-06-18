/***************************************************************************
                  dstructfactory.hxx  -  DStructGDL creation helper
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

#ifndef DSTRUCTFACTORY_HXX
#define DSTRUCTFACTORY_HXX

#include "includefirst.hpp"
#include "dstructgdl.hpp"
#include <map>

class DStructFactory
{
public:
     DStructFactory();
    ~DStructFactory();

    template<class T, typename... Vs>
    void Add(const char* name, const Vs&... vals)
    {
        constexpr auto N = sizeof...(Vs);
        auto proto = new typename T::Traits(dimension(N));
        typename T::Ty arr[] = { static_cast<typename T::Ty>(vals)... };
        desc_->AddTag(name, proto);
        vals_[name] = (N > 1 ? new T(arr, N) : new T(arr[0]));
        delete proto;
    }

    template<class T, typename V>
    void AddArr(const char* name, size_t count, const V* vals)
    {
        auto proto = new typename T::Traits(dimension(count));
        typename T::Traits::DataT arr(vals, count);
        desc_->AddTag(name, proto);
        vals_[name] = new T(dimension(count), arr);
        delete proto;
    }

    template<class T, typename V>
    void AddMat(const char* name, size_t cols, size_t rows, const V* vals)
    {
        auto proto = new typename T::Traits(dimension(cols, rows));
        typename T::Traits::DataT mat(vals, rows * cols);
        desc_->AddTag(name, proto);
        vals_[name] = new T(dimension(cols, rows), mat);
        delete proto;
    }

    DStructGDL* Create(); 

private:
    DStructDesc* desc_ = nullptr;
    std::map<const char*, BaseGDL*> vals_;
};

#endif

