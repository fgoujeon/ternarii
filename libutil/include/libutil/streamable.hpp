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

#ifndef LIBUTIL_STREAMABLE_HPP
#define LIBUTIL_STREAMABLE_HPP

#include "matrix.hpp"
#include <vector>
#include <optional>
#include <iostream>

namespace libutil
{

template<class T>
struct streamable
{
    const T& value;
};

template<class T>
streamable(const T& value) -> streamable<T>;



//General case. Relies on ADL.
template<class T>
std::ostream& operator<<(std::ostream& l, const streamable<T>& r)
{
    return l << r.value;
}



/*
Fundamental types
*/

inline
std::ostream& operator<<(std::ostream& l, const streamable<int>& r)
{
    return l << r.value;
}

inline
std::ostream& operator<<(std::ostream& l, const streamable<double>& r)
{
    return l << r.value;
}



/*
Helpers
*/

namespace streamable_detail
{
    template<class Container>
    std::ostream& stream_sequence_container(std::ostream& l, const Container& r)
    {
        l << "{";
        auto first = true;
        for(const auto& v: r)
        {
            if(!first)
            {
                l << ", ";
            }
            l << streamable{v};
            first = false;
        }
        l << "}";
        return l;
    }
}



/*
Standard library types
*/

template<class T, size_t Size>
std::ostream& operator<<(std::ostream& l, const streamable<std::array<T, Size>>& r)
{
    return streamable_detail::stream_sequence_container(l, r.value);
}

template<class T>
std::ostream& operator<<(std::ostream& l, const streamable<std::optional<T>>& r)
{
    if(r.value.has_value())
    {
        return l << streamable{r.value.value()};
    }
    else
    {
        return l << "std::nullopt";
    }
}

template<class T>
std::ostream& operator<<(std::ostream& l, const streamable<std::vector<T>>& r)
{
    return streamable_detail::stream_sequence_container(l, r.value);
}



/*
libutil types
*/

template<class T, size_t M, size_t N>
std::ostream& operator<<(std::ostream& l, const streamable<libutil::matrix<T, M, N>>& r)
{
    return l << streamable{r.value.data};
}

} //namespace

#endif
