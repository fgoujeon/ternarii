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

#ifndef LIBVIEW_FEATURES_ANIMABLE_HPP
#define LIBVIEW_FEATURES_ANIMABLE_HPP

#include <Magnum/SceneGraph/AbstractGroupedFeature.h>
#include <libutil/time.hpp>

namespace libview::features
{

class animable: public Magnum::SceneGraph::AbstractGroupedFeature2D<animable>
{
    public:
        using Magnum::SceneGraph::AbstractGroupedFeature2D<animable>::AbstractGroupedFeature2D;

        virtual void advance(const libutil::time::point& now, float elapsed_s) = 0;
};

using animable_group = Magnum::SceneGraph::FeatureGroup2D<animable>;

} //namespace

#endif
