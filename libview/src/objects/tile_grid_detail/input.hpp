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

#ifndef LIBVIEW_OBJECTS_TILE_GRID_DETAIL_INPUT_HPP
#define LIBVIEW_OBJECTS_TILE_GRID_DETAIL_INPUT_HPP

#include "common.hpp"
#include "../number_tile.hpp"
#include "../../animation.hpp"
#include "../../common.hpp"
#include <libview/data_types.hpp>
#include <libcommon/constants.hpp>
#include <libutil/matrix.hpp>
#include <libutil/time.hpp>
#include <libutil/void_function.hpp>
#include <memory>

namespace libview::objects::tile_grid_detail
{

class input: public Object2D, public features::animable, public features::key_event_handler
{
    public:
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

        using drop_request_callback = libutil::void_function<const data_types::input_layout&>;

    public:
        input
        (
            Object2D& parent,
            features::animable_group& animables,
            features::key_event_handler_group& key_event_handlers,
            const drop_request_callback& drop_cb
        );

        void set_tiles(const input_tile_object_matrix& tiles);

        input_tile_object_matrix release_tile_objects();

    //Animation
    public:
        void suspend();

        void resume();

        void advance(const libutil::time_point& now, float elapsed_s) override;

    //Keyboard event handling
    public:
        void handle_key_press(key_event& event) override;

        void handle_key_release(key_event& event) override;

    private:
        void update_cog_target_position();

    private:
        animator insertion_animator_;

        drop_request_callback drop_request_callback_;

        keyboard_state keyboard_state_;

        //Current/target X position of center of gravity (COG)
        //Note: Y position of COG is always 0.
        float cog_current_x_ = 0;
        float cog_target_x_ = 0;

        int cog_target_rotation_ = 0; //in number of 90 deg clockwise rotations
        float cog_current_rotation_rad_ = 0; //in radians

        input_tile_object_matrix tiles_;

        order last_received_order_ = order::shift_left;

        bool suspended_ = false;
};

} //namespace

#endif
