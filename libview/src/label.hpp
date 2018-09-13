/*
Copyright 2018 Florian Goujeon

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

#ifndef LIBVIEW_LABEL_HPP
#define LIBVIEW_LABEL_HPP

#include "system.hpp"
#include "point.hpp"
#include <libsdl.hpp>
#include <string>

namespace libview
{

enum class horizontal_alignment
{
    left,
    center,
    right
};

enum class vertical_alignment
{
    top,
    center,
    bottom
};

class label
{
    public:
        label
        (
            SDL_Renderer& renderer,
            const std::string& font_file_path,
            const unsigned int font_size,
            const SDL_Color& color,
            const point& position,
            const unsigned int w,
            const unsigned int h,
            const std::string& text,
            const horizontal_alignment halign,
            const vertical_alignment valign
        );

        void set_position(const point& position);

        void set_text(const std::string& text);

        void draw(const system& sys);

    private:
        SDL_Renderer& renderer_;
        libsdl::unique_ptr<TTF_Font> pfont_;
        point position_;
        unsigned int w_;
        unsigned int h_;
        SDL_Color color_ = SDL_Color{0, 0, 0, 0};
        libsdl::unique_ptr<SDL_Texture> ptexture_;
        horizontal_alignment halign_;
        vertical_alignment valign_;
};

} //namespace libview

#endif
