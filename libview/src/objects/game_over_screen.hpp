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
#include "label_button.hpp"
#include "static_label.hpp"
#include "../common.hpp"
#include <libutil/void_function.hpp>

namespace libview::objects
{

class game_over_screen: public Object2D, public features::drawable
{
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
        objects::label_button new_game_button_;
        bool visible_ = false;
};

} //namespace

#endif
