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

#include "../animation.hpp"
#include "../common.hpp"
#include <libview/data_types.hpp>

namespace libview::objects
{

class input: public Object2D
{
    public:
        using tile_object_matrix = libutil::matrix
        <
            std::shared_ptr<Object2D>,
            libcommon::constants::input_column_count,
            libcommon::constants::input_row_count
        >;

    public:
        input
        (
            Object2D& parent,
            animator& animator,
            tile_object_matrix& input_tile_objects
        );

        void insert_next_input
        (
            tile_object_matrix& next_input_tile_objects,
            const data_types::input_layout& layout
        );

        void set_input_layout(const data_types::input_layout& layout);

    private:
        animator& animator_;
        tile_object_matrix& input_tile_objects_;
        data_types::input_layout layout_;
};

} //namespace

#endif
