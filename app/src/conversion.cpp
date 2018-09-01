#include "conversion.hpp"

namespace conversion
{

libview::tile_coordinate to_view(const libgame::tile_coordinate& from)
{
    auto to = libview::tile_coordinate{};
    to.x = from.x;
    to.y = from.y;
    return to;
}

} //namespace conversion
