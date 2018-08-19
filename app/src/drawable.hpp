#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "rectangle.hpp"
#include "libsdl.hpp"

struct drawable
{
    virtual void draw
    (
        libsdl::renderer& renderer,
        const rectangle& area
    ) = 0;
};

#endif
