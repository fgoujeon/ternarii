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

#ifndef LIBUTIL_TREE_HPP
#define LIBUTIL_TREE_HPP

#include <forward_list>

namespace libutil
{

template<class T>
class tree
{
    public:
        tree() = default;

        tree(const T& value):
            value_(value)
        {
        }

        tree(const tree&) = default;

        tree(tree&&) = default;

        const T& get_value() const
        {
            return value_;
        }

        void set_value(const T& value)
        {
            value_ = value;
        }

        tree* get_parent()
        {
            return pparent_;
        }

        const tree* get_parent() const
        {
            return pparent_;
        }

        const auto& get_children() const
        {
            return children_;
        }

        tree& add_child(const T& value)
        {
            auto& child = children_.emplace_front(value);
            child.pparent_ = this;
            return child;
        }

    private:
        T value_ = T{};
        tree* pparent_ = nullptr;
        std::forward_list<tree> children_;
};

template<class T>
int get_height(const tree<T>& t)
{
    int height = 0;
    for(const auto& child: t.get_children())
        height = std::max(get_height(child) + 1, height);
    return height;
}

template<class T>
bool contains(const tree<T>& t, const T& value)
{
    for(const auto& child: t.get_children())
    {
        if(contains(child, value))
        {
            return true;
        }
    }

    return false;
}

} //namespace

#endif
