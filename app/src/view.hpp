#ifndef VIEW_HPP
#define VIEW_HPP

#include "fixed_ratio_box.hpp"
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
        libsdl::session session_;
        libsdl::window window_;
        libsdl::renderer renderer_;
        fixed_ratio_box child_;
        bool quit_ = false;
};

#endif
