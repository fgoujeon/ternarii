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

#ifndef LIBUTIL_UNIQUE_FUNCTION_HPP
#define LIBUTIL_UNIQUE_FUNCTION_HPP

#include <memory>

namespace libutil
{

/*
A variation of std::function that manages move-only function objects.
*/

namespace detail
{
    template<class R, class... Args>
    struct abstract_unique_function
    {
        virtual ~abstract_unique_function() = default;
        virtual R operator()(Args...) = 0;
    };

    template<class F, class R, class... Args>
    class concrete_unique_function: public abstract_unique_function<R, Args...>
    {
        public:
            concrete_unique_function(F&& f):
                f_(std::move(f))
            {
            }

            R operator()(Args... args) override
            {
                return f_(std::forward<Args>(args)...);
            }

        private:
            F f_;
    };
}

template<class Signature>
class unique_function;

template<class R, class... Args>
class unique_function<R(Args...)>
{
    private:
        using fn_t = detail::abstract_unique_function<R, Args...>;

    public:
        unique_function() = default;

        unique_function(const unique_function&) = delete;

        unique_function(unique_function&&) = default;

        template<class F>
        unique_function(F f):
            pf_
            (
                std::make_unique<detail::concrete_unique_function<F, R, Args...>>
                (
                    std::move(f)
                )
            )
        {
        }

        R operator()(Args... args)
        {
            return (*pf_)(std::forward<Args>(args)...);
        }

    private:
        std::unique_ptr<fn_t> pf_;
};

} //namespace

#endif
