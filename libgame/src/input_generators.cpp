/*
Copyright 2018 - 2022 Florian Goujeon

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
    struct abstract_input_subgenerator
    {
        virtual ~abstract_input_subgenerator() = default;

        virtual data_types::input_tile_matrix generate
        (
            const int max,
            const double standard_deviation
        ) = 0;
    };



    class random_number_tile_generator
    {
        private:
            using distribution = std::normal_distribution<double>;

        public:
            //return random value from 0 to max
            data_types::tiles::number generate
            (
                const int max,
                const double standard_deviation
            )
            {
                //generate random number with normal distribution
                const auto real_val = dis_
                (
                    rng_.engine,
                    distribution::param_type{0, standard_deviation}
                );

                //remove negative values
                const auto positive_real_val = std::abs(real_val);

                //discretize
                const auto natural_val = static_cast<int>(positive_real_val);

                //stay inside [0, max]
                const auto final_val = natural_val % (max + 1);

                return data_types::tiles::number{final_val};
            }

        private:
            libutil::rng rng_;
            distribution dis_;
    };



    class random_granite_tile_generator
    {
        public:
            data_types::tiles::granite generate()
            {
                return data_types::tiles::granite{dis_(rng_.engine) + 1};
            }

        private:
            libutil::rng rng_;
            std::discrete_distribution<int> dis_{13, 17, 10};
    };



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
                const double /*standard_deviation*/
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

    template<int value>
    abstract_input_subgenerator& get_adder_generator()
    {
        const auto input = data_types::input_tile_matrix{data_types::tiles::adder{value}};
        static auto generator = simple_input_generator{input};
        return generator;
    }



    class random_number_tile_pair_generator: public abstract_input_subgenerator
    {
        public:
            data_types::input_tile_matrix generate
            (
                const int max,
                const double standard_deviation
            ) override
            {
                return
                {
                    gen_.generate(max, standard_deviation),
                    std::nullopt,
                    gen_.generate(max, standard_deviation),
                    std::nullopt
                };
            }

        private:
            random_number_tile_generator gen_;
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
                const double standard_deviation
            ) override
            {
                return
                {
                    gen_.generate(max, standard_deviation),
                    gen_.generate(max, standard_deviation),
                    gen_.generate(max, standard_deviation),
                    std::nullopt
                };
            }

        private:
            random_number_tile_generator gen_;
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
                const double standard_deviation
            ) override
            {
                return
                {
                    number_gen_.generate(max, standard_deviation),
                    std::nullopt,
                    granite_gen_.generate(),
                    std::nullopt
                };
            }

        private:
            random_number_tile_generator number_gen_;
            random_granite_tile_generator granite_gen_;
    };

    abstract_input_subgenerator& get_random_number_and_granite_tile_generator()
    {
        static auto generator = random_number_and_granite_tile_generator{};
        return generator;
    }



    /*
    random_input_generator

    Higher-order random input generator.
    */

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
                const int board_tile_count
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

                const auto r = dis_(rng_.engine);
                return subgenerators_[r].generator.generate
                (
                    max_value,
                    standard_deviation
                );
            }

        private:
            libutil::rng rng_;
            weighted_input_subgenerator_ref_list subgenerators_;
            std::vector<double> weights_;
            std::discrete_distribution<int> dis_;
    };



    /*
    Top-level generators
    */

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

    abstract_input_generator& get_math_classroom_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(), 5000},
                {get_adder_generator<-2>(),               30},
                {get_adder_generator<-1>(),               30},
                {get_adder_generator<1>(),                70},
                {get_adder_generator<2>(),                70}
            }
        );
        return generator;
    }

    abstract_input_generator& get_waterfalls_input_generator()
    {
        static auto generator = random_input_generator
        (
            {
                {get_random_number_tile_pair_generator(), 5000},
                {get_simple_input_generator<data_types::tiles::column_nullifier>(), 15},
                {get_simple_input_generator<data_types::tiles::row_nullifier>(), 30},
                {get_simple_input_generator<data_types::tiles::number_nullifier>(), 10},
                {get_simple_input_generator<data_types::tiles::outer_columns_nullifier>(), 45}
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
        CASE(math_classroom);
        CASE(waterfalls);
    }

#undef CASE
}

} //namespace
