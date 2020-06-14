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

#ifndef LIBUTIL_FSM_HPP
#define LIBUTIL_FSM_HPP

#include <any>
#include <memory>
#include <utility>

namespace libutil::fsm
{

struct state
{
    virtual ~state() = default;
    virtual void handle_event(const std::any& /*event*/){}
};

template<class Context>
class fsm
{
    public:
        fsm(Context& context):
            context_(context)
        {
        }

        void handle_event(const std::any& event)
        {
            if(pstate_)
            {
                pstate_->handle_event(event);
            }
        }

        template<class State, class... Args>
        void set_state(Args&&... args)
        {
            pstate_.reset();
            pstate_ = std::make_unique<State>(*this, std::forward<Args>(args)...);
        }

        Context& get_context()
        {
            return context_;
        }

    private:
        std::unique_ptr<state> pstate_;
        Context& context_;
};

} //namespace

#endif
