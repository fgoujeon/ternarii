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

#ifndef LIBVIEW_OBJECTS_TILE_GRID_DETAIL_NEXT_INPUT_HPP
#define LIBVIEW_OBJECTS_TILE_GRID_DETAIL_NEXT_INPUT_HPP

#include "common.hpp"
#include "../../animation.hpp"
#include "../../common.hpp"
#include <libview/data_types.hpp>

namespace libview::objects::tile_grid_detail
{

class next_input: public object2d, public features::animable
{
    public:
        next_input
        (
            object2d& parent,
            features::drawable_group& drawables,
            features::animable_group& animables
        );

        void create_tiles(const data_types::input_tile_matrix& tiles);

        input_tile_object_matrix release_tile_objects();

    //Animation
    public:
        void suspend();

        void resume();

        void advance(const libutil::time_point& now, float elapsed_s) override;

    private:
        features::drawable_group& drawables_;
        features::animable_group& animables_;

        animation::animator animator_;
        bool suspended_ = false;
        input_tile_object_matrix tile_objects_;
};

} //namespace

#endif
