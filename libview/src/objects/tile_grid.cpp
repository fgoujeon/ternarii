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

#include "tile_grid.hpp"
#include "tile_grid_detail/common.hpp"
#include "number_tile.hpp"
#include "granite_tile.hpp"
#include "sdf_image.hpp"
#include "../colors.hpp"
#include <libres.hpp>
#include <libutil/log.hpp>
#include <libutil/matrix.hpp>
#include <libutil/tree.hpp>
#include <libutil/overload.hpp>
#include <queue>
#include <map>

namespace libview::objects
{

namespace
{
    using matrix_coordinate_tree = libutil::tree<libutil::matrix_coordinate>;

    Magnum::Vector2 tile_coordinate_to_position(const libutil::matrix_coordinate& c)
    {
        return
        {
            -2.5f + c.col,
            -5.0f + c.row
        };
    }

    /*
    Make a tree containing the coordinates of the merged tiles. Edges represent
    the paths to the destination position.

    For example, with the following board (where tile A to F are identical
    number tiles)...:

        | A    |
        |BCD   |
        |xxEF  |
        --------

    ... the corresponding coordinate tree is the following:

            E
           / \
          D   F
          |
          C
         / \
        A   B
    */
    std::unique_ptr<matrix_coordinate_tree> make_merge_tree
    (
        const libutil::matrix_coordinate_list& src_tile_coordinates,
        const libutil::matrix_coordinate& dst_tile_coordinate
    )
    {
        struct coordinate_info
        {
            libutil::matrix_coordinate coordinate;
            matrix_coordinate_tree* pparent_node = nullptr;
        };

        auto ptree = std::make_unique<matrix_coordinate_tree>();

        //Breadth-first traversal
        auto coordinate_queue = std::queue<coordinate_info>{};
        auto explored_coordinates = std::vector<libutil::matrix_coordinate>{};
        explored_coordinates.push_back(dst_tile_coordinate);
        coordinate_queue.push(coordinate_info{dst_tile_coordinate, nullptr});
        while(!coordinate_queue.empty())
        {
            const auto tile_coordinate_info = coordinate_queue.front();
            const auto& current_tile_coordinate = tile_coordinate_info.coordinate;
            const auto pparent_node = tile_coordinate_info.pparent_node;
            coordinate_queue.pop();

            //Add coordinate to merge tree
            matrix_coordinate_tree* pcurrent_node;
            if(tile_coordinate_info.pparent_node == nullptr)
            {
                ptree->set_value(current_tile_coordinate);
                pcurrent_node = ptree.get();
            }
            else
            {
                pcurrent_node = &pparent_node->add_child(current_tile_coordinate);
            }

            const auto neighbor_tile_coordinates =
                std::array
                {
                    libutil::matrix_coordinate{current_tile_coordinate.col,     current_tile_coordinate.row + 1}, //above
                    libutil::matrix_coordinate{current_tile_coordinate.col + 1, current_tile_coordinate.row},     //right
                    libutil::matrix_coordinate{current_tile_coordinate.col,     current_tile_coordinate.row - 1}, //below
                    libutil::matrix_coordinate{current_tile_coordinate.col - 1, current_tile_coordinate.row}      //left
                }
            ;

            //Explore neighbor coordinates
            for(const auto& neighbor_tile_coordinate: neighbor_tile_coordinates)
            {
                const auto is_src_tile_coordinate = std::find
                (
                    src_tile_coordinates.begin(),
                    src_tile_coordinates.end(),
                    neighbor_tile_coordinate
                ) != src_tile_coordinates.end();

                if
                (
                    is_src_tile_coordinate &&
                    std::find
                    (
                        explored_coordinates.begin(),
                        explored_coordinates.end(),
                        neighbor_tile_coordinate
                    ) == explored_coordinates.end()
                )
                {
                    explored_coordinates.push_back(neighbor_tile_coordinate);
                    coordinate_queue.push
                    (
                        coordinate_info
                        {
                            neighbor_tile_coordinate,
                            pcurrent_node
                        }
                    );
                }
            }
        }

        return ptree;
    }

