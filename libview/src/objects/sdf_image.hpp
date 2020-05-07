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

#ifndef LIBVIEW_OBJECTS_SDF_IMAGE_HPP
#define LIBVIEW_OBJECTS_SDF_IMAGE_HPP

#include "../magnum_common.hpp"
#include <Magnum/GL/Texture.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>
#include <filesystem>

namespace libview::objects
{

//Signed distance field image object
class sdf_image: public Object2D, public SceneGraph::Drawable2D
{
    public:
        explicit sdf_image
        (
            const std::filesystem::path& image_path,
            SceneGraph::DrawableGroup2D& drawables,
            Object2D* parent
        );

        void set_color(const Magnum::Color4& color);

        void set_outline_color(const Magnum::Color4& color);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override;

    private:
        Magnum::GL::Texture2D texture_;
        Magnum::Color4 color_;
        Magnum::Color4 outline_color_;
};

} //namespace

#endif
