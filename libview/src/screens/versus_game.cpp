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

#include <libview/screens/versus_game.hpp>
#include "versus_game_detail/states/playing.hpp"
#include "versus_game_detail/states/showing_game_over_overlay.hpp"
#include "versus_game_detail/fsm.hpp"
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
#include <chrono>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

namespace
{
    std::unique_ptr<objects::sdf_image> make_background_image
    (
        versus_game& parent,
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

#define MOVE_BUTTON_INITIALIZER(IMAGE, PLAYER, MOVE) \
    p##PLAYER##_##MOVE##_button \
    ( \
        self, \
        feature_groups.drawables, \
        feature_groups.clickables, \
        IMAGE, \
        objects::sdf_image_button::callback_set \
        { \
            .handle_mouse_press = [this] \
            { \
                fsm.handle_event(versus_game_detail::events::p##PLAYER##_button_press{data_types::move_button::MOVE}); \
            }, \
            .handle_mouse_release = [this] \
            { \
                fsm.handle_event(versus_game_detail::events::p##PLAYER##_button_release{data_types::move_button::MOVE}); \
            } \
        } \
    )

struct versus_game::impl
{
    impl
    (
        versus_game& self,
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
                    fsm.handle_event(versus_game_detail::events::pause_request{});
                }
            }
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

        p1_score_name_label
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
        p1_score_display(self, feature_groups.drawables),
        p1_granite_counter_name_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleLeft,
                .color = colors::light_gray,
                .font_size = 0.28f,
                .outline_range = {0.5f, 1.0f}
            },
            "GRANITE COUNTER"
        ),
        p1_granite_counter(self, feature_groups.drawables),
        p1_tile_grid
        (
            self,
            feature_groups.drawables,
            feature_groups.animables,
            animator,
            callbacks.handle_p1_drop_request,
            callbacks.handle_p1_input_layout_change
        ),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   1, left_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   1, right_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   1, drop),
        MOVE_BUTTON_INITIALIZER(libres::images::rotate_button, 1, clockwise_rotation),

