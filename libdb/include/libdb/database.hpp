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

#ifndef LIBDB_DATABASE_HPP
#define LIBDB_DATABASE_HPP

#include "events.hpp"
#include "data_types.hpp"
#include <memory>

namespace libdb
{

class database
{
    public:
        database(bool fail_on_access_error, const event_handler& evt_handler);

        ~database();

        const std::optional<data_types::game_state>& get_game_state() const;

        void set_stage_state(data_types::stage stage, const data_types::stage_state& state);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
