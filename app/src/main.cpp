#include <libgame/game.hpp>
#include <libview/view.hpp>

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
#include <emscripten.h>
#endif

#include <iostream>
#include <cstdlib>
#include <cassert>

#ifdef EMSCRIPTEN
void iterate(void* arg)
{
    auto pv = reinterpret_cast<libview::view*>(arg);
    pv->iterate();
}

void sync_window_size_with_canvas_size(libview::view& v)
{
    double width, height;
    emscripten_get_element_css_size("canvas", &width, &height);
    v.set_window_size(width, height);
}

int on_canvas_resize(int, const EmscriptenUiEvent*, void* arg)
{
    auto* pctx = static_cast<libview::view*>(arg);
    auto& v = *pctx;
    sync_window_size_with_canvas_size(v);
    return 0;
}

void run(libview::view& v)
{
    sync_window_size_with_canvas_size(v);
    emscripten_set_resize_callback(nullptr, &v, false, on_canvas_resize);
    emscripten_set_main_loop_arg
    (
        iterate,
        &v,
        -1, // call the function as fast as the browser wants to render (typically 60fps)
        1 //call the function repeatedly
    );
}
#else
void run(libview::view& v)
{
    const unsigned int frame_duration = 1000 / 60;

    while(!v.must_quit())
    {
        const auto begin_time = SDL_GetTicks();
        v.iterate();
        const auto end_time = SDL_GetTicks();

        //regulate the frame rate
        assert(end_time >= begin_time);
        const auto elapsed_time = end_time - begin_time;
        if(elapsed_time < frame_duration)
        {
            const Uint32 remaining_time = frame_duration - elapsed_time;
            SDL_Delay(remaining_time);
        }
    }
}
#endif

class controller
{
    public:
        controller():
            view_(make_view_callbacks())
        {
            update_view();
        }

        void run()
        {
            ::run(view_);
        }

    private:
        libview::view::callback_set make_view_callbacks()
        {
            libview::view::callback_set callbacks;

            callbacks.left_shift = [this]
            {
                game_.shift_input_left();
                update_view_input();
            };

            callbacks.right_shift = [this]
            {
                game_.shift_input_right();
                update_view_input();
            };

            callbacks.clockwise_rotation = [this]
            {
                game_.rotate_input();
                update_view_input();
            };

            callbacks.down = [this]
            {
                game_.drop_input();
                update_view();
            };

            return callbacks;
        }

        void update_view_next_input()
        {
            libview::next_input_item_array view_items;
            const auto& game_items = game_.next_input_items();
            auto x = 0;
            for(const auto& pgame_item: game_items)
            {
                if(pgame_item)
                    view_items[x + 2][0] = libview::item{pgame_item->value};
                ++x;
            }
            view_.set_next_input_items(view_items);
        }

        std::optional<libview::item> to_view_item(const std::optional<std::shared_ptr<libgame::element>>& optpgame_item)
        {
            if(!optpgame_item)
                return std::nullopt;

            const auto& pgame_item = *optpgame_item;

            return libview::item{pgame_item->value};
        }

        void update_view_input()
        {
            libview::input_item_array view_items;
            const auto& game_items = game_.get_input_items();
            const auto x_offset = game_.get_input_x_offset();
            switch(game_.get_input_rotation())
            {
                case 0:
                    view_items[x_offset    ][0] = to_view_item(game_items[0]);
                    view_items[x_offset + 1][0] = to_view_item(game_items[1]);
                    break;
                case 1:
                    view_items[x_offset    ][1] = to_view_item(game_items[0]);
                    view_items[x_offset    ][0] = to_view_item(game_items[1]);
                    break;
                case 2:
                    view_items[x_offset + 1][0] = to_view_item(game_items[0]);
                    view_items[x_offset    ][0] = to_view_item(game_items[1]);
                    break;
                case 3:
                    view_items[x_offset    ][0] = to_view_item(game_items[0]);
                    view_items[x_offset    ][1] = to_view_item(game_items[1]);
                    break;
            }
            view_.set_input_items(view_items);
        }

        void update_view_board()
        {
            libview::board_item_array view_items;
            const auto& game_items = game_.board_items();

            for(auto x = 0; x < libgame::board_grid_t::column_count; ++x)
            {
                for(auto y = 0; y < libgame::board_grid_t::row_count; ++y)
                {
                    const auto& opt_pitem = game_items.at(x, y);
                    if(opt_pitem)
                    {
                        const auto pitem = *opt_pitem;
                        view_items[x][y] = libview::item{pitem->value};
                    }
                }
            }

            view_.set_board_items(view_items);
        }

        void update_view()
        {
            update_view_next_input();
            update_view_input();
            update_view_board();
        }

    private:
        libgame::game game_;
        libview::view view_;
};

int main(int, char**)
{
    std::srand(std::time(nullptr));

    controller c;
    c.run();
    return EXIT_SUCCESS;
}
