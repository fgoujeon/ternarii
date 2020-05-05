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

//Add support of std::optional
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

namespace libcommon::data_types
{

void to_json(nlohmann::json& to, const tile& from)
{
    to = from.value;
}

void from_json(const nlohmann::json& from, tile& to)
{
    to.value = from.get<int>();
}

} //namespace


namespace libgame::data_types
{

namespace
{
    void from_json_0(const nlohmann::json& from, game_state& to)
    {
        /*
        Example:

        {
            "boardTiles":
            [
                [1,    0,    null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null]
            ],
            "hiScore":3,
            "inputRotation":0,
            "inputTiles":[0,1],
            "inputXOffset":2,
            "nextInputTiles":[2,0]
        }
        */

        const auto from_tile_array1d = [](const nlohmann::json& from, auto& to)
        {
            auto i = 0;
            for(const auto& json_tile: from)
            {
                json_tile.get_to(libutil::at(to, i, 0));
                ++i;
            }
        };

        to.hi_score              = from.at("hiScore").get<int>();
        from_tile_array1d(from.at("nextInputTiles"), to.next_input_tiles);
        from_tile_array1d(from.at("inputTiles"), to.input_tiles);
        to.input_layout.x_offset = from.at("inputXOffset").get<int>();
        to.input_layout.rotation = from.at("inputRotation").get<int>();
        to.board_tiles.data      = from.at("boardTiles");
    }

    void from_json_1(const nlohmann::json& from, game_state& to)
    {
        /*
        Example:

        {
            "boardTiles":
            [
                [1,    0,    null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null],
                [null, null, null, null, null, null, null, null, null, null]
            ],
            "hiScore":3,
            "inputRotation":0,
            "inputTiles":[[0,1],[null,null]],
            "inputXOffset":2,
            "nextInputTiles":[[2,0],[null,null]]
        }
        */

        to.hi_score              = from.at("hiScore").get<int>();
        to.next_input_tiles.data = from.at("nextInputTiles");
        to.input_tiles.data      = from.at("inputTiles");
        to.input_layout.x_offset = from.at("inputXOffset").get<int>();
        to.input_layout.rotation = from.at("inputRotation").get<int>();
        to.board_tiles.data      = from.at("boardTiles");
    }
}

void to_json(nlohmann::json& to, const game_state& from)
{
    to["version"]        = 1;
    to["hiScore"]        = from.hi_score;
    to["nextInputTiles"] = from.next_input_tiles.data;
    to["inputTiles"]     = from.input_tiles.data;
    to["inputXOffset"]   = from.input_layout.x_offset;
    to["inputRotation"]  = from.input_layout.rotation;
    to["boardTiles"]     = from.board_tiles.data;
}

void from_json(const nlohmann::json& from, game_state& to)
{
    const auto version = [&]
    {
        try
        {
            return from.at("version").get<int>();
        }
        catch(...)
        {
            return 0;
        }
    }();

    switch(version)
    {
        case 0: from_json_0(from, to); break;
        case 1: from_json_1(from, to); break;
    }
}

} //namespace

#endif
