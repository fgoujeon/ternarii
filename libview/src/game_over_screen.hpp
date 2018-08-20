#ifndef LIBVIEW_GAME_OVER_SCREEN_HPP
#define LIBVIEW_GAME_OVER_SCREEN_HPP

#include "drawable.hpp"
#include "libsdl.hpp"

namespace libview
{

class game_over_screen: public drawable
{
    public:
        game_over_screen(SDL_Renderer& renderer);

        void draw
        (
            SDL_Renderer& renderer,
            const SDL_Rect& area
        );

        void set_visible(const bool value);

    private:
        SDL_Renderer& renderer_;
        bool visible_ = false;
        libsdl::font_unique_ptr pfont_;
        libsdl::texture_unique_ptr pgame_over_texture_;
};

} //namespace view

#endif
