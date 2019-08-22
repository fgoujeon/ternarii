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

#include "score_display.hpp"
#include <MagnumPlugins/FreeTypeFont/FreeTypeFont.h>
#include <Magnum/Shaders/Vector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>

namespace libview
{

namespace
{
    Magnum::Text::FreeTypeFont& get_font()
    {
        static Magnum::Text::FreeTypeFont font;
        static bool initialized = false;

        if(!initialized)
        {
            font.initialize();

            if(!font.openFile("res/fonts/DejaVuSans.ttf", 40.0f /*font size*/))
            {
                std::exit(1);
            }

            initialized = true;
        }

        return font;
    }

    Magnum::Text::GlyphCache& get_glyph_cache()
    {
        static Magnum::Text::GlyphCache cache
        {
            Magnum::Vector2i(2048) //Unscaled glyph cache texture size
        };
        static bool initialized = false;

        if(!initialized)
        {
            get_font().fillGlyphCache(cache, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ");
            initialized = true;
        }

        return cache;
    }

    Magnum::Shaders::Vector2D& get_shader()
    {
        static Magnum::Shaders::Vector2D shader;
        return shader;
    }

    std::string score_to_string(const unsigned int score)
    {
        std::string str;
        auto temp_score = score;
        auto digit_index = 0;

        do
        {
            const auto digit = temp_score % 10;
            const auto digit_char = static_cast<char>('0' + digit);

            //add thousands separator
            if(digit_index != 0 && digit_index % 3 == 0)
                str = ' ' + str;

            //add digit
            str = std::string{digit_char} + str;

            ++digit_index;
            temp_score /= 10;
        } while(temp_score != 0);

        return str;
    }
}

score_display::score_display(SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    renderer_(get_font(), get_glyph_cache(), 1.0f, Magnum::Text::Alignment::TopRight)
{
    renderer_.reserve(40, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    renderer_.render("0");
}

void score_display::set_score(const int value)
{
    renderer_.render(score_to_string(value));
}

void score_display::draw(const Magnum::Matrix3& transformationMatrix, SceneGraph::Camera2D& camera)
{
    using namespace Magnum::Math::Literals;

    get_shader().bindVectorTexture(get_glyph_cache().texture());
    get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);
    get_shader().setColor(0xffffff_rgbf);
    renderer_.mesh().draw(get_shader());
}

} //namespace
