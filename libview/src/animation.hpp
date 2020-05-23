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

#include <libutil/unique_function.hpp>
#include <libutil/time.hpp>
#include <Magnum/Animation/Player.h>
#include <chrono>
#include <list>
#include <memory>

namespace libview
{

using player_t = Magnum::Animation::Player<std::chrono::nanoseconds, Magnum::Float>;
using player_supplier_t = libutil::unique_function<void(player_t&)>;

namespace tracks
{
    struct pause
    {
        float duration_s = 0;
    };

    inline
    player_supplier_t make_player_supplier(const pause& info)
    {
        using track_t = Magnum::Animation::Track<Magnum::Float, int>;
        return [info, track = track_t{}](player_t& player) mutable
        {
            track = track_t
            (
                {
                    {0.0f, 0},
                    {info.duration_s, 0}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            );

            player.addWithCallback
            (
                track,
                [](Magnum::Float, const int&, void*){},
                nullptr
            );
        };
    }



    template<class Object>
    struct fixed_duration_translation
    {
        std::shared_ptr<Object> pobj;
        Magnum::Vector2 finish_position;
        float duration_s = 0; //in seconds
    };

    template<class Object>
    player_supplier_t make_player_supplier(const fixed_duration_translation<Object>& info)
    {
        using track_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;
        return [info, track = track_t{}](player_t& player) mutable
        {
            const auto& current_position = info.pobj->transformation().translation();

            if(current_position == info.finish_position)
            {
                return;
            }

            track = track_t
            {
                {
                    {0.0f, current_position},
                    {info.duration_s, info.finish_position}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            player.addWithCallback
            (
                track,
                [](Magnum::Float, const Magnum::Vector2& translation, Object& obj)
                {
                    const auto current_translation = obj.transformation().translation();
                    const auto translation_delta = translation - current_translation;
                    obj.translate(translation_delta);
                },
                *info.pobj
            );
        };
    }



    template<class Object>
    struct fixed_speed_translation
    {
        std::shared_ptr<Object> pobj;
        Magnum::Vector2 finish_position;
        float speed = 0; //in distance unit per second
    };

    template<class Object>
    player_supplier_t make_player_supplier(const fixed_speed_translation<Object>& info)
    {
        using track_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;
        return [info, track = track_t{}](player_t& player) mutable
        {
            const auto& current_position = info.pobj->transformation().translation();

            if(current_position == info.finish_position)
            {
                return;
            }

            const auto x1 = current_position.x();
            const auto y1 = current_position.y();
            const auto x2 = info.finish_position.x();
            const auto y2 = info.finish_position.y();
            const auto distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
            const auto time = distance / info.speed;

            track = track_t
            {
                {
                    {0.0f, current_position},
                    {time, info.finish_position}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            player.addWithCallback
            (
                track,
                [](Magnum::Float, const Magnum::Vector2& translation, Object& obj)
                {
                    const auto current_translation = obj.transformation().translation();
                    const auto translation_delta = translation - current_translation;
                    obj.translate(translation_delta);
                },
                *info.pobj
            );
        };
    }



    template<class Object>
    struct alpha_transition
    {
        std::shared_ptr<Object> pobj;
        float finish_alpha = 0;
        float duration_s = 0; //in seconds
    };

    template<class Object>
    player_supplier_t make_player_supplier(const alpha_transition<Object>& info)
    {
        using track_t = Magnum::Animation::Track<Magnum::Float, float>;
        return [info, track = track_t{}](player_t& player) mutable
        {
            const auto current_alpha = info.pobj->get_alpha();

            if(current_alpha == info.finish_alpha)
            {
                return;
            }

            track = track_t
            {
                {
                    {0.0f, current_alpha},
                    {info.duration_s, info.finish_alpha}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            player.addWithCallback
            (
                track,
                [](Magnum::Float, const float& alpha, Object& obj)
                {
                    obj.set_alpha(alpha);
                },
                *info.pobj
            );
        };
    }



    template<class Object>
    struct immediate_alpha_transition
    {
        std::shared_ptr<Object> pobj;
        float finish_alpha = 0;
    };

    template<class Object>
    player_supplier_t make_player_supplier(const immediate_alpha_transition<Object>& info)
    {
        using track_t = Magnum::Animation::Track<Magnum::Float, float>;
        return [info, track = track_t{}](player_t& player) mutable
        {
            track = track_t
            {
                {
                    {0.0f, info.finish_alpha},
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            };

            player.addWithCallback
            (
                track,
                [](Magnum::Float, const float& alpha, Object& obj)
                {
                    obj.set_alpha(alpha);
                },
                *info.pobj
            );
        };
    }
}

class animation
{
    public:
        template<class Descriptor>
        void add(const Descriptor& info)
        {
            tracks_.push_back(make_player_supplier(info));
        }

        void advance(const libutil::time_point& now)
        {
            if(!started_)
            {
                //Supply player with tracks
                for(auto& player_supplier: tracks_)
                {
                    player_supplier(player_);
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
        std::vector<player_supplier_t> tracks_;
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
