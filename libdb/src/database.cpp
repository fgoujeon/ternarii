/*
Copyright 2018 - 2019 Florian Goujeon

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

#include <libdb/database.hpp>
#include <nlohmann/json.hpp>
#include <emscripten.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace libdb
{

namespace
{
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

        int get_hi_score() const
        {
            return hi_score_;
        }

        void set_hi_score(int value)
        {
            hi_score_ = value;

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

        void load_data()
        {
            try
            {
                auto ifs = std::ifstream{path_};
                auto json = nlohmann::json{};
                ifs >> json;
                hi_score_ = json["hiScore"].get<int>();
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
                auto json = nlohmann::json{};
                json["hiScore"] = hi_score_;

                std::filesystem::create_directories(path_.parent_path());
                std::ofstream ofs{path_};
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
            if(emscripten_run_script_int("Module.savingPersistentFilesystem") == 0)
            {
                EM_ASM(
                    Module.savingPersistentFilesystem = 1;
                    FS.syncfs(false, function(err) {
                        assert(!err);
                        Module.savingPersistentFilesystem = 0;
                    });
                );
            }
        }

    private:
        const std::filesystem::path path_ = std::filesystem::path{std::getenv("HOME")} / ".config" / "ternarii" / "database.json";
        event_handler event_handler_;
        state current_state_ = state::starting;
        int hi_score_ = 0;
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

int database::get_hi_score() const
{
    return pimpl_->get_hi_score();
}

void database::set_hi_score(int value)
{
    pimpl_->set_hi_score(value);
}

} //namespace
