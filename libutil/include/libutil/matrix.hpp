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

#ifndef LIBUTIL_MATRIX_HPP
#define LIBUTIL_MATRIX_HPP

#include <array>
#include <cassert>

namespace libutil
{

template<typename T, int Cols, int Rows>
struct matrix
{
    static_assert(Cols > 0);
    static_assert(Rows > 0);

    static constexpr auto cols = Cols;
    static constexpr auto rows = Rows;
    static constexpr auto size = Cols * Rows;

    std::array<T, size> data;
};

struct matrix_coordinate
{
    int col = 0;
    int row = 0;

    bool operator==(const matrix_coordinate&) const = default;
};

using matrix_coordinate_list = std::vector<matrix_coordinate>;

template<class Matrix>
auto begin(Matrix& mat)
{
    return mat.data.begin();
}

template<class Matrix>
auto end(Matrix& mat)
{
    return mat.data.end();
}

template<class Matrix>
decltype(auto) at(Matrix& mat, const int i)
{
    assert(i < mat.size);
    return mat.data[i];
}

template<class Matrix>
decltype(auto) at(Matrix& mat, const int col, const int row)
{
    assert(col < mat.cols && row < mat.rows);
    return mat.data[col * mat.rows + row];
}

template<class Matrix>
decltype(auto) at(Matrix& mat, const matrix_coordinate& c)
{
    return at(mat, c.col, c.row);
}

template<class Matrix, class Matrix2>
constexpr bool have_same_size()
{
    return Matrix::size == Matrix2::size;
}

//For each element matN_ji of each given matrix,
//call f(mat0_ji, mat1_ji, ..., matN_ji).
template<typename F, class Matrix, class... Matrices>
void for_each(F&& f, Matrix& mat, Matrices&... mats)
{
    static_assert((have_same_size<Matrix, Matrices>() && ...));

    for(auto i = 0; i < mat.size; ++i)
    {
        f(at(mat, i), at(mats, i)...);
    }
}

//For each element matN_colrow of each given matrix,
//call f(mat0_colrow, mat1_colrow, ..., matN_colrow, col, row).
template<typename F, class Matrix, class... Matrices>
void for_each_colrow(F&& f, Matrix& mat, Matrices&... mats)
{
    for(auto col = 0; col < mat.cols; ++col)
    {
        for(auto row = 0; row < mat.rows; ++row)
        {
            f
            (
                at(mat, col, row),
                at(mats, col, row)...,
                col,
                row
            );
        }
    }
}

} //namespace

#endif
