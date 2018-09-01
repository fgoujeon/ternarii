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

#ifndef LIBVIEW_POINT_HPP
#define LIBVIEW_POINT_HPP

namespace libview
{

struct point
{
    point()
    {
    }

    point(const double x, const double y):
        x(x),
        y(y)
    {
    }

    double x = 0;
    double y = 0;
};

inline
bool operator==(const point& l, const point& r)
{
    return l.x == r.x && l.y == r.y;
}

inline
bool operator!=(const point& l, const point& r)
{
    return !(l == r);
}

} //namespace libview

#endif
