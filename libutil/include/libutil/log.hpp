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

#ifndef LIBUTIL_LOG_HPP
#define LIBUTIL_LOG_HPP

#include <string_view>
#include <iostream>

namespace libutil::log
{

void enable();

bool is_enabled();

namespace detail
{
    template<class... Args>
    void log_to(std::ostream& os, const Args&... args)
    {
        if(is_enabled())
        {
            (os << ... << args) << std::endl;
        }
    }
}

template<class... Args>
void info(const Args&... args)
{
    detail::log_to(std::cout, args...);
}

} //namespace

#endif
