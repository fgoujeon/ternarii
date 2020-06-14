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
    std::string get_label_text(const int value)
    {
        switch(value)
        {
            default: return std::to_string(value);
            case 10: return "X";
            case 11: return "γ"; //Gamma
            case 12: return "Ω"; //Omega
            case 13: return "?";
        }
    }

    Magnum::Color3 lighter(const Magnum::Color3& c)
    {
        auto hsv = c.toHsv();
        hsv.value = (hsv.value + 1.0f) / 2;
        return Magnum::Color3::fromHsv(hsv);
    }

    Magnum::Color3 darker(const Magnum::Color3& c)
    {
        auto hsv = c.toHsv();
        hsv.value /= 1.2f;
        return Magnum::Color3::fromHsv(hsv);
    }

    Magnum::Color3 get_square_color(const int value)
    {
        switch(value)
        {
            case 0:  return 0x8c5e5e_rgbf; //Infrared
            case 1:  return 0xdd3b3b_rgbf; //Red
            case 2:  return 0xef7429_rgbf; //Orange
            case 3:  return 0xe5b413_rgbf; //Yellow
            case 4:  return 0xa1c927_rgbf; //Lime
            case 5:  return 0x2bcd61_rgbf; //Green
            case 6:  return 0x25d1d7_rgbf; //Cyan
            case 7:  return 0x3f8fde_rgbf; //Blue
            case 8:  return 0x6c55e0_rgbf; //Violet
            case 9:  return 0xc6a9ff_rgbf; //Ultraviolet
            case 10: return 0x969696_rgbf; //X-ray
            case 11: return 0xd7d13f_rgbf; //Gamma ray
            case 12: return 0x000000_rgbf; //Cosmic ray
            default: return 0x000000_rgbf;
        }
    }

    Magnum::Color3 get_glow_color(const int value)
    {
        return lighter(get_square_color(value));
    }

    Magnum::Color3 get_shine_color(const int value)
    {
        return lighter(get_square_color(value));
    }

    std::unique_ptr<shine> make_shine
    (
        object2d& parent,
        features::drawable_group& drawables,
        features::animable_group& animables,
        const shine::style& style,
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
            animables,
            style
        );
        pshine->setScaling({1.7f, 1.7f});

        return pshine;
    }

    std::unique_ptr<rounded_rectangle> make_glow
    (
        object2d& parent,
        features::drawable_group& drawables,
        const int value
    )
    {
        if(value < 9)
        {
            return nullptr;
        }

        auto pglow = std::make_unique<rounded_rectangle>
        (
            parent,
            drawables,
            rounded_rectangle::style
            {
                .color = get_glow_color(value),
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
    object2d& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    const int value
):
    object2d(&parent),
    features::animable(*this, &animables),
    pshine0_
    (
        make_shine
        (
            *this,
            drawables,
            animables,
            shine::style
            {
                .color = get_shine_color(value),
                .ray_count = 8,
                .ray_width = 0.5f,
                .speed_radps = 0.05f
            },
            value
        )
    ),
    pshine1_
    (
        make_shine
        (
            *this,
            drawables,
            animables,
            shine::style
            {
                .color = get_shine_color(value),
                .ray_count = 6,
                .ray_width = 0.1f,
                .speed_radps = 0.12f
            },
            value
        )
    ),
    square_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = get_square_color(value),
            .radius = 0.6f
        }
    ),
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
            .outline_color = darker(get_square_color(value)),
            .outline_range = {0.45f, 0.40f}
        },
        get_label_text(value).c_str()
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
