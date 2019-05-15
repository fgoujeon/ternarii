/*
Copyright 2018 - 2019 Florian Goujeon

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

#ifndef LIBVIEW_LABEL_BUTTON_HPP
#define LIBVIEW_LABEL_BUTTON_HPP

#include "clickable_area.hpp"
#include "label.hpp"
#include "geometry.hpp"
#include <libview/events.hpp>
#include <libsdl.hpp>

namespace libview
{

class label_button
{
    public:
        using click_event_handler = std::function<void()>;

        label_button
        (
            SDL_Renderer& renderer,
            const SDL_Rect& area,
            const unsigned int font_size,
            const std::string& text,
            const click_event_handler& evt_handler
        );

        void draw(const geometry::system& sys);

    private:
        SDL_Renderer& renderer_;
        clickable_area clickable_area_;
        label label_;
};

} //namespace libview

#endif
