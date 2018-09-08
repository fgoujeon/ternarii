/*
Copyright 2018 Florian Goujeon

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

#ifndef LIBVIEW_EVENTS_HPP
#define LIBVIEW_EVENTS_HPP

#include "data_types.hpp"
#include <variant>
#include <functional>

namespace libview
{

namespace events
{
    struct clockwise_rotation_request{};
    struct drop_request{};
    struct left_shift_request{};
    struct right_shift_request{};
    struct clear_request{};
}

using event = std::variant
<
    events::clockwise_rotation_request,
    events::drop_request,
    events::left_shift_request,
    events::right_shift_request,
    events::clear_request
>;

using event_handler = std::function<void(const event&)>;

} //namespace libview

#endif
