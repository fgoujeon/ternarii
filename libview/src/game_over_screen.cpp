/*
Copyright 2018 - 2019 Florian Goujeon

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
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>

namespace libview
{

namespace
{
    Magnum::GL::Mesh& get_square_mesh()
    {
        static Magnum::GL::Mesh mesh;
        static bool initialized = false;

        if(!initialized)
        {
            struct vertex
            {
                Magnum::Vector2 position;
            };

            /*
            A---B
            |   |
            D---C
            */
            const vertex data[]
            {
                {Magnum::Vector2{-1.0f,  1.0f}}, //A
                {Magnum::Vector2{-1.0f, -1.0f}}, //D
                {Magnum::Vector2{ 1.0f, -1.0f}}, //C
                {Magnum::Vector2{ 1.0f,  1.0f}}, //B
                {Magnum::Vector2{-1.0f,  1.0f}}, //A
                {Magnum::Vector2{ 1.0f, -1.0f}}, //C
            };
            Magnum::GL::Buffer buffer;
            buffer.setData(data, Magnum::GL::BufferUsage::StaticDraw);

            mesh.setCount(6);
            mesh.addVertexBuffer
            (
                std::move(buffer),
                0,
                Magnum::Shaders::Flat2D::Position{}
            );

            initialized = true;
        }

        return mesh;
    }

    Magnum::Shaders::Flat2D& get_square_shader()
    {
        static Magnum::Shaders::Flat2D shader;
        return shader;
    }
}

class game_over_screen::new_game_button: public Object2D, public SceneGraph::Drawable2D, public clickable
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
            SceneGraph::Drawable2D{*this, &drawables},
            clickable{*this, &clickables},
            mouse_press_callback_(cb),
            text_renderer_(text::get_font(), text::get_glyph_cache(), 0.5f, Magnum::Text::Alignment::MiddleCenter)
        {
            text_renderer_.reserve(10, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
            text_renderer_.render("NEW GAME");
        }

        void set_enabled(const bool enabled)
        {
            enabled_ = enabled;
        }

    //Drawable2D virtual functions
    private:
        void draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera) override
        {
            using namespace Magnum::Math::Literals;

            get_square_shader().setColor(0x444444_rgbf);
            get_square_shader().setTransformationProjectionMatrix
            (
                camera.projectionMatrix() *
                transformation_matrix *
                Magnum::Matrix3::scaling({1.5f, 0.35f})
            );
            get_square_mesh().draw(get_square_shader());

            text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
            text::get_shader().setColor(0xffffff_rgbf);
            text::get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
            text_renderer_.mesh().draw(text::get_shader());
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
        Magnum::Text::Renderer2D text_renderer_;
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
    text_renderer_(text::get_font(), text::get_glyph_cache(), 1.0f, Magnum::Text::Alignment::MiddleCenter),
    new_game_button_(addChild<new_game_button>(new_game_button_press_callback, internal_drawables_, clickables))
{
    text_renderer_.reserve(10, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);
    text_renderer_.render("GAME OVER");

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

void game_over_screen::draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera)
{
    if(visible_)
    {
        using namespace Magnum::Math::Literals;

        //background
        get_square_shader().setColor(0xffffff_rgbf);
        get_square_shader().setTransformationProjectionMatrix
        (
            camera.projectionMatrix() *
            transformation_matrix *
            Magnum::Matrix3::scaling({50.0f, 1.0f})
        );
        get_square_mesh().draw(get_square_shader());

        //"GAME OVER" text
        text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
        text::get_shader().setTransformationProjectionMatrix
        (
            camera.projectionMatrix() *
            transformation_matrix *
            Magnum::Matrix3::translation({0.0f, 0.5f})
        );
        text::get_shader().setColor(0x444444_rgbf);
        text_renderer_.mesh().draw(text::get_shader());

        camera.draw(internal_drawables_);
    }
}

} //namespace