        p2_score_name_label
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
        p2_score_display(self, feature_groups.drawables),
        p2_granite_counter_name_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleLeft,
                .color = colors::light_gray,
                .font_size = 0.28f,
                .outline_range = {0.5f, 1.0f}
            },
            "GRANITE COUNTER"
        ),
        p2_granite_counter(self, feature_groups.drawables),
        p2_tile_grid
        (
            self,
            feature_groups.drawables,
            feature_groups.animables,
            animator,
            callbacks.handle_p2_drop_request,
            callbacks.handle_p2_input_layout_change
        ),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   2, left_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   2, right_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   2, drop),
        MOVE_BUTTON_INITIALIZER(libres::images::rotate_button, 2, clockwise_rotation)
    {
        menu_label.setTranslation({0.0f, 7.25f});
        menu_button.scale({0.5f, 0.5f});
        menu_button.translate({0.0f, 6.75f});

        stage_name_label.translate({0.0f, -4.68f});

        p1_granite_counter_name_label.setScaling({0.75f, 0.75f});
        p1_granite_counter_name_label.setTranslation({-7.85f, 7.25f});
        p1_granite_counter.setScaling({0.75f, 0.75f});
        p1_granite_counter.setTranslation({-7.85f, 7.25f});
        p1_score_name_label.setTranslation({-1.75f, 7.25f});
        p1_score_display.setScaling({0.75f, 0.75f});
        p1_score_display.setTranslation({-1.6f, 7.2f});
        p1_tile_grid.translate({-5.0f, 1.0f});

        //Player 1 move buttons
        {
            const auto move_button_scaling = 0.95f;

            p1_left_shift_button.scale({move_button_scaling, move_button_scaling});
            p1_left_shift_button.translate({-8.25f, -5.85f});

            p1_right_shift_button.rotate(180.0_degf);
            p1_right_shift_button.scale({move_button_scaling, move_button_scaling});
            p1_right_shift_button.translate({-6.4f, -6.75f});

            p1_drop_button.rotate(90.0_degf);
            p1_drop_button.scale({move_button_scaling, move_button_scaling});
            p1_drop_button.translate({-3.6f, -6.75f});

            p1_clockwise_rotation_button.scale({move_button_scaling, move_button_scaling});
            p1_clockwise_rotation_button.translate({-1.75f, -5.85f});
        }

        p2_granite_counter_name_label.setScaling({0.75f, 0.75f});
        p2_granite_counter_name_label.setTranslation({2.0f, 7.25f});
        p2_granite_counter.setScaling({0.75f, 0.75f});
        p2_granite_counter.setTranslation({2.0f, 7.25f});
        p2_score_name_label.setTranslation({8.25f, 7.25f});
        p2_score_display.setScaling({0.75f, 0.75f});
        p2_score_display.setTranslation({8.4f, 7.2f});
        p2_tile_grid.translate({5.0f, 1.0f});

        //Player 2 move buttons
        {
            const auto move_button_scaling = 0.95f;

            p2_left_shift_button.scale({move_button_scaling, move_button_scaling});
            p2_left_shift_button.translate({1.75f, -5.85f});

            p2_right_shift_button.rotate(180.0_degf);
            p2_right_shift_button.scale({move_button_scaling, move_button_scaling});
            p2_right_shift_button.translate({3.6f, -6.75f});

            p2_drop_button.rotate(90.0_degf);
            p2_drop_button.scale({move_button_scaling, move_button_scaling});
            p2_drop_button.translate({6.4f, -6.75f});

            p2_clockwise_rotation_button.scale({move_button_scaling, move_button_scaling});
            p2_clockwise_rotation_button.translate({8.25f, -5.85f});
        }

        fsm.set_state<versus_game_detail::states::playing>();
    }

    versus_game& self;

    feature_group_set& feature_groups;

    callback_set callbacks;

    animation::animator animator;
    animation::animator pause_animator;

    std::unique_ptr<objects::sdf_image> pbackground_image;
    objects::label menu_label;
    objects::sdf_image_button menu_button;
    objects::label stage_name_label;

    objects::label p1_score_name_label;
    objects::score_display p1_score_display;
    objects::label p1_granite_counter_name_label;
    objects::score_display p1_granite_counter;
    objects::tile_grid p1_tile_grid;
    objects::sdf_image_button p1_left_shift_button;
    objects::sdf_image_button p1_right_shift_button;
    objects::sdf_image_button p1_drop_button;
    objects::sdf_image_button p1_clockwise_rotation_button;

    objects::label p2_score_name_label;
    objects::score_display p2_score_display;
    objects::label p2_granite_counter_name_label;
    objects::score_display p2_granite_counter;
    objects::tile_grid p2_tile_grid;
    objects::sdf_image_button p2_left_shift_button;
    objects::sdf_image_button p2_right_shift_button;
    objects::sdf_image_button p2_drop_button;
    objects::sdf_image_button p2_clockwise_rotation_button;

    versus_game_detail::fsm_context ctx
    {
        self,
        feature_groups,
        callbacks,
        animator,
        pause_animator,
        p1_tile_grid,
        p2_tile_grid
    };
    versus_game_detail::fsm fsm{ctx};
};

versus_game::versus_game
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

versus_game::~versus_game() = default;

void versus_game::advance(const std::chrono::steady_clock::time_point& now, const float /*elapsed_s*/)
{
    pimpl_->fsm.handle_event(versus_game_detail::events::iteration{});
    pimpl_->animator.advance(now);
    pimpl_->pause_animator.advance(now);
}

void versus_game::handle_key_press(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::A:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_press{data_types::move_button::left_shift});
            break;
        case key_event::Key::D:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_press{data_types::move_button::right_shift});
            break;
        case key_event::Key::W:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_press{data_types::move_button::clockwise_rotation});
            break;
        case key_event::Key::S:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_press{data_types::move_button::drop});
            break;

        case key_event::Key::Left:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_press{data_types::move_button::left_shift});
            break;
        case key_event::Key::Right:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_press{data_types::move_button::right_shift});
            break;
        case key_event::Key::Up:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_press{data_types::move_button::clockwise_rotation});
            break;
        case key_event::Key::Down:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_press{data_types::move_button::drop});
            break;

        default:
            break;
    }
}

