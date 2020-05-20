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

#include <libview/screens/about.hpp>
#include "../objects/label_button.hpp"
#include "../objects/sdf_image.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libutil/time.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>

namespace libview::screens
{

namespace
{
    constexpr auto body_label_str =
"Ternarii, a game by Florian Goujeon\n\n"
"Current version is " PROJECT_VERSION ", released on " PROJECT_DATE ".\n\n"
R"^(Ternarii is largely inspired by NaturalChimie, a fantastic,
soon-to-be discontinued game by Motion Twin.

Ternarii is powered by:
- Emscripten, by Emscripten contributors;
- Magnum and Corrade, by Vladimír Vondruš and contributors;
- JSON for Modern C++, by Niels Lohmann.

Ternarii is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later
version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with Ternarii.
If not, see https://www.gnu.org/licenses/.

Official website: http://ternarii.com
Source code: https://github.com/fgoujeon/ternarii

Copyright 2018 - 2020 Florian Goujeon)^";
}

struct about::impl
{
    impl
    (
        about& self,
        feature_group_set& feature_groups,
        const callback_set& callbacks
    ):
        callbacks(callbacks),
        title_label
        (
            self,
            feature_groups.drawables,
            "ABOUT",
            objects::static_label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleCenter,
                .font_size = 0.6f,
                .color = colors::light_gray,
                .outline_color = colors::dark_gray,
                .outline_range = {0.47f, 0.40f}
            }
        ),
        body_label
        (
            self,
            feature_groups.drawables,
            body_label_str,
            objects::static_label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleCenter,
                .font_size = 0.3f,
                .color = colors::light_gray,
                .outline_color = colors::dark_gray,
                .outline_range = {0.47f, 0.40f}
            }
        ),
        back_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            "BACK",
            objects::label_button::style
            {
                .label = objects::static_label::style
                {
                    .alignment = Magnum::Text::Alignment::MiddleCenter,
                    .font_size = 0.2f,
                    .color = colors::dark_gray,
                    .outline_color = colors::dark_gray,
                    .outline_range = {0.6f, 0.5f}
                },
                .color = colors::light_gray,
                .highlight_color = colors::white,
                .outline_color = colors::dark_gray
            },
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.back_request();}
            }
        )
    {
        title_label.translate({0.0f, 7.0f});

        back_button.scale({2.0f, 2.0f});
        back_button.translate({0.0f, -7.0f});
    }

    callback_set callbacks;

    objects::static_label title_label;
    objects::static_label body_label;
    objects::label_button back_button;
};

about::about
(
    Object2D& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    Object2D{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks))
{
}

about::~about() = default;

} //namespace
