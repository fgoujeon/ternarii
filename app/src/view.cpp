#include "view.hpp"

view::view():
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

void view::set_window_size(const unsigned int width, const unsigned int height)
{
    SDL_SetWindowSize(window.ptr, width, height);
}

void view::iterate()
{
    process_events();

    int window_width;
    int window_height;
    SDL_GetWindowSize(window.ptr, &window_width, &window_height);

    //white background
    SDL_SetRenderDrawColor(renderer.ptr, 255, 255, 255, 255);
    SDL_RenderClear(renderer.ptr);

    //centered red rectangle
    {
        const auto size = 50;

        SDL_Rect r;
        r.x = window_width / 2 - size / 2;
        r.y = window_height / 2 - size / 2;
        r.w = size;
        r.h = size;
        SDL_SetRenderDrawColor(renderer.ptr, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer.ptr, &r);
    }

    SDL_RenderPresent(renderer.ptr);
}

bool view::must_quit()
{
    return quit;
}

void view::process_events()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
        }
    }
}
