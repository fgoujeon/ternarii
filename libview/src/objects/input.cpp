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
#include <cmath>

namespace libview::objects
{

namespace
{
    enum class direction
    {
        neutral,
        left,
        right
    };

    direction get_direction(const input::keyboard_state& state)
    {
        if(state.left_shift_button_pressed && !state.right_shift_button_pressed)
        {
            return direction::left;
        }

        if(!state.left_shift_button_pressed && state.right_shift_button_pressed)
        {
            return direction::right;
        }

        return direction::neutral;
    }

    //Get valid X position that is nearest to the given X center of gravity.
    //List of valid X positions depend on whether the input is laid out
    //vertically or horizontally.
    float get_nearest_valid_cog_x
    (
        const float x,
        const bool vertical,
        const input::order last_received_order
    )
    {
        //We need this inclination, otherwise the tiles might not shift if the
        //user presses and releases the shift button a little too quickly.
        const auto inclination = [&]
        {
            switch(last_received_order)
            {
                case input::order::shift_left:
                    return -0.4f;
                case input::order::shift_right:
                    return 0.4f;
                default:
                    return 0.0f;
            }
        }();

        const auto x2 = x + inclination;

        if(vertical)
        {
            return std::clamp
            (
                std::ceil(x2) - 0.5f,
                -2.5f,
                2.5f
            );
        }
        else
        {
            //Influence rounding so that when rotating from a vertical
            //position, we preferably shift to the right.
            const auto epsilon = 0.01f;

            return std::clamp
            (
                std::round(x2 + epsilon),
                -2.0f,
                2.0f
            );
        }
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

    update_cog_target_position();
}

void input::advance(const libutil::time_point& /*now*/, float elapsed_s)
{
    const auto speed = 7.0f; //in units per second
    const auto step = speed * elapsed_s;

    //First, define current position of COG.
    cog_current_x_ = move_toward(cog_current_x_, cog_target_x_, step);
    cog_current_rotation_rad_ = -cog_target_rotation_ * M_PI / 2.0f;

    //Then, move the tiles relatively to the COG current position and rotation.
    {
        const auto x_shift = std::cosf(cog_current_rotation_rad_) / 2.0f;
        const auto y_shift = std::sinf(cog_current_rotation_rad_) / 2.0f;

        at(tiles_, 0, 0)->setTranslation
        (
            {
                cog_current_x_ + x_shift,
                y_shift
            }
        );
    }
    {
        const auto x_shift = std::cosf(cog_current_rotation_rad_ + M_PI) / 2.0f;
        const auto y_shift = std::sinf(cog_current_rotation_rad_ + M_PI) / 2.0f;

        at(tiles_, 1, 0)->setTranslation
        (
            {
                cog_current_x_ + x_shift,
                y_shift
            }
        );
    }
}

void input::handle_key_press(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            if(!keyboard_state_.left_shift_button_pressed)
            {
                keyboard_state_.left_shift_button_pressed = true;
                last_received_order_ = order::shift_left;
                update_cog_target_position();
            }
            break;
        case key_event::Key::Right:
            if(!keyboard_state_.right_shift_button_pressed)
            {
                keyboard_state_.right_shift_button_pressed = true;
                last_received_order_ = order::shift_right;
                update_cog_target_position();
            }
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            if(!keyboard_state_.rotate_button_pressed)
            {
                keyboard_state_.rotate_button_pressed = true;
                cog_target_rotation_ = (cog_target_rotation_ + 1) % 4;
                last_received_order_ = order::rotate;
                update_cog_target_position();
            }
            break;
        default:
            break;
    }
}

void input::handle_key_release(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            keyboard_state_.left_shift_button_pressed = false;
            update_cog_target_position();
            break;
        case key_event::Key::Right:
            keyboard_state_.right_shift_button_pressed = false;
            update_cog_target_position();
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            keyboard_state_.rotate_button_pressed = false;
            break;
        default:
            break;
    }
}

void input::update_cog_target_position()
{
    const auto dir = get_direction(keyboard_state_);

    //First of all, compute the target position of the center of gravity (COG).
    cog_target_x_ = [&]() -> float
    {
        const auto vertical = (cog_target_rotation_ % 2 == 1);

        switch(dir)
        {
            case direction::left:
                if(vertical)
                {
                    return -2.5f;
                }
                else
                {
                    return -2;
                }
            case direction::right:
                if(vertical)
                {
                    return 2.5f;
                }
                else
                {
                    return 2;
                }
            case direction::neutral:
            default:
                return get_nearest_valid_cog_x
                (
                    cog_current_x_,
                    vertical,
                    last_received_order_
                );
        }
    }();
}

} //namespace
