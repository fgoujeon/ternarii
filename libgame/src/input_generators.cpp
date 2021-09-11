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

#include "input_generators.hpp"
#include <memory>
#include <random>

namespace libgame
{

namespace
{
    /*
    A number generator that "generates" the number given at construction.
    Satisfies UniformRandomBitGenerator requirements.
    */
    struct uint16_generator
    {
        using result_type = uint16_t;

        static constexpr result_type min()
        {
            return 0;
        }

        static constexpr result_type max()
        {
            return 0xffff;
        }

        result_type operator()() const
        {
            return n;
        }

        result_type n = 0;
    };

    struct abstract_input_subgenerator
    {
        virtual ~abstract_input_subgenerator() = default;

        virtual data_types::input_tile_matrix generate
        (
            const int max,
            const double standard_deviation,
            const uint64_t random
        ) = 0;
    };

    /*
    Return a number tile whose random value goes from 0 to max, following a
    normal distribution.
    */
    data_types::tiles::number generate_random_number_tile
    (
        const int max,
        const double standard_deviation,
        const uint16_t random
    )
    {
        /*
        Generate a random number with normal distribution.

        We can't use std::normal_distribution because it needs an undefined
        amount of random numbers, and we only have one.

        Interval of x and y is [1/256, 1]. We don't want 0 to avoid
        log(0) = inf. That's why we must add 1.
        */
        constexpr auto pi = 3.1415926535897932384626433;
        const auto x = ((random & 0xff) + 1.0) / 256.0; //[1/256, 1]
        const auto y = ((random >> 8) + 1.0) / 256.0; //[1/256, 1]
        const auto real_val =
            sqrt(-2 * log(x)) * cos(2 * pi * y) * standard_deviation
        ;

        //remove negative values
        const auto positive_real_val = std::abs(real_val);

        //discretize
        const auto natural_val = static_cast<int>(positive_real_val);

        //stay inside [0, max]
        const auto final_val = natural_val % (max + 1);

        assert(final_val >= 0);

        return data_types::tiles::number{final_val};
    }



    class random_granite_tile_generator
    {
        public:
            data_types::tiles::granite generate(const uint16_t random)
            {
                auto engine = uint16_generator{random};
                return data_types::tiles::granite
                {
                    dis_(engine) + 1
                };
            }

        private:
            std::discrete_distribution<int> dis_{13, 17, 10};
    };



    //"Generates" the tiles given at construction
    class simple_input_generator: public abstract_input_subgenerator
    {
        public:
            simple_input_generator(const data_types::input_tile_matrix& tiles):
                tiles_(tiles)
            {
            }

            data_types::input_tile_matrix generate
            (
                const int /*max*/,
                const double /*standard_deviation*/,
                const uint64_t /*random*/
            ) override
            {
                return tiles_;
            }

        private:
            data_types::input_tile_matrix tiles_;
    };

    template<class Tile>
    abstract_input_subgenerator& get_simple_input_generator()
    {
        const auto input = data_types::input_tile_matrix{Tile{}};
        static auto generator = simple_input_generator{input};
        return generator;
    }



    class random_number_tile_pair_generator: public abstract_input_subgenerator
    {
        public:
            data_types::input_tile_matrix generate
            (
                const int max,
                const double standard_deviation,
                const uint64_t random
            ) override
            {
                const auto tile0_rand = random;
                const auto tile1_rand = random >> 16;

                return
                {
                    generate_random_number_tile(max, standard_deviation, tile0_rand),
                    std::nullopt,
                    generate_random_number_tile(max, standard_deviation, tile1_rand),
                    std::nullopt
                };
            }
    };

    abstract_input_subgenerator& get_random_number_tile_pair_generator()
    {
        static auto generator = random_number_tile_pair_generator{};
        return generator;
    }



    class random_number_tile_triple_generator: public abstract_input_subgenerator
    {
        public:
            data_types::input_tile_matrix generate
            (
                const int max,
                const double standard_deviation,
                const uint64_t random
            ) override
            {
                const auto tile0_rand = random;
                const auto tile1_rand = random >> 16;
                const auto tile2_rand = random >> 32;

                return
                {
                    generate_random_number_tile(max, standard_deviation, tile0_rand),
                    generate_random_number_tile(max, standard_deviation, tile1_rand),
                    generate_random_number_tile(max, standard_deviation, tile2_rand),
                    std::nullopt
                };
            }
    };

    abstract_input_subgenerator& get_random_number_tile_triple_generator()
    {
        static auto generator = random_number_tile_triple_generator{};
        return generator;
    }



    class random_number_and_granite_tile_generator: public abstract_input_subgenerator
    {
        public:
            data_types::input_tile_matrix generate
            (
                const int max,
                const double standard_deviation,
                const uint64_t random
            ) override
            {
                const auto number_tile_rand = random;
                const auto granite_tile_rand = random >> 16;

                return
                {
                    generate_random_number_tile(max, standard_deviation, number_tile_rand),
                    std::nullopt,
                    granite_gen_.generate(granite_tile_rand),
                    std::nullopt
                };
            }

