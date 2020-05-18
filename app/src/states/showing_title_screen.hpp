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

#ifndef STATES_SHOWING_TITLE_SCREEN_HPP
#define STATES_SHOWING_TITLE_SCREEN_HPP

#include "playing.hpp"
#include "showing_about_screen.hpp"
#include "../fsm.hpp"
#include <libview/screens/title.hpp>

namespace states
{

class showing_title_screen final: public state
{
    private:
        using screen = libview::screens::title;

    public:
        showing_title_screen(fsm& ctx):
            fsm_(ctx)
        {
            auto callbacks = screen::callback_set{};
            callbacks.play_request.assign<&showing_title_screen::handle_play_request>(*this);
            callbacks.about_request.assign<&showing_title_screen::handle_about_request>(*this);
            pscreen_ = fsm_.view.make_screen<libview::screens::title>(callbacks);
        }

    private:
        void handle_play_request()
        {
            fsm_.set_state<playing>();
        }

        void handle_about_request()
        {
            fsm_.set_state<showing_about_screen>();
        }

    private:
        fsm& fsm_;
        std::shared_ptr<screen> pscreen_;
};

} //namespace

#endif
