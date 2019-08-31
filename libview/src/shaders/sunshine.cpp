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

#include "sunshine.hpp"
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Extensions.h>
#include <Magnum/GL/Shader.h>
#include <Corrade/Containers/Reference.h>

namespace libview::shaders
{

namespace
{
    enum: Magnum::Int { TextureLayer = 0 };

    constexpr auto vert_src =
       #include "sunshine/vert.inc"
    ;

    constexpr auto frag_src =
       #include "sunshine/frag.inc"
    ;
}

sunshine::sunshine()
{
    const auto version = Magnum::GL::Version::GLES200;

    Magnum::GL::Shader vert(version, Magnum::GL::Shader::Type::Vertex);
    vert.addSource(vert_src);

    Magnum::GL::Shader frag(version, Magnum::GL::Shader::Type::Fragment);
    frag.addSource(frag_src);

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    transformationProjectionMatrixUniform_ = uniformLocation("transformationProjectionMatrix");
    colorUniform_ = uniformLocation("color");

    setTransformationProjectionMatrix({});
    setColor(Magnum::Color4{1.0f});
}

} //namespace
