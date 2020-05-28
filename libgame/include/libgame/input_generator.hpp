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

#ifndef LIBGAME_INPUT_GENERATOR_HPP
#define LIBGAME_INPUT_GENERATOR_HPP

#include "data_types.hpp"
#include <libutil/rng.hpp>

namespace libgame
{

struct abstract_input_generator
{
    virtual ~abstract_input_generator() = default;

    virtual data_types::input_tile_matrix generate(const int max, const double standard_deviation) = 0;
};

class random_tile_generator
{
    private:
        using distribution = std::normal_distribution<double>;

    public:
        random_tile_generator(libutil::rng& rng):
            rng_(rng)
        {
        }

        //return random value from 0 to max
        int generate(const int max, const double standard_deviation)
        {
            //generate random number with normal distribution
            const auto real_val = dis_(rng_.engine, distribution::param_type{0, standard_deviation});

            //remove negative values
            const auto positive_real_val = std::abs(real_val);

            //discretize
            const auto natural_val = static_cast<int>(positive_real_val);

            //stay inside [0, max]
            return natural_val % (max + 1);
        }

    private:
        libutil::rng& rng_;
        distribution dis_;
};

class random_input_generator: public abstract_input_generator
{
    private:
        using distribution = std::discrete_distribution<int>;

    public:
        data_types::input_tile_matrix generate(const int max, const double standard_deviation)
        {
            const auto count = dis_(rng_.engine);

            switch(count)
            {
                default:
                case 0:
                    return
                    {
                        data_types::number_tile{gen_.generate(max, standard_deviation)},
                        data_types::number_tile{gen_.generate(max, standard_deviation)}
                    };
                case 1: return {data_types::column_nullifier_tile{}};
                case 2: return {data_types::row_nullifier_tile{}};
                case 3: return {data_types::number_nullifier_tile{}};
            }
        }

    private:
        libutil::rng rng_;
        distribution dis_{300, 1, 1, 1};
        random_tile_generator gen_{rng_};
};

} //namespace

#endif
