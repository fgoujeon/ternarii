#ifndef LIBVIEW_VIEW_HPP
#define LIBVIEW_VIEW_HPP

#include "item.hpp"
#include <functional>
#include <memory>

namespace libview
{

class view
{
    public:
        struct callback_set
        {
            std::function<void()> left_shift;
            std::function<void()> right_shift;
            std::function<void()> clockwise_rotation;
            std::function<void()> down;
        };

    public:
        view(const callback_set& callbacks);

        ~view();

        void set_window_size(const unsigned int width, const unsigned int height);

        void iterate();

        bool must_quit() const;

        void set_score(const unsigned int value);

        void create_next_input(const unsigned int value0, const unsigned int value1);

        void insert_next_input(const unsigned int x_offset, const unsigned int rotation);

        void set_next_input_items(const next_input_item_array& items);

        void set_input_items(const input_item_array& items);

        void set_input_x_offset(const unsigned int value);

        void set_input_rotation(const unsigned int value);

        void insert_input
        (
            const unsigned int tile0_dst_column_index,
            const unsigned int tile0_dst_row_index,
            const unsigned int tile1_dst_column_index,
            const unsigned int tile1_dst_row_index
        );

        void set_board_items(const board_item_array& items);

        void set_game_over_screen_visible(const bool visible);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace view

#endif
