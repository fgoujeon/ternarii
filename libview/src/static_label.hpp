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

#ifndef LIBVIEW_STATIC_LABEL_HPP
#define LIBVIEW_STATIC_LABEL_HPP

#include "magnum_common.hpp"
#include <Magnum/Text/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview
{

class static_label: public Object2D, public SceneGraph::Drawable2D
{
    public:
        explicit static_label
        (
            const char* const value,
            const float font_size,
            const Magnum::Text::Alignment alignment,
            const Magnum::Color4& color,
            SceneGraph::DrawableGroup2D& drawables,
            Object2D* parent
        );

        void set_color(const Magnum::Color4& color);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override;

    private:
        Magnum::Text::Renderer2D renderer_;
        Magnum::Color4 color_;
};

} //namespace

#endif