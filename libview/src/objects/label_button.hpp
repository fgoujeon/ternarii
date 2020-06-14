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

#include "label.hpp"
#include "blank_button.hpp"
#include "../common.hpp"

namespace libview::objects
{

class label_button: public object2d
{
    public:
        using callback_set = blank_button::callback_set;

        struct style
        {
            label::style label;
            blank_button::style button;
        };

    public:
        label_button
        (
            object2d& parent,
            features::drawable_group& drawables,
            features::clickable_group& clickables,
            const style& stl,
            const char* const text,
            const callback_set& callbacks
        );

        void set_enabled(const bool enabled);

    private:
        blank_button button_;
        label label_;
};

} //namespace

#endif
