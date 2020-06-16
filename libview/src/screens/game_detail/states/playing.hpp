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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_STATES_PLAYING_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_STATES_PLAYING_HPP

#include "../events.hpp"
#include "../fsm.hpp"
#include <libview/data_types.hpp>
#include <libutil/fsm.hpp>

namespace libview::screens::game_detail::states
{

class playing: public libutil::fsm::state
{
    public:
        playing(fsm& fsm):
            fsm_(fsm)
        {
        }

        void handle_event(const std::any& event);

    private:
        fsm& fsm_;
};

} //namespace

#endif
