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

#ifndef LIBCLIENT_CLIENT_HPP
#define LIBCLIENT_CLIENT_HPP

#include "data_types.hpp"
#include <libutil/void_function.hpp>

namespace libclient
{

class client
{
    public:
        using async_get_or_add_game_success_callback =
            libutil::void_function<const data_types::game&>
        ;

        using async_get_or_add_game_failure_callback =
            libutil::void_function<const std::string&>
        ;

    public:
        void async_get_or_add_game
        (
            int player_id,
            const std::string& player_password,
            int stage_id,
            const async_get_or_add_game_success_callback& success_callback,
            const async_get_or_add_game_failure_callback& failure_callback
        );
};

} //namespace

#endif
