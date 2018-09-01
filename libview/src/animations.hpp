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
            const auto distance = speed_ * ellapsed_time;

            const auto curr_pos = t_.get_position();

            auto next_pos = point{};

            if(std::abs(curr_pos.x - dst_pos_.x) <= distance)
                next_pos.x = dst_pos_.x;
            else if(curr_pos.x < dst_pos_.x)
                next_pos.x = curr_pos.x + distance;
            else
                next_pos.x = curr_pos.x - distance;

            if(std::abs(curr_pos.y - dst_pos_.y) <= distance)
                next_pos.y = dst_pos_.y;
            else if(curr_pos.y < dst_pos_.y)
                next_pos.y = curr_pos.y + distance;
            else
                next_pos.y = curr_pos.y - distance;

            if(next_pos == dst_pos_)
                done_ = true;

            t_.set_position(next_pos);
        }

        bool is_done() const
        {
            return done_;
        }

    private:
        tile& t_;
        const point dst_pos_;
        const double speed_;
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
