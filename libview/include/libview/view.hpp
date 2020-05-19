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

#ifndef LIBVIEW_VIEW_HPP
#define LIBVIEW_VIEW_HPP

#include "screens/game.hpp"
#include "data_types.hpp"
#include <Magnum/Platform/Sdl2Application.h>
#include <memory>

namespace libview
{

class view
{
    public:
        using key_event = Magnum::Platform::Sdl2Application::KeyEvent;
        using mouse_event = Magnum::Platform::Sdl2Application::MouseEvent;

    public:
        view();

        ~view();

        template<class T, class... Args>
        std::shared_ptr<T> make_screen(Args&&... args)
        {
            return std::make_shared<T>
            (
                get_scene(),
                get_feature_groups(),
                std::forward<Args>(args)...
            );
        }

    //Magnum event handling
    public:
        void draw();

        void set_viewport(const Magnum::Vector2i& size);

        void handle_key_press(key_event& event);

        void handle_mouse_press(mouse_event& event);

        void handle_mouse_release(mouse_event& event);

    private:
        Scene2D& get_scene();

        feature_group_set& get_feature_groups();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
