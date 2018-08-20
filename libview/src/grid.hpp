#ifndef LIBVIEW_GRID_HPP
#define LIBVIEW_GRID_HPP

#include "drawable.hpp"
#include "rectangle.hpp"
#include "libsdl.hpp"
#include <libview/item.hpp>

namespace libview
{

class grid: public drawable
{
    public:
        grid();

        void draw
        (
            SDL_Renderer& renderer,
            const rectangle& area
        );

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
        next_input_item_array next_input_items;
        input_item_array input_items;
        board_item_array board_items;
};

} //namespace view

#endif
