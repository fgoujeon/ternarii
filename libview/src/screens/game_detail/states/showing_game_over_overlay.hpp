/*
Copyright 2018 - 2021 Florian Goujeon

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

#ifndef LIBVIEW_SCREENS_GAME_DETAIL_STATES_SHOWING_GAME_OVER_OVERLAY_HPP
#define LIBVIEW_SCREENS_GAME_DETAIL_STATES_SHOWING_GAME_OVER_OVERLAY_HPP

#include "../context.hpp"
#include "../../../objects/game_over_overlay.hpp"
#include <libview/data_types.hpp>
#include <fgfsm.hpp>

namespace libview::screens::game_detail
{

class showing_game_over_overlay
{
    public:
        showing_game_over_overlay(context& ctx):
            ctx_(ctx)
        {
        }

        void on_entry()
        {
            pgame_over_overlay_ =
                std::make_shared<objects::game_over_overlay>
                (
                    ctx_.screen,
                    ctx_.feature_groups.drawables,
                    ctx_.feature_groups.clickables,
                    ctx_.move_count,
                    ctx_.time_s,
                    objects::game_over_overlay::callback_set
                    {
                        .handle_exit_button_press = [this]
                        {
                            ctx_.callbacks.handle_exit_request();
                        },
                        .handle_new_game_button_press = [this]
                        {
                            ctx_.callbacks.handle_clear_request();
                        }
                    }
                )
            ;

            pgame_over_overlay_->translate({0.0f, 5.5f});
            pgame_over_overlay_->set_alpha(0);

            auto anim = animation::animation{};
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pgame_over_overlay_,
                    .finish_position = {0.0f, 4.5f},
                    .duration_s = 0.5f,
                    .interpolator = animation::get_cubic_out_vector2_interpolator()
                }
            );
            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pgame_over_overlay_,
                    .finish_alpha = 1.0f,
                    .duration_s = 0.5f
                }
            );
            ctx_.animator.push(std::move(anim));
        }

        void on_exit()
        {
            auto anim = animation::animation{};
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pgame_over_overlay_,
                    .finish_position = {0.0f, 5.5f},
                    .duration_s = 0.5f,
                    .interpolator = animation::get_cubic_out_vector2_interpolator()
                }
            );
            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pgame_over_overlay_,
                    .finish_alpha = 0.0f,
                    .duration_s = 0.5f
                }
            );
            ctx_.animator.push(std::move(anim));

            pgame_over_overlay_.reset();
        }

    private:
        context& ctx_;
        std::shared_ptr<objects::game_over_overlay> pgame_over_overlay_;
};

} //namespace

#endif
