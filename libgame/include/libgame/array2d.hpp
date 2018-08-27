#ifndef LIBGAME_ARRAY2D_HPP
#define LIBGAME_ARRAY2D_HPP

#include <array>

namespace libgame
{

template<typename T, size_t Size0, size_t Size1>
using array2d = std::array<std::array<T, Size1>, Size0>;

} //namespace libgame

#endif
