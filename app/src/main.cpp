#include <libview/view.hpp>

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
#include <emscripten.h>
#endif

#include <iostream>
#include <cstdlib>
#include <cassert>

#ifdef EMSCRIPTEN
void iterate(void* arg)
{
    auto pv = reinterpret_cast<libview::view*>(arg);
    pv->iterate();
}

void sync_window_size_with_canvas_size(libview::view& v)
{
    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    v.set_window_size(width, height);
}

int on_canvas_resize(int, const EmscriptenUiEvent*, void* arg)
{
    auto* pctx = static_cast<libview::view*>(arg);
    auto& v = *pctx;
    sync_window_size_with_canvas_size(v);
    return 0;
}

void run(libview::view& v)
{
    sync_window_size_with_canvas_size(v);
    emscripten_set_resize_callback(nullptr, &v, false, on_canvas_resize);
    emscripten_set_main_loop_arg
    (
        iterate,
        &v,
        -1, // call the function as fast as the browser wants to render (typically 60fps)
        1 //call the function repeatedly
    );
}
#else
void run(libview::view& v)
{
    const unsigned int frame_duration = 1000 / 60;

    while(!v.must_quit())
    {
        const auto begin_time = SDL_GetTicks();
        v.iterate();
        const auto end_time = SDL_GetTicks();

        //regulate the frame rate
        assert(end_time >= begin_time);
        const auto elapsed_time = end_time - begin_time;
        if(elapsed_time < frame_duration)
        {
            const Uint32 remaining_time = frame_duration - elapsed_time;
            SDL_Delay(remaining_time);
        }
    }
}
#endif

int main(int, char**)
{
    libview::view::callback_set callbacks;
    callbacks.left_shift = []{std::cout << "left\n";};
    callbacks.right_shift = []{std::cout << "right\n";};
    callbacks.clockwise_rotation = []{std::cout << "clockwise_rotation\n";};
    callbacks.down = []{std::cout << "down\n";};

    libview::view v{callbacks};
    run(v);
    return EXIT_SUCCESS;
}
