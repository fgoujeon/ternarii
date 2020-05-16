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
#include "../objects/background.hpp"
#include "../objects/game_over_screen.hpp"
#include "../objects/button.hpp"
#include "../objects/sdf_image.hpp"
#include "../objects/tile_grid.hpp"
#include "../objects/score_display.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libview/events.hpp>
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

class game::impl
{
    public:
        impl
        (
            const object_arg_set& args,
            const callback_set& callbacks
        ):
            callbacks_(callbacks),
            background_(drawables_, args.parent),
            tile_grid_(drawables_, args.parent),
            score_display_(drawables_, args.parent),
            hi_score_display_(drawables_, args.parent),
            left_button_   ("/res/images/move_button.tga",   [this]{send_move_request(data_types::move::left_shift);},         drawables_, args.clickables, args.parent),
            right_button_  ("/res/images/move_button.tga",   [this]{send_move_request(data_types::move::right_shift);},        drawables_, args.clickables, args.parent),
            drop_button_   ("/res/images/move_button.tga",   [this]{send_move_request(data_types::move::drop);},               drawables_, args.clickables, args.parent),
            rotate_button_ ("/res/images/rotate_button.tga", [this]{send_move_request(data_types::move::clockwise_rotation);}, drawables_, args.clickables, args.parent),
            game_over_screen_([this]{callbacks_.handle_clear_request();}, drawables_, args.clickables, args.parent)
        {
            background_.scale({16.0f, 16.0f});
            background_.translate({0.0f, -1.0f});
            background_.set_color(Magnum::Color4{1.0, 1.0, 1.0, 0.02});

            score_display_.set_visible(true);
            score_display_.scale({0.7f, 0.7f});
            score_display_.translate({3.1f, 7.6f});

            hi_score_display_.scale({0.3f, 0.3f});
            hi_score_display_.translate({3.0f, 6.8f});

            tile_grid_.translate({0.0f, 1.0f});

            left_button_.scale({0.90f, 0.90f});
            left_button_.translate({-3.25f, -5.75f});

            right_button_.rotate(180.0_degf);
            right_button_.scale({0.90f, 0.90f});
            right_button_.translate({-1.5f, -6.75f});

            drop_button_.rotate(90.0_degf);
            drop_button_.scale({0.90f, 0.90f});
            drop_button_.translate({1.5f, -6.75f});

            rotate_button_.scale({0.90f, 0.90f});
            rotate_button_.translate({3.25f, -5.75f});

            game_over_screen_.translate({0.0f, 4.5f});
        }

        void send_move_request(const data_types::move move)
        {
            /*
            Note: We want to ignore user inputs when we're animating, so that:
            - we don't queue too many animations;
            - user moves only when they knows what they're moving.
            */

            if(!tile_grid_.is_animating())
            {
                callbacks_.handle_move_request(move);
            }
        }

    public:
        callback_set callbacks_;

        SceneGraph::DrawableGroup2D drawables_;

        bool visible_ = false;

        objects::background background_;
        objects::tile_grid tile_grid_;
        objects::score_display score_display_;
        objects::score_display hi_score_display_;
        objects::button left_button_;
        objects::button right_button_;
        objects::button drop_button_;
        objects::button rotate_button_;
        objects::game_over_screen game_over_screen_;
};

game::game
(
    const object_arg_set& args,
    const callback_set& callbacks
):
    Object2D{&args.parent},
    SceneGraph::Drawable2D{*this, &args.drawables},
    pimpl_(std::make_unique<impl>(args, callbacks))
{
}

game::~game() = default;

void game::advance(const libutil::time_point& now)
{
    pimpl_->background_.advance(now);
    pimpl_->tile_grid_.advance(now);
}

void game::handle_key_press(key_event& event)
{
    switch(event.key())
    {
        case key_event::Key::Left:
            pimpl_->send_move_request(data_types::move::left_shift);
            break;
        case key_event::Key::Right:
            pimpl_->send_move_request(data_types::move::right_shift);
            break;
        case key_event::Key::Up:
        case key_event::Key::Space:
            pimpl_->send_move_request(data_types::move::clockwise_rotation);
            break;
        case key_event::Key::Down:
            pimpl_->send_move_request(data_types::move::drop);
            break;
        default:
            break;
    }
}

void game::draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera)
{
    camera.draw(pimpl_->drawables_);
}

void game::clear()
{
    pimpl_->tile_grid_.clear();
    pimpl_->game_over_screen_.set_visible(false);
}

void game::set_score(const int value)
{
    pimpl_->score_display_.set_score(value);
}

void game::set_hi_score(const int value)
{
    if(value != 0)
    {
        pimpl_->hi_score_display_.set_score(value);
        pimpl_->hi_score_display_.set_visible(true);
    }
}

void game::create_next_input(const data_types::input_tile_array& tiles)
{
    pimpl_->tile_grid_.create_next_input(tiles);
}

void game::insert_next_input(const data_types::input_layout& layout)
{
    pimpl_->tile_grid_.insert_next_input(layout);
}

void game::set_input_layout(const data_types::input_layout& layout)
{
    pimpl_->tile_grid_.set_input_layout(layout);
}

void game::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    pimpl_->tile_grid_.drop_input_tiles(drops);
}

void game::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    pimpl_->tile_grid_.drop_board_tiles(drops);
}

void game::nullify_tiles(const data_types::tile_coordinate_list& nullified_tile_coordinates)
{
    pimpl_->tile_grid_.nullify_tiles(nullified_tile_coordinates);
}

void game::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->tile_grid_.merge_tiles(merges);
}

void game::mark_tiles_for_nullification(const data_types::tile_coordinate_list& tile_coordinates)
{
    pimpl_->tile_grid_.mark_tiles_for_nullification(tile_coordinates);
}

void game::set_board_tiles(const data_types::board_tile_array& tiles)
{
    pimpl_->tile_grid_.set_board_tiles(tiles);
}

void game::set_visible(const bool visible)
{
    pimpl_->visible_ = visible;
}

void game::set_game_over_screen_visible(const bool visible)
{
    pimpl_->game_over_screen_.set_visible(visible);
}

} //namespace
