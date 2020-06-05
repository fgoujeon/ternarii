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
#include <cassert>

namespace libview::objects::tile_grid_detail
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

    /*
    Size = 1:
        --
        x-

    Size = 2:
        --
        xx

    Size = 3:
        x-
        xx

    Size = 4:
        xx
        xx

    Other configurations are invalid and are considered 4 sized.
    */
    int get_size(const input_tile_object_matrix& tiles)
    {
        assert(at(tiles, 0, 0));

        if(!at(tiles, 1, 0))
        {
            assert(!at(tiles, 0, 1));
            assert(!at(tiles, 1, 1));
            return 1;
        }

        if(!at(tiles, 0, 1))
        {
            assert(!at(tiles, 1, 1));
            return 2;
        }

        if(!at(tiles, 1, 1))
        {
            return 3;
        }

        return 4;
    }

    libutil::matrix<Magnum::Vector2, 2, 2> compute_tile_positions
    (
        const int input_size,
        const float cog_current_x,
        const float cog_current_rotation_rad
    )
    {
        auto positions = libutil::matrix<Magnum::Vector2, 2, 2>{};

        switch(input_size)
        {
            case 1:
                at(positions, 0, 0) = {cog_current_x, 0.0f};
                break;

            case 2:
                at(positions, 0, 0) =
                {
                    std::cosf(cog_current_rotation_rad + M_PI) / 2.0f + cog_current_x,
                    std::sinf(cog_current_rotation_rad + M_PI) / 2.0f
                };
                at(positions, 1, 0) =
                {
                    std::cosf(cog_current_rotation_rad) / 2.0f + cog_current_x,
                    std::sinf(cog_current_rotation_rad) / 2.0f
                };
                break;

            default:
                {
                    const auto angle_gap = M_PI / 4.0f;
                    const auto radius = 0.707f; //sqrt(0.5)

                    at(positions, 0, 0) =
                    {
                        std::cosf(cog_current_rotation_rad - 3 * angle_gap) * radius + cog_current_x,
                        std::sinf(cog_current_rotation_rad - 3 * angle_gap) * radius
                    };
                    at(positions, 0, 1) =
                    {
                        std::cosf(cog_current_rotation_rad + 3 * angle_gap) * radius + cog_current_x,
                        std::sinf(cog_current_rotation_rad + 3 * angle_gap) * radius
                    };
                    at(positions, 1, 0) =
                    {
                        std::cosf(cog_current_rotation_rad - 1 * angle_gap) * radius + cog_current_x,
                        std::sinf(cog_current_rotation_rad - 1 * angle_gap) * radius
                    };
                    at(positions, 1, 1) =
                    {
                        std::cosf(cog_current_rotation_rad + 1 * angle_gap) * radius + cog_current_x,
                        std::sinf(cog_current_rotation_rad + 1 * angle_gap) * radius
                    };
                }
                break;
        }

        return positions;
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

    float move_toward_clockwise(const float current_rad, const float target_rad, const float step_rad)
    {
        const auto tolerance = static_cast<float>(M_PI) / 8.0f;

        if(std::abs(current_rad - target_rad) < tolerance || step_rad > 2 * M_PI)
        {
            return target_rad;
        }

        if(target_rad < current_rad)
        {
            return current_rad - step_rad;
        }
        else
        {
            return current_rad - step_rad + 2 * M_PI;
        }
    }

    const auto tile_move_interpolator = Magnum::Animation::ease
    <
        Magnum::Vector2,
        Magnum::Math::lerp,
        Magnum::Animation::Easing::cubicOut
    >();
}

input::input
(
    Object2D& parent,
    features::animable_group& animables,
    const drop_request_callback& drop_cb
):
    Object2D(&parent),
    features::animable{*this, &animables},
    drop_request_callback_(drop_cb)
{
    update_cog_target_position();
}

void input::set_tiles(const input_tile_object_matrix& tiles)
{
    const auto input_size = get_size(tiles);

    //Init tiles.
    tiles_ = tiles;
    for(auto& ptile: tiles)
    {
        if(ptile)
        {
            ptile->setParentKeepTransformation(this);
        }
    }

    //Reset center of gravity.
    cog_current_rotation_rad_ = cog_target_rotation_ = 0;
    if(input_size == 1)
    {
        cog_current_x_ = cog_target_x_ = -0.5;
    }
    else
    {
        cog_current_x_ = cog_target_x_ = 0;
    }

    //Animate insertion of tiles.
    {
        const auto animation_duration_s = 0.2f;

        auto anim = animation{};

        const auto dst_positions = compute_tile_positions
        (
            input_size,
            cog_current_x_,
            cog_current_rotation_rad_
        );

        libutil::for_each
        (
            [&](const auto& ptile, const auto& dst_position)
            {
                if(ptile)
                {
                    anim.add
                    (
                        tracks::fixed_duration_translation
                        {
                            ptile,
                            dst_position,
                            animation_duration_s,
                            tile_move_interpolator
                        }
                    );
                    anim.add(tracks::alpha_transition{ptile, 1, animation_duration_s});
                }
            },
            tiles_,
            dst_positions
        );

        insertion_animator_.push(std::move(anim));
    }
}

