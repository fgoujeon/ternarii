/*
Copyright 2018 - 2019 Florian Goujeon

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

class view::impl final: public Magnum::Platform::Sdl2Application
{
    public:
        impl(int argc, char** argv, const event_handler& evt_handler):
            Magnum::Platform::Sdl2Application
            {
                Arguments{argc, argv},
                Configuration{}.setWindowFlags(Configuration::WindowFlag::Resizable)
            },
            event_handler_(evt_handler),
            cameraObject_(&scene_),
            camera_(cameraObject_),
            background_(scene_.addChild<background>(drawables_)),
            tile_grid_(scene_.addChild<tile_grid>(drawables_)),
            score_display_(scene_.addChild<score_display>(drawables_)),
            hi_score_display_(scene_.addChild<score_display>(drawables_)),
            left_button_   (scene_.addChild<button>("LEFT",   [this]{send_move_request(events::left_shift_request{});},         drawables_, clickables_)),
            right_button_  (scene_.addChild<button>("RIGHT",  [this]{send_move_request(events::right_shift_request{});},        drawables_, clickables_)),
            drop_button_   (scene_.addChild<button>("DROP",   [this]{send_move_request(events::drop_request{});},               drawables_, clickables_)),
            rotate_button_ (scene_.addChild<button>("ROTATE", [this]{send_move_request(events::clockwise_rotation_request{});}, drawables_, clickables_)),
            game_over_screen_(scene_.addChild<game_over_screen>([this]{send_move_request(events::clear_request{});}, drawables_, clickables_))
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

            left_button_.scale({0.85f, 0.85f});
            left_button_.translate({-3.25f, -5.75f});

            right_button_.scale({0.85f, 0.85f});
            right_button_.translate({-1.5f, -6.75f});

            drop_button_.scale({0.85f, 0.85f});
            drop_button_.translate({1.5f, -6.75f});

            rotate_button_.scale({0.85f, 0.85f});
            rotate_button_.translate({3.25f, -5.75f});

            game_over_screen_.translate({0.0f, 4.5f});

            //configure renderer
            using namespace Magnum::Math::Literals;
            Magnum::GL::Renderer::setClearColor(colors::dark_gray);
            Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
            Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
            Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
        }

    private:
        void drawEvent()
        {
            const auto now = std::chrono::steady_clock::now();

            event_handler_(events::draw{});

            //advance animations
            background_.advance(now);
            tile_grid_.advance(now);

            Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

            if(visible_)
            {
                camera_.draw(drawables_);
            }

            swapBuffers();
            redraw();
        }

        void viewportEvent(ViewportEvent& event)
        {
            Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
            camera_.setViewport(event.windowSize());
        }

        void keyPressEvent(KeyEvent& event)
        {
            switch(event.key())
            {
                case KeyEvent::Key::Left:
                    send_move_request(events::left_shift_request{});
                    break;
                case KeyEvent::Key::Right:
                    send_move_request(events::right_shift_request{});
                    break;
                case KeyEvent::Key::Up:
                case KeyEvent::Key::Space:
                    send_move_request(events::clockwise_rotation_request{});
                    break;
                case KeyEvent::Key::Down:
                    send_move_request(events::drop_request{});
                    break;
                default:
                    break;
            }
        }

        void mousePressEvent(MouseEvent& event)
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
        template<class Event>
        void send_move_request(Event&& event)
        {
            /*
            Note: We want to ignore user inputs when we're animating, so that:
            - we don't queue too many animations;
            - user moves only when they knows what they're moving.
            */

            if(!tile_grid_.is_animating())
                event_handler_(std::forward<Event>(event));
        }

    private:
        event_handler event_handler_;

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

view::view(int argc, char** argv, const event_handler& evt_handler):
    pimpl_(std::make_unique<impl>(argc, argv, evt_handler))
{
}

view::~view() = default;

int view::exec()
{
    return pimpl_->exec();
}

void view::clear()
{
    pimpl_->tile_grid_.clear();
    pimpl_->game_over_screen_.set_visible(false);
}

void view::set_score(const unsigned int value)
{
    pimpl_->score_display_.set_score(value);
}

void view::set_hi_score(const unsigned int value)
{
    pimpl_->hi_score_display_.set_score(value);
}

void view::create_next_input(const unsigned int value0, const unsigned int value1)
{
    pimpl_->tile_grid_.create_next_input(value0, value1);
}

void view::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    pimpl_->tile_grid_.insert_next_input(x_offset, rotation);
}

void view::set_input_layout(const unsigned int x_offset, const unsigned int rotation)
{
    pimpl_->tile_grid_.set_input_layout(x_offset, rotation);
}

void view::insert_input
(
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    pimpl_->tile_grid_.insert_input
    (
        tile0_dst_column_index,
        tile0_dst_row_index,
        tile1_dst_column_index,
        tile1_dst_row_index
    );
}

void view::drop_tiles(const data_types::tile_drop_list& drops)
{
    pimpl_->tile_grid_.drop_tiles(drops);
}

void view::merge_tiles(const data_types::tile_merge_list& merges)
{
    pimpl_->tile_grid_.merge_tiles(merges);
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
