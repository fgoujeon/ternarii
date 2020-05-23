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
    struct pause
    {
        float duration_s = 0;
    };

    class pause_impl: public abstract_track
    {
        private:
            using desc_t = pause;
            using track_t = Magnum::Animation::Track<Magnum::Float, int>;

        public:
            pause_impl(const desc_t& desc):
                desc_(desc)
            {
            }

            void add_to_player(player_t& p) override
            {
                track_ = track_t
                (
                    {
                        {0.0f, 0},
                        {desc_.duration_s, 0}
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
            desc_t desc_;
            track_t track_;
    };

    inline
    std::unique_ptr<abstract_track> make_impl(const pause& desc)
    {
        return std::make_unique<pause_impl>(desc);
    }



    template<class Object>
    struct fixed_duration_translation
    {
        std::shared_ptr<Object> pobj;
        Magnum::Vector2 finish_position;
        float duration_s = 0; //in seconds
    };

    template<class Object>
    class fixed_duration_translation_impl: public abstract_track
    {
        private:
            using desc_t = fixed_duration_translation<Object>;
            using track_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;

        public:
            fixed_duration_translation_impl(const desc_t& desc):
                desc_(desc)
            {
            }

            void add_to_player(player_t& p) override
            {
                const auto& current_position = desc_.pobj->transformation().translation();

                if(current_position == desc_.finish_position)
                {
                    return;
                }

                track_ = track_t
                {
                    {
                        {0.0f, current_position},
                        {desc_.duration_s, desc_.finish_position}
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
                    *desc_.pobj
                );
            }

        private:
            desc_t desc_;
            track_t track_;
    };

    template<class Object>
    std::unique_ptr<abstract_track> make_impl(const fixed_duration_translation<Object>& desc)
    {
        return std::make_unique<fixed_duration_translation_impl<Object>>(desc);
    }



    template<class Object>
    struct fixed_speed_translation
    {
        std::shared_ptr<Object> pobj;
        Magnum::Vector2 finish_position;
        float speed = 0; //in distance unit per second
    };

    template<class Object>
    class fixed_speed_translation_impl: public abstract_track
    {
        private:
            using desc_t = fixed_speed_translation<Object>;
            using track_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;

        public:
            fixed_speed_translation_impl(const desc_t& desc):
                desc_(desc)
            {
            }

            void add_to_player(player_t& p) override
            {
                const auto& current_position = desc_.pobj->transformation().translation();

                if(current_position == desc_.finish_position)
                {
                    return;
                }

                const auto x1 = current_position.x();
                const auto y1 = current_position.y();
                const auto x2 = desc_.finish_position.x();
                const auto y2 = desc_.finish_position.y();
                const auto distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
                const auto time = distance / desc_.speed;

                track_ = track_t
                {
                    {
                        {0.0f, current_position},
                        {time, desc_.finish_position}
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
                    *desc_.pobj
                );
            }

        private:
            desc_t desc_;
            track_t track_;
    };

    template<class Object>
    std::unique_ptr<abstract_track> make_impl(const fixed_speed_translation<Object>& desc)
    {
        return std::make_unique<fixed_speed_translation_impl<Object>>(desc);
    }



    template<class Object>
    struct alpha_transition
    {
        std::shared_ptr<Object> pobj;
        float finish_alpha = 0;
        float duration_s = 0; //in seconds
    };

    template<class Object>
    class alpha_transition_impl: public abstract_track
    {
        private:
            using desc_t = alpha_transition<Object>;
            using track_t = Magnum::Animation::Track<Magnum::Float, float>;

        public:
            alpha_transition_impl(const desc_t& desc):
                desc_(desc)
            {
            }

            void add_to_player(player_t& p) override
            {
                const auto current_alpha = desc_.pobj->get_alpha();

                if(current_alpha == desc_.finish_alpha)
                {
                    return;
                }

                track_ = track_t
                {
                    {
                        {0.0f, current_alpha},
                        {desc_.duration_s, desc_.finish_alpha}
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
                    *desc_.pobj
                );
            }

        private:
            desc_t desc_;
            track_t track_;
    };

    template<class Object>
    std::unique_ptr<abstract_track> make_impl(const alpha_transition<Object>& desc)
    {
        return std::make_unique<alpha_transition_impl<Object>>(desc);
    }



    template<class Object>
    struct immediate_alpha_transition
    {
        std::shared_ptr<Object> pobj;
        float finish_alpha = 0;
    };

    template<class Object>
    class immediate_alpha_transition_impl: public abstract_track
    {
        private:
            using desc_t = immediate_alpha_transition<Object>;
            using track_t = Magnum::Animation::Track<Magnum::Float, float>;

        public:
            immediate_alpha_transition_impl(const desc_t& desc):
                desc_(desc)
            {
            }

            void add_to_player(player_t& p) override
            {
                track_ = track_t
                {
                    {
                        {0.0f, desc_.finish_alpha},
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
                    *desc_.pobj
                );
            }

        private:
            desc_t desc_;
            track_t track_;
    };

    template<class Object>
    std::unique_ptr<abstract_track> make_impl(const immediate_alpha_transition<Object>& desc)
    {
        return std::make_unique<immediate_alpha_transition_impl<Object>>(desc);
    }
}

class animation
{
    public:
        template<class Descriptor>
        void add(const Descriptor& desc)
        {
            tracks_.push_back(make_impl(desc));
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
