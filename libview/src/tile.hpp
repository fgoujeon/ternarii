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

#ifndef LIBVIEW_TILE_HPP
#define LIBVIEW_TILE_HPP

#include "magnum_common.hpp"
#include <Magnum/Text/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview
{

class tile: public Object2D, public SceneGraph::Drawable2D
{
    public:
        explicit tile(const int value, SceneGraph::DrawableGroup2D& drawables, Object2D* parent);

        void set_alpha(const float alpha);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override;

    private:
        Magnum::Text::Renderer2D text_renderer_;
        Magnum::Color3 square_color_;
        float alpha_ = 0;
};

} //namespace

#endif
