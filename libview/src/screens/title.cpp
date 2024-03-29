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

#include <libview/screens/title.hpp>
#include "../objects/label_button.hpp"
#include "../objects/sdf_image.hpp"
#include "../colors.hpp"
#include "../styles.hpp"
#include "../common.hpp"
#include <libres.hpp>
#include <chrono>
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
        logo
        (
            self,
            feature_groups.drawables,
            libres::images::logo,
            objects::sdf_image::style
            {
                .color = colors::light_gray,
                .outline_color = colors::dark_gray,
                .outline_range = {0.5f, 0.4f}
            }
        ),
        logo_text
        (
            self,
            feature_groups.drawables,
            libres::images::logo_text,
            objects::sdf_image::style
            {
                .color = colors::light_gray,
                .outline_color = colors::dark_gray,
                .outline_range = {0.5f, 0.4f}
            }
        ),
        play_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            styles::white_label_button,
            "PLAY",
            objects::label_button::callback_set
            {
                .mouse_click_callback = [this]{this->callbacks.play_request();}
            }
        ),
        hi_scores_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            styles::white_label_button,
            "HI-SCORES",
            objects::label_button::callback_set
            {
                .mouse_click_callback = [this]{this->callbacks.hi_scores_request();}
            }
        ),
        about_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            styles::white_label_button,
            "ABOUT",
            objects::label_button::callback_set
            {
                .mouse_click_callback = [this]{this->callbacks.about_request();}
            }
        )
    {
        logo.set_color(colors::light_gray);
        logo.set_outline_color(colors::dark_gray);
        logo.scale({2.3f, 2.3f});
        logo.translate({0.0f, 4.5f});

        logo_text.set_color(colors::light_gray);
        logo_text.set_outline_color(colors::dark_gray);
        logo_text.scale({2.0f, 2.0f});
        logo_text.translate({0.0f, 1.9f});

        play_button.scale({2.8f, 2.8f});
        play_button.translate({0.0f, -1.0f});

        hi_scores_button.scale({2.8f, 2.8f});
        hi_scores_button.translate({0.0f, -2.5f});

        about_button.scale({2.8f, 2.8f});
        about_button.translate({0.0f, -4.0f});
    }

    callback_set callbacks;

    objects::sdf_image logo;
    objects::sdf_image logo_text;
    objects::label_button play_button;
    objects::label_button hi_scores_button;
    objects::label_button about_button;
};

title::title
(
    object2d& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    object2d{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks))
{
}

title::~title() = default;

} //namespace
