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

#ifndef LIBVIEW_SHADERS_ROUNDED_RECTANGLE_HPP
#define LIBVIEW_SHADERS_ROUNDED_RECTANGLE_HPP

#include "Magnum/GL/AbstractShaderProgram.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Matrix3.h"

namespace libview::shaders
{

class rounded_rectangle: public Magnum::GL::AbstractShaderProgram
{
    public:
        typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
        typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;

        explicit rounded_rectangle();

        rounded_rectangle(const rounded_rectangle&) = delete;

        rounded_rectangle(rounded_rectangle&&) noexcept = default;

        rounded_rectangle& operator=(const rounded_rectangle&) = delete;

        rounded_rectangle& operator=(rounded_rectangle&&) noexcept = default;

        rounded_rectangle& set_transformation_projection_matrix(const Magnum::Matrix3& matrix)
        {
            setUniform(transformation_projection_matrix_uniform_, matrix);
            return *this;
        }

        rounded_rectangle& set_color(const Magnum::Color4& color)
        {
            setUniform(color_uniform_, color);
            return *this;
        }

        rounded_rectangle& set_dimension(const Magnum::Vector2& value)
        {
            setUniform(dimension_uniform_, value);
            return *this;
        }

        rounded_rectangle& set_radius(const Magnum::Float radius)
        {
            setUniform(radius_uniform_, radius);
            return *this;
        }

        rounded_rectangle& set_smoothness(const Magnum::Float smoothness)
        {
            setUniform(smoothness_uniform_, smoothness);
            return *this;
        }

        rounded_rectangle& set_outline_thickness(const Magnum::Float value)
        {
            setUniform(outline_thickness_uniform_, value);
            return *this;
        }

        rounded_rectangle& set_outline_color(const Magnum::Color4& value)
        {
            setUniform(outline_color_uniform_, value);
            return *this;
        }

    private:
        Magnum::Int transformation_projection_matrix_uniform_ = 0;
        Magnum::Int color_uniform_ = 1;
        Magnum::Int dimension_uniform_ = 2;
        Magnum::Int radius_uniform_ = 3;
        Magnum::Int smoothness_uniform_ = 4;
        Magnum::Int outline_thickness_uniform_ = 5;
        Magnum::Int outline_color_uniform_ = 6;
};

} //namespace

#endif
