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

#ifndef FSM_HPP
#define FSM_HPP

#include <libdb/database.hpp>
#include <libgame/game.hpp>
#include <libview/view.hpp>
#include <memory>

struct state
{
    virtual ~state() = default;
    virtual void handle_database_event(const libdb::events::end_of_loading&){}
};

//Finite state machine
struct fsm
{
    fsm
    (
        libdb::database& database,
        libview::view& view
    );

    template<class State, class... Args>
    void set_state(Args&&... args)
    {
        pstate = std::make_unique<State>(*this, std::forward<Args>(args)...);
    }

    std::unique_ptr<state> pstate;

    libdb::database& database;
    libview::view& view;
};

#endif
