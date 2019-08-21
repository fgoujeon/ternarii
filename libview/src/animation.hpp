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

#ifndef LIBVIEW_ANIMATION_HPP
#define LIBVIEW_ANIMATION_HPP

#include <Magnum/Animation/Player.h>
#include <chrono>
#include <list>

namespace libview
{

class animation
{
    public:
        ~animation()
        {
            if(cleanup_callback_)
            {
                cleanup_callback_();
            }
        }

        template<class Object>
        void add_fixed_duration_translation
        (
            const Magnum::Vector2& start_position,
            const Magnum::Vector2& finish_position,
            const float duration_s,
            Object& object
        )
        {
            auto track = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>
            {
                {
                    {0.0f, start_position},
                    {duration_s, finish_position}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            translations_.push_back(std::move(track));

            player.addWithCallback
            (
                translations_.back(),
                [](Magnum::Float, const Magnum::Vector2& translation, Object& obj)
                {
                    obj.resetTransformation();
                    obj.translate(translation);
                },
                object
            );
        }

        template<class Object>
        void add_fixed_speed_translation
        (
            const Magnum::Vector2& start_position,
            const Magnum::Vector2& finish_position,
            const float speed, //in distance unit per second
            Object& object
        )
        {
            const auto x1 = start_position.x();
            const auto y1 = start_position.y();
            const auto x2 = finish_position.x();
            const auto y2 = finish_position.y();
            const auto distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
            const auto time = distance / speed;

            auto track = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>
            {
                {
                    {0.0f, start_position},
                    {time, finish_position}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            translations_.push_back(std::move(track));

            player.addWithCallback
            (
                translations_.back(),
                [](Magnum::Float, const Magnum::Vector2& translation, Object& obj)
                {
                    obj.resetTransformation();
                    obj.translate(translation);
                },
                object
            );
        }

        template<class Object>
        void add_alpha_transition
        (
            const float start_alpha,
            const float finish_alpha,
            const float duration, //in seconds
            Object& object
        )
        {
            auto track = Magnum::Animation::Track<Magnum::Float, float>
            {
                {
                    {0.0f, start_alpha},
                    {duration, finish_alpha}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            alpha_transitions_.push_back(std::move(track));

            player.addWithCallback
            (
                alpha_transitions_.back(),
                [](Magnum::Float, const float& alpha, Object& obj)
                {
                    obj.set_alpha(alpha);
                },
                object
            );
        }

        void advance()
        {
            if(!started_)
            {
                player.play(std::chrono::system_clock::now().time_since_epoch());
                started_ = true;
            }

            player.advance(std::chrono::system_clock::now().time_since_epoch());
        }

        bool is_done() const
        {
            return started_ && player.state() == Magnum::Animation::State::Stopped;
        }

        template<class Callback>
        void set_cleanup_callback(Callback&& callback)
        {
            cleanup_callback_ = std::forward<Callback>(callback);
        }

        Magnum::Animation::Player<std::chrono::nanoseconds, Magnum::Float> player;

    private:
        //for storage only
        std::list<Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>> translations_;
        std::list<Magnum::Animation::Track<Magnum::Float, float>> alpha_transitions_;

        bool started_ = false;
        std::function<void()> cleanup_callback_;
};

using animation_list = std::list<animation>;

} //namespace

#endif
