/*
Copyright 2018 - 2021 Florian Goujeon

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

#ifndef LIBVIEW_SRC_DATA_TYPES_HPP
#define LIBVIEW_SRC_DATA_TYPES_HPP

#include <libview/data_types.hpp>
#include <filesystem>
#include <optional>

namespace libview::data_types
{

std::string_view get_pretty_name(const stage s);

std::optional<std::filesystem::path> get_image(const stage s);

} //namespace

#endif
