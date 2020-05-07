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

namespace
{
    template<class... Ts>
    struct for_each_type_in_pack;

    template<>
    struct for_each_type_in_pack<>
    {
        template<class F>
        static void call(F&&)
        {
        }
    };

    template<class T, class... Ts>
    struct for_each_type_in_pack<T, Ts...>
    {
        template<class F>
        static void call(F&& f)
        {
            using tag_t = T*;
            const auto tag = tag_t{nullptr};
            f(tag);

            for_each_type_in_pack<Ts...>::call(f);
        }
    };
}

//Add support of std::optional
namespace nlohmann
{
    template<typename... Ts>
    struct adl_serializer<std::variant<Ts...>>
    {
        static void to_json(json& to, const std::variant<Ts...>& from)
        {
            to["type"] = from.index();
            std::visit
            (
                [&](const auto& v)
                {
                    to["value"] = v;
                },
                from
            );
        }

        static void from_json(const json& from, std::variant<Ts...>& to)
        {
            const auto type_index = from.at("type").get<int>();

            auto i = 0;
            auto found = false;
            for_each_type_in_pack<Ts...>::call
            (
                [&](const auto* tag)
                {
                    using value_t = std::decay_t<decltype(*tag)>;

                    if(i == type_index)
                    {
                        found = true;
                        to = from.at("value").get<value_t>();
                    }

                    ++i;
                }
            );

            if(!found)
            {
                throw std::runtime_error{"Invalid type index: " + std::to_string(type_index)};
            }
        }
    };

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

namespace libcommon::data_types::tiles
{

void to_json(nlohmann::json& to, const number& from)
{
    to = from.value;
}

void from_json(const nlohmann::json& from, number& to)
{
    to.value = from.get<int>();
}

void to_json(nlohmann::json&, const column_nullifier&)
{
}

void from_json(const nlohmann::json&, column_nullifier&)
{
}

void to_json(nlohmann::json&, const row_nullifier&)
{
}

void from_json(const nlohmann::json&, row_nullifier&)
{
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
            "inputTiles":[0,1],
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
        to.board_tiles.data      = from.at("boardTiles");
    }

    void from_json_1(const nlohmann::json& from, game_state& to)
    {
        /*
        Example:

        {
            "boardTiles":
            [
                1,    0,    null, null, null, null, null, null, null, null,
                null, null, null, null, null, null, null, null, null, null,
                null, null, null, null, null, null, null, null, null, null,
                null, null, null, null, null, null, null, null, null, null,
                null, null, null, null, null, null, null, null, null, null,
                null, null, null, null, null, null, null, null, null, null
            ],
            "hiScore":3,
            "inputTiles":[0,null,1,null],
            "nextInputTiles":[2,null,0,null]
        }
        */

        to.hi_score              = from.at("hiScore").get<int>();
        to.next_input_tiles.data = from.at("nextInputTiles");
        to.input_tiles.data      = from.at("inputTiles");
        to.board_tiles.data      = from.at("boardTiles");
    }
}

void to_json(nlohmann::json& to, const game_state& from)
{
    to["version"]        = 1;
    to["hiScore"]        = from.hi_score;
    to["nextInputTiles"] = from.next_input_tiles.data;
    to["inputTiles"]     = from.input_tiles.data;
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
