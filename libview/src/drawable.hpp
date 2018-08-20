#ifndef LIBVIEW_DRAWABLE_HPP
#define LIBVIEW_DRAWABLE_HPP

#include "libsdl.hpp"

namespace libview
{

struct drawable
{
    virtual void draw
    (
        SDL_Renderer& renderer,
        const SDL_Rect& area
    ) = 0;
};

} //namespace view

#endif
