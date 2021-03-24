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

#ifndef LIBVIEW_OBJECTS_SDF_IMAGE_TILE_HPP
#define LIBVIEW_OBJECTS_SDF_IMAGE_TILE_HPP

#include "sdf_image.hpp"
#include "rounded_rectangle.hpp"
#include "../common.hpp"
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview::objects
{

class sdf_image_tile: public object2d
{
    public:
        sdf_image_tile(object2d& parent, features::drawable_group& drawables, const std::filesystem::path& image_path);

    private:
        rounded_rectangle square_;
        sdf_image image_;
};

} //namespace

#endif
