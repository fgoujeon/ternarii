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

#ifndef LIBVIEW_BUTTON_HPP
#define LIBVIEW_BUTTON_HPP

#include "clickable.hpp"
#include "magnum_common.hpp"
#include <Magnum/Text/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Magnum.h>

namespace libview
{

class button: public Object2D, public SceneGraph::Drawable2D, public clickable
{
    public:
        using mouse_press_callback = std::function<void()>;

    public:
        explicit button
        (
            const char* const label,
            const mouse_press_callback& cb,
            SceneGraph::DrawableGroup2D& drawables,
            clickable_group& clickables,
            Object2D* parent
        );

        void call_mouse_press_callback()
        {
            mouse_press_callback_();
        }

    //Drawable2D virtual functions
    private:
        void draw(const Magnum::Matrix3& transformationMatrix, SceneGraph::Camera2D& camera) override;

    //clickable virtual functions
    private:
        bool is_inside(const Magnum::Vector2& model_space_position) const override;

        void mouse_press_event() override;

    private:
        const mouse_press_callback mouse_press_callback_;
        Magnum::Text::Renderer2D text_renderer_;
};

} //namespace

#endif
