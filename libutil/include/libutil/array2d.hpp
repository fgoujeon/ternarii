/*
Copyright 2018 - 2020 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBUTIL_ARRAY2D_HPP
#define LIBUTIL_ARRAY2D_HPP

#include <array>

namespace libutil
{

template<typename T, size_t N0, size_t N1>
using array2d = std::array<std::array<T, N1>, N0>;

//For each item of the given array
template<typename T, size_t N0, size_t N1, typename F>
void for_each(const array2d<T, N0, N1>& arr, F&& f)
{
    for(const auto& subarr: arr)
    {
        for(const auto& item: subarr)
        {
            f(item);
        }
    }
}

//For each item of the given array
template<typename T, size_t N0, size_t N1, typename F>
void for_each(array2d<T, N0, N1>& arr, F&& f)
{
    for(auto& subarr: arr)
    {
        for(auto& item: subarr)
        {
            f(item);
        }
    }
}

//For each item of the given array
template<typename T, size_t N0, size_t N1, typename F>
void for_each_i(const array2d<T, N0, N1>& arr, F&& f)
{
    auto i0 = 0;
    for(const auto& subarr: arr)
    {
        auto i1 = 0;
        for(const auto& item: subarr)
        {
            f(item, i0, i1);
            ++i1;
        }
        ++i0;
    }
}

//For each item of the given array
template<typename T, size_t N0, size_t N1, typename F>
void for_each_i(array2d<T, N0, N1>& arr, F&& f)
{
    auto i0 = 0;
    for(auto& subarr: arr)
    {
        auto i1 = 0;
        for(auto& item: subarr)
        {
            f(item, i0, i1);
            ++i1;
        }
        ++i0;
    }
}

} //namespace

#endif
