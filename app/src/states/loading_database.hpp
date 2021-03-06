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

#ifndef STATES_LOADING_DATABASE_HPP
#define STATES_LOADING_DATABASE_HPP

#include "showing_title_screen.hpp"
#include "../fsm.hpp"

namespace states
{

class loading_database final: public libutil::fsm::state
{
    public:
        loading_database(fsm& ctx):
            fsm_(ctx)
        {
        }

        void handle_event(const std::any& event) override
        {
            if(const auto pevent = std::any_cast<libdb::events::end_of_loading>(&event))
            {
                fsm_.set_state<showing_title_screen>
                (
                    libview::view::screen_transition::top_to_bottom
                );
            }
        }

    private:
        fsm& fsm_;
};

} //namespace

#endif
