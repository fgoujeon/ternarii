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

#include <libview/screens/title.hpp>
#include "../objects/background.hpp"
#include "../objects/game_over_screen.hpp"
#include "../objects/button.hpp"
#include "../objects/sdf_image.hpp"
#include "../objects/tile_grid.hpp"
#include "../objects/score_display.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libview/events.hpp>
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

class title::impl
{
    public:
        impl
        (
            title& self,
            features::clickable_group& clickables,
            const callback_set& callbacks
        ):
            callbacks_(callbacks),
            logo_(self, drawables_, "/res/images/logo.tga"),
            logo_text_(self, drawables_, "/res/images/logo_text.tga"),
            drop_button_(self, drawables_, clickables, "/res/images/move_button.tga", [this]{callbacks_.play_request();})
        {
            logo_.set_color(colors::light_gray);
            logo_.set_outline_color(colors::dark_gray);
            logo_.scale({1.5f, 1.5f});
            logo_.translate({0.0f, 4.5f});

            logo_text_.set_color(colors::light_gray);
            logo_text_.set_outline_color(colors::dark_gray);
            logo_text_.scale({1.4f, 1.4f});
            logo_text_.translate({0.0f, 2.9f});

            drop_button_.rotate(180.0_degf);
            drop_button_.translate({0.0f, 0.0f});
        }

    public:
        callback_set callbacks_;

        features::drawable_group drawables_;

        objects::sdf_image logo_;
        objects::sdf_image logo_text_;
        objects::button drop_button_;
};

title::title
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& /*animables*/,
    features::clickable_group& clickables,
    features::key_event_handler_group& /*key_event_handlers*/,
    const callback_set& callbacks
):
    Object2D{&parent},
    features::drawable{*this, &drawables},
    pimpl_(std::make_unique<impl>(*this, clickables, callbacks))
{
}

title::~title() = default;

void title::draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera)
{
    camera.draw(pimpl_->drawables_);
}

} //namespace
