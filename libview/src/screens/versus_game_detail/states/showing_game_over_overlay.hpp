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

#ifndef LIBVIEW_SCREENS_VERSUS_GAME_DETAIL_STATES_SHOWING_GAME_OVER_OVERLAY_HPP
#define LIBVIEW_SCREENS_VERSUS_GAME_DETAIL_STATES_SHOWING_GAME_OVER_OVERLAY_HPP

#include "../fsm.hpp"
#include "../../../objects/game_over_overlay.hpp"
#include <libview/data_types.hpp>
#include <libutil/fsm.hpp>

namespace libview::screens::versus_game_detail::states
{

class showing_game_over_overlay: public libutil::fsm::state
{
    public:
        showing_game_over_overlay(fsm& fsm):
            fsm_(fsm),
            pgame_over_overlay_
            (
                std::make_shared<objects::game_over_overlay>
                (
                    fsm_.get_context().screen,
                    fsm_.get_context().feature_groups.drawables,
                    fsm_.get_context().feature_groups.clickables,
                    fsm_.get_context().move_count,
                    fsm_.get_context().time_s,
                    objects::game_over_overlay::callback_set
                    {
                        .handle_exit_button_press = [this]
                        {
                            fsm_.get_context().callbacks.handle_exit_request();
                        },
                        .handle_new_game_button_press = [this]
                        {
                            fsm_.get_context().callbacks.handle_clear_request();
                        }
                    }
                )
            )
        {
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
                    .interpolator = animation::get_cubic_out_position_interpolator()
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
            fsm_.get_context().animator.push(std::move(anim));
        }

        ~showing_game_over_overlay()
        {
            auto anim = animation::animation{};
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pgame_over_overlay_,
                    .finish_position = {0.0f, 5.5f},
                    .duration_s = 0.5f,
                    .interpolator = animation::get_cubic_out_position_interpolator()
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
            fsm_.get_context().animator.push(std::move(anim));
        }

        void handle_event(const std::any& event)
        {
            if(std::any_cast<events::new_game_request>(&event))
            {
                fsm_.set_state<playing>();
            }
        }

    private:
        fsm& fsm_;
        std::shared_ptr<objects::game_over_overlay> pgame_over_overlay_;
};

} //namespace

#endif