input_tile_object_matrix input::release_tile_objects()
{
    auto tiles = tiles_;
    for(auto& ptile: tiles_)
    {
        ptile.reset();
    }
    return tiles;
}

void input::suspend()
{
    suspended_ = true;
}

void input::resume()
{
    suspended_ = false;
}

void input::advance(const libutil::time_point& now, float elapsed_s)
{
    if(suspended_)
    {
        return;
    }

    //Finish insertion animation before doing anything else.
    if(insertion_animator_.is_animating())
    {
        insertion_animator_.advance(now);
        return;
    }

    /*
    Note: we want to synchronize rotation and translation speeds so that it
    takes the same amount of time to animate a 90 degree rotation and a 0.5 unit
    translation (which always happen simultaneously when rotating a 2 tile
    input). Otherwise the animation seems weird.
    */
    const auto translation_speed = 7.0f; //in units/s
    const auto translation_step = translation_speed * elapsed_s;
    const auto rotation_speed_radps = translation_speed * M_PI; //in radians/s
    const auto rotation_step_rad = rotation_speed_radps * elapsed_s;

    //First, define current position of COG.
    cog_current_x_ = move_toward
    (
        cog_current_x_,
        cog_target_x_,
        translation_step
    );
    cog_current_rotation_rad_ = move_toward_clockwise
    (
        cog_current_rotation_rad_,
        -cog_target_rotation_ * M_PI / 2.0f,
        rotation_step_rad
    );

    //Then, move the tiles relatively to the COG current position and rotation.
    const auto positions = compute_tile_positions
    (
        get_size(tiles_),
        cog_current_x_,
        cog_current_rotation_rad_
    );
    libutil::for_each
    (
        [&](auto& ptile, const auto& position)
        {
            if(ptile)
            {
                ptile->setTranslation(position);
            }
        },
        tiles_,
        positions
    );
}

void input::handle_button_press(data_types::move_button button)
{
    switch(button)
    {
        case data_types::move_button::left_shift:
            keyboard_state_.left_shift_button_pressed = true;
            last_received_order_ = order::shift_left;
            update_cog_target_position();
            break;
        case data_types::move_button::right_shift:
            keyboard_state_.right_shift_button_pressed = true;
            last_received_order_ = order::shift_right;
            update_cog_target_position();
            break;
        case data_types::move_button::clockwise_rotation:
            keyboard_state_.rotate_button_pressed = true;
            if(get_size(tiles_) != 1)
            {
                cog_target_rotation_ = (cog_target_rotation_ + 1) % 4;
            }
            last_received_order_ = order::rotate;
            update_cog_target_position();
            break;
        case data_types::move_button::drop:
            if
            (
                !keyboard_state_.left_shift_button_pressed &&
                !keyboard_state_.right_shift_button_pressed &&
                !keyboard_state_.rotate_button_pressed &&
                !insertion_animator_.is_animating()
            )
            {
                const auto special_case_offset =
                    get_size(tiles_) == 2 && cog_target_rotation_ == 3 ?
                    -1 :
                    0
                ;

                const auto col_offset = static_cast<int>
                (
                    std::round
                    (
                        cog_target_x_ +
                        2.4f //not 2.5, to make sure we don't round integers up
                    )
                ) + special_case_offset;

                drop_request_callback_
                (
                    data_types::input_layout
                    {
                        .col_offset = col_offset,
                        .rotation = cog_target_rotation_
                    }
                );
            }
    }
}

void input::handle_button_release(data_types::move_button button)
{
    switch(button)
    {
        case data_types::move_button::left_shift:
            keyboard_state_.left_shift_button_pressed = false;
            update_cog_target_position();
            break;
        case data_types::move_button::right_shift:
            keyboard_state_.right_shift_button_pressed = false;
            update_cog_target_position();
            break;
        case data_types::move_button::clockwise_rotation:
            keyboard_state_.rotate_button_pressed = false;
            break;
        default:
            break;
    }
}

void input::update_cog_target_position()
{
    cog_target_x_ = [&]() -> float
    {
        //Is current input (as currently rotated) one-tile large?
        const auto vertical = [&]
        {
            const auto size = get_size(tiles_);

            if(size == 1)
            {
                return true;
            }

            //vertical position
            if(size == 2 && (cog_target_rotation_ % 2 == 1))
            {
                return true;
            }

            return false;
        }();

        switch(get_direction(keyboard_state_))
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
