#ifndef LIBVIEW_DRAWABLE_HPP
#define LIBVIEW_DRAWABLE_HPP

#include "rectangle.hpp"
#include "libsdl.hpp"

namespace libview
{

struct drawable
{
    virtual void draw
    (
        libsdl::renderer& renderer,
        const rectangle& area
    ) = 0;
};

} //namespace view

#endif
