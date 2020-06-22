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

#ifndef LIBDB_INDEXED_DB_HPP
#define LIBDB_INDEXED_DB_HPP

namespace libdb::indexed_db
{

using read_success_callback_t  = void(*)(void* arg, void* data, int size);
using write_success_callback_t = void(*)(void* arg);
using failure_callback_t       = void(*)(void* arg, const char* error);

/*
Reads given database entry.
Note: If given entry doesn't exist, calls success_callback(arg, nullptr, 0).
*/
void async_read
(
    const char* database_name,
    const char* store_name,
    int key,
    void* arg,
    const read_success_callback_t success_callback,
    const failure_callback_t failure_callback
);

/*
Writes to given database entry.
*/
void async_write
(
    const char* database_name,
    const char* store_name,
    int key,
    const void* data,
    int size,
    void* arg,
    const write_success_callback_t success_callback,
    const failure_callback_t failure_callback
);

} //namespace

#endif
