/*
Copyright 2018 - 2022 Florian Goujeon

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

#include "square.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData2D.h>

namespace libview::objects
{

namespace
{
    Magnum::GL::Mesh& get_mesh()
    {
        static Magnum::GL::Mesh mesh = Magnum::MeshTools::compile(Magnum::Primitives::squareSolid());
        return mesh;
    }

    Magnum::Shaders::Flat2D& get_shader()
    {
        static Magnum::Shaders::Flat2D shader;
        return shader;
    }
}

square::square(object2d& parent, features::drawable_group& drawables, const Magnum::Color4& color):
    object2d{&parent},
    features::drawable{*this, &drawables},
    color_(color)
{
}

void square::set_color(const Magnum::Color4& color)
{
    color_ = color;
}

void square::draw(const Magnum::Matrix3& transformation_matrix, camera& camera)
{
    get_shader().setColor(color_ * get_absolute_alpha());
    get_shader().setTransformationProjectionMatrix
    (
        camera.projectionMatrix() *
        transformation_matrix
    );
    get_shader().draw(get_mesh());
}

} //namespace
