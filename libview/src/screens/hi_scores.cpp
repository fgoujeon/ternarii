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
#include "../objects/sdf_image.hpp"
#include "../objects/label_button.hpp"
#include "../styles.hpp"
#include "../colors.hpp"
#include "../common.hpp"
#include <libres.hpp>
#include <libutil/to_string.hpp>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <chrono>

namespace libview::screens
{

namespace
{
    int get_score
    (
        const hi_scores::score_map& scores,
        const data_types::stage stage
    )
    {
        const auto it = scores.find(stage);
        if(it != scores.end())
            return it->second;
        return 0;
    }

    int get_total_score(const hi_scores::score_map& scores)
    {
        int total = 0;
        for(const auto [stage, score]: scores)
            total += score;
        return total;
    }
}

struct hi_scores::impl
{
    impl
    (
        hi_scores& self,
        feature_group_set& feature_groups,
        const score_map& scores,
        const callback_set& callbacks
    ):
        self(self),
        feature_groups(feature_groups),
        scores(scores),
        callbacks(callbacks),
        title_label
        (
            self,
            feature_groups.drawables,
            styles::title_label,
            "HI-SCORES"
        ),
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

        logo.scale({3.0f, 3.0f});
        logo.set_alpha(0.1f);

        back_button.scale({2.0f, 2.0f});
        back_button.translate({0.0f, -7.0f});

        add_score(data_types::stage::purity_chapel, 3.25f);
        add_score(data_types::stage::nullifier_room, 2.25f);
        add_score(data_types::stage::math_classroom, 1.25f);
        add_score(data_types::stage::waterfalls, 0.25f);
        add_score(data_types::stage::granite_cave, -0.75f);
        add_score(data_types::stage::triplet_pines_mall, -1.75f);

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
            pname_label->translate({-3.5f, -3.25f});
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
                libutil::to_string(get_total_score(scores))
            );
            pscore_label->translate({3.5f, -3.25f});
            objects.push_back(pscore_label);
        }
    }

    void add_score
    (
        const data_types::stage stage,
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
            libutil::to_string(get_score(scores, stage))
        );
        pscore_label->translate({3.5f, y_position});
        objects.push_back(pscore_label);
    }

    object2d& self;
    feature_group_set& feature_groups;
    score_map scores;

    callback_set callbacks;

    objects::label title_label;
    objects::sdf_image logo;
    std::vector<std::shared_ptr<object2d>> objects;
    objects::label_button back_button;
};

hi_scores::hi_scores
(
    object2d& parent,
    feature_group_set& feature_groups,
    const score_map& scores,
    const callback_set& callbacks
):
    object2d{&parent},
    pimpl_(std::make_unique<impl>(*this, feature_groups, scores, callbacks))
{
}

hi_scores::~hi_scores() = default;

} //namespace
