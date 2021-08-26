/*
Copyright 2018 - 2021 Florian Goujeon

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

#include "animation.hpp"

namespace libview::animation
{

const interpolator_t<float>& get_exponential_in_float_interpolator()
{
    static const auto interpolator = interpolator_t<float>
    {
        Magnum::Animation::ease
        <
            float,
            Magnum::Math::lerp,
            Magnum::Animation::Easing::exponentialIn
        >()
    };
    return interpolator;
}

const interpolator_t<Magnum::Vector2>& get_cubic_out_position_interpolator()
{
    static const auto interpolator = interpolator_t<Magnum::Vector2>
    {
        Magnum::Animation::ease
        <
            Magnum::Vector2,
            Magnum::Math::lerp,
            Magnum::Animation::Easing::cubicOut
        >()
    };
    return interpolator;
}

} //namespace
