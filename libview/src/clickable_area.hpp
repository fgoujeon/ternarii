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

#ifndef LIBVIEW_CLICKABLE_AREA_HPP
#define LIBVIEW_CLICKABLE_AREA_HPP

#include "geometry.hpp"
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

        bool is_clicked() const
        {
            return clicked_;
        }

        bool is_hovered() const
        {
            return hovered_;
        }

        void set_system(const geometry::system& sys);

        void set_area(const SDL_Rect& area);

    private:
        static int static_process_event
        (
            void* pdata,
            SDL_Event* pevent
        );

        void process_event(SDL_Event& event);

        void update_window_system_area();

    private:
        geometry::system sys_;
        SDL_Rect area_;
        SDL_Rect window_system_area_;
        click_event_handler evt_handler_;
        bool clicked_ = false;
        bool hovered_ = false;
};

} //namespace libview

#endif
