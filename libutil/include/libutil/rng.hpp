/*
Copyright 2018 - 2021 Florian Goujeon

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

#ifndef LIBUTIL_RNG_HPP
#define LIBUTIL_RNG_HPP

#include <random>

namespace libutil
{

class rng
{
    public:
        uint64_t operator()()
        {
            return static_cast<uint64_t>(engine_());
        }

    private:
        std::random_device device_;
        std::mt19937_64 engine_{device_()};
};

} //namespace

#endif
