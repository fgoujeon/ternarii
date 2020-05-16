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

using Scene2D = SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation2D>;
using Object2D = SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation2D>;

namespace features
{
    using drawable = SceneGraph::Drawable2D;
    using drawable_group = SceneGraph::DrawableGroup2D;
}

} //namespace

#endif
