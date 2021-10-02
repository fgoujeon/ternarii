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

#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <libview/view.hpp>

using screen_transition = libview::view::screen_transition;

namespace events
{
    struct start{};

    struct title_screen_show_request
    {
        screen_transition transition;
    };

    struct stage_selection_screen_show_request
    {
        screen_transition transition;
    };

    struct about_screen_show_request
    {
        screen_transition transition;
    };

    struct play_screen_show_request
    {
        screen_transition transition;
        libgame::data_types::stage stage;
    };

    struct iteration
    {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        double elapsed_s = 0;
    };
}

#endif
