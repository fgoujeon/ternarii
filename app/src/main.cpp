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

#include "states/loading_database.hpp"
#include "fsm.hpp"
#include <libdb/database.hpp>
#include <libgame/game.hpp>
#include <libview/view.hpp>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

#ifndef NDEBUG
#include <iostream>
#endif

class app: public Magnum::Platform::Sdl2Application
{
    public:
        explicit app(const Arguments& arguments):
            Magnum::Platform::Sdl2Application
            {
                arguments,
                Configuration{}.setWindowFlags(Configuration::WindowFlag::Resizable)
            },
            database_([this](const libdb::event& event){handle_database_event(event);}),
            fsm_(database_, view_)
        {
        }

    //Sdl2Application virtual functions
    private:
        void drawEvent() override
        {
            Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
            view_.draw();
            database_.iterate();
            swapBuffers();
            redraw();
        }

        void viewportEvent(ViewportEvent& event) override
        {
            Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
            view_.set_viewport(event.windowSize());
        }

        void keyPressEvent(KeyEvent& event) override
        {
            view_.handle_key_press(event);
        }

        void mousePressEvent(MouseEvent& event) override
        {
            view_.handle_mouse_press(event);
        }

        void mouseReleaseEvent(MouseEvent& event) override
        {
            view_.handle_mouse_release(event);
        }

    private:
        void handle_database_event(const libdb::event& event)
        {
            std::visit
            (
                [this](const auto& event)
                {
                    fsm_.pstate->handle_database_event(event);
                },
                event
            );
        }

    private:
        libdb::database database_;

        libview::view view_;

        fsm fsm_;
};

MAGNUM_APPLICATION_MAIN(app)
