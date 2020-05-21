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
#include "../objects/sdf_image_button.hpp"
#include "../objects/sdf_image.hpp"
#include "../objects/tile_grid.hpp"
#include "../objects/score_display.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

struct game::impl
{
    impl
    (
        game& self,
        features::clickable_group& clickables,
        const callback_set& callbacks
    ):
        callbacks(callbacks),
        background(self, drawables),
        tile_grid(self, drawables),
        score_display(self, drawables),
        hi_score_display(self, drawables),
        exit_button   (self, drawables, clickables, "/res/images/exit.tga",          [this]{this->callbacks.handle_exit_request();}                   ),
        left_button   (self, drawables, clickables, "/res/images/move_button.tga",   [this]{send_move_request(data_types::move::left_shift);}         ),
        right_button  (self, drawables, clickables, "/res/images/move_button.tga",   [this]{send_move_request(data_types::move::right_shift);}        ),
        drop_button   (self, drawables, clickables, "/res/images/move_button.tga",   [this]{send_move_request(data_types::move::drop);}               ),
        rotate_button (self, drawables, clickables, "/res/images/rotate_button.tga", [this]{send_move_request(data_types::move::clockwise_rotation);} ),
        game_over_screen(self, drawables, clickables, [this]{this->callbacks.handle_clear_request();})
    {
        background.scale({16.0f, 16.0f});
        background.translate({0.0f, -1.0f});
        background.set_color(Magnum::Color4{1.0, 1.0, 1.0, 0.02});

        score_display.set_visible(true);
        score_display.scale({0.7f, 0.7f});
        score_display.translate({3.4f, 7.6f});

        hi_score_display.scale({0.3f, 0.3f});
        hi_score_display.translate({3.3f, 6.8f});

        tile_grid.translate({0.0f, 1.0f});

        exit_button.scale({0.5f, 0.5f});
        exit_button.translate({-2.8f, 7.0f});

        left_button.scale({0.90f, 0.90f});
        left_button.translate({-3.25f, -5.75f});

        right_button.rotate(180.0_degf);
        right_button.scale({0.90f, 0.90f});
        right_button.translate({-1.5f, -6.75f});

        drop_button.rotate(90.0_degf);
        drop_button.scale({0.90f, 0.90f});
        drop_button.translate({1.5f, -6.75f});

        rotate_button.scale({0.90f, 0.90f});
        rotate_button.translate({3.25f, -5.75f});

        game_over_screen.translate({0.0f, 4.5f});
    }

    void send_move_request(const data_types::move move)
    {
        /*
        Note: We want to ignore user inputs when we're animating, so that:
        - we don't queue too many animations;
        - user moves only when they knows what they're moving.
        */

        if(!tile_grid.is_animating())
        {
            callbacks.handle_move_request(move);
        }
    }

    callback_set callbacks;

    features::drawable_group drawables;

    bool visible = false;

    objects::background background;
    objects::tile_grid tile_grid;
    objects::score_display score_display;
    objects::score_display hi_score_display;
    objects::sdf_image_button exit_button;
    objects::sdf_image_button left_button;
    objects::sdf_image_button right_button;
    objects::sdf_image_button drop_button;
    objects::sdf_image_button rotate_button;
    objects::game_over_screen game_over_screen;
};

game::game
(
    Object2D& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    Object2D{&parent},
    features::drawable{*this, &feature_groups.drawables},
    features::animable{*this, &feature_groups.animables},
    features::key_event_handler{*this, &feature_groups.key_event_handlers},
    pimpl_(std::make_unique<impl>(*this, feature_groups.clickables, callbacks))
{
}

game::~game() = default;

void game::draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera)
{
    camera.draw(pimpl_->drawables);
}

void game::advance(const libutil::time_point& now)
{
    pimpl_->background.advance(now);
    pimpl_->tile_grid.advance(now);
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

void game::clear()
{
    pimpl_->tile_grid.clear();
    pimpl_->game_over_screen.set_visible(false);
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

void game::create_next_input(const data_types::input_tile_array& tiles)
{
    pimpl_->tile_grid.create_next_input(tiles);
}

void game::insert_next_input(const data_types::input_layout& layout)
{
    pimpl_->tile_grid.insert_next_input(layout);
}

void game::set_input_layout(const data_types::input_layout& layout)
{
    pimpl_->tile_grid.set_input_layout(layout);
}

void game::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    pimpl_->tile_grid.drop_input_tiles(drops);
}

void game::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    pimpl_->tile_grid.drop_board_tiles(drops);
}

void game::nullify_tiles(const data_types::tile_coordinate_list& nullified_tile_coordinates)
{
    pimpl_->tile_grid.nullify_tiles(nullified_tile_coordinates);
}

void game::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->tile_grid.merge_tiles(merges);
}

void game::mark_tiles_for_nullification(const data_types::tile_coordinate_list& tile_coordinates)
{
    pimpl_->tile_grid.mark_tiles_for_nullification(tile_coordinates);
}

void game::set_board_tiles(const data_types::board_tile_array& tiles)
{
    pimpl_->tile_grid.set_board_tiles(tiles);
}

void game::set_visible(const bool visible)
{
    pimpl_->visible = visible;
}

void game::set_game_over_screen_visible(const bool visible)
{
    pimpl_->game_over_screen.set_visible(visible);
}

} //namespace
