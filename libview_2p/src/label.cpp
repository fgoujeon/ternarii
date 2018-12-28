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

namespace libview_2p
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
    if(position_ != position)
    {
        position_ = position;
        must_update_rect_ = true;
    }
}

void label::set_text(const std::string& text)
{
    if(text_ != text)
    {
        text_ = text;
        must_update_texture_ = true;
    }
}

void label::draw(const geometry::system& system)
{
    if(system_ != system)
    {
        system_ = system;
        must_update_font_ = true;
    }

    update_font();
    update_texture();
    update_rect();

    SDL_RenderCopy(&renderer_, ptexture_.get(), nullptr, &rect_);
}

void label::update_font()
{
    if(must_update_font_)
    {
        pfont_.reset(TTF_OpenFont(font_file_path_.c_str(), font_size_ * system_.unit));
        must_update_font_ = false;
        must_update_texture_ = true;
    }
}

void label::update_texture()
{
    if(must_update_texture_)
    {
        ptexture_ = libsdl::make_texture(renderer_, *pfont_, text_, color_);
        must_update_texture_ = false;
        must_update_rect_ = true;
    }
}

void label::update_rect()
{
    if(must_update_rect_)
    {
        SDL_QueryTexture
        (
            ptexture_.get(),
            nullptr,
            nullptr,
            &rect_.w,
            &rect_.h
        );

        auto texture_x = 0.0;
        switch(halign_)
        {
            case horizontal_alignment::left:
                texture_x = system_.origin.x + system_.unit * position_.x;
                break;
            case horizontal_alignment::center:
                texture_x = system_.origin.x + system_.unit * (position_.x + w_ / 2.0) - rect_.w / 2.0;
                break;
            default:
                texture_x = system_.origin.x + system_.unit * (position_.x + w_) - rect_.w;
        }
        rect_.x = static_cast<int>(texture_x);

        auto texture_y = 0.0;
        switch(valign_)
        {
            case vertical_alignment::top:
                texture_y = system_.origin.y + system_.unit * position_.y;
                break;
            case vertical_alignment::center:
                texture_y = system_.origin.y + system_.unit * (position_.y + h_ / 2.0) - rect_.h / 2.0;
                break;
            default:
                texture_y = system_.origin.y + system_.unit * (position_.y + h_) - rect_.h;
        }
        rect_.y = static_cast<int>(texture_y);

        must_update_rect_ = false;
    }
}

} //namespace view
