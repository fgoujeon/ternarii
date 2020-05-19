/*
Copyright 2018 - 2020 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBVIEW_FEATURES_CLICKABLE_HPP
#define LIBVIEW_FEATURES_CLICKABLE_HPP

#include <Magnum/SceneGraph/AbstractGroupedFeature.h>

namespace libview::features
{

class clickable: public Magnum::SceneGraph::AbstractGroupedFeature2D<clickable>
{
    public:
        using Magnum::SceneGraph::AbstractGroupedFeature2D<clickable>::AbstractGroupedFeature2D;

        virtual bool is_inside(const Magnum::Vector2& model_space_position) const = 0;

        virtual void handle_mouse_press(){}

        virtual void handle_mouse_release(){}
};

using clickable_group = Magnum::SceneGraph::FeatureGroup2D<clickable>;

} //namespace

#endif
