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

#ifndef LIBVIEW_GAME_OVER_SCREEN_HPP
#define LIBVIEW_GAME_OVER_SCREEN_HPP

#include "square.hpp"
#include "clickable.hpp"
#include "magnum_common.hpp"
#include <Magnum/Text/Renderer.h>

namespace libview
{

class game_over_screen: public Object2D, public SceneGraph::Drawable2D
{
    private:
        class new_game_button;

    public:
        explicit game_over_screen
        (
            const std::function<void()>& new_game_button_press_callback,
            SceneGraph::DrawableGroup2D& drawables,
            clickable_group& clickables,
            Object2D* parent
        );

        void set_visible(const bool visible);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override;

    private:
        SceneGraph::DrawableGroup2D& drawables_;
        SceneGraph::DrawableGroup2D drawable_children_;
        square& background_rectangle_;
        bool visible_ = false;
        Magnum::Text::Renderer2D text_renderer_;
        new_game_button& new_game_button_;
};

} //namespace

#endif
