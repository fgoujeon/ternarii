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

#ifndef LIBVIEW_SHADERS_FLAT_CIRCLE_HPP
#define LIBVIEW_SHADERS_FLAT_CIRCLE_HPP

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"

namespace libview::shaders
{

class flat_circle: public Magnum::GL::AbstractShaderProgram
{
    public:
        typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
        typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;

        explicit flat_circle();

        flat_circle(const flat_circle&) = delete;

        flat_circle(flat_circle&&) noexcept = default;

        flat_circle& operator=(const flat_circle&) = delete;

        flat_circle& operator=(flat_circle&&) noexcept = default;

        flat_circle& setTransformationProjectionMatrix(const Magnum::Matrix3& matrix)
        {
            setUniform(transformationProjectionMatrixUniform_, matrix);
            return *this;
        }

        flat_circle& setColor(const Magnum::Color4& color)
        {
            setUniform(colorUniform_, color);
            return *this;
        }

    private:
        Magnum::Int transformationProjectionMatrixUniform_{0}, colorUniform_{1};
};

} //namespace

#endif