    constexpr auto tile_scaling_factor = 0.46f;
}

tile_grid::tile_grid
(
    object2d& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    animation::animator& animator,
    const drop_request_callback& drop_cb,
    const input_layout_change_callback& layout_cb
):
    object2d{&parent},
    features::animable(*this, &animables),
    drawables_(drawables),
    animables_(animables),
    animator_(animator),
    next_input_(*this, drawables, animables),
    input_(*this, animables, drop_cb, layout_cb)
{
    //board corners
    {
        auto add_corner = [&](const float x, const float y, const auto rotation)
        {
            auto pobj = std::make_unique<sdf_image>
            (
                *this,
                drawables,
                libres::images::board_corner,
                sdf_image::style
                {
                    .color = colors::light_gray,
                    .outline_color = colors::dark_gray,
                    .outline_range = {0.5f, 0.3f}
                }
            );
            pobj->rotate(rotation);
            pobj->scale({0.50f, 0.50f});
            pobj->translate({x, y});

            board_corners_.push_back(std::move(pobj));
        };

        const auto shift  = 0.3f;
        const auto right  = 2.5f + shift;
        const auto left   = -right;
        const auto top    = 1.0f + shift;
        const auto bottom = -5.0f - shift;

        add_corner(right, top,    180.0_degf);
        add_corner(right, bottom,  90.0_degf);
        add_corner(left,  bottom,   0.0_degf);
        add_corner(left,  top,    -90.0_degf);
    }

    next_input_.setTranslation({0.0f, 5.0f});
    input_.setTranslation({0.0f, 3.0f});
}

bool tile_grid::is_animating() const
{
    return animator_.is_animating();
}

const data_types::input_layout& tile_grid::get_input_layout() const
{
    return input_.get_layout();
}

void tile_grid::clear()
{
    next_input_.release_tile_objects();
    input_.release_tile_objects();

    for(auto& ptile: board_tiles_)
    {
        ptile.reset();
    }
}

void tile_grid::create_next_input(const data_types::input_tile_matrix& tiles)
{
    next_input_.create_tiles(tiles);
}

void tile_grid::insert_next_input()
{
    input_.set_tiles(next_input_.release_tile_objects());
}

void tile_grid::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    auto anim = animation::animation{};

    //Retrieve tile objects
    const auto input_tile_objects = input_.release_tile_objects();
    for(auto& ptile: input_tile_objects)
    {
        if(ptile)
        {
            ptile->setParentKeepTransformation(this);
        }
    }

    for(const auto& drop: drops)
    {
        const auto& ptile = at(input_tile_objects, drop.input_coordinate);

        const auto dst_position = tile_coordinate_to_position(drop.board_coordinate);

        anim.add
        (
            animation::tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                22
            }
        );

        at(board_tiles_, drop.board_coordinate) = ptile;
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(animation::tracks::pause{0.05});
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    auto anim = animation::animation{};

    for(const auto& drop: drops)
    {
        auto& ptile = at(board_tiles_, drop.col, drop.src_row);

        const auto dst_position = tile_coordinate_to_position(libutil::matrix_coordinate{drop.col, drop.dst_row});

        anim.add
        (
            animation::tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                15
            }
        );

