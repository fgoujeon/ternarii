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
#include <libutil/log.hpp>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Corrade/Utility/Arguments.h>

namespace
{
    struct configuration
    {
        bool show_debug_grid = false;
        bool enable_log = false;
        bool show_fps_counter = false;
    };

    configuration parse_command_line(const int argc, char** const argv)
    {
        Corrade::Utility::Arguments args;
        args.addBooleanOption("debug-grid").setHelp("debug-grid", "show debug grid");
        args.addBooleanOption("log").setHelp("log", "enable log");
        args.addBooleanOption("fps").setHelp("log", "show FPS counter");

        if(args.tryParse(argc, argv))
        {
            return configuration
            {
                .show_debug_grid = args.isSet("debug-grid"),
                .enable_log = args.isSet("log"),
                .show_fps_counter = args.isSet("fps")
            };
        }
        else
        {
            return configuration{};
        }
    }

    struct configurator
    {
        configurator(const configuration& conf)
        {
            if(conf.enable_log)
            {
                libutil::log::enable();
            }
        }
    };
}

class app: public Magnum::Platform::Sdl2Application
{
    public:
        explicit app(const Arguments& args):
            Magnum::Platform::Sdl2Application
            {
                args,
                Configuration{}.setWindowFlags(Configuration::WindowFlag::Resizable)
            },
            conf_(parse_command_line(args.argc, args.argv)),
            configurator_(conf_),
            database_([this](const libdb::event& event){handle_database_event(event);}),
            view_
            (
                libview::view::configuration
                {
                    .show_fps_counter = conf_.show_fps_counter,
                    .show_debug_grid = conf_.show_debug_grid
                }
            ),
            fsm_(ctx_)
        {
            fsm_.set_state<states::loading_database>();
        }

    //Sdl2Application virtual functions
    private:
        void drawEvent() override
        {
            Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
            view_.draw();
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

        void keyReleaseEvent(KeyEvent& event) override
        {
            view_.handle_key_release(event);
        }

        void mousePressEvent(MouseEvent& event) override
        {
            view_.handle_mouse_press(event);
        }

        void mouseReleaseEvent(MouseEvent& event) override
        {
            view_.handle_mouse_release(event);
        }

        void mouseMoveEvent(MouseMoveEvent& event) override
        {
            view_.handle_mouse_move(event);
        }

    private:
        void handle_database_event(const libdb::event& event)
        {
            std::visit
            (
                [this](const auto& event)
                {
                    fsm_.handle_event(event);
                },
                event
            );
        }

    private:
        configuration conf_;
        configurator configurator_;
        libdb::database database_;
        libview::view view_;
        fsm_context ctx_{database_, view_};
        fsm fsm_;
};

MAGNUM_APPLICATION_MAIN(app)
