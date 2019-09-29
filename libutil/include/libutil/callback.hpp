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

#ifndef LIBUTIL_CALLBACK_HPP
#define LIBUTIL_CALLBACK_HPP

#include "constant.hpp"

namespace libutil
{

/*
callback is a type-safe wrapper of C-style callback.
It uses void*-based type erasure, which makes it a much faster alternative to
std::function, though less flexible.
*/

template<class Signature>
class callback;

template<class R, class... Args>
class callback<R(Args...)>
{
    private:
        using fn_ptr = void(*)(void*, Args...);

    public:
        callback() = default;

        //Make from function pointer.
        template<auto fn, class Context>
        callback(constant<fn>, Context& context):
            pfn_(make_fn_ptr<fn, Context>()),
            pcontext_(&context)
        {
        }

        callback(const callback&) = default;

        callback(callback&&) = default;

        callback& operator=(const callback&) = default;

        callback& operator=(callback&&) = default;

        template<auto fn, class Context>
        void assign(Context& context)
        {
            pfn_ = make_fn_ptr<fn, Context>();
            pcontext_ = &context;
        }

        operator bool() const
        {
            return pfn_ != nullptr;
        }

        R operator()(Args... args)
        {
            return pfn_(pcontext_, args...);
        }

    private:
        template<auto fn, class Context>
        static fn_ptr make_fn_ptr()
        {
            return [](void* vpcontext, Args... args)
            {
                auto pcontext = reinterpret_cast<Context*>(vpcontext);
                return std::invoke(fn, *pcontext, args...);
            };
        }

    private:
        fn_ptr pfn_ = nullptr;
        void* pcontext_ = nullptr;
};

} //namespace

#endif
