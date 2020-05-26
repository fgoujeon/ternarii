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

#ifndef LIBVIEW_COMMON_HPP
#define LIBVIEW_COMMON_HPP

#include "features/animable.hpp"
#include "features/clickable.hpp"
#include "features/key_event_handler.hpp"
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>

namespace libview
{

namespace SceneGraph = Magnum::SceneGraph;

class Object2D: public SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D>
{
    private:
        using base_t = SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D>;

    public:
        Object2D(Object2D* pparent = nullptr):
            base_t(pparent)
        {
        }

        virtual ~Object2D() = default;

        float get_alpha() const
        {
            return alpha_;
        }

        void set_alpha(const float value)
        {
            alpha_ = value;
        }

    private:
        float alpha_ = 1.0f;
};

class Scene2D: public Object2D
{
    public:
        explicit Scene2D() = default;

    private:
        bool isScene() const override final
        {
            return true;
        }
};

namespace features
{
    using drawable = SceneGraph::Drawable2D;
    using drawable_group = SceneGraph::DrawableGroup2D;
}

struct feature_group_set
{
    features::drawable_group drawables;
    features::animable_group animables;
    features::clickable_group clickables;
    features::key_event_handler_group key_event_handlers;
};

} //namespace

#endif
