#include "grid.hpp"

namespace libview
{

namespace
{
    void draw_item
    (
        libsdl::renderer& renderer,
        const rectangle& area,
        const double pos_x_cell, //position of the center of the item, in cell unity
        const double pos_y_cell
    )
    {
        const auto cell_size_px = area.width_px / 6;
        const auto size_px = cell_size_px * 0.9;

        SDL_Rect r;
        r.x = area.pos_x_px + pos_x_cell * cell_size_px + cell_size_px / 2.0 - size_px / 2.0;
        r.y = area.pos_y_px + pos_y_cell * cell_size_px + cell_size_px / 2.0 - size_px / 2.0;
        r.w = size_px;
        r.h = size_px;
        SDL_SetRenderDrawColor(renderer.ptr, 0x69, 0xff, 0xC2, 255);
        SDL_RenderFillRect(renderer.ptr, &r);
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

    //next input
    draw_item(renderer, area, 2, 1);
    draw_item(renderer, area, 3, 1);

    //input
    draw_item(renderer, area, 2, 4.5);
    draw_item(renderer, area, 3, 4.5);

    //board
    for(auto x = 0; x < 6; ++x)
        for(auto y = 0; y < 7; ++y)
            draw_item(renderer, area, x, y + 7);
}

} //namespace view
