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

#include "number_tile.hpp"
#include "../text.hpp"
#include "../colors.hpp"
#include <libres.hpp>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <cmath>

namespace libview::objects
{

namespace
{
    Magnum::Color3 value_to_color(const int value)
    {
        using namespace Magnum::Math::Literals;

        switch(value)
        {
            case 0:  return 0xa05252_rgbf;
            case 1:  return 0xdd3b3b_rgbf;
            case 2:  return 0xef7429_rgbf;
            case 3:  return 0xe5bb13_rgbf;
            case 4:  return 0xa1c927_rgbf;
            case 5:  return 0x2bcd73_rgbf;
            case 6:  return 0x25d7d2_rgbf;
            case 7:  return 0x3f94de_rgbf;
            case 8:  return 0x4c52e0_rgbf;
            case 9:  return 0x904ce0_rgbf;
            case 10: return 0xc490ff_rgbf;
            case 11: return 0x989898_rgbf;
            case 12: return 0x461b3f_rgbf;
            default: return 0x000000_rgbf;
        }
    }

    Magnum::Color3 darker(const Magnum::Color3& c)
    {
        auto hsv = c.toHsv();
        hsv.value /= 1.2f;
        return Magnum::Color3::fromHsv(hsv);
    }

    std::unique_ptr<shine> make_shine
    (
        Object2D& parent,
        features::drawable_group& drawables,
        features::animable_group& animables,
        const int value
    )
    {
        if(value < 11)
        {
            return nullptr;
        }

        auto pshine = std::make_unique<shine>
        (
            parent,
            drawables,
            animables
        );
        pshine->set_color(value_to_color(value));
        pshine->setScaling({1.7f, 1.7f});

        return pshine;
    }

    std::unique_ptr<rounded_rectangle> make_glow
    (
        Object2D& parent,
        features::drawable_group& drawables,
        const int value
    )
    {
        if(value < 10)
        {
            return nullptr;
        }

        auto pglow = std::make_unique<rounded_rectangle>
        (
            parent,
            drawables,
            rounded_rectangle::style
            {
                .color = value_to_color(value),
                .radius = 0.7f,
                .smoothness_factor = 30
            }
        );

        pglow->setScaling({1.8f, 1.8f});

        return pglow;
    }
}

number_tile::number_tile
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    const int value
):
    Object2D(&parent),
    features::animable(*this, &animables),
    square_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = value_to_color(value),
            .radius = 0.5f
        }
    ),
    pshine_(make_shine(*this, drawables, animables, value)),
    pglow_(make_glow(*this, drawables, value)),
    label_
    (
        *this,
        drawables,
        label::style
        {
            .alignment = Magnum::Text::Alignment::MiddleCenter,
            .color = colors::white,
            .font_size = 1.15f,
            .outline_color = darker(value_to_color(value)),
            .outline_range = {0.45f, 0.40f}
        },
        std::to_string(value).c_str()
    )
{
}

void number_tile::advance(const libutil::time_point& /*now*/, float elapsed_s)
{
    if(pglow_)
    {
        glow_cycle_ = std::fmodf(glow_cycle_ + (elapsed_s * 2), 2 * M_PI);
        const auto glow_alpha = std::sinf(glow_cycle_) / 2.0f + 0.5f;
        pglow_->set_alpha(glow_alpha);
    }
}

} //namespace
