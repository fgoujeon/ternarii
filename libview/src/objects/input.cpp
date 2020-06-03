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

#include "input.hpp"
#include <libutil/log.hpp>

namespace libview::objects
{

namespace
{
    const auto ymid = 0.0f;

    enum class direction
    {
        neutral,
        left,
        right
    };

    direction get_direction(const input::keyboard_state& state)
    {
        if(state.left_pressed && !state.right_pressed)
        {
            return direction::left;
        }

        if(!state.left_pressed && state.right_pressed)
        {
            return direction::right;
        }

        return direction::neutral;
    }

    //Get nearest valid X position for the center of gravity.
    float get_nearest_valid_x_cog(const float current_x, const float target_x)
    {
        //We need this inclination, otherwise the tiles might not move if the
        //user presses and releases a move button a little too quickly.
        const auto inclination = target_x > 0 ? 0.4f : -0.4f;

        const auto x = current_x + inclination;
        return std::round(x);
    }

    float move_toward(const float current, const float target, const float step)
    {
        const auto tolerance = 0.1f;

        if(std::abs(current - target) < tolerance)
        {
            return target;
        }

        if(current < target)
        {
            return current + step;
        }
        else
        {
            return current - step;
        }
    }
}

input::input
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    features::key_event_handler_group& key_event_handlers
):
    Object2D(&parent),
    features::animable{*this, &animables},
    features::key_event_handler{*this, &key_event_handlers},
    drawables_(drawables)
{
    at(tiles_, 0, 0) = std::make_shared<number_tile>(*this, drawables_, 5);
    at(tiles_, 0, 0)->scale({0.46f, 0.46f});

    at(tiles_, 1, 0) = std::make_shared<number_tile>(*this, drawables_, 6);
    at(tiles_, 1, 0)->scale({0.46f, 0.46f});
    at(tiles_, 1, 0)->translate({1.0f, 0.0f});

    update_target_positions();
}

void input::advance(const libutil::time_point& /*now*/, float elapsed_s)
{
    const auto speed = 7.0f; //in units per second
    const auto step = speed * elapsed_s;

    current_x_cog_ = move_toward(current_x_cog_, target_x_cog_, step);

    libutil::for_each
    (
        [&](auto& ptile, auto& target_pos)
        {
            if(ptile)
            {
                const auto pos = ptile->translation();
                const auto x = move_toward(pos.x(), target_pos.x(), step);
                const auto y = move_toward(pos.y(), target_pos.y(), step);
                ptile->setTranslation({x, y});
            }
        },
        tiles_,
        target_positions_
    );
}

void input::handle_key_press(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            keyboard_state_.left_pressed = true;
            break;
        case key_event::Key::Right:
            keyboard_state_.right_pressed = true;
            break;
        default:
            break;
    }

    update_target_positions();
}

void input::handle_key_release(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            keyboard_state_.left_pressed = false;
            break;
        case key_event::Key::Right:
            keyboard_state_.right_pressed = false;
            break;
        default:
            break;
    }

    update_target_positions();
}

void input::update_target_positions()
{
    //First of all, compute the target position of the center of gravity (COG).
    target_x_cog_ = [&]() -> float
    {
        switch(get_direction(keyboard_state_))
        {
            case direction::left:
                return -2;
            case direction::right:
                return 2;
            case direction::neutral:
            default:
                return get_nearest_valid_x_cog
                (
                    current_x_cog_,
                    target_x_cog_
                );
        }
    }();

    //Then, define the target positions of the other tiles relatively to the
    //COG.
    at(target_positions_, 0, 0) = {target_x_cog_ - 0.5f, ymid};
    at(target_positions_, 1, 0) = {target_x_cog_ + 0.5f, ymid};
}

} //namespace
