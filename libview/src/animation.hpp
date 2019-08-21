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

#ifndef LIBVIEW_ANIMATION_HPP
#define LIBVIEW_ANIMATION_HPP

#include <Magnum/Animation/Player.h>
#include <chrono>
#include <list>

namespace libview
{

struct animation
{
    template<class Callback, class UserData>
    void add_fixed_speed_translation
    (
        const Magnum::Vector2& start_position,
        const Magnum::Vector2& finish_position,
        const float speed, //in distance unit per second
        Callback&& callback,
        UserData& user_data
    )
    {
        const auto x1 = start_position.x();
        const auto y1 = start_position.y();
        const auto x2 = finish_position.x();
        const auto y2 = finish_position.y();
        const auto distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        const auto time = distance / speed;

        auto track = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>
        {
            {
                {0.0f, start_position},
                {time, finish_position}
            },
            Magnum::Math::lerp,
            Magnum::Animation::Extrapolation::Constant
        };

        tracks.push_back(std::move(track));

        player.addWithCallback
        (
            tracks.back(),
            std::forward<Callback>(callback),
            user_data
        );
    }

    Magnum::Animation::Player<std::chrono::nanoseconds, Magnum::Float> player;
    std::list<Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>> tracks; //for storage only
};

using animation_list = std::list<animation>;

} //namespace

#endif
