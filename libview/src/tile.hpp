#ifndef LIBVIEW_TILE_HPP
#define LIBVIEW_TILE_HPP

#include <libsdl.hpp>

namespace libview
{

class tile
{
    public:
        tile();

        void set_area(const SDL_Rect& area);

        void set_value(const unsigned int value);

        void draw(SDL_Renderer& renderer);

    private:
        libsdl::unique_ptr<TTF_Font> pfont_;
        SDL_Rect area_;
        unsigned int value_ = 0;
};

} //namespace view

#endif
