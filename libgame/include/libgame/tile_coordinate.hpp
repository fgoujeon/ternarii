#ifndef LIBGAME_TILE_COORDINATE_HPP
#define LIBGAME_TILE_COORDINATE_HPP

#include <ostream>

namespace libgame
{

struct tile_coordinate
{
    unsigned int x;
    unsigned int y;
};

inline
std::ostream& operator<<(std::ostream& l, const tile_coordinate& r)
{
    l << "tile_coordinate";
    l << "{";
    l << "x: " << r.x << ", ";
    l << "y: " << r.y;
    l << "}";
    return l;
}

} //namespace libgame

#endif
