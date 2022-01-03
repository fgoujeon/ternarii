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

#include "json_conversion.hpp"
#include "filesystem.hpp"
#include "indexed_db.hpp"
#include <libdb/database.hpp>
#include <nlohmann/json.hpp>
#include <libutil/log.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace libdb
{

namespace
{
    constexpr int current_version = 2;

    //Deprecated files
    std::filesystem::path get_config_dir()
    {
        static const auto dir = std::filesystem::path{std::getenv("HOME")} / ".config" / "ternarii";
        return dir;
    }

    //Deprecated files
    std::filesystem::path get_database_path(const int version = current_version)
    {
        switch(version)
        {
            case 0: return get_config_dir() / "database.json";
            case 1: return get_config_dir() / "database_v1.json";
            case 2: return get_config_dir() / "database_v2.json";
            default: throw std::runtime_error{"Unsupported database version."};
        }
    }
}

struct database::impl
{
    public:
        impl(const event_handler& evt_handler):
            event_handler_(evt_handler)
        {
            indexed_db::async_read
            (
                "database",
                "game_state",
                2,
                [this](void* data, int size)
                {
                    if(data)
                    {
                        auto json_str = std::string_view
                        {
                            reinterpret_cast<char*>(data),
                            static_cast<std::string_view::size_type>(size)
                        };
                        libutil::log::info("Loaded data from IndexedDB.", json_str);
                        load_from_idb(json_str, 2);
                    }
                    else
                    {
                        libutil::log::info("No IndexedDB data found. Trying with IDBFS...");
                        filesystem::async_load([this]{load_from_idbfs();});
                    }
                },
                [](const char* error)
                {
                    libutil::log::error("IndexedDB read error: ", error);
                }
            );
        }

        const std::optional<data_types::game_state>& get_game_state() const
        {
            return opt_game_state_;
        }

        void set_stage_state(const data_types::stage stage, const data_types::stage_state& state)
        {
            if(!opt_game_state_)
            {
                opt_game_state_ = data_types::game_state{};
            }

            opt_game_state_->stage_states[stage] = state;

            save_data();
        }

    private:
        //Deprecated format
        bool try_load_from_idbfs(const int version)
        {
            const auto path = get_database_path(version);
            if(!exists(path))
            {
                return false;
            }

            //read json from file
            auto ifs = std::ifstream{path};
            auto json = nlohmann::json{};
            ifs >> json;

            libutil::log::info("Loaded JSON:");
            libutil::log::info(json);

            //convert json to state
            auto game_state = data_types::game_state{};
            from_json(json, game_state, version);
            opt_game_state_ = game_state;

            return true;
        }

        //Deprecated format
        void load_from_idbfs()
        {
            try
            {
                auto loaded = false;
                for(auto i = current_version; i >= 0 && !loaded; --i)
                {
                    loaded = try_load_from_idbfs(i);
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            catch(...)
            {
                std::cerr << "Unknown error\n";
            }

            event_handler_(events::end_of_loading{});
        }

        void load_from_idb(const std::string_view& json_str, const int json_version)
        {
            //Parse JSON string
            const auto json = nlohmann::json::parse(json_str);

            //Convert JSON to state
            auto game_state = data_types::game_state{};
            from_json(json, game_state, json_version);
            opt_game_state_ = game_state;

            event_handler_(events::end_of_loading{});
        }

        void save_data()
        {
            try
            {
                //Convert to JSON
                const auto json = nlohmann::json(*opt_game_state_);

                //Get JSON string
                auto oss = std::ostringstream{};
                oss << json;

                auto pstr = std::make_shared<std::string>(oss.str());

                indexed_db::async_write
                (
                    "database",
                    "game_state",
                    2,
                    pstr->c_str(),
                    pstr->size(),
                    [pstr]
                    {
                        libutil::log::info("Write success.");
                    },
                    [](const char* error)
                    {
                        libutil::log::error("Write error: ", error);
                    }
                );
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            catch(...)
            {
                std::cerr << "Unknown error\n";
            }
        }

    private:
        event_handler event_handler_;
        std::optional<data_types::game_state> opt_game_state_;
};

database::database(const event_handler& evt_handler):
    pimpl_(std::make_unique<impl>(evt_handler))
{
}

database::~database() = default;

const std::optional<data_types::game_state>& database::get_game_state() const
{
    return pimpl_->get_game_state();
}

void database::set_stage_state(const data_types::stage stage, const data_types::stage_state& state)
{
    pimpl_->set_stage_state(stage, state);
}

} //namespace
