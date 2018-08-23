#ifndef LIBVIEW_SCORE_DISPLAY_HPP
#define LIBVIEW_SCORE_DISPLAY_HPP

#include <libsdl.hpp>

namespace libview
{

class score_display
{
    public:
        score_display(SDL_Renderer& renderer);

        void set_area(const SDL_Rect& area);

        void set_score(const unsigned int value);

        void draw(SDL_Renderer& renderer);

    private:
        SDL_Renderer& renderer_;
        libsdl::unique_ptr<TTF_Font> pfont_;
        libsdl::unique_ptr<SDL_Texture> ptexture_;
        SDL_Rect area_ = SDL_Rect{0, 0, 0, 0};
};

} //namespace view

#endif
