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

#include "label.hpp"

namespace libview
{

namespace
{
    libsdl::unique_ptr<SDL_Texture> make_texture
    (
        SDL_Renderer& renderer,
        TTF_Font& font,
        const std::string& text,
        const SDL_Color& color
    )
    {
        return libsdl::make_texture
        (
            renderer,
            font,
            text,
            color
        );
    }
}

label::label
(
    SDL_Renderer& renderer,
    const point& position,
    const unsigned int w,
    const unsigned int h,
    const std::string& text,
    const horizontal_alignment halign,
    const vertical_alignment valign,
    const std::string& font_file_path,
    const SDL_Color& color
):
    renderer_(renderer),
    pfont_(TTF_OpenFont(font_file_path.c_str(), 90)),
    position_(position),
    w_(w),
    h_(h),
    color_(color),
    ptexture_(make_texture(renderer, *pfont_, text, color)),
    halign_(halign),
    valign_(valign)
{
}

void label::set_position(const point& position)
{
    position_ = position;
}

void label::set_text(const std::string& text)
{
    ptexture_ = make_texture(renderer_, *pfont_, text, color_);
}

void label::draw()
{
    int texture_width;
    int texture_height;
    SDL_QueryTexture
    (
        ptexture_.get(),
        nullptr,
        nullptr,
        &texture_width,
        &texture_height
    );
    const auto texture_ratio =
        static_cast<double>(texture_width) /
        texture_height
    ;

    const auto label_ratio =
        static_cast<double>(w_) /
        h_
    ;

    SDL_Rect r;
    if(texture_ratio > label_ratio)
    {
        r.w = w_;
        r.h = r.w / texture_ratio;

        r.x = position_.x;

        switch(valign_)
        {
            case vertical_alignment::top:
                r.y = position_.y;
                break;
            case vertical_alignment::center:
                r.y = position_.y + h_ / 2 - r.h / 2;
                break;
            default:
                r.y = position_.y + h_ - r.h;
        }
    }
    else
    {
        r.h = h_;
        r.w = r.h * texture_ratio;

        switch(halign_)
        {
            case horizontal_alignment::left:
                r.x = position_.x;
                break;
            case horizontal_alignment::center:
                r.x = position_.x + w_ / 2 - r.w / 2;
                break;
            default:
                r.x = position_.x + w_ - r.w;
        }

        r.y = position_.y;
    }

    SDL_RenderCopy(&renderer_, ptexture_.get(), nullptr, &r);
}

} //namespace view
