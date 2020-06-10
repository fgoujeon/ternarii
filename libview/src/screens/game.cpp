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
#include "../objects/shine.hpp"
#include "../objects/game_over_overlay.hpp"
#include "../objects/sdf_image_button.hpp"
#include "../objects/sdf_image.hpp"
#include "../objects/tile_grid.hpp"
#include "../objects/score_display.hpp"
#include "../animation.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libres.hpp>
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

namespace
{
    std::unique_ptr<objects::shine> make_background
    (
        game& self,
        feature_group_set& feature_groups,
        const bool make
    )
    {
        if(!make)
        {
            return nullptr;
        }

        auto pbackground = std::make_unique<objects::shine>
        (
            self,
            feature_groups.drawables,
            feature_groups.animables
        );

        pbackground->scale({16.0f, 16.0f});
        pbackground->translate({0.0f, -1.0f});
        pbackground->set_color(colors::white);
        pbackground->set_alpha(0.02);

        return pbackground;
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
                tile_grid.handle_button_press(data_types::move_button::MOVE); \
            }, \
            .handle_mouse_release = [this] \
            { \
                tile_grid.handle_button_release(data_types::move_button::MOVE); \
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
        const std::string_view& stage_name,
        const bool show_background
    ):
        feature_groups(feature_groups),
        callbacks(callbacks),
        pbackground(make_background(self, feature_groups, show_background)),
        tile_grid
        (
            self,
            feature_groups.drawables,
            feature_groups.animables,
            animator,
            callbacks.handle_drop_request,
            callbacks.handle_input_layout_change
        ),
        score_display(self, feature_groups.drawables),
        hi_score_display(self, feature_groups.drawables),
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
            stage_name
        ),
        exit_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            libres::images::exit,
            objects::sdf_image_button::callback_set
            {
                .handle_mouse_release = [this]{this->callbacks.handle_exit_request();}
            }
        ),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   left_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   right_shift),
        MOVE_BUTTON_INITIALIZER(libres::images::move_button,   drop),
        MOVE_BUTTON_INITIALIZER(libres::images::rotate_button, clockwise_rotation)
    {
        const auto move_button_scaling = 0.95f;

        score_display.set_visible(true);
        score_display.scale({0.7f, 0.7f});
        score_display.translate({3.4f, 7.6f});

        hi_score_display.scale({0.3f, 0.3f});
        hi_score_display.translate({3.3f, 6.8f});

        tile_grid.translate({0.0f, 1.0f});

        stage_name_label.translate({0.0f, -4.68f});

        exit_button.scale({0.5f, 0.5f});
        exit_button.translate({-2.8f, 7.0f});

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

    feature_group_set& feature_groups;

    callback_set callbacks;

    animation::animator animator;

    std::unique_ptr<objects::shine> pbackground;
    objects::tile_grid tile_grid;
    objects::score_display score_display;
    objects::score_display hi_score_display;
    objects::label stage_name_label;
    objects::sdf_image_button exit_button;
    objects::sdf_image_button left_shift_button;
    objects::sdf_image_button right_shift_button;
    objects::sdf_image_button drop_button;
    objects::sdf_image_button clockwise_rotation_button;

    std::shared_ptr<objects::game_over_overlay> pgame_over_overlay;
};

game::game
(
    Object2D& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks,
    const std::string_view& stage_name,
    const bool show_background
):
    Object2D{&parent},
    features::animable{*this, &feature_groups.animables},
    features::key_event_handler{*this, &feature_groups.key_event_handlers},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks, stage_name, show_background))
{
}

game::~game() = default;

void game::advance(const libutil::time_point& now, const float /*elapsed_s*/)
{
    pimpl_->animator.advance(now);
}

void game::handle_key_press(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            pimpl_->tile_grid.handle_button_press(data_types::move_button::left_shift);
            break;
        case key_event::Key::Right:
            pimpl_->tile_grid.handle_button_press(data_types::move_button::right_shift);
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            pimpl_->tile_grid.handle_button_press(data_types::move_button::clockwise_rotation);
            break;
        case key_event::Key::Down:
            pimpl_->tile_grid.handle_button_press(data_types::move_button::drop);
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
            pimpl_->tile_grid.handle_button_release(data_types::move_button::left_shift);
            break;
        case key_event::Key::Right:
            pimpl_->tile_grid.handle_button_release(data_types::move_button::right_shift);
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            pimpl_->tile_grid.handle_button_release(data_types::move_button::clockwise_rotation);
            break;
        case key_event::Key::Down:
            pimpl_->tile_grid.handle_button_release(data_types::move_button::drop);
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
    if(value != 0)
    {
        pimpl_->hi_score_display.set_score(value);
        pimpl_->hi_score_display.set_visible(true);
    }
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
        if(pimpl_->pgame_over_overlay)
        {
            return;
        }

        pimpl_->pgame_over_overlay = std::make_shared<objects::game_over_overlay>
        (
            *this,
            pimpl_->feature_groups.drawables,
            pimpl_->feature_groups.clickables,
            [this]{pimpl_->callbacks.handle_clear_request();}
        );
        pimpl_->pgame_over_overlay->translate({0.0f, 5.5f});
        pimpl_->pgame_over_overlay->set_alpha(0);

        auto anim = animation::animation{};
        anim.add
        (
            animation::tracks::fixed_duration_translation
            {
                .pobj = pimpl_->pgame_over_overlay,
                .finish_position = {0.0f, 4.5f},
                .duration_s = 0.5f,
                .interpolator = animation::get_cubic_out_position_interpolator()
            }
        );
        anim.add
        (
            animation::tracks::alpha_transition
            {
                .pobj = pimpl_->pgame_over_overlay,
                .finish_alpha = 1.0f,
                .duration_s = 0.5f
            }
        );
        pimpl_->animator.push(std::move(anim));
    }
    else
    {
        if(!pimpl_->pgame_over_overlay)
        {
            return;
        }

        auto anim = animation::animation{};
        anim.add
        (
            animation::tracks::fixed_duration_translation
            {
                .pobj = pimpl_->pgame_over_overlay,
                .finish_position = {0.0f, 5.5f},
                .duration_s = 0.5f,
                .interpolator = animation::get_cubic_out_position_interpolator()
            }
        );
        anim.add
        (
            animation::tracks::alpha_transition
            {
                .pobj = pimpl_->pgame_over_overlay,
                .finish_alpha = 0.0f,
                .duration_s = 0.5f
            }
        );
        pimpl_->animator.push(std::move(anim));

        pimpl_->pgame_over_overlay.reset();
    }
}

} //namespace
