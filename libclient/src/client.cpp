/*
Copyright 2018 - 2021 Florian Goujeon

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

#include <libclient/client.hpp>
#include <nlohmann/json.hpp>
#include <emscripten.h>

namespace libclient
{

void client::async_get_player_id
(
    const std::string& player_name,
    const async_get_player_id_success_callback& success_callback,
    const async_get_player_id_failure_callback& failure_callback
)
{
    struct arg
    {
        async_get_player_id_success_callback success_callback;
        async_get_player_id_failure_callback failure_callback;
    };
    auto parg = new arg{success_callback, failure_callback};

    const auto query =
        "https://ternarii.com/test/online/api/player/get_player_id?"
        "player_name=" + player_name
    ;

    emscripten_async_wget_data
    (
        query.c_str(),
        parg,
        [](void* vparg, void* vpdata, int data_size)
        {
            /*
            Example of success message:
                {
                    "success":
                    {
                        "id":1
                    }
                }

            Example of failure message:
                {
                    "failure":
                    {
                        "message":"No player found"
                    }
                }
            */

            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};

            try
            {
                const auto pdata = reinterpret_cast<const char*>(vpdata);
                const auto json = nlohmann::json::parse
                (
                    pdata,
                    pdata + data_size
                );

                if(json.contains("failure"))
                {
                    parg->failure_callback(json.at("failure").at("message").get<std::string>());
                    return;
                }

                const auto player_id = json.at("success").at("id").get<unsigned int>();

                parg->success_callback(player_id);
            }
            catch(const std::exception& e)
            {
                parg->failure_callback(e.what());
            }
        },
        [](void* vparg)
        {
            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};
            parg->failure_callback("Could not connect to the server.");
        }
    );
}

void client::async_get_or_add_game
(
    const int player_id,
    const std::string& player_password,
    const int stage_id,
    const client::async_get_or_add_game_success_callback& success_callback,
    const client::async_get_or_add_game_failure_callback& failure_callback
)
{
    struct arg
    {
        async_get_or_add_game_success_callback success_callback;
        async_get_or_add_game_failure_callback failure_callback;
    };
    auto parg = new arg{success_callback, failure_callback};

    const auto query =
        "https://ternarii.com/test/online/api/player/get_or_add_game?"
        "player_id=" + std::to_string(player_id) + "&"
        "player_password=" + player_password + "&"
        "stage_id=" + std::to_string(stage_id)
    ;

    emscripten_async_wget_data
    (
        query.c_str(),
        parg,
        [](void* vparg, void* vpdata, int data_size)
        {
            /*
            Example of success message:
                {
                    "success":
                    {
                        "game_id":4,
                        "first_input_random_number":431353885,
                        "first_next_input_random_number":857073887,
                        "moves":
                        [
                            {"column_offset":0,"rotation":0,"next_input_random_number":9182349071092374},
                            {"column_offset":2,"rotation":3,"next_input_random_number":15539980385327},
                            {"column_offset":2,"rotation":3,"next_input_random_number":3232982676826174}
                        ]
                    }
                }

            Example of failure message:
                {
                    "failure":
                    {
                        "message":"No player found"
                    }
                }
            */

            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};

            try
            {
                const auto pdata = reinterpret_cast<const char*>(vpdata);
                const auto json = nlohmann::json::parse
                (
                    pdata,
                    pdata + data_size
                );

                if(json.contains("failure"))
                {
                    parg->failure_callback(json.at("failure").at("message").get<std::string>());
                    return;
                }

                auto game = data_types::game{};
                json.at("success").at("game_id").get_to(game.id);
                json.at("success").at("first_input_random_number").get_to(game.first_input_random_number);
                json.at("success").at("first_next_input_random_number").get_to(game.first_next_input_random_number);

                for(const auto& [key, move_json]: json.at("success").at("moves").items())
                {
                    auto move = data_types::move{};
                    move_json.at("column_offset").get_to(move.column_offset);
                    move_json.at("rotation").get_to(move.rotation);
                    move_json.at("next_input_random_number").get_to(move.next_input_random_number);
                    game.moves.push_back(move);
                }

                parg->success_callback(game);
            }
            catch(const std::exception& e)
            {
                parg->failure_callback(e.what());
            }
        },
        [](void* vparg)
        {
            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};
            parg->failure_callback("Could not connect to the server.");
        }
    );
}

