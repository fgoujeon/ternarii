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
    static std::unique_ptr<controller> pcontroller;
    if(!pcontroller)
    {
        pcontroller = std::make_unique<controller>();
        sync_window_size_with_canvas_size(*pcontroller);
        emscripten_set_resize_callback(nullptr, pcontroller.get(), false, on_canvas_resize);
    }

    pcontroller->iterate();
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
