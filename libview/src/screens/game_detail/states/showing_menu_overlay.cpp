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

namespace libview::screens::game_detail
{

showing_menu_overlay::showing_menu_overlay(context& ctx):
    ctx_(ctx)
{
}

void showing_menu_overlay::on_entry()
{
    pmenu_overlay_ = std::make_shared<objects::game_menu_overlay>
    (
        ctx_.screen,
        ctx_.feature_groups.drawables,
        ctx_.feature_groups.clickables,
        objects::game_menu_overlay::callback_set
        {
            .handle_exit_request = [this]
            {
                ctx_.callbacks.handle_exit_request();
            },
            .handle_resume_request = [this]
            {
                ctx_.process_event(events::pause_request{});
            }
        }
    );

    ctx_.animator.pause();

    pmenu_overlay_->setTranslation({0.0f, 3.5f});
    pmenu_overlay_->set_alpha(0);
    pmenu_overlay_->set_time_s(ctx_.time_s);
    pmenu_overlay_->set_move_count(ctx_.move_count);
    pmenu_overlay_->set_hi_score(ctx_.hi_score);

    auto anim = animation::animation{};
    anim.add
    (
        animation::tracks::fixed_duration_translation
        {
            .pobj = pmenu_overlay_,
            .finish_position = {0.0f, 3.0f},
            .duration_s = 0.3f,
            .interpolator = animation::get_cubic_out_vector2_interpolator()
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
    ctx_.pause_animator.push(std::move(anim));
}

void showing_menu_overlay::on_event(const fgfsm::event_ref& event)
{
    visit
    (
        event,

        [this](const events::pause_request&)
        {
            ctx_.process_event(resume_request{});
        },

        [this](const events::iteration&)
        {
            pmenu_overlay_->set_time_s(ctx_.time_s);
        }
    );
}

void showing_menu_overlay::on_exit()
{
    auto anim = animation::animation{};
    anim.add
    (
        animation::tracks::fixed_duration_translation
        {
            .pobj = pmenu_overlay_,
            .finish_position = {0.0f, 3.5f},
            .duration_s = 0.3f,
            .interpolator = animation::get_cubic_out_vector2_interpolator()
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
    ctx_.pause_animator.push(std::move(anim));

    ctx_.animator.resume();

    pmenu_overlay_.reset();
}

} //namespace
