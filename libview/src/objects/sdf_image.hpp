/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBVIEW_OBJECTS_SDF_IMAGE_HPP
#define LIBVIEW_OBJECTS_SDF_IMAGE_HPP

#include "../common.hpp"
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>
#include <filesystem>

namespace libview::objects
{

//Signed distance field image object
class sdf_image: public object2d, public features::drawable
{
    public:
        struct style
        {
            Magnum::Color4 color;
            Magnum::Color4 outline_color;
            Magnum::Vector2 outline_range = Magnum::Vector2{0.5, 0.5};
        };

    public:
        sdf_image
        (
            object2d& parent,
            features::drawable_group& drawables,
            const std::filesystem::path& image_path,
            const style& stl
        );

        void set_color(const Magnum::Color4& color);

        void set_outline_color(const Magnum::Color4& color);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, camera& camera) override;

    private:
        style style_;
        Magnum::GL::Texture2D texture_;
};

} //namespace

#endif
