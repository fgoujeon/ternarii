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
#include <libutil/void_function.hpp>

namespace libview::screens
{

class game: public Object2D, public features::key_event_handler
{
    public:
        struct callback_set
        {
            libutil::void_function<data_types::move> handle_move_request;
            libutil::void_function<> handle_clear_request;
            libutil::void_function<> handle_exit_request;
        };

    public:
        game
        (
            Object2D& parent,
            feature_group_set& feature_groups,
            const callback_set& callbacks,
            const std::string_view& stage_name,
            bool show_background
        );

        ~game();

        void handle_key_press(key_event& event) override;

    public:
        void clear();

        void set_score(const int value);

        void set_hi_score(const int value);

        void create_next_input(const data_types::input_tile_matrix& tiles);

        void insert_next_input(const data_types::input_layout& layout);

        void set_input_layout(const data_types::input_layout& layout);

        void drop_input_tiles(const data_types::input_tile_drop_list& drops);

        void drop_board_tiles(const data_types::board_tile_drop_list& drops);

        void nullify_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates);

        void merge_tiles(const data_types::tile_merge_list& merges);

        void mark_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates);

        void set_board_tiles(const data_types::board_tile_matrix& tiles);

        void set_game_over_screen_visible(const bool visible);

    private:
        void send_move_request(const data_types::move move);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
