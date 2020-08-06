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

#include "shine.hpp"
#include "../shaders/shine.hpp"
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

    shaders::shine& get_shader()
    {
        static shaders::shine shader;
        return shader;
    }
}

shine::shine
(
    object2d& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    const style& stl
):
    object2d{&parent},
    features::drawable{*this, &drawables},
    features::animable{*this, &animables},
    style_(stl)
{
}

void shine::draw(const Magnum::Matrix3& transformation_matrix, camera& camera)
{
    get_shader().set_color(style_.color * get_absolute_alpha());
    get_shader().set_transformation_projection_matrix
    (
        camera.projectionMatrix() *
        transformation_matrix
    );
    get_shader().set_angle_rad(angle_rad_);
    get_shader().set_ray_count(style_.ray_count);
    get_shader().set_ray_width(style_.ray_width);
    get_shader().set_smoothness(0.045f / transformation_matrix.uniformScaling());
    get_shader().draw(get_mesh());
}

void shine::advance(const std::chrono::steady_clock::time_point& /*now*/, const float elapsed_s)
{
    angle_rad_ = std::fmodf(angle_rad_ + elapsed_s * style_.speed_radps, 2 * M_PI);
}

} //namespace
