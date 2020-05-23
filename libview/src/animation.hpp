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

#ifndef LIBVIEW_ANIMATION_HPP
#define LIBVIEW_ANIMATION_HPP

#include <libutil/time.hpp>
#include <Magnum/Animation/Player.h>
#include <chrono>
#include <list>
#include <memory>

namespace libview
{

using player_t = Magnum::Animation::Player<std::chrono::nanoseconds, Magnum::Float>;

struct abstract_track
{
    virtual ~abstract_track(){}

    virtual void add_to_player(player_t&) = 0;
};

namespace tracks
{
    class pause: public abstract_track
    {
        private:
            using track_t = Magnum::Animation::Track<Magnum::Float, int>;

        public:
            pause(const float duration_s):
                duration_s_(duration_s)
            {
            }

            void add_to_player(player_t& p) override
            {
                track_ = track_t
                (
                    {
                        {0.0f, 0},
                        {duration_s_, 0}
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                );

                p.addWithCallback
                (
                    track_,
                    [](Magnum::Float, const int&, void*){},
                    nullptr
                );
            }

        private:
            float duration_s_ = 0;
            track_t track_;
    };

    template<class Object>
    class fixed_duration_translation: public abstract_track
    {
        private:
            using track_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;

        public:
            fixed_duration_translation
            (
                const std::shared_ptr<Object>& pobj,
                const Magnum::Vector2& finish_position,
                const float duration_s
            ):
                pobj_(pobj),
                finish_position_(finish_position),
                duration_s_(duration_s)
            {
            }

            void add_to_player(player_t& p) override
            {
                const auto& current_position = pobj_->transformation().translation();

                if(current_position == finish_position_)
                {
                    return;
                }

                track_ = track_t
                {
                    {
                        {0.0f, current_position},
                        {duration_s_, finish_position_}
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };

                p.addWithCallback
                (
                    track_,
                    [](Magnum::Float, const Magnum::Vector2& translation, Object& obj)
                    {
                        const auto current_translation = obj.transformation().translation();
                        const auto translation_delta = translation - current_translation;
                        obj.translate(translation_delta);
                    },
                    *pobj_
                );
            }

        private:
            std::shared_ptr<Object> pobj_;
            Magnum::Vector2 finish_position_;
            float duration_s_;
            track_t track_;
    };

    template<class Object>
    class fixed_speed_translation: public abstract_track
    {
        private:
            using track_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;

        public:
            fixed_speed_translation
            (
                const std::shared_ptr<Object>& pobj,
                const Magnum::Vector2& finish_position,
                const float speed //in distance unit per second
            ):
                pobj_(pobj),
                finish_position_(finish_position),
                speed_(speed)
            {
            }

            void add_to_player(player_t& p) override
            {
                const auto& current_position = pobj_->transformation().translation();

                if(current_position == finish_position_)
                {
                    return;
                }

                const auto x1 = current_position.x();
                const auto y1 = current_position.y();
                const auto x2 = finish_position_.x();
                const auto y2 = finish_position_.y();
                const auto distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
                const auto time = distance / speed_;

                track_ = track_t
                {
                    {
                        {0.0f, current_position},
                        {time, finish_position_}
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };

                p.addWithCallback
                (
                    track_,
                    [](Magnum::Float, const Magnum::Vector2& translation, Object& obj)
                    {
                        const auto current_translation = obj.transformation().translation();
                        const auto translation_delta = translation - current_translation;
                        obj.translate(translation_delta);
                    },
                    *pobj_
                );
            }

        private:
            std::shared_ptr<Object> pobj_;
            Magnum::Vector2 finish_position_;
            float speed_;
            track_t track_;
    };

    template<class Object>
    class alpha_transition: public abstract_track
    {
        private:
            using track_t = Magnum::Animation::Track<Magnum::Float, float>;

        public:
            alpha_transition
            (
                const std::shared_ptr<Object>& pobj,
                const float finish_alpha,
                const float duration_s //in seconds
            ):
                pobj_(pobj),
                finish_alpha_(finish_alpha),
                duration_s_(duration_s)
            {
            }

            void add_to_player(player_t& p) override
            {
                const auto current_alpha = pobj_->get_alpha();

                if(current_alpha == finish_alpha_)
                {
                    return;
                }

                track_ = track_t
                {
                    {
                        {0.0f, current_alpha},
                        {duration_s_, finish_alpha_}
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };

                p.addWithCallback
                (
                    track_,
                    [](Magnum::Float, const float& alpha, Object& obj)
                    {
                        obj.set_alpha(alpha);
                    },
                    *pobj_
                );
            }

        private:
            std::shared_ptr<Object> pobj_;
            float finish_alpha_;
            float duration_s_;
            track_t track_;
    };

    template<class Object>
    class immediate_alpha_transition: public abstract_track
    {
        private:
            using track_t = Magnum::Animation::Track<Magnum::Float, float>;

        public:
            immediate_alpha_transition
            (
                const std::shared_ptr<Object>& pobj,
                const float finish_alpha
            ):
                pobj_(pobj),
                finish_alpha_(finish_alpha)
            {
            }

            void add_to_player(player_t& p) override
            {
                track_ = track_t
                {
                    {
                        {0.0f, finish_alpha_},
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };

                p.addWithCallback
                (
                    track_,
                    [](Magnum::Float, const float& alpha, Object& obj)
                    {
                        obj.set_alpha(alpha);
                    },
                    *pobj_
                );
            }

        private:
            std::shared_ptr<Object> pobj_;
            float finish_alpha_;
            track_t track_;
    };
}

class animation
{
    public:
        template<class Track, class... Args>
        void add(Args&&... args)
        {
            tracks_.push_back
            (
                std::make_unique<Track>
                (
                    std::forward<Args>(args)...
                )
            );
        }

        void advance(const libutil::time_point& now)
        {
            if(!started_)
            {
                for(auto& ptrack: tracks_)
                {
                    ptrack->add_to_player(player_);
                }
                player_.play(now.time_since_epoch());
                started_ = true;
            }

            player_.advance(now.time_since_epoch());
        }

        bool is_done() const
        {
            return started_ && player_.state() == Magnum::Animation::State::Stopped;
        }

    private:
        std::vector<std::unique_ptr<abstract_track>> tracks_;
        player_t player_;
        bool started_ = false;
};

class animator
{
    public:
        void push(animation&& anim)
        {
            return animations_.push_back(std::move(anim));
        }

        bool is_animating() const
        {
            return !animations_.empty();
        }

        void advance(const libutil::time_point& now)
        {
            auto keep_advancing = true;

            while(!animations_.empty() && keep_advancing)
            {
                auto& anim = animations_.front();

                if(!anim.is_done())
                {
                    anim.advance(now);
                }

                if(anim.is_done())
                {
                    animations_.pop_front();
                    keep_advancing = true;
                }
                else
                {
                    keep_advancing = false;
                }
            }
        }

    private:
        std::list<animation> animations_;
};

} //namespace

#endif
