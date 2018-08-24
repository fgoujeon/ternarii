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

    template<class TileArray, class ItemArray>
    void fill_tiles(TileArray& tiles, const ItemArray& items, const unsigned int y_offset)
    {
        auto x = 0;
        for(auto& item_column: items)
        {
            auto y = 0;
            for(auto& opt_item: item_column)
            {
                if(opt_item)
                {
                    auto ptile = std::make_unique<tile>();
                    ptile->set_value(opt_item->value);
                    ptile->set_area
                    (
                        SDL_Rect
                        {
                            static_cast<int>(x * cell_size + tile_margin),
                            static_cast<int>((y_offset - y) * cell_size + tile_margin),
                            static_cast<int>(tile_size),
                            static_cast<int>(tile_size)
                        }
                    );
                    tiles[x][y] = std::move(ptile);
                }
                else
                {
                    tiles[x][y] = nullptr;
                }

                ++y;
            }

            ++x;
        }
    }

    template<class TileArray>
    void draw_tiles(TileArray& tiles, SDL_Renderer& renderer)
    {
        for(auto& tile_column: tiles)
            for(auto& ptile: tile_column)
                if(ptile)
                    ptile->draw(renderer);
    }
}

grid::grid(SDL_Renderer& renderer)
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

void grid::set_next_input_items(const next_input_item_array& items)
{
    fill_tiles(next_input_tiles_, items, 0);
}

void grid::set_input_items(const input_item_array& items)
{
    fill_tiles(input_tiles_, items, 3);
}

void grid::set_board_items(const board_item_array& items)
{
    fill_tiles(board_tiles_, items, 11);
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
    draw_tiles(next_input_tiles_, renderer);
    draw_tiles(input_tiles_, renderer);
    draw_tiles(board_tiles_, renderer);
    //draw_tiles(renderer, *ptile_font_, tile_number_textures_, next_input_items, 0);
    //draw_tiles(renderer, *ptile_font_, tile_number_textures_, input_items, 3);
    //draw_tiles(renderer, *ptile_font_, tile_number_textures_, board_items, 11);
}

} //namespace view
