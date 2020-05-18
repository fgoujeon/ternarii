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

#include "sdf_image.hpp"
#include <MagnumPlugins/TgaImporter/TgaImporter.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Trade/MeshData2D.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/ImageView.h>
#include <Corrade/Containers/Optional.h>

namespace libview::objects
{

namespace
{
    Magnum::GL::Mesh& get_mesh()
    {
        static Magnum::GL::Mesh mesh = Magnum::MeshTools::compile
        (
            Magnum::Primitives::squareSolid
            (
                Magnum::Primitives::SquareTextureCoords::Generate
            )
        );
        return mesh;
    }

    Magnum::Shaders::DistanceFieldVector2D& get_shader()
    {
        static Magnum::Shaders::DistanceFieldVector2D shader;
        return shader;
    }

    auto make_texture(const std::filesystem::path& image_path)
    {
        auto importer = Magnum::Trade::TgaImporter{};
        importer.openFile(image_path);

        auto optImage = importer.image2D(0);
        const auto& image = *optImage;

        auto texture = Magnum::GL::Texture2D{};
        texture.setMagnificationFilter(Magnum::GL::SamplerFilter::Linear);
        texture.setMinificationFilter(Magnum::GL::SamplerFilter::Linear);
        texture.setWrapping(Magnum::GL::SamplerWrapping::ClampToEdge);
        texture.setStorage(1, Magnum::GL::TextureFormat::Luminance, image.size());
        texture.setSubImage(0, {}, image);

        return texture;
    }
}

sdf_image::sdf_image(Object2D& parent, features::drawable_group& drawables, const std::filesystem::path& image_path):
    Object2D{&parent},
    features::drawable{*this, &drawables},
    texture_(make_texture(image_path))
{
}

void sdf_image::set_color(const Magnum::Color4& color)
{
    color_ = color;
}

void sdf_image::set_outline_color(const Magnum::Color4& color)
{
    outline_color_ = color;
}

void sdf_image::draw(const Magnum::Matrix3& transformation_matrix, SceneGraph::Camera2D& camera)
{
    get_shader().setColor(color_);
    get_shader().bindVectorTexture(texture_);
    get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
    get_shader().setSmoothness(0.24f / transformation_matrix.uniformScaling());
    get_shader().setOutlineColor(outline_color_);
    get_shader().setOutlineRange(0.50, 0.40);
    get_mesh().draw(get_shader());
}

} //namespace
