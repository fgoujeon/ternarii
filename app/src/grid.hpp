#ifndef GRID_HPP
#define GRID_HPP

#include "drawable.hpp"
#include "rectangle.hpp"
#include "libsdl.hpp"

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

#endif
