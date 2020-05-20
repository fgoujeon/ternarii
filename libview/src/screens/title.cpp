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
#include "../objects/label_button.hpp"
#include "../objects/sdf_image.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

struct title::impl
{
    impl
    (
        title& self,
        feature_group_set& feature_groups,
        const callback_set& callbacks
    ):
        callbacks(callbacks),
        logo(self, feature_groups.drawables, "/res/images/logo.tga"),
        logo_text(self, feature_groups.drawables, "/res/images/logo_text.tga"),
        play_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            "PLAY",
            objects::label_button::style
            {
                .label_size = 0.2f
            },
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.play_request();}
            }
        ),
        about_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            "ABOUT",
            objects::label_button::style
            {
                .label_size = 0.2f
            },
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.about_request();}
            }
        )
    {
        logo.set_color(colors::light_gray);
        logo.set_outline_color(colors::dark_gray);
        logo.scale({2.5f, 2.5f});
        logo.translate({0.0f, 4.5f});

        logo_text.set_color(colors::light_gray);
        logo_text.set_outline_color(colors::dark_gray);
        logo_text.scale({2.0f, 2.0f});
        logo_text.translate({0.0f, 1.9f});

        play_button.scale({3.0f, 3.0f});
        play_button.translate({0.0f, -1.0f});

        about_button.scale({3.0f, 3.0f});
        about_button.translate({0.0f, -2.5f});
    }

    callback_set callbacks;

    objects::sdf_image logo;
    objects::sdf_image logo_text;
    objects::label_button play_button;
    objects::label_button about_button;
};

title::title
(
    Object2D& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    Object2D{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks))
{
}

title::~title() = default;

} //namespace
