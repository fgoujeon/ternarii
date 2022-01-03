/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBDB_EVENTS_HPP
#define LIBDB_EVENTS_HPP

#include <variant>
#include <functional>
#include <libutil/void_function.hpp>

namespace libdb
{

namespace events
{
    struct end_of_loading{};
}

using event = std::variant
<
    events::end_of_loading
>;

using event_handler = libutil::void_function<const event&>;

} //namespace

#endif
