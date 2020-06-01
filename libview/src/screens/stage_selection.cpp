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
#include "../objects/blank_button.hpp"
#include "../objects/label.hpp"
#include "../objects/sdf_image_tile.hpp"
#include "../colors.hpp"
#include "../styles.hpp"
#include "../common.hpp"
#include <libres.hpp>

namespace libview::screens
{

namespace
{
    const auto stage_button_style = objects::blank_button::style
    {
        .color = colors::light_gray,
        .highlight_color = colors::white,
        .radius = 0.16f,
        .scaling = {1.0f, 0.6f}
    };

    const auto stage_name_label_style = objects::label::style
    {
        .alignment = Magnum::Text::Alignment::MiddleCenter,
        .color = colors::dark_gray,
        .font_size = 0.4f,
        .outline_range = {0.5f, 0.5f}
    };

    const auto stage_description_label_style = objects::label::style
    {
        .alignment = Magnum::Text::Alignment::MiddleCenter,
        .color = colors::dark_gray,
        .font_size = 0.3f,
        .outline_range = {0.5f, 0.5f}
    };
}

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
            styles::title_label,
            "SELECT STAGE"
        ),
        purity_room_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            stage_button_style,
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.purity_room_selection_request();}
            }
        ),
        purity_room_name_label
        (
            self,
            feature_groups.drawables,
            stage_name_label_style,
            "PURITY ROOM"
        ),
        purity_room_description_label
        (
            self,
            feature_groups.drawables,
            stage_description_label_style,
            "(no special tiles)"
        ),
        nullifier_room_button
        (
            self,
            feature_groups.drawables,
            feature_groups.clickables,
            stage_button_style,
            objects::label_button::callback_set
            {
                .mouse_release_callback = [this]{this->callbacks.nullifier_room_selection_request();}
            }
        ),
        nullifier_room_name_label
        (
            self,
            feature_groups.drawables,
            stage_name_label_style,
            "NULLIFIER ROOM"
        ),
        nullifier_room_special_tile_0
        (
            self,
            feature_groups.drawables,
            libres::images::row_nullifier
        ),
        nullifier_room_special_tile_1
        (
            self,
            feature_groups.drawables,
            libres::images::column_nullifier
        ),
        nullifier_room_special_tile_2
        (
            self,
            feature_groups.drawables,
            libres::images::number_nullifier
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

        {
            purity_room_button.scale({2.0f, 2.0f});
            purity_room_button.translate({0.0f, 2.5f});

            purity_room_name_label.translate({0.0f, 3.0f});

            purity_room_description_label.translate({0.0f, 2.0f});
        }

        {
            nullifier_room_button.scale({2.0f, 2.0f});
            nullifier_room_button.translate({0.0f, -1.5f});

            nullifier_room_name_label.translate({0.0f, -1.0f});

            nullifier_room_special_tile_0.scale({0.4f, 0.4f});
            nullifier_room_special_tile_0.translate({-1.0f, -2.0f});

            nullifier_room_special_tile_1.scale({0.4f, 0.4f});
            nullifier_room_special_tile_1.translate({-0.0f, -2.0f});

            nullifier_room_special_tile_2.scale({0.4f, 0.4f});
            nullifier_room_special_tile_2.translate({1.0f, -2.0f});
        }

        back_button.scale({2.0f, 2.0f});
        back_button.translate({0.0f, -7.0f});
    }

    callback_set callbacks;

    objects::label title_label;

    objects::blank_button purity_room_button;
    objects::label purity_room_name_label;
    objects::label purity_room_description_label;

    objects::blank_button nullifier_room_button;
    objects::label nullifier_room_name_label;
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
