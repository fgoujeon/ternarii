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

#include "paused.hpp"
#include "playing.hpp"

namespace libview::screens::game_detail::states
{

paused::paused(fsm& fsm):
    fsm_(fsm),
    ppause_overlay_
    (
        std::make_shared<objects::pause_overlay>
        (
            fsm_.get_context().screen,
            fsm_.get_context().feature_groups.drawables,
            fsm_.get_context().feature_groups.clickables,
            objects::pause_overlay::callback_set
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

    ppause_overlay_->setTranslation({0.0f, 4.0f});
    ppause_overlay_->set_alpha(0);
    ppause_overlay_->set_hi_score(fsm_.get_context().hi_score);

    auto anim = animation::animation{};
    anim.add
    (
        animation::tracks::fixed_duration_translation
        {
            .pobj = ppause_overlay_,
            .finish_position = {0.0f, 3.0f},
            .duration_s = 0.5f,
            .interpolator = animation::get_cubic_out_position_interpolator()
        }
    );
    anim.add
    (
        animation::tracks::alpha_transition
        {
            .pobj = ppause_overlay_,
            .finish_alpha = 1.0f,
            .duration_s = 0.5f
        }
    );
    fsm_.get_context().pause_animator.push(std::move(anim));
}

paused::~paused()
{
    auto anim = animation::animation{};
    anim.add
    (
        animation::tracks::fixed_duration_translation
        {
            .pobj = ppause_overlay_,
            .finish_position = {0.0f, 4.0f},
            .duration_s = 0.5f,
            .interpolator = animation::get_cubic_out_position_interpolator()
        }
    );
    anim.add
    (
        animation::tracks::alpha_transition
        {
            .pobj = ppause_overlay_,
            .finish_alpha = 0.0f,
            .duration_s = 0.5f
        }
    );
    fsm_.get_context().pause_animator.push(std::move(anim));

    fsm_.get_context().animator.resume();
}

void paused::handle_event(const std::any& event)
{
    if(std::any_cast<events::pause_request>(&event))
    {
        fsm_.set_state<playing>();
    }
}

} //namespace
