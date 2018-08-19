#ifndef FIXED_RATIO_BOX_HPP
#define FIXED_RATIO_BOX_HPP

#include "drawable.hpp"
#include "libsdl.hpp"
#include <memory>
#include <vector>

class fixed_ratio_box: public drawable
{
    public:
        fixed_ratio_box(const double ratio); //width / height

        void draw
        (
            libsdl::renderer& renderer,
            const rectangle& area
        );

        void add(const std::shared_ptr<drawable>& child);

    private:
        const double ratio_;
        std::vector<std::shared_ptr<drawable>> children_;
};

#endif
