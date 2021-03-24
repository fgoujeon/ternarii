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

#ifndef LIBVIEW_OBJECTS_SCORE_DISPLAY_HPP
#define LIBVIEW_OBJECTS_SCORE_DISPLAY_HPP

#include "../common.hpp"
#include <Magnum/Text/Renderer.h>

namespace libview::objects
{

class score_display: public object2d, public features::drawable
{
    public:
        explicit score_display(object2d& parent, features::drawable_group& drawables);

        void set_score(const int value);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, camera& camera) override;

    private:
        Magnum::Text::Renderer2D renderer_;
};

} //namespace

#endif
