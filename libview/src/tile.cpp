#include "tile.hpp"
#include <map>

namespace libview
{

namespace
{
    SDL_Color get_background_color(const unsigned int value)
    {
        const auto value_color_map = std::map<unsigned int, SDL_Color>
        {
            {0,  SDL_Color{0x82, 0x3b, 0x3b, 0xff}},
            {1,  SDL_Color{0xe2, 0x1c, 0x1c, 0xff}},
            {2,  SDL_Color{0xe1, 0x82, 0x1d, 0xff}},
            {3,  SDL_Color{0xd2, 0xd8, 0x26, 0xff}},
            {4,  SDL_Color{0x7b, 0xb3, 0x4b, 0xff}},
            {5,  SDL_Color{0x4a, 0xb4, 0x50, 0xff}},
            {6,  SDL_Color{0x51, 0xad, 0x88, 0xff}},
            {7,  SDL_Color{0x51, 0xa3, 0xad, 0xff}},
            {8,  SDL_Color{0x50, 0x73, 0xae, 0xff}},
            {9,  SDL_Color{0x5c, 0x4e, 0xb0, 0xff}},
            {10, SDL_Color{0x8e, 0x51, 0xad, 0xff}},
            {11, SDL_Color{0xb4, 0x4a, 0xa2, 0xff}},
            {12, SDL_Color{0xe7, 0x00, 0xc1, 0xff}},
            {13, SDL_Color{0x98, 0x98, 0x98, 0xff}},
            {14, SDL_Color{0x46, 0x1b, 0x3f, 0xff}},
            {15, SDL_Color{0x00, 0x00, 0x00, 0xff}}
        };

        const auto it = value_color_map.find(value);
        if(it != value_color_map.end())
            return it->second;
        else
            return SDL_Color{0x00, 0x00, 0x00, 0xff};
    }

    SDL_Texture* get_number_texture
    (
        SDL_Renderer& renderer,
        TTF_Font& font,
        const unsigned int value
    )
    {
        static auto number_textures = std::map<unsigned int, libsdl::unique_ptr<SDL_Texture>>{};
        auto& ptexture = number_textures[value];

        if(!ptexture)
        {
            ptexture = libsdl::make_texture
            (
                renderer,
                font,
                std::to_string(value),
                SDL_Color{255, 255, 255, 255}
            );
        }

        return ptexture.get();
    }
}

tile::tile():
    pfont_
    (
        TTF_OpenFont("res/fonts/DejaVuSans.ttf", 90)
    )
{
}

void tile::set_area(const SDL_Rect& area)
{
    area_ = area;
}

void tile::set_value(const unsigned int value)
{
    value_ = value;
}

void tile::draw(SDL_Renderer& renderer)
{
    //draw background box
    {
        const auto c = get_background_color(value_);
        SDL_SetRenderDrawColor(&renderer, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(&renderer, &area_);
    }

    //draw number
    {
        auto pnumber_texture = get_number_texture
        (
            renderer,
            *pfont_,
            value_
        );

        int number_texture_width;
        int number_texture_height;
        SDL_QueryTexture
        (
            pnumber_texture,
            nullptr,
            nullptr,
            &number_texture_width,
            &number_texture_height
        );
        const auto number_texture_ratio =
            static_cast<double>(number_texture_width) /
            number_texture_height
        ;

        const auto number_width = area_.w * number_texture_ratio * 0.6;
        const auto number_height = area_.h * 0.6;

        SDL_Rect r;
        r.x = static_cast<int>(area_.x + area_.w / 2.0 - number_width / 2.0);
        r.y = static_cast<int>(area_.y + area_.h / 2.0 - number_height / 2.0);
        r.w = number_width;
        r.h = number_height;
        SDL_RenderCopy(&renderer, pnumber_texture, nullptr, &r);
    }
}

} //namespace view