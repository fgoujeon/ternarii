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

#ifndef LIBVIEW_VIEW_HPP
#define LIBVIEW_VIEW_HPP

#include "events.hpp"
#include "data_types.hpp"
#include <Magnum/Platform/Sdl2Application.h>
#include <memory>

namespace libview
{

class view
{
    public:
        using key_event = Magnum::Platform::Sdl2Application::KeyEvent;
        using mouse_event = Magnum::Platform::Sdl2Application::MouseEvent;

    public:
        view(const callback_set& callbacks);

        ~view();

    //Magnum event handling
    public:
        void draw();

        void set_viewport(const Magnum::Vector2i& size);

        void handle_key_press(key_event& event);

        void handle_mouse_press(mouse_event& event);

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

        void set_board_tiles(const data_types::board_tile_array& tiles);

        void set_visible(const bool visible);

        void set_game_over_screen_visible(const bool visible);

    private:
        class impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
