#include "fixed_ratio_box.hpp"

namespace libview
{

fixed_ratio_box::fixed_ratio_box(const double ratio):
    ratio_(ratio)
{
}

void fixed_ratio_box::draw
(
    SDL_Renderer& renderer,
    const SDL_Rect& area
)
{
    const auto area_ratio = static_cast<double>(area.w) / area.h;

    SDL_Rect child_area;
    if(area_ratio > ratio_)
    {
        child_area.w = area.h * ratio_;
        child_area.h = area.h;
    }
    else
    {
        child_area.w = area.w;
        child_area.h = area.w / ratio_;
    }

    child_area.x = area.x + area.w / 2 - child_area.w / 2;
    child_area.y = area.y + area.h / 2 - child_area.h / 2;

    for(const auto& pchild: children_)
        pchild->draw(renderer, child_area);
}

void fixed_ratio_box::add(const std::shared_ptr<drawable>& child)
{
    children_.push_back(child);
}

} //namespace view
