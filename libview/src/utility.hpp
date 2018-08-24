#ifndef LIBVIEW_UTILITY_HPP
#define LIBVIEW_UTILITY_HPP

#include "libsdl.hpp"
#include <optional>
#include <iostream>

inline
bool operator==(const SDL_Rect& l, const SDL_Rect& r)
{
    return
        l.x == r.x &&
        l.y == r.y &&
        l.w == r.w &&
        l.h == r.h
    ;
}

inline
bool operator!=(const SDL_Rect& l, const SDL_Rect& r)
{
    return !(l == r);
}

namespace libview { namespace utility
{

template<class F>
void do_once(F&& f)
{
    static auto first = true;
    if(first) f();
    first = false;
}

template<class Value, class F>
void do_when_value_changes(const Value& value, F&& f)
{
    static auto old_value = std::optional<Value>{};
    if(old_value != value)
    {
        f();
        old_value = value;
    }
}

inline void print_current_viewport(SDL_Renderer& renderer)
{
    SDL_Rect r;
    SDL_RenderGetViewport(&renderer, &r);
    std::cout << "Viewport is " << r.x << ", " << r.y << ", " << r.w << ", " << r.h << ".\n";
}

inline void print_current_scale(SDL_Renderer& renderer)
{
    float x, y;
    SDL_RenderGetScale(&renderer, &x, &y);
    std::cout << "Scale is " << x << ", " << y << ".\n";
}

inline void print_current_viewport_and_scale(SDL_Renderer& renderer)
{
    print_current_viewport(renderer);
    print_current_scale(renderer);
}

}} //namespace libview::utility

#endif
