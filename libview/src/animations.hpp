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
        translation(tile& t, const point& dst_pos):
            t_(t),
            dst_pos_(dst_pos)
        {
        }

        void iterate()
        {
            if(first_)
            {
                init();
                first_ = false;
            }

            auto pos = t_.get_position();
            pos.x += x_step_;
            pos.y += y_step_;
            t_.set_position(pos);

            --iteration_countdown_;
            if(iteration_countdown_ == 0)
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
            const auto& src_pos = t_.get_position();
            const auto src_x = src_pos.x;
            const auto src_y = src_pos.y;
            const auto dst_x = dst_pos_.x;
            const auto dst_y = dst_pos_.y;

            x_step_ = (dst_x - src_x) / 10;
            y_step_ = (dst_y - src_y) / 10;
        }

    private:
        tile& t_;
        point dst_pos_;
        bool first_ = true;
        double x_step_ = 0;
        double y_step_ = 0;
        unsigned int iteration_countdown_ = 10;
        bool done_ = false;
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
