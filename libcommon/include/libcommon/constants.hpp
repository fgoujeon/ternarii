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

#ifndef LIBCOMMON_CONSTANTS_HPP
#define LIBCOMMON_CONSTANTS_HPP

namespace libcommon::constants
{

//Dimensions of the zone, starting from the bottom of the board, that can
//take tiles without causing game over
constexpr auto board_authorized_column_count = 6;
constexpr auto board_authorized_row_count = 7;
constexpr auto board_authorized_cell_count = board_authorized_column_count * board_authorized_row_count;

constexpr auto input_column_count = 2;
constexpr auto input_row_count = 2;

constexpr auto board_column_count = board_authorized_column_count;
constexpr auto board_row_count = board_authorized_row_count + input_row_count; //We need extra space for the input.

} //namespace

#endif