void versus_game::handle_key_release(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::A:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_release{data_types::move_button::left_shift});
            break;
        case key_event::Key::D:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_release{data_types::move_button::right_shift});
            break;
        case key_event::Key::W:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_release{data_types::move_button::clockwise_rotation});
            break;
        case key_event::Key::S:
            pimpl_->fsm.handle_event(versus_game_detail::events::p1_button_release{data_types::move_button::drop});
            break;

        case key_event::Key::Left:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_release{data_types::move_button::left_shift});
            break;
        case key_event::Key::Right:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_release{data_types::move_button::right_shift});
            break;
        case key_event::Key::Up:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_release{data_types::move_button::clockwise_rotation});
            break;
        case key_event::Key::Down:
            pimpl_->fsm.handle_event(versus_game_detail::events::p2_button_release{data_types::move_button::drop});
            break;
        default:
            break;
    }
}

const data_types::input_layout& versus_game::get_p1_input_layout() const
{
    return pimpl_->p1_tile_grid.get_input_layout();
}

const data_types::input_layout& versus_game::get_p2_input_layout() const
{
    return pimpl_->p2_tile_grid.get_input_layout();
}

void versus_game::clear()
{
    pimpl_->p1_tile_grid.clear();
    pimpl_->p2_tile_grid.clear();
    set_game_over_overlay_visible(false);
}

void versus_game::set_p1_score(const int value)
{
    pimpl_->p1_score_display.set_score(value);
}

void versus_game::set_p2_score(const int value)
{
    pimpl_->p2_score_display.set_score(value);
}

void versus_game::set_p1_granite_counter(const int value)
{
    pimpl_->p1_granite_counter.set_score(value);
}

void versus_game::set_p2_granite_counter(const int value)
{
    pimpl_->p2_granite_counter.set_score(value);
}

void versus_game::create_p1_next_input(const data_types::input_tile_matrix& tiles)
{
    pimpl_->p1_tile_grid.create_next_input(tiles);
}

void versus_game::create_p2_next_input(const data_types::input_tile_matrix& tiles)
{
    pimpl_->p2_tile_grid.create_next_input(tiles);
}

void versus_game::insert_p1_next_input()
{
    pimpl_->p1_tile_grid.insert_next_input();
}

void versus_game::insert_p2_next_input()
{
    pimpl_->p2_tile_grid.insert_next_input();
}

void versus_game::drop_p1_input_tiles(const data_types::input_tile_drop_list& drops)
{
    pimpl_->p1_tile_grid.drop_input_tiles(drops);
}

void versus_game::drop_p2_input_tiles(const data_types::input_tile_drop_list& drops)
{
    pimpl_->p2_tile_grid.drop_input_tiles(drops);
}

void versus_game::drop_p1_board_tiles(const data_types::board_tile_drop_list& drops)
{
    pimpl_->p1_tile_grid.drop_board_tiles(drops);
}

void versus_game::drop_p2_board_tiles(const data_types::board_tile_drop_list& drops)
{
    pimpl_->p2_tile_grid.drop_board_tiles(drops);
}

void versus_game::nullify_p1_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates)
{
    pimpl_->p1_tile_grid.nullify_tiles(nullified_tile_coordinates);
}

void versus_game::nullify_p2_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates)
{
    pimpl_->p2_tile_grid.nullify_tiles(nullified_tile_coordinates);
}

void versus_game::merge_p1_tiles
(
    const data_types::tile_merge_list& merges,
    const data_types::granite_erosion_list& granite_erosions
)
{
    pimpl_->p1_tile_grid.merge_tiles(merges, granite_erosions);
}

void versus_game::merge_p2_tiles
(
    const data_types::tile_merge_list& merges,
    const data_types::granite_erosion_list& granite_erosions
)
{
    pimpl_->p2_tile_grid.merge_tiles(merges, granite_erosions);
}

void versus_game::mark_p1_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates)
{
    pimpl_->p1_tile_grid.mark_tiles_for_nullification(tile_coordinates);
}

void versus_game::mark_p2_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates)
{
    pimpl_->p2_tile_grid.mark_tiles_for_nullification(tile_coordinates);
}

void versus_game::set_game_over_overlay_visible(const bool visible)
{
    if(visible)
    {
        pimpl_->fsm.handle_event(versus_game_detail::events::game_over{});
    }
    else
    {
        pimpl_->fsm.handle_event(versus_game_detail::events::new_game_request{});
    }
}

} //namespace
