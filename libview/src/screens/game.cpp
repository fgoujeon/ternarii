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

#include <libview/screens/game.hpp>
#include "game_detail/states/playing.hpp"
#include "game_detail/states/showing_game_over_overlay.hpp"
#include "game_detail/fsm.hpp"
#include "game_detail/events.hpp"
#include "../objects/shine.hpp"
#include "../objects/game_over_overlay.hpp"
#include "../objects/sdf_image_button.hpp"
#include "../objects/sdf_image.hpp"
#include "../objects/tile_grid.hpp"
#include "../objects/score_display.hpp"
#include "../animation.hpp"
#include "../colors.hpp"
#include "../data_types.hpp"
#include "../common.hpp"
#include <libres.hpp>
#include <libutil/fsm.hpp>
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

namespace
{
    std::unique_ptr<objects::sdf_image> make_background_image
    (
        game& parent,
        feature_group_set& feature_groups,
        const data_types::stage stage
    )
    {
        const auto opt_background_image_path = data_types::get_image(stage);
        if(!opt_background_image_path)
        {
            return nullptr;
        }

        auto pimage = std::make_unique<objects::sdf_image>
        (
            parent,
            feature_groups.drawables,
            *opt_background_image_path,
            objects::sdf_image::style
            {
                .color = colors::white
            }
        );

        pimage->setScaling({3.0f, 3.0f});
        pimage->set_alpha(0.1f);

        return pimage;
    }
}

#define MOVE_BUTTON_INITIALIZER(IMAGE, MOVE) \
    MOVE##_button \
    ( \
        self, \
        feature_groups.drawables, \
        feature_groups.clickables, \
        IMAGE, \
        objects::sdf_image_button::callback_set \
        { \
            .handle_mouse_press = [this] \
            { \
                fsm.handle_event(game_detail::events::button_press{data_types::move_button::MOVE}); \
            }, \
            .handle_mouse_release = [this] \
            { \
                fsm.handle_event(game_detail::events::button_release{data_types::move_button::MOVE}); \
            } \
        } \
    )

struct game::impl
{
    impl
    (
        game& self,
        feature_group_set& feature_groups,
        const callback_set& callbacks,
        const data_types::stage stage
    ):
        self(self),
        feature_groups(feature_groups),
        callbacks(callbacks),
        pbackground_image
        (
            make_background_image
            (
                self,
                feature_groups,
                stage
            )
        ),
        menu_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleCenter,
                .color = colors::light_gray,
                .font_size = 0.28f,
                .outline_range = {0.5f, 1.0f}
            },
            "MENU"
        ),
        menu_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            libres::images::menu,
            objects::sdf_image_button::callback_set
            {
                .handle_mouse_release = [this]
                {
                    fsm.handle_event(game_detail::events::pause_request{});
                }
            }
        ),
        score_name_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleRight,
                .color = colors::light_gray,
                .font_size = 0.28f,
                .outline_range = {0.5f, 1.0f}
            },
            "SCORE"
        ),
        score_display(self, feature_groups.drawables),
        tile_grid
        (
            self,
            feature_groups.drawables,
            feature_groups.animables,
            animator,
            callbacks.handle_drop_request,
            callbacks.handle_input_layout_change
        ),
        stage_name_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleCenter,
                .color = colors::light_gray,
                .font_size = 0.28f,
                .outline_color = colors::dark_gray,
                .outline_range = {0.47f, 0.40f}
            },
            data_types::get_pretty_name(stage)
        ),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   left_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   right_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   drop),
        MOVE_BUTTON_INITIALIZER(libres::images::rotate_button, clockwise_rotation)
    {
        menu_label.setTranslation({-2.85f, 7.25f});
        menu_button.scale({0.5f, 0.5f});
        menu_button.translate({-2.85f, 6.75f});

        score_name_label.setTranslation({3.25f, 7.25f});
        score_display.setScaling({0.75f, 0.75f});
        score_display.setTranslation({3.4f, 7.2f});

        tile_grid.translate({0.0f, 1.0f});

        stage_name_label.translate({0.0f, -4.68f});

        //Move buttons
        {
            const auto move_button_scaling = 0.95f;

            left_shift_button.scale({move_button_scaling, move_button_scaling});
            left_shift_button.translate({-3.25f, -5.85f});

            right_shift_button.rotate(180.0_degf);
            right_shift_button.scale({move_button_scaling, move_button_scaling});
            right_shift_button.translate({-1.4f, -6.75f});

            drop_button.rotate(90.0_degf);
            drop_button.scale({move_button_scaling, move_button_scaling});
            drop_button.translate({1.4f, -6.75f});

            clockwise_rotation_button.scale({move_button_scaling, move_button_scaling});
            clockwise_rotation_button.translate({3.25f, -5.85f});
        }

        fsm.set_state<game_detail::states::playing>();
    }

    game& self;

    feature_group_set& feature_groups;

    callback_set callbacks;

    animation::animator animator;
    animation::animator pause_animator;

    std::unique_ptr<objects::sdf_image> pbackground_image;
    objects::label menu_label;
    objects::sdf_image_button menu_button;
    objects::label score_name_label;
    objects::score_display score_display;
    objects::tile_grid tile_grid;
    objects::label stage_name_label;
    objects::sdf_image_button left_shift_button;
    objects::sdf_image_button right_shift_button;
    objects::sdf_image_button drop_button;
    objects::sdf_image_button clockwise_rotation_button;

    game_detail::fsm_context ctx
    {
        self,
        feature_groups,
        callbacks,
        animator,
        pause_animator,
        tile_grid
    };
    game_detail::fsm fsm{ctx};
};

