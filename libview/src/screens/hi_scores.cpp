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

#include <libview/screens/hi_scores.hpp>
#include "../data_types.hpp"
#include "../objects/label_button.hpp"
#include "../styles.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libutil/to_string.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <chrono>

namespace libview::screens
{

struct hi_scores::impl
{
    impl
    (
        hi_scores& self,
        feature_group_set& feature_groups,
        const callback_set& callbacks
    ):
        self(self),
        feature_groups(feature_groups),
        callbacks(callbacks),
        title_label
        (
            self,
            feature_groups.drawables,
            styles::title_label,
            "HI-SCORES"
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
        title_label.translate({0.0f, 7.0f});

        back_button.scale({2.0f, 2.0f});
        back_button.translate({0.0f, -7.0f});

        add_score(data_types::stage::purity_chapel, 1234, 4.0f);
        add_score(data_types::stage::nullifier_room, 1234, 3.0f);
        add_score(data_types::stage::math_classroom, 1234567, 2.0f);
        add_score(data_types::stage::waterfalls, 1234567, 1.0f);
        add_score(data_types::stage::granite_cave, 1234567, 0.0f);
        add_score(data_types::stage::triplet_pines_mall, 1234567, -1.0f);

        //total score
        {
            auto pname_label = std::make_shared<objects::label>
            (
                self,
                feature_groups.drawables,
                objects::label::style
                {
                    .alignment = Magnum::Text::Alignment::MiddleLeft,
                    .color = colors::light_gray,
                    .font_size = 0.6f
                },
                "TOTAL"
            );
            pname_label->translate({-3.5f, -2.5f});
            objects.push_back(pname_label);

            auto pscore_label = std::make_shared<objects::label>
            (
                self,
                feature_groups.drawables,
                objects::label::style
                {
                    .alignment = Magnum::Text::Alignment::MiddleRight,
                    .color = colors::light_gray,
                    .font_size = 0.6f
                },
                libutil::to_string(3349986)
            );
            pscore_label->translate({3.5f, -2.5f});
            objects.push_back(pscore_label);
        }
    }

    void add_score
    (
        const data_types::stage stage,
        const int score,
        const float y_position
    )
    {
        auto pname_label = std::make_shared<objects::label>
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleLeft,
                .color = colors::light_gray,
                .font_size = 0.4f
            },
            data_types::get_pretty_name(stage)
        );
        pname_label->translate({-3.5f, y_position});
        objects.push_back(pname_label);

        auto pscore_label = std::make_shared<objects::label>
        (
            self,
            feature_groups.drawables,
            objects::label::style
            {
                .alignment = Magnum::Text::Alignment::MiddleRight,
                .color = colors::light_gray,
                .font_size = 0.4f
            },
            libutil::to_string(score)
        );
        pscore_label->translate({3.5f, y_position});
        objects.push_back(pscore_label);
    }

    object2d& self;
    feature_group_set& feature_groups;

    callback_set callbacks;

    objects::label title_label;
    std::vector<std::shared_ptr<object2d>> objects;
    objects::label_button back_button;
};

hi_scores::hi_scores
(
    object2d& parent,
    feature_group_set& feature_groups,
    const callback_set& callbacks
):
    object2d{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, callbacks))
{
}

hi_scores::~hi_scores() = default;

} //namespace
