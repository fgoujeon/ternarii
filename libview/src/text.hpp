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

#ifndef LIBVIEW_TEXT_HPP
#define LIBVIEW_TEXT_HPP

#include "magnum_common.hpp"
#include <MagnumPlugins/MagnumFont/MagnumFont.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/GlyphCache.h>

namespace libview::text
{

Magnum::Text::MagnumFont& get_font();

Magnum::Text::GlyphCache& get_glyph_cache();

Magnum::Shaders::DistanceFieldVector2D& get_shader();

} //namespace

#endif
