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

#include "rounded_rectangle.hpp"
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Extensions.h>
#include <Magnum/GL/Shader.h>
#include <Corrade/Containers/Reference.h>

namespace libview::shaders
{

namespace
{
    constexpr auto vert_src =
       #include "common.vert"
    ;

    constexpr auto frag_src =
       #include "rounded_rectangle.frag"
    ;
}

rounded_rectangle::rounded_rectangle()
{
    const auto version = Magnum::GL::Version::GLES200;

    Magnum::GL::Shader vert(version, Magnum::GL::Shader::Type::Vertex);
    vert.addSource(vert_src);

    Magnum::GL::Shader frag(version, Magnum::GL::Shader::Type::Fragment);
    frag.addSource(frag_src);

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    transformation_projection_matrix_uniform_ = uniformLocation("u_transformation_projection_matrix");
    color_uniform_ = uniformLocation("u_color");
    dimension_uniform_ = uniformLocation("u_dimension");
    radius_uniform_ = uniformLocation("u_radius");
    smoothness_uniform_ = uniformLocation("u_smoothness");
    outline_thickness_uniform_ = uniformLocation("u_outline_thickness");
    outline_color_uniform_ = uniformLocation("u_outline_color");

    setTransformationProjectionMatrix({});
    setColor(Magnum::Color4{1.0f});
    set_dimension({1.0f, 1.0f});
    set_radius(0.0f);
    set_smoothness(0.0f);
    set_outline_thickness(0.0f);
    set_outline_color(Magnum::Color4{1.0f});
}

} //namespace
