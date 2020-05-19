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

#ifndef LIBVIEW_SCREENS_GAME_HPP
#define LIBVIEW_SCREENS_GAME_HPP

#include "../features/key_event_handler.hpp"
#include "../common.hpp"
#include <libview/data_types.hpp>
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

class game: public Object2D, public features::drawable, public features::animable, public features::key_event_handler
{
    public:
        struct callback_set
        {
            std::function<void(data_types::move)> handle_move_request;
            std::function<void()> handle_clear_request;
        };

    public:
        game
        (
            Object2D& parent,
            feature_group_set& feature_groups,
            const callback_set& callbacks
        );

        ~game();

        void draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera) override;

        void advance(const libutil::time_point& now) override;

        void handle_key_press(key_event& event) override;

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
