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

#ifndef LIBVIEW_SCORE_DISPLAY_HPP
#define LIBVIEW_SCORE_DISPLAY_HPP

#include "magnum_common.hpp"
#include <Magnum/Text/Renderer.h>

namespace libview
{

class score_display: public Object2D, public SceneGraph::Drawable2D
{
    public:
        explicit score_display(SceneGraph::DrawableGroup2D& drawables, Object2D* parent);

        void set_score(const int value);

    private:
        void draw(const Magnum::Matrix3& transformationMatrix, SceneGraph::Camera2D& camera) override;

    private:
        Magnum::Text::Renderer2D renderer_;
};

} //namespace

#endif
