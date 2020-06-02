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
#include <iostream>

namespace libview::objects
{

namespace
{
    const auto xmin = -2.5f;
    const auto xmax = 2.5f;

    const auto ymid = 0.0f;

    const auto speed_ups = 7.0f; //in units per second
    const auto tolerance_u = 0.1f; //in units

    std::optional<input::key_event::Key> get_pressed_key(const input::keyboard_state& state)
    {
        if(state.left_pressed && !state.right_pressed)
        {
            return input::key_event::Key::Left;
        }

        if(!state.left_pressed && state.right_pressed)
        {
            return input::key_event::Key::Right;
        }

        return std::nullopt;
    }

    //Get index of nearest column.
    int get_nearest_column(const float current_x, const float target_x)
    {
        //We need this inclination, otherwise the tiles might not move if the
        //user presses and releases a move button a little too quickly.
        const auto inclination = target_x > 0 ? 0.4f : -0.4f;

        const auto x = current_x + inclination;
        return static_cast<int>(std::round(x - xmin));
    }

    float get_column_position(const int col)
    {
        return xmin + col;
    }

    void move_toward(Object2D& tile, const Magnum::Vector2& target_pos, const float step)
    {
        const auto x_pos = tile.translation().x();
        if(std::abs(x_pos - target_pos.x()) > tolerance_u)
        {
            if(x_pos < target_pos.x())
            {
                tile.translate({step, 0});
            }
            else
            {
                tile.translate({-step, 0});
            }
        }
        else
        {
            tile.setTranslation(target_pos);
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

    update();
}

void input::advance(const libutil::time_point& /*now*/, float elapsed_s)
{
    const auto step = speed_ups * elapsed_s;

    libutil::for_each
    (
        [&](auto& ptile, auto& target_pos)
        {
            if(ptile)
            {
                move_toward(*ptile, target_pos, step);
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

    update();
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

    update();
}

void input::update()
{
    const auto opt_pressed_key = get_pressed_key(keyboard_state_);

    if(!opt_pressed_key.has_value())
    {
        libutil::for_each
        (
            [&](auto& ptile, auto& target_pos)
            {
                if(!ptile)
                {
                    return;
                }

                const auto current_pos = ptile->translation().x();
                const auto nearest_column = get_nearest_column(current_pos, target_pos.x());
                target_pos = {get_column_position(nearest_column), ymid};
            },
            tiles_,
            target_positions_
        );
    }
    else
    {
        switch(*opt_pressed_key)
        {
            case key_event::Key::Left:
                at(target_positions_, 0, 0) = {xmin, ymid};
                at(target_positions_, 1, 0) = {xmin + 1, ymid};
                break;
            case key_event::Key::Right:
                at(target_positions_, 0, 0) = {xmax - 1, ymid};
                at(target_positions_, 1, 0) = {xmax, ymid};
                break;
            default:
                break;
        }
    }
}

} //namespace
