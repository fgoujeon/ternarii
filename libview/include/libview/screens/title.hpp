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

#ifndef LIBVIEW_SCREENS_TITLE_HPP
#define LIBVIEW_SCREENS_TITLE_HPP

#include <libview/events.hpp>
#include "../common.hpp"
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

class title: public Object2D
{
    public:
        struct callback_set
        {
            libutil::callback<void()> play_request;
        };

    public:
        title
        (
            Object2D& parent,
            feature_group_set& feature_groups,
            const callback_set& callbacks
        );

        ~title();

    public:
        void clear();

        void set_score(const int value);

        void set_hi_score(const int value);

        void create_next_input(const data_types::input_tile_array& tiles);

        void insert_next_input(const data_types::input_layout& layout);

        void set_input_layout(const data_types::input_layout& layout);

        void drop_input_tiles(const data_types::input_tile_drop_list& drops);

        void drop_board_tiles(const data_types::board_tile_drop_list& drops);

        void nullify_tiles(const data_types::tile_coordinate_list& nullified_tile_coordinates);

        void merge_tiles(const data_types::tile_merge_list& merges);

        void mark_tiles_for_nullification(const data_types::tile_coordinate_list& tile_coordinates);

        void set_board_tiles(const data_types::board_tile_array& tiles);

        void set_visible(const bool visible);

        void set_game_over_screen_visible(const bool visible);

    private:
        void send_move_request(const data_types::move move);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
