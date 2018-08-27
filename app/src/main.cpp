#include "conversion.hpp"
#include <libgame/game.hpp>
#include <libview/view.hpp>

#ifdef EMSCRIPTEN
#include <emscripten/html5.h>
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>
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
            const auto& input_tiles = game_.get_input_tiles();
            const auto& next_input_tiles = game_.get_next_input_tiles();

            handle_game_event
            (
                libgame::events::next_input_creation
                {
                    {
                        input_tiles[0],
                        input_tiles[1]
                    }
                }
            );

            handle_game_event(libgame::events::next_input_insertion{2, 0});

            handle_game_event
            (
                libgame::events::next_input_creation
                {
                    {
                        next_input_tiles[0],
                        next_input_tiles[1]
                    }
                }
            );
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
                handle_game_events(game_.shift_input_left());
            };

            callbacks.right_shift = [this]
            {
                handle_game_events(game_.shift_input_right());
            };

            callbacks.clockwise_rotation = [this]
            {
                handle_game_events(game_.rotate_input());
            };

            callbacks.down = [this]
            {
                handle_game_events(game_.drop_input());
            };

            return callbacks;
        }

        template<class Event>
        void handle_game_event(const Event& event)
        {
        }

        void handle_game_event(const libgame::events::score_change& event)
        {
            view_.set_score(event.score);
        }

        void handle_game_event(const libgame::events::next_input_creation& event)
        {
            view_.create_next_input(event.tiles[0].value, event.tiles[1].value);
        }

        void handle_game_event(const libgame::events::next_input_insertion& event)
        {
            view_.insert_next_input(event.x_offset, event.rotation);
        }

        void handle_game_event(const libgame::events::input_layout_change& event)
        {
            view_.set_input_x_offset(event.x_offset);
            view_.set_input_rotation(event.rotation);
        }

        void handle_game_event(const libgame::events::input_insertion& event)
        {
            view_.insert_input
            (
                event.tile0_dst_column_index,
                event.tile0_dst_row_index,
                event.tile1_dst_column_index,
                event.tile1_dst_row_index
            );
        }

        void handle_game_event(const libgame::events::tile_drop& event)
        {
            view_.drop_tile
            (
                event.column_index,
                event.src_row_index,
                event.dst_row_index
            );
        }

        void handle_game_event(const libgame::events::element_transmutation& event)
        {
            view_.merge_tiles
            (
                conversion::to_view(event.src_tiles),
                conversion::to_view(event.dst_tile),
                event.dst_tile_value
            );
        }

        void handle_game_event(const libgame::events::end_of_game&)
        {
            view_.set_game_over_screen_visible(true);
        }

        void handle_game_events(const libgame::event_list& events)
        {
            for(const auto& event: events)
            {
                std::visit
                (
                    [this](const auto& event)
                    {
                        std::cout << event << '\n';
                        handle_game_event(event);
                    },
                    event
                );
            }
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
