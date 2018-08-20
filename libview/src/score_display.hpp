#ifndef LIBVIEW_SCORE_DISPLAY_HPP
#define LIBVIEW_SCORE_DISPLAY_HPP

#include "drawable.hpp"
#include "libsdl.hpp"

namespace libview
{

class score_display: public drawable
{
    public:
        score_display(SDL_Renderer& renderer);

        void draw
        (
            SDL_Renderer& renderer,
            const SDL_Rect& area
        );

        void set_score(const unsigned int value);

    private:
        SDL_Renderer& renderer_;
        libsdl::font_unique_ptr pfont_;
        libsdl::texture_unique_ptr ptexture_;
};

} //namespace view

#endif
