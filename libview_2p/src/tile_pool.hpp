/*
Copyright 2018 Florian Goujeon

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

#ifndef LIBVIEW_2P_TILE_POOL_HPP
#define LIBVIEW_2P_TILE_POOL_HPP

#include "tile.hpp"
#include "animations.hpp"
#include "geometry.hpp"
#include <libview_2p/data_types.hpp>
#include <libsdl.hpp>
#include <vector>
#include <array>

namespace libview_2p
{

class tile_pool
{
    private:
        using tile_array = std::array<std::array<std::unique_ptr<tile>, 2>, 6>;

    public:
        tile_pool(SDL_Renderer& renderer);

        int get_logical_width() const;

        int get_logical_height() const;

        void draw(SDL_Renderer& renderer, const geometry::system& sys);

        void set_tiles(const data_types::tile_pool& tiles);

        void clear();

    private:
        SDL_Renderer& renderer_;
        tile_array tiles_;
};

} //namespace libview_2p

#endif
