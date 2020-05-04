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

#include "background.hpp"
#include "game_over_screen.hpp"
#include "button.hpp"
#include "sdf_image.hpp"
#include "tile_grid.hpp"
#include "score_display.hpp"
#include "colors.hpp"
#include "clickable.hpp"
#include "time.hpp"
#include "magnum_common.hpp"
#include <libview/view.hpp>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview
{

class view::impl final
{
    public:
        impl(const callback_set& callbacks):
            callbacks_(callbacks),
            cameraObject_(&scene_),
            camera_(cameraObject_),
            background_(scene_.addChild<background>(drawables_)),
            tile_grid_(scene_.addChild<tile_grid>(drawables_)),
            score_display_(scene_.addChild<score_display>(drawables_)),
            hi_score_display_(scene_.addChild<score_display>(drawables_)),
            left_button_   (scene_.addChild<button>("/res/images/move_button.tga",   [this]{send_move_request(data_types::move::left_shift);},         drawables_, clickables_)),
            right_button_  (scene_.addChild<button>("/res/images/move_button.tga",   [this]{send_move_request(data_types::move::right_shift);},        drawables_, clickables_)),
            drop_button_   (scene_.addChild<button>("/res/images/move_button.tga",   [this]{send_move_request(data_types::move::drop);},               drawables_, clickables_)),
            rotate_button_ (scene_.addChild<button>("/res/images/rotate_button.tga", [this]{send_move_request(data_types::move::clockwise_rotation);}, drawables_, clickables_)),
            game_over_screen_(scene_.addChild<game_over_screen>([this]{callbacks_.handle_clear_request();}, drawables_, clickables_))
        {
            camera_.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
            camera_.setProjectionMatrix(Magnum::Matrix3::projection({9.0f, 16.0f}));
            camera_.setViewport(Magnum::GL::defaultFramebuffer.viewport().size());

            background_.scale({16.0f, 16.0f});
            background_.translate({0.0f, -1.0f});
            background_.set_color(Magnum::Color4{1.0, 1.0, 1.0, 0.02});

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

            //configure renderer
            using namespace Magnum::Math::Literals;
            Magnum::GL::Renderer::setClearColor(colors::dark_gray);
            Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
            Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
            Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
        }

        void draw()
        {
            const auto now = clock::now();

            //advance animations
            background_.advance(now);
            tile_grid_.advance(now);

            if(visible_)
            {
                camera_.draw(drawables_);
            }
        }

        void set_viewport(const Magnum::Vector2i& size)
        {
            camera_.setViewport(size);
        }

        void handle_key_press(key_event& event)
        {
            switch(event.key())
            {
                case key_event::Key::Left:
                    send_move_request(data_types::move::left_shift);
                    break;
                case key_event::Key::Right:
                    send_move_request(data_types::move::right_shift);
                    break;
                case key_event::Key::Up:
                case key_event::Key::Space:
                    send_move_request(data_types::move::clockwise_rotation);
                    break;
                case key_event::Key::Down:
                    send_move_request(data_types::move::drop);
                    break;
                default:
                    break;
            }
        }

        void handle_mouse_press(mouse_event& event)
        {
            //integer window-space coordinates (with origin in top left corner and Y down)
            const auto& window_space_position = event.position();

            //convert to floating-point world-space coordinates (with origin at camera position and Y up)
            const auto world_space_position =
                (Magnum::Vector2{window_space_position} / Magnum::Vector2{Magnum::GL::defaultFramebuffer.viewport().size()} - Magnum::Vector2{0.5f})
                * Magnum::Vector2::yScale(-1.0f)
                * camera_.projectionSize()
            ;

            for(std::size_t i = 0; i < clickables_.size(); ++i)
            {
                auto& button = clickables_[i];

                //convert to model-space coordinates of button
                const auto button_space_position = button.object().absoluteTransformationMatrix().inverted().transformPoint(world_space_position);

                //check if click position is inside button
                if(button.is_inside(button_space_position))
                {
                    button.mouse_press_event();
                }
            }
        }

    private:
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

    private:
        callback_set callbacks_;

        Scene2D scene_;
        Object2D cameraObject_;
        SceneGraph::Camera2D camera_;
        SceneGraph::DrawableGroup2D drawables_;
        clickable_group clickables_;

    public:
        bool visible_ = false;

        background& background_;
        tile_grid& tile_grid_;
        score_display& score_display_;
        score_display& hi_score_display_;
        button& left_button_;
        button& right_button_;
        button& drop_button_;
        button& rotate_button_;
        game_over_screen& game_over_screen_;
};

view::view(const callback_set& callbacks):
    pimpl_(std::make_unique<impl>(callbacks))
{
}

view::~view() = default;

void view::draw()
{
    pimpl_->draw();
}

void view::set_viewport(const Magnum::Vector2i& size)
{
    pimpl_->set_viewport(size);
}

void view::handle_key_press(key_event& event)
{
    pimpl_->handle_key_press(event);
}

void view::handle_mouse_press(mouse_event& event)
{
    pimpl_->handle_mouse_press(event);
}

void view::clear()
{
    pimpl_->tile_grid_.clear();
    pimpl_->game_over_screen_.set_visible(false);
}

void view::set_score(const int value)
{
    pimpl_->score_display_.set_score(value);
}

void view::set_hi_score(const int value)
{
    pimpl_->hi_score_display_.set_score(value);
}

void view::create_next_input(const int value0, const int value1)
{
    pimpl_->tile_grid_.create_next_input(value0, value1);
}

void view::insert_next_input(const data_types::input_layout& layout)
{
    pimpl_->tile_grid_.insert_next_input(layout);
}

void view::set_input_layout(const data_types::input_layout& layout)
{
    pimpl_->tile_grid_.set_input_layout(layout);
}

void view::insert_input(const data_types::tile_coordinate_list& dst_coordinates)
{
    pimpl_->tile_grid_.insert_input(dst_coordinates);
}

void view::drop_tiles(const data_types::tile_drop_list& drops)
{
    pimpl_->tile_grid_.drop_tiles(drops);
}

void view::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->tile_grid_.merge_tiles(merges);
}

void view::set_board_tiles(const data_types::board_tile_array& tiles)
{
    pimpl_->tile_grid_.set_board_tiles(tiles);
}

void view::set_visible(const bool visible)
{
    pimpl_->visible_ = visible;
}

void view::set_game_over_screen_visible(const bool visible)
{
    pimpl_->game_over_screen_.set_visible(visible);
}

} //namespace
