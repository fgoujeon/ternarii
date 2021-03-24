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

#ifndef LIBVIEW_OBJECTS_GAME_MENU_OVERLAY_HPP
#define LIBVIEW_OBJECTS_GAME_MENU_OVERLAY_HPP

#include "rounded_rectangle.hpp"
#include "square.hpp"
#include "label_button.hpp"
#include "label.hpp"
#include "../common.hpp"
#include <libutil/void_function.hpp>
#include <chrono>

namespace libview::objects
{

class game_menu_overlay: public object2d
{
    public:
        struct callback_set
        {
            libutil::void_function<> handle_exit_request;
            libutil::void_function<> handle_resume_request;
        };

    public:
        game_menu_overlay
        (
            object2d& parent,
            features::drawable_group& drawables,
            features::clickable_group& clickables,
            const callback_set& callbacks
        );

        void set_time_s(int value);

        void set_move_count(int value);

        void set_hi_score(int value);

    private:
        void update_game_time();

    private:
        square triangle_;
        rounded_rectangle background_rectangle_;

        label time_name_label_;
        label time_value_label_;

        label move_count_name_label_;
        label move_count_value_label_;

        label hi_score_name_label_;
        label hi_score_value_label_;

        label_button resume_button_;
        label_button exit_button_;
        label save_note_label_;
};

} //namespace

#endif
