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

#ifndef LIBVIEW_SCREENS_VERSUS_GAME_HPP
#define LIBVIEW_SCREENS_VERSUS_GAME_HPP

#include "../features/key_event_handler.hpp"
#include "../common.hpp"
#include <libview/data_types.hpp>
#include <chrono>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <libutil/void_function.hpp>
#include <filesystem>

namespace libview::screens
{

class versus_game: public object2d, public features::animable, public features::key_event_handler
{
    public:
        struct callback_set
        {
            libutil::void_function<> handle_clear_request;
            libutil::void_function<> handle_exit_request;
            libutil::void_function<const data_types::input_layout&> handle_p1_drop_request;
            libutil::void_function<const data_types::input_layout&> handle_p1_input_layout_change;
            libutil::void_function<const data_types::input_layout&> handle_p2_drop_request;
            libutil::void_function<const data_types::input_layout&> handle_p2_input_layout_change;
        };

    public:
        versus_game
        (
            object2d& parent,
            feature_group_set& feature_groups,
            data_types::stage stage,
            const callback_set& callbacks
        );

        ~versus_game();

    //animable overrides
    private:
        void advance(const std::chrono::steady_clock::time_point& now, float elapsed_s) override;

    //key_event_handler overrides
    private:
        void handle_key_press(key_event& event) override;

        void handle_key_release(key_event& event) override;

    public:
        const data_types::input_layout& get_p1_input_layout() const;

        const data_types::input_layout& get_p2_input_layout() const;

        void clear();

        void set_p1_score(const int value);

        void set_p2_score(const int value);

        void create_p1_next_input(const data_types::input_tile_matrix& tiles);

        void create_p2_next_input(const data_types::input_tile_matrix& tiles);

        void insert_p1_next_input();

        void insert_p2_next_input();

        void drop_p1_input_tiles(const data_types::input_tile_drop_list& drops);

        void drop_p2_input_tiles(const data_types::input_tile_drop_list& drops);

        void drop_p1_board_tiles(const data_types::board_tile_drop_list& drops);

        void drop_p2_board_tiles(const data_types::board_tile_drop_list& drops);

        void nullify_p1_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates);

        void nullify_p2_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates);

        void merge_p1_tiles
        (
            const data_types::tile_merge_list& merges,
            const data_types::granite_erosion_list& granite_erosions
        );

        void merge_p2_tiles
        (
            const data_types::tile_merge_list& merges,
            const data_types::granite_erosion_list& granite_erosions
        );

        void mark_p1_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates);

        void mark_p2_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates);

        void set_game_over_overlay_visible(const bool visible);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
