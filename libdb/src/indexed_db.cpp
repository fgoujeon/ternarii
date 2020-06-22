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

#include "indexed_db.hpp"
#include <emscripten.h>

namespace libdb::indexed_db
{

namespace
{
    EM_JS(
        void,
        async_read_2,
        (
            const char* database_name,
            const char* store_name,
            const int key,
            void* arg,
            const read_success_callback_t success_callback,
            const failure_callback_t failure_callback
        ),
        {
            var openRequest = indexedDB.open(UTF8ToString(database_name), 1);

            //Init database if needed
            openRequest.onupgradeneeded = function() {
                let db = openRequest.result;
                if (!db.objectStoreNames.contains(UTF8ToString(store_name))) {
                    db.createObjectStore(UTF8ToString(store_name));
                }
            };

            openRequest.onerror = function() {
                ccall
                (
                    "libdb_indexed_db_call_failure_callback",
                    'v',
                    ["number", "number", "string"],
                    [success_callback, arg, openRequest.error]
                );
            };

            openRequest.onsuccess = function() {
                console.log("Open success");

                let db = openRequest.result;
                let transaction = db.transaction(UTF8ToString(store_name));
                let store = transaction.objectStore(UTF8ToString(store_name));
                let request = store.get(key);

                request.onsuccess = function() {
                    ccall
                    (
                        "libdb_indexed_db_call_read_success_callback",
                        'v',
                        ["number", "number", "array", "number"],
                        [success_callback, arg, request.result, request.result.length]
                    );
                };

                request.onerror = function() {
                    ccall
                    (
                        "libdb_indexed_db_call_failure_callback",
                        'v',
                        ["number", "number", "string"],
                        [success_callback, arg, request.error]
                    );
                };
            };
        }
    );

    EM_JS(
        void,
        async_write_2,
        (
            const char* database_name,
            const char* store_name,
            const int key,
            const void* data,
            const int size,
            void* arg,
            const write_success_callback_t success_callback,
            const failure_callback_t failure_callback
        ),
        {
            var openRequest = indexedDB.open(UTF8ToString(database_name), 1);

            //Init database if needed
            openRequest.onupgradeneeded = function() {
                let db = openRequest.result;
                if (!db.objectStoreNames.contains(UTF8ToString(store_name))) {
                    db.createObjectStore(UTF8ToString(store_name));
                }
            };

            openRequest.onerror = function() {
                ccall
                (
                    "libdb_indexed_db_call_failure_callback",
                    'v',
                    ["number", "number", "string"],
                    [success_callback, arg, openRequest.error]
                );
            };

            openRequest.onsuccess = function() {
                let db = openRequest.result;
                let transaction = db.transaction(UTF8ToString(store_name), "readwrite");
                let store = transaction.objectStore(UTF8ToString(store_name));
                let request = store.put(HEAPU8.slice(data, data + size), key);

                request.onsuccess = function() {
                    ccall
                    (
                        "libdb_indexed_db_call_write_success_callback",
                        'v',
                        ["number", "number"],
                        [success_callback, arg]
                    );
                };

                request.onerror = function() {
                    ccall
                    (
                        "libdb_indexed_db_call_failure_callback",
                        'v',
                        ["number", "number", "string"],
                        [success_callback, arg, request.error]
                    );
                };
            };
        }
    );
}

void async_read
(
    const char* database_name,
    const char* store_name,
    const int key,
    void* arg,
    const read_success_callback_t success_callback,
    const failure_callback_t failure_callback
)
{
    async_read_2
    (
        database_name,
        store_name,
        key,
        arg,
        success_callback,
        failure_callback
    );
}

void async_write
(
    const char* database_name,
    const char* store_name,
    int key,
    const void* data,
    const int size,
    void* arg,
    const write_success_callback_t success_callback,
    const failure_callback_t failure_callback
)
{
    async_write_2
    (
        database_name,
        store_name,
        key,
        data,
        size,
        arg,
        success_callback,
        failure_callback
    );
}

} //namespace

//Functions called from JavaScript
extern "C"
{
    void EMSCRIPTEN_KEEPALIVE libdb_indexed_db_call_read_success_callback
    (
        const libdb::indexed_db::read_success_callback_t cb,
        void* arg,
        void* data,
        int size
    )
    {
        cb(arg, data, size);
    }

    void EMSCRIPTEN_KEEPALIVE libdb_indexed_db_call_write_success_callback
    (
        const libdb::indexed_db::write_success_callback_t cb,
        void* arg
    )
    {
        cb(arg);
    }

    void EMSCRIPTEN_KEEPALIVE libdb_indexed_db_call_failure_callback
    (
        const libdb::indexed_db::failure_callback_t cb,
        void* arg,
        const char* error
    )
    {
        cb(arg, error);
    }
}
