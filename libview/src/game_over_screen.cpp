#include "game_over_screen.hpp"

namespace libview
{

game_over_screen::game_over_screen(SDL_Renderer& renderer):
    renderer_(renderer),
    pfont_
    (
        TTF_OpenFont("res/fonts/DejaVuSans.ttf", 90)
    ),
    pgame_over_texture_
    (
        libsdl::make_texture
        (
            renderer_,
            *pfont_,
            "GAME OVER",
            SDL_Color{0x88, 0x88, 0x88, 255}
        )
    )
{
}

void game_over_screen::draw
(
    SDL_Renderer& renderer,
    const SDL_Rect& area0
)
{
    if(!visible_)
        return;

    SDL_Rect area;
    area.x = area0.x;
    area.y = area0.y + 2.0 / 13 * area0.h;
    area.w = area0.w;
    area.h = 3.0 / 13 * area0.h;

    //background
    SDL_SetRenderDrawColor(&renderer, 0x44, 0x44, 0x44, 255);
    SDL_RenderFillRect(&renderer, &area);

    //"GAME OVER"
    {
        int texture_width_px;
        int texture_height_px;
        SDL_QueryTexture
        (
            pgame_over_texture_.get(),
            nullptr,
            nullptr,
            &texture_width_px,
            &texture_height_px
        );
        const auto texture_ratio =
            static_cast<double>(texture_width_px) /
            texture_height_px
        ;

        SDL_Rect r;
        r.w = area.w;
        r.h = r.w / texture_ratio;
        r.x = area.x + area.w / 2 - r.w / 2;
        r.y = area.y + area.h / 2 - r.h / 2;

        SDL_RenderCopy(&renderer, pgame_over_texture_.get(), nullptr, &r);
    }
}

void game_over_screen::set_visible(const bool value)
{
    visible_ = value;
}

} //namespace view
