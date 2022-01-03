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

#include "animation.hpp"

namespace libview::animation
{

#define INTERPOLATOR(FUNCTION_NAME, TYPE, EASING) \
    const interpolator_t<TYPE>& FUNCTION_NAME() \
    { \
        static const auto interpolator = interpolator_t<TYPE> \
        { \
            Magnum::Animation::ease \
            < \
                TYPE, \
                Magnum::Math::lerp, \
                Magnum::Animation::Easing::EASING \
            >() \
        }; \
        return interpolator; \
    }

INTERPOLATOR(get_exponential_in_float_interpolator, float, exponentialIn)
INTERPOLATOR(get_exponential_out_float_interpolator, float, exponentialOut)
INTERPOLATOR(get_cubic_out_vector2_interpolator, Magnum::Vector2, cubicOut)
INTERPOLATOR(get_back_out_vector2_interpolator, Magnum::Vector2, backOut)

#undef INTERPOLATOR

} //namespace
