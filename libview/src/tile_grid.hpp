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

#ifndef LIBVIEW_TILE_GRID_HPP
#define LIBVIEW_TILE_GRID_HPP

#include "magnum_common.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/VertexColor.h>

namespace libview
{

class tile_grid: public Object2D
{
    public:
        explicit tile_grid(SceneGraph::DrawableGroup2D& drawables, Object2D* parent);
};

} //namespace

#endif
