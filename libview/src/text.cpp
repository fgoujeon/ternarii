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

#include "text.hpp"

namespace libview::text
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

} //namespace
