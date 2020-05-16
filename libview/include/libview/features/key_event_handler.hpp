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

#ifndef LIBVIEW_FEATURES_KEY_EVENT_HANDLER_HPP
#define LIBVIEW_FEATURES_KEY_EVENT_HANDLER_HPP

#include <Magnum/SceneGraph/AbstractGroupedFeature.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::features
{

class key_event_handler: public Magnum::SceneGraph::AbstractGroupedFeature2D<key_event_handler>
{
    public:
        using key_event = Magnum::Platform::Sdl2Application::KeyEvent;

        using Magnum::SceneGraph::AbstractGroupedFeature2D<key_event_handler>::AbstractGroupedFeature2D;

        virtual void handle_key_press(key_event& event) = 0;
};

using key_event_handler_group = Magnum::SceneGraph::FeatureGroup2D<key_event_handler>;

} //namespace

#endif
