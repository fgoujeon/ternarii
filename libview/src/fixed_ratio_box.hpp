#ifndef LIBVIEW_FIXED_RATIO_BOX_HPP
#define LIBVIEW_FIXED_RATIO_BOX_HPP

#include "drawable.hpp"
#include "libsdl.hpp"
#include <memory>
#include <vector>

namespace libview
{

class fixed_ratio_box: public drawable
{
    public:
        fixed_ratio_box(const double ratio); //width / height

        void draw
        (
            SDL_Renderer& renderer,
            const rectangle& area
        );

        void add(const std::shared_ptr<drawable>& child);

    private:
        const double ratio_;
        std::vector<std::shared_ptr<drawable>> children_;
};

} //namespace view

#endif
