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

#include "rounded_rectangle.hpp"
#include "../shaders/rounded_rectangle.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
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

    shaders::rounded_rectangle& get_shader()
    {
        static shaders::rounded_rectangle shader;
        return shader;
    }
}

rounded_rectangle::rounded_rectangle
(
    object2d& parent,
    features::drawable_group& drawables,
    const style& stl
):
    object2d{&parent},
    features::drawable{*this, &drawables},
    style_(stl)
{
}

void rounded_rectangle::set_color(const Magnum::Color4& color)
{
    style_.color = color;
}

void rounded_rectangle::draw(const Magnum::Matrix3& transformation_matrix, camera& camera)
{
    const auto absolute_alpha = get_absolute_alpha();

    get_shader().set_color(style_.color * absolute_alpha);
    get_shader().set_transformation_projection_matrix
    (
        camera.projectionMatrix() *
        transformation_matrix
    );
    get_shader().set_dimension(style_.dimension);
    get_shader().set_radius(style_.radius);
    get_shader().set_smoothness(style_.smoothness_factor * 0.03f / transformation_matrix.scaling().x());
    get_shader().set_outline_color(style_.outline_color * absolute_alpha);
    get_shader().set_outline_thickness(style_.outline_thickness);
    get_shader().draw(get_mesh());
}

} //namespace
