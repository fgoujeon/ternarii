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
#include <libutil/void_function.hpp>

namespace libview::objects
{

class game_over_screen: public Object2D, public features::drawable
{
    private:
        class new_game_button: public Object2D, public features::clickable
        {
            public:
                using mouse_press_callback = libutil::void_function<>;

            public:
                new_game_button
                (
                    Object2D& parent, features::drawable_group& drawables, features::clickable_group& clickables,
                    const mouse_press_callback& cb
                );

                void set_enabled(const bool enabled);

            //features::clickable virtual functions
            private:
                bool is_inside(const Magnum::Vector2& model_space_position) const override;

                void handle_mouse_press() override;

            private:
                const mouse_press_callback mouse_press_callback_;
                square background_rectangle_;
                static_label label_;
                bool enabled_ = false;
        };

    public:
        game_over_screen
        (
            Object2D& parent,
            features::drawable_group& drawables,
            features::clickable_group& clickables,
            const libutil::void_function<>& new_game_button_press_callback
        );

        void set_visible(const bool visible);

    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override;

    private:
        features::drawable_group& drawables_;
        features::drawable_group drawable_children_;
        square background_rectangle_;
        static_label label_;
        new_game_button new_game_button_;
        bool visible_ = false;
};

} //namespace

#endif
