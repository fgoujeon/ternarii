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

#ifndef LIBVIEW_OBJECTS_SDF_IMAGE_BUTTON_HPP
#define LIBVIEW_OBJECTS_SDF_IMAGE_BUTTON_HPP

#include "sdf_image.hpp"
#include "../common.hpp"
#include <libutil/void_function.hpp>

namespace libview::objects
{

class sdf_image_button: public object2d, public features::clickable
{
    public:
        struct callback_set
        {
            libutil::void_function<> handle_mouse_press;
            libutil::void_function<> handle_mouse_release;
        };

    public:
        sdf_image_button
        (
            object2d& parent,
            features::drawable_group& drawables,
            features::clickable_group& clickables,
            const std::filesystem::path& image_path,
            const callback_set& callbacks
        );

    //features::clickable virtual functions
    private:
        bool is_inside(const Magnum::Vector2& model_space_position) const override;

        void handle_mouse_press() override;

        void handle_mouse_release() override;

        void handle_mouse_enter() override;

        void handle_mouse_leave() override;

    private:
        callback_set callbacks_;
        sdf_image image_;
};

} //namespace

#endif
