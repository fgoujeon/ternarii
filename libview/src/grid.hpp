#ifndef LIBVIEW_GRID_HPP
#define LIBVIEW_GRID_HPP

#include "drawable.hpp"
#include "rectangle.hpp"
#include "libsdl.hpp"

namespace libview
{

class grid: public drawable
{
    public:
        grid();

        void draw
        (
            libsdl::renderer& renderer,
            const rectangle& area
        );
};

} //namespace view

#endif
