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

#include <libview/screens/stage_selection.hpp>
#include "../objects/label_button.hpp"
#include "../objects/sdf_image_tile.hpp"
#include "../colors.hpp"
#include "../styles.hpp"
#include "../common.hpp"

namespace libview::screens
{

struct stage_selection::impl
{
    impl
    (
        stage_selection& self,
        feature_group_set& feature_groups,
        const callback_set& callbacks
    ):
        callbacks(callbacks),
        title_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleCenter,
                .font_size = 0.6f,
                .color = colors::light_gray,
                .outline_color = colors::dark_gray,
                .outline_range = {0.47f, 0.40f}
            },
            "SELECT STAGE"
        ),
        purity_room_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            styles::white_label_button,
            "PURITY ROOM",
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.purity_room_selection_request();}
            }
        ),
        purity_room_label
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleCenter,
                .font_size = 0.3f,
                .color = colors::light_gray,
                .outline_color = colors::dark_gray,
                .outline_range = {0.47f, 0.40f}
            },
            "No special tiles."
        ),
        nullifier_room_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            styles::white_label_button,
            "NULLIFIER ROOM",
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.nullifier_room_selection_request();}
            }
        ),
        nullifier_room_special_tile_0
        (
            self,
            feature_groups.drawables,
            "/res/images/row_nullifier.tga"
        ),
        nullifier_room_special_tile_1
        (
            self,
            feature_groups.drawables,
            "/res/images/column_nullifier.tga"
        ),
        nullifier_room_special_tile_2
        (
            self,
            feature_groups.drawables,
            "/res/images/number_nullifier.tga"
        ),
        back_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            styles::white_label_button,
            "BACK",
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.back_request();}
            }
        )
    {
        title_label.translate({0.0f, 7.0f});

        purity_room_button.scale({2.0f, 2.0f});
        purity_room_button.translate({0.0f, 2.5f});

        purity_room_label.translate({0.0f, 1.5f});

        nullifier_room_button.scale({2.0f, 2.0f});
        nullifier_room_button.translate({0.0f, -1.5f});

        nullifier_room_special_tile_0.scale({0.3f, 0.3f});
        nullifier_room_special_tile_0.translate({-1.0f, -2.5f});

        nullifier_room_special_tile_1.scale({0.3f, 0.3f});
        nullifier_room_special_tile_1.translate({-0.0f, -2.5f});

        nullifier_room_special_tile_2.scale({0.3f, 0.3f});
        nullifier_room_special_tile_2.translate({1.0f, -2.5f});

        back_button.scale({2.0f, 2.0f});
        back_button.translate({0.0f, -7.0f});
    }

    callback_set callbacks;

    objects::label title_label;

    objects::label_button purity_room_button;
    objects::label purity_room_label;

    objects::label_button nullifier_room_button;
    objects::sdf_image_tile nullifier_room_special_tile_0;
    objects::sdf_image_tile nullifier_room_special_tile_1;
    objects::sdf_image_tile nullifier_room_special_tile_2;

    objects::label_button back_button;
};

stage_selection::stage_selection
(
    Object2D& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    Object2D{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks))
{
}

stage_selection::~stage_selection() = default;

} //namespace
