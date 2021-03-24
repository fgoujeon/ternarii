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

#ifndef LIBUTIL_VOID_FUNCTION_HPP
#define LIBUTIL_VOID_FUNCTION_HPP

#include <chrono>

namespace libutil
{

//Like std::function<void(Args...)>, but whose operator() doesn't throw if
//default-constructed.
template<class... Args>
class void_function
{
    private:
        using impl = std::function<void(Args...)>;

    public:
        void_function()
        {
        }

        template<class F>
        void_function(F&& f):
            impl_(f)
        {
        }

        void_function(const void_function& other):
            impl_(other.impl_)
        {
        }

        void_function(void_function& other):
            impl_(other.impl_)
        {
        }

        void_function(void_function&& other):
            impl_(std::move(other.impl_))
        {
        }

        void operator()(Args... args) const
        {
            if(impl_)
            {
                impl_(std::forward<Args>(args)...);
            }
        }

    private:
        impl impl_;
};

} //namespace

#endif
