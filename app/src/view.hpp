#ifndef VIEW_HPP
#define VIEW_HPP

#include "libsdl.hpp"

class view
{
    public:
        view();

        void set_window_size(const unsigned int width, const unsigned int height);

        void iterate();

        bool must_quit();

    private:
        void process_events();

    private:
        libsdl::session session;
        libsdl::window window;
        libsdl::renderer renderer;
        bool quit = false;
};

#endif
