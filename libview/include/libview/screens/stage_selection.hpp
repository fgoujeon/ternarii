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

#ifndef LIBVIEW_SCREENS_STAGE_SELECTION_HPP
#define LIBVIEW_SCREENS_STAGE_SELECTION_HPP

#include "../common.hpp"
#include <libview/data_types.hpp>
#include <chrono>
#include <libutil/void_function.hpp>

namespace libview::screens
{

class stage_selection: public object2d
{
    public:
        struct callback_set
        {
            libutil::void_function
            <
                data_types::stage,
                const Magnum::Vector2& /*stage_symbol_position*/
            > stage_selection_request;

            libutil::void_function<> back_request;
        };

    public:
        stage_selection
        (
            object2d& parent,
            feature_group_set& feature_groups,
            const callback_set& callbacks
        );

        ~stage_selection();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