        at(board_tiles_, drop.col, drop.dst_row) = ptile;
        ptile = nullptr;
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(animation::tracks::pause{0.05});
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::nullify_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates)
{
    auto anim0 = animation::animation{};
    auto anim1 = animation::animation{};

    for(const auto& coord: nullified_tile_coordinates)
    {
        auto& ptile = at(board_tiles_, coord);

        if(!ptile)
        {
            continue;
        }

        anim0.add(animation::tracks::alpha_transition{ptile, 1, 0});
        anim1.add(animation::tracks::alpha_transition{ptile, 0, 0.4});

        ptile = nullptr;
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim0));
    animator_.push(std::move(anim1));
    animator_.push(animation::tracks::pause{0.05});
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::merge_tiles
(
    const data_types::tile_merge_list& merges,
    const data_types::granite_erosion_list& granite_erosions
)
{
    auto animations = std::map<int, animation::animation>{};

    constexpr auto track_duration_s = 0.17f;

    for(const auto& granite_erosion: granite_erosions)
    {
        auto& ptile = at(board_tiles_, granite_erosion.coordinate);

        animations[0].add
        (
            animation::tracks::alpha_transition
            {
                .pobj = ptile,
                .finish_alpha = 0,
                .duration_s = track_duration_s
            }
        );

        if(granite_erosion.new_thickness <= 0)
        {
            ptile = nullptr;
        }
        else
        {
            const auto dst_position = tile_coordinate_to_position(granite_erosion.coordinate);
            ptile = make_tile
            (
                data_types::tiles::granite{granite_erosion.new_thickness},
                dst_position
            );
            ptile->set_alpha(0);

            animations[0].add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = ptile,
                    .finish_alpha = 1,
                    .duration_s = track_duration_s
                }
            );
        }
    }

    for(const auto& merge: merges)
    {
        const auto pmerge_tree = make_merge_tree
        (
            merge.src_tile_coordinates,
            merge.dst_tile_coordinate
        );

        //translate all src tiles but the last one (the one at dst position)
        const auto tree_height = get_height(*pmerge_tree);
        for(auto i = tree_height; i > 0; --i)
        {
            const auto animation_index = tree_height - i;

            libutil::for_each_node
            (
                *pmerge_tree,
                [&](const matrix_coordinate_tree& node)
                {
                    if(get_depth(node) == i)
                    {
                        const auto& src_coordinate = node.get_value();
                        const auto& dst_coordinate = node.get_parent()->get_value();
                        const auto dst_position = tile_coordinate_to_position(dst_coordinate);

                        auto& ptile = at(board_tiles_, src_coordinate);

                        //translate tile toward position of parent tile
                        animations[animation_index].add
                        (
                            animation::tracks::fixed_duration_translation
                            {
                                .pobj = ptile,
                                .finish_position = dst_position,
                                .duration_s = track_duration_s
                            }
                        );

                        //also, make it disappear with a fade out
                        animations[animation_index].add
                        (
                            animation::tracks::alpha_transition
                            {
                                .pobj = ptile,
                                .finish_alpha = 0,
                                .duration_s = track_duration_s,
                                .interpolator = animation::get_exponential_in_float_interpolator()
                            }
                        );

                        //remove the tile object from the matrix so that it is
                        //deleted once the animation ends
                        ptile = nullptr;
                    }
                }
            );
        }

        //make last tile disappear with a fade out
        {
            auto& ptile = at(board_tiles_, merge.dst_tile_coordinate);
            animations[tree_height].add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = ptile,
                    .finish_alpha = 0,
                    .duration_s = track_duration_s
                }
            );
        }

        const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

        //create destination tile
        auto pdst_tile = make_tile(data_types::tiles::number{merge.dst_tile_value}, dst_position);
        pdst_tile->set_alpha(0);
        pdst_tile->setScaling({tile_scaling_factor / 4, tile_scaling_factor / 4});
        at(board_tiles_, merge.dst_tile_coordinate) = pdst_tile;

        //make destination tile appear with a fade in...
        animations[tree_height].add
        (
            animation::tracks::alpha_transition
            {
                .pobj = pdst_tile,
                .finish_alpha = 1,
                .duration_s = track_duration_s,
                .interpolator = animation::get_exponential_out_float_interpolator()
            }
        );

        //... and a "pop"
        animations[tree_height].add
        (
            animation::tracks::scaling_transition
            {
                .pobj = pdst_tile,
                .finish_scaling = {tile_scaling_factor, tile_scaling_factor},
                .duration_s = track_duration_s,
                .interpolator = animation::get_back_out_vector2_interpolator()
            }
        );
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    for(auto& [index, anim]: animations)
        animator_.push(std::move(anim));
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::change_tiles_value
(
    const libutil::matrix_coordinate& nullified_tile_coordinate,
    const data_types::tile_value_change_list& changes
)
{
    constexpr auto track_duration_s = 0.4f;

    auto anim = animation::animation{};

    //Make nullified tile disappear
    {
        auto& ptile = at(board_tiles_, nullified_tile_coordinate);
        if(ptile)
        {
            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = ptile,
                    .finish_alpha = 0,
                    .duration_s = track_duration_s
                }
            );
            ptile = nullptr;
        }
    }

    //Make changed tiles disappear then reappear with new value
    for(const auto& change: changes)
    {
        auto& ptile = at(board_tiles_, change.coordinate);

        //Disappear
        if(ptile)
        {
            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = ptile,
                    .finish_alpha = 0,
                    .duration_s = track_duration_s
                }
            );
            ptile = nullptr;
        }

        //Reappear
        {
            const auto dst_position = tile_coordinate_to_position(change.coordinate);
            auto pnew_tile = make_tile(data_types::tiles::number{change.new_value}, dst_position);
            pnew_tile->set_alpha(0);
            pnew_tile->setScaling({tile_scaling_factor, tile_scaling_factor});
            at(board_tiles_, change.coordinate) = pnew_tile;

            anim.add
            (
                animation::tracks::alpha_transition
                {
                    .pobj = pnew_tile,
                    .finish_alpha = 1,
                    .duration_s = track_duration_s
                }
            );
        }
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::mark_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates)
{
    auto anim = animation::animation{};

    //Unmark all board tiles
    for(auto& ptile: board_tiles_)
    {
        if(ptile)
        {
            anim.add(animation::tracks::alpha_transition{ptile, 1, 0});
        }
    }

    //Mark given board tiles
    for(const auto& coords: tile_coordinates)
    {
        auto& ptile = at(board_tiles_, coords);
        if(ptile)
        {
            anim.add(animation::tracks::alpha_transition{ptile, 0.5, 0});
        }
    }

    animator_.push(std::move(anim));
}