        private:
            random_granite_tile_generator granite_gen_;
    };

    abstract_input_subgenerator& get_random_number_and_granite_tile_generator()
    {
        static auto generator = random_number_and_granite_tile_generator{};
        return generator;
    }
}

/*
random_input_generator
Higher-order random input generator.
*/
namespace
{
    struct weighted_input_subgenerator_ref
    {
        abstract_input_subgenerator& generator;
        double weight = 1;
    };

    using weighted_input_subgenerator_ref_list = std::vector<weighted_input_subgenerator_ref>;

    std::vector<double> get_weigths(const weighted_input_subgenerator_ref_list& generators)
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
            random_input_generator(const weighted_input_subgenerator_ref_list& subgenerators):
                subgenerators_(std::move(subgenerators)),
                weights_(get_weigths(subgenerators_)),
                dis_(weights_.begin(), weights_.end())
            {
            }

            data_types::input_tile_matrix generate
            (
                const int board_highest_tile_value,
                const int board_tile_count,
                const uint64_t random
            ) override
            {
                /*
                We want to generate tiles whose value goes from 0 to the value
                of the highest tile of the board.
                This max value is clamped between 2 (we don't want only 0s at
                the beginning of a game) and 9 (we don't want the player to get
                too many "free" points from the input just by being lucky).
                */
                const auto max_value = std::clamp(board_highest_tile_value, 2, 9);

                /*
                Compute the standard deviation (SD) of the normal distribution
                (whose average is set to 0).
                The lower the SD is, the more likely the generated value is
                small.
                We want to adjust the SD according to the fill rate of the
                board:
                - When the board is near empty, the SD is high so that the
                player can quickly grow his tiles.
                - When the board is near full, the SD is low so that the player
                gets the low-value tiles he might be waiting for to unlock his
                combos.
                */
                const auto standard_deviation = [&]
                {
                    /*
                    Normalized fill rate of the board.
                    Value goes from 0.0 (empty) to 1.0 (full).
                    */
                    const auto fill_rate =
                        static_cast<double>(board_tile_count) /
                        constants::board_authorized_cell_count
                    ;

                    const auto sd_max = 4.0; //SD of empty board
                    const auto sd_min = 1.8; //SD of full board
                    const auto sd_variable_part = sd_max - sd_min;

                    /*
                    The higher the exponent is, the "longer" the SD stays at max value.
                    We only want the SD to significantly decrease when the board is 3/4
                    full.
                    */
                    const auto fill_rate_pow = std::pow(fill_rate, 6);

                    return sd_min + sd_variable_part * (1.0 - fill_rate_pow);
                }();

                //Use the first 16 bits for this level, the rest for the others
                const auto random0 = static_cast<uint16_t>(random);
                const auto random1 = random >> 16;

                auto engine = uint16_generator{random0};
                const auto r = dis_(engine);
                return subgenerators_[r].generator.generate
                (
                    max_value,
                    standard_deviation,
                    random1
                );
            }

        private:
            weighted_input_subgenerator_ref_list subgenerators_;
            std::vector<double> weights_;
            std::discrete_distribution<int> dis_;
    };
}

//Top-level generators
namespace
{
    abstract_input_generator& get_purity_chapel_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(), 1}
            }
        );
        return generator;
    }

    abstract_input_generator& get_nullifier_room_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(), 5000},
                {get_simple_input_generator<data_types::tiles::column_nullifier>(), 15},
                {get_simple_input_generator<data_types::tiles::row_nullifier>(), 30},
                {get_simple_input_generator<data_types::tiles::number_nullifier>(), 20},
            }
        );
        return generator;
    }

    abstract_input_generator& get_triplet_pines_mall_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(), 3700},
                {get_random_number_tile_triple_generator(), 1300},
                {get_simple_input_generator<data_types::tiles::column_nullifier>(), 22},
                {get_simple_input_generator<data_types::tiles::row_nullifier>(), 37},
                {get_simple_input_generator<data_types::tiles::number_nullifier>(), 17},
            }
        );
        return generator;
    }

    abstract_input_generator& get_granite_cave_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(), 4000},
                {get_random_number_and_granite_tile_generator(), 1000},
                {get_simple_input_generator<data_types::tiles::column_nullifier>(), 45},
                {get_simple_input_generator<data_types::tiles::row_nullifier>(), 30},
                {get_simple_input_generator<data_types::tiles::number_nullifier>(), 10},
            }
        );
        return generator;
    }
}

abstract_input_generator& get_input_generator(data_types::stage stage)
{
#define CASE(STAGE) \
    case data_types::stage::STAGE: \
        return get_##STAGE##_input_generator();

    switch(stage)
    {
        default:
        CASE(purity_chapel);
        CASE(nullifier_room);
        CASE(triplet_pines_mall);
        CASE(granite_cave);
    }

#undef CASE
}

} //namespace
