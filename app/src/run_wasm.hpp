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

#include "controller.hpp"
#include <emscripten/html5.h>
#include <emscripten.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace
{
    enum class state
    {
        starting,
        loading_persistent_filesystem,
        loading_persistent_data,
        iterating
    };

    state current_state = state::starting;

    void async_load_persistent_filesystem()
    {
        EM_ASM(
            FS.mkdir('/persistent');
            FS.mount(IDBFS, {}, '/persistent');

            Module.persistentFilesystemLoaded = 0;
            console.log("Loading persistent filesystem...");
            FS.syncfs(true, function(err) {
                assert(!err);
                console.log("Loaded persistent filesystem.");
                Module.persistentFilesystemLoaded = 1;
            });
        );

        current_state = state::loading_persistent_filesystem;
    }

    void async_wait_for_persistent_filesystem()
    {
        if(emscripten_run_script_int("Module.persistentFilesystemLoaded") == 1)
        {
            current_state = state::loading_persistent_data;
        }
    }

    void load_persistent_data()
    {
        {
            std::ofstream ofs
            {
                "/persistent/test.txt",
                std::ios_base::out | std::ios_base::app
            };
            ofs << "t";
        }

        {
            std::ifstream ifs{"/persistent/test.txt"};
            std::stringstream buffer;
            buffer << ifs.rdbuf();

            std::cout << "content is: " << buffer.str() << std::endl;
        }

        EM_ASM(
            console.log("Saving persistent filesystem...");
            FS.syncfs(false, function(err) {
                assert(!err);
                console.log("Saved persistent filesystem.");
            });
        );

        current_state = state::iterating;
    }
}

void sync_window_size_with_canvas_size(controller& c)
{
    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    c.set_window_size(width, height);
}

int on_canvas_resize(int, const EmscriptenUiEvent*, void* arg)
{
    auto* pc = static_cast<controller*>(arg);
    auto& c = *pc;
    sync_window_size_with_canvas_size(c);
    return 0;
}

void iterate()
{
    switch(current_state)
    {
        case state::starting:
            async_load_persistent_filesystem();
            break;
        case state::loading_persistent_filesystem:
            async_wait_for_persistent_filesystem();
            break;
        case state::loading_persistent_data:
            load_persistent_data();
            break;
        case state::iterating:
            static std::unique_ptr<controller> pcontroller;
            if(!pcontroller)
            {
                pcontroller = std::make_unique<controller>();
                sync_window_size_with_canvas_size(*pcontroller);
                emscripten_set_resize_callback(nullptr, pcontroller.get(), false, on_canvas_resize);
            }
            pcontroller->iterate();
            break;
    }
}

void run()
{
    emscripten_set_main_loop
    (
        iterate,
        -1, // call the function as fast as the browser wants to render (typically 60fps)
        1 //call the function repeatedly
    );
}
