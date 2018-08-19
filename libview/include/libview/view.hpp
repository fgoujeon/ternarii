#ifndef LIBVIEW_VIEW_HPP
#define LIBVIEW_VIEW_HPP

#include <memory>

namespace libview
{

class view
{
    public:
        view();

        ~view();

        void set_window_size(const unsigned int width, const unsigned int height);

        void iterate();

        bool must_quit();

    private:
        void process_events();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace view

#endif
