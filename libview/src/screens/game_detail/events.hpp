/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_EVENTS_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_EVENTS_HPP

#include <libview/data_types.hpp>

namespace libview::screens::game_detail
{

//Events sent to states
namespace events
{
    struct button_press
    {
        data_types::move_button button;
    };

    struct button_release
    {
        data_types::move_button button;
    };

    struct game_over{};

    struct new_game_request{};

    struct pause_request{};

    struct iteration{};
}

} //namespace

#endif
