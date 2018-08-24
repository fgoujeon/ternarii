#ifndef LIBVIEW_GRID_HPP
#define LIBVIEW_GRID_HPP

#include "tile.hpp"
#include <libsdl.hpp>
#include <libview/item.hpp>

namespace libview
{

class grid
{
    private:
        template<size_t Size0, size_t Size1>
        using tile_array = std::array<std::array<std::unique_ptr<tile>, Size1>, Size0>;

        using next_input_tile_array = tile_array<6, 2>;
        using input_tile_array = tile_array<6, 2>;
        using board_tile_array = tile_array<6, 10>;

    public:
        grid(SDL_Renderer& renderer);

        int get_logical_width() const;

        int get_logical_height() const;

        void draw(SDL_Renderer& renderer);

        void set_next_input_items(const next_input_item_array& items);

        void set_input_items(const input_item_array& items);

        void set_board_items(const board_item_array& items);

    private:
        next_input_tile_array next_input_tiles_;
        input_tile_array input_tiles_;
        board_tile_array board_tiles_;
};

} //namespace view

#endif
