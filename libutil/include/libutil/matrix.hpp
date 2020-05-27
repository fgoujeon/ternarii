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

#ifndef LIBUTIL_MATRIX_HPP
#define LIBUTIL_MATRIX_HPP

#include <array>
#include <cassert>

namespace libutil
{

template<typename T, size_t M, size_t N>
struct matrix
{
    static constexpr int m = M;
    static constexpr int n = N;
    std::array<T, M * N> data;
};

struct matrix_coordinate
{
    int i = 0;
    int j = 0;
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
decltype(auto) at(Matrix& mat, const int ij)
{
    assert(ij < mat.m * mat.n);
    return mat.data[ij];
}

template<class Matrix>
decltype(auto) at(Matrix& mat, const int i, const int j)
{
    assert(i < mat.m && j < mat.n);
    return mat.data[i * mat.n + j];
}

template<class Matrix>
decltype(auto) at(Matrix& mat, const matrix_coordinate& c)
{
    return at(mat, c.i, c.j);
}

template<class Matrix, class Matrix2>
constexpr bool have_same_size()
{
    return Matrix::m == Matrix2::m && Matrix::n == Matrix2::n;
}

//For each element matN_ij of each given matrix,
//call f(mat0_ij, mat1_ij, ..., matN_ij).
template<typename F, class Matrix, class... Matrices>
void for_each(F&& f, Matrix& mat, Matrices&... mats)
{
    static_assert((have_same_size<Matrix, Matrices>() && ...));

    for(auto ij = 0; ij < mat.m * mat.n; ++ij)
    {
        f(at(mat, ij), at(mats, ij)...);
    }
}

//For each element matN_ij of each given matrix,
//call f(mat0_ij, mat1_ij, ..., matN_ij, i, j).
template<typename F, class Matrix, class... Matrices>
void for_each_ij(F&& f, Matrix& mat, Matrices&... mats)
{
    for(auto i = 0; i < mat.m; ++i)
    {
        for(auto j = 0; j < mat.n; ++j)
        {
            f
            (
                at(mat, i, j),
                at(mats, i, j)...,
                i,
                j
            );
        }
    }
}

} //namespace

#endif
