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

    int get_nearest_column(const float x)
    {
        return static_cast<int>(std::round(x + 2.5f));
    }

    float get_column_position(const int col)
    {
        return -2.5f + col;
    }

    const auto speed_ups = 7.0f; //in unit per second
    const auto tolerance_u = 0.1f; //in unit
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
    tiles_.data[0] = std::make_shared<number_tile>(*this, drawables_, 5);
    tiles_.data[0]->scale({0.46f, 0.46f});

    update();
}

void input::advance(const libutil::time_point& /*now*/, float elapsed_s)
{
    auto& tile = *tiles_.data[0];

    const auto x_pos = tile.translation().x();
    if(std::abs(x_pos - target_pos_.x()) > tolerance_u)
    {
        if(x_pos < target_pos_.x())
        {
            tile.translate({speed_ups * elapsed_s, 0});
        }
        else
        {
            tile.translate({-speed_ups * elapsed_s, 0});
        }
    }
    else
    {
        tile.setTranslation(target_pos_);
    }
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

    auto& tile = *tiles_.data[0];

    if(!opt_pressed_key.has_value())
    {
        const auto current_pos = tile.translation().x();
        const auto nearest_column = get_nearest_column(current_pos);
        target_pos_ = {get_column_position(nearest_column), 0};
    }
    else
    {
        switch(*opt_pressed_key)
        {
            case key_event::Key::Left:
                target_pos_ = {-2.5, 0};
                break;
            case key_event::Key::Right:
                target_pos_ = {2.5, 0};
                break;
            default:
                break;
        }
    }
}

} //namespace
