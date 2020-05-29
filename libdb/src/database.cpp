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

#include "json_conversion.hpp"
#include <libdb/database.hpp>
#include <nlohmann/json.hpp>
#include <libutil/log.hpp>
#include <emscripten.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace libdb
{

namespace
{
    constexpr int current_version = 2;

    std::filesystem::path get_config_dir()
    {
        static const auto dir = std::filesystem::path{std::getenv("HOME")} / ".config" / "ternarii";
        return dir;
    }

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
    private:
        enum class state
        {
            starting,
            loading_filesystem,
            loading_data,
            ready
        };

    public:
        impl(const event_handler& evt_handler):
            event_handler_(evt_handler)
        {
        }

        void iterate()
        {
            switch(current_state_)
            {
                case state::starting:
                    async_load_filesystem();
                    break;
                case state::loading_filesystem:
                    async_wait_for_filesystem();
                    break;
                case state::loading_data:
                    load_data();
                    break;
                case state::ready:
                    break;
            }
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

            if(current_state_ == state::ready)
            {
                save_data();
                async_save_filesystem();
            }
        }

    private:
        void async_load_filesystem()
        {
            EM_ASM(
                FS.mount(IDBFS, {}, '/home');

                Module.savingPersistentFilesystem = 0;
                Module.persistentFilesystemLoaded = 0;
                console.log("Loading persistent filesystem...");
                FS.syncfs(true, function(err) {
                    assert(!err);
                    console.log("Loaded persistent filesystem.");
                    Module.persistentFilesystemLoaded = 1;
                });
            );

            current_state_ = state::loading_filesystem;
        }

        void async_wait_for_filesystem()
        {
            if(emscripten_run_script_int("Module.persistentFilesystemLoaded") == 1)
            {
                current_state_ = state::loading_data;
            }
        }

        bool try_load_data(const int version)
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

        void load_data()
        {
            try
            {
                auto loaded = false;
                for(auto i = current_version; i >= 0 && !loaded; --i)
                {
                    loaded = try_load_data(i);
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

            current_state_ = state::ready;
            event_handler_(events::end_of_loading{});
        }

        void save_data()
        {
            try
            {
                const auto path = get_database_path();
                std::filesystem::create_directories(path.parent_path());
                auto ofs = std::ofstream{path};
                const auto json = nlohmann::json(*opt_game_state_);
                ofs << json;
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

        void async_save_filesystem()
        {
            EM_ASM(
                if(Module.savingPersistentFilesystem == 0) {
                    Module.savingPersistentFilesystem = 1;
                    FS.syncfs(false, function(err) {
                        assert(!err);
                        Module.savingPersistentFilesystem = 0;
                    });
                }
            );
        }

    private:
        event_handler event_handler_;
        state current_state_ = state::starting;
        std::optional<data_types::game_state> opt_game_state_;
};

database::database(const event_handler& evt_handler):
    pimpl_(std::make_unique<impl>(evt_handler))
{
}

database::~database() = default;

void database::iterate()
{
    pimpl_->iterate();
}

const std::optional<data_types::game_state>& database::get_game_state() const
{
    return pimpl_->get_game_state();
}

void database::set_stage_state(const data_types::stage stage, const data_types::stage_state& state)
{
    pimpl_->set_stage_state(stage, state);
}

} //namespace
