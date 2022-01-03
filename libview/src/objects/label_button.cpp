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

#include "label_button.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <libres.hpp>
#include <Magnum/Text/AbstractFont.h>

namespace libview::objects
{

label_button::label_button
(
    object2d& parent,
    features::drawable_group& drawables,
    features::clickable_group& clickables,
    const style& stl,
    const char* const text,
    const callback_set& callbacks
):
    object2d{&parent},
    button_
    (
        *this,
        drawables,
        clickables,
        stl.button,
        callbacks
    ),
    label_
    (
        *this,
        drawables,
        stl.label,
        text
    )
{
}

void label_button::set_enabled(const bool enabled)
{
    button_.set_enabled(enabled);
}

} //namespace
