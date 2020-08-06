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
                Magnum::Primitives::SquareFlag::TextureCoordinates
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

    /*
    We want the object scaling to be applied to the original PNG image
    (i.e. WITHOUT the distance field). To do so, we must upscale the object
    by the following factor.
    */
    constexpr auto original_size = static_cast<float>(TERNARII_SDF_IMAGE_PNG_SIZE);
    constexpr auto radius = static_cast<float>(TERNARII_SDF_IMAGE_RADIUS);
    constexpr auto scaling_factor = (original_size + 2 * radius) / original_size;
    constexpr auto scaling_matrix = Magnum::Matrix3::scaling({scaling_factor, scaling_factor});
}

sdf_image::sdf_image
(
    object2d& parent,
    features::drawable_group& drawables,
    const std::filesystem::path& image_path,
    const style& stl
):
    object2d{&parent},
    features::drawable{*this, &drawables},
    style_(stl),
    texture_(make_texture(image_path))
{
}

void sdf_image::set_color(const Magnum::Color4& color)
{
    style_.color = color;
}

void sdf_image::set_outline_color(const Magnum::Color4& color)
{
    style_.outline_color = color;
}

void sdf_image::draw(const Magnum::Matrix3& transformation_matrix, camera& camera)
{
    const auto absolute_alpha = get_absolute_alpha();

    get_shader().setColor(style_.color * absolute_alpha);
    get_shader().bindVectorTexture(texture_);
    get_shader().setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix * scaling_matrix);
    get_shader().setSmoothness(0.15f / transformation_matrix.uniformScaling());
    get_shader().setOutlineColor(style_.outline_color * absolute_alpha);
    get_shader().setOutlineRange(style_.outline_range[0], style_.outline_range[1]);
    get_shader().draw(get_mesh());
}

} //namespace
