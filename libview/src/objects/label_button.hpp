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

#ifndef LIBVIEW_OBJECTS_LABEL_BUTTON_HPP
#define LIBVIEW_OBJECTS_LABEL_BUTTON_HPP

#include "static_label.hpp"
#include "sdf_image.hpp"
#include "../common.hpp"
#include <libutil/void_function.hpp>

namespace libview::objects
{

class label_button: public Object2D, public features::clickable
{
    public:
        struct callback_set
        {
            libutil::void_function<> mouse_press_callback;
            libutil::void_function<> mouse_release_callback;
        };

        struct style
        {
            float label_size = 1.0f;
        };

    public:
        label_button
        (
            Object2D& parent,
            features::drawable_group& drawables,
            features::clickable_group& clickables,
            const char* const text,
            const style& stl,
            const callback_set& callbacks
        );

        void set_enabled(const bool enabled);

    //features::clickable virtual functions
    private:
        bool is_inside(const Magnum::Vector2& model_space_position) const override;

        void handle_mouse_press() override;

        void handle_mouse_release() override;

        void handle_mouse_enter() override;

        void handle_mouse_leave() override;

    private:
        style style_;
        const callback_set callbacks_;
        sdf_image background_rectangle_;
        static_label label_;
        bool enabled_ = true;
};

} //namespace

#endif
