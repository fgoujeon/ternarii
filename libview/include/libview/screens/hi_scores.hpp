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

#ifndef LIBVIEW_SCREENS_HI_SCORES_HPP
#define LIBVIEW_SCREENS_HI_SCORES_HPP

#include "../data_types.hpp"
#include "../common.hpp"
#include <chrono>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <libutil/void_function.hpp>
#include <map>

namespace libview::screens
{

class hi_scores: public object2d
{
    public:
        struct callback_set
        {
            libutil::void_function<> back_request;
        };

        using score_map = std::map
        <
            data_types::stage,
            int /*score*/
        >;

        hi_scores
        (
            object2d& parent,
            feature_group_set& feature_groups,
            const score_map& scores,
            const callback_set& callbacks
        );

        ~hi_scores();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
