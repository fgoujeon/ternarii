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

#ifndef STATES_SHOWING_STAGE_SELECTION_SCREEN_HPP
#define STATES_SHOWING_STAGE_SELECTION_SCREEN_HPP

#include "../fsm.hpp"
#include <libview/screens/stage_selection.hpp>

namespace states
{

class showing_stage_selection_screen final: public state
{
    public:
        using screen_transition = libview::view::screen_transition;

    private:
        using screen = libview::screens::stage_selection;

    public:
        showing_stage_selection_screen(fsm& ctx, screen_transition trans);

    private:
        fsm& fsm_;
        std::shared_ptr<screen> pscreen_;
};

} //namespace

#endif
