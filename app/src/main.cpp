#include "libsdl.hpp"

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
#include <emscripten.h>
#endif

#include <iostream>
#include <cstdlib>
#include <cassert>

struct context
{
    context():
        window
        (
            "Test",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_RESIZABLE
        ),
        renderer
        (
            window.ptr,
            -1,
            0
        )
    {
    }

    libsdl::session session;
    libsdl::window window;
    libsdl::renderer renderer;
    bool quit = false;
};

void process_events(context& ctx)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                ctx.quit = true;
                break;
        }
    }
}

void iterate(void *arg)
{
    auto* pctx = static_cast<context*>(arg);
    auto& ctx = *pctx;

    process_events(ctx);

    SDL_Renderer* prenderer = ctx.renderer.ptr;

    int window_width;
    int window_height;
    SDL_GetWindowSize(ctx.window.ptr, &window_width, &window_height);

    //white background
    SDL_SetRenderDrawColor(prenderer, 255, 255, 255, 255);
    SDL_RenderClear(prenderer);

    //centered red rectangle
    {
        const auto size = 50;

        SDL_Rect r;
        r.x = window_width / 2 - size / 2;
        r.y = window_height / 2 - size / 2;
        r.w = size;
        r.h = size;
        SDL_SetRenderDrawColor(prenderer, 255, 0, 0, 255);
        SDL_RenderFillRect(prenderer, &r);
    }

    SDL_RenderPresent(prenderer);
}

#ifdef EMSCRIPTEN
void sync_window_size_with_canvas_size(context& ctx)
{
    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    SDL_SetWindowSize(ctx.window.ptr, width, height);
}

int on_canvas_resize(int, const EmscriptenUiEvent*, void* arg)
{
    auto* pctx = static_cast<context*>(arg);
    auto& ctx = *pctx;
    sync_window_size_with_canvas_size(ctx);
    return 0;
}

void run(context& ctx)
{
    sync_window_size_with_canvas_size(ctx);
    emscripten_set_resize_callback(nullptr, &ctx, false, on_canvas_resize);
    emscripten_set_main_loop_arg
    (
        iterate,
        &ctx,
        -1, // call the function as fast as the browser wants to render (typically 60fps)
        1 //call the function repeatedly
    );
}
#else
void run(context& ctx)
{
    const unsigned int frame_duration = 1000 / 60;

    while(!ctx.quit)
    {
        const auto begin_time = SDL_GetTicks();
        iterate(&ctx);
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
    context ctx;
    run(ctx);
    return EXIT_SUCCESS;
}
