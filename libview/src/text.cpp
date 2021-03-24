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

#include "text.hpp"

namespace libview::text
{

Magnum::Text::MagnumFont& get_font()
{
    static Magnum::Text::MagnumFont font;
    static bool initialized = false;

    if(!initialized)
    {
        font.initialize();

        if(!font.openFile("res/fonts/DejaVuSans.conf", 0.0f /*dummy*/))
        {
            std::exit(1);
        }

        initialized = true;
    }

    return font;
}

Magnum::Text::GlyphCache& get_glyph_cache()
{
    static Corrade::Containers::Pointer<Magnum::Text::GlyphCache> pcache;

    if(!pcache)
    {
        pcache = Corrade::Containers::pointerCast<Magnum::Text::GlyphCache>(get_font().createGlyphCache());
    }

    return *pcache;
}

Magnum::Shaders::DistanceFieldVector2D& get_shader()
{
    static Magnum::Shaders::DistanceFieldVector2D shader;
    return shader;
}

} //namespace
