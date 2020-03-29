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

#include "game_over_screen.hpp"
#include "text.hpp"
#include "colors.hpp"

namespace libview
{

class game_over_screen::new_game_button: public Object2D, public clickable
{
    public:
        using mouse_press_callback = std::function<void()>;

    public:
        explicit new_game_button
        (
            const mouse_press_callback& cb,
            SceneGraph::DrawableGroup2D& drawables,
            clickable_group& clickables,
            Object2D* parent
        ):
            Object2D{parent},
            clickable{*this, &clickables},
            mouse_press_callback_(cb),
            background_rectangle_(addChild<square>(colors::dark_gray, drawables)),
            label_(addChild<static_label>("NEW GAME", 0.4f, Magnum::Text::Alignment::MiddleCenter, drawables))
        {
            background_rectangle_.scale({1.5f, 0.35f});
            label_.set_color(colors::light_gray);
        }

        void set_enabled(const bool enabled)
        {
            enabled_ = enabled;
        }

    //clickable virtual functions
    private:
        bool is_inside(const Magnum::Vector2& model_space_position) const override
        {
            if(!enabled_)
            {
                return false;
            }

            const auto x = model_space_position.x();
            const auto y = model_space_position.y();
            return -1.5 <= x && x <= 1.5 && -0.35 <= y && y <= 0.35;
        }

        void mouse_press_event() override
        {
            mouse_press_callback_();
        }

    private:
        const mouse_press_callback mouse_press_callback_;
        square& background_rectangle_;
        static_label& label_;
        bool enabled_ = false;
};

game_over_screen::game_over_screen
(
    const std::function<void()>& new_game_button_press_callback,
    SceneGraph::DrawableGroup2D& drawables,
    clickable_group& clickables,
    Object2D* parent
):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    drawables_(drawables),
    background_rectangle_(addChild<square>(colors::light_gray, drawable_children_)),
    label_(addChild<static_label>("GAME OVER", 1.0f, Magnum::Text::Alignment::MiddleCenter, drawable_children_)),
    new_game_button_(addChild<new_game_button>(new_game_button_press_callback, drawable_children_, clickables))
{
    background_rectangle_.scale({50.0f, 1.0f});
    label_.set_color(colors::dark_gray);
    label_.set_outline_color(colors::dark_gray);
    label_.set_outline_range(0.47, 0.5);
    label_.translate({0.0f, 0.5f});

    new_game_button_.translate({0.0f, -0.5f});
}

void game_over_screen::set_visible(const bool visible)
{
    visible_ = visible;
    new_game_button_.set_enabled(visible);

    //bring to foreground
    drawables_.remove(*this);
    drawables_.add(*this);
}

void game_over_screen::draw(const Magnum::Matrix3& /*transformation_matrix*/, SceneGraph::Camera2D& camera)
{
    if(visible_)
    {
        camera.draw(drawable_children_);
    }
}

} //namespace
