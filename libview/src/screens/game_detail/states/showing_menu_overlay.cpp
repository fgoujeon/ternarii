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

#include "showing_menu_overlay.hpp"
#include "playing.hpp"

namespace libview::screens::game_detail::states
{

showing_menu_overlay::showing_menu_overlay(fsm& fsm):
    fsm_(fsm),
    pmenu_overlay_
    (
        std::make_shared<objects::game_menu_overlay>
        (
            fsm_.get_context().screen,
            fsm_.get_context().feature_groups.drawables,
            fsm_.get_context().feature_groups.clickables,
            objects::game_menu_overlay::callback_set
            {
                .handle_exit_request = [this]
                {
                    fsm_.get_context().callbacks.handle_exit_request();
                },
                .handle_resume_request = [this]
                {
                    handle_event(events::pause_request{});
                }
            }
        )
    )
{
    fsm_.get_context().animator.pause();

    pmenu_overlay_->setTranslation({0.0f, 3.5f});
    pmenu_overlay_->set_alpha(0);
    pmenu_overlay_->set_time_s(fsm_.get_context().time_s);
    pmenu_overlay_->set_move_count(fsm_.get_context().move_count);
    pmenu_overlay_->set_hi_score(fsm_.get_context().hi_score);

    auto anim = animation::animation{};
    anim.add
    (
        animation::tracks::fixed_duration_translation
        {
            .pobj = pmenu_overlay_,
            .finish_position = {0.0f, 3.0f},
            .duration_s = 0.3f,
            .interpolator = animation::get_cubic_out_position_interpolator()
        }
    );
    anim.add
    (
        animation::tracks::alpha_transition
        {
            .pobj = pmenu_overlay_,
            .finish_alpha = 1.0f,
            .duration_s = 0.3f
        }
    );
    fsm_.get_context().pause_animator.push(std::move(anim));
}

showing_menu_overlay::~showing_menu_overlay()
{
    auto anim = animation::animation{};
    anim.add
    (
        animation::tracks::fixed_duration_translation
        {
            .pobj = pmenu_overlay_,
            .finish_position = {0.0f, 3.5f},
            .duration_s = 0.3f,
            .interpolator = animation::get_cubic_out_position_interpolator()
        }
    );
    anim.add
    (
        animation::tracks::alpha_transition
        {
            .pobj = pmenu_overlay_,
            .finish_alpha = 0.0f,
            .duration_s = 0.3f
        }
    );
    fsm_.get_context().pause_animator.push(std::move(anim));

    fsm_.get_context().animator.resume();
}

void showing_menu_overlay::handle_event(const std::any& event)
{
    if(std::any_cast<events::pause_request>(&event))
    {
        fsm_.set_state<playing>();
    }
    else if(std::any_cast<events::iteration>(&event))
    {
        pmenu_overlay_->set_time_s(fsm_.get_context().time_s);
    }
}

} //namespace
