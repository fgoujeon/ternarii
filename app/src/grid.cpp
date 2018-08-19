#include "grid.hpp"

grid::grid()
{
}

void grid::draw
(
    libsdl::renderer& renderer,
    const rectangle& area
)
{
    SDL_Rect r;
    r.x = area.pos_x_px;
    r.y = area.pos_y_px;
    r.w = area.width_px;
    r.h = area.height_px;

    SDL_SetRenderDrawColor(renderer.ptr, 0x69, 0, 0xC2, 255);
    SDL_RenderFillRect(renderer.ptr, &r);
}
