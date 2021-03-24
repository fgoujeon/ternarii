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

#ifndef FSM_HPP
#define FSM_HPP

#include <libdb/database.hpp>
#include <libview/view.hpp>
#include <libutil/fsm.hpp>

namespace events
{
    struct iteration
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        double elapsed_s = 0;
    };
};

struct fsm_context
{
    libdb::database& database;
    libview::view& view;
};

//Finite state machine
using fsm = libutil::fsm::fsm<fsm_context>;

#endif
