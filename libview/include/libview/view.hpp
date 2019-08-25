/*
Copyright 2018 - 2019 Florian Goujeon

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
#include <memory>

namespace libview
{

class view
{
    public:
        view(int argc, char** argv, const event_handler& evt_handler);

        ~view();

        int exec();

        void clear();

        void set_score(const unsigned int value);

        void set_hi_score(const unsigned int value);

        void create_next_input(const unsigned int value0, const unsigned int value1);

        void insert_next_input(const unsigned int x_offset, const unsigned int rotation);

        void set_input_layout(const unsigned int x_offset, const unsigned int rotation);

        void insert_input
        (
            const unsigned int tile0_dst_column_index,
            const unsigned int tile0_dst_row_index,
            const unsigned int tile1_dst_column_index,
            const unsigned int tile1_dst_row_index
        );

        void drop_tiles(const data_types::tile_drop_list& drops);

        void merge_tiles(const data_types::tile_merge_list& merges);

        void set_visible(const bool visible);

        void set_game_over_screen_visible(const bool visible);

    private:
        class impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
