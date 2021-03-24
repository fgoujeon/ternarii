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

#include "filesystem.hpp"
#include <libutil/log.hpp>
#include <emscripten.h>

namespace libdb::filesystem
{

namespace
{
    enum class state
    {
        idle,
        loading,
        ready
    };

    state current_state = state::idle;

    std::function<void()> loading_completion_callback;
}

void async_load(const std::function<void()>& cb)
{
    if(current_state != state::idle)
    {
        return;
    }

    loading_completion_callback = cb;

    libutil::log::info("Loading persistent filesystem...");
    current_state = state::loading;

    //Mount and load filesystem, and call libdb_filesystem_handle_loading() when
    //it's done.
    EM_ASM(
        FS.mount(IDBFS, {}, '/home');
        FS.syncfs(true, function(err) {
            assert(!err);
            ccall('libdb_filesystem_handle_loading', 'v');
        });
    );
}

} //namespace

//Functions called from JavaScript
extern "C"
{
    void EMSCRIPTEN_KEEPALIVE libdb_filesystem_handle_loading()
    {
        using namespace libdb::filesystem;
        libutil::log::info("Loaded persistent filesystem.");
        current_state = state::ready;
        loading_completion_callback();
    }
}
