/*
Copyright 2018 - 2022 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBDB_DATA_TYPES_HPP
#define LIBDB_DATA_TYPES_HPP

#include <libgame.hpp>
#include <map>

namespace libdb::data_types
{

using stage       = libgame::data_types::stage;
using stage_state = libgame::data_types::stage_state;
using tile        = libgame::data_types::tile;

using stage_state_map = std::map<stage, stage_state>;

//State of the whole game
struct game_state
{
    stage_state_map stage_states;
};

} //namespace

#endif