game::game
(
    object2d& parent,
    feature_group_set& feature_groups,
    const data_types::stage stage,
    const callback_set& callbacks
):
    object2d{&parent},
    features::animable{*this, &feature_groups.animables},
    features::key_event_handler{*this, &feature_groups.key_event_handlers},
    pimpl_
    (
        std::make_unique<impl>
        (
            *this,
            feature_groups,
            callbacks,
            stage
        )
    )
{
}

game::~game() = default;

void game::advance(const libutil::time::point& now, const float /*elapsed_s*/)
{
    pimpl_->animator.advance(now);
    pimpl_->pause_animator.advance(now);
}

void game::handle_key_press(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            pimpl_->fsm.handle_event(game_detail::events::button_press{data_types::move_button::left_shift});
            break;
        case key_event::Key::Right:
            pimpl_->fsm.handle_event(game_detail::events::button_press{data_types::move_button::right_shift});
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            pimpl_->fsm.handle_event(game_detail::events::button_press{data_types::move_button::clockwise_rotation});
            break;
        case key_event::Key::Down:
            pimpl_->fsm.handle_event(game_detail::events::button_press{data_types::move_button::drop});
            break;
        default:
            break;
    }
}

void game::handle_key_release(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            pimpl_->fsm.handle_event(game_detail::events::button_release{data_types::move_button::left_shift});
            break;
        case key_event::Key::Right:
            pimpl_->fsm.handle_event(game_detail::events::button_release{data_types::move_button::right_shift});
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            pimpl_->fsm.handle_event(game_detail::events::button_release{data_types::move_button::clockwise_rotation});
            break;
        case key_event::Key::Down:
            pimpl_->fsm.handle_event(game_detail::events::button_release{data_types::move_button::drop});
            break;
        default:
            break;
    }
}

const data_types::input_layout& game::get_input_layout() const
{
    return pimpl_->tile_grid.get_input_layout();
}

void game::clear()
{
    pimpl_->tile_grid.clear();
    set_game_over_overlay_visible(false);
}

void game::set_score(const int value)
{
    pimpl_->score_display.set_score(value);
}

void game::set_hi_score(const int value)
{
    pimpl_->ctx.hi_score = value;
}

void game::set_move_count(const int value)
{
    pimpl_->ctx.move_count = value;
}

void game::create_next_input(const data_types::input_tile_matrix& tiles)
{
    pimpl_->tile_grid.create_next_input(tiles);
}

void game::insert_next_input()
{
    pimpl_->tile_grid.insert_next_input();
}

void game::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    pimpl_->tile_grid.drop_input_tiles(drops);
}

void game::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    pimpl_->tile_grid.drop_board_tiles(drops);
}

void game::nullify_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates)
{
    pimpl_->tile_grid.nullify_tiles(nullified_tile_coordinates);
}

void game::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->tile_grid.merge_tiles(merges);
}

void game::mark_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates)
{
    pimpl_->tile_grid.mark_tiles_for_nullification(tile_coordinates);
}

void game::set_board_tiles(const data_types::board_tile_matrix& tiles)
{
    pimpl_->tile_grid.set_board_tiles(tiles);
}

void game::set_game_over_overlay_visible(const bool visible)
{
    if(visible)
    {
        pimpl_->fsm.handle_event(game_detail::events::game_over{});
    }
    else
    {
        pimpl_->fsm.handle_event(game_detail::events::new_game_request{});
    }
}

} //namespace
