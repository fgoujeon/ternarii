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

#ifndef LIBVIEW_SRC_COMMON_HPP
#define LIBVIEW_SRC_COMMON_HPP

#include <libview/common.hpp>
#include <Magnum/SceneGraph/AbstractGroupedFeature.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Math/Color.h>

namespace libview
{

using namespace Magnum::Math::Literals;

using Vector2 = Magnum::Vector2;

class clickable: public Magnum::SceneGraph::AbstractGroupedFeature2D<clickable>
{
    public:
        using Magnum::SceneGraph::AbstractGroupedFeature2D<clickable>::AbstractGroupedFeature2D;

        virtual bool is_inside(const Magnum::Vector2& model_space_position) const = 0;

        virtual void mouse_press_event(){}
};

using clickable_group = Magnum::SceneGraph::FeatureGroup2D<clickable>;

struct object_arg_set
{
    Object2D& parent;
    SceneGraph::DrawableGroup2D& drawables;
    clickable_group& clickables;
};

} //namespace

#endif
