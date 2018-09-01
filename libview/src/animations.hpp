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

    virtual void iterate() = 0;

    virtual bool is_done() const = 0;
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
            speed_(speed)
        {
        }

        void iterate()
        {
            //We must postpone initialization because source position isn't
            //known at construction time.
            if(!initialized_)
            {
                init();
                initialized_ = true;
            }

            if(iteration_countdown_ > 1)
            {
                auto pos = t_.get_position();
                pos.x += x_step_;
                pos.y += y_step_;
                t_.set_position(pos);
                --iteration_countdown_;
            }
            else
            {
                t_.set_position(dst_pos_);
                iteration_countdown_ = 0;
            }
        }

        bool is_done() const
        {
            return iteration_countdown_ == 0;
        }

    private:
        void init()
        {
            const auto& src_pos = t_.get_position();
            const auto src_x = src_pos.x;
            const auto src_y = src_pos.y;
            const auto dst_x = dst_pos_.x;
            const auto dst_y = dst_pos_.y;

            const auto x_diff = dst_x - src_x;
            const auto y_diff = dst_y - src_y;

            const auto distance = std::sqrt(x_diff * x_diff + y_diff * y_diff);
            const auto needed_iteration_count = std::max
            (
                1u, //iterate at least once
                static_cast<unsigned int>(distance / std::abs(speed_ / 60))
            );

            x_step_ = x_diff / needed_iteration_count;
            y_step_ = y_diff / needed_iteration_count;
            iteration_countdown_ = needed_iteration_count;
        }

    private:
        tile& t_;
        const point dst_pos_;
        const double speed_;

        bool initialized_ = false;
        double x_step_ = 0;
        double y_step_ = 0;
        unsigned int iteration_countdown_ = 1;
};

class fade_in: public animation
{
    public:
        fade_in(tile& t):
            t_(t)
        {
        }

        void iterate()
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

        void iterate()
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
void iterate(animation_group& g)
{
    for(auto& a: g)
        if(!a->is_done())
            a->iterate();
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
void iterate(animation_queue& q)
{
    if(!q.empty())
    {
        auto& g = q.front();
        iterate(g);
        if(is_done(g))
            q.pop();
    }
}

} //namespace libview

#endif
