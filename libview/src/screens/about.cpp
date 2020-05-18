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
#include <libview/events.hpp>
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
        title_label(self, feature_groups.drawables, "ABOUT", 0.6, Magnum::Text::Alignment::MiddleCenter),
        body_label(self, feature_groups.drawables, body_label_str, 0.3, Magnum::Text::Alignment::MiddleCenter),
        back_button(self, feature_groups.drawables, feature_groups.clickables, "BACK", [this]{this->callbacks.back_request();})
    {
        title_label.set_color(colors::light_gray);
        title_label.set_outline_color(colors::dark_gray);
        title_label.set_outline_range(0.47, 0.40);
        title_label.translate({0.0f, 7.0f});

        body_label.set_color(colors::light_gray);
        body_label.set_outline_color(colors::dark_gray);
        body_label.set_outline_range(0.47, 0.40);
        body_label.translate({0.0f, 0.0f});

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
