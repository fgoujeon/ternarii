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

#include "background.hpp"
#include "../shaders/sunshine.hpp"
#include <Magnum/GL/Mesh.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Trade/MeshData2D.h>
#include <cmath>

namespace libview::objects
{

namespace
{
    Magnum::GL::Mesh& get_mesh()
    {
        static Magnum::GL::Mesh mesh = Magnum::MeshTools::compile(Magnum::Primitives::squareSolid());
        return mesh;
    }

    shaders::sunshine& get_shader()
    {
        static shaders::sunshine shader;
        return shader;
    }
}

background::background
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& animables
):
    Object2D{&parent},
    features::drawable{*this, &drawables},
    features::animable{*this, &animables}
{
}

void background::set_color(const Magnum::Color4& color)
{
    color_ = color;
}

void background::draw(const Magnum::Matrix3& transformation_matrix, Magnum::SceneGraph::Camera2D& camera)
{
    get_shader().setColor(get_color_transformation_matrix() * color_);
    get_shader().setTransformationProjectionMatrix
    (
        camera.projectionMatrix() *
        transformation_matrix
    );
    get_mesh().draw(get_shader());
}

void background::advance(const libutil::time_point& /*now*/, const float elapsed_s)
{
    const auto speed_radps = 0.05f; //in radians per second
    angle_rad_ = std::fmodf(angle_rad_ + elapsed_s * speed_radps, 2 * M_PI);
    get_shader().set_angle_rad(angle_rad_);
}

} //namespace
