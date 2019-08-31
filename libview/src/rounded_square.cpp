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

#include "rounded_square.hpp"
#include "shaders/flat_rounded_square.hpp"
#include <Magnum/GL/Mesh.h>

namespace libview
{

namespace
{
    Magnum::GL::Mesh& get_mesh()
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
                shaders::flat_rounded_square::Position{}
            );

            initialized = true;
        }

        return mesh;
    }

    shaders::flat_rounded_square& get_shader()
    {
        static shaders::flat_rounded_square shader;
        return shader;
    }
}

rounded_square::rounded_square(const Magnum::Color4& color, SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables},
    color_(color)
{
}

void rounded_square::set_color(const Magnum::Color4& color)
{
    color_ = color;
}

void rounded_square::draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera)
{
    get_shader().setColor(color_);
    get_shader().setTransformationProjectionMatrix
    (
        camera.projectionMatrix() *
        transformation_matrix
    );
    get_mesh().draw(get_shader());
}

} //namespace
