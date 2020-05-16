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

#include <libview/view.hpp>
#include "colors.hpp"
#include "common.hpp"
#include <libview/screens/game.hpp>
#include <libutil/time.hpp>
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
            cameraObject_(&scene_),
            camera_(cameraObject_),
            game_screen_(scene_, drawables_, animables_, clickables_, key_event_handlers_, callbacks)
        {
            camera_.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
            camera_.setProjectionMatrix(Magnum::Matrix3::projection({9.0f, 16.0f}));
            camera_.setViewport(Magnum::GL::defaultFramebuffer.viewport().size());

            //configure renderer
            Magnum::GL::Renderer::setClearColor(colors::dark_gray);
            Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
            Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
            Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
        }

        void draw()
        {
            const auto now = libutil::clock::now();

            //advance animations
            for(std::size_t i = 0; i < animables_.size(); ++i)
            {
                animables_[i].advance(now);
            }

            camera_.draw(drawables_);
        }

        void set_viewport(const Magnum::Vector2i& size)
        {
            camera_.setViewport(size);
        }

        void handle_key_press(key_event& event)
        {
            for(std::size_t i = 0; i < key_event_handlers_.size(); ++i)
            {
                key_event_handlers_[i].handle_key_press(event);
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
        Scene2D scene_;
        Object2D cameraObject_;
        SceneGraph::Camera2D camera_;
        features::drawable_group drawables_;
        features::animable_group animables_;
        features::clickable_group clickables_;
        features::key_event_handler_group key_event_handlers_;

    public:
        screens::game game_screen_;
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

screens::game& view::get_game_screen()
{
    return pimpl_->game_screen_;
}

} //namespace
