#ifndef LIBVIEW_VIEW_HPP
#define LIBVIEW_VIEW_HPP

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

        bool must_quit();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace view

#endif