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
#include <memory>

class DStructFactory
{
public:
    template<class T, typename... Vs>
    void Add(const char* name, const Vs&... vals)
    {
        constexpr auto N = sizeof...(Vs);
        typename T::Traits proto{dimension(N)};
        typename T::Ty arr[] = { static_cast<typename T::Ty>(vals)... };
        AddTag(name, proto, N > 1 ? std::make_unique<T>(arr, N) : std::make_unique<T>(arr[0]));
    }

    template<class T, typename V>
    void AddArr(const char* name, size_t count, const V* vals)
    {
        typename T::Traits proto{dimension(count)};
        typename T::Traits::DataT arr(vals, count);
        AddTag(name, proto, std::make_unique<T>(dimension(count), arr));
    }

    template<class T, typename V>
    void AddMat(const char* name, size_t cols, size_t rows, const V* vals)
    {
        typename T::Traits proto{dimension(cols, rows)};
        typename T::Traits::DataT mat(vals, rows * cols);
        AddTag(name, proto, std::make_unique<T>(dimension(cols, rows), mat));
    }

    DStructGDL* Create(); 

private:
    void AddTag(const char* name, const BaseGDL& prototype, std::unique_ptr<BaseGDL> value);
    std::unique_ptr<DStructDesc> desc_;
    std::map<std::string, std::unique_ptr<BaseGDL>> vals_;
};

#endif

