/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBVIEW_COMMON_HPP
#define LIBVIEW_COMMON_HPP

#include "features/animable.hpp"
#include "features/clickable.hpp"
#include "features/key_event_handler.hpp"
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation2D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Magnum.h>

namespace libview
{

using camera = Magnum::SceneGraph::Camera2D;
using transformation_t = Magnum::SceneGraph::TranslationRotationScalingTransformation2D;

class object2d: public Magnum::SceneGraph::Object<transformation_t>
{
    public:
        object2d(object2d* pparent = nullptr):
            Magnum::SceneGraph::Object<transformation_t>(pparent),
            pparent_(pparent)
        {
        }

        virtual ~object2d() = default;

        float get_alpha() const
        {
            return alpha_;
        }

        float get_absolute_alpha() const
        {
            if(pparent_)
            {
                return alpha_ * pparent_->get_absolute_alpha();
            }

            return alpha_;
        }

        void set_alpha(const float value)
        {
            alpha_ = value;
        }

    private:
        object2d* pparent_ = nullptr;
        float alpha_ = 1.0f;
};

class scene: public object2d
{
    public:
        explicit scene() = default;

    private:
        bool isScene() const override final
        {
            return true;
        }
};

namespace features
{
    using drawable = Magnum::SceneGraph::Drawable2D;
    using drawable_group = Magnum::SceneGraph::DrawableGroup2D;
}

struct feature_group_set
{
    features::drawable_group drawables;
    features::animable_group animables;
    features::clickable_group clickables;
    features::key_event_handler_group key_event_handlers;
};

} //namespace

#endif
