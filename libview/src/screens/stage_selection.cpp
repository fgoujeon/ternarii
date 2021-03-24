/*
Copyright 2018 - 2021 Florian Goujeon

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
#include "../data_types.hpp"
#include "../common.hpp"
#include <libres.hpp>

namespace libview::screens
{

namespace
{
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

    class selection_button: public object2d
    {
        private:
            static std::unique_ptr<objects::sdf_image> make_stage_image
            (
                selection_button& parent,
                feature_group_set& feature_groups,
                const data_types::stage stage
            )
            {
                const auto opt_path = data_types::get_image(stage);
                if(!opt_path)
                {
                    return nullptr;
                }

                auto pimage = std::make_unique<objects::sdf_image>
                (
                    parent,
                    feature_groups.drawables,
                    *opt_path,
                    objects::sdf_image::style
                    {
                        .color = colors::black
                    }
                );

                pimage->setScaling({0.75f, 0.75f});
                pimage->setTranslation({-2.25f, 0.0f});

                return pimage;
            }

        public:
            selection_button
            (
                object2d& parent,
                feature_group_set& feature_groups,
                const data_types::stage stage,
                const stage_selection::callback_set& callbacks
            ):
                object2d(&parent),
                button_
                (
                    *this,
                    feature_groups.drawables,
                    feature_groups.clickables,
                    objects::blank_button::style
                    {
                        .color = colors::light_gray,
                        .dimension = {1.0f, 0.33f},
                        .highlight_color = colors::white,
                        .radius = 0.16f
                    },
                    objects::label_button::callback_set
                    {
                        .mouse_click_callback = [callbacks, stage]
                        {
                            callbacks.stage_selection_request(stage);
                        }
                    }
                ),
                name_label_
                (
                    *this,
                    feature_groups.drawables,
                    stage_name_label_style,
                    data_types::get_pretty_name(stage)
                ),
                pstage_image_
                (
                    make_stage_image(*this, feature_groups, stage)
                )
            {
                button_.setScaling({3.3f, 3.3f});
                name_label_.setTranslation({0.75f, 0.5f});
            }

        private:
            objects::blank_button button_;
            objects::label name_label_;
            std::unique_ptr<objects::sdf_image> pstage_image_;
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

        //Purity chapel
        purity_chapel_button
        (
            self,
            feature_groups,
            data_types::stage::purity_chapel,
            callbacks
        ),
        purity_chapel_description_label
        (
            purity_chapel_button,
            feature_groups.drawables,
            stage_description_label_style,
            "(no special tiles)"
        ),

        //Nullifier room
        nullifier_room_button
        (
            self,
            feature_groups,
            data_types::stage::nullifier_room,
            callbacks
        ),
        nullifier_room_special_tile_0
        (
            nullifier_room_button,
            feature_groups.drawables,
            libres::images::row_nullifier
        ),
        nullifier_room_special_tile_1
        (
            nullifier_room_button,
            feature_groups.drawables,
            libres::images::column_nullifier
        ),
        nullifier_room_special_tile_2
        (
            nullifier_room_button,
            feature_groups.drawables,
            libres::images::number_nullifier
        ),

        //Triplet pines mall
        triplet_pines_mall_button
        (
            self,
            feature_groups,
            data_types::stage::triplet_pines_mall,
            callbacks
        ),
        triplet_pines_mall_tile_triplet
        (
            triplet_pines_mall_button,
            feature_groups.drawables,
            libres::images::tile_triplet,
            objects::sdf_image::style
            {
                .color = colors::black
            }
        ),
        triplet_pines_mall_special_tile_0
        (
            triplet_pines_mall_button,
            feature_groups.drawables,
            libres::images::row_nullifier
        ),
        triplet_pines_mall_special_tile_1
        (
            triplet_pines_mall_button,
            feature_groups.drawables,
            libres::images::column_nullifier
        ),
        triplet_pines_mall_special_tile_2
        (
            triplet_pines_mall_button,
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
                .mouse_click_callback = [this]{this->callbacks.back_request();}
            }
        )
    {
        title_label.setTranslation({0.0f, 7.0f});

        {
            purity_chapel_button.setTranslation({0.0f, 3.0f});

            purity_chapel_description_label.setTranslation({0.75f, -0.4f});
        }

        {
            nullifier_room_button.setTranslation({0.0f, 0.0f});

            nullifier_room_special_tile_0.setScaling({0.4f, 0.4f});
            nullifier_room_special_tile_0.setTranslation({-0.25f, -0.4f});

            nullifier_room_special_tile_1.setScaling({0.4f, 0.4f});
            nullifier_room_special_tile_1.setTranslation({0.75f, -0.4f});

            nullifier_room_special_tile_2.setScaling({0.4f, 0.4f});
            nullifier_room_special_tile_2.setTranslation({1.75f, -0.4f});
        }

        {
            triplet_pines_mall_button.setTranslation({0.0f, -3.0f});

            triplet_pines_mall_tile_triplet.setScaling({0.4f, 0.4f});
            triplet_pines_mall_tile_triplet.setTranslation({-0.75f, -0.4f});

            triplet_pines_mall_special_tile_0.setScaling({0.4f, 0.4f});
            triplet_pines_mall_special_tile_0.setTranslation({0.25f, -0.4f});

            triplet_pines_mall_special_tile_1.setScaling({0.4f, 0.4f});
            triplet_pines_mall_special_tile_1.setTranslation({1.25f, -0.4f});

            triplet_pines_mall_special_tile_2.setScaling({0.4f, 0.4f});
            triplet_pines_mall_special_tile_2.setTranslation({2.25f, -0.4f});
        }

        back_button.setScaling({2.0f, 2.0f});
        back_button.setTranslation({0.0f, -7.0f});
    }

    callback_set callbacks;

    objects::label title_label;

    selection_button purity_chapel_button;
    objects::label purity_chapel_description_label;

    selection_button nullifier_room_button;
    objects::sdf_image_tile nullifier_room_special_tile_0;
    objects::sdf_image_tile nullifier_room_special_tile_1;
    objects::sdf_image_tile nullifier_room_special_tile_2;

    selection_button triplet_pines_mall_button;
    objects::sdf_image triplet_pines_mall_tile_triplet;
    objects::sdf_image_tile triplet_pines_mall_special_tile_0;
    objects::sdf_image_tile triplet_pines_mall_special_tile_1;
    objects::sdf_image_tile triplet_pines_mall_special_tile_2;

    objects::label_button back_button;
};

stage_selection::stage_selection
(
    object2d& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    object2d{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks))
{
}

stage_selection::~stage_selection() = default;

} //namespace
