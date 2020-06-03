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

#ifndef LIBVIEW_OBJECTS_INPUT_HPP
#define LIBVIEW_OBJECTS_INPUT_HPP

#include "number_tile.hpp"
#include "../animation.hpp"
#include "../common.hpp"
#include <libview/data_types.hpp>
#include <libcommon/constants.hpp>
#include <libutil/matrix.hpp>
#include <libutil/time.hpp>
#include <Magnum/Animation/Player.h>
#include <memory>

namespace libview::objects
{

class input: public Object2D, public features::animable, public features::key_event_handler
{
    public:
        using tile_matrix = libutil::matrix
        <
            std::shared_ptr<Object2D>,
            libcommon::constants::input_column_count,
            libcommon::constants::input_row_count
        >;

        using position_matrix = libutil::matrix
        <
            Magnum::Vector2,
            libcommon::constants::input_column_count,
            libcommon::constants::input_row_count
        >;

        enum class order
        {
            rotate,
            shift_left,
            shift_right
        };

        struct keyboard_state
        {
            bool left_shift_button_pressed = false;
            bool right_shift_button_pressed = false;
            bool rotate_button_pressed = false;
        };

    public:
        input
        (
            Object2D& parent,
            features::drawable_group& drawables,
            features::animable_group& animables,
            features::key_event_handler_group& key_event_handlers
        );

        void advance(const libutil::time_point& now, float elapsed_s) override;

        void handle_key_press(key_event& event) override;

        void handle_key_release(key_event& event) override;

    private:
        void update_target_positions();

    private:
        features::drawable_group& drawables_;

        keyboard_state keyboard_state_;

        //Current/target X position of center of gravity (COG)
        //Note: Y position of COG is always 0.
        float current_x_cog_ = 0;
        float target_x_cog_ = 0;

        tile_matrix tiles_ = {};
        position_matrix target_positions_;

        int rotation_ = 0;
        order last_received_order_ = order::shift_left;
};

} //namespace

#endif
