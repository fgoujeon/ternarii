/*
Copyright 2018 - 2019 Florian Goujeon

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

#include "circle.hpp"
#include "shaders/flat_circle.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData2D.h>

namespace libview
{

namespace
{
    Magnum::GL::Mesh& get_mesh()
    {
        static Magnum::GL::Mesh mesh = Magnum::MeshTools::compile(Magnum::Primitives::squareSolid());
        return mesh;
    }

    shaders::flat_circle& get_shader()
    {
        static shaders::flat_circle shader;
        return shader;
    }
}

circle::circle(const Magnum::Color4& color, SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    color_(color)
{
}

void circle::set_color(const Magnum::Color4& color)
{
    color_ = color;
}

void circle::draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera)
{
    get_shader().setColor(color_);
    get_shader().setTransformationProjectionMatrix
    (
        camera.projectionMatrix() *
        transformation_matrix
    );
    get_mesh().draw(get_shader());
}

} //namespace
