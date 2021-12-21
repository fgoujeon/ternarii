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

#ifndef LIBVIEW_OBJECTS_TILE_GRID_DETAIL_INPUT_HPP
#define LIBVIEW_OBJECTS_TILE_GRID_DETAIL_INPUT_HPP

#include "common.hpp"
#include "../number_tile.hpp"
#include "../../animation.hpp"
#include "../../common.hpp"
#include <libview/data_types.hpp>
#include <libgame.hpp>
#include <libutil/matrix.hpp>
#include <libutil/void_function.hpp>
#include <chrono>
#include <memory>

namespace libview::objects::tile_grid_detail
{

class input: public object2d, public features::animable
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
        using layout_change_callback = libutil::void_function<const data_types::input_layout&>;

    public:
        input
        (
            object2d& parent,
            features::animable_group& animables,
            const drop_request_callback& drop_cb,
            const layout_change_callback& layout_cb
        );

        const data_types::input_layout& get_layout() const;

        void set_tiles(const input_tile_object_matrix& tiles);

        input_tile_object_matrix release_tile_objects();

    //Animation
    public:
        void suspend();

        void resume();

        void advance(const std::chrono::steady_clock::time_point& now, float elapsed_s) override;

    //Keyboard event handling
    public:
        void handle_button_press(data_types::move_button button);

        void handle_button_release(data_types::move_button button);

    private:
        void update_cog_target_position();

        void update_layout();

    private:
        animation::animator insertion_animator_;

        drop_request_callback drop_request_callback_;
        layout_change_callback layout_change_callback_;

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
        bool settled_ = false;

        data_types::input_layout layout_;
};

} //namespace

#endif
