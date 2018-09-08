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

#ifndef LIBVIEW_BUTTON_HPP
#define LIBVIEW_BUTTON_HPP

#include <libsdl.hpp>
#include <functional>

namespace libview
{

class button
{
    public:
        using click_event_handler = std::function<void()>;

    public:
        button(const click_event_handler& evt_handler);

        ~button();

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
        click_event_handler evt_handler_;
        SDL_Rect area_ = SDL_Rect{0, 0, 100, 100};
};

} //namespace libview

#endif
