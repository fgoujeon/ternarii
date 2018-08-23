#ifndef LIBVIEW_GRID_HPP
#define LIBVIEW_GRID_HPP

#include "libsdl.hpp"
#include <libview/item.hpp>
#include <map>

namespace libview
{

class grid
{
    public:
        using number_texture_map = std::map<unsigned int, libsdl::texture_unique_ptr>;

    public:
        grid(SDL_Renderer& renderer);

        int get_logical_width() const;

        int get_logical_height() const;

        void draw(SDL_Renderer& renderer);

        void set_next_input_items(const next_input_item_array& items)
        {
            next_input_items = items;
        }

        void set_input_items(const input_item_array& items)
        {
            input_items = items;
        }

        void set_board_items(const board_item_array& items)
        {
            board_items = items;
        }

    private:
        libsdl::font_unique_ptr ptile_font_;
        number_texture_map tile_number_textures_;

        next_input_item_array next_input_items;
        input_item_array input_items;
        board_item_array board_items;
};

} //namespace view

#endif
