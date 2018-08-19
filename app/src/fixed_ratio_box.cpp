#include "fixed_ratio_box.hpp"

fixed_ratio_box::fixed_ratio_box(const double ratio):
    ratio_(ratio)
{
}

void fixed_ratio_box::draw
(
    libsdl::renderer& renderer,
    const rectangle& area
)
{
    const auto area_ratio = static_cast<double>(area.width_px) / area.height_px;

    rectangle child_area;
    if(area_ratio > ratio_)
    {
        child_area.width_px = area.height_px * ratio_;
        child_area.height_px = area.height_px;
    }
    else
    {
        child_area.width_px = area.width_px;
        child_area.height_px = area.width_px / ratio_;
    }

    child_area.pos_x_px = area.pos_x_px + area.width_px / 2 - child_area.width_px / 2;
    child_area.pos_y_px = area.pos_y_px + area.height_px / 2 - child_area.height_px / 2;

    for(const auto& pchild: children_)
        pchild->draw(renderer, child_area);
}

void fixed_ratio_box::add(const std::shared_ptr<drawable>& child)
{
    children_.push_back(child);
}
