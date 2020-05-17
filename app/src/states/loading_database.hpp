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

#ifndef STATES_LOADING_DATABASE_HPP
#define STATES_LOADING_DATABASE_HPP

#include "playing.hpp"
#include "../fsm.hpp"

namespace states
{

class loading_database final: public state
{
    public:
        loading_database(fsm& ctx):
            fsm_(ctx)
        {
        }

        void handle_database_event(const libdb::events::end_of_loading&) override
        {
            fsm_.pstate = std::make_unique<playing>(fsm_);
        }

    private:
        fsm& fsm_;
};

} //namespace

#endif
