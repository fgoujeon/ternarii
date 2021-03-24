/*
Copyright 2018 - 2021 Florian Goujeon

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

#include "common.hpp"
#include <libutil/unique_function.hpp>
#include <chrono>
#include <Magnum/Animation/Easing.h>
#include <Magnum/Animation/Player.h>
#include <Magnum/Animation/Track.h>
#include <chrono>
#include <list>
#include <memory>

namespace libview::animation
{

using player_t = Magnum::Animation::Player<std::chrono::nanoseconds, Magnum::Float>;
using player_supplier_t = libutil::unique_function<void(player_t&)>;

template<class ValueType, class ResultType = Magnum::Animation::ResultOf<ValueType>>
using interpolator_t = ResultType(*)(const ValueType&, const ValueType&, Magnum::Float);

const interpolator_t<Magnum::Vector2>& get_cubic_out_position_interpolator();

namespace tracks
{
    //Arbitrary code
    struct closure
    {
        libutil::unique_function<void()> f;
    };

    inline
    player_supplier_t make_player_supplier(closure&& track)
    {
        using track_impl_t = Magnum::Animation::Track<Magnum::Float, int>;
        return [track = std::move(track), track_impl = track_impl_t{}](player_t& player) mutable
        {
            track_impl = track_impl_t
            (
                {
                    {0.0f, 0},
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            );

            player.addWithCallback
            (
                track_impl,
                [](Magnum::Float, const int&, closure& t)
                {
                    t.f();
                },
                track
            );
        };
    }



    struct pause
    {
        float duration_s = 0;
    };

    inline
    player_supplier_t make_player_supplier(const pause& track)
    {
        using track_impl_t = Magnum::Animation::Track<Magnum::Float, int>;
        return [track, track_impl = track_impl_t{}](player_t& player) mutable
        {
            track_impl = track_impl_t
            (
                {
                    {0.0f, 0},
                    {track.duration_s, 0}
                },
                Magnum::Math::lerp,
                Magnum::Animation::Extrapolation::Constant
            );

            player.addWithCallback
            (
                track_impl,
                [](Magnum::Float, const int&, void*){},
                nullptr
            );
        };
    }



    struct fixed_duration_translation
    {
        std::shared_ptr<object2d> pobj;
        Magnum::Vector2 finish_position;
        float duration_s = 0; //in seconds
        Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>::Interpolator interpolator = Magnum::Math::lerp;
    };

    inline
    player_supplier_t make_player_supplier(const fixed_duration_translation& track)
    {
        using track_impl_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;
        return [track, track_impl = track_impl_t{}](player_t& player) mutable
        {
            const auto& current_position = track.pobj->transformation().translation();

            if(current_position == track.finish_position)
            {
                return;
            }

            if(track.duration_s == 0)
            {
                track_impl = track_impl_t
                {
                    {
                        {0.0f, track.finish_position},
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };
            }
            else
            {
                track_impl = track_impl_t
                {
                    {
                        {0.0f, current_position},
                        {track.duration_s, track.finish_position}
                    },
                    track.interpolator,
                    Magnum::Animation::Extrapolation::Constant
                };
            }

            player.addWithCallback
            (
                track_impl,
                [](Magnum::Float, const Magnum::Vector2& translation, object2d& obj)
                {
                    obj.setTranslation(translation);
                },
                *track.pobj
            );
        };
    }



    struct fixed_speed_translation
    {
        std::shared_ptr<object2d> pobj;
        Magnum::Vector2 finish_position;
        float speed = 0; //in distance unit per second
        Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>::Interpolator interpolator = Magnum::Math::lerp;
    };

    inline
    player_supplier_t make_player_supplier(const fixed_speed_translation& track)
    {
        using track_impl_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;
        return [track, track_impl = track_impl_t{}](player_t& player) mutable
        {
            const auto& current_position = track.pobj->transformation().translation();

            if(current_position == track.finish_position)
            {
                return;
            }

            const auto x1 = current_position.x();
            const auto y1 = current_position.y();
            const auto x2 = track.finish_position.x();
            const auto y2 = track.finish_position.y();
            const auto distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
            const auto time = distance / track.speed;

            track_impl = track_impl_t
            {
                {
                    {0.0f, current_position},
                    {time, track.finish_position}
                },
                track.interpolator,
                Magnum::Animation::Extrapolation::Constant
            };

            player.addWithCallback
            (
                track_impl,
                [](Magnum::Float, const Magnum::Vector2& translation, object2d& obj)
                {
                    obj.setTranslation(translation);
                },
                *track.pobj
            );
        };
    }



    struct alpha_transition
    {
        std::shared_ptr<object2d> pobj;
        float finish_alpha = 0;
        float duration_s = 0; //in seconds
        Magnum::Animation::Track<Magnum::Float, float>::Interpolator interpolator = Magnum::Math::lerp;
    };

    inline
    player_supplier_t make_player_supplier(const alpha_transition& track)
    {
        using track_impl_t = Magnum::Animation::Track<Magnum::Float, float>;
        return [track, track_impl = track_impl_t{}](player_t& player) mutable
        {
            const auto current_alpha = track.pobj->get_alpha();

            if(track.duration_s == 0)
            {
                track_impl = track_impl_t
                {
                    {
                        {0.0f, track.finish_alpha},
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };
            }
            else
            {
                track_impl = track_impl_t
                {
                    {
                        {0.0f, current_alpha},
                        {track.duration_s, track.finish_alpha}
                    },
                    track.interpolator,
                    Magnum::Animation::Extrapolation::Constant
                };
            }

            player.addWithCallback
            (
                track_impl,
                [](Magnum::Float, const float& alpha, object2d& obj)
                {
                    obj.set_alpha(alpha);
                },
                *track.pobj
            );
        };
    }



    struct scaling_transition
    {
        std::shared_ptr<object2d> pobj;
        Magnum::Vector2 finish_scaling;
        float duration_s = 0; //in seconds
        Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>::Interpolator interpolator = Magnum::Math::lerp;
    };

    inline
    player_supplier_t make_player_supplier(const scaling_transition& track)
    {
        using track_impl_t = Magnum::Animation::Track<Magnum::Float, Magnum::Vector2>;

        return [track, track_impl = track_impl_t{}](player_t& player) mutable
        {
            const auto current_scaling = track.pobj->transformation().scaling();

            if(current_scaling == track.finish_scaling)
            {
                return;
            }

            if(track.duration_s == 0)
            {
                track_impl = track_impl_t
                {
                    {
                        {0.0f, track.finish_scaling}
                    },
                    Magnum::Math::lerp,
                    Magnum::Animation::Extrapolation::Constant
                };
            }
            else
            {
                track_impl = track_impl_t
                {
                    {
                        {0.0f, current_scaling},
                        {track.duration_s, track.finish_scaling}
                    },
                    track.interpolator,
                    Magnum::Animation::Extrapolation::Constant
                };
            }

            player.addWithCallback
            (
                track_impl,
                [](Magnum::Float, const Magnum::Vector2& scaling, object2d& obj)
                {
                    obj.setScaling(scaling);
                },
                *track.pobj
            );
        };
    }
}

//An animation is made of several tracks. Tracks are played concurrently.
class animation
{
    public:
        animation() = default;

        animation(const animation&) = delete;

        animation(animation&&) = default;

        /*
        Create a one-track animation.
        This constructor is implicit so that one-track animations can be
        pushed into the animator like so:
            some_animator.push(some_track{...});
        */
        template<class Track>
        animation(Track&& track)
        {
            add(std::forward<Track>(track));
        }

        template<class Track>
        void add(Track&& track)
        {
            player_suppliers_.push_back(make_player_supplier(std::forward<Track>(track)));
        }

        void advance(const std::chrono::steady_clock::time_point& now)
        {
            if(!started_)
            {
                //Supply player with tracks
                for(auto& player_supplier: player_suppliers_)
                {
                    player_supplier(player_);
                }

                player_.play(now.time_since_epoch());

                started_ = true;
            }

            player_.advance(now.time_since_epoch());
        }

        void pause()
        {
            player_.pause(std::chrono::steady_clock::now().time_since_epoch());
        }

        void resume()
        {
            player_.resume(std::chrono::steady_clock::now().time_since_epoch());
        }

        bool is_done() const
        {
            return started_ && player_.state() == Magnum::Animation::State::Stopped;
        }

    private:
        std::vector<player_supplier_t> player_suppliers_;
        player_t player_;
        bool started_ = false;
};

//An animator manages a queue of animations. Pushed animations are played
//sequentially.
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

        void advance(const std::chrono::steady_clock::time_point& now)
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

        void pause()
        {
            if(!animations_.empty())
            {
                animations_.front().pause();
            }
        }

        void resume()
        {
            if(!animations_.empty())
            {
                animations_.front().resume();
            }
        }

    private:
        std::list<animation> animations_;
};

} //namespace

#endif
