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

#include "input_generators.hpp"
#include <libutil/rng.hpp>
#include <memory>

namespace libgame
{

namespace
{
    class random_number_tile_generator
    {
        private:
            using distribution = std::normal_distribution<double>;

        public:
            //return random value from 0 to max
            data_types::number_tile generate(const int max, const double standard_deviation)
            {
                //generate random number with normal distribution
                const auto real_val = dis_(rng_.engine, distribution::param_type{0, standard_deviation});

                //remove negative values
                const auto positive_real_val = std::abs(real_val);

                //discretize
                const auto natural_val = static_cast<int>(positive_real_val);

                //stay inside [0, max]
                const auto final_val = natural_val % (max + 1);

                return data_types::number_tile{final_val};
            }

        private:
            libutil::rng rng_;
            distribution dis_;
    };



    class simple_input_generator: public abstract_input_generator
    {
        public:
            simple_input_generator(const data_types::input_tile_matrix& tiles):
                tiles_(tiles)
            {
            }

            data_types::input_tile_matrix generate(const int /*max*/, const double /*standard_deviation*/) override
            {
                return tiles_;
            }

        private:
            data_types::input_tile_matrix tiles_;
    };

    template<class Tile>
    abstract_input_generator& get_simple_input_generator()
    {
        const auto input = data_types::input_tile_matrix{Tile{}};
        static auto generator = simple_input_generator{input};
        return generator;
    }



    class random_number_tile_pair_generator: public abstract_input_generator
    {
        public:
            data_types::input_tile_matrix generate(const int max, const double standard_deviation) override
            {
                return
                {
                    gen_.generate(max, standard_deviation),
                    gen_.generate(max, standard_deviation)
                };
            }

        private:
            random_number_tile_generator gen_;
    };

    abstract_input_generator& get_random_number_tile_pair_generator()
    {
        static auto generator = random_number_tile_pair_generator{};
        return generator;
    }



    /*
    random_input_generator

    Higher-order random input generator.
    */

    struct weighted_input_generator_ref
    {
        abstract_input_generator& generator;
        double weight = 1;
    };

    using weighted_input_generator_ref_list = std::vector<weighted_input_generator_ref>;

    std::vector<double> get_weigths(const weighted_input_generator_ref_list& generators)
    {
        auto weights = std::vector<double>{};
        for(const auto& generator: generators)
        {
            weights.push_back(generator.weight);
        }
        return weights;
    }

    class random_input_generator: public abstract_input_generator
    {
        public:
            random_input_generator(const weighted_input_generator_ref_list& generators):
                generators_(std::move(generators)),
                weights_(get_weigths(generators_)),
                dis_(weights_.begin(), weights_.end())
            {
            }

            data_types::input_tile_matrix generate(const int max, const double standard_deviation) override
            {
                const auto r = dis_(rng_.engine);
                return generators_[r].generator.generate(max, standard_deviation);
            }

        private:
            libutil::rng rng_;
            weighted_input_generator_ref_list generators_;
            std::vector<double> weights_;
            std::discrete_distribution<int> dis_;
    };



    /*
    Top-level generators
    */

    abstract_input_generator& get_purity_room_input_generator()
    {
        return get_random_number_tile_pair_generator();
    }

    abstract_input_generator& get_nullifier_room_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(),                         300},
                {get_simple_input_generator<data_types::column_nullifier_tile>(), 1},
                {get_simple_input_generator<data_types::row_nullifier_tile>(),    1},
                {get_simple_input_generator<data_types::number_nullifier_tile>(), 1},
            }
        );
        return generator;
    }
}

abstract_input_generator& get_input_generator(data_types::stage stage)
{
    switch(stage)
    {
        default:
        case data_types::stage::purity_room:
            return get_purity_room_input_generator();
        case data_types::stage::nullifier_room:
            return get_nullifier_room_input_generator();
    }
}

} //namespace
