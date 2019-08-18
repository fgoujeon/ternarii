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
#include <Magnum/Shaders/DistanceFieldVector.h>
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

            if(!font.openFile("res/fonts/DejaVuSans.ttf", 110.0f /*font size*/))
            {
                std::exit(1);
            }

            initialized = true;
        }

        return font;
    }

    Magnum::Text::DistanceFieldGlyphCache& get_glyph_cache()
    {
        static Magnum::Text::DistanceFieldGlyphCache cache
        {
            Magnum::Vector2i(2048), //Unscaled glyph cache texture size
            Magnum::Vector2i(512), //Actual glyph cache texture size
            22 //Distance field computation radius
        };
        static bool initialized = false;

        if(!initialized)
        {
            get_font().fillGlyphCache(cache, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-+,.! ");
            initialized = true;
        }

        return cache;
    }

    Magnum::Shaders::DistanceFieldVector2D& get_shader()
    {
        static Magnum::Shaders::DistanceFieldVector2D shader;
        return shader;
    }
}

score_display::score_display(SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    renderer_(get_font(), get_glyph_cache(), 0.7f, Magnum::Text::Alignment::LineRight)
{
    renderer_.reserve(40, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    renderer_.render("1 234 567");
}

void score_display::draw(const Magnum::Matrix3& transformationMatrix, SceneGraph::Camera2D& camera)
{
    using namespace Magnum::Math::Literals;

    get_shader().bindVectorTexture(get_glyph_cache().texture());
    get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);
    get_shader().setColor(0xffffff_rgbf);
    get_shader().setSmoothness(0.025f/ transformationMatrix.uniformScaling());
    renderer_.mesh().draw(get_shader());
}

} //namespace