void tile_grid::mark_tiles_for_addition(const data_types::tile_value_change_list& changes)
{
    auto anim = animation::animation{};

    //Unmark all tiles
    for(const auto& ptile: addition_preview_tiles_)
    {
        anim.add(animation::tracks::alpha_transition{ptile, 0, 0});
    }
    addition_preview_tiles_.clear();

    //Mark given tiles
    for(const auto& change: changes)
    {
        const auto position = tile_coordinate_to_position(change.coordinate);
        auto ptile = make_preview_tile
        (
            data_types::tiles::adder{change.value_diff},
            position
        );
        ptile->set_alpha(0.0f);
        anim.add(animation::tracks::alpha_transition{ptile, 0.7, 0});
        addition_preview_tiles_.push_back(ptile);
    }

    animator_.push(std::move(anim));
}

void tile_grid::set_board_tiles(const data_types::board_tile_matrix& tiles)
{
    libutil::for_each_colrow
    (
        [&](const auto& opt_tile, const int col, const int row)
        {
            if(!opt_tile.has_value())
            {
                return;
            }

            const auto position = tile_coordinate_to_position({col, row});

            auto ptile = make_tile(opt_tile.value(), position);
            at(board_tiles_, col, row) = ptile;
        },
        tiles
    );
}

void tile_grid::advance(const std::chrono::steady_clock::time_point& now, const float /*elapsed_s*/)
{
    animator_.advance(now);
}

void tile_grid::handle_button_press(data_types::move_button button)
{
    input_.handle_button_press(button);
}

void tile_grid::handle_button_release(data_types::move_button button)
{
    input_.handle_button_release(button);
}

std::shared_ptr<object2d> tile_grid::make_tile
(
    const data_types::tile& tile,
    const Magnum::Vector2& position
)
{
    auto ptile = tile_grid_detail::make_tile_object(*this, drawables_, animables_, tile);
    ptile->setScaling({tile_scaling_factor, tile_scaling_factor});
    ptile->setTranslation(position);
    return ptile;
}

std::shared_ptr<object2d> tile_grid::make_preview_tile
(
    const data_types::tile& tile,
    const Magnum::Vector2& position
)
{
    auto ptile = tile_grid_detail::make_preview_tile_object(*this, drawables_, animables_, tile);
    ptile->setScaling({tile_scaling_factor, tile_scaling_factor});
    ptile->setTranslation(position);
    return ptile;
}

} //namespace
