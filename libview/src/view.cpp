/*
Copyright 2018 - 2022 Florian Goujeon

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
#include "objects/label.hpp"
#include "objects/debug_grid.hpp"
#include "colors.hpp"
#include "animation.hpp"
#include "common.hpp"
#include <libutil/overload.hpp>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <chrono>
#include <map>

namespace libview
{

namespace
{
    enum class key_state
    {
        released,
        pressed
    };

    using keyboard_state = std::map<view::key, key_state>;
}

struct view::impl final
{
    impl(const configuration& conf):
        conf_(conf),
        camera_object(&scene),
        camera(camera_object)
    {
        camera.setAspectRatioPolicy(Magnum::SceneGraph::AspectRatioPolicy::Extend);
        camera.setProjectionMatrix(Magnum::Matrix3::projection({9.0f, 16.0f}));
        camera.setViewport(Magnum::GL::defaultFramebuffer.viewport().size());

        //configure renderer
        Magnum::GL::Renderer::setClearColor(colors::dark_gray);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
        Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::One, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
        Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);

        if(conf.show_fps_counter)
        {
            pfps_counter = std::make_unique<objects::label>
            (
                camera_object,
                feature_groups.drawables,
                objects::label::style
                {
                    .alignment = Magnum::Text::Alignment::MiddleLeft,
                    .color = colors::white,
                    .font_size = 0.3f
                }
            );
            pfps_counter->translate({-4.0f, -7.0f});
        }

        if(conf.show_debug_grid)
        {
            pdebug_grid = std::make_unique<objects::debug_grid>(camera_object, feature_groups.drawables);
        }
    }

    void advance(const std::chrono::steady_clock::time_point& now, float elapsed_s)
    {
        //Advance animations
        screen_transition_animator.advance(now);
        for(std::size_t i = 0; i < feature_groups.animables.size(); ++i)
        {
            feature_groups.animables[i].advance(now, elapsed_s);
        }

        //Update FPS counter
        if(conf_.show_fps_counter)
        {
            const auto measure_duration_s = std::chrono::duration<double>{now - fps_measure_start_time}.count();
            if(measure_duration_s > 0.5)
            {
                const auto fps = static_cast<int>(std::round(fps_measure_count / measure_duration_s));
                pfps_counter->set_text(std::to_string(fps).c_str());

                //reset
                fps_measure_start_time = now;
                fps_measure_count = 0;
            }
            else
            {
                ++fps_measure_count;
            }
        }
    }

    void draw()
    {
        camera.draw(feature_groups.drawables);
    }

    void set_viewport(const Magnum::Vector2i& size)
    {
        camera.setViewport(size);
    }

    void handle_key_press(key_event& event)
    {
        if(screen_transition_animator.is_animating())
        {
            return;
        }

        //Filter autorepeat
        if(key_states[event.key()] == key_state::pressed)
        {
            return;
        }
        key_states[event.key()] = key_state::pressed;

        //Forward event to handlers
        for(std::size_t i = 0; i < feature_groups.key_event_handlers.size(); ++i)
        {
            feature_groups.key_event_handlers[i].handle_key_press(event);
        }
    }

    void handle_key_release(key_event& event)
    {
        if(screen_transition_animator.is_animating())
        {
            return;
        }

        key_states[event.key()] = key_state::released;

        for(std::size_t i = 0; i < feature_groups.key_event_handlers.size(); ++i)
        {
            feature_groups.key_event_handlers[i].handle_key_release(event);
        }
    }

    void handle_mouse_press(mouse_event& event)
    {
        //Only manage left button
        if(event.button() != mouse_event::Button::Left)
        {
            return;
        }

        if(screen_transition_animator.is_animating())
        {
            return;
        }

        handle_mouse_event
        (
            event,
            [](features::clickable& clickable, const Magnum::Vector2& clickable_space_position)
            {
                clickable.handle_mouse_press(clickable_space_position);
            }
        );
    }

    void handle_mouse_release(mouse_event& event)
    {
        //Only manage left button
        if(event.button() != mouse_event::Button::Left)
        {
            return;
        }

        if(screen_transition_animator.is_animating())
        {
            return;
        }

        handle_mouse_event
        (
            event,
            [](features::clickable& clickable, const Magnum::Vector2& clickable_space_position)
            {
                clickable.handle_mouse_release(clickable_space_position);
            }
        );
    }

    void handle_mouse_move(mouse_move_event& event)
    {
        if(screen_transition_animator.is_animating())
        {
            return;
        }

        handle_mouse_event
        (
            event,
            [](features::clickable& clickable, const Magnum::Vector2& clickable_space_position)
            {
                clickable.handle_mouse_move(clickable_space_position);
            }
        );
    }

    template<class Event, class F>
    void handle_mouse_event(Event& event, F&& f)
    {
        //Integer window-space coordinates (with origin in top left corner and Y down)
        const auto& window_space_position = event.position();

        //Convert to floating-point world-space coordinates (with origin at camera position and Y up)
        const auto world_space_position =
            (Magnum::Vector2{window_space_position} / Magnum::Vector2{Magnum::GL::defaultFramebuffer.viewport().size()} - Magnum::Vector2{0.5f})
            * Magnum::Vector2::yScale(-1.0f)
            * camera.projectionSize()
        ;

        for(std::size_t i = 0; i < feature_groups.clickables.size(); ++i)
        {
            auto& clickable = feature_groups.clickables[i];

            //Convert to model-space coordinates of clickable
            const auto clickable_space_position = clickable.object().absoluteTransformationMatrix().inverted().transformPoint(world_space_position);

            f(clickable, clickable_space_position);
        }
    }

    configuration conf_;

    scene scene;
    object2d camera_object;
    camera camera;

    feature_group_set feature_groups;

    animation::animator screen_transition_animator;

    keyboard_state key_states;

    std::shared_ptr<object2d> pscreen;
    std::unique_ptr<objects::debug_grid> pdebug_grid;
    std::unique_ptr<objects::label> pfps_counter;

    std::chrono::steady_clock::time_point fps_measure_start_time = std::chrono::steady_clock::now();
    int fps_measure_count = 0;
};

view::view(const configuration& conf):
    pimpl_(std::make_unique<impl>(conf))
{
}

view::~view() = default;

void view::show_screen
(
    const std::shared_ptr<object2d>& pscreen,
    const screen_transition& trans
)
{
    const auto vector_interpolator = Magnum::Animation::ease
    <
        Magnum::Vector2,
        Magnum::Math::lerp,
        Magnum::Animation::Easing::exponentialOut
    >();

    const auto float_interpolator = Magnum::Animation::ease
    <
        Magnum::Float,
        Magnum::Math::lerp,
        Magnum::Animation::Easing::exponentialOut
    >();

    const auto do_translation_transition = [&]
    (
        const float duration_s,
        const Magnum::Vector2& new_screen_start_position
    )
    {
        const auto old_screen_finish_position = Magnum::Vector2
        {
            -new_screen_start_position.x(),
            -new_screen_start_position.y()
        };

        pimpl_->screen_transition_animator.push
        (
            animation::tracks::fixed_duration_translation
            {
                pscreen,
                new_screen_start_position,
                0
            }
        );

        auto anim = animation::animation{};

        //Hide old screen, if any
        if(pimpl_->pscreen)
        {
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pimpl_->pscreen,
                    .finish_position = old_screen_finish_position,
                    .duration_s = duration_s,
                    .interpolator = vector_interpolator
                }
            );

            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pimpl_->pscreen,
                    .finish_alpha = 0.0f,
                    .duration_s = duration_s,
                    .interpolator = float_interpolator
                }
            );
        }

        //Show new screen
        {
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pscreen,
                    .finish_position = {0.0f, 0.0f},
                    .duration_s = duration_s,
                    .interpolator = vector_interpolator
                }
            );

            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pscreen,
                    .finish_alpha = 1.0f,
                    .duration_s = duration_s,
                    .interpolator = float_interpolator
                }
            );
        }

        pimpl_->screen_transition_animator.push(std::move(anim));
    };

    const auto do_zoom_transition = [&]
    (
        const float duration_s,
        const Magnum::Vector2& new_screen_start_position,
        const float new_screen_start_scaling
    )
    {
        const auto old_screen_finish_position = Magnum::Vector2
        {
            -new_screen_start_position.x() / new_screen_start_scaling,
            -new_screen_start_position.y() / new_screen_start_scaling
        };

        const auto old_screen_finish_scaling = Magnum::Vector2
        {
            1.0f / new_screen_start_scaling,
            1.0f / new_screen_start_scaling
        };

        pimpl_->screen_transition_animator.push
        (
            animation::tracks::fixed_duration_translation
            {
                .pobj = pscreen,
                .finish_position = new_screen_start_position,
                .duration_s = 0
            }
        );

        pimpl_->screen_transition_animator.push
        (
            animation::tracks::scaling_transition
            {
                .pobj = pscreen,
                .finish_scaling = Magnum::Vector2
                {
                    new_screen_start_scaling,
                    new_screen_start_scaling
                },
                .duration_s = 0
            }
        );

        auto anim = animation::animation{};

        //Hide old screen, if any
        if(pimpl_->pscreen)
        {
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pimpl_->pscreen,
                    .finish_position = old_screen_finish_position,
                    .duration_s = duration_s,
                    .interpolator = vector_interpolator
                }
            );

            anim.add
            (
                animation::tracks::scaling_transition
                {
                    .pobj = pimpl_->pscreen,
                    .finish_scaling = old_screen_finish_scaling,
                    .duration_s = duration_s,
                    .interpolator = vector_interpolator
                }
            );

            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pimpl_->pscreen,
                    .finish_alpha = 0.0f,
                    .duration_s = duration_s,
                    .interpolator = float_interpolator
                }
            );
        }

        //Show new screen
        {
            anim.add
            (
                animation::tracks::fixed_duration_translation
                {
                    .pobj = pscreen,
                    .finish_position = {0.0f, 0.0f},
                    .duration_s = duration_s,
                    .interpolator = vector_interpolator
                }
            );

            anim.add
            (
                animation::tracks::scaling_transition
                {
                    .pobj = pscreen,
                    .finish_scaling = Magnum::Vector2{1.0f, 1.0f},
                    .duration_s = duration_s,
                    .interpolator = vector_interpolator
                }
            );

            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pscreen,
                    .finish_alpha = 1.0f,
                    .duration_s = duration_s,
                    .interpolator = float_interpolator
                }
            );
        }

        pimpl_->screen_transition_animator.push(std::move(anim));
    };

    std::visit
    (
        libutil::overload
        {
            [&](const screen_transitions::top_to_bottom)
            {
                do_translation_transition(1.0f, Magnum::Vector2{0.0f, 4.0f});
            },
            [&](const screen_transitions::right_to_left)
            {
                do_translation_transition(0.5f, Magnum::Vector2{12.0f, 0.0f});
            },
            [&](const screen_transitions::left_to_right)
            {
                do_translation_transition(0.5f, Magnum::Vector2{-12.0f, 0.0f});
            },
            [&](const screen_transitions::zoom_in& trans)
            {
                do_zoom_transition
                (
                    trans.duration_s,
                    trans.new_screen_start_position,
                    trans.new_screen_start_scaling
                );
            },
            [&](const screen_transitions::zoom_out& trans)
            {
                do_zoom_transition
                (
                    trans.duration_s,
                    trans.new_screen_start_position,
                    trans.new_screen_start_scaling
                );
            }
        },
        trans
    );

    //Save new screen for future transition
    pimpl_->pscreen = pscreen;
}

void view::advance
(
    const std::chrono::steady_clock::time_point& now,
    float elapsed_s
)
{
    pimpl_->advance(now, elapsed_s);
}

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

void view::handle_key_release(key_event& event)
{
    pimpl_->handle_key_release(event);
}

void view::handle_mouse_press(mouse_event& event)
{
    pimpl_->handle_mouse_press(event);
}

void view::handle_mouse_release(mouse_event& event)
{
    pimpl_->handle_mouse_release(event);
}

void view::handle_mouse_move(mouse_move_event& event)
{
    pimpl_->handle_mouse_move(event);
}

scene& view::get_scene()
{
    return pimpl_->scene;
}

feature_group_set& view::get_feature_groups()
{
    return pimpl_->feature_groups;
}

} //namespace
