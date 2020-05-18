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
#include "../objects/sdf_image_button.hpp"
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

namespace
{
    constexpr auto version_text = "version " PROJECT_VERSION " (" PROJECT_DATE ")";
}

struct title::impl
{
    impl
    (
        title& self,
        features::clickable_group& clickables,
        const callback_set& callbacks
    ):
        callbacks(callbacks),
        logo(self, drawables, "/res/images/logo.tga"),
        logo_text(self, drawables, "/res/images/logo_text.tga"),
        drop_button(self, drawables, clickables, "/res/images/move_button.tga", [this]{this->callbacks.play_request();}),
        version_label(self, drawables, version_text, 0.2, Magnum::Text::Alignment::MiddleRight)
    {
        logo.set_color(colors::light_gray);
        logo.set_outline_color(colors::dark_gray);
        logo.scale({2.0f, 2.0f});
        logo.translate({0.0f, 4.5f});

        logo_text.set_color(colors::light_gray);
        logo_text.set_outline_color(colors::dark_gray);
        logo_text.scale({1.8f, 1.8f});
        logo_text.translate({0.0f, 2.3f});

        drop_button.rotate(180.0_degf);
        drop_button.translate({0.0f, 0.0f});

        version_label.set_color(colors::light_gray);
        version_label.set_outline_color(colors::dark_gray);
        version_label.set_outline_range(0.47, 0.40);
        version_label.translate({4.0f, -7.0f});
    }

    callback_set callbacks;

    features::drawable_group drawables;

    objects::sdf_image logo;
    objects::sdf_image logo_text;
    objects::sdf_image_button drop_button;
    objects::static_label version_label;
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
    camera.draw(pimpl_->drawables);
}

} //namespace
