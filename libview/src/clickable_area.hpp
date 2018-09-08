/*
Copyright 2018 Florian Goujeon

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

#ifndef LIBVIEW_CLICKABLE_AREA_HPP
#define LIBVIEW_CLICKABLE_AREA_HPP

#include <libsdl.hpp>
#include <functional>

namespace libview
{

class clickable_area
{
    public:
        using click_event_handler = std::function<void()>;

    public:
        clickable_area
        (
            const SDL_Rect& area,
            const click_event_handler& evt_handler
        );

        ~clickable_area();

        const SDL_Rect& get_area() const;

        void set_area(const SDL_Rect& area);

        void draw(SDL_Renderer& renderer);

    private:
        static int static_process_event
        (
            void* pdata,
            SDL_Event* pevent
        );

        void process_event(SDL_Event& event);

    private:
        SDL_Rect area_;
        click_event_handler evt_handler_;
};

} //namespace libview

#endif
