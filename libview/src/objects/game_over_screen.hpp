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

#ifndef LIBVIEW_OBJECTS_GAME_OVER_SCREEN_HPP
#define LIBVIEW_OBJECTS_GAME_OVER_SCREEN_HPP

#include "square.hpp"
#include "static_label.hpp"
#include "../common.hpp"

namespace libview::objects
{

class game_over_screen: public Object2D, public SceneGraph::Drawable2D
{
    private:
        class new_game_button: public Object2D, public clickable
        {
            public:
                using mouse_press_callback = std::function<void()>;

            public:
                new_game_button
                (
                    const mouse_press_callback& cb,
                    SceneGraph::DrawableGroup2D& drawables,
                    clickable_group& clickables,
                    Object2D& parent
                );

                void set_enabled(const bool enabled);

            //clickable virtual functions
            private:
                bool is_inside(const Magnum::Vector2& model_space_position) const override;

                void mouse_press_event() override;

            private:
                const mouse_press_callback mouse_press_callback_;
                square background_rectangle_;
                static_label label_;
                bool enabled_ = false;
        };

    public:
        explicit game_over_screen
        (
            const std::function<void()>& new_game_button_press_callback,
            SceneGraph::DrawableGroup2D& drawables,
            clickable_group& clickables,
            Object2D& parent
        );

        void set_visible(const bool visible);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override;

    private:
        SceneGraph::DrawableGroup2D& drawables_;
        SceneGraph::DrawableGroup2D drawable_children_;
        square background_rectangle_;
        static_label label_;
        new_game_button new_game_button_;
        bool visible_ = false;
};

} //namespace

#endif
