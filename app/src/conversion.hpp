#ifndef CONVERSION_HPP
#define CONVERSION_HPP

#include <libview/data_types.hpp>
#include <libgame/tile_coordinate.hpp>

namespace conversion
{

libview::tile_coordinate to_view(const libgame::tile_coordinate& from);

template<class T>
auto to_view(const std::vector<T>& from)
{
    using to_item_t = decltype(to_view(std::declval<T>()));
    std::vector<to_item_t> to;
    for(const auto& from_item: from)
        to.push_back(to_view(from_item));
    return to;
}

} //namespace conversion

#endif
