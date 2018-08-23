#include "grid.hpp"
#include <map>

namespace libview
{

namespace
{
    const auto cell_size = 50;
    const auto column_count = 6;
    const auto row_count = 12;
    const auto tile_margin = cell_size * 0.05;
    const auto tile_size = cell_size - tile_margin * 2;

    std::map<unsigned int, SDL_Color> item_color_map
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

    SDL_Texture* get_tile_number_texture
    (
        SDL_Renderer& renderer,
        TTF_Font& font,
        grid::number_texture_map& tile_number_textures,
        const unsigned int value
    )
    {
        auto& ptexture = tile_number_textures[value];

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

    SDL_Color get_item_color(const unsigned int value)
    {
        const auto it = item_color_map.find(value);
        if(it != item_color_map.end())
            return it->second;
        else
            return SDL_Color{0x00, 0x00, 0x00, 0xff};
    }

    void draw_tile
    (
        SDL_Renderer& renderer,
        TTF_Font& font,
        grid::number_texture_map& tile_number_textures,
        const unsigned int value,
        const double pos_x_cell, //position of the center of the item, in cell unity
        const double pos_y_cell
    )
    {
        const auto c = get_item_color(value);

        //draw background box
        {
            SDL_Rect r;
            r.x = pos_x_cell * cell_size + tile_margin;
            r.y = pos_y_cell * cell_size + tile_margin;
            r.w = tile_size;
            r.h = tile_size;
            SDL_SetRenderDrawColor(&renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(&renderer, &r);
        }

        //draw number
        {
            auto pnumber_texture = get_tile_number_texture
            (
                renderer,
                font,
                tile_number_textures,
                value
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

            const auto number_width = tile_size * number_texture_ratio * 0.6;
            const auto number_height = tile_size * 0.6;

            SDL_Rect r;
            r.x = pos_x_cell * cell_size + cell_size / 2 - number_width / 2;
            r.y = pos_y_cell * cell_size + cell_size / 2 - number_height / 2;
            r.w = number_width;
            r.h = number_height;
            SDL_RenderCopy(&renderer, pnumber_texture, nullptr, &r);
        }
    }

    template<class Items>
    void draw_tiles
    (
        SDL_Renderer& renderer,
        TTF_Font& font,
        grid::number_texture_map& tile_number_textures,
        const Items& items,
        const double pos_y_cell
    )
    {
        auto x = 0;
        for(const auto& item_column: items)
        {
            auto y = 0;
            for(const auto& opt_item: item_column)
            {
                if(opt_item)
                    draw_tile(renderer, font, tile_number_textures, opt_item->value, x, pos_y_cell - y);
                ++y;
            }
            ++x;
        }
    }
}

grid::grid(SDL_Renderer& renderer):
    ptile_font_
    (
        TTF_OpenFont("res/fonts/DejaVuSans.ttf", 90)
    )
{
}

int grid::get_logical_width() const
{
    return column_count * cell_size;
}

int grid::get_logical_height() const
{
    return row_count * cell_size;
}

void grid::draw(SDL_Renderer& renderer)
{
    //background
    {
        const auto r = SDL_Rect{0, 0, 6 * cell_size, 12 * cell_size};
        SDL_SetRenderDrawColor(&renderer, 0x66, 0x66, 0x66, 255);
        SDL_RenderFillRect(&renderer, &r);
    }

    //death line
    {
        const auto r = SDL_Rect{0, 5 * cell_size - 1, 6 * cell_size, 2};
        SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderFillRect(&renderer, &r);
    }

    //tiles
    draw_tiles(renderer, *ptile_font_, tile_number_textures_, next_input_items, 0);
    draw_tiles(renderer, *ptile_font_, tile_number_textures_, input_items, 3);
    draw_tiles(renderer, *ptile_font_, tile_number_textures_, board_items, 11);
}

} //namespace view
