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

#ifndef LIBVIEW_FEATURES_CLICKABLE_HPP
#define LIBVIEW_FEATURES_CLICKABLE_HPP

#include <Magnum/SceneGraph/AbstractGroupedFeature.h>

namespace libview::features
{

class clickable: public Magnum::SceneGraph::AbstractGroupedFeature2D<clickable>
{
    public:
        using Magnum::SceneGraph::AbstractGroupedFeature2D<clickable>::AbstractGroupedFeature2D;

        void handle_mouse_move(const Magnum::Vector2& model_space_position)
        {
            const auto was_inside = is_inside_;
            is_inside_ = do_is_inside(model_space_position);

            if(!was_inside && is_inside_)
            {
                do_handle_mouse_enter();
            }
            else if(was_inside && !is_inside_)
            {
                do_handle_mouse_leave();

                if(pressed_)
                {
                    do_handle_mouse_release();
                    pressed_ = false;
                }
            }
        }

        void handle_mouse_press(const Magnum::Vector2& model_space_position)
        {
            is_inside_ = do_is_inside(model_space_position);

            if(is_inside_)
            {
                pressed_ = true;

                do_handle_mouse_press();
            }
        }

        void handle_mouse_release(const Magnum::Vector2& model_space_position)
        {
            is_inside_ = do_is_inside(model_space_position);

            if(is_inside_)
            {
                do_handle_mouse_release();

                if(pressed_)
                {
                    do_handle_mouse_click();
                }

                pressed_ = false;
            }
        }

    protected:
        virtual bool do_is_inside(const Magnum::Vector2& model_space_position) const = 0;

        virtual void do_handle_mouse_enter(){}

        virtual void do_handle_mouse_leave(){}

        virtual void do_handle_mouse_press(){}

        virtual void do_handle_mouse_release(){}

        virtual void do_handle_mouse_click(){}

    private:
        bool is_inside_ = false;
        bool pressed_ = false;
};

using clickable_group = Magnum::SceneGraph::FeatureGroup2D<clickable>;

} //namespace

#endif
