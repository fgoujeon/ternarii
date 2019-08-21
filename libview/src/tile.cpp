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

#include "tile.hpp"

namespace libview
{

namespace
{
    Magnum::Color3 value_to_color(const int value)
    {
        using namespace Magnum::Math::Literals;

        switch(value)
        {
            case 0:  return 0xa05252_rgbf;
            case 1:  return 0xdd3b3b_rgbf;
            case 2:  return 0xef7429_rgbf;
            case 3:  return 0xe5bb13_rgbf;
            case 4:  return 0xa1c927_rgbf;
            case 5:  return 0x2bcd73_rgbf;
            case 6:  return 0x25d7d2_rgbf;
            case 7:  return 0x3f94de_rgbf;
            case 8:  return 0x4c52e0_rgbf;
            case 9:  return 0x904ce0_rgbf;
            case 10: return 0xd84ce0_rgbf;
            case 11: return 0xe998ed_rgbf;
            case 12: return 0x989898_rgbf;
            case 13: return 0x461b3f_rgbf;
            default: return 0x000000_rgbf;
        }
    }

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
                {Magnum::Vector2{-0.43f,  0.43f}}, //A
                {Magnum::Vector2{-0.43f, -0.43f}}, //D
                {Magnum::Vector2{ 0.43f, -0.43f}}, //C
                {Magnum::Vector2{ 0.43f,  0.43f}}, //B
                {Magnum::Vector2{-0.43f,  0.43f}}, //A
                {Magnum::Vector2{ 0.43f, -0.43f}}, //C
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

    Magnum::Shaders::Flat2D& get_shader()
    {
        static Magnum::Shaders::Flat2D shader;
        return shader;
    }
}

tile::tile(SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    SceneGraph::Drawable2D{*this, &drawables}
{
}

void tile::set_value(const int value)
{
    value_ = value;
}

void tile::set_alpha(const float alpha)
{
    alpha_ = alpha;
}

void tile::draw(const Magnum::Matrix3& transformationMatrix, SceneGraph::Camera2D& camera)
{
    const auto color_no_alpha = value_to_color(value_);
    const auto color = Magnum::Color4{color_no_alpha, alpha_};
    get_shader().setColor(color);
    get_shader().setTransformationProjectionMatrix
    (
        camera.projectionMatrix() *
        transformationMatrix
    );
    get_mesh().draw(get_shader());
}

} //namespace
