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

#include <libutil/to_string.hpp>

namespace libutil
{

std::string to_string(int from)
{
    std::string str;
    auto digit_index = 0;

    do
    {
        const auto digit = from % 10;
        const auto digit_char = static_cast<char>('0' + digit);

        //add thousands separator
        if(digit_index != 0 && digit_index % 3 == 0)
            str = ' ' + str;

        //add digit
        str = std::string{digit_char} + str;

        ++digit_index;
        from /= 10;
    } while(from != 0);

    return str;
}

} //namespace
