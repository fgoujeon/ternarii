#ifndef BOARD_NEXT_INPUT_HPP
#define BOARD_NEXT_INPUT_HPP

#include "element.hpp"
#include <array>
#include <memory>

namespace libgame
{

typedef std::array<std::shared_ptr<element>, 2> board_next_input_t;

} //namespace libgame

#endif
