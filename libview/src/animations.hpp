/*
Copyright 2018 Florian Goujeon

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

#ifndef LIBVIEW_ANIMATIONS_HPP
#define LIBVIEW_ANIMATIONS_HPP

#include "tile.hpp"
#include "point.hpp"
#include <queue>
#include <cmath>

namespace libview
{

struct animation
{
    virtual ~animation(){}

    virtual void iterate(const double ellapsed_time) = 0;

    virtual bool is_done() const = 0;
};

class pause: public animation
{
    public:
        pause(const double duration /*in seconds*/):
            duration_(duration)
        {
        }

        void iterate(const double ellapsed_time)
        {
            cumulated_ellapsed_time_ += ellapsed_time;
        }

        bool is_done() const
        {
            return cumulated_ellapsed_time_ >= duration_;
        }

    private:
        const double duration_;
        double cumulated_ellapsed_time_ = 0;
};

class translation: public animation
{
    public:
        translation
        (
            tile& t,
            const point& dst_pos,
            const double speed //in pixels per second
        ):
            t_(t),
            dst_pos_(dst_pos),
            speed_(std::abs(speed))
        {
        }

        void iterate(const double ellapsed_time)
        {
            if(!initialized_)
            {
                init();
                initialized_ = true;
            }

            cumulated_ellapsed_time_ += ellapsed_time;

            if(cumulated_ellapsed_time_ < total_time_)
            {
                const auto step_distance = speed_ * ellapsed_time;

                const auto curr_pos = t_.get_position();

                const auto next_pos = point
                {
                    curr_pos.x + step_distance * x_ratio_,
                    curr_pos.y + step_distance * y_ratio_
                };

                t_.set_position(next_pos);
            }
            else
            {
                t_.set_position(dst_pos_);
                done_ = true;
            }
        }

        bool is_done() const
        {
            return done_;
        }

    private:
        void init()
        {
            const auto src_pos = t_.get_position();

            const auto x_diff = dst_pos_.x - src_pos.x;
            const auto y_diff = dst_pos_.y - src_pos.y;

            const auto total_distance = std::sqrt(x_diff * x_diff + y_diff * y_diff);

            x_ratio_ = x_diff / total_distance;
            y_ratio_ = y_diff / total_distance;
            total_time_ = total_distance / speed_;
        }

    private:
        //initialized by constructor
        tile& t_;
        const point dst_pos_;
        const double speed_;

        //initialized by init()
        bool initialized_ = false;
        double x_ratio_;
        double y_ratio_;
        double total_time_;

        //updated by iterate()
        double cumulated_ellapsed_time_ = 0;
        bool done_ = false;
};

class fade_in: public animation
{
    public:
        fade_in(tile& t):
            t_(t)
        {
        }

        void iterate(const double ellapsed_time)
        {
            t_.set_visible(true);
            done_ = true;
        }

        bool is_done() const
        {
            return done_;
        }

    private:
        tile& t_;
        bool done_ = false;
};

class fade_out: public animation
{
    public:
        fade_out(tile& t):
            t_(t)
        {
        }

        void iterate(const double ellapsed_time)
        {
            t_.set_visible(false);
            done_ = true;
        }

        bool is_done() const
        {
            return done_;
        }

    private:
        tile& t_;
        bool done_ = false;
};



/*
animation_group
Group of animations running simultaneously
*/

using animation_group = std::vector<std::unique_ptr<animation>>;

inline
void iterate(animation_group& g, const double ellapsed_time)
{
    for(auto& a: g)
        if(!a->is_done())
            a->iterate(ellapsed_time);
}

inline
bool is_done(const animation_group& g)
{
    for(auto& a: g)
        if(!a->is_done())
            return false;
    return true;
}



using animation_queue = std::queue<animation_group>;

inline
void iterate(animation_queue& q, const double ellapsed_time)
{
    if(!q.empty())
    {
        auto& g = q.front();
        iterate(g, ellapsed_time);
        if(is_done(g))
            q.pop();
    }
}

} //namespace libview

#endif
