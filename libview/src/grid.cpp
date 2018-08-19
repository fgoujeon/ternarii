#include "grid.hpp"
#include "color.hpp"
#include <map>

namespace libview
{

namespace
{
    std::map<unsigned int, color> item_color_map
    {
        {0,  color{0x82, 0x3b, 0x3b, 0xff}},
        {1,  color{0xe2, 0x1c, 0x1c, 0xff}},
        {2,  color{0xe1, 0x82, 0x1d, 0xff}},
        {3,  color{0xd2, 0xd8, 0x26, 0xff}},
        {4,  color{0x7b, 0xb3, 0x4b, 0xff}},
        {5,  color{0x4a, 0xb4, 0x50, 0xff}},
        {6,  color{0x51, 0xad, 0x88, 0xff}},
        {7,  color{0x51, 0xa3, 0xad, 0xff}},
        {8,  color{0x50, 0x73, 0xae, 0xff}},
        {9,  color{0x5c, 0x4e, 0xb0, 0xff}},
        {10, color{0x8e, 0x51, 0xad, 0xff}},
        {11, color{0xb4, 0x4a, 0xa2, 0xff}},
        {12, color{0xe7, 0x00, 0xc1, 0xff}},
        {13, color{0x98, 0x98, 0x98, 0xff}},
        {14, color{0x46, 0x1b, 0x3f, 0xff}},
        {15, color{0x00, 0x00, 0x00, 0xff}}
    };

    color get_item_color(const unsigned int value)
    {
        const auto it = item_color_map.find(value);
        if(it != item_color_map.end())
            return it->second;
        else
            return color{0x00, 0x00, 0x00, 0xff};
    }

    void draw_item
    (
        libsdl::renderer& renderer,
        const rectangle& area,
        const unsigned int value,
        const double pos_x_cell, //position of the center of the item, in cell unity
        const double pos_y_cell
    )
    {
        const auto cell_size_px = area.width_px / 6;
        const auto size_px = cell_size_px * 0.9;
        const auto c = get_item_color(value);

        SDL_Rect r;
        r.x = area.pos_x_px + pos_x_cell * cell_size_px + cell_size_px / 2.0 - size_px / 2.0;
        r.y = area.pos_y_px + pos_y_cell * cell_size_px + cell_size_px / 2.0 - size_px / 2.0;
        r.w = size_px;
        r.h = size_px;
        SDL_SetRenderDrawColor(renderer.ptr, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(renderer.ptr, &r);
    }

    template<class Items>
    void draw_items
    (
        libsdl::renderer& renderer,
        const rectangle& area,
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
                    draw_item(renderer, area, opt_item->value, x, pos_y_cell - y);
                ++y;
            }
            ++x;
        }
    }
}

grid::grid()
{
}

void grid::draw
(
    libsdl::renderer& renderer,
    const rectangle& area
)
{
    {
        SDL_Rect r;
        r.x = area.pos_x_px;
        r.y = area.pos_y_px;
        r.w = area.width_px;
        r.h = area.height_px;
        SDL_SetRenderDrawColor(renderer.ptr, 0x66, 0x66, 0x66, 255);
        SDL_RenderFillRect(renderer.ptr, &r);
    }

    draw_items(renderer, area, next_input_items, 1);
    draw_items(renderer, area, input_items, 4.5);
    draw_items(renderer, area, board_items, 13);
}

} //namespace view
