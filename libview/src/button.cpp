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

#include "button.hpp"
#include "text.hpp"
#include <MagnumPlugins/FreeTypeFont/FreeTypeFont.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Vector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/GlyphCache.h>

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

button::button
(
    const char* const label,
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
    text_renderer_.render(label);
}

void button::draw(const Magnum::Matrix3& transformationMatrix, SceneGraph::Camera2D& camera)
{
    using namespace Magnum::Math::Literals;

    get_square_shader().setColor(0xffffff_rgbf);
    get_square_shader().setTransformationProjectionMatrix
    (
        camera.projectionMatrix() *
        transformationMatrix
    );
    get_square_mesh().draw(get_square_shader());

    text::get_shader().bindVectorTexture(text::get_glyph_cache().texture());
    text::get_shader().setColor(0x444444_rgbf);
    text::get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformationMatrix);
    text_renderer_.mesh().draw(text::get_shader());
}

bool button::is_inside(const Magnum::Vector2& model_space_position) const
{
    const auto x = model_space_position.x();
    const auto y = model_space_position.y();
    return -1 <= x && x <= 1 && -1 <= y && y <= 1;
}

void button::mouse_press_event()
{
    mouse_press_callback_();
}

} //namespace
