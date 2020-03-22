/*
Copyright 2018 - 2019 Florian Goujeon

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
#include <libutil/callback.hpp>

namespace libview
{

struct callback_set
{
    libutil::callback<void(data_types::move)> handle_move_request;
    libutil::callback<void()> handle_clear_request;
};

} //namespace

#endif
