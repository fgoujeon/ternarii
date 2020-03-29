/*
Copyright 2018 - 2020 Florian Goujeon

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

#ifndef LIBDB_JSON_CONVERSION_HPP
#define LIBDB_JSON_CONVERSION_HPP

#include <libdb/data_types.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann
{
    template<typename T>
    struct adl_serializer<std::optional<T>>
    {
        static void to_json(json& to, const std::optional<T>& from)
        {
            if(from == std::nullopt)
            {
                to = nullptr;
            }
            else
            {
                to = *from;
            }
        }

        static void from_json(const json& from, std::optional<T>& to)
        {
            if(from.is_null())
            {
                to = std::nullopt;
            }
            else
            {
                to = from.get<T>();
            }
        }
    };
}

namespace libgame::data_types
{

void to_json(nlohmann::json& to, const tile& from)
{
    to = from.value;
}

void from_json(const nlohmann::json& from, tile& to)
{
    to.value = from.get<int>();
}



void to_json(nlohmann::json& to, const game_state& from)
{
    to["hiScore"]        = from.hi_score;
    to["nextInputTiles"] = from.next_input_tiles;
    to["inputTiles"]     = from.input.tiles;
    to["inputXOffset"]   = from.input.x_offset;
    to["inputRotation"]  = from.input.rotation;
    to["boardTiles"]     = from.board_tiles;
}

void from_json(const nlohmann::json& from, game_state& to)
{
    to.hi_score         = from["hiScore"].get<int>();
    to.next_input_tiles = from["nextInputTiles"];
    to.input.tiles      = from["inputTiles"];
    to.input.x_offset   = from["inputXOffset"].get<int>();
    to.input.rotation   = from["inputRotation"].get<int>();
    to.board_tiles      = from["boardTiles"];
}

} //namespace

#endif
