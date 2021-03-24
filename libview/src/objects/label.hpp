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

#ifndef LIBVIEW_OBJECTS_LABEL_HPP
#define LIBVIEW_OBJECTS_LABEL_HPP

#include "../common.hpp"
#include <Magnum/Text/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>
#include <string_view>

namespace libview::objects
{

class label: public object2d, public features::drawable
{
    public:
        struct style
        {
            Magnum::Text::Alignment alignment = Magnum::Text::Alignment::MiddleCenter;
            Magnum::Color4 color;
            const float font_size = 1.0f;
            Magnum::Color4 outline_color;
            Magnum::Vector2 outline_range = {0.5f, 0.5f};
        };

    public:
        label
        (
            object2d& parent,
            features::drawable_group& drawables,
            const style& style,
            const std::string_view& value = {}
        );

        void set_text(const std::string_view& value);

        void set_color(const Magnum::Color4& color)
        {
            style_.color = color;
        }

        void set_outline_color(const Magnum::Color4& outline_color)
        {
            style_.outline_color = outline_color;
        }

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, camera& camera) override;

    private:
        Magnum::Text::Renderer2D renderer_;
        style style_;
        bool visible_ = true;
        std::string text_;
};

} //namespace

#endif
