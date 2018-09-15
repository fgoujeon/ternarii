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
#include "draw.hpp"

namespace libview
{

label::label
(
    SDL_Renderer& renderer,
    const std::string& font_file_path,
    const unsigned int font_size,
    const SDL_Color& color,
    const geometry::point& position,
    const unsigned int w,
    const unsigned int h,
    const std::string& text,
    const horizontal_alignment halign,
    const vertical_alignment valign
):
    renderer_(renderer),
    font_file_path_(font_file_path),
    font_size_(font_size),
    color_(color),
    position_(position),
    w_(w),
    h_(h),
    text_(text),
    halign_(halign),
    valign_(valign)
{
}

void label::set_position(const geometry::point& position)
{
    position_ = position;
}

void label::set_text(const std::string& text)
{
    if(text_ != text)
    {
        text_ = text;
        ptexture_.reset();
    }
}

void label::draw(const geometry::system& sys)
{
    update_font(sys.unit);
    update_texture();

    SDL_Rect r;

    SDL_QueryTexture
    (
        ptexture_.get(),
        nullptr,
        nullptr,
        &r.w,
        &r.h
    );

    auto texture_x = 0.0;
    switch(halign_)
    {
        case horizontal_alignment::left:
            texture_x = sys.origin.x + sys.unit * position_.x;
            break;
        case horizontal_alignment::center:
            texture_x = sys.origin.x + sys.unit * (position_.x + w_ / 2.0) - r.w / 2.0;
            break;
        default:
            texture_x = sys.origin.x + sys.unit * (position_.x + w_) - r.w;
    }

    auto texture_y = 0.0;
    switch(valign_)
    {
        case vertical_alignment::top:
            texture_y = sys.origin.y + sys.unit * position_.y;
            break;
        case vertical_alignment::center:
            texture_y = sys.origin.y + sys.unit * (position_.y + h_ / 2.0) - r.h / 2.0;
            break;
        default:
            texture_y = sys.origin.y + sys.unit * (position_.y + h_) - r.h;
    }

    {
        const auto r2 = SDL_Rect
        {
            static_cast<int>(texture_x),
            static_cast<int>(texture_y),
            r.w,
            r.h
        };
        SDL_RenderCopy(&renderer_, ptexture_.get(), nullptr, &r2);
    }
}

void label::update_font(const double system_unit)
{
    if(!pfont_ || applied_system_unit_ != system_unit)
    {
        pfont_.reset(TTF_OpenFont(font_file_path_.c_str(), font_size_ * system_unit));
        applied_system_unit_ = system_unit;

        ptexture_.reset();
    }
}

void label::update_texture()
{
    if(!ptexture_)
    {
        ptexture_ = libsdl::make_texture(renderer_, *pfont_, text_, color_);
    }
}

} //namespace view
