/*
Copyright 2018 - 2022 Florian Goujeon

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
#include <variant>
#include <memory>

namespace libview
{

class view
{
    public:
        using application = Magnum::Platform::Sdl2Application;
        using key              = application::KeyEvent::Key;
        using key_event        = application::KeyEvent;
        using mouse_event      = application::MouseEvent;
        using mouse_move_event = application::MouseMoveEvent;

        struct screen_transitions
        {
            struct top_to_bottom{};
            struct left_to_right{};
            struct right_to_left{};

            struct zoom_in
            {
                float duration_s = 0;
                Magnum::Vector2 new_screen_start_position;
                float new_screen_start_scaling = 1.0f;
            };

            struct zoom_out
            {
                float duration_s = 0;
                Magnum::Vector2 new_screen_start_position;
                float new_screen_start_scaling = 1.0f;
            };
        };

        using screen_transition = std::variant
        <
            screen_transitions::top_to_bottom,
            screen_transitions::left_to_right,
            screen_transitions::right_to_left,
            screen_transitions::zoom_in,
            screen_transitions::zoom_out
        >;

        struct configuration
        {
            bool show_fps_counter = false;
            bool show_debug_grid = false;
        };

    public:
        view(const configuration& conf);

        ~view();

        template<class T, class... Args>
        std::shared_ptr<T> make_screen(Args&&... args)
        {
            auto pscreen = std::make_shared<T>
            (
                get_scene(),
                get_feature_groups(),
                std::forward<Args>(args)...
            );
            pscreen->set_alpha(0.0f);

            return pscreen;
        }

        void show_screen
        (
            const std::shared_ptr<object2d>& pscreen,
            const screen_transition& trans
        );

    //Magnum event handling
    public:
        void advance
        (
            const std::chrono::steady_clock::time_point& now,
            float elapsed_s
        );

        void draw();

        void set_viewport(const Magnum::Vector2i& size);

        void handle_key_press(key_event& event);

        void handle_key_release(key_event& event);

        void handle_mouse_press(mouse_event& event);

        void handle_mouse_release(mouse_event& event);

        void handle_mouse_move(mouse_move_event& event);

    private:
        scene& get_scene();

        feature_group_set& get_feature_groups();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
