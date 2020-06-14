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

#ifndef LIBVIEW_SHADERS_SHINE_HPP
#define LIBVIEW_SHADERS_SHINE_HPP

#include <libutil/time.hpp>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix3.h>

namespace libview::shaders
{

class shine: public Magnum::GL::AbstractShaderProgram
{
    public:
        typedef Magnum::GL::Attribute<0, Magnum::Vector2> Position;
        typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;

        explicit shine();

        shine(const shine&) = delete;

        shine(shine&&) noexcept = default;

        shine& operator=(const shine&) = delete;

        shine& operator=(shine&&) noexcept = default;

        shine& set_transformation_projection_matrix(const Magnum::Matrix3& matrix)
        {
            setUniform(transformation_projection_matrix_uniform_, matrix);
            return *this;
        }

        shine& set_color(const Magnum::Color4& color)
        {
            setUniform(color_uniform_, color);
            return *this;
        }

        shine& set_ray_count(const int count)
        {
            setUniform(ray_count_uniform_, static_cast<float>(count));
            return *this;
        }

        shine& set_ray_width(const float count)
        {
            setUniform(ray_width_uniform_, count);
            return *this;
        }

        shine& set_smoothness(const float value)
        {
            setUniform(smoothness_uniform_, value);
            return *this;
        }

        void set_angle_rad(float angle_rad);

    private:
        Magnum::Int transformation_projection_matrix_uniform_ = 0;
        Magnum::Int color_uniform_ = 1;
        Magnum::Int angle_uniform_ = 2;
        Magnum::Int ray_count_uniform_ = 3;
        Magnum::Int ray_width_uniform_ = 4;
        Magnum::Int smoothness_uniform_ = 5;
};

} //namespace

#endif
