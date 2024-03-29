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

#ifndef LIBVIEW_OBJECTS_GAME_OVER_OVERLAY_HPP
#define LIBVIEW_OBJECTS_GAME_OVER_OVERLAY_HPP

#include "square.hpp"
#include "label_button.hpp"
#include "label.hpp"
#include "../common.hpp"
#include <libutil/void_function.hpp>

namespace libview::objects
{

class game_over_overlay: public object2d
{
    public:
        struct callback_set
        {
            libutil::void_function<> handle_exit_button_press;
            libutil::void_function<> handle_new_game_button_press;
        };

    public:
        game_over_overlay
        (
            object2d& parent,
            features::drawable_group& drawables,
            features::clickable_group& clickables,
            int move_count,
            int time_s,
            const callback_set& callbacks
        );

    private:
        square background_rectangle_;
        label title_label_;
        label stat_label_;
        objects::label_button exit_button_;
        objects::label_button new_game_button_;
};

} //namespace

#endif
