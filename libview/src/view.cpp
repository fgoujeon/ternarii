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
#include <libutil/time.hpp>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview
{

struct view::impl final
{
    impl():
        camera_object(&scene),
        camera(camera_object)
    {
        camera.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);
        camera.setProjectionMatrix(Magnum::Matrix3::projection({9.0f, 16.0f}));
        camera.setViewport(Magnum::GL::defaultFramebuffer.viewport().size());

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
        for(std::size_t i = 0; i < feature_groups.animables.size(); ++i)
        {
            feature_groups.animables[i].advance(now);
        }

        camera.draw(feature_groups.drawables);
    }

    void set_viewport(const Magnum::Vector2i& size)
    {
        camera.setViewport(size);
    }

    void handle_key_press(key_event& event)
    {
        for(std::size_t i = 0; i < feature_groups.key_event_handlers.size(); ++i)
        {
            feature_groups.key_event_handlers[i].handle_key_press(event);
        }
    }

    void handle_mouse_press(mouse_event& event)
    {
        handle_mouse_event
        (
            event,
            [](features::clickable& clickable)
            {
                clickable.handle_mouse_press();
            }
        );
    }

    void handle_mouse_release(mouse_event& event)
    {
        handle_mouse_event
        (
            event,
            [](features::clickable& clickable)
            {
                clickable.handle_mouse_release();
            }
        );
    }

    template<class F>
    void handle_mouse_event(mouse_event& event, F&& f)
    {
        //integer window-space coordinates (with origin in top left corner and Y down)
        const auto& window_space_position = event.position();

        //convert to floating-point world-space coordinates (with origin at camera position and Y up)
        const auto world_space_position =
            (Magnum::Vector2{window_space_position} / Magnum::Vector2{Magnum::GL::defaultFramebuffer.viewport().size()} - Magnum::Vector2{0.5f})
            * Magnum::Vector2::yScale(-1.0f)
            * camera.projectionSize()
        ;

        for(std::size_t i = 0; i < feature_groups.clickables.size(); ++i)
        {
            auto& clickable = feature_groups.clickables[i];

            //convert to model-space coordinates of clickable
            const auto clickable_space_position = clickable.object().absoluteTransformationMatrix().inverted().transformPoint(world_space_position);

            //check if click position is inside clickable
            if(clickable.is_inside(clickable_space_position))
            {
                f(clickable);
            }
        }
    }

    Scene2D scene;
    Object2D camera_object;
    SceneGraph::Camera2D camera;

    feature_group_set feature_groups;
};

view::view():
    pimpl_(std::make_unique<impl>())
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

void view::handle_mouse_release(mouse_event& event)
{
    pimpl_->handle_mouse_release(event);
}

Scene2D& view::get_scene()
{
    return pimpl_->scene;
}

feature_group_set& view::get_feature_groups()
{
    return pimpl_->feature_groups;
}

} //namespace
