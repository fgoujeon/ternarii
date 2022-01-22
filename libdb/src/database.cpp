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
}

struct database::impl
{
    public:
        impl
        (
            const bool fail_on_access_error,
            const event_handler& evt_handler
        ):
            fail_on_access_error_(fail_on_access_error),
            event_handler_(evt_handler)
        {
            indexed_db::async_read
            (
                "database",
                "game_state",
                current_version,
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
                        load_from_idb(json_str, current_version);
                    }
                    else
                    {
                        event_handler_(events::end_of_loading{});
                    }
                },
                [fail_on_access_error](const char* error)
                {
                    libutil::log::error("IndexedDB read error: ", error);
                    if(fail_on_access_error)
                        throw std::runtime_error{std::string{"IndexedDB read error: "} + error};
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
                opt_game_state_ = data_types::game_state{};

            opt_game_state_->stage_states[stage] = state;

            save_data();
        }

    private:
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
                    current_version,
                    pstr->c_str(),
                    pstr->size(),
                    [pstr]
                    {
                        libutil::log::info("Write success.");
                    },
                    [this](const char* error)
                    {
                        libutil::log::error("Write error: ", error);
                        if(fail_on_access_error_)
                            throw std::runtime_error{std::string{"Write: "} + error};
                    }
                );
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                if(fail_on_access_error_)
                    throw;
            }
            catch(...)
            {
                std::cerr << "Unknown error\n";
                if(fail_on_access_error_)
                    throw;
            }
        }

    private:
        const bool fail_on_access_error_;
        event_handler event_handler_;
        std::optional<data_types::game_state> opt_game_state_;
};

database::database
(
    const bool fail_on_access_error,
    const event_handler& evt_handler
):
    pimpl_(std::make_unique<impl>(fail_on_access_error, evt_handler))
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