void client::async_add_move
(
    const int player_id,
    const std::string& player_password,
    const int game_id,
    const int move_index,
    const int column_offset,
    const int rotation,
    const async_add_move_success_callback& success_callback,
    const async_add_move_failure_callback& failure_callback
)
{
    struct arg
    {
        async_add_move_success_callback success_callback;
        async_add_move_failure_callback failure_callback;
    };
    auto parg = new arg{success_callback, failure_callback};

    const auto query =
        "https://ternarii.com/test/online/api/player/add_move?"
        "player_id=" + std::to_string(player_id) + "&"
        "player_password=" + player_password + "&"
        "game_id=" + std::to_string(game_id) + "&"
        "move_idx=" + std::to_string(move_index) + "&"
        "column_offset=" + std::to_string(column_offset) + "&"
        "rotation=" + std::to_string(rotation)
    ;

    emscripten_async_wget_data
    (
        query.c_str(),
        parg,
        [](void* vparg, void* vpdata, int data_size)
        {
            /*
            Example of success message:
                {
                    "success":
                    {
                        "next_input_random_number":857073887
                    }
                }

            Example of failure message:
                {
                    "failure":
                    {
                        "message":"No player found"
                    }
                }
            */

            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};

            try
            {
                const auto pdata = reinterpret_cast<const char*>(vpdata);
                const auto json = nlohmann::json::parse
                (
                    pdata,
                    pdata + data_size
                );

                if(json.contains("failure"))
                {
                    parg->failure_callback(json.at("failure").at("message").get<std::string>());
                    return;
                }

                const auto next_input_random_number =
                    json.at("success").at("next_input_random_number").get<unsigned long long>();

                parg->success_callback(next_input_random_number);
            }
            catch(const std::exception& e)
            {
                parg->failure_callback(e.what());
            }
        },
        [](void* vparg)
        {
            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};
            parg->failure_callback("Could not connect to the server.");
        }
    );
}

void client::async_finish_game
(
    const int player_id,
    const std::string& player_password,
    const int game_id,
    const async_finish_game_success_callback& success_callback,
    const async_finish_game_failure_callback& failure_callback
)
{
    struct arg
    {
        async_finish_game_success_callback success_callback;
        async_finish_game_failure_callback failure_callback;
    };
    auto parg = new arg{success_callback, failure_callback};

    const auto query =
        "https://ternarii.com/test/online/api/player/finish_game?"
        "player_id=" + std::to_string(player_id) + "&"
        "player_password=" + player_password + "&"
        "game_id=" + std::to_string(game_id)
    ;

    emscripten_async_wget_data
    (
        query.c_str(),
        parg,
        [](void* vparg, void* vpdata, int data_size)
        {
            /*
            Example of success message:
                {
                    "success": {}
                }

            Example of failure message:
                {
                    "failure":
                    {
                        "message":"No player found"
                    }
                }
            */

            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};

            try
            {
                const auto pdata = reinterpret_cast<const char*>(vpdata);
                const auto json = nlohmann::json::parse
                (
                    pdata,
                    pdata + data_size
                );

                if(json.contains("failure"))
                {
                    parg->failure_callback(json.at("failure").at("message").get<std::string>());
                    return;
                }

                json.at("success");

                parg->success_callback();
            }
            catch(const std::exception& e)
            {
                parg->failure_callback(e.what());
            }
        },
        [](void* vparg)
        {
            auto parg = std::unique_ptr<arg>{reinterpret_cast<arg*>(vparg)};
            parg->failure_callback("Could not connect to the server.");
        }
    );
}

} //namespace
